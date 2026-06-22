// [leopard] Unified wrapper to compile RemotePlayback.cpp with full context.
// RemotePlayback.cpp was removed from Sources.txt but WIRELESS_PLAYBACK_TARGET is ON,
// leaving HTMLMediaElement.cpp:456 referencing an undefined RemotePlayback vtable.
#include "config.h"
#include "HTMLNames.h"
#include "HTMLMediaElement.h"
#include "MediaElementSession.h"
#include "Modules/remoteplayback/RemotePlayback.cpp"
