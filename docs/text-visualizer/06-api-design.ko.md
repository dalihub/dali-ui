# TextVisualizer Public API 설계

## 목적

이 문서의 목적은 `Dali::Ui::TextVisualizer`의 public API 초안을 Dali UI 스타일에 맞게 제안하는 것이다.

이번 문서에서는 다음을 명확히 정리한다.

- `public-api/text-visualizer.h` 초안
- `integration-api/text-visualizer-impl.h` 초안
- property 제공 여부
- method API와 property API의 역할 분리
- `Prepare()` naming 검토
- `Layout()` / `Relayout()` public API 필요성 검토
- `SetExclusionBounds` / `SetExclusionRegions` naming 검토
- `ClearExclusionRegions()` 필요성
- Dali coding style 관점의 naming
- ABI / API 안정성 고려사항

애매한 내용은 추측하지 않고 `확인 필요`로 표시한다.

## 참고 파일

### 직접 참고한 파일

- `dali-ui-foundation/public-api/input-field.h`
- `dali-ui-foundation/integration-api/input-field-impl.h`
- `docs/text-visualizer/05-text-visualizer-design.ko.md`
- `docs/text-visualizer/05-layout-design.ko.md`

### 함께 참고한 규칙

- `View` 계열 public handle / internal impl 패턴
- `SetX() / GetX()` naming
- `DownCast(BaseHandle)` 패턴
- property enum + static property handler 패턴

## 현재 구조 요약

`TextVisualizer`는 다음 전제를 갖는다.

- `View` 계열 public handle이다
- 읽기 전용 텍스트 view이다
- `Prepare`와 `Layout`은 내부적으로 분리된다
- `Layout`은 view lifecycle의 `OnRelayout()`에서 자동 수행된다
- 초기 구현에서는 `style`, `ellipsis`, `selection`, `cursor`, `editing`을 제외한다

따라서 public API도 이 전제에 맞게 작고 보수적으로 설계하는 것이 맞다.

핵심 방향:

1. text state는 property + convenience method로 노출
2. expensive command인 `Prepare()`는 method로만 노출
3. exclusion region은 composite collection이므로 method 중심으로 노출
4. `Layout()` / `Relayout()`는 public API로 노출하지 않는다

## public-api/text-visualizer.h 초안

아래는 1차 구현 기준의 권장 public header 초안이다.

```cpp
#pragma once

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-foundation/public-api/ui-color.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{
class TextVisualizerImpl;
}

class DALI_UI_API TextVisualizer : public View
{
public:
  struct Property
  {
    enum
    {
      TEXT,
      FONT_FAMILY,
      FONT_SIZE,
      TEXT_COLOR,
    };
  };

public: // Creation & Destruction
  TextVisualizer();
  static TextVisualizer New();
  TextVisualizer(const TextVisualizer& textVisualizer);
  TextVisualizer(TextVisualizer&& rhs) noexcept;
  ~TextVisualizer();

public: // Operators
  TextVisualizer& operator=(const TextVisualizer& handle);
  TextVisualizer& operator=(TextVisualizer&& rhs) noexcept;

public: // Static Methods
  static TextVisualizer DownCast(BaseHandle handle);

public: // Setters for chaining
  TextVisualizer& SetText(const Dali::String& text);
  Dali::String    GetText() const;

  TextVisualizer& SetFontFamily(const Dali::String& fontFamily);
  Dali::String    GetFontFamily() const;

  TextVisualizer& SetFontSize(float fontSize);
  float           GetFontSize() const;

  TextVisualizer& SetTextColor(const UiColor& color);
  UiColor         GetTextColor() const;

public: // Commands
  void Prepare();

  void SetExclusionRegions(const Dali::Vector<Rect<float>>& regions);
  void ClearExclusionRegions();
  uint32_t GetExclusionRegionCount() const;

protected:
  explicit DALI_UI_API TextVisualizer(Integration::TextVisualizerImpl& implementation);
  explicit DALI_UI_API TextVisualizer(Dali::Internal::CustomActor* internal);
};

} // namespace Ui
} // namespace Dali
```

### 초안 해석

