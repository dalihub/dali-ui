# Text Overview

[→ English](https://github.sec.samsung.net/NUI/dali-ui/wiki/Text)

## Overview

dali-ui의 text 기능은 모두 `View` 기반 컴포넌트로 제공됩니다. 텍스트 렌더링과 레이아웃은 내부 text backend가 처리하며, 앱 개발자는 API를 통해 텍스트 내용과 스타일을 설정합니다.

| Component | 용도 | 주요 기능 |
|---|---|---|
| [Label](https://github.sec.samsung.net/NUI/dali-ui/wiki/Label-(kr)) | 읽기 전용 텍스트 표시 | multi-line, text style, marquee, text fit, async rendering |
| [InputField](https://github.sec.samsung.net/NUI/dali-ui/wiki/Text-Input-(kr)) | 한 줄 텍스트 입력 | placeholder, cursor, selection, password, input filter |
| [InputEditor](https://github.sec.samsung.net/NUI/dali-ui/wiki/Text-Input-(kr)) | 여러 줄 텍스트 입력 | multi-line editing, auto grow, line count |

읽기 전용 텍스트는 `Label`, 한 줄 입력은 `InputField`, 여러 줄 입력은 `InputEditor`를 사용합니다.

<br/>

## Text 기능 분류

세 component가 동일한 API를 모두 제공하는 것은 아니지만, dali-ui text 기능은 대략 다음 범주로 나눌 수 있습니다.

| 범주 | 관련 기능 | 주요 component |
|---|---|---|
| Text layout | multi-line, wrap, alignment, overflow, line height | Label, InputEditor |
| Text style | color, underline, shadow, outline, background | Label, InputField, InputEditor |
| Font | font family, size, weight, width, slant, variation | Label, InputField, InputEditor |
| Text input | placeholder, cursor, selection, input filter | InputField, InputEditor |
| Advanced label rendering | text fit, marquee, async rendering, render scale, cutout | Label |
| Localization | translatable text/placeholder, binding | Label, InputField, InputEditor |

<br/>

## Text Size 측정

Text component의 크기는 일반 View와 동일하게 layout system의 measure/arrange 과정에서 결정됩니다. 텍스트는 font, style, line breaking 결과에 따라 size가 달라질 수 있습니다. `GetNaturalSize()`는 width/height 제약 없이 text content를 layout했을 때의 size를 조회하고, `GetHeightForWidth(width)`는 특정 width에서 text content를 layout했을 때의 height를 조회합니다.

| API | 설명 |
|---|---|
| `GetNaturalSize()` | width/height 제약 없이 text content를 layout했을 때의 size 반환 |
| `GetHeightForWidth(float width)` | 주어진 width에서 text content를 layout했을 때의 height 반환 |
| `GetLineCount()` | 현재 layout width 기준 line count 반환 |
| `GetLineCount(float width)` | 지정한 width 기준 line count 반환 |

~~~cpp
Vector3 naturalSize = label.GetNaturalSize();

float height = label.GetHeightForWidth(300.0f);

int lineCount = label.GetLineCount(300.0f);
~~~

> [!NOTE]
> `GetNaturalSize()`와 `GetHeightForWidth()`는 padding을 포함한 값을 기준으로 동작합니다. `GetHeightForWidth(width)`에 전달하는 width는 padding을 포함한 전체 width이며, 반환되는 height도 padding을 포함합니다.

> [!WARNING]
> Text size 측정은 text model update와 layout을 유발할 수 있는 비용이 있는 연산입니다. text 속성이 변경되지 않은 경우 캐시된 값을 사용할 수 있지만, 매 frame 반복 호출하는 방식은 피하는 것이 좋습니다.

> [!NOTE]
> `GetLineCount()`처럼 현재 layout width에 의존하는 값은 layout resolve 전에는 정확하지 않을 수 있습니다. 필요한 경우 `GetLineCount(width)`처럼 width를 명시하는 API를 사용하세요.

<br/>

## Text Layout Direction

`Text::LayoutDirectionMode`는 text component 내부에서 text content의 기준 방향을 어떻게 결정할지 제어합니다. RTL 문자가 포함된 문자열은 Unicode Bidirectional Algorithm에 따라 시각적 순서가 결정될 수 있으며, 이 mode는 해당 text layout의 기준 방향을 정하는 데 사용됩니다.

기본 mode는 `UiConfig`에서 설정합니다.

~~~cpp
UiConfig::New()
  .SetTextLayoutDirectionMode(Text::LayoutDirectionMode::INHERIT)
  .Apply();
~~~

각 text view에서도 직접 설정할 수 있습니다:

~~~cpp
Label label = Label::New()
  .SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS);
~~~

| Mode | 설명 |
|---|---|
| `INHERIT` | parent View의 layout direction을 따름 |
| `CONTENTS` | text 내용으로 direction 판단 |
| `LOCALE` | system locale 기준 |

> [!NOTE]
> 일반적인 다국어 UI에서는 상위 View의 layout direction을 따르도록 `INHERIT`를 사용하는 것이 자연스럽습니다.

자세한 내용: [Localization & Multilingual UI](https://github.sec.samsung.net/NUI/dali-ui/wiki/Localization-&-Multilingual-UI-(kr))

<br/>

## Samples

| 기능 | 샘플 |
|---|---|
| 기본 | [text-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-example.cpp) |
| Layout direction | [text-layout-direction-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-layout-direction-example.cpp) |
| Markup | [text-markup-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-markup-example.cpp) |
| Style | [text-style-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-style-example.cpp) |
| Bevel | [text-style-bevel-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-style-bevel-example.cpp) |
| Font variation | [text-font-variation-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-font-variation-example.cpp) |
| Text fit | [text-fit-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-fit-example.cpp) |
| Text fit candidate | [text-fit-candidate-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-fit-candidate-example.cpp) |
| Marquee | [text-marquee-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-marquee-example.cpp) |
| Render scale | [text-render-scale-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-render-scale-example.cpp) |
| Cutout / Mask | [text-cutout-mask-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-cutout-mask-example.cpp) |
| Input | [text-input-field-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-input-field-example.cpp), [text-input-editor-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-input-editor-example.cpp) |
| Localization | [text-localization-po-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-localization-po-example.cpp) |

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#documents)
