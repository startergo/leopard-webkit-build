#ifndef COREMEDIA_STUB_H
#define COREMEDIA_STUB_H
/*
 * Minimal CoreMedia/CoreMedia.h shim for the MacOSX 10.6 cross-build.
 *
 * CoreMedia.framework is 10.7+ and absent from the 10.6 SDK. PAL's SPI header
 * (pal/spi/cf/CoreMediaSPI.h) does `#include <CoreMedia/CoreMedia.h>`; this
 * umbrella provides every CM* type that WebKit's soft-link layer
 * (CoreMediaSoftLink.{h,cpp}) references in its dlsym function-pointer typedefs.
 * See CoreMedia/CMTime.h for the full rationale — at runtime every CoreMedia
 * function/constant is resolved via dlopen, so only the *types* are needed at
 * compile time and the media path is inert on 10.6 (ENABLE_VIDEO=0).
 *
 * Intentionally NOT defined here (CoreMediaSPI.h declares these itself for the
 * PLATFORM(COCOA) && !USE(APPLE_INTERNAL_SDK) branch, so a duplicate here would
 * be a conflicting redeclaration):
 *   CMNotificationCenterRef, CMNotificationCallback
 */
#include <CoreFoundation/CoreFoundation.h>   /* MacTypes scalars (SInt32/UInt32/SInt64/OSStatus/Boolean/Float64/FourCharCode) + CF types */
#include <CoreAudio/CoreAudioTypes.h>   /* AudioStreamBasicDescription, AudioBufferList, AudioStreamPacketDescription */
#include <AudioToolbox/AudioToolbox.h>  /* AudioChannelLayout */
#include <CoreGraphics/CGGeometry.h>    /* CGSize */
#include <CoreVideo/CoreVideo.h>        /* CVImageBufferRef (referenced by CoreMediaSoftLink.h) */

#include <CoreMedia/CMTime.h>

/* Opaque handle types — only ever passed around (and dlopen'd) by the soft-link
 * layer; never dereferenced in the compiled media path. */
typedef struct OpaqueCMBlockBuffer            *CMBlockBufferRef;
typedef const struct opaqueCMFormatDescription *CMFormatDescriptionRef;
typedef struct opaqueCMSampleBuffer           *CMSampleBufferRef;
// [leopard-webkit-build] In real CoreMedia, CMVideoFormatDescriptionRef and
// CMAudioFormatDescriptionRef are NOT distinct opaque structs — they are typedefs
// of CMFormatDescriptionRef (a video/audio format description IS a format
// description, passed interchangeably). Declaring them as separate opaque tags
// made MediaSampleAVFObjC.mm's call (CMFormatDescriptionRef arg ->
// CMVideoFormatDescriptionRef parameter) a type mismatch. Match the real types.
typedef CMFormatDescriptionRef                 CMVideoFormatDescriptionRef;
typedef CMFormatDescriptionRef                 CMAudioFormatDescriptionRef;
typedef struct OpaqueCMBufferQueue            *CMBufferQueueRef;
typedef struct OpaqueCMBuffer                 *CMBufferRef;
typedef struct OpaqueCMClock                  *CMClockRef;
typedef struct OpaqueCMTimebase               *CMTimebaseRef;
typedef struct OpaqueCMAttachmentBearer       *CMAttachmentBearerRef;
typedef struct OpaqueCMBufferQueueTriggerToken*CMBufferQueueTriggerToken;

/* CoreMedia scalar typedefs. FourCharCode comes from MacTypes.h. */
typedef signed long CMItemCount;
typedef signed long CMItemIndex;
typedef UInt32       CMAttachmentMode;
typedef FourCharCode CMMediaType;
typedef UInt32       CMTimeRoundingMethod;
typedef UInt32       CMBufferQueueTriggerCondition;

/* [leopard-webkit-build] CoreMedia compile-time constants consumed (as bare
 * values) by the AVFoundation media TUs that still compile under ENABLE_VIDEO=0
 * (e.g. MediaSampleAVFObjC.mm). CoreMedia.framework is 10.7+ and absent from the
 * 10.6 SDK; these are defined here as static constants so the dead-on-10.6 media
 * code compiles without a link dependency. Values match the modern CoreMedia SDK.
 *   kCMMediaType_Video                          = 'vide' (FourCharCode 0x76696465)
 *   kCMSampleBufferError_BufferHasNoSampleSizes = -12744 (CMSampleBufferError range) */
#ifndef kCMMediaType_Video
#define kCMMediaType_Video ((CMMediaType)'vide')
#endif
#ifndef kCMSampleBufferError_BufferHasNoSampleSizes
#define kCMSampleBufferError_BufferHasNoSampleSizes ((OSStatus)-12744)
#endif

/* Value structs accessed directly by callers. */
typedef struct {
    CMTime start;
    CMTime duration;
} CMTimeRange;

typedef struct {
    CMTime duration;
    CMTime presentationTimeStamp;
    CMTime decodeTimeStamp;
} CMSampleTimingInfo;

typedef struct {
    SInt32 width;
    SInt32 height;
} CMVideoDimensions;

/* Callback / callback-table types — referenced only as parameter types inside
 * the soft-link function-pointer typedefs; never dereferenced on 10.6, so a
 * minimal definition suffices. CMBufferCallbacks is left incomplete (it is only
 * ever used by-pointer in the soft-link signatures). */
typedef OSStatus (*CMSampleBufferMakeDataReadyCallback)(CMSampleBufferRef, void *);
typedef void     (*CMBufferQueueTriggerCallback)(CMBufferQueueTriggerToken, void *);
typedef struct CMBufferCallbacks CMBufferCallbacks;

#endif /* COREMEDIA_STUB_H */