- `Text`, `FontFamily`, `FontSize`, `TextColor`는 상태이므로 setter/getter를 둔다
- `Prepare()`는 명령이므로 method로 둔다
- exclusion region은 collection이므로 method로 둔다
- public `Layout()` / `Relayout()`는 없다

## integration-api/text-visualizer-impl.h 초안

아래는 1차 구현 기준의 권장 impl header 초안이다.

```cpp
#pragma once

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/view-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/rendering/text-renderer.h>
#include <dali-ui-foundation/public-api/ui-color.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class TextVisualizerImpl;
using TextVisualizerImplPtr = IntrusivePtr<TextVisualizerImpl>;

class DALI_UI_API TextVisualizerImpl : public ViewImpl
{
public:
  static TextVisualizerImplPtr New();

protected:
  virtual ~TextVisualizerImpl();

public: // API
  void         SetText(const Dali::String& text);
  Dali::String GetText() const;

  void         SetFontFamily(const Dali::String& fontFamily);
  Dali::String GetFontFamily() const;

  void  SetFontSize(float fontSize);
  float GetFontSize() const;

  void    SetTextColor(const UiColor& color);
  UiColor GetTextColor() const;

  void Prepare();

  void     SetExclusionRegions(const Dali::Vector<Rect<float>>& regions);
  void     ClearExclusionRegions();
  uint32_t GetExclusionRegionCount() const;

public: // From ViewImpl
  void         OnInitialize() override;
  void         OnRelayout(const Vector2& size, RelayoutContainer& container) override;
  Vector3      GetNaturalSize() override;
  float        GetHeightForWidth(float width) override;
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;
  MeasuredSize OnArrange(const LayoutRect& bounds) override;
  void         OnPropertySet(Dali::Property::Index index, const Dali::Property::Value& propertyValue) override;

public: // Property helpers
  static void                  SetProperty(BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value);
  static Dali::Property::Value GetProperty(BaseObject* object, Dali::Property::Index index);

private:
  TextVisualizerImpl();

private:
  Dali::String               mText;
  Dali::String               mFontFamily;
  float                      mFontSize;
  UiColor                    mTextColor;
  Dali::Vector<Rect<float>>  mExclusionRegions;
  Text::RendererPtr          mRenderer;
};

} // namespace Integration
} // namespace Ui
} // namespace Dali
```

### 초안 해석

`InputFieldImpl`와 달리 아래를 의도적으로 넣지 않는다.

- `Text::EditableControlInterface`
- `Text::SelectableControlInterface`
- `Text::AnchorControlInterface`
- `InputMethodContext`
- gesture detector
- decorator 관련 멤버

즉, impl은 `ViewImpl + prepare/layout orchestration + renderer host`에 집중한다.

## property 제공 여부 검토

### 제공하는 것이 좋은 property

아래는 property로 제공하는 것이 적절하다.

- `TEXT`
- `FONT_FAMILY`
- `FONT_SIZE`
- `TEXT_COLOR`

이유:

1. 모두 "지속 상태(state)"다.
2. setter/getter와 property가 자연스럽게 대응된다.
3. Dali UI의 기존 text control 스타일과 일관된다.

### 1차 구현에서 property로 제공하지 않는 것이 좋은 항목

- `Prepare`
- exclusion region collection
- layout result
- prepared state

이유:

1. `Prepare`는 상태가 아니라 명령이다.
2. exclusion region은 collection 타입이며 변경 빈도가 높고 property로 다루기 번거롭다.
3. prepared/layout state를 property로 노출하면 동기화 의미가 불명확해진다.

### 권장 property 결론

1차 구현에서는 "기본 text state만 property 제공"이 맞다.

## method API와 property API의 역할 구분

권장 원칙은 아래와 같다.

### property API

- view의 지속 상태를 나타낸다
- property system을 통해 set/get 가능해야 한다
- convenience method와 1:1 대응한다

예:

- `TEXT`
- `FONT_FAMILY`
- `FONT_SIZE`
- `TEXT_COLOR`

### method API

- 명령(command) 또는 composite object update를 담당한다
- property와 1:1 대응하지 않아도 된다
- 실행 시점 의미가 중요하다

예:

- `Prepare()`
- `SetExclusionRegions(...)`
- `ClearExclusionRegions()`

즉, 이번 API 설계는 "상태는 property, 명령과 컬렉션은 method"로 정리하는 것이 적절하다.

## Prepare() naming 검토

### 후보

- `Prepare()`
- `UpdatePreparedText()`
- `EnsurePrepared()`
- `BuildPreparedText()`

### 권장안

`Prepare()`가 가장 적절하다.

이유:

1. 기존 설계 문서에서 이미 `Prepare / Layout` 2단계 용어를 사용하고 있다
2. 길이가 짧고 Dali style에 맞는다
3. expensive preprocessing을 명시적으로 실행한다는 의미가 분명하다

### 주의점

`Prepare()`는 public API로 노출되더라도 "호출하지 않으면 동작하지 않는 API"가 되면 안 된다.

권장 의미:

- 명시적으로 미리 준비를 수행하고 싶을 때 호출
- 호출하지 않아도 내부적으로 필요 시 `OnRelayout()`에서 자동 실행

즉, `Prepare()`는 `optional eager command`로 해석하는 것이 맞다.

## Layout() 또는 Relayout() API 필요성 검토

### 결론

1차 구현에서는 public `Layout()` 또는 `Relayout()` API를 두지 않는 것이 맞다.

### 이유

1. `View` 계열은 원래 layout lifecycle이 존재한다.
2. public `Layout()`은 parent layout system과 의미가 충돌하기 쉽다.
3. public `Relayout()`은 `ViewImpl::OnRelayout()`와 이름이 너무 가깝고 혼동을 만든다.
4. `TextVisualizer`가 직접 layout을 수행한다 해도, 외부에서는 결국 bounds 변경과 relayout request를 통해 동작하는 것이 자연스럽다.

### 만약 추후 필요하다면

추후 명시적 API가 꼭 필요할 경우 다음처럼 더 명확한 이름이 낫다.

- `UpdateLayout()`
- `CalculateLayout()`

하지만 1차 구현에서는 public 노출을 보류하는 것이 적절하다.

## SetExclusionBounds / SetExclusionRegions naming 검토

### 후보 비교

#### `SetExclusionBounds`

장점:

- 입력이 rectangle 배열이라는 점이 직접 드러난다

단점:

- 향후 region 형태가 rectangle 외로 확장되면 이름이 좁다
- "bounds"는 보통 view 자신의 bounds와 혼동될 수 있다

#### `SetExclusionRegions`

장점:

- 의미가 더 상위 개념이다
- 현재는 rectangle list여도, 미래에 shape / rounded region / path로 확장 가능하다
- "텍스트를 배치하지 않을 영역 집합"이라는 의미가 자연스럽다

단점:

- 현재 구현이 rectangle 기반이라는 점은 이름만으로는 드러나지 않는다

### 권장안

`SetExclusionRegions()`가 더 적절하다.

단, ABI 안정성을 위해 public parameter type은 별도 public struct보다 아래처럼 단순하게 두는 것이 좋다.

```cpp
void SetExclusionRegions(const Dali::Vector<Rect<float>>& regions);
```

즉, naming은 `Regions`, type은 `Rect<float>` 집합으로 가는 것이 1차 구현에 가장 안정적이다.

## ClearExclusionRegions 필요성

### 결론

`ClearExclusionRegions()`는 필요하다.

### 이유

1. 빈 vector를 전달하는 패턴보다 의도가 더 명확하다
2. property가 아닌 method API로 exclusion region을 다룰 때 대칭 API가 된다
3. language binding이나 script binding에서도 사용성이 좋다

권장 시그니처:

```cpp
void ClearExclusionRegions();
```

추가로 `GetExclusionRegionCount() const` 정도는 read-only query로 둘 수 있다.

## Dali coding style 관점의 naming

이번 초안에서 권장하는 naming은 다음과 같다.

- 타입명: `TextVisualizer`
- impl 타입명: `TextVisualizerImpl`
- 생성: `New()`
- downcast: `DownCast(BaseHandle)`
- setter/getter: `SetText()`, `GetText()`
- command: `Prepare()`
- collection update: `SetExclusionRegions()`
- clear: `ClearExclusionRegions()`

