#pragma once

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-load-error.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-plugin.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/property-notification.h>
#include <functional>
#include <memory>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/public-api/types/callback.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/web/web-view.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class WebViewImpl;
using WebViewImplPtr = IntrusivePtr<WebViewImpl>;

/**
 * @brief This is the internal implementation class for WebView.
 *
 * WebViewImpl wraps the dali-adaptor WebEngine and adapts its interface to
 * the dali-ui conventions: Dali::String instead of std::string, and
 * Dali::Ui::Callback<> instead of std::function.
 *
 * @see Dali::Ui::WebView
 */
class DALI_UI_API WebViewImpl : public ViewImpl
{
public: // Properties (required by DALI_PROPERTY_REGISTRATION)
  /// @brief The start and end property ranges for this impl.
  enum
  {
    PROPERTY_START_INDEX = Dali::PROPERTY_REGISTRATION_START_INDEX,
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the WebViewImpl class.
   */
  struct Property
  {
    enum
    {
      URL = PROPERTY_START_INDEX, ///< The URL of the currently loaded page
      USER_AGENT,                 ///< The user agent string
      SCROLL_POSITION,            ///< The current scroll position
      SCROLL_SIZE,                ///< The total scrollable size
      CONTENT_SIZE,               ///< The total size of the page content
      TITLE,                      ///< The title of the current web page
      VIDEO_HOLE_ENABLED,         ///< Whether video hole rendering is enabled
      MOUSE_EVENTS_ENABLED,       ///< Whether mouse events are forwarded
      KEY_EVENTS_ENABLED,         ///< Whether key events are forwarded
      DOCUMENT_BACKGROUND_COLOR,  ///< The background color of the document
      TILES_CLEARED_WHEN_HIDDEN,  ///< Whether tiles are cleared when hidden
      TILE_COVER_AREA_MULTIPLIER, ///< The tile cover area multiplier
      CURSOR_ENABLED_BY_CLIENT,   ///< Whether cursor is managed by client
      SELECTED_TEXT,              ///< The currently selected text
      PAGE_ZOOM_FACTOR,           ///< The page zoom factor
      TEXT_ZOOM_FACTOR,           ///< The text zoom factor
      LOAD_PROGRESS_PERCENTAGE,   ///< The load progress percentage
    };
  };

  /// @cond internal
  static void                  SetProperty(Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value);
  static Dali::Property::Value GetProperty(Dali::BaseObject* object, Dali::Property::Index index);
  /// @endcond

protected: // Construction & Destruction
  /**
   * @brief WebViewImpl constructor.
   */
  WebViewImpl();

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~WebViewImpl();

public: // Creation
  /**
   * @brief Creates a new WebViewImpl with no engine arguments.
   *
   * @return An intrusive pointer to the newly allocated WebViewImpl
   */
  static WebViewImplPtr New();

public: // API — URL & User Agent
  /**
   * @copydoc Dali::Ui::WebView::GetUrl
   */
  Dali::String GetUrl() const;

  /**
   * @copydoc Dali::Ui::WebView::SetUserAgent
   */
  void SetUserAgent(const Dali::String& userAgent);

  /**
   * @copydoc Dali::Ui::WebView::GetUserAgent
   */
  Dali::String GetUserAgent() const;

public: // API — Page Loading
  /**
   * @copydoc Dali::Ui::WebView::LoadUrl
   */
  void LoadUrl(const Dali::String& url);

  /**
   * @copydoc Dali::Ui::WebView::LoadHtmlString
   */
  void LoadHtmlString(const Dali::String& htmlString);

  /**
   * @copydoc Dali::Ui::WebView::LoadHtmlStringOverrideCurrentEntry
   */
  bool LoadHtmlStringOverrideCurrentEntry(const Dali::String& html, const Dali::String& basicUri, const Dali::String& unreachableUrl);

  /**
   * @copydoc Dali::Ui::WebView::LoadContents
   */
  bool LoadContents(const int8_t* contents, uint32_t contentSize, const Dali::String& mimeType, const Dali::String& encoding, const Dali::String& baseUri);

  /**
   * @copydoc Dali::Ui::WebView::Reload
   */
  void Reload();

  /**
   * @copydoc Dali::Ui::WebView::ReloadWithoutCache
   */
  bool ReloadWithoutCache();

  /**
   * @copydoc Dali::Ui::WebView::StopLoading
   */
  void StopLoading();

