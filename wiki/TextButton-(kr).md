[English](https://github.sec.samsung.net/NUI/dali-ui/wiki/TextButton) · [컴포넌트 목록](https://github.sec.samsung.net/NUI/dali-ui/wiki/Components-%28kr%29)

# TextButton

`Dali::Ui::TextButton`은 텍스트로 동작을 표현하는 `InteractiveView`입니다. 선택 상태를 저장하는 컨트롤이 아니며, 체크 옵션에는 CheckBox를 사용합니다. 아이콘 전용 동작에는 IconButton을 사용합니다.

## 설정과 기본 사용

애플리케이션 초기화 과정에서 스타일 조회와 컴포넌트 생성 전에 Components 설정을 한 번 적용합니다. 아래 `AddSaveButton`은 초기화된 부모 View에 버튼을 추가하는 예입니다.

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
void OnSaveClicked(View sender, InputEvent event)
{
  // Start the application action here.
}

TextButton AddSaveButton(View parent)
{
  auto style = TextButtonStyle::Default().Configure()
    .SetMinimumHeight(48.0f)
    .SetPadding(Insets(20.0f, 20.0f, 12.0f, 12.0f))
    .SetFontSize(18.0f)
    .Build();
  auto button = TextButton::New("Save", style);
  button.ClickedSignal().Connect(&OnSaveClicked);
  parent.Add(button);
  return button;
}
```

`New()`, `New(style)`, `New(text)`, `New(text, style)`을 제공합니다. 스타일을 생략하면 현재 설정의 `TextButtonStyle::Default()`를 사용합니다. 기본 생성자 `TextButton()`은 빈 핸들이므로 실제 사용에는 `New()`가 필요합니다.

## 스타일과 런타임 API

`Default()`는 설정된 스타일을, `DefaultPreset()`은 내장 기본값을 반환합니다. `Configure()`는 원본과 독립적인 Builder를 만들며 성공한 `Build()`는 이동 전용 Builder를 소비합니다. 생성에 사용한 스타일은 이후 런타임 속성을 자동으로 덮어쓰는 실시간 연결이 아닙니다.

| 대상 | API |
|---|---|
| 버튼 크기 제약 | Builder의 `SetMinimumWidth/Height`, `SetMaximumWidth/Height`, `SetMinimumSize`, `SetMaximumSize` |
| 외형 | Builder의 `SetPadding`, `SetBackgroundColor`, `SetCornerRadius`, `SetCornerRadiusPolicy`, `SetStateEffect` |
| 텍스트 갱신 | 버튼의 `SetText`, `GetText` |
| 타이포그래피 | 스타일 또는 버튼의 `SetTextColor`, `SetFontSize`, `SetFontFamily`, `SetTextUnderline` |
| 라벨 정렬 | 스타일 또는 버튼의 `SetHorizontalAlignment`, `SetVerticalAlignment` |

폰트 크기·최소 크기·여백·반경은 유한한 0 이상 값이어야 합니다. 최대 크기는 유한한 0 이상 값 또는 `UNCONSTRAINED_MAX_SIZE`입니다. 각 축의 최소값이 제한된 최대값을 넘으면 `Build()`가 assertion을 발생시키며, 이 경우 Builder를 수정해 다시 빌드할 수 있습니다. 폰트 크기 0은 기본값으로 대체되지 않고 내부 Label에 그대로 전달됩니다.

내장 기본값은 최소 10×10, 최대 제한 없음, 좌우 여백 16·상하 12, 상대 모서리 반경 0.5, 가운데 정렬, 폰트 `SamsungOneUI600`·16입니다. 배경은 `PRIMARY`, 텍스트는 `ON_PRIMARY` 색상 토큰이며 설정된 기본 스타일은 이와 다를 수 있습니다. 폰트 설치 여부에 따라 대체 폰트가 사용될 수 있습니다.

## 이벤트와 접근성

상속된 `ClickedSignal()`의 콜백 형식은 `void(View, InputEvent)`입니다. 멤버 콜백은 연결 추적이 가능한 컨트롤러의 `this`와 연결합니다. 클릭 이벤트와 업무 동작은 앱에서 연결하며, TextButton에 별도의 토글 상태 API는 없습니다. `SetEnabled(false)`로 비활성화합니다.

접근성 역할은 `BUTTON`입니다. 표시 텍스트가 기본 접근성 이름이며 `SetAccessibilityName()`으로 지정한 이름이 우선합니다. 내부 Label은 중복 읽기를 막기 위해 접근성 트리에서 숨겨집니다. 활성 상태도 접근성 상태와 동기화됩니다. 실제 키보드 이동과 스크린리더 발화는 앱 환경에서 별도 검증해야 합니다.

## 관련 자료

- [공개 API](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/text-button.h)
- [TextButtonStyle](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/styles/text-button-style.h)
- [수동 예제 소스](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/manual-tests/dali-ui-components/tc/tc-text-button-basics.cpp)
- [수동 검증 시나리오](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/manual-tests/dali-ui-components/tc/tc-text-button-basics.md)
- [UTC](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/automated-tests/src/dali-ui-components/utc-Dali-TextButton.cpp)
- [공통 스타일 계약(영문)](https://github.sec.samsung.net/NUI/dali-ui/wiki/ComponentConsistency)
