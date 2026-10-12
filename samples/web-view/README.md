# WebView TV 키 포커스 샘플

`web-view-key-test.example`은 TV 리모컨의 키가 Window → WebView → 웹 페이지로
전달되는지 확인하는 오프라인 샘플입니다. `res/key-test.html`을 읽어
`LoadHtmlString()`으로 로드하므로 인터넷 연결이나 외부 웹사이트가 필요하지 않습니다.
TV/Linux에서는 Chromium, Windows에서는 LWE 엔진을 사용합니다.

## 확인하는 패치

`WebViewImpl::OnFocusChanged(bool)`에서 웹 엔진의 `SetFocus(focused)`를 호출한 뒤
기존 View 포커스 처리를 수행하는 패치를 검증합니다. 샘플은
`FocusManager::SetCurrentFocusView()`로 포커스를 요청하며, 웹 엔진에 직접 포커스를
설정하거나 키를 합성하지 않습니다. 최초 로드 완료 후 WebView에 포커스를 요청합니다.
Window가 다시 활성화되면 마지막으로 요청한 대상에 포커스를 요청합니다.

화면 상단에는 Window 활성 상태, 실제 탐색 포커스, 키 입력 대상, WebView 포커스
획득·해제 횟수와 Window/WebView/Native 키 카운터가 표시됩니다. HTML에는
`document.hasFocus()`, `activeElement`, DOM DOWN/UP 카운터와 최근 50개 로그가 있습니다.
노란 테두리는 페이지가 실제 포커스를 가진 경우에만 표시됩니다.

## 조작

| 키 | 동작 |
| --- | --- |
| 방향키 | HTML의 2행 × 3열 항목 이동. 가장자리에서는 멈춤 |
| Enter / 확인 | 현재 항목 선택. 길게 눌러도 한 번만 선택 |
| 1 / W | WebView에 포커스 요청 |
| 2 / N | 네이티브 Label에 포커스 요청하여 WebView 포커스 해제 |
| 3 / R | 로컬 HTML을 다시 읽어 로드. DOM 카운터와 선택 결과 초기화 |
| 0 / S | 포커스 상태와 누적 키 카운터를 dlog에 기록 |
| Back / Escape | 앱 종료 |

명령키는 Window Intercept에서 DOWN/UP 모두 소비하며, UP 전달이 끝난 뒤 타이머에서
실행합니다. 방향키와 확인키는 Intercept에서 소비하지 않고 정상 전달합니다.
WebView의 키 관찰 콜백은 `false`를 반환하고, 실제 소비 여부는 웹 엔진이 결정합니다.
WebView를 포커스 그룹으로 설정하여 엔진이 방향키를 소비하지 않아도 DALi 탐색이
네이티브 대상으로 넘어가지 않도록 했습니다. 네이티브 Label은 테스트 키를 소비하여
명시적인 1/W 조작 전까지 포커스를 유지합니다.

## TV에서 실행

변경된 foundation 라이브러리와 호환되는 core/adaptor 및 Chromium 웹 엔진 플러그인이
필요합니다. 기존 WebView 패키지의 CMake 타깃, 앱 manifest와 RPM 파일 목록에
`web-view-key-test.example`을 등록했습니다. Tizen 리소스 설치 경로는 기존
`samples/common.cmake` 설정에 따라 `/usr/apps/com.samsung.dali.web-view/res/`입니다.

이미 생성한 샘플 RPM과 변경된 라이브러리를 개발 TV에 설치한 뒤 실행합니다.
`<sample.rpm>`은 실제 패키지 경로로 바꿉니다.

```sh
sdb root on
sdb push <sample.rpm> /tmp/web-view-sample.rpm
sdb shell rpm -Uvh /tmp/web-view-sample.rpm
sdb shell app_launcher -s web-view-key-test.example
```

초기 포커스 로그도 수집하려면 앱 실행 전에 별도 터미널에서 시작합니다.

```sh
sdb shell dlogutil DALI:I | rg WebViewKeyTest
```

## 실기기 확인 순서와 기대 결과

1. 시작 후 HTML이 표시되고 `Window: ACTIVE`, `navigation: WebView`,
   `key target: WebView`, `WebView focus: GAINED`인지 확인합니다.
   HTML의 `document.hasFocus=true`와 첫 항목의 노란 테두리도 확인합니다.
2. Right → Down → Enter를 누릅니다. 항목 1 → 2 → 5로 이동하고 항목 5가 선택됩니다.
   각 키의 DOWN/UP이 Window·WebView·DOM에 기록되어야 합니다.
   가장자리 방향키는 선택을 유지하고 확인키를 길게 누르면 선택 횟수는 한 번만 증가합니다.
3. 2/N을 누릅니다. WebView `LOST`, native `GAINED` 로그와
   `navigation: native`, `key target: native`를 확인합니다. HTML은
   `document.hasFocus=false`가 되고 노란 테두리가 사라져야 합니다.
   방향키/Enter는 Window·Native 카운터만 증가시키고 WebView·DOM 카운터와 선택 결과는
   유지되어야 합니다.
4. 1/W를 누릅니다. WebView `GAINED`, `document.hasFocus=true`가 되고 이전 HTML 항목을
   이어서 조작할 수 있어야 합니다. 2 → 1을 여러 번 반복하여 포커스 획득·해제를 확인합니다.
5. 다른 앱이나 TV 시스템 UI로 Window 포커스를 옮긴 뒤 복귀합니다. 마지막 요청 대상이
   WebView면 HTML 조작이 복구되고, native면 native 포커스가 복구되어야 합니다.
6. 3/R로 로컬 페이지를 다시 로드하고 조작을 반복합니다. DOM 카운터와 선택 결과는
   초기화되며 네이티브 카운터는 누적됩니다. Back/Escape로 종료합니다.

카운터는 DOWN과 UP을 각각 한 건으로 셉니다. 명령키도 Window 카운터에는 포함되지만
WebView·Native·DOM에는 전달되지 않으므로 전체 누적 값이 항상 같지는 않습니다.
웹 엔진의 DOM 전달은 비동기일 수 있습니다. 비교할 때는 같은 방향키/Enter의
`key`, `code`, 상태와 로그를 확인합니다.

Window 로그가 없으면 플랫폼 키 전달을, Window만 증가하면 실제 키 입력 대상과
View 전달을 확인합니다. WebView 카운터는 증가하지만 DOM 로그가 없거나
`document.hasFocus=false`이면 웹 엔진의 포커스와 키 처리를 확인합니다.
View의 `GAINED`만으로 엔진 포커스 적용 성공을 판단하지 말고 DOM 상태도 확인합니다.

위 내용은 실기기에서 확인할 **기대 결과**입니다. 이번 작업에서는 빌드나 TV 실행을
수행하지 않았습니다.
