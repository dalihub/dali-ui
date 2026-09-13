[English](https://github.sec.samsung.net/NUI/dali-ui/wiki/IconButton) · [컴포넌트 목록](https://github.sec.samsung.net/NUI/dali-ui/wiki/Components-%28kr%29)

# IconButton

`Dali::Ui::IconButton`은 하나의 아이콘으로 동작을 표현하는 `InteractiveView`입니다. 화면에 텍스트 라벨을 표시하지 않으므로, 아이콘의 의미를 전달하는 접근성 이름을 앱에서 지정해야 합니다. 텍스트 버튼이 필요하면 TextButton을 사용합니다.

## 설정과 기본 사용

애플리케이션 초기화 중 Components 설정을 한 번 적용한 뒤 프리셋 조회와 컴포넌트 생성을 수행합니다. 다음 예의 부모 View는 이미 초기화되어 있어야 합니다. 뒤로 가기 동작 자체는 앱의 Navigator 정책에 연결해야 합니다.

```cpp
#include <dali-ui-components/dali-ui-components.h>

using namespace Dali;
using namespace Dali::Ui;

void ConfigureComponents()
{
  Components::UiConfig::New().Apply();
}

void OnBackClicked(View sender, InputEvent event)
{
  // Ask the application navigator to go back.
}

IconButton AddBackButton(View parent)
{
  auto button = IconButton::New(IconButtonStyle::BackPreset());
  button.SetAccessibilityName("Back");
  button.ClickedSignal().Connect(&OnBackClicked);
  parent.Add(button);
  return button;
}
```

생성 API는 `New()`, `New(style)`, `New(iconUrl)`, `New(iconUrl, style)`입니다. 스타일을 생략하면 현재 설정의 `IconButtonStyle::Default()`를 사용합니다. `IconButton()`은 빈 핸들이며, `DownCast(BaseHandle)`는 타입이 다르면 빈 핸들을 반환합니다.

## 프리셋과 스타일

| 팩터리 | 아이콘 | 요청 아이콘 크기 | padding |
|---|---|---|---|
| `DefaultPreset()` | 빈 URL | 56×56 | 0 |
| `AddPreset()` | 내장 추가 아이콘 | 56×56 | 0 |
| `BackPreset()` | 내장 뒤로 가기 아이콘 | 40×40 | 각 방향 6 |
| `MorePreset()` | 내장 더 보기 아이콘 | 40×40 | 각 방향 6 |
| `Builder()` | 빈 URL | 양 축 `WRAP_CONTENT` | 0 |

위 값은 내장 프리셋 기준이며 `Default()`는 앱에서 등록한 스타일을 반환할 수 있습니다. 아이콘 크기는 컨트롤 전체 크기와 다릅니다. padding과 부모 레이아웃 제약, UI 배율이 최종 크기에 영향을 줍니다. 내장 아이콘을 사용하려면 컴포넌트 이미지 리소스가 설치되어 있어야 합니다.

`Configure()`는 원본을 변경하지 않는 독립적인 Builder를 반환합니다. Builder는 이동 전용이며 `Build()` 후 소비됩니다. 지원 필드는 아이콘 URL·너비·높이·색상과 컨트롤 padding입니다. 런타임 아이콘 모서리 반경은 `SetIconCornerRadius()`로 설정하며 IconButtonStyle의 Builder 필드는 아닙니다.

## 크기 결정과 이미지 변경

아이콘의 각 축은 **명시한 크기 → 유효한 natural size → 56 논리 단위 fallback** 순서로 결정합니다. 기본 프리셋은 이미 명시적 크기를 지정하므로 URL만 바꿔도 natural size 모드로 전환되지는 않습니다.

```cpp
IconButton AddNaturalIcon(View parent, const Dali::String& iconUrl)
{
  auto style = IconButtonStyle::Default().Configure()
    .SetIconUrl(iconUrl)
    .SetIconWidth(WRAP_CONTENT)
    .SetIconHeight(WRAP_CONTENT)
    .SetPadding(Insets(4.0f, 4.0f))
    .Build();
  auto button = IconButton::New(style);
  button.SetAccessibilityName("Open details");
  parent.Add(button);
  return button;
}

void UpdateIcon(IconButton button, const Dali::String& iconUrl)
{
  button.SetIconUrl(iconUrl);
  button.SetIconWidth(32.0f);
  button.SetIconHeight(WRAP_CONTENT);
  button.SetIconColor(UiColor(UiColor::PRIMARY));
  button.SetIconCornerRadius(Vector4(2.0f, 2.0f, 2.0f, 2.0f));
}
```

- 원시 `Builder()`에서 크기를 생략하면 `WRAP_CONTENT`입니다. 프리셋을 복제할 때는 위 예처럼 필요한 축을 `WRAP_CONTENT`로 명시합니다.
- 유효한 이미지 natural size가 준비되면 그것을 사용합니다. fallback 56은 최소 크기가 아니므로 32×16 이미지가 준비되면 padding·배율 적용 전 아이콘 크기는 32×16입니다.
- 한 축이 양수 고정값이고 다른 축이 `WRAP_CONTENT`이면 이미지 비율을 반영합니다. 예를 들어 32×16 이미지에 너비 64를 지정하면 높이는 32입니다.
- 명시적 0은 실제 0 크기이며 미지정이 아닙니다. CheckBox의 아이콘 0 처리와 다릅니다.
- `MATCH_PARENT`는 부모 제약에 따른 배치 의미를 유지합니다.
- 비동기 이미지 준비 완료나 URL 변경 시 재측정되어 임시 fallback이 고정 크기로 남지 않습니다.
- `GetIconWidth/Height()`는 요청값 또는 sentinel을 반환하며 실제 측정 크기를 반환하지 않습니다.

크기는 유한한 0 이상 값 또는 `WRAP_CONTENT`/`MATCH_PARENT`여야 합니다. padding과 모서리 반경 성분은 유한한 0 이상 값이어야 하며 잘못된 입력은 assertion 대상입니다. URL은 앱이 배포한 실제 리소스 경로로 전달하세요.

## 이벤트와 접근성

`ClickedSignal()`은 InteractiveView에서 상속하며 콜백 형식은 `void(View, InputEvent)`입니다. 앱이 실제 동작을 처리하며 IconButton 자체에는 선택 상태나 navigation 동작이 없습니다. `SetEnabled(false)`는 활성화를 막고 접근성 enabled 상태를 지웁니다. 다시 활성화하면 상태가 복원됩니다.

접근성 역할은 `BUTTON`이고 내부 이미지는 접근성 트리에서 숨겨집니다. `SetAccessibilityName("Back")`처럼 동작을 설명하는 이름을 지정하고 앱의 언어 리소스로 번역하세요. 충분한 터치 영역, 포커스 이동과 실제 스크린리더 발화는 대상 환경에서 확인해야 합니다.

## 관련 자료

- [IconButton API](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/icon-button.h)
- [IconButtonStyle](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/styles/icon-button-style.h)
- [샘플](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/icon-button)
- [샘플 실행 안내](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/samples/icon-button/README.md)
- [UTC](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/automated-tests/src/dali-ui-components/utc-Dali-IconButton.cpp)
- [TextButton](https://github.sec.samsung.net/NUI/dali-ui/wiki/TextButton-%28kr%29)
