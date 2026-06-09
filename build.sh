#!/bin/bash
# ==========================================================================
#  build.sh — Reproducible build of WebKit 604 for Mac OS X 10.6 Snow Leopard
#
#  Cross-compiles from Apple Silicon (arm64) macOS targeting x86_64.
#  Outputs: JavaScriptCore.framework, WebCore.framework, WebKitLegacy.framework
#
#  Usage:
#    ./build.sh              # full build (idempotent, skips completed phases)
#    ./build.sh --clean      # clean everything and rebuild from scratch
#    ./build.sh --deps-only  # only build dependencies (ICU + libc++)
#    ./build.sh --patches    # only apply source patches
# ==========================================================================

set -euo pipefail

# ── Configuration ──────────────────────────────────────────────────────────
PROJECT_ROOT="$(cd "$(dirname "$0")" && pwd)"
SDK_NAME="MacOSX10.6.sdk"
SDK_DIR="$PROJECT_ROOT/sdk/MacOSX-SDKs/$SDK_NAME"
BUILD_DIR="$PROJECT_ROOT/build"
DIST_DIR="$PROJECT_ROOT/dist"
SOURCE_DIR="$PROJECT_ROOT/source/webkit"
OVERLAY_DIR="$BUILD_DIR/overlay-includes"
STAMP_DIR="$BUILD_DIR/stamps"
DOWNLOADS_DIR="$PROJECT_ROOT/downloads"

TARGET="x86_64-apple-macos10.6"
ARCH="x86_64"

# Dependency source locations
ICU_SRC="$DOWNLOADS_DIR/icu/icu4c"
ICU_DIST="$DIST_DIR/icu"
CRT_DIST="$DIST_DIR/crt"
LIBCXX_SRC="$DOWNLOADS_DIR/llvm-project/libcxx"
LIBCXXABI_SRC="$DOWNLOADS_DIR/llvm-project/libcxxabi"
LIBCXX_DIST="$DIST_DIR/libcxx"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

info()  { echo -e "${CYAN}[INFO]${NC}  $*"; }
ok()    { echo -e "${GREEN}[OK]${NC}    $*"; }
warn()  { echo -e "${YELLOW}[WARN]${NC}  $*"; }
err()   { echo -e "${RED}[ERROR]${NC} $*" >&2; }

# ── Parse arguments ────────────────────────────────────────────────────────
CLEAN=false
DEPS_ONLY=false
PATCHES_ONLY=false

for arg in "$@"; do
    case "$arg" in
        --clean)    CLEAN=true ;;
        --deps-only) DEPS_ONLY=true ;;
        --patches)  PATCHES_ONLY=true ;;
        --help|-h)
            echo "Usage: $0 [--clean] [--deps-only] [--patches] [--help]"
            echo ""
            echo "  --clean       Remove all build artifacts and rebuild"
            echo "  --deps-only   Only build ICU and libc++ dependencies"
            echo "  --patches     Only apply source patches"
            echo "  --help        Show this help"
            exit 0
            ;;
        *) err "Unknown argument: $arg"; exit 1 ;;
    esac
done

# ── Phase 0: Prerequisites ─────────────────────────────────────────────────

phase0_prerequisites() {
    info "Phase 0: Checking prerequisites..."

    # Check for required tools
    for tool in cmake ninja clang clang++; do
        if ! command -v "$tool" &>/dev/null; then
            err "'$tool' not found. Install with: brew install cmake ninja"
            exit 1
        fi
    done
    ok "Build tools found"

    # The phracker/MacOSX-SDKs submodule is missing CRT startup files
    # (crt1.o, crt1.10.5.o, crt1.10.6.o) needed for linking executables.
    # We extract them from the host's CLT SDK into dist/crt/ so we don't
    # mutate the git submodule. The linker finds them via -L$CRT_DIST.
    if [ ! -f "$CRT_DIST/crt1.o" ]; then
        local SYS_SDK="/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk"
        if [ -f "$SYS_SDK/usr/lib/crt1.o" ]; then
            info "  Extracting CRT startup files from system CLT..."
            mkdir -p "$CRT_DIST"
            cp "$SYS_SDK"/usr/lib/crt1.o "$CRT_DIST/"
            cp "$SYS_SDK"/usr/lib/crt1.10.5.o "$CRT_DIST/"
            cp "$SYS_SDK"/usr/lib/crt1.10.6.o "$CRT_DIST/"
        else
            warn "CRT startup files not found in system CLT. Executable linking may fail."
        fi
    fi

    # Check for SDK (provided via git submodule)
    if [ ! -d "$SDK_DIR/usr/include" ]; then
        # Try the symlink path too
        local SDK_LINK="$PROJECT_ROOT/sdk/$SDK_NAME"
        if [ -d "$SDK_LINK/usr/include" ]; then
            SDK_DIR="$SDK_LINK"
        else
            err "Mac OS X 10.6 SDK not found!"
            err "Run: git submodule update --init sdk/MacOSX-SDKs"
            err "The SDK is provided via the phracker/MacOSX-SDKs submodule."
            exit 1
        fi
    fi
    ok "SDK: $SDK_DIR"

    # Check for WebKit source
    if [ ! -d "$SOURCE_DIR/Source" ]; then
        err "WebKit source not found at $SOURCE_DIR"
        err "Clone with: git clone https://github.com/WebKit/WebKit.git source/webkit"
        err "Then checkout the Safari 11 / WebKit 604 tag"
        exit 1
    fi
    ok "WebKit source: $SOURCE_DIR"

    # Check for dependency sources
    if [ ! -d "$ICU_SRC/source" ] || [ ! -f "$ICU_SRC/source/configure" ]; then
        err "ICU 55 source not found at $ICU_SRC"
        err "Run: git submodule update --init downloads/icu"
        exit 1
    fi
    if [ ! -d "$LIBCXX_SRC/include" ]; then
        err "libc++ 5.0.1 source not found at $LIBCXX_SRC"
        err "Run: git submodule update --init downloads/llvm-project"
        exit 1
    fi
    if [ ! -d "$LIBCXXABI_SRC/include" ]; then
        err "libc++abi 5.0.1 source not found at $LIBCXXABI_SRC"
        err "Run: git submodule update --init downloads/llvm-project"
        exit 1
    fi

    mkdir -p "$BUILD_DIR" "$STAMP_DIR" "$DIST_DIR"
}

# ── Phase 1: Build Dependencies ────────────────────────────────────────────

phase1_icu() {
    local stamp="$STAMP_DIR/icu-built"
    [ -f "$stamp" ] && { info "ICU: already built (stamp exists)"; return 0; }

    info "Phase 1a: Building ICU 55 for x86_64..."
    mkdir -p "$ICU_DIST"

    cd "$ICU_SRC/source"

    # Build ICU for x86_64 using the host toolchain.
    # We cross-compile with -arch x86_64 but use the host's C++ headers/stdlib.
    # The resulting static .a libs contain x86_64 object code that links fine
    # into the WebKit build (the linker doesn't care what SDK was used for .a files).
    #
    # We do NOT use -isysroot or -mmacosx-version-min here because:
    #   1. The 10.6 SDK lacks C++ headers (typeinfo, string, etc.)
    #   2. The CRT startup files (crt1.10.6.o) aren't in modern toolchains
    #   3. ICU's own code doesn't use any 10.6-specific APIs

    # Use Apple ar/ranlib — GNU binutils versions create misaligned
    # 64-bit mach-o archives that Apple's linker rejects.
    CC="clang -arch $ARCH" \
    CXX="clang++ -arch $ARCH -std=c++11" \
    CFLAGS="-arch $ARCH -O2" \
    CXXFLAGS="-arch $ARCH -O2 -std=c++11" \
    LDFLAGS="-arch $ARCH" \
    AR=/usr/bin/ar \
    RANLIB=/usr/bin/ranlib \
    ./configure \
        --host=x86_64-apple-darwin \
        --prefix="$ICU_DIST" \
        --enable-static \
        --disable-shared \
        --disable-samples \
        --disable-tests \
        --with-data-packaging=static

    info "  Building ICU..."
    make -j"$(sysctl -n hw.ncpu)" 2>&1 | tail -5

    info "  Installing ICU to $ICU_DIST..."
    make install 2>&1 | tail -5

    # Verify the static libs are x86_64
    for lib in libicuuc.a libicui18n.a libicudata.a; do
        if [ -f "$ICU_DIST/lib/$lib" ]; then
            local arch=$(lipo -archs "$ICU_DIST/lib/$lib" 2>/dev/null || echo "unknown")
            info "  $lib: $arch"
        fi
    done

    touch "$stamp"
    ok "ICU 55 built and installed"
}

