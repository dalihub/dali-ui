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
 */

#include <dali-ui-foundation/integration-api/text/input-style.h>
#include <dali-ui-foundation/internal/text/logical-model-impl.h>
#include <dali-ui-test-suite-utils.h>
#include <cstdlib>
#include <cstring>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Text;

void utc_dali_text_logical_model_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_logical_model_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextLogicalModelRunsAndStyleP(void)
{
  UiTestApplication application;
  LogicalModelPtr model = LogicalModel::New();
  model->mText.PushBack('a');
  model->mText.PushBack('b');
  model->mText.PushBack('c');
  model->mText.PushBack('d');

  ScriptRun script{{1u, 2u}, TextAbstraction::LATIN, false};
  model->mScriptRuns.PushBack(script);
  DALI_TEST_EQUALS(static_cast<int>(model->GetScript(1u)), static_cast<int>(TextAbstraction::LATIN), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(model->GetScript(3u)), static_cast<int>(TextAbstraction::UNKNOWN), TEST_LOCATION);
  DALI_TEST_CHECK(!model->GetCharacterDirection(10u));
  model->mCharacterDirections.PushBack(false);
  model->mCharacterDirections.PushBack(true);
  DALI_TEST_CHECK(model->GetCharacterDirection(1u));

  ColorRun color;
  color.characterRun = {0u, 3u};
  color.color = Color::RED;
  model->mColorRuns.PushBack(color);

  char* family = new char[5];
  std::memcpy(family, "Sans", 5u);
  FontDescriptionRun font({0u, 3u}, family, 4u,
                          TextAbstraction::FontWeight::BOLD,
                          TextAbstraction::FontWidth::CONDENSED,
                          TextAbstraction::FontSlant::ITALIC,
                          12 * 64, true, true, true, true, true);
  model->mFontDescriptionRuns.PushBack(font);
  Ui::Integration::Text::InputStyle style;
  model->RetrieveStyle(1u, style);
  DALI_TEST_CHECK(!style.isDefaultColor);
  DALI_TEST_EQUALS(style.textColor, Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(style.familyName, std::string("Sans"), TEST_LOCATION);
  DALI_TEST_CHECK(style.isWeightDefined && style.isWidthDefined && style.isSlantDefined && style.isSizeDefined);
  DALI_TEST_EQUALS(style.size, 12.0f, 0.001f, TEST_LOCATION);

  model->UpdateTextStyleRuns(1u, 1);
  model->UpdateTextStyleRuns(1u, -1);
  model->ClearStrikethroughRuns();
  model->ClearUnderlineRuns();
  model->ClearFontDescriptionRuns();
  END_TEST;
}

int UtcDaliTextLogicalModelParagraphAndBidiP(void)
{
  UiTestApplication application;
  LogicalModelPtr model = LogicalModel::New();
  for(uint32_t i = 0u; i < 8u; ++i)
  {
    model->mText.PushBack('a' + i);
    model->mCharacterDirections.PushBack(i % 2u != 0u);
    model->mLineBreakInfo.PushBack((i == 2u || i == 5u || i == 7u)
                                    ? TextAbstraction::LINE_MUST_BREAK
                                    : TextAbstraction::LINE_NO_BREAK);
  }
  model->CreateParagraphInfo(0u, 8u);
  DALI_TEST_EQUALS(model->mParagraphInfo.Count(), 3u, TEST_LOCATION);
  Vector<ParagraphRunIndex> paragraphs;
  model->FindParagraphs(1u, 5u, paragraphs);
  DALI_TEST_EQUALS(paragraphs.Count(), 2u, TEST_LOCATION);
  model->CreateParagraphInfo(3u, 3u);

  BidirectionalLineInfoRun first{};
  first.characterRun = {0u, 4u};
  first.visualToLogicalMap = static_cast<CharacterIndex*>(std::calloc(4u, sizeof(CharacterIndex)));
  first.visualToLogicalMap[0] = 3u;
  first.visualToLogicalMap[1] = 2u;
  first.visualToLogicalMap[2] = 1u;
  first.visualToLogicalMap[3] = 0u;
  first.direction = true;
  first.isIdentity = false;
  model->mBidirectionalLineInfo.PushBack(first);

  BidirectionalLineInfoRun second{};
  second.characterRun = {6u, 2u};
  second.visualToLogicalMap = static_cast<CharacterIndex*>(std::calloc(2u, sizeof(CharacterIndex)));
  second.visualToLogicalMap[0] = 0u;
  second.visualToLogicalMap[1] = 1u;
  second.direction = false;
  second.isIdentity = true;
  model->mBidirectionalLineInfo.PushBack(second);

  DALI_TEST_CHECK(model->FetchBidirectionalLineInfo(1u));
  DALI_TEST_EQUALS(model->GetLogicalCharacterIndex(0u), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(model->GetLogicalCursorIndex(0u), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(model->GetLogicalCursorIndex(4u), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!model->FetchBidirectionalLineInfo(5u));
  DALI_TEST_CHECK(model->FetchBidirectionalLineInfo(6u));
  DALI_TEST_EQUALS(model->GetBidirectionalLineInfo(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!model->FetchBidirectionalLineInfo(20u));

  BoundedParagraphRun bounded;
  bounded.characterRun = {0u, 2u};
  model->mBoundedParagraphRuns.PushBack(bounded);
  CharacterSpacingCharacterRun spacing;
  spacing.characterRun = {0u, 2u};
  model->mCharacterSpacingCharacterRuns.PushBack(spacing);
  DALI_TEST_EQUALS(model->GetNumberOfBoundedParagraphRuns(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(model->GetBoundedParagraphRuns().Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(model->GetNumberOfCharacterSpacingCharacterRuns(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(model->GetCharacterSpacingCharacterRuns().Count(), 1u, TEST_LOCATION);
  model->ClearAnchors();
  END_TEST;
}
