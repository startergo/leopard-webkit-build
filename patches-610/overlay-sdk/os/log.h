// [leopard-webkit-build] Stub <os/log.h> for Mac OS X 10.6.
// os_log (the unified logging system) is 10.12+. WebKit uses it only for
// assertion/debug logging; on 10.6 we no-op it. All macros expand to nothing
// or to harmless fprintf-free stubs so BAssert.h and the ~22 os_log call sites
// compile and link without the real framework.
#ifndef LEOPARD_OS_LOG_STUB_H
#define LEOPARD_OS_LOG_STUB_H
#include <stdint.h>

typedef struct os_log_s* os_log_t;
#define OS_LOG_DEFAULT ((os_log_t)0)
typedef uint8_t os_log_type_t;
#define OS_LOG_TYPE_DEFAULT 0
#define OS_LOG_TYPE_INFO    1
#define OS_LOG_TYPE_DEBUG   2
#define OS_LOG_TYPE_ERROR   16
#define OS_LOG_TYPE_FAULT   17

static inline os_log_t os_log_create(const char* subsystem, const char* category) {
    (void)subsystem; (void)category; return OS_LOG_DEFAULT;
}
static inline int os_log_type_enabled(os_log_t log, int type) {
    (void)log; (void)type; return 0;
}
// Variadic logging macros → no-ops (the args are evaluated nowhere).
#define os_log(log, format, ...)            do { (void)(log); } while (0)
#define os_log_info(log, format, ...)       do { (void)(log); } while (0)
#define os_log_debug(log, format, ...)      do { (void)(log); } while (0)
#define os_log_error(log, format, ...)      do { (void)(log); } while (0)
#define os_log_fault(log, format, ...)      do { (void)(log); } while (0)
#define os_log_with_type(log, type, fmt, ...) do { (void)(log); (void)(type); } while (0)

#endif
