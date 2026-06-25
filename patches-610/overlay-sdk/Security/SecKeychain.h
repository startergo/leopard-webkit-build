// SecKeychain.h override for C/ObjC compilation
// Original uses static_cast which is C++ only

#ifndef SECKEYCHAIN_COMPAT_H
#define SECKEYCHAIN_COMPAT_H

#include <Security/SecItem.h>
#include <Security/SecBase.h>
#include <CoreFoundation/CoreFoundation.h>

enum SecAuthenticationTypeValues {
    kSecAuthenticationTypeNTLM             = 'ntlm',
    kSecAuthenticationTypeMSN              = 'msna',
    kSecAuthenticationTypeDPA              = 'dpaa',
    kSecAuthenticationTypeRPA              = 'rpaa',
    kSecAuthenticationTypeHTTPBasic        = 'http',
    kSecAuthenticationTypeHTTPDigest       = 'httd',
    kSecAuthenticationTypeHTMLForm         = 'form',
    kSecAuthenticationTypeDefault          = 'dflt',
    kSecAuthenticationTypeAny              = 0
};
typedef uint32_t SecAuthenticationType;

#endif /* SECKEYCHAIN_COMPAT_H */
