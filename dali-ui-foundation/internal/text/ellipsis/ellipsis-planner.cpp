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
#include <algorithm>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/ellipsis/ellipsis-metrics.h>
#include <dali-ui-foundation/internal/text/ellipsis/ellipsis-planner.h>
#include <dali-ui-foundation/internal/text/glyph-metrics-helper.h>
#include <dali-ui-foundation/internal/text/rendering/styles/character-spacing-helper-functions.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-run-snapshot.h>

namespace Dali::Ui::Text
{
namespace
{
class EllipsisFontSearch
{
public:
  explicit EllipsisFontSearch(const EndEllipsisInputView& input)
  : mInput(input)
  {
  }

  FontId Resolve(GlyphIndex glyphIndex,
                 GlyphIndex overriddenGlyphIndex,
                 FontId     overriddenFontId)
  {
    UpdateFollowing(glyphIndex, overriddenGlyphIndex, overriddenFontId);

    const GlyphInfo& glyph = mInput.glyphs[glyphIndex];
    if(glyph.fontId != 0u)
    {
      return glyph.fontId;
    }
    if(!IsSyntheticReplacementGlyph(glyph))
    {
      return 0u;
    }

    ResolvePreceding(glyphIndex);
    ResolveFollowing(glyphIndex, overriddenGlyphIndex, overriddenFontId);
    const GlyphIndex invalid = std::numeric_limits<GlyphIndex>::max();
    if(mPrecedingIndex != invalid &&
       (mFollowingIndex == invalid ||
        glyphIndex - mPrecedingIndex <= mFollowingIndex - glyphIndex))
    {
      return mPrecedingFontId;
    }
    return mFollowingIndex == invalid ? 0u : mFollowingFontId;
  }

private:
  void UpdateFollowing(GlyphIndex glyphIndex,
                       GlyphIndex overriddenGlyphIndex,
                       FontId     overriddenFontId)
  {
    const GlyphIndex invalid = std::numeric_limits<GlyphIndex>::max();
    if(mCurrentIndex == invalid)
    {
      mCurrentIndex = glyphIndex;
      return;
    }

    for(GlyphIndex index = mCurrentIndex; index > glyphIndex; --index)
    {
      const FontId fontId = index == overriddenGlyphIndex
                              ? overriddenFontId
                              : mInput.glyphs[index].fontId;
      if(fontId != 0u)
      {
        mFollowingIndex    = index;
        mFollowingFontId   = fontId;
        mFollowingResolved = true;
      }
    }

    if(mPrecedingIndex != invalid && mPrecedingIndex >= glyphIndex)
    {
      mPrecedingIndex    = invalid;
      mPrecedingFontId   = 0u;
      mPrecedingResolved = false;
    }
    mCurrentIndex = glyphIndex;
  }

  void ResolvePreceding(GlyphIndex glyphIndex)
  {
    if(mPrecedingResolved)
    {
      return;
    }

    mPrecedingIndex  = std::numeric_limits<GlyphIndex>::max();
    mPrecedingFontId = 0u;
    for(GlyphIndex index = glyphIndex; index > 0u;)
    {
      --index;
      const FontId fontId = mInput.glyphs[index].fontId;
      if(fontId != 0u)
      {
        mPrecedingIndex  = index;
        mPrecedingFontId = fontId;
        break;
      }
    }
    mPrecedingResolved = true;
  }

