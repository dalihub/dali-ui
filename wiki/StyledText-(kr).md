# StyledText

[→ English](https://github.sec.samsung.net/NUI/dali-ui/wiki/StyledText)

## Overview

`StyledText`는 UTF-8 입력으로부터 만들어지는 styled text 값입니다. DALi는 입력 텍스트를 decode하고, decoded UTF-32 sequence 기준 range에 span을 attach합니다. 앱에서 특정 텍스트 범위를 코드로 스타일링하거나, semantic markup을 해석해서 rich text로 적용해야 할 때 사용할 수 있습니다.

`StyledTextBuilder`는 `StyledText` snapshot을 만들기 위한 조립용 API입니다. 텍스트를 순서대로 구성할 때는 `PushSpan()`과 `PopSpan()`으로 range index를 직접 계산하지 않고 append되는 텍스트에 style을 적용할 수 있습니다. 대상 range를 이미 알고 있는 경우에는 `SetSpan()`을 사용합니다.

`Label`, `InputField`, `InputEditor`는 `SetStyledText()`로 `StyledText` 값을 적용할 수 있습니다.

~~~cpp
Label label = Label::New();

Text::StyledTextBuilder builder = Text::StyledTextBuilder::New();
builder.AppendText("Hello ");
builder.PushSpan(Text::ForegroundColorSpan::New(UiColor(0x2563EB)));
builder.AppendText("StyledText");
builder.PopSpan();

label.SetStyledText(builder.Build());
~~~

![StyledText 결과](./assets/text/hello_styled_text.png)

> [!NOTE]
> `StyledText` range index는 UTF-8 byte offset이 아니라 decoded text의 UTF-32 index입니다.

<br/>

## StyledText And Markup

단순한 static markup은 바로 `StyledText`로 변환할 수 있습니다.

~~~cpp
Label label = Label::New();
label.SetStyledText(Text::StyledText::FromMarkup("<color value='red'>Red</color> text"));
~~~

markup을 먼저 parsing한 뒤 앱에서 span을 추가해야 한다면 `StyledTextBuilder::FromMarkup()`을 사용합니다.

~~~cpp
Text::StyledTextBuilder builder =
  Text::StyledTextBuilder::FromMarkup("<u>Hello</u> StyledText");

builder.SetSpan(Text::ForegroundColorSpan::New(UiColor(0xDC2626)), 0u, 5u);

label.SetStyledText(builder.Build());
~~~

> [!NOTE]
> `Label::New()`, `SetText()`, `InputField::SetText()`, `InputEditor::SetText()`에 markup을 전달하면 plain text로 처리됩니다. parsing된 markup이나 span을 적용하려면 `SetStyledText()`를 사용하세요.

<br/>

## Span Types

`Span`은 style payload를 저장합니다. Text range attachment는 `StyledTextBuilder`가 관리하고 `StyledText`에 저장됩니다.

| Span | 용도 |
|---|---|
| `ForegroundColorSpan` | 텍스트 색상 |
| `GradientSpan` | 특정 텍스트 범위의 foreground gradient |
| `BackgroundColorSpan` | 텍스트 배경색 |
| `FontSpan` | font family, size, weight, width, slant, variation |
| `UnderlineSpan` | underline style |
| `LineThroughSpan` | line-through style |
| `ImageSpan` | 텍스트 범위를 atomic inline image로 치환 |
| `AnchorSpan` | link 처리를 위한 anchor metadata |
| `AnnotationSpan` | semantic key/value metadata |

서로 다른 style category라면 여러 span을 같은 range에 적용할 수 있습니다.

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

![Span types 결과](./assets/text/span_types.png)

> [!NOTE]
> builder는 같은 `Span` handle에 대해 하나의 attachment만 저장합니다. 같은 span handle을 다시 attach하면 range가 갱신됩니다. 동일한 시각 스타일을 여러 range에 적용하려면 별도의 span handle을 생성하세요.

<br/>

## Push And Pop

텍스트를 append하면서 style을 적용할 때는 `PushSpan()`과 `PopSpan()`을 사용하는 것이 가장 간단합니다. `PushSpan()`은 현재 decoded text 끝 위치에서 span을 열고, `PopSpan()`은 현재 끝 위치에서 span을 닫습니다. 두 호출 사이에 append된 텍스트가 자동으로 span range가 됩니다.

~~~cpp
Text::StyledTextBuilder builder = Text::StyledTextBuilder::New();

builder.AppendText("Normal ");
builder.PushSpan(Text::ForegroundColorSpan::New(UiColor(0xDC2626)));
builder.AppendText("Red");
builder.PopSpan();
builder.AppendText(" Normal");

label.SetStyledText(builder.Build());
~~~

![Push and Pop 결과](./assets/text/push_and_pop.png)

`PopSpan()`은 가장 최근에 열린 span을 닫습니다. Nested construction에서 특정 pushed span을 닫아야 한다면 `PopSpan(token)`을 사용할 수 있습니다.

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

![Push and Pop token 결과](./assets/text/push_and_pop_token.png)

`PopSpan(token)`은 token과 일치하는 open span을 닫습니다. 해당 token 이후에 열린 span이 아직 열려 있다면 같은 위치에서 함께 닫힙니다. Search, parser, application logic 등으로 range를 이미 계산한 경우에는 `SetSpan()`을 사용합니다.

<br/>

## UTF-32 Ranges

`SetSpan()`과 span inspection API는 UTF-32 index를 사용합니다. `PushSpan()`과 `PopSpan()`으로 텍스트를 구성하는 경우에는 보통 index를 직접 계산할 필요가 없지만, source text에 non-ASCII 문자나 emoji가 포함되어 있거나 UTF-8 string API에서 찾은 범위를 사용할 때는 주의해야 합니다.

범위가 UTF-8 byte offset으로 주어진 경우 `SetSpan()` 호출 전에 변환합니다.

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

텍스트를 단계적으로 append하는 경우에는 `AppendText()` 전후의 `GetUtf32Length()` 값을 사용해서 새로 추가된 range를 계산할 수 있습니다.

<br/>

## ImageSpan

`ImageSpan`은 지정한 text range를 하나의 atomic inline image box로 치환합니다. Image source, layout에서 확보할 크기, text baseline에 대한 정렬 등은 `ImageAttributes`로 설정합니다.

가장 안전한 작성 방식은 image가 들어갈 위치에 U+FFFC OBJECT REPLACEMENT CHARACTER 하나를 넣고, 그 한 글자에 `ImageSpan`을 적용하는 것입니다.

~~~cpp
Text::StyledTextBuilder builder = Text::StyledTextBuilder::New();
builder.AppendText("DALi ");

// U+FFFC를 append하기 전의 UTF-32 길이가 image range의 시작입니다.
const uint32_t imageIndex = builder.GetUtf32Length();
builder.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);
builder.AppendText(" Icon");

