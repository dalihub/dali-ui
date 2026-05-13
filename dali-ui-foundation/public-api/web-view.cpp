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

#include <dali-ui-foundation/integration-api/web-view-impl.h>
#include <dali-ui-foundation/public-api/web-view.h>

namespace Dali
{
namespace Ui
{

namespace
{

Integration::WebViewImpl& GetImpl(WebView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  return static_cast<Integration::WebViewImpl&>(obj.GetImplementation());
}

const Integration::WebViewImpl& GetImpl(const WebView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  return static_cast<const Integration::WebViewImpl&>(obj.GetImplementation());
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// Construction & Destruction
// ---------------------------------------------------------------------------

WebView::WebView()
{
}

WebView::WebView(const WebView& webView)            = default;
WebView::WebView(WebView&& rhs) noexcept            = default;
WebView& WebView::operator=(const WebView& webView) = default;
WebView& WebView::operator=(WebView&& rhs) noexcept = default;

WebView::~WebView()
{
}

// ---------------------------------------------------------------------------
// Static factories
// ---------------------------------------------------------------------------

WebView WebView::New()
{
  Integration::WebViewImplPtr impl = Integration::WebViewImpl::New();
  WebView                     view(*impl);
  impl->Initialize();
  return view;
}

WebView WebView::New(const Dali::String& url)
{
  Integration::WebViewImplPtr impl = Integration::WebViewImpl::New();
  WebView                     view(*impl);
  impl->Initialize();
  if(!url.Empty())
  {
    view.LoadUrl(url);
  }
  return view;
}

WebView WebView::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<WebView, Ui::Integration::WebViewImpl>(handle);
}

// ---------------------------------------------------------------------------
// Internal construction (not for application use)
// ---------------------------------------------------------------------------

WebView::WebView(Integration::WebViewImpl& implementation)
: View(implementation)
{
}

WebView::WebView(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Integration::WebViewImpl>(internal);
}

// ---------------------------------------------------------------------------
// URL & User Agent (Fluent API)
// ---------------------------------------------------------------------------

Dali::String WebView::GetUrl() const
{
  return GetImpl(*this).GetUrl();
}

WebView& WebView::SetUserAgent(const Dali::String& userAgent)
{
  GetImpl(*this).SetUserAgent(userAgent);
  return *this;
}

Dali::String WebView::GetUserAgent() const
{
  return GetImpl(*this).GetUserAgent();
}

// ---------------------------------------------------------------------------
// Page Loading (Fluent API)
// ---------------------------------------------------------------------------

WebView& WebView::LoadUrl(const Dali::String& url)
{
  GetImpl(*this).LoadUrl(url);
  return *this;
}

WebView& WebView::LoadHtmlString(const Dali::String& htmlString)
{
  GetImpl(*this).LoadHtmlString(htmlString);
  return *this;
}

WebView& WebView::Reload()
{
  GetImpl(*this).Reload();
  return *this;
}

WebView& WebView::StopLoading()
{
  GetImpl(*this).StopLoading();
  return *this;
}

WebView& WebView::Suspend()
{
  GetImpl(*this).Suspend();
  return *this;
}

WebView& WebView::Resume()
{
  GetImpl(*this).Resume();
  return *this;
}

// ---------------------------------------------------------------------------
// Page Loading (Non-chainable)
// ---------------------------------------------------------------------------

bool WebView::LoadHtmlStringOverrideCurrentEntry(const Dali::String& html, const Dali::String& basicUri, const Dali::String& unreachableUrl)
{
  return GetImpl(*this).LoadHtmlStringOverrideCurrentEntry(html, basicUri, unreachableUrl);
}

bool WebView::LoadContents(const int8_t* contents, uint32_t contentSize, const Dali::String& mimeType, const Dali::String& encoding, const Dali::String& baseUri)
{
  return GetImpl(*this).LoadContents(contents, contentSize, mimeType, encoding, baseUri);
}

bool WebView::ReloadWithoutCache()
{
  return GetImpl(*this).ReloadWithoutCache();
}

float WebView::GetLoadProgressPercentage() const
{
  return GetImpl(*this).GetLoadProgressPercentage();
}

// ---------------------------------------------------------------------------
// Custom Headers
// ---------------------------------------------------------------------------

bool WebView::AddCustomHeader(const Dali::String& name, const Dali::String& value)
{
  return GetImpl(*this).AddCustomHeader(name, value);
}

bool WebView::RemoveCustomHeader(const Dali::String& name)
{
  return GetImpl(*this).RemoveCustomHeader(name);
}

// ---------------------------------------------------------------------------
// Scroll (Getters)
// ---------------------------------------------------------------------------

Dali::Vector2 WebView::GetScrollPosition() const
{
  return GetImpl(*this).GetScrollPosition();
}

Dali::Vector2 WebView::GetScrollSize() const
{
  return GetImpl(*this).GetScrollSize();
}

Dali::Vector2 WebView::GetContentSize() const
{
  return GetImpl(*this).GetContentSize();
}

// ---------------------------------------------------------------------------
// Scroll (Actions)
// ---------------------------------------------------------------------------

void WebView::ScrollBy(int32_t deltaX, int32_t deltaY)
{
  GetImpl(*this).ScrollBy(deltaX, deltaY);
}

bool WebView::ScrollEdgeBy(int32_t deltaX, int32_t deltaY)
{
  return GetImpl(*this).ScrollEdgeBy(deltaX, deltaY);
}

// ---------------------------------------------------------------------------
// Navigation (Fluent API)
// ---------------------------------------------------------------------------

WebView& WebView::GoForward()
{
  GetImpl(*this).GoForward();
  return *this;
}

WebView& WebView::GoBack()
{
  GetImpl(*this).GoBack();
  return *this;
}

WebView& WebView::ClearHistory()
{
  GetImpl(*this).ClearHistory();
  return *this;
}

// ---------------------------------------------------------------------------
// Navigation (Non-chainable)
// ---------------------------------------------------------------------------

bool WebView::CanGoForward()
{
  return GetImpl(*this).CanGoForward();
}

bool WebView::CanGoBack()
{
  return GetImpl(*this).CanGoBack();
}

void WebView::ExitFullscreen()
{
  GetImpl(*this).ExitFullscreen();
}

void WebView::ClearAllTilesResources()
{
  GetImpl(*this).ClearAllTilesResources();
}

void WebView::SuspendNetworkLoading()
{
  GetImpl(*this).SuspendNetworkLoading();
}

void WebView::ResumeNetworkLoading()
{
  GetImpl(*this).ResumeNetworkLoading();
}

// ---------------------------------------------------------------------------
// Zoom (Fluent API)
// ---------------------------------------------------------------------------

WebView& WebView::SetPageZoomFactor(float zoomFactor)
{
  GetImpl(*this).SetPageZoomFactor(zoomFactor);
  return *this;
}

WebView& WebView::SetTextZoomFactor(float zoomFactor)
{
  GetImpl(*this).SetTextZoomFactor(zoomFactor);
  return *this;
}

// ---------------------------------------------------------------------------
// Zoom (Getters)
// ---------------------------------------------------------------------------

float WebView::GetPageZoomFactor() const
{
  return GetImpl(*this).GetPageZoomFactor();
}

float WebView::GetTextZoomFactor() const
{
  return GetImpl(*this).GetTextZoomFactor();
}

// ---------------------------------------------------------------------------
// Scale
// ---------------------------------------------------------------------------

void WebView::SetScaleFactor(float scaleFactor, Dali::Vector2 point)
{
  GetImpl(*this).SetScaleFactor(scaleFactor, point);
}

float WebView::GetScaleFactor() const
{
  return GetImpl(*this).GetScaleFactor();
}

// ---------------------------------------------------------------------------
// Screenshot & Page Info
// ---------------------------------------------------------------------------

Dali::Ui::ImageView WebView::GetScreenshot(Dali::BoundsInteger viewArea, float scaleFactor)
{
  return GetImpl(*this).GetScreenshot(viewArea, scaleFactor);
}

bool WebView::GetScreenshotAsynchronously(Dali::BoundsInteger viewArea, float scaleFactor, ScreenshotCapturedCallback callback)
{
  return GetImpl(*this).GetScreenshotAsynchronously(viewArea, scaleFactor, std::move(callback));
}

Dali::String WebView::GetTitle() const
{
  return GetImpl(*this).GetTitle();
}

Dali::Ui::ImageView WebView::GetFavicon() const
{
  return GetImpl(*this).GetFavicon();
}

Dali::String WebView::GetSelectedText() const
{
  return GetImpl(*this).GetSelectedText();
}

void WebView::GetPlainTextAsynchronously(PlainTextCallback callback)
{
  GetImpl(*this).GetPlainTextAsynchronously(std::move(callback));
}

// ---------------------------------------------------------------------------
// Document Appearance (Fluent API)
// ---------------------------------------------------------------------------

WebView& WebView::SetDocumentBackgroundColor(const Dali::Vector4& color)
{
  GetImpl(*this).SetDocumentBackgroundColor(color);
  return *this;
}

WebView& WebView::SetTilesClearedWhenHidden(bool cleared)
{
  GetImpl(*this).SetTilesClearedWhenHidden(cleared);
  return *this;
}

WebView& WebView::SetTileCoverAreaMultiplier(float multiplier)
{
  GetImpl(*this).SetTileCoverAreaMultiplier(multiplier);
  return *this;
}

WebView& WebView::SetCursorEnabledByClient(bool enabled)
{
  GetImpl(*this).SetCursorEnabledByClient(enabled);
  return *this;
}

// ---------------------------------------------------------------------------
// Input & Video (Fluent API)
// ---------------------------------------------------------------------------

WebView& WebView::SetMouseEventsEnabled(bool enabled)
{
  GetImpl(*this).SetMouseEventsEnabled(enabled);
  return *this;
}

WebView& WebView::SetKeyEventsEnabled(bool enabled)
{
  GetImpl(*this).SetKeyEventsEnabled(enabled);
  return *this;
}

WebView& WebView::SetVideoHoleEnabled(bool enabled)
{
  GetImpl(*this).SetVideoHoleEnabled(enabled);
  return *this;
}

// ---------------------------------------------------------------------------
// Input & Video (Getters)
// ---------------------------------------------------------------------------

bool WebView::GetMouseEventsEnabled() const
{
  return GetImpl(*this).GetMouseEventsEnabled();
}

bool WebView::GetKeyEventsEnabled() const
{
  return GetImpl(*this).GetKeyEventsEnabled();
}

bool WebView::GetVideoHoleEnabled() const
{
  return GetImpl(*this).GetVideoHoleEnabled();
}

// ---------------------------------------------------------------------------
// Input & Video (Actions)
// ---------------------------------------------------------------------------

void WebView::FeedMouseWheel(bool yDirection, int step, int x, int y)
{
  GetImpl(*this).FeedMouseWheel(yDirection, step, x, y);
}

bool WebView::FeedKeyEvent(const KeyEvent& keyEvent)
{
  return GetImpl(*this).FeedKeyEvent(keyEvent);
}

bool WebView::FeedTouchEvent(const TouchEvent& touchEvent)
{
  return GetImpl(*this).FeedTouchEvent(touchEvent);
}

bool WebView::CheckVideoPlayingAsynchronously(VideoPlayingCallback callback)
{
  return GetImpl(*this).CheckVideoPlayingAsynchronously(std::move(callback));
}

// ---------------------------------------------------------------------------
// JavaScript
// ---------------------------------------------------------------------------

void WebView::EvaluateJavaScript(const Dali::String& script)
{
  GetImpl(*this).EvaluateJavaScript(script);
}

void WebView::EvaluateJavaScript(const Dali::String& script, JavaScriptCallback callback)
{
  GetImpl(*this).EvaluateJavaScript(script, std::move(callback));
}

void WebView::AddJavaScriptMessageHandler(const Dali::String& exposedObjectName, JavaScriptCallback callback)
{
  GetImpl(*this).AddJavaScriptMessageHandler(exposedObjectName, std::move(callback));
}

void WebView::RegisterJavaScriptAlertCallback(JavaScriptAlertCallback callback)
{
  GetImpl(*this).RegisterJavaScriptAlertCallback(std::move(callback));
}

void WebView::JavaScriptAlertReply()
{
  GetImpl(*this).JavaScriptAlertReply();
}

void WebView::RegisterJavaScriptConfirmCallback(JavaScriptConfirmCallback callback)
{
  GetImpl(*this).RegisterJavaScriptConfirmCallback(std::move(callback));
}

void WebView::JavaScriptConfirmReply(bool confirmed)
{
  GetImpl(*this).JavaScriptConfirmReply(confirmed);
}

void WebView::RegisterJavaScriptPromptCallback(JavaScriptPromptCallback callback)
{
  GetImpl(*this).RegisterJavaScriptPromptCallback(std::move(callback));
}

void WebView::JavaScriptPromptReply(const Dali::String& result)
{
  GetImpl(*this).JavaScriptPromptReply(result);
}

// ---------------------------------------------------------------------------
// Signals - Page Loading
// ---------------------------------------------------------------------------

WebView::PageLoadStartedSignalType& WebView::PageLoadStartedSignal()
{
  return GetImpl(*this).mPageLoadStartedSignal;
}

WebView::PageLoadInProgressSignalType& WebView::PageLoadInProgressSignal()
{
  return GetImpl(*this).mPageLoadInProgressSignal;
}

WebView::PageLoadFinishedSignalType& WebView::PageLoadFinishedSignal()
{
  return GetImpl(*this).mPageLoadFinishedSignal;
}

// ---------------------------------------------------------------------------
// Signals - Navigation
// ---------------------------------------------------------------------------

WebView::UrlChangedSignalType& WebView::UrlChangedSignal()
{
  return GetImpl(*this).mUrlChangedSignal;
}

// ---------------------------------------------------------------------------
// Signals - Scroll
// ---------------------------------------------------------------------------

WebView::ScrollEdgeReachedSignalType& WebView::ScrollEdgeReachedSignal()
{
  return GetImpl(*this).mScrollEdgeReachedSignal;
}

WebView::OverScrolledSignalType& WebView::OverScrolledSignal()
{
  return GetImpl(*this).mOverScrolledSignal;
}

// ---------------------------------------------------------------------------
// Signals - Rendering
// ---------------------------------------------------------------------------

WebView::FrameRenderedSignalType& WebView::FrameRenderedSignal()
{
  return GetImpl(*this).mFrameRenderedSignal;
}

// ---------------------------------------------------------------------------
// Signals - Fullscreen
// ---------------------------------------------------------------------------

WebView::FullscreenEnteredSignalType& WebView::FullscreenEnteredSignal()
{
  return GetImpl(*this).mFullscreenEnteredSignal;
}

WebView::FullscreenExitedSignalType& WebView::FullscreenExitedSignal()
{
  return GetImpl(*this).mFullscreenExitedSignal;
}

// ---------------------------------------------------------------------------
// Signals - Text Search & Geolocation
// ---------------------------------------------------------------------------

WebView::TextFoundSignalType& WebView::TextFoundSignal()
{
  return GetImpl(*this).mTextFoundSignal;
}

WebView::GeolocationPermissionSignalType& WebView::GeolocationPermissionSignal()
{
  return GetImpl(*this).mGeolocationPermissionSignal;
}

// ---------------------------------------------------------------------------
// Signals - Process Events
// ---------------------------------------------------------------------------

WebView::WebProcessCrashedSignalType& WebView::WebProcessCrashedSignal()
{
  return GetImpl(*this).mWebProcessCrashedSignal;
}

} // namespace Ui
} // namespace Dali