  void ResolveFollowing(GlyphIndex glyphIndex,
                        GlyphIndex overriddenGlyphIndex,
                        FontId     overriddenFontId)
  {
    if(mFollowingResolved)
    {
      return;
    }

    mFollowingIndex  = std::numeric_limits<GlyphIndex>::max();
    mFollowingFontId = 0u;
    for(GlyphIndex index = glyphIndex + 1u; index < mInput.numberOfGlyphs; ++index)
    {
      const FontId fontId = index == overriddenGlyphIndex
                              ? overriddenFontId
                              : mInput.glyphs[index].fontId;
      if(fontId != 0u)
      {
        mFollowingIndex  = index;
        mFollowingFontId = fontId;
        break;
      }
    }
    mFollowingResolved = true;
  }

private:
  const EndEllipsisInputView& mInput;
  GlyphIndex                  mCurrentIndex{std::numeric_limits<GlyphIndex>::max()};
  GlyphIndex                  mPrecedingIndex{std::numeric_limits<GlyphIndex>::max()};
  GlyphIndex                  mFollowingIndex{std::numeric_limits<GlyphIndex>::max()};
  FontId                      mPrecedingFontId{0u};
  FontId                      mFollowingFontId{0u};
  bool                        mPrecedingResolved{false};
  bool                        mFollowingResolved{false};
};

const Vector2* GetGlyphPosition(const EndEllipsisInputView& input, GlyphIndex glyphIndex)
{
  if(glyphIndex < input.glyphPositionStartIndex)
  {
    return nullptr;
  }

  const GlyphIndex positionIndex = glyphIndex - input.glyphPositionStartIndex;
  return positionIndex < input.numberOfGlyphPositions ? input.glyphPositions + positionIndex : nullptr;
}
} // unnamed namespace

EndEllipsisPlan ResolveEndEllipsisPlan(const EndEllipsisInputView&  input,
                                       TextAbstraction::FontClient& fontClient)
{
  EndEllipsisPlan plan;
  if(!input.glyphs ||
     !input.glyphPositions ||
     !input.text ||
     !input.glyphToCharacterMap ||
     !input.characterSpacingRuns ||
     input.numberOfGlyphs == 0u ||
     input.numberOfGlyphPositions == 0u ||
     input.startIndex >= input.numberOfGlyphs ||
     !GetGlyphPosition(input, input.startIndex))
  {
    return plan;
  }

  float firstPenX          = 0.0f;
  float penY               = 0.0f;
  float removedGlyphsWidth = 0.0f;
  bool  firstPenSet        = false;

  GlyphIndex         glyphIndex                        = input.startIndex;
  GlyphIndex         overriddenGlyphIndex              = std::numeric_limits<GlyphIndex>::max();
  GlyphIndex         firstRemovedReplacementGlyphIndex = EndEllipsisPlan::INVALID_GLYPH_INDEX;
  GlyphIndex         lastRemovedReplacementGlyphIndex  = EndEllipsisPlan::INVALID_GLYPH_INDEX;
  FontId             overriddenFontId                  = 0u;
  float              overriddenPositionX               = 0.0f;
  EllipsisFontSearch fontSearch(input);

  while(true)
  {
    if(glyphIndex >= input.numberOfGlyphs)
    {
      return plan;
    }
    const Vector2* glyphPosition = GetGlyphPosition(input, glyphIndex);
    if(!glyphPosition)
    {
      return plan;
    }

    const CharacterIndex characterIndex = input.glyphToCharacterMap[glyphIndex];
    if(characterIndex >= input.numberOfCharacters)
    {
      return plan;
    }

    const GlyphInfo& glyphToRemove = input.glyphs[glyphIndex];
    if(IsSyntheticReplacementGlyph(glyphToRemove))
    {
      firstRemovedReplacementGlyphIndex = std::min(firstRemovedReplacementGlyphIndex, glyphIndex);
      lastRemovedReplacementGlyphIndex  = lastRemovedReplacementGlyphIndex == EndEllipsisPlan::INVALID_GLYPH_INDEX
                                            ? glyphIndex
                                            : std::max(lastRemovedReplacementGlyphIndex, glyphIndex);
    }
    const float characterSpacing =
      GetGlyphCharacterSpacing(glyphIndex, *input.characterSpacingRuns, input.modelCharacterSpacing);
    const float calculatedAdvance =
      GetCalculatedAdvance(input.text[characterIndex], characterSpacing, glyphToRemove.advance);
    const float glyphWidth = std::min(calculatedAdvance, glyphToRemove.xBearing + glyphToRemove.width);
    removedGlyphsWidth += glyphWidth;

    const FontId ellipsisFontId = fontSearch.Resolve(glyphIndex,
                                                     overriddenGlyphIndex,
                                                     overriddenFontId);
    GlyphInfo    ellipsisGlyph;
    const bool   hasEllipsisMetrics = ResolveFontClientEllipsisMetrics(fontClient,
                                                                       ellipsisFontId,
                                                                       ellipsisFontId == 0u &&
                                                                         IsSyntheticReplacementGlyph(glyphToRemove),
                                                                       ellipsisGlyph);
    if(hasEllipsisMetrics)
    {
      if(!firstPenSet)
      {
        const Vector2& position = *glyphPosition;
        penY                    = position.y + glyphToRemove.yBearing;
        firstPenX               = position.x + input.positionOffset - glyphToRemove.xBearing;
        if(firstPenX < -ellipsisGlyph.xBearing)
        {
          firstPenX = -ellipsisGlyph.xBearing;
        }
        removedGlyphsWidth += -ellipsisGlyph.xBearing;
        firstPenSet = true;
      }

      const float ellipsisGlyphWidth = ellipsisGlyph.width + ellipsisGlyph.xBearing;
      if(ellipsisGlyphWidth < removedGlyphsWidth || glyphIndex == 0u)
      {
        Vector2 position = *glyphPosition;
        position.x += input.positionOffset;
        position.x -= 0.0f > glyphToRemove.xBearing ? glyphToRemove.xBearing : 0.0f;

        if(position.x > firstPenX)
        {
          float nextPositionX = input.lineWidth;
          if(glyphIndex + 1u < input.numberOfGlyphs)
          {
            const Vector2* nextPosition = GetGlyphPosition(input, glyphIndex + 1u);
            if(nextPosition)
            {
              nextPositionX = glyphIndex + 1u == overriddenGlyphIndex
                                ? overriddenPositionX
                                : nextPosition->x + input.positionOffset;
            }
          }

          if(position.x > nextPositionX)
          {
            if(glyphIndex > 0u && position.x - nextPositionX > removedGlyphsWidth)
            {
              firstPenX = position.x - ellipsisGlyph.xBearing;
              if(firstPenX < -ellipsisGlyph.xBearing)
              {
                firstPenX = -ellipsisGlyph.xBearing;
              }
              removedGlyphsWidth = std::min(calculatedAdvance,
                                            ellipsisGlyph.xBearing + ellipsisGlyph.width) -
                                   ellipsisGlyph.xBearing;
              overriddenGlyphIndex = glyphIndex;
              overriddenFontId     = ellipsisGlyph.fontId;
              overriddenPositionX  = position.x;
              --glyphIndex;
              continue;
            }

            position.x = firstPenX + removedGlyphsWidth - ellipsisGlyphWidth;
          }
        }

        position.x += ellipsisGlyph.xBearing;
        position.y = penY - ellipsisGlyph.yBearing;

        plan.ellipsisGlyph      = ellipsisGlyph;
        plan.ellipsisPosition   = position;
        plan.ellipsisGlyphIndex = glyphIndex;

        plan.firstRemovedReplacementGlyphIndex = firstRemovedReplacementGlyphIndex;
        plan.lastRemovedReplacementGlyphIndex  = lastRemovedReplacementGlyphIndex;

        plan.resolved = true;
        return plan;
      }
    }

    if(glyphIndex == 0u)
    {
      ++plan.numberOfRemovedGlyphs;
      return plan;
    }

    --glyphIndex;
    ++plan.numberOfRemovedGlyphs;
  }
}

} // namespace Dali::Ui::Text
