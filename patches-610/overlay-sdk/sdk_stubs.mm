/*
 * sdk_stubs.mm - Missing symbols for MacOSX 10.6 SDK (ObjC++ linkage).
 */

// XPC typedefs — needed because sdk_stubs is compiled without -include TargetConditionals_compat.h
typedef struct _xpc_connection_s *xpc_connection_t;
typedef void *xpc_object_t;

#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>
#include <CoreFoundation/CoreFoundation.h>
#include <dispatch/dispatch.h>
#include <pthread.h>

#if __MAC_OS_X_VERSION_MAX_ALLOWED < 1070
@implementation NSFileManager (WebKit10_7Compat)
- (BOOL)createDirectoryAtURL:(NSURL *)url withIntermediateDirectories:(BOOL)createIntermediates attributes:(NSDictionary *)attributes error:(NSError **)error {
    return [self createDirectoryAtPath:[url path] withIntermediateDirectories:createIntermediates attributes:attributes error:error];
}
@end
#endif

// ── NSNotificationCenter — postNotificationOnMainThread (10.7+) ──
// WebKit 604 calls this frequently. Forward to the synchronous 10.6 API.
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

#include <objc/objc.h>
#include <stdlib.h>

extern "C" {

CFTypeRef _CFXPCCreateCFObjectFromXPCMessage(void *xpc_message) {
    (void)xpc_message; return NULL;
}
CFTypeRef _CFXPCCreateXPCMessageWithCFObject(CFTypeRef cf) {
    (void)cf; return NULL;
}

/* XPC type globals — names must match TargetConditionals_compat.h declarations */
static int xpc_type_dictionary_val = 0;
static int xpc_type_error_val = 0;
static int xpc_error_connection_invalid_val = 0;

void *XPC_TYPE_DICTIONARY = &xpc_type_dictionary_val;
void *XPC_TYPE_ERROR = &xpc_type_error_val;
void *XPC_ERROR_CONNECTION_INVALID = &xpc_error_connection_invalid_val;

typedef void *xpc_object_t;

xpc_object_t xpc_dictionary_create(const char * const *keys, const xpc_object_t *values, size_t count) {
    (void)keys; (void)values; (void)count; return NULL;
}
xpc_object_t xpc_dictionary_get_value(xpc_object_t xdict, const char *key) {
    (void)xdict; (void)key; return NULL;
}
void xpc_dictionary_set_value(xpc_object_t xdict, const char *key, xpc_object_t value) {
    (void)xdict; (void)key; (void)value;
}
xpc_object_t xpc_get_type(xpc_object_t obj) { (void)obj; return NULL; }
xpc_object_t xpc_retain(xpc_object_t obj) { return obj; }
void xpc_release(xpc_object_t obj) { (void)obj; }
xpc_connection_t xpc_connection_create_mach_service(const char *name, dispatch_queue_t targetq, uint64_t flags) {
    (void)name; (void)targetq; (void)flags; return NULL;
}
void xpc_connection_set_event_handler(xpc_connection_t conn, void (^handler)(xpc_object_t)) {
    (void)conn; (void)handler;
}
void xpc_connection_resume(xpc_connection_t conn) { (void)conn; }
void xpc_connection_cancel(xpc_connection_t conn) { (void)conn; }
void xpc_connection_send_message(xpc_connection_t conn, xpc_object_t msg) { (void)conn; (void)msg; }
void xpc_connection_send_message_with_reply(xpc_connection_t conn, xpc_object_t msg, dispatch_queue_t queue, void (^handler)(xpc_object_t)) {
    (void)conn; (void)msg; (void)queue; if (handler) handler(NULL);
}
xpc_connection_t xpc_connection_create(const char *name, dispatch_queue_t targetq) {
    (void)name; (void)targetq; return NULL;
}
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
int sandbox_check(pid_t pid, const char *operation, enum sandbox_filter_type type, ...) {
    (void)pid; (void)operation; (void)type; return 0;
}

// Sandbox extension constants (10.7+, from libWebKitSystemInterface)
const char *APP_SANDBOX_READ = "com.apple.app-sandbox.read";
const char *APP_SANDBOX_READ_WRITE = "com.apple.app-sandbox.read-write";

// Additional sandbox functions referenced by WebKit
int sandbox_check_by_audit_token(audit_token_t token, const char *operation, enum sandbox_filter_type type, ...) {
    (void)token; (void)operation; (void)type; return 0;
}
int sandbox_container_path_for_pid(pid_t pid, char *buffer, size_t bufsize) {
    (void)pid; (void)buffer; (void)bufsize; return -1;
}
int sandbox_init_with_parameters(const char *profile, uint64_t flags, const char *const parameters[], char **errorbuf) {
    (void)profile; (void)flags; (void)parameters; if (errorbuf) *errorbuf = NULL; return 0;
}
int sandbox_extension_consume(const char *extension_token) { (void)extension_token; return -1; }
char *sandbox_extension_issue_file(const char *extension_class, const char *path, int flags, int *error) {
    (void)extension_class; (void)path; (void)flags; if (error) *error = 0; return NULL;
}
char *sandbox_extension_issue_generic(const char *extension_class, int flags, int *error) {
    (void)extension_class; (void)flags; if (error) *error = 0; return NULL;
}
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

// DISPATCH_DATA_DESTRUCTOR_DEFAULT — must be a global export, not local.
// Use extern declaration + definition to ensure it appears as 'D' not 'd'.
extern const dispatch_block_t DISPATCH_DATA_DESTRUCTOR_DEFAULT;
const dispatch_block_t DISPATCH_DATA_DESTRUCTOR_DEFAULT __attribute__((used, visibility("default"))) = (dispatch_block_t)__dispatch_data_destructor_noop;
dispatch_data_t dispatch_data_empty_val = NULL;
dispatch_data_t dispatch_data_empty = NULL;
dispatch_data_t dispatch_data_create(const void *buffer, size_t size, dispatch_queue_t queue, dispatch_block_t destructor) {
    (void)buffer; (void)size; (void)queue; (void)destructor; return NULL;
}
dispatch_data_t dispatch_data_create_map(dispatch_data_t data, const void **buffer_ptr, size_t *size_ptr) {
    (void)data; if (buffer_ptr) *buffer_ptr = NULL; if (size_ptr) *size_ptr = 0; return NULL;
}
bool dispatch_data_apply(dispatch_data_t data, bool (^applier)(dispatch_data_t, size_t, const void*, size_t)) {
    (void)data; (void)applier; return false;
}
dispatch_data_t dispatch_data_create_subrange(dispatch_data_t data, size_t offset, size_t size) {
    (void)data; (void)offset; (void)size; return NULL;
}
dispatch_data_t dispatch_data_create_concat(dispatch_data_t a, dispatch_data_t b) {
    (void)a; (void)b; return NULL;
}
size_t dispatch_data_get_size(dispatch_data_t data) { (void)data; return 0; }
dispatch_io_t dispatch_io_create(int type, dispatch_fd_t fd, dispatch_queue_t queue, void (^cleanup_handler)(int error)) {
    (void)type; (void)fd; (void)queue; (void)cleanup_handler; return NULL;
}
void dispatch_io_set_low_water(dispatch_io_t channel, size_t low_water) { (void)channel; (void)low_water; }
void dispatch_io_read(dispatch_io_t channel, off_t offset, size_t length, dispatch_queue_t queue, dispatch_io_handler_t io_handler) {
    (void)channel; (void)offset; (void)length; (void)queue; if (io_handler) io_handler(true, NULL, 0);
}
void dispatch_io_write(dispatch_io_t channel, off_t offset, dispatch_data_t data, dispatch_queue_t queue, dispatch_io_handler_t io_handler) {
    (void)channel; (void)offset; (void)data; (void)queue; if (io_handler) io_handler(true, NULL, 0);
}
} /* extern "C" */

