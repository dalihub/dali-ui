/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/math/vector2.h>
#include <memory.h>
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/ellipsis/ellipsis-planner.h>
#include <dali-ui-foundation/internal/text/glyph-metrics-helper.h>
#include <dali-ui-foundation/internal/text/line-helper-functions.h>
#include <dali-ui-foundation/internal/text/rendering/styles/character-spacing-helper-functions.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-run-snapshot.h>
#include <dali-ui-foundation/internal/text/text-view.h>

namespace Dali::Ui::Text
{
namespace
{
// Safely shift a glyph buffer in-place using memmove,
// clamping the copy length to the valid [0, bufferSize) range.
template<typename T>
void GlyphMemmove(T* buffer, Length bufferSize, Length dstIndex, Length srcIndex, Length count)
{
  if(!buffer || bufferSize == 0u || count == 0u || dstIndex >= bufferSize || srcIndex >= bufferSize)
  {
    return;
  }

  const Length maxByDst  = bufferSize - dstIndex;
  const Length maxBySrc  = bufferSize - srcIndex;
  Length       safeCount = std::min(count, std::min(maxByDst, maxBySrc));

  if(safeCount > 0u)
  {
    memmove(buffer + dstIndex, buffer + srcIndex, safeCount * sizeof(T));
  }
}

FontId FindEllipsisFontId(const GlyphInfo* glyphs, Length numberOfGlyphs, GlyphIndex glyphIndex)
{
  if(!glyphs || glyphIndex >= numberOfGlyphs)
  {
    return 0u;
  }

  if(glyphs[glyphIndex].fontId != 0u)
  {
    return glyphs[glyphIndex].fontId;
  }

  if(!IsSyntheticReplacementGlyph(glyphs[glyphIndex]))
  {
    return 0u;
  }

  for(Length distance = 1u; distance < numberOfGlyphs; ++distance)
  {
    if(distance <= glyphIndex)
    {
      const FontId precedingFontId = glyphs[glyphIndex - distance].fontId;
      if(precedingFontId != 0u)
      {
        return precedingFontId;
      }
    }

    const GlyphIndex followingIndex = glyphIndex + distance;
    if(followingIndex < numberOfGlyphs)
    {
      const FontId followingFontId = glyphs[followingIndex].fontId;
      if(followingFontId != 0u)
      {
        return followingFontId;
      }
    }
  }

  return 0u;
}

FontId ResolveEllipsisFontId(TextAbstraction::FontClient& fontClient,
                             const GlyphInfo*             glyphs,
                             Length                       numberOfGlyphs,
                             GlyphIndex                   glyphIndex)
{
  FontId fontId = FindEllipsisFontId(glyphs, numberOfGlyphs, glyphIndex);
  if(fontId == 0u && glyphs && glyphIndex < numberOfGlyphs && IsSyntheticReplacementGlyph(glyphs[glyphIndex]))
  {
    // Replacement-only lines deliberately have no font run. Use the normal
    // default font solely for the generated ellipsis; the synthetic identity
    // and font-free shaping contract remain unchanged.
    TextAbstraction::FontDescription defaultFontDescription;
    fontId = fontClient.GetFontId(defaultFontDescription, TextAbstraction::FontClient::DEFAULT_POINT_SIZE);
  }
  return fontId;
}

/// If ellipsis is enabled, calculate the number of laid out glyphs.
/// Otherwise use the given number of glyphs.
void CalculateNumberOfLaidOutGlyphes(const bool hasEllipsis, bool& textElided, Length& numberOfLaidOutGlyphs,
                                     Length& numberOfActualLaidOutGlyphs, const Length& numberOfGlyphs,
                                     const Text::EllipsisPosition::Type& ellipsisPosition,
                                     const LineRun*& ellipsisLine, const Length& numberOfLines,
                                     const LineRun* const& lines)
{
  if(hasEllipsis)
  {
    textElided            = true;
    numberOfLaidOutGlyphs = numberOfGlyphs;

    switch(ellipsisPosition)
    {
      case Text::EllipsisPosition::START:
      {
        numberOfActualLaidOutGlyphs = numberOfGlyphs - ellipsisLine->glyphRun.glyphIndex;
        break;
      }
      case Text::EllipsisPosition::MIDDLE:
      {
        numberOfActualLaidOutGlyphs = 0u;
        for(Length lineIndex = 0u; lineIndex < numberOfLines; lineIndex++)
        {
          numberOfActualLaidOutGlyphs +=
            lines[lineIndex].glyphRun.numberOfGlyphs + lines[lineIndex].glyphRunSecondHalf.numberOfGlyphs;
        }
        break;
      }
      case Text::EllipsisPosition::END:
      {
        numberOfActualLaidOutGlyphs = ellipsisLine->glyphRun.glyphIndex + ellipsisLine->glyphRun.numberOfGlyphs;
        break;
      }
    }
  }
  else
  {
    numberOfActualLaidOutGlyphs = numberOfLaidOutGlyphs = numberOfGlyphs;
  }
}

bool InsertEllipsisGlyph(GlyphInfo*& glyphs, GlyphIndex& indexOfEllipsis, Length& numberOfRemovedGlyphs,
                         Vector2*& glyphPositions, TextAbstraction::FontClient& fontClient,
                         const Vector<CharacterSpacingGlyphRun>& characterSpacingGlyphRuns,
                         const float& modelCharacterSpacing, float& calculatedAdvance, const Character*& textBuffer,
                         const CharacterIndex*& glyphToCharacterMapBuffer, const Length& numberOfGlyphs,
                         const bool isTailMode, const LineRun*& ellipsisLine, const Length& numberOfLaidOutGlyphs,
                         bool hasActiveReplacement)
{
  // firstPenX, penY and firstPenSet are used to position the ellipsis glyph if needed.
  float firstPenX        = 0.f; // Used if rtl text is elided.
  float penY             = 0.f;
  bool  firstPenSet      = false;
  bool  inserted         = false;
  bool  ellipsisInserted = false;

  float removedGlyphsWidth = 0.f;

  while(!inserted)
  {
    const GlyphInfo& glyphToRemove = *(glyphs + indexOfEllipsis);

    if(hasActiveReplacement)
    {
      const float characterSpacing =
        GetGlyphCharacterSpacing(indexOfEllipsis, characterSpacingGlyphRuns, modelCharacterSpacing);
      calculatedAdvance = GetCalculatedAdvance(*(textBuffer + (*(glyphToCharacterMapBuffer + indexOfEllipsis))),
                                               characterSpacing, glyphToRemove.advance);
      // Synthetic replacement glyphs have fontId 0 but retain their full box
      // advance. That width must participate in the removed space or the
      // ellipsis is separated from the visible text by an image-sized hole.
      removedGlyphsWidth += std::min(calculatedAdvance, (glyphToRemove.xBearing + glyphToRemove.width));
    }

    const FontId ellipsisFontId = hasActiveReplacement
                                    ? ResolveEllipsisFontId(fontClient, glyphs, numberOfGlyphs, indexOfEllipsis)
                                    : glyphToRemove.fontId;

    if(0u != ellipsisFontId)
    {
      // Need to reshape the glyph as the font may be different in size.
      const GlyphInfo& ellipsisGlyph = fontClient.GetEllipsisGlyph(fontClient.GetPointSize(ellipsisFontId));

      if(!firstPenSet)
      {
        const Vector2& position = *(glyphPositions + indexOfEllipsis);

        // Calculates the penY of the current line. It will be used to position the ellipsis glyph.
        penY = position.y + glyphToRemove.yBearing;

        // Calculates the first penX which will be used if rtl text is elided.
        firstPenX = position.x - glyphToRemove.xBearing;
        if(firstPenX < -ellipsisGlyph.xBearing)
        {
          // Avoids to exceed the bounding box when rtl text is elided.
          firstPenX = -ellipsisGlyph.xBearing;
        }

        removedGlyphsWidth = (hasActiveReplacement ? removedGlyphsWidth : 0.0f) - ellipsisGlyph.xBearing;

        firstPenSet = true;
      }

      if(!hasActiveReplacement)
      {
        const float characterSpacing =
          GetGlyphCharacterSpacing(indexOfEllipsis, characterSpacingGlyphRuns, modelCharacterSpacing);
        calculatedAdvance = GetCalculatedAdvance(*(textBuffer + (*(glyphToCharacterMapBuffer + indexOfEllipsis))),
                                                 characterSpacing, glyphToRemove.advance);
        removedGlyphsWidth += std::min(calculatedAdvance, (glyphToRemove.xBearing + glyphToRemove.width));
      }

      // Calculate the width of the ellipsis glyph and check if it fits.
      const float ellipsisGlyphWidth = ellipsisGlyph.width + ellipsisGlyph.xBearing;
      if((ellipsisGlyphWidth < removedGlyphsWidth) ||
         (isTailMode ? (indexOfEllipsis == 0u) : (indexOfEllipsis == numberOfGlyphs - 1u)))
      {
        GlyphInfo& glyphInfo = *(glyphs + indexOfEllipsis);
        Vector2&   position  = *(glyphPositions + indexOfEllipsis);
        position.x -= (0.f > glyphInfo.xBearing) ? glyphInfo.xBearing : 0.f;

        // Replace the glyph by the ellipsis glyph.
        glyphInfo        = ellipsisGlyph;
        ellipsisInserted = true;

        // Change the 'x' and 'y' position of the ellipsis glyph.
        if(position.x > firstPenX)
        {
          if(isTailMode)
          {
            // To handle case of the mixed languages (LTR then RTL) with
            // EllipsisPosition::END and the LayoutDirection::RIGHT_TO_LEFT
            float nextXPositions = ellipsisLine->width;
            if(indexOfEllipsis + 1u < numberOfGlyphs)
            {
              Vector2& positionOfNextGlyph = *(glyphPositions + indexOfEllipsis + 1u);
              nextXPositions               = positionOfNextGlyph.x;
            }

            if(position.x > nextXPositions) // RTL language
            {
              if((indexOfEllipsis > 0u) && ((position.x - nextXPositions) > removedGlyphsWidth))
              {
                // To handle mixed directions
                // Re-calculates the first penX which will be used if rtl text is elided.
                firstPenX = position.x - glyphToRemove.xBearing;
                if(firstPenX < -ellipsisGlyph.xBearing)
                {
                  // Avoids to exceed the bounding box when rtl text is elided.
                  firstPenX = -ellipsisGlyph.xBearing;
                }
                // Reset the width of removed glyphs
                removedGlyphsWidth = std::min(calculatedAdvance, (glyphToRemove.xBearing + glyphToRemove.width)) -
                                     ellipsisGlyph.xBearing;

                --indexOfEllipsis;
                continue;
              }
              else
              {
                // To handle the case of RTL language with EllipsisPosition::END
                position.x = firstPenX + removedGlyphsWidth - ellipsisGlyphWidth;
              }
            }
          }
          else
          {
            // To handle the case of LTR language with EllipsisPosition::START
            position.x = firstPenX + removedGlyphsWidth - ellipsisGlyphWidth;
          }
        }
        else
        {
          if(!isTailMode)
          {
            // To handle case of the mixed languages (RTL then LTR) with
            // EllipsisPosition::START and the LayoutDirection::RIGHT_TO_LEFT
            float nextXPositions = ellipsisLine->width;
            if(indexOfEllipsis + 1u < numberOfGlyphs)
            {
              Vector2& positionOfNextGlyph = *(glyphPositions + indexOfEllipsis + 1u);
              nextXPositions               = positionOfNextGlyph.x;
            }

            if(position.x < nextXPositions) // LTR language
            {
              position.x = firstPenX + removedGlyphsWidth - ellipsisGlyphWidth;

              if((position.x + ellipsisGlyphWidth + ellipsisGlyph.xBearing) > nextXPositions)
              {
                position.x -= (position.x + ellipsisGlyphWidth + ellipsisGlyph.xBearing) - nextXPositions;
              }
            }
          }
        }

        position.x += ellipsisGlyph.xBearing;
        position.y = penY - ellipsisGlyph.yBearing;

        inserted = true;
      }
    }

    if(!inserted)
    {
      if(isTailMode && indexOfEllipsis > 0u)
      {
        // Tail Mode: remove glyphs from startIndexOfEllipsis then decrement indexOfEllipsis, until arrive to index
        // zero.
        --indexOfEllipsis;
      }
      else if(!isTailMode && indexOfEllipsis < numberOfLaidOutGlyphs - 1u)
      {
        // Not Tail Mode: remove glyphs from startIndexOfEllipsis then increase indexOfEllipsis, until arrive to last
        // index (numberOfGlyphs - 1u).
        ++indexOfEllipsis;
      }
      else
      {
        // No space for the ellipsis.
        inserted = true;
      }
      ++numberOfRemovedGlyphs;
    }
  }
  return ellipsisInserted;
}

/// 'Removes' all the glyphs after the ellipsis glyph.
void RemoveAllGlyphsAfterEllipsisGlyph(const Text::EllipsisPosition::Type& ellipsisPosition,
                                       Length& numberOfLaidOutGlyphs, const Length& numberOfActualLaidOutGlyphs,
                                       const Length& numberOfRemovedGlyphs, const bool isTailMode,
                                       const GlyphIndex& indexOfEllipsis, const LineRun*& ellipsisNextLine,
                                       const LineRun*& ellipsisLine, GlyphInfo*& glyphs, Vector2*& glyphPositions,
                                       const Length& numberOfGlyphs, const GlyphIndex& startIndexOfEllipsis,
                                       VisualModelPtr& visualModel, GlyphIndex* sourceGlyphIndices,
                                       GlyphIndex* ellipsisFinalGlyphIndex)
{
  switch(ellipsisPosition)
  {
    case Text::EllipsisPosition::MIDDLE:
    {
      // Reduce size, shift glyphs and start from ellipsis glyph
      numberOfLaidOutGlyphs = numberOfActualLaidOutGlyphs - numberOfRemovedGlyphs;

      GlyphIndex firstMiddleIndexOfElidedGlyphs  = 0u;
      GlyphIndex secondMiddleIndexOfElidedGlyphs = 0u;

      bool isOnlySecondHalf = false;
      if(isTailMode)
      {
        // Multi-lines case with MIDDLE
        // In case the Ellipsis in the end of line,
        // then this index will be the firstMiddleIndex.
        // The secondMiddleIndex will be the fisrt index in next line.
        // But in case there is no line after Ellipsis's line then secondMiddleIndex and endIndex equal firstMiddle
        // Example:
        // A: are laid out glyphs in line has Ellipsis in the end.
        // N: are laid out glyphs in lines after removed lines.
        // R: are removed glyphs.
        // L: are removed glyphs when removed lines.
        // AAAAAAAAAAAA...RRR    => Here's the firstMiddleIndex (First index after last A)
        // LLLLLLLLLLLLLLL
        // LLLLLLLLLLLLLLL
        // NNNNNNNNNNNNNN        => Here's the secondMiddleIndex (First N)
        // NNNNNNNNNN

        firstMiddleIndexOfElidedGlyphs = indexOfEllipsis;
        if(ellipsisNextLine != nullptr)
        {
          secondMiddleIndexOfElidedGlyphs = ellipsisNextLine->glyphRun.glyphIndex;
        }
        else
        {
          secondMiddleIndexOfElidedGlyphs = firstMiddleIndexOfElidedGlyphs;
          visualModel->SetEndIndexOfElidedGlyphs(firstMiddleIndexOfElidedGlyphs);
        }
      }
      else
      {
        // Single line case with MIDDLE
        // In case the Ellipsis in the middle of line,
        // Then the last index in first half will be firstMiddleIndex.
        // And the indexOfEllipsis will be secondMiddleIndex, which is the first index in second half.
        // Example:
        // A: are laid out glyphs in first half of line.
        // N: are laid out glyphs in second half of line.
        // R: are removed glyphs.
        // L: re removed glyphs when layouting text
        // AAAAAAALLLLLLLLLLLRRR...NNNNN
        // firstMiddleIndex (index of last A)
        // secondMiddleIndex (index before first N)

        firstMiddleIndexOfElidedGlyphs =
          (ellipsisLine->glyphRun.numberOfGlyphs > 0u)
            ? (ellipsisLine->glyphRun.glyphIndex + ellipsisLine->glyphRun.numberOfGlyphs - 1u)
            : (ellipsisLine->glyphRun.glyphIndex);
        secondMiddleIndexOfElidedGlyphs = indexOfEllipsis;
        isOnlySecondHalf =
          ellipsisLine->glyphRun.numberOfGlyphs == 0u && ellipsisLine->glyphRunSecondHalf.numberOfGlyphs > 0u;
      }

      visualModel->SetFirstMiddleIndexOfElidedGlyphs(firstMiddleIndexOfElidedGlyphs);
      visualModel->SetSecondMiddleIndexOfElidedGlyphs(secondMiddleIndexOfElidedGlyphs);

      // The number of shifted glyphs and shifting positions will be different according to Single-line or Multi-lines.
      // isOnlySecondHalf will be true when MIDDLE Ellipsis glyph in single line.
      if(isOnlySecondHalf)
      {
        Length numberOfSecondHalfGlyphs = numberOfLaidOutGlyphs - firstMiddleIndexOfElidedGlyphs;

        // Copy elided glyphs after the ellipsis glyph.
        GlyphMemmove(glyphs, numberOfGlyphs, firstMiddleIndexOfElidedGlyphs, secondMiddleIndexOfElidedGlyphs,
                     numberOfSecondHalfGlyphs);
        GlyphMemmove(glyphPositions, numberOfGlyphs, firstMiddleIndexOfElidedGlyphs, secondMiddleIndexOfElidedGlyphs,
                     numberOfSecondHalfGlyphs);
        if(sourceGlyphIndices)
        {
          GlyphMemmove(sourceGlyphIndices, numberOfGlyphs, firstMiddleIndexOfElidedGlyphs,
                       secondMiddleIndexOfElidedGlyphs, numberOfSecondHalfGlyphs);
        }
        if(ellipsisFinalGlyphIndex)
        {
          *ellipsisFinalGlyphIndex = firstMiddleIndexOfElidedGlyphs;
        }
      }
      else
      {
        Length numberOfSecondHalfGlyphs = numberOfLaidOutGlyphs - firstMiddleIndexOfElidedGlyphs + 1u;

        // Make sure that out-of-boundary does not occur for the source range.
        if(secondMiddleIndexOfElidedGlyphs + numberOfSecondHalfGlyphs > numberOfGlyphs)
        {
          numberOfSecondHalfGlyphs = numberOfGlyphs - secondMiddleIndexOfElidedGlyphs;
        }

        const Length dstIndex = firstMiddleIndexOfElidedGlyphs + 1u;

        // Copy elided glyphs after the ellipsis glyph.
        GlyphMemmove(glyphs, numberOfGlyphs, dstIndex, secondMiddleIndexOfElidedGlyphs, numberOfSecondHalfGlyphs);
        GlyphMemmove(glyphPositions, numberOfGlyphs, dstIndex, secondMiddleIndexOfElidedGlyphs,
                     numberOfSecondHalfGlyphs);
        if(sourceGlyphIndices)
        {
          GlyphMemmove(sourceGlyphIndices, numberOfGlyphs, dstIndex, secondMiddleIndexOfElidedGlyphs,
                       numberOfSecondHalfGlyphs);
        }
        if(ellipsisFinalGlyphIndex)
        {
          *ellipsisFinalGlyphIndex = isTailMode ? indexOfEllipsis : dstIndex;
        }
      }
      break;
    }

    case Text::EllipsisPosition::START:
    {
      numberOfLaidOutGlyphs = numberOfActualLaidOutGlyphs - numberOfRemovedGlyphs;

      const Length dstIndex = 0u;
      const Length srcIndex = startIndexOfEllipsis + numberOfRemovedGlyphs;

      // Copy elided glyphs after the ellipsis glyph.
      GlyphMemmove(glyphs, numberOfGlyphs, dstIndex, srcIndex, numberOfLaidOutGlyphs);
      GlyphMemmove(glyphPositions, numberOfGlyphs, dstIndex, srcIndex, numberOfLaidOutGlyphs);
      if(sourceGlyphIndices)
      {
        GlyphMemmove(sourceGlyphIndices, numberOfGlyphs, dstIndex, srcIndex, numberOfLaidOutGlyphs);
      }

      visualModel->SetStartIndexOfElidedGlyphs(indexOfEllipsis);
      if(ellipsisFinalGlyphIndex)
      {
        *ellipsisFinalGlyphIndex = 0u;
      }
      break;
    }

    case Text::EllipsisPosition::END:
    {
      numberOfLaidOutGlyphs = numberOfActualLaidOutGlyphs - numberOfRemovedGlyphs;
      visualModel->SetEndIndexOfElidedGlyphs(indexOfEllipsis);
      if(ellipsisFinalGlyphIndex)
      {
        *ellipsisFinalGlyphIndex = indexOfEllipsis;
      }
      break;
    }
  }
} // unnamed namespace

} // namespace
struct View::Impl
{
  VisualModelPtr            mVisualModel;
  LogicalModelPtr           mLogicalModel;
  const FinalElisionResult* mFinalElisionResult{nullptr}; ///< Non-owning authoritative final glyph sequence.
};

View::View()
: mImpl(NULL)
{
  mImpl = new View::Impl();
}

View::~View()
{
  delete mImpl;
}

void View::SetVisualModel(VisualModelPtr visualModel)
{
  mImpl->mVisualModel        = visualModel;
  mImpl->mFinalElisionResult = nullptr;
}

void View::SetLogicalModel(LogicalModelPtr logicalModel)
{
  mImpl->mLogicalModel       = logicalModel;
  mImpl->mFinalElisionResult = nullptr;
}

void View::SetFinalElisionResult(const FinalElisionResult* result)
{
  mImpl->mFinalElisionResult = result;
}

void View::ResolveFinalElision(TextAbstraction::FontClient& fontClient,
                               FinalElisionResult&          result,
                               uint64_t                     layoutGeneration) const
{
  // A completed layout generation owns one immutable final sequence. Render,
  // geometry and replacement consumers may ask for it repeatedly, but they
  // must never run the mutation-based legacy resolver a second time.
  if(result.resolved && result.layoutGeneration == layoutGeneration)
  {
    return;
  }

  result.Clear();
  if(!mImpl->mVisualModel || !mImpl->mLogicalModel)
  {
    return;
  }

  const VisualModel& visualModel = *mImpl->mVisualModel;

  Length ellipsisLineIndex = static_cast<Dali::Ui::Text::Length>(visualModel.mLines.Count());
  for(Length lineIndex = 0u; lineIndex < visualModel.mLines.Count(); ++lineIndex)
  {
    if(visualModel.mLines[lineIndex].ellipsis && ellipsisLineIndex == visualModel.mLines.Count())
    {
      ellipsisLineIndex = lineIndex;
    }
  }

  result.textElided       = ellipsisLineIndex < visualModel.mLines.Count();
  result.layoutGeneration = layoutGeneration;
  result.resolved         = true;
  if(!result.textElided)
  {
    // The render model already owns the authoritative ordinary glyph arrays.
    // An empty resolved result means pass-through and performs no vector allocation.
    return;
  }

  const Length glyphCount = GetNumberOfGlyphs();
  result.sourceToFinalGlyphIndices.Resize(glyphCount);
  Vector<GlyphIndex> finalToSourceGlyphIndices;
  finalToSourceGlyphIndices.Resize(glyphCount);
  for(Length index = 0u; index < glyphCount; ++index)
  {
    result.sourceToFinalGlyphIndices[index] = FinalElisionResult::INVALID_GLYPH_INDEX;
    finalToSourceGlyphIndices[index]        = index;
  }

  result.glyphs.Resize(glyphCount);
  result.viewGlyphPositions.Resize(glyphCount);
  GlyphIndex ellipsisFinalGlyphIndex = FinalElisionResult::INVALID_GLYPH_INDEX;
  if(glyphCount > 0u)
  {
    const Length finalGlyphCount = GetGlyphsUncached(result.glyphs.Begin(),
                                                     result.viewGlyphPositions.Begin(),
                                                     &fontClient,
                                                     result.minimumLineOffset,
                                                     0u,
                                                     glyphCount,
                                                     finalToSourceGlyphIndices.Begin(),
                                                     &ellipsisFinalGlyphIndex,
                                                     true);
    result.glyphs.Resize(finalGlyphCount);
    result.viewGlyphPositions.Resize(finalGlyphCount);
    finalToSourceGlyphIndices.Resize(finalGlyphCount);
  }

  // GetGlyphsUncached is the sole physical ellipsis generator. Read its
  // resulting indices only after that call; values left in VisualModel by a
  // previous generation are not authoritative for this result.
  result.startIndex            = visualModel.GetStartIndexOfElidedGlyphs();
  result.endIndex              = visualModel.GetEndIndexOfElidedGlyphs();
  result.firstMiddleIndex      = visualModel.GetFirstMiddleIndexOfElidedGlyphs();
  result.secondMiddleIndex     = visualModel.GetSecondMiddleIndexOfElidedGlyphs();
  const Length finalGlyphCount = static_cast<Dali::Ui::Text::Length>(result.glyphs.Count());
  if(ellipsisFinalGlyphIndex < finalGlyphCount)
  {
    result.applied                 = true;
    result.ellipsisUnitCount       = 1u;
    result.ellipsisFinalGlyphIndex = ellipsisFinalGlyphIndex;
    result.ellipsisLineIndex       = ellipsisLineIndex;
    result.ellipsisOmissionReason  = FinalElisionResult::EllipsisOmissionReason::NONE;
  }
  else
  {
    result.ellipsisOmissionReason =
      finalGlyphCount == 0u
        ? FinalElisionResult::EllipsisOmissionReason::NO_VISIBLE_LINE
        : FinalElisionResult::EllipsisOmissionReason::ELLIPSIS_CANNOT_FIT;
  }

  // Build the compacted source map once, then visit each source glyph through
  // its LineRun. This keeps the conversion O(sourceGlyphCount +
  // finalGlyphCount + lineCount) without temporary per-glyph line storage.
  result.lineLocalGlyphPositions = result.viewGlyphPositions;
  for(GlyphIndex outputIndex = 0u; outputIndex < finalGlyphCount; ++outputIndex)
  {
    const GlyphIndex sourceIndex = finalToSourceGlyphIndices[outputIndex];
    if(sourceIndex != FinalElisionResult::INVALID_GLYPH_INDEX && sourceIndex < glyphCount)
    {
      result.sourceToFinalGlyphIndices[sourceIndex] = outputIndex;
    }
  }

  const Length lineCount = static_cast<Dali::Ui::Text::Length>(visualModel.mLines.Count());
  float        lineTop   = 0.0f;
  for(LineIndex lineIndex = 0u; lineIndex < lineCount; ++lineIndex)
  {
    const LineRun& line = visualModel.mLines[lineIndex];
    const float    baseline =
      lineTop + line.ascender + GetPreOffsetVerticalLineAlignment(line, GetVerticalLineAlignment());

    auto mapRun = [&](const GlyphRun& run)
    {
      const GlyphIndex end = std::min<GlyphIndex>(run.glyphIndex + run.numberOfGlyphs, glyphCount);
      for(GlyphIndex sourceIndex = run.glyphIndex; sourceIndex < end; ++sourceIndex)
      {
        const GlyphIndex outputIndex = result.sourceToFinalGlyphIndices[sourceIndex];
        if(outputIndex == FinalElisionResult::INVALID_GLYPH_INDEX || outputIndex >= finalGlyphCount)
        {
          continue;
        }
        result.lineLocalGlyphPositions[outputIndex].x -= line.alignmentOffset;
        result.lineLocalGlyphPositions[outputIndex].y -= baseline;
      }
    };
    mapRun(line.glyphRun);
    if(line.isSplitToTwoHalves)
    {
      mapRun(line.glyphRunSecondHalf);
    }

    if(lineIndex == ellipsisLineIndex)
    {
      // GetGlyphsUncached generated final View/atlas coordinates after this
      // exact line alignment. Typesetter consumes line-local coordinates and
      // must add the same offset rather than recomputing another alignment.
      result.elidedOffset = line.alignmentOffset;
      if(ellipsisFinalGlyphIndex < finalGlyphCount)
      {
        result.lineLocalGlyphPositions[ellipsisFinalGlyphIndex].x -= line.alignmentOffset;
        result.lineLocalGlyphPositions[ellipsisFinalGlyphIndex].y -= baseline;
      }
    }
    lineTop += GetLineHeight(line, false);
  }
}

const Vector2& View::GetControlSize() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->mControlSize;
  }

  return Vector2::ZERO;
}

