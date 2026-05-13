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
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/callback.h>
#include <dali-ui-foundation/public-api/image-view.h>
#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-foundation/public-api/web-view-types.h>

namespace Dali
{
namespace Ui
{
namespace Integration DALI_INTERNAL
{
class WebViewImpl;
} //namespace Integration DALI_INTERNAL

#include "web-view.autogen.h"
/**
 * @brief WebView is a View for displaying web content.
 *
 * This enables embedding web pages in the application.
 * For a working WebView, a web engine plugin for the target platform must be provided.
 *
 * @code
 * WebView view = WebView::New();
 * view.LoadUrl("https://www.example.com");
 * @endcode
 */
class DALI_UI_API WebView : public View
{
public: // Callback types
  // -- JavaScript --
  /// @brief Callback for JavaScript result or message.
  using JavaScriptCallback = Callback<void(const Dali::String&)>;

  /// @brief Callback for JavaScript alert dialog.
  using JavaScriptAlertCallback = Callback<bool(const Dali::String&)>;

  /// @brief Callback for JavaScript confirm dialog.
  using JavaScriptConfirmCallback = Callback<bool(const Dali::String&)>;

  /// @brief Callback for JavaScript prompt dialog.
  using JavaScriptPromptCallback = Callback<bool(const Dali::String&, const Dali::String&)>;

  // -- Async operations --
  /// @brief Callback for screenshot captured asynchronously.
  using ScreenshotCapturedCallback = Callback<void(Dali::Ui::ImageView)>;

  /// @brief Callback for plain text retrieval.
  using PlainTextCallback = Callback<void(const Dali::String&)>;

  /// @brief Callback for video playing status check.
  using VideoPlayingCallback = Callback<void(bool)>;

public: // Signal types
  // -- Page loading --
  /// @brief Signal type for page load started events.
  using PageLoadStartedSignalType = Signal<void(WebView, const Dali::String&)>;

  /// @brief Signal type for page load in progress events.
  using PageLoadInProgressSignalType = Signal<void(WebView, const Dali::String&)>;

  /// @brief Signal type for page load finished events.
  using PageLoadFinishedSignalType = Signal<void(WebView, const Dali::String&)>;

  // -- Navigation --
  /// @brief Signal type for URL changes.
  using UrlChangedSignalType = Signal<void(WebView, const Dali::String&)>;

  // -- Rendering --
  /// @brief Signal type for frame rendered events.
  using FrameRenderedSignalType = Signal<void(WebView)>;

  // -- Scroll --
  /// @brief Signal type for scroll edge reached events.
  using ScrollEdgeReachedSignalType = Signal<void(WebView, WebViewScrollEdge)>;

  /// @brief Signal type for over-scroll events.
  using OverScrolledSignalType = Signal<void(WebView, WebViewOverScrolled)>;

  // -- Fullscreen --
  /// @brief Signal type for fullscreen enter events.
  using FullscreenEnteredSignalType = Signal<void(WebView)>;

  /// @brief Signal type for fullscreen exit events.
  using FullscreenExitedSignalType = Signal<void(WebView)>;

  // -- Text search --
  /// @brief Signal type for text search results.
  using TextFoundSignalType = Signal<void(WebView, uint32_t)>;

  // -- Geolocation --
  /// @brief Signal type for geolocation permission requests.
  using GeolocationPermissionSignalType = Signal<bool(WebView, const Dali::String&, const Dali::String&)>;

  // -- Process events --
  /// @brief Signal type for web process crash events.
  using WebProcessCrashedSignalType = Signal<void(WebView)>;

public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized WebView handle.
   */
  WebView();

  /**
   * @brief Creates an initialized WebView.
   * @return A handle to a newly allocated Dali resource
   */
  static WebView New();

  /**
   * @brief Creates an initialized WebView and immediately loads the given URL.
   * @param[in] url The URL to load
   * @return A handle to a newly allocated Dali resource
   */
  static WebView New(const Dali::String& url);

  /**
   * @brief Destructor.
   */
  ~WebView();

  /**
   * @brief Copy constructor.
   */
  WebView(const WebView& webView);

  /**
   * @brief Move constructor.
   */
  WebView(WebView&& rhs) noexcept;

public: // Operators
  /**
   * @brief Copy assignment operator.
   */
  WebView& operator=(const WebView& webView);

  /**
   * @brief Move assignment operator.
   */
  WebView& operator=(WebView&& rhs) noexcept;

public: // Static Methods
  /**
   * @brief Downcasts a handle to WebView.
   */
  static WebView DownCast(BaseHandle handle);

public: // Setters for chaining
  // @CHAIN_START(WebView, View)

  /**
   * @brief Sets the user agent string.
   *
   * @param[in] userAgent The user agent string
   * @return Reference to this for fluent chaining
   */
  WebView& SetUserAgent(const Dali::String& userAgent);

