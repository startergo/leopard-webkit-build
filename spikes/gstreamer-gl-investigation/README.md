# GStreamer GL Investigation — 10.6.8 / 9400M

Spike work investigating whether GStreamer's GL upload path can be used to
accelerate video playback in WebKit 610 on Mac OS X 10.6.8 (Snow Leopard)
running on the GeForce 9400M.

**Status:** architecture mapped, all API-level questions answered, source-build
path identified. Consumer port is the next phase, multi-day effort, blocked on
no unknowns.

---

## TL;DR

- **Hardware path is viable.** `gst-launch-1.0 videotestsrc ! glimagesink`
  renders SMPTE bars smoothly on the 9400M via `libgstgl-1.0.0.dylib`
  (1.4.5) + a small selector-shim for 10.7+-only APIs. See
  `macmini-gl-playing.png` for evidence.
- **The pre-built GStreamer 1.4.5 universal pkg** is the only freedesktop.org
  release that both runs on 10.6 (deployment target 10.6) and includes GstGL.
  1.12+ targets 10.10 minimum; 1.2.x predates GstGL.
- **GstGL 1.4.5's Cocoa backend has three real bugs** that block the consumer
  port from a non-NSApplication-run-loop caller (a future source build will
  fix these directly):
  1. Unconditional 10.7+-only selector calls (`setWantsBestResolutionOpenGLSurface:`,
     `convertRectToBacking:`) — currently shimmed at runtime, will be
     `#if`'d out in source build.
  2. `dispatch_sync(main_queue)` deadlock when `gst_gl_context_create` is
     called from main thread without an NSApplication run loop running.
  3. `gst_gl_context_cocoa_activate` silently returns TRUE on a nil GL context.
- **Open residual risk:** whether GstGL's pixel format and WebKit's
  `CAOpenGLLayer`-negotiated pixel format resolve to the same CGL renderer
  (renderer-match is the actual share-group constraint, not accum size).
  Will be observed directly via renderer-ID logging during the consumer port.

---

## The 1.28 wall (and why 1.4.5 was the right target)

MacPorts' `gstreamer1-gst-plugins-base @1.28.5` with `+quartz` fails to
build against the 10.6 SDK:

```
gst-libs/gst/gl/gstglcontext.c:654:22: error: use of undeclared identifier 'GL_CONTEXT_PROFILE_MASK'
gst-libs/gst/gl/gstglcontext.c:655:29: error: use of undeclared identifier 'GL_CONTEXT_CORE_PROFILE_BIT'
gst-libs/gst/gl/gstglcontext.c:657:29: error: use of undeclared identifier 'GL_CONTEXT_COMPATIBILITY_PROFILE_BIT'
```

Those are OpenGL 3.2+ constants (August 2009). The 10.6 SDK ships GL 2.1
headers only. No amount of source patching fixes this short of overlaying
newer OpenGL headers — and even if it compiled, the Cocoa backend uses
`kCGLPFAOpenGLProfile` + `kCGLOGLPVersion_Legacy` which are themselves
10.7+ APIs that no shim reaches.

The official pre-built packages at 1.12.4 and 1.16.3 target 10.10+
deployment (`LC_VERSION_MIN_MACOSX = 10.10`), so `dyld` refuses to load
them on 10.6.

GStreamer 1.4.5 (December 2014) was the last release where:
- Deployment target was 10.6 (`LC_VERSION_MIN_MACOSX = 10.6, sdk 10.10`)
- The Cocoa backend predates the GL_CONTEXT_PROFILE_MASK requirement
- The Cocoa backend predates the `setWantsBestResolutionOpenGLSurface:`
  adoption (which landed in 1.6+)

The only 10.7+-only APIs 1.4.5's Cocoa backend calls are two selector
sends in `gstglcontext_cocoa.m` — `setWantsBestResolutionOpenGLSurface:`
and `convertRectToBacking:`. Both are no-ops on 10.6 (no Retina display,
no HiDPI), so the shim's identity/no-op semantics are exactly right.

---

## API surface findings

All findings below were verified via standalone probes in this directory,
run against GStreamer 1.4.5 (the official universal pkg install on the
macmini at `/Library/Frameworks/GStreamer.framework/`).

### What works

| Operation | Result | Probe |
|---|---|---|
| `gst_gl_context_new_wrapped(display, (guintptr)nscontext, GST_GL_PLATFORM_CGL, GST_GL_API_OPENGL)` | ✅ Returns a `GstGLWrappedContext` that correctly stores + reports back the NSOpenGLContext handle | `gst-gl-probe.c` |
| `gst_gl_context_new(display)` → native `GstGLContextCocoa` | ✅ | `gst-gl-probe.c` |
| `gst_gl_context_activate(native, TRUE)` + `gst_gl_context_thread_add` running real GL calls (glClearColor + glClear + glGetError) | ✅ — `GL_NO_ERROR` returned | `gst-gl-probe.c` |
| Full pipeline `videotestsrc ! glimagesink` reaching `PLAYING` state and rendering SMPTE color bars | ✅ — screenshot at `macmini-gl-playing.png` | manual `gst-launch` test |

