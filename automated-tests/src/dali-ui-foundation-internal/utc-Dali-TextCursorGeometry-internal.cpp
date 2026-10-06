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

#include <dali-ui-foundation/internal/text/controller/text-controller-impl.h>
#include <dali-ui-foundation/internal/text/cursor-helper-functions.h>
#include <dali-ui-foundation/internal/text/decorator/text-decorator.h>
#include <dali-ui-foundation/internal/text/text-geometry.h>
#include <dali-ui-foundation/internal/text/text-selection-handle-controller.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_text_cursor_geometry_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_cursor_geometry_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
Text::ControllerPtr CreateController(const char* text, const Size& size)
{
  Text::ControllerPtr controller = Text::Controller::New();
  controller->SetDefaultFontSize(20.0f, Text::Controller::PIXEL_SIZE);
  controller->SetMultiLineEnabled(true);
  controller->SetLineWrapMode(Text::LineWrapMode::WORD);
  controller->SetText(text);
  controller->Relayout(size);
  return controller;
}
} // namespace

int UtcDaliTextCursorClosestLineAndIndexP(void)
{
  UiTestApplication application(UiConfig::New());
  Text::ControllerPtr controller = CreateController("one two three four five six seven", Size(95.0f, 180.0f));
  Text::Controller::Impl& impl = Text::Controller::Impl::GetImplementation(*controller.Get());
  Text::VisualModelPtr visual = impl.mModel->mVisualModel;
  Text::LogicalModelPtr logical = impl.mModel->mLogicalModel;

  DALI_TEST_CHECK(visual->mLines.Count() > 1u);

  bool matched = true;
  DALI_TEST_EQUALS(Text::GetClosestLine(visual, -20.0f, matched), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!matched);
  const Text::LineIndex lastLine = static_cast<Text::LineIndex>(visual->mLines.Count() - 1u);
  DALI_TEST_EQUALS(Text::GetClosestLine(visual, 1000.0f, matched), lastLine, TEST_LOCATION);
  DALI_TEST_CHECK(!matched);
  DALI_TEST_EQUALS(Text::GetClosestLine(visual, 1.0f, matched), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(matched);

  DALI_TEST_EQUALS(Text::CalculateLineOffset(visual->mLines, 0u), 0.0f, TEST_LOCATION);
  DALI_TEST_CHECK(Text::CalculateLineOffset(visual->mLines, lastLine) > 0.0f);

  bool matchedCharacter = false;
  Text::CharacterIndex left = Text::GetClosestCursorIndex(
    visual, logical, impl.mMetrics, -50.0f, 1.0f, Text::CharacterHitTest::TAP, matchedCharacter);
  DALI_TEST_EQUALS(left, 0u, TEST_LOCATION);
  Text::CharacterIndex right = Text::GetClosestCursorIndex(
    visual, logical, impl.mMetrics, 500.0f, 1.0f, Text::CharacterHitTest::TAP, matchedCharacter);
  DALI_TEST_CHECK(right > 0u);
  Text::GetClosestCursorIndex(
    visual, logical, impl.mMetrics, 10.0f, -30.0f, Text::CharacterHitTest::SCROLL, matchedCharacter);
  Text::GetClosestCursorIndex(
    visual, logical, impl.mMetrics, 10.0f, 1000.0f, Text::CharacterHitTest::SCROLL, matchedCharacter);
  Text::GetClosestCursorIndex(
    visual, logical, impl.mMetrics, 20.0f, 10.0f, Text::CharacterHitTest::TAP, matchedCharacter);
  END_TEST;
}

int UtcDaliTextCursorSelectionAndPositionP(void)
{
  UiTestApplication application(UiConfig::New());
  Text::ControllerPtr controller = CreateController("word  spaces\nnext אבג line", Size(260.0f, 120.0f));
  Text::Controller::Impl& impl = Text::Controller::Impl::GetImplementation(*controller.Get());
  Text::VisualModelPtr visual = impl.mModel->mVisualModel;
  Text::LogicalModelPtr logical = impl.mModel->mLogicalModel;

  Text::CharacterIndex start = 0u;
  Text::CharacterIndex end = 0u;
  Text::CharacterIndex noHit = 0u;
  Text::FindSelectionIndices(visual, logical, impl.mMetrics, 12.0f, 8.0f, start, end, noHit);
  DALI_TEST_CHECK(end >= start);
  Text::FindSelectionIndices(visual, logical, impl.mMetrics, 70.0f, 8.0f, start, end, noHit);
  Text::FindSelectionIndices(visual, logical, impl.mMetrics, 500.0f, 8.0f, start, end, noHit);
  Text::FindSelectionIndices(visual, logical, impl.mMetrics, 10.0f, 55.0f, start, end, noHit);

  Text::GetCursorPositionParameters parameters;
  parameters.visualModel = visual;
  parameters.logicalModel = logical;
  parameters.metrics = impl.mMetrics;
  parameters.verticalLineAlignment = Text::Alignment::CENTER;
  parameters.isMultiline = true;

  for(Text::CharacterIndex index : {0u, 4u, 6u, 13u, static_cast<Text::CharacterIndex>(logical->mText.Count())})
  {
    parameters.logical = index;
    Text::CursorInfo cursorInfo;
    Text::GetCursorPosition(parameters, 20.0f, cursorInfo);
    DALI_TEST_CHECK(cursorInfo.lineHeight >= 0.0f);
  }
  END_TEST;
}

int UtcDaliTextGeometryRangesAndBoundsP(void)
{
  UiTestApplication application(UiConfig::New());
  Text::ControllerPtr controller = CreateController("alpha beta gamma delta", Size(120.0f, 120.0f));
  Text::Controller::Impl& impl = Text::Controller::Impl::GetImplementation(*controller.Get());
  Text::ModelPtr model = impl.mModel;
  Vector<Vector2> sizes;
  Vector<Vector2> positions;

  Text::GetTextGeometry(model, 0u, 4u, sizes, positions);
  DALI_TEST_CHECK(!sizes.Empty());
  DALI_TEST_EQUALS(sizes.Count(), positions.Count(), TEST_LOCATION);
  Text::GetTextGeometry(model, 2u, 18u, sizes, positions);
  DALI_TEST_CHECK(sizes.Count() >= 1u);
  Text::GetTextGeometry(model, 20u, 2u, sizes, positions);

  Bounds firstLine = Text::GetLineBoundingRect(model, 0u);
  DALI_TEST_CHECK(firstLine.width > 0.0f);
  DALI_TEST_EQUALS(Text::GetLineBoundingRect(model, 999u), Bounds(), TEST_LOCATION);
  DALI_TEST_CHECK(Text::GetLineLeft(model->mVisualModel->mLines[0u]) >= 0.0f);
  DALI_TEST_EQUALS(Text::GetLineTop(model->mVisualModel->mLines, model->mVisualModel->mLines[0u]), 0.0f, TEST_LOCATION);

  Bounds firstCharacter = Text::GetCharacterBoundingRect(model, 0u);
  DALI_TEST_CHECK(firstCharacter.width > 0.0f);
  DALI_TEST_EQUALS(Text::GetCharacterBoundingRect(model, 999u), Bounds(), TEST_LOCATION);
  DALI_TEST_CHECK(Text::GetCharIndexAtPosition(model, firstCharacter.x + 1.0f, firstCharacter.y + 1.0f) >= 0);
  DALI_TEST_EQUALS(Text::GetCharIndexAtPosition(model, -100.0f, -100.0f), -1, TEST_LOCATION);

  const Text::GlyphInfo& glyph = model->mVisualModel->mGlyphs[0u];
  const Vector2& position = model->mVisualModel->mGlyphPositions[0u];
  DALI_TEST_EQUALS(Text::GetCharacterLeft(glyph, position), position.x - glyph.xBearing, TEST_LOCATION);
  DALI_TEST_EQUALS(Text::GetCharacterHeight(glyph), glyph.height, TEST_LOCATION);
  DALI_TEST_EQUALS(Text::GetCharacterWidth(glyph), glyph.advance, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextCursorGeometryEmptyModelP(void)
{
  UiTestApplication application(UiConfig::New());
  Text::ControllerPtr controller = CreateController("", Size(100.0f, 40.0f));
  Text::Controller::Impl& impl = Text::Controller::Impl::GetImplementation(*controller.Get());
  bool matched = true;
  Text::CharacterIndex start = 1u;
  Text::CharacterIndex end = 1u;
  Text::CharacterIndex noHit = 1u;

  DALI_TEST_EQUALS(Text::GetClosestLine(impl.mModel->mVisualModel, 0.0f, matched), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!matched);
  DALI_TEST_CHECK(!Text::FindSelectionIndices(
    impl.mModel->mVisualModel, impl.mModel->mLogicalModel, impl.mMetrics, 0.0f, 0.0f, start, end, noHit));
  Text::CursorInfo cursorInfo;
  Text::GetCursorPositionParameters parameters;
  parameters.visualModel = impl.mModel->mVisualModel;
  parameters.logicalModel = impl.mModel->mLogicalModel;
  parameters.metrics = impl.mMetrics;
  parameters.logical = 0u;
  parameters.verticalLineAlignment = Text::Alignment::START;
  parameters.isMultiline = true;
  Text::GetCursorPosition(parameters, 20.0f, cursorInfo);
  DALI_TEST_EQUALS(cursorInfo.primaryPosition, Vector2::ZERO, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextSelectionHandleRepositionP(void)
{
  UiTestApplication application(UiConfig::New());
  Text::ControllerPtr controller = Text::Controller::New();
  Text::DecoratorPtr decorator = Text::Decorator::New(*controller, *controller);
  InputMethodContext inputMethodContext;
  controller->EnableTextInput(decorator, inputMethodContext);
  controller->SetDefaultFontSize(20.0f, Text::Controller::PIXEL_SIZE);
  controller->SetMultiLineEnabled(true);
  controller->SetText("one two three four five six seven eight nine");
  controller->KeyboardFocusGainEvent(false);
  controller->Relayout(Size(100.0f, 240.0f));

  Text::Controller::Impl& impl = Text::Controller::Impl::GetImplementation(*controller.Get());
  DALI_TEST_CHECK(impl.mModel->mVisualModel->mLines.Count() > 2u);
  impl.mEventData->mLeftSelectionPosition = 1u;
  impl.mEventData->mRightSelectionPosition = 35u;
  Text::SelectionHandleController::Reposition(impl);
  DALI_TEST_CHECK(impl.mEventData->mDecoratorUpdated);

  impl.mEventData->mLeftSelectionPosition = 35u;
  impl.mEventData->mRightSelectionPosition = 1u;
  Text::SelectionHandleController::Reposition(impl);

  impl.mEventData->mLeftSelectionPosition = 4u;
  impl.mEventData->mRightSelectionPosition = 4u;
  Text::SelectionHandleController::Reposition(impl);
  END_TEST;
}

int UtcDaliTextSelectionHandleHitAndUpdateP(void)
{
  UiTestApplication application(UiConfig::New());
  Text::ControllerPtr controller = Text::Controller::New();
  Text::DecoratorPtr decorator = Text::Decorator::New(*controller, *controller);
  InputMethodContext inputMethodContext;
  controller->EnableTextInput(decorator, inputMethodContext);
  controller->SetDefaultFontSize(20.0f, Text::Controller::PIXEL_SIZE);
  controller->SetMultiLineEnabled(true);
  controller->SetText("select words here");
  controller->Relayout(Size(180.0f, 80.0f));
  Text::Controller::Impl& impl = Text::Controller::Impl::GetImplementation(*controller.Get());

  Text::SelectionHandleController::Reposition(impl, 15.0f, 8.0f, Text::Controller::NoTextTap::HIGHLIGHT);
  Text::SelectionHandleController::Reposition(impl, 1000.0f, 8.0f, Text::Controller::NoTextTap::SHOW_SELECTION_POPUP);
  Text::SelectionHandleController::Reposition(impl, -100.0f, 8.0f, Text::Controller::NoTextTap::NO_ACTION);

  Text::CursorInfo cursorInfo;
  impl.GetCursorPosition(0u, cursorInfo);
  Text::SelectionHandleController::Update(impl, Text::LEFT_SELECTION_HANDLE, cursorInfo);
  Text::SelectionHandleController::Update(impl, Text::RIGHT_SELECTION_HANDLE, cursorInfo);
  Text::SelectionHandleController::Update(impl, Text::GRAB_HANDLE, cursorInfo);

  Text::ControllerPtr emptyController = Text::Controller::New();
  Text::Controller::Impl& emptyImpl = Text::Controller::Impl::GetImplementation(*emptyController.Get());
  Text::SelectionHandleController::Reposition(emptyImpl, 0.0f, 0.0f, Text::Controller::NoTextTap::NO_ACTION);
  DALI_TEST_CHECK(impl.mEventData);
  END_TEST;
}

int UtcDaliTextGeometryEllipsisAndBidiMatrixP(void)
{
  UiTestApplication application(UiConfig::New());
  for(const char* content : {"office affinity fish", "abc אבגדה def مرحبا xyz", "one two three four five six"})
  {
    for(auto position : {Text::EllipsisPosition::START, Text::EllipsisPosition::MIDDLE, Text::EllipsisPosition::END})
    {
      Text::ControllerPtr controller = CreateController(content, Size(72.0f, 32.0f));
      controller->SetTextElideEnabled(true);
      controller->SetMaximumNumberOfLines(1);
      controller->SetEllipsisPosition(position);
      controller->Relayout(Size(72.0f, 32.0f));
      Text::Controller::Impl& impl = Text::Controller::Impl::GetImplementation(*controller.Get());
      Text::ModelPtr model = impl.mModel;
      Text::VisualModelPtr visual = model->mVisualModel;
      Text::LogicalModelPtr logical = model->mLogicalModel;
      const auto* finalElision = controller->GetFinalElisionResult();
      Vector<Vector2> sizes;
      Vector<Vector2> positions;
      const Text::CharacterIndex last = static_cast<Text::CharacterIndex>(logical->mText.Count() - 1u);
      Text::GetTextGeometry(model, 0u, last, sizes, positions);
      Text::GetTextGeometry(model, 1u, last - 1u, sizes, positions, finalElision);
      Text::GetTextGeometry(model, last, 0u, sizes, positions);
      DALI_TEST_EQUALS(sizes.Count(), positions.Count(), TEST_LOCATION);
      for(Text::CharacterIndex index = 0u; index <= last; ++index)
      {
        if(visual->GetLineOfCharacter(index) < visual->mLines.Count())
        {
          Text::GetCharacterBoundingRect(model, index, finalElision);
        }
      }
      for(float y : {-10.0f, 0.0f, 15.0f, 100.0f})
      {
        bool matched = false;
        Text::GetClosestCursorIndex(visual, logical, impl.mMetrics, 12.0f, y, Text::CharacterHitTest::TAP, matched);
        Text::GetClosestCursorIndex(visual, logical, impl.mMetrics, 12.0f, y, Text::CharacterHitTest::SCROLL, matched);
        Text::CharacterIndex start = 0u;
        Text::CharacterIndex end = 0u;
        Text::CharacterIndex noHit = 0u;
        Text::FindSelectionIndices(visual, logical, impl.mMetrics, 12.0f, y, start, end, noHit);
      }
    }
  }
  END_TEST;
}

int UtcDaliTextCursorWhitespaceBidiAndLigatureMatrixP(void)
{
  UiTestApplication application(UiConfig::New());
  for(const char* content : {"  one   two\n\nthree  ", "abc אבג def\nمرحبا xyz", "office affinity fi ffi\n", "A 👨‍👩‍👧‍👦 B"})
  {
    Text::ControllerPtr controller = CreateController(content, Size(110.0f, 280.0f));
    Text::Controller::Impl& impl = Text::Controller::Impl::GetImplementation(*controller.Get());
    Text::VisualModelPtr visual = impl.mModel->mVisualModel;
    Text::LogicalModelPtr logical = impl.mModel->mLogicalModel;
    DALI_TEST_CHECK(!visual->mLines.Empty());

    Text::GetCursorPositionParameters parameters;
    parameters.visualModel = visual;
    parameters.logicalModel = logical;
    parameters.metrics = impl.mMetrics;
    parameters.verticalLineAlignment = Text::Alignment::END;
    parameters.isMultiline = true;
    for(Text::CharacterIndex index = 0u; index <= logical->mText.Count(); ++index)
    {
      parameters.logical = index;
      Text::CursorInfo cursor;
      Text::GetCursorPosition(parameters, 20.0f, cursor);
    }

    for(float y : {-4.0f, 1.0f, 15.0f, 28.0f, 40.0f, 60.0f, 120.0f, 300.0f})
    {
      for(float x : {-20.0f, 0.0f, 12.0f, 30.0f, 60.0f, 90.0f, 150.0f})
      {
        bool matched = false;
        Text::GetClosestCursorIndex(visual, logical, impl.mMetrics, x, y, Text::CharacterHitTest::TAP, matched);
        Text::GetClosestCursorIndex(visual, logical, impl.mMetrics, x, y, Text::CharacterHitTest::SCROLL, matched);
        Text::CharacterIndex start = 0u;
        Text::CharacterIndex end = 0u;
        Text::CharacterIndex noHit = 0u;
        Text::FindSelectionIndices(visual, logical, impl.mMetrics, x, y, start, end, noHit);
      }
    }
  }
  END_TEST;
}

int UtcDaliTextGeometryMappedLigatureSlicesP(void)
{
  UiTestApplication application(UiConfig::New());
  Text::ControllerPtr controller = CreateController("afib", Size(200.0f, 80.0f));
  Text::Controller::Impl& impl = Text::Controller::Impl::GetImplementation(*controller.Get());
  Text::ModelPtr model = impl.mModel;
  Text::VisualModelPtr visual = model->mVisualModel;
  DALI_TEST_CHECK(visual->mGlyphs.Count() >= 4u);

  // Model the character-to-glyph tables emitted by a font that shapes "fi" as one ligature.
  // The local mock font does not combine those glyphs by itself.
  visual->mCharactersToGlyph[2u] = visual->mCharactersToGlyph[1u];
  visual->mGlyphsPerCharacter[2u] = 0u;
  visual->mCharactersPerGlyph[visual->mCharactersToGlyph[1u]] = 2u;
  Vector<Vector2> sizes;
  Vector<Vector2> positions;
  Text::GetTextGeometry(model, 1u, 2u, sizes, positions);
  DALI_TEST_CHECK(!sizes.Empty());
  DALI_TEST_EQUALS(sizes.Count(), positions.Count(), TEST_LOCATION);
  Text::GetTextGeometry(model, 0u, 2u, sizes, positions);
  DALI_TEST_CHECK(!sizes.Empty());
  Text::GetTextGeometry(model, 2u, 2u, sizes, positions);
  DALI_TEST_CHECK(!sizes.Empty());
  Text::GetCursorPositionParameters parameters;
  parameters.visualModel = visual;
  parameters.logicalModel = model->mLogicalModel;
  parameters.metrics = impl.mMetrics;
  parameters.isMultiline = true;
  parameters.verticalLineAlignment = Text::Alignment::START;
  parameters.logical = 2u;
  Text::CursorInfo cursor;
  Text::GetCursorPosition(parameters, 20.0f, cursor);
  DALI_TEST_CHECK(cursor.lineHeight > 0.0f);
  bool matched = false;
  for(float x : {0.0f, 10.0f, 20.0f, 30.0f, 40.0f})
  {
    Text::GetClosestCursorIndex(visual, model->mLogicalModel, impl.mMetrics, x, 10.0f,
                                Text::CharacterHitTest::TAP, matched);
  }
  END_TEST;
}

int UtcDaliTextGeometryOutOfRangeAndEmptyModelP(void)
{
  UiTestApplication application(UiConfig::New());
  Vector<Vector2> sizes;
  Vector<Vector2> positions;
  Text::ControllerPtr empty = CreateController("", Size(100.0f, 40.0f));
  Text::ModelPtr emptyModel = Text::Controller::Impl::GetImplementation(*empty.Get()).mModel;
  Text::GetTextGeometry(emptyModel, 0u, 0u, sizes, positions);
  DALI_TEST_CHECK(sizes.Empty());
  DALI_TEST_EQUALS(Text::GetCharIndexAtPosition(Text::ModelPtr(), 0.0f, 0.0f), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(Text::GetCharIndexAtPosition(emptyModel, 0.0f, 0.0f), -1, TEST_LOCATION);

  Text::ControllerPtr controller = CreateController("alpha beta", Size(200.0f, 40.0f));
  Text::ModelPtr model = Text::Controller::Impl::GetImplementation(*controller.Get()).mModel;
  Text::GetTextGeometry(model, 999u, 999u, sizes, positions);
  DALI_TEST_CHECK(sizes.Empty());
  Text::GetTextGeometry(model, 999u, 0u, sizes, positions);
  Text::GetTextGeometry(model, 0u, 999u, sizes, positions);
  DALI_TEST_CHECK(!sizes.Empty());

  Text::VisualModelPtr visual = model->mVisualModel;
  model->mVisualModel = nullptr;
  DALI_TEST_EQUALS(Text::GetLineBoundingRect(model, 0u), Bounds(), TEST_LOCATION);
  DALI_TEST_EQUALS(Text::GetCharacterBoundingRect(model, 0u), Bounds(), TEST_LOCATION);
  model->mVisualModel = visual;
  END_TEST;
}

int UtcDaliTextCursorBidirectionalBoundaryP(void)
{
  UiTestApplication application(UiConfig::New());
  Text::ControllerPtr controller = CreateController("abcd", Size(200.0f, 40.0f));
  Text::Controller::Impl& impl = Text::Controller::Impl::GetImplementation(*controller.Get());
  Text::VisualModelPtr visual = impl.mModel->mVisualModel;
  Text::LogicalModelPtr logical = impl.mModel->mLogicalModel;
  DALI_TEST_EQUALS(visual->mLines.Count(), 1u, TEST_LOCATION);

  logical->mCharacterDirections.Clear();
  for(bool rightToLeft : {false, false, true, true})
  {
    logical->mCharacterDirections.PushBack(rightToLeft);
  }

  Text::BidirectionalLineInfoRun bidiLine{};
  bidiLine.characterRun = visual->mLines[0u].characterRun;
  bidiLine.visualToLogicalMap = static_cast<Text::CharacterIndex*>(malloc(4u * sizeof(Text::CharacterIndex)));
  DALI_TEST_CHECK(bidiLine.visualToLogicalMap);
  for(Text::CharacterIndex index = 0u; index < 4u; ++index)
  {
    bidiLine.visualToLogicalMap[index] = index;
  }
  bidiLine.direction = false;
  bidiLine.isIdentity = true;
  logical->mBidirectionalLineInfo.PushBack(bidiLine);

  Text::GetCursorPositionParameters parameters;
  parameters.visualModel = visual;
  parameters.logicalModel = logical;
  parameters.metrics = impl.mMetrics;
  parameters.isMultiline = true;
  parameters.verticalLineAlignment = Text::Alignment::START;
  for(Text::CharacterIndex index = 0u; index <= 4u; ++index)
  {
    parameters.logical = index;
    Text::CursorInfo cursor;
    Text::GetCursorPosition(parameters, 20.0f, cursor);
    DALI_TEST_CHECK(cursor.lineHeight > 0.0f);
    if(index == 2u || index == 4u)
    {
      DALI_TEST_CHECK(cursor.isSecondaryCursor);
    }
  }
  END_TEST;
}
