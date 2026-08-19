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
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>
#include <dali/public-api/math/math-utils.h>
#include <chrono>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/text/text-editable-control-interface.h>
#include <dali-ui-foundation/internal/text/bidirectional-support.h>
#include <dali-ui-foundation/internal/text/character-set-conversion.h>
#include <dali-ui-foundation/internal/text/color-segmentation.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl-model-updater.h>
#include <dali-ui-foundation/internal/text/hyphenator.h>
#include <dali-ui-foundation/internal/text/multi-language-support.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-glyph-helper.h>
#include <dali-ui-foundation/internal/text/segmentation.h>
#include <dali-ui-foundation/internal/text/shaper.h>

namespace Dali::Ui::Text
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_TEXT_PERFORMANCE_MARKER, false);

#if defined(TRACE_ENABLED)
uint32_t GetMilliSeconds()
{
  // Get the time of a monotonic clock since its epoch.
  auto epoch = std::chrono::steady_clock::now().time_since_epoch();

  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);

  return static_cast<uint32_t>(duration.count());
}
#endif

// The relative luminance of a color is defined as (L = 0.2126 * R + 0.7152 * G + 0.0722 * B)
// based on W3C Recommendations (https://www.w3.org/TR/WCAG20/)
constexpr float         BRIGHTNESS_THRESHOLD = 0.179f;
constexpr float         CONSTANT_R           = 0.2126f;
constexpr float         CONSTANT_G           = 0.7152f;
constexpr float         CONSTANT_B           = 0.0722f;
constexpr Dali::Vector4 BLACK(0.f, 0.f, 0.f, 1.f);
constexpr Dali::Vector4 WHITE(1.f, 1.f, 1.f, 1.f);
constexpr Dali::Vector4 LIGHT_BLUE(0.75f, 0.96f, 1.f, 1.f);
constexpr Dali::Vector4 BACKGROUND_SUB4(0.58f, 0.87f, 0.96f, 1.f);
constexpr Dali::Vector4 BACKGROUND_SUB5(0.83f, 0.94f, 0.98f, 1.f);
constexpr Dali::Vector4 BACKGROUND_SUB6(1.f, 0.5f, 0.5f, 1.f);
constexpr Dali::Vector4 BACKGROUND_SUB7(1.f, 0.8f, 0.8f, 1.f);
} // namespace

bool ControllerImplModelUpdater::Update(Controller::Impl& impl, OperationsMask operationsRequired)
{
  const TextProcessingSource source = MakeTextProcessingSource(*impl.mModel);
  return Update(impl, source, *impl.mModel, operationsRequired);
}

