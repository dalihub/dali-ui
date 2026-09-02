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
#include <dali/public-api/math/math-utils.h>
#include <memory.h>
#include <atomic>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/text/text-editable-control-interface.h>
#include <dali-ui-foundation/internal/text/character-set-conversion.h>
#include <dali-ui-foundation/internal/text/characters-helper-functions.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-placeholder-handler.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-text-updater.h>
#include <dali-ui-foundation/internal/text/emoji-helper.h>
#include <dali-ui-foundation/internal/text/styled-text/styled-text-applier.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

float GetDpi()
{
  static uint32_t horizontalDpi = 0u;
  static uint32_t verticalDpi   = 0u;

  if(DALI_UNLIKELY(horizontalDpi == 0u))
  {
    Dali::TextAbstraction::FontClient fontClient = Dali::TextAbstraction::FontClient::Get();
    fontClient.GetDpi(horizontalDpi, verticalDpi);
  }
  return static_cast<float>(horizontalDpi);
}

uint64_t NextReplacementSourceRevision()
{
  static std::atomic<uint64_t> revision{0u};
  return revision.fetch_add(1u, std::memory_order_relaxed) + 1u;
}

} // namespace

namespace Dali
{
namespace Ui
{
namespace Text
{
void Controller::TextUpdater::SetText(Controller& controller, const std::string& text)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Controller::SetText\n");

  Controller::Impl& impl = *controller.mImpl;

  if(impl.mEventData)
  {
    impl.mEventData->mEditableStyledText.reset();
  }

  // Reset keyboard as text changed
  impl.ResetInputMethodContext();

  // Remove the previously set text and style.
  ResetText(controller);

  // Remove the style.
  impl.ClearStyleData();

  CharacterIndex lastCursorIndex = 0u;

  EventData*& eventData = impl.mEventData;

  if(nullptr != eventData)
  {
    // If popup shown then hide it by switching to Editing state
    if((EventData::SELECTING == eventData->mState) || (EventData::EDITING_WITH_POPUP == eventData->mState) ||
       (EventData::EDITING_WITH_GRAB_HANDLE == eventData->mState) ||
       (EventData::EDITING_WITH_PASTE_POPUP == eventData->mState))
    {
      if((impl.mSelectableControlInterface != nullptr) && (EventData::SELECTING == eventData->mState))
      {
        impl.mSelectableControlInterface->SelectionChanged(
          eventData->mLeftSelectionPosition, eventData->mRightSelectionPosition, eventData->mPrimaryCursorPosition,
          eventData->mPrimaryCursorPosition);
      }

      impl.ChangeState(EventData::EDITING);
    }
  }

  if(!text.empty())
  {
    ModelPtr&        model        = impl.mModel;
    LogicalModelPtr& logicalModel = model->mLogicalModel;
    model->mVisualModel->SetTextColor(impl.mTextColor);

    const Length   textSize = static_cast<Dali::Ui::Text::Length>(text.size());
    const uint8_t* utf8     = reinterpret_cast<const uint8_t*>(text.c_str());

    //  Convert text into UTF-32
    Vector<Character>& utf32Characters = logicalModel->mText;
    utf32Characters.Resize(textSize);

    // Transform a text array encoded in utf8 into an array encoded in utf32.
    // It returns the actual number of characters.
    Length characterCount = Utf8ToUtf32(utf8, textSize, utf32Characters.Begin());
    utf32Characters.Resize(characterCount);

    DALI_ASSERT_DEBUG(textSize >= characterCount && "Invalid UTF32 conversion length");
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Controller::SetText %p UTF8 size %d, UTF32 size %d\n", &controller,
                  textSize, logicalModel->mText.Count());

    // The characters to be added.
    impl.mTextUpdateInfo.mNumberOfCharactersToAdd = static_cast<Dali::Ui::Text::Length>(logicalModel->mText.Count());

    // To reset the cursor position
    lastCursorIndex = characterCount;

    // Update the rest of the model during size negotiation
    impl.QueueModifyEvent(ModifyEvent::TEXT_REPLACED);

