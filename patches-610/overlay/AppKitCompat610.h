/*
 * AppKitCompat610.h  [leopard]
 * Maps modern AppKit enum & constant names (10.7-10.13 renames) to their
 * 10.6-SDK equivalents. Force-included after the base SDK headers.
 * Each mapping is #ifndef-guarded (no-op where SDK already provides it).
 * Renames only - identical underlying values.
 */
#ifndef APPKIT_COMPAT_610_H
#define APPKIT_COMPAT_610_H

#if defined(__OBJC__)

#ifndef NSControlSizeRegular
#define NSControlSizeRegular NSRegularControlSize
#endif
#ifndef NSControlSizeSmall
#define NSControlSizeSmall NSSmallControlSize
#endif
#ifndef NSControlSizeMini
#define NSControlSizeMini NSMiniControlSize
#endif

#ifndef NSControlStateValueOn
#define NSControlStateValueOn NSOnState
#endif
#ifndef NSControlStateValueOff
#define NSControlStateValueOff NSOffState
#endif
#ifndef NSControlStateValueMixed
#define NSControlStateValueMixed NSMixedState
#endif

#ifndef NSEventTypeLeftMouseDown
#define NSEventTypeLeftMouseDown NSLeftMouseDown
#endif
#ifndef NSEventTypeLeftMouseUp
#define NSEventTypeLeftMouseUp NSLeftMouseUp
#endif
#ifndef NSEventTypeLeftMouseDragged
#define NSEventTypeLeftMouseDragged NSLeftMouseDragged
#endif
#ifndef NSEventTypeRightMouseDown
#define NSEventTypeRightMouseDown NSRightMouseDown
#endif
#ifndef NSEventTypeRightMouseUp
#define NSEventTypeRightMouseUp NSRightMouseUp
#endif
#ifndef NSEventTypeRightMouseDragged
#define NSEventTypeRightMouseDragged NSRightMouseDragged
#endif
#ifndef NSEventTypeOtherMouseDown
#define NSEventTypeOtherMouseDown NSOtherMouseDown
#endif
#ifndef NSEventTypeOtherMouseUp
#define NSEventTypeOtherMouseUp NSOtherMouseUp
#endif
#ifndef NSEventTypeOtherMouseDragged
#define NSEventTypeOtherMouseDragged NSOtherMouseDragged
#endif
#ifndef NSEventTypeMouseMoved
#define NSEventTypeMouseMoved NSMouseMoved
#endif
#ifndef NSEventTypeMouseEntered
#define NSEventTypeMouseEntered NSMouseEntered
#endif
#ifndef NSEventTypeMouseExited
#define NSEventTypeMouseExited NSMouseExited
#endif
#ifndef NSEventTypeScrollWheel
#define NSEventTypeScrollWheel NSScrollWheel
#endif
#ifndef NSEventTypeKeyDown
#define NSEventTypeKeyDown NSKeyDown
#endif
#ifndef NSEventTypeKeyUp
#define NSEventTypeKeyUp NSKeyUp
#endif
#ifndef NSEventTypePressure
#define NSEventTypePressure 34
#endif
#ifndef NSEventMaskAny
#define NSEventMaskAny NSAnyEventMask
#endif

#ifndef NSCompositingOperationSourceOver
#define NSCompositingOperationSourceOver NSCompositeSourceOver
#endif
#ifndef NSCompositingOperationCopy
#define NSCompositingOperationCopy NSCompositeCopy
#endif

#ifndef NSWindowStyleMaskBorderless
#define NSWindowStyleMaskBorderless NSBorderlessWindowMask
#endif
#ifndef NSWindowStyleMaskTitled
#define NSWindowStyleMaskTitled NSTitledWindowMask
#endif

#ifndef NSButtonTypeSwitch
#define NSButtonTypeSwitch NSSwitchButton
#endif
#ifndef NSButtonTypeRadio
#define NSButtonTypeRadio NSRadioButton
#endif

#ifndef NSBitmapImageFileTypePNG
#define NSBitmapImageFileTypePNG NSPNGFileType
#endif

#ifndef NSPasteboardNameDrag
#define NSPasteboardNameDrag NSDragPboard
#endif


#ifndef NSEventTypeFlagsChanged
#define NSEventTypeFlagsChanged NSFlagsChanged
#endif

