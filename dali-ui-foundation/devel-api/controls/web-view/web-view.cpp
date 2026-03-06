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
namespace Ui
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

Ui::WebView WebView::FindWebView(Dali::WebEnginePlugin* plugin)
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
  return Dali::Ui::GetImpl(*this).ChangeOrientation(orientation);
}

Dali::Ui::WebSettings* WebView::GetSettings() const
{
  return Dali::Ui::GetImpl(*this).GetSettings();
}

Dali::Ui::WebBackForwardList* WebView::GetBackForwardList() const
{
  return Dali::Ui::GetImpl(*this).GetBackForwardList();
}

Dali::WebEnginePlugin* WebView::GetPlugin() const
{
  return Dali::Ui::GetImpl(*this).GetPlugin();
}

Dali::Ui::ImageView WebView::GetFavicon() const
{
  return Dali::Ui::GetImpl(*this).GetFavicon();
}

void WebView::LoadUrl(const std::string& url)
{
  Dali::Ui::GetImpl(*this).LoadUrl(url);
}

void WebView::LoadHtmlString(const std::string& htmlString)
{
  Dali::Ui::GetImpl(*this).LoadHtmlString(htmlString);
}

bool WebView::LoadHtmlStringOverrideCurrentEntry(const std::string& html, const std::string& basicUri,
                                                 const std::string& unreachableUrl)
{
  return Dali::Ui::GetImpl(*this).LoadHtmlStringOverrideCurrentEntry(html, basicUri, unreachableUrl);
}

bool WebView::LoadContents(const int8_t* contents, uint32_t contentSize, const std::string& mimeType,
                           const std::string& encoding, const std::string& baseUri)
{
  return Dali::Ui::GetImpl(*this).LoadContents(contents, contentSize, mimeType, encoding, baseUri);
}

void WebView::Reload()
{
  Dali::Ui::GetImpl(*this).Reload();
}

bool WebView::ReloadWithoutCache()
{
  return Dali::Ui::GetImpl(*this).ReloadWithoutCache();
}

void WebView::StopLoading()
{
  Dali::Ui::GetImpl(*this).StopLoading();
}

void WebView::Suspend()
{
  Dali::Ui::GetImpl(*this).Suspend();
}

void WebView::Resume()
{
  Dali::Ui::GetImpl(*this).Resume();
}

void WebView::SuspendNetworkLoading()
{
  Dali::Ui::GetImpl(*this).SuspendNetworkLoading();
}

void WebView::ResumeNetworkLoading()
{
  Dali::Ui::GetImpl(*this).ResumeNetworkLoading();
}

bool WebView::AddCustomHeader(const std::string& name, const std::string& value)
{
  return Dali::Ui::GetImpl(*this).AddCustomHeader(name, value);
}

bool WebView::RemoveCustomHeader(const std::string& name)
{
  return Dali::Ui::GetImpl(*this).RemoveCustomHeader(name);
}

uint32_t WebView::StartInspectorServer(uint32_t port)
{
  return Dali::Ui::GetImpl(*this).StartInspectorServer(port);
}

bool WebView::StopInspectorServer()
{
  return Dali::Ui::GetImpl(*this).StopInspectorServer();
}

bool WebView::SetImePositionAndAlignment(Dali::Vector2 position, int alignment)
{
  return Dali::Ui::GetImpl(*this).SetImePositionAndAlignment(position, alignment);
}

void WebView::SetCursorThemeName(const std::string themeName)
{
  Dali::Ui::GetImpl(*this).SetCursorThemeName(themeName);
}

void WebView::ScrollBy(int32_t deltaX, int32_t deltaY)
{
  Dali::Ui::GetImpl(*this).ScrollBy(deltaX, deltaY);
}

bool WebView::ScrollEdgeBy(int32_t deltaX, int32_t deltaY)
{
  return Dali::Ui::GetImpl(*this).ScrollEdgeBy(deltaX, deltaY);
}