const Vector2& View::GetLayoutSize() const
{
  const FinalElisionResult* finalResult = mImpl->mFinalElisionResult;
  if(finalResult && finalResult->HasAuthoritativeLayout())
  {
    return finalResult->layoutSize;
  }
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->GetLayoutSize();
  }

  return Vector2::ZERO;
}

Length View::GetNumberOfGlyphs() const
{
  const FinalElisionResult* finalResult = mImpl->mFinalElisionResult;
  if(finalResult && finalResult->resolved && finalResult->textElided)
  {
    return static_cast<Length>(finalResult->glyphs.Count());
  }
  if(mImpl->mVisualModel)
  {
    const VisualModel& model = *mImpl->mVisualModel;

    const Length glyphCount    = static_cast<Dali::Ui::Text::Length>(model.mGlyphs.Count());
    const Length positionCount = static_cast<Dali::Ui::Text::Length>(model.mGlyphPositions.Count());

    DALI_ASSERT_DEBUG(positionCount <= glyphCount && "Invalid glyph positions in Model");

    return (positionCount < glyphCount) ? positionCount : glyphCount;
  }

  return 0;
}

Length View::GetGlyphs(GlyphInfo* glyphs, Vector2* glyphPositions, float& minLineOffset, GlyphIndex glyphIndex,
                       Length numberOfGlyphs) const
{
  const FinalElisionResult* finalResult = mImpl->mFinalElisionResult;
  if(finalResult && finalResult->resolved && finalResult->textElided)
  {
    const Length finalCount = static_cast<Dali::Ui::Text::Length>(finalResult->glyphs.Count());
    if(glyphIndex >= finalCount)
    {
      minLineOffset = finalResult->minimumLineOffset;
      return 0u;
    }

    const Length copyCount = std::min(numberOfGlyphs, finalCount - glyphIndex);
    if(copyCount > 0u)
    {
      memcpy(glyphs, finalResult->glyphs.Begin() + glyphIndex, copyCount * sizeof(GlyphInfo));
      memcpy(glyphPositions, finalResult->viewGlyphPositions.Begin() + glyphIndex, copyCount * sizeof(Vector2));
    }
    minLineOffset = finalResult->minimumLineOffset;
    return copyCount;
  }

  return GetGlyphsUncached(glyphs, glyphPositions, nullptr, minLineOffset, glyphIndex, numberOfGlyphs);
}

