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
 * @file  web-view-example.cpp
 * @brief Basic WebView sample: loads a URL, shows page-load events and title.
 *
 * Key bindings
 *   ESC / BACK   Quit
 *   F            GoForward
 *   B            GoBack
 *   R            Reload
 *   H            LoadHtmlString (inline HTML demo)
 *   L            Load local HTML file (test.html)
 *   J            EvaluateJavaScript (get document.title)
 *   S            GetScreenshot → saved as screenshot.png via PixelData
 *   T            FindText ("the") → highlights matches; count via TextFoundSignal
 *   C            ClearCache (clears the web engine resource cache)
 *   K            ClearCookies (clears all cookies)
 *
 * Page-load failures are reported through PageLoadErrorSignal (see OnPageLoadError).
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

// Local HTML file path (relative to application resource directory)
const char* LOCAL_HTML_PATH = "test.html";

// Fallback HTML content when local file is not available
const char* FALLBACK_HTML =
  "<!DOCTYPE html>"
  "<html><head><meta charset='UTF-8'><title>Offline Mode</title></head>"
  "<body style='background:linear-gradient(135deg,#1a1a2e,#16213e);color:#eee;font-family:sans-serif;padding:40px;text-align:center'>"
  "<h1 style='color:#e94560'>🌐 No Internet Connection</h1>"
  "<p>Unable to connect to the internet.</p>"
  "<p>Please check your network connection and try again.</p>"
  "<hr style='border-color:#333;margin:30px 0'>"
  "<p style='color:#888'>Press <b>R</b> to reload</p>"
  "</body></html>";
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
    Window window     = application.GetWindow();
    auto   windowSize    = window.GetPositionSize();
    window.SetBackgroundColor(Color::BLACK);

    // ---- Create WebView ------------------------------------------------
    mWebView = WebView::New();
    mWebView.SetRequestedWidth(static_cast<float>(windowSize.width));
    mWebView.SetRequestedHeight(static_cast<float>(windowSize.height));
    mWebView.SetRequestedPositionX(0.0f);
    mWebView.SetRequestedPositionY(0.0f);

    window.Add(mWebView);

    // ---- Connect signals ----------------------------------
    mWebView.PageLoadStartedSignal().Connect(this, &WebViewSampleController::OnPageLoadStarted);
    mWebView.PageLoadFinishedSignal().Connect(this, &WebViewSampleController::OnPageLoadFinished);
    mWebView.PageLoadErrorSignal().Connect(this, &WebViewSampleController::OnPageLoadError);
    mWebView.UrlChangedSignal().Connect(this, &WebViewSampleController::OnUrlChanged);
    mWebView.FrameRenderedSignal().Connect(this, &WebViewSampleController::OnFrameRendered);
    mWebView.TextFoundSignal().Connect(this, &WebViewSampleController::OnTextFound);

    // ---- Load initial URL ----------------------------------------------
    mWebView.LoadUrl(Dali::String(DEFAULT_URL));

    // ---- Key & window signals ------------------------------------------
    window.KeyEventSignal().Connect(this, &WebViewSampleController::OnKeyEvent);

    DALI_LOG_RELEASE_INFO("[WebViewSample] Initialized. Loading: %s\n", DEFAULT_URL);
  }

  // -----------------------------------------------------------------------
  // Page-load callbacks (Signal handlers - first param is WebView)
  // -----------------------------------------------------------------------
  void OnPageLoadStarted(WebView view, const Dali::String& url)
  {
    DALI_LOG_RELEASE_INFO("[WebViewSample] Page load started: %s\n", url.CStr());
  }

  void OnPageLoadFinished(WebView view, const Dali::String& url)
  {
    Dali::String title = mWebView.GetTitle();
    DALI_LOG_RELEASE_INFO("[WebViewSample] Page load finished: %s  title=\"%s\"\n",
                          url.CStr(), title.CStr());
  }

  void OnPageLoadError(WebView view, const WebViewPageLoadError& error)
  {
    DALI_LOG_RELEASE_INFO("[WebViewSample] Page load error: url=%s code=%d desc=\"%s\"\n",
                          error.url.CStr(),
                          static_cast<int>(error.code),
                          error.description.CStr());
  }

  bool TryLoadLocalHtml(const Dali::String& path)
  {
    // Try to load from file:// protocol
    // Note: The path should be absolute or relative to app's resource directory
    FILE* file = fopen(path.CStr(), "r");
    if(file)
    {
      fclose(file);
      Dali::String fileUrl = Dali::String("file://") + path;
      mWebView.LoadUrl(fileUrl);
      return true;
    }
    return false;
  }

  void OnUrlChanged(WebView view, const Dali::String& url)
  {
    DALI_LOG_RELEASE_INFO("[WebViewSample] URL changed: %s\n", url.CStr());
  }

  void OnFrameRendered(WebView view)
  {
    // Called each time a new frame from the web engine is composited.
    // We only log the very first frame to avoid flooding the log.
    if(!mFirstFrameReceived)
    {
      mFirstFrameReceived = true;
      DALI_LOG_RELEASE_INFO("[WebViewSample] First frame rendered!\n");
    }
  }

  void OnTextFound(WebView view, uint32_t matchCount)
  {
    DALI_LOG_RELEASE_INFO("[WebViewSample] FindText matches: %u\n", matchCount);
  }

  // -----------------------------------------------------------------------
  // Key input
  // -----------------------------------------------------------------------
  void OnKeyEvent(Window window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
    else if(event.GetKeyName() == "F" || event.GetKeyName() == "f")
    {
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
    else if(event.GetKeyName() == "B" || event.GetKeyName() == "b")
    {
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
    else if(event.GetKeyName() == "R" || event.GetKeyName() == "r")
    {
      DALI_LOG_RELEASE_INFO("[WebViewSample] Reload\n");
      mWebView.Reload();
    }
    else if(event.GetKeyName() == "H" || event.GetKeyName() == "h")
    {
      DALI_LOG_RELEASE_INFO("[WebViewSample] LoadHtmlString\n");
      mWebView.LoadHtmlString(Dali::String(
        "<html><body style='background:#1a1a2e;color:#eee;font-family:sans-serif;padding:40px'>"
        "<h1>dali-ui WebView</h1>"
        "<p>Loaded from <b>LoadHtmlString()</b>.</p>"
        "<p>Press <b>B</b> to go back, <b>R</b> to reload.</p>"
        "</body></html>"));
    }
    else if(event.GetKeyName() == "L" || event.GetKeyName() == "l")
    {
      // Load local HTML file
      DALI_LOG_RELEASE_INFO("[WebViewSample] Load local HTML\n");
      Dali::String localPath = Dali::String(LOCAL_HTML_PATH);
      if(TryLoadLocalHtml(localPath))
      {
        DALI_LOG_RELEASE_INFO("[WebViewSample] Loaded local HTML: %s\n", localPath.CStr());
      }
      else
      {
        DALI_LOG_RELEASE_INFO("[WebViewSample] Local HTML not found, using fallback\n");
        mWebView.LoadHtmlString(Dali::String(FALLBACK_HTML));
      }
    }
    else if(event.GetKeyName() == "J" || event.GetKeyName() == "j")
    {
      // Evaluate JavaScript and log result
      DALI_LOG_RELEASE_INFO("[WebViewSample] EvaluateJavaScript\n");
      mWebView.EvaluateJavaScript(
        Dali::String("document.title"),
        WebView::JavaScriptCallback::New(this, &WebViewSampleController::OnJavaScriptResult));
    }
    else if(event.GetKeyName() == "S" || event.GetKeyName() == "s")
    {
      DALI_LOG_RELEASE_INFO("[WebViewSample] GetScreenshotAsynchronously\n");
      Dali::BoundsInteger viewArea(0, 0,
                                   static_cast<int32_t>(mWebView.GetProperty<float>(Actor::Property::SIZE_WIDTH)),
                                   static_cast<int32_t>(mWebView.GetProperty<float>(Actor::Property::SIZE_HEIGHT)));
      mWebView.GetScreenshotAsynchronously(
        viewArea,
        /*scaleFactor=*/1.0f,
        WebView::ScreenshotCapturedCallback::New(this, &WebViewSampleController::OnScreenshotCaptured));
    }
    else if(event.GetKeyName() == "T" || event.GetKeyName() == "t")
    {
      // Find & highlight text; the match count is reported via TextFoundSignal -> OnTextFound.
      DALI_LOG_RELEASE_INFO("[WebViewSample] FindText(\"the\")\n");
      mWebView.FindText(Dali::String("the"),
                        WebViewFindOption::CASE_INSENSITIVE | WebViewFindOption::SHOW_HIGHLIGHT,
                        /*maxMatchCount=*/100u);
    }
    else if(event.GetKeyName() == "C" || event.GetKeyName() == "c")
    {
      DALI_LOG_RELEASE_INFO("[WebViewSample] ClearCache\n");
      mWebView.ClearCache();
    }
    else if(event.GetKeyName() == "K" || event.GetKeyName() == "k")
    {
      DALI_LOG_RELEASE_INFO("[WebViewSample] ClearCookies\n");
      mWebView.ClearCookies();
    }
  }

  // -----------------------------------------------------------------------
  // JavaScript result callback
  // -----------------------------------------------------------------------
  void OnJavaScriptResult(const Dali::String& result)
  {
    DALI_LOG_RELEASE_INFO("[WebViewSample] JS result: %s\n", result.CStr());
  }

  // -----------------------------------------------------------------------
  // Screenshot callback
  // -----------------------------------------------------------------------
  void OnScreenshotCaptured(Dali::Ui::ImageView screenshot)
  {
    if(!screenshot)
    {
      DALI_LOG_ERROR("[WebViewSample] Screenshot failed\n");
      return;
    }
    DALI_LOG_RELEASE_INFO("[WebViewSample] Screenshot captured! URL=%s\n",
                          screenshot.GetResourceUrl().CStr());
  }

private:
  Application&     mApplication;
  Dali::Ui::WebView mWebView;
  bool              mFirstFrameReceived{false};
};

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application           application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  WebViewSampleController sample(application);
  application.MainLoop();
  return 0;
}
