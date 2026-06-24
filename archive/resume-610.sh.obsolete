#!/bin/bash
# [leopard-webkit-build] Resume the 610 ninja build, re-applying the libicucore
# strip that cmake re-adds on every reconfigure. Use this instead of bare ninja.
cd "$(dirname "$0")/build-610" || exit 1
# [leopard] stage quoted-import headers not auto-copied to ForwardingHeaders root
FH="DerivedSources/ForwardingHeaders"
SRC="../sources_610/Source/WebCore"
for h in platform/text/cocoa/LocaleCocoa.h platform/text/PlatformLocale.h; do
    base=$(basename "$h")
    [ -f "$FH/$base" ] || cp "$SRC/$h" "$FH/$base" 2>/dev/null
done
grep -q 'sdk/MacOSX-SDKs/MacOSX10.6.sdk/usr/lib/libicucore.dylib' build.ninja || sed -i '' 's# /usr/lib/libicucore.dylib# /Users/macbookpro/leopard-webkit-build/sdk/MacOSX-SDKs/MacOSX10.6.sdk/usr/lib/libicucore.dylib#g' build.ninja
# Fix CMake list-expansion that joins linker flags with a literal ';' — the shell
# treats it as a command separator, causing "command not found" (exit 127) at link.
sed -i '' 's/;-compatibility_version/ -compatibility_version/g' build.ninja
# Restore ICU defines the Mac port omits (605 had them): disable symbol renaming
# so Intl's ICU C-API calls resolve against bundled ICU 55.
grep -q 'GL_GLES_PROTOTYPES=0' build.ninja || sed -i '' 's/-DBUILDING_WITH_CMAKE=1/-DBUILDING_WITH_CMAKE=1 -DU_DISABLE_RENAMING=1 -DU_SHOW_CPLUSPLUS_API=0 -DUCHAR_TYPE=uint16_t -DGL_GLES_PROTOTYPES=0/g' build.ninja
sed -i '' 's#libobjc.A.dylib;-compatibility_version#libobjc.A.dylib -compatibility_version#g' build.ninja
# Suppress -Wmissing-template-arg-list-after-template-kw: a newer-clang diagnostic
# (error-by-default) that fires on WebKit 610's valid `.template m()` idiom.
# Anchored to an always-present -Wno flag so it lands on every C++ compile line.
sed -i '' 's/-Wno-nontrivial-memcall/-Wno-nontrivial-memcall -Wno-missing-template-arg-list-after-template-kw/g' build.ninja
grep -q "include AppKitCompat610.h" build.ninja || sed -i "" "s/-include Overlay605Supplement.h/-include Overlay605Supplement.h -include AppKitCompat610.h/g" build.ninja
# Also re-apply the framework scrubs in case cmake reconfigured:
for fw in AVFoundation AVFAudio Metal DataDetectorsCore Lookup; do
    sed -i '' "s|/System/Library/Frameworks/$fw.framework||g; s|/System/Library/PrivateFrameworks/$fw.framework||g" build.ninja
    sed -i '' "s/-framework $fw //g; s/-framework $fw\$//g" build.ninja
done
sed -i '' 's|-iframework /Versions/Current/Frameworks||g' build.ninja
sed -i '' 's/-framework CFNetwork //g; s/-framework CFNetwork$//g' build.ninja

# [leopard] cmake regen / cleanup can leave the WebKitLegacy forwarding-headers dir EMPTY,
# breaking WebKitLegacy compile (WebView.h / WebKitLegacy/DOMCore.h not found). Regenerate a
# forwarding header for every WebKitLegacy *.h (mac/ preferred) pointing to its real rel path.
python3 - <<'REGEN_FH'
import pathlib
src = pathlib.Path("sources_610/Source/WebKitLegacy")
outdir = pathlib.Path("build-610/DerivedSources/ForwardingHeaders/WebKitLegacy")
outdir.mkdir(parents=True, exist_ok=True)
seen = {}
for h in src.rglob("*.h"):
    rel = h.relative_to(src).as_posix(); name = h.name
    if name in seen and "mac/" in seen[name] and "mac/" not in rel: continue
    (outdir / name).write_text("#import \"%s\"" % rel); seen[name] = rel
