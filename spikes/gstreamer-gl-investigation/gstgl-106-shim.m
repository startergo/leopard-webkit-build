// gstgl-106-shim.m — adds 10.7+-only NSView selectors that GstGL 1.4.5
// calls unconditionally. Without these, glimagesink crashes on 10.6.
// On 10.6 these have benign identity/no-op behavior (no Retina → points == pixels).
//
// Loaded via DYLD_INSERT_LIBRARIES prior to gst-launch (or Safari).

#import <AppKit/AppKit.h>
#import <objc/runtime.h>

static void gstgl106shim_setWantsBestResolutionOpenGLSurface(id __unused self, SEL __unused cmd, BOOL __unused flag) { }
static NSRect gstgl106shim_convertRectToBacking(id __unused self, SEL __unused cmd, NSRect aRect) { return aRect; }
static NSRect gstgl106shim_convertRectFromBacking(id __unused self, SEL __unused cmd, NSRect aRect) { return aRect; }
static NSPoint gstgl106shim_convertPointToBacking(id __unused self, SEL __unused cmd, NSPoint p) { return p; }
static NSPoint gstgl106shim_convertPointFromBacking(id __unused self, SEL __unused cmd, NSPoint p) { return p; }
static NSSize gstgl106shim_convertSizeToBacking(id __unused self, SEL __unused cmd, NSSize s) { return s; }
static NSSize gstgl106shim_convertSizeFromBacking(id __unused self, SEL __unused cmd, NSSize s) { return s; }
static double gstgl106shim_backingScaleFactor(id __unused self, SEL __unused cmd) { return 1.0; }

@interface NSView (GstGL106Shim)
- (void)setWantsBestResolutionOpenGLSurface:(BOOL)flag;
- (NSRect)convertRectToBacking:(NSRect)aRect;
- (NSRect)convertRectFromBacking:(NSRect)aRect;
- (NSPoint)convertPointToBacking:(NSPoint)p;
- (NSPoint)convertPointFromBacking:(NSPoint)p;
- (NSSize)convertSizeToBacking:(NSSize)s;
- (NSSize)convertSizeFromBacking:(NSSize)s;
- (double)backingScaleFactor;
@end

@implementation NSView (GstGL106Shim)
+ (void)load {
    struct { SEL sel; IMP imp; const char *enc; } table[] = {
        { @selector(setWantsBestResolutionOpenGLSurface:), (IMP)gstgl106shim_setWantsBestResolutionOpenGLSurface, "v@:B" },
        { @selector(convertRectToBacking:),                (IMP)gstgl106shim_convertRectToBacking,                "{NSRect=dddd}@:{NSRect=dddd}" },
        { @selector(convertRectFromBacking:),              (IMP)gstgl106shim_convertRectFromBacking,              "{NSRect=dddd}@:{NSRect=dddd}" },
        { @selector(convertPointToBacking:),               (IMP)gstgl106shim_convertPointToBacking,               "{NSPoint=dd}@:{NSPoint=dd}" },
        { @selector(convertPointFromBacking:),             (IMP)gstgl106shim_convertPointFromBacking,             "{NSPoint=dd}@:{NSPoint=dd}" },
        { @selector(convertSizeToBacking:),                (IMP)gstgl106shim_convertSizeToBacking,                "{NSSize=dd}@:{NSSize=dd}" },
        { @selector(convertSizeFromBacking:),              (IMP)gstgl106shim_convertSizeFromBacking,              "{NSSize=dd}@:{NSSize=dd}" },
        { @selector(backingScaleFactor),                   (IMP)gstgl106shim_backingScaleFactor,                   "d@:" },
    };
    for (size_t i = 0; i < sizeof(table)/sizeof(table[0]); i++) {
        if (!class_getInstanceMethod(self, table[i].sel)) {
            class_addMethod(self, table[i].sel, table[i].imp, table[i].enc);
            NSLog(@"gstgl-106-shim: added no-op %@ to NSView", NSStringFromSelector(table[i].sel));
        }
    }
}
@end
