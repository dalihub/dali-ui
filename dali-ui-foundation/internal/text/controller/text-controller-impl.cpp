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

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptor-framework/clipboard-integ.h>
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/adaptor-framework/clipboard-data.h>
#include <dali/public-api/rendering/renderer.h>
#include <algorithm>
#include <cmath>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/text/text-control-interface.h>
#include <dali-ui-foundation/integration-api/text/text-editable-control-interface.h>
#include <dali-ui-foundation/internal/controls/text-controls/common-text-utils.h>
#include <dali-ui-foundation/internal/text/character-set-conversion.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl-data-clearer.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl-event-handler.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl-model-updater.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-placeholder-handler.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-relayouter.h>
#include <dali-ui-foundation/internal/text/cursor-helper-functions.h>
#include <dali-ui-foundation/internal/text/glyph-metrics-helper.h>
#include <dali-ui-foundation/internal/text/line-helper-functions.h>
#include <dali-ui-foundation/internal/text/text-enumerations-impl.h>
#include <dali-ui-foundation/internal/text/text-run-container.h>
#include <dali-ui-foundation/internal/text/text-selection-handle-controller.h>
#include <dali-ui-foundation/internal/text/underlined-glyph-run.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>

using namespace Dali;

using Dali::Integration::ToPropertyValue;

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

constexpr float MAX_FLOAT = std::numeric_limits<float>::max();

const char* EMPTY_STRING         = "";
const char* MIME_TYPE_TEXT_PLAIN = "text/plain;charset=utf-8";
const char* MIME_TYPE_HTML       = "application/xhtml+xml";

} // namespace

namespace Dali::Ui::Text
{
namespace
{
void SetDefaultInputStyle(Ui::Integration::Text::InputStyle& inputStyle, const FontDefaults* const fontDefaults, const Vector4& textColor)
{
  // Sets the default text's color.
  inputStyle.textColor      = textColor;
  inputStyle.isDefaultColor = true;

  inputStyle.familyName.clear();
  inputStyle.weight = TextAbstraction::FontWeight::NORMAL;
  inputStyle.width  = TextAbstraction::FontWidth::NORMAL;
  inputStyle.slant  = TextAbstraction::FontSlant::NORMAL;
  inputStyle.size   = 0.f;

  inputStyle.lineSpacing = 0.f;

  inputStyle.underlineProperties.clear();
  inputStyle.shadowProperties.clear();
  inputStyle.embossProperties.clear();
  inputStyle.outlineProperties.clear();

  inputStyle.isFamilyDefined = false;
  inputStyle.isWeightDefined = false;
  inputStyle.isWidthDefined  = false;
  inputStyle.isSlantDefined  = false;
  inputStyle.isSizeDefined   = false;

  inputStyle.isLineSpacingDefined = false;

  inputStyle.isUnderlineDefined = false;
  inputStyle.isShadowDefined    = false;
  inputStyle.isEmbossDefined    = false;
  inputStyle.isOutlineDefined   = false;

  // Sets the default font's family name, weight, width, slant and size.
  if(fontDefaults)
  {
    if(fontDefaults->familyDefined)
    {
      inputStyle.familyName      = fontDefaults->GetFontDescription().family;
      inputStyle.isFamilyDefined = true;
    }

    if(fontDefaults->weightDefined)
    {
      inputStyle.weight          = fontDefaults->GetFontDescription().weight;
      inputStyle.isWeightDefined = true;
    }

    if(fontDefaults->widthDefined)
    {
      inputStyle.width          = fontDefaults->GetFontDescription().width;
      inputStyle.isWidthDefined = true;
    }

    if(fontDefaults->slantDefined)
    {
      inputStyle.slant          = fontDefaults->GetFontDescription().slant;
      inputStyle.isSlantDefined = true;
    }

    if(fontDefaults->sizeDefined)
    {
      inputStyle.size          = fontDefaults->mDefaultPointSize;
      inputStyle.isSizeDefined = true;
    }
  }
}

void ChangeTextControllerState(Controller::Impl& impl, EventData::State newState)
{
  EventData* eventData = impl.mEventData;

  if(nullptr == eventData)
  {
    // Nothing to do if there is no text input.
    return;
  }

  DecoratorPtr& decorator = eventData->mDecorator;
  if(!decorator)
  {
    // Nothing to do if there is no decorator.
    return;
  }

  DALI_LOG_INFO(gLogFilter, Debug::General, "ChangeState state:%d  newstate:%d\n", eventData->mState, newState);

  if(eventData->mState != newState)
  {
    eventData->mPreviousState = eventData->mState;
    eventData->mState         = newState;

    switch(eventData->mState)
    {
      case EventData::INACTIVE:
      {
        decorator->SetActiveCursor(ACTIVE_CURSOR_NONE);
        decorator->StopCursorBlink();
        decorator->SetHandleActive(GRAB_HANDLE, false);
        decorator->SetHandleActive(LEFT_SELECTION_HANDLE, false);
        decorator->SetHandleActive(RIGHT_SELECTION_HANDLE, false);
        decorator->SetHighlightActive(false);
        decorator->SetPopupActive(false);
        eventData->mDecoratorUpdated = true;
        break;
      }

      case EventData::INTERRUPTED:
      {
        decorator->SetHandleActive(GRAB_HANDLE, false);
        decorator->SetHandleActive(LEFT_SELECTION_HANDLE, false);
        decorator->SetHandleActive(RIGHT_SELECTION_HANDLE, false);
        decorator->SetHighlightActive(false);
        decorator->SetPopupActive(false);
        eventData->mDecoratorUpdated = true;
        break;
      }

      case EventData::SELECTING:
      {
        decorator->SetActiveCursor(ACTIVE_CURSOR_NONE);
        decorator->StopCursorBlink();
        decorator->SetHandleActive(GRAB_HANDLE, false);
        if(eventData->mGrabHandleEnabled)
        {
          decorator->SetHandleActive(LEFT_SELECTION_HANDLE, true);
          decorator->SetHandleActive(RIGHT_SELECTION_HANDLE, true);
        }
        decorator->SetHighlightActive(true);
        if(eventData->mGrabHandlePopupEnabled)
        {
          impl.SetPopupButtons();
          decorator->SetPopupActive(true);
        }
        eventData->mDecoratorUpdated = true;
        break;
      }

      case EventData::EDITING:
      {
        decorator->SetActiveCursor(ACTIVE_CURSOR_PRIMARY);
        if(eventData->mCursorBlinkEnabled)
        {
          decorator->StartCursorBlink();
        }
        // Grab handle is not shown until a tap is received whilst EDITING
        decorator->SetHandleActive(GRAB_HANDLE, false);
        decorator->SetHandleActive(LEFT_SELECTION_HANDLE, false);
        decorator->SetHandleActive(RIGHT_SELECTION_HANDLE, false);
        decorator->SetHighlightActive(false);
        if(eventData->mGrabHandlePopupEnabled)
        {
          decorator->SetPopupActive(false);
        }
        eventData->mDecoratorUpdated = true;
        break;
      }
      case EventData::EDITING_WITH_POPUP:
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "EDITING_WITH_POPUP \n", newState);

        decorator->SetActiveCursor(ACTIVE_CURSOR_PRIMARY);
        if(eventData->mCursorBlinkEnabled)
        {
          decorator->StartCursorBlink();
        }
        if(eventData->mSelectionEnabled)
        {
          decorator->SetHandleActive(LEFT_SELECTION_HANDLE, false);
          decorator->SetHandleActive(RIGHT_SELECTION_HANDLE, false);
          decorator->SetHighlightActive(false);
        }
        else if(eventData->mGrabHandleEnabled)
        {
          decorator->SetHandleActive(GRAB_HANDLE, true);
        }
        if(eventData->mGrabHandlePopupEnabled)
        {
          impl.SetPopupButtons();
          decorator->SetPopupActive(true);
        }
        eventData->mDecoratorUpdated = true;
        break;
      }
      case EventData::EDITING_WITH_GRAB_HANDLE:
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "EDITING_WITH_GRAB_HANDLE \n", newState);

        decorator->SetActiveCursor(ACTIVE_CURSOR_PRIMARY);
        if(eventData->mCursorBlinkEnabled)
        {
          decorator->StartCursorBlink();
        }
        // Grab handle is not shown until a tap is received whilst EDITING
        if(eventData->mGrabHandleEnabled)
        {
          decorator->SetHandleActive(GRAB_HANDLE, true);
        }
        decorator->SetHandleActive(LEFT_SELECTION_HANDLE, false);
        decorator->SetHandleActive(RIGHT_SELECTION_HANDLE, false);
        decorator->SetHighlightActive(false);
        if(eventData->mGrabHandlePopupEnabled)
        {
          decorator->SetPopupActive(false);
        }
        eventData->mDecoratorUpdated = true;
        break;
      }

      case EventData::SELECTION_HANDLE_PANNING:
      {
        decorator->SetActiveCursor(ACTIVE_CURSOR_NONE);
        decorator->StopCursorBlink();
        decorator->SetHandleActive(GRAB_HANDLE, false);
        if(eventData->mGrabHandleEnabled)
        {
          decorator->SetHandleActive(LEFT_SELECTION_HANDLE, true);
          decorator->SetHandleActive(RIGHT_SELECTION_HANDLE, true);
        }
        decorator->SetHighlightActive(true);
        if(eventData->mGrabHandlePopupEnabled)
        {
          decorator->SetPopupActive(false);
        }
        eventData->mDecoratorUpdated = true;
        break;
      }

      case EventData::GRAB_HANDLE_PANNING:
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "GRAB_HANDLE_PANNING \n", newState);

        decorator->SetActiveCursor(ACTIVE_CURSOR_PRIMARY);
        if(eventData->mCursorBlinkEnabled)
        {
          decorator->StartCursorBlink();
        }
        if(eventData->mGrabHandleEnabled)
        {
          decorator->SetHandleActive(GRAB_HANDLE, true);
        }
        decorator->SetHandleActive(LEFT_SELECTION_HANDLE, false);
        decorator->SetHandleActive(RIGHT_SELECTION_HANDLE, false);
        decorator->SetHighlightActive(false);
        if(eventData->mGrabHandlePopupEnabled)
        {
          decorator->SetPopupActive(false);
        }
        eventData->mDecoratorUpdated = true;
        break;
      }

      case EventData::EDITING_WITH_PASTE_POPUP:
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "EDITING_WITH_PASTE_POPUP \n", newState);

        decorator->SetActiveCursor(ACTIVE_CURSOR_PRIMARY);
        if(eventData->mCursorBlinkEnabled)
        {
          decorator->StartCursorBlink();
        }

        if(eventData->mGrabHandleEnabled)
        {
          decorator->SetHandleActive(GRAB_HANDLE, true);
        }
        decorator->SetHandleActive(LEFT_SELECTION_HANDLE, false);
        decorator->SetHandleActive(RIGHT_SELECTION_HANDLE, false);
        decorator->SetHighlightActive(false);

        if(eventData->mGrabHandlePopupEnabled)
        {
          impl.SetPopupButtons();
          decorator->SetPopupActive(true);
        }
        eventData->mDecoratorUpdated = true;
        break;
      }

      case EventData::TEXT_PANNING:
      {
        decorator->SetActiveCursor(ACTIVE_CURSOR_NONE);
        decorator->StopCursorBlink();
        decorator->SetHandleActive(GRAB_HANDLE, false);
        if(eventData->mDecorator->IsHandleActive(LEFT_SELECTION_HANDLE) ||
           decorator->IsHandleActive(RIGHT_SELECTION_HANDLE))
        {
          decorator->SetHandleActive(LEFT_SELECTION_HANDLE, false);
          decorator->SetHandleActive(RIGHT_SELECTION_HANDLE, false);
          decorator->SetHighlightActive(true);
        }

        if(eventData->mGrabHandlePopupEnabled)
        {
          decorator->SetPopupActive(false);
        }

        eventData->mDecoratorUpdated = true;
        break;
      }
    }
  }
}

void UpdateCursorPositionForAlignment(Controller::Impl& impl, bool needFullAlignment)
{
  EventData* eventData = impl.mEventData;

  // Set the flag to redo the alignment operation
  impl.mOperationsPending =
    static_cast<Controller::OperationsMask>(impl.mOperationsPending | Controller::OperationsMask::ALIGN);

  if(eventData)
  {
    // Note: mUpdateAlignment is currently only needed for horizontal alignment
    eventData->mUpdateAlignment = needFullAlignment;

    // Update the cursor if it's in editing mode
    if(EventData::IsEditingState(eventData->mState))
    {
      impl.ChangeState(EventData::EDITING);
      eventData->mUpdateCursorPosition = true;
    }
  }
}