bool ControllerImplModelUpdater::Update(Controller::Impl&           impl,
                                        const TextProcessingSource& source,
                                        Model&                      targetModel,
                                        OperationsMask              operationsRequired)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::UpdateModel\n");

  const bool isProjectedTarget = source.HasReplacements();

  // Calculate the operations to be done.
  const OperationsMask operations = isProjectedTarget
                                      ? operationsRequired
                                      : static_cast<OperationsMask>(impl.mOperationsPending & operationsRequired);

  if(Controller::NO_OPERATION == operations)
  {
    // Nothing to do if no operations are pending and required.
    return false;
  }
  DALI_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_MODEL_UPDATE");

  if(isProjectedTarget)
  {
    ApplyTextProcessingSource(source, *targetModel.mLogicalModel);
  }

  TextUpdateInfo projectedUpdateInfo;
  if(isProjectedTarget)
  {
    projectedUpdateInfo.Clear();
    projectedUpdateInfo.mCharacterIndex              = 0u;
    projectedUpdateInfo.mParagraphCharacterIndex     = 0u;
    projectedUpdateInfo.mRequestedNumberOfCharacters = static_cast<Dali::Ui::Text::Length>(targetModel.mLogicalModel->mText.Count());
    projectedUpdateInfo.mNumberOfCharactersToAdd     = static_cast<Dali::Ui::Text::Length>(targetModel.mLogicalModel->mText.Count());
    projectedUpdateInfo.mPreviousNumberOfCharacters  = 0u;
    projectedUpdateInfo.mStartGlyphIndex             = 0u;
    projectedUpdateInfo.mStartLineIndex              = 0u;
    projectedUpdateInfo.mEstimatedNumberOfLines      = 1u;
    projectedUpdateInfo.mClearAll                    = false;
    projectedUpdateInfo.mFullRelayoutNeeded          = true;
  }
  TextUpdateInfo& updateInfo = isProjectedTarget ? projectedUpdateInfo : impl.mTextUpdateInfo;

  Vector<Character>& srcCharacters = targetModel.mLogicalModel->mText;
  Vector<Character>  displayCharacters;
  bool               useHiddenText = false;

  if(!isProjectedTarget && impl.mHiddenInput && impl.mEventData != nullptr)
  {
    if(impl.mEventData->mIsShowingPlaceholderText)
    {
      impl.mHiddenInput->InitPreviousTextCount();
    }
    else if(impl.mHiddenInput->GetMode() != HiddenText::Mode::NONE)
    {
      impl.mHiddenInput->Substitute(srcCharacters, displayCharacters, impl.mEventData->mPrimaryCursorPosition);
      useHiddenText = true;
    }
  }

  Vector<Character>& utf32Characters    = useHiddenText ? displayCharacters : srcCharacters;
  const Length       numberOfCharacters = static_cast<Dali::Ui::Text::Length>(utf32Characters.Count());

  // Index to the first character of the first paragraph to be updated.
  CharacterIndex startIndex = 0u;
  // Number of characters of the paragraphs to be removed.
  Length paragraphCharacters = 0u;

  if(!isProjectedTarget)
  {
    impl.CalculateTextUpdateIndices(paragraphCharacters);
  }

  // Check whether the indices for updating the text is valid
  if(updateInfo.mParagraphCharacterIndex > numberOfCharacters ||
     updateInfo.mRequestedNumberOfCharacters > numberOfCharacters)
  {
    if(numberOfCharacters == 0u)
    {
      updateInfo.Clear();
      updateInfo.mClearAll = true;
    }
    else // numberOfCharacters > 0u
    {
      std::string currentText;
      Utf32ToUtf8(targetModel.mLogicalModel->mText.Begin(), numberOfCharacters, currentText);

      DALI_LOG_ERROR("Controller::Impl::UpdateModel: mTextUpdateInfo has invalid indices\n");
      DALI_LOG_ERROR("Number of characters: %d, current text is: %s paragraphCharacters: %d\n", numberOfCharacters,
                     currentText.c_str(), paragraphCharacters);

      // Dump mTextUpdateInfo
      DALI_LOG_ERROR("Dump mTextUpdateInfo:\n");
      DALI_LOG_ERROR("     mTextUpdateInfo.mCharacterIndex = %u\n", updateInfo.mCharacterIndex);
      DALI_LOG_ERROR("     mTextUpdateInfo.mNumberOfCharactersToRemove = %u\n",
                     updateInfo.mNumberOfCharactersToRemove);
      DALI_LOG_ERROR("     mTextUpdateInfo.mNumberOfCharactersToAdd = %u\n",
                     updateInfo.mNumberOfCharactersToAdd);
      DALI_LOG_ERROR("     mTextUpdateInfo.mPreviousNumberOfCharacters = %u\n",
                     updateInfo.mPreviousNumberOfCharacters);
      DALI_LOG_ERROR("     mTextUpdateInfo.mParagraphCharacterIndex = %u\n",
                     updateInfo.mParagraphCharacterIndex);
      DALI_LOG_ERROR("     mTextUpdateInfo.mRequestedNumberOfCharacters = %u\n",
                     updateInfo.mRequestedNumberOfCharacters);
      DALI_LOG_ERROR("     mTextUpdateInfo.mStartGlyphIndex = %u\n", updateInfo.mStartGlyphIndex);
      DALI_LOG_ERROR("     mTextUpdateInfo.mStartLineIndex = %u\n", updateInfo.mStartLineIndex);
      DALI_LOG_ERROR("     mTextUpdateInfo.mEstimatedNumberOfLines = %u\n",
                     updateInfo.mEstimatedNumberOfLines);
      DALI_LOG_ERROR("     mTextUpdateInfo.mClearAll = %d\n", updateInfo.mClearAll);
      DALI_LOG_ERROR("     mTextUpdateInfo.mFullRelayoutNeeded = %d\n", updateInfo.mFullRelayoutNeeded);
      DALI_LOG_ERROR("     mTextUpdateInfo.mIsLastCharacterNewParagraph = %d\n",
                     updateInfo.mIsLastCharacterNewParagraph);

      return false;
    }
  }

  startIndex = updateInfo.mParagraphCharacterIndex;

  if(!isProjectedTarget && (updateInfo.mClearAll || (0u != paragraphCharacters)))
  {
    impl.ClearModelData(startIndex, startIndex + ((paragraphCharacters > 0u) ? paragraphCharacters - 1u : 0u),
                        operations);
  }

  updateInfo.mClearAll = false;

  // Whether the model is updated.
  bool updated = false;

  Vector<LineBreakInfo>& lineBreakInfo               = targetModel.mLogicalModel->mLineBreakInfo;
  const Length           requestedNumberOfCharacters = updateInfo.mRequestedNumberOfCharacters;

  if(Controller::NO_OPERATION != (Controller::GET_LINE_BREAKS & operations))
  {
    // Retrieves the line break info. The line break info is used to split the text in 'paragraphs' to
    // calculate the bidirectional info for each 'paragraph'.
    // It's also used to layout the text (where it should be a new line) or to shape the text (text in different lines
    // is not shaped together).
    lineBreakInfo.Resize(numberOfCharacters, TextAbstraction::LINE_NO_BREAK);

    TextAbstraction::Segmentation segmentation = TextAbstraction::Segmentation::Get();
    SetLineBreakInfo(segmentation, utf32Characters, startIndex, requestedNumberOfCharacters, lineBreakInfo);

    MultilanguageSupport multilanguageSupport = MultilanguageSupport::Get();

    // Check if an ICU-based line break update is required.
    if(multilanguageSupport.IsICULineBreakNeeded())
    {
      std::string currentText;
      Utf32ToUtf8(targetModel.mLogicalModel->mText.Begin(), numberOfCharacters, currentText);
      multilanguageSupport.UpdateICULineBreak(currentText, numberOfCharacters, lineBreakInfo.Begin());
    }

    if(targetModel.mLineWrapMode == LineWrapMode::HYPHENATION ||
       targetModel.mLineWrapMode == LineWrapMode::MIXED)
    {
      CharacterIndex end                 = startIndex + requestedNumberOfCharacters;
      LineBreakInfo* lineBreakInfoBuffer = lineBreakInfo.Begin();

      TextAbstraction::Hyphenation hyphenation = TextAbstraction::Hyphenation::Get();

      for(CharacterIndex index = startIndex; index < end; index++)
      {
        CharacterIndex wordEnd = index;
        while((*(lineBreakInfoBuffer + wordEnd) != TextAbstraction::LINE_ALLOW_BREAK) &&
              (*(lineBreakInfoBuffer + wordEnd) != TextAbstraction::LINE_MUST_BREAK))
        {
          wordEnd++;
        }

        if((wordEnd + 1) == end) // add last char
        {
          wordEnd++;
        }

        Vector<bool> hyphens = GetWordHyphens(hyphenation, utf32Characters.Begin() + index, wordEnd - index, nullptr);

        for(CharacterIndex i = 0; i < (wordEnd - index) && i < hyphens.Size(); i++)
        {
          if(hyphens[i])
          {
            *(lineBreakInfoBuffer + index + i) = TextAbstraction::LINE_HYPHENATION_BREAK;
          }
        }

        index = wordEnd;
      }
    }

    // Create the paragraph info.
    targetModel.mLogicalModel->CreateParagraphInfo(startIndex, requestedNumberOfCharacters);
    updated = true;
  }

  const bool getScripts    = Controller::NO_OPERATION != (Controller::GET_SCRIPTS & operations);
  const bool validateFonts = Controller::NO_OPERATION != (Controller::VALIDATE_FONTS & operations);

  Vector<ScriptRun>& scripts    = targetModel.mLogicalModel->mScriptRuns;
  Vector<FontRun>&   validFonts = targetModel.mLogicalModel->mFontRuns;

  if(getScripts || validateFonts)
  {
    // Validates the fonts assigned by the application or assigns default ones.
    // It makes sure all the characters are going to be rendered by the correct font.
    MultilanguageSupport multilanguageSupport = MultilanguageSupport::Get();

    if(getScripts)
    {
      // Retrieves the scripts used in the text.
      multilanguageSupport.SetScripts(utf32Characters, startIndex, requestedNumberOfCharacters, scripts);
    }

    if(validateFonts)
    {
      // Validate the fonts set through the mark-up string.
      float effectiveTextScale = impl.GetEffectiveTextScale();
      // Get the default font's description.
      TextAbstraction::FontDescription defaultFontDescription;
      TextAbstraction::PointSize26Dot6 defaultPointSize =
        static_cast<TextAbstraction::PointSize26Dot6>(TextAbstraction::FontClient::DEFAULT_POINT_SIZE * effectiveTextScale);

      // Get the number of points per one unit of point-size
      uint32_t numberOfPointsPerOneUnitOfPointSize = impl.GetFontClient().GetNumberOfPointsPerOneUnitOfPointSize();

      if(impl.IsShowingPlaceholderText() && impl.mEventData && (nullptr != impl.mEventData->mPlaceholderFont))
      {
        // If the placeholder font is set specifically, only placeholder font is changed.
        defaultFontDescription = impl.mEventData->mPlaceholderFont->GetFontDescription();
        if(impl.mEventData->mPlaceholderFont->sizeDefined)
        {
          defaultPointSize = static_cast<TextAbstraction::PointSize26Dot6>(impl.mEventData->mPlaceholderFont->mDefaultPointSize * effectiveTextScale * numberOfPointsPerOneUnitOfPointSize);
        }
      }
      else if(nullptr != impl.mFontDefaults)
      {
        // Set the normal font and the placeholder font.
        defaultFontDescription = impl.mFontDefaults->GetFontDescription();

        if(impl.mTextFitEnabled || impl.mTextFitCandidatesEnabled)
        {
          defaultPointSize = static_cast<TextAbstraction::PointSize26Dot6>(impl.mFontDefaults->mFitPointSize * numberOfPointsPerOneUnitOfPointSize);
        }
        else
        {
          defaultPointSize =
            static_cast<TextAbstraction::PointSize26Dot6>(impl.mFontDefaults->mDefaultPointSize * effectiveTextScale * numberOfPointsPerOneUnitOfPointSize);
        }
      }

      Property::Map* variationsMapPtr = nullptr;
      if(!targetModel.mLogicalModel->mVariationsMap.Empty())
      {
        variationsMapPtr = &targetModel.mLogicalModel->mVariationsMap;
      }

      // Validates the fonts. If there is a character with no assigned font it sets a default one.
      // After this call, fonts are validated.
      if(source.HasReplacements())
      {
        ValidateFontsForProcessingSource(multilanguageSupport,
                                         impl.GetFontClient(),
                                         source,
                                         scripts,
                                         defaultFontDescription,
                                         defaultPointSize,
                                         effectiveTextScale,
                                         startIndex,
                                         requestedNumberOfCharacters,
                                         validFonts,
                                         variationsMapPtr);
      }
      else
      {
        // Preserve the original hidden-input source and call shape exactly on the ordinary fast path.
        multilanguageSupport.ValidateFonts(impl.GetFontClient(),
                                           utf32Characters,
                                           scripts,
                                           targetModel.mLogicalModel->mFontDescriptionRuns,
                                           defaultFontDescription,
                                           defaultPointSize,
                                           effectiveTextScale,
                                           startIndex,
                                           requestedNumberOfCharacters,
                                           validFonts,
                                           variationsMapPtr);
      }
    }
    updated = true;
  }

  Vector<Character> mirroredUtf32Characters;
  bool              textMirrored       = false;
  const Length      numberOfParagraphs = static_cast<Dali::Ui::Text::Length>(targetModel.mLogicalModel->mParagraphInfo.Count());
  if(Controller::NO_OPERATION != (Controller::BIDI_INFO & operations))
  {
    Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo = targetModel.mLogicalModel->mBidirectionalParagraphInfo;
    bidirectionalInfo.Reserve(numberOfParagraphs);

    TextAbstraction::BidirectionalSupport bidirectionalSupport = TextAbstraction::BidirectionalSupport::Get();

    // Calculates the bidirectional info for the whole paragraph if it contains right to left scripts.
    SetBidirectionalInfo(
      bidirectionalSupport, utf32Characters, scripts, lineBreakInfo, startIndex, requestedNumberOfCharacters,
      bidirectionalInfo, targetModel.mLogicalModel->mBidirectionalLineInfo,
      (targetModel.mLayoutDirectionMode != LayoutDirectionMode::CONTENTS), impl.mLayoutDirection);

    if(0u != bidirectionalInfo.Count())
    {
      // Only set the character directions if there is right to left characters.
      Vector<CharacterDirection>& directions = targetModel.mLogicalModel->mCharacterDirections;
      GetCharactersDirection(bidirectionalSupport, bidirectionalInfo, numberOfCharacters, startIndex,
                             requestedNumberOfCharacters, directions);

      // This paragraph has right to left text. Some characters may need to be mirrored.
      // TODO: consider if the mirrored string can be stored as well.

      textMirrored = GetMirroredText(bidirectionalSupport, utf32Characters, directions, bidirectionalInfo, startIndex,
                                     requestedNumberOfCharacters, mirroredUtf32Characters);
    }
    else
    {
      // There is no right to left characters. Clear the directions vector.
      targetModel.mLogicalModel->mCharacterDirections.Clear();
    }
    updated = true;
  }

  Vector<GlyphInfo>& glyphs = targetModel.mVisualModel->mGlyphs;
  Vector<GlyphIndex> newParagraphGlyphs;
  newParagraphGlyphs.Reserve(numberOfParagraphs);

  const Length currentNumberOfGlyphs = static_cast<Dali::Ui::Text::Length>(glyphs.Count());

