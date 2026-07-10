# StyledText

[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/StyledText-(kr))

## Overview

`StyledText` is a styled text value created from UTF-8 input. DALi decodes the text and attaches spans to ranges in the decoded UTF-32 sequence. It is useful when an application needs to style selected text ranges programmatically, or when rich text should be derived from semantic markup before being applied to a text component.

`StyledTextBuilder` is used to assemble a `StyledText` snapshot. For sequential text construction, `PushSpan()` and `PopSpan()` apply style to appended text without requiring the application to calculate range indexes manually. Use `SetSpan()` when the target range is already known.

`Label`, `InputField`, and `InputEditor` can apply a `StyledText` value with `SetStyledText()`.

~~~cpp
Label label = Label::New();

Text::StyledTextBuilder builder = Text::StyledTextBuilder::New();
builder.AppendText("Hello ");
builder.PushSpan(Text::ForegroundColorSpan::New(UiColor(0x2563EB)));
builder.AppendText("StyledText");
builder.PopSpan();

label.SetStyledText(builder.Build());
~~~

![StyledText result](./assets/text/hello_styled_text.png)

> [!NOTE]
> `StyledText` range indexes are UTF-32 indexes in the decoded text, not UTF-8 byte offsets.

<br/>

## StyledText And Markup

For simple static markup, convert directly to `StyledText`:

~~~cpp
Label label = Label::New();
label.SetStyledText(Text::StyledText::FromMarkup("<color value='red'>Red</color> text"));
~~~

Use `StyledTextBuilder::FromMarkup()` when markup should be parsed first and then extended with application-defined spans:

~~~cpp
Text::StyledTextBuilder builder =
  Text::StyledTextBuilder::FromMarkup("<u>Hello</u> StyledText");

builder.SetSpan(Text::ForegroundColorSpan::New(UiColor(0xDC2626)), 0u, 5u);

label.SetStyledText(builder.Build());
~~~

> [!NOTE]
> Passing markup to `Label::New()`, `SetText()`, `InputField::SetText()`, or `InputEditor::SetText()` treats it as plain text. Use `SetStyledText()` to apply parsed markup or spans.

<br/>

## Span Types

A `Span` stores the style payload. The text range is attached by `StyledTextBuilder` and stored in `StyledText`.

| Span | Purpose |
|---|---|
| `ForegroundColorSpan` | Text color |
| `BackgroundColorSpan` | Text background color |
| `FontSpan` | Font family, size, weight, width, slant, variation |
| `UnderlineSpan` | Underline style |
| `LineThroughSpan` | Line-through style |
| `AnchorSpan` | Anchor metadata for link handling |
| `AnnotationSpan` | Semantic key/value metadata |

Multiple spans can target the same range when they describe different style categories:

~~~cpp
Text::StyledTextBuilder builder = Text::StyledTextBuilder::New("Decorated text");

builder.SetSpan(Text::ForegroundColorSpan::New(UiColor(0x2563EB)), 0u, 9u);
builder.SetSpan(Text::BackgroundColorSpan::New(UiColor(0xDBEAFE)), 0u, 9u);

Text::Underline underline;
underline.SetColor(UiColor(0x2563EB));
underline.SetThickness(2.0f);
builder.SetSpan(Text::UnderlineSpan::New(underline), 0u, 9u);

label.SetStyledText(builder.Build());
~~~

![Span types result](./assets/text/span_types.png)

> [!NOTE]
> A builder stores at most one attachment for the same `Span` handle. If the same span handle is attached again, its range is updated. Create separate span handles when the same visual style should be attached to multiple ranges.

<br/>

## Push And Pop

`PushSpan()` and `PopSpan()` are the easiest way to apply style while appending text. `PushSpan()` opens a span at the current end of the decoded text, and `PopSpan()` closes it at the current end. The text appended between the two calls becomes the span range automatically.

~~~cpp
Text::StyledTextBuilder builder = Text::StyledTextBuilder::New();

builder.AppendText("Normal ");
builder.PushSpan(Text::ForegroundColorSpan::New(UiColor(0xDC2626)));
builder.AppendText("Red");
builder.PopSpan();
builder.AppendText(" Normal");

label.SetStyledText(builder.Build());
~~~

![Push and Pop result](./assets/text/push_and_pop.png)

`PopSpan()` closes the most recent open span. `PopSpan(token)` can be used when a specific pushed span should be closed in nested construction:

~~~cpp
Text::StyledTextBuilder builder = Text::StyledTextBuilder::New();

const uint32_t warningToken =
  builder.PushSpan(Text::ForegroundColorSpan::New(UiColor(0xDC2626)));

builder.AppendText("Delete ");

Text::Underline underline;
underline.SetColor(UiColor(0xDC2626));
builder.PushSpan(Text::UnderlineSpan::New(underline));

builder.AppendText("now?");
builder.PopSpan(warningToken);
builder.AppendText(" Cancel");

label.SetStyledText(builder.Build());
~~~

![Push and Pop token result](./assets/text/push_and_pop_token.png)

`PopSpan(token)` closes the matching open span. If spans were opened after that token and are still open, they are closed at the same position as well. Use `SetSpan()` instead when a range has already been calculated by search, parser, or application logic.

