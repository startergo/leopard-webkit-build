#ifndef LEOPARD_WEBKIT
#define LEOPARD_WEBKIT 1
#endif
/*
 * Overlay605Supplement.h
 *
 * Force-included into WebKit 605 translation units AFTER
 * TargetConditionals_compat.h (a second -include on the compile command line).
 * It provides the small set of 10.7+ AppKit *enum types* that 605's PAL SPI
 * headers reference as property/parameter types but which are absent from the
 * public MacOSX 10.6 SDK headers.
 *
 * We deliberately use PLAIN typedefs + #ifndef-guarded constants here — NOT
 * NS_ENUM — because:
 *   * NS_ENUM itself is unavailable in the 10.6 SDK / not defined by the base
 *     overlay, so `typedef NS_ENUM(...)` would not expand.
 *   * TargetConditionals_compat.h §22 already #defines NSScrollerStyleLegacy /
 *     NSScrollerStyleOverlay as integer macros; an NS_ENUM enumerator of the
 *     same name would expand to `0 = 0` and fail to compile.
 *
 * This mirrors the proven 604-overlay pattern (plain `typedef long` for the
 * type; preprocessor macros for the named values) and is idempotent.
 */
#ifndef OVERLAY_605_SUPPLEMENT_H
#define OVERLAY_605_SUPPLEMENT_H

/* CF_OPTIONS / CF_ENUM — CoreFoundation enum-definition macros absent from the
   10.6 SDK (they first appear in CFAvailability.h, 10.8+).  WebKit's PAL SPI
   headers use the modern form `typedef CF_OPTIONS(_type, _name) { ... };` which
   assumes these macros exist.  WebCorePrefix.h:113-116 ships a fallback
   (`_type _name; enum`) but that form is malformed under a leading `typedef`:
   it yields a stray `typedef _type _name;` + a bare anonymous enum that clang
   rejects as a type/typedef self-conflict, so the enumerators never enter scope
   and downstream code sees "undeclared identifier" (e.g. Font.cpp:373).
   This block mirrors the ACTIVE branch of a modern CFAvailability.h (line 146):
   `typedef _type _name` provides the named alias (an integer typedef, so the
   bitwise/ternary expressions in e.g. Font.cpp:373 compile) and `enum : _name`
   defines an anonymous fixed-underlying-type enum for the enumerators.
   Defining them here means WebCorePrefix.h's `#ifndef CF_OPTIONS`/`CF_ENUM`
   guards skip the broken fallback.  Verified against: the 2-arg CF_OPTIONS form
   (CoreTextSPI.h), the 2-arg CF_ENUM form (CoreGraphicsSPI.h), the 1-arg
   anonymous CF_ENUM form (CoreAudioSPI.h), and the exact Font.cpp:373 pattern.
   The flag_enum/enum_extensibility attributes are intentionally omitted — they
   affect only warnings/Swift bridging, not codegen. */
#ifndef CF_OPTIONS
#define CF_OPTIONS(_type, _name) _type _name; enum : _name
#endif
#ifndef __CF_NAMED_ENUM
#define __CF_NAMED_ENUM(_type, _name) _type _name; enum : _name
#endif
#ifndef __CF_ANON_ENUM
#define __CF_ANON_ENUM(_type) enum : _type
#endif
#ifndef __CF_ENUM_GET_MACRO
#define __CF_ENUM_GET_MACRO(_1, _2, NAME, ...) NAME
#endif
#ifndef CF_ENUM
#define CF_ENUM(...) __CF_ENUM_GET_MACRO(__VA_ARGS__, __CF_NAMED_ENUM, __CF_ANON_ENUM, )(__VA_ARGS__)
#endif

/* CF_BRIDGED_TYPE / CF_BRIDGED_MUTABLE_TYPE — CoreFoundation toll-free-bridging
   annotations absent from the 10.6 SDK (added in 10.8 CFAvailability.h).  Used in
   PAL SPI headers as `typedef struct CF_BRIDGED_TYPE(id) FooObj *FooRef;`.  With
   the macro undefined, clang treats CF_BRIDGED_TYPE as a struct tag and the
   declaration fails to parse (CoreGraphicsSPI.h:201-202 CGSRegionObj/CGStyleRef).
   Non-ARC build → expands to nothing, yielding a clean opaque-pointer typedef. */
#ifndef CF_BRIDGED_TYPE
#define CF_BRIDGED_TYPE(T)
#endif
#ifndef CF_BRIDGED_MUTABLE_TYPE
#define CF_BRIDGED_MUTABLE_TYPE(T)
#endif

/* CoreText trait aliases — modern CoreText (CTFontTraits.h) spells the symbolic
   trait bits kCTFontTraitBold / kCTFontTraitItalic; the 10.6 SDK only has the
   kCTFontBoldTrait / kCTFontItalicTrait spellings.  Alias them so FontCocoa.mm
   compiles (the values are identical: 1<<1 and 1<<0). */
