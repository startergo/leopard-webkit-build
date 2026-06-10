# WebKit 604.5.6 for Mac OS X 10.6 Snow Leopard

Cross-compile WebKit (Safari 11.0) targeting **x86_64** on **macOS 10.6** from a modern Apple Silicon host.

## Quick Start

```bash
# Clone with submodules (~2 GB)
git clone --recurse-submodules https://github.com/startergo/leopard-webkit-build.git

# Build everything (dependencies + frameworks + package + DMG)
./build.sh
```

The build produces a `WebKit.dmg` containing a ready-to-use Safari launcher:

| Framework | Size | Description |
|---|---|---|
| JavaScriptCore.framework | 40M | JavaScript engine |
| WebCore.framework | 64M | HTML/CSS/DOM/WebGL rendering |
| WebKit.framework | 8.7M | Modern WebKit API |
| WebKitLegacy.framework | 215K | Legacy WebKit API (WebView) |

## Installation

1. Open `WebKit.dmg`
2. Run **`install.command`** to copy WebKit.app to `/Applications`
3. Run **`enable advanced features.command`** to enable WebGL, WebAudio, and accelerated compositing
4. Launch `/Applications/WebKit.app` — it starts Safari using the updated WebKit frameworks

## Prerequisites

- macOS with Xcode Command Line Tools
- cmake, ninja (`brew install cmake ninja`)

## Build Options

```bash
./build.sh                  # Full build (all phases)
./build.sh --clean          # Remove all artifacts and rebuild
./build.sh --reset          # Reset sources/locks to fresh state (no build)
./build.sh --deps-only      # Only build ICU + libc++ dependencies
./build.sh --patches        # Only apply source patches
./build.sh --package-only   # Repackage existing build (phases 8-10 only)
```

## Build Phases

| Phase | Description |
|---|---|
| 0 | Prerequisites check |
| 1 | Build ICU 55 + libc++ 5.0.1 from source |
| 2 | Apply source patches for 10.6 compat |
| 3 | Generate overlay compat headers |
| 4 | CMake configure (targeting 10.6 SDK) |
| 5 | Post-cmake fixes (build.ninja patching) |
| 6 | Ninja build (~5540 targets) |
| 7 | Verify framework outputs |
| 8 | Package into WebKit.app bundle |
| 9 | Create install/config scripts |
| 10 | Build DMG disk image |

Each phase is idempotent — stamp files in `build/stamps/` track completion.

## Submodules

| Path | Repository | Ref |
|---|---|---|
| `sdk/MacOSX-SDKs` | [phracker/MacOSX-SDKs](https://github.com/phracker/MacOSX-SDKs) | 11.3 |
| `source/webkit` | [WebKit/WebKit](https://github.com/WebKit/WebKit) | Safari-604.5.6 |
| `downloads/icu` | [unicode-org/icu](https://github.com/unicode-org/icu) | release-55-1 |
| `downloads/llvm-project` | [llvm/llvm-project](https://github.com/llvm/llvm-project) | llvmorg-5.0.1 |

## Distribution Contents

The DMG includes:

- **WebKit.app** — Safari launcher with embedded frameworks and runtime libraries
- **install.command** / **uninstall.command** — Install/remove from /Applications
- **enable advanced features.command** — Enable WebGL, WebAudio, accelerated compositing, full-screen
- **revert advanced features to defaults.command** — Reset to defaults
- **disable/revert TopSites preview rendering.command** — Save CPU/RAM by disabling preview images
- **Readme.txt** — Usage instructions

## Documentation

- [BUILD.md](BUILD.md) — Step-by-step build instructions and configuration
- [BUILD_REFERENCE.md](BUILD_REFERENCE.md) — Technical reference (frameworks, libraries, patches, comparison with PPC build)

## License

The build scripts and configuration in this repository are provided as-is. WebKit itself is covered by its own license (BSD/LGPL). ICU is distributed under the ICU License. LLVM/libc++ is under the LLVM License (Apache 2.0 with exceptions).