#ifndef NSEventModifierFlagControl
#define NSEventModifierFlagControl NSControlKeyMask
#endif
#ifndef NSEventModifierFlagShift
#define NSEventModifierFlagShift NSShiftKeyMask
#endif
#ifndef NSEventModifierFlagOption
#define NSEventModifierFlagOption NSAlternateKeyMask
#endif
#ifndef NSEventModifierFlagCommand
#define NSEventModifierFlagCommand NSCommandKeyMask
#endif
#ifndef NSEventModifierFlagCapsLock
#define NSEventModifierFlagCapsLock NSAlphaShiftKeyMask
#endif
#ifndef NSEventModifierFlagFunction
#define NSEventModifierFlagFunction NSFunctionKeyMask
#endif
#ifndef NSEventModifierFlagNumericPad
#define NSEventModifierFlagNumericPad NSNumericPadKeyMask
#endif

#ifndef NSBezelStyleRounded
#define NSBezelStyleRounded NSRoundedBezelStyle
#endif
#ifndef NSBezelStyleShadowlessSquare
#define NSBezelStyleShadowlessSquare NSShadowlessSquareBezelStyle
#endif
#ifndef NSBezelStyleTexturedSquare
#define NSBezelStyleTexturedSquare NSTexturedSquareBezelStyle
#endif

#ifndef NSSliderTypeLinear
#define NSSliderTypeLinear NSLinearSlider
#endif

#ifndef NSLevelIndicatorStyleContinuousCapacity
#define NSLevelIndicatorStyleContinuousCapacity NSContinuousCapacityLevelIndicatorStyle
#endif
#ifndef NSLevelIndicatorStyleDiscreteCapacity
#define NSLevelIndicatorStyleDiscreteCapacity NSDiscreteCapacityLevelIndicatorStyle
#endif
#ifndef NSLevelIndicatorStyleRating
#define NSLevelIndicatorStyleRating NSRatingLevelIndicatorStyle
#endif
#ifndef NSLevelIndicatorStyleRelevancy
#define NSLevelIndicatorStyleRelevancy NSRelevancyLevelIndicatorStyle
#endif

#ifndef NSButtonTypeMomentaryPushIn
#define NSButtonTypeMomentaryPushIn NSMomentaryPushInButton
#endif


#ifndef NSPasteboardTypeURL
#define NSPasteboardTypeURL NSURLPboardType
#endif
#ifndef NSPasteboardTypeFileURL
#define NSPasteboardTypeFileURL NSFilenamesPboardType
#endif

#ifndef NSScrollerStyleLegacy
#define NSScrollerStyleLegacy ((NSScrollerStyle)0)
#endif
#ifndef NSScrollerStyleOverlay
#define NSScrollerStyleOverlay ((NSScrollerStyle)1)
#endif

#ifndef kVK_RightCommand
#define kVK_RightCommand 0x36
#endif


#ifndef __NSPASTEBOARDTYPE_COMPAT_610__
#define __NSPASTEBOARDTYPE_COMPAT_610__
typedef NSString *NSPasteboardType;
#endif

#ifndef __NSCONTROLSTATEVALUE_COMPAT_610__
#define __NSCONTROLSTATEVALUE_COMPAT_610__
typedef NSInteger NSControlStateValue;
#endif

#ifndef __NSHTTPCOOKIEPROPERTYKEY_COMPAT_610__
#define __NSHTTPCOOKIEPROPERTYKEY_COMPAT_610__
typedef NSString *NSHTTPCookiePropertyKey;
#endif


/* [leopard] CALayerDelegate is a formal @protocol only on 10.12+. On 10.6 layer
   delegation is informal. Declare an empty protocol so <CALayerDelegate>
   conformance clauses compile. */
#if defined(LEOPARD_WEBKIT)
@protocol CALayerDelegate;
#endif


/* [leopard] NSAppearance is a 10.9+ class. Forward-declare it so SPI headers that
   use it as a method/property type (e.g. NSScrollerImpSPI.h) compile. Runtime use
   is confined to dark-mode paths that do not execute on 10.6. */
#if defined(LEOPARD_WEBKIT)
@class NSAppearance;
#endif


#ifndef NSEventMaskLeftMouseUp
#define NSEventMaskLeftMouseUp NSLeftMouseUpMask
#endif
#ifndef NSEventMaskLeftMouseDragged
#define NSEventMaskLeftMouseDragged NSLeftMouseDraggedMask
#endif
#ifndef NSEventModifierFlagDeviceIndependentFlagsMask
#define NSEventModifierFlagDeviceIndependentFlagsMask NSDeviceIndependentModifierFlagsMask
#endif


#ifndef NSWindowStyleMaskResizable
#define NSWindowStyleMaskResizable NSResizableWindowMask
#endif
#ifndef NSWindowStyleMaskMiniaturizable
#define NSWindowStyleMaskMiniaturizable NSMiniaturizableWindowMask
#endif
#ifndef NSWindowStyleMaskClosable
#define NSWindowStyleMaskClosable NSClosableWindowMask
#endif

#endif
#endif
