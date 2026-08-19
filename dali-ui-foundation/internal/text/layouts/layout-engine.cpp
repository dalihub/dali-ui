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
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/integration-api/debug.h>
#include <cmath>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/bidirectional-support.h>
#include <dali-ui-foundation/internal/text/cursor-helper-functions.h>
#include <dali-ui-foundation/internal/text/ellipsis/ellipsis-planner.h>
#include <dali-ui-foundation/internal/text/glyph-metrics-helper.h>
#include <dali-ui-foundation/internal/text/layouts/layout-engine-helper-functions.h>
#include <dali-ui-foundation/internal/text/layouts/layout-engine.h>
#include <dali-ui-foundation/internal/text/layouts/layout-parameters.h>
#include <dali-ui-foundation/internal/text/rendering/styles/character-spacing-helper-functions.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
float GetLineHeight(const LineRun lineRun, bool isLastLine)
{
  // The line height is the addition of the line ascender, the line descender and the line spacing.
  // However, the line descender has a negative value, hence the subtraction.
  // In case this is the only/last line then line spacing should be ignored.
  float lineHeight = lineRun.ascender - lineRun.descender;

  if(!isLastLine || lineRun.lineSpacing > 0)
  {
    lineHeight += lineRun.lineSpacing;
  }
  return lineHeight;
}

namespace Layout
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::Concise, true, "LOG_TEXT_LAYOUT");
#endif

const float              MAX_FLOAT               = std::numeric_limits<float>::max();
const CharacterDirection LTR                     = false;
const CharacterDirection RTL                     = !LTR;
const float              LINE_SPACING            = 0.f;
const float              MIN_LINE_SIZE           = 0.f;
const Character          HYPHEN_UNICODE          = 0x002D;
const float              RELATIVE_LINE_SIZE      = 1.f;
const float              RELATIVE_LINE_SIZE_AUTO = -1.f;
const float              DEFAULT_FONT_PIXEL_SIZE = 10.f;

inline bool isEmptyLineAtLast(const Vector<LineRun>& lines, const Vector<LineRun>::Iterator& line)
{
  return ((*line).characterRun.numberOfCharacters == 0 && line + 1u == lines.End());
}

inline const float GetDefaultRelativeLineSize()
{
  return TextAbstraction::DesignCompatibilityEnabled() ? RELATIVE_LINE_SIZE_AUTO : RELATIVE_LINE_SIZE;
}

} // namespace

/**
 * @brief Stores temporary layout info of the line.
 */
struct LineLayout
{
  LineLayout()
  : glyphIndex{0u},
    characterIndex{0u},
    numberOfGlyphs{0u},
    numberOfCharacters{0u},
    ascender{-MAX_FLOAT},
    descender{MAX_FLOAT},
    lineSpacing{0.f},
    penX{0.f},
    previousAdvance{0.f},
    length{0.f},
    whiteSpaceLengthEndOfLine{0.f},
    textAscender{-MAX_FLOAT},
    textDescender{MAX_FLOAT},
    direction{LTR},
    isSplitToTwoHalves(false),
    containsReplacement(false),
    hasTextMetrics(false),
    glyphIndexInSecondHalfLine{0u},
    characterIndexInSecondHalfLine{0u},
    numberOfGlyphsInSecondHalfLine{0u},
    numberOfCharactersInSecondHalfLine{0u},
    relativeLineSize{GetDefaultRelativeLineSize()}

  {
  }

  ~LineLayout()
  {
  }

  void Clear()
  {
    glyphIndex                         = 0u;
    characterIndex                     = 0u;
    numberOfGlyphs                     = 0u;
    numberOfCharacters                 = 0u;
    ascender                           = -MAX_FLOAT;
    descender                          = MAX_FLOAT;
    textAscender                       = -MAX_FLOAT;
    textDescender                      = MAX_FLOAT;
    direction                          = LTR;
    isSplitToTwoHalves                 = false;
    containsReplacement                = false;
    hasTextMetrics                     = false;
    glyphIndexInSecondHalfLine         = 0u;
    characterIndexInSecondHalfLine     = 0u;
    numberOfGlyphsInSecondHalfLine     = 0u;
    numberOfCharactersInSecondHalfLine = 0u;
    relativeLineSize                   = GetDefaultRelativeLineSize();
  }

  GlyphIndex         glyphIndex;                ///< Index of the first glyph to be laid-out.
  CharacterIndex     characterIndex;            ///< Index of the first character to be laid-out.
  Length             numberOfGlyphs;            ///< The number of glyph which fit in one line.
  Length             numberOfCharacters;        ///< The number of characters which fit in one line.
  float              ascender;                  ///< The maximum ascender of all fonts in the line.
  float              descender;                 ///< The minimum descender of all fonts in the line.
  float              lineSpacing;               ///< The line spacing
  float              penX;                      ///< The origin of the current glyph ( is the start point plus the accumulation of all advances ).
  float              previousAdvance;           ///< The advance of the previous glyph.
  float              length;                    ///< The current length of the line.
  float              whiteSpaceLengthEndOfLine; ///< The length of the white spaces at the end of the line.
  float              textAscender;              ///< Ordinary-text ascender before replacement expansion.
  float              textDescender;             ///< Ordinary-text descender before replacement expansion.
  CharacterDirection direction;

  bool       isSplitToTwoHalves;         ///< Whether the second half is defined.
  bool       containsReplacement;        ///< Whether replacement metrics expanded this final candidate line.
  bool       hasTextMetrics;             ///< Whether ordinary-text metrics are available.
  GlyphIndex glyphIndexInSecondHalfLine; ///< Index of the first glyph to be laid-out for the second half of line.
  CharacterIndex
         characterIndexInSecondHalfLine;     ///< Index of the first character to be laid-out for the second half of line.
  Length numberOfGlyphsInSecondHalfLine;     ///< The number of glyph which fit in one line for the second half of line.
  Length numberOfCharactersInSecondHalfLine; ///< The number of characters which fit in one line for the second half of
                                             ///< line.

  float relativeLineSize; ///< The relative line size to be applied for this line.
};

struct LayoutBidiParameters
{
  void Clear()
  {
    paragraphDirection = LTR;
    bidiParagraphIndex = 0u;
    bidiLineIndex      = 0u;
    isBidirectional    = false;
  }

  CharacterDirection        paragraphDirection = LTR;   ///< The paragraph's direction.
  BidirectionalRunIndex     bidiParagraphIndex = 0u;    ///< Index to the paragraph's bidi info.
  BidirectionalLineRunIndex bidiLineIndex      = 0u;    ///< Index where to insert the next bidi line info.
  bool                      isBidirectional    = false; ///< Whether the text is bidirectional.
};

struct Engine::Impl
{
  Impl()
  : mLayout{Layout::Engine::SINGLE_LINE_BOX},
    mCursorWidth{0.f},
    mDefaultLineSpacing{LINE_SPACING},
    mDefaultLineSize{MIN_LINE_SIZE},
    mRelativeLineSize{GetDefaultRelativeLineSize()},
    mPixelSize{DEFAULT_FONT_PIXEL_SIZE},
    mFontSizeScale{1.0f},
    mIsCursorInsetEnabled{false}
  {
  }

  /**
   * @brief get the line spacing.
   *
   * @param[in] textSize The text size.
   * @param[in] relativeLineSize The relative line size to be applied.
   * @return the line spacing value.
   */
  float GetLineSpacing(float textSize, float relativeLineSize)
  {
    if(TextAbstraction::DesignCompatibilityEnabled())
    {
      // For readability and maintainability, completely separate the code.
      float lineSpacing;
      float relTextSize;
      float defaultLineSize = mDefaultLineSize * mFontSizeScale;

      lineSpacing = defaultLineSize - textSize;
      lineSpacing = lineSpacing < 0.f ? 0.f : lineSpacing;

      lineSpacing += mDefaultLineSpacing;

      // If relativeLineSize is less than 0, it will attempt to set LineHeight to NaturalSize.
      // Or, if there is a minLineSize set, it will attempt to apply minLineSize.
      relTextSize = relativeLineSize < 0 ? textSize : std::floor(mPixelSize * relativeLineSize);
      if(relTextSize >= defaultLineSize)
      {
        if(relTextSize <= textSize)
        {
          lineSpacing -= textSize - relTextSize;
        }
        else
        {
          if(defaultLineSize > textSize)
          {
            lineSpacing -= defaultLineSize - textSize;
          }

          lineSpacing += relTextSize - textSize;
        }
      }

      return lineSpacing;
    }
    else
    {
      float lineSpacing;
      float relTextSize;
      float defaultLineSize = mDefaultLineSize * mFontSizeScale;

      // Sets the line size
      lineSpacing = defaultLineSize - textSize;
      lineSpacing = lineSpacing < 0.f ? 0.f : lineSpacing;

      // Add the line spacing
      lineSpacing += mDefaultLineSpacing;

      // subtract line spcaing if relativeLineSize < 1 & larger than min height
      relTextSize = textSize * relativeLineSize;
      if(relTextSize > defaultLineSize)
      {
        if(relativeLineSize < 1)
        {
          // subtract the difference (always will be positive)
          lineSpacing -= (textSize - relTextSize);
        }
        else
        {
          // reverse the addition in the top.
          if(defaultLineSize > textSize)
          {
            lineSpacing -= defaultLineSize - textSize;
          }

          // add difference instead
          lineSpacing += relTextSize - textSize;
        }
      }

      return lineSpacing;
    }
  }

  /**
   * @brief Updates the line ascender and descender with the metrics of a new font.
   *
   * @param[in] glyphMetrics The metrics of the new font.
   * @param[in,out] lineLayout The line layout.
   */
  void UpdateLineHeight(const GlyphMetrics& glyphMetrics, LineLayout& lineLayout)
  {
    Text::FontMetrics fontMetrics;
    if(0u != glyphMetrics.fontId)
    {
      mMetrics->GetFontMetrics(glyphMetrics.fontId, fontMetrics);
    }
    else
    {
      fontMetrics.ascender           = glyphMetrics.fontHeight;
      fontMetrics.descender          = 0.f;
      fontMetrics.height             = fontMetrics.ascender;
      fontMetrics.underlinePosition  = 0.f;
      fontMetrics.underlineThickness = 1.f;
    }

    // Sets the maximum ascender.
    lineLayout.ascender = std::max(lineLayout.ascender, fontMetrics.ascender);

    // Sets the minimum descender.
    lineLayout.descender = std::min(lineLayout.descender, fontMetrics.descender);

    lineLayout.lineSpacing = GetLineSpacing(lineLayout.ascender + -lineLayout.descender, lineLayout.relativeLineSize);
  }

  /**
   * @brief Updates the ordinary-text metrics used by replacement layout.
   *
   * @param[in] glyphMetrics The metrics of the new font.
   * @param[in,out] lineLayout The line layout.
   */
  void UpdateReplacementTextMetrics(const GlyphMetrics& glyphMetrics, LineLayout& lineLayout)
  {
    if(0u == glyphMetrics.fontId)
    {
      return;
    }

    Text::FontMetrics fontMetrics;
    mMetrics->GetFontMetrics(glyphMetrics.fontId, fontMetrics);
    lineLayout.textAscender   = lineLayout.hasTextMetrics
                                  ? std::max(lineLayout.textAscender, fontMetrics.ascender)
                                  : fontMetrics.ascender;
    lineLayout.textDescender  = lineLayout.hasTextMetrics
                                  ? std::min(lineLayout.textDescender, fontMetrics.descender)
                                  : fontMetrics.descender;
    lineLayout.hasTextMetrics = true;
  }

  /**
   * @brief Sets glyph positions for an ellipsis candidate line.
   *
   * @param[in] parameters The layout parameters.
   * @param[in,out] bidiParameters The bidirectional layout state.
   * @param[in] lineLayout The candidate line.
   * @param[out] glyphPositionsBuffer The glyph position buffer.
   * @return The candidate line direction.
   */
  CharacterDirection SetEllipsisLineGlyphPositions(const Parameters&     parameters,
                                                   LayoutBidiParameters& bidiParameters,
                                                   const LineLayout&     lineLayout,
                                                   Vector2*              glyphPositionsBuffer)
  {
    const Vector<BidirectionalLineInfoRun>& bidirectionalLinesInfo =
      parameters.textModel->mLogicalModel->mBidirectionalLineInfo;

    if(bidiParameters.isBidirectional)
    {
      bidiParameters.bidiLineIndex = 0u;
      for(Vector<BidirectionalLineInfoRun>::ConstIterator it    = bidirectionalLinesInfo.Begin(),
                                                          endIt = bidirectionalLinesInfo.End();
          it != endIt;
          ++it, ++bidiParameters.bidiLineIndex)
      {
        const BidirectionalLineInfoRun& run = *it;
        if(lineLayout.characterIndex == run.characterRun.characterIndex &&
           lineLayout.numberOfCharacters == run.characterRun.numberOfCharacters &&
           lineLayout.characterIndexInSecondHalfLine == run.characterRunForSecondHalfLine.characterIndex &&
           lineLayout.numberOfCharactersInSecondHalfLine == run.characterRunForSecondHalfLine.numberOfCharacters)
        {
          break;
        }
      }
    }

    const BidirectionalLineInfoRun* const bidirectionalLineInfo =
      (bidiParameters.isBidirectional && bidiParameters.bidiLineIndex < bidirectionalLinesInfo.Count())
        ? &bidirectionalLinesInfo[bidiParameters.bidiLineIndex]
        : nullptr;
    if(bidirectionalLineInfo &&
       !bidirectionalLineInfo->isIdentity &&
       lineLayout.characterIndex == bidirectionalLineInfo->characterRun.characterIndex)
    {
      SetGlyphPositions(parameters, glyphPositionsBuffer, bidiParameters, lineLayout);
      return RTL;
    }

    SetGlyphPositions(parameters, glyphPositionsBuffer, lineLayout);
    return LTR;
  }

