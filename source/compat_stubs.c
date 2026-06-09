/*
 * Compat stubs for APIs unavailable in the macOS 10.6 SDK runtime.
 * These provide minimal implementations so WebKit 604 can link.
 *
 * Missing symbols:
 *   _Block_has_signature, _Block_signature   (10.7+)
 *   _objc_destroyWeak, _objc_initWeak        (10.7+)
 *   _objc_loadWeak, _objc_storeWeak          (10.7+)
 *   _protocol_getMethodTypeEncoding           (10.7+)
 *   _g_globalDataPoison, _g_jitCodePoison, _g_nativeCodePoison  (JSC poison globals)
 *   strndup                                   (POSIX-2008, not in 10.6)
 */

#include <stdlib.h>
#include <string.h>
#include <objc/objc.h>

/* ── Block runtime extensions (10.7+) ── */

/* Block layout has a signature field at a known offset on 10.7+.
 * On 10.6, blocks don't carry signatures. Return 0 / NULL. */
int Block_has_signature(void *block) {
    (void)block;
    return 0;
}

const char *Block_signature(void *block) {
    (void)block;
    return NULL;
}

/* ── ObjC weak references (10.7+ ARC runtime) ── */
/* On 10.6 the runtime doesn't support true weak refs.
 * These stubs provide safe no-op / pass-through behavior. */

id objc_initWeak(id *addr, id val) {
    if (!val) { *addr = nil; return nil; }
    *addr = val;
    return val;
}

void objc_destroyWeak(id *addr) {
    *addr = nil;
}

id objc_loadWeak(id *addr) {
    return *addr;
}

id objc_storeWeak(id *addr, id val) {
    *addr = val;
    return val;
}

/* ── Protocol introspection (10.7+) ── */

struct objc_protocol;
const char *protocol_getMethodTypeEncoding(struct objc_protocol *p,
                                            BOOL isRequiredMethod,
                                            BOOL isInstanceMethod) {
    (void)p; (void)isRequiredMethod; (void)isInstanceMethod;
    return NULL;
}

/* ── strndup (POSIX-2008, missing on 10.6) ── */

char *strndup(const char *s, size_t n) {
    size_t len = strlen(s);
    if (len > n) len = n;
    char *dup = (char *)malloc(len + 1);
    if (!dup) return NULL;
    memcpy(dup, s, len);
    dup[len] = '\0';
    return dup;
}