  /**
   * @brief Loads the specified URL.
   *
   * @param[in] url The URL to load
   * @return Reference to this for fluent chaining
   */
  WebView& LoadUrl(const Dali::String& url);

  /**
   * @brief Loads the specified HTML string.
   *
   * @param[in] htmlString The HTML string to load
   * @return Reference to this for fluent chaining
   */
  WebView& LoadHtmlString(const Dali::String& htmlString);

  /**
   * @brief Reloads the current page.
   *
   * @return Reference to this for fluent chaining
   */
  WebView& Reload();

  /**
   * @brief Stops loading the current page.
   *
   * @return Reference to this for fluent chaining
   */
  WebView& StopLoading();

  /**
   * @brief Suspends the web view.
   *
   * @return Reference to this for fluent chaining
   */
  WebView& Suspend();

  /**
   * @brief Resumes the web view.
   *
   * @return Reference to this for fluent chaining
   */
  WebView& Resume();

  /**
   * @brief Navigates forward in history.
   *
   * @return Reference to this for fluent chaining
   */
  WebView& GoForward();

  /**
   * @brief Navigates backward in history.
   *
   * @return Reference to this for fluent chaining
   */
  WebView& GoBack();

  /**
   * @brief Clears the navigation history.
   *
   * @return Reference to this for fluent chaining
   */
  WebView& ClearHistory();

  /**
   * @brief Sets the page zoom factor.
   *
   * @param[in] zoomFactor The zoom factor
   * @return Reference to this for fluent chaining
   */
  WebView& SetPageZoomFactor(float zoomFactor);

  /**
   * @brief Sets the text zoom factor.
   *
   * @param[in] zoomFactor The zoom factor
   * @return Reference to this for fluent chaining
   */
  WebView& SetTextZoomFactor(float zoomFactor);

  /**
   * @brief Sets whether mouse events are enabled.
   *
   * @param[in] enabled True to enable mouse events
   * @return Reference to this for fluent chaining
   */
  WebView& SetMouseEventsEnabled(bool enabled);

  /**
   * @brief Sets whether key events are enabled.
   *
   * @param[in] enabled True to enable key events
   * @return Reference to this for fluent chaining
   */
  WebView& SetKeyEventsEnabled(bool enabled);

  /**
   * @brief Sets whether video hole is enabled.
   *
   * @param[in] enabled True to enable video hole
   * @return Reference to this for fluent chaining
   */
  WebView& SetVideoHoleEnabled(bool enabled);

  /**
   * @brief Sets the document background color.
   *
   * @param[in] color The background color
   * @return Reference to this for fluent chaining
   */
  WebView& SetDocumentBackgroundColor(const Dali::Vector4& color);

  /**
   * @brief Sets whether tiles are cleared when hidden.
   *
   * @param[in] cleared True to clear tiles when hidden
   * @return Reference to this for fluent chaining
   */
  WebView& SetTilesClearedWhenHidden(bool cleared);

  /**
   * @brief Sets the tile cover area multiplier.
   *
   * @param[in] multiplier The multiplier value
   * @return Reference to this for fluent chaining
   */
  WebView& SetTileCoverAreaMultiplier(float multiplier);

  /**
   * @brief Sets whether cursor is enabled by client.
   *
   * @param[in] enabled True to enable cursor by client
   * @return Reference to this for fluent chaining
   */
  WebView& SetCursorEnabledByClient(bool enabled);

  // @CHAIN_END

public: // Getters (non-chainable)
  /**
   * @brief Gets the current URL.
   *
   * @return The current URL
   */
  Dali::String GetUrl() const;

  /**
   * @brief Gets the user agent string.
   *
   * @return The user agent string
   */
  Dali::String GetUserAgent() const;

  /**
   * @brief Gets the scroll position.
   *
   * @return The current scroll position
   */
  Dali::Vector2 GetScrollPosition() const;

  /**
   * @brief Gets the scroll size.
   *
   * @return The scroll size
   */
  Dali::Vector2 GetScrollSize() const;

  /**
   * @brief Gets the content size.
   *
   * @return The content size
   */
  Dali::Vector2 GetContentSize() const;

  /**
   * @brief Gets the page title.
   *
   * @return The page title
   */
  Dali::String GetTitle() const;

  /**
   * @brief Gets the load progress percentage.
   *
   * @return The load progress percentage (0.0 - 100.0)
   */
  float GetLoadProgressPercentage() const;

  /**
   * @brief Gets the page zoom factor.
   *
   * @return The page zoom factor
   */
  float GetPageZoomFactor() const;

  /**
   * @brief Gets the text zoom factor.
   *
   * @return The text zoom factor
   */
  float GetTextZoomFactor() const;

