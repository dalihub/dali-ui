# Text Gradient

[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Text-Gradient-(kr))

## Overview

`TextGradient` is a `Label` rendering feature that applies a gradient to the resolved text glyph fill. It is different from setting a background gradient on a `View`: the gradient is sampled only for visible text glyph pixels.

`TextGradientOverlay` applies an additional gradient over the resolved glyph fill. It is useful for highlight or shimmer effects without creating a separate overlay actor.

~~~text
Label text color / StyledText color / TextGradient
  -> resolved glyph fill
  -> TextGradientOverlay
  -> rendered glyph pixels
~~~

<br/>

## Visual Examples

| Case | Preview | Description |
|---|---|---|
| `TextGradient` | ![TextGradient](./assets/text/text_gradient.gif) | Uses one gradient as the glyph fill. |
| `TextGradientOverlay` | ![TextGradientOverlay](./assets/text/text_gradient_overlay.gif) | Applies an overlay gradient after the glyph fill is resolved. |
| `TextGradient` + `TextGradientOverlay` | ![TextGradient and overlay](./assets/text/text_gradient_and_overlay.gif) | Combines a base text gradient with an overlay gradient. |

<br/>

## Basic TextGradient

Create a DALi `Gradient` and apply it with `Label::SetTextGradient()`.

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

A gradient with `Gradient::Type::NONE`, or a gradient with fewer than two stop nodes, removes the text gradient and falls back to normal text color rendering.

~~~cpp
label.SetTextGradient(Gradient::Base::None());
~~~

<br/>

## Bounds Mode

`Text::GradientBoundsMode` selects the rectangle used to evaluate gradient coordinates.

| Mode | Description |
|---|---|
| `CONTENT_BOUND` | Uses laid-out text content bounds. This is the default. |
| `VIEW_BOUND` | Uses the full `Label` view bounds, including padding. |

~~~cpp
label.SetTextGradientBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
label.SetTextGradient(gradient);
~~~

Use `CONTENT_BOUND` when the gradient should follow the text content itself. Use `VIEW_BOUND` when multiple labels or multiple lines should share a gradient mapped to the view area.

<br/>

## Gradient Units

`Gradient::Units` controls how coordinates are interpreted inside the selected bounds.

| Units | Coordinate Meaning |
|---|---|
| `OBJECT_BOUNDING_BOX` | Normalized coordinates inside the selected bounds. For label text gradients, the common range is around `-0.5` to `0.5`. |
| `USER_SPACE` | Pixel coordinates inside the selected bounds. |

For radial gradients, non-square `OBJECT_BOUNDING_BOX` bounds can make the gradient follow the selected bounds aspect. Use `USER_SPACE` when pixel-like center and radius values are needed.

<br/>

## TextGradientOverlay

`TextGradientOverlay` is evaluated after the base glyph fill is resolved. The base fill may come from `SetTextColor()`, `StyledText` color spans, color glyph rendering, or `TextGradient`.

The overlay affects visible glyph fill pixels only. Text decorations such as shadow, underline, line-through, outline, and background are not affected.

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

| Overlay Mode | Description |
|---|---|
| `SRC_OVER` | Draws the overlay gradient source over the resolved glyph fill. Transparent stops leave the glyph fill unchanged. |
| `SCREEN` | Screens the overlay gradient with the resolved glyph fill. This is useful for highlight and shimmer effects. |

Remove the overlay with:

~~~cpp
label.SetTextGradientOverlay(Gradient::Base::None());
~~~

<br/>

## Animation

Gradient motion is usually created by animating the gradient start offset. Both `TextGradient` and `TextGradientOverlay` support start offset animation through `Label` animation helpers.

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

To animate the overlay gradient, use the overlay start offset helper:

~~~cpp
Animation animation = Animation::New(1.6f);
label.Animate(animation)
  .TextGradientOverlayStartOffset(-1.15f, Duration(1.6f), AlphaFunction::LINEAR);
animation.SetLooping(true);
animation.Play();
~~~

<br/>

## TextGradient And StyledText

`StyledText` color spans and `TextGradient` both affect text glyph fill. Use them for different levels of styling:

| Feature | Best For |
|---|---|
| `StyledText` color spans | Styling specific text ranges |
| `TextGradient` | Applying one gradient across the resolved label text |
| `TextGradientOverlay` | Applying a highlight or shimmer over the resolved glyph fill |

When both `StyledText` and `TextGradientOverlay` are used, the overlay is applied after the glyph fill is resolved.

<br/>

## Samples

| Feature | Sample |
|---|---|
| Simple text gradient | [text-gradient-simple-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-gradient-simple-example.cpp) |
| Gradient modes and bounds | [text-gradient-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-gradient-example.cpp) |
| Overlay shimmer | [text-effect-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-effect-example.cpp) |
| Localized gradient text | [text-gradient-localization-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-gradient-localization-example.cpp) |

<br/>

---

[← Text Overview](https://github.sec.samsung.net/NUI/dali-ui/wiki/Text)
