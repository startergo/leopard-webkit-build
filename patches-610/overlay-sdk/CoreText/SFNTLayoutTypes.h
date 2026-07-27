/*
 * CoreText/SFNTLayoutTypes.h shim — the MacOSX 10.6 CoreText.framework ships no
 * SFNTLayoutTypes.h public header (it became a public CoreText header later), so
 * WebKit's FontCacheCoreText.cpp:32 `#include <CoreText/SFNTLayoutTypes.h>` would be
 * a fatal "file not found".  The canonical AAT feature-type/selector enum for 10.6
 * lives instead in the ATS sub-framework
 * (ApplicationServices.framework/Frameworks/ATS.framework/Headers/SFNTLayoutTypes.h),
 * which is reachable via the ApplicationServices `-F` sub-framework path that WebCore
 * already sets.  This shim simply redirects the CoreText include to that real ATS
 * header, so kFractionsType / kNumberCaseType / kTextSpacingType / etc. resolve to
 * their correct enum values.
 *
 * The 10.6 ATS SFNTLayoutTypes.h ships a 378-member AAT enum but predates a handful
 * of selectors/types that modern CoreText added (CSS font-variant-caps, contextual/
 * historical ligatures, contextual alternates, the JIS-2004 selector).  Those few
 * constants — used by FontCacheCoreText.cpp — are #ifndef-guarded #defines below,
 * placed AFTER the ATS include so they only ever DEFINE names ATS lacks (they can
 * never clobber an ATS enum member).  Values are taken verbatim from the modern
 * CoreText.framework SFNTLayoutTypes.h; at runtime 10.6 CoreText honours the same
 * AAT feature-type/selector codes, so the symbolic names map to identical behaviour.
 * (The six font-variant-caps names are also provided by Overlay605Supplement.h, but
 * are repeated here so the shim is self-contained; both sets are #ifndef-guarded.)
 */
#ifndef SFNT_LAYOUT_TYPES_SHIM_605_H
#define SFNT_LAYOUT_TYPES_SHIM_605_H

#include <ATS/SFNTLayoutTypes.h>

/* ---- modern CoreText additions absent from the 10.6 ATS enum ---- */

/* CSS font-variant-caps: kLowerCaseType (37) / kUpperCaseType (38) + selectors. */
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

/* Contextual / historical ligatures and contextual alternates
   (CSS font-variant-ligatures / font-variant-alternates). */
#ifndef kContextualLigaturesOnSelector
#define kContextualLigaturesOnSelector 18
#endif
#ifndef kContextualLigaturesOffSelector
#define kContextualLigaturesOffSelector 19
#endif
#ifndef kHistoricalLigaturesOnSelector
#define kHistoricalLigaturesOnSelector 20
#endif
#ifndef kHistoricalLigaturesOffSelector
#define kHistoricalLigaturesOffSelector 21
#endif
#ifndef kContextualAlternatesType
#define kContextualAlternatesType 36
#endif
#ifndef kContextualAlternatesOnSelector
#define kContextualAlternatesOnSelector 0
#endif
#ifndef kContextualAlternatesOffSelector
#define kContextualAlternatesOffSelector 1
#endif

/* JIS-2004 glyph form selector (kCharacterShapeType family).  ATS 10.6 ships the
   JIS-1978/1983/1990 selectors but not the 2004 one (added to CoreText later). */
#ifndef kJIS2004CharactersSelector
#define kJIS2004CharactersSelector 11
#endif

#endif /* SFNT_LAYOUT_TYPES_SHIM_605_H */
