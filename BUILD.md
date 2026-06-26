# Building WebKit 604 for Mac OS X 10.6 Snow Leopard

Cross-compilation of WebKit 604.5.6 (Safari 11 era) from Apple Silicon macOS targeting x86_64 Snow Leopard.

## Output Frameworks

| Framework | Size | Notes |
|-----------|------|-------|
| JavaScriptCore.framework | ~40 MB | JavaScript engine |
| WebCore.framework | ~65 MB | HTML/CSS/DOM/WebGL rendering |
| WebKit.framework | ~8.7 MB | WebKit2 multi-process API |
| WebKitLegacy.framework | ~215 KB | Legacy WebKit API (WebView) |

WebKit.framework (WebKit2, the multi-process architecture) builds successfully after removing XPC-dependent IPC source files from the CMake configuration. See [WebKit2 Status](#webkit2-status) below.

## Prerequisites

- **macOS** host (Apple Silicon or Intel)
- **Xcode Command Line Tools** (`xcode-select --install`)
- **CMake** 3.20+ (`brew install cmake`)
- **Ninja** (`brew install ninja`)
- **Mac OS X 10.6 SDK** — provided via git submodule (`sdk/MacOSX-SDKs`). Run `git submodule update --init` after cloning.
- **WebKit 604 source** — clone `https://github.com/WebKit/WebKit.git` to `source/webkit/`, checkout Safari 11 tag
- **ICU 55 source** — extract `icu55_1-src.tgz` to `downloads/icu-release-55-1/`
- **libc++ 5.0.1 source** — extract to `downloads/libcxx-5.0.1.src/`
- **libc++abi 5.0.1 source** — extract to `downloads/libcxxabi-5.0.1.src/`

## Directory Layout

```
leopard-webkit-build/
├── build.sh                        # Main build script (run this)
├── BUILD.md                        # This file
├── .gitignore
├── sdk/
│   ├── MacOSX-SDKs/               # Git submodule (phracker/MacOSX-SDKs)
│   │   └── MacOSX10.6.sdk/        # 10.6 SDK (the one we need)
│   └── MacOSX10.6.sdk -> MacOSX-SDKs/MacOSX10.6.sdk  # Symlink
├── downloads/
│   ├── icu-release-55-1/          # ICU 55 source
│   ├── libcxx-5.0.1.src/          # libc++ source
│   ├── libcxxabi-5.0.1.src/       # libc++abi source
│   └── patches-604/               # Leopard PowerPC reference patches
├── source/
│   └── webkit/                    # WebKit git repo (tag: Safari 11 / 604.5.6)
├── build/                         # Build directory (generated)
│   ├── overlay-includes/          # Compatibility headers
│   ├── fix-forwarding-headers.sh  # Forwarding header fix
│   ├── DerivedSources/            # Generated sources
│   └── lib/                       # Framework output
└── dist/                          # Dependency output (generated)
    ├── icu/                       # ICU libraries + headers
    └── libcxx/                    # libc++ + libc++abi
```

## Quick Start

```bash
# After cloning this repo:
git submodule update --init sdk/MacOSX-SDKs    # Fetch the 10.6 SDK
./build.sh                                       # Full build
```

This runs all phases in order. Each phase is idempotent — it checks a stamp file and skips if already complete.

Other invocations:
```bash
./build.sh --clean          # Remove all artifacts and rebuild
./build.sh --reset          # Reset sources and locks to fresh state (no build)
./build.sh --deps-only      # Only build ICU + libc++ dependencies
./build.sh --patches        # Only apply source patches
./build.sh --package-only   # Repackage existing build (phases 8-10 only)
```

## Build Phases

### Phase 0: Prerequisites
Verifies all required tools (cmake, ninja, clang) and source trees exist. Checks for the 10.6 SDK in `sdk/` or extracts from a tarball in `downloads/`.

### Phase 1: Dependencies
Builds from source if not already present in `dist/`:

- **ICU 55** — Configured with `--host=x86_64-apple-darwin10`, built as static libraries (`libicuuc.a`, `libicui18n.a`, `libicudata.a`)
- **libc++abi 5.0.1** — Compiled with `-target x86_64-apple-macos10.6`, produces `libc++abi.dylib`
- **libc++ 5.0.1** — Links against libc++abi, produces `libc++.1.dylib`

Headers are installed to `dist/libcxx/include/` and `dist/icu/include/`.

### Phase 2: Source Patches
Applies targeted patches to the WebKit source tree to resolve 10.6 API gaps:

| File | Issue | Fix |
|------|-------|-----|
| `WebKit/config.h` | HAVE_SEC_ACCESS_CONTROL (10.9+), ENABLE_NETWORK_CACHE (needs dispatch_data_t, 10.7+), HAVE_OS_ACTIVITY (10.10+) | Version guards with `__MAC_OS_X_VERSION_MIN_REQUIRED` |
| `NetworkConnectionToWebProcess.messages.in` | StoreDerivedDataToCache uses NetworkCache types | `#if ENABLE(NETWORK_CACHE)` guard |
| `NetworkConnectionToWebProcess.cpp` | NetworkCache include and function | `#if ENABLE(NETWORK_CACHE)` guard |
| `WebLoaderStrategy.cpp` | storeDerivedDataToCache function | `#if ENABLE(NETWORK_CACHE)` guard |
| `APIContentRuleListStore.cpp` | Requires NetworkCache::Data | `&& ENABLE(NETWORK_CACHE)` guard |
| `WebCompiledContentRuleListData.h` | NetworkCache::Data include | `#if ENABLE(NETWORK_CACHE)` guard |
| `WebCoreArgumentCoders.cpp` | Cursor type ambiguous with X11 Cursor typedef | Qualify as `WebCore::Cursor` |
| `WebChromeClient.cpp` | Same Cursor ambiguity | Qualify as `WebCore::Cursor` |
| `MenuUtilities.mm` | DataDetectors framework (10.10+) | `__MAC_OS_X_VERSION_MIN_REQUIRED >= 101000` guard |
| `WebPreferencesDefinitions.h` | Missing WebGL2Enabled key | Add `macro(WebGL2Enabled, ...)` entry |

### Phase 3: Overlay Files
Generates compatibility headers into `build/overlay-includes/` that are force-included via CMake flags:

| Overlay File | Purpose |
|-------------|---------|
| `TargetConditionals_compat.h` | TARGET_OS macros, availability macros, ObjC subscripting, weak ref stubs, XPC declarations, property introspection, strndup, calendar constants — 616 lines covering 18 compat areas |
| `Security/SecKeychain.h` | SecAuthenticationType enum (original uses C++ `static_cast`) |
| `os/object.h` | os_object_t, os_retain/os_release stubs |
| `dispatch/queue_shim.h` | DISPATCH_QUEUE_SERIAL/CONCURRENT constants |
| `sdk_stubs.c` | C-linkage XPC function stubs, sandbox_check |
| `sdk_stubs.mm` | ObjC++ linkage stubs: XPC globals, weak ref runtime, block introspection, JSC poison, sandbox, NSNotificationCenter postNotificationOnMainThread category |
| `wk_stubs.c` | WebKitSystemInterface function stubs (44 WK* functions that don't exist on 10.6) |
| `WebCoreStubs.cpp` | ScrollAnimator::create, ScrollbarThemeMac::painterForScrollbar, CABackdropLayer class |

### Phase 4: CMake Configure
Runs `cmake -G Ninja` with:

**Key flags:**
- `CMAKE_OSX_ARCHITECTURES=x86_64` — prevents cmake injecting `-arch arm64`
- `CMAKE_OSX_SYSROOT=` path to 10.6 SDK
- `CMAKE_POLICY_VERSION_MINIMUM=3.5` — suppresses cmake version check
- `PORT=Mac` — macOS port
- `CMAKE_C/CXX_FLAGS` — includes `-target x86_64-apple-macos10.6`, overlay includes via `-include TargetConditionals_compat.h`, libc++ and ICU include paths, framework sub-paths, `-D_LIBCPP_DISABLE_EXTERN_TEMPLATE -DOS_OBJECT_USE_OBJC=0 -std=gnu++14`
- `CMAKE_SHARED_LINKER_FLAGS` — includes `sdk_stubs.o`, ICU library paths, `-Wl,-undefined,dynamic_lookup`

**Disabled features:**
- `ENABLE_SERVICE_CONTROLS=OFF` — NSSharingServicePicker (10.10+)
- `ENABLE_ACCELERATED_2D_CANVAS=OFF` — requires Metal
- `ENABLE_CONTENT_FILTERING=OFF` — requires parental controls framework
- `ENABLE_ACCESSIBILITY=OFF` — accessibility API issues
- `ENABLE_DATALIST_ELEMENT=OFF`, `ENABLE_DEVICE_ORIENTATION=OFF`, etc.

**Enabled features:**
- `ENABLE_WEBGL=ON`, `ENABLE_WEBGL2=ON` — WebGL 1+2 with ANGLE
- `ENABLE_3D_TRANSFORMS=ON`, `ENABLE_CSS_REGIONS=ON`, etc.

### Phase 5: Post-CMake Fixes
- Sets `ENABLE_SERVICE_CONTROLS` to 0 in `cmakeconfig.h` (cmake incorrectly enables it)
- Runs `fix-forwarding-headers.sh`: copies WebKitLegacy forwarding headers over WebKit ones, converting `#include` to `#import` to prevent duplicate ObjC `@interface` errors

### Phase 6: Build
Runs `ninja` to compile all targets. Expect ~30-60 minutes depending on hardware.

### Phase 7: Verify
Checks each framework binary:
- Exists at expected path
- Is x86_64 architecture
- Reports file size

### Phase 8: Package into App Bundle
Creates `WebKit.app` in `build/`:

- **Info.plist** — App bundle metadata (identifier, version, minimum OS)
- **PkgInfo** — Classic Mac type/creator (`APPLwbkt`)
- **MacOS/WebKit** — Shell launcher that sets `DYLD_LIBRARY_PATH` and `DYLD_FRAMEWORK_PATH` to the 10.6 framework directory, then execs Safari
- **Frameworks/10.6/** — All built frameworks (JavaScriptCore, WebCore, WebKit, WebKitLegacy) plus runtime dylibs (libc++.1, libc++abi, libicuuc, libicui18n, libicudata)
- **Resources/** — VERSION, BRANCH, start.html

Framework binaries are stripped (`strip -x`) to remove debug symbols and reduce size.

### Phase 9: Create Install Scripts
Generates `.command` scripts in `build/scripts/`:

| Script | Purpose |
|--------|---------|
| `install.command` | Copies WebKit.app to `/Applications` |
| `uninstall.command` | Removes `/Applications/WebKit.app` |
| `enable advanced features.command` | Enables WebGL, WebAudio, accelerated compositing, full-screen via `defaults write` |
| `revert advanced features to defaults.command` | Resets all advanced feature defaults |
| `disable TopSites preview rendering.command` | Disables TopSites preview to save CPU/RAM |
| `revert disabling TopSites preview rendering.command` | Re-enables TopSites previews |

### Phase 10: Build DMG
Assembles a compressed DMG disk image containing:

- WebKit.app
- All install/config scripts
- Readme.txt with installation instructions and feature list

Output: `build/WebKit-604.5.6-SnowLeopard-x86_64.dmg`

## Configuration Reference

### Complete CMake Flags

```bash
cmake -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
  -DCMAKE_OSX_ARCHITECTURES=x86_64 \
  -DCMAKE_OSX_SYSROOT=/path/to/MacOSX10.6.sdk \
  -DCMAKE_C_FLAGS="-target x86_64-apple-macos10.6 -fallow-unsupported \
    -I$LIBCXX/include -I$ICU/include -I$OVERLAY \
    -include TargetConditionals_compat.h \
    -D_LIBCPP_DISABLE_EXTERN_TEMPLATE -DOS_OBJECT_USE_OBJC=0 \
    -F$SDK/.../Frameworks ... " \
  -DCMAKE_CXX_FLAGS="... -std=gnu++14" \
  -DCMAKE_SHARED_LINKER_FLAGS="-target x86_64-apple-macos10.6 \
    sdk_stubs.o -L$WebKitLibraries -L$ICU/lib \
    -licuuc -licui18n -licudata -Wl,-undefined,dynamic_lookup" \
  -DPORT=Mac \
  -DENABLE_WEBGL=ON -DENABLE_WEBGL2=ON \
  -DENABLE_SERVICE_CONTROLS=OFF \
  ...
```

### Why `-Wl,-undefined,dynamic_lookup`?
The 10.6 SDK lacks many symbols that WebKit references (XPC, dispatch_data_t, etc.). Rather than providing stubs for every missing function, we let the linker resolve them at runtime. The overlay headers declare the types; sdk_stubs provides the actual symbol bodies for critical ones.

### Why `CMAKE_POLICY_VERSION_MINIMUM=3.5`?
WebKit's CMakeLists.txt requires a specific cmake version range. Setting this to 3.5 bypasses the check, allowing modern cmake to work.

## WebKit2 Status

WebKit2 (the modern multi-process WebKit API) compiles **97.6% of files** but has **6 unresolvable failures** in the core IPC layer:

| File | Missing API | Available |
|------|-------------|-----------|
| `ConnectionMac.mm` | `xpc_connection_*`, `mach_port_unguard`, `DISPATCH_MACH_SEND_POSSIBLE` | 10.7+ |
| `SharedMemoryCocoa.cpp` | `MAP_MEM_VM_SHARE` (mach vm flag) | 10.7+ |
| `LayerHostingContext.mm` | `CAContext` (CoreAnimation hosting) | 10.7+ |
| Various | `NSSecureCoding` protocol | 10.8+ |

These are **kernel and runtime level APIs** that cannot be shimmed or stubbed — XPC is a fundamentally different IPC mechanism than Mach ports, and the multi-process architecture relies on it throughout.

The Leopard PowerPC build (patches-604) also **skipped WebKit2 entirely** (0 patches for `Source/WebKit/`).

## Runtime Fixes (Phase 2 Source Patches)

These patches fix crashes that occur at runtime on 10.6, not at compile time:

| File | Issue | Fix |
|------|-------|-----|
| `WebInspectorClient.mm` | `sendMessageToFrontend()` declared in header but never defined — dyld lazy binding crash | Added stub that dispatches to frontend page inspector controller |
| `WebDynamicScrollBarsView.mm` | `_webcore_effectiveFirstResponder` called on WebDynamicScrollBarsView but only existed on WebView/WebFrameView — crash on page focus | Added method returning `documentView` |
| `WebSystemInterface.mm` | `INIT(CGContextGetShouldSmoothFonts)` and 43 other WK function pointer initializations reference functions that don't exist on 10.6 | Guarded with `__MAC_OS_X_VERSION_MIN_REQUIRED >= 1070` |

### Stub Strategy

The build uses multiple stub files linked into different frameworks:

| Stub File | Linked Into | Contents |
|-----------|------------|----------|
| `sdk_stubs.o` | All frameworks | C functions (XPC, sandbox, dispatch_data), NSNotificationCenter category |
| `protocol_stubs.o` | All frameworks | Formal protocol declarations (NSURLConnectionDelegate) |
| `ehtype_stubs.o` | All frameworks | ObjC exception typeinfo (id, NSException) |
| `wk_stubs.o` | WebKitLegacy only | 44 WebKitSystemInterface function stubs |
| `WebNotificationProviderGrowl.o` | WebKitLegacy only | WebNotification protocol stub |

### Known Issue: WTF::lockAtomicallyInitializedStaticMutex

When relinking JavaScriptCore, the linker may drop `WTF::lockAtomicallyInitializedStaticMutex` from libWTF.a because no code in JSC directly references it. Safari references it at runtime via lazy binding. The fix for a future full rebuild: add `-Wl,-force_load,libWTF.a` to JSC's linker flags in CMakeLists.txt.

## TLS / HTTPS Limitation

Mac OS X 10.6's Security.framework only supports TLS 1.0. Sites requiring TLS 1.2+ (GitHub, most modern sites) will fail to load. This is an OS-level limitation that cannot be fixed within WebKit — the system's Secure Transport API handles TLS, not WebKit.

The established workaround is a **local Squid proxy with SSL bumping**:
1. Run Squid (built with OpenSSL) on another machine or locally
2. Configure Safari's proxy settings to use it
3. Squid negotiates TLS 1.2+ with modern servers and re-serves content

A future goal is to cross-compile Squid for x86_64 10.6 and bundle it in WebKit.app.

## Credits

- **Leopard PowerPC patches** — `downloads/patches-604/` for reference compatibility patterns
- **WebKit source** — `github.com/WebKit/WebKit`, tag 604.5.6
- **ICU 55** — `github.com/unicode-org/icu`, release-55-1
- **libc++/libc++abi 5.0.1** — `releases.llvm.org/5.0.1/`
