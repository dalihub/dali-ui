# Text Overview

[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Text-(kr))

## Overview

dali-ui text features are provided as `View`-based components. Text rendering and layout are handled by the internal text backend, and applications configure text content and style through public APIs.

| Component | Purpose | Main Features |
|---|---|---|
| [Label](https://github.sec.samsung.net/NUI/dali-ui/wiki/Label) | Read-only text display | multi-line, text style, marquee, text fit, async rendering |
| [InputField](https://github.sec.samsung.net/NUI/dali-ui/wiki/Text-Input) | Single-line text input | placeholder, cursor, selection, password, input filter |
| [InputEditor](https://github.sec.samsung.net/NUI/dali-ui/wiki/Text-Input) | Multi-line text input | multi-line editing, auto grow, line count |

Use `Label` for read-only text, `InputField` for single-line input, and `InputEditor` for multi-line input.

<br/>

## Text Feature Categories

The three components do not all provide the same APIs, but dali-ui text features can be roughly categorized as follows.

| Category | Related Features | Main Components |
|---|---|---|
| Text layout | multi-line, wrap, alignment, overflow, line height | Label, InputEditor |
| Text style | color, underline, shadow, outline, background | Label, InputField, InputEditor |
| Font | font family, size, weight, width, slant, variation | Label, InputField, InputEditor |
| Text input | placeholder, cursor, selection, input filter | InputField, InputEditor |
| Advanced label rendering | text fit, marquee, async rendering, render scale, cutout | Label |
| Localization | translatable text/placeholder, binding | Label, InputField, InputEditor |

<br/>

## Text Size Measurement

Text component size is determined through the normal View layout system's measure/arrange process. Text size may change depending on font, style, and line breaking. `GetNaturalSize()` queries the size when text content is laid out without width/height constraints, and `GetHeightForWidth(width)` queries the height when text content is laid out at the given width.

| API | Description |
|---|---|
| `GetNaturalSize()` | Size when text content is laid out without width/height constraints |
| `GetHeightForWidth(float width)` | Height when text content is laid out at the given width |
| `GetLineCount()` | Line count based on current layout width |
| `GetLineCount(float width)` | Line count based on the specified width |

~~~cpp
Vector3 naturalSize = label.GetNaturalSize();

float height = label.GetHeightForWidth(300.0f);

int lineCount = label.GetLineCount(300.0f);
~~~

> [!NOTE]
> `GetNaturalSize()` and `GetHeightForWidth()` work with values that include padding. The width passed to `GetHeightForWidth(width)` is the total width including padding, and the returned height also includes padding.

> [!WARNING]
> Text size measurement can trigger text model update and layout. Cached values may be reused when text properties have not changed, but repeatedly calling these APIs every frame should be avoided.

> [!NOTE]
> Values that depend on the current layout width, such as `GetLineCount()`, may not be accurate before layout is resolved. Use the width-explicit variant like `GetLineCount(width)` when needed.

<br/>

## Text Layout Direction

`Text::LayoutDirectionMode` controls how the base direction of text content is resolved inside a text component. When a string contains RTL characters, the visual order may be determined by the Unicode Bidirectional Algorithm, and this mode is used to resolve the base direction for text layout.

The default mode can be set through `UiConfig`:

~~~cpp
UiConfig::New()
  .SetTextLayoutDirectionMode(Text::LayoutDirectionMode::INHERIT)
  .Apply();
~~~

Each text view can also be configured directly:

~~~cpp
Label label = Label::New()
  .SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS);
~~~

| Mode | Description |
|---|---|
| `INHERIT` | Follows the parent View's layout direction |
| `CONTENTS` | Determines direction from text content |
| `LOCALE` | Based on system locale |

> [!NOTE]
> For typical multilingual UIs, using `INHERIT` to follow the parent View's layout direction is the natural choice.

See also: [Localization & Multilingual UI](https://github.sec.samsung.net/NUI/dali-ui/wiki/Localization-&-Multilingual-UI)

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
| Input | [text-input-field-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-input-field-example.cpp), [text-input-editor-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-input-editor-example.cpp) |
| Localization | [text-localization-po-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-localization-po-example.cpp) |

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#documents)
