# WebKit 604.5.6 Build Reference — Mac OS X 10.6 Snow Leopard

Cross-compiled on macOS (Darwin 25.5.0) targeting **x86_64** with `__MAC_OS_X_VERSION_MIN_REQUIRED = 1060`.

SDK: phracker/MacOSX-SDKs (10.6 SDK via `-syslibroot`).

---

## Frameworks Produced

| Framework | Size | Exported Symbols |
|---|---|---|
| JavaScriptCore.framework | 40M | — |
| WebCore.framework | 64M | — |
| WebKit.framework | 8.7M | 15,905 |
| WebKitLegacy.framework | 216K | 316 |

**Total: ~113M**

`WebKit.framework` is the modern name for what the Xcode build system calls `WebKit2`. Same code, same exports.

## Static Libraries

| Library | Purpose |
|---|---|
| libWTF.a | WebKit Template Framework |
| libbmalloc.a | Bump-pointer allocator |
| libPAL.a | Platform Abstraction Layer |
| libANGLESupport.a | ANGLE (WebGL) support |
| libWebCoreDerivedSources.a | Generated WebCore sources |

## Dynamic Libraries

| Library | Purpose |
|---|---|
| libWebCoreTestSupport.dylib | WebCore test helpers |
| libgtest.dylib | Google Test framework |

## Executables

| Binary | Size | Purpose |
|---|---|---|
| jsc | 320K | JavaScript REPL/shell |
| testapi | 304K | JSC API tests |
| testb3 | 24K | B3 JIT tests |
| testair | 24K | Air JIT tests |
| LLIntOffsetsExtractor | 3.0M | JSC internal tool |

---

## WebGL — Fully Compiled

| Component | Status | Details |
|---|---|---|
| `ENABLE_WEBGL` cmake flag | `ON` | WebGL 1 enabled |
| `ENABLE_WEBGL2` cmake flag | `ON` | WebGL 2 enabled |
| ANGLE (WebGL → OpenGL translator) | ✅ | 113 `.o` files → `libANGLESupport.a` (2.6M) |
| `ANGLEWebKitBridge` | ✅ | Bridges ANGLE to WebCore |
| `GraphicsContext3D` (OpenGL backend) | ✅ | 5 `.o` files (OpenGL, Cocoa, CG paths) |
| WebGL 1 implementation | ✅ | `WebGLRenderingContext`, `WebGLRenderingContextBase` |
| WebGL 2 implementation | ✅ | `WebGL2RenderingContext` |
| WebGL object types | ✅ | Buffer, Texture, Framebuffer, Renderbuffer, Shader, Program, Uniform, etc. |
| WebGL extensions | ✅ | Compressed textures (ATC, PVRTC, S3TC), depth texture, draw buffers, vertex array objects |
| WebGL JS bindings (generated) | ✅ | 29 JS binding `.o` files |
| **Total WebGL artifacts** | **67 `.o` files** | |

WebGL uses ANGLE (Almost Native Graphics Layer Engine) to translate WebGL/GLES calls to desktop OpenGL. The OpenGL backend (`GraphicsContext3DOpenGL.cpp`, `GraphicsContext3DOpenGLCommon.cpp`) provides the actual GPU rendering path. On 10.6, this routes through the system OpenGL framework which is fully available.

---

## Third-Party Libraries

| Library | Our Build | Notes |
|---|---|---|
| ANGLE | ✅ libANGLESupport.a | WebGL → OpenGL translation |
| brotli | ✅ compiled into WebCore | Font decompression |
| woff2 | ✅ compiled into WebCore | Font compression |
| xdgmime | ✅ compiled into WebCore | MIME type detection |
| gtest | ✅ libgtest.dylib | Test infrastructure |

### Not Included (absent from both our build AND the Leopard PPC binary)

These three libraries are listed in the Leopard PPC patch's build configs (`Source/Makefile` MODULES, `.xcconfig` linker flags) but **are not present in the actual PPC binary** either. Symbol analysis of the PPC `WebCore.framework` (84MB, ppc_970 + ppc_7400, 22,390 exported symbols) confirms zero `lz4`, `ots::`, or `libwebrtc` symbols. The build system references them but they are conditionally excluded or stubbed out for PowerPC.

