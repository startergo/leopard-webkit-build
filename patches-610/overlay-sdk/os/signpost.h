// [leopard-webkit-build] Stub <os/signpost.h> for 10.6. Signposts are 10.12+
// performance-tracing; WebKit's SystemTracing.h uses them for instrumentation
// only. No-op everything.
#ifndef LEOPARD_OS_SIGNPOST_STUB_H
#define LEOPARD_OS_SIGNPOST_STUB_H
#include <os/log.h>
typedef uint64_t os_signpost_id_t;
#define OS_SIGNPOST_ID_NULL    ((os_signpost_id_t)0)
#define OS_SIGNPOST_ID_INVALID ((os_signpost_id_t)~0)
#define OS_SIGNPOST_ID_EXCLUSIVE ((os_signpost_id_t)0xEEEEB0B5B2B2EEEEULL)
static inline os_signpost_id_t os_signpost_id_generate(os_log_t l) { (void)l; return 1; }
static inline os_signpost_id_t os_signpost_id_make_with_pointer(os_log_t l, const void* p) { (void)l; (void)p; return 1; }
static inline int os_signpost_enabled(os_log_t l) { (void)l; return 0; }
#define os_signpost_interval_begin(log, id, name, ...) do { (void)(log); (void)(id); } while (0)
#define os_signpost_interval_end(log, id, name, ...)   do { (void)(log); (void)(id); } while (0)
#define os_signpost_event_emit(log, id, name, ...)     do { (void)(log); (void)(id); } while (0)
#define os_signpost_animation_interval_begin(log, id, name, ...) do { (void)(log); (void)(id); } while (0)
#endif
