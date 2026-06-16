# Label

[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Label-(kr))

## Overview

`Label` is a non-editable View that displays text. It performs text layout and rendering, but does not support user input or editing. It provides various features from simple text display to multi-line, style, marquee, and async rendering.

<br/>

## Basic Usage

Create with text:

~~~cpp
Label label = Label::New("Hello");

window.Add(label);
~~~

Create and configure:

~~~cpp
Label label = Label::New();
label.SetText("Hello");
label.SetFontSize(24.0f);
label.SetTextColor(UiColor::PRIMARY);
~~~

<br/>

## Text Layout

Representative text layout APIs.

| API | Description |
|---|---|
| `SetMultiLine()` | Enable multi-line |
| `SetLineWrapMode()` | Line wrap mode (`WORD`, `CHARACTER`, `HYPHENATION`, `MIXED`) |
| `SetHorizontalTextAlignment()` | Horizontal alignment (`START`, `CENTER`, `END`) |
| `SetVerticalTextAlignment()` | Vertical alignment (`START`, `CENTER`, `END`) |
| `SetOverflowMode()` | Overflow handling (`ELLIPSIS`, `CLIP`) |

~~~cpp
Label label = Label::New("Long text...");
label.SetMultiLine(true);
label.SetLineWrapMode(Text::LineWrapMode::WORD);
label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
~~~

<br/>

## Line Height

Control line spacing with `SetLineHeight()` and `SetLineHeightMode()`.

- **RELATIVE** (default): Multiplier relative to font size
  - `CalculatedLineHeight(px) = fontSize(px) * lineHeight * effectiveScale`
- **ABSOLUTE**: Absolute value in pixels
  - `CalculatedLineHeight(px) = lineHeight(px) * effectiveScale`

Effective scale may include UI scale and font size scale.

~~~cpp
Label label = Label::New("Line height");
label.SetMultiLine(true);
label.SetLineHeight(1.4f);
label.SetLineHeightMode(Text::LineHeightMode::RELATIVE);
~~~

~~~cpp
Label label = Label::New("Auto line height");
label.SetLineHeight(Text::LINE_HEIGHT_AUTO);
~~~

> [!NOTE]
> `LINE_HEIGHT_AUTO` uses the default line height calculated from font metrics. If the calculated line height is smaller than the font-metrics-based height, the font-metrics-based height is used to avoid clipping glyphs.

<br/>

## Text Style

Label can apply text styles such as underline, shadow, outline, line-through, and bevel through style objects.

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
Label label = Label::New("Underline");
label.SetUnderline(Text::Underline());

// Dashed underline with color
Text::Underline underline;
underline.SetColor(UiColor(0x0088FF));
underline.SetThickness(2.0f);
underline.SetType(Text::Underline::Type::DASHED);
underline.SetDashLength(4.0f);
underline.SetDashGap(4.0f);

Label label2 = Label::New("Dashed");
label2.SetUnderline(underline);

// Shadow
Text::Shadow shadow;
shadow.SetColor(UiColor(0xFF5500));
shadow.SetOffset(Vector2(3.0f, 3.0f));
shadow.SetBlurRadius(2.0f);

Label label3 = Label::New("Shadow");
label3.SetShadow(shadow);

// Outline
Text::Outline outline;
outline.SetColor(UiColor(0x0066FF));
outline.SetWidth(2.0f);

Label label4 = Label::New("Outline");
label4.SetOutline(outline);

// LineThrough
Text::LineThrough lineThrough;
lineThrough.SetColor(UiColor(0xFF00FF));
lineThrough.SetThickness(3.0f);

Label label5 = Label::New("Strikethrough");
label5.SetLineThrough(lineThrough);

// Text background color
Label label6 = Label::New("Highlighted");
label6.SetTextBackgroundColor(UiColor(0xFFFF00));
~~~

See also:
- [text-style-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-style-example.cpp)
- [text-style-bevel-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-style-bevel-example.cpp)

<br/>

## Font

| API | Description |
|---|---|
| `SetFontFamily()` | Set font family |
| `SetFontSize()` | Set font size (pixels) |
| `SetFontWeight()` | Set font weight (`THIN` ~ `BLACK`) |
| `SetFontWidth()` | Set font width (`ULTRA_CONDENSED` ~ `ULTRA_EXPANDED`) |
| `SetFontSlant()` | Set font slant (`NORMAL`, `ITALIC`, `OBLIQUE`) |
| `SetFontVariation()` | Set variable font axis |

