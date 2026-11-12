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
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-foundation/public-api/views/web/web-back-forward-list.h>
#include <dali-ui-foundation/public-api/views/web/web-engine-type.h>
#include <dali-ui-foundation/public-api/views/web/web-profile.h>
#include <dali-ui-foundation/public-api/views/web/web-settings.h>
#include <dali-ui-foundation/public-api/views/web/web-view-page-load-error.h>
#include <dali-ui-foundation/public-api/views/web/web-view-types.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/signals/callback.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
/** @brief Result of configuring process-wide web engine arguments. @SINCE_3_0.0 */
enum class WebEngineArgumentsResult : uint8_t
{
  SUCCESS,             ///< Settings stored; engine initialization and switch support are not guaranteed.
  INVALID_ARGUMENT,    ///< Invalid count, array, or argument pointer.
  ALREADY_INITIALIZED, ///< Initialization has started, including a failed attempt.
  BACKEND_UNAVAILABLE  ///< Plugin loading or required feature access failed.
};

namespace Integration DALI_INTERNAL
{
class WebViewImpl;
} //namespace Integration DALI_INTERNAL

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
 * @SINCE_3_0.0
 */
class DALI_UI_API WebView : public View
{
public: // Callback types
  /// @brief Callback for JavaScript result or message.
  /// @SINCE_3_0.0
  using JavaScriptCallback = Callback<void(const Dali::String&)>;

  /// @brief Callback for JavaScript alert dialog.
  /// @SINCE_3_0.0
  using JavaScriptAlertCallback = Callback<bool(const Dali::String&)>;

  /// @brief Callback for JavaScript confirm dialog.
  /// @SINCE_3_0.0
  using JavaScriptConfirmCallback = Callback<bool(const Dali::String&)>;

  /// @brief Callback for JavaScript prompt dialog.
  /// @SINCE_3_0.0
  using JavaScriptPromptCallback = Callback<bool(const Dali::String&, const Dali::String&)>;
  /// @brief Callback for screenshot captured asynchronously.
  /// @SINCE_3_0.0
  using ScreenshotCapturedCallback = Callback<void(Dali::Ui::ImageView)>;

  /// @brief Callback for plain text retrieval.
  /// @SINCE_3_0.0
  using PlainTextCallback = Callback<void(const Dali::String&)>;

  /// @brief Callback for video playing status check.
  /// @SINCE_3_0.0
  using VideoPlayingCallback = Callback<void(bool)>;

public: // Signal types
  /// @brief Signal type for page load started events.
  /// @SINCE_3_0.0
  using PageLoadStartedSignalType = Signal<void(WebView, const Dali::String&)>;

  /// @brief Signal type for page load in progress events.
  /// @SINCE_3_0.0
  using PageLoadInProgressSignalType = Signal<void(WebView, const Dali::String&)>;

  /// @brief Signal type for page load finished events.
  /// @SINCE_3_0.0
  using PageLoadFinishedSignalType = Signal<void(WebView, const Dali::String&)>;
  /// @brief Signal type for page load error events.
  /// @SINCE_3_0.0
  using PageLoadErrorSignalType = Signal<void(WebView, const WebViewPageLoadError&)>;
  /// @brief Signal type for URL changes.
  /// @SINCE_3_0.0
  using UrlChangedSignalType = Signal<void(WebView, const Dali::String&)>;
  /// @brief Signal type for frame rendered events.
  /// @SINCE_3_0.0
  using FrameRenderedSignalType = Signal<void(WebView)>;
  /// @brief Signal type for scroll edge reached events.
  /// @SINCE_3_0.0
  using ScrollEdgeReachedSignalType = Signal<void(WebView, WebViewScrollEdge)>;

  /// @brief Signal type for over-scroll events.
  /// @SINCE_3_0.0
  using OverScrolledSignalType = Signal<void(WebView, WebViewOverScrolled)>;
  /// @brief Signal type for fullscreen enter events.
  /// @SINCE_3_0.0
  using FullscreenEnteredSignalType = Signal<void(WebView)>;

  /// @brief Signal type for fullscreen exit events.
  /// @SINCE_3_0.0
  using FullscreenExitedSignalType = Signal<void(WebView)>;
  /// @brief Signal type for text search results.
  /// @SINCE_3_0.0
  using TextFoundSignalType = Signal<void(WebView, uint32_t)>;
  /// @brief Signal type for geolocation permission requests.
  /// @SINCE_3_0.0
  using GeolocationPermissionSignalType = Signal<bool(WebView, const Dali::String&, const Dali::String&)>;
  /// @brief Signal type for web process crash events.
  /// @SINCE_3_0.0
  using WebProcessCrashedSignalType = Signal<void(WebView)>;

public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized WebView handle.
   * @SINCE_3_0.0
   */
  WebView();