#ifndef kCTFontTraitBold
#define kCTFontTraitBold kCTFontBoldTrait
#endif
#ifndef kCTFontTraitItalic
#define kCTFontTraitItalic kCTFontItalicTrait
#endif
/* kCTFontTraitColorGlyphs — symbolic-trait bit for colour-bitmap fonts ('sbix',
   'COLR', 'SVG '), added to CTFontTraits.h in 10.8.  Absent from the 10.6 SDK;
   used by FontPlatformDataCocoa.mm:50 (`CTFontGetSymbolicTraits(font) &
   kCTFontTraitColorGlyphs`).  Value (1<<13) per modern CTFontTraits.h:66. */
#ifndef kCTFontTraitColorGlyphs
#define kCTFontTraitColorGlyphs (1 << 13)
#endif

/* CTFontOrientation enumerators — the 10.6 SDK declares the CTFontOrientation
   TYPE (used in the CTFontGetBoundingRectsForGlyphs / CTFontGetAdvancesForGlyphs
   signatures, CTFont.h:807/835) but does NOT ship the named enumerators
   kCTFontOrientationDefault/Horizontal/Vertical (added to CTFontDescriptor.h in
   10.8).  FontCocoa.mm passes them as the orientation argument; without the
   names both "undeclared identifier" and the cascading "no matching function for
   call" fire.  Values per modern CTFontDescriptor.h:190-192.  (Plain macros, not
   an enum, so they can never clash with the SDK's own CTFontOrientation type.) */
#ifndef kCTFontOrientationDefault
#define kCTFontOrientationDefault 0
#endif
#ifndef kCTFontOrientationHorizontal
#define kCTFontOrientationHorizontal 1
#endif
#ifndef kCTFontOrientationVertical
#define kCTFontOrientationVertical 2
#endif

/* AAT font feature constants — the MacOSX 10.6 SDK ships the canonical AAT
   feature-type/selector enum in ATS.framework/Headers/SFNTLayoutTypes.h, pulled in
   transitively by ApplicationServices.h (→ ATS.h → ATSLayoutTypes.h).  So
   kFractionsType / kNumberCaseType / kTextSpacingType / kNumberSpacingType /
   kStyleOptionsType / kTypographicExtrasType / kVerticalPositionType /
   kLowerCaseNumbersSelector / kUpperCaseNumbersSelector resolve to their REAL
   values from that header — they MUST NOT be redefined here (a #define clobbers
   the enum members and turns the declaration into `enum { 6 = 6, ... }`, producing
   "expected identifier").
   However the 10.6 ATS SFNTLayoutTypes.h predates the CSS font-variant-caps
   additions (kLowerCaseType / kUpperCaseType and their SmallCaps / PetiteCaps
   selectors), which were added to the modern CoreText SFNTLayoutTypes.h.  Provide
   ONLY those here so FontCocoa.mm / FontCacheCoreText.cpp see them.  Values per
   modern SFNTLayoutTypes.h; 10.6 CoreText honours the AAT codes at runtime. */
#ifndef kLowerCaseType
#define kLowerCaseType 37
#endif
#ifndef kUpperCaseType
#define kUpperCaseType 38
#endif
#ifndef kLowerCaseSmallCapsSelector
#define kLowerCaseSmallCapsSelector 1
#endif
#ifndef kLowerCasePetiteCapsSelector
#define kLowerCasePetiteCapsSelector 2
#endif
#ifndef kUpperCaseSmallCapsSelector
#define kUpperCaseSmallCapsSelector 1
#endif
#ifndef kUpperCasePetiteCapsSelector
#define kUpperCasePetiteCapsSelector 2
#endif

/* NSScrollerStyle — AppKit enum (10.7 Lion overlay scrollers).
   Used as a type by pal/spi/mac/NSScrollerImpSPI.h and WebHTMLView.mm.
   The constant values NSScrollerStyleLegacy / NSScrollerStyleOverlay are
   provided by TargetConditionals_compat.h §22 (force-included before this). */
#ifndef __NSSCROLLERSTYLE_TYPEDEF_605__
#define __NSSCROLLERSTYLE_TYPEDEF_605__
enum NSScrollerStyle : long;
#endif

/* NSScrollerKnobStyle — AppKit enum (Default/Light/Dark), 10.7+.
   Used as a property type by pal/spi/mac/NSScrollerImpSPI.h:59.
   TC_compat does not define the knob-style constants, so provide them here. */
#ifndef __NSSCROLLERKNOBSTYLE_TYPEDEF_605__
#define __NSSCROLLERKNOBSTYLE_TYPEDEF_605__
typedef long NSScrollerKnobStyle;
#endif
#ifndef NSScrollerKnobStyleDefault
#define NSScrollerKnobStyleDefault 0
#endif
#ifndef NSScrollerKnobStyleDark
#define NSScrollerKnobStyleDark 1
#endif
#ifndef NSScrollerKnobStyleLight
#define NSScrollerKnobStyleLight 2
#endif

