/*
 * TargetConditionals_compat.h
 *
 * Compatibility shim for using the MacOSX 10.6 SDK with modern Clang.
 * Force-included via -include in CMAKE_C/CXX_FLAGS.
 *
 * Fixes:
 *  1. TARGET_* macros and __TARGETCONDITIONALS__ guard
 *  2. Architecture typedefs (__int32_t, __uint32_t, etc.)
 *  3. Availability macros (NS_CLASS_AVAILABLE, NS_AVAILABLE, etc.)
 *  4. GCD constants (DISPATCH_QUEUE_SERIAL)
 *  5. Objective-C dictionary subscripting (objectForKeyedSubscript:)
 *  6. Sandbox constant
 *  7. Post-10.6 runtime API stubs (objc_loadWeak, property_copyAttributeList, etc.)
 *  8. Missing C library functions (strndup)
 */

#ifndef TARGETCONDITIONALS_COMPAT_H
#define TARGETCONDITIONALS_COMPAT_H

/* NS_ASSUME_NONNULL_BEGIN/END — modern ObjC nullability annotations.
   These macros are no-ops; they just need to exist for the 10.6 SDK. */
#ifndef NS_ASSUME_NONNULL_BEGIN
#define NS_ASSUME_NONNULL_BEGIN
#endif
#ifndef NS_NOESCAPE
#define NS_NOESCAPE
#endif
#ifndef NS_ASSUME_NONNULL_END
#define NS_ASSUME_NONNULL_END
#endif

/* Pull in objc/objc.h early so BOOL, YES, NO are defined before any
   other header needs them. Then override YES/NO to use integer literals
   instead of (BOOL) casts — the SDK's definitions cause @YES / @NO to
   expand to @(BOOL)1 / @(BOOL)0, which the compiler can't parse as
   ObjC boxed expressions. */
#ifdef __OBJC__
#include <objc/objc.h>
#undef YES
#undef NO
#define YES  1
#define NO   0
#endif

/* ================================================================== */
/*  1. TARGET_OS_* / TARGET_CPU_* / TARGET_RT_*                      */
/* ================================================================== */

#define TARGET_OS_MAC               1
#define TARGET_OS_WIN32             0
#define TARGET_OS_UNIX              0
#define TARGET_OS_EMBEDDED          0
#define TARGET_OS_IPHONE            0
#define TARGET_IPHONE_SIMULATOR     0

#define TARGET_CPU_PPC              0
#define TARGET_CPU_PPC64            0
#define TARGET_CPU_68K              0
#define TARGET_CPU_X86              0
#define TARGET_CPU_X86_64           1
#define TARGET_CPU_ARM              0
#define TARGET_CPU_MIPS             0
#define TARGET_CPU_SPARC            0
#define TARGET_CPU_ALPHA            0

#define TARGET_RT_MAC_CFM           0
#define TARGET_RT_MAC_MACHO         1
#define TARGET_RT_LITTLE_ENDIAN     1
#define TARGET_RT_BIG_ENDIAN        0
#define TARGET_RT_64_BIT            1

/* Guard so SDK's TargetConditionals.h is a no-op */
#define __TARGETCONDITIONALS__

/* ================================================================== */
/*  2. Architecture typedefs (normally from machine/_types.h)          */
/* ================================================================== */

#ifndef __X86_64_TYPEDEFS_H
#define __X86_64_TYPEDEFS_H

#ifndef __int8_t_defined
#define __int8_t_defined
typedef signed char          __int8_t;
typedef unsigned char        __uint8_t;
typedef signed short         __int16_t;
typedef unsigned short       __uint16_t;
typedef signed int           __int32_t;
typedef unsigned int         __uint32_t;
typedef signed long long     __int64_t;
typedef unsigned long long   __uint64_t;
#endif /* __int8_t_defined */

typedef long                 __darwin_intptr_t;
typedef unsigned int         __darwin_natural_t;

#endif /* __X86_64_TYPEDEFS_H */

/* ================================================================== */
/*  3. Availability macros (missing from 10.6 SDK's NSObjCRuntime.h)  */
/* ================================================================== */

#ifndef NS_AVAILABLE
#define NS_AVAILABLE(_mac, _ios)
#endif

#ifndef NS_CLASS_AVAILABLE
#define NS_CLASS_AVAILABLE(_mac, _ios)
#endif

#ifndef NS_DEPRECATED
#define NS_DEPRECATED(_macIntro, _macDep, _iosIntro, _iosDep)
#endif

#ifndef NS_CLASS_DEPRECATED
#define NS_CLASS_DEPRECATED(_macIntro, _macDep, _iosIntro, _iosDep)
#endif

#ifndef NS_ENUM_AVAILABLE
#define NS_ENUM_AVAILABLE(_mac, _ios)
#endif

#ifndef NS_ENUM_DEPRECATED
#define NS_ENUM_DEPRECATED(_macIntro, _macDep, _iosIntro, _iosDep)
#endif

#ifndef API_AVAILABLE
#define API_AVAILABLE(...)
#endif

#ifndef API_DEPRECATED
#define API_DEPRECATED(...)
#endif

#ifndef API_UNAVAILABLE
#define API_UNAVAILABLE(...)
#endif

