#ifndef AVPLAYERLAYER_STUB_H
#define AVPLAYERLAYER_STUB_H
/* [leopard] AVFoundation/AVPlayerLayer.h shim — class soft-linked via PAL
 * (PAL::getAVPlayerLayerClass()), nil on 10.6. PlatformCALayerCocoa references
 * -player/-setPlayer: but only on layers of this class, which are never created
 * on 10.6 (the AVPlayerLayer code path is unreachable without the framework). */
#import <QuartzCore/QuartzCore.h>
#import <AVFoundation/AVPlayer.h>
@interface AVPlayerLayer : CALayer
@property (nonatomic, retain) AVPlayer *player;
@end
#endif