    // The natural size needs to be re-calculated.
    impl.mRecalculateNaturalSize    = true;
    impl.mRecalculateLayoutSize     = true;
    impl.mRecalculateHeightForWidth = true;
    impl.mModel->mVisualModel->SetHeightForWidth(Size::ZERO);

    // The text direction needs to be updated.
    impl.mUpdateTextDirection = true;

    // Apply modifications to the model
    impl.mOperationsPending = ALL_OPERATIONS;
  }
  else
  {
    if(nullptr != eventData)
    {
      PlaceholderHandler::ShowPlaceholderText(impl);
      if(!impl.IsShowingPlaceholderText())
      {
        impl.mModel->mVisualModel->SetLayoutSize(Size::ZERO);
      }
    }
    else
    {
      // Clear the layout size cache when the label's text is set to empty.
      impl.mModel->mVisualModel->SetLayoutSize(Size::ZERO);
    }
  }

  unsigned int oldCursorPos = (nullptr != eventData ? eventData->mPrimaryCursorPosition : 0);

  // Resets the cursor position.
  controller.ResetCursorPosition(lastCursorIndex);

  // Scrolls the text to make the cursor visible.
  impl.ResetScrollPosition();

  impl.RequestRelayout();
  impl.RequestAsyncRender();

  if(nullptr != eventData)
  {
    // Cancel previously queued events
    eventData->mEventQueue.clear();
  }

  // Do this last since it provides callbacks into application code.
  if(NULL != impl.mEditableControlInterface)
  {
    impl.mEditableControlInterface->CursorPositionChanged(oldCursorPos, lastCursorIndex);
    impl.mEditableControlInterface->TextChanged(true);
  }
}

void Controller::TextUpdater::SetStyledText(Controller& controller, const StyledText& styledText)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Controller::SetStyledText\n");

  Controller::Impl& impl = *controller.mImpl;

  // Reset keyboard as text changed
  impl.ResetInputMethodContext();

  // StyledText is a new content source, so clear previous text/style state first.
  ResetText(controller);
  impl.ClearStyleData();

  if(impl.mEventData)
  {
    if(styledText)
    {
      impl.mEventData->mEditableStyledText = std::make_unique<EditableStyledTextData>();
      impl.mEventData->mEditableStyledText->Set(styledText);
    }
    else
    {
      impl.mEventData->mEditableStyledText.reset();
    }
  }

  CharacterIndex lastCursorIndex = 0u;

  EventData*& eventData = impl.mEventData;

  ModelPtr&        model        = impl.mModel;
  LogicalModelPtr& logicalModel = model->mLogicalModel;

  const std::string styledPlainText = styledText ? std::string(styledText.GetText().CStr()) : std::string();

  ReplacementSourceSnapshot replacementSource =
    Dali::Ui::Internal::Text::StyledTextApplier::BuildReplacementSourceSnapshot(styledText,
                                                                                0u);
  if(replacementSource.hasValidReplacementSource)
  {
    replacementSource.sourceRevision            = NextReplacementSourceRevision();
    impl.GetOrCreateReplacementSourceSnapshot() = std::move(replacementSource);
  }

  if(styledText && !styledPlainText.empty())
  {
    model->mVisualModel->SetTextColor(impl.mTextColor);

    Dali::Ui::Internal::Text::StyledTextApplier::ApplyTextAndStyleRunsToLogicalModel(styledText,
                                                                                     *logicalModel,
                                                                                     GetDpi(),
                                                                                     impl.GetAnchorColor(),
                                                                                     impl.GetAnchorClickedColor());

    const Length characterCount = static_cast<Dali::Ui::Text::Length>(logicalModel->mText.Count());

    // The characters to be added.
    impl.mTextUpdateInfo.mNumberOfCharactersToAdd = characterCount;

    // To reset the cursor position
    lastCursorIndex = characterCount;

    // Update the rest of the model during size negotiation
    impl.QueueModifyEvent(ModifyEvent::TEXT_REPLACED);

    // The natural size needs to be re-calculated.
    impl.mRecalculateNaturalSize    = true;
    impl.mRecalculateLayoutSize     = true;
    impl.mRecalculateHeightForWidth = true;
    impl.mModel->mVisualModel->SetHeightForWidth(Size::ZERO);

    // The text direction needs to be updated.
    impl.mUpdateTextDirection = true;

    // Apply modifications to the model
    impl.mOperationsPending = ALL_OPERATIONS;
  }
  else
  {
    if(nullptr != eventData)
    {
      PlaceholderHandler::ShowPlaceholderText(impl);
      if(!impl.IsShowingPlaceholderText())
      {
        impl.mModel->mVisualModel->SetLayoutSize(Size::ZERO);
      }
    }
    else
    {
      // Clear the layout size cache when the label's text is set to empty.
      impl.mModel->mVisualModel->SetLayoutSize(Size::ZERO);
    }
  }

