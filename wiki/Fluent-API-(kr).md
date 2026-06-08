> [!CAUTION]
> This page is no longer valid (2026.06.08~)

<br/>

# Fluent API
복잡한 UI 계층 구조를 선언형의 메서드 체인으로 보다 쉽게 표현할 수 있도록 하는 장치입니다.

```cpp
#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali::UI;

void Create(Application& application) {
  application.GetWindow().Add(
    StackLayout::New(StackOrientation::VERTICAL)
      .SetBackgroundColor(UiColor::PRIMARY)
      .Contents({
        View::New()
          .SetBackgroundColor(UiColor(0xFF00FF))
          .SetRequestedWidth(100_spx)
          .SetRequestedHeight(100_spx)
          .With([](View& v) {
            v.TouchedSignal().Connect([](Actor a, const TouchEvent& t) {
              /* Describes on touch handler */
              return true;
            });
          }),
        View::New()
          .SetBackgroundColor(UiColor(0x0000FF))
          .SetRequestedWidth(100_spx)
          .SetRequestedHeight(100_spx)
          .As(mSecondChild)
      })
  );
}
```

<br/>

## `As`와 `With`

`As`와 `With`는 `View`의 유틸리티 메서드로, fluent 메서드 체인 안에서 자연스럽게 사용할 수 있도록 설계되었습니다.

### `As` — 참조 캡처

`As`는 체인을 끊지 않고 현재 뷰를 외부 변수에 할당합니다.
선언형 UI 트리 안에서 인라인으로 생성된 뷰의 참조를 나중에 사용하고 싶을 때 유용합니다.

```cpp
Label mLabel;

View::New()
  .Children({
    Label::New("Hello")
      .SetTextColor(UiColor::PRIMARY)
      .As(mLabel),   // mLabel이 이 Label을 가리키게 됨
  });

// 이후 mLabel을 통해 접근 가능
mLabel.SetText("Updated");
```

### `With` — 커스텀 로직 삽입

`With`는 현재 뷰에 람다를 실행한 뒤 뷰를 그대로 반환합니다.
시그널 연결처럼 단일 setter로 표현하기 어려운 초기화 로직을 체인을 끊지 않고 삽입할 때 사용합니다.

```cpp
View::New()
  .SetBackgroundColor(UiColor::PRIMARY)
  .With([](View& v) {
    v.StateChangedSignal().Connect([](View view, StateEvent event) {
      // 상태 변경 처리
    });
  });
```

> [!NOTE]
> `With`는 시그널 연결이나 조건부 초기화를 인라인으로 처리할 때 특히 유용합니다.

<br/>

## Extending Fluent API (for Framework Developers)

dali-ui의 Fluent API는 메서드 체이닝이 클래스 상속 경계를 넘어서도 끊기지 않도록 설계되어 있습니다.
`View`를 상속한 새로운 컨트롤을 개발할 때 이 메커니즘을 이해하고 올바르게 확장해야 합니다.

<br/>

### 왜 문제가 생기는가

C++에서 `View`의 setter가 `View&`를 반환하면, `Label` 인스턴스에서 호출해도 `View&`가 반환됩니다.
그 결과 `Label` 고유의 메서드를 이어서 체이닝할 수 없게 됩니다.

```cpp
// ❌ 컴파일 에러: SetPositionX()가 View&를 반환하므로 SetText()를 이어 부를 수 없음
Label::New()
  .SetPositionX(10_dp)   // returns View&
  .SetText("hello");     // error: View has no SetText()
```

<br/>

### dali-ui의 해법: autogen 매크로

dali-ui는 이 문제를 **코드 생성 매크로**로 해결합니다.

빌드 시 `gen-chaining-macro.py` 스크립트가 헤더 파일을 분석하여 `*.autogen.h` 파일을 자동 생성합니다.
이 파일에는 `DALI_UI_CHAIN_{CLASS}_METHODS(ChildClass)` 매크로가 정의되어 있으며,
부모 클래스의 모든 setter를 `ChildClass&`를 반환하도록 래핑합니다.

> [!IMPORTANT]
> 코드 생성 스크립트는 dali-ui 소스 코드 한정으로 빌드 타임에 자동으로 실행됩니다.
> 외부 라이브러리에 적용하고 싶을 경우 [여기](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/scripts/autogen)를 참고하세요.

