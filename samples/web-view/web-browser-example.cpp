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
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/views/web/web-view.h>
#include <dali/integration-api/debug.h>
#include <cstdio>
#include <cstring>

using namespace Dali;
using namespace Dali::Ui;

// === Test instrumentation ===
// Plain DALI_LOG_RELEASE_INFO calls with a [WVLOG] prefix so signal/callback firing is
// observable on the console. Filter with:  ./web-browser.example 2>&1 | grep WVLOG
//   [WVLOG][signal] = WebView signal handler actually fired   (the thing under test)
//   [WVLOG][button] = button TouchedSignal fired
//   [WVLOG][cmd]    = imperative WebView call (LoadUrl/GoBack/query...)
//   [WVLOG][init]   = setup / lifecycle
//
// Newly added APIs under test (see migration from EWK):
//   PageLoadErrorSignal -> OnPageLoadError ([WVLOG][signal], e.g. load an unreachable URL)
//   Key 'C' = ClearCache(), Key 'K' = ClearAllCookies()  ([WVLOG][cmd])

namespace
{
const char* HOME_URL       = "https://www.samsung.com";
const float TOOLBAR_HEIGHT = 52.0f;
const float STATUS_HEIGHT  = 28.0f;
const float NAV_BTN_WIDTH  = 78.0f;
const float GO_BTN_WIDTH   = 48.0f;
const float FONT_SIZE_BTN  = 20.0f;
const float FONT_SIZE_URL  = 19.0f;
const float FONT_SIZE_STS  = 15.0f;

Label MakeNavButton(const char* text)
{
  Label button = Label::New(text);
  button.SetFontSize(FONT_SIZE_BTN);
  button.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  button.SetVerticalTextAlignment(Text::Alignment::CENTER);
  button.SetBackgroundColor(Color::IVORY);
  button.SetTextColor(Color::DARK_BLUE);
  button.SetRequestedWidth(NAV_BTN_WIDTH);
  button.SetRequestedHeight(TOOLBAR_HEIGHT);
  button.SetFocusable(true);
  return button;
}

} // namespace

class BrowserController : public ConnectionTracker
{
public:
  explicit BrowserController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, [this](Application application) {
      OnInit(application);
    });
  }