  /**
   * @brief Gets whether mouse events are enabled.
   *
   * @return True if mouse events are enabled
   */
  bool GetMouseEventsEnabled() const;

  /**
   * @brief Gets whether key events are enabled.
   *
   * @return True if key events are enabled
   */
  bool GetKeyEventsEnabled() const;

  /**
   * @brief Gets whether video hole is enabled.
   *
   * @return True if video hole is enabled
   */
  bool GetVideoHoleEnabled() const;

  /**
   * @brief Gets the currently selected text.
   *
   * @return The selected text
   */
  Dali::String GetSelectedText() const;

  /**
   * @brief Gets the scale factor.
   *
   * @return The scale factor
   */
  float GetScaleFactor() const;

public: // Query methods (non-chainable, bool return)
  /**
   * @brief Checks if forward navigation is possible.
   *
   * @return True if can go forward
   */
  bool CanGoForward();

  /**
   * @brief Checks if backward navigation is possible.
   *
   * @return True if can go back
   */
  bool CanGoBack();

  /**
   * @brief Loads HTML string with override current entry.
   *
   * @param[in] html The HTML string
   * @param[in] basicUri The base URI
   * @param[in] unreachableUrl The unreachable URL
   * @return True on success
   */
  bool LoadHtmlStringOverrideCurrentEntry(const Dali::String& html, const Dali::String& basicUri, const Dali::String& unreachableUrl);

  /**
   * @brief Loads contents.
   *
   * @param[in] contents The content data
   * @param[in] contentSize The size of content data
   * @param[in] mimeType The MIME type
   * @param[in] encoding The encoding
   * @param[in] baseUri The base URI
   * @return True on success
   */
  bool LoadContents(const int8_t* contents, uint32_t contentSize, const Dali::String& mimeType, const Dali::String& encoding, const Dali::String& baseUri);

  /**
   * @brief Reloads without cache.
   *
   * @return True on success
   */
  bool ReloadWithoutCache();

  /**
   * @brief Adds a custom header.
   *
   * @param[in] name The header name
   * @param[in] value The header value
   * @return True on success
   */
  bool AddCustomHeader(const Dali::String& name, const Dali::String& value);

  /**
   * @brief Removes a custom header.
   *
   * @param[in] name The header name
   * @return True on success
   */
  bool RemoveCustomHeader(const Dali::String& name);

public: // Actions (non-chainable, void or special return)
  /**
   * @brief Scrolls by the specified delta.
   *
   * @param[in] deltaX The horizontal scroll delta
   * @param[in] deltaY The vertical scroll delta
   */
  void ScrollBy(int32_t deltaX, int32_t deltaY);

  /**
   * @brief Scrolls by edge delta.
   *
   * @param[in] deltaX The horizontal scroll delta
   * @param[in] deltaY The vertical scroll delta
   * @return True on success
   */
  bool ScrollEdgeBy(int32_t deltaX, int32_t deltaY);

  /**
   * @brief Exits fullscreen mode.
   */
  void ExitFullscreen();

  /**
   * @brief Clears all tiles resources.
   */
  void ClearAllTilesResources();

  /**
   * @brief Suspends network loading.
   */
  void SuspendNetworkLoading();

  /**
   * @brief Resumes network loading.
   */
  void ResumeNetworkLoading();

  /**
   * @brief Sets the scale factor at a point.
   *
   * @param[in] scaleFactor The scale factor
   * @param[in] point The point to scale around
   */
  void SetScaleFactor(float scaleFactor, Dali::Vector2 point);

  /**
   * @brief Feeds mouse wheel event.
   *
   * @param[in] yDirection True for Y direction
   * @param[in] step The wheel step
   * @param[in] x The X coordinate
   * @param[in] y The Y coordinate
   */
  void FeedMouseWheel(bool yDirection, int step, int x, int y);

  /**
   * @brief Feeds a key event directly to the web engine.
   *
   * Use this to forward key events to the web engine when automatic key event
   * forwarding is disabled (SetKeyEventsEnabled(false)).
   *
   * @param[in] keyEvent The key event to feed
   * @return True if the event was consumed by the web engine
   */
  bool FeedKeyEvent(const KeyEvent& keyEvent);

  /**
   * @brief Feeds a touch event directly to the web engine.
   *
   * Use this to forward touch events to the web engine when automatic touch
   * forwarding is disabled (SetMouseEventsEnabled(false)).
   *
   * @param[in] touchEvent The touch event to feed
   * @return True if the event was consumed by the web engine
   */
  bool FeedTouchEvent(const TouchEvent& touchEvent);

public: // Screenshot & Page Info
  /**
   * @brief Gets a screenshot.
   *
   * @param[in] viewArea The view area
   * @param[in] scaleFactor The scale factor
   * @return An ImageView containing the screenshot
   */
  Dali::Ui::ImageView GetScreenshot(Dali::BoundsInteger viewArea, float scaleFactor);