bool WebView::CanGoForward()
{
  return Dali::Ui::GetImpl(*this).CanGoForward();
}

void WebView::GoForward()
{
  Dali::Ui::GetImpl(*this).GoForward();
}

bool WebView::CanGoBack()
{
  return Dali::Ui::GetImpl(*this).CanGoBack();
}

void WebView::GoBack()
{
  Dali::Ui::GetImpl(*this).GoBack();
}

void WebView::EvaluateJavaScript(const std::string& script,
                                 Dali::WebEnginePlugin::JavaScriptMessageHandlerCallback resultHandler)
{
  Dali::Ui::GetImpl(*this).EvaluateJavaScript(script, resultHandler);
}

void WebView::EvaluateJavaScript(const std::string& script)
{
  Dali::Ui::GetImpl(*this).EvaluateJavaScript(script, nullptr);
}

void WebView::AddJavaScriptMessageHandler(const std::string& exposedObjectName,
                                          Dali::WebEnginePlugin::JavaScriptMessageHandlerCallback handler)
{
  Dali::Ui::GetImpl(*this).AddJavaScriptMessageHandler(exposedObjectName, handler);
}

void WebView::AddJavaScriptEntireMessageHandler(const std::string& exposedObjectName,
                                                Dali::WebEnginePlugin::JavaScriptEntireMessageHandlerCallback handler)
{
  Dali::Ui::GetImpl(*this).AddJavaScriptEntireMessageHandler(exposedObjectName, handler);
}

void WebView::RegisterJavaScriptAlertCallback(Dali::WebEnginePlugin::JavaScriptAlertCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterJavaScriptAlertCallback(callback);
}

void WebView::JavaScriptAlertReply()
{
  Dali::Ui::GetImpl(*this).JavaScriptAlertReply();
}

void WebView::RegisterJavaScriptConfirmCallback(Dali::WebEnginePlugin::JavaScriptConfirmCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterJavaScriptConfirmCallback(callback);
}

void WebView::JavaScriptConfirmReply(bool confirmed)
{
  Dali::Ui::GetImpl(*this).JavaScriptConfirmReply(confirmed);
}

void WebView::RegisterJavaScriptPromptCallback(Dali::WebEnginePlugin::JavaScriptPromptCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterJavaScriptPromptCallback(callback);
}

void WebView::JavaScriptPromptReply(const std::string& result)
{
  Dali::Ui::GetImpl(*this).JavaScriptPromptReply(result);
}

std::unique_ptr<Dali::WebEngineHitTest> WebView::CreateHitTest(int32_t x, int32_t y,
                                                               Dali::WebEngineHitTest::HitTestMode mode)
{
  return Dali::Ui::GetImpl(*this).CreateHitTest(x, y, mode);
}

bool WebView::CreateHitTestAsynchronously(int32_t x, int32_t y, Dali::WebEngineHitTest::HitTestMode mode,
                                          Dali::WebEnginePlugin::WebEngineHitTestCreatedCallback callback)
{
  return Dali::Ui::GetImpl(*this).CreateHitTestAsynchronously(x, y, mode, callback);
}

void WebView::ExitFullscreen()
{
  Dali::Ui::GetImpl(*this).ExitFullscreen();
}

void WebView::ClearHistory()
{
  Dali::Ui::GetImpl(*this).ClearHistory();
}

void WebView::ClearAllTilesResources()
{
  Dali::Ui::GetImpl(*this).ClearAllTilesResources();
}

void WebView::SetScaleFactor(float scaleFactor, Dali::Vector2 point)
{
  Dali::Ui::GetImpl(*this).SetScaleFactor(scaleFactor, point);
}

float WebView::GetScaleFactor() const
{
  return Dali::Ui::GetImpl(*this).GetScaleFactor();
}