/* qos_class_t + QOS_CLASS_* — pthread / dispatch thread QoS (10.10+).
   bmalloc's Scavenger uses qos_class_t as a member / parameter type
   (Scavenger.h:51/52/89), bmalloc.h:116, bmalloc.cpp:87 — all under
   #if BOS(DARWIN) — and QOS_CLASS_USER_INITIATED as the default scavenger QoS.
   The type is defined nowhere in the MacOSX 10.6 SDK (there is no
   <pthread/qos.h>), so define it here so the bmalloc TUs compile.  We also
   forward-declare pthread_set_qos_class_self_np(), which Scavenger.cpp:189
   calls (with no #include that declares it); its no-op DEFINITION lives in
   sdk_stubs_605.mm so it links and the scavenger runs at default priority. */
#ifndef __QOS_CLASS_T_TYPEDEF_605__
#define __QOS_CLASS_T_TYPEDEF_605__
typedef unsigned int qos_class_t;
#endif
#ifndef QOS_CLASS_UNSPECIFIED
#define QOS_CLASS_UNSPECIFIED       0x00
#endif
#ifndef QOS_CLASS_BACKGROUND
#define QOS_CLASS_BACKGROUND        0x09
#endif
#ifndef QOS_CLASS_UTILITY
#define QOS_CLASS_UTILITY           0x11
#endif
#ifndef QOS_CLASS_DEFAULT
#define QOS_CLASS_DEFAULT           0x15
#endif
#ifndef QOS_CLASS_USER_INITIATED
#define QOS_CLASS_USER_INITIATED    0x19
#endif
#ifndef QOS_CLASS_USER_INTERACTIVE
#define QOS_CLASS_USER_INTERACTIVE  0x21
#endif
extern int pthread_set_qos_class_self_np(qos_class_t qos_class, int relative_priority);

/* CF_ENUM / CF_OPTIONS / CF_CLOSED_ENUM — the typed-enum macros modern
   CoreFoundation SPI headers (e.g. wtf/spi/cf/CFStringSPI.h:38) rely on.  The
   10.6 SDK's CFBase.h predates these macros (added ~10.8), so `CF_ENUM(...)` is
   left undefined and clang parses the enum as a function declaration.  The
   underlying symbols these SPI headers declare DO exist on 10.6 —
   CFStringGetRangeOfCharacterClusterAtIndex and _CFStringGetUserDefaultEncoding
   are both in the 10.6 CoreFoundation binary — so only the macro is missing.
   Definitions mirror the upstream CFBase.h verbatim. */
#ifndef CF_ENUM
#if (defined(__cplusplus) && __cplusplus >= 201103L && (__has_extension(cxx_strong_enums) || __has_feature(objc_fixed_enum))) || (!defined(__cplusplus) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)
#define CF_CLOSED_ENUM(_type, _name) enum _name : _type _name; enum _name : _type
#define CF_ENUM(_type, _name)       enum _name : _type _name; enum _name : _type
#define CF_OPTIONS(_type, _name)    _type _name; enum _name : _type
#else
#define CF_CLOSED_ENUM(_type, _name) _type _name; enum
#define CF_ENUM(_type, _name)        _type _name; enum
#define CF_OPTIONS(_type, _name)     _type _name; enum
#endif
#endif

/* ENABLE_INSPECTOR_ALTERNATE_DISPATCHERS — FeatureDefines.h defaults it to 1,
   but it carries a static_assert (FeatureDefines.h:677) that REQUIRES
   ENABLE_REMOTE_INSPECTOR.  We disable REMOTE_INSPECTOR for the 10.6 build (its
   inspector XPC service / backend needs post-10.6 infrastructure), so this must
   be 0 too.  This header is force-included (-include) before FeatureDefines.h is
   seen, so the `#if !defined(...)` guard there honors our 0 and the assert holds. */
#ifndef ENABLE_INSPECTOR_ALTERNATE_DISPATCHERS
#define ENABLE_INSPECTOR_ALTERNATE_DISPATCHERS 0
#endif

/* HAVE_QOS_CLASSES — Platform.h:1167 defaults it to 1, gating
   pthread_attr_set_qos_class_np (ThreadingPthreads.cpp:233) and friends.  QoS
   classes are a 10.10+ pthread/dispatch feature; force this off so the QoS call
   sites compile out.  (bmalloc's Scavenger uses pthread_set_qos_class_self_np
   under BOS(DARWIN), not HAVE(QOS_CLASSES) — that one is satisfied by a no-op
   in sdk_stubs_605.mm.) */
#ifndef HAVE_QOS_CLASSES
#define HAVE_QOS_CLASSES 0
#endif