  /**
   * @copydoc Dali::Ui::WebView::Suspend
   */
  void Suspend();

  /**
   * @copydoc Dali::Ui::WebView::Resume
   */
  void Resume();

  /**
   * @copydoc Dali::Ui::WebView::SuspendNetworkLoading
   */
  void SuspendNetworkLoading();

  /**
   * @copydoc Dali::Ui::WebView::ResumeNetworkLoading
   */
  void ResumeNetworkLoading();

  /**
   * @copydoc Dali::Ui::WebView::GetLoadProgressPercentage
   */
  float GetLoadProgressPercentage() const;

public: // API — Custom Headers
  /**
   * @copydoc Dali::Ui::WebView::AddCustomHeader
   */
  bool AddCustomHeader(const Dali::String& name, const Dali::String& value);

  /**
   * @copydoc Dali::Ui::WebView::RemoveCustomHeader
   */
  bool RemoveCustomHeader(const Dali::String& name);

public: // API — Scroll
  /**
   * @copydoc Dali::Ui::WebView::GetScrollPosition
   */
  Dali::Vector2 GetScrollPosition() const;

  /**
   * @copydoc Dali::Ui::WebView::GetScrollSize
   */
  Dali::Vector2 GetScrollSize() const;

  /**
   * @copydoc Dali::Ui::WebView::GetContentSize
   */
  Dali::Vector2 GetContentSize() const;

  /**
   * @copydoc Dali::Ui::WebView::ScrollBy
   */
  void ScrollBy(int32_t deltaX, int32_t deltaY);

  /**
   * @copydoc Dali::Ui::WebView::ScrollEdgeBy
   */
  bool ScrollEdgeBy(int32_t deltaX, int32_t deltaY);

public: // API — Navigation
  /**
   * @copydoc Dali::Ui::WebView::CanGoForward
   */
  bool CanGoForward() const;

  /**
   * @copydoc Dali::Ui::WebView::GoForward
   */
  void GoForward();

  /**
   * @copydoc Dali::Ui::WebView::CanGoBack
   */
  bool CanGoBack() const;

  /**
   * @copydoc Dali::Ui::WebView::GoBack
   */
  void GoBack();

  /**
   * @copydoc Dali::Ui::WebView::ClearHistory
   */
  void ClearHistory();

  /**
   * @copydoc Dali::Ui::WebView::ClearAllTilesResources
   */
  void ClearAllTilesResources();

  /**
   * @copydoc Dali::Ui::WebView::ClearCache
   */
  void ClearCache();

  /**
   * @copydoc Dali::Ui::WebView::ClearCookies
   */
  void ClearCookies();

  /**
   * @copydoc Dali::Ui::WebView::ExitFullscreen
   */
  void ExitFullscreen();

public: // API — JavaScript
  /**
   * @copydoc Dali::Ui::WebView::EvaluateJavaScript(const Dali::String&)
   */
  void EvaluateJavaScript(const Dali::String& script);

  /**
   * @copydoc Dali::Ui::WebView::EvaluateJavaScript(const Dali::String&, JavaScriptCallback)
   */
  void EvaluateJavaScript(const Dali::String& script, WebView::JavaScriptCallback callback);

  /**
   * @copydoc Dali::Ui::WebView::AddJavaScriptMessageHandler
   */
  void AddJavaScriptMessageHandler(const Dali::String& exposedObjectName, WebView::JavaScriptCallback callback);

  /**
   * @copydoc Dali::Ui::WebView::RegisterJavaScriptAlertCallback
   */
  void RegisterJavaScriptAlertCallback(WebView::JavaScriptAlertCallback callback);

  /**
   * @copydoc Dali::Ui::WebView::JavaScriptAlertReply
   */
  void JavaScriptAlertReply();

  /**
   * @copydoc Dali::Ui::WebView::RegisterJavaScriptConfirmCallback
   */
  void RegisterJavaScriptConfirmCallback(WebView::JavaScriptConfirmCallback callback);

  /**
   * @copydoc Dali::Ui::WebView::JavaScriptConfirmReply
   */
  void JavaScriptConfirmReply(bool confirmed);

  /**
   * @copydoc Dali::Ui::WebView::RegisterJavaScriptPromptCallback
   */
  void RegisterJavaScriptPromptCallback(WebView::JavaScriptPromptCallback callback);

