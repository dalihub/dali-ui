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
#include <dali-ui-foundation/internal/text/character-set-conversion.h>
#include <dali-ui-foundation/internal/text/emoji-helper.h>
#include <dali-ui-foundation/internal/text/characters-helper-functions.h>
#include <dali-ui-foundation/internal/text/logical-model-impl.h>
#include <dali-ui-foundation/internal/text/markup-processor/markup-processor-helper-functions.h>
#include <dali-ui-foundation/internal/text/visual-model-impl.h>
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

  uint32_t first = static_cast<uint32_t>(runs.Count());
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

  uint32_t first = static_cast<uint32_t>(runs.Count());
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

int UtcDaliTextClusteredCharacterRunP(void)
{
  Text::VisualModelPtr visual = Text::VisualModel::New();
  Text::LogicalModelPtr logical = Text::LogicalModel::New();
  visual->mCharactersToGlyph.PushBack(0u);
  visual->mCharactersToGlyph.PushBack(0u);
  visual->mCharactersPerGlyph.PushBack(2u);
  visual->mGlyphsToCharacters.PushBack(0u);

  Text::ScriptRun script{};
  script.characterRun = Text::CharacterRun(0u, 2u);
  script.script = TextAbstraction::UNKNOWN;
  logical->mScriptRuns.PushBack(script);
  Text::CharacterRun cluster = Text::RetrieveClusteredCharactersOfCharacterIndex(visual, logical, 1u);
  DALI_TEST_EQUALS(cluster.characterIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(cluster.numberOfCharacters, 2u, TEST_LOCATION);

  logical->mScriptRuns[0].script = TextAbstraction::ARABIC;
  cluster = Text::RetrieveClusteredCharactersOfCharacterIndex(visual, logical, 1u);
  DALI_TEST_EQUALS(cluster.characterIndex, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(cluster.numberOfCharacters, 1u, TEST_LOCATION);

  visual->mCharactersPerGlyph[0] = 0u;
  visual->mCharactersPerGlyph.PushBack(2u);
  visual->mGlyphsToCharacters.PushBack(0u);
  cluster = Text::RetrieveClusteredCharactersOfCharacterIndex(visual, logical, 1u);
  DALI_TEST_EQUALS(cluster.characterIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(cluster.numberOfCharacters, 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextUtf8ConversionBoundariesP(void)
{
  const uint32_t characters[] = {'A', 0x00E9u, 0x4E2Du, 0x1F642u};
  uint8_t encoded[32] = {};
  const uint32_t byteCount = Text::Utf32ToUtf8(characters, 4u, encoded);
  DALI_TEST_EQUALS(byteCount, 10u, TEST_LOCATION);
  DALI_TEST_EQUALS(Text::GetNumberOfUtf8Bytes(characters, 4u), byteCount, TEST_LOCATION);
  DALI_TEST_EQUALS(Text::GetNumberOfUtf8Characters(encoded, byteCount), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(Text::GetUtf8Length(encoded[0]), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(Text::GetUtf8Length(encoded[1]), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(Text::GetUtf8Length(encoded[3]), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(Text::GetUtf8Length(encoded[6]), 4u, TEST_LOCATION);

  uint32_t decoded[8] = {};
  DALI_TEST_EQUALS(Text::Utf8ToUtf32(encoded, byteCount, decoded), 4u, TEST_LOCATION);
  for(uint32_t index = 0u; index < 4u; ++index)
  {
    DALI_TEST_EQUALS(decoded[index], characters[index], TEST_LOCATION);
  }

  const uint8_t lineEndings[] = {'A', '\r', '\n', 'B', '\r', 'C', 0xFEu};
  DALI_TEST_EQUALS(Text::Utf8ToUtf32(lineEndings, 7u, decoded), 6u, TEST_LOCATION);
  DALI_TEST_EQUALS(decoded[1], static_cast<uint32_t>('\n'), TEST_LOCATION);
  DALI_TEST_EQUALS(decoded[3], static_cast<uint32_t>('\n'), TEST_LOCATION);
  DALI_TEST_EQUALS(decoded[5], 0x20u, TEST_LOCATION);

  std::string roundTrip;
  Text::Utf32ToUtf8(characters, 4u, roundTrip);
  DALI_TEST_EQUALS(roundTrip.size(), static_cast<size_t>(byteCount), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextLegacyExtendedUtf8WidthsP(void)
{
  const uint32_t codePoints[] = {0x200000u, 0x4000000u};
  uint8_t encoded[12] = {};
  DALI_TEST_EQUALS(Text::GetNumberOfUtf8Bytes(codePoints, 2u), 11u, TEST_LOCATION);
  DALI_TEST_EQUALS(Text::Utf32ToUtf8(codePoints, 2u, encoded), 11u, TEST_LOCATION);
  DALI_TEST_EQUALS(Text::GetUtf8Length(encoded[0]), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(Text::GetUtf8Length(encoded[5]), 6u, TEST_LOCATION);
  DALI_TEST_EQUALS(Text::GetNumberOfUtf8Characters(encoded, 11u), 2u, TEST_LOCATION);
  uint32_t decoded[2] = {};
  DALI_TEST_EQUALS(Text::Utf8ToUtf32(encoded, 11u, decoded), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(decoded[0], codePoints[0], TEST_LOCATION);
  DALI_TEST_EQUALS(decoded[1], codePoints[1], TEST_LOCATION);
  END_TEST;
}

int UtcDaliMarkupHelperColorAndAlignmentRoundTripsP(void)
{
  const struct { const char* name; Vector4 value; } colors[] = {
    {"black", Color::BLACK}, {"white", Color::WHITE}, {"red", Color::RED},
    {"green", Color::GREEN}, {"blue", Color::BLUE}, {"yellow", Color::YELLOW},
    {"magenta", Color::MAGENTA}, {"cyan", Color::CYAN}, {"transparent", Color::TRANSPARENT}
  };
  for(const auto& color : colors)
  {
    Vector4 parsed;
    Text::ColorStringToVector4(color.name, static_cast<Text::Length>(strlen(color.name)), parsed);
    DALI_TEST_EQUALS(parsed, color.value, TEST_LOCATION);
    std::string serialized;
    Text::Vector4ToColorString(color.value, serialized);
    DALI_TEST_EQUALS(serialized, std::string(color.name), TEST_LOCATION);
  }

  Vector4 webColor;
  Text::ColorStringToVector4("#f00", 4u, webColor);
  DALI_TEST_EQUALS(webColor, Color::RED, TEST_LOCATION);
  Text::ColorStringToVector4("#FF0000", 7u, webColor);
  DALI_TEST_EQUALS(webColor, Color::RED, TEST_LOCATION);
  Text::ColorStringToVector4("0xFF0000FF", 10u, webColor);
  DALI_TEST_EQUALS(webColor, Color::BLUE, TEST_LOCATION);
  std::string serialized;
  Text::Vector4ToColorString(Vector4(0.2f, 0.4f, 0.6f, 0.8f), serialized);
  DALI_TEST_CHECK(serialized.find("0x") == 0u);

  Text::Alignment alignment = Text::Alignment::START;
  DALI_TEST_CHECK(Text::HorizontalAlignmentTypeStringToTypeValue("center", 6u, alignment));
  DALI_TEST_EQUALS(alignment, Text::Alignment::CENTER, TEST_LOCATION);
  DALI_TEST_CHECK(Text::HorizontalAlignmentTypeStringToTypeValue("end", 3u, alignment));
  DALI_TEST_EQUALS(alignment, Text::Alignment::END, TEST_LOCATION);
  DALI_TEST_CHECK(Text::HorizontalAlignmentTypeStringToTypeValue("start", 5u, alignment));
  DALI_TEST_EQUALS(alignment, Text::Alignment::START, TEST_LOCATION);
  DALI_TEST_CHECK(!Text::HorizontalAlignmentTypeStringToTypeValue("unknown", 7u, alignment));
  DALI_TEST_EQUALS(Text::StringToUint("42"), 42u, TEST_LOCATION);
  std::string number;
  Text::UintToString(42u, number);
  DALI_TEST_EQUALS(number, std::string("42"), TEST_LOCATION);
  Text::Underline::Type underline = Text::Underline::Type::DOUBLE;
  Text::UnderlineTypeStringToTypeValue("solid", 5u, underline);
  DALI_TEST_EQUALS(underline, Text::Underline::Type::SOLID, TEST_LOCATION);
  Text::UnderlineTypeStringToTypeValue("dashed", 6u, underline);
  DALI_TEST_EQUALS(underline, Text::Underline::Type::DASHED, TEST_LOCATION);
  END_TEST;
}

int UtcDaliEmojiHelperSequenceFallbackMatrixP(void)
{
  using Script = TextAbstraction::Script;
  auto checkSequence = [](std::initializer_list<Text::Character> characters, bool expected,
                          Text::Length expectedLength, Script expectedScript)
  {
    Text::Length length = 0u;
    Script script = TextAbstraction::UNKNOWN;
    const bool found = Text::GetEmojiSequence(characters.begin(), 0u,
                                               static_cast<Text::Length>(characters.size() - 1u),
                                               TextAbstraction::UNKNOWN, length, script);
    DALI_TEST_EQUALS(found, expected, TEST_LOCATION);
    if(expected)
    {
      DALI_TEST_EQUALS(length, expectedLength, TEST_LOCATION);
      DALI_TEST_EQUALS(script, expectedScript, TEST_LOCATION);
    }
  };

  checkSequence({'1', 0xFE0Fu, 0x20E3u}, true, 3u, TextAbstraction::EMOJI_COLOR);
  checkSequence({'1', 0xFE0Eu, 0x20E3u}, true, 3u, TextAbstraction::EMOJI_TEXT);
  checkSequence({'1', 0x20E3u}, true, 2u, TextAbstraction::EMOJI);
  checkSequence({0x1F1FAu, 0x1F1F8u}, true, 2u, TextAbstraction::EMOJI_COLOR);
  checkSequence({0x1F1FAu}, true, 1u, TextAbstraction::EMOJI);
  checkSequence({0x1F469u, 0x200Du, 0x1F4BBu}, true, 3u, TextAbstraction::EMOJI_COLOR);
  checkSequence({0x1F469u, 0x200Du}, true, 2u, TextAbstraction::EMOJI_COLOR);
  checkSequence({0x1F44Du, 0x1F3FDu}, true, 2u, TextAbstraction::EMOJI_COLOR);
  checkSequence({0x1F3F4u, 0xE0067u, 0xE0062u, 0xE007Fu}, true, 4u, TextAbstraction::EMOJI_COLOR);
  checkSequence({0x2764u, 0xFE0Fu}, true, 2u, TextAbstraction::EMOJI_COLOR);
  checkSequence({0x2764u, 0xFE0Eu}, true, 2u, TextAbstraction::EMOJI_TEXT);
  checkSequence({'A'}, false, 0u, TextAbstraction::UNKNOWN);
  checkSequence({0x1F3FBu}, false, 0u, TextAbstraction::UNKNOWN);

  const Text::Character keycap[] = {'1', 0xFE0Fu, 0x20E3u};
  Script script = TextAbstraction::COMMON;
  DALI_TEST_CHECK(Text::IsNewSequence(keycap, TextAbstraction::COMMON, 0u, 2u, script));
  DALI_TEST_EQUALS(script, TextAbstraction::EMOJI_COLOR, TEST_LOCATION);
  DALI_TEST_CHECK(!Text::IsNewSequence(keycap, TextAbstraction::EMOJI_COLOR, 2u, 2u, script));
  const Text::Character heartText[] = {0x2764u, 0xFE0Eu, 'A'};
  DALI_TEST_CHECK(Text::IsNewSequence(heartText, TextAbstraction::EMOJI_COLOR, 0u, 2u, script));
  DALI_TEST_EQUALS(script, TextAbstraction::EMOJI_TEXT, TEST_LOCATION);
  DALI_TEST_CHECK(!Text::IsNewSequence(heartText, TextAbstraction::EMOJI_TEXT, 2u, 2u, script));
  const Text::Character heartPlain[] = {0x2764u, 'A'};
  DALI_TEST_CHECK(Text::IsNewSequence(heartPlain, TextAbstraction::EMOJI, 0u, 1u, script));

  script = TextAbstraction::COMMON;
  DALI_TEST_CHECK(Text::IsScriptChangedToFollowSequence(TextAbstraction::EMOJI,
                                                          0x20E3u, script));
  DALI_TEST_EQUALS(script, TextAbstraction::EMOJI, TEST_LOCATION);
  script = TextAbstraction::COMMON;
  DALI_TEST_CHECK(Text::IsScriptChangedToFollowSequence(TextAbstraction::EMOJI_COLOR,
                                                          0xFE0Eu, script));
  DALI_TEST_EQUALS(script, TextAbstraction::EMOJI_TEXT, TEST_LOCATION);
  script = TextAbstraction::COMMON;
  DALI_TEST_CHECK(Text::IsScriptChangedToFollowSequence(TextAbstraction::EMOJI,
                                                          0xFE0Fu, script));
  DALI_TEST_EQUALS(script, TextAbstraction::EMOJI_COLOR, TEST_LOCATION);
  DALI_TEST_EQUALS(Text::GetVariationSelectorByScript(TextAbstraction::EMOJI_COLOR),
                   0xFE0Fu, TEST_LOCATION);
  DALI_TEST_EQUALS(Text::GetVariationSelectorByScript(TextAbstraction::EMOJI_TEXT),
                   0xFE0Eu, TEST_LOCATION);
  DALI_TEST_EQUALS(Text::GetVariationSelectorByScript(TextAbstraction::COMMON),
                   0u, TEST_LOCATION);
  END_TEST;
}
