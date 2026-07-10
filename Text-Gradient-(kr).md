# Text Gradient

[→ English](https://github.sec.samsung.net/NUI/dali-ui/wiki/Text-Gradient)

## Overview

`TextGradient`는 `Label`의 text glyph fill에 gradient를 적용하는 rendering 기능입니다. `View`에 background gradient를 설정하는 것과 달리, gradient는 실제로 보이는 text glyph pixel에만 sampling됩니다.

`TextGradientOverlay`는 resolve된 glyph fill 위에 추가 gradient를 적용합니다. 별도의 overlay actor를 만들지 않고 highlight나 shimmer 효과를 만들 때 유용합니다.

~~~text
Label text color / StyledText color / TextGradient
  -> resolved glyph fill
  -> TextGradientOverlay
  -> rendered glyph pixels
~~~

<br/>

## 시각 예시

| 케이스 | 미리보기 | 설명 |
|---|---|---|
| `TextGradient` | ![TextGradient](./assets/text/text_gradient.gif) | 하나의 gradient를 glyph fill로 사용합니다. |
| `TextGradientOverlay` | ![TextGradientOverlay](./assets/text/text_gradient_overlay.gif) | Glyph fill이 resolve된 뒤 overlay gradient를 적용합니다. |
| `TextGradient` + `TextGradientOverlay` | ![TextGradient and overlay](./assets/text/text_gradient_and_overlay.gif) | 기본 text gradient와 overlay gradient를 함께 사용합니다. |

<br/>

## Basic TextGradient

DALi `Gradient`를 만들고 `Label::SetTextGradient()`로 적용합니다.

~~~cpp
Label label = Label::New("Gradient Text");
label.SetFontSize(60.0f);

Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
gradient.SetSpreadMethod(Gradient::SpreadMethod::PAD);
gradient.SetStopNodes({
  Gradient::StopNode(0.0f, UiColor(0x2563EB)),
  Gradient::StopNode(0.45f, UiColor(0x14B8A6)),
  Gradient::StopNode(1.0f, UiColor(0xF97316)),
});

label.SetTextGradient(gradient);
~~~

`Gradient::Type::NONE`이거나 stop node가 2개 미만인 gradient를 설정하면 text gradient가 제거되고 일반 text color rendering으로 돌아갑니다.

~~~cpp
label.SetTextGradient(Gradient::Base::None());
~~~

<br/>

## Bounds Mode

`Text::GradientBoundsMode`는 gradient coordinate를 평가할 rectangle을 선택합니다.

| Mode | 설명 |
|---|---|
| `CONTENT_BOUND` | layout된 text content bounds를 사용합니다. 기본값입니다. |
| `VIEW_BOUND` | padding을 포함한 전체 `Label` view bounds를 사용합니다. |

~~~cpp
label.SetTextGradientBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
label.SetTextGradient(gradient);
~~~

Gradient가 텍스트 자체를 따라가야 하면 `CONTENT_BOUND`를 사용합니다. 여러 label이나 여러 줄 텍스트가 view 영역 기준 gradient를 공유해야 한다면 `VIEW_BOUND`를 사용합니다.

<br/>

## Gradient Units

`Gradient::Units`는 선택된 bounds 안에서 coordinate를 어떻게 해석할지 결정합니다.

| Units | Coordinate 의미 |
|---|---|
| `OBJECT_BOUNDING_BOX` | 선택된 bounds 내부의 normalized coordinate입니다. Label text gradient에서는 보통 `-0.5` ~ `0.5` 근처 값을 사용합니다. |
| `USER_SPACE` | 선택된 bounds 내부의 pixel coordinate입니다. |

Radial gradient에서 non-square `OBJECT_BOUNDING_BOX` bounds를 사용하면 gradient가 선택된 bounds의 aspect를 따를 수 있습니다. Pixel 기준 center/radius 값을 사용해야 한다면 `USER_SPACE`를 사용하세요.

<br/>

## TextGradientOverlay

`TextGradientOverlay`는 base glyph fill이 resolve된 뒤 적용됩니다. Base fill은 `SetTextColor()`, `StyledText` color span, color glyph rendering, 또는 `TextGradient`에서 올 수 있습니다.

Overlay는 visible glyph fill pixel에만 적용됩니다. Shadow, underline, line-through, outline, background 같은 text decoration에는 영향을 주지 않습니다.

~~~cpp
Gradient::Linear overlay(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
overlay.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
overlay.SetSpreadMethod(Gradient::SpreadMethod::PAD);
overlay.SetStopNodes({
  Gradient::StopNode(0.0f, UiColor(1.0f, 1.0f, 1.0f, 0.0f)),
  Gradient::StopNode(0.5f, UiColor(1.0f, 1.0f, 1.0f, 0.85f)),
  Gradient::StopNode(1.0f, UiColor(1.0f, 1.0f, 1.0f, 0.0f)),
});

label.SetTextGradientOverlayBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
label.SetTextGradientOverlayMode(Text::GradientOverlayMode::SCREEN);
label.SetTextGradientOverlay(overlay);
~~~

| Overlay Mode | 설명 |
|---|---|
| `SRC_OVER` | Overlay gradient source를 resolved glyph fill 위에 그립니다. Transparent stop은 glyph fill을 변경하지 않습니다. |
| `SCREEN` | Overlay gradient와 resolved glyph fill을 screen 방식으로 합성합니다. Highlight나 shimmer 효과에 유용합니다. |

Overlay 제거:

~~~cpp
label.SetTextGradientOverlay(Gradient::Base::None());
~~~

<br/>

## Animation

Gradient motion은 보통 gradient start offset을 animate해서 만듭니다. `TextGradient`와 `TextGradientOverlay`는 모두 `Label` animation helper를 통해 start offset animation을 지원합니다.

~~~cpp
Gradient::Linear gradient(Vector2(-0.35f, 0.0f), Vector2(0.35f, 0.0f));
gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
gradient.SetSpreadMethod(Gradient::SpreadMethod::REFLECT);
gradient.SetStartOffset(0.0f);
gradient.SetStopNodes({
  Gradient::StopNode(0.0f, UiColor(0x7C3AED)),
  Gradient::StopNode(0.5f, UiColor(0x06B6D4)),
  Gradient::StopNode(1.0f, UiColor(0xF97316)),
});

label.SetTextGradient(gradient);

Animation animation = Animation::New(2.0f);
label.Animate(animation)
  .TextGradientStartOffset(1.0f, Duration(2.0f), AlphaFunction::LINEAR);
animation.SetLooping(true);
animation.Play();
~~~

Overlay gradient를 animate하려면 overlay start offset helper를 사용합니다.

~~~cpp
Animation animation = Animation::New(1.6f);
label.Animate(animation)
  .TextGradientOverlayStartOffset(-1.15f, Duration(1.6f), AlphaFunction::LINEAR);
animation.SetLooping(true);
animation.Play();
~~~

<br/>

## TextGradient And StyledText

`StyledText` color span과 `TextGradient`는 모두 text glyph fill에 영향을 줄 수 있지만, 적용 레벨이 다릅니다.

| 기능 | 적합한 용도 |
|---|---|
| `StyledText` color span | 특정 text range만 스타일링 |
| `TextGradient` | label text 전체에 하나의 gradient 적용 |
| `TextGradientOverlay` | resolve된 glyph fill 위에 highlight 또는 shimmer 적용 |

`StyledText`와 `TextGradientOverlay`를 함께 사용하면 overlay는 glyph fill이 resolve된 뒤 적용됩니다.

<br/>

## Samples

| 기능 | 샘플 |
|---|---|
| Simple text gradient | [text-gradient-simple-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-gradient-simple-example.cpp) |
| Gradient modes and bounds | [text-gradient-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-gradient-example.cpp) |
| Overlay shimmer | [text-effect-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-effect-example.cpp) |
| Localized gradient text | [text-gradient-localization-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-gradient-localization-example.cpp) |

<br/>

---

[← Text Overview](https://github.sec.samsung.net/NUI/dali-ui/wiki/Text-(kr))