#if defined(TRACE_ENABLED)
  uint32_t logThreshold = TextAbstraction::FontClient::GetPerformanceLogThresholdTime();
  bool     logEnabled   = TextAbstraction::FontClient::IsPerformanceLogEnabled();

  uint32_t timeStamps[6];
  uint32_t timeStampIndex = 0;

  if(logEnabled)
  {
    timeStamps[timeStampIndex++] = GetMilliSeconds();
  }
#endif

  if(Controller::NO_OPERATION != (Controller::SHAPE_TEXT & operations))
  {
    const Vector<Character>& textToShape = textMirrored ? mirroredUtf32Characters : utf32Characters;

    TextAbstraction::Shaping shaping = TextAbstraction::Shaping::Get();

    // Shapes the text.
    ShapeTextForProcessingSource(shaping,
                                 impl.GetFontClient(),
                                 source,
                                 textToShape,
                                 lineBreakInfo,
                                 scripts,
                                 validFonts,
                                 startIndex,
                                 updateInfo.mStartGlyphIndex,
                                 requestedNumberOfCharacters,
                                 *targetModel.mVisualModel,
                                 newParagraphGlyphs);

    // Create the 'number of glyphs' per character and the glyph to character conversion tables.
    targetModel.mVisualModel->CreateGlyphsPerCharacterTable(startIndex, updateInfo.mStartGlyphIndex,
                                                            requestedNumberOfCharacters);
    targetModel.mVisualModel->CreateCharacterToGlyphTable(startIndex, updateInfo.mStartGlyphIndex,
                                                          requestedNumberOfCharacters);

    updated = true;
  }

