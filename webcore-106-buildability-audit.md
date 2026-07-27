# WebCore Buildability Audit for macOS 10.6
## `safari-605.2.8.1-branch` — Source/WebCore on Snow Leopard x86_64

**Audit Date:** June 2026
**Scope:** Buildability of `Source/WebCore` against a public macOS 10.6 SDK. Every claim below was read from source at commit `638989847985af550947c53986514adebf9f0d02`.

---

## 0. Headline

WebCore is **buildable on 10.6 with patches**:

- **Favorable:** `Source/WTF/wtf/Platform.h` degrades gracefully on old OS versions. A 10.6 deployment target auto-selects the QTKit media path, disables `os_log`, disables modern Security/crypto features, and avoids the Apple Internal SDK — all in the correct direction, with no hard `#error` floor on OS version. C++17 `std::optional` is supplied by a WTF backport, so a `-std=c++14` toolchain is sufficient.
- **One compile break:** `ENABLE_WEBGPU` defaults **ON** for the Mac port, and the 22 WebGPU/Metal source files are compiled unconditionally. At least one (`GPUDeviceMetal.mm`) imports `<Metal/Metal.h>` outside its `#if ENABLE(WEBGPU)` guard, and Metal does not exist before 10.11. This is a direct compile break with a clean exclusion fix.

Net: WebCore is buildable with three patches plus two runtime watch-items.

---

## 1. Build-System Mechanics

### 1.1 Unified Sources Are Authoritative
- `Source/WebCore/CMakeLists.txt:14` registers `Sources.txt`; `PlatformMac.cmake:24` appends `SourcesCocoa.txt` and `SourcesMac.txt` to `WebCore_UNIFIED_SOURCE_LIST_FILES`. `CMakeLists.txt:1920` calls `WEBKIT_COMPUTE_SOURCES(WebCore)`.
- Source counts: `Sources.txt` ≈ 2,995 entries, `SourcesCocoa.txt` 335, `SourcesMac.txt` 90. WebCore is a ~3,400-file compile — large, but expected and not a blocker.

### 1.2 The Ruby Generator
- `Source/WTF/Scripts/generate-unified-source-bundles.rb` requires only stdlib (`fileutils`, `pathname`, `getoptlong`) with no version-sensitive syntax. It runs on the build host, never on the 10.6 target, so the host's system Ruby (2.6+/3.x) satisfies it. No action needed.

### 1.3 Dual Source Listing — Watch Item
- The 22 WebGPU/Metal files appear in both the unified manifest `SourcesCocoa.txt` (e.g. `GPUDeviceMetal.mm` at line 246, no `@no-unify`) and the direct-compile list `list(APPEND WebCore_SOURCES …)` in `PlatformMac.cmake:147–475` (Metal files at lines 310–330). 199 source paths overlap between the two in total.
- Any exclusion of these files must be applied in both locations to avoid either a duplicate-symbol link error or an incomplete exclusion. See Patch 1.

---

## 2. The Metal/WebGPU Compile Break

### 2.1 The Cause
- `Source/cmake/OptionsMac.cmake:44`: `WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_WEBGPU PRIVATE ON)` — WebGPU is ON by default for Mac, even though the base feature default in `WebKitFeatures.cmake:179` is OFF.
- `PlatformMac.cmake:14`: `find_library(METAL_LIBRARY Metal)`; line 42 adds `${METAL_LIBRARY}` to `WebCore_LIBRARIES` unconditionally. (Harmless on its own: on a 10.6 SDK `find_library` returns `NOTFOUND`, the variable expands to empty, and the linker receives nothing.)
- `PlatformMac.cmake:310–330`: 21 `GPU*Metal.mm` files plus `WebGPULayer.mm` (line 345) are listed in the `WebCore_SOURCES` direct-compile block — not wrapped in `if (ENABLE_WEBGPU)`.

### 2.2 Why It Breaks
Most files guard their bodies with `#if ENABLE(WEBGPU)`, so with the feature off they compile to near-empty translation units — except `GPUDeviceMetal.mm`, where the import order is wrong:

```objc
// Source/WebCore/platform/graphics/cocoa/GPUDeviceMetal.mm
26  #import "config.h"
27  #import "GPUDevice.h"
29  #import "Logging.h"
30  #import "WebGPULayer.h"
32  #import <Metal/Metal.h>     // unguarded: processed regardless of ENABLE(WEBGPU)
33  #import <runtime/ArrayBuffer.h>
34  #import <wtf/BlockObjCExceptions.h>
36  #if ENABLE(WEBGPU)          // guard starts after the Metal import
...
83  #endif
```

On a 10.6 SDK there is no `<Metal/Metal.h>` (Metal is 10.11+), so the preprocessor fails with `'Metal/Metal.h' file not found` before the guard at line 36 applies. This is a hard compile error, independent of the `${METAL_LIBRARY}` link question.

