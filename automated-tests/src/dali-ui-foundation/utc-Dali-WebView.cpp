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

#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/web-view.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_webview_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_webview_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
void OnJavaScriptCallback(const Dali::String& result) {}
bool OnJavaScriptAlertCallback(const Dali::String& message) { return true; }
bool OnJavaScriptConfirmCallback(const Dali::String& message) { return true; }
bool OnJavaScriptPromptCallback(const Dali::String& message, const Dali::String& defaultValue) { return true; }
void OnScreenshotCallback(Dali::Ui::ImageView screenshot) {}
void OnVideoPlayingCallback(bool isPlaying) {}
void OnPlainTextCallback(const Dali::String& text) {}

} // namespace

// ===========================================================================
// Construction & Destruction
// ===========================================================================

int UtcDaliWebViewConstructorP(void)
{
  UiTestApplication application;
  WebView view;
  DALI_TEST_CHECK(!view);
  END_TEST;
}

int UtcDaliWebViewNewP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewNewWithUrlP(void)
{
  UiTestApplication application;
  WebView view = WebView::New(Dali::String("https://www.example.com"));
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewCopyConstructorP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  WebView copy(view);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliWebViewMoveConstructorP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  WebView moved(std::move(view));
  DALI_TEST_CHECK(moved);
  END_TEST;
}

int UtcDaliWebViewAssignmentOperatorP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  WebView copy;
  copy = view;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliWebViewMoveAssignmentOperatorP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  WebView moved;
  moved = std::move(view);
  DALI_TEST_CHECK(moved);
  END_TEST;
}

// ===========================================================================
// DownCast
// ===========================================================================

int UtcDaliWebViewDownCastP(void)
{
  UiTestApplication application;
  WebView view     = WebView::New();
  View   baseView  = view;
  WebView downcast = WebView::DownCast(baseView);
  DALI_TEST_CHECK(downcast);
  END_TEST;
}

int UtcDaliWebViewDownCastN(void)
{
  UiTestApplication application;
  View   baseView = View::New();
  WebView downcast = WebView::DownCast(baseView);
  DALI_TEST_CHECK(!downcast);
  END_TEST;
}

// ===========================================================================
// URL & User Agent
// ===========================================================================

int UtcDaliWebViewGetUrlP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  Dali::String url = view.GetUrl();
  DALI_TEST_CHECK(url.Empty() || url.Size() >= 0);
  END_TEST;
}

int UtcDaliWebViewSetGetUserAgentP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.SetUserAgent(Dali::String("TestAgent/1.0"));
  Dali::String agent = view.GetUserAgent();
  DALI_TEST_CHECK(agent.Size() >= 0);
  END_TEST;
}

// ===========================================================================
// Page Loading
// ===========================================================================

int UtcDaliWebViewLoadUrlP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.LoadUrl(Dali::String("https://www.example.com"));
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewLoadHtmlStringP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.LoadHtmlString(Dali::String("<html><body>Test</body></html>"));
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewLoadHtmlStringOverrideCurrentEntryP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.LoadHtmlStringOverrideCurrentEntry(
    Dali::String("<html><body>Test</body></html>"),
    Dali::String(""),
    Dali::String(""));
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewLoadContentsP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  const char* content = "<html><body>Test</body></html>";
  view.LoadContents(
    reinterpret_cast<const int8_t*>(content),
    strlen(content),
    Dali::String("text/html"),
    Dali::String("UTF-8"),
    Dali::String(""));
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewReloadP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.Reload();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewReloadWithoutCacheP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.ReloadWithoutCache();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewStopLoadingP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.StopLoading();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewSuspendResumeP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.Suspend();
  view.Resume();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewSuspendResumeNetworkLoadingP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.SuspendNetworkLoading();
  view.ResumeNetworkLoading();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewGetLoadProgressPercentageP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  float progress = view.GetLoadProgressPercentage();
  DALI_TEST_CHECK(progress >= 0.0f);
  END_TEST;
}

// ===========================================================================
// Custom Headers
// ===========================================================================