struct ReplacementCursorBoundary
{
  const ReplacementPlacement* placement{nullptr};
  bool                        leading{false};
};

ReplacementCursorBoundary ResolveReplacementCursorBoundary(const ReplacementRenderState& state,
                                                           CharacterIndex                logicalBoundary)
{
  const ReplacementPlacement* trailingPlacement = nullptr;
  for(const ReplacementPlacement& placement : state.placements)
  {
    if(!placement.visible || placement.elided)
    {
      continue;
    }

    const CharacterIndex start = placement.logicalCharacterRange.characterIndex;
    const CharacterIndex end   = start + placement.logicalCharacterRange.numberOfCharacters;
    if(logicalBoundary == start)
    {
      return ReplacementCursorBoundary{&placement, true};
    }
    if(logicalBoundary == end)
    {
      trailingPlacement = &placement;
    }
  }

  return ReplacementCursorBoundary{trailingPlacement, false};
}

void ApplyReplacementCaretGeometry(const ReplacementCursorBoundary& boundary, CursorInfo& cursorInfo)
{
  const ReplacementPlacement* placement = boundary.placement;
  if(placement == nullptr)
  {
    return;
  }

  const ReplacementCaretMetric& metric = boundary.leading ? placement->leadingCaretMetric
                                                          : placement->trailingCaretMetric;
  if(metric.height <= 0.0f)
  {
    return;
  }

  const float leadingEdge  = placement->lineDirection ? placement->position.x + placement->size.x
                                                      : placement->position.x;
  const float trailingEdge = placement->lineDirection ? placement->position.x
                                                      : placement->position.x + placement->size.x;
  const float caretX       = boundary.leading ? leadingEdge : trailingEdge;
  const float caretTop     = placement->baseline - metric.ascender;

  cursorInfo.primaryPosition.x       = caretX;
  cursorInfo.primaryCaretPosition    = Vector2(caretX, caretTop);
  cursorInfo.primaryCaretHeight      = cursorInfo.isSecondaryCursor ? 0.5f * metric.height : metric.height;
  cursorInfo.hasPrimaryCaretGeometry = true;

  if(cursorInfo.isSecondaryCursor)
  {
    cursorInfo.secondaryCaretPosition    = Vector2(cursorInfo.secondaryPosition.x,
                                                   caretTop + 0.5f * metric.height);
    cursorInfo.secondaryCaretHeight      = 0.5f * metric.height;
    cursorInfo.hasSecondaryCaretGeometry = true;
  }
}

} // unnamed Namespace

EventData::EventData(DecoratorPtr decorator, InputMethodContext& inputMethodContext)
: mDecorator(decorator),
  mInputMethodContext(inputMethodContext),
  mPlaceholderFont(nullptr),
  mEditableStyledText(),
  mPlaceholderTextActive(),
  mPlaceholderTextInactive(),
  mPlaceholderTextColor(0.8f, 0.8f, 0.8f, 0.8f),
  mEventQueue(),
  mInputStyleChangedQueue(),
  mPreviousState(INACTIVE),
  mState(INACTIVE),
  mPrimaryCursorPosition(0u),
  mLeftSelectionPosition(0u),
  mRightSelectionPosition(0u),
  mPreEditStartPosition(0u),
  mPreEditLength(0u),
  mCursorHookPositionX(0.f),
  mDoubleTapAction(Controller::NoTextTap::NO_ACTION),
  mLongPressAction(Controller::NoTextTap::SHOW_SELECTION_POPUP),
  mIsShowingPlaceholderText(false),
  mPreEditFlag(false),
  mDecoratorUpdated(false),
  mCursorBlinkEnabled(true),
  mGrabHandleEnabled(true),
  mGrabHandlePopupEnabled(true),
  mSelectionEnabled(true),
  mUpdateCursorHookPosition(false),
  mUpdateCursorPosition(false),
  mUpdateGrabHandlePosition(false),
  mUpdateLeftSelectionPosition(false),
  mUpdateRightSelectionPosition(false),
  mIsLeftHandleSelected(false),
  mIsRightHandleSelected(false),
  mUpdateHighlightBox(false),
  mScrollAfterUpdatePosition(false),
  mScrollAfterDelete(false),
  mAllTextSelected(false),
  mUpdateInputStyle(false),
  mPasswordInput(false),
  mCheckScrollAmount(false),
  mIsPlaceholderPixelSize(false),
  mIsPlaceholderElideEnabled(false),
  mPlaceholderEllipsisFlag(false),
  mShowPlaceholderOnFocus(true),
  mShiftSelectionFlag(true),
  mUpdateAlignment(false),
  mEditingEnabled(true)
{
}

bool Controller::Impl::ProcessInputEvents()
{
  return ControllerImplEventHandler::ProcessInputEvents(*this);
}

void Controller::Impl::SetAnchorColor(const Vector4& color)
{
  mAnchorColor = color;
  UpdateAnchorColor();
}

const Vector4& Controller::Impl::GetAnchorColor() const
{
  return mAnchorColor;
}

void Controller::Impl::SetAnchorClickedColor(const Vector4& color)
{
  mAnchorClickedColor = color;
  UpdateAnchorColor();
}

const Vector4& Controller::Impl::GetAnchorClickedColor() const
{
  return mAnchorClickedColor;
}

void Controller::Impl::UpdateAnchorColor()
{
  if(!mAnchorControlInterface || !mModel->mLogicalModel->mAnchors.Count() || !IsShowingRealText())
  {
    return;
  }

  bool updateNeeded = false;

  // The anchor color & clicked color needs to be updated with the property's color.
  for(auto& anchor : mModel->mLogicalModel->mAnchors)
  {
    if(!anchor.isMarkupColorSet && !anchor.isClicked)
    {
      if(mModel->mLogicalModel->mColorRuns.Count() > anchor.colorRunIndex)
      {
        ColorRun& colorRun = *(mModel->mLogicalModel->mColorRuns.Begin() + anchor.colorRunIndex);
        colorRun.color     = mAnchorColor;
        updateNeeded       = true;
      }
      if(mModel->mLogicalModel->mUnderlinedCharacterRuns.Count() > anchor.underlinedCharacterRunIndex)
      {
        UnderlinedCharacterRun& underlineRun =
          *(mModel->mLogicalModel->mUnderlinedCharacterRuns.Begin() + anchor.underlinedCharacterRunIndex);
        underlineRun.properties.color = mAnchorColor;
        updateNeeded                  = true;
      }
    }
    else if(!anchor.isMarkupClickedColorSet && anchor.isClicked)
    {
      if(mModel->mLogicalModel->mColorRuns.Count() > anchor.colorRunIndex)
      {
        ColorRun& colorRun = *(mModel->mLogicalModel->mColorRuns.Begin() + anchor.colorRunIndex);
        colorRun.color     = mAnchorClickedColor;
        updateNeeded       = true;
      }
      if(mModel->mLogicalModel->mUnderlinedCharacterRuns.Count() > anchor.underlinedCharacterRunIndex)
      {
        UnderlinedCharacterRun& underlineRun =
          *(mModel->mLogicalModel->mUnderlinedCharacterRuns.Begin() + anchor.underlinedCharacterRunIndex);
        underlineRun.properties.color = mAnchorClickedColor;
        updateNeeded                  = true;
      }
    }
  }

  if(updateNeeded)
  {
    ClearFontData();
    mOperationsPending = static_cast<OperationsMask>(mOperationsPending | COLOR);
    RequestRelayout();
    RequestAsyncRender();
  }
}

void Controller::Impl::InvalidateFontData()
{
  ClearFontData();
  UpdateAnchorColor();
  RequestRelayout();
  RequestAsyncRender();
}

void Controller::Impl::InvalidateLayoutDirectionData()
{
  mTextUpdateInfo.mCharacterIndex             = 0u;
  mTextUpdateInfo.mNumberOfCharactersToRemove = mTextUpdateInfo.mPreviousNumberOfCharacters;
  mTextUpdateInfo.mNumberOfCharactersToAdd    = mModel->mLogicalModel->mText.Count();

  mTextUpdateInfo.mClearAll           = true;
  mTextUpdateInfo.mFullRelayoutNeeded = true;
  mRecalculateNaturalSize             = true;
  mRecalculateLayoutSize              = true;
  mRecalculateHeightForWidth          = true;

  mOperationsPending = static_cast<OperationsMask>(mOperationsPending | SHAPE_TEXT | BIDI_INFO | GET_GLYPH_METRICS |
                                                   LAYOUT | UPDATE_LAYOUT_SIZE | REORDER | ALIGN | UPDATE_DIRECTION);

  RequestRelayout();
  RequestAsyncRender();
}

void Controller::Impl::NotifyInputMethodContext()
{
  if(mEventData && mEventData->mInputMethodContext)
  {
    CharacterIndex cursorPosition = GetLogicalCursorPosition();
    Dali::Integration::InputMethodContext::SetCursorPosition(mEventData->mInputMethodContext, cursorPosition);
    Dali::Integration::InputMethodContext::NotifyCursorPosition(mEventData->mInputMethodContext);
  }
}

void Controller::Impl::NotifyInputMethodContextMultiLineStatus()
{
  if(mEventData && mEventData->mInputMethodContext)
  {
    Text::Layout::Engine::Type layout = mLayoutEngine.GetLayout();
    Dali::Integration::InputMethodContext::NotifyTextInputMultiLine(mEventData->mInputMethodContext, layout == Text::Layout::Engine::MULTI_LINE_BOX);
  }
}

CharacterIndex Controller::Impl::GetLogicalCursorPosition() const
{
  CharacterIndex cursorPosition = 0u;

  if(mEventData)
  {
    if((EventData::SELECTING == mEventData->mState) || (EventData::SELECTION_HANDLE_PANNING == mEventData->mState))
    {
      cursorPosition = std::min(mEventData->mRightSelectionPosition, mEventData->mLeftSelectionPosition);
    }
    else
    {
      cursorPosition = mEventData->mPrimaryCursorPosition;
    }
  }

  return cursorPosition;
}

Length Controller::Impl::GetNumberOfWhiteSpaces(CharacterIndex index) const
{
  Length numberOfWhiteSpaces = 0u;

  // Get the buffer to the text.
  Character* utf32CharacterBuffer = mModel->mLogicalModel->mText.Begin();

  const Length totalNumberOfCharacters = static_cast<Dali::Ui::Text::Length>(mModel->mLogicalModel->mText.Count());
  for(; index < totalNumberOfCharacters; ++index, ++numberOfWhiteSpaces)
  {
    if(!TextAbstraction::IsWhiteSpace(*(utf32CharacterBuffer + index)))
    {
      break;
    }
  }

  return numberOfWhiteSpaces;
}

void Controller::Impl::GetText(std::string& text) const
{
  if(!IsShowingPlaceholderText())
  {
    // Retrieves the text string.
    GetText(0u, text);
  }
  else
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Controller::GetText %p empty (but showing placeholder)\n", this);
  }
}

Length Controller::Impl::GetNumberOfCharacters() const
{
  if(!IsShowingPlaceholderText())
  {
    return mModel->GetNumberOfCharacters();
  }
  else
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Controller::GetNumberOfCharacters %p empty (but showing placeholder)\n",
                  this);
    return 0u;
  }
}

void Controller::Impl::GetText(CharacterIndex index, std::string& text) const
{
  // Get the total number of characters.
  Length numberOfCharacters = static_cast<Dali::Ui::Text::Length>(mModel->mLogicalModel->mText.Count());

  // Retrieve the text.
  if(0u != numberOfCharacters)
  {
    Utf32ToUtf8(mModel->mLogicalModel->mText.Begin() + index, numberOfCharacters - index, text);
  }
}

Dali::LayoutDirection::Type Controller::Impl::GetLayoutDirection(Dali::Actor& actor) const
{
  if(mModel->mLayoutDirectionMode == LayoutDirectionMode::LOCALE ||
     (mModel->mLayoutDirectionMode == LayoutDirectionMode::INHERIT && !mIsLayoutDirectionChanged))
  {
    Dali::Integration::SceneHolder sceneHolder = Dali::Integration::SceneHolder::Get(actor);
    return sceneHolder ? sceneHolder.GetRootLayer().GetEffectiveLayoutDirection()
                       : LayoutDirection::LEFT_TO_RIGHT;
  }
  else
  {
    return actor.GetEffectiveLayoutDirection();
  }
}