  unsigned int oldCursorPos = (nullptr != eventData ? eventData->mPrimaryCursorPosition : 0);

  // Resets the cursor position.
  controller.ResetCursorPosition(lastCursorIndex);

  // Scrolls the text to make the cursor visible.
  impl.ResetScrollPosition();

  impl.RequestRelayout();
  impl.RequestAsyncRender();

  if(nullptr != eventData)
  {
    // Cancel previously queued events
    eventData->mEventQueue.clear();
  }

  // Do this last since it provides callbacks into application code.
  if(NULL != impl.mEditableControlInterface)
  {
    impl.mEditableControlInterface->CursorPositionChanged(oldCursorPos, lastCursorIndex);
    impl.mEditableControlInterface->TextChanged(true);
  }
}

void Controller::TextUpdater::InsertText(Controller& controller, const std::string& text, Controller::InsertType type)
{
  Controller::Impl& impl      = *controller.mImpl;
  EventData*&       eventData = impl.mEventData;

  DALI_ASSERT_DEBUG(nullptr != eventData && "Unexpected InsertText")

  if(NULL == eventData)
  {
    return;
  }

  bool         removedPrevious  = false;
  bool         removedSelected  = false;
  bool         maxLengthReached = false;
  unsigned int oldCursorPos     = eventData->mPrimaryCursorPosition;

  DALI_LOG_INFO(gLogFilter, Debug::Verbose,
                "Controller::InsertText %p %s (%s) mPrimaryCursorPosition %d mPreEditFlag %d mPreEditStartPosition %d "
                "mPreEditLength %d\n",
                &controller, text.c_str(), (COMMIT == type ? "COMMIT" : "PRE_EDIT"), eventData->mPrimaryCursorPosition,
                eventData->mPreEditFlag, eventData->mPreEditStartPosition, eventData->mPreEditLength);

  ModelPtr&        model        = impl.mModel;
  LogicalModelPtr& logicalModel = model->mLogicalModel;

  // TODO: At the moment the underline runs are only for pre-edit.
  model->mVisualModel->mUnderlineRuns.Clear();

  // Remove the previous InputMethodContext pre-edit.
  if(eventData->mPreEditFlag && (0u != eventData->mPreEditLength))
  {
    removedPrevious =
      RemoveText(controller, -static_cast<int>(eventData->mPrimaryCursorPosition - eventData->mPreEditStartPosition),
                 eventData->mPreEditLength, DONT_UPDATE_INPUT_STYLE, true);

    eventData->mPrimaryCursorPosition = eventData->mPreEditStartPosition;
    eventData->mPreEditLength         = 0u;
  }
  else
  {
    // Remove the previous Selection.
    removedSelected = RemoveSelectedText(controller);
  }

  Vector<Character> utf32Characters;
  Length            characterCount = 0u;

  std::string redefinedText = text;
  if(!redefinedText.empty())
  {
    if(impl.mInputFilterProcessor != nullptr)
    {
      const bool filteredByAllow = impl.mInputFilterProcessor->ApplyAllowPattern(redefinedText);
      if(filteredByAllow && impl.mEditableControlInterface != nullptr)
      {
        // Signal emits when the string to be inserted is filtered by the allow pattern.
        impl.mEditableControlInterface->InputRejected(Text::InputFilter::RejectReason::NOT_ALLOWED);
      }

      const bool filteredByDeny = impl.mInputFilterProcessor->ApplyDenyPattern(redefinedText);

      if(filteredByDeny && impl.mEditableControlInterface != nullptr)
      {
        // Signal emits when the string to be inserted is filtered by the deny pattern.
        impl.mEditableControlInterface->InputRejected(Text::InputFilter::RejectReason::DENIED);
      }
    }

    //  Convert text into UTF-32
    utf32Characters.Resize(redefinedText.size());

    // This is a bit horrible but std::string returns a (signed) char*
    const uint8_t* utf8 = reinterpret_cast<const uint8_t*>(redefinedText.c_str());

    // Transform a text array encoded in utf8 into an array encoded in utf32.
    // It returns the actual number of characters.
    characterCount = Utf8ToUtf32(utf8, static_cast<uint32_t>(redefinedText.size()), utf32Characters.Begin());
    utf32Characters.Resize(characterCount);

    DALI_ASSERT_DEBUG(redefinedText.size() >= utf32Characters.Count() && "Invalid UTF32 conversion length");
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "UTF8 size %d, UTF32 size %d\n", redefinedText.size(),
                  utf32Characters.Count());
  }

  if(0u != utf32Characters.Count()) // Check if Utf8ToUtf32 conversion succeeded
  {
    // The placeholder text is no longer needed
    if(impl.IsShowingPlaceholderText())
    {
      ResetText(controller);
    }

    impl.ChangeState(EventData::EDITING);

    // Handle the InputMethodContext (predicitive text) state changes
    if(COMMIT == type)
    {
      // InputMethodContext is no longer handling key-events
      impl.ClearPreEditFlag();
    }
    else // PRE_EDIT
    {
      if(!eventData->mPreEditFlag)
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Entered PreEdit state\n");

        // Record the start of the pre-edit text
        eventData->mPreEditStartPosition = eventData->mPrimaryCursorPosition;
      }

      eventData->mPreEditLength = static_cast<Dali::Ui::Text::Length>(utf32Characters.Count());
      eventData->mPreEditFlag   = true;

      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "mPreEditStartPosition %d mPreEditLength %d\n",
                    eventData->mPreEditStartPosition, eventData->mPreEditLength);
    }

    const Length numberOfCharactersInModel = static_cast<Dali::Ui::Text::Length>(logicalModel->mText.Count());

    // Restrict new text to fit within Maximum characters setting.
    Length temp_length      = (impl.mMaximumNumberOfCharacters > numberOfCharactersInModel
                                 ? impl.mMaximumNumberOfCharacters - numberOfCharactersInModel
                                 : 0);
    Length maxSizeOfNewText = std::min(temp_length, characterCount);
    maxLengthReached        = (characterCount > maxSizeOfNewText);

    // The cursor position.
    CharacterIndex& cursorIndex = eventData->mPrimaryCursorPosition;
    cursorIndex                 = impl.NormalizeReplacementBoundary(cursorIndex,
                                                                    ReplacementEditNormalizer::BoundaryAffinity::TRAILING);

    impl.PrepareEditableTextEdit(cursorIndex, 0u, maxSizeOfNewText);

    // Update the text's style.

    // Updates the text style runs by adding characters.
    logicalModel->UpdateTextStyleRuns(cursorIndex, maxSizeOfNewText);

    // Get the character index from the cursor index.
    const CharacterIndex styleIndex = (cursorIndex > 0u) ? cursorIndex - 1u : 0u;

    // Retrieve the text's style for the given index.
    Ui::Integration::Text::InputStyle style;
    impl.RetrieveDefaultInputStyle(style);
    logicalModel->RetrieveStyle(styleIndex, style);

    Ui::Integration::Text::InputStyle& inputStyle = eventData->mInputStyle;

    // Whether to add a new text color run.
    const bool addColorRun = (style.textColor != inputStyle.textColor) && !inputStyle.isDefaultColor;

    // Whether to add a new font run.
    const bool addFontNameRun   = (style.familyName != inputStyle.familyName) && inputStyle.isFamilyDefined;
    const bool addFontWeightRun = (style.weight != inputStyle.weight) && inputStyle.isWeightDefined;
    const bool addFontWidthRun  = (style.width != inputStyle.width) && inputStyle.isWidthDefined;
    const bool addFontSlantRun  = (style.slant != inputStyle.slant) && inputStyle.isSlantDefined;
    const bool addFontSizeRun   = (!Dali::Equals(style.size, inputStyle.size)) && inputStyle.isSizeDefined;

    // Add style runs.
    if(addColorRun)
    {
      const VectorBase::SizeType numberOfRuns = logicalModel->mColorRuns.Count();
      logicalModel->mColorRuns.Resize(numberOfRuns + 1u);

      ColorRun& colorRun                       = *(logicalModel->mColorRuns.Begin() + numberOfRuns);
      colorRun.color                           = inputStyle.textColor;
      colorRun.characterRun.characterIndex     = cursorIndex;
      colorRun.characterRun.numberOfCharacters = maxSizeOfNewText;
    }

    if(addFontNameRun || addFontWeightRun || addFontWidthRun || addFontSlantRun || addFontSizeRun)
    {
      const VectorBase::SizeType numberOfRuns = logicalModel->mFontDescriptionRuns.Count();
      logicalModel->mFontDescriptionRuns.Resize(numberOfRuns + 1u);

      FontDescriptionRun& fontDescriptionRun = *(logicalModel->mFontDescriptionRuns.Begin() + numberOfRuns);

      if(addFontNameRun)
      {
        fontDescriptionRun.familyLength = static_cast<Dali::Ui::Text::Length>(inputStyle.familyName.size());
        fontDescriptionRun.familyName   = new char[fontDescriptionRun.familyLength];
        memcpy(fontDescriptionRun.familyName, inputStyle.familyName.c_str(), fontDescriptionRun.familyLength);
        fontDescriptionRun.familyDefined = true;

        // The memory allocated for the font family name is freed when the font description is removed from the logical
        // model.
      }

      if(addFontWeightRun)
      {
        fontDescriptionRun.weight        = inputStyle.weight;
        fontDescriptionRun.weightDefined = true;
      }

      if(addFontWidthRun)
      {
        fontDescriptionRun.width        = inputStyle.width;
        fontDescriptionRun.widthDefined = true;
      }

      if(addFontSlantRun)
      {
        fontDescriptionRun.slant        = inputStyle.slant;
        fontDescriptionRun.slantDefined = true;
      }

      if(addFontSizeRun)
      {
        fontDescriptionRun.size        = static_cast<PointSize26Dot6>(inputStyle.size * impl.GetEffectiveTextScale() * 64.f);
        fontDescriptionRun.sizeDefined = true;
      }

      fontDescriptionRun.characterRun.characterIndex     = cursorIndex;
      fontDescriptionRun.characterRun.numberOfCharacters = maxSizeOfNewText;
    }

    // Insert at current cursor position.
    Vector<Character>& modifyText = logicalModel->mText;

    auto pos = modifyText.End();
    if(cursorIndex < numberOfCharactersInModel)
    {
      pos = modifyText.Begin() + cursorIndex;
    }
    unsigned int realPos = static_cast<unsigned int>(pos - modifyText.Begin());
    modifyText.Insert(pos, utf32Characters.Begin(), utf32Characters.Begin() + maxSizeOfNewText);

    if(NULL != impl.mEditableControlInterface)
    {
      std::string insertedText;
      Utf32ToUtf8(utf32Characters.Begin(), maxSizeOfNewText, insertedText);
      impl.mEditableControlInterface->TextInserted(realPos, maxSizeOfNewText, insertedText);
    }

    TextUpdateInfo& textUpdateInfo = impl.mTextUpdateInfo;

    // Mark the first paragraph to be updated.
    if(Layout::Engine::SINGLE_LINE_BOX == impl.mLayoutEngine.GetLayout())
    {
      textUpdateInfo.mCharacterIndex             = 0;
      textUpdateInfo.mNumberOfCharactersToRemove = textUpdateInfo.mPreviousNumberOfCharacters;
      textUpdateInfo.mNumberOfCharactersToAdd    = numberOfCharactersInModel + maxSizeOfNewText;
      textUpdateInfo.mClearAll                   = true;
    }
    else
    {
      textUpdateInfo.mCharacterIndex = std::min(cursorIndex, textUpdateInfo.mCharacterIndex);
      textUpdateInfo.mNumberOfCharactersToAdd += maxSizeOfNewText;
    }

    // Update the cursor index.
    cursorIndex += maxSizeOfNewText;

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Inserted %d characters, new size %d new cursor %d\n", maxSizeOfNewText,
                  logicalModel->mText.Count(), eventData->mPrimaryCursorPosition);
  }

  if((0u == logicalModel->mText.Count()) && impl.IsPlaceholderAvailable())
  {
    // Show place-holder if empty after removing the pre-edit text
    PlaceholderHandler::ShowPlaceholderText(impl);
    eventData->mUpdateCursorPosition = true;
    impl.ClearPreEditFlag();
  }
  else if(removedPrevious || removedSelected || (0 != utf32Characters.Count()))
  {
    // Queue an inserted event
    impl.QueueModifyEvent(ModifyEvent::TEXT_INSERTED);

    eventData->mUpdateCursorPosition = true;
    if(removedSelected)
    {
      eventData->mScrollAfterDelete = true;
    }
    else
    {
      eventData->mScrollAfterUpdatePosition = true;
    }
  }

  if(nullptr != impl.mEditableControlInterface)
  {
    impl.mEditableControlInterface->CursorPositionChanged(oldCursorPos, eventData->mPrimaryCursorPosition);
  }

  if(maxLengthReached)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "MaxLengthReached (%d)\n", logicalModel->mText.Count());

    impl.ResetInputMethodContext();

    if(NULL != impl.mEditableControlInterface)
    {
      // Do this last since it provides callbacks into application code
      impl.mEditableControlInterface->MaximumLengthReached();
    }
  }
}