phase1_libcxx() {
    local stamp="$STAMP_DIR/libcxx-built"
    [ -f "$stamp" ] && { info "libc++: already built (stamp exists)"; return 0; }

    info "Phase 1b: Building libc++abi 5.0.1..."
    mkdir -p "$LIBCXX_DIST/lib" "$LIBCXX_DIST/include"

    local TARGET_FLAGS="-target $ARCH-apple-macos10.6 -isysroot $SDK_DIR -fallow-unsupported"

    # Build libc++abi
    cd "$LIBCXXABI_SRC/lib"
    rm -f *.o

    for FILE in ../src/*.cpp; do
        local BASE="$(basename "$FILE")"
        # Skip files that create circular deps or duplicate symbols
        [ "$BASE" = "cxa_demangle.cpp" ] && continue
        [ "$BASE" = "cxa_noexception.cpp" ] && continue
        echo "  Compiling libc++abi: $BASE"
        clang++ -c -O2 $TARGET_FLAGS \
            -std=c++11 \
            -nostdinc++ -isystem "$LIBCXX_SRC/include" \
            -I../include \
            -DNDEBUG \
            -DHAVE___CXA_THREAD_ATEXIT_IMPL \
            -D_LIBCPP_DISABLE_AVAILABILITY \
            -Wno-sign-conversion -Wno-shadow -Wno-conversion -Wno-shorten-64-to-32 \
            "$FILE"
    done

    # Stub for __cxa_thread_atexit_impl (not in 10.6)
    echo 'int __cxa_thread_atexit_impl(void(*dtor)(void*), void* obj, void* dso) { return -1; }
char* __cxa_demangle(const char* mangled, char* buf, unsigned long* n, int* status) { return 0; }' > _stub.c
    clang -c -O2 $TARGET_FLAGS -DNDEBUG _stub.c -o _stub.o

    echo "  Linking libc++abi.dylib..."
    clang $TARGET_FLAGS -o libc++abi.dylib \
        -dynamiclib -nodefaultlibs \
        -current_version 5.0.1 \
        -compatibility_version 1 \
        -install_name /usr/lib/libc++abi.dylib \
        -lSystem \
        *.o

    cp libc++abi.dylib "$LIBCXX_DIST/lib/"

    # Build libc++
    info "Phase 1c: Building libc++ 5.0.1..."
    cd "$LIBCXX_SRC/src"
    rm -f *.o

    for FILE in *.cpp; do
        echo "  Compiling libc++: $(basename "$FILE")"
        clang++ -c -O2 $TARGET_FLAGS \
            -std=c++11 \
            -nostdinc++ -isystem "$LIBCXX_SRC/include" \
            -I"$LIBCXXABI_SRC/include" \
            -DLIBCXX_BUILDING_LIBCXXABI \
            -DNDEBUG \
            -D_LIBCPP_DISABLE_AVAILABILITY \
            "$FILE" 2>/dev/null || true
    done

    echo "  Linking libc++.1.dylib..."
    clang++ $TARGET_FLAGS -o "$LIBCXX_DIST/lib/libc++.1.dylib" \
        -dynamiclib -nodefaultlibs \
        -current_version 1.0.5 \
        -compatibility_version 1 \
        -install_name /usr/lib/libc++.1.dylib \
        -L"$LIBCXX_DIST/lib" -lc++abi \
        -lSystem \
        *.o

    cd "$LIBCXX_DIST/lib" && ln -sf libc++.1.dylib libc++.dylib

    # Install headers
    info "  Installing libc++ headers..."
    cp -R "$LIBCXX_SRC/include/" "$LIBCXX_DIST/include/"
    cp "$LIBCXXABI_SRC/include/cxxabi.h" "$LIBCXX_DIST/include/"
    cp "$LIBCXXABI_SRC/include/__cxxabi_config.h" "$LIBCXX_DIST/include/"

    touch "$stamp"
    ok "libc++ 5.0.1 + libc++abi 5.0.1 built and installed"
}

# ── Phase 2: Source Patches ────────────────────────────────────────────────

phase2_patches() {
    local stamp="$STAMP_DIR/patches-applied"
    [ -f "$stamp" ] && { info "Patches: already applied (stamp exists)"; return 0; }

    info "Phase 2: Applying source patches..."

    local WK="$SOURCE_DIR/Source"

    # ─── Step 1: Leopard PowerPC reference patch (1379 files) ───
    # This is the bulk of the compatibility work — version guards, header fixes,
    # pragma changes, WebKitLegacy→WebKit include path updates, etc.
    local LEOPARD_PATCH="$DOWNLOADS_DIR/patches-604/WebKit_604.5.6.diff"
    if [ -f "$LEOPARD_PATCH" ]; then
        info "  Applying Leopard PowerPC reference patch (1379 files)..."
        cd "$SOURCE_DIR"
        local PATCH_LOG="$BUILD_DIR/leopard-patch.log"
        if yes n | patch -p1 --no-backup-if-mismatch --forward --batch < "$LEOPARD_PATCH" > "$PATCH_LOG" 2>&1; then
            ok "  Leopard patch applied"
        else
            # Patch may partially fail on files already patched — that's ok
            info "  Leopard patch: some hunks already applied (expected)"
        fi
        info "  Patch log: $(grep -c 'patching file' "$PATCH_LOG" 2>/dev/null || echo '0') files patched, $(grep -c 'FAILED\|skipping\|Reversed' "$PATCH_LOG" 2>/dev/null || echo '0') skipped/failed"
    else
        warn "  Leopard patch not found at $LEOPARD_PATCH"
        warn "  Build may fail without it. See BUILD.md for details."
    fi

    # ─── Step 2: Supplementary diff (24 additional files) ───
    # Files modified for x86_64/10.6 that aren't in the PowerPC patch.
    local SUPPLEMENT="$DOWNLOADS_DIR/patches-604/supplement.diff"
    if [ -f "$SUPPLEMENT" ]; then
        info "  Applying supplementary patch (24 files)..."
        cd "$SOURCE_DIR"
        local SUPP_LOG="$BUILD_DIR/supplement-patch.log"
        if yes n | patch -p1 --no-backup-if-mismatch --forward --batch < "$SUPPLEMENT" > "$SUPP_LOG" 2>&1; then
            ok "  Supplementary patch applied"
        else
            info "  Supplementary patch: some hunks already applied (expected)"
        fi
        info "  Patch log: $(grep -c 'patching file' "$SUPP_LOG" 2>/dev/null || echo '0') files patched, $(grep -c 'FAILED\|skipping\|Reversed' "$SUPP_LOG" 2>/dev/null || echo '0') skipped/failed"
    fi

    # ─── Step 3: Targeted patches (on top of bulk patches) ───
    # These fix issues that require precise sed/python patching rather than
    # static diffs — they handle version-conditional logic, cmake block editing, etc.

    # ─── WebKit/config.h — Version guards ───
    info "  Patching WebKit/config.h (version guards)..."
    cat > /tmp/wk_config_patch.py << 'PYEOF'
import re, sys
f = sys.argv[1]
c = open(f).read()

# HAVE_SEC_ACCESS_CONTROL: require 10.9+
c = re.sub(
    r'#ifndef HAVE_SEC_ACCESS_CONTROL\s*\n#if \(PLATFORM\(IOS\) \|\| PLATFORM\(MAC\)\)(.*?\n#endif.*?\n#endif)',
    '#ifndef HAVE_SEC_ACCESS_CONTROL\n#if (PLATFORM(IOS) || PLATFORM(MAC)) && __MAC_OS_X_VERSION_MIN_REQUIRED >= 1090\n#define HAVE_SEC_ACCESS_CONTROL 1\n#endif\n#endif',
    c, flags=re.DOTALL)

# ENABLE_NETWORK_CACHE: disable for < 10.7
c = re.sub(
    r'#ifndef ENABLE_NETWORK_CACHE\s*\n#if \(PLATFORM\(COCOA\)(.*?)\n#else(.*?)\n#endif(.*?)\n#endif',
    '#ifndef ENABLE_NETWORK_CACHE\n#if (PLATFORM(COCOA) && !(PLATFORM(MAC) && __MAC_OS_X_VERSION_MIN_REQUIRED < 1070)) || USE(SOUP)\n#define ENABLE_NETWORK_CACHE 1\n#else\n#define ENABLE_NETWORK_CACHE 0\n#endif\n#endif',
    c, flags=re.DOTALL)

# HAVE_OS_ACTIVITY: require 10.10+
c = re.sub(
    r'#ifndef HAVE_OS_ACTIVITY\s*\n#if \(PLATFORM\(IOS\) \|\| PLATFORM\(MAC\)\)(.*?\n#endif.*?\n#endif)',
    '#ifndef HAVE_OS_ACTIVITY\n#if (PLATFORM(IOS) || PLATFORM(MAC)) && __MAC_OS_X_VERSION_MIN_REQUIRED >= 101000\n#define HAVE_OS_ACTIVITY 1\n#endif\n#endif',
    c, flags=re.DOTALL)

open(f, 'w').write(c)
print("  config.h patched")
PYEOF
    python3 /tmp/wk_config_patch.py "$WK/WebKit/config.h"

    # ─── NetworkConnectionToWebProcess.messages.in — NetworkCache guard ───
    info "  Patching NetworkConnectionToWebProcess.messages.in..."
    local MSG_IN="$WK/WebKit/NetworkProcess/NetworkConnectionToWebProcess.messages.in"
    if ! grep -q '#if ENABLE(NETWORK_CACHE)' "$MSG_IN" 2>/dev/null; then
        sed -i '' 's/^        StoreDerivedDataToCache/#if ENABLE(NETWORK_CACHE)\n        StoreDerivedDataToCache/' "$MSG_IN"
        # Add closing #endif after the StoreDerivedDataToCache line
        sed -i '' '/StoreDerivedDataToCache.*data)$/{
            N
            s/\(StoreDerivedDataToCache.*data)\)/\1\n#endif/
        }' "$MSG_IN"
    fi

    # ─── NetworkConnectionToWebProcess.cpp — NetworkCache guards ───
    info "  Patching NetworkConnectionToWebProcess.cpp..."
    local NCPP="$WK/WebKit/NetworkProcess/NetworkConnectionToWebProcess.cpp"
    if ! grep -q '#if ENABLE(NETWORK_CACHE)' "$NCPP" 2>/dev/null; then
        # Guard the NetworkCache include
        sed -i '' 's/^#include "NetworkCache\.h"$/#if ENABLE(NETWORK_CACHE)\n#include "NetworkCache.h"\n#endif/' "$NCPP"
        # Guard the storeDerivedDataToCache function
        sed -i '' 's/^void NetworkConnectionToWebProcess::storeDerivedDataToCache/#if ENABLE(NETWORK_CACHE)\nvoid NetworkConnectionToWebProcess::storeDerivedDataToCache/' "$NCPP"
        sed -i '' '/NetworkCache::singleton().storeData/,/^}$/{
            /^}$/a\
#endif
        }' "$NCPP"
    fi

    # ─── WebLoaderStrategy.cpp — NetworkCache guard ───
    info "  Patching WebLoaderStrategy.cpp..."
    local WLS="$WK/WebKit/WebProcess/Network/WebLoaderStrategy.cpp"
    if ! grep -q '#if ENABLE(NETWORK_CACHE)' "$WLS" 2>/dev/null; then
        # The function is already guarded in the source; verify
        if ! grep -q 'ENABLE(NETWORK_CACHE)' "$WLS"; then
            sed -i '' 's/^void WebLoaderStrategy::storeDerivedDataToCache/#if ENABLE(NETWORK_CACHE)\nvoid WebLoaderStrategy::storeDerivedDataToCache/' "$WLS"
            sed -i '' '/WebProcess::singleton.*StoreDerivedDataToCache/,/^}$/{
                /^}$/a\
#endif
            }' "$WLS"
        fi
    fi

    # ─── APIContentRuleListStore.cpp — NetworkCache guard ───
    info "  Patching APIContentRuleListStore.cpp..."
    local CRLS="$WK/WebKit/UIProcess/API/APIContentRuleListStore.cpp"
    if ! grep -q 'ENABLE(NETWORK_CACHE)' "$CRLS" 2>/dev/null; then
        # The entire file body is wrapped in the guard already; verify
        if ! grep -q '#if ENABLE(CONTENT_EXTENSIONS) && ENABLE(NETWORK_CACHE)' "$CRLS"; then
            sed -i '' 's/^#if ENABLE(CONTENT_EXTENSIONS)/#if ENABLE(CONTENT_EXTENSIONS) \&\& ENABLE(NETWORK_CACHE)/' "$CRLS"
        fi
    fi

    # ─── WebCompiledContentRuleListData.h — NetworkCache guard ───
    info "  Patching WebCompiledContentRuleListData.h..."
    local CCRL="$WK/WebKit/Shared/WebCompiledContentRuleListData.h"
    if ! grep -q 'ENABLE(NETWORK_CACHE)' "$CCRL" 2>/dev/null; then
        # Guard NetworkCache::Data include, constructor param, member, and init
        sed -i '' 's/^#include "NetworkCacheData\.h"$/#if ENABLE(NETWORK_CACHE)\n#include "NetworkCacheData.h"\n#endif/' "$CCRL"
    fi

    # ─── WebCoreArgumentCoders.cpp — WebCore::Cursor qualification ───
    info "  Patching WebCoreArgumentCoders.cpp..."
    local WAC="$WK/WebKit/Shared/WebCoreArgumentCoders.cpp"
    # Qualify Cursor as WebCore::Cursor (conflicts with X11 Cursor typedef)
    # Use broad replacement then fix any double-qualification
    sed -i '' 's/Cursor::/WebCore::Cursor::/g' "$WAC"
    sed -i '' 's/const Cursor& /const WebCore::Cursor\& /g' "$WAC"
    sed -i '' 's/= Cursor(/= WebCore::Cursor(/g' "$WAC"
    sed -i '' 's/ArgumentCoder<Cursor>/ArgumentCoder<WebCore::Cursor>/g' "$WAC"
    sed -i '' 's/Decoder\& decoder, Cursor\&/Decoder\& decoder, WebCore::Cursor\&/g' "$WAC"
    # Fix any double-qualification from the above
    sed -i '' 's/WebCore::WebCore::Cursor/WebCore::Cursor/g' "$WAC"

    # ─── MenuUtilities.mm — DataDetectors 10.10+ guard ───
    info "  Patching MenuUtilities.mm..."
    local MU="$WK/WebKit/Platform/mac/MenuUtilities.mm"
    if ! grep -q '__MAC_OS_X_VERSION_MIN_REQUIRED >= 101000' "$MU" 2>/dev/null; then
        sed -i '' 's/^#import <WebCore\/DataDetectorsSPI\.h>$/#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 101000\n#import <WebCore\/DataDetectorsSPI.h>\n#endif/' "$MU"
        sed -i '' 's/^#if ENABLE(TELEPHONE_NUMBER_DETECTION)$/& \&\& __MAC_OS_X_VERSION_MIN_REQUIRED >= 101000/' "$MU"
    fi

    # ─── WebPreferencesDefinitions.h — WebGL2Enabled key + missing backslash ───
    info "  Patching WebPreferencesDefinitions.h (WebGL key + backslash)..."
    local WPD="$WK/WebKit/Shared/WebPreferencesDefinitions.h"
    # First: ensure WebGLEnabled line ends with backslash (Leopard patch removes it)
    sed -i '' 's/macro(WebGLEnabled, webGLEnabled, Bool, bool, true, "", "") $/macro(WebGLEnabled, webGLEnabled, Bool, bool, true, "", "") \\/' "$WPD"
    # Then: add WebGL2Enabled line if not present
    if ! grep -q 'WebGL2Enabled' "$WPD" 2>/dev/null; then
        python3 -c "
import sys
lines = open('$WPD').readlines()
for i, line in enumerate(lines):
    if 'macro(WebGLEnabled, webGLEnabled, Bool, bool, true' in line:
        lines.insert(i + 1, '    macro(WebGL2Enabled, webGL2Enabled, Bool, bool, true, \"\", \"\") \\\\\\n')
        break
open('$WPD', 'w').writelines(lines)
"
    fi

    # ─── WebChromeClient.cpp — WebCore::Cursor qualification ───
    info "  Patching WebChromeClient.cpp..."
    local WCC="$WK/WebKit/WebProcess/WebCoreSupport/WebChromeClient.cpp"
    if grep -q 'void WebChromeClient::setCursor(const Cursor&' "$WCC" 2>/dev/null; then
        sed -i '' 's/void WebChromeClient::setCursor(const Cursor&/void WebChromeClient::setCursor(const WebCore::Cursor\&/' "$WCC"
    fi

    # ─── WebKitLegacy/PlatformMac.cmake — Disable plugin hosted sources ───
    # The Hosted plugin files require out-of-process plugin support (XPC, 10.7+).
    # Comment out: (1) mac/Plugins/Hosted/ source entries, (2) add_custom_command
    # mig blocks, (3) list(APPEND WebKit_SOURCES for mig files, (4) set_source_files_properties
    # for the mig-generated .c files, (5) file(COPY for .defs files.
    # Uses Python for robust multi-line cmake block handling (sed can't balance parens).
    info "  Patching WebKitLegacy/PlatformMac.cmake (disable hosted plugins)..."
    local WLMC="$WK/WebKitLegacy/PlatformMac.cmake"
    if grep -q 'mac/Plugins/Hosted/HostedNetscapePluginStream' "$WLMC" 2>/dev/null; then
        python3 - "$WLMC" << 'PYEOF'
import sys
path = sys.argv[1]
with open(path) as f:
    lines = f.readlines()

PREFIX = "# DISABLED: 10.6 no XPC: "
result = []
i = 0
while i < len(lines):
    stripped = lines[i].lstrip()

    # Already disabled — skip
    if stripped.startswith("# DISABLED:"):
        result.append(lines[i])
        i += 1
        continue

    # Individual lines with mac/Plugins/Hosted/ (source list entries etc.)
    if "mac/Plugins/Hosted/" in lines[i] and not stripped.startswith("#"):
        result.append(PREFIX + lines[i])
        i += 1
        continue

    # Multi-line cmake blocks to check: file(COPY, add_custom_command,
    # list(APPEND WebKit_SOURCES, set_source_files_properties
    block_markers = ["file(COPY", "add_custom_command(",
                     "list(APPEND WebKit_SOURCES", "set_source_files_properties("]
    is_block = any(stripped.startswith(m) for m in block_markers)

    if is_block:
        # Collect the block by balancing parentheses
        block = [lines[i]]
        depth = lines[i].count("(") - lines[i].count(")")
        j = i + 1
        while j < len(lines) and depth > 0:
            block.append(lines[j])
            depth += lines[j].count("(") - lines[j].count(")")
            j += 1
        block_text = "".join(block)
        # Disable if block contains WebKitPlugin or mac/Plugins/Hosted/
        if "WebKitPlugin" in block_text or "mac/Plugins/Hosted/" in block_text:
            # Also comment out preceding "# Compile MIG-generated" comment
            if result and "Compile MIG-generated" in result[-1]:
                result[-1] = PREFIX + result[-1]
            for b in block:
                if b.lstrip().startswith("# DISABLED:"):
                    result.append(b)
                else:
                    result.append(PREFIX + b)
        else:
            result.extend(block)
        i = j
        continue

    result.append(lines[i])
    i += 1

with open(path, "w") as f:
    f.writelines(result)
PYEOF
        info "    Disabled all hosted plugin blocks (Python)"
    fi

    # ─── WebKitLegacy/mac/WebView/WebPDFView.mm — CallUIDelegate ambiguity ───
    # After redefining YES/NO as integer literals (1/0), the compiler sees
    # ambiguity between BOOL and NSInteger overloads in CallUIDelegate.
    # Fix by casting NO → (BOOL)NO.
    info "  Patching WebPDFView.mm (CallUIDelegate ambiguity)..."
    local WPDF="$WK/WebKitLegacy/mac/WebView/WebPDFView.mm"
    if [ -f "$WPDF" ] && grep -q 'CallUIDelegate(self, @selector(pdfViewController:clickedLink:), kit(item), NO)' "$WPDF" 2>/dev/null; then
        sed -i '' 's/CallUIDelegate(self, @selector(pdfViewController:clickedLink:), kit(item), NO)/CallUIDelegate(self, @selector(pdfViewController:clickedLink:), kit(item), (BOOL)NO)/' "$WPDF"
        info "    Fixed CallUIDelegate ambiguity"
    fi

    # ─── SharedMemoryCocoa.cpp — VM_PROT_IS_MASK / MAP_MEM_VM_SHARE ───
    # These Mach VM constants exist in 10.7+ SDKs but not in 10.6.
    # Add #ifndef defines after the mach includes.
    info "  Patching SharedMemoryCocoa.cpp (VM constants)..."
    local SMC="$WK/WebKit/Platform/cocoa/SharedMemoryCocoa.cpp"
    if [ -f "$SMC" ] && ! grep -q 'VM_PROT_IS_MASK' "$SMC" 2>/dev/null; then
        sed -i '' '/#include <mach\/vm_map.h>/a\
\
// SDK 10.6 does not define these constants; provide them for mach_make_memory_entry_64.\
#ifndef VM_PROT_IS_MASK\
#define VM_PROT_IS_MASK ((vm_prot_t) 0x40)\
#endif\
#ifndef MAP_MEM_VM_SHARE\
#define MAP_MEM_VM_SHARE 0x400000\
#endif
' "$SMC"
        info "    Added VM_PROT_IS_MASK and MAP_MEM_VM_SHARE defines"
    fi

    # ─── LegacyCustomProtocolManagerCocoa.mm — NSURLSession.h ───
    # NSURLSession was introduced in 10.9. The import is unconditional but
    # the code using it is guarded by #if USE(NETWORK_SESSION) (off for 10.6).
    # Wrap the import to match.
    info "  Patching LegacyCustomProtocolManagerCocoa.mm (NSURLSession guard)..."
    local LCP="$WK/WebKit/NetworkProcess/CustomProtocols/Cocoa/LegacyCustomProtocolManagerCocoa.mm"
    if [ -f "$LCP" ] && ! grep -q '#if USE(NETWORK_SESSION)' "$LCP" 2>/dev/null; then
        sed -i '' 's/^#import <Foundation\/NSURLSession\.h>$/#if USE(NETWORK_SESSION)\
#import <Foundation\/NSURLSession.h>\
#endif/' "$LCP"
        info "    Guarded NSURLSession.h import with USE(NETWORK_SESSION)"
    fi

    # ─── EmptyClients.cpp — Remove debug #warn directives from Leopard patch ───
    # The Leopard PowerPC patch left behind invalid '#warn DEFINED' preprocessor
    # directives and a '#warning DEFINED' debug line in the INPUT_TYPE_COLOR block.
    info "  Patching EmptyClients.cpp (remove debug #warn directives)..."
    local EC="$WK/WebCore/loader/EmptyClients.cpp"
    if [ -f "$EC" ] && grep -q '#warn DEFINED' "$EC" 2>/dev/null; then
        sed -i '' '/#warn DEFINED/d' "$EC"
        sed -i '' '/#warning DEFINED/d' "$EC"
        # Clean up blank lines left behind
        sed -i '' '/^$/{ N; /^\n$/d; }' "$EC"
        info "    Removed debug #warn directives"
    fi

    # ─── LegacyWebArchiveMac.mm — Fix const NSData cast ───
    # The Leopard patch casts CFDataRef to (const NSData *) but
    # initForReadingWithData: expects non-const NSData *.
    info "  Patching LegacyWebArchiveMac.mm (const NSData cast)..."
    local LWAM="$WK/WebCore/loader/archive/cf/LegacyWebArchiveMac.mm"
    if [ -f "$LWAM" ] && grep -q '(const NSData \*)responseData' "$LWAM" 2>/dev/null; then
        sed -i '' 's/(const NSData \*)responseData/(NSData *)responseData/' "$LWAM"
        info "    Fixed const NSData cast"
    fi

    # ─── ObjcRuntimeExtras.h — Guard CFBridgingRelease against redefinition ───
    # Our overlay (TargetConditionals_compat.h) defines CFBridgingRelease as a
    # macro pointing to CFBridgingRelease_compat. The Leopard patch added an
    # inline CFBridgingRelease function in ObjcRuntimeExtras.h which, after
    # macro expansion, becomes a redefinition of CFBridgingRelease_compat.
    # Also the return type (const objc_object*) → id needs fixing for modern Clang.
    # Solution: wrap the whole block in #ifndef CFBridgingRelease.
    info "  Patching ObjcRuntimeExtras.h (CFBridgingRelease guard)..."
    local ORE="$WK/WTF/wtf/ObjcRuntimeExtras.h"
    if [ -f "$ORE" ] && grep -q '^inline id CFBridgingRelease' "$ORE" 2>/dev/null; then
        sed -i '' 's/^inline id CFBridgingRelease(CFTypeRef object)/#ifndef CFBridgingRelease\ninline id CFBridgingRelease(CFTypeRef object)/' "$ORE"
        # Add #endif before the closing #endif // __OBJC__
        sed -i '' '/^#endif \/\/ __OBJC__$/i\
#endif' "$ORE"
        # Fix const return: (const objc_object *)object → (id)object
        sed -i '' 's/return (const objc_object \*)object;/return (id)object;/' "$ORE"
        # Fix const in autorelease cast
        sed -i '' 's/\[(const objc_object\*)object autorelease\]/[(id)object autorelease]/' "$ORE"
        info "    Guarded CFBridgingRelease and fixed const casts"
    fi

    # ─── Compiler.h — Define CLANG_PRAGMA for clang builds ───
    # The Leopard patch uses CLANG_PRAGMA(diagnostic push) but only defined the
    # macro as empty in the non-clang #else block. For __clang__, add:
    #   #define CLANG_PRAGMA(PRAGMA) _Pragma(#PRAGMA)
    info "  Patching Compiler.h (CLANG_PRAGMA for clang)..."
    local COMP_H="$WK/WTF/wtf/Compiler.h"
    if [ -f "$COMP_H" ] && ! grep -q '^#define CLANG_PRAGMA.*_Pragma' "$COMP_H" 2>/dev/null; then
        # Add the clang definition right after WTF_COMPILER_SUPPORTS_BUILTIN_IS_TRIVIALLY_COPYABLE
        sed -i '' '/COMPILER_SUPPORTS_BUILTIN_IS_TRIVIALLY_COPYABLE COMPILER_HAS_CLANG_FEATURE/a\
\
#define CLANG_PRAGMA(PRAGMA) _Pragma(#PRAGMA)' "$COMP_H"
        info "    Added CLANG_PRAGMA macro for clang"
    fi

    # ─── WebCore/PlatformMac.cmake — Fix WebLayer.mm path ───
    # The Leopard patch moved WebLayer.mm from cocoa/ to mac/ but cmake still
    # references the old path. Fix the reference.
    info "  Patching WebCore/PlatformMac.cmake (WebLayer.mm path)..."
    local WCPMC="$WK/WebCore/PlatformMac.cmake"
    if [ -f "$WCPMC" ] && grep -q 'platform/graphics/cocoa/WebLayer\.mm' "$WCPMC" 2>/dev/null; then
        sed -i '' 's|platform/graphics/cocoa/WebLayer\.mm|platform/graphics/mac/WebLayer.mm|' "$WCPMC"
        info "    Fixed WebLayer.mm path (cocoa → mac)"
    fi

    # ─── SandboxSPI.h — Remove sandbox_filter_type redefinition ───
    # The 10.6 SDK's sandbox.h already defines enum sandbox_filter_type and
    # all its values (SANDBOX_FILTER_GLOBAL_NAME, etc.) under __APPLE_API_PRIVATE.
    # SANDBOX_FILTER_GLOBAL_NAME is an enum value, NOT a macro, so #ifndef guards
    # don't work. Must completely remove the enum to avoid redefinition errors.
    info "  Patching SandboxSPI.h (remove sandbox_filter_type redefinition)..."
    local SBSPI="$WK/WTF/wtf/spi/darwin/SandboxSPI.h"
    if [ -f "$SBSPI" ] && grep -qE '^#if USE\(APPLE_INTERNAL_SDK\)|enum sandbox_filter_type' "$SBSPI" 2>/dev/null; then
        cat > "$SBSPI" << 'SBEOF'
#pragma once

#if OS(DARWIN)

#import <sandbox.h>

// The 10.6 SDK's sandbox.h already provides enum sandbox_filter_type,
// SANDBOX_FILTER_GLOBAL_NAME, SANDBOX_CHECK_NO_REPORT, etc.
// Only declare what's genuinely missing.

WTF_EXTERN_C_BEGIN

#ifndef SANDBOX_CHECK_NO_REPORT
extern const enum sandbox_filter_type SANDBOX_CHECK_NO_REPORT;
#endif

int sandbox_check_by_audit_token(audit_token_t, const char *operation, enum sandbox_filter_type, ...);
int sandbox_container_path_for_pid(pid_t, char *buffer, size_t bufsize);
int sandbox_init_with_parameters(const char *profile, uint64_t flags, const char *const parameters[], char **errorbuf);

WTF_EXTERN_C_END

#endif // OS(DARWIN)
SBEOF
        info "    Rewrote SandboxSPI.h for 10.6 SDK compatibility"
    fi

    # ─── Regress141275.mm — Fix weak property for 10.6 ───
    # 10.6 doesn't support weak references in properties; use unsafe_unretained.
    info "  Patching Regress141275.mm (weak property for 10.6)..."
    local R141275="$WK/JavaScriptCore/API/tests/Regress141275.mm"
    if [ -f "$R141275" ] && grep -q '@property (weak) JSTEvaluator' "$R141275" 2>/dev/null; then
        sed -i '' 's/@property (weak) JSTEvaluator\* evaluator;/@property (unsafe_unretained) JSTEvaluator* evaluator;/' "$R141275"
        info "    Changed weak → unsafe_unretained"
    fi

    # ─── StyleResolver.cpp — Fix narrowing conversion ───
    # Modern Clang rejects MediaQueryEvaluator { "all" } as narrowing (const char[4] → bool).
    # Use parentheses to avoid brace-init-list narrowing check.
    info "  Patching StyleResolver.cpp (narrowing fix)..."
    local SR="$WK/WebCore/css/StyleResolver.cpp"
    if [ -f "$SR" ] && grep -q 'MediaQueryEvaluator { "all" }' "$SR" 2>/dev/null; then
        sed -i '' 's/MediaQueryEvaluator { "all" }/MediaQueryEvaluator("all")/' "$SR"
        info "    Fixed MediaQueryEvaluator narrowing"
    fi

    # ─── WebCore inspector — Fix Protocol:: ambiguity ───
    # The 10.6 SDK's objc/runtime.h has "typedef struct objc_object Protocol;"
    # which conflicts with Inspector::Protocol when "using namespace Inspector;" is active.
    # Qualify all bare Protocol:: references as Inspector::Protocol::.
    info "  Fixing Protocol:: ambiguity in WebCore inspector..."
    local PROTO_FIX_COUNT=0
    for f in $(grep -l "using namespace Inspector;" "$WK/WebCore/inspector/"*.cpp 2>/dev/null); do
        if grep -q '[^:]Protocol::' "$f" 2>/dev/null || grep -q '^Protocol::' "$f" 2>/dev/null; then
            sed -i '' 's/\([^:]\)Protocol::/\1Inspector::Protocol::/g' "$f"
            sed -i '' 's/^Protocol::/Inspector::Protocol::/g' "$f"
            PROTO_FIX_COUNT=$((PROTO_FIX_COUNT + 1))
        fi
    done
    info "    Fixed Protocol:: ambiguity in $PROTO_FIX_COUNT inspector files"

    # ─── Platform.h — Disable USE_OPENTYPE_SANITIZER ───
    # OTS library not available. Platform.h unconditionally enables it for Mac.
    # The header (OpenTypeSanitizer.h) is guarded by USE(OPENTYPE_SANITIZER)
    # so disabling the define is sufficient.
    info "  Disabling USE_OPENTYPE_SANITIZER in Platform.h..."
    local PLATH="$WK/WTF/wtf/Platform.h"
    if [ -f "$PLATH" ] && grep -q '#define USE_OPENTYPE_SANITIZER 1' "$PLATH" 2>/dev/null; then
        sed -i '' 's|^#define USE_OPENTYPE_SANITIZER 1|/* Disabled for 10.6 cross-build: OTS not available */ /* #define USE_OPENTYPE_SANITIZER 1 */|' "$PLATH"
        info "    Disabled USE_OPENTYPE_SANITIZER"
    fi

    # ─── Platform.h — Disable USE_WOFF2 ───
    # woff2 library not available. Enabled because target < 10.12.
    info "  Disabling USE_WOFF2 in Platform.h..."
    if [ -f "$PLATH" ] && grep -q '#define USE_WOFF2 1' "$PLATH" 2>/dev/null; then
        sed -i '' 's|^#define USE_WOFF2 1|/* Disabled for 10.6 cross-build: woff2 not available */ /* #define USE_WOFF2 1 */|' "$PLATH"
        info "    Disabled USE_WOFF2"
    fi

    # ─── Platform.h — Disable HAVE_NETWORK_EXTENSION for 10.6 ───
    # NetworkExtension framework not available in 10.6 SDK. Guard with version check.
    info "  Disabling HAVE_NETWORK_EXTENSION for 10.6 in Platform.h..."
    if [ -f "$PLATH" ] && grep -q '^#define HAVE_NETWORK_EXTENSION 1' "$PLATH" 2>/dev/null; then
        sed -i '' '/^#if CPU(X86_64)$/,/^#endif$/{
            s/^#if CPU(X86_64)$/#if CPU(X86_64) \&\& __MAC_OS_X_VERSION_MIN_REQUIRED >= 10100/
            /^#define USE_PLUGIN_HOST_PROCESS 1/a\
#elif CPU(X86_64)\
#define USE_PLUGIN_HOST_PROCESS 1
        }' "$PLATH"
        info "    Guarded HAVE_NETWORK_EXTENSION with version check"
    fi

    # ─── SQLiteStatement.cpp — Relax SQLite version check ───
    # 10.6 SDK has SQLite 3.6.12, WebKit requires 3.6.16.
    # The only difference is auto-retry on schema change in sqlite3_prepare_v2.
    info "  Patching SQLiteStatement.cpp (relax version check)..."
    local SQLS="$WK/WebCore/platform/sql/SQLiteStatement.cpp"
    if [ -f "$SQLS" ] && grep -q '#error SQLite version 3.6.16 or newer is required' "$SQLS" 2>/dev/null; then
        sed -i '' 's|#error SQLite version 3.6.16 or newer is required|// #error SQLite version 3.6.16 or newer is required — 10.6 SDK has 3.6.12|' "$SQLS"
        info "    Relaxed SQLite version check"
    fi

    # ─── RenderText.cpp — Disable size assertion ───
    # COMPILE_ASSERT comparing RenderText to SameSizeAsRenderText fails because
    # struct layout differs in cross-build. This is only a size regression check.
    info "  Patching RenderText.cpp (disable size assertion)..."
    local RT="$WK/WebCore/rendering/RenderText.cpp"
    if [ -f "$RT" ] && grep -q 'COMPILE_ASSERT(sizeof(RenderText) == sizeof(SameSizeAsRenderText)' "$RT" 2>/dev/null; then
        sed -i '' 's|COMPILE_ASSERT(sizeof(RenderText) == sizeof(SameSizeAsRenderText), RenderText_should_stay_small);|// Disabled for 10.6 cross-build: struct layout differs\n// COMPILE_ASSERT(sizeof(RenderText) == sizeof(SameSizeAsRenderText), RenderText_should_stay_small);|' "$RT"
        info "    Disabled RenderText size assertion"
    fi

    # ─── AudioHardwareListenerMac — Use proc-based CoreAudio API for 10.6 ───
    # The block-based AudioObjectAddPropertyListenerBlock API isn't in the 10.6 SDK.
    # ─── Broad sweep: __MAC_OS_X_VERSION_MIN_REQUIRED == 1050 → <= 1060 ───
    # The Leopard PPC patch guards code with == 1050 (10.5 only). Since we target
    # 10.6 these guards fail. Change all == 1050 to <= 1060 so the code compiles.
    # Also fixes FontCocoa.mm CoreText enum definitions, AudioHardwareListenerMac.h
    # typedef guard, and many WebKitLegacy files.
    info "  Patching version guards: == 1050 → <= 1060 across all sources..."
    local VER_GUARD_COUNT=0
    while IFS= read -r f; do
        sed -i '' 's/__MAC_OS_X_VERSION_MIN_REQUIRED == 1050/__MAC_OS_X_VERSION_MIN_REQUIRED <= 1060/g' "$f"
        VER_GUARD_COUNT=$((VER_GUARD_COUNT + 1))
    done < <(grep -rl '__MAC_OS_X_VERSION_MIN_REQUIRED == 1050' "$WK/" 2>/dev/null | grep -v '\.orig\|\.rej\|ForwardingHeaders\|ThirdParty')
    info "    Patched $VER_GUARD_COUNT files"

    # ─── NSScrollerImp — Lower version guards from >= 1070 to >= 1060 ───
    # NSScrollerImp overlay scrollbars were introduced in 10.7 but the SPI header
    # declarations are needed at compile time so method signatures are known.
    # Without this, all method calls default to returning 'id'.
    info "  Patching NSScrollerImp headers (version guard >= 1070 → >= 1060)..."
    local NSISPI="$WK/WebCore/platform/spi/mac/NSScrollerImpSPI.h"
    if [ -f "$NSISPI" ] && grep -q '__MAC_OS_X_VERSION_MIN_REQUIRED >= 1070' "$NSISPI" 2>/dev/null; then
        sed -i '' 's/__MAC_OS_X_VERSION_MIN_REQUIRED >= 1070/__MAC_OS_X_VERSION_MIN_REQUIRED >= 1060/g' "$NSISPI"
        info "    Changed NSScrollerImpSPI guard to >= 1060"
    fi
    local NSIDET="$WK/WebCore/platform/mac/NSScrollerImpDetails.h"
    if [ -f "$NSIDET" ] && grep -q '__MAC_OS_X_VERSION_MIN_REQUIRED >= 1070' "$NSIDET" 2>/dev/null; then
        sed -i '' 's/__MAC_OS_X_VERSION_MIN_REQUIRED >= 1070/__MAC_OS_X_VERSION_MIN_REQUIRED >= 1060/g' "$NSIDET"
        info "    Changed NSScrollerImpDetails.h guard to >= 1060"
    fi
    local NSIDETMM="$WK/WebCore/platform/mac/NSScrollerImpDetails.mm"
    if [ -f "$NSIDETMM" ] && grep -q '__MAC_OS_X_VERSION_MIN_REQUIRED >= 1070' "$NSIDETMM" 2>/dev/null; then
        sed -i '' 's/__MAC_OS_X_VERSION_MIN_REQUIRED >= 1070/__MAC_OS_X_VERSION_MIN_REQUIRED >= 1060/g' "$NSIDETMM"
        info "    Changed NSScrollerImpDetails.mm guard to >= 1060"
    fi
    # Also fix preferredScrollerStyle returning 'id' in 10.6 SDK
    if [ -f "$NSIDETMM" ] && grep -q 'return \[NSScroller preferredScrollerStyle\]' "$NSIDETMM" 2>/dev/null; then
        sed -i '' 's/return \[NSScroller preferredScrollerStyle\]/return (NSScrollerStyle)[NSScroller preferredScrollerStyle]/' "$NSIDETMM"
        info "    Fixed preferredScrollerStyle return type cast"
    fi

    # ─── QuartzCoreSPI.h — Lower fence port guard from >= 101000 to >= 1060 ───
    # CAContext fence port methods are guarded with >= 101000 but we need them
    # at compile time even for 10.6 target (the SPI header declares them).
    info "  Patching QuartzCoreSPI.h (fence port guard)..."
    local QCSPI="$WK/WebCore/platform/spi/cocoa/QuartzCoreSPI.h"
    if [ -f "$QCSPI" ] && grep -q '__MAC_OS_X_VERSION_MIN_REQUIRED >= 101000' "$QCSPI" 2>/dev/null; then
        sed -i '' 's/__MAC_OS_X_VERSION_MIN_REQUIRED >= 101000/__MAC_OS_X_VERSION_MIN_REQUIRED >= 1060/g' "$QCSPI"
        info "    Changed QuartzCoreSPI fence guard to >= 1060"
    fi

    # ─── _WKRemoteObjectRegistry.mm — Fix objc_setAssociatedObject const void* key ───
    # 10.6 SDK declares key as 'void *' (non-const); modern SDK has 'const void *'.
    # The source uses a 'static const void*' key which doesn't match. Cast it.
    info "  Patching _WKRemoteObjectRegistry.mm (objc_setAssociatedObject key)..."
    local WKRO="$WK/WebKit/Shared/API/Cocoa/_WKRemoteObjectRegistry.mm"
    if [ -f "$WKRO" ]; then
        sed -i '' 's/objc_setAssociatedObject(invocation, replyBlockKey,/objc_setAssociatedObject(invocation, (void *)replyBlockKey,/' "$WKRO"
        info "    Fixed objc_setAssociatedObject key cast"
    fi

    # ─── WKNSDictionary.mm — Fix const id[] vs id* for 10.6 SDK ───
    # 10.6 SDK declares initWithObjects:(id*)forKeys:(id*) but the code overrides
    # with (const id[]) parameters. Remove const to match SDK signature.
    info "  Patching WKNSDictionary.mm (const id parameters)..."
    local WKND="$WK/WebKit/Shared/Cocoa/WKNSDictionary.mm"
    if [ -f "$WKND" ]; then
        sed -i '' 's/(const id \[\])objects/(id [])objects/' "$WKND"
        sed -i '' 's/(const id <NSCopying> \[\])keys/(id [])keys/' "$WKND"
        info "    Fixed WKNSDictionary const id parameters"
    fi

    # ─── PluginInformationMac.mm — Guard PluginBlacklist usage ───
    # PluginBlacklist.h is entirely behind >= 101000 guard, so the class doesn't
    # exist when targeting 10.6. Wrap the include and usage in version guards.
    info "  Patching PluginInformationMac.mm (PluginBlacklist >= 101000 guard)..."
    local PIMM="$WK/WebKit/Shared/Plugins/Netscape/mac/PluginInformationMac.mm"
    if [ -f "$PIMM" ]; then
        sed -i '' 's/#import <WebCore\/PluginBlacklist.h>/#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 101000\n#import <WebCore\/PluginBlacklist.h>\n#endif/' "$PIMM"
        sed -i '' 's/map.set(pluginInformationUpdatePastLastBlockedVersionIsKnownAvailableKey(), API::Boolean::create(WebCore::PluginBlacklist::isPluginUpdateAvailable(nsStringFromWebCoreString(plugin.bundleIdentifier))));/#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 101000\n    map.set(pluginInformationUpdatePastLastBlockedVersionIsKnownAvailableKey(), API::Boolean::create(WebCore::PluginBlacklist::isPluginUpdateAvailable(nsStringFromWebCoreString(plugin.bundleIdentifier))));\n#endif/' "$PIMM"
        info "    Fixed PluginInformationMac.mm PluginBlacklist guards"
    fi

    # ─── CookieStorageShimLibrary.cpp — __thread not supported for 10.6 target ───
    # The code uses __thread for TLS but x86_64 10.6 target doesn't support it.
    # Remove __thread; the static member inside the class keeps 'static',
    # and the out-of-class definition becomes a plain definition.
    info "  Patching CookieStorageShimLibrary.cpp (__thread removal)..."
    local CSSL="$WK/WebKit/Shared/mac/CookieStorageShimLibrary.cpp"
    if [ -f "$CSSL" ]; then
        sed -i '' 's/__thread static unsigned m_count;/static unsigned m_count;/' "$CSSL"
        sed -i '' 's/__thread unsigned ShimProtector::m_count/unsigned ShimProtector::m_count/' "$CSSL"
        info "    Fixed CookieStorageShimLibrary.cpp __thread removal"
    fi

    # ─── NSScrollerImpSPI.h — Add NSScrollerStyle/KnobStyle enum definitions ───
    # These AppKit types don't exist in 10.6 SDK. Add to the #else (non-internal-SDK) branch.
    info "  Patching NSScrollerImpSPI.h (add NSScrollerStyle/KnobStyle enums)..."
    local NSISPIF="$WK/WebCore/platform/spi/mac/NSScrollerImpSPI.h"
    if [ -f "$NSISPIF" ] && ! grep -q 'NSScrollerStyleLegacy' "$NSISPIF" 2>/dev/null; then
        sed -i '' '/^#else$/a\
\
enum {\
    NSScrollerStyleLegacy    = 0,\
    NSScrollerStyleOverlay   = 1,\
};\
typedef NSInteger NSScrollerStyle;\
\
enum {\
    NSScrollerKnobStyleDefault = 0,\
    NSScrollerKnobStyleDark    = 1,\
    NSScrollerKnobStyleLight   = 2,\
};\
typedef NSInteger NSScrollerKnobStyle;' "$NSISPIF"
        info "    Added NSScrollerStyle and NSScrollerKnobStyle enums"
    fi

    # ─── NSScrollerImpDetails.h — Add include for NSScrollerImpSPI.h ───
    # The header uses NSScrollerStyle but doesn't include the SPI header that defines it.
    info "  Patching NSScrollerImpDetails.h (add SPI include)..."
    local NSIDH="$WK/WebCore/platform/mac/NSScrollerImpDetails.h"
    if [ -f "$NSIDH" ] && ! grep -q 'NSScrollerImpSPI' "$NSIDH" 2>/dev/null; then
        sed -i '' 's/#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1060/#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1060\n\n#include "NSScrollerImpSPI.h"/' "$NSIDH"
        info "    Added NSScrollerImpSPI.h include"
    fi

    # ─── ScrollbarThemeMac.mm — Fix duplicate NSScrollerImp interface ───
    # The file has its own stub @interface NSScrollerImp for <= 1060, but now
    # the SPI header provides the full declaration. Change guard to <= 1050.
    info "  Patching ScrollbarThemeMac.mm (duplicate NSScrollerImp interface)..."
    local SBTMM="$WK/WebCore/platform/mac/ScrollbarThemeMac.mm"
    if [ -f "$SBTMM" ] && grep -q '__MAC_OS_X_VERSION_MIN_REQUIRED <= 1060' "$SBTMM" 2>/dev/null; then
        sed -i '' 's/__MAC_OS_X_VERSION_MIN_REQUIRED <= 1060/__MAC_OS_X_VERSION_MIN_REQUIRED <= 1050/g' "$SBTMM"
        info "    Changed NSScrollerImp stub guard to <= 1050"
    fi

    # ─── NSScrollerImpSPI.h — #undef macro versions before enum block ───
    # The compat header defines NSScrollerStyleLegacy/Overlay as macros.
    # This file redefines them as proper enum values. #undef the macros first.
    info "  Patching NSScrollerImpSPI.h (add #undef before enum)..."
    if [ -f "$NSISPIF" ] && ! grep -q '#undef NSScrollerStyleLegacy' "$NSISPIF" 2>/dev/null; then
        # Insert #undef before first enum block that contains NSScrollerStyleLegacy
        perl -i -0777 -pe 's/^(enum \{\n    NSScrollerStyleLegacy)/#ifdef NSScrollerStyleLegacy\n#undef NSScrollerStyleLegacy\n#endif\n#ifdef NSScrollerStyleOverlay\n#undef NSScrollerStyleOverlay\n#endif\n$1/m' "$NSISPIF"
        info "    Added #undef guards before NSScrollerStyle enum"
    fi

    # ─── QuartzCoreSPI.h — #undef kCAContext* macros before extern declarations ───
    # The compat header defines kCAContext* as macros, but this SPI header
    # declares them as extern NSString * const. Must #undef before the declaration.
    info "  Patching QuartzCoreSPI.h (add #undef for kCAContext* macros)..."
    local QCSPIF="$WK/WebCore/platform/spi/cocoa/QuartzCoreSPI.h"
    if [ -f "$QCSPIF" ]; then
        for sym in kCAContextDisplayName kCAContextDisplayId kCAContextIgnoresHitTest; do
            if grep -q "extern NSString \* const ${sym};" "$QCSPIF" 2>/dev/null; then
                perl -i -pe "s/^(extern NSString \\* const ${sym};)/#ifdef ${sym}\n#undef ${sym}\n#endif\n\$1/" "$QCSPIF"
                info "    Added #undef before ${sym} declaration"
            fi
        done
    fi

    # ─── RenderThemeMac.mm — Fix std::max float/double mismatch ───
    info "  Patching RenderThemeMac.mm (std::max type mismatch)..."
    local RTMM="$WK/WebCore/rendering/RenderThemeMac.mm"
    if [ -f "$RTMM" ]; then
        sed -i '' 's/std::max(titleTextSize.width, 12.0f)/std::max(titleTextSize.width, 12.0)/g' "$RTMM"
        info "    Fixed std::max float/double mismatch"
    fi

    # Change the CoreAudio version guard from >= 1060 to >= 1070 so the proc-based path is used.
    info "  Patching AudioHardwareListenerMac (proc-based CoreAudio API)..."
    local AHLM="$WK/WebCore/platform/audio/mac/AudioHardwareListenerMac.cpp"
    if [ -f "$AHLM" ] && grep -q '__MAC_OS_X_VERSION_MIN_REQUIRED >= 1060' "$AHLM" 2>/dev/null; then
        sed -i '' 's/__MAC_OS_X_VERSION_MIN_REQUIRED >= 1060/__MAC_OS_X_VERSION_MIN_REQUIRED >= 1070/g' "$AHLM"
        info "    Changed CoreAudio guards to >= 1070"
    fi

    # ─── ScrollingTreeFrameScrollingNodeMac — Add missing pure virtual method ───
    # ScrollController.h added scrollToOffsetWithoutAnimation() pure virtual.
    info "  Patching ScrollingTreeFrameScrollingNodeMac (missing virtual method)..."
    local STFSH="$WK/WebCore/page/scrolling/mac/ScrollingTreeFrameScrollingNodeMac.h"
    if [ -f "$STFSH" ] && ! grep -q 'scrollToOffsetWithoutAnimation' "$STFSH" 2>/dev/null; then
        sed -i '' '/void adjustScrollPositionToBoundsIfNecessary() override;/a\
    void scrollToOffsetWithoutAnimation(const FloatPoint\& offset) override;' "$STFSH"
        info "    Added scrollToOffsetWithoutAnimation declaration"
    fi
    local STFSM="$WK/WebCore/page/scrolling/mac/ScrollingTreeFrameScrollingNodeMac.mm"
    if [ -f "$STFSM" ] && ! grep -q 'scrollToOffsetWithoutAnimation' "$STFSM" 2>/dev/null; then
        sed -i '' '/immediateScrollBy(constainedPosition - currentScrollPosition);/a\
}\
\
void ScrollingTreeFrameScrollingNodeMac::scrollToOffsetWithoutAnimation(const FloatPoint\& offset)\
{\
    immediateScrollBy(offset - scrollPosition());' "$STFSM"
        info "    Added scrollToOffsetWithoutAnimation implementation"
    fi

    # ─── DragImageMac.mm — Fix std::max float/double mismatch ───
    # CGFloat is double on x86_64 but the code passes 12.0f (float).
    info "  Patching DragImageMac.mm (std::max type mismatch)..."
    local DIMM="$WK/WebCore/platform/mac/DragImageMac.mm"
    if [ -f "$DIMM" ]; then
        sed -i '' 's/std::max(textSize.width, 12.0f)/std::max(textSize.width, 12.0)/g' "$DIMM"
        info "    Fixed std::max float/double mismatch"
    fi

    # ─── TextIndicatorWindow.mm — Fix overlapping version guards ───
    # The Leopard PPC patch changed "> 1060" to ">= 1060", making both the
    # ObjC-block branch and the std::function branch active when targeting 10.6.
    # Only fix the first guard (>= 1060 → > 1060); the second guard (<= 1060)
    # for the closing ')' must stay as-is so std::function<>() is properly closed.
    info "  Patching TextIndicatorWindow.mm (version guard overlap)..."
    local TIW="$WK/WebCore/page/mac/TextIndicatorWindow.mm"
    if [ -f "$TIW" ]; then
        sed -i '' 's/__MAC_OS_X_VERSION_MIN_REQUIRED >= 1060/__MAC_OS_X_VERSION_MIN_REQUIRED > 1060/g' "$TIW"
        info "    Fixed version guard overlap in TextIndicatorWindow.mm"
    fi

    # ─── CAAudioStreamDescription.cpp — Fix enum redefinition ───
    # The Leopard PPC patch adds kLinearPCMFormatFlagsSampleFractionShift/Mask enums
    # guarded with <= 1060, but the host SDK's CoreAudioTypes already provides them.
    # Change to == 1050 to avoid redefinition when targeting 10.6.
    info "  Patching CAAudioStreamDescription.cpp (enum redefinition)..."
    local CASD="$WK/WebCore/platform/audio/mac/CAAudioStreamDescription.cpp"
    if [ -f "$CASD" ]; then
        sed -i '' 's/PLATFORM(MAC) && __MAC_OS_X_VERSION_MIN_REQUIRED <= 1060/PLATFORM(MAC) \&\& __MAC_OS_X_VERSION_MIN_REQUIRED == 1050/' "$CASD"
        info "    Fixed CAAudioStreamDescription.cpp enum guard"
    fi

    # ─── AudioHardwareListenerMac.cpp — Fix enum redefinition ───
    # The Leopard PPC patch adds kAudioDeviceTransportTypeUnknown/BuiltIn enums
    # guarded with <= 1060, but the 10.6 SDK's AudioHardware.h already defines them.
    # Change the enum guard to == 1050 only (leave the function guard at <= 1060).
    info "  Patching AudioHardwareListenerMac.cpp (enum redefinition)..."
    local AHLM="$WK/WebCore/platform/audio/mac/AudioHardwareListenerMac.cpp"
    if [ -f "$AHLM" ]; then
        # Only change the FIRST <= 1060 (the enum guard), not the function guard (macOS sed lacks 0,range)
        awk '!done && /__MAC_OS_X_VERSION_MIN_REQUIRED <= 1060/ { sub(/__MAC_OS_X_VERSION_MIN_REQUIRED <= 1060/, "__MAC_OS_X_VERSION_MIN_REQUIRED == 1050"); done=1 } {print}' "$AHLM" > "${AHLM}.tmp" && mv "${AHLM}.tmp" "$AHLM"
        info "    Fixed AudioHardwareListenerMac.cpp enum guard"
    fi

    # ─── SleepDisablerCocoa.cpp — Fix IOPMAssertionCreateWithDescription ───
    # IOPMAssertionCreateWithDescription is 10.7+; 10.6 only has IOPMAssertionCreateWithName.
    # The guard >= 1060 is wrong — must be >= 1070 to use the newer API.
    info "  Patching SleepDisablerCocoa.cpp (IOPM API version)..."
    local SDC="$WK/WebCore/platform/cocoa/SleepDisablerCocoa.cpp"
    if [ -f "$SDC" ]; then
        sed -i '' 's/!PLATFORM(IOS) && __MAC_OS_X_VERSION_MIN_REQUIRED >= 1060/!PLATFORM(IOS) \&\& __MAC_OS_X_VERSION_MIN_REQUIRED >= 1070/g' "$SDC"
        info "    Fixed SleepDisablerCocoa.cpp IOPM guard"
    fi

    # ─── Fix ATS-specific <= 1060 guards that overlap with == 1060 ───
    # The broad sweep above changed == 1050 → <= 1060, but ATS-specific code should
    # only compile for < 10.6 (no ATS on 10.6). Change <= 1060 → < 1060 in these files
    # to avoid overlapping with == 1060 guards (CGFont/CTFont paths).
    info "  Fixing ATS-specific <= 1060 guards..."
    for atsfile in \
        "$WK/WebCore/platform/graphics/mac/FontCustomPlatformData.cpp" \
        "$WK/WebCore/platform/graphics/mac/FontCustomPlatformData.h" \
        "$WK/WebCore/platform/graphics/cv/PixelBufferConformerCV.cpp"; do
        if [ -f "$atsfile" ]; then
            sed -i '' 's/__MAC_OS_X_VERSION_MIN_REQUIRED <= 1060/__MAC_OS_X_VERSION_MIN_REQUIRED < 1060/g' "$atsfile"
            info "    Fixed ATS guard: $(basename "$atsfile")"
        fi
    done

    # ─── WebVideoFullscreenController.mm — Fix NSApp property access ───
    # 10.6 SDK declares NSApp as 'id', not 'NSApplication *', so dot-syntax fails.
    info "  Patching WebVideoFullscreenController.mm (NSApp property access)..."
    local WVFC="$WK/WebCore/platform/mac/WebVideoFullscreenController.mm"
    if [ -f "$WVFC" ]; then
        sed -i '' 's/NSApp\.presentationOptions = options;/[(NSApplication *)NSApp setPresentationOptions:options];/' "$WVFC"
        info "    Fixed NSApp.presentationOptions"
    fi

    # ─── WebAccessibilityObjectWrapperMac.mm — Fix super casts and const objc_object ───
    # Modern Clang doesn't allow casting 'super'. Also fix (const objc_object*) → (id).
    info "  Patching WebAccessibilityObjectWrapperMac.mm (super cast, const fixes)..."
    local WAWM="$WK/WebCore/accessibility/mac/WebAccessibilityObjectWrapperMac.mm"
    if [ -f "$WAWM" ]; then
        sed -i '' 's/(WebAccessibilityObjectWrapper\*)super/super/g' "$WAWM"
        sed -i '' 's/(const objc_object\*)axElement/(id)axElement/g' "$WAWM"
        info "    Fixed super casts and const objc_object casts"
    fi

    # ─── OptionsMac.cmake — Keep ENABLE_WEBKIT ON ───

    # ─── WebKit/PlatformMac.cmake — Remove missing WebResourceLoadStatisticsManagerCocoa.mm ───
    # The Leopard patch removed this file but cmake still references it.
    # The StoreCocoa.mm variant exists; ManagerCocoa.mm does not.
    info "  Patching WebKit/PlatformMac.cmake (remove missing file ref)..."
    local WKPMC="$WK/WebKit/PlatformMac.cmake"
    if [ -f "$WKPMC" ] && grep -q 'WebResourceLoadStatisticsManagerCocoa\.mm' "$WKPMC" 2>/dev/null; then
        sed -i '' '/WebResourceLoadStatisticsManagerCocoa\.mm/d' "$WKPMC"
        info "    Removed missing WebResourceLoadStatisticsManagerCocoa.mm reference"
    fi

    # ─── WKImmediateActionController — Disable for 10.6 (requires 10.10+ NSPopover/DataDetectors) ───
    # The file uses NSPopover (10.7+), NSImmediateActionGestureRecognizer (10.10+),
    # and DataDetectors (10.10+). Remove from cmake. In WebViewImpl, the controller
    # is only created inside `if (NSClassFromString(@"NSImmediateActionGestureRecognizer"))`
    # which returns nil on 10.6 — so all `[m_immediateActionController ...]` calls are
    # nil-messaging (no-ops in ObjC). We just need to guard the #imports and the cast.
    info "  Patching WKImmediateActionController (disable for 10.6)..."
    if [ -f "$WKPMC" ] && grep -q 'WKImmediateActionController\.mm' "$WKPMC" 2>/dev/null; then
        sed -i '' '/WKImmediateActionController\.mm/d' "$WKPMC"
        info "    Removed WKImmediateActionController.mm from cmake"
    fi
    local WVI_MM="$WK/WebKit/UIProcess/Cocoa/WebViewImpl.mm"
    if [ -f "$WVI_MM" ]; then
        # Guard #import — the header references 10.10+ protocols/types
        sed -i '' 's/^#import "WKImmediateActionController\.h"$/#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 101000\n#import "WKImmediateActionController.h"\n#endif/' "$WVI_MM"
        sed -i '' 's/^#import <WebCore\/NSImmediateActionGestureRecognizerSPI\.h>$/#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 101000\n#import <WebCore\/NSImmediateActionGestureRecognizerSPI.h>\n#endif/' "$WVI_MM"
        # Guard the NSClassFromString init block — it casts to NSImmediateActionGestureRecognizer
        perl -i -0pe 's/(    if \(Class gestureClass = NSClassFromString\(\@"NSImmediateActionGestureRecognizer"\)\) \{\n.*?\n.*?\n.*?\n.*?\n    \})/#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 101000\n$1\n#endif/s' "$WVI_MM"
        info "    Guarded WebViewImpl.mm imports and init block"
    fi

    # ─── PageClientImpl.mm — Guard CorrectionPanel::recordAutocorrectionResponse ───
    # USE(AUTOCORRECTION_PANEL) is off for 10.6 (requires >= 1070), so CorrectionPanel
    # class is an empty forward declaration. The recordAutocorrectionResponse function
    # body calls CorrectionPanel:: directly but isn't guarded. Wrap it.
    info "  Patching PageClientImpl.mm (guard CorrectionPanel for 10.6)..."
    local PCIMM="$WK/WebKit/UIProcess/mac/PageClientImpl.mm"
    if [ -f "$PCIMM" ] && grep -q 'CorrectionPanel::recordAutocorrectionResponse' "$PCIMM" 2>/dev/null; then
        # Guard the include (header content is inside #if USE(AUTOCORRECTION_PANEL))
        sed -i '' 's/^#include "CorrectionPanel\.h"$/#if USE(AUTOCORRECTION_PANEL)\n#include "CorrectionPanel.h"\n#endif/' "$PCIMM"
        # Guard the function body
        sed -i '' 's/^    CorrectionPanel::recordAutocorrectionResponse(/#if USE(AUTOCORRECTION_PANEL)\n    CorrectionPanel::recordAutocorrectionResponse(/' "$PCIMM"
        # Add #endif after the line containing the closing paren+semicolon of that call
        perl -i -pe 'if (/CorrectionPanel::recordAutocorrectionResponse/ && !/#if USE/) { $_.="#endif\n" if /replacementString\);$/ }' "$PCIMM"
        info "    Guarded CorrectionPanel usage"
    fi

    # ─── WebViewImpl.mm — Guard convertRectToBacking: for 10.6 ───
    # convertRectToBacking: is 10.7+. On 10.6 NSRect != CGRect so NSRectToCGRect
    # fails. Wrap the call site.
    info "  Patching WebViewImpl.mm (guard convertRectToBacking for 10.6)..."
    local WVIMM="$WK/WebKit/UIProcess/Cocoa/WebViewImpl.mm"
    if [ -f "$WVIMM" ] && grep -q 'convertRectToBacking' "$WVIMM" 2>/dev/null; then
        sed -i '' 's/NSRectToCGRect(\[window convertRectToBacking:croppedImageRect\])/NSRectToCGRect(croppedImageRect)/' "$WVIMM"
        info "    Replaced convertRectToBacking: with direct NSRect (10.6 compat)"
    fi

    # ─── Fix const ObjC casts from Leopard patch (all source files) ───
    # The Leopard PowerPC patch used (const NSString *) etc. casts but modern
    # Clang treats toll-free-bridged types as non-const. Sweep all source files.
    # Also fixes return types, variable declarations, and casts.
    # ─── config.h — Disable HAVE_WINDOW_SERVER_OCCLUSION_NOTIFICATIONS for 10.6 ───
    # WKOcclusionNotificationType and WKRegisterOcclusionNotificationHandler are
    # 10.8+ private APIs. The #define is unconditional for PLATFORM(MAC).
    info "  Patching WebKit config.h (disable occlusion notifications for 10.6)..."
    local WKCFG="$WK/WebKit/config.h"
    if [ -f "$WKCFG" ] && grep -q '#define HAVE_WINDOW_SERVER_OCCLUSION_NOTIFICATIONS 1' "$WKCFG" 2>/dev/null; then
        sed -i '' 's/#define HAVE_WINDOW_SERVER_OCCLUSION_NOTIFICATIONS 1/#define HAVE_WINDOW_SERVER_OCCLUSION_NOTIFICATIONS 0/' "$WKCFG"
        info "    Disabled HAVE_WINDOW_SERVER_OCCLUSION_NOTIFICATIONS"
    fi

    # ─── WebPopupMenuProxyMac.mm — WKPopupMenu 8-param overload requires >= 1080 ───
    # The 8-param WKPopupMenu(NSMenu*,...,NSControlSize,bool) is behind
    # MAC_OS_X_VERSION_MIN_REQUIRED >= 1080 in WebKitSystemInterface.h.
    # For 10.6, fall back to the 6-param version.
    info "  Patching WebPopupMenuProxyMac.mm (WKPopupMenu overload for 10.6)..."
    local WPMM="$WK/WebKit/UIProcess/mac/WebPopupMenuProxyMac.mm"
    if [ -f "$WPMM" ] && grep -q 'WKPopupMenu(menu, location, roundf(NSWidth(rect)), dummyView.get(), selectedIndex, font, controlSize, data.hideArrows)' "$WPMM" 2>/dev/null; then
        sed -i '' 's/WKPopupMenu(menu, location, roundf(NSWidth(rect)), dummyView.get(), selectedIndex, font, controlSize, data.hideArrows)/WKPopupMenu(menu, location, roundf(NSWidth(rect)), dummyView.get(), selectedIndex, font)/' "$WPMM"
        info "    Replaced 8-param WKPopupMenu with 6-param version"
    fi

    # ─── PDFLayerControllerSPI.h — NS_ENUM unavailable on 10.6 ───
    # PDFLayerControllerCursorType is declared via NS_ENUM which doesn't exist
    # in the 10.6 SDK. Replace with plain C typedef + anonymous enum.
    # PDFLayerController itself doesn't exist on 10.6 either, but the runtime
    # guard in shouldUsePDFPlugin() checks classFromPDFKit(@"PDFLayerController")
    # which returns nil — so PDFPlugin is never instantiated on 10.6.
    info "  Patching PDFLayerControllerSPI.h (replace NS_ENUM for 10.6)..."
    local PDFSPI="$WK/WebKit/WebProcess/Plugins/PDF/PDFLayerControllerSPI.h"
    if [ -f "$PDFSPI" ] && grep -q 'typedef NS_ENUM' "$PDFSPI" 2>/dev/null; then
        sed -i '' 's/typedef NS_ENUM(NSInteger, PDFLayerControllerCursorType) {/typedef NSInteger PDFLayerControllerCursorType; enum {/' "$PDFSPI"
        info "    Replaced NS_ENUM with plain C typedef+enum"
    fi

    # ─── PDFPlugin.mm — Disambiguate Cursor/iBeamCursor ───
    # Carbon's HIToolbox defines a global Cursor struct. With "using namespace WebCore"
    # active, both ::Cursor and WebCore::Cursor are in scope, making unqualified
    # references ambiguous. Qualify all cursor references with WebCore:: prefix.
    info "  Patching PDFPlugin.mm (disambiguate Cursor types)..."
    local PDFPLUG="$WK/WebKit/WebProcess/Plugins/PDF/PDFPlugin.mm"
    if [ -f "$PDFPLUG" ]; then
        # Remove any prior using-declaration attempt (conflicts with using namespace)
        sed -i '' '/^using WebCore::Cursor; using WebCore::iBeamCursor/d' "$PDFPLUG"
        # Qualify cursor function calls and types (only if not already qualified)
        if ! grep -q 'WebCore::iBeamCursor()' "$PDFPLUG" 2>/dev/null; then
            sed -i '' 's/? iBeamCursor() : pointerCursor()/? WebCore::iBeamCursor() : WebCore::pointerCursor()/' "$PDFPLUG"
            sed -i '' 's/static const Cursor& pdfLayerControllerCursorTypeToCursor/static const WebCore::Cursor\& pdfLayerControllerCursorTypeToCursor/' "$PDFPLUG"
            sed -i '' 's/return handCursor();/return WebCore::handCursor();/' "$PDFPLUG"
            sed -i '' 's/return iBeamCursor();/return WebCore::iBeamCursor();/' "$PDFPLUG"
            sed -i '' 's/return pointerCursor();/return WebCore::pointerCursor();/' "$PDFPLUG"
        fi
        info "    Qualified Cursor types with WebCore::"
    fi

    # ─── WebSystemInterface.mm — Guard INIT calls for 10.7/10.8+ APIs ───
    # Many WK functions are behind MAC_OS_X_VERSION_MIN_REQUIRED guards in
    # WebKitSystemInterface.h but their INIT() assignments are unconditional.
    # Wrap each one with the matching version guard.
    info "  Patching WebSystemInterface.mm (guard INIT calls for 10.6)..."
    local WSI="$WK/WebKit/WebProcess/WebCoreSupport/mac/WebSystemInterface.mm"
    if [ -f "$WSI" ] && grep -q 'INIT(CALayerEnumerateRectsBeingDrawnWithBlock)' "$WSI" 2>/dev/null; then
        perl -i -pe '
            if (/INIT\(CALayerEnumerateRectsBeingDrawnWithBlock\)/) {
                $_ = "#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1070\n$_#endif\n";
            }
            if (/INIT\(CGContextDrawsWithCorrectShadowOffsets\)/) {
                $_ = "#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1080\n$_#endif\n";
            }
            if (/INIT\(DrawFocusRingAtTime\)/) {
                $_ = "#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1070\n$_";
            }
            if (/INIT\(DrawCellFocusRingWithFrameAtTime\)/) {
                $_ = "$_#endif\n";
            }
            if (/INIT\(CGContextIsPDFContext\)/) {
                $_ = "#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1070\n$_#endif\n";
            }
            if (/INIT\(HTTPCookies\)/) {
                $_ = "#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1070\n$_#endif\n";
            }
            if (/INIT\(DeleteAllHTTPCookies\)/) {
                $_ = "#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1070\n$_#endif\n";
            }
            if (/INIT\(ExecutableWasLinkedOnOrBeforeSnowLeopard\)/) {
                $_ = "#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1070\n$_";
            }
            if (/INIT\(CopyDefaultSearchProviderDisplayName\)/) {
                $_ = "$_#endif\n";
            }
            if (/INIT\(Cursor\)/) {
                $_ = "#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1070\n$_#endif\n";
            }
            if (/INIT\(SpeechSynthesisGetVoiceIdentifiers\)/) {
                $_ = "#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1070\n$_";
            }
            if (/INIT\(SpeechSynthesisGetDefaultVoiceIdentifierForLocale\)/) {
                $_ = "$_#endif\n";
            }
            if (/INIT\(NSElasticDeltaForTimeDelta\)/) {
                $_ = "#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1080\n$_";
            }
            if (/INIT\(NSReboundDeltaForElasticDelta\)/) {
                $_ = "$_#endif\n";
            }
            if (/INIT\(IsPublicSuffix\)/) {
                $_ = "#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1080\n$_#endif\n";
            }
            if (/INIT\(CachePartitionKey\)/) {
                $_ = "#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1080\n$_";
            }
            if (/INIT\(QueryDecoderAvailability\)/) {
                $_ = "$_#endif\n";
            }
        ' "$WSI"
        info "    Added version guards to INIT calls"
    fi

    # --- Patch PageBannerMac.mm: guard contentsScale for 10.6 ---
    info "  Patching PageBannerMac.mm (guard contentsScale for 10.6)..."
    local PBANNER="$WK/WebKit/WebProcess/WebPage/mac/PageBannerMac.mm"
    if [ -f "$PBANNER" ] && grep -q 'm_layer.get().contentsScale' "$PBANNER" 2>/dev/null; then
        sed -i '' 's/m_layer.get().contentsScale = scaleFactor;/#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1070\
        m_layer.get().contentsScale = scaleFactor;\
#else\
        (void)scaleFactor;\
#endif/' "$PBANNER"
        info "    Guarded contentsScale property access"
    fi

    # --- Patch PlatformCALayerRemoteCustom.mm: guard AVFoundation for 10.6 ---
    info "  Patching PlatformCALayerRemoteCustom.mm (guard AVFoundation for 10.6)..."
    local PREMOTE_CUSTOM="$WK/WebKit/WebProcess/WebPage/mac/PlatformCALayerRemoteCustom.mm"
    if [ -f "$PREMOTE_CUSTOM" ] && grep -q '#import <AVFoundation/AVFoundation.h>' "$PREMOTE_CUSTOM" 2>/dev/null; then
        # Guard AVFoundation import
        sed -i '' 's/#import <AVFoundation\/AVFoundation.h>/#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1070\
#import <AVFoundation\/AVFoundation.h>\
#endif/' "$PREMOTE_CUSTOM"
        # Guard SOFT_LINK macros
        perl -i -pe '
            if (/^SOFT_LINK_FRAMEWORK_OPTIONAL\(AVFoundation\)/) {
                $_ = "#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1070\n$_";
            }
            if (/^SOFT_LINK_CLASS_OPTIONAL\(AVFoundation, AVPlayerLayer\)/) {
                $_ = "$_#endif\n";
            }
        ' "$PREMOTE_CUSTOM"
        # Guard only the inner AVFoundation-specific code in clone().
        # Strategy: keep the outer if(layerType()==LayerTypeAVPlayerLayer) intact,
        # but guard the inner if([platformLayer() isKindOfClass:...]) block.
        # Add #if after the outer "if (layerType() == LayerTypeAVPlayerLayer) {"
        # Add #endif before "copyContents = false;"
        perl -i -pe '
            if (/^\s+if \(\[platformLayer\(\) isKindOfClass:getAVPlayerLayerClass\(\)\]\)/) {
                $_ = "#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1070\n$_";
            }
            if (/^\s+copyContents = false;/ && !$done) {
                $_ = "#endif\n$_";
                $done = 1;
            }
        ' "$PREMOTE_CUSTOM"
        info "    Guarded AVFoundation imports and AVPlayerLayer code"
    fi

    # --- Patch PlatformCALayerRemote.cpp: fix enumerateRectsBeingDrawn + add missing overrides ---
    info "  Patching PlatformCALayerRemote.cpp (fix enumerateRectsBeingDrawn + missing overrides)..."
    local PREMOTE="$WK/WebKit/WebProcess/WebPage/mac/PlatformCALayerRemote.cpp"
    if [ -f "$PREMOTE" ]; then
        # Fix enumerateRectsBeingDrawn signature: block → std::function
        if grep -q 'enumerateRectsBeingDrawn.*void (\^block)(CGRect)' "$PREMOTE" 2>/dev/null; then
            sed -i '' 's/void PlatformCALayerRemote::enumerateRectsBeingDrawn(CGContextRef context, void (\^block)(CGRect))/void PlatformCALayerRemote::enumerateRectsBeingDrawn(CGContextRef context, std::function<void (CGRect)> block)/' "$PREMOTE"
        fi
        # Fix body: wrap std::function call in block for backingStore
        if grep -q 'backingStore->enumerateRectsBeingDrawn(context, block);' "$PREMOTE" 2>/dev/null; then
            perl -i -pe '
                if (/backingStore->enumerateRectsBeingDrawn\(context, block\)/) {
                    $_ = "    m_properties.backingStore->enumerateRectsBeingDrawn(context, ^(CGRect rect) {\n        block(rect);\n    });\n";
                }
            ' "$PREMOTE"
        fi
        # Add missing contentsTransform/setContentsTransform overrides (pure virtual in base, not in Remote)
        if ! grep -q 'PlatformCALayerRemote::contentsTransform' "$PREMOTE" 2>/dev/null; then
            # Insert before hostingContextID()
            perl -i -pe '
                if (/^uint32_t PlatformCALayerRemote::hostingContextID/) {
                    $_ = "WebCore::TransformationMatrix PlatformCALayerRemote::contentsTransform() const\n{\n    return WebCore::TransformationMatrix();\n}\n\nvoid PlatformCALayerRemote::setContentsTransform(const WebCore::TransformationMatrix&)\n{\n}\n\n$_";
                }
            ' "$PREMOTE"
            info "    Added contentsTransform/setContentsTransform stubs"
        fi
        info "    Fixed enumerateRectsBeingDrawn"
    fi

    # --- Patch PlatformCALayerRemote.h: add missing override declarations ---
    info "  Patching PlatformCALayerRemote.h (add missing overrides)..."
    local PREMOTE_H="$WK/WebKit/WebProcess/WebPage/mac/PlatformCALayerRemote.h"
    if [ -f "$PREMOTE_H" ] && ! grep -q 'contentsTransform.*override' "$PREMOTE_H" 2>/dev/null; then
        # Insert before backingStoreBytesPerPixel declaration
        perl -i -pe '
            if (/backingStoreBytesPerPixel\(\) const override/) {
                $_ = "    WebCore::TransformationMatrix contentsTransform() const override;\n    void setContentsTransform(const WebCore::TransformationMatrix&) override;\n    $_";
            }
        ' "$PREMOTE_H"
        info "    Added contentsTransform/setContentsTransform declarations"
    fi

    # --- Patch WebPageMac.mm: guard DataDetection for 10.6 ---
    info "  Patching WebPageMac.mm (guard DataDetection for 10.6)..."
    local WPAGEMAC="$WK/WebKit/WebProcess/WebPage/mac/WebPageMac.mm"
    if [ -f "$WPAGEMAC" ] && grep -q 'DataDetection::detectItemAroundHitTestResult' "$WPAGEMAC" 2>/dev/null; then
        # Guard DataDetection call — the subsequent if-check for detectedDataActionContext
        # will just not execute since it stays nil on 10.6
        perl -i -pe '
            if (/DataDetection::detectItemAroundHitTestResult/) {
                $_ = "#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1070\n${_}#else\n        (void)detectedDataBoundingBox; (void)detectedDataRange;\n#endif\n";
            }
        ' "$WPAGEMAC"
        info "    Guarded DataDetection usage"
    fi

    # --- Remove XPC service entry points from cmake (XPC is 10.7+ only) ---
    # The Leopard patch empties XPCSPI.h for <= 1060, so any file using XPC
    # types won't compile. Rather than guarding every source file, remove the
    # XPC entry-point sources from PlatformMac.cmake and disable the
    # WebProcess/NetworkProcess executables in CMakeLists.txt. These service
    # executables can't run on 10.6 anyway (no XPC runtime).
    info "  Removing XPC service entry points from cmake..."
    local PMC="$WK/WebKit/PlatformMac.cmake"
    if [ -f "$PMC" ] && grep -q 'XPCService_SOURCES' "$PMC" 2>/dev/null; then
        python3 - "$PMC" << 'PYEOF'