### 피하는 것이 좋은 이름

- `DoPrepare()`
- `RunPrepare()`
- `ApplyLayout()`
- `Relayout()`
- `SetExcludedBounds()`
- `SetExclusionRects()`

이유:

- Dali public API 관점에서 너무 내부 구현 지향적이거나
- existing lifecycle과 충돌하거나
- rectangle 구현 상세를 과하게 드러낸다

## ABI/API 안정성 고려사항

### 1. public type을 최소화한다

`PreparedText`, `LayoutResult`, `ClusterPlacement` 같은 내부 구조는 public API에 노출하지 않는 것이 맞다.

이유:

- 향후 layout engine 변경 시 ABI 부담이 커진다
- 1차 구현에서 구조가 자주 바뀔 가능성이 높다

### 2. exclusion region용 새 public struct는 신중해야 한다

public `struct TextExclusionRegion`을 노출하면 향후 필드 추가가 ABI 부담이 된다.

따라서 1차 구현에서는 새 public struct보다 다음이 더 안전하다.

```cpp
Dali::Vector<Rect<float>>
```

### 3. property enum은 초기에 작게 유지한다

처음부터 많은 property를 공개하면 이후 제거가 어렵다.

권장:

- 1차는 `TEXT`, `FONT_FAMILY`, `FONT_SIZE`, `TEXT_COLOR`만

### 4. command API의 동기 의미를 문서화해야 한다

`Prepare()`는 sync인지 lazy trigger인지 사용자 입장에서 오해가 생길 수 있다.

권장 문서화:

- `Prepare()`는 현재 state 기준으로 prepared cache를 갱신한다
- 필요 시 내부 relayout에서도 자동 호출된다

### 5. internal impl 의존 타입은 public에 새지 않게 한다

public header에서는:

- renderer
- prepared model
- layout result
- atlas adapter

같은 내부 타입을 forward declare도 하지 않는 편이 안전하다.

## TextVisualizer에 필요한 부분

### 직접 필요한 내용

- `View` 계열 handle/impl 패턴
- minimal state property
- explicit `Prepare()` method
- `SetExclusionRegions()` / `ClearExclusionRegions()` method
- public `Layout()` 비노출
- 작은 API surface

### 참고용 내용

- `GetExclusionRegionCount()`
- future `LAYOUT_DIRECTION_MODE` property 추가 가능성
- 향후 `font weight / width / slant` 추가 가능성

## 버릴 부분

1차 public API에서는 아래를 노출하지 않는다.

- selection API
- cursor API
- editable API
- IME / clipboard API
- style API
- ellipsis API
- explicit public `Layout()` / `Relayout()`
- prepared/layout result 내부 구조

## 설계 결론

`TextVisualizer`의 public API는 `InputField`처럼 넓게 시작하면 안 되고, `View` + minimal text state + explicit prepare command + exclusion collection API 정도로 작게 시작하는 것이 맞다.

권장 public surface는 아래로 요약된다.

- state: `Text`, `FontFamily`, `FontSize`, `TextColor`
- command: `Prepare()`
- exclusion: `SetExclusionRegions()`, `ClearExclusionRegions()`
- layout: public `Layout()` 비노출

## 확인 필요 사항

- `TEXT_COLOR`를 1차 property에 포함할지, render host 완성 시점에 함께 넣을지 최종 확정 필요
- `LAYOUT_DIRECTION_MODE`를 1차 범위에 넣을지 여부 확정 필요
- companion property index header를 별도 파일로 둘지 (`text-visualizer-properties.h`) 구현 시 확정 필요

## TextVisualizer 설계에 주는 결론

1. `TextVisualizer`는 `View` 계열 public handle로 설계하되, API surface는 매우 작게 유지하는 것이 맞다.
2. `Prepare()`는 public method로 두되, `Layout()` / `Relayout()`는 public에 노출하지 않는 것이 적절하다.
3. exclusion 영역은 property보다 method API가 적합하며, naming은 `SetExclusionRegions()`가 가장 자연스럽다.
4. ABI 안정성을 위해 내부 layout/prepare 구조와 새 public composite type 노출은 최소화해야 한다.
