/*
 * AppKit/NSWindowScreenConversionCompat.h — 10.6 back-port of the 10.7+
 * NSWindow screen-coordinate RECT conversion API.
 *
 * 10.7 replaced the 10.6 -convertBaseToScreen: / -convertScreenToBase: POINT
 * methods with -convertRectToScreen: / -convertRectFromScreen: (NSRect in/out).
 * WebHTMLView.mm and WebImmediateActionController.mm (safari-605 branch) call the
 * NSRect form at 7 sites.  This category declares the two methods so the call
 * sites compile and resolve to NSRect (not id).  A WORKING 10.6 implementation —
 * bridging through the point primitives (origin converted, size preserved, which
 * is exactly the transform the 10.7 methods perform) — lives in
 * sdk_stubs_605.mm (NSWindow/WebKitCompat605).  Guarded < 1070 so newer SDKs that
 * declare the real methods are unaffected.
 *
 * This is a separately-#imported header (NOT the force-included overlay) because a
 * category requires NSWindow's full @interface to already be visible; import it
 * after AppKit.
 */
#ifndef NSWINDOW_SCREEN_CONVERSION_COMPAT_605_H
#define NSWINDOW_SCREEN_CONVERSION_COMPAT_605_H

#import <AppKit/NSWindow.h>

#if defined(__OBJC__) && __MAC_OS_X_VERSION_MAX_ALLOWED < 1070

@interface NSWindow (WebKitCompat605)
- (NSRect)convertRectFromScreen:(NSRect)rect;
- (NSRect)convertRectToScreen:(NSRect)rect;
@end

#endif

#endif /* NSWINDOW_SCREEN_CONVERSION_COMPAT_605_H */
