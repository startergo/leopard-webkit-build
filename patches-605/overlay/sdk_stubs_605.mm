/*
 * sdk_stubs_605.mm — Missing symbols for MacOSX 10.6 SDK (WebKit 605, ObjC++ linkage).
 *
 * Differs from the 604 sdk_stubs.mm:
 *   - NSScrollerImp / NSScrollerImpPair are 10.7+ overlay-scrollbar classes declared
 *     in 605's pal/spi/mac/NSScrollerImpSPI.h (full @interface, NO @implementation).
 *     dyld hard-relocates the _OBJC_CLASS_$_ refs (they do NOT no-op to nil under
 *     dynamic_lookup), and ScrollAnimatorMac/ScrollbarThemeMac unconditionally
 *     instantiate + message them — so on 10.6 we must PROVIDE no-op @implementation
 *     stubs (see NSSCROLLERIMP STUBS below) covering every selector WebCore sends.
 *   - _NSRecommendedScrollerStyle is a FUNCTION (per NSScrollerImpSPI.h:154), not a
 *     variable.  Returning NSScrollerStyleLegacy forces the legacy scroller path.
 *   - Adds _CFAppVersionCheckLessThan (used by WebView.mm app-quirk logic).
 *   - Adds NSWindowWillOrder{On,Off}ScreenNotification (used by WebView.mm window-order
 *     observers); link-first from AppKit is not possible against the 10.6 SDK, so the
 *     constant string literals are provided.
 */

// XPC typedefs — this TU is compiled without -include TargetConditionals_compat.h.
typedef struct _xpc_connection_s *xpc_connection_t;
typedef void *xpc_object_t;

#import <Foundation/Foundation.h>
#import <Foundation/NSURLSession.h>
#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <CoreText/CoreText.h>
#include <dispatch/dispatch.h>
#include <pthread.h>
#include <objc/objc.h>
#include <objc/runtime.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <mach/mach_types.h>
#include <math.h>

/* vm_kernel_page_* — 10.9+ runtime symbols referenced by bmalloc's
 * VMAllocate.h:96 (pageSize()). The 10.6 kernel has no such symbols, so define
 * them here with the x86_64 kernel page size (4096). vm_page_size itself is
 * provided by the 10.6 kernel via mach_init; only the kernel variants are
 * missing. See overlay shim mach/vm_page_size.h for the declarations. */