### 2.3 Fix
Exclude these files on 10.6 (Patch 1), which is also correct functionally (no Metal, no WebGPU).

---

## 3. Platform.h Degrades Gracefully

`Source/WTF/wtf/Platform.h` was audited for OS-version assumptions.

### 3.1 No Hard OS Floor
The only `#error` directives are for unknown endianness (415/432/447/454), unsupported pointer width (725), and JIT/assembler misconfiguration (996/1006). There is no `#error` enforcing a minimum macOS version; a 10.6 deployment target is not rejected.

### 3.2 Version Gates Run in the Safe Direction
Modern features are gated ON only for newer OS, leaving 10.6 on the legacy path:

| Platform.h line | Gate | Effect on 10.6 |
|---|---|---|
| 575 | `#if __MAC_OS_X_VERSION_MIN_REQUIRED < 101200` → `#define USE_QTKIT 1` | QTKit media path auto-selected instead of AVFoundation |
| 1255 | `>= 101200` → `USE_OS_LOG 1` | `os_log` (10.12+) disabled on 10.6 |
| 1262 | `>= 101200` → `HAVE_SEC_TRUST_SERIALIZATION 1` | disabled on 10.6 |
| 1312 | `>= 101300` → `HAVE_RSA_PSS 1` | disabled on 10.6 |
| 1320 | `>= 101300` → `HAVE_AVCONTENTKEYSESSION 1` | disabled on 10.6 |

In WebCore's `platform/` tree, the 45 version-gate sites are predominantly `#if __MAC_OS_X_VERSION_MIN_REQUIRED < 101300` style — the compat path is taken when older than 10.13, which is what a 10.6 build wants (e.g. `WebPlaybackControlsManager.mm:137`, `DragImageMac.mm:299/310`).

### 3.3 Apple Internal SDK Is Auto-Avoided
`Platform.h:505–508`:
```c
#if PLATFORM(COCOA)
#if defined __has_include && __has_include(<CoreFoundation/CFPriv.h>)
#define USE_APPLE_INTERNAL_SDK 1
#endif
#endif
```
A public 10.6 SDK has no `<CoreFoundation/CFPriv.h>`, so `USE_APPLE_INTERNAL_SDK` stays undefined and the open-source code paths are selected automatically. No patch needed. (The Xcode build, by contrast, hardcodes `SDKROOT = macosx.internal` and cannot be used.)

---

## 4. C++ Standard Library

WebCore uses `std::optional`/`std::nullopt` (C++17) pervasively, but the build is `-std=c++14`. This is reconciled by a backport:

- `Source/WTF/wtf/Optional.h` defines `std::optional` inside `namespace std` (the Krzemieński `std::experimental::optional` reference implementation, adapted): `namespace std {` at line 141, specializations through line 1092.
- No C++17 toolchain is required. A cross-compiling clang new enough for `-std=c++14` satisfies WebCore.

---

## 5. Framework Availability on 10.6

All frameworks `find_library`'d by `WebCore/PlatformMac.cmake`:

| Framework | 10.6? | Notes |
|---|---|---|
| Accelerate, ApplicationServices, AudioToolbox, AudioUnit, Carbon, CFNetwork, Cocoa, CoreAudio, CoreServices, DiskArbitration, IOKit, OpenGL, Quartz, QuartzCore, Security, SystemConfiguration, XML2 | ✅ | All present on 10.6 |
| **Metal** | ❌ 10.11+ | `find_library` → NOTFOUND → empty link var (harmless); the source compile is the problem (§2) |
| **AVFoundation** | ⚠️ 10.7+ on Mac | Safe only because sources are guarded by `#if ENABLE(VIDEO) && USE(AVFOUNDATION)` and `ENABLE_VIDEO=OFF` (`MediaPlayerPrivateAVFoundationObjC.mm:29` guards the AVFoundation import at line 91) |
| **IOSurface** | ⚠️ | Existed on 10.6 with limited public API; umbrella linkage is tolerant |
| **DataDetectorsCore**, **Lookup** | ❌ private, 10.7+/10.8+ | Both guarded by `if (NOT …-NOTFOUND)` in `PlatformMac.cmake:59–67` — added to the link list only if found, so a 10.6 SDK omits them. No patch needed. |

The DataDetectorsCore/Lookup pattern (`find_library … HINTS /System/Library/PrivateFrameworks` then conditional append) is the correct defensive idiom and is already in upstream. Metal is the one framework that lacks this guard.

---

## 6. Other Surfaces

