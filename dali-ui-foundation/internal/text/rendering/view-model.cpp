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
#include <memory.h>
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/final-elision-result.h>
#include <dali-ui-foundation/internal/text/glyph-metrics-helper.h>
#include <dali-ui-foundation/internal/text/line-run.h>
#include <dali-ui-foundation/internal/text/rendering/view-model.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-run-snapshot.h>

namespace Dali
{
namespace Ui
{
namespace Text
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

  // A replacement is deliberately not backed by a font glyph. If END
  // ellipsis lands on it, use the closest surrounding text font so the
  // replacement box itself can be exchanged for the ellipsis glyph. Looking
  // backward first follows the existing tail-elision typography.
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
} // unnamed namespace

ViewModel::ViewModel(const ModelInterface* const model)
: mModel(model),
  mFinalElisionResult(nullptr),
  mElidedGlyphs(),
  mElidedLayout(),
  mElidedFinalToSourceGlyphIndices(),
  mEllipsisFinalGlyphIndex(FinalElisionResult::INVALID_GLYPH_INDEX),
  mFinalGlyphMappingRequested(false),
  mIsTextElided(false),
  mElidedOffset(0.0f),
  mStartIndexOfElidedGlyphs(0u),
  mEndIndexOfElidedGlyphs(0u),
  mFirstMiddleIndexOfElidedGlyphs(0u),
  mSecondMiddleIndexOfElidedGlyphs(0u)
{
}

ViewModel::~ViewModel()
{
}

void ViewModel::SetModel(const ModelInterface* model)
{
  mFinalElisionResult = nullptr;
  if(mModel != model)
  {
    mModel = model;
    mElidedGlyphs.Clear();
    mElidedLayout.Clear();
    mElidedFinalToSourceGlyphIndices.Clear();
    mEllipsisFinalGlyphIndex         = FinalElisionResult::INVALID_GLYPH_INDEX;
    mFinalGlyphMappingRequested      = false;
    mIsTextElided                    = false;
    mElidedOffset                    = 0.0f;
    mStartIndexOfElidedGlyphs        = 0u;
    mEndIndexOfElidedGlyphs          = 0u;
    mFirstMiddleIndexOfElidedGlyphs  = 0u;
    mSecondMiddleIndexOfElidedGlyphs = 0u;
  }
}

void ViewModel::SetFinalElisionResult(const FinalElisionResult* result)
{
  mFinalElisionResult = result;
}

const GlyphIndex* ViewModel::GetFinalGlyphStyleSourceIndices() const
{
  return mFinalElisionResult && mFinalElisionResult->resolved &&
             !mFinalElisionResult->finalToStyleGlyphIndices.Empty()
           ? mFinalElisionResult->finalToStyleGlyphIndices.Begin()
           : nullptr;
}

void ViewModel::EnableFinalGlyphMapping()
{
  mFinalGlyphMappingRequested = true;
}

const GlyphIndex* ViewModel::GetFinalToSourceGlyphIndices() const
{
  return mIsTextElided && !mElidedFinalToSourceGlyphIndices.Empty()
           ? mElidedFinalToSourceGlyphIndices.Begin()
           : nullptr;
}

GlyphIndex ViewModel::GetEllipsisFinalGlyphIndex() const
{
  return mFinalElisionResult && mFinalElisionResult->resolved
           ? mFinalElisionResult->ellipsisFinalGlyphIndex
           : mEllipsisFinalGlyphIndex;
}

const Size& ViewModel::GetControlSize() const
{
  return mModel->GetControlSize();
}

const Size& ViewModel::GetLayoutSize() const
{
  if(mFinalElisionResult && mFinalElisionResult->HasAuthoritativeLayout())
  {
    return mFinalElisionResult->layoutSize;
  }
  return mModel->GetLayoutSize();
}

const Vector2& ViewModel::GetScrollPosition() const
{
  return mModel->GetScrollPosition();
}

Alignment ViewModel::GetHorizontalAlignment() const
{
  return mModel->GetHorizontalAlignment();
}

Alignment ViewModel::GetVerticalAlignment() const
{
  return mModel->GetVerticalAlignment();
}

Alignment ViewModel::GetVerticalLineAlignment() const
{
  return mModel->GetVerticalLineAlignment();
}

Text::EllipsisPosition::Type ViewModel::GetEllipsisPosition() const
{
  return mModel->GetEllipsisPosition();
}

bool ViewModel::IsTextElideEnabled() const
{
  return mModel->IsTextElideEnabled();
}

Length ViewModel::GetNumberOfLines() const
{
  return mFinalElisionResult && mFinalElisionResult->resolved && mFinalElisionResult->textElided &&
             mFinalElisionResult->authoritativeLines
           ? static_cast<Length>(mFinalElisionResult->lines.Count())
           : mModel->GetNumberOfLines();
}

const LineRun* ViewModel::GetLines() const
{
  return mFinalElisionResult && mFinalElisionResult->resolved && mFinalElisionResult->textElided &&
             mFinalElisionResult->authoritativeLines
           ? mFinalElisionResult->lines.Begin()
           : mModel->GetLines();
}

Length ViewModel::GetNumberOfScripts() const
{
  return mModel->GetNumberOfScripts();
}

const ScriptRun* ViewModel::GetScriptRuns() const
{
  return mModel->GetScriptRuns();
}

Length ViewModel::GetNumberOfCharacters() const
{
  return mModel->GetNumberOfCharacters();
}

Length ViewModel::GetNumberOfGlyphs() const
{
  if(mIsTextElided && mModel->IsTextElideEnabled())
  {
    return mFinalElisionResult ? static_cast<Length>(mFinalElisionResult->glyphs.Count()) : static_cast<Length>(mElidedGlyphs.Count());
  }
  else
  {
    return mModel->GetNumberOfGlyphs();
  }

  return 0u;
}

GlyphIndex ViewModel::GetStartIndexOfElidedGlyphs() const
{
  if(mIsTextElided && mModel->IsTextElideEnabled())
  {
    return mStartIndexOfElidedGlyphs;
  }

  return mModel->GetStartIndexOfElidedGlyphs();
}

GlyphIndex ViewModel::GetEndIndexOfElidedGlyphs() const
{
  if(mIsTextElided && mModel->IsTextElideEnabled())
  {
    return mEndIndexOfElidedGlyphs;
  }

  return mModel->GetEndIndexOfElidedGlyphs();
}

GlyphIndex ViewModel::GetFirstMiddleIndexOfElidedGlyphs() const
{
  if(mIsTextElided && mModel->IsTextElideEnabled())
  {
    return mFirstMiddleIndexOfElidedGlyphs;
  }

  return mModel->GetFirstMiddleIndexOfElidedGlyphs();
}

GlyphIndex ViewModel::GetSecondMiddleIndexOfElidedGlyphs() const
{
  if(mIsTextElided && mModel->IsTextElideEnabled())
  {
    return mSecondMiddleIndexOfElidedGlyphs;
  }

  return mModel->GetSecondMiddleIndexOfElidedGlyphs();
}

const GlyphInfo* ViewModel::GetGlyphs() const
{
  if(mIsTextElided && mModel->IsTextElideEnabled())
  {
    return mFinalElisionResult ? mFinalElisionResult->glyphs.Begin() : mElidedGlyphs.Begin();
  }
  else
  {
    return mModel->GetGlyphs();
  }

  return NULL;
}

const Vector2* ViewModel::GetLayout() const
{
  if(mIsTextElided && mModel->IsTextElideEnabled())
  {
    return mFinalElisionResult ? mFinalElisionResult->lineLocalGlyphPositions.Begin() : mElidedLayout.Begin();
  }
  else
  {
    return mModel->GetLayout();
  }

  return NULL;
}

const Vector4* ViewModel::GetColors() const
{
  return mModel->GetColors();
}

const ColorIndex* ViewModel::GetColorIndices() const
{
  return mFinalElisionResult && mFinalElisionResult->resolved && !mFinalElisionResult->colorIndices.Empty()
           ? mFinalElisionResult->colorIndices.Begin()
           : mModel->GetColorIndices();
}

const Vector4* ViewModel::GetBackgroundColors() const
{
  return mModel->GetBackgroundColors();
}

const ColorIndex* ViewModel::GetBackgroundColorIndices() const
{
  return mFinalElisionResult && mFinalElisionResult->resolved && !mFinalElisionResult->backgroundColorIndices.Empty()
           ? mFinalElisionResult->backgroundColorIndices.Begin()
           : mModel->GetBackgroundColorIndices();
}

bool ViewModel::IsMarkupBackgroundColorSet() const
{
  return mModel->IsMarkupBackgroundColorSet();
}

const Vector4& ViewModel::GetDefaultColor() const
{
  return mModel->GetDefaultColor();
}

const Vector2& ViewModel::GetShadowOffset() const
{
  return mModel->GetShadowOffset();
}

bool ViewModel::IsShadowEnabled() const
{
  return mModel->IsShadowEnabled();
}

const Vector4& ViewModel::GetShadowColor() const
{
  return mModel->GetShadowColor();
}

const float& ViewModel::GetShadowBlurRadius() const
{
  return mModel->GetShadowBlurRadius();
}

const Vector4& ViewModel::GetUnderlineColor() const
{
  return mModel->GetUnderlineColor();
}

bool ViewModel::IsUnderlineEnabled() const
{
  return mModel->IsUnderlineEnabled();
}

bool ViewModel::IsMarkupUnderlineSet() const
{
  return mModel->IsMarkupUnderlineSet();
}

float ViewModel::GetUnderlineHeight() const
{
  return mModel->GetUnderlineHeight();
}

Text::Underline::Type ViewModel::GetUnderlineType() const
{
  return mModel->GetUnderlineType();
}

float ViewModel::GetDashedUnderlineWidth() const
{
  return mModel->GetDashedUnderlineWidth();
}

float ViewModel::GetDashedUnderlineGap() const
{
  return mModel->GetDashedUnderlineGap();
}

Length ViewModel::GetNumberOfUnderlineRuns() const
{
  return mModel->GetNumberOfUnderlineRuns();
}

void ViewModel::GetUnderlineRuns(UnderlinedGlyphRun* underlineRuns, UnderlineRunIndex index, Length numberOfRuns) const
{
  mModel->GetUnderlineRuns(underlineRuns, index, numberOfRuns);
}

const Vector2& ViewModel::GetOutlineOffset() const
{
  return mModel->GetOutlineOffset();
}

const Vector4& ViewModel::GetOutlineColor() const
{
  return mModel->GetOutlineColor();
}

uint16_t ViewModel::GetOutlineWidth() const
{
  return mModel->GetOutlineWidth();
}

bool ViewModel::IsOutlineEnabled() const
{
  return mModel->IsOutlineEnabled();
}

const float& ViewModel::GetOutlineBlurRadius() const
{
  return mModel->GetOutlineBlurRadius();
}

const Vector4& ViewModel::GetBackgroundColor() const
{
  return mModel->GetBackgroundColor();
}

bool ViewModel::IsBackgroundEnabled() const
{
  return mModel->IsBackgroundEnabled();
}

const GlyphInfo* ViewModel::GetHyphens() const
{
  return mModel->GetHyphens();
}

const Length* ViewModel::GetHyphenIndices() const
{
  return mModel->GetHyphenIndices();
}

Length ViewModel::GetHyphensCount() const
{
  return mModel->GetHyphensCount();
}

float ViewModel::GetCharacterSpacing() const
{
  return mModel->GetCharacterSpacing();
}

const Character* ViewModel::GetTextBuffer() const
{
  return mModel->GetTextBuffer();
}

const Vector<CharacterIndex>& ViewModel::GetGlyphsToCharacters() const
{
  return mModel->GetGlyphsToCharacters();
}

const float ViewModel::GetElidedOffset() const
{
  return mElidedOffset;
}

const bool ViewModel::GetCharacterDirection(CharacterIndex logicalIndex) const
{
  auto   directions = mModel->GetCharacterDirections();
  size_t length     = directions.Count();
  if(length == 0u || length <= logicalIndex)
  {
    return false;
  }
  return directions[logicalIndex];
}

void ViewModel::ElideGlyphs(TextAbstraction::FontClient& fontClient)
{
  // Mapping is a one-shot consumer request. Consume it before every return so
  // enabling Reveal can never turn later ordinary elision into a mapping path.
  const bool finalGlyphMappingRequested = mFinalGlyphMappingRequested;
  mFinalGlyphMappingRequested           = false;

  mIsTextElided            = false;
  mEllipsisFinalGlyphIndex = FinalElisionResult::INVALID_GLYPH_INDEX;
  mElidedFinalToSourceGlyphIndices.Clear();
  mStartIndexOfElidedGlyphs = mFirstMiddleIndexOfElidedGlyphs = mSecondMiddleIndexOfElidedGlyphs = 0;
  mEndIndexOfElidedGlyphs                                                                        = mModel->GetNumberOfGlyphs() == 0u ? 0u : mModel->GetNumberOfGlyphs() - 1u;

  if(mFinalElisionResult && mFinalElisionResult->resolved)
  {
    mIsTextElided                    = mFinalElisionResult->textElided;
    mElidedOffset                    = mFinalElisionResult->elidedOffset;
    mStartIndexOfElidedGlyphs        = mFinalElisionResult->startIndex;
    mEndIndexOfElidedGlyphs          = mFinalElisionResult->endIndex;
    mFirstMiddleIndexOfElidedGlyphs  = mFinalElisionResult->firstMiddleIndex;
    mSecondMiddleIndexOfElidedGlyphs = mFinalElisionResult->secondMiddleIndex;
    if(finalGlyphMappingRequested && mIsTextElided)
    {
      const Length finalGlyphCount = static_cast<Length>(mFinalElisionResult->glyphs.Count());
      mElidedFinalToSourceGlyphIndices.Resize(finalGlyphCount);
      if(mFinalElisionResult->finalToSourceGlyphIndices.Count() == finalGlyphCount)
      {
        std::copy(mFinalElisionResult->finalToSourceGlyphIndices.Begin(),
                  mFinalElisionResult->finalToSourceGlyphIndices.End(),
                  mElidedFinalToSourceGlyphIndices.Begin());
      }
      else
      {
        std::fill(mElidedFinalToSourceGlyphIndices.Begin(),
                  mElidedFinalToSourceGlyphIndices.End(),
                  FinalElisionResult::INVALID_GLYPH_INDEX);
        for(GlyphIndex sourceGlyph = 0u;
            sourceGlyph < mFinalElisionResult->sourceToFinalGlyphIndices.Count();
            ++sourceGlyph)
        {
          const GlyphIndex finalGlyph = mFinalElisionResult->sourceToFinalGlyphIndices[sourceGlyph];
          if(finalGlyph < finalGlyphCount)
          {
            mElidedFinalToSourceGlyphIndices[finalGlyph] = sourceGlyph;
          }
        }
      }
    }
    return;
  }

  // START/MIDDLE retain the renderer-owned ellipsis path. Controller and async
  // non-replacement END supply FinalElisionResult above; the END branch below
  // remains only for standalone ModelInterface clients outside that pipeline.
  auto                          ellipsisPosition          = GetEllipsisPosition();
  auto                          characterSpacing          = GetCharacterSpacing();
  const Character*              textBuffer                = GetTextBuffer();
  const Vector<CharacterIndex>& glyphToCharacterMap       = GetGlyphsToCharacters();
  const CharacterIndex*         glyphToCharacterMapBuffer = glyphToCharacterMap.Begin();
  float                         calculatedAdvance         = 0.f;

  if(IsTextElideEnabled())
  {
    const Length numberOfLines = mModel->GetNumberOfLines();
    if(0u != numberOfLines)
    {
      const LineRun* const lines = mModel->GetLines();

      // Get line of ellipsis
      const LineRun* ellipsisLine     = nullptr;
      const LineRun* ellipsisNextLine = nullptr;

      for(Length lineIndex = 0; lineIndex < numberOfLines; lineIndex++)
      {
        const LineRun* line = (lines + lineIndex);
        if(line->ellipsis)
        {
          ellipsisLine = line;
          if(lineIndex < numberOfLines - 1u)
          {
            ellipsisNextLine = (lines + lineIndex + 1u);
          }
          break;
        }
      }

      // Check if there is a line contains Ellipsis.
      // Then find total number of glyphs and total number of laid out glyphs.
      // Check where to set Ellipsis glyph in line.
      // Determine index of Ellipsis glyph and how many glyphs should be replaced by Ellipsis glyph, according to width
      // of Ellipsis glyph.
      if(ellipsisLine != nullptr)
      {
        // Total number of glyphs.
        const Length numberOfGlyphs = mModel->GetNumberOfGlyphs();
        // Total number of laid out glyphs.
        Length numberOfActualLaidOutGlyphs = 0u;

        // Accumulate laid out glyphs for each line to find total number of laid out glyphs.
        for(Length lineIndex = 0u; lineIndex < numberOfLines; lineIndex++)
        {
          numberOfActualLaidOutGlyphs +=
            lines[lineIndex].glyphRun.numberOfGlyphs + lines[lineIndex].glyphRunSecondHalf.numberOfGlyphs;
        }

        // Make sure there are laid out glyphs.
        {
          // There are elided glyphs.
          mIsTextElided = true;

          // Retrieve the whole glyphs and their positions.
          const GlyphInfo* const glyphs    = mModel->GetGlyphs();
          const Vector2* const   positions = mModel->GetLayout();

          // Copy the glyphs to be elided.
          mElidedGlyphs.Resize(numberOfGlyphs);
          mElidedLayout.Resize(numberOfGlyphs);
          if(finalGlyphMappingRequested)
          {
            mElidedFinalToSourceGlyphIndices.Resize(numberOfGlyphs);
            for(GlyphIndex glyphIndex = 0u; glyphIndex < numberOfGlyphs; ++glyphIndex)
            {
              mElidedFinalToSourceGlyphIndices[glyphIndex] = glyphIndex;
            }
          }
          GlyphInfo* elidedGlyphsBuffer    = mElidedGlyphs.Begin();
          Vector2*   elidedPositionsBuffer = mElidedLayout.Begin();

          memcpy(elidedGlyphsBuffer, glyphs, numberOfGlyphs * sizeof(GlyphInfo));
          memcpy(elidedPositionsBuffer, positions, numberOfGlyphs * sizeof(Vector2));

          // Set index where to set Ellipsis according to the selected position of Ellipsis.
          // Start with this index to replace its glyph by Ellipsis, if the width  is not enough, then remove more
          // glyphs.
          GlyphIndex startIndexOfEllipsis = 0u;
          if(ellipsisPosition == Text::EllipsisPosition::START)
          {
            // It's the fisrt glyph in line.
            startIndexOfEllipsis = ellipsisLine->glyphRun.glyphIndex;
          }
          else if(ellipsisPosition == Text::EllipsisPosition::MIDDLE)
          {
            // It's the second middle of the line in case the line split to two halves.
            // Otherwise it's It's the last glyph in line (line before all removed lines).
            startIndexOfEllipsis =
              ellipsisLine->isSplitToTwoHalves
                ? (ellipsisLine->glyphRunSecondHalf.glyphIndex)
                : (ellipsisLine->glyphRun.glyphIndex + ellipsisLine->glyphRun.numberOfGlyphs - 1u);
          }
          else // Text::EllipsisPosition::END
          {
            // It's the last glyph in line.
            startIndexOfEllipsis =
              (ellipsisLine->glyphRun.glyphIndex + ellipsisLine->glyphRun.numberOfGlyphs) -
              ((ellipsisLine->glyphRun.glyphIndex + ellipsisLine->glyphRun.numberOfGlyphs) > 0u ? 1u : 0u);
          }

          Length EllidedLineIndex                 = 0u;
          Length numberOfActualLaidOutGlyphsCount = 0u;
          for(Length lineIndex = 0u; lineIndex < numberOfLines; lineIndex++)
          {
            numberOfActualLaidOutGlyphsCount +=
              lines[lineIndex].glyphRun.numberOfGlyphs + lines[lineIndex].glyphRunSecondHalf.numberOfGlyphs;
            if(startIndexOfEllipsis < numberOfActualLaidOutGlyphsCount)
            {
              EllidedLineIndex = lineIndex;
              break;
            }
          }

          // firstPenX, penY and firstPenSet are used to position the ellipsis glyph if needed.
          float firstPenX   = 0.f; // Used if rtl text is elided.
          float penY        = 0.f;
          bool  firstPenSet = false;

          // Add the ellipsis glyph.
          bool       inserted              = false;
          float      removedGlypsWidth     = 0.f;
          Length     numberOfRemovedGlyphs = 0u;
          GlyphIndex indexOfEllipsis       = startIndexOfEllipsis;
          float      actualAdvance         = 0.f;
          for(Length i = 0; i < ellipsisLine->glyphRun.numberOfGlyphs; i++)
          {
            const GlyphInfo& currentGlyph   = *(elidedGlyphsBuffer + lines[EllidedLineIndex].glyphRun.glyphIndex + i);
            float            currentAdvance = currentGlyph.advance;
            actualAdvance += currentAdvance;
          }

          // Tail Mode: start by the end of line.
          bool isTailMode = (ellipsisPosition == Text::EllipsisPosition::END) ||
                            (ellipsisPosition == Text::EllipsisPosition::MIDDLE && numberOfLines != 1u);

          // The ellipsis glyph has to fit in the place where the last glyph(s) is(are) removed.
          while(!inserted)
          {
            const GlyphInfo& glyphToRemove = *(elidedGlyphsBuffer + indexOfEllipsis);

            // Every removed glyph contributes to the fit calculation, even
            // if it has no FontClient glyph. Synthetic replacements use
            // fontId 0 but can have a large non-zero advance. Keeping that
            // advance here leaves a replacement-sized hole before ellipsis.
            calculatedAdvance = GetCalculatedAdvance(*(textBuffer + (*(glyphToCharacterMapBuffer + indexOfEllipsis))),
                                                     characterSpacing, glyphToRemove.advance);
            removedGlypsWidth += calculatedAdvance;
            actualAdvance -= glyphToRemove.advance;

            const FontId ellipsisFontId = FindEllipsisFontId(elidedGlyphsBuffer, numberOfGlyphs, indexOfEllipsis);

            if(0u != ellipsisFontId)
            {
              // Need to reshape the glyph as the font may be different in size.
              const GlyphInfo& ellipsisGlyph =
                fontClient.GetEllipsisGlyph(fontClient.GetPointSize(ellipsisFontId));

              if(!firstPenSet || EqualsZero(glyphToRemove.advance))
              {
                const Vector2& position = *(elidedPositionsBuffer + indexOfEllipsis);

                // Calculates the penY of the current line. It will be used to position the ellipsis glyph.
                penY = position.y + glyphToRemove.yBearing;

                // Calculates the first penX which will be used if rtl text is elided.
                firstPenX = position.x - glyphToRemove.xBearing;
                if(firstPenX < -ellipsisGlyph.xBearing)
                {
                  // Avoids to exceed the bounding box when rtl text is elided.
                  firstPenX = -ellipsisGlyph.xBearing;
                }
                firstPenSet = true;
              }

              // Calculate the width of the ellipsis glyph and check if it fits.
              const float ellipsisGlyphWidth = ellipsisGlyph.advance;

              float calculatedWidth = actualAdvance + ellipsisGlyphWidth;

              // For Marquee, there are cases where the layout is larger than the control.
              // If the layout is larger than the max texture, it should be ellipsised.
              const float controlWidth = std::max(mModel->GetControlSize().width, mModel->GetLayoutSize().width);

              bool isRTLLine = ellipsisLine->direction;
              // If it is the last glyph to remove, add the ellipsis glyph without checking its width.
              if((calculatedWidth <= controlWidth) ||
                 (isTailMode ? (indexOfEllipsis == 0u) : (indexOfEllipsis == numberOfGlyphs - 1u)))
              {
                switch(mModel->GetHorizontalAlignment())
                {
                  case Alignment::START:
                  {
                    mElidedOffset = isRTLLine ? ellipsisLine->alignmentOffset : 0.0f;
                    break;
                  }
                  case Alignment::CENTER:
                  {
                    mElidedOffset = isRTLLine ? controlWidth - ellipsisLine->width + ellipsisLine->alignmentOffset
                                              : controlWidth - calculatedWidth;
                    mElidedOffset = std::floor(mElidedOffset * 0.5f);
                    break;
                  }
                  case Alignment::END:
                  {
                    mElidedOffset = isRTLLine ? controlWidth - ellipsisLine->width + ellipsisLine->alignmentOffset
                                              : controlWidth - calculatedWidth;
                    break;
                  }
                }

                GlyphInfo& glyphInfo = *(elidedGlyphsBuffer + indexOfEllipsis);
                Vector2&   position  = *(elidedPositionsBuffer + indexOfEllipsis);
                position.x -= glyphInfo.xBearing;

                // Replace the glyph by the ellipsis glyph.
                glyphInfo = ellipsisGlyph;
                if(finalGlyphMappingRequested)
                {
                  mElidedFinalToSourceGlyphIndices[indexOfEllipsis] = FinalElisionResult::INVALID_GLYPH_INDEX;
                }

                // Change the 'x' and 'y' position of the ellipsis glyph.
                if(position.x >= firstPenX)
                {
                  if(isTailMode)
                  {
                    // To handle case of the mixed languages (LTR then RTL) with
                    // EllipsisPosition::END and the LayoutDirection::RIGHT_TO_LEFT
                    float nextXPositions = ellipsisLine->width;

                    if(indexOfEllipsis + 1u < numberOfGlyphs)
                    {
                      Vector2& positionOfNextGlyph = *(elidedPositionsBuffer + indexOfEllipsis + 1u);
                      nextXPositions               = positionOfNextGlyph.x;
                    }

                    if(GetCharacterDirection(glyphToCharacterMapBuffer[indexOfEllipsis])) // RTL character
                    {
                      if((indexOfEllipsis > 0u) && ((position.x - nextXPositions) > removedGlypsWidth))
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
                        removedGlypsWidth =
                          std::min(calculatedAdvance, (glyphToRemove.xBearing + glyphToRemove.width)) -
                          ellipsisGlyph.xBearing;

                        --indexOfEllipsis;
                        continue;
                      }
                      else
                      {
                        // To handle the case of RTL language with EllipsisPosition::END
                        position.x = firstPenX + removedGlypsWidth - ellipsisGlyphWidth;
                      }
                    }
                  }
                  else
                  {
                    // To handle the case of LTR language with EllipsisPosition::START
                    position.x = firstPenX + removedGlypsWidth - ellipsisGlyphWidth;
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
                      Vector2& positionOfNextGlyph = *(elidedPositionsBuffer + indexOfEllipsis + 1u);
                      nextXPositions               = positionOfNextGlyph.x;
                    }

                    if(!GetCharacterDirection(glyphToCharacterMapBuffer[indexOfEllipsis])) // LTR Character
                    {
                      position.x = firstPenX + removedGlypsWidth - ellipsisGlyphWidth;

                      if((position.x + ellipsisGlyphWidth + ellipsisGlyph.xBearing) > nextXPositions)
                      {
                        position.x -= (position.x + ellipsisGlyphWidth + ellipsisGlyph.xBearing) - nextXPositions;
                      }
                    }
                  }
                }

                position.x += ellipsisGlyph.xBearing;
                position.y    = penY - ellipsisGlyph.yBearing;
                mElidedOffset = position.x + mElidedOffset < 0.0f ? -position.x : mElidedOffset;
                inserted      = true;
              }
            }

            if(!inserted)
            {
              if(!isTailMode && indexOfEllipsis < numberOfGlyphs - 1u)
              {
                // Not Tail Mode: remove glyphs from startIndexOfEllipsis then increase indexOfEllipsis, until arrive to
                // last index (numberOfGlyphs - 1u).
                ++indexOfEllipsis;
              }
              else if(isTailMode && indexOfEllipsis > 0u)
              {
                // Tail Mode: remove glyphs from startIndexOfEllipsis then decrement indexOfEllipsis, until arrive to
                // index zero.
                --indexOfEllipsis;
              }
              else
              {
                // No space for the ellipsis.
                inserted = true;
              }
              ++numberOfRemovedGlyphs;
            }
          } // while( !inserted )

          // Reduce size, shift glyphs and start from ellipsis glyph
          Length numberOfElidedGlyphs = numberOfActualLaidOutGlyphs - numberOfRemovedGlyphs;
          mElidedGlyphs.Resize(numberOfElidedGlyphs);
          mElidedLayout.Resize(numberOfElidedGlyphs);

          if(ellipsisPosition == Text::EllipsisPosition::START)
          {
            // 'Shifts' glyphs after ellipsis glyph and 'Removes' before ellipsis glyph
            GlyphMemmove(elidedGlyphsBuffer, numberOfGlyphs, 0u, indexOfEllipsis, numberOfElidedGlyphs);
            GlyphMemmove(elidedPositionsBuffer, numberOfGlyphs, 0u, indexOfEllipsis, numberOfElidedGlyphs);
            if(finalGlyphMappingRequested)
            {
              GlyphMemmove(mElidedFinalToSourceGlyphIndices.Begin(), numberOfGlyphs, 0u, indexOfEllipsis,
                           numberOfElidedGlyphs);
            }

            mStartIndexOfElidedGlyphs = mFirstMiddleIndexOfElidedGlyphs = mSecondMiddleIndexOfElidedGlyphs =
              indexOfEllipsis;
          }
          else if(ellipsisPosition == Text::EllipsisPosition::MIDDLE)
          {
            // 'Shifts and connects' glyphs before and after ellipsis glyph and 'Removes' in-between.
            bool isOnlySecondHalf = false;

            if(isTailMode)
            {
              mFirstMiddleIndexOfElidedGlyphs = indexOfEllipsis;
              if(ellipsisNextLine != nullptr)
              {
                mSecondMiddleIndexOfElidedGlyphs = ellipsisNextLine->glyphRun.glyphIndex;
              }
              else
              {
                mEndIndexOfElidedGlyphs = mSecondMiddleIndexOfElidedGlyphs = mFirstMiddleIndexOfElidedGlyphs;
              }
            }
            else
            {
              mFirstMiddleIndexOfElidedGlyphs =
                (ellipsisLine->glyphRun.numberOfGlyphs > 0u)
                  ? (ellipsisLine->glyphRun.glyphIndex + ellipsisLine->glyphRun.numberOfGlyphs - 1u)
                  : (ellipsisLine->glyphRun.glyphIndex);
              mSecondMiddleIndexOfElidedGlyphs = indexOfEllipsis;
              isOnlySecondHalf =
                ellipsisLine->glyphRun.numberOfGlyphs == 0u && ellipsisLine->glyphRunSecondHalf.numberOfGlyphs > 0u;
            }

            if(isOnlySecondHalf)
            {
              Length numberOfSecondHalfGlyphs = numberOfElidedGlyphs - mFirstMiddleIndexOfElidedGlyphs;

              // Copy elided glyphs after the ellipsis glyph.
              GlyphMemmove(elidedGlyphsBuffer, numberOfGlyphs, mFirstMiddleIndexOfElidedGlyphs,
                           mSecondMiddleIndexOfElidedGlyphs, numberOfSecondHalfGlyphs);
              GlyphMemmove(elidedPositionsBuffer, numberOfGlyphs, mFirstMiddleIndexOfElidedGlyphs,
                           mSecondMiddleIndexOfElidedGlyphs, numberOfSecondHalfGlyphs);
              if(finalGlyphMappingRequested)
              {
                GlyphMemmove(mElidedFinalToSourceGlyphIndices.Begin(), numberOfGlyphs,
                             mFirstMiddleIndexOfElidedGlyphs, mSecondMiddleIndexOfElidedGlyphs,
                             numberOfSecondHalfGlyphs);
              }
            }
            else
            {
              Length numberOfSecondHalfGlyphs = numberOfElidedGlyphs - mFirstMiddleIndexOfElidedGlyphs + 1u;

              // Make sure that out-of-boundary does not occur for the source range.
              if(mSecondMiddleIndexOfElidedGlyphs + numberOfSecondHalfGlyphs > numberOfGlyphs)
              {
                numberOfSecondHalfGlyphs = numberOfGlyphs - mSecondMiddleIndexOfElidedGlyphs;
              }

              const Length dstIndex = mFirstMiddleIndexOfElidedGlyphs + 1u;

              // Copy elided glyphs after the ellipsis glyph.
              GlyphMemmove(elidedGlyphsBuffer, numberOfGlyphs, dstIndex, mSecondMiddleIndexOfElidedGlyphs,
                           numberOfSecondHalfGlyphs);
              GlyphMemmove(elidedPositionsBuffer, numberOfGlyphs, dstIndex, mSecondMiddleIndexOfElidedGlyphs,
                           numberOfSecondHalfGlyphs);
              if(finalGlyphMappingRequested)
              {
                GlyphMemmove(mElidedFinalToSourceGlyphIndices.Begin(), numberOfGlyphs, dstIndex,
                             mSecondMiddleIndexOfElidedGlyphs, numberOfSecondHalfGlyphs);
              }
            }
          }
          else // Text::EllipsisPosition::END
          {
            // 'Removes' all the glyphs after the ellipsis glyph.
            mEndIndexOfElidedGlyphs = indexOfEllipsis;
          }

          if(finalGlyphMappingRequested)
          {
            mElidedFinalToSourceGlyphIndices.Resize(numberOfElidedGlyphs);
            for(GlyphIndex finalGlyph = 0u; finalGlyph < numberOfElidedGlyphs; ++finalGlyph)
            {
              if(mElidedFinalToSourceGlyphIndices[finalGlyph] == FinalElisionResult::INVALID_GLYPH_INDEX)
              {
                mEllipsisFinalGlyphIndex = finalGlyph;
                break;
              }
            }
          }
        }
      }
    }
  }
}

