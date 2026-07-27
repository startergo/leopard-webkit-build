/* [leopard-webkit-build] Overlay: the 10.6 SDK's libxml2 encoding.h does
   `typedef uint16_t UChar;` under LIBXML_ICU_ENABLED, which conflicts with ICU's
   `typedef char16_t UChar;` (C++11). WebKit doesn't use libxml2's ICU converter, so
   suppress libxml2's UChar/UConverter block by pre-defining its trigger off, then pull
   the real header. We temporarily undefine LIBXML_ICU_ENABLED so the conflicting typedef
   is skipped; libxml2's other functionality is unaffected for WebKit's usage. */
#ifndef LEOPARD_LIBXML_ENCODING_OVERLAY_H
#define LEOPARD_LIBXML_ENCODING_OVERLAY_H
#pragma push_macro("LIBXML_ICU_ENABLED")
#undef LIBXML_ICU_ENABLED
#include_next <libxml/encoding.h>
#pragma pop_macro("LIBXML_ICU_ENABLED")
#endif
