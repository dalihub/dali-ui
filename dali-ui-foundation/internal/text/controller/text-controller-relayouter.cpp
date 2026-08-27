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
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/math-utils.h>
#include <algorithm>
#include <cmath>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/controller/text-controller-event-handler.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl-model-updater.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-relayouter.h>
#include <dali-ui-foundation/internal/text/ellipsis/ellipsis-resolver.h>
#include <dali-ui-foundation/internal/text/layouts/layout-parameters.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-layout-data.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-placement.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-processing-source.h>
#include <dali-ui-foundation/internal/text/text-alignment.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_TEXT_PERFORMANCE_MARKER, false);
DALI_INIT_TRACE_FILTER(gTraceFilter2, DALI_TRACE_PERFORMANCE_MARKER, false);

constexpr float MAX_FLOAT = std::numeric_limits<float>::max();

float ConvertToEven(float value)
{
  int intValue(static_cast<int>(value));
  return static_cast<float>(intValue + (intValue & 1));
}

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

float ConvertPixelToPoint(float pixel)
{
  return pixel * 72.0f / GetDpi();
}

float ConvertPointToPixel(float point)
{
  // Pixel size = Point size * DPI / 72.f
  return point * GetDpi() / 72.0f;
}

} // namespace

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace
{
/**
 * @brief Calculate the sum of line heights for all lines.
 *
 * @param lines The vector of lines.
 * @return The sum of line heights.
 */
float CalculateLineHeightSum(const Vector<LineRun>& lines)
{
  float lineHeightSum = 0.0f;
  for(LineIndex index = 0u, count = static_cast<Dali::Ui::Text::LineIndex>(lines.Count()); index < count; ++index)
  {
    const bool isLastLine = (index + 1u == count);
    lineHeightSum += GetLineHeight(lines[index], isLastLine);
  }
  return lineHeightSum;
}

/**
 * @brief Get effective layout height for vertical alignment calculation.
 *
 * For editable multi-line text with a trailing empty line, this function returns
 * the sum of line heights instead of the raw layout height to avoid treating
 * the text as scrollable due to an overestimated layout height.
 *
 * @param impl Controller::Impl reference
 * @param layoutHeight The original layout height from visualModel->GetLayoutSize().height
 * @return Effective height for editable multi-line text with trailing empty line.
 *
 * Editable multi-line text with a trailing empty line may have an overestimated
 * raw layout height while relayout or measurement is in progress. In that case,
 * use the sum of actual line heights so layout-dependent queries and vertical
 * alignment do not treat the text as having an extra line.
 */
float GetEffectiveEditableLayoutHeight(Controller::Impl& impl, float layoutHeight)
{
  const bool      isEditable    = NULL != impl.mEventData;
  const bool      isMultiline   = impl.mLayoutEngine.GetLayout() == Layout::Engine::MULTI_LINE_BOX;
  ModelPtr        geometryModel = impl.GetEditableGeometryModel();
  VisualModelPtr& visualModel   = geometryModel->mVisualModel;

  if(!isEditable || !isMultiline || visualModel->mLines.Empty())
  {
    return layoutHeight;
  }

  const LineIndex lineCount = static_cast<Dali::Ui::Text::LineIndex>(visualModel->mLines.Count());
  const LineRun&  lastLine  = visualModel->mLines[lineCount - 1u];
  const bool      hasTrailingEmptyLine =
    lastLine.characterRun.numberOfCharacters == 0u &&
    lastLine.glyphRun.numberOfGlyphs == 0u;

  if(!hasTrailingEmptyLine)
  {
    return layoutHeight;
  }

  const float lineHeightSum = CalculateLineHeightSum(visualModel->mLines);

  // Return the smaller value to prevent incorrect scrollable detection.
  return (lineHeightSum < layoutHeight) ? lineHeightSum : layoutHeight;
}

bool IsReplacementElideEnabled(const Controller::Impl& impl)
{
  bool enabled = impl.mModel->mElideEnabled;
  if(impl.mEventData != nullptr)
  {
    if(impl.mEventData->mPlaceholderEllipsisFlag && impl.IsShowingPlaceholderText())
    {
      enabled = impl.mEventData->mIsPlaceholderElideEnabled;
    }
    else if(EventData::INACTIVE != impl.mEventData->mState)
    {
      enabled = false;
    }
  }
  return enabled;
}

void UpdateReplacementRenderState(Controller::Impl& impl, const Size& contentSize, Length maximumNumberOfLines,
                                  bool& maximumNumberOfLinesExceeded)
{
  if(!impl.HasValidReplacementSource())
  {
    return;
  }

  if(impl.mHiddenInput && impl.mEventData && impl.mHiddenInput->GetMode() != HiddenText::Mode::NONE)
  {
    const ReplacementRenderState* state = impl.GetReplacementRenderStatePtr();
    if(state && state->attempted)
    {
      impl.InvalidateReplacementRenderState();
    }
    return;
  }

  const ReplacementSourceSnapshot& source = impl.GetReplacementSourceSnapshot();

  // START/MIDDLE are not released for replacement content. Preserve the
  // projected atomic boxes and use CLIP instead of exposing their underlying
  // source text through a non-replacement ellipsis pass.
  const bool useReplacementClipFallback =
    IsReplacementElideEnabled(impl) && impl.mModel->mEllipsisPosition != EllipsisPosition::END;

  ReplacementRenderState&               result               = impl.GetOrCreateReplacementRenderState();
  TextAbstraction::BidirectionalSupport bidirectionalSupport = TextAbstraction::BidirectionalSupport::Get();
  // Detach the View before releasing a previous projected model. Any rejected
  // or failed projection below therefore leaves the controller on its immutable
  // original model instead of retaining a stale pointer until the next relayout.
  impl.mView.SetVisualModel(impl.mModel->mVisualModel);
  impl.mView.SetLogicalModel(impl.mModel->mLogicalModel);
  impl.mView.SetFinalElisionResult(nullptr);
  result.Clear(bidirectionalSupport);
  result.attempted        = true;
  result.sourceRevision   = source.sourceRevision;
  result.layoutGeneration = impl.NextReplacementLayoutGeneration();
  result.projection       = ReplacementProjection::Build(impl.mModel->mLogicalModel->mText,
                                                         source.runs,
                                                         impl.GetEffectiveTextScale());
  if(!result.projection.HasReplacements())
  {
    return;
  }

  ProjectedTextProcessingSource projectedSource;
  if(!PrepareProjectedTextProcessingSource(*impl.mModel, result.projection, projectedSource))
  {
    return;
  }

  result.processingModel = Model::New();
  CopyTextProcessingProperties(*impl.mModel, *result.processingModel);
  result.processingModel->mVisualModel->mControlSize = contentSize;

  const uint32_t pointsPerUnit = impl.GetFontClient().GetNumberOfPointsPerOneUnitOfPointSize();
  float          pointSize     = static_cast<float>(TextAbstraction::FontClient::DEFAULT_POINT_SIZE) /
                    static_cast<float>(pointsPerUnit);
  if(impl.mFontDefaults != nullptr)
  {
    pointSize = (impl.IsTextFitEnabled() || impl.IsTextFitCandidatesEnabled())
                  ? impl.mFontDefaults->mFitPointSize
                  : impl.mFontDefaults->mDefaultPointSize * impl.GetEffectiveTextScale();
  }

  const TextAbstraction::PointSize26Dot6 fontPointSize =
    static_cast<TextAbstraction::PointSize26Dot6>(pointSize * pointsPerUnit);
  FontId defaultFontId = 0u;
  if(impl.mFontDefaults != nullptr)
  {
    defaultFontId = impl.mFontDefaults->GetFontId(impl.GetFontClient(), pointSize);
  }
  else
  {
    TextAbstraction::FontDescription defaultFontDescription;
    defaultFontId = impl.GetFontClient().GetFontId(defaultFontDescription, fontPointSize);
  }

  const Controller::OperationsMask updateOperations = static_cast<Controller::OperationsMask>(
    Controller::GET_LINE_BREAKS | Controller::GET_SCRIPTS | Controller::VALIDATE_FONTS |
    Controller::BIDI_INFO | Controller::SHAPE_TEXT | Controller::GET_GLYPH_METRICS | Controller::COLOR);
  ControllerImplModelUpdater::Update(impl,
                                     projectedSource.source,
                                     *result.processingModel,
                                     updateOperations);

  VisualModel& projectedVisual = *result.processingModel->mVisualModel;
  const Length glyphCount      = static_cast<Dali::Ui::Text::Length>(projectedVisual.mGlyphs.Count());
  projectedVisual.mGlyphPositions.Resize(glyphCount);
  result.processingModel->mElideEnabled = IsReplacementElideEnabled(impl) && !useReplacementClipFallback;
  projectedVisual.SetTextElideEnabled(result.processingModel->mElideEnabled);
  projectedVisual.SetEllipsisPosition(result.processingModel->mEllipsisPosition);

  ReplacementLayoutData replacementLayoutData;
  replacementLayoutData.runs                = &result.projection.GetReplacementRuns();
  replacementLayoutData.defaultFontId       = defaultFontId;
  replacementLayoutData.horizontalAlignment = result.processingModel->mHorizontalAlignment;
  replacementLayoutData.layoutDirection     = impl.mLayoutDirection;
  replacementLayoutData.matchLayoutDirection =
    result.processingModel->mLayoutDirectionMode != LayoutDirectionMode::CONTENTS;
  Layout::Parameters layoutParameters(contentSize,
                                      result.processingModel,
                                      impl.GetFontClient(),
                                      bidirectionalSupport);
  layoutParameters.numberOfGlyphs         = glyphCount;
  layoutParameters.estimatedNumberOfLines = 1u;
  layoutParameters.maximumNumberOfLines   = maximumNumberOfLines;
  layoutParameters.replacementLayoutData  = &replacementLayoutData;
  const Vector<Character>& processingText = result.processingModel->mLogicalModel->mText;
  layoutParameters.isLastNewParagraph     = !processingText.Empty() &&
                                        TextAbstraction::IsNewParagraph(processingText[processingText.Count() - 1u]);

  const float fontPixelSize = ConvertPointToPixel(pointSize);
  impl.mLayoutEngine.SetFontPixelSize(fontPixelSize);
  bool marqueeEnabled = impl.mIsMarqueeEnabled;
  impl.mLayoutEngine.LayoutText(layoutParameters,
                                result.layoutSize,
                                result.processingModel->mElideEnabled,
                                marqueeEnabled,
                                impl.mIsMarqueeMaxTextureExceeded,
                                false,
                                result.processingModel->mEllipsisPosition);
  maximumNumberOfLinesExceeded |= layoutParameters.maximumNumberOfLinesExceeded;
  impl.mIsMarqueeEnabled = marqueeEnabled;
  projectedVisual.SetLayoutSize(result.layoutSize);

  float alignmentOffset = 0.0f;
  impl.mLayoutEngine.Align(contentSize,
                           0u,
                           static_cast<Dali::Ui::Text::Length>(processingText.Count()),
                           result.processingModel->mHorizontalAlignment,
                           projectedVisual.mLines,
                           alignmentOffset,
                           impl.mLayoutDirection,
                           result.processingModel->mLayoutDirectionMode != LayoutDirectionMode::CONTENTS);

  // Synchronous Label alignment is applied by the TextVisual transform. Keep
  // the active model's scroll in the same coordinate space as the original
  // model; ReplacementPlacement already carries its independent vertical
  // offset for the registered inline visual. Async layout applies alignment
  // inside its worker-local model instead.
  result.processingModel->mScrollPosition = impl.mModel->mScrollPosition;
  impl.mView.SetVisualModel(result.processingModel->mVisualModel);
  impl.mView.SetLogicalModel(result.processingModel->mLogicalModel);
  impl.mView.ResolveFinalElision(impl.GetFontClient(), result.finalElision, result.layoutGeneration);
  impl.mView.SetFinalElisionResult(&result.finalElision);
  ExtractReplacementPlacements(*result.processingModel,
                               result.projection,
                               result.finalElision,
                               impl.GetFontClient(),
                               defaultFontId,
                               result.placements);
}
} // anonymous namespace

