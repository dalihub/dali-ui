#ifndef DALI_UI_TEXT_CONTROLLER_IMPL_H
#define DALI_UI_TEXT_CONTROLLER_IMPL_H

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
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/adaptor-framework/clipboard-integ.h>
#include <dali/public-api/rendering/shader.h>
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/text/input-style.h>
#include <dali-ui-foundation/internal/text/controller/text-controller.h>
#include <dali-ui-foundation/internal/text/cursor-helper-functions.h>
#include <dali-ui-foundation/internal/text/input-filter-processor.h>
#include <dali-ui-foundation/internal/text/multi-language-support.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-edit-normalizer.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-render-state.h>
#include <dali-ui-foundation/internal/text/styled-text/editable-styled-text-data.h>
#include <dali-ui-foundation/internal/text/text-definitions.h>
#include <dali-ui-foundation/internal/text/text-model.h>
#include <dali-ui-foundation/internal/text/text-view.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
const float DEFAULT_TEXTFIT_MIN            = 10.f;
const float DEFAULT_TEXTFIT_MAX            = 100.f;
const float DEFAULT_TEXTFIT_STEP           = 1.f;
const float DEFAULT_UI_SCALE               = 1.f;
const float DEFAULT_FONT_SIZE_SCALE        = 1.f;
const float DEFAULT_MIN_FONT_SIZE_SCALE    = 0.01f;
const float DEFAULT_MAX_FONT_SIZE_SCALE    = 10.f;
const float DEFAULT_DISABLED_COLOR_OPACITY = 0.3f;

// Forward declarations
struct CursorInfo;
struct FontDefaults;
struct ControllerImplEventHandler;
struct ControllerImplModelUpdater;
struct SelectionHandleController;

struct Event
{
  // Used to queue input events until DoRelayout()
  enum Type
  {
    CURSOR_KEY_EVENT,
    TAP_EVENT,
    PAN_EVENT,
    LONG_PRESS_EVENT,
    GRAB_HANDLE_EVENT,
    LEFT_SELECTION_HANDLE_EVENT,
    RIGHT_SELECTION_HANDLE_EVENT,
    SELECT,
    SELECT_ALL,
    SELECT_NONE,
    SELECT_RANGE,
  };

  union Param
  {
    int          mInt;
    unsigned int mUint;
    float        mFloat;
    bool         mBool;
  };

  Event(Type eventType)
  : type(eventType)
  {
    p1.mInt = 0;
    p2.mInt = 0;
    p3.mInt = 0;
  }

  Type  type;
  Param p1;
  Param p2;
  Param p3;
};

struct EventData
{
  enum State
  {
    INACTIVE,
    INTERRUPTED,
    SELECTING,
    EDITING,
    EDITING_WITH_POPUP,
    EDITING_WITH_GRAB_HANDLE,
    EDITING_WITH_PASTE_POPUP,
    GRAB_HANDLE_PANNING,
    SELECTION_HANDLE_PANNING,
    TEXT_PANNING
  };

  EventData(DecoratorPtr decorator, InputMethodContext& inputMethodContext);

  ~EventData() = default;

  static bool IsEditingState(State stateToCheck)
  {
    return (stateToCheck == EDITING || stateToCheck == EDITING_WITH_POPUP || stateToCheck == EDITING_WITH_GRAB_HANDLE ||
            stateToCheck == EDITING_WITH_PASTE_POPUP);
  }

  DecoratorPtr                            mDecorator;               ///< Pointer to the decorator.
  InputMethodContext                      mInputMethodContext;      ///< The Input Method Framework Manager.
  std::unique_ptr<FontDefaults>           mPlaceholderFont;         ///< The placeholder default font.
  std::unique_ptr<EditableStyledTextData> mEditableStyledText;      ///< Authored editable StyledText data, allocated on demand.
  std::string                             mPlaceholderTextActive;   ///< The text to display when the TextField is empty with key-input focus.
  std::string                             mPlaceholderTextInactive; ///< The text to display when the TextField is empty and inactive.
  Vector4                                 mPlaceholderTextColor;    ///< The in/active placeholder text color.

  /**
   * This is used to delay handling events until after the model has been updated.
   * The number of updates to the model is minimized to improve performance.
   */
  std::vector<Event> mEventQueue; ///< The queue of touch events etc.

  Vector<Ui::Integration::Text::InputStyle::Mask>
    mInputStyleChangedQueue; ///< Queue of changes in the input style. Used to emit the signal in the iddle callback.

  Ui::Integration::Text::InputStyle mInputStyle; ///< The style to be set to the new inputed text.

  State mPreviousState; ///< Stores the current state before it's updated with the new one.
  State mState;         ///< Selection mode, edit mode etc.

  CharacterIndex mPrimaryCursorPosition;  ///< Index into logical model for primary cursor.
  CharacterIndex mLeftSelectionPosition;  ///< Index into logical model for left selection handle.
  CharacterIndex mRightSelectionPosition; ///< Index into logical model for right selection handle.

  CharacterIndex mPreEditStartPosition; ///< Used to remove the pre-edit text if necessary.
  Length         mPreEditLength;        ///< Used to remove the pre-edit text if necessary.

  float mCursorHookPositionX; ///< Used to move the cursor with the keys or when scrolling the text vertically with the
                              ///< handles.

  Controller::NoTextTap::Action mDoubleTapAction; ///< Action to be done when there is a double tap on top of 'no text'
  Controller::NoTextTap::Action mLongPressAction; ///< Action to be done when there is a long press on top of 'no text'

  bool mIsShowingPlaceholderText : 1;     ///< True if the place-holder text is being displayed.
  bool mPreEditFlag : 1;                  ///< True if the model contains text in pre-edit state.
  bool mDecoratorUpdated : 1;             ///< True if the decorator was updated during event processing.
  bool mCursorBlinkEnabled : 1;           ///< True if cursor should blink when active.
  bool mGrabHandleEnabled : 1;            ///< True if grab handle is enabled.
  bool mGrabHandlePopupEnabled : 1;       ///< True if the grab handle popu-up should be shown.
  bool mSelectionEnabled : 1;             ///< True if selection handles, highlight etc. are enabled.
  bool mUpdateCursorHookPosition : 1;     ///< True if the cursor hook position must be updated. Used to move the cursor
                                          ///< with the keys 'up' and 'down'.
  bool mUpdateCursorPosition : 1;         ///< True if the visual position of the cursor must be recalculated.
  bool mUpdateGrabHandlePosition : 1;     ///< True if the visual position of the grab handle must be recalculated.
  bool mUpdateLeftSelectionPosition : 1;  ///< True if the visual position of the left selection handle must be
                                          ///< recalculated.
  bool mUpdateRightSelectionPosition : 1; ///< True if the visual position of the right selection handle must be
                                          ///< recalculated.
  bool mIsLeftHandleSelected : 1;         ///< Whether is the left handle the one which is selected.
  bool mIsRightHandleSelected : 1;        ///< Whether is the right handle the one which is selected.
  bool mUpdateHighlightBox : 1;           ///< True if the text selection high light box must be updated.
  bool mScrollAfterUpdatePosition : 1;    ///< Whether to scroll after the cursor position is updated.
  bool mScrollAfterDelete : 1;            ///< Whether to scroll after delete characters.
  bool mAllTextSelected : 1;              ///< True if the selection handles are selecting all the text.
  bool mUpdateInputStyle : 1;             ///< Whether to update the input style after moving the cursor.
  bool mPasswordInput : 1;                ///< True if password input is enabled.
  bool mCheckScrollAmount : 1;            ///< Whether to check scrolled amount after updating the position
  bool mIsPlaceholderPixelSize : 1;       ///< True if the placeholder font size is set as pixel size.
  bool mIsPlaceholderElideEnabled : 1;    ///< True if the placeholder text's elide is enabled.
  bool mPlaceholderEllipsisFlag : 1;      ///< True if the text controller sets the placeholder ellipsis.
  bool mShowPlaceholderOnFocus : 1;       ///< Whether to show the placeholder text when the control has focus.
  bool mShiftSelectionFlag : 1;           ///< True if the text selection using Shift key is enabled.
  bool mUpdateAlignment : 1;              ///< True if the whole text needs to be full aligned..
  bool mEditingEnabled : 1;               ///< True if the editing is enabled, false otherwise.
};