  /**
   * @brief Resolves the END ellipsis plan for a replacement candidate line.
   *
   * @param[in] parameters The layout parameters.
   * @param[in,out] bidiParameters The bidirectional layout state.
   * @param[in] lineLayout The candidate line.
   * @param[out] glyphPositionsBuffer The glyph position buffer.
   * @param[out] lineDirection The candidate line direction.
   * @return The resolved END ellipsis plan.
   */
  EndEllipsisPlan ResolveEndEllipsisCandidatePlan(const Parameters&     parameters,
                                                  LayoutBidiParameters& bidiParameters,
                                                  const LineLayout&     lineLayout,
                                                  Vector2*              glyphPositionsBuffer,
                                                  CharacterDirection&   lineDirection)
  {
    EndEllipsisPlan plan;
    if(parameters.replacementLayoutData == nullptr || lineLayout.numberOfGlyphs == 0u)
    {
      return plan;
    }

    const VisualModel& visualModel = *parameters.textModel->mVisualModel;
    lineDirection                  = SetEllipsisLineGlyphPositions(parameters, bidiParameters, lineLayout, glyphPositionsBuffer);

    LineRun alignmentLine{};
    alignmentLine.width                          = lineLayout.length;
    alignmentLine.extraLength                    = std::ceil(lineLayout.whiteSpaceLengthEndOfLine);
    alignmentLine.direction                      = lineDirection;
    const ReplacementLayoutData& replacementData = *parameters.replacementLayoutData;
    CalculateHorizontalAlignment(parameters.boundingBox.width,
                                 replacementData.horizontalAlignment,
                                 alignmentLine,
                                 replacementData.layoutDirection,
                                 replacementData.matchLayoutDirection);

    TextAbstraction::FontClient             fontClient = parameters.fontClient;
    const Vector<CharacterSpacingGlyphRun>& characterSpacingRuns =
      visualModel.GetCharacterSpacingGlyphRuns();
    EndEllipsisInputView input;
    input.glyphs                  = visualModel.mGlyphs.Begin();
    input.glyphPositions          = glyphPositionsBuffer;
    input.text                    = parameters.textModel->mLogicalModel->mText.Begin();
    input.glyphToCharacterMap     = visualModel.mGlyphsToCharacters.Begin();
    input.characterSpacingRuns    = &characterSpacingRuns;
    input.numberOfGlyphs          = static_cast<Dali::Ui::Text::Length>(visualModel.mGlyphs.Count());
    input.glyphPositionStartIndex = parameters.startGlyphIndex;
    input.numberOfGlyphPositions  = parameters.numberOfGlyphs;
    input.numberOfCharacters      = static_cast<Dali::Ui::Text::Length>(parameters.textModel->mLogicalModel->mText.Count());
    input.startIndex              = std::min<GlyphIndex>(lineLayout.glyphIndex + lineLayout.numberOfGlyphs,
                                                         static_cast<GlyphIndex>(visualModel.mGlyphs.Count())) -
                       1u;
    input.lineWidth             = lineLayout.length;
    input.positionOffset        = alignmentLine.alignmentOffset;
    input.modelCharacterSpacing = visualModel.GetCharacterSpacing();
    return ResolveEndEllipsisPlan(input, fontClient);
  }

  /**
   * @brief Applies replacement metrics to a candidate line.
   *
   * The resulting ascender, descender and spacing are used by height fitting
   * and ellipsis selection.
   *
   * @param[in] parameters The current layout parameters.
   * @param[in,out] lineLayout The candidate line metrics to update.
   */
  void ApplyReplacementLineMetrics(
    const Parameters& parameters,
    LineLayout&       lineLayout,
    GlyphIndex        firstExcludedReplacementGlyph = EndEllipsisPlan::INVALID_GLYPH_INDEX,
    GlyphIndex        lastExcludedReplacementGlyph  = EndEllipsisPlan::INVALID_GLYPH_INDEX)
  {
    if(parameters.replacementLayoutData == nullptr || parameters.replacementLayoutData->runs == nullptr)
    {
      return;
    }

    const VisualModel& visualModel               = *parameters.textModel->mVisualModel;
    float              ordinaryAscender          = lineLayout.textAscender;
    float              ordinaryDescender         = lineLayout.textDescender;
    float              top                       = -ordinaryAscender;
    float              bottom                    = -ordinaryDescender;
    bool               metricsInitialized        = lineLayout.hasTextMetrics;
    bool               hasReplacement            = false;
    bool               hasExcludedReplacement    = false;
    auto               initializeOrdinaryMetrics = [&]()
    {
      if(metricsInitialized)
      {
        return;
      }

      if(parameters.replacementLayoutData->defaultFontId != 0u)
      {
        Text::FontMetrics fontMetrics;
        mMetrics->GetFontMetrics(parameters.replacementLayoutData->defaultFontId, fontMetrics);
        ordinaryAscender  = fontMetrics.ascender;
        ordinaryDescender = fontMetrics.descender;
      }
      else
      {
        ordinaryAscender  = mPixelSize * 0.8f;
        ordinaryDescender = -mPixelSize * 0.2f;
      }
      lineLayout.textAscender   = ordinaryAscender;
      lineLayout.textDescender  = ordinaryDescender;
      lineLayout.hasTextMetrics = true;
      top                       = -ordinaryAscender;
      bottom                    = -ordinaryDescender;
      metricsInitialized        = true;
    };

    // GetLineLayoutForBox may inspect one glyph beyond the committed line
    // before deciding where to wrap. Keep only metrics belonging to the
    // committed ordinary glyphs; retained replacements are applied below.
    if(metricsInitialized)
    {
      lineLayout.ascender    = ordinaryAscender;
      lineLayout.descender   = ordinaryDescender;
      lineLayout.lineSpacing = 0.0f;
    }
    lineLayout.containsReplacement = false;

    auto includeReplacementRun = [&](GlyphIndex glyphIndex, Length numberOfGlyphs)
    {
      const GlyphIndex end = std::min<GlyphIndex>(glyphIndex + numberOfGlyphs, static_cast<GlyphIndex>(visualModel.mGlyphs.Count()));
      for(GlyphIndex index = glyphIndex; index < end; ++index)
      {
        const GlyphInfo& glyph = visualModel.mGlyphs[index];
        if(!IsSyntheticReplacementGlyph(glyph) || index >= visualModel.mGlyphsToCharacters.Count())
        {
          continue;
        }
        if(firstExcludedReplacementGlyph != EndEllipsisPlan::INVALID_GLYPH_INDEX &&
           index >= firstExcludedReplacementGlyph &&
           index <= lastExcludedReplacementGlyph)
        {
          hasExcludedReplacement = true;
          continue;
        }
        hasReplacement = true;
        initializeOrdinaryMetrics();

        const ProjectedReplacementRun* replacement =
          parameters.replacementLayoutData->Find(visualModel.mGlyphsToCharacters[index]);
        if(replacement == nullptr)
        {
          continue;
        }

        float replacementTop    = 0.0f;
        float replacementBottom = 0.0f;
        switch(replacement->metrics.verticalAlignment)
        {
          case ReplacementVerticalAlignment::TEXT_BOTTOM:
            replacementBottom = -ordinaryDescender + replacement->metrics.verticalOffset;
            replacementTop    = replacementBottom - replacement->metrics.height;
            break;
          case ReplacementVerticalAlignment::TEXT_CENTER:
          {
            const float center = -0.5f * (ordinaryAscender + ordinaryDescender) +
                                 replacement->metrics.verticalOffset;
            replacementTop    = center - 0.5f * replacement->metrics.height;
            replacementBottom = center + 0.5f * replacement->metrics.height;
            break;
          }
          case ReplacementVerticalAlignment::TEXT_BASELINE:
          default:
            replacementBottom = replacement->metrics.verticalOffset;
            replacementTop    = replacementBottom - replacement->metrics.height;
            break;
        }
        top    = std::min(top, replacementTop);
        bottom = std::max(bottom, replacementBottom);
      }
    };

    includeReplacementRun(lineLayout.glyphIndex, lineLayout.numberOfGlyphs);
    if(lineLayout.isSplitToTwoHalves)
    {
      includeReplacementRun(lineLayout.glyphIndexInSecondHalfLine, lineLayout.numberOfGlyphsInSecondHalfLine);
    }

    if(!hasReplacement)
    {
      if(hasExcludedReplacement && !metricsInitialized)
      {
        initializeOrdinaryMetrics();
        lineLayout.ascender    = ordinaryAscender;
        lineLayout.descender   = ordinaryDescender;
        lineLayout.lineSpacing = 0.0f;
      }
      return;
    }
    lineLayout.containsReplacement = true;

    lineLayout.ascender  = std::max(0.0f, -top);
    lineLayout.descender = std::min(0.0f, -bottom);
    // Relative line size may intentionally shrink an ordinary font box, but
    // it must never cancel space reserved by an inline replacement. Preserve
    // only the non-negative spacing of the surrounding ordinary line.
    lineLayout.lineSpacing = std::max(0.0f,
                                      GetLineSpacing(ordinaryAscender - ordinaryDescender,
                                                     lineLayout.relativeLineSize));
  }

  /**
   * @brief Merges a temporary line layout into the line layout.
   *
   * @param[in,out] lineLayout The line layout.
   * @param[in] tmpLineLayout A temporary line layout.
   * @param[in] isShifted Whether to shift first glyph and character indices.
   */
  void MergeLineLayout(LineLayout& lineLayout, const LineLayout& tmpLineLayout, bool isShifted)
  {
    lineLayout.numberOfCharacters += tmpLineLayout.numberOfCharacters;
    lineLayout.numberOfGlyphs += tmpLineLayout.numberOfGlyphs;

    lineLayout.penX            = tmpLineLayout.penX;
    lineLayout.previousAdvance = tmpLineLayout.previousAdvance;

    lineLayout.length                    = tmpLineLayout.length;
    lineLayout.whiteSpaceLengthEndOfLine = tmpLineLayout.whiteSpaceLengthEndOfLine;

    // Sets the maximum ascender.
    lineLayout.ascender = std::max(lineLayout.ascender, tmpLineLayout.ascender);

    // Sets the minimum descender.
    lineLayout.descender = std::min(lineLayout.descender, tmpLineLayout.descender);

    if(tmpLineLayout.hasTextMetrics)
    {
      lineLayout.textAscender   = lineLayout.hasTextMetrics
                                    ? std::max(lineLayout.textAscender, tmpLineLayout.textAscender)
                                    : tmpLineLayout.textAscender;
      lineLayout.textDescender  = lineLayout.hasTextMetrics
                                    ? std::min(lineLayout.textDescender, tmpLineLayout.textDescender)
                                    : tmpLineLayout.textDescender;
      lineLayout.hasTextMetrics = true;
    }

    // To handle cases START in ellipsis position when want to shift first glyph to let width fit.
    if(isShifted)
    {
      lineLayout.glyphIndex     = tmpLineLayout.glyphIndex;
      lineLayout.characterIndex = tmpLineLayout.characterIndex;
    }

    lineLayout.isSplitToTwoHalves                 = tmpLineLayout.isSplitToTwoHalves;
    lineLayout.glyphIndexInSecondHalfLine         = tmpLineLayout.glyphIndexInSecondHalfLine;
    lineLayout.characterIndexInSecondHalfLine     = tmpLineLayout.characterIndexInSecondHalfLine;
    lineLayout.numberOfGlyphsInSecondHalfLine     = tmpLineLayout.numberOfGlyphsInSecondHalfLine;
    lineLayout.numberOfCharactersInSecondHalfLine = tmpLineLayout.numberOfCharactersInSecondHalfLine;
  }

