# DALi UI Foundation - WebView

[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/WebView-(kr))

`WebView` is a View for displaying web content. It enables embedding web pages and HTML content directly in your application, with support for navigation, JavaScript interaction, and various web engine features.

---

## Table of Contents

1. [Basic Setup](#1-basic-setup)
2. [Loading Content](#2-loading-content)
3. [Navigation](#3-navigation)
4. [JavaScript Integration](#4-javascript-integration)
5. [Event Handling & Signals](#5-event-handling--signals)
6. [User Input & Events](#6-user-input--events)
7. [Page Information & Queries](#7-page-information--queries)
8. [Rendering & Display](#8-rendering--display)
9. [Text Search](#9-text-search)
10. [Cache & Cookies](#10-cache--cookies)
11. [Important Notes](#11-important-notes)

---

## 1. Basic Setup

Create a `WebView` using the static factory method `New()`.

```cpp
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/views/web/web-view.h>
#include <dali/integration-api/debug.h>

using namespace Dali::Ui;

// Create an empty WebView
WebView webView = WebView::New();

// Load a URL
webView.LoadUrl("https://www.example.com");

// Set size
webView.SetRequestedWidth(800.0f);
webView.SetRequestedHeight(600.0f);

// Add to the scene
window.Add(webView);
```

Run this code after application initialization, with `window` referring to the application's `Dali::Window`. The later examples use these headers and `webView`. The umbrella header does not include WebView, so include its header explicitly.

### Selecting a Web Engine

WebView uses Chromium by default. To use another web engine, configure it in `UiConfig` at application startup.

```cpp
UiConfig config = UiConfig::New();
config.SetWebEngineType(WebEngineType::LWE);
config.Apply();
```

---

## 2. Loading Content

### Load URL

```cpp
webView.LoadUrl("https://www.example.com");
```

### Load HTML String

```cpp
Dali::String html = "<html><body><h1>Hello World</h1></body></html>";
webView.LoadHtmlString(html);
```

### Load HTML with Base URI (Override Current Entry)

```cpp
webView.LoadHtmlStringOverrideCurrentEntry(
  "<html><body>Content</body></html>",
  "https://base.example.com",
  "https://unreachable.example.com"
);
```

### Load Raw Content

```cpp
const char htmlContent[] = "<html><body>Raw content</body></html>";
const int8_t* content = reinterpret_cast<const int8_t*>(htmlContent);
const uint32_t contentSize = static_cast<uint32_t>(sizeof(htmlContent) - 1);
webView.LoadContents(
  content,
  contentSize,
  "text/html",
  "UTF-8",
  "https://base.uri.com"
);
```

---

## 3. Navigation

Control page navigation with these methods:

| Method | Description |
|---|---|
| `GoForward()` | Navigate to the next page in history (if available). |
| `GoBack()` | Navigate to the previous page in history (if available). |
| `CanGoForward()` | Check if forward navigation is possible. |
| `CanGoBack()` | Check if backward navigation is possible. |
| `ClearHistory()` | Clear the entire navigation history. |
| `Reload()` | Reload the current page. |
| `ReloadWithoutCache()` | Reload without using cached resources. |
| `StopLoading()` | Stop the current page load. |

```cpp
if (webView.CanGoBack())
{
  webView.GoBack();
}

webView.Reload();
webView.StopLoading();
```

---

## 4. JavaScript Integration

Use the callback type expected by the API, such as `WebView::JavaScriptCallback::New(...)`. A noncapturing lambda can be passed to `New(...)`. For instance state, bind a member function with `New(this, &Controller::Method)`, as shown in the dialog example below.

### Evaluate JavaScript

Execute JavaScript and optionally receive the result:

```cpp
// Execute without result
webView.EvaluateJavaScript("console.log('Hello from C++');");

// Execute and receive result via callback
webView.EvaluateJavaScript(
  "document.title",
  WebView::JavaScriptCallback::New(
    [](const Dali::String& result) {
      DALI_LOG_RELEASE_INFO("Title: %s\n", result.CStr());
    })
);
```

### JavaScript Message Handler

Register a handler to receive messages from JavaScript:

```cpp
webView.AddJavaScriptMessageHandler(
  "nativeObject",
  WebView::JavaScriptCallback::New(
    [](const Dali::String& message) {
      DALI_LOG_RELEASE_INFO("Message from JS: %s\n", message.CStr());
    })
);
```

JavaScript can then send messages:

```javascript
// In the web page
if (window.nativeObject) {
  window.nativeObject.postMessage("Hello from JavaScript");
}
```

```cpp
// Remove the handler when it is no longer needed
webView.RemoveJavaScriptMessageHandler("nativeObject");
```

### JavaScript Dialogs

Handle JavaScript alert, confirm, and prompt dialogs:

Use a controller whose member functions reply through the WebView. Construct it with the existing `webView` before loading the page, and keep it alive for as long as the WebView has these callbacks registered. This example replies to alerts immediately, accepts confirms, and returns fixed text for prompts.

```cpp
class WebViewDialogController
{
public:
  explicit WebViewDialogController(WebView webView)
  : mWebView(webView)
  {
    mWebView.RegisterJavaScriptAlertCallback(
      WebView::JavaScriptAlertCallback::New(this, &WebViewDialogController::OnAlert));
    mWebView.RegisterJavaScriptConfirmCallback(
      WebView::JavaScriptConfirmCallback::New(this, &WebViewDialogController::OnConfirm));
    mWebView.RegisterJavaScriptPromptCallback(
      WebView::JavaScriptPromptCallback::New(this, &WebViewDialogController::OnPrompt));
  }

private:
  bool OnAlert(const Dali::String& message)
  {
    DALI_LOG_RELEASE_INFO("Alert: %s\n", message.CStr());
    mWebView.JavaScriptAlertReply();
    return true;
  }

  bool OnConfirm(const Dali::String& message)
  {
    DALI_LOG_RELEASE_INFO("Confirm: %s\n", message.CStr());
    mWebView.JavaScriptConfirmReply(true); // or false
    return true;
  }

  bool OnPrompt(const Dali::String& message, const Dali::String& defaultText)
  {
    DALI_LOG_RELEASE_INFO("Prompt: %s, Default: %s\n", message.CStr(), defaultText.CStr());
    mWebView.JavaScriptPromptReply("user input");
    return true;
  }

  WebView mWebView;
};
```

---

## 5. Event Handling & Signals

Connect to WebView signals to handle page load events and other notifications:

```cpp
webView.PageLoadStartedSignal().Connect(
  [](WebView view, const Dali::String& url) {
    DALI_LOG_RELEASE_INFO("Page load started: %s\n", url.CStr());
  }
);

webView.PageLoadInProgressSignal().Connect(
  [](WebView view, const Dali::String& url) {
    // Called periodically during loading
    DALI_LOG_RELEASE_INFO("Loading in progress...\n");
  }
);

webView.PageLoadFinishedSignal().Connect(
  [](WebView view, const Dali::String& url) {
    DALI_LOG_RELEASE_INFO("Page load finished: %s\n", url.CStr());
  }
);

webView.PageLoadErrorSignal().Connect(
  [](WebView view, const WebViewPageLoadError& error) {
    DALI_LOG_ERROR("Load error: %s\n", error.GetDescription().CStr());
  }
);

webView.UrlChangedSignal().Connect(
  [](WebView view, const Dali::String& url) {
    DALI_LOG_RELEASE_INFO("URL changed: %s\n", url.CStr());
  }
);
```

### Available Signals

| Signal | Description |
|---|---|
| `PageLoadStartedSignal()` | Emitted when page load starts. |
| `PageLoadInProgressSignal()` | Emitted during page loading. |
| `PageLoadFinishedSignal()` | Emitted when page load completes. |
| `PageLoadErrorSignal()` | Emitted when a page load error occurs. |
| `UrlChangedSignal()` | Emitted when the URL changes. |
| `FrameRenderedSignal()` | Emitted after a frame is rendered. |
| `ScrollEdgeReachedSignal()` | Emitted when scroll reaches an edge. |
| `OverScrolledSignal()` | Emitted when over-scroll occurs. |
| `FullscreenEnteredSignal()` | Emitted when entering fullscreen mode. |
| `FullscreenExitedSignal()` | Emitted when exiting fullscreen mode. |
| `TextFoundSignal()` | Emitted when text is found (see FindText). |
| `GeolocationPermissionSignal()` | Emitted for geolocation permission requests. |
| `WebProcessCrashedSignal()` | Emitted when the web process crashes. |

---

## 6. User Input & Events

### Mouse and Key Events

Enable or disable mouse and keyboard event handling:

```cpp
webView.SetMouseEventsEnabled(true);   // Enable mouse events (default)
webView.SetKeyEventsEnabled(true);     // Enable key events (default)

bool mouseEnabled = webView.IsMouseEventsEnabled();
bool keyEnabled   = webView.IsKeyEventsEnabled();
```

### Feed Events Manually

Forward `keyEvent` (`Dali::KeyEvent`) and `touchEvent` (`Dali::TouchEvent`) received by your input handlers to the web engine:

```cpp
// Forward a key event
bool keyConsumed = webView.FeedKeyEvent(keyEvent);

// Forward a touch event
bool touchConsumed = webView.FeedTouchEvent(touchEvent);

// Send a mouse wheel event
webView.FeedMouseWheel(true, 3, 100, 200); // yDirection, step, x, y
```

### Scroll

```cpp
webView.ScrollBy(10, 20);      // Scroll by delta
webView.ScrollEdgeBy(0, -50);  // Scroll with edge detection
```

---

## 7. Page Information & Queries

### URL and Title

```cpp
Dali::String currentUrl = webView.GetUrl();
Dali::String pageTitle  = webView.GetTitle();
```

### User Agent

```cpp
webView.SetUserAgent("Custom User Agent String");
Dali::String userAgent = webView.GetUserAgent();
```

### Load Progress

```cpp
float progress = webView.GetLoadProgressPercentage(); // 0.0 to 100.0
```

### Zoom Factors

```cpp
// Page zoom (entire page scaling)
webView.SetPageZoomFactor(1.2f);
float pageZoom = webView.GetPageZoomFactor();

// Text zoom (only text scaling)
webView.SetTextZoomFactor(1.1f);
float textZoom = webView.GetTextZoomFactor();
```

### Scale Factor

```cpp
webView.SetScaleFactor(2.0f, Dali::Vector2(100.0f, 100.0f)); // scale, pivot point
float scale = webView.GetScaleFactor();
```

### Content Queries

```cpp
Dali::Vector2 scrollPos  = webView.GetScrollPosition();
Dali::Vector2 scrollSize = webView.GetScrollSize();
Dali::Vector2 contentSize = webView.GetContentSize();
Dali::String selectedText = webView.GetSelectedText();
```

---

## 8. Rendering & Display

### Document Background Color

```cpp
webView.SetDocumentBackgroundColor(Dali::Vector4(1.0f, 1.0f, 1.0f, 1.0f)); // RGBA
```

### Video Hole

Enable a transparent area for video rendering over the web content:

```cpp
webView.SetVideoHoleEnabled(true);
bool videoHoleEnabled = webView.IsVideoHoleEnabled();
```

### Tile Management

Optimize rendering performance for large pages:

```cpp
webView.SetTilesClearedWhenHidden(true);     // Clear tiles when view is hidden
webView.SetTileCoverAreaMultiplier(2.0f);    // Multiplier for tile cover area
webView.ClearAllTilesResources();            // Clear all tile resources manually
```

### Suspend & Resume

Pause or resume web engine processing:

```cpp
webView.Suspend();
webView.Resume();

webView.SuspendNetworkLoading();
webView.ResumeNetworkLoading();
```

### Screenshot

Capture the current web view content as an image:

```cpp
// Synchronous screenshot
Dali::BoundsInteger area(0, 0, 800, 600);
Dali::Ui::ImageView screenshot = webView.GetScreenshot(area, 1.0f);

// Asynchronous screenshot with callback
webView.GetScreenshotAsynchronously(
  area,
  1.0f,
  WebView::ScreenshotCapturedCallback::New(
    [](Dali::Ui::ImageView image) {
      // Use the screenshot
      DALI_LOG_RELEASE_INFO("Screenshot captured\n");
    })
);
```

### Favicon

```cpp
Dali::Ui::ImageView favicon = webView.GetFavicon();
```

### Page Plain Text

Retrieve all visible text from the page asynchronously:

```cpp
webView.GetPlainTextAsynchronously(
  WebView::PlainTextCallback::New(
    [](const Dali::String& text) {
      DALI_LOG_RELEASE_INFO("Plain text: %s\n", text.CStr());
    })
);
```

### Video Playing Status

Check if video is currently playing on the page:

```cpp
webView.CheckVideoPlayingAsynchronously(
  WebView::VideoPlayingCallback::New(
    [](bool isPlaying) {
      DALI_LOG_RELEASE_INFO("Video playing: %s\n", isPlaying ? "yes" : "no");
    })
);
```

---

## 9. Text Search

Search for and highlight text within the page:

```cpp
// Search with case-insensitive, show highlight
uint32_t maxMatches = 100;
bool result = webView.FindText(
  "search term",
  Dali::Ui::WebViewFindOption::CASE_INSENSITIVE |
  Dali::Ui::WebViewFindOption::SHOW_HIGHLIGHT,
  maxMatches
);

// Receive results via signal
webView.TextFoundSignal().Connect(
  [](WebView view, uint32_t matchCount) {
    DALI_LOG_RELEASE_INFO("Found %u matches\n", matchCount);
  }
);
```

### Find Options

| Option | Description |
|---|---|
| `NONE` | No options. |
| `CASE_INSENSITIVE` | Ignore case when matching. |
| `AT_WORD_STARTS` | Match at word boundaries. |
| `TREAT_MEDIAL_CAPITAL_AS_WORD_START` | Capital letters in the middle are treated as word starts. |
| `BACKWARDS` | Search backwards through the page. |
| `WRAP_AROUND` | Wrap search to the beginning/end of the page. |
| `SHOW_OVERLAY` | Show an overlay with the results. |
| `SHOW_FIND_INDICATOR` | Show a find indicator. |
| `SHOW_HIGHLIGHT` | Highlight matching text. |

---

## 10. Cache & Cookies

### Cache Management

```cpp
// Clear the resource cache (shared across all WebViews with the same profile)
webView.GetProfile().ClearCache();

// Convenience shortcut for the same operation
webView.ClearCache();

// Reload without cache
webView.ReloadWithoutCache();
```

### Cookie Management

```cpp
// Clear all cookies (shared across all WebViews with the same profile)
webView.GetProfile().GetCookieManager().ClearAllCookies();

// Convenience shortcut for the same operation
webView.ClearCookies();
```

### Custom Headers

Add or remove custom HTTP headers for requests:

```cpp
webView.AddCustomHeader("X-Custom-Header", "CustomValue");
webView.RemoveCustomHeader("X-Custom-Header");
```

### Extra Feature Settings

Use the generic string setting path for features that do not have a dedicated API:

```cpp
WebSettings settings = webView.GetSettings();
settings.SetExtraFeatureValue("featureName", "featureValue");
Dali::String value = settings.GetExtraFeatureValue("featureName");
```

---

## 11. Important Notes

- Apply `UiConfig` before creating the first `WebView`.
- Select the web engine before calling `UiConfig::Apply()`. Chromium is used by default.
- Web engine capabilities may differ by platform and engine implementation.

---

## Guide Reference

For detailed guidance on WebView APIs and use cases, see the [API Reference](https://pages.github.sec.samsung.net/NUI/dali-ui/).