struct ModifyEvent
{
  enum Type
  {
    TEXT_REPLACED, ///< The entire text was replaced
    TEXT_INSERTED, ///< Insert characters at the current cursor position
    TEXT_DELETED   ///< Characters were deleted
  };

  Type type;
};

struct FontDefaults
{
  FontDefaults()
  : mDefaultPointSize(0.f),
    mFitPointSize(0.f),
    mFontId(0u),
    familyDefined(false),
    weightDefined(false),
    widthDefined(false),
    slantDefined(false),
    sizeDefined(false),
    fontDescriptionGetted(false),
    mFontDescription()
  {
  }

  FontId GetFontId(TextAbstraction::FontClient& fontClient, float fontPointSize)
  {
    if(!mFontId)
    {
      const PointSize26Dot6 pointSize = static_cast<PointSize26Dot6>(fontPointSize * 64.f);

      if(DALI_UNLIKELY(!fontDescriptionGetted))
      {
        fontDescriptionGetted = true;
        fontClient.GetDefaultPlatformFontDescription(mFontDescription);
      }

      mFontId = fontClient.GetFontId(mFontDescription, pointSize);
    }

    return mFontId;
  }

  TextAbstraction::FontDescription& GetFontDescription()
  {
    if(!fontDescriptionGetted)
    {
      DALI_ASSERT_DEBUG(Dali::Adaptor::IsAvailable() && "Must be called GetFontDescription() for non-const type at main thread!");

      fontDescriptionGetted = true;
      // Initially use the default platform font
      TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
      fontClient.GetDefaultPlatformFontDescription(mFontDescription);
    }
    return mFontDescription;
  }

  const TextAbstraction::FontDescription& GetFontDescription() const
  {
    DALI_ASSERT_DEBUG(fontDescriptionGetted && "Must be called GetFontDescription() for non-const type before!");
    return mFontDescription;
  }

  float  mDefaultPointSize; ///< The default font's point size.
  float  mFitPointSize;     ///< The fit font's point size.
  FontId mFontId;           ///< The font's id of the default font.

  bool familyDefined : 1;         ///< Whether the default font's family name is defined.
  bool weightDefined : 1;         ///< Whether the default font's weight is defined.
  bool widthDefined : 1;          ///< Whether the default font's width is defined.
  bool slantDefined : 1;          ///< Whether the default font's slant is defined.
  bool sizeDefined : 1;           ///< Whether the default font's point size is defined.
  bool fontDescriptionGetted : 1; ///< Whether the default font's description getted from default platform font.

private:
  TextAbstraction::FontDescription mFontDescription; ///< The default font's description. Make it private to ensure to get
                                                     ///< the default font's description from the system at least once to initialize it.
};

/**
 * @brief Stores indices used to update the text.
 * Stores the character index where the text is updated and the number of characters removed and added.
 * Stores as well indices to the first and the last paragraphs to be updated.
 */
struct TextUpdateInfo
{
  TextUpdateInfo()
  : mCharacterIndex(0u),
    mNumberOfCharactersToRemove(0u),
    mNumberOfCharactersToAdd(0u),
    mPreviousNumberOfCharacters(0u),
    mParagraphCharacterIndex(0u),
    mRequestedNumberOfCharacters(0u),
    mStartGlyphIndex(0u),
    mStartLineIndex(0u),
    mEstimatedNumberOfLines(0u),
    mClearAll(true),
    mFullRelayoutNeeded(true),
    mIsLastCharacterNewParagraph(false)
  {
  }

  ~TextUpdateInfo()
  {
  }

  CharacterIndex mCharacterIndex;             ///< Index to the first character to be updated.
  Length         mNumberOfCharactersToRemove; ///< The number of characters to be removed.
  Length         mNumberOfCharactersToAdd;    ///< The number of characters to be added.
  Length         mPreviousNumberOfCharacters; ///< The number of characters before the text update.

  CharacterIndex mParagraphCharacterIndex;     ///< Index of the first character of the first paragraph to be updated.
  Length         mRequestedNumberOfCharacters; ///< The requested number of characters.
  GlyphIndex     mStartGlyphIndex;
  LineIndex      mStartLineIndex;
  Length         mEstimatedNumberOfLines; ///< The estimated number of lines. Used to avoid reallocations when layouting.

  bool mClearAll : 1; ///< Whether the whole text is cleared. i.e. when the text is reset.
  bool
       mFullRelayoutNeeded : 1;          ///< Whether a full re-layout is needed. i.e. when a new size is set to the text control.
  bool mIsLastCharacterNewParagraph : 1; ///< Whether the last character is a new paragraph character.

  void Clear()
  {
    // Clear all info except the mPreviousNumberOfCharacters member.
    mCharacterIndex              = static_cast<CharacterIndex>(-1);
    mNumberOfCharactersToRemove  = 0u;
    mNumberOfCharactersToAdd     = 0u;
    mParagraphCharacterIndex     = 0u;
    mRequestedNumberOfCharacters = 0u;
    mStartGlyphIndex             = 0u;
    mStartLineIndex              = 0u;
    mEstimatedNumberOfLines      = 0u;
    mClearAll                    = false;
    mFullRelayoutNeeded          = false;
    mIsLastCharacterNewParagraph = false;
  }
};

struct UnderlineDefaults
{
  std::string properties;
  // TODO: complete with underline parameters.
};

struct ShadowDefaults
{
  std::string properties;
  // TODO: complete with shadow parameters.
};

struct EmbossDefaults
{
  std::string properties;
  // TODO: complete with emboss parameters.
};

struct OutlineDefaults
{
  std::string properties;
  // TODO: complete with outline parameters.
};

struct Controller::Impl
{
public:
  /**
   * @brief Stores replacement data allocated for controllers that use replacements.
   */
  struct ReplacementData
  {
    ReplacementSourceSnapshot sourceSnapshot;
    ReplacementRenderState    renderState;
    uint64_t                  layoutGeneration{0u};
  };

