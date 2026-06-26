# WebKit 610.1.15 -> Mac OS X 10.6.8 Snow Leopard (x86_64)

Reproduces the WebKit framework stack (WebCore + WebKitLegacy + JavaScriptCore)
for Snow Leopard, cross-compiled from an Apple Silicon host. Output is a
framework-injection wrapper around system Safari 5.0.5.

## Source baseline
Upstream tag: Safari-610.1.15.5  (41 commits / 193 files on top)

## Reproduce
1. Check out WebKit at tag Safari-610.1.15.5.
2. Apply the source patch:
       git apply webkit-610-snowleopard-source.patch
   (or apply series/000*.patch in order; 0002 only deletes upstream-absent
   .rej files and is safe to skip)
3. Build (performs all build-time generation):
       ./build_610.sh --clean
4. Output: build-610/WebKit.app (~158 MB)

## Source patch scope (10.6 compile/link/runtime adaptations)
SDK-compat guards across WTF / JSC / WebCore / WebKitLegacy / bmalloc:
task_vm_info / VM_MEMORY_* guards; ThemeMac form-control selector guards;
software paint path (CA compositing off, 10.6-absent CG/AX SPIs guarded);
complex-text + cookie-write fixes; NSTextCheckingInsertionPointKey symbol;
WebViewVisualIdentificationOverlay caller guards;
convertBaseToScreen/convertScreenToBase restorations; OptionsMac/PlatformMac
cmake.

## build_610.sh scope (build-time generation, not in source patch)
Forwarding headers (flat WebCore redirects limited to genuine include-path
gaps; WebCore-prefix headers for WebKitLegacy, which carries no WebCore source
subdirs on its -I path; ANGLE / JSC / pal / wtf prefixes; GLES/KHR/EGL trees);
605 runtime stub objects; cmake configure; build.ninja regen-edge neutering;
UserAgentScripts.h generation.

Forwarding-header rule of note: emit a flat ForwardingHeaders/<X>.h redirect
EXCEPT when the header's source dir is already on WebCore's -I path AND cmake
stages a ForwardingHeaders/WebCore/<X>.h copy. Only that combination makes the
flat redirect a redundant second physical path, which broke #pragma once
(DictationContextType redefinition) in WebKitLegacy.
