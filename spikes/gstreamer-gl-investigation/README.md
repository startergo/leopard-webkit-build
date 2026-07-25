# GStreamer GL Investigation — 10.6.8 / 9400M

Spike work investigating whether GStreamer's GL upload path can be used to
accelerate video playback in WebKit 610 on Mac OS X 10.6.8 (Snow Leopard)
running on the GeForce 9400M.

**Status:** architecture mapped, all API-level questions answered, source-build
path identified. Consumer port is the next phase, multi-day effort, blocked on
no unknowns.

---

## Production reality (post-port) — read alongside the rest of this file

The spike's predicted "consumer port will not hit the deadlock" (see
the table at line ~95 and the gst-gl-share-probe3.m conclusion at line
~278) was **wrong**. The consumer port did hit it, on every launch,
from inside `HTMLMediaElement::selectMediaResource`'s CFRunLoopTimer
callback. This file documents the spike's pre-production reasoning;
this section is the post-production correction.

### The recurring pattern: GstGL threading vs Cocoa

This is the third time the `dispatch_sync` / wrapped-context /
thread-topology cluster has produced a distinct bug. They are all one
underlying tension:

> **GstGL's threading model assumes a pumping main run loop and a native
> context. Cocoa/WebKit in production gives it neither.**

Three instances, in order:

1. **Probe-time deadlock** (`gst-gl-share-probe2.m`). The probe's main
   thread didn't run `NSApplication`'s event loop at all. The spike's
   patch 2 ("dispatch_sync inline-on-main") fixed the caller-on-main
   case here.
2. **`can_share` dead-end** (`gst-gl-share-probe3.m`). Walked GstGL's
   internal `other_context_ref` chain to test share capability;
   returned 0 because the share relationship hadn't been established
   via `gst_gl_context_create` yet. Circular check; not a real
   capability query.
3. **Caller-on-worker deadlock** (production, every launch). `gst_gl_context_create`
   is called on main; it spawns a worker thread; the worker calls
   `gst_gl_context_cocoa_create_context` which does
   `dispatch_barrier_sync(main_queue)`; main is blocked in `g_cond_wait`
   inside `__CFRunLoopRun`/`timerFired` and cannot drain the main queue.
   Patch 2 doesn't help: its "am I on main?" check returns false on the
   *worker*, so the dispatch fires, and main isn't pumping.

The shape that distinguishes #3 from #1 is *who* the caller of
`cocoa_create_context` is. Patch 2 covered "the caller is main → run
inline." #3's caller is the worker that `gst_gl_context_create`
itself spawned, so patch 2's predicate doesn't fire. Same library,
same call, different call-site drainage.

Recognize the next variant of this as "the GstGL-threading-vs-Cocoa
mismatch again" and skip the from-scratch investigation.

### Fix #1 (deployed): wrap-only, no `gst_gl_context_create`

The durable fix removes the call rather than patching its timing.
`PlatformDisplayGStreamer.cpp`'s Cocoa branch now:

1. Creates an `NSOpenGLContext*` pinned to the hardware renderer
   (`kCGLPFAAccelerated` + `kCGLPFANoRecovery`).
2. `gst_gl_context_new_wrapped(display, (guintptr)nsCtx,
   GST_GL_PLATFORM_CGL, GST_GL_API_OPENGL)`.
