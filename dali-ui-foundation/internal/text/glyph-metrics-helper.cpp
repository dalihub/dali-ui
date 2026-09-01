
/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

// FILE HEADER
#include <dali-ui-foundation/internal/text/glyph-metrics-helper.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/rendering/styles/character-spacing-helper-functions.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-run-snapshot.h>
#include <dali-ui-foundation/internal/text/text-model-interface.h>

#include <algorithm>
#include <cmath>
#include <vector>

namespace Dali
{
namespace Ui
{
namespace Text
{
Length GetNumberOfGlyphsOfGroup(GlyphIndex glyphIndex, GlyphIndex lastGlyphPlusOne,
                                const Length* const charactersPerGlyphBuffer)
{
  Length numberOfGLyphsInGroup = 1u;

  for(GlyphIndex index = glyphIndex; index < lastGlyphPlusOne; ++index)
  {
    if(0u == *(charactersPerGlyphBuffer + index))
    {
      ++numberOfGLyphsInGroup;
    }
    else
    {
      break;
    }
  }

  return numberOfGLyphsInGroup;
}

void GetGlyphsMetrics(GlyphIndex glyphIndex, Length numberOfGlyphs, GlyphMetrics& glyphMetrics,
                      const GlyphInfo* const glyphsBuffer, MetricsPtr& metrics, float calculatedAdvance)
{
  const GlyphInfo& firstGlyph = *(glyphsBuffer + glyphIndex);

  Text::FontMetrics fontMetrics;
  if(0u != firstGlyph.fontId)
  {
    metrics->GetFontMetrics(firstGlyph.fontId, fontMetrics);
  }
  else if(0u != firstGlyph.index)
  {
    // It may be an embedded image.
    fontMetrics.ascender  = firstGlyph.height;
    fontMetrics.descender = 0.f;
    fontMetrics.height    = fontMetrics.ascender;
  }

  const bool isItalicFont = metrics->HasItalicStyle(firstGlyph.fontId);

  glyphMetrics.fontId     = firstGlyph.fontId;
  glyphMetrics.fontHeight = fontMetrics.height;
  glyphMetrics.width      = firstGlyph.width;
  glyphMetrics.advance    = calculatedAdvance;
  glyphMetrics.ascender   = fontMetrics.ascender;
  glyphMetrics.xBearing   = firstGlyph.xBearing;

  if(1u < numberOfGlyphs)
  {
    float maxWidthEdge = firstGlyph.xBearing + firstGlyph.width;

    for(unsigned int i = 1u; i < numberOfGlyphs; ++i)
    {
      const GlyphInfo& glyphInfo = *(glyphsBuffer + glyphIndex + i);

      // update the initial xBearing if smaller.
      glyphMetrics.xBearing = std::min(glyphMetrics.xBearing, glyphMetrics.advance + glyphInfo.xBearing);

      // update the max width edge if bigger.
      const float currentMaxGlyphWidthEdge = glyphMetrics.advance + glyphInfo.xBearing + glyphInfo.width;
      maxWidthEdge                         = std::max(maxWidthEdge, currentMaxGlyphWidthEdge);

      glyphMetrics.advance += (glyphInfo.advance);
    }

    glyphMetrics.width = maxWidthEdge - glyphMetrics.xBearing;
  }

  glyphMetrics.width += (firstGlyph.isItalicRequired && !isItalicFont)
                          ? TextAbstraction::FontClient::DEFAULT_ITALIC_ANGLE * firstGlyph.height
                          : 0.f;
}

void GetGlyphMetricsFromCharacterIndex(CharacterIndex index, const VisualModelPtr& visualModel,
                                       const LogicalModelPtr& logicalModel, MetricsPtr& metrics,
                                       GlyphMetrics& glyphMetrics, GlyphIndex& glyphIndex, Length& numberOfGlyphs)
{
  const GlyphIndex* const charactersToGlyphBuffer   = visualModel->mCharactersToGlyph.Begin();
  const Length* const     glyphsPerCharacterBuffer  = visualModel->mGlyphsPerCharacter.Begin();
  const GlyphInfo* const  glyphInfoBuffer           = visualModel->mGlyphs.Begin();
  Vector<CharacterIndex>& glyphToCharacterMap       = visualModel->mGlyphsToCharacters;
  const CharacterIndex*   glyphToCharacterMapBuffer = glyphToCharacterMap.Begin();
  const float             modelCharacterSpacing     = visualModel->GetCharacterSpacing();

  // Get the character-spacing runs.
  const Vector<CharacterSpacingGlyphRun>& characterSpacingGlyphRuns = visualModel->GetCharacterSpacingGlyphRuns();

  // Takes the character index, obtains the glyph index (and the number of Glyphs) from it and finally gets the glyph
  // metrics.
  glyphIndex     = *(charactersToGlyphBuffer + index);
  numberOfGlyphs = *(glyphsPerCharacterBuffer + index);

  float calculatedAdvance = 0.f;

  const float characterSpacing = GetGlyphCharacterSpacing(glyphIndex, characterSpacingGlyphRuns, modelCharacterSpacing);
  calculatedAdvance            = GetCalculatedAdvance(*(logicalModel->mText.Begin() + (*(glyphToCharacterMapBuffer + glyphIndex))),
                                                      characterSpacing, (*(visualModel->mGlyphs.Begin() + glyphIndex)).advance);

  // Get the metrics for the group of glyphs.
  GetGlyphsMetrics(glyphIndex, numberOfGlyphs, glyphMetrics, glyphInfoBuffer, metrics, calculatedAdvance);
}

float GetCalculatedAdvance(unsigned int character, float characterSpacing, float advance)
{
  // Gets the final advance value by adding the CharacterSpacing value to it
  // In some cases the CharacterSpacing should not be added. Ex: when the glyph is a ZWSP (Zero Width Space)
  return (TextAbstraction::IsZeroWidthNonJoiner(character) || TextAbstraction::IsZeroWidthJoiner(character) ||
          TextAbstraction::IsZeroWidthSpace(character) || TextAbstraction::IsNewParagraph(character) ||
          TextAbstraction::IsLeftToRightMark(character) || TextAbstraction::IsRightToLeftMark(character))
           ? advance
           : advance + characterSpacing;
}

float ResolveTextForegroundReferencePixelSize(const ModelInterface&       model,
                                              bool                        hasInlineReplacement,
                                              TextAbstraction::FontClient fontClient,
                                              std::vector<float>*         lineReferencePixelSizes)
{
  float          referencePixelSize = 0.0f;
  const Length   numberOfLines      = model.GetNumberOfLines();
  const LineRun* lines              = model.GetLines();
  if(lineReferencePixelSizes)
  {
    lineReferencePixelSizes->assign(numberOfLines, 0.0f);
  }
  if(!hasInlineReplacement)
  {
    for(Length index = 0u; lines && index < numberOfLines; ++index)
    {
      const float lineTextHeight = lines[index].ascender - lines[index].descender;
      if(std::isfinite(lineTextHeight))
      {
        referencePixelSize = std::max(referencePixelSize, lineTextHeight);
        if(lineReferencePixelSizes)
        {
          (*lineReferencePixelSizes)[index] = std::max(1.0f, lineTextHeight);
        }
      }
    }
    return std::max(1.0f, referencePixelSize);
  }

  struct CachedFontMetrics
  {
    FontId      fontId;
    FontMetrics metrics;
  };

  const GlyphInfo*               glyphs         = model.GetGlyphs();
  const Length                   numberOfGlyphs = model.GetNumberOfGlyphs();
  std::vector<CachedFontMetrics> metricsCache;
  auto                           getFontMetrics = [&](FontId fontId) -> const FontMetrics&
  {
    for(const CachedFontMetrics& cached : metricsCache)
    {
      if(cached.fontId == fontId)
      {
        return cached.metrics;
      }
    }
    CachedFontMetrics cached{fontId, {}};
    fontClient.GetFontMetrics(fontId, cached.metrics);
    metricsCache.push_back(cached);
    return metricsCache.back().metrics;
  };

  for(Length index = 0u; lines && index < numberOfLines; ++index)
  {
    float lineAscender    = 0.0f;
    float lineDescender   = 0.0f;
    bool  hasTextGlyph    = false;
    auto  includeGlyphRun = [&](const GlyphRun& run)
    {
      const GlyphIndex end = std::min<GlyphIndex>(run.glyphIndex + run.numberOfGlyphs, numberOfGlyphs);
      for(GlyphIndex glyphIndex = run.glyphIndex; glyphs && glyphIndex < end; ++glyphIndex)
      {
        const GlyphInfo& glyph = glyphs[glyphIndex];
        if(glyph.fontId == 0u || IsSyntheticReplacementGlyph(glyph))
        {
          continue;
        }
        const FontMetrics& metrics = getFontMetrics(glyph.fontId);
        lineAscender               = hasTextGlyph ? std::max(lineAscender, metrics.ascender) : metrics.ascender;
        lineDescender              = hasTextGlyph ? std::min(lineDescender, metrics.descender) : metrics.descender;
        hasTextGlyph               = true;
      }
    };
    includeGlyphRun(lines[index].glyphRun);
    if(lines[index].isSplitToTwoHalves)
    {
      includeGlyphRun(lines[index].glyphRunSecondHalf);
    }
    if(hasTextGlyph)
    {
      const float lineTextHeight = lineAscender - lineDescender;
      if(std::isfinite(lineTextHeight))
      {
        referencePixelSize = std::max(referencePixelSize, lineTextHeight);
        if(lineReferencePixelSizes)
        {
          (*lineReferencePixelSizes)[index] = lineTextHeight;
        }
      }
    }
  }
  return referencePixelSize;
}

} // namespace Text

} // namespace Ui

} // namespace Dali
