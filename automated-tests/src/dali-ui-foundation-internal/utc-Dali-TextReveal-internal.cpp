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
#include <dali-ui-foundation/internal/text/controller/text-controller.h>
#include <dali-ui-foundation/internal/text/line-run.h>
#include <dali-ui-foundation/internal/text/rendering/text-typesetter-impl.h>
#include <dali-ui-foundation/internal/text/rendering/text-typesetter.h>
#include <dali-ui-foundation/internal/text/rendering/view-model.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-run-snapshot.h>
#include <dali-ui-foundation/internal/text/reveal/text-reveal.h>
#include <dali-ui-foundation/public-api/animation/duration.h>
#include <dali-ui-foundation/public-api/animation/label-animation-bridge.autogen.h>
#include <dali-ui-foundation/public-api/text/style/reveal.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <dali/devel-api/text-abstraction/segmentation.h>
#include <dali/integration-api/pixel-data-integ.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

using namespace Dali;

namespace
{
namespace Reveal = Dali::Ui::Text::Internal::Reveal;
namespace UiText = Dali::Ui::Text;

constexpr float EPSILON = 0.0001f;

float CalculateRevealOpacityReference(float progress, float unitStart, float fadeDuration)
{
  progress = std::isnan(progress) ? 0.0f : std::max(0.0f, std::min(1.0f, progress));
  if(progress <= 0.0f)
  {
    return 0.0f;
  }
  if(progress >= 1.0f)
  {
    return 1.0f;
  }
  if(fadeDuration <= 0.000001f)
  {
    return progress >= unitStart ? 1.0f : 0.0f;
  }
  return std::max(0.0f, std::min(1.0f, (progress - unitStart) / fadeDuration));
}

UiText::ControllerPtr BuildReplacementController(const char*                                 text,
                                                 std::initializer_list<UiText::CharacterRun> ranges,
                                                 const Size&                                 size,
                                                 bool                                        elideText = false)
{
  UiText::ControllerPtr     controller = UiText::Controller::New();
  UiText::Controller::Impl& impl       = UiText::Controller::Impl::GetImplementation(*controller.Get());
  controller->SetText(text);
  controller->SetDefaultFontSize(20.0f, UiText::Controller::PIXEL_SIZE);
  controller->SetTextElideEnabled(elideText);

  UiText::ReplacementSourceSnapshot source;
  uint64_t                          identity = 1u;
  for(const UiText::CharacterRun& range : ranges)
  {
    UiText::ReplacementRunSnapshot replacement;
    replacement.logicalCharacterRange = range;
    replacement.metrics.width         = 32.0f;
    replacement.metrics.height        = 24.0f;
    replacement.type                  = UiText::ReplacementType::IMAGE;
    replacement.occurrenceIdentity    = identity++;
    replacement.image.source          = "unused.png";
    source.runs.PushBack(replacement);
  }
  source.sourceRevision                       = 1u;
  source.hasValidReplacementSource            = !source.runs.Empty();
  impl.GetOrCreateReplacementSourceSnapshot() = source;
  controller->Relayout(size);
  return controller;
}
} // namespace