예를 들어 `view.autogen.h`의 매크로는 다음과 같은 형태입니다:

```cpp
#define DALI_UI_CHAIN_VIEW_METHODS(ChildClass)              \
  ChildClass& SetPositionX(float x)                        \
    { View::SetPositionX(x); return *this; }               \
  ChildClass& SetRequestedWidth(float width)               \
    { View::SetRequestedWidth(width); return *this; }      \
  // ... (View의 모든 setter)
```

`Label`은 이 매크로를 클래스 끝에 삽입하여, `Label` 인스턴스에서 `View`의 setter를 체이닝해도
항상 `Label&`가 반환되도록 합니다:

```cpp
class Label : public View {
public:
  Label& SetText(const Dali::String& text);
  // ...

public:
  DALI_UI_CHAIN_VIEW_METHODS(Label)
};
```

이로써 체이닝이 올바르게 동작합니다:

```cpp
// ✅ 정상 동작
Label::New()
  .SetPositionX(10_dp)   // returns Label& (via macro)
  .SetText("hello");     // OK
```

<br/>

### 계층 전파

상속이 깊어질수록 매크로도 계층적으로 전파됩니다.
`label.autogen.h`의 `DALI_UI_CHAIN_LABEL_METHODS`는 `View` 매크로를 포함합니다:

```cpp
#define DALI_UI_CHAIN_LABEL_METHODS(ChildClass)   \
  DALI_UI_CHAIN_VIEW_METHODS(ChildClass)          \  // View의 setter 전부 포함
  ChildClass& SetText(...) { ... }                \
  ChildClass& SetFontSize(...) { ... }            \
  // ... (Label의 모든 setter)
```

따라서 `Label`을 상속하는 클래스는 `DALI_UI_CHAIN_LABEL_METHODS`만 사용하면
`View`와 `Label`의 모든 setter를 체이닝할 수 있습니다.

<br/>

### 새 클래스에 Fluent API 추가하기

새로운 컨트롤 클래스를 개발할 때는 아래 단계를 따르세요.

**1. setter 메서드를 `ClassName&` 반환으로 선언하고 어노테이션을 추가합니다**

```cpp
// @CHAIN_START(MyView, View)   ← 클래스 이름, 부모 클래스 이름
MyView& SetRadius(float radius);
MyView& SetBorderWidth(float width);
// @CHAIN_END
```

`@CHAIN_START(ClassName, ParentClass)` ~ `@CHAIN_END` 범위 안의 `ClassName& SetXXX(...)` 형태의
setter는 자동으로 수집됩니다.

**2. 빌드하면 나머지는 자동으로 처리됩니다**

빌드 시 스크립트가 아래 작업을 자동으로 수행합니다:
- `myview.autogen.h` 생성 (`DALI_UI_CHAIN_MYVIEW_METHODS` 매크로 포함)
- 헤더 파일에 `#include "myview.autogen.h"` 삽입
- 클래스 끝에 `DALI_UI_CHAIN_VIEW_METHODS(MyView)` 삽입

> **참고**: GBS 빌드 환경에서는 autogen이 실행되지 않으므로,
> 데스크탑 빌드 후 생성된 `*.autogen.h` 파일을 저장소에 함께 커밋해야 합니다.

**3. `MyView`를 상속하는 클래스는 `DALI_UI_CHAIN_MYVIEW_METHODS`를 사용합니다**

```cpp
class MySpecialView : public MyView {
public:
  // @CHAIN_START(MySpecialView, MyView)
  MySpecialView& SetShadow(float blur);
  // @CHAIN_END
};
```

<br/>

### `@CHAIN_MANUAL`: SetXXX 패턴이 아닌 메서드

자동 수집은 `ClassName& SetXXX(...)` 패턴만 인식합니다.
다른 이름의 setter나 `Enable...`, `As...` 형태의 메서드는 `@CHAIN_MANUAL` 태그를 사용하세요:

```cpp
// @CHAIN_START(MyView, View)
MyView& SetRadius(float radius);   // 자동 수집됨

// @CHAIN_MANUAL
MyView& EnableShadow(bool enable); // SetXXX가 아니므로 수동 지정 필요
// @CHAIN_END
```

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr)#development-guides)