void WebView::ActivateAccessibility(bool activated)
{
  Dali::Ui::GetImpl(*this).ActivateAccessibility(activated);
}

bool WebView::HighlightText(const std::string& text, Dali::WebEnginePlugin::FindOption options, uint32_t maxMatchCount)
{
  return Dali::Ui::GetImpl(*this).HighlightText(text, options, maxMatchCount);
}

void WebView::AddDynamicCertificatePath(const std::string& host, const std::string& certPath)
{
  Dali::Ui::GetImpl(*this).AddDynamicCertificatePath(host, certPath);
}

Dali::Ui::ImageView WebView::GetScreenshot(Dali::Rect<int32_t> viewArea, float scaleFactor)
{
  return Dali::Ui::GetImpl(*this).GetScreenshot(viewArea, scaleFactor);
}

bool WebView::GetScreenshotAsynchronously(Dali::Rect<int32_t> viewArea, float scaleFactor,
                                          Dali::Ui::WebView::WebViewScreenshotCapturedCallback callback)
{
  return Dali::Ui::GetImpl(*this).GetScreenshotAsynchronously(viewArea, scaleFactor, callback);
}

bool WebView::CheckVideoPlayingAsynchronously(Dali::WebEnginePlugin::VideoPlayingCallback callback)
{
  return Dali::Ui::GetImpl(*this).CheckVideoPlayingAsynchronously(callback);
}

void WebView::RegisterGeolocationPermissionCallback(Dali::WebEnginePlugin::GeolocationPermissionCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterGeolocationPermissionCallback(callback);
}

void WebView::SetTtsFocus(bool focused)
{
  Dali::Ui::GetImpl(*this).SetTtsFocus(focused);
}

void WebView::RegisterPageLoadStartedCallback(Dali::WebEnginePlugin::WebEnginePageLoadCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterPageLoadStartedCallback(callback);
}

void WebView::RegisterPageLoadInProgressCallback(Dali::WebEnginePlugin::WebEnginePageLoadCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterPageLoadInProgressCallback(callback);
}

void WebView::RegisterPageLoadFinishedCallback(Dali::WebEnginePlugin::WebEnginePageLoadCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterPageLoadFinishedCallback(callback);
}

void WebView::RegisterPageLoadErrorCallback(Dali::WebEnginePlugin::WebEnginePageLoadErrorCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterPageLoadErrorCallback(callback);
}

void WebView::RegisterScrollEdgeReachedCallback(Dali::WebEnginePlugin::WebEngineScrollEdgeReachedCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterScrollEdgeReachedCallback(callback);
}

void WebView::RegisterOverScrolledCallback(Dali::WebEnginePlugin::WebEngineOverScrolledCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterOverScrolledCallback(callback);
}

void WebView::RegisterUrlChangedCallback(Dali::WebEnginePlugin::WebEngineUrlChangedCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterUrlChangedCallback(callback);
}

void WebView::RegisterFormRepostDecidedCallback(Dali::WebEnginePlugin::WebEngineFormRepostDecidedCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterFormRepostDecidedCallback(callback);
}

void WebView::RegisterFrameRenderedCallback(Dali::WebEnginePlugin::WebEngineFrameRenderedCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterFrameRenderedCallback(callback);
}

void WebView::RegisterConsoleMessageReceivedCallback(
    Dali::WebEnginePlugin::WebEngineConsoleMessageReceivedCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterConsoleMessageReceivedCallback(callback);
}

void WebView::RegisterResponsePolicyDecidedCallback(
    Dali::WebEnginePlugin::WebEngineResponsePolicyDecidedCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterResponsePolicyDecidedCallback(callback);
}

void WebView::RegisterNavigationPolicyDecidedCallback(
    Dali::WebEnginePlugin::WebEngineNavigationPolicyDecidedCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterNavigationPolicyDecidedCallback(callback);
}