If no font family is specified, the platform default font is used. Font family, weight, width, slant, and variation are applied within the range supported by the selected font.

### Font Variation

Set using string:

~~~cpp
Label label = Label::New("Variable Font");
label.SetFontFamily("Sans VF");
label.SetFontVariation("wght=700,wdth=90");
~~~

Set using `FontVariationAxis`:

~~~cpp
Dali::Vector<Text::FontVariationAxis> axes;
axes.PushBack(Text::FontVariationAxis("wght", 700.0f));
axes.PushBack(Text::FontVariationAxis("wdth", 90.0f));

Label label = Label::New("Variable Font");
label.SetFontFamily("Sans VF");
label.SetFontVariation(axes);
~~~

> [!NOTE]
> If the font does not support a variation axis, the axis value may be ignored. `"Sans VF"` in the example above is a placeholder variable font name.

See also: [text-font-variation-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-font-variation-example.cpp)

<br/>

## UI Scale

UI scale is reflected in font size and text layout related values during the measure/relayout process of Label, InputField, and InputEditor. Unlike Font Size Scale, UI scale is a value connected to the View/layout hierarchy's scale policy.

| Target | Description |
|---|---|
| FontSize | Reflected in text layout / rendering base font size |
| LineHeight | Reflected in relative/absolute line height calculation |
| TextFit FitRange | Reflected in min/max/step values |
| TextFit FitCandidate | Reflected in font size / line height |
| Margin / Padding | Reflected in text layout area calculation |
| Marquee | Reflected in MarqueeGap and other marquee layout values |

> [!NOTE]
> TBD: UI scale application for FontStyle-related values such as Underline, Shadow, and Outline.

<br/>

## Font Size Scale

`SetFontSizeScale()` does not change the original `SetFontSize()` property value. It sets a scale that is applied during layout/rendering.
`SetSystemFontSizeScaleEnabled(true)` enables reflecting the system font size setting.
Minimum/maximum font size scale limits the final scale range. If the minimum value is greater than the maximum, the minimum takes priority.

User-defined scale:

~~~cpp
Label label = Label::New("Scaled text");
label.SetFontSizeScale(1.5f);
label.SetMinimumFontSizeScale(0.8f);
label.SetMaximumFontSizeScale(2.0f);
~~~

System font size scale:

~~~cpp
Label label = Label::New("System scaled text");
label.SetSystemFontSizeScaleEnabled(true);
label.SetMinimumFontSizeScale(0.8f);
label.SetMaximumFontSizeScale(2.0f);
~~~

`GetAdjustedFontSizeScale()` returns the final applied font size scale.

<br/>

## Markup

Mark-up tags can be used to change the color, font, underline, anchor, etc. of part of the text. By default, text components do not process mark-up strings. To enable mark-up processing, set `SetMarkupEnabled(true)`.

> [!WARNING]
> The mark-up processor does not validate the correctness of mark-up strings. Incorrect mark-up strings may cause text to render differently than intended.

> [!NOTE]
> Mark-up attribute values must be wrapped in quotation marks to guarantee correct behavior. Example: `value='0xFF0000'`

### Supported Tags

| Tag | Description |
|---|---|
| `<color>` | Change text color |
| `<font>` | Change font family, size, etc. |
| `<b>` | Bold |
| `<i>` | Italic |
| `<u>` | Underline |
| `<s>` | Line-through |
| `<background>` | Text background color |
| `<a>` | Anchor (link) |

### Color Value Format

Color values used in mark-up such as `<color>`, `<background>` can be named colors or RGB/ARGB hexadecimal values.

Available named colors: `red`, `green`, `blue`, `yellow`, `magenta`, `cyan`, `white`, `black`, `transparent`

Hexadecimal formats:
- `0xFF0000` — RGB (`0x` prefix)
- `0xFFFF0000` — ARGB (`0x` prefix)
- `#9C3A64` — RGB (`#` prefix)
- `#FF9C3A64` — ARGB (`#` prefix)

### Examples

color (RGB / ARGB):

~~~cpp
Label rgb = Label::New("<color value='0xFF0000'>Red Text</color>");
rgb.SetMarkupEnabled(true);

Label argb = Label::New("<color value='0xFFFF0000'>Red Text</color>");
argb.SetMarkupEnabled(true);
~~~

font:

~~~cpp
Label label = Label::New("<font family='Sans' size='20'>Hello world</font>");
label.SetMarkupEnabled(true);
~~~

bold:

~~~cpp
Label label = Label::New("<b>Bold</b>");
label.SetMarkupEnabled(true);
~~~