/* NS_ENUM / NS_OPTIONS / NS_CLOSED_ENUM — the typed-enum macros modern AppKit
   SPI headers (e.g. pal/spi/mac/NSMenuSPI.h:38 `typedef NS_ENUM(NSInteger,
   NSMenuType)`) and many PAL/AppKit SPI headers rely on.  The 10.6 SDK's
   NSObjCRuntime.h predates these macros (added ~10.8), so `NS_ENUM(...)` is
   left undefined and clang parses the enum as a function declaration.  The
   underlying types these headers declare DO exist on 10.6 (or are no-ops), so
   only the macro is missing.

   Usage convention: every call site in this tree (WebCore + WebKitLegacy) pairs
   the macro with a leading `typedef` — `typedef NS_ENUM(NSInteger, Foo) { ... };`
   (23 NS_ENUM + 4 NS_OPTIONS sites in WebCore alone, plus WebKitLegacy's
   WebPolicyDelegate.h / WebPreferences.h / WebUIDelegate.h).  We use Apple's
   actual modern form: `enum _n : _t _n; enum _n : _t`.  The first clause supplies
   a typedef declarator (`... _n;`) so `typedef NS_ENUM(_t,_n) {...}` expands to
   `typedef enum _n : _t _n; enum _n : _t { ... }` — TWO statements: a typedef that
   binds the bare name `_n`, then the enum body.  This is REQUIRED for plain-C
   sources: WebKitLegacy's .m delegate files (WebDefaultPolicyDelegate.m,
   WebDefaultEditingDelegate.m, ...) are compiled as C, where a bare enum TAG is
   not usable without the `enum` keyword.  A "simple" `enum _n : _t` form produces
   `typedef enum _n : _t { ... }` (typedef with no declarator), so `_n` survives
   only as an enum tag and C usage fails with "must use 'enum' tag to refer to
   type" — exactly the break seen on those .m files.  (.mm/ObjC++ tolerates the
   bare tag, which is why an earlier WebCore-only probe missed it; WebCore is
   all .mm, WebKitLegacy has the plain-C .m delegates.)  Verified against the
   cross-compiler: Apple's form compiles a .m that uses the bare enum name in a
   C signature (RC=0); the simple form fails identically to the build. */
#ifndef NS_ENUM
#if (__cplusplus && __cplusplus >= 201103L && (__has_extension(cxx_strong_enums) || __has_feature(objc_fixed_enum))) || (!__cplusplus && __has_feature(objc_fixed_enum))
#define NS_ENUM(_type, _name) enum _name : _type _name; enum _name : _type
#define NS_OPTIONS(_type, _name) enum _name : _type _name; enum _name : _type
#define NS_CLOSED_ENUM(_type, _name) enum _name : _type _name; enum _name : _type
#else
#define NS_ENUM(_type, _name) _name; enum
#define NS_OPTIONS(_type, _name) _name; enum
#define NS_CLOSED_ENUM(_type, _name) _name; enum
#endif
#endif

/* CF_AVAILABLE / NS_CLASS_AVAILABLE / NS_DESIGNATED_INITIALIZER — CoreFoundation
   & AppKit availability/annotation macros introduced ~10.8, absent from the
   10.6 SDK.  The shared overlay already defines NS_AVAILABLE / NS_DEPRECATED /
   API_AVAILABLE, but NOT the CF_* family.  Modern 605 JSC API headers annotate
   ~50 declarations with CF_AVAILABLE(10_6, 7_0) (e.g. JSObjectRef.h:455);
   without a definition clang sees the macro token after a function declarator
   and errors "expected function body after function declarator".  We target
   10.6 (min-required), so availability checking is moot — define them as empty
   (argument-consuming) no-ops. */
#ifndef CF_AVAILABLE
#define CF_AVAILABLE(_mac, _ios)
#endif
#ifndef CF_AVAILABLE_IOS
#define CF_AVAILABLE_IOS(_ios)
#endif
#ifndef CF_AVAILABLE_MAC
#define CF_AVAILABLE_MAC(_mac)
#endif
#ifndef CF_ENUM_AVAILABLE
#define CF_ENUM_AVAILABLE(_mac, _ios)
#endif
#ifndef CF_ENUM_AVAILABLE_IOS
#define CF_ENUM_AVAILABLE_IOS(_ios)
#endif
#ifndef CF_ENUM_AVAILABLE_MAC
#define CF_ENUM_AVAILABLE_MAC(_mac)
#endif
#ifndef CF_ENUM_DEPRECATED
#define CF_ENUM_DEPRECATED(_macIntro, _macDep, _iosIntro, _iosDep, ...)
#endif
#ifndef CF_DEPRECATED
#define CF_DEPRECATED(_macIntro, _macDep, _iosIntro, _iosDep, ...)
#endif
#ifndef CF_DEPRECATED_IOS
#define CF_DEPRECATED_IOS(_iosIntro, _iosDep, ...)
#endif
#ifndef CF_DEPRECATED_MAC
#define CF_DEPRECATED_MAC(_macIntro, _macDep, ...)
#endif
#ifndef NS_CLASS_AVAILABLE
#define NS_CLASS_AVAILABLE(_mac, _ios)
#endif
#ifndef NS_CLASS_AVAILABLE_IOS
#define NS_CLASS_AVAILABLE_IOS(_ios)
#endif
#ifndef NS_CLASS_DEPRECATED
#define NS_CLASS_DEPRECATED(_macIntro, _macDep, _iosIntro, _iosDep, ...)
#endif
#ifndef NS_DESIGNATED_INITIALIZER
#define NS_DESIGNATED_INITIALIZER
#endif
#ifndef NS_REQUIRES_SUPER
#define NS_REQUIRES_SUPER
#endif
#ifndef NS_SWIFT_NAME
#define NS_SWIFT_NAME(_name)
#endif
#ifndef CF_SWIFT_NAME
#define CF_SWIFT_NAME(_name)
#endif