  /**
   * @brief Creates an initialized WebView.
   * @SINCE_3_0.0
   * @return A handle to a newly allocated Dali resource
   */
  static WebView New();

  /**
   * @brief Creates an initialized WebView and immediately loads the given URL.
   * @SINCE_3_0.0
   * @param[in] url The URL to load
   * @return A handle to a newly allocated Dali resource
   */
  static WebView New(const Dali::String& url);

  /**
   * @brief Creates a view with the default profile; all arguments are ignored.
   * @SINCE_3_0.0
   * @deprecated Use SetWebEngineArguments() before engine initialization and
   * WebProfile::New() with WebView::New(profile) for private browsing.
   * @param[in] argc Ignored
   * @param[in] argv Ignored
   * @return A new WebView, or an empty handle on backend failure
   */
  static WebView New(uint32_t argc, char** argv);

  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~WebView();

  /**
   * @brief Copy constructor.
   * @SINCE_3_0.0
   */
  WebView(const WebView& webView);

  /**
   * @brief Move constructor.
   * @SINCE_3_0.0
   */
  WebView(WebView&& rhs) noexcept;

public: // Operators
  /**
   * @brief Copy assignment operator.
   * @SINCE_3_0.0
   */
  WebView& operator=(const WebView& webView);

  /**
   * @brief Move assignment operator.
   * @SINCE_3_0.0
   */
  WebView& operator=(WebView&& rhs) noexcept;

  DALI_UI_VIEW_WITH(WebView)

public: // Static Methods
  /**
   * @brief Downcasts a handle to WebView.
   * @SINCE_3_0.0
   */
  static WebView DownCast(BaseHandle handle);

public: // Setters & actions
  /**
   * @brief Sets the user agent string.
   *
   * @SINCE_3_0.0
   * @param[in] userAgent The user agent string
   */
  void SetUserAgent(const Dali::String& userAgent);

  /**
   * @brief Loads the specified URL.
   *
   * @SINCE_3_0.0
   * @param[in] url The URL to load
   */
  void LoadUrl(const Dali::String& url);

  /**
   * @brief Loads the specified HTML string.
   *
   * @SINCE_3_0.0
   * @param[in] htmlString The HTML string to load
   */
  void LoadHtmlString(const Dali::String& htmlString);

  /**
   * @brief Reloads the current page.
   *
   * @SINCE_3_0.0
   */
  void Reload();

  /**
   * @brief Stops loading the current page.
   *
   * @SINCE_3_0.0
   */
  void StopLoading();

  /**
   * @brief Suspends the web view.
   *
   * @SINCE_3_0.0
   */
  void Suspend();

  /**
   * @brief Resumes the web view.
   *
   * @SINCE_3_0.0
   */
  void Resume();

  /**
   * @brief Navigates forward in history.
   *
   * @SINCE_3_0.0
   */
  void GoForward();

  /**
   * @brief Navigates backward in history.
   *
   * @SINCE_3_0.0
   */
  void GoBack();

  /**
   * @brief Clears the navigation history.
   *
   * @SINCE_3_0.0
   */
  void ClearHistory();

  /**
   * @brief Sets the page zoom factor.
   *
   * @SINCE_3_0.0
   * @param[in] zoomFactor The zoom factor
   */
  void SetPageZoomFactor(float zoomFactor);

  /**
   * @brief Sets the text zoom factor.
   *
   * @SINCE_3_0.0
   * @param[in] zoomFactor The zoom factor
   */
  void SetTextZoomFactor(float zoomFactor);

  /**
   * @brief Sets whether mouse events are enabled.
   *
   * @SINCE_3_0.0
   * @param[in] enabled True to enable mouse events
   */
  void SetMouseEventsEnabled(bool enabled);

  /**
   * @brief Sets whether key events are enabled.
   *
   * @SINCE_3_0.0
   * @param[in] enabled True to enable key events
   */
  void SetKeyEventsEnabled(bool enabled);

  /**
   * @brief Sets whether video hole is enabled.
   *
   * @SINCE_3_0.0
   * @param[in] enabled True to enable video hole
   */
  void SetVideoHoleEnabled(bool enabled);

