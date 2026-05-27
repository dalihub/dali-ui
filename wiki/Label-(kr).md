# Label

[→ English](https://github.sec.samsung.net/NUI/dali-ui/wiki/Label)

## Overview

`Label`은 non-editable View로 텍스트를 표시합니다. 텍스트 레이아웃과 렌더링을 수행하지만, 사용자 입력이나 편집은 지원하지 않습니다. 단순 텍스트 표시부터 multi-line, style, marquee, async rendering 등 다양한 기능을 제공합니다.

<br/>

## Basic Usage

텍스트를 전달하여 생성:

~~~cpp
Label label = Label::New("Hello");

window.Add(label);
~~~

Fluent API를 사용한 생성:

~~~cpp
Label label = Label::New()
  .SetText("Hello")
  .SetFontSize(24.0f)
  .SetTextColor(UiColor::PRIMARY);
~~~

<br/>

## Text Layout

대표적인 텍스트 레이아웃 API입니다.

| API | 설명 |
|---|---|
| `SetMultiLine()` | multi-line 활성화 |
| `SetLineWrapMode()` | 줄바꿈 모드 (`WORD`, `CHARACTER`, `HYPHENATION`, `MIXED`) |
| `SetHorizontalTextAlignment()` | 수평 정렬 (`START`, `CENTER`, `END`) |
| `SetVerticalTextAlignment()` | 수직 정렬 (`START`, `CENTER`, `END`) |
| `SetOverflowMode()` | 오버플로우 처리 (`ELLIPSIS`, `CLIP`) |

~~~cpp
Label label = Label::New("Long text...")
  .SetMultiLine(true)
  .SetLineWrapMode(Text::LineWrapMode::WORD)
  .SetHorizontalTextAlignment(Text::Alignment::CENTER);
~~~

<br/>

## Line Height

`SetLineHeight()`와 `SetLineHeightMode()`로 줄 간격을 제어합니다.

- **RELATIVE** (기본): font size에 대한 배수로 계산
  - `CalculatedLineHeight(px) = fontSize(px) * lineHeight * effectiveScale`
- **ABSOLUTE**: 픽셀 단위 절대값
  - `CalculatedLineHeight(px) = lineHeight(px) * effectiveScale`

effective scale에는 UI scale과 font size scale 등이 반영됩니다.

~~~cpp
Label label = Label::New("Line height")
  .SetMultiLine(true)
  .SetLineHeight(1.4f)
  .SetLineHeightMode(Text::LineHeightMode::RELATIVE);
~~~

~~~cpp
Label label = Label::New("Auto line height")
  .SetLineHeight(Text::LINE_HEIGHT_AUTO);
~~~

> [!NOTE]
> `LINE_HEIGHT_AUTO`를 설정하면 font metrics에서 계산한 기본 line height를 사용합니다. line height 계산 결과가 font metrics 기준 높이보다 작으면, glyph가 잘리지 않도록 font metrics 기준 높이를 사용합니다.

<br/>

## Text Style

Label은 style object를 통해 underline, shadow, outline, line-through, bevel 등의 text style을 적용할 수 있습니다.

| Style | API | Clear |
|---|---|---|
| Underline | `SetUnderline(Text::Underline())` | `ClearUnderline()` |
| Shadow | `SetShadow(Text::Shadow())` | `ClearShadow()` |
| Outline | `SetOutline(Text::Outline())` | `ClearOutline()` |
| LineThrough | `SetLineThrough(Text::LineThrough())` | `ClearLineThrough()` |
| Bevel | `SetBevel(Text::Bevel())` | `ClearBevel()` |
| Text Background | `SetTextBackgroundColor(UiColor)` | `ClearTextBackgroundColor()` |

~~~cpp
// Underline
Label label = Label::New("Underline")
  .SetUnderline(Text::Underline());

// Dashed underline with color
Label label2 = Label::New("Dashed")
  .SetUnderline(
    Text::Underline()
      .SetColor(UiColor(0x0088FF))
      .SetThickness(2.0f)
      .SetType(Text::Underline::Type::DASHED)
      .SetDashLength(4.0f)
      .SetDashGap(4.0f));

// Shadow
Label label3 = Label::New("Shadow")
  .SetShadow(
    Text::Shadow()
      .SetColor(UiColor(0xFF5500))
      .SetOffset(Vector2(3.0f, 3.0f))
      .SetBlurRadius(2.0f));

// Outline
Label label4 = Label::New("Outline")
  .SetOutline(
    Text::Outline()
      .SetColor(UiColor(0x0066FF))
      .SetWidth(2.0f));

// LineThrough
Label label5 = Label::New("Strikethrough")
  .SetLineThrough(
    Text::LineThrough()
      .SetColor(UiColor(0xFF00FF))
      .SetThickness(3.0f));

// Text background color
Label label6 = Label::New("Highlighted")
  .SetTextBackgroundColor(UiColor(0xFFFF00));
~~~

참고 샘플:
- [text-style-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-style-example.cpp)
- [text-style-bevel-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-style-bevel-example.cpp)

<br/>

## Font

| API | 설명 |
|---|---|
| `SetFontFamily()` | font family 설정 |
| `SetFontSize()` | font size 설정 (pixel 단위) |
| `SetFontWeight()` | font weight 설정 (`THIN` ~ `BLACK`) |
| `SetFontWidth()` | font width 설정 (`ULTRA_CONDENSED` ~ `ULTRA_EXPANDED`) |
| `SetFontSlant()` | font slant 설정 (`NORMAL`, `ITALIC`, `OBLIQUE`) |
| `SetFontVariation()` | variable font axis 설정 |

font family를 명시하지 않으면 platform에서 설정한 default font가 사용됩니다. font family, weight, width, slant, variation은 선택된 font가 지원하는 범위 내에서 적용됩니다.

### Font Variation

문자열로 설정:

~~~cpp
Label label = Label::New("Variable Font")
  .SetFontFamily("Sans VF")
  .SetFontVariation("wght=700,wdth=90");
~~~

`FontVariationAxis`로 설정:

~~~cpp
Dali::Vector<Text::FontVariationAxis> axes;
axes.PushBack(Text::FontVariationAxis("wght", 700.0f));
axes.PushBack(Text::FontVariationAxis("wdth", 90.0f));

Label label = Label::New("Variable Font")
  .SetFontFamily("Sans VF")
  .SetFontVariation(axes);
~~~

> [!NOTE]
> 사용하는 font가 해당 variation axis를 지원하지 않으면 axis 값은 무시될 수 있습니다. 위 예시의 `"Sans VF"`는 variable font 이름의 예시입니다.

참고 샘플: [text-font-variation-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-font-variation-example.cpp)

<br/>

## UI Scale

UI scale은 Label, InputField, InputEditor의 measure/relayout 과정에서 font size와 text layout 관련 값에 반영됩니다. Font Size Scale과 달리 UI scale은 View/layout 계층의 scale 정책에 연결된 값입니다.

| 대상 | 설명 |
|---|---|
| FontSize | text layout / rendering 기준 font size에 반영 |
| LineHeight | relative/absolute line height 계산에 반영 |
| TextFit FitRange | min/max/step 값에 반영 |
| TextFit FitCandidate | font size / line height에 반영 |
| Margin / Padding | text layout 영역 계산에 반영 |
| Marquee | MarqueeGap 등 marquee 관련 layout 값에 반영 |

> [!NOTE]
> TBD: Underline, Shadow, Outline 등 FontStyle 관련 값의 UI scale 적용.

<br/>

## Font Size Scale

`SetFontSizeScale()`은 `SetFontSize()`로 설정한 font size 값을 변경하지 않고, layout/rendering 시 적용되는 scale을 설정합니다.
`SetSystemFontSizeScaleEnabled(true)`를 사용하면 시스템 font size 설정을 반영합니다.
minimum/maximum font size scale은 최종 scale 범위를 제한하며, minimum 값이 maximum 값보다 큰 경우 minimum 값이 우선됩니다.

사용자 지정 scale:

~~~cpp
Label label = Label::New("Scaled text")
  .SetFontSizeScale(1.5f)
  .SetMinimumFontSizeScale(0.8f)
  .SetMaximumFontSizeScale(2.0f);
~~~

시스템 font size scale 반영:

~~~cpp
Label label = Label::New("System scaled text")
  .SetSystemFontSizeScaleEnabled(true)
  .SetMinimumFontSizeScale(0.8f)
  .SetMaximumFontSizeScale(2.0f);
~~~

`GetAdjustedFontSizeScale()`로 최종 적용된 font size scale을 조회할 수 있습니다.

<br/>

## Markup

Mark-up tag를 사용하면 텍스트 일부의 color, font, underline, anchor 등을 변경할 수 있습니다. 기본적으로 text component는 mark-up string을 처리하지 않습니다. mark-up 처리를 사용하려면 `SetMarkupEnabled(true)`를 설정해야 합니다.

> [!WARNING]
> mark-up processor는 mark-up string의 정확성을 검증하지 않습니다. 잘못된 mark-up string은 텍스트가 의도와 다르게 렌더링되는 원인이 될 수 있습니다.

> [!NOTE]
> Mark-up attribute value는 quotation mark로 감싸야 정상 동작을 보장할 수 있습니다. 예: `value='0xFF0000'`

### 지원 tag

| Tag | 설명 |
|---|---|
| `<color>` | 텍스트 색상 변경 |
| `<font>` | font family, size 등 변경 |
| `<b>` | bold |
| `<i>` | italic |
| `<u>` | underline |
| `<s>` | line-through |
| `<background>` | 텍스트 배경 색상 |
| `<a>` | anchor (링크) |

### Color 값 형식

`<color>`, `<background>` 등 mark-up에서 사용하는 color 값은 named color 또는 RGB/ARGB hexadecimal 값을 사용할 수 있습니다.

사용 가능한 color 이름: `red`, `green`, `blue`, `yellow`, `magenta`, `cyan`, `white`, `black`, `transparent`

Hexadecimal 형식:
- `0xFF0000` — RGB (`0x` 접두사)
- `0xFFFF0000` — ARGB (`0x` 접두사)
- `#9C3A64` — RGB (`#` 접두사)
- `#FF9C3A64` — ARGB (`#` 접두사)

### 예시

color (RGB / ARGB):

~~~cpp
Label rgb = Label::New("<color value='0xFF0000'>Red Text</color>")
  .SetMarkupEnabled(true);

Label argb = Label::New("<color value='0xFFFF0000'>Red Text</color>")
  .SetMarkupEnabled(true);
~~~

font:

~~~cpp
Label label = Label::New("<font family='Sans' size='20'>Hello world</font>")
  .SetMarkupEnabled(true);
~~~

bold:

~~~cpp
Label label = Label::New("<b>Bold</b>")
  .SetMarkupEnabled(true);
~~~

> [!NOTE]
> `<b>`는 시스템에서 bold font를 찾아 적용합니다. bold font가 없으면 software bold 처리를 시도하며, 이 경우 실제 bold font를 사용하는 것보다 품질이 떨어질 수 있습니다.

italic:

~~~cpp
Label label = Label::New("<i>Italic</i>")
  .SetMarkupEnabled(true);
~~~

> [!NOTE]
> `<i>`는 시스템에서 italic font를 찾아 적용합니다. italic font가 없으면 software italic 처리를 시도하며, 이 경우 실제 italic font를 사용하는 것보다 품질이 떨어질 수 있습니다.

underline:

~~~cpp
Label label = Label::New("<u color='0xFF0000' height='2'>Underline</u>")
  .SetMarkupEnabled(true);
~~~

line-through:

~~~cpp
Label label = Label::New("<s color='#9C3A64' height='2'>Strike</s>")
  .SetMarkupEnabled(true);
~~~

background:

~~~cpp
Label label = Label::New("<background color='yellow'>Background</background>")
  .SetMarkupEnabled(true);
~~~

anchor:

~~~cpp
Label label = Label::New("<a href='https://www.tizen.org'>Tizen</a>")
  .SetMarkupEnabled(true);
~~~

anchor 클릭 처리:

~~~cpp
label.AnchorClickedSignal().Connect(this, &MyApp::OnAnchorClicked);

void MyApp::OnAnchorClicked(View view, const Dali::String& href)
{
  Label label = Label::DownCast(view);
  if(label)
  {
    // handle href
  }
}
~~~

참고 샘플: [text-markup-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-markup-example.cpp)

<br/>

## Marquee

Marquee는 긴 텍스트를 한정된 영역 안에서 스크롤 애니메이션으로 보여주는 기능입니다.

| API | 설명 |
|---|---|
| `SetMarqueeTriggerPolicy()` | 시작 정책 (`MANUAL`, `ON_OVERFLOW`) |
| `SetMarqueeSpeed()` | 속도 (pixels/second) |
| `SetMarqueeLoopCount()` | 반복 횟수 |
| `SetMarqueeLoopDelay()` | 루프 간 지연 (초) |
| `SetMarqueeGap()` | 끝과 시작 사이 간격 (pixel) |
| `SetMarqueeOrientation()` | 방향 (`HORIZONTAL`, `VERTICAL`) |
| `SetMarqueeStopMode()` | 정지 방식 (`IMMEDIATE`, `FINISH_LOOP`) |
| `StartMarquee()` | marquee 시작 |
| `StopMarquee()` | marquee 정지 |

### MANUAL

`MANUAL`에서는 overflow 여부와 관계없이 `StartMarquee()`를 호출해야 marquee가 시작됩니다. `StopMarquee()`로 정지합니다.

~~~cpp
Label label = Label::New("Very long text...")
  .SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::MANUAL)
  .SetMarqueeSpeed(80);