/* NS_RETURNS_NOT_RETAINED / NS_RETURNS_RETAINED — Foundation memory-management
   attribute macros (~10.7, absent from the 10.6 NSObjCRuntime.h).  WebKitLegacy's
   public DOM headers annotate methods with NS_RETURNS_NOT_RETAINED (e.g.
   DOMMutationEvent.h); without a definition the macro token after a method
   declarator misparses.  Expand to the underlying clang attributes. */
#ifndef NS_RETURNS_NOT_RETAINED
#define NS_RETURNS_NOT_RETAINED __attribute__((ns_returns_not_retained))
#endif
#ifndef NS_RETURNS_RETAINED
#define NS_RETURNS_RETAINED __attribute__((ns_returns_retained))
#endif
/* NS_RETURNS_INNER_POINTER — Foundation attribute (~10.7, absent from the 10.6
   NSObjCRuntime.h) annotating methods that return a pointer into the receiver's
   storage.  PAL's NSEventSPI.h annotates -_eventRef with it; without a definition
   the macro token after the declarator makes clang report "expected ';' after
   method prototype".  Expand to the underlying clang attribute. */
#ifndef NS_RETURNS_INNER_POINTER
#define NS_RETURNS_INNER_POINTER __attribute__((objc_returns_inner_pointer))
#endif

/* __deprecated_msg(_msg) — the compiler/SDK availability macro that
   NS_DEPRECATED_MSG and WebKit's ICON_DATABASE_DEPRECATED expand to.  The 10.6
   SDK's Availability headers predate it, so it is left UNDEFINED and clang parses
   `__deprecated_msg("...")` as a function call ("expected parameter declarator"),
   breaking every declaration annotated with it (WebIconDatabase.h defines
   ICON_DATABASE_DEPRECATED unconditionally from it, at file AND method scope).
   Map it to the underlying clang attribute, which this toolchain supports. */
#ifndef __deprecated_msg
#define __deprecated_msg(_msg) __attribute__((deprecated(_msg)))
#endif

/* NS_*_MAC / NS_*_IOS single-argument availability annotations (~10.8, absent
   from the 10.6 SDK).  WebKitAvailability.h maps its WEBKIT_*_MAC macros onto
   these (WEBKIT_AVAILABLE_MAC->NS_AVAILABLE_MAC, WEBKIT_CLASS_AVAILABLE_MAC->
   NS_CLASS_AVAILABLE_MAC, WEBKIT_ENUM_AVAILABLE_MAC->NS_ENUM_AVAILABLE_MAC,
   WEBKIT_DEPRECATED_MAC->NS_DEPRECATED_MAC); without definitions the macro
   token after a function/method/enum declarator misparses.  The shared overlay
   only defines the two-argument NS_AVAILABLE(_mac,_ios) / NS_DEPRECATED(...) /
   NS_ENUM_AVAILABLE(_mac,_ios) forms — these single-arg variants are missing.
   We target 10.6 (min-required), so availability checking is moot: define them
   as empty (argument-consuming) no-ops. */
#ifndef NS_AVAILABLE_MAC
#define NS_AVAILABLE_MAC(_mac)
#endif
#ifndef NS_AVAILABLE_IOS
#define NS_AVAILABLE_IOS(_ios)
#endif
#ifndef NS_CLASS_AVAILABLE_MAC
#define NS_CLASS_AVAILABLE_MAC(_mac)
#endif
#ifndef NS_CLASS_AVAILABLE_IOS
#define NS_CLASS_AVAILABLE_IOS(_ios)
#endif
#ifndef NS_DEPRECATED_MAC
#define NS_DEPRECATED_MAC(_macIntro, _macDep)
#endif
#ifndef NS_DEPRECATED_IOS
#define NS_DEPRECATED_IOS(_iosIntro, _iosDep)
#endif
#ifndef NS_ENUM_AVAILABLE_MAC
#define NS_ENUM_AVAILABLE_MAC(_mac)
#endif
#ifndef NS_ENUM_AVAILABLE_IOS
#define NS_ENUM_AVAILABLE_IOS(_ios)
#endif
#ifndef NS_ENUM_DEPRECATED_MAC
#define NS_ENUM_DEPRECATED_MAC(_macIntro, _macDep, _iosIntro, _iosDep)
#endif
#ifndef NS_ENUM_DEPRECATED_IOS
#define NS_ENUM_DEPRECATED_IOS(_iosIntro, _iosDep)
#endif