  /**
   * @brief Stores customized anchor colors.
   */
  struct AnchorColorData
  {
    Vector4 color{Color::MEDIUM_BLUE};
    Vector4 clickedColor{Color::DARK_MAGENTA};
  };

  enum class ClearFocusOnEscapeState
  {
    ENABLE  = 0,
    DISABLE = 1,
    UNKNOWN = 2, ///< Unknown state
  };

public:
  Impl(Ui::Integration::Text::ControlInterface*           controlInterface,
       Ui::Integration::Text::EditableControlInterface*   editableControlInterface,
       Ui::Integration::Text::SelectableControlInterface* selectableControlInterface,
       Ui::Integration::Text::AnchorControlInterface*     anchorControlInterface)
  : mControlInterface(controlInterface),
    mEditableControlInterface(editableControlInterface),
    mSelectableControlInterface(selectableControlInterface),
    mAnchorControlInterface(anchorControlInterface),
    mModel(),
    mFontDefaults(NULL),
    mUnderlineDefaults(NULL),
    mShadowDefaults(NULL),
    mEmbossDefaults(NULL),
    mOutlineDefaults(NULL),
    mEventData(NULL),
    mIdleCallback(NULL),
    mHiddenInput(NULL),
    mInputFilterProcessor(nullptr),
    mMetrics(),
    mFontClient(),
    mClipboard(),
    mView(),
    mModifyEvents(),
    mTextFitCandidates(),
    mTextUpdateInfo(),
    mReplacementData(),
    mEndEllipsisResult(),
    mAnchorColorData(),
    mTextColor(Color::BLACK),
    mTextFitContentSize(),
    mOperationsPending(NO_OPERATION),
    mMaximumNumberOfCharacters(50u),
    mMaxFitCandidateIndex(-1),
    mLayoutDirection(LayoutDirection::LEFT_TO_RIGHT),
    mCurrentLineSize(0.f),
    mTextFitMinSize(DEFAULT_TEXTFIT_MIN),
    mTextFitMaxSize(DEFAULT_TEXTFIT_MAX),
    mTextFitStepSize(DEFAULT_TEXTFIT_STEP),
    mUiScale(DEFAULT_UI_SCALE),
    mFontSizeScale(DEFAULT_FONT_SIZE_SCALE),
    mMinFontSizeScale(DEFAULT_MIN_FONT_SIZE_SCALE),
    mMaxFontSizeScale(DEFAULT_MAX_FONT_SIZE_SCALE),
    mAdjustedFontSizeScale(DEFAULT_FONT_SIZE_SCALE),
    mSystemFontSizeScale(DEFAULT_FONT_SIZE_SCALE),
    mDisabledColorOpacity(DEFAULT_DISABLED_COLOR_OPACITY),
    mRenderScale(1.0f),
    mShouldClearFocusOnEscape(ClearFocusOnEscapeState::UNKNOWN),
    mIsTextDirectionRTL(false),
    mRecalculateNaturalSize(true),
    mRecalculateLayoutSize(true),
    mRecalculateHeightForWidth(true),
    mIsEmittingTextChangedSignal(false),
    mClipboardHideEnabled(true),
    mIsMarqueeEnabled(false),
    mIsMarqueeMaxTextureExceeded(false),
    mUpdateTextDirection(true),
    mUnderlineSetByString(false),
    mShadowSetByString(false),
    mOutlineSetByString(false),
    mFontStyleSetByString(false),
    mStrikethroughSetByString(false),
    mSystemFontSizeScaleEnabled(false),
    mTextFitEnabled(false),
    mTextFitChanged(false),
    mTextFitCandidatesEnabled(false),
    mIsLayoutDirectionChanged(false),
    mIsUserInteractionEnabled(true),
    mProcessorRegistered(false),
    mTextCutout(false),
    mIsCursorInsetEnabled(false),
    mIsAsyncRendering(false)
  {
    mModel = Model::New();

    if(mEditableControlInterface != nullptr && Dali::Integration::Clipboard::IsAvailable())
    {
      mClipboard = Clipboard::Get();
    }

    mView.SetVisualModel(mModel->mVisualModel);
    mView.SetLogicalModel(mModel->mLogicalModel);

    mMetrics = Metrics::New();
    mLayoutEngine.SetMetrics(mMetrics);

    // Set the text properties to default
    mModel->mVisualModel->SetUnderlineEnabled(false);
    mModel->mVisualModel->SetUnderlineHeight(0.0f);
  }

  ~Impl()
  {
    mView.SetFinalElisionResult(nullptr);
    if(mReplacementData && mReplacementData->renderState.attempted)
    {
      mView.SetVisualModel(mModel->mVisualModel);
      mView.SetLogicalModel(mModel->mLogicalModel);
      TextAbstraction::BidirectionalSupport bidirectionalSupport = TextAbstraction::BidirectionalSupport::Get();
      mReplacementData->renderState.Clear(bidirectionalSupport);
    }
    delete mHiddenInput;
    delete mFontDefaults;
    delete mUnderlineDefaults;
    delete mShadowDefaults;
    delete mEmbossDefaults;
    delete mOutlineDefaults;
    delete mEventData;
  }

  /**
   * @brief Get the font client.
   *
   * @return fontClient The font client.
   */
  TextAbstraction::FontClient& GetFontClient()
  {
    if(!mFontClient)
    {
      mFontClient = TextAbstraction::FontClient::Get();
    }
    return mFontClient;
  }

  void ClearEndEllipsisResult()
  {
    if(!HasValidReplacementSource())
    {
      mView.SetFinalElisionResult(nullptr);
    }
    mEndEllipsisResult.reset();
  }

  FinalElisionResult& GetOrCreateEndEllipsisResult()
  {
    if(!mEndEllipsisResult)
    {
      mEndEllipsisResult = std::make_unique<FinalElisionResult>();
    }
    return *mEndEllipsisResult;
  }

  const FinalElisionResult* GetEndEllipsisResult() const
  {
    return mEndEllipsisResult && mEndEllipsisResult->resolved
             ? mEndEllipsisResult.get()
             : nullptr;
  }

  /**
   * @brief Invalidates all replacement render state.
   *
   * The active view is detached before the projected model is released.
   */
  void InvalidateReplacementRenderState()
  {
    // Detach first: ReplacementRenderState::Clear() may release the model
    // currently referenced by mView.
    mView.SetVisualModel(mModel->mVisualModel);
    mView.SetLogicalModel(mModel->mLogicalModel);
    mView.SetFinalElisionResult(nullptr);
    if(mReplacementData && mReplacementData->renderState.attempted)
    {
      TextAbstraction::BidirectionalSupport bidirectionalSupport = TextAbstraction::BidirectionalSupport::Get();
      mReplacementData->renderState.Clear(bidirectionalSupport);
    }
    if(mReplacementData)
    {
      ++mReplacementData->layoutGeneration;
    }
  }

  bool HasReplacementData() const
  {
    return static_cast<bool>(mReplacementData);
  }

  bool HasValidReplacementSource() const
  {
    return mReplacementData && mReplacementData->sourceSnapshot.hasValidReplacementSource &&
           !mReplacementData->sourceSnapshot.runs.Empty();
  }