#if defined(TRACE_ENABLED)
  if(logEnabled)
  {
    timeStamps[timeStampIndex++] = GetMilliSeconds();
  }
#endif

  const Length numberOfGlyphs = static_cast<Length>(glyphs.Count()) - currentNumberOfGlyphs;

  if(Controller::NO_OPERATION != (Controller::GET_GLYPH_METRICS & operations))
  {
    GetGlyphMetricsForProcessingSource(*impl.mMetrics,
                                       source,
                                       glyphs,
                                       updateInfo.mStartGlyphIndex,
                                       numberOfGlyphs,
                                       newParagraphGlyphs);
    updated = true;
  }

#if defined(TRACE_ENABLED)
  if(logEnabled)
  {
    timeStamps[timeStampIndex++] = GetMilliSeconds();
  }
#endif

  if(!isProjectedTarget && (nullptr != impl.mEventData) && impl.mEventData->mPreEditFlag &&
     (0u != impl.mModel->mVisualModel->mCharactersToGlyph.Count()))
  {
    Dali::Integration::InputMethodContext::PreEditAttributeDataContainer attrs;
    Dali::Integration::InputMethodContext::GetPreeditStyle(impl.mEventData->mInputMethodContext, attrs);
    Dali::Integration::InputMethodContext::PreeditStyle type = Dali::Integration::InputMethodContext::PreeditStyle::NONE;

    // Check the type of preedit and run it.
    for(Dali::Integration::InputMethodContext::PreEditAttributeDataContainer::Iterator it = attrs.Begin(), endIt = attrs.End();
        it != endIt; it++)
    {
      Dali::Integration::InputMethodContext::PreeditAttributeData attrData = *it;
      DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::UpdateModel PreeditStyle type : %d  start %d end %d \n",
                    attrData.preeditType, attrData.startIndex, attrData.endIndex);
      type = attrData.preeditType;

      // Check the number of commit characters for the start position.
      unsigned int numberOfCommit  = impl.mEventData->mPrimaryCursorPosition - impl.mEventData->mPreEditLength;
      Length       numberOfIndices = attrData.endIndex - attrData.startIndex;

      switch(type)
      {
        case Dali::Integration::InputMethodContext::PreeditStyle::UNDERLINE:
        {
          // Add the underline for the pre-edit text.
          UnderlinedGlyphRun underlineRun;
          underlineRun.glyphRun.glyphIndex     = attrData.startIndex + numberOfCommit;
          underlineRun.glyphRun.numberOfGlyphs = numberOfIndices;
          impl.mModel->mVisualModel->mUnderlineRuns.PushBack(underlineRun);

          if(0u != impl.mModel->mLogicalModel->mUnderlinedCharacterRuns.Count())
          {
            impl.CopyUnderlinedFromLogicalToVisualModels(false);
          }
          break;
        }
        case Dali::Integration::InputMethodContext::PreeditStyle::REVERSE:
        {
          Vector4  textColor = impl.mModel->mVisualModel->GetTextColor();
          ColorRun backgroundColorRun;
          backgroundColorRun.characterRun.characterIndex     = attrData.startIndex + numberOfCommit;
          backgroundColorRun.characterRun.numberOfCharacters = numberOfIndices;
          backgroundColorRun.color                           = textColor;
          impl.mModel->mLogicalModel->mBackgroundColorRuns.PushBack(backgroundColorRun);

          Vector4 backgroundColor = impl.mModel->mVisualModel->GetBackgroundColor();
          if(Dali::EqualsZero(backgroundColor.a)) // There is no text background color.
          {
            // Try use the control's background color.
            if(nullptr != impl.mEditableControlInterface)
            {
              impl.mEditableControlInterface->GetControlBackgroundColor(backgroundColor);
              if(Dali::EqualsZero(backgroundColor.a)) // There is no control background color.
              {
                // Determines black or white color according to text color.
                // Based on W3C Recommendations (https://www.w3.org/TR/WCAG20/)
                float L         = CONSTANT_R * textColor.r + CONSTANT_G * textColor.g + CONSTANT_B * textColor.b;
                backgroundColor = L > BRIGHTNESS_THRESHOLD ? BLACK : WHITE;
              }
            }
          }

          Vector<ColorRun> colorRuns;
          colorRuns.Resize(1u);
          ColorRun& colorRun                       = *(colorRuns.Begin());
          colorRun.color                           = backgroundColor;
          colorRun.characterRun.characterIndex     = attrData.startIndex + numberOfCommit;
          colorRun.characterRun.numberOfCharacters = numberOfIndices;
          impl.mModel->mLogicalModel->mColorRuns.PushBack(colorRun);

          if(0u != impl.mModel->mLogicalModel->mUnderlinedCharacterRuns.Count())
          {
            impl.CopyUnderlinedFromLogicalToVisualModels(false);
          }
          break;
        }
        case Dali::Integration::InputMethodContext::PreeditStyle::HIGHLIGHT:
        {
          ColorRun backgroundColorRun;
          backgroundColorRun.characterRun.characterIndex     = attrData.startIndex + numberOfCommit;
          backgroundColorRun.characterRun.numberOfCharacters = numberOfIndices;
          backgroundColorRun.color                           = LIGHT_BLUE;
          impl.mModel->mLogicalModel->mBackgroundColorRuns.PushBack(backgroundColorRun);

          if(0u != impl.mModel->mLogicalModel->mUnderlinedCharacterRuns.Count())
          {
            impl.CopyUnderlinedFromLogicalToVisualModels(false);
          }
          break;
        }
        case Dali::Integration::InputMethodContext::PreeditStyle::CUSTOM_PLATFORM_STYLE_1:
        {
          // CUSTOM_PLATFORM_STYLE_1 should be drawn with background and underline together.
          ColorRun backgroundColorRun;
          backgroundColorRun.characterRun.characterIndex     = attrData.startIndex + numberOfCommit;
          backgroundColorRun.characterRun.numberOfCharacters = numberOfIndices;
          backgroundColorRun.color                           = BACKGROUND_SUB4;
          impl.mModel->mLogicalModel->mBackgroundColorRuns.PushBack(backgroundColorRun);

          UnderlinedGlyphRun underlineRun;
          underlineRun.glyphRun.glyphIndex     = attrData.startIndex + numberOfCommit;
          underlineRun.glyphRun.numberOfGlyphs = numberOfIndices;
          impl.mModel->mVisualModel->mUnderlineRuns.PushBack(underlineRun);

          if(0u != impl.mModel->mLogicalModel->mUnderlinedCharacterRuns.Count())
          {
            impl.CopyUnderlinedFromLogicalToVisualModels(false);
          }
          break;
        }
        case Dali::Integration::InputMethodContext::PreeditStyle::CUSTOM_PLATFORM_STYLE_2:
        {
          // CUSTOM_PLATFORM_STYLE_2 should be drawn with background and underline together.
          ColorRun backgroundColorRun;
          backgroundColorRun.characterRun.characterIndex     = attrData.startIndex + numberOfCommit;
          backgroundColorRun.characterRun.numberOfCharacters = numberOfIndices;
          backgroundColorRun.color                           = BACKGROUND_SUB5;
          impl.mModel->mLogicalModel->mBackgroundColorRuns.PushBack(backgroundColorRun);

          UnderlinedGlyphRun underlineRun;
          underlineRun.glyphRun.glyphIndex     = attrData.startIndex + numberOfCommit;
          underlineRun.glyphRun.numberOfGlyphs = numberOfIndices;
          impl.mModel->mVisualModel->mUnderlineRuns.PushBack(underlineRun);

          if(0u != impl.mModel->mLogicalModel->mUnderlinedCharacterRuns.Count())
          {
            impl.CopyUnderlinedFromLogicalToVisualModels(false);
          }
          break;
        }
        case Dali::Integration::InputMethodContext::PreeditStyle::CUSTOM_PLATFORM_STYLE_3:
        {
          // CUSTOM_PLATFORM_STYLE_3 should be drawn with background and underline together.
          ColorRun backgroundColorRun;
          backgroundColorRun.characterRun.characterIndex     = attrData.startIndex + numberOfCommit;
          backgroundColorRun.characterRun.numberOfCharacters = numberOfIndices;
          backgroundColorRun.color                           = BACKGROUND_SUB6;
          impl.mModel->mLogicalModel->mBackgroundColorRuns.PushBack(backgroundColorRun);

          UnderlinedGlyphRun underlineRun;
          underlineRun.glyphRun.glyphIndex     = attrData.startIndex + numberOfCommit;
          underlineRun.glyphRun.numberOfGlyphs = numberOfIndices;
          impl.mModel->mVisualModel->mUnderlineRuns.PushBack(underlineRun);

          if(0u != impl.mModel->mLogicalModel->mUnderlinedCharacterRuns.Count())
          {
            impl.CopyUnderlinedFromLogicalToVisualModels(false);
          }
          break;
        }
        case Dali::Integration::InputMethodContext::PreeditStyle::CUSTOM_PLATFORM_STYLE_4:
        {
          // CUSTOM_PLATFORM_STYLE_4 should be drawn with background and underline together.
          ColorRun backgroundColorRun;
          backgroundColorRun.characterRun.characterIndex     = attrData.startIndex + numberOfCommit;
          backgroundColorRun.characterRun.numberOfCharacters = numberOfIndices;
          backgroundColorRun.color                           = BACKGROUND_SUB7;
          impl.mModel->mLogicalModel->mBackgroundColorRuns.PushBack(backgroundColorRun);

          UnderlinedGlyphRun underlineRun;
          underlineRun.glyphRun.glyphIndex     = attrData.startIndex + numberOfCommit;
          underlineRun.glyphRun.numberOfGlyphs = numberOfIndices;
          impl.mModel->mVisualModel->mUnderlineRuns.PushBack(underlineRun);

          if(0u != impl.mModel->mLogicalModel->mUnderlinedCharacterRuns.Count())
          {
            impl.CopyUnderlinedFromLogicalToVisualModels(false);
          }
          break;
        }
        case Dali::Integration::InputMethodContext::PreeditStyle::NONE:
        default:
        {
          break;
        }
      }
    }
    attrs.Clear();
    updated = true;
  }

