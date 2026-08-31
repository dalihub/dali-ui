#ifndef DALI_UI_INTERNAL_TEXT_REVEAL_H
#define DALI_UI_INTERNAL_TEXT_REVEAL_H

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

#include <dali-ui-foundation/internal/text/text-definitions.h>
#include <dali-ui-foundation/public-api/text/style/reveal.h>

#include <cstdint>
#include <vector>

namespace Dali::TextAbstraction
{
class Segmentation;
}

namespace Dali
{
namespace Ui
{
namespace Text
{
class ModelInterface;
struct LineRun;

namespace Internal
{
namespace Reveal
{
constexpr uint32_t NO_UNIT = 0xffffffffu;

enum class Unit : uint8_t
{
  DISABLED,
  CHARACTER,
  WORD
};

enum class Sequence : uint8_t
{
  WHOLE_TEXT,
  PER_LINE
};

/**
 * @brief Stores backend-independent reveal ownership and timing data.
 *
 * glyphToUnit maps each glyph to a logical reveal unit or NO_UNIT. unitStart
 * and fadeDuration use the normalized progress timeline. fadeDurationRatio
 * preserves the authored AUTO sentinel or explicit duration through final
 * projection so the schedule can be resolved from the final visible unit
 * count before metadata is rasterized.
 */
struct Plan
{
  std::vector<uint32_t> glyphToUnit;
  std::vector<float>    unitStart;
  float                 fadeDurationRatio{Text::Reveal::AUTO_FADE_DURATION_RATIO};
  float                 fadeDuration{0.0f};

  /**
   * @brief Returns the number of scheduled reveal units.
   *
   * @return The number of entries in the normalized unit schedule.
   */
  uint32_t GetUnitCount() const
  {
    return static_cast<uint32_t>(unitStart.size());
  }
};

/**
 * @brief Converts a public reveal unit to the internal representation.
 *
 * @param[in] unit The public reveal unit.
 * @return The corresponding internal reveal unit.
 */
Unit ToInternalUnit(Text::Reveal::Unit unit);

/**
 * @brief Converts a public reveal sequence to the internal representation.
 *
 * @param[in] sequence The public reveal sequence.
 * @return The corresponding internal reveal sequence.
 */
Sequence ToInternalSequence(Text::Reveal::Sequence sequence);

/**
 * @brief Builds a source-glyph reveal plan from shaped text arrays.
 *
 * The glyph-to-character entries are shaping-cluster starts. WORD mode uses
 * the caller-provided DALi word-break array; passing nullptr intentionally
 * falls back to whitespace-separated runs and is primarily useful to tests.
 * Whitespace glyphs are assigned NO_UNIT.
 *
 * @param[in] text The UTF-32 logical text buffer.
 * @param[in] characterCount The number of entries in text.
 * @param[in] glyphToCharacter The source glyph-to-character mapping.
 * @param[in] glyphCount The number of entries in glyphToCharacter.
 * @param[in] unit The reveal unit to build.
 * @param[in] fadeDurationRatio The authored automatic sentinel or normalized fade duration.
 * @param[in] wordBreakInfo The optional word-break array with characterCount entries.
 * @return A plan indexed by source glyph.
 */
Plan BuildPlan(const Character*      text,
               Length                characterCount,
               const CharacterIndex* glyphToCharacter,
               Length                glyphCount,
               Unit                  unit,
               float                 fadeDurationRatio,
               const WordBreakInfo*  wordBreakInfo = nullptr);

/**
 * @brief Builds a source-glyph plan from a text model.
 *
 * WORD mode invokes the supplied Segmentation handle. The caller must provide
 * a valid instance owned by the current execution context; this function does
 * not acquire the event-thread SingletonService and does not share the handle
 * between workers. CHARACTER mode does not access segmentation.
 *
 * @param[in] model The shaped text model used as the source sequence.
 * @param[in] unit The reveal unit to build.
 * @param[in] fadeDurationRatio The authored automatic sentinel or normalized fade duration.
 * @param[in] segmentation The caller-owned segmentation instance used for WORD mode.
 * @return A plan indexed by source glyph.
 */
Plan BuildPlan(const ModelInterface&          model,
               Unit                           unit,
               float                          fadeDurationRatio,
               TextAbstraction::Segmentation& segmentation);

/**
 * @brief Builds a CHARACTER source plan without acquiring segmentation.
 *
 * This entry point is used by paths that must guarantee the no-segmentation
 * CHARACTER fast path.
 *
 * @param[in] model The shaped text model used as the source sequence.
 * @param[in] fadeDurationRatio The authored automatic sentinel or normalized fade duration.
 * @return A CHARACTER plan indexed by source glyph.
 */
Plan BuildCharacterPlan(const ModelInterface& model, float fadeDurationRatio);

/**
 * @brief Projects source reveal semantics onto the final rendered glyph sequence.
 *
 * The projection removes elided source units, preserves their logical order,
 * assigns deterministic END ellipsis ownership, and redistributes the
 * remaining schedule over the normalized timeline. A nullptr mapping means
 * that final and source glyph indices are identical. In CHARACTER mode the
 * ellipsis owns the last unit. In WORD mode it joins the last preceding unit,
 * or owns unit zero when no ordinary glyph survives.
 *
 * @param[in] sourcePlan The plan indexed by source glyph.
 * @param[in] finalGlyphCount The number of glyphs in the final rendered sequence.
 * @param[in] finalToSourceGlyph The final-to-source mapping, or nullptr for identity mapping.
 * @param[in] ellipsisFinalGlyph The final ellipsis index, or INVALID_GLYPH_INDEX when absent.
 * @param[in] unit The reveal unit whose ellipsis ownership rule is applied.
 * @return A plan indexed by final rendered glyph.
 */
Plan ProjectToFinalGlyphs(const Plan&       sourcePlan,
                          Length            finalGlyphCount,
                          const GlyphIndex* finalToSourceGlyph,
                          GlyphIndex        ellipsisFinalGlyph,
                          Unit              unit);

/**
 * @brief Applies PER_LINE sequence scheduling to a projected reveal plan.
 *
 * The final LineRun glyph ranges group existing logical units without using
 * visual glyph traversal as reveal order. Units shared by multiple lines are
 * split by (line, unit), and active line starts are evenly spaced. Invalid or
 * incomplete line mappings leave the input plan unchanged.
 *
 * @param[in,out] plan The final-glyph plan to schedule.
 * @param[in] lines The final visual lines.
 * @param[in] lineCount The number of entries in lines.
 * @param[in] sequenceStartDelayRatio The authored normalized start delay.
 * @return True if the final line mapping was valid, including no-op schedules.
 */
bool ApplyPerLineSequenceSchedule(Plan&          plan,
                               const LineRun* lines,
                               Length         lineCount,
                               float          sequenceStartDelayRatio);

} // namespace Reveal
} // namespace Internal
} // namespace Text
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_INTERNAL_TEXT_REVEAL_H