REGEN_FH
cp -f sources_610/Source/WebKitLegacy/mac/Plugins/Hosted/WebKitPluginHostTypes.h build-610/DerivedSources/WebKitLegacy/ 2>/dev/null || true
# [leopard] WebScriptObject.h exists in WebKitLegacy only as win/ (COM); the Mac definition is
# in WebCore. Redirect the WebKitLegacy forwarding header to WebCore so DOMObject's superclass resolves.
printf '#import <WebCore/WebScriptObject.h>' > "$FH/WebKitLegacy/WebScriptObject.h"
[ -f "$FH/WebKitLegacy/WebScriptObjectPrivate.h" ] && printf '#import <WebCore/WebScriptObjectPrivate.h>' > "$FH/WebKitLegacy/WebScriptObjectPrivate.h"
# [leopard] NPAPI headers (np*.h) live in WebCore/plugins, imported as <WebKitLegacy/np*.h>.
for _np in npapi npfunctions npruntime npruntime_internal np_jsobject; do
  if [ -f "sources_610/Source/WebCore/plugins/$_np.h" ]; then
    printf '#import "%s/sources_610/Source/WebCore/plugins/%s.h"' "$PWD" "$_np" > "$FH/WebKitLegacy/$_np.h"
  fi
done

# [leopard] cmake regen can rewrite WebKitLegacy forwarding headers as self-including stubs
# (#include of themselves -> "nested too deeply"). The generator's `if(NOT EXISTS)` guard then
# skips fixing them. Delete any self-includers so cmake/ninja regenerate them from the correct
# relative source paths (mac/.../X.h).
for f in "$FH"/WebKitLegacy/*.h; do
    [ -f "$f" ] || continue
    bn=$(basename "$f")
    if head -1 "$f" 2>/dev/null | grep -q "ForwardingHeaders/WebKitLegacy/$bn"; then
        rm -f "$f"
    fi
done

# [leopard] Stage WebCore editing/cocoa SPI headers that aren't auto-copied to ForwardingHeaders
# but are #imported by staged forwarding headers (HTMLConverter.h, AlternativeTextUIController.h).
for h in AttributedString.h AlternativeTextContextController.h WebViewVisualIdentificationOverlay.h; do
    src=$(find sources_610/Source/WebCore -name "$h" 2>/dev/null | head -1)
    [ -n "$src" ] && [ ! -f "$FH/WebCore/$h" ] && cp "$src" "$FH/WebCore/$h" 2>/dev/null
done

# [leopard] The generated WebKitLegacy/WebKitAvailability.h forwarding header self-includes
# (effectively empty), leaving WEBKIT_*_DEPRECATED_MAC undefined so 10_14 tokens leak as
# C++ user-defined-literals. Rewrite it with no-op macro definitions (regen-proof).
WKA="$FH/WebKitLegacy/WebKitAvailability.h"
if [ -f "$WKA" ] && ! grep -q 'WebKitLegacy_WebKitAvailability_h_leopard' "$WKA"; then
cat > "$WKA" <<'WKAEOF'
#ifndef WebKitLegacy_WebKitAvailability_h_leopard
#define WebKitLegacy_WebKitAvailability_h_leopard
#import <Foundation/NSObjCRuntime.h>
#ifndef WEBKIT_AVAILABLE_MAC
#define WEBKIT_AVAILABLE_MAC(introduced)
#endif
#ifndef WEBKIT_CLASS_AVAILABLE_MAC
#define WEBKIT_CLASS_AVAILABLE_MAC(introduced)
#endif
#ifndef WEBKIT_ENUM_AVAILABLE_MAC
#define WEBKIT_ENUM_AVAILABLE_MAC(introduced)
#endif
#ifndef WEBKIT_DEPRECATED_MAC
#define WEBKIT_DEPRECATED_MAC(introduced, deprecated, ...)
#endif
#ifndef WEBKIT_CLASS_DEPRECATED_MAC
#define WEBKIT_CLASS_DEPRECATED_MAC(introduced, deprecated, ...)
#endif
#ifndef WEBKIT_ENUM_DEPRECATED_MAC
#define WEBKIT_ENUM_DEPRECATED_MAC(introduced, deprecated, ...)
#endif
#ifndef JSC_API_AVAILABLE
#define JSC_API_AVAILABLE(...)
#endif
#endif
WKAEOF
fi

exec ninja -j12 -k 0 "$@"