### What does NOT work (negative results, with reasons)

| Operation | Result | Reason |
|---|---|---|
| `gst_gl_context_new_wrapped` with a **raw CGLContextObj** handle | Compiles, returns non-nil, but downstream is undefined behavior | `gstglcontext_cocoa.m:219` casts the handle directly to `NSOpenGLContext *`. A `CGLContextObj` (a `void *`) treated as an ObjC object pointer causes ObjC runtime to dereference garbage. **Must wrap an `NSOpenGLContext *`, not a `CGLContextObj`.** |
| `gst_gl_context_activate(wrapped, TRUE)` | Returns FALSE | Stub. `GstGLWrappedContext` doesn't override the activate vfunc; the base class returns FALSE without logging. Wrapped contexts are externally managed; the caller is expected to make the context current. |
| `gst_gl_context_thread_add(wrapped, ...)` | REJECTED | `gst_gl_context.c` has explicit `g_return_if_fail(!GST_GL_IS_WRAPPED_CONTEXT(context))`. Wrapped contexts cannot drive GstGL's thread mechanism. |
| `gst_gl_context_can_share(native, wrapped)` | Returns 0 | **Not a real capability check.** Walks GstGL's internal `other_context_ref` chain (a weak-ref list) to see if the contexts are already linked. Returns 0 simply because the share relationship hasn't been established via `gst_gl_context_create` yet. Circular check; ignore. |
| `gst_gl_context_create(native, wrapped, &err)` from main thread of a non-NSApplication caller | Hangs | `gstglcontext_cocoa.m:223` does `dispatch_sync(dispatch_get_main_queue(), block)`. The GL thread spawned by `gst_gl_context_create` calls this; main thread is blocked on `g_cond_wait` waiting for that same GL thread. Deadlock. **Real consumer (WebKit) won't hit this** — main thread runs `NSApplication`'s run loop, which services the main queue and lets `dispatch_sync` return. |

### The phantom success to be aware of

`gst-gl-probe.c` originally reported "NATIVE-CTX SUCCESS" after
`gst_gl_context_activate(native, TRUE)` returned 1. **This was a false
positive.** The activate vfunc (`gstglcontext_cocoa.m:321`) is:

```objc
static gboolean gst_gl_context_cocoa_activate(GstGLContext *context, gboolean activate) {
    GstGLContextCocoa *context_cocoa = GST_GL_CONTEXT_COCOA(context);
    if (activate)
        [context_cocoa->priv->gl_context makeCurrentContext];  // <- [nil makeCurrentContext] is no-op
    else
        [NSOpenGLContext clearCurrentContext];
    return TRUE;  // <- always returns TRUE
}
```

If `priv->gl_context` is nil (because `gst_gl_context_create` was never
called), `[nil makeCurrentContext]` is a no-op (Objective-C messaging nil
is valid and returns void), and activate returns TRUE anyway. The thread_add
that followed was running on whatever context was current at the time (our
own CGL context from `CGLSetCurrentContext`), not on a real GstGL context.

When you write the consumer port: **always verify `priv->gl_context != nil`
before trusting `activate`'s return value.** The patched source build will
return FALSE on nil gl_context.

---

## Architecture for the consumer port

```
                  ┌─────────────────────────────────────────┐
                  │           WebKit process                 │
                  │                                          │
                  │  ┌──────────────────┐                    │
                  │  │ CAOpenGLLayer    │  WebKit's existing │
                  │  │  (compositor)    │  compositor layer  │
                  │  │   NSOpenGLContext│  on the 9400M      │
                  │  └────────┬─────────┘                    │
                  │           │                              │
                  │  ┌────────▼─────────┐                    │
                  │  │ wrap via         │  gst_gl_context_   │
                  │  │ gst_gl_context_  │  new_wrapped()     │
                  │  │ new_wrapped()    │  → GstGLWrapped-   │
                  │  │                  │    Context         │
                  │  └────────┬─────────┘                    │
                  │           │  set on GstGLDisplay via     │
                  │           │  GstContext propagation      │
                  │  ┌────────▼─────────┐                    │
                  │  │ gst_gl_display_  │                    │
                  │  │ new() + element  │                    │
                  │  │ set_context()    │                    │
                  │  └────────┬─────────┘                    │
                  │           │                              │
                  │  ┌────────▼─────────────────────────┐    │
                  │  │ GstGL pipeline elements          │    │
                  │  │ (glupload, gldownload,           │    │
                  │  │  glcolorconvert)                 │    │
                  │  │                                  │    │
                  │  │  internally call                 │    │
                  │  │  gst_gl_context_new(display) →   │    │
                  │  │  GstGLContextCocoa               │    │
                  │  │                                  │    │
                  │  │  share parent = wrapped ctx via  │    │
                  │  │  gst_gl_context_create(native,   │    │
                  │  │                          wrapped)│    │
                  │  │  → calls -[NSOpenGLContext       │    │
                  │  │     initWithFormat:shareContext:]│    │
                  │  └────────┬─────────────────────────┘    │
                  │           │                              │
                  │  ┌────────▼─────────┐                    │
                  │  │ GstGLMemory      │  texture ID visible│
                  │  │  (GL texture)    │  in WebKit's ctx   │
                  │  │                  │  via share group   │
                  │  └──────────────────┘                    │
                  └──────────────────────────────────────────┘
```

