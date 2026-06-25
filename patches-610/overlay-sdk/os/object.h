#ifndef OS_OBJECT_H_COMPAT
#define OS_OBJECT_H_COMPAT

#include <dispatch/dispatch.h>

#ifndef OS_OBJECT_USE_OBJC
#define OS_OBJECT_USE_OBJC 0
#endif

#ifndef os_object_t
typedef void *os_object_t;
#endif

#ifndef OS_OBJECT_DECL
#define OS_OBJECT_DECL(name) typedef void *name##_t
#endif

#ifndef os_retain
static inline void *os_retain(void *obj) { return obj; }
#endif

#ifndef os_release
static inline void os_release(void *obj) { (void)obj; }
#endif

#endif /* OS_OBJECT_H_COMPAT */