  void LayoutRightToLeft(const Parameters& parameters, const BidirectionalLineInfoRun& bidirectionalLineInfo,
                         float& length, float& whiteSpaceLengthEndOfLine)
  {
    // Travers characters in line then draw it form right to left by mapping index using visualToLogicalMap.
    // When the line is spllited by MIDDLE ellipsis then travers the second half of line "characterRunForSecondHalfLine"
    // then the first half of line "characterRun",
    // Otherwise travers whole characters in"characterRun".

    const Character* const  textBuffer               = parameters.textModel->mLogicalModel->mText.Begin();
    const Length* const     charactersPerGlyphBuffer = parameters.textModel->mVisualModel->mCharactersPerGlyph.Begin();
    const GlyphInfo* const  glyphsBuffer             = parameters.textModel->mVisualModel->mGlyphs.Begin();
    const GlyphIndex* const charactersToGlyphsBuffer = parameters.textModel->mVisualModel->mCharactersToGlyph.Begin();

    const float outlineWidth =
      parameters.textModel->IsOutlineEnabled() ? static_cast<float>(parameters.textModel->GetOutlineWidth()) : 0.0f;
    const GlyphIndex lastGlyphOfParagraphPlusOne = parameters.startGlyphIndex + parameters.numberOfGlyphs;
    const float      modelCharacterSpacing       = parameters.textModel->mVisualModel->GetCharacterSpacing();

    // Get the character-spacing runs.
    const Vector<CharacterSpacingGlyphRun>& characterSpacingGlyphRuns =
      parameters.textModel->mVisualModel->GetCharacterSpacingGlyphRuns();

    CharacterIndex characterLogicalIndex = 0u;
    CharacterIndex characterVisualIndex  = 0u;

    float calculatedAdvance = 0.f;

    // If there are characters in the second half of Line then the first visual index mapped from
    // visualToLogicalMapSecondHalf Otherwise maps the first visual index from visualToLogicalMap. This is to initialize
    // the first visual index.
    if(bidirectionalLineInfo.characterRunForSecondHalfLine.numberOfCharacters > 0u)
    {
      characterVisualIndex = bidirectionalLineInfo.characterRunForSecondHalfLine.characterIndex +
                             (bidirectionalLineInfo.visualToLogicalMapSecondHalf
                                ? *(bidirectionalLineInfo.visualToLogicalMapSecondHalf + characterLogicalIndex)
                                : 0u);
    }
    else
    {
      characterVisualIndex = bidirectionalLineInfo.characterRun.characterIndex +
                             (bidirectionalLineInfo.visualToLogicalMap
                                ? *(bidirectionalLineInfo.visualToLogicalMap + characterLogicalIndex)
                                : 0u);
    }

    bool extendedToSecondHalf = false; // Whether the logical index is extended to second half

    if(RTL == bidirectionalLineInfo.direction)
    {
      // If there are characters in the second half of Line.
      if(bidirectionalLineInfo.characterRunForSecondHalfLine.numberOfCharacters > 0u)
      {
        // Keep adding the WhiteSpaces to the whiteSpaceLengthEndOfLine
        while(TextAbstraction::IsWhiteSpace(*(textBuffer + characterVisualIndex)))
        {
          const GlyphInfo& glyphInfo = *(glyphsBuffer + *(charactersToGlyphsBuffer + characterVisualIndex));

          const float characterSpacing =
            GetGlyphCharacterSpacing(characterVisualIndex, characterSpacingGlyphRuns, modelCharacterSpacing);
          calculatedAdvance =
            GetCalculatedAdvance(*(textBuffer + characterVisualIndex), characterSpacing, glyphInfo.advance);
          whiteSpaceLengthEndOfLine += calculatedAdvance;

          ++characterLogicalIndex;
          characterVisualIndex = bidirectionalLineInfo.characterRunForSecondHalfLine.characterIndex +
                                 (bidirectionalLineInfo.visualToLogicalMapSecondHalf
                                    ? *(bidirectionalLineInfo.visualToLogicalMapSecondHalf + characterLogicalIndex)
                                    : 0u);
        }
      }

      // If all characters in the second half of Line are WhiteSpaces.
      // then continue adding the WhiteSpaces from the first hel of Line.
      // Also this is valid when the line was not splitted.
      if(characterLogicalIndex == bidirectionalLineInfo.characterRunForSecondHalfLine.numberOfCharacters)
      {
        extendedToSecondHalf  = true; // Whether the logical index is extended to second half
        characterLogicalIndex = 0u;
        characterVisualIndex  = bidirectionalLineInfo.characterRun.characterIndex +
                               (bidirectionalLineInfo.visualToLogicalMap
                                  ? *(bidirectionalLineInfo.visualToLogicalMap + characterLogicalIndex)
                                  : 0u);

        // Keep adding the WhiteSpaces to the whiteSpaceLengthEndOfLine
        while(TextAbstraction::IsWhiteSpace(*(textBuffer + characterVisualIndex)))
        {
          const GlyphInfo& glyphInfo = *(glyphsBuffer + *(charactersToGlyphsBuffer + characterVisualIndex));

          const float characterSpacing =
            GetGlyphCharacterSpacing(characterVisualIndex, characterSpacingGlyphRuns, modelCharacterSpacing);
          calculatedAdvance =
            GetCalculatedAdvance(*(textBuffer + characterVisualIndex), characterSpacing, glyphInfo.advance);
          whiteSpaceLengthEndOfLine += calculatedAdvance;

          ++characterLogicalIndex;
          characterVisualIndex = bidirectionalLineInfo.characterRun.characterIndex +
                                 (bidirectionalLineInfo.visualToLogicalMap
                                    ? *(bidirectionalLineInfo.visualToLogicalMap + characterLogicalIndex)
                                    : 0u);
        }
      }
    }

    // Here's the first index of character is not WhiteSpace
    const GlyphIndex glyphIndex = *(charactersToGlyphsBuffer + characterVisualIndex);

    // Check whether the first glyph comes from a character that is shaped in multiple glyphs.
    const Length numberOfGLyphsInGroup =
      GetNumberOfGlyphsOfGroup(glyphIndex, lastGlyphOfParagraphPlusOne, charactersPerGlyphBuffer);

    GlyphMetrics glyphMetrics;
    const float  characterSpacing =
      GetGlyphCharacterSpacing(glyphIndex, characterSpacingGlyphRuns, modelCharacterSpacing);
    calculatedAdvance = GetCalculatedAdvance(*(textBuffer + characterVisualIndex), characterSpacing,
                                             (*(glyphsBuffer + glyphIndex)).advance);
    GetGlyphsMetrics(glyphIndex, numberOfGLyphsInGroup, glyphMetrics, glyphsBuffer, mMetrics, calculatedAdvance);

    float penX = -glyphMetrics.xBearing + GetCursorInsetWidth() + outlineWidth;

    // Traverses the characters of the right to left paragraph.
    // Continue in the second half of line, because in it the first index of character that is not WhiteSpace.
    if(!extendedToSecondHalf && bidirectionalLineInfo.characterRunForSecondHalfLine.numberOfCharacters > 0u)
    {
      for(; characterLogicalIndex < bidirectionalLineInfo.characterRunForSecondHalfLine.numberOfCharacters;)
      {
        // Convert the character in the logical order into the character in the visual order.
        const CharacterIndex characterVisualIndex =
          bidirectionalLineInfo.characterRunForSecondHalfLine.characterIndex +
          (bidirectionalLineInfo.visualToLogicalMapSecondHalf
             ? *(bidirectionalLineInfo.visualToLogicalMapSecondHalf + characterLogicalIndex)
             : 0u);
        const bool isWhiteSpace = TextAbstraction::IsWhiteSpace(*(textBuffer + characterVisualIndex));

        const GlyphIndex glyphIndex = *(charactersToGlyphsBuffer + characterVisualIndex);

        // Check whether this glyph comes from a character that is shaped in multiple glyphs.
        const Length numberOfGLyphsInGroup =
          GetNumberOfGlyphsOfGroup(glyphIndex, lastGlyphOfParagraphPlusOne, charactersPerGlyphBuffer);

        characterLogicalIndex += *(charactersPerGlyphBuffer + glyphIndex + numberOfGLyphsInGroup - 1u);

        GlyphMetrics glyphMetrics;
        const float  characterSpacing =
          GetGlyphCharacterSpacing(glyphIndex, characterSpacingGlyphRuns, modelCharacterSpacing);
        calculatedAdvance = GetCalculatedAdvance(*(textBuffer + characterVisualIndex), characterSpacing,
                                                 (*(glyphsBuffer + glyphIndex)).advance);
        GetGlyphsMetrics(glyphIndex, numberOfGLyphsInGroup, glyphMetrics, glyphsBuffer, mMetrics, calculatedAdvance);

        if(isWhiteSpace)
        {
          // If glyph is WhiteSpace then:
          // For RTL it is whitespace but not at endOfLine. Use "advance" to accumulate length and shift penX.
          // the endOfLine in RTL was the headOfLine for layouting.
          // But for LTR added it to the endOfLine and use "advance" to accumulate length.
          if(RTL == bidirectionalLineInfo.direction)
          {
            length += glyphMetrics.advance;
          }
          else
          {
            whiteSpaceLengthEndOfLine += glyphMetrics.advance;
          }
          penX += glyphMetrics.advance;
        }
        else
        {
          // If glyph is not whiteSpace then:
          // Reset endOfLine for LTR because there is a non-whiteSpace so the tail of line is not whiteSpaces
          // Use "advance" and "interGlyphExtraAdvance" to shift penX.
          // Set length to the maximum possible length, of the current glyph "xBearing" and "width" are shifted penX to
          // length greater than current lenght. Otherwise the current length is maximum.
          if(LTR == bidirectionalLineInfo.direction)
          {
            whiteSpaceLengthEndOfLine = 0.f;
          }

          if(parameters.textModel->mRemoveBackInset)
          {
            length = std::max(length, penX + glyphMetrics.xBearing + glyphMetrics.width);
          }
          else
          {
            length = std::max(length, penX + glyphMetrics.advance);
          }

          penX += (glyphMetrics.advance + parameters.interGlyphExtraAdvance);
        }
      }
    }

    // Continue traversing in the first half of line or in the whole line.
    // If the second half of line was extended then continue from logical index in the first half of line
    // Also this is valid when the line was not splitted and there were WhiteSpace.
    // Otherwise start from first logical index in line.
    characterLogicalIndex = extendedToSecondHalf ? characterLogicalIndex : 0u;
    for(; characterLogicalIndex < bidirectionalLineInfo.characterRun.numberOfCharacters;)
    {
      // Convert the character in the logical order into the character in the visual order.
      const CharacterIndex characterVisualIndex =
        bidirectionalLineInfo.characterRun.characterIndex +
        (bidirectionalLineInfo.visualToLogicalMap
           ? *(bidirectionalLineInfo.visualToLogicalMap + characterLogicalIndex)
           : 0u);
      const bool isWhiteSpace = TextAbstraction::IsWhiteSpace(*(textBuffer + characterVisualIndex));

      const GlyphIndex glyphIndex = *(charactersToGlyphsBuffer + characterVisualIndex);

      // Check whether this glyph comes from a character that is shaped in multiple glyphs.
      const Length numberOfGLyphsInGroup =
        GetNumberOfGlyphsOfGroup(glyphIndex, lastGlyphOfParagraphPlusOne, charactersPerGlyphBuffer);

      characterLogicalIndex += *(charactersPerGlyphBuffer + glyphIndex + numberOfGLyphsInGroup - 1u);

      GlyphMetrics glyphMetrics;
      const float  characterSpacing =
        GetGlyphCharacterSpacing(glyphIndex, characterSpacingGlyphRuns, modelCharacterSpacing);
      calculatedAdvance = GetCalculatedAdvance(*(textBuffer + characterVisualIndex), characterSpacing,
                                               (*(glyphsBuffer + glyphIndex)).advance);
      GetGlyphsMetrics(glyphIndex, numberOfGLyphsInGroup, glyphMetrics, glyphsBuffer, mMetrics, calculatedAdvance);

      if(isWhiteSpace)
      {
        // If glyph is WhiteSpace then:
        // For RTL it is whitespace but not at endOfLine. Use "advance" to accumulate length and shift penX.
        // the endOfLine in RTL was the headOfLine for layouting.
        // But for LTR added it to the endOfLine and use "advance" to accumulate length.
        if(RTL == bidirectionalLineInfo.direction)
        {
          length += glyphMetrics.advance;
        }
        else
        {
          whiteSpaceLengthEndOfLine += glyphMetrics.advance;
        }
        penX += glyphMetrics.advance;
      }
      else
      {
        // If glyph is not whiteSpace then:
        // Reset endOfLine for LTR because there is a non-whiteSpace so the tail of line is not whiteSpaces
        // Use "advance" and "interGlyphExtraAdvance" to shift penX.
        // Set length to the maximum possible length, of the current glyph "xBearing" and "width" are shifted penX to
        // length greater than current lenght. Otherwise the current length is maximum.
        if(LTR == bidirectionalLineInfo.direction)
        {
          whiteSpaceLengthEndOfLine = 0.f;
        }

        if(parameters.textModel->mRemoveBackInset)
        {
          length = std::max(length, penX + glyphMetrics.xBearing + glyphMetrics.width);
        }
        else
        {
          length = std::max(length, penX + glyphMetrics.advance);
        }
        penX += (glyphMetrics.advance + parameters.interGlyphExtraAdvance);
      }
    }
  }

  void ReorderBiDiLayout(const Parameters& parameters, LayoutBidiParameters& bidiParameters,
                         const LineLayout& currentLineLayout, LineLayout& lineLayout, bool breakInCharacters,
                         bool enforceEllipsisInSingleLine)
  {
    const Length* const charactersPerGlyphBuffer = parameters.textModel->mVisualModel->mCharactersPerGlyph.Begin();

    // The last glyph to be laid-out.
    const GlyphIndex lastGlyphOfParagraphPlusOne = parameters.startGlyphIndex + parameters.numberOfGlyphs;

    const Vector<BidirectionalParagraphInfoRun>& bidirectionalParagraphsInfo =
      parameters.textModel->mLogicalModel->mBidirectionalParagraphInfo;

    const BidirectionalParagraphInfoRun& bidirectionalParagraphInfo =
      bidirectionalParagraphsInfo[bidiParameters.bidiParagraphIndex];
    if((lineLayout.characterIndex >= bidirectionalParagraphInfo.characterRun.characterIndex) &&
       (lineLayout.characterIndex < bidirectionalParagraphInfo.characterRun.characterIndex +
                                      bidirectionalParagraphInfo.characterRun.numberOfCharacters))
    {
      Vector<BidirectionalLineInfoRun>& bidirectionalLinesInfo =
        parameters.textModel->mLogicalModel->mBidirectionalLineInfo;

      TextAbstraction::BidirectionalSupport bidirectionalSupport = parameters.bidirectionalSupport;

      // Sets the visual to logical map tables needed to reorder the text.
      ReorderLine(bidirectionalSupport, bidirectionalParagraphInfo, bidirectionalLinesInfo,
                  bidiParameters.bidiLineIndex, lineLayout.characterIndex, lineLayout.numberOfCharacters,
                  lineLayout.characterIndexInSecondHalfLine, lineLayout.numberOfCharactersInSecondHalfLine,
                  bidiParameters.paragraphDirection);

      // Recalculate the length of the line and update the layout.
      const BidirectionalLineInfoRun& bidirectionalLineInfo =
        *(bidirectionalLinesInfo.Begin() + bidiParameters.bidiLineIndex);

      if(!bidirectionalLineInfo.isIdentity)
      {
        float length                    = 0.f;
        float whiteSpaceLengthEndOfLine = 0.f;
        LayoutRightToLeft(parameters, bidirectionalLineInfo, length, whiteSpaceLengthEndOfLine);

        lineLayout.whiteSpaceLengthEndOfLine = whiteSpaceLengthEndOfLine;
        if(!Equals(length, lineLayout.length))
        {
          const bool isMultiline = (!enforceEllipsisInSingleLine) && (mLayout == MULTI_LINE_BOX);

          if(isMultiline && (length > parameters.boundingBox.width))
          {
            if(breakInCharacters || (isMultiline && (0u == currentLineLayout.numberOfGlyphs)))
            {
              // The word doesn't fit in one line. It has to be split by character.

              // Remove the last laid out glyph(s) as they doesn't fit.
              for(GlyphIndex glyphIndex = lineLayout.glyphIndex + lineLayout.numberOfGlyphs - 1u;
                  glyphIndex >= lineLayout.glyphIndex;)
              {
                const Length numberOfGLyphsInGroup =
                  GetNumberOfGlyphsOfGroup(glyphIndex, lastGlyphOfParagraphPlusOne, charactersPerGlyphBuffer);

                const Length numberOfCharacters = *(charactersPerGlyphBuffer + glyphIndex + numberOfGLyphsInGroup - 1u);

                lineLayout.numberOfGlyphs -= numberOfGLyphsInGroup;
                lineLayout.numberOfCharacters -= numberOfCharacters;

                AdjustLayout(parameters, bidiParameters, bidirectionalParagraphInfo, lineLayout);

                if(lineLayout.length < parameters.boundingBox.width)
                {
                  break;
                }

                if(glyphIndex < numberOfGLyphsInGroup)
                {
                  // avoids go under zero for an unsigned int.
                  break;
                }

                glyphIndex -= numberOfGLyphsInGroup;
              }
            }
            else
            {
              lineLayout = currentLineLayout;

              AdjustLayout(parameters, bidiParameters, bidirectionalParagraphInfo, lineLayout);
            }
          }
          else
          {
            lineLayout.length = std::max(length, lineLayout.length);
          }
        }
      }
    }
  }

  void AdjustLayout(const Parameters& parameters, LayoutBidiParameters& bidiParameters,
                    const BidirectionalParagraphInfoRun& bidirectionalParagraphInfo, LineLayout& lineLayout)
  {
    Vector<BidirectionalLineInfoRun>& bidirectionalLinesInfo =
      parameters.textModel->mLogicalModel->mBidirectionalLineInfo;

    // Remove current reordered line.
    bidirectionalLinesInfo.Erase(bidirectionalLinesInfo.Begin() + bidiParameters.bidiLineIndex);

    TextAbstraction::BidirectionalSupport bidirectionalSupport = parameters.bidirectionalSupport;

    // Re-build the conversion table without the removed glyphs.
    ReorderLine(bidirectionalSupport, bidirectionalParagraphInfo, bidirectionalLinesInfo, bidiParameters.bidiLineIndex,
                lineLayout.characterIndex, lineLayout.numberOfCharacters, lineLayout.characterIndexInSecondHalfLine,
                lineLayout.numberOfCharactersInSecondHalfLine, bidiParameters.paragraphDirection);

    const BidirectionalLineInfoRun& bidirectionalLineInfo =
      *(bidirectionalLinesInfo.Begin() + bidiParameters.bidiLineIndex);

    float length                    = 0.f;
    float whiteSpaceLengthEndOfLine = 0.f;
    LayoutRightToLeft(parameters, bidirectionalLineInfo, length, whiteSpaceLengthEndOfLine);

    lineLayout.length                    = length;
    lineLayout.whiteSpaceLengthEndOfLine = whiteSpaceLengthEndOfLine;
  }