/* Version-specific availability macros used in WebKit */
#ifndef AVAILABLE_MAC_OS_X_VERSION_10_7_AND_LATER
#define AVAILABLE_MAC_OS_X_VERSION_10_7_AND_LATER
#endif
#ifndef AVAILABLE_MAC_OS_X_VERSION_10_8_AND_LATER
#define AVAILABLE_MAC_OS_X_VERSION_10_8_AND_LATER
#endif
#ifndef AVAILABLE_MAC_OS_X_VERSION_10_9_AND_LATER
#define AVAILABLE_MAC_OS_X_VERSION_10_9_AND_LATER
#endif
#ifndef AVAILABLE_MAC_OS_X_VERSION_10_10_AND_LATER
#define AVAILABLE_MAC_OS_X_VERSION_10_10_AND_LATER
#endif
#ifndef AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER
#define AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER
#endif

#ifndef DEPRECATED_IN_MAC_OS_X_VERSION_10_7_AND_LATER
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_7_AND_LATER
#endif
#ifndef DEPRECATED_IN_MAC_OS_X_VERSION_10_8_AND_LATER
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_8_AND_LATER
#endif
#ifndef DEPRECATED_IN_MAC_OS_X_VERSION_10_9_AND_LATER
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_9_AND_LATER
#endif
#ifndef DEPRECATED_IN_MAC_OS_X_VERSION_10_10_AND_LATER
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_10_AND_LATER
#endif

/* ================================================================== */
/*  4. GCD / dispatch constants and functions                          */
/* ================================================================== */

#ifndef DISPATCH_QUEUE_SERIAL
#define DISPATCH_QUEUE_SERIAL NULL
#endif

#include <dispatch/dispatch.h>
#ifndef DISPATCH_QUEUE_CONCURRENT
#define DISPATCH_QUEUE_CONCURRENT \
    ({ dispatch_queue_attr_t _a = (dispatch_queue_attr_t)0x80; _a; })
#endif

#ifndef DISPATCH_QUEUE_PRIORITY_BACKGROUND
#define DISPATCH_QUEUE_PRIORITY_BACKGROUND 0x08
#endif

/* dispatch_data_t (introduced 10.7) */
#ifndef dispatch_data_t
typedef dispatch_object_t dispatch_data_t;
#endif

/* dispatch_io_t (introduced 10.7) */
#ifndef dispatch_io_t
typedef dispatch_object_t dispatch_io_t;
#endif

/* dispatch_data API functions (introduced 10.7) */
#include <sys/types.h>  /* for off_t */
#include <dispatch/dispatch.h>

/* dispatch_fd_t (introduced in later dispatch headers) */
#ifndef dispatch_fd_t
typedef int dispatch_fd_t;
#endif

#ifndef _DISPATCH_DATA_COMPAT_DEFINED
#define _DISPATCH_DATA_COMPAT_DEFINED
#ifdef __cplusplus
extern "C" {
#endif

extern const const dispatch_block_t DISPATCH_DATA_DESTRUCTOR_DEFAULT;
extern dispatch_data_t dispatch_data_empty;

size_t dispatch_data_get_size(dispatch_data_t data);
dispatch_data_t dispatch_data_create(const void* buffer, size_t size, dispatch_queue_t queue, dispatch_block_t destructor);
dispatch_data_t dispatch_data_create_map(dispatch_data_t data, const void **buffer_ptr, size_t *size_ptr);
bool dispatch_data_apply(dispatch_data_t data, bool (^applier)(dispatch_data_t region, size_t offset, const void* buffer, size_t size));
dispatch_data_t dispatch_data_create_subrange(dispatch_data_t data, size_t offset, size_t size);
dispatch_data_t dispatch_data_create_concat(dispatch_data_t a, dispatch_data_t b);

/* dispatch_io API (introduced 10.7) */
#define DISPATCH_IO_RANDOM 0
typedef void (^dispatch_io_handler_t)(bool done, dispatch_data_t data, int error);

dispatch_io_t dispatch_io_create(int type, dispatch_fd_t fd, dispatch_queue_t queue, void (^cleanup_handler)(int error));
void dispatch_io_set_low_water(dispatch_io_t channel, size_t low_water);
void dispatch_io_read(dispatch_io_t channel, off_t offset, size_t length, dispatch_queue_t queue, dispatch_io_handler_t io_handler);
void dispatch_io_write(dispatch_io_t channel, off_t offset, dispatch_data_t data, dispatch_queue_t queue, dispatch_io_handler_t io_handler);

#ifdef __cplusplus
}
#endif
#endif /* _DISPATCH_DATA_COMPAT_DEFINED */

/* NSSecureCoding protocol (introduced 10.8) */
#ifdef __OBJC__
#ifndef NSSecureCoding
@protocol NSSecureCoding
@end
#endif
#endif

/* Mach VM constants (introduced 10.7) */
#ifndef VM_PROT_IS_MASK
#define VM_PROT_IS_MASK 0x40
#endif
#ifndef MAP_MEM_VM_SHARE
#define MAP_MEM_VM_SHARE 0x400000
#endif

/* dispatch_barrier_async / dispatch_barrier_sync (introduced 10.7) */
#ifndef _DISPATCH_BARRIER_COMPAT_DEFINED
#define _DISPATCH_BARRIER_COMPAT_DEFINED
static inline void dispatch_barrier_async_compat(dispatch_queue_t queue, dispatch_block_t block) {
    dispatch_async(queue, block);
}
static inline void dispatch_barrier_sync_compat(dispatch_queue_t queue, dispatch_block_t block) {
    dispatch_sync(queue, block);
}
#define dispatch_barrier_async dispatch_barrier_async_compat
#define dispatch_barrier_sync  dispatch_barrier_sync_compat
#endif