extern "C" {
vm_size_t vm_kernel_page_size = 4096;
vm_size_t vm_kernel_page_mask = 0xFFF;
int vm_kernel_page_shift = 12;

/* wkGetHyphenationLocationBeforeIndex — 10.6 has no public CFString hyphenation API
 * (CFStringIsHyphenationAvailableForLocale / CFStringGetHyphenationLocationBeforeIndex
 * are 10.7+).  The 10.6-era private SPI _CFStringGetHyphenationLocationBeforeIndex
 * (2-arg) provided English hyphenation; resolve it lazily via dlsym so the build links
 * against the 10.6 SDK and degrades to "no hyphenation" (kCFNotFound) if the SPI is
 * absent.  Called by WebCore/platform/text/cf/HyphenationCF.cpp on the 10.6 path. */
CFIndex wkGetHyphenationLocationBeforeIndex(CFStringRef string, CFIndex location)
{
    typedef CFIndex (*HyphenationSPI)(CFStringRef, CFIndex);
    static HyphenationSPI spi = [] {
        return reinterpret_cast<HyphenationSPI>(dlsym(RTLD_DEFAULT, "_CFStringGetHyphenationLocationBeforeIndex"));
    }();
    if (spi)
        return spi(string, location);
    return kCFNotFound;
}

/* CFStringIsHyphenationAvailableForLocale + CFStringGetHyphenationLocationBeforeIndex —
 * the PUBLIC 10.7+ CFString hyphenation API. HyphenationCF.cpp calls them by their
 * public names. On 10.6 there is no locale-aware availability check (the private
 * SPI doesn't expose one), so isHyphenationAvailable returns false → canHyphenate()
 * yields false and getHyphenationLocationBeforeIndex is never reached. Provide both
 * so the TU links; getHyphenationLocationBeforeIndex delegates to the SPI wrapper
 * above for a best-effort result if canHyphenate is bypassed. */
#if __MAC_OS_X_VERSION_MAX_ALLOWED < 1070
extern "C" Boolean CFStringIsHyphenationAvailableForLocale(CFLocaleRef locale) {
    (void)locale;
    return false;
}
extern "C" CFIndex CFStringGetHyphenationLocationBeforeIndex(CFStringRef string, CFIndex location, CFRange limitRange, CFOptionFlags options, CFLocaleRef locale, char *hyphenCharacters) {
    (void)limitRange; (void)options; (void)locale; (void)hyphenCharacters;
    return wkGetHyphenationLocationBeforeIndex(string, location);
}
#endif
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED < 1070
@implementation NSFileManager (WebKit10_7Compat)
- (BOOL)createDirectoryAtURL:(NSURL *)url withIntermediateDirectories:(BOOL)createIntermediates attributes:(NSDictionary *)attributes error:(NSError **)error {
    return [self createDirectoryAtPath:[url path] withIntermediateDirectories:createIntermediates attributes:attributes error:error];
}
@end
#endif

// NSNotificationCenter — postNotificationOnMainThread (10.7+). Forward to sync 10.6 API.
@interface NSNotificationCenter (SLCompat106)
- (void)postNotificationOnMainThreadWithName:(NSString *)name object:(id)object userInfo:(NSDictionary *)userInfo;
- (void)postNotificationOnMainThreadWithName:(NSString *)name object:(id)object;
@end
@implementation NSNotificationCenter (SLCompat106)
- (void)postNotificationOnMainThreadWithName:(NSString *)name object:(id)object userInfo:(NSDictionary *)userInfo {
    [self postNotificationName:name object:object userInfo:userInfo];
}
- (void)postNotificationOnMainThreadWithName:(NSString *)name object:(id)object {
    [self postNotificationName:name object:object];
}
@end

// NSURLConnection -setDelegateQueue: (10.7+).  WebCore's ResourceHandleMac.mm calls it
// (lines ~272, ~400) to direct delegate callbacks to an NSOperationQueue.  The
// connection is created with the private _initWithRequest:...startImmediately:NO...
// initializer, so it is NOT auto-scheduled.  On 10.7+, setDelegateQueue: supplies the
// dispatch target; on 10.6 there is no operation-queue model, so route to the classic
// run-loop scheduling the connection needs, otherwise URLConnectionClient::start() ->
// RunLoopMultiplexer::schedule() calls CFSetApplyFunction(NULL) and faults (SIGSEGV).
// Scheduling the connection here (before -start) is its sole schedule on the 10.6 path.
@interface NSURLConnection (SLCompat106)
- (void)setDelegateQueue:(NSOperationQueue *)queue;
- (NSURLRequest *)currentRequest;
- (NSDictionary *)_timingData;
@end
@implementation NSURLConnection (SLCompat106)
- (void)setDelegateQueue:(NSOperationQueue *)queue {
    (void)queue;
    [self scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
}
// -currentRequest (10.7+).  10.6 NSURLConnection exposes neither -currentRequest nor
// -originalRequest, so this must NOT delegate to originalRequest (that faults at runtime:
// -[NSURLConnection originalRequest]: unrecognized selector).  The WK1 redirect delegate
// (WebCoreResourceHandleAsOperationQueueDelegate.mm) gates on MIN_REQUIRED<1070 and passes
// newRequest directly, so it never reaches this category; returning nil here is a safe
// fallback for any other caller (nil-messaging is graceful and the scheme compare in
// synthesizeRedirectResponseIfNecessary simply yields "schemes differ").
- (NSURLRequest *)currentRequest { return nil; }
// -_timingData (SPI, 10.8+).  Read by ResourceHandleMac.mm:746 in
// getConnectionTimingData() during didReceiveResponse for NetworkLoadMetrics.
// copyTimingData() (NetworkLoadMetrics.mm) early-returns on nil, so no timing data is
// recorded on 10.6 — page loading is unaffected (timing is diagnostic only).
- (NSDictionary *)_timingData { return nil; }
@end

// NSThread -initWithBlock: (10.10+).  WebCore's webCoreBackgroundNetworkingRunLoop()
// (ResourceHandleMac.mm:~88) allocates the background networking thread with it.  On
// 10.6 NSThread has no block-based init, so map onto the classic
// -initWithTarget:selector:object: model: store a copy of the block as the argument
// object and invoke it from a helper selector that serves as the thread body.  Returns
// an initialized (not-yet-started) thread, matching the 10.10 contract (the caller
// sends -start).  A self-target is harmless: the thread retains its target until exit,
// and this networking thread is intentionally permanent.
@interface NSThread (SLCompat106)
- (instancetype)initWithBlock:(void (^)(void))block;
- (void)_sl106_runBlock:(id)blockObject;
@end
@implementation NSThread (SLCompat106)
- (instancetype)initWithBlock:(void (^)(void))block {
    return [self initWithTarget:self selector:@selector(_sl106_runBlock:) object:[block copy]];
}
- (void)_sl106_runBlock:(id)blockObject {
    void (^block)(void) = (void (^)(void))blockObject;
    if (block) block();
}
@end

// Safari 5.0.5 ↔ 605 WebView SPI bridge.  Safari calls private SPIs on its BrowserWebView
// (a WebView subclass) that the 605 branch removed/renamed; stub the removed ones as no-ops
// so Safari launches against our 605 WebKit.  Uses a +load hook (NOT a category) because
// sdk_stubs_605.o is force-linked into ALL three frameworks (JSC, WebCore, WebKitLegacy);
// a category's @implementation would emit a _OBJC_CLASS_$_WebView link-time reference
// in JSC, which has no WebView class and fails dyld load ("Symbol not found:
// _OBJC_CLASS_$_WebView"). The +load hook uses objc_getClass (runtime lookup) which
// returns NULL in JSC/WebCore (no WebView there) and the real class in WebKitLegacy,
// where it adds the method via class_addMethod — exactly the proven WebIconDatabase
// injector pattern at the bottom of this file.
@interface WebViewSPIInjector : NSObject
@end

/* Plain C IMP for _setJavaScriptURLsAreAllowed: — void return, no-op.
 * Signature "v@:c" = void(id, SEL, BOOL) where BOOL is signed char on 10.6. */
static void webViewSPI_setJavaScriptURLsAreAllowed(id self, SEL _cmd, BOOL flag) {
    (void)self; (void)_cmd; (void)flag;
}

@implementation WebViewSPIInjector
+ (void)load {
    Class webView = objc_getClass("WebView");
    if (!webView)
        return; /* not present in this image (e.g. JSC/WebCore copies) */
    SEL sel = sel_registerName("_setJavaScriptURLsAreAllowed:");
    if (class_getInstanceMethod(webView, sel))
        return; /* already implemented — don't clobber a real future impl */
    class_addMethod(webView, sel, (IMP)webViewSPI_setJavaScriptURLsAreAllowed, "v@:c");
}
@end

// NSFont +systemFontOfSize:weight: (10.8+, SPI per pal/spi/mac/NSFontSPI.h:38).  WebCore's
// FontCacheMac.mm:80 system-font lookup (-apple-system / system-ui) calls the weighted
// variant; 10.6 NSFont only has the unweighted +systemFontOfSize:.  Delegate to it and drop
// the weight (bold system-font text renders regular — the only loss on 10.6).
@interface NSFont (SLCompat106)
+ (NSFont *)systemFontOfSize:(CGFloat)size weight:(CGFloat)weight;
@end
@implementation NSFont (SLCompat106)
+ (NSFont *)systemFontOfSize:(CGFloat)size weight:(CGFloat)weight {
    (void)weight;
    return [self systemFontOfSize:size];
}
@end

// NSColor -CGColor — the CGColor property on NSColor is 10.8+. WebCore callers
// (GraphicsContextCocoa.mm, TextIndicatorWindow.mm) use [nsColor CGColor] to get a
// CGColorRef. Provide a WORKING 10.6 bridge: convert to device RGB, extract the
// four components, build a CGColorRef via CGColorCreateGenericRGB (10.5+). Returns
// a +1 reference (caller-owned), matching the 10.8+ -CGColor contract.
@interface NSColor (SLCompat106CGColor)
- (CGColorRef)CGColor;
@end
@implementation NSColor (SLCompat106CGColor)
- (CGColorRef)CGColor
{
    NSColor *rgb = [self colorUsingColorSpaceName:NSDeviceRGBColorSpace];
    if (!rgb)
        rgb = self;
    CGFloat r, g, b, a;
    if (![rgb respondsToSelector:@selector(getRed:green:blue:alpha:)]) {
        // Non-RGB color space that can't convert; return opaque black.
        return CGColorCreateGenericRGB(0, 0, 0, 1);
    }
    [rgb getRed:&r green:&g blue:&b alpha:&a];
    return CGColorCreateGenericRGB(r, g, b, a);
}
@end

extern "C" {

CFTypeRef _CFXPCCreateCFObjectFromXPCMessage(void *xpc_message) { (void)xpc_message; return NULL; }
CFTypeRef _CFXPCCreateXPCMessageWithCFObject(CFTypeRef cf) { (void)cf; return NULL; }

/* XPC type globals — names must match TargetConditionals_compat.h declarations */
static int xpc_type_dictionary_val = 0;
static int xpc_type_error_val = 0;
static int xpc_error_connection_invalid_val = 0;
void *XPC_TYPE_DICTIONARY = &xpc_type_dictionary_val;
void *XPC_TYPE_ERROR = &xpc_type_error_val;
void *XPC_ERROR_CONNECTION_INVALID = &xpc_error_connection_invalid_val;

xpc_object_t xpc_dictionary_create(const char * const *keys, const xpc_object_t *values, size_t count) { (void)keys; (void)values; (void)count; return NULL; }
xpc_object_t xpc_dictionary_get_value(xpc_object_t xdict, const char *key) { (void)xdict; (void)key; return NULL; }
void xpc_dictionary_set_value(xpc_object_t xdict, const char *key, xpc_object_t value) { (void)xdict; (void)key; (void)value; }
xpc_object_t xpc_get_type(xpc_object_t obj) { (void)obj; return NULL; }
xpc_object_t xpc_retain(xpc_object_t obj) { return obj; }
void xpc_release(xpc_object_t obj) { (void)obj; }
xpc_connection_t xpc_connection_create_mach_service(const char *name, dispatch_queue_t targetq, uint64_t flags) { (void)name; (void)targetq; (void)flags; return NULL; }
void xpc_connection_set_event_handler(xpc_connection_t conn, void (^handler)(xpc_object_t)) { (void)conn; (void)handler; }
void xpc_connection_resume(xpc_connection_t conn) { (void)conn; }
void xpc_connection_cancel(xpc_connection_t conn) { (void)conn; }
void xpc_connection_send_message(xpc_connection_t conn, xpc_object_t msg) { (void)conn; (void)msg; }
void xpc_connection_send_message_with_reply(xpc_connection_t conn, xpc_object_t msg, dispatch_queue_t queue, void (^handler)(xpc_object_t)) {
    (void)conn; (void)msg; (void)queue; if (handler) handler(NULL);
}
xpc_connection_t xpc_connection_create(const char *name, dispatch_queue_t targetq) { (void)name; (void)targetq; return NULL; }
void xpc_connection_set_bootstrap(xpc_connection_t conn, xpc_object_t bootstrap) { (void)conn; (void)bootstrap; }
void xpc_connection_set_oneshot_instance(xpc_connection_t conn, const unsigned char inst[16]) { (void)conn; (void)inst; }
pid_t xpc_connection_get_pid(xpc_connection_t conn) { (void)conn; return 0; }
void xpc_connection_get_audit_token(xpc_connection_t conn, audit_token_t *token) { (void)conn; (void)token; }
void xpc_connection_kill(xpc_connection_t conn, int signo) { (void)conn; (void)signo; }
xpc_object_t xpc_array_create(const xpc_object_t *objects, size_t count) { (void)objects; (void)count; return NULL; }
void xpc_array_set_string(xpc_object_t arr, size_t idx, const char *str) { (void)arr; (void)idx; (void)str; }
void xpc_dictionary_set_string(xpc_object_t dict, const char *key, const char *str) { (void)dict; (void)key; (void)str; }
void xpc_dictionary_set_fd(xpc_object_t dict, const char *key, int fd) { (void)dict; (void)key; (void)fd; }
void xpc_dictionary_set_mach_send(xpc_object_t dict, const char *key, mach_port_t port) { (void)dict; (void)key; (void)port; }

} /* extern "C" */

extern "C" {
#include <sandbox.h>
int sandbox_check(pid_t pid, const char *operation, enum sandbox_filter_type type, ...) { (void)pid; (void)operation; (void)type; return 0; }
const char *APP_SANDBOX_READ = "com.apple.app-sandbox.read";
const char *APP_SANDBOX_READ_WRITE = "com.apple.app-sandbox.read-write";
int sandbox_check_by_audit_token(audit_token_t token, const char *operation, enum sandbox_filter_type type, ...) { (void)token; (void)operation; (void)type; return 0; }
int sandbox_container_path_for_pid(pid_t pid, char *buffer, size_t bufsize) { (void)pid; (void)buffer; (void)bufsize; return -1; }
int sandbox_init_with_parameters(const char *profile, uint64_t flags, const char *const parameters[], char **errorbuf) { (void)profile; (void)flags; (void)parameters; if (errorbuf) *errorbuf = NULL; return 0; }
int sandbox_extension_consume(const char *extension_token) { (void)extension_token; return -1; }
char *sandbox_extension_issue_file(const char *extension_class, const char *path, int flags, int *error) { (void)extension_class; (void)path; (void)flags; if (error) *error = 0; return NULL; }
char *sandbox_extension_issue_generic(const char *extension_class, int flags, int *error) { (void)extension_class; (void)flags; if (error) *error = 0; return NULL; }
void sandbox_extension_release(char *extension_token) { (void)extension_token; }
} /* extern "C" */

/* dispatch_data / dispatch_io stubs (10.7+ APIs, not in 10.6 libdispatch) */
extern "C" {
#ifndef dispatch_data_t
typedef dispatch_object_t dispatch_data_t;
#endif
#ifndef dispatch_io_t
typedef dispatch_object_t dispatch_io_t;
#endif
typedef int dispatch_fd_t;
typedef void (^dispatch_io_handler_t)(bool done, dispatch_data_t data, int error);
void __dispatch_data_destructor_noop(void) {}
extern const dispatch_block_t DISPATCH_DATA_DESTRUCTOR_DEFAULT;
const dispatch_block_t DISPATCH_DATA_DESTRUCTOR_DEFAULT __attribute__((used, visibility("default"))) = (dispatch_block_t)__dispatch_data_destructor_noop;
dispatch_data_t dispatch_data_empty_val = NULL;
dispatch_data_t dispatch_data_empty = NULL;
dispatch_data_t dispatch_data_create(const void *buffer, size_t size, dispatch_queue_t queue, dispatch_block_t destructor) { (void)buffer; (void)size; (void)queue; (void)destructor; return NULL; }
dispatch_data_t dispatch_data_create_map(dispatch_data_t data, const void **buffer_ptr, size_t *size_ptr) { (void)data; if (buffer_ptr) *buffer_ptr = NULL; if (size_ptr) *size_ptr = 0; return NULL; }
bool dispatch_data_apply(dispatch_data_t data, bool (^applier)(dispatch_data_t, size_t, const void*, size_t)) { (void)data; (void)applier; return false; }
dispatch_data_t dispatch_data_create_subrange(dispatch_data_t data, size_t offset, size_t size) { (void)data; (void)offset; (void)size; return NULL; }
dispatch_data_t dispatch_data_create_concat(dispatch_data_t a, dispatch_data_t b) { (void)a; (void)b; return NULL; }
size_t dispatch_data_get_size(dispatch_data_t data) { (void)data; return 0; }
dispatch_io_t dispatch_io_create(int type, dispatch_fd_t fd, dispatch_queue_t queue, void (^cleanup_handler)(int error)) { (void)type; (void)fd; (void)queue; (void)cleanup_handler; return NULL; }
void dispatch_io_set_low_water(dispatch_io_t channel, size_t low_water) { (void)channel; (void)low_water; }
void dispatch_io_read(dispatch_io_t channel, off_t offset, size_t length, dispatch_queue_t queue, dispatch_io_handler_t io_handler) { (void)channel; (void)offset; (void)length; (void)queue; if (io_handler) io_handler(true, NULL, 0); }
void dispatch_io_write(dispatch_io_t channel, off_t offset, dispatch_data_t data, dispatch_queue_t queue, dispatch_io_handler_t io_handler) { (void)channel; (void)offset; (void)data; (void)queue; if (io_handler) io_handler(true, NULL, 0); }
} /* extern "C" */

/* Private AppKit symbols (10.7+) not in 10.6 */
extern "C" {
const char* NSPopUpMenuPopupButtonWidget = "popup";
float NSPopUpMenuPopupButtonBounds[4] = {0, 0, 100, 24};
float NSPopUpMenuPopupButtonLabelOffset = 0;
float NSPopUpMenuPopupButtonSize[2] = {100, 24};
float _NSElasticDeltaForTimeDelta(float axis, float delta, float velocity, float size) { (void)axis; (void)velocity; (void)size; return delta; }
float _NSElasticDeltaForReboundDelta(float delta) { return delta; }
float _NSReboundDeltaForElasticDelta(float delta) { return delta; }
void NSInitializeCGFocusRingStyleForTime(int *style, float time, int *color, int *enabled) { (void)time; if (style) *style = 0; if (enabled) *enabled = 0; }
} /* extern "C" */

/* NSAccessibilityPostNotificationWithUserInfo — public accessibility C function
 * (10.9+) referenced by AXObjectCacheMac.mm:258 via the AXPostNotificationWithUserInfo
 * helper. Declared in pal/spi/mac/NSAccessibilitySPI.h. The 10.6 AppKit only exports
 * the 2-arg NSAccessibilityPostNotification(id, NSString*) — no userInfo support —
 * so bridge to that and drop userInfo (10.6 AX clients did not consume it). The 2-arg
 * function is a public API present since 10.4, available via the AppKit import above. */
extern "C" void NSAccessibilityPostNotificationWithUserInfo(id element, NSString *notification, id userInfo)
{
    (void)userInfo;
    NSAccessibilityPostNotification(element, notification);
}

#if 0
/* WebCore::RenderThemeMac::purgeCaches() — DISABLED: this stub was needed when WebCore
 * compiled at -O3 (which eliminated the final virtual override). Now that the build
 * uses -O2 for all TUs, the real definition is emitted normally and this stub would
 * cause a duplicate-symbol error. Kept (commented via #if 0) for documentation. */
asm(
    ".text\n"
    ".globl __ZN7WebCore14RenderThemeMac11purgeCachesEv\n"
    ".p2align 4, 0x90\n"
    "__ZN7WebCore14RenderThemeMac11purgeCachesEv:\n"
    "    ret\n"
);
#endif

#if __MAC_OS_X_VERSION_MAX_ALLOWED < 1090
/* CGPathCreateWithRect — CoreGraphics convenience (10.9+). DragImageMac.mm uses it
 * to build a rect path for a CTFrame. Build via CGPathCreateMutable+CGPathAddRect. */
extern "C" CGPathRef CGPathCreateWithRect(CGRect rect, const CGAffineTransform* transform) {
    CGMutablePathRef path = CGPathCreateMutable();
    CGPathAddRect(path, transform, rect);
    return path;
}
#endif

#if __MAC_OS_X_VERSION_MAX_ALLOWED < 1080
/* CFAutorelease — CoreFoundation (10.8+). Adds a +0 CF object to the current
 * autorelease pool and returns it. RenderThemeMac.mm uses it on a short-lived
 * color/dictionary during the paint pass. On 10.6 there is no CF-level autorelease;
 * return the object as-is (the caller leaks — acceptable for short-lived paint
 * objects; the leak is bounded by the number of paints, not unbounded). */
extern "C" CFTypeRef CFAutorelease(CFTypeRef cf) {
    return cf;
}
#endif

#if __MAC_OS_X_VERSION_MAX_ALLOWED < 1080
/* CTLineGetBoundsWithOptions — CoreText (10.8+). DragImageMac.mm uses it to get the
 * image bounds of a CTLine. Fall back to typographic bounds (ascent/descent/width). */
typedef unsigned int CTLineBoundsOptions;
extern "C" CGRect CTLineGetBoundsWithOptions(CTLineRef line, CTLineBoundsOptions options) {
    (void)options;
    CGFloat ascent = 0, descent = 0, leading = 0;
    double width = CTLineGetTypographicBounds(line, &ascent, &descent, &leading);
    return CGRectMake(0, -descent, width, ascent + descent);
}
#endif

#if __MAC_OS_X_VERSION_MAX_ALLOWED < 1070
/* NSFileCoordinator — Foundation coordinated file access (10.7+).
 * BlobDataFileReferenceMac.mm creates one to coordinate reading a blob source.
 * Provide a minimal stub: init returns self; coordinateReadingItemAtURL: invokes
 * the accessor block synchronously with the original URL (no coordination on 10.6,
 * which has no file-presenters / iCloud document sync — safe for single-process
 * WebKit blob reads). */
@interface NSFileCoordinator : NSObject
@end
@implementation NSFileCoordinator
- (instancetype)initWithFilePresenter:(id)presenter { (void)presenter; return [super init]; }
- (void)coordinateReadingItemAtURL:(NSURL *)url options:(unsigned long)options error:(NSError **)error byAccessor:(void (^)(NSURL *))accessor {
    (void)options; if (error) *error = nil; if (accessor) accessor(url);
}
@end
#endif

#if __MAC_OS_X_VERSION_MAX_ALLOWED < 1070
/* IONotificationPortSetDispatchQueue — IOKit (10.6 runtime symbol but not in the
 * 10.6 SDK headers on some toolchains). PowerObserverMac.cpp uses it to bind the
 * power-state notification port to a dispatch queue. Provide a no-op stub; the
 * power-state change notifications are delivered via the run loop instead. */
typedef struct IONotificationPort *IONotificationPortRef;
extern "C" void IONotificationPortSetDispatchQueue(IONotificationPortRef notify, dispatch_queue_t queue) {
    (void)notify; (void)queue;
}
#endif

#if __MAC_OS_X_VERSION_MAX_ALLOWED < 1090
/* CGPathAddRoundedRect — CoreGraphics convenience (10.9+). PathCG.cpp uses it to
 * append a rounded-rect subpath. On 10.6 there is no single-call equivalent short
 * of building four arcs; degrade to a plain rect (rounded corners lost — cosmetic
 * only, no functional impact on layout/hit-testing). The forward declaration lives
 * in PathCG.cpp (guarded < 1090). */
extern "C" void CGPathAddRoundedRect(CGMutablePathRef path, const CGAffineTransform* transform, CGRect rect, CGFloat cornerWidth, CGFloat cornerHeight) {
    (void)cornerWidth; (void)cornerHeight;
    CGPathAddRect(path, transform, rect);
}
#endif

#if __MAC_OS_X_VERSION_MAX_ALLOWED < 1070
/* AudioObject{Add,Remove}PropertyListenerBlock — block-based CoreAudio property
 * listener API (10.7+). Referenced by AudioHardwareListenerMac.{h,cpp}. On 10.6
 * the block variant doesn't exist; provide no-op stubs that return noErr and drop
 * the block. Audio-hardware change detection (headphone plug/unplug, output device
 * switch) is inert on 10.6 — Safari runs normally, just doesn't get live callbacks. */
#include <CoreAudio/CoreAudio.h>
typedef void (^AudioObjectPropertyListenerBlock)(UInt32, const AudioObjectPropertyAddress[]);
extern "C" OSStatus AudioObjectAddPropertyListenerBlock(AudioObjectID inObjectID, const AudioObjectPropertyAddress* inAddress, dispatch_queue_t inDispatchQueue, AudioObjectPropertyListenerBlock inListenerBlock) {
    (void)inObjectID; (void)inAddress; (void)inDispatchQueue; (void)inListenerBlock;
    return noErr;
}
extern "C" OSStatus AudioObjectRemovePropertyListenerBlock(AudioObjectID inObjectID, const AudioObjectPropertyAddress* inAddress, dispatch_queue_t inDispatchQueue, AudioObjectPropertyListenerBlock inListenerBlock) {
    (void)inObjectID; (void)inAddress; (void)inDispatchQueue; (void)inListenerBlock;
    return noErr;
}
#endif

/* Missing ObjC classes (10.7+) */
@interface CASpringAnimation : CABasicAnimation
@property CGFloat mass;
@property CGFloat stiffness;
@property CGFloat damping;
@property CGFloat initialVelocity;
@property CGFloat velocity;
@end
@implementation CASpringAnimation
@synthesize mass, stiffness, damping, initialVelocity, velocity;
@end
@interface NSLayoutConstraint : NSObject @end
@implementation NSLayoutConstraint @end
@interface NSPopoverColorWell : NSObject @end
@implementation NSPopoverColorWell @end

/* Missing C symbols (10.7+) */
extern "C" {
void CABackingStoreCollectBlocking(void) {}
const void* kCAContextCIFilterBehavior = NULL;
const void* kCAContextPortNumber = NULL;
int CGContextDrawsWithCorrectShadowOffsets = 0;
void CGSPackagesEnableConnectionOcclusionNotifications(int a, int b) { (void)a; (void)b; }
void CGSPackagesEnableConnectionWindowModificationNotifications(int a, int b) { (void)a; (void)b; }
} /* extern "C" */

/* QuartzCore SPI constants (10.7+) */
extern "C" {
/* kCAFilterColorInvert / kCAFilterGaussianBlur (and the full blend-mode set) are
   now defined as NSString* const further below in the 10.7+ SPI batch — they MUST
   be real NSStrings (WebKit SPI declares them `extern NSString * const`). */
const void* kCFWebServicesProviderDefaultDisplayNameKey = NULL;
const void* kCFWebServicesTypeWebSearch = NULL;
__attribute__((used)) void* kCFStreamPropertyCONNECTAdditionalHeaders = 0;
__attribute__((used)) void* kCFStreamPropertyCONNECTProxy = 0;
__attribute__((used)) void* kCFStreamPropertyCONNECTProxyHost = 0;
__attribute__((used)) void* kCFStreamPropertyCONNECTProxyPort = 0;
__attribute__((used)) void* kCFStreamPropertyCONNECTResponse = 0;
__attribute__((used)) void* kMDItemDownloadedDate = 0;
} /* extern "C" */

struct _xpc_connection_s;
struct dispatch_queue_s;
extern "C" {
void xpc_connection_set_target_queue(struct _xpc_connection_s *conn, struct dispatch_queue_s *queue) { (void)conn; (void)queue; }
}

extern "C" {
id objc_initWeak(id *addr, id val) { *addr = val; return val; }
void objc_destroyWeak(id *addr) { *addr = nil; }
}

extern "C" {
int _Block_has_signature(void *block) { (void)block; return 0; }
const char *_Block_signature(void *block) { (void)block; return NULL; }
}

extern "C" {
const char *_protocol_getMethodTypeEncoding(Protocol *proto, SEL sel, BOOL isRequiredMethod, BOOL isInstanceMethod) {
    (void)proto; (void)sel; (void)isRequiredMethod; (void)isInstanceMethod; return NULL;
}
}

extern "C" {
uintptr_t g_globalDataPoison = 0;
uintptr_t g_jitCodePoison = 0;
uintptr_t g_nativeCodePoison = 0;
}
// Note: JSC::initializePoison() and the C++-mangled poison globals are defined
// by JSC's own runtime/JSCPoison.cpp (UnifiedSource105); do not stub them here.

// CABackdropLayer is 10.10+. Minimal ObjC class stub.
@interface CABackdropLayer : CALayer @end
@implementation CABackdropLayer @end

/* WebKitLegacy helper classes referenced by legacy plugin/inspector code. */
@interface WebHostedNetscapePluginView : NSObject @end
@implementation WebHostedNetscapePluginView @end
@interface WebKeyGenerator : NSObject @end
@implementation WebKeyGenerator @end
@interface WebRenderNode : NSObject @end
@implementation WebRenderNode @end
@interface WebSerializedJSValue : NSObject @end
@implementation WebSerializedJSValue @end

/* C++ mangled sandbox_check — called from JSC code that did not see extern "C" */
extern "C" void __sandbox_check_cpp_stub(void) {}
asm(".globl __Z13sandbox_checkiPKc19sandbox_filter_typez");
asm(".set __Z13sandbox_checkiPKc19sandbox_filter_typez, ___sandbox_check_cpp_stub");

/* NSURLSession + friends — the 10.9+ Foundation networking classes. With
 * ENABLE(VIDEO)=1, WebCoreNSURLSession.mm compiles and references
 * _OBJC_CLASS_$_NSURLSession (a dyld hard-relocation). On 10.6 these classes
 * don't exist; provide empty @implementations so the class refs resolve.
 * The methods are no-ops (return nil) — on 10.6 there's no NSURLSession-based
 * networking; the classic NSURLConnection path is used instead. */
@interface NSURLSessionConfiguration () @end
@implementation NSURLSessionConfiguration
+ (instancetype)defaultSessionConfiguration { return [[self alloc] init]; }
+ (instancetype)ephemeralSessionConfiguration { return [[self alloc] init]; }
- (id)copyWithZone:(NSZone *)zone { return [self retain]; }
@end

@interface NSURLSession () @end
@implementation NSURLSession
+ (NSURLSession *)sessionWithConfiguration:(NSURLSessionConfiguration *)configuration { (void)configuration; return nil; }
+ (NSURLSession *)sharedSession { return nil; }
@end

@interface NSURLSessionTask () @end
@implementation NSURLSessionTask @end
@interface NSURLSessionDataTask () @end
@implementation NSURLSessionDataTask @end
@interface NSURLSessionUploadTask () @end
@implementation NSURLSessionUploadTask @end
@interface NSURLSessionDownloadTask () @end
@implementation NSURLSessionDownloadTask @end
@interface NSURLSessionStreamTask () @end
@implementation NSURLSessionStreamTask @end

/* ====================================================================== */
/*  605-specific PAL SPI symbols (see webkit-605-migration-audit-v2 §11)   */
/* ====================================================================== */

/* NSScrollerStyle — AppKit enum introduced in 10.7; absent from the 10.6 SDK.
   NSScrollerImpSPI.h uses it as a property/return type.  The overlay header
   (Overlay605Supplement.h, force-included into WebKit TUs) provides the same
   typedef; this local definition makes sdk_stubs_605.mm self-contained. */
#ifndef __NSSCROLLERSTYLE_605_DEFINED__
#define __NSSCROLLERSTYLE_605_DEFINED__
typedef NSInteger NSScrollerStyle;
enum {
    NSScrollerStyleLegacy_605 = 0,
    NSScrollerStyleOverlay_605 = 1
};
#endif

/* NSScrollerKnobStyle — AppKit enum (Default/Dark/Light), 10.7+; absent from the
   10.6 SDK.  NSScrollerImpSPI.h declares NSScrollerImp.knobStyle with this type
   (force-included Overlay605Supplement.h gives WebKit the same typedef); this
   local definition keeps sdk_stubs_605.mm self-contained.  The stub accepts the
   value but does not act on it — classic 10.6 NSScroller scrollbars have a single
   knob appearance. */
#ifndef __NSSCROLLERKNOBSTYLE_605_DEFINED__
#define __NSSCROLLERKNOBSTYLE_605_DEFINED__
typedef long NSScrollerKnobStyle;
enum {
    NSScrollerKnobStyleDefault_605 = 0,
    NSScrollerKnobStyleDark_605 = 1,
    NSScrollerKnobStyleLight_605 = 2
};
#endif

/* _NSRecommendedScrollerStyle — declared in NSScrollerImpSPI.h:154 and called
   from WebHTMLView.mm:4660.  On 10.6 there are only legacy scrollers. */
extern "C" NSScrollerStyle _NSRecommendedScrollerStyle(void) {
    return (NSScrollerStyle)0;  /* NSScrollerStyleLegacy */
}

/* _CFAppVersionCheckLessThan — declared in pal/spi/cf/CFUtilitiesSPI.h:50, used
   at ~15 sites in WebView.mm for legacy app quirks (always with the -1 sentinel).
   Returning false means "this app is NOT older than the quirk threshold" → no
   quirks applied, which is correct for a modern WebKit on 10.6. */
extern "C" Boolean _CFAppVersionCheckLessThan(CFStringRef bundleID, int linkedOnAnOlderSystemThan, double versionNumberLessThan) {
    (void)bundleID; (void)linkedOnAnOlderSystemThan; (void)versionNumberLessThan;
    return false;
}

/* NSWindowWillOrder{On,Off}ScreenNotification — declared in
   pal/spi/mac/NSWindowSPI.h:52-53 and used by WebView.mm window-order observers.
   These private AppKit constants existed on 10.6 but are not in the public SDK
   headers; provide the literal-string definition so the observers link.  The
   literal value matches WebKit's own convention for unavailable AppKit constants
   (WebView.mm:5884) and should be verified to fire at runtime. */
extern "C" {
NSString * const NSWindowWillOrderOnScreenNotification = @"NSWindowWillOrderOnScreenNotification";
NSString * const NSWindowWillOrderOffScreenNotification = @"NSWindowWillOrderOffScreenNotification";
}

/* NSWindow (WebKitCompat605) — screen-coordinate rect conversion, the 10.7+
   replacements for the 10.6 convertBaseToScreen:/convertScreenToBase: pair.
   The category is DECLARED in Overlay605Supplement.h (force-included, so visible
   at the WebHTMLView/WebImmediateActionController call sites).  Here we provide a
   WORKING 10.6 implementation: the 10.6 primitives convert POINTS, so convert the
   rect's origin and preserve its size.  This is exactly the transform the 10.7+
   methods perform (no scaling), so the bridge is correct.  Guarded < 1070 so the
   real methods (on newer SDKs) are not shadowed. */
#if defined(__OBJC__) && __MAC_OS_X_VERSION_MAX_ALLOWED < 1070
@implementation NSWindow (WebKitCompat605)
- (NSRect)convertRectFromScreen:(NSRect)rect
{
    rect.origin = [self convertScreenToBase:rect.origin];
    return rect;
}
- (NSRect)convertRectToScreen:(NSRect)rect
{
    rect.origin = [self convertBaseToScreen:rect.origin];
    return rect;
}
@end
#endif

/* pthread QoS — pthread_set_qos_class_self_np() ships in libsystem_pthread on
   10.10+.  bmalloc's scavenger thread calls it (Scavenger.cpp:189) to drop to a
   background QoS; on 10.6 there is no QoS API, so provide a no-op that reports
   success.  The scavenger then simply runs at the thread's default priority,
   which is correct for 10.6.  (qos_class_t is `unsigned int`; the declaration
   that Scavenger.cpp sees is force-included via Overlay605Supplement.h.) */
extern "C" int pthread_set_qos_class_self_np(unsigned int qos_class, int relative_priority) {
    (void)qos_class; (void)relative_priority; return 0;
}

/* ====================================================================== */
/*  CoreText symbols absent from the 10.6 SDK (see CoreTextSPI.h guards)   */
/* ====================================================================== */

/* kCTFontOpenTypeFeatureTag / kCTFontOpenTypeFeatureValue — 10.10+ CoreText
   OpenType feature attribute keys.  Declared (C linkage) in CoreTextSPI.h.
   At runtime on 10.6 CoreText never produces these dictionary keys, so the
   OpenType branch in FontCocoa.mm is dead; the string values are arbitrary
   unique placeholders. */
extern "C" const CFStringRef kCTFontOpenTypeFeatureTag = CFSTR("org.webkit.kCTFontOpenTypeFeatureTag");
extern "C" const CFStringRef kCTFontOpenTypeFeatureValue = CFSTR("org.webkit.kCTFontOpenTypeFeatureValue");

/* CTFontDrawGlyphs — public CoreText glyph-drawing API (10.7+).  10.6 fallback:
   configure the CGContext with the CTFont's CGFont, size and matrix, then draw
   via CGContextShowGlyphsAtPositions — the primitive CTFontDrawGlyphs itself
   uses internally on newer OSes.  Every API below is available on 10.6
   (CoreText 10.5+, CGContext glyph primitives 10.5+), so the result is
   behaviour-equivalent for WebKit's drawGlyphs() callers in FontCascadeCocoa.mm. */
extern "C" void CTFontDrawGlyphs(CTFontRef font, const CGGlyph* glyphs, const CGPoint* positions, size_t count, CGContextRef context)
{
    CGFontRef cgFont = CTFontCopyGraphicsFont(font, nullptr);
    CGContextSetFont(context, cgFont);
    CGContextSetFontSize(context, CTFontGetSize(font));
    CGContextSetTextMatrix(context, CTFontGetMatrix(font));
    CGContextShowGlyphsAtPositions(context, glyphs, positions, count);
    CGFontRelease(cgFont);
}

/* ====================================================================== */
/*  CommonCrypto CCRandom — the entire CCRandom SPI is 10.7+; 10.6         */
/*  libcommonCrypto exports NEITHER kCCRandomDefault nor CCRandomCopyBytes.*/
/* ====================================================================== */
#include <fcntl.h>
#include <unistd.h>

/* Match the SPI declaration every WebKit TU makes itself (CommonCryptoSPI.h,
   bmalloc/CryptoRandom.cpp, WebCore/crypto/CommonCryptoUtilities.h):
       typedef struct __CCRandom* CCRandomRef;
       extern const CCRandomRef kCCRandomDefault;
       int CCRandomCopyBytes(CCRandomRef rnd, void *bytes, size_t count);
   Referenced from JSC (WTF/wtf/RandomDevice.cpp — security-sensitive: feeds
   hash/structure randomization + WebCrypto), bmalloc/CryptoRandom.cpp, and
   WebCore/crypto.  Provide a WORKING implementation backed by /dev/urandom
   (the kernel CSPRNG, present on 10.6) — NOT a no-op: returning constant bytes
   would weaken JSC's randomization and break SubtleCrypto.  kCCRandomDefault is
   an opaque token the caller passes straight back to CCRandomCopyBytes, which
   ignores it (the "default" RNG is implicit); any stable non-NULL value works. */
typedef struct __CCRandom* CCRandomRef;
static int ccrandom_default_storage;
extern "C" const CCRandomRef kCCRandomDefault = (CCRandomRef)&ccrandom_default_storage;

extern "C" int CCRandomCopyBytes(CCRandomRef rnd, void *bytes, size_t count)
{
    (void)rnd;  /* kCCRandomDefault is an opaque token; the default RNG is implicit */
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0)
        return -1;  /* failure (WebKit treats non-zero as error) */
    char *p = (char *)bytes;
    size_t got = 0;
    while (got < count) {
        ssize_t n = read(fd, p + got, count - got);
        if (n <= 0) { close(fd); return -1; }
        got += (size_t)n;
    }
    close(fd);
    return 0;  /* kCCSuccess */
}

/* ====================================================================== */
/*  Batch: 10.7+ C-symbol SPI discovered missing by the dlsym oracle run    */
/*  against the real 10.6 VM (see webkit-605-migration-audit). These are   */
/*  the symbols NOT covered by static-ICU, NOT ObjC-runtime class refs,    */
/*  and NOT dyld_stub_binder. No-op functions return NULL/0; WebCore       */
/*  guards their results and degrades gracefully.                          */
/* ====================================================================== */

/* ---- NSFontWeight* : AppKit font-weight trait values (CGFloat, 10.7+).   */
/* Declared `extern const CGFloat NSFontWeightXxx` in WebKit SPI; values    */
/* are Apple's standard weight axis offsets. 10.6 has no weight traits, but */
/* providing the real numbers keeps font-weight selection correct.          */
extern "C" const CGFloat NSFontWeightUltraLight = -0.8;
extern "C" const CGFloat NSFontWeightThin        = -0.6;
extern "C" const CGFloat NSFontWeightLight       = -0.4;
extern "C" const CGFloat NSFontWeightRegular     = 0.0;
extern "C" const CGFloat NSFontWeightMedium      = 0.23;
extern "C" const CGFloat NSFontWeightSemibold    = 0.3;
extern "C" const CGFloat NSFontWeightBold        = 0.4;
extern "C" const CGFloat NSFontWeightHeavy       = 0.56;
extern "C" const CGFloat NSFontWeightBlack       = 0.62;

/* ---- AppKit / CoreText string-constant attribute keys (10.7+).          */
extern "C" NSString *NSTextInsertionUndoableAttributeName = @"NSTextInsertionUndoableAttributeName";
extern "C" const CFStringRef kCTFontOpticalSizeAttribute = CFSTR("CTFontOpticalSizeAttribute");
extern "C" const CFStringRef kCTFrameMaximumNumberOfLinesAttributeName = CFSTR("CTFrameMaximumNumberOfLines");

/* ---- QuartzCore CA filter name keys. Declared `extern NSString * const  */
/* kCAFilterXxx` in WebKit SPI -> they MUST be real NSStrings (the previous */
/* const-char* form was a latent crash: ObjC messages sent to a C string). */
extern "C" NSString * const kCAFilterColorInvert        = @"colorInvert";
extern "C" NSString * const kCAFilterGaussianBlur       = @"gaussianBlur";
extern "C" NSString * const kCAFilterColorBurnBlendMode = @"colorBurnBlendMode";
extern "C" NSString * const kCAFilterColorDodgeBlendMode = @"colorDodgeBlendMode";
extern "C" NSString * const kCAFilterColorHueRotate     = @"colorHueRotate";
extern "C" NSString * const kCAFilterColorMatrix        = @"colorMatrix";
extern "C" NSString * const kCAFilterColorMonochrome    = @"colorMonochrome";
extern "C" NSString * const kCAFilterColorSaturate      = @"colorSaturate";
extern "C" NSString * const kCAFilterDarkenBlendMode    = @"darkenBlendMode";
extern "C" NSString * const kCAFilterDifferenceBlendMode = @"differenceBlendMode";
extern "C" NSString * const kCAFilterExclusionBlendMode = @"exclusionBlendMode";
extern "C" NSString * const kCAFilterHardLightBlendMode = @"hardLightBlendMode";
extern "C" NSString * const kCAFilterLightenBlendMode   = @"lightenBlendMode";
extern "C" NSString * const kCAFilterMultiplyBlendMode  = @"multiplyBlendMode";
extern "C" NSString * const kCAFilterOverlayBlendMode   = @"overlayBlendMode";
extern "C" NSString * const kCAFilterPlusD              = @"plusD";
extern "C" NSString * const kCAFilterScreenBlendMode    = @"screenBlendMode";
extern "C" NSString * const kCAFilterSoftLightBlendMode = @"softLightBlendMode";

/* ---- CFNetwork / ImageIO private dictionary keys (10.7+). Placeholders  */
/* are fine: they are used as CFDictionary keys; a lookup miss is benign.   */
extern "C" const CFStringRef kCFStreamPropertySourceApplication = CFSTR("kCFStreamPropertySourceApplication");
extern "C" const CFStringRef _kCFStreamSocketSetNoDelay        = CFSTR("_kCFStreamSocketSetNoDelay");
extern "C" const CFStringRef _kCFURLCachePartitionKey          = CFSTR("_kCFURLCachePartitionKey");
extern "C" const CFStringRef kCFURLRequestAllowAllPOSTCaching  = CFSTR("kCFURLRequestAllowAllPOSTCaching");
extern "C" const CFStringRef kCGImageSourceShouldCacheImmediately = CFSTR("kCGImageSourceShouldCacheImmediately");
extern "C" const CFStringRef kCGImageSourceSkipMetadata        = CFSTR("kCGImageSourceSkipMetadata");

/* ---- 10.7+ SPI functions. All no-op: return NULL/0/false; callers guard */
/* the result and fall back to the 10.6 path. x86_64 ignores unclaimed arg */
/* registers, so (void) arity is safe for these no-op stubs.               */
extern "C" {
/* CFURLRequest priority (10.7). */
int  CFURLRequestGetRequestPriority(void) { return 0; }
void CFURLRequestSetRequestPriority(void) { }

/* IOSurface-backed CGContext (10.7+). NULL -> callers create a normal context. */
void *CGIOSurfaceContextCreate(void) { return NULL; }
void *CGIOSurfaceContextCreateImage(void) { return NULL; }
void *CGIOSurfaceContextCreateImageReference(void) { return NULL; }
void *CGIOSurfaceContextGetColorSpace(void) { return NULL; }
void *CGContextCopyDeviceColorSpace(void) { return NULL; }

/* CoreText 10.7+ SPI, absent from 10.6 CoreText.                               */
/* CTFontCreateForCSS — CSS font-matching SPI. The 10.6 build takes the          */
/* SHOULD_USE_CORE_TEXT_FONT_LOOKUP path, which calls this as the PRIMARY family  */
/* resolver (not a fallback). A NULL return therefore makes FontCache::           */
/* fontForFamily() yield nullptr, and lastResortFallbackFont() derefs it (SIGSEGV,*/
/* "FontCache::lastResortFallbackFont().cold"). Implement it via the public 10.6  */
/* API: resolve by PostScript/family name, then apply the bold/italic symbolic    */
/* traits the caller already encoded. (The uint16 CSS weight is reduced to a      */
/* bold/not-bold bit here; precise weight axes aren't expressible on 10.6.)       */
CTFontRef CTFontCreateForCSS(CFStringRef name, uint16_t weight, CTFontSymbolicTraits traits, CGFloat size)
{
    (void)weight;
    CTFontRef font = CTFontCreateWithName(name, size, nullptr);
    if (!font)
        return nullptr;
    CTFontSymbolicTraits desired = traits & (kCTFontBoldTrait | kCTFontItalicTrait);
    if (desired) {
        CTFontRef styled = CTFontCreateCopyWithSymbolicTraits(font, size, nullptr, desired, desired);
        if (styled) {
            CFRelease(font);
            return styled;
        }
    }
    return font;
}
/* CTFontCreateForCharactersWithLanguage — per-glyph system fallback (used for     */
/* missing glyphs, e.g. CJK). NULL => WebCore treats the run as having no fallback */
/* (Latin pages render fine; non-Latin shows .notdef). Safe to leave NULL for now. */
void *CTFontCreateForCharactersWithLanguage(void) { return NULL; }
unsigned char CTFontDescriptorIsSystemUIFont(void) { return 0; }
void CTFontGetVerticalGlyphsForCharacters(void) { }       /* leaves caller glyph buffer as-is */
void CTFontTransformGlyphs(void) { }
void CTRunGetBaseAdvancesAndOrigins(void) { }
void *CTTypesetterCreateWithUniCharProviderAndOptions(void) { return NULL; }

/* Data Detectors (10.7+) — used by editor;s never on the basic load path. */
void *DDScannerCreate(void) { return NULL; }
void *DDScannerCopyResultsWithOptions(void) { return NULL; }

/* Speech synthesis SPI (10.7+). NULL -> no custom voice list. */
void *CopySpeechSynthesisVoicesForMode(void) { return NULL; }
void *GetIdentifierStringForPreferredVoiceInListWithLocale(void) { return NULL; }

/* IOSurface purgeable state (10.7+). 0 = non-purgeable, harmless default. */
int IOSurfaceSetPurgeable(void) { return 0; }

/* Security: certificate signature hash alg (10.7+). NULL -> caller skips. */
void *SecCertificateGetSignatureHashAlgorithm(void) { return NULL; }

/* CFNetwork private SPI (10.7+). NULL/false -> networking degrades safely. */
void *_CFCachedURLResponseGetMemMappedData(void) { return NULL; }
void _CFCachedURLResponseSetBecameFileBackedCallBackBlock(void) { }
void *_CFHTTPCookieStorageCopyCookiesForURLWithMainDocumentURL(void) { return NULL; }
void _CFHTTPMessageSetResponseProxyURL(void) { }
unsigned char _CFHostIsDomainTopLevel(void) { return 0; }
unsigned char _CFNetworkIsKnownHSTSHostWithSession(void) { return 0; }
void *_CFWebServicesCopyProviderInfo(void) { return NULL; }

/* objc autorelease pool fast-path (ARC-era libobjc, absent on 10.6). WebKit */
/* is non-ARC, so these are rarely called; no-op (objects leak, never crash). */
void *objc_autoreleasePoolPush(void) { return (void *)0x1; }
void objc_autoreleasePoolPop(void *ctx) { (void)ctx; }
}

/* pthread_set_qos_class_self_np is also referenced via its C++-mangled name  */
/* (a WebCore TU saw the declaration without extern "C"). Alias the C stub   */
/* (defined above) under the mangled symbol, exactly like __sandbox_check.   */
extern "C" int __pthread_set_qos_class_self_np_mangled(unsigned int qos, int pri) { (void)qos; (void)pri; return 0; }
asm(".globl __Z29pthread_set_qos_class_self_npji");
asm(".set __Z29pthread_set_qos_class_self_npji, ___pthread_set_qos_class_self_np_mangled");

/*  libm intrinsics a modern Xcode clang emits but 10.6's libm lacks.        */
/*  clang folds `sin();cos();` into __sincos_stret and `pow(10,x)`-style     */
/*  code into __exp10 when optimizing — so every TU compiled with this       */
/*  toolchain carries references to these *internal* libm names.  10.6's      */
/*  libcommonCrypto/libSystem predates them, leaving them unresolved; that   */
/*  kept our JavaScriptCore.framework from loading AT ALL (dyld fell back to */
/*  the system JSC, which has no JSContext -> WebCore's class ref crashed).  */
/*                                                                            */
/*  Signatures match <math.h> verbatim (struct __double2 = {sin,cos}).       */
/*  CRITICAL: every function is `optnone`. If these were optimized, clang    */
/*  would fold our own sin()+cos() right back into a __sincos_stret call ->  */
/*  infinite recursion. optnone forces sin/cos/exp to stay as real libm      */
/*  calls (all present on 10.6), and the 16-byte struct return is emitted    */
/*  per the x86_64 ABI (RAX:RDX) by the compiler.                            */
struct __double2 { double __sinval; double __cosval; };
struct __float2  { float  __sinval; float  __cosval; };

extern "C" __attribute__((optnone))
double __exp10(double x) { return exp(x * M_LN10); }

extern "C" __attribute__((optnone))
float __exp10f(float x) { return expf(x * (float)M_LN10); }

extern "C" __attribute__((optnone))
struct __double2 __sincos_stret(double x) {
    struct __double2 r; r.__sinval = sin(x); r.__cosval = cos(x); return r;
}

extern "C" __attribute__((optnone))
struct __float2 __sincosf_stret(float x) {
    struct __float2 r; r.__sinval = sinf(x); r.__cosval = cosf(x); return r;
}

extern "C" __attribute__((optnone))
struct __double2 __sincospi_stret(double x) {
    struct __double2 r; r.__sinval = sin(M_PI * x); r.__cosval = cos(M_PI * x); return r;
}

extern "C" __attribute__((optnone))
struct __float2 __sincospif_stret(float x) {
    struct __float2 r; r.__sinval = sinf((float)M_PI * x); r.__cosval = cosf((float)M_PI * x); return r;
}


/*  ICU symbol-rename glue.  The static ICU 55 archive was built WITH   */
/*  ICU versioned renaming -> every symbol has a _55 suffix (e.g.        */
/*  ucal_getKeywordValuesForLocale_55).  But the WebKit TUs were built  */
/*  WITHOUT the matching urename.h, so they emit PLAIN ICU names that   */
/*  nothing defines (and many -- ICU 4.6+ -- aren't in 10.6's ICU 4.2   */
/*  libicucore either), keeping JSC from loading.  These asm aliases do  */
/*  at LINK time what urename.h would have done at compile time: map    */
/*  each plain name to its _55 implementation.  Generated from the      */
/*  undefined-vs-defined symbol diff of our own frameworks.             */
asm(".globl _u_charDirection");
asm(".set _u_charDirection, _u_charDirection_55");
asm(".globl _u_charMirror");
asm(".set _u_charMirror, _u_charMirror_55");
asm(".globl _u_charType");
asm(".set _u_charType, _u_charType_55");
asm(".globl _u_errorName");
asm(".set _u_errorName, _u_errorName_55");
asm(".globl _u_foldCase");
asm(".set _u_foldCase, _u_foldCase_55");
asm(".globl _u_getCombiningClass");
asm(".set _u_getCombiningClass, _u_getCombiningClass_55");
asm(".globl _u_getIntPropertyValue");
asm(".set _u_getIntPropertyValue, _u_getIntPropertyValue_55");
asm(".globl _u_getPropertyValueEnum");
asm(".set _u_getPropertyValueEnum, _u_getPropertyValueEnum_55");
asm(".globl _u_hasBinaryProperty");
asm(".set _u_hasBinaryProperty, _u_hasBinaryProperty_55");
asm(".globl _u_isUWhiteSpace");
asm(".set _u_isUWhiteSpace, _u_isUWhiteSpace_55");
asm(".globl _u_isprint");
asm(".set _u_isprint, _u_isprint_55");
asm(".globl _u_ispunct");
asm(".set _u_ispunct, _u_ispunct_55");
asm(".globl _u_strFoldCase");
asm(".set _u_strFoldCase, _u_strFoldCase_55");
asm(".globl _u_strToLower");
asm(".set _u_strToLower, _u_strToLower_55");
asm(".globl _u_strToUpper");
asm(".set _u_strToUpper, _u_strToUpper_55");
asm(".globl _u_tolower");
asm(".set _u_tolower, _u_tolower_55");
asm(".globl _u_totitle");
asm(".set _u_totitle, _u_totitle_55");
asm(".globl _u_toupper");
asm(".set _u_toupper, _u_toupper_55");
asm(".globl _ublock_getCode");
asm(".set _ublock_getCode, _ublock_getCode_55");
asm(".globl _ubrk_close");
asm(".set _ubrk_close, _ubrk_close_55");
asm(".globl _ubrk_current");
asm(".set _ubrk_current, _ubrk_current_55");
asm(".globl _ubrk_first");
asm(".set _ubrk_first, _ubrk_first_55");
asm(".globl _ubrk_following");
asm(".set _ubrk_following, _ubrk_following_55");
asm(".globl _ubrk_getRuleStatus");
asm(".set _ubrk_getRuleStatus, _ubrk_getRuleStatus_55");
asm(".globl _ubrk_isBoundary");
asm(".set _ubrk_isBoundary, _ubrk_isBoundary_55");
asm(".globl _ubrk_next");
asm(".set _ubrk_next, _ubrk_next_55");
asm(".globl _ubrk_openRules");
asm(".set _ubrk_openRules, _ubrk_openRules_55");
asm(".globl _ubrk_open");
asm(".set _ubrk_open, _ubrk_open_55");
asm(".globl _ubrk_preceding");
asm(".set _ubrk_preceding, _ubrk_preceding_55");
asm(".globl _ubrk_setText");
asm(".set _ubrk_setText, _ubrk_setText_55");
asm(".globl _ubrk_setUText");
asm(".set _ubrk_setUText, _ubrk_setUText_55");
asm(".globl _ucal_getCanonicalTimeZoneID");
asm(".set _ucal_getCanonicalTimeZoneID, _ucal_getCanonicalTimeZoneID_55");
asm(".globl _ucal_getDefaultTimeZone");
asm(".set _ucal_getDefaultTimeZone, _ucal_getDefaultTimeZone_55");
asm(".globl _ucal_getKeywordValuesForLocale");
asm(".set _ucal_getKeywordValuesForLocale, _ucal_getKeywordValuesForLocale_55");
asm(".globl _ucal_openTimeZones");
asm(".set _ucal_openTimeZones, _ucal_openTimeZones_55");
asm(".globl _ucnv_cbFromUWriteBytes");
asm(".set _ucnv_cbFromUWriteBytes, _ucnv_cbFromUWriteBytes_55");
asm(".globl _ucnv_cbFromUWriteUChars");
asm(".set _ucnv_cbFromUWriteUChars, _ucnv_cbFromUWriteUChars_55");
asm(".globl _ucnv_close");
asm(".set _ucnv_close, _ucnv_close_55");
asm(".globl _ucnv_fromUnicode");
asm(".set _ucnv_fromUnicode, _ucnv_fromUnicode_55");
asm(".globl _ucnv_getCanonicalName");
asm(".set _ucnv_getCanonicalName, _ucnv_getCanonicalName_55");
asm(".globl _ucnv_getName");
asm(".set _ucnv_getName, _ucnv_getName_55");
asm(".globl _ucnv_open");
asm(".set _ucnv_open, _ucnv_open_55");
asm(".globl _ucnv_reset");
asm(".set _ucnv_reset, _ucnv_reset_55");
asm(".globl _ucnv_setFallback");
asm(".set _ucnv_setFallback, _ucnv_setFallback_55");
asm(".globl _ucnv_setFromUCallBack");
asm(".set _ucnv_setFromUCallBack, _ucnv_setFromUCallBack_55");
asm(".globl _ucnv_setSubstChars");
asm(".set _ucnv_setSubstChars, _ucnv_setSubstChars_55");
asm(".globl _ucnv_setToUCallBack");
asm(".set _ucnv_setToUCallBack, _ucnv_setToUCallBack_55");
asm(".globl _ucnv_toUnicode");
asm(".set _ucnv_toUnicode, _ucnv_toUnicode_55");
asm(".globl _ucol_close");
asm(".set _ucol_close, _ucol_close_55");
asm(".globl _ucol_countAvailable");
asm(".set _ucol_countAvailable, _ucol_countAvailable_55");
asm(".globl _ucol_getAvailable");
asm(".set _ucol_getAvailable, _ucol_getAvailable_55");
asm(".globl _ucol_getKeywordValuesForLocale");
asm(".set _ucol_getKeywordValuesForLocale, _ucol_getKeywordValuesForLocale_55");
asm(".globl _ucol_getStrength");
asm(".set _ucol_getStrength, _ucol_getStrength_55");
asm(".globl _ucol_open");
asm(".set _ucol_open, _ucol_open_55");
asm(".globl _ucol_setAttribute");
asm(".set _ucol_setAttribute, _ucol_setAttribute_55");
asm(".globl _ucol_setStrength");
asm(".set _ucol_setStrength, _ucol_setStrength_55");
asm(".globl _ucol_strcollIter");
asm(".set _ucol_strcollIter, _ucol_strcollIter_55");
asm(".globl _ucsdet_close");
asm(".set _ucsdet_close, _ucsdet_close_55");
asm(".globl _ucsdet_detectAll");
asm(".set _ucsdet_detectAll, _ucsdet_detectAll_55");
asm(".globl _ucsdet_enableInputFilter");
asm(".set _ucsdet_enableInputFilter, _ucsdet_enableInputFilter_55");
asm(".globl _ucsdet_getConfidence");
asm(".set _ucsdet_getConfidence, _ucsdet_getConfidence_55");
asm(".globl _ucsdet_getName");
asm(".set _ucsdet_getName, _ucsdet_getName_55");
asm(".globl _ucsdet_open");
asm(".set _ucsdet_open, _ucsdet_open_55");
asm(".globl _ucsdet_setText");
asm(".set _ucsdet_setText, _ucsdet_setText_55");
asm(".globl _udat_close");
asm(".set _udat_close, _udat_close_55");
asm(".globl _udat_countAvailable");
asm(".set _udat_countAvailable, _udat_countAvailable_55");
asm(".globl _udat_formatForFields");
asm(".set _udat_formatForFields, _udat_formatForFields_55");
asm(".globl _udat_format");
asm(".set _udat_format, _udat_format_55");
asm(".globl _udat_getAvailable");
asm(".set _udat_getAvailable, _udat_getAvailable_55");
asm(".globl _udat_open");
asm(".set _udat_open, _udat_open_55");
asm(".globl _udatpg_close");
asm(".set _udatpg_close, _udatpg_close_55");
asm(".globl _udatpg_getBestPattern");
asm(".set _udatpg_getBestPattern, _udatpg_getBestPattern_55");
asm(".globl _udatpg_open");
asm(".set _udatpg_open, _udatpg_open_55");
asm(".globl _uenum_close");
asm(".set _uenum_close, _uenum_close_55");
asm(".globl _uenum_next");
asm(".set _uenum_next, _uenum_next_55");
asm(".globl _uenum_unext");
asm(".set _uenum_unext, _uenum_unext_55");
asm(".globl _ufieldpositer_close");
asm(".set _ufieldpositer_close, _ufieldpositer_close_55");
asm(".globl _ufieldpositer_next");
asm(".set _ufieldpositer_next, _ufieldpositer_next_55");
asm(".globl _ufieldpositer_open");
asm(".set _ufieldpositer_open, _ufieldpositer_open_55");
asm(".globl _uidna_IDNToASCII");
asm(".set _uidna_IDNToASCII, _uidna_IDNToASCII_55");
asm(".globl _uidna_nameToASCII");
asm(".set _uidna_nameToASCII, _uidna_nameToASCII_55");
asm(".globl _uidna_nameToUnicode");
asm(".set _uidna_nameToUnicode, _uidna_nameToUnicode_55");
asm(".globl _uidna_openUTS46");
asm(".set _uidna_openUTS46, _uidna_openUTS46_55");
asm(".globl _uiter_setString");
asm(".set _uiter_setString, _uiter_setString_55");
asm(".globl _uiter_setUTF8");
asm(".set _uiter_setUTF8, _uiter_setUTF8_55");
asm(".globl _uloc_getDefault");
asm(".set _uloc_getDefault, _uloc_getDefault_55");
asm(".globl _uloc_setKeywordValue");
asm(".set _uloc_setKeywordValue, _uloc_setKeywordValue_55");
asm(".globl _unorm2_getNFCInstance");
asm(".set _unorm2_getNFCInstance, _unorm2_getNFCInstance_55");
asm(".globl _unorm2_getNFDInstance");
asm(".set _unorm2_getNFDInstance, _unorm2_getNFDInstance_55");
asm(".globl _unorm2_getNFKCInstance");
asm(".set _unorm2_getNFKCInstance, _unorm2_getNFKCInstance_55");
asm(".globl _unorm2_getNFKDInstance");
asm(".set _unorm2_getNFKDInstance, _unorm2_getNFKDInstance_55");
asm(".globl _unorm2_normalize");
asm(".set _unorm2_normalize, _unorm2_normalize_55");
asm(".globl _unorm_normalize");
asm(".set _unorm_normalize, _unorm_normalize_55");
asm(".globl _unorm_quickCheck");
asm(".set _unorm_quickCheck, _unorm_quickCheck_55");
asm(".globl _unum_close");
asm(".set _unum_close, _unum_close_55");
asm(".globl _unum_countAvailable");
asm(".set _unum_countAvailable, _unum_countAvailable_55");
asm(".globl _unum_formatDouble");
asm(".set _unum_formatDouble, _unum_formatDouble_55");
asm(".globl _unum_getAvailable");
asm(".set _unum_getAvailable, _unum_getAvailable_55");
asm(".globl _unum_open");
asm(".set _unum_open, _unum_open_55");
asm(".globl _unum_setAttribute");
asm(".set _unum_setAttribute, _unum_setAttribute_55");
asm(".globl _unum_setTextAttribute");
asm(".set _unum_setTextAttribute, _unum_setTextAttribute_55");
asm(".globl _unumsys_close");
asm(".set _unumsys_close, _unumsys_close_55");
asm(".globl _unumsys_getName");
asm(".set _unumsys_getName, _unumsys_getName_55");
asm(".globl _unumsys_openAvailableNames");
asm(".set _unumsys_openAvailableNames, _unumsys_openAvailableNames_55");
asm(".globl _unumsys_open");
asm(".set _unumsys_open, _unumsys_open_55");
asm(".globl _uscript_getScript");
asm(".set _uscript_getScript, _uscript_getScript_55");
asm(".globl _usearch_getCollator");
asm(".set _usearch_getCollator, _usearch_getCollator_55");
asm(".globl _usearch_getMatchedLength");
asm(".set _usearch_getMatchedLength, _usearch_getMatchedLength_55");
asm(".globl _usearch_next");
asm(".set _usearch_next, _usearch_next_55");
asm(".globl _usearch_open");
asm(".set _usearch_open, _usearch_open_55");
asm(".globl _usearch_reset");
asm(".set _usearch_reset, _usearch_reset_55");
asm(".globl _usearch_setAttribute");
asm(".set _usearch_setAttribute, _usearch_setAttribute_55");
asm(".globl _usearch_setOffset");
asm(".set _usearch_setOffset, _usearch_setOffset_55");
asm(".globl _usearch_setPattern");
asm(".set _usearch_setPattern, _usearch_setPattern_55");
asm(".globl _usearch_setText");
asm(".set _usearch_setText, _usearch_setText_55");
asm(".globl _utext_close");
asm(".set _utext_close, _utext_close_55");
asm(".globl _utext_setup");
asm(".set _utext_setup, _utext_setup_55");
asm(".globl _utf8_appendCharSafeBody");
asm(".set _utf8_appendCharSafeBody, _utf8_appendCharSafeBody_55");

/* =====================================================================
 * NSSCROLLERIMP STUBS — 10.7+ overlay-scrollbar classes for 10.6.
 *
 * NSScrollerImpSPI.h (non-internal-SDK branch) declares the full @interface
 * for NSScrollerImp / NSScrollerImpPair but ships NO @implementation; the
 * classes exist only in AppKit ≥ 10.7.  WebCore's ScrollAnimatorMac.mm /
 * ScrollbarThemeMac.mm compile against that @interface (emitting class-ref
 * relocations) AND unconditionally instantiate/message them at runtime
 * (ScrollAnimatorMac.mm:636 `[[NSScrollerImpPair alloc] init]`,
 * ScrollbarThemeMac.mm:172 `[NSScrollerImp scrollerImpWithStyle:...]`).
 * dyld treats the _OBJC_CLASS_$_ refs as REQUIRED hard relocations — they do
 * NOT resolve to nil under -undefined,dynamic_lookup — so without these
 * stubs WebCore fails to load: "Symbol not found: _OBJC_CLASS_$_NSScrollerImp".
 *
 * Every selector below is one WebCore actually sends (extracted via grep over
 * Source/WebCore); @properties auto-synthesize accessors, the factory returns a
 * live instance, void methods are no-ops, and the one query returns NO.  Since
 * _NSRecommendedScrollerStyle() above returns Legacy, these stay inert — the
 * stubs exist to satisfy dyld + keep every call safe, not to paint scrollbars.
 * NSScrollerStyle is typedef NSInteger (line ~279); CALayer/NSControlSize/
 * NSScrollerPart come from the imported AppKit/QuartzCore.  The 10.6-absent
 * NSScrollerKnobStyle / NSUserInterfaceLayoutDirection are deliberately omitted
 * (WebCore never sends them). */
@protocol NSScrollerImpDelegate;        /* forward — type annotation only */
@protocol NSScrollerImpPairDelegate;    /* forward — type annotation only */

@interface NSScrollerImp : NSObject
+ (NSScrollerImp *)scrollerImpWithStyle:(NSScrollerStyle)scrollerStyle controlSize:(NSControlSize)controlSize horizontal:(BOOL)horizontal replacingScrollerImp:(id)previous;
@property (assign) id<NSScrollerImpDelegate> delegate;
@property (retain) CALayer *layer;
@property BOOL needsDisplay;
@property (getter=isHorizontal) BOOL horizontal;
@property (getter=isEnabled) BOOL enabled;
@property NSSize boundsSize;
@property double doubleValue;
@property double presentationValue;
@property (getter=shouldUsePresentationValue) BOOL usePresentationValue;
@property CGFloat knobProportion;
@property CGFloat uiStateTransitionProgress;
@property CGFloat expansionTransitionProgress;
@property CGFloat trackAlpha;
@property CGFloat knobAlpha;
@property (getter=isExpanded) BOOL expanded;
@property NSScrollerKnobStyle knobStyle;
@property NSUserInterfaceLayoutDirection userInterfaceLayoutDirection;
/* readonly geometry — ScrollbarThemeMac::scrollbarThickness/hasThumb query these.
 * On 10.6 there is no NSScrollerImp, so derive thickness from the real NSScroller
 * width and expose classic (non-overlay) insets (all zero). */
@property (readonly) CGFloat trackBoxWidth;
@property (readonly) CGFloat trackWidth;
@property (readonly) CGFloat trackSideInset;
@property (readonly) CGFloat trackEndInset;
@property (readonly) CGFloat knobEndInset;
@property (readonly) CGFloat knobMinLength;
@property (readonly) CGFloat knobOverlapEndInset;
@property (readonly) CGFloat trackOverlapEndInset;
- (NSRect)rectForPart:(NSScrollerPart)partCode;
- (void)drawKnob;
- (void)drawKnobSlotInRect:(NSRect)slotRect highlight:(BOOL)flag;
- (void)drawKnobSlotInRect:(NSRect)slotRect highlight:(BOOL)flag alpha:(CGFloat)alpha;
- (void)mouseEnteredScroller;
@end

/* NSScrollerImp is a 10.7+ overlay-scrollbar painter with no 10.6 equivalent.
 * Rather than no-op every call (which left Safari crashing on -trackBoxWidth and
 * would render blank scrollbar gutters), we back the drawing with a real 10.6
 * NSScroller: its -drawKnob / -drawKnobSlotInRect:highlight: / -rectForPart:
 * produce genuine classic scrollbars, and -scrollerWidthForControlSize: feeds the
 * geometry getters WebCore reads for layout.  NSScroller infers orientation from
 * its frame aspect ratio, so setBoundsSize: is enough to orient it. */
@implementation NSScrollerImp {
    NSControlSize _slControlSize;
    NSScroller *_slScroller;   /* retained for the lifetime of this imp (MRC) */
}

+ (NSScrollerImp *)scrollerImpWithStyle:(NSScrollerStyle)scrollerStyle controlSize:(NSControlSize)controlSize horizontal:(BOOL)horizontal replacingScrollerImp:(id)previous
{
    (void)scrollerStyle; (void)previous;
    NSScrollerImp *imp = [[NSScrollerImp alloc] init];
    imp->_slControlSize = controlSize;
    imp->_horizontal = horizontal;
    imp->_slScroller = [[NSScroller alloc] initWithFrame:NSZeroRect];
    [imp->_slScroller setControlSize:controlSize];
    return imp;
}

- (CGFloat)trackBoxWidth { return [NSScroller scrollerWidthForControlSize:_slControlSize]; }
- (CGFloat)trackWidth { return [NSScroller scrollerWidthForControlSize:_slControlSize]; }
- (CGFloat)trackSideInset { return 0; }
- (CGFloat)trackEndInset { return 0; }
- (CGFloat)knobEndInset { return 0; }
- (CGFloat)knobMinLength { return 26; }
- (CGFloat)knobOverlapEndInset { return 0; }
- (CGFloat)trackOverlapEndInset { return 0; }

/* Keep the backing NSScroller's frame/value/proportion in lock-step with the
 * state WebCore pushes before each paint, so its drawing lands in the right spot. */
- (void)setBoundsSize:(NSSize)size
{
    _boundsSize = size;
    if (_slScroller)
        [_slScroller setFrame:NSMakeRect(0, 0, size.width, size.height)];
}
- (void)setDoubleValue:(double)value
{
    _doubleValue = value;
    if (_slScroller) [_slScroller setDoubleValue:value];
}
- (void)setKnobProportion:(CGFloat)proportion
{
    _knobProportion = proportion;
    if (_slScroller) [_slScroller setKnobProportion:proportion];
}
- (void)setEnabled:(BOOL)flag
{
    _enabled = flag;
    if (_slScroller) [_slScroller setEnabled:flag];
}

- (NSRect)rectForPart:(NSScrollerPart)partCode
{
    if (_slScroller) return [_slScroller rectForPart:partCode];
    return NSZeroRect;
}
- (void)drawKnobSlotInRect:(NSRect)slotRect highlight:(BOOL)flag
{
    if (_slScroller) [_slScroller drawKnobSlotInRect:slotRect highlight:flag];
}
- (void)drawKnobSlotInRect:(NSRect)slotRect highlight:(BOOL)flag alpha:(CGFloat)alpha
{
    (void)alpha;
    if (_slScroller) [_slScroller drawKnobSlotInRect:slotRect highlight:flag];
}
- (void)drawKnob
{
    if (_slScroller) [_slScroller drawKnob];
}
- (void)mouseEnteredScroller { }
@end

@interface NSScrollerImpPair : NSObject
@property (assign) id<NSScrollerImpPairDelegate> delegate;
@property (retain) NSScrollerImp *verticalScrollerImp;
@property (retain) NSScrollerImp *horizontalScrollerImp;
@property NSScrollerStyle scrollerStyle;
- (void)flashScrollers;
- (void)hideOverlayScrollers;
- (BOOL)overlayScrollerStateIsLocked;
- (void)contentAreaWillDraw;
- (void)contentAreaDidResize;
- (void)mouseEnteredContentArea;
@end

@implementation NSScrollerImpPair
- (BOOL)overlayScrollerStateIsLocked { return NO; }
- (void)flashScrollers { }
- (void)hideOverlayScrollers { }
- (void)contentAreaWillDraw { }
- (void)contentAreaDidResize { }
- (void)mouseEnteredContentArea { }
@end

/* _NSScrollingMomentumCalculator / _NSScrollingPredominantAxisFilter — 10.8+
 * private AppKit classes (declared in NSScrollingMomentumCalculatorSPI.h /
 * NSScrollingInputFilterSPI.h, no @implementation shipped).  Like the scroller
 * classes above, the _OBJC_CLASS_$_ refs are dyld-hard-relocations that do NOT
 * no-op to nil on 10.6, so without these stubs WebCore fails to load
 * ("Symbol not found: _OBJC_CLASS_$__NSScrollingMomentumCalculator").
 * Selectors below are exactly those WebCore sends
 * (WheelEventDeltaFilterMac.mm:39,56,65; ScrollingMomentumCalculatorMac.mm:13,
 * 15,24,42,43).  Stubs are inert — momentum/predominant-axis filtering no-ops;
 * the momentum calculator returns zero duration/position so no animation runs. */

@interface _NSScrollingMomentumCalculator : NSObject
- (instancetype)initWithInitialOrigin:(NSPoint)origin velocity:(NSPoint)velocity documentFrame:(NSRect)docFrame constrainedClippingOrigin:(NSPoint)constrainedClippingOrigin clippingSize:(NSSize)clipViewSize tolerance:(NSSize)tolerance;
- (NSPoint)positionAfterDuration:(NSTimeInterval)duration;
- (void)calculateToReachDestination;
@property NSPoint destinationOrigin;
@property (readonly) NSTimeInterval durationUntilStop;
@end

@implementation _NSScrollingMomentumCalculator
- (instancetype)initWithInitialOrigin:(NSPoint)origin velocity:(NSPoint)velocity documentFrame:(NSRect)docFrame constrainedClippingOrigin:(NSPoint)constrainedClippingOrigin clippingSize:(NSSize)clipViewSize tolerance:(NSSize)tolerance
{
    (void)origin; (void)velocity; (void)docFrame; (void)constrainedClippingOrigin; (void)clipViewSize; (void)tolerance;
    return [super init];
}
- (NSPoint)positionAfterDuration:(NSTimeInterval)duration { (void)duration; return NSZeroPoint; }
- (void)calculateToReachDestination { }
- (NSTimeInterval)durationUntilStop { return 0; }
@end

@interface _NSScrollingPredominantAxisFilter : NSObject
- (void)filterInputDelta:(NSPoint)delta timestamp:(NSTimeInterval)timestamp outputDelta:(NSPoint *)outputDelta velocity:(NSPoint *)velocity;
- (void)reset;
@end

@implementation _NSScrollingPredominantAxisFilter
/* Pass the input delta through unchanged (no predominant-axis snapping) so
 * wheel scrolling still works; velocity is left at zero (callers tolerate it). */
- (void)filterInputDelta:(NSPoint)delta timestamp:(NSTimeInterval)timestamp outputDelta:(NSPoint *)outputDelta velocity:(NSPoint *)velocity
{
    (void)timestamp;
    if (outputDelta) *outputDelta = delta;
    if (velocity) *velocity = NSZeroPoint;
}
- (void)reset { }
@end

/* === Legacy WTF threading / main-thread API ===============================
 * Safari 5.0.5's Safari.framework was linked against the 2010 JSC.framework,
 * which still HOSTED WTF and exported these symbols. Modern WebKit (a) moved
 * WTF into libWTF.a linked privately into each framework and (b) replaced the
 * old ThreadIdentifier-based threading API (createThread/detachThread/
 * waitForThreadCompletion/currentThread) and callOnMainThread with WTF::Thread
 * and RunLoop. Our 605 JSC therefore no longer exports them, so when Safari
 * lazily binds e.g. __ZN3WTF36lockAtomicallyInitializedStaticMutexEv from
 * JavaScriptCore.framework, dyld aborts ("Symbol not found").
 *
 * Defining them inside `namespace WTF` makes the compiler emit the exact
 * __ZN3WTF... mangled names Safari expects (no asm aliasing needed). They are
 * FUNCTIONAL: threads bridge to pthreads and callOnMainThread to the main
 * dispatch queue, with cancelCallOnMainThread removing a pending call before
 * it fires. The atomically-initialized-static-mutex pair guards a real
 * pthread_mutex (one-time global init is exactly its purpose in old WTF). */
namespace WTF {

typedef unsigned ThreadIdentifier;

static pthread_mutex_t wtfThreadMutex = PTHREAD_MUTEX_INITIALIZER;
static CFMutableDictionaryRef wtfThreadMap = NULL; /* id -> pthread_t (raw-pointer keys/values) */
static pthread_once_t wtfThreadOnce = PTHREAD_ONCE_INIT;
static pthread_key_t wtfCurrentThreadKey;
static unsigned wtfNextThreadID = 1;

static void wtfThreadInit(void)
{
    pthread_key_create(&wtfCurrentThreadKey, NULL);
    /* NULL callbacks -> raw pointer keys/values, pointer-identity equality/hash. */
    wtfThreadMap = CFDictionaryCreateMutable(NULL, 0, NULL, NULL);
}

struct WTFThreadBootstrap {
    void *(*entry)(void *);
    void *arg;
    ThreadIdentifier id;
};

static void *wtfThreadTrampoline(void *raw)
{
    struct WTFThreadBootstrap *b = (struct WTFThreadBootstrap *)raw;
    void *(*entry)(void *) = b->entry;
    void *arg = b->arg;
    pthread_setspecific(wtfCurrentThreadKey, (void *)(uintptr_t)b->id);
    free(b);
    return entry(arg);
}

__attribute__((visibility("default")))
ThreadIdentifier createThread(void *(*entry)(void *), void *arg, const char *name)
{
    (void)name;
    pthread_once(&wtfThreadOnce, wtfThreadInit);
    struct WTFThreadBootstrap *b = (struct WTFThreadBootstrap *)malloc(sizeof(*b));
    if (!b)
        return 0;
    b->entry = entry;
    b->arg = arg;

    pthread_mutex_lock(&wtfThreadMutex);
    b->id = wtfNextThreadID++;
    pthread_mutex_unlock(&wtfThreadMutex);
    ThreadIdentifier id = b->id; /* copy: trampoline frees b before we read it back */

    pthread_t t;
    if (pthread_create(&t, NULL, wtfThreadTrampoline, b) != 0) {
        free(b);
        return 0;
    }

    pthread_mutex_lock(&wtfThreadMutex);
    CFDictionarySetValue(wtfThreadMap, (const void *)(uintptr_t)id, (const void *)t);
    pthread_mutex_unlock(&wtfThreadMutex);
    return id;
}

__attribute__((visibility("default")))
void detachThread(ThreadIdentifier id)
{
    pthread_once(&wtfThreadOnce, wtfThreadInit);
    pthread_mutex_lock(&wtfThreadMutex);
    const void *t = CFDictionaryGetValue(wtfThreadMap, (const void *)(uintptr_t)id);
    if (t) {
        pthread_detach((pthread_t)t);
        CFDictionaryRemoveValue(wtfThreadMap, (const void *)(uintptr_t)id);
    }
    pthread_mutex_unlock(&wtfThreadMutex);
}

__attribute__((visibility("default")))
ThreadIdentifier currentThread()
{
    pthread_once(&wtfThreadOnce, wtfThreadInit);
    void *p = pthread_getspecific(wtfCurrentThreadKey);
    if (p)
        return (ThreadIdentifier)(uintptr_t)p;
    /* Thread not spawned via createThread (main thread / external pthread).
     * Old WTF guaranteed uniqueness only for its own threads; callers use this
     * for "am I thread X" comparisons, where pthread_self() is stable per-thread. */
    return (ThreadIdentifier)(uintptr_t)pthread_self();
}

__attribute__((visibility("default")))
void waitForThreadCompletion(ThreadIdentifier id, void **result)
{
    pthread_once(&wtfThreadOnce, wtfThreadInit);
    pthread_mutex_lock(&wtfThreadMutex);
    const void *t = CFDictionaryGetValue(wtfThreadMap, (const void *)(uintptr_t)id);
    if (t)
        CFDictionaryRemoveValue(wtfThreadMap, (const void *)(uintptr_t)id);
    pthread_mutex_unlock(&wtfThreadMutex);
    if (t)
        pthread_join((pthread_t)t, result);
    else if (result)
        *result = NULL;
}

/* --- main-thread dispatch (old callOnMainThread / cancelCallOnMainThread) --
 * Pending calls live in wtfPendingMainCalls (array of MainThreadCall*). Set
 * membership is the single ownership token: whoever removes a call from the
 * array owns free(); the other path finds it gone and skips it, so there is no
 * double-free and no use-after-free. dispatch_async_f (C API) avoids block
 * capture of the context pointer. */
struct MainThreadCall {
    void (*fn)(void *);
    void *arg;
};

static pthread_mutex_t wtfMainMutex = PTHREAD_MUTEX_INITIALIZER;
static CFMutableArrayRef wtfPendingMainCalls = NULL; /* of MainThreadCall* */
static pthread_once_t wtfMainOnce = PTHREAD_ONCE_INIT;

static void wtfMainInit(void)
{
    wtfPendingMainCalls = CFArrayCreateMutable(NULL, 0, NULL);
}

static void wtfMainDispatch(void *ctx)
{
    struct MainThreadCall *c = (struct MainThreadCall *)ctx;
    bool run = false;
    pthread_mutex_lock(&wtfMainMutex);
    CFIndex count = CFArrayGetCount(wtfPendingMainCalls);
    for (CFIndex i = 0; i < count; ++i) {
        if (CFArrayGetValueAtIndex(wtfPendingMainCalls, i) == c) {
            CFArrayRemoveValueAtIndex(wtfPendingMainCalls, i);
            run = true;
            break;
        }
    }
    pthread_mutex_unlock(&wtfMainMutex);
    if (run) {
        c->fn(c->arg);
        free(c);
    }
}

__attribute__((visibility("default")))
void callOnMainThread(void (*fn)(void *), void *arg)
{
    pthread_once(&wtfMainOnce, wtfMainInit);
    struct MainThreadCall *c = (struct MainThreadCall *)malloc(sizeof(*c));
    if (!c)
        return;
    c->fn = fn;
    c->arg = arg;
    pthread_mutex_lock(&wtfMainMutex);
    CFArrayAppendValue(wtfPendingMainCalls, c);
    pthread_mutex_unlock(&wtfMainMutex);
    dispatch_async_f(dispatch_get_main_queue(), c, wtfMainDispatch);
}

__attribute__((visibility("default")))
void cancelCallOnMainThread(void (*fn)(void *), void *arg)
{
    pthread_once(&wtfMainOnce, wtfMainInit);
    pthread_mutex_lock(&wtfMainMutex);
    CFIndex count = CFArrayGetCount(wtfPendingMainCalls);
    for (CFIndex i = count - 1; i >= 0; --i) {
        struct MainThreadCall *c = (struct MainThreadCall *)CFArrayGetValueAtIndex(wtfPendingMainCalls, i);
        if (c->fn == fn && c->arg == arg) {
            CFArrayRemoveValueAtIndex(wtfPendingMainCalls, i);
            free(c); /* cancel owns this call; the dispatched block will not find it */
        }
    }
    pthread_mutex_unlock(&wtfMainMutex);
}

/* --- atomically-initialized static mutex (real mutex, not a no-op) -------- */
static pthread_mutex_t wtfAtomicStaticMutex = PTHREAD_MUTEX_INITIALIZER;

__attribute__((visibility("default")))
void lockAtomicallyInitializedStaticMutex()
{
    pthread_mutex_lock(&wtfAtomicStaticMutex);
}

__attribute__((visibility("default")))
void unlockAtomicallyInitializedStaticMutex()
{
    pthread_mutex_unlock(&wtfAtomicStaticMutex);
}

} /* namespace WTF */

/* === WebIconDatabase Safari SPI stub =======================================
 * Safari 5.0.5 calls +[WebIconDatabase _checkIntegrityBeforeOpening] (a private
 * SPI added 2007, DELETED from WebKit per Source/WebKitLegacy/mac/ChangeLog
 * line 3304) during nib instantiation; its absence terminates the app with
 * "unrecognized selector sent to class". Historically it set a hint so the
 * IconDatabase integrity-checks itself on next open. A no-op is safe — we
 * simply skip that integrity check.
 *
 * We cannot use a category (clang rejects a category on a class whose full
 * @interface is not in this TU, and we must not redeclare WebIconDatabase here
 * — that would collide with the real @implementation in WebKitLegacy). Instead
 * a +load hook adds the method to the real class at image-load time via the
 * ObjC runtime. +load runs only after every class in the image is registered,
 * so when this fires inside WebKit.framework (= WebKitLegacy, where
 * WebIconDatabase lives) objc_getClass finds it; copies linked into JSC/WebCore
 * no-op out (WebIconDatabase not present there). This all happens before
 * Safari's nib is instantiated, so the selector is in place when first sent. */
@interface WebIconDatabaseSPIInjector : NSObject
@end

/* Plain C IMP (the 10.6 SDK has no imp_implementationWithBlock): no-op class
 * method — void return, ignores self/_cmd. Signature "v@:" = void(id,SEL). */
static void webIconDBSPI_checkIntegrityBeforeOpening(id self, SEL _cmd)
{
    (void)self;
    (void)_cmd;
}

@implementation WebIconDatabaseSPIInjector
+ (void)load
{
    Class webIconDB = objc_getClass("WebIconDatabase");
    if (!webIconDB)
        return; /* not present in this image (e.g. JSC/WebCore copies) */
    SEL sel = sel_registerName("_checkIntegrityBeforeOpening");
    if (class_getClassMethod(webIconDB, sel))
        return; /* already implemented — don't clobber a real future impl */
    Class meta = object_getClass(webIconDB);
    class_addMethod(meta, sel, (IMP)webIconDBSPI_checkIntegrityBeforeOpening, "v@:");
}
@end