void Controller::TextUpdater::PasteText(Controller& controller, const std::string& stringToPaste)
{
  InsertText(controller, stringToPaste, Text::Controller::COMMIT);
  Controller::Impl& impl = *controller.mImpl;
  impl.ChangeState(EventData::EDITING);
  impl.RequestRelayout();

  if(NULL != impl.mEditableControlInterface)
  {
    // Do this last since it provides callbacks into application code
    impl.mEditableControlInterface->TextChanged(true);
  }
}

bool Controller::TextUpdater::RemoveText(Controller& controller, int cursorOffset, int numberOfCharacters,
                                         UpdateInputStyleType type, bool isDeletingPreEdit)
{
  bool removed   = false;
  bool removeAll = false;

  Controller::Impl& impl      = *controller.mImpl;
  EventData*&       eventData = impl.mEventData;

  if(nullptr == eventData)
  {
    return removed;
  }

  ModelPtr&        model        = impl.mModel;
  LogicalModelPtr& logicalModel = model->mLogicalModel;
  VisualModelPtr&  visualModel  = model->mVisualModel;

  DALI_LOG_INFO(gLogFilter, Debug::General,
                "Controller::RemoveText %p mText.Count() %d cursor %d cursorOffset %d numberOfCharacters %d\n",
                &controller, logicalModel->mText.Count(), eventData->mPrimaryCursorPosition, cursorOffset,
                numberOfCharacters);

  if(!impl.IsShowingPlaceholderText())
  {
    // Delete at current cursor position
    Vector<Character>& currentText         = logicalModel->mText;
    CharacterIndex&    previousCursorIndex = eventData->mPrimaryCursorPosition;

    CharacterIndex cursorIndex = 0;

    // Validate the cursor position & number of characters
    if((static_cast<int>(eventData->mPrimaryCursorPosition) + cursorOffset) >= 0)
    {
      cursorIndex = eventData->mPrimaryCursorPosition + cursorOffset;
    }

    const CharacterIndex requestedCursorIndex = cursorIndex;
    const int            requestedCount       = numberOfCharacters;
    if(impl.HasValidReplacementSource())
    {
      CharacterRun normalized = ReplacementEditNormalizer::NormalizeDeletion(
        impl.GetReplacementSourceSnapshot().runs,
        CharacterRun{cursorIndex, static_cast<Length>(std::max(numberOfCharacters, 0))},
        static_cast<Dali::Ui::Text::Length>(currentText.Count()));
      cursorIndex        = normalized.characterIndex;
      numberOfCharacters = static_cast<int>(normalized.numberOfCharacters);
    }
    const bool replacementNormalized = cursorIndex != requestedCursorIndex || numberOfCharacters != requestedCount;

    // Handle Emoji clustering for cursor handling
    //  Deletion case: this is handling the deletion cases when the cursor is before or after Emoji
    //   - Before: when use delete key and cursor is before Emoji (cursorOffset = -1)
    //   - After: when use backspace key and cursor is after Emoji (cursorOffset = 0)

    const Script script = logicalModel->GetScript(cursorIndex);
    if(!replacementNormalized && (numberOfCharacters == 1u) && (IsOneOfEmojiScripts(script)))
    {
      // TODO: Use this clustering for Emoji cases only. This needs more testing to generalize to all scripts.
      CharacterRun emojiClusteredCharacters =
        RetrieveClusteredCharactersOfCharacterIndex(visualModel, logicalModel, cursorIndex);
      Length actualNumberOfCharacters = emojiClusteredCharacters.numberOfCharacters;

      // Set cursorIndex at the first characterIndex of clustred Emoji
      cursorIndex = emojiClusteredCharacters.characterIndex;

      numberOfCharacters = actualNumberOfCharacters;
    }

    if(!replacementNormalized && HasLigatureMustBreak(script) && cursorOffset == 0) // delete key.
    {
      GlyphIndex glyphIndex               = *(visualModel->mCharactersToGlyph.Begin() + cursorIndex);
      Length     actualNumberOfCharacters = *(visualModel->mCharactersPerGlyph.Begin() + glyphIndex);
      if(actualNumberOfCharacters == 2u &&
         TextAbstraction::IsCombiningDiacriticalMarks(*(currentText.Begin() + cursorIndex + 1u)))
      {
        numberOfCharacters = 2u;
      }
    }

    if((cursorIndex + numberOfCharacters) > currentText.Count())
    {
      numberOfCharacters = static_cast<int32_t>(currentText.Count() - cursorIndex);
    }

    if((cursorIndex == 0) && (currentText.Count() - numberOfCharacters == 0))
    {
      removeAll = true;
    }

    TextUpdateInfo& textUpdateInfo = impl.mTextUpdateInfo;

    if(eventData->mPreEditFlag || removeAll || // If the preedit flag is enabled, it means two (or more) of them came
                                               // together i.e. when two keys have been pressed at the same time.
       ((cursorIndex + numberOfCharacters) <= textUpdateInfo.mPreviousNumberOfCharacters))
    {
      // Mark the paragraphs to be updated.
      if(Layout::Engine::SINGLE_LINE_BOX == impl.mLayoutEngine.GetLayout())
      {
        textUpdateInfo.mCharacterIndex             = 0;
        textUpdateInfo.mNumberOfCharactersToRemove = textUpdateInfo.mPreviousNumberOfCharacters;
        textUpdateInfo.mNumberOfCharactersToAdd    = textUpdateInfo.mPreviousNumberOfCharacters - numberOfCharacters;
        textUpdateInfo.mClearAll                   = true;
      }
      else
      {
        textUpdateInfo.mCharacterIndex = std::min(cursorIndex, textUpdateInfo.mCharacterIndex);
        textUpdateInfo.mNumberOfCharactersToRemove += numberOfCharacters;
      }

      // Update the input style and remove the text's style before removing the text.

      if(UPDATE_INPUT_STYLE == type)
      {
        Ui::Integration::Text::InputStyle& eventDataInputStyle = eventData->mInputStyle;

        // Keep a copy of the current input style.
        Ui::Integration::Text::InputStyle currentInputStyle;
        currentInputStyle.Copy(eventDataInputStyle);

        // Set first the default input style.
        impl.RetrieveDefaultInputStyle(eventDataInputStyle);

        // Update the input style.
        logicalModel->RetrieveStyle(cursorIndex, eventDataInputStyle);

        // Compare if the input style has changed.
        const bool hasInputStyleChanged = !currentInputStyle.Equal(eventDataInputStyle);

        if(hasInputStyleChanged)
        {
          const Ui::Integration::Text::InputStyle::Mask styleChangedMask = currentInputStyle.GetInputStyleChangeMask(eventDataInputStyle);
          // Queue the input style changed signal.
          eventData->mInputStyleChangedQueue.PushBack(styleChangedMask);
        }
      }

      // If the number of current text and the number of characters to be deleted are same,
      // it means all texts should be removed and all Preedit variables should be initialized.
      if(removeAll)
      {
        impl.ClearPreEditFlag();
        if(!isDeletingPreEdit)
        {
          textUpdateInfo.mNumberOfCharactersToAdd = 0;
        }
      }

      // Updates the text style runs by removing characters. Runs with no characters are removed.
      impl.PrepareEditableTextEdit(cursorIndex, numberOfCharacters, 0u);
      logicalModel->UpdateTextStyleRuns(cursorIndex, -numberOfCharacters);

      // Remove the characters.
      Vector<Character>::Iterator first = currentText.Begin() + cursorIndex;
      Vector<Character>::Iterator last  = first + numberOfCharacters;

      if(NULL != impl.mEditableControlInterface)
      {
        std::string utf8;
        Utf32ToUtf8(first, numberOfCharacters, utf8);
        if(!isDeletingPreEdit)
        {
          impl.mEditableControlInterface->TextDeleted(cursorIndex, numberOfCharacters, utf8);
        }
      }

      currentText.Erase(first, last);

      if(nullptr != impl.mEditableControlInterface)
      {
        impl.mEditableControlInterface->CursorPositionChanged(previousCursorIndex, cursorIndex);
      }

      // Cursor position retreat
      previousCursorIndex = cursorIndex;

      eventData->mScrollAfterDelete = true;

      if(EventData::INACTIVE == eventData->mState)
      {
        impl.ChangeState(EventData::EDITING);
      }

      DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::RemoveText %p removed %d\n", &controller,
                    numberOfCharacters);
      removeAll = false;
      removed   = true;
    }
  }

  return removed;
}

