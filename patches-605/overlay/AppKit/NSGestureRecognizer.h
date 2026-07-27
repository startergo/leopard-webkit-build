/*
 * AppKit/NSGestureRecognizer.h — minimal shim for the 10.6 SDK.
 *
 * NSGestureRecognizer is an AppKit base class introduced in OS X 10.10.
 * The MacOSX 10.6 SDK does not ship its header, so WebCore's
 * NSImmediateActionGestureRecognizerSPI.h (which `#imports
 * <AppKit/NSGestureRecognizer.h>` and declares NSImmediateActionGestureRecognizer
 * as its subclass) fails to find it.  Force Touch / immediate-action code paths
 * are guarded at runtime by NSClassFromString(@"NSImmediateActionGestureRecognizer"),
 * so the real framework class is only touched on 10.10+; this minimal declaration
 * exists solely so the SPI header (and its WebKitLegacy/WebCore consumers) compile
 * against older SDKs.  It is found via the build's overlay -I path before any
 * (absent) framework copy.
 */
#ifndef _NS_GESTURERECOGNIZER_SHIM_605
#define _NS_GESTURERECOGNIZER_SHIM_605

#import <AppKit/NSResponder.h>

NS_ASSUME_NONNULL_BEGIN

@class NSEvent;
@class NSGestureRecognizer;
@class NSView;

API_AVAILABLE(macos(10.10))
@protocol NSGestureRecognizerDelegate <NSObject>
@optional
- (BOOL)gestureRecognizer:(NSGestureRecognizer *)gestureRecognizer shouldAttemptToRecognizeWithEvent:(NSEvent *)event;
- (void)gestureRecognizerShouldBegin:(NSGestureRecognizer *)gestureRecognizer;
@end

API_AVAILABLE(macos(10.10))
@interface NSGestureRecognizer : NSResponder

@property (nullable, assign) id target;
@property (nullable) SEL action;
@property (getter=isEnabled) BOOL enabled;

- (instancetype)initWithTarget:(nullable id)target action:(nullable SEL)action;
- (void) addTarget:(id)target action:(SEL)action;
- (void) removeTarget:(nullable id)target action:(nullable SEL)action;

/* locationInView: — returns the gesture's location in the given view's coordinate
   space (per the 10.10 SDK, returns NSPoint).  WebImmediateActionController.mm:175
   calls it on an NSImmediateActionGestureRecognizer (a subclass); without this
   declaration the message returns `id` and the NSPoint assignment fails to compile.
   Runtime is guarded by NSClassFromString, so the method is only ever sent on 10.10+. */
- (NSPoint)locationInView:(nullable NSView *)view;

@end

NS_ASSUME_NONNULL_END

#endif /* _NS_GESTURERECOGNIZER_SHIM_605 */