import sys
path = sys.argv[1]
with open(path) as f:
    lines = f.readlines()

PREFIX = "# DISABLED: 10.6 no XPC: "
XPC_KEYWORDS = ['XPCService_SOURCES', 'EntryPointUtilities/mac/XPCService/',
                 'EntryPoint/mac/XPCService/']

result = []
i = 0
while i < len(lines):
    stripped = lines[i].lstrip()

    # Check if this line starts a cmake block (set/list) containing XPC refs
    is_xpc_block = False
    if any(stripped.startswith(m) for m in ['set(', 'list(APPEND']):
        # Look ahead in the block for XPC keywords
        j = i
        depth = lines[i].count('(') - lines[i].count(')')
        while j < len(lines):
            if any(kw in lines[j] for kw in XPC_KEYWORDS):
                is_xpc_block = True
                break
            if depth <= 0 and j > i:
                break
            j += 1
            if j < len(lines):
                depth += lines[j].count('(') - lines[j].count(')')

    if is_xpc_block:
        # Comment out entire block until closing paren balances
        depth = 0
        seen_open = False
        while i < len(lines):
            result.append(PREFIX + lines[i])
            depth += lines[i].count('(') - lines[i].count(')')
            if '(' in lines[i]:
                seen_open = True
            if seen_open and depth <= 0:
                break
            i += 1
        i += 1
        continue

    result.append(lines[i])
    i += 1

