# WebKit 610 + GStreamer 1.4.5 on 10.6 — spike findings

Three premise invalidations, in chronological order. Each was caught by
probing before building, and each changed the plan. The most recent
(the CALayer finding) redesigns the consumer-port bridge architecture
and is the current entry point — read it first.

**Status:** All 26 patches landed. Build succeeds end-to-end with
USE_GSTREAMER_GL genuinely on; DMG produced. The CPU-download fallback
(patch 26) is the working-but-unaccelerated baseline. The GL-accelerated
path requires the IOSurface bridge documented in §"CURRENT ENTRY POINT"
below.

The original two findings (GL-off bug, header floor) remain below as
historical context — both are resolved, both shaped the path, neither
is the current question.

---

## CURRENT ENTRY POINT: Mac compositor is plain CALayer — bridge redesigned

**Fourth premise invalidation.** The original spike
(`spikes/gstreamer-gl-investigation/`) planned a wrapped→native→sharegroup
architecture: wrap WebKit's compositor NSOpenGLContext via
`gst_gl_context_new_wrapped()`, let GstGL's contexts share with it via
share-group inheritance, and the resulting textures would be visible in
WebKit's compositor. That plan was internally sound — the can_share
semantics, the dispatch_sync deadlock, the 1.6 sharegroup inheritance,
all correctly characterized — but it was aimed at the wrong target.

**The premise that doesn't hold: WebKit 610's Mac compositor uses
CAOpenGLLayer with an NSOpenGLContext we can wrap.**

It doesn't. The compositor uses plain `CALayer` + Core Animation. There
is no GL context in the compositor at all.

### Evidence

| Source | Finding |
|---|---|
| `platform/graphics/cocoa/WebGLLayer.h:39-43` | `WebGLLayer : CALayer` (not `CAOpenGLLayer`) when `USE(OPENGL)` is true |
| `platform/graphics/ca/cocoa/PlatformCALayerCocoa.h:34` | Compositor layer wrapper manages plain `CALayer` instances |
| `platform/graphics/ca/PlatformCALayer.h:62-80` | No `LayerTypeCAOpenGLLayer` or equivalent exists |
| `platform/graphics/gstreamer/PlatformDisplayGStreamer.cpp:81-130` | The `#if PLATFORM(COCOA) return false` stub in patch 23 was correctly avoiding a non-existent API surface |

Additionally: no `PlatformDisplayMac` class exists, no
`sharingGLContext()` singleton on Mac, no `createShared()` root context.
`GraphicsContextGLOpenGLCocoa` contexts are WebGL/Canvas-only, not the
compositor's. `CGLCreateContext` supports sharing via constructor param
but each context is per-instance — no root share group exists.

### The actual Mac path (what the bridge becomes)

This is structurally good news even though it voids the spike's plan.
The IOSurface path is the standard, well-trodden macOS zero-copy GPU
presentation mechanism — `CGLTexImageIOSurface2D` binding an IOSurface
as a GL render target, then handing that IOSurface to a `CALayer`, is
how QuickTime, AVFoundation, and every accelerated video layer on the
Mac has worked since 10.6. Both `IOSurface` and `CGLTexImageIOSurface2D`
are 10.6-era APIs — no version wall.

The redesigned bridge:

```
1. GstGL renders into its own CGL context
   (no sharing needed — see "Why this is a quiet win" below)
2. The GL texture is bound to an IOSurface via CGLTexImageIOSurface2D
3. The IOSurface becomes CALayer.contents
4. Core Animation composites it — no compositor GL context required,
   because there isn't one
```

### Why this is a quiet win

Every hard problem mapped in the share-group architecture was in service
of getting GstGL's texture into WebKit's GL context. On the IOSurface
path, GstGL keeps its own context entirely and hands off via IOSurface.
The share-group problem doesn't get solved — it gets deleted.

Concretely, voided from the spike's risk list:
- Pixel format matching between wrapped and native contexts
- `can_share` semantics being a circular check rather than real capability
- The `dispatch_sync(main_queue)` deadlock inside `gst_gl_context_create`
  (still exists in patched libgstgl but never gets hit — no wrapping)
- Renderer-ID match for CGL share-group validity
- The wrapped-vs-native context distinction itself

### What survives from the spike

| Survives | Reason |
|---|---|
| Patched 10.6-native `libgstgl` build (`/Users/macbookpro/gst-plugins-bad/`, `snowleopard-1.4.5` branch) | GstGL still produces the texture — just into its own context now, not a wrapped one |
| 1.4.5-vs-1.6.4 base-version decision | The GstGL version that produces the texture is unaffected by the presentation mechanism |
| Build scripts (`build-snowleopard.sh`, macports-mirror, gst145-mirror) | Independent of bridge shape |
| Patches 1-26 (compile-time gates, CMake wiring, Cocoa stubs) | All proven by the end-to-end DMG build |