float ViewModel::GetStrikethroughHeight() const
{
  return mModel->GetStrikethroughHeight();
}

const Vector4& ViewModel::GetStrikethroughColor() const
{
  return mModel->GetStrikethroughColor();
}

bool ViewModel::IsStrikethroughEnabled() const
{
  return mModel->IsStrikethroughEnabled();
}

bool ViewModel::IsMarkupStrikethroughSet() const
{
  return mModel->IsMarkupStrikethroughSet();
}

Length ViewModel::GetNumberOfStrikethroughRuns() const
{
  return mModel->GetNumberOfStrikethroughRuns();
}

void ViewModel::GetStrikethroughRuns(StrikethroughGlyphRun* strikethroughRuns, StrikethroughRunIndex index,
                                     Length numberOfRuns) const
{
  mModel->GetStrikethroughRuns(strikethroughRuns, index, numberOfRuns);
}

Length ViewModel::GetNumberOfBoundedParagraphRuns() const
{
  return mModel->GetNumberOfBoundedParagraphRuns();
}

const Vector<BoundedParagraphRun>& ViewModel::GetBoundedParagraphRuns() const
{
  return mModel->GetBoundedParagraphRuns();
}

Length ViewModel::GetNumberOfCharacterSpacingGlyphRuns() const
{
  return mModel->GetNumberOfCharacterSpacingGlyphRuns();
}

