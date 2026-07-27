// [leopard] Stub replacement for MediaEngineConfigurationFactoryCocoa.cpp.
// Real impl uses VideoToolbox + HEVCUtilitiesCocoa (10.7+/10.13+), unavailable on 10.6.
// MediaCapabilities decoding-config is not needed for WebGL/core browsing; provide the symbol
// with the unsupported-configuration callback the real code uses for unsupported codecs.
#include "config.h"
#include "MediaEngineConfigurationFactoryCocoa.h"

#if PLATFORM(COCOA)

#include "MediaCapabilitiesDecodingInfo.h"
#include "MediaDecodingConfiguration.h"

namespace WebCore {

void createMediaPlayerDecodingConfigurationCocoa(MediaDecodingConfiguration&& configuration, WTF::Function<void(MediaCapabilitiesDecodingInfo&&)>&& callback)
{
    callback({{ }, WTFMove(configuration)});
}

}
#endif