label.StartMarquee();
label.StopMarquee();
~~~

### ON_OVERFLOW

layout 중 텍스트가 available space를 초과하면 자동으로 시작됩니다. 조건이 맞으면 `StartMarquee()`를 호출할 필요가 없습니다.

~~~cpp
Label label = Label::New("Very long text...")
  .SetRequestedWidth(200.0f)
  .SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::ON_OVERFLOW)
  .SetMarqueeSpeed(80);
~~~

> [!NOTE]
> `HORIZONTAL`은 single-line 텍스트에만, `VERTICAL`은 multi-line 텍스트에만 적용됩니다. 조건이 맞지 않으면 설정이 무시됩니다.

> [!NOTE]
> Label의 inherited visibility가 변경되어 화면에서 사라지면 marquee는 자동으로 멈춥니다. 다시 visible 상태가 되면 이전 동작 여부에 따라 marquee가 다시 시작될 수 있습니다.

참고 샘플: [text-marquee-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-marquee-example.cpp)

<br/>

## Text Fit

Text fit은 주어진 width/height 안에서 텍스트가 overflow되지 않도록 가능한 가장 큰 font size를 선택하는 기능입니다. Label은 font size 범위나 후보 목록을 기준으로 available space에 맞는 값을 선택합니다.

| API | 설명 |
|---|---|
| `SetTextFit(Text::FitRange)` | font size 범위로 fit 설정 |
| `SetTextFit(Vector<FitCandidate>)` | 후보 목록으로 fit 설정 |
| `ClearTextFit()` | text fit 해제 |