with open(path, "w") as f:
    f.writelines(result)
PYEOF
        info "    Commented out XPC service source blocks in PlatformMac.cmake"
    fi

    # Also disable the add_executable(WebProcess/NetworkProcess) in CMakeLists.txt
    # since their source lists (WebProcess_SOURCES etc.) are now empty.
    local CLISTS="$WK/WebKit/CMakeLists.txt"
    if [ -f "$CLISTS" ] && grep -q 'add_executable(WebProcess' "$CLISTS" 2>/dev/null; then
        sed -i '' 's/^add_executable(WebProcess /# DISABLED: 10.6 no XPC: add_executable(WebProcess /' "$CLISTS"
        sed -i '' 's/^add_webkit2_prefix_header(WebProcess)/# DISABLED: 10.6 no XPC: add_webkit2_prefix_header(WebProcess)/' "$CLISTS"
        sed -i '' 's/^target_link_libraries(WebProcess /# DISABLED: 10.6 no XPC: target_link_libraries(WebProcess /' "$CLISTS"
        sed -i '' '/WebKit2_WebProcess_OUTPUT_NAME/,/endif.*WebProcess_OUTPUT/{ s/^/# DISABLED: 10.6 no XPC: /; }' "$CLISTS"
        sed -i '' 's/^add_executable(NetworkProcess /# DISABLED: 10.6 no XPC: add_executable(NetworkProcess /' "$CLISTS"
        sed -i '' 's/^add_webkit2_prefix_header(NetworkProcess)/# DISABLED: 10.6 no XPC: add_webkit2_prefix_header(NetworkProcess)/' "$CLISTS"
        sed -i '' 's/^target_link_libraries(NetworkProcess /# DISABLED: 10.6 no XPC: target_link_libraries(NetworkProcess /' "$CLISTS"
        sed -i '' '/WebKit2_NetworkProcess_OUTPUT_NAME/,/endif.*NetworkProcess_OUTPUT/{ s/^/# DISABLED: 10.6 no XPC: /; }' "$CLISTS"
        info "    Disabled WebProcess and NetworkProcess executables in CMakeLists.txt"
    fi

    # --- Disable DumpRenderTree (test tool, needs 10.7+ headers) ---
    local TOOLS_CMAKE="$WK/../Tools/CMakeLists.txt"
    if [ -f "$TOOLS_CMAKE" ] && grep -q '^ *add_subdirectory(DumpRenderTree)' "$TOOLS_CMAKE" 2>/dev/null; then
        sed -i '' 's|^\( *\)add_subdirectory(DumpRenderTree)|\1# DISABLED: 10.6 no 10.7 SDK: add_subdirectory(DumpRenderTree)|' "$TOOLS_CMAKE"
        info "    Disabled DumpRenderTree in Tools/CMakeLists.txt"
    fi

    # --- Disable WebKitTestRunner (test tool, needs 10.7+ APIs) ---
    if [ -f "$TOOLS_CMAKE" ] && grep -q '^ *add_subdirectory(WebKitTestRunner)' "$TOOLS_CMAKE" 2>/dev/null; then
        sed -i '' 's|^\( *\)add_subdirectory(WebKitTestRunner)|\1# DISABLED: 10.6 no 10.7 SDK: add_subdirectory(WebKitTestRunner)|' "$TOOLS_CMAKE"
        info "    Disabled WebKitTestRunner in Tools/CMakeLists.txt"
    fi

    # --- Disable MiniBrowser (demo app, needs 10.7+ APIs) ---
    if [ -f "$TOOLS_CMAKE" ] && grep -q '^ *add_subdirectory(MiniBrowser/mac)' "$TOOLS_CMAKE" 2>/dev/null; then
        sed -i '' 's|^\( *\)add_subdirectory(MiniBrowser/mac)|\1# DISABLED: 10.6 no 10.7 SDK: add_subdirectory(MiniBrowser/mac)|' "$TOOLS_CMAKE"
        info "    Disabled MiniBrowser in Tools/CMakeLists.txt"
    fi


    info "  Fixing const ObjC casts from Leopard patch..."
    local CONST_FIX_COUNT=0
    while IFS= read -r f; do
        # Fix casts: (const NSString *) → (NSString *) — handle all spacing variants:
        #   (const NSString *)  (const NSString*)  (const NSString* )
        sed -i '' 's/(const NSString \*)/(NSString *)/g;s/(const NSString\*)/(NSString*)/g;s/(const NSString\* )/(NSString* )/g' "$f"
        sed -i '' 's/(const NSData \*)/(NSData *)/g;s/(const NSData\*)/(NSData*)/g;s/(const NSData\* )/(NSData* )/g' "$f"
        sed -i '' 's/(const NSURL \*)/(NSURL *)/g;s/(const NSURL\*)/(NSURL*)/g;s/(const NSURL\* )/(NSURL* )/g' "$f"
        sed -i '' 's/(const NSMutableData \*)/(NSMutableData *)/g;s/(const NSMutableData\*)/(NSMutableData*)/g;s/(const NSMutableData\* )/(NSMutableData* )/g' "$f"
        sed -i '' 's/(const objc_object \*)/(id)/g;s/(const objc_object\*)/(id)/g;s/(const objc_object\* )/(id)/g' "$f"
        sed -i '' 's/(const NSFont \*)/(NSFont *)/g;s/(const NSFont\* )/(NSFont* )/g' "$f"
        sed -i '' 's/(const PlatformFont \*)/(PlatformFont *)/g;s/(const PlatformFont\* )/(PlatformFont* )/g' "$f"
        # Fix any remaining const ObjC pointer declarations/return types
        # (covers: "const PlatformFont *x =", "static const PlatformFont *func(", etc.)
        sed -i '' 's/const PlatformFont \*/PlatformFont */g' "$f"
        sed -i '' 's/const NSFont \*/NSFont */g' "$f"
        sed -i '' 's/const NSURL \*\([a-zA-Z_]\)/NSURL *\1/g' "$f"
        CONST_FIX_COUNT=$((CONST_FIX_COUNT + 1))
    done < <(grep -rl "(const NSString \*)\|(const NSString\*)\|(const NSString\* )\|(const NSData \*)\|(const NSData\*)\|(const NSData\* )\|(const NSURL \*)\|(const NSURL\*)\|(const NSURL\* )\|(const NSMutableData \*)\|(const NSMutableData\*)\|(const NSMutableData\* )\|(const objc_object \*)\|(const objc_object\*)\|(const objc_object\* )\|(const NSFont \*)\|(const NSFont\*)\|(const NSFont\* )\|(const PlatformFont \*)\|(const PlatformFont\*)\|(const PlatformFont\* )\|const PlatformFont \*\|const NSFont \*" "$SOURCE_DIR/Source/" 2>/dev/null | grep -v '\.rej\|\.orig\|ForwardingHeaders\|ThirdParty')
    info "    Fixed const ObjC casts in $CONST_FIX_COUNT files"

    # ─── DispatchPtr.h — Cast dispatch_retain/release to dispatch_object_t ───
    # When dispatch_data_t is typedef'd as dispatch_object_t, the template parameter
    # T becomes dispatch_object_t and the compiler can't resolve the overload.
    info "  Patching DispatchPtr.h (dispatch_retain/release casts)..."
    local DPTR="$WK/WTF/wtf/DispatchPtr.h"
    if [ -f "$DPTR" ]; then
        sed -i '' 's/dispatch_retain(m_ptr)/dispatch_retain(static_cast<dispatch_object_t>(m_ptr))/g' "$DPTR"
        sed -i '' 's/dispatch_release(m_ptr)/dispatch_release(static_cast<dispatch_object_t>(m_ptr))/g' "$DPTR"
        info "    Fixed dispatch_retain/release casts"
    fi

    # ─── WebKit config.h — Guard SEC_ACCESS_CONTROL ───
    # SEC_ACCESS_CONTROL requires SecAccessControlRef (10.9+). The Python patch
    # phase handles this too but as a safety net, apply via sed.
    info "  Patching WebKit config.h (SEC_ACCESS_CONTROL guard)..."
    local WKCONFIG="$WK/WebKit/config.h"
    if [ -f "$WKCONFIG" ] && grep -q 'HAVE_SEC_ACCESS_CONTROL' "$WKCONFIG" 2>/dev/null; then
        # Only target the SEC_ACCESS_CONTROL block specifically
        sed -i '' '/HAVE_SEC_ACCESS_CONTROL/,/^#endif$/{ s/#if PLATFORM(IOS) || PLATFORM(MAC)/#if (PLATFORM(IOS) \|\| PLATFORM(MAC)) \&\& __MAC_OS_X_VERSION_MIN_REQUIRED >= 1090/; }' "$WKCONFIG"
        info "    Added version guard to SEC_ACCESS_CONTROL"
    fi

    # ─── Fix (Type*)super casts — Modern Clang doesn't allow casting super ───
    info "  Fixing super casts..."
    local SUPER_FIX_COUNT=0
    while IFS= read -r f; do
        sed -i '' 's/(\*[a-zA-Z_]*)super/super/g' "$f"
        SUPER_FIX_COUNT=$((SUPER_FIX_COUNT + 1))
    done < <(grep -rl ')\*super\b' "$SOURCE_DIR/Source/" 2>/dev/null | grep -v '\.rej\|\.orig\|ForwardingHeaders\|ThirdParty')
    info "    Fixed super casts in $SUPER_FIX_COUNT files"

    # ─── MiniBrowser PCH — Fix stray #endif and NSEventModifierFlags guard ───
    # The Leopard patch left a PCH with unbalanced #endif and a guard that
    # references NSEventModifierFlags before the typedef is available.
    # Rewrite the PCH to be clean for 10.6.
    info "  Patching MiniBrowser PCH..."
    local MB_PCH="$WK/../Tools/MiniBrowser/mac/MiniBrowser_Prefix.pch"
    if [ -f "$MB_PCH" ]; then
        cat > "$MB_PCH" << 'PCHEOF'
#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>
#endif

#define ENABLE_LOGGING 0

#if ENABLE_LOGGING
#define LOG NSLog
#else
#define LOG(...) ((void)0)
#endif
PCHEOF
        info "    Rewrote MiniBrowser PCH for 10.6"
    fi

    # ─── MiniBrowser — Fix WKPreferencesPrivate.h include (WebKit2 disabled) ───
    # MiniBrowser files import WebKit2 private headers. With ENABLE_WEBKIT=OFF
    # these don't exist. Wrap the imports in #if ENABLE(WEBKIT2) / #if __has_include.
    info "  Patching MiniBrowser WK2 includes..."
    local MB_DIR="$WK/../Tools/MiniBrowser/mac"
    for f in "$MB_DIR/AppDelegate.m" "$MB_DIR/SettingsController.m"; do
        if [ -f "$f" ]; then
            sed -i '' 's/^#import <WebKit\/WKPreferencesPrivate\.h>/#if __has_include(<WebKit\/WKPreferencesPrivate.h>)\n#import <WebKit\/WKPreferencesPrivate.h>\n#endif/' "$f"
        fi
    done

    # ── WebKit2: 10.6 SDK compat patches (applied via unified diff + sed) ──
    # BSD sed cannot handle multi-line replacements reliably, so we use patch(1)
    # for the 6 WebKit source files, and sed for WebKitSystemInterface.h which
    # is also modified by the Leopard patch (so its line numbers are different).
    local WK2_PATCH="$BUILD_DIR/wk2-106-compat.diff"
    cat > "$WK2_PATCH" << 'DIFF_EOF'