/* NSAccessibility* attributes/subroles referenced by 605 WebCore accessibility
   code that were added after 10.6 (NSAccessibilityMarkedMisspelledTextAttribute
   ~10.10, NSAccessibilityToggleSubrole / NSAccessibilitySwitchSubrole ~10.13).
   Absent from the MacOSX 10.6 SDK.  Provide fallbacks so the TUs compile; only
   active when building against a pre-10.13 SDK. */
#if __MAC_OS_X_VERSION_MAX_ALLOWED < 101300
/* The 10.6 SDK ships NSAccessibilityMisspelledTextAttribute (the un-"Marked"
   name) — it is the same spelling attribute, so alias to it. */
#ifndef NSAccessibilityMarkedMisspelledTextAttribute
#define NSAccessibilityMarkedMisspelledTextAttribute NSAccessibilityMisspelledTextAttribute
#endif
/* Subrole strings: pass the canonical AX string literals; on 10.6 the runtime
   and assistive clients simply ignore unrecognized subroles. */
#ifndef NSAccessibilityToggleSubrole
#define NSAccessibilityToggleSubrole @"AXToggle"
#endif
#ifndef NSAccessibilitySwitchSubrole
#define NSAccessibilitySwitchSubrole @"AXSwitch"
#endif
#endif

/* NSTextAlignment — the modern (10.12+) enum-member names for text alignment.
   Apple renamed the classic NSLeftTextAlignment family to NSTextAlignmentLeft etc.
   for Swift interop; WebCore's editing/cocoa/HTMLConverter.mm uses the modern
   spellings. The 10.6 SDK only has the classic names (NSText.h:30-34), so map each
   modern name to its classic equivalent. These macros expand at the USE site
   (after AppKit is imported), where the classic names are in scope. Values are
   ABI-identical (0-4). */
#ifndef NSTextAlignmentLeft
#define NSTextAlignmentLeft NSLeftTextAlignment
#endif
#ifndef NSTextAlignmentRight
#define NSTextAlignmentRight NSRightTextAlignment
#endif
#ifndef NSTextAlignmentCenter
#define NSTextAlignmentCenter NSCenterTextAlignment
#endif
#ifndef NSTextAlignmentJustified
#define NSTextAlignmentJustified NSJustifiedTextAlignment
#endif
#ifndef NSTextAlignmentNatural
#define NSTextAlignmentNatural NSNaturalTextAlignment
#endif

/* NSWritingDirection text-attribute values — HTMLConverter.mm:1843/1848 build the
   NSAttributedString NSWritingDirectionAttributeName array using the modern
   (10.12+) names NSWritingDirectionEmbedding / NSWritingDirectionOverride. The
   10.6 SDK ships the same attribute values under the classic spellings
   NSTextWritingDirectionEmbedding (=(0<<1)=0) / NSTextWritingDirectionOverride
   (=(1<<1)=2) in NSText.h:50-51. Map modern→classic; expands at the use site. */
#ifndef NSWritingDirectionEmbedding
#define NSWritingDirectionEmbedding NSTextWritingDirectionEmbedding
#endif
#ifndef NSWritingDirectionOverride
#define NSWritingDirectionOverride NSTextWritingDirectionOverride
#endif

/* NSURLCredentialPersistenceSynchronizable — the iCloud-sync credential
   persistence option (10.10+). CredentialCocoa.mm references it in a switch;
   on the 10.6 SDK it's absent. Define it to the real enum value (3) so the switch
   case resolves. */
#ifndef NSURLCredentialPersistenceSynchronizable
#define NSURLCredentialPersistenceSynchronizable 3
#endif

/* NSFileCoordinatorReadingOptions + NSFileCoordinatorReadingWithoutChanges — the
   coordinated file-reading API (10.7+). BlobDataFileReferenceMac.mm uses it to
   coordinate reading a blob source file. Define the type + the "without changes"
   option so the call site compiles; sdk_stubs_605.mm provides a no-op NSFileCoordinator
   that invokes the accessor block immediately (no coordination on 10.6). */
#ifndef NSFileCoordinatorReadingOptions
typedef unsigned long NSFileCoordinatorReadingOptions;
#endif
#ifndef NSFileCoordinatorReadingWithoutChanges
#define NSFileCoordinatorReadingWithoutChanges 1UL
#endif

/* NSCorrectionIndicatorType + NSCorrectionResponse — AppKit spell-checker
   autocorrection UI types (10.7+). CorrectionPanel.h/.mm in WebKitLegacy uses them.
   The 10.6 SDK predates the autocorrection panel API. Define the type + constants so
   the TUs compile; at runtime the autocorrection SPI (NSSpellChecker
   showCorrectionIndicatorOfType:...) exists on 10.6 via the private SPI path. */
