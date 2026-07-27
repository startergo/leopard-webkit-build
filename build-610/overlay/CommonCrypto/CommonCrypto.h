/*
 * CommonCrypto/CommonCrypto.h — minimal shim for the MacOSX 10.6 SDK.
 *
 * CommonCrypto shipped as SPI (libcommonCrypto.dylib, part of libSystem) on
 * 10.6 but had no public SDK headers until 10.10.  PAL's
 * crypto/commoncrypto/CryptoDigestCommonCrypto.cpp includes
 * <CommonCrypto/CommonCrypto.h> unconditionally on Apple platforms, so we
 * provide the digest SPI surface it uses.
 *
 * The digest symbols (CC_SHA*_Init/Update/Final) are stable and present in the
 * 10.6 libcommonCrypto.dylib.  CryptoDigest treats the context structs purely
 * opaquely (new'd, passed by pointer, never inspected), so the opaque buffers
 * below only need to be AT LEAST as large as the real structs (CC_SHA1_CTX ~92
 * bytes, CC_SHA256_CTX ~104, CC_SHA512_CTX ~210); over-allocation is safe
 * because the libcommonCrypto functions write only within their fixed internal
 * offsets.
 */
#ifndef _COMMONCRYPTO_COMMONCRYPTO_H_SHIM_605
#define _COMMONCRYPTO_COMMONCRYPTO_H_SHIM_605

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef size_t CC_LONG;   /* CommonCrypto uses unsigned long; identical on x86_64 */

#define CC_SHA1_DIGEST_LENGTH    20
#define CC_SHA224_DIGEST_LENGTH  28
#define CC_SHA256_DIGEST_LENGTH  32
#define CC_SHA384_DIGEST_LENGTH  48
#define CC_SHA512_DIGEST_LENGTH  64

/* Over-sized opaque buffers (see file header). */
typedef struct { uint8_t _opaque[256]; } CC_SHA1_CTX;
typedef struct { uint8_t _opaque[256]; } CC_SHA256_CTX;
typedef struct { uint8_t _opaque[320]; } CC_SHA512_CTX;

int CC_SHA1_Init(CC_SHA1_CTX *c);
int CC_SHA1_Update(CC_SHA1_CTX *c, const void *data, CC_LONG len);
int CC_SHA1_Final(unsigned char *md, CC_SHA1_CTX *c);

int CC_SHA224_Init(CC_SHA256_CTX *c);
int CC_SHA224_Update(CC_SHA256_CTX *c, const void *data, CC_LONG len);
int CC_SHA224_Final(unsigned char *md, CC_SHA256_CTX *c);

int CC_SHA256_Init(CC_SHA256_CTX *c);
int CC_SHA256_Update(CC_SHA256_CTX *c, const void *data, CC_LONG len);
int CC_SHA256_Final(unsigned char *md, CC_SHA256_CTX *c);

int CC_SHA384_Init(CC_SHA512_CTX *c);
int CC_SHA384_Update(CC_SHA512_CTX *c, const void *data, CC_LONG len);
int CC_SHA384_Final(unsigned char *md, CC_SHA512_CTX *c);

int CC_SHA512_Init(CC_SHA512_CTX *c);
int CC_SHA512_Update(CC_SHA512_CTX *c, const void *data, CC_LONG len);
int CC_SHA512_Final(unsigned char *md, CC_SHA512_CTX *c);

#ifdef __cplusplus
}
#endif

#endif /* _COMMONCRYPTO_COMMONCRYPTO_H_SHIM_605 */
