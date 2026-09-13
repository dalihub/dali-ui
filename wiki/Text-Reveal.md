# Text Reveal

[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Text-Reveal-(kr))

## Overview

Text Reveal displays the text foreground and inline `ImageSpan` in a `Label`'s final layout in sequence according to normalized progress. `Text::Reveal` defines the reveal unit, sequence, and fade, while the application controls playback duration and direction by setting or animating `TextRevealProgress`.

- Progress `0.0`: All reveal targets are hidden.
- Progress `1.0`: All reveal targets are fully visible.
- Decreasing progress follows the same schedule in reverse and hides the content.

![Text Reveal](./assets/text/text_reveal.gif)

<br/>

## Basic Usage

~~~cpp
Label label = Label::New("Sequential text reveal");

Text::Reveal reveal;
reveal.SetUnit(Text::Reveal::Unit::CHARACTER);
reveal.SetSequence(Text::Reveal::Sequence::WHOLE_TEXT);
reveal.SetFadeDurationRatio(Text::Reveal::AUTO_FADE_DURATION_RATIO);
reveal.SetSequenceStaggerRatio(0.0f);

label.SetTextReveal(reveal);
label.SetTextRevealProgress(0.0f);

Animation animation = Animation::New(2.0f);
label.Animate(animation)
  .TextRevealProgress(1.0f, Duration(2.0f), AlphaFunction::EASE_IN_OUT);
animation.Play();
~~~

`TextRevealProgress` is clamped to the range `0.0` to `1.0`, and NaN is normalized to `0.0`. Its initial value is `0.0`. `Text::Reveal` does not define playback time; the application supplies the total duration and alpha function through an animation, as shown above.

To hide fully visible content again, animate the target progress to `0.0`.

~~~cpp
Animation reverse = Animation::New(1.5f);
label.Animate(reverse)
  .TextRevealProgress(0.0f, Duration(1.5f), AlphaFunction::EASE_IN_OUT);
reverse.Play();
~~~

Set `Text::Reveal::None()` to remove Reveal rendering.

~~~cpp
label.SetTextReveal(Text::Reveal::None());
~~~

When Reveal is disabled, the content is fully visible regardless of the current progress. Progress is preserved, so enabling Reveal again applies the existing value.

<br/>

## Unit

`Unit` selects how the reveal schedule divides content.

| Unit | Behavior |
|---|---|
| `CHARACTER` | The default. Content progresses character by character, but ligatures, combining sequences, emoji clusters, and other elements that shaping cannot separate are revealed together. |
| `WORD` | Reveals at word boundaries. Whitespace does not consume a separate unit, and punctuation is associated with an adjacent word where possible. |
| `LINE` | Treats each final visible layout line, after wrapping and ellipsis processing, as one unit. |
| `PIXEL` | Continuously reveals the visible foreground pixel by pixel. |

The progression order of `CHARACTER`, `WORD`, and `PIXEL` follows logical text order and shaping boundaries. Consequently, bidirectional text is not necessarily revealed from the left side of the screen to the right.

`PIXEL` distributes reveal timing in proportion to the width occupied by each glyph and inline image. Wider elements receive a longer progress interval, while narrower elements receive a shorter one. `WHOLE_TEXT` treats all visible content as one continuous distance, while `PER_LINE` calculates each line as an independent distance. With a `LINEAR` animation, reveal therefore moves at a roughly constant speed even when glyphs and images have different widths.

~~~cpp
reveal.SetUnit(Text::Reveal::Unit::PIXEL);
label.SetTextReveal(reveal);
~~~

<!-- Add after receiving the GIF: ![Text Reveal unit comparison](./assets/text/text_reveal_units.gif) -->

<br/>

## Fade Duration Ratio

`FadeDurationRatio` is not a delay between units. It is the length of the interval over which **each unit fades** on the normalized reveal timeline. Explicit values range from `0.0` to `1.0`. The default, `AUTO_FADE_DURATION_RATIO`, resolves automatically from the final visible content.

