# WebKit 610's GStreamer header floor

Spike determining the minimum GStreamer devel set against which WebKit
610's `Source/WebCore/platform/graphics/gstreamer/` compiles
unmodified, and what that floor implies for the bundled-runtime
strategy.

**Status:** floor located at **GStreamer ≥ 1.10** (header level). Both
1.4.5 and 1.6.5 runtime targets are ruled out as compile targets unless
WebKit source is patched. Scaffolding committed; source-gating patch
deferred to a fresh session.

---

## TL;DR

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

Concrete shape of the source patch (Option 2), so the next session opens
on the real work rather than re-deriving it:

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

6. **Verify** by grepping the post-patch source for any remaining
   unconditional `GstStream`/`GstStreamCollection`/`GstVideoConverter`/
   `GstAudioConverter` references; iterate until clean.

7. **Run the clean rebuild.** Expect `GLVideoSinkGStreamer.cpp.o`,
   `PlatformDisplayGStreamer.cpp.o`, `VideoTextureCopierGStreamer.cpp.o`
   to compile. Verify `otool -L WebCore` shows
   `/Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstgl-1.0.0.dylib`
   linked (previously absent) and no `/opt/local/lib/` paths.

**Risk to watch during the gate:** the `TrackPrivateBaseGStreamer`
ctor signature is on a hot code path (every track created for every
media element goes through it). Verify the playbin path's
`select-stream` flow doesn't secretly depend on a `GstStream` handle
reaching the track object — if it does, the gate has to preserve the
handle as `void*` or a forward-declared opaque type rather than eliding
the field. Read `MediaPlayerPrivateGStreamer`'s bus handler around the
`GST_MESSAGE_STREAM_STATUS` / `select-stream` call sites before writing
the gate.
