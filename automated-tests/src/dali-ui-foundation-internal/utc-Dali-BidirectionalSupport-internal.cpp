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

#include <dali-ui-foundation/internal/text/bidirectional-support.h>
#include <dali-ui-foundation/internal/text/bidirectional-line-info-run.h>
#include <dali-ui-foundation/internal/text/bidirectional-paragraph-info-run.h>
#include <dali-ui-foundation/internal/text/script-run.h>
#include <dali-ui-test-suite-utils.h>
#include <cstdlib>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Text;

void utc_dali_bidirectional_support_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_bidirectional_support_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliBidirectionalMixedParagraphAndSplitLineP(void)
{
  UiTestApplication application;
  TextAbstraction::BidirectionalSupport bidi = TextAbstraction::BidirectionalSupport::Get();

  Vector<Character> text;
  for(Character character : {Character('a'), Character('b'), Character(' '),
                             Character(0x05D0), Character('('), Character(0x05D1),
                             Character(')'), Character('\n'), Character('c')})
  {
    text.PushBack(character);
  }

  Vector<ScriptRun> scripts;
  scripts.PushBack({{0u, 3u}, TextAbstraction::LATIN, false});
  scripts.PushBack({{3u, 5u}, TextAbstraction::HEBREW, true});
  scripts.PushBack({{8u, 1u}, TextAbstraction::LATIN, false});

  Vector<LineBreakInfo> lineBreaks;
  lineBreaks.Resize(text.Count());
  for(uint32_t index = 0u; index < text.Count(); ++index)
  {
    lineBreaks[index] = TextAbstraction::LINE_NO_BREAK;
  }
  lineBreaks[7u] = TextAbstraction::LINE_MUST_BREAK;
  lineBreaks[8u] = TextAbstraction::LINE_MUST_BREAK;

  Vector<BidirectionalParagraphInfoRun> paragraphs;
  Vector<BidirectionalLineInfoRun> lines;
  SetBidirectionalInfo(bidi, text, scripts, lineBreaks, 0u, text.Count(), paragraphs, lines);
  DALI_TEST_EQUALS(paragraphs.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(paragraphs[0u].characterRun.characterIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(paragraphs[0u].characterRun.numberOfCharacters, 8u, TEST_LOCATION);

  Vector<CharacterDirection> directions;
  GetCharactersDirection(bidi, paragraphs, text.Count(), 0u, text.Count(), directions);
  DALI_TEST_CHECK(!directions[0u]);
  DALI_TEST_EQUALS(paragraphs[0u].direction, true, TEST_LOCATION);
  DALI_TEST_EQUALS(directions.Count(), text.Count(), TEST_LOCATION);

  ReorderLine(bidi, paragraphs[0u], lines, 0u, 0u, 8u, 0u, 0u, paragraphs[0u].direction);
  DALI_TEST_EQUALS(lines.Count(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(lines[0u].visualToLogicalMap != nullptr);
  DALI_TEST_CHECK(!lines[0u].isIdentity);

  ReorderLine(bidi, paragraphs[0u], lines, 1u, 0u, 4u, 4u, 4u, paragraphs[0u].direction);
  DALI_TEST_EQUALS(lines.Count(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(lines[1u].visualToLogicalMap != nullptr);
  DALI_TEST_CHECK(lines[1u].visualToLogicalMapSecondHalf != nullptr);

  Vector<Character> mirrored;
  DALI_TEST_CHECK(GetMirroredText(bidi, text, directions, paragraphs, 0u, text.Count(), mirrored));
  DALI_TEST_EQUALS(mirrored.Count(), text.Count(), TEST_LOCATION);

  for(auto& line : lines)
  {
    free(line.visualToLogicalMap);
    free(line.visualToLogicalMapSecondHalf);
  }
  bidi.DestroyInfo(paragraphs[0u].bidirectionalInfoIndex);

  END_TEST;
}
