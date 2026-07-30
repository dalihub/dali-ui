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

/**
 * @file  web-view-win-test-example.cpp
 * @brief Windows LWE backend test harness: buttons switch the WebView between
 *        google.com and the local GC-stress page (res/gc-stress.html).
 *
 * The GC-stress page churns JS objects (40 properties each) and canvas image
 * data so Escargot/BDWGC regressions in the Windows Debug Starfish build
 * surface within seconds instead of depending on whichever external site
 * happens to be loaded.
 *
 * Buttons  Google | GC Stress | Test Page | Reload
 * Keys     ESC/BACK quit, G google, T stress page, L local test page, R reload
 */

#if !defined(_WIN32)
#include <cstdio>

// This sample exercises the Windows LWE web-engine backend only.
int main(int, char**)
{
  std::printf("web-view-win-test.example is a Windows-only sample.\n");
  return 0;
}

#else // _WIN32

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/views/web/web-view.h>
#include <dali/integration-api/debug.h>

#include <cstdlib>
#include <cstring>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
const char* GOOGLE_URL = "https://www.google.com";

// Relative to the executable directory (the sample runtime sets the working
// directory there before main()).
#if defined(RESOURCES_DIR)
const char* STRESS_HTML_PATH = RESOURCES_DIR "gc-stress.html";
const char* TEST_HTML_PATH   = RESOURCES_DIR "test.html";
#else
const char* STRESS_HTML_PATH = "gc-stress.html";
const char* TEST_HTML_PATH   = "test.html";
#endif

const float TOOLBAR_HEIGHT = 52.0f;
const float STATUS_HEIGHT  = 28.0f;
const float NAV_BTN_WIDTH  = 130.0f;
const float FONT_SIZE_BTN  = 20.0f;
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

// LWE needs an absolute file URL; a "file://../..." relative form is not
// resolved. Expand the runtime-relative resource path against the current
// working directory (the executable directory).
Dali::String MakeResourceFileUrl(const char* relativePath)
{
  char absolute[_MAX_PATH] = {};
  if(!_fullpath(absolute, relativePath, sizeof(absolute)))
  {
    return Dali::String("file:///") + relativePath;
  }
  for(char* p = absolute; *p; ++p)
  {
    if(*p == '\\')
    {
      *p = '/';
    }
  }
  return Dali::String("file:///") + absolute;
}

} // namespace

class WebViewWinTestController : public ConnectionTracker
{
public:
  explicit WebViewWinTestController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &WebViewWinTestController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window  = application.GetWindow();
    auto   posSize = window.GetPositionSize();
    window.SetBackgroundColor(Color::BLACK);

    BuildUI(window, Vector2(static_cast<float>(posSize.width), static_cast<float>(posSize.height)));
    ConnectSignals(window);