/* Private AppKit symbols (10.7+) not in 10.6 */
extern "C" {
const char* NSPopUpMenuPopupButtonWidget = "popup";
float NSPopUpMenuPopupButtonBounds[4] = {0, 0, 100, 24};
float NSPopUpMenuPopupButtonLabelOffset = 0;
float NSPopUpMenuPopupButtonSize[2] = {100, 24};
float _NSElasticDeltaForTimeDelta(float axis, float delta, float velocity, float size) {
    (void)axis; (void)velocity; (void)size; return delta;
}
float _NSElasticDeltaForReboundDelta(float delta) { return delta; }
float _NSReboundDeltaForElasticDelta(float delta) { return delta; }
int _NSRecommendedScrollerStyle = 0;
void NSInitializeCGFocusRingStyleForTime(int *style, float time, int *color, int *enabled) {
    (void)time; if (style) *style = 0; if (enabled) *enabled = 0;
}
} /* extern "C" */

/* Missing ObjC classes (10.7+) — referenced by libWebKitSystemInterfaceElCapitan.a */
@interface CASpringAnimation : CAAnimation
@end
@implementation CASpringAnimation
@end
@interface NSLayoutConstraint : NSObject
@end
@implementation NSLayoutConstraint
@end
@interface NSPopoverColorWell : NSObject
@end
@implementation NSPopoverColorWell
@end
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
const char* kCAFilterColorInvert = "colorInvert";
const char* kCAFilterGaussianBlur = "gaussianBlur";
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
void xpc_connection_set_target_queue(struct _xpc_connection_s *conn, struct dispatch_queue_s *queue) {
    (void)conn; (void)queue;
}
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
namespace JSC {
void initializePoison() {}
}

// CABackdropLayer is 10.10+. Provide a minimal ObjC class stub.
@interface CABackdropLayer : CALayer
@end
@implementation CABackdropLayer
@end