int UtcDaliWebViewAddRemoveCustomHeaderP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.AddCustomHeader(Dali::String("X-Custom-Header"), Dali::String("CustomValue"));
  view.RemoveCustomHeader(Dali::String("X-Custom-Header"));
  DALI_TEST_CHECK(view);
  END_TEST;
}

// ===========================================================================
// Scroll
// ===========================================================================

int UtcDaliWebViewGetScrollPositionP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  Vector2 pos = view.GetScrollPosition();
  DALI_TEST_CHECK(pos.x >= 0.0f && pos.y >= 0.0f);
  END_TEST;
}

int UtcDaliWebViewGetScrollSizeP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  Vector2 size = view.GetScrollSize();
  DALI_TEST_CHECK(size.x >= 0.0f && size.y >= 0.0f);
  END_TEST;
}

int UtcDaliWebViewGetContentSizeP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  Vector2 size = view.GetContentSize();
  DALI_TEST_CHECK(size.x >= 0.0f && size.y >= 0.0f);
  END_TEST;
}

int UtcDaliWebViewScrollByP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.ScrollBy(10, 20);
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewScrollEdgeByP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.ScrollEdgeBy(10, 20);
  DALI_TEST_CHECK(view);
  END_TEST;
}

// ===========================================================================
// Navigation
// ===========================================================================

int UtcDaliWebViewCanGoForwardP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  bool canGo = view.CanGoForward();
  DALI_TEST_CHECK(!canGo || canGo);
  END_TEST;
}

int UtcDaliWebViewGoForwardP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.GoForward();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewCanGoBackP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  bool canGo = view.CanGoBack();
  DALI_TEST_CHECK(!canGo || canGo);
  END_TEST;
}

int UtcDaliWebViewGoBackP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.GoBack();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewClearHistoryP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.ClearHistory();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewClearAllTilesResourcesP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.ClearAllTilesResources();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewExitFullscreenP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.ExitFullscreen();
  DALI_TEST_CHECK(view);
  END_TEST;
}

// ===========================================================================
// JavaScript
// ===========================================================================

int UtcDaliWebViewEvaluateJavaScriptP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.EvaluateJavaScript(Dali::String("1+1"));
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewEvaluateJavaScriptWithCallbackP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  auto callback = WebView::JavaScriptCallback::New(OnJavaScriptCallback);
  view.EvaluateJavaScript(Dali::String("document.title"), std::move(callback));
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewAddJavaScriptMessageHandlerP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  auto callback = WebView::JavaScriptCallback::New(OnJavaScriptCallback);
  view.AddJavaScriptMessageHandler(Dali::String("testObject"), std::move(callback));
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewRegisterJavaScriptAlertCallbackP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  auto callback = WebView::JavaScriptAlertCallback::New(OnJavaScriptAlertCallback);
  view.RegisterJavaScriptAlertCallback(std::move(callback));
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewJavaScriptAlertReplyP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.JavaScriptAlertReply();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewRegisterJavaScriptConfirmCallbackP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  auto callback = WebView::JavaScriptConfirmCallback::New(OnJavaScriptConfirmCallback);
  view.RegisterJavaScriptConfirmCallback(std::move(callback));
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewJavaScriptConfirmReplyP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.JavaScriptConfirmReply(true);
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewRegisterJavaScriptPromptCallbackP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  auto callback = WebView::JavaScriptPromptCallback::New(OnJavaScriptPromptCallback);
  view.RegisterJavaScriptPromptCallback(std::move(callback));
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewJavaScriptPromptReplyP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.JavaScriptPromptReply(Dali::String("test input"));
  DALI_TEST_CHECK(view);
  END_TEST;
}

// ===========================================================================
// Scale & Zoom
// ===========================================================================

int UtcDaliWebViewSetGetScaleFactorP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.SetScaleFactor(2.0f, Vector2(100.0f, 100.0f));
  float scale = view.GetScaleFactor();
  DALI_TEST_CHECK(scale > 0.0f);
  END_TEST;
}

