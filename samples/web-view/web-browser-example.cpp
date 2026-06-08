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
#include <dali-ui-foundation/public-api/web-view.h>
#include <dali/integration-api/debug.h>
#include <cstdio>
#include <cstring>

using namespace Dali;
using namespace Dali::Ui;

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
    mWindowSize = window.GetSize();

    BuildUI(window);
    ConnectSignals(window);

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
    mWebView.SetRequestedWidth(static_cast<float>(mWindowSize.width));
    mWebView.SetRequestedHeight(webViewHeight);
    mWebView.SetRequestedPositionX(0.0f);
    mWebView.SetRequestedPositionY(TOOLBAR_HEIGHT + STATUS_HEIGHT);

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
    mBtnBack.TouchedSignal().Connect(this, [this](Actor actor, const TouchEvent& touch) {
      return OnBackTouched(actor, touch);
    });
    mBtnForward.TouchedSignal().Connect(this, [this](Actor actor, const TouchEvent& touch) {
      return OnForwardTouched(actor, touch);
    });
    mBtnReload.TouchedSignal().Connect(this, [this](Actor actor, const TouchEvent& touch) {
      return OnReloadTouched(actor, touch);
    });
    mBtnGo.TouchedSignal().Connect(this, [this](Actor actor, const TouchEvent& touch) {
      return OnGoTouched(actor, touch);
    });

    mWebView.PageLoadStartedSignal().Connect(this, &BrowserController::OnPageLoadStarted);
    mWebView.PageLoadInProgressSignal().Connect(this, &BrowserController::OnPageLoadInProgress);
    mWebView.PageLoadFinishedSignal().Connect(this, &BrowserController::OnPageLoadFinished);
    mWebView.UrlChangedSignal().Connect(this, &BrowserController::OnUrlChanged);

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

    mWebView.LoadUrl(url);
    mUrlBar.SetText(url);
    UpdateNavButtons();
  }

  void UpdateNavButtons()
  {
    mBtnBack.SetTextColor(mWebView.CanGoBack() ? Color::BLACK : Color::GRAY);
    mBtnForward.SetTextColor(mWebView.CanGoForward() ? Color::BLACK : Color::GRAY);
    mBtnReload.SetText(mIsLoading ? Dali::String("Stop") : Dali::String("Reload"));
  }

  void SetStatus(const Dali::String& text)
  {
    mStatusLabel.SetText(text);
  }

  bool OnBackTouched(Actor /*actor*/, const TouchEvent& touch)
  {
    if(mWebView.CanGoBack())
    {
      mWebView.GoBack();
      UpdateNavButtons();
    }
    return true;
  }

  bool OnForwardTouched(Actor /*actor*/, const TouchEvent& touch)
  {
    if(mWebView.CanGoForward())
    {
      mWebView.GoForward();
      UpdateNavButtons();
    }
    return true;
  }

  bool OnReloadTouched(Actor /*actor*/, const TouchEvent& touch)
  {
    if(mIsLoading)
    {
      mWebView.StopLoading();
      mIsLoading = false;
      SetStatus(Dali::String("Stopped"));
    }
    else
    {
      mWebView.Reload();
    }
    UpdateNavButtons();

    return true;
  }

  bool OnGoTouched(Actor /*actor*/, const TouchEvent& touch)
  {
    Navigate(mUrlBar.GetText());
    return true;
  }

  void OnPageLoadStarted(WebView /*view*/, const Dali::String& url)
  {
    mIsLoading = true;
    SetStatus(Dali::String("Loading..."));
    UpdateNavButtons();
    DALI_LOG_RELEASE_INFO("[Browser] Loading: %s\n", url.CStr());
  }

  void OnPageLoadInProgress(WebView /*view*/, const Dali::String& url)
  {
    float pct = mWebView.GetLoadProgressPercentage();
    char  buf[64];
    snprintf(buf, sizeof(buf), "Loading  %.0f%%", pct);
    SetStatus(Dali::String(buf));
  }

  void OnPageLoadFinished(WebView /*view*/, const Dali::String& url)
  {
    mIsLoading = false;
    Dali::String title = mWebView.GetTitle();
    SetStatus(title.Empty() ? url : title);
    UpdateNavButtons();
    DALI_LOG_RELEASE_INFO("[Browser] Loaded: %s  title=\"%s\"\n", url.CStr(), title.CStr());
  }

  void OnUrlChanged(WebView /*view*/, const Dali::String& url)
  {
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
      Navigate(mUrlBar.GetText());
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
  Application    application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  BrowserController browser(application);
  application.MainLoop();
  return 0;
}