Size Controller::Relayouter::CalculateLayoutSizeOnRequiredControllerSize(Controller&           controller,
                                                                         const Size&           requestedControllerSize,
                                                                         Length                maximumNumberOfLines,
                                                                         const OperationsMask& requestedOperationsMask)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->CalculateLayoutSizeOnRequiredControllerSize\n");
  Size calculatedLayoutSize;

  Controller::Impl& impl        = *controller.mImpl;
  ModelPtr&         model       = impl.mModel;
  VisualModelPtr&   visualModel = model->mVisualModel;

  // Operations that can be done only once until the text changes.
  const OperationsMask onlyOnceOperations = static_cast<OperationsMask>(
    CONVERT_TO_UTF32 | GET_SCRIPTS | VALIDATE_FONTS | GET_LINE_BREAKS | BIDI_INFO | SHAPE_TEXT | GET_GLYPH_METRICS);

  const OperationsMask sizeOperations = static_cast<OperationsMask>(LAYOUT | ALIGN | REORDER);

  // Set the update info to relayout the whole text.
  TextUpdateInfo& textUpdateInfo = impl.mTextUpdateInfo;
  if((0 == textUpdateInfo.mNumberOfCharactersToAdd) && (0 == textUpdateInfo.mPreviousNumberOfCharacters) &&
     ((visualModel->mControlSize.width < Math::MACHINE_EPSILON_1000) ||
      (visualModel->mControlSize.height < Math::MACHINE_EPSILON_1000)))
  {
    textUpdateInfo.mNumberOfCharactersToAdd = static_cast<Dali::Ui::Text::Length>(model->mLogicalModel->mText.Count());
  }
  textUpdateInfo.mParagraphCharacterIndex     = 0u;
  textUpdateInfo.mRequestedNumberOfCharacters = static_cast<Dali::Ui::Text::Length>(model->mLogicalModel->mText.Count());

  // Get a reference to the pending operations member
  OperationsMask& operationsPending = impl.mOperationsPending;

  // Store the actual control's size to restore later.
  const Size actualControlSize = visualModel->mControlSize;

  // This is to keep Index to the first character to be updated.
  // Then restore it after calling Clear method.
  auto updateInfoCharIndexBackup = textUpdateInfo.mCharacterIndex;

  // Whether the text control is editable
  const bool isEditable = NULL != impl.mEventData;

  if(!isEditable)
  {
    if(NO_OPERATION != (VALIDATE_FONTS & operationsPending) &&
       textUpdateInfo.mCharacterIndex == static_cast<CharacterIndex>(-1))
    {
      impl.ClearFontData();
      updateInfoCharIndexBackup = textUpdateInfo.mCharacterIndex;
    }

    impl.UpdateModel(onlyOnceOperations);

    // Layout the text for the new width.
    operationsPending = static_cast<OperationsMask>(operationsPending | requestedOperationsMask);

    DoRelayout(impl, requestedControllerSize, maximumNumberOfLines,
               static_cast<OperationsMask>(onlyOnceOperations | requestedOperationsMask), calculatedLayoutSize);

    textUpdateInfo.Clear();
    textUpdateInfo.mClearAll = true;

    // Do not do again the only once operations.
    operationsPending = static_cast<OperationsMask>(operationsPending & ~onlyOnceOperations);
  }
  else
  {
    // Layout the text for the new width.
    // Apply the pending operations, requested operations and the only once operations.
    // Then remove onlyOnceOperations
    operationsPending = static_cast<OperationsMask>(operationsPending | requestedOperationsMask | onlyOnceOperations);

    // Make sure the model is up-to-date before layouting
    impl.UpdateModel(static_cast<OperationsMask>(operationsPending & ~UPDATE_LAYOUT_SIZE));

    DoRelayout(impl, requestedControllerSize, maximumNumberOfLines,
               static_cast<OperationsMask>(operationsPending & ~UPDATE_LAYOUT_SIZE), calculatedLayoutSize);

    // Clear the update info. This info will be set the next time the text is updated.
    textUpdateInfo.Clear();

    // TODO: Refactor "DoRelayout" and extract common code of size calculation without modifying attributes of
    // mVisualModel,
    // TODO: then calculate GlyphPositions. Lines, Size, Layout for Natural-Size
    // TODO: and utilize the values in OperationsPending and TextUpdateInfo without changing the original one.
    // TODO: Also it will improve performance because there is no need todo FullRelyout on the next need for layouting.
  }

  // FullRelayoutNeeded should be true because DoRelayout is MAX_FLOAT, MAX_FLOAT.
  // By this no need to take backup and restore it.
  textUpdateInfo.mFullRelayoutNeeded = true;

  // Restore mCharacterIndex. Because "Clear" set it to the maximum integer.
  // The "CalculateTextUpdateIndices" does not work proprely because the mCharacterIndex will be greater than
  // mPreviousNumberOfCharacters. Which apply an assumption to update only the last  paragraph. That could cause many of
  // out of index crashes.
  textUpdateInfo.mCharacterIndex = updateInfoCharIndexBackup;

  // Do the size related operations again.
  operationsPending = static_cast<OperationsMask>(operationsPending | sizeOperations);

  // Restore the actual control's size.
  visualModel->mControlSize = actualControlSize;

  // A valid projected model is the authoritative measurement result. The
  // immutable original model remains available for logical/semantic queries.
  const ReplacementRenderState& replacement = impl.GetReplacementRenderState();
  if(replacement.processingModel && replacement.projection.HasReplacements())
  {
    calculatedLayoutSize = replacement.layoutSize;
  }

  return calculatedLayoutSize;
}