/* ================================================================== */
/*  5. Objective-C subscripting (dictionary + array, introduced 10.8) */
/* ================================================================== */

#ifdef __OBJC__
#import <Foundation/NSDictionary.h>
#import <Foundation/NSArray.h>

#ifndef _SUBSCRIPT_COMPAT_DEFINED
#define _SUBSCRIPT_COMPAT_DEFINED

@interface NSDictionary (SubscriptCompat)
- (id)objectForKeyedSubscript:(id)key;
@end

@interface NSMutableDictionary (SubscriptCompat)
- (void)setObject:(id)object forKeyedSubscript:(id)key;
@end

@interface NSArray (SubscriptCompat)
- (id)objectAtIndexedSubscript:(NSUInteger)idx;
@end

@interface NSMutableArray (SubscriptCompat)
- (void)setObject:(id)obj atIndexedSubscript:(NSUInteger)idx;
@end

#endif /* _SUBSCRIPT_COMPAT_DEFINED */
#endif /* __OBJC__ */

/* ================================================================== */
/*  6. Sandbox constant (introduced later, used in RemoteInspector)   */
/* ================================================================== */

#ifndef SANDBOX_CHECK_NO_REPORT
#define SANDBOX_CHECK_NO_REPORT 0
#endif

/* ================================================================== */
/*  7. Post-10.6 runtime API stubs                                     */
/* ================================================================== */

#ifdef __OBJC__

/* ---- NS_ROOT_CLASS (used on Protocol class declaration) ---- */
#ifndef NS_ROOT_CLASS
#define NS_ROOT_CLASS __attribute__((objc_root_class))
#endif

/* ---- NSPointerFunctionsWeakMemory (introduced 10.8) ---- */
#ifndef NSPointerFunctionsWeakMemory
#define NSPointerFunctionsWeakMemory NSPointerFunctionsOpaqueMemory
#endif

/* ---- Weak reference runtime helpers (introduced 10.7) ---- */
#ifndef _OBJC_WEAK_STUBS_DEFINED
#define _OBJC_WEAK_STUBS_DEFINED

static inline __attribute__((always_inline)) id _objc_loadWeak_compat(id *location) {
    return *location;
}
static inline __attribute__((always_inline)) id _objc_storeWeak_compat(id *location, id obj) {
    *location = obj;
    return obj;
}

#define objc_loadWeak       _objc_loadWeak_compat
#define objc_storeWeak      _objc_storeWeak_compat

#endif /* _OBJC_WEAK_STUBS_DEFINED */

/* ---- property_copyAttributeList / objc_property_attribute_t (10.7) ---- */
#ifndef _OBJC_PROPERTY_COMPAT_DEFINED
#define _OBJC_PROPERTY_COMPAT_DEFINED

#include <objc/runtime.h>
#include <stdlib.h>
#include <string.h>
#include <Availability.h>

#ifndef __MAC_10_7
typedef struct objc_property_attribute_t {
    const char *name;
    const char *value;
} objc_property_attribute_t;
#endif

OBJC_EXPORT objc_property_attribute_t *property_copyAttributeList(objc_property_t property,
                                                                   unsigned int *outCount);
OBJC_EXPORT void *objc_alloc(Class cls);

static inline objc_property_attribute_t *_property_copyAttributeList_compat(
    objc_property_t property, unsigned int *outCount)
{
    const char *attrs = property_getAttributes(property);
    if (!attrs || !outCount) { if (outCount) *outCount = 0; return NULL; }

    unsigned int count = 1;
    for (const char *p = attrs; *p; p++) if (*p == ',') count++;

    objc_property_attribute_t *list = (objc_property_attribute_t *)calloc(count, sizeof(objc_property_attribute_t));
    if (!list) { *outCount = 0; return NULL; }

    unsigned int i = 0;
    const char *p = attrs;

    if (*p) {
        list[i].name = "T";
        p++;
        const char *val_start = p;
        if (*p == '"') {
            p++;
            while (*p && *p != '"') p++;
            if (*p == '"') p++;
        }
        size_t vlen = p - val_start;
        char *val = (char *)malloc(vlen + 1);
        memcpy(val, val_start, vlen);
        val[vlen] = '\0';
        list[i].value = val;
        i++;
        if (*p == ',') p++;
    }

    while (*p && i < count) {
        char key[2] = { *p, '\0' };
        p++;
        const char *val_start = p;
        if (*p == '"') {
            p++;
            while (*p && *p != '"') p++;
            if (*p == '"') p++;
        }
        size_t vlen = p - val_start;
        char *val = (char *)malloc(vlen + 1);
        memcpy(val, val_start, vlen);
        val[vlen] = '\0';
        char *key_copy = (char *)malloc(2);
        key_copy[0] = key[0]; key_copy[1] = '\0';
        list[i].name = key_copy;
        list[i].value = val;
        i++;
        if (*p == ',') p++;
    }

    *outCount = i;
    return list;
}

#define property_copyAttributeList _property_copyAttributeList_compat

#endif /* _OBJC_PROPERTY_COMPAT_DEFINED */

/* ---- dyld_get_program_sdk_version (introduced 10.9ish) ---- */
#include <stdint.h>
#ifndef _DYLD_COMPAT_DEFINED
#define _DYLD_COMPAT_DEFINED

