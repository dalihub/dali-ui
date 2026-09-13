[→ English](https://github.sec.samsung.net/NUI/dali-ui/wiki/Toast)

# DALi UI Components - Toast

`Dali::Ui::Toast`는 특정 `Window` 위에 짧은 비모달 피드백을 표시합니다. 메시지
text와 optional icon으로 구성되는 One UI Toast 구조를 따르며, 사용자 응답을
요구하지 않고 유한한 시간이 지나면 사라집니다.

Toast는 Snackbar의 action이나 확장 상태를 제공하지 않습니다. 사용자의 선택이나
action 실행이 필요하면 다른 컴포넌트를 사용하십시오.

## 1. 설정과 기본 사용법

Style을 생성하거나 main loop에 진입하기 전에 component configuration을
적용합니다.

```cpp
#include <dali-ui-components/dali-ui-components.h>

Components::UiConfig::New().Apply();

Toast toast = Toast::New("저장했습니다", Toast::Duration::SHORT);
toast.Post(window);
```

`Post()`는 초기화된 `Window`를 반드시 받아 해당 Window의 overlay layer에 Toast를
추가합니다. 빈 Window를 전달하면 DALi assertion이 발생합니다.

## 2. Content와 표시 시간

Toast는 하나의 text message를 포함하며 icon을 선택적으로 표시할 수 있습니다.
Text는 고정된 최대 줄 수 없이 자연스럽게 줄바꿈됩니다. 표시 가능한 content는 설정된
Toast 최대 높이로 제한되며, 높이를 초과하는 text에는 ellipsis가 적용됩니다.

```cpp
toast.SetText("네트워크 연결이 복구되었습니다");
toast.SetDuration(Toast::Duration::LONG);
toast.Post(window);
```

Icon URL을 설정하면 icon-and-text variant가 됩니다. Icon은 필요할 때 생성되어 text
앞에 배치되며, 빈 URL을 설정하면 layout에서 제외됩니다.

```cpp
toast.SetIconResourceUrl("images/saved.png");
toast.SetIconColor(UiColor(Color::WHITE));
toast.SetIconSynchronousLoading(false);

toast.SetIconResourceUrl(""); // Text-only variant로 되돌립니다.
```

기본 표시 시간은 다음과 같습니다.

| 값 | 시간 |
|---|---:|
| `Toast::Duration::SHORT` | 1500 ms |
| `Toast::Duration::LONG` | 2500 ms |

모든 표시 시간은 유한하며 0보다 커야 합니다. One UI Components와 동일하게
timeout은 `Post()` 호출 시 진입 transition과 함께 시작됩니다. 게시 중인 Toast의
시간을 변경하면 timeout이 다시 시작됩니다.

## 3. 닫기와 lifecycle signal

```cpp
toast.ShownSignal().Connect(this, &Controller::OnToastShown);
toast.HiddenSignal().Connect(this, &Controller::OnToastHidden);

toast.Post(window);
toast.Dismiss();
```

- `ShownSignal()`은 진입 transition이 끝난 뒤 발생합니다.
- `HiddenSignal()`은 timeout, 명시적인 닫기 또는 외부 제거 cleanup이 끝난 뒤
  발생합니다.
- Toast가 이미 분리되었거나 닫히는 중이면 `Dismiss()`는 아무 작업도 하지 않습니다.
- 같은 Toast를 다시 `Post()`하면 현재 표시를 교체하며, 교체된 표시에 대해서는
  `HiddenSignal()`을 발생시키지 않습니다.

전역 queue나 manager를 제공하지 않으므로 서로 다른 Toast instance는 동시에 표시될
수 있습니다.

## 4. ToastStyle

`ToastStyle`에는 Toast에 필요한 다음 시각·layout 값이 있습니다.

- 최대 너비 비율
- 최대 높이와 Window 높이 기준 하단 offset 비율
- content padding, icon/text 간격과 corner radius
- background와 text color
- font size와 family
- icon 크기, corner radius와 color
- shadow와 borderline

기존 style을 설정해 immutable custom style을 만듭니다.

```cpp
ToastStyle style = ToastStyle::Default()
                     .Configure()
                     .SetMaximumWidthRatio(0.75f)
                     .SetMaximumHeight(120.0f)
                     .SetBottomOffsetRatio(0.08f)
                     .SetItemSpacing(12.0f)
                     .SetPadding(Insets(24.0f, 24.0f, 12.0f, 12.0f))
                     .SetBackgroundColor(UiColor(0x23465Du))
                     .SetTextColor(UiColor(0xFFFFFFu))
                     .SetCornerRadius(Vector4(20.0f, 20.0f, 20.0f, 20.0f))
                     .Build();

Toast toast = Toast::New("사용자 style", Toast::Duration::LONG, style);
```

기본 One UI 값은 다음과 같습니다.

| 값 | 기본값 |
|---|---:|
| 최대 너비 | Window 너비의 68% |
| 최대 높이 | 112 |
| 하단 offset | Window 높이의 5% |
| Padding | 가로 32, 세로 16 |
| Icon/text 간격 | 16 |
| Corner radius | 36 |
| Icon 크기 / radius | 36 × 36 / 0 |
| Font | SamsungOneUI400, 28 |
| Borderline | width 2, offset -1, `OutlineACC` |
| Background / text | `SurfaceFixed` / `OnSurfaceContainerFixedVariantBright` |

Window 크기가 바뀌면 Toast의 위치와 크기를 다시 계산합니다. Focus된 view가
`InputField` 또는 `InputEditor`이고 input panel이 표시 중이면 그 위로 이동합니다.
진입과 퇴장은 각각 One UI의 300ms opacity+vertical motion curve를 사용하며,
transition 시간은 style property가 아닌 Toast 고유 동작입니다.

## 5. 접근성과 다국어

Toast는 `NOTIFICATION` 접근성 role을 사용합니다. 접근성 name은 메시지 text이며,
중복 안내를 막기 위해 내부 Label은 접근성 tree에서 숨깁니다.

Toast 전용 translation type은 없습니다. `UiLocalizationManager`로 application
resource를 `SetText()`에 binding합니다.

```cpp
UiLocalizationManager::Get().SetBindingResource(
  toast,
  "ToastText",
  "IDS_SAVED",
  LocalizedStringCallback::New(this, &MyApp::ApplyToastText));

void MyApp::ApplyToastText(BaseHandle target, const Dali::String& text)
{
  Toast toast = Toast::DownCast(target);
  if(toast)
  {
    toast.SetText(text);
  }
}
```

## 6. Sample과 GBS package

Interactive sample은 text-only, optional icon, short, long, long-text, custom
style, 명시적인 dismissal, lifecycle signal 경로를 확인합니다.

```sh
cmake -S samples/toast -B samples/toast/build
cmake --build samples/toast/build

gbs build -A armv7l --include-all --packaging-dir samples/toast/packaging
```

Package id는 `com.samsung.dali.toast`이고 실행 파일은 `toast.example`입니다.

---

[← Components로 돌아가기](Components.md)
