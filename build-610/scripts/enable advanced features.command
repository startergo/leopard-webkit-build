#!/bin/bash
defaults write com.apple.Safari WebKitAcceleratedCompositingEnabled -bool YES
defaults write com.apple.Safari WebKitAccelerated2dCanvasEnabled -bool YES
defaults write com.apple.Safari WebKitAcceleratedDrawingEnabled -bool YES
defaults write com.apple.Safari WebKitCanvasUsesAcceleratedDrawing -bool YES
defaults write com.apple.Safari WebKitWebGLEnabled -bool YES
defaults write com.apple.Safari WebKitHiddenPageDOMTimerThrottlingEnabled -bool YES
defaults write com.apple.Safari WebKitHiddenPageCSSAnimationSuspensionEnabled -bool YES
echo "Advanced features enabled for Safari."