<br/>

## UTF-32 Ranges

`SetSpan()` and span inspection APIs use UTF-32 indexes. You usually do not need to calculate these indexes when building text with `PushSpan()` and `PopSpan()`, but they matter when the source text contains non-ASCII characters, emoji, or when ranges come from UTF-8 string APIs.

When a range is known as UTF-8 byte offsets, convert it before calling `SetSpan()`:

~~~cpp
std::string text = "Hello 가😀 StyledText";
std::string target = "가😀";

std::size_t utf8Start = text.find(target);
std::size_t utf8End   = utf8Start + target.size();

uint32_t utf32Start = 0u;
uint32_t utf32End   = 0u;

if(Text::Utf8ToUtf32Range(Dali::StringView(text.data(), static_cast<uint32_t>(text.size())),
                          static_cast<uint32_t>(utf8Start),
                          static_cast<uint32_t>(utf8End),
                          utf32Start,
                          utf32End))
{
  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(text.c_str());
  builder.SetSpan(Text::ForegroundColorSpan::New(UiColor(0xDC2626)), utf32Start, utf32End);
  label.SetStyledText(builder.Build());
}
~~~

When building text incrementally, use `GetUtf32Length()` before and after `AppendText()` to calculate the appended range.

<br/>

## Annotation

`AnnotationSpan` stores semantic key/value metadata. It does not directly change rendering. Application or theme code can read annotations and resolve them into visual spans.

`AnnotationSpan` is also a `Span`, so it appears when iterating all spans with `GetSpanCount()` and `GetSpanAt()`. `GetAnnotationCount()`, `GetAnnotationAt()`, `GetAnnotationStartIndexAt()`, and `GetAnnotationEndIndexAt()` provide a filtered view for annotation spans, which is convenient when resolving markup or localized text into visual styling. In other words, the annotation APIs are convenience filters; annotation spans are still part of normal span iteration.

~~~cpp
Text::StyledTextBuilder builder = Text::StyledTextBuilder::FromMarkup(
  "<annotation style='muted'>Muted text</annotation> Normal text");

const uint32_t annotationCount = builder.GetAnnotationCount();
for(uint32_t index = 0u; index < annotationCount; ++index)
{
  Text::AnnotationSpan annotation = builder.GetAnnotationAt(index);
  if(annotation.GetKey() == "style" && annotation.GetValue() == "muted")
  {
    builder.SetSpan(Text::ForegroundColorSpan::New(UiColor(0x808080)),
                    builder.GetAnnotationStartIndexAt(index),
                    builder.GetAnnotationEndIndexAt(index));
  }
}

label.SetStyledText(builder.Build());
~~~

Each attribute in one `<annotation>` tag becomes a separate `AnnotationSpan` over the annotated text range.

~~~cpp
Text::StyledTextBuilder builder = Text::StyledTextBuilder::FromMarkup(
  "<annotation style='accent' role='action'>Apply</annotation>");
~~~

The example above creates annotation spans for `style=accent` and `role=action`.

<br/>

## Localized Rich Text

Annotations are useful for localized rich text because the semantic range can move with the translated sentence.

PO example:

~~~po
msgid "IDS_ACTION_MESSAGE"
msgstr "Tap <annotation style='accent'>Apply</annotation> to continue."
~~~

Resolve the localized markup and then apply `StyledText`:

~~~cpp
void MyApp::ApplyLocalizedMessage(BaseHandle target, const Dali::String& localizedMarkup)
{
  Label label = Label::DownCast(target);
  if(!label)
  {
    return;
  }

  Text::StyledTextBuilder builder = Text::StyledTextBuilder::FromMarkup(localizedMarkup);
  const uint32_t annotationCount = builder.GetAnnotationCount();
  for(uint32_t index = 0u; index < annotationCount; ++index)
  {
    Text::AnnotationSpan annotation = builder.GetAnnotationAt(index);
    if(annotation.GetKey() == "style" && annotation.GetValue() == "accent")
    {
      builder.SetSpan(Text::ForegroundColorSpan::New(UiColor::PRIMARY),
                      builder.GetAnnotationStartIndexAt(index),
                      builder.GetAnnotationEndIndexAt(index));
    }
  }

  label.SetStyledText(builder.Build());
}

UiLocalizationManager::Get().SetBindingResource(
  label,
  "StyledText",
  "IDS_ACTION_MESSAGE",
  LocalizedStringCallback::New(this, &MyApp::ApplyLocalizedMessage));
~~~

> [!NOTE]
> `Label::SetTranslatableText()` is convenient for plain localized text. For localized rich text that needs markup or annotations, use direct lookup or `SetBindingResource()` and apply `SetStyledText()` in the callback.

<br/>

## Samples

| Feature | Sample |
|---|---|
| Simple spans | [text-styled-text-simple-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-styled-text-simple-example.cpp) |
| Builder cases | [text-styled-text-builder-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-styled-text-builder-example.cpp) |
| Interactive spans | [text-styled-text-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-styled-text-example.cpp) |
| Markup | [text-markup-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-markup-example.cpp) |

<br/>

---

[← Text Overview](https://github.sec.samsung.net/NUI/dali-ui/wiki/Text)