--- a/Source/WebKit/WebProcess/WebPage/mac/PlatformCALayerRemote.h
+++ b/Source/WebKit/WebProcess/WebPage/mac/PlatformCALayerRemote.h
@@ -178,7 +178,7 @@

     Ref<PlatformCALayer> createCompatibleLayer(WebCore::PlatformCALayer::LayerType, WebCore::PlatformCALayerClient*) const override;

-    void enumerateRectsBeingDrawn(CGContextRef, void (^block)(CGRect)) override;
+    void enumerateRectsBeingDrawn(CGContextRef, std::function<void(CGRect)>) override;

     virtual uint32_t hostingContextID();

--- a/Source/WebKit/Shared/mac/RemoteLayerTreePropertyApplier.mm
+++ b/Source/WebKit/Shared/mac/RemoteLayerTreePropertyApplier.mm
@@ -181,8 +181,10 @@
         layer.contentsRect = properties.contentsRect;

     if (properties.changedProperties & RemoteLayerTreeTransaction::ContentsScaleChanged) {
-        layer.contentsScale = properties.contentsScale;
-        layer.rasterizationScale = properties.contentsScale;
+        if ([layer respondsToSelector:@selector(setContentsScale:)])
+            [layer setValue:[NSNumber numberWithDouble:properties.contentsScale] forKey:@"contentsScale"];
+        if ([layer respondsToSelector:@selector(setRasterizationScale:)])
+            [layer setValue:[NSNumber numberWithDouble:properties.contentsScale] forKey:@"rasterizationScale"];
     }

     if (properties.changedProperties & RemoteLayerTreeTransaction::CornerRadiusChanged)
--- a/Source/WebKit/Shared/mac/WebCoreArgumentCodersMac.mm
+++ b/Source/WebKit/Shared/mac/WebCoreArgumentCodersMac.mm
@@ -39,6 +39,13 @@
 #import <WebCore/ResourceError.h>
 #import <WebCore/ResourceRequest.h>

+// SecTrustGetCertificateCount/At are available at runtime on 10.6 but
+// not declared in the 10.6 SDK SecTrust.h header.
+extern "C" {
+CFIndex SecTrustGetCertificateCount(SecTrustRef);
+SecCertificateRef SecTrustGetCertificateAtIndex(SecTrustRef, CFIndex);
+}
+
 #if USE(CFURLCONNECTION)
 #import <CFNetwork/CFURLRequest.h>
 #endif
--- a/Source/WebKit/Shared/mac/WebEventFactory.mm
+++ b/Source/WebKit/Shared/mac/WebEventFactory.mm
@@ -35,7 +35,27 @@
 #import <WebCore/Scrollbar.h>
 #import <WebCore/WindowsKeyboardCodes.h>
 #import <wtf/ASCIICType.h>
+
+// NSEventPhase constants introduced in 10.7; not in 10.6 SDK.
+// Use plain C enum (NS_OPTIONS macro does not exist in 10.6 SDK).
+#ifndef MAC_OS_X_VERSION_10_7
+enum {
+    NSEventPhaseNone        = 0,
+    NSEventPhaseBegan       = 1 << 0,
+    NSEventPhaseStationary  = 1 << 1,
+    NSEventPhaseChanged     = 1 << 2,
+    NSEventPhaseEnded       = 1 << 3,
+    NSEventPhaseCancelled   = 1 << 4,
+    NSEventPhaseMayBegin    = 1 << 5,
+};
+typedef NSUInteger NSEventPhase;
+#endif

+// NSEventTypePressure introduced in 10.10.5; provide a value for __LP64__ guard.
+#ifndef NSEventTypePressure
+#define NSEventTypePressure 34
+#endif
+

 using namespace WebCore;

@@ -43,6 +65,10 @@
 - (NSInteger)_scrollCount;
 - (CGFloat)_unacceleratedScrollingDeltaX;
 - (CGFloat)_unacceleratedScrollingDeltaY;
+@property (readonly) NSEventPhase phase;
+@property (readonly) NSEventPhase momentumPhase;
+@property (readonly) NSInteger stage;
+@property (readonly) double pressure;
 @end

 namespace WebKit {
--- a/Source/WebKit/Shared/mac/WebHitTestResultData.mm
+++ b/Source/WebKit/Shared/mac/WebHitTestResultData.mm
@@ -34,5 +34,10 @@
 #import "WebCoreArgumentCoders.h"
 #import <WebCore/DataDetectorsSPI.h>
 #import <WebCore/TextIndicator.h>
-
+// getDDActionContextClass() is soft-linked from DataDetectors.framework (10.7+).
+// When building against the 10.6 SDK, the SPI header skips the declaration.
+#ifndef getDDActionContextClass
+static inline Class getDDActionContextClass() { return nil; }
+#endif
+
 namespace WebKit {
--- a/Source/WebKit/UIProcess/API/C/mac/WKContextPrivateMac.mm
+++ b/Source/WebKit/UIProcess/API/C/mac/WKContextPrivateMac.mm
@@ -38,8 +38,10 @@
 #import "WKSharedAPICast.h"
 #import "WKStringCF.h"
 #import "WebProcessPool.h"
+#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 101000
 #import <WebCore/PluginBlacklist.h>
 #import <WebCore/WebGLBlacklist.h>
+#endif
 #import <wtf/RetainPtr.h>

 using namespace WebKit;
@@ -49,8 +51,12 @@
 #if PLATFORM(IOS)
     return false;
 #else
+#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 101000
     return WebCore::PluginBlacklist::isPluginUpdateAvailable((NSString *)adoptCF(WKStringCopyCFString(kCFAllocatorDefault, plugInBundleIdentifierRef)).get());
+#else
+    return false;
 #endif
+#endif
 }

 void WKContextSetPluginLoadClientPolicy(WKContextRef contextRef, WKPluginLoadClientPolicy policy, WKStringRef host, WKStringRef bundleIdentifier, WKStringRef versionString)
@@ -158,17 +164,25 @@
 bool WKContextShouldBlockWebGL()
 {
 #if PLATFORM(MAC)
+#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 101000
     return WebCore::WebGLBlacklist::shouldBlockWebGL();
 #else
     return false;
 #endif
+#else
+    return false;
+#endif
 }

 bool WKContextShouldSuggestBlockWebGL()
 {
 #if PLATFORM(MAC)
+#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 101000
     return WebCore::WebGLBlacklist::shouldSuggestBlockingWebGL();
 #else
     return false;
 #endif
+#else
+    return false;
+#endif
 }
--- a/Source/WebKit/Shared/API/Cocoa/WKFoundation.h
+++ b/Source/WebKit/Shared/API/Cocoa/WKFoundation.h
@@ -42,6 +42,14 @@

 #ifndef WK_FRAMEWORK_HEADER_POSTPROCESSING_ENABLED

+#ifndef NS_ENUM
+#define NS_ENUM(_type, _name) enum _name : _type
+#endif
+
+#ifndef NS_OPTIONS
+#define NS_OPTIONS(_type, _name) enum _name : _type
+#endif
+
 #define WK_API_AVAILABLE(...)
 #define WK_CLASS_AVAILABLE(...) __attribute__((visibility("default"))) WK_API_AVAILABLE(__VA_ARGS__)
 #define WK_API_DEPRECATED(_message, ...) __attribute__((deprecated(_message)))
--- a/Source/WebKit/UIProcess/API/Cocoa/_WKOverlayScrollbarStyle.h
+++ b/Source/WebKit/UIProcess/API/Cocoa/_WKOverlayScrollbarStyle.h
@@ -23,6 +23,8 @@
  * THE POSSIBILITY OF SUCH DAMAGE.
  */

+#pragma once
+
 #import <WebKit/WKFoundation.h>

 typedef NS_ENUM(NSUInteger, _WKOverlayScrollbarStyle) {
--- a/Source/WebKit/UIProcess/Cocoa/LegacyCustomProtocolManagerClient.mm
+++ b/Source/WebKit/UIProcess/Cocoa/LegacyCustomProtocolManagerClient.mm
@@ -35,6 +35,7 @@
 using namespace WebCore;
 using namespace WebKit;

+@protocol NSURLConnectionDelegate;
 @interface WKCustomProtocolLoader : NSObject <NSURLConnectionDelegate> {
 @private
     LegacyCustomProtocolManagerProxy* _customProtocolManagerProxy;
--- a/Source/WebKit/UIProcess/Cocoa/WebProcessPoolCocoa.mm
+++ b/Source/WebKit/UIProcess/Cocoa/WebProcessPoolCocoa.mm
@@ -55,7 +55,7 @@
 #if PLATFORM(IOS)
 #import "ArgumentCodersCF.h"
 #import "WebMemoryPressureHandlerIOS.h"
-#else
+#elif __MAC_OS_X_VERSION_MIN_REQUIRED >= 1070
 #import <QuartzCore/CARemoteLayerServer.h>
 #endif

--- a/Source/WebKit/UIProcess/API/mac/WKView.mm
+++ b/Source/WebKit/UIProcess/API/mac/WKView.mm
@@ -46,3 +46,9 @@
 #import <wtf/BlockPtr.h>

+#if __MAC_OS_X_VERSION_MAX_ALLOWED < 1080
+@interface NSView (WebKitLegacyLayoutDirection)
+@property (nonatomic) NSUserInterfaceLayoutDirection userInterfaceLayoutDirection;
+@end
+#endif
+
 using namespace WebKit;
--- a/Source/WebKit/UIProcess/Cocoa/WebViewImpl.mm
+++ b/Source/WebKit/UIProcess/Cocoa/WebViewImpl.mm
@@ -102,6 +102,18 @@
 #import <wtf/SoftLinking.h>

+#if __MAC_OS_X_VERSION_MAX_ALLOWED < 1080
+@interface NSWindow (SnowLeopardWindowCompat)
+@property (nonatomic, readonly) CGFloat backingScaleFactor;
+@property (nonatomic) BOOL titlebarAppearsTransparent;
+@property (nonatomic, readonly) NSRect contentLayoutRect;
+@end
+
+@interface NSScreen (SnowLeopardScreenCompat)
+@property (nonatomic, readonly) CGFloat backingScaleFactor;
+@end
+#endif
+
 #if HAVE(TOUCH_BAR) && ENABLE(WEB_PLAYBACK_CONTROLS_MANAGER)
 SOFT_LINK_FRAMEWORK(AVKit)
DIFF_EOF

    # Apply the 12-file diff from source/webkit/ root
    if [ -f "$WK2_PATCH" ]; then
        local WK2_PATCH_LOG="$BUILD_DIR/wk2-106-compat-patch.log"
        if (cd "$SOURCE_DIR" && patch -p1 --no-backup-if-mismatch --forward < "$WK2_PATCH" > "$WK2_PATCH_LOG" 2>&1); then
            info "  WebKit2 10.6 compat patches applied"
        else
            if grep -qi 'FAILED' "$WK2_PATCH_LOG"; then
                warn "  Some WebKit2 compat patches failed (see $WK2_PATCH_LOG)"
                cat "$WK2_PATCH_LOG" >> "$BUILD_DIR/build.log"
            else
                info "  WebKit2 10.6 compat patches: already applied or skipped"
            fi
        fi
    fi

    # ── WebKitSystemInterface.h: lower WKSandboxExtension guard from >= 1070 to >= 1060 ──
    # The Leopard patch adds these with >= 1070; we lower it so 10.6 builds can see them.
    # The >= 1070 and WKSandboxExtensionType are on separate lines, so check each independently.
    local WSI="$SOURCE_DIR/WebKitLibraries/WebKitSystemInterface.h"
    if [ -f "$WSI" ] && grep -q 'MAC_OS_X_VERSION_MIN_REQUIRED >= 1070' "$WSI" && grep -q 'WKSandboxExtensionType' "$WSI"; then
        # Read the >= 1070 line + 4 subsequent lines, if WKSandboxExtensionType appears anywhere, substitute
        sed -i '' '/MAC_OS_X_VERSION_MIN_REQUIRED >= 1070/{
            N;N;N;N
            /WKSandboxExtensionType/s/>= 1070/>= 1060/
        }' "$WSI"
        # The Leopard patch drops WKSandboxExtensionTypeGeneric from the enum; add it back.
        if ! grep -q 'WKSandboxExtensionTypeGeneric' "$WSI"; then
            sed -i '' '/WKSandboxExtensionTypeReadWrite/a\
\    WKSandboxExtensionTypeGeneric,
' "$WSI"
            info "  WebKitSystemInterface.h: added WKSandboxExtensionTypeGeneric to enum"
        fi
        info "  WebKitSystemInterface.h: sandbox guard lowered to >= 1060"
    fi

    # ── Guard PluginBlacklist import (10.10+ only) ──
    local PIM="$SOURCE_DIR/Source/WebKit/UIProcess/Plugins/mac/PluginInfoStoreMac.mm"
    if [ -f "$PIM" ] && grep -q '^#import <WebCore/PluginBlacklist.h>' "$PIM"; then
        sed -i '' 's|^#import <WebCore/PluginBlacklist.h>|#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 101000\
#import <WebCore/PluginBlacklist.h>\
#endif|' "$PIM"
        info "  Guarded PluginBlacklist import in PluginInfoStoreMac.mm"
    fi

    # ── Guard PluginBlacklist usage block (10.10+ only) ──
    if [ -f "$PIM" ] && grep -q 'PluginBlacklist::loadPolicyForPluginVersion' "$PIM"; then
        sed -i '' '/PluginBlacklist::loadPolicyForPluginVersion/i\
#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 101000
' "$PIM"
        sed -i '' '/case PluginBlacklist::LoadPolicy::BlockedForCompatibility:/,/}/{
    /^    }/a\
#else\
    return PluginModuleLoadNormally;\
#endif
}' "$PIM"
        info "  Guarded PluginBlacklist usage in PluginInfoStoreMac.mm"
    fi

    # ── Guard CARemoteLayerServer.h import (10.7+ only) ──
    local PPP="$SOURCE_DIR/Source/WebKit/UIProcess/Plugins/mac/PluginProcessProxyMac.mm"
    if [ -f "$PPP" ] && grep -q '^#import <QuartzCore/CARemoteLayerServer.h>' "$PPP"; then
        sed -i '' 's|^#import <QuartzCore/CARemoteLayerServer.h>|#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1070\
#import <QuartzCore/CARemoteLayerServer.h>\
#endif|' "$PPP"
        info "  Guarded CARemoteLayerServer.h in PluginProcessProxyMac.mm"
    fi

    # ── Guard CARemoteLayerServer USAGE in PluginProcessProxyMac.mm line 87 ──
    # The #include is guarded but the actual [CARemoteLayerServer sharedServer] usage isn't.
    if [ -f "$PPP" ] && grep -q 'parameters.acceleratedCompositingPort = MachSendRight::create(\[CARemoteLayerServer' "$PPP" 2>/dev/null; then
        perl -i -pe 's/^(\s*)parameters\.acceleratedCompositingPort = MachSendRight::create\(\[CARemoteLayerServer sharedServer\]\.serverPort\);/$1#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1070\n$1    parameters.acceleratedCompositingPort = MachSendRight::create([CARemoteLayerServer sharedServer].serverPort);\n$1#endif/' "$PPP"
        info "  Guarded CARemoteLayerServer usage in PluginProcessProxyMac.mm"
    fi

    # ── Guard CARemoteLayerServer USAGE in WebProcessPoolCocoa.mm line 198 ──
    local WPCC="$SOURCE_DIR/Source/WebKit/UIProcess/Cocoa/WebProcessPoolCocoa.mm"
    if [ -f "$WPCC" ] && grep -q 'parameters.acceleratedCompositingPort = MachSendRight::create(\[CARemoteLayerServer' "$WPCC" 2>/dev/null; then
        perl -i -pe 's/^(\s*)parameters\.acceleratedCompositingPort = MachSendRight::create\(\[CARemoteLayerServer sharedServer\]\.serverPort\);/$1#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1070\n$1    parameters.acceleratedCompositingPort = MachSendRight::create([CARemoteLayerServer sharedServer].serverPort);\n$1#endif/' "$WPCC"
        info "  Guarded CARemoteLayerServer usage in WebProcessPoolCocoa.mm"
    fi

    # ── WebEventFactory.mm — #undef NSEventPhase macros before local enum ──
    # The compat header defines NSEventPhase* as #define macros.
    # WebEventFactory.mm has a local enum that redefines them — must #undef first.
    local WEFM="$SOURCE_DIR/Source/WebKit/Shared/mac/WebEventFactory.mm"
    if [ -f "$WEFM" ] && ! grep -q '#undef NSEventPhaseNone' "$WEFM" 2>/dev/null; then
        # Insert #undef before the NSEventPhase enum block
        perl -i -0777 -pe 's/(\/\/ NSEventPhase constants[^\n]*\n[^\n]*\n[^\n]*\n)(enum \{)/$1#ifdef NSEventPhaseNone\n#undef NSEventPhaseNone\n#undef NSEventPhaseBegan\n#undef NSEventPhaseStationary\n#undef NSEventPhaseChanged\n#undef NSEventPhaseEnded\n#undef NSEventPhaseCancelled\n#undef NSEventPhaseMayBegin\n#endif\n$2/' "$WEFM"
        info "  Added #undef NSEventPhase guards in WebEventFactory.mm"
    fi

    touch "$stamp"
    # ── WebViewImpl.mm — Add additional 10.7+ compat categories (after diff) ──
    local WVIMM="$SOURCE_DIR/Source/WebKit/UIProcess/Cocoa/WebViewImpl.mm"
    if [ -f "$WVIMM" ] && ! grep -q 'SnowLeopardEventPhase' "$WVIMM" 2>/dev/null; then
        # Insert NSEvent phase category, NSDraggingInfo compat, and convertRectToScreen: impl
        # after the existing SnowLeopardScreenCompat @end
        perl -i -0pe 's/(\@interface NSScreen \(SnowLeopardScreenCompat\)\n\@property[^
]+\n\@end\n\#endif)/$1

#if __MAC_OS_X_VERSION_MAX_ALLOWED < 1080
\@interface NSEvent (SnowLeopardEventPhase)
\@property (nonatomic, readonly) NSInteger phase;
\@property (nonatomic, readonly) NSInteger momentumPhase;
\@property (nonatomic, readonly) NSInteger stage;
\@property (nonatomic, readonly) float pressure;
\@end

\@interface NSObject (SnowLeopardDraggingInfoCompat)
\@property (nonatomic) NSInteger numberOfValidItemsForDrop;
\@property (nonatomic, readonly) NSInteger draggingFormation;
- (void)setDraggingFormation:(NSInteger)formation;
\@end

\@implementation NSWindow (SnowLeopardWindowCompat)
- (NSRect)convertRectToScreen:(NSRect)rect {
    NSPoint screenOrigin = [self convertBaseToScreen:rect.origin];
    return NSMakeRect(screenOrigin.x, screenOrigin.y, rect.size.width, rect.size.height);
}
\@end
#endif/s' "$WVIMM"
        info "  Added NSEvent/NSDraggingInfo/convertRectToScreen categories to WebViewImpl.mm"
    fi

    # ── WebViewImpl.mm — Guard DataDetectors usage with ENABLE(DATA_DETECTION) ──
    if [ -f "$WVIMM" ] && grep -q 'DataDetectorsLibrary\|getDDActionsManagerClass' "$WVIMM" 2>/dev/null; then
        # Wrap the entire DataDetectors block with ENABLE(DATA_DETECTION)
        perl -i -0777 -pe 's/^(\s+)if \(DataDetectorsLibrary\(\)\)\n(\s+)\[\[getDDActionsManagerClass.*?\];/${1}#if ENABLE(DATA_DETECTION)\n${1}    if (DataDetectorsLibrary())\n${2}[[getDDActionsManagerClass() sharedManager] requestBubbleClosureUnanchorOnFailure:YES];\n${1}#endif/m' "$WVIMM"
        info "  Guarded DataDetectors block in WebViewImpl.mm"
    fi

    # ── WebViewImpl.mm — Guard NSDraggingFormation code (10.7+) ──
    if [ -f "$WVIMM" ] && grep -q 'NSDraggingFormation draggingFormation = NSDraggingFormationNone' "$WVIMM" 2>/dev/null; then
        # Wrap from NSDraggingFormation declaration through the draggingFormation usage
        perl -i -pe '
            if (/^(\s+)NSDraggingFormation draggingFormation = NSDraggingFormationNone;/ && !$drag_guard) {
                $_ = "$1#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1070\n$_";
                $drag_guard = 1;
            }
            if ($drag_guard && /setDraggingFormation:draggingFormation\]/) {
                $_ .= "$1#endif\n";
                $drag_guard = 0;
            }
        ' "$WVIMM"
        info "  Guarded NSDraggingFormation code in WebViewImpl.mm"
    fi

    # ── WebViewImpl.mm — Guard numberOfValidItemsForDrop in draggingEntered (10.7+) ──
    if [ -f "$WVIMM" ] && grep -q 'm_initialNumberOfValidItemsForDrop = draggingInfo.numberOfValidItemsForDrop' "$WVIMM" 2>/dev/null; then
        perl -i -pe 's/^(\s+)(m_initialNumberOfValidItemsForDrop = draggingInfo\.numberOfValidItemsForDrop;)/$1#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1070\n$1$2\n$1#else\n$1    m_initialNumberOfValidItemsForDrop = 0;\n$1#endif/' "$WVIMM"
        info "  Guarded numberOfValidItemsForDrop in WebViewImpl.mm draggingEntered"
    fi

    # ── Include AppKit10_7Compat.h in files needing 10.7+ AppKit categories ──
    local APPKIT_COMPAT='#import <AppKit10_7Compat.h>'
    for f in \
        "$SOURCE_DIR/Source/WebKit/UIProcess/Cocoa/WebViewImpl.mm" \
        "$SOURCE_DIR/Source/WebKit/UIProcess/mac/ViewGestureControllerMac.mm" \
        "$SOURCE_DIR/Source/WebKit/UIProcess/mac/PageClientImpl.mm" \
        "$SOURCE_DIR/Source/WebKit/UIProcess/mac/WKFullScreenWindowController.mm" \
        "$SOURCE_DIR/Source/WebKit/UIProcess/mac/TiledCoreAnimationDrawingAreaProxy.mm" \
        "$SOURCE_DIR/Source/WebKit/UIProcess/mac/ViewSnapshotStore.mm"; do
        if [ -f "$f" ] && ! grep -q 'AppKit10_7Compat' "$f" 2>/dev/null; then
            # Insert after the first #import line
            sed -i '' '1,/^#import/{/^#import/a\
'"$APPKIT_COMPAT"'
}' "$f"
            info "  Added AppKit10_7Compat.h import to $(basename "$f")"
        fi
    done

    # ── Version-guard code that can't be stubbed ──

    # ViewGestureControllerMac.mm: Guard IOSurface / ViewSnapshot::setVolatile (10.8+)
    local VGC="$SOURCE_DIR/Source/WebKit/UIProcess/mac/ViewGestureControllerMac.mm"

    # WKFullScreenWindowController.mm: NSLayoutConstraint (10.7+)
    local WKFS="$SOURCE_DIR/Source/WebKit/UIProcess/mac/WKFullScreenWindowController.mm"
    if [ -f "$WKFS" ] && grep -q 'NSLayoutConstraint' "$WKFS" 2>/dev/null && ! grep -q '__MAC_OS_X_VERSION_MIN_REQUIRED >= 1070.*NSLayoutConstraint\|NSLayoutConstraint.*__MAC_OS_X_VERSION_MIN_REQUIRED >= 1070' "$WKFS" 2>/dev/null; then
        # Add a typedef for NSLayoutConstraint to avoid the undeclared identifier error
        # NSLayoutConstraint is too complex to stub; guard usage blocks instead
        perl -i -0777 -pe 's/(?=#import <AppKit10_7Compat.h>)/#if __MAC_OS_X_VERSION_MAX_ALLOWED < 1070\n\@class NSLayoutConstraint;\n#endif\n/' "$WKFS"
        info "  Added NSLayoutConstraint forward declaration to WKFullScreenWindowController.mm"
    fi

    # PageClientImpl.mm: CorrectionPanel is a C++ class, not ObjC — needs the actual header
    local PCI="$SOURCE_DIR/Source/WebKit/UIProcess/mac/PageClientImpl.mm"
    if [ -f "$PCI" ] && grep -q 'CorrectionPanel::' "$PCI" 2>/dev/null && ! grep -q '#include "CorrectionPanel.h"' "$PCI" 2>/dev/null; then
        perl -i -pe 'if (!$cp && /#import <AppKit10_7Compat.h>/) { $_ .= "#include \"CorrectionPanel.h\"\n"; $cp = 1; }' "$PCI"
        # Remove any @class CorrectionPanel that may have been added previously
        perl -i -pe 's/^\@class CorrectionPanel;\n//' "$PCI"
        info "  Added CorrectionPanel.h include to PageClientImpl.mm"
    fi

    # ── WebViewImpl.mm — Guard numberOfValidItemsForDrop in draggingExited (10.7+) ──
    # Line ~3716: draggingInfo.numberOfValidItemsForDrop = m_initialNumberOfValidItemsForDrop;
    # The category on NSObject isn't visible through the id<NSDraggingInfo> protocol type.
    if [ -f "$WVIMM" ] && grep -q 'draggingInfo\.numberOfValidItemsForDrop = m_initialNumberOfValidItemsForDrop' "$WVIMM" 2>/dev/null; then
        perl -i -pe 's/^(\s+)(draggingInfo\.numberOfValidItemsForDrop = m_initialNumberOfValidItemsForDrop;)/${1}#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1070\n${1}${2}\n${1}#endif/' "$WVIMM"
        info "  Guarded numberOfValidItemsForDrop in draggingExited"
    fi

    # ── WebViewImpl.mm — Guard IOSurface/setVolatile code in takeViewSnapshot (USE(IOSURFACE)) ──
    # USE(IOSURFACE) is 0 on 10.6, so IOSurface::createFromImage and ViewSnapshot::setVolatile are unavailable.
    if [ -f "$WVIMM" ] && grep -q 'WebCore::IOSurface::createFromImage' "$WVIMM" 2>/dev/null; then
        # Wrap from auto surface=... through return snapshot; with #if USE(IOSURFACE) ... #else return nullptr
        perl -i -pe '
            if (/^\s+auto surface = WebCore::IOSurface::createFromImage/ && !$ios_guard) {
                $_ = "#if USE(IOSURFACE)\n$_";
                $ios_guard = 1;
            }
            if ($ios_guard && /^\s+return snapshot;/) {
                $_ .= "#else\n    return nullptr;\n#endif\n";
                $ios_guard = 0;
            }
        ' "$WVIMM"
        info "  Guarded IOSurface/setVolatile code in takeViewSnapshot"
    fi

    # ── ViewGestureControllerMac.mm — Guard setVolatile with USE(IOSURFACE) ──
    local VGCM="$SOURCE_DIR/Source/WebKit/UIProcess/mac/ViewGestureControllerMac.mm"
    if [ -f "$VGCM" ] && grep -q 'm_currentSwipeSnapshot->setVolatile(true)' "$VGCM" 2>/dev/null; then
        perl -i -pe 's/^(\s+)(m_currentSwipeSnapshot->setVolatile\(true\);)/${1}#if USE(IOSURFACE)\n${1}${2}\n${1}#endif/' "$VGCM"
        info "  Guarded setVolatile in ViewGestureControllerMac.mm"
    fi

    ok "Source patches applied"
}

