// [leopard-webkit-build] Stub <CommonCrypto/CommonRandom.h> for 10.6.
// CommonRandom is 10.10+; back CCRandomGenerateBytes with /dev/urandom (always
// present on 10.6, no Security.framework linkage needed in bmalloc).
#ifndef LEOPARD_COMMONRANDOM_STUB_H
#define LEOPARD_COMMONRANDOM_STUB_H
#include <CommonCrypto/CommonCryptoError.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#ifndef LEOPARD_CCRNG_DEFINED
#define LEOPARD_CCRNG_DEFINED 1
typedef int32_t CCRNGStatus;
enum { kCCRNGFailure = -1 };
#endif
static inline CCRNGStatus CCRandomGenerateBytes(void* bytes, size_t count) {
    FILE* f = fopen("/dev/urandom", "rb");
    if (!f) return kCCRNGFailure;
    size_t got = fread(bytes, 1, count, f);
    fclose(f);
    return (got == count) ? kCCSuccess : kCCRNGFailure;
}
#endif
