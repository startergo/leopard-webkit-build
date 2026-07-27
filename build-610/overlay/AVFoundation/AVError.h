#ifndef AVERROR_STUB_H
#define AVERROR_STUB_H
/* [leopard] AVFoundation/AVError.h shim — AVFoundation is 10.7+, absent on 10.6.
 * WebCoreNSErrorExtras.mm references only AVErrorUnknown. Real value never
 * crosses an ABI boundary (AVFoundation never loads on 10.6). */
#import <Foundation/Foundation.h>
typedef NS_ENUM(NSInteger, AVError) {
    AVErrorUnknown = -11800
};
#endif
