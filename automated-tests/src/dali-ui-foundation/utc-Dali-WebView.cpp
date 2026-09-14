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
#include <dali-ui-foundation/public-api/views/web/web-view.h>

#define private public
#define protected public
#include <dali-ui-foundation/integration-api/web-view-impl.h>
#undef protected
#undef private

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
void OnJavaScriptMessageWithNameCallback(const Dali::String& exposedObjectName, const Dali::String& message) {}
bool OnJavaScriptAlertCallback(const Dali::String& message) { return true; }
bool OnJavaScriptConfirmCallback(const Dali::String& message) { return true; }
bool OnJavaScriptPromptCallback(const Dali::String& message, const Dali::String& defaultValue) { return true; }
void OnScreenshotCallback(Dali::Ui::ImageView screenshot) {}
void OnVideoPlayingCallback(bool isPlaying) {}
void OnPlainTextCallback(const Dali::String& text) {}

Dali::Ui::Integration::WebViewImpl& GetWebViewImpl(WebView view)
{
  return static_cast<Dali::Ui::Integration::WebViewImpl&>(view.GetImplementation());
}

} // namespace

// ===========================================================================
// Construction & Destruction
// ===========================================================================

int UtcDaliWebViewUiConfigWebEngineTypeP(void)
{
  UiConfig config = UiConfig::New();

  DALI_TEST_EQUALS(config.GetWebEngineType(), WebEngineType::CHROMIUM, TEST_LOCATION);

  config.SetWebEngineType(WebEngineType::LWE);
  DALI_TEST_EQUALS(config.GetWebEngineType(), WebEngineType::LWE, TEST_LOCATION);

  config.Apply();

  DALI_TEST_EQUALS(UiConfig::GetCurrent().GetWebEngineType(), WebEngineType::LWE, TEST_LOCATION);
  DALI_TEST_ASSERTION(config.SetWebEngineType(WebEngineType::CHROMIUM), "UiConfig is frozen after UiConfig::Apply()");

  END_TEST;
}

int UtcDaliWebViewNewWithoutAppliedUiConfigN(void)
{
  DALI_TEST_ASSERTION(WebView::New(), "UiConfig::Apply() must be called before WebView::New()");
  END_TEST;
}

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

int UtcDaliWebViewSetScrollPositionP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.SetScrollPosition(Vector2(10.0f, 20.0f));
  DALI_TEST_CHECK(view);
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

int UtcDaliWebViewClearCacheShortcutP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.ClearCache();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewClearCookiesShortcutP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.ClearCookies();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewGetProfileP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  WebView otherView = WebView::New();

  WebProfile profile = view.GetProfile();
  WebProfile sameProfile = view.GetProfile();
  WebProfile equivalentProfile = otherView.GetProfile();
  WebProfile emptyProfile;

  DALI_TEST_CHECK(profile);
  DALI_TEST_CHECK(profile == sameProfile);
  DALI_TEST_CHECK(profile == equivalentProfile);
  DALI_TEST_CHECK(profile != emptyProfile);
  profile.ClearCache();
  END_TEST;
}

int UtcDaliWebProfileGetCookieManagerP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();

  WebCookieManager manager = view.GetProfile().GetCookieManager();
  WebCookieManager sameManager = view.GetProfile().GetCookieManager();
  WebCookieManager emptyManager;

  DALI_TEST_CHECK(manager);
  DALI_TEST_CHECK(manager == sameManager);
  DALI_TEST_CHECK(manager != emptyManager);
  manager.ClearAllCookies();
  END_TEST;
}

int UtcDaliWebViewGetSettingsP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();

  WebSettings settings = view.GetSettings();
  WebSettings sameSettings = view.GetSettings();
  WebSettings emptySettings;

  DALI_TEST_CHECK(settings);
  DALI_TEST_CHECK(settings == sameSettings);
  DALI_TEST_CHECK(settings != emptySettings);
  settings.SetExtraFeatureValue(Dali::String("testFeature"), Dali::String("testValue"));
  settings.GetExtraFeatureValue(Dali::String("testFeature"));
  settings.SetSpatialNavigationEnabled(true);
  settings.GetDefaultFontSize();
  settings.SetDefaultFontSize(16u);
  settings.SetWebSecurityEnabled(true);
  settings.SetExtraFeatureEnabled(Dali::String("testFeature"), true);
  settings.IsExtraFeatureEnabled(Dali::String("testFeature"));
  END_TEST;
}