const GlyphIndex* View::GetFinalGlyphStyleSourceIndices() const
{
  const FinalElisionResult* finalResult = mImpl->mFinalElisionResult;
  return finalResult && finalResult->resolved && !finalResult->finalToStyleGlyphIndices.Empty()
           ? finalResult->finalToStyleGlyphIndices.Begin()
           : nullptr;
}

Length View::GetGlyphsUncached(GlyphInfo*                   glyphs,
                               Vector2*                     glyphPositions,
                               TextAbstraction::FontClient* fontClient,
                               float&                       minLineOffset,
                               GlyphIndex                   glyphIndex,
                               Length                       numberOfGlyphs,
                               GlyphIndex*                  sourceGlyphIndices,
                               GlyphIndex*                  ellipsisFinalGlyphIndex,
                               bool                         hasActiveReplacement) const
{
  TextAbstraction::FontClient localFontClient;
  auto                        getFontClient = [&]() -> TextAbstraction::FontClient&
  {
    if(fontClient)
    {
      return *fontClient;
    }
    if(!localFontClient)
    {
      localFontClient = TextAbstraction::FontClient::Get();
    }
    return localFontClient;
  };

  if(ellipsisFinalGlyphIndex)
  {
    *ellipsisFinalGlyphIndex = FinalElisionResult::INVALID_GLYPH_INDEX;
  }
  Length                  numberOfLaidOutGlyphs       = 0u;
  Length                  numberOfActualLaidOutGlyphs = 0u;
  const float             modelCharacterSpacing       = mImpl->mVisualModel->GetCharacterSpacing();
  Vector<CharacterIndex>& glyphToCharacterMap         = mImpl->mVisualModel->mGlyphsToCharacters;
  const CharacterIndex*   glyphToCharacterMapBuffer   = glyphToCharacterMap.Begin();
  float                   calculatedAdvance           = 0.f;
  const Character*        textBuffer                  = mImpl->mLogicalModel->mText.Begin();

  if(mImpl->mVisualModel)
  {
    // Get the character-spacing runs.
    const Vector<CharacterSpacingGlyphRun>& characterSpacingGlyphRuns =
      mImpl->mVisualModel->GetCharacterSpacingGlyphRuns();

    bool                         textElided       = false;
    Text::EllipsisPosition::Type ellipsisPosition = GetEllipsisPosition();

    // Reset indices of ElidedGlyphs
    mImpl->mVisualModel->SetStartIndexOfElidedGlyphs(0u);
    mImpl->mVisualModel->SetEndIndexOfElidedGlyphs(numberOfGlyphs - 1u); // Initialization is the last index of Glyphs
    mImpl->mVisualModel->SetFirstMiddleIndexOfElidedGlyphs(0u);
    mImpl->mVisualModel->SetSecondMiddleIndexOfElidedGlyphs(0u);

    // If ellipsis is enabled, the number of glyphs the layout engine has laid out may be less than 'numberOfGlyphs'.
    // Check the last laid out line to know if the layout engine elided some text.

    const Length numberOfLines = static_cast<Dali::Ui::Text::Length>(mImpl->mVisualModel->mLines.Count());
    if(numberOfLines > 0u)
    {
      const LineRun* const lines = mImpl->mVisualModel->mLines.Begin();

      // Get line of ellipsis
      const LineRun* ellipsisLine     = nullptr;
      const LineRun* ellipsisNextLine = nullptr;
      bool           hasEllipsis      = false;
      for(Length lineIndex = 0; lineIndex < numberOfLines; lineIndex++)
      {
        const LineRun* line = (lines + lineIndex);
        if(line->ellipsis)
        {
          ellipsisLine = line;
          hasEllipsis  = true;
          if(lineIndex < numberOfLines - 1u)
          {
            ellipsisNextLine = (lines + lineIndex + 1u);
          }
          break;
        }
      }

      CalculateNumberOfLaidOutGlyphes(hasEllipsis, textElided, numberOfLaidOutGlyphs, numberOfActualLaidOutGlyphs,
                                      numberOfGlyphs, ellipsisPosition, ellipsisLine, numberOfLines, lines);

      if(0u < numberOfActualLaidOutGlyphs)
      {
        // Retrieve from the visual model the glyphs and positions.
        mImpl->mVisualModel->GetGlyphs(glyphs, glyphIndex, numberOfLaidOutGlyphs);

        mImpl->mVisualModel->GetGlyphPositions(glyphPositions, glyphIndex, numberOfLaidOutGlyphs);

        // Get the lines for the given range of glyphs.
        // The lines contain the alignment offset which needs to be added to the glyph's position.
        LineIndex firstLineIndex = 0u;
        Length    numberOfLines  = 0u;
        mImpl->mVisualModel->GetNumberOfLines(glyphIndex, numberOfLaidOutGlyphs, firstLineIndex, numberOfLines);

        Vector<LineRun> lines;
        lines.Resize(numberOfLines);
        LineRun* lineBuffer = lines.Begin();

        mImpl->mVisualModel->GetLinesOfGlyphRange(lineBuffer, glyphIndex, numberOfLaidOutGlyphs);

        // Get the first line for the given glyph range.
        LineIndex lineIndex = 0u;
        LineRun*  line      = lineBuffer + lineIndex;

        // Index of the last glyph of the line.
        GlyphIndex lastGlyphIndexOfLine =
          (line->isSplitToTwoHalves ? line->glyphRunSecondHalf.glyphIndex + line->glyphRunSecondHalf.numberOfGlyphs
                                    : line->glyphRun.glyphIndex + line->glyphRun.numberOfGlyphs) -
          1u;

        // Get vertical line alignment for glyph positioning
        const Alignment verticalLineAlignment = GetVerticalLineAlignment();

        // Add the alignment offset to the glyph's position.

        minLineOffset            = line->alignmentOffset;
        float penY               = line->ascender;
        float verticalLineOffset = GetPreOffsetVerticalLineAlignment(*line, verticalLineAlignment);
        for(Length index = 0u; index < numberOfLaidOutGlyphs; ++index)
        {
          Vector2& position = *(glyphPositions + index);
          position.x += line->alignmentOffset;
          position.y += penY + verticalLineOffset;

          const GlyphIndex currentGlyphIndex = glyphIndex + index;
          if(lastGlyphIndexOfLine == currentGlyphIndex)
          {
            penY += -line->descender + line->lineSpacing;

            // Get the next line.
            ++lineIndex;

            if(lineIndex < numberOfLines)
            {
              line          = lineBuffer + lineIndex;
              minLineOffset = std::min(minLineOffset, line->alignmentOffset);

              lastGlyphIndexOfLine =
                (line->isSplitToTwoHalves
                   ? line->glyphRunSecondHalf.glyphIndex + line->glyphRunSecondHalf.numberOfGlyphs
                   : line->glyphRun.glyphIndex + line->glyphRun.numberOfGlyphs) -
                1u;

              penY += line->ascender;
              verticalLineOffset = GetPreOffsetVerticalLineAlignment(*line, verticalLineAlignment);
            }
          }
        }

        // Set index where to set Ellipsis according to the selected position of Ellipsis.
        // Start with this index to replace its glyph by Ellipsis, if the width  is not enough, then remove more glyphs.
        GlyphIndex startIndexOfEllipsis = 0u;
        if(hasEllipsis)
        {
          switch(ellipsisPosition)
          {
            case Text::EllipsisPosition::START:
            {
              // It's the fisrt glyph in line.
              startIndexOfEllipsis = ellipsisLine->glyphRun.glyphIndex;
              break;
            }
            case Text::EllipsisPosition::MIDDLE:
            {
              // It's the second middle of the line in case the line split to two halves.
              // Otherwise it's It's the last glyph in line (line before all removed lines).
              startIndexOfEllipsis =
                ellipsisLine->isSplitToTwoHalves
                  ? (ellipsisLine->glyphRunSecondHalf.glyphIndex)
                  : (ellipsisLine->glyphRun.glyphIndex + ellipsisLine->glyphRun.numberOfGlyphs - 1u);
              break;
            }
            case Text::EllipsisPosition::END:
            {
              // It's the last glyph in line.
              startIndexOfEllipsis = ellipsisLine->glyphRun.glyphIndex + ellipsisLine->glyphRun.numberOfGlyphs - 1u;
              break;
            }
          }
        }

        if(1u == numberOfLaidOutGlyphs)
        {
          // not a point try to do ellipsis with only one laid out character.

          return numberOfLaidOutGlyphs;
        }

        if(textElided)
        {
          const LineRun& elidedLine = *ellipsisLine;

          if((1u == numberOfLines) && (GetLineHeight(elidedLine, true) > mImpl->mVisualModel->mControlSize.height))
          {
            // Replace the first glyph with ellipsis glyph
            auto indexOfFirstGlyph =
              (ellipsisPosition == Text::EllipsisPosition::START) ? startIndexOfEllipsis : 0u;

            // Regardless where the location of ellipsis,in-case the hight of line is greater than control's height
            // then replace the first glyph with ellipsis glyph.

            // Get the first glyph which is going to be replaced and the ellipsis glyph.
            GlyphInfo&                   glyphInfo          = *(glyphs + indexOfFirstGlyph);
            TextAbstraction::FontClient& ellipsisFontClient = getFontClient();
            const FontId                 ellipsisFontId     = hasActiveReplacement
                                                                ? ResolveEllipsisFontId(ellipsisFontClient,
                                                                                        glyphs,
                                                                                        numberOfGlyphs,
                                                                                        indexOfFirstGlyph)
                                                                : glyphInfo.fontId;
            if(0u == ellipsisFontId)
            {
              // A replacement-only line has no font-backed glyph from which
              // an ellipsis can be selected. Keep the atomic box unchanged;
              // callers will still classify it through FinalElisionResult.
              return numberOfLaidOutGlyphs;
            }
            const GlyphInfo& ellipsisGlyph = ellipsisFontClient.GetEllipsisGlyph(ellipsisFontClient.GetPointSize(ellipsisFontId));

            // Change the 'x' and 'y' position of the ellipsis glyph.
            Vector2& position = *(glyphPositions + indexOfFirstGlyph);
            position.x        = ellipsisGlyph.xBearing;
            position.y        = mImpl->mVisualModel->mControlSize.height - ellipsisGlyph.yBearing;

            // Replace the glyph by the ellipsis glyph.
            glyphInfo = ellipsisGlyph;
            if(sourceGlyphIndices)
            {
              sourceGlyphIndices[indexOfFirstGlyph] = FinalElisionResult::INVALID_GLYPH_INDEX;
            }
            if(ellipsisFinalGlyphIndex)
            {
              *ellipsisFinalGlyphIndex = indexOfFirstGlyph;
            }

            mImpl->mVisualModel->SetStartIndexOfElidedGlyphs(indexOfFirstGlyph);
            mImpl->mVisualModel->SetEndIndexOfElidedGlyphs(indexOfFirstGlyph);
            mImpl->mVisualModel->SetFirstMiddleIndexOfElidedGlyphs(indexOfFirstGlyph);
            mImpl->mVisualModel->SetSecondMiddleIndexOfElidedGlyphs(indexOfFirstGlyph);

            numberOfLaidOutGlyphs = 1u;

            return numberOfLaidOutGlyphs;
          }

          // Add the ellipsis glyph. Controller-owned non-replacement END returns
          // from GetGlyphs() with FinalElisionResult before this fallback. END
          // here serves replacement projection and standalone View clients.
          Length     numberOfRemovedGlyphs = 0u;
          GlyphIndex indexOfEllipsis       = startIndexOfEllipsis;

          // Tail Mode: start by the end of line.
          const bool isTailMode = ellipsisPosition == Text::EllipsisPosition::END ||
                                  (ellipsisPosition == Text::EllipsisPosition::MIDDLE && numberOfLines != 1u);

          // The ellipsis glyph has to fit in the place where the last glyph(s) is(are) removed.
          bool ellipsisInserted = false;
          if(ellipsisPosition == Text::EllipsisPosition::END && hasActiveReplacement)
          {
            EndEllipsisInputView input;
            input.glyphs                  = glyphs;
            input.glyphPositions          = glyphPositions;
            input.text                    = textBuffer;
            input.glyphToCharacterMap     = glyphToCharacterMapBuffer;
            input.characterSpacingRuns    = &characterSpacingGlyphRuns;
            input.numberOfGlyphs          = numberOfGlyphs;
            input.glyphPositionStartIndex = 0u;
            input.numberOfGlyphPositions  = numberOfGlyphs;
            input.numberOfCharacters      = static_cast<Dali::Ui::Text::Length>(mImpl->mLogicalModel->mText.Count());
            input.startIndex              = startIndexOfEllipsis;
            input.lineWidth               = ellipsisLine->width;
            input.modelCharacterSpacing   = modelCharacterSpacing;
            const EndEllipsisPlan plan    = ResolveEndEllipsisPlan(input, getFontClient());
            numberOfRemovedGlyphs         = plan.numberOfRemovedGlyphs;
            ellipsisInserted              = plan.resolved;
            if(ellipsisInserted)
            {
              indexOfEllipsis                 = plan.ellipsisGlyphIndex;
              glyphs[indexOfEllipsis]         = plan.ellipsisGlyph;
              glyphPositions[indexOfEllipsis] = plan.ellipsisPosition;
            }
          }
          else
          {
            ellipsisInserted =
              InsertEllipsisGlyph(glyphs, indexOfEllipsis, numberOfRemovedGlyphs, glyphPositions,
                                  getFontClient(), characterSpacingGlyphRuns, modelCharacterSpacing,
                                  calculatedAdvance, textBuffer, glyphToCharacterMapBuffer, numberOfGlyphs,
                                  isTailMode, ellipsisLine, numberOfLaidOutGlyphs, hasActiveReplacement);
          }

          if(sourceGlyphIndices && ellipsisInserted)
          {
            sourceGlyphIndices[indexOfEllipsis] = FinalElisionResult::INVALID_GLYPH_INDEX;
          }

          RemoveAllGlyphsAfterEllipsisGlyph(ellipsisPosition, numberOfLaidOutGlyphs, numberOfActualLaidOutGlyphs,
                                            numberOfRemovedGlyphs, isTailMode, indexOfEllipsis, ellipsisNextLine,
                                            ellipsisLine, glyphs, glyphPositions, numberOfGlyphs, startIndexOfEllipsis,
                                            mImpl->mVisualModel, sourceGlyphIndices,
                                            ellipsisInserted ? ellipsisFinalGlyphIndex : nullptr);
        }
      }
    }
  }

  return numberOfLaidOutGlyphs;
}

