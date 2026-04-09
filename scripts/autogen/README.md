# Chaining Method Code Generator

Fluent method chaining을 위한 코드 자동 생성 도구입니다.

## 목적

DALi UI의 View, Label 등 public-api 클래스는 fluent chaining 패턴을 지원합니다:

```cpp
Label::New()
  .SetText("Hello")
  .SetFontSize(24)
  .SetBackgroundColor(UiColor::RED)
  .SetOpacity(0.8f);  // View에서 상속받은 메소드
```

derived class(Label)에서 부모(View)의 체이닝 메소드를 사용하려면
리턴 타입을 `Label&`로 오버로딩해야 합니다.
이 스크립트가 해당 오버로딩 코드를 자동 생성합니다.

## 매크로 생성 스크립트

소스 코드 빌드시 자동 실행되며, `@CHAIN_START` 태그를 포함하는 헤더를 대상으로 아래 작업을 진행합니다.

* `@CHAIN_START` 와 `@CHAIN_END`로 지정되는 블럭 안에 위치한 `ClassName& SetXXX` 패턴의 메소드를 수집
* 블럭 안에서 `@CHAIN_MANUAL` 로 태깅되어 있는 메소드를 수집
* 수집된 메소드들을 오버로딩 해주는 `DALI_UI_CHAIN_{ClassName}_METHODS` 이라는 매크로를 작성
* `classname.autogen.h`에 저장
* 대상 헤더에 `classname.autogen.h`를 인클루드

### 사용 예

```cpp
// view.h — base class
class View : public CustomActor
{
  //@CHAIN_START(View)

  View& SetOpacity(float opacity);

  float GetOpacity() const;

  //@CHAIN_MANUAL
  View& EnableFocusEscape(bool enable);

  //@CHAIN_END
};

// label.h — derived class
class Label : public View
{
  //@CHAIN_START(Label, View)

  Label& SetText(const Dali::String& text);

  //@CHAIN_END
};
```

## 생성 결과

위 코드에 대해 다음 파일이 생성됩니다:

* **view.autogen.h** — `DALI_UI_CHAIN_VIEW_METHODS` 매크로가 총 2개 메소드 오버로딩 (`SetOpacity`, `EnableFocusEscape`)

* **label.autogen.h** — `DALI_UI_CHAIN_LABEL_METHODS` 매크로가 2개의 View 메소드 및 1개의 Label 메소드 오버로딩 (`SetText`)

앱 개발자는 생성된 매크로를 사용하여 자신의 클래스에서 체이닝 메소드를 오버로드 할 수 있습니다:

```cpp
#include <dali-ui-foundation/public-api/label.h>

class MyLabel : public Label
{
public:
  DALI_UI_CHAIN_LABEL_METHODS(MyLabel)
};

// View + Label의 모든 체이닝 메소드가 MyLabel& 리턴으로 사용 가능
MyLabel::New()
  .SetText("Hello")      // Label 메소드
  .SetOpacity(0.5f);      // View 메소드
```

<br/>

## 확장 라이브러리에서 사용

스크립트와 CMake 모듈을 빌드에 통합하면 확장 라이브러리에서도
동일한 태그 기반 코드 생성을 사용할 수 있습니다.

## CMake 통합

```cmake
# autogen_chain.cmake를 include
include(/path/to/scripts/autogen/autogen_chain.cmake)

# 타겟과 헤더 디렉토리를 등록
dali_ui_autogen_chaining_macro(my-library ${CMAKE_SOURCE_DIR}/public-api)
```

이렇게 하면 빌드 시 `public-api/` 하위의 모든 헤더를 스캔하여
`@CHAIN_START` 태그가 있는 파일에 대해 autogen 파일을 생성합니다.

## 직접 실행

```bash
python3 gen-chaining-macro.py <directory_or_file> [...]
```

디렉토리를 지정하면 하위의 모든 `.h` 파일을 스캔합니다.

---

# Animation Bridge / Spec Code Generator

컴포넌트 헤더의 `@ANIMATABLE` / `@ANIMATABLE_MANUAL` 태그를 읽어
Animation Bridge, Spec, Impl 파일을 자동 생성합니다.

