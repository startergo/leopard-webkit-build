// [leopard-webkit-build] On 10.6, CCCryptorStatus / kCCSuccess live in CommonCryptor.h.
// Including that real header here provides them; do NOT redefine (causes redefinition errors).
#ifndef LEOPARD_COMMONCRYPTOERROR_STUB_H
#define LEOPARD_COMMONCRYPTOERROR_STUB_H
#include <CommonCrypto/CommonCryptor.h>
#endif