  /**
   * @brief Sets the document background color.
   *
   * @SINCE_3_0.0
   * @param[in] color The background color
   */
  void SetDocumentBackgroundColor(const Dali::Vector4& color);

  /**
   * @brief Sets whether tiles are cleared when hidden.
   *
   * @deprecated This API is deprecated.
   *
   * @SINCE_3_0.0
   * @param[in] cleared True to clear tiles when hidden
   */
  void SetTilesClearedWhenHidden(bool cleared);

  /**
   * @brief Sets the tile cover area multiplier.
   *
   * @deprecated This API is deprecated.
   *
   * @SINCE_3_0.0
   * @param[in] multiplier The multiplier value
   */
  void SetTileCoverAreaMultiplier(float multiplier);

  /**
   * @brief Sets whether cursor is enabled by client.
   *
   * @SINCE_3_0.0
   * @param[in] enabled True to enable cursor by client
   */
  void SetCursorEnabledByClient(bool enabled);

public: // Getters
  /**
   * @brief Gets the current URL.
   *
   * @SINCE_3_0.0
   * @return The current URL
   */
  Dali::String GetUrl() const;

  /**
   * @brief Gets the user agent string.
   *
   * @SINCE_3_0.0
   * @return The user agent string
   */
  Dali::String GetUserAgent() const;

  /**
   * @brief Gets the scroll position.
   *
   * @SINCE_3_0.0
   * @return The current scroll position
   */
  Dali::Vector2 GetScrollPosition() const;

  /**
   * @brief Gets the scroll size.
   *
   * @SINCE_3_0.0
   * @return The scroll size
   */
  Dali::Vector2 GetScrollSize() const;

  /**
   * @brief Gets the content size.
   *
   * @SINCE_3_0.0
   * @return The content size
   */
  Dali::Vector2 GetContentSize() const;

  /**
   * @brief Gets the page title.
   *
   * @SINCE_3_0.0
   * @return The page title
   */
  Dali::String GetTitle() const;

  /**
   * @brief Gets the load progress percentage.
   *
   * @SINCE_3_0.0
   * @return The load progress percentage (0.0 - 100.0)
   */
  float GetLoadProgressPercentage() const;

  /**
   * @brief Gets the page zoom factor.
   *
   * @SINCE_3_0.0
   * @return The page zoom factor
   */
  float GetPageZoomFactor() const;

  /**
   * @brief Gets the text zoom factor.
   *
   * @SINCE_3_0.0
   * @return The text zoom factor
   */
  float GetTextZoomFactor() const;

  /**
   * @brief Gets whether mouse events are enabled.
   *
   * @SINCE_3_0.0
   * @return True if mouse events are enabled
   */
  bool IsMouseEventsEnabled() const;

  /**
   * @brief Gets whether key events are enabled.
   *
   * @SINCE_3_0.0
   * @return True if key events are enabled
   */
  bool IsKeyEventsEnabled() const;

  /**
   * @brief Gets whether video hole is enabled.
   *
   * @SINCE_3_0.0
   * @return True if video hole is enabled
   */
  bool IsVideoHoleEnabled() const;

  /**
   * @brief Gets the currently selected text.
   *
   * @SINCE_3_0.0
   * @return The selected text
   */
  Dali::String GetSelectedText() const;

  /**
   * @brief Gets the scale factor.
   *
   * @SINCE_3_0.0
   * @return The scale factor
   */
  float GetScaleFactor() const;

  /**
   * @brief Gets the browsing profile used by this WebView.
   *
   * WebViews using the same browsing context return profiles that compare equal.
   * The returned handle remains initialized when the backend does not implement
   * a browsing context; its operations then do nothing.
   *
   * @SINCE_3_0.0
   * @return The browsing profile associated with this WebView
   */
  WebProfile GetProfile() const;

  /**
   * @brief Gets the settings associated with this WebView.
   *
   * @SINCE_3_0.0
   * @return The settings for this WebView
   */
  WebSettings GetSettings() const;

public: // Query methods
  /**
   * @brief Checks if forward navigation is possible.
   *
   * @SINCE_3_0.0
   * @return True if can go forward
   */
  bool CanGoForward() const;

  /**
   * @brief Checks if backward navigation is possible.
   *
   * @SINCE_3_0.0
   * @return True if can go back
   */
  bool CanGoBack() const;