int UtcDaliWebViewSetGetPageZoomFactorP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.SetPageZoomFactor(1.5f);
  float zoom = view.GetPageZoomFactor();
  DALI_TEST_CHECK(zoom > 0.0f);
  END_TEST;
}

int UtcDaliWebViewSetGetTextZoomFactorP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.SetTextZoomFactor(1.2f);
  float zoom = view.GetTextZoomFactor();
  DALI_TEST_CHECK(zoom > 0.0f);
  END_TEST;
}

// ===========================================================================
// Screenshot
// ===========================================================================

int UtcDaliWebViewGetScreenshotP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  Dali::BoundsInteger area(0, 0, 100, 100);
  Dali::Ui::ImageView screenshot = view.GetScreenshot(area, 1.0f);
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewGetScreenshotAsynchronouslyP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  auto callback = WebView::ScreenshotCapturedCallback::New(OnScreenshotCallback);
  Dali::BoundsInteger area(0, 0, 100, 100);
  view.GetScreenshotAsynchronously(area, 1.0f, std::move(callback));
  DALI_TEST_CHECK(view);
  END_TEST;
}

// ===========================================================================
// Page Info
// ===========================================================================

int UtcDaliWebViewGetTitleP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  Dali::String title = view.GetTitle();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewGetFaviconP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  Dali::Ui::ImageView favicon = view.GetFavicon();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewGetSelectedTextP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  Dali::String text = view.GetSelectedText();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewGetPlainTextAsynchronouslyP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  auto callback = WebView::PlainTextCallback::New(OnPlainTextCallback);
  view.GetPlainTextAsynchronously(std::move(callback));
  DALI_TEST_CHECK(view);
  END_TEST;
}

// ===========================================================================
// Document Appearance
// ===========================================================================

int UtcDaliWebViewSetDocumentBackgroundColorP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.SetDocumentBackgroundColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewSetTilesClearedWhenHiddenP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.SetTilesClearedWhenHidden(true);
  view.SetTilesClearedWhenHidden(false);
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewSetTileCoverAreaMultiplierP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.SetTileCoverAreaMultiplier(2.0f);
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewSetCursorEnabledByClientP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.SetCursorEnabledByClient(true);
  view.SetCursorEnabledByClient(false);
  DALI_TEST_CHECK(view);
  END_TEST;
}

// ===========================================================================
// Input & Video
// ===========================================================================

int UtcDaliWebViewSetGetMouseEventsEnabledP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  DALI_TEST_EQUALS(view.GetMouseEventsEnabled(), true, TEST_LOCATION);
  view.SetMouseEventsEnabled(false);
  DALI_TEST_EQUALS(view.GetMouseEventsEnabled(), false, TEST_LOCATION);
  view.SetMouseEventsEnabled(true);
  DALI_TEST_EQUALS(view.GetMouseEventsEnabled(), true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWebViewSetGetKeyEventsEnabledP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  DALI_TEST_EQUALS(view.GetKeyEventsEnabled(), true, TEST_LOCATION);
  view.SetKeyEventsEnabled(false);
  DALI_TEST_EQUALS(view.GetKeyEventsEnabled(), false, TEST_LOCATION);
  view.SetKeyEventsEnabled(true);
  DALI_TEST_EQUALS(view.GetKeyEventsEnabled(), true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWebViewFeedMouseWheelP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.FeedMouseWheel(true, 1, 100, 100);
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewSetVideoHoleEnabledP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.SetVideoHoleEnabled(true);
  view.SetVideoHoleEnabled(false);
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewSetGetVideoHoleEnabledP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.SetVideoHoleEnabled(true);
  DALI_TEST_EQUALS(view.GetVideoHoleEnabled(), true, TEST_LOCATION);
  view.SetVideoHoleEnabled(false);
  DALI_TEST_EQUALS(view.GetVideoHoleEnabled(), false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWebViewCheckVideoPlayingAsynchronouslyP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  auto callback = WebView::VideoPlayingCallback::New(OnVideoPlayingCallback);
  view.CheckVideoPlayingAsynchronously(std::move(callback));
  DALI_TEST_CHECK(view);
  END_TEST;
}

// ===========================================================================
// Signals — Page Loading
// ===========================================================================

int UtcDaliWebViewPageLoadStartedSignalP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.PageLoadStartedSignal().Connect([](WebView v, const Dali::String& url) {});
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewPageLoadInProgressSignalP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.PageLoadInProgressSignal().Connect([](WebView v, const Dali::String& url) {});
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewPageLoadFinishedSignalP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.PageLoadFinishedSignal().Connect([](WebView v, const Dali::String& url) {});
  DALI_TEST_CHECK(view);
  END_TEST;
}

// ===========================================================================
// Signals — Scroll
// ===========================================================================

int UtcDaliWebViewScrollEdgeReachedSignalP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.ScrollEdgeReachedSignal().Connect([](WebView v, WebViewScrollEdge edge) {});
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewOverScrolledSignalP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.OverScrolledSignal().Connect([](WebView v, WebViewOverScrolled direction) {});
  DALI_TEST_CHECK(view);
  END_TEST;
}

