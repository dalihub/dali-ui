# DALi UI Foundation - WebView

[→ English Documentation](https://github.sec.samsung.net/NUI/dali-ui/wiki/WebView)

`WebView`는 웹 콘텐츠를 표시하는 View입니다. 애플리케이션 안에 웹 페이지와 HTML 콘텐츠를 표시하며, 페이지 탐색, JavaScript 연동 및 다양한 웹 엔진 기능을 제공합니다.

---

## 목차

1. [기본 설정](#1-기본-설정)
2. [콘텐츠 로딩](#2-콘텐츠-로딩)
3. [페이지 탐색](#3-페이지-탐색)
4. [JavaScript 연동](#4-javascript-연동)
5. [이벤트 및 시그널 처리](#5-이벤트-및-시그널-처리)
6. [사용자 입력 및 이벤트](#6-사용자-입력-및-이벤트)
7. [페이지 정보 조회](#7-페이지-정보-조회)
8. [렌더링 및 표시](#8-렌더링-및-표시)
9. [텍스트 검색](#9-텍스트-검색)
10. [캐시 및 쿠키](#10-캐시-및-쿠키)
11. [주의사항](#11-주의사항)

---

## 1. 기본 설정

정적 factory 메서드 `New()`를 사용하여 `WebView`를 생성합니다.

```cpp
#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali::Ui;

// 빈 WebView 생성
WebView webView = WebView::New();

// URL 로드
webView.LoadUrl("https://www.example.com");

// 크기 설정
webView.SetRequestedWidth(800.0f);
webView.SetRequestedHeight(600.0f);

// Scene에 추가
window.Add(webView);
```

### 웹 엔진 선택

WebView는 기본적으로 Chromium을 사용합니다. 다른 웹 엔진을 사용하려면 앱 시작 시점에 `UiConfig`에서 설정합니다.

```cpp
UiConfig config = UiConfig::New();
config.SetWebEngineType(WebEngineType::LWE);
config.Apply();
```

---

## 2. 콘텐츠 로딩

### URL 로드

```cpp
webView.LoadUrl("https://www.example.com");
```

### HTML 문자열 로드

```cpp
std::string html = "<html><body><h1>Hello World</h1></body></html>";
webView.LoadHtmlString(html);
```

### Base URI와 함께 HTML 로드

현재 history entry를 대체하면서 HTML 문자열과 base URI를 지정합니다.

```cpp
webView.LoadHtmlStringOverrideCurrentEntry(
  "<html><body>Content</body></html>",
  "https://base.example.com",
  "https://unreachable.example.com"
);
```

### Raw 콘텐츠 로드

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

## 3. 페이지 탐색

다음 메서드로 페이지 history를 탐색하거나 로딩 상태를 제어할 수 있습니다.

| 메서드 | 설명 |
|---|---|
| `GoForward()` | 다음 history 페이지로 이동합니다. |
| `GoBack()` | 이전 history 페이지로 이동합니다. |
| `CanGoForward()` | 다음 페이지로 이동할 수 있는지 확인합니다. |
| `CanGoBack()` | 이전 페이지로 이동할 수 있는지 확인합니다. |
| `ClearHistory()` | 전체 페이지 history를 삭제합니다. |
| `Reload()` | 현재 페이지를 다시 로드합니다. |
| `ReloadWithoutCache()` | 캐시를 사용하지 않고 현재 페이지를 다시 로드합니다. |
| `StopLoading()` | 현재 페이지 로딩을 중지합니다. |

```cpp
if (webView.CanGoBack())
{
  webView.GoBack();
}

webView.Reload();
webView.StopLoading();
```

---

## 4. JavaScript 연동

### JavaScript 실행

JavaScript를 실행하고 필요한 경우 callback으로 결과를 받을 수 있습니다.

```cpp
// 결과를 받지 않고 실행
webView.EvaluateJavaScript("console.log('Hello from C++');");

// callback으로 실행 결과 수신
webView.EvaluateJavaScript(
  "document.title",
  [](const Dali::String& result) {
    Dali::DALI_LOG_INFO("Title: %s", result.c_str());
  }
);
```

### JavaScript 메시지 핸들러

JavaScript가 보내는 메시지를 수신할 handler를 등록합니다.

```cpp
webView.AddJavaScriptMessageHandler(
  "nativeObject",
  [](const Dali::String& message) {
    Dali::DALI_LOG_INFO("Message from JS: %s", message.c_str());
  }
  );
```

웹 페이지의 JavaScript에서는 다음과 같이 메시지를 보낼 수 있습니다.

```javascript
// 웹 페이지에서 실행
if (window.nativeObject) {
  window.nativeObject("Hello from JavaScript");
}
```

### JavaScript 대화상자

JavaScript의 alert, confirm 및 prompt 대화상자를 처리합니다.

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
    webView.JavaScriptConfirmReply(true); // 또는 false
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

// 더 이상 필요하지 않은 handler 삭제
webView.RemoveJavaScriptMessageHandler("nativeObject");
```

---

## 5. 이벤트 및 시그널 처리

페이지 로딩 이벤트와 기타 알림을 처리하려면 WebView 시그널에 연결합니다.

```cpp
webView.PageLoadStartedSignal().Connect(
  [](WebView view, const Dali::String& url) {
    Dali::DALI_LOG_INFO("Page load started: %s", url.c_str());
  }
);

webView.PageLoadInProgressSignal().Connect(
  [](WebView view, const Dali::String& url) {
    // 로딩 중 주기적으로 호출됨
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

### 제공 시그널

| 시그널 | 설명 |
|---|---|
| `PageLoadStartedSignal()` | 페이지 로딩이 시작될 때 발생합니다. |
| `PageLoadInProgressSignal()` | 페이지를 로딩하는 동안 발생합니다. |
| `PageLoadFinishedSignal()` | 페이지 로딩이 완료될 때 발생합니다. |
| `PageLoadErrorSignal()` | 페이지 로딩 오류가 발생할 때 발생합니다. |
| `UrlChangedSignal()` | URL이 변경될 때 발생합니다. |
| `FrameRenderedSignal()` | 프레임 렌더링이 완료된 후 발생합니다. |
| `ScrollEdgeReachedSignal()` | 스크롤이 가장자리에 도달할 때 발생합니다. |
| `OverScrolledSignal()` | over-scroll이 발생할 때 발생합니다. |
| `FullscreenEnteredSignal()` | 전체 화면 모드로 진입할 때 발생합니다. |
| `FullscreenExitedSignal()` | 전체 화면 모드에서 나올 때 발생합니다. |
| `TextFoundSignal()` | 텍스트 검색 결과가 발견될 때 발생합니다. |
| `GeolocationPermissionSignal()` | 위치 정보 권한이 요청될 때 발생합니다. |
| `WebProcessCrashedSignal()` | 웹 프로세스가 비정상 종료될 때 발생합니다. |

---

## 6. 사용자 입력 및 이벤트

### 마우스 및 키 이벤트

마우스와 키보드 이벤트 처리를 활성화하거나 비활성화합니다.

```cpp
webView.SetMouseEventsEnabled(true);   // 마우스 이벤트 활성화(기본값)
webView.SetKeyEventsEnabled(true);     // 키 이벤트 활성화(기본값)

bool mouseEnabled = webView.IsMouseEventsEnabled();
bool keyEnabled   = webView.IsKeyEventsEnabled();
```

### 이벤트 직접 전달

터치 및 키 이벤트를 웹 엔진으로 직접 전달합니다.

```cpp
// 키 이벤트 전달
const KeyEvent& keyEvent = ...;
bool consumed = webView.FeedKeyEvent(keyEvent);

// 터치 이벤트 전달
const TouchEvent& touchEvent = ...;
bool consumed = webView.FeedTouchEvent(touchEvent);

// 마우스 휠 이벤트 전달
webView.FeedMouseWheel(true, 3, 100, 200); // yDirection, step, x, y
```

### 스크롤

```cpp
webView.ScrollBy(10, 20);      // 지정한 delta만큼 스크롤
webView.ScrollEdgeBy(0, -50);  // 가장자리 감지를 적용하여 스크롤
```

---

## 7. 페이지 정보 조회

### URL 및 제목

```cpp
Dali::String currentUrl = webView.GetUrl();
Dali::String pageTitle  = webView.GetTitle();
```

### User Agent

```cpp
webView.SetUserAgent("Custom User Agent String");
Dali::String userAgent = webView.GetUserAgent();
```

### 로딩 진행률

```cpp
float progress = webView.GetLoadProgressPercentage(); // 0.0~100.0
```

### 확대/축소 비율

```cpp
// 전체 페이지 확대/축소
webView.SetPageZoomFactor(1.2f);
float pageZoom = webView.GetPageZoomFactor();

// 텍스트만 확대/축소
webView.SetTextZoomFactor(1.1f);
float textZoom = webView.GetTextZoomFactor();
```

### Scale Factor

```cpp
webView.SetScaleFactor(2.0f, Dali::Vector2(100.0f, 100.0f)); // 배율, 중심점
float scale = webView.GetScaleFactor();
```

### 콘텐츠 정보

```cpp
Dali::Vector2 scrollPos   = webView.GetScrollPosition();
Dali::Vector2 scrollSize  = webView.GetScrollSize();
Dali::Vector2 contentSize = webView.GetContentSize();
Dali::String selectedText = webView.GetSelectedText();
```

---

## 8. 렌더링 및 표시

### 문서 배경색

```cpp
webView.SetDocumentBackgroundColor(Dali::Vector4(1.0f, 1.0f, 1.0f, 1.0f)); // RGBA
```

### Video Hole

웹 콘텐츠 위에 비디오를 렌더링할 투명 영역을 활성화합니다.

```cpp
webView.SetVideoHoleEnabled(true);
bool videoHoleEnabled = webView.IsVideoHoleEnabled();
```

### Tile 관리

큰 페이지를 렌더링할 때의 성능과 리소스 사용을 조정합니다.

```cpp
webView.SetTilesClearedWhenHidden(true);   // 숨겨졌을 때 tile 삭제
webView.SetTileCoverAreaMultiplier(2.0f);  // tile cover area 배수
webView.ClearAllTilesResources();          // 모든 tile 리소스 즉시 삭제
```

### 일시 중지 및 재개

웹 엔진 처리 또는 네트워크 로딩을 일시 중지하고 재개합니다.

```cpp
webView.Suspend();
webView.Resume();

webView.SuspendNetworkLoading();
webView.ResumeNetworkLoading();
```

### 스크린샷

현재 WebView 콘텐츠를 이미지로 캡처합니다.

```cpp
// 동기 스크린샷
Dali::BoundsInteger area(0, 0, 800, 600);
Dali::Ui::ImageView screenshot = webView.GetScreenshot(area, 1.0f);

// callback으로 비동기 스크린샷 수신
webView.GetScreenshotAsynchronously(
  area,
  1.0f,
  [](Dali::Ui::ImageView image) {
    // 캡처한 이미지 사용
    Dali::DALI_LOG_INFO("Screenshot captured");
  }
);
```

### Favicon

```cpp
Dali::Ui::ImageView favicon = webView.GetFavicon();
```

### 페이지 Plain Text

페이지에 표시되는 전체 텍스트를 비동기로 가져옵니다.

```cpp
webView.GetPlainTextAsynchronously(
  [](const Dali::String& text) {
    Dali::DALI_LOG_INFO("Plain text: %s", text.c_str());
  }
);
```

### 비디오 재생 상태

페이지에서 비디오가 재생 중인지 비동기로 확인합니다.

```cpp
webView.CheckVideoPlayingAsynchronously(
  [](bool isPlaying) {
    Dali::DALI_LOG_INFO("Video playing: %s", isPlaying ? "yes" : "no");
  }
);
```

---

## 9. 텍스트 검색

페이지 안의 텍스트를 검색하고 강조 표시합니다.

```cpp
// 대소문자를 구분하지 않고 검색하여 결과 강조
uint32_t maxMatches = 100;
bool result = webView.FindText(
  "search term",
  Dali::Ui::WebViewFindOption::CASE_INSENSITIVE |
  Dali::Ui::WebViewFindOption::SHOW_HIGHLIGHT,
  maxMatches
);

// 시그널로 검색 결과 수신
webView.TextFoundSignal().Connect(
  [](WebView view, uint32_t matchCount) {
    Dali::DALI_LOG_INFO("Found %u matches", matchCount);
  }
);
```

### 검색 옵션

| 옵션 | 설명 |
|---|---|
| `NONE` | 옵션을 사용하지 않습니다. |
| `CASE_INSENSITIVE` | 대소문자를 구분하지 않습니다. |
| `AT_WORD_STARTS` | 단어 경계에서 일치하는 항목을 검색합니다. |
| `TREAT_MEDIAL_CAPITAL_AS_WORD_START` | 단어 중간의 대문자를 단어 시작으로 처리합니다. |
| `BACKWARDS` | 뒤쪽 방향으로 검색합니다. |
| `WRAP_AROUND` | 페이지 시작과 끝을 연결하여 계속 검색합니다. |
| `SHOW_OVERLAY` | 검색 결과 overlay를 표시합니다. |
| `SHOW_FIND_INDICATOR` | 검색 indicator를 표시합니다. |
| `SHOW_HIGHLIGHT` | 일치한 텍스트를 강조 표시합니다. |

---

## 10. 캐시 및 쿠키

### 캐시 관리

```cpp
// 같은 profile의 모든 WebView가 공유하는 리소스 캐시 삭제
webView.GetProfile().ClearCache();

// 같은 동작을 수행하는 편의 shortcut
webView.ClearCache();

// 캐시를 사용하지 않고 다시 로드
webView.ReloadWithoutCache();
```

### 쿠키 관리

```cpp
// 같은 profile의 모든 WebView가 공유하는 쿠키 삭제
webView.GetProfile().GetCookieManager().ClearAllCookies();

// 같은 동작을 수행하는 편의 shortcut
webView.ClearCookies();
```

### Custom Header

요청에 사용할 custom HTTP header를 추가하거나 삭제합니다.

```cpp
webView.AddCustomHeader("X-Custom-Header", "CustomValue");
webView.RemoveCustomHeader("X-Custom-Header");
```

### 추가 기능 설정

전용 API가 없는 기능은 범용 string 설정 경로를 사용합니다.

```cpp
WebSettings settings = webView.GetSettings();
settings.SetExtraFeatureValue("featureName", "featureValue");
Dali::String value = settings.GetExtraFeatureValue("featureName");
```

---

## 11. 주의사항

- 첫 번째 `WebView`를 생성하기 전에 `UiConfig`를 적용해야 합니다.
- `UiConfig::Apply()`를 호출하기 전에 웹 엔진을 선택합니다. 기본 엔진은 Chromium입니다.
- 지원하는 웹 엔진 기능은 플랫폼 및 엔진 구현에 따라 다를 수 있습니다.

---

## 가이드 참고

WebView API와 사용 사례에 관한 자세한 내용은 [API Reference](https://pages.github.sec.samsung.net/NUI/dali-ui/)를 참고하세요.