  /**
   * @brief Loads HTML string with override current entry.
   *
   * @SINCE_3_0.0
   * @param[in] html The HTML string
   * @param[in] basicUri The base URI
   * @param[in] unreachableUrl The unreachable URL
   * @return True on success
   */
  bool LoadHtmlStringOverrideCurrentEntry(const Dali::String& html, const Dali::String& basicUri, const Dali::String& unreachableUrl);

  /**
   * @brief Loads contents.
   *
   * @SINCE_3_0.0
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
   * @SINCE_3_0.0
   * @return True on success
   */
  bool ReloadWithoutCache();

  /**
   * @brief Adds a custom header.
   *
   * @SINCE_3_0.0
   * @param[in] name The header name
   * @param[in] value The header value
   * @return True on success
   */
  bool AddCustomHeader(const Dali::String& name, const Dali::String& value);

  /**
   * @brief Removes a custom header.
   *
   * @SINCE_3_0.0
   * @param[in] name The header name
   * @return True on success
   */
  bool RemoveCustomHeader(const Dali::String& name);

public: // Actions
  /**
   * @brief Scrolls by the specified delta.
   *
   * @SINCE_3_0.0
   * @param[in] deltaX The horizontal scroll delta
   * @param[in] deltaY The vertical scroll delta
   */
  void ScrollBy(int32_t deltaX, int32_t deltaY);

  /**
   * @brief Scrolls by edge delta.
   *
   * @SINCE_3_0.0
   * @param[in] deltaX The horizontal scroll delta
   * @param[in] deltaY The vertical scroll delta
   * @return True on success
   */
  bool ScrollEdgeBy(int32_t deltaX, int32_t deltaY);

  /**
   * @brief Exits fullscreen mode.
   * @SINCE_3_0.0
   */
  void ExitFullscreen();

  /**
   * @brief Clears all tiles resources.
   * @SINCE_3_0.0
   */
  void ClearAllTilesResources();

  /**
   * @brief Clears the resource cache for this WebView's profile.
   *
   * This is a convenience shortcut for @c GetProfile().ClearCache(). The cache
   * is shared by every WebView using the same profile, so those WebViews are
   * affected as well.
   * @SINCE_3_0.0
   */
  void ClearCache();

  /**
   * @brief Clears all cookies for this WebView's profile.
   *
   * This is a convenience shortcut for
   * @c GetProfile().GetCookieManager().ClearAllCookies(). The cookie store is
   * shared by every WebView using the same profile.
   * @SINCE_3_0.0
   */
  void ClearCookies();

  /**
   * @brief Suspends network loading.
   * @SINCE_3_0.0
   */
  void SuspendNetworkLoading();

  /**
   * @brief Resumes network loading.
   * @SINCE_3_0.0
   */
  void ResumeNetworkLoading();

  /**
   * @brief Sets the scale factor at a point.
   *
   * @SINCE_3_0.0
   * @param[in] scaleFactor The scale factor
   * @param[in] point The point to scale around
   */
  void SetScaleFactor(float scaleFactor, Dali::Vector2 point);

  /**
   * @brief Feeds mouse wheel event.
   *
   * @SINCE_3_0.0
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
   * @SINCE_3_0.0
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
   * @SINCE_3_0.0
   * @param[in] touchEvent The touch event to feed
   * @return True if the event was consumed by the web engine
   */
  bool FeedTouchEvent(const TouchEvent& touchEvent);

public: // Screenshot & Page Info
  /**
   * @brief Gets a screenshot.
   *
   * @SINCE_3_0.0
   * @param[in] viewArea The view area
   * @param[in] scaleFactor The scale factor
   * @return An ImageView containing the screenshot
   */
  Dali::Ui::ImageView GetScreenshot(Dali::BoundsInteger viewArea, float scaleFactor);

  /**
   * @brief Gets a screenshot asynchronously.
   *
   * @SINCE_3_0.0
   * @param[in] viewArea The view area
   * @param[in] scaleFactor The scale factor
   * @param[in] callback The callback to receive the screenshot
   * @return True if the operation started successfully
   */
  bool GetScreenshotAsynchronously(Dali::BoundsInteger viewArea, float scaleFactor, ScreenshotCapturedCallback callback);

  /**
   * @brief Gets the favicon.
   *
   * @SINCE_3_0.0
   * @return An ImageView containing the favicon
   */
  Dali::Ui::ImageView GetFavicon() const;

  /**
   * @brief Gets plain text asynchronously.
   *
   * @SINCE_3_0.0
   * @param[in] callback The callback to receive the plain text
   */
  void GetPlainTextAsynchronously(PlainTextCallback callback);

