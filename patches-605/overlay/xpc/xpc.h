/*
 * xpc/xpc.h — minimal shim for the MacOSX 10.6 SDK (WebKit 605 build).
 *
 * The 10.6 SDK has no <xpc/xpc.h> (XPC is 10.7+), but wtf/spi/darwin/XPCSPI.h
 * does `#include <xpc/xpc.h>` unconditionally on PLATFORM(MAC) (line 32).  This
 * shim supplies exactly the pieces XPCSPI.h's MAC branch expects the real xpc.h
 * to provide:
 *
 *   * uuid_t                              — via <uuid/uuid.h> (in the 10.6 SDK).
 *   * xpc_object_t / xpc_connection_t     — already supplied by the shared
 *                                           overlay's TargetConditionals_compat.h
 *                                           (force-included before every TU);
 *                                           defined here too for self-containment.
 *   * xpc_type_t                          — typedef'd as void* to MATCH the
 *                                           overlay's xpc_object_t, so XPCSPI.h's
 *                                           `xpc_type_t xpc_get_type(...)` is
 *                                           identical to TC_compat's
 *                                           `xpc_object_t xpc_get_type(...)`.
 *   * block typedefs (xpc_handler_t etc.) — XPCSPI.h references them but only
 *                                           DEFINES them in its skipped non-MAC
 *                                           #else branch, so the shim provides.
 *
 * xpc_object_t / xpc_connection_t and the xpc_* function declarations come from
 * TC_compat (which also #defines _XPC_COMPAT_DEFINED); they are not redefined
 * here when that has already run.
 */
#ifndef _XPC_XPC_H_SHIM_605
#define _XPC_XPC_H_SHIM_605

#include <stddef.h>      /* size_t */
#include <uuid/uuid.h>   /* uuid_t */

#ifndef _XPC_COMPAT_DEFINED
typedef void *xpc_object_t;
typedef struct _xpc_connection_s *xpc_connection_t;
#endif

/* xpc_type_t — void* to match the overlay's xpc_object_t (avoids a return-type
   conflict between XPCSPI.h's and TC_compat's declarations of xpc_get_type). */
typedef void *xpc_type_t;

/* Block / function-pointer typedefs the real <xpc/xpc.h> declares under
   __BLOCKS__.  XPCSPI.h references these (xpc_array_applier_t :98,
   xpc_dictionary_applier_t :99, xpc_handler_t :111/:112,
   xpc_connection_handler_t :128) but defines them only in its skipped non-MAC
   #else branch. */
#if __BLOCKS__
typedef void (^xpc_handler_t)(xpc_object_t);
typedef bool (^xpc_array_applier_t)(size_t index, xpc_object_t);
typedef bool (^xpc_dictionary_applier_t)(const char *key, xpc_object_t);
#endif
typedef void (*xpc_connection_handler_t)(xpc_connection_t connection);

#endif /* _XPC_XPC_H_SHIM_605 */