  /**
   * @copydoc Dali::Ui::WebView::JavaScriptPromptReply
   */
  void JavaScriptPromptReply(const Dali::String& result);

public: // API — Scale & Zoom
  /**
   * @copydoc Dali::Ui::WebView::SetScaleFactor
   */
  void SetScaleFactor(float scaleFactor, Dali::Vector2 point);

  /**
   * @copydoc Dali::Ui::WebView::GetScaleFactor
   */
  float GetScaleFactor() const;

  /**
   * @copydoc Dali::Ui::WebView::GetPageZoomFactor
   */
  float GetPageZoomFactor() const;

  /**
   * @copydoc Dali::Ui::WebView::SetPageZoomFactor
   */
  void SetPageZoomFactor(float zoomFactor);

  /**
   * @copydoc Dali::Ui::WebView::GetTextZoomFactor
   */
  float GetTextZoomFactor() const;

  /**
   * @copydoc Dali::Ui::WebView::SetTextZoomFactor
   */
  void SetTextZoomFactor(float zoomFactor);

public: // API — Screenshot
  /**
   * @copydoc Dali::Ui::WebView::GetScreenshot
   */
  Dali::Ui::ImageView GetScreenshot(Dali::BoundsInteger viewArea, float scaleFactor);

  /**
   * @copydoc Dali::Ui::WebView::GetScreenshotAsynchronously
   */
  bool GetScreenshotAsynchronously(Dali::BoundsInteger viewArea, float scaleFactor, WebView::ScreenshotCapturedCallback callback);

public: // API — Page Info
  /**
   * @copydoc Dali::Ui::WebView::GetTitle
   */
  Dali::String GetTitle() const;

  /**
   * @copydoc Dali::Ui::WebView::GetFavicon
   */
  Dali::Ui::ImageView GetFavicon() const;

  /**
   * @copydoc Dali::Ui::WebView::GetSelectedText
   */
  Dali::String GetSelectedText() const;

  /**
   * @copydoc Dali::Ui::WebView::GetPlainTextAsynchronously
   */
  void GetPlainTextAsynchronously(WebView::PlainTextCallback callback);

  /**
   * @copydoc Dali::Ui::WebView::FindText
   */
  bool FindText(const Dali::String& text, WebViewFindOption options, uint32_t maxMatchCount);

public: // API — Document Appearance
  /**
   * @copydoc Dali::Ui::WebView::SetDocumentBackgroundColor
   */
  void SetDocumentBackgroundColor(const Dali::Vector4& color);

  /**
   * @copydoc Dali::Ui::WebView::SetTilesClearedWhenHidden
   */
  void SetTilesClearedWhenHidden(bool cleared);

  /**
   * @copydoc Dali::Ui::WebView::SetTileCoverAreaMultiplier
   */
  void SetTileCoverAreaMultiplier(float multiplier);

  /**
   * @copydoc Dali::Ui::WebView::SetCursorEnabledByClient
   */
  void SetCursorEnabledByClient(bool enabled);

public: // API — Input & Video
  /**
   * @copydoc Dali::Ui::WebView::SetMouseEventsEnabled
   */
  void SetMouseEventsEnabled(bool enabled);

  /**
   * @copydoc Dali::Ui::WebView::IsMouseEventsEnabled
   */
  bool IsMouseEventsEnabled() const;

  /**
   * @copydoc Dali::Ui::WebView::SetKeyEventsEnabled
   */
  void SetKeyEventsEnabled(bool enabled);

  /**
   * @copydoc Dali::Ui::WebView::IsKeyEventsEnabled
   */
  bool IsKeyEventsEnabled() const;

  /**
   * @copydoc Dali::Ui::WebView::FeedMouseWheel
   */
  void FeedMouseWheel(bool yDirection, int step, int x, int y);

  /**
   * @copydoc Dali::Ui::WebView::FeedKeyEvent
   */
  bool FeedKeyEvent(const KeyEvent& keyEvent);

  /**
   * @copydoc Dali::Ui::WebView::FeedTouchEvent
   */
  bool FeedTouchEvent(const TouchEvent& touchEvent);

  /**
   * @copydoc Dali::Ui::WebView::SetVideoHole
   */
  void SetVideoHole(bool enabled, bool isWaylandWindow);

  /**
   * @copydoc Dali::Ui::WebView::SetVideoHoleEnabled
   */
  void SetVideoHoleEnabled(bool enabled);