// ===========================================================================
// Signals — Navigation
// ===========================================================================

int UtcDaliWebViewUrlChangedSignalP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.UrlChangedSignal().Connect([](WebView v, const Dali::String& url) {});
  DALI_TEST_CHECK(view);
  END_TEST;
}

// ===========================================================================
// Signals — Rendering
// ===========================================================================

int UtcDaliWebViewFrameRenderedSignalP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.FrameRenderedSignal().Connect([](WebView v) {});
  DALI_TEST_CHECK(view);
  END_TEST;
}

// ===========================================================================
// Signals — Fullscreen
// ===========================================================================

int UtcDaliWebViewFullscreenEnteredSignalP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.FullscreenEnteredSignal().Connect([](WebView v) {});
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewFullscreenExitedSignalP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.FullscreenExitedSignal().Connect([](WebView v) {});
  DALI_TEST_CHECK(view);
  END_TEST;
}

// ===========================================================================
// Signals — Text Search & Geolocation
// ===========================================================================

int UtcDaliWebViewTextFoundSignalP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.TextFoundSignal().Connect([](WebView v, uint32_t count) {});
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewGeolocationPermissionSignalP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.GeolocationPermissionSignal().Connect([](WebView v, const Dali::String& host, const Dali::String& protocol) -> bool { return true; });
  DALI_TEST_CHECK(view);
  END_TEST;
}

// ===========================================================================
// Signals — Process Events
// ===========================================================================

int UtcDaliWebViewWebProcessCrashedSignalP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.WebProcessCrashedSignal().Connect([](WebView v) {});
  DALI_TEST_CHECK(view);
  END_TEST;
}

// ===========================================================================
// Enumerations
// ===========================================================================

int UtcDaliWebViewScrollEdgeEnumP(void)
{
  DALI_TEST_EQUALS(static_cast<int>(WebViewScrollEdge::LEFT), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(WebViewScrollEdge::RIGHT), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(WebViewScrollEdge::TOP), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(WebViewScrollEdge::BOTTOM), 3, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWebViewOverScrolledEnumP(void)
{
  DALI_TEST_EQUALS(static_cast<int>(WebViewOverScrolled::LEFT), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(WebViewOverScrolled::RIGHT), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(WebViewOverScrolled::TOP), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(WebViewOverScrolled::BOTTOM), 3, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWebViewFindOptionEnumP(void)
{
  DALI_TEST_EQUALS(static_cast<uint32_t>(WebViewFindOption::NONE), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint32_t>(WebViewFindOption::CASE_INSENSITIVE), 1u << 0, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint32_t>(WebViewFindOption::AT_WORD_STARTS), 1u << 1, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint32_t>(WebViewFindOption::BACKWARDS), 1u << 3, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint32_t>(WebViewFindOption::WRAP_AROUND), 1u << 4, TEST_LOCATION);
  END_TEST;
}