  uint64_t NextReplacementLayoutGeneration()
  {
    return ++mReplacementData->layoutGeneration;
  }

  /**
   * @brief Gets the authored replacement snapshot.
   *
   * @return The authored snapshot, or an empty snapshot if none was allocated.
   */
  const ReplacementSourceSnapshot& GetReplacementSourceSnapshot() const
  {
    static const ReplacementSourceSnapshot EMPTY_SNAPSHOT;
    return mReplacementData ? mReplacementData->sourceSnapshot : EMPTY_SNAPSHOT;
  }

  /**
   * @brief Gets or creates the authored replacement snapshot.
   *
   * @return The authored snapshot.
   */
  ReplacementSourceSnapshot& GetOrCreateReplacementSourceSnapshot()
  {
    if(!mReplacementData)
    {
      mReplacementData = std::make_unique<ReplacementData>();
    }
    return mReplacementData->sourceSnapshot;
  }

  /**
   * @brief Gets the replacement render state.
   *
   * @return The render state, or an empty state if none was allocated.
   */
  const ReplacementRenderState& GetReplacementRenderState() const
  {
    static const ReplacementRenderState EMPTY_STATE;
    return mReplacementData ? mReplacementData->renderState : EMPTY_STATE;
  }

  /**
   * @brief Gets the allocated replacement render state.
   *
   * @return The render state, or nullptr if none was allocated.
   */
  const ReplacementRenderState* GetReplacementRenderStatePtr() const
  {
    return mReplacementData ? &mReplacementData->renderState : nullptr;
  }

  /**
   * @brief Gets or creates the replacement render state.
   *
   * @return The replacement render state.
   */
  ReplacementRenderState& GetOrCreateReplacementRenderState()
  {
    if(!mReplacementData)
    {
      mReplacementData = std::make_unique<ReplacementData>();
    }
    return mReplacementData->renderState;
  }

  /**
   * @brief Releases the authored and rendered replacement data.
   */
  void ClearReplacementData()
  {
    mReplacementData.reset();
  }

  /**
   * @brief Gets the model used for editable geometry.
   *
   * @return The current editable geometry model.
   */
  ModelPtr GetEditableGeometryModel() const
  {
    const ReplacementRenderState* state = GetReplacementRenderStatePtr();
    return state && state->processingModel && state->projection.HasReplacements() ? state->processingModel : mModel;
  }

  /**
   * @brief Maps a logical boundary to the editable geometry model.
   *
   * @param[in] boundary The logical boundary.
   * @param[in] affinity The boundary affinity.
   * @return The geometry-model boundary.
   */
  CharacterIndex LogicalBoundaryToEditable(CharacterIndex                          boundary,
                                           ReplacementProjection::BoundaryAffinity affinity) const
  {
    const ReplacementRenderState* state = GetReplacementRenderStatePtr();
    return state && state->processingModel && state->projection.HasReplacements()
             ? state->projection.LogicalBoundaryToProjected(boundary, affinity)
             : std::min(boundary, static_cast<CharacterIndex>(mModel->mLogicalModel->mText.Count()));
  }

  /**
   * @brief Maps an editable geometry boundary to the logical model.
   *
   * @param[in] boundary The geometry-model boundary.
   * @return The logical boundary.
   */
  CharacterIndex EditableBoundaryToLogical(CharacterIndex boundary) const
  {
    const ReplacementRenderState* state = GetReplacementRenderStatePtr();
    return state && state->processingModel && state->projection.HasReplacements()
             ? state->projection.ProjectedBoundaryToLogical(boundary)
             : std::min(boundary, static_cast<CharacterIndex>(mModel->mLogicalModel->mText.Count()));
  }

  /**
   * @brief Copies the authoritative editable scroll position to the processing model.
   */
  void SyncReplacementScrollPosition()
  {
    ReplacementRenderState* state = mReplacementData ? &mReplacementData->renderState : nullptr;
    if(state && state->processingModel && state->projection.HasReplacements())
    {
      state->processingModel->mScrollPosition     = mModel->mScrollPosition;
      state->processingModel->mScrollPositionLast = mModel->mScrollPositionLast;
    }
  }

  /**
   * @brief Normalizes a logical cursor boundary around replacements.
   *
   * @param[in] boundary The logical boundary.
   * @param[in] affinity The requested affinity.
   * @return The normalized logical boundary.
   */
  CharacterIndex NormalizeReplacementBoundary(CharacterIndex                              boundary,
                                              ReplacementEditNormalizer::BoundaryAffinity affinity) const
  {
    return HasValidReplacementSource()
             ? ReplacementEditNormalizer::NormalizeBoundary(GetReplacementSourceSnapshot().runs,
                                                            boundary,
                                                            affinity,
                                                            static_cast<Dali::Ui::Text::Length>(mModel->mLogicalModel->mText.Count()))
             : std::min(boundary, static_cast<CharacterIndex>(mModel->mLogicalModel->mText.Count()));
  }

  /**
   * @brief Expands a logical selection around replacements.
   *
   * @param[in,out] first The first boundary.
   * @param[in,out] second The second boundary.
   */
  void NormalizeReplacementSelection(CharacterIndex& first, CharacterIndex& second) const
  {
    if(HasValidReplacementSource())
    {
      ReplacementEditNormalizer::NormalizeSelection(GetReplacementSourceSnapshot().runs,
                                                    first,
                                                    second,
                                                    static_cast<Dali::Ui::Text::Length>(mModel->mLogicalModel->mText.Count()));
    }
    else
    {
      const CharacterIndex textLength = static_cast<Dali::Ui::Text::CharacterIndex>(mModel->mLogicalModel->mText.Count());
      first                           = std::min(first, textLength);
      second                          = std::min(second, textLength);
    }
  }

  /**
   * @brief Invalidates derived state and updates authored ranges for an edit.
   *
   * @param[in] start The normalized edit start.
   * @param[in] removedLength The normalized removed length.
   * @param[in] insertedLength The inserted length.
   */
  void PrepareEditableTextEdit(CharacterIndex start, Length removedLength, Length insertedLength)
  {
    if(removedLength == 0u && insertedLength == 0u)
    {
      return;
    }

    if(mEventData && mEventData->mEditableStyledText)
    {
      mEventData->mEditableStyledText->ApplyEdit(start, removedLength, insertedLength);
    }

    ClearEndEllipsisResult();

    if(!HasReplacementData())
    {
      return;
    }

    InvalidateReplacementRenderState();
    ReplacementSourceSnapshot& source = mReplacementData->sourceSnapshot;
    ReplacementEditNormalizer::ApplyEdit(source.runs, start, removedLength, insertedLength);
    source.hasValidReplacementSource = !source.runs.Empty();
    ++source.sourceRevision;
    if(source.sourceRevision == 0u)
    {
      source.sourceRevision = 1u;
    }
    if(!source.hasValidReplacementSource)
    {
      ClearReplacementData();
    }
  }

  // Text Controller Implementation.

  /**
   * @copydoc Text::Controller::RequestRelayout()
   */
  void RequestRelayout();

  /**
   * @copydoc Text::Controller::InvalidateMeasure()
   */
  void InvalidateMeasure();

