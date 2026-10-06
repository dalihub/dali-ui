# Markup

[→ English](https://github.sec.samsung.net/NUI/dali-ui/wiki/Markup)

## Overview

Mark-up tag를 사용하면 텍스트 일부의 color, font, underline, anchor 등을 변경할 수 있습니다. Mark-up string은 `Text::StyledText::FromMarkup()`으로 `StyledText`로 변환한 뒤 `SetStyledText()`로 적용합니다.

Markup은 `Label`, `InputField`, `InputEditor`에서 공통으로 사용할 수 있습니다. 문자열로 style을 작성할 때는 markup을 사용하고, 코드로 text range에 span을 추가하거나 annotation을 해석해야 할 때는 [StyledText](https://github.sec.samsung.net/NUI/dali-ui/wiki/StyledText-(kr))를 참고하세요.

Mark-up string을 `Label::New()`, `Label::SetText()`, `InputField::SetText()`, `InputEditor::SetText()`에 직접 전달하면 plain text로 처리됩니다.

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
> mark-up processor는 mark-up string의 정확성을 검증하지 않습니다. 잘못된 mark-up string은 텍스트가 의도와 다르게 렌더링되는 원인이 될 수 있습니다.

> [!NOTE]
> Mark-up attribute value는 quotation mark로 감싸야 정상 동작을 보장할 수 있습니다. 예: `value='0xFFFF0000'`

<br/>

## 지원 tag

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
| `<img>` | text 안에 inline image 삽입 |
| `<annotation>` | text range에 semantic key/value metadata 추가 |

### Tag attribute

| Tag | Attribute | 설명 |
|---|---|---|
| `<color>` | `value` | 텍스트 색상 |
| `<font>` | `family`, `size`, `weight`, `width`, `slant` | font family, pixel 단위 size, weight, width, slant |
| `<u>` | `color`, `height`, `type`, `dash-gap`, `dash-width` | 밑줄 색상, 두께, 종류, dashed 밑줄의 간격과 길이 |
| `<s>` | `color`, `height` | line-through 색상과 두께 |
| `<background>` | `color` | 텍스트 배경 색상 |
| `<a>` | `href`, `color`, `clicked-color` | link 대상, link 색상, 클릭 시 색상 |
| `<img>` | `src`, `width`, `height` | image source와 pixel 단위의 너비와 높이. 세 값 모두 필수 |
| `<annotation>` | 앱에서 정의한 key/value attribute | 각 attribute를 별도의 `AnnotationSpan`으로 변환 |

`<u>`의 `type`은 `solid`, `dashed`, `double`을 지원합니다. `dash-gap`, `dash-width`는 `dashed` 밑줄에 적용합니다.

`<font>`의 `weight`, `width`, `slant`는 font enum에 대응하는 문자열을 사용합니다. 예: `weight='bold'`, `width='condensed'`, `slant='italic'`.

> [!NOTE]
> DALi markup은 위에 나열한 tag를 지원하며 HTML 전체 문법을 지원하지 않습니다.

Font variation이나 gradient span을 추가하려면 `StyledTextBuilder`와 해당 span API를 사용하세요. Markup으로 시작한 text에 span을 추가하는 방법은 [StyledText And Markup](https://github.sec.samsung.net/NUI/dali-ui/wiki/StyledText-(kr)#styledtext-and-markup)을 참고하세요.

<br/>

## Color 값 형식

`<color>`, `<background>` 등 mark-up에서 사용하는 color 값은 named color 또는 RGB/ARGB hexadecimal 값을 사용할 수 있습니다.

사용 가능한 color 이름: `red`, `green`, `blue`, `yellow`, `magenta`, `cyan`, `white`, `black`, `transparent`

Hexadecimal 형식:

- `0xFFFF0000` — ARGB (`0x` 접두사, `0xAARRGGBB`)
- `#9C3A64` — RGB (`#` 접두사, `#RRGGBB`)
- `#FF9C3A64` — ARGB (`#` 접두사, `#AARRGGBB`)
- `#F00` — RGB 축약형 (`#RGB`). `#FF0000`과 같은 불투명한 빨강
- `0x80FF0000`, `#80FF0000` — 반투명한 빨강

ARGB의 `AA`는 투명도를 나타냅니다. `00`은 완전히 투명하고, `FF`는 불투명합니다.

> [!NOTE]
> RGB 색상은 `#RRGGBB`로 작성합니다. `0x` 형식은 투명도(`AA`)를 포함한 8자리 값(`0xAARRGGBB`)으로 작성하세요. 예를 들어 불투명한 빨강은 `#FF0000` 또는 `0xFFFF0000`입니다.

<br/>

## 예시

아래 예시는 `Label`에 적용합니다. 동일한 markup을 `InputField`, `InputEditor`에도 `SetStyledText()`로 적용할 수 있습니다. Anchor 클릭 처리는 별도로 표시한 Label 전용 예시를 참고하세요.

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
> `<b>`는 시스템에서 bold font를 찾아 적용합니다. bold font가 없으면 software bold 처리를 시도하며, 이 경우 실제 bold font를 사용하는 것보다 품질이 떨어질 수 있습니다.

italic:

~~~cpp
Label label = Label::New();
label.SetStyledText(Text::StyledText::FromMarkup("<i>Italic</i>"));
~~~

> [!NOTE]
> `<i>`는 시스템에서 italic font를 찾아 적용합니다. italic font가 없으면 software italic 처리를 시도하며, 이 경우 실제 italic font를 사용하는 것보다 품질이 떨어질 수 있습니다.

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

anchor 클릭 처리 (Label):

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
> `InputField`, `InputEditor`에도 anchor markup을 적용할 수 있지만, 현재 공개 API에는 `AnchorClickedSignal()`이 없습니다. 위 클릭 처리 예시는 `Label`에서 사용합니다.

### 추가 attribute

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

### Inline image

`<img>`로 텍스트 안에 이미지를 삽입할 수 있습니다. `src`에는 앱에서 불러올 수 있는 이미지 경로를 지정합니다.

~~~cpp
Label label = Label::New();
label.SetStyledText(Text::StyledText::FromMarkup(
  "DALi <img src='res/flag_kr.png' width='32' height='24'/> Icon"));
~~~

크기는 `width='32'`, `height='24'`처럼 숫자로 작성하며, 단위는 pixel입니다. 위 예시는 너비 32pixel, 높이 24pixel의 이미지를 표시합니다.

이미지 정렬 등 추가 설정은 [StyledText의 ImageSpan](https://github.sec.samsung.net/NUI/dali-ui/wiki/StyledText-(kr)#imagespan)을 참고하세요.

### Annotation

`<annotation>`은 text range에 semantic key/value metadata를 붙입니다. 하나의 tag 안에 있는 각 attribute가 동일한 range의 별도 `AnnotationSpan`으로 변환됩니다.

~~~cpp
Text::StyledText styledText = Text::StyledText::FromMarkup(
  "<annotation style='accent' role='action'>Apply</annotation>");
~~~

위 예시는 `Apply` range에 `style=accent`, `role=action` annotation span을 생성합니다. Annotation 자체는 색상이나 font를 변경하지 않습니다. 앱에서 metadata를 읽고 visual span을 추가하는 방법은 [StyledText의 Annotation](https://github.sec.samsung.net/NUI/dali-ui/wiki/StyledText-(kr)#annotation)을 참고하세요.

<br/>

## 중첩 tag와 Entity

Tag를 중첩하면 여러 스타일을 함께 적용할 수 있습니다. 아래 예시는 텍스트를 빨간색의 굵은 글씨로 표시합니다.

~~~cpp
Label label = Label::New();
label.SetStyledText(Text::StyledText::FromMarkup(
  "<color value='red'><b>Red Bold</b></color>"));
~~~

`<`, `>`, `&`처럼 markup 문법에 쓰이는 기호를 텍스트로 표시할 때는 Entity를 사용할 수 있습니다. 예를 들어 `&lt;`는 `<`로 표시됩니다.

| 작성 값 | 변환 결과 |
|---|---|
| `&lt;` | `<` |
| `&gt;` | `>` |
| `&amp;` | `&` |
| `&quot;` | `"` |
| `&apos;` | `'` |
| `&#65;`, `&#x41;` | `A` (10진수 / 16진수 문자 코드) |

~~~cpp
Label label = Label::New();
label.SetStyledText(Text::StyledText::FromMarkup("&lt;tag&gt; &amp; &#65; &#x41;"));
~~~

위 예시는 `<tag> & A A`로 표시됩니다. Attribute 값에도 같은 표기를 사용할 수 있습니다. 예를 들어 `href='https://example.com?a=1&amp;b=2'`의 URL은 `https://example.com?a=1&b=2`로 처리됩니다.

<br/>

## 입력 컨트롤에서 사용

`InputField`, `InputEditor`는 `SetStyledText()`로 적용한 style을 초기 editable style로 사용합니다. 사용자가 text를 편집하면 남아 있는 span의 range도 갱신됩니다.

`GetText()`는 markup tag가 포함되지 않은 plain text를 반환합니다. 원래 markup 문자열이나 `StyledText`를 되돌려 주는 API는 아니므로, 원본이 필요하면 앱에서 보관하세요. `SetText()`를 호출하면 기존 styled text 상태가 지워집니다.

입력·선택·typing style 관련 API는 [Text Input](https://github.sec.samsung.net/NUI/dali-ui/wiki/Text-Input-(kr))을 참고하세요.

<br/>

## Samples

| 기능 | 샘플 |
|---|---|
| Label / InputField / InputEditor markup | [text-markup-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-markup-example.cpp) |
| Markup 변환과 inline image | [text-styled-text-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-styled-text-example.cpp) |
| 입력 컨트롤의 ImageSpan | [text-input-image-span-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-input-image-span-example.cpp) |

<br/>

---

[← Text Overview](https://github.sec.samsung.net/NUI/dali-ui/wiki/Text-(kr))