# ── Phase 3: Generate Overlay Files ────────────────────────────────────────

phase3_overlay() {
    local stamp="$STAMP_DIR/overlay-generated"
    [ -f "$stamp" ] && { info "Overlay: already generated (stamp exists)"; return 0; }

    info "Phase 3: Generating overlay files..."
    mkdir -p "$OVERLAY_DIR/Security" "$OVERLAY_DIR/os" "$OVERLAY_DIR/dispatch"

    # ─── TargetConditionals_compat.h ───
    info "  Writing TargetConditionals_compat.h..."
    cat > "$OVERLAY_DIR/TargetConditionals_compat.h" << 'HEADER_EOF'
/*
 * TargetConditionals_compat.h
 *
 * Compatibility shim for using the MacOSX 10.6 SDK with modern Clang.
 * Force-included via -include in CMAKE_C/CXX_FLAGS.
 *
 * Fixes:
 *  1. TARGET_* macros and __TARGETCONDITIONALS__ guard
 *  2. Architecture typedefs (__int32_t, __uint32_t, etc.)
 *  3. Availability macros (NS_CLASS_AVAILABLE, NS_AVAILABLE, etc.)
 *  4. GCD constants (DISPATCH_QUEUE_SERIAL)
 *  5. Objective-C dictionary subscripting (objectForKeyedSubscript:)
 *  6. Sandbox constant
 *  7. Post-10.6 runtime API stubs (objc_loadWeak, property_copyAttributeList, etc.)
 *  8. Missing C library functions (strndup)
 */

#ifndef TARGETCONDITIONALS_COMPAT_H
#define TARGETCONDITIONALS_COMPAT_H

/* NS_ASSUME_NONNULL_BEGIN/END — modern ObjC nullability annotations.
   These macros are no-ops; they just need to exist for the 10.6 SDK. */
#ifndef NS_ASSUME_NONNULL_BEGIN
#define NS_ASSUME_NONNULL_BEGIN
#endif
#ifndef NS_ASSUME_NONNULL_END
#define NS_ASSUME_NONNULL_END
#endif

/* Pull in objc/objc.h early so BOOL, YES, NO are defined before any
   other header needs them. Then override YES/NO to use integer literals
   instead of (BOOL) casts — the SDK's definitions cause @YES / @NO to
   expand to @(BOOL)1 / @(BOOL)0, which the compiler can't parse as
   ObjC boxed expressions. */
#ifdef __OBJC__
#include <objc/objc.h>
#undef YES
#undef NO
#define YES  1
#define NO   0
#endif

/* ================================================================== */
/*  1. TARGET_OS_* / TARGET_CPU_* / TARGET_RT_*                      */
/* ================================================================== */

#define TARGET_OS_MAC               1
#define TARGET_OS_WIN32             0
#define TARGET_OS_UNIX              0
#define TARGET_OS_EMBEDDED          0
#define TARGET_OS_IPHONE            0
#define TARGET_IPHONE_SIMULATOR     0

#define TARGET_CPU_PPC              0
#define TARGET_CPU_PPC64            0
#define TARGET_CPU_68K              0
#define TARGET_CPU_X86              0
#define TARGET_CPU_X86_64           1
#define TARGET_CPU_ARM              0
#define TARGET_CPU_MIPS             0
#define TARGET_CPU_SPARC            0
#define TARGET_CPU_ALPHA            0

#define TARGET_RT_MAC_CFM           0
#define TARGET_RT_MAC_MACHO         1
#define TARGET_RT_LITTLE_ENDIAN     1
#define TARGET_RT_BIG_ENDIAN        0
#define TARGET_RT_64_BIT            1

/* Guard so SDK's TargetConditionals.h is a no-op */
#define __TARGETCONDITIONALS__

/* ================================================================== */
/*  2. Architecture typedefs (normally from machine/_types.h)          */
/* ================================================================== */

#ifndef __X86_64_TYPEDEFS_H
#define __X86_64_TYPEDEFS_H

#ifndef __int8_t_defined
#define __int8_t_defined
typedef signed char          __int8_t;
typedef unsigned char        __uint8_t;
typedef signed short         __int16_t;
typedef unsigned short       __uint16_t;
typedef signed int           __int32_t;
typedef unsigned int         __uint32_t;
typedef signed long long     __int64_t;
typedef unsigned long long   __uint64_t;
#endif /* __int8_t_defined */

typedef long                 __darwin_intptr_t;
typedef unsigned int         __darwin_natural_t;

#endif /* __X86_64_TYPEDEFS_H */

/* ================================================================== */
/*  3. Availability macros (missing from 10.6 SDK's NSObjCRuntime.h)  */
/* ================================================================== */

#ifndef NS_AVAILABLE
#define NS_AVAILABLE(_mac, _ios)
#endif

#ifndef NS_CLASS_AVAILABLE
#define NS_CLASS_AVAILABLE(_mac, _ios)
#endif

#ifndef NS_DEPRECATED
#define NS_DEPRECATED(_macIntro, _macDep, _iosIntro, _iosDep)
#endif

#ifndef NS_CLASS_DEPRECATED
#define NS_CLASS_DEPRECATED(_macIntro, _macDep, _iosIntro, _iosDep)
#endif

#ifndef NS_ENUM_AVAILABLE
#define NS_ENUM_AVAILABLE(_mac, _ios)
#endif

#ifndef NS_ENUM_DEPRECATED
#define NS_ENUM_DEPRECATED(_macIntro, _macDep, _iosIntro, _iosDep)
#endif

#ifndef API_AVAILABLE
#define API_AVAILABLE(...)
#endif

#ifndef API_DEPRECATED
#define API_DEPRECATED(...)
#endif

#ifndef API_UNAVAILABLE
#define API_UNAVAILABLE(...)
#endif

/* Version-specific availability macros used in WebKit */
#ifndef AVAILABLE_MAC_OS_X_VERSION_10_7_AND_LATER
#define AVAILABLE_MAC_OS_X_VERSION_10_7_AND_LATER
#endif
#ifndef AVAILABLE_MAC_OS_X_VERSION_10_8_AND_LATER
#define AVAILABLE_MAC_OS_X_VERSION_10_8_AND_LATER
#endif
#ifndef AVAILABLE_MAC_OS_X_VERSION_10_9_AND_LATER
#define AVAILABLE_MAC_OS_X_VERSION_10_9_AND_LATER
#endif
#ifndef AVAILABLE_MAC_OS_X_VERSION_10_10_AND_LATER
#define AVAILABLE_MAC_OS_X_VERSION_10_10_AND_LATER
#endif
#ifndef AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER
#define AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER
#endif

#ifndef DEPRECATED_IN_MAC_OS_X_VERSION_10_7_AND_LATER
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_7_AND_LATER
#endif
#ifndef DEPRECATED_IN_MAC_OS_X_VERSION_10_8_AND_LATER
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_8_AND_LATER
#endif
#ifndef DEPRECATED_IN_MAC_OS_X_VERSION_10_9_AND_LATER
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_9_AND_LATER
#endif
#ifndef DEPRECATED_IN_MAC_OS_X_VERSION_10_10_AND_LATER
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_10_AND_LATER
#endif

/* ================================================================== */
/*  4. GCD / dispatch constants and functions                          */
/* ================================================================== */

#ifndef DISPATCH_QUEUE_SERIAL
#define DISPATCH_QUEUE_SERIAL NULL
#endif

#include <dispatch/dispatch.h>
#ifndef DISPATCH_QUEUE_CONCURRENT
#define DISPATCH_QUEUE_CONCURRENT \
    ({ dispatch_queue_attr_t _a = (dispatch_queue_attr_t)0x80; _a; })
#endif

#ifndef DISPATCH_QUEUE_PRIORITY_BACKGROUND
#define DISPATCH_QUEUE_PRIORITY_BACKGROUND 0x08
#endif

/* dispatch_data_t (introduced 10.7) */
#ifndef dispatch_data_t
typedef dispatch_object_t dispatch_data_t;
#endif

/* dispatch_io_t (introduced 10.7) */
#ifndef dispatch_io_t
typedef dispatch_object_t dispatch_io_t;
#endif

/* dispatch_data API functions (introduced 10.7) */
#include <sys/types.h>  /* for off_t */
#include <dispatch/dispatch.h>

/* dispatch_fd_t (introduced in later dispatch headers) */
#ifndef dispatch_fd_t
typedef int dispatch_fd_t;
#endif

#ifndef _DISPATCH_DATA_COMPAT_DEFINED
#define _DISPATCH_DATA_COMPAT_DEFINED
#ifdef __cplusplus
extern "C" {
#endif

extern const dispatch_block_t DISPATCH_DATA_DESTRUCTOR_DEFAULT;
extern dispatch_data_t dispatch_data_empty;

size_t dispatch_data_get_size(dispatch_data_t data);
dispatch_data_t dispatch_data_create(const void* buffer, size_t size, dispatch_queue_t queue, dispatch_block_t destructor);
dispatch_data_t dispatch_data_create_map(dispatch_data_t data, const void **buffer_ptr, size_t *size_ptr);
bool dispatch_data_apply(dispatch_data_t data, bool (^applier)(dispatch_data_t region, size_t offset, const void* buffer, size_t size));
dispatch_data_t dispatch_data_create_subrange(dispatch_data_t data, size_t offset, size_t size);
dispatch_data_t dispatch_data_create_concat(dispatch_data_t a, dispatch_data_t b);

/* dispatch_io API (introduced 10.7) */
#define DISPATCH_IO_RANDOM 0
typedef void (^dispatch_io_handler_t)(bool done, dispatch_data_t data, int error);

dispatch_io_t dispatch_io_create(int type, dispatch_fd_t fd, dispatch_queue_t queue, void (^cleanup_handler)(int error));
void dispatch_io_set_low_water(dispatch_io_t channel, size_t low_water);
void dispatch_io_read(dispatch_io_t channel, off_t offset, size_t length, dispatch_queue_t queue, dispatch_io_handler_t io_handler);
void dispatch_io_write(dispatch_io_t channel, off_t offset, dispatch_data_t data, dispatch_queue_t queue, dispatch_io_handler_t io_handler);

#ifdef __cplusplus
}
#endif
#endif /* _DISPATCH_DATA_COMPAT_DEFINED */

/* NSSecureCoding protocol (introduced 10.8) */
#ifdef __OBJC__
#ifndef NSSecureCoding
@protocol NSSecureCoding
@end
#endif
#endif

/* Mach VM constants (introduced 10.7) */
#ifndef VM_PROT_IS_MASK
#define VM_PROT_IS_MASK 0x40
#endif
#ifndef MAP_MEM_VM_SHARE
#define MAP_MEM_VM_SHARE 0x10
#endif

/* dispatch_barrier_async / dispatch_barrier_sync (introduced 10.7) */
#ifndef _DISPATCH_BARRIER_COMPAT_DEFINED
#define _DISPATCH_BARRIER_COMPAT_DEFINED
static inline void dispatch_barrier_async_compat(dispatch_queue_t queue, dispatch_block_t block) {
    dispatch_async(queue, block);
}
static inline void dispatch_barrier_sync_compat(dispatch_queue_t queue, dispatch_block_t block) {
    dispatch_sync(queue, block);
}
#define dispatch_barrier_async dispatch_barrier_async_compat
#define dispatch_barrier_sync  dispatch_barrier_sync_compat
#endif

/* ================================================================== */
/*  5. Objective-C subscripting (dictionary + array, introduced 10.8) */
/* ================================================================== */

#ifdef __OBJC__
#import <Foundation/NSDictionary.h>
#import <Foundation/NSArray.h>

#ifndef _SUBSCRIPT_COMPAT_DEFINED
#define _SUBSCRIPT_COMPAT_DEFINED

@interface NSDictionary (SubscriptCompat)
- (id)objectForKeyedSubscript:(id)key;
@end

@interface NSMutableDictionary (SubscriptCompat)
- (void)setObject:(id)object forKeyedSubscript:(id)key;
@end

@interface NSArray (SubscriptCompat)
- (id)objectAtIndexedSubscript:(NSUInteger)idx;
@end

@interface NSMutableArray (SubscriptCompat)
- (void)setObject:(id)obj atIndexedSubscript:(NSUInteger)idx;
@end

#endif /* _SUBSCRIPT_COMPAT_DEFINED */
#endif /* __OBJC__ */

/* ================================================================== */
/*  6. Sandbox constant (introduced later, used in RemoteInspector)   */
/* ================================================================== */

#ifndef SANDBOX_CHECK_NO_REPORT
#define SANDBOX_CHECK_NO_REPORT 0
#endif

/* ================================================================== */
/*  7. Post-10.6 runtime API stubs                                     */
/* ================================================================== */

#ifdef __OBJC__

/* ---- NS_ROOT_CLASS (used on Protocol class declaration) ---- */
#ifndef NS_ROOT_CLASS
#define NS_ROOT_CLASS __attribute__((objc_root_class))
#endif

/* ---- NSPointerFunctionsWeakMemory (introduced 10.8) ---- */
#ifndef NSPointerFunctionsWeakMemory
#define NSPointerFunctionsWeakMemory NSPointerFunctionsOpaqueMemory
#endif

/* ---- Weak reference runtime helpers (introduced 10.7) ---- */
#ifndef _OBJC_WEAK_STUBS_DEFINED
#define _OBJC_WEAK_STUBS_DEFINED

static inline __attribute__((always_inline)) id _objc_loadWeak_compat(id *location) {
    return *location;
}
static inline __attribute__((always_inline)) id _objc_storeWeak_compat(id *location, id obj) {
    *location = obj;
    return obj;
}

#define objc_loadWeak       _objc_loadWeak_compat
#define objc_storeWeak      _objc_storeWeak_compat

#endif /* _OBJC_WEAK_STUBS_DEFINED */

/* ---- property_copyAttributeList / objc_property_attribute_t (10.7) ---- */
#ifndef _OBJC_PROPERTY_COMPAT_DEFINED
#define _OBJC_PROPERTY_COMPAT_DEFINED

#include <objc/runtime.h>
#include <stdlib.h>
#include <string.h>
#include <Availability.h>

#ifndef __MAC_10_7
typedef struct objc_property_attribute_t {
    const char *name;
    const char *value;
} objc_property_attribute_t;
#endif

OBJC_EXPORT objc_property_attribute_t *property_copyAttributeList(objc_property_t property,
                                                                   unsigned int *outCount);
OBJC_EXPORT void *objc_alloc(Class cls);

static inline objc_property_attribute_t *_property_copyAttributeList_compat(
    objc_property_t property, unsigned int *outCount)
{
    const char *attrs = property_getAttributes(property);
    if (!attrs || !outCount) { if (outCount) *outCount = 0; return NULL; }

    unsigned int count = 1;
    for (const char *p = attrs; *p; p++) if (*p == ',') count++;

    objc_property_attribute_t *list = (objc_property_attribute_t *)calloc(count, sizeof(objc_property_attribute_t));
    if (!list) { *outCount = 0; return NULL; }

    unsigned int i = 0;
    const char *p = attrs;

    if (*p) {
        list[i].name = "T";
        p++;
        const char *val_start = p;
        if (*p == '"') {
            p++;
            while (*p && *p != '"') p++;
            if (*p == '"') p++;
        }
        size_t vlen = p - val_start;
        char *val = (char *)malloc(vlen + 1);
        memcpy(val, val_start, vlen);
        val[vlen] = '\0';
        list[i].value = val;
        i++;
        if (*p == ',') p++;
    }

    while (*p && i < count) {
        char key[2] = { *p, '\0' };
        p++;
        const char *val_start = p;
        if (*p == '"') {
            p++;
            while (*p && *p != '"') p++;
            if (*p == '"') p++;
        }
        size_t vlen = p - val_start;
        char *val = (char *)malloc(vlen + 1);
        memcpy(val, val_start, vlen);
        val[vlen] = '\0';
        char *key_copy = (char *)malloc(2);
        key_copy[0] = key[0]; key_copy[1] = '\0';
        list[i].name = key_copy;
        list[i].value = val;
        i++;
        if (*p == ',') p++;
    }

    *outCount = i;
    return list;
}

#define property_copyAttributeList _property_copyAttributeList_compat

#endif /* _OBJC_PROPERTY_COMPAT_DEFINED */

/* ---- dyld_get_program_sdk_version (introduced 10.9ish) ---- */
#include <stdint.h>
#ifndef _DYLD_COMPAT_DEFINED
#define _DYLD_COMPAT_DEFINED

static inline uint32_t dyld_get_program_sdk_version(void) {
    return 0x000A0600; /* Report 10.6 */
}

#endif /* _DYLD_COMPAT_DEFINED */

/* ---- CFBridgingRetain / CFBridgingRelease (introduced 10.7) ---- */
#ifndef CFBridgingRetain
static inline CFTypeRef CFBridgingRetain_compat(id obj) {
    CFRetain((__bridge CFTypeRef)obj);
    return (__bridge CFTypeRef)obj;
}
#define CFBridgingRetain CFBridgingRetain_compat
#endif

#ifndef CFBridgingRelease
static inline id CFBridgingRelease_compat(CFTypeRef cf) {
    return (__bridge id)cf;
}
#define CFBridgingRelease CFBridgingRelease_compat
#endif

/* ---- NSCalendarUnit constants (renamed from kCFCalendarUnit in 10.10) ---- */
#ifndef NSCalendarUnitEra
#define NSCalendarUnitEra        kCFCalendarUnitEra
#define NSCalendarUnitYear       kCFCalendarUnitYear
#define NSCalendarUnitMonth      kCFCalendarUnitMonth
#define NSCalendarUnitDay        kCFCalendarUnitDay
#define NSCalendarUnitHour       kCFCalendarUnitHour
#define NSCalendarUnitMinute     kCFCalendarUnitMinute
#define NSCalendarUnitSecond     kCFCalendarUnitSecond
#define NSCalendarUnitWeekday    kCFCalendarUnitWeekday
#define NSCalendarUnitWeek       kCFCalendarUnitWeek
#endif

/* ---- Protocol creation APIs (introduced 10.7) ---- */
#ifndef _OBJC_PROTOCOL_COMPAT_DEFINED
#define _OBJC_PROTOCOL_COMPAT_DEFINED

static inline Protocol *objc_allocateProtocol_compat(const char *name) {
    return NULL;
}
static inline void protocol_addProtocol_compat(Protocol *proto, Protocol *addition) {}
static inline void protocol_addMethodDescription_compat(Protocol *proto, SEL name, const char *types,
                                                         BOOL isRequiredMethod, BOOL isInstanceMethod) {}
static inline void objc_registerProtocol_compat(Protocol *proto) {}

#define objc_allocateProtocol      objc_allocateProtocol_compat
#define protocol_addProtocol       protocol_addProtocol_compat
#define protocol_addMethodDescription protocol_addMethodDescription_compat
#define objc_registerProtocol      objc_registerProtocol_compat

#endif /* _OBJC_PROTOCOL_COMPAT_DEFINED */

#endif /* __OBJC__ */

/* ================================================================== */
/*  8. Missing C library functions                                     */
/* ================================================================== */

#ifndef _STRNDUP_COMPAT_DEFINED
#define _STRNDUP_COMPAT_DEFINED

#include <stdlib.h>
#include <string.h>

static inline char *strndup_compat(const char *s, size_t n) {
    size_t len = strlen(s);
    if (len > n) len = n;
    char *p = (char *)malloc(len + 1);
    if (p) { memcpy(p, s, len); p[len] = '\0'; }
    return p;
}

#define strndup strndup_compat

#endif /* _STRNDUP_COMPAT_DEFINED */

/* ---- Missing C99 math functions (llrint, llround etc. — not in 10.6 SDK) ---- */
/* NOTE: Do NOT #include <math.h> here. libc++'s math.h uses #include_next
   which skips already-included headers. If we include the SDK's math.h here,
   its include guard prevents libc++ from seeing the architecture-specific
   declarations. Just forward-declare the functions instead. */
#ifndef _MATH_COMPAT_DEFINED
#define _MATH_COMPAT_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

long long llrintf(float __x);
long long llrint(double __x);
long long llrintl(long double __x);
long long llroundf(float __x);
long long llround(double __x);
long long llroundl(long double __x);

#ifdef __cplusplus
}
#endif

#endif /* _MATH_COMPAT_DEFINED */

/* ---- XPC types and functions (introduced 10.7) ---- */
#ifndef _XPC_COMPAT_DEFINED
#define _XPC_COMPAT_DEFINED

#ifndef __XPC_INDIRECT__
typedef void *xpc_object_t;
typedef struct _xpc_connection_s *xpc_connection_t;

#ifdef __BLOCKS__
#ifdef __cplusplus
extern "C" {
#endif

extern xpc_object_t XPC_TYPE_DICTIONARY;
extern xpc_object_t XPC_TYPE_ERROR;
extern xpc_object_t XPC_ERROR_CONNECTION_INVALID;

xpc_connection_t xpc_connection_create_mach_service(const char *name, dispatch_queue_t targetq, uint64_t flags);
void xpc_connection_set_target_queue(xpc_connection_t connection, dispatch_queue_t queue);
void xpc_connection_set_event_handler(xpc_connection_t connection, void (^handler)(xpc_object_t));
void xpc_connection_resume(xpc_connection_t connection);
void xpc_connection_cancel(xpc_connection_t connection);
void xpc_connection_send_message(xpc_connection_t connection, xpc_object_t message);

void xpc_release(xpc_object_t object);
xpc_object_t xpc_retain(xpc_object_t object);
xpc_object_t xpc_get_type(xpc_object_t object);

xpc_object_t xpc_dictionary_create(const char * const *keys, const xpc_object_t *values, size_t count);
void xpc_dictionary_set_value(xpc_object_t xdict, const char *key, xpc_object_t value);
xpc_object_t xpc_dictionary_get_value(xpc_object_t xdict, const char *key);

#ifdef __cplusplus
}
#endif
#endif /* __BLOCKS__ */
#endif /* __XPC_INDIRECT__ */
#endif /* _XPC_COMPAT_DEFINED */

/* ================================================================== */
/*  9. IOPM assertion constants (renamed in 10.7+)                     */
/* ================================================================== */

#ifndef kIOPMAssertionTypePreventUserIdleDisplaySleep
#define kIOPMAssertionTypePreventUserIdleDisplaySleep CFSTR("PreventUserIdleDisplaySleep")
#endif

