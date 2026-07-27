/*
 * THIS FILE WAS AUTOMATICALLY GENERATED, DO NOT EDIT.
 *
 * Copyright (C) 2017 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "Settings.h"

#include "InspectorInstrumentation.h"
#include "Page.h"
#include "SettingsDefaultValues.h"

namespace WebCore {

Ref<Settings> Settings::create(Page* page)
{
    return adoptRef(*new Settings(page));
}

Settings::Settings(Page* page)
    : SettingsBase(page)
    , m_backForwardCacheExpirationInterval(30_min)
    , m_clipboardAccessPolicy(ClipboardAccessPolicy::RequiresUserGesture)
    , m_defaultFixedFontSize(13)
    , m_defaultFontSize(16)
    , m_defaultTextEncodingName()
    , m_defaultVideoPosterURL()
    , m_deviceHeight(0)
    , m_deviceWidth(0)
    , m_editableLinkBehavior(EditableLinkDefaultBehavior)
    , m_editingBehaviorType(editingBehaviorTypeForPlatform())
    , m_fontLoadTimingOverride(FontLoadTimingOverride::None)
    , m_fontRenderingMode(FontRenderingMode::Normal)
    , m_forcedColorsAreInvertedAccessibilityValue(defaultForcedColorsAreInvertedAccessibilityValue)
    , m_forcedDisplayIsMonochromeAccessibilityValue(defaultForcedDisplayIsMonochromeAccessibilityValue)
    , m_forcedPrefersReducedMotionAccessibilityValue(defaultForcedPrefersReducedMotionAccessibilityValue)
    , m_forcedSupportsHighDynamicRangeValue(defaultForcedSupportsHighDynamicRangeValue)
    , m_frameFlattening(FrameFlattening::Disabled)
    , m_ftpDirectoryTemplatePath()
    , m_incrementalRenderingSuppressionTimeoutInSeconds(defaultIncrementalRenderingSuppressionTimeoutInSeconds)
    , m_javaScriptRuntimeFlags()
    , m_layoutFallbackWidth(980)
    , m_layoutViewportHeightExpansionFactor(0)
    , m_localStorageDatabasePath()
    , m_maxParseDuration(-1)
    , m_maximumAccelerated2dCanvasSize(5120*2880)
    , m_maximumHTMLParserDOMTreeDepth(defaultMaximumHTMLParserDOMTreeDepth)
    , m_maximumPlugInSnapshotAttempts(20)
    , m_mediaKeysStorageDirectory()
    , m_mediaTypeOverride("screen")
    , m_minimumAccelerated2dCanvasSize(257*256)
    , m_minimumFontSize(0)
    , m_minimumLogicalFontSize(0)
    , m_parserScriptingFlagPolicy(ParserScriptingFlagPolicy::OnlyIfScriptIsEnabled)
    , m_passwordEchoDurationInSeconds(1)
    , m_pdfImageCachingPolicy(PDFImageCachingDefault)
    , m_sessionStorageQuota(StorageMap::noQuota)
    , m_storageBlockingPolicy(SecurityOrigin::AllowAllStorage)
    , m_systemLayoutDirection(TextDirection::LTR)
    , m_textDirectionSubmenuInclusionBehavior(TextDirectionSubmenuAutomaticallyIncluded)
    , m_timeWithoutMouseMovementBeforeHidingControls(3_s)
    , m_userInterfaceDirectionPolicy(UserInterfaceDirectionPolicy::Content)
    , m_userStyleSheetLocation()
    , m_validationMessageTimerMagnification(50)
    , m_visibleDebugOverlayRegions(0)
#if ENABLE(DATA_DETECTION)
    , m_dataDetectorTypes()
#endif
#if ENABLE(MEDIA_SOURCE)
    , m_maximumSourceBufferSize(318767104)
#endif
#if ENABLE(MEDIA_STREAM)
    , m_mediaDeviceIdentifierStorageDirectory()
#endif
#if ENABLE(TEXT_AUTOSIZING)
    , m_minimumZoomFontSize(defaultMinimumZoomFontSize())
    , m_textAutosizingWindowSizeOverride()
#endif
    , m_CSSOMViewScrollingAPIEnabled(false)
    , m_CSSOMViewSmoothScrollingEnabled(false)
    , m_DOMPasteAllowed(false)
    , m_HTTPSUpgradeEnabled(false)
    , m_NeedsInAppBrowserPrivacyQuirks(false)
    , m_accelerated2dCanvasEnabled(false)
    , m_acceleratedCompositedAnimationsEnabled(true)
    , m_acceleratedCompositingEnabled(true)
    , m_acceleratedCompositingForFixedPositionEnabled(defaultAcceleratedCompositingForFixedPositionEnabled)
    , m_acceleratedDrawingEnabled(false)
    , m_acceleratedFiltersEnabled(false)
    , m_adClickAttributionEnabled(false)
    , m_aggressiveTileRetentionEnabled(false)
    , m_allowContentSecurityPolicySourceStarToMatchAnyProtocol(false)
    , m_allowCrossOriginSubresourcesToAskForCredentials(false)
    , m_allowDisplayOfInsecureContent(false)
    , m_allowFileAccessFromFileURLs(true)
    , m_allowMediaContentTypesRequiringHardwareSupportAsFallback(false)
    , m_allowMultiElementImplicitSubmission(false)
    , m_allowRunningOfInsecureContent(false)
    , m_allowSettingAnyXHRHeaderFromFileURLs(false)
    , m_allowTopNavigationToDataURLs(false)
    , m_allowUniversalAccessFromFileURLs(true)
    , m_allowViewportShrinkToFitContent(true)
    , m_allowsInlineMediaPlayback(defaultAllowsInlineMediaPlayback)
    , m_allowsInlineMediaPlaybackAfterFullscreen(true)
    , m_allowsPictureInPictureMediaPlayback(defaultAllowsPictureInPictureMediaPlayback)
    , m_alwaysUseAcceleratedOverflowScroll(false)
    , m_animatedImageAsyncDecodingEnabled(true)
    , m_animatedImageDebugCanvasDrawingEnabled(false)
    , m_appleMailPaginationQuirkEnabled(false)
    , m_aspectRatioOfImgFromWidthAndHeightEnabled(false)
    , m_asyncClipboardAPIEnabled(false)
    , m_asyncFrameScrollingEnabled(false)
    , m_asyncOverflowScrollingEnabled(false)
    , m_asynchronousSpellCheckingEnabled(false)
    , m_audioPlaybackRequiresUserGesture(defaultAudioPlaybackRequiresUserGesture)
    , m_authorAndUserStylesEnabled(true)
    , m_autoscrollForDragAndDropEnabled(false)
    , m_autostartOriginPlugInSnapshottingEnabled(true)
    , m_backForwardCacheSupportsPlugins(false)
    , m_backgroundShouldExtendBeyondPage(false)
    , m_backspaceKeyNavigationEnabled(true)
    , m_beaconAPIEnabled(false)
    , m_blockingOfSmallPluginsEnabled(true)
    , m_canvasUsesAcceleratedDrawing(false)
    , m_caretBrowsingEnabled(false)
    , m_clientCoordinatesRelativeToLayoutViewport(false)
    , m_colorFilterEnabled(false)
    , m_constantPropertiesEnabled(false)
    , m_contentChangeObserverEnabled(defaultContentChangeObserverEnabled())
    , m_contentDispositionAttachmentSandboxEnabled(false)
    , m_cookieEnabled(true)
    , m_coreMathMLEnabled(false)
    , m_crossOriginCheckInGetMatchedCSSRulesDisabled(false)
    , m_deferredCSSParserEnabled(false)
    , m_developerExtrasEnabled(false)
    , m_diagnosticLoggingEnabled(false)
    , m_disableScreenSizeOverride(false)
    , m_disallowSyncXHRDuringPageDismissalEnabled(true)
    , m_displayListDrawingEnabled(false)
    , m_dnsPrefetchingEnabled(false)
    , m_domPasteAccessRequestsEnabled(false)
    , m_domTimersThrottlingEnabled(true)
    , m_downloadableBinaryFontsEnabled(defaultDownloadableBinaryFontsEnabled())
    , m_editableImagesEnabled(false)
    , m_enableInheritURIQueryComponent(false)
    , m_enforceCSSMIMETypeInNoQuirksMode(true)
    , m_enterKeyHintEnabled(false)
    , m_experimentalNotificationsEnabled(false)
    , m_fixedBackgroundsPaintRelativeToDocument(defaultFixedBackgroundsPaintRelativeToDocument)
    , m_fixedElementsLayoutRelativeToFrame(false)
    , m_fontFallbackPrefersPictographs(false)
    , m_forceCompositingMode(false)
    , m_forceFTPDirectoryListings(false)
    , m_forcePendingWebGLPolicy(false)
    , m_forceUpdateScrollbarsOnMainThreadForPerformanceTesting(false)
    , m_forceWebGLUsesLowPower(false)
    , m_geolocationFloorLevelEnabled(true)
    , m_googleAntiFlickerOptimizationQuirkEnabled(true)
    , m_hdrMediaCapabilitiesEnabled(false)
    , m_hiddenPageCSSAnimationSuspensionEnabled(false)
    , m_hiddenPageDOMTimerThrottlingAutoIncreases(false)
    , m_hiddenPageDOMTimerThrottlingEnabled(false)
    , m_httpEquivEnabled(true)
    , m_hyperlinkAuditingEnabled(false)
    , m_iceCandidateFilteringEnabled(true)
    , m_imageSubsamplingEnabled(defaultImageSubsamplingEnabled)
    , m_imagesEnabled(true)
    , m_inProcessCookieCacheEnabled(false)
    , m_incompleteImageBorderEnabled(false)
    , m_inlineMediaPlaybackRequiresPlaysInlineAttribute(defaultInlineMediaPlaybackRequiresPlaysInlineAttribute)
    , m_inputEventsEnabled(true)
    , m_interactiveFormValidationEnabled(false)
    , m_invisibleAutoplayNotPermitted(false)
    , m_isAccessibilityIsolatedTreeEnabled(false)
    , m_isFirstPartyWebsiteDataRemovalDisabled(false)
    , m_isFirstPartyWebsiteDataRemovalLiveOnTestingEnabled(false)
    , m_isFirstPartyWebsiteDataRemovalReproTestingEnabled(false)
    , m_isInAppBrowserPrivacyEnabled(false)
    , m_isLoggedInAPIEnabled(false)
    , m_isSameSiteStrictEnforcementEnabled(false)
    , m_isThirdPartyCookieBlockingDisabled(false)
    , m_javaEnabled(false)
    , m_javaEnabledForLocalFiles(true)
    , m_javaScriptCanAccessClipboard(false)
    , m_javaScriptCanOpenWindowsAutomatically(false)
    , m_langAttributeAwareFormControlUIEnabled(false)
    , m_largeImageAsyncDecodingEnabled(true)
    , m_linkPreconnectEnabled(false)
    , m_loadDeferringEnabled(true)
    , m_loadsImagesAutomatically(false)
    , m_loadsSiteIconsIgnoringImageLoadingSetting(false)
    , m_localFileContentSniffingEnabled(false)
    , m_localStorageEnabled(false)
    , m_logsPageMessagesToSystemConsoleEnabled(false)
    , m_mainContentUserGestureOverrideEnabled(false)
    , m_mediaCapabilitiesEnabled(false)
    , m_mediaCapabilitiesExtensionsEnabled(false)
    , m_mediaControlsScaleWithPageZoom(defaultMediaControlsScaleWithPageZoom)
    , m_mediaDataLoadsAutomatically(defaultMediaDataLoadsAutomatically)
    , m_mediaEnabled(defaultMediaEnabled)
    , m_mediaPreloadingEnabled(false)
    , m_mediaUserGestureInheritsFromDocument(false)
    , m_needsAdobeFrameReloadingQuirk(false)
    , m_needsDeferKeyDownAndKeyPressTimersUntilNextEditingCommandQuirk(false)
    , m_needsFrameNameFallbackToIdQuirk(false)
    , m_needsIsLoadingInAPISenseQuirk(false)
    , m_needsKeyboardEventDisambiguationQuirks(false)
    , m_needsSiteSpecificQuirks(false)
    , m_needsStorageAccessFromFileURLsQuirk(true)
    , m_notificationsEnabled(true)
    , m_offlineWebApplicationCacheEnabled(false)
    , m_paginateDuringLayoutEnabled(false)
    , m_passiveTouchListenersAsDefaultOnDocument(true)
    , m_passwordEchoEnabled(false)
    , m_pictureInPictureAPIEnabled(true)
    , m_plugInSnapshottingEnabled(false)
    , m_pluginsEnabled(false)
    , m_preferMIMETypeForImages(false)
    , m_preventKeyboardDOMEventDispatch(false)
    , m_primaryPlugInSnapshotDetectionEnabled(true)
    , m_punchOutWhiteBackgroundsInDarkMode(false)
    , m_quickTimePluginReplacementEnabled(defaultQuickTimePluginReplacementEnabled)
    , m_repaintOutsideLayoutEnabled(false)
    , m_requestAnimationFrameEnabled(true)
    , m_requestIdleCallbackEnabled(false)
    , m_requiresUserGestureToLoadVideo(defaultRequiresUserGestureToLoadVideo)
    , m_resourceUsageOverlayVisible(false)
    , m_scriptEnabled(false)
    , m_scriptMarkupEnabled(true)
    , m_scrollingCoordinatorEnabled(false)
    , m_scrollingPerformanceLoggingEnabled(false)
    , m_scrollingTreeIncludesFrames(defaultScrollingTreeIncludesFrames)
    , m_selectTrailingWhitespaceEnabled(defaultSelectTrailingWhitespaceEnabled)
    , m_selectionAcrossShadowBoundariesEnabled(true)
    , m_selectionPaintingWithoutSelectionGapsEnabled(false)
    , m_shouldAllowUserInstalledFonts(true)
    , m_shouldConvertInvalidURLsToBlank(true)
    , m_shouldConvertPositionStyleOnCopy(false)
    , m_shouldDecidePolicyBeforeLoadingQuickLookPreview(false)
    , m_shouldDeferAsynchronousScriptsUntilAfterDocumentLoadOrFirstPaint(true)
    , m_shouldDispatchSyntheticMouseEventsWhenModifyingSelection(false)
    , m_shouldDispatchSyntheticMouseOutAfterSyntheticClick(false)
    , m_shouldIgnoreFontLoadCompletions(false)
    , m_shouldIgnoreMetaViewport(false)
    , m_shouldInjectUserScriptsInInitialEmptyDocument(false)
    , m_shouldPrintBackgrounds(false)
    , m_shouldRespectImageOrientation(defaultShouldRespectImageOrientation)
    , m_shouldRestrictBaseURLSchemes(true)
    , m_shouldSuppressTextInputFromEditingDuringProvisionalNavigation(false)
    , m_shouldUseServiceWorkerShortTimeout(false)
    , m_showDebugBorders(false)
    , m_showRepaintCounter(false)
    , m_showTiledScrollingIndicator(false)
    , m_showsToolTipOverTruncatedText(false)
    , m_showsURLsInToolTips(false)
    , m_shrinksStandaloneImagesToFit(true)
    , m_simpleLineLayoutDebugBordersEnabled(false)
    , m_simpleLineLayoutEnabled(true)
    , m_smartInsertDeleteEnabled(defaultSmartInsertDeleteEnabled)
    , m_snapshotAllPlugIns(false)
    , m_spatialNavigationEnabled(false)
    , m_springTimingFunctionEnabled(false)
    , m_standalone(false)
    , m_storageAccessAPIEnabled(true)
    , m_subpixelAntialiasedLayerTextEnabled(false)
    , m_subpixelCSSOMElementMetricsEnabled(false)
    , m_subresourceIntegrityEnabled(true)
    , m_suppressesIncrementalRendering(false)
    , m_syntheticEditingCommandsEnabled(true)
    , m_telephoneNumberParsingEnabled(false)
    , m_temporaryTileCohortRetentionEnabled(true)
    , m_textAreasAreResizable(false)
    , m_thirdPartyIframeRedirectBlockingEnabled(true)
    , m_treatIPAddressAsDomain(false)
    , m_treatsAnyTextCSSLinkAsStylesheet(false)
    , m_unhandledPromiseRejectionToConsoleEnabled(true)
    , m_unifiedTextCheckerEnabled(defaultUnifiedTextCheckerEnabled)
    , m_useAnonymousModeWhenFetchingMaskImages(true)
    , m_useGiantTiles(false)
    , m_useImageDocumentForSubframePDF(false)
    , m_useLegacyBackgroundSizeShorthandBehavior(false)
    , m_useLegacyTextAlignPositionedElementBehavior(false)
    , m_usePreHTML5ParserQuirks(false)
    , m_usesBackForwardCache(false)
    , m_usesEncodingDetector(false)
    , m_videoPlaybackRequiresUserGesture(defaultVideoPlaybackRequiresUserGesture)
    , m_viewportFitEnabled(false)
    , m_visualViewportAPIEnabled(false)
    , m_visualViewportEnabled(true)
    , m_wantsBalancedSetDefersLoadingBehavior(false)
    , m_webAudioEnabled(false)
    , m_webGLEnabled(false)
    , m_webGLErrorsToConsoleEnabled(true)
    , m_webRTCEncryptionEnabled(true)
    , m_webSecurityEnabled(true)
    , m_webkitImageReadyEventEnabled(false)
    , m_windowFocusRestricted(true)
    , m_xssAuditorEnabled(false)
    , m_youTubeFlashPluginReplacementEnabled(defaultYouTubeFlashPluginReplacementEnabled)
#if ENABLE(APPLE_PAY)
    , m_applePayCapabilityDisclosureAllowed(true)
    , m_applePayEnabled(defaultApplePayEnabled)
#endif
#if ENABLE(APPLE_PAY_REMOTE_UI)
    , m_applePayRemoteUIEnabled(true)
#endif
#if ENABLE(DEVICE_ORIENTATION)
    , m_deviceOrientationEventEnabled(true)
    , m_deviceOrientationPermissionAPIEnabled(false)
#endif
#if ENABLE(FULLSCREEN_API)
    , m_fullScreenEnabled(false)
#endif
#if ENABLE(GPU_PROCESS)
    , m_useGPUProcessForMedia(false)
#endif
#if ENABLE(MEDIA_SOURCE)
    , m_mediaSourceEnabled(platformDefaultMediaSourceEnabled())
    , m_sourceBufferChangeTypeEnabled(true)
#endif
#if ENABLE(MEDIA_STREAM)
    , m_legacyGetUserMediaEnabled(false)
    , m_mediaCaptureRequiresSecureConnection(true)
    , m_mockCaptureDevicesEnabled(false)
#endif
#if ENABLE(OVERFLOW_SCROLLING_TOUCH)
    , m_legacyOverflowScrollingTouchEnabled(true)
#endif
#if ENABLE(PAYMENT_REQUEST)
    , m_paymentRequestEnabled(false)
#endif
#if ENABLE(RESIZE_OBSERVER)
    , m_resizeObserverEnabled(false)
#endif
#if ENABLE(RUBBER_BANDING)
    , m_rubberBandingForSubScrollableRegionsEnabled(true)
#endif
#if ENABLE(SERVICE_CONTROLS)
    , m_imageControlsEnabled(false)
    , m_serviceControlsEnabled(false)
#endif
#if ENABLE(SMOOTH_SCROLLING)
    , m_scrollAnimatorEnabled(true)
#endif
#if ENABLE(TEXT_AUTOSIZING)
    , m_idempotentModeAutosizingOnlyHonorsPercentages(false)
    , m_shouldEnableTextAutosizingBoost(false)
    , m_textAutosizingEnabled(defaultTextAutosizingEnabled())
    , m_textAutosizingUsesIdempotentMode(false)
#endif
#if ENABLE(TOUCH_EVENTS)
    , m_touchEventEmulationEnabled(false)
#endif
#if ENABLE(VIDEO)
    , m_videoQualityIncludesDisplayCompositingEnabled(false)
#endif
#if ENABLE(VIDEO_TRACK)
    , m_genericCueAPIEnabled(false)
    , m_shouldDisplayCaptions(false)
    , m_shouldDisplaySubtitles(false)
    , m_shouldDisplayTextDescriptions(false)
#endif
#if ENABLE(WEB_ARCHIVE)
    , m_webArchiveDebugModeEnabled(false)
#endif
#if ENABLE(WIRELESS_PLAYBACK_TARGET)
    , m_allowsAirPlayForMediaPlayback(true)
    , m_remotePlaybackEnabled(true)
#endif
{
}

Settings::~Settings()
{
}

bool Settings::authorAndUserStylesEnabled() const
{
    if (UNLIKELY(m_authorAndUserStylesEnabledInspectorOverride)) {
        ASSERT(InspectorInstrumentation::hasFrontends());
        return m_authorAndUserStylesEnabledInspectorOverride.value();
    }
    return m_authorAndUserStylesEnabled;
}

bool Settings::iceCandidateFilteringEnabled() const
{
    if (UNLIKELY(m_iceCandidateFilteringEnabledInspectorOverride)) {
        ASSERT(InspectorInstrumentation::hasFrontends());
        return m_iceCandidateFilteringEnabledInspectorOverride.value();
    }
    return m_iceCandidateFilteringEnabled;
}

bool Settings::areImagesEnabled() const
{
    if (UNLIKELY(m_imagesEnabledInspectorOverride)) {
        ASSERT(InspectorInstrumentation::hasFrontends());
        return m_imagesEnabledInspectorOverride.value();
    }
    return m_imagesEnabled;
}

bool Settings::needsSiteSpecificQuirks() const
{
    if (UNLIKELY(m_needsSiteSpecificQuirksInspectorOverride)) {
        ASSERT(InspectorInstrumentation::hasFrontends());
        return m_needsSiteSpecificQuirksInspectorOverride.value();
    }
    return m_needsSiteSpecificQuirks;
}

bool Settings::isScriptEnabled() const
{
    if (UNLIKELY(m_scriptEnabledInspectorOverride)) {
        ASSERT(InspectorInstrumentation::hasFrontends());
        return m_scriptEnabledInspectorOverride.value();
    }
    return m_scriptEnabled;
}

bool Settings::showDebugBorders() const
{
    if (UNLIKELY(m_showDebugBordersInspectorOverride)) {
        ASSERT(InspectorInstrumentation::hasFrontends());
        return m_showDebugBordersInspectorOverride.value();
    }
    return m_showDebugBorders;
}

bool Settings::showRepaintCounter() const
{
    if (UNLIKELY(m_showRepaintCounterInspectorOverride)) {
        ASSERT(InspectorInstrumentation::hasFrontends());
        return m_showRepaintCounterInspectorOverride.value();
    }
    return m_showRepaintCounter;
}

bool Settings::webRTCEncryptionEnabled() const
{
    if (UNLIKELY(m_webRTCEncryptionEnabledInspectorOverride)) {
        ASSERT(InspectorInstrumentation::hasFrontends());
        return m_webRTCEncryptionEnabledInspectorOverride.value();
    }
    return m_webRTCEncryptionEnabled;
}

bool Settings::webSecurityEnabled() const
{
    if (UNLIKELY(m_webSecurityEnabledInspectorOverride)) {
        ASSERT(InspectorInstrumentation::hasFrontends());
        return m_webSecurityEnabledInspectorOverride.value();
    }
    return m_webSecurityEnabled;
}

void Settings::setAcceleratedCompositedAnimationsEnabled(bool acceleratedCompositedAnimationsEnabled)
{
    if (m_acceleratedCompositedAnimationsEnabled == acceleratedCompositedAnimationsEnabled)
        return;
    m_acceleratedCompositedAnimationsEnabled = acceleratedCompositedAnimationsEnabled;
    setNeedsRecalcStyleInAllFrames();
}

void Settings::setAcceleratedCompositingEnabled(bool acceleratedCompositingEnabled)
{
    if (m_acceleratedCompositingEnabled == acceleratedCompositingEnabled)
        return;
    m_acceleratedCompositingEnabled = acceleratedCompositingEnabled;
    setNeedsRecalcStyleInAllFrames();
}

void Settings::setAsyncFrameScrollingEnabled(bool asyncFrameScrollingEnabled)
{
    if (m_asyncFrameScrollingEnabled == asyncFrameScrollingEnabled)
        return;
    m_asyncFrameScrollingEnabled = asyncFrameScrollingEnabled;
    setNeedsRelayoutAllFrames();
}

void Settings::setAsyncOverflowScrollingEnabled(bool asyncOverflowScrollingEnabled)
{
    if (m_asyncOverflowScrollingEnabled == asyncOverflowScrollingEnabled)
        return;
    m_asyncOverflowScrollingEnabled = asyncOverflowScrollingEnabled;
    setNeedsRelayoutAllFrames();
}

void Settings::setAuthorAndUserStylesEnabled(bool authorAndUserStylesEnabled)
{
    if (m_authorAndUserStylesEnabled == authorAndUserStylesEnabled)
        return;
    m_authorAndUserStylesEnabled = authorAndUserStylesEnabled;
    setNeedsRecalcStyleInAllFrames();
}

void Settings::setBackgroundShouldExtendBeyondPage(bool backgroundShouldExtendBeyondPage)
{
    if (m_backgroundShouldExtendBeyondPage == backgroundShouldExtendBeyondPage)
        return;
    m_backgroundShouldExtendBeyondPage = backgroundShouldExtendBeyondPage;
    backgroundShouldExtendBeyondPageChanged();
}

void Settings::setClientCoordinatesRelativeToLayoutViewport(bool clientCoordinatesRelativeToLayoutViewport)
{
    if (m_clientCoordinatesRelativeToLayoutViewport == clientCoordinatesRelativeToLayoutViewport)
        return;
    m_clientCoordinatesRelativeToLayoutViewport = clientCoordinatesRelativeToLayoutViewport;
    setNeedsRecalcStyleInAllFrames();
}

void Settings::setColorFilterEnabled(bool colorFilterEnabled)
{
    if (m_colorFilterEnabled == colorFilterEnabled)
        return;
    m_colorFilterEnabled = colorFilterEnabled;
    setNeedsRecalcStyleInAllFrames();
}

void Settings::setCoreMathMLEnabled(bool coreMathMLEnabled)
{
    if (m_coreMathMLEnabled == coreMathMLEnabled)
        return;
    m_coreMathMLEnabled = coreMathMLEnabled;
    setNeedsRecalcStyleInAllFrames();
}

void Settings::setDefaultFixedFontSize(int defaultFixedFontSize)
{
    if (m_defaultFixedFontSize == defaultFixedFontSize)
        return;
    m_defaultFixedFontSize = defaultFixedFontSize;
    setNeedsRecalcStyleInAllFrames();
}

void Settings::setDefaultFontSize(int defaultFontSize)
{
    if (m_defaultFontSize == defaultFontSize)
        return;
    m_defaultFontSize = defaultFontSize;
    setNeedsRecalcStyleInAllFrames();
}

void Settings::setDNSPrefetchingEnabled(bool dnsPrefetchingEnabled)
{
    if (m_dnsPrefetchingEnabled == dnsPrefetchingEnabled)
        return;
    m_dnsPrefetchingEnabled = dnsPrefetchingEnabled;
    dnsPrefetchingEnabledChanged();
}

void Settings::setFontFallbackPrefersPictographs(bool fontFallbackPrefersPictographs)
{
    if (m_fontFallbackPrefersPictographs == fontFallbackPrefersPictographs)
        return;
    m_fontFallbackPrefersPictographs = fontFallbackPrefersPictographs;
    setNeedsRecalcStyleInAllFrames();
}

void Settings::setForcedSupportsHighDynamicRangeValue(ForcedAccessibilityValue forcedSupportsHighDynamicRangeValue)
{
    if (m_forcedSupportsHighDynamicRangeValue == forcedSupportsHighDynamicRangeValue)
        return;
    m_forcedSupportsHighDynamicRangeValue = forcedSupportsHighDynamicRangeValue;
    setNeedsRecalcStyleInAllFrames();
}

void Settings::setFrameFlattening(FrameFlattening frameFlattening)
{
    if (m_frameFlattening == frameFlattening)
        return;
    m_frameFlattening = frameFlattening;
    setNeedsRelayoutAllFrames();
}

void Settings::setHiddenPageCSSAnimationSuspensionEnabled(bool hiddenPageCSSAnimationSuspensionEnabled)
{
    if (m_hiddenPageCSSAnimationSuspensionEnabled == hiddenPageCSSAnimationSuspensionEnabled)
        return;
    m_hiddenPageCSSAnimationSuspensionEnabled = hiddenPageCSSAnimationSuspensionEnabled;
    hiddenPageCSSAnimationSuspensionEnabledChanged();
}

void Settings::setHiddenPageDOMTimerThrottlingAutoIncreases(bool hiddenPageDOMTimerThrottlingAutoIncreases)
{
    if (m_hiddenPageDOMTimerThrottlingAutoIncreases == hiddenPageDOMTimerThrottlingAutoIncreases)
        return;
    m_hiddenPageDOMTimerThrottlingAutoIncreases = hiddenPageDOMTimerThrottlingAutoIncreases;
    hiddenPageDOMTimerThrottlingStateChanged();
}

void Settings::setHiddenPageDOMTimerThrottlingEnabled(bool hiddenPageDOMTimerThrottlingEnabled)
{
    if (m_hiddenPageDOMTimerThrottlingEnabled == hiddenPageDOMTimerThrottlingEnabled)
        return;
    m_hiddenPageDOMTimerThrottlingEnabled = hiddenPageDOMTimerThrottlingEnabled;
    hiddenPageDOMTimerThrottlingStateChanged();
}

void Settings::setICECandidateFilteringEnabled(bool iceCandidateFilteringEnabled)
{
    if (m_iceCandidateFilteringEnabled == iceCandidateFilteringEnabled)
        return;
    m_iceCandidateFilteringEnabled = iceCandidateFilteringEnabled;
    iceCandidateFilteringEnabledChanged();
}

void Settings::setImagesEnabled(bool imagesEnabled)
{
    if (m_imagesEnabled == imagesEnabled)
        return;
    m_imagesEnabled = imagesEnabled;
    imagesEnabledChanged();
}

void Settings::setLayoutViewportHeightExpansionFactor(double layoutViewportHeightExpansionFactor)
{
    if (m_layoutViewportHeightExpansionFactor == layoutViewportHeightExpansionFactor)
        return;
    m_layoutViewportHeightExpansionFactor = layoutViewportHeightExpansionFactor;
    setNeedsRecalcStyleInAllFrames();
}

void Settings::setLoadsImagesAutomatically(bool loadsImagesAutomatically)
{
    if (m_loadsImagesAutomatically == loadsImagesAutomatically)
        return;
    m_loadsImagesAutomatically = loadsImagesAutomatically;
    imagesEnabledChanged();
}

void Settings::setMediaTypeOverride(const String& mediaTypeOverride)
{
    if (m_mediaTypeOverride == mediaTypeOverride)
        return;
    m_mediaTypeOverride = mediaTypeOverride;
    mediaTypeOverrideChanged();
}

void Settings::setMinimumFontSize(int minimumFontSize)
{
    if (m_minimumFontSize == minimumFontSize)
        return;
    m_minimumFontSize = minimumFontSize;
    setNeedsRecalcStyleInAllFrames();
}

void Settings::setMinimumLogicalFontSize(int minimumLogicalFontSize)
{
    if (m_minimumLogicalFontSize == minimumLogicalFontSize)
        return;
    m_minimumLogicalFontSize = minimumLogicalFontSize;
    setNeedsRecalcStyleInAllFrames();
}

void Settings::setPluginsEnabled(bool pluginsEnabled)
{
    if (m_pluginsEnabled == pluginsEnabled)
        return;
    m_pluginsEnabled = pluginsEnabled;
    pluginsEnabledChanged();
}

void Settings::setPunchOutWhiteBackgroundsInDarkMode(bool punchOutWhiteBackgroundsInDarkMode)
{
    if (m_punchOutWhiteBackgroundsInDarkMode == punchOutWhiteBackgroundsInDarkMode)
        return;
    m_punchOutWhiteBackgroundsInDarkMode = punchOutWhiteBackgroundsInDarkMode;
    setNeedsRecalcStyleInAllFrames();
}

void Settings::setResourceUsageOverlayVisible(bool resourceUsageOverlayVisible)
{
    if (m_resourceUsageOverlayVisible == resourceUsageOverlayVisible)
        return;
    m_resourceUsageOverlayVisible = resourceUsageOverlayVisible;
    resourceUsageOverlayVisibleChanged();
}

void Settings::setScrollingPerformanceLoggingEnabled(bool scrollingPerformanceLoggingEnabled)
{
    if (m_scrollingPerformanceLoggingEnabled == scrollingPerformanceLoggingEnabled)
        return;
    m_scrollingPerformanceLoggingEnabled = scrollingPerformanceLoggingEnabled;
    scrollingPerformanceLoggingEnabledChanged();
}

void Settings::setShouldAllowUserInstalledFonts(bool shouldAllowUserInstalledFonts)
{
    if (m_shouldAllowUserInstalledFonts == shouldAllowUserInstalledFonts)
        return;
    m_shouldAllowUserInstalledFonts = shouldAllowUserInstalledFonts;
    setNeedsRecalcStyleInAllFrames();
}

void Settings::setShowDebugBorders(bool showDebugBorders)
{
    if (m_showDebugBorders == showDebugBorders)
        return;
    m_showDebugBorders = showDebugBorders;
    setNeedsRecalcStyleInAllFrames();
}

void Settings::setShowRepaintCounter(bool showRepaintCounter)
{
    if (m_showRepaintCounter == showRepaintCounter)
        return;
    m_showRepaintCounter = showRepaintCounter;
    setNeedsRecalcStyleInAllFrames();
}

void Settings::setSimpleLineLayoutDebugBordersEnabled(bool simpleLineLayoutDebugBordersEnabled)
{
    if (m_simpleLineLayoutDebugBordersEnabled == simpleLineLayoutDebugBordersEnabled)
        return;
    m_simpleLineLayoutDebugBordersEnabled = simpleLineLayoutDebugBordersEnabled;
    setNeedsRecalcStyleInAllFrames();
}

void Settings::setSimpleLineLayoutEnabled(bool simpleLineLayoutEnabled)
{
    if (m_simpleLineLayoutEnabled == simpleLineLayoutEnabled)
        return;
    m_simpleLineLayoutEnabled = simpleLineLayoutEnabled;
    setNeedsRecalcStyleInAllFrames();
}

void Settings::setStorageBlockingPolicy(SecurityOrigin::StorageBlockingPolicy storageBlockingPolicy)
{
    if (m_storageBlockingPolicy == storageBlockingPolicy)
        return;
    m_storageBlockingPolicy = storageBlockingPolicy;
    storageBlockingPolicyChanged();
}

void Settings::setSubpixelAntialiasedLayerTextEnabled(bool subpixelAntialiasedLayerTextEnabled)
{
    if (m_subpixelAntialiasedLayerTextEnabled == subpixelAntialiasedLayerTextEnabled)
        return;
    m_subpixelAntialiasedLayerTextEnabled = subpixelAntialiasedLayerTextEnabled;
    setNeedsRecalcStyleInAllFrames();
}

void Settings::setTextAreasAreResizable(bool textAreasAreResizable)
{
    if (m_textAreasAreResizable == textAreasAreResizable)
        return;
    m_textAreasAreResizable = textAreasAreResizable;
    setNeedsRecalcStyleInAllFrames();
}

void Settings::setUserStyleSheetLocation(const URL& userStyleSheetLocation)
{
    if (m_userStyleSheetLocation == userStyleSheetLocation)
        return;
    m_userStyleSheetLocation = userStyleSheetLocation;
    userStyleSheetLocationChanged();
}

void Settings::setUsesBackForwardCache(bool usesBackForwardCache)
{
    if (m_usesBackForwardCache == usesBackForwardCache)
        return;
    m_usesBackForwardCache = usesBackForwardCache;
    usesBackForwardCacheChanged();
}

void Settings::setVisualViewportEnabled(bool visualViewportEnabled)
{
    if (m_visualViewportEnabled == visualViewportEnabled)
        return;
    m_visualViewportEnabled = visualViewportEnabled;
    setNeedsRecalcStyleInAllFrames();
}

#if ENABLE(MEDIA_STREAM)
bool Settings::mediaCaptureRequiresSecureConnection() const
{
    if (UNLIKELY(m_mediaCaptureRequiresSecureConnectionInspectorOverride)) {
        ASSERT(InspectorInstrumentation::hasFrontends());
        return m_mediaCaptureRequiresSecureConnectionInspectorOverride.value();
    }
    return m_mediaCaptureRequiresSecureConnection;
}
bool Settings::mockCaptureDevicesEnabled() const
{
    if (UNLIKELY(m_mockCaptureDevicesEnabledInspectorOverride)) {
        ASSERT(InspectorInstrumentation::hasFrontends());
        return m_mockCaptureDevicesEnabledInspectorOverride.value();
    }
    return m_mockCaptureDevicesEnabled;
}
void Settings::setMockCaptureDevicesEnabled(bool mockCaptureDevicesEnabled)
{
    if (m_mockCaptureDevicesEnabled == mockCaptureDevicesEnabled)
        return;
    m_mockCaptureDevicesEnabled = mockCaptureDevicesEnabled;
    mockCaptureDevicesEnabledChanged();
}
#endif

#if ENABLE(OVERFLOW_SCROLLING_TOUCH)
void Settings::setLegacyOverflowScrollingTouchEnabled(bool legacyOverflowScrollingTouchEnabled)
{
    if (m_legacyOverflowScrollingTouchEnabled == legacyOverflowScrollingTouchEnabled)
        return;
    m_legacyOverflowScrollingTouchEnabled = legacyOverflowScrollingTouchEnabled;
    setNeedsRelayoutAllFrames();
}
#endif

#if ENABLE(TEXT_AUTOSIZING)
void Settings::setIdempotentModeAutosizingOnlyHonorsPercentages(bool idempotentModeAutosizingOnlyHonorsPercentages)
{
    if (m_idempotentModeAutosizingOnlyHonorsPercentages == idempotentModeAutosizingOnlyHonorsPercentages)
        return;
    m_idempotentModeAutosizingOnlyHonorsPercentages = idempotentModeAutosizingOnlyHonorsPercentages;
    setNeedsRecalcStyleInAllFrames();
}
void Settings::setShouldEnableTextAutosizingBoost(bool shouldEnableTextAutosizingBoost)
{
    if (m_shouldEnableTextAutosizingBoost == shouldEnableTextAutosizingBoost)
        return;
    m_shouldEnableTextAutosizingBoost = shouldEnableTextAutosizingBoost;
    shouldEnableTextAutosizingBoostChanged();
}
void Settings::setTextAutosizingEnabled(bool textAutosizingEnabled)
{
    if (m_textAutosizingEnabled == textAutosizingEnabled)
        return;
    m_textAutosizingEnabled = textAutosizingEnabled;
    setNeedsRecalcStyleInAllFrames();
}
void Settings::setTextAutosizingUsesIdempotentMode(bool textAutosizingUsesIdempotentMode)
{
    if (m_textAutosizingUsesIdempotentMode == textAutosizingUsesIdempotentMode)
        return;
    m_textAutosizingUsesIdempotentMode = textAutosizingUsesIdempotentMode;
    setNeedsRecalcStyleInAllFrames();
}
void Settings::setTextAutosizingWindowSizeOverride(IntSize textAutosizingWindowSizeOverride)
{
    if (m_textAutosizingWindowSizeOverride == textAutosizingWindowSizeOverride)
        return;
    m_textAutosizingWindowSizeOverride = textAutosizingWindowSizeOverride;
    setNeedsRecalcStyleInAllFrames();
}
#endif

void Settings::setAuthorAndUserStylesEnabledInspectorOverride(Optional<bool> authorAndUserStylesEnabledInspectorOverride)
{
    if (m_authorAndUserStylesEnabledInspectorOverride == authorAndUserStylesEnabledInspectorOverride)
        return;
    m_authorAndUserStylesEnabledInspectorOverride = authorAndUserStylesEnabledInspectorOverride;
    setNeedsRecalcStyleInAllFrames();
}

void Settings::setICECandidateFilteringEnabledInspectorOverride(Optional<bool> iceCandidateFilteringEnabledInspectorOverride)
{
    if (m_iceCandidateFilteringEnabledInspectorOverride == iceCandidateFilteringEnabledInspectorOverride)
        return;
    m_iceCandidateFilteringEnabledInspectorOverride = iceCandidateFilteringEnabledInspectorOverride;
    iceCandidateFilteringEnabledChanged();
}

void Settings::setImagesEnabledInspectorOverride(Optional<bool> imagesEnabledInspectorOverride)
{
    if (m_imagesEnabledInspectorOverride == imagesEnabledInspectorOverride)
        return;
    m_imagesEnabledInspectorOverride = imagesEnabledInspectorOverride;
    imagesEnabledChanged();
}

void Settings::setMockCaptureDevicesEnabledInspectorOverride(Optional<bool> mockCaptureDevicesEnabledInspectorOverride)
{
    if (m_mockCaptureDevicesEnabledInspectorOverride == mockCaptureDevicesEnabledInspectorOverride)
        return;
    m_mockCaptureDevicesEnabledInspectorOverride = mockCaptureDevicesEnabledInspectorOverride;
#if ENABLE(MEDIA_STREAM)
    mockCaptureDevicesEnabledChanged();
#endif
}

void Settings::setShowDebugBordersInspectorOverride(Optional<bool> showDebugBordersInspectorOverride)
{
    if (m_showDebugBordersInspectorOverride == showDebugBordersInspectorOverride)
        return;
    m_showDebugBordersInspectorOverride = showDebugBordersInspectorOverride;
    setNeedsRecalcStyleInAllFrames();
}

void Settings::setShowRepaintCounterInspectorOverride(Optional<bool> showRepaintCounterInspectorOverride)
{
    if (m_showRepaintCounterInspectorOverride == showRepaintCounterInspectorOverride)
        return;
    m_showRepaintCounterInspectorOverride = showRepaintCounterInspectorOverride;
    setNeedsRecalcStyleInAllFrames();
}

}