    mStressPageUrl = MakeResourceFileUrl(STRESS_HTML_PATH);
    mTestPageUrl   = MakeResourceFileUrl(TEST_HTML_PATH);
    DALI_LOG_RELEASE_INFO("[WebViewWinTest] Initialized. stressPage=%s testPage=%s\n",
                          mStressPageUrl.CStr(), mTestPageUrl.CStr());
    Navigate(Dali::String(GOOGLE_URL));
  }

  void BuildUI(Window& window, Vector2 windowSize)
  {
    mBtnGoogle   = MakeNavButton("Google");
    mBtnStress   = MakeNavButton("GC Stress");
    mBtnTestPage = MakeNavButton("Test Page");
    mBtnReload   = MakeNavButton("Reload");

    StackLayout toolbar = StackLayout::New(StackOrientation::HORIZONTAL);
    toolbar.SetRequestedWidth(MATCH_PARENT);
    toolbar.SetRequestedHeight(TOOLBAR_HEIGHT);
    toolbar.SetSpacing(2.0f);
    toolbar.SetBackgroundColor(Color::AQUA_MARINE);

    toolbar.Add(mBtnGoogle);
    toolbar.Add(mBtnStress);
    toolbar.Add(mBtnTestPage);
    toolbar.Add(mBtnReload);

    mStatusLabel = Label::New("Ready");
    mStatusLabel.SetFontSize(FONT_SIZE_STS);
    mStatusLabel.SetHorizontalTextAlignment(Text::Alignment::START);
    mStatusLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mStatusLabel.SetTextColor(Color::YELLOW);
    mStatusLabel.SetBackgroundColor(Color::DARK_SLATE_GRAY);
    mStatusLabel.SetRequestedWidth(MATCH_PARENT);
    mStatusLabel.SetRequestedHeight(STATUS_HEIGHT);
    mStatusLabel.SetPadding(Extents(8, 8, 0, 0));

    mWebView = WebView::New();
    mWebView.SetRequestedWidth(windowSize.width);
    mWebView.SetRequestedHeight(windowSize.height - TOOLBAR_HEIGHT - STATUS_HEIGHT);
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
    mBtnGoogle.TouchEventSignal().Connect(this, [this](Actor, const TouchEvent& touch) {
      if(touch.GetState(0) == PointState::DOWN)
      {
        Navigate(Dali::String(GOOGLE_URL));
      }
      return true;
    });
    mBtnStress.TouchEventSignal().Connect(this, [this](Actor, const TouchEvent& touch) {
      if(touch.GetState(0) == PointState::DOWN)
      {
        Navigate(mStressPageUrl);
      }
      return true;
    });
    mBtnTestPage.TouchEventSignal().Connect(this, [this](Actor, const TouchEvent& touch) {
      if(touch.GetState(0) == PointState::DOWN)
      {
        Navigate(mTestPageUrl);
      }
      return true;
    });
    mBtnReload.TouchEventSignal().Connect(this, [this](Actor, const TouchEvent& touch) {
      if(touch.GetState(0) == PointState::DOWN)
      {
        DALI_LOG_RELEASE_INFO("[WebViewWinTest] Reload\n");
        mWebView.Reload();
      }
      return true;
    });

    mWebView.PageLoadStartedSignal().Connect(this, &WebViewWinTestController::OnPageLoadStarted);
    mWebView.PageLoadFinishedSignal().Connect(this, &WebViewWinTestController::OnPageLoadFinished);
    mWebView.PageLoadErrorSignal().Connect(this, &WebViewWinTestController::OnPageLoadError);
    mWebView.UrlChangedSignal().Connect(this, &WebViewWinTestController::OnUrlChanged);

    window.KeyEventSignal().Connect(this, &WebViewWinTestController::OnKeyEvent);
  }

  void Navigate(const Dali::String& url)
  {
    DALI_LOG_RELEASE_INFO("[WebViewWinTest] LoadUrl: %s\n", url.CStr());
    SetStatus(Dali::String("Loading: ") + url);
    mWebView.LoadUrl(url);
  }

  void SetStatus(const Dali::String& text)
  {
    mStatusLabel.SetText(text);
  }

  void OnPageLoadStarted(WebView, const Dali::String& url)
  {
    DALI_LOG_RELEASE_INFO("[WebViewWinTest] Page load started: %s\n", url.CStr());
  }

  void OnPageLoadFinished(WebView, const Dali::String& url)
  {
    Dali::String title = mWebView.GetTitle();
    DALI_LOG_RELEASE_INFO("[WebViewWinTest] Page load finished: %s title=\"%s\"\n", url.CStr(), title.CStr());
    SetStatus(title.Empty() ? url : title);
  }

  void OnPageLoadError(WebView, const WebViewPageLoadError& error)
  {
    DALI_LOG_RELEASE_INFO("[WebViewWinTest] Page load error: url=%s code=%d desc=\"%s\"\n",
                          error.url.CStr(),
                          static_cast<int>(error.code),
                          error.description.CStr());
    SetStatus(Dali::String("Load error: ") + error.description);
  }

  void OnUrlChanged(WebView, const Dali::String& url)
  {
    DALI_LOG_RELEASE_INFO("[WebViewWinTest] URL changed: %s\n", url.CStr());
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
    else if(event.GetKeyName() == "G" || event.GetKeyName() == "g")
    {
      Navigate(Dali::String(GOOGLE_URL));
    }
    else if(event.GetKeyName() == "T" || event.GetKeyName() == "t")
    {
      Navigate(mStressPageUrl);
    }
    else if(event.GetKeyName() == "L" || event.GetKeyName() == "l")
    {
      Navigate(mTestPageUrl);
    }
    else if(event.GetKeyName() == "R" || event.GetKeyName() == "r")
    {
      DALI_LOG_RELEASE_INFO("[WebViewWinTest] Reload\n");
      mWebView.Reload();
    }
  }

private:
  Application& mApplication;

  Label mBtnGoogle;
  Label mBtnStress;
  Label mBtnTestPage;
  Label mBtnReload;
  Label mStatusLabel;

  WebView      mWebView;
  Dali::String mStressPageUrl;
  Dali::String mTestPageUrl;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);

  UiConfig config = UiConfig::New();
  // The Windows web-engine backend is the LWE plugin
  // (dali2-web-engine-lwe-plugin-win.dll). UiConfig defaults to CHROMIUM,
  // which does not exist on Windows, so selecting LWE here is required.
  config.SetWebEngineType(WebEngineType::LWE);
  config.Apply();

  WebViewWinTestController sample(application);
  application.MainLoop();
  return 0;
}

#endif // _WIN32