> [!NOTE]
> `<b>` looks for a bold font in the system. If no bold font is available, it falls back to software bold, which may have lower quality than using a real bold font.

italic:

~~~cpp
Label label = Label::New("<i>Italic</i>");
label.SetMarkupEnabled(true);
~~~

> [!NOTE]
> `<i>` looks for an italic font in the system. If no italic font is available, it falls back to software italic, which may have lower quality than using a real italic font.

underline:

~~~cpp
Label label = Label::New("<u color='0xFF0000' height='2'>Underline</u>");
label.SetMarkupEnabled(true);
~~~

line-through:

~~~cpp
Label label = Label::New("<s color='#9C3A64' height='2'>Strike</s>");
label.SetMarkupEnabled(true);
~~~

background:

~~~cpp
Label label = Label::New("<background color='yellow'>Background</background>");
label.SetMarkupEnabled(true);
~~~

anchor:

~~~cpp
Label label = Label::New("<a href='https://www.tizen.org'>Tizen</a>");
label.SetMarkupEnabled(true);
~~~

anchor click handling:

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

See also: [text-markup-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-markup-example.cpp)

<br/>

## Marquee

Marquee is a feature that displays long text with scroll animation within a limited area.

| API | Description |
|---|---|
| `SetMarqueeTriggerPolicy()` | Trigger policy (`MANUAL`, `ON_OVERFLOW`) |
| `SetMarqueeSpeed()` | Speed (pixels/second) |
| `SetMarqueeLoopCount()` | Loop count |
| `SetMarqueeLoopDelay()` | Delay between loops (seconds) |
| `SetMarqueeGap()` | Gap between end and start (pixels) |
| `SetMarqueeOrientation()` | Direction (`HORIZONTAL`, `VERTICAL`) |
| `SetMarqueeStopMode()` | Stop mode (`IMMEDIATE`, `FINISH_LOOP`) |
| `StartMarquee()` | Start marquee |
| `StopMarquee()` | Stop marquee |

### MANUAL

With `MANUAL`, `StartMarquee()` must be called to start the marquee regardless of overflow. `StopMarquee()` stops it.

~~~cpp
Label label = Label::New("Very long text...");
label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::MANUAL);
label.SetMarqueeSpeed(80);

label.StartMarquee();
label.StopMarquee();
~~~

### ON_OVERFLOW

With `ON_OVERFLOW`, marquee starts automatically during layout when the text exceeds the available space. If the condition is met, there is no need to call `StartMarquee()`.

~~~cpp
Label label = Label::New("Very long text...");
label.SetRequestedWidth(200.0f);
label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::ON_OVERFLOW);
label.SetMarqueeSpeed(80);
~~~

> [!NOTE]
> `HORIZONTAL` applies only to single-line text, and `VERTICAL` applies only to multi-line text. If the condition is not met, the setting is ignored.

> [!NOTE]
> When the Label's inherited visibility changes and it disappears from the screen, marquee stops automatically. When it becomes visible again, marquee may restart depending on the previous running state.

See also: [text-marquee-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-marquee-example.cpp)

<br/>

## Text Fit

Text fit selects the largest font size that does not overflow within the given width/height. Label selects a value that fits the available space based on a font size range or candidate list.

| API | Description |
|---|---|
| `SetTextFit(Text::FitRange)` | Set fit with font size range |
| `SetTextFit(Vector<FitCandidate>)` | Set fit with candidate list |
| `ClearTextFit()` | Clear text fit |

### FitRange

Specify a range with min/max font size and step. Line height follows the current style setting.

~~~cpp
Label label = Label::New("Auto-sized text");
label.SetRequestedWidth(MATCH_PARENT);
label.SetRequestedHeight(66.0f);
label.SetMultiLine(true);
label.SetTextFit(Text::FitRange(16.0f, 32.0f, 4.0f));
~~~

### FitCandidate

Each candidate can specify fontSize and lineHeight directly. Text fit selects the largest candidate that fits the available space.

~~~cpp
Dali::Vector<Text::FitCandidate> candidates;
candidates.PushBack(Text::FitCandidate(16.0f, 32.0f));
candidates.PushBack(Text::FitCandidate(20.0f, 40.0f));
candidates.PushBack(Text::FitCandidate(24.0f, 48.0f));

Label label = Label::New("Candidate fit");
label.SetRequestedWidth(MATCH_PARENT);
label.SetRequestedHeight(80.0f);
label.SetMultiLine(true);
label.SetTextFit(candidates);
~~~