For a single sequence containing `N` reveal units of equal weight, such as `CHARACTER`, `WORD`, or `LINE`, and a fade ratio `F`, the basic schedule is:

~~~text
N > 1:
  startInterval = (1 - F) / (N - 1)
  start(i)      = i * startInterval,  i = 0, ..., N - 1

N = 1:
  start(0) = 0
~~~

For `F > 0`, the opacity of unit `i` is calculated as follows:

~~~text
opacity(i, progress) = clamp((progress - start(i)) / F, 0, 1)
~~~

- `F = 0`: Each unit appears in sequence without fading, producing a step/typewriter reveal. At progress `0.0`, all units, including the first one, are hidden.

  ![FadeDurationRatio 0](./assets/text/fade_ratio_0.gif)

- `F = 1`: In a single sequence, every unit starts at `0`, so all content fades together.

  ![FadeDurationRatio 1](./assets/text/fade_ratio_1.gif)

- `0 < F < 1`: The next unit may start before the previous unit finishes fading. With the same number of units, a larger value produces a longer overlap.

  ![FadeDurationRatio 0.5](./assets/text/fade_ratio_0.5.gif)

With `WHOLE_TEXT`, the schedule above applies to all content. With `PER_LINE`, a schedule is built for each line and then normalized into one overall timeline that includes stagger. `PIXEL` does not use the simple `startInterval` equation above because it weights the actual progression space occupied by glyphs and inline images instead of dividing the timeline evenly by unit count.

For a single sequence animated from `0.0` to `1.0` with `LINEAR` over a total duration `T`, each unit's actual fade time is `F * T`, and the interval between unit starts is `startInterval * T`. For example, if `N = 5`, `F = 0.25`, and `T = 4 seconds`, each unit fades for `1 second` and the next unit starts every `0.75 seconds`.

Explicit ratios outside the supported range are clamped, and NaN is normalized to `0.0`. `AUTO_FADE_DURATION_RATIO` remains a special sentinel value. `GetFadeDurationRatio()` returns this authored value rather than the internally resolved duration.

<br/>

### Automatic Fade

`AUTO_FADE_DURATION_RATIO` determines the fade duration automatically from the final visible units. It preserves a sequential feel for short content and adjusts overlap for long content so that each unit's fade does not become excessively short. For `PIXEL`, the calculation accounts for the visible spatial range and text scale.

The automatic calculation uses the final rendering result rather than the original source length.

- Whitespace and source content hidden by elision do not consume reveal units.
- A visible ellipsis glyph participates in reveal progression.
- An inline `ImageSpan` participates as a visible replacement.

<br/>

## Sequence

`Sequence` determines whether multi-line content shares one timeline or gives each line an independent timeline.

| Sequence | Behavior |
|---|---|
| `WHOLE_TEXT` | The default. All visible content shares one reveal sequence. |
| `PER_LINE` | Creates an independent sequence for each final visible line. |

With `PER_LINE`, lines are determined after shaping, wrapping, maximum-line limits, ellipsis, bidirectional layout, and `ImageSpan` placement. A line with no revealable content does not create a sequence. When combined with `Unit::LINE`, each sequence contains one reveal unit representing the entire line.

~~~cpp
reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
reveal.SetSequenceStaggerRatio(0.35f);
label.SetTextReveal(reveal);
~~~

<br/>

### Sequence Stagger Ratio

`SequenceStaggerRatio` controls the spacing between consecutive line-sequence starts in `PER_LINE` mode.

| Value | Behavior |
|---|---|
| `0.0` | All active line sequences start together. This is the default. |
| `0.0 < ratio < 1.0` | A later line starts after a delay and overlaps part of the preceding line's sequence. |
| `1.0` | Consecutive line sequences start without overlapping. |

The value is clamped to the range `0.0` to `1.0`, and NaN is normalized to `0.0`. Because `WHOLE_TEXT` has only one sequence, the authored stagger has no visual effect in that mode.