#### lz4 — Extremely Fast Lossless Compression

- **What it is**: A BSD-licensed compression library providing ~400 MB/s compression and multi-GB/s decompression.
- **WebKit usage**: Referenced in `WebCore.xcconfig` (`-llz4`) alongside brotli and woff2. The Xcode build system lists it as a dependency of WebCore.
- **Why not in either build**: Zero `lz4` symbols found in the PPC WebCore binary — it was never linked. The cmake build configs (`CMakeLists.txt`, `PlatformMac.cmake`) also contain zero references to lz4. No WebCore or WTF source files `#include <lz4.h>` outside of `ThirdParty/lz4/` itself. Both builds work without it — the Xcode linker flag is likely preemptive but the actual dependency doesn't exist in the source.
- **Impact of missing it**: None. Our build completes and all frameworks link successfully without it. The PPC build also works fine without it.

#### ots — OpenType Sanitizer

- **What it is**: A library from Google that validates and sanitizes OpenType/TrueType font files. It parses font tables, validates their structure, and rejects malformed or malicious fonts before they reach the renderer.
- **WebKit usage**: `WebCore/platform/graphics/opentype/OpenTypeSanitizer.cpp` includes `<ots/opentype-sanitiser.h>` and `<ots/ots-memory-stream.h>`. The `OpenTypeSanitizer` class inherits from `ots::OTSContext` and validates downloaded web fonts before use.
- **Why not in either build**: The cmake build does not include `OpenTypeSanitizer.cpp` for Mac. The PPC binary has `OpenTypeSanitizer::sanitize` as an island stub but zero `ots::` namespace symbols — the class is compiled but ots itself is not linked (guarded by `#if USE(OPENTYPE_SANITIZER)`). The `ThirdParty/ots/` directory doesn't exist in our source tree; the Leopard PPC patch adds Xcode build files for it but it's not actually compiled for PowerPC.
- **Impact of missing it**: Fonts loaded by the renderer are **not sanitized** before use. This is a security hardening feature — the browser works without it, but is less robust against malformed or malicious web fonts.

#### libwebrtc — WebRTC (Real-Time Communication)

- **What it is**: Google's WebRTC library — provides peer-to-peer audio/video communication, data channels, and the `getUserMedia()` API used by web apps for real-time communication (video calls, screen sharing, etc.).
- **WebKit usage**: Referenced by `WebKit2.xcconfig` via `$(LIBWEBRTC_LDFLAGS)`. The source tree includes a 5,837-line `ThirdParty/libwebrtc/CMakeLists.txt`.
- **Why not in either build**: The Leopard PPC patch has a Perl guard in `build-webkit` that **conditionally removes** libwebrtc from the build for ppc/ppc64/i386 architectures — it only compiles for x86_64 targeting macOS 10.11+. Symbol analysis confirms: the PPC binary has only `LibWebRTCProvider` stub symbols (`webRTCAvailable()` returning false, destructor) — no actual WebRTC code. Our cmake build also excludes it (`ENABLE_WEB_RTC=OFF`). **Both builds omit it for the same reason: the target OS doesn't support the required frameworks** (AVFoundation, CoreMedia, CoreVideo, VideoToolbox — all 10.7+).
- **Impact of missing it**: **No WebRTC support**. Web pages cannot use `navigator.mediaDevices.getUserMedia()`, `RTCPeerConnection`, or any real-time communication APIs. This is acceptable for a legacy OS target — the PPC build made the same trade-off.

---

## Disabled Components

### Test/Demo Tools — Removed from cmake

| Tool | Reason |
|---|---|
| DumpRenderTree | Needs 10.7+ headers (NSTextAlternatives.h, nullptr in .m files) |
| WebKitTestRunner | Needs 10.7+ APIs (NSURLFileResourceTypeKey, NSAccessibility attrs) |
| MiniBrowser | Needs 10.7+ APIs (NSModalResponse, NSSharingServicePickerDelegate, NSEventModifierFlagOption) |
| TestWebKitAPI | Not included in cmake build.ninja for PORT=Mac |

### XPC Sources — Removed from cmake

Five cmake blocks commented out in `source/webkit/PlatformMac.cmake`:

1. `XPCService_SOURCES` — shared XPC utility code
2. `WebProcess_SOURCES` — WebContent XPC entry point
3. `PluginProcess_SOURCES` — Plugin XPC entry point
4. `NetworkProcess_SOURCES` — Network XPC entry point
5. `DatabaseProcess_SOURCES` — Database XPC entry point

XPC is 10.7+ only. These entry-point `.mm` files call `xpc_main()`, `xpc_connection_t`, etc.

### Frameworks Removed from Linking (build.ninja patching)

| Framework | Reason |
|---|---|
| AVFAudio | Doesn't exist in 10.6 SDK (10.9+) |
| AVFoundation (sub-framework path) | 10.7+ framework |
| Metal | 10.11+ |

---

## XPC Strategy Comparison

| | Our cmake Build | Leopard PPC Patch |
|---|---|---|
| **XPCSPI.h** | Not modified | Emptied for `<= 1060` — all XPC types become unavailable |
| **XPC entry-point .mm** | Removed from cmake (5 blocks) | Not touched (Xcode doesn't compile them without targets) |
| **XPC runtime stubs** | `sdk_stubs.mm` provides weak stubs | Not needed (no XPC references compiled) |

---

## Framework Link Dependencies

### WebKit.framework
```
WebKitLegacy.framework (reexport)
WebCore.framework (reexport)
JavaScriptCore.framework
Accelerate, AudioToolbox, AudioUnit, Carbon, CoreAudio, Cocoa
DiskArbitration, IOKit, IOSurface, OpenGL, Quartz, QuartzCore
Security, SystemConfiguration
libsqlite3, libxml2, libz
```

### WebKitLegacy.framework
```
WebCore.framework
JavaScriptCore.framework
CoreFoundation, Accelerate, AudioToolbox, AudioUnit, Carbon, Cocoa
CoreAudio, DiskArbitration, IOKit, IOSurface, OpenGL, Quartz
QuartzCore, Security, SystemConfiguration
libedit, libicucore, libsqlite3, libxml2, libz, libc++, libSystem
```

### WebCore.framework
```
JavaScriptCore.framework
Accelerate, AudioToolbox, AudioUnit, Carbon, Cocoa, CoreAudio
DiskArbitration, IOKit, IOSurface, OpenGL, Quartz, QuartzCore
Security, SystemConfiguration
libsqlite3, libxml2, libz
```

### jsc
```
JavaScriptCore.framework
CoreFoundation, Cocoa, Security
libedit, libicucore, libc++, libSystem
```

---

## Packaging Output

### WebKit.app Bundle Structure
```
WebKit.app/
├── Contents/
│   ├── Info.plist              # Bundle metadata (org.webkit.nightly.snowleopard)
│   ├── PkgInfo                 # APPLwbkt
│   ├── MacOS/
│   │   └── WebKit              # Shell launcher → Safari + DYLD overrides
│   ├── Frameworks/
│   │   └── 10.6/
│   │       ├── JavaScriptCore.framework/
│   │       ├── WebCore.framework/
│   │       ├── WebKit.framework/
│   │       ├── WebKitLegacy.framework/
│   │       ├── libc++.1.dylib
│   │       ├── libc++abi.dylib
│   │       ├── libicuuc.dylib
│   │       ├── libicui18n.dylib
│   │       └── libicudata.dylib
│   └── Resources/
│       ├── VERSION
│       ├── BRANCH
│       └── start.html
```

### DMG Contents
```
WebKit-604.5.6-SnowLeopard-x86_64.dmg
├── WebKit.app/
├── install.command
├── uninstall.command
├── enable advanced features.command
├── revert advanced features to defaults.command
├── disable TopSites preview rendering.command
├── revert disabling TopSites preview rendering.command
└── Readme.txt
```

### Launcher Behavior
The `MacOS/WebKit` shell script:
1. Detects OS version via `sw_vers` (e.g., `10.6`)
2. Sets `DYLD_LIBRARY_PATH` and `DYLD_FRAMEWORK_PATH` to `Frameworks/10.6/`
3. Execs `/Applications/Safari.app/Contents/MacOS/Safari`

This causes Safari to load the custom WebKit frameworks instead of the system ones, providing modern web rendering on 10.6.

---

## Build Patches Applied by build.sh

All patches are applied programmatically by `build.sh` — **no direct source edits**. The build is fully reproducible from fresh source.

### Phase 1: Source Patches (stamp: `patches-applied`)
1. Python script comments out 5 XPC cmake blocks using parenthesis-balancing
2. sed disables DumpRenderTree in `Tools/CMakeLists.txt`
3. sed disables WebKitTestRunner in `Tools/CMakeLists.txt`
4. sed disables MiniBrowser/mac in `Tools/CMakeLists.txt`

### Phase 5: Post-cmake Fixes (stamp: `post-cmake-fixed`)
1. Remove AVFAudio framework path from `build.ninja`
2. Remove `-framework AVFAudio` from `build.ninja`
3. Fix semicolons in linker flags (`dynamic_lookup;-compatibility_version` → separate tokens)

---

## Comparison: Our Build vs Leopard PowerPC Patch

### Same Output
- ✅ JavaScriptCore.framework
- ✅ WebCore.framework
- ✅ WebKitLegacy.framework
- ✅ WebKit.framework (= WebKit2 in Leopard's naming)
- ✅ jsc, testapi, testb3, testair, LLIntOffsetsExtractor
- ✅ brotli + woff2 compiled into WebCore (confirmed in both PPC binary and our build)

### Different Approach
| Aspect | Our cmake 10.6 Build | Leopard PPC Xcode Build |
|---|---|---|
| Build system | cmake + ninja | Xcode (Makefile.shared) |
| Target | x86_64, macOS 10.6 | ppc_970 + ppc_7400, macOS 10.5 |
| Test tools | Disabled in cmake | Patched with `#if __MAC_OS_X_VERSION_MIN_REQUIRED` guards |
| XPC handling | Remove sources from cmake | Empty XPCSPI.h header |
| Framework naming | `WebKit.framework` | `WebKit2.framework` |
| IOSurface | Linked (10.6+ API) | Removed (10.5 target) |
| DataDetectorsCore | Not linked | Removed from linker flags |
| Security.framework | System framework | Custom 4.5MB PPC build bundled in app |
| libicucore | System ICU | Bundled 28MB ICU 55 dylib |
| Sparkle | Not included | Auto-updater framework (ppc) |
| WebKitLegacy | ✅ Built (216K) | ❌ Not in app bundle (WebKit2-only) |

### Same Third-Party Libs — Both Omit lz4, ots, libwebrtc
| Library | Our Build | PPC Build | Binary Evidence |
|---|---|---|---|
| lz4 | ❌ not in cmake | ❌ zero lz4 symbols in WebCore | Not linked by either |
| ots | ❌ not in cmake | ❌ zero `ots::` symbols, only stub class | `OpenTypeSanitizer::sanitize` island stubs only |
| libwebrtc | ❌ `ENABLE_WEB_RTC=OFF` | ❌ Perl guard skips for ppc/ppc64/i386 | `LibWebRTCProvider` stub returns `webRTCAvailable()=false` |

The Leopard PPC patch's build system *lists* these in MODULES and xcconfig files, but the actual PPC binary proves they were never linked. The Perl guard in `build-webkit` removes libwebrtc for non-x86_64; lz4 and ots are referenced in linker flags but produce no symbols in the output.

### Extras in Leopard PPC Build
- DumpRenderTree and MiniBrowser with reduced functionality (version-guarded)
- Custom Security.framework (ppc_7400, 4.5MB) bundled for 10.5 compatibility
- Bundled runtime dylibs: libicucore.A (28MB), libsqlite3 (1.2MB), libstdc++ (1.2MB), libxml2 (1.3MB), libxslt (220K), libgcc_s (56K)
- Sparkle.framework (auto-updater)

### Extras in Our Build
- WebKitLegacy.framework (PPC build is WebKit2-only)
- WebKit.app Safari launcher bundle with DYLD override
- Install/uninstall scripts
- DMG disk image for distribution
- enable/disable advanced features scripts
- xdgmime (MIME detection — not in Leopard's module list)
- gtest (test infrastructure)

---

## Build Stats

| Metric | Value |
|---|---|
| Ninja targets | 5540/5540 |
| Build errors | 0 |
| Stamp-based caching | `patches-applied`, `post-cmake-fixed` |
| Date built | June 2026 |
