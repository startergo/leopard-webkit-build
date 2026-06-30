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
#include "InternalSettingsGenerated.h"

#include "Page.h"
#include "Settings.h"

namespace WebCore {

InternalSettingsGenerated::InternalSettingsGenerated(Page* page)
    : m_page(page)
    , m_CSSOMViewScrollingAPIEnabled(page->settings().CSSOMViewScrollingAPIEnabled())
    , m_CSSOMViewSmoothScrollingEnabled(page->settings().CSSOMViewSmoothScrollingEnabled())
    , m_DOMPasteAllowed(page->settings().DOMPasteAllowed())
    , m_HTTPSUpgradeEnabled(page->settings().HTTPSUpgradeEnabled())
    , m_NeedsInAppBrowserPrivacyQuirks(page->settings().NeedsInAppBrowserPrivacyQuirks())
    , m_accelerated2dCanvasEnabled(page->settings().accelerated2dCanvasEnabled())
    , m_acceleratedCompositedAnimationsEnabled(page->settings().acceleratedCompositedAnimationsEnabled())
    , m_acceleratedCompositingEnabled(page->settings().acceleratedCompositingEnabled())
    , m_acceleratedCompositingForFixedPositionEnabled(page->settings().acceleratedCompositingForFixedPositionEnabled())
    , m_acceleratedDrawingEnabled(page->settings().acceleratedDrawingEnabled())
    , m_acceleratedFiltersEnabled(page->settings().acceleratedFiltersEnabled())
    , m_adClickAttributionEnabled(page->settings().adClickAttributionEnabled())
    , m_aggressiveTileRetentionEnabled(page->settings().aggressiveTileRetentionEnabled())
    , m_allowContentSecurityPolicySourceStarToMatchAnyProtocol(page->settings().allowContentSecurityPolicySourceStarToMatchAnyProtocol())
    , m_allowCrossOriginSubresourcesToAskForCredentials(page->settings().allowCrossOriginSubresourcesToAskForCredentials())
    , m_allowDisplayOfInsecureContent(page->settings().allowDisplayOfInsecureContent())
    , m_allowFileAccessFromFileURLs(page->settings().allowFileAccessFromFileURLs())
    , m_allowMediaContentTypesRequiringHardwareSupportAsFallback(page->settings().allowMediaContentTypesRequiringHardwareSupportAsFallback())
    , m_allowMultiElementImplicitSubmission(page->settings().allowMultiElementImplicitSubmission())
    , m_allowRunningOfInsecureContent(page->settings().allowRunningOfInsecureContent())
    , m_allowSettingAnyXHRHeaderFromFileURLs(page->settings().allowSettingAnyXHRHeaderFromFileURLs())
    , m_allowTopNavigationToDataURLs(page->settings().allowTopNavigationToDataURLs())
    , m_allowUniversalAccessFromFileURLs(page->settings().allowUniversalAccessFromFileURLs())
    , m_allowViewportShrinkToFitContent(page->settings().allowViewportShrinkToFitContent())
#if ENABLE(WIRELESS_PLAYBACK_TARGET)
    , m_allowsAirPlayForMediaPlayback(page->settings().allowsAirPlayForMediaPlayback())
#endif
    , m_allowsInlineMediaPlayback(page->settings().allowsInlineMediaPlayback())
    , m_allowsInlineMediaPlaybackAfterFullscreen(page->settings().allowsInlineMediaPlaybackAfterFullscreen())
    , m_allowsPictureInPictureMediaPlayback(page->settings().allowsPictureInPictureMediaPlayback())
    , m_alwaysUseAcceleratedOverflowScroll(page->settings().alwaysUseAcceleratedOverflowScroll())
    , m_animatedImageAsyncDecodingEnabled(page->settings().animatedImageAsyncDecodingEnabled())
    , m_animatedImageDebugCanvasDrawingEnabled(page->settings().animatedImageDebugCanvasDrawingEnabled())
    , m_appleMailPaginationQuirkEnabled(page->settings().appleMailPaginationQuirkEnabled())
#if ENABLE(APPLE_PAY)
    , m_applePayCapabilityDisclosureAllowed(page->settings().applePayCapabilityDisclosureAllowed())
#endif
#if ENABLE(APPLE_PAY)
    , m_applePayEnabled(page->settings().applePayEnabled())
#endif
#if ENABLE(APPLE_PAY_REMOTE_UI)
    , m_applePayRemoteUIEnabled(page->settings().applePayRemoteUIEnabled())
#endif
    , m_aspectRatioOfImgFromWidthAndHeightEnabled(page->settings().aspectRatioOfImgFromWidthAndHeightEnabled())
    , m_asyncClipboardAPIEnabled(page->settings().asyncClipboardAPIEnabled())
    , m_asyncFrameScrollingEnabled(page->settings().asyncFrameScrollingEnabled())
    , m_asyncOverflowScrollingEnabled(page->settings().asyncOverflowScrollingEnabled())
    , m_asynchronousSpellCheckingEnabled(page->settings().asynchronousSpellCheckingEnabled())
    , m_audioPlaybackRequiresUserGesture(page->settings().audioPlaybackRequiresUserGesture())
    , m_authorAndUserStylesEnabled(page->settings().authorAndUserStylesEnabled())
    , m_autoscrollForDragAndDropEnabled(page->settings().autoscrollForDragAndDropEnabled())
    , m_autostartOriginPlugInSnapshottingEnabled(page->settings().autostartOriginPlugInSnapshottingEnabled())
    , m_backForwardCacheSupportsPlugins(page->settings().backForwardCacheSupportsPlugins())
    , m_backgroundShouldExtendBeyondPage(page->settings().backgroundShouldExtendBeyondPage())
    , m_backspaceKeyNavigationEnabled(page->settings().backspaceKeyNavigationEnabled())
    , m_beaconAPIEnabled(page->settings().beaconAPIEnabled())
    , m_blockingOfSmallPluginsEnabled(page->settings().blockingOfSmallPluginsEnabled())
    , m_canvasUsesAcceleratedDrawing(page->settings().canvasUsesAcceleratedDrawing())
    , m_caretBrowsingEnabled(page->settings().caretBrowsingEnabled())
    , m_clientCoordinatesRelativeToLayoutViewport(page->settings().clientCoordinatesRelativeToLayoutViewport())
    , m_colorFilterEnabled(page->settings().colorFilterEnabled())
    , m_constantPropertiesEnabled(page->settings().constantPropertiesEnabled())
    , m_contentChangeObserverEnabled(page->settings().contentChangeObserverEnabled())
    , m_contentDispositionAttachmentSandboxEnabled(page->settings().contentDispositionAttachmentSandboxEnabled())
    , m_cookieEnabled(page->settings().cookieEnabled())
    , m_coreMathMLEnabled(page->settings().coreMathMLEnabled())
    , m_crossOriginCheckInGetMatchedCSSRulesDisabled(page->settings().crossOriginCheckInGetMatchedCSSRulesDisabled())
    , m_defaultFixedFontSize(page->settings().defaultFixedFontSize())
    , m_defaultFontSize(page->settings().defaultFontSize())
    , m_defaultTextEncodingName(page->settings().defaultTextEncodingName())
    , m_defaultVideoPosterURL(page->settings().defaultVideoPosterURL())
    , m_deferredCSSParserEnabled(page->settings().deferredCSSParserEnabled())
    , m_developerExtrasEnabled(page->settings().developerExtrasEnabled())
    , m_deviceHeight(page->settings().deviceHeight())
#if ENABLE(DEVICE_ORIENTATION)
    , m_deviceOrientationEventEnabled(page->settings().deviceOrientationEventEnabled())
#endif
#if ENABLE(DEVICE_ORIENTATION)
    , m_deviceOrientationPermissionAPIEnabled(page->settings().deviceOrientationPermissionAPIEnabled())
#endif
    , m_deviceWidth(page->settings().deviceWidth())
    , m_diagnosticLoggingEnabled(page->settings().diagnosticLoggingEnabled())
    , m_disableScreenSizeOverride(page->settings().disableScreenSizeOverride())
    , m_disallowSyncXHRDuringPageDismissalEnabled(page->settings().disallowSyncXHRDuringPageDismissalEnabled())
    , m_displayListDrawingEnabled(page->settings().displayListDrawingEnabled())
    , m_dnsPrefetchingEnabled(page->settings().dnsPrefetchingEnabled())
    , m_domPasteAccessRequestsEnabled(page->settings().domPasteAccessRequestsEnabled())
    , m_domTimersThrottlingEnabled(page->settings().domTimersThrottlingEnabled())
    , m_downloadableBinaryFontsEnabled(page->settings().downloadableBinaryFontsEnabled())
    , m_editableImagesEnabled(page->settings().editableImagesEnabled())
    , m_enableInheritURIQueryComponent(page->settings().enableInheritURIQueryComponent())
    , m_enforceCSSMIMETypeInNoQuirksMode(page->settings().enforceCSSMIMETypeInNoQuirksMode())
    , m_enterKeyHintEnabled(page->settings().enterKeyHintEnabled())
    , m_experimentalNotificationsEnabled(page->settings().experimentalNotificationsEnabled())
    , m_fixedBackgroundsPaintRelativeToDocument(page->settings().fixedBackgroundsPaintRelativeToDocument())
    , m_fixedElementsLayoutRelativeToFrame(page->settings().fixedElementsLayoutRelativeToFrame())
    , m_fontFallbackPrefersPictographs(page->settings().fontFallbackPrefersPictographs())
    , m_forceCompositingMode(page->settings().forceCompositingMode())
    , m_forceFTPDirectoryListings(page->settings().forceFTPDirectoryListings())
    , m_forcePendingWebGLPolicy(page->settings().isForcePendingWebGLPolicy())
    , m_forceUpdateScrollbarsOnMainThreadForPerformanceTesting(page->settings().forceUpdateScrollbarsOnMainThreadForPerformanceTesting())
    , m_forceWebGLUsesLowPower(page->settings().forceWebGLUsesLowPower())
    , m_ftpDirectoryTemplatePath(page->settings().ftpDirectoryTemplatePath())
#if ENABLE(FULLSCREEN_API)
    , m_fullScreenEnabled(page->settings().fullScreenEnabled())
#endif
#if ENABLE(VIDEO_TRACK)
    , m_genericCueAPIEnabled(page->settings().genericCueAPIEnabled())
#endif
    , m_geolocationFloorLevelEnabled(page->settings().geolocationFloorLevelEnabled())
    , m_googleAntiFlickerOptimizationQuirkEnabled(page->settings().googleAntiFlickerOptimizationQuirkEnabled())
    , m_hdrMediaCapabilitiesEnabled(page->settings().hdrMediaCapabilitiesEnabled())
    , m_hiddenPageCSSAnimationSuspensionEnabled(page->settings().hiddenPageCSSAnimationSuspensionEnabled())
    , m_hiddenPageDOMTimerThrottlingAutoIncreases(page->settings().hiddenPageDOMTimerThrottlingAutoIncreases())
    , m_hiddenPageDOMTimerThrottlingEnabled(page->settings().hiddenPageDOMTimerThrottlingEnabled())
    , m_httpEquivEnabled(page->settings().httpEquivEnabled())
    , m_hyperlinkAuditingEnabled(page->settings().hyperlinkAuditingEnabled())
    , m_iceCandidateFilteringEnabled(page->settings().iceCandidateFilteringEnabled())
#if ENABLE(TEXT_AUTOSIZING)
    , m_idempotentModeAutosizingOnlyHonorsPercentages(page->settings().idempotentModeAutosizingOnlyHonorsPercentages())
#endif
#if ENABLE(SERVICE_CONTROLS)
    , m_imageControlsEnabled(page->settings().imageControlsEnabled())
#endif
    , m_imageSubsamplingEnabled(page->settings().imageSubsamplingEnabled())
    , m_imagesEnabled(page->settings().areImagesEnabled())
    , m_inProcessCookieCacheEnabled(page->settings().inProcessCookieCacheEnabled())
    , m_incompleteImageBorderEnabled(page->settings().incompleteImageBorderEnabled())
    , m_incrementalRenderingSuppressionTimeoutInSeconds(page->settings().incrementalRenderingSuppressionTimeoutInSeconds())
    , m_inlineMediaPlaybackRequiresPlaysInlineAttribute(page->settings().inlineMediaPlaybackRequiresPlaysInlineAttribute())
    , m_inputEventsEnabled(page->settings().inputEventsEnabled())
    , m_interactiveFormValidationEnabled(page->settings().interactiveFormValidationEnabled())
    , m_invisibleAutoplayNotPermitted(page->settings().invisibleAutoplayNotPermitted())
    , m_isAccessibilityIsolatedTreeEnabled(page->settings().isAccessibilityIsolatedTreeEnabled())
    , m_isFirstPartyWebsiteDataRemovalDisabled(page->settings().isFirstPartyWebsiteDataRemovalDisabled())
    , m_isFirstPartyWebsiteDataRemovalLiveOnTestingEnabled(page->settings().isFirstPartyWebsiteDataRemovalLiveOnTestingEnabled())
    , m_isFirstPartyWebsiteDataRemovalReproTestingEnabled(page->settings().isFirstPartyWebsiteDataRemovalReproTestingEnabled())
    , m_isInAppBrowserPrivacyEnabled(page->settings().isInAppBrowserPrivacyEnabled())
    , m_isLoggedInAPIEnabled(page->settings().isLoggedInAPIEnabled())
    , m_isSameSiteStrictEnforcementEnabled(page->settings().isSameSiteStrictEnforcementEnabled())
    , m_isThirdPartyCookieBlockingDisabled(page->settings().isThirdPartyCookieBlockingDisabled())
    , m_javaEnabled(page->settings().isJavaEnabled())
    , m_javaEnabledForLocalFiles(page->settings().isJavaEnabledForLocalFiles())
    , m_javaScriptCanAccessClipboard(page->settings().javaScriptCanAccessClipboard())
    , m_javaScriptCanOpenWindowsAutomatically(page->settings().javaScriptCanOpenWindowsAutomatically())
    , m_langAttributeAwareFormControlUIEnabled(page->settings().langAttributeAwareFormControlUIEnabled())
    , m_largeImageAsyncDecodingEnabled(page->settings().largeImageAsyncDecodingEnabled())
    , m_layoutFallbackWidth(page->settings().layoutFallbackWidth())
    , m_layoutViewportHeightExpansionFactor(page->settings().layoutViewportHeightExpansionFactor())
#if ENABLE(MEDIA_STREAM)
    , m_legacyGetUserMediaEnabled(page->settings().legacyGetUserMediaEnabled())
#endif
#if ENABLE(OVERFLOW_SCROLLING_TOUCH)
    , m_legacyOverflowScrollingTouchEnabled(page->settings().legacyOverflowScrollingTouchEnabled())
#endif
    , m_linkPreconnectEnabled(page->settings().linkPreconnectEnabled())
    , m_loadDeferringEnabled(page->settings().loadDeferringEnabled())
    , m_loadsImagesAutomatically(page->settings().loadsImagesAutomatically())
    , m_loadsSiteIconsIgnoringImageLoadingSetting(page->settings().loadsSiteIconsIgnoringImageLoadingSetting())
    , m_localFileContentSniffingEnabled(page->settings().localFileContentSniffingEnabled())
    , m_localStorageDatabasePath(page->settings().localStorageDatabasePath())
    , m_localStorageEnabled(page->settings().localStorageEnabled())
    , m_logsPageMessagesToSystemConsoleEnabled(page->settings().logsPageMessagesToSystemConsoleEnabled())
    , m_mainContentUserGestureOverrideEnabled(page->settings().mainContentUserGestureOverrideEnabled())
    , m_maxParseDuration(page->settings().maxParseDuration())
    , m_maximumAccelerated2dCanvasSize(page->settings().maximumAccelerated2dCanvasSize())
    , m_maximumHTMLParserDOMTreeDepth(page->settings().maximumHTMLParserDOMTreeDepth())
    , m_maximumPlugInSnapshotAttempts(page->settings().maximumPlugInSnapshotAttempts())
#if ENABLE(MEDIA_SOURCE)
    , m_maximumSourceBufferSize(page->settings().maximumSourceBufferSize())
#endif
    , m_mediaCapabilitiesEnabled(page->settings().mediaCapabilitiesEnabled())
    , m_mediaCapabilitiesExtensionsEnabled(page->settings().mediaCapabilitiesExtensionsEnabled())
#if ENABLE(MEDIA_STREAM)
    , m_mediaCaptureRequiresSecureConnection(page->settings().mediaCaptureRequiresSecureConnection())
#endif
    , m_mediaControlsScaleWithPageZoom(page->settings().mediaControlsScaleWithPageZoom())
    , m_mediaDataLoadsAutomatically(page->settings().mediaDataLoadsAutomatically())
#if ENABLE(MEDIA_STREAM)
    , m_mediaDeviceIdentifierStorageDirectory(page->settings().mediaDeviceIdentifierStorageDirectory())
#endif
    , m_mediaEnabled(page->settings().mediaEnabled())
    , m_mediaKeysStorageDirectory(page->settings().mediaKeysStorageDirectory())
    , m_mediaPreloadingEnabled(page->settings().mediaPreloadingEnabled())
#if ENABLE(MEDIA_SOURCE)
    , m_mediaSourceEnabled(page->settings().mediaSourceEnabled())
#endif
    , m_mediaTypeOverride(page->settings().mediaTypeOverride())
    , m_mediaUserGestureInheritsFromDocument(page->settings().mediaUserGestureInheritsFromDocument())
    , m_minimumAccelerated2dCanvasSize(page->settings().minimumAccelerated2dCanvasSize())
    , m_minimumFontSize(page->settings().minimumFontSize())
    , m_minimumLogicalFontSize(page->settings().minimumLogicalFontSize())
#if ENABLE(TEXT_AUTOSIZING)
    , m_minimumZoomFontSize(page->settings().minimumZoomFontSize())
#endif
#if ENABLE(MEDIA_STREAM)
    , m_mockCaptureDevicesEnabled(page->settings().mockCaptureDevicesEnabled())
#endif
    , m_needsAdobeFrameReloadingQuirk(page->settings().needsAcrobatFrameReloadingQuirk())
    , m_needsDeferKeyDownAndKeyPressTimersUntilNextEditingCommandQuirk(page->settings().needsDeferKeyDownAndKeyPressTimersUntilNextEditingCommandQuirk())
    , m_needsFrameNameFallbackToIdQuirk(page->settings().needsFrameNameFallbackToIdQuirk())
    , m_needsIsLoadingInAPISenseQuirk(page->settings().needsIsLoadingInAPISenseQuirk())
    , m_needsKeyboardEventDisambiguationQuirks(page->settings().needsKeyboardEventDisambiguationQuirks())
    , m_needsSiteSpecificQuirks(page->settings().needsSiteSpecificQuirks())
    , m_needsStorageAccessFromFileURLsQuirk(page->settings().needsStorageAccessFromFileURLsQuirk())
    , m_notificationsEnabled(page->settings().notificationsEnabled())
    , m_offlineWebApplicationCacheEnabled(page->settings().offlineWebApplicationCacheEnabled())
    , m_paginateDuringLayoutEnabled(page->settings().paginateDuringLayoutEnabled())
    , m_passiveTouchListenersAsDefaultOnDocument(page->settings().passiveTouchListenersAsDefaultOnDocument())
    , m_passwordEchoDurationInSeconds(page->settings().passwordEchoDurationInSeconds())
    , m_passwordEchoEnabled(page->settings().passwordEchoEnabled())
#if ENABLE(PAYMENT_REQUEST)
    , m_paymentRequestEnabled(page->settings().paymentRequestEnabled())
#endif
    , m_pictureInPictureAPIEnabled(page->settings().pictureInPictureAPIEnabled())
    , m_plugInSnapshottingEnabled(page->settings().plugInSnapshottingEnabled())
    , m_pluginsEnabled(page->settings().arePluginsEnabled())
    , m_preferMIMETypeForImages(page->settings().preferMIMETypeForImages())
    , m_preventKeyboardDOMEventDispatch(page->settings().preventKeyboardDOMEventDispatch())
    , m_primaryPlugInSnapshotDetectionEnabled(page->settings().primaryPlugInSnapshotDetectionEnabled())
    , m_punchOutWhiteBackgroundsInDarkMode(page->settings().punchOutWhiteBackgroundsInDarkMode())
    , m_quickTimePluginReplacementEnabled(page->settings().quickTimePluginReplacementEnabled())
#if ENABLE(WIRELESS_PLAYBACK_TARGET)
    , m_remotePlaybackEnabled(page->settings().remotePlaybackEnabled())
#endif
    , m_repaintOutsideLayoutEnabled(page->settings().repaintOutsideLayoutEnabled())
    , m_requestAnimationFrameEnabled(page->settings().requestAnimationFrameEnabled())
    , m_requestIdleCallbackEnabled(page->settings().requestIdleCallbackEnabled())
    , m_requiresUserGestureToLoadVideo(page->settings().requiresUserGestureToLoadVideo())
#if ENABLE(RESIZE_OBSERVER)
    , m_resizeObserverEnabled(page->settings().resizeObserverEnabled())
#endif
    , m_resourceUsageOverlayVisible(page->settings().resourceUsageOverlayVisible())
#if ENABLE(RUBBER_BANDING)
    , m_rubberBandingForSubScrollableRegionsEnabled(page->settings().rubberBandingForSubScrollableRegionsEnabled())
#endif
    , m_scriptEnabled(page->settings().isScriptEnabled())
    , m_scriptMarkupEnabled(page->settings().scriptMarkupEnabled())
#if ENABLE(SMOOTH_SCROLLING)
    , m_scrollAnimatorEnabled(page->settings().scrollAnimatorEnabled())
#endif
    , m_scrollingCoordinatorEnabled(page->settings().scrollingCoordinatorEnabled())
    , m_scrollingPerformanceLoggingEnabled(page->settings().scrollingPerformanceLoggingEnabled())
    , m_scrollingTreeIncludesFrames(page->settings().scrollingTreeIncludesFrames())
    , m_selectTrailingWhitespaceEnabled(page->settings().selectTrailingWhitespaceEnabled())
    , m_selectionAcrossShadowBoundariesEnabled(page->settings().selectionAcrossShadowBoundariesEnabled())
    , m_selectionPaintingWithoutSelectionGapsEnabled(page->settings().selectionPaintingWithoutSelectionGapsEnabled())
#if ENABLE(SERVICE_CONTROLS)
    , m_serviceControlsEnabled(page->settings().serviceControlsEnabled())
#endif
    , m_sessionStorageQuota(page->settings().sessionStorageQuota())
    , m_shouldAllowUserInstalledFonts(page->settings().shouldAllowUserInstalledFonts())
    , m_shouldConvertInvalidURLsToBlank(page->settings().shouldConvertInvalidURLsToBlank())
    , m_shouldConvertPositionStyleOnCopy(page->settings().shouldConvertPositionStyleOnCopy())
    , m_shouldDecidePolicyBeforeLoadingQuickLookPreview(page->settings().shouldDecidePolicyBeforeLoadingQuickLookPreview())
    , m_shouldDeferAsynchronousScriptsUntilAfterDocumentLoadOrFirstPaint(page->settings().shouldDeferAsynchronousScriptsUntilAfterDocumentLoadOrFirstPaint())
    , m_shouldDispatchSyntheticMouseEventsWhenModifyingSelection(page->settings().shouldDispatchSyntheticMouseEventsWhenModifyingSelection())
    , m_shouldDispatchSyntheticMouseOutAfterSyntheticClick(page->settings().shouldDispatchSyntheticMouseOutAfterSyntheticClick())
#if ENABLE(VIDEO_TRACK)
    , m_shouldDisplayCaptions(page->settings().shouldDisplayCaptions())
#endif
#if ENABLE(VIDEO_TRACK)
    , m_shouldDisplaySubtitles(page->settings().shouldDisplaySubtitles())
#endif
#if ENABLE(VIDEO_TRACK)
    , m_shouldDisplayTextDescriptions(page->settings().shouldDisplayTextDescriptions())
#endif
#if ENABLE(TEXT_AUTOSIZING)
    , m_shouldEnableTextAutosizingBoost(page->settings().shouldEnableTextAutosizingBoost())
#endif
    , m_shouldIgnoreFontLoadCompletions(page->settings().shouldIgnoreFontLoadCompletions())
    , m_shouldIgnoreMetaViewport(page->settings().shouldIgnoreMetaViewport())
    , m_shouldInjectUserScriptsInInitialEmptyDocument(page->settings().shouldInjectUserScriptsInInitialEmptyDocument())
    , m_shouldPrintBackgrounds(page->settings().shouldPrintBackgrounds())
    , m_shouldRespectImageOrientation(page->settings().shouldRespectImageOrientation())
    , m_shouldRestrictBaseURLSchemes(page->settings().shouldRestrictBaseURLSchemes())
    , m_shouldSuppressTextInputFromEditingDuringProvisionalNavigation(page->settings().shouldSuppressTextInputFromEditingDuringProvisionalNavigation())
    , m_shouldUseServiceWorkerShortTimeout(page->settings().shouldUseServiceWorkerShortTimeout())
    , m_showDebugBorders(page->settings().showDebugBorders())
    , m_showRepaintCounter(page->settings().showRepaintCounter())
    , m_showTiledScrollingIndicator(page->settings().showTiledScrollingIndicator())
    , m_showsToolTipOverTruncatedText(page->settings().showsToolTipOverTruncatedText())
    , m_showsURLsInToolTips(page->settings().showsURLsInToolTips())
    , m_shrinksStandaloneImagesToFit(page->settings().shrinksStandaloneImagesToFit())
    , m_simpleLineLayoutDebugBordersEnabled(page->settings().simpleLineLayoutDebugBordersEnabled())
    , m_simpleLineLayoutEnabled(page->settings().simpleLineLayoutEnabled())
    , m_smartInsertDeleteEnabled(page->settings().smartInsertDeleteEnabled())
    , m_snapshotAllPlugIns(page->settings().snapshotAllPlugIns())
#if ENABLE(MEDIA_SOURCE)
    , m_sourceBufferChangeTypeEnabled(page->settings().sourceBufferChangeTypeEnabled())
#endif
    , m_spatialNavigationEnabled(page->settings().spatialNavigationEnabled())
    , m_springTimingFunctionEnabled(page->settings().springTimingFunctionEnabled())
    , m_standalone(page->settings().standalone())
    , m_storageAccessAPIEnabled(page->settings().storageAccessAPIEnabled())
    , m_subpixelAntialiasedLayerTextEnabled(page->settings().subpixelAntialiasedLayerTextEnabled())
    , m_subpixelCSSOMElementMetricsEnabled(page->settings().subpixelCSSOMElementMetricsEnabled())
    , m_subresourceIntegrityEnabled(page->settings().subresourceIntegrityEnabled())
    , m_suppressesIncrementalRendering(page->settings().suppressesIncrementalRendering())
    , m_syntheticEditingCommandsEnabled(page->settings().syntheticEditingCommandsEnabled())
    , m_telephoneNumberParsingEnabled(page->settings().telephoneNumberParsingEnabled())
    , m_temporaryTileCohortRetentionEnabled(page->settings().temporaryTileCohortRetentionEnabled())
    , m_textAreasAreResizable(page->settings().textAreasAreResizable())
#if ENABLE(TEXT_AUTOSIZING)
    , m_textAutosizingEnabled(page->settings().textAutosizingEnabled())
#endif
#if ENABLE(TEXT_AUTOSIZING)
    , m_textAutosizingUsesIdempotentMode(page->settings().textAutosizingUsesIdempotentMode())
#endif
    , m_thirdPartyIframeRedirectBlockingEnabled(page->settings().thirdPartyIframeRedirectBlockingEnabled())
#if ENABLE(TOUCH_EVENTS)
    , m_touchEventEmulationEnabled(page->settings().isTouchEventEmulationEnabled())
#endif
    , m_treatIPAddressAsDomain(page->settings().treatIPAddressAsDomain())
    , m_treatsAnyTextCSSLinkAsStylesheet(page->settings().treatsAnyTextCSSLinkAsStylesheet())
    , m_unhandledPromiseRejectionToConsoleEnabled(page->settings().unhandledPromiseRejectionToConsoleEnabled())
    , m_unifiedTextCheckerEnabled(page->settings().unifiedTextCheckerEnabled())
    , m_useAnonymousModeWhenFetchingMaskImages(page->settings().useAnonymousModeWhenFetchingMaskImages())
#if ENABLE(GPU_PROCESS)
    , m_useGPUProcessForMedia(page->settings().useGPUProcessForMedia())
#endif
    , m_useGiantTiles(page->settings().useGiantTiles())
    , m_useImageDocumentForSubframePDF(page->settings().useImageDocumentForSubframePDF())
    , m_useLegacyBackgroundSizeShorthandBehavior(page->settings().useLegacyBackgroundSizeShorthandBehavior())
    , m_useLegacyTextAlignPositionedElementBehavior(page->settings().useLegacyTextAlignPositionedElementBehavior())
    , m_usePreHTML5ParserQuirks(page->settings().usePreHTML5ParserQuirks())
    , m_usesEncodingDetector(page->settings().usesEncodingDetector())
    , m_validationMessageTimerMagnification(page->settings().validationMessageTimerMagnification())
    , m_videoPlaybackRequiresUserGesture(page->settings().videoPlaybackRequiresUserGesture())
#if ENABLE(VIDEO)
    , m_videoQualityIncludesDisplayCompositingEnabled(page->settings().videoQualityIncludesDisplayCompositingEnabled())
#endif
    , m_viewportFitEnabled(page->settings().viewportFitEnabled())
    , m_visualViewportAPIEnabled(page->settings().visualViewportAPIEnabled())
    , m_visualViewportEnabled(page->settings().visualViewportEnabled())
    , m_wantsBalancedSetDefersLoadingBehavior(page->settings().wantsBalancedSetDefersLoadingBehavior())
#if ENABLE(WEB_ARCHIVE)
    , m_webArchiveDebugModeEnabled(page->settings().webArchiveDebugModeEnabled())
#endif
    , m_webAudioEnabled(page->settings().webAudioEnabled())
    , m_webGLEnabled(page->settings().webGLEnabled())
    , m_webGLErrorsToConsoleEnabled(page->settings().webGLErrorsToConsoleEnabled())
    , m_webRTCEncryptionEnabled(page->settings().webRTCEncryptionEnabled())
    , m_webSecurityEnabled(page->settings().webSecurityEnabled())
    , m_webkitImageReadyEventEnabled(page->settings().webkitImageReadyEventEnabled())
    , m_windowFocusRestricted(page->settings().windowFocusRestricted())
    , m_xssAuditorEnabled(page->settings().xssAuditorEnabled())
    , m_youTubeFlashPluginReplacementEnabled(page->settings().youTubeFlashPluginReplacementEnabled())
{
}

InternalSettingsGenerated::~InternalSettingsGenerated()
{
}

void InternalSettingsGenerated::resetToConsistentState()
{
    m_page->settings().setCSSOMViewScrollingAPIEnabled(m_CSSOMViewScrollingAPIEnabled);
    m_page->settings().setCSSOMViewSmoothScrollingEnabled(m_CSSOMViewSmoothScrollingEnabled);
    m_page->settings().setDOMPasteAllowed(m_DOMPasteAllowed);
    m_page->settings().setHTTPSUpgradeEnabled(m_HTTPSUpgradeEnabled);
    m_page->settings().setNeedsInAppBrowserPrivacyQuirks(m_NeedsInAppBrowserPrivacyQuirks);
    m_page->settings().setAccelerated2dCanvasEnabled(m_accelerated2dCanvasEnabled);
    m_page->settings().setAcceleratedCompositedAnimationsEnabled(m_acceleratedCompositedAnimationsEnabled);
    m_page->settings().setAcceleratedCompositingEnabled(m_acceleratedCompositingEnabled);
    m_page->settings().setAcceleratedCompositingForFixedPositionEnabled(m_acceleratedCompositingForFixedPositionEnabled);
    m_page->settings().setAcceleratedDrawingEnabled(m_acceleratedDrawingEnabled);
    m_page->settings().setAcceleratedFiltersEnabled(m_acceleratedFiltersEnabled);
    m_page->settings().setAdClickAttributionEnabled(m_adClickAttributionEnabled);
    m_page->settings().setAggressiveTileRetentionEnabled(m_aggressiveTileRetentionEnabled);
    m_page->settings().setAllowContentSecurityPolicySourceStarToMatchAnyProtocol(m_allowContentSecurityPolicySourceStarToMatchAnyProtocol);
    m_page->settings().setAllowCrossOriginSubresourcesToAskForCredentials(m_allowCrossOriginSubresourcesToAskForCredentials);
    m_page->settings().setAllowDisplayOfInsecureContent(m_allowDisplayOfInsecureContent);
    m_page->settings().setAllowFileAccessFromFileURLs(m_allowFileAccessFromFileURLs);
    m_page->settings().setAllowMediaContentTypesRequiringHardwareSupportAsFallback(m_allowMediaContentTypesRequiringHardwareSupportAsFallback);
    m_page->settings().setAllowMultiElementImplicitSubmission(m_allowMultiElementImplicitSubmission);
    m_page->settings().setAllowRunningOfInsecureContent(m_allowRunningOfInsecureContent);
    m_page->settings().setAllowSettingAnyXHRHeaderFromFileURLs(m_allowSettingAnyXHRHeaderFromFileURLs);
    m_page->settings().setAllowTopNavigationToDataURLs(m_allowTopNavigationToDataURLs);
    m_page->settings().setAllowUniversalAccessFromFileURLs(m_allowUniversalAccessFromFileURLs);
    m_page->settings().setAllowViewportShrinkToFitContent(m_allowViewportShrinkToFitContent);
#if ENABLE(WIRELESS_PLAYBACK_TARGET)
    m_page->settings().setAllowsAirPlayForMediaPlayback(m_allowsAirPlayForMediaPlayback);
#endif
    m_page->settings().setAllowsInlineMediaPlayback(m_allowsInlineMediaPlayback);
    m_page->settings().setAllowsInlineMediaPlaybackAfterFullscreen(m_allowsInlineMediaPlaybackAfterFullscreen);
    m_page->settings().setAllowsPictureInPictureMediaPlayback(m_allowsPictureInPictureMediaPlayback);
    m_page->settings().setAlwaysUseAcceleratedOverflowScroll(m_alwaysUseAcceleratedOverflowScroll);
    m_page->settings().setAnimatedImageAsyncDecodingEnabled(m_animatedImageAsyncDecodingEnabled);
    m_page->settings().setAnimatedImageDebugCanvasDrawingEnabled(m_animatedImageDebugCanvasDrawingEnabled);
    m_page->settings().setAppleMailPaginationQuirkEnabled(m_appleMailPaginationQuirkEnabled);
#if ENABLE(APPLE_PAY)
    m_page->settings().setApplePayCapabilityDisclosureAllowed(m_applePayCapabilityDisclosureAllowed);
#endif
#if ENABLE(APPLE_PAY)
    m_page->settings().setApplePayEnabled(m_applePayEnabled);
#endif
#if ENABLE(APPLE_PAY_REMOTE_UI)
    m_page->settings().setApplePayRemoteUIEnabled(m_applePayRemoteUIEnabled);
#endif
    m_page->settings().setAspectRatioOfImgFromWidthAndHeightEnabled(m_aspectRatioOfImgFromWidthAndHeightEnabled);
    m_page->settings().setAsyncClipboardAPIEnabled(m_asyncClipboardAPIEnabled);
    m_page->settings().setAsyncFrameScrollingEnabled(m_asyncFrameScrollingEnabled);
    m_page->settings().setAsyncOverflowScrollingEnabled(m_asyncOverflowScrollingEnabled);
    m_page->settings().setAsynchronousSpellCheckingEnabled(m_asynchronousSpellCheckingEnabled);
    m_page->settings().setAudioPlaybackRequiresUserGesture(m_audioPlaybackRequiresUserGesture);
    m_page->settings().setAuthorAndUserStylesEnabled(m_authorAndUserStylesEnabled);
    m_page->settings().setAutoscrollForDragAndDropEnabled(m_autoscrollForDragAndDropEnabled);
    m_page->settings().setAutostartOriginPlugInSnapshottingEnabled(m_autostartOriginPlugInSnapshottingEnabled);
    m_page->settings().setBackForwardCacheSupportsPlugins(m_backForwardCacheSupportsPlugins);
    m_page->settings().setBackgroundShouldExtendBeyondPage(m_backgroundShouldExtendBeyondPage);
    m_page->settings().setBackspaceKeyNavigationEnabled(m_backspaceKeyNavigationEnabled);
    m_page->settings().setBeaconAPIEnabled(m_beaconAPIEnabled);
    m_page->settings().setBlockingOfSmallPluginsEnabled(m_blockingOfSmallPluginsEnabled);
    m_page->settings().setCanvasUsesAcceleratedDrawing(m_canvasUsesAcceleratedDrawing);
    m_page->settings().setCaretBrowsingEnabled(m_caretBrowsingEnabled);
    m_page->settings().setClientCoordinatesRelativeToLayoutViewport(m_clientCoordinatesRelativeToLayoutViewport);
    m_page->settings().setColorFilterEnabled(m_colorFilterEnabled);
    m_page->settings().setConstantPropertiesEnabled(m_constantPropertiesEnabled);
    m_page->settings().setContentChangeObserverEnabled(m_contentChangeObserverEnabled);
    m_page->settings().setContentDispositionAttachmentSandboxEnabled(m_contentDispositionAttachmentSandboxEnabled);
    m_page->settings().setCookieEnabled(m_cookieEnabled);
    m_page->settings().setCoreMathMLEnabled(m_coreMathMLEnabled);
    m_page->settings().setCrossOriginCheckInGetMatchedCSSRulesDisabled(m_crossOriginCheckInGetMatchedCSSRulesDisabled);
    m_page->settings().setDefaultFixedFontSize(m_defaultFixedFontSize);
    m_page->settings().setDefaultFontSize(m_defaultFontSize);
    m_page->settings().setDefaultTextEncodingName(m_defaultTextEncodingName);
    m_page->settings().setDefaultVideoPosterURL(m_defaultVideoPosterURL);
    m_page->settings().setDeferredCSSParserEnabled(m_deferredCSSParserEnabled);
    m_page->settings().setDeveloperExtrasEnabled(m_developerExtrasEnabled);
    m_page->settings().setDeviceHeight(m_deviceHeight);
#if ENABLE(DEVICE_ORIENTATION)
    m_page->settings().setDeviceOrientationEventEnabled(m_deviceOrientationEventEnabled);
#endif
#if ENABLE(DEVICE_ORIENTATION)
    m_page->settings().setDeviceOrientationPermissionAPIEnabled(m_deviceOrientationPermissionAPIEnabled);
#endif
    m_page->settings().setDeviceWidth(m_deviceWidth);
    m_page->settings().setDiagnosticLoggingEnabled(m_diagnosticLoggingEnabled);
    m_page->settings().setDisableScreenSizeOverride(m_disableScreenSizeOverride);
    m_page->settings().setDisallowSyncXHRDuringPageDismissalEnabled(m_disallowSyncXHRDuringPageDismissalEnabled);
    m_page->settings().setDisplayListDrawingEnabled(m_displayListDrawingEnabled);
    m_page->settings().setDNSPrefetchingEnabled(m_dnsPrefetchingEnabled);
    m_page->settings().setDOMPasteAccessRequestsEnabled(m_domPasteAccessRequestsEnabled);
    m_page->settings().setDOMTimersThrottlingEnabled(m_domTimersThrottlingEnabled);
    m_page->settings().setDownloadableBinaryFontsEnabled(m_downloadableBinaryFontsEnabled);
    m_page->settings().setEditableImagesEnabled(m_editableImagesEnabled);
    m_page->settings().setEnableInheritURIQueryComponent(m_enableInheritURIQueryComponent);
    m_page->settings().setEnforceCSSMIMETypeInNoQuirksMode(m_enforceCSSMIMETypeInNoQuirksMode);
    m_page->settings().setEnterKeyHintEnabled(m_enterKeyHintEnabled);
    m_page->settings().setExperimentalNotificationsEnabled(m_experimentalNotificationsEnabled);
    m_page->settings().setFixedBackgroundsPaintRelativeToDocument(m_fixedBackgroundsPaintRelativeToDocument);
    m_page->settings().setFixedElementsLayoutRelativeToFrame(m_fixedElementsLayoutRelativeToFrame);
    m_page->settings().setFontFallbackPrefersPictographs(m_fontFallbackPrefersPictographs);
    m_page->settings().setForceCompositingMode(m_forceCompositingMode);
    m_page->settings().setForceFTPDirectoryListings(m_forceFTPDirectoryListings);
    m_page->settings().setForcePendingWebGLPolicy(m_forcePendingWebGLPolicy);
    m_page->settings().setForceUpdateScrollbarsOnMainThreadForPerformanceTesting(m_forceUpdateScrollbarsOnMainThreadForPerformanceTesting);
    m_page->settings().setForceWebGLUsesLowPower(m_forceWebGLUsesLowPower);
    m_page->settings().setFTPDirectoryTemplatePath(m_ftpDirectoryTemplatePath);
#if ENABLE(FULLSCREEN_API)
    m_page->settings().setFullScreenEnabled(m_fullScreenEnabled);
#endif
#if ENABLE(VIDEO_TRACK)
    m_page->settings().setGenericCueAPIEnabled(m_genericCueAPIEnabled);
#endif
    m_page->settings().setGeolocationFloorLevelEnabled(m_geolocationFloorLevelEnabled);
    m_page->settings().setGoogleAntiFlickerOptimizationQuirkEnabled(m_googleAntiFlickerOptimizationQuirkEnabled);
    m_page->settings().setHDRMediaCapabilitiesEnabled(m_hdrMediaCapabilitiesEnabled);
    m_page->settings().setHiddenPageCSSAnimationSuspensionEnabled(m_hiddenPageCSSAnimationSuspensionEnabled);
    m_page->settings().setHiddenPageDOMTimerThrottlingAutoIncreases(m_hiddenPageDOMTimerThrottlingAutoIncreases);
    m_page->settings().setHiddenPageDOMTimerThrottlingEnabled(m_hiddenPageDOMTimerThrottlingEnabled);
    m_page->settings().setHttpEquivEnabled(m_httpEquivEnabled);
    m_page->settings().setHyperlinkAuditingEnabled(m_hyperlinkAuditingEnabled);
    m_page->settings().setICECandidateFilteringEnabled(m_iceCandidateFilteringEnabled);
#if ENABLE(TEXT_AUTOSIZING)
    m_page->settings().setIdempotentModeAutosizingOnlyHonorsPercentages(m_idempotentModeAutosizingOnlyHonorsPercentages);
#endif
#if ENABLE(SERVICE_CONTROLS)
    m_page->settings().setImageControlsEnabled(m_imageControlsEnabled);
#endif
    m_page->settings().setImageSubsamplingEnabled(m_imageSubsamplingEnabled);
    m_page->settings().setImagesEnabled(m_imagesEnabled);
    m_page->settings().setInProcessCookieCacheEnabled(m_inProcessCookieCacheEnabled);
    m_page->settings().setIncompleteImageBorderEnabled(m_incompleteImageBorderEnabled);
    m_page->settings().setIncrementalRenderingSuppressionTimeoutInSeconds(m_incrementalRenderingSuppressionTimeoutInSeconds);
    m_page->settings().setInlineMediaPlaybackRequiresPlaysInlineAttribute(m_inlineMediaPlaybackRequiresPlaysInlineAttribute);
    m_page->settings().setInputEventsEnabled(m_inputEventsEnabled);
    m_page->settings().setInteractiveFormValidationEnabled(m_interactiveFormValidationEnabled);
    m_page->settings().setInvisibleAutoplayNotPermitted(m_invisibleAutoplayNotPermitted);
    m_page->settings().setIsAccessibilityIsolatedTreeEnabled(m_isAccessibilityIsolatedTreeEnabled);
    m_page->settings().setIsFirstPartyWebsiteDataRemovalDisabled(m_isFirstPartyWebsiteDataRemovalDisabled);
    m_page->settings().setIsFirstPartyWebsiteDataRemovalLiveOnTestingEnabled(m_isFirstPartyWebsiteDataRemovalLiveOnTestingEnabled);
    m_page->settings().setIsFirstPartyWebsiteDataRemovalReproTestingEnabled(m_isFirstPartyWebsiteDataRemovalReproTestingEnabled);
    m_page->settings().setIsInAppBrowserPrivacyEnabled(m_isInAppBrowserPrivacyEnabled);
    m_page->settings().setIsLoggedInAPIEnabled(m_isLoggedInAPIEnabled);
    m_page->settings().setIsSameSiteStrictEnforcementEnabled(m_isSameSiteStrictEnforcementEnabled);
    m_page->settings().setIsThirdPartyCookieBlockingDisabled(m_isThirdPartyCookieBlockingDisabled);
    m_page->settings().setJavaEnabled(m_javaEnabled);
    m_page->settings().setJavaEnabledForLocalFiles(m_javaEnabledForLocalFiles);
    m_page->settings().setJavaScriptCanAccessClipboard(m_javaScriptCanAccessClipboard);
    m_page->settings().setJavaScriptCanOpenWindowsAutomatically(m_javaScriptCanOpenWindowsAutomatically);
    m_page->settings().setLangAttributeAwareFormControlUIEnabled(m_langAttributeAwareFormControlUIEnabled);
    m_page->settings().setLargeImageAsyncDecodingEnabled(m_largeImageAsyncDecodingEnabled);
    m_page->settings().setLayoutFallbackWidth(m_layoutFallbackWidth);
    m_page->settings().setLayoutViewportHeightExpansionFactor(m_layoutViewportHeightExpansionFactor);
#if ENABLE(MEDIA_STREAM)
    m_page->settings().setLegacyGetUserMediaEnabled(m_legacyGetUserMediaEnabled);
#endif
#if ENABLE(OVERFLOW_SCROLLING_TOUCH)
    m_page->settings().setLegacyOverflowScrollingTouchEnabled(m_legacyOverflowScrollingTouchEnabled);
#endif
    m_page->settings().setLinkPreconnectEnabled(m_linkPreconnectEnabled);
    m_page->settings().setLoadDeferringEnabled(m_loadDeferringEnabled);
    m_page->settings().setLoadsImagesAutomatically(m_loadsImagesAutomatically);
    m_page->settings().setLoadsSiteIconsIgnoringImageLoadingSetting(m_loadsSiteIconsIgnoringImageLoadingSetting);
    m_page->settings().setLocalFileContentSniffingEnabled(m_localFileContentSniffingEnabled);
    m_page->settings().setLocalStorageDatabasePath(m_localStorageDatabasePath);
    m_page->settings().setLocalStorageEnabled(m_localStorageEnabled);
    m_page->settings().setLogsPageMessagesToSystemConsoleEnabled(m_logsPageMessagesToSystemConsoleEnabled);
    m_page->settings().setMainContentUserGestureOverrideEnabled(m_mainContentUserGestureOverrideEnabled);
    m_page->settings().setMaxParseDuration(m_maxParseDuration);
    m_page->settings().setMaximumAccelerated2dCanvasSize(m_maximumAccelerated2dCanvasSize);
    m_page->settings().setMaximumHTMLParserDOMTreeDepth(m_maximumHTMLParserDOMTreeDepth);
    m_page->settings().setMaximumPlugInSnapshotAttempts(m_maximumPlugInSnapshotAttempts);
#if ENABLE(MEDIA_SOURCE)
    m_page->settings().setMaximumSourceBufferSize(m_maximumSourceBufferSize);
#endif
    m_page->settings().setMediaCapabilitiesEnabled(m_mediaCapabilitiesEnabled);
    m_page->settings().setMediaCapabilitiesExtensionsEnabled(m_mediaCapabilitiesExtensionsEnabled);
#if ENABLE(MEDIA_STREAM)
    m_page->settings().setMediaCaptureRequiresSecureConnection(m_mediaCaptureRequiresSecureConnection);
#endif
    m_page->settings().setMediaControlsScaleWithPageZoom(m_mediaControlsScaleWithPageZoom);
    m_page->settings().setMediaDataLoadsAutomatically(m_mediaDataLoadsAutomatically);
#if ENABLE(MEDIA_STREAM)
    m_page->settings().setMediaDeviceIdentifierStorageDirectory(m_mediaDeviceIdentifierStorageDirectory);
#endif
    m_page->settings().setMediaEnabled(m_mediaEnabled);
    m_page->settings().setMediaKeysStorageDirectory(m_mediaKeysStorageDirectory);
    m_page->settings().setMediaPreloadingEnabled(m_mediaPreloadingEnabled);
#if ENABLE(MEDIA_SOURCE)
    m_page->settings().setMediaSourceEnabled(m_mediaSourceEnabled);
#endif
    m_page->settings().setMediaTypeOverride(m_mediaTypeOverride);
    m_page->settings().setMediaUserGestureInheritsFromDocument(m_mediaUserGestureInheritsFromDocument);
    m_page->settings().setMinimumAccelerated2dCanvasSize(m_minimumAccelerated2dCanvasSize);
    m_page->settings().setMinimumFontSize(m_minimumFontSize);
    m_page->settings().setMinimumLogicalFontSize(m_minimumLogicalFontSize);
#if ENABLE(TEXT_AUTOSIZING)
    m_page->settings().setMinimumZoomFontSize(m_minimumZoomFontSize);
#endif
#if ENABLE(MEDIA_STREAM)
    m_page->settings().setMockCaptureDevicesEnabled(m_mockCaptureDevicesEnabled);
#endif
    m_page->settings().setNeedsAdobeFrameReloadingQuirk(m_needsAdobeFrameReloadingQuirk);
    m_page->settings().setNeedsDeferKeyDownAndKeyPressTimersUntilNextEditingCommandQuirk(m_needsDeferKeyDownAndKeyPressTimersUntilNextEditingCommandQuirk);
    m_page->settings().setNeedsFrameNameFallbackToIdQuirk(m_needsFrameNameFallbackToIdQuirk);
    m_page->settings().setNeedsIsLoadingInAPISenseQuirk(m_needsIsLoadingInAPISenseQuirk);
    m_page->settings().setNeedsKeyboardEventDisambiguationQuirks(m_needsKeyboardEventDisambiguationQuirks);
    m_page->settings().setNeedsSiteSpecificQuirks(m_needsSiteSpecificQuirks);
    m_page->settings().setNeedsStorageAccessFromFileURLsQuirk(m_needsStorageAccessFromFileURLsQuirk);
    m_page->settings().setNotificationsEnabled(m_notificationsEnabled);
    m_page->settings().setOfflineWebApplicationCacheEnabled(m_offlineWebApplicationCacheEnabled);
    m_page->settings().setPaginateDuringLayoutEnabled(m_paginateDuringLayoutEnabled);
    m_page->settings().setPassiveTouchListenersAsDefaultOnDocument(m_passiveTouchListenersAsDefaultOnDocument);
    m_page->settings().setPasswordEchoDurationInSeconds(m_passwordEchoDurationInSeconds);
    m_page->settings().setPasswordEchoEnabled(m_passwordEchoEnabled);
#if ENABLE(PAYMENT_REQUEST)
    m_page->settings().setPaymentRequestEnabled(m_paymentRequestEnabled);
#endif
    m_page->settings().setPictureInPictureAPIEnabled(m_pictureInPictureAPIEnabled);
    m_page->settings().setPlugInSnapshottingEnabled(m_plugInSnapshottingEnabled);
    m_page->settings().setPluginsEnabled(m_pluginsEnabled);
    m_page->settings().setPreferMIMETypeForImages(m_preferMIMETypeForImages);
    m_page->settings().setPreventKeyboardDOMEventDispatch(m_preventKeyboardDOMEventDispatch);
    m_page->settings().setPrimaryPlugInSnapshotDetectionEnabled(m_primaryPlugInSnapshotDetectionEnabled);
    m_page->settings().setPunchOutWhiteBackgroundsInDarkMode(m_punchOutWhiteBackgroundsInDarkMode);
    m_page->settings().setQuickTimePluginReplacementEnabled(m_quickTimePluginReplacementEnabled);
#if ENABLE(WIRELESS_PLAYBACK_TARGET)
    m_page->settings().setRemotePlaybackEnabled(m_remotePlaybackEnabled);
#endif
    m_page->settings().setRepaintOutsideLayoutEnabled(m_repaintOutsideLayoutEnabled);
    m_page->settings().setRequestAnimationFrameEnabled(m_requestAnimationFrameEnabled);
    m_page->settings().setRequestIdleCallbackEnabled(m_requestIdleCallbackEnabled);
    m_page->settings().setRequiresUserGestureToLoadVideo(m_requiresUserGestureToLoadVideo);
#if ENABLE(RESIZE_OBSERVER)
    m_page->settings().setResizeObserverEnabled(m_resizeObserverEnabled);
#endif
    m_page->settings().setResourceUsageOverlayVisible(m_resourceUsageOverlayVisible);
#if ENABLE(RUBBER_BANDING)
    m_page->settings().setRubberBandingForSubScrollableRegionsEnabled(m_rubberBandingForSubScrollableRegionsEnabled);
#endif
    m_page->settings().setScriptEnabled(m_scriptEnabled);
    m_page->settings().setScriptMarkupEnabled(m_scriptMarkupEnabled);
#if ENABLE(SMOOTH_SCROLLING)
    m_page->settings().setScrollAnimatorEnabled(m_scrollAnimatorEnabled);
#endif
    m_page->settings().setScrollingCoordinatorEnabled(m_scrollingCoordinatorEnabled);
    m_page->settings().setScrollingPerformanceLoggingEnabled(m_scrollingPerformanceLoggingEnabled);
    m_page->settings().setScrollingTreeIncludesFrames(m_scrollingTreeIncludesFrames);
    m_page->settings().setSelectTrailingWhitespaceEnabled(m_selectTrailingWhitespaceEnabled);
    m_page->settings().setSelectionAcrossShadowBoundariesEnabled(m_selectionAcrossShadowBoundariesEnabled);
    m_page->settings().setSelectionPaintingWithoutSelectionGapsEnabled(m_selectionPaintingWithoutSelectionGapsEnabled);
#if ENABLE(SERVICE_CONTROLS)
    m_page->settings().setServiceControlsEnabled(m_serviceControlsEnabled);
#endif
    m_page->settings().setSessionStorageQuota(m_sessionStorageQuota);
    m_page->settings().setShouldAllowUserInstalledFonts(m_shouldAllowUserInstalledFonts);
    m_page->settings().setShouldConvertInvalidURLsToBlank(m_shouldConvertInvalidURLsToBlank);
    m_page->settings().setShouldConvertPositionStyleOnCopy(m_shouldConvertPositionStyleOnCopy);
    m_page->settings().setShouldDecidePolicyBeforeLoadingQuickLookPreview(m_shouldDecidePolicyBeforeLoadingQuickLookPreview);
    m_page->settings().setShouldDeferAsynchronousScriptsUntilAfterDocumentLoadOrFirstPaint(m_shouldDeferAsynchronousScriptsUntilAfterDocumentLoadOrFirstPaint);
    m_page->settings().setShouldDispatchSyntheticMouseEventsWhenModifyingSelection(m_shouldDispatchSyntheticMouseEventsWhenModifyingSelection);
    m_page->settings().setShouldDispatchSyntheticMouseOutAfterSyntheticClick(m_shouldDispatchSyntheticMouseOutAfterSyntheticClick);
#if ENABLE(VIDEO_TRACK)
    m_page->settings().setShouldDisplayCaptions(m_shouldDisplayCaptions);
#endif
#if ENABLE(VIDEO_TRACK)
    m_page->settings().setShouldDisplaySubtitles(m_shouldDisplaySubtitles);
#endif
#if ENABLE(VIDEO_TRACK)
    m_page->settings().setShouldDisplayTextDescriptions(m_shouldDisplayTextDescriptions);
#endif
#if ENABLE(TEXT_AUTOSIZING)
    m_page->settings().setShouldEnableTextAutosizingBoost(m_shouldEnableTextAutosizingBoost);
#endif
    m_page->settings().setShouldIgnoreFontLoadCompletions(m_shouldIgnoreFontLoadCompletions);
    m_page->settings().setShouldIgnoreMetaViewport(m_shouldIgnoreMetaViewport);
    m_page->settings().setShouldInjectUserScriptsInInitialEmptyDocument(m_shouldInjectUserScriptsInInitialEmptyDocument);
    m_page->settings().setShouldPrintBackgrounds(m_shouldPrintBackgrounds);
    m_page->settings().setShouldRespectImageOrientation(m_shouldRespectImageOrientation);
    m_page->settings().setShouldRestrictBaseURLSchemes(m_shouldRestrictBaseURLSchemes);
    m_page->settings().setShouldSuppressTextInputFromEditingDuringProvisionalNavigation(m_shouldSuppressTextInputFromEditingDuringProvisionalNavigation);
    m_page->settings().setShouldUseServiceWorkerShortTimeout(m_shouldUseServiceWorkerShortTimeout);
    m_page->settings().setShowDebugBorders(m_showDebugBorders);
    m_page->settings().setShowRepaintCounter(m_showRepaintCounter);
    m_page->settings().setShowTiledScrollingIndicator(m_showTiledScrollingIndicator);
    m_page->settings().setShowsToolTipOverTruncatedText(m_showsToolTipOverTruncatedText);
    m_page->settings().setShowsURLsInToolTips(m_showsURLsInToolTips);
    m_page->settings().setShrinksStandaloneImagesToFit(m_shrinksStandaloneImagesToFit);
    m_page->settings().setSimpleLineLayoutDebugBordersEnabled(m_simpleLineLayoutDebugBordersEnabled);
    m_page->settings().setSimpleLineLayoutEnabled(m_simpleLineLayoutEnabled);
    m_page->settings().setSmartInsertDeleteEnabled(m_smartInsertDeleteEnabled);
    m_page->settings().setSnapshotAllPlugIns(m_snapshotAllPlugIns);
#if ENABLE(MEDIA_SOURCE)
    m_page->settings().setSourceBufferChangeTypeEnabled(m_sourceBufferChangeTypeEnabled);
#endif
    m_page->settings().setSpatialNavigationEnabled(m_spatialNavigationEnabled);
    m_page->settings().setSpringTimingFunctionEnabled(m_springTimingFunctionEnabled);
    m_page->settings().setStandalone(m_standalone);
    m_page->settings().setStorageAccessAPIEnabled(m_storageAccessAPIEnabled);
    m_page->settings().setSubpixelAntialiasedLayerTextEnabled(m_subpixelAntialiasedLayerTextEnabled);
    m_page->settings().setSubpixelCSSOMElementMetricsEnabled(m_subpixelCSSOMElementMetricsEnabled);
    m_page->settings().setSubresourceIntegrityEnabled(m_subresourceIntegrityEnabled);
    m_page->settings().setSuppressesIncrementalRendering(m_suppressesIncrementalRendering);
    m_page->settings().setSyntheticEditingCommandsEnabled(m_syntheticEditingCommandsEnabled);
    m_page->settings().setTelephoneNumberParsingEnabled(m_telephoneNumberParsingEnabled);
    m_page->settings().setTemporaryTileCohortRetentionEnabled(m_temporaryTileCohortRetentionEnabled);
    m_page->settings().setTextAreasAreResizable(m_textAreasAreResizable);
#if ENABLE(TEXT_AUTOSIZING)
    m_page->settings().setTextAutosizingEnabled(m_textAutosizingEnabled);
#endif
#if ENABLE(TEXT_AUTOSIZING)
    m_page->settings().setTextAutosizingUsesIdempotentMode(m_textAutosizingUsesIdempotentMode);
#endif
    m_page->settings().setThirdPartyIframeRedirectBlockingEnabled(m_thirdPartyIframeRedirectBlockingEnabled);
#if ENABLE(TOUCH_EVENTS)
    m_page->settings().setTouchEventEmulationEnabled(m_touchEventEmulationEnabled);
#endif
    m_page->settings().setTreatIPAddressAsDomain(m_treatIPAddressAsDomain);
    m_page->settings().setTreatsAnyTextCSSLinkAsStylesheet(m_treatsAnyTextCSSLinkAsStylesheet);
    m_page->settings().setUnhandledPromiseRejectionToConsoleEnabled(m_unhandledPromiseRejectionToConsoleEnabled);
    m_page->settings().setUnifiedTextCheckerEnabled(m_unifiedTextCheckerEnabled);
    m_page->settings().setUseAnonymousModeWhenFetchingMaskImages(m_useAnonymousModeWhenFetchingMaskImages);
#if ENABLE(GPU_PROCESS)
    m_page->settings().setUseGPUProcessForMedia(m_useGPUProcessForMedia);
#endif
    m_page->settings().setUseGiantTiles(m_useGiantTiles);
    m_page->settings().setUseImageDocumentForSubframePDF(m_useImageDocumentForSubframePDF);
    m_page->settings().setUseLegacyBackgroundSizeShorthandBehavior(m_useLegacyBackgroundSizeShorthandBehavior);
    m_page->settings().setUseLegacyTextAlignPositionedElementBehavior(m_useLegacyTextAlignPositionedElementBehavior);
    m_page->settings().setUsePreHTML5ParserQuirks(m_usePreHTML5ParserQuirks);
    m_page->settings().setUsesEncodingDetector(m_usesEncodingDetector);
    m_page->settings().setValidationMessageTimerMagnification(m_validationMessageTimerMagnification);
    m_page->settings().setVideoPlaybackRequiresUserGesture(m_videoPlaybackRequiresUserGesture);
#if ENABLE(VIDEO)
    m_page->settings().setVideoQualityIncludesDisplayCompositingEnabled(m_videoQualityIncludesDisplayCompositingEnabled);
#endif
    m_page->settings().setViewportFitEnabled(m_viewportFitEnabled);
    m_page->settings().setVisualViewportAPIEnabled(m_visualViewportAPIEnabled);
    m_page->settings().setVisualViewportEnabled(m_visualViewportEnabled);
    m_page->settings().setWantsBalancedSetDefersLoadingBehavior(m_wantsBalancedSetDefersLoadingBehavior);
#if ENABLE(WEB_ARCHIVE)
    m_page->settings().setWebArchiveDebugModeEnabled(m_webArchiveDebugModeEnabled);
#endif
    m_page->settings().setWebAudioEnabled(m_webAudioEnabled);
    m_page->settings().setWebGLEnabled(m_webGLEnabled);
    m_page->settings().setWebGLErrorsToConsoleEnabled(m_webGLErrorsToConsoleEnabled);
    m_page->settings().setWebRTCEncryptionEnabled(m_webRTCEncryptionEnabled);
    m_page->settings().setWebSecurityEnabled(m_webSecurityEnabled);
    m_page->settings().setWebkitImageReadyEventEnabled(m_webkitImageReadyEventEnabled);
    m_page->settings().setWindowFocusRestricted(m_windowFocusRestricted);
    m_page->settings().setXSSAuditorEnabled(m_xssAuditorEnabled);
    m_page->settings().setYouTubeFlashPluginReplacementEnabled(m_youTubeFlashPluginReplacementEnabled);
}

void InternalSettingsGenerated::setCSSOMViewScrollingAPIEnabled(bool CSSOMViewScrollingAPIEnabled)
{
    m_page->settings().setCSSOMViewScrollingAPIEnabled(CSSOMViewScrollingAPIEnabled);
}

void InternalSettingsGenerated::setCSSOMViewSmoothScrollingEnabled(bool CSSOMViewSmoothScrollingEnabled)
{
    m_page->settings().setCSSOMViewSmoothScrollingEnabled(CSSOMViewSmoothScrollingEnabled);
}

void InternalSettingsGenerated::setDOMPasteAllowed(bool DOMPasteAllowed)
{
    m_page->settings().setDOMPasteAllowed(DOMPasteAllowed);
}

void InternalSettingsGenerated::setHTTPSUpgradeEnabled(bool HTTPSUpgradeEnabled)
{
    m_page->settings().setHTTPSUpgradeEnabled(HTTPSUpgradeEnabled);
}

void InternalSettingsGenerated::setNeedsInAppBrowserPrivacyQuirks(bool NeedsInAppBrowserPrivacyQuirks)
{
    m_page->settings().setNeedsInAppBrowserPrivacyQuirks(NeedsInAppBrowserPrivacyQuirks);
}

void InternalSettingsGenerated::setAccelerated2dCanvasEnabled(bool accelerated2dCanvasEnabled)
{
    m_page->settings().setAccelerated2dCanvasEnabled(accelerated2dCanvasEnabled);
}

void InternalSettingsGenerated::setAcceleratedCompositedAnimationsEnabled(bool acceleratedCompositedAnimationsEnabled)
{
    m_page->settings().setAcceleratedCompositedAnimationsEnabled(acceleratedCompositedAnimationsEnabled);
}

void InternalSettingsGenerated::setAcceleratedCompositingEnabled(bool acceleratedCompositingEnabled)
{
    m_page->settings().setAcceleratedCompositingEnabled(acceleratedCompositingEnabled);
}

void InternalSettingsGenerated::setAcceleratedCompositingForFixedPositionEnabled(bool acceleratedCompositingForFixedPositionEnabled)
{
    m_page->settings().setAcceleratedCompositingForFixedPositionEnabled(acceleratedCompositingForFixedPositionEnabled);
}

void InternalSettingsGenerated::setAcceleratedDrawingEnabled(bool acceleratedDrawingEnabled)
{
    m_page->settings().setAcceleratedDrawingEnabled(acceleratedDrawingEnabled);
}

void InternalSettingsGenerated::setAcceleratedFiltersEnabled(bool acceleratedFiltersEnabled)
{
    m_page->settings().setAcceleratedFiltersEnabled(acceleratedFiltersEnabled);
}

void InternalSettingsGenerated::setAdClickAttributionEnabled(bool adClickAttributionEnabled)
{
    m_page->settings().setAdClickAttributionEnabled(adClickAttributionEnabled);
}

void InternalSettingsGenerated::setAggressiveTileRetentionEnabled(bool aggressiveTileRetentionEnabled)
{
    m_page->settings().setAggressiveTileRetentionEnabled(aggressiveTileRetentionEnabled);
}

void InternalSettingsGenerated::setAllowContentSecurityPolicySourceStarToMatchAnyProtocol(bool allowContentSecurityPolicySourceStarToMatchAnyProtocol)
{
    m_page->settings().setAllowContentSecurityPolicySourceStarToMatchAnyProtocol(allowContentSecurityPolicySourceStarToMatchAnyProtocol);
}

void InternalSettingsGenerated::setAllowCrossOriginSubresourcesToAskForCredentials(bool allowCrossOriginSubresourcesToAskForCredentials)
{
    m_page->settings().setAllowCrossOriginSubresourcesToAskForCredentials(allowCrossOriginSubresourcesToAskForCredentials);
}

void InternalSettingsGenerated::setAllowDisplayOfInsecureContent(bool allowDisplayOfInsecureContent)
{
    m_page->settings().setAllowDisplayOfInsecureContent(allowDisplayOfInsecureContent);
}

void InternalSettingsGenerated::setAllowFileAccessFromFileURLs(bool allowFileAccessFromFileURLs)
{
    m_page->settings().setAllowFileAccessFromFileURLs(allowFileAccessFromFileURLs);
}

void InternalSettingsGenerated::setAllowMediaContentTypesRequiringHardwareSupportAsFallback(bool allowMediaContentTypesRequiringHardwareSupportAsFallback)
{
    m_page->settings().setAllowMediaContentTypesRequiringHardwareSupportAsFallback(allowMediaContentTypesRequiringHardwareSupportAsFallback);
}

void InternalSettingsGenerated::setAllowMultiElementImplicitSubmission(bool allowMultiElementImplicitSubmission)
{
    m_page->settings().setAllowMultiElementImplicitSubmission(allowMultiElementImplicitSubmission);
}

void InternalSettingsGenerated::setAllowRunningOfInsecureContent(bool allowRunningOfInsecureContent)
{
    m_page->settings().setAllowRunningOfInsecureContent(allowRunningOfInsecureContent);
}

void InternalSettingsGenerated::setAllowSettingAnyXHRHeaderFromFileURLs(bool allowSettingAnyXHRHeaderFromFileURLs)
{
    m_page->settings().setAllowSettingAnyXHRHeaderFromFileURLs(allowSettingAnyXHRHeaderFromFileURLs);
}

void InternalSettingsGenerated::setAllowTopNavigationToDataURLs(bool allowTopNavigationToDataURLs)
{
    m_page->settings().setAllowTopNavigationToDataURLs(allowTopNavigationToDataURLs);
}

void InternalSettingsGenerated::setAllowUniversalAccessFromFileURLs(bool allowUniversalAccessFromFileURLs)
{
    m_page->settings().setAllowUniversalAccessFromFileURLs(allowUniversalAccessFromFileURLs);
}

void InternalSettingsGenerated::setAllowViewportShrinkToFitContent(bool allowViewportShrinkToFitContent)
{
    m_page->settings().setAllowViewportShrinkToFitContent(allowViewportShrinkToFitContent);
}

void InternalSettingsGenerated::setAllowsAirPlayForMediaPlayback(bool allowsAirPlayForMediaPlayback)
{
#if ENABLE(WIRELESS_PLAYBACK_TARGET)
    m_page->settings().setAllowsAirPlayForMediaPlayback(allowsAirPlayForMediaPlayback);
#else
    UNUSED_PARAM(allowsAirPlayForMediaPlayback);
#endif
}

void InternalSettingsGenerated::setAllowsInlineMediaPlayback(bool allowsInlineMediaPlayback)
{
    m_page->settings().setAllowsInlineMediaPlayback(allowsInlineMediaPlayback);
}

void InternalSettingsGenerated::setAllowsInlineMediaPlaybackAfterFullscreen(bool allowsInlineMediaPlaybackAfterFullscreen)
{
    m_page->settings().setAllowsInlineMediaPlaybackAfterFullscreen(allowsInlineMediaPlaybackAfterFullscreen);
}

void InternalSettingsGenerated::setAllowsPictureInPictureMediaPlayback(bool allowsPictureInPictureMediaPlayback)
{
    m_page->settings().setAllowsPictureInPictureMediaPlayback(allowsPictureInPictureMediaPlayback);
}

void InternalSettingsGenerated::setAlwaysUseAcceleratedOverflowScroll(bool alwaysUseAcceleratedOverflowScroll)
{
    m_page->settings().setAlwaysUseAcceleratedOverflowScroll(alwaysUseAcceleratedOverflowScroll);
}

void InternalSettingsGenerated::setAnimatedImageAsyncDecodingEnabled(bool animatedImageAsyncDecodingEnabled)
{
    m_page->settings().setAnimatedImageAsyncDecodingEnabled(animatedImageAsyncDecodingEnabled);
}

void InternalSettingsGenerated::setAnimatedImageDebugCanvasDrawingEnabled(bool animatedImageDebugCanvasDrawingEnabled)
{
    m_page->settings().setAnimatedImageDebugCanvasDrawingEnabled(animatedImageDebugCanvasDrawingEnabled);
}

void InternalSettingsGenerated::setAppleMailPaginationQuirkEnabled(bool appleMailPaginationQuirkEnabled)
{
    m_page->settings().setAppleMailPaginationQuirkEnabled(appleMailPaginationQuirkEnabled);
}

void InternalSettingsGenerated::setApplePayCapabilityDisclosureAllowed(bool applePayCapabilityDisclosureAllowed)
{
#if ENABLE(APPLE_PAY)
    m_page->settings().setApplePayCapabilityDisclosureAllowed(applePayCapabilityDisclosureAllowed);
#else
    UNUSED_PARAM(applePayCapabilityDisclosureAllowed);
#endif
}

void InternalSettingsGenerated::setApplePayEnabled(bool applePayEnabled)
{
#if ENABLE(APPLE_PAY)
    m_page->settings().setApplePayEnabled(applePayEnabled);
#else
    UNUSED_PARAM(applePayEnabled);
#endif
}

void InternalSettingsGenerated::setApplePayRemoteUIEnabled(bool applePayRemoteUIEnabled)
{
#if ENABLE(APPLE_PAY_REMOTE_UI)
    m_page->settings().setApplePayRemoteUIEnabled(applePayRemoteUIEnabled);
#else
    UNUSED_PARAM(applePayRemoteUIEnabled);
#endif
}

void InternalSettingsGenerated::setAspectRatioOfImgFromWidthAndHeightEnabled(bool aspectRatioOfImgFromWidthAndHeightEnabled)
{
    m_page->settings().setAspectRatioOfImgFromWidthAndHeightEnabled(aspectRatioOfImgFromWidthAndHeightEnabled);
}

void InternalSettingsGenerated::setAsyncClipboardAPIEnabled(bool asyncClipboardAPIEnabled)
{
    m_page->settings().setAsyncClipboardAPIEnabled(asyncClipboardAPIEnabled);
}

void InternalSettingsGenerated::setAsyncFrameScrollingEnabled(bool asyncFrameScrollingEnabled)
{
    m_page->settings().setAsyncFrameScrollingEnabled(asyncFrameScrollingEnabled);
}

void InternalSettingsGenerated::setAsyncOverflowScrollingEnabled(bool asyncOverflowScrollingEnabled)
{
    m_page->settings().setAsyncOverflowScrollingEnabled(asyncOverflowScrollingEnabled);
}

void InternalSettingsGenerated::setAsynchronousSpellCheckingEnabled(bool asynchronousSpellCheckingEnabled)
{
    m_page->settings().setAsynchronousSpellCheckingEnabled(asynchronousSpellCheckingEnabled);
}

void InternalSettingsGenerated::setAudioPlaybackRequiresUserGesture(bool audioPlaybackRequiresUserGesture)
{
    m_page->settings().setAudioPlaybackRequiresUserGesture(audioPlaybackRequiresUserGesture);
}

void InternalSettingsGenerated::setAuthorAndUserStylesEnabled(bool authorAndUserStylesEnabled)
{
    m_page->settings().setAuthorAndUserStylesEnabled(authorAndUserStylesEnabled);
}

void InternalSettingsGenerated::setAutoscrollForDragAndDropEnabled(bool autoscrollForDragAndDropEnabled)
{
    m_page->settings().setAutoscrollForDragAndDropEnabled(autoscrollForDragAndDropEnabled);
}

void InternalSettingsGenerated::setAutostartOriginPlugInSnapshottingEnabled(bool autostartOriginPlugInSnapshottingEnabled)
{
    m_page->settings().setAutostartOriginPlugInSnapshottingEnabled(autostartOriginPlugInSnapshottingEnabled);
}

void InternalSettingsGenerated::setBackForwardCacheSupportsPlugins(bool backForwardCacheSupportsPlugins)
{
    m_page->settings().setBackForwardCacheSupportsPlugins(backForwardCacheSupportsPlugins);
}

void InternalSettingsGenerated::setBackgroundShouldExtendBeyondPage(bool backgroundShouldExtendBeyondPage)
{
    m_page->settings().setBackgroundShouldExtendBeyondPage(backgroundShouldExtendBeyondPage);
}

void InternalSettingsGenerated::setBackspaceKeyNavigationEnabled(bool backspaceKeyNavigationEnabled)
{
    m_page->settings().setBackspaceKeyNavigationEnabled(backspaceKeyNavigationEnabled);
}

void InternalSettingsGenerated::setBeaconAPIEnabled(bool beaconAPIEnabled)
{
    m_page->settings().setBeaconAPIEnabled(beaconAPIEnabled);
}

void InternalSettingsGenerated::setBlockingOfSmallPluginsEnabled(bool blockingOfSmallPluginsEnabled)
{
    m_page->settings().setBlockingOfSmallPluginsEnabled(blockingOfSmallPluginsEnabled);
}

void InternalSettingsGenerated::setCanvasUsesAcceleratedDrawing(bool canvasUsesAcceleratedDrawing)
{
    m_page->settings().setCanvasUsesAcceleratedDrawing(canvasUsesAcceleratedDrawing);
}

void InternalSettingsGenerated::setCaretBrowsingEnabled(bool caretBrowsingEnabled)
{
    m_page->settings().setCaretBrowsingEnabled(caretBrowsingEnabled);
}

void InternalSettingsGenerated::setClientCoordinatesRelativeToLayoutViewport(bool clientCoordinatesRelativeToLayoutViewport)
{
    m_page->settings().setClientCoordinatesRelativeToLayoutViewport(clientCoordinatesRelativeToLayoutViewport);
}

void InternalSettingsGenerated::setColorFilterEnabled(bool colorFilterEnabled)
{
    m_page->settings().setColorFilterEnabled(colorFilterEnabled);
}

void InternalSettingsGenerated::setConstantPropertiesEnabled(bool constantPropertiesEnabled)
{
    m_page->settings().setConstantPropertiesEnabled(constantPropertiesEnabled);
}

void InternalSettingsGenerated::setContentChangeObserverEnabled(bool contentChangeObserverEnabled)
{
    m_page->settings().setContentChangeObserverEnabled(contentChangeObserverEnabled);
}

void InternalSettingsGenerated::setContentDispositionAttachmentSandboxEnabled(bool contentDispositionAttachmentSandboxEnabled)
{
    m_page->settings().setContentDispositionAttachmentSandboxEnabled(contentDispositionAttachmentSandboxEnabled);
}

void InternalSettingsGenerated::setCookieEnabled(bool cookieEnabled)
{
    m_page->settings().setCookieEnabled(cookieEnabled);
}

void InternalSettingsGenerated::setCoreMathMLEnabled(bool coreMathMLEnabled)
{
    m_page->settings().setCoreMathMLEnabled(coreMathMLEnabled);
}

void InternalSettingsGenerated::setCrossOriginCheckInGetMatchedCSSRulesDisabled(bool crossOriginCheckInGetMatchedCSSRulesDisabled)
{
    m_page->settings().setCrossOriginCheckInGetMatchedCSSRulesDisabled(crossOriginCheckInGetMatchedCSSRulesDisabled);
}

void InternalSettingsGenerated::setDefaultFixedFontSize(int defaultFixedFontSize)
{
    m_page->settings().setDefaultFixedFontSize(defaultFixedFontSize);
}

void InternalSettingsGenerated::setDefaultFontSize(int defaultFontSize)
{
    m_page->settings().setDefaultFontSize(defaultFontSize);
}

void InternalSettingsGenerated::setDefaultTextEncodingName(const String& defaultTextEncodingName)
{
    m_page->settings().setDefaultTextEncodingName(defaultTextEncodingName);
}

void InternalSettingsGenerated::setDefaultVideoPosterURL(const String& defaultVideoPosterURL)
{
    m_page->settings().setDefaultVideoPosterURL(defaultVideoPosterURL);
}

void InternalSettingsGenerated::setDeferredCSSParserEnabled(bool deferredCSSParserEnabled)
{
    m_page->settings().setDeferredCSSParserEnabled(deferredCSSParserEnabled);
}

void InternalSettingsGenerated::setDeveloperExtrasEnabled(bool developerExtrasEnabled)
{
    m_page->settings().setDeveloperExtrasEnabled(developerExtrasEnabled);
}

void InternalSettingsGenerated::setDeviceHeight(int deviceHeight)
{
    m_page->settings().setDeviceHeight(deviceHeight);
}

void InternalSettingsGenerated::setDeviceOrientationEventEnabled(bool deviceOrientationEventEnabled)
{
#if ENABLE(DEVICE_ORIENTATION)
    m_page->settings().setDeviceOrientationEventEnabled(deviceOrientationEventEnabled);
#else
    UNUSED_PARAM(deviceOrientationEventEnabled);
#endif
}

void InternalSettingsGenerated::setDeviceOrientationPermissionAPIEnabled(bool deviceOrientationPermissionAPIEnabled)
{
#if ENABLE(DEVICE_ORIENTATION)
    m_page->settings().setDeviceOrientationPermissionAPIEnabled(deviceOrientationPermissionAPIEnabled);
#else
    UNUSED_PARAM(deviceOrientationPermissionAPIEnabled);
#endif
}

void InternalSettingsGenerated::setDeviceWidth(int deviceWidth)
{
    m_page->settings().setDeviceWidth(deviceWidth);
}

void InternalSettingsGenerated::setDiagnosticLoggingEnabled(bool diagnosticLoggingEnabled)
{
    m_page->settings().setDiagnosticLoggingEnabled(diagnosticLoggingEnabled);
}

void InternalSettingsGenerated::setDisableScreenSizeOverride(bool disableScreenSizeOverride)
{
    m_page->settings().setDisableScreenSizeOverride(disableScreenSizeOverride);
}

void InternalSettingsGenerated::setDisallowSyncXHRDuringPageDismissalEnabled(bool disallowSyncXHRDuringPageDismissalEnabled)
{
    m_page->settings().setDisallowSyncXHRDuringPageDismissalEnabled(disallowSyncXHRDuringPageDismissalEnabled);
}

void InternalSettingsGenerated::setDisplayListDrawingEnabled(bool displayListDrawingEnabled)
{
    m_page->settings().setDisplayListDrawingEnabled(displayListDrawingEnabled);
}

void InternalSettingsGenerated::setDNSPrefetchingEnabled(bool dnsPrefetchingEnabled)
{
    m_page->settings().setDNSPrefetchingEnabled(dnsPrefetchingEnabled);
}

void InternalSettingsGenerated::setDOMPasteAccessRequestsEnabled(bool domPasteAccessRequestsEnabled)
{
    m_page->settings().setDOMPasteAccessRequestsEnabled(domPasteAccessRequestsEnabled);
}

void InternalSettingsGenerated::setDOMTimersThrottlingEnabled(bool domTimersThrottlingEnabled)
{
    m_page->settings().setDOMTimersThrottlingEnabled(domTimersThrottlingEnabled);
}

void InternalSettingsGenerated::setDownloadableBinaryFontsEnabled(bool downloadableBinaryFontsEnabled)
{
    m_page->settings().setDownloadableBinaryFontsEnabled(downloadableBinaryFontsEnabled);
}

void InternalSettingsGenerated::setEditableImagesEnabled(bool editableImagesEnabled)
{
    m_page->settings().setEditableImagesEnabled(editableImagesEnabled);
}

void InternalSettingsGenerated::setEnableInheritURIQueryComponent(bool enableInheritURIQueryComponent)
{
    m_page->settings().setEnableInheritURIQueryComponent(enableInheritURIQueryComponent);
}

void InternalSettingsGenerated::setEnforceCSSMIMETypeInNoQuirksMode(bool enforceCSSMIMETypeInNoQuirksMode)
{
    m_page->settings().setEnforceCSSMIMETypeInNoQuirksMode(enforceCSSMIMETypeInNoQuirksMode);
}

void InternalSettingsGenerated::setEnterKeyHintEnabled(bool enterKeyHintEnabled)
{
    m_page->settings().setEnterKeyHintEnabled(enterKeyHintEnabled);
}

void InternalSettingsGenerated::setExperimentalNotificationsEnabled(bool experimentalNotificationsEnabled)
{
    m_page->settings().setExperimentalNotificationsEnabled(experimentalNotificationsEnabled);
}

void InternalSettingsGenerated::setFixedBackgroundsPaintRelativeToDocument(bool fixedBackgroundsPaintRelativeToDocument)
{
    m_page->settings().setFixedBackgroundsPaintRelativeToDocument(fixedBackgroundsPaintRelativeToDocument);
}

void InternalSettingsGenerated::setFixedElementsLayoutRelativeToFrame(bool fixedElementsLayoutRelativeToFrame)
{
    m_page->settings().setFixedElementsLayoutRelativeToFrame(fixedElementsLayoutRelativeToFrame);
}

void InternalSettingsGenerated::setFontFallbackPrefersPictographs(bool fontFallbackPrefersPictographs)
{
    m_page->settings().setFontFallbackPrefersPictographs(fontFallbackPrefersPictographs);
}

void InternalSettingsGenerated::setForceCompositingMode(bool forceCompositingMode)
{
    m_page->settings().setForceCompositingMode(forceCompositingMode);
}

void InternalSettingsGenerated::setForceFTPDirectoryListings(bool forceFTPDirectoryListings)
{
    m_page->settings().setForceFTPDirectoryListings(forceFTPDirectoryListings);
}

void InternalSettingsGenerated::setForcePendingWebGLPolicy(bool forcePendingWebGLPolicy)
{
    m_page->settings().setForcePendingWebGLPolicy(forcePendingWebGLPolicy);
}

void InternalSettingsGenerated::setForceUpdateScrollbarsOnMainThreadForPerformanceTesting(bool forceUpdateScrollbarsOnMainThreadForPerformanceTesting)
{
    m_page->settings().setForceUpdateScrollbarsOnMainThreadForPerformanceTesting(forceUpdateScrollbarsOnMainThreadForPerformanceTesting);
}

void InternalSettingsGenerated::setForceWebGLUsesLowPower(bool forceWebGLUsesLowPower)
{
    m_page->settings().setForceWebGLUsesLowPower(forceWebGLUsesLowPower);
}

void InternalSettingsGenerated::setFTPDirectoryTemplatePath(const String& ftpDirectoryTemplatePath)
{
    m_page->settings().setFTPDirectoryTemplatePath(ftpDirectoryTemplatePath);
}

void InternalSettingsGenerated::setFullScreenEnabled(bool fullScreenEnabled)
{
#if ENABLE(FULLSCREEN_API)
    m_page->settings().setFullScreenEnabled(fullScreenEnabled);
#else
    UNUSED_PARAM(fullScreenEnabled);
#endif
}

void InternalSettingsGenerated::setGenericCueAPIEnabled(bool genericCueAPIEnabled)
{
#if ENABLE(VIDEO_TRACK)
    m_page->settings().setGenericCueAPIEnabled(genericCueAPIEnabled);
#else
    UNUSED_PARAM(genericCueAPIEnabled);
#endif
}

void InternalSettingsGenerated::setGeolocationFloorLevelEnabled(bool geolocationFloorLevelEnabled)
{
    m_page->settings().setGeolocationFloorLevelEnabled(geolocationFloorLevelEnabled);
}

void InternalSettingsGenerated::setGoogleAntiFlickerOptimizationQuirkEnabled(bool googleAntiFlickerOptimizationQuirkEnabled)
{
    m_page->settings().setGoogleAntiFlickerOptimizationQuirkEnabled(googleAntiFlickerOptimizationQuirkEnabled);
}

void InternalSettingsGenerated::setHDRMediaCapabilitiesEnabled(bool hdrMediaCapabilitiesEnabled)
{
    m_page->settings().setHDRMediaCapabilitiesEnabled(hdrMediaCapabilitiesEnabled);
}

void InternalSettingsGenerated::setHiddenPageCSSAnimationSuspensionEnabled(bool hiddenPageCSSAnimationSuspensionEnabled)
{
    m_page->settings().setHiddenPageCSSAnimationSuspensionEnabled(hiddenPageCSSAnimationSuspensionEnabled);
}

void InternalSettingsGenerated::setHiddenPageDOMTimerThrottlingAutoIncreases(bool hiddenPageDOMTimerThrottlingAutoIncreases)
{
    m_page->settings().setHiddenPageDOMTimerThrottlingAutoIncreases(hiddenPageDOMTimerThrottlingAutoIncreases);
}

void InternalSettingsGenerated::setHiddenPageDOMTimerThrottlingEnabled(bool hiddenPageDOMTimerThrottlingEnabled)
{
    m_page->settings().setHiddenPageDOMTimerThrottlingEnabled(hiddenPageDOMTimerThrottlingEnabled);
}

void InternalSettingsGenerated::setHttpEquivEnabled(bool httpEquivEnabled)
{
    m_page->settings().setHttpEquivEnabled(httpEquivEnabled);
}

void InternalSettingsGenerated::setHyperlinkAuditingEnabled(bool hyperlinkAuditingEnabled)
{
    m_page->settings().setHyperlinkAuditingEnabled(hyperlinkAuditingEnabled);
}

void InternalSettingsGenerated::setICECandidateFilteringEnabled(bool iceCandidateFilteringEnabled)
{
    m_page->settings().setICECandidateFilteringEnabled(iceCandidateFilteringEnabled);
}

void InternalSettingsGenerated::setIdempotentModeAutosizingOnlyHonorsPercentages(bool idempotentModeAutosizingOnlyHonorsPercentages)
{
#if ENABLE(TEXT_AUTOSIZING)
    m_page->settings().setIdempotentModeAutosizingOnlyHonorsPercentages(idempotentModeAutosizingOnlyHonorsPercentages);
#else
    UNUSED_PARAM(idempotentModeAutosizingOnlyHonorsPercentages);
#endif
}

void InternalSettingsGenerated::setImageControlsEnabled(bool imageControlsEnabled)
{
#if ENABLE(SERVICE_CONTROLS)
    m_page->settings().setImageControlsEnabled(imageControlsEnabled);
#else
    UNUSED_PARAM(imageControlsEnabled);
#endif
}

void InternalSettingsGenerated::setImageSubsamplingEnabled(bool imageSubsamplingEnabled)
{
    m_page->settings().setImageSubsamplingEnabled(imageSubsamplingEnabled);
}

void InternalSettingsGenerated::setImagesEnabled(bool imagesEnabled)
{
    m_page->settings().setImagesEnabled(imagesEnabled);
}

void InternalSettingsGenerated::setInProcessCookieCacheEnabled(bool inProcessCookieCacheEnabled)
{
    m_page->settings().setInProcessCookieCacheEnabled(inProcessCookieCacheEnabled);
}

void InternalSettingsGenerated::setIncompleteImageBorderEnabled(bool incompleteImageBorderEnabled)
{
    m_page->settings().setIncompleteImageBorderEnabled(incompleteImageBorderEnabled);
}

void InternalSettingsGenerated::setIncrementalRenderingSuppressionTimeoutInSeconds(double incrementalRenderingSuppressionTimeoutInSeconds)
{
    m_page->settings().setIncrementalRenderingSuppressionTimeoutInSeconds(incrementalRenderingSuppressionTimeoutInSeconds);
}

void InternalSettingsGenerated::setInlineMediaPlaybackRequiresPlaysInlineAttribute(bool inlineMediaPlaybackRequiresPlaysInlineAttribute)
{
    m_page->settings().setInlineMediaPlaybackRequiresPlaysInlineAttribute(inlineMediaPlaybackRequiresPlaysInlineAttribute);
}

void InternalSettingsGenerated::setInputEventsEnabled(bool inputEventsEnabled)
{
    m_page->settings().setInputEventsEnabled(inputEventsEnabled);
}

void InternalSettingsGenerated::setInteractiveFormValidationEnabled(bool interactiveFormValidationEnabled)
{
    m_page->settings().setInteractiveFormValidationEnabled(interactiveFormValidationEnabled);
}

void InternalSettingsGenerated::setInvisibleAutoplayNotPermitted(bool invisibleAutoplayNotPermitted)
{
    m_page->settings().setInvisibleAutoplayNotPermitted(invisibleAutoplayNotPermitted);
}

void InternalSettingsGenerated::setIsAccessibilityIsolatedTreeEnabled(bool isAccessibilityIsolatedTreeEnabled)
{
    m_page->settings().setIsAccessibilityIsolatedTreeEnabled(isAccessibilityIsolatedTreeEnabled);
}

void InternalSettingsGenerated::setIsFirstPartyWebsiteDataRemovalDisabled(bool isFirstPartyWebsiteDataRemovalDisabled)
{
    m_page->settings().setIsFirstPartyWebsiteDataRemovalDisabled(isFirstPartyWebsiteDataRemovalDisabled);
}

void InternalSettingsGenerated::setIsFirstPartyWebsiteDataRemovalLiveOnTestingEnabled(bool isFirstPartyWebsiteDataRemovalLiveOnTestingEnabled)
{
    m_page->settings().setIsFirstPartyWebsiteDataRemovalLiveOnTestingEnabled(isFirstPartyWebsiteDataRemovalLiveOnTestingEnabled);
}

void InternalSettingsGenerated::setIsFirstPartyWebsiteDataRemovalReproTestingEnabled(bool isFirstPartyWebsiteDataRemovalReproTestingEnabled)
{
    m_page->settings().setIsFirstPartyWebsiteDataRemovalReproTestingEnabled(isFirstPartyWebsiteDataRemovalReproTestingEnabled);
}

void InternalSettingsGenerated::setIsInAppBrowserPrivacyEnabled(bool isInAppBrowserPrivacyEnabled)
{
    m_page->settings().setIsInAppBrowserPrivacyEnabled(isInAppBrowserPrivacyEnabled);
}

void InternalSettingsGenerated::setIsLoggedInAPIEnabled(bool isLoggedInAPIEnabled)
{
    m_page->settings().setIsLoggedInAPIEnabled(isLoggedInAPIEnabled);
}

void InternalSettingsGenerated::setIsSameSiteStrictEnforcementEnabled(bool isSameSiteStrictEnforcementEnabled)
{
    m_page->settings().setIsSameSiteStrictEnforcementEnabled(isSameSiteStrictEnforcementEnabled);
}

void InternalSettingsGenerated::setIsThirdPartyCookieBlockingDisabled(bool isThirdPartyCookieBlockingDisabled)
{
    m_page->settings().setIsThirdPartyCookieBlockingDisabled(isThirdPartyCookieBlockingDisabled);
}

void InternalSettingsGenerated::setJavaEnabled(bool javaEnabled)
{
    m_page->settings().setJavaEnabled(javaEnabled);
}

void InternalSettingsGenerated::setJavaEnabledForLocalFiles(bool javaEnabledForLocalFiles)
{
    m_page->settings().setJavaEnabledForLocalFiles(javaEnabledForLocalFiles);
}

void InternalSettingsGenerated::setJavaScriptCanAccessClipboard(bool javaScriptCanAccessClipboard)
{
    m_page->settings().setJavaScriptCanAccessClipboard(javaScriptCanAccessClipboard);
}

void InternalSettingsGenerated::setJavaScriptCanOpenWindowsAutomatically(bool javaScriptCanOpenWindowsAutomatically)
{
    m_page->settings().setJavaScriptCanOpenWindowsAutomatically(javaScriptCanOpenWindowsAutomatically);
}

void InternalSettingsGenerated::setLangAttributeAwareFormControlUIEnabled(bool langAttributeAwareFormControlUIEnabled)
{
    m_page->settings().setLangAttributeAwareFormControlUIEnabled(langAttributeAwareFormControlUIEnabled);
}

void InternalSettingsGenerated::setLargeImageAsyncDecodingEnabled(bool largeImageAsyncDecodingEnabled)
{
    m_page->settings().setLargeImageAsyncDecodingEnabled(largeImageAsyncDecodingEnabled);
}

void InternalSettingsGenerated::setLayoutFallbackWidth(int layoutFallbackWidth)
{
    m_page->settings().setLayoutFallbackWidth(layoutFallbackWidth);
}

void InternalSettingsGenerated::setLayoutViewportHeightExpansionFactor(double layoutViewportHeightExpansionFactor)
{
    m_page->settings().setLayoutViewportHeightExpansionFactor(layoutViewportHeightExpansionFactor);
}

void InternalSettingsGenerated::setLegacyGetUserMediaEnabled(bool legacyGetUserMediaEnabled)
{
#if ENABLE(MEDIA_STREAM)
    m_page->settings().setLegacyGetUserMediaEnabled(legacyGetUserMediaEnabled);
#else
    UNUSED_PARAM(legacyGetUserMediaEnabled);
#endif
}

void InternalSettingsGenerated::setLegacyOverflowScrollingTouchEnabled(bool legacyOverflowScrollingTouchEnabled)
{
#if ENABLE(OVERFLOW_SCROLLING_TOUCH)
    m_page->settings().setLegacyOverflowScrollingTouchEnabled(legacyOverflowScrollingTouchEnabled);
#else
    UNUSED_PARAM(legacyOverflowScrollingTouchEnabled);
#endif
}

void InternalSettingsGenerated::setLinkPreconnectEnabled(bool linkPreconnectEnabled)
{
    m_page->settings().setLinkPreconnectEnabled(linkPreconnectEnabled);
}

void InternalSettingsGenerated::setLoadDeferringEnabled(bool loadDeferringEnabled)
{
    m_page->settings().setLoadDeferringEnabled(loadDeferringEnabled);
}

void InternalSettingsGenerated::setLoadsImagesAutomatically(bool loadsImagesAutomatically)
{
    m_page->settings().setLoadsImagesAutomatically(loadsImagesAutomatically);
}

void InternalSettingsGenerated::setLoadsSiteIconsIgnoringImageLoadingSetting(bool loadsSiteIconsIgnoringImageLoadingSetting)
{
    m_page->settings().setLoadsSiteIconsIgnoringImageLoadingSetting(loadsSiteIconsIgnoringImageLoadingSetting);
}

void InternalSettingsGenerated::setLocalFileContentSniffingEnabled(bool localFileContentSniffingEnabled)
{
    m_page->settings().setLocalFileContentSniffingEnabled(localFileContentSniffingEnabled);
}

void InternalSettingsGenerated::setLocalStorageDatabasePath(const String& localStorageDatabasePath)
{
    m_page->settings().setLocalStorageDatabasePath(localStorageDatabasePath);
}

void InternalSettingsGenerated::setLocalStorageEnabled(bool localStorageEnabled)
{
    m_page->settings().setLocalStorageEnabled(localStorageEnabled);
}

void InternalSettingsGenerated::setLogsPageMessagesToSystemConsoleEnabled(bool logsPageMessagesToSystemConsoleEnabled)
{
    m_page->settings().setLogsPageMessagesToSystemConsoleEnabled(logsPageMessagesToSystemConsoleEnabled);
}

void InternalSettingsGenerated::setMainContentUserGestureOverrideEnabled(bool mainContentUserGestureOverrideEnabled)
{
    m_page->settings().setMainContentUserGestureOverrideEnabled(mainContentUserGestureOverrideEnabled);
}

void InternalSettingsGenerated::setMaxParseDuration(double maxParseDuration)
{
    m_page->settings().setMaxParseDuration(maxParseDuration);
}

void InternalSettingsGenerated::setMaximumAccelerated2dCanvasSize(unsigned maximumAccelerated2dCanvasSize)
{
    m_page->settings().setMaximumAccelerated2dCanvasSize(maximumAccelerated2dCanvasSize);
}

void InternalSettingsGenerated::setMaximumHTMLParserDOMTreeDepth(unsigned maximumHTMLParserDOMTreeDepth)
{
    m_page->settings().setMaximumHTMLParserDOMTreeDepth(maximumHTMLParserDOMTreeDepth);
}

void InternalSettingsGenerated::setMaximumPlugInSnapshotAttempts(unsigned maximumPlugInSnapshotAttempts)
{
    m_page->settings().setMaximumPlugInSnapshotAttempts(maximumPlugInSnapshotAttempts);
}

void InternalSettingsGenerated::setMaximumSourceBufferSize(int maximumSourceBufferSize)
{
#if ENABLE(MEDIA_SOURCE)
    m_page->settings().setMaximumSourceBufferSize(maximumSourceBufferSize);
#else
    UNUSED_PARAM(maximumSourceBufferSize);
#endif
}

void InternalSettingsGenerated::setMediaCapabilitiesEnabled(bool mediaCapabilitiesEnabled)
{
    m_page->settings().setMediaCapabilitiesEnabled(mediaCapabilitiesEnabled);
}

void InternalSettingsGenerated::setMediaCapabilitiesExtensionsEnabled(bool mediaCapabilitiesExtensionsEnabled)
{
    m_page->settings().setMediaCapabilitiesExtensionsEnabled(mediaCapabilitiesExtensionsEnabled);
}

void InternalSettingsGenerated::setMediaCaptureRequiresSecureConnection(bool mediaCaptureRequiresSecureConnection)
{
#if ENABLE(MEDIA_STREAM)
    m_page->settings().setMediaCaptureRequiresSecureConnection(mediaCaptureRequiresSecureConnection);
#else
    UNUSED_PARAM(mediaCaptureRequiresSecureConnection);
#endif
}

void InternalSettingsGenerated::setMediaControlsScaleWithPageZoom(bool mediaControlsScaleWithPageZoom)
{
    m_page->settings().setMediaControlsScaleWithPageZoom(mediaControlsScaleWithPageZoom);
}

void InternalSettingsGenerated::setMediaDataLoadsAutomatically(bool mediaDataLoadsAutomatically)
{
    m_page->settings().setMediaDataLoadsAutomatically(mediaDataLoadsAutomatically);
}

void InternalSettingsGenerated::setMediaDeviceIdentifierStorageDirectory(const String& mediaDeviceIdentifierStorageDirectory)
{
#if ENABLE(MEDIA_STREAM)
    m_page->settings().setMediaDeviceIdentifierStorageDirectory(mediaDeviceIdentifierStorageDirectory);
#else
    UNUSED_PARAM(mediaDeviceIdentifierStorageDirectory);
#endif
}

void InternalSettingsGenerated::setMediaEnabled(bool mediaEnabled)
{
    m_page->settings().setMediaEnabled(mediaEnabled);
}

void InternalSettingsGenerated::setMediaKeysStorageDirectory(const String& mediaKeysStorageDirectory)
{
    m_page->settings().setMediaKeysStorageDirectory(mediaKeysStorageDirectory);
}

void InternalSettingsGenerated::setMediaPreloadingEnabled(bool mediaPreloadingEnabled)
{
    m_page->settings().setMediaPreloadingEnabled(mediaPreloadingEnabled);
}

void InternalSettingsGenerated::setMediaSourceEnabled(bool mediaSourceEnabled)
{
#if ENABLE(MEDIA_SOURCE)
    m_page->settings().setMediaSourceEnabled(mediaSourceEnabled);
#else
    UNUSED_PARAM(mediaSourceEnabled);
#endif
}

void InternalSettingsGenerated::setMediaTypeOverride(const String& mediaTypeOverride)
{
    m_page->settings().setMediaTypeOverride(mediaTypeOverride);
}

void InternalSettingsGenerated::setMediaUserGestureInheritsFromDocument(bool mediaUserGestureInheritsFromDocument)
{
    m_page->settings().setMediaUserGestureInheritsFromDocument(mediaUserGestureInheritsFromDocument);
}

void InternalSettingsGenerated::setMinimumAccelerated2dCanvasSize(int minimumAccelerated2dCanvasSize)
{
    m_page->settings().setMinimumAccelerated2dCanvasSize(minimumAccelerated2dCanvasSize);
}

void InternalSettingsGenerated::setMinimumFontSize(int minimumFontSize)
{
    m_page->settings().setMinimumFontSize(minimumFontSize);
}

void InternalSettingsGenerated::setMinimumLogicalFontSize(int minimumLogicalFontSize)
{
    m_page->settings().setMinimumLogicalFontSize(minimumLogicalFontSize);
}

void InternalSettingsGenerated::setMinimumZoomFontSize(float minimumZoomFontSize)
{
#if ENABLE(TEXT_AUTOSIZING)
    m_page->settings().setMinimumZoomFontSize(minimumZoomFontSize);
#else
    UNUSED_PARAM(minimumZoomFontSize);
#endif
}

void InternalSettingsGenerated::setMockCaptureDevicesEnabled(bool mockCaptureDevicesEnabled)
{
#if ENABLE(MEDIA_STREAM)
    m_page->settings().setMockCaptureDevicesEnabled(mockCaptureDevicesEnabled);
#else
    UNUSED_PARAM(mockCaptureDevicesEnabled);
#endif
}

void InternalSettingsGenerated::setNeedsAdobeFrameReloadingQuirk(bool needsAdobeFrameReloadingQuirk)
{
    m_page->settings().setNeedsAdobeFrameReloadingQuirk(needsAdobeFrameReloadingQuirk);
}

void InternalSettingsGenerated::setNeedsDeferKeyDownAndKeyPressTimersUntilNextEditingCommandQuirk(bool needsDeferKeyDownAndKeyPressTimersUntilNextEditingCommandQuirk)
{
    m_page->settings().setNeedsDeferKeyDownAndKeyPressTimersUntilNextEditingCommandQuirk(needsDeferKeyDownAndKeyPressTimersUntilNextEditingCommandQuirk);
}

void InternalSettingsGenerated::setNeedsFrameNameFallbackToIdQuirk(bool needsFrameNameFallbackToIdQuirk)
{
    m_page->settings().setNeedsFrameNameFallbackToIdQuirk(needsFrameNameFallbackToIdQuirk);
}

void InternalSettingsGenerated::setNeedsIsLoadingInAPISenseQuirk(bool needsIsLoadingInAPISenseQuirk)
{
    m_page->settings().setNeedsIsLoadingInAPISenseQuirk(needsIsLoadingInAPISenseQuirk);
}

void InternalSettingsGenerated::setNeedsKeyboardEventDisambiguationQuirks(bool needsKeyboardEventDisambiguationQuirks)
{
    m_page->settings().setNeedsKeyboardEventDisambiguationQuirks(needsKeyboardEventDisambiguationQuirks);
}

void InternalSettingsGenerated::setNeedsSiteSpecificQuirks(bool needsSiteSpecificQuirks)
{
    m_page->settings().setNeedsSiteSpecificQuirks(needsSiteSpecificQuirks);
}

void InternalSettingsGenerated::setNeedsStorageAccessFromFileURLsQuirk(bool needsStorageAccessFromFileURLsQuirk)
{
    m_page->settings().setNeedsStorageAccessFromFileURLsQuirk(needsStorageAccessFromFileURLsQuirk);
}

void InternalSettingsGenerated::setNotificationsEnabled(bool notificationsEnabled)
{
    m_page->settings().setNotificationsEnabled(notificationsEnabled);
}

void InternalSettingsGenerated::setOfflineWebApplicationCacheEnabled(bool offlineWebApplicationCacheEnabled)
{
    m_page->settings().setOfflineWebApplicationCacheEnabled(offlineWebApplicationCacheEnabled);
}

void InternalSettingsGenerated::setPaginateDuringLayoutEnabled(bool paginateDuringLayoutEnabled)
{
    m_page->settings().setPaginateDuringLayoutEnabled(paginateDuringLayoutEnabled);
}

void InternalSettingsGenerated::setPassiveTouchListenersAsDefaultOnDocument(bool passiveTouchListenersAsDefaultOnDocument)
{
    m_page->settings().setPassiveTouchListenersAsDefaultOnDocument(passiveTouchListenersAsDefaultOnDocument);
}

void InternalSettingsGenerated::setPasswordEchoDurationInSeconds(double passwordEchoDurationInSeconds)
{
    m_page->settings().setPasswordEchoDurationInSeconds(passwordEchoDurationInSeconds);
}

void InternalSettingsGenerated::setPasswordEchoEnabled(bool passwordEchoEnabled)
{
    m_page->settings().setPasswordEchoEnabled(passwordEchoEnabled);
}

void InternalSettingsGenerated::setPaymentRequestEnabled(bool paymentRequestEnabled)
{
#if ENABLE(PAYMENT_REQUEST)
    m_page->settings().setPaymentRequestEnabled(paymentRequestEnabled);
#else
    UNUSED_PARAM(paymentRequestEnabled);
#endif
}

void InternalSettingsGenerated::setPictureInPictureAPIEnabled(bool pictureInPictureAPIEnabled)
{
    m_page->settings().setPictureInPictureAPIEnabled(pictureInPictureAPIEnabled);
}

void InternalSettingsGenerated::setPlugInSnapshottingEnabled(bool plugInSnapshottingEnabled)
{
    m_page->settings().setPlugInSnapshottingEnabled(plugInSnapshottingEnabled);
}

void InternalSettingsGenerated::setPluginsEnabled(bool pluginsEnabled)
{
    m_page->settings().setPluginsEnabled(pluginsEnabled);
}

void InternalSettingsGenerated::setPreferMIMETypeForImages(bool preferMIMETypeForImages)
{
    m_page->settings().setPreferMIMETypeForImages(preferMIMETypeForImages);
}

void InternalSettingsGenerated::setPreventKeyboardDOMEventDispatch(bool preventKeyboardDOMEventDispatch)
{
    m_page->settings().setPreventKeyboardDOMEventDispatch(preventKeyboardDOMEventDispatch);
}

void InternalSettingsGenerated::setPrimaryPlugInSnapshotDetectionEnabled(bool primaryPlugInSnapshotDetectionEnabled)
{
    m_page->settings().setPrimaryPlugInSnapshotDetectionEnabled(primaryPlugInSnapshotDetectionEnabled);
}

void InternalSettingsGenerated::setPunchOutWhiteBackgroundsInDarkMode(bool punchOutWhiteBackgroundsInDarkMode)
{
    m_page->settings().setPunchOutWhiteBackgroundsInDarkMode(punchOutWhiteBackgroundsInDarkMode);
}

void InternalSettingsGenerated::setQuickTimePluginReplacementEnabled(bool quickTimePluginReplacementEnabled)
{
    m_page->settings().setQuickTimePluginReplacementEnabled(quickTimePluginReplacementEnabled);
}

void InternalSettingsGenerated::setRemotePlaybackEnabled(bool remotePlaybackEnabled)
{
#if ENABLE(WIRELESS_PLAYBACK_TARGET)
    m_page->settings().setRemotePlaybackEnabled(remotePlaybackEnabled);
#else
    UNUSED_PARAM(remotePlaybackEnabled);
#endif
}

void InternalSettingsGenerated::setRepaintOutsideLayoutEnabled(bool repaintOutsideLayoutEnabled)
{
    m_page->settings().setRepaintOutsideLayoutEnabled(repaintOutsideLayoutEnabled);
}

void InternalSettingsGenerated::setRequestAnimationFrameEnabled(bool requestAnimationFrameEnabled)
{
    m_page->settings().setRequestAnimationFrameEnabled(requestAnimationFrameEnabled);
}

void InternalSettingsGenerated::setRequestIdleCallbackEnabled(bool requestIdleCallbackEnabled)
{
    m_page->settings().setRequestIdleCallbackEnabled(requestIdleCallbackEnabled);
}

void InternalSettingsGenerated::setRequiresUserGestureToLoadVideo(bool requiresUserGestureToLoadVideo)
{
    m_page->settings().setRequiresUserGestureToLoadVideo(requiresUserGestureToLoadVideo);
}

void InternalSettingsGenerated::setResizeObserverEnabled(bool resizeObserverEnabled)
{
#if ENABLE(RESIZE_OBSERVER)
    m_page->settings().setResizeObserverEnabled(resizeObserverEnabled);
#else
    UNUSED_PARAM(resizeObserverEnabled);
#endif
}

void InternalSettingsGenerated::setResourceUsageOverlayVisible(bool resourceUsageOverlayVisible)
{
    m_page->settings().setResourceUsageOverlayVisible(resourceUsageOverlayVisible);
}

void InternalSettingsGenerated::setRubberBandingForSubScrollableRegionsEnabled(bool rubberBandingForSubScrollableRegionsEnabled)
{
#if ENABLE(RUBBER_BANDING)
    m_page->settings().setRubberBandingForSubScrollableRegionsEnabled(rubberBandingForSubScrollableRegionsEnabled);
#else
    UNUSED_PARAM(rubberBandingForSubScrollableRegionsEnabled);
#endif
}

void InternalSettingsGenerated::setScriptEnabled(bool scriptEnabled)
{
    m_page->settings().setScriptEnabled(scriptEnabled);
}

void InternalSettingsGenerated::setScriptMarkupEnabled(bool scriptMarkupEnabled)
{
    m_page->settings().setScriptMarkupEnabled(scriptMarkupEnabled);
}

void InternalSettingsGenerated::setScrollAnimatorEnabled(bool scrollAnimatorEnabled)
{
#if ENABLE(SMOOTH_SCROLLING)
    m_page->settings().setScrollAnimatorEnabled(scrollAnimatorEnabled);
#else
    UNUSED_PARAM(scrollAnimatorEnabled);
#endif
}

void InternalSettingsGenerated::setScrollingCoordinatorEnabled(bool scrollingCoordinatorEnabled)
{
    m_page->settings().setScrollingCoordinatorEnabled(scrollingCoordinatorEnabled);
}

void InternalSettingsGenerated::setScrollingPerformanceLoggingEnabled(bool scrollingPerformanceLoggingEnabled)
{
    m_page->settings().setScrollingPerformanceLoggingEnabled(scrollingPerformanceLoggingEnabled);
}

void InternalSettingsGenerated::setScrollingTreeIncludesFrames(bool scrollingTreeIncludesFrames)
{
    m_page->settings().setScrollingTreeIncludesFrames(scrollingTreeIncludesFrames);
}

void InternalSettingsGenerated::setSelectTrailingWhitespaceEnabled(bool selectTrailingWhitespaceEnabled)
{
    m_page->settings().setSelectTrailingWhitespaceEnabled(selectTrailingWhitespaceEnabled);
}

void InternalSettingsGenerated::setSelectionAcrossShadowBoundariesEnabled(bool selectionAcrossShadowBoundariesEnabled)
{
    m_page->settings().setSelectionAcrossShadowBoundariesEnabled(selectionAcrossShadowBoundariesEnabled);
}

void InternalSettingsGenerated::setSelectionPaintingWithoutSelectionGapsEnabled(bool selectionPaintingWithoutSelectionGapsEnabled)
{
    m_page->settings().setSelectionPaintingWithoutSelectionGapsEnabled(selectionPaintingWithoutSelectionGapsEnabled);
}

void InternalSettingsGenerated::setServiceControlsEnabled(bool serviceControlsEnabled)
{
#if ENABLE(SERVICE_CONTROLS)
    m_page->settings().setServiceControlsEnabled(serviceControlsEnabled);
#else
    UNUSED_PARAM(serviceControlsEnabled);
#endif
}

void InternalSettingsGenerated::setSessionStorageQuota(unsigned sessionStorageQuota)
{
    m_page->settings().setSessionStorageQuota(sessionStorageQuota);
}

void InternalSettingsGenerated::setShouldAllowUserInstalledFonts(bool shouldAllowUserInstalledFonts)
{
    m_page->settings().setShouldAllowUserInstalledFonts(shouldAllowUserInstalledFonts);
}

void InternalSettingsGenerated::setShouldConvertInvalidURLsToBlank(bool shouldConvertInvalidURLsToBlank)
{
    m_page->settings().setShouldConvertInvalidURLsToBlank(shouldConvertInvalidURLsToBlank);
}

void InternalSettingsGenerated::setShouldConvertPositionStyleOnCopy(bool shouldConvertPositionStyleOnCopy)
{
    m_page->settings().setShouldConvertPositionStyleOnCopy(shouldConvertPositionStyleOnCopy);
}

void InternalSettingsGenerated::setShouldDecidePolicyBeforeLoadingQuickLookPreview(bool shouldDecidePolicyBeforeLoadingQuickLookPreview)
{
    m_page->settings().setShouldDecidePolicyBeforeLoadingQuickLookPreview(shouldDecidePolicyBeforeLoadingQuickLookPreview);
}

void InternalSettingsGenerated::setShouldDeferAsynchronousScriptsUntilAfterDocumentLoadOrFirstPaint(bool shouldDeferAsynchronousScriptsUntilAfterDocumentLoadOrFirstPaint)
{
    m_page->settings().setShouldDeferAsynchronousScriptsUntilAfterDocumentLoadOrFirstPaint(shouldDeferAsynchronousScriptsUntilAfterDocumentLoadOrFirstPaint);
}

void InternalSettingsGenerated::setShouldDispatchSyntheticMouseEventsWhenModifyingSelection(bool shouldDispatchSyntheticMouseEventsWhenModifyingSelection)
{
    m_page->settings().setShouldDispatchSyntheticMouseEventsWhenModifyingSelection(shouldDispatchSyntheticMouseEventsWhenModifyingSelection);
}

void InternalSettingsGenerated::setShouldDispatchSyntheticMouseOutAfterSyntheticClick(bool shouldDispatchSyntheticMouseOutAfterSyntheticClick)
{
    m_page->settings().setShouldDispatchSyntheticMouseOutAfterSyntheticClick(shouldDispatchSyntheticMouseOutAfterSyntheticClick);
}

void InternalSettingsGenerated::setShouldDisplayCaptions(bool shouldDisplayCaptions)
{
#if ENABLE(VIDEO_TRACK)
    m_page->settings().setShouldDisplayCaptions(shouldDisplayCaptions);
#else
    UNUSED_PARAM(shouldDisplayCaptions);
#endif
}

void InternalSettingsGenerated::setShouldDisplaySubtitles(bool shouldDisplaySubtitles)
{
#if ENABLE(VIDEO_TRACK)
    m_page->settings().setShouldDisplaySubtitles(shouldDisplaySubtitles);
#else
    UNUSED_PARAM(shouldDisplaySubtitles);
#endif
}

void InternalSettingsGenerated::setShouldDisplayTextDescriptions(bool shouldDisplayTextDescriptions)
{
#if ENABLE(VIDEO_TRACK)
    m_page->settings().setShouldDisplayTextDescriptions(shouldDisplayTextDescriptions);
#else
    UNUSED_PARAM(shouldDisplayTextDescriptions);
#endif
}

void InternalSettingsGenerated::setShouldEnableTextAutosizingBoost(bool shouldEnableTextAutosizingBoost)
{
#if ENABLE(TEXT_AUTOSIZING)
    m_page->settings().setShouldEnableTextAutosizingBoost(shouldEnableTextAutosizingBoost);
#else
    UNUSED_PARAM(shouldEnableTextAutosizingBoost);
#endif
}

void InternalSettingsGenerated::setShouldIgnoreFontLoadCompletions(bool shouldIgnoreFontLoadCompletions)
{
    m_page->settings().setShouldIgnoreFontLoadCompletions(shouldIgnoreFontLoadCompletions);
}

void InternalSettingsGenerated::setShouldIgnoreMetaViewport(bool shouldIgnoreMetaViewport)
{
    m_page->settings().setShouldIgnoreMetaViewport(shouldIgnoreMetaViewport);
}

void InternalSettingsGenerated::setShouldInjectUserScriptsInInitialEmptyDocument(bool shouldInjectUserScriptsInInitialEmptyDocument)
{
    m_page->settings().setShouldInjectUserScriptsInInitialEmptyDocument(shouldInjectUserScriptsInInitialEmptyDocument);
}

void InternalSettingsGenerated::setShouldPrintBackgrounds(bool shouldPrintBackgrounds)
{
    m_page->settings().setShouldPrintBackgrounds(shouldPrintBackgrounds);
}

void InternalSettingsGenerated::setShouldRespectImageOrientation(bool shouldRespectImageOrientation)
{
    m_page->settings().setShouldRespectImageOrientation(shouldRespectImageOrientation);
}

void InternalSettingsGenerated::setShouldRestrictBaseURLSchemes(bool shouldRestrictBaseURLSchemes)
{
    m_page->settings().setShouldRestrictBaseURLSchemes(shouldRestrictBaseURLSchemes);
}

void InternalSettingsGenerated::setShouldSuppressTextInputFromEditingDuringProvisionalNavigation(bool shouldSuppressTextInputFromEditingDuringProvisionalNavigation)
{
    m_page->settings().setShouldSuppressTextInputFromEditingDuringProvisionalNavigation(shouldSuppressTextInputFromEditingDuringProvisionalNavigation);
}

void InternalSettingsGenerated::setShouldUseServiceWorkerShortTimeout(bool shouldUseServiceWorkerShortTimeout)
{
    m_page->settings().setShouldUseServiceWorkerShortTimeout(shouldUseServiceWorkerShortTimeout);
}

void InternalSettingsGenerated::setShowDebugBorders(bool showDebugBorders)
{
    m_page->settings().setShowDebugBorders(showDebugBorders);
}

void InternalSettingsGenerated::setShowRepaintCounter(bool showRepaintCounter)
{
    m_page->settings().setShowRepaintCounter(showRepaintCounter);
}

void InternalSettingsGenerated::setShowTiledScrollingIndicator(bool showTiledScrollingIndicator)
{
    m_page->settings().setShowTiledScrollingIndicator(showTiledScrollingIndicator);
}

void InternalSettingsGenerated::setShowsToolTipOverTruncatedText(bool showsToolTipOverTruncatedText)
{
    m_page->settings().setShowsToolTipOverTruncatedText(showsToolTipOverTruncatedText);
}

void InternalSettingsGenerated::setShowsURLsInToolTips(bool showsURLsInToolTips)
{
    m_page->settings().setShowsURLsInToolTips(showsURLsInToolTips);
}

void InternalSettingsGenerated::setShrinksStandaloneImagesToFit(bool shrinksStandaloneImagesToFit)
{
    m_page->settings().setShrinksStandaloneImagesToFit(shrinksStandaloneImagesToFit);
}

void InternalSettingsGenerated::setSimpleLineLayoutDebugBordersEnabled(bool simpleLineLayoutDebugBordersEnabled)
{
    m_page->settings().setSimpleLineLayoutDebugBordersEnabled(simpleLineLayoutDebugBordersEnabled);
}

void InternalSettingsGenerated::setSimpleLineLayoutEnabled(bool simpleLineLayoutEnabled)
{
    m_page->settings().setSimpleLineLayoutEnabled(simpleLineLayoutEnabled);
}

void InternalSettingsGenerated::setSmartInsertDeleteEnabled(bool smartInsertDeleteEnabled)
{
    m_page->settings().setSmartInsertDeleteEnabled(smartInsertDeleteEnabled);
}

void InternalSettingsGenerated::setSnapshotAllPlugIns(bool snapshotAllPlugIns)
{
    m_page->settings().setSnapshotAllPlugIns(snapshotAllPlugIns);
}

void InternalSettingsGenerated::setSourceBufferChangeTypeEnabled(bool sourceBufferChangeTypeEnabled)
{
#if ENABLE(MEDIA_SOURCE)
    m_page->settings().setSourceBufferChangeTypeEnabled(sourceBufferChangeTypeEnabled);
#else
    UNUSED_PARAM(sourceBufferChangeTypeEnabled);
#endif
}

void InternalSettingsGenerated::setSpatialNavigationEnabled(bool spatialNavigationEnabled)
{
    m_page->settings().setSpatialNavigationEnabled(spatialNavigationEnabled);
}

void InternalSettingsGenerated::setSpringTimingFunctionEnabled(bool springTimingFunctionEnabled)
{
    m_page->settings().setSpringTimingFunctionEnabled(springTimingFunctionEnabled);
}

void InternalSettingsGenerated::setStandalone(bool standalone)
{
    m_page->settings().setStandalone(standalone);
}

void InternalSettingsGenerated::setStorageAccessAPIEnabled(bool storageAccessAPIEnabled)
{
    m_page->settings().setStorageAccessAPIEnabled(storageAccessAPIEnabled);
}

void InternalSettingsGenerated::setSubpixelAntialiasedLayerTextEnabled(bool subpixelAntialiasedLayerTextEnabled)
{
    m_page->settings().setSubpixelAntialiasedLayerTextEnabled(subpixelAntialiasedLayerTextEnabled);
}

void InternalSettingsGenerated::setSubpixelCSSOMElementMetricsEnabled(bool subpixelCSSOMElementMetricsEnabled)
{
    m_page->settings().setSubpixelCSSOMElementMetricsEnabled(subpixelCSSOMElementMetricsEnabled);
}

void InternalSettingsGenerated::setSubresourceIntegrityEnabled(bool subresourceIntegrityEnabled)
{
    m_page->settings().setSubresourceIntegrityEnabled(subresourceIntegrityEnabled);
}

void InternalSettingsGenerated::setSuppressesIncrementalRendering(bool suppressesIncrementalRendering)
{
    m_page->settings().setSuppressesIncrementalRendering(suppressesIncrementalRendering);
}

void InternalSettingsGenerated::setSyntheticEditingCommandsEnabled(bool syntheticEditingCommandsEnabled)
{
    m_page->settings().setSyntheticEditingCommandsEnabled(syntheticEditingCommandsEnabled);
}

void InternalSettingsGenerated::setTelephoneNumberParsingEnabled(bool telephoneNumberParsingEnabled)
{
    m_page->settings().setTelephoneNumberParsingEnabled(telephoneNumberParsingEnabled);
}

void InternalSettingsGenerated::setTemporaryTileCohortRetentionEnabled(bool temporaryTileCohortRetentionEnabled)
{
    m_page->settings().setTemporaryTileCohortRetentionEnabled(temporaryTileCohortRetentionEnabled);
}

void InternalSettingsGenerated::setTextAreasAreResizable(bool textAreasAreResizable)
{
    m_page->settings().setTextAreasAreResizable(textAreasAreResizable);
}

void InternalSettingsGenerated::setTextAutosizingEnabled(bool textAutosizingEnabled)
{
#if ENABLE(TEXT_AUTOSIZING)
    m_page->settings().setTextAutosizingEnabled(textAutosizingEnabled);
#else
    UNUSED_PARAM(textAutosizingEnabled);
#endif
}

void InternalSettingsGenerated::setTextAutosizingUsesIdempotentMode(bool textAutosizingUsesIdempotentMode)
{
#if ENABLE(TEXT_AUTOSIZING)
    m_page->settings().setTextAutosizingUsesIdempotentMode(textAutosizingUsesIdempotentMode);
#else
    UNUSED_PARAM(textAutosizingUsesIdempotentMode);
#endif
}

void InternalSettingsGenerated::setThirdPartyIframeRedirectBlockingEnabled(bool thirdPartyIframeRedirectBlockingEnabled)
{
    m_page->settings().setThirdPartyIframeRedirectBlockingEnabled(thirdPartyIframeRedirectBlockingEnabled);
}

void InternalSettingsGenerated::setTouchEventEmulationEnabled(bool touchEventEmulationEnabled)
{
#if ENABLE(TOUCH_EVENTS)
    m_page->settings().setTouchEventEmulationEnabled(touchEventEmulationEnabled);
#else
    UNUSED_PARAM(touchEventEmulationEnabled);
#endif
}

void InternalSettingsGenerated::setTreatIPAddressAsDomain(bool treatIPAddressAsDomain)
{
    m_page->settings().setTreatIPAddressAsDomain(treatIPAddressAsDomain);
}

void InternalSettingsGenerated::setTreatsAnyTextCSSLinkAsStylesheet(bool treatsAnyTextCSSLinkAsStylesheet)
{
    m_page->settings().setTreatsAnyTextCSSLinkAsStylesheet(treatsAnyTextCSSLinkAsStylesheet);
}

void InternalSettingsGenerated::setUnhandledPromiseRejectionToConsoleEnabled(bool unhandledPromiseRejectionToConsoleEnabled)
{
    m_page->settings().setUnhandledPromiseRejectionToConsoleEnabled(unhandledPromiseRejectionToConsoleEnabled);
}

void InternalSettingsGenerated::setUnifiedTextCheckerEnabled(bool unifiedTextCheckerEnabled)
{
    m_page->settings().setUnifiedTextCheckerEnabled(unifiedTextCheckerEnabled);
}

void InternalSettingsGenerated::setUseAnonymousModeWhenFetchingMaskImages(bool useAnonymousModeWhenFetchingMaskImages)
{
    m_page->settings().setUseAnonymousModeWhenFetchingMaskImages(useAnonymousModeWhenFetchingMaskImages);
}

void InternalSettingsGenerated::setUseGPUProcessForMedia(bool useGPUProcessForMedia)
{
#if ENABLE(GPU_PROCESS)
    m_page->settings().setUseGPUProcessForMedia(useGPUProcessForMedia);
#else
    UNUSED_PARAM(useGPUProcessForMedia);
#endif
}

void InternalSettingsGenerated::setUseGiantTiles(bool useGiantTiles)
{
    m_page->settings().setUseGiantTiles(useGiantTiles);
}

void InternalSettingsGenerated::setUseImageDocumentForSubframePDF(bool useImageDocumentForSubframePDF)
{
    m_page->settings().setUseImageDocumentForSubframePDF(useImageDocumentForSubframePDF);
}

void InternalSettingsGenerated::setUseLegacyBackgroundSizeShorthandBehavior(bool useLegacyBackgroundSizeShorthandBehavior)
{
    m_page->settings().setUseLegacyBackgroundSizeShorthandBehavior(useLegacyBackgroundSizeShorthandBehavior);
}

void InternalSettingsGenerated::setUseLegacyTextAlignPositionedElementBehavior(bool useLegacyTextAlignPositionedElementBehavior)
{
    m_page->settings().setUseLegacyTextAlignPositionedElementBehavior(useLegacyTextAlignPositionedElementBehavior);
}

void InternalSettingsGenerated::setUsePreHTML5ParserQuirks(bool usePreHTML5ParserQuirks)
{
    m_page->settings().setUsePreHTML5ParserQuirks(usePreHTML5ParserQuirks);
}

void InternalSettingsGenerated::setUsesEncodingDetector(bool usesEncodingDetector)
{
    m_page->settings().setUsesEncodingDetector(usesEncodingDetector);
}

void InternalSettingsGenerated::setValidationMessageTimerMagnification(int validationMessageTimerMagnification)
{
    m_page->settings().setValidationMessageTimerMagnification(validationMessageTimerMagnification);
}

void InternalSettingsGenerated::setVideoPlaybackRequiresUserGesture(bool videoPlaybackRequiresUserGesture)
{
    m_page->settings().setVideoPlaybackRequiresUserGesture(videoPlaybackRequiresUserGesture);
}

void InternalSettingsGenerated::setVideoQualityIncludesDisplayCompositingEnabled(bool videoQualityIncludesDisplayCompositingEnabled)
{
#if ENABLE(VIDEO)
    m_page->settings().setVideoQualityIncludesDisplayCompositingEnabled(videoQualityIncludesDisplayCompositingEnabled);
#else
    UNUSED_PARAM(videoQualityIncludesDisplayCompositingEnabled);
#endif
}

void InternalSettingsGenerated::setViewportFitEnabled(bool viewportFitEnabled)
{
    m_page->settings().setViewportFitEnabled(viewportFitEnabled);
}

void InternalSettingsGenerated::setVisualViewportAPIEnabled(bool visualViewportAPIEnabled)
{
    m_page->settings().setVisualViewportAPIEnabled(visualViewportAPIEnabled);
}

void InternalSettingsGenerated::setVisualViewportEnabled(bool visualViewportEnabled)
{
    m_page->settings().setVisualViewportEnabled(visualViewportEnabled);
}

void InternalSettingsGenerated::setWantsBalancedSetDefersLoadingBehavior(bool wantsBalancedSetDefersLoadingBehavior)
{
    m_page->settings().setWantsBalancedSetDefersLoadingBehavior(wantsBalancedSetDefersLoadingBehavior);
}

void InternalSettingsGenerated::setWebArchiveDebugModeEnabled(bool webArchiveDebugModeEnabled)
{
#if ENABLE(WEB_ARCHIVE)
    m_page->settings().setWebArchiveDebugModeEnabled(webArchiveDebugModeEnabled);
#else
    UNUSED_PARAM(webArchiveDebugModeEnabled);
#endif
}

void InternalSettingsGenerated::setWebAudioEnabled(bool webAudioEnabled)
{
    m_page->settings().setWebAudioEnabled(webAudioEnabled);
}

void InternalSettingsGenerated::setWebGLEnabled(bool webGLEnabled)
{
    m_page->settings().setWebGLEnabled(webGLEnabled);
}

void InternalSettingsGenerated::setWebGLErrorsToConsoleEnabled(bool webGLErrorsToConsoleEnabled)
{
    m_page->settings().setWebGLErrorsToConsoleEnabled(webGLErrorsToConsoleEnabled);
}

void InternalSettingsGenerated::setWebRTCEncryptionEnabled(bool webRTCEncryptionEnabled)
{
    m_page->settings().setWebRTCEncryptionEnabled(webRTCEncryptionEnabled);
}

void InternalSettingsGenerated::setWebSecurityEnabled(bool webSecurityEnabled)
{
    m_page->settings().setWebSecurityEnabled(webSecurityEnabled);
}

void InternalSettingsGenerated::setWebkitImageReadyEventEnabled(bool webkitImageReadyEventEnabled)
{
    m_page->settings().setWebkitImageReadyEventEnabled(webkitImageReadyEventEnabled);
}

void InternalSettingsGenerated::setWindowFocusRestricted(bool windowFocusRestricted)
{
    m_page->settings().setWindowFocusRestricted(windowFocusRestricted);
}

void InternalSettingsGenerated::setXSSAuditorEnabled(bool xssAuditorEnabled)
{
    m_page->settings().setXSSAuditorEnabled(xssAuditorEnabled);
}

void InternalSettingsGenerated::setYouTubeFlashPluginReplacementEnabled(bool youTubeFlashPluginReplacementEnabled)
{
    m_page->settings().setYouTubeFlashPluginReplacementEnabled(youTubeFlashPluginReplacementEnabled);
}

} // namespace WebCore
