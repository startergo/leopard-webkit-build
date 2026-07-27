// [leopard] Compatibility definitions for 10.7+ symbols absent on 10.6 that WebCore references.
// Compiled WITHOUT the TargetConditionals_compat overlay; linked into WebCore. Constants get
// sensible values; functions are no-op/safe stubs. WebGL EGL/GLES come from libGLESv2.a.
#import <CoreFoundation/CoreFoundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <CoreText/CoreText.h>
#import <objc/objc.h>
#import <stdint.h>
// Avoid <Foundation/Foundation.h> — it pulls <Security/SecKeychain.h> whose AUTH_TYPE_FIX_('ntlm')
// multichar-code enums are rejected by this clang. We only need NSString* for one constant; declare
// the class and the literal builder minimally.
@class NSString;
extern "C" NSString *NSStringFromCFString(CFStringRef) ;


extern "C" {

// ---- Gigacage: disablePrimitiveGigacageRequested is provided by JSC (resolves at load). ----

// ---- CoreText font-weight constants (CGFloat; standard NSFontWeight scale) ----
__attribute__((used)) extern const CGFloat kCTFontWeightUltraLight = -0.80;
__attribute__((used)) extern const CGFloat kCTFontWeightThin       = -0.60;
__attribute__((used)) extern const CGFloat kCTFontWeightLight      = -0.40;
__attribute__((used)) extern const CGFloat kCTFontWeightRegular    =  0.00;
__attribute__((used)) extern const CGFloat kCTFontWeightMedium     =  0.23;
__attribute__((used)) extern const CGFloat kCTFontWeightSemibold   =  0.30;
__attribute__((used)) extern const CGFloat kCTFontWeightBold       =  0.40;
__attribute__((used)) extern const CGFloat kCTFontWeightHeavy      =  0.56;
__attribute__((used)) extern const CGFloat kCTFontWeightBlack      =  0.62;

// ---- CoreText CFStringRef attribute/family constants ----
__attribute__((used)) extern const CFStringRef kCTFontCSSFamilySerif      = CFSTR("CTFontCSSFamilySerif");
__attribute__((used)) extern const CFStringRef kCTFontCSSFamilySansSerif  = CFSTR("CTFontCSSFamilySansSerif");
__attribute__((used)) extern const CFStringRef kCTFontCSSFamilyCursive    = CFSTR("CTFontCSSFamilyCursive");
__attribute__((used)) extern const CFStringRef kCTFontCSSFamilyFantasy    = CFSTR("CTFontCSSFamilyFantasy");
__attribute__((used)) extern const CFStringRef kCTFontCSSFamilyMonospace  = CFSTR("CTFontCSSFamilyMonospace");
__attribute__((used)) extern const CFStringRef kCTFontCSSWeightAttribute  = CFSTR("CTFontCSSWeightAttribute");
__attribute__((used)) extern const CFStringRef kCTFontCSSWidthAttribute   = CFSTR("CTFontCSSWidthAttribute");
__attribute__((used)) extern const CFStringRef kCTFontFallbackOptionAttribute = CFSTR("CTFontFallbackOptionAttribute");
__attribute__((used)) extern const CFStringRef kCTFontPostScriptNameAttribute = CFSTR("NSCTFontPostScriptNameAttribute");
__attribute__((used)) extern const CFStringRef kCTFontUserInstalledAttribute  = CFSTR("CTFontUserInstalledAttribute");
__attribute__((used)) extern const CFStringRef kCTFontUIFontDesignTrait   = CFSTR("CTFontUIFontDesignTrait");
__attribute__((used)) extern const CFStringRef kCTFontUIFontDesignDefault = CFSTR("CTFontUIFontDesignDefault");

// ---- CFNetwork constants ----
__attribute__((used)) extern NSString * const NSURLAuthenticationMethodOAuth = (NSString *)CFSTR("NSURLAuthenticationMethodOAuth");
__attribute__((used)) extern const CFStringRef kCFStreamSocketSecurityLevelTLSv1_2 = CFSTR("kCFStreamSocketSecurityLevelTLSv1_2");
__attribute__((used)) extern const CFStringRef kCFURLRequestContentDecoderSkipURLCheck = CFSTR("kCFURLRequestContentDecoderSkipURLCheck");

// ---- CoreUI constant ----
__attribute__((used)) extern const CFStringRef kCUIWidgetScrollBarTrackCorner = CFSTR("kCUIWidgetScrollBarTrackCorner");

// ---- CoreGraphics function stubs (newer CG APIs) ----
void CGContextDrawConicGradient(CGContextRef, CGGradientRef, CGPoint, CGFloat) {}
void CGContextDrawPathDirect(CGContextRef c, CGPathDrawingMode mode, CGPathRef, const void*) { if (c) CGContextDrawPath(c, mode); }
bool CGFontRenderingGetFontSmoothingDisabled(void) { return false; }
void CGIOSurfaceContextSetDisplayMask(CGContextRef, uint32_t) {}
void CGPathAddUnevenCornersRoundedRect(void) {}

// ---- CoreText function stubs ----
CFDataRef CTFontCopyGlyphCoverageForFeature(CTFontRef, CFDictionaryRef) { return NULL; }
CTFontRef CTFontCopyPhysicalFont(CTFontRef f) { return f ? (CTFontRef)CFRetain(f) : NULL; }
CTFontDescriptorRef CTFontDescriptorCreateForCSSFamily(CFStringRef, CFStringRef) { return NULL; }
CTFontDescriptorRef CTFontDescriptorCreateLastResort(void) { return NULL; }
bool CTFontGetUnsummedAdvancesForGlyphsAndStyle(CTFontRef, const void*, const void*, void*, CFIndex) { return false; }
bool CTFontIsAppleColorEmoji(CTFontRef) { return false; }
extern "C" {
size_t WK_woff2_ComputeFinalSize(const uint8_t* data, size_t length);
bool WK_woff2_ConvertToTTF(uint8_t* result, size_t result_length, const uint8_t* data, size_t length);
}

CFArrayRef CTFontManagerCreateFontDescriptorsFromData(CFDataRef data)
{
    if (!data)
        return NULL;
    CFDataRef sfntData = data;
    CFDataRef decoded = NULL;
    const UInt8* bytes = CFDataGetBytePtr(data);
    CFIndex len = CFDataGetLength(data);
    if (len >= 4 && bytes[0] == 0x77 && bytes[1] == 0x4F && bytes[2] == 0x46 && bytes[3] == 0x32) {
        size_t sfntSize = WK_woff2_ComputeFinalSize(bytes, len);
        if (!sfntSize)
            return NULL;
        UInt8* out = (UInt8*)malloc(sfntSize);
        if (!out)
            return NULL;
        if (!WK_woff2_ConvertToTTF(out, sfntSize, bytes, len)) {
            free(out);
            return NULL;
        }
        decoded = CFDataCreate(kCFAllocatorDefault, out, sfntSize);
        free(out);
        if (!decoded)
            return NULL;
        sfntData = decoded;
    }
    CGDataProviderRef provider = CGDataProviderCreateWithCFData(sfntData);
    if (!provider) {
        if (decoded) CFRelease(decoded);
        return NULL;
    }
    CGFontRef cgFont = CGFontCreateWithDataProvider(provider);
    CGDataProviderRelease(provider);
    if (decoded) CFRelease(decoded);
    if (!cgFont)
        return NULL;
    CTFontRef ctFont = CTFontCreateWithGraphicsFont(cgFont, 12.0, NULL, NULL);
    CGFontRelease(cgFont);
    if (!ctFont)
        return NULL;
    CTFontDescriptorRef descriptor = CTFontCopyFontDescriptor(ctFont);
    CFRelease(ctFont);
    if (!descriptor)
        return NULL;
    const void* values[1] = { descriptor };
    CFArrayRef array = CFArrayCreate(kCFAllocatorDefault, values, 1, &kCFTypeArrayCallBacks);
    CFRelease(descriptor);
    return array;
}
void CTParagraphStyleSetCompositionLanguage(void*, CFStringRef) {}

// ---- DataDetectors / IOKit-PM stubs ----
CFTypeID DDResultGetCFTypeID(void) { return 0; }
uint32_t IOPMAssertionCreateWithDescription(CFStringRef, CFStringRef, CFStringRef, CFStringRef, CFStringRef, double, CFStringRef, uint32_t*) { return 0; }

// ---- sqlite3 (10.6 sqlite is older; provide newer entry points) ----
const char* sqlite3_errstr(int) { return "SQLite error"; }
int sqlite3_wal_checkpoint_v2(void*, const char*, int, int*, int*) { return 0; }
void* sqlite3_wal_hook(void*, int(*)(void*,void*,const char*,int), void*) { return NULL; }

} // extern "C"