  /**
   * @copydoc Text::Controller::RequestAsyncRender()
   */
  void RequestAsyncRender();

  /**
   * @brief Request a relayout using the Ui::Integration::Text::ControlInterface.
   */
  void QueueModifyEvent(ModifyEvent::Type type)
  {
    if(ModifyEvent::TEXT_REPLACED == type)
    {
      // Cancel previously queued inserts etc.
      mModifyEvents.Clear();
    }

    ModifyEvent event;
    event.type = type;
    mModifyEvents.PushBack(event);

    // The event will be processed during relayout
    RequestRelayout();
  }

  /**
   * @copydoc Text::Controller::InvalidateFontData()
   */
  void InvalidateFontData();

  /**
   * @brief Invalidates layout-direction dependent text data and requests relayout.
   */
  void InvalidateLayoutDirectionData();

  /**
   * @brief Helper to move the cursor, grab handle etc.
   */
  bool ProcessInputEvents();

  /**
   * @brief Helper to check whether any place-holder text is available.
   */
  bool IsPlaceholderAvailable() const
  {
    return (mEventData &&
            (!mEventData->mPlaceholderTextInactive.empty() || !mEventData->mPlaceholderTextActive.empty()));
  }

  bool IsShowingPlaceholderText() const
  {
    return (mEventData && mEventData->mIsShowingPlaceholderText);
  }

  /**
   * @brief Helper to check whether active place-holder text is available.
   */
  bool IsFocusedPlaceholderAvailable() const
  {
    return (mEventData && !mEventData->mPlaceholderTextActive.empty());
  }

  bool IsShowingRealText() const
  {
    return (!IsShowingPlaceholderText() && 0u != mModel->mLogicalModel->mText.Count());
  }

  /**
   * @brief Called when placeholder-text is hidden
   */
  void PlaceholderCleared()
  {
    if(mEventData)
    {
      mEventData->mIsShowingPlaceholderText = false;

      // Remove mPlaceholderTextColor
      mModel->mVisualModel->SetTextColor(mTextColor);
    }
  }

  void ClearPreEditFlag()
  {
    if(mEventData)
    {
      mEventData->mPreEditFlag          = false;
      mEventData->mPreEditStartPosition = 0;
      mEventData->mPreEditLength        = 0;
    }
  }

  void ResetInputMethodContext()
  {
    if(mEventData)
    {
      // Reset incase we are in a pre-edit state.
      if(mEventData->mInputMethodContext)
      {
        Dali::Integration::InputMethodContext::Reset(mEventData->mInputMethodContext); // Will trigger a message ( commit, get surrounding )
      }

      ClearPreEditFlag();
    }
  }

  /**
   * @copydoc Controller::SetAnchorColor()
   */
  void SetAnchorColor(const Vector4& color);

  /**
   * @copydoc Controller::GetAnchorColor()
   */
  const Vector4& GetAnchorColor() const;

  /**
   * @copydoc Controller::SetAnchorClickedColor()
   */
  void SetAnchorClickedColor(const Vector4& color);

  /**
   * @copydoc Controller::GetAnchorClickedColor()
   */
  const Vector4& GetAnchorClickedColor() const;

  /**
   * @brief Updates the color of anchors.
   */
  void UpdateAnchorColor();

  /**
   * @brief Helper to notify InputMethodContext with surrounding text & cursor changes.
   */
  void NotifyInputMethodContext();

  /**
   * @brief Helper to notify InputMethodContext with multi line status.
   */
  void NotifyInputMethodContextMultiLineStatus();

  /**
   * @brief Retrieve the current cursor position.
   *
   * @return The cursor position.
   */
  CharacterIndex GetLogicalCursorPosition() const;

  /**
   * @brief Retrieves the number of consecutive white spaces starting from the given @p index.
   *
   * @param[in] index The character index from where to count the number of consecutive white spaces.
   *
   * @return The number of consecutive white spaces.
   */
  Length GetNumberOfWhiteSpaces(CharacterIndex index) const;

  /**
   * @brief Retrieve any text previously set.
   *
   * @param[out] text A string of UTF-8 characters.
   */
  void GetText(std::string& text) const;

  /**
   * @brief Retrieve any text previously set starting from the given @p index.
   *
   * @param[in] index The character index from where to retrieve the text.
   * @param[out] text A string of UTF-8 characters.
   *
   * @see Dali::Ui::Text::Controller::GetText()
   */
  void GetText(CharacterIndex index, std::string& text) const;

  /**
   * @brief Retrieves number of characters previously set.
   *
   * @return A length of UTF-32 characters.
   *
   * @see Dali::Ui::Text::Controller::GetNumberOfCharacters()
   */
  Length GetNumberOfCharacters() const;

  bool EnsureClipboardCreated()
  {
    if(!mClipboard)
    {
      mClipboard = Clipboard::Get();
    }

    return mClipboard != nullptr ? true : false;
  }

  bool IsClipboardVisible()
  {
    bool result(Dali::Integration::Clipboard::IsAvailable() && EnsureClipboardCreated() && Dali::Integration::Clipboard::IsVisible(mClipboard));
    return result;
  }

  /**
   * @brief Whether the clipboard is empty or not.
   * Checks the types that the text controller can paste and returns the result.
   *
   * @return Return whether or not the clipboard is empty.
   */
  bool IsClipboardEmpty();

  /**
   * @copydoc Controller::GetLayoutDirection()
   */
  Dali::LayoutDirection::Type GetLayoutDirection(Dali::Actor& actor) const;

  /**
   * @brief Checks text direction.
   * @return The text direction.
   */
  Direction GetTextDirection();

  /**
   * @brief Calculates the start character index of the first paragraph to be updated and
   * the end character index of the last paragraph to be updated.
   *
   * @param[out] numberOfCharacters The number of characters to be updated.
   */
  void CalculateTextUpdateIndices(Length& numberOfCharacters);

  /**
   * @brief Helper to clear the parts of the model specified by the given @p operations and from @p startIndex to @p
   * endIndex.
   *
   * @note It never clears the text stored in utf32.
   *
   * @param[in] startIndex Index to the first character to be cleared.
   * @param[in] endIndex Index to the last character to be cleared.
   * @param[in] operations The operations required.
   */
  void ClearModelData(CharacterIndex startIndex, CharacterIndex endIndex, OperationsMask operations);

  /**
   * @brief Updates the logical and visual models. Updates the style runs in the visual model when the text's styles
   * changes.
   *
   * When text or style changes the model is set with some operations pending.
   * When i.e. the text's size or a relayout is required this method is called
   * with a given @p operationsRequired parameter. The operations required are
   * matched with the operations pending to perform the minimum number of operations.
   *
   * @param[in] operationsRequired The operations required.
   *
   * @return @e true if the model has been modified.
   */
  bool UpdateModel(OperationsMask operationsRequired);

  /**
   * @brief Retreieves the default style.
   *
   * @param[out] inputStyle The default style.
   */
  void RetrieveDefaultInputStyle(Ui::Integration::Text::InputStyle& inputStyle);

  /**
   * @brief Retrieves the line height of the default font.
   */
  float GetDefaultFontLineHeight();