const Vector4* View::GetColors() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->mColors.Begin();
  }

  return NULL;
}

const ColorIndex* View::GetColorIndices() const
{
  const FinalElisionResult* finalResult = mImpl->mFinalElisionResult;
  if(finalResult && finalResult->resolved && !finalResult->colorIndices.Empty())
  {
    return finalResult->colorIndices.Begin();
  }
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->mColorIndices.Begin();
  }

  return NULL;
}

const Vector4* View::GetBackgroundColors() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->mBackgroundColors.Begin();
  }

  return nullptr;
}

const ColorIndex* View::GetBackgroundColorIndices() const
{
  const FinalElisionResult* finalResult = mImpl->mFinalElisionResult;
  if(finalResult && finalResult->resolved && !finalResult->backgroundColorIndices.Empty())
  {
    return finalResult->backgroundColorIndices.Begin();
  }
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->mBackgroundColorIndices.Begin();
  }

  return nullptr;
}

bool View::IsMarkupBackgroundColorSet() const
{
  if(mImpl->mVisualModel)
  {
    return (mImpl->mVisualModel->mBackgroundColors.Count() > 0);
  }

  return false;
}

const Vector4& View::GetTextColor() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->GetTextColor();
  }
  return Vector4::ZERO;
}

const Vector2& View::GetShadowOffset() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->GetShadowOffset();
  }
  return Vector2::ZERO;
}