#if defined(TRACE_ENABLED)
  if(logEnabled)
  {
    timeStamps[timeStampIndex++] = GetMilliSeconds();
  }
#endif

  if(Controller::NO_OPERATION != (Controller::COLOR & operations))
  {
    // Set the color runs in glyphs.
    SetColorSegmentationInfo(targetModel.mLogicalModel->mColorRuns, targetModel.mVisualModel->mCharactersToGlyph,
                             targetModel.mVisualModel->mGlyphsPerCharacter, startIndex,
                             updateInfo.mStartGlyphIndex, requestedNumberOfCharacters,
                             targetModel.mVisualModel->mColors, targetModel.mVisualModel->mColorIndices);

    // Set the background color runs in glyphs.
    SetColorSegmentationInfo(
      targetModel.mLogicalModel->mBackgroundColorRuns, targetModel.mVisualModel->mCharactersToGlyph,
      targetModel.mVisualModel->mGlyphsPerCharacter, startIndex, updateInfo.mStartGlyphIndex,
      requestedNumberOfCharacters, targetModel.mVisualModel->mBackgroundColors,
      targetModel.mVisualModel->mBackgroundColorIndices);

    updated = true;
  }

#if defined(TRACE_ENABLED)
  if(logEnabled)
  {
    timeStamps[timeStampIndex++] = GetMilliSeconds();
  }
