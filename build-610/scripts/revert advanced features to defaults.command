#!/bin/bash
for k in WebKitAcceleratedCompositingEnabled WebKitAccelerated2dCanvasEnabled WebKitAcceleratedDrawingEnabled WebKitCanvasUsesAcceleratedDrawing WebKitWebGLEnabled WebKitHiddenPageDOMTimerThrottlingEnabled WebKitHiddenPageCSSAnimationSuspensionEnabled; do
    defaults delete com.apple.Safari "$k" 2>/dev/null
done
echo "Advanced features reverted to defaults."