Vector3 Controller::Relayouter::GetNaturalSize(Controller& controller, bool convertToEven)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->Controller::GetNaturalSize\n");
  DALI_TRACE_SCOPE_WITH_FORMAT(gTraceFilter, "DALI_TEXT_GET_NATURAL_SIZE", "[%p]", static_cast<void*>(&controller));
  Vector3 naturalSizeVec3;

  // Make sure the model is up-to-date before layouting
  EventHandler::ProcessModifyEvents(controller);

  Controller::Impl& impl        = *controller.mImpl;
  ModelPtr&         model       = impl.mModel;
  VisualModelPtr&   visualModel = model->mVisualModel;

  if(impl.mRecalculateNaturalSize)
  {
    Size naturalSize;

    // Layout the text for the new width.
    OperationsMask requestedOperationsMask  = static_cast<OperationsMask>(LAYOUT | REORDER);
    Size           sizeMaxWidthAndMaxHeight = Size(MAX_FLOAT, MAX_FLOAT);

    naturalSize =
      CalculateLayoutSizeOnRequiredControllerSize(controller, sizeMaxWidthAndMaxHeight,
                                                  impl.mMaximumNumberOfLines, requestedOperationsMask);

    // Editable multi-line layout can overestimate the height when the last line is empty.
    // Use the effective height to correct the overestimation during TextChangedSignal emission.
    if(impl.mIsEmittingTextChangedSignal)
    {
      naturalSize.height = GetEffectiveEditableLayoutHeight(impl, naturalSize.height);
    }

    // Stores the natural size to avoid recalculate it again
    // unless the text/style changes.
    visualModel->SetNaturalSize(naturalSize);
    naturalSizeVec3 = naturalSize;

    impl.mRecalculateNaturalSize = false;

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::GetNaturalSize calculated %f,%f,%f\n", naturalSizeVec3.x,
                  naturalSizeVec3.y, naturalSizeVec3.z);
  }
  else
  {
    naturalSizeVec3 = visualModel->GetNaturalSize();

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::GetNaturalSize cached %f,%f,%f\n", naturalSizeVec3.x,
                  naturalSizeVec3.y, naturalSizeVec3.z);
  }

  if(convertToEven)
  {
    naturalSizeVec3.x = ConvertToEven(naturalSizeVec3.x);
    naturalSizeVec3.y = ConvertToEven(naturalSizeVec3.y);
  }

  return naturalSizeVec3;
}

bool Controller::Relayouter::CheckForTextFit(Controller& controller, float pointSize, const Size& layoutSize)
{
  Size              textSize;
  Controller::Impl& impl            = *controller.mImpl;
  TextUpdateInfo&   textUpdateInfo  = impl.mTextUpdateInfo;
  impl.mFontDefaults->mFitPointSize = pointSize;
  impl.mFontDefaults->sizeDefined   = true;
  impl.ClearFontData();

  // Operations that can be done only once until the text changes.
  const OperationsMask onlyOnceOperations = static_cast<OperationsMask>(
    CONVERT_TO_UTF32 | GET_SCRIPTS | VALIDATE_FONTS | GET_LINE_BREAKS | BIDI_INFO | SHAPE_TEXT | GET_GLYPH_METRICS);

  textUpdateInfo.mParagraphCharacterIndex     = 0u;
  textUpdateInfo.mRequestedNumberOfCharacters = static_cast<Dali::Ui::Text::Length>(impl.mModel->mLogicalModel->mText.Count());

  // Make sure the model is up-to-date before layouting
  impl.UpdateModel(onlyOnceOperations);

  bool layoutTooSmall               = false;
  bool maximumNumberOfLinesExceeded = false;
  DoRelayout(impl, Size(layoutSize.width, MAX_FLOAT), impl.mMaximumNumberOfLines,
             static_cast<OperationsMask>(onlyOnceOperations | LAYOUT), textSize, layoutTooSmall,
             maximumNumberOfLinesExceeded);

  // Clear the update info. This info will be set the next time the text is updated.
  textUpdateInfo.Clear();
  textUpdateInfo.mClearAll = true;

  if(layoutTooSmall || maximumNumberOfLinesExceeded ||
     textSize.width > layoutSize.width || textSize.height > layoutSize.height)
  {
    return false;
  }
  return true;
}