private:
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    auto posSize = window.GetPositionSize();
    mWindowSize = Vector2(posSize.width, posSize.height);

    DALI_LOG_RELEASE_INFO("[WVLOG][init] OnInit: window=%.0fx%.0f\n", mWindowSize.width, mWindowSize.height);

    BuildUI(window);
    ConnectSignals(window);

    DALI_LOG_RELEASE_INFO("[WVLOG][init] OnInit: navigating to HOME_URL=%s\n", HOME_URL);
    Navigate(Dali::String(HOME_URL));
  }

  void BuildUI(Window& window)
  {
    mBtnBack    = MakeNavButton("Back");
    mBtnForward = MakeNavButton("Forward");
    mBtnReload  = MakeNavButton("Reload");

    mUrlBar = InputField::New();
    mUrlBar.SetPlaceholder("Enter URL");
    mUrlBar.SetPlaceholderColor(Color::GRAY);
    mUrlBar.SetText(Dali::String(HOME_URL));
    mUrlBar.SetFontSize(FONT_SIZE_URL);
    mUrlBar.SetTextColor(Color::DARK_BLUE);
    mUrlBar.SetBackgroundColor(Color::LIGHT_SKY_BLUE);
    mUrlBar.SetCursorWidth(2);
    mUrlBar.SetRequestedWidth(0.0f);
    mUrlBar.SetRequestedHeight(TOOLBAR_HEIGHT);
    mUrlBar.SetPadding(Extents(10, 10, 0, 0));
    mUrlBar.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mUrlBar.SetFocusable(true);
    mUrlBar.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    mBtnGo = Label::New("Go");
    mBtnGo.SetFontSize(FONT_SIZE_BTN);
    mBtnGo.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mBtnGo.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mBtnGo.SetTextColor(Color::WHITE);
    mBtnGo.SetBackgroundColor(Color::DARK_ORANGE);
    mBtnGo.SetRequestedWidth(GO_BTN_WIDTH);
    mBtnGo.SetRequestedHeight(TOOLBAR_HEIGHT);
    mBtnGo.SetFocusable(true);

    StackLayout toolbar = StackLayout::New(StackOrientation::HORIZONTAL);
    toolbar.SetRequestedWidth(MATCH_PARENT);
    toolbar.SetRequestedHeight(TOOLBAR_HEIGHT);
    toolbar.SetSpacing(2.0f);
    toolbar.SetBackgroundColor(Color::AQUA_MARINE);

      toolbar.Add(mBtnBack);
      toolbar.Add(mBtnForward);
      toolbar.Add(mBtnReload);
      toolbar.Add(mUrlBar);
      toolbar.Add(mBtnGo);

    mStatusLabel = Label::New("Ready");
    mStatusLabel.SetFontSize(FONT_SIZE_STS);
    mStatusLabel.SetHorizontalTextAlignment(Text::Alignment::START);
    mStatusLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mStatusLabel.SetTextColor(Color::YELLOW);
    mStatusLabel.SetBackgroundColor(Color::DEEP_PINK);
    mStatusLabel.SetRequestedWidth(MATCH_PARENT);
    mStatusLabel.SetRequestedHeight(STATUS_HEIGHT);
    mStatusLabel.SetPadding(Extents(8, 8, 0, 0));

    float webViewHeight = static_cast<float>(mWindowSize.height) - TOOLBAR_HEIGHT - STATUS_HEIGHT;
    mWebView = WebView::New();
    DALI_LOG_RELEASE_INFO("[WVLOG][init] WebView::New() -> handle=%s\n", mWebView ? "valid" : "EMPTY");
    mWebView.SetRequestedWidth(static_cast<float>(mWindowSize.width));
    mWebView.SetRequestedHeight(webViewHeight);
    mWebView.SetRequestedX(0.0f);
    mWebView.SetRequestedY(TOOLBAR_HEIGHT + STATUS_HEIGHT);

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);

      root.Add(toolbar);
      root.Add(mStatusLabel);
      root.Add(mWebView);

    window.Add(root);
  }

  void ConnectSignals(Window& window)
  {
    mBtnBack.TouchEventSignal().Connect(this, [this](Actor actor, const TouchEvent& touch) {
      return OnBackTouched(actor, touch);
    });
    mBtnForward.TouchEventSignal().Connect(this, [this](Actor actor, const TouchEvent& touch) {
      return OnForwardTouched(actor, touch);
    });
    mBtnReload.TouchEventSignal().Connect(this, [this](Actor actor, const TouchEvent& touch) {
      return OnReloadTouched(actor, touch);
    });
    mBtnGo.TouchEventSignal().Connect(this, [this](Actor actor, const TouchEvent& touch) {
      return OnGoTouched(actor, touch);
    });

    DALI_LOG_RELEASE_INFO("[WVLOG][init] ConnectSignals: connecting 4 WebView signals...\n");
    mWebView.PageLoadStartedSignal().Connect(this, &BrowserController::OnPageLoadStarted);
    mWebView.PageLoadInProgressSignal().Connect(this, &BrowserController::OnPageLoadInProgress);
    mWebView.PageLoadFinishedSignal().Connect(this, &BrowserController::OnPageLoadFinished);
    mWebView.PageLoadErrorSignal().Connect(this, &BrowserController::OnPageLoadError);
    mWebView.UrlChangedSignal().Connect(this, &BrowserController::OnUrlChanged);
    DALI_LOG_RELEASE_INFO("[WVLOG][init] ConnectSignals: connected (started/inProgress/finished/error/urlChanged). If these never fire on load, signals are dead.\n");

    window.KeyEventSignal().Connect(this, [this](Window /*window*/, const KeyEvent& event) {
      OnKeyEvent(event);
    });
  }

  void Navigate(Dali::String url)
  {
    if(!strstr(url.CStr(), "://"))
    {
      url = Dali::String("https://") + url;
    }

    DALI_LOG_RELEASE_INFO("[WVLOG][cmd] Navigate -> WebView.LoadUrl(%s)\n", url.CStr());
    mWebView.LoadUrl(url);
    mUrlBar.SetText(url);
    UpdateNavButtons();
  }

  void UpdateNavButtons()
  {
    bool canBack = mWebView.CanGoBack();
    bool canFwd  = mWebView.CanGoForward();
    DALI_LOG_RELEASE_INFO("[WVLOG][cmd] UpdateNavButtons: CanGoBack=%d CanGoForward=%d isLoading=%d\n", canBack, canFwd, mIsLoading);
    mBtnBack.SetTextColor(canBack ? Color::BLACK : Color::GRAY);
    mBtnForward.SetTextColor(canFwd ? Color::BLACK : Color::GRAY);
    mBtnReload.SetText(mIsLoading ? Dali::String("Stop") : Dali::String("Reload"));
  }

  void SetStatus(const Dali::String& text)
  {
    mStatusLabel.SetText(text);
  }

  bool OnBackTouched(Actor /*actor*/, const TouchEvent& touch)
  {
    DALI_LOG_RELEASE_INFO("[WVLOG][button] Back touched (pointState=%d) canGoBack=%d\n", static_cast<int>(touch.GetState(0)), mWebView.CanGoBack());
    if(mWebView.CanGoBack())
    {
      DALI_LOG_RELEASE_INFO("[WVLOG][cmd] WebView.GoBack()\n");
      mWebView.GoBack();
      UpdateNavButtons();
    }
    return true;
  }

  bool OnForwardTouched(Actor /*actor*/, const TouchEvent& touch)
  {
    DALI_LOG_RELEASE_INFO("[WVLOG][button] Forward touched (pointState=%d) canGoForward=%d\n", static_cast<int>(touch.GetState(0)), mWebView.CanGoForward());
    if(mWebView.CanGoForward())
    {
      DALI_LOG_RELEASE_INFO("[WVLOG][cmd] WebView.GoForward()\n");
      mWebView.GoForward();
      UpdateNavButtons();
    }
    return true;
  }

  bool OnReloadTouched(Actor /*actor*/, const TouchEvent& touch)
  {
    DALI_LOG_RELEASE_INFO("[WVLOG][button] Reload/Stop touched (pointState=%d) isLoading=%d\n", static_cast<int>(touch.GetState(0)), mIsLoading);
    if(mIsLoading)
    {
      DALI_LOG_RELEASE_INFO("[WVLOG][cmd] WebView.StopLoading()\n");
      mWebView.StopLoading();
      mIsLoading = false;
      SetStatus(Dali::String("Stopped"));
    }
    else
    {
      DALI_LOG_RELEASE_INFO("[WVLOG][cmd] WebView.Reload()\n");
      mWebView.Reload();
    }
    UpdateNavButtons();

    return true;
  }

  bool OnGoTouched(Actor /*actor*/, const TouchEvent& touch)
  {
    DALI_LOG_RELEASE_INFO("[WVLOG][button] Go touched (pointState=%d) url=%s\n", static_cast<int>(touch.GetState(0)), mUrlBar.GetText().CStr());
    Navigate(mUrlBar.GetText());
    return true;
  }

  void OnPageLoadStarted(WebView /*view*/, const Dali::String& url)
  {
    DALI_LOG_RELEASE_INFO("[WVLOG][signal] >>>> OnPageLoadStarted FIRED: %s\n", url.CStr());
    mIsLoading = true;
    SetStatus(Dali::String("Loading..."));
    UpdateNavButtons();
  }

  void OnPageLoadInProgress(WebView /*view*/, const Dali::String& url)
  {
    float pct = mWebView.GetLoadProgressPercentage();
    DALI_LOG_RELEASE_INFO("[WVLOG][signal] >>>> OnPageLoadInProgress FIRED: %s (%.0f%%)\n", url.CStr(), pct);
    char  buf[64];
    snprintf(buf, sizeof(buf), "Loading  %.0f%%", pct);
    SetStatus(Dali::String(buf));
  }

  void OnPageLoadFinished(WebView /*view*/, const Dali::String& url)
  {
    mIsLoading = false;
    Dali::String title = mWebView.GetTitle();
    DALI_LOG_RELEASE_INFO("[WVLOG][signal] >>>> OnPageLoadFinished FIRED: %s title=\"%s\"\n", url.CStr(), title.CStr());
    SetStatus(title.Empty() ? url : title);
    UpdateNavButtons();
  }

  void OnPageLoadError(WebView /*view*/, const WebViewPageLoadError& error)
  {
    mIsLoading = false;
    DALI_LOG_RELEASE_INFO("[WVLOG][signal] >>>> OnPageLoadError FIRED: url=%s code=%d desc=\"%s\"\n",
                          error.url.CStr(), static_cast<int>(error.code), error.description.CStr());
    SetStatus(Dali::String("Load error: ") + error.description);
    UpdateNavButtons();
  }

  void OnUrlChanged(WebView /*view*/, const Dali::String& url)
  {
    DALI_LOG_RELEASE_INFO("[WVLOG][signal] >>>> OnUrlChanged FIRED: %s\n", url.CStr());
    mUrlBar.SetText(url);
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
    else if(event.GetKeyName() == "Return")
    {
      DALI_LOG_RELEASE_INFO("[WVLOG][button] Return key -> navigate\n");
      Navigate(mUrlBar.GetText());
    }
    else if(event.GetKeyName() == "C" || event.GetKeyName() == "c")
    {
      DALI_LOG_RELEASE_INFO("[WVLOG][cmd] WebProfile.ClearCache()\n");
      mWebView.GetProfile().ClearCache();
      SetStatus(Dali::String("Cache cleared"));
    }
    else if(event.GetKeyName() == "K" || event.GetKeyName() == "k")
    {
      DALI_LOG_RELEASE_INFO("[WVLOG][cmd] WebCookieManager.ClearAllCookies()\n");
      mWebView.GetProfile().GetCookieManager().ClearAllCookies();
      SetStatus(Dali::String("Cookies cleared"));
    }
  }

private:
  Application& mApplication;
  Size         mWindowSize;

  Label      mBtnBack;
  Label      mBtnForward;
  Label      mBtnReload;
  InputField mUrlBar;
  Label      mBtnGo;

  Label   mStatusLabel;
  WebView mWebView;

  bool mIsLoading{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  DALI_LOG_RELEASE_INFO("[WVLOG][init] main: starting. Watch [WVLOG][signal] lines: if a page loads but no [signal] line appears, WebView signals are dead.\n");
  Application    application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  BrowserController browser(application);
  application.MainLoop();
  return 0;
}