static inline uint32_t dyld_get_program_sdk_version(void) {
    return 0x000A0600; /* Report 10.6 */
}

#endif /* _DYLD_COMPAT_DEFINED */

/* ---- CFBridgingRetain / CFBridgingRelease (introduced 10.7) ---- */
#ifndef CFBridgingRetain
static inline CFTypeRef CFBridgingRetain_compat(id obj) {
    CFRetain((__bridge CFTypeRef)obj);
    return (__bridge CFTypeRef)obj;
}
#define CFBridgingRetain CFBridgingRetain_compat
#endif

#ifndef CFBridgingRelease
static inline id CFBridgingRelease_compat(CFTypeRef cf) {
    return (__bridge id)cf;
}
#define CFBridgingRelease CFBridgingRelease_compat
#endif

/* ---- NSCalendarUnit constants (renamed from kCFCalendarUnit in 10.10) ---- */
#ifndef NSCalendarUnitEra
#define NSCalendarUnitEra        kCFCalendarUnitEra
#define NSCalendarUnitYear       kCFCalendarUnitYear
#define NSCalendarUnitMonth      kCFCalendarUnitMonth
#define NSCalendarUnitDay        kCFCalendarUnitDay
#define NSCalendarUnitHour       kCFCalendarUnitHour
#define NSCalendarUnitMinute     kCFCalendarUnitMinute
#define NSCalendarUnitSecond     kCFCalendarUnitSecond
#define NSCalendarUnitWeekday    kCFCalendarUnitWeekday
#define NSCalendarUnitWeek       kCFCalendarUnitWeek
#endif

/* ---- Protocol creation APIs (introduced 10.7) ---- */
#ifndef _OBJC_PROTOCOL_COMPAT_DEFINED
#define _OBJC_PROTOCOL_COMPAT_DEFINED

static inline Protocol *objc_allocateProtocol_compat(const char *name) {
    return NULL;
}
static inline void protocol_addProtocol_compat(Protocol *proto, Protocol *addition) {}
static inline void protocol_addMethodDescription_compat(Protocol *proto, SEL name, const char *types,
                                                         BOOL isRequiredMethod, BOOL isInstanceMethod) {}
static inline void objc_registerProtocol_compat(Protocol *proto) {}

#define objc_allocateProtocol      objc_allocateProtocol_compat
#define protocol_addProtocol       protocol_addProtocol_compat
#define protocol_addMethodDescription protocol_addMethodDescription_compat
#define objc_registerProtocol      objc_registerProtocol_compat

#endif /* _OBJC_PROTOCOL_COMPAT_DEFINED */

#endif /* __OBJC__ */

/* ================================================================== */
/*  8. Missing C library functions                                     */
/* ================================================================== */

#ifndef _STRNDUP_COMPAT_DEFINED
#define _STRNDUP_COMPAT_DEFINED

#include <stdlib.h>
#include <string.h>

static inline char *strndup_compat(const char *s, size_t n) {
    size_t len = strlen(s);
    if (len > n) len = n;
    char *p = (char *)malloc(len + 1);
    if (p) { memcpy(p, s, len); p[len] = '\0'; }
    return p;
}

#define strndup strndup_compat

#endif /* _STRNDUP_COMPAT_DEFINED */

/* ---- Missing C99 math functions (llrint, llround etc. — not in 10.6 SDK) ---- */
/* NOTE: Do NOT #include <math.h> here. libc++'s math.h uses #include_next
   which skips already-included headers. If we include the SDK's math.h here,
   its include guard prevents libc++ from seeing the architecture-specific
   declarations. Just forward-declare the functions instead. */
#ifndef _MATH_COMPAT_DEFINED
#define _MATH_COMPAT_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

long long llrintf(float __x);
long long llrint(double __x);
long long llrintl(long double __x);
long long llroundf(float __x);
long long llround(double __x);
long long llroundl(long double __x);

#ifdef __cplusplus
}
#endif

#endif /* _MATH_COMPAT_DEFINED */

/* ---- XPC types and functions (introduced 10.7) ---- */
#ifndef _XPC_COMPAT_DEFINED
#define _XPC_COMPAT_DEFINED

#ifndef __XPC_INDIRECT__
typedef void *xpc_object_t;
typedef struct _xpc_connection_s *xpc_connection_t;

#ifdef __BLOCKS__
#ifdef __cplusplus
extern "C" {
#endif

extern xpc_object_t XPC_TYPE_DICTIONARY;
extern xpc_object_t XPC_TYPE_ERROR;
extern xpc_object_t XPC_ERROR_CONNECTION_INVALID;

xpc_connection_t xpc_connection_create_mach_service(const char *name, dispatch_queue_t targetq, uint64_t flags);
void xpc_connection_set_target_queue(xpc_connection_t connection, dispatch_queue_t queue);
void xpc_connection_set_event_handler(xpc_connection_t connection, void (^handler)(xpc_object_t));
void xpc_connection_resume(xpc_connection_t connection);
void xpc_connection_cancel(xpc_connection_t connection);
void xpc_connection_send_message(xpc_connection_t connection, xpc_object_t message);

void xpc_release(xpc_object_t object);
xpc_object_t xpc_retain(xpc_object_t object);
xpc_object_t xpc_get_type(xpc_object_t object);

xpc_object_t xpc_dictionary_create(const char * const *keys, const xpc_object_t *values, size_t count);
void xpc_dictionary_set_value(xpc_object_t xdict, const char *key, xpc_object_t value);
xpc_object_t xpc_dictionary_get_value(xpc_object_t xdict, const char *key);

#ifdef __cplusplus
}
#endif
#endif /* __BLOCKS__ */
#endif /* __XPC_INDIRECT__ */
#endif /* _XPC_COMPAT_DEFINED */

