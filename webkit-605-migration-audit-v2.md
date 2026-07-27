# WebKit Branch Migration Audit
## `safari-604.5.6.0-branch` → `safari-605.2.8.1-branch`
### Target Platform: macOS Snow Leopard 10.6 x86_64

**Audit Date:** June 2026
**Branches compared:**
- `safari-604.5.6.0-branch` @ `8f4d186c2e4b7f5a57ffb6923ad80c8bb3994300`
- `safari-605.2.8.1-branch` @ `638989847985af550947c53986514adebf9f0d02`
**Method:** Live sparse-clone diff via `git diff` on the upstream WebKit repository. Every code snippet, line number, signature, and cmake value in this document was read directly from source.

---

## Table of Contents

1. [Executive Summary](#1-executive-summary)
2. [Directory Structure](#2-directory-structure)
3. [WebKitLegacy/mac/ Integrity](#3-webkitlegacymac-integrity)
4. [WebKitSystemInterface Elimination](#4-webkitsysteminterface-elimination)
5. [API-Level Breaking Changes](#5-api-level-breaking-changes)
6. [Build System (CMake) Changes](#6-build-system-cmake-changes)
7. [Xcode Build Path Is Unusable for 10.6](#7-xcode-build-path-is-unusable-for-106)
8. [Feature Flag Defaults](#8-feature-flag-defaults)
9. [PAL SPI Layer](#9-pal-spi-layer)
10. [WebCore Buildability on 10.6](#10-webcore-buildability-on-106)
11. [Recommendations — Symbol Shims](#11-recommendations--symbol-shims)
12. [Recommendations — CMake Patches](#12-recommendations--cmake-patches)
13. [Recommendations — build.sh Configuration](#13-recommendations--buildsh-configuration)
14. [Recommendations — PlatformMac.cmake](#14-recommendations--platformmaccmake)
15. [Go/No-Go Decision Matrix](#15-gono-go-decision-matrix)
16. [Open Items Requiring 10.6-SDK / Runtime Verification](#16-open-items-requiring-106-sdk--runtime-verification)
17. [Appendix A — Full WSI Symbol Table (604)](#appendix-a--full-wsi-symbol-table-604)
18. [Appendix B — PAL SPI Header Inventory (605)](#appendix-b--pal-spi-header-inventory-605)

---

## 1. Executive Summary

The migration from `safari-604.5.6.0` to `safari-605.2.8.1` is **feasible** for the Snow Leopard x86_64 WebKit1 build target. The legacy ObjC layer is structurally intact, the WSI framework dependency is gone, and WebCore builds on 10.6 with a small set of patches. The blocking items are all configuration- or shim-level; none require rewriting WebKit logic.

| Area | State |
|---|---|
| `WebKitLegacy/mac/` integrity | Fully intact. No `ASSERT_NOT_REACHED`/stub bodies; the legacy ObjC API layer is complete. |
| WebKitSystemInterface | Eliminated. The shim set required is smaller than before. |
| CMake WebKit2 suppression | `OptionsMac.cmake:58` does an unconditional `set(ENABLE_WEBKIT ON)` that overrides `-DENABLE_WEBKIT=OFF`. Requires a one-line patch. |
| Xcode build path | Structurally cannot target 10.6 (requires `macosx.internal` SDK; version map stops at 10.11). CMake is the only viable path. |
| Feature defaults | Several media/10.7+ features default ON and must be disabled. |
| WebCore on 10.6 | Buildable with patches. `Platform.h` degrades gracefully on old OS; the one compile break is the Metal/WebGPU surface (default ON for Mac). |

**Blocking items:** (1) the `ENABLE_WEBKIT` cmake patch (§12 Patch 1); (2) disabling the ON-by-default media/10.7+ features (§12 Patch 2); (3) the Metal/WebGPU exclusion (§10, §12 Patch 4); (4) the `_NSRecommendedScrollerStyle` function-plus-enum shim (§11.3); (5) resolving the window-order notification constants (§11.5).

---

## 2. Directory Structure

The legacy ObjC API layer lives at `Source/WebKitLegacy/mac/`. To diff it:

```bash
git diff safari-604.5.6.0-branch..safari-605.2.8.1-branch \
    -- Source/WebKitLegacy/mac/
```

`Source/WebKit/mac/` in both branches contains only three WK2 build-glue files (`MigrateHeadersFromWebKitLegacy.make`, `WebKit2.order`, `postprocess-framework-headers.sh`) — it is not the legacy API layer.

---

## 3. WebKitLegacy/mac/ Integrity

### File-Level Manifest

| Change | File | Notes |
|---|---|---|
| ➕ Added | `Misc/WebNSDataExtras.mm` | Promoted from `.m` to `.mm` (ObjC++) |
| ➖ Removed | `WebCoreSupport/WebIconDatabaseClient.mm` | Removed because its WebCore base class `IconDatabaseClient` was deleted. See §3.1. |
| ➖ Removed | `WebCoreSupport/WebSystemInterface.mm` | WSI function table eliminated entirely |

### 3.1 Icon Database State

- `ENABLE_ICONDATABASE` is still defined in `Source/cmake/WebKitFeatures.cmake:113` with default **ON**, and is present in `FeatureDefines.xcconfig`.
- `Source/WebKitLegacy/mac/Misc/WebIconDatabase.mm` still exists (10,721 bytes).
- The WebCore virtual interface `WebCore::IconDatabaseClient` (the base class `WebIconDatabaseClient` inherited from) was removed; with the base gone, the `WebIconDatabaseClient` subclass was deleted.
- The public `WebIconDatabase` ObjC class is now a deprecated no-op shell. The source carries a runtime warning that the class no longer handles icon loading and will be removed in a future release.

**Build impact:** none. The icon DB is a stub. Do not list `WebIconDatabaseClient.mm` in the sources (it does not exist in 605); no shim or disable is required.

### File Size Comparison

| File | 604 lines | 605 lines | Delta |
|---|---|---|---|
| `WebView/WebView.mm` | 10,124 | 10,151 | +27 |
| `WebView/WebHTMLView.mm` | 7,642 | 7,338 | −304 |
| `WebView/WebFrame.mm` | 2,631 | 2,639 | +8 |
| `WebView/WebFrameView.mm` | 1,279 | 1,276 | −3 |
| `WebView/WebPreferences.mm` | 3,205 | 3,318 | +113 |
| `WebCoreSupport/WebFrameLoaderClient.mm` | 2,446 | 2,476 | +30 |

### 3.2 WebHTMLView.mm −304 Lines: Accounting

The diff is 1,017 lines removed, 713 added (net −304), and is functionally preserving:

- ~258 of the removed lines are preprocessor directives (`#if PLATFORM(IOS)` / `#endif` / `#else`) — guard consolidation.
- The remainder is relocation, not deletion: forward-declarations of private AppKit categories (`NSApplication isSpeaking`, `NSView _recursive:…`, `NSWindow _newFirstResponderAfterResigning`, etc.) moved from scattered inline blocks into consolidated `PLATFORM(MAC)` blocks. (`isSpeaking` declaration present at line 187, used at line 2997.)
- SPI `#import`s changed path (`<WebCore/NSViewSPI.h>` → PAL equivalents); `<WebKitSystemInterface.h>` was dropped.
- `WebMenuTarget` was modernized to a Meyers singleton (`static WebMenuTarget *target = [[WebMenuTarget alloc] init];`, line 603).
- A few `#define` constants (`AUTOSCROLL_INTERVAL`, `WebDataProtocolScheme`) were removed as defines, but the autoscroll logic is intact (timer at lines 954/2024/2095).

The added side of the diff contains zero `ASSERT_NOT_REACHED` and zero `notImplemented()`. No functionality was stubbed.

---

## 4. WebKitSystemInterface Elimination

`#import <WebKitSystemInterface.h>` is absent from all `.mm`/`.h` files in `Source/WebKitLegacy/mac/` (only ChangeLog hits remain). `WebSystemInterface.mm`, with its `InitWebCoreSystemInterface()` and 63-entry `INIT()` table, is deleted.

### WSI Call Sites Remaining in 605

| Symbol | File:line | Guard | Action |
|---|---|---|---|
| `WKGetCurrentGraphicsContext()` | `WebHTMLView.mm:7036`, `WebFrameView.mm:520,532`, `WebFrame.mm:632` | `#if PLATFORM(IOS)` | None — iOS-only |
| `WKSetCurrentGraphicsContext()` | `WebHTMLView.mm:7038,7052` | `#if PLATFORM(IOS)` | None — iOS-only |
| `WKRectFill()` | `WebFrameView.mm:522,534` | iOS branch of `!PLATFORM(IOS)` | None — iOS-only |
| `WKNotifyHistoryItemChanged()` | `WebHistoryItem.mm:112` (def), `:303` (assigned to `WebCore::notifyHistoryItemChanged`) | Unconditional | None — local definition, not a WSI import |
| `WKPH*`/`WKPC*`/`WKPASpawnPluginHost` | `Plugins/Hosted/*.mm` | Unconditional | Disable via `ENABLE_NETSCAPE_PLUGIN_API=OFF`; MIG IPC can't build on 10.6 |

### WSI → 605 Replacement Map

| 604 WSI Call | 605 Replacement | Source |
|---|---|---|
| `WKAppVersionCheckLessThan(...)` | `_CFAppVersionCheckLessThan(CFSTR(...), -1, ver)` | `pal/spi/cf/CFUtilitiesSPI.h` — 15 call sites in `WebView.mm:2623–2942` |
| `WKGetWebDefaultCFStringEncoding()` | `webDefaultCFStringEncoding()` | WebCore `TextEncodingRegistry.h` — `WebPreferences.mm:1839` |
| `WKWindowWillOrderOnScreenNotification()` | `NSWindowWillOrderOnScreenNotification` | `pal/spi/mac/NSWindowSPI.h` (extern NSString*) — used `WebView.mm:5895,5919` |
| `WKWindowWillOrderOffScreenNotification()` | `NSWindowWillOrderOffScreenNotification` | same — used `WebView.mm:5897,5921` |
| `WKRecommendedScrollerStyle()` | `_NSRecommendedScrollerStyle()` | AppKit private — `WebHTMLView.mm:4660` (see §11.3) |
| `InitWebCoreSystemInterface()` call | Deleted | Removed from `WebPreferences.mm` |

---

## 5. API-Level Breaking Changes

All snippets below were read verbatim from 605 source. These cause compile errors only if WebCore and WebKitLegacy are at mismatched versions — build both from the same 605 tree and they resolve.

### 5.1 `PolicyAction` Enum Scoping
`PolicyUse`/`PolicyIgnore`/`PolicyDownload` → `PolicyAction::Use`/`::Ignore`/`::Download`. `WebFrameLoaderClient.mm:2421,2444,2449,2459,2461` (7 sites).

### 5.2 `FramePolicyFunction` Signature
```cpp
// 605 (WebFrameLoaderClient.mm:948):
void WebFrameLoaderClient::dispatchWillSubmitForm(FormState& formState, WTF::Function<void(void)>&& function)
{
    ...
    function();   // no-arg in the early-return path
    ...
    setUpPolicyListener([function = WTFMove(function)](PolicyAction) { function(); }).get());
}
```
The callback type changed from `FramePolicyFunction` (takes `PolicyAction`) to `WTF::Function<void(void)>` (takes nothing); the lambda absorbs the `PolicyAction` argument.

### 5.3 `PAL::SessionID`
```cpp
// 605 WebFrameLoaderClient.mm:204–217
std::optional<uint64_t> WebFrameLoaderClient::pageID() const  { return std::nullopt; }
std::optional<uint64_t> WebFrameLoaderClient::frameID() const { return std::nullopt; }
PAL::SessionID WebFrameLoaderClient::sessionID() const
{
    RELEASE_ASSERT_NOT_REACHED();
    return PAL::SessionID::defaultSessionID();
}
```
`SessionID` moved from the `WebCore` to the `PAL` namespace. The `RELEASE_ASSERT_NOT_REACHED()` is an intentional boundary (WebKit1 has no network session isolation), not a removed feature. `WebFrameLoaderClient.mm:1155` similarly has `RELEASE_ASSERT_NOT_REACHED(); // Content blockers are not enabled in WebKit1.`

### 5.4 `PageConfiguration` Constructor — `CacheStorageProvider`
```cpp
// 605 (WebView.mm:1421):
PageConfiguration pageConfiguration(
    makeUniqueRef<WebEditorClient>(self),
    SocketProvider::create(),
    LibWebRTCProvider::create(),                 // changed: makeUniqueRef<…> → ::create()
    WebCore::CacheStorageProvider::create()      // new argument
);
// later: _private->page = new Page(WTFMove(pageConfiguration));  (line 1457)
```
Two call sites (lines 1421 and 1689). `CacheStorageProvider` is a new WebCore class for the Cache API; the legacy layer creates a stub. It is provided by the 605 WebCore tree.

### 5.5 `handleEvent` Pointer → Reference
`handleEvent(Event*)` → `handleEvent(Event&)` in the plugin handler. Mechanical.

### 5.6 `webGLPolicyForURL` / `resolveWebGLPolicyForURL`
Both changed `const String&` → `const URL&` (605 lines 2157/2162).

### 5.7 `DeprecatedGlobalSettings` Refactor
16 call sites in `WebView.mm` moved from per-page `Settings` to the global `DeprecatedGlobalSettings`: e.g. `setMockScrollbarsEnabled` (2855), `setLowPowerVideoAudioBufferSizeEnabled` (2861), `setQTKitEnabled` (2948), `setShouldRespectPriorityInCSSAttributeSetters` (1408), `setShouldManageAudioSessionCategory` (1412).

### 5.8 `webKitBundleVersionString()` Removed
605 line 1138 calls `standardUserAgentWithApplicationName(applicationName)` with no version argument. The helper is gone; the UA no longer embeds the bundle version.

### 5.9 `PageVisibilityState` → `VisibilityState`
- The WebCore enum renamed `PageVisibilityState` → `VisibilityState` (values `::Visible`/`::Hidden`/`::Prerender`, used at `WebView.mm:603–607`).
- The public WebKit ObjC enum `WebPageVisibilityState*` is unchanged (lines 600–612, 4966–4985). The `kit()` adapter signature changed from `kit(PageVisibilityState)` to `kit(VisibilityState)`. The public API surface is untouched.

### 5.10 `PlaybackSessionInterface` Rename
`WebPlaybackSessionInterfaceMac`/`WebPlaybackSessionModelMediaElement` → `PlaybackSessionInterfaceMac`/`PlaybackSessionModelMediaElement` (headers at 605 lines 324–325, usage 9296–9300). Guarded by `#if PLATFORM(MAC) && ENABLE(VIDEO_PRESENTATION_MODE)`, which is OFF on 10.6 (§7), so this code compiles out entirely. No action needed.

### 5.11 New Preference Keys (+113 lines in WebPreferences.mm)
605 adds preference key/accessor pairs (Cache API, VisualViewport, WebAuthn, EME, AOM, Media Capabilities, Fetch keepalive, etc.), all with conservative defaults. The `FrameFlattening` enum became scoped (`FrameFlattening::FullyEnabled`/`::Disabled`).

---

## 6. Build System (CMake) Changes

### 6.1 `ENABLE_WEBKIT` Override — Highest-Priority Issue

```cmake
# Source/cmake/OptionsMac.cmake:57-58:
set(ENABLE_WEBKIT_LEGACY ON)
set(ENABLE_WEBKIT ON)          # unconditional — overrides the command line
```

Execution order:
```
cmake -DENABLE_WEBKIT=OFF
  CMakeLists.txt:26   if (NOT DEFINED ENABLE_WEBKIT) set(ENABLE_WEBKIT ON)   # skipped; -D wins here
  CMakeLists.txt:152  include(WebKitCommon) → include(OptionsMac)
                        OptionsMac.cmake:58  set(ENABLE_WEBKIT ON)            # OVERWRITES the -D
  CMakeLists.txt:164  add_subdirectory(Source)
                        Source/CMakeLists.txt:32  if (ENABLE_WEBKIT) add_subdirectory(WebKit)  # now TRUE
```
The gate at `Source/CMakeLists.txt:28-33` is itself correct:
```cmake
if (ENABLE_WEBKIT_LEGACY)
    add_subdirectory(WebKitLegacy)
endif ()
if (ENABLE_WEBKIT)
    add_subdirectory(WebKit)        # the WK2 tree to skip
endif ()
```
Without the patch in §12, `-DENABLE_WEBKIT=OFF` is silently discarded and the entire WK2 tree (NetworkProcess, StorageProcess) compiles.

### 6.2 DatabaseProcess → StorageProcess Rename
`Source/WebKit/DatabaseProcess/` (604) → `Source/WebKit/StorageProcess/` (605), the latter gaining a `ServiceWorker/` subdir. `ENABLE_DATABASE_PROCESS` is gone as a standalone flag; the process tree is gated by `ENABLE_WEBKIT`. Neither compiles when `ENABLE_WEBKIT=OFF` is correctly applied.

### 6.3 Unified Source Build
`WebKitMacros.cmake` adds `WEBKIT_COMPUTE_SOURCES`, invoking `generate-unified-source-bundles.rb`. `WebKitLegacy` does not use it (its `PlatformMac.cmake` lists files individually); WebCore does. The generator requires Ruby at configure time and uses only stdlib (`fileutils`, `pathname`, `getoptlong`) — it runs on the build host, so the host's system Ruby (2.6+/3.x) is sufficient. Verify `ruby --version` is present.

### 6.4 ICU Linkage
`OptionsMac.cmake` adds `add_definitions(-DU_DISABLE_RENAMING=1 -DU_SHOW_CPLUSPLUS_API=0)` and `set(ICU_LIBRARIES libicucore.dylib)`. `libicucore.dylib` ships on 10.6. `U_DISABLE_RENAMING` stabilizes ICU symbol names across SDK/runtime version skew — keep it.

### 6.5 `c++14` Standard
`.mm` files compile with `-ObjC++ -std=c++14`, which requires a host clang newer than anything that runs natively on 10.6 — hence cross-compilation.

---

## 7. Xcode Build Path Is Unusable for 10.6

The 605 tree ships an Xcode project (`WebKitLegacy.xcodeproj`) with `.xcconfig` files. It cannot target 10.6:

- `Source/WebKitLegacy/mac/Configurations/Base.xcconfig:113` hardcodes `SDKROOT = macosx.internal` — the Xcode build requires the Apple Internal SDK, which is not publicly available.
- `Base.xcconfig:86-90` maps `TARGET_MAC_OS_X_VERSION_MAJOR` only for suffixes `_11`/`_12`/`_13`/`_14` (10.11–10.14). There is no mapping for 10.6 (or 10.7–10.10); a 10.6 deployment target resolves the variable to empty.
- Consequence: every version-keyed feature in `FeatureDefines.xcconfig` (e.g. `ENABLE_VIDEO_PRESENTATION_MODE_macosx_101200`) evaluates to empty for 10.6.

CMake is therefore the only viable build system for this port. This is also why `ENABLE(VIDEO_PRESENTATION_MODE)` is correctly OFF for a 10.6 target.

---

## 8. Feature Flag Defaults

### Features Defaulting ON in 605 (WebKitFeatures.cmake)

| Feature | Default | 10.6 viable? | Action |
|---|---|---|---|
| `ENABLE_VIDEO` | ON | ❌ AVFoundation | Set OFF |
| `ENABLE_VIDEO_TRACK` | ON | ❌ depends on VIDEO | Set OFF |
| `ENABLE_WEB_AUDIO` | ON | ❌ AVFoundation | Set OFF |
| `ENABLE_FULLSCREEN_API` | ON | ⚠️ 10.7+ | Set OFF |
| `ENABLE_NOTIFICATIONS` | ON | ❌ NSUserNotification 10.8+ | Set OFF |
| `ENABLE_SMOOTH_SCROLLING` | ON | ⚠️ 10.7+ overlay scrollers | Set OFF |
| `ENABLE_REMOTE_INSPECTOR` | ON | ⚠️ XPC-based | Set OFF |
| `ENABLE_USER_MESSAGE_HANDLERS` | ON | ⚠️ WK2-coupled | Set OFF |
| `ENABLE_WEBGPU` | ON (Mac port) | ❌ Metal 10.11+ | Set OFF (see §10) |
| `ENABLE_WEBGL` | ON | ✅ OpenGL on 10.6 | Keep ON |
| `ENABLE_SUBTLE_CRYPTO` / `ENABLE_WEB_CRYPTO` | ON | ✅ CommonCrypto | Keep ON |
| `ENABLE_GEOLOCATION` | ON | ✅ CoreLocation | Keep ON |
| `ENABLE_INDEXED_DATABASE` | ON | ✅ file I/O | Keep ON |
| `ENABLE_SERVICE_WORKER` | OFF | — | Already off |

### Port-Specific Overrides
`OptionsMac.cmake` overrides base feature defaults via `WEBKIT_OPTION_DEFAULT_PORT_VALUE`. For example, `ENABLE_INDEXED_DATABASE_IN_WORKERS` is defined ON in `WebKitFeatures.cmake` but forced OFF for Mac, and `ENABLE_WEBGPU` is forced ON for Mac. Always read `OptionsMac.cmake`'s port overrides on top of `WebKitFeatures.cmake`'s base defaults.

### Flags Absent from 605 cmake
These 7 are absent from `WebKitFeatures.cmake` — do not pass them:
```
ENABLE_DATABASE_PROCESS   ENABLE_CSS_REGIONS   ENABLE_CANVAS_PROXY
ENABLE_GAMEPAD_DEPRECATED   ENABLE_LEGACY_VENDOR_PREFIXES
ENABLE_ALLINONE_BUILD   ENABLE_IMAGE_DECODER_DOWN_SAMPLING
```
`ENABLE_ICONDATABASE` is still present (default ON) and may be left as-is.

Passing an unknown `-DENABLE_FOO=OFF` produces a CMake warning ("Manually-specified variables were not used by the project"), not an error. Stale flags are cosmetic, not build-blocking.

---

## 9. PAL SPI Layer

PAL lives at `Source/WebCore/PAL/`, header-only, replacing WSI. Key headers:

- `pal/spi/cf/CFUtilitiesSPI.h` — declares `_CFAppVersionCheckLessThan(CFStringRef, int, double)`, `_CFExecutableLinkedOnOrAfter(CFSystemVersion)`, `_CFCopySystemVersionDictionary()`. Under `#if !USE(APPLE_INTERNAL_SDK)` it defines a `CFSystemVersion` enum (`Lion=7`, `MountainLion=8`).
- `pal/spi/mac/NSWindowSPI.h:52-53` — `extern NSString *NSWindowWillOrderOnScreenNotification;` / `…OffScreen…`. Declarations only; the string values live in AppKit, not here.
- `pal/spi/cocoa/pthreadSPI.h` — declares `int pthread_set_fixedpriority_self(void);` (not referenced by the legacy layer — see §11.4).

---

## 10. WebCore Buildability on 10.6

WebCore is buildable on 10.6 with patches. The detailed audit is in the companion document `webcore-106-buildability-audit.md`; the essentials:

### 10.1 Platform.h Degrades Gracefully
`Source/WTF/wtf/Platform.h` has no `#error` enforcing a minimum macOS version (only endian/pointer-width/JIT errors). Its version gates run in the safe direction for old OS:

| Platform.h line | Gate | Effect on 10.6 |
|---|---|---|
| 575 | `< 101200` → `USE_QTKIT 1` | QTKit media path auto-selected over AVFoundation |
| 1255 | `>= 101200` → `USE_OS_LOG 1` | `os_log` disabled on 10.6 |
| 1262 | `>= 101200` → `HAVE_SEC_TRUST_SERIALIZATION 1` | disabled on 10.6 |
| 1312 | `>= 101300` → `HAVE_RSA_PSS 1` | disabled on 10.6 |
| 1320 | `>= 101300` → `HAVE_AVCONTENTKEYSESSION 1` | disabled on 10.6 |

`USE_APPLE_INTERNAL_SDK` is auto-detected via `__has_include(<CoreFoundation/CFPriv.h>)` (Platform.h:505–508); a public 10.6 SDK lacks that header, so the open-source code paths are selected automatically.

### 10.2 C++ Standard Library
WebCore uses `std::optional`/`std::nullopt` (C++17), but `Source/WTF/wtf/Optional.h` provides a `std::optional` backport inside `namespace std` (line 141 onward). The `-std=c++14` toolchain is sufficient; no C++17 compiler is required.

### 10.3 Framework Availability
All frameworks `find_library`'d by `WebCore/PlatformMac.cmake` are present on 10.6 except:
- **Metal** (10.11+) — see §10.4.
- **AVFoundation** (10.7+ on Mac) — safe only because its sources are guarded by `#if ENABLE(VIDEO) && USE(AVFOUNDATION)` and `ENABLE_VIDEO=OFF` (e.g. `MediaPlayerPrivateAVFoundationObjC.mm:29` guards the AVFoundation import at line 91).
- **DataDetectorsCore** / **Lookup** (private, 10.7+/10.8+) — added to the link list only via `if (NOT …-NOTFOUND)` guards (`PlatformMac.cmake:59–67`), so a 10.6 SDK omits them automatically.

### 10.4 Metal / WebGPU — The WebCore Compile Break
`ENABLE_WEBGPU` defaults ON for the Mac port (`OptionsMac.cmake:44`). The 22 WebGPU/Metal source files (`PlatformMac.cmake:310–330` plus `WebGPULayer.mm:345`) are listed in the `WebCore_SOURCES` direct-compile block — not wrapped in `if (ENABLE_WEBGPU)` — and also appear in the unified manifest `SourcesCocoa.txt`. Most guard their bodies with `#if ENABLE(WEBGPU)`, but `GPUDeviceMetal.mm` imports `<Metal/Metal.h>` at line 32, before the guard begins at line 36:

```objc
32  #import <Metal/Metal.h>     // unguarded; processed regardless of ENABLE(WEBGPU)
...
36  #if ENABLE(WEBGPU)
```

`<Metal/Metal.h>` does not exist before 10.11, so this is a hard `'Metal/Metal.h' file not found` compile error. The fix is to exclude these sources (§12 Patch 4). The `${METAL_LIBRARY}` link entry itself is harmless — `find_library` returns `NOTFOUND` on a 10.6 SDK and the variable expands empty.

### 10.5 Other Surfaces (Handled by Upstream Guards)
- SQLite: `SQLiteDatabase.cpp:63` guards `sqlite3_errstr` behind `#if SQLITE_VERSION_NUMBER >= 3007015`; 10.6's older SQLite takes the fallback path.
- `os_log`: not used directly in WebCore `platform/`; gated by `USE(OS_LOG)`, off on 10.6.
- `WebCoreNSURLSession` is a WebKit wrapper class (`: NSObject`), not the system `NSURLSession`; media plumbing, neutralized with video off.
- x86_64 JIT/assembler is fully supported (`Platform.h:586`).

### 10.6 Runtime Watch-Items (Not Compile)
- bmalloc Gigacage enables on Darwin x86_64 (`Gigacage.h:58`) and uses `madvise(MADV_FREE_REUSABLE)` (`VMAllocate.h:200`), an API present on 10.6. The large virtual-memory reservation is expected to work but should be confirmed on hardware.
- After the Metal exclusion, confirm the final link has no duplicate `GPU*Metal` symbols (the dual source listing in §10.4).

---

## 11. Recommendations — Symbol Shims

### 11.1 WSI Symbols Not Needed
All 63 WSI `INIT()` entries (Appendix A) are unused in 605. The only WSI-prefixed symbol still live on macOS in the legacy layer is `WKNotifyHistoryItemChanged`, which is defined by WebKitLegacy itself (not shimmed).

### 11.2 `_CFAppVersionCheckLessThan`
Used at 15 sites in `WebView.mm:2623–2942` (Adobe/Apple app quirks), always with `linkedOnAnOlderSystemThan = -1`.
```c
#include <CoreFoundation/CoreFoundation.h>
// Signature from CFUtilitiesSPI.h:
Boolean _CFAppVersionCheckLessThan(CFStringRef bundleID, int linkedOnAnOlderSystemThan, double versionNumberLessThan)
{
    return false;  // 10.6: no quirks needed for these legacy apps
}
```

### 11.3 `_NSRecommendedScrollerStyle` + `NSScrollerStyle`
Used at `WebHTMLView.mm:4660`: `if (_NSRecommendedScrollerStyle() == NSScrollerStyleLegacy …)`. Both the function and the enum value are 10.7+ (overlay scrollers shipped in Lion). On 10.6 provide both:
```c
#import <AppKit/AppKit.h>
#if !defined(NSScrollerStyleLegacy)   // enum introduced in 10.7
typedef NS_ENUM(NSInteger, NSScrollerStyle) {
    NSScrollerStyleLegacy  = 0,
    NSScrollerStyleOverlay = 1,
};
#endif
NSScrollerStyle _NSRecommendedScrollerStyle(void)
{
    return NSScrollerStyleLegacy;   // 10.6 has only legacy scrollers
}
```

### 11.4 `pthread_set_fixedpriority_self`
Signature is `int (void)` (`pal/spi/cocoa/pthreadSPI.h`), but it is not referenced anywhere in `WebKitLegacy/mac/`. It may be pulled in by WTF/WebCore; if the linker reports it, provide the stub at that layer. It is not a legacy-layer requirement.

### 11.5 `NSWindowWillOrderOn/OffScreenNotification`
Used at `WebView.mm:5895,5897,5919,5921` as `addObserver:…name:…object:window` for selectors `_windowWillOrderOnScreen:` / `_windowWillOrderOffScreen:`. The on-screen handler:
```objc
- (void)_windowWillOrderOnScreen:(NSNotification *)notification {
    if (![self shouldUpdateWhileOffscreen]) [self setNeedsDisplay:YES];
    [self doWindowDidChangeScreen];
    if (_private && _private->page) {
        _private->page->resumeScriptedAnimations();
        _private->page->setIsVisible(true);
    }
}
```
If the notification constant does not match the real AppKit value, the observer never fires → `setIsVisible(true)` and `resumeScriptedAnimations()` don't run on window-order → a WebView can stay paused/not-redrawing when brought on screen. This is a real rendering bug, not cosmetic.

These are real private AppKit symbols that existed on 10.6. Approach:
1. **Link against AppKit's private symbol.** Declare `extern NSString *NSWindowWillOrderOnScreenNotification;` and let the linker resolve it from the 10.6 AppKit. The notification was private at least since 10.5; if exported, this needs no literal.
2. **Fallback if the link fails** — define the constant using WebKit's own convention for unavailable AppKit constants (full symbol name as the literal, per `WebView.mm:5884`): `NSString * const NSWindowWillOrderOnScreenNotification = @"NSWindowWillOrderOnScreenNotification";`. The literal value cannot be confirmed from open source; verify at runtime by logging in `_windowWillOrderOnScreen:` and confirming it fires when a window is ordered in.

### 11.6 `__interpose` Strategy
The dylib/`__interpose`/weak-symbol approach remains applicable; no structural change for 605.

---

## 12. Recommendations — CMake Patches

### Patch 1 — Fix `ENABLE_WEBKIT` Override (REQUIRED)
```cmake
# Source/cmake/OptionsMac.cmake, lines 57-58
# BEFORE:
set(ENABLE_WEBKIT_LEGACY ON)
set(ENABLE_WEBKIT ON)
# AFTER:
set(ENABLE_WEBKIT_LEGACY ON)
if (NOT DEFINED ENABLE_WEBKIT)
    set(ENABLE_WEBKIT ON)
endif ()
```

### Patch 2 — Disable Unavailable Features (REQUIRED)
Place before `WEBKIT_OPTION_END()` in `OptionsMac.cmake`, or pass as `-D` flags (§13):
```cmake
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_VIDEO PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_VIDEO_TRACK PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_WEB_AUDIO PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_MEDIA_STREAM PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_FULLSCREEN_API PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_SMOOTH_SCROLLING PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_REMOTE_INSPECTOR PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_NOTIFICATIONS PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_USER_MESSAGE_HANDLERS PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_MEDIA_CONTROLS_SCRIPT PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_SERVICE_WORKER PRIVATE OFF)
```

### Patch 3 — ICU (already correct in 605)
Keep the 605 lines; hardcode the full path only if `libicucore.dylib` doesn't resolve:
```cmake
set(ICU_LIBRARIES /usr/lib/libicucore.dylib)
```

### Patch 4 — Exclude Metal / WebGPU Sources (REQUIRED)
The files are listed in two places; both must be handled.

**4a. Force the feature off** (with the Patch 2 block):
```cmake
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_WEBGPU PRIVATE OFF)
```
**4b. Guard the direct-compile list** in `Source/WebCore/PlatformMac.cmake`. Split the large `list(APPEND WebCore_SOURCES …)` (lines ~147–475) so the GPU\*Metal block (lines ~310–330) and `WebGPULayer.mm` (line 345) are conditional:
```cmake
if (ENABLE_WEBGPU)
    list(APPEND WebCore_SOURCES
        platform/graphics/cocoa/GPUBufferMetal.mm
        # … all 21 GPU*Metal.mm …
        platform/graphics/cocoa/WebGPULayer.mm
    )
endif ()
```
**4c. Remove the unified entries** for the same files from `Source/WebCore/SourcesCocoa.txt` (the unified generator does not see the CMake `if()`).

**4d. (Defensive) Guard the Metal link entry** in `PlatformMac.cmake:42`:
```cmake
if (ENABLE_WEBGPU)
    list(APPEND WebCore_LIBRARIES ${METAL_LIBRARY})
endif ()
```
Alternative: move `#import <Metal/Metal.h>` in `GPUDeviceMetal.mm` to after the `#if ENABLE(WEBGPU)` at line 36, so all 22 files compile to empty TUs with the feature off. Exclusion is more robust because it also drops the dead object files.

---

## 13. Recommendations — build.sh Configuration

```bash
#!/bin/bash
set -e
WEBKIT_SRC="$(pwd)/../WebKit"
BUILD_DIR="$(pwd)/build-605"
SDK_PATH="/path/to/MacOSX10.6.sdk"
mkdir -p "$BUILD_DIR"; cd "$BUILD_DIR"

# WebCore unified-source generation needs Ruby at configure time
ruby --version || { echo "Ruby required at configure time"; exit 1; }

cmake "$WEBKIT_SRC" \
    -DPORT=Mac \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 \
    -DCMAKE_OSX_SYSROOT="$SDK_PATH" \
    -DCMAKE_INSTALL_PREFIX="$BUILD_DIR/install" \
    -DENABLE_WEBKIT=OFF \
    -DENABLE_WEBKIT_LEGACY=ON \
    -DENABLE_VIDEO=OFF \
    -DENABLE_VIDEO_TRACK=OFF \
    -DENABLE_WEB_AUDIO=OFF \
    -DENABLE_MEDIA_STREAM=OFF \
    -DENABLE_MEDIA_CONTROLS_SCRIPT=OFF \
    -DENABLE_FULLSCREEN_API=OFF \
    -DENABLE_SMOOTH_SCROLLING=OFF \
    -DENABLE_REMOTE_INSPECTOR=OFF \
    -DENABLE_NOTIFICATIONS=OFF \
    -DENABLE_USER_MESSAGE_HANDLERS=OFF \
    -DENABLE_SERVICE_WORKER=OFF \
    -DENABLE_WEBGPU=OFF \
    -DENABLE_WEBGL=ON \
    -DENABLE_SUBTLE_CRYPTO=ON \
    -DENABLE_WEB_CRYPTO=ON \
    -DENABLE_GEOLOCATION=ON \
    -DENABLE_INDEXED_DATABASE=ON \
    -DENABLE_3D_TRANSFORMS=ON \
    2>&1 | tee cmake-configure.log

make -j"$(sysctl -n hw.ncpu)" 2>&1 | tee cmake-build.log
```

Setting `-DENABLE_WEBGPU=OFF` is necessary but not sufficient on its own — the source-list exclusion in §12 Patch 4 is also required, because the Metal sources are listed unconditionally.

Do not pass the absent flags (harmless warning, but remove for cleanliness):
`-DENABLE_DATABASE_PROCESS`, `-DENABLE_CSS_REGIONS`, `-DENABLE_CANVAS_PROXY`, `-DENABLE_GAMEPAD_DEPRECATED`, `-DENABLE_LEGACY_VENDOR_PREFIXES`, `-DENABLE_ALLINONE_BUILD`, `-DENABLE_IMAGE_DECODER_DOWN_SAMPLING`. `ENABLE_ICONDATABASE` is valid and may be left at its default.

---

## 14. Recommendations — PlatformMac.cmake (WebKitLegacy)

If applying a patched 604 `PlatformMac.cmake` onto 605:

**Remove from sources** (deleted in 605):
```cmake
mac/WebCoreSupport/WebIconDatabaseClient.mm
mac/WebCoreSupport/WebSystemInterface.mm
```
**Change extension:** `mac/Misc/WebNSDataExtras.m` → `mac/Misc/WebNSDataExtras.mm`
**Variable renames** (the target was split out): `WebKit_SOURCES`→`WebKitLegacy_SOURCES`, `WebKit_INCLUDE_DIRECTORIES`→`WebKitLegacy_INCLUDE_DIRECTORIES`, `WebKit_SYSTEM_INCLUDE_DIRECTORIES`→`WebKitLegacy_SYSTEM_INCLUDE_DIRECTORIES`, `${WEBKIT_DIR}/mac/`→`${WEBKITLEGACY_DIR}/mac/`, `set(WebKit_OUTPUT_NAME …)`→`set(WebKitLegacy_OUTPUT_NAME …)`.
**Remove:** `link_directories(../../WebKitLibraries)` (dropped in 605).
**Add to forwarding headers:** `mac/Storage` directory and `mac/Storage/WebDatabaseManagerPrivate.h`.
The MIG custom commands changed from `MAIN_DEPENDENCY` to `DEPENDS` with explicit `.defs` lists — already reflected in the stock 605 file.

---

## 15. Go/No-Go Decision Matrix

| Item | Status | Effort | Blocking? |
|---|---|---|---|
| `WebKitLegacy/mac/` intact, no stubs | OK | 0 | No |
| WSI elimination | OK | ~1 h | No |
| `_CFAppVersionCheckLessThan` shim | shim ready | 15 m | No |
| Scroller-style shim: function + enum | shim ready | 1 h | **Yes** |
| Window-order notification constants | link-first, confirm at runtime | 1–2 h + runtime test | **Yes** |
| `ENABLE_WEBKIT` cmake override patch | patch ready | 15 m | **Yes** |
| Disable ON-by-default media/10.7+ features | patch ready | 30 m | **Yes** |
| Metal / WebGPU exclusion (WebCore) | patch ready (3-part) | 30 m | **Yes** |
| `PolicyAction` / `FramePolicyFunction` / `PageConfiguration` ABI | same-tree resolves | build-time | No |
| Icon DB | no action (deprecated stub) | 0 | No |
| Absent cmake flags | warning only | 10 m | No |
| Xcode build path | use CMake | 0 | No (informs approach) |
| Unified-source (Ruby) at configure | check host | 5 m | Conditionally |
| WebCore Platform.h / C++14 / frameworks | buildable, degrades gracefully | 0 | No |

**Overall: Proceed.** The blocking items are all configuration- or shim-level. None require rewriting WebKit logic. The remaining uncertainty is runtime, not compile (§16).

---

## 16. Open Items Requiring 10.6-SDK / Runtime Verification

These cannot be settled from source diff alone and need a real 10.6 SDK / link / runtime test:

1. **Window-order notification symbol resolution** (§11.5). Confirm whether 10.6 AppKit exports `NSWindowWillOrderOnScreenNotification`/`…OffScreen…`; if not, confirm the fallback literal fires the observer at runtime.
2. **`_NSRecommendedScrollerStyle` absence** (§11.3). Confirm the symbol is absent on 10.6 (expected) and that the stub returning `NSScrollerStyleLegacy` produces correct tracking-area behavior.
3. **Carbon `TECGetWebTextEncodings`** in `webDefaultCFStringEncoding` — present on 10.6 but deprecated; confirm it links.
4. **`pthread_set_fixedpriority_self`** — determine whether WTF/WebCore references it and needs the stub at that layer.
5. **bmalloc Gigacage** virtual-memory reservation on 10.6 x86_64 (§10.6) — expected to work; verify the large reservation succeeds.
6. **Metal/WebGPU final link** — after the §12 Patch 4 exclusion, confirm no duplicate `GPU*Metal` symbols and no unresolved Metal references.

---

## Appendix A — Full WSI Symbol Table (604)

All 63 `INIT()` entries from 604's `WebSystemInterface.mm` — unused in 605:

```
AccessibilityHandleFocusChanged        GetHTTPCookieAcceptPolicy
CALayerEnumerateRectsBeingDrawnWithBlock  GetNSEventKeyChar
CGContextDrawsWithCorrectShadowOffsets    GetWebDefaultCFStringEncoding
CGContextIsPDFContext                     GetWheelEventDeltas
CGPatternCreateWithImageAndTransform      HTTPCookies
CachePartitionKey                         HitTestMediaUIPart
CopyAXTextMarkerRangeEnd                  IsPublicSuffix
CopyAXTextMarkerRangeStart                MeasureMediaUIPart
CopyCONNECTProxyResponse                  NSElasticDeltaForReboundDelta
CopyDefaultSearchProviderDisplayName      NSElasticDeltaForTimeDelta
CopyRequestWithStorageSession             NSReboundDeltaForElasticDelta
CreateAXTextMarker                        QTClearMediaDownloadCache
CreateAXTextMarkerRange                   QTClearMediaDownloadCacheForSite
CreateAXUIElementRef                      QTGetSitesInMediaDownloadCache
CreateMediaUIBackgroundView               QTIncludeOnlyModernMediaFileTypes
CreateMediaUIControl                      QTMovieDisableComponent
CreatePrivateStorageSession               QTMovieGetType
Cursor                                    QTMovieHasClosedCaptions
DeleteAllHTTPCookies                      QTMovieMaxTimeLoaded
DeleteHTTPCookie                          QTMovieMaxTimeLoadedChangeNotification
DrawBezeledTextArea                       QTMovieResolvedURL
DrawCellFocusRingWithFrameAtTime          QTMovieSelectPreferredAlternates
DrawFocusRing                             QTMovieSetShowClosedCaptions
DrawFocusRingAtTime                       QueryDecoderAvailability
DrawMediaSliderTrack                      SetCONNECTProxyAuthorizationForStream
DrawMediaUIPart                           SetCONNECTProxyForStream
ExecutableWasLinkedOnOrBeforeSnowLeopard  SetDragImage
ExernalDeviceDisplayNameForPlayer         SetHTTPCookiesForURL
ExernalDeviceTypeForPlayer                SetMetadataURL
GetAXTextMarkerRangeTypeID                SpeechSynthesisGetDefaultVoiceIdentifierForLocale
GetAXTextMarkerTypeID                     SpeechSynthesisGetVoiceIdentifiers
GetBytesFromAXTextMarker                  UnregisterUniqueIdForElement
                                          WindowSetAlpha
                                          WindowSetScaledFrame
```

---

## Appendix B — PAL SPI Header Inventory (605)

**`cf/`:** `CFLocaleSPI.h`, `CFNetworkConnectionCacheSPI.h`, `CFNetworkSPI.h`, `CFUtilitiesSPI.h`, `CoreAudioSPI.h`, `CoreMediaSPI.h`

**`cocoa/`:** `AVKitSPI.h`, `CFNSURLConnectionSPI.h`, `CoreTextSPI.h`, `DataDetectorsCoreSPI.h`, `FoundationSPI.h`, `IOPMLibSPI.h`, `IOPSLibSPI.h`, `IOReturnSPI.h`, `IOSurfaceSPI.h`, `IOTypesSPI.h`, `LaunchServicesSPI.h`, `LinkPresentationSPI.h`, `MachVMSPI.h`, `NEFilterSourceSPI.h`, `NSAttributedStringSPI.h`, `NSButtonCellSPI.h`, `NSCalendarDateSPI.h`, `NSColorSPI.h`, `NSExtensionSPI.h`, `NSFileManagerSPI.h`, `NSFileSizeFormatterSPI.h`, `NSKeyedArchiverSPI.h`, `NSStringSPI.h`, `NSTouchBarSPI.h`, `NSURLConnectionSPI.h`, `NSURLDownloadSPI.h`, `NSURLFileTypeMappingsSPI.h`, `PassKitSPI.h`, `QuartzCoreSPI.h`, `ServersSPI.h`, `WebFilterEvaluatorSPI.h`, `pthreadSPI.h`

**`mac/`:** `AVFoundationSPI.h`, `DataDetectorsSPI.h`, `HIServicesSPI.h`, `HIToolboxSPI.h`, `LookupSPI.h`, `MediaRemoteSPI.h`, `MetadataSPI.h`, `NSAccessibilitySPI.h`, `NSApplicationSPI.h`, `NSCellSPI.h`, `NSEventSPI.h`, `NSFontSPI.h`, `NSGraphicsSPI.h`, `NSImmediateActionGestureRecognizerSPI.h`, `NSMenuSPI.h`, `NSPasteboardSPI.h`, `NSPopoverSPI.h`, `NSResponderSPI.h`, `NSScrollViewSPI.h`, `NSScrollerImpSPI.h`, `NSScrollingInputFilterSPI.h`, `NSScrollingMomentumCalculatorSPI.h`, `NSSharingServicePickerSPI.h`, `NSSharingServiceSPI.h`, `NSSpellCheckerSPI.h`, `NSTextFinderSPI.h`, `NSViewSPI.h`, `NSWindowSPI.h`, `PIPSPI.h`, `QTKitSPI.h`, `QuickDrawSPI.h`, `QuickLookMacSPI.h`, `SpeechSynthesisSPI.h`, `TUCallSPI.h`

**`ios/`:** `MobileGestaltSPI.h` (never compiled on macOS)

---

*Every claim in this document was read from upstream WebKit source at the two named commits. Items that could not be settled from source are listed as open in §16.*
