[English](https://github.sec.samsung.net/NUI/dali-ui/wiki/CheckBox) · [컴포넌트 목록](https://github.sec.samsung.net/NUI/dali-ui/wiki/Components-%28kr%29)

# CheckBox

`Dali::Ui::CheckBox`는 체크/해제 상태와 선택적인 후행 텍스트를 제공하는 `SelectableView`입니다. 기본 글리프는 설치된 `checkbox.json` Lottie 리소스를 사용합니다.

## 설정과 생성

애플리케이션 초기화 시 Components 설정을 먼저 적용합니다. 다음 예는 초기화된 부모 View에 선택된 옵션을 추가합니다.

```cpp
#include <dali-ui-components/dali-ui-components.h>

using namespace Dali;
using namespace Dali::Ui;

void ConfigureComponents()
{
  Components::UiConfig::New().Apply();
}
```

```cpp
void OnOptionChanged(View sender, bool selected, InputEvent event)
{
  // Store the selected state in the application model.
}

CheckBox AddOption(View parent)
{
  auto style = CheckBoxStyle::Default().Configure()
    .SetIconWidth(28.0f).SetIconHeight(28.0f)
    .SetLabelGap(12.0f).SetFontSize(18.0f)
    .Build();
  auto option = CheckBox::New("Receive updates", style);
  option.SetSelected(true);
  option.SelectionChangedSignal().Connect(&OnOptionChanged);
  parent.Add(option);
  return option;
}
```

생성 API는 `New()`, `New(style)`, `New(text)`, `New(text, style)`입니다. 스타일을 생략하면 `CheckBoxStyle::Default()`가 사용됩니다. `SetText("")`로 box-only 형태로 바꿀 수 있습니다. 라벨 유무가 달라지면 재측정됩니다.

## 선택 상태와 애니메이션

`IsSelected()`, `SetSelected(bool)`, `SelectionChangedSignal()` 및 `SetToggleByClickEnabled(bool)`은 SelectableView에서 상속합니다. `SelectionChangedSignal()`은 `void(View, bool, InputEvent)`이며 사용자 조작뿐 아니라 프로그램의 상태 변경도 전달할 수 있습니다. 위 예에서는 초기값을 먼저 설정하고 콜백을 연결합니다.

클릭으로 상태가 바뀌지 않게 하려면 `SetToggleByClickEnabled(false)`를 사용합니다. 이는 컨트롤 전체를 비활성화하거나 `SetSelected()` 호출을 막는 API가 아닙니다. 전체 비활성화에는 `SetEnabled(false)`를 사용합니다. 독립적인 옵션이 아니라 단일 선택 그룹이 필요하면 RadioButton 문서를 참고하세요.

`SetSelectionAnimationMode()`는 다음 정책을 설정합니다. 화면 밖이거나 보이지 않으면 애니메이션 대신 최종 상태를 적용합니다.

| 값 | 동작 |
|---|---|
| `AUTO` (기본) | 보이는 화면 안에서 사용자에 의한 변경만 애니메이션 적용 |
| `ENABLED` | 보이는 화면 안에서는 프로그램 변경도 애니메이션 적용 |
| `DISABLED` | 모든 변경을 즉시 반영 |

## 스타일과 크기

`CheckBoxStyle::Default().Configure()`로 기본 스타일을 독립적으로 복제합니다. 내장 기본값이 필요하면 `DefaultPreset()`을 사용합니다. Builder는 이동 전용이며 `Build()` 후 재사용하지 않습니다. 생성 이후 인스턴스에는 텍스트·폰트·색상·밑줄·아이콘 크기 setter가 제공됩니다.

| 스타일 항목 | 내장 기본값 |
|---|---|
| 최소 너비·높이 | 0·0 |
| padding / 라벨 간격 | 각 방향 8 / 8 |
| 아이콘 너비·높이 | 36×36 |
| 비선택 / 선택 색상 | `OUTLINE` / `PRIMARY` |
| 라벨 색상 / 폰트 | `ON_SURFACE` / `SamsungOneUI600`, 16 |
| 상태 효과 | 원형 Overlay 효과 |

최소 크기·padding·아이콘 크기·라벨 간격·폰트 크기는 유한한 0 이상 값만 허용합니다. 아이콘의 0은 미지정입니다. 너비는 결정된 높이를 따르고 높이는 콘텐츠 높이를 따릅니다. 따라서 IconButton의 명시적 0 크기와 의미가 다릅니다. `GetIconWidth/Height()`는 실제 배치 크기가 아닌 요청값을 반환합니다. 컨트롤 전체 크기는 아이콘·라벨·padding과 부모의 배치 제약을 함께 반영합니다.

`SetIconGenerator(IconGenerator&&)`는 `SelectableImageInterface`를 만드는 사용자 팩터리를 받습니다. 생성기는 스타일 복제본 간 공유될 수 있으므로 상태 없는 함수를 권장합니다. 사용자 Lottie의 프레임 구간과 색상 키 경로는 실제 리소스에 맞춰야 합니다. 빌드 가능한 사용자 생성기 예제는 아래 샘플을 참고하세요.

## 접근성과 리소스

역할은 `CHECK_BOX`이며 선택 상태는 접근성 `CHECKED`에 반영됩니다. 라벨 텍스트가 기본 이름이고 명시적 `SetAccessibilityName()`이 우선합니다. 라벨 없는 체크박스에는 의미 있는 접근성 이름을 지정하세요. 내부 라벨·아이콘은 중복 노출되지 않으며 비활성 상태도 동기화됩니다.

기본 Lottie와 사용자 아이콘 리소스가 설치되어 있어야 합니다. 리소스가 누락되어 그림이 보이지 않는 경우를 상태 변경 실패와 구분하세요. 실제 스크린리더 발화와 GUI 애니메이션은 실행 환경에서 별도 확인해야 합니다.

## 관련 자료

- [CheckBox API](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/check-box.h)
- [CheckBoxStyle](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/styles/check-box-style.h)
- [SelectableView API](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-foundation/public-api/views/selectable-view.h)
- [샘플과 사용자 아이콘 생성기](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/check-box)
- [수동 시나리오](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/manual-tests/dali-ui-components/tc/tc-check-box-basics.md)
- [UTC](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/automated-tests/src/dali-ui-components/utc-Dali-CheckBox.cpp)
- [RadioButton](https://github.sec.samsung.net/NUI/dali-ui/wiki/RadioButton-%28kr%29)
