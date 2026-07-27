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

### BASE DECISION CLOSED: 1.4.5 is the correct base. Both GL paths proven end-to-end.

**This section replaces three prior versions that were each
superseded by isolated probes. The progression is kept as a record
of how the premise was tested at each layer, because the shape of
the errors is the lesson — each conclusion looked settled until the
next probe tested the load-bearing assumption and found it wrong.**

#### The finding, finally tested at the real load-bearing boundary

The question that decided the base was never "does glupload exist as
a standalone element" — it was "can a system-memory decoder frame
become GLMemory on 1.4.5." That question was tested end-to-end with
a real H.264 decoder, not a GL producer:

```
gst-launch-1.0 -v filesrc location=/tmp/test-h264.mp4 ! queue !
  decodebin ! videoconvert ! glcolorscale !
  "video/x-raw(memory:GLMemory),format=RGBA" ! fakesink
```

Caps trace (from the macmini, against the deployed 1.4.5 framework):

```
avdec_h264-0:src:     video/x-raw, format=(string)I420        (system memory)
videoconvert0:src:    video/x-raw, format=(string)I420        (system memory)
glcolorscale0:src:    video/x-raw(memory:GLMemory), format=RGBA (GL MEMORY)
capsfilter0:src:      video/x-raw(memory:GLMemory), format=RGBA (GL MEMORY)
fakesink0:sink:       video/x-raw(memory:GLMemory), format=RGBA (GL MEMORY)
```

Pipeline prerolled, reached PLAYING, ran to EOS. **glcolorscale
accepted the system-memory I420 from avdec_h264, uploaded it
internally to GL, and emitted `video/x-raw(memory:GLMemory)` RGBA.
No glupload element needed. No 1.6+ dependency at any layer.**

#### Why glcolorscale is the glupload substitute

In GstGL 1.4.x, the system→GL upload was not a standalone element —
it was folded into the `GstGLFilter` base class. Every element that
inherits from `GstGLFilter` (glcolorscale, glfiltercube, gleffects,
glfilter*) uploads its system-memory input to a GL texture before
processing, and can emit `video/x-raw(memory:GLMemory)` on output.
In GStreamer 1.6, this embedded upload was refactored out into the
standalone `glupload` element — a restructuring of capability that
1.4.5 already had, not new capability.

WebKit's stock `GLVideoSinkGStreamer.cpp:73-77` hardcodes
`gst_element_factory_make("glupload")` — which fails on 1.4.5 because
the standalone element doesn't exist. But the *capability* it
represents (system→GLMemory upload) is present in glcolorscale and
every other GstGLFilter-derived element. The stock wiring was the
wrong import; the base was never the problem.

#### The two-level error, named so it's not repeated

**Level 1 (sink construction):** "glupload factory_make returns NULL
→ the sink can't construct → 1.4.5 can't host the GL path." The
inference from sink-construction failure to base-insufficiency was
the first error. The appsink-GLMemory probe (gltestsrc → appsink)
disproved it: GLMemory reaches appsink on 1.4.5 when a GL element
is upstream. The sink construction was a wiring issue, not a base
issue.

**Level 2 (upload capability):** "avdec_h264 produces system memory
→ GLMemory can't reach appsink from the decode path without
glupload → 1.6.4 is required." This was the second error, caught by
the glcolorscale probe. avdec_h264 DOES produce system memory (Probe
A confirmed), but glcolorscale accepts system memory in and emits
GLMemory out (Check 2 confirmed). The upload capability exists on
1.4.5 — embedded in GstGLFilter, not exposed as glupload.