#endif

  if((Controller::NO_OPERATION != (Controller::SHAPE_TEXT & operations)) &&
     !((!isProjectedTarget && nullptr != impl.mEventData) && impl.mEventData->mPreEditFlag &&
       (0u != impl.mModel->mVisualModel->mCharactersToGlyph.Count())))
  {
    // Markup-era inline run containers are also used by StyledText range spans.
    // Existing visual runs are included so a later plain text/style update clears stale glyph runs.
    const bool shouldSyncInlineStyleRuns =
      (0u != targetModel.mLogicalModel->mUnderlinedCharacterRuns.Count()) ||
      (0u != targetModel.mLogicalModel->mStrikethroughCharacterRuns.Count()) ||
      (0u != targetModel.mLogicalModel->mCharacterSpacingCharacterRuns.Count()) ||
      (0u != targetModel.mVisualModel->mUnderlineRuns.Count()) ||
      (0u != targetModel.mVisualModel->mStrikethroughRuns.Count()) ||
      (0u != targetModel.mVisualModel->mCharacterSpacingRuns.Count());

    if(shouldSyncInlineStyleRuns)
    {
      if(isProjectedTarget)
      {
        CopyProcessingCharacterStylesToVisual(*targetModel.mLogicalModel, *targetModel.mVisualModel);
      }
      else
      {
        impl.CopyUnderlinedFromLogicalToVisualModels(true);
        impl.CopyStrikethroughFromLogicalToVisualModels();
        impl.CopyCharacterSpacingFromLogicalToVisualModels();
      }
    }

    updated = true;
  }