  /**
   * @brief Retrieves the default line box height after applying absolute or
   * relative LineHeight settings.
   *
   * @return The default line box height.
   */
  float GetDefaultLineBoxHeight();

  /**
   * @brief Retrieves the default line box height using the given font line height.
   *
   * This overload avoids recalculating the font line height when it is already known.
   *
   * @param[in] defaultFontLineHeight The font line height.
   * @return The default line box height.
   */
  float GetDefaultLineBoxHeight(float defaultFontLineHeight);

  /**
   * @copydoc Controller::SetDefaultLineSpacing
   */
  bool SetDefaultLineSpacing(float lineSpacing);

  /**
   * @brief Requests cursor or selection decorator update after text geometry changes.
   */
  void RequestDecoratorUpdate();

  /**
   * @copydoc Controller::SetDefaultLineSize
   */
  bool SetDefaultLineSize(float lineSize);

  /**
   * @copydoc Controller::SetRelativeLineSize
   */
  bool SetRelativeLineSize(float relativeLineSize);

  /**
   * @copydoc Controller::GetRelativeLineSize
   */
  float GetRelativeLineSize();

  /**
   * @copydoc Text::Controller::GetPrimaryCursorPosition()
   */
  CharacterIndex GetPrimaryCursorPosition() const;

  /**
   * @copydoc Text::Controller::SetPrimaryCursorPosition()
   */
  bool SetPrimaryCursorPosition(CharacterIndex index, bool focused);

  /**
   * @copydoc Ui::Integration::Text::SelectableControlInterface::GetSelectedText()
   */
  std::string GetSelectedText();

  /**
   * @copydoc Ui::Integration::Text::EditableControlInterface::CopyText()
   */
  std::string CopyText();

  /**
   * @copydoc Ui::Integration::Text::EditableControlInterface::CutText()
   */
  std::string CutText();

  /**
   * @copydoc Ui::Integration::Text::SelectableControlInterface::SetTextSelectionRange()
   */
  void SetTextSelectionRange(const uint32_t* pStart, const uint32_t* pEndf);

  /**
   * @copydoc Ui::Integration::Text::SelectableControlInterface::GetTextSelectionRange()
   */
  Ui::Integration::Text::Uint32Pair GetTextSelectionRange() const;

  /**
   * @copydoc Ui::Integration::Text::EditableControlInterface::IsEditable()
   */
  bool IsEditable() const;

  /**
   * @copydoc Ui::Integration::Text::EditableControlInterface::SetEditable()
   */
  void SetEditable(bool editable);

  /**
   * @copydoc Controller::UpdateAfterFontChange
   */
  void UpdateAfterFontChange(const std::string& newDefaultFont);

  /**
   * @brief Retrieves the selected text. It removes the text if the @p deleteAfterRetrieval parameter is @e true.
   *
   * @param[out] selectedText The selected text encoded in utf8.
   * @param[in] deleteAfterRetrieval Whether the text should be deleted after retrieval.
   */
  void RetrieveSelection(std::string& selectedText, bool deleteAfterRetrieval);

  void SetSelection(int start, int end);

  std::pair<int, int> GetSelectionIndexes() const;

  void ShowClipboard();

  void HideClipboard();

  void SetClipboardHideEnable(bool enable);

  bool CopyStringToClipboard(const std::string& source);

  void SendSelectionToClipboard(bool deleteAfterSending);

  void RepositionSelectionHandles();
  void RepositionSelectionHandles(float visualX, float visualY, Controller::NoTextTap::Action action);

  void SetPopupButtons();

  void ChangeState(EventData::State newState);

  /**
   * @brief Calculates the cursor's position for a given character index in the logical order.
   *
   * It retrieves as well the line's height and the cursor's height and
   * if there is a valid alternative cursor, its position and height.
   *
   * @param[in] logical The logical cursor position (in characters). 0 is just before the first character, a value equal
   * to the number of characters is just after the last character.
   * @param[out] cursorInfo The line's height, the cursor's height, the cursor's position and whether there is an
   * alternative cursor.
   */
  void GetCursorPosition(CharacterIndex logical, CursorInfo& cursorInfo);

  /**
   * @brief Calculates the new cursor index.
   *
   * It takes into account that in some scripts multiple characters can form a glyph and all of them
   * need to be jumped with one key event.
   *
   * @param[in] index The initial new index.
   *
   * @return The new cursor index.
   */
  CharacterIndex CalculateNewCursorIndex(CharacterIndex index) const;

  /**
   * @brief Hit-tests a cursor boundary in the current editable geometry.
   *
   * @param[in] visualX The x position in text coordinates.
   * @param[in] visualY The y position in text coordinates.
   * @param[in] hitTest The hit-test mode.
   * @param[out] matchedCharacter Whether a character was hit.
   * @return The logical cursor boundary.
   */
  CharacterIndex GetClosestLogicalCursorIndex(float                  visualX,
                                              float                  visualY,
                                              CharacterHitTest::Mode hitTest,
                                              bool&                  matchedCharacter) const;

  /**
   * @brief Updates the cursor position.
   *
   * Sets the cursor's position into the decorator. It transforms the cursor's position into decorator's coords.
   * It sets the position of the secondary cursor if it's a valid one.
   * Sets which cursors are active.
   *
   * @param[in] cursorInfo Contains the selection handle position in Actor's coords.
   *
   */
  void UpdateCursorPosition(const CursorInfo& cursorInfo);

  /**
   * @brief Updates the position of the given selection handle. It transforms the handle's position into decorator's
   * coords.
   *
   * @param[in] handleType One of the selection handles.
   * @param[in] cursorInfo Contains the selection handle position in Actor's coords.
   */
  void UpdateSelectionHandle(HandleType handleType, const CursorInfo& cursorInfo);

  /**
   * @biref Clamps the horizontal scrolling to get the control always filled with text.
   *
   * @param[in] layoutSize The size of the laid out text.
   */
  void ClampHorizontalScroll(const Vector2& layoutSize);

  /**
   * @biref Clamps the vertical scrolling to get the control always filled with text.
   *
   * @param[in] layoutSize The size of the laid out text.
   */
  void ClampVerticalScroll(const Vector2& layoutSize);

  /**
   * @brief Scrolls the text to make a position visible.
   *
   * @pre mEventData must not be NULL. (there is a text-input or selection capabilities).
   *
   * @param[in] position A position in text coords.
   * @param[in] lineHeight The line height for the given position.
   *
   * This method is called after inserting text, moving the cursor with the grab handle or the keypad,
   * or moving the selection handles.
   */
  void ScrollToMakePositionVisible(const Vector2& position, float lineHeight);

  /**
   * @brief Calculates the scroll target range from cursor info, including first/last line box edges.
   *
   * @param[in] info The cursor information.
   * @return The visible top and bottom range for scroll-to-visible.
   */
  std::pair<float, float> CalculateScrollTarget(const CursorInfo& info) const;

  /**
   * @brief Scrolls the text to make the cursor visible.
   *
   * This method is called after deleting text.
   */
  void ScrollTextToMatchCursor(const CursorInfo& cursorInfo);

  /**
   * @brief Scrolls the text to make primary cursor visible.
   */
  void ScrollTextToMatchCursor();

