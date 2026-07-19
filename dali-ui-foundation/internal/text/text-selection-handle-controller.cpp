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
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/controller/text-controller-impl-event-handler.h>
#include <dali-ui-foundation/internal/text/cursor-helper-functions.h>
#include <dali-ui-foundation/internal/text/glyph-metrics-helper.h>
#include <dali-ui-foundation/internal/text/line-helper-functions.h>
#include <dali-ui-foundation/internal/text/rendering/styles/character-spacing-helper-functions.h>
#include <dali-ui-foundation/internal/text/text-selection-handle-controller.h>

using namespace Dali;

namespace
{
/**
 * @brief Struct used to calculate the selection box.
 */
struct SelectionBoxInfo
{
  float lineOffset;
  float lineHeight;
  float minX;
  float maxX;
};

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

const float                              MAX_FLOAT = std::numeric_limits<float>::max();
const float                              MIN_FLOAT = std::numeric_limits<float>::min();
const Dali::Ui::Text::CharacterDirection LTR       = false; ///< Left To Right direction

} // namespace

namespace Dali
{
namespace Ui
{
namespace Text
{
void SelectionHandleController::Reposition(Controller::Impl& impl)
{
  EventData*& eventData = impl.mEventData;

  CharacterIndex selectionStart = eventData->mLeftSelectionPosition;
  CharacterIndex selectionEnd   = eventData->mRightSelectionPosition;

  impl.NormalizeReplacementSelection(selectionStart, selectionEnd);
  eventData->mLeftSelectionPosition  = selectionStart;
  eventData->mRightSelectionPosition = selectionEnd;

  DecoratorPtr& decorator = eventData->mDecorator;

  if(selectionStart == selectionEnd)
  {
    // Nothing to select if handles are in the same place.
    // So, deactive Highlight box.
    decorator->SetHighlightActive(false);
    return;
  }

  // Re-activate highlight in case it was deactivated when selection was empty.
  decorator->SetHighlightActive(true);
  decorator->ClearHighlights();

  const bool indicesSwapped = selectionStart > selectionEnd;
  if(indicesSwapped)
  {
    selectionStart = impl.LogicalBoundaryToEditable(selectionStart,
                                                    ReplacementProjection::BoundaryAffinity::TRAILING);
    selectionEnd   = impl.LogicalBoundaryToEditable(selectionEnd,
                                                    ReplacementProjection::BoundaryAffinity::LEADING);
  }
  else
  {
    selectionStart = impl.LogicalBoundaryToEditable(selectionStart,
                                                    ReplacementProjection::BoundaryAffinity::LEADING);
    selectionEnd   = impl.LogicalBoundaryToEditable(selectionEnd,
                                                    ReplacementProjection::BoundaryAffinity::TRAILING);
  }

  ModelPtr         model        = impl.GetEditableGeometryModel();
  VisualModelPtr&  visualModel  = model->mVisualModel;
  LogicalModelPtr& logicalModel = model->mLogicalModel;

  const GlyphIndex* const         charactersToGlyphBuffer  = visualModel->mCharactersToGlyph.Begin();
  const Length* const             glyphsPerCharacterBuffer = visualModel->mGlyphsPerCharacter.Begin();
  const GlyphInfo* const          glyphsBuffer             = visualModel->mGlyphs.Begin();
  const Vector2* const            positionsBuffer          = visualModel->mGlyphPositions.Begin();
  const Length* const             charactersPerGlyphBuffer = visualModel->mCharactersPerGlyph.Begin();
  const CharacterIndex* const     glyphToCharacterBuffer   = visualModel->mGlyphsToCharacters.Begin();
  const CharacterDirection* const modelCharacterDirectionsBuffer =
    (0u != logicalModel->mCharacterDirections.Count()) ? logicalModel->mCharacterDirections.Begin() : NULL;

  auto directionAtBoundary = [&](CharacterIndex boundary, bool trailing)
  {
    if(NULL == modelCharacterDirectionsBuffer || logicalModel->mText.Empty())
    {
      return CharacterDirection(false);
    }
    CharacterIndex character = trailing && boundary > 0u ? boundary - 1u : boundary;
    character                = std::min(character, static_cast<CharacterIndex>(logicalModel->mText.Count() - 1u));
    return *(modelCharacterDirectionsBuffer + character);
  };
  const CharacterDirection startDirection = directionAtBoundary(selectionStart, indicesSwapped);
  const CharacterDirection endDirection   = directionAtBoundary(selectionEnd, !indicesSwapped);

  // Swap the indices if the start is greater than the end.
  // Tell the decorator to flip the selection handles if needed.
  decorator->SetSelectionHandleFlipState(indicesSwapped, startDirection, endDirection);

  if(indicesSwapped)
  {
    std::swap(selectionStart, selectionEnd);
  }

  // Get the indices to the first and last selected glyphs.
  const CharacterIndex selectionEndMinusOne = selectionEnd - 1u;
  const GlyphIndex     glyphStart           = *(charactersToGlyphBuffer + selectionStart);
  const Length         numberOfGlyphs       = *(glyphsPerCharacterBuffer + selectionEndMinusOne);
  const GlyphIndex     glyphEnd =
    *(charactersToGlyphBuffer + selectionEndMinusOne) + ((numberOfGlyphs > 0) ? numberOfGlyphs - 1u : 0u);
  const float             modelCharacterSpacing     = visualModel->GetCharacterSpacing();
  Vector<CharacterIndex>& glyphToCharacterMap       = visualModel->mGlyphsToCharacters;
  const CharacterIndex*   glyphToCharacterMapBuffer = glyphToCharacterMap.Begin();

  // Get the character-spacing runs.
  const Vector<CharacterSpacingGlyphRun>& characterSpacingGlyphRuns = visualModel->GetCharacterSpacingGlyphRuns();

  // Get the lines where the glyphs are laid-out.
  const LineRun* lineRun = visualModel->mLines.Begin();

  LineIndex lineIndex     = 0u;
  Length    numberOfLines = 0u;
  visualModel->GetNumberOfLines(glyphStart, 1u + glyphEnd - glyphStart, lineIndex, numberOfLines);
  const LineIndex firstLineIndex = lineIndex;

  // Create the structure to store some selection box info.
  Vector<SelectionBoxInfo> selectionBoxLinesInfo;
  selectionBoxLinesInfo.Resize(numberOfLines);

  SelectionBoxInfo* selectionBoxInfo = selectionBoxLinesInfo.Begin();
  selectionBoxInfo->minX             = MAX_FLOAT;
  selectionBoxInfo->maxX             = MIN_FLOAT;

  // Keep the min and max 'x' position to calculate the size and position of the highlighed text.
  float minHighlightX = std::numeric_limits<float>::max();
  float maxHighlightX = std::numeric_limits<float>::lowest();
  // Keep the min and max 'y' position to calculate the height of the highlight actor.
  // When LineHeight is large, lineOffset gaps are larger than natural text height.
  // The highlight actor bounds must contain all quads, even if quad heights are natural text height.
  float   minHighlightY = std::numeric_limits<float>::max();
  float   maxHighlightY = std::numeric_limits<float>::lowest();
  Size    highLightSize;
  Vector2 highLightPosition; // The highlight position in decorator's coords.

  // Retrieve the first line and get the line's vertical offset, the line's height and the index to the last glyph.

  lineRun += firstLineIndex;

  // Get vertical line alignment for selection highlight positioning
  const Alignment verticalLineAlignment = model->GetVerticalLineAlignment();
  const float     verticalLineOffset    = GetPreOffsetVerticalLineAlignment(*lineRun, verticalLineAlignment);

  // The line's vertical offset of all the lines before the line where the first glyph is laid-out.
  // Add verticalLineOffset to position highlight at visible text area within LineHeight.
  selectionBoxInfo->lineOffset = CalculateLineOffset(visualModel->mLines, firstLineIndex) + verticalLineOffset;

  // Transform to decorator's (control) coords.
  selectionBoxInfo->lineOffset += model->mScrollPosition.y;

  // Selection highlight uses the natural text line height.
  // Explicit LineHeight affects line placement, but the highlight should not
  // fill the extra line spacing area.
  selectionBoxInfo->lineHeight = lineRun->ascender - lineRun->descender;

  GlyphIndex lastGlyphOfLine = lineRun->glyphRun.glyphIndex + lineRun->glyphRun.numberOfGlyphs - 1u;

  // Check if the first glyph is a ligature that must be broken like Latin ff, fi, or Arabic ﻻ, etc which needs special
  // code.
  const Length numberOfCharactersStart = *(charactersPerGlyphBuffer + glyphStart);
  bool         splitStartGlyph =
    (numberOfCharactersStart > 1u) && HasLigatureMustBreak(logicalModel->GetScript(selectionStart));

  // Check if the last glyph is a ligature that must be broken like Latin ff, fi, or Arabic ﻻ, etc which needs special
  // code.
  const Length numberOfCharactersEnd = *(charactersPerGlyphBuffer + glyphEnd);
  bool         splitEndGlyph         = (glyphStart != glyphEnd) && (numberOfCharactersEnd > 1u) &&
                       HasLigatureMustBreak(logicalModel->GetScript(selectionEndMinusOne));

  // The number of quads of the selection box.
  const unsigned int numberOfQuads = 1u + (glyphEnd - glyphStart) + ((numberOfLines > 1u) ? 2u * numberOfLines : 0u);
  decorator->ResizeHighlightQuads(numberOfQuads);

  // Count the actual number of quads.
  unsigned int actualNumberOfQuads = 0u;
  Vector4      quad;
  float        calculatedAdvance = 0.f;

  // Traverse the glyphs.
  for(GlyphIndex index = glyphStart; index <= glyphEnd; ++index)
  {
    const float      characterSpacing = GetGlyphCharacterSpacing(index, characterSpacingGlyphRuns, modelCharacterSpacing);
    const GlyphInfo& glyph            = *(glyphsBuffer + index);
    const Vector2&   position         = *(positionsBuffer + index);
    calculatedAdvance                 = GetCalculatedAdvance(*(logicalModel->mText.Begin() + (*(glyphToCharacterMapBuffer + index))),
                                                             characterSpacing, glyph.advance);

    if(splitStartGlyph)
    {
      // If the first glyph is a ligature that must be broken it may be needed to add only part of the glyph to the
      // highlight box.

      const float          glyphAdvance    = calculatedAdvance / static_cast<float>(numberOfCharactersStart);
      const CharacterIndex interGlyphIndex = selectionStart - *(glyphToCharacterBuffer + glyphStart);
      // Get the direction of the character.
      CharacterDirection isCurrentRightToLeft = false;
      if(nullptr != modelCharacterDirectionsBuffer) // If modelCharacterDirectionsBuffer is NULL, it means the whole
                                                    // text is left to right.
      {
        isCurrentRightToLeft = *(modelCharacterDirectionsBuffer + selectionStart);
      }

      // The end point could be in the middle of the ligature.
      // Calculate the number of characters selected.
      const Length numberOfCharacters =
        (glyphStart == glyphEnd) ? (selectionEnd - selectionStart) : (numberOfCharactersStart - interGlyphIndex);

      quad.x = lineRun->alignmentOffset + position.x - glyph.xBearing + model->mScrollPosition.x +
               glyphAdvance * static_cast<float>(isCurrentRightToLeft
                                                   ? (numberOfCharactersStart - interGlyphIndex - numberOfCharacters)
                                                   : interGlyphIndex);
      quad.y = selectionBoxInfo->lineOffset;
      quad.z = quad.x + static_cast<float>(numberOfCharacters) * glyphAdvance;
      quad.w = selectionBoxInfo->lineOffset + selectionBoxInfo->lineHeight;

      // Store the min and max 'x' for each line.
      selectionBoxInfo->minX = std::min(selectionBoxInfo->minX, quad.x);
      selectionBoxInfo->maxX = std::max(selectionBoxInfo->maxX, quad.z);

      decorator->AddHighlight(actualNumberOfQuads, quad);
      ++actualNumberOfQuads;

      splitStartGlyph = false;
      continue;
    }

    if(splitEndGlyph && (index == glyphEnd))
    {
      // Equally, if the last glyph is a ligature that must be broken it may be needed to add only part of the glyph to
      // the highlight box.

      const float          glyphAdvance    = calculatedAdvance / static_cast<float>(numberOfCharactersEnd);
      const CharacterIndex interGlyphIndex = selectionEnd - *(glyphToCharacterBuffer + glyphEnd);
      // Get the direction of the character.
      CharacterDirection isCurrentRightToLeft = false;
      if(nullptr != modelCharacterDirectionsBuffer) // If modelCharacterDirectionsBuffer is NULL, it means the whole
                                                    // text is left to right.
      {
        isCurrentRightToLeft = *(modelCharacterDirectionsBuffer + selectionEnd);
      }

      const Length numberOfCharacters = numberOfCharactersEnd - interGlyphIndex;

      quad.x = lineRun->alignmentOffset + position.x - glyph.xBearing + model->mScrollPosition.x +
               (isCurrentRightToLeft ? (glyphAdvance * static_cast<float>(numberOfCharacters)) : 0.f);
      quad.y = selectionBoxInfo->lineOffset;
      quad.z = quad.x + static_cast<float>(interGlyphIndex) * glyphAdvance;
      quad.w = quad.y + selectionBoxInfo->lineHeight;

      // Store the min and max 'x' for each line.
      selectionBoxInfo->minX = std::min(selectionBoxInfo->minX, quad.x);
      selectionBoxInfo->maxX = std::max(selectionBoxInfo->maxX, quad.z);

      decorator->AddHighlight(actualNumberOfQuads, quad);
      ++actualNumberOfQuads;

      splitEndGlyph = false;
      continue;
    }

    quad.x = lineRun->alignmentOffset + position.x - glyph.xBearing + model->mScrollPosition.x;
    quad.y = selectionBoxInfo->lineOffset;
    quad.z = quad.x + calculatedAdvance;
    quad.w = quad.y + selectionBoxInfo->lineHeight;

    // Store the min and max 'x' for each line.
    selectionBoxInfo->minX = std::min(selectionBoxInfo->minX, quad.x);
    selectionBoxInfo->maxX = std::max(selectionBoxInfo->maxX, quad.z);

    decorator->AddHighlight(actualNumberOfQuads, quad);
    ++actualNumberOfQuads;

    // Whether to retrieve the next line.
    if(index == lastGlyphOfLine)
    {
      ++lineIndex;
      if(lineIndex < firstLineIndex + numberOfLines)
      {
        // Retrieve the next line.
        ++lineRun;

        // Get the last glyph of the new line.
        lastGlyphOfLine = lineRun->glyphRun.glyphIndex + lineRun->glyphRun.numberOfGlyphs - 1u;

        // Get the selection box info for the next line.
        ++selectionBoxInfo;

        selectionBoxInfo->minX = MAX_FLOAT;
        selectionBoxInfo->maxX = MIN_FLOAT;

        // Calculate vertical line offset for the next line
        const float nextLineVerticalOffset = GetPreOffsetVerticalLineAlignment(*lineRun, verticalLineAlignment);

        // Use CalculateLineOffset for consistent line offset calculation.
        // This avoids cumulative errors when explicit LineHeight is set.
        // Add verticalLineOffset to position highlight at visible text area within LineHeight.
        selectionBoxInfo->lineOffset = CalculateLineOffset(visualModel->mLines, lineIndex) +
                                       nextLineVerticalOffset +
                                       model->mScrollPosition.y;

        // Selection highlight uses the natural text line height.
        // Explicit LineHeight affects line placement, but the highlight should not
        // fill the extra line spacing area.
        selectionBoxInfo->lineHeight = lineRun->ascender - lineRun->descender;
      }
    }
  }

  // Traverses all the lines and updates the min and max 'x' positions and the y extents.
  // The final width is calculated after 'boxifying' the selection.
  // The height is calculated from the actual y extents of all quads, not by summing lineHeight.
  // When LineHeight is large, lineOffset gaps are larger than natural text height,
  // so we need to use min/max y to include all quads in the highlight actor bounds.
  for(Vector<SelectionBoxInfo>::ConstIterator it = selectionBoxLinesInfo.Begin(), endIt = selectionBoxLinesInfo.End();
      it != endIt; ++it)
  {
    const SelectionBoxInfo& info = *it;

    minHighlightX = std::min(minHighlightX, info.minX);
    maxHighlightX = std::max(maxHighlightX, info.maxX);
    minHighlightY = std::min(minHighlightY, info.lineOffset);
    maxHighlightY = std::max(maxHighlightY, info.lineOffset + info.lineHeight);
  }

  // Add extra geometry to 'boxify' the selection.

  if(1u < numberOfLines)
  {
    // Boxify the first line.
    lineRun                                           = visualModel->mLines.Begin() + firstLineIndex;
    const SelectionBoxInfo& firstSelectionBoxLineInfo = *(selectionBoxLinesInfo.Begin());

    bool boxifyBegin = (LTR != lineRun->direction) && (LTR != startDirection);
    bool boxifyEnd   = (LTR == lineRun->direction) && (LTR == startDirection);

    if(boxifyBegin)
    {
      quad.x = 0.f;
      quad.y = firstSelectionBoxLineInfo.lineOffset;
      quad.z = firstSelectionBoxLineInfo.minX;
      quad.w = firstSelectionBoxLineInfo.lineOffset + firstSelectionBoxLineInfo.lineHeight;

      // Boxify at the beginning of the line.
      decorator->AddHighlight(actualNumberOfQuads, quad);
      ++actualNumberOfQuads;

      // Update the size of the highlighted text.
      minHighlightX = 0.f;
    }

    if(boxifyEnd)
    {
      quad.x = firstSelectionBoxLineInfo.maxX;
      quad.y = firstSelectionBoxLineInfo.lineOffset;
      quad.z = visualModel->mControlSize.width;
      quad.w = firstSelectionBoxLineInfo.lineOffset + firstSelectionBoxLineInfo.lineHeight;

      // Boxify at the end of the line.
      decorator->AddHighlight(actualNumberOfQuads, quad);
      ++actualNumberOfQuads;

      // Update the size of the highlighted text.
      maxHighlightX = visualModel->mControlSize.width;
    }

    // Boxify the central lines.
    if(2u < numberOfLines)
    {
      for(Vector<SelectionBoxInfo>::ConstIterator it    = selectionBoxLinesInfo.Begin() + 1u,
                                                  endIt = selectionBoxLinesInfo.End() - 1u;
          it != endIt; ++it)
      {
        const SelectionBoxInfo& info = *it;

        quad.x = 0.f;
        quad.y = info.lineOffset;
        quad.z = info.minX;
        quad.w = info.lineOffset + info.lineHeight;

        decorator->AddHighlight(actualNumberOfQuads, quad);
        ++actualNumberOfQuads;

        quad.x = info.maxX;
        quad.y = info.lineOffset;
        quad.z = visualModel->mControlSize.width;
        quad.w = info.lineOffset + info.lineHeight;

        decorator->AddHighlight(actualNumberOfQuads, quad);
        ++actualNumberOfQuads;
      }

      // Update the size of the highlighted text.
      minHighlightX = 0.f;
      maxHighlightX = visualModel->mControlSize.width;
    }

    // Boxify the last line.
    lineRun                                          = visualModel->mLines.Begin() + firstLineIndex + numberOfLines - 1u;
    const SelectionBoxInfo& lastSelectionBoxLineInfo = *(selectionBoxLinesInfo.End() - 1u);

    boxifyBegin = (LTR == lineRun->direction) && (LTR == endDirection);
    boxifyEnd   = (LTR != lineRun->direction) && (LTR != endDirection);

    if(boxifyBegin)
    {
      quad.x = 0.f;
      quad.y = lastSelectionBoxLineInfo.lineOffset;
      quad.z = lastSelectionBoxLineInfo.minX;
      quad.w = lastSelectionBoxLineInfo.lineOffset + lastSelectionBoxLineInfo.lineHeight;

      // Boxify at the beginning of the line.
      decorator->AddHighlight(actualNumberOfQuads, quad);
      ++actualNumberOfQuads;

      // Update the size of the highlighted text.
      minHighlightX = 0.f;
    }

    if(boxifyEnd)
    {
      quad.x = lastSelectionBoxLineInfo.maxX;
      quad.y = lastSelectionBoxLineInfo.lineOffset;
      quad.z = visualModel->mControlSize.width;
      quad.w = lastSelectionBoxLineInfo.lineOffset + lastSelectionBoxLineInfo.lineHeight;

      // Boxify at the end of the line.
      decorator->AddHighlight(actualNumberOfQuads, quad);
      ++actualNumberOfQuads;

      // Update the size of the highlighted text.
      maxHighlightX = visualModel->mControlSize.width;
    }
  }

  // Set the actual number of quads.
  decorator->ResizeHighlightQuads(actualNumberOfQuads);

  // Sets the highlight's size and position. In decorator's coords.
  // The highlight's width is calculated from min/max x positions.
  // The highlight's height is calculated from min/max y extents of all quads.
  // This ensures the highlight actor bounds contain all quads, even when LineHeight
  // causes large gaps between lineOffset values.
  highLightSize.width  = maxHighlightX - minHighlightX;
  highLightSize.height = maxHighlightY - minHighlightY;

  highLightPosition.x = minHighlightX;
  highLightPosition.y = minHighlightY;

  const float outlineWidth = model->IsOutlineEnabled() ? static_cast<float>(model->GetOutlineWidth()) : 0.0f;
  decorator->SetHighLightBox(highLightPosition, highLightSize, outlineWidth);

  if(!decorator->IsSmoothHandlePanEnabled())
  {
    CursorInfo primaryCursorInfo;
    impl.GetCursorPosition(eventData->mLeftSelectionPosition, primaryCursorInfo);

    const Vector2 primaryPosition = primaryCursorInfo.primaryPosition + model->mScrollPosition;

    // Use the visible text area top, not the line box top.
    const float primaryVisibleLineTop = primaryCursorInfo.primaryPosition.y - primaryCursorInfo.glyphOffset;
    decorator->SetPosition(LEFT_SELECTION_HANDLE, primaryPosition.x,
                           primaryVisibleLineTop + model->mScrollPosition.y, primaryCursorInfo.lineHeight);

    CursorInfo secondaryCursorInfo;
    impl.GetCursorPosition(eventData->mRightSelectionPosition, secondaryCursorInfo);

    const Vector2 secondaryPosition = secondaryCursorInfo.primaryPosition + model->mScrollPosition;

    // Use the visible text area top, not the line box top.
    const float secondaryVisibleLineTop = secondaryCursorInfo.primaryPosition.y - secondaryCursorInfo.glyphOffset;
    decorator->SetPosition(RIGHT_SELECTION_HANDLE, secondaryPosition.x,
                           secondaryVisibleLineTop + model->mScrollPosition.y, secondaryCursorInfo.lineHeight);
  }

  // Set the flag to update the decorator.
  eventData->mDecoratorUpdated = true;
}

void SelectionHandleController::Reposition(Controller::Impl& impl, float visualX, float visualY,
                                           Controller::NoTextTap::Action action)
{
  EventData*& eventData = impl.mEventData;
  if(nullptr == eventData)
  {
    // Nothing to do if there is no text input.
    return;
  }

  if(impl.IsShowingPlaceholderText())
  {
    // Nothing to do if there is the place-holder text.
    return;
  }

  ModelPtr        model          = impl.GetEditableGeometryModel();
  VisualModelPtr& visualModel    = model->mVisualModel;
  const Length    numberOfGlyphs = visualModel->mGlyphs.Count();
  const Length    numberOfLines  = visualModel->mLines.Count();
  if((0 == numberOfGlyphs) || (0 == numberOfLines))
  {
    // Nothing to do if there is no text.
    return;
  }

  // Find which word was selected
  CharacterIndex selectionStart(0);
  CharacterIndex selectionEnd(0);
  CharacterIndex noTextHitIndex(0);
  const bool     characterHit = FindSelectionIndices(visualModel, model->mLogicalModel, impl.mMetrics, visualX, visualY,
                                                     selectionStart, selectionEnd, noTextHitIndex);
  selectionStart              = impl.EditableBoundaryToLogical(selectionStart);
  selectionEnd                = impl.EditableBoundaryToLogical(selectionEnd);
  noTextHitIndex              = impl.EditableBoundaryToLogical(noTextHitIndex);
  impl.NormalizeReplacementSelection(selectionStart, selectionEnd);
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "%p selectionStart %d selectionEnd %d\n", &impl, selectionStart,
                selectionEnd);

