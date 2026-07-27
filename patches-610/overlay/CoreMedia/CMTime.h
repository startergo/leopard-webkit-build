#ifndef COREMEDIA_CMTIME_STUB_H
#define COREMEDIA_CMTIME_STUB_H
/*
 * Minimal CoreMedia/CMTime.h shim for the MacOSX 10.6 cross-build.
 *
 * CoreMedia.framework is 10.7+ and entirely absent from the MacOSX 10.6 SDK,
 * but WebKit's PAL still references CMTime at compile time under
 * USE(COREMEDIA) / USE(AVFOUNDATION) (ClockCM.mm, MediaTimeAVFoundation.cpp,
 * CoreMediaSoftLink.cpp). Those translation units only need the *types* — every
 * CoreMedia function and constant they call is dlopen'd at runtime via PAL's
 * soft-link layer (CoreMediaSoftLink.{h,cpp}), so the whole media path is inert
 * on 10.6 (the build sets ENABLE_VIDEO=0 / ENABLE_WEB_AUDIO=0). This header
 * provides just the CMTime value struct and the flag enumerators that the code
 * reads and writes directly (cmTime.value / .timescale / .flags; kCMTimeFlags_*).
 *
 * Layout matches the public CoreMedia definition closely enough that the local
 * (in-process) construction/field access in MediaTimeAVFoundation.cpp compiles;
 * CMTime is never exchanged with a real CoreMedia on 10.6.
 */
#include <CoreFoundation/CoreFoundation.h>   /* MacTypes scalars (SInt32/UInt32/SInt64) */

typedef SInt64 CMTimeValue;
typedef SInt32 CMTimeScale;
typedef UInt32 CMTimeFlags;
typedef SInt64 CMTimeEpoch;

typedef struct {
    CMTimeValue value;
    CMTimeScale timescale;
    CMTimeFlags flags;
    CMTimeEpoch epoch;
} CMTime;

enum {
    kCMTimeFlags_Valid            = 1,
    kCMTimeFlags_HasBeenRounded   = 2,
    kCMTimeFlags_PositiveInfinity = 4,
    kCMTimeFlags_NegativeInfinity = 8,
    kCMTimeFlags_Indefinite       = 16,
};

#endif /* COREMEDIA_CMTIME_STUB_H */
