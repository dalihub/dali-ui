# Chaining Method Code Generator

Fluent method chaining을 위한 코드 자동 생성 도구입니다.

## 상태

View와 View를 상속하는 public API 클래스의 fluent chaining은 제거되었습니다.
`View`, `Label`, `InputField`, layout class 등 앱/프레임워크 개발자가 상속할 수 있는
handle 계층에는 이 generator를 사용하지 않습니다.

이 generator는 아직 fluent style을 유지하기로 결정한 제한된 API 영역에서만 사용합니다.
예를 들어 Visual 계열과 Chart 계열처럼 설정 객체/팩토리 성격이 강하거나 별도 보류 상태인
클래스가 여기에 해당합니다.

새로운 View 계열 API에는 `@CHAIN_START`, `@CHAIN_MANUAL`, `DALI_UI_CHAIN_*` 매크로를
추가하지 마세요.

## 매크로 생성 스크립트

소스 코드 빌드시 자동 실행되며, `@CHAIN_START` 태그를 포함하는 헤더를 대상으로 아래 작업을 진행합니다.

* `@CHAIN_START` 와 `@CHAIN_END`로 지정되는 블럭 안에 위치한 `ClassName& SetXXX` 패턴의 메소드를 수집
* 블럭 안에서 `@CHAIN_MANUAL` 로 태깅되어 있는 메소드를 수집
* 수집된 메소드들을 오버로딩 해주는 `DALI_UI_CHAIN_{ClassName}_METHODS` 이라는 매크로를 작성
* `classname.autogen.h`에 저장
* 대상 헤더에 `classname.autogen.h`를 인클루드

### 사용 예

```cpp
// color-visual.h
class ColorVisual : public VisualBase
{
  //@CHAIN_START(ColorVisual, VisualBase)

  ColorVisual& SetColor(const Vector4& color);

  // @CHAIN_END
};
```

## 생성 결과

위 코드에 대해 다음 파일이 생성됩니다:

* **color-visual.autogen.h** — `DALI_UI_CHAIN_COLORVISUAL_METHODS` 매크로 생성

생성된 매크로는 해당 public API 클래스 내부에서만 사용합니다. 앱 개발자가 View를 상속한
클래스에 이 매크로를 직접 사용하는 방식은 지원하지 않습니다.

```cpp
class ColorVisual : public VisualBase
{
public:
  DALI_UI_CHAIN_VISUALBASE_METHODS(ColorVisual)
};
```

<br/>

## 확장 라이브러리에서 사용

이 generator는 dali-ui 내부 빌드용입니다. 확장 라이브러리나 앱 개발자가 View 상속
클래스의 fluent API를 생성하기 위해 사용하는 것은 권장하지 않습니다.

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