Direction Controller::Impl::GetTextDirection()
{
  if(mUpdateTextDirection)
  {
    // Operations that can be done only once until the text changes.
    const OperationsMask onlyOnceOperations = static_cast<OperationsMask>(
      CONVERT_TO_UTF32 | GET_SCRIPTS | VALIDATE_FONTS | GET_LINE_BREAKS | BIDI_INFO | SHAPE_TEXT | GET_GLYPH_METRICS);

    // Set the update info to relayout the whole text.
    mTextUpdateInfo.mParagraphCharacterIndex     = 0u;
    mTextUpdateInfo.mRequestedNumberOfCharacters = static_cast<Dali::Ui::Text::Length>(mModel->mLogicalModel->mText.Count());

    // Make sure the model is up-to-date before layouting
    UpdateModel(onlyOnceOperations);

    Vector3 naturalSize;
    Relayouter::DoRelayout(*this, Size(MAX_FLOAT, MAX_FLOAT),
                           static_cast<OperationsMask>(onlyOnceOperations | LAYOUT | REORDER | UPDATE_DIRECTION),
                           naturalSize.GetVectorXY());

    // Do not do again the only once operations.
    mOperationsPending = static_cast<OperationsMask>(mOperationsPending & ~onlyOnceOperations);

    // Clear the update info. This info will be set the next time the text is updated.
    mTextUpdateInfo.Clear();

    // FullRelayoutNeeded should be true because DoRelayout is MAX_FLOAT, MAX_FLOAT.
    mTextUpdateInfo.mFullRelayoutNeeded = true;

    mUpdateTextDirection = false;
  }

  return mIsTextDirectionRTL ? Direction::RIGHT_TO_LEFT : Direction::LEFT_TO_RIGHT;
}

void Controller::Impl::CalculateTextUpdateIndices(Length& numberOfCharacters)
{
  mTextUpdateInfo.mParagraphCharacterIndex = 0u;
  mTextUpdateInfo.mStartGlyphIndex         = 0u;
  mTextUpdateInfo.mStartLineIndex          = 0u;
  numberOfCharacters                       = 0u;

  const Length numberOfParagraphs = static_cast<Dali::Ui::Text::Length>(mModel->mLogicalModel->mParagraphInfo.Count());
  if(0u == numberOfParagraphs)
  {
    mTextUpdateInfo.mParagraphCharacterIndex = 0u;
    numberOfCharacters                       = 0u;

    mTextUpdateInfo.mRequestedNumberOfCharacters =
      mTextUpdateInfo.mNumberOfCharactersToAdd - mTextUpdateInfo.mNumberOfCharactersToRemove;

    return;
  }

  if(mTextUpdateInfo.mFullRelayoutNeeded)
  {
    const Length currentNumberOfCharacters = static_cast<Dali::Ui::Text::Length>(mModel->mLogicalModel->mText.Count());

    // A full relayout must rebuild the update range from the beginning.
    // Do not use mCharacterIndex here because it may still point to the last
    // edited position. Otherwise the range can be narrowed to a later paragraph
    // and active visual lines can be cleared partially.
    mTextUpdateInfo.mCharacterIndex              = 0u;
    mTextUpdateInfo.mParagraphCharacterIndex     = 0u;
    mTextUpdateInfo.mRequestedNumberOfCharacters = currentNumberOfCharacters;
    mTextUpdateInfo.mStartGlyphIndex             = 0u;
    mTextUpdateInfo.mStartLineIndex              = 0u;

    numberOfCharacters = mTextUpdateInfo.mPreviousNumberOfCharacters;
    return;
  }

  // Find the paragraphs to be updated.
  Vector<ParagraphRunIndex> paragraphsToBeUpdated;
  if(mTextUpdateInfo.mCharacterIndex >= mTextUpdateInfo.mPreviousNumberOfCharacters)
  {
    // Text is being added at the end of the current text.
    if(mTextUpdateInfo.mIsLastCharacterNewParagraph)
    {
      // Text is being added in a new paragraph after the last character of the text.
      mTextUpdateInfo.mParagraphCharacterIndex = mTextUpdateInfo.mPreviousNumberOfCharacters;
      numberOfCharacters                       = 0u;
      mTextUpdateInfo.mRequestedNumberOfCharacters =
        mTextUpdateInfo.mNumberOfCharactersToAdd - mTextUpdateInfo.mNumberOfCharactersToRemove;

      mTextUpdateInfo.mStartGlyphIndex = static_cast<Dali::Ui::Text::GlyphIndex>(mModel->mVisualModel->mGlyphs.Count());
      mTextUpdateInfo.mStartLineIndex =
        static_cast<LineIndex>((mModel->mVisualModel->mLines.Count() > 0u) ? mModel->mVisualModel->mLines.Count() - 1u : 0u);

      return;
    }

    paragraphsToBeUpdated.PushBack(numberOfParagraphs - 1u);
  }
  else
  {
    const Length numberOfCharactersToUpdate =
      (mTextUpdateInfo.mNumberOfCharactersToRemove > 0u) ? mTextUpdateInfo.mNumberOfCharactersToRemove : 1u;

    mModel->mLogicalModel->FindParagraphs(mTextUpdateInfo.mCharacterIndex,
                                          numberOfCharactersToUpdate,
                                          paragraphsToBeUpdated);
  }

  if(0u != paragraphsToBeUpdated.Count())
  {
    const ParagraphRunIndex firstParagraphIndex = *(paragraphsToBeUpdated.Begin());
    const ParagraphRun&     firstParagraph      = *(mModel->mLogicalModel->mParagraphInfo.Begin() + firstParagraphIndex);
    mTextUpdateInfo.mParagraphCharacterIndex    = firstParagraph.characterRun.characterIndex;

    ParagraphRunIndex   lastParagraphIndex = *(paragraphsToBeUpdated.End() - 1u);
    const ParagraphRun& lastParagraph      = *(mModel->mLogicalModel->mParagraphInfo.Begin() + lastParagraphIndex);

    if((mTextUpdateInfo.mNumberOfCharactersToRemove > 0u) &&
       (lastParagraphIndex < numberOfParagraphs - 1u) &&
       ((lastParagraph.characterRun.characterIndex + lastParagraph.characterRun.numberOfCharacters) ==
        (mTextUpdateInfo.mCharacterIndex + mTextUpdateInfo.mNumberOfCharactersToRemove)))
    {
      // The new paragraph character of the last updated paragraph has been removed so it is going to be merged with
      // the next one.
      const ParagraphRun& nextParagraph = *(mModel->mLogicalModel->mParagraphInfo.Begin() + lastParagraphIndex + 1u);

      numberOfCharacters = nextParagraph.characterRun.characterIndex + nextParagraph.characterRun.numberOfCharacters -
                           mTextUpdateInfo.mParagraphCharacterIndex;
    }
    else
    {
      numberOfCharacters = lastParagraph.characterRun.characterIndex + lastParagraph.characterRun.numberOfCharacters -
                           mTextUpdateInfo.mParagraphCharacterIndex;
    }
  }

  mTextUpdateInfo.mRequestedNumberOfCharacters =
    numberOfCharacters + mTextUpdateInfo.mNumberOfCharactersToAdd - mTextUpdateInfo.mNumberOfCharactersToRemove;

  mTextUpdateInfo.mStartGlyphIndex =
    *(mModel->mVisualModel->mCharactersToGlyph.Begin() + mTextUpdateInfo.mParagraphCharacterIndex);
}

void Controller::Impl::ClearModelData(CharacterIndex startIndex, CharacterIndex endIndex, OperationsMask operations)
{
  ControllerImplDataClearer::ClearModelData(*this, startIndex, endIndex, operations);
}

bool Controller::Impl::UpdateModel(OperationsMask operationsRequired)
{
  return ControllerImplModelUpdater::Update(*this, operationsRequired);
}

void Controller::Impl::RetrieveDefaultInputStyle(Ui::Integration::Text::InputStyle& inputStyle)
{
  SetDefaultInputStyle(inputStyle, mFontDefaults, mTextColor);
}

float Controller::Impl::GetDefaultFontLineHeight()
{
  FontId defaultFontId = 0u;
  if(nullptr == mFontDefaults)
  {
    TextAbstraction::FontDescription fontDescription;
    defaultFontId = GetFontClient().GetFontId(fontDescription,
                                              static_cast<TextAbstraction::PointSize26Dot6>(TextAbstraction::FontClient::DEFAULT_POINT_SIZE * GetEffectiveTextScale()));
  }
  else
  {
    defaultFontId = mFontDefaults->GetFontId(GetFontClient(), mFontDefaults->mDefaultPointSize * GetEffectiveTextScale());
  }

  Text::FontMetrics fontMetrics;
  mMetrics->GetFontMetrics(defaultFontId, fontMetrics);

  return (fontMetrics.ascender - fontMetrics.descender);
}

float Controller::Impl::GetDefaultLineBoxHeight()
{
  return GetDefaultLineBoxHeight(GetDefaultFontLineHeight());
}

float Controller::Impl::GetDefaultLineBoxHeight(float defaultFontLineHeight)
{
  // Calculate line box height using the layout engine's line spacing calculation.
  // This ensures consistency with actual text layout.
  const float lineSpacing =
    mLayoutEngine.GetLineSpacing(defaultFontLineHeight, mLayoutEngine.GetRelativeLineSize());

  return defaultFontLineHeight + lineSpacing;
}

bool Controller::Impl::SetDefaultLineSpacing(float lineSpacing)
{
  if(std::fabs(lineSpacing - mLayoutEngine.GetDefaultLineSpacing()) > Math::MACHINE_EPSILON_1000)
  {
    mLayoutEngine.SetDefaultLineSpacing(lineSpacing);

    RelayoutAllCharacters();
    return true;
  }
  return false;
}

void Controller::Impl::RequestDecoratorUpdate()
{
  if(!mEventData)
  {
    return;
  }

  // Only update cursor/decorator positions when in active editing/selecting state.
  // In inactive states, avoid scrolling to the old cursor position.
  if(mEventData->mState == EventData::INACTIVE || mEventData->mState == EventData::INTERRUPTED)
  {
    return;
  }

  // Check if there is an active selection.
  const bool hasSelection =
    EventData::SELECTING == mEventData->mState ||
    mEventData->mLeftSelectionPosition != mEventData->mRightSelectionPosition;

  if(hasSelection)
  {
    // Collapse selection to avoid state mismatch when text geometry changes.
    const CharacterIndex textLength        = static_cast<Dali::Ui::Text::CharacterIndex>(mModel->mLogicalModel->mText.Count());
    const CharacterIndex collapsedPosition = std::min(mEventData->mPrimaryCursorPosition, textLength);
    const uint32_t       oldStart          = mEventData->mLeftSelectionPosition;
    const uint32_t       oldEnd            = mEventData->mRightSelectionPosition;

    ChangeState(EventData::EDITING);

    mEventData->mPrimaryCursorPosition  = collapsedPosition;
    mEventData->mLeftSelectionPosition  = collapsedPosition;
    mEventData->mRightSelectionPosition = collapsedPosition;

    mEventData->mUpdateHighlightBox           = false;
    mEventData->mUpdateLeftSelectionPosition  = false;
    mEventData->mUpdateRightSelectionPosition = false;

    if(mEventData->mDecorator)
    {
      mEventData->mDecorator->SetHighlightActive(false);
    }

    if(mSelectableControlInterface != nullptr &&
       ((oldStart != collapsedPosition) || (oldEnd != collapsedPosition)))
    {
      mSelectableControlInterface->SelectionChanged(oldStart, oldEnd, collapsedPosition, collapsedPosition);
    }
  }

  mEventData->mUpdateCursorPosition      = true;
  mEventData->mUpdateGrabHandlePosition  = true;
  mEventData->mUpdateInputStyle          = true;
  mEventData->mScrollAfterUpdatePosition = true;
  mEventData->mDecoratorUpdated          = true;
}

bool Controller::Impl::SetDefaultLineSize(float lineSize)
{
  if(std::fabs(lineSize - mLayoutEngine.GetDefaultLineSize()) > Math::MACHINE_EPSILON_1000)
  {
    mLayoutEngine.SetDefaultLineSize(lineSize);
    RelayoutAllCharacters();
    RequestDecoratorUpdate();

    return true;
  }
  return false;
}

bool Controller::Impl::SetRelativeLineSize(float relativeLineSize)
{
  if(std::fabs(relativeLineSize - GetRelativeLineSize()) > Math::MACHINE_EPSILON_1000)
  {
    mLayoutEngine.SetRelativeLineSize(relativeLineSize);
    RelayoutAllCharacters();
    RequestDecoratorUpdate();

    return true;
  }
  return false;
}