### What's voided

The entire wrapped→native→sharegroup consumer architecture documented
in `spikes/gstreamer-gl-investigation/README.md` §"Architecture for the
consumer port" and §"API surface findings." Those sections describe the
GTK/EGL/WPE bridge pattern; they don't apply to Cocoa's CALayer-based
compositor.

The investigation's diagnostic hooks (share-result logging, renderer-ID
capture) were for observing share-group behavior that no longer matters.
The three probe files (`gst-gl-probe.c`, `gst-gl-share-probe*.m`) tested
a wrapping path that the IOSurface bridge doesn't use.

### First probe for next session

Before writing the IOSurface bridge, answer this:

**Does GstGL 1.4.5 expose a way to render into a caller-provided
FBO/texture backed by an IOSurface, or does it insist on its own
allocation?**

**Probe result (recorded for next-session entry):** both paths exist in
1.4.5. From `dist/gst145-mirror/include/gstreamer-1.0/gst/gl/gstglmemory.h`:

- `gst_gl_memory_wrapped_texture(context, texture_id, tex_type, w, h, user_data, notify)`
  (line 160) — wraps a caller-provided texture id as a GstGLMemory. GstGL
  renders into your texture. Zero blit.
- `gst_gl_memory_alloc(context, tex_type, w, h, stride)` (line 154) —
  GstGL allocates its own texture. Bridge would then blit.
- `gst_gl_memory_copy_into_texture(gl_mem, tex_id, tex_type, w, h, stride, respecify)`
  (line 165) — copies GstGL's texture into a caller-provided target. The
  blit API.

The probe question resolves to "both are supported." Choice becomes
architectural:

- **Path (a) — wrap IOSurface-backed texture:** Bridge code calls
  `CGLTexImageIOSurface2D` inside GstGL's context to allocate a texture
  backed by the IOSurface, then calls `gst_gl_memory_wrapped_texture`
  with that texture id. GstGL's upload elements write directly into the
  IOSurface-backed texture. Zero blit, single context.
- **Path (b) — GstGL-allocated + blit:** Let GstGL allocate its own
  texture via `gst_gl_memory_alloc`. Bridge allocates IOSurface +
  IOSurface-backed texture in GstGL's context, then calls
  `gst_gl_memory_copy_into_texture` per frame. Two textures in the same
  context, one blit.