Text::ImageAttributes attributes("res/dali-icon.png", Vector2(72.0f, 72.0f));
attributes.SetAlignment(Text::ImageAttributes::InlineAlignment::TEXT_CENTER);

builder.SetSpan(Text::ImageSpan::New(attributes), imageIndex, imageIndex + 1u);
label.SetStyledText(builder.Build());
~~~

![ImageSpan 결과](./assets/text/image_span.png)

여기서 주의할 점은 다음과 같습니다.

- `OBJECT_REPLACEMENT_CHARACTER`는 UTF-8로는 3 byte이지만 decoded text에서는 UTF-32 index 하나를 차지합니다.
- `SetSpan()`의 range는 반개구간 `[start, end)`이므로 U+FFFC 하나를 치환하려면 `[imageIndex, imageIndex + 1)`을 사용합니다.
- `ImageSpan`이 적용되면 range 안의 logical text는 `StyledText`에 남아 있지만 화면에는 하나의 image layout unit으로 나타납니다.
- 여러 code point를 하나의 image로 치환하는 것도 지원하지만, source와 range의 의미가 분명하고 편집 시 다루기 쉬운 U+FFFC 한 글자 방식을 권장합니다.
- Range 또는 필수 `ImageAttributes`가 유효하지 않으면 replacement가 적용되지 않고 원래 range가 그대로 남습니다.

Append 구간을 span으로 감싸는 방식도 사용할 수 있습니다.

~~~cpp
Text::ImageAttributes attributes("res/dali-icon.png", Vector2(72.0f, 72.0f));

