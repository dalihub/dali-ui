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

#include <sstream>
#include <string>

#include <dali/devel-api/text-abstraction/script.h>
#include <dali-ui-foundation/internal/text/bounded-paragraph-helper-functions.h>
#include <dali-ui-foundation/internal/text/font-run.h>
#include <dali-ui-foundation/internal/text/font-variation/font-variation-property-data.h>
#include <dali-ui-foundation/internal/text/line-run.h>
#include <dali-ui-foundation/internal/text/script-run.h>
#include <dali-ui-foundation/internal/text/text-io.h>
#include <dali-ui-foundation/internal/text/text-run-container.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
Text::FontRun MakeFontRun(Text::CharacterIndex index, Text::Length length)
{
  Text::FontRun run{};
  run.characterRun = Text::CharacterRun(index, length);
  return run;
}

Text::LineRun MakeLineRun(Text::GlyphIndex index, Text::Length length)
{
  Text::LineRun run{};
  run.glyphRun.glyphIndex      = index;
  run.glyphRun.numberOfGlyphs = length;
  run.characterRun            = Text::CharacterRun(index, length);
  run.width                   = static_cast<float>(length);
  run.ascender                = 8.0f;
  run.descender               = -2.0f;
  return run;
}

Text::BoundedParagraphRun MakeParagraphRun(Text::CharacterIndex index, Text::Length length)
{
  Text::BoundedParagraphRun run;
  run.characterRun = Text::CharacterRun(index, length);
  return run;
}
} // namespace