float Controller::Impl::GetRelativeLineSize()
{
  return mLayoutEngine.GetRelativeLineSize();
}

std::string Controller::Impl::GetSelectedText()
{
  std::string text;
  if(EventData::SELECTING == mEventData->mState)
  {
    RetrieveSelection(text, false);
  }
  return text;
}

std::string Controller::Impl::CopyText()
{
  std::string text;
  RetrieveSelection(text, false);
  SendSelectionToClipboard(false); // Text not modified

  mEventData->mUpdateCursorPosition = true;

  RequestRelayout(); // Cursor, Handles, Selection Highlight, Popup

  return text;
}

std::string Controller::Impl::CutText()
{
  std::string text;
  RetrieveSelection(text, false);

  if(!IsEditable())
  {
    return EMPTY_STRING;
  }

  SendSelectionToClipboard(true); // Synchronous call to modify text
  mOperationsPending = ALL_OPERATIONS;

  if((0u != mModel->mLogicalModel->mText.Count()) || !IsPlaceholderAvailable())
  {
    QueueModifyEvent(ModifyEvent::TEXT_DELETED);
  }
  else
  {
    PlaceholderHandler::ShowPlaceholderText(*this);
  }

  mEventData->mUpdateCursorPosition = true;
  mEventData->mScrollAfterDelete    = true;

  RequestRelayout();

  if(nullptr != mEditableControlInterface)
  {
    mEditableControlInterface->TextChanged(true);
  }
  return text;
}

void Controller::Impl::SetTextSelectionRange(const uint32_t* pStart, const uint32_t* pEnd)
{
  if(nullptr == mEventData)
  {
    // Nothing to do if there is no text.
    return;
  }

  if(mEventData->mSelectionEnabled && (pStart || pEnd))
  {
    uint32_t length   = static_cast<uint32_t>(mModel->mLogicalModel->mText.Count());
    uint32_t oldStart = mEventData->mLeftSelectionPosition;
    uint32_t oldEnd   = mEventData->mRightSelectionPosition;

    if(pStart)
    {
      mEventData->mLeftSelectionPosition = std::min(*pStart, length);
    }
    if(pEnd)
    {
      mEventData->mRightSelectionPosition = std::min(*pEnd, length);
    }

    NormalizeReplacementSelection(mEventData->mLeftSelectionPosition, mEventData->mRightSelectionPosition);

    if(mEventData->mLeftSelectionPosition == mEventData->mRightSelectionPosition)
    {
      ChangeState(EventData::EDITING);
      mEventData->mPrimaryCursorPosition = mEventData->mLeftSelectionPosition = mEventData->mRightSelectionPosition;
      mEventData->mUpdateCursorPosition                                       = true;
    }
    else
    {
      ChangeState(EventData::SELECTING);
      mEventData->mUpdateHighlightBox           = true;
      mEventData->mUpdateLeftSelectionPosition  = true;
      mEventData->mUpdateRightSelectionPosition = true;
    }

    if(mSelectableControlInterface != nullptr)
    {
      mSelectableControlInterface->SelectionChanged(oldStart, oldEnd, mEventData->mLeftSelectionPosition,
                                                    mEventData->mRightSelectionPosition);
    }
  }
}

CharacterIndex Controller::Impl::GetPrimaryCursorPosition() const
{
  if(nullptr == mEventData)
  {
    return 0;
  }
  return mEventData->mPrimaryCursorPosition;
}

bool Controller::Impl::SetPrimaryCursorPosition(CharacterIndex index, bool focused)
{
  if(nullptr == mEventData)
  {
    // Nothing to do if there is no text.
    return false;
  }

  index = NormalizeReplacementBoundary(index, ReplacementEditNormalizer::BoundaryAffinity::LEADING);

  if(mEventData->mPrimaryCursorPosition == index && mEventData->mState != EventData::SELECTING)
  {
    // Nothing for same cursor position.
    return false;
  }

  uint32_t length                    = static_cast<uint32_t>(mModel->mLogicalModel->mText.Count());
  uint32_t oldCursorPos              = mEventData->mPrimaryCursorPosition;
  mEventData->mPrimaryCursorPosition = std::min(index, length);
  // If there is no focus, only the value is updated.
  if(focused)
  {
    bool     wasInSelectingState = mEventData->mState == EventData::SELECTING;
    uint32_t oldStart            = mEventData->mLeftSelectionPosition;
    uint32_t oldEnd              = mEventData->mRightSelectionPosition;
    ChangeState(EventData::EDITING);
    mEventData->mLeftSelectionPosition = mEventData->mRightSelectionPosition = mEventData->mPrimaryCursorPosition;
    mEventData->mUpdateCursorPosition                                        = true;

    if(mSelectableControlInterface != nullptr && wasInSelectingState)
    {
      mSelectableControlInterface->SelectionChanged(oldStart, oldEnd, mEventData->mLeftSelectionPosition,
                                                    mEventData->mRightSelectionPosition);
    }

    ScrollTextToMatchCursor();
  }

  if(nullptr != mEditableControlInterface)
  {
    mEditableControlInterface->CursorPositionChanged(oldCursorPos, mEventData->mPrimaryCursorPosition);
  }

  return true;
}

Ui::Integration::Text::Uint32Pair Controller::Impl::GetTextSelectionRange() const
{
  Ui::Integration::Text::Uint32Pair range;

  if(mEventData)
  {
    range.first  = mEventData->mLeftSelectionPosition;
    range.second = mEventData->mRightSelectionPosition;
  }

  return range;
}

bool Controller::Impl::IsEditable() const
{
  return mEventData && mEventData->mEditingEnabled;
}

void Controller::Impl::SetEditable(bool editable)
{
  if(mEventData)
  {
    mEventData->mEditingEnabled = editable;

    if(mEventData->mDecorator)
    {
      bool decoratorEditable = editable && mIsUserInteractionEnabled;
      mEventData->mDecorator->SetEditable(decoratorEditable);
      mEventData->mDecoratorUpdated = true;
      RequestRelayout();
    }
  }
}

void Controller::Impl::UpdateAfterFontChange(const std::string& newDefaultFont)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Controller::UpdateAfterFontChange\n");

  if(!mFontDefaults->familyDefined) // If user defined font then should not update when system font changes
  {
    DALI_LOG_INFO(gLogFilter, Debug::Concise, "Controller::UpdateAfterFontChange newDefaultFont(%s)\n",
                  newDefaultFont.c_str());
    mFontDefaults->GetFontDescription().family = newDefaultFont;

    ClearFontData();

    RequestRelayout();
  }
}

void Controller::Impl::RetrieveSelection(std::string& selectedText, bool deleteAfterRetrieval)
{
  if(mEventData->mLeftSelectionPosition == mEventData->mRightSelectionPosition)
  {
    // Nothing to select if handles are in the same place.
    selectedText.clear();
    return;
  }

  NormalizeReplacementSelection(mEventData->mLeftSelectionPosition, mEventData->mRightSelectionPosition);

  const bool handlesCrossed = mEventData->mLeftSelectionPosition > mEventData->mRightSelectionPosition;

  // Get start and end position of selection
  const CharacterIndex startOfSelectedText =
    handlesCrossed ? mEventData->mRightSelectionPosition : mEventData->mLeftSelectionPosition;
  const Length lengthOfSelectedText =
    (handlesCrossed ? mEventData->mLeftSelectionPosition : mEventData->mRightSelectionPosition) - startOfSelectedText;

  Vector<Character>& utf32Characters    = mModel->mLogicalModel->mText;
  const Length       numberOfCharacters = static_cast<Dali::Ui::Text::Length>(utf32Characters.Count());

  // Validate the start and end selection points
  if((startOfSelectedText + lengthOfSelectedText) <= numberOfCharacters)
  {
    // Get text as a UTF8 string
    Utf32ToUtf8(&utf32Characters[startOfSelectedText], lengthOfSelectedText, selectedText);

    if(deleteAfterRetrieval) // Only delete text if copied successfully
    {
      // Keep a copy of the current input style.
      Ui::Integration::Text::InputStyle currentInputStyle;
      currentInputStyle.Copy(mEventData->mInputStyle);

      // Set as input style the style of the first deleted character.
      mModel->mLogicalModel->RetrieveStyle(startOfSelectedText, mEventData->mInputStyle);

      // Compare if the input style has changed.
      const bool hasInputStyleChanged = !currentInputStyle.Equal(mEventData->mInputStyle);

      if(hasInputStyleChanged)
      {
        const Ui::Integration::Text::InputStyle::Mask styleChangedMask = currentInputStyle.GetInputStyleChangeMask(mEventData->mInputStyle);
        // Queue the input style changed signal.
        mEventData->mInputStyleChangedQueue.PushBack(styleChangedMask);
      }

      PrepareEditableTextEdit(startOfSelectedText, lengthOfSelectedText, 0u);
      mModel->mLogicalModel->UpdateTextStyleRuns(startOfSelectedText, -static_cast<int>(lengthOfSelectedText));

      // Mark the paragraphs to be updated.
      if(Layout::Engine::SINGLE_LINE_BOX == mLayoutEngine.GetLayout())
      {
        mTextUpdateInfo.mCharacterIndex             = 0;
        mTextUpdateInfo.mNumberOfCharactersToRemove = mTextUpdateInfo.mPreviousNumberOfCharacters;
        mTextUpdateInfo.mNumberOfCharactersToAdd    = mTextUpdateInfo.mPreviousNumberOfCharacters - lengthOfSelectedText;
        mTextUpdateInfo.mClearAll                   = true;
      }
      else
      {
        mTextUpdateInfo.mCharacterIndex             = startOfSelectedText;
        mTextUpdateInfo.mNumberOfCharactersToRemove = lengthOfSelectedText;
      }

      // Delete text between handles
      Vector<Character>::Iterator first = utf32Characters.Begin() + startOfSelectedText;
      Vector<Character>::Iterator last  = first + lengthOfSelectedText;
      utf32Characters.Erase(first, last);

      // Will show the cursor at the first character of the selection.
      mEventData->mPrimaryCursorPosition =
        handlesCrossed ? mEventData->mRightSelectionPosition : mEventData->mLeftSelectionPosition;
    }
    else
    {
      // Will show the cursor at the last character of the selection.
      mEventData->mPrimaryCursorPosition =
        handlesCrossed ? mEventData->mLeftSelectionPosition : mEventData->mRightSelectionPosition;
    }

    mEventData->mDecoratorUpdated = true;
  }
}

void Controller::Impl::SetSelection(int start, int end)
{
  uint32_t oldStart = mEventData->mLeftSelectionPosition;
  uint32_t oldEnd   = mEventData->mRightSelectionPosition;

  CharacterIndex normalizedStart = static_cast<CharacterIndex>(std::max(start, 0));
  CharacterIndex normalizedEnd   = static_cast<CharacterIndex>(std::max(end, 0));
  NormalizeReplacementSelection(normalizedStart, normalizedEnd);
  mEventData->mLeftSelectionPosition  = normalizedStart;
  mEventData->mRightSelectionPosition = normalizedEnd;
  mEventData->mUpdateCursorPosition   = true;

  if(mSelectableControlInterface != nullptr)
  {
    mSelectableControlInterface->SelectionChanged(oldStart, oldEnd, normalizedStart, normalizedEnd);
  }
}

std::pair<int, int> Controller::Impl::GetSelectionIndexes() const
{
  return {mEventData->mLeftSelectionPosition, mEventData->mRightSelectionPosition};
}

void Controller::Impl::ShowClipboard()
{
  if(EnsureClipboardCreated())
  {
    Dali::Integration::Clipboard::ShowClipboard(mClipboard);
  }
}

void Controller::Impl::HideClipboard()
{
  if(EnsureClipboardCreated() && mClipboardHideEnabled)
  {
    Dali::Integration::Clipboard::HideClipboard(mClipboard);
  }
}

void Controller::Impl::SetClipboardHideEnable(bool enable)
{
  mClipboardHideEnabled = enable;
}

bool Controller::Impl::CopyStringToClipboard(const std::string& source)
{
  if(EnsureClipboardCreated())
  {
    Dali::ClipboardData data(MIME_TYPE_TEXT_PLAIN, source.c_str());
    return mClipboard.SetData(data); // Send clipboard data to clipboard.
  }

  return false;
}

