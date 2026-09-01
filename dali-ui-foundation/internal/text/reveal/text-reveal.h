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

#include <dali-ui-foundation/internal/text/replacement/replacement-run-snapshot.h>
#include <dali-ui-foundation/internal/text/text-definitions.h>
#include <dali-ui-foundation/public-api/text/style/reveal.h>

#include <cstdint>
#include <vector>

namespace Dali::TextAbstraction
{
class FontClient;
class Segmentation;
} //namespace Dali::TextAbstraction

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
  WORD,
  PIXEL
};

enum class Sequence : uint8_t
{
  WHOLE_TEXT,
  PER_LINE
};

/**
 * @brief Stores a PIXEL cluster's final visual interpolation data.
 */
struct PixelUnitTiming
{
  float visualMinimum{0.0f};
  float visualMaximum{0.0f};
  float progressionSpan{0.0f};
  bool  rightToLeft{false};
};

/**
 * @brief Stores backend-independent reveal ownership and timing data.
 *
 * glyphToUnit maps each glyph to a logical reveal unit or NO_UNIT. unitStart
 * and fadeDuration use the normalized progress timeline. fadeDurationRatio
 * preserves the authored AUTO sentinel or explicit duration through final
 * projection so the schedule can be resolved from the final visible unit
 * count before metadata is rasterized. imageReplacementUnitMask remains empty
 * for text-only plans. When present, it has one entry per unit and contains at
 * least one eligible visible ImageSpan marker.
 */
struct Plan
{
  std::vector<uint32_t>        glyphToUnit;
  std::vector<float>           unitStart;
  std::vector<PixelUnitTiming> pixelUnitTiming;
  std::vector<uint8_t>         imageReplacementUnitMask;
  float                        fadeDurationRatio{Text::Reveal::AUTO_FADE_DURATION_RATIO};
  float                        fadeDuration{0.0f};

  /**
   * @brief Returns the number of scheduled reveal units.
   *
   * @return The number of entries in the normalized unit schedule.
   */
  uint32_t GetUnitCount() const
  {
    return static_cast<uint32_t>(unitStart.size());
  }

  /**
   * @brief Returns whether this plan contains continuous PIXEL timing.
   */
  bool HasPixelTiming() const
  {
    return !pixelUnitTiming.empty();
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
 * @brief Builds a model plan that retains eligible ImageSpan replacements.
 *
 * Visible image replacements remain in the logical schedule while the
 * rasterizer continues to skip their synthetic glyphs. Other replacement
 * types remain excluded.
 */
Plan BuildPlanWithImageReplacements(const ModelInterface&               model,
                                    Unit                                unit,
                                    float                               fadeDurationRatio,
                                    TextAbstraction::Segmentation&      segmentation,
                                    const ReplacementSourceSnapshot&    source,
                                    const Vector<ReplacementPlacement>& placements);

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
 * @brief Builds the CHARACTER logical skeleton used by PIXEL.
 *
 * The returned source plan contains no spatial descriptors. Those are derived
 * only after final elision and line layout are known.
 */
Plan BuildPixelPlan(const ModelInterface& model, float fadeDurationRatio);

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
 * @param[in] sequenceStaggerRatio The authored sequence stagger ratio.
 * @return True if the final line mapping was valid, including no-op schedules.
 */
bool ApplyPerLineSequenceSchedule(Plan&          plan,
                               const LineRun* lines,
                               Length         lineCount,
                               float          sequenceStaggerRatio);

/**
 * @brief Builds and schedules final PIXEL descriptors from final layout data.
 *
 * This is separate from ApplyPerLineSequenceSchedule() so CHARACTER and WORD keep
 * their existing count-based path unchanged. Scheduling commits atomically;
 * failure leaves the projected CHARACTER-compatible plan unchanged.
 *
 * @param[in,out] plan The final-glyph plan to schedule.
 * @param[in] finalModel The final render model.
 * @param[in] fontClient The font client owned by the current text pipeline.
 * @param[in] finalToSourceGlyph The final-to-source glyph mapping.
 * @param[in] ellipsisFinalGlyph The final ellipsis glyph, or an invalid index.
 * @param[in] sequence The authored reveal sequence.
 * @param[in] sequenceStaggerRatio The authored sequence stagger ratio.
 * @return True if the PIXEL schedule was completed atomically.
 */
bool ApplyPixelSpatialSchedule(Plan&                       plan,
                               const ModelInterface&       finalModel,
                               TextAbstraction::FontClient fontClient,
                               const GlyphIndex*           finalToSourceGlyph,
                               GlyphIndex                  ellipsisFinalGlyph,
                               Sequence                    sequence,
                               float                       sequenceStaggerRatio);

/**
 * @brief Resolves one foreground pixel's normalized PIXEL start timing.
 */
float ResolvePixelStart(const Plan& plan, uint32_t unit, float visualX);

} // namespace Reveal
} // namespace Internal
} // namespace Text
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_INTERNAL_TEXT_REVEAL_H