void utc_dali_text_reveal_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_reveal_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextRevealWordPlanP(void)
{
  const UiText::Character      text[]     = {'H', 'i', ' ', 'a', 'l', 'l'};
  const UiText::CharacterIndex glyphMap[] = {0u, 0u, 1u, 2u, 3u, 4u, 5u};
  const UiText::WordBreakInfo  breaks[]   = {
    TextAbstraction::WORD_NO_BREAK, TextAbstraction::WORD_BREAK, TextAbstraction::WORD_BREAK,
    TextAbstraction::WORD_NO_BREAK, TextAbstraction::WORD_NO_BREAK, TextAbstraction::WORD_BREAK};

  const Reveal::Plan plan = Reveal::BuildPlan(text, 6u, glyphMap, 7u, Reveal::Unit::WORD,
                                              UiText::Reveal::AUTO_FADE_DURATION_RATIO, breaks);
  DALI_TEST_EQUALS(plan.GetUnitCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.glyphToUnit[0], 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.glyphToUnit[1], 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.glyphToUnit[2], 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.glyphToUnit[3], Reveal::NO_UNIT, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.glyphToUnit[4], 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.glyphToUnit[6], 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.unitStart[0], 0.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.unitStart[1], 1.0f / (1.0f + std::sqrt(2.0f)), EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.fadeDuration, std::sqrt(2.0f) / (1.0f + std::sqrt(2.0f)), EPSILON, TEST_LOCATION);

  // Actual DALi segmentation exposes punctuation boundaries. WORD reveal
  // folds adjacent punctuation into the neighboring word so it consumes no
  // standalone timing unit: "Hello, (world)!" -> two units.
  const UiText::Character punctuationText[] = {
    '"', 'H', 'e', 'l', 'l', 'o', ',', ' ', '(', 'w', 'o', 'r', 'l', 'd', ')', '!'};
  const UiText::CharacterIndex punctuationMap[] = {
    0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u};
  const UiText::WordBreakInfo punctuationBreaks[] = {
    TextAbstraction::WORD_BREAK,
    TextAbstraction::WORD_NO_BREAK, TextAbstraction::WORD_NO_BREAK,
    TextAbstraction::WORD_NO_BREAK, TextAbstraction::WORD_NO_BREAK,
    TextAbstraction::WORD_BREAK, TextAbstraction::WORD_BREAK, TextAbstraction::WORD_BREAK,
    TextAbstraction::WORD_BREAK,
    TextAbstraction::WORD_NO_BREAK, TextAbstraction::WORD_NO_BREAK,
    TextAbstraction::WORD_NO_BREAK, TextAbstraction::WORD_NO_BREAK,
    TextAbstraction::WORD_BREAK, TextAbstraction::WORD_BREAK, TextAbstraction::WORD_BREAK};
  const Reveal::Plan punctuationPlan = Reveal::BuildPlan(
    punctuationText, 16u, punctuationMap, 16u, Reveal::Unit::WORD,
    UiText::Reveal::AUTO_FADE_DURATION_RATIO, punctuationBreaks);
  DALI_TEST_EQUALS(punctuationPlan.GetUnitCount(), 2u, TEST_LOCATION);
  for(uint32_t glyph = 0u; glyph <= 6u; ++glyph)
  {
    DALI_TEST_EQUALS(punctuationPlan.glyphToUnit[glyph], 0u, TEST_LOCATION);
  }
  DALI_TEST_EQUALS(punctuationPlan.glyphToUnit[7u], Reveal::NO_UNIT, TEST_LOCATION);
  for(uint32_t glyph = 8u; glyph < 16u; ++glyph)
  {
    DALI_TEST_EQUALS(punctuationPlan.glyphToUnit[glyph], 1u, TEST_LOCATION);
  }

  // Apostrophes and numeric punctuation are already retained inside DALi
  // segmentation units and remain untouched by the post-processing.
  const UiText::Character      apostropheText[]   = {'d', 'o', 'n', '\'', 't', ' ', '1', ',', '0', '0', '0', '.', '5', '0'};
  const UiText::CharacterIndex apostropheMap[]    = {0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u};
  const UiText::WordBreakInfo  apostropheBreaks[] = {
    TextAbstraction::WORD_NO_BREAK, TextAbstraction::WORD_NO_BREAK,
    TextAbstraction::WORD_NO_BREAK, TextAbstraction::WORD_NO_BREAK,
    TextAbstraction::WORD_BREAK, TextAbstraction::WORD_BREAK,
    TextAbstraction::WORD_NO_BREAK, TextAbstraction::WORD_NO_BREAK,
    TextAbstraction::WORD_NO_BREAK, TextAbstraction::WORD_NO_BREAK,
    TextAbstraction::WORD_NO_BREAK, TextAbstraction::WORD_NO_BREAK,
    TextAbstraction::WORD_NO_BREAK, TextAbstraction::WORD_BREAK};
  const Reveal::Plan apostrophePlan = Reveal::BuildPlan(
    apostropheText, 14u, apostropheMap, 14u, Reveal::Unit::WORD,
    UiText::Reveal::AUTO_FADE_DURATION_RATIO, apostropheBreaks);
  DALI_TEST_EQUALS(apostrophePlan.GetUnitCount(), 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealWordSymbolPunctuationCorpusP(void)
{
  auto CheckUnits = [&](const char*                                  source,
                        std::initializer_list<UiText::WordBreakInfo> breakFixture,
                        std::initializer_list<uint32_t>              expected)
  {
    const size_t length = std::strlen(source);
    DALI_TEST_EQUALS(length, breakFixture.size(), TEST_LOCATION);
    DALI_TEST_EQUALS(length, expected.size(), TEST_LOCATION);

    std::vector<UiText::Character>      text(length);
    std::vector<UiText::CharacterIndex> glyphMap(length);
    std::vector<UiText::WordBreakInfo>  breaks(breakFixture);
    for(size_t index = 0u; index < length; ++index)
    {
      text[index]     = static_cast<unsigned char>(source[index]);
      glyphMap[index] = static_cast<UiText::CharacterIndex>(index);
    }
    const Reveal::Plan plan  = Reveal::BuildPlan(text.data(), static_cast<UiText::Length>(length),
                                                 glyphMap.data(), static_cast<UiText::Length>(length),
                                                 Reveal::Unit::WORD, UiText::Reveal::AUTO_FADE_DURATION_RATIO, breaks.data());
    size_t             index = 0u;
    for(uint32_t unit : expected)
    {
      DALI_TEST_EQUALS(plan.glyphToUnit[index++], unit, TEST_LOCATION);
    }
  };

  constexpr auto B = TextAbstraction::WORD_BREAK;
  constexpr auto N = TextAbstraction::WORD_NO_BREAK;
  // These fixtures are the output of the production DALi segmentation engine
  // for this corpus. (The UTC text-abstraction stub intentionally does not run
  // the production segmenter.) Punctuation folds into an adjacent word, while
  // currency, math and general symbols retain the segmenter's boundaries.
  CheckUnits("$100", {B, N, N, B}, {0u, 1u, 1u, 1u});
  CheckUnits("A+B", {B, B, B}, {0u, 1u, 2u});
  CheckUnits("x=10", {B, B, N, B}, {0u, 1u, 2u, 2u});
  CheckUnits("foo/bar", {N, N, B, B, N, N, B}, {0u, 0u, 0u, 0u, 1u, 1u, 1u});
  CheckUnits("a|b", {B, B, B}, {0u, 1u, 2u});
  CheckUnits("~test", {B, N, N, N, B}, {0u, 1u, 1u, 1u, 1u});
  CheckUnits("100%", {N, N, B, B}, {0u, 0u, 0u, 0u});
  CheckUnits("email@example.com", {N, N, N, N, B, B, N, N, N, N, N, N, N, N, N, N, B},
             {0u, 0u, 0u, 0u, 0u, 0u,
              1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u});
  CheckUnits("C++", {B, B, B}, {0u, 1u, 2u});
  END_TEST;
}

int UtcDaliTextRevealFadeDurationRatioScheduleP(void)
{
  const uint32_t autoUnitCounts[] = {1u, 2u, 5u, 10u, 20u, 50u, 100u, 200u};
  for(uint32_t unitCount : autoUnitCounts)
  {
    std::vector<UiText::Character>      text(unitCount, static_cast<UiText::Character>('A'));
    std::vector<UiText::CharacterIndex> glyphMap(unitCount);
    for(uint32_t index = 0u; index < unitCount; ++index)
    {
      glyphMap[index] = index;
    }

    const Reveal::Plan automatic   = Reveal::BuildPlan(text.data(), unitCount, glyphMap.data(), unitCount,
                                                       Reveal::Unit::CHARACTER,
                                                       UiText::Reveal::AUTO_FADE_DURATION_RATIO);
    const float        fadeSpan    = std::min(10.0f, std::max(1.0f, std::sqrt(static_cast<float>(unitCount))));
    const float        denominator = static_cast<float>(unitCount - 1u) + fadeSpan;
    DALI_TEST_EQUALS(automatic.fadeDurationRatio, UiText::Reveal::AUTO_FADE_DURATION_RATIO, EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(automatic.fadeDuration, fadeSpan / denominator, EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(automatic.unitStart.back() + automatic.fadeDuration, 1.0f, EPSILON, TEST_LOCATION);
    if(unitCount > 1u)
    {
      DALI_TEST_EQUALS(automatic.unitStart[1] - automatic.unitStart[0], 1.0f / denominator, EPSILON, TEST_LOCATION);
      DALI_TEST_CHECK(automatic.fadeDuration > automatic.unitStart[1]);
    }
  }

  const uint32_t explicitUnitCounts[] = {1u, 2u, 5u, 20u, 100u};
  const float    ratios[]             = {0.0f, 0.05f, 0.1f, 0.2f, 0.5f, 1.0f};
  for(uint32_t unitCount : explicitUnitCounts)
  {
    std::vector<UiText::Character>      text(unitCount, static_cast<UiText::Character>('A'));
    std::vector<UiText::CharacterIndex> glyphMap(unitCount);
    for(uint32_t index = 0u; index < unitCount; ++index)
    {
      glyphMap[index] = index;
    }

    for(float ratio : ratios)
    {
      const Reveal::Plan plan = Reveal::BuildPlan(text.data(), unitCount, glyphMap.data(), unitCount,
                                                  Reveal::Unit::CHARACTER, ratio);
      DALI_TEST_EQUALS(plan.fadeDurationRatio, ratio, EPSILON, TEST_LOCATION);
      DALI_TEST_EQUALS(plan.fadeDuration, ratio, EPSILON, TEST_LOCATION);
      DALI_TEST_EQUALS(plan.unitStart.front(), 0.0f, EPSILON, TEST_LOCATION);
      if(unitCount == 1u)
      {
        DALI_TEST_EQUALS(plan.unitStart.front(), 0.0f, EPSILON, TEST_LOCATION);
      }
      else
      {
        const float expectedInterval = (1.0f - ratio) / static_cast<float>(unitCount - 1u);
        DALI_TEST_EQUALS(plan.unitStart.back() + plan.fadeDuration, 1.0f, EPSILON, TEST_LOCATION);
        for(uint32_t unit = 1u; unit < unitCount; ++unit)
        {
          DALI_TEST_EQUALS(plan.unitStart[unit] - plan.unitStart[unit - 1u], expectedInterval, EPSILON, TEST_LOCATION);
        }
      }

      if(ratio == 0.0f)
      {
        DALI_TEST_EQUALS(plan.fadeDuration, 0.0f, EPSILON, TEST_LOCATION);
        DALI_TEST_EQUALS(CalculateRevealOpacityReference(0.0f, plan.unitStart.front(), plan.fadeDuration),
                         0.0f, EPSILON, TEST_LOCATION);
        DALI_TEST_EQUALS(CalculateRevealOpacityReference(0.0001f, plan.unitStart.front(), plan.fadeDuration),
                         1.0f, EPSILON, TEST_LOCATION);
      }
      if(ratio == 1.0f)
      {
        for(float start : plan.unitStart)
        {
          DALI_TEST_EQUALS(start, 0.0f, EPSILON, TEST_LOCATION);
        }
      }
    }
  }

  // Unit selection changes ownership only; the same final unit count and
  // authored ratio produce the same schedule for CHARACTER and WORD.
  const UiText::Character      wordText[]   = {'A', ' ', 'B', ' ', 'C', ' ', 'D', ' ', 'E'};
  const UiText::CharacterIndex wordMap[]    = {0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u};
  const UiText::WordBreakInfo  wordBreaks[] = {
    TextAbstraction::WORD_BREAK, TextAbstraction::WORD_NO_BREAK,
    TextAbstraction::WORD_BREAK, TextAbstraction::WORD_NO_BREAK,
    TextAbstraction::WORD_BREAK, TextAbstraction::WORD_NO_BREAK,
    TextAbstraction::WORD_BREAK, TextAbstraction::WORD_NO_BREAK,
    TextAbstraction::WORD_BREAK};
  const Reveal::Plan words = Reveal::BuildPlan(wordText, 9u, wordMap, 9u, Reveal::Unit::WORD, 0.2f, wordBreaks);
  DALI_TEST_EQUALS(words.GetUnitCount(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(words.fadeDuration, 0.2f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(words.unitStart.back() + words.fadeDuration, 1.0f, EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealPerLineSequenceScheduleP(void)
{
  auto MakePlan = [](const std::vector<uint32_t>& lineUnits, float fadeDurationRatio)
  {
    uint32_t totalUnits = 0u;
    for(uint32_t count : lineUnits)
    {
      totalUnits += count;
    }
    std::vector<UiText::Character>      text(totalUnits, static_cast<UiText::Character>('A'));
    std::vector<UiText::CharacterIndex> glyphMap(totalUnits);
    for(uint32_t unit = 0u; unit < totalUnits; ++unit)
    {
      glyphMap[unit] = unit;
    }
    return Reveal::BuildPlan(text.data(), totalUnits, glyphMap.data(), totalUnits,
                             Reveal::Unit::CHARACTER, fadeDurationRatio);
  };
  auto MakeLines = [](const std::vector<uint32_t>& lineUnits)
  {
    std::vector<UiText::LineRun> lines(lineUnits.size());
    uint32_t                     glyph = 0u;
    for(uint32_t line = 0u; line < lineUnits.size(); ++line)
    {
      lines[line].glyphRun = {glyph, lineUnits[line]};
      glyph += lineUnits[line];
    }
    return lines;
  };

  const auto standaloneFive = MakePlan({5u}, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  auto       equalLines     = MakePlan({5u, 5u, 5u}, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  const auto equalLineRuns  = MakeLines({5u, 5u, 5u});
  DALI_TEST_CHECK(Reveal::ApplyPerLineSequenceSchedule(equalLines, equalLineRuns.data(), equalLineRuns.size(), 0.0f));
  DALI_TEST_EQUALS(equalLines.fadeDuration, standaloneFive.fadeDuration, EPSILON, TEST_LOCATION);
  for(uint32_t line = 0u; line < 3u; ++line)
  {
    for(uint32_t unit = 0u; unit < 5u; ++unit)
    {
      DALI_TEST_EQUALS(equalLines.unitStart[line * 5u + unit], standaloneFive.unitStart[unit], EPSILON, TEST_LOCATION);
    }
  }

  auto       uneven      = MakePlan({3u, 8u, 5u}, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  const auto unevenLines = MakeLines({3u, 8u, 5u});
  const auto standaloneEight = MakePlan({8u}, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  DALI_TEST_CHECK(Reveal::ApplyPerLineSequenceSchedule(uneven, unevenLines.data(), unevenLines.size(), 0.0f));
  DALI_TEST_EQUALS(uneven.fadeDuration, standaloneEight.fadeDuration, EPSILON, TEST_LOCATION);
  for(uint32_t unit = 0u; unit < 8u; ++unit)
  {
    DALI_TEST_EQUALS(uneven.unitStart[3u + unit], standaloneEight.unitStart[unit], EPSILON, TEST_LOCATION);
  }

  auto       withShortLine      = MakePlan({3u, 8u, 5u, 2u}, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  const auto withShortLineRuns  = MakeLines({3u, 8u, 5u, 2u});
  DALI_TEST_CHECK(Reveal::ApplyPerLineSequenceSchedule(withShortLine,
                                                    withShortLineRuns.data(),
                                                    withShortLineRuns.size(),
                                                    0.0f));
  DALI_TEST_EQUALS(withShortLine.fadeDuration, uneven.fadeDuration, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(withShortLine.unitStart[4u] - withShortLine.unitStart[3u],
                   uneven.unitStart[4u] - uneven.unitStart[3u], EPSILON, TEST_LOCATION);

  auto ratioOne = MakePlan({3u, 8u, 5u}, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  DALI_TEST_CHECK(Reveal::ApplyPerLineSequenceSchedule(ratioOne, unevenLines.data(), unevenLines.size(), 1.0f));
  const uint32_t lineStarts[] = {0u, 3u, 11u};
  const uint32_t lineEnds[]   = {2u, 10u, 15u};
  DALI_TEST_EQUALS(ratioOne.unitStart[lineStarts[1u]] - ratioOne.unitStart[lineStarts[0u]],
                   ratioOne.unitStart[lineStarts[2u]] - ratioOne.unitStart[lineStarts[1u]], EPSILON, TEST_LOCATION);
  for(uint32_t line = 0u; line + 1u < 3u; ++line)
  {
    DALI_TEST_CHECK(ratioOne.unitStart[lineEnds[line]] + ratioOne.fadeDuration <=
                    ratioOne.unitStart[lineStarts[line + 1u]] + EPSILON);
  }

  for(float authoredFade : {UiText::Reveal::AUTO_FADE_DURATION_RATIO, 0.25f, 0.0f})
  {
    for(float ratio : {0.0f, 0.5f, 1.0f})
    {
      auto       singleton      = MakePlan({1u, 1u, 1u}, authoredFade);
      const auto singletonLines = MakeLines({1u, 1u, 1u});
      DALI_TEST_CHECK(Reveal::ApplyPerLineSequenceSchedule(singleton,
                                                        singletonLines.data(),
                                                        singletonLines.size(),
                                                        ratio));
      const float totalDuration = 2.0f * ratio + 1.0f;
      const float resolvedFade  = authoredFade == UiText::Reveal::AUTO_FADE_DURATION_RATIO ? 1.0f : authoredFade;
      DALI_TEST_EQUALS(singleton.unitStart[0u], 0.0f, EPSILON, TEST_LOCATION);
      DALI_TEST_EQUALS(singleton.unitStart[1u], ratio / totalDuration, EPSILON, TEST_LOCATION);
      DALI_TEST_EQUALS(singleton.unitStart[2u], 2.0f * ratio / totalDuration, EPSILON, TEST_LOCATION);
      DALI_TEST_EQUALS(singleton.fadeDuration, resolvedFade / totalDuration, EPSILON, TEST_LOCATION);
    }
  }

  auto       singleLine      = MakePlan({1u}, 0.25f);
  const auto originalSingle  = singleLine;
  const auto singleLineRuns  = MakeLines({1u});
  DALI_TEST_CHECK(Reveal::ApplyPerLineSequenceSchedule(singleLine, singleLineRuns.data(), singleLineRuns.size(), 1.0f));
  DALI_TEST_CHECK(singleLine.glyphToUnit == originalSingle.glyphToUnit);
  DALI_TEST_CHECK(singleLine.unitStart == originalSingle.unitStart);
  DALI_TEST_EQUALS(singleLine.fadeDuration, originalSingle.fadeDuration, EPSILON, TEST_LOCATION);

  Reveal::Plan crossLineWord;
  crossLineWord.glyphToUnit      = {0u, 0u, 1u, 2u};
  crossLineWord.unitStart        = {0.0f, 0.5f, 1.0f};
  crossLineWord.fadeDurationRatio = UiText::Reveal::AUTO_FADE_DURATION_RATIO;
  auto crossLineRuns             = MakeLines({1u, 3u});
  DALI_TEST_CHECK(Reveal::ApplyPerLineSequenceSchedule(crossLineWord,
                                                    crossLineRuns.data(),
                                                    crossLineRuns.size(),
                                                    0.5f));
  DALI_TEST_EQUALS(crossLineWord.GetUnitCount(), 4u, TEST_LOCATION);
  DALI_TEST_CHECK(crossLineWord.glyphToUnit[0u] != crossLineWord.glyphToUnit[1u]);

  Reveal::Plan blankLine;
  blankLine.glyphToUnit       = {0u, Reveal::NO_UNIT, 1u};
  blankLine.unitStart         = {0.0f, 1.0f};
  blankLine.fadeDurationRatio = 0.0f;
  auto blankRuns              = MakeLines({1u, 1u, 1u});
  DALI_TEST_CHECK(Reveal::ApplyPerLineSequenceSchedule(blankLine, blankRuns.data(), blankRuns.size(), 0.5f));
  DALI_TEST_EQUALS(blankLine.unitStart[1u], 1.0f / 3.0f, EPSILON, TEST_LOCATION);

  Reveal::Plan bidi;
  bidi.glyphToUnit       = {2u, 1u, 0u, 3u};
  bidi.unitStart         = {0.0f, 0.25f, 0.5f, 0.75f};
  bidi.fadeDurationRatio = 0.25f;
  auto bidiLines         = MakeLines({3u, 1u});
  DALI_TEST_CHECK(Reveal::ApplyPerLineSequenceSchedule(bidi, bidiLines.data(), bidiLines.size(), 0.5f));
  DALI_TEST_EQUALS(bidi.glyphToUnit[0u], 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(bidi.glyphToUnit[1u], 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(bidi.glyphToUnit[2u], 0u, TEST_LOCATION);

  Reveal::Plan splitLine;
  splitLine.glyphToUnit       = {1u, Reveal::NO_UNIT, 0u, 2u};
  splitLine.unitStart         = {0.0f, 0.5f, 1.0f};
  splitLine.fadeDurationRatio = 0.25f;
  std::vector<UiText::LineRun> splitRuns(2u);
  splitRuns[0u].glyphRun             = {0u, 1u};
  splitRuns[0u].isSplitToTwoHalves   = true;
  splitRuns[0u].glyphRunSecondHalf   = {2u, 1u};
  splitRuns[1u].glyphRun             = {3u, 1u};
  DALI_TEST_CHECK(Reveal::ApplyPerLineSequenceSchedule(splitLine, splitRuns.data(), splitRuns.size(), 0.5f));
  DALI_TEST_EQUALS(splitLine.glyphToUnit[0u], 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(splitLine.glyphToUnit[2u], 0u, TEST_LOCATION);

  auto       incompleteMapping = MakePlan({2u}, 0.25f);
  const auto originalMapping   = incompleteMapping;
  auto       incompleteRuns    = MakeLines({1u});
  DALI_TEST_CHECK(!Reveal::ApplyPerLineSequenceSchedule(incompleteMapping,
                                                     incompleteRuns.data(),
                                                     incompleteRuns.size(),
                                                     0.5f));
  DALI_TEST_CHECK(incompleteMapping.glyphToUnit == originalMapping.glyphToUnit);
  DALI_TEST_CHECK(incompleteMapping.unitStart == originalMapping.unitStart);
  DALI_TEST_EQUALS(incompleteMapping.fadeDuration, originalMapping.fadeDuration, EPSILON, TEST_LOCATION);

  const std::vector<uint32_t> stressCounts(100u, 50u);
  auto                        stressPlan = MakePlan(stressCounts, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  const auto                  stressLines = MakeLines(stressCounts);
  DALI_TEST_CHECK(Reveal::ApplyPerLineSequenceSchedule(stressPlan,
                                                    stressLines.data(),
                                                    stressLines.size(),
                                                    0.25f));
  DALI_TEST_EQUALS(stressPlan.GetUnitCount(), 5000u, TEST_LOCATION);
  for(uint32_t line = 0u; line < stressCounts.size(); ++line)
  {
    const uint32_t begin = line * stressCounts[line];
    for(uint32_t unit = begin; unit < begin + stressCounts[line]; ++unit)
    {
      DALI_TEST_CHECK(std::isfinite(stressPlan.unitStart[unit]));
      DALI_TEST_CHECK(stressPlan.unitStart[unit] >= 0.0f && stressPlan.unitStart[unit] <= 1.0f);
      if(unit > begin)
      {
        DALI_TEST_CHECK(stressPlan.unitStart[unit - 1u] <= stressPlan.unitStart[unit]);
      }
    }
  }

  END_TEST;
}

int UtcDaliTextRevealPerLineSequenceFinalLayoutP(void)
{
  UiTestApplication application;

  auto BuildFinalPlans = [](float width)
  {
    UiText::ControllerPtr controller = UiText::Controller::New();
    controller->SetText("One two three four five six seven eight nine ten eleven twelve");
    controller->SetDefaultFontSize(20.0f, UiText::Controller::PIXEL_SIZE);
    controller->SetMultiLineEnabled(true);
    controller->Relayout(Size(width, 400.0f));

    const UiText::ModelInterface* model = controller->GetRenderTextModel();
    DALI_TEST_CHECK(model);
    UiText::TypesetterPtr typesetter = UiText::Typesetter::New(model);
    const Reveal::Plan source = Reveal::BuildCharacterPlan(*model, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
    const Reveal::Plan text   = typesetter->CreateFinalRevealPlan(source, Reveal::Unit::CHARACTER);
    const Reveal::Plan line   = typesetter->CreateFinalRevealPlan(source,
                                                                  Reveal::Unit::CHARACTER,
                                                                  Reveal::Sequence::PER_LINE,
                                                                  0.5f);
    return std::make_tuple(controller, text, line);
  };

  auto [narrowController, narrowText, narrowLine] = BuildFinalPlans(120.0f);
  const UiText::ModelInterface* narrowModel       = narrowController->GetRenderTextModel();
  DALI_TEST_CHECK(narrowModel->GetNumberOfLines() >= 3u);
  DALI_TEST_CHECK(narrowLine.unitStart != narrowText.unitStart);

  std::vector<float> activeLineStarts;
  const UiText::LineRun* lines = narrowModel->GetLines();
  auto CollectRunStart = [&](const UiText::GlyphRun& run, float& start, bool& active)
  {
    for(uint32_t glyph = run.glyphIndex; glyph < run.glyphIndex + run.numberOfGlyphs; ++glyph)
    {
      const uint32_t unit = narrowLine.glyphToUnit[glyph];
      if(unit != Reveal::NO_UNIT)
      {
        start  = active ? std::min(start, narrowLine.unitStart[unit]) : narrowLine.unitStart[unit];
        active = true;
      }
    }
  };
  for(uint32_t lineIndex = 0u; lineIndex < narrowModel->GetNumberOfLines(); ++lineIndex)
  {
    float start  = 0.0f;
    bool  active = false;
    CollectRunStart(lines[lineIndex].glyphRun, start, active);
    if(lines[lineIndex].isSplitToTwoHalves)
    {
      CollectRunStart(lines[lineIndex].glyphRunSecondHalf, start, active);
    }
    if(active)
    {
      activeLineStarts.push_back(start);
    }
  }
  DALI_TEST_CHECK(activeLineStarts.size() >= 3u);
  const float lineStartGap = activeLineStarts[1u] - activeLineStarts[0u];
  for(uint32_t line = 2u; line < activeLineStarts.size(); ++line)
  {
    DALI_TEST_EQUALS(activeLineStarts[line] - activeLineStarts[line - 1u], lineStartGap, EPSILON, TEST_LOCATION);
  }

  auto [wideController, wideText, wideLine] = BuildFinalPlans(2000.0f);
  DALI_TEST_EQUALS(wideController->GetRenderTextModel()->GetNumberOfLines(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(wideLine.glyphToUnit == wideText.glyphToUnit);
  DALI_TEST_CHECK(wideLine.unitStart == wideText.unitStart);
  DALI_TEST_EQUALS(wideLine.fadeDuration, wideText.fadeDuration, EPSILON, TEST_LOCATION);

  UiText::ControllerPtr wordController = UiText::Controller::New();
  wordController->SetText("Supercalifragilisticexpialidocious");
  wordController->SetDefaultFontSize(20.0f, UiText::Controller::PIXEL_SIZE);
  wordController->SetMultiLineEnabled(true);
  wordController->SetLineWrapMode(UiText::LineWrapMode::CHARACTER);
  wordController->Relayout(Size(72.0f, 400.0f));
  const UiText::ModelInterface* wordModel = wordController->GetRenderTextModel();
  DALI_TEST_CHECK(wordModel && wordModel->GetNumberOfLines() >= 2u);
  TextAbstraction::Segmentation segmentation = TextAbstraction::Segmentation::New();
  const Reveal::Plan wordSource = Reveal::BuildPlan(*wordModel,
                                                     Reveal::Unit::WORD,
                                                     UiText::Reveal::AUTO_FADE_DURATION_RATIO,
                                                     segmentation);
  DALI_TEST_EQUALS(wordSource.GetUnitCount(), 1u, TEST_LOCATION);
  UiText::TypesetterPtr wordTypesetter = UiText::Typesetter::New(wordModel);
  const Reveal::Plan wordLine = wordTypesetter->CreateFinalRevealPlan(wordSource,
                                                                      Reveal::Unit::WORD,
                                                                      Reveal::Sequence::PER_LINE,
                                                                      0.5f);
  DALI_TEST_CHECK(wordLine.GetUnitCount() >= 2u);
  std::vector<uint32_t> firstUnitByLine;
  const UiText::LineRun* wordLines = wordModel->GetLines();
  for(uint32_t lineIndex = 0u; lineIndex < wordModel->GetNumberOfLines(); ++lineIndex)
  {
    const UiText::GlyphRun& run = wordLines[lineIndex].glyphRun;
    for(uint32_t glyph = run.glyphIndex; glyph < run.glyphIndex + run.numberOfGlyphs; ++glyph)
    {
      if(wordLine.glyphToUnit[glyph] != Reveal::NO_UNIT)
      {
        firstUnitByLine.push_back(wordLine.glyphToUnit[glyph]);
        break;
      }
    }
  }
  DALI_TEST_CHECK(firstUnitByLine.size() >= 2u);
  for(uint32_t line = 1u; line < firstUnitByLine.size(); ++line)
  {
    DALI_TEST_CHECK(firstUnitByLine[line - 1u] != firstUnitByLine[line]);
  }

  for(const char* bidiText : {
        "אבג דהו זחט יכל מנס עפצ קרשת אבג דהו זחט יכל",
        "ABC אבג DEF דהו XYZ זחט ABC יכל DEF מנס XYZ"})
  {
    UiText::ControllerPtr bidiController = UiText::Controller::New();
    bidiController->SetText(bidiText);
    bidiController->SetDefaultFontSize(20.0f, UiText::Controller::PIXEL_SIZE);
    bidiController->SetMultiLineEnabled(true);
    const Vector3 bidiNaturalSize = bidiController->GetNaturalSize(false);
    DALI_TEST_CHECK(bidiNaturalSize.x > 0.0f && bidiNaturalSize.y > 0.0f);
    bidiController->Relayout(Size(bidiNaturalSize.x * 0.4f, bidiNaturalSize.y * 4.0f));
    const UiText::ModelInterface* bidiModel = bidiController->GetRenderTextModel();
    DALI_TEST_CHECK(bidiModel && bidiModel->GetNumberOfLines() >= 2u);
    UiText::TypesetterPtr bidiTypesetter = UiText::Typesetter::New(bidiModel);
    const Reveal::Plan bidiSource = Reveal::BuildCharacterPlan(*bidiModel,
                                                               UiText::Reveal::AUTO_FADE_DURATION_RATIO);
    const Reveal::Plan bidiTextPlan = bidiTypesetter->CreateFinalRevealPlan(bidiSource,
                                                                            Reveal::Unit::CHARACTER);
    const Reveal::Plan bidiLinePlan = bidiTypesetter->CreateFinalRevealPlan(bidiSource,
                                                                            Reveal::Unit::CHARACTER,
                                                                            Reveal::Sequence::PER_LINE,
                                                                            0.5f);
    const UiText::LineRun* bidiLines = bidiModel->GetLines();
    float                  previousLineStart = -1.0f;
    for(uint32_t lineIndex = 0u; lineIndex < bidiModel->GetNumberOfLines(); ++lineIndex)
    {
      const UiText::GlyphRun& run = bidiLines[lineIndex].glyphRun;
      std::vector<std::pair<uint32_t, float>> logicalStarts;
      for(uint32_t glyph = run.glyphIndex; glyph < run.glyphIndex + run.numberOfGlyphs; ++glyph)
      {
        if(bidiTextPlan.glyphToUnit[glyph] != Reveal::NO_UNIT)
        {
          logicalStarts.emplace_back(bidiTextPlan.glyphToUnit[glyph],
                                     bidiLinePlan.unitStart[bidiLinePlan.glyphToUnit[glyph]]);
        }
      }
      std::sort(logicalStarts.begin(), logicalStarts.end());
      DALI_TEST_CHECK(!logicalStarts.empty());
      for(uint32_t unit = 1u; unit < logicalStarts.size(); ++unit)
      {
        DALI_TEST_CHECK(logicalStarts[unit - 1u].second <= logicalStarts[unit].second);
      }
      const float lineStart = logicalStarts.front().second;
      DALI_TEST_CHECK(lineStart > previousLineStart);
      previousLineStart = lineStart;
    }
  }

  END_TEST;
}

int UtcDaliTextRevealPerLineSequenceMaximumLinesEndEllipsisP(void)
{
  UiTestApplication application;

  struct Result
  {
    Reveal::Plan         plan;
    uint32_t             sourceUnitCount{0u};
    std::vector<uint32_t> lineUnitCounts;
    std::vector<float>    lineStarts;
    uint32_t              ellipsisLine{Reveal::NO_UNIT};
  };

  auto BuildResult = [](Reveal::Unit unit, const char* hiddenText)
  {
    UiText::ControllerPtr controller = UiText::Controller::New();
    const std::string     text       = std::string("Alpha beta\nGamma delta\n") + hiddenText;
    controller->SetText(text);
    controller->SetDefaultFontSize(20.0f, UiText::Controller::PIXEL_SIZE);
    controller->SetMultiLineEnabled(true);
    controller->SetTextElideEnabled(true);
    controller->SetEllipsisPosition(UiText::EllipsisPosition::END);
    controller->SetMaximumNumberOfLines(2);
    controller->Relayout(Size(500.0f, 500.0f));

    const UiText::ModelInterface* model = controller->GetRenderTextModel();
    DALI_TEST_CHECK(model);
    DALI_TEST_EQUALS(model->GetNumberOfLines(), 2u, TEST_LOCATION);
    const UiText::FinalElisionResult* finalElision = controller->GetFinalElisionResult();
    DALI_TEST_CHECK(finalElision && finalElision->resolved && finalElision->applied && finalElision->textElided);
    DALI_TEST_EQUALS(finalElision->ellipsisUnitCount, 1u, TEST_LOCATION);

    Reveal::Plan source;
    if(unit == Reveal::Unit::WORD)
    {
      TextAbstraction::Segmentation segmentation = TextAbstraction::Segmentation::New();
      source = Reveal::BuildPlan(*model,
                                 unit,
                                 UiText::Reveal::AUTO_FADE_DURATION_RATIO,
                                 segmentation);
    }
    else
    {
      source = Reveal::BuildCharacterPlan(*model, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
    }

    UiText::TypesetterPtr typesetter = UiText::Typesetter::New(model);
    typesetter->SetFinalElisionResult(finalElision);
    Result result;
    result.sourceUnitCount = source.GetUnitCount();
    result.plan            = typesetter->CreateFinalRevealPlan(source,
                                                               unit,
                                                               Reveal::Sequence::PER_LINE,
                                                               0.25f);

    UiText::ViewModel* viewModel = typesetter->GetViewModel();
    DALI_TEST_CHECK(viewModel);
    DALI_TEST_EQUALS(viewModel->GetNumberOfLines(), 2u, TEST_LOCATION);
    const UiText::GlyphIndex ellipsisGlyph = viewModel->GetEllipsisFinalGlyphIndex();
    DALI_TEST_CHECK(ellipsisGlyph < result.plan.glyphToUnit.size());
    DALI_TEST_CHECK(result.plan.glyphToUnit[ellipsisGlyph] != Reveal::NO_UNIT);

    const UiText::LineRun* lines = viewModel->GetLines();
    DALI_TEST_CHECK(lines);
    for(uint32_t lineIndex = 0u; lineIndex < viewModel->GetNumberOfLines(); ++lineIndex)
    {
      std::vector<bool> seen(result.plan.GetUnitCount(), false);
      float             lineStart = 0.0f;
      uint32_t          count     = 0u;
      auto CollectRun = [&](const UiText::GlyphRun& run)
      {
        for(UiText::GlyphIndex glyph = run.glyphIndex; glyph < run.glyphIndex + run.numberOfGlyphs; ++glyph)
        {
          DALI_TEST_CHECK(glyph < result.plan.glyphToUnit.size());
          if(glyph == ellipsisGlyph)
          {
            result.ellipsisLine = lineIndex;
          }
          const uint32_t revealUnit = result.plan.glyphToUnit[glyph];
          if(revealUnit != Reveal::NO_UNIT && !seen[revealUnit])
          {
            seen[revealUnit] = true;
            lineStart        = count == 0u ? result.plan.unitStart[revealUnit]
                                          : std::min(lineStart, result.plan.unitStart[revealUnit]);
            ++count;
          }
        }
      };
      CollectRun(lines[lineIndex].glyphRun);
      if(lines[lineIndex].isSplitToTwoHalves)
      {
        CollectRun(lines[lineIndex].glyphRunSecondHalf);
      }
      DALI_TEST_CHECK(count > 0u);
      result.lineUnitCounts.push_back(count);
      result.lineStarts.push_back(lineStart);
    }

    DALI_TEST_EQUALS(result.ellipsisLine, 1u, TEST_LOCATION);
    DALI_TEST_CHECK(result.lineStarts[1u] > result.lineStarts[0u]);
    DALI_TEST_CHECK(result.plan.GetUnitCount() < result.sourceUnitCount);
    return result;
  };

  for(Reveal::Unit unit : {Reveal::Unit::CHARACTER, Reveal::Unit::WORD})
  {
    const Result shortHidden = BuildResult(unit, "Hidden tail");
    const Result longHidden  = BuildResult(unit,
                                          "Hidden tail with many extra words\n"
                                          "Another hidden line\n"
                                          "And one more hidden line");
    DALI_TEST_CHECK(longHidden.sourceUnitCount > shortHidden.sourceUnitCount);
    DALI_TEST_CHECK(longHidden.plan.glyphToUnit == shortHidden.plan.glyphToUnit);
    DALI_TEST_CHECK(longHidden.plan.unitStart == shortHidden.plan.unitStart);
    DALI_TEST_EQUALS(longHidden.plan.fadeDuration,
                     shortHidden.plan.fadeDuration,
                     EPSILON,
                     TEST_LOCATION);
    DALI_TEST_CHECK(longHidden.lineUnitCounts == shortHidden.lineUnitCounts);
    DALI_TEST_CHECK(longHidden.lineStarts == shortHidden.lineStarts);
  }

  END_TEST;
}

int UtcDaliTextRevealCharacterPlanP(void)
{
  // Synthetic shaping map: ffi and e+combining acute share cluster starts.
  const UiText::Character      text[]     = {'o', 'f', 'f', 'i', 'c', 'e', ' ', 'e', 0x0301u};
  const UiText::CharacterIndex glyphMap[] = {0u, 1u, 4u, 5u, 6u, 7u, 7u};
  const Reveal::Plan           plan       = Reveal::BuildPlan(text, 9u, glyphMap, 7u,
                                                              Reveal::Unit::CHARACTER, UiText::Reveal::AUTO_FADE_DURATION_RATIO);

  DALI_TEST_EQUALS(plan.GetUnitCount(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.glyphToUnit[0], 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.glyphToUnit[1], 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.glyphToUnit[2], 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.glyphToUnit[3], 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.glyphToUnit[4], Reveal::NO_UNIT, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.glyphToUnit[5], 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.glyphToUnit[6], 4u, TEST_LOCATION);

  // Visual glyph order may be reversed by bidi layout; unit timing remains in
  // logical character order while each visual glyph keeps the correct unit.
  const UiText::Character      bidiText[]       = {'A', 'B', ' ', 0x05D0u, 0x05D1u};
  const UiText::CharacterIndex visualGlyphMap[] = {4u, 3u, 2u, 1u, 0u};
  const Reveal::Plan           bidiPlan         = Reveal::BuildPlan(bidiText, 5u, visualGlyphMap, 5u,
                                                                    Reveal::Unit::CHARACTER, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  DALI_TEST_EQUALS(bidiPlan.GetUnitCount(), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(bidiPlan.glyphToUnit[0], 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(bidiPlan.glyphToUnit[1], 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(bidiPlan.glyphToUnit[2], Reveal::NO_UNIT, TEST_LOCATION);
  DALI_TEST_EQUALS(bidiPlan.glyphToUnit[3], 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(bidiPlan.glyphToUnit[4], 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealSyntheticReplacementDoesNotConsumeTimingP(void)
{
  UiTestApplication application;

  auto BuildPlan = [](const UiText::ModelInterface& model, Reveal::Unit unit, float ratio)
  {
    if(unit == Reveal::Unit::WORD)
    {
      TextAbstraction::Segmentation segmentation = TextAbstraction::Segmentation::New();
      return Reveal::BuildPlan(model, unit, ratio, segmentation);
    }
    return Reveal::BuildCharacterPlan(model, ratio);
  };

  auto CheckNoDeadUnits = [](const UiText::ModelInterface& model, const Reveal::Plan& plan)
  {
    const UiText::GlyphInfo* glyphs = model.GetGlyphs();
    DALI_TEST_CHECK(glyphs);
    DALI_TEST_EQUALS(plan.glyphToUnit.size(), static_cast<size_t>(model.GetNumberOfGlyphs()), TEST_LOCATION);
    std::vector<bool> hasOrdinaryGlyph(plan.GetUnitCount(), false);
    for(UiText::GlyphIndex glyph = 0u; glyph < model.GetNumberOfGlyphs(); ++glyph)
    {
      const uint32_t unit = plan.glyphToUnit[glyph];
      if(UiText::IsSyntheticReplacementGlyph(glyphs[glyph]))
      {
        DALI_TEST_EQUALS(unit, Reveal::NO_UNIT, TEST_LOCATION);
      }
      else if(unit != Reveal::NO_UNIT)
      {
        DALI_TEST_CHECK(unit < hasOrdinaryGlyph.size());
        hasOrdinaryGlyph[unit] = true;
      }
    }
    for(bool populated : hasOrdinaryGlyph)
    {
      DALI_TEST_CHECK(populated);
    }
  };

  // This is the processing model produced for "A [ImageSpan] B": the logical
  // image range is replaced by one layout-only synthetic glyph.
  UiText::ControllerPtr primary = BuildReplacementController(
    "A icon B", {{2u, 4u}}, Size(320.0f, 80.0f));
  const UiText::ModelInterface* primaryModel = primary->GetRenderTextModel();
  DALI_TEST_CHECK(primaryModel && primaryModel != primary->GetLogicalTextModel());
  for(Reveal::Unit unit : {Reveal::Unit::CHARACTER, Reveal::Unit::WORD})
  {
    const Reveal::Plan automatic = BuildPlan(*primaryModel, unit, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
    CheckNoDeadUnits(*primaryModel, automatic);
    DALI_TEST_EQUALS(automatic.GetUnitCount(), 2u, TEST_LOCATION);
    DALI_TEST_EQUALS(automatic.fadeDurationRatio, UiText::Reveal::AUTO_FADE_DURATION_RATIO, EPSILON, TEST_LOCATION);
    const float fadeSpan = std::sqrt(2.0f);
    DALI_TEST_EQUALS(automatic.fadeDuration, fadeSpan / (1.0f + fadeSpan), EPSILON, TEST_LOCATION);

    const Reveal::Plan explicitRatio = BuildPlan(*primaryModel, unit, 0.2f);
    CheckNoDeadUnits(*primaryModel, explicitRatio);
    DALI_TEST_EQUALS(explicitRatio.GetUnitCount(), 2u, TEST_LOCATION);
    DALI_TEST_EQUALS(explicitRatio.fadeDurationRatio, 0.2f, EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(explicitRatio.fadeDuration, 0.2f, EPSILON, TEST_LOCATION);

    UiText::TypesetterPtr primaryTypesetter = UiText::Typesetter::New(primaryModel);
    const Reveal::Plan    linePlan = primaryTypesetter->CreateFinalRevealPlan(automatic,
                                                                              unit,
                                                                              Reveal::Sequence::PER_LINE,
                                                                              0.5f);
    DALI_TEST_CHECK(linePlan.glyphToUnit == automatic.glyphToUnit);
    DALI_TEST_CHECK(linePlan.unitStart == automatic.unitStart);
    DALI_TEST_EQUALS(linePlan.fadeDuration, automatic.fadeDuration, EPSILON, TEST_LOCATION);
  }

  UiText::ControllerPtr replacementOnlyLine = BuildReplacementController(
    "AAAA BBBB CCCC", {{5u, 4u}}, Size(55.0f, 240.0f), false, 24.0f);
  replacementOnlyLine->SetMultiLineEnabled(true);
  replacementOnlyLine->Relayout(Size(55.0f, 240.0f));
  const UiText::ModelInterface* replacementOnlyModel = replacementOnlyLine->GetRenderTextModel();
  DALI_TEST_CHECK(replacementOnlyModel && replacementOnlyModel->GetNumberOfLines() >= 3u);
  const Reveal::Plan replacementOnlySource = BuildPlan(*replacementOnlyModel,
                                                        Reveal::Unit::CHARACTER,
                                                        0.0f);
  CheckNoDeadUnits(*replacementOnlyModel, replacementOnlySource);
  UiText::TypesetterPtr replacementOnlyTypesetter = UiText::Typesetter::New(replacementOnlyModel);
  const Reveal::Plan replacementOnlyPlan = replacementOnlyTypesetter->CreateFinalRevealPlan(
    replacementOnlySource,
    Reveal::Unit::CHARACTER,
    Reveal::Sequence::PER_LINE,
    0.5f);
  DALI_TEST_EQUALS(replacementOnlyPlan.GetUnitCount(), 8u, TEST_LOCATION);
  DALI_TEST_EQUALS(replacementOnlyPlan.unitStart[0u], 0.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(replacementOnlyPlan.unitStart[4u], 1.0f / 3.0f, EPSILON, TEST_LOCATION);

  // Multiple image replacements cover start, middle and end. Units containing
  // remaining ordinary glyphs survive, while replacement-only units vanish.
  UiText::ControllerPtr multiple = BuildReplacementController(
    "XaYbZ", {{0u, 1u}, {2u, 1u}, {4u, 1u}}, Size(320.0f, 80.0f));
  const UiText::ModelInterface* multipleModel = multiple->GetRenderTextModel();
  DALI_TEST_CHECK(multipleModel);
  const Reveal::Plan multipleCharacters = BuildPlan(*multipleModel, Reveal::Unit::CHARACTER, 0.2f);
  const Reveal::Plan multipleWords      = BuildPlan(*multipleModel, Reveal::Unit::WORD, 0.2f);
  CheckNoDeadUnits(*multipleModel, multipleCharacters);
  CheckNoDeadUnits(*multipleModel, multipleWords);
  DALI_TEST_EQUALS(multipleCharacters.GetUnitCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(multipleWords.GetUnitCount(), 1u, TEST_LOCATION);

  // Logical unit ordering must remain independent of bidi visual traversal.
  UiText::ControllerPtr bidi = BuildReplacementController(
    "\xD7\x90X\xD7\x91", {{1u, 1u}}, Size(320.0f, 80.0f));
  const UiText::ModelInterface* bidiModel = bidi->GetRenderTextModel();
  DALI_TEST_CHECK(bidiModel);
  for(Reveal::Unit unit : {Reveal::Unit::CHARACTER, Reveal::Unit::WORD})
  {
    const Reveal::Plan bidiPlan = BuildPlan(*bidiModel, unit, 0.2f);
    CheckNoDeadUnits(*bidiModel, bidiPlan);
    if(unit == Reveal::Unit::CHARACTER)
    {
      const auto& glyphMap = bidiModel->GetGlyphsToCharacters();
      std::vector<std::pair<UiText::CharacterIndex, uint32_t>> logicalUnits;
      for(UiText::GlyphIndex glyph = 0u; glyph < bidiModel->GetNumberOfGlyphs(); ++glyph)
      {
        if(bidiPlan.glyphToUnit[glyph] != Reveal::NO_UNIT)
        {
          logicalUnits.emplace_back(glyphMap[glyph], bidiPlan.glyphToUnit[glyph]);
        }
      }
      std::sort(logicalUnits.begin(), logicalUnits.end());
      DALI_TEST_CHECK(!logicalUnits.empty());
      DALI_TEST_EQUALS(logicalUnits.front().second, 0u, TEST_LOCATION);
      for(size_t index = 1u; index < logicalUnits.size(); ++index)
      {
        DALI_TEST_CHECK(logicalUnits[index - 1u].second <= logicalUnits[index].second);
      }
      DALI_TEST_EQUALS(logicalUnits.back().second, bidiPlan.GetUnitCount() - 1u, TEST_LOCATION);
    }
  }

  // END ellipsis may retain or remove the image box, but the final Reveal
  // schedule must be backed only by ordinary text or the generated ellipsis.
  UiText::ControllerPtr elided = BuildReplacementController(
    "A icon B C D E F G H I J K L M N O P", {{2u, 4u}}, Size(320.0f, 80.0f), true);
  const Vector3 elidedNaturalSize = elided->GetNaturalSize(false);
  DALI_TEST_CHECK(elidedNaturalSize.x > 0.0f && elidedNaturalSize.y > 0.0f);
  const Vector2 elidedLayoutSize(elidedNaturalSize.x * 0.5f, elidedNaturalSize.y + 1.0f);
  elided->Relayout(elidedLayoutSize);
  const UiText::ModelInterface* elidedModel = elided->GetRenderTextModel();
  DALI_TEST_CHECK(elidedModel);
  UiText::TypesetterPtr typesetter = UiText::Typesetter::New(elidedModel);
  typesetter->SetFinalElisionResult(elided->GetFinalElisionResult());
  for(Reveal::Unit unit : {Reveal::Unit::CHARACTER, Reveal::Unit::WORD})
  {
    const Reveal::Plan source = BuildPlan(*elidedModel, unit, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
    CheckNoDeadUnits(*elidedModel, source);
    const Reveal::Plan finalPlan = typesetter->CreateFinalRevealPlan(source,
                                                                     unit,
                                                                     Reveal::Sequence::PER_LINE,
                                                                     0.5f);
    UiText::ViewModel* viewModel = typesetter->GetViewModel();
    DALI_TEST_CHECK(viewModel);
    const UiText::GlyphIndex ellipsisGlyph = viewModel->GetEllipsisFinalGlyphIndex();
    DALI_TEST_CHECK(ellipsisGlyph < finalPlan.glyphToUnit.size());

    const UiText::GlyphInfo* finalGlyphs = viewModel->GetGlyphs();
    DALI_TEST_CHECK(finalGlyphs);
    std::vector<bool> hasRenderableGlyph(finalPlan.GetUnitCount(), false);
    for(UiText::GlyphIndex glyph = 0u; glyph < finalPlan.glyphToUnit.size(); ++glyph)
    {
      const uint32_t revealUnit = finalPlan.glyphToUnit[glyph];
      if(UiText::IsSyntheticReplacementGlyph(finalGlyphs[glyph]))
      {
        DALI_TEST_EQUALS(revealUnit, Reveal::NO_UNIT, TEST_LOCATION);
      }
      else if(revealUnit != Reveal::NO_UNIT)
      {
        hasRenderableGlyph[revealUnit] = true;
      }
    }
    for(bool populated : hasRenderableGlyph)
    {
      DALI_TEST_CHECK(populated);
    }
  }
  END_TEST;
}

int UtcDaliTextRevealProgressEndpointsP(void)
{
  const uint32_t unitCounts[]         = {1u, 2u, 7u, 257u};
  const float    fadeDurationRatios[] = {0.0f, UiText::Reveal::AUTO_FADE_DURATION_RATIO};
  for(uint32_t unitCount : unitCounts)
  {
    std::vector<UiText::Character>      text(unitCount, static_cast<UiText::Character>('A'));
    std::vector<UiText::CharacterIndex> glyphMap(unitCount);
    for(uint32_t index = 0u; index < unitCount; ++index)
    {
      glyphMap[index] = index;
    }

    for(float fadeDurationRatio : fadeDurationRatios)
    {
      const Reveal::Plan plan = Reveal::BuildPlan(text.data(), unitCount, glyphMap.data(), unitCount,
                                                  Reveal::Unit::CHARACTER, fadeDurationRatio);
      for(uint32_t unit = 0u; unit < plan.GetUnitCount(); ++unit)
      {
        // Metadata encodes start in two 8-bit channels. Decode that exact
        // representation before checking the CPU/shader endpoint contract.
        const uint32_t encodedStart = static_cast<uint32_t>(
          std::round(std::max(0.0f, std::min(1.0f, plan.unitStart[unit])) * 65535.0f));
        const float decodedStart = static_cast<float>(encodedStart) / 65535.0f;
        DALI_TEST_CHECK(CalculateRevealOpacityReference(0.0f, decodedStart, plan.fadeDuration) == 0.0f);
        DALI_TEST_CHECK(CalculateRevealOpacityReference(1.0f, decodedStart, plan.fadeDuration) == 1.0f);
      }
    }
  }
  END_TEST;
}

int UtcDaliTextRevealLabelAnimationNoRerasterP(void)
{
  UiTestApplication application;
  Dali::Ui::Label   label = Dali::Ui::Label::New("Hello sequential reveal world");
  label.SetProperty(Actor::Property::SIZE, Vector3(420.0f, 80.0f, 0.0f));

  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(label.GetPropertyIndex("uTextRevealProgress"), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetRendererAt(0u).GetTextures().GetTextureCount(), 1u, TEST_LOCATION);

  UiText::Reveal reveal;
  reveal.SetUnit(UiText::Reveal::Unit::WORD);
  label.SetTextReveal(reveal);
  label.SetTextRevealProgress(0.0f);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK(label.GetRendererCount() > 0u);
  Renderer   renderer = label.GetRendererAt(0u);
  TextureSet textures = renderer.GetTextures();
  DALI_TEST_CHECK(renderer.GetPropertyIndex("uTextRevealProgress") != Property::INVALID_INDEX);
  DALI_TEST_CHECK(renderer.GetPropertyIndex("uTextRevealFadeDuration") != Property::INVALID_INDEX);
  DALI_TEST_CHECK(textures.GetTextureCount() >= 2u);
  Texture metadata = textures.GetTexture(textures.GetTextureCount() - 1u);

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.EnableTextureCallTrace(true);
  gl.ResetTextureCallStack();

  Animation animation = Animation::New(0.20f);
  label.Animate(animation).TextRevealProgress(1.0f, Dali::Ui::Duration(0.20f));
  animation.Play();
  for(uint32_t frame = 0u; frame < 8u; ++frame)
  {
    application.SendNotification();
    application.Render(32);
    DALI_TEST_CHECK(label.GetRendererAt(0u) == renderer);
    TextureSet currentTextures = label.GetRendererAt(0u).GetTextures();
    DALI_TEST_CHECK(currentTextures.GetTexture(currentTextures.GetTextureCount() - 1u) == metadata);
  }
  DALI_TEST_EQUALS(label.GetCurrentProperty<float>(label.GetPropertyIndex("uTextRevealProgress")),
                   1.0f, EPSILON, TEST_LOCATION);

  Animation reverse = Animation::New(0.20f);
  label.Animate(reverse).TextRevealProgress(0.0f, Dali::Ui::Duration(0.20f));
  reverse.Play();
  for(uint32_t frame = 0u; frame < 8u; ++frame)
  {
    application.SendNotification();
    application.Render(32);
    DALI_TEST_CHECK(label.GetRendererAt(0u) == renderer);
    TextureSet currentTextures = label.GetRendererAt(0u).GetTextures();
    DALI_TEST_CHECK(currentTextures.GetTexture(currentTextures.GetTextureCount() - 1u) == metadata);
  }
  DALI_TEST_EQUALS(label.GetCurrentProperty<float>(label.GetPropertyIndex("uTextRevealProgress")),
                   0.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexImage2D"), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexSubImage2D"), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("CompressedTexImage2D"), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("CompressedTexSubImage2D"), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealWhitespaceAndPunctuationP(void)
{
  const UiText::Character      text[]     = {'H', 'i', ' ', ' ', '\n', '!', ' ', 'A'};
  const UiText::CharacterIndex glyphMap[] = {0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u};
  const Reveal::Plan           plan       = Reveal::BuildPlan(text, 8u, glyphMap, 8u,
                                                              Reveal::Unit::CHARACTER, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  DALI_TEST_EQUALS(plan.GetUnitCount(), 4u, TEST_LOCATION); // H, i, !, A
  DALI_TEST_EQUALS(plan.glyphToUnit[2], Reveal::NO_UNIT, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.glyphToUnit[3], Reveal::NO_UNIT, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.glyphToUnit[4], Reveal::NO_UNIT, TEST_LOCATION);
  DALI_TEST_EQUALS(plan.glyphToUnit[6], Reveal::NO_UNIT, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealFinalElisionProjectionP(void)
{
  Reveal::Plan source;
  source.glyphToUnit         = {0u, 1u, 2u, 3u, 4u};
  source.unitStart           = {0.0f, 0.2f, 0.4f, 0.6f, 0.8f};
  source.fadeDurationRatio   = UiText::Reveal::AUTO_FADE_DURATION_RATIO;
  const float sourceFadeSpan = std::sqrt(5.0f);
  source.fadeDuration        = sourceFadeSpan / (4.0f + sourceFadeSpan);

  // Only source units 1 and 4 survive, in visual bidi order. Projection
  // compacts them in logical order and appends the generated END ellipsis.
  const UiText::GlyphIndex invalid     = std::numeric_limits<UiText::GlyphIndex>::max();
  const UiText::GlyphIndex endMap[]    = {4u, 1u, invalid};

  const Reveal::Plan character = Reveal::ProjectToFinalGlyphs(
    source, 3u, endMap, 2u, Reveal::Unit::CHARACTER);
  DALI_TEST_EQUALS(character.GetUnitCount(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(character.glyphToUnit[0], 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(character.glyphToUnit[1], 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(character.glyphToUnit[2], 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(character.unitStart.back() + character.fadeDuration, 1.0f, EPSILON, TEST_LOCATION);

  const Reveal::Plan word = Reveal::ProjectToFinalGlyphs(
    source, 3u, endMap, 2u, Reveal::Unit::WORD);
  DALI_TEST_EQUALS(word.GetUnitCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(word.glyphToUnit[0], 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(word.glyphToUnit[1], 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(word.glyphToUnit[2], 1u, TEST_LOCATION);
  const float wordFadeSpan = std::sqrt(2.0f);
  DALI_TEST_EQUALS(word.fadeDurationRatio, UiText::Reveal::AUTO_FADE_DURATION_RATIO, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(word.fadeDuration, wordFadeSpan / (1.0f + wordFadeSpan), EPSILON, TEST_LOCATION);
  DALI_TEST_CHECK(std::fabs(word.fadeDuration - source.fadeDuration) > EPSILON);

  Reveal::Plan explicitSource      = source;
  explicitSource.fadeDurationRatio = 0.2f;
  const Reveal::Plan explicitWord  = Reveal::ProjectToFinalGlyphs(
    explicitSource, 3u, endMap, 2u, Reveal::Unit::WORD);
  DALI_TEST_EQUALS(explicitWord.GetUnitCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(explicitWord.fadeDurationRatio, 0.2f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(explicitWord.fadeDuration, 0.2f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(explicitWord.unitStart[1] - explicitWord.unitStart[0], 0.8f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(explicitWord.unitStart.back() + explicitWord.fadeDuration, 1.0f, EPSILON, TEST_LOCATION);

  const UiText::GlyphIndex noEllipsisMap[] = {4u, 1u};
  const Reveal::Plan       noEllipsis      = Reveal::ProjectToFinalGlyphs(
    source, 2u, noEllipsisMap, invalid, Reveal::Unit::CHARACTER);
  DALI_TEST_EQUALS(noEllipsis.GetUnitCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(noEllipsis.glyphToUnit[0], 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(noEllipsis.glyphToUnit[1], 0u, TEST_LOCATION);

  const UiText::GlyphIndex onlyMap[] = {invalid};
  for(Reveal::Unit unit : {Reveal::Unit::CHARACTER, Reveal::Unit::WORD})
  {
    const Reveal::Plan only = Reveal::ProjectToFinalGlyphs(
      source, 1u, onlyMap, 0u, unit);
    DALI_TEST_EQUALS(only.GetUnitCount(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(only.glyphToUnit[0], 0u, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliTextRevealBidiVisibleUnitOrderingP(void)
{
  Reveal::Plan source;
  source.glyphToUnit       = {3u, 2u, Reveal::NO_UNIT, 1u, 0u};
  source.unitStart         = {0.0f, 0.25f, 0.5f, 0.75f};
  source.fadeDurationRatio = 0.2f;

  const UiText::GlyphIndex mapping[] = {0u, 1u, 3u};
  const Reveal::Plan       projected = Reveal::ProjectToFinalGlyphs(
    source, 3u, mapping, UiText::FinalElisionResult::INVALID_GLYPH_INDEX,
    Reveal::Unit::CHARACTER);
  DALI_TEST_EQUALS(projected.GetUnitCount(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(projected.glyphToUnit[0], 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(projected.glyphToUnit[1], 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(projected.glyphToUnit[2], 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(projected.fadeDuration, 0.2f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(projected.unitStart.back(), 0.8f, EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealNonFiniteProgressP(void)
{
  const UiText::Character      text[]     = {'A'};
  const UiText::CharacterIndex glyphMap[] = {0u};

  const Reveal::Plan fadePlan = Reveal::BuildPlan(
    text, 1u, glyphMap, 1u, Reveal::Unit::CHARACTER, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  DALI_TEST_EQUALS(fadePlan.fadeDurationRatio, UiText::Reveal::AUTO_FADE_DURATION_RATIO, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(CalculateRevealOpacityReference(std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.5f),
                   0.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(CalculateRevealOpacityReference(std::numeric_limits<float>::infinity(), 0.0f, 0.5f),
                   1.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(CalculateRevealOpacityReference(-std::numeric_limits<float>::infinity(), 0.0f, 0.5f),
                   0.0f, EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealEllipsisMetadataP(void)
{
  UiTestApplication     application;
  UiText::ControllerPtr controller = UiText::Controller::New();
  controller->SetText("Alpha Beta Gamma Delta Epsilon Zeta Eta Theta Iota Kappa Lambda");
  controller->SetDefaultFontSize(24.0f, UiText::Controller::PIXEL_SIZE);
  controller->SetTextElideEnabled(true);
  const Vector3 naturalSize = controller->GetNaturalSize(false);
  DALI_TEST_CHECK(naturalSize.x > 0.0f && naturalSize.y > 0.0f);
  const Vector2 layoutSize(naturalSize.x * 0.5f, naturalSize.y + 1.0f);
  controller->Relayout(layoutSize);

  const UiText::ModelInterface* model = controller->GetRenderTextModel();
  DALI_TEST_CHECK(model);
  const UiText::FinalElisionResult* finalElision = controller->GetFinalElisionResult();
  DALI_TEST_CHECK(finalElision);
  DALI_TEST_CHECK(finalElision->ellipsisFinalGlyphIndex < finalElision->glyphs.Count());
  DALI_TEST_EQUALS(finalElision->finalToSourceGlyphIndices[finalElision->ellipsisFinalGlyphIndex],
                   UiText::FinalElisionResult::INVALID_GLYPH_INDEX,
                   TEST_LOCATION);

  for(Reveal::Unit unit : {Reveal::Unit::CHARACTER, Reveal::Unit::WORD})
  {
    Reveal::Plan source;
    if(unit == Reveal::Unit::WORD)
    {
      TextAbstraction::Segmentation segmentation = TextAbstraction::Segmentation::New();
      source                                     = Reveal::BuildPlan(*model, unit, UiText::Reveal::AUTO_FADE_DURATION_RATIO, segmentation);
    }
    else
    {
      source = Reveal::BuildCharacterPlan(*model, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
    }

    UiText::TypesetterPtr typesetter = UiText::Typesetter::New(model);
    typesetter->SetFinalElisionResult(finalElision);
    const Reveal::Plan textPlan = typesetter->CreateFinalRevealPlan(source, unit);
    const Reveal::Plan finalPlan = typesetter->CreateFinalRevealPlan(source,
                                                                     unit,
                                                                     Reveal::Sequence::PER_LINE,
                                                                     0.5f);
    DALI_TEST_CHECK(finalPlan.glyphToUnit == textPlan.glyphToUnit);
    DALI_TEST_CHECK(finalPlan.unitStart == textPlan.unitStart);
    DALI_TEST_EQUALS(finalPlan.fadeDuration, textPlan.fadeDuration, EPSILON, TEST_LOCATION);
    UiText::ViewModel* viewModel = typesetter->GetViewModel();
    DALI_TEST_CHECK(viewModel);
    const UiText::GlyphIndex ellipsisGlyph = viewModel->GetEllipsisFinalGlyphIndex();
    DALI_TEST_CHECK(ellipsisGlyph < finalPlan.glyphToUnit.size());
    DALI_TEST_CHECK(viewModel->GetFinalToSourceGlyphIndices());
    DALI_TEST_EQUALS(viewModel->GetFinalToSourceGlyphIndices()[ellipsisGlyph],
                     UiText::FinalElisionResult::INVALID_GLYPH_INDEX,
                     TEST_LOCATION);
    DALI_TEST_CHECK(finalPlan.glyphToUnit[ellipsisGlyph] != Reveal::NO_UNIT);
    DALI_TEST_CHECK(finalPlan.GetUnitCount() > 0u);
    DALI_TEST_CHECK(finalPlan.GetUnitCount() < source.GetUnitCount());

    float     fadeDuration = 0.0f;
    PixelData metadata     = typesetter->RenderTextRevealMetadata(
      layoutSize, UiText::Direction::LEFT_TO_RIGHT,
      finalPlan, fadeDuration);
    DALI_TEST_CHECK(metadata);
    DALI_TEST_EQUALS(fadeDuration, finalPlan.fadeDuration, EPSILON, TEST_LOCATION);

    const uint32_t ellipsisUnit  = finalPlan.glyphToUnit[ellipsisGlyph];
    const uint32_t ellipsisStart = static_cast<uint32_t>(
      std::round(finalPlan.unitStart[ellipsisUnit] * 65535.0f));
    const Dali::Integration::PixelDataBuffer pixels = Dali::Integration::GetPixelDataBuffer(metadata);
    DALI_TEST_CHECK(pixels.buffer);
    bool foundEllipsisTiming = false;
    for(uint32_t y = 0u; y < metadata.GetHeight() && !foundEllipsisTiming; ++y)
    {
      const uint8_t* row = pixels.buffer + y * metadata.GetStrideBytes();
      for(uint32_t x = 0u; x < metadata.GetWidth(); ++x)
      {
        const uint8_t* pixel = row + x * 4u;
        if(pixel[2] != 0u)
        {
          const uint32_t encoded = static_cast<uint32_t>(pixel[0]) * 256u + pixel[1];
          foundEllipsisTiming |= encoded == ellipsisStart;
        }
      }
    }
    DALI_TEST_CHECK(foundEllipsisTiming);
    DALI_TEST_EQUALS(CalculateRevealOpacityReference(0.0f, finalPlan.unitStart[ellipsisUnit], fadeDuration),
                     0.0f, EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(CalculateRevealOpacityReference(1.0f, finalPlan.unitStart[ellipsisUnit], fadeDuration),
                     1.0f, EPSILON, TEST_LOCATION);

    // Final mapping is a one-shot reveal projection request. A subsequent
    // ordinary render must consume no mapping storage.
    PixelData ordinary = typesetter->Render(layoutSize, UiText::Direction::LEFT_TO_RIGHT,
                                            UiText::Typesetter::RENDER_NO_STYLES);
    DALI_TEST_CHECK(ordinary);
    DALI_TEST_CHECK(viewModel->GetFinalToSourceGlyphIndices() == nullptr);
  }
  END_TEST;
}

int UtcDaliTextRevealTileBoundaryMetadataP(void)
{
  UiTestApplication application;
  const Vector2     fullSize(180.0f, 160.0f);

  UiText::ControllerPtr controller = UiText::Controller::New();
  // Stable ASCII ascenders and descenders exercise a boundary crossing without
  // relying on emoji fallback or font-specific italic overhang.
  controller->SetText("Agjpqy Agjpqy Agjpqy Agjpqy Agjpqy");
  controller->SetDefaultFontSize(40.0f, UiText::Controller::PIXEL_SIZE);
  controller->SetMultiLineEnabled(true);
  controller->Relayout(fullSize);

  const UiText::ModelInterface* model = controller->GetRenderTextModel();
  DALI_TEST_CHECK(model);
  UiText::TypesetterPtr typesetter = UiText::Typesetter::New(model);
  const Reveal::Plan    sourcePlan = Reveal::BuildCharacterPlan(*model, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  const Reveal::Plan    finalPlan  = typesetter->CreateFinalRevealPlan(sourcePlan,
                                                                       Reveal::Unit::CHARACTER,
                                                                       Reveal::Sequence::PER_LINE,
                                                                       0.5f);

  float     fadeDuration = 0.0f;
  PixelData full         = typesetter->RenderTextRevealMetadata(
    fullSize, UiText::Direction::LEFT_TO_RIGHT, finalPlan, fadeDuration);
  DALI_TEST_CHECK(full);
  const Dali::Integration::PixelDataBuffer fullPixels = Dali::Integration::GetPixelDataBuffer(full);
  DALI_TEST_CHECK(fullPixels.buffer);

  // Pick a boundary that cuts through actual glyph coverage. A correct tiled
  // traversal must then reproduce the two adjacent full-texture rows exactly.
  uint32_t boundary = 0u;
  for(uint32_t y = 1u; y < full.GetHeight() && boundary == 0u; ++y)
  {
    const uint8_t* previous = fullPixels.buffer + (y - 1u) * full.GetStrideBytes();
    const uint8_t* current  = fullPixels.buffer + y * full.GetStrideBytes();
    for(uint32_t x = 0u; x < full.GetWidth(); ++x)
    {
      if(previous[x * 4u + 2u] != 0u && current[x * 4u + 2u] != 0u)
      {
        boundary = y;
        break;
      }
    }
  }
  DALI_TEST_CHECK(boundary > 0u && boundary < full.GetHeight());

  PixelData upper = typesetter->RenderTextRevealMetadata(
    Vector2(fullSize.width, static_cast<float>(boundary)),
    UiText::Direction::LEFT_TO_RIGHT, finalPlan, fadeDuration, 0u, fullSize);
  PixelData lower = typesetter->RenderTextRevealMetadata(
    Vector2(fullSize.width, fullSize.height - static_cast<float>(boundary)),
    UiText::Direction::LEFT_TO_RIGHT, finalPlan, fadeDuration, boundary, fullSize);
  DALI_TEST_CHECK(upper);
  DALI_TEST_CHECK(lower);

  const Dali::Integration::PixelDataBuffer upperPixels = Dali::Integration::GetPixelDataBuffer(upper);
  const Dali::Integration::PixelDataBuffer lowerPixels = Dali::Integration::GetPixelDataBuffer(lower);
  const size_t                             rowBytes    = static_cast<size_t>(full.GetWidth()) * 4u;
  for(uint32_t y = 0u; y < boundary; ++y)
  {
    DALI_TEST_EQUALS(std::memcmp(fullPixels.buffer + y * full.GetStrideBytes(),
                                 upperPixels.buffer + y * upper.GetStrideBytes(),
                                 rowBytes),
                     0,
                     TEST_LOCATION);
  }
  for(uint32_t y = boundary; y < full.GetHeight(); ++y)
  {
    DALI_TEST_EQUALS(std::memcmp(fullPixels.buffer + y * full.GetStrideBytes(),
                                 lowerPixels.buffer + (y - boundary) * lower.GetStrideBytes(),
                                 rowBytes),
                     0,
                     TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliTextRevealMetadataOwnershipHaloP(void)
{
  constexpr uint32_t WIDTH      = 5u;
  constexpr uint32_t HEIGHT     = 5u;
  constexpr uint32_t PIXEL_SIZE = 4u;

  auto pixelAt = [PIXEL_SIZE](std::vector<uint8_t>& pixels, uint32_t width, uint32_t x, uint32_t y)
  {
    return pixels.data() + (static_cast<size_t>(y) * width + x) * PIXEL_SIZE;
  };

  std::vector<uint8_t> metadata(static_cast<size_t>(WIDTH) * HEIGHT * PIXEL_SIZE, 0u);
  uint8_t*             center = pixelAt(metadata, WIDTH, 2u, 2u);
  center[0u]                  = 0x12u;
  center[1u]                  = 0x34u;
  center[2u]                  = 0xffu;
  center[3u]                  = 0xc8u;

  Reveal::ExpandMetadataOwnership(metadata.data(), WIDTH, HEIGHT);

  for(uint32_t y = 0u; y < HEIGHT; ++y)
  {
    for(uint32_t x = 0u; x < WIDTH; ++x)
    {
      const uint8_t* pixel = pixelAt(metadata, WIDTH, x, y);
      const uint32_t dx    = x > 2u ? x - 2u : 2u - x;
      const uint32_t dy    = y > 2u ? y - 2u : 2u - y;
      if(dx == 0u && dy == 0u)
      {
        // Raster-owned metadata, including coverage, remains bit-identical.
        DALI_TEST_EQUALS(pixel[0u], 0x12u, TEST_LOCATION);
        DALI_TEST_EQUALS(pixel[1u], 0x34u, TEST_LOCATION);
        DALI_TEST_EQUALS(pixel[2u], 0xffu, TEST_LOCATION);
        DALI_TEST_EQUALS(pixel[3u], 0xc8u, TEST_LOCATION);
      }
      else if(dx <= 1u && dy <= 1u)
      {
        // Halo texels own the same timing while carrying no raster coverage.
        DALI_TEST_EQUALS(pixel[0u], 0x12u, TEST_LOCATION);
        DALI_TEST_EQUALS(pixel[1u], 0x34u, TEST_LOCATION);
        DALI_TEST_EQUALS(pixel[2u], 0xffu, TEST_LOCATION);
        DALI_TEST_EQUALS(pixel[3u], 0u, TEST_LOCATION);
      }
      else
      {
        // Newly written halo texels must not seed a recursive expansion.
        DALI_TEST_EQUALS(pixel[0u], 0u, TEST_LOCATION);
        DALI_TEST_EQUALS(pixel[1u], 0u, TEST_LOCATION);
        DALI_TEST_EQUALS(pixel[2u], 0u, TEST_LOCATION);
        DALI_TEST_EQUALS(pixel[3u], 0u, TEST_LOCATION);
      }
    }
  }

  // When two units compete for an empty texel, later-start-wins prevents a
  // future glyph's LINEAR coverage from inheriting an earlier unit's timing.
  std::vector<uint8_t> conflict(3u * PIXEL_SIZE, 0u);
  uint8_t*             early = pixelAt(conflict, 3u, 0u, 0u);
  early[0u]                  = 0x10u;
  early[1u]                  = 0x00u;
  early[2u]                  = 0xffu;
  early[3u]                  = 0xffu;
  uint8_t* late              = pixelAt(conflict, 3u, 2u, 0u);
  late[0u]                   = 0x90u;
  late[1u]                   = 0x00u;
  late[2u]                   = 0xffu;
  late[3u]                   = 0x01u;

  Reveal::ExpandMetadataOwnership(conflict.data(), 3u, 1u);

  const uint8_t* selected = pixelAt(conflict, 3u, 1u, 0u);
  DALI_TEST_EQUALS(selected[0u], 0x90u, TEST_LOCATION);
  DALI_TEST_EQUALS(selected[1u], 0x00u, TEST_LOCATION);
  DALI_TEST_EQUALS(selected[2u], 0xffu, TEST_LOCATION);
  DALI_TEST_EQUALS(selected[3u], 0u, TEST_LOCATION);

  // A buffer containing no revealable glyph ownership remains background.
  std::vector<uint8_t> empty(3u * 3u * PIXEL_SIZE, 0u);
  Reveal::ExpandMetadataOwnership(empty.data(), 3u, 3u);
  DALI_TEST_CHECK(std::all_of(empty.begin(), empty.end(), [](uint8_t value) { return value == 0u; }));
  END_TEST;
}