void Controller::Relayouter::FitCandidatesPointSizeForLayout(Controller& controller, const Size& layoutSize)
{
  Controller::Impl&              impl    = *controller.mImpl;
  Controller::Impl::TextFitData& textFit = impl.GetOrCreateTextFitData();

  const OperationsMask operations = impl.mOperationsPending;
  if(NO_OPERATION != (UPDATE_LAYOUT_SIZE & operations) || textFit.contentSize != layoutSize)
  {
    DALI_TRACE_SCOPE_WITH_FORMAT(gTraceFilter, "DALI_TEXT_FIT_CANDIDATES_LAYOUT", "[%p]", static_cast<void*>(&controller));

    Dali::Vector<Ui::Text::Fit::Candidate> fitCandidates  = textFit.candidates;
    const int                              candidateCount = static_cast<int>(fitCandidates.Count());

    if(candidateCount == 0)
    {
      DALI_LOG_ERROR("fitCandidates is empty\n");
      return;
    }

    ModelPtr& model          = impl.mModel;
    bool      actualEllipsis = model->mElideEnabled;
    model->mElideEnabled     = false;

    // Sort in ascending order by font size.
    std::sort(fitCandidates.Begin(), fitCandidates.End(), compareByPointSize);

    // Decide whether to use binary search.
    // If lineHeight is not sorted in ascending order,
    // binary search cannot guarantee that it will always find the best value.
    bool  binarySearch       = true;
    float prevLineHeight     = 0.0f;
    float effectiveTextScale = impl.GetEffectiveTextScale();

    for(auto it = fitCandidates.Begin(); it != fitCandidates.End(); ++it)
    {
      const float candidateLineHeight = it->GetLineHeight();
      if(prevLineHeight > candidateLineHeight)
      {
        binarySearch = false;
        break;
      }
      prevLineHeight = candidateLineHeight;
    }

    // Set the first candidate (minimum font size) as the default best value.
    // If the search does not find an optimal value, the minimum point size will be used.
    const Ui::Text::Fit::Candidate& firstCandidate        = fitCandidates[0];
    bool                            bestSizeUpdatedLatest = false;
    float                           bestPointSize         = ConvertPixelToPoint(firstCandidate.GetFontSize()) * effectiveTextScale;
    float                           bestLineHeight        = firstCandidate.GetLineHeight();

    if(binarySearch)
    {
      int left  = 0;
      int right = candidateCount - 1;

      while(left <= right)
      {
        const int                       mid            = left + (right - left) / 2;
        const Ui::Text::Fit::Candidate& candidate      = fitCandidates[mid];
        const float                     testPointSize  = ConvertPixelToPoint(candidate.GetFontSize()) * effectiveTextScale;
        const float                     testLineHeight = candidate.GetLineHeight();
        impl.SetDefaultLineSize(testLineHeight);

        if(CheckForTextFit(controller, testPointSize, layoutSize))
        {
          bestSizeUpdatedLatest = true;
          bestPointSize         = testPointSize;
          bestLineHeight        = testLineHeight;
          left                  = mid + 1;
        }
        else
        {
          bestSizeUpdatedLatest = false;
          right                 = mid - 1;
        }
      }
    }
    else
    {
      // If binary search is not possible, search sequentially from the largest font size.
      for(auto it = fitCandidates.End(); it != fitCandidates.Begin();)
      {
        --it;

        const float testPointSize  = ConvertPixelToPoint(it->GetFontSize()) * effectiveTextScale;
        const float testLineHeight = it->GetLineHeight();
        impl.SetDefaultLineSize(testLineHeight);

        if(CheckForTextFit(controller, testPointSize, layoutSize))
        {
          bestSizeUpdatedLatest = true;
          bestPointSize         = testPointSize;
          bestLineHeight        = testLineHeight;
          break;
        }
        else
        {
          bestSizeUpdatedLatest = false;
        }
      }
    }

    // Best point size was not updated. Re-run so the text fit is really applied.
    if(!bestSizeUpdatedLatest)
    {
      impl.SetDefaultLineSize(bestLineHeight);
      CheckForTextFit(controller, bestPointSize, layoutSize);
    }

    model->mElideEnabled              = actualEllipsis;
    impl.mFontDefaults->mFitPointSize = bestPointSize;
    impl.mFontDefaults->sizeDefined   = true;
    impl.ClearFontData();
  }
}

void Controller::Relayouter::FitPointSizeforLayout(Controller& controller, const Size& layoutSize)
{
  Controller::Impl&              impl    = *controller.mImpl;
  Controller::Impl::TextFitData& textFit = impl.GetOrCreateTextFitData();

  const OperationsMask operations = impl.mOperationsPending;
  if(NO_OPERATION != (UPDATE_LAYOUT_SIZE & operations) || textFit.contentSize != layoutSize)
  {
    DALI_TRACE_SCOPE_WITH_FORMAT(gTraceFilter, "DALI_TEXT_FIT_LAYOUT", "[%p]", static_cast<void*>(&controller));
    ModelPtr& model = impl.mModel;

    bool  actualellipsis      = model->mElideEnabled;
    float effectiveTextScale  = impl.GetEffectiveTextScale();
    float minPointSize        = textFit.minSize * effectiveTextScale;
    float maxPointSize        = textFit.maxSize * effectiveTextScale;
    float pointInterval       = textFit.stepSize * effectiveTextScale;
    float currentFitPointSize = impl.mFontDefaults->mFitPointSize;
    bool  isMultiLine         = impl.mLayoutEngine.GetLayout() == Layout::Engine::MULTI_LINE_BOX;

    model->mElideEnabled = false;
    float bestPointSize  = minPointSize;

    // check zero value
    if(pointInterval < 1.f)
    {
      pointInterval = 1.0f;
    }

    uint32_t pointSizeRange = static_cast<uint32_t>(ceil((maxPointSize - minPointSize) / pointInterval));

    if(isMultiLine || pointSizeRange < 3)
    {
      // Ensure minPointSize + pointSizeRange * pointInverval >= maxPointSize
      while(minPointSize + static_cast<float>(pointSizeRange) * pointInterval < maxPointSize)
      {
        ++pointSizeRange;
      }

      uint32_t bestSizeIndex = 0;
      uint32_t minIndex      = bestSizeIndex + 1u;
      uint32_t maxIndex      = pointSizeRange + 1u;

      bool bestSizeUpdatedLatest = false;
      // Find best size as binary search.
      // Range format as [l r). (left closed, right opened)
      // It mean, we already check all i < l is valid, and r <= i is invalid.
      // Below binary search will check m = (l+r)/2 point.
      // Search area sperate as [l m) or [m+1 r)
      //
      // Basically, we can assume that 0 (minPointSize) is always valid.
      // Now, we will check [1 pointSizeRange] range s.t. pointSizeRange mean the maxPointSize
      while(minIndex < maxIndex)
      {
        uint32_t    testIndex = minIndex + ((maxIndex - minIndex) >> 1u);
        const float testPointSize =
          std::min(maxPointSize, minPointSize + static_cast<float>(testIndex) * pointInterval);

        if(CheckForTextFit(controller, testPointSize, layoutSize))
        {
          bestSizeUpdatedLatest = true;

          bestSizeIndex = testIndex;
          minIndex      = testIndex + 1u;
        }
        else
        {
          bestSizeUpdatedLatest = false;
          maxIndex              = testIndex;
        }
      }
      bestPointSize = std::min(maxPointSize, minPointSize + static_cast<float>(bestSizeIndex) * pointInterval);

      // Best point size was not updated. re-run so the TextFit should be fitted really.
      if(!bestSizeUpdatedLatest)
      {
        CheckForTextFit(controller, bestPointSize, layoutSize);
      }
    }
    else
    {
      // assume textSize = a * pointSize + b, finding a and b.
      Size                 textSize;
      TextUpdateInfo&      textUpdateInfo = impl.mTextUpdateInfo;
      const OperationsMask onlyOnceOperations =
        static_cast<OperationsMask>(CONVERT_TO_UTF32 | GET_SCRIPTS | VALIDATE_FONTS | GET_LINE_BREAKS | BIDI_INFO |
                                    SHAPE_TEXT | GET_GLYPH_METRICS);

      float resultBasedX[2];
      float resultBasedY[2];
      float tmpPointSize[2] = {minPointSize, maxPointSize};

      // Calculate a and b by creating simultaneous equations with two calculations.
      for(int i = 0; i < 2; i++)
      {
        impl.mFontDefaults->mFitPointSize = tmpPointSize[i];
        impl.mFontDefaults->sizeDefined   = true;
        impl.ClearFontData();

        textUpdateInfo.mParagraphCharacterIndex     = 0u;
        textUpdateInfo.mRequestedNumberOfCharacters = static_cast<Dali::Ui::Text::Length>(impl.mModel->mLogicalModel->mText.Count());

        // Make sure the model is up-to-date before layouting
        impl.UpdateModel(onlyOnceOperations);

        DoRelayout(impl, Size(layoutSize.width, MAX_FLOAT), impl.mMaximumNumberOfLines,
                   static_cast<OperationsMask>(onlyOnceOperations | LAYOUT), textSize);

        // Clear the update info. This info will be set the next time the text is updated.
        textUpdateInfo.Clear();
        textUpdateInfo.mClearAll = true;

        resultBasedX[i] = textSize.x;
        resultBasedY[i] = textSize.y;
      }

      float aBasedX = (resultBasedX[1] - resultBasedX[0]) / (tmpPointSize[1] - tmpPointSize[0]);
      float bBasedX = resultBasedX[1] - aBasedX * tmpPointSize[1];
      aBasedX       = std::max(aBasedX, Dali::Math::MACHINE_EPSILON_1000);

      float aBasedY = (resultBasedY[1] - resultBasedY[0]) / (tmpPointSize[1] - tmpPointSize[0]);
      float bBasedY = resultBasedY[1] - aBasedY * tmpPointSize[1];
      aBasedY       = std::max(aBasedY, Dali::Math::MACHINE_EPSILON_1000);

      float bestPointSizeBasedX = (layoutSize.x - bBasedX) / aBasedX;
      float bestPointSizeBasedY = (layoutSize.y - bBasedY) / aBasedY;

      bestPointSize = std::min(bestPointSizeBasedX, bestPointSizeBasedY);
      bestPointSize = std::min(std::max(bestPointSize, minPointSize), maxPointSize);
      bestPointSize = std::floor((bestPointSize - minPointSize) / pointInterval) * pointInterval + minPointSize;

      if(CheckForTextFit(controller, bestPointSize, layoutSize))
      {
        while(bestPointSize + pointInterval <= maxPointSize &&
              CheckForTextFit(controller, bestPointSize + pointInterval, layoutSize))
        {
          bestPointSize += pointInterval;
        }
      }
      else if(bestPointSize - pointInterval >= minPointSize)
      {
        do
        {
          bestPointSize -= pointInterval;
        } while(bestPointSize - pointInterval >= minPointSize &&
                !CheckForTextFit(controller, bestPointSize, layoutSize));
      }
    }

    model->mElideEnabled = actualellipsis;
    if(!Dali::Equals(currentFitPointSize, bestPointSize))
    {
      textFit.changed = true;
    }

    impl.mFontDefaults->mFitPointSize = bestPointSize;
    impl.mFontDefaults->sizeDefined   = true;
    impl.ClearFontData();
  }
}