  /**
   * @brief Checks if video is playing asynchronously.
   *
   * @SINCE_3_0.0
   * @param[in] callback The callback to receive the result
   * @return True if the operation started successfully
   */
  bool CheckVideoPlayingAsynchronously(VideoPlayingCallback callback);

public: // JavaScript
  /**
   * @brief Evaluates JavaScript code.
   *
   * @SINCE_3_0.0
   * @param[in] script The JavaScript code to evaluate
   */
  void EvaluateJavaScript(const Dali::String& script);

  /**
   * @brief Evaluates JavaScript code with callback.
   *
   * @SINCE_3_0.0
   * @param[in] script The JavaScript code to evaluate
   * @param[in] callback The callback to receive the result
   */
  void EvaluateJavaScript(const Dali::String& script, JavaScriptCallback callback);

  /**
   * @brief Adds a JavaScript message handler.
   *
   * @SINCE_3_0.0
   * @param[in] exposedObjectName The exposed object name
   * @param[in] callback The callback to handle messages
   */
  void AddJavaScriptMessageHandler(const Dali::String& exposedObjectName, Callback<void(const Dali::String& message)> callback);

  /**
   * @brief Adds a JavaScript message handler that also receives the exposed object name.
   *
   * For example, when @p exposedObjectName is @c myExposedObjectName, JavaScript
   * invokes the handler as follows:
   * @code
   * window.myExposedObjectName.postMessage(messageBody);
   * @endcode
   * Only one message handler should be registered for a given exposed object
   * name.
   *
   * @SINCE_3_0.0
   * @param[in] exposedObjectName The exposed object name
   * @param[in] callback The callback receiving the exposed object name and message body
   */
  void AddJavaScriptMessageHandler(const Dali::String& exposedObjectName, Callback<void(const Dali::String& exposedObjectName, const Dali::String& message)> callback);

  /**
   * @brief Removes a JavaScript message handler.
   *
   * @SINCE_3_0.0
   * @param[in] exposedObjectName The exposed object name to remove
   */
  void RemoveJavaScriptMessageHandler(const Dali::String& exposedObjectName);

  /**
   * @brief Registers a JavaScript alert callback.
   *
   * @SINCE_3_0.0
   * @param[in] callback The callback for alert dialogs
   */
  void RegisterJavaScriptAlertCallback(JavaScriptAlertCallback callback);

  /**
   * @brief Replies to a JavaScript alert.
   * @SINCE_3_0.0
   */
  void JavaScriptAlertReply();

  /**
   * @brief Registers a JavaScript confirm callback.
   *
   * @SINCE_3_0.0
   * @param[in] callback The callback for confirm dialogs
   */
  void RegisterJavaScriptConfirmCallback(JavaScriptConfirmCallback callback);

  /**
   * @brief Replies to a JavaScript confirm.
   *
   * @SINCE_3_0.0
   * @param[in] confirmed The confirmation result
   */
  void JavaScriptConfirmReply(bool confirmed);

  /**
   * @brief Registers a JavaScript prompt callback.
   *
   * @SINCE_3_0.0
   * @param[in] callback The callback for prompt dialogs
   */
  void RegisterJavaScriptPromptCallback(JavaScriptPromptCallback callback);

  /**
   * @brief Replies to a JavaScript prompt.
   *
   * @SINCE_3_0.0
   * @param[in] result The prompt result
   */
  void JavaScriptPromptReply(const Dali::String& result);

public: // Text search
  /**
   * @brief Searches for and highlights the given text in the page.
   *
   * The number of matches is reported asynchronously through TextFoundSignal().
   *
   * @SINCE_3_0.0
   * @param[in] text The text to find
   * @param[in] options The find options (combine WebViewFindOption values with bitwise OR)
   * @param[in] maxMatchCount The maximum number of matches to highlight
   * @return True if the search was started successfully
   */
  bool FindText(const Dali::String& text, WebViewFindOption options, uint32_t maxMatchCount);

public: // Signals
  /**
   * @brief Signal emitted when page load starts.
   * @SINCE_3_0.0
   */
  PageLoadStartedSignalType& PageLoadStartedSignal();

  /**
   * @brief Signal emitted while page is loading.
   * @SINCE_3_0.0
   */
  PageLoadInProgressSignalType& PageLoadInProgressSignal();

  /**
   * @brief Signal emitted when page load finishes.
   * @SINCE_3_0.0
   */
  PageLoadFinishedSignalType& PageLoadFinishedSignal();

