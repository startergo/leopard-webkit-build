# WebKit 610.1.15 -> Mac OS X 10.6.8 Snow Leopard (x86_64)

Reproduces the WebKit framework stack (WebCore + WebKitLegacy + JavaScriptCore)
for Snow Leopard, cross-compiled from an Apple Silicon host. Output is a
framework-injection wrapper around system Safari 5.0.5.

## Source baseline
Upstream tag: Safari-610.1.15.5  (single squashed commit / 203 files on top)

## Reproduce
0. Fetch pinned dependencies (ICU 62.2, ICU 55.1, libc++ 5.0.1, SDKs) as
   submodules:
       git submodule update --init
1. Check out WebKit at tag Safari-610.1.15.5.
2. Apply the source patch:
       git apply webkit-610-snowleopard-source.patch
   (equivalently: git am series/0001-*.patch)
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

WebGL (direct-CGL on OpenGL 2.1): ANGLE desktop-GLSL translator enabled
(ANGLE_ENABLE_GLSL) with SH_GLSL_COMPATIBILITY_OUTPUT; WebGLLayer 10.6
presentation (plain GL_TEXTURE_2D FBO, CGImage contents, copy-to-temp readback,
internal-FBO binding-state init); Widget paint via graphicsContextWithGraphicsPort.
The WebGL cube and WebGL Aquarium render GPU-accelerated on a GeForce 9400.

User agent: presents the genuine Safari 14 / Big Sur string for this WebKit
generation (10_15_7, AppleWebKit/605.1.15, Version/14.0.1) so modern sites and
reCAPTCHA accept the browser. Override with
defaults write com.apple.Safari CustomUserAgent.

Real-hardware crash/rendering fixes: AppKit/CoreText/scroll/control/inspector
(NSGraphicsContext graphics-port APIs, MediaAccessibility, overlay-scrollbar
SPIs, focus-ring, CoreText OpenType-feature 10.10+ key absence,
_web_URLWithLowercasedScheme / _schemeWasUpgradedDueToDynamicHSTS shims);
GitHub rendering (synthetic-redirect + cross-origin CORS) and UserTiming.

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