float Controller::Relayouter::GetHeightForWidth(Controller& controller, float width)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->Controller::GetHeightForWidth %p width %f\n", &controller, width);
  DALI_TRACE_SCOPE_WITH_FORMAT(gTraceFilter, "DALI_TEXT_GET_HEIGHT_FOR_WIDTH", "[%p]", static_cast<void*>(&controller));

  // Make sure the model is up-to-date before layouting
  EventHandler::ProcessModifyEvents(controller);

  Controller::Impl& impl           = *controller.mImpl;
  ModelPtr&         model          = impl.mModel;
  VisualModelPtr&   visualModel    = model->mVisualModel;
  TextUpdateInfo&   textUpdateInfo = impl.mTextUpdateInfo;

  // Get cached value.
  Size layoutSize = visualModel->GetHeightForWidth();

  const bool isWidthChanged =
    fabsf(width - layoutSize.width) > Math::MACHINE_EPSILON_1000;

  if(isWidthChanged ||
     impl.mRecalculateHeightForWidth ||
     textUpdateInfo.mFullRelayoutNeeded ||
     textUpdateInfo.mClearAll)
  {
    // Layout the text for the new width.
    OperationsMask requestedOperationsMask        = static_cast<OperationsMask>(LAYOUT | ALIGN);
    Size           sizeRequestedWidthAndMaxHeight = Size(width, MAX_FLOAT);

    layoutSize = CalculateLayoutSizeOnRequiredControllerSize(controller,
                                                             sizeRequestedWidthAndMaxHeight,
                                                             impl.mMaximumNumberOfLines,
                                                             requestedOperationsMask);

    // Editable multi-line layout can overestimate the height when the last line is empty.
    // Use the effective height to correct the overestimation during TextChangedSignal emission.
    if(impl.mIsEmittingTextChangedSignal)
    {
      layoutSize.height = GetEffectiveEditableLayoutHeight(impl, layoutSize.height);
    }

    // The calculated layout width may not be the same as the requested width.
    // For cache efficiency, the requested width is stored.
    layoutSize.width = width;
    visualModel->SetHeightForWidth(layoutSize);
    impl.mRecalculateHeightForWidth = false;

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::GetHeightForWidth calculated %f\n", layoutSize.height);
  }
  else
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::GetHeightForWidth cached %f\n", layoutSize.height);
  }

  return layoutSize.height;
}

Vector2 Controller::Relayouter::CalculateLayoutSize(Controller& controller, float width, float height, bool forceUpdate)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->Controller::CalculateLayoutSize %p width %f height %f\n", &controller,
                width, height);
  DALI_TRACE_SCOPE_WITH_FORMAT(gTraceFilter, "DALI_TEXT_CALCULATE_LAYOUT_SIZE", "[%p]",
                               static_cast<void*>(&controller));
  Vector2 layoutSize;

  // Make sure the model is up-to-date before layouting
  EventHandler::ProcessModifyEvents(controller);

  Controller::Impl& impl        = *controller.mImpl;
  ModelPtr&         model       = impl.mModel;
  VisualModelPtr&   visualModel = model->mVisualModel;

  if(impl.mRecalculateLayoutSize || forceUpdate)
  {
    // Layout the text for the new width.
    OperationsMask requestedOperationsMask      = static_cast<OperationsMask>(LAYOUT | ALIGN);
    Size           sizeFixedWidthAndFixedHeight = Size(width, height);

    layoutSize =
      CalculateLayoutSizeOnRequiredControllerSize(controller, sizeFixedWidthAndFixedHeight,
                                                  impl.mMaximumNumberOfLines, requestedOperationsMask);

    // Stores the layout size to avoid recalculate it again
    visualModel->SetCachedLayoutSize(layoutSize);

    impl.mRecalculateLayoutSize = forceUpdate;
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::CalculateLayoutSize calculated %f,%f\n", layoutSize.x,
                  layoutSize.y);
  }
  else
  {
    layoutSize = visualModel->GetCachedLayoutSize();
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::CalculateLayoutSize cached %f,%f\n", layoutSize.x,
                  layoutSize.y);
  }

  return layoutSize;
}

