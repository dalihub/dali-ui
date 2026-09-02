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
 * @brief Describes how text foreground and inline ImageSpan content are revealed.
 *
 * Reveal distributes progression across visible text and ImageSpan content
 * and controls its transition as TextRevealProgress advances from 0.0 to 1.0.
 * Applications control the playback duration by animating TextRevealProgress.
 *
 * Unit controls how reveal progression is divided across visible content.
 * Sequence controls how those units share the normalized reveal timeline.
 * For example, CHARACTER with WHOLE_TEXT progresses by character across all
 * visible content, while CHARACTER with PER_LINE gives each final visible line
 * its own character progression. LINE with WHOLE_TEXT reveals final visible
 * lines in order. LINE with PER_LINE treats every active line as an independent
 * singleton sequence. PIXEL follows the same whole-text or per-line distinction
 * using spatial progression.
 *
 * By default, content is revealed by character using one whole-text sequence,
 * no sequence stagger, and an automatically selected fade duration. These
 * options can be configured explicitly.
 *
 * Reveal affects the text foreground and inline ImageSpan content. ImageSpan
 * content reveals as an atomic item in CHARACTER, WORD, and LINE modes, and
 * reveals spatially across the rendered image in PIXEL mode.
 * Text decorations and other style layers such as shadow, outline, underline,
 * strikethrough, and background are not affected. When content is elided,
 * hidden source content does not consume reveal units and the ellipsis
 * participates in the visible reveal progression.
 */
class DALI_UI_API Reveal
{
public:
  /**
   * @brief Selects an automatically resolved reveal transition duration.
   *
   * When this value is used as the fade duration ratio, the fade duration is
   * selected automatically for the current rendered reveal progression.
   *
   * This value represents authored automatic behavior; GetFadeDurationRatio()
   * returns this value rather than the internally resolved duration ratio.
   */
  static constexpr float AUTO_FADE_DURATION_RATIO = -1.0f;

  /**
   * @brief Selects how reveal progression is divided across visible content.
   *
   * ImageSpan content participates in the reveal schedule in every mode.
   */
  enum class Unit : uint8_t
  {
    /**
     * @brief Reveals visible text by character while preserving shaping boundaries.
     *
     * Progression follows logical text order. Characters rendered as one
     * indivisible text unit reveal together.
     */
    CHARACTER,

    /**
     * @brief Reveals visible text by word while preserving logical text order.
     *
     * Whitespace does not create a reveal step.
     */
    WORD,

    /**
     * @brief Reveals one final visible layout line as a unit.
     *
     * Lines are determined after shaping, wrapping, maximum-line limiting,
     * ellipsis, bidirectional layout, and ImageSpan placement. A line without
     * revealable visible content does not consume a reveal unit. With
     * WHOLE_TEXT, active lines reveal in final line order. With PER_LINE, each
     * active line is an independent singleton sequence.
     */
    LINE,

    /**
     * @brief Reveals visible text continuously across its rendered foreground.
     *
     * PIXEL follows the same logical text order and shaping boundaries as
     * CHARACTER while distributing reveal timing continuously in pixel space.
     * Inline ImageSpan content contributes its reserved width to the
     * progression and reveals spatially across the rendered image.
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
     * Lines created by wrapping are separate sequences. Lines without
     * revealable content do not create a sequence.
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
   * @brief Sets how reveal progression is divided across visible content.
   *
   * @param[in] unit The reveal unit.
   */
  void SetUnit(Unit unit);

  /**
   * @brief Returns how reveal progression is divided across visible content.
   *
   * @return The reveal unit.
   */
  Unit GetUnit() const;

  /**
   * @brief Sets how reveal units share the normalized reveal timeline.
   *
   * WHOLE_TEXT uses one sequence for all final visible content. PER_LINE uses
   * each final visible layout line, including lines created by wrapping, as an
   * independent sequence. Lines without revealable content do not create a
   * sequence.
   *
   * @param[in] sequence The reveal sequencing mode.
   */
  void SetSequence(Sequence sequence);

  /**
   * @brief Returns how reveal units share the normalized reveal timeline.
   *
   * @return The reveal sequencing mode.
   */
  Sequence GetSequence() const;

  /**
   * @brief Sets the ratio controlling the stagger between consecutive sequence starts.
   *
   * A value of 0.0 starts all active sequences together. Increasing the ratio
   * separates consecutive sequence starts. A value of 1.0 prevents consecutive
   * active sequences from overlapping.
   *
   * PER_LINE creates consecutive active sequences from final visible layout
   * lines. With LINE, those sequences each contain one line unit. WHOLE_TEXT
   * has one sequence, so the stagger has no visual effect, but the authored
   * value is retained. Values outside [0.0, 1.0] are clamped, and NaN is
   * normalized to 0.0.
   *
   * @param[in] ratio The sequence stagger ratio in [0.0, 1.0].
   */
  void SetSequenceStaggerRatio(float ratio);

  /**
   * @brief Returns the authored sequence stagger ratio.
   *
   * @return The authored ratio in [0.0, 1.0].
   */
  float GetSequenceStaggerRatio() const;

  /**
   * @brief Sets the transition duration ratio on the normalized reveal timeline.
   *
   * AUTO_FADE_DURATION_RATIO selects an appropriate duration automatically.
   * Values from 0.0 to 1.0 specify the transition duration. Zero removes the
   * transition fade, while one fades revealable content over the full timeline.
   *
   * Values outside [0.0, 1.0] are clamped, except AUTO_FADE_DURATION_RATIO.
   * NaN is normalized to 0.0.
   *
   * @param[in] ratio AUTO_FADE_DURATION_RATIO or a value in [0.0, 1.0].
   */
  void SetFadeDurationRatio(float ratio);

  /**
   * @brief Returns the authored fade duration ratio.
   *
   * If automatic duration is selected, this returns
   * AUTO_FADE_DURATION_RATIO rather than the duration resolved for the current
   * text layout.
   *
   * @return AUTO_FADE_DURATION_RATIO or the authored ratio in [0.0, 1.0].
   */
  float GetFadeDurationRatio() const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};

} // namespace Text
} // namespace Ui
} // namespace Dali
