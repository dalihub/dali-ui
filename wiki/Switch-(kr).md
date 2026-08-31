[→ English](https://github.sec.samsung.net/NUI/dali-ui/wiki/Switch)

# DALi UI Components - Switch

`Dali::Ui::Switch`는 `dali-ui-components`에서 제공하는 이진 선택
컴포넌트입니다. Track과 움직이는 thumb로 상태를 표현하며 터치 탭, 수평 드래그,
focus 상태의 Enter 키 입력을 지원합니다. 선택 API와 signal은 기반 클래스인
`SelectableView`에서 상속합니다.

기본 style은 DALi UI 컴포넌트 theme을 따르면서 OneUI Switch의 개념을 유지합니다.
Theme 변수는 `SwitchStyle`로 표현하고, 선택 전환에는 animation을 적용하며, touch
overlay는 thumb를 중심으로 표시합니다.

---

## 1. 기본 설정

Components 통합 header를 include하고, component style을 생성하거나 main loop에
진입하기 전에 `Components::UiConfig`를 적용합니다.

```cpp
#include <dali-ui-components/dali-ui-components.h>

using namespace Dali;
using namespace Dali::Ui;

int main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);

  // Foundation 설정과 component style을 모두 포함합니다.
  Components::UiConfig::New().Apply();

  MyController controller(application);
  application.MainLoop();
  return 0;
}
```

`dali-ui-components`를 사용하는 애플리케이션에서는
`Components::UiConfig`만 적용합니다. Foundation `UiConfig`를 별도로 적용할 필요가
없습니다.

---

## 2. Switch 생성과 배치

설정된 기본 style로 Switch를 생성합니다.

```cpp
Switch wifiSwitch = Switch::New();
wifiSwitch.SetAccessibilityName("Wi-Fi");
rootLayout.Add(wifiSwitch);
```

처음부터 켜진 상태여야 한다면 scene에 추가하기 전에 초기 상태를 설정합니다.

```cpp
Switch bluetoothSwitch = Switch::New();
bluetoothSwitch.SetSelected(true);
rootLayout.Add(bluetoothSwitch);
```

기본 track 크기는 `56 x 32` unit입니다. `10` unit의 외부 padding이 확장된 touch
effect target을 포함하므로 기본 natural size는 `76 x 52` unit입니다. 일반적인
`View` 크기 및 layout API로 requested size나 minimum size를 변경할 수 있습니다.

---

## 3. 입력과 상호작용

지원하는 모든 입력 방식이 동일한 selected 상태를 제어합니다.

| 입력 | 동작 |
|---|---|
| 탭/클릭 | On과 off 상태를 전환합니다. |
| 수평 드래그 | Pointer를 따라 이동하고 손을 놓으면 가까운 상태로 정착합니다. |
| Focus 상태의 Enter | 상속받은 key-click 경로를 통해 상태를 전환합니다. |
| 중단된 드래그 | 중단된 gesture 이전의 상태로 돌아갑니다. |
| RTL layout | Thumb 위치와 수평 드래그 방향을 반전합니다. |

`Switch`는 상속받은 interaction 정책도 따릅니다.

```cpp
wifiSwitch.SetEnabled(false);              // Pointer와 Enter 입력을 무시합니다.
wifiSwitch.SetClickable(false);            // 사용자 activation을 막습니다.
wifiSwitch.SetToggleByClickEnabled(false); // 사용자 상태 전환을 막습니다.
```

이 정책들은 Switch 드래그를 포함한 사용자 입력을 차단하지만 애플리케이션의
`SetSelected()` 호출은 차단하지 않습니다.

---

## 4. 선택 상태와 Signal

상속받은 `SelectableView` API로 상태를 읽거나 변경합니다.

```cpp
bool enabled = wifiSwitch.IsSelected();
wifiSwitch.SetSelected(!enabled);
```

`SelectionChangedSignal()`을 연결하면 실제 상태 변경을 확인할 수 있습니다.

```cpp
wifiSwitch.SelectionChangedSignal().Connect(
  this,
  [this](View /*view*/, bool selected, InputEvent event) {
    if(event.IsProgrammatic())
    {
      // 애플리케이션 SetSelected() 호출 또는 완료된 드래그에 의한 변경입니다.
    }
    else
    {
      // 탭 또는 Enter 입력에 의한 변경입니다.
    }

    UpdateWifi(selected);
  });
```

Signal은 selected 값이 실제로 달라졌을 때만 발생합니다. 현재 값과 동일한 값으로
`SetSelected()`를 호출하면 signal이 발생하지 않습니다. Tap과 Enter 변경은 원래 input
event를 전달합니다. 드래그는 `SetSelected()`로 상태를 확정하므로 signal event는
programmatic이지만 AUTO mode에서도 드래그 전환 animation은 실행됩니다.

---

## 5. 선택 Animation

`SetSelectionAnimationMode()`로 thumb와 track의 전환 animation 적용 시점을
제어합니다.

| Mode | 동작 |
|---|---|
| `SelectionAnimationMode::AUTO` | 기본값입니다. 사용자 입력은 animation하고 programmatic 변경은 즉시 적용합니다. |
| `SelectionAnimationMode::ENABLED` | 화면에 보이고 scene에 연결된 동안 사용자 입력과 programmatic 변경을 모두 animation합니다. |
| `SelectionAnimationMode::DISABLED` | 항상 요청한 상태를 즉시 적용합니다. |

```cpp
wifiSwitch.SetSelectionAnimationMode(SelectionAnimationMode::ENABLED);
wifiSwitch.SetSelected(true); // 화면에 보이고 scene에 연결되어 있으면 animation합니다.
```

Animation이 활성화되면 남은 이동 거리에 비례해 duration을 조정합니다. 따라서 전환
중 상태를 반대로 바꾸거나 드래그를 시작해도 현재 화면에 표시된 위치부터 이어집니다.

전체 전환은 OneUI의 고정 timing인 `0.3`초와 cubic control point `(0.22, 0.25)`,
`(0.0, 1.0)`을 사용합니다. 이 값들은 `SwitchStyle` 항목이 아니라 Switch 고유
동작입니다.

---

## 6. SwitchStyle 사용자 설정

`SwitchStyle`은 OneUI Switch variable에 대응하는 DALi UI 구조입니다. 설정된 기본
style을 복제하고 필요한 값만 변경한 뒤 immutable style을 build하여
`Switch::New(style)`에 전달합니다.

```cpp
SwitchStyle compactGreen = SwitchStyle::Default()
                             .Configure()
                             .SetTrackSize(48.0f, 28.0f)
                             .SetThumbPadding(Insets(3.0f, 3.0f, 3.0f, 3.0f))
                             .SetTrackColors(UiColor(0xB7B7BBu),
                                             UiColor(0x0BB075u))
                             .Build();

Switch compactSwitch = Switch::New(compactGreen);
```

Style은 Switch 생성 시점에 적용됩니다. `Switch`는 runtime `SetStyle()` API를
제공하지 않습니다.

### 기본값

| Style 값 | 기본값 |
|---|---|
| Minimum width / height | `0 / 0` |
| 외부 padding | `Insets(10, 10, 10, 10)` |
| Track width / height | `56 / 32` |
| Thumb padding | `Insets(3.25, 3.25, 3.25, 3.25)` |
| Off track color | Theme color `SurfaceContainerHigher` |
| On track color | `UiColor::PRIMARY` |
| Thumb color | Theme color `OnSurfaceContainerFixed` |
| Overlay effect | `Insets(10)` overlay padding과 `0.96` child recoil scale을 사용하는 round overlay |
| Drag threshold | `5` unit |

### Builder API 그룹

| 용도 | API |
|---|---|
| 크기 | `SetMinimumWidth()`, `SetMinimumHeight()`, `SetPadding()` |
| Track | `SetTrackWidth()`, `SetTrackHeight()`, `SetTrackSize()` |
| Thumb geometry | `SetThumbPadding()` |
| 색상 | `SetTrackColor()`, `SetSelectedTrackColor()`, `SetTrackColors()`, `SetThumbColor()` |
| Interaction effect | `SetOverlayEffect()` |
| 드래그 | `SetDragThreshold()` |

### Style 제약

`Build()`는 thumb geometry가 유효하도록 다음 조건을 검증합니다.

- Track width와 height는 finite한 양수여야 하며 width는 height 이상이어야 합니다.
- Insets 값은 finite하고 음수가 아니어야 합니다.
- 서로 마주 보는 thumb padding 값의 합은 track height보다 작아 양수 크기의
  thumb가 남아야 합니다.
- Minimum size와 drag threshold는 finite하고 음수가 아니어야 합니다.

이 조건을 위반하면 DALi assertion이 발생합니다.

---

## 7. Touch Overlay와 Recoil

기본 style은 round `OverlayEffect`를 사용합니다. `OverlayPadding`은 Switch의 layout이나
touch bounds를 바꾸지 않고 32x32 primary target 주변으로 visual을 10 unit 확장합니다.
화면에 보이는 thumb는 target의 자식이므로 누르는 동안 `0.96` scale로 줄어듭니다.

제품에서 다른 반응이 필요하면 style에 다른 `OverlayEffect`를 지정합니다.

```cpp
OverlayEffect effect = OverlayEffect::Round()
                         .Configure()
                         .SetOverlayPadding(Insets(8.0f, 8.0f, 8.0f, 8.0f))
                         .SetRecoilScope(RecoilScope::OVERLAY_TARGET_CHILDREN)
                         .SetRecoilScaleFactor(0.94f)
                         .Build();

SwitchStyle style = SwitchStyle::Default()
                      .Configure()
                      .SetOverlayEffect(effect)
                      .Build();
```

`SetOverlayEffect()`에 빈 `OverlayEffect`를 전달하면 Switch overlay effect를
비활성화합니다.

---

## 8. 접근성과 RTL

`Switch`는 접근성 role `SWITCH`를 제공하고 selected 및 enabled 상태에 맞춰 checked
및 enabled 접근성 상태를 동기화합니다. 각 Switch에는 제어하는 설정을 설명하는
이름을 지정합니다.

```cpp
Switch notifications = Switch::New();
notifications.SetAccessibilityName("Notifications");
```

접근성 이름에 현재 값(`on` 또는 `off`)을 넣지 마십시오. Checked 상태가 현재 값을
전달합니다. Focus 상태의 Enter 키 activation도 pointer 입력과 동일한 선택 경로를
사용합니다.

Thumb endpoint와 드래그 방향은 effective layout direction을 따르므로
애플리케이션에서 LTR과 RTL용 Switch style을 따로 만들 필요가 없습니다.

---

## 9. 샘플과 Packaging

Interactive sample은 기본, 초기 selected, custom style, disabled Switch를
보여줍니다. Label에서 callback 횟수와 마지막 변경이 programmatic인지 사용자 입력인지
확인할 수 있습니다.

DALi 환경을 불러온 뒤 저장소 root에서 standalone sample을 빌드합니다.

```sh
source ~/setenv
cmake -S samples/switch -B /tmp/dali-ui-switch-build -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug
cmake --build /tmp/dali-ui-switch-build
```

Sample 실행 파일은 다음 경로에 생성됩니다.

```text
samples/switch/bin/switch.example
```

Sample에는 Tizen packaging metadata도 포함되어 있습니다.

| 파일 | 용도 |
|---|---|
| `samples/switch/packaging/com.samsung.dali.switch.spec` | Package build 및 install 규칙 |
| `samples/switch/com.samsung.dali.switch.xml` | Tizen application metadata |
| `samples/switch/com.samsung.dali.switch.manifest` | Package manifest 및 privilege |

전체 코드는
[`samples/switch`](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/switch)에서
확인할 수 있습니다.

---

## 10. 사용 시 주의사항

- `Switch::New()` 또는 `SwitchStyle::Default()`를 호출하기 전에
  `Components::UiConfig`를 적용하십시오.
- 애플리케이션 동작이 selected 값에 의존한다면 `ClickedSignal()` 대신
  `SelectionChangedSignal()`을 사용하십시오. 드래그는 단순 클릭 없이 선택 상태를
  변경할 수 있습니다.
- `InputEvent::IsProgrammatic()`으로 tap/Enter와 programmatic 변경을 구분할 수 있지만,
  드래그 확정도 programmatic으로 전달됩니다.
- Programmatic 변경도 전환 과정이 보여야 한다면
  `SelectionAnimationMode::ENABLED`를 사용하십시오.
- Compact custom style을 만들 때 외부 padding이 설정한 OverlayEffect padding을
  포함하도록 유지하십시오.
- 제어하는 설정을 설명하는 접근성 이름을 지정하십시오.

<br/>

---

[← 컴포넌트 목록으로](Components.md)
