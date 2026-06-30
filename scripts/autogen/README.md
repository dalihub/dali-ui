# Autogen Scripts

## Animation Bridge / Spec Code Generator

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
python3 gen-animation-spec.py --class View --header public-api/views/view.h
```

## 템플릿

`scripts/autogen/templates/` 에 생성 파일의 골격이 정의되어 있습니다.
생성되는 파일의 구조를 변경하려면 템플릿 파일을 수정하세요.