  /**
   * @copydoc Dali::Ui::WebView::IsVideoHoleEnabled
   */
  bool IsVideoHoleEnabled() const;

  /**
   * @copydoc Dali::Ui::WebView::CheckVideoPlayingAsynchronously
   */
  bool CheckVideoPlayingAsynchronously(WebView::VideoPlayingCallback callback);

public: // Signal emission helpers (called by WebView to emit signals)
  /// @brief Emits the PageLoadStartedSignal
  void EmitPageLoadStarted(const Dali::String& url);
  /// @brief Emits the PageLoadInProgressSignal
  void EmitPageLoadInProgress(const Dali::String& url);
  /// @brief Emits the PageLoadFinishedSignal
  void EmitPageLoadFinished(const Dali::String& url);
  /// @brief Emits the PageLoadErrorSignal
  void EmitPageLoadError(const WebViewPageLoadError& error);
  /// @brief Emits the ScrollEdgeReachedSignal
  void EmitScrollEdgeReached(WebViewScrollEdge edge);
  /// @brief Emits the OverScrolledSignal
  void EmitOverScrolled(WebViewOverScrolled overScrolled);
  /// @brief Emits the UrlChangedSignal
  void EmitUrlChanged(const Dali::String& url);
  /// @brief Emits the FrameRenderedSignal
  void EmitFrameRendered();
  /// @brief Emits the FullscreenEnteredSignal
  void EmitFullscreenEntered();
  /// @brief Emits the FullscreenExitedSignal
  void EmitFullscreenExited();
  /// @brief Emits the TextFoundSignal
  void EmitTextFound(uint32_t count);
  /// @brief Emits the GeolocationPermissionSignal and returns the result
  bool EmitGeolocationPermission(const Dali::String& host, const Dali::String& protocol);
  /// @brief Emits the WebProcessCrashedSignal
  void EmitWebProcessCrashed();

private: // From ViewImpl
  /**
   * @copydoc ViewImpl::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc ViewImpl::OnMeasure
   */
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;

  /**
   * @copydoc ViewImpl::OnArrange
   */
  MeasuredSize OnArrange(const LayoutRect& bounds) override;

  /**
   * @copydoc ViewImpl::OnKeyEvent
   */
  bool OnKeyEvent(const Dali::KeyEvent& event) override;

  /**
   * @brief Handles touch events from Actor::TouchedSignal.
   *
   * Forwards the touch to the web engine via SendTouchEvent.
   */
  bool OnTouchEvent(Dali::Actor actor, Dali::TouchEvent touch);

private: // Internal rendering callbacks
  /**
   * @brief Called by the web engine each time a new frame is ready.
   *
   * Creates or updates the image visual from the latest NativeImage texture.
   */
  void OnFrameRendered();

  /**
   * @brief Called when a position, size, or scale PropertyNotification fires.
   *
   * Re-calculates the screen display area and forwards it to the web engine.
   */
  void OnDisplayAreaUpdated(Dali::PropertyNotification source);

  /**
   * @brief Applies a new display area to the web engine and updates the visual if needed.
   *
   * @param[in] displayArea The new display rectangle in screen coordinates
   */
  void SetDisplayArea(const Dali::BoundsInteger& displayArea);

  /**
   * @brief Enables or disables alpha blending on the web content actor.
   *
   * @param[in] blend true to enable blending (default), false to disable (video hole mode)
   */
  void EnableBlendMode(bool blend);

  /**
   * @brief Wraps a PixelData in an ImageView for use as a favicon or screenshot thumbnail.
   *
   * @param[in] pixelData The pixel data to wrap
   * @return An ImageView showing the pixel data, or an uninitialized handle if pixelData is invalid
   */
  static Dali::Ui::ImageView CreateImageViewFromPixelData(Dali::PixelData pixelData);

private: // Internal conversion helpers
  /**
   * @brief Converts a Dali::String to a std::string for dali-adaptor interop.
   *
   * @param[in] str The Dali::String to convert
   * @return The corresponding std::string
   */
  static std::string ToStdString(const Dali::String& str);

  /**
   * @brief Converts a std::string from dali-adaptor to a Dali::String.
   *
   * @param[in] str The std::string to convert
   * @return The corresponding Dali::String
   */
  static Dali::String ToDaliString(const std::string& str);

  /**
   * @brief Converts a WebEnginePlugin::ScrollEdge to Dali::Ui::WebViewScrollEdge.
   */
  static WebViewScrollEdge ToUiScrollEdge(Dali::WebEnginePlugin::ScrollEdge edge);