Controller::UpdateTextType Controller::Relayouter::Relayout(Controller& controller, const Size& size,
                                                            Dali::LayoutDirection::Type layoutDirection)
{
  Controller::Impl& impl           = *controller.mImpl;
  ModelPtr&         model          = impl.mModel;
  VisualModelPtr&   visualModel    = model->mVisualModel;
  TextUpdateInfo&   textUpdateInfo = impl.mTextUpdateInfo;

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->Controller::Relayout %p size %f,%f, marquee[%s]\n", &controller,
                size.width, size.height, impl.mIsMarqueeEnabled ? "true" : "false");
  DALI_TRACE_SCOPE_WITH_FORMAT(gTraceFilter, "DALI_TEXT_RELAYOUT", "[%p]", static_cast<void*>(&controller));

  UpdateTextType updateTextType = NONE_UPDATED;

  if((size.width < Math::MACHINE_EPSILON_1000) || (size.height < Math::MACHINE_EPSILON_1000))
  {
    if(0u != visualModel->mGlyphPositions.Count())
    {
      visualModel->mGlyphPositions.Clear();
      updateTextType = MODEL_UPDATED;
    }

    // Clear the update info. This info will be set the next time the text is updated.
    textUpdateInfo.Clear();

    // Not worth to relayout if width or height is equal to zero.
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::Relayout (skipped)\n");

    // When the Relayout size becomes 0, the size should be stored.
    // Since we check whether it is a new size with mControlSize,
    // if the size is 0 and then relayout to the same size as before, the text will not be updated.
    visualModel->mControlSize = size;

    return updateTextType;
  }

  // Whether a new size has been set.
  const bool newSize = (size != visualModel->mControlSize);

  // Get a reference to the pending operations member
  OperationsMask& operationsPending = impl.mOperationsPending;

  if(newSize)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "new size (previous size %f,%f)\n", visualModel->mControlSize.width,
                  visualModel->mControlSize.height);

    if((0 == textUpdateInfo.mNumberOfCharactersToAdd) && (0 == textUpdateInfo.mPreviousNumberOfCharacters) &&
       ((visualModel->mControlSize.width < Math::MACHINE_EPSILON_1000) ||
        (visualModel->mControlSize.height < Math::MACHINE_EPSILON_1000)))
    {
      textUpdateInfo.mNumberOfCharactersToAdd = static_cast<Dali::Ui::Text::Length>(model->mLogicalModel->mText.Count());
    }

    // Layout operations that need to be done if the size changes.
    operationsPending = static_cast<OperationsMask>(operationsPending | LAYOUT | ALIGN | UPDATE_LAYOUT_SIZE | REORDER);
    // Set the update info to relayout the whole text.
    textUpdateInfo.mFullRelayoutNeeded = true;
    textUpdateInfo.mCharacterIndex     = 0u;

    // Store the size used to layout the text.
    visualModel->mControlSize = size;
  }

  // Whether there are modify events.
  if(0u != impl.mModifyEvents.Count())
  {
    // Style operations that need to be done if the text is modified.
    operationsPending = static_cast<OperationsMask>(operationsPending | COLOR);
  }

  // Set the update info to elide the text.
  if(model->mElideEnabled || ((NULL != impl.mEventData) && impl.mEventData->mIsPlaceholderElideEnabled))
  {
    // Update Text layout for applying elided
    operationsPending                  = static_cast<OperationsMask>(operationsPending | ALIGN | LAYOUT | UPDATE_LAYOUT_SIZE | REORDER);
    textUpdateInfo.mFullRelayoutNeeded = true;
    textUpdateInfo.mCharacterIndex     = 0u;
  }

  bool layoutDirectionChanged = false;
  if(impl.mLayoutDirection != layoutDirection)
  {
    // Flag to indicate that the layout direction has changed.
    layoutDirectionChanged = true;
    // Clear the update info. This info will be set the next time the text is updated.
    textUpdateInfo.mClearAll = true;
    // Apply modifications to the model
    // Shape the text again is needed because characters like '()[]{}' have to be mirrored and the glyphs generated
    // again.
    operationsPending     = static_cast<OperationsMask>(operationsPending | GET_GLYPH_METRICS | SHAPE_TEXT |
                                                        UPDATE_DIRECTION | ALIGN | LAYOUT | BIDI_INFO | REORDER);
    impl.mLayoutDirection = layoutDirection;
  }

  // Make sure the model is up-to-date before layouting.
  EventHandler::ProcessModifyEvents(controller);
  bool updated = impl.UpdateModel(operationsPending);

  // Layout the text.
  Size layoutSize;
  updated = DoRelayout(impl, size, impl.mMaximumNumberOfLines, operationsPending, layoutSize) || updated;

  const ReplacementRenderState& replacement = impl.GetReplacementRenderState();
  if(replacement.processingModel && replacement.projection.HasReplacements())
  {
    layoutSize = replacement.layoutSize;
  }

  if(updated)
  {
    updateTextType = MODEL_UPDATED;
  }

  // Do not re-do any operation until something changes.
  operationsPending          = NO_OPERATION;
  model->mScrollPositionLast = model->mScrollPosition;

  // Whether the text control is editable
  const bool isEditable = NULL != impl.mEventData;

  // Keep the current offset as it will be used to update the decorator's positions (if the size changes).
  Vector2 offset;
  if(newSize && isEditable)
  {
    offset = model->mScrollPosition;
  }

  if(!isEditable || !controller.IsMultiLineEnabled())
  {
    // After doing the text layout, the vertical offset to place the actor in the desired position can be calculated.
    CalculateVerticalOffset(impl, size);
  }
  else // TextEditor
  {
    // If layoutSize is bigger than size, vertical align has no meaning.
    // Use effective layout height that accounts for trailing empty lines.
    const float effectiveLayoutHeight = GetEffectiveEditableLayoutHeight(impl, layoutSize.y);
    if(effectiveLayoutHeight <= size.y + Math::MACHINE_EPSILON_1000)
    {
      CalculateVerticalOffset(impl, size);
      if(impl.mEventData)
      {
        impl.mEventData->mScrollAfterDelete = false;
      }
    }
  }
  impl.SyncReplacementScrollPosition();

  if(isEditable)
  {
    if(newSize || layoutDirectionChanged)
    {
      // If there is a new size or layout direction is changed, the scroll position needs to be clamped.
      impl.ClampHorizontalScroll(layoutSize);

      // Update the decorator's positions is needed if there is a new size.
      impl.mEventData->mDecorator->UpdatePositions(model->mScrollPosition - offset);

      // All decorator elements need to be updated.
      if(EventData::IsEditingState(impl.mEventData->mState))
      {
        impl.mEventData->mScrollAfterUpdatePosition = true;
        impl.mEventData->mUpdateCursorPosition      = true;
        impl.mEventData->mUpdateGrabHandlePosition  = true;
      }
      else if(impl.mEventData->mState == EventData::SELECTING)
      {
        impl.mEventData->mUpdateHighlightBox = true;
      }
    }

    // Move the cursor, grab handle etc.
    if(impl.ProcessInputEvents())
    {
      updateTextType = static_cast<UpdateTextType>(updateTextType | DECORATOR_UPDATED);
    }
  }

  // Clear the update info. This info will be set the next time the text is updated.
  textUpdateInfo.Clear();
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::Relayout\n");

  return updateTextType;
}

bool Controller::Relayouter::DoRelayout(Controller::Impl& impl, const Size& size, Length maximumNumberOfLines,
                                        OperationsMask operationsRequired, Size& layoutSize)
{
  bool layoutTooSmall               = false;
  bool maximumNumberOfLinesExceeded = false;
  return DoRelayout(impl, size, maximumNumberOfLines, operationsRequired, layoutSize, layoutTooSmall,
                    maximumNumberOfLinesExceeded);
}

bool Controller::Relayouter::DoRelayout(Controller::Impl& impl, const Size& size, Length maximumNumberOfLines,
                                        OperationsMask operationsRequired, Size& layoutSize, bool& layoutTooSmall)
{
  bool maximumNumberOfLinesExceeded = false;
  return DoRelayout(impl, size, maximumNumberOfLines, operationsRequired, layoutSize, layoutTooSmall,
                    maximumNumberOfLinesExceeded);
}