/* ================================================================== */
/*  9. IOPM assertion constants (renamed in 10.7+)                     */
/* ================================================================== */

#ifndef kIOPMAssertionTypePreventUserIdleDisplaySleep
#define kIOPMAssertionTypePreventUserIdleDisplaySleep CFSTR("PreventUserIdleDisplaySleep")
#endif

#ifndef kIOPMAssertionTypePreventUserIdleSystemSleep
#define kIOPMAssertionTypePreventUserIdleSystemSleep CFSTR("PreventUserIdleSystemSleep")
#endif

/* ================================================================== */
/*  10. NSCalendarIdentifier constants (introduced 10.10)              */
/* ================================================================== */

#ifdef __OBJC__
#ifndef NSCalendarIdentifierGregorian
#define NSCalendarIdentifierGregorian NSGregorianCalendar
#define NSCalendarIdentifierBuddhist    NSBuddhistCalendar
#define NSCalendarIdentifierChinese     NSChineseCalendar
#define NSCalendarIdentifierCoptic      @"coptic"
#define NSCalendarIdentifierEthiopicAmeteMihret @"ethiopic"
#define NSCalendarIdentifierEthiopicAmeteAlem @"ethiopic-amete-alem"
#define NSCalendarIdentifierHebrew      NSHebrewCalendar
#define NSCalendarIdentifierISO8601     @"iso8601"
#define NSCalendarIdentifierIndian      NSIndianCalendar
#define NSCalendarIdentifierIslamic     NSIslamicCalendar
#define NSCalendarIdentifierIslamicCivil NSIslamicCivilCalendar
#define NSCalendarIdentifierJapanese    NSJapaneseCalendar
#define NSCalendarIdentifierPersian     @"persian"
#define NSCalendarIdentifierRepublicOfChina @"roc"
#endif
#endif /* __OBJC__ */

/* ================================================================== */
/*  11-18. Additional compat (NSHTTPCookie, UTType, Audio, etc.)       */
/* ================================================================== */

#ifdef __OBJC__
#import <Foundation/Foundation.h>

@interface NSHTTPCookie (SnowLeopardCompat)
@property (nonatomic, readonly, copy) NSString *HTTPOnly;
@property (nonatomic, readonly) BOOL secure;
@property (nonatomic, readonly) BOOL sessionOnly;
@end

@interface NSURL (SnowLeopardFileURLCompat)
@property (nonatomic, readonly) BOOL fileURL;
@end

@interface NSFileManager (SnowLeopardCompat)
- (BOOL)createDirectoryAtURL:(NSURL *)url withIntermediateDirectories:(BOOL)createIntermediates attributes:(NSDictionary *)attributes error:(NSError **)error;
@end
#endif /* __OBJC__ */

#ifdef __OBJC__

/* NSEventModifierFlags (introduced 10.8, renamed from NSUInteger modifier flags) */
#ifndef NSEventModifierFlags
typedef NSUInteger NSEventModifierFlags;
#endif
/* NOTE: Do NOT #define NSEventModifierFlagCommand etc. here as macros that
   expand to NSCommandKeyMask etc. AppKitCompatibilityDeclarations.h uses
   "static const NSEventModifierFlags NSEventModifierFlagCommand = NSCommandKeyMask;"
   which would get macro-expanded to "static const ... NSCommandKeyMask = NSCommandKeyMask;"
   causing redefinition errors. AppKitCompatibilityDeclarations.h handles these. */

/* NSURLDownloadDelegate (introduced 10.7, informal protocol in 10.6) */
#ifndef NSURLDownloadDelegate
@protocol NSURLDownloadDelegate
@end
#endif

/* NSSharingServicePicker / NSSharingService (introduced 10.8):
   real @interface declarations live in the build/overlay-includes/AppKit/NSSharingService.h
   shim, found via -I on the <AppKit/NSSharingService.h> import.  NSSharingServicePickerStyle
   is defined by NSSharingServicePickerSPI.h's own NS_ENUM (do not pre-typedef it here). */
#ifndef NSSharingServiceDelegate
@protocol NSSharingServiceDelegate
@end
#endif

#endif /* __OBJC__ */

/* CoreText: the CTFontUIFontType enum was renumbered and its constants renamed in 10.7.
   The 10.6 SDK/runtime exposes kCTFontSystemFontType (=2) where modern (10.7+) code writes
   kCTFontUIFontSystem (=0).  Map the modern name to the 10.6 constant so the correct value
   reaches the 10.6 runtime; CTFont.h is included by the caller before use. */
#ifndef kCTFontUIFontSystem
#define kCTFontUIFontSystem kCTFontSystemFontType
#endif
/* Bold/emphasized system font: modern kCTFontUIFontEmphasizedSystem (=1 in the
   10.7+ enum) is the 10.6 kCTFontEmphasizedSystemFontType (=3).  Same mapping
   rationale as kCTFontUIFontSystem above. */
#ifndef kCTFontUIFontEmphasizedSystem
#define kCTFontUIFontEmphasizedSystem kCTFontEmphasizedSystemFontType
#endif

