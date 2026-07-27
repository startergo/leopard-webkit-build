/*
 * THIS FILE WAS AUTOMATICALLY GENERATED, DO NOT EDIT.
 *
 * Copyright (C) 2011 Google Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY GOOGLE, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

namespace WebCore {

enum EventTargetInterface {
#if ENABLE(APPLE_PAY)
    ApplePaySessionEventTargetInterfaceType = 1,
#endif
#if ENABLE(CSS_PAINTING_API)
    WorkletGlobalScopeEventTargetInterfaceType = 2,
#endif
#if ENABLE(ENCRYPTED_MEDIA)
    MediaKeySessionEventTargetInterfaceType = 3,
#endif
#if ENABLE(INDEXED_DATABASE)
    IDBDatabaseEventTargetInterfaceType = 4,
    IDBOpenDBRequestEventTargetInterfaceType = 5,
    IDBRequestEventTargetInterfaceType = 6,
    IDBTransactionEventTargetInterfaceType = 7,
#endif
#if ENABLE(LEGACY_ENCRYPTED_MEDIA)
    WebKitMediaKeySessionEventTargetInterfaceType = 8,
#endif
#if ENABLE(MEDIA_SESSION)
    MediaRemoteControlsEventTargetInterfaceType = 9,
#endif
#if ENABLE(MEDIA_SOURCE)
    MediaSourceEventTargetInterfaceType = 10,
    SourceBufferEventTargetInterfaceType = 11,
    SourceBufferListEventTargetInterfaceType = 12,
#endif
#if ENABLE(MEDIA_STREAM)
    MediaDevicesEventTargetInterfaceType = 13,
    MediaRecorderEventTargetInterfaceType = 14,
    MediaStreamEventTargetInterfaceType = 15,
    MediaStreamTrackEventTargetInterfaceType = 16,
#endif
#if ENABLE(NOTIFICATIONS)
    NotificationEventTargetInterfaceType = 17,
#endif
#if ENABLE(OFFSCREEN_CANVAS)
    OffscreenCanvasEventTargetInterfaceType = 18,
#endif
#if ENABLE(PAYMENT_REQUEST)
    PaymentRequestEventTargetInterfaceType = 19,
    PaymentResponseEventTargetInterfaceType = 20,
#endif
#if ENABLE(PICTURE_IN_PICTURE_API)
    PictureInPictureWindowEventTargetInterfaceType = 21,
#endif
#if ENABLE(SERVICE_WORKER)
    ServiceWorkerEventTargetInterfaceType = 22,
    ServiceWorkerContainerEventTargetInterfaceType = 23,
    ServiceWorkerGlobalScopeEventTargetInterfaceType = 24,
    ServiceWorkerRegistrationEventTargetInterfaceType = 25,
#endif
#if ENABLE(SPEECH_SYNTHESIS)
    SpeechSynthesisUtteranceEventTargetInterfaceType = 26,
#endif
#if ENABLE(VIDEO)
    MediaControllerEventTargetInterfaceType = 27,
#endif
#if ENABLE(VIDEO_TRACK)
    AudioTrackListEventTargetInterfaceType = 28,
    TextTrackEventTargetInterfaceType = 29,
    TextTrackCueEventTargetInterfaceType = 30,
    TextTrackListEventTargetInterfaceType = 31,
    VideoTrackListEventTargetInterfaceType = 32,
#endif
#if ENABLE(WEBGPU)
    WebGPUDeviceEventTargetInterfaceType = 33,
#endif
#if ENABLE(WEBXR)
    WebXRSessionEventTargetInterfaceType = 34,
    WebXRSpaceEventTargetInterfaceType = 35,
    WebXRSystemEventTargetInterfaceType = 36,
#endif
#if ENABLE(WEB_AUDIO)
    AudioContextEventTargetInterfaceType = 37,
    AudioNodeEventTargetInterfaceType = 38,
#endif
#if ENABLE(WEB_RTC)
    RTCDTMFSenderEventTargetInterfaceType = 39,
    RTCDataChannelEventTargetInterfaceType = 40,
    RTCPeerConnectionEventTargetInterfaceType = 41,
#endif
#if ENABLE(WIRELESS_PLAYBACK_TARGET)
    RemotePlaybackEventTargetInterfaceType = 42,
#endif
    EventTargetInterfaceType = 43,
    AbortSignalEventTargetInterfaceType = 44,
    ClipboardEventTargetInterfaceType = 45,
    DOMApplicationCacheEventTargetInterfaceType = 46,
    DOMWindowEventTargetInterfaceType = 47,
    DedicatedWorkerGlobalScopeEventTargetInterfaceType = 48,
    EventSourceEventTargetInterfaceType = 49,
    FileReaderEventTargetInterfaceType = 50,
    FontFaceSetEventTargetInterfaceType = 51,
    MediaQueryListEventTargetInterfaceType = 52,
    MessagePortEventTargetInterfaceType = 53,
    NodeEventTargetInterfaceType = 54,
    PerformanceEventTargetInterfaceType = 55,
    VisualViewportEventTargetInterfaceType = 56,
    WebAnimationEventTargetInterfaceType = 57,
    WebSocketEventTargetInterfaceType = 58,
    WorkerEventTargetInterfaceType = 59,
    XMLHttpRequestEventTargetInterfaceType = 60,
    XMLHttpRequestUploadEventTargetInterfaceType = 61,
};

} // namespace WebCore
