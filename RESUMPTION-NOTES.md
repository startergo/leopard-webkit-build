# WebKit 610 / Snow Leopard — Resumption Notes (checkpoint e8dce24817)

## Status
- WebCore.framework LINKS (89MB, x86_64). Build is NOT clean-reproducible (full
  reconfigure reshuffles unified-sources, exposing latent include gaps in WTF/bmalloc).
- ~24 residual undefined symbols block WebCore dlopen. Categorized below.
- Work committed at e8dce24817 (safe from --clean).

## Build workflow that WORKS (incremental, avoid full reconfigure)
- Do NOT `rm stamps/cmake-configured` unless necessary (triggers cascade).
- Edit source -> targeted `ninja -C build-610 lib/WebCore.framework/...` rebuild.
- Probe: tar WebCore+JSC to sl@slqemu.local:/tmp/wk610, dlopen probe (RTLD_LAZY worth trying).
- SSH needs inline: -o HostKeyAlgorithms=+ssh-rsa -o PubkeyAcceptedAlgorithms=+ssh-rsa

## The 24 residual symbols (from `comm` of WebCore-undefined vs JSC-defined)
- AVF cluster (7): ImageDecoderAVFObjC::*, MediaPlayerPrivateAVFoundationObjC::registerMediaEngine,
  MediaSessionManagerCocoa::* -> guard callers MediaPlayer.cpp:246, ImageDecoder.cpp:52
- #if !PLATFORM(COCOA) generics (3): PlatformMediaSessionManager::create, LegacyWebArchive::* 
  -> .rej shows fix is at OUTER guard (line 33: add || PLATFORM(COCOA)), not just inner
- ResourceUsageThread::platform* (3) -> guard caller or feature-off (RESOURCE_USAGE=OFF broke WTF, avoid)
- Real stubs needed: AudioSession::* (3), CertificateInfo Coder+summaryInfo+type (4),
  DictionaryLookup::rangeAtHitTestResult (cocoa one should define-check), LocalDefaultSystemAppearance

## Fix guidance
- patches-610/rej-reference/ has the INTENDED fixes (rejected hunks) for many symbols.
- LEOPARD_WEBKIT macro (in Overlay605Supplement.h) is the durable 10.6-guard marker.
- Flags in build_610.sh -D block are authoritative. DATACUE_VALUE=OFF good.
  USE_AVFOUNDATION=OFF and RESOURCE_USAGE=OFF caused cascades - prefer caller-guards.

## Durability (the recurring concern)
- 163 files were uncommitted; NOW COMMITTED at e8dce24817.
- To make --clean reproduce: the committed tree IS the durable state now (phase2 git checkout
  restores to committed = our work). Overlay shims in patches-610/overlay/ durable.
- WTF/bmalloc clean-build breaks (missing <unistd.h>/<sys/param.h>, 10.7+ MAP_JIT/kMDItemDownloadedDate)
  need include-adds/guards IF pursuing clean reproducibility.

## WTF clean-build gaps (if making reproducible)
- ResourceUsageCocoa.cpp: needs <unistd.h> (_SC_PAGESIZE)
- FileSystemCocoa.mm: needs <sys/param.h> (MAXPATHLEN)
- OSAllocatorPOSIX.cpp: MAP_JIT is 10.7+ (guard)
- FileSystemMac.mm: kMDItemDownloadedDate is 10.7+ (guard)
- bmalloc DebugHeap.cpp: malloc_zone_pressure_relief guarded (DONE, LEOPARD_WEBKIT)
