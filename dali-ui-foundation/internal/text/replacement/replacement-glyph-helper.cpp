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
#include <dali-ui-foundation/internal/text/replacement/replacement-glyph-helper.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/replacement/replacement-segment-iterator.h>
#include <algorithm>

namespace Dali::Ui::Text
{
namespace
{
void AppendSyntheticFontRun(const ProjectedReplacementRun& replacement, Vector<FontRun>& fonts)
{
  FontRun run;
  run.characterRun.characterIndex     = replacement.projectedCharacterIndex;
  run.characterRun.numberOfCharacters = 1u;
  run.fontId                          = 0u;
  run.isItalicRequired                = false;
  run.isBoldRequired                  = false;
  fonts.PushBack(run);
}

void AppendSyntheticGlyph(const ProjectedReplacementRun& replacement, VisualModel& visualModel)
{
  GlyphInfo glyph;
  glyph.fontId           = 0u;
  glyph.index            = SYNTHETIC_REPLACEMENT_GLYPH_ID;
  glyph.width            = replacement.metrics.width;
  glyph.height           = replacement.metrics.height;
  glyph.xBearing         = 0.0f;
  glyph.yBearing         = replacement.metrics.height - replacement.metrics.verticalOffset;
  glyph.advance          = replacement.metrics.width;
  glyph.scaleFactor      = 1.0f;
  glyph.isItalicRequired = false;
  glyph.isBoldRequired   = false;
  glyph.isShaped         = false;

  visualModel.mGlyphs.PushBack(glyph);
  visualModel.mGlyphsToCharacters.PushBack(replacement.projectedCharacterIndex);
  visualModel.mCharactersPerGlyph.PushBack(1u);
}

template<typename CharacterStyleRun, typename GlyphStyleRun, typename PropertyCopier>
void ConvertCharacterStyleRuns(const Vector<CharacterStyleRun>& characterRuns,
                               const VisualModel&               visualModel,
                               Vector<GlyphStyleRun>&           glyphRuns,
                               PropertyCopier                   copyProperties)
{
  glyphRuns.Clear();
  for(const CharacterStyleRun& characterRun : characterRuns)
  {
    const CharacterIndex characterIndex     = characterRun.characterRun.characterIndex;
    const Length         numberOfCharacters = characterRun.characterRun.numberOfCharacters;
    if(numberOfCharacters == 0u || characterIndex >= visualModel.mCharactersToGlyph.Count())
    {
      continue;
    }

    GlyphStyleRun glyphRun;
    glyphRun.glyphRun.glyphIndex     = visualModel.mCharactersToGlyph[characterIndex];
    glyphRun.glyphRun.numberOfGlyphs = 0u;
    const CharacterIndex end         = std::min<CharacterIndex>(characterIndex + numberOfCharacters,
                                                                visualModel.mGlyphsPerCharacter.Count());
    for(CharacterIndex index = characterIndex; index < end; ++index)
    {
      glyphRun.glyphRun.numberOfGlyphs += visualModel.mGlyphsPerCharacter[index];
    }
    copyProperties(characterRun, glyphRun);
    glyphRuns.PushBack(glyphRun);
  }
}

} // unnamed namespace

void ValidateFontsForProcessingSource(MultilanguageSupport&                   multilanguageSupport,
                                      TextAbstraction::FontClient&            fontClient,
                                      const TextProcessingSource&             source,
                                      const Vector<ScriptRun>&                scripts,
                                      const TextAbstraction::FontDescription& defaultFontDescription,
                                      TextAbstraction::PointSize26Dot6        defaultPointSize,
                                      float                                   effectiveScale,
                                      CharacterIndex                          startIndex,
                                      Length                                  numberOfCharacters,
                                      Vector<FontRun>&                        validFonts,
                                      Property::Map*                          variationsMapPtr)
{
  if(!source.HasReplacements())
  {
    multilanguageSupport.ValidateFonts(fontClient,
                                       *source.text,
                                       scripts,
                                       *source.fontDescriptionRuns,
                                       defaultFontDescription,
                                       defaultPointSize,
                                       effectiveScale,
                                       startIndex,
                                       numberOfCharacters,
                                       validFonts,
                                       variationsMapPtr);
    return;
  }

  const CharacterIndex end = startIndex + numberOfCharacters;
  ForEachProcessingSegment(source.text->Count(),
                           *source.replacementRuns,
                           [&](CharacterIndex segmentStart, Length segmentLength)
  {
    const CharacterIndex segmentEnd   = segmentStart + segmentLength;
    const CharacterIndex clippedStart = std::max(segmentStart, startIndex);
    const CharacterIndex clippedEnd   = std::min(segmentEnd, end);
    if(clippedEnd > clippedStart)
    {
      multilanguageSupport.ValidateFonts(fontClient,
                                         *source.text,
                                         scripts,
                                         *source.fontDescriptionRuns,
                                         defaultFontDescription,
                                         defaultPointSize,
                                         effectiveScale,
                                         clippedStart,
                                         clippedEnd - clippedStart,
                                         validFonts,
                                         variationsMapPtr);
    }
  },
                           [&](const ProjectedReplacementRun& replacement)
  {
    if(replacement.projectedCharacterIndex >= startIndex && replacement.projectedCharacterIndex < end)
    {
      AppendSyntheticFontRun(replacement, validFonts);
    }
  });
}

void ShapeTextForProcessingSource(TextAbstraction::Shaping&    shaping,
                                  TextAbstraction::FontClient& fontClient,
                                  const TextProcessingSource&  source,
                                  const Vector<Character>&     textToShape,
                                  const Vector<LineBreakInfo>& lineBreakInfo,
                                  const Vector<ScriptRun>&     scripts,
                                  const Vector<FontRun>&       validFonts,
                                  CharacterIndex               startIndex,
                                  GlyphIndex                   startGlyphIndex,
                                  Length                       numberOfCharacters,
                                  VisualModel&                 visualModel,
                                  Vector<GlyphIndex>&          newParagraphGlyphs)
{
  if(!source.HasReplacements())
  {
    ShapeText(shaping,
              fontClient,
              textToShape,
              lineBreakInfo,
              scripts,
              validFonts,
              startIndex,
              startGlyphIndex,
              numberOfCharacters,
              visualModel.mGlyphs,
              visualModel.mGlyphsToCharacters,
              visualModel.mCharactersPerGlyph,
              newParagraphGlyphs);
    return;
  }

  const CharacterIndex end = startIndex + numberOfCharacters;
  ForEachProcessingSegment(source.text->Count(),
                           *source.replacementRuns,
                           [&](CharacterIndex segmentStart, Length segmentLength)
  {
    const CharacterIndex segmentEnd   = segmentStart + segmentLength;
    const CharacterIndex clippedStart = std::max(segmentStart, startIndex);
    const CharacterIndex clippedEnd   = std::min(segmentEnd, end);
    if(clippedEnd > clippedStart)
    {
      ShapeText(shaping,
                fontClient,
                textToShape,
                lineBreakInfo,
                scripts,
                validFonts,
                clippedStart,
                visualModel.mGlyphs.Count(),
                clippedEnd - clippedStart,
                visualModel.mGlyphs,
                visualModel.mGlyphsToCharacters,
                visualModel.mCharactersPerGlyph,
                newParagraphGlyphs);
    }
  },
                           [&](const ProjectedReplacementRun& replacement)
  {
    if(replacement.projectedCharacterIndex >= startIndex && replacement.projectedCharacterIndex < end)
    {
      AppendSyntheticGlyph(replacement, visualModel);
    }
  });
}

void GetGlyphMetricsForProcessingSource(Metrics&                    metrics,
                                        const TextProcessingSource& source,
                                        Vector<GlyphInfo>&          glyphs,
                                        GlyphIndex                  startGlyphIndex,
                                        Length                      numberOfGlyphs,
                                        const Vector<GlyphIndex>&   newParagraphGlyphs)
{
  if(!source.HasReplacements())
  {
    metrics.GetGlyphMetrics(glyphs.Begin() + startGlyphIndex, numberOfGlyphs);
  }
  else
  {
    const GlyphIndex end           = std::min<GlyphIndex>(startGlyphIndex + numberOfGlyphs, glyphs.Count());
    GlyphIndex       ordinaryStart = startGlyphIndex;
    for(GlyphIndex index = startGlyphIndex; index <= end; ++index)
    {
      const bool atEnd     = index == end;
      const bool synthetic = !atEnd && IsSyntheticReplacementGlyph(glyphs[index]);
      if((atEnd || synthetic) && index > ordinaryStart)
      {
        metrics.GetGlyphMetrics(glyphs.Begin() + ordinaryStart, index - ordinaryStart);
      }
      if(synthetic)
      {
        ordinaryStart = index + 1u;
      }
    }
  }

  for(const GlyphIndex index : newParagraphGlyphs)
  {
    GlyphInfo& glyph = glyphs[index];
    glyph.xBearing   = 0.0f;
    glyph.width      = 0.0f;
    glyph.advance    = 0.0f;
  }
}

void CopyProcessingCharacterStylesToVisual(const LogicalModel& logicalModel, VisualModel& visualModel)
{
  ConvertCharacterStyleRuns(logicalModel.mUnderlinedCharacterRuns,
                            visualModel,
                            visualModel.mUnderlineRuns,
                            [](const UnderlinedCharacterRun& source, UnderlinedGlyphRun& destination)
  {
    destination.properties = source.properties;
  });
  ConvertCharacterStyleRuns(logicalModel.mStrikethroughCharacterRuns,
                            visualModel,
                            visualModel.mStrikethroughRuns,
                            [](const StrikethroughCharacterRun& source, StrikethroughGlyphRun& destination)
  {
    destination.properties = source.properties;
  });
  ConvertCharacterStyleRuns(logicalModel.mCharacterSpacingCharacterRuns,
                            visualModel,
                            visualModel.mCharacterSpacingRuns,
                            [](const CharacterSpacingCharacterRun& source, CharacterSpacingGlyphRun& destination)
  {
    destination.value = source.value;
  });
}

} // namespace Dali::Ui::Text