Both errors had the same shape: a premise looked settled because the
examined factor (element availability, decoder output type) pointed
one way, while the unexamined factor (appsink's caps acceptance,
glcolorscale's embedded upload) pointed the other. Each was caught
by an isolated probe testing the actual load-bearing assumption
rather than a proxy.

#### Two proven paths — the wiring choice for next session

Both deliver accelerated video on 1.4.5 + 10.6.8. The choice between
them is a design decision, not a typing task, and should be probed
specifically: which one hands the IOSurface bridge its texture more
cleanly.

**Path A: glimagesink + other-context + client-draw**

  - `playbin video-sink = glimagesink`.
  - Set `other-context` = fix #1's wrapped CGL/NSOpenGL context.
  - Connect `client-draw` signal handler.
  - glimagesink uploads system memory to GL internally, calls
    client-draw with the uploaded texture.
  - Handler binds texture to IOSurface via CGLTexImageIOSurface2D,
    sets CALayer.contents.
  - Proven: Probe D showed `decodebin → glimagesink` plays to EOS
    with avdec_h264's system-memory I420 input. glimagesink's
    other-context + client-draw properties confirmed via
    gst-inspect-1.0.
  - Trade-off: texture arrives in a draw callback (GL thread
    context), lifetime tied to the callback's scope. Threading
    must be handled carefully.

**Path B: glcolorscale → appsink(GLMemory) + IOSurface bridge**

  - `playbin` with a custom sink bin:
    `glcolorscale → appsink` with caps
    `video/x-raw(memory:GLMemory),format=RGBA`.
  - glcolorscale uploads system memory to GLMemory (the glupload
    substitute).
  - appsink receives GstGLMemory buffers.
  - IOSurface bridge pulls the GstGLMemory, binds its texture to
    IOSurface via CGLTexImageIOSurface2D, sets CALayer.contents.
  - Proven: Check 2 showed `decodebin → videoconvert → glcolorscale
    → appsink(GLMemory)` delivers GLMemory end-to-end from a real
    H.264 decode path.
  - Trade-off: texture arrives as a pulled GstGLMemory (caller's
    thread, main thread in WebKit's case), lifetime tied to the
    sample. More natural fit for the existing
    MediaPlayerPrivateGStreamerIOSurface.mm design.

Both paths use the same primitives already proven this session:
fix #1's wrapped CGL context, the `CGLTexImageIOSurface2D` IOSurface
bind, Core Animation compositing. The choice is about which
integration shape (callback vs pull) feeds the IOSurface bridge
more cleanly — a probe-first decision for next session.

#### Everything transfers

  - Fix #1 (wrap-only Cocoa GstGL context, commit `f569a3ac90`):
    the wrapped context on the GeForce 9400M is the share parent
    for both paths. Verified at runtime
    (`renderer=0x102260e (GEFORCE)` in the launch log).
  - Version-gate recovery layers 1-4 (`5c96cb01d4`, `f09061ea47`,
    `9f64bf7cc1`, `e1ac2dc3c6`): compile-correct on 1.4.5. Some
    gates (GstStream 1.10, Video/AudioConverter 1.6) would be
    unnecessary on a 1.6+ base, but on 1.4.5 they're required and
    correct.
  - Platform gates (EGL/TextureMapper): correct for Cocoa where
    neither EGL nor TextureMapper-GL is the present path.
  - IOSurface bridge code (`MediaPlayerPrivateGStreamerIOSurface.mm`):
    the CGLTexImageIOSurface2D bind is the present primitive for
    both paths. The texture-source side changes (from the stock
    GLVideoSinkGStreamer to either glimagesink+client-draw or
    glcolorscale→appsink), but the bind-to-IOSurface side stays.
  - Phase2 dirty-tree guard (`fed62360`): prevents the data-loss
    class of error from recurring.
  - MacPorts hygiene: disabled, reversible.

### Repo locations (for whichever way the probe resolves)

If 1.4.5 stands (probe TRUE): no port needed. The current
`/Users/macbookpro/gst-plugins-bad` snowleopard-1.4.5 tree, the
deployed framework, and all WebKit-side recovery work stay as-is.
WebKit changes to drop `GLVideoSinkGStreamer` and route through
appsink with GLMemory caps + the existing IOSurface bridge.

If 1.6.4 is required (probe FALSE): the patched source tree lives
at `/Users/macbookpro/gst-plugins-bad` on branch `snowleopard-1.4.5`
with six `[leopard]` commits on top of upstream `Release 1.4.5`
(`faed71d32`):

  `2a8297477` patch Cocoa GL backend for 10.6 SDK compatibility + diagnostics
  `0487014ea` add build-snowleopard.sh for reproducible 10.6 cross-compile
  `8b8760a20` fix K&R declaration in gstglfilterglass.c for modern clang
  `14222aa88` guard gstgltransformation.c with HAVE_GRAPHENE
  `f9b34e9ac` build-snowleopard.sh: add plugin build steps + all overrides
  `9fef0c937` gitignore ORC-generated bad-video-orc.h

Tags `1.6.0` through `1.6.4` are present in the same repo. The
1.6.4 tag contains `ext/gl/gstglcolorconvertelement.c`,
`gst-libs/gst/gl/gstglcolorconvert.c`, `gst-libs/gst/gl/gstglupload.c`.
Port path: `git checkout -b snowleopard-1.6.4 1.6.4`, cherry-pick the
six `[leopard]` commits, build, deploy.

**Do not confuse with** `/Users/macbookpro/gstreamer` — that's the
`gstreamer` CORE module clone (separate repo, no GstGL content, no
leopard patches; its `snowleopard-1.4.5` branch == upstream 1.4.5
tag).

### Share-group victory + render-to-IOSurface wall (probe results)

**The share-group works.** Verified at three ascending levels:

  1. `glIsTexture(glcolorscale_tex_id)` in wrapped context = TRUE.
  2. Both contexts on the same renderer (0x102260e, GeForce 9400M).
  3. `glGetTexImage` reads glcolorscale's texture in the wrapped context
     with pixel-correct results (SMPTE75 pattern: TL=0xffc0bec0,
     TR=0xff0100c1 — gray and blue, matching videotestsrc's output).

**Two fixes were required to establish the share-group:**

  1. **`other-context` property, not GstContext propagation.** On
     GstGL 1.4.5, `gst_element_set_context` stores a GstContext on
     the element but does NOT populate `filter->other_context`
     (gstglfilter.c:178-180), which is what `gst_gl_context_create`
     uses as the share parent (gstglfilter.c:893-894). Only the
     GObject property setter populates `other_context`. The property
     is the SAME mechanism as glimagesink's `other-context`.
     GstContext propagation (even brute-force on all children) does
     not reach this code path on 1.4.5.

  2. **Pixel format must match GstGL's Cocoa backend exactly.**
     GstGL's Cocoa backend uses `DoubleBuffer + AccumSize=32`
     (gstglcontext_cocoa.m:228-231). Any deviation (ColorSize=24,
     AlphaSize=8, Accelerated, NoRecovery) causes "invalid share
     context" via Apple's pixel-format-compatibility sharing rules.
     The fix #1 wrapped context (`CocoaGstGLContextHelper.mm`) must
     be updated to use this pixel format. This loses the
     `kCGLPFAAccelerated + kCGLPFANoRecovery` hardware pinning, but
     on a machine with a discrete GPU (9400M), the default renderer
     resolves to hardware anyway — verified by the renderer-ID log.

**But render-to-IOSurface-texture does NOT work on the 9400M's GL 2.1
driver.** This is the third hardware limitation the 9400M has imposed:

  1. GL 3.2 context profiles (kCGLPFAOpenGLProfile is 10.7+).
  2. IOSurface-FBO-readback quirk (FBO readback produces false-black
     that direct IOSurface readback doesn't — commit 18456f36).
  3. **GL_BGRA internal-format textures are not FBO-renderable.**
     `CGLTexImageIOSurface2D` on 10.6 creates the texture with
     GL_BGRA internal format (matching the IOSurface's BGRA pixel
     layout). On the 9400M's GL 2.1 driver, this internal format
     cannot be used as an FBO color attachment
     (`GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT`). Both
     `gst_gl_memory_copy_into_texture` (which internally uses an
     FBO-style copy) and a manual FBO blit produce no output
     (texture exists but rendering to it fails silently).

The pattern across all three: the 9400M can SAMPLE IOSurface textures
(read) and can BIND them (glIsTexture=TRUE), but its render-to-IOSurface
path is broken or absent at every turn. This is starting to look like
a hardware/driver limitation, not a fixable bug.

**The fallback copy is CPU-mediated: `glGetTexImage → memcpy → IOSurface`.**
The source texture (glcolorscale's output) is fully readable in the
wrapped context via `glGetTexImage`. The pixels can be copied to the
IOSurface's base address via `memcpy`. CALayer then composites the
IOSurface. But this is a GPU→CPU→IOSurface round-trip per frame —
NOT zero-copy.

### Benchmark verdict: GL+readback wins by 35% at 720p (PATH IS WORTH WIRING)

The zero-copy premise died (render-to-IOSurface broken on the 9400M's
GL 2.1 driver). The fallback is a per-frame GPU→CPU readback via
`glGetTexImage`. The question was whether the GPU colorspace conversion
savings outweigh the readback cost. Answer: **yes, decisively.**

**Readback measurement** (three methodologies, to rule out glFinish
deflation and confirm the result is robust):

  | Methodology | Readback | Notes |
  |---|---|---|
  | A) Isolated, no glFinish (static texture) | 1.30 ms | Pure transfer |
  | B) Realistic (GPU work + glFlush + readback) | **1.98 ms** | Real per-frame stall |
  | C) Original (glFinish + readback) | 1.34 ms | Slightly deflated |

  The realistic measurement (B) captures the real per-frame cost: GPU
  work pending (simulating glcolorscale's conversion) → glFlush →
  immediate glGetTexImage without glFinish. The 0.64ms difference
  between B and C is the pipeline stall cost that glFinish was hiding.
  All three methodologies agree within 0.7ms and all show GL winning.

**Conversion throughput** (gst-launch, 100 frames, sync=false, 720p):

  | Path | Time | FPS | Per-frame |
  |---|---|---|---|
  | CPU (videoconvert I420→RGBA) | 1.859s | 53.8 | 18.6 ms |
  | GL (glcolorscale, GPU) | 1.017s | 98.3 | 10.2 ms |

**Total per-frame cost comparison:**

  - GL path: glcolorscale 10.2ms + readback 1.98ms = **12.2 ms** (realistic)
  - CPU path: videoconvert = **18.6 ms**
  - **GL wins by 6.4ms (35%)**

The readback is 8% of the CPU path's per-frame budget — negligible
compared to the 8.4ms conversion savings. At 60fps (16.7ms budget),
only the GL path fits; CPU conversion alone exceeds the budget.

The slope between 480p and 720p was not separately measured, but the
readback cost scales with pixel count (linear on this driver based on
the 1.48ms at 3.5MB), while the GPU conversion advantage scales
similarly. The GL path's margin is expected to widen, not narrow,
at higher resolutions.

**Pre-committed decision rule applied**: "wash goes to CPU (simpler,
already deployed)." This is not a wash — 37% is a decisive margin.
The GL path is worth wiring.

The zero-copy premise that justified the entire GL arc just died:
render-to-IOSurface doesn't work on the 9400M, so the path must
round-trip through CPU. The question is whether glcolorscale's GPU
colorspace conversion (I420→RGBA) saves more than the per-frame
`glGetTexImage` readback costs. On GL 2.1 hardware, GPU readback is
the slowest path — it may be slower than doing the colorspace
conversion on CPU in the first place.

**Patch 26's existing CPU path**: GStreamer decodes (CPU) →
videoconvert converts I420→RGBA (CPU) → ImageGStreamer uploads to
CGImage → CALayer. No GPU involvement.

**The new GL+readback path**: GStreamer decodes (CPU) → videoconvert
I420→RGBA (CPU) → glcolorscale converts RGBA→GL texture (GPU) →
glGetTexImage reads back (GPU→CPU) → memcpy to IOSurface → CALayer.

The new path adds a GPU upload + a GPU→CPU readback on top of the
CPU path. For it to be a win, the GPU colorspace conversion must
outweigh the readback stall. At 720p (1280×720×4 = 3.5MB/frame),
the readback is non-trivial.

**The thirteenth probe** (not yet run): benchmark both paths at
720p, measure sustained fps. The result decides whether the last
fifteen turns of GL investigation wire into WebKit or become the
proof that patch 26's CPU path was already the ceiling for this
hardware.

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

**[base decision CLOSED: 1.4.5]** The "1.4.5 was the right target"
framing below survived the entire session through ten premise tests.
Two intermediate conclusions ("can't host the sink → reopen to
1.6.4", then "appsink handoff proven but decoder memory type open")
were each disproved by isolated probes — see "BASE DECISION CLOSED"
in the Production reality section for the full caps trace. The
finding that actually closed it: `glcolorscale` on 1.4.5 accepts
avdec_h264's system-memory I420 and emits `video/x-raw(memory:GLMemory)`
RGBA — the embedded upload that the 1.6 `glupload` element was a
refactoring of, not new capability. Both GL paths (glimagesink +
client-draw, glcolorscale → appsink + IOSurface bridge) are proven
end-to-end with a real H.264 decoder. No 1.6.4 needed. All session
work transfers. The text below is preserved as the historical 1.4.5
investigation record.

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

---

## YouTube / MSE Assessment — July 2026

### What works (banked milestone)

- **Progressive web video over HTTPS** — proven end-to-end (Big Buck Bunny
  from test-videos.co.uk, H.264, played through the IOSurface/CALayer
  accelerated render path)
- **GStreamer 1.4.5 fully integrated** — deadlock fixed (wrap-only Cocoa
  GstGL context), all codecs available (H.264, VP8, VP9, AAC, Opus, Vorbis)
- **IOSurface/CALayer render path** — CPU fallback sink → ImageGStreamer →
  CGImageRef → IOSurface → CALayer.contents → Core Animation GPU compositing
- **MSE compiles and initializes** — `window.MediaSource` is defined,
  `isTypeSupported` returns correct values for all codecs (H.265=false,
  H.264 all profiles=true, VP9=true, AAC=true)

### What YouTube needs (scoped out — requires base change)

YouTube requires MSE append-pipeline behavior that is stacked across ≥3
pipeline layers, one of which is not patchable WebKit-side on 1.4.5:

1. **EOS-instead-of-updateend signaling** (WebKit append-glue) — the
   AppendPipeline signals stream-end after the init segment instead of
   "segment complete, ready for next append." YouTube's JS never receives
   `updateend`, waits ~10s, gives up, clears tracks. Patchable in WebKit.

2. **`gst_base_sink_set_drop_out_of_segment` is a 1.6 API** (GStreamer
   base-library gap) — MSE needs the appsink to keep out-of-segment
   buffers (media fragments arrive with new segment boundaries). On 1.4.5,
   the base sink hardcodes dropping them with no override. Not patchable
   WebKit-side — would require forking `gstbasesink.c` in libgstbase.

3. **Video appends entirely unproven** — trace showed only audio (mp4a)
   qtdemux instances in the MSE path. Zero video qtdemux instances.
   May be explained by #1/#2 or may be a separate issue.

4. **WebKitMediaSrc dynamic-pad READY→PAUSED stall** — playbin can't
   transition from READY to PAUSED with WebKitMediaSrc as source.
   WebKitMediaSrc starts with no pads; pads are added dynamically when
   tracks are attached (`attachTrack`). On 1.4.5, playbin doesn't
   re-negotiate after dynamic pad addition from a custom source.
   Confirmed: need-data callback fires 0 times (ordering deadlock, not
   negotiation failure). The appsrc elements in WebKitMediaSrc are never
   fed, decodebin never activates, pipeline never prerolls. YouTube
   retries 3× (creating new MediaSource each time), same stall each cycle.
   91 append completions confirmed — the AppendPipeline works. The wall
   is in the PlaybackPipeline's playbin interaction.

5. **YouTube serves VP9+Opus** — tracks attached as `video/x-vp9` and
   `audio/x-opus`. VP9 decoder (vp9dec) IS available in the GStreamer
   1.4.5 framework, so the codec isn't the wall — the pipeline stall (#4)
   is. But YouTube's codec selection is worth noting for the 1.6.4
   project: verify VP9 and Opus decoders are included in the new build.

### Why 1.6.4 (not 1.4.5-forked) is the path to YouTube

The MSE walls are precisely the things GStreamer 1.6 fixed:
- `gst_base_sink_set_drop_out_of_segment` (native in 1.6)
- `gst_app_sink_try_pull_sample` (native in 1.6)
- `gst_flow_combiner_update_pad_flow` (native in 1.6)
- Better fMP4 append-mode handling in qtdemux

Moving to 1.6.4 would require re-porting the entire proven stack:
Cocoa GL backend, deadlock fix, share-group pixel-format matching,
IOSurface bridge, all version gates. That's a deliberate separate project.

### Decision

Progressive accelerated web video is the deliverable. YouTube is scoped
out with a precise map of what it would take, so the work can be picked
up from the map rather than rediscovered.