bool Controller::Relayouter::DoRelayout(Controller::Impl& impl, const Size& size, Length maximumNumberOfLines,
                                        OperationsMask operationsRequired, Size& layoutSize, bool& layoutTooSmall,
                                        bool& maximumNumberOfLinesExceeded)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->Controller::Relayouter::DoRelayout %p size %f,%f\n", &impl, size.width,
                size.height);
  DALI_TRACE_SCOPE(gTraceFilter2, "DALI_TEXT_DORELAYOUT");
  bool viewUpdated(false);
  bool endEllipsisFinalizedInLayout{false};
  maximumNumberOfLinesExceeded = false;

  // Calculate the operations to be done.
  const OperationsMask operations = static_cast<OperationsMask>(impl.mOperationsPending & operationsRequired);

  TextUpdateInfo&      textUpdateInfo              = impl.mTextUpdateInfo;
  const CharacterIndex startIndex                  = textUpdateInfo.mParagraphCharacterIndex;
  const Length         requestedNumberOfCharacters = textUpdateInfo.mRequestedNumberOfCharacters;

  // Get the current layout size.
  VisualModelPtr& visualModel = impl.mModel->mVisualModel;
  layoutSize                  = visualModel->GetLayoutSize();
  if(const FinalElisionResult* endEllipsis = impl.GetEndEllipsisResult();
     endEllipsis && endEllipsis->HasAuthoritativeLayout())
  {
    layoutSize = endEllipsis->layoutSize;
  }

  // A valid replacement projection owns the complete layout/alignment pass.
  // Do not first lay out the underlying glyph stream and then mix its result
  // with replacement placements: natural size, wrapping and ellipsis must all
  // come from the same projected model.
  if(impl.HasValidReplacementSource() &&
     NO_OPERATION != ((LAYOUT | ALIGN) & operations))
  {
    UpdateReplacementRenderState(impl, size, maximumNumberOfLines, maximumNumberOfLinesExceeded);
    const ReplacementRenderState& replacement = impl.GetReplacementRenderState();
    if(replacement.processingModel && replacement.projection.HasReplacements())
    {
      layoutSize = replacement.layoutSize;
      if(NO_OPERATION != (UPDATE_LAYOUT_SIZE & operations))
      {
        visualModel->SetLayoutSize(layoutSize);
      }
      impl.mIsTextDirectionRTL = false;
      const Vector<LineRun>& projectedLines =
        replacement.processingModel->mVisualModel->mLines;
      if(!projectedLines.Empty())
      {
        impl.mIsTextDirectionRTL = projectedLines[0u].direction;
      }
      return true;
    }
  }

  if(NO_OPERATION != (LAYOUT & operations))
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "-->Controller::DoRelayout LAYOUT & operations\n");

    // Some vectors with data needed to layout and reorder may be void
    // after the first time the text has been laid out.
    // Fill the vectors again.

    // Calculate the number of glyphs to layout.
    const Vector<GlyphIndex>& charactersToGlyph        = visualModel->mCharactersToGlyph;
    const Vector<Length>&     glyphsPerCharacter       = visualModel->mGlyphsPerCharacter;
    const GlyphIndex* const   charactersToGlyphBuffer  = charactersToGlyph.Begin();
    const Length* const       glyphsPerCharacterBuffer = glyphsPerCharacter.Begin();

    const CharacterIndex lastIndex =
      startIndex + ((requestedNumberOfCharacters > 0u) ? requestedNumberOfCharacters - 1u : 0u);
    const GlyphIndex startGlyphIndex = textUpdateInfo.mStartGlyphIndex;

    // Make sure the index is not out of bound
    if(charactersToGlyph.Count() != glyphsPerCharacter.Count() ||
       requestedNumberOfCharacters > charactersToGlyph.Count() ||
       (lastIndex > charactersToGlyph.Count() && charactersToGlyph.Count() > 0u))
    {
      std::string currentText;
      impl.GetText(currentText);

      DALI_LOG_ERROR("Controller::DoRelayout: Attempting to access invalid buffer\n");
      DALI_LOG_ERROR("Current text is: %s\n", currentText.c_str());
      DALI_LOG_ERROR(
        "startIndex: %u, lastIndex: %u, requestedNumberOfCharacters: %u, charactersToGlyph.Count = %lu, "
        "glyphsPerCharacter.Count = %lu\n",
        startIndex, lastIndex, requestedNumberOfCharacters, charactersToGlyph.Count(), glyphsPerCharacter.Count());

      return false;
    }

    const Length numberOfGlyphs =
      (requestedNumberOfCharacters > 0u)
        ? *(charactersToGlyphBuffer + lastIndex) + *(glyphsPerCharacterBuffer + lastIndex) - startGlyphIndex
        : 0u;
    const Length totalNumberOfGlyphs = static_cast<Dali::Ui::Text::Length>(visualModel->mGlyphs.Count());

    if(0u == totalNumberOfGlyphs)
    {
      if(NO_OPERATION != (UPDATE_LAYOUT_SIZE & operations))
      {
        visualModel->SetLayoutSize(Size::ZERO);
      }

      // Nothing else to do if there is no glyphs.
      if(impl.HasValidReplacementSource())
      {
        UpdateReplacementRenderState(impl, size, maximumNumberOfLines, maximumNumberOfLinesExceeded);
      }
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::DoRelayout no glyphs, view updated true\n");
      return true;
    }

    TextAbstraction::BidirectionalSupport bidirectionalSupport = TextAbstraction::BidirectionalSupport::Get();

    // Set the layout parameters.
    Layout::Parameters layoutParameters(size, impl.mModel, impl.GetFontClient(), bidirectionalSupport);
    layoutParameters.maximumNumberOfLines = maximumNumberOfLines;

    // Resize the vector of positions to have the same size than the vector of glyphs.
    Vector<Vector2>& glyphPositions = visualModel->mGlyphPositions;
    glyphPositions.Resize(totalNumberOfGlyphs);

    // Whether the last character is a new paragraph character.
    const Character* const textBuffer = impl.mModel->mLogicalModel->mText.Begin();
    textUpdateInfo.mIsLastCharacterNewParagraph =
      TextAbstraction::IsNewParagraph(*(textBuffer + (impl.mModel->mLogicalModel->mText.Count() - 1u)));
    layoutParameters.isLastNewParagraph = textUpdateInfo.mIsLastCharacterNewParagraph;

    // The initial glyph and the number of glyphs to layout.
    layoutParameters.startGlyphIndex        = startGlyphIndex;
    layoutParameters.numberOfGlyphs         = numberOfGlyphs;
    layoutParameters.startLineIndex         = textUpdateInfo.mStartLineIndex;
    layoutParameters.estimatedNumberOfLines = textUpdateInfo.mEstimatedNumberOfLines;

    float fontPointSize =
      (impl.IsTextFitEnabled() || impl.IsTextFitCandidatesEnabled())
        ? (impl.mFontDefaults ? impl.mFontDefaults->mFitPointSize : 0.f)
        : (impl.mFontDefaults ? impl.mFontDefaults->mDefaultPointSize : 0.f) * impl.GetEffectiveTextScale();
    impl.mLayoutEngine.SetFontPixelSize(ConvertPointToPixel(fontPointSize));

    // Update the ellipsis
    bool elideTextEnabled = impl.mModel->mElideEnabled;
    auto ellipsisPosition = impl.mModel->mEllipsisPosition;

    if(NULL != impl.mEventData)
    {
      if(impl.mEventData->mPlaceholderEllipsisFlag && impl.IsShowingPlaceholderText())
      {
        elideTextEnabled = impl.mEventData->mIsPlaceholderElideEnabled;
      }
      else if(EventData::INACTIVE != impl.mEventData->mState)
      {
        // Disable ellipsis when editing
        elideTextEnabled = false;
      }

      // Reset the scroll position in inactive state
      if(elideTextEnabled && (impl.mEventData->mState == EventData::INACTIVE))
      {
        impl.ResetScrollPosition();
      }
    }

    // Update the visual model.
    bool isMarqueeEnabled            = impl.mIsMarqueeEnabled;
    bool isMarqueeMaxTextureExceeded = impl.mIsMarqueeMaxTextureExceeded;
    bool isHiddenInputEnabled        = false;
    if(impl.mHiddenInput && impl.mEventData != nullptr &&
       impl.mHiddenInput->GetMode() != Ui::Text::HiddenText::Mode::NONE)
    {
      isHiddenInputEnabled = true;
    }

    Size newLayoutSize;
    if(!impl.HasValidReplacementSource())
    {
      impl.ClearEndEllipsisResult();
    }
    viewUpdated = impl.mLayoutEngine.LayoutText(layoutParameters, newLayoutSize, elideTextEnabled, isMarqueeEnabled,
                                                isMarqueeMaxTextureExceeded, isHiddenInputEnabled, ellipsisPosition);
    maximumNumberOfLinesExceeded |= layoutParameters.maximumNumberOfLinesExceeded;

    const auto hasEndEllipsisCandidate = [visualModel]()
    {
      for(const LineRun& line : visualModel->mLines)
      {
        if(line.ellipsis)
        {
          return true;
        }
      }
      return false;
    };
    const bool hasNoVisibleEndLine = !viewUpdated && visualModel->mLines.Empty() &&
                                     !impl.mModel->mLogicalModel->mText.Empty();
    if(!impl.HasValidReplacementSource() && elideTextEnabled &&
       ellipsisPosition == EllipsisPosition::END &&
       (hasEndEllipsisCandidate() || hasNoVisibleEndLine))
    {
      visualModel->SetLayoutSize(newLayoutSize);
      FinalElisionResult& finalElision = impl.GetOrCreateEndEllipsisResult();
      const bool          resolved     = ResolveEndEllipsis(*impl.mModel,
                                                            size,
                                                            impl.GetFontClient(),
                                                            finalElision);
      DALI_ASSERT_DEBUG(resolved && finalElision.resolved &&
                        "Supported END layout must publish an authoritative final result");
      if(resolved)
      {
        newLayoutSize = finalElision.layoutSize;
        FinalizeEndEllipsisGeometry(*impl.mModel,
                                    size,
                                    impl.mLayoutDirection,
                                    impl.mModel->mLayoutDirectionMode != LayoutDirectionMode::CONTENTS,
                                    impl.mLayoutEngine,
                                    finalElision);
        endEllipsisFinalizedInLayout = true;
        impl.mView.SetFinalElisionResult(&finalElision);
      }
      else
      {
        impl.ClearEndEllipsisResult();
      }
    }

    impl.mIsMarqueeEnabled = isMarqueeEnabled;
    layoutTooSmall         = !viewUpdated;

    viewUpdated = viewUpdated || (newLayoutSize != layoutSize);

    if(viewUpdated)
    {
      layoutSize = newLayoutSize;

      if(NO_OPERATION != (UPDATE_DIRECTION & operations))
      {
        impl.mIsTextDirectionRTL = false;
      }

      if((NO_OPERATION != (UPDATE_DIRECTION & operations)) && !visualModel->mLines.Empty())
      {
        impl.mIsTextDirectionRTL = visualModel->mLines[0u].direction;
      }

      // Sets the layout size.
      if(NO_OPERATION != (UPDATE_LAYOUT_SIZE & operations))
      {
        if(!impl.GetEndEllipsisResult())
        {
          visualModel->SetLayoutSize(layoutSize);
        }
      }
    } // view updated
  }

  if(NO_OPERATION != (ALIGN & operations))
  {
    DoRelayoutHorizontalAlignment(impl, size, startIndex, requestedNumberOfCharacters);
    if(FinalElisionResult* finalElision = impl.mEndEllipsisResult.get();
       finalElision && finalElision->resolved)
    {
      // LAYOUT-only and ALIGN-only requests each need one finalization. When
      // both operations share this transaction, the LAYOUT result is already
      // aligned in the same final-domain coordinate space.
      if(!endEllipsisFinalizedInLayout)
      {
        FinalizeEndEllipsisGeometry(*impl.mModel,
                                    size,
                                    impl.mLayoutDirection,
                                    impl.mModel->mLayoutDirectionMode != LayoutDirectionMode::CONTENTS,
                                    impl.mLayoutEngine,
                                    *finalElision);
      }
      impl.mView.SetFinalElisionResult(finalElision);
    }
    viewUpdated = true;
  }

  if(impl.HasValidReplacementSource() && NO_OPERATION != ((LAYOUT | ALIGN) & operations))
  {
    // Replacement placements are produced after the final layout/alignment used by this relayout request.
    UpdateReplacementRenderState(impl, size, maximumNumberOfLines, maximumNumberOfLinesExceeded);
  }