빌드 시 CMake에서 자동 실행됩니다 (`autogen_anim.cmake`).

## 태그

### @ANIMATION_CONFIG — 클래스 등록

컴포넌트 헤더에 한 줄 추가하여 해당 클래스를 animation autogen 대상으로 등록합니다.
클래스 이름만 지정하면 Bridge/Spec/Impl 이름은 자동 도출됩니다.

```cpp
// view.h — 루트 클래스
// @ANIMATION_CONFIG(View)

// label.h — View를 상속하는 클래스
// @ANIMATION_CONFIG(Label, View)
```

| Tag | 도출 |
|-----|------|
| `@ANIMATION_CONFIG(View)` | `ViewAnimationBridge`, `ViewAnimationSpec`, `ViewAnimationSpecImpl` |
| `@ANIMATION_CONFIG(Label, View)` | `LabelAnimationBridge`, `LabelAnimationSpec`, `LabelAnimationSpecImpl` |

두 번째 인자(부모 클래스)를 지정하면 부모의 `@ANIMATION_CONFIG`가 있는 헤더를 자동 탐색하고,
부모 프로퍼티의 inline 오버라이드를 자식 파일에 포함하여 fluent chaining을 보장합니다.

### @ANIMATABLE — 자동 apply 프로퍼티

setter 메소드의 doxygen 블록 **앞**에 태그를 추가합니다.

```cpp
// @ANIMATABLE(Actor::Property::OPACITY, float)
/**
 * @brief Sets the opacity of the view.
 */
View& SetOpacity(float opacity);

// To만 생성 (By 없음)
// @ANIMATABLE(View::Property::BORDERLINE_COLOR, UiColor, TO)
/**
 * @brief Sets the borderline color.
 */
View& SetBorderlineColor(const UiColor& color);
```

### @ANIMATABLE_MANUAL — 수동 apply 프로퍼티

커스텀 애니메이션 로직이 필요한 프로퍼티. Property::Index 없이 프로퍼티 이름과 타입만 지정.

```cpp
// @ANIMATABLE_MANUAL(BackgroundColor, UiColor)
```

개발자는 `internal/animation/*-impl.cpp`에 `Animate{PropertyName}()` static 함수만 구현하면 됩니다.

## 출력

| 파일 | 위치 | 생성 | 내용 |
|------|------|------|------|
| `{bridge}.h` | `public-api/animation/` | 자동 | Bridge 클래스 전체 |
| `{bridge}.cpp` | `public-api/animation/` | 자동 | Bridge 구현 |
| `{spec}.h` | `public-api/animation/` | 자동 | Spec handle 클래스 전체 |
| `{spec}.cpp` | `public-api/animation/` | 자동 | Spec boilerplate |
| `{impl}.h` | `internal/animation/` | 자동 | Impl 클래스 + Entry + Apply 선언 |
| `{impl}.autogen.inc` | `internal/animation/` | 자동 | Spec handle AddEntry 호출 구현 |
| `{impl}.cpp` | `internal/animation/` | **수동** | .inc include + Animate{Name} 구현 |

## CMake 통합

```cmake
# autogen_anim.cmake를 include
include(/path/to/scripts/autogen/autogen_anim.cmake)

# 타겟과 헤더 디렉토리를 등록
dali_ui_autogen_animation_spec(my-library ${CMAKE_SOURCE_DIR}/public-api)
```

빌드 시 `public-api/` 하위의 모든 헤더를 스캔하여 `@ANIMATION_CONFIG` 태그가 있는
클래스에 대해 autogen 파일을 생성합니다.

## 직접 실행

```bash
# 디렉토리 스캔 (권장)
python3 gen-animation-spec.py --scan-dir public-api/

# 단일 클래스
python3 gen-animation-spec.py --class View --header public-api/view.h
```

## 템플릿

`scripts/autogen/templates/` 에 생성 파일의 골격이 정의되어 있습니다.
생성되는 파일의 구조를 변경하려면 템플릿 파일을 수정하세요.