### FitRange

min/max font size와 step으로 범위를 지정합니다. line height는 현재 style 설정을 따릅니다.

~~~cpp
Label label = Label::New("Auto-sized text")
  .SetRequestedWidth(MATCH_PARENT)
  .SetRequestedHeight(66.0f)
  .SetMultiLine(true)
  .SetTextFit(Text::FitRange(16.0f, 32.0f, 4.0f));
~~~

### FitCandidate

각 후보에 fontSize와 lineHeight를 직접 지정할 수 있습니다. text fit은 available space에 맞는 가장 큰 후보를 선택합니다.

~~~cpp
Dali::Vector<Text::FitCandidate> candidates;
candidates.PushBack(Text::FitCandidate(16.0f, 32.0f));
candidates.PushBack(Text::FitCandidate(20.0f, 40.0f));
candidates.PushBack(Text::FitCandidate(24.0f, 48.0f));

Label label = Label::New("Candidate fit")
  .SetRequestedWidth(MATCH_PARENT)
  .SetRequestedHeight(80.0f)
  .SetMultiLine(true)
  .SetTextFit(candidates);
~~~

> [!WARNING]
> Text fit은 fixed size 또는 명시적인 width/height 제약이 있는 layout에서 사용하는 것을 권장합니다.
> `WRAP_CONTENT`와 함께 사용하면 측정 시 최대 font size(또는 최대 candidate) 기준으로 크기가 결정됩니다.

