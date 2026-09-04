[→ English](https://github.sec.samsung.net/NUI/dali-ui/wiki/Toast)

# DALi UI Components - Toast

`Dali::Ui::Toast`는 특정 `Window` 위에 일시적인 비모달 메시지를 표시합니다.
동작과 시각 비율은 OneUI Snackbar를 기준으로 하고, API와 immutable
`ToastStyle`, handle/body 구현은 DALi UI 컴포넌트 규칙을 따릅니다.

Toast는 `Navigator` page나 dialog가 아닙니다. `Post(window)`가 해당 Window의
overlay layer에 추가하고, `Dismiss()`는 종료 animation 뒤에 제거합니다.

## 1. 설정과 기본 사용법

Style을 생성하거나 main loop에 진입하기 전에 component configuration을
적용합니다.

```cpp
#include <dali-ui-components/dali-ui-components.h>

Components::UiConfig::New().Apply();

Toast toast = Toast::New("저장했습니다", Toast::Duration::SHORT);
toast.Post(window);
```

`Post()`는 초기화된 `Window`를 반드시 받습니다. 인자가 없는 overload는 없으며
빈 Window를 전달하면 DALi assertion이 발생합니다.

## 2. Duration과 닫기

기본 millisecond 값은 다음과 같습니다.

| 값 | 시간 |
|---|---:|
| `Toast::Duration::SHORT` | 1500 ms |
| `Toast::Duration::LONG` | 2500 ms |
| `Toast::Duration::INDEFINITE` | 자동 timeout 없음 |

```cpp
toast.SetDuration(Toast::Duration::LONG);
toast.Post(window);

// Posting, 표시, 확장 중 어느 시점에도 호출할 수 있습니다.
toast.Dismiss();
```

Timeout은 posting 시작 시점에 시작하고 첫 action click이 멈춥니다. 같은 Toast에
`Post(window)`를 다시 호출하면 기존 표시 lifecycle을 교체하며, 교체된 표시에
대해서는 `HiddenSignal()`을 발생시키지 않습니다.

## 3. 선택적인 2단계 Action

Action button의 기본 text는 `Button`입니다. 빈 문자열을 설정하면 숨깁니다.

```cpp
toast.SetActionButtonText("자세히");
toast.ActionButtonClickedSignal().Connect(
  this,
  [](Toast toast, Toast::ActionStage stage) {
    if(stage == Toast::ActionStage::CONFIRM_IN_DEFAULT_MODE)
    {
      // 첫 클릭 뒤 Toast가 확장됩니다.
    }
    else
    {
      // 두 번째 클릭 뒤 Toast가 닫힙니다.
    }
  });
```

첫 action click은 `CONFIRM_IN_DEFAULT_MODE`를 발생시키고 현재 timeout을 멈춘 뒤
Toast를 확장합니다. 두 번째 click은 `CONFIRM_IN_EXPANDED_MODE`를 발생시키고
Toast를 닫습니다.

## 4. Lifecycle Signal

```cpp
toast.ShownSignal().Connect(this, &Controller::OnToastShown);
toast.HiddenSignal().Connect(this, &Controller::OnToastHidden);
```

- `ShownSignal()`은 표시 content가 준비된 뒤 post lifecycle마다 한 번
  발생합니다. Posting 도중 확장이 시작되면 확장 완료 시 발생합니다.
- `HiddenSignal()`은 명시적 닫기, timeout, action 또는 외부 제거 cleanup이 끝난
  뒤 발생합니다.
- 이미 분리된 Toast를 파괴할 때는 lifecycle signal이 발생하지 않습니다.

전역 queue나 manager를 제공하지 않으므로 서로 다른 Toast instance는 동시에 표시될
수 있습니다.

## 5. Content Property

다음 property는 표시 전이나 표시 중에 변경할 수 있습니다.

```cpp
toast.SetText("네트워크 연결이 복구되었습니다");
toast.SetTextColor(UiColor(0xFFFFFFu));
toast.SetFontSize(28.0f);
toast.SetFontFamily("SamsungOneUI400");
toast.SetActionButtonText("열기");
toast.SetActionButtonTextColor(UiColor(0x010102u));
toast.SetItemSpacing(20.0f);
```

## 6. ToastStyle

설정된 default를 복제해 immutable style을 build합니다.

```cpp
ToastStyle style = ToastStyle::Default()
                     .Configure()
                     .SetBackgroundColor(UiColor(0x23465Du))
                     .SetTextColor(UiColor(0xFFFFFFu))
                     .SetCornerRadius(Vector4(20.0f, 20.0f, 20.0f, 20.0f))
                     .SetMaximumWidthRatio(0.75f)
                     .Build();

Toast toast = Toast::New("사용자 style", Toast::Duration::LONG, style);
```

`ToastStyle`은 geometry, padding, spacing, color, typography, shadow, border,
action-button style 값을 포함합니다. Transition duration과 easing은 Toast 고유
동작이므로 style property로 노출하지 않습니다.

기본 최대 너비는 Window 너비의 68%입니다. 0이 아닌 절대 최대 너비를 지정하면
추가 상한으로 적용합니다. 기본 하단 offset은 Window 높이의 5%이며 Window resize
시 다시 계산합니다.

## 7. 접근성과 다국어

Root는 `NOTIFICATION` 접근성 role을 사용하고 자체 highlight 대상이 되지 않습니다.
Message Label은 접근성 tree에서 숨기며 선택적인 action은 계속 조작할 수 있습니다.

Toast 전용 translation type은 없습니다. `UiLocalizationManager`로 application
resource를 일반 setter에 binding합니다.

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

같은 방식으로 action-button text도 binding할 수 있습니다.

## 8. Sample과 GBS Package

Interactive sample은 short, long, indefinite, action 없음, custom style, 명시적
dismissal, signal count 경로를 확인합니다.

```sh
cmake -S samples/toast -B samples/toast/build
cmake --build samples/toast/build

gbs build -A armv7l --include-all --packaging-dir samples/toast/packaging
```

Package id는 `com.samsung.dali.toast`이고 실행 파일은 `toast.example`입니다.

---

[← Components로 돌아가기](Components.md)