bool Controller::Impl::IsClipboardEmpty()
{
  bool result(Dali::Integration::Clipboard::IsAvailable() && EnsureClipboardCreated() &&
              (mClipboard.HasType(MIME_TYPE_TEXT_PLAIN) || mClipboard.HasType(MIME_TYPE_HTML)));
  return !result;
}

void Controller::Impl::SendSelectionToClipboard(bool deleteAfterSending)
{
  std::string selectedText;
  RetrieveSelection(selectedText, deleteAfterSending);
  CopyStringToClipboard(selectedText);
  ChangeState(EventData::EDITING);
}

void Controller::Impl::RepositionSelectionHandles()
{
  SelectionHandleController::Reposition(*this);
}
void Controller::Impl::RepositionSelectionHandles(float visualX, float visualY, Controller::NoTextTap::Action action)
{
  SelectionHandleController::Reposition(*this, visualX, visualY, action);
}

void Controller::Impl::SetPopupButtons()
{
}

void Controller::Impl::ChangeState(EventData::State newState)
{
  ChangeTextControllerState(*this, newState);
}

void Controller::Impl::GetCursorPosition(CharacterIndex logical, CursorInfo& cursorInfo)
{
  if(!IsShowingRealText())
  {
    // Do not want to use the place-holder text to set the cursor position.
    // Empty cursor should be calculated based on real input default font/LineHeight,
    // not placeholder VisualModel's LineRun which might be placeholder layout.

    const float defaultFontLineHeight = GetDefaultFontLineHeight();
    const float lineBoxHeight         = GetDefaultLineBoxHeight();
    const float lineSpacing           = std::max(lineBoxHeight - defaultFontLineHeight, 0.0f);

    cursorInfo.lineOffset          = 0.0f;
    cursorInfo.lineHeight          = defaultFontLineHeight;
    cursorInfo.primaryCursorHeight = defaultFontLineHeight;
    cursorInfo.glyphOffset         = 0.0f;

    // Create a synthetic line for vertical line alignment offset calculation.
    // This ensures cursor height is not affected by placeholder layout.
    LineRun syntheticLine{};
    syntheticLine.ascender    = defaultFontLineHeight;
    syntheticLine.descender   = 0.0f;
    syntheticLine.lineSpacing = lineSpacing;

    const float verticalLineOffset =
      GetPreOffsetVerticalLineAlignment(syntheticLine, mModel->GetVerticalLineAlignment());

    // cursorInfo.primaryPosition.y should only include the line-internal
    // VerticalLineAlignment offset. The block-level VerticalAlignment offset is
    // handled by mScrollPosition.y / CalculateVerticalOffset().
    cursorInfo.primaryPosition.y = verticalLineOffset;

    bool isRTL = false;
    if(mModel->mLayoutDirectionMode != LayoutDirectionMode::CONTENTS)
    {
      isRTL = mLayoutDirection == LayoutDirection::RIGHT_TO_LEFT;
    }

    switch(mModel->mHorizontalAlignment)
    {
      case Alignment::START:
      {
        if(isRTL)
        {
          cursorInfo.primaryPosition.x = mModel->mVisualModel->mControlSize.width - mEventData->mDecorator->GetEffectiveCursorWidth();
        }
        else
        {
          cursorInfo.primaryPosition.x = 0.f;
        }
        break;
      }
      case Alignment::CENTER:
      {
        cursorInfo.primaryPosition.x = floorf(0.5f * mModel->mVisualModel->mControlSize.width);
        break;
      }
      case Alignment::END:
      {
        if(isRTL)
        {
          cursorInfo.primaryPosition.x = 0.f;
        }
        else
        {
          cursorInfo.primaryPosition.x = mModel->mVisualModel->mControlSize.width - mEventData->mDecorator->GetEffectiveCursorWidth();
        }
        break;
      }
    }

    // Nothing else to do.
    return;
  }

  const bool                      isMultiLine   = (Layout::Engine::MULTI_LINE_BOX == mLayoutEngine.GetLayout());
  ModelPtr                        geometryModel = GetEditableGeometryModel();
  const ReplacementRenderState*   replacement   = GetReplacementRenderStatePtr();
  const ReplacementCursorBoundary replacementBoundary =
    replacement && replacement->processingModel && replacement->projection.HasReplacements()
      ? ResolveReplacementCursorBoundary(*replacement, logical)
      : ReplacementCursorBoundary{};
  const ReplacementProjection::BoundaryAffinity projectionAffinity =
    replacementBoundary.placement && !replacementBoundary.leading
      ? ReplacementProjection::BoundaryAffinity::TRAILING
      : ReplacementProjection::BoundaryAffinity::LEADING;
  const CharacterIndex        geometryLogical = LogicalBoundaryToEditable(logical, projectionAffinity);
  GetCursorPositionParameters parameters;
  parameters.visualModel           = geometryModel->mVisualModel;
  parameters.logicalModel          = geometryModel->mLogicalModel;
  parameters.metrics               = mMetrics;
  parameters.logical               = geometryLogical;
  parameters.verticalLineAlignment = geometryModel->GetVerticalLineAlignment();
  parameters.isMultiline           = isMultiLine;

  float defaultFontLineHeight = GetDefaultFontLineHeight();

  Text::GetCursorPosition(parameters, defaultFontLineHeight, cursorInfo);

  if(replacementBoundary.placement)
  {
    ApplyReplacementCaretGeometry(replacementBoundary, cursorInfo);
  }

  // Adds Outline offset.
  const float outlineWidth = mModel->IsOutlineEnabled() ? static_cast<float>(mModel->GetOutlineWidth()) : 0.0f;
  cursorInfo.primaryPosition.x += outlineWidth;
  cursorInfo.primaryPosition.y += outlineWidth;
  cursorInfo.secondaryPosition.x += outlineWidth;
  cursorInfo.secondaryPosition.y += outlineWidth;
  if(cursorInfo.hasPrimaryCaretGeometry)
  {
    cursorInfo.primaryCaretPosition += Vector2(outlineWidth, outlineWidth);
  }
  if(cursorInfo.hasSecondaryCaretGeometry)
  {
    cursorInfo.secondaryCaretPosition += Vector2(outlineWidth, outlineWidth);
  }

  if(isMultiLine && !cursorInfo.hasPrimaryCaretGeometry)
  {
    // If the text is editable and multi-line, the cursor position after a white space shouldn't exceed the boundaries
    // of the text control.

    // Note the white spaces laid-out at the end of the line might exceed the boundaries of the control.
    // The reason is a wrapped line must not start with a white space so they are laid-out at the end of the line.

    if(0.f > cursorInfo.primaryPosition.x)
    {
      cursorInfo.primaryPosition.x = 0.f;
    }

    const float edgeWidth = geometryModel->mVisualModel->mControlSize.width - mEventData->mDecorator->GetEffectiveCursorWidth();
    if(cursorInfo.primaryPosition.x > edgeWidth)
    {
      cursorInfo.primaryPosition.x = edgeWidth;
    }
  }
}

CharacterIndex Controller::Impl::CalculateNewCursorIndex(CharacterIndex index) const
{
  if(nullptr == mEventData)
  {
    // Nothing to do if there is no text input.
    return 0u;
  }

  CharacterIndex cursorIndex  = mEventData->mPrimaryCursorPosition;
  const bool     moveBackward = index < cursorIndex;

  ModelPtr                      geometryModel       = GetEditableGeometryModel();
  CharacterIndex                geometryCursorIndex = cursorIndex;
  CharacterIndex                geometryIndex       = index;
  const ReplacementRenderState* replacement         = GetReplacementRenderStatePtr();
  if(replacement && replacement->processingModel && replacement->projection.HasReplacements())
  {
    geometryCursorIndex = replacement->projection.LogicalBoundaryToProjected(
      cursorIndex,
      moveBackward ? ReplacementProjection::BoundaryAffinity::LEADING
                   : ReplacementProjection::BoundaryAffinity::TRAILING);
    if(moveBackward)
    {
      if(geometryCursorIndex == 0u)
      {
        return 0u;
      }
      geometryIndex = geometryCursorIndex - 1u;
    }
    else
    {
      geometryIndex = geometryCursorIndex;
    }
  }

  const GlyphIndex* const charactersToGlyphBuffer  = geometryModel->mVisualModel->mCharactersToGlyph.Begin();
  const Length* const     charactersPerGlyphBuffer = geometryModel->mVisualModel->mCharactersPerGlyph.Begin();

  GlyphIndex glyphIndex         = *(charactersToGlyphBuffer + geometryIndex);
  Length     numberOfCharacters = *(charactersPerGlyphBuffer + glyphIndex);

  if(numberOfCharacters > 1u)
  {
    const Script script = geometryModel->mLogicalModel->GetScript(geometryIndex);
    if(HasLigatureMustBreak(script))
    {
      if(numberOfCharacters == 2u)
      {
        const Character* const textBuffer = geometryModel->mLogicalModel->mText.Begin();
        Character              character  = *(textBuffer + geometryIndex);

        CharacterIndex nextIndex          = geometryIndex + 1u;
        bool           isCurrentCombining = TextAbstraction::IsCombiningDiacriticalMarks(character);
        bool           isNextValid        = nextIndex < geometryModel->mLogicalModel->mText.Count();
        bool           isNextCombining    = isNextValid && TextAbstraction::IsCombiningDiacriticalMarks(*(textBuffer + nextIndex));

        if(!isCurrentCombining && !isNextCombining)
        {
          numberOfCharacters = 1u;
        }
      }
      else
      {
        // Prevents to jump the whole Latin ligatures like fi, ff, or Arabic ﻻ, ...
        numberOfCharacters = 1u;
      }
    }
  }
  else
  {
    while(0u == numberOfCharacters)
    {
      ++glyphIndex;
      numberOfCharacters = *(charactersPerGlyphBuffer + glyphIndex);
    }
  }

  if(moveBackward)
  {
    geometryCursorIndex = geometryCursorIndex < numberOfCharacters ? 0u : geometryCursorIndex - numberOfCharacters;
  }
  else
  {
    Length textLength   = static_cast<Dali::Ui::Text::Length>(geometryModel->mVisualModel->mCharactersToGlyph.Count());
    geometryCursorIndex = geometryCursorIndex + numberOfCharacters > textLength
                            ? textLength
                            : geometryCursorIndex + numberOfCharacters;
  }

  cursorIndex = replacement && replacement->processingModel && replacement->projection.HasReplacements()
                  ? replacement->projection.ProjectedBoundaryToLogical(geometryCursorIndex)
                  : geometryCursorIndex;

  // Will update the cursor hook position.
  mEventData->mUpdateCursorHookPosition = true;

  return cursorIndex;
}

CharacterIndex Controller::Impl::GetClosestLogicalCursorIndex(float                  visualX,
                                                              float                  visualY,
                                                              CharacterHitTest::Mode hitTest,
                                                              bool&                  matchedCharacter) const
{
  const ReplacementRenderState* replacement = GetReplacementRenderStatePtr();
  if(replacement && replacement->processingModel && replacement->projection.HasReplacements())
  {
    for(const ReplacementPlacement& placement : replacement->placements)
    {
      if(placement.visible && !placement.elided &&
         visualX >= placement.position.x && visualX < placement.position.x + placement.size.x &&
         visualY >= placement.position.y && visualY < placement.position.y + placement.size.y)
      {
        const ProjectedReplacementRun* run = replacement->projection.FindByLogicalCharacter(
          placement.logicalCharacterRange.characterIndex);
        if(run)
        {
          matchedCharacter = true;
          return replacement->projection.HitTestLogicalBoundary(run->projectedCharacterIndex,
                                                                visualX - placement.position.x,
                                                                placement.size.x,
                                                                placement.lineDirection);
        }
      }
    }
  }

  ModelPtr             geometryModel    = GetEditableGeometryModel();
  const CharacterIndex geometryBoundary = Text::GetClosestCursorIndex(geometryModel->mVisualModel,
                                                                      geometryModel->mLogicalModel,
                                                                      mMetrics,
                                                                      visualX,
                                                                      visualY,
                                                                      hitTest,
                                                                      matchedCharacter);
  return EditableBoundaryToLogical(geometryBoundary);
}

