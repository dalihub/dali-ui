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

// CLASS HEADER
#include <dali-ui-foundation/internal/text/marquee/marquee-start-geometry.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/final-elision-result.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-run-snapshot.h>
#include <dali-ui-foundation/internal/text/text-model-interface.h>
#include <dali-ui-foundation/internal/text/visual-model-impl.h>

namespace Dali::Ui::Text
{
MarqueeStartAnchor ResolveMarqueeStartAnchor(const FinalElisionResult* finalElision,
                                             const VisualModel*        sourceModel)
{
  constexpr float RIGID_TRANSLATION_EPSILON = 0.01f;

  if(!finalElision || !sourceModel || !finalElision->HasAuthoritativeLayout() ||
     !finalElision->applied || finalElision->ellipsisUnitCount != 1u ||
     finalElision->lines.Count() != 1u)
  {
    return {};
  }

  const Length finalGlyphCount  = static_cast<Length>(finalElision->glyphs.Count());
  const Length sourceGlyphCount = static_cast<Length>(sourceModel->mGlyphs.Count());
  if(finalGlyphCount == 0u || sourceGlyphCount == 0u ||
     sourceModel->mGlyphPositions.Count() != sourceGlyphCount ||
     finalElision->ellipsisFinalGlyphIndex >= finalGlyphCount ||
     finalElision->viewGlyphPositions.Count() != finalGlyphCount ||
     finalElision->finalToSourceGlyphIndices.Count() != finalGlyphCount ||
     finalElision->sourceToFinalGlyphIndices.Count() != sourceGlyphCount)
  {
    return {};
  }

  const GlyphInfo* const sourceGlyphs    = sourceModel->mGlyphs.Begin();
  const Vector2* const   sourcePositions = sourceModel->mGlyphPositions.Begin();
  if(!sourceGlyphs || !sourcePositions)
  {
    return {};
  }

  float                 minimumOffset = std::numeric_limits<float>::max();
  float                 maximumOffset = std::numeric_limits<float>::lowest();
  Length                sampleCount   = 0u;
  MarqueeStartAnchor    anchor;
  const CharacterIndex* sourceGlyphsToCharacters = sourceModel->mGlyphsToCharacters.Begin();

  if(sourceModel->mGlyphsToCharacters.Count() != sourceGlyphCount || !sourceGlyphsToCharacters)
  {
    return {};
  }

  for(GlyphIndex finalGlyphIndex = 0u; finalGlyphIndex < finalGlyphCount; ++finalGlyphIndex)
  {
    const GlyphIndex sourceGlyphIndex = finalElision->finalToSourceGlyphIndices[finalGlyphIndex];
    if(finalGlyphIndex == finalElision->ellipsisFinalGlyphIndex)
    {
      if(sourceGlyphIndex != FinalElisionResult::INVALID_GLYPH_INDEX)
      {
        return {};
      }
      continue;
    }

    if(sourceGlyphIndex == FinalElisionResult::INVALID_GLYPH_INDEX ||
       sourceGlyphIndex >= sourceGlyphCount)
    {
      return {};
    }

    const GlyphIndex representativeFinal = finalElision->sourceToFinalGlyphIndices[sourceGlyphIndex];
    if(representativeFinal == FinalElisionResult::INVALID_GLYPH_INDEX ||
       representativeFinal >= finalGlyphCount)
    {
      return {};
    }

    const GlyphInfo& finalGlyph  = finalElision->glyphs[finalGlyphIndex];
    const GlyphInfo& sourceGlyph = sourceGlyphs[sourceGlyphIndex];
    if(IsSyntheticReplacementGlyph(finalGlyph) || IsSyntheticReplacementGlyph(sourceGlyph))
    {
      return {};
    }

    // Direction controls and other zero-area items cannot provide a visual anchor.
    if(sourceGlyph.width <= RIGID_TRANSLATION_EPSILON ||
       sourceGlyph.height <= RIGID_TRANSLATION_EPSILON)
    {
      continue;
    }

    if(finalGlyph.fontId != sourceGlyph.fontId || finalGlyph.index != sourceGlyph.index)
    {
      return {};
    }

    const float staticPosition = finalElision->viewGlyphPositions[finalGlyphIndex].x;
    const float sourcePosition = sourcePositions[sourceGlyphIndex].x;
    if(!std::isfinite(staticPosition) || !std::isfinite(sourcePosition))
    {
      return {};
    }

    const float staticTranslation = staticPosition - sourcePosition;
    minimumOffset                 = std::min(minimumOffset, staticTranslation);
    maximumOffset                 = std::max(maximumOffset, staticTranslation);

    if(!anchor.valid)
    {
      const CharacterIndex characterIndex  = sourceGlyphsToCharacters[sourceGlyphIndex];
      Length               glyphOccurrence = 0u;
      for(GlyphIndex candidate = 0u; candidate < sourceGlyphIndex; ++candidate)
      {
        if(sourceGlyphsToCharacters[candidate] == characterIndex)
        {
          ++glyphOccurrence;
        }
      }

      anchor.characterIndex  = characterIndex;
      anchor.glyphOccurrence = glyphOccurrence;
      anchor.staticControlX  = staticPosition;
      anchor.valid           = true;
    }
    ++sampleCount;
  }

  // A single glyph is insufficient to prove that all retained runs translate rigidly.
  if(sampleCount < 2u || maximumOffset - minimumOffset > RIGID_TRANSLATION_EPSILON)
  {
    return {};
  }

  return anchor;
}

MarqueeFittingStartGeometry ResolveMarqueeFittingStartGeometry(const ModelInterface* model)
{
  if(!model || model->GetNumberOfLines() != 1u || model->GetNumberOfGlyphs() == 0u)
  {
    return {};
  }

  const LineRun* const lines = model->GetLines();
  if(!lines || lines[0].glyphRun.numberOfGlyphs == 0u || lines[0].ellipsis ||
     !std::isfinite(lines[0].alignmentOffset))
  {
    return {};
  }

  // Keep this conversion identical to Typesetter's non-ellipsis static path.
  const float staticTranslation = static_cast<float>(static_cast<int32_t>(lines[0].alignmentOffset));
  return MarqueeFittingStartGeometry{staticTranslation, true};
}

} // namespace Dali::Ui::Text
