#ifndef DALI_UI_TEXT_FINAL_GLYPH_GEOMETRY_H
#define DALI_UI_TEXT_FINAL_GLYPH_GEOMETRY_H

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

#include <dali-ui-foundation/internal/text/final-elision-result.h>
#include <dali-ui-foundation/internal/text/line-helper-functions.h>
#include <dali-ui-foundation/internal/text/text-model.h>

namespace Dali::Ui::Text
{
/**
 * @brief Stores final source-glyph geometry in one explicit coordinate conversion.
 */
struct FinalGlyphGeometry
{
  LineIndex lineIndex{FinalElisionResult::INVALID_LINE_INDEX};
  Vector2   contentLocalPenPosition{Vector2::ZERO}; ///< Final position after line and vertical text alignment.
  float     baseline{0.0f};
};

/**
 * @brief Gets the final geometry of a source glyph.
 *
 * @param[in] model The authoritative render model.
 * @param[in] finalElision The final glyph sequence for the model's layout generation.
 * @param[in] sourceGlyphIndex The glyph index in the source visual model.
 * @param[out] geometry The resolved final geometry.
 * @return true if the source glyph is visible in the final layout.
 */
inline bool GetFinalSourceGlyphGeometry(const Model&              model,
                                        const FinalElisionResult& finalElision,
                                        GlyphIndex                sourceGlyphIndex,
                                        FinalGlyphGeometry&       geometry)
{
  geometry = FinalGlyphGeometry{};
  if(!finalElision.resolved || !model.mVisualModel)
  {
    return false;
  }

  const VisualModel& visual = *model.mVisualModel;
  if(sourceGlyphIndex >= visual.mGlyphPositions.Count())
  {
    return false;
  }

  const float spareHeight             = visual.mControlSize.height - visual.GetLayoutSize().height;
  float       verticalAlignmentOffset = 0.0f;
  switch(model.GetVerticalAlignment())
  {
    case Alignment::CENTER:
      verticalAlignmentOffset = 0.5f * spareHeight;
      break;
    case Alignment::END:
      verticalAlignmentOffset = spareHeight;
      break;
    case Alignment::START:
    default:
      break;
  }

  float lineTop = 0.0f;
  for(LineIndex lineIndex = 0u; lineIndex < visual.mLines.Count(); ++lineIndex)
  {
    const LineRun& line     = visual.mLines[lineIndex];
    const auto     contains = [sourceGlyphIndex](const GlyphRun& run)
    {
      return sourceGlyphIndex >= run.glyphIndex && sourceGlyphIndex < run.glyphIndex + run.numberOfGlyphs;
    };
    if(contains(line.glyphRun) || (line.isSplitToTwoHalves && contains(line.glyphRunSecondHalf)))
    {
      geometry.lineIndex = lineIndex;
      break;
    }
    lineTop += GetLineHeight(line, false);
  }
  if(geometry.lineIndex == FinalElisionResult::INVALID_LINE_INDEX)
  {
    return false;
  }

  if(finalElision.textElided)
  {
    GlyphIndex finalGlyphIndex = 0u;
    if(!finalElision.FindFinalGlyphIndex(sourceGlyphIndex, finalGlyphIndex) ||
       finalGlyphIndex >= finalElision.viewGlyphPositions.Count() ||
       finalGlyphIndex >= finalElision.lineLocalGlyphPositions.Count())
    {
      return false;
    }

    const Vector2& lineLocalPosition = finalElision.lineLocalGlyphPositions[finalGlyphIndex];
    geometry.contentLocalPenPosition = finalElision.viewGlyphPositions[finalGlyphIndex];
    geometry.baseline                = geometry.contentLocalPenPosition.y - lineLocalPosition.y;
  }
  else
  {
    const LineRun& line        = visual.mLines[geometry.lineIndex];
    const Vector2& penPosition = visual.mGlyphPositions[sourceGlyphIndex];
    geometry.baseline          = lineTop + line.ascender +
                        GetPreOffsetVerticalLineAlignment(line, model.GetVerticalLineAlignment());
    geometry.contentLocalPenPosition =
      penPosition + Vector2(line.alignmentOffset, geometry.baseline);
  }

  geometry.baseline += verticalAlignmentOffset;
  geometry.contentLocalPenPosition.y += verticalAlignmentOffset;
  return true;
}

} // namespace Dali::Ui::Text

#endif // DALI_UI_TEXT_FINAL_GLYPH_GEOMETRY_H
