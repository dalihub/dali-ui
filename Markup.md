# Markup

[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Markup-(kr))

## Overview

Mark-up tags can be used to change the color, font, underline, anchor, etc. of part of the text. Convert a mark-up string with `Text::StyledText::FromMarkup()` and apply it with `SetStyledText()`.

Markup is shared by `Label`, `InputField`, and `InputEditor`. Use markup to write styles in a string. To add spans to text ranges in code or resolve annotations, see [StyledText](https://github.sec.samsung.net/NUI/dali-ui/wiki/StyledText).

If a mark-up string is passed directly to `Label::New()`, `Label::SetText()`, `InputField::SetText()`, or `InputEditor::SetText()`, it is treated as plain text.

~~~cpp
Text::StyledText styledText =
  Text::StyledText::FromMarkup("<color value='red'>Red</color> text");

Label label = Label::New();
label.SetStyledText(styledText);

InputField field = InputField::New();
field.SetStyledText(styledText);

InputEditor editor = InputEditor::New();
editor.SetStyledText(styledText);
~~~

> [!WARNING]
> The mark-up processor does not validate the correctness of mark-up strings. Incorrect mark-up strings may cause text to render differently than intended.

> [!NOTE]
> Mark-up attribute values must be wrapped in quotation marks to guarantee correct behavior. Example: `value='0xFFFF0000'`

<br/>

## Supported Tags

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
| `<img>` | Insert an inline image into text |
| `<annotation>` | Add semantic key/value metadata to a text range |

### Tag Attributes

| Tag | Attribute | Description |
|---|---|---|
| `<color>` | `value` | Text color |
| `<font>` | `family`, `size`, `weight`, `width`, `slant` | Font family, size in pixels, weight, width, and slant |
| `<u>` | `color`, `height`, `type`, `dash-gap`, `dash-width` | Underline color, thickness, type, and dash gap and length |
| `<s>` | `color`, `height` | Line-through color and thickness |
| `<background>` | `color` | Text background color |
| `<a>` | `href`, `color`, `clicked-color` | Link target, link color, and clicked color |
| `<img>` | `src`, `width`, `height` | Image source and width and height in pixels. All three are required |
| `<annotation>` | Application-defined key/value attributes | Convert each attribute into a separate `AnnotationSpan` |

The `type` attribute of `<u>` supports `solid`, `dashed`, and `double`. `dash-gap` and `dash-width` apply to dashed underlines.

The `weight`, `width`, and `slant` attributes of `<font>` use strings corresponding to font enum values. Examples: `weight='bold'`, `width='condensed'`, `slant='italic'`.

> [!NOTE]
> DALi markup supports the tags listed above and does not support the full HTML syntax.

To add font variation or gradient spans, use `StyledTextBuilder` and the corresponding span APIs. See [StyledText And Markup](https://github.sec.samsung.net/NUI/dali-ui/wiki/StyledText#styledtext-and-markup) for adding spans to text created from markup.

<br/>

## Color Value Format

Color values used in mark-up such as `<color>`, `<background>` can be named colors or RGB/ARGB hexadecimal values.

Available named colors: `red`, `green`, `blue`, `yellow`, `magenta`, `cyan`, `white`, `black`, `transparent`

Hexadecimal formats:

- `0xFFFF0000` — ARGB (`0x` prefix, `0xAARRGGBB`)
- `#9C3A64` — RGB (`#` prefix, `#RRGGBB`)
- `#FF9C3A64` — ARGB (`#` prefix, `#AARRGGBB`)
- `#F00` — Short RGB form (`#RGB`). The same opaque red as `#FF0000`
- `0x80FF0000`, `#80FF0000` — Semi-transparent red

In ARGB, `AA` represents opacity. `00` is fully transparent, and `FF` is opaque.

> [!NOTE]
> Write RGB colors as `#RRGGBB`. For the `0x` format, use eight digits including opacity (`AA`): `0xAARRGGBB`. For example, opaque red is `#FF0000` or `0xFFFF0000`.

<br/>

## Examples

The following examples apply markup to `Label`. You can apply the same markup to `InputField` and `InputEditor` with `SetStyledText()`. For anchor clicks, see the example marked as Label-specific below.

color (RGB / ARGB):

~~~cpp
Label rgb = Label::New();
rgb.SetStyledText(Text::StyledText::FromMarkup("<color value='#FF0000'>Red Text</color>"));

Label argb = Label::New();
argb.SetStyledText(Text::StyledText::FromMarkup("<color value='0xFFFF0000'>Red Text</color>"));
~~~

font:

~~~cpp
Label label = Label::New();
label.SetStyledText(Text::StyledText::FromMarkup("<font family='Sans' size='20'>Hello world</font>"));
~~~

bold:

~~~cpp
Label label = Label::New();
label.SetStyledText(Text::StyledText::FromMarkup("<b>Bold</b>"));
~~~

> [!NOTE]
> `<b>` looks for a bold font in the system. If no bold font is available, it falls back to software bold, which may have lower quality than using a real bold font.

italic:

~~~cpp
Label label = Label::New();
label.SetStyledText(Text::StyledText::FromMarkup("<i>Italic</i>"));
~~~

> [!NOTE]
> `<i>` looks for an italic font in the system. If no italic font is available, it falls back to software italic, which may have lower quality than using a real italic font.

underline:

~~~cpp
Label label = Label::New();
label.SetStyledText(Text::StyledText::FromMarkup("<u color='0xFFFF0000' height='2'>Underline</u>"));
~~~

line-through:

~~~cpp
Label label = Label::New();
label.SetStyledText(Text::StyledText::FromMarkup("<s color='#9C3A64' height='2'>Strike</s>"));
~~~

background:

~~~cpp
Label label = Label::New();
label.SetStyledText(Text::StyledText::FromMarkup("<background color='yellow'>Background</background>"));
~~~

anchor:

~~~cpp
Label label = Label::New();
label.SetStyledText(Text::StyledText::FromMarkup("<a href='https://www.tizen.org'>Tizen</a>"));
~~~

anchor click handling (Label):

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

> [!NOTE]
> `InputField` and `InputEditor` can also apply anchor markup, but their public APIs currently do not provide `AnchorClickedSignal()`. Use the click handling example above with `Label`.

### Additional Attributes

font weight, width, slant:

~~~cpp
Label label = Label::New();
label.SetStyledText(Text::StyledText::FromMarkup(
  "<font family='Sans' size='20' weight='bold' width='condensed' slant='italic'>Hello world</font>"));
~~~

dashed underline:

~~~cpp
Label label = Label::New();
label.SetStyledText(Text::StyledText::FromMarkup(
  "<u color='0xFFFF0000' height='2' type='dashed' dash-gap='3' dash-width='5'>Underline</u>"));
~~~

anchor color:

~~~cpp
Label label = Label::New();
label.SetStyledText(Text::StyledText::FromMarkup(
  "<a href='https://www.tizen.org' color='blue' clicked-color='red'>Tizen</a>"));
~~~

### Inline Image

Use `<img>` to insert an image into text. Set `src` to an image path that the application can load.

~~~cpp
Label label = Label::New();
label.SetStyledText(Text::StyledText::FromMarkup(
  "DALi <img src='res/flag_kr.png' width='32' height='24'/> Icon"));
~~~

Write the size as numbers, such as `width='32'` and `height='24'`, in pixels. The example above displays an image 32 pixels wide and 24 pixels high.

For additional settings such as image alignment, see [ImageSpan in StyledText](https://github.sec.samsung.net/NUI/dali-ui/wiki/StyledText#imagespan).

### Annotation

`<annotation>` attaches semantic key/value metadata to a text range. Each attribute in one tag becomes a separate `AnnotationSpan` over the same range.

~~~cpp
Text::StyledText styledText = Text::StyledText::FromMarkup(
  "<annotation style='accent' role='action'>Apply</annotation>");
~~~

The example above creates annotation spans for `style=accent` and `role=action` over `Apply`. An annotation does not change the color or font by itself. See [Annotation in StyledText](https://github.sec.samsung.net/NUI/dali-ui/wiki/StyledText#annotation) for reading metadata and adding visual spans in application code.

<br/>

## Nested Tags And Entities

Nest tags to combine styles. The example below displays the text in bold red.

~~~cpp
Label label = Label::New();
label.SetStyledText(Text::StyledText::FromMarkup(
  "<color value='red'><b>Red Bold</b></color>"));
~~~

Use entities to display symbols used in markup syntax, such as `<`, `>`, and `&`, as text. For example, `&lt;` is displayed as `<`.

| Written Value | Result |
|---|---|
| `&lt;` | `<` |
| `&gt;` | `>` |
| `&amp;` | `&` |
| `&quot;` | `"` |
| `&apos;` | `'` |
| `&#65;`, `&#x41;` | `A` (decimal / hexadecimal character code) |

~~~cpp
Label label = Label::New();
label.SetStyledText(Text::StyledText::FromMarkup("&lt;tag&gt; &amp; &#65; &#x41;"));
~~~

The example above displays `<tag> & A A`. You can use the same notation in attribute values. For example, the URL in `href='https://example.com?a=1&amp;b=2'` is processed as `https://example.com?a=1&b=2`.

<br/>

## Using Markup In Input Controls

`InputField` and `InputEditor` use styles applied with `SetStyledText()` as the initial editable styles. As the user edits the text, the ranges of surviving spans are updated.

`GetText()` returns plain text without markup tags. It does not return the original markup string or `StyledText`, so keep the source in the application if needed. Calling `SetText()` clears the existing styled text state.

For input, selection, and typing style APIs, see [Text Input](https://github.sec.samsung.net/NUI/dali-ui/wiki/Text-Input).

<br/>

## Samples

| Feature | Sample |
|---|---|
| Label / InputField / InputEditor markup | [text-markup-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-markup-example.cpp) |
| Markup conversion and inline images | [text-styled-text-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-styled-text-example.cpp) |
| ImageSpan in input controls | [text-input-image-span-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-input-image-span-example.cpp) |

<br/>

---

[← Text Overview](https://github.sec.samsung.net/NUI/dali-ui/wiki/Text)