builder.PushSpan(Text::ImageSpan::New(attributes));
builder.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);
builder.PopSpan();
~~~

<br/>

## GradientSpan

`GradientSpan`은 전체 text가 아니라 지정한 range의 foreground fill에만 gradient를 적용합니다. 일반 text color와 섞어 문장 안의 특정 단어나 구문만 강조할 때 사용할 수 있습니다.

~~~cpp
Text::StyledTextBuilder builder = Text::StyledTextBuilder::New();
builder.AppendText("일반 텍스트와 ");

const uint32_t gradientStart = builder.GetUtf32Length();
builder.AppendText("그라디언트 텍스트");
const uint32_t gradientEnd = builder.GetUtf32Length();

Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
gradient.SetStopNodes({
  Gradient::StopNode(0.0f, UiColor(0x0EA5E9)),
  Gradient::StopNode(0.25f, UiColor(0x2563EB)),
  Gradient::StopNode(0.5f, UiColor(0x7C3AED)),
  Gradient::StopNode(0.75f, UiColor(0xDB2777)),
  Gradient::StopNode(1.0f, UiColor(0xF97316)),
});

builder.SetSpan(
  Text::GradientSpan::New(gradient, Text::GradientSpan::BoundsMode::SPAN_BOUND),
  gradientStart,
  gradientEnd);

label.SetTextColor(UiColor(0x111827));
label.SetStyledText(builder.Build());
~~~

![GradientSpan 결과](./assets/text/gradient_span.png)

`BoundsMode`는 gradient 좌표를 평가할 기준 영역을 선택합니다.

| Mode | 기준 영역 | 용도 |
|---|---|---|
| `SPAN_BOUND` | 이 span이 소유한 visible glyph ink bounds의 합집합 | 특정 단어나 구문 자체에 맞춘 gradient. 기본값입니다. |
| `CONTENT_BOUND` | layout된 전체 text content bounds | 서로 다른 span이 하나의 content 기준 gradient 흐름을 공유해야 하는 경우 |
| `VIEW_BOUND` | text view 전체 bounds | Padding을 포함한 view 영역을 기준으로 gradient를 고정해야 하는 경우 |

`Gradient::Units::OBJECT_BOUNDING_BOX`는 선택된 bounds 안에서 normalized coordinate를 사용하고, `Gradient::Units::USER_SPACE`는 같은 bounds 안의 pixel coordinate를 사용합니다. 즉, `BoundsMode`는 기준 rectangle을 고르고 `Gradient::Units`는 그 rectangle 안에서 좌표를 해석하는 방식을 고릅니다.

`GradientSpan::New()`는 전달한 gradient를 snapshot으로 저장합니다. 원본 gradient를 나중에 변경해도 이미 만들어진 span에는 반영되지 않으며, gradient를 바꾸려면 새 `GradientSpan`을 만들어 다시 적용해야 합니다. `ForegroundColorSpan`과 range가 겹치면 나중에 적용한 span이 겹치는 문자의 foreground paint를 결정하고, color emoji는 고유 색상을 유지합니다.

<br/>

## Annotation

`AnnotationSpan`은 semantic key/value metadata를 저장합니다. 자체적으로 rendering을 바꾸지는 않으며, 앱이나 theme code가 annotation을 읽고 visual span으로 변환하는 방식으로 사용합니다.

`AnnotationSpan`도 `Span`이므로 `GetSpanCount()`와 `GetSpanAt()`으로 전체 span을 순회하면 함께 나타납니다. Annotation만 처리할 때는 `GetAnnotationCount()`, `GetAnnotationAt()`, `GetAnnotationStartIndexAt()`, `GetAnnotationEndIndexAt()` API를 사용하면 annotation span만 순회할 수 있어 markup이나 localized text를 visual styling으로 변환하는 코드가 단순해집니다. 즉, annotation 전용 API는 convenience filter이고, annotation span은 일반 span 순회에도 포함됩니다.

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

하나의 `<annotation>` tag 안에 있는 각 attribute는 동일한 text range를 가진 별도의 `AnnotationSpan`으로 변환됩니다.

~~~cpp
Text::StyledTextBuilder builder = Text::StyledTextBuilder::FromMarkup(
  "<annotation style='accent' role='action'>Apply</annotation>");