  /**
   * @brief Converts a WebEnginePlugin::OverScrolled to Dali::Ui::WebViewOverScrolled.
   */
  static WebViewOverScrolled ToUiOverScrolled(Dali::WebEnginePlugin::OverScrolled overScrolled);

  /**
   * @brief Converts a WebViewFindOption to WebEnginePlugin::FindOption.
   */
  static Dali::WebEnginePlugin::FindOption ToEngineFindOption(WebViewFindOption options);

  /**
   * @brief Converts a WebEngineLoadError::ErrorCode to Dali::Ui::WebViewPageLoadErrorCode.
   */
  static WebViewPageLoadErrorCode ToUiPageLoadErrorCode(Dali::WebEngineLoadError::ErrorCode code);

  /**
   * @brief Converts a WebEngineLoadError::ErrorType to Dali::Ui::WebViewPageLoadErrorType.
   */
  static WebViewPageLoadErrorType ToUiPageLoadErrorType(Dali::WebEngineLoadError::ErrorType type);

private:
  // Not copyable or movable
  WebViewImpl(const WebViewImpl&)            = delete;
  WebViewImpl(WebViewImpl&&)                 = delete;
  WebViewImpl& operator=(const WebViewImpl&) = delete;
  WebViewImpl& operator=(WebViewImpl&&)      = delete;

private:                                              // Data — Rendering
  Ui::Visual::Base    mVisual;                        ///< Current image visual showing the web content
  Dali::Size          mWebViewSize;                   ///< Current web view size (width, height)
  Dali::BoundsInteger mWebViewArea;                   ///< Last known display area in screen coordinates
  uint32_t            mLastRenderedNativeImageWidth;  ///< Width of the last uploaded NativeImage texture
  uint32_t            mLastRenderedNativeImageHeight; ///< Height of the last uploaded NativeImage texture
  bool                mVisualChangeRequired;          ///< True when the visual must be re-created on next frame
  bool                mVideoHoleEnabled;              ///< Video hole rendering enabled
  bool                mMouseEventsEnabled;            ///< Mouse events forwarded to web content
  bool                mKeyEventsEnabled;              ///< Key events forwarded to web content

  // Display-area update notifications
  Dali::PropertyNotification mPositionUpdateNotification; ///< Fires on world-position change
  Dali::PropertyNotification mSizeUpdateNotification;     ///< Fires on size change
  Dali::PropertyNotification mScaleUpdateNotification;    ///< Fires on world-scale change

public: // Signal members (accessible by WebView for signal emission)
  WebView::PageLoadStartedSignalType       mPageLoadStartedSignal;
  WebView::PageLoadInProgressSignalType    mPageLoadInProgressSignal;
  WebView::PageLoadFinishedSignalType      mPageLoadFinishedSignal;
  WebView::PageLoadErrorSignalType         mPageLoadErrorSignal;
  WebView::ScrollEdgeReachedSignalType     mScrollEdgeReachedSignal;
  WebView::OverScrolledSignalType          mOverScrolledSignal;
  WebView::UrlChangedSignalType            mUrlChangedSignal;
  WebView::FrameRenderedSignalType         mFrameRenderedSignal;
  WebView::FullscreenEnteredSignalType     mFullscreenEnteredSignal;
  WebView::FullscreenExitedSignalType      mFullscreenExitedSignal;
  WebView::TextFoundSignalType             mTextFoundSignal;
  WebView::GeolocationPermissionSignalType mGeolocationPermissionSignal;
  WebView::WebProcessCrashedSignalType     mWebProcessCrashedSignal;

private:                      // Data — Engine
  Dali::WebEngine mWebEngine; ///< The underlying dali-adaptor web engine instance

  // One-shot screenshot callback — replaced on each GetScreenshotAsynchronously() call.
  std::function<void(Dali::Ui::ImageView)> mPendingScreenshotCallback;

  // Stored callbacks for web engine events
  Dali::WebEnginePlugin::JavaScriptAlertCallback   mJavaScriptAlertCallback;
  Dali::WebEnginePlugin::JavaScriptConfirmCallback mJavaScriptConfirmCallback;
  Dali::WebEnginePlugin::JavaScriptPromptCallback  mJavaScriptPromptCallback;
};

} // namespace Integration
} // namespace Ui
} // namespace Dali
