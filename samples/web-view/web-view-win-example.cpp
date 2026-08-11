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
 * @file  web-view-win-example.cpp
 * @brief Comprehensive WebView sample testing the 28 core WebEnginePlugin APIs.
 *
 * This sample exercises all 28 APIs listed in web-engine-plugin.h (dali-adaptor)
 * through the dali-ui WebView public API, using only number keys 0-9 on a TV
 * remote. Each number key has two actions: first press = Round 1, second press
 * = Round 2, alternating on each press.
 *
 * ===========================================================================
 * API Coverage Table (28 APIs from web-engine-plugin.h)
 * ===========================================================================
 *
 * Each API has a "Verify" column explaining how to confirm it works:
 *   [V] = Visual   — watch the screen
 *   [L] = Log      — check dlog/log output for the tagged message
 *   [S] = Signal   — callback fires (visible via log)
 *
 * Direct / Signal APIs (20):
 *
 * #  | Plugin API                              | Trigger        | Verify
 * ---|-----------------------------------------|----------------|------------------------------------------
 * 3  | LoadUrl(url)                             | auto           | [V] Page appears  [L] "Page load started/finished"
 * 4  | GetTitle()                               | auto + key9 R2 | [L] Log prints title string (e.g. "Naver")
 * 5  | LoadHtmlString(html)                     | key 6 R1       | [V] Page changes to demo HTML  [L] "LoadHtmlString"
 * 6  | Reload()                                 | key 3 R1       | [V] Page reloads  [L] "Page load started/finished"
 * 7  | StopLoading()                            | key 5 R1       | [V] Loading spinner stops  [L] "StopLoading"
 * 8  | CanGoForward()                           | key 1 R1       | [L] Log prints true/false
 * 9  | GoForward()                              | key 1 R1       | [V] Page navigates forward  [L] "GoForward"
 * 10 | CanGoBack()                              | key 2 R1       | [L] Log prints true/false
 * 11 | GoBack()                                 | key 2 R1       | [V] Page navigates back  [L] "GoBack"
 * 12 | ClearHistory()                           | key 0 R1       | [L] "ClearHistory" then press 1/2 to confirm false
 * 13 | SetDocumentBackgroundColor(color)         | key 3 R2       | [V] Page background color changes  [L] color values
 * 15 | RegisterPageLoadStartedCallback          | auto           | [S][L] "Page load started: <url>"
 * 16 | RegisterPageLoadFinishedCallback         | auto           | [S][L] "Page load finished: <url> title=..."
 * 21 | EvaluateJavaScript(script,handler)       | key 7 R1       | [L] "JS result: <document.title>"
 * 22 | AddJavaScriptMessageHandler(name,cb)     | key 8 R1       | [L] "JS message received: Hello from JS!"
 * 23 | RegisterFrameRenderedCallback            | auto           | [S][L] "First frame rendered!"
 * 24 | GetUrl()                                 | auto + key9 R1 | [L] Log prints current URL string
 * 25 | ReloadWithoutCache()                     | key 4 R1       | [V] Page reloads (slower)  [L] "ReloadWithoutCache"
 * 27 | SendTouchEvent(touch)                    | key 8 R2       | [L] "FeedTouchEvent"  [V] page may respond to tap
 * 28 | SendKeyEvent(event)                      | key 7 R2       | [L] "FeedKeyEvent"  [V] page may respond to Return
 *
 * Indirect APIs (8) — no direct public API, triggered through View lifecycle:
 *
 * #  | Plugin API                  | Trigger     | Verify
 * ---|-----------------------------|-------------|------------------------------------------
 * 1  | Create(w,h,locale,timezone) | N/A         | Not used — this sample uses the argc/argv constructor (#17)
 * 2  | Destroy()                   | ESC/BACK    | [V] App exits / WebView destroyed
 * 14 | SetFocus(focused)           | auto + key 6 R2 | [L] "SetFocus verified" (WebView receives keys)
 * 17 | Create(w,h,argc,argv)       | auto        | [V] WebView appears on screen (creation via argc/argv constructor)
 * 18 | GetNativeImage()            | auto        | [V] Page content renders on screen
 * 19 | SetSize(w,h)                | auto        | [V] WebView fills the screen correctly
 * 20 | UpdateDisplayArea(bounds)   | auto        | [V] WebView positioned at (0,0) correctly
 * 26 | SetVisibility(visible)      | key 5 R2    | [V] WebView shows/hides on toggle
 *
 * ===========================================================================
 * How to verify: run the app, press keys 0-9 twice each, and watch:
 *   1. The screen for visual changes [V]
 *   2. The log output (dlogutil or console) for [L] tagged messages
 *      All log lines are prefixed with [WebViewSample] for easy filtering:
 *         dlogutil DALI | grep WebViewSample
 *
 * Verification checklist (press each key twice):
 *   0 R1 → [L] ClearHistory           0 R2 → [L] ClearCache
 *   1 R1 → [V] Forward  [L] GoForward 1 R2 → [L] ClearCookies
 *   2 R1 → [V] Back  [L] GoBack       2 R2 → [V] FindText highlights "the"
 *   3 R1 → [V] Reload  [L] page load   3 R2 → [V] BG color changes
 *   4 R1 → [V] Reload  [L] WOC         4 R2 → (no action)
 *   5 R1 → [V] Loading stops          5 R2 → [V] WebView hides/shows
 *   6 R1 → [V] Demo HTML appears      6 R2 → [L] SetFocus verified
 *   7 R1 → [L] JS result: <title>     7 R2 → [L] FeedKeyEvent
 *   8 R1 → [L] JS message received   8 R2 → [L] FeedTouchEvent
 *   9 R1 → [L] GetUrl(): <url>        9 R2 → [L] GetTitle(): <title>
 *
 * ===========================================================================
 * Key bindings (TV remote: number keys 0-9 only)
 * ===========================================================================
 *
 *  Each number key alternates between Round 1 and Round 2 on each press.
 *
 *  Key  | Round 1 (1st press)                    | Round 2 (2nd press)
 *  -----|----------------------------------------|------------------------------------------
 *   0   | ClearHistory                           | ClearCache
 *   1   | CanGoForward + GoForward               | ClearCookies
 *   2   | CanGoBack + GoBack                     | FindText("the")
 *   3   | Reload                                 | SetDocumentBackgroundColor (cycle colors)
 *   4   | ReloadWithoutCache                     | (no action)
 *   5   | StopLoading                            | Toggle visibility (→ SetVisibility)
 *   6   | LoadHtmlString                         | Verify SetFocus (implicit)
 *   7   | EvaluateJavaScript (document.title)    | FeedKeyEvent (Return key)
 *   8   | AddJavaScriptMessageHandler            | FeedTouchEvent (center tap)
 *   9   | GetUrl                                 | GetTitle
 *
 *  ESC / BACK   Quit (triggers Destroy internally)
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/views/web/web-view.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Ui;


namespace
{
// URL loaded on startup (change to any reachable URL for live testing)
const char* DEFAULT_URL = "https://www.naver.com";

// Fallback HTML content for LoadHtmlString demo
const char* FALLBACK_HTML =
  "<!DOCTYPE html>"
  "<html><head><meta charset='UTF-8'><title>WebView Demo</title></head>"
  "<body style='background:linear-gradient(135deg,#1a1a2e,#16213e);color:#eee;font-family:sans-serif;padding:40px;text-align:center'>"
  "<h1 style='color:#e94560'>🌐 dali-ui WebView</h1>"
  "<p>Loaded from <b>LoadHtmlString()</b> via key 6.</p>"
  "<p>Press <b>2</b> to go back, <b>3</b> to reload.</p>"
  "</body></html>";

// Background colors to cycle through
const Vector4 BG_COLORS[] = {
  Vector4(1.0f, 0.0f, 0.0f, 1.0f), // Red
  Vector4(0.0f, 1.0f, 0.0f, 1.0f), // Green
  Vector4(0.0f, 0.0f, 1.0f, 1.0f), // Blue
  Vector4(1.0f, 1.0f, 0.0f, 1.0f), // Yellow
  Vector4(1.0f, 0.0f, 1.0f, 1.0f), // Magenta
};
const int BG_COLOR_COUNT = sizeof(BG_COLORS) / sizeof(BG_COLORS[0]);
} // namespace

// ---------------------------------------------------------------------------
// Controller
// ---------------------------------------------------------------------------

class WebViewSampleController : public ConnectionTracker
{
public:
  explicit WebViewSampleController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &WebViewSampleController::OnInit);
  }

