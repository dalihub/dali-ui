# Text Reveal

[→ English](https://github.sec.samsung.net/NUI/dali-ui/wiki/Text-Reveal)

## Overview

Text Reveal은 `Label`의 최종 layout에 표시되는 text foreground와 inline `ImageSpan`을 normalized progress에 따라 순서대로 보여주는 기능입니다. `Text::Reveal`이 reveal unit, sequence, fade를 정의하고, 애플리케이션은 `TextRevealProgress`를 직접 설정하거나 animate하여 재생 시간과 방향을 제어합니다.

- Progress `0.0`: 모든 reveal 대상이 숨겨집니다.
- Progress `1.0`: 모든 reveal 대상이 완전히 표시됩니다.
- Progress를 감소시키면 같은 schedule을 반대로 따라가며 content가 숨겨집니다.

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

`TextRevealProgress`는 `0.0`부터 `1.0` 사이로 clamp되며 NaN은 `0.0`으로 처리됩니다. 초기값은 `0.0`입니다. `Text::Reveal`은 재생 시간을 갖지 않으며, 전체 duration과 alpha function은 위 예시처럼 애플리케이션이 animation에 지정합니다.

완전히 표시된 content를 다시 숨기려면 target progress를 `0.0`으로 animate합니다.

~~~cpp
Animation reverse = Animation::New(1.5f);
label.Animate(reverse)
  .TextRevealProgress(0.0f, Duration(1.5f), AlphaFunction::EASE_IN_OUT);
reverse.Play();
~~~

Reveal rendering을 제거하려면 `Text::Reveal::None()`을 설정합니다.

~~~cpp
label.SetTextReveal(Text::Reveal::None());
~~~

Reveal이 비활성화되면 content는 현재 progress와 관계없이 완전히 표시됩니다. Progress는 유지되므로 Reveal을 다시 설정하면 기존 progress가 적용됩니다.

<br/>

## Unit

`Unit`은 reveal schedule을 구성하는 단위를 선택합니다.

| Unit | 동작 |
|---|---|
| `CHARACTER` | 기본값입니다. Character 단위로 진행하되 shaping 결과에서 분리할 수 없는 ligature, combining sequence, emoji cluster 등은 함께 표시합니다. |
| `WORD` | 단어 경계에 따라 표시합니다. Whitespace는 별도 unit을 소비하지 않으며 punctuation은 가능한 경우 인접한 단어와 함께 표시됩니다. |
| `LINE` | Wrapping과 ellipsis 처리가 끝난 최종 visible layout line 전체를 하나의 unit으로 표시합니다. |
| `PIXEL` | Visible foreground 내부를 pixel 단위로 연속해서 표시합니다. |

`CHARACTER`, `WORD`, `PIXEL`의 진행 순서는 logical text order와 shaping boundary를 따릅니다. 따라서 bidirectional text에서도 무조건 화면의 왼쪽에서 오른쪽으로 진행하는 효과가 아닙니다.

`PIXEL`은 glyph와 inline image가 차지하는 폭에 비례해 reveal timing을 배분합니다. 폭이 넓은 요소에는 더 긴 progress 구간이, 좁은 요소에는 더 짧은 progress 구간이 배정됩니다. `WHOLE_TEXT`에서는 전체 visible content를 하나의 연속된 거리로 계산하고, `PER_LINE`에서는 각 line을 독립된 거리로 계산합니다. 따라서 `LINEAR` animation에서는 폭이 서로 다른 glyph와 image가 섞여 있어도 reveal이 대체로 일정한 속도로 이동합니다.

~~~cpp
reveal.SetUnit(Text::Reveal::Unit::PIXEL);
label.SetTextReveal(reveal);
~~~

<!-- GIF 파일 수신 후 추가: ![Text Reveal Unit 비교](./assets/text/text_reveal_units.gif) -->

<br/>

## Fade Duration Ratio

`FadeDurationRatio`는 unit 사이의 delay가 아니라 normalized reveal timeline에서 **각 unit이 fade하는 구간의 길이**입니다. 명시적인 값은 `0.0`부터 `1.0` 사이이며, 기본값인 `AUTO_FADE_DURATION_RATIO`를 사용하면 최종 visible content에 맞춰 자동으로 결정됩니다.

`CHARACTER`, `WORD`, `LINE`처럼 각 unit이 같은 비중을 가지는 하나의 sequence에 reveal unit이 `N`개 있고 fade ratio가 `F`일 때 기본 schedule은 다음과 같습니다.

~~~text
N > 1:
  startInterval = (1 - F) / (N - 1)
  start(i)      = i * startInterval,  i = 0, ..., N - 1

N = 1:
  start(0) = 0
~~~

`F > 0`일 때 unit `i`의 opacity는 다음과 같이 계산됩니다.

~~~text
opacity(i, progress) = clamp((progress - start(i)) / F, 0, 1)
~~~

- `F = 0`: fade 없이 각 unit이 순서대로 나타나는 step/typewriter 방식입니다. Progress `0.0`에서는 첫 unit을 포함해 모두 숨겨집니다.

  ![FadeDurationRatio 0](./assets/text/fade_ratio_0.gif)

- `F = 1`: 단일 sequence에서는 모든 unit의 start가 `0`이 되어 전체 content가 함께 fade됩니다.

  ![FadeDurationRatio 1](./assets/text/fade_ratio_1.gif)

- `0 < F < 1`: 앞 unit의 fade가 끝나기 전에 다음 unit이 시작될 수 있습니다. 같은 unit 수에서는 값이 클수록 겹치는 구간이 길어집니다.

  ![FadeDurationRatio 0.5](./assets/text/fade_ratio_0.5.gif)

`WHOLE_TEXT`에서는 위 schedule이 전체 content에 적용됩니다. `PER_LINE`에서는 line별 schedule을 만든 뒤 stagger를 포함한 전체 timeline으로 다시 normalize합니다. `PIXEL`은 unit 수를 균등 분할하지 않고 glyph와 inline image의 실제 진행 공간에 가중치를 주므로 위의 단순한 `startInterval` 식을 그대로 사용하지 않습니다.

Progress를 `0.0`에서 `1.0`까지 전체 시간 `T` 동안 `LINEAR`로 animate하는 단일 sequence라면 각 unit의 실제 fade 시간은 `F * T`, unit 시작 간격은 `startInterval * T`입니다. 예를 들어 `N = 5`, `F = 0.25`, `T = 4초`이면 각 unit은 `1초` 동안 fade하고 다음 unit은 `0.75초` 간격으로 시작합니다.

명시적인 ratio는 범위를 벗어나면 clamp되고 NaN은 `0.0`으로 처리됩니다. `AUTO_FADE_DURATION_RATIO`는 예외적인 sentinel 값으로 유지되며, `GetFadeDurationRatio()`도 내부에서 계산된 값 대신 이 authored 값을 반환합니다.

<br/>

### Automatic Fade

`AUTO_FADE_DURATION_RATIO`는 최종 visible unit 수에 따라 fade duration을 자동으로 정합니다. 짧은 content에서는 순차적인 느낌을 유지하고, 긴 content에서는 unit별 fade가 지나치게 짧아지지 않도록 overlap을 조정합니다. `PIXEL`에서는 visible 공간 범위와 text scale을 반영합니다.

자동 계산은 원본 source 길이가 아니라 최종 rendering 결과를 기준으로 합니다.

- Whitespace와 elide되어 보이지 않는 원본 content는 reveal unit을 소비하지 않습니다.
- 화면에 표시되는 ellipsis glyph는 reveal progression에 포함됩니다.
- Inline `ImageSpan`은 visible replacement로 포함됩니다.

<br/>

## Sequence

`Sequence`는 multi-line content가 하나의 timeline을 공유할지, 각 line이 독립적인 timeline을 가질지 결정합니다.

| Sequence | 동작 |
|---|---|
| `WHOLE_TEXT` | 기본값입니다. 모든 visible content가 하나의 reveal sequence를 공유합니다. |
| `PER_LINE` | 최종 visible line마다 독립적인 sequence를 만듭니다. |

`PER_LINE`에서 line은 shaping, wrapping, maximum line 제한, ellipsis, bidirectional layout, `ImageSpan` 배치가 끝난 결과를 기준으로 합니다. Reveal할 content가 없는 line은 sequence를 만들지 않습니다. `Unit::LINE`과 함께 사용하면 각 sequence가 line 전체 하나를 reveal unit으로 가집니다.

~~~cpp
reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
reveal.SetSequenceStaggerRatio(0.35f);
label.SetTextReveal(reveal);
~~~

<br/>

### Sequence Stagger Ratio

`SequenceStaggerRatio`는 `PER_LINE`에서 연속된 line sequence의 시작 간격을 제어합니다.

| 값 | 동작 |
|---|---|
| `0.0` | 모든 active line sequence가 함께 시작합니다. 기본값입니다. |
| `0.0 < ratio < 1.0` | 뒤 line이 지연되어 시작하며 앞 line과 일부 구간이 겹칩니다. |
| `1.0` | 연속된 line sequence가 서로 겹치지 않도록 시작합니다. |

값은 `0.0`부터 `1.0` 사이로 clamp되며 NaN은 `0.0`으로 처리됩니다. `WHOLE_TEXT`는 sequence가 하나뿐이므로 설정한 stagger 값이 시각적으로 영향을 주지 않습니다.

Stagger ratio는 fade 길이가 아니라 연속된 line sequence의 **시작점 사이 간격**을 의미합니다. 예를 들어 `0.35`이면 앞 line sequence가 기준 진행 구간의 35%만큼 진행한 시점에 다음 line이 시작하여, 이후 구간은 두 line이 함께 진행됩니다. 값이 작으면 겹치는 구간이 길고, 값이 커질수록 다음 line이 늦게 시작해 겹치는 구간이 짧아집니다. `1.0`에서는 앞 line sequence가 끝난 뒤 다음 line sequence가 시작합니다. 실제로 겹치는 길이는 각 line의 content 길이에 따라 달라질 수 있습니다.

`Unit = PIXEL`, `SequenceStaggerRatio = 0.0`, `FadeDurationRatio = AUTO_FADE_DURATION_RATIO`

![PIXEL PER_LINE stagger 0.0](./assets/text/per_line_stagger_0.gif)

`Unit = PIXEL`, `SequenceStaggerRatio = 0.25`, `FadeDurationRatio = AUTO_FADE_DURATION_RATIO`

![PIXEL PER_LINE stagger 0.25](./assets/text/per_line_stagger_25.gif)

`Unit = PIXEL`, `SequenceStaggerRatio = 0.5`, `FadeDurationRatio = 0.0`

![PIXEL PER_LINE stagger 0.5, no fade](./assets/text/no_fade.gif)

<br/>

## Animation

Reveal animation은 `TextRevealProgress` 하나를 animate합니다. 따라서 같은 Reveal configuration이라도 duration과 alpha function에 따라 전혀 다른 속도감을 만들 수 있습니다.

~~~cpp
const float duration = 4.0f;

Animation animation = Animation::New(duration);
label.Animate(animation)
  .TextRevealProgress(1.0f,
                      Duration(duration),
                      AlphaFunction::EASE_IN_OUT_SINE);
animation.Play();
~~~

`LINEAR`는 normalized progress가 시간에 따라 일정하게 증가합니다. `EASE_IN`, `EASE_OUT`, `EASE_IN_OUT`, sine 계열 같은 non-linear alpha function을 사용하면 각 unit의 schedule은 같지만 그 schedule을 통과하는 실제 시간과 속도가 달라집니다.

재생 도중 현재 값을 읽어 정지하거나 반대 방향으로 이어서 animate할 수 있습니다. On-scene `GetTextRevealProgress()`는 animation이 반영된 최신 current property를 반환하고, off-scene에서는 마지막으로 설정한 event-side 값을 반환합니다.

> [!NOTE]
> Text Reveal 자체에는 play, pause, duration 개념이 없습니다. Animation handle과 재생 상태는 애플리케이션이 관리해야 합니다.

<br/>

## ImageSpan

Inline `ImageSpan`도 text와 같은 reveal timeline에 참여합니다.

| Unit | ImageSpan 동작 |
|---|---|
| `CHARACTER` | 하나의 atomic item으로 표시됩니다. |
| `WORD` | 하나의 atomic item으로 표시됩니다. |
| `LINE` | ImageSpan이 배치된 최종 line과 함께 표시됩니다. |
| `PIXEL` | ImageSpan이 확보한 폭이 progression에 포함되며 image 내부가 공간적으로 표시됩니다. |

`ImageSpan`의 source 작성법과 U+FFFC replacement range는 [StyledText의 ImageSpan](https://github.sec.samsung.net/NUI/dali-ui/wiki/StyledText-(kr)#imagespan)을 참고하세요.

<br/>

## Styled Foreground

Reveal은 최종 resolved foreground에 적용되므로 일반 text color뿐 아니라 `TextGradient`와 `GradientSpan`이 적용된 text에도 사용할 수 있습니다. `GradientSpan`을 사용하면 일반 색상 문장 안의 특정 range에만 gradient를 적용한 상태로 reveal할 수 있습니다.

Shadow, outline, underline, line-through, background 같은 decoration은 reveal 대상이 아닙니다.

<br/>

## Layout, Ellipsis And Content Changes

Reveal schedule은 source의 단순 문자 수가 아니라 최종 visible layout을 기준으로 만들어집니다. Wrapping, maximum line 제한, ellipsis, bidirectional layout, shaping 결과와 inline replacement 배치가 schedule에 반영됩니다.

Text, style, size 또는 layout이 바뀌면 새 최종 layout에 맞춰 schedule이 다시 계산되지만 현재 `TextRevealProgress`는 초기화되지 않습니다. `Unit`, `Sequence`, fade 또는 stagger configuration을 변경해도 progress는 그대로 유지됩니다. 새 content를 처음부터 재생하려면 애플리케이션에서 progress를 명시적으로 `0.0`으로 설정해야 합니다.

<br/>

## Samples

| 기능 | 샘플 |
|---|---|
| Unit, sequence, fade, stagger, animation 및 styled fill | [text-reveal-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-reveal-example.cpp) |

<br/>

---

[← Label](https://github.sec.samsung.net/NUI/dali-ui/wiki/Label-(kr)#text-reveal)