#ifndef NSCorrectionIndicatorType
typedef unsigned long NSCorrectionIndicatorType;
#endif
#ifndef NSCorrectionIndicatorTypeDefault
#define NSCorrectionIndicatorTypeDefault ((NSCorrectionIndicatorType)0)
#endif
#ifndef NSCorrectionIndicatorTypeReversion
#define NSCorrectionIndicatorTypeReversion ((NSCorrectionIndicatorType)1)
#endif
#ifndef NSCorrectionIndicatorTypeGuesses
#define NSCorrectionIndicatorTypeGuesses ((NSCorrectionIndicatorType)2)
#endif
#ifndef NSCorrectionResponseAccepted
#define NSCorrectionResponseAccepted 0
#endif
#ifndef NSCorrectionResponseRejected
#define NSCorrectionResponseRejected 1
#endif
#ifndef NSCorrectionResponseIgnored
#define NSCorrectionResponseIgnored 2
#endif
#ifndef NSCorrectionResponseEdited
#define NSCorrectionResponseEdited 3
#endif

/* NSEdgeInsets — contentInsets struct (10.10+ AppKit). platform/mac/ScrollViewMac.mm
   uses it as a category return type and accesses .top/.left/.right/.bottom. Define it
   here (CGFloat == double on x86_64) so the type resolves without the 10.10 SDK. */
#ifndef NSEdgeInsets
typedef struct { double top, left, bottom, right; } NSEdgeInsets;
#endif

/* NSActivityOptions + the NSActivity* constants — NSProcessInfo activity API
   (10.6.6+ runtime). The 10.6.0 SDK headers predate them, so UserActivityMac.mm
   sees neither the type nor the bit constants. Provide both; the runtime symbols
   (-beginActivityWithOptions:reason: / -endActivity:) ARE present on 10.6.6+. */
#ifndef NSActivityOptions
typedef unsigned long NSActivityOptions;
#endif
#ifndef NSActivityUserInitiatedAllowingIdleSystemSleep
#define NSActivityUserInitiatedAllowingIdleSystemSleep (1UL << 19)
#endif
#ifndef NSActivityLatencyCritical
#define NSActivityLatencyCritical (1ULL << 63)
#endif
#ifndef NSActivitySuddenTerminationDisabled
#define NSActivitySuddenTerminationDisabled (1UL << 14)
#endif
#ifndef NSActivityAutomaticTerminationDisabled
#define NSActivityAutomaticTerminationDisabled (1UL << 15)
#endif

/* NSPreferredScrollerStyleDidChangeNotification — AppKit distributed notification
   (10.7+ Lion overlay scrollers), observed by WebCore's ScrollbarThemeMac when
   the user toggles the scroller style.  Absent from the MacOSX 10.6 SDK.  The
   notification name is a plain NSString constant; pass the canonical name.  On
   10.6 the scroller style never changes at runtime (legacy scrollers only), so
   the observer is effectively a no-op. */
#ifndef NSPreferredScrollerStyleDidChangeNotification
#define NSPreferredScrollerStyleDidChangeNotification @"NSPreferredScrollerStyleDidChange"
#endif

/* NSURLConnectionDelegate — the formal @protocol introduced in 10.7.  WebKit's
   resource-handle delegate classes conform to it (NSObject <NSURLConnectionDelegate>
   in WebCoreResourceHandleAsOperationQueueDelegate.h / ResourceHandleMac.mm).  On the
   10.6 SDK NSURLConnectionDelegate exists only as an INFORMAL category
   (NSObject(NSURLConnectionDelegate)), so the formal-protocol conformance clause
   fails to compile.  On 10.6 NSURLConnection dispatches via informal-protocol
   selectors, so an empty formal protocol is sufficient and harmless at runtime.
   Guarded by __OBJC__ so this header's force-include into pure-C++ TUs is a no-op,
   and by the SDK version so newer SDKs (which declare the real protocol) are
   unaffected. */
#if defined(__OBJC__) && __MAC_OS_X_VERSION_MAX_ALLOWED < 1070
@protocol NSURLConnectionDelegate
@end
#endif

/* NSDraggingSource / NSDraggingDestination / NSDraggingInfo — formal @protocols
   promoted from informal categories in 10.7.  WebKitLegacy declares formal
   conformance to them (e.g. WebHTMLViewInternal.h:44 `WebHTMLView () <NSDraggingSource>`,
   and WebHTMLView <NSDraggingDestination>).  On the 10.6 SDK these exist only as
   NSObject categories, so the `<NSDraggingSource>` conformance clause fails with
   "cannot find protocol declaration for 'NSDraggingSource'".  On 10.6 the drag
   machinery dispatches via informal-protocol selectors, so empty formal protocols
   are sufficient and harmless at runtime.  Same guard rationale as above. */