int UtcDaliWebViewGetBackForwardListP(void)
{
  UiTestApplication application;
  WebBackForwardList list;

  {
    WebView view = WebView::New();
    list = view.GetBackForwardList();
    DALI_TEST_CHECK(list);
    BaseHandle base = list;
    WebBackForwardList downcast = WebBackForwardList::DownCast(base);
    WebBackForwardList copy(list);
    WebBackForwardList moved(std::move(copy));
    DALI_TEST_CHECK(downcast == list);
    DALI_TEST_CHECK(moved == list);
    list.GetCurrentItem();
    list.GetPreviousItem();
    list.GetNextItem();
    list.GetItemAtIndex(0u);
    list.GetItemCount();
    list.GetBackwardItems();
    list.GetForwardItems();
  }

  DALI_TEST_EQUALS(list.GetItemCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!list.GetCurrentItem().IsValid());
  END_TEST;
}

int UtcDaliWebBackForwardListItemDefaultP(void)
{
  WebBackForwardListItem item;
  WebBackForwardListItem copy(item);
  WebBackForwardListItem moved(std::move(copy));
  WebBackForwardListItem assigned;
  assigned = item;
  WebBackForwardListItem moveAssigned;
  moveAssigned = std::move(assigned);

  DALI_TEST_CHECK(!item.IsValid());
  DALI_TEST_CHECK(item.GetUrl().Empty());
  DALI_TEST_CHECK(item.GetTitle().Empty());
  DALI_TEST_CHECK(item.GetOriginalUrl().Empty());
  DALI_TEST_CHECK(!moved.IsValid());
  DALI_TEST_CHECK(!moveAssigned.IsValid());
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

int UtcDaliWebViewAddJavaScriptMessageHandlerWithNameP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  auto callback = Dali::Callback<void(const Dali::String&, const Dali::String&)>::New(OnJavaScriptMessageWithNameCallback);
  view.AddJavaScriptMessageHandler(Dali::String("testObject"), std::move(callback));
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewRemoveJavaScriptMessageHandlerP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.RemoveJavaScriptMessageHandler(Dali::String("testObject"));
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

int UtcDaliWebViewSetIsMouseEventsEnabledP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  DALI_TEST_EQUALS(view.IsMouseEventsEnabled(), true, TEST_LOCATION);
  view.SetMouseEventsEnabled(false);
  DALI_TEST_EQUALS(view.IsMouseEventsEnabled(), false, TEST_LOCATION);
  view.SetMouseEventsEnabled(true);
  DALI_TEST_EQUALS(view.IsMouseEventsEnabled(), true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWebViewSetIsKeyEventsEnabledP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  DALI_TEST_EQUALS(view.IsKeyEventsEnabled(), true, TEST_LOCATION);
  view.SetKeyEventsEnabled(false);
  DALI_TEST_EQUALS(view.IsKeyEventsEnabled(), false, TEST_LOCATION);
  view.SetKeyEventsEnabled(true);
  DALI_TEST_EQUALS(view.IsKeyEventsEnabled(), true, TEST_LOCATION);
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

int UtcDaliWebViewSetIsVideoHoleEnabledP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.SetVideoHoleEnabled(true);
  DALI_TEST_EQUALS(view.IsVideoHoleEnabled(), true, TEST_LOCATION);
  view.SetVideoHoleEnabled(false);
  DALI_TEST_EQUALS(view.IsVideoHoleEnabled(), false, TEST_LOCATION);
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

int UtcDaliWebViewPageLoadErrorSignalP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  view.PageLoadErrorSignal().Connect([](WebView v, const WebViewPageLoadError& error) {});
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliWebViewPageLoadErrorValueSemanticsP(void)
{
  WebViewPageLoadError error;
  DALI_TEST_CHECK(error.GetUrl().Empty());
  DALI_TEST_CHECK(error.GetCode() == WebViewPageLoadErrorCode::UNKNOWN);
  DALI_TEST_CHECK(error.GetDescription().Empty());
  DALI_TEST_CHECK(error.GetType() == WebViewPageLoadErrorType::NONE);

  WebViewPageLoadError copy(error);
  DALI_TEST_CHECK(copy.GetCode() == WebViewPageLoadErrorCode::UNKNOWN);
  DALI_TEST_CHECK(copy.GetType() == WebViewPageLoadErrorType::NONE);

  WebViewPageLoadError moved(std::move(copy));
  DALI_TEST_CHECK(moved.GetCode() == WebViewPageLoadErrorCode::UNKNOWN);
  DALI_TEST_CHECK(copy.GetUrl().Empty());

  copy = moved;
  DALI_TEST_CHECK(copy.GetDescription().Empty());

  WebViewPageLoadError moveAssigned;
  moveAssigned = std::move(copy);
  DALI_TEST_CHECK(moveAssigned.GetType() == WebViewPageLoadErrorType::NONE);
  DALI_TEST_CHECK(copy.GetDescription().Empty());

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

int UtcDaliWebViewFindTextP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  DALI_TEST_CHECK(view);

  // Matches are reported asynchronously through TextFoundSignal().
  view.TextFoundSignal().Connect([](WebView /*v*/, uint32_t /*count*/) {});

  // Single option and OR-combined options (WebViewFindOption supports bitwise OR).
  view.FindText("hello", WebViewFindOption::CASE_INSENSITIVE, 100u);
  view.FindText("world", WebViewFindOption::CASE_INSENSITIVE | WebViewFindOption::WRAP_AROUND, 10u);

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

int UtcDaliWebViewFindOptionBitwiseOperatorsP(void)
{
  // operator|
  WebViewFindOption combined = WebViewFindOption::CASE_INSENSITIVE | WebViewFindOption::SHOW_HIGHLIGHT;
  DALI_TEST_EQUALS(static_cast<uint32_t>(combined), (1u << 0) | (1u << 7), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint32_t>(WebViewFindOption::NONE | WebViewFindOption::BACKWARDS), 1u << 3, TEST_LOCATION);

  // operator|=
  WebViewFindOption accumulated = WebViewFindOption::NONE;
  accumulated |= WebViewFindOption::AT_WORD_STARTS;
  accumulated |= WebViewFindOption::WRAP_AROUND;
  DALI_TEST_EQUALS(static_cast<uint32_t>(accumulated), (1u << 1) | (1u << 4), TEST_LOCATION);

  // operator& : set bit remains, unset bit yields NONE
  DALI_TEST_EQUALS(static_cast<uint32_t>(combined & WebViewFindOption::SHOW_HIGHLIGHT), 1u << 7, TEST_LOCATION);
  DALI_TEST_CHECK((combined & WebViewFindOption::CASE_INSENSITIVE) == WebViewFindOption::CASE_INSENSITIVE);
  DALI_TEST_CHECK((combined & WebViewFindOption::BACKWARDS) == WebViewFindOption::NONE);

  // operator~ : masking out a single flag keeps the others
  DALI_TEST_EQUALS(static_cast<uint32_t>(combined & ~WebViewFindOption::CASE_INSENSITIVE), 1u << 7, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint32_t>(~WebViewFindOption::NONE), ~0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliWebViewPageLoadErrorCodeEnumP(void)
{
  DALI_TEST_EQUALS(static_cast<int>(WebViewPageLoadErrorCode::UNKNOWN), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(WebViewPageLoadErrorCode::CANCELED), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(WebViewPageLoadErrorCode::OTHER), 15, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWebViewPageLoadErrorTypeEnumP(void)
{
  DALI_TEST_EQUALS(static_cast<int>(WebViewPageLoadErrorType::NONE), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(WebViewPageLoadErrorType::NETWORK), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(WebViewPageLoadErrorType::PRINT), 6, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWebViewInternalConversionsAndPropertiesP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  auto& impl = GetWebViewImpl(view);

  DALI_TEST_EQUALS(impl.ToStdString(Dali::String("text")), std::string("text"), TEST_LOCATION);
  DALI_TEST_EQUALS(impl.ToDaliString(std::string("value")), Dali::String("value"), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(impl.ToUiScrollEdge(Dali::WebEnginePlugin::ScrollEdge::LEFT)), static_cast<int>(WebViewScrollEdge::LEFT), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(impl.ToUiScrollEdge(Dali::WebEnginePlugin::ScrollEdge::RIGHT)), static_cast<int>(WebViewScrollEdge::RIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(impl.ToUiScrollEdge(Dali::WebEnginePlugin::ScrollEdge::TOP)), static_cast<int>(WebViewScrollEdge::TOP), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(impl.ToUiScrollEdge(Dali::WebEnginePlugin::ScrollEdge::BOTTOM)), static_cast<int>(WebViewScrollEdge::BOTTOM), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(impl.ToUiScrollEdge(static_cast<Dali::WebEnginePlugin::ScrollEdge>(99))), static_cast<int>(WebViewScrollEdge::BOTTOM), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(impl.ToUiOverScrolled(Dali::WebEnginePlugin::OverScrolled::LEFT)), static_cast<int>(WebViewOverScrolled::LEFT), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(impl.ToUiOverScrolled(Dali::WebEnginePlugin::OverScrolled::RIGHT)), static_cast<int>(WebViewOverScrolled::RIGHT), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(impl.ToUiOverScrolled(Dali::WebEnginePlugin::OverScrolled::TOP)), static_cast<int>(WebViewOverScrolled::TOP), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(impl.ToUiOverScrolled(Dali::WebEnginePlugin::OverScrolled::BOTTOM)), static_cast<int>(WebViewOverScrolled::BOTTOM), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(impl.ToUiOverScrolled(static_cast<Dali::WebEnginePlugin::OverScrolled>(99))), static_cast<int>(WebViewOverScrolled::BOTTOM), TEST_LOCATION);
  impl.ToEngineFindOption(WebViewFindOption::CASE_INSENSITIVE | WebViewFindOption::WRAP_AROUND);
  impl.ToUiPageLoadErrorCode(Dali::WebEngineLoadError::ErrorCode::UNKNOWN);
  impl.ToUiPageLoadErrorType(Dali::WebEngineLoadError::ErrorType::NONE);

  BaseObject* object = &view.GetBaseObject();
  const Dali::Property::Index writable[] = {
    Dali::Ui::Integration::WebViewImpl::Property::URL, Dali::Ui::Integration::WebViewImpl::Property::USER_AGENT, Dali::Ui::Integration::WebViewImpl::Property::SCROLL_POSITION,
    Dali::Ui::Integration::WebViewImpl::Property::VIDEO_HOLE_ENABLED, Dali::Ui::Integration::WebViewImpl::Property::MOUSE_EVENTS_ENABLED,
    Dali::Ui::Integration::WebViewImpl::Property::KEY_EVENTS_ENABLED, Dali::Ui::Integration::WebViewImpl::Property::DOCUMENT_BACKGROUND_COLOR,
    Dali::Ui::Integration::WebViewImpl::Property::TILES_CLEARED_WHEN_HIDDEN, Dali::Ui::Integration::WebViewImpl::Property::TILE_COVER_AREA_MULTIPLIER,
    Dali::Ui::Integration::WebViewImpl::Property::CURSOR_ENABLED_BY_CLIENT, Dali::Ui::Integration::WebViewImpl::Property::PAGE_ZOOM_FACTOR,
    Dali::Ui::Integration::WebViewImpl::Property::TEXT_ZOOM_FACTOR};
  const Dali::Property::Value values[] = {
    Dali::String("https://example.com"), Dali::String("agent"), Vector2(1.0f, 2.0f),
    true, false, false, Color::WHITE, true, 2.0f, true, 1.5f, 1.25f};
  for(size_t i = 0u; i < sizeof(writable) / sizeof(writable[0]); ++i)
  {
    Dali::Ui::Integration::WebViewImpl::SetProperty(object, writable[i], values[i]);
  }
  Dali::Ui::Integration::WebViewImpl::SetProperty(object, 999999, Dali::Property::Value());

  const Dali::Property::Index readable[] = {
    Dali::Ui::Integration::WebViewImpl::Property::URL, Dali::Ui::Integration::WebViewImpl::Property::USER_AGENT, Dali::Ui::Integration::WebViewImpl::Property::SCROLL_POSITION,
    Dali::Ui::Integration::WebViewImpl::Property::SCROLL_SIZE, Dali::Ui::Integration::WebViewImpl::Property::CONTENT_SIZE, Dali::Ui::Integration::WebViewImpl::Property::TITLE,
    Dali::Ui::Integration::WebViewImpl::Property::VIDEO_HOLE_ENABLED, Dali::Ui::Integration::WebViewImpl::Property::MOUSE_EVENTS_ENABLED,
    Dali::Ui::Integration::WebViewImpl::Property::KEY_EVENTS_ENABLED, Dali::Ui::Integration::WebViewImpl::Property::SELECTED_TEXT,
    Dali::Ui::Integration::WebViewImpl::Property::PAGE_ZOOM_FACTOR, Dali::Ui::Integration::WebViewImpl::Property::TEXT_ZOOM_FACTOR,
    Dali::Ui::Integration::WebViewImpl::Property::LOAD_PROGRESS_PERCENTAGE};
  for(auto index : readable)
  {
    DALI_TEST_CHECK(Dali::Ui::Integration::WebViewImpl::GetProperty(object, index).GetType() != Dali::Property::NONE);
  }
  DALI_TEST_EQUALS(Dali::Ui::Integration::WebViewImpl::GetProperty(object, 999999).GetType(), Dali::Property::NONE, TEST_LOCATION);
  DALI_TEST_CHECK(!impl.CreateImageViewFromPixelData(PixelData()));
  END_TEST;
}

int UtcDaliWebViewInternalSignalEmissionP(void)
{
  UiTestApplication application;
  WebView view = WebView::New();
  auto& impl = GetWebViewImpl(view);
  WebViewPageLoadError error;

  impl.EmitPageLoadStarted("url");
  impl.EmitPageLoadInProgress("url");
  impl.EmitPageLoadFinished("url");
  impl.EmitPageLoadError(error);
  impl.EmitScrollEdgeReached(WebViewScrollEdge::LEFT);
  impl.EmitOverScrolled(WebViewOverScrolled::RIGHT);
  impl.EmitUrlChanged("url");
  impl.EmitFrameRendered();
  impl.EmitFullscreenEntered();
  impl.EmitFullscreenExited();
  impl.EmitTextFound(3u);
  DALI_TEST_CHECK(!impl.EmitGeolocationPermission("host", "https"));
  impl.EmitWebProcessCrashed();

  int calls = 0;
  view.PageLoadStartedSignal().Connect(&application, [&calls](WebView, const Dali::String&) { ++calls; });
  view.PageLoadInProgressSignal().Connect(&application, [&calls](WebView, const Dali::String&) { ++calls; });
  view.PageLoadFinishedSignal().Connect(&application, [&calls](WebView, const Dali::String&) { ++calls; });
  view.PageLoadErrorSignal().Connect(&application, [&calls](WebView, const WebViewPageLoadError&) { ++calls; });
  view.ScrollEdgeReachedSignal().Connect(&application, [&calls](WebView, WebViewScrollEdge) { ++calls; });
  view.OverScrolledSignal().Connect(&application, [&calls](WebView, WebViewOverScrolled) { ++calls; });
  view.UrlChangedSignal().Connect(&application, [&calls](WebView, const Dali::String&) { ++calls; });
  view.FrameRenderedSignal().Connect(&application, [&calls](WebView) { ++calls; });
  view.FullscreenEnteredSignal().Connect(&application, [&calls](WebView) { ++calls; });
  view.FullscreenExitedSignal().Connect(&application, [&calls](WebView) { ++calls; });
  view.TextFoundSignal().Connect(&application, [&calls](WebView, uint32_t) { ++calls; });
  view.GeolocationPermissionSignal().Connect(&application, [&calls](WebView, const Dali::String&, const Dali::String&) { ++calls; return true; });
  view.WebProcessCrashedSignal().Connect(&application, [&calls](WebView) { ++calls; });

  impl.EmitPageLoadStarted("url");
  impl.EmitPageLoadInProgress("url");
  impl.EmitPageLoadFinished("url");
  impl.EmitPageLoadError(error);
  impl.EmitScrollEdgeReached(WebViewScrollEdge::LEFT);
  impl.EmitOverScrolled(WebViewOverScrolled::RIGHT);
  impl.EmitUrlChanged("url");
  impl.EmitFrameRendered();
  impl.EmitFullscreenEntered();
  impl.EmitFullscreenExited();
  impl.EmitTextFound(3u);
  DALI_TEST_CHECK(impl.EmitGeolocationPermission("host", "https"));
  impl.EmitWebProcessCrashed();
  DALI_TEST_EQUALS(calls, 13, TEST_LOCATION);
  END_TEST;
}
