#ifndef DALI_UI_TEXT_FINAL_ELISION_RESULT_H
#define DALI_UI_TEXT_FINAL_ELISION_RESULT_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/math/vector2.h>
#include <cstdint>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/line-run.h>
#include <dali-ui-foundation/internal/text/text-definitions.h>

namespace Dali::Ui::Text
{
/**
 * @brief Stores the final glyph and elision state for one layout generation.
 */
struct FinalElisionResult
{
  enum class EllipsisOmissionReason : uint8_t
  {
    NONE,
    NO_VISIBLE_LINE,
    ELLIPSIS_CANNOT_FIT
  };

  static constexpr GlyphIndex INVALID_GLYPH_INDEX = std::numeric_limits<GlyphIndex>::max();
  static constexpr LineIndex  INVALID_LINE_INDEX  = std::numeric_limits<LineIndex>::max();

  FinalElisionResult()  = default;
  ~FinalElisionResult() = default;

  /**
   * @brief Clears the resolved glyph sequence and generation state.
   */
  void Clear()
  {
    glyphs.Clear();
    viewGlyphPositions.Clear();
    lineLocalGlyphPositions.Clear();
    lines.Clear();
    sourceToFinalGlyphIndices.Clear();
    finalToSourceGlyphIndices.Clear();
    finalToStyleGlyphIndices.Clear();
    colorIndices.Clear();
    backgroundColorIndices.Clear();
    layoutSize              = Size::ZERO;
    minimumLineOffset       = 0.0f;
    elidedOffset            = 0.0f;
    startIndex              = 0u;
    endIndex                = 0u;
    firstMiddleIndex        = 0u;
    secondMiddleIndex       = 0u;
    ellipsisFinalGlyphIndex = INVALID_GLYPH_INDEX;
    ellipsisLineIndex       = INVALID_LINE_INDEX;
    ellipsisUnitCount       = 0u;
    ellipsisOmissionReason  = EllipsisOmissionReason::NONE;
    layoutGeneration        = 0u;
    authoritativeLines      = false;
    resolved                = false;
    textElided              = false;
    applied                 = false;
  }

  /**
   * @brief Checks whether an original glyph is visible in the final sequence.
   *
   * @param[in] glyphIndex The glyph index in the source visual model.
   * @return true if the source glyph remains visible.
   */
  bool IsOriginalGlyphVisible(GlyphIndex glyphIndex) const
  {
    return resolved && (!textElided ||
                        (glyphIndex < sourceToFinalGlyphIndices.Count() &&
                         sourceToFinalGlyphIndices[glyphIndex] != INVALID_GLYPH_INDEX));
  }

  /**
   * @brief Finds the final glyph corresponding to an original glyph.
   *
   * @param[in] sourceGlyphIndex The glyph index in the source visual model.
   * @param[out] finalGlyphIndex The corresponding index in the final sequence.
   * @return true if the source glyph remains in the final sequence.
   */
  bool FindFinalGlyphIndex(GlyphIndex  sourceGlyphIndex,
                           GlyphIndex& finalGlyphIndex) const
  {
    if(!resolved || !textElided || sourceGlyphIndex >= sourceToFinalGlyphIndices.Count())
    {
      return false;
    }

    finalGlyphIndex = sourceToFinalGlyphIndices[sourceGlyphIndex];
    if(finalGlyphIndex == INVALID_GLYPH_INDEX)
    {
      return false;
    }
    return finalGlyphIndex < viewGlyphPositions.Count();
  }

  /**
   * @brief Gets the final View position of an original glyph.
   *
   * @param[in] sourceGlyphIndex The glyph index in the source visual model.
   * @param[out] finalPosition The final position after line alignment and baseline offsets.
   * @return true if the source glyph remains in the final sequence.
   */
  bool GetFinalGlyphPosition(GlyphIndex sourceGlyphIndex,
                             Vector2&   finalPosition) const
  {
    GlyphIndex finalGlyphIndex = 0u;
    if(!FindFinalGlyphIndex(sourceGlyphIndex, finalGlyphIndex))
    {
      return false;
    }

    finalPosition = viewGlyphPositions[finalGlyphIndex];
    return true;
  }

  /** @return true when this object owns complete final-layout geometry. */
  bool HasAuthoritativeLayout() const
  {
    return resolved && textElided && authoritativeLines;
  }

  Vector<GlyphInfo>      glyphs;
  Vector<Vector2>        viewGlyphPositions;        ///< Final positions in View/atlas coordinates.
  Vector<Vector2>        lineLocalGlyphPositions;   ///< Same sequence in Typesetter's line-local coordinates.
  Vector<LineRun>        lines;                     ///< Lines whose glyph runs index the final glyph sequence.
  Vector<GlyphIndex>     sourceToFinalGlyphIndices; ///< Source glyph to a representative final glyph in the same authored cluster, or invalid when elided.
  Vector<GlyphIndex>     finalToSourceGlyphIndices; ///< Final glyph to a representative source glyph in the same authored cluster; generated glyphs are invalid.
  Vector<GlyphIndex>     finalToStyleGlyphIndices;  ///< Final glyph to the source glyph used for authored style-run lookup.
  Vector<ColorIndex>     colorIndices;              ///< Color indices in the final glyph domain.
  Vector<ColorIndex>     backgroundColorIndices;    ///< Background-color indices in the final glyph domain.
  Size                   layoutSize{Size::ZERO};    ///< Layout size derived only from final-domain lines.
  float                  minimumLineOffset{0.0f};
  float                  elidedOffset{0.0f};
  GlyphIndex             startIndex{0u};
  GlyphIndex             endIndex{0u};
  GlyphIndex             firstMiddleIndex{0u};
  GlyphIndex             secondMiddleIndex{0u};
  GlyphIndex             ellipsisFinalGlyphIndex{INVALID_GLYPH_INDEX};
  LineIndex              ellipsisLineIndex{INVALID_LINE_INDEX};
  Length                 ellipsisUnitCount{0u};
  EllipsisOmissionReason ellipsisOmissionReason{EllipsisOmissionReason::NONE};

  uint64_t layoutGeneration{0u};
  bool     authoritativeLines{false}; ///< END lines, including an intentionally empty set, use the final domain. Replacement projection leaves this false.
  bool     resolved{false};
  bool     textElided{false}; ///< LayoutEngine selected an elision boundary.
  bool     applied{false};    ///< Exactly one ellipsis unit exists in the final sequence.
};

} // namespace Dali::Ui::Text

#endif // DALI_UI_TEXT_FINAL_ELISION_RESULT_H