const Vector<CharacterSpacingGlyphRun>& ViewModel::GetCharacterSpacingGlyphRuns() const
{
  return mModel->GetCharacterSpacingGlyphRuns();
}

const Vector<FontRun>& ViewModel::GetFontRuns() const
{
  return mModel->GetFontRuns();
}

const Vector<FontDescriptionRun>& ViewModel::GetFontDescriptionRuns() const
{
  return mModel->GetFontDescriptionRuns();
}

bool ViewModel::IsRemoveFrontInset() const
{
  return mModel->IsRemoveFrontInset();
}

bool ViewModel::IsRemoveBackInset() const
{
  return mModel->IsRemoveBackInset();
}

bool ViewModel::IsCutoutEnabled() const
{
  return mModel->IsCutoutEnabled();
}

const bool ViewModel::IsBackgroundWithCutoutEnabled() const
{
  return mModel->IsBackgroundWithCutoutEnabled();
}

const Vector4& ViewModel::GetBackgroundColorWithCutout() const
{
  return mModel->GetBackgroundColorWithCutout();
}

const Vector2& ViewModel::GetOffsetWithCutout() const
{
  return mModel->GetOffsetWithCutout();
}

const Vector<CharacterDirection>& ViewModel::GetCharacterDirections() const
{
  return mModel->GetCharacterDirections();
}

} // namespace Text

} // namespace Ui

} // namespace Dali
