#ifndef MEDIAACCESSIBILITY_STUB_H
#define MEDIAACCESSIBILITY_STUB_H
/*
 * Minimal MediaAccessibility/MediaAccessibility.h shim for the MacOSX 10.6 cross-build.
 * MediaAccessibility.framework is 10.7+ and absent from the 10.6 SDK.
 * MediaAccessibilitySoftLink.{h,cpp} reference MACaptionAppearance* functions in
 * dlsym typedefs; CaptionUserPreferencesMediaAF.cpp switches on the enum constants.
 * Functions resolve via dlopen at runtime (absent on 10.6 => nil, captions inert).
 * Only types + enum constants are needed at compile time. Enum values are
 * sequential placeholders; only the member NAMES must match for case labels.
 */
#include <CoreFoundation/CoreFoundation.h>
#include <CoreText/CoreText.h>
#include <CoreGraphics/CoreGraphics.h>

typedef enum {
    kMACaptionAppearanceDomainDefault = 0,
    kMACaptionAppearanceDomainUser    = 1
} MACaptionAppearanceDomain;

typedef enum {
    kMACaptionAppearanceBehaviorUseValue   = 0,
    kMACaptionAppearanceBehaviorUseContent = 1
} MACaptionAppearanceBehavior;

typedef enum {
    kMACaptionAppearanceDisplayTypeForcedOnly = 0,
    kMACaptionAppearanceDisplayTypeAutomatic  = 1,
    kMACaptionAppearanceDisplayTypeAlwaysOn   = 2
} MACaptionAppearanceDisplayType;

typedef enum {
    kMACaptionAppearanceFontStyleDefault                  = 0,
    kMACaptionAppearanceFontStyleMonospacedWithSerif      = 1,
    kMACaptionAppearanceFontStyleProportionalWithSerif    = 2,
    kMACaptionAppearanceFontStyleMonospacedWithoutSerif   = 3,
    kMACaptionAppearanceFontStyleProportionalWithoutSerif = 4,
    kMACaptionAppearanceFontStyleCasual                   = 5,
    kMACaptionAppearanceFontStyleCursive                  = 6,
    kMACaptionAppearanceFontStyleSmallCapital             = 7
} MACaptionAppearanceFontStyle;

typedef enum {
    kMACaptionAppearanceTextEdgeStyleUndefined  = 0,
    kMACaptionAppearanceTextEdgeStyleNone       = 1,
    kMACaptionAppearanceTextEdgeStyleRaised     = 2,
    kMACaptionAppearanceTextEdgeStyleDepressed  = 3,
    kMACaptionAppearanceTextEdgeStyleUniform    = 4,
    kMACaptionAppearanceTextEdgeStyleDropShadow = 5
} MACaptionAppearanceTextEdgeStyle;

#endif /* MEDIAACCESSIBILITY_STUB_H */