#ifdef __OBJC__
#import <CoreServices/CoreServices.h>
#ifndef _UTTYPE_COMPAT_DEFINED
#define _UTTYPE_COMPAT_DEFINED
static inline Boolean UTTypeIsDeclared_compat(CFStringRef inUTI) { (void)inUTI; return true; }
static inline Boolean UTTypeIsDynamic_compat(CFStringRef inUTI) { (void)inUTI; return false; }
#define UTTypeIsDeclared UTTypeIsDeclared_compat
#define UTTypeIsDynamic UTTypeIsDynamic_compat
#endif
#endif

#ifdef AudioObjectPropertyAddress
#ifndef AudioObjectPropertyListenerBlock
typedef void (^AudioObjectPropertyListenerBlock)(UInt32 inNumberAddresses, const AudioObjectPropertyAddress *inAddresses);
#endif
#ifndef AudioObjectAddPropertyListenerBlock
#define AudioObjectAddPropertyListenerBlock(id, addr, queue, block) do { (void)(queue); (void)(block); } while(0)
#define AudioObjectRemovePropertyListenerBlock(id, addr, queue, block) do { (void)(queue); (void)(block); } while(0)
#endif
#endif

#ifdef __OBJC__
#ifndef _OBJC_COLLECTING_ENABLED_STUB
#define _OBJC_COLLECTING_ENABLED_STUB
static inline BOOL objc_collectingEnabled_compat(void) { return NO; }
#define objc_collectingEnabled objc_collectingEnabled_compat
#endif
#endif

/* Disable OpenType Sanitizer — OTS library not available in build */
#ifdef USE_OPENTYPE_SANITIZER
#undef USE_OPENTYPE_SANITIZER
#endif

/* ================================================================== */
/*  20. Mach port guards, XPC functions, CFNetwork privates (10.7+)   */
/* ================================================================== */

#include <mach/mach.h>
#include <CoreFoundation/CoreFoundation.h>

#ifdef __cplusplus
extern "C" {
#endif

/* mach_port_guard/unguard (introduced 10.12) */
#ifndef mach_port_context_t
typedef uint64_t mach_port_context_t;
#endif
#ifndef MACH_PORT_DENAP_RECEIVER
#define MACH_PORT_DENAP_RECEIVER 0x00000010
#endif
kern_return_t mach_port_guard(ipc_space_t task, mach_port_name_t name, mach_port_context_t context, boolean_t strict);
kern_return_t mach_port_unguard(ipc_space_t task, mach_port_name_t name, mach_port_context_t context);

/* XPC functions (introduced 10.7) */
#ifndef _XPC_INDIRECT__
pid_t xpc_connection_get_pid(xpc_connection_t connection);
void xpc_connection_kill(xpc_connection_t connection, int reason);
void xpc_connection_get_audit_token(xpc_connection_t connection, audit_token_t *token);
#endif

/* DISPATCH_MACH_SEND_POSSIBLE */
#ifndef DISPATCH_MACH_SEND_POSSIBLE
#define DISPATCH_MACH_SEND_POSSIBLE 0x8000000000000000ULL
#endif

/* MACH_SEND_NOTIFY */
#ifndef MACH_SEND_NOTIFY
#define MACH_SEND_NOTIFY 0x00000001
#endif

/* QOS policy constants (10.10+ task_policy.h) — only define what 10.6 SDK lacks */
#ifndef TASK_BASE_QOS_POLICY
#define TASK_BASE_QOS_POLICY 0
#define TASK_OVERRIDE_QOS_POLICY 1
#define TASK_QOS_POLICY_COUNT 2
struct task_qos_policy {
    integer_t task_latency_qos_tier;
    integer_t task_throughput_qos_tier;
};
#define LATENCY_QOS_TIER_0 0
#define THROUGHPUT_QOS_TIER_0 0
#define LATENCY_QOS_TIER_UNSPECIFIED (-1)
#define THROUGHPUT_QOS_TIER_UNSPECIFIED (-1)
#endif

/* CFNetwork private SPI (_CFNetworkSetATSContext / _CFNetworkResetHSTSHostsSinceDate)
   are declared by WebCore's pal/spi/cf/CFNetworkSPI.h with their real signatures;
   no stub here — a CFTypeRef-typed stub would conflict with the real (CFDataRef/
   CFURLStorageSessionRef) declarations in TUs that include CFNetworkSPI.h. */

/* WKSetCrashReportApplicationSpecificInformation */
void WKSetCrashReportApplicationSpecificInformation(CFStringRef info);

#ifdef __cplusplus
}
#endif

/* OBJC_ASSOCIATION_* constants — ensure they exist */
#ifdef __OBJC__
#import <objc/runtime.h>
#ifndef OBJC_ASSOCIATION_RETAIN_NONATOMIC
#define OBJC_ASSOCIATION_ASSIGN 0
#define OBJC_ASSOCIATION_RETAIN_NONATOMIC 1
#define OBJC_ASSOCIATION_COPY_NONATOMIC 3
#define OBJC_ASSOCIATION_RETAIN 01401
#define OBJC_ASSOCIATION_COPY 01403
typedef uintptr_t objc_AssociationPolicy;
#endif
#endif