  /**
   * @brief Create an actor that renders the text background color
   *
   * @return the created actor or an empty handle if no background color needs to be rendered.
   */
  Actor CreateBackgroundActor();

  /**
   * @brief fill needed relayout parameters whenever a property is changed and a re-layout is needed for the entire
   * text.
   */
  void RelayoutAllCharacters();

  /**
   * @copydoc Controller::IsInputStyleChangedSignalsQueueEmpty
   */
  bool IsInputStyleChangedSignalsQueueEmpty();

  /**
   * @copydoc Controller::ProcessInputStyleChangedSignals
   */
  void ProcessInputStyleChangedSignals();

  /**
   * @copydoc Controller::ScrollBy()
   */
  void ScrollBy(Vector2 scroll);

  /**
   * @copydoc Controller::IsScrollable()
   */
  bool IsScrollable(const Vector2& displacement);

  /**
   * @copydoc Controller::GetHorizontalScrollPosition()
   */
  float GetHorizontalScrollPosition();

  /**
   * @copydoc Controller::GetVerticalScrollPosition()
   */
  float GetVerticalScrollPosition();

  /**
   * @copydoc Controller::SetMarqueeEnabled()
   */
  void SetMarqueeEnabled(bool enable, bool requestRelayout, MarqueeOrientation orientation);

  /**
   * @copydoc Controller::SetEnableCursorBlink()
   */
  void SetEnableCursorBlink(bool enable);

  /**
   * @copydoc Controller::SetMultiLineEnabled()
   */
  void SetMultiLineEnabled(bool enable);

  /**
   * @copydoc Controller::SetHorizontalAlignment()
   */
  void SetHorizontalAlignment(Alignment alignment);

  /**
   * @copydoc Controller::SetVerticalAlignment()
   */
  void SetVerticalAlignment(Alignment alignment);

  /**
   * @copydoc Controller::SetLineWrapMode()
   */
  void SetLineWrapMode(LineWrapMode textWarpMode);

  /**
   * @copydoc Controller::SetDefaultColor()
   */
  void SetDefaultColor(const Vector4& color);

  /**
   * @copydoc Controller::SetUserInteractionEnabled()
   */
  void SetUserInteractionEnabled(bool enabled);

  /**
   * @brief Helper to clear font-specific data (only).
   */
  void ClearFontData();

  /**
   * @copydoc Controller::GetEffectiveTextScale()
   */
  float GetEffectiveTextScale() const;

  /**
   * @copydoc Controller::SetUiScale()
   */
  bool SetUiScale(float scale);

  /**
   * @copydoc Controller::GetUiScale()
   */
  float GetUiScale() const;

  /**
   * @copydoc Controller::SetFontSizeScale()
   */
  void SetFontSizeScale(float scale);

  /**
   * @copydoc Controller::GetFontSizeScale()
   */
  float GetFontSizeScale() const;

  /**
   * @copydoc Controller::SetMinimumFontSizeScale()
   */
  void SetMinimumFontSizeScale(float scale);

  /**
   * @copydoc Controller::GetMinimumFontSizeScale()
   */
  float GetMinimumFontSizeScale() const;

  /**
   * @copydoc Controller::SetMaximumFontSizeScale()
   */
  void SetMaximumFontSizeScale(float scale);

  /**
   * @copydoc Controller::GetMaximumFontSizeScale()
   */
  float GetMaximumFontSizeScale() const;

  /**
   * @copydoc Controller::SetSystemFontSizeScaleEnabled()
   */
  void SetSystemFontSizeScaleEnabled(bool enabled);

  /**
   * @copydoc Controller::IsSystemFontSizeScaleEnabled()
   */
  bool IsSystemFontSizeScaleEnabled() const;

  /**
   * @copydoc Controller::SetSystemFontSizeScale()
   */
  void SetSystemFontSizeScale(float scale);

  /**
   * @copydoc Controller::GetAdjustedFontSizeScale()
   */
  float GetAdjustedFontSizeScale() const;

  /**
   * @brief Gets the system font size scale.
   *
   * @return The system font size scale.
   */
  float GetSystemFontSizeScale() const;

  /**
   * @brief Gets the current font size scale.
   *
   * @return The current font size scale.
   */
  float GetCurrentFontSizeScale() const;

  /**
   * @brief Calculates the adjusted font size scale.
   *
   * @return The adjusted font size scale.
   */
  float CalculateAdjustedFontSizeScale() const;

  /**
   * @brief Applies the adjusted font size scale.
   *
   * @return True if updated, otherwise false.
   */
  bool ApplyAdjustedFontSizeScale();

  /**
   * @brief Helper to clear text's style data.
   */
  void ClearStyleData();

  /**
   * @brief Used to reset the scroll position after setting a new text.
   */
  void ResetScrollPosition();

  /**
   * @brief Resets a provided vector with actors that marks the position of anchors in markup enabled text
   *
   * @param[out] anchorActors the vector of actor (empty collection if no anchors available).
   */
  void GetAnchorActors(std::vector<Ui::TextAnchor>& anchorActors);

  /**
   * @brief Return an index of first anchor in the anchor vector whose boundaries includes given character offset
   *
   * @param[in] characterOffset A position in text coords.
   *
   * @return the 0-based index in anchor vector (-1 if an anchor not found)
   */
  int32_t GetAnchorIndex(size_t characterOffset) const;

  /**
   * @brief Return the actor representing an anchor.
   *
   * @param[in] anchor An anchor.
   *
   * @return The actor representing an anchor.
   */
  Ui::TextAnchor CreateAnchorActor(Anchor anchor);

  /**
   * @brief Return true when text control should clear key input focus when escape key is pressed.
   *
   * @note We ask to style manager configurations, and store the option.
   * @note Default is true. Mean, without any options, text control should clear key input focus when escape key is
   * pressed.
   *
   * @return Whether text control should clear key input focus or not when escape key is pressed.
   */
  bool ShouldClearFocusOnEscape() const;

public:
  /**
   * @brief Gets implementation from the controller handle.
   * @param controller The text controller
   * @return The implementation of the Controller
   */
  static Impl& GetImplementation(Text::Controller& controller)
  {
    return *controller.mImpl;
  }

private:
  // Declared private and left undefined to avoid copies.
  Impl(const Impl&);
  // Declared private and left undefined to avoid copies.
  Impl& operator=(const Impl&);

  /**
   * @brief Copy Underlined-Character-Runs from Logical-Model to Underlined-Glyph-Runs in Visual-Model
   *
   * @param shouldClearPreUnderlineRuns Whether should clear the existing Underlined-Glyph-Runs in Visual-Model
   */
  void CopyUnderlinedFromLogicalToVisualModels(bool shouldClearPreUnderlineRuns);

  /**
   * @brief Copy strikethrough-Character-Runs from Logical-Model to strikethrough-Glyph-Runs in Visual-Model
   *
   */
  void CopyStrikethroughFromLogicalToVisualModels();