void Controller::Impl::UpdateCursorPosition(const CursorInfo& cursorInfo)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->Controller::UpdateCursorPosition %p\n", this);
  if(nullptr == mEventData)
  {
    // Nothing to do if there is no text input.
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::UpdateCursorPosition no event data\n");
    return;
  }

  const Vector2 cursorPosition = cursorInfo.primaryPosition + mModel->mScrollPosition;

  mEventData->mDecorator->SetGlyphOffset(PRIMARY_CURSOR, cursorInfo.glyphOffset);

  // Sets the cursor position.
  mEventData->mDecorator->SetPosition(PRIMARY_CURSOR, cursorPosition.x, cursorPosition.y,
                                      cursorInfo.primaryCursorHeight, cursorInfo.lineHeight);
  if(cursorInfo.hasPrimaryCaretGeometry)
  {
    const Vector2 visualPosition = cursorInfo.primaryCaretPosition + mModel->mScrollPosition;
    mEventData->mDecorator->SetVisualCursorGeometry(PRIMARY_CURSOR,
                                                    visualPosition.x,
                                                    visualPosition.y,
                                                    cursorInfo.primaryCaretHeight);
  }
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Primary cursor position: %f,%f\n", cursorPosition.x, cursorPosition.y);

  if(mEventData->mUpdateGrabHandlePosition)
  {
    // Use the visible text area top, not the line box top.
    const float visibleLineTop = cursorInfo.primaryPosition.y - cursorInfo.glyphOffset;
    // Sets the grab handle position.
    mEventData->mDecorator->SetPosition(GRAB_HANDLE, cursorPosition.x,
                                        visibleLineTop + mModel->mScrollPosition.y, cursorInfo.lineHeight);
  }

  if(cursorInfo.isSecondaryCursor)
  {
    const Vector2 secondaryPosition = cursorInfo.secondaryPosition + mModel->mScrollPosition;
    mEventData->mDecorator->SetPosition(SECONDARY_CURSOR,
                                        secondaryPosition.x,
                                        secondaryPosition.y,
                                        cursorInfo.secondaryCursorHeight,
                                        cursorInfo.lineHeight);
    if(cursorInfo.hasSecondaryCaretGeometry)
    {
      const Vector2 visualPosition = cursorInfo.secondaryCaretPosition + mModel->mScrollPosition;
      mEventData->mDecorator->SetVisualCursorGeometry(SECONDARY_CURSOR,
                                                      visualPosition.x,
                                                      visualPosition.y,
                                                      cursorInfo.secondaryCaretHeight);
    }
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Secondary cursor position: %f,%f\n",
                  secondaryPosition.x,
                  secondaryPosition.y);
  }

  // Set which cursors are active according the state.
  if(EventData::IsEditingState(mEventData->mState) || (EventData::GRAB_HANDLE_PANNING == mEventData->mState))
  {
    if(cursorInfo.isSecondaryCursor)
    {
      mEventData->mDecorator->SetActiveCursor(ACTIVE_CURSOR_BOTH);
    }
    else
    {
      mEventData->mDecorator->SetActiveCursor(ACTIVE_CURSOR_PRIMARY);
    }
  }
  else
  {
    mEventData->mDecorator->SetActiveCursor(ACTIVE_CURSOR_NONE);
  }

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::UpdateCursorPosition\n");
}

void Controller::Impl::UpdateSelectionHandle(HandleType handleType, const CursorInfo& cursorInfo)
{
  SelectionHandleController::Update(*this, handleType, cursorInfo);
}

void Controller::Impl::ClampHorizontalScroll(const Vector2& layoutSize)
{
  // Clamp between -space & -alignment offset.

  if(layoutSize.width > mModel->mVisualModel->mControlSize.width)
  {
    const float space         = (layoutSize.width - mModel->mVisualModel->mControlSize.width) + mModel->mAlignmentOffset;
    mModel->mScrollPosition.x = (mModel->mScrollPosition.x < -space) ? -space : mModel->mScrollPosition.x;
    mModel->mScrollPosition.x =
      (mModel->mScrollPosition.x > -mModel->mAlignmentOffset) ? -mModel->mAlignmentOffset : mModel->mScrollPosition.x;

    mEventData->mDecoratorUpdated = true;
  }
  else
  {
    mModel->mScrollPosition.x = 0.f;
  }
  SyncReplacementScrollPosition();
}

void Controller::Impl::ClampVerticalScroll(const Vector2& layoutSize)
{
  if(Layout::Engine::SINGLE_LINE_BOX == mLayoutEngine.GetLayout())
  {
    // Nothing to do if the text is single line.
    SyncReplacementScrollPosition();
    return;
  }

  // Clamp between -space & 0.
  if(layoutSize.height > mModel->mVisualModel->mControlSize.height)
  {
    const float space         = (layoutSize.height - mModel->mVisualModel->mControlSize.height);
    mModel->mScrollPosition.y = (mModel->mScrollPosition.y < -space) ? -space : mModel->mScrollPosition.y;
    mModel->mScrollPosition.y = (mModel->mScrollPosition.y > 0.f) ? 0.f : mModel->mScrollPosition.y;

    mEventData->mDecoratorUpdated = true;
  }
  else
  {
    // Content is smaller than or equal to control size.
    // Recalculate vertical offset to preserve VerticalAlignment (CENTER/END).
    Relayouter::CalculateVerticalOffset(*this, mModel->mVisualModel->mControlSize);
  }
  SyncReplacementScrollPosition();
}

void Controller::Impl::ScrollToMakePositionVisible(const Vector2& position, float lineHeight)
{
  const float cursorWidth = mEventData->mDecorator ? mEventData->mDecorator->GetEffectiveCursorWidth() : 0.0f;

  // position is in actor's coords.
  const float positionEndX = position.x + cursorWidth;
  const float positionEndY = position.y + lineHeight;

  // Transform the position to decorator coords.
  const float decoratorPositionBeginX = position.x + mModel->mScrollPosition.x;
  const float decoratorPositionEndX   = positionEndX + mModel->mScrollPosition.x;

  const float decoratorPositionBeginY = position.y + mModel->mScrollPosition.y;
  const float decoratorPositionEndY   = positionEndY + mModel->mScrollPosition.y;

  if(decoratorPositionBeginX < 0.f)
  {
    mModel->mScrollPosition.x = -position.x;
  }
  else if(decoratorPositionEndX > mModel->mVisualModel->mControlSize.width)
  {
    mModel->mScrollPosition.x = mModel->mVisualModel->mControlSize.width - positionEndX;
  }

  if(Layout::Engine::MULTI_LINE_BOX == mLayoutEngine.GetLayout())
  {
    if(decoratorPositionBeginY < 0.f)
    {
      mModel->mScrollPosition.y = -position.y;
    }
    else if(decoratorPositionEndY > mModel->mVisualModel->mControlSize.height)
    {
      mModel->mScrollPosition.y = mModel->mVisualModel->mControlSize.height - positionEndY;
    }
    else if(mModel->mLogicalModel->mText.Count() == 0u)
    {
      Relayouter::CalculateVerticalOffset(*this, mModel->mVisualModel->mControlSize);
    }

    // Clamp the scroll position to valid bounds after adjustment.
    ClampVerticalScroll(GetEditableGeometryModel()->mVisualModel->GetLayoutSize());
  }
  SyncReplacementScrollPosition();
}

std::pair<float, float> Controller::Impl::CalculateScrollTarget(const CursorInfo& info) const
{
  float visibleTop    = info.primaryPosition.y - info.glyphOffset;
  float visibleBottom = visibleTop + info.lineHeight;

  ModelPtr     geometryModel = GetEditableGeometryModel();
  const Length lineCount     = static_cast<Dali::Ui::Text::Length>(geometryModel->mVisualModel->mLines.Count());
  if(lineCount > 0u)
  {
    // Find the line index by comparing line offsets with cursor's lineOffset.
    // This is more accurate than using cursorPosition, especially when cursor is at line boundary
    // or on a new empty line (e.g., after Enter key).
    LineIndex lineIndex   = 0u;
    float     minDistance = std::numeric_limits<float>::max();

    for(LineIndex index = 0u; index < lineCount; ++index)
    {
      const float lineOffset = CalculateLineOffset(geometryModel->mVisualModel->mLines, index);
      const float distance   = std::fabs(lineOffset - info.lineOffset);

      if(distance < minDistance)
      {
        minDistance = distance;
        lineIndex   = index;
      }
    }

    const bool isFirstLine = (lineIndex == 0u);
    const bool isLastLine  = (lineIndex + 1u >= lineCount);

    if(isFirstLine || isLastLine)
    {
      const LineRun&  line              = *(geometryModel->mVisualModel->mLines.Begin() + lineIndex);
      const float     naturalLineHeight = line.ascender - line.descender;
      const Alignment verticalLineAlign = geometryModel->GetVerticalLineAlignment();
      const float     lineBoxHeight =
        GetPreOffsetVerticalLineAlignment(line, verticalLineAlign) +
        naturalLineHeight +
        GetPostOffsetVerticalLineAlignment(line, verticalLineAlign);

      const float lineBoxTop    = info.lineOffset;
      const float lineBoxBottom = lineBoxTop + lineBoxHeight;

      if(isFirstLine)
      {
        visibleTop = lineBoxTop;
      }
      if(isLastLine)
      {
        visibleBottom = std::max(visibleBottom, lineBoxBottom);
      }
    }
  }

  return {visibleTop, visibleBottom};
}

void Controller::Impl::ScrollTextToMatchCursor(const CursorInfo& cursorInfo)
{
  // Get the current cursor position in decorator coords.
  const Vector2& currentCursorPosition = mEventData->mDecorator->GetPosition(PRIMARY_CURSOR);

  ModelPtr       geometryModel  = GetEditableGeometryModel();
  CharacterIndex geometryCursor = LogicalBoundaryToEditable(
    mEventData->mPrimaryCursorPosition,
    ReplacementProjection::BoundaryAffinity::LEADING);
  const CharacterIndex characterIndex = geometryCursor > 0u ? geometryCursor - 1u : 0u;
  const LineIndex      lineIndex      = geometryModel->mVisualModel->GetLineOfCharacter(characterIndex);

  // Calculate the offset to match the cursor position before the character was deleted.
  mModel->mScrollPosition.x = currentCursorPosition.x - cursorInfo.primaryPosition.x;

  // If text control has more than two lines and current line index is not last, calculate scrollpositionY
  if(geometryModel->mVisualModel->mLines.Count() > 1u && lineIndex != geometryModel->mVisualModel->mLines.Count() - 1u)
  {
    const float currentCursorGlyphOffset = mEventData->mDecorator->GetGlyphOffset(PRIMARY_CURSOR);
    mModel->mScrollPosition.y            = currentCursorPosition.y - cursorInfo.lineOffset - currentCursorGlyphOffset;
  }

  ClampHorizontalScroll(geometryModel->mVisualModel->GetLayoutSize());
  ClampVerticalScroll(geometryModel->mVisualModel->GetLayoutSize());

  // Makes the new cursor position visible if needed, using LineHeight-aware scroll target.
  auto [visibleTop, visibleBottom] = CalculateScrollTarget(cursorInfo);
  const Vector2 scrollTargetPosition(cursorInfo.primaryPosition.x, visibleTop);
  ScrollToMakePositionVisible(scrollTargetPosition, visibleBottom - visibleTop);
}

void Controller::Impl::ScrollTextToMatchCursor()
{
  CursorInfo cursorInfo;
  GetCursorPosition(mEventData->mPrimaryCursorPosition, cursorInfo);
  ScrollTextToMatchCursor(cursorInfo);
}

void Controller::Impl::RequestRelayout()
{
  if(nullptr != mControlInterface)
  {
    mControlInterface->RequestTextRelayout();
  }
}

void Controller::Impl::InvalidateMeasure()
{
  if(nullptr != mControlInterface)
  {
    mControlInterface->InvalidateTextMeasure();
  }
}

void Controller::Impl::RequestAsyncRender()
{
  if(nullptr != mControlInterface)
  {
    mControlInterface->RequestAsyncRender();
  }
}

void Controller::Impl::RelayoutAllCharacters()
{
  // relayout all characters
  mTextUpdateInfo.mCharacterIndex             = 0;
  mTextUpdateInfo.mNumberOfCharactersToRemove = mTextUpdateInfo.mPreviousNumberOfCharacters;
  mTextUpdateInfo.mNumberOfCharactersToAdd    = static_cast<Dali::Ui::Text::Length>(mModel->mLogicalModel->mText.Count());
  mOperationsPending                          = static_cast<OperationsMask>(mOperationsPending | LAYOUT);

  mTextUpdateInfo.mFullRelayoutNeeded = true;

  // Need to recalculate natural size
  mRecalculateNaturalSize    = true;
  mRecalculateLayoutSize     = true;
  mRecalculateHeightForWidth = true;

  // remove selection
  if((mEventData != nullptr) && (mEventData->mState == EventData::SELECTING))
  {
    ChangeState(EventData::EDITING);
  }

  RequestRelayout();
}

