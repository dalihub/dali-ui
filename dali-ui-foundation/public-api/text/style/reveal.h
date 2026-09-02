#pragma once

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <dali-ui-foundation/public-api/dali-ui-common.h>

#include <cstdint>

namespace Dali
{
namespace Ui
{
namespace Text
{

/**
 * @brief Describes how visible text and inline ImageSpan content are revealed.
 *
 * Reveal controls how content becomes visible as TextRevealProgress advances
 * from 0.0 to 1.0. Applications control playback duration by animating
 * TextRevealProgress.
 *
 * Unit selects the reveal granularity, while Sequence selects whether reveal
 * units share one timeline across the whole visible content or use an
 * independent timeline for each final visible line.
 *
 * Reveal affects the text foreground and inline ImageSpan content. ImageSpan
 * content reveals as an atomic item in CHARACTER, WORD, and LINE modes, and
 * reveals spatially across the rendered image in PIXEL mode. Text decorations
 * and style layers such as shadow, outline, underline, strikethrough, and
 * background are not affected.
 *
 * When content is elided, hidden source content does not consume reveal units
 * and the visible ellipsis participates in the reveal progression.
 *
 * By default, Reveal uses CHARACTER with WHOLE_TEXT sequencing, no sequence
 * stagger, and an automatically resolved fade duration.
 */
class DALI_UI_API Reveal
{
public:
  /**
   * @brief Selects automatic fade duration.
   *
   * GetFadeDurationRatio() returns this authored value rather than the
   * duration resolved for the current layout.
   */
  static constexpr float AUTO_FADE_DURATION_RATIO = -1.0f;

  /**
   * @brief Selects the granularity of reveal progression.
   */
  enum class Unit : uint8_t
  {
    /**
     * @brief Reveals visible text by character while preserving shaping boundaries.
     *
     * Progression follows logical text order. Characters that form one
     * indivisible shaped unit reveal together.
     */
    CHARACTER,

    /**
     * @brief Reveals visible text by word in logical text order.
     *
     * Whitespace does not create a reveal unit.
     */
    WORD,

    /**
     * @brief Reveals one final visible layout line as a unit.
     *
     * Lines are determined after shaping, wrapping, maximum-line limiting,
     * ellipsis, bidirectional layout, and ImageSpan placement. Lines without
     * revealable visible content do not consume a reveal unit.
     */
    LINE,

    /**
     * @brief Reveals visible content continuously across its rendered foreground.
     *
     * PIXEL follows logical text order and shaping boundaries while distributing
     * reveal timing continuously in pixel space. Inline ImageSpan content
     * contributes its reserved width to the progression and reveals spatially
     * across the rendered image.
     *
     * The progression is normalized and does not correspond one-to-one with
     * physical framebuffer pixels.
     */
    PIXEL
  };

  /**
   * @brief Selects how reveal units share the normalized reveal timeline.
   */
  enum class Sequence : uint8_t
  {
    /**
     * @brief Uses one reveal sequence across all final visible content.
     */
    WHOLE_TEXT,

    /**
     * @brief Uses an independent sequence for each final visible layout line.
     *
     * Lines created by wrapping are separate sequences, while lines without
     * revealable content do not create a sequence. With Unit::LINE, each
     * sequence contains one whole-line reveal unit.
     */
    PER_LINE
  };

public:
  /**
   * @brief Creates a CHARACTER reveal using WHOLE_TEXT sequencing and automatic fade duration.
   */
  Reveal();

  /**
   * @brief Creates a reveal by copying another reveal.
   *
   * @param[in] rhs The reveal to copy.
   */
  Reveal(const Reveal& rhs);

  /**
   * @brief Creates a reveal by moving another reveal.
   *
   * @param[in] rhs The reveal to move.
   */
  Reveal(Reveal&& rhs) noexcept;

  /**
   * @brief Copies another reveal to this object.
   *
   * @param[in] rhs The reveal to copy.
   */
  Reveal& operator=(const Reveal& rhs);

  /**
   * @brief Moves another reveal to this object.
   *
   * @param[in] rhs The reveal to move.
   */
  Reveal& operator=(Reveal&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~Reveal();

  /**
   * @brief Returns the value that disables text reveal.
   *
   * Pass this value to Label::SetTextReveal() to remove reveal rendering.
   *
   * @return A shared immutable none value.
   */
  static const Reveal& None();

  /**
   * @brief Compares this reveal with another reveal.
   *
   * @param[in] rhs The reveal to compare with.
   * @return true if both values are equal.
   */
  bool operator==(const Reveal& rhs) const;

  /**
   * @brief Compares this reveal with another reveal.
   *
   * @param[in] rhs The reveal to compare with.
   * @return true if both values are not equal.
   */
  bool operator!=(const Reveal& rhs) const;

public:
  /**
   * @brief Sets the reveal unit.
   *
   * @param[in] unit The reveal unit.
   */
  void SetUnit(Unit unit);

  /**
   * @brief Gets the reveal unit.
   *
   * @return The reveal unit.
   */
  Unit GetUnit() const;

  /**
   * @brief Sets the reveal sequencing mode.
   *
   * @param[in] sequence The reveal sequencing mode.
   */
  void SetSequence(Sequence sequence);

  /**
   * @brief Gets the reveal sequencing mode.
   *
   * @return The reveal sequencing mode.
   */
  Sequence GetSequence() const;

  /**
   * @brief Sets the fade duration on the normalized reveal timeline.
   *
   * AUTO_FADE_DURATION_RATIO selects the duration automatically. Values from
   * 0.0 to 1.0 specify the normalized fade duration; zero disables the fade.
   *
   * Values outside [0.0, 1.0] are clamped, except AUTO_FADE_DURATION_RATIO.
   * NaN is normalized to 0.0.
   *
   * @param[in] ratio AUTO_FADE_DURATION_RATIO or a value in [0.0, 1.0].
   */
  void SetFadeDurationRatio(float ratio);

  /**
   * @brief Gets the authored fade duration ratio.
   *
   * When automatic duration is selected, this returns
   * AUTO_FADE_DURATION_RATIO rather than the resolved duration.
   *
   * @return AUTO_FADE_DURATION_RATIO or the authored ratio in [0.0, 1.0].
   */
  float GetFadeDurationRatio() const;

  /**
   * @brief Sets the stagger between consecutive reveal sequences.
   *
   * The ratio controls the spacing between consecutive active sequence starts.
   * A value of 0.0 starts all active sequences together, while 1.0 prevents
   * consecutive sequences from overlapping.
   *
   * Stagger affects PER_LINE sequencing. WHOLE_TEXT uses a single sequence, so
   * the authored value has no visual effect. With Unit::LINE, stagger controls
   * the start spacing between whole-line transitions independently of the fade
   * duration.
   *
   * Values outside [0.0, 1.0] are clamped, and NaN is normalized to 0.0.
   *
   * @param[in] ratio The sequence stagger ratio in [0.0, 1.0].
   */
  void SetSequenceStaggerRatio(float ratio);

  /**
   * @brief Gets the authored sequence stagger ratio.
   *
   * @return The authored ratio in [0.0, 1.0].
   */
  float GetSequenceStaggerRatio() const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};

} // namespace Text
} // namespace Ui
} // namespace Dali
