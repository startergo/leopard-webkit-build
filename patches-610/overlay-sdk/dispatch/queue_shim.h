#ifndef DISPATCH_COMPAT_SHIM_H
#define DISPATCH_COMPAT_SHIM_H

#include_next <dispatch/dispatch.h>

#ifndef DISPATCH_QUEUE_SERIAL
#define DISPATCH_QUEUE_SERIAL NULL
#endif

#ifndef DISPATCH_QUEUE_CONCURRENT
#define DISPATCH_QUEUE_CONCURRENT \
    ({ dispatch_queue_attr_t _a; _a = (dispatch_queue_attr_t)_dispatch_queue_attr_concurrent; _a; })
#endif

#ifndef DISPATCH_BLOCK_ASSIGN_CONTEXT
#define DISPATCH_BLOCK_ASSIGN_CONTEXT 0
#endif

#ifndef DISPATCH_BLOCK_NO_FLAGS
#define DISPATCH_BLOCK_NO_FLAGS 0
#endif

#ifndef DISPATCH_QUEUE_PRIORITY_BACKGROUND
#define DISPATCH_QUEUE_PRIORITY_BACKGROUND 0x08
#endif

#endif /* DISPATCH_COMPAT_SHIM_H */