bool View::IsShadowEnabled() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->IsShadowEnabled();
  }
  return false;
}

const Vector4& View::GetShadowColor() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->GetShadowColor();
  }
  return Vector4::ZERO;
}

const Vector4& View::GetUnderlineColor() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->GetUnderlineColor();
  }
  return Vector4::ZERO;
}

bool View::IsUnderlineEnabled() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->IsUnderlineEnabled();
  }
  return false;
}

bool View::IsMarkupUnderlineSet() const
{
  return (GetNumberOfUnderlineRuns() > 0u);
}

const GlyphInfo* View::GetHyphens() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->mHyphen.glyph.Begin();
  }

  return nullptr;
}

const Length* View::GetHyphenIndices() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->mHyphen.index.Begin();
  }

  return nullptr;
}

Length View::GetHyphensCount() const
{
  if(mImpl->mVisualModel)
  {
    return static_cast<Dali::Ui::Text::Length>(mImpl->mVisualModel->mHyphen.glyph.Size());
  }

  return 0;
}
float View::GetUnderlineHeight() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->GetUnderlineHeight();
  }
  return 0.0f;
}

Text::Underline::Type View::GetUnderlineType() const
{
  Text::Underline::Type type = Text::Underline::Type::SOLID;
  if(mImpl->mVisualModel)
  {
    type = mImpl->mVisualModel->GetUnderlineType();
  }
  return type;
}