> [!WARNING]
> Text fit is recommended for layouts with a fixed size or explicit width/height constraints.
> When used with `WRAP_CONTENT`, measurement is based on the maximum font size or maximum candidate.

See also:
- [text-fit-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-fit-example.cpp)
- [text-fit-candidate-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-fit-candidate-example.cpp)

<br/>

## Localization

Use `SetTranslatableText()` to bind a localized string.

~~~cpp
Label title = Label::New();
title.SetTranslatableText("IDS_TITLE");
~~~

With explicit domain:

~~~cpp
Label title = Label::New();
title.SetTranslatableText("IDS_TITLE", "myapp-settings");
~~~

> [!WARNING]
> `SetText()` does not remove the translatable text binding.
> To keep text set by `SetText()`, call `ClearTranslatableText()` first.

See also: [Localization & Multilingual UI](https://github.sec.samsung.net/NUI/dali-ui/wiki/Localization-&-Multilingual-UI)

<br/>

## Advanced Rendering

### Async Rendering

Async rendering runs the major stages of the text rendering stack — text model update, layout, and render (pixel buffer creation and write) — through AsyncTask. It can help reduce main-thread delay when rendering large text or complex layouts.

When async rendering is enabled, an async render request is automatically made during the OnRelayout phase based on the size determined by OnMeasure/OnArrange. When complete, `AsyncRenderFinishedSignal()` is emitted.

~~~cpp
Label label = Label::New("Async");
label.SetRequestedWidth(300.0f);
label.SetRequestedHeight(80.0f);
label.SetAsyncRendering(true);
~~~

> [!NOTE]
> Using a fixed size is beneficial for reducing main-thread computation during the measure phase.

> [!WARNING]
> If size/text/style changes continuously in real time, previous async requests may be canceled and replaced by new ones. Intermediate states may not be rendered, and only the final change is guaranteed to produce a rendering result. If immediate synchronization between rendering result and layout state is important, Async Rendering may not be suitable.

### Async Size Computation

Async Size Computation performs the same calculation as the sync APIs `GetNaturalSize()` and `GetHeightForWidth(width)` asynchronously.

| API | Description |
|---|---|
| `RequestAsyncNaturalSize()` | Async computation corresponding to `GetNaturalSize()` |
| `RequestAsyncHeightForWidth(float width)` | Async computation corresponding to `GetHeightForWidth(width)` |
| `AsyncNaturalSizeComputedSignal()` | Signal emitted when async natural size computation completes |
| `AsyncHeightForWidthComputedSignal()` | Signal emitted when async height-for-width computation completes |

> [!NOTE]
> Async size computation APIs can be used even when synchronous rendering mode is used.

~~~cpp
label.AsyncNaturalSizeComputedSignal().Connect(
  [](View view, float width, float height)
  {
    // width, height: natural size including padding
  });

label.RequestAsyncNaturalSize();
~~~

~~~cpp
label.AsyncHeightForWidthComputedSignal().Connect(
  [](View view, float width, float height)
  {
    // width: requested width, height: computed height including padding
  });

label.RequestAsyncHeightForWidth(300.0f);
~~~

### Render Scale

Render scale rasterizes glyphs at a larger scale and downscales them to reduce texture upscaling quality loss when the View is visually scaled up. It does not affect layout size. It is only valid when async rendering is enabled, and the value must be 1.0 or greater.

~~~cpp
Label label = Label::New("High quality");
label.SetAsyncRendering(true);
label.SetRenderScale(2.0f);
~~~

### Cutout & Mask Effect

Apply cutout using glyph shapes or mask effects.

~~~cpp
// Cutout
Label label = Label::New("Cutout");
label.SetCutoutEnabled(true);

// Mask effect
View maskView = ImageView::New("mask.png");

Label label2 = Label::New("Masked");
label2.SetMaskEffect(maskView);
~~~

See also:
- [text-render-scale-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-render-scale-example.cpp)
- [text-cutout-mask-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-cutout-mask-example.cpp)

<br/>

## Signals

| Signal | Signature | Description |
|---|---|---|
| `AnchorClickedSignal()` | `void(View, const String&)` | When an anchor is clicked |
| `AsyncRenderFinishedSignal()` | `void(View, float, float)` | When async rendering completes |

> [!NOTE]
> For `AsyncNaturalSizeComputedSignal()` and `AsyncHeightForWidthComputedSignal()`, see the [Async Size Computation](#async-size-computation) section.

<br/>

## Samples

| Feature | Sample |
|---|---|
| Basic | [text-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-example.cpp) |
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