  if(characterHit || (Controller::NoTextTap::HIGHLIGHT == action))
  {
    uint32_t oldStart = eventData->mLeftSelectionPosition;
    uint32_t oldEnd   = eventData->mRightSelectionPosition;

    impl.ChangeState(EventData::SELECTING);

    eventData->mLeftSelectionPosition  = selectionStart;
    eventData->mRightSelectionPosition = selectionEnd;

    eventData->mUpdateLeftSelectionPosition  = true;
    eventData->mUpdateRightSelectionPosition = true;
    eventData->mUpdateHighlightBox           = true;

    // It may happen an InputMethodContext commit event arrives before the selection event
    // if the InputMethodContext is in pre-edit state. The commit event will set the
    // eventData->mUpdateCursorPosition flag to true. If it's not set back
    // to false, the highlight box won't be updated.
    eventData->mUpdateCursorPosition = false;

    eventData->mScrollAfterUpdatePosition = (eventData->mLeftSelectionPosition != eventData->mRightSelectionPosition);

    // Cursor to be positioned at end of selection so if selection interrupted and edit mode restarted the cursor will
    // be at end of selection
    eventData->mPrimaryCursorPosition = std::max(eventData->mLeftSelectionPosition, eventData->mRightSelectionPosition);

    if(impl.mSelectableControlInterface != nullptr)
    {
      impl.mSelectableControlInterface->SelectionChanged(oldStart, oldEnd, eventData->mLeftSelectionPosition,
                                                         eventData->mRightSelectionPosition);
    }
  }
  else if(Controller::NoTextTap::SHOW_SELECTION_POPUP == action)
  {
    // Nothing to select. i.e. a white space, out of bounds
    impl.ChangeState(EventData::EDITING_WITH_POPUP);

    eventData->mPrimaryCursorPosition = noTextHitIndex;

    eventData->mUpdateCursorPosition      = true;
    eventData->mUpdateGrabHandlePosition  = true;
    eventData->mScrollAfterUpdatePosition = true;
    eventData->mUpdateInputStyle          = true;
  }
  else if(Controller::NoTextTap::NO_ACTION == action)
  {
    // Nothing to select. i.e. a white space, out of bounds
    eventData->mPrimaryCursorPosition = noTextHitIndex;

    eventData->mUpdateCursorPosition      = true;
    eventData->mUpdateGrabHandlePosition  = true;
    eventData->mScrollAfterUpdatePosition = true;
    eventData->mUpdateInputStyle          = true;
  }
}

void SelectionHandleController::Update(Controller::Impl& impl, HandleType handleType, const CursorInfo& cursorInfo)
{
  if((LEFT_SELECTION_HANDLE != handleType) && (RIGHT_SELECTION_HANDLE != handleType))
  {
    return;
  }

  ModelPtr&     model          = impl.mModel;
  const Vector2 cursorPosition = cursorInfo.primaryPosition + model->mScrollPosition;

  // Use the visible text area top, not the line box top.
  const float visibleLineTop = cursorInfo.primaryPosition.y - cursorInfo.glyphOffset;

  // Sets the handle's position.
  EventData*& eventData = impl.mEventData;
  eventData->mDecorator->SetPosition(handleType, cursorPosition.x, visibleLineTop + model->mScrollPosition.y,
                                     cursorInfo.lineHeight);

  // If selection handle at start of the text and other at end of the text then all text is selected.
  const CharacterIndex startOfSelection =
    std::min(eventData->mLeftSelectionPosition, eventData->mRightSelectionPosition);
  const CharacterIndex endOfSelection = std::max(eventData->mLeftSelectionPosition, eventData->mRightSelectionPosition);
  eventData->mAllTextSelected         = (startOfSelection == 0) && (endOfSelection == model->mLogicalModel->mText.Count());
}

} // namespace Text

} // namespace Ui

} // namespace Dali