float View::GetDashedUnderlineWidth() const
{
  float width = 0.0f;
  if(mImpl->mVisualModel)
  {
    width = mImpl->mVisualModel->GetDashedUnderlineWidth();
  }
  return width;
}

float View::GetDashedUnderlineGap() const
{
  float gap = 0.0f;
  if(mImpl->mVisualModel)
  {
    gap = mImpl->mVisualModel->GetDashedUnderlineGap();
  }
  return gap;
}

Length View::GetNumberOfUnderlineRuns() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->GetNumberOfUnderlineRuns();
  }

  return 0u;
}

void View::GetUnderlineRuns(UnderlinedGlyphRun* underlineRuns, UnderlineRunIndex index, Length numberOfRuns) const
{
  if(mImpl->mVisualModel)
  {
    mImpl->mVisualModel->GetUnderlineRuns(underlineRuns, index, numberOfRuns);
  }
}

const Vector2& View::GetOutlineOffset() const
{
  // TODO : We should support outline offset to editable text.
  /*
    if(mImpl->mVisualModel)
    {
      return mImpl->mVisualModel->GetOutlineOffset();
    }
  */
  return Vector2::ZERO;
}

const Vector4& View::GetOutlineColor() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->GetOutlineColor();
  }
  return Vector4::ZERO;
}

