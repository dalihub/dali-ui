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

// CLASS HEADER
#include <dali-ui-foundation/internal/text/styled-text/gradient-span-data.h>

// EXTERNAL INCLUDES
#include <algorithm>

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace Internal
{

void SetGradientSpanSegmentationInfo(GradientSpanModelData&    data,
                                     const Vector<GlyphIndex>& charactersToGlyph,
                                     const Vector<Length>&     glyphsPerCharacter,
                                     CharacterIndex            startCharacterIndex,
                                     GlyphIndex                startGlyphIndex,
                                     Length                    numberOfCharacters)
{
  if(charactersToGlyph.Empty() || numberOfCharacters == 0u)
  {
    return;
  }

  const CharacterIndex lastCharacterIndex = startCharacterIndex + numberOfCharacters - 1u;
  const Length         numberOfNewGlyphs  = charactersToGlyph[lastCharacterIndex] + glyphsPerCharacter[lastCharacterIndex] -
                                   charactersToGlyph[startCharacterIndex];

  Vector<GradientSpanPaintIndex> newPaintIndices;
  newPaintIndices.Resize(numberOfNewGlyphs);

  for(const GradientSpanCharacterRun& run : data.characterRuns)
  {
    const CharacterIndex runEnd    = run.characterRun.characterIndex + run.characterRun.numberOfCharacters;
    const CharacterIndex updateEnd = startCharacterIndex + numberOfCharacters;
    if(run.characterRun.numberOfCharacters == 0u || runEnd <= startCharacterIndex ||
       run.characterRun.characterIndex >= updateEnd)
    {
      continue;
    }

    const CharacterIndex firstCharacter = std::max(startCharacterIndex, run.characterRun.characterIndex);
    const CharacterIndex lastCharacter  = std::min(updateEnd, runEnd) - 1u;
    const GlyphIndex     firstGlyph     = std::max(startGlyphIndex, charactersToGlyph[firstCharacter]) - startGlyphIndex;
    const GlyphIndex     lastGlyphPlusOne =
      std::min(numberOfNewGlyphs,
               charactersToGlyph[lastCharacter] + glyphsPerCharacter[lastCharacter] - startGlyphIndex);
    for(GlyphIndex glyph = firstGlyph; glyph < lastGlyphPlusOne; ++glyph)
    {
      newPaintIndices[glyph] = run.paintIndex;
    }
  }

  data.glyphPaintIndices.Insert(data.glyphPaintIndices.Begin() + startGlyphIndex,
                                newPaintIndices.Begin(),
                                newPaintIndices.End());
}

} // namespace Internal
} // namespace Text
} // namespace Ui
} // namespace Dali