참고 샘플:
- [text-fit-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-fit-example.cpp)
- [text-fit-candidate-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-fit-candidate-example.cpp)

<br/>

## Localization

`SetTranslatableText()`를 사용하여 다국어 문자열을 바인딩할 수 있습니다.

~~~cpp
Label title = Label::New()
  .SetTranslatableText("IDS_TITLE");
~~~

explicit domain 지정:

~~~cpp
Label title = Label::New()
  .SetTranslatableText("IDS_TITLE", "myapp-settings");
~~~

> [!WARNING]
> `SetText()`는 translatable text binding을 제거하지 않습니다.
> `SetText()`로 설정한 텍스트를 계속 유지하려면 `ClearTranslatableText()`를 먼저 호출해야 합니다.

자세한 내용: [Localization & Multilingual UI](https://github.sec.samsung.net/NUI/dali-ui/wiki/Localization-&-Multilingual-UI-(kr))

<br/>

## Advanced Rendering

### Async Rendering

async rendering은 Label의 text model update, layout, render(pixel buffer 생성 및 write) 등 text rendering stack의 주요 단계를 AsyncTask로 수행합니다. 큰 텍스트나 복잡한 레이아웃을 렌더링할 때 main thread 지연을 줄이는 데 도움이 될 수 있습니다.

async rendering이 활성화되면 OnMeasure/OnArrange로 결정된 size를 기반으로 OnRelayout 단계에서 async rendering이 자동 요청됩니다. 완료되면 `AsyncRenderFinishedSignal()`이 호출됩니다.

~~~cpp
Label label = Label::New("Async")
  .SetRequestedWidth(300.0f)
  .SetRequestedHeight(80.0f)
  .SetAsyncRendering(true);
~~~

> [!NOTE]
> Fixed size를 사용하면 measure 단계에서 main thread 계산을 줄이는 데 유리합니다.

> [!WARNING]
> 실시간으로 size/text/style이 계속 바뀌는 경우 이전 async 요청이 취소되고 새 요청이 발생할 수 있습니다. 이 경우 중간 상태의 rendering은 수행되지 않을 수 있으며, 마지막 변경에 대한 rendering 결과만 보장됩니다. rendering 결과와 layout 상태의 즉시 동기화가 중요한 경우에는 Async Rendering 사용이 부적합할 수 있습니다.

### Async Size Computation

Async Size Computation은 sync API인 `GetNaturalSize()`와 `GetHeightForWidth(width)`에 대응되는 계산을 비동기로 수행하는 기능입니다.

| API | 설명 |
|---|---|
| `RequestAsyncNaturalSize()` | `GetNaturalSize()`에 대응되는 size를 비동기로 계산 |
| `RequestAsyncHeightForWidth(float width)` | `GetHeightForWidth(width)`에 대응되는 height를 비동기로 계산 |
| `AsyncNaturalSizeComputedSignal()` | async natural size 계산 완료 signal |
| `AsyncHeightForWidthComputedSignal()` | async height-for-width 계산 완료 signal |

> [!NOTE]
> sync rendering mode일 때도 비동기 size 계산 API를 사용할 수 있습니다.

~~~cpp
label.AsyncNaturalSizeComputedSignal().Connect(
  [](View view, float width, float height)
  {
    // width, height: padding 포함된 natural size
  });

label.RequestAsyncNaturalSize();
~~~

~~~cpp
label.AsyncHeightForWidthComputedSignal().Connect(
  [](View view, float width, float height)
  {
    // width: 요청 시 전달한 width, height: 계산된 height (padding 포함)
  });

label.RequestAsyncHeightForWidth(300.0f);
~~~

### Render Scale

Render scale은 glyph를 더 큰 scale로 rasterize한 뒤 downscale하여, View가 시각적으로 확대될 때 texture upscaling으로 인한 품질 저하를 줄이는 기능입니다. layout size에는 영향을 주지 않습니다. async rendering이 활성화된 상태에서만 유효하며, 값은 1.0 이상이어야 합니다.

~~~cpp
Label label = Label::New("High quality")
  .SetAsyncRendering(true)
  .SetRenderScale(2.0f);
~~~

### Cutout & Mask Effect

글리프 모양으로 cutout하거나 mask effect를 적용할 수 있습니다.

~~~cpp
// Cutout
Label label = Label::New("Cutout")
  .SetCutoutEnabled(true);

// Mask effect
View maskView = ImageView::New("mask.png");

Label label2 = Label::New("Masked")
  .SetMaskEffect(maskView);
~~~

참고 샘플:
- [text-render-scale-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-render-scale-example.cpp)
- [text-cutout-mask-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-cutout-mask-example.cpp)

<br/>

## Signals

| Signal | 시그니처 | 설명 |
|---|---|---|
| `AnchorClickedSignal()` | `void(View, const String&)` | anchor 클릭 시 |
| `AsyncRenderFinishedSignal()` | `void(View, float, float)` | 비동기 렌더링 완료 시 |

> [!NOTE]
> `AsyncNaturalSizeComputedSignal()`과 `AsyncHeightForWidthComputedSignal()`은 [Async Size Computation](#async-size-computation) 섹션을 참고하세요.

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
| Localization | [text-localization-po-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-localization-po-example.cpp) |

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#documents)