  /**
   * Retrieves the line layout for a given box width.
   *
   * @note This method starts to layout text as if it was left to right. However, it might be differences in the length
   *       of the line if it's a bidirectional one. If the paragraph is bidirectional, this method will call a function
   *       to reorder the line and recalculate its length.
   *

   * @param[in] parameters The layout parameters.
   * @param[] bidiParameters Bidirectional info for the current line.
   * @param[out] lineLayout The line layout.
   * @param[in] completelyFill Whether to completely fill the line ( even if the last word exceeds the boundaries ).
   * @param[in] ellipsisPosition Where is the location the text elide
   * @param[in] hiddenInputEnabled Whether the hidden input is enabled.
   */
  void GetLineLayoutForBox(const Parameters& parameters, LayoutBidiParameters& bidiParameters, LineLayout& lineLayout,
                           bool completelyFill, Text::EllipsisPosition::Type ellipsisPosition,
                           bool enforceEllipsisInSingleLine, bool elideTextEnabled, bool hiddenInputEnabled)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->GetLineLayoutForBox\n");
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "  initial glyph index : %d\n", lineLayout.glyphIndex);

    const Character* const      textBuffer               = parameters.textModel->mLogicalModel->mText.Begin();
    const Length* const         charactersPerGlyphBuffer = parameters.textModel->mVisualModel->mCharactersPerGlyph.Begin();
    const GlyphInfo* const      glyphsBuffer             = parameters.textModel->mVisualModel->mGlyphs.Begin();
    const CharacterIndex* const glyphsToCharactersBuffer =
      parameters.textModel->mVisualModel->mGlyphsToCharacters.Begin();
    const LineBreakInfo* const lineBreakInfoBuffer = parameters.textModel->mLogicalModel->mLineBreakInfo.Begin();

    const float outlineWidth =
      parameters.textModel->IsOutlineEnabled() ? static_cast<float>(parameters.textModel->GetOutlineWidth()) : 0.0f;
    const Length totalNumberOfGlyphs = static_cast<Dali::Ui::Text::Length>(parameters.textModel->mVisualModel->mGlyphs.Count());

    const bool isMultiline = !enforceEllipsisInSingleLine && (mLayout == MULTI_LINE_BOX);
    const bool isWordLaidOut =
      parameters.textModel->mLineWrapMode == LineWrapMode::WORD ||
      (parameters.textModel->mLineWrapMode == LineWrapMode::HYPHENATION) ||
      (parameters.textModel->mLineWrapMode == LineWrapMode::MIXED);
    const bool isHyphenMode =
      parameters.textModel->mLineWrapMode == LineWrapMode::HYPHENATION;
    const bool isMixedMode = parameters.textModel->mLineWrapMode == LineWrapMode::MIXED;

    const bool isSplitToTwoHalves =
      elideTextEnabled && !isMultiline && ellipsisPosition == Text::EllipsisPosition::MIDDLE;

    // The last glyph to be laid-out.
    const GlyphIndex lastGlyphOfParagraphPlusOne = parameters.startGlyphIndex + parameters.numberOfGlyphs;

    // If the first glyph has a negative bearing its absolute value needs to be added to the line length.
    // In the case the line starts with a right to left character, if the width is longer than the advance,
    // the difference needs to be added to the line length.

    // Check whether the first glyph comes from a character that is shaped in multiple glyphs.
    const Length numberOfGLyphsInGroup =
      GetNumberOfGlyphsOfGroup(lineLayout.glyphIndex, lastGlyphOfParagraphPlusOne, charactersPerGlyphBuffer);

    float targetWidth = parameters.boundingBox.width;
    float widthFirstHalf =
      ((ellipsisPosition != Text::EllipsisPosition::MIDDLE) ? targetWidth
                                                            : targetWidth - std::floor(targetWidth / 2));

    bool isSecondHalf = false;
    // Character Spacing
    const float             modelCharacterSpacing     = parameters.textModel->mVisualModel->GetCharacterSpacing();
    float                   calculatedAdvance         = 0.f;
    Vector<CharacterIndex>& glyphToCharacterMap       = parameters.textModel->mVisualModel->mGlyphsToCharacters;
    const CharacterIndex*   glyphToCharacterMapBuffer = glyphToCharacterMap.Begin();

    // Get the character-spacing runs.
    const Vector<CharacterSpacingGlyphRun>& characterSpacingGlyphRuns =
      parameters.textModel->mVisualModel->GetCharacterSpacingGlyphRuns();

    GlyphMetrics glyphMetrics;
    const float  characterSpacing =
      GetGlyphCharacterSpacing(lineLayout.glyphIndex, characterSpacingGlyphRuns, modelCharacterSpacing);
    calculatedAdvance = GetCalculatedAdvance(*(textBuffer + (*(glyphToCharacterMapBuffer + lineLayout.glyphIndex))),
                                             characterSpacing, (*(glyphsBuffer + lineLayout.glyphIndex)).advance);
    GetGlyphsMetrics(lineLayout.glyphIndex, numberOfGLyphsInGroup, glyphMetrics, glyphsBuffer, mMetrics,
                     calculatedAdvance);

    // Set the direction of the first character of the line.
    lineLayout.characterIndex = *(glyphsToCharactersBuffer + lineLayout.glyphIndex);

    // Stores temporary line layout which has not been added to the final line layout.
    LineLayout tmpLineLayout;

    // Initialize the start point.

    // The initial start point is zero. However it needs a correction according the 'x' bearing of the first glyph.
    // i.e. if the bearing of the first glyph is negative it may exceed the boundaries of the text area.
    // It needs to add as well space for the cursor if the text is in edit mode and extra space in case the text is
    // outlined.

    tmpLineLayout.penX = GetCursorInsetWidth() + outlineWidth;
    if(parameters.textModel->mRemoveFrontInset)
    {
      tmpLineLayout.penX -= glyphMetrics.xBearing;
    }

    tmpLineLayout.relativeLineSize = lineLayout.relativeLineSize;

    // Calculate the line height if there is no characters.
    FontId     lastFontId         = glyphMetrics.fontId;
    const bool collectTextMetrics = parameters.replacementLayoutData != nullptr;
    UpdateLineHeight(glyphMetrics, tmpLineLayout);
    if(collectTextMetrics)
    {
      UpdateReplacementTextMetrics(glyphMetrics, tmpLineLayout);
    }

    bool       oneWordLaidOut   = false;
    bool       oneHyphenLaidOut = false;
    GlyphIndex hyphenIndex      = 0;
    GlyphInfo  hyphenGlyph;

    for(GlyphIndex glyphIndex = lineLayout.glyphIndex; glyphIndex < lastGlyphOfParagraphPlusOne;)
    {
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "  glyph index : %d\n", glyphIndex);

      // Check whether this glyph comes from a character that is shaped in multiple glyphs.
      const Length numberOfGLyphsInGroup =
        GetNumberOfGlyphsOfGroup(glyphIndex, lastGlyphOfParagraphPlusOne, charactersPerGlyphBuffer);

      GlyphMetrics glyphMetrics;
      const float  characterSpacing =
        GetGlyphCharacterSpacing(glyphIndex, characterSpacingGlyphRuns, modelCharacterSpacing);
      calculatedAdvance = GetCalculatedAdvance(*(textBuffer + (*(glyphToCharacterMapBuffer + glyphIndex))),
                                               characterSpacing, (*(glyphsBuffer + glyphIndex)).advance);
      GetGlyphsMetrics(glyphIndex, numberOfGLyphsInGroup, glyphMetrics, glyphsBuffer, mMetrics, calculatedAdvance);

      const bool isLastGlyph = glyphIndex + numberOfGLyphsInGroup == totalNumberOfGlyphs;

      // Check if the font of the current glyph is the same of the previous one.
      // If it's different the ascender and descender need to be updated.
      if(lastFontId != glyphMetrics.fontId)
      {
        UpdateLineHeight(glyphMetrics, tmpLineLayout);
        if(collectTextMetrics)
        {
          UpdateReplacementTextMetrics(glyphMetrics, tmpLineLayout);
        }
        lastFontId = glyphMetrics.fontId;
      }

      // Get the character indices for the current glyph. The last character index is needed
      // because there are glyphs formed by more than one character but their break info is
      // given only for the last character.
      const Length         charactersPerGlyph  = *(charactersPerGlyphBuffer + glyphIndex + numberOfGLyphsInGroup - 1u);
      const bool           hasCharacters       = charactersPerGlyph > 0u;
      const CharacterIndex characterFirstIndex = *(glyphsToCharactersBuffer + glyphIndex);
      const CharacterIndex characterLastIndex  = characterFirstIndex + (hasCharacters ? charactersPerGlyph - 1u : 0u);

      // Get the line break info for the current character.
      const LineBreakInfo lineBreakInfo =
        hasCharacters ? *(lineBreakInfoBuffer + characterLastIndex) : TextAbstraction::LINE_NO_BREAK;

      if(isSecondHalf)
      {
        // Increase the number of characters.
        tmpLineLayout.numberOfCharactersInSecondHalfLine += charactersPerGlyph;

        // Increase the number of glyphs.
        tmpLineLayout.numberOfGlyphsInSecondHalfLine += numberOfGLyphsInGroup;
      }
      else
      {
        // Increase the number of characters.
        tmpLineLayout.numberOfCharacters += charactersPerGlyph;

        // Increase the number of glyphs.
        tmpLineLayout.numberOfGlyphs += numberOfGLyphsInGroup;
      }

      // Check whether is a white space.
      const Character character    = *(textBuffer + characterFirstIndex);
      const bool      isWhiteSpace = TextAbstraction::IsWhiteSpace(character);

      // Calculate the length of the line.

      // Used to restore the temporal line layout when a single word does not fit in the control's width and is split by
      // character.
      const float previousTmpPenX                      = tmpLineLayout.penX;
      const float previousTmpAdvance                   = tmpLineLayout.previousAdvance;
      const float previousTmpLength                    = tmpLineLayout.length;
      const float previousTmpWhiteSpaceLengthEndOfLine = tmpLineLayout.whiteSpaceLengthEndOfLine;

      // The calculated text size is used in atlas renderer.
      // When the text is all white space, partial render issue occurs because the width is 0.
      // To avoid issue, do not remove the white space size in hidden input mode.
      if(isWhiteSpace && !hiddenInputEnabled)
      {
        // Add the length to the length of white spaces at the end of the line.
        tmpLineLayout.whiteSpaceLengthEndOfLine += glyphMetrics.advance;
        // The advance is used as the width is always zero for the white spaces.
      }
      else
      {
        tmpLineLayout.penX += tmpLineLayout.previousAdvance + tmpLineLayout.whiteSpaceLengthEndOfLine;
        tmpLineLayout.previousAdvance = (glyphMetrics.advance + parameters.interGlyphExtraAdvance);

        if(parameters.textModel->mRemoveBackInset)
        {
          tmpLineLayout.length =
            std::max(tmpLineLayout.length, tmpLineLayout.penX + glyphMetrics.xBearing + glyphMetrics.width);
        }
        else
        {
          tmpLineLayout.length = std::max(tmpLineLayout.length, tmpLineLayout.penX + glyphMetrics.advance);
        }

        // Clear the white space length at the end of the line.
        tmpLineLayout.whiteSpaceLengthEndOfLine = 0.f;
      }

      if(isSplitToTwoHalves && (!isSecondHalf) &&
         (tmpLineLayout.length + tmpLineLayout.whiteSpaceLengthEndOfLine > widthFirstHalf))
      {
        tmpLineLayout.numberOfCharacters -= charactersPerGlyph;
        tmpLineLayout.numberOfGlyphs -= numberOfGLyphsInGroup;

        tmpLineLayout.numberOfCharactersInSecondHalfLine += charactersPerGlyph;
        tmpLineLayout.numberOfGlyphsInSecondHalfLine += numberOfGLyphsInGroup;

        tmpLineLayout.glyphIndexInSecondHalfLine     = tmpLineLayout.glyphIndex + tmpLineLayout.numberOfGlyphs;
        tmpLineLayout.characterIndexInSecondHalfLine = tmpLineLayout.characterIndex + tmpLineLayout.numberOfCharacters;

        tmpLineLayout.isSplitToTwoHalves = isSecondHalf = true;
      }
      // Check if the accumulated length fits in the width of the box.
      if((ellipsisPosition == Text::EllipsisPosition::START ||
          (ellipsisPosition == Text::EllipsisPosition::MIDDLE && isSecondHalf)) &&
         completelyFill && !isMultiline &&
         (tmpLineLayout.length + tmpLineLayout.whiteSpaceLengthEndOfLine > targetWidth))
      {
        GlyphIndex glyphIndexToRemove =
          isSecondHalf ? tmpLineLayout.glyphIndexInSecondHalfLine : tmpLineLayout.glyphIndex;

        while(tmpLineLayout.length + tmpLineLayout.whiteSpaceLengthEndOfLine > targetWidth &&
              glyphIndexToRemove < glyphIndex)
        {
          GlyphMetrics glyphMetrics;
          const float  characterSpacing =
            GetGlyphCharacterSpacing(glyphIndexToRemove, characterSpacingGlyphRuns, modelCharacterSpacing);
          calculatedAdvance = GetCalculatedAdvance(*(textBuffer + (*(glyphToCharacterMapBuffer + glyphIndexToRemove))),
                                                   characterSpacing, (*(glyphsBuffer + glyphIndexToRemove)).advance);
          GetGlyphsMetrics(glyphIndexToRemove, numberOfGLyphsInGroup, glyphMetrics, glyphsBuffer, mMetrics,
                           calculatedAdvance);

          const Length numberOfGLyphsInGroup =
            GetNumberOfGlyphsOfGroup(glyphIndexToRemove, lastGlyphOfParagraphPlusOne, charactersPerGlyphBuffer);

          const Length charactersPerGlyph =
            *(charactersPerGlyphBuffer + glyphIndexToRemove + numberOfGLyphsInGroup - 1u);
          const bool           hasCharacters       = charactersPerGlyph > 0u;
          const CharacterIndex characterFirstIndex = *(glyphsToCharactersBuffer + glyphIndexToRemove);
          const CharacterIndex characterLastIndex =
            characterFirstIndex + (hasCharacters ? charactersPerGlyph - 1u : 0u);

          // Check whether is a white space.
          const Character character                = *(textBuffer + characterFirstIndex);
          const bool      isRemovedGlyphWhiteSpace = TextAbstraction::IsWhiteSpace(character);

          if(isSecondHalf)
          {
            // Decrease the number of characters for SecondHalf.
            tmpLineLayout.numberOfCharactersInSecondHalfLine -= charactersPerGlyph;

            // Decrease the number of glyphs for SecondHalf.
            tmpLineLayout.numberOfGlyphsInSecondHalfLine -= numberOfGLyphsInGroup;
          }
          else
          {
            // Decrease the number of characters.
            tmpLineLayout.numberOfCharacters -= charactersPerGlyph;

            // Decrease the number of glyphs.
            tmpLineLayout.numberOfGlyphs -= numberOfGLyphsInGroup;
          }

          if(isRemovedGlyphWhiteSpace && !hiddenInputEnabled)
          {
            tmpLineLayout.penX -= glyphMetrics.advance;
            tmpLineLayout.length -= glyphMetrics.advance;
          }
          else
          {
            tmpLineLayout.penX -= (glyphMetrics.advance + parameters.interGlyphExtraAdvance);
            tmpLineLayout.length -= (std::min(glyphMetrics.advance + parameters.interGlyphExtraAdvance,
                                              glyphMetrics.xBearing + glyphMetrics.width));
          }

          if(isSecondHalf)
          {
            tmpLineLayout.glyphIndexInSecondHalfLine += numberOfGLyphsInGroup;
            tmpLineLayout.characterIndexInSecondHalfLine = characterLastIndex + 1u;
            glyphIndexToRemove                           = tmpLineLayout.glyphIndexInSecondHalfLine;
          }
          else
          {
            tmpLineLayout.glyphIndex += numberOfGLyphsInGroup;
            tmpLineLayout.characterIndex = characterLastIndex + 1u;
            glyphIndexToRemove           = tmpLineLayout.glyphIndex;
          }
        }
      }
      else if((completelyFill || isMultiline) && (tmpLineLayout.length > targetWidth))
      {
        // Current word does not fit in the box's width.
        if(((oneHyphenLaidOut && isHyphenMode) || (!oneWordLaidOut && isMixedMode && oneHyphenLaidOut)) &&
           !completelyFill)
        {
          parameters.textModel->mVisualModel->mHyphen.glyph.PushBack(hyphenGlyph);
          parameters.textModel->mVisualModel->mHyphen.index.PushBack(hyphenIndex + 1);
        }

        if((!oneWordLaidOut && !oneHyphenLaidOut) || completelyFill)
        {
          DALI_LOG_INFO(gLogFilter, Debug::Verbose, "  Break the word by character\n");

          // The word doesn't fit in the control's width. It needs to be split by character.
          const Length committedGlyphCount = lineLayout.numberOfGlyphs + lineLayout.numberOfGlyphsInSecondHalfLine;
          const Length candidateGlyphCount = tmpLineLayout.numberOfGlyphs +
                                             tmpLineLayout.numberOfGlyphsInSecondHalfLine;
          const bool firstGlyphGroupExceedsWidth = parameters.replacementLayoutData != nullptr &&
                                                   committedGlyphCount == 0u &&
                                                   candidateGlyphCount == numberOfGLyphsInGroup;
          if(candidateGlyphCount > 0u && !firstGlyphGroupExceedsWidth)
          {
            if(isSecondHalf)
            {
              tmpLineLayout.numberOfCharactersInSecondHalfLine -= charactersPerGlyph;
              tmpLineLayout.numberOfGlyphsInSecondHalfLine -= numberOfGLyphsInGroup;
            }
            else
            {
              tmpLineLayout.numberOfCharacters -= charactersPerGlyph;
              tmpLineLayout.numberOfGlyphs -= numberOfGLyphsInGroup;
            }

            tmpLineLayout.penX                      = previousTmpPenX;
            tmpLineLayout.previousAdvance           = previousTmpAdvance;
            tmpLineLayout.length                    = previousTmpLength;
            tmpLineLayout.whiteSpaceLengthEndOfLine = previousTmpWhiteSpaceLengthEndOfLine;
          }

          // A box narrower than one synthetic replacement must still make
          // layout progress. Keep that atomic box on the line and let the
          // renderer clip it; returning an empty line would abort the
          // remaining paragraph.
          if(ellipsisPosition == Text::EllipsisPosition::START && !isMultiline)
          {
            // Add part of the word to the line layout and shift the first glyph.
            MergeLineLayout(lineLayout, tmpLineLayout, true);
          }
          else if(ellipsisPosition != Text::EllipsisPosition::START ||
                  (ellipsisPosition == Text::EllipsisPosition::START && (!completelyFill)))
          {
            // Add part of the word to the line layout.
            MergeLineLayout(lineLayout, tmpLineLayout, false);
          }
        }
        else
        {
          DALI_LOG_INFO(gLogFilter, Debug::Verbose, "  Current word does not fit.\n");
        }

        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--GetLineLayoutForBox.\n");

        // Reorder the RTL line.
        if(bidiParameters.isBidirectional)
        {
          ReorderBiDiLayout(parameters, bidiParameters, lineLayout, lineLayout, true, enforceEllipsisInSingleLine);
        }

        return;
      }

      if((isMultiline || isLastGlyph) && (TextAbstraction::LINE_MUST_BREAK == lineBreakInfo))
      {
        LineLayout currentLineLayout = lineLayout;
        oneHyphenLaidOut             = false;

        if(ellipsisPosition == Text::EllipsisPosition::START && !isMultiline)
        {
          // Must break the line. Update the line layout, shift the first glyph and return.
          MergeLineLayout(lineLayout, tmpLineLayout, true);
        }
        else
        {
          // Must break the line. Update the line layout and return.
          MergeLineLayout(lineLayout, tmpLineLayout, false);
        }

        // Reorder the RTL line.
        if(bidiParameters.isBidirectional)
        {
          ReorderBiDiLayout(parameters, bidiParameters, currentLineLayout, lineLayout, false,
                            enforceEllipsisInSingleLine);
        }

        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "  Must break\n");
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--GetLineLayoutForBox\n");

        return;
      }

      if(isMultiline && (TextAbstraction::LINE_ALLOW_BREAK == lineBreakInfo))
      {
        oneHyphenLaidOut = false;
        oneWordLaidOut   = isWordLaidOut;
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "  One word laid-out\n");

        // Current glyph is the last one of the current word.
        // Add the temporal layout to the current one.
        MergeLineLayout(lineLayout, tmpLineLayout, false);

        tmpLineLayout.Clear();
      }

      if(isMultiline && ((isHyphenMode || (!oneWordLaidOut && isMixedMode))) &&
         (TextAbstraction::LINE_HYPHENATION_BREAK == lineBreakInfo))
      {
        hyphenGlyph        = GlyphInfo();
        hyphenGlyph.fontId = glyphsBuffer[glyphIndex].fontId;

        TextAbstraction::FontClient fontClient = parameters.fontClient;
        hyphenGlyph.index                      = fontClient.GetGlyphIndex(hyphenGlyph.fontId, HYPHEN_UNICODE);

        mMetrics->GetGlyphMetrics(&hyphenGlyph, 1);

        if((tmpLineLayout.length + hyphenGlyph.width) <= targetWidth)
        {
          hyphenIndex      = glyphIndex;
          oneHyphenLaidOut = true;

          DALI_LOG_INFO(gLogFilter, Debug::Verbose, "  One hyphen laid-out\n");

          // Current glyph is the last one of the current word hyphen.
          // Add the temporal layout to the current one.
          MergeLineLayout(lineLayout, tmpLineLayout, false);

          tmpLineLayout.Clear();
        }
      }

      glyphIndex += numberOfGLyphsInGroup;
    }

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--GetLineLayoutForBox\n");
  }

  void SetGlyphPositions(const Parameters& layoutParameters, Vector2* glyphPositionsBuffer, const LineLayout& layout)
  {
    // Traverse the glyphs and set the positions.

    const GlyphInfo* const glyphsBuffer = layoutParameters.textModel->mVisualModel->mGlyphs.Begin();
    const float            outlineWidth =
      layoutParameters.textModel->IsOutlineEnabled()
                   ? static_cast<float>(layoutParameters.textModel->GetOutlineWidth())
                   : 0.0f;
    const Length numberOfGlyphs         = layout.numberOfGlyphs;
    const float  interGlyphExtraAdvance = layoutParameters.interGlyphExtraAdvance;

    const GlyphIndex startIndexForGlyph          = layout.glyphIndex;
    const GlyphIndex startIndexForGlyphPositions = startIndexForGlyph - layoutParameters.startGlyphIndex;

    // Check if the x bearing of the first character is negative.
    // If it has a negative x bearing, it will exceed the boundaries of the actor,
    // so the penX position needs to be moved to the right.
    const GlyphInfo& glyph = *(glyphsBuffer + startIndexForGlyph);
    float            penX  = GetCursorInsetWidth() + outlineWidth;

    if(layoutParameters.textModel->mRemoveFrontInset)
    {
      penX -= glyph.xBearing;
    }

    CalculateGlyphPositionsLTR(layoutParameters.textModel->mVisualModel, layoutParameters.textModel->mLogicalModel,
                               interGlyphExtraAdvance, numberOfGlyphs,
                               startIndexForGlyph, // startIndexForGlyph is the index of the first glyph in the line
                               startIndexForGlyphPositions, glyphPositionsBuffer, penX);

    if(layout.isSplitToTwoHalves)
    {
      const GlyphIndex startIndexForGlyphInSecondHalf = layout.glyphIndexInSecondHalfLine;
      const Length     numberOfGlyphsInSecondHalfLine = layout.numberOfGlyphsInSecondHalfLine;
      const GlyphIndex startIndexForGlyphPositionsnSecondHalf =
        layout.glyphIndexInSecondHalfLine - layoutParameters.startGlyphIndex;

      CalculateGlyphPositionsLTR(
        layoutParameters.textModel->mVisualModel, layoutParameters.textModel->mLogicalModel, interGlyphExtraAdvance,
        numberOfGlyphsInSecondHalfLine,
        startIndexForGlyphInSecondHalf, // startIndexForGlyph is the index of the first glyph in the line
        startIndexForGlyphPositionsnSecondHalf, glyphPositionsBuffer, penX);
    }
  }

  void SetGlyphPositions(const Parameters& layoutParameters, Vector2* glyphPositionsBuffer,
                         LayoutBidiParameters& layoutBidiParameters, const LineLayout& layout)
  {
    const BidirectionalLineInfoRun& bidiLine =
      layoutParameters.textModel->mLogicalModel->mBidirectionalLineInfo[layoutBidiParameters.bidiLineIndex];
    const GlyphInfo* const  glyphsBuffer = layoutParameters.textModel->mVisualModel->mGlyphs.Begin();
    const GlyphIndex* const charactersToGlyphsBuffer =
      layoutParameters.textModel->mVisualModel->mCharactersToGlyph.Begin();

    CharacterIndex characterLogicalIndex = 0u;
    CharacterIndex characterVisualIndex =
      bidiLine.characterRunForSecondHalfLine.characterIndex +
      (bidiLine.visualToLogicalMapSecondHalf ? *(bidiLine.visualToLogicalMapSecondHalf + characterLogicalIndex) : 0u);
    bool extendedToSecondHalf = false; // Whether the logical index is extended to second half

    float penX = 0.f;

    if(layout.isSplitToTwoHalves)
    {
      CalculateGlyphPositionsRTL(layoutParameters.textModel->mVisualModel, layoutParameters.textModel->mLogicalModel,
                                 layoutBidiParameters.bidiLineIndex, layoutParameters.startGlyphIndex,
                                 glyphPositionsBuffer, characterVisualIndex, characterLogicalIndex, penX);
    }

    if(characterLogicalIndex == bidiLine.characterRunForSecondHalfLine.numberOfCharacters)
    {
      extendedToSecondHalf  = true;
      characterLogicalIndex = 0u;
      characterVisualIndex =
        bidiLine.characterRun.characterIndex +
        (bidiLine.visualToLogicalMap ? *(bidiLine.visualToLogicalMap + characterLogicalIndex) : 0u);

      CalculateGlyphPositionsRTL(layoutParameters.textModel->mVisualModel, layoutParameters.textModel->mLogicalModel,
                                 layoutBidiParameters.bidiLineIndex, layoutParameters.startGlyphIndex,
                                 glyphPositionsBuffer, characterVisualIndex, characterLogicalIndex, penX);
    }

    const GlyphIndex glyphIndex = *(charactersToGlyphsBuffer + characterVisualIndex);
    const GlyphInfo& glyph      = *(glyphsBuffer + glyphIndex);

    penX += -glyph.xBearing;

    // Traverses the characters of the right to left paragraph.
    if(layout.isSplitToTwoHalves && !extendedToSecondHalf)
    {
      TraversesCharactersForGlyphPositionsRTL(
        layoutParameters.textModel->mVisualModel, layoutParameters.textModel->mLogicalModel->mText.Begin(),
        layoutParameters.startGlyphIndex, layoutParameters.interGlyphExtraAdvance,
        bidiLine.characterRunForSecondHalfLine, bidiLine.visualToLogicalMapSecondHalf, glyphPositionsBuffer,
        characterLogicalIndex, penX);
    }

    characterLogicalIndex = extendedToSecondHalf ? characterLogicalIndex : 0u;

    TraversesCharactersForGlyphPositionsRTL(
      layoutParameters.textModel->mVisualModel, layoutParameters.textModel->mLogicalModel->mText.Begin(),
      layoutParameters.startGlyphIndex, layoutParameters.interGlyphExtraAdvance, bidiLine.characterRun,
      bidiLine.visualToLogicalMap, glyphPositionsBuffer, characterLogicalIndex, penX);
  }

  /**
   * @brief Resizes the line buffer.
   *
   * @param[in,out] lines The vector of lines. Used when the layout is created from scratch.
   * @param[in,out] newLines The vector of lines used instead of @p lines when the layout is updated.
   * @param[in,out] linesCapacity The capacity of the vector (either lines or newLines).
   * @param[in] updateCurrentBuffer Whether the layout is updated.
   *
   * @return Pointer to either lines or newLines.
   */
  LineRun* ResizeLinesBuffer(Vector<LineRun>& lines, Vector<LineRun>& newLines, Length& linesCapacity,
                             bool updateCurrentBuffer)
  {
    LineRun* linesBuffer = nullptr;
    // Reserve more space for the next lines.
    linesCapacity *= 2u;
    if(updateCurrentBuffer)
    {
      newLines.Resize(linesCapacity);
      linesBuffer = newLines.Begin();
    }
    else
    {
      lines.Resize(linesCapacity);
      linesBuffer = lines.Begin();
    }

    return linesBuffer;
  }

  /**
   * Ellipsis a line if it exceeds the width's of the bounding box.
   *
   * @param[in] layoutParameters The parameters needed to layout the text.
   * @param[in] layout The line layout.
   * @param[in,out] layoutSize The text's layout size.
   * @param[in,out] linesBuffer Pointer to the line's buffer.
   * @param[in,out] glyphPositionsBuffer Pointer to the position's buffer.
   * @param[in,out] numberOfLines The number of laid-out lines.
   * @param[in] penY The vertical layout position.
   * @param[in] currentParagraphDirection The current paragraph's direction.
   * @param[in,out] isMarqueeEnabled If the isMarqueeEnabled is true and the height of the text exceeds the
   * boundaries of the control the text is elided and the isMarqueeEnabled is set to false to disable the marquee
   * @param[in] isHiddenInputEnabled Whether the hidden input is enabled.
   * @param[in] ellipsisPosition Where is the location the text elide
   *
   * return Whether the line is ellipsized.
   */
  bool EllipsisLine(const Parameters& layoutParameters, LayoutBidiParameters& layoutBidiParameters,
                    const LineLayout& layout, Size& layoutSize, LineRun* linesBuffer, Vector2* glyphPositionsBuffer,
                    Length& numberOfLines, float penY, bool& isMarqueeEnabled, bool isMarqueeMaxTextureExceeded,
                    bool isHiddenInputEnabled, Text::EllipsisPosition::Type ellipsisPosition,
                    bool enforceEllipsisInSingleLine)
  {
    const bool  hasReplacementLayout = layoutParameters.replacementLayoutData != nullptr;
    const float candidateLineSpacing =
      hasReplacementLayout && layout.containsReplacement
        ? layout.lineSpacing
        : GetLineSpacing(layout.ascender - layout.descender, mRelativeLineSize);
    const bool ellipsis =
      enforceEllipsisInSingleLine ||
      (isMarqueeEnabled
         ? isMarqueeMaxTextureExceeded
         : (((mLayout == MULTI_LINE_BOX) &&
             !((numberOfLines == 0) && (layout.length <= layoutParameters.boundingBox.width)) &&
             (penY - layout.descender +
                std::max(0.0f, candidateLineSpacing) >
              layoutParameters.boundingBox.height)) ||
            ((mLayout == SINGLE_LINE_BOX) && (layout.length > layoutParameters.boundingBox.width))));

    const bool isMultiline = !enforceEllipsisInSingleLine && (mLayout == MULTI_LINE_BOX);
    if(ellipsis && (ellipsisPosition == Text::EllipsisPosition::END || !isMultiline))
    {
      if(penY - layout.descender > layoutParameters.boundingBox.height)
      {
        // Even if marquee is enabled and text is bigger than max texture size,
        // if the the height is small, marquee should not work.
        isMarqueeEnabled = false;
      }

      // Do not layout more lines if ellipsis is enabled.
      // The last line needs to be completely filled with characters.
      // Part of a word may be used.

      LineRun*   lineRun = nullptr;
      LineLayout ellipsisLayout;
      float      layoutHeightBeforeEllipsisLine = layoutSize.height;
      GlyphIndex previousEllipsisLineGlyphEnd   = 0u;

      ellipsisLayout.relativeLineSize = layout.relativeLineSize;

      if(0u != numberOfLines)
      {
        // Get the last line and layout it again with the 'completelyFill' flag to true.
        lineRun = linesBuffer + (numberOfLines - 1u);
        penY -= layout.ascender - lineRun->descender + lineRun->lineSpacing;

        // UpdateTextLayout accounted for this line as a non-final line. The
        // ellipsis recomposition below may change its metrics, so retain only
        // the height of the preceding visible lines.
        if(hasReplacementLayout)
        {
          layoutHeightBeforeEllipsisLine =
            std::max(0.0f, layoutSize.height - GetLineHeight(*lineRun, false));
          previousEllipsisLineGlyphEnd =
            lineRun->glyphRun.glyphIndex + lineRun->glyphRun.numberOfGlyphs;
          if(lineRun->isSplitToTwoHalves)
          {
            previousEllipsisLineGlyphEnd =
              std::max(previousEllipsisLineGlyphEnd,
                       lineRun->glyphRunSecondHalf.glyphIndex + lineRun->glyphRunSecondHalf.numberOfGlyphs);
          }
        }

        ellipsisLayout.glyphIndex = lineRun->glyphRun.glyphIndex;
      }
      else
      {
        // At least there is space reserved for one line.
        lineRun = linesBuffer;

        lineRun->glyphRun.glyphIndex = 0u;
        ellipsisLayout.glyphIndex    = 0u;
        lineRun->isSplitToTwoHalves  = false;

        ++numberOfLines;
      }

      const LayoutBidiParameters bidiParametersBeforeEllipsis = layoutBidiParameters;
      GetLineLayoutForBox(layoutParameters, layoutBidiParameters, ellipsisLayout, true, ellipsisPosition,
                          enforceEllipsisInSingleLine, true, isHiddenInputEnabled);
      CharacterDirection ellipsisLineDirection         = LTR;
      bool               ellipsisLinePositionsPrepared = false;
      EndEllipsisPlan    endEllipsisPlan;
      if(ellipsisPosition == Text::EllipsisPosition::END && hasReplacementLayout)
      {
        endEllipsisPlan               = ResolveEndEllipsisCandidatePlan(layoutParameters,
                                                                        layoutBidiParameters,
                                                                        ellipsisLayout,
                                                                        glyphPositionsBuffer,
                                                                        ellipsisLineDirection);
        ellipsisLinePositionsPrepared = ellipsisLayout.numberOfGlyphs > 0u;
      }
      if(hasReplacementLayout)
      {
        ApplyReplacementLineMetrics(layoutParameters,
                                    ellipsisLayout,
                                    endEllipsisPlan.firstRemovedReplacementGlyphIndex,
                                    endEllipsisPlan.lastRemovedReplacementGlyphIndex);
      }

      // Completely filling the previous line may pull an oversized
      // replacement from the vertically rejected candidate line. That
      // replacement is later removed atomically, but its metrics would still
      // push the semantic ellipsis outside the control. Recompose the same
      // host line once, bounded immediately before the first newly introduced
      // replacement. Existing replacements already owned by the visible host
      // line remain eligible and preserve the ordinary fast path.
      const float recomposedLineHeight = ellipsisLayout.ascender - ellipsisLayout.descender +
                                         std::max(0.0f, ellipsisLayout.lineSpacing);
      if(0u != numberOfLines &&
         ellipsisPosition == Text::EllipsisPosition::END &&
         hasReplacementLayout &&
         layoutHeightBeforeEllipsisLine + recomposedLineHeight > layoutParameters.boundingBox.height)
      {
        const VisualModel& visualModel              = *layoutParameters.textModel->mVisualModel;
        GlyphIndex         firstNewReplacementGlyph = static_cast<Dali::Ui::Text::GlyphIndex>(visualModel.mGlyphs.Count());
        const GlyphIndex   recomposedEnd =
          std::min<GlyphIndex>(ellipsisLayout.glyphIndex + ellipsisLayout.numberOfGlyphs,
                               static_cast<GlyphIndex>(visualModel.mGlyphs.Count()));
        const GlyphIndex replacementSearchEnd =
          std::min<GlyphIndex>(recomposedEnd + (recomposedEnd < visualModel.mGlyphs.Count() ? 1u : 0u),
                               static_cast<GlyphIndex>(visualModel.mGlyphs.Count()));
        for(GlyphIndex glyphIndex = std::max(previousEllipsisLineGlyphEnd, ellipsisLayout.glyphIndex);
            glyphIndex < replacementSearchEnd;
            ++glyphIndex)
        {
          if(IsSyntheticReplacementGlyph(visualModel.mGlyphs[glyphIndex]))
          {
            firstNewReplacementGlyph = glyphIndex;
            break;
          }
        }

        if(firstNewReplacementGlyph < replacementSearchEnd &&
           firstNewReplacementGlyph > ellipsisLayout.glyphIndex &&
           firstNewReplacementGlyph > layoutParameters.startGlyphIndex)
        {
          Parameters boundedParameters     = layoutParameters;
          boundedParameters.numberOfGlyphs = firstNewReplacementGlyph - boundedParameters.startGlyphIndex;

          LineLayout boundedEllipsisLayout;
          boundedEllipsisLayout.relativeLineSize     = layout.relativeLineSize;
          boundedEllipsisLayout.glyphIndex           = ellipsisLayout.glyphIndex;
          LayoutBidiParameters boundedBidiParameters = bidiParametersBeforeEllipsis;
          GetLineLayoutForBox(boundedParameters, boundedBidiParameters, boundedEllipsisLayout, true,
                              ellipsisPosition, enforceEllipsisInSingleLine, true, isHiddenInputEnabled);
          CharacterDirection    boundedLineDirection         = LTR;
          const EndEllipsisPlan boundedEndEllipsisPlan       = ResolveEndEllipsisCandidatePlan(boundedParameters,
                                                                                               boundedBidiParameters,
                                                                                               boundedEllipsisLayout,
                                                                                               glyphPositionsBuffer,
                                                                                               boundedLineDirection);
          const bool            boundedLinePositionsPrepared = boundedEllipsisLayout.numberOfGlyphs > 0u;
          ApplyReplacementLineMetrics(boundedParameters, boundedEllipsisLayout,
                                      boundedEndEllipsisPlan.firstRemovedReplacementGlyphIndex,
                                      boundedEndEllipsisPlan.lastRemovedReplacementGlyphIndex);
          if(boundedEllipsisLayout.numberOfGlyphs + boundedEllipsisLayout.numberOfGlyphsInSecondHalfLine > 0u)
          {
            ellipsisLayout                = boundedEllipsisLayout;
            layoutBidiParameters          = boundedBidiParameters;
            ellipsisLineDirection         = boundedLineDirection;
            ellipsisLinePositionsPrepared = boundedLinePositionsPrepared;
          }
        }
      }

      if(ellipsisPosition == Text::EllipsisPosition::START && !isMultiline)
      {
        lineRun->glyphRun.glyphIndex = ellipsisLayout.glyphIndex;
      }

      lineRun->glyphRun.numberOfGlyphs         = ellipsisLayout.numberOfGlyphs;
      lineRun->characterRun.characterIndex     = ellipsisLayout.characterIndex;
      lineRun->characterRun.numberOfCharacters = ellipsisLayout.numberOfCharacters;
      lineRun->width                           = ellipsisLayout.length;
      lineRun->extraLength                     = std::ceil(ellipsisLayout.whiteSpaceLengthEndOfLine);
      lineRun->ascender                        = ellipsisLayout.ascender;
      lineRun->descender                       = ellipsisLayout.descender;
      if(hasReplacementLayout)
      {
        lineRun->lineSpacing = ellipsisLayout.containsReplacement
                                 ? ellipsisLayout.lineSpacing
                                 : GetLineSpacing(lineRun->ascender - lineRun->descender,
                                                  ellipsisLayout.relativeLineSize);
      }
      lineRun->ellipsis = true;

      lineRun->isSplitToTwoHalves                               = ellipsisLayout.isSplitToTwoHalves;
      lineRun->glyphRunSecondHalf.glyphIndex                    = ellipsisLayout.glyphIndexInSecondHalfLine;
      lineRun->glyphRunSecondHalf.numberOfGlyphs                = ellipsisLayout.numberOfGlyphsInSecondHalfLine;
      lineRun->characterRunForSecondHalfLine.characterIndex     = ellipsisLayout.characterIndexInSecondHalfLine;
      lineRun->characterRunForSecondHalfLine.numberOfCharacters = ellipsisLayout.numberOfCharactersInSecondHalfLine;

      layoutSize.width = layoutParameters.boundingBox.width;
      if(hasReplacementLayout)
      {
        layoutSize.height  = layoutHeightBeforeEllipsisLine + GetLineHeight(*lineRun, true);
        lineRun->direction = ellipsisLinePositionsPrepared
                               ? ellipsisLineDirection
                               : SetEllipsisLineGlyphPositions(layoutParameters,
                                                               layoutBidiParameters,
                                                               ellipsisLayout,
                                                               glyphPositionsBuffer);
      }
      else
      {
        if(layoutSize.height < Math::MACHINE_EPSILON_1000)
        {
          layoutSize.height += GetLineHeight(*lineRun, true);
        }
        else if(lineRun->lineSpacing < 0.0f)
        {
          layoutSize.height -= lineRun->lineSpacing;
        }

        const Vector<BidirectionalLineInfoRun>& bidirectionalLinesInfo =
          layoutParameters.textModel->mLogicalModel->mBidirectionalLineInfo;
        if(layoutBidiParameters.isBidirectional)
        {
          layoutBidiParameters.bidiLineIndex = 0u;
          for(Vector<BidirectionalLineInfoRun>::ConstIterator it    = bidirectionalLinesInfo.Begin(),
                                                              endIt = bidirectionalLinesInfo.End();
              it != endIt;
              ++it, ++layoutBidiParameters.bidiLineIndex)
          {
            const BidirectionalLineInfoRun& run = *it;
            if(ellipsisLayout.characterIndex == run.characterRun.characterIndex &&
               ellipsisLayout.numberOfCharacters == run.characterRun.numberOfCharacters &&
               ellipsisLayout.characterIndexInSecondHalfLine == run.characterRunForSecondHalfLine.characterIndex &&
               ellipsisLayout.numberOfCharactersInSecondHalfLine == run.characterRunForSecondHalfLine.numberOfCharacters)
            {
              break;
            }
          }
        }

        const BidirectionalLineInfoRun* const bidirectionalLineInfo =
          (layoutBidiParameters.isBidirectional &&
           layoutBidiParameters.bidiLineIndex < bidirectionalLinesInfo.Count())
            ? &bidirectionalLinesInfo[layoutBidiParameters.bidiLineIndex]
            : nullptr;
        lineRun->direction = layoutBidiParameters.paragraphDirection;
        if(bidirectionalLineInfo &&
           ellipsisLayout.characterIndex == bidirectionalLineInfo->characterRun.characterIndex)
        {
          lineRun->direction = bidirectionalLineInfo->direction;
          if(!bidirectionalLineInfo->isIdentity)
          {
            SetGlyphPositions(layoutParameters, glyphPositionsBuffer, layoutBidiParameters, ellipsisLayout);
          }
          else
          {
            SetGlyphPositions(layoutParameters, glyphPositionsBuffer, ellipsisLayout);
          }
        }
        else
        {
          SetGlyphPositions(layoutParameters, glyphPositionsBuffer, ellipsisLayout);
        }
      }
    }

    return ellipsis;
  }

  /**
   * @brief Updates the text layout with a new laid-out line.
   *
   * @param[in] layoutParameters The parameters needed to layout the text.
   * @param[in] layout The line layout.
   * @param[in,out] layoutSize The text's layout size.
   * @param[in,out] linesBuffer Pointer to the line's buffer.
   * @param[in] index Index to the vector of glyphs.
   * @param[in,out] numberOfLines The number of laid-out lines.
   * @param[in] isLastLine Whether the laid-out line is the last one.
   */
  void UpdateTextLayout(const Parameters& layoutParameters, const LineLayout& layout, Size& layoutSize,
                        LineRun* linesBuffer, GlyphIndex index, Length& numberOfLines, bool isLastLine)
  {
    LineRun& lineRun = *(linesBuffer + numberOfLines);
    ++numberOfLines;

    lineRun.glyphRun.glyphIndex             = index;
    lineRun.glyphRun.numberOfGlyphs         = layout.numberOfGlyphs;
    lineRun.characterRun.characterIndex     = layout.characterIndex;
    lineRun.characterRun.numberOfCharacters = layout.numberOfCharacters;
    lineRun.width                           = layout.length;
    lineRun.extraLength                     = std::ceil(layout.whiteSpaceLengthEndOfLine);

    lineRun.isSplitToTwoHalves                               = layout.isSplitToTwoHalves;
    lineRun.glyphRunSecondHalf.glyphIndex                    = layout.glyphIndexInSecondHalfLine;
    lineRun.glyphRunSecondHalf.numberOfGlyphs                = layout.numberOfGlyphsInSecondHalfLine;
    lineRun.characterRunForSecondHalfLine.characterIndex     = layout.characterIndexInSecondHalfLine;
    lineRun.characterRunForSecondHalfLine.numberOfCharacters = layout.numberOfCharactersInSecondHalfLine;

    // Rounds upward to avoid a non integer size.
    lineRun.width = std::ceil(lineRun.width);

    lineRun.ascender  = layout.ascender;
    lineRun.descender = layout.descender;
    lineRun.direction = layout.direction;
    lineRun.ellipsis  = false;

    lineRun.lineSpacing = layout.containsReplacement
                            ? layout.lineSpacing
                            : GetLineSpacing(lineRun.ascender + -lineRun.descender, layout.relativeLineSize);

    // Update the actual size.
    if(lineRun.width > layoutSize.width)
    {
      layoutSize.width = lineRun.width;
    }

    layoutSize.height += GetLineHeight(lineRun, isLastLine);
  }

  /**
   * @brief Updates the text layout with the last laid-out line.
   *
   * @param[in] layoutParameters The parameters needed to layout the text.
   * @param[in] characterIndex The character index of the line.
   * @param[in] glyphIndex The glyph index of the line.
   * @param[in,out] layoutSize The text's layout size.
   * @param[in,out] linesBuffer Pointer to the line's buffer.
   * @param[in,out] numberOfLines The number of laid-out lines.
   */
  void UpdateTextLayout(const Parameters& layoutParameters, CharacterIndex characterIndex, GlyphIndex glyphIndex,
                        Size& layoutSize, LineRun* linesBuffer, Length& numberOfLines)
  {
    const Vector<GlyphInfo>& glyphs = layoutParameters.textModel->mVisualModel->mGlyphs;

    if(glyphs.Size() == 0u)
    {
      // Do nothing.
      return;
    }

    // Need to add a new line with no characters but with height to increase the layoutSize.height
    const GlyphInfo& glyphInfo = glyphs[glyphs.Count() - 1u];

    Text::FontMetrics fontMetrics;
    if(0u != glyphInfo.fontId)
    {
      mMetrics->GetFontMetrics(glyphInfo.fontId, fontMetrics);
    }

    LineRun& lineRun = *(linesBuffer + numberOfLines);
    ++numberOfLines;

    lineRun.glyphRun.glyphIndex             = glyphIndex;
    lineRun.glyphRun.numberOfGlyphs         = 0u;
    lineRun.characterRun.characterIndex     = characterIndex;
    lineRun.characterRun.numberOfCharacters = 0u;
    lineRun.width                           = 0.f;
    lineRun.ascender                        = fontMetrics.ascender;
    lineRun.descender                       = fontMetrics.descender;
    lineRun.extraLength                     = 0.f;
    lineRun.alignmentOffset                 = 0.f;
    lineRun.direction                       = LTR;
    lineRun.ellipsis                        = false;

    BoundedParagraphRun currentParagraphRun;
    LineLayout          tempLineLayout;
    (GetBoundedParagraph(layoutParameters.textModel->GetBoundedParagraphRuns(), characterIndex, currentParagraphRun)
       ? SetRelativeLineSize(&currentParagraphRun, tempLineLayout)
       : SetRelativeLineSize(nullptr, tempLineLayout));

    lineRun.lineSpacing = GetLineSpacing(lineRun.ascender + -lineRun.descender, tempLineLayout.relativeLineSize);

    layoutSize.height += GetLineHeight(lineRun, true);
  }

  /**
   * @brief Updates the text's layout size adding the size of the previously laid-out lines.
   *
   * @param[in] lines The vector of lines (before the new laid-out lines are inserted).
   * @param[in,out] layoutSize The text's layout size.
   */
  void UpdateLayoutSize(const Vector<LineRun>& lines, Size& layoutSize)
  {
    for(Vector<LineRun>::ConstIterator it = lines.Begin(), endIt = lines.End(); it != endIt; ++it)
    {
      const LineRun& line       = *it;
      bool           isLastLine = (it + 1 == endIt);

      if(line.width > layoutSize.width)
      {
        layoutSize.width = line.width;
      }

      layoutSize.height += GetLineHeight(line, isLastLine);
    }
  }

  /**
   * @brief Updates the indices of the character and glyph runs of the lines before the new lines are inserted.
   *
   * @param[in] layoutParameters The parameters needed to layout the text.
   * @param[in,out] lines The vector of lines (before the new laid-out lines are inserted).
   * @param[in] characterOffset The offset to be added to the runs of characters.
   * @param[in] glyphOffset The offset to be added to the runs of glyphs.
   */
  void UpdateLineIndexOffsets(const Parameters& layoutParameters, Vector<LineRun>& lines, Length characterOffset,
                              Length glyphOffset)
  {
    // Update the glyph and character runs.
    for(Vector<LineRun>::Iterator it = lines.Begin() + layoutParameters.startLineIndex, endIt = lines.End();
        it != endIt; ++it)
    {
      LineRun& line = *it;

      line.glyphRun.glyphIndex         = glyphOffset;
      line.characterRun.characterIndex = characterOffset;

      glyphOffset += line.glyphRun.numberOfGlyphs;
      characterOffset += line.characterRun.numberOfCharacters;
    }
  }

  /**
   * @brief Sets the relative line size for the LineLayout
   *
   * @param[in] currentParagraphRun Contains the bounded paragraph for this line layout.
   * @param[in,out] lineLayout The line layout to be updated.
   */
  void SetRelativeLineSize(BoundedParagraphRun* currentParagraphRun, LineLayout& lineLayout)
  {
    lineLayout.relativeLineSize = mRelativeLineSize;

    if(currentParagraphRun != nullptr && currentParagraphRun->relativeLineSizeDefined)
    {
      lineLayout.relativeLineSize = currentParagraphRun->relativeLineSize;
    }
  }

  /**
   * @brief Get the bounded paragraph for the characterIndex if exists.
   *
   * @param[in] boundedParagraphRuns The bounded paragraph list to search in.
   * @param[in] characterIndex The character index to get bounded paragraph for.
   * @param[out] currentParagraphRun Contains the bounded paragraph if found for the characterIndex.
   *
   * @return returns true if a bounded paragraph was found.
   */
  bool GetBoundedParagraph(const Vector<BoundedParagraphRun> boundedParagraphRuns, CharacterIndex characterIndex,
                           BoundedParagraphRun& currentParagraphRun)
  {
    for(Vector<BoundedParagraphRun>::Iterator it = boundedParagraphRuns.Begin(), endIt = boundedParagraphRuns.End();
        it != endIt; ++it)
    {
      BoundedParagraphRun& tempParagraphRun = *it;

      if(characterIndex >= tempParagraphRun.characterRun.characterIndex &&
         characterIndex <
           (tempParagraphRun.characterRun.characterIndex + tempParagraphRun.characterRun.numberOfCharacters))
      {
        currentParagraphRun = tempParagraphRun;
        return true;
      }
    }

    return false;
  }

  bool LayoutText(Parameters& layoutParameters, Size& layoutSize, bool elideTextEnabled, bool& isMarqueeEnabled,
                  bool isMarqueeMaxTextureExceeded, bool isHiddenInputEnabled,
                  Text::EllipsisPosition::Type ellipsisPosition)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->LayoutText\n");
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "  box size %f, %f\n", layoutParameters.boundingBox.width,
                  layoutParameters.boundingBox.height);

    layoutParameters.textModel->mVisualModel->mHyphen.glyph.Clear();
    layoutParameters.textModel->mVisualModel->mHyphen.index.Clear();

    // Reset indices of ElidedGlyphs
    layoutParameters.textModel->mVisualModel->SetStartIndexOfElidedGlyphs(0u);
    layoutParameters.textModel->mVisualModel->SetEndIndexOfElidedGlyphs(
      layoutParameters.textModel->GetNumberOfGlyphs() - 1u);
    layoutParameters.textModel->mVisualModel->SetFirstMiddleIndexOfElidedGlyphs(0u);
    layoutParameters.textModel->mVisualModel->SetSecondMiddleIndexOfElidedGlyphs(0u);

    Vector<LineRun>&                   lines                = layoutParameters.textModel->mVisualModel->mLines;
    const Vector<BoundedParagraphRun>& boundedParagraphRuns = layoutParameters.textModel->GetBoundedParagraphRuns();

    if(0u == layoutParameters.numberOfGlyphs)
    {
      // Add an extra line if the last character is a new paragraph character and the last line doesn't have zero
      // characters.
      if(layoutParameters.isLastNewParagraph)
      {
        Length numberOfLines = static_cast<Dali::Ui::Text::Length>(lines.Count());
        if(0u != numberOfLines)
        {
          const LineRun& lastLine = *(lines.End() - 1u);

          if(0u != lastLine.characterRun.numberOfCharacters)
          {
            // Need to add a new line with no characters but with height to increase the layoutSize.height
            LineRun newLine;
            Initialize(newLine);
            lines.PushBack(newLine);

            UpdateTextLayout(layoutParameters,
                             lastLine.characterRun.characterIndex + lastLine.characterRun.numberOfCharacters,
                             lastLine.glyphRun.glyphIndex + lastLine.glyphRun.numberOfGlyphs, layoutSize, lines.Begin(),
                             numberOfLines);
          }
        }
      }

      // Calculates the layout size.
      UpdateLayoutSize(lines, layoutSize);

      // Rounds upward to avoid a non integer size.
      layoutSize.height = std::ceil(layoutSize.height);

      // Nothing else do if there are no glyphs to layout.
      return false;
    }

    const GlyphIndex lastGlyphPlusOne    = layoutParameters.startGlyphIndex + layoutParameters.numberOfGlyphs;
    const Length     totalNumberOfGlyphs = static_cast<Dali::Ui::Text::Length>(layoutParameters.textModel->mVisualModel->mGlyphs.Count());
    Vector<Vector2>& glyphPositions      = layoutParameters.textModel->mVisualModel->mGlyphPositions;

    // In a previous layout, an extra line with no characters may have been added if the text ended with a new paragraph
    // character. This extra line needs to be removed.
    if(0u != lines.Count())
    {
      Vector<LineRun>::Iterator lastLine = lines.End() - 1u;

      if((0u == lastLine->characterRun.numberOfCharacters) && (lastGlyphPlusOne == totalNumberOfGlyphs))
      {
        lines.Remove(lastLine);
      }
    }

    // Retrieve BiDi info.
    const bool hasBidiParagraphs = !layoutParameters.textModel->mLogicalModel->mBidirectionalParagraphInfo.Empty();

    const CharacterIndex* const glyphsToCharactersBuffer =
      layoutParameters.textModel->mVisualModel->mGlyphsToCharacters.Begin();
    const Vector<BidirectionalParagraphInfoRun>& bidirectionalParagraphsInfo =
      layoutParameters.textModel->mLogicalModel->mBidirectionalParagraphInfo;
    const Vector<BidirectionalLineInfoRun>& bidirectionalLinesInfo =
      layoutParameters.textModel->mLogicalModel->mBidirectionalLineInfo;

    // Set the layout bidirectional paramters.
    LayoutBidiParameters layoutBidiParameters;

    // Whether the layout is being updated or set from scratch.
    const bool updateCurrentBuffer = layoutParameters.numberOfGlyphs < totalNumberOfGlyphs;

    Vector2*        glyphPositionsBuffer = nullptr;
    Vector<Vector2> newGlyphPositions;

    LineRun*        linesBuffer = nullptr;
    Vector<LineRun> newLines;

    // Estimate the number of lines.
    Length linesCapacity = std::max(1u, layoutParameters.estimatedNumberOfLines);
    Length numberOfLines = 0u;

    if(updateCurrentBuffer)
    {
      // Increase Vector size by 2 to prevent out-of-bounds access during Ellipsis calculation.
      newGlyphPositions.Resize(layoutParameters.numberOfGlyphs + 2);
      glyphPositionsBuffer = newGlyphPositions.Begin();

      newLines.Resize(linesCapacity);
      linesBuffer = newLines.Begin();
    }
    else
    {
      glyphPositionsBuffer = glyphPositions.Begin();

      lines.Resize(linesCapacity);
      linesBuffer = lines.Begin();
    }

    float penY            = CalculateLineOffset(lines, layoutParameters.startLineIndex);
    bool  anyLineIsEliped = false;
    for(GlyphIndex index = layoutParameters.startGlyphIndex; index < lastGlyphPlusOne;)
    {
      layoutBidiParameters.Clear();

      if(hasBidiParagraphs)
      {
        const CharacterIndex startCharacterIndex = *(glyphsToCharactersBuffer + index);

        for(Vector<BidirectionalParagraphInfoRun>::ConstIterator it    = bidirectionalParagraphsInfo.Begin(),
                                                                 endIt = bidirectionalParagraphsInfo.End();
            it != endIt; ++it, ++layoutBidiParameters.bidiParagraphIndex)
        {
          const BidirectionalParagraphInfoRun& run = *it;

          const CharacterIndex lastCharacterIndex =
            run.characterRun.characterIndex + run.characterRun.numberOfCharacters;

          if(lastCharacterIndex <= startCharacterIndex)
          {
            // Do not process, the paragraph has already been processed.
            continue;
          }

          if(startCharacterIndex >= run.characterRun.characterIndex && startCharacterIndex < lastCharacterIndex)
          {
            layoutBidiParameters.paragraphDirection = run.direction;
            layoutBidiParameters.isBidirectional    = true;
          }

          // Has already been found.
          break;
        }

        if(layoutBidiParameters.isBidirectional)
        {
          for(Vector<BidirectionalLineInfoRun>::ConstIterator it    = bidirectionalLinesInfo.Begin(),
                                                              endIt = bidirectionalLinesInfo.End();
              it != endIt; ++it, ++layoutBidiParameters.bidiLineIndex)
          {
            const BidirectionalLineInfoRun& run = *it;

            const CharacterIndex lastCharacterIndex =
              run.characterRun.characterIndex + run.characterRun.numberOfCharacters;

            if(lastCharacterIndex <= startCharacterIndex)
            {
              // skip
              continue;
            }

            if(startCharacterIndex < lastCharacterIndex)
            {
              // Found where to insert the bidi line info.
              break;
            }
          }
        }
      }

      CharacterDirection currentParagraphDirection = layoutBidiParameters.paragraphDirection;

      // Get the layout for the line.
      LineLayout layout;
      layout.direction  = layoutBidiParameters.paragraphDirection;
      layout.glyphIndex = index;

      BoundedParagraphRun currentParagraphRun;
      (GetBoundedParagraph(boundedParagraphRuns, *(glyphsToCharactersBuffer + index), currentParagraphRun)
         ? SetRelativeLineSize(&currentParagraphRun, layout)
         : SetRelativeLineSize(nullptr, layout));

      GetLineLayoutForBox(layoutParameters, layoutBidiParameters, layout, false, ellipsisPosition, false,
                          elideTextEnabled, isHiddenInputEnabled);
      if(layoutParameters.replacementLayoutData != nullptr)
      {
        ApplyReplacementLineMetrics(layoutParameters, layout);
      }

      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "           glyph index %d\n", layout.glyphIndex);
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "       character index %d\n", layout.characterIndex);
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "      number of glyphs %d\n", layout.numberOfGlyphs);
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "  number of characters %d\n", layout.numberOfCharacters);
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "                length %f\n", layout.length);

      CharacterIndex lastCharacterInParagraph =
        currentParagraphRun.characterRun.characterIndex + currentParagraphRun.characterRun.numberOfCharacters - 1;

      // check if this is the last line in paragraph, if false we should use the default relative line size (the one set
      // using the property)
      if(lastCharacterInParagraph >= layout.characterIndex &&
         lastCharacterInParagraph < layout.characterIndex + layout.numberOfCharacters)
      {
        layout.relativeLineSize = mRelativeLineSize;
      }

      if(0u == layout.numberOfGlyphs + layout.numberOfGlyphsInSecondHalfLine)
      {
        // The width is too small and no characters are laid-out.
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--LayoutText width too small!\n\n");

        lines.Resize(numberOfLines);

        // Rounds upward to avoid a non integer size.
        layoutSize.height = std::ceil(layoutSize.height);

        return false;
      }

      // Set the line position. DISCARD if ellipsis is enabled and the position exceeds the boundaries
      // of the box.
      penY += layout.ascender;

      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "  pen y %f\n", penY);

      bool ellipsis = false;
      if(elideTextEnabled)
      {
        layoutBidiParameters.paragraphDirection = currentParagraphDirection;

        // Does the ellipsis of the last line.
        ellipsis = EllipsisLine(layoutParameters, layoutBidiParameters, layout, layoutSize, linesBuffer,
                                glyphPositionsBuffer, numberOfLines, penY, isMarqueeEnabled,
                                isMarqueeMaxTextureExceeded, isHiddenInputEnabled, ellipsisPosition, false);
      }

      if(ellipsis && ((ellipsisPosition == Text::EllipsisPosition::END) || (numberOfLines == 1u)))
      {
        const bool isMultiline = mLayout == MULTI_LINE_BOX;
        if(isMultiline && ellipsisPosition != Text::EllipsisPosition::END)
        {
          ellipsis = EllipsisLine(layoutParameters, layoutBidiParameters, layout, layoutSize, linesBuffer,
                                  glyphPositionsBuffer, numberOfLines, penY, isMarqueeEnabled,
                                  isMarqueeMaxTextureExceeded, isHiddenInputEnabled, ellipsisPosition, true);
        }

        // clear hyphen from ellipsis line
        const Length* hyphenIndices = layoutParameters.textModel->mVisualModel->mHyphen.index.Begin();
        Length        hyphensCount  = static_cast<Dali::Ui::Text::Length>(layoutParameters.textModel->mVisualModel->mHyphen.glyph.Size());

        while(hyphenIndices && hyphensCount > 0 && hyphenIndices[hyphensCount - 1] >= layout.glyphIndex)
        {
          layoutParameters.textModel->mVisualModel->mHyphen.index.Remove(
            layoutParameters.textModel->mVisualModel->mHyphen.index.Begin() + hyphensCount - 1);
          layoutParameters.textModel->mVisualModel->mHyphen.glyph.Remove(
            layoutParameters.textModel->mVisualModel->mHyphen.glyph.Begin() + hyphensCount - 1);
          hyphensCount--;
        }

        if(!isMultiline)
        {
          // Recalculate line spacing and line height
          LineRun& firstLineRun = *(lines.Begin());
          firstLineRun.lineSpacing =
            GetLineSpacing(firstLineRun.ascender + -firstLineRun.descender, layout.relativeLineSize);
          layoutSize.height = GetLineHeight(firstLineRun, false);
        }

        // No more lines to layout.
        break;
      }
      else
      {
        // In START location of ellipsis whether to shift lines or not.
        anyLineIsEliped |= ellipsis;

        // Whether the last line has been laid-out.
        const bool isLastLine =
          index + (layout.numberOfGlyphs + layout.numberOfGlyphsInSecondHalfLine) == totalNumberOfGlyphs;

        if(numberOfLines == linesCapacity)
        {
          // Reserve more space for the next lines.
          linesBuffer = ResizeLinesBuffer(lines, newLines, linesCapacity, updateCurrentBuffer);
        }

        // Updates the current text's layout with the line's layout.
        UpdateTextLayout(layoutParameters, layout, layoutSize, linesBuffer, index, numberOfLines, isLastLine);

        const GlyphIndex nextIndex = index + layout.numberOfGlyphs + layout.numberOfGlyphsInSecondHalfLine;

        if((nextIndex == totalNumberOfGlyphs) && layoutParameters.isLastNewParagraph && (mLayout == MULTI_LINE_BOX))
        {
          // The last character of the text is a new paragraph character.
          // An extra line with no characters is added to increase the text's height
          // in order to place the cursor.

          if(numberOfLines == linesCapacity)
          {
            // Reserve more space for the next lines.
            linesBuffer = ResizeLinesBuffer(lines, newLines, linesCapacity, updateCurrentBuffer);
          }

          UpdateTextLayout(
            layoutParameters,
            layout.characterIndex + (layout.numberOfCharacters + layout.numberOfCharactersInSecondHalfLine),
            index + (layout.numberOfGlyphs + layout.numberOfGlyphsInSecondHalfLine), layoutSize, linesBuffer,
            numberOfLines);
        } // whether to add a last line.

        const BidirectionalLineInfoRun* const bidirectionalLineInfo =
          (layoutBidiParameters.isBidirectional && !bidirectionalLinesInfo.Empty())
            ? &bidirectionalLinesInfo[layoutBidiParameters.bidiLineIndex]
            : nullptr;

        if((nullptr != bidirectionalLineInfo) && !bidirectionalLineInfo->isIdentity &&
           (layout.characterIndex == bidirectionalLineInfo->characterRun.characterIndex))
        {
          SetGlyphPositions(layoutParameters, glyphPositionsBuffer, layoutBidiParameters, layout);
        }
        else
        {
          // Sets the positions of the glyphs.
          SetGlyphPositions(layoutParameters, glyphPositionsBuffer, layout);
        }

        // Updates the vertical pen's position.
        penY += -layout.descender + layout.lineSpacing;
        if(!layout.containsReplacement)
        {
          penY += GetLineSpacing(layout.ascender + -layout.descender, layout.relativeLineSize);
        }

        // Increase the glyph index.
        index = nextIndex;
      } // no ellipsis
    } // end for() traversing glyphs.

    // Shift lines to up if ellipsis and multilines and set ellipsis of first line to true
    if(anyLineIsEliped && numberOfLines > 1u)
    {
      if(ellipsisPosition == Text::EllipsisPosition::START)
      {
        Length lineIndex = 0;
        while(lineIndex < numberOfLines && layoutParameters.boundingBox.height < layoutSize.height)
        {
          LineRun& delLine = linesBuffer[lineIndex];
          delLine.ellipsis = true;

          layoutSize.height -= (delLine.ascender + -delLine.descender) + delLine.lineSpacing;
          for(Length lineIndex = 0; lineIndex < numberOfLines - 1; lineIndex++)
          {
            linesBuffer[lineIndex]          = linesBuffer[lineIndex + 1];
            linesBuffer[lineIndex].ellipsis = false;
          }
          numberOfLines--;
        }
        linesBuffer[0u].ellipsis = true;
      }
      else if(ellipsisPosition == Text::EllipsisPosition::MIDDLE)
      {
        Length middleLineIndex   = (numberOfLines) / 2u;
        Length ellipsisLineIndex = 0u;
        while(1u < numberOfLines && 0u < middleLineIndex && layoutParameters.boundingBox.height < layoutSize.height)
        {
          LineRun& delLine = linesBuffer[middleLineIndex];
          delLine.ellipsis = true;

          layoutSize.height -= (delLine.ascender + -delLine.descender) + delLine.lineSpacing;
          for(Length lineIndex = middleLineIndex; lineIndex < numberOfLines - 1; lineIndex++)
          {
            linesBuffer[lineIndex]          = linesBuffer[lineIndex + 1];
            linesBuffer[lineIndex].ellipsis = false;
          }
          numberOfLines--;
          ellipsisLineIndex = middleLineIndex - 1u;
          middleLineIndex   = (numberOfLines) / 2u;
        }

        linesBuffer[ellipsisLineIndex].ellipsis = true;
      }
    }

    if(updateCurrentBuffer)
    {
      // Insert up to newGlyphPositions.Begin() + layoutParameters.numberOfGlyphs (not newGlyphPositions.End())
      // to avoid duplicating the extra element added for Ellipsis calculation.
      glyphPositions.Insert(glyphPositions.Begin() + layoutParameters.startGlyphIndex, newGlyphPositions.Begin(),
                            newGlyphPositions.Begin() + layoutParameters.numberOfGlyphs);
      glyphPositions.Resize(totalNumberOfGlyphs);

      newLines.Resize(numberOfLines);

      // Current text's layout size adds only the newly laid-out lines.
      // Updates the layout size with the previously laid-out lines.
      UpdateLayoutSize(lines, layoutSize);

      if(0u != newLines.Count())
      {
        const LineRun& lastLine = *(newLines.End() - 1u);

        const Length characterOffset = lastLine.characterRun.characterIndex + lastLine.characterRun.numberOfCharacters;
        const Length glyphOffset     = lastLine.glyphRun.glyphIndex + lastLine.glyphRun.numberOfGlyphs;

        // Update the indices of the runs before the new laid-out lines are inserted.
        UpdateLineIndexOffsets(layoutParameters, lines, characterOffset, glyphOffset);

        // Insert the lines.
        lines.Insert(lines.Begin() + layoutParameters.startLineIndex, newLines.Begin(), newLines.End());
      }
    }
    else
    {
      lines.Resize(numberOfLines);
    }

    // Rounds upward to avoid a non integer size.
    layoutSize.height = std::ceil(layoutSize.height);

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--LayoutText\n\n");

    return true;
  }

  void Align(const Size& size, CharacterIndex startIndex, Length numberOfCharacters,
             Alignment horizontalAlignment, Vector<LineRun>& lines, float& alignmentOffset,
             Dali::LayoutDirection::Type layoutDirection, bool matchLayoutDirection)
  {
    const CharacterIndex lastCharacterPlusOne = startIndex + numberOfCharacters;

    alignmentOffset = MAX_FLOAT;
    // Traverse all lines and align the glyphs.
    for(Vector<LineRun>::Iterator it = lines.Begin(), endIt = lines.End(); it != endIt; ++it)
    {
      LineRun& line = *it;

      if(line.characterRun.characterIndex < startIndex)
      {
        // Do not align lines which have already been aligned.
        continue;
      }

      if(line.characterRun.characterIndex > lastCharacterPlusOne)
      {
        // Do not align lines beyond the last laid-out character.
        break;
      }

      if(line.characterRun.characterIndex == lastCharacterPlusOne && !isEmptyLineAtLast(lines, it))
      {
        // Do not align lines beyond the last laid-out character unless the line is last and empty.
        break;
      }

      // Calculate the line's alignment offset accordingly with the align option,
      // the box width, line length, and the paragraph's direction.
      CalculateHorizontalAlignment(size.width, horizontalAlignment, line, layoutDirection, matchLayoutDirection);

      // Updates the alignment offset.
      alignmentOffset = std::min(alignmentOffset, line.alignmentOffset);
    }
  }

  void CalculateHorizontalAlignment(float boxWidth, Alignment horizontalAlignment, LineRun& line,
                                    Dali::LayoutDirection::Type layoutDirection, bool matchLayoutDirection)
  {
    line.alignmentOffset = 0.f;
    const bool isLineRTL = RTL == line.direction;

    // Whether to swap the alignment.
    // Swap if the line is RTL and is not required to match the direction of the system's language or if it's required
    // to match the direction of the system's language and it's RTL.
    bool  isLayoutRTL = isLineRTL;
    float lineLength  = line.width;

    // match align for system language direction
    if(matchLayoutDirection)
    {
      // Swap the alignment type if the line is right to left.
      isLayoutRTL = layoutDirection == LayoutDirection::RIGHT_TO_LEFT;
    }
    // Calculate the horizontal line offset.
    switch(horizontalAlignment)
    {
      case Alignment::START:
      {
        if(isLayoutRTL)
        {
          if(isLineRTL)
          {
            lineLength += line.extraLength;
          }

          line.alignmentOffset = boxWidth - lineLength;
        }
        else
        {
          line.alignmentOffset = 0.f;

          if(isLineRTL)
          {
            // 'Remove' the white spaces at the end of the line (which are at the beginning in visual order)
            line.alignmentOffset -= line.extraLength;
          }
        }
        break;
      }
      case Alignment::CENTER:
      {
        line.alignmentOffset = 0.5f * (boxWidth - lineLength);

        if(isLineRTL)
        {
          line.alignmentOffset -= line.extraLength;
        }

        line.alignmentOffset = std::floor(line.alignmentOffset); // floor() avoids pixel alignment issues.
        break;
      }
      case Alignment::END:
      {
        if(isLayoutRTL)
        {
          line.alignmentOffset = 0.f;

          if(isLineRTL)
          {
            // 'Remove' the white spaces at the end of the line (which are at the beginning in visual order)
            line.alignmentOffset -= line.extraLength;
          }
        }
        else
        {
          if(isLineRTL)
          {
            lineLength += line.extraLength;
          }

          line.alignmentOffset = boxWidth - lineLength;
        }
        break;
      }
    }
  }

  float GetCursorInsetWidth()
  {
    return mIsCursorInsetEnabled ? mCursorWidth : 0.0f;
  }

  void Initialize(LineRun& line)
  {
    line.glyphRun.glyphIndex                              = 0u;
    line.glyphRun.numberOfGlyphs                          = 0u;
    line.characterRun.characterIndex                      = 0u;
    line.characterRun.numberOfCharacters                  = 0u;
    line.width                                            = 0.f;
    line.ascender                                         = 0.f;
    line.descender                                        = 0.f;
    line.extraLength                                      = 0.f;
    line.alignmentOffset                                  = 0.f;
    line.direction                                        = LTR;
    line.ellipsis                                         = false;
    line.lineSpacing                                      = mDefaultLineSpacing;
    line.isSplitToTwoHalves                               = false;
    line.glyphRunSecondHalf.glyphIndex                    = 0u;
    line.glyphRunSecondHalf.numberOfGlyphs                = 0u;
    line.characterRunForSecondHalfLine.characterIndex     = 0u;
    line.characterRunForSecondHalfLine.numberOfCharacters = 0u;
  }

  Type  mLayout;
  float mCursorWidth;
  float mDefaultLineSpacing;
  float mDefaultLineSize;

  IntrusivePtr<Metrics> mMetrics;
  float                 mRelativeLineSize;
  float                 mPixelSize;
  float                 mFontSizeScale;
  bool                  mIsCursorInsetEnabled : 1;
};