Either path is single-context (GstGL's own) — no share-group required,
because CALayer can take the IOSurface directly via `setContents:`
without needing a GL context on the consumer side. Core Animation
composites the IOSurface without any WebKit GL involvement.

The architectural decision (path a vs b) is the next session's first
design choice. Path (a) is structurally simpler but requires the
wrapped_texture API to be reliable in 1.4.5 (probe its actual behavior
on a real GstGLMemory produced by glupload — does the wrapped texture
get respected end-to-end through the pipeline, or do some elements
insist on allocating their own?). Path (b) is more conservative (GstGL
controls its own allocation, blit is explicit) but adds per-frame blit
cost.

Probe before designing: write a small test that allocates an
IOSurface-backed texture in a CGL context, wraps it via
`gst_gl_memory_wrapped_texture`, runs videotestsrc ! glupload !
appsink, and checks whether the resulting samples reference the wrapped
texture or allocate new ones. That probe determines path (a) vs (b)
empirically.

**Probe written (not yet run):** `spikes/gstreamer-gl-investigation/gst-gl-iosurface-probe.c`
tests the three primitives — IOSurfaceCreate, CGLTexImageIOSurface2D,
gst_gl_memory_wrapped_texture — on 10.6 + 9400M. Build/run instructions
in the file header. This is the prerequisite probe; the full
glupload-pipeline test is a follow-up once these primitives are confirmed.

### CPU-download fallback (current baseline)

### CPU-download fallback (current baseline)

Patch 26's stubs make the build succeed by routing video through the
existing `ImageGStreamerCG` CPU paint path. Video plays correctly
without GL acceleration. This is the working baseline the IOSurface
bridge replaces — not a workaround to tolerate, but a known-good
fallback that lets development proceed in parallel.

---

## HEADLINE FINDING: USE_GSTREAMER_GL is forced OFF by a CMake wiring bug

**Status (at discovery):** GL is OFF at compile time, despite `find_package(GStreamer 1.4.5 ...
gl)` succeeding, `PC_GSTREAMER_GL_FOUND=1`, `GSTREAMER_GL_LIBRARIES`
pointing at the mirror's `libgstgl-1.0.dylib`, and the
`GLVideoSinkGStreamer.cpp.o` target being present in `build.ninja`.

### Root cause

`Source/cmake/OptionsMac.cmake:120` uses plain `set()`:

```cmake
set(ENABLE_GRAPHICS_CONTEXT_GL ON)
```

`ENABLE_GRAPHICS_CONTEXT_GL` is **only** registered as a `WEBKIT_OPTION` in
`OptionsGTK.cmake:92` and `OptionsWPE.cmake:89` — *not* in `OptionsMac.cmake`.
Plain `set()` creates a regular CMake variable; the `WEBKIT_OPTION` framework
only knows about options registered via `WEBKIT_OPTION_DEFINE`.

`Source/cmake/GStreamerDependencies.cmake:2` declares:

```cmake
WEBKIT_OPTION_DEPEND(USE_GSTREAMER_GL ENABLE_GRAPHICS_CONTEXT_GL)
```

The `WEBKIT_OPTION_DEPEND` resolver looks up the *registered* option
`ENABLE_GRAPHICS_CONTEXT_GL`. On the Mac platform that option doesn't exist
in the registry → the dependency is treated as unsatisfied →
`USE_GSTREAMER_GL` is forced `OFF`, overriding its `WEBKIT_OPTION_DEFINE(...)
PRIVATE ON` default.

### Consequence

Every `#if USE(GSTREAMER_GL)` source guard evaluates false. Specifically:

- `GLVideoSinkGStreamer.cpp` — entire body gated on line 22:
  `#if ENABLE(VIDEO) && USE(GSTREAMER_GL)`. The file compiles to an empty
  translation unit. The `GLVideoSinkGStreamer.cpp.o` in `build.ninja` is
  real but empty.
- `PlatformDisplayGStreamer.cpp` — not even added to the build
  (gated by `if (USE_GSTREAMER_GL)` in `Source/WebCore/platform/GStreamer.cmake:120`).
- `GStreamerVideoFrameHolder.cpp`'s GL sections, the `GstEGLImage`
  specializations in `GRefPtrGStreamer.{h,cpp}`, the GL branches in
  `MediaPlayerPrivateGStreamer.cpp` — all preprocessed away.

The patched `libgstgl` + `libgstopengl.so` we deployed to the bundle have
no consumer in WebCore. The standalone `videotestsrc ! glimagesink` probe
(spike: `spikes/gstreamer-gl-investigation/`) is real — that path runs in
the GStreamer framework's own process, not in WebKit. The WebKit consumer
port has not started.

### The fix is one line — but it opens rather than closes work

```cmake
# OptionsMac.cmake, replace line 120:
WEBKIT_OPTION_DEFINE(ENABLE_GRAPHICS_CONTEXT_GL "Whether to use OpenGL." PUBLIC ON)
```

(or equivalently `-DUSE_GSTREAMER_GL=ON` on the `cmake` invocation in
`build_610.sh`). Either flips GL on at compile time. But doing so
activates a much larger body of GL code that was previously preprocessed
out — code written for the EGL/GTK/WPE platforms. On a Cocoa/CGL target
that code references symbols that don't exist in the 1.4.5 Cocoa GL
backend (`GstEGLImage`, the EGL/X11 display constructors in
`PlatformDisplayGStreamer.cpp`, etc.). That's the multi-day consumer
port already scoped in `spikes/gstreamer-gl-investigation/README.md`.

### Post-fix state (this session — patches 21 + 22 landed)

The "one line" framing in the prior section was incomplete. The actual
fix required two patches because there were two stacked wiring bugs,
not one:

- **Patch 21** (`patches-610/21-options-mac-register-graphics-context-gl.patch`)
  adds both `include(GStreamerDefinitions)` AND
  `WEBKIT_OPTION_DEFINE(ENABLE_GRAPHICS_CONTEXT_GL ...)` inside the
  `WEBKIT_OPTION_BEGIN/END` scope in `OptionsMac.cmake`. The prior
  session's analysis identified only the missing DEFINE; the missing
  `include(GStreamerDefinitions)` was a second bug stacked underneath —
  without it, `USE_GSTREAMER_GL` was never registered as a WEBKIT_OPTION
  on Mac at all, so the dependency check never fired even after the
  DEFINE was added.

- **Patch 22** (`patches-610/22-gstreamer-checks-expose-use-gstreamer-gl.patch`)
  adds `SET_AND_EXPOSE_TO_BUILD(USE_GSTREAMER_GL TRUE)` in
  `GStreamerChecks.cmake` after the GL_FOUND check. The WEBKIT_OPTION
  framework registers the option in cache but doesn't auto-emit the
  compile define; `SET_AND_EXPOSE_TO_BUILD` is what propagates to
  `cmakeconfig.h`. Without this, `USE_GSTREAMER_GL:BOOL=ON` appeared in
  `CMakeCache.txt` but `#define USE_GSTREAMER_GL 1` was absent from
  `cmakeconfig.h` and every `#if USE(GSTREAMER_GL)` source guard still
  evaluated false.

**Verification post-fix:**

```
$ grep GSTREAMER_GL build-610/cmakeconfig.h
#define ENABLE_GRAPHICS_CONTEXT_GL 1
#define USE_GSTREAMER_GL 1
#define USE_GSTREAMER_GL 1     (duplicated — cosmetic, see note)
#define USE_GSTREAMER_MPEGTS 0
```

(Cosmetic note: `USE_GSTREAMER_GL` appears twice because
`SET_AND_EXPOSE_TO_BUILD` is called via both the WEBKIT_OPTION framework
and my patch 22. Harmless — redefinition warning, same value. Can be
cleaned up later by removing the explicit call once the framework's
emission is sufficient.)

**What GL-on surfaces (the consumer-port boundary):**

The rebuild now fails at `GRefPtrGStreamer.cpp:27`:

```
fatal error: 'gst/gl/egl/gsteglimage.h' file not found
```

The 1.4.5 Cocoa framework ships only **platform-agnostic** GL headers
(20 files in `gst/gl/`). The `gst/gl/egl/` and `gst/gl/x11/` subdirs
don't exist — Cocoa uses CGL, not EGL. WebKit 610's `#if USE(GSTREAMER_GL)`
blocks include those platform headers *without platform sub-guards*:

| File:line | Include |
|---|---|
| `GRefPtrGStreamer.cpp:27` | `<gst/gl/egl/gsteglimage.h>` |
| `MediaPlayerPrivateGStreamer.cpp:149` | `<gst/gl/egl/gsteglimage.h>` |
| `MediaPlayerPrivateGStreamer.cpp:150` | `<gst/gl/egl/gstglmemoryegl.h>` |
| `PlatformDisplayGStreamer.cpp:27` | `<gst/gl/x11/gstgldisplay_x11.h>` |
| `PlatformDisplayGStreamer.cpp:32` | `<gst/gl/egl/gstgldisplay_egl.h>` |

And the 15 EGL/X11-platform-specific symbols these code paths reference
(none in the 1.4.5 Cocoa export table):

```
gst_egl_image_export_dmabuf          gst_gl_color_convert_new
gst_egl_image_from_texture           gst_gl_color_convert_perform
gst_egl_image_ref                    gst_gl_color_convert_set_caps
gst_egl_image_unref                  gst_gl_context_fill_info
gst_buffer_get_gl_sync_meta          gst_gl_display_egl_new_with_egl_display
gst_buffer_get_video_gl_texture_upload_meta   gst_gl_display_x11_new_with_display
gst_gl_memory_get_texture_target     gst_gl_sync_meta_wait_cpu
gst_is_gl_memory_egl
```

This is the consumer-port surface — the work scoped in
`spikes/gstreamer-gl-investigation/README.md`. It is now reachable code,
not preprocessed-away theory.

### Consumer-port entry gates (patch 23, this session)

Four changes that get the build past the first wave of GL-on failures.
Each is a discrete, behavior-preserving gate or version substitution —
not the actual Cocoa→GstGL bridge, which remains multi-day work.

| File | Change | Why |
|---|---|---|
| `GRefPtrGStreamer.{h,cpp}` | Tighten `GstEGLImage` template specializations from `USE(GSTREAMER_GL)` to `USE(GSTREAMER_GL) && USE(EGL)`. Gate the `<gst/gl/egl/gsteglimage.h>` include the same way. | GstEGLImage is the EGL-platform wrapper; on Cocoa (CGL) it doesn't apply. Matches the pattern already in `PlatformDisplayGStreamer.cpp:25-33`. |
| `ImageDecoderGStreamer.cpp:230,235` | `gst_app_sink_try_pull_preroll/_sample(sink, 0)` → blocking `gst_app_sink_pull_preroll/_sample(sink)` on `<1.10`. | Both calls are inside `GstAppSinkCallbacks` (`new_preroll`, `new_sample`), which fire precisely because data is queued. Blocking returns immediately. Per-site comment in source. |
| `WebKitWebSourceGStreamer.cpp:227` | `gst_element_class_add_static_pad_template(klass, &tmpl)` → two-step `gst_static_pad_template_get(&tmpl)` + `gst_element_class_add_pad_template(klass, ...)` on `<1.14`. | Pure mechanical rewrite — the 1.14+ API is a convenience wrapper around the universal two-step. |
| `PlatformDisplayGStreamer.cpp:81-130` | Stub `PlatformDisplay::tryEnsureGstGLContext()` to `return false` on Cocoa via `#if PLATFORM(COCOA)`. | The body assumes EGL/GLX/WPE: calls `sharingGLContext()`, `PlatformGraphicsContextGL`, `GLContext::current()` — none exist on Cocoa's PlatformDisplay. The actual CGL→GstGL bridge is the multi-day consumer port. Stubbing lets callers fall back to the non-GL video sink path. |

**Build state post-patch-23:** fails on `VideoTextureCopierGStreamer.h:26: fatal error: 'TextureMapperGLHeaders.h' file not found`. The header exists at `platform/graphics/texmap/TextureMapperGLHeaders.h` but the WebCore ForwardingHeaders mechanism isn't generating a forwarding entry for it (or for `TextureMapperPlatformLayerBuffer.h`). Affected TUs: `VideoTextureCopierGStreamer.cpp` and `MediaPlayerPrivateGStreamer.cpp` (transitively).

**Next-session entry point:** either (a) add `platform/graphics/texmap` to WebCore's include paths in `Source/WebCore/CMakeLists.txt`, (b) extend the ForwardingHeaders generator to cover texmap/, or (c) write the actual Cocoa compositor bridge (the proper consumer port). The first two are mechanical; the third is multi-session.

### How this went uncaught

The build script's verification steps ("GLVideoSinkGStreamer.cpp.o
present in build.ninja", "PC_GSTREAMER_GL_FOUND=1 in CMakeCache") were
all green. They measure *infrastructure presence*, not *code path
activation*. A `.o` file that compiles to nothing satisfies the build
system without satisfying the goal. `DUSE_GSTREAMER_GL` is absent from
every compile command in the build log — the one tell that would have
surfaced this — but nothing was looking for it.

**Lesson:** the GL-on verification needs to be `grep -E 'DUSE_GSTREAMER_GL'
in the actual compile command`, not "library found" or "`.o` target
present." Add this check to `build_610.sh`'s Phase 7 verify.

---

## Measurement attempts and their validity

A symbol-gap analysis was performed mid-spike. Its results are **void** —
recorded here only so the methodology isn't trusted for the wrong build.

| Measurement | Result | Validity |
|---|---|---|
| 39-symbol closed gap list | Bounded, "finishable" | **Invalid.** Measured against the GL-off build. Most "missing" GL/EGL symbols were inside `#if USE(GSTREAMER_GL)` guards that were false, so the preprocessor eliminated them. The actual gap for the intended (GL-on) build is unmeasured. |
| 4 `gst_allocator_fast_malloc_*` symbols | Real gap | **False positive.** These are auto-generated by `G_DEFINE_TYPE` — `_init`/`_class_init` are file-local statics; `_get_type` is exported under its C++ mangled name. Source-grep can't distinguish external refs from macro-emitted local names. |
| Type-mismatch errors on `gst_element_query_duration` / `gst_structure_get_uint64` | gint64/long-long issue | **Real, mirror-side fix.** See "Mirror glibconfig.h patch" below. |

The methodology (source-grep + nm-on-dylibs + diff) is sound; the
*application* was wrong. Re-running it against the GL-on build is a
next-session task.

---

## Mirror-side requirement: glibconfig.h gint64 patch

The mirror at `dist/gst145-mirror/lib/glib-2.0/include/x86_64/glibconfig.h`
ships with the LP64-default `typedef long gint64`. The 10.6 SDK's
`int64_t` is `long long` — a C++-distinct type. This breaks overload
resolution for `gst_element_query_duration`, `gst_structure_get_uint64`,
and similar 1.0-era APIs that take `gint64*`/`guint64*`.

The macports-mirror has a hand-patched glibconfig.h that forces
`typedef signed long long gint64`. Port that patch to gst145-mirror —
see `dist/macports-mirror/lib/glib-2.0/include/glibconfig.h:67-76` for
the reference. ABI is identical on x86_64 LP64 (both 8-byte); only C++
type identity changes, so the 1.4.5 dylibs (built with `typedef long`)
remain link-compatible.

This fix is independent of the GL-on question — it's required for *any*
compile against gst145-mirror.

---

## SECONDARY FINDING: WebKit 610's GStreamer header floor (1.10+)

Valid finding, but downstream of the GL-off issue. Kept here for the
record; its in-flight patch (patches-610/20) is committable as
playbin3-path gating regardless of GL state, but it doesn't unblock the
consumer port.

### TL;DR (header-floor portion)

- **WebKit 610 was written assuming GStreamer ≥ 1.10.** Three type
  families are referenced *unconditionally* in header template
  specializations and then propagate into ~7 files' public signatures:
  - `GstStream`, `GstStreamCollection` — added in GStreamer **1.10**
    stable (1.5.2 dev). Referenced by `GRefPtrGStreamer.h:119-125`.
  - `GstVideoConverter`, `GstAudioConverter` — added in **1.6**.
    Referenced by `GUniquePtrGStreamer.h:45-46`.
- **The plan's risk #5 claim was wrong.** "All `GST_CHECK_VERSION` gates
  traced to working `<1.10` `#else` fallbacks" applied only to the
  *implementation* `.cpp` files (MediaPlayerPrivateGStreamer.cpp,
  GLVideoSinkGStreamer.cpp). The *header* specializations are
  unconditional — they blow up the moment any TU `#include`s
  `GRefPtrGStreamer.h` against 1.4.5 or 1.6.x headers.
- **Runtime-dead does not help you.** The `GstStream` path is driven by
  `GST_MESSAGE_STREAMS_SELECTED`, which is playbin3-only; this build
  uses playbin (see `MediaPlayerPrivateGStreamer.cpp:2751-2752`). So
  the code is runtime-dead — but it still has to *compile*, and at link
  time the emitted calls to `gst_stream_collection_get_stream`,
  `gst_video_converter_new`, etc. require symbols the 1.4.5 dylibs do
  not export. See "Option 1 is a trap" below.
- **This also rules out 1.6.4 as a target.** An earlier analysis
  proposed gst-plugins-bad 1.6.4 for native share-group inheritance.
  1.6.4 still lacks `GstStream`/`GstStreamCollection`; it would hit the
  same wall. GstStream is 1.10+, period.

---

## The compile failure (concrete)

`./build_610.sh --clean` against `dist/gst145-mirror/` (GStreamer 1.4.5
devel set, mirrored from the official framework extract). Configure
succeeds; CMake records `PC_GSTREAMER_VERSION=1.4.5`,
`PC_GSTREAMER_GL_FOUND=1`, `GSTREAMER_GL_LIBRARIES` →
`gst145-mirror/lib/libgstgl-1.0.dylib`. Patch 17 relaxes the
`find_package(GStreamer 1.10.0)` gate to `1.4.5`. Patch applied, mirror
wired, USE_GSTREAMER_GL on, `GLVideoSinkGStreamer.cpp.o` in
`build.ninja`. Ninja then fails at the WebCore gstreamer TUs with:

```
GRefPtrGStreamer.h:119:20: error: unknown type name 'GstStream'; did you mean 'TextStream'?
GRefPtrGStreamer.h:119:41: error: unknown type name 'GstStream'
GRefPtrGStreamer.h:120:12: error: unknown type name 'GstStream'; did you mean 'TextStream'?
GRefPtrGStreamer.h:120:31: error: unknown type name 'GstStream'; did you mean 'TextStream'?
GRefPtrGStreamer.h:121:27: error: unknown type name 'GstStream'; did you mean 'TextStream'?
GRefPtrGStreamer.h:121:38: error: unknown type name 'GstStream'; did you mean 'TextStream'?
GRefPtrGStreamer.h:123:20: error: use of undeclared identifier 'GstStreamCollection'; did you mean 'GetNewCollection'?
GRefPtrGStreamer.h:123:51: error: use of undeclared identifier 'GstStreamCollection'; did you mean 'GetNewCollection'?
GRefPtrGStreamer.h:123:71: error: expected expression
GRefPtrGStreamer.h:124:12: error: unknown type name 'GstStreamCollection'
GRefPtrGStreamer.h:125:27: error: use of undeclared identifier 'GstStreamCollection'; did you mean 'GetNewCollection'?
GUniquePtrGStreamer.h:45:25: error: use of undeclared identifier 'GstVideoConverter'
GUniquePtrGStreamer.h:46:25: error: use of undeclared identifier 'GstAudioConverter'
```

Once these are gated, additional errors will surface in the ~7 consumer
files. The grep of the gstreamer subtree shows `GstStream` reaches:

| File | Usage |
|---|---|
| `MediaPlayerPrivateGStreamer.cpp:1514` | `updateTracks(GRefPtr<GstStreamCollection>&&)` |
| `MediaPlayerPrivateGStreamer.cpp:1533,1610,1616,1631` | `gst_stream_collection_get_stream`, `gst_stream_get_stream_type`, `gst_message_streams_selected_get_stream` |
| `TrackPrivateBaseGStreamer.{h,cpp}` | ctor takes `GRefPtr<GstStream>`; `GstStream* m_stream` |
| `AudioTrackPrivateGStreamer.{h,cpp}` | `create(..., GRefPtr<GstStream>)`; calls `gst_stream_get_stream_flags`, `gst_stream_set_stream_flags` |
| `VideoTrackPrivateGStreamer.{h,cpp}` | same shape as Audio |
| `InbandTextTrackPrivateGStreamer.{h,cpp}` | same shape |
| `MediaPlayerPrivateGStreamer.h:466` | `void updateTracks(GRefPtr<GstStreamCollection>&&)` private decl |

---

## Version arithmetic

| Type | GStreamer version added | Notes |
|---|---|---|
| `GstStream` | 1.5.2 dev / **1.10 stable** | High-level stream object; `gst_stream_get_stream_flags`, `gst_stream_set_stream_flags`, `gst_stream_collection_get_stream`, `gst_message_streams_selected_get_stream` all land here. |
| `GstStreamCollection` | 1.5.2 dev / **1.10 stable** | Drives the playbin3 `streams-selected` message path. |
| `GstVideoConverter` | **1.6** | Used by `GUniquePtrGStreamer.h` deleter only; symbol is `gst_video_converter_free`. |
| `GstAudioConverter` | **1.6** | Same shape; `gst_audio_converter_free`. |

Sources: [1.10 release notes](https://gstreamer.freedesktop.org/releases/1.10/)
(`A new GstStream API provides applications a more meaningful view of
the structure of streams`), [GstStreamCollection
docs](https://gstreamer.freedesktop.org/documentation/gstreamer/gststreamcollection.html),
[1.6.4 release notes](https://gstreamer.freedesktop.org/releases/gstreamer/1.6.4.html)
(1.6.x maintains ABI back to 1.0; cannot introduce GstStream).

---

## Option 1 is a trap

The intuitive fix — bump only the devel headers past 1.10, keep the
proven 1.4.5 runtime binaries in the bundle — does not work.

Compiling WebCore against 1.10 headers while linking 1.4.5 dylibs means
WebCore emits calls to `gst_stream_collection_get_stream`,
`gst_video_converter_new`, etc. — symbols that do not exist in the 1.4.5
export table. The "playbin3 is runtime-dead, we use playbin" argument
protects you only at execution time; the **linker** still needs every
referenced symbol present, or the link fails with `undefined symbol`.
And worse, if you trick the link (e.g. weak stubs), the first runtime
reach of any of these paths crashes with `dyld: symbol not found`.

So Option 1 actually requires a **1.10+ runtime too**, which means
re-running the entire source-build + 10.6-compat investigation
(`kCGLPFAOpenGLProfile`, `setWantsBestResolutionOpenGLSurface:`,
`contentsScale`, `dispatch_sync` deadlock, the GL_CONTEXT_PROFILE_MASK
wall that ruled out 1.28.5 in the gl-investigation spike) at a higher
version. 1.10 is late 2016, the 10.10 packaging-floor era — likely
needs the same overlay-newer-OpenGL-headers trick that 1.28.5 needed,
plus whatever 1.10/1.12/1.14 added on top.

**Rule of thumb, capture once:** GStreamer devel headers and runtime
binaries must move together. The 1.x→1.x core ABI stability is for the
*core* symbols; the track-collection + converter + GL APIs are the ones
actively gaining across the versions WebKit 610 cares about, and
cross-version mixing fails exactly there.

---

## Options considered

| Option | What it entails | Verdict |
|---|---|---|
| **1. Bump devel headers ≥1.10, keep 1.4.5 runtime** | Just swap the mirror | **Trap.** Link fails on missing symbols. Requires 1.10+ runtime → full source-build/10.6-compat audit at the new version. Doesn't reduce consumer-port work. |
| **2. Source-patch ~7 files with `GST_CHECK_VERSION` gates** | Gate the specializations behind `GST_CHECK_VERSION(1,10,0)` / `GST_CHECK_VERSION(1,6,0)`; stub `updateTracks()` to no-op on `<1.10`. Track selection falls back to the older playbin `select-stream` / `GST_MESSAGE_STREAM_STATUS` path, which is what this config runs anyway. | **Correct next-session task.** Diff is ~7 files, mechanical, truthful (encodes a real fact about the target). Belongs in `patches-610/` alongside the existing 19 patches. |
| **3. Build GStreamer from source in phase1** | glib + orc + gstreamer + gst-plugins-base cross-compiled for 10.6. Truly self-contained repo, no `/tmp/gst-devel-extract/` extraction step. | **Cleanest long-term shape, large scope.** Phase1 extension; separate decision from the version-floor question. Doesn't unblock compile — the source build still needs to be ≥1.10 for the headers, OR WebKit still needs patch 2. |

---

## What was committed this session

| Artifact | Where | Purpose |
|---|---|---|
| `dist/gst145-mirror/` | local only (gitignored via `dist/`) | Build-time devel mirror: 29 dylibs + 4 include trees + 27 `.pc` files + hand-authored `gstreamer-gl-1.0.pc` + `gstreamer-mpegts-1.0.pc`. Resolves `pkg-config --modversion` to 1.4.5 across all 10 components the build needs. Regenerate from `/tmp/gst-devel-extract/` if `/tmp/` wipes; the prefix-rewrite + two hand-authored `.pc` files are the only manual steps (scriptable). |
| `patches-610/17-gstreamer-accept-1.4.5.patch` | tracked | Relaxes `find_package(GStreamer 1.10.0)` → `1.4.5` at `GStreamerChecks.cmake:13`. Forward-applies cleanly against the sl-port-610 branch HEAD. Numbering: 16 was already taken by `gstreamerregistryscanner-forward-declare-glist`; 17 was free. |
| `build_610.sh` rewiring (lines ~408-455) | tracked | Adds `GST145_MIRROR`-first branch; demotes `macports-mirror` to fallback. The two mirrors are never wired simultaneously (would mix 1.4.5 into 1.28.4, a Frankenstein). Errors out if neither mirror is found rather than silently falling back to `/opt/local/` (the original behavior caused int64/gint64 mismatch). |

The mirror is **build scaffolding**, not a deployment artifact. The
app's runtime self-containment comes from
`WebKit.app/Contents/Frameworks/10.6/gst/` (Phase 8b, not yet
implemented — depends on step 4 succeeding).

---

## Next session: the gating patch

Concrete shape of the source patch (Option 2) — **done, captured in
`patches-610/20-gstreamer-pre-1.10-type-gates.patch`**:

1. **`GRefPtrGStreamer.h:119-125`** — wrap the `GstStream` and
   `GstStreamCollection` template specializations
   (`adoptGRef`/`refGPtr`/`derefGPtr`) in `#if GST_CHECK_VERSION(1,10,0)`.
   Symmetric guard on the corresponding `.cpp` definitions in
   `GRefPtrGStreamer.cpp`.

2. **`GUniquePtrGStreamer.h:45-46`** — wrap the two converter deleters
   in `#if GST_CHECK_VERSION(1,6,0)`. (`GstVideoConverter`/`GstAudioConverter`
   are not used in any WebKit 610 code path that ships on 10.6; deleter
   gating is sufficient — no consumer code to stub.)

3. **`MediaPlayerPrivateGStreamer.{h,cpp}`** — gate the
   `updateTracks(GRefPtr<GstStreamCollection>&&)` declaration and
   definition behind `GST_CHECK_VERSION(1,10,0)`. Provide an empty
   `#else` overload or `#define` stub so callers compile. Gate the
   `GST_MESSAGE_STREAMS_SELECTED` branch in the bus handler.

4. **`TrackPrivateBaseGStreamer.{h,cpp}`** — the `GRefPtr<GstStream>`
   ctor and `GstStream* stream()` accessor need conditional
   compilation. The playbin path uses `GST_MESSAGE_STREAM_STATUS` +
   `select-stream` and does not need a `GstStream` handle.

5. **`Audio/Video/InbandTextTrackPrivateGStreamer.{h,cpp}`** — same
   pattern; the `create(..., GRefPtr<GstStream>)` overloads are
   playbin3-path entry points and can be `#if`'d out wholesale.

The bus-handler risk flagged in the prior session (whether playbin's
`select-stream` flow depends on a `GstStream` handle reaching the track
object) was verified safe: `stream()` is only called from inside
`!m_isLegacyPlaybin` branches, and the GstPad ctor path never sets
`m_stream`. Patch 20 gates the field and accessors cleanly.

---

## Next session entry point

**Step 1: Turn GL on at compile time.** Apply the one-line CMake fix
(see "Root cause" above). Verify with:

```
./build_610.sh --clean
grep -E 'DUSE_GSTREAMER_GL' build-610/build.ninja
# expect: -DUSE_GSTREAMER_GL=1 in every WebCore TU's compile command
```

If `DUSE_GSTREAMER_GL` is still absent, the fix didn't take — debug the
WEBKIT_OPTION chain before proceeding.

**Step 2: Re-measure the symbol gap against the GL-on build.** The
39-symbol list above is void. With GL on, `GLVideoSinkGStreamer.cpp`'s
body, `PlatformDisplayGStreamer.cpp`, `GStreamerVideoFrameHolder.cpp`'s
GL sections, and the `GstEGLImage` specializations all compile. Run the
symbol diff fresh:

```
nm -u $(find build-610 -name "*.o" -path "*gstreamer*") 2>/dev/null \
  | grep -oE '\b_gst_[A-Za-z_]+' | sed 's/^_//' | sort -u > /tmp/wants.txt
for lib in dist/gst145-mirror/lib/libgst*.dylib; do nm -gU "$lib"; done \
  | grep -oE '\b_gst_[A-Za-z_]+' | sed 's/^_//' | sort -u > /tmp/has.txt
comm -23 /tmp/wants.txt /tmp/has.txt
```

Expect a different and larger set. Most of it will be EGL/X11 GL backend
symbols that need Cocoa-aware gating — the consumer-port shape described
in `spikes/gstreamer-gl-investigation/README.md`.

**Step 3: Decide scope.** The GL-on build is the start of the consumer
port, not the end of the scaffolding phase. Expect a multi-session
effort. The gl-investigation spike's architecture diagram and three
planned diagnostic hooks are the entry points.

**Do not** be surprised by:
- `GstEGLImage` references that need Cocoa-aware gating (EGL is not CGL).
- `PlatformDisplayGStreamer.cpp` failing to compile on Cocoa without
  significant patching — it was written for GTK/WPE.
- `gst_gl_display_x11_new_with_display` and similar X11 symbols appearing
  in code that should be platform-gated but isn't.

These are the real consumer-port surface.