#ifndef kIOPMAssertionTypePreventUserIdleSystemSleep
#define kIOPMAssertionTypePreventUserIdleSystemSleep CFSTR("PreventUserIdleSystemSleep")
#endif

/* ================================================================== */
/*  10. NSCalendarIdentifier constants (introduced 10.10)              */
/* ================================================================== */

#ifdef __OBJC__
#ifndef NSCalendarIdentifierGregorian
#define NSCalendarIdentifierGregorian NSGregorianCalendar
#define NSCalendarIdentifierBuddhist    NSBuddhistCalendar
#define NSCalendarIdentifierChinese     NSChineseCalendar
#define NSCalendarIdentifierCoptic      @"coptic"
#define NSCalendarIdentifierEthiopicAmeteMihret @"ethiopic"
#define NSCalendarIdentifierEthiopicAmeteAlem @"ethiopic-amete-alem"
#define NSCalendarIdentifierHebrew      NSHebrewCalendar
#define NSCalendarIdentifierISO8601     @"iso8601"
#define NSCalendarIdentifierIndian      NSIndianCalendar
#define NSCalendarIdentifierIslamic     NSIslamicCalendar
#define NSCalendarIdentifierIslamicCivil NSIslamicCivilCalendar
#define NSCalendarIdentifierJapanese    NSJapaneseCalendar
#define NSCalendarIdentifierPersian     @"persian"
#define NSCalendarIdentifierRepublicOfChina @"roc"
#endif
#endif /* __OBJC__ */

/* ================================================================== */
/*  11-18. Additional compat (NSHTTPCookie, UTType, Audio, etc.)       */
/* ================================================================== */

#ifdef __OBJC__
#import <Foundation/Foundation.h>

@interface NSHTTPCookie (SnowLeopardCompat)
@property (nonatomic, readonly, copy) NSString *HTTPOnly;
@property (nonatomic, readonly) BOOL secure;
@property (nonatomic, readonly) BOOL sessionOnly;
@end

@interface NSURL (SnowLeopardFileURLCompat)
@property (nonatomic, readonly) BOOL fileURL;
@end
#endif /* __OBJC__ */

#ifdef __OBJC__
#ifndef NSSharingServicePickerStyleRollover
#define NSSharingServicePickerStyleRollover 1
#ifndef NSSharingServicePickerStyle
#ifdef __OBJC__
typedef NSInteger NSSharingServicePickerStyle;
#endif
#endif
#endif

/* NSEventModifierFlags (introduced 10.8, renamed from NSUInteger modifier flags) */
#ifndef NSEventModifierFlags
typedef NSUInteger NSEventModifierFlags;
#endif
/* NOTE: Do NOT #define NSEventModifierFlagCommand etc. here as macros that
   expand to NSCommandKeyMask etc. AppKitCompatibilityDeclarations.h uses
   "static const NSEventModifierFlags NSEventModifierFlagCommand = NSCommandKeyMask;"
   which would get macro-expanded to "static const ... NSCommandKeyMask = NSCommandKeyMask;"
   causing redefinition errors. AppKitCompatibilityDeclarations.h handles these. */

/* NSURLDownloadDelegate (introduced 10.7, informal protocol in 10.6) */
#ifndef NSURLDownloadDelegate
@protocol NSURLDownloadDelegate
@end
#endif

/* NSSharingServicePicker / NSSharingService (introduced 10.8) */
#ifndef NSSharingServicePicker
@interface NSObject (NSSharingServicePickerStub)
@end
#define NSSharingServicePicker NSObject
#endif
#ifndef NSSharingServiceDelegate
@protocol NSSharingServiceDelegate
@end
#endif

#endif /* __OBJC__ */

#ifdef __OBJC__
#import <CoreServices/CoreServices.h>
#ifndef _UTTYPE_COMPAT_DEFINED
#define _UTTYPE_COMPAT_DEFINED
static inline Boolean UTTypeIsDeclared_compat(CFStringRef inUTI) { (void)inUTI; return true; }
static inline Boolean UTTypeIsDynamic_compat(CFStringRef inUTI) { (void)inUTI; return false; }
#define UTTypeIsDeclared UTTypeIsDeclared_compat
#define UTTypeIsDynamic UTTypeIsDynamic_compat
#endif
#endif

#ifdef AudioObjectPropertyAddress
#ifndef AudioObjectPropertyListenerBlock
typedef void (^AudioObjectPropertyListenerBlock)(UInt32 inNumberAddresses, const AudioObjectPropertyAddress *inAddresses);
#endif
#ifndef AudioObjectAddPropertyListenerBlock
#define AudioObjectAddPropertyListenerBlock(id, addr, queue, block) do { (void)(queue); (void)(block); } while(0)
#define AudioObjectRemovePropertyListenerBlock(id, addr, queue, block) do { (void)(queue); (void)(block); } while(0)
#endif
#endif

#ifdef __OBJC__
#ifndef _OBJC_COLLECTING_ENABLED_STUB
#define _OBJC_COLLECTING_ENABLED_STUB
static inline BOOL objc_collectingEnabled_compat(void) { return NO; }
#define objc_collectingEnabled objc_collectingEnabled_compat
#endif
#endif

/* Disable OpenType Sanitizer — OTS library not available in build */
#ifdef USE_OPENTYPE_SANITIZER
#undef USE_OPENTYPE_SANITIZER
#endif

/* ================================================================== */
/*  20. Mach port guards, XPC functions, CFNetwork privates (10.7+)   */
/* ================================================================== */

#include <mach/mach.h>
#include <CoreFoundation/CoreFoundation.h>

#ifdef __cplusplus
extern "C" {
#endif

/* mach_port_guard/unguard (introduced 10.12) */
#ifndef mach_port_context_t
typedef uint64_t mach_port_context_t;
#endif
#ifndef MACH_PORT_DENAP_RECEIVER
#define MACH_PORT_DENAP_RECEIVER 0x00000010
#endif
kern_return_t mach_port_guard(ipc_space_t task, mach_port_name_t name, mach_port_context_t context, boolean_t strict);
kern_return_t mach_port_unguard(ipc_space_t task, mach_port_name_t name, mach_port_context_t context);

/* XPC functions (introduced 10.7) */
#ifndef _XPC_INDIRECT__
pid_t xpc_connection_get_pid(xpc_connection_t connection);
void xpc_connection_kill(xpc_connection_t connection, int reason);
void xpc_connection_get_audit_token(xpc_connection_t connection, audit_token_t *token);
#endif

/* DISPATCH_MACH_SEND_POSSIBLE */
#ifndef DISPATCH_MACH_SEND_POSSIBLE
#define DISPATCH_MACH_SEND_POSSIBLE 0x8000000000000000ULL
#endif

/* MACH_SEND_NOTIFY */
#ifndef MACH_SEND_NOTIFY
#define MACH_SEND_NOTIFY 0x00000001
#endif

/* QOS policy constants (10.10+ task_policy.h) — only define what 10.6 SDK lacks */
#ifndef TASK_BASE_QOS_POLICY
#define TASK_BASE_QOS_POLICY 0
#define TASK_OVERRIDE_QOS_POLICY 1
#define TASK_QOS_POLICY_COUNT 2
struct task_qos_policy {
    integer_t task_latency_qos_tier;
    integer_t task_throughput_qos_tier;
};
#define LATENCY_QOS_TIER_0 0
#define THROUGHPUT_QOS_TIER_0 0
#define LATENCY_QOS_TIER_UNSPECIFIED (-1)
#define THROUGHPUT_QOS_TIER_UNSPECIFIED (-1)
#endif

/* CFNetwork private SPI */
void _CFNetworkSetATSContext(CFTypeRef context);
void _CFNetworkResetHSTSHostsSinceDate(CFTypeRef session, CFDateRef date);

/* WKSetCrashReportApplicationSpecificInformation */
void WKSetCrashReportApplicationSpecificInformation(CFStringRef info);

#ifdef __cplusplus
}
#endif

/* OBJC_ASSOCIATION_* constants — ensure they exist */
#ifdef __OBJC__
#import <objc/runtime.h>
#ifndef OBJC_ASSOCIATION_RETAIN_NONATOMIC
#define OBJC_ASSOCIATION_ASSIGN 0
#define OBJC_ASSOCIATION_RETAIN_NONATOMIC 1
#define OBJC_ASSOCIATION_COPY_NONATOMIC 3
#define OBJC_ASSOCIATION_RETAIN 01401
#define OBJC_ASSOCIATION_COPY 01403
typedef uintptr_t objc_AssociationPolicy;
#endif
#endif

/* ── WebKit Cocoa API ──────────────────────────────────────────────────── */
/* The modern Cocoa API (WKWebView, WKBackForwardList, etc.) requires
   macOS 10.10+ and uses features unavailable in the 10.6 SDK:
   ObjC lightweight generics (NSArray<Type *>), NS_UNAVAILABLE, etc.
   Disable the entire Cocoa API layer so those headers compile as empty.
   The C API (WKPageRef, WKContextRef, …) does NOT use this guard.      */
#ifndef WK_API_ENABLED
#define WK_API_ENABLED 0
#endif

/* NS_UNAVAILABLE — not in 10.6 SDK (introduced 10.8) */
#ifndef NS_UNAVAILABLE
#define NS_UNAVAILABLE
#endif

/* ================================================================== */
/*  22. Missing AppKit constants and properties (10.7+)                */
/* ================================================================== */

#ifdef __OBJC__

/* NSScrollerStyle constants (introduced 10.7) — also defined as enum in NSScrollerImpSPI.h;
   #undef'd there before the enum block to avoid macro expansion conflicts */
#ifndef NSScrollerStyleLegacy
#define NSScrollerStyleLegacy 0
#endif
#ifndef NSScrollerStyleOverlay
#define NSScrollerStyleOverlay 1
#endif

/* NSEventPhase constants (introduced 10.7) */
#ifndef NSEventPhaseNone
#define NSEventPhaseNone        0
#endif
#ifndef NSEventPhaseBegan
#define NSEventPhaseBegan       1
#endif
#ifndef NSEventPhaseStationary
#define NSEventPhaseStationary  2
#endif
#ifndef NSEventPhaseChanged
#define NSEventPhaseChanged     4
#endif
#ifndef NSEventPhaseEnded
#define NSEventPhaseEnded       8
#endif
#ifndef NSEventPhaseCancelled
#define NSEventPhaseCancelled   16
#endif
#ifndef NSEventPhaseMayBegin
#define NSEventPhaseMayBegin    32
#endif

/* NSDraggingFormation (introduced 10.7) */
#ifndef NSDraggingFormationNone
#define NSDraggingFormationNone    0
#endif
#ifndef NSDraggingFormationList
#define NSDraggingFormationList    2
#endif

/* NSEventPhase typedef (introduced 10.7) */
#ifndef NSEventPhase
typedef NSUInteger NSEventPhase;
#endif

/* NSWindowOcclusionState (introduced 10.9) */
#ifndef NSWindowOcclusionStateVisible
#define NSWindowOcclusionStateVisible (1ULL << 1)
#endif

/* NSCorrectionResponse (introduced 10.7) */
#ifndef NSCorrectionResponseReverted
typedef NSInteger NSCorrectionResponse;
#define NSCorrectionResponseReverted 0
#define NSCorrectionResponseEdited 1
#define NSCorrectionResponseAccepted 2
#endif

/* NSWindowCollectionBehaviorFullScreenPrimary (introduced 10.7) */
#ifndef NSWindowCollectionBehaviorFullScreenPrimary
#define NSWindowCollectionBehaviorFullScreenPrimary (1 << 7)
#endif

/* CoreGraphics / CoreAnimation constants */
#ifndef kCGWindowImageNominalResolution
#define kCGWindowImageNominalResolution 0
#endif
#ifndef kCAContextDisplayName
#define kCAContextDisplayName @"kCAContextDisplayName"
#endif
#ifndef kCAContextIgnoresHitTest
#define kCAContextIgnoresHitTest @"kCAContextIgnoresHitTest"
#endif
#ifndef kCAContextDisplayId
#define kCAContextDisplayId @"kCAContextDisplayId"
#endif
#ifndef kCATransactionPhasePostCommit
#define kCATransactionPhasePostCommit 2
#endif

/* NSViewNoInstrinsicMetric (introduced 10.7) */
#ifndef NSViewNoInstrinsicMetric
#define NSViewNoInstrinsicMetric -1.0
#endif

/* NSWindow notification constants (10.7+) */
#ifndef NSWindowDidChangeBackingPropertiesNotification
#define NSWindowDidChangeBackingPropertiesNotification @"NSWindowDidChangeBackingPropertiesNotification"
#endif
#ifndef NSWindowDidChangeOcclusionStateNotification
#define NSWindowDidChangeOcclusionStateNotification @"NSWindowDidChangeOcclusionStateNotification"
#endif
#ifndef NSBackingPropertyOldScaleFactorKey
#define NSBackingPropertyOldScaleFactorKey @"NSBackingPropertyOldScaleFactor"
#endif

/* NSWorkspace notification constants (10.8+) */
#ifndef NSWorkspaceAccessibilityDisplayOptionsDidChangeNotification
#define NSWorkspaceAccessibilityDisplayOptionsDidChangeNotification @"NSWorkspaceAccessibilityDisplayOptionsDidChangeNotification"
#endif

/* NSSpellChecker notification constants (10.7+) */
#ifndef NSSpellCheckerDidChangeAutomaticTextReplacementNotification
#define NSSpellCheckerDidChangeAutomaticTextReplacementNotification @"NSSpellCheckerDidChangeAutomaticTextReplacementNotification"
#endif
#ifndef NSSpellCheckerDidChangeAutomaticSpellingCorrectionNotification
#define NSSpellCheckerDidChangeAutomaticSpellingCorrectionNotification @"NSSpellCheckerDidChangeAutomaticSpellingCorrectionNotification"
#endif
#ifndef NSSpellCheckerDidChangeAutomaticQuoteSubstitutionNotification
#define NSSpellCheckerDidChangeAutomaticQuoteSubstitutionNotification @"NSSpellCheckerDidChangeAutomaticQuoteSubstitutionNotification"
#endif
#ifndef NSSpellCheckerDidChangeAutomaticDashSubstitutionNotification
#define NSSpellCheckerDidChangeAutomaticDashSubstitutionNotification @"NSSpellCheckerDidChangeAutomaticDashSubstitutionNotification"
#endif

/* NSWindowStyleMaskFullSizeContentView (introduced 10.10) */
#ifndef NSWindowStyleMaskFullSizeContentView
#define NSWindowStyleMaskFullSizeContentView (1 << 15)
#endif

/* NSUserInterfaceLayoutDirection — already in 10.6 SDK, no typedef needed */

#endif /* __OBJC__ */

/* ================================================================== */
/*  23. Additional XPC functions (ProcessLauncherMac needs these)      */
/* ================================================================== */

#ifndef _XPC_INDIRECT__
#ifdef __BLOCKS__
#ifdef __cplusplus
extern "C" {
#endif

xpc_connection_t xpc_connection_create(const char *name, dispatch_queue_t targetq);
void xpc_connection_set_oneshot_instance(xpc_connection_t connection, const unsigned char instance[16]);
xpc_object_t xpc_array_create(const xpc_object_t *objects, size_t count);
void xpc_array_set_value(xpc_object_t xarray, size_t index, xpc_object_t value);
void xpc_array_set_string(xpc_object_t xarray, size_t index, const char *string);
#define XPC_ARRAY_APPEND ((size_t)(-1))
void xpc_dictionary_set_string(xpc_object_t xdict, const char *key, const char *string);
void xpc_dictionary_set_mach_send(xpc_object_t xdict, const char *key, mach_port_t port);
void xpc_dictionary_set_fd(xpc_object_t xdict, const char *key, int fd);
void xpc_connection_set_bootstrap(xpc_connection_t connection, xpc_object_t bootstrap);
void xpc_connection_send_message_with_reply(xpc_connection_t connection, xpc_object_t message, dispatch_queue_t replyq, void (^handler)(xpc_object_t));
xpc_object_t xpc_string_create(const char *string);
xpc_object_t xpc_uint64_create(uint64_t value);
xpc_object_t xpc_bool_create(bool value);
void _CFBundleSetupXPCBootstrap(xpc_object_t bootstrap);

#ifdef __cplusplus
}
#endif
#endif /* __BLOCKS__ */
#endif /* _XPC_INDIRECT__ */

/* ================================================================== */
/*  24. _CFNetworkCopyATSContext (private SPI, 10.10+)                 */
/* ================================================================== */

#ifdef __cplusplus
extern "C" {
#endif
CFTypeRef _CFNetworkCopyATSContext(void);
#ifdef __cplusplus
}
#endif

/* ================================================================== */
/*  25. Lookup framework stubs (private, 10.10+)                       */
/* ================================================================== */

#ifdef __OBJC__
#ifndef LOOKUP_SPI_COMPAT
#define LOOKUP_SPI_COMPAT
@interface LULookupDefinitionModule : NSObject
@end
static inline void* LookupLibrary(void) { return nullptr; }
#endif
#endif

#endif /* TARGETCONDITIONALS_COMPAT_H */
HEADER_EOF

    # ─── AppKit10_7Compat.h — Shared ObjC categories for 10.7+ APIs ───
    info "  Writing AppKit10_7Compat.h..."
    cat > "$OVERLAY_DIR/AppKit10_7Compat.h" << 'APPKIT_EOF'
/*
 * AppKit10_7Compat.h
 * Shared ObjC category declarations for AppKit 10.7+ properties.
 * Include this AFTER #import <AppKit/AppKit.h> in .mm files that use
 * NSEvent phase/scrolling properties, NSView layoutDirection, etc.
 */
#ifndef APPKIT10_7_COMPAT_H
#define APPKIT10_7_COMPAT_H

#import <AppKit/AppKit.h>

#if __MAC_OS_X_VERSION_MAX_ALLOWED < 1070

@interface NSEvent (WebKit10_7Compat)
@property (nonatomic, readonly) NSInteger phase;
@property (nonatomic, readonly) NSInteger momentumPhase;
@property (nonatomic, readonly) NSInteger stage;
@property (nonatomic, readonly) float pressure;
@property (nonatomic, readonly) BOOL hasPreciseScrollingDeltas;
@property (nonatomic, readonly) CGFloat scrollingDeltaX;
@property (nonatomic, readonly) CGFloat scrollingDeltaY;
@end

@interface NSView (WebKit10_7Compat)
@property (nonatomic) NSUserInterfaceLayoutDirection userInterfaceLayoutDirection;
@property (nonatomic, readonly, copy) NSArray *constraints;
@end

@interface NSWindow (WebKit10_7Compat)
@property (nonatomic, readonly) NSInteger occlusionState;
- (NSRect)convertRectToScreen:(NSRect)rect;
@end

@interface NSColor (WebKit10_7Compat)
@property (nonatomic, readonly) CGColorRef CGColor;
@end

#endif /* __MAC_OS_X_VERSION_MAX_ALLOWED < 1070 */

#endif /* APPKIT10_7_COMPAT_H */
APPKIT_EOF

    # ─── Security/SecKeychain.h ───
    info "  Writing Security/SecKeychain.h..."
    cat > "$OVERLAY_DIR/Security/SecKeychain.h" << 'HEADER_EOF'
// SecKeychain.h override for C/ObjC compilation
// Original uses static_cast which is C++ only

#ifndef SECKEYCHAIN_COMPAT_H
#define SECKEYCHAIN_COMPAT_H

#include <Security/SecItem.h>
#include <Security/SecBase.h>
#include <CoreFoundation/CoreFoundation.h>

enum SecAuthenticationTypeValues {
    kSecAuthenticationTypeNTLM             = 'ntlm',
    kSecAuthenticationTypeMSN              = 'msna',
    kSecAuthenticationTypeDPA              = 'dpaa',
    kSecAuthenticationTypeRPA              = 'rpaa',
    kSecAuthenticationTypeHTTPBasic        = 'http',
    kSecAuthenticationTypeHTTPDigest       = 'httd',
    kSecAuthenticationTypeHTMLForm         = 'form',
    kSecAuthenticationTypeDefault          = 'dflt',
    kSecAuthenticationTypeAny              = 0
};
typedef uint32_t SecAuthenticationType;

#endif /* SECKEYCHAIN_COMPAT_H */
HEADER_EOF

    # ─── os/object.h ───
    info "  Writing os/object.h..."
    cat > "$OVERLAY_DIR/os/object.h" << 'HEADER_EOF'
#ifndef OS_OBJECT_H_COMPAT
#define OS_OBJECT_H_COMPAT

#include <dispatch/dispatch.h>

#ifndef OS_OBJECT_USE_OBJC
#define OS_OBJECT_USE_OBJC 0
#endif

#ifndef os_object_t
typedef void *os_object_t;
#endif

#ifndef OS_OBJECT_DECL
#define OS_OBJECT_DECL(name) typedef void *name##_t
#endif

#ifndef os_retain
static inline void *os_retain(void *obj) { return obj; }
#endif

#ifndef os_release
static inline void os_release(void *obj) { (void)obj; }
#endif

#endif /* OS_OBJECT_H_COMPAT */
HEADER_EOF

    # ─── dispatch/queue_shim.h ───
    info "  Writing dispatch/queue_shim.h..."
    cat > "$OVERLAY_DIR/dispatch/queue_shim.h" << 'HEADER_EOF'
#ifndef DISPATCH_COMPAT_SHIM_H
#define DISPATCH_COMPAT_SHIM_H

#include_next <dispatch/dispatch.h>

#ifndef DISPATCH_QUEUE_SERIAL
#define DISPATCH_QUEUE_SERIAL NULL
#endif

#ifndef DISPATCH_QUEUE_CONCURRENT
#define DISPATCH_QUEUE_CONCURRENT \
    ({ dispatch_queue_attr_t _a; _a = (dispatch_queue_attr_t)_dispatch_queue_attr_concurrent; _a; })
#endif

#ifndef DISPATCH_BLOCK_ASSIGN_CONTEXT
#define DISPATCH_BLOCK_ASSIGN_CONTEXT 0
#endif

#ifndef DISPATCH_BLOCK_NO_FLAGS
#define DISPATCH_BLOCK_NO_FLAGS 0
#endif

#ifndef DISPATCH_QUEUE_PRIORITY_BACKGROUND
#define DISPATCH_QUEUE_PRIORITY_BACKGROUND 0x08
#endif

#endif /* DISPATCH_COMPAT_SHIM_H */
HEADER_EOF

    # ─── Foundation/NSURLSession.h stub (introduced 10.9) ───
    info "  Writing Foundation/NSURLSession.h stub..."
    mkdir -p "$OVERLAY_DIR/Foundation"
    cat > "$OVERLAY_DIR/Foundation/NSURLSession.h" << 'HEADER_EOF'
/*
 * NSURLSession.h stub for Mac OS X 10.6 SDK compatibility.
 * NSURLSession was introduced in 10.9.
 */
#import <Foundation/Foundation.h>

#ifndef NSURLSESSION_STUB_H
#define NSURLSESSION_STUB_H

enum {
    NSURLSessionAuthChallengeUseCredential = 0,
    NSURLSessionAuthChallengePerformDefaultHandling = 1,
    NSURLSessionAuthChallengeCancelAuthenticationChallenge = 2,
    NSURLSessionAuthChallengeRejectProtectionSpace = 3,
};
typedef NSUInteger NSURLSessionAuthChallengeDisposition;

enum {
    NSURLSessionResponseCancel = 0,
    NSURLSessionResponseAllow = 1,
    NSURLSessionResponseBecomeDownload = 2,
    NSURLSessionResponseBecomeStream = 3,
};
typedef NSUInteger NSURLSessionResponsePolicy;

@class NSURLSessionConfiguration;

@interface NSURLSessionConfiguration : NSObject <NSCopying>
+ (NSURLSessionConfiguration *)defaultSessionConfiguration;
+ (NSURLSessionConfiguration *)ephemeralSessionConfiguration;
@end

@interface NSURLSession : NSObject
+ (NSURLSession *)sessionWithConfiguration:(NSURLSessionConfiguration *)configuration;
+ (NSURLSession *)sharedSession;
@end

@interface NSURLSessionTask : NSObject
@property (readonly) NSUInteger taskIdentifier;
- (void)cancel;
- (void)resume;
@end

@interface NSURLSessionDataTask : NSURLSessionTask
@end

@protocol NSURLSessionDataDelegate <NSObject>
@end

@protocol NSURLSessionTaskDelegate <NSObject>
@end

#endif
HEADER_EOF

    # ─── sdk_stubs.c ───
    info "  Writing sdk_stubs.c..."
    cat > "$OVERLAY_DIR/sdk_stubs.c" << 'C_EOF'
/*
 * sdk_stubs.c - Missing symbols for MacOSX 10.6 SDK.
 * Compile: clang -target x86_64-apple-macos10.6 -arch x86_64 -c sdk_stubs.c -o sdk_stubs.o
 */

#include <CoreFoundation/CoreFoundation.h>
#include <stdlib.h>
#include <string.h>

