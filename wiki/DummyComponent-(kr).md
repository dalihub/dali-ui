[English](https://github.sec.samsung.net/NUI/dali-ui/wiki/DummyComponent) · [컴포넌트 목록](https://github.sec.samsung.net/NUI/dali-ui/wiki/Components-%28kr%29)

# DummyComponent

`Dali::Ui::DummyComponent`는 View 기반 컴포넌트의 최소 구조를 보여 주는 호환용 타입입니다. 독자적인 기능, 스타일, 신호를 제공하지 않습니다. 일반적인 시각 요소나 자식 컨테이너를 만들 때는 `View::New()`를 사용하세요.

## 제공하는 API

`New()`로 초기화된 핸들을 만들고 `DownCast(BaseHandle)`로 타입을 확인합니다. 타입이 맞지 않으면 빈 핸들을 반환합니다. 기본 생성자는 빈 핸들이며, 복사는 새로운 컴포넌트 생성이 아니라 같은 객체를 가리키는 핸들 복사입니다. 배경·크기·자식 관리는 View에서 상속합니다.

Components 통합 헤더에는 이 호환 타입이 포함되어 있지 않습니다. 기존 코드를 유지보수할 때는 `public-api/dummy-component.h`를 명시적으로 포함해야 합니다.

```cpp
#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-components/public-api/dummy-component.h>

using namespace Dali;
using namespace Dali::Ui;

View AddContainer(View parent)
{
  // Prefer View for a general-purpose container.
  auto container = View::New();
  parent.Add(container);
  return container;
}

DummyComponent AddLegacyComponent(View parent)
{
  auto component = DummyComponent::New();
  parent.Add(component);
  return DummyComponent::DownCast(component);
}
```

예제는 애플리케이션이 이미 초기화되고 부모 View가 생성된 상태를 전제로 합니다. DummyComponent 자체에는 전용 스타일 조회가 없지만 다른 컴포넌트와 함께 사용하는 앱은 생성 전에 Components::UiConfig를 적용해야 합니다.

## 사용 범위와 제한

- 기존 클라이언트와의 호환성을 위한 공개 헤더·생성·캐스팅 API입니다.
- `DummyComponentStyle`이나 `New(style)`, 고유 클릭·선택 상태 API는 없습니다.
- 클릭 가능한 컨테이너에는 InteractiveView, 텍스트 동작에는 TextButton 등 목적에 맞는 타입을 선택하세요.
- 별도 접근성 역할이나 이름 정책을 추가하지 않으므로, 상호작용 컨트롤처럼 동작한다고 가정하지 마세요.
- 새로운 컴포넌트를 구현하려면 구조 예제를 그대로 제품 컨트롤로 취급하기보다 컴포넌트 생성 가이드를 참고하세요.

## 관련 자료

- [공개 API](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/dummy-component.h)
- [구현](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/internal/dummy-component-impl.cpp)
- [UTC](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/automated-tests/src/dali-ui-components/utc-Dali-DummyComponent.cpp)
- [View](https://github.sec.samsung.net/NUI/dali-ui/wiki/View-%28kr%29)
- [새 컴포넌트 만들기](https://github.sec.samsung.net/NUI/dali-ui/wiki/Create-New-Component-%28kr%29)