  /**
   * @brief Signal emitted when an error occurs during page load.
   *
   * The error reference is valid only for the duration of the signal callback.
   * Copy the error to retain it after the callback returns.
   * @SINCE_3_0.0
   */
  PageLoadErrorSignalType& PageLoadErrorSignal();

  /**
   * @brief Signal emitted when URL changes.
   * @SINCE_3_0.0
   */
  UrlChangedSignalType& UrlChangedSignal();

  /**
   * @brief Signal emitted when scroll edge is reached.
   * @SINCE_3_0.0
   */
  ScrollEdgeReachedSignalType& ScrollEdgeReachedSignal();

  /**
   * @brief Signal emitted when over-scroll occurs.
   * @SINCE_3_0.0
   */
  OverScrolledSignalType& OverScrolledSignal();

  /**
   * @brief Signal emitted when a frame is rendered.
   * @SINCE_3_0.0
   */
  FrameRenderedSignalType& FrameRenderedSignal();

  /**
   * @brief Signal emitted when entering fullscreen.
   * @SINCE_3_0.0
   */
  FullscreenEnteredSignalType& FullscreenEnteredSignal();

  /**
   * @brief Signal emitted when exiting fullscreen.
   * @SINCE_3_0.0
   */
  FullscreenExitedSignalType& FullscreenExitedSignal();

  /**
   * @brief Signal emitted when text is found.
   * @SINCE_3_0.0
   */
  TextFoundSignalType& TextFoundSignal();

  /**
   * @brief Signal emitted for geolocation permission request.
   * @SINCE_3_0.0
   */
  GeolocationPermissionSignalType& GeolocationPermissionSignal();

  /**
   * @brief Signal emitted when web process crashes.
   * @SINCE_3_0.0
   */
  WebProcessCrashedSignalType& WebProcessCrashedSignal();

public: // Additional APIs
  /**
   * @brief Sets the absolute scroll position.
   *
   * The underlying engine may apply the requested position asynchronously, so
   * an immediate GetScrollPosition() call is not guaranteed to return the
   * requested value.
   *
   * @SINCE_3_0.0
   * @param[in] position The horizontal and vertical scroll offsets
   */
  void SetScrollPosition(const Dali::Vector2& position);

  /**
   * @brief Gets the live back-forward list associated with this WebView.
   *
   * The returned handle remains safe to retain after this WebView is destroyed.
   * Its queries then return empty results.
   *
   * @SINCE_3_0.0
   * @return The back-forward list
   */
  WebBackForwardList GetBackForwardList() const;

  /**
   * @brief Selects the process-wide web engine for WebView instances.
   *
   * Call before SetWebEngineArguments(), WebView::New() or WebProfile::New().
   * Their first use fixes the engine type, defaulting to Chromium if unset,
   * even if argument storage or backend creation fails. Afterwards, selecting
   * a different engine triggers an assertion, even if no engine was started
   * or every view and profile has been destroyed. Selecting the same type is allowed.
   * The deprecated UiConfig web engine setting is ignored.
   *
   * @SINCE_3_0.0
   * @param[in] type The web engine type
   */
  static void SetWebEngineType(WebEngineType type);

  /**
   * @brief Replaces process-wide startup arguments before engine initialization begins.
   *
   * Call on the DALi event thread after UiConfig::Apply(). Profile creation can
   * initialize the engine before any view exists. This call fixes the engine
   * type but does not start it.
   * argv[0] is the program name. argc == 0 clears settings and permits null argv.
   * Failed calls preserve existing arguments; calls after initialization starts
   * return ALREADY_INITIALIZED, even when initialization failed.
   * @SINCE_3_0.0
   * @param[in] argc Argument count, within the native int range
   * @param[in] argv Argument array with argc non-null entries when argc > 0
   * @return The storage result
   */
  static WebEngineArgumentsResult SetWebEngineArguments(uint32_t argc, char** argv);

  /**
   * @brief Creates a view using the specified profile, retained for the view's lifetime.
   * @SINCE_3_0.0
   * @param[in] profile An initialized profile; an empty handle is an assertion error
   * @return The new view, or an empty handle on backend failure; no default fallback
   */
  static WebView New(const WebProfile& profile);

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL WebView(Integration::WebViewImpl& implementation);
  explicit DALI_INTERNAL WebView(Dali::Internal::CustomActor* internal);
  /// @endcond

public:
};

} // namespace Ui
} //namespace DALI_NAMESPACE