#if defined(__OBJC__) && __MAC_OS_X_VERSION_MAX_ALLOWED < 1070
@protocol NSDraggingSource
@end
@protocol NSDraggingDestination
@end
@protocol NSDraggingInfo
/* Minimal method set actually consumed by WebView.mm's drag-destination handlers
   (draggingEntered:/Updated:/Exited:/performDragOperation:).  Declared here (not
   left empty) so the typed returns resolve at the call sites — otherwise clang
   treats them as `id` and the IntPoint/static_cast initialisations fail.  Returns
   use NSUInteger (not NSDragOperation) and id so this protocol compiles in the
   force-include header without an AppKit drag-headers dependency; NSUInteger IS
   NSDragOperation's underlying type, so static_cast<DragOperation> still works. */
- (NSPoint)draggingLocation;
- (NSUInteger)draggingSourceOperationMask;
- (id)draggingSource;
@end
#endif

/* AppKit names introduced after 10.6, consumed only as bare VALUES (not method
   sends) so defining them is runtime-safe for a 10.6 target.

   NSWindowStyleMaskFullScreen — the 10.12 name for the fullscreen style bit
   (1 << 14 = 16384), stable across every macOS version.  WebInspectorClient does
   `[window styleMask] & NSWindowStyleMaskFullScreen`.  Alias to the literal
   value rather than the 10.5-era NSFullScreenWindowMask, which this 10.6 SDK
   build does not expose (it is conditionally compiled out of NSWindow.h here).
   NSModalResponse — the 10.10 NSInteger enum for sheet/panel results.
   WebInspectorClient compares an NSOpenPanel runModal result against
   NSModalResponseCancel; on 10.6 runModal returns a plain NSInteger
   (0 = cancel, 1 = OK), so these values compare correctly. */
#if defined(__OBJC__) && __MAC_OS_X_VERSION_MAX_ALLOWED < 101200
#ifndef NSWindowStyleMaskFullScreen
#define NSWindowStyleMaskFullScreen (1 << 14)
#endif
#endif
#if defined(__OBJC__) && __MAC_OS_X_VERSION_MAX_ALLOWED < 101000
typedef NS_ENUM(NSInteger, NSModalResponse) {
    NSModalResponseCancel = 0,
    NSModalResponseOK = 1,
    NSModalResponseStop = -1000,
    NSModalResponseAbort,
    NSModalResponseContinue
};
#endif

/* NSDraggingSession / NSDraggingContext — the modern drag-session API (10.7+).
   WebHTMLView declares the NSDraggingSource protocol methods
   -draggingSession:sourceOperationMaskForDraggingContext: and
   -draggingSession:endedAtPoint:operation: (WebHTMLView.mm:4395/4406).  These are
   dispatched only by the 10.7+ drag machinery, which itself only runs when
   -beginDraggingSessionWithItems: is used — and WebDragClient gates that to 10.7+,
   so these methods are never called at runtime on 10.6.  The types are needed only
   so the method SIGNATURES compile: NSDraggingSession as a forward-declared class
   (the bodies never dereference it) and NSDraggingContext as a typedef enum
   (values per AppKit/NSDragging.h). */
#if defined(__OBJC__) && __MAC_OS_X_VERSION_MAX_ALLOWED < 1070
@class NSDraggingSession;
typedef NS_ENUM(NSInteger, NSDraggingContext) {
    NSDraggingContextOutsideApplication = 0,
    NSDraggingContextWithinApplication,
};
#endif

/* NSWindow screen-coordinate conversion — the 10.7+ replacements for the 10.6
   convertBaseToScreen:/convertScreenToBase: pair (convertRectFromScreen: /
   convertRectToScreen:).  The CATEGORY DECLARATION lives in a separately-imported
   overlay header, AppKit/NSWindowScreenConversionCompat.h, because a category
   requires NSWindow's full @interface to already be visible — and this header is
   force-included (-include) BEFORE each TU's own #import <AppKit/...>, so NSWindow
   is unknown here.  The @implementation (a WORKING 10.6 bridge through the point
   primitives convertScreenToBase:/convertBaseToScreen:) lives in sdk_stubs_605.mm. */


/* [leopard] CFAutorelease() declaration (10.9+ API; defined in sdk_stubs_605.mm).
   Provides the prototype so callers compile; the symbol resolves at link. */
#if defined(LEOPARD_WEBKIT)
#ifdef __cplusplus
extern "C" {
#endif
extern CFTypeRef CFAutorelease(CFTypeRef);
#ifdef __cplusplus
}
#endif
#endif


/* [leopard] vImage *_BGRA8888 premultiply helpers are 10.7+; the 10.6 SDK has only
   the *_RGBA8888 variants. The (un)premultiply math is channel-order-independent
   (alpha is byte 3 in both layouts), so the RGBA entry points are equivalent. */
#if defined(LEOPARD_WEBKIT)
#ifndef vImageUnpremultiplyData_BGRA8888
#define vImageUnpremultiplyData_BGRA8888 vImageUnpremultiplyData_RGBA8888
#endif
#ifndef vImagePremultiplyData_BGRA8888
#define vImagePremultiplyData_BGRA8888 vImagePremultiplyData_RGBA8888
#endif
#endif

#endif /* OVERLAY_605_SUPPLEMENT_H */