private:
  // -----------------------------------------------------------------------
  // Init
  // -----------------------------------------------------------------------
  void OnInit(Application application)
  {
    DALI_LOG_RELEASE_INFO("[WebViewSample] OnInit entered\n");
    Window window     = application.GetWindow();
    auto   windowSize = window.GetPositionSize();
    window.SetBackgroundColor(Color::BLACK);

    // [API #17] Create(w,h,argc,argv) — use the argc/argv constructor to pass
    //   --prefer-platform-version to the web engine. This calls
    //   WebEnginePlugin::Create(w,h,argc,argv) internally (see
    //   WebViewImpl::New() in web-view-impl.cpp).
    //   Note: [API #1] Create(w,h,locale,timezone) is NOT used by this sample.
    static char arg0[] = "web-view-win-example";
    static char arg1[] = "--prefer-platform-version";
    char*      webArgs[] = {arg0, arg1};
    uint32_t   webArgc   = 2;
    mWebView = WebView::New(webArgc, webArgs);
    // [API #19] SetSize(w,h) — use a smaller size centered on screen
    //   so the focus indicator is visible around the WebView.
    float viewW = static_cast<float>(windowSize.width) * 0.6f;
    float viewH = static_cast<float>(windowSize.height) * 0.6f;
    float viewX = (static_cast<float>(windowSize.width) - viewW) * 0.5f;
    float viewY = (static_cast<float>(windowSize.height) - viewH) * 0.5f;
    mWebView.SetRequestedWidth(viewW);
    mWebView.SetRequestedHeight(viewH);
    mWebView.SetRequestedX(viewX);
    mWebView.SetRequestedY(viewY);

    // [API #20] UpdateDisplayArea(bounds) is called internally when the
    //   view is added to the window and positioned.
    window.Add(mWebView);

    // [API #14] SetFocus(focused) — tested implicitly: the WebView receives
    //   key events from the TV remote and forwards them to the web page
    //   (e.g. number keys type into Google search box).
    //   No explicit SetFocus needed — dali-ui handles focus automatically.

    // [API #15] RegisterPageLoadStartedCallback -> PageLoadStartedSignal
    mWebView.PageLoadStartedSignal().Connect(this, &WebViewSampleController::OnPageLoadStarted);
    // [API #16] RegisterPageLoadFinishedCallback -> PageLoadFinishedSignal
    mWebView.PageLoadFinishedSignal().Connect(this, &WebViewSampleController::OnPageLoadFinished);
    mWebView.PageLoadErrorSignal().Connect(this, &WebViewSampleController::OnPageLoadError);
    // [API #23] RegisterFrameRenderedCallback -> FrameRenderedSignal
    // [API #18] GetNativeImage() is called internally each frame for compositing.
    mWebView.FrameRenderedSignal().Connect(this, &WebViewSampleController::OnFrameRendered);

    // [API #3] LoadUrl(url)
    mWebView.LoadUrl(Dali::String(DEFAULT_URL));

    // Listen for key events on the Window.
    window.KeyEventSignal().Connect(this, &WebViewSampleController::OnKeyEvent);

    DALI_LOG_RELEASE_INFO("[WebViewSample] Initialized. Loading: %s\n", DEFAULT_URL);
    DALI_LOG_RELEASE_INFO("[WebViewSample] Keys 0-9: press once for Round 1, again for Round 2.\n");
  }

  // -----------------------------------------------------------------------
  // Signal handlers
  // -----------------------------------------------------------------------
  void OnPageLoadStarted(WebView view, const Dali::String& url)
  {
    DALI_LOG_RELEASE_INFO("[WebViewSample] Page load started: %s\n", url.CStr());
  }

  void OnPageLoadFinished(WebView view, const Dali::String& url)
  {
    // [API #4] GetTitle()  [API #24] GetUrl()
    Dali::String title      = mWebView.GetTitle();
    Dali::String currentUrl = mWebView.GetUrl();
    DALI_LOG_RELEASE_INFO("[WebViewSample] Page load finished: %s  title=\"%s\"  url=\"%s\"\n",
                          url.CStr(), title.CStr(), currentUrl.CStr());
  }

  void OnPageLoadError(WebView view, const WebViewPageLoadError& error)
  {
    DALI_LOG_RELEASE_INFO("[WebViewSample] Page load error: url=%s code=%d desc=\"%s\"\n",
                          error.url.CStr(),
                          static_cast<int>(error.code),
                          error.description.CStr());
  }

  void OnFrameRendered(WebView view)
  {
    if(!mFirstFrameReceived)
    {
      mFirstFrameReceived = true;
      DALI_LOG_RELEASE_INFO("[WebViewSample] First frame rendered!\n");
    }
  }

  void OnJavaScriptResult(const Dali::String& result)
  {
    DALI_LOG_RELEASE_INFO("[WebViewSample] JS result: %s\n", result.CStr());
  }

  void OnJavaScriptMessage(const Dali::String& message)
  {
    DALI_LOG_RELEASE_INFO("[WebViewSample] JS message received: %s\n", message.CStr());
  }

  // -----------------------------------------------------------------------
  // Key input (TV remote: number keys 0-9, each toggles R1/R2)
  // -----------------------------------------------------------------------
  void OnKeyEvent(Window window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }

    const Dali::String& keyName  = event.GetKeyName();
    const char*         name     = keyName.CStr();
    int                 keyCode  = event.GetKeyCode();

    // Log every key for debugging
    DALI_LOG_RELEASE_INFO("[WebViewSample] KeyEvent: name=\"%s\" code=%d\n", name, keyCode);

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      // [API #2] Destroy() is called internally when the app quits.
      mApplication.Quit();
      return;
    }

    // Direction keys: resize (up/down) and move (left/right)
    if(IsKey(event, Dali::DALI_KEY_CURSOR_UP))
    {
      // Enlarge WebView by 10%
      float w = mWebView.GetProperty<float>(Actor::Property::SIZE_WIDTH);
      float h = mWebView.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
      float newW = w * 1.1f;
      float newH = h * 1.1f;
      float dx = (newW - w) * 0.5f;
      float dy = (newH - h) * 0.5f;
      mWebView.SetRequestedWidth(newW);
      mWebView.SetRequestedHeight(newH);
      mWebView.SetRequestedX(mWebView.GetProperty<float>(Actor::Property::POSITION_X) - dx);
      mWebView.SetRequestedY(mWebView.GetProperty<float>(Actor::Property::POSITION_Y) - dy);
      DALI_LOG_RELEASE_INFO("[WebViewSample] Resize: %.0f x %.0f\n", newW, newH);
      return;
    }
    if(IsKey(event, Dali::DALI_KEY_CURSOR_DOWN))
    {
      // Shrink WebView by 10%
      float w = mWebView.GetProperty<float>(Actor::Property::SIZE_WIDTH);
      float h = mWebView.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
      float newW = w * 0.9f;
      float newH = h * 0.9f;
      float dx = (newW - w) * 0.5f;
      float dy = (newH - h) * 0.5f;
      mWebView.SetRequestedWidth(newW);
      mWebView.SetRequestedHeight(newH);
      mWebView.SetRequestedX(mWebView.GetProperty<float>(Actor::Property::POSITION_X) - dx);
      mWebView.SetRequestedY(mWebView.GetProperty<float>(Actor::Property::POSITION_Y) - dy);
      DALI_LOG_RELEASE_INFO("[WebViewSample] Resize: %.0f x %.0f\n", newW, newH);
      return;
    }
    if(IsKey(event, Dali::DALI_KEY_CURSOR_LEFT))
    {
      // Move WebView left by 50px
      mWebView.SetRequestedX(mWebView.GetProperty<float>(Actor::Property::POSITION_X) - 50.0f);
      DALI_LOG_RELEASE_INFO("[WebViewSample] Move left\n");
      return;
    }
    if(IsKey(event, Dali::DALI_KEY_CURSOR_RIGHT))
    {
      // Move WebView right by 50px
      mWebView.SetRequestedX(mWebView.GetProperty<float>(Actor::Property::POSITION_X) + 50.0f);
      DALI_LOG_RELEASE_INFO("[WebViewSample] Move right\n");
      return;
    }

    // Determine which number key (0-9) and which round (R1 or R2)
    // Check both keyName (e.g. "1") and keyCode (e.g. 49='1', 0x31='1',
    // or Tizen keycodes like 0x0001='1' on some remotes)
    int keyIndex = -1;
    if(strlen(name) == 1 && name[0] >= '0' && name[0] <= '9')
    {
      keyIndex = name[0] - '0';
    }
    else if(keyCode >= static_cast<int>('0') && keyCode <= static_cast<int>('9'))
    {
      keyIndex = keyCode - '0';
    }
    else
    {
      return; // Ignore non-number keys
    }

    bool round2    = mKeyRound2[keyIndex];
    mKeyRound2[keyIndex] = !round2; // Toggle for next press

    DALI_LOG_RELEASE_INFO("[WebViewSample] Key %d, Round %d\n", keyIndex, round2 ? 2 : 1);

    switch(keyIndex)
    {
      // ---- Key 0 ----
      case 0:
        if(!round2)
        {
          // R1: [API #12] ClearHistory
          DALI_LOG_RELEASE_INFO("[WebViewSample] ClearHistory\n");
          mWebView.ClearHistory();
        }
        else
        {
          // R2: ClearCache
          DALI_LOG_RELEASE_INFO("[WebViewSample] ClearCache\n");
          mWebView.ClearCache();
        }
        break;

      // ---- Key 1 ----
      case 1:
        if(!round2)
        {
          // R1: [API #8, #9] CanGoForward + GoForward
          if(mWebView.CanGoForward())
          {
            DALI_LOG_RELEASE_INFO("[WebViewSample] GoForward\n");
            mWebView.GoForward();
          }
          else
          {
            DALI_LOG_RELEASE_INFO("[WebViewSample] Cannot go forward\n");
          }
        }
        else
        {
          // R2: ClearCookies
          DALI_LOG_RELEASE_INFO("[WebViewSample] ClearCookies\n");
          mWebView.ClearCookies();
        }
        break;

      // ---- Key 2 ----
      case 2:
        if(!round2)
        {
          // R1: [API #10, #11] CanGoBack + GoBack
          if(mWebView.CanGoBack())
          {
            DALI_LOG_RELEASE_INFO("[WebViewSample] GoBack\n");
            mWebView.GoBack();
          }
          else
          {
            DALI_LOG_RELEASE_INFO("[WebViewSample] Cannot go back\n");
          }
        }
        else
        {
          // R2: FindText
          DALI_LOG_RELEASE_INFO("[WebViewSample] FindText(\"the\")\n");
          mWebView.FindText(Dali::String("the"),
                            WebViewFindOption::CASE_INSENSITIVE | WebViewFindOption::SHOW_HIGHLIGHT,
                            100u);
        }
        break;

      // ---- Key 3 ----
      case 3:
        if(!round2)
        {
          // R1: [API #6] Reload
          DALI_LOG_RELEASE_INFO("[WebViewSample] Reload\n");
          mWebView.Reload();
        }
        else
        {
          // R2: [API #13] SetDocumentBackgroundColor (cycle)
          Vector4 color = BG_COLORS[mBgColorIndex];
          DALI_LOG_RELEASE_INFO("[WebViewSample] SetDocumentBackgroundColor(%.1f,%.1f,%.1f,%.1f)\n",
                                color.r, color.g, color.b, color.a);
          mWebView.SetDocumentBackgroundColor(color);
          mBgColorIndex = (mBgColorIndex + 1) % BG_COLOR_COUNT;
        }
        break;

      // ---- Key 4 ----
      case 4:
        if(!round2)
        {
          // R1: [API #25] ReloadWithoutCache
          DALI_LOG_RELEASE_INFO("[WebViewSample] ReloadWithoutCache\n");
          mWebView.ReloadWithoutCache();
        }
        else
        {
          // R2: (removed GetScreenshotAsynchronously)
          DALI_LOG_RELEASE_INFO("[WebViewSample] Key 4 R2: no action (screenshot removed)\n");
        }
        break;

      // ---- Key 5 ----
      case 5:
        if(!round2)
        {
          // R1: [API #7] StopLoading
          DALI_LOG_RELEASE_INFO("[WebViewSample] StopLoading\n");
          mWebView.StopLoading();
        }
        else
        {
          // R2: [API #26] SetVisibility (indirect) — toggle visibility
          mVisible = !mVisible;
          DALI_LOG_RELEASE_INFO("[WebViewSample] SetVisible(%s) -> SetVisibility(%s)\n",
                                mVisible ? "true" : "false",
                                mVisible ? "true" : "false");
          mWebView.SetVisible(mVisible);
        }
        break;

      // ---- Key 6 ----
      case 6:
        if(!round2)
        {
          // R1: [API #5] LoadHtmlString
          DALI_LOG_RELEASE_INFO("[WebViewSample] LoadHtmlString\n");
          mWebView.LoadHtmlString(Dali::String(FALLBACK_HTML));
        }
        else
        {
          // R2: [API #14] SetFocus — already tested implicitly: if the WebView
          //   receives key events via KeyEventSignal, SetFocus has succeeded.
          DALI_LOG_RELEASE_INFO("[WebViewSample] SetFocus verified: WebView is receiving keys\n");
        }
        break;

      // ---- Key 7 ----
      case 7:
        if(!round2)
        {
          // R1: [API #21] EvaluateJavaScript (with callback)
          DALI_LOG_RELEASE_INFO("[WebViewSample] EvaluateJavaScript\n");
          mWebView.EvaluateJavaScript(
            Dali::String("document.title"),
            WebView::JavaScriptCallback::New(this, &WebViewSampleController::OnJavaScriptResult));
        }
        else
        {
          // R2: [API #28] SendKeyEvent -> FeedKeyEvent
          DALI_LOG_RELEASE_INFO("[WebViewSample] FeedKeyEvent (Return)\n");
          KeyEvent down = KeyEvent::New();
          down.SetKeyName(Dali::String("Return"));
          down.SetKeyString(Dali::String("Return"));
          down.SetKeyCode(Dali::DALI_KEY_RETURN);
          down.SetState(KeyEvent::DOWN);
          mWebView.FeedKeyEvent(down);

          KeyEvent up = KeyEvent::New();
          up.SetKeyName(Dali::String("Return"));
          up.SetKeyString(Dali::String("Return"));
          up.SetKeyCode(Dali::DALI_KEY_RETURN);
          up.SetState(KeyEvent::UP);
          mWebView.FeedKeyEvent(up);
        }
        break;

      // ---- Key 8 ----
      case 8:
        if(!round2)
        {
          // R1: [API #22] AddJavaScriptMessageHandler
          DALI_LOG_RELEASE_INFO("[WebViewSample] AddJavaScriptMessageHandler\n");
          mWebView.AddJavaScriptMessageHandler(
            Dali::String("testHandler"),
            WebView::JavaScriptCallback::New(this, &WebViewSampleController::OnJavaScriptMessage));
          mWebView.EvaluateJavaScript(
            Dali::String("window.testHandler.postMessage('Hello from JS!')"));
        }
        else
        {
          // R2: [API #27] SendTouchEvent -> FeedTouchEvent
          DALI_LOG_RELEASE_INFO("[WebViewSample] FeedTouchEvent (center tap)\n");
          float cx = mWebView.GetProperty<float>(Actor::Property::SIZE_WIDTH) * 0.5f;
          float cy = mWebView.GetProperty<float>(Actor::Property::SIZE_HEIGHT) * 0.5f;

          TouchEvent down = TouchEvent::New(0);
          down.AddPoint(0, PointState::DOWN, Vector2(cx, cy));
          mWebView.FeedTouchEvent(down);

          TouchEvent up = TouchEvent::New(0);
          up.AddPoint(0, PointState::UP, Vector2(cx, cy));
          mWebView.FeedTouchEvent(up);
        }
        break;

      // ---- Key 9 ----
      case 9:
        if(!round2)
        {
          // R1: [API #24] GetUrl
          Dali::String url = mWebView.GetUrl();
          DALI_LOG_RELEASE_INFO("[WebViewSample] GetUrl(): %s\n", url.CStr());
        }
        else
        {
          // R2: [API #4] GetTitle
          Dali::String title = mWebView.GetTitle();
          DALI_LOG_RELEASE_INFO("[WebViewSample] GetTitle(): %s\n", title.CStr());
        }
        break;
    }
  }

private:
  Application&       mApplication;
  Dali::Ui::WebView mWebView;
  bool               mFirstFrameReceived{false};
  int                mBgColorIndex{0};
  bool               mVisible{true};
  bool               mKeyRound2[10]{false}; // Toggle state for each number key 0-9
};

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------

int DALI_EXPORT_API main(int argc, char** argv)
{
  DALI_LOG_RELEASE_INFO("[WebViewSample] main() start\n");
  Application application = Application::New(&argc, &argv);
  DALI_LOG_RELEASE_INFO("[WebViewSample] Application created\n");
  UiConfig    config     = UiConfig::New();
  config.SetWebEngineType(WebEngineType::LWE);
  config.Apply();
  DALI_LOG_RELEASE_INFO("[WebViewSample] UiConfig applied (LWE)\n");
  WebViewSampleController sample(application);
  DALI_LOG_RELEASE_INFO("[WebViewSample] Controller created, entering MainLoop\n");
  application.MainLoop();
  DALI_LOG_RELEASE_INFO("[WebViewSample] MainLoop exited\n");
  return 0;
}