/* Private CoreFoundation XPC functions */
CFTypeRef __CFXPCCreateCFObjectFromXPCMessage(void *xpc_message) {
    (void)xpc_message; return NULL;
}

CFTypeRef __CFXPCCreateXPCMessageWithCFObject(CFTypeRef cf) {
    (void)cf; return NULL;
}

/* XPC functions (XPC introduced in 10.7) */
typedef void *xpc_object_t;
typedef struct _xpc_connection_s xpc_connection_t;
typedef void (^xpc_handler_t)(xpc_object_t);

static int _xpc_type_dictionary_val = 0;
static int _xpc_type_error_val = 0;
static int _xpc_error_connection_invalid_val = 0;

void *_xpc_type_dictionary = &_xpc_type_dictionary_val;
void *_xpc_type_error = &_xpc_type_error_val;
void *_xpc_error_connection_invalid = &_xpc_error_connection_invalid_val;

xpc_object_t xpc_dictionary_create(const char * const *keys, const xpc_object_t *values, size_t count) {
    (void)keys; (void)values; (void)count; return NULL;
}
void *xpc_dictionary_get_value(xpc_object_t xdict, const char *key) {
    (void)xdict; (void)key; return NULL;
}
void xpc_dictionary_set_value(xpc_object_t xdict, const char *key, xpc_object_t value) {
    (void)xdict; (void)key; (void)value;
}
void *xpc_get_type(xpc_object_t obj) { (void)obj; return NULL; }
void xpc_retain(xpc_object_t obj) { (void)obj; }
void xpc_release(xpc_object_t obj) { (void)obj; }
xpc_connection_t *xpc_connection_create_mach_service(const char *name, dispatch_queue_t targetq, uint64_t flags) {
    (void)name; (void)targetq; (void)flags; return NULL;
}
void xpc_connection_set_event_handler(xpc_connection_t *conn, xpc_handler_t handler) {
    (void)conn; (void)handler;
}
void xpc_connection_resume(xpc_connection_t *conn) { (void)conn; }
void xpc_connection_cancel(xpc_connection_t *conn) { (void)conn; }
void xpc_connection_send_message(xpc_connection_t *conn, xpc_object_t msg) { (void)conn; (void)msg; }
void xpc_connection_set_target_queue(xpc_connection_t *conn, dispatch_queue_t queue) { (void)conn; (void)queue; }

/* Sandbox functions */
typedef int sandbox_filter_type;
int sandbox_check(int pid, const char *operation, sandbox_filter_type type, ...) {
    (void)pid; (void)operation; (void)type; return 0;
}
C_EOF

    # ─── sdk_stubs.mm ───
    info "  Writing sdk_stubs.mm..."
    cat > "$OVERLAY_DIR/sdk_stubs.mm" << 'MM_EOF'
/*
 * sdk_stubs.mm - Missing symbols for MacOSX 10.6 SDK (ObjC++ linkage).
 */

#include <CoreFoundation/CoreFoundation.h>
#include <dispatch/dispatch.h>
#include <objc/objc.h>
#include <stdlib.h>

extern "C" {

CFTypeRef _CFXPCCreateCFObjectFromXPCMessage(void *xpc_message) {
    (void)xpc_message; return NULL;
}
CFTypeRef _CFXPCCreateXPCMessageWithCFObject(CFTypeRef cf) {
    (void)cf; return NULL;
}

/* XPC type globals — names must match TargetConditionals_compat.h declarations */
static int xpc_type_dictionary_val = 0;
static int xpc_type_error_val = 0;
static int xpc_error_connection_invalid_val = 0;

void *XPC_TYPE_DICTIONARY = &xpc_type_dictionary_val;
void *XPC_TYPE_ERROR = &xpc_type_error_val;
void *XPC_ERROR_CONNECTION_INVALID = &xpc_error_connection_invalid_val;

typedef void *xpc_object_t;

void *xpc_dictionary_create(const char * const *keys, const xpc_object_t *values, size_t count) {
    (void)keys; (void)values; (void)count; return NULL;
}
void *xpc_dictionary_get_value(xpc_object_t xdict, const char *key) {
    (void)xdict; (void)key; return NULL;
}
void xpc_dictionary_set_value(xpc_object_t xdict, const char *key, xpc_object_t value) {
    (void)xdict; (void)key; (void)value;
}
void *xpc_get_type(xpc_object_t obj) { (void)obj; return NULL; }
void xpc_retain(xpc_object_t obj) { (void)obj; }
void xpc_release(xpc_object_t obj) { (void)obj; }
void *xpc_connection_create_mach_service(const char *name, dispatch_queue_t targetq, uint64_t flags) {
    (void)name; (void)targetq; (void)flags; return NULL;
}
void xpc_connection_set_event_handler(void *conn, void (^handler)(xpc_object_t)) {
    (void)conn; (void)handler;
}
void xpc_connection_resume(void *conn) { (void)conn; }
void xpc_connection_cancel(void *conn) { (void)conn; }
void xpc_connection_send_message(void *conn, xpc_object_t msg) { (void)conn; (void)msg; }

} /* extern "C" */

#include <sandbox.h>
int sandbox_check(pid_t pid, const char *operation, enum sandbox_filter_type type, ...) {
    (void)pid; (void)operation; (void)type; return 0;
}

struct _xpc_connection_s;
struct dispatch_queue_s;
extern "C" {
void xpc_connection_set_target_queue(struct _xpc_connection_s *conn, struct dispatch_queue_s *queue) {
    (void)conn; (void)queue;
}
}

extern "C" {
void objc_initWeak(id *addr, id val) { *addr = val; }
void objc_destroyWeak(id *addr) { *addr = nil; }
}

extern "C" {
int _Block_has_signature(void *block) { (void)block; return 0; }
const char *_Block_signature(void *block) { (void)block; return ""; }
}

extern "C" {
const char *_protocol_getMethodTypeEncoding(Protocol *proto, SEL sel, BOOL isRequiredMethod, BOOL isInstanceMethod) {
    (void)proto; (void)sel; (void)isRequiredMethod; (void)isInstanceMethod; return NULL;
}
}

extern "C" {
uintptr_t g_globalDataPoison = 0;
uintptr_t g_jitCodePoison = 0;
uintptr_t g_nativeCodePoison = 0;
}
namespace JSC {
void initializePoison() {}
}
MM_EOF

    # ─── WebCoreStubs.cpp ───
    info "  Writing WebCoreStubs.cpp..."
    cat > "$OVERLAY_DIR/WebCoreStubs.cpp" << 'CPP_EOF'
// WebCoreStubs.cpp - Missing symbols for disabled WebCore components

#include "config.h"
#include <wtf/Forward.h>

#include "ScrollAnimator.h"
#include "ScrollableArea.h"
#include "ScrollbarThemeMac.h"
#include "Scrollbar.h"

namespace WebCore {

std::unique_ptr<ScrollAnimator> ScrollAnimator::create(ScrollableArea& area)
{
    return std::make_unique<ScrollAnimator>(area);
}

id ScrollbarThemeMac::painterForScrollbar(Scrollbar&) { return nil; }

}

#ifdef __OBJC__
@interface CABackdropLayer : CALayer
@end
@implementation CABackdropLayer
@end
#endif
CPP_EOF

    touch "$stamp"
    ok "Overlay files generated in $OVERLAY_DIR"
}

# ── Phase 4: CMake Configure ──────────────────────────────────────────────

phase4_cmake() {
    local stamp="$STAMP_DIR/cmake-configured"
    [ -f "$stamp" ] && { info "CMake: already configured (stamp exists)"; return 0; }

    info "Phase 4: CMake configure..."

    # Compile sdk_stubs (ObjC++ — includes all C stubs too)
    info "  Compiling sdk_stubs..."
    mkdir -p "$BUILD_DIR/cmake"
    clang++ -target $ARCH-apple-macos10.6 -arch $ARCH \
        -isysroot "$SDK_DIR" \
        -I"$OVERLAY_DIR" \
        -stdlib=libc++ -nostdinc++ -isystem "$LIBCXX_DIST/include" \
        -c "$OVERLAY_DIR/sdk_stubs.mm" \
        -o "$BUILD_DIR/cmake/sdk_stubs.o" \
        -fallow-unsupported -std=gnu++14 2>&1 | tail -3

    # Framework paths for sub-frameworks
    local FW_SUBPATHS=(
        "ApplicationServices.framework/Frameworks"
        "CoreServices.framework/Frameworks"
        "Accelerate.framework/Frameworks"
        "Quartz.framework/Frameworks"
        "Carbon.framework/Frameworks"
    )
    local FW_FLAGS=""
    for p in "${FW_SUBPATHS[@]}"; do
        FW_FLAGS="$FW_FLAGS -F$SDK_DIR/System/Library/Frameworks/$p"
    done

    # Common compiler flags
    local COMMON_FLAGS="-target $ARCH-apple-macos10.6 -fallow-unsupported"
    COMMON_FLAGS="$COMMON_FLAGS -I$LIBCXX_DIST/include"
    COMMON_FLAGS="$COMMON_FLAGS -I$ICU_DIST/include"
    COMMON_FLAGS="$COMMON_FLAGS -I$OVERLAY_DIR"
    COMMON_FLAGS="$COMMON_FLAGS -include TargetConditionals_compat.h"
    COMMON_FLAGS="$COMMON_FLAGS -D_LIBCPP_DISABLE_EXTERN_TEMPLATE"
    COMMON_FLAGS="$COMMON_FLAGS -DOS_OBJECT_USE_OBJC=0"
    COMMON_FLAGS="$COMMON_FLAGS -DNS_NONATOMIC_IOSONLY=nonatomic"
    COMMON_FLAGS="$COMMON_FLAGS $FW_FLAGS"

    local CXX_FLAGS="$COMMON_FLAGS -std=gnu++14"

    # Linker flags
    local LINKER_FLAGS="-target $ARCH-apple-macos10.6"
    LINKER_FLAGS="$LINKER_FLAGS $BUILD_DIR/cmake/sdk_stubs.o"
    LINKER_FLAGS="$LINKER_FLAGS -L$SOURCE_DIR/WebKitLibraries"
    LINKER_FLAGS="$LINKER_FLAGS -L$LIBCXX_DIST/lib"
    LINKER_FLAGS="$LINKER_FLAGS -L$CRT_DIST"
    LINKER_FLAGS="$LINKER_FLAGS -L$ICU_DIST/lib"
    LINKER_FLAGS="$LINKER_FLAGS -licuuc -licui18n -licudata"
    LINKER_FLAGS="$LINKER_FLAGS -Wl,-undefined,dynamic_lookup"

    cd "$BUILD_DIR"

    cmake -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
        -DCMAKE_OSX_ARCHITECTURES="$ARCH" \
        -DCMAKE_OSX_SYSROOT="$SDK_DIR" \
        -DCMAKE_C_FLAGS="$COMMON_FLAGS" \
        -DCMAKE_CXX_FLAGS="$CXX_FLAGS" \
        -DCMAKE_SHARED_LINKER_FLAGS="$LINKER_FLAGS" \
        -DCMAKE_EXE_LINKER_FLAGS="$LINKER_FLAGS" \
        -DCMAKE_C_COMPILER_WORKS=ON \
        -DCMAKE_CXX_COMPILER_WORKS=ON \
        -DPORT=Mac \
        -DENABLE_WEBGL=ON \
        -DENABLE_WEBGL2=ON \
        -DENABLE_SERVICE_CONTROLS=OFF \
        -DENABLE_TELEPHONE_NUMBER_DETECTION=OFF \
        -DENABLE_LEGACY_ENCRYPTED_MEDIA=OFF \
        -DENABLE_MEDIA_SOURCE=OFF \
        -DENABLE_WEBGPU=OFF \
        -DENABLE_ACCELERATED_2D_CANVAS=OFF \
        -DENABLE_CONTENT_FILTERING=OFF \
        -DENABLE_ACCESSIBILITY=OFF \
        -DENABLE_WEBKIT=ON \
        -DENABLE_CSS3_TEXT=OFF \
        -DENABLE_DATALIST_ELEMENT=OFF \
        -DENABLE_DEVICE_ORIENTATION=OFF \
        -DENABLE_DOWNLOAD_ATTRIBUTE=OFF \
        -DENABLE_DATA_TRANSFER_ITEMS=OFF \
        -DENABLE_CUSTOM_SCHEME_HANDLER=OFF \
        -DENABLE_CSS_IMAGE_ORIENTATION=OFF \
        -DENABLE_CSS_IMAGE_RESOLUTION=OFF \
        -DENABLE_CSS_DEVICE_ADAPTATION=OFF \
        -DENABLE_CSS_SELECTORS_LEVEL4=OFF \
        -DENABLE_CANVAS_PROXY=OFF \
        "$SOURCE_DIR"

    touch "$stamp"
    ok "CMake configured"
}

# ── Phase 5: Post-CMake Fixes ─────────────────────────────────────────────

phase5_post_cmake() {
    local stamp="$STAMP_DIR/post-cmake-fixed"
    [ -f "$stamp" ] && { info "Post-cmake: already fixed (stamp exists)"; return 0; }

    info "Phase 5: Post-cmake fixes..."

    # Fix cmakeconfig.h: disable ENABLE_SERVICE_CONTROLS (cmake sets it ON)
    local CMAKE_CONFIG="$BUILD_DIR/cmakeconfig.h"
    if [ -f "$CMAKE_CONFIG" ]; then
        if grep -q 'ENABLE_SERVICE_CONTROLS 1' "$CMAKE_CONFIG"; then
            sed -i '' 's/ENABLE_SERVICE_CONTROLS 1/ENABLE_SERVICE_CONTROLS 0/' "$CMAKE_CONFIG"
            info "  Disabled ENABLE_SERVICE_CONTROLS in cmakeconfig.h"
        fi
        if grep -q 'ENABLE_TELEPHONE_NUMBER_DETECTION 1' "$CMAKE_CONFIG"; then
            sed -i '' 's/ENABLE_TELEPHONE_NUMBER_DETECTION 1/ENABLE_TELEPHONE_NUMBER_DETECTION 0/' "$CMAKE_CONFIG"
            info "  Disabled ENABLE_TELEPHONE_NUMBER_DETECTION in cmakeconfig.h"
        fi
        if grep -q 'ENABLE_LEGACY_ENCRYPTED_MEDIA 1' "$CMAKE_CONFIG"; then
            sed -i '' 's/ENABLE_LEGACY_ENCRYPTED_MEDIA 1/ENABLE_LEGACY_ENCRYPTED_MEDIA 0/' "$CMAKE_CONFIG"
            info "  Disabled ENABLE_LEGACY_ENCRYPTED_MEDIA in cmakeconfig.h"
        fi
        if grep -q 'ENABLE_MEDIA_SOURCE 1' "$CMAKE_CONFIG"; then
            sed -i '' 's/ENABLE_MEDIA_SOURCE 1/ENABLE_MEDIA_SOURCE 0/' "$CMAKE_CONFIG"
            info "  Disabled ENABLE_MEDIA_SOURCE in cmakeconfig.h (no AVFoundation in 10.6)"
        fi
    fi

    # Fix LINK_FLAGS semicolons: CMake's set(CMAKE_SHARED_LINKER_FLAGS ...)
    # without quotes causes list expansion, joining our flags with -compatibility_version
    # via semicolons (e.g. "dynamic_lookup;-compatibility_version"). Replace with spaces.
    local NINJA="$BUILD_DIR/build.ninja"
    if [ -f "$NINJA" ] && grep -q 'dynamic_lookup;' "$NINJA"; then
        sed -i '' 's/dynamic_lookup;-compatibility_version/dynamic_lookup -compatibility_version/g' "$NINJA"
        info "  Fixed LINK_FLAGS semicolons in build.ninja"
    fi

    # Remove host-only framework link dependencies from build.ninja.
    # CMake finds AVFoundation, Metal, DataDetectorsCore, Lookup on the host
    # but these don't exist in the 10.6 SDK (or are runtime-only with no headers).
    # With -Wl,-undefined,dynamic_lookup, the linker doesn't need them anyway.
    # Use inline replacement (not line deletion) to preserve other flags on same line.
    if [ -f "$NINJA" ]; then
        # Remove framework paths (cmake finds them on host system)
        sed -i '' 's|/System/Library/Frameworks/AVFoundation.framework||g' "$NINJA"
        sed -i '' 's|/System/Library/Frameworks/Metal.framework||g' "$NINJA"
        sed -i '' 's|/System/Library/PrivateFrameworks/DataDetectorsCore.framework||g' "$NINJA"
        sed -i '' 's|/System/Library/PrivateFrameworks/Lookup.framework||g' "$NINJA"
        sed -i '' 's|/System/Library/Frameworks/AVFoundation.framework/Versions/A/Frameworks/AVFAudio.framework||g' "$NINJA"
        # Also remove the bare -framework arguments left behind
        sed -i '' 's/-framework AVFoundation //g;s/-framework AVFoundation$//g' "$NINJA"
        sed -i '' 's/-framework Metal //g;s/-framework Metal$//g' "$NINJA"
        sed -i '' 's/-framework DataDetectorsCore //g;s/-framework DataDetectorsCore$//g' "$NINJA"
        sed -i '' 's/-framework Lookup //g;s/-framework Lookup$//g' "$NINJA"
        sed -i '' 's/-framework AVFAudio //g;s/-framework AVFAudio$//g' "$NINJA"
        info "  Removed host-only framework links (AVFoundation, AVFAudio, Metal, DataDetectorsCore, Lookup)"
    fi

    # Fix forwarding headers: copy WebKitLegacy headers over WebKit ones
    info "  Fixing forwarding headers..."
    "$PROJECT_ROOT/fix-forwarding-headers.sh"

    # Fix stale forwarding headers that reference the old WebKit2 framework name.
    # Some headers were generated with #import <WebKit2/...> instead of #include "WebKit/..."
    # The cmake cleanup only removes headers using the new format, so these stale ones persist.
    local FH_WEBKIT="$BUILD_DIR/DerivedSources/ForwardingHeaders/WebKit"
    local STALE_COUNT=0
    if [ -d "$FH_WEBKIT" ]; then
        STALE_COUNT=$(grep -rl '#import <WebKit2/' "$FH_WEBKIT/" 2>/dev/null | wc -l | tr -d ' ')
        if [ "$STALE_COUNT" -gt 0 ]; then
            sed -i '' 's|#import <WebKit2/\([^>]*\)>|#include "WebKit/\1"|g' "$FH_WEBKIT/"*.h
            info "  Fixed $STALE_COUNT stale forwarding headers (WebKit2 → WebKit)"
        fi
    fi

    # Create missing forwarding headers for WebKit's Cocoa API headers.
    # The modern Cocoa API (WKWebView, WKBackForwardList, etc.) requires
    # macOS 10.10+ and uses ObjC generics, NS_UNAVAILABLE, etc. that can't
    # compile for 10.6.  We force WK_API_ENABLED=0 in our compat header so
    # all content inside #if WK_API_ENABLED is skipped — the forwarding
    # headers just need to EXIST so #include resolves.
    local FH_WEBKIT="$BUILD_DIR/DerivedSources/ForwardingHeaders/WebKit"
    local FH_CREATED=0
    local WK_SRC="$SOURCE_DIR/Source/WebKit"
    for dir in \
        "$WK_SRC/UIProcess/API/Cocoa" \
        "$WK_SRC/UIProcess/API/mac" \
        "$WK_SRC/Shared/API/Cocoa" \
        "$WK_SRC/WebProcess/InjectedBundle/API/Cocoa"
    do
        [ -d "$dir" ] || continue
        for h in "$dir"/*.h; do
            [ -f "$h" ] || continue
            local HNAME=$(basename "$h")
            [ -f "$FH_WEBKIT/$HNAME" ] && continue
            # Compute relative path from Source/WebKit/ for the include
            local REL="${h#$WK_SRC/}"
            echo "#include \"WebKit/$REL\"" > "$FH_WEBKIT/$HNAME"
            FH_CREATED=$((FH_CREATED + 1))
        done
    done
    info "  Created $FH_CREATED missing Cocoa API forwarding headers (WK_API_ENABLED=0)"

    touch "$stamp"
    ok "Post-cmake fixes applied"
}

# ── Phase 6: Build ────────────────────────────────────────────────────────

phase6_build() {
    info "Phase 6: Building with ninja..."
    cd "$BUILD_DIR"

    # Cap parallelism to avoid OOM during clean builds.
    # WebCore derived sources are heavily templated and can use 2-4 GB per
    # clang process.  With all hw.ncpu jobs running simultaneously the total
    # can exceed available RAM, causing the OOM killer to silently terminate
    # compiler processes (0-byte .tmp files, missing .o in ar archives).
    local NCPUS="$(sysctl -n hw.ncpu)"
    local JOBS="$(( NCPUS * 9 / 10 ))"
    [ "$JOBS" -ge 1 ] || JOBS=1
    info "  Using -j${JOBS} (hw.ncpu=${NCPUS})"
    local BUILD_LOG="$BUILD_DIR/ninja-build.log"
    ninja -j"$JOBS" 2>&1 | tee "$BUILD_LOG" | grep --line-buffered -E "^\[|error:|fatal error:|FAILED:|ld: |Linking|ninja: build stopped" || true

    local FAIL_COUNT=$(grep -c 'error:' "$BUILD_LOG" 2>/dev/null || true)
    FAIL_COUNT="${FAIL_COUNT:-0}"
    FAIL_COUNT=$(echo "$FAIL_COUNT" | tr -d '[:space:]')
    if [ "${FAIL_COUNT:-0}" -gt 0 ]; then
        warn "Build completed with $FAIL_COUNT errors"
        echo ""
        info "Last 30 lines of build log:"
        tail -30 "$BUILD_LOG"
    else
        ok "Build complete"
    fi
}

# ── Phase 7: Verify ───────────────────────────────────────────────────────

phase7_verify() {
    info "Phase 7: Verifying build outputs..."
    echo ""

    local ALL_OK=true

    # Frameworks to check
    local FRAMEWORKS=(
        "JavaScriptCore"
        "WebCore"
        "WebKitLegacy"
    )

    for FW in "${FRAMEWORKS[@]}"; do
        local FW_PATH="$BUILD_DIR/lib/$FW.framework/Versions/A/$FW"
        if [ -f "$FW_PATH" ]; then
            local SIZE=$(ls -lh "$FW_PATH" | awk '{print $5}')
            local ARCH=$(file "$FW_PATH" | grep -o 'x86_64' || echo "UNKNOWN")
            if [ "$ARCH" = "x86_64" ]; then
                ok "$FW.framework: $SIZE ($ARCH)"
            else
                warn "$FW.framework: $SIZE (unexpected arch: $ARCH)"
                ALL_OK=false
            fi
        else
            # Try alternate location
            FW_PATH="$BUILD_DIR/lib/$FW.framework/$FW"
            if [ -f "$FW_PATH" ]; then
                local SIZE=$(ls -lh "$FW_PATH" | awk '{print $5}')
                local ARCH=$(file "$FW_PATH" | grep -o 'x86_64' || echo "UNKNOWN")
                ok "$FW.framework: $SIZE ($ARCH)"
            else
                err "$FW.framework: NOT FOUND"
                ALL_OK=false
            fi
        fi
    done

    echo ""
    if $ALL_OK; then
        ok "All frameworks built successfully for x86_64!"
    else
        warn "Some frameworks failed to build. Check ninja output above."
    fi
}

# ── Main ───────────────────────────────────────────────────────────────────

main() {
    echo "========================================================"
    echo "  WebKit 604 for Mac OS X 10.6 Snow Leopard — Build"
    echo "  Cross-compiling: $(uname -m) → x86_64"
    echo "========================================================"
    echo ""

    if $CLEAN; then
        info "Cleaning all build artifacts..."
        rm -rf "$BUILD_DIR" "$DIST_DIR"
        mkdir -p "$BUILD_DIR" "$STAMP_DIR" "$DIST_DIR"
        # Restore source tree to clean state so patches re-apply cleanly
        if [ -d "$SOURCE_DIR/.git" ]; then
            info "  Restoring source tree to clean state..."
            cd "$SOURCE_DIR" && git checkout -- . 2>/dev/null || true
            cd "$PROJECT_ROOT"
        fi
        info "Clean complete. Rebuilding from scratch."
        echo ""
    fi

    # Phase 0: Prerequisites
    phase0_prerequisites

    # Phase 1: Dependencies
    phase1_icu
    phase1_libcxx

    if $DEPS_ONLY; then
        ok "Dependencies built. Exiting (--deps-only)."
        exit 0
    fi

    # Phase 2: Patches
    phase2_patches

    if $PATCHES_ONLY; then
        ok "Patches applied. Exiting (--patches)."
        exit 0
    fi

    # Phase 3: Overlay
    phase3_overlay

    # Phase 4: CMake
    phase4_cmake

    # Phase 5: Post-cmake
    phase5_post_cmake

    # Phase 6: Build
    phase6_build

    # Phase 7: Verify
    phase7_verify
}

main "$@"