| Surface | Finding | Action |
|---|---|---|
| **SQLite version** | `SQLiteDatabase.cpp:63` guards `sqlite3_errstr` behind `#if SQLITE_VERSION_NUMBER >= 3007015` (3.7.15 ≈ 10.10). On 10.6's SQLite 3.6.x the guard takes the fallback log path. | None — handled |
| **NSURLSession** | `WebCoreNSURLSession` is a WebKit wrapper class (`: NSObject`, holds a `PlatformMediaResourceLoader`), not the system `NSURLSession` (10.9+). Media-loader plumbing, guarded with media features. | None for non-video build |
| **os_log** | Not used directly in WebCore `platform/`; gated by `USE(OS_LOG)`, OFF on 10.6. | None — handled |
| **bmalloc Gigacage** | `Gigacage.h:58` enables on `(DARWIN)` || LINUX) && X86_64`, unconditionally for Darwin x86_64. Uses `madvise(MADV_FREE_REUSABLE)` (`VMAllocate.h:200`), an API present since 10.6. | Runtime watch-item; expected to work, test on hardware |
| **x86_64 JIT/assembler** | `Platform.h:586` `CPU(X86_64)` block present; full JIT support for x86_64. | None — supported |
| **MetalKit / CAMetalLayer** | Zero references in WebCore `platform/`. Only `<Metal/Metal.h>` appears, exclusively in the WebGPU GPU\*Metal files. | Covered by Patch 1 |

---

## 7. Required Patches

### Patch 1 — Exclude WebGPU/Metal Sources (REQUIRED, blocking)
Force `ENABLE_WEBGPU=OFF` and ensure the sources are not compiled. Because the files are listed in two places, both must be addressed.

**1a. Force the feature off** (add to `OptionsMac.cmake` with the other Mac feature overrides):
```cmake
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_WEBGPU PRIVATE OFF)
```

**1b. Guard the direct-compile list** in `Source/WebCore/PlatformMac.cmake`. The Metal entries sit inside the single large `list(APPEND WebCore_SOURCES …)` spanning 147–475; split that block so the GPU\*Metal entries (lines ~310–330) and `WebGPULayer.mm` (line 345) are conditionally appended:
```cmake
if (ENABLE_WEBGPU)
    list(APPEND WebCore_SOURCES
        platform/graphics/cocoa/GPUBufferMetal.mm
        # … all 21 GPU*Metal.mm …
        platform/graphics/cocoa/WebGPULayer.mm
    )
endif ()
```

**1c. Remove the unified entries** for the same files from `Source/WebCore/SourcesCocoa.txt` (lines listing `platform/graphics/cocoa/GPU*Metal.mm` and `WebGPULayer.mm`), since the unified generator does not see the CMake `if()`.

**1d. (Defensive) Guard the Metal link entry** in `PlatformMac.cmake:42` — harmless if left (expands empty) but cleaner to gate:
```cmake
if (ENABLE_WEBGPU)
    list(APPEND WebCore_LIBRARIES ${METAL_LIBRARY})
endif ()
```

**Alternative:** instead of excluding, fix the single import-order bug by moving `#import <Metal/Metal.h>` in `GPUDeviceMetal.mm` to after the `#if ENABLE(WEBGPU)` on line 36. This makes all 22 files compile to empty TUs with the feature off and avoids touching the source lists. Exclusion is more robust because it also drops the dead object files.

### Patch 2 — Nothing Else Required at the WebCore Level
Platform.h, the `std::optional` backport, framework guards (except Metal), SQLite, and os_log all degrade correctly without intervention. The `ENABLE_VIDEO=OFF` / `ENABLE_WEB_AUDIO=OFF` flags already neutralize the AVFoundation surface.

---

## 8. Runtime Watch-Items (Not Compile)

- **bmalloc Gigacage** virtual-memory reservation on 10.6 x86_64 (`madvise(MADV_FREE_REUSABLE)` is 10.6-era; expected to work, but verify the large VM reservation succeeds under the 10.6 VM subsystem).
- **Metal/WebGPU final link** — after Patch 1, confirm the final link has no duplicate `GPU*Metal` symbols (the dual source listing in §1.3) and no unresolved Metal references.
- **Carbon `TECGetWebTextEncodings`** (used by `webDefaultCFStringEncoding`) — present on 10.6, deprecated; confirm it links.
- **`pthread_set_fixedpriority_self`** — not in the legacy layer; referenced by WTF. Provide the stub at the WTF/bmalloc layer if the linker reports it.

---

## 9. Bottom Line

WebCore is buildable on 10.6 because WebKit's own version-gating was written to degrade on old systems. The one genuine compile break (Metal/WebGPU, ON by default for Mac) has a clean three-part exclusion patch. The remaining WebCore risk is runtime, not compile: bmalloc's address-space reservation and final-link symbol/dependency resolution, both of which can only be settled on real 10.6 hardware or a faithful VM.

---

*Every file path, line number, and cmake value above was read from upstream WebKit source at commit `638989847985af550947c53986514adebf9f0d02`. Runtime-only concerns are labeled and deferred to on-hardware testing rather than asserted.*