bool Controller::Impl::IsInputStyleChangedSignalsQueueEmpty()
{
  return (nullptr == mEventData) || (0u == mEventData->mInputStyleChangedQueue.Count());
}

void Controller::Impl::ProcessInputStyleChangedSignals()
{
  if(mEventData)
  {
    if(mEditableControlInterface)
    {
      // Emit the input style changed signal for each mask
      std::for_each(mEventData->mInputStyleChangedQueue.begin(), mEventData->mInputStyleChangedQueue.end(),
                    [&](const auto mask)
      { mEditableControlInterface->InputStyleChanged(mask); });
    }

    mEventData->mInputStyleChangedQueue.Clear();
  }
}

void Controller::Impl::ScrollBy(Vector2 scroll)
{
  if(mEventData && (fabs(scroll.x) > Math::MACHINE_EPSILON_0 || fabs(scroll.y) > Math::MACHINE_EPSILON_0))
  {
    const Vector2& layoutSize    = GetEditableGeometryModel()->mVisualModel->GetLayoutSize();
    const Vector2  currentScroll = mModel->mScrollPosition;

    scroll.x = -scroll.x;
    scroll.y = -scroll.y;

    if(fabs(scroll.x) > Math::MACHINE_EPSILON_0)
    {
      mModel->mScrollPosition.x += scroll.x;
      ClampHorizontalScroll(layoutSize);
    }

    if(fabs(scroll.y) > Math::MACHINE_EPSILON_0)
    {
      mModel->mScrollPosition.y += scroll.y;
      ClampVerticalScroll(layoutSize);
    }

    if(mModel->mScrollPosition != currentScroll)
    {
      mEventData->mDecorator->UpdatePositions(mModel->mScrollPosition - currentScroll);
      RequestRelayout();
    }
  }
}

bool Controller::Impl::IsScrollable(const Vector2& displacement)
{
  bool isScrollable = false;
  if(mEventData)
  {
    const bool isHorizontalScrollEnabled = mEventData->mDecorator->IsHorizontalScrollEnabled();
    const bool isVerticalScrollEnabled   = mEventData->mDecorator->IsVerticalScrollEnabled();
    if(isHorizontalScrollEnabled || isVerticalScrollEnabled)
    {
      ModelPtr       geometryModel = GetEditableGeometryModel();
      const Vector2& targetSize    = geometryModel->mVisualModel->mControlSize;
      const Vector2& layoutSize    = geometryModel->mVisualModel->GetLayoutSize();

      if(isHorizontalScrollEnabled)
      {
        const float scrollPositionX = std::max(mModel->mScrollPosition.x, -(layoutSize.width - targetSize.width));
        const float positionX       = scrollPositionX + displacement.x;
        if(layoutSize.width > targetSize.width && -positionX > 0.f && -positionX < layoutSize.width - targetSize.width)
        {
          isScrollable = true;
        }
      }

      if(isVerticalScrollEnabled)
      {
        const float scrollPositionY = std::max(mModel->mScrollPosition.y, -(layoutSize.height - targetSize.height));
        const float positionY       = scrollPositionY + displacement.y;
        if(layoutSize.height > targetSize.height && -positionY > 0.f &&
           -positionY < layoutSize.height - targetSize.height)
        {
          isScrollable = true;
        }
      }
    }
  }
  return isScrollable;
}

float Controller::Impl::GetHorizontalScrollPosition()
{
  // Scroll values are negative internally so we convert them to positive numbers
  return mEventData ? -mModel->mScrollPosition.x : 0.0f;
}

float Controller::Impl::GetVerticalScrollPosition()
{
  // Scroll values are negative internally so we convert them to positive numbers
  return mEventData ? -mModel->mScrollPosition.y : 0.0f;
}

Ui::TextAnchor Controller::Impl::CreateAnchorActor(Anchor anchor)
{
  auto actor = Ui::TextAnchor::New();
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);

  ModelPtr                      geometryModel = mModel;
  CharacterIndex                geometryStart = anchor.startIndex;
  CharacterIndex                geometryEnd   = anchor.endIndex;
  const ReplacementRenderState& replacement   = GetReplacementRenderState();
  if(replacement.processingModel && replacement.projection.HasReplacements())
  {
    geometryModel = replacement.processingModel;
    geometryStart = replacement.projection.LogicalBoundaryToProjected(
      anchor.startIndex,
      ReplacementProjection::BoundaryAffinity::LEADING);
    geometryEnd = replacement.projection.LogicalBoundaryToProjected(
      anchor.endIndex,
      ReplacementProjection::BoundaryAffinity::TRAILING);
  }

  Bounds rect;
  if(geometryEnd > geometryStart)
  {
    rect = Ui::Internal::CommonTextUtils::GetTextBoundingRectangle(geometryModel, geometryStart, geometryEnd - 1u);
  }
  Vector2 offset = mModel->mLayoutOffsetWithPadding;
  actor.SetProperty(Actor::Property::POSITION, Vector2(rect.x + offset.x, rect.y + offset.y));
  actor.SetProperty(Actor::Property::SIZE, Vector2(rect.width, rect.height));

  std::string anchorText;
  std::string anchorHref               = anchor.href ? anchor.href : "";
  Length      numberOfAnchorCharacters = anchor.endIndex - anchor.startIndex;
  if(numberOfAnchorCharacters > 0u && mModel->mLogicalModel->mText.Size() >= numberOfAnchorCharacters)
  {
    Utf32ToUtf8(mModel->mLogicalModel->mText.Begin() + anchor.startIndex, numberOfAnchorCharacters, anchorText);
  }
  DALI_LOG_INFO(gLogFilter, Debug::General, "CreateAnchorActor NAME:%s, URI:%s\n", anchorText.c_str(),
                anchorHref.c_str());

  actor.SetProperty(Actor::Property::NAME, ToPropertyValue(anchorText));
  actor.SetProperty(Ui::TextAnchor::Property::URI, ToPropertyValue(anchorHref));
  actor.SetProperty(Ui::TextAnchor::Property::START_CHARACTER_INDEX, static_cast<int>(anchor.startIndex));
  actor.SetProperty(Ui::TextAnchor::Property::END_CHARACTER_INDEX, static_cast<int>(anchor.endIndex));
  return actor;
}

void Controller::Impl::GetAnchorActors(std::vector<Ui::TextAnchor>& anchorActors)
{
  /* TODO: Now actors are created/destroyed in every "RenderText" function call. Even when we add just 1 character,
           we need to create and destroy potentially many actors. Some optimization can be considered here.
           Maybe a "dirty" flag in mLogicalModel? */
  anchorActors.clear();
  for(auto& anchor : mModel->mLogicalModel->mAnchors)
  {
    auto actor = CreateAnchorActor(anchor);
    anchorActors.push_back(actor);
  }
}

int32_t Controller::Impl::GetAnchorIndex(size_t characterOffset) const
{
  Vector<Anchor>::Iterator it = mModel->mLogicalModel->mAnchors.Begin();

  while(it != mModel->mLogicalModel->mAnchors.End() &&
        (it->startIndex > characterOffset || it->endIndex <= characterOffset))
  {
    it++;
  }

  return it == mModel->mLogicalModel->mAnchors.End() ? -1 : static_cast<int32_t>(it - mModel->mLogicalModel->mAnchors.Begin());
}

bool Controller::Impl::ShouldClearFocusOnEscape() const
{
  if(DALI_UNLIKELY(mShouldClearFocusOnEscape == ClearFocusOnEscapeState::UNKNOWN))
  {
    if(UiConfig::HasCurrent())
    {
      mShouldClearFocusOnEscape = UiConfig::GetCurrent().IsClearFocusOnEscapeEnabled()
                                    ? ClearFocusOnEscapeState::ENABLE
                                    : ClearFocusOnEscapeState::DISABLE;
    }
  }
  DALI_ASSERT_DEBUG(mShouldClearFocusOnEscape != ClearFocusOnEscapeState::UNKNOWN &&
                    "mShouldClearFocusOnEscape Should be set now");

  return (mShouldClearFocusOnEscape == ClearFocusOnEscapeState::ENABLE);
}

void Controller::Impl::CopyUnderlinedFromLogicalToVisualModels(bool shouldClearPreUnderlineRuns)
{
  // Underlined character runs for markup-processor
  const Vector<UnderlinedCharacterRun>& underlinedCharacterRuns = mModel->mLogicalModel->mUnderlinedCharacterRuns;
  const Vector<GlyphIndex>&             charactersToGlyph       = mModel->mVisualModel->mCharactersToGlyph;
  const Vector<Length>&                 glyphsPerCharacter      = mModel->mVisualModel->mGlyphsPerCharacter;

  if(shouldClearPreUnderlineRuns)
  {
    mModel->mVisualModel->mUnderlineRuns.Clear();
  }

  for(Vector<UnderlinedCharacterRun>::ConstIterator it    = underlinedCharacterRuns.Begin(),
                                                    endIt = underlinedCharacterRuns.End();
      it != endIt; ++it)
  {
    CharacterIndex characterIndex     = it->characterRun.characterIndex;
    Length         numberOfCharacters = it->characterRun.numberOfCharacters;

    if(numberOfCharacters == 0)
    {
      continue;
    }

    // Create one run for all glyphs of all run's characters that has same properties
    // This enhance performance and reduce the needed memory to store glyphs-runs
    UnderlinedGlyphRun underlineGlyphRun;
    underlineGlyphRun.glyphRun.glyphIndex     = charactersToGlyph[characterIndex];
    underlineGlyphRun.glyphRun.numberOfGlyphs = glyphsPerCharacter[characterIndex];
    // Copy properties (attributes)
    underlineGlyphRun.properties = it->properties;

    for(Length index = 1u; index < numberOfCharacters; index++)
    {
      underlineGlyphRun.glyphRun.numberOfGlyphs += glyphsPerCharacter[characterIndex + index];
    }

    mModel->mVisualModel->mUnderlineRuns.PushBack(underlineGlyphRun);
  }
}

void Controller::Impl::CopyStrikethroughFromLogicalToVisualModels()
{
  // Strikethrough character runs from markup-processor
  const Vector<StrikethroughCharacterRun>& strikethroughCharacterRuns =
    mModel->mLogicalModel->mStrikethroughCharacterRuns;
  const Vector<GlyphIndex>& charactersToGlyph  = mModel->mVisualModel->mCharactersToGlyph;
  const Vector<Length>&     glyphsPerCharacter = mModel->mVisualModel->mGlyphsPerCharacter;

  mModel->mVisualModel->mStrikethroughRuns.Clear();

  for(Vector<StrikethroughCharacterRun>::ConstIterator it    = strikethroughCharacterRuns.Begin(),
                                                       endIt = strikethroughCharacterRuns.End();
      it != endIt; ++it)
  {
    CharacterIndex characterIndex     = it->characterRun.characterIndex;
    Length         numberOfCharacters = it->characterRun.numberOfCharacters;

    if(numberOfCharacters == 0)
    {
      continue;
    }

    StrikethroughGlyphRun strikethroughGlyphRun;
    strikethroughGlyphRun.properties              = it->properties;
    strikethroughGlyphRun.glyphRun.glyphIndex     = charactersToGlyph[characterIndex];
    strikethroughGlyphRun.glyphRun.numberOfGlyphs = glyphsPerCharacter[characterIndex];

    for(Length index = 1u; index < numberOfCharacters; index++)
    {
      strikethroughGlyphRun.glyphRun.numberOfGlyphs += glyphsPerCharacter[characterIndex + index];
    }

    mModel->mVisualModel->mStrikethroughRuns.PushBack(strikethroughGlyphRun);
  }
}