void WebView::RegisterNewWindowPolicyDecidedCallback(
    Dali::WebEnginePlugin::WebEngineNewWindowPolicyDecidedCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterNewWindowPolicyDecidedCallback(callback);
}

void WebView::RegisterNewWindowCreatedCallback(Dali::WebEnginePlugin::WebEngineNewWindowCreatedCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterNewWindowCreatedCallback(callback);
}

void WebView::RegisterCertificateConfirmedCallback(Dali::WebEnginePlugin::WebEngineCertificateCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterCertificateConfirmedCallback(callback);
}

void WebView::RegisterSslCertificateChangedCallback(Dali::WebEnginePlugin::WebEngineCertificateCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterSslCertificateChangedCallback(callback);
}

void WebView::RegisterHttpAuthHandlerCallback(Dali::WebEnginePlugin::WebEngineHttpAuthHandlerCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterHttpAuthHandlerCallback(callback);
}

void WebView::RegisterContextMenuShownCallback(Dali::WebEnginePlugin::WebEngineContextMenuShownCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterContextMenuShownCallback(callback);
}

void WebView::RegisterContextMenuHiddenCallback(Dali::WebEnginePlugin::WebEngineContextMenuHiddenCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterContextMenuHiddenCallback(callback);
}

void WebView::RegisterFullscreenEnteredCallback(Dali::WebEnginePlugin::WebEngineFullscreenEnteredCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterFullscreenEnteredCallback(callback);
}

void WebView::RegisterFullscreenExitedCallback(Dali::WebEnginePlugin::WebEngineFullscreenExitedCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterFullscreenExitedCallback(callback);
}

void WebView::RegisterTextFoundCallback(Dali::WebEnginePlugin::WebEngineTextFoundCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterTextFoundCallback(callback);
}

void WebView::GetPlainTextAsynchronously(Dali::WebEnginePlugin::PlainTextReceivedCallback callback)
{
  Dali::Ui::GetImpl(*this).GetPlainTextAsynchronously(callback);
}

void WebView::WebAuthenticationCancel()
{
  Dali::Ui::GetImpl(*this).WebAuthenticationCancel();
}

void WebView::RegisterWebAuthDisplayQRCallback(Dali::WebEnginePlugin::WebEngineWebAuthDisplayQRCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterWebAuthDisplayQRCallback(callback);
}

void WebView::RegisterWebAuthResponseCallback(Dali::WebEnginePlugin::WebEngineWebAuthResponseCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterWebAuthResponseCallback(callback);
}

void WebView::RegisterFileChooserRequestedCallback(
    Dali::WebEnginePlugin::WebEngineFileChooserRequestedCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterFileChooserRequestedCallback(callback);
}

void WebView::RegisterWebProcessCrashedCallback(Dali::WebEnginePlugin::WebEngineWebProcessCrashedCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterWebProcessCrashedCallback(callback);
}

void WebView::RegisterUserMediaPermissionRequestCallback(
    Dali::WebEnginePlugin::WebEngineUserMediaPermissionRequestCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterUserMediaPermissionRequestCallback(callback);
}

void WebView::RegisterDeviceConnectionChangedCallback(
    Dali::WebEnginePlugin::WebEngineDeviceConnectionChangedCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterDeviceConnectionChangedCallback(callback);
}

void WebView::RegisterDeviceListGetCallback(Dali::WebEnginePlugin::WebEngineDeviceListGetCallback callback)
{
  Dali::Ui::GetImpl(*this).RegisterDeviceListGetCallback(callback);
}

void WebView::FeedMouseWheel(bool yDirection, int step, int x, int y)
{
  Dali::Ui::GetImpl(*this).FeedMouseWheel(yDirection, step, x, y);
}

void WebView::SetVideoHole(bool enabled, bool isWaylandWindow)
{
  Dali::Ui::GetImpl(*this).SetVideoHole(enabled, isWaylandWindow);
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

} // namespace Ui

} // namespace Dali
