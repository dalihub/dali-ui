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
std::string html = "<html><body><h1>Hello World</h1></body></html>";
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
const int8_t* content = reinterpret_cast<const int8_t*>("raw data");
webView.LoadContents(
  content,
  content_size,
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

### Evaluate JavaScript

Execute JavaScript and optionally receive the result:

```cpp
// Execute without result
webView.EvaluateJavaScript("console.log('Hello from C++');");

// Execute and receive result via callback
webView.EvaluateJavaScript(
  "document.title",
  [](const Dali::String& result) {
    Dali::DALI_LOG_INFO("Title: %s", result.c_str());
  }
);
```

### JavaScript Message Handler

Register a handler to receive messages from JavaScript:

```cpp
webView.AddJavaScriptMessageHandler(
  "nativeObject",
  [](const Dali::String& message) {
    Dali::DALI_LOG_INFO("Message from JS: %s", message.c_str());
  }
  );
```

JavaScript can then send messages:

```javascript
// In the web page
if (window.nativeObject) {
  window.nativeObject("Hello from JavaScript");
}
```

### JavaScript Dialogs

Handle JavaScript alert, confirm, and prompt dialogs:

```cpp
// Alert callback
webView.RegisterJavaScriptAlertCallback(
  [webView](const Dali::String& message) {
    Dali::DALI_LOG_INFO("Alert: %s", message.c_str());
    webView.JavaScriptAlertReply();
    return true;
  }
);

// Confirm callback
webView.RegisterJavaScriptConfirmCallback(
  [webView](const Dali::String& message) {
    Dali::DALI_LOG_INFO("Confirm: %s", message.c_str());
    webView.JavaScriptConfirmReply(true); // or false
    return true;
  }
);

// Prompt callback
webView.RegisterJavaScriptPromptCallback(
  [webView](const Dali::String& message, const Dali::String& defaultText) {
    Dali::DALI_LOG_INFO("Prompt: %s, Default: %s", message.c_str(), defaultText.c_str());
    webView.JavaScriptPromptReply("user input");
    return true;
  }
);

// Remove the handler when it is no longer needed
webView.RemoveJavaScriptMessageHandler("nativeObject");
```

---

## 5. Event Handling & Signals

Connect to WebView signals to handle page load events and other notifications:

```cpp
webView.PageLoadStartedSignal().Connect(
  [](WebView view, const Dali::String& url) {
    Dali::DALI_LOG_INFO("Page load started: %s", url.c_str());
  }
);

webView.PageLoadInProgressSignal().Connect(
  [](WebView view, const Dali::String& url) {
    // Called periodically during loading
    Dali::DALI_LOG_INFO("Loading in progress...");
  }
);

webView.PageLoadFinishedSignal().Connect(
  [](WebView view, const Dali::String& url) {
    Dali::DALI_LOG_INFO("Page load finished: %s", url.c_str());
  }
);

webView.PageLoadErrorSignal().Connect(
  [](WebView view, const WebViewPageLoadError& error) {
    Dali::DALI_LOG_ERROR("Load error: %s", error.description.c_str());
  }
);

webView.UrlChangedSignal().Connect(
  [](WebView view, const Dali::String& url) {
    Dali::DALI_LOG_INFO("URL changed: %s", url.c_str());
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

Forward touch and key events to the web engine:

```cpp
// Forward a key event
const KeyEvent& keyEvent = ...;
bool consumed = webView.FeedKeyEvent(keyEvent);

// Forward a touch event
const TouchEvent& touchEvent = ...;
bool consumed = webView.FeedTouchEvent(touchEvent);

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
  [](Dali::Ui::ImageView image) {
    // Use the screenshot
    Dali::DALI_LOG_INFO("Screenshot captured");
  }
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
  [](const Dali::String& text) {
    Dali::DALI_LOG_INFO("Plain text: %s", text.c_str());
  }
);
```

### Video Playing Status

Check if video is currently playing on the page:

```cpp
webView.CheckVideoPlayingAsynchronously(
  [](bool isPlaying) {
    Dali::DALI_LOG_INFO("Video playing: %s", isPlaying ? "yes" : "no");
  }
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
    Dali::DALI_LOG_INFO("Found %u matches", matchCount);
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
