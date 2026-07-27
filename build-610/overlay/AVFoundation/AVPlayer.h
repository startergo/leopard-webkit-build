#ifndef AVPLAYER_STUB_H
#define AVPLAYER_STUB_H
/* [leopard] AVFoundation/AVPlayer.h shim — class is soft-linked via PAL
 * (PAL::getAVPlayerClass()); resolved at runtime, nil on 10.6. Interface
 * declared only so the soft-link macro and type references compile. */
#import <Foundation/Foundation.h>
@interface AVPlayer : NSObject
@end
#endif