/* ── WebKit Cocoa API ──────────────────────────────────────────────────── */
/* The modern Cocoa API (WKWebView, WKBackForwardList, etc.) requires
   macOS 10.10+ and uses features unavailable in the 10.6 SDK:
   ObjC lightweight generics (NSArray<Type *>), NS_UNAVAILABLE, etc.
   Disable the entire Cocoa API layer so those headers compile as empty.
   The C API (WKPageRef, WKContextRef, …) does NOT use this guard.      */
#ifndef WK_API_ENABLED
#define WK_API_ENABLED 0
#endif

/* NS_UNAVAILABLE — not in 10.6 SDK (introduced 10.8) */
#ifndef NS_UNAVAILABLE
#define NS_UNAVAILABLE
#endif

/* ================================================================== */
/*  22. Missing AppKit constants and properties (10.7+)                */
/* ================================================================== */

#ifdef __OBJC__

/* NSScrollerStyle constants (introduced 10.7) — also defined as enum in NSScrollerImpSPI.h;
   #undef'd there before the enum block to avoid macro expansion conflicts */
#ifndef NSScrollerStyleLegacy
/* [leopard] removed (now enum member): #define NSScrollerStyleLegacy 0 */
#endif
#ifndef NSScrollerStyleOverlay
/* [leopard] removed (now enum member): #define NSScrollerStyleOverlay 1 */
#endif

/* NSEventPhase constants (introduced 10.7) */
#ifndef NSEventPhaseNone
#define NSEventPhaseNone        0
#endif
#ifndef NSEventPhaseBegan
#define NSEventPhaseBegan       1
#endif
#ifndef NSEventPhaseStationary
#define NSEventPhaseStationary  2
#endif
#ifndef NSEventPhaseChanged
#define NSEventPhaseChanged     4
#endif
#ifndef NSEventPhaseEnded
#define NSEventPhaseEnded       8
#endif
#ifndef NSEventPhaseCancelled
#define NSEventPhaseCancelled   16
#endif
#ifndef NSEventPhaseMayBegin
#define NSEventPhaseMayBegin    32
#endif

/* NSDraggingFormation (introduced 10.7) */
#ifndef NSDraggingFormationNone
#define NSDraggingFormationNone    0
#endif
#ifndef NSDraggingFormationList
#define NSDraggingFormationList    2
#endif

/* NSEventPhase typedef (introduced 10.7) */
#ifndef NSEventPhase
typedef NSUInteger NSEventPhase;
#endif

/* NSWindowOcclusionState (introduced 10.9) */
#ifndef NSWindowOcclusionStateVisible
#define NSWindowOcclusionStateVisible (1ULL << 1)
#endif

/* NSCorrectionResponse (introduced 10.7) */
#ifndef NSCorrectionResponseReverted
typedef NSInteger NSCorrectionResponse;
#define NSCorrectionResponseReverted 0
#define NSCorrectionResponseEdited 1
#define NSCorrectionResponseAccepted 2
#endif

/* NSWindowCollectionBehaviorFullScreenPrimary (introduced 10.7) */
#ifndef NSWindowCollectionBehaviorFullScreenPrimary
#define NSWindowCollectionBehaviorFullScreenPrimary (1 << 7)
#endif

/* CoreGraphics / CoreAnimation constants */
#ifndef kCGWindowImageNominalResolution
#define kCGWindowImageNominalResolution 0
#endif
// [leopard] kCAContextDisplayName declared extern by QuartzCoreSPI.h; do not macro-define.
// [leopard] kCAContextIgnoresHitTest declared extern by QuartzCoreSPI.h.
// [leopard] kCAContextDisplayId declared extern by QuartzCoreSPI.h.

/* mach vm_statistics submap user tags absent from the 10.6 SDK (added 10.9+).
   Values taken from <mach/vm_statistics.h> on modern macOS; identical across
   all SDKs. kCATransactionPhasePostCommit is intentionally NOT defined here --
   QuartzCoreSPI.h owns it via its CATransactionPhase enum. */
#ifndef VM_MEMORY_MALLOC_NANO
#define VM_MEMORY_MALLOC_NANO 11
#endif
#ifndef VM_MEMORY_UNSHARED_PMAP
#define VM_MEMORY_UNSHARED_PMAP 35
#endif
#ifndef VM_MEMORY_CORESERVICES
#define VM_MEMORY_CORESERVICES 43
#endif
#ifndef VM_MEMORY_IMAGEIO
#define VM_MEMORY_IMAGEIO 70
#endif
#ifndef VM_MEMORY_OS_ALLOC_ONCE
#define VM_MEMORY_OS_ALLOC_ONCE 73
#endif
#ifndef VM_MEMORY_LIBDISPATCH
#define VM_MEMORY_LIBDISPATCH 74
#endif

/* NSViewNoIntrinsicMetric (introduced 10.7) */
#ifndef NSViewNoIntrinsicMetric
#define NSViewNoIntrinsicMetric -1.0
#endif

/* NSFileWriteFileExistsError (10.7+; the 10.6 SDK's FoundationErrors.h jumps
   straight to NSFileWriteOutOfSpaceError=640). Value from modern Foundation. */
#ifndef NSFileWriteFileExistsError
#define NSFileWriteFileExistsError 516
#endif

/* NSFileManagerDelegate formal protocol is 10.7+.  On older SDKs declare an
   empty one so <NSFileManagerDelegate> conformance compiles (FileSystemCocoa.mm). */
#ifdef __OBJC__
#if __MAC_OS_X_VERSION_MAX_ALLOWED < 1070
@protocol NSFileManagerDelegate <NSObject>
@end
#endif
#endif