#if defined(TRACE_ENABLED)
  if(logEnabled)
  {
    timeStamps[timeStampIndex++] = GetMilliSeconds();
    uint32_t timeShape           = timeStamps[1] - timeStamps[0];
    uint32_t timeGlyph           = timeStamps[2] - timeStamps[1];
    uint32_t timePreedit         = timeStamps[3] - timeStamps[2];
    uint32_t timeColor           = timeStamps[4] - timeStamps[3];
    uint32_t timeCopy            = timeStamps[5] - timeStamps[4];

    if(timeStamps[5] - timeStamps[0] > logThreshold)
    {
      std::string currentText;
      Utf32ToUtf8(targetModel.mLogicalModel->mText.Begin(), numberOfCharacters, currentText);
      DALI_LOG_DEBUG_INFO("DALI_TEXT_MODEL_UPDATE shape:%u ms, glyph:%u ms, preedit:%u ms, color:%u ms, copy:%u ms\n",
                          timeShape, timeGlyph, timePreedit, timeColor, timeCopy);
      DALI_LOG_DEBUG_INFO("DALI_TEXT_MODEL_UPDATE chars:%d, text:%s\n", numberOfCharacters, currentText.c_str());
    }
  }
#endif

  // The estimated number of lines. Used to avoid reallocations when layouting.
  updateInfo.mEstimatedNumberOfLines =
    static_cast<Length>(std::max(targetModel.mVisualModel->mLines.Count(), targetModel.mLogicalModel->mParagraphInfo.Count()));

  // Set the previous number of characters for the next time the text is updated.
  updateInfo.mPreviousNumberOfCharacters = numberOfCharacters;

  return updated;
}

} // namespace Dali::Ui::Text
