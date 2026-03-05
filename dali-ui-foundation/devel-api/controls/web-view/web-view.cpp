/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali-ui-foundation/devel-api/controls/web-view/web-view.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/controls/web-view/web-view-impl.h>
#include <dali-ui-foundation/public-api/controls/image-view/image-view.h>

namespace Dali
{
namespace UI
{
WebView::WebView()
{
}

WebView::WebView(const WebView& WebView)
  : Control(WebView)
{
}

WebView& WebView::operator=(const WebView& view)
{
  if (&view != this)
  {
    Control::operator=(view);
  }

  return *this;
}

WebView::~WebView()
{
}

WebView WebView::New()
{
  return Internal::WebView::New();
}

WebView WebView::New(const std::string& locale, const std::string& timezoneId)
{
  return Internal::WebView::New(locale, timezoneId);
}

WebView WebView::New(uint32_t argc, char** argv)
{
  return Internal::WebView::New(argc, argv, -1);
}

WebView WebView::New(uint32_t argc, char** argv, int32_t type)
{
  return Internal::WebView::New(argc, argv, type);
}

UI::WebView WebView::FindWebView(Dali::WebEnginePlugin* plugin)
{
  return Internal::WebView::FindWebView(plugin);
}

Dali::WebEngineContext* WebView::GetContext()
{
  return Internal::WebView::GetContext();
}

Dali::WebEngineCookieManager* WebView::GetCookieManager()
{
  return Internal::WebView::GetCookieManager();
}

WebView WebView::DownCast(BaseHandle handle)
{
  return Control::DownCast<WebView, Internal::WebView>(handle);
}

void WebView::ChangeOrientation(int orientation)
{
  return Dali::UI::GetImpl(*this).ChangeOrientation(orientation);
}

Dali::UI::WebSettings* WebView::GetSettings() const
{
  return Dali::UI::GetImpl(*this).GetSettings();
}

Dali::UI::WebBackForwardList* WebView::GetBackForwardList() const
{
  return Dali::UI::GetImpl(*this).GetBackForwardList();
}

Dali::WebEnginePlugin* WebView::GetPlugin() const
{
  return Dali::UI::GetImpl(*this).GetPlugin();
}

Dali::UI::ImageView WebView::GetFavicon() const
{
  return Dali::UI::GetImpl(*this).GetFavicon();
}

void WebView::LoadUrl(const std::string& url)
{
  Dali::UI::GetImpl(*this).LoadUrl(url);
}

void WebView::LoadHtmlString(const std::string& htmlString)
{
  Dali::UI::GetImpl(*this).LoadHtmlString(htmlString);
}

bool WebView::LoadHtmlStringOverrideCurrentEntry(const std::string& html, const std::string& basicUri,
                                                 const std::string& unreachableUrl)
{
  return Dali::UI::GetImpl(*this).LoadHtmlStringOverrideCurrentEntry(html, basicUri, unreachableUrl);
}

bool WebView::LoadContents(const int8_t* contents, uint32_t contentSize, const std::string& mimeType,
                           const std::string& encoding, const std::string& baseUri)
{
  return Dali::UI::GetImpl(*this).LoadContents(contents, contentSize, mimeType, encoding, baseUri);
}

void WebView::Reload()
{
  Dali::UI::GetImpl(*this).Reload();
}

bool WebView::ReloadWithoutCache()
{
  return Dali::UI::GetImpl(*this).ReloadWithoutCache();
}

void WebView::StopLoading()
{
  Dali::UI::GetImpl(*this).StopLoading();
}

void WebView::Suspend()
{
  Dali::UI::GetImpl(*this).Suspend();
}

void WebView::Resume()
{
  Dali::UI::GetImpl(*this).Resume();
}

void WebView::SuspendNetworkLoading()
{
  Dali::UI::GetImpl(*this).SuspendNetworkLoading();
}

void WebView::ResumeNetworkLoading()
{
  Dali::UI::GetImpl(*this).ResumeNetworkLoading();
}

bool WebView::AddCustomHeader(const std::string& name, const std::string& value)
{
  return Dali::UI::GetImpl(*this).AddCustomHeader(name, value);
}

bool WebView::RemoveCustomHeader(const std::string& name)
{
  return Dali::UI::GetImpl(*this).RemoveCustomHeader(name);
}

uint32_t WebView::StartInspectorServer(uint32_t port)
{
  return Dali::UI::GetImpl(*this).StartInspectorServer(port);
}

bool WebView::StopInspectorServer()
{
  return Dali::UI::GetImpl(*this).StopInspectorServer();
}

bool WebView::SetImePositionAndAlignment(Dali::Vector2 position, int alignment)
{
  return Dali::UI::GetImpl(*this).SetImePositionAndAlignment(position, alignment);
}

void WebView::SetCursorThemeName(const std::string themeName)
{
  Dali::UI::GetImpl(*this).SetCursorThemeName(themeName);
}

void WebView::ScrollBy(int32_t deltaX, int32_t deltaY)
{
  Dali::UI::GetImpl(*this).ScrollBy(deltaX, deltaY);
}

bool WebView::ScrollEdgeBy(int32_t deltaX, int32_t deltaY)
{
  return Dali::UI::GetImpl(*this).ScrollEdgeBy(deltaX, deltaY);
}

bool WebView::CanGoForward()
{
  return Dali::UI::GetImpl(*this).CanGoForward();
}

void WebView::GoForward()
{
  Dali::UI::GetImpl(*this).GoForward();
}

bool WebView::CanGoBack()
{
  return Dali::UI::GetImpl(*this).CanGoBack();
}

void WebView::GoBack()
{
  Dali::UI::GetImpl(*this).GoBack();
}

void WebView::EvaluateJavaScript(const std::string& script,
                                 Dali::WebEnginePlugin::JavaScriptMessageHandlerCallback resultHandler)
{
  Dali::UI::GetImpl(*this).EvaluateJavaScript(script, resultHandler);
}

void WebView::EvaluateJavaScript(const std::string& script)
{
  Dali::UI::GetImpl(*this).EvaluateJavaScript(script, nullptr);
}

void WebView::AddJavaScriptMessageHandler(const std::string& exposedObjectName,
                                          Dali::WebEnginePlugin::JavaScriptMessageHandlerCallback handler)
{
  Dali::UI::GetImpl(*this).AddJavaScriptMessageHandler(exposedObjectName, handler);
}

void WebView::AddJavaScriptEntireMessageHandler(const std::string& exposedObjectName,
                                                Dali::WebEnginePlugin::JavaScriptEntireMessageHandlerCallback handler)
{
  Dali::UI::GetImpl(*this).AddJavaScriptEntireMessageHandler(exposedObjectName, handler);
}

void WebView::RegisterJavaScriptAlertCallback(Dali::WebEnginePlugin::JavaScriptAlertCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterJavaScriptAlertCallback(callback);
}

void WebView::JavaScriptAlertReply()
{
  Dali::UI::GetImpl(*this).JavaScriptAlertReply();
}

void WebView::RegisterJavaScriptConfirmCallback(Dali::WebEnginePlugin::JavaScriptConfirmCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterJavaScriptConfirmCallback(callback);
}

void WebView::JavaScriptConfirmReply(bool confirmed)
{
  Dali::UI::GetImpl(*this).JavaScriptConfirmReply(confirmed);
}

void WebView::RegisterJavaScriptPromptCallback(Dali::WebEnginePlugin::JavaScriptPromptCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterJavaScriptPromptCallback(callback);
}

void WebView::JavaScriptPromptReply(const std::string& result)
{
  Dali::UI::GetImpl(*this).JavaScriptPromptReply(result);
}

std::unique_ptr<Dali::WebEngineHitTest> WebView::CreateHitTest(int32_t x, int32_t y,
                                                               Dali::WebEngineHitTest::HitTestMode mode)
{
  return Dali::UI::GetImpl(*this).CreateHitTest(x, y, mode);
}

bool WebView::CreateHitTestAsynchronously(int32_t x, int32_t y, Dali::WebEngineHitTest::HitTestMode mode,
                                          Dali::WebEnginePlugin::WebEngineHitTestCreatedCallback callback)
{
  return Dali::UI::GetImpl(*this).CreateHitTestAsynchronously(x, y, mode, callback);
}

void WebView::ExitFullscreen()
{
  Dali::UI::GetImpl(*this).ExitFullscreen();
}

void WebView::ClearHistory()
{
  Dali::UI::GetImpl(*this).ClearHistory();
}

void WebView::ClearAllTilesResources()
{
  Dali::UI::GetImpl(*this).ClearAllTilesResources();
}

void WebView::SetScaleFactor(float scaleFactor, Dali::Vector2 point)
{
  Dali::UI::GetImpl(*this).SetScaleFactor(scaleFactor, point);
}

float WebView::GetScaleFactor() const
{
  return Dali::UI::GetImpl(*this).GetScaleFactor();
}

void WebView::ActivateAccessibility(bool activated)
{
  Dali::UI::GetImpl(*this).ActivateAccessibility(activated);
}

bool WebView::HighlightText(const std::string& text, Dali::WebEnginePlugin::FindOption options, uint32_t maxMatchCount)
{
  return Dali::UI::GetImpl(*this).HighlightText(text, options, maxMatchCount);
}

void WebView::AddDynamicCertificatePath(const std::string& host, const std::string& certPath)
{
  Dali::UI::GetImpl(*this).AddDynamicCertificatePath(host, certPath);
}

Dali::UI::ImageView WebView::GetScreenshot(Dali::Rect<int32_t> viewArea, float scaleFactor)
{
  return Dali::UI::GetImpl(*this).GetScreenshot(viewArea, scaleFactor);
}

bool WebView::GetScreenshotAsynchronously(Dali::Rect<int32_t> viewArea, float scaleFactor,
                                          Dali::UI::WebView::WebViewScreenshotCapturedCallback callback)
{
  return Dali::UI::GetImpl(*this).GetScreenshotAsynchronously(viewArea, scaleFactor, callback);
}

bool WebView::CheckVideoPlayingAsynchronously(Dali::WebEnginePlugin::VideoPlayingCallback callback)
{
  return Dali::UI::GetImpl(*this).CheckVideoPlayingAsynchronously(callback);
}

void WebView::RegisterGeolocationPermissionCallback(Dali::WebEnginePlugin::GeolocationPermissionCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterGeolocationPermissionCallback(callback);
}

void WebView::SetTtsFocus(bool focused)
{
  Dali::UI::GetImpl(*this).SetTtsFocus(focused);
}

void WebView::RegisterPageLoadStartedCallback(Dali::WebEnginePlugin::WebEnginePageLoadCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterPageLoadStartedCallback(callback);
}

void WebView::RegisterPageLoadInProgressCallback(Dali::WebEnginePlugin::WebEnginePageLoadCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterPageLoadInProgressCallback(callback);
}

void WebView::RegisterPageLoadFinishedCallback(Dali::WebEnginePlugin::WebEnginePageLoadCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterPageLoadFinishedCallback(callback);
}

void WebView::RegisterPageLoadErrorCallback(Dali::WebEnginePlugin::WebEnginePageLoadErrorCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterPageLoadErrorCallback(callback);
}

void WebView::RegisterScrollEdgeReachedCallback(Dali::WebEnginePlugin::WebEngineScrollEdgeReachedCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterScrollEdgeReachedCallback(callback);
}

void WebView::RegisterOverScrolledCallback(Dali::WebEnginePlugin::WebEngineOverScrolledCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterOverScrolledCallback(callback);
}

void WebView::RegisterUrlChangedCallback(Dali::WebEnginePlugin::WebEngineUrlChangedCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterUrlChangedCallback(callback);
}

void WebView::RegisterFormRepostDecidedCallback(Dali::WebEnginePlugin::WebEngineFormRepostDecidedCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterFormRepostDecidedCallback(callback);
}

void WebView::RegisterFrameRenderedCallback(Dali::WebEnginePlugin::WebEngineFrameRenderedCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterFrameRenderedCallback(callback);
}

void WebView::RegisterConsoleMessageReceivedCallback(
    Dali::WebEnginePlugin::WebEngineConsoleMessageReceivedCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterConsoleMessageReceivedCallback(callback);
}

void WebView::RegisterResponsePolicyDecidedCallback(
    Dali::WebEnginePlugin::WebEngineResponsePolicyDecidedCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterResponsePolicyDecidedCallback(callback);
}

void WebView::RegisterNavigationPolicyDecidedCallback(
    Dali::WebEnginePlugin::WebEngineNavigationPolicyDecidedCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterNavigationPolicyDecidedCallback(callback);
}

void WebView::RegisterNewWindowPolicyDecidedCallback(
    Dali::WebEnginePlugin::WebEngineNewWindowPolicyDecidedCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterNewWindowPolicyDecidedCallback(callback);
}

void WebView::RegisterNewWindowCreatedCallback(Dali::WebEnginePlugin::WebEngineNewWindowCreatedCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterNewWindowCreatedCallback(callback);
}

void WebView::RegisterCertificateConfirmedCallback(Dali::WebEnginePlugin::WebEngineCertificateCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterCertificateConfirmedCallback(callback);
}

void WebView::RegisterSslCertificateChangedCallback(Dali::WebEnginePlugin::WebEngineCertificateCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterSslCertificateChangedCallback(callback);
}

void WebView::RegisterHttpAuthHandlerCallback(Dali::WebEnginePlugin::WebEngineHttpAuthHandlerCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterHttpAuthHandlerCallback(callback);
}

void WebView::RegisterContextMenuShownCallback(Dali::WebEnginePlugin::WebEngineContextMenuShownCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterContextMenuShownCallback(callback);
}

void WebView::RegisterContextMenuHiddenCallback(Dali::WebEnginePlugin::WebEngineContextMenuHiddenCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterContextMenuHiddenCallback(callback);
}

void WebView::RegisterFullscreenEnteredCallback(Dali::WebEnginePlugin::WebEngineFullscreenEnteredCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterFullscreenEnteredCallback(callback);
}

void WebView::RegisterFullscreenExitedCallback(Dali::WebEnginePlugin::WebEngineFullscreenExitedCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterFullscreenExitedCallback(callback);
}

void WebView::RegisterTextFoundCallback(Dali::WebEnginePlugin::WebEngineTextFoundCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterTextFoundCallback(callback);
}

void WebView::GetPlainTextAsynchronously(Dali::WebEnginePlugin::PlainTextReceivedCallback callback)
{
  Dali::UI::GetImpl(*this).GetPlainTextAsynchronously(callback);
}

void WebView::WebAuthenticationCancel()
{
  Dali::UI::GetImpl(*this).WebAuthenticationCancel();
}

void WebView::RegisterWebAuthDisplayQRCallback(Dali::WebEnginePlugin::WebEngineWebAuthDisplayQRCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterWebAuthDisplayQRCallback(callback);
}

void WebView::RegisterWebAuthResponseCallback(Dali::WebEnginePlugin::WebEngineWebAuthResponseCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterWebAuthResponseCallback(callback);
}

void WebView::RegisterFileChooserRequestedCallback(
    Dali::WebEnginePlugin::WebEngineFileChooserRequestedCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterFileChooserRequestedCallback(callback);
}

void WebView::RegisterWebProcessCrashedCallback(Dali::WebEnginePlugin::WebEngineWebProcessCrashedCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterWebProcessCrashedCallback(callback);
}

void WebView::RegisterUserMediaPermissionRequestCallback(
    Dali::WebEnginePlugin::WebEngineUserMediaPermissionRequestCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterUserMediaPermissionRequestCallback(callback);
}

void WebView::RegisterDeviceConnectionChangedCallback(
    Dali::WebEnginePlugin::WebEngineDeviceConnectionChangedCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterDeviceConnectionChangedCallback(callback);
}

void WebView::RegisterDeviceListGetCallback(Dali::WebEnginePlugin::WebEngineDeviceListGetCallback callback)
{
  Dali::UI::GetImpl(*this).RegisterDeviceListGetCallback(callback);
}

void WebView::FeedMouseWheel(bool yDirection, int step, int x, int y)
{
  Dali::UI::GetImpl(*this).FeedMouseWheel(yDirection, step, x, y);
}

void WebView::SetVideoHole(bool enabled, bool isWaylandWindow)
{
  Dali::UI::GetImpl(*this).SetVideoHole(enabled, isWaylandWindow);
}

WebView::WebView(Internal::WebView& implementation)
  : Control(implementation)
{
}

WebView::WebView(Dali::Internal::CustomActor* internal)
  : Control(internal)
{
  VerifyCustomActorPointer<Internal::WebView>(internal);
}

} // namespace UI

} // namespace Dali