/* NSWindow notification constants (10.7+) */
#ifndef NSWindowDidChangeBackingPropertiesNotification
#define NSWindowDidChangeBackingPropertiesNotification @"NSWindowDidChangeBackingPropertiesNotification"
#endif
#ifndef NSWindowDidChangeOcclusionStateNotification
#define NSWindowDidChangeOcclusionStateNotification @"NSWindowDidChangeOcclusionStateNotification"
#endif
#ifndef NSBackingPropertyOldScaleFactorKey
#define NSBackingPropertyOldScaleFactorKey @"NSBackingPropertyOldScaleFactor"
#endif

/* NSWorkspace notification constants (10.8+) */
#ifndef NSWorkspaceAccessibilityDisplayOptionsDidChangeNotification
#define NSWorkspaceAccessibilityDisplayOptionsDidChangeNotification @"NSWorkspaceAccessibilityDisplayOptionsDidChangeNotification"
#endif

/* NSSpellChecker notification constants (10.7+) */
#ifndef NSSpellCheckerDidChangeAutomaticTextReplacementNotification
#define NSSpellCheckerDidChangeAutomaticTextReplacementNotification @"NSSpellCheckerDidChangeAutomaticTextReplacementNotification"
#endif
#ifndef NSSpellCheckerDidChangeAutomaticSpellingCorrectionNotification
#define NSSpellCheckerDidChangeAutomaticSpellingCorrectionNotification @"NSSpellCheckerDidChangeAutomaticSpellingCorrectionNotification"
#endif
#ifndef NSSpellCheckerDidChangeAutomaticQuoteSubstitutionNotification
#define NSSpellCheckerDidChangeAutomaticQuoteSubstitutionNotification @"NSSpellCheckerDidChangeAutomaticQuoteSubstitutionNotification"
#endif
#ifndef NSSpellCheckerDidChangeAutomaticDashSubstitutionNotification
#define NSSpellCheckerDidChangeAutomaticDashSubstitutionNotification @"NSSpellCheckerDidChangeAutomaticDashSubstitutionNotification"
#endif

/* NSWindowStyleMaskFullSizeContentView (introduced 10.10) */
#ifndef NSWindowStyleMaskFullSizeContentView
#define NSWindowStyleMaskFullSizeContentView (1 << 15)
#endif

/* NSUserInterfaceLayoutDirection — already in 10.6 SDK, no typedef needed */

#endif /* __OBJC__ */

/* ================================================================== */
/*  23. Additional XPC functions (ProcessLauncherMac needs these)      */
/* ================================================================== */

#ifndef _XPC_INDIRECT__
#ifdef __BLOCKS__
#ifdef __cplusplus
extern "C" {
#endif

xpc_connection_t xpc_connection_create(const char *name, dispatch_queue_t targetq);
void xpc_connection_set_oneshot_instance(xpc_connection_t connection, const unsigned char instance[16]);
xpc_object_t xpc_array_create(const xpc_object_t *objects, size_t count);
void xpc_array_set_value(xpc_object_t xarray, size_t index, xpc_object_t value);
void xpc_array_set_string(xpc_object_t xarray, size_t index, const char *string);
#define XPC_ARRAY_APPEND ((size_t)(-1))
void xpc_dictionary_set_string(xpc_object_t xdict, const char *key, const char *string);
void xpc_dictionary_set_mach_send(xpc_object_t xdict, const char *key, mach_port_t port);
void xpc_dictionary_set_fd(xpc_object_t xdict, const char *key, int fd);
void xpc_connection_set_bootstrap(xpc_connection_t connection, xpc_object_t bootstrap);
void xpc_connection_send_message_with_reply(xpc_connection_t connection, xpc_object_t message, dispatch_queue_t replyq, void (^handler)(xpc_object_t));
xpc_object_t xpc_string_create(const char *string);
xpc_object_t xpc_uint64_create(uint64_t value);
xpc_object_t xpc_bool_create(bool value);
void _CFBundleSetupXPCBootstrap(xpc_object_t bootstrap);

#ifdef __cplusplus
}
#endif
#endif /* __BLOCKS__ */
#endif /* _XPC_INDIRECT__ */

/* ================================================================== */
/*  24. _CFNetworkCopyATSContext (private SPI, 10.10+)                 */
/*                                                                     */
/*  Declared by WebCore's pal/spi/cf/CFNetworkSPI.h as returning       */
/*  CFDataRef.  No stub here — a CFTypeRef-typed stub would conflict   */
/*  with the real declaration.                                         */
/* ================================================================== */

/* ================================================================== */
/*  25. Lookup framework (private, 10.10+)                             */
/*                                                                     */
/*  The Lookup SPI (LULookupDefinitionModule + the soft-linked          */
/*  LookupLibrary()) is owned by WebCore's LookupSPI.h, which declares  */
/*  the full @interface and SOFT_LINK_PRIVATE_FRAMEWORK_OPTIONAL(Lookup)*/
/*  so the private framework is resolved at runtime (absent on 10.6 →   */
/*  nil class → dictionary/lookup gracefully disabled).  No compile-time*/
/*  stub belongs here: declaring one would duplicate the @interface and */
/*  the LookupLibrary() symbol, colliding with LookupSPI.h and breaking */
/*  DictionaryLookup.mm (its class methods would resolve to id).        */
/* ================================================================== */

#endif /* TARGETCONDITIONALS_COMPAT_H */
