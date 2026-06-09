# WebKit 604.5.6 for Mac OS X 10.6 Snow Leopard

Cross-compile WebKit (Safari 11.0) targeting **x86_64** on **macOS 10.6** from a modern Apple Silicon host.

## Quick Start

```bash
# Clone with submodules (~2 GB)
git clone --recurse-submodules https://github.com/startergo/leopard-webkit-build.git

# Build everything
./build.sh
```

Frameworks are output to `build/lib/`:

| Framework | Size |
|---|---|
| JavaScriptCore.framework | 40M |
| WebCore.framework | 64M |
| WebKit.framework | 8.7M |
| WebKitLegacy.framework | 215K |

## Prerequisites

- macOS with Xcode Command Line Tools
- cmake, ninja (`brew install cmake ninja`)

## How It Works

`build.sh` is an idempotent, stamp-based orchestrator that runs these phases in order:

1. **ICU 55** — Builds static libraries from source for x86_64
2. **libc++ 5.0.1 + libc++abi 5.0.1** — C++ runtime for the 10.6 target
3. **Source patches** — Disables XPC sources, test tools, and 10.7+ APIs in cmake configs
4. **CMake configure** — Generates ninja build files targeting 10.6 SDK
5. **Post-cmake fixes** — Removes unavailable frameworks from build.ninja
6. **Ninja build** — Compiles all frameworks and tools

Each phase writes a stamp file to `build/stamps/`. Re-running skips completed phases.

## Submodules

| Path | Repository | Ref |
|---|---|---|
| `sdk/MacOSX-SDKs` | [phracker/MacOSX-SDKs](https://github.com/phracker/MacOSX-SDKs) | 11.3 |
| `source/webkit` | [WebKit/WebKit](https://github.com/WebKit/WebKit) | Safari-604.5.6 |
| `downloads/icu` | [unicode-org/icu](https://github.com/unicode-org/icu) | release-55-1 |
| `downloads/llvm-project` | [llvm/llvm-project](https://github.com/llvm/llvm-project) | llvmorg-5.0.1 |

## Documentation

- [BUILD.md](BUILD.md) — Step-by-step build instructions
- [BUILD_REFERENCE.md](BUILD_REFERENCE.md) — Technical reference (frameworks, libraries, patches, comparison with PPC build)

## License

The build scripts and configuration in this repository are provided as-is. WebKit itself is covered by its own license (BSD/LGPL). ICU is distributed under the ICU License. LLVM/libc++ is under the LLVM License (Apache 2.0 with exceptions).