  /**
   * @brief Gets a screenshot asynchronously.
   *
   * @param[in] viewArea The view area
   * @param[in] scaleFactor The scale factor
   * @param[in] callback The callback to receive the screenshot
   * @return True if the operation started successfully
   */
  bool GetScreenshotAsynchronously(Dali::BoundsInteger viewArea, float scaleFactor, ScreenshotCapturedCallback callback);

  /**
   * @brief Gets the favicon.
   *
   * @return An ImageView containing the favicon
   */
  Dali::Ui::ImageView GetFavicon() const;

  /**
   * @brief Gets plain text asynchronously.
   *
   * @param[in] callback The callback to receive the plain text
   */
  void GetPlainTextAsynchronously(PlainTextCallback callback);

  /**
   * @brief Checks if video is playing asynchronously.
   *
   * @param[in] callback The callback to receive the result
   * @return True if the operation started successfully
   */
  bool CheckVideoPlayingAsynchronously(VideoPlayingCallback callback);

public: // JavaScript
  /**
   * @brief Evaluates JavaScript code.
   *
   * @param[in] script The JavaScript code to evaluate
   */
  void EvaluateJavaScript(const Dali::String& script);

  /**
   * @brief Evaluates JavaScript code with callback.
   *
   * @param[in] script The JavaScript code to evaluate
   * @param[in] callback The callback to receive the result
   */
  void EvaluateJavaScript(const Dali::String& script, JavaScriptCallback callback);

  /**
   * @brief Adds a JavaScript message handler.
   *
   * @param[in] exposedObjectName The exposed object name
   * @param[in] callback The callback to handle messages
   */
  void AddJavaScriptMessageHandler(const Dali::String& exposedObjectName, JavaScriptCallback callback);

  /**
   * @brief Registers a JavaScript alert callback.
   *
   * @param[in] callback The callback for alert dialogs
   */
  void RegisterJavaScriptAlertCallback(JavaScriptAlertCallback callback);

  /**
   * @brief Replies to a JavaScript alert.
   */
  void JavaScriptAlertReply();

  /**
   * @brief Registers a JavaScript confirm callback.
   *
   * @param[in] callback The callback for confirm dialogs
   */
  void RegisterJavaScriptConfirmCallback(JavaScriptConfirmCallback callback);

  /**
   * @brief Replies to a JavaScript confirm.
   *
   * @param[in] confirmed The confirmation result
   */
  void JavaScriptConfirmReply(bool confirmed);

  /**
   * @brief Registers a JavaScript prompt callback.
   *
   * @param[in] callback The callback for prompt dialogs
   */
  void RegisterJavaScriptPromptCallback(JavaScriptPromptCallback callback);

  /**
   * @brief Replies to a JavaScript prompt.
   *
   * @param[in] result The prompt result
   */
  void JavaScriptPromptReply(const Dali::String& result);

public: // Signals
  /**
   * @brief Signal emitted when page load starts.
   */
  PageLoadStartedSignalType& PageLoadStartedSignal();

  /**
   * @brief Signal emitted while page is loading.
   */
  PageLoadInProgressSignalType& PageLoadInProgressSignal();

  /**
   * @brief Signal emitted when page load finishes.
   */
  PageLoadFinishedSignalType& PageLoadFinishedSignal();

  /**
   * @brief Signal emitted when URL changes.
   */
  UrlChangedSignalType& UrlChangedSignal();

  /**
   * @brief Signal emitted when scroll edge is reached.
   */
  ScrollEdgeReachedSignalType& ScrollEdgeReachedSignal();

  /**
   * @brief Signal emitted when over-scroll occurs.
   */
  OverScrolledSignalType& OverScrolledSignal();

  /**
   * @brief Signal emitted when a frame is rendered.
   */
  FrameRenderedSignalType& FrameRenderedSignal();

  /**
   * @brief Signal emitted when entering fullscreen.
   */
  FullscreenEnteredSignalType& FullscreenEnteredSignal();

  /**
   * @brief Signal emitted when exiting fullscreen.
   */
  FullscreenExitedSignalType& FullscreenExitedSignal();

  /**
   * @brief Signal emitted when text is found.
   */
  TextFoundSignalType& TextFoundSignal();

  /**
   * @brief Signal emitted for geolocation permission request.
   */
  GeolocationPermissionSignalType& GeolocationPermissionSignal();

  /**
   * @brief Signal emitted when web process crashes.
   */
  WebProcessCrashedSignalType& WebProcessCrashedSignal();

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL WebView(Integration::WebViewImpl& implementation);
  explicit DALI_INTERNAL WebView(Dali::Internal::CustomActor* internal);
  /// @endcond

public:
  DALI_UI_CHAIN_VIEW_METHODS(WebView)
};

} // namespace Ui
} // namespace Dali
