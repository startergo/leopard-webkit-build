/* [leopard-webkit-build] Overlay: pull the real SDK NSGraphicsContext.h, then append a
   CGContext property category mapping to 10.6's -graphicsPort. This overlay is first in -I,
   so it shadows the SDK header; #include_next reaches the real one. The category only exists
   in TUs that import AppKit/NSGraphicsContext (so non-AppKit TUs are unaffected). */
#ifndef LEOPARD_NSGRAPHICSCONTEXT_OVERLAY_H
#define LEOPARD_NSGRAPHICSCONTEXT_OVERLAY_H
#include_next <AppKit/NSGraphicsContext.h>
#if defined(__OBJC__) && __MAC_OS_X_VERSION_MAX_ALLOWED < 101000
#include <CoreGraphics/CoreGraphics.h>
@interface NSGraphicsContext (LeopardCGContext)
@property (readonly) CGContextRef CGContext;
@end
#endif
#endif /* LEOPARD_NSGRAPHICSCONTEXT_OVERLAY_H */