~~~

위 예시는 `style=accent`, `role=action` annotation span을 생성합니다.

<br/>

## Localized Rich Text

Annotation은 localized rich text에 유용합니다. 번역된 문장에서 어순이 바뀌더라도 semantic range가 번역 문자열 안에서 함께 이동할 수 있기 때문입니다.

PO 예:

~~~po
msgid "IDS_ACTION_MESSAGE"
msgstr "Tap <annotation style='accent'>Apply</annotation> to continue."
~~~

localized markup을 resolve한 뒤 `StyledText`로 적용합니다.

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
> `Label::SetTranslatableText()`는 plain localized text에 편리합니다. Markup이나 annotation이 포함된 localized rich text가 필요하다면 direct lookup 또는 `SetBindingResource()` callback에서 `SetStyledText()`를 적용하세요.

### 다국어 GradientSpan

번역 문자열에는 gradient 자체를 넣는 대신 semantic annotation만 유지하고, callback에서 해당 range를 `GradientSpan`으로 변환할 수 있습니다. 번역으로 어순이나 문자열 길이가 바뀌어도 annotation range가 함께 이동합니다.

~~~po
msgid "IDS_GRADIENT_ACTION_MESSAGE"
msgstr "Today <annotation style='gradient'>the weather is clear</annotation> and it is a great day for exercise."
~~~

Gradient 생성 코드는 helper로 분리하면 localized range를 처리하는 핵심 흐름을 짧게 유지할 수 있습니다.

~~~cpp
Gradient::Base CreateLocalizedAccentGradient()
{
  Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetStopNodes({
    Gradient::StopNode(0.0f, UiColor(0x2563EB)),
    Gradient::StopNode(0.5f, UiColor(0x7C3AED)),
    Gradient::StopNode(1.0f, UiColor(0xF97316)),
  });
  return gradient;
}

void MyApp::ApplyLocalizedGradientMessage(BaseHandle target,
                                          const Dali::String& localizedMarkup)
{
  Label label = Label::DownCast(target);
  if(!label)
  {
    return;
  }

  Text::StyledTextBuilder builder = Text::StyledTextBuilder::FromMarkup(localizedMarkup);
  const Gradient::Base gradient = CreateLocalizedAccentGradient();

  const uint32_t annotationCount = builder.GetAnnotationCount();
  for(uint32_t index = 0u; index < annotationCount; ++index)
  {
    Text::AnnotationSpan annotation = builder.GetAnnotationAt(index);
    if(annotation.GetKey() == "style" && annotation.GetValue() == "gradient")
    {
      builder.SetSpan(
        Text::GradientSpan::New(gradient, Text::GradientSpan::BoundsMode::SPAN_BOUND),
        builder.GetAnnotationStartIndexAt(index),
        builder.GetAnnotationEndIndexAt(index));
    }
  }

  label.SetStyledText(builder.Build());
}

UiLocalizationManager::Get().SetBindingResource(
  label,
  "StyledText",
  "IDS_GRADIENT_ACTION_MESSAGE",
  LocalizedStringCallback::New(this, &MyApp::ApplyLocalizedGradientMessage));
~~~

`SPAN_BOUND`를 사용하면 번역된 강조 구문의 실제 glyph bounds에 맞춰 gradient가 다시 평가됩니다.

![다국어 GradientSpan](./assets/text/gradient_span_localization.gif)

<br/>

## Samples

| 기능 | 샘플 |
|---|---|
| Simple spans | [text-styled-text-simple-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-styled-text-simple-example.cpp) |
| Builder cases | [text-styled-text-builder-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-styled-text-builder-example.cpp) |
| Interactive spans | [text-styled-text-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-styled-text-example.cpp) |
| ImageSpan | [text-image-span-simple-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-image-span-simple-example.cpp) |
| GradientSpan | [text-gradient-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-gradient-example.cpp) |
| Localized GradientSpan | [text-gradient-localization-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-gradient-localization-example.cpp) |
| Markup | [text-markup-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-markup-example.cpp) |

<br/>

---

[← Text Overview](https://github.sec.samsung.net/NUI/dali-ui/wiki/Text-(kr))