The stagger ratio represents the **spacing between consecutive line-sequence starts**, not the fade length. For example, with `0.35`, the next line starts after the preceding line has advanced by 35% of the reference progression interval, and both lines progress during the remaining overlapping interval. Smaller values create a longer overlap; larger values delay the next line and shorten the overlap. At `1.0`, the next line starts after the preceding sequence finishes. The actual overlap may vary with the content length of each line.

`Unit = PIXEL`, `SequenceStaggerRatio = 0.0`, `FadeDurationRatio = AUTO_FADE_DURATION_RATIO`

![PIXEL PER_LINE stagger 0.0](./assets/text/per_line_stagger_0.gif)

`Unit = PIXEL`, `SequenceStaggerRatio = 0.25`, `FadeDurationRatio = AUTO_FADE_DURATION_RATIO`

![PIXEL PER_LINE stagger 0.25](./assets/text/per_line_stagger_25.gif)

`Unit = PIXEL`, `SequenceStaggerRatio = 0.5`, `FadeDurationRatio = 0.0`

![PIXEL PER_LINE stagger 0.5, no fade](./assets/text/no_fade.gif)

<br/>

## Animation

A Reveal animation animates only `TextRevealProgress`. The same Reveal configuration can therefore produce very different pacing depending on its duration and alpha function.

~~~cpp
const float duration = 4.0f;

Animation animation = Animation::New(duration);
label.Animate(animation)
  .TextRevealProgress(1.0f,
                      Duration(duration),
                      AlphaFunction::EASE_IN_OUT_SINE);
animation.Play();
~~~

`LINEAR` increases normalized progress uniformly over time. Non-linear alpha functions such as `EASE_IN`, `EASE_OUT`, `EASE_IN_OUT`, and the sine variants keep the same per-unit schedule but change the actual time and speed at which the animation passes through it.

During playback, the application can read the current value to stop or continue in the opposite direction. On scene, `GetTextRevealProgress()` returns the latest current property value including animation updates. Off scene, it returns the last event-side value that was set.

> [!NOTE]
> Text Reveal itself has no play, pause, or duration concept. The application manages the animation handle and playback state.

<br/>

## ImageSpan

An inline `ImageSpan` participates in the same reveal timeline as text.

| Unit | ImageSpan behavior |
|---|---|
| `CHARACTER` | Appears as one atomic item. |
| `WORD` | Appears as one atomic item. |
| `LINE` | Appears with the final line in which the ImageSpan is laid out. |
| `PIXEL` | The width reserved by the ImageSpan participates in progression, and the image is revealed spatially across its interior. |

For the `ImageSpan` source pattern and U+FFFC replacement range, see [ImageSpan in StyledText](https://github.sec.samsung.net/NUI/dali-ui/wiki/StyledText#imagespan).

<br/>

## Styled Foreground

Reveal applies to the final resolved foreground, so it works with `TextGradient` and `GradientSpan` as well as a normal text color. With `GradientSpan`, an application can reveal a sentence that uses a gradient only on selected ranges while the rest remains a solid color.

Decorations such as shadow, outline, underline, strikethrough, and background are not reveal targets.

<br/>

## Layout, Ellipsis And Content Changes

The Reveal schedule is built from the final visible layout rather than the source character count. It accounts for wrapping, maximum-line limits, ellipsis, bidirectional layout, shaping results, and inline replacement placement.

When text, style, size, or layout changes, the schedule is rebuilt for the new final layout, but the current `TextRevealProgress` is not reset. Changing `Unit`, `Sequence`, fade, or stagger configuration also preserves progress. To play new content from the beginning, the application must explicitly set progress to `0.0`.

<br/>

## Samples

| Feature | Sample |
|---|---|
| Unit, sequence, fade, stagger, animation, and styled fill | [text-reveal-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-reveal-example.cpp) |

<br/>

---

[← Label](https://github.sec.samsung.net/NUI/dali-ui/wiki/Label#text-reveal)