Engine::Engine()
: mImpl{nullptr}
{
  mImpl = new Engine::Impl();
}

Engine::~Engine()
{
  delete mImpl;
}

void Engine::SetMetrics(MetricsPtr& metrics)
{
  mImpl->mMetrics = metrics;
}

void Engine::SetLayout(Type layout)
{
  mImpl->mLayout = layout;
}

Engine::Type Engine::GetLayout() const
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "GetLayout[%d]\n", mImpl->mLayout);
  return mImpl->mLayout;
}

void Engine::SetCursorWidth(int width)
{
  mImpl->mCursorWidth = static_cast<float>(width);
}

int Engine::GetCursorWidth() const
{
  return static_cast<int>(mImpl->mCursorWidth);
}

void Engine::SetCursorInsetEnabled(bool enable)
{
  mImpl->mIsCursorInsetEnabled = enable;
}

bool Engine::LayoutText(Parameters& layoutParameters, Size& layoutSize, bool elideTextEnabled,
                        bool& isMarqueeEnabled, bool isMarqueeMaxTextureExceeded, bool isHiddenInputEnabled,
                        Text::EllipsisPosition::Type ellipsisPosition)
{
  return mImpl->LayoutText(layoutParameters, layoutSize, elideTextEnabled, isMarqueeEnabled,
                           isMarqueeMaxTextureExceeded, isHiddenInputEnabled, ellipsisPosition);
}