3. **Does not call** `gst_gl_context_create` (no worker = no deadlock).
4. **Does not call** `gst_gl_context_activate` on the wrapped context
   (1.4.5's `gst_gl_wrapped_context_activate` is `g_assert_not_reached`,
   so the call would abort, not return FALSE — the spike's "returns
   FALSE" claim was wrong too).
5. **Does not call** `gst_gl_context_fill_info` (does not exist in
   1.4.5; the spike's non-Cocoa branch references it but that branch
   never builds on macOS).

The wrap handle **must be `NSOpenGLContext*`, not a raw `CGLContextObj`**
(see "What does NOT work" below). `MediaPlayerPrivateGStreamerIOSurface.mm`
is unchanged — it extracts the handle via
`gst_gl_context_get_gl_context`, walks to `CGLContextObj` via
`-[NSOpenGLContext CGLContextObj]`, and binds the IOSurface texture
on that CGL handle directly. No `thread_add`, no GstGL worker thread
participation in the IOSurface copy path.

### What fix #1 does NOT solve

- **Share-group visibility across the cross-context boundary** is still
  open. Under fix #1, glupload still creates its own NSOpenGLContext
  sharing with the wrapped one via normal GstContext propagation.
  Whether that sharing actually works on the 9400M (renderer match)
  is the original "Open residual risk" below — fix #1 is neutral on
  it, but the renderer-ID log emitted by `createCocoaGstGLShareContext`
  is now load-bearing for distinguishing a real cross-context share
  failure from the FBO-readback false-black the spike already chased.
- **Runtime `gst_gl_context_create` from downstream GstGL elements**
  (e.g. glupload creating its own native context) can still deadlock
  if main is inside a long-running timer callback at the moment the
  worker dispatches. Between timer firings, main pumps normally and
  the dispatch lands. Fix #1 solves the per-launch certain deadlock;
  the runtime one is latent and intermittent.

### 1.4.5 cannot host the GL video sink — 1.6.4 reopens as the base

**Foundational finding, post-deploy.** Fix #1 was deployed, the
deadlock was eliminated, and the wrapped context resolved to the
hardware renderer (`renderer=0x102260e (GEFORCE)` in the launch log).
But video still did not play. Tracing the failure:

  - `GLVideoSinkGStreamer.cpp:73-77` (the `webkitglvideosink`
    element's constructor) calls `gst_element_factory_make("glupload")`
    and `gst_element_factory_make("glcolorconvert")`, then
    `gst_bin_add_many(sink, upload, colorconvert, appSink, ...)`.
  - On GStreamer 1.4.5, both factory calls return NULL.
    `gst-inspect-1.0 glupload` → "No such element or plugin 'glupload'".
    The 1.4.5 `libgstopengl.so` provides `glimagesink`, `glfiltercube`,
    `gleffects`, `glcolorscale`, `glvideomixer`, `glshader`,
    `gltestsrc`, `glfilter*`, `gldeinterlace` — but **not**
    `glupload`, `glcolorconvert`, or `gldownload` as discrete elements.
  - The NULL `upload` cascades through `gst_bin_add_many` (element_1
    assertion failures), the sink is constructed half-broken, and the
    pipeline never produces frames.

  **`glupload`, `glcolorconvert`, and `gldownload` are 1.6+ elements.**
  They were added as part of the 1.6 GL rework that introduced the
  public GstGLMemory allocator + GstGLBufferPool infrastructure. In
  1.4.x the equivalent upload logic existed only as an opaque internal
  helper inside `glimagesink` — not exposed as a discrete element
  factory, not usable from outside the sink.

### Why this reopens the 1.4.5-vs-1.6.4 decision

When the base was chosen, the comparison weighed:

  - Cocoa backend compatibility (1.4.5 needs two 10.7+ selector
    guards; 1.6.4 doesn't).
  - `gst_gl_context_create` deadlock topology (1.4.5 has it; 1.6.4
    reworks the Cocoa backend such that the dispatch_sync path is
    different).
  - `can_share` accepting wrapped contexts (1.6.4 better).
  - kCGLPFAOpenGLProfile uses (1.6.4 properly `#if`-guarded).
  - "Already built and working" — which weighed heavily in 1.4.5's
    favor.

What that comparison did NOT weigh: **element availability**. The
`webkitglvideosink` element in upstream WebKit (which is what the
leopard consumer port imports) assumes GstGL 1.6+ element factories.
The "1.4.5 is the right target" conclusion was correct for the
factors examined and wrong for a factor not examined — the same shape
as the "consumer port won't hit the deadlock" claim disproven by
production. The base decision must be reopened with element
availability as a decided factor.

### The three "fix on 1.4.5" options are all dead ends

  - **Substitute 1.4.5 equivalents for glupload/glcolorconvert.**
    There are none. The raw-frame path (appsink pulling CPU frames)
    defeats the IOSurface bridge, which expects GstGLMemory textures.
  - **Stub the sink's GL processing, let playbin autoplug upstream.**
    The spike architecture's original assumption was that glupload
    lives in playbin's autoplug — but it actually lives in the sink's
    OWN bin (line 77). Stubbing the sink means GstGLMemory never gets
    produced, so the IOSurface bridge has nothing to consume. This
    option is honestly "disable the GL sink," same as patch 26's
    CPU-paint fallback.
  - **Backport glupload/glcolorconvert from 1.6 into 1.4.5.** These
    elements depend on 1.6's GstGLMemory allocator changes,
    GstGLBufferPool reworks, and the upload-meta infrastructure.
    Backporting them is backporting most of the 1.6 GL memory
    subsystem into a 1.4 tree — producing a 1.4/1.6 hybrid rather
    than a clean base.

None of the three delivers accelerated video on 1.4.5. The base
itself is structurally insufficient.

### Path forward: 1.6.4 as the base

All the leopard toolchain and patch work transfers. The current
patched source tree lives at:

  `/Users/macbookpro/gst-plugins-bad` — branch `snowleopard-1.4.5`
  with six `[leopard]` commits on top of upstream `Release 1.4.5`
  (`faed71d32`):

    `2a8297477` patch Cocoa GL backend for 10.6 SDK compatibility + diagnostics
    `0487014ea` add build-snowleopard.sh for reproducible 10.6 cross-compile
    `8b8760a20` fix K&R declaration in gstglfilterglass.c for modern clang
    `14222aa88` guard gstgltransformation.c with HAVE_GRAPHENE
    `f9b34e9ac` build-snowleopard.sh: add plugin build steps + all overrides
    `9fef0c937` gitignore ORC-generated bad-video-orc.h

  Tags `1.6.0` through `1.6.4` are present in the same repo. The
  1.6.4 tag contains the elements the sink needs:
  `ext/gl/gstglcolorconvertelement.c`,
  `gst-libs/gst/gl/gstglcolorconvert.c`,
  `gst-libs/gst/gl/gstglupload.c`.

  **Do not confuse with** `/Users/macbookpro/gstreamer` — that's
  the `gstreamer` CORE module clone (separate repo, no GstGL content,
  no leopard patches; its `snowleopard-1.4.5` branch == upstream 1.4.5
  tag).

Concrete port path:

  1. `git checkout -b snowleopard-1.6.4 1.6.4` in
     `/Users/macbookpro/gst-plugins-bad`.
  2. Cherry-pick the six `[leopard]` commits. Most should apply
     cleanly; the load-bearing one (`2a8297477` Cocoa GL backend)
     may need rework against 1.6.4's refactored Cocoa backend.
  3. The fix #1 deadlock patch (`2a8297477`'s dispatch_sync inline-
     when-main) is likely **unnecessary on 1.6.4** — the Cocoa
     backend's `gst_gl_context_cocoa_create_context` was reworked
     to avoid the dispatch_sync(main_queue) topology entirely. The
     WebKit-side fix #1 (commit `f569a3ac90` in sources_610) is
     still valid and harmless on 1.6.4 (it just wraps the share-
     parent context differently), but the libgstgl patch becomes
     a no-op.
  4. Build via the existing `build-snowleopard.sh`, deploy the
     resulting `libgstgl-1.0.0.dylib` + `libgstopengl.so` to the
     framework slot, replacing the current 1.4.5 versions.
  5. Re-test the deploy. The renderer-ID log from fix #1's
     `createCocoaGstGLShareContext` is still load-bearing for the
     share-group verification (cross-context visibility between the
     wrapped context and glupload's auto-created context).

### MacPorts hygiene (preventive)

`/opt/local/lib/gstreamer-1.0/` and ~35 MacPorts `libgst*-1.0.0.dylib`
files on the macmini were renamed to `.disabled` (reversible). lsof
confirmed Safari was loading exclusively from
`/Library/Frameworks/GStreamer.framework/` anyway, so this is
preventive — eliminates the future-conflict risk if the GST registry
ever rebuilds and scans both paths. Restore via `mv *.disabled`
back to the original names.

---

## TL;DR

**[post-deploy update]** The "1.4.5 was the right target" framing
below is now overturned — see "1.4.5 cannot host the GL video sink"
in the Production reality section. The choice was correct for the
factors weighed (Cocoa backend, deadlock topology, can_share) but
missed element availability: `webkitglvideosink` requires `glupload`
and `glcolorconvert`, both 1.6+ additions. 1.6.4 reopens as the
correct base. The 1.4.5 work isn't wasted — it proved the toolchain,
the Cocoa patches, the build script, and the wrap-only fix #1 design;
all transfer to 1.6.4. The text below is preserved as historical
record of the 1.4.5 investigation; read it with the post-deploy
update in mind.

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
     **[post-port correction]** Patch 2 only covers the caller-on-main
     case. The production hit was caller-on-worker — see "Production
     reality" above. Fix #1 removes the call rather than patching the
     topology.
  3. `gst_gl_context_cocoa_activate` silently returns TRUE on a nil GL context.
     **[post-port correction]** On wrapped contexts in 1.4.5,
     `gst_gl_wrapped_context_activate` is `g_assert_not_reached`, not
     a FALSE return. Do not call activate on a wrapped context at all.
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
| `gst_gl_context_create(native, wrapped, &err)` from main thread of a non-NSApplication caller | Hangs | `gstglcontext_cocoa.m:223` does `dispatch_sync(dispatch_get_main_queue(), block)`. The GL thread spawned by `gst_gl_context_create` calls this; main thread is blocked on `g_cond_wait` waiting for that same GL thread. Deadlock. ~~**Real consumer (WebKit) won't hit this** — main thread runs `NSApplication`'s run loop, which services the main queue and lets `dispatch_sync` return.~~ **[post-port correction: WRONG.** The consumer port did hit this, every launch, from inside a CFRunLoopTimer callback where the main run loop is not pumping the dispatch queue. See "Production reality" at the top of this file. Fix #1 removes the call entirely.) |

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
| `gst-gl-share-probe3.m` | Third attempt with proper `NSApplication` run loop threading. Also hung — the probe's main thread runs NSApp but the GL thread's dispatch_sync didn't land. ~~Conclusion: standalone probe approach hit its useful limit; consumer port will not hit this because WebKit's main is already the NSApplication run loop.~~ **[post-port correction: that conclusion was wrong — the consumer port hits exactly this.** The probe's hang was the real topology: main is inside something that isn't pumping the dispatch queue. In the probe that was NSApp-without-drainage; in the consumer port it's `__CFRunLoopRun`/`timerFired`. Same shape. The spike had the evidence in hand and misread it. See "Production reality" at the top of this file.] |
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