#if defined(DEBUG_ENABLED)
  std::string currentText;
  impl.GetText(currentText);
  DALI_LOG_INFO(gLogFilter, Debug::Concise,
                "Controller::Relayouter::DoRelayout [%p] mImpl->mIsTextDirectionRTL[%s] [%s]\n", &impl,
                (impl.mIsTextDirectionRTL) ? "true" : "false", currentText.c_str());
#endif
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::Relayouter::DoRelayout, view updated %s\n",
                (viewUpdated ? "true" : "false"));
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "<--Controller::Relayouter::DoRelayout, layout too small %s\n",
                (layoutTooSmall ? "true" : "false"));
  return viewUpdated;
}

void Controller::Relayouter::DoRelayoutHorizontalAlignment(Controller::Impl& impl, const Size& size,
                                                           const CharacterIndex startIndex,
                                                           const Length         requestedNumberOfCharacters)
{
  // The visualModel
  VisualModelPtr& visualModel = impl.mModel->mVisualModel;

  // The laid-out lines.
  Vector<LineRun>& lines = visualModel->mLines;

  CharacterIndex alignStartIndex                  = startIndex;
  Length         alignRequestedNumberOfCharacters = requestedNumberOfCharacters;

  // the whole text needs to be full aligned.
  // If you do not do a full aligned, only the last line of the multiline input is aligned.
  if(impl.mEventData && impl.mEventData->mUpdateAlignment)
  {
    alignStartIndex                   = 0u;
    alignRequestedNumberOfCharacters  = static_cast<Dali::Ui::Text::Length>(impl.mModel->mLogicalModel->mText.Count());
    impl.mEventData->mUpdateAlignment = false;
  }

  AlignTextLines(impl.mLayoutEngine,
                 size,
                 alignStartIndex,
                 alignRequestedNumberOfCharacters,
                 *impl.mModel,
                 lines,
                 impl.mModel->mAlignmentOffset,
                 impl.mLayoutDirection,
                 impl.mModel->mLayoutDirectionMode != LayoutDirectionMode::CONTENTS);
}

void Controller::Relayouter::CalculateVerticalOffset(Controller::Impl& impl, const Size& controlSize)
{
  ModelPtr&       model       = impl.mModel;
  VisualModelPtr& visualModel = model->mVisualModel;
  Size            layoutSize  = model->mVisualModel->GetLayoutSize();
  if(const FinalElisionResult* endEllipsis = impl.GetEndEllipsisResult();
     endEllipsis && endEllipsis->HasAuthoritativeLayout())
  {
    layoutSize = endEllipsis->layoutSize;
  }
  Size  oldLayoutSize = layoutSize;
  float offsetY       = 0.f;
  bool  needRecalc    = false;

  // Whether the text control is editable
  const bool  isEditable            = NULL != impl.mEventData;
  const float defaultFontLineHeight = impl.GetDefaultFontLineHeight();
  const float defaultLineBoxHeight  = impl.GetDefaultLineBoxHeight(defaultFontLineHeight);
  const bool  isShowingPlaceholder  = isEditable && impl.IsShowingPlaceholderText();

  // For editable empty text without placeholder, use line box height as fallback.
  // This ensures block-level VerticalAlignment is consistent with the cursor's internal offset.
  const bool isEmptyEditableText =
    isEditable && !isShowingPlaceholder && impl.mModel->mLogicalModel->mText.Count() == 0u;

  if(fabsf(layoutSize.height) < Math::MACHINE_EPSILON_1000)
  {
    // Use line box height for editable empty text, otherwise use font line height.
    layoutSize.height = isEmptyEditableText ? defaultLineBoxHeight : defaultFontLineHeight;
  }

  // Apply the legacy placeholder height adjustment only for single-line placeholders.
  // Multiline placeholders should be aligned using their actual layout height.
  const bool isMultilinePlaceholder = isShowingPlaceholder && visualModel->mLines.Count() > 1u;

  // Avoid cumulative glyph-position adjustment when explicit LineHeight changes
  // the default line box height.
  const bool hasExplicitLineHeight = !Dali::Equals(defaultLineBoxHeight, defaultFontLineHeight);

  if(isEditable && isShowingPlaceholder && !isMultilinePlaceholder &&
     !Dali::Equals(layoutSize.height, defaultLineBoxHeight))
  {
    layoutSize.height = defaultLineBoxHeight;
    needRecalc        = !hasExplicitLineHeight;
  }

  // Use effective layout height that accounts for trailing empty lines.
  // This handles delete-induced trailing empty lines.
  layoutSize.height = GetEffectiveEditableLayoutHeight(impl, layoutSize.height);

  switch(model->mVerticalAlignment)
  {
    case Alignment::START:
    {
      model->mScrollPosition.y = 0.f;
      offsetY                  = 0.f;
      break;
    }
    case Alignment::CENTER:
    {
      model->mScrollPosition.y =
        floorf(0.5f * (controlSize.height - layoutSize.height)); // try to avoid pixel alignment.
      if(needRecalc)
      {
        offsetY = floorf(0.5f * (layoutSize.height - oldLayoutSize.height));
      }
      break;
    }
    case Alignment::END:
    {
      model->mScrollPosition.y = controlSize.height - layoutSize.height;
      if(needRecalc)
      {
        offsetY = layoutSize.height - oldLayoutSize.height;
      }
      break;
    }
  }

  if(needRecalc)
  {
    // Update glyphPositions according to recalculation.
    const Length     positionCount  = static_cast<Dali::Ui::Text::Length>(visualModel->mGlyphPositions.Count());
    Vector<Vector2>& glyphPositions = visualModel->mGlyphPositions;
    for(Length index = 0u; index < positionCount; index++)
    {
      glyphPositions[index].y += offsetY;
    }
  }
}

} // namespace Text

} // namespace Ui

} // namespace Dali