void utc_dali_text_utilities_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_utilities_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextRunContainerCharacterOperationsP(void)
{
  Vector<Text::FontRun> runs;
  runs.PushBack(MakeFontRun(0u, 3u));
  runs.PushBack(MakeFontRun(4u, 3u));
  runs.PushBack(MakeFontRun(8u, 2u));

  uint32_t first = runs.Count();
  uint32_t last  = first;
  Text::ClearCharacterRuns(2u, 5u, runs, first, last);
  DALI_TEST_EQUALS(first, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(last, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(runs[2].characterRun.characterIndex, 4u, TEST_LOCATION);

  runs.Clear();
  runs.PushBack(MakeFontRun(0u, 3u));
  runs.PushBack(MakeFontRun(4u, 3u));
  runs.PushBack(MakeFontRun(8u, 2u));
  Text::ClearCharacterRuns(2u, 5u, runs);
  DALI_TEST_EQUALS(runs.Count(), 1u, TEST_LOCATION);

  Vector<Text::FontRun> removed;
  runs.Clear();
  runs.PushBack(MakeFontRun(0u, 3u));
  runs.PushBack(MakeFontRun(4u, 3u));
  runs.PushBack(MakeFontRun(8u, 2u));
  Text::UpdateCharacterRuns(3u, -5, 10u, runs, removed);
  DALI_TEST_CHECK(!runs.Empty());
  DALI_TEST_CHECK(!removed.Empty());

  removed.Clear();
  Text::UpdateCharacterRuns(0u, -10, 10u, runs, removed);
  DALI_TEST_CHECK(runs.Empty());
  DALI_TEST_CHECK(!removed.Empty());

  runs.PushBack(MakeFontRun(0u, 2u));
  runs.PushBack(MakeFontRun(4u, 2u));
  runs.PushBack(MakeFontRun(8u, 0u));
  Text::UpdateCharacterRuns(0u, 2, 4u, runs, removed);
  Text::UpdateCharacterRuns(3u, 2, 6u, runs, removed);
  Text::UpdateCharacterRuns(10u, 2, 8u, runs, removed);
  DALI_TEST_EQUALS(runs[0].characterRun.numberOfCharacters, 6u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRunContainerGlyphOperationsP(void)
{
  Vector<Text::LineRun> runs;
  runs.PushBack(MakeLineRun(0u, 3u));
  runs.PushBack(MakeLineRun(4u, 3u));
  runs.PushBack(MakeLineRun(8u, 2u));

  uint32_t first = runs.Count();
  uint32_t last  = first;
  Text::ClearGlyphRuns(2u, 5u, runs, first, last);
  DALI_TEST_EQUALS(first, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(last, 2u, TEST_LOCATION);
  Text::ClearGlyphRuns(2u, 5u, runs);
  DALI_TEST_EQUALS(runs.Count(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliBoundedParagraphMergeP(void)
{
  Vector<Text::Character> text;
  for(uint32_t index = 0u; index < 16u; ++index)
  {
    text.PushBack(index == 7u ? '\n' : 'a');
  }

  Vector<Text::BoundedParagraphRun> runs;
  Text::MergeBoundedParagraphRunsWhenRemoveCharacters(text, 0u, -1, runs);
  runs.PushBack(MakeParagraphRun(4u, 3u));
  Text::MergeBoundedParagraphRunsWhenRemoveCharacters(text, 0u, 1, runs);
  Text::MergeBoundedParagraphRunsWhenRemoveCharacters(text, 0u, -1, runs);
  DALI_TEST_EQUALS(runs.Count(), 1u, TEST_LOCATION);

  runs.Clear();
  runs.PushBack(MakeParagraphRun(0u, 3u));
  runs.PushBack(MakeParagraphRun(4u, 3u));
  runs.PushBack(MakeParagraphRun(8u, 3u));
  runs.PushBack(MakeParagraphRun(12u, 3u));
  Text::MergeBoundedParagraphRunsWhenRemoveCharacters(text, 2u, -9, runs);
  DALI_TEST_CHECK(!runs.Empty());

  runs.Clear();
  runs.PushBack(MakeParagraphRun(4u, 3u));
  Text::MergeBoundedParagraphRunsWhenRemoveCharacters(text, 3u, -2, runs);
  DALI_TEST_CHECK(runs.Count() <= 1u);
  END_TEST;
}

int UtcDaliTextIoP(void)
{
  UiTestApplication application;
  Vector<Text::Character> characters;
  characters.PushBack('A');
  characters.PushBack(0x20acu);
  std::ostringstream characterStream;
  Text::operator<<(characterStream, characters);
  DALI_TEST_EQUALS(characterStream.str(), std::string("41 20ac"), TEST_LOCATION);

  Vector<Text::ScriptRun> scripts;
  scripts.PushBack({Text::CharacterRun(0u, 2u), TextAbstraction::LATIN, false});
  scripts.PushBack({Text::CharacterRun(2u, 1u), TextAbstraction::ARABIC, true});
  std::ostringstream scriptStream;
  Text::operator<<(scriptStream, scripts);
  DALI_TEST_CHECK(scriptStream.str().find("LATIN") != std::string::npos);

  Vector<Text::FontRun> fonts;
  fonts.PushBack(MakeFontRun(0u, 2u));
  fonts.PushBack(MakeFontRun(2u, 1u));
  std::ostringstream fontStream;
  Text::operator<<(fontStream, fonts);
  DALI_TEST_CHECK(fontStream.str().find("ID:0") != std::string::npos);

  Vector<Text::LineRun> lines;
  lines.PushBack(MakeLineRun(0u, 2u));
  lines.PushBack(MakeLineRun(2u, 3u));
  std::ostringstream lineStream;
  Text::operator<<(lineStream, lines);
  DALI_TEST_CHECK(lineStream.str().find("Line 1") != std::string::npos);
  END_TEST;
}

int UtcDaliFontVariationPropertyDataP(void)
{
  UiTestApplication application;
  namespace UiTextInternal = Dali::Ui::Internal::Text;

  View emptyOwner;
  DALI_TEST_CHECK(UiTextInternal::GetFontVariationPropertyData(emptyOwner) == nullptr);

  View owner = View::New();
  DALI_TEST_CHECK(UiTextInternal::GetFontVariationPropertyData(owner) == nullptr);
  auto& data = UiTextInternal::GetOrCreateFontVariationPropertyData(owner);
  DALI_TEST_CHECK(&data == UiTextInternal::GetFontVariationPropertyData(owner));
  DALI_TEST_CHECK(&data == &UiTextInternal::GetOrCreateFontVariationPropertyData(owner));

  Property::Index weightIndex = owner.RegisterProperty("variation-weight", 400.0f);
  Property::Index widthIndex  = owner.RegisterProperty("variation-width", 100.0f);
  DALI_TEST_CHECK(data.Insert(weightIndex, "wght"));
  DALI_TEST_CHECK(data.Insert(widthIndex, "wdth"));
  DALI_TEST_CHECK(!data.Insert(weightIndex, "duplicate"));

  Dali::String tag;
  DALI_TEST_CHECK(data.Find(weightIndex, tag));
  DALI_TEST_EQUALS(tag, Dali::String("wght"), TEST_LOCATION);
  DALI_TEST_CHECK(!data.Find(Property::INVALID_INDEX, tag));

  Property::Map map;
  data.ApplyCurrentPropertyValues(Actor(), map);
  DALI_TEST_EQUALS(map.Count(), 0u, TEST_LOCATION);
  owner.SetProperty(weightIndex, 650.0f);
  owner.SetProperty(widthIndex, 120.0f);
  application.GetScene().Add(owner);
  application.SendNotification();
  application.Render();
  data.ApplyCurrentPropertyValues(owner, map);
  DALI_TEST_EQUALS(map["wght"].Get<float>(), 650.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(map["wdth"].Get<float>(), 120.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}