uint16_t View::GetOutlineWidth() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->GetOutlineWidth();
  }
  return 0u;
}

bool View::IsOutlineEnabled() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->IsOutlineEnabled();
  }
  return false;
}

Text::EllipsisPosition::Type View::GetEllipsisPosition() const
{
  Text::EllipsisPosition::Type ellipsisPosition = Text::EllipsisPosition::END;
  if(mImpl->mVisualModel)
  {
    const VisualModel& model = *mImpl->mVisualModel;
    ellipsisPosition         = model.GetEllipsisPosition();
  }

  return ellipsisPosition;
}

bool View::IsTextElideEnabled() const
{
  bool isTextElideEnabled = false;

  if(mImpl->mVisualModel)
  {
    const VisualModel& model = *mImpl->mVisualModel;
    isTextElideEnabled       = model.IsTextElideEnabled();
  }

  return isTextElideEnabled;
}

GlyphIndex View::GetStartIndexOfElidedGlyphs() const
{
  GlyphIndex startIndexOfElidedGlyphs = 0u;

  if(mImpl->mVisualModel)
  {
    const VisualModel& model = *mImpl->mVisualModel;
    startIndexOfElidedGlyphs = model.GetStartIndexOfElidedGlyphs();
  }

  return startIndexOfElidedGlyphs;
}