void Engine::Align(const Size& size, CharacterIndex startIndex, Length numberOfCharacters,
                   Alignment horizontalAlignment, Vector<LineRun>& lines, float& alignmentOffset,
                   Dali::LayoutDirection::Type layoutDirection, bool matchLayoutDirection)
{
  mImpl->Align(size, startIndex, numberOfCharacters, horizontalAlignment, lines, alignmentOffset, layoutDirection,
               matchLayoutDirection);
}

void Engine::SetDefaultLineSpacing(float lineSpacing)
{
  mImpl->mDefaultLineSpacing = lineSpacing;
}

float Engine::GetDefaultLineSpacing() const
{
  return mImpl->mDefaultLineSpacing;
}

void Engine::SetDefaultLineSize(float lineSize)
{
  mImpl->mDefaultLineSize = lineSize;
}

float Engine::GetDefaultLineSize() const
{
  return mImpl->mDefaultLineSize;
}

void Engine::SetRelativeLineSize(float relativeLineSize)
{
  mImpl->mRelativeLineSize = relativeLineSize;
}

float Engine::GetRelativeLineSize() const
{
  return mImpl->mRelativeLineSize;
}

void Engine::SetFontPixelSize(float pixelSize)
{
  mImpl->mPixelSize = pixelSize;
}

void Engine::SetFontSizeScale(float scale)
{
  mImpl->mFontSizeScale = scale;
}

float Engine::GetLineSpacing(float textSize, float relativeLineSize) const
{
  return mImpl->GetLineSpacing(textSize, relativeLineSize);
}

} // namespace Layout

} // namespace Text

} // namespace Ui

} // namespace Dali