bool Controller::TextUpdater::RemoveSelectedText(Controller& controller)
{
  bool textRemoved(false);

  Controller::Impl& impl = *controller.mImpl;

  if(EventData::SELECTING == impl.mEventData->mState)
  {
    std::string removedString;
    uint32_t    oldSelStart = impl.mEventData->mLeftSelectionPosition;
    uint32_t    oldSelEnd   = impl.mEventData->mRightSelectionPosition;

    impl.RetrieveSelection(removedString, true);

    if(!removedString.empty())
    {
      textRemoved = true;
      impl.ChangeState(EventData::EDITING);

      if(impl.mSelectableControlInterface != nullptr)
      {
        impl.mSelectableControlInterface->SelectionChanged(
          oldSelStart, oldSelEnd, impl.mEventData->mPrimaryCursorPosition, impl.mEventData->mPrimaryCursorPosition);
      }
    }
  }

  return textRemoved;
}

void Controller::TextUpdater::ResetText(Controller& controller)
{
  Controller::Impl& impl         = *controller.mImpl;
  LogicalModelPtr&  logicalModel = impl.mModel->mLogicalModel;

  const bool hadReplacementData = impl.HasReplacementData();
  if(hadReplacementData)
  {
    impl.InvalidateReplacementRenderState();
  }
  impl.ClearEndEllipsisResult();

  // Reset buffers.
  logicalModel->mText.Clear();

  // A source mutation invalidates every sync/async placement and image visual.
  if(hadReplacementData)
  {
    impl.ClearReplacementData();
  }
  // Reset the anchors buffer.
  logicalModel->ClearAnchors();

  // We have cleared everything including the placeholder-text
  impl.PlaceholderCleared();

  impl.mTextUpdateInfo.mCharacterIndex             = 0u;
  impl.mTextUpdateInfo.mNumberOfCharactersToRemove = impl.mTextUpdateInfo.mPreviousNumberOfCharacters;
  impl.mTextUpdateInfo.mNumberOfCharactersToAdd    = 0u;

  // Clear any previous text.
  impl.mTextUpdateInfo.mClearAll = true;

  // The natural size needs to be re-calculated.
  impl.mRecalculateNaturalSize    = true;
  impl.mRecalculateLayoutSize     = true;
  impl.mRecalculateHeightForWidth = true;

  // The text direction needs to be updated.
  impl.mUpdateTextDirection = true;

  // Apply modifications to the model
  impl.mOperationsPending = ALL_OPERATIONS;
}

} // namespace Text

} // namespace Ui

} // namespace Dali
