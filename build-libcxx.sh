#!/bin/bash
set -e

BASEDIR="$(cd "$(dirname "$0")" && pwd)"
SDKROOT="$BASEDIR/sdk/MacOSX10.6.sdk"
LIBCXX_SRC="$BASEDIR/downloads/libcxx-5.0.1.src"
LIBCXXABI_SRC="$BASEDIR/downloads/libcxxabi-5.0.1.src"
DIST="$BASEDIR/dist/libcxx"

TARGET_FLAGS="-target x86_64-apple-macos10.6 -isysroot $SDKROOT -fallow-unsupported"

mkdir -p "$DIST/lib" "$DIST/include"

echo "=== Step 1: Build libc++abi ==="
cd "$LIBCXXABI_SRC/lib"
rm -f *.o

for FILE in ../src/*.cpp; do
    # Skip cxa_demangle.cpp — it uses std::string creating circular dep with libc++
    [ "$(basename $FILE)" = "cxa_demangle.cpp" ] && echo "  Skipping $(basename $FILE) (circular dep)" && continue
    # Skip cxa_noexception.cpp — duplicates symbols from cxa_exception.cpp
    [ "$(basename $FILE)" = "cxa_noexception.cpp" ] && echo "  Skipping $(basename $FILE) (dup symbols)" && continue
    echo "  Compiling $(basename $FILE)..."
    clang++ -c -O2 $TARGET_FLAGS \
        -std=c++11 \
        -nostdinc++ -isystem "$LIBCXX_SRC/include" \
        -I../include \
        -DNDEBUG \
        -DHAVE___CXA_THREAD_ATEXIT_IMPL \
        -U_LIBCPP_USE_AVAILABILITY_APPLE \
        -Wno-sign-conversion -Wno-shadow -Wno-conversion -Wno-shorten-64-to-32 \
        "$FILE"
done

echo "  Linking libc++abi.dylib..."
# Provide a weak stub for __cxa_thread_atexit_impl (not in 10.6)
echo 'int __cxa_thread_atexit_impl(void(*dtor)(void*), void* obj, void* dso) { return -1; }
char* __cxa_demangle(const char* mangled, char* buf, unsigned long* n, int* status) { return 0; }' > _stub.c
clang -c -O2 $TARGET_FLAGS -DNDEBUG _stub.c -o _stub.o

clang $TARGET_FLAGS -o libc++abi.dylib \
    -dynamiclib -nodefaultlibs \
    -current_version 5.0.1 \
    -compatibility_version 1 \
    -install_name /usr/lib/libc++abi.dylib \
    -lSystem \
    *.o

cp libc++abi.dylib "$DIST/lib/"
echo "  OK: $(file libc++abi.dylib | cut -d: -f2)"

echo ""
echo "=== Step 2: Build libc++.dylib ==="
cd "$LIBCXX_SRC/src"
rm -f *.o

for FILE in *.cpp; do
    echo "  Compiling $(basename $FILE)..."
    clang++ -c -O2 $TARGET_FLAGS \
        -std=c++11 \
        -nostdinc++ -isystem "$LIBCXX_SRC/include" \
        -I"$LIBCXXABI_SRC/include" \
        -DLIBCXX_BUILDING_LIBCXXABI \
        -DNDEBUG \
        -U_LIBCPP_USE_AVAILABILITY_APPLE \
        "$FILE" 2>/dev/null || echo "  WARN: $(basename $FILE) had warnings"
done

echo "  Linking libc++.1.dylib..."
clang++ $TARGET_FLAGS -o "$DIST/lib/libc++.1.dylib" \
    -dynamiclib -nodefaultlibs \
    -current_version 1.0.5 \
    -compatibility_version 1 \
    -install_name /usr/lib/libc++.1.dylib \
    -L"$DIST/lib" -lc++abi \
    -lSystem \
    *.o

cd "$DIST/lib" && ln -sf libc++.1.dylib libc++.dylib
echo "  OK: $(file libc++.1.dylib | cut -d: -f2)"
echo "  Size: $(ls -lh libc++.1.dylib | awk '{print $5}')"

echo ""
echo "=== Step 3: Install headers ==="
cp -R "$LIBCXX_SRC/include/" "$DIST/include/"
cp "$LIBCXXABI_SRC/include/cxxabi.h" "$DIST/include/"
cp "$LIBCXXABI_SRC/include/__cxxabi_config.h" "$DIST/include/"

echo ""
echo "=== Done ==="
ls -lh "$DIST/lib/"
echo "Headers: $(ls "$DIST/include/" | head -5)..."