  /**
   * @brief Copy CharacterSpacing-Character-Runs from Logical-Model to CharacterSpacing-Glyph-Runs in Visual-Model
   *
   */
  void CopyCharacterSpacingFromLogicalToVisualModels();

public:
  // Core references / owned helpers
  Ui::Integration::Text::ControlInterface*           mControlInterface;           ///< Reference to the text controller.
  Ui::Integration::Text::EditableControlInterface*   mEditableControlInterface;   ///< Reference to the editable text controller.
  Ui::Integration::Text::SelectableControlInterface* mSelectableControlInterface; ///< Reference to the selectable text controller.
  Ui::Integration::Text::AnchorControlInterface*     mAnchorControlInterface;     ///< Reference to the anchor controller.
  ModelPtr                                           mModel;                      ///< Pointer to the text's model.
  FontDefaults*                                      mFontDefaults;               ///< Avoid allocating this when the user does not specify a font.
  UnderlineDefaults*                                 mUnderlineDefaults;          ///< Avoid allocating this when the user does not specify underline parameters.
  ShadowDefaults*                                    mShadowDefaults;             ///< Avoid allocating this when the user does not specify shadow parameters.
  EmbossDefaults*                                    mEmbossDefaults;             ///< Avoid allocating this when the user does not specify emboss parameters.
  OutlineDefaults*                                   mOutlineDefaults;            ///< Avoid allocating this when the user does not specify outline parameters.
  EventData*                                         mEventData;                  ///< Avoid allocating everything for text input until EnableTextInput().
  CallbackBase*                                      mIdleCallback;               ///< Callback what would be called at idler
  HiddenText*                                        mHiddenInput;                ///< Avoid allocating this when the user does not specify hidden input mode.
  std::unique_ptr<InputFilterProcessor>              mInputFilterProcessor;       ///< Avoid allocating this when the user does not specify input filter mode.
  MetricsPtr                                         mMetrics;                    ///< A wrapper around FontClient used to get metrics & potentially down-scaled Emoji metrics.

  // Main runtime objects
  TextAbstraction::FontClient mFontClient;       ///< Handle to the font client.
  Clipboard                   mClipboard;        ///< Handle to the system clipboard
  View                        mView;             ///< The view interface to the rendering back-end.
  Layout::Engine              mLayoutEngine;     ///< The layout engine.
  Shader                      mShaderBackground; ///< The shader for text background.

  // Containers / complex values
  Vector<ModifyEvent>                 mModifyEvents;      ///< Temporary stores the text set until the next relayout.
  Dali::Vector<Text::Fit::Candidate>  mTextFitCandidates; ///< List of Text::Fit::Candidate for TextFitCandidates operation.
  TextUpdateInfo                      mTextUpdateInfo;    ///< Info of the characters updated.
  std::unique_ptr<ReplacementData>    mReplacementData;   ///< Replacement data allocated on demand.
  std::unique_ptr<FinalElisionResult> mEndEllipsisResult; ///< Non-replacement END result, alive only while elision is active.
  std::unique_ptr<AnchorColorData>    mAnchorColorData;   ///< Customized anchor colors allocated on demand.

  // Geometry / colors
  Vector4 mTextColor;          ///< The regular text color
  Vector2 mTextFitContentSize; ///< Size of Text fit content

  // Integer / enum-like values
  OperationsMask        mOperationsPending;         ///< Operations pending to be done to layout the text.
  Length                mMaximumNumberOfCharacters; ///< Maximum number of characters that can be inserted.
  int                   mMaxFitCandidateIndex;
  LayoutDirection::Type mLayoutDirection; ///< Current system language direction

  // Floating-point values
  float mCurrentLineSize;       ///< Used to store the MinLineSize set by user when TextFitCandidates is enabled.
  float mTextFitMinSize;        ///< Minimum Font Size for text fit. Default 10
  float mTextFitMaxSize;        ///< Maximum Font Size for text fit. Default 100
  float mTextFitStepSize;       ///< Step Size for font intervalse. Default 1
  float mUiScale;               ///< Scale value for text-specific UI metrics. Default 1.0
  float mFontSizeScale;         ///< Scale value for Font Size. Default 1.0
  float mMinFontSizeScale;      ///< Minimum scale value for Font Size. Default 0.01
  float mMaxFontSizeScale;      ///< Maximum scale value for Font Size. Default 10.0
  float mAdjustedFontSizeScale; ///< The final font size scale applied after clamping and system scaling.
  float mSystemFontSizeScale;   ///< System font size scale. Default 1.0
  float mDisabledColorOpacity;  ///< Color opacity when disabled.
  float mRenderScale;           ///< The render scale. Default 1.0

  mutable ClearFocusOnEscapeState
    mShouldClearFocusOnEscape : 3; ///< Whether text control should clear key input focus.
                                   ///< Make it mutable so we can update it at const method.

  CharacterDirection mIsTextDirectionRTL : 1;          ///< Whether the text direction is right to left or not
  bool               mRecalculateNaturalSize : 1;      ///< Whether the natural size needs to be recalculated.
  bool               mRecalculateLayoutSize : 1;       ///< Whether the layout size needs to be recalculated.
  bool               mRecalculateHeightForWidth : 1;   ///< Whether the height for width needs to be recalculated.
  bool               mIsEmittingTextChangedSignal : 1; ///< Whether TextChangedSignal is currently being emitted.
  bool               mClipboardHideEnabled : 1;        ///< Whether the ClipboardHide function work or not
  bool               mIsMarqueeEnabled : 1;            ///< Whether auto text scrolling is enabled.
  bool               mIsMarqueeMaxTextureExceeded : 1; ///< Whether auto text scrolling is exceed max texture size.
  bool               mUpdateTextDirection : 1;         ///< Whether the text direction needs to be updated.
  bool               mUnderlineSetByString : 1;        ///< Set when underline is set by string (legacy) instead of map
  bool               mShadowSetByString : 1;           ///< Set when shadow is set by string (legacy) instead of map
  bool               mOutlineSetByString : 1;          ///< Set when outline is set by string (legacy) instead of map
  bool               mFontStyleSetByString : 1;        ///< Set when font style is set by string (legacy) instead of map
  bool               mStrikethroughSetByString : 1;    ///< Set when strikethrough is set by string (legacy) instead of map
  bool               mSystemFontSizeScaleEnabled : 1;  ///< Whether the system font size scale is applied.
  bool               mTextFitEnabled : 1;              ///< Whether the text's fit is enabled.
  bool               mTextFitChanged : 1;              ///< Whether the text fit property has changed.
  bool               mTextFitCandidatesEnabled : 1;    ///< Whether the text's fit Candidates is enabled.
  bool               mIsLayoutDirectionChanged : 1;    ///< Whether the layout has changed.
  bool               mIsUserInteractionEnabled : 1;    ///< Whether the user interaction is enabled.
  bool               mProcessorRegistered : 1;         ///< Whether the text controller registered into processor or not.
  bool               mTextCutout : 1;                  ///< Whether the text cutout enabled.
  bool               mIsCursorInsetEnabled : 1;        ///< Whether the cursor inset is enabled.
  bool               mIsAsyncRendering : 1;            ///< whether asynchronous text rendering is enabled.

private:
  friend ControllerImplEventHandler;
  friend ControllerImplModelUpdater;
  friend SelectionHandleController;
};

} // namespace Text

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_TEXT_CONTROLLER_IMPL_H