void Controller::Impl::CopyCharacterSpacingFromLogicalToVisualModels()
{
  // CharacterSpacing character runs from markup-processor
  const Vector<CharacterSpacingCharacterRun>& characterSpacingCharacterRuns =
    mModel->mLogicalModel->mCharacterSpacingCharacterRuns;
  const Vector<GlyphIndex>& charactersToGlyph  = mModel->mVisualModel->mCharactersToGlyph;
  const Vector<Length>&     glyphsPerCharacter = mModel->mVisualModel->mGlyphsPerCharacter;

  mModel->mVisualModel->mCharacterSpacingRuns.Clear();

  for(Vector<CharacterSpacingCharacterRun>::ConstIterator it    = characterSpacingCharacterRuns.Begin(),
                                                          endIt = characterSpacingCharacterRuns.End();
      it != endIt; ++it)
  {
    const CharacterIndex& characterIndex     = it->characterRun.characterIndex;
    const Length&         numberOfCharacters = it->characterRun.numberOfCharacters;

    if(numberOfCharacters == 0)
    {
      continue;
    }

    CharacterSpacingGlyphRun characterSpacingGlyphRun;
    characterSpacingGlyphRun.value                   = it->value;
    characterSpacingGlyphRun.glyphRun.glyphIndex     = charactersToGlyph[characterIndex];
    characterSpacingGlyphRun.glyphRun.numberOfGlyphs = glyphsPerCharacter[characterIndex];

    for(Length index = 1u; index < numberOfCharacters; index++)
    {
      characterSpacingGlyphRun.glyphRun.numberOfGlyphs += glyphsPerCharacter[characterIndex + index];
    }

    mModel->mVisualModel->mCharacterSpacingRuns.PushBack(characterSpacingGlyphRun);
  }
}

void Controller::Impl::SetMarqueeEnabled(bool enable, bool requestRelayout, MarqueeOrientation orientation)
{
  if((mLayoutEngine.GetLayout() == Layout::Engine::SINGLE_LINE_BOX && orientation == MarqueeOrientation::HORIZONTAL) ||
     (mLayoutEngine.GetLayout() == Layout::Engine::MULTI_LINE_BOX && orientation == MarqueeOrientation::VERTICAL))
  {
    mOperationsPending = static_cast<OperationsMask>(mOperationsPending | LAYOUT | ALIGN | UPDATE_LAYOUT_SIZE | REORDER);

    if(enable)
    {
      DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::SetMarqueeEnabled\n");
      mOperationsPending = static_cast<OperationsMask>(mOperationsPending | UPDATE_DIRECTION);
    }
    else
    {
      DALI_LOG_INFO(gLogFilter, Debug::General, "Controller::SetMarqueeEnabled Disabling marquee\n");
    }

    mIsMarqueeEnabled = enable;
    if(requestRelayout)
    {
      RequestRelayout();
    }
  }
  else
  {
    DALI_LOG_DEBUG_INFO("Attempted Marqueeing, request ignored\n");
    mIsMarqueeEnabled = false;
  }
}

void Controller::Impl::SetEnableCursorBlink(bool enable)
{
  DALI_ASSERT_DEBUG(nullptr != mEventData && "TextInput disabled");

  if(mEventData)
  {
    mEventData->mCursorBlinkEnabled = enable;
    if(mEventData->mDecorator)
    {
      if(enable)
      {
        switch(mEventData->mState)
        {
          case EventData::EDITING:
          case EventData::EDITING_WITH_POPUP:
          case EventData::EDITING_WITH_GRAB_HANDLE:
          case EventData::GRAB_HANDLE_PANNING:
          case EventData::EDITING_WITH_PASTE_POPUP:
          {
            mEventData->mDecorator->StartCursorBlink();
            break;
          }
          default:
          {
            break;
          }
        }
      }
      else
      {
        mEventData->mDecorator->StopCursorBlink();
      }
    }
  }
}

void Controller::Impl::SetMultiLineEnabled(bool enable)
{
  const Layout::Engine::Type layout = enable ? Layout::Engine::MULTI_LINE_BOX : Layout::Engine::SINGLE_LINE_BOX;

  if(layout != mLayoutEngine.GetLayout())
  {
    // Set the layout type.
    mLayoutEngine.SetLayout(layout);

    // Set the flags to redo the layout operations
    const OperationsMask layoutOperations = static_cast<OperationsMask>(LAYOUT | UPDATE_LAYOUT_SIZE | ALIGN | REORDER);

    mTextUpdateInfo.mFullRelayoutNeeded = true;
    mOperationsPending                  = static_cast<OperationsMask>(mOperationsPending | layoutOperations);

    // Need to recalculate natural size
    mRecalculateNaturalSize    = true;
    mRecalculateLayoutSize     = true;
    mRecalculateHeightForWidth = true;

    RequestRelayout();
    RequestAsyncRender();
  }
}

void Controller::Impl::SetHorizontalAlignment(Alignment alignment)
{
  if(alignment != mModel->mHorizontalAlignment)
  {
    // Set the alignment.
    mModel->mHorizontalAlignment = alignment;
    UpdateCursorPositionForAlignment(*this, true);
    RequestRelayout();
    RequestAsyncRender();
  }
}

void Controller::Impl::SetVerticalAlignment(Alignment alignment)
{
  if(alignment != mModel->mVerticalAlignment)
  {
    // Set the alignment.
    mModel->mVerticalAlignment = alignment;
    UpdateCursorPositionForAlignment(*this, false);
    RequestRelayout();
    RequestAsyncRender();
  }
}

void Controller::Impl::SetLineWrapMode(LineWrapMode lineWrapMode)
{
  if(lineWrapMode != mModel->mLineWrapMode)
  {
    // Update Text layout for applying wrap mode
    mOperationsPending =
      static_cast<OperationsMask>(mOperationsPending | ALIGN | LAYOUT | UPDATE_LAYOUT_SIZE | REORDER);

    if((mModel->mLineWrapMode == LineWrapMode::HYPHENATION) ||
       (lineWrapMode == LineWrapMode::HYPHENATION) ||
       (mModel->mLineWrapMode == LineWrapMode::MIXED) ||
       (lineWrapMode == LineWrapMode::MIXED)) // hyphen is treated as line break
    {
      mOperationsPending = static_cast<OperationsMask>(mOperationsPending | GET_LINE_BREAKS);
    }

    // Set the text wrap mode.
    mModel->mLineWrapMode = lineWrapMode;

    mTextUpdateInfo.mCharacterIndex             = 0u;
    mTextUpdateInfo.mNumberOfCharactersToRemove = mTextUpdateInfo.mPreviousNumberOfCharacters;
    mTextUpdateInfo.mNumberOfCharactersToAdd    = static_cast<Dali::Ui::Text::Length>(mModel->mLogicalModel->mText.Count());

    // Request relayout
    RequestRelayout();
    RequestAsyncRender();
  }
}

void Controller::Impl::SetDefaultColor(const Vector4& color)
{
  mTextColor = color;

  if(!IsShowingPlaceholderText())
  {
    mModel->mVisualModel->SetTextColor(color);
    mOperationsPending = static_cast<OperationsMask>(mOperationsPending | COLOR);
    RequestRelayout();
  }
}

void Controller::Impl::SetUserInteractionEnabled(bool enabled)
{
  mIsUserInteractionEnabled = enabled;

  if(mEventData && mEventData->mDecorator)
  {
    bool editable = mEventData->mEditingEnabled && enabled;
    mEventData->mDecorator->SetEditable(editable);
    mEventData->mDecoratorUpdated = true;
    RequestRelayout();
  }
}

float Controller::Impl::GetEffectiveTextScale() const
{
  return mAdjustedFontSizeScale * mUiScale;
}

bool Controller::Impl::SetUiScale(float scale)
{
  if(scale <= 0.0f)
  {
    return false;
  }

  if(Dali::Equals(mUiScale, scale, Math::MACHINE_EPSILON_1000))
  {
    return false;
  }

  mUiScale = scale;
  mLayoutEngine.SetFontSizeScale(GetEffectiveTextScale());
  return true;
}

float Controller::Impl::GetUiScale() const
{
  return mUiScale;
}

void Controller::Impl::SetFontSizeScale(float scale)
{
  if(Dali::Equals(mFontSizeScale, scale) && !mSystemFontSizeScaleEnabled)
  {
    return;
  }

  mFontSizeScale              = scale;
  mSystemFontSizeScaleEnabled = false;

  ApplyAdjustedFontSizeScale();
}

float Controller::Impl::GetFontSizeScale() const
{
  return mFontSizeScale;
}

void Controller::Impl::SetMinimumFontSizeScale(float scale)
{
  if(Dali::Equals(mMinFontSizeScale, scale))
  {
    return;
  }

  mMinFontSizeScale = scale;
  ApplyAdjustedFontSizeScale();
}

float Controller::Impl::GetMinimumFontSizeScale() const
{
  return mMinFontSizeScale;
}

void Controller::Impl::SetMaximumFontSizeScale(float scale)
{
  if(Dali::Equals(mMaxFontSizeScale, scale))
  {
    return;
  }

  mMaxFontSizeScale = scale;
  ApplyAdjustedFontSizeScale();
}

float Controller::Impl::GetMaximumFontSizeScale() const
{
  return mMaxFontSizeScale;
}

void Controller::Impl::SetSystemFontSizeScaleEnabled(bool enabled)
{
  if(mSystemFontSizeScaleEnabled == enabled)
  {
    return;
  }

  mSystemFontSizeScaleEnabled = enabled;
  ApplyAdjustedFontSizeScale();
}

bool Controller::Impl::IsSystemFontSizeScaleEnabled() const
{
  return mSystemFontSizeScaleEnabled;
}

// TODO: Update this from the system font size changed callback.
void Controller::Impl::SetSystemFontSizeScale(float scale)
{
  if(Dali::Equals(mSystemFontSizeScale, scale))
  {
    return;
  }

  mSystemFontSizeScale = scale;
  if(mSystemFontSizeScaleEnabled)
  {
    ApplyAdjustedFontSizeScale();
  }
}

float Controller::Impl::GetAdjustedFontSizeScale() const
{
  return mAdjustedFontSizeScale;
}

float Controller::Impl::GetSystemFontSizeScale() const
{
  return mSystemFontSizeScale;
}

float Controller::Impl::GetCurrentFontSizeScale() const
{
  return mSystemFontSizeScaleEnabled ? GetSystemFontSizeScale() : mFontSizeScale;
}

float Controller::Impl::CalculateAdjustedFontSizeScale() const
{
  float minimumScale = mMinFontSizeScale;
  float maximumScale = mMaxFontSizeScale;

  if(minimumScale > maximumScale)
  {
    maximumScale = minimumScale;
  }

  const float currentScale = GetCurrentFontSizeScale();

  return std::clamp(currentScale, minimumScale, maximumScale);
}

bool Controller::Impl::ApplyAdjustedFontSizeScale()
{
  const float adjustedScale = CalculateAdjustedFontSizeScale();
  if(Dali::Equals(mAdjustedFontSizeScale, adjustedScale))
  {
    return false;
  }

  mAdjustedFontSizeScale = adjustedScale;
  mLayoutEngine.SetFontSizeScale(GetEffectiveTextScale());

  if(mEventData && EventData::IsEditingState(mEventData->mState))
  {
    mEventData->mDecoratorUpdated     = true;
    mEventData->mUpdateCursorPosition = true; // Cursor position should be updated when the font size is updated.
  }

  ClearFontData();
  RequestRelayout();
  RequestAsyncRender();
  InvalidateMeasure();

  return true;
}

void Controller::Impl::ClearFontData()
{
  if(mFontDefaults)
  {
    mFontDefaults->mFontId = 0u; // Remove old font ID
  }

  // Set flags to update the model.
  mTextUpdateInfo.mCharacterIndex             = 0u;
  mTextUpdateInfo.mNumberOfCharactersToRemove = mTextUpdateInfo.mPreviousNumberOfCharacters;
  mTextUpdateInfo.mNumberOfCharactersToAdd    = static_cast<Dali::Ui::Text::Length>(mModel->mLogicalModel->mText.Count());

  mTextUpdateInfo.mClearAll           = true;
  mTextUpdateInfo.mFullRelayoutNeeded = true;
  mRecalculateNaturalSize             = true;
  mRecalculateLayoutSize              = true;
  mRecalculateHeightForWidth          = true;

  mOperationsPending = static_cast<OperationsMask>(mOperationsPending | VALIDATE_FONTS | SHAPE_TEXT | BIDI_INFO |
                                                   GET_GLYPH_METRICS | LAYOUT | UPDATE_LAYOUT_SIZE | REORDER | ALIGN);
}

void Controller::Impl::ClearStyleData()
{
  mModel->mLogicalModel->mColorRuns.Clear();
  mModel->mLogicalModel->mBackgroundColorRuns.Clear();
  mModel->mLogicalModel->ClearFontDescriptionRuns();
  mModel->mLogicalModel->ClearStrikethroughRuns();
  mModel->mLogicalModel->ClearUnderlineRuns();
}

void Controller::Impl::ResetScrollPosition()
{
  if(mEventData)
  {
    // Reset the scroll position.
    mModel->mScrollPosition                = Vector2::ZERO;
    mEventData->mScrollAfterUpdatePosition = true;
    SyncReplacementScrollPosition();
  }
}

} // namespace Dali::Ui::Text