GlyphIndex View::GetEndIndexOfElidedGlyphs() const
{
  GlyphIndex endIndexOfElidedGlyphs = 0u;

  if(mImpl->mVisualModel)
  {
    const VisualModel& model = *mImpl->mVisualModel;
    endIndexOfElidedGlyphs   = model.GetEndIndexOfElidedGlyphs();
  }

  return endIndexOfElidedGlyphs;
}

GlyphIndex View::GetFirstMiddleIndexOfElidedGlyphs() const
{
  GlyphIndex firstMiddleIndexOfElidedGlyphs = 0u;

  if(mImpl->mVisualModel)
  {
    const VisualModel& model       = *mImpl->mVisualModel;
    firstMiddleIndexOfElidedGlyphs = model.GetFirstMiddleIndexOfElidedGlyphs();
  }

  return firstMiddleIndexOfElidedGlyphs;
}

GlyphIndex View::GetSecondMiddleIndexOfElidedGlyphs() const
{
  GlyphIndex secondMiddleIndexOfElidedGlyphs = 0u;

  if(mImpl->mVisualModel)
  {
    const VisualModel& model        = *mImpl->mVisualModel;
    secondMiddleIndexOfElidedGlyphs = model.GetSecondMiddleIndexOfElidedGlyphs();
  }

  return secondMiddleIndexOfElidedGlyphs;
}

const Vector4& View::GetStrikethroughColor() const
{
  return (mImpl->mVisualModel) ? mImpl->mVisualModel->GetStrikethroughColor() : Vector4::ZERO;
}

bool View::IsStrikethroughEnabled() const
{
  return (mImpl->mVisualModel) ? mImpl->mVisualModel->IsStrikethroughEnabled() : false;
}

bool View::IsMarkupStrikethroughSet() const
{
  return (GetNumberOfStrikethroughRuns() > 0u);
}

float View::GetStrikethroughHeight() const
{
  return (mImpl->mVisualModel) ? mImpl->mVisualModel->GetStrikethroughHeight() : 0.0f;
}

Length View::GetNumberOfStrikethroughRuns() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->GetNumberOfStrikethroughRuns();
  }

  return 0u;
}

void View::GetStrikethroughRuns(StrikethroughGlyphRun* strikethroughRuns, StrikethroughRunIndex index,
                                Length numberOfRuns) const
{
  if(mImpl->mVisualModel)
  {
    mImpl->mVisualModel->GetStrikethroughRuns(strikethroughRuns, index, numberOfRuns);
  }
}

Length View::GetNumberOfBoundedParagraphRuns() const
{
  if(mImpl->mLogicalModel)
  {
    return mImpl->mLogicalModel->GetNumberOfBoundedParagraphRuns();
  }

  return 0u;
}

const Vector<BoundedParagraphRun>& View::GetBoundedParagraphRuns() const
{
  return mImpl->mLogicalModel->GetBoundedParagraphRuns();
}

float View::GetCharacterSpacing() const
{
  return mImpl->mVisualModel->GetCharacterSpacing();
}

const Character* View::GetTextBuffer() const
{
  return mImpl->mLogicalModel->mText.Begin();
}

const Vector<CharacterIndex>& View::GetGlyphsToCharacters() const
{
  return mImpl->mVisualModel->GetGlyphsToCharacters();
}

bool View::IsCutoutEnabled() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->IsCutoutEnabled();
  }
  return false;
}

Alignment View::GetVerticalLineAlignment() const
{
  if(mImpl->mVisualModel)
  {
    return mImpl->mVisualModel->GetVerticalLineAlignment();
  }
  return Alignment::CENTER;
}

} // namespace Dali::Ui::Text