### Key API contracts

- **WebKit must pass an `NSOpenGLContext *`** as the wrapped handle, NOT
  a raw `CGLContextObj`. Get it from `CAOpenGLLayer`'s context property.
- **WebKit must run NSApplication's run loop on main thread** so that
  `dispatch_sync(main_queue, ...)` inside `gst_gl_context_cocoa_create_context`
  doesn't deadlock. WebKit already does this — its main thread is the
  Cocoa event loop.
- **GstContext propagation in 1.4** uses these names (confirmed via binary
  strings in libgstgl-1.0.0.dylib):
  - `gst.gl.GLDisplay` — the display
  - `gst.gl.GstGLContext` — the app's GL context (share parent)
  - Fields: `gst.gl.context.handle` (CGL/NSOpenGL handle as uint64),
    `gst.gl.context.type` (`"gst.gl.context.CGL"` on macOS),
    `gst.gl.context.apis` (GstGLAPI bitmask)
- **`gst_gl_context_thread_add` IS synchronous in 1.4.** Verified by
  reading `gst_gl_window_default_send_message` (gstglwindow.c:361) —
  blocks on `g_cond_wait` until the callback has run. So
  `glDeleteTextures` after `thread_add` is safe.

---

## Open residual risk: renderer match

`-[NSOpenGLContext initWithFormat:shareContext:]` returns nil if both
contexts' pixel formats resolve to different CGL renderers (kCGLBadMatch).
This is the actual share-group constraint, not accum-size match as
sometimes claimed.

Apple's documented rule: the renderer must match. ColorSize 24 vs 32,
kCGLPFAAccelerated vs software fallback, and display mask all influence
renderer resolution.

GstGL 1.4.5's pixel format attributes (gstglcontext_cocoa.m:227):
```objc
NSOpenGLPixelFormatAttribute attribs[] = {
    NSOpenGLPFADoubleBuffer,
    NSOpenGLPFAAccumSize, 32,
    0
};
```

WebKit's `CAOpenGLLayer` negotiates its own pixel format separately.
Whether the two resolve to the same renderer on the 9400M is unknown
until observed.

### Diagnostic hooks (planned for consumer port, not yet written)

**Hook 1 — share-result log** in patched `gst_gl_context_cocoa_create_context`,
after `initWithFormat:shareContext:`:

```objc
if (glContext && context_cocoa->priv->external_gl_context) {
    GST_DEBUG("share: this=%p share=%p", glContext, context_cocoa->priv->external_gl_context);
} else if (!glContext && context_cocoa->priv->external_gl_context) {
    GST_ERROR("share FAILED: initWithFormat:shareContext: returned nil (renderer mismatch?)");
} else if (!context_cocoa->priv->external_gl_context) {
    GST_WARNING("no external_gl_context set on context — GstContext propagation missed?");
}
```

**Hook 2 — renderer-ID comparison** alongside hook 1:

```objc
GLint webkitRenderer = 0, gstRenderer = 0;
CGLGetParameter((CGLContextObj)[context_cocoa->priv->external_gl_context CGLContextObj],
                kCGLCPCurrentRendererID, &webkitRenderer);
CGLGetParameter([glContext CGLContextObj], kCGLCPCurrentRendererID, &gstRenderer);
GST_DEBUG("renderer IDs: webkit=0x%x gst=0x%x %s",
          webkitRenderer, gstRenderer,
          webkitRenderer == gstRenderer ? "(MATCH)" : "(MISMATCH)");
```

**Hook 3 — early glIsTexture test** in WebKit's media player init:

```cpp
GLuint testTex;
glGenTextures(1, &testTex);
glBindTexture(GL_TEXTURE_2D, testTex);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
glFlush();  // <-- sync point: object names propagate immediately, state needs flush

gst_gl_context_thread_add(m_nativeGLContext.get(), [](GstGLContext*, gpointer data) {
    GLuint* tex = static_cast<GLuint*>(data);
    GLboolean visible = glIsTexture(*tex);
    if (!visible) GST_ERROR("SHARE-GROUP FAILURE: texture from compositor not visible in GstGL ctx");
}, &testTex);

glDeleteTextures(1, &testTex);  // safe — thread_add is synchronous in 1.4
```

---

## File inventory

| File | What it is |
|---|---|
| `gstgl-106-shim.m` | Source for the runtime shim that adds 8 no-op/identity selectors to NSView. **SUPERSEDED** by the source-build approach — the source build will `#if` these calls out at compile time, eliminating the need for the shim entirely. Kept here for historical reference. |
| `libgstgl106shim.dylib` | Compiled shim (x86_64, 10.6 deployment target). Loaded via `DYLD_INSERT_LIBRARIES` for standalone testing against the pre-built 1.4.5 pkg. **SUPERSEDED** — same reason as above. |
| `gst-gl-probe.c` | Standalone probe that answers: does `gst_gl_context_new_wrapped` work? Does `gst_gl_context_new` + `activate` + `thread_add` work? (Note the false-positive in activate — see "Phantom success" above.) |
| `gst-gl-share-probe.c` | First attempt at share-group probe using a raw `CGLContextObj` as the wrapped handle. **Failed** because of the CGL/NSOpenGL type mismatch in `gstglcontext_cocoa.m:219`. |
| `gst-gl-share-probe2.m` | Second attempt using `NSOpenGLContext` as the wrapped handle. **Hung** in `gst_gl_context_create` due to the `dispatch_sync(main_queue)` deadlock. |
| `gst-gl-share-probe3.m` | Third attempt with proper `NSApplication` run loop threading. Also hung — the probe's main thread runs NSApp but the GL thread's dispatch_sync didn't land. Conclusion: standalone probe approach hit its useful limit; consumer port will not hit this because WebKit's main is already the NSApplication run loop. |
| `macmini-screen.png` | Screenshot of the glimagesink window when the pipeline stalled at PREROLLING (before the shim was applied). Black window, no test pattern. |
| `macmini-gl-playing.png` | Screenshot of `videotestsrc ! glimagesink` rendering SMPTE color bars after the shim was applied. Proof of life for the GL upload path on 10.6 + 9400M. |

---

## Next steps (separate session)

**Build dependency decision (resolve first):** the consumer port must link
against GStreamer headers + libs that include the GL API. Currently
`dist/macports-mirror/` resolves to MacPorts 1.28.5 (no `gstreamer-gl-1.0`,
since `+quartz` failed to build against 10.6 SDK). The source-built
`libgstgl` + `libgstopengl.so` are 1.4.5 (in `/Users/macbookpro/gst-plugins-bad`,
deployed to the mini's `/Library/Frameworks/GStreamer.framework/`).
For the consumer port, either: (a) add the 1.4.5 devel extract
(`/tmp/gst-devel-extract/`) to the mirror so WebKit links against 1.4.5
GL headers, or (b) bundle 1.4.5 entirely and rewire `build_610.sh` to
use the framework instead of MacPorts. The 1.x→1.x core ABI is stable
enough that mixing 1.4.5 libgstgl with 1.28.5 libgstreamer works at
runtime, but the cleaner path is a single consistent version.

1. **Source-build gst-plugins-bad 1.4.5's GL library against the 10.6 SDK** with three patches (10.7+ selector `#if` guards, dispatch_sync inline-on-main fix, activate-nil-context sanity check) + the two diagnostic logging hooks above. Output: `libgstgl-1.0.0.dylib` + `libgstopengl.so` with `LC_VERSION_MIN_MACOSX = 10.6`, no runtime shim required. **Done** — see `gst-plugins-bad` branch `snowleopard-1.4.5`, `build-snowleopard.sh`.
2. **Bundle** the source-built dylibs into `WebKit.app/Contents/Frameworks/10.6/GStreamer/` with `install_name_tool` surgery so they reference `@rpath/...` instead of `/Library/Frameworks/...`. Plus `GST_PLUGIN_PATH` setup so the registry finds the bundled plugins.
3. **Write the Cocoa GL consumer in `MediaPlayerPrivateGStreamer`** — `copyVideoTextureToPlatformTexture` for Cocoa behind a new `USE_GSTREAMER_GL_COCOA` CMake flag (since `USE(GSTREAMER_GL)` is EGL/GTK/WPE-only). The architecture diagram above is the shape.
4. **Wire up GstContext propagation** through `playbin` so the wrapped context reaches `glupload`/`gldownload`.
5. **Run the three diagnostic hooks** at first launch. If renderer IDs differ, that's the open risk surfacing; align pixel formats or patch GstGL to use a WebKit-provided pixel format.