// CAFilter is a private QuartzCore class (10.7+) used for layer filters.
// Used by PlatformCAFiltersCocoa.mm: [CAFilter filterWithType:...]
// NSScrollerImp and NSScrollerImpPair are 10.7+ overlay scrollbar classes.
// ScrollAnimatorMac.mm uses them for overlay scrollbar animation.
typedef NSInteger NSScrollerStyle;

@interface NSScrollerImp : NSObject
@property (assign) id delegate;
@property CGFloat knobAlpha;
@property CGFloat trackAlpha;
@property CGFloat uiStateTransitionProgress;
@property CGFloat expansionTransitionProgress;
@property BOOL usePresentationValue;
@property (retain) CALayer *layer;
@property BOOL needsDisplay;
@property BOOL tracking;
@property BOOL isHorizontal;
- (NSRect)rectForPart:(NSInteger)part;
- (void)mouseEnteredScroller;
- (void)mouseExitedScroller;
- (void)setNeedsDisplay:(BOOL)flag;
@end
@implementation NSScrollerImp
- (NSRect)rectForPart:(NSInteger)part { (void)part; return NSZeroRect; }
- (void)mouseEnteredScroller {}
- (void)mouseExitedScroller {}
- (void)setNeedsDisplay:(BOOL)flag { (void)flag; self.needsDisplay = YES; }
@end

@protocol NSScrollerImpPairDelegate <NSObject>
@optional
- (NSRect)contentAreaRectForScrollerImpPair:(id)scrollerImpPair;
- (BOOL)inLiveResizeForScrollerImpPair:(id)scrollerImpPair;
- (NSPoint)mouseLocationInContentAreaForScrollerImpPair:(id)scrollerImpPair;
- (NSPoint)scrollerImpPair:(id)pair convertContentPoint:(NSPoint)point toScrollerImp:(id)imp;
- (void)scrollerImpPair:(id)pair setContentAreaNeedsDisplayInRect:(NSRect)rect;
- (void)scrollerImpPair:(id)pair updateScrollerStyleForNewRecommendedScrollerStyle:(NSScrollerStyle)style;
@end

@interface NSScrollerImpPair : NSObject
@property (assign) id delegate;
@property NSScrollerStyle scrollerStyle;
@property (retain) NSScrollerImp *verticalScrollerImp;
@property (retain) NSScrollerImp *horizontalScrollerImp;
- (void)flashScrollers;
- (void)hideOverlayScrollers;
- (void)beginScrollGesture;
- (void)endScrollGesture;
- (void)contentAreaDidResize;
- (void)contentAreaScrolled;
- (void)contentAreaScrolledInDirection:(NSPoint)direction;
- (void)contentAreaWillDraw;
- (BOOL)overlayScrollerStateIsLocked;
- (void)lockOverlayScrollerState:(NSInteger)state;
- (void)unlockOverlayScrollerState;
- (void)mouseEnteredContentArea;
- (void)mouseExitedContentArea;
- (void)mouseMovedInContentArea;
- (void)startLiveResize;
- (void)endLiveResize;
- (void)windowOrderedIn;
- (void)windowOrderedOut;
@end
@implementation NSScrollerImpPair
- (void)flashScrollers {}
- (void)hideOverlayScrollers {}
- (void)beginScrollGesture {}
- (void)endScrollGesture {}
- (void)contentAreaDidResize {}
- (void)contentAreaScrolled {}
- (void)contentAreaScrolledInDirection:(NSPoint)direction { (void)direction; }
- (void)contentAreaWillDraw {}
- (BOOL)overlayScrollerStateIsLocked { return NO; }
- (void)lockOverlayScrollerState:(NSInteger)state { (void)state; }
- (void)unlockOverlayScrollerState {}
- (void)mouseEnteredContentArea {}
- (void)mouseExitedContentArea {}
- (void)mouseMovedInContentArea {}
- (void)startLiveResize {}
- (void)endLiveResize {}
- (void)windowOrderedIn {}
- (void)windowOrderedOut {}
@end

/* WebHostedNetscapePluginView — out-of-process plugin view (XPC, 10.7+).
   Disabled in CMake build but referenced by WebKitLegacy plugin code. */
@interface WebHostedNetscapePluginView : NSObject
@end
@implementation WebHostedNetscapePluginView
@end

/* WebKeyGenerator — SSL key generation helper */
@interface WebKeyGenerator : NSObject
@end
@implementation WebKeyGenerator
@end

/* WebRenderNode — rendering tree debug helper */
@interface WebRenderNode : NSObject
@end
@implementation WebRenderNode
@end

/* WebSerializedJSValue — JS value serialization */
@interface WebSerializedJSValue : NSObject
@end
@implementation WebSerializedJSValue
@end

/* C++ mangled sandbox_check — called from JSC code that did not see extern "C" */
extern "C" void __sandbox_check_cpp_stub(void) {}
asm(".globl __Z13sandbox_checkiPKc19sandbox_filter_typez");
asm(".set __Z13sandbox_checkiPKc19sandbox_filter_typez, ___sandbox_check_cpp_stub");
