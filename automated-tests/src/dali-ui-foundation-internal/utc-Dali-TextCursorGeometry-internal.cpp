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
#include <dali-ui-foundation/internal/text/text-geometry.h>
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
  const Text::LineIndex lastLine = visual->mLines.Count() - 1u;
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
