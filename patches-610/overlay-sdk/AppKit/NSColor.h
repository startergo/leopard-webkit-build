/* [leopard-webkit-build] Overlay: real SDK NSColor.h + a CGColor property category. */
#ifndef LEOPARD_NSCOLOR_OVERLAY_H
#define LEOPARD_NSCOLOR_OVERLAY_H
#include_next <AppKit/NSColor.h>
#if defined(__OBJC__) && __MAC_OS_X_VERSION_MAX_ALLOWED < 101000
#include <CoreGraphics/CoreGraphics.h>
@interface NSColor (LeopardCGColor)
@property (readonly) CGColorRef CGColor;
@end
#endif
#endif /* LEOPARD_NSCOLOR_OVERLAY_H */
