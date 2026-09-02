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

#include <dali-ui-foundation/internal/text/async-text/async-text-loader-impl.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl.h>
#include <dali-ui-foundation/internal/text/controller/text-controller.h>
#include <dali-ui-foundation/internal/text/line-run.h>
#include <dali-ui-foundation/internal/text/rendering/text-typesetter-impl.h>
#include <dali-ui-foundation/internal/text/rendering/text-typesetter.h>
#include <dali-ui-foundation/internal/text/rendering/view-model.h>
#include <dali-ui-foundation/internal/text/replacement/inline-replacement-data.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-run-snapshot.h>
#include <dali-ui-foundation/internal/text/reveal/text-reveal.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/animation/duration.h>
#include <dali-ui-foundation/public-api/animation/label-animation-bridge.autogen.h>
#include <dali-ui-foundation/public-api/image-loader/image-url.h>
#include <dali-ui-foundation/public-api/text/style/reveal.h>
#include <dali-ui-foundation/public-api/text/styled-text/font-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/image-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text-builder.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-async-task-manager.h>
#include <dali-ui/ui-event-thread-callback.h>
#include <dali.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/devel-api/text-abstraction/segmentation.h>
#include <dali/integration-api/pixel-data-integ.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <limits>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "inline-replacement-manager-test-accessor.h"

using namespace Dali;

namespace Dali::Ui::Internal
{
std::size_t GetInlineReplacementRevealConstraintCount(Ui::View owner)
{
  const Text::InlineReplacementData* data = Text::GetInlineReplacementData(owner);
  return data ? Text::InlineReplacementManagerTestAccessor::GetRevealConstraintCount(data->manager) : 0u;
}

std::size_t GetInlineReplacementRevealTimingCount(Ui::View owner)
{
  const Text::InlineReplacementData* data = Text::GetInlineReplacementData(owner);
  return data ? Text::InlineReplacementManagerTestAccessor::GetRevealTimingCount(data->manager) : 0u;
}

uint64_t GetInlineReplacementEntrySourceRevision(Ui::View owner)
{
  const Text::InlineReplacementData* data = Text::GetInlineReplacementData(owner);
  return data ? Text::InlineReplacementManagerTestAccessor::GetEntrySourceRevision(data->manager) : 0u;
}

uint64_t GetInlineReplacementRevealSourceRevision(Ui::View owner)
{
  const Text::InlineReplacementData* data = Text::GetInlineReplacementData(owner);
  return data ? Text::InlineReplacementManagerTestAccessor::GetRevealSourceRevision(data->manager) : 0u;
}

bool IsInlineReplacementRevealPixelSpatial(Ui::View owner)
{
  const Text::InlineReplacementData* data = Text::GetInlineReplacementData(owner);
  return data && Text::InlineReplacementManagerTestAccessor::IsRevealPixelSpatial(data->manager, 1u);
}

bool GetInlineReplacementRevealTiming(Ui::View owner, Ui::Text::ReplacementRevealTiming& timing)
{
  const Text::InlineReplacementData* data = Text::GetInlineReplacementData(owner);
  return data && Text::InlineReplacementManagerTestAccessor::GetRevealTiming(data->manager, 1u, timing);
}
} // namespace Dali::Ui::Internal

namespace
{
namespace Reveal = Dali::Ui::Text::Internal::Reveal;
namespace UiText = Dali::Ui::Text;

constexpr float EPSILON = 0.0001f;

std::string GetRepositoryResourcePath(const char* relativePath)
{
  const std::string testResourceDirectory(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR);
  const std::string marker("/automated-tests/");
  const std::size_t markerOffset = testResourceDirectory.rfind(marker);
  return markerOffset == std::string::npos
           ? std::string(relativePath)
           : testResourceDirectory.substr(0u, markerOffset + 1u) + relativePath;
}

Ui::Integration::Visual::Base FindInlineReplacementVisual(Ui::View owner)
{
  const Property::Index visualIndex = owner.GetPropertyIndex("__dali_ui_inline_replacement_0");
  if(visualIndex == Property::INVALID_INDEX)
  {
    return {};
  }
  auto& viewData = Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(owner));
  return viewData.GetVisual(visualIndex);
}

Renderer FindInlineReplacementRevealRenderer(Ui::View owner)
{
  Ui::Integration::Visual::Base visual = FindInlineReplacementVisual(owner);
  return visual ? visual.GetRenderer() : Renderer{};
}

std::size_t CountInlineReplacementRevealBaseOpacityProperties(Ui::View owner,
                                                              std::size_t maximumSlotCount)
{
  auto&       viewData = Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(owner));
  std::size_t count    = 0u;
  for(std::size_t slot = 0u; slot < maximumSlotCount; ++slot)
  {
    const std::string propertyName = "__dali_ui_inline_replacement_" + std::to_string(slot);
    const Property::Index visualIndex = owner.GetPropertyIndex(Dali::String(propertyName.c_str()));
    if(visualIndex == Property::INVALID_INDEX)
    {
      continue;
    }
    Ui::Integration::Visual::Base visual = viewData.GetVisual(visualIndex);
    if(visual && visual.GetRenderer().GetPropertyIndex("__dali_ui_inline_replacement_reveal_base_opacity") !=
                   Property::INVALID_INDEX)
    {
      ++count;
    }
  }
  return count;
}

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
                                                 const std::vector<UiText::CharacterRun>&     ranges,
                                                 const Size&                                 size,
                                                 bool                                        elideText         = false,
                                                 float                                       replacementHeight = 24.0f,
                                                 float                                       replacementWidth  = 32.0f)
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
    replacement.metrics.width         = replacementWidth;
    replacement.metrics.height        = replacementHeight;
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

UiText::ControllerPtr BuildReferenceMetricController(const char* text,
                                                     float       fontSize,
                                                     float       relativeLineSize,
                                                     float       uiScale,
                                                     const Size& layoutSize)
{
  UiText::ControllerPtr controller = UiText::Controller::New();
  controller->SetMultiLineEnabled(true);
  controller->SetText(text);
  controller->SetDefaultFontSize(fontSize, UiText::Controller::PIXEL_SIZE);
  controller->SetRelativeLineSize(relativeLineSize);
  controller->SetUiScale(uiScale);
  controller->Relayout(layoutSize);
  return controller;
}

UiText::ControllerPtr BuildMixedSizeReferenceMetricController(float       largeSize,
                                                              bool        heading,
                                                              const Size& layoutSize)
{
  const char*               text    = heading
                                        ? "Heading body line body line body line body line body line body line"
                                        : "small BIG small";
  UiText::StyledTextBuilder builder = UiText::StyledTextBuilder::New(text);
  UiText::FontAttributes    attributes;
  attributes.SetSize(largeSize);
  DALI_TEST_CHECK(builder.SetSpan(UiText::FontSpan::New(attributes),
                                  heading ? 0u : 6u,
                                  heading ? 7u : 9u));

  UiText::ControllerPtr controller = UiText::Controller::New();
  controller->SetDefaultFontSize(20.0f, UiText::Controller::PIXEL_SIZE);
  controller->SetStyledText(builder.Build());
  controller->SetMultiLineEnabled(true);
  controller->Relayout(layoutSize);
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
  DALI_TEST_CHECK(!plan.HasPixelTiming());
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
  const Reveal::Plan           pixelPlan  = Reveal::BuildPlan(text, 9u, glyphMap, 7u,
                                                              Reveal::Unit::PIXEL, UiText::Reveal::AUTO_FADE_DURATION_RATIO);

  DALI_TEST_CHECK(!plan.HasPixelTiming());
  DALI_TEST_CHECK(pixelPlan.glyphToUnit == plan.glyphToUnit);
  DALI_TEST_CHECK(pixelPlan.unitStart == plan.unitStart);
  DALI_TEST_CHECK(!pixelPlan.HasPixelTiming());
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
    const Reveal::Plan    perLinePlan = primaryTypesetter->CreateFinalRevealPlan(automatic,
                                                                              unit,
                                                                              Reveal::Sequence::PER_LINE,
                                                                              0.5f);
    DALI_TEST_CHECK(perLinePlan.glyphToUnit == automatic.glyphToUnit);
    DALI_TEST_CHECK(perLinePlan.unitStart == automatic.unitStart);
    DALI_TEST_EQUALS(perLinePlan.fadeDuration, automatic.fadeDuration, EPSILON, TEST_LOCATION);
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
    const Reveal::Plan wholeTextPlan = typesetter->CreateFinalRevealPlan(source, unit);
    const Reveal::Plan finalPlan = typesetter->CreateFinalRevealPlan(source,
                                                                     unit,
                                                                     Reveal::Sequence::PER_LINE,
                                                                     0.5f);
    DALI_TEST_CHECK(finalPlan.glyphToUnit == wholeTextPlan.glyphToUnit);
    DALI_TEST_CHECK(finalPlan.unitStart == wholeTextPlan.unitStart);
    DALI_TEST_EQUALS(finalPlan.fadeDuration, wholeTextPlan.fadeDuration, EPSILON, TEST_LOCATION);
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
  UiTestApplication           application;
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  const Vector2               fullSize(180.0f, 160.0f);

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

int UtcDaliTextRevealPixelP(void)
{
  UiTestApplication application;
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();

  auto BuildFinalPixelPlan = [](UiText::ControllerPtr controller,
                                float                 fadeRatio,
                                Reveal::Sequence      sequence = Reveal::Sequence::WHOLE_TEXT,
                                float                 staggerRatio = 0.0f)
  {
    const UiText::ModelInterface* model = controller->GetRenderTextModel();
    DALI_TEST_CHECK(model);
    UiText::TypesetterPtr typesetter = UiText::Typesetter::New(model);
    typesetter->SetFinalElisionResult(controller->GetFinalElisionResult());
    const Reveal::Plan source = Reveal::BuildPixelPlan(*model, fadeRatio);
    return typesetter->CreateFinalRevealPlan(source, Reveal::Unit::PIXEL, sequence, staggerRatio);
  };

  // PIXEL reuses the exact CHARACTER logical skeleton. Mixed bidi changes
  // visual X direction inside RTL clusters, not the global logical unit order.
  UiText::ControllerPtr bidi = UiText::Controller::New();
  bidi->SetText("ABC אבג DEF\nالعربية Hello 123\nHello עברית 123");
  bidi->SetDefaultFontSize(28.0f, UiText::Controller::PIXEL_SIZE);
  bidi->SetMultiLineEnabled(true);
  const Vector2 bidiSize(640.0f, 220.0f);
  bidi->Relayout(bidiSize);
  // The UTC text-abstraction stub intentionally does not implement Unicode
  // bidi. Seed the resolved per-character directions that production HarfBuzz
  // and bidi support provide so the PIXEL interpolation policy is exercised.
  UiText::Controller::Impl& bidiImpl = UiText::Controller::Impl::GetImplementation(*bidi.Get());
  auto&                     logical  = *bidiImpl.mModel->mLogicalModel;
  logical.mCharacterDirections.Resize(logical.mText.Count());
  for(UiText::CharacterIndex character = 0u; character < logical.mText.Count(); ++character)
  {
    const UiText::Character value = logical.mText[character];
    logical.mCharacterDirections[character] = (value >= 0x0590u && value <= 0x08ffu);
  }
  const UiText::ModelInterface* bidiModel = bidi->GetRenderTextModel();
  DALI_TEST_CHECK(bidiModel);

  UiText::TypesetterPtr characterTypesetter = UiText::Typesetter::New(bidiModel);
  const Reveal::Plan    characterSource     = Reveal::BuildCharacterPlan(*bidiModel, 0.25f);
  const Reveal::Plan    characterFinal      = characterTypesetter->CreateFinalRevealPlan(characterSource,
                                                                                          Reveal::Unit::CHARACTER);
  const Reveal::Plan    pixelFinal          = BuildFinalPixelPlan(bidi, 0.25f);
  DALI_TEST_CHECK(pixelFinal.HasPixelTiming());
  DALI_TEST_CHECK(pixelFinal.glyphToUnit == characterFinal.glyphToUnit);
  DALI_TEST_EQUALS(pixelFinal.pixelUnitTiming.size(), pixelFinal.unitStart.size(), TEST_LOCATION);
  DALI_TEST_EQUALS(pixelFinal.fadeDuration, 0.25f, EPSILON, TEST_LOCATION);

  bool foundLeftToRight = false;
  bool foundRightToLeft = false;
  for(uint32_t unit = 0u; unit < pixelFinal.GetUnitCount(); ++unit)
  {
    const Reveal::PixelUnitTiming& timing = pixelFinal.pixelUnitTiming[unit];
    DALI_TEST_CHECK(timing.visualMaximum > timing.visualMinimum);
    DALI_TEST_CHECK(timing.progressionSpan > 0.0f);
    const float left  = Reveal::ResolvePixelStart(pixelFinal, unit, timing.visualMinimum);
    const float right = Reveal::ResolvePixelStart(pixelFinal, unit, timing.visualMaximum);
    if(timing.rightToLeft)
    {
      foundRightToLeft = true;
      DALI_TEST_CHECK(left > right);
    }
    else
    {
      foundLeftToRight = true;
      DALI_TEST_CHECK(left < right);
    }
    if(unit > 0u)
    {
      DALI_TEST_CHECK(pixelFinal.unitStart[unit - 1u] <= pixelFinal.unitStart[unit]);
    }
  }
  DALI_TEST_CHECK(foundLeftToRight && foundRightToLeft);

  // RG16 contains dense intra-cluster timings; the Plan remains one entry per
  // final shaping cluster instead of one entry per physical pixel.
  UiText::TypesetterPtr bidiTypesetter = UiText::Typesetter::New(bidiModel);
  float                 fadeDuration  = 0.0f;
  PixelData             metadata      = bidiTypesetter->RenderTextRevealMetadata(
    bidiSize, UiText::Direction::LEFT_TO_RIGHT, pixelFinal, fadeDuration);
  DALI_TEST_CHECK(metadata);
  DALI_TEST_EQUALS(fadeDuration, pixelFinal.fadeDuration, EPSILON, TEST_LOCATION);
  const Dali::Integration::PixelDataBuffer metadataPixels = Dali::Integration::GetPixelDataBuffer(metadata);
  DALI_TEST_CHECK(metadataPixels.buffer);
  std::vector<bool> encodedStarts(65536u, false);
  uint32_t          distinctStarts = 0u;
  for(uint32_t y = 0u; y < metadata.GetHeight(); ++y)
  {
    const uint8_t* row = metadataPixels.buffer + static_cast<size_t>(y) * metadata.GetStrideBytes();
    for(uint32_t x = 0u; x < metadata.GetWidth(); ++x)
    {
      const uint8_t* pixel = row + static_cast<size_t>(x) * 4u;
      if(pixel[2u] != 0u)
      {
        const uint32_t encoded = static_cast<uint32_t>(pixel[0u]) * 256u + pixel[1u];
        if(!encodedStarts[encoded])
        {
          encodedStarts[encoded] = true;
          ++distinctStarts;
        }
      }
    }
  }
  DALI_TEST_CHECK(distinctStarts > pixelFinal.GetUnitCount());

  UiText::ControllerPtr weights = UiText::Controller::New();
  weights->SetDefaultFontSize(32.0f, UiText::Controller::PIXEL_SIZE);
  UiText::StyledTextBuilder weightBuilder = UiText::StyledTextBuilder::New("I WWWW");
  UiText::FontAttributes   largeAttributes;
  largeAttributes.SetSize(64.0f);
  DALI_TEST_CHECK(weightBuilder.SetSpan(UiText::FontSpan::New(largeAttributes), 2u, 5u));
  weights->SetStyledText(weightBuilder.Build());
  weights->Relayout(Size(400.0f, 100.0f));

  const Reveal::Plan weightedPlan = BuildFinalPixelPlan(weights, 0.25f);
  DALI_TEST_CHECK(weightedPlan.GetUnitCount() > 2u);
  bool               foundDifferentSpan = false;
  for(uint32_t unit = 1u; unit < weightedPlan.GetUnitCount(); ++unit)
  {
    foundDifferentSpan |= std::abs(weightedPlan.pixelUnitTiming[unit].progressionSpan -
                                   weightedPlan.pixelUnitTiming[0u].progressionSpan) > EPSILON;
  }
  DALI_TEST_CHECK(foundDifferentSpan);

  UiText::ControllerPtr noSpace = UiText::Controller::New();
  noSpace->SetText("IWWWW");
  noSpace->SetDefaultFontSize(32.0f, UiText::Controller::PIXEL_SIZE);
  noSpace->Relayout(Size(400.0f, 100.0f));
  const Reveal::Plan noSpacePlan = BuildFinalPixelPlan(noSpace, 0.25f);
  DALI_TEST_EQUALS(weightedPlan.GetUnitCount(), noSpacePlan.GetUnitCount(), TEST_LOCATION);
  DALI_TEST_CHECK(weightedPlan.unitStart[1u] > noSpacePlan.unitStart[1u]);

  // Spatial AUTO is based on final visual progression and text height rather
  // than the CHARACTER cluster count. Explicit ratios remain unchanged.
  const Reveal::Plan spatialAuto = BuildFinalPixelPlan(weights, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  DALI_TEST_CHECK(spatialAuto.fadeDuration > 0.0f);
  DALI_TEST_CHECK(spatialAuto.fadeDuration <= 1.0f);

  // PER_LINE uses final layout lines and the existing fixed stagger ratio,
  // while retaining width-proportional progression inside each sequence.
  UiText::ControllerPtr lineController = UiText::Controller::New();
  lineController->SetText("WWWW WWWW\nI");
  lineController->SetDefaultFontSize(32.0f, UiText::Controller::PIXEL_SIZE);
  lineController->SetMultiLineEnabled(true);
  lineController->Relayout(Size(400.0f, 160.0f));
  const Reveal::Plan perLinePlan = BuildFinalPixelPlan(lineController, 0.25f, Reveal::Sequence::PER_LINE, 0.25f);
  DALI_TEST_CHECK(perLinePlan.HasPixelTiming());
  DALI_TEST_CHECK(perLinePlan.GetUnitCount() > 2u);
  const UiText::ModelInterface* lineModel = lineController->GetRenderTextModel();
  DALI_TEST_CHECK(lineModel && lineModel->GetNumberOfLines() == 2u);
  UiText::TypesetterPtr lineTypesetter = UiText::Typesetter::New(lineModel);
  lineTypesetter->CreateFinalRevealPlan(Reveal::BuildPixelPlan(*lineModel, 0.25f),
                                       Reveal::Unit::PIXEL,
                                       Reveal::Sequence::PER_LINE,
                                       0.25f);
  UiText::ViewModel* lineViewModel = lineTypesetter->GetViewModel();
  DALI_TEST_CHECK(lineViewModel);
  const UiText::LineRun* finalLines = lineViewModel->GetLines();
  float firstLineStart  = 1.0f;
  float secondLineStart = 1.0f;
  for(UiText::GlyphIndex glyph = finalLines[0u].glyphRun.glyphIndex;
      glyph < finalLines[0u].glyphRun.glyphIndex + finalLines[0u].glyphRun.numberOfGlyphs;
      ++glyph)
  {
    const uint32_t unit = perLinePlan.glyphToUnit[glyph];
    if(unit != Reveal::NO_UNIT)
    {
      firstLineStart = std::min(firstLineStart, perLinePlan.unitStart[unit]);
    }
  }
  for(UiText::GlyphIndex glyph = finalLines[1u].glyphRun.glyphIndex;
      glyph < finalLines[1u].glyphRun.glyphIndex + finalLines[1u].glyphRun.numberOfGlyphs;
      ++glyph)
  {
    const uint32_t unit = perLinePlan.glyphToUnit[glyph];
    if(unit != Reveal::NO_UNIT)
    {
      secondLineStart = std::min(secondLineStart, perLinePlan.unitStart[unit]);
    }
  }
  DALI_TEST_EQUALS(firstLineStart, 0.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_CHECK(secondLineStart > firstLineStart);

  // Replacement width never enters the PIXEL schedule and every surviving
  // descriptor has an ordinary text glyph backing it.
  UiText::ControllerPtr replacement = BuildReplacementController("A icon B", {{2u, 4u}}, Size(320.0f, 80.0f));
  const UiText::ModelInterface* replacementModel = replacement->GetRenderTextModel();
  DALI_TEST_CHECK(replacementModel);
  const Reveal::Plan replacementPlan = BuildFinalPixelPlan(replacement, 0.25f);
  DALI_TEST_EQUALS(replacementPlan.GetUnitCount(), 2u, TEST_LOCATION);
  std::vector<bool> replacementBacked(replacementPlan.GetUnitCount(), false);
  for(UiText::GlyphIndex glyph = 0u; glyph < replacementModel->GetNumberOfGlyphs(); ++glyph)
  {
    const uint32_t unit = replacementPlan.glyphToUnit[glyph];
    if(UiText::IsSyntheticReplacementGlyph(replacementModel->GetGlyphs()[glyph]))
    {
      DALI_TEST_EQUALS(unit, Reveal::NO_UNIT, TEST_LOCATION);
    }
    else if(unit != Reveal::NO_UNIT)
    {
      replacementBacked[unit] = true;
    }
  }
  DALI_TEST_CHECK(std::all_of(replacementBacked.begin(), replacementBacked.end(), [](bool value)
  {
    return value;
  }));

  // Negative spacing retains finite spatial timing.
  UiText::ControllerPtr overlap = UiText::Controller::New();
  overlap->SetText("AVATAR ffi office");
  overlap->SetDefaultFontSize(40.0f, UiText::Controller::PIXEL_SIZE);
  overlap->SetCharacterSpacing(-12.0f);
  const Vector2 overlapSize(480.0f, 120.0f);
  overlap->Relayout(overlapSize);
  const Reveal::Plan overlapPlan = BuildFinalPixelPlan(overlap, 0.25f);
  for(uint32_t unit = 0u; unit < overlapPlan.GetUnitCount(); ++unit)
  {
    DALI_TEST_CHECK(std::isfinite(overlapPlan.unitStart[unit]));
    DALI_TEST_CHECK(std::isfinite(overlapPlan.pixelUnitTiming[unit].progressionSpan));
  }
  for(float explicitFade : {0.0f, 0.5f, 1.0f})
  {
    const Reveal::Plan explicitPlan = BuildFinalPixelPlan(overlap, explicitFade);
    DALI_TEST_EQUALS(explicitPlan.fadeDuration, explicitFade, EPSILON, TEST_LOCATION);
    for(const Reveal::PixelUnitTiming& timing : explicitPlan.pixelUnitTiming)
    {
      if(explicitFade < 1.0f)
      {
        DALI_TEST_CHECK(timing.progressionSpan > 0.0f);
      }
      else
      {
        DALI_TEST_EQUALS(timing.progressionSpan, 0.0f, EPSILON, TEST_LOCATION);
      }
    }
  }

  // MaximumLines and END ellipsis schedule only the authoritative final
  // glyphs. A much longer hidden suffix must not change spatial timing.
  struct ElidedPixelResult
  {
    Reveal::Plan plan;
    uint32_t     sourceUnitCount{0u};
  };
  auto BuildElidedResult = [&](const char* hiddenSuffix)
  {
    UiText::ControllerPtr controller = UiText::Controller::New();
    controller->SetText((std::string("Alpha beta\nGamma delta\n") + hiddenSuffix).c_str());
    controller->SetDefaultFontSize(24.0f, UiText::Controller::PIXEL_SIZE);
    controller->SetMultiLineEnabled(true);
    controller->SetTextElideEnabled(true);
    controller->SetEllipsisPosition(UiText::EllipsisPosition::END);
    controller->SetMaximumNumberOfLines(2u);
    controller->Relayout(Size(500.0f, 500.0f));
    const UiText::ModelInterface* model = controller->GetRenderTextModel();
    DALI_TEST_CHECK(model);
    const UiText::FinalElisionResult* finalElision = controller->GetFinalElisionResult();
    DALI_TEST_CHECK(finalElision && finalElision->resolved && finalElision->textElided);
    UiText::TypesetterPtr typesetter = UiText::Typesetter::New(model);
    typesetter->SetFinalElisionResult(finalElision);
    const Reveal::Plan source = Reveal::BuildPixelPlan(*model, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
    ElidedPixelResult result;
    result.sourceUnitCount = source.GetUnitCount();
    result.plan = typesetter->CreateFinalRevealPlan(source,
                                                    Reveal::Unit::PIXEL,
                                                    Reveal::Sequence::PER_LINE,
                                                    0.25f);
    UiText::ViewModel* viewModel = typesetter->GetViewModel();
    DALI_TEST_CHECK(viewModel);
    const UiText::GlyphIndex ellipsis = viewModel->GetEllipsisFinalGlyphIndex();
    DALI_TEST_CHECK(ellipsis < result.plan.glyphToUnit.size());
    DALI_TEST_CHECK(result.plan.glyphToUnit[ellipsis] != Reveal::NO_UNIT);
    return result;
  };
  const ElidedPixelResult shortHidden = BuildElidedResult("hidden");
  const ElidedPixelResult longHidden  = BuildElidedResult(
    "hidden suffix repeated many times hidden suffix repeated many times hidden suffix repeated many times");
  DALI_TEST_CHECK(longHidden.sourceUnitCount > shortHidden.sourceUnitCount);
  DALI_TEST_CHECK(shortHidden.plan.glyphToUnit == longHidden.plan.glyphToUnit);
  DALI_TEST_EQUALS(shortHidden.plan.unitStart.size(), longHidden.plan.unitStart.size(), TEST_LOCATION);
  DALI_TEST_EQUALS(shortHidden.plan.fadeDuration, longHidden.plan.fadeDuration, EPSILON, TEST_LOCATION);
  for(uint32_t unit = 0u; unit < shortHidden.plan.GetUnitCount(); ++unit)
  {
    DALI_TEST_EQUALS(shortHidden.plan.unitStart[unit], longHidden.plan.unitStart[unit], EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(shortHidden.plan.pixelUnitTiming[unit].progressionSpan,
                     longHidden.plan.pixelUnitTiming[unit].progressionSpan,
                     EPSILON,
                     TEST_LOCATION);
  }

  // The canonical full-layout descriptor is reused by height tiles; tile
  // boundaries do not locally renormalize PIXEL timing.
  const Vector2 tileSize(180.0f, 160.0f);
  UiText::ControllerPtr tileController = UiText::Controller::New();
  tileController->SetText("Agjpqy Agjpqy Agjpqy Agjpqy Agjpqy");
  tileController->SetDefaultFontSize(40.0f, UiText::Controller::PIXEL_SIZE);
  tileController->SetMultiLineEnabled(true);
  tileController->Relayout(tileSize);
  const UiText::ModelInterface* tileModel = tileController->GetRenderTextModel();
  DALI_TEST_CHECK(tileModel);
  UiText::TypesetterPtr tileTypesetter = UiText::Typesetter::New(tileModel);
  const Reveal::Plan tilePlan = tileTypesetter->CreateFinalRevealPlan(
    Reveal::BuildPixelPlan(*tileModel, 0.25f),
    Reveal::Unit::PIXEL,
    Reveal::Sequence::PER_LINE,
    0.25f);
  float     tileDuration = 0.0f;
  PixelData fullMetadata = tileTypesetter->RenderTextRevealMetadata(
    tileSize, UiText::Direction::LEFT_TO_RIGHT, tilePlan, tileDuration);
  DALI_TEST_CHECK(fullMetadata);
  const uint32_t boundary = fullMetadata.GetHeight() / 2u;
  PixelData upperMetadata = tileTypesetter->RenderTextRevealMetadata(
    Vector2(tileSize.width, static_cast<float>(boundary)),
    UiText::Direction::LEFT_TO_RIGHT,
    tilePlan,
    tileDuration,
    0u,
    tileSize);
  PixelData lowerMetadata = tileTypesetter->RenderTextRevealMetadata(
    Vector2(tileSize.width, tileSize.height - static_cast<float>(boundary)),
    UiText::Direction::LEFT_TO_RIGHT,
    tilePlan,
    tileDuration,
    boundary,
    tileSize);
  DALI_TEST_CHECK(upperMetadata && lowerMetadata);
  const Dali::Integration::PixelDataBuffer fullTilePixels = Dali::Integration::GetPixelDataBuffer(fullMetadata);
  const Dali::Integration::PixelDataBuffer upperTilePixels = Dali::Integration::GetPixelDataBuffer(upperMetadata);
  const Dali::Integration::PixelDataBuffer lowerTilePixels = Dali::Integration::GetPixelDataBuffer(lowerMetadata);
  const size_t tileRowBytes = static_cast<size_t>(fullMetadata.GetWidth()) * 4u;
  for(uint32_t y = 0u; y < fullMetadata.GetHeight(); ++y)
  {
    const uint8_t* tiledRow = y < boundary
                                ? upperTilePixels.buffer + static_cast<size_t>(y) * upperMetadata.GetStrideBytes()
                                : lowerTilePixels.buffer + static_cast<size_t>(y - boundary) * lowerMetadata.GetStrideBytes();
    DALI_TEST_EQUALS(std::memcmp(fullTilePixels.buffer + static_cast<size_t>(y) * fullMetadata.GetStrideBytes(),
                                 tiledRow,
                                 tileRowBytes),
                     0,
                     TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliTextRevealPixelAtomicFallbackP(void)
{
  UiTestApplication application;

  UiText::ControllerPtr controller = UiText::Controller::New();
  controller->SetText("Alpha beta gamma delta\nSecond line content");
  controller->SetDefaultFontSize(24.0f, UiText::Controller::PIXEL_SIZE);
  controller->SetMultiLineEnabled(true);
  controller->Relayout(Size(220.0f, 180.0f));

  const UiText::ModelInterface* model = controller->GetRenderTextModel();
  DALI_TEST_CHECK(model && model->GetNumberOfLines() > 1u);
  UiText::TypesetterPtr typesetter = UiText::Typesetter::New(model);
  Reveal::Plan          plan       = typesetter->CreateFinalRevealPlan(
    Reveal::BuildCharacterPlan(*model, 0.25f), Reveal::Unit::CHARACTER);
  DALI_TEST_CHECK(plan.GetUnitCount() > 0u);
  DALI_TEST_CHECK(!plan.HasPixelTiming());

  // Force a deterministic late failure after line mapping and spatial
  // geometry have succeeded. The authored NaN reaches final fade validation.
  plan.fadeDurationRatio = std::numeric_limits<float>::quiet_NaN();
  plan.fadeDuration      = 0.125f;
  const std::vector<uint32_t> originalGlyphToUnit = plan.glyphToUnit;
  const std::vector<float>    originalUnitStart   = plan.unitStart;
  const float                 originalFade        = plan.fadeDuration;

  DALI_TEST_CHECK(!Reveal::ApplyPixelSpatialSchedule(plan,
                                                     *model,
                                                     TextAbstraction::FontClient::Get(),
                                                     nullptr,
                                                     std::numeric_limits<UiText::GlyphIndex>::max(),
                                                     Reveal::Sequence::PER_LINE,
                                                     0.25f));
  DALI_TEST_CHECK(plan.glyphToUnit == originalGlyphToUnit);
  DALI_TEST_CHECK(plan.unitStart == originalUnitStart);
  DALI_TEST_EQUALS(plan.fadeDuration, originalFade, EPSILON, TEST_LOCATION);
  DALI_TEST_CHECK(!plan.HasPixelTiming());

  END_TEST;
}

int UtcDaliTextRevealPixelInsertedHyphenP(void)
{
  UiTestApplication application;

  UiText::ControllerPtr controller = UiText::Controller::New();
  controller->SetText("internationalization representation localization extraordinarycharactersequence");
  controller->SetDefaultFontSize(20.0f, UiText::Controller::PIXEL_SIZE);
  controller->SetMultiLineEnabled(true);
  controller->SetLineWrapMode(UiText::LineWrapMode::HYPHENATION);
  const Size layoutSize(180.0f, 400.0f);
  controller->Relayout(layoutSize);

  const UiText::ModelInterface* model = controller->GetRenderTextModel();
  DALI_TEST_CHECK(model);
  DALI_TEST_CHECK(model->GetHyphensCount() > 0u);
  const UiText::Length* hyphenIndices = model->GetHyphenIndices();
  DALI_TEST_CHECK(hyphenIndices);
  for(UiText::Length index = 0u; index < model->GetHyphensCount(); ++index)
  {
    DALI_TEST_CHECK(hyphenIndices[index] > 0u);
    DALI_TEST_CHECK(hyphenIndices[index] <= model->GetNumberOfGlyphs());
  }

  for(Reveal::Sequence sequence : {Reveal::Sequence::WHOLE_TEXT, Reveal::Sequence::PER_LINE})
  {
    UiText::TypesetterPtr typesetter = UiText::Typesetter::New(model);
    const Reveal::Plan    characterPlan = typesetter->CreateFinalRevealPlan(
      Reveal::BuildCharacterPlan(*model, 0.25f), Reveal::Unit::CHARACTER, sequence, 0.25f);
    const Reveal::Plan    plan       = typesetter->CreateFinalRevealPlan(
      Reveal::BuildPixelPlan(*model, 0.25f), Reveal::Unit::PIXEL, sequence, 0.25f);
    DALI_TEST_CHECK(plan.GetUnitCount() > 0u);
    DALI_TEST_EQUALS(plan.GetUnitCount(), characterPlan.GetUnitCount(), TEST_LOCATION);
    DALI_TEST_CHECK(plan.glyphToUnit == characterPlan.glyphToUnit);
    DALI_TEST_CHECK(plan.HasPixelTiming());
    DALI_TEST_EQUALS(plan.pixelUnitTiming.size(), plan.unitStart.size(), TEST_LOCATION);
    for(uint32_t unit = 0u; unit < plan.GetUnitCount(); ++unit)
    {
      const Reveal::PixelUnitTiming& timing = plan.pixelUnitTiming[unit];
      DALI_TEST_CHECK(std::isfinite(plan.unitStart[unit]));
      DALI_TEST_CHECK(std::isfinite(timing.visualMinimum));
      DALI_TEST_CHECK(std::isfinite(timing.visualMaximum));
      DALI_TEST_CHECK(std::isfinite(timing.progressionSpan));
      DALI_TEST_CHECK(timing.visualMaximum > timing.visualMinimum);
    }

    float     fadeDuration = 0.0f;
    PixelData metadata     = typesetter->RenderTextRevealMetadata(
      layoutSize, UiText::Direction::LEFT_TO_RIGHT, plan, fadeDuration);
    DALI_TEST_CHECK(metadata);
    DALI_TEST_EQUALS(fadeDuration, plan.fadeDuration, EPSILON, TEST_LOCATION);
    const Dali::Integration::PixelDataBuffer pixels = Dali::Integration::GetPixelDataBuffer(metadata);
    DALI_TEST_CHECK(pixels.buffer);
    bool foundEncodedStart = false;
    for(uint32_t y = 0u; y < metadata.GetHeight() && !foundEncodedStart; ++y)
    {
      const uint8_t* row = pixels.buffer + static_cast<size_t>(y) * metadata.GetStrideBytes();
      for(uint32_t x = 0u; x < metadata.GetWidth(); ++x)
      {
        if(row[static_cast<size_t>(x) * 4u + 2u] != 0u)
        {
          foundEncodedStart = true;
          break;
        }
      }
    }
    DALI_TEST_CHECK(foundEncodedStart);
  }

  END_TEST;
}

int UtcDaliTextRevealPixelSpatialReferenceP(void)
{
  UiTestApplication application;

  auto BuildFinalPixelPlan = [](UiText::ControllerPtr controller,
                                float                 fadeRatio = UiText::Reveal::AUTO_FADE_DURATION_RATIO,
                                Reveal::Sequence      sequence  = Reveal::Sequence::WHOLE_TEXT)
  {
    const UiText::ModelInterface* model = controller->GetRenderTextModel();
    DALI_TEST_CHECK(model);
    UiText::TypesetterPtr typesetter = UiText::Typesetter::New(model);
    typesetter->SetFinalElisionResult(controller->GetFinalElisionResult());
    return typesetter->CreateFinalRevealPlan(
      Reveal::BuildPixelPlan(*model, fadeRatio), Reveal::Unit::PIXEL, sequence, 0.25f);
  };

  // Replacement geometry is excluded from both progression distance and the
  // text-height reference used by spatial AUTO.
  UiText::ControllerPtr ordinary = BuildReferenceMetricController(
    "A  B", 20.0f, 1.0f, 1.0f, Size(320.0f, 320.0f));
  UiText::ControllerPtr ordinaryReplacement =
    BuildReplacementController("A X B", {{2u, 1u}}, Size(320.0f, 320.0f), false, 24.0f);
  UiText::ControllerPtr tallReplacement =
    BuildReplacementController("A X B", {{2u, 1u}}, Size(320.0f, 320.0f), false, 240.0f);
  const Reveal::Plan ordinaryPlan           = BuildFinalPixelPlan(ordinary);
  const Reveal::Plan ordinaryReplacementPlan = BuildFinalPixelPlan(ordinaryReplacement);
  const Reveal::Plan tallReplacementPlan     = BuildFinalPixelPlan(tallReplacement);
  DALI_TEST_EQUALS(ordinaryPlan.GetUnitCount(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(ordinaryReplacementPlan.glyphToUnit.size() != ordinaryPlan.glyphToUnit.size());
  DALI_TEST_EQUALS(ordinaryReplacementPlan.GetUnitCount(), ordinaryPlan.GetUnitCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(tallReplacementPlan.GetUnitCount(), ordinaryPlan.GetUnitCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(ordinaryReplacementPlan.fadeDuration, ordinaryPlan.fadeDuration, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(tallReplacementPlan.fadeDuration, ordinaryPlan.fadeDuration, EPSILON, TEST_LOCATION);
  for(uint32_t unit = 0u; unit < ordinaryPlan.GetUnitCount(); ++unit)
  {
    DALI_TEST_EQUALS(ordinaryReplacementPlan.unitStart[unit], ordinaryPlan.unitStart[unit], EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(tallReplacementPlan.unitStart[unit], ordinaryPlan.unitStart[unit], EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(ordinaryReplacementPlan.pixelUnitTiming[unit].progressionSpan,
                     ordinaryPlan.pixelUnitTiming[unit].progressionSpan,
                     EPSILON,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(tallReplacementPlan.pixelUnitTiming[unit].progressionSpan,
                     ordinaryPlan.pixelUnitTiming[unit].progressionSpan,
                     EPSILON,
                     TEST_LOCATION);
  }

  UiText::ControllerPtr replacementOnly =
    BuildReplacementController("X", {{0u, 1u}}, Size(320.0f, 320.0f), false, 240.0f);
  const Reveal::Plan replacementOnlyPlan = BuildFinalPixelPlan(replacementOnly);
  DALI_TEST_EQUALS(replacementOnlyPlan.GetUnitCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!replacementOnlyPlan.HasPixelTiming());

  // Leading/trailing whitespace does not alter normalized timing. An
  // internal gap does, while newline itself never becomes a timing unit.
  UiText::ControllerPtr trimmed = BuildReferenceMetricController(
    "Hello World", 24.0f, 1.0f, 1.0f, Size(480.0f, 160.0f));
  UiText::ControllerPtr padded = BuildReferenceMetricController(
    "   Hello World   ", 24.0f, 1.0f, 1.0f, Size(480.0f, 160.0f));
  const Reveal::Plan trimmedPlan = BuildFinalPixelPlan(trimmed, 0.25f);
  const Reveal::Plan paddedPlan  = BuildFinalPixelPlan(padded, 0.25f);
  DALI_TEST_EQUALS(trimmedPlan.GetUnitCount(), paddedPlan.GetUnitCount(), TEST_LOCATION);
  DALI_TEST_CHECK(trimmedPlan.unitStart == paddedPlan.unitStart);
  for(uint32_t unit = 0u; unit < trimmedPlan.GetUnitCount(); ++unit)
  {
    DALI_TEST_EQUALS(trimmedPlan.pixelUnitTiming[unit].progressionSpan,
                     paddedPlan.pixelUnitTiming[unit].progressionSpan,
                     EPSILON,
                     TEST_LOCATION);
  }

  UiText::ControllerPtr multiline = BuildReferenceMetricController(
    "Hello\nWorld", 24.0f, 1.0f, 1.0f, Size(480.0f, 160.0f));
  const UiText::ModelInterface* multilineModel = multiline->GetRenderTextModel();
  const Reveal::Plan            multilinePlan  = BuildFinalPixelPlan(multiline, 0.25f);
  DALI_TEST_CHECK(multilineModel && multilineModel->GetNumberOfLines() == 2u);
  DALI_TEST_EQUALS(multilinePlan.GetUnitCount(), 10u, TEST_LOCATION);
  DALI_TEST_CHECK(std::all_of(multilinePlan.unitStart.begin(), multilinePlan.unitStart.end(), [](float value)
  {
    return std::isfinite(value);
  }));

  // Uniform UI scale changes final geometry but does not turn PIXEL into a
  // physical framebuffer-pixel count. The normalized spatial cadence remains
  // stable within font rasterization tolerance.
  float baselineFade = 0.0f;
  for(float uiScale : {0.8f, 1.0f, 1.2f, 1.4f})
  {
    UiText::ControllerPtr scaled = BuildReferenceMetricController(
      "AVATAR ffi office 안녕하세요", 24.0f, 1.0f, uiScale, Size(800.0f, 192.0f));
    const Reveal::Plan scaledPlan = BuildFinalPixelPlan(scaled);
    DALI_TEST_CHECK(scaledPlan.HasPixelTiming());
    DALI_TEST_CHECK(std::isfinite(scaledPlan.fadeDuration));
    if(uiScale == 0.8f)
    {
      baselineFade = scaledPlan.fadeDuration;
    }
    else
    {
      DALI_TEST_CHECK(std::abs(scaledPlan.fadeDuration - baselineFade) < 0.05f);
    }
  }

  // Line alignment is a raster placement offset. Scheduler geometry and
  // intra-cluster direction stay in the model coordinate system.
  Reveal::Plan alignmentReference;
  bool         haveAlignmentReference = false;
  for(UiText::Alignment alignment : {UiText::Alignment::START,
                                     UiText::Alignment::CENTER,
                                     UiText::Alignment::END})
  {
    UiText::ControllerPtr aligned = BuildReferenceMetricController(
      "Alignment PIXEL", 32.0f, 1.0f, 1.0f, Size(640.0f, 128.0f));
    aligned->SetHorizontalAlignment(alignment);
    aligned->Relayout(Size(640.0f, 128.0f));
    const Reveal::Plan alignedPlan = BuildFinalPixelPlan(aligned, 0.25f);
    DALI_TEST_CHECK(alignedPlan.HasPixelTiming());
    if(!haveAlignmentReference)
    {
      alignmentReference     = alignedPlan;
      haveAlignmentReference = true;
    }
    else
    {
      DALI_TEST_CHECK(alignedPlan.glyphToUnit == alignmentReference.glyphToUnit);
      DALI_TEST_CHECK(alignedPlan.unitStart == alignmentReference.unitStart);
      DALI_TEST_EQUALS(alignedPlan.pixelUnitTiming.size(),
                       alignmentReference.pixelUnitTiming.size(),
                       TEST_LOCATION);
      for(uint32_t unit = 0u; unit < alignedPlan.GetUnitCount(); ++unit)
      {
        DALI_TEST_EQUALS(alignedPlan.pixelUnitTiming[unit].visualMinimum,
                         alignmentReference.pixelUnitTiming[unit].visualMinimum,
                         EPSILON,
                         TEST_LOCATION);
        DALI_TEST_EQUALS(alignedPlan.pixelUnitTiming[unit].visualMaximum,
                         alignmentReference.pixelUnitTiming[unit].visualMaximum,
                         EPSILON,
                         TEST_LOCATION);
      }
    }
  }

  const Reveal::Plan oneLineWholeText = BuildFinalPixelPlan(trimmed, 0.25f, Reveal::Sequence::WHOLE_TEXT);
  const Reveal::Plan oneLinePerLine = BuildFinalPixelPlan(trimmed, 0.25f, Reveal::Sequence::PER_LINE);
  DALI_TEST_CHECK(oneLineWholeText.glyphToUnit == oneLinePerLine.glyphToUnit);
  DALI_TEST_CHECK(oneLineWholeText.unitStart == oneLinePerLine.unitStart);
  DALI_TEST_EQUALS(oneLineWholeText.fadeDuration, oneLinePerLine.fadeDuration, EPSILON, TEST_LOCATION);

  for(float largeSize : {40.0f, 84.0f})
  {
    for(bool heading : {false, true})
    {
      UiText::ControllerPtr mixed = BuildMixedSizeReferenceMetricController(
        largeSize, heading, heading ? Size(180.0f, 640.0f) : Size(640.0f, 256.0f));
      const Reveal::Plan mixedPlan = BuildFinalPixelPlan(mixed);
      DALI_TEST_CHECK(mixedPlan.HasPixelTiming());
      DALI_TEST_CHECK(std::isfinite(mixedPlan.fadeDuration));
      DALI_TEST_CHECK(std::all_of(mixedPlan.pixelUnitTiming.begin(), mixedPlan.pixelUnitTiming.end(), [](const auto& timing)
      {
        return std::isfinite(timing.progressionSpan) && timing.visualMaximum > timing.visualMinimum;
      }));
    }
  }

  END_TEST;
}

int UtcDaliTextRevealImageReplacementPlanP(void)
{
  UiTestApplication application;

  auto build = [](UiText::ControllerPtr       controller,
                  Reveal::Unit                unit,
                  Reveal::Sequence            sequence   = Reveal::Sequence::WHOLE_TEXT,
                  float                       fadeRatio  = 0.25f,
                  TextAbstraction::FontClient fontClient = {})
  {
    const UiText::ModelInterface* model = controller->GetRenderTextModel();
    DALI_TEST_CHECK(model);
    const UiText::ReplacementRenderState& state = controller->GetReplacementRenderState();
    DALI_TEST_CHECK(state.processingModel && state.projection.HasReplacements());

    UiText::TypesetterPtr typesetter = UiText::Typesetter::New(model);
    if(fontClient)
    {
      typesetter->SetFontClient(fontClient);
    }
    typesetter->SetFinalElisionResult(controller->GetFinalElisionResult());
    TextAbstraction::Segmentation segmentation = TextAbstraction::Segmentation::New();
    const Reveal::Plan source = Reveal::BuildPlanWithImageReplacements(
      *model,
      unit,
      fadeRatio,
      segmentation,
      controller->GetReplacementSourceSnapshot(),
      state.placements);
    const Reveal::Plan final = typesetter->CreateFinalRevealPlan(source, unit, sequence, 0.25f);
    Vector<UiText::ReplacementRevealTiming> timings;
    DALI_TEST_CHECK(typesetter->ExtractReplacementRevealTimings(final,
                                                                controller->GetReplacementSourceSnapshot(),
                                                                state.placements,
                                                                timings));
    if(!timings.Empty())
    {
      DALI_TEST_EQUALS(final.imageReplacementUnitMask.size(),
                       static_cast<std::size_t>(final.GetUnitCount()),
                       TEST_LOCATION);
      DALI_TEST_CHECK(std::find(final.imageReplacementUnitMask.begin(),
                                final.imageReplacementUnitMask.end(),
                                1u) != final.imageReplacementUnitMask.end());
    }
    return std::make_pair(final, timings);
  };

  UiText::ControllerPtr mixed = BuildReplacementController(
    "A X B", {{2u, 1u}}, Size(320.0f, 120.0f));

  auto checkPlanParity = [](const Reveal::Plan& left, const Reveal::Plan& right)
  {
    DALI_TEST_CHECK(left.glyphToUnit == right.glyphToUnit);
    DALI_TEST_CHECK(left.imageReplacementUnitMask == right.imageReplacementUnitMask);
    DALI_TEST_EQUALS(left.unitStart.size(), right.unitStart.size(), TEST_LOCATION);
    DALI_TEST_EQUALS(left.pixelUnitTiming.size(), right.pixelUnitTiming.size(), TEST_LOCATION);
    DALI_TEST_EQUALS(left.fadeDuration, right.fadeDuration, EPSILON, TEST_LOCATION);
    for(std::size_t unit = 0u; unit < left.unitStart.size(); ++unit)
    {
      DALI_TEST_EQUALS(left.unitStart[unit], right.unitStart[unit], EPSILON, TEST_LOCATION);
      DALI_TEST_EQUALS(left.pixelUnitTiming[unit].visualMinimum,
                       right.pixelUnitTiming[unit].visualMinimum,
                       EPSILON,
                       TEST_LOCATION);
      DALI_TEST_EQUALS(left.pixelUnitTiming[unit].visualMaximum,
                       right.pixelUnitTiming[unit].visualMaximum,
                       EPSILON,
                       TEST_LOCATION);
      DALI_TEST_EQUALS(left.pixelUnitTiming[unit].progressionSpan,
                       right.pixelUnitTiming[unit].progressionSpan,
                       EPSILON,
                       TEST_LOCATION);
      DALI_TEST_EQUALS(left.pixelUnitTiming[unit].rightToLeft,
                       right.pixelUnitTiming[unit].rightToLeft,
                       TEST_LOCATION);
    }
  };

  TextAbstraction::FontClient workerFontClient = TextAbstraction::FontClient::New();
  for(Reveal::Sequence sequence : {Reveal::Sequence::WHOLE_TEXT, Reveal::Sequence::PER_LINE})
  {
    const auto sync        = build(mixed,
                                   Reveal::Unit::PIXEL,
                                   sequence,
                                   UiText::Reveal::AUTO_FADE_DURATION_RATIO);
    const auto workerOwned = build(mixed,
                                   Reveal::Unit::PIXEL,
                                   sequence,
                                   UiText::Reveal::AUTO_FADE_DURATION_RATIO,
                                   workerFontClient);
    checkPlanParity(sync.first, workerOwned.first);
    DALI_TEST_EQUALS(sync.second.Count(), workerOwned.second.Count(), TEST_LOCATION);
    DALI_TEST_EQUALS(sync.second[0u].start, workerOwned.second[0u].start, EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(sync.second[0u].fadeDuration,
                     workerOwned.second[0u].fadeDuration,
                     EPSILON,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(sync.second[0u].progressionSpan,
                     workerOwned.second[0u].progressionSpan,
                     EPSILON,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(sync.second[0u].rightToLeft,
                     workerOwned.second[0u].rightToLeft,
                     TEST_LOCATION);
  }

  for(Reveal::Unit unit : {Reveal::Unit::CHARACTER, Reveal::Unit::WORD, Reveal::Unit::PIXEL})
  {
    const auto result = build(mixed, unit);
    const Reveal::Plan& plan = result.first;
    const Vector<UiText::ReplacementRevealTiming>& timings = result.second;
    DALI_TEST_EQUALS(plan.GetUnitCount(), 3u, TEST_LOCATION);
    DALI_TEST_EQUALS(timings.Count(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(timings[0u].occurrenceIdentity, 1u, TEST_LOCATION);
    DALI_TEST_CHECK(timings[0u].start > 0.0f && timings[0u].start < 1.0f);
    DALI_TEST_EQUALS(timings[0u].fadeDuration, plan.fadeDuration, EPSILON, TEST_LOCATION);
    if(unit == Reveal::Unit::PIXEL)
    {
      DALI_TEST_CHECK(plan.HasPixelTiming());
      uint32_t replacementUnit = Reveal::NO_UNIT;
      for(uint32_t candidate = 0u; candidate < plan.imageReplacementUnitMask.size(); ++candidate)
      {
        if(plan.imageReplacementUnitMask[candidate] != 0u)
        {
          replacementUnit = candidate;
          break;
        }
      }
      DALI_TEST_CHECK(replacementUnit != Reveal::NO_UNIT);
      DALI_TEST_EQUALS(timings[0u].start,
                       plan.unitStart[replacementUnit] +
                         0.5f * plan.pixelUnitTiming[replacementUnit].progressionSpan,
                       EPSILON,
                       TEST_LOCATION);
      DALI_TEST_EQUALS(timings[0u].progressionSpan,
                       plan.pixelUnitTiming[replacementUnit].progressionSpan,
                       EPSILON,
                       TEST_LOCATION);
      DALI_TEST_EQUALS(timings[0u].rightToLeft,
                       plan.pixelUnitTiming[replacementUnit].rightToLeft,
                       TEST_LOCATION);
    }
    else
    {
      DALI_TEST_EQUALS(timings[0u].progressionSpan, 0.0f, EPSILON, TEST_LOCATION);
      DALI_TEST_CHECK(!timings[0u].rightToLeft);
    }
  }

  UiText::ControllerPtr wideImage = BuildReplacementController(
    "A X B", {{2u, 1u}}, Size(480.0f, 120.0f), false, 24.0f, 96.0f);
  const auto narrowPixel = build(mixed, Reveal::Unit::PIXEL);
  const auto widePixel   = build(wideImage, Reveal::Unit::PIXEL);
  auto replacementUnit = [](const Reveal::Plan& plan)
  {
    const auto found = std::find(plan.imageReplacementUnitMask.begin(), plan.imageReplacementUnitMask.end(), 1u);
    return found == plan.imageReplacementUnitMask.end()
             ? Reveal::NO_UNIT
             : static_cast<uint32_t>(std::distance(plan.imageReplacementUnitMask.begin(), found));
  };
  const uint32_t narrowReplacementUnit = replacementUnit(narrowPixel.first);
  const uint32_t wideReplacementUnit   = replacementUnit(widePixel.first);
  DALI_TEST_CHECK(narrowReplacementUnit != Reveal::NO_UNIT && wideReplacementUnit != Reveal::NO_UNIT);
  DALI_TEST_CHECK(widePixel.first.pixelUnitTiming[wideReplacementUnit].progressionSpan >
                  narrowPixel.first.pixelUnitTiming[narrowReplacementUnit].progressionSpan);

  // The established model API remains text-only for compatibility.
  DALI_TEST_EQUALS(Reveal::BuildCharacterPlan(*mixed->GetRenderTextModel(), 0.25f).GetUnitCount(),
                   2u,
                   TEST_LOCATION);

  UiText::ControllerPtr multiple = BuildReplacementController(
    "A X B X C", {{2u, 1u}, {6u, 1u}}, Size(480.0f, 120.0f));
  const auto multipleResult = build(multiple, Reveal::Unit::WORD);
  DALI_TEST_EQUALS(multipleResult.first.GetUnitCount(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(multipleResult.second.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(multipleResult.second[0u].occurrenceIdentity, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(multipleResult.second[1u].occurrenceIdentity, 2u, TEST_LOCATION);
  DALI_TEST_CHECK(multipleResult.second[0u].start < multipleResult.second[1u].start);

  UiText::ControllerPtr imageOnly = BuildReplacementController(
    "X", {{0u, 1u}}, Size(160.0f, 120.0f), false, 64.0f);
  for(Reveal::Unit unit : {Reveal::Unit::CHARACTER, Reveal::Unit::WORD, Reveal::Unit::PIXEL})
  {
    const auto result = build(imageOnly, unit, Reveal::Sequence::PER_LINE);
    DALI_TEST_EQUALS(result.first.GetUnitCount(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(result.second.Count(), 1u, TEST_LOCATION);
    if(unit == Reveal::Unit::PIXEL)
    {
      DALI_TEST_CHECK(result.first.HasPixelTiming());
      DALI_TEST_EQUALS(result.second[0u].start,
                       result.first.unitStart[0u] +
                         0.5f * result.first.pixelUnitTiming[0u].progressionSpan,
                       EPSILON,
                       TEST_LOCATION);
    }
    else
    {
      DALI_TEST_EQUALS(result.second[0u].start, 0.0f, EPSILON, TEST_LOCATION);
    }
  }
  const auto imageOnlyAuto = build(imageOnly,
                                   Reveal::Unit::PIXEL,
                                   Reveal::Sequence::PER_LINE,
                                   UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  DALI_TEST_CHECK(imageOnlyAuto.first.HasPixelTiming());
  DALI_TEST_EQUALS(imageOnlyAuto.second.Count(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(imageOnlyAuto.second[0u].fadeDuration > 0.0f);
  const auto imageOnlyWorkerOwned = build(imageOnly,
                                          Reveal::Unit::PIXEL,
                                          Reveal::Sequence::PER_LINE,
                                          UiText::Reveal::AUTO_FADE_DURATION_RATIO,
                                          workerFontClient);
  checkPlanParity(imageOnlyAuto.first, imageOnlyWorkerOwned.first);
  DALI_TEST_EQUALS(imageOnlyAuto.second[0u].start,
                   imageOnlyWorkerOwned.second[0u].start,
                   EPSILON,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(imageOnlyAuto.second[0u].fadeDuration,
                   imageOnlyWorkerOwned.second[0u].fadeDuration,
                   EPSILON,
                   TEST_LOCATION);

  UiText::ControllerPtr bidi = BuildReplacementController(
    u8"אבג X ABC", {{4u, 1u}}, Size(420.0f, 120.0f));
  const auto bidiResult = build(bidi, Reveal::Unit::WORD);
  DALI_TEST_EQUALS(bidiResult.first.GetUnitCount(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(bidiResult.second.Count(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(bidiResult.second[0u].start > 0.0f && bidiResult.second[0u].start < 1.0f);
  UiText::ControllerPtr inverseBidi = BuildReplacementController(
    u8"ABC X אבג", {{4u, 1u}}, Size(420.0f, 120.0f));
  for(Reveal::Unit unit : {Reveal::Unit::CHARACTER, Reveal::Unit::WORD, Reveal::Unit::PIXEL})
  {
    const auto inverseResult = build(inverseBidi, unit);
    DALI_TEST_EQUALS(inverseResult.second.Count(), 1u, TEST_LOCATION);
    DALI_TEST_CHECK(inverseResult.second[0u].start > 0.0f && inverseResult.second[0u].start < 1.0f);
  }
  UiText::ControllerPtr rtlReplacement = BuildReplacementController(
    u8"א ב ג", {{2u, 1u}}, Size(420.0f, 120.0f));
  UiText::ModelPtr rtlProcessingModel = rtlReplacement->GetReplacementRenderState().processingModel;
  DALI_TEST_CHECK(rtlProcessingModel);
  auto& rtlDirections = rtlProcessingModel->mLogicalModel->mCharacterDirections;
  rtlDirections.Resize(rtlProcessingModel->mLogicalModel->mText.Count());
  std::fill(rtlDirections.Begin(), rtlDirections.End(), true);
  const auto rtlPixel = build(rtlReplacement, Reveal::Unit::PIXEL);
  DALI_TEST_EQUALS(rtlPixel.second.Count(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(rtlPixel.second[0u].progressionSpan > 0.0f);
  DALI_TEST_CHECK(rtlPixel.second[0u].rightToLeft);

  for(const auto& wordCase : {
        std::make_tuple("helloXworld", 5u, 3u),
        std::make_tuple("hello X world", 6u, 3u),
        std::make_tuple("text,X", 5u, 2u)})
  {
    UiText::ControllerPtr word = BuildReplacementController(
      std::get<0>(wordCase), {{std::get<1>(wordCase), 1u}}, Size(420.0f, 120.0f));
    const auto result = build(word, Reveal::Unit::WORD);
    DALI_TEST_EQUALS(result.first.GetUnitCount(), std::get<2>(wordCase), TEST_LOCATION);
    DALI_TEST_EQUALS(result.second.Count(), 1u, TEST_LOCATION);
  }

  UiText::AsyncTextParameters asyncParameters;
  asyncParameters.text                          = "A X B";
  asyncParameters.textWidth                     = 320.0f;
  asyncParameters.textHeight                    = 120.0f;
  asyncParameters.fontSize                      = 20.0f;
  asyncParameters.maxTextureSize                = 4096;
  asyncParameters.ellipsis                      = false;
  asyncParameters.isTextRevealEnabled           = true;
  asyncParameters.textRevealUnit                = Reveal::Unit::PIXEL;
  asyncParameters.textRevealSequence            = Reveal::Sequence::PER_LINE;
  asyncParameters.textRevealFadeDurationRatio   = 0.25f;
  asyncParameters.replacementSourceSnapshot     = mixed->GetReplacementSourceSnapshot();
  asyncParameters.replacementLayoutGeneration   = 17u;
  UiText::AsyncTextLoader           asyncLoader = UiText::AsyncTextLoader::New();
  const UiText::AsyncTextRenderInfo asyncInfo =
    asyncLoader.RenderText(asyncParameters, false, Size::ZERO);
  DALI_TEST_CHECK(asyncInfo.isTextRevealEnabled);
  DALI_TEST_EQUALS(asyncInfo.replacementPlacements.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncInfo.replacementRevealTimings.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncInfo.replacementRevealTimings[0u].occurrenceIdentity, 1u, TEST_LOCATION);

  float autoLineReplacementStart        = 0.0f;
  float autoLineReplacementFadeDuration = 0.0f;
  float autoLineReplacementSpan         = 0.0f;
  for(Reveal::Sequence sequence : {Reveal::Sequence::WHOLE_TEXT, Reveal::Sequence::PER_LINE})
  {
    asyncParameters.textRevealSequence           = sequence;
    asyncParameters.textRevealFadeDurationRatio  = UiText::Reveal::AUTO_FADE_DURATION_RATIO;
    UiText::AsyncTextLoader           autoLoader = UiText::AsyncTextLoader::New();
    const UiText::AsyncTextRenderInfo autoInfo =
      autoLoader.RenderText(asyncParameters, false, Size::ZERO);
    const UiText::ReplacementRenderState* asyncState =
      UiText::GetImplementation(autoLoader).GetReplacementRenderState();
    DALI_TEST_CHECK(asyncState && asyncState->processingModel);
    UiText::TypesetterPtr parityTypesetter = UiText::Typesetter::New(asyncState->processingModel.Get());
    parityTypesetter->SetFinalElisionResult(&asyncState->finalElision);
    TextAbstraction::Segmentation paritySegmentation = TextAbstraction::Segmentation::New();
    const Reveal::Plan            paritySource       = Reveal::BuildPlanWithImageReplacements(
      *asyncState->processingModel,
      Reveal::Unit::PIXEL,
      UiText::Reveal::AUTO_FADE_DURATION_RATIO,
      paritySegmentation,
      asyncParameters.replacementSourceSnapshot,
      asyncState->placements);
    const Reveal::Plan                      parityPlan = parityTypesetter->CreateFinalRevealPlan(paritySource,
                                                                                                 Reveal::Unit::PIXEL,
                                                                                                 sequence,
                                                                                                 0.25f);
    Vector<UiText::ReplacementRevealTiming> parityTimings;
    DALI_TEST_CHECK(parityTypesetter->ExtractReplacementRevealTimings(
      parityPlan,
      asyncParameters.replacementSourceSnapshot,
      asyncState->placements,
      parityTimings));
    DALI_TEST_CHECK(autoInfo.isTextRevealEnabled);
    DALI_TEST_EQUALS(autoInfo.replacementRevealTimings.Count(), parityTimings.Count(), TEST_LOCATION);
    DALI_TEST_EQUALS(autoInfo.replacementRevealTimings[0u].start,
                     parityTimings[0u].start,
                     EPSILON,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(autoInfo.replacementRevealTimings[0u].fadeDuration,
                     parityTimings[0u].fadeDuration,
                     EPSILON,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(autoInfo.replacementRevealTimings[0u].progressionSpan,
                     parityTimings[0u].progressionSpan,
                     EPSILON,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(autoInfo.replacementRevealTimings[0u].rightToLeft,
                     parityTimings[0u].rightToLeft,
                     TEST_LOCATION);
    if(sequence == Reveal::Sequence::PER_LINE)
    {
      autoLineReplacementStart        = autoInfo.replacementRevealTimings[0u].start;
      autoLineReplacementFadeDuration = autoInfo.replacementRevealTimings[0u].fadeDuration;
      autoLineReplacementSpan         = autoInfo.replacementRevealTimings[0u].progressionSpan;
    }
  }

  asyncParameters.textRevealSequence                  = Reveal::Sequence::PER_LINE;
  asyncParameters.textRevealFadeDurationRatio         = UiText::Reveal::AUTO_FADE_DURATION_RATIO;
  asyncParameters.renderScale                         = 1.5f;
  asyncParameters.maxTextureSize                      = 32;
  UiText::AsyncTextLoader           scaledLoader      = UiText::AsyncTextLoader::New();
  bool                              cachedNaturalSize = false;
  const Size                        naturalSize       = scaledLoader.SetupRenderScale(asyncParameters, cachedNaturalSize);
  const UiText::AsyncTextRenderInfo scaledInfo =
    scaledLoader.RenderText(asyncParameters, cachedNaturalSize, naturalSize);
  DALI_TEST_CHECK(scaledInfo.isTextRevealEnabled);
  DALI_TEST_CHECK(scaledInfo.revealMetadataTiles.size() > 1u);
  DALI_TEST_EQUALS(scaledInfo.replacementPlacements.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(scaledInfo.replacementRevealTimings.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(scaledInfo.replacementRevealTimings[0u].start,
                   autoLineReplacementStart,
                   EPSILON,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(scaledInfo.replacementRevealTimings[0u].fadeDuration,
                   autoLineReplacementFadeDuration,
                   EPSILON,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(scaledInfo.replacementRevealTimings[0u].progressionSpan,
                   autoLineReplacementSpan,
                   EPSILON,
                   TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextRevealImageReplacementPixelFragmentP(void)
{
  UiTestApplication application;

  Texture      imageTexture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 4u, 4u);
  Ui::ImageUrl imageUrl     = Ui::ImageUrl::New(imageTexture, true);
  UiText::StyledTextBuilder builder = UiText::StyledTextBuilder::New("A X B");
  DALI_TEST_CHECK(builder.SetSpan(
    UiText::ImageSpan::New(UiText::ImageAttributes(imageUrl.GetUrl(), Vector2(120.0f, 40.0f))),
    2u,
    3u));

  Ui::Label label = Ui::Label::New();
  label.SetStyledText(builder.Build());
  label.SetProperty(Actor::Property::SIZE, Vector2(360.0f, 100.0f));
  UiText::Reveal reveal;
  reveal.SetUnit(UiText::Reveal::Unit::PIXEL);
  reveal.SetSequence(UiText::Reveal::Sequence::WHOLE_TEXT);
  reveal.SetFadeDurationRatio(0.0f);
  label.SetTextReveal(reveal);
  label.SetTextRevealProgress(0.0f);
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                   1u,
                   TEST_LOCATION);
  DALI_TEST_CHECK(Dali::Ui::Internal::IsInlineReplacementRevealPixelSpatial(label));
  Renderer renderer = FindInlineReplacementRevealRenderer(label);
  DALI_TEST_CHECK(renderer);
  Ui::Integration::Visual::Base replacementVisual = FindInlineReplacementVisual(label);
  DALI_TEST_CHECK(replacementVisual);
  DALI_TEST_CHECK(Ui::GetImplementation(replacementVisual).IsUsingCustomShader());
  const Property::Value shaderProgramValue = renderer.GetShader().GetProperty(Shader::Property::PROGRAM);
  const Property::Map*  shaderProgram      = shaderProgramValue.GetMap();
  DALI_TEST_CHECK(shaderProgram);
  Dali::String fragmentSource;
  Dali::String shaderHints;
  DALI_TEST_CHECK(shaderProgram->Find("fragment") &&
                  shaderProgram->Find("fragment")->Get(fragmentSource));
  DALI_TEST_CHECK(shaderProgram->Find("hints") &&
                  shaderProgram->Find("hints")->Get(shaderHints));
  const std::string fragmentSourceString(fragmentSource.CStr());
  const std::string shaderHintsString(shaderHints.CStr());
  DALI_TEST_CHECK(fragmentSourceString.find("uInlineReplacementRevealProgress") != std::string::npos);
  DALI_TEST_CHECK(fragmentSourceString.find("vTexCoord.x") != std::string::npos);
  DALI_TEST_CHECK(fragmentSourceString.find("pixelArea") == std::string::npos);
  DALI_TEST_CHECK(shaderHintsString.find("OUTPUT_IS_TRANSPARENT") != std::string::npos);
  const Property::Index progressIndex = renderer.GetPropertyIndex("uInlineReplacementRevealProgress");
  const Property::Index timingIndex   = renderer.GetPropertyIndex("uInlineReplacementRevealTiming");
  DALI_TEST_CHECK(progressIndex != Property::INVALID_INDEX);
  DALI_TEST_CHECK(timingIndex != Property::INVALID_INDEX);
  const Vector3 timing = renderer.GetProperty<Vector3>(timingIndex);
  DALI_TEST_CHECK(timing.y > 0.0f);
  UiText::ReplacementRevealTiming revealTiming;
  DALI_TEST_CHECK(Dali::Ui::Internal::GetInlineReplacementRevealTiming(label, revealTiming));
  DALI_TEST_EQUALS(timing.y, revealTiming.progressionSpan / 3.0f, EPSILON, TEST_LOCATION);

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.EnableTextureCallTrace(true);
  for(float progress : {0.0f, 0.25f, 0.5f, 0.75f, 1.0f})
  {
    gl.ResetTextureCallStack();
    label.SetTextRevealProgress(progress);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(progressIndex), progress, EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexImage2D"), 0, TEST_LOCATION);
    DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexSubImage2D"), 0, TEST_LOCATION);
  }

  reveal.SetFadeDurationRatio(0.25f);
  label.SetTextReveal(reveal);
  application.SendNotification();
  application.Render();
  const Vector3 fadedTiming = renderer.GetProperty<Vector3>(timingIndex);
  DALI_TEST_CHECK(fadedTiming.z > 0.0f);
  DALI_TEST_CHECK(Dali::Ui::Internal::IsInlineReplacementRevealPixelSpatial(label));

  label.SetTextCutoutEnabled(true);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(!Dali::Ui::Internal::IsInlineReplacementRevealPixelSpatial(label));
  replacementVisual = FindInlineReplacementVisual(label);
  DALI_TEST_CHECK(replacementVisual);
  DALI_TEST_CHECK(!Ui::GetImplementation(replacementVisual).IsUsingCustomShader());
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                   0u,
                   TEST_LOCATION);

  label.SetTextCutoutEnabled(false);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Dali::Ui::Internal::IsInlineReplacementRevealPixelSpatial(label));
  replacementVisual = FindInlineReplacementVisual(label);
  DALI_TEST_CHECK(replacementVisual);
  DALI_TEST_CHECK(Ui::GetImplementation(replacementVisual).IsUsingCustomShader());

  reveal.SetUnit(UiText::Reveal::Unit::WORD);
  label.SetTextReveal(reveal);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(!Dali::Ui::Internal::IsInlineReplacementRevealPixelSpatial(label));
  replacementVisual = FindInlineReplacementVisual(label);
  DALI_TEST_CHECK(replacementVisual);
  DALI_TEST_CHECK(!Ui::GetImplementation(replacementVisual).IsUsingCustomShader());
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                   1u,
                   TEST_LOCATION);

  label.SetTextReveal(UiText::Reveal::None());
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                   0u,
                   TEST_LOCATION);

  UiText::StyledTextBuilder matchingBuilder = UiText::StyledTextBuilder::New("X");
  DALI_TEST_CHECK(matchingBuilder.SetSpan(
    UiText::ImageSpan::New(UiText::ImageAttributes(imageUrl.GetUrl(), Vector2(40.0f, 40.0f))),
    0u,
    1u));
  Ui::Label matchingLabel = Ui::Label::New();
  matchingLabel.SetStyledText(matchingBuilder.Build());
  matchingLabel.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  reveal.SetUnit(UiText::Reveal::Unit::PIXEL);
  reveal.SetFadeDurationRatio(0.0f);
  matchingLabel.SetTextReveal(reveal);
  matchingLabel.SetTextRevealProgress(0.5f);
  application.GetScene().Add(matchingLabel);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  Renderer matchingRenderer = FindInlineReplacementRevealRenderer(matchingLabel);
  DALI_TEST_CHECK(matchingRenderer);
  const Property::Index matchingTimingIndex =
    matchingRenderer.GetPropertyIndex("uInlineReplacementRevealTiming");
  DALI_TEST_CHECK(matchingTimingIndex != Property::INVALID_INDEX);
  UiText::ReplacementRevealTiming matchingRevealTiming;
  DALI_TEST_CHECK(Dali::Ui::Internal::GetInlineReplacementRevealTiming(matchingLabel,
                                                                       matchingRevealTiming));
  const Vector3 matchingTiming = matchingRenderer.GetProperty<Vector3>(matchingTimingIndex);
  DALI_TEST_EQUALS(matchingTiming.y,
                   matchingRevealTiming.progressionSpan,
                   EPSILON,
                   TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextRevealImageReplacementVisibilityP(void)
{
  UiTestApplication application;

  auto buildFinal = [](UiText::ControllerPtr                    controller,
                       Reveal::Unit                             unit,
                       Reveal::Sequence                         sequence,
                       Vector<UiText::ReplacementRevealTiming>& timings)
  {
    const UiText::ModelInterface* model = controller->GetRenderTextModel();
    DALI_TEST_CHECK(model);
    const UiText::ReplacementRenderState& state = controller->GetReplacementRenderState();
    DALI_TEST_CHECK(state.processingModel && state.projection.HasReplacements());
    UiText::TypesetterPtr typesetter = UiText::Typesetter::New(model);
    typesetter->SetFinalElisionResult(controller->GetFinalElisionResult());
    TextAbstraction::Segmentation segmentation = TextAbstraction::Segmentation::New();
    const Reveal::Plan source = Reveal::BuildPlanWithImageReplacements(
      *model,
      unit,
      UiText::Reveal::AUTO_FADE_DURATION_RATIO,
      segmentation,
      controller->GetReplacementSourceSnapshot(),
      state.placements);
    const Reveal::Plan final = typesetter->CreateFinalRevealPlan(source, unit, sequence, 0.25f);
    DALI_TEST_CHECK(typesetter->ExtractReplacementRevealTimings(final,
                                                                controller->GetReplacementSourceSnapshot(),
                                                                state.placements,
                                                                timings));
    return final;
  };

  // Whitespace-only and empty lines remain inactive, while an ImageSpan-only
  // line participates in PER_LINE sequencing as one atomic item.
  UiText::ControllerPtr imageLine = BuildReplacementController(
    "top\n   \nX\n\nbottom", {{8u, 1u}}, Size(320.0f, 320.0f));
  imageLine->SetMultiLineEnabled(true);
  imageLine->Relayout(Size(320.0f, 320.0f));
  DALI_TEST_EQUALS(imageLine->GetReplacementRenderState().placements.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(imageLine->GetReplacementRenderState().placements[0u].lineIndex, 2u, TEST_LOCATION);
  for(Reveal::Unit unit : {Reveal::Unit::CHARACTER, Reveal::Unit::WORD, Reveal::Unit::PIXEL})
  {
    Vector<UiText::ReplacementRevealTiming> timings;
    const Reveal::Plan plan = buildFinal(imageLine, unit, Reveal::Sequence::PER_LINE, timings);
    DALI_TEST_EQUALS(timings.Count(), 1u, TEST_LOCATION);
    DALI_TEST_CHECK(timings[0u].start > 0.0f && timings[0u].start < 1.0f);
    DALI_TEST_CHECK(std::count(plan.imageReplacementUnitMask.begin(), plan.imageReplacementUnitMask.end(), 1u) == 1);
    if(unit == Reveal::Unit::PIXEL)
    {
      DALI_TEST_CHECK(plan.HasPixelTiming());
      DALI_TEST_CHECK(timings[0u].fadeDuration > 0.0f);
    }
  }

  auto buildHiddenSuffix = [&](const char* text, const std::vector<UiText::CharacterRun>& ranges)
  {
    UiText::ControllerPtr controller = BuildReplacementController(text, ranges, Size(500.0f, 500.0f), true);
    controller->SetMultiLineEnabled(true);
    controller->SetMaximumNumberOfLines(2);
    controller->SetEllipsisPosition(UiText::EllipsisPosition::END);
    controller->Relayout(Size(500.0f, 500.0f));
    const UiText::ReplacementRenderState& state = controller->GetReplacementRenderState();
    DALI_TEST_CHECK(state.finalElision.textElided);
    for(const UiText::ReplacementPlacement& placement : state.placements)
    {
      DALI_TEST_CHECK(!placement.visible);
      DALI_TEST_CHECK(placement.elided);
    }
    Vector<UiText::ReplacementRevealTiming> timings;
    Reveal::Plan plan = buildFinal(controller, Reveal::Unit::WORD, Reveal::Sequence::PER_LINE, timings);
    DALI_TEST_EQUALS(timings.Count(), 0u, TEST_LOCATION);
    DALI_TEST_CHECK(plan.imageReplacementUnitMask.empty());
    return plan;
  };

  const Reveal::Plan oneHidden = buildHiddenSuffix(
    "Alpha beta\nGamma delta\nX", {{23u, 1u}});
  const Reveal::Plan threeHidden = buildHiddenSuffix(
    "Alpha beta\nGamma delta\nX X X", {{23u, 1u}, {25u, 1u}, {27u, 1u}});
  DALI_TEST_CHECK(oneHidden.glyphToUnit == threeHidden.glyphToUnit);
  DALI_TEST_CHECK(oneHidden.unitStart == threeHidden.unitStart);
  DALI_TEST_EQUALS(oneHidden.fadeDuration, threeHidden.fadeDuration, EPSILON, TEST_LOCATION);

  // Find a font-independent END-ellipsis width where the image survives and
  // the generated ellipsis follows it. The two remain independent WORD items.
  UiText::ControllerPtr endEllipsis = BuildReplacementController(
    "A X trailing words force a stable end ellipsis", {{2u, 1u}}, Size(320.0f, 80.0f), true);
  bool verifiedVisibleImageEllipsis = false;
  for(float width = 48.0f; width <= 180.0f && !verifiedVisibleImageEllipsis; width += 2.0f)
  {
    endEllipsis->Relayout(Size(width, 80.0f));
    const UiText::ReplacementRenderState& state = endEllipsis->GetReplacementRenderState();
    if(state.placements.Empty() || !state.placements[0u].visible || !state.finalElision.textElided)
    {
      continue;
    }
    Vector<UiText::ReplacementRevealTiming> timings;
    const Reveal::Plan plan = buildFinal(endEllipsis,
                                         Reveal::Unit::WORD,
                                         Reveal::Sequence::WHOLE_TEXT,
                                         timings);
    UiText::TypesetterPtr typesetter = UiText::Typesetter::New(endEllipsis->GetRenderTextModel());
    typesetter->SetFinalElisionResult(endEllipsis->GetFinalElisionResult());
    TextAbstraction::Segmentation segmentation = TextAbstraction::Segmentation::New();
    const Reveal::Plan source = Reveal::BuildPlanWithImageReplacements(
      *endEllipsis->GetRenderTextModel(),
      Reveal::Unit::WORD,
      0.25f,
      segmentation,
      endEllipsis->GetReplacementSourceSnapshot(),
      state.placements);
    const Reveal::Plan explicitPlan = typesetter->CreateFinalRevealPlan(source,
                                                                        Reveal::Unit::WORD,
                                                                        Reveal::Sequence::WHOLE_TEXT,
                                                                        0.25f);
    UiText::ViewModel* viewModel = typesetter->GetViewModel();
    const UiText::GlyphIndex ellipsisGlyph = viewModel->GetEllipsisFinalGlyphIndex();
    if(ellipsisGlyph >= explicitPlan.glyphToUnit.size())
    {
      continue;
    }
    const uint32_t ellipsisUnit = explicitPlan.glyphToUnit[ellipsisGlyph];
    if(ellipsisUnit >= explicitPlan.imageReplacementUnitMask.size())
    {
      continue;
    }
    DALI_TEST_EQUALS(timings.Count(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(plan.imageReplacementUnitMask[ellipsisUnit], 0u, TEST_LOCATION);
    DALI_TEST_EQUALS(explicitPlan.imageReplacementUnitMask[ellipsisUnit], 0u, TEST_LOCATION);
    verifiedVisibleImageEllipsis = true;
  }
  DALI_TEST_CHECK(verifiedVisibleImageEllipsis);

  END_TEST;
}

int UtcDaliTextRevealImageReplacementEndEllipsisPerLineSequenceP(void)
{
  UiTestApplication application;

  Texture      imageTexture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 4u, 4u);
  Ui::ImageUrl imageUrl     = Ui::ImageUrl::New(imageTexture, true);

  struct CheckedPlan
  {
    UiText::ReplacementRevealTiming replacementTiming;
  };

  auto checkPlan = [](const UiText::ModelInterface&               model,
                      const UiText::FinalElisionResult*            finalElision,
                      const UiText::ReplacementSourceSnapshot&     sourceSnapshot,
                      const Vector<UiText::ReplacementPlacement>& placements,
                      Reveal::Unit                                unit,
                      bool                                        expectEllipsis,
                      uint32_t                                    expectedImageLine,
                      uint32_t                                    expectedEllipsisLine,
                      bool                                        requireSeparateWordUnits)
  {
    DALI_TEST_EQUALS(placements.Count(), 1u, TEST_LOCATION);
    DALI_TEST_CHECK(placements[0u].visible && !placements[0u].elided);
    DALI_TEST_EQUALS(placements[0u].lineIndex, expectedImageLine, TEST_LOCATION);

    UiText::TypesetterPtr typesetter = UiText::Typesetter::New(&model);
    typesetter->SetFinalElisionResult(finalElision);
    TextAbstraction::Segmentation segmentation = TextAbstraction::Segmentation::New();
    const Reveal::Plan source = Reveal::BuildPlanWithImageReplacements(model,
                                                                        unit,
                                                                        0.25f,
                                                                        segmentation,
                                                                        sourceSnapshot,
                                                                        placements);
    const Reveal::Plan wholeTextPlan = typesetter->CreateFinalRevealPlan(source,
                                                                    unit,
                                                                    Reveal::Sequence::WHOLE_TEXT,
                                                                    0.25f);
    const Reveal::Plan perLinePlan = typesetter->CreateFinalRevealPlan(source,
                                                                    unit,
                                                                    Reveal::Sequence::PER_LINE,
                                                                    0.25f);

    UiText::ViewModel* viewModel = typesetter->GetViewModel();
    DALI_TEST_CHECK(viewModel);
    const uint32_t glyphCount = viewModel->GetNumberOfGlyphs();
    const uint32_t lineCount  = viewModel->GetNumberOfLines();
    DALI_TEST_CHECK(glyphCount > 0u && lineCount >= 2u);
    DALI_TEST_EQUALS(perLinePlan.glyphToUnit.size(), static_cast<std::size_t>(glyphCount), TEST_LOCATION);
    DALI_TEST_EQUALS(perLinePlan.unitStart.size(),
                     static_cast<std::size_t>(perLinePlan.GetUnitCount()),
                     TEST_LOCATION);
    DALI_TEST_CHECK(perLinePlan.unitStart != wholeTextPlan.unitStart);
    DALI_TEST_CHECK(std::isfinite(perLinePlan.fadeDuration) && perLinePlan.fadeDuration > 0.0f);

    // Call the scheduler explicitly so a valid PER_LINE request can never
    // regress to CreateFinalRevealPlan's safe WHOLE_TEXT-like fallback without
    // failing here.
    Reveal::Plan scheduled = Reveal::ProjectToFinalGlyphs(source,
                                                           glyphCount,
                                                           viewModel->GetFinalToSourceGlyphIndices(),
                                                           viewModel->GetEllipsisFinalGlyphIndex(),
                                                           unit);
    const bool schedulerSucceeded = unit == Reveal::Unit::PIXEL
                                      ? Reveal::ApplyPixelSpatialSchedule(scheduled,
                                                                          *viewModel,
                                                                          {},
                                                                          viewModel->GetFinalToSourceGlyphIndices(),
                                                                          viewModel->GetEllipsisFinalGlyphIndex(),
                                                                          Reveal::Sequence::PER_LINE,
                                                                          0.25f)
                                      : Reveal::ApplyPerLineSequenceSchedule(scheduled,
                                                                          viewModel->GetLines(),
                                                                          lineCount,
                                                                          0.25f);
    DALI_TEST_CHECK(schedulerSucceeded);
    DALI_TEST_CHECK(scheduled.glyphToUnit == perLinePlan.glyphToUnit);
    DALI_TEST_CHECK(scheduled.unitStart == perLinePlan.unitStart);
    DALI_TEST_CHECK(scheduled.imageReplacementUnitMask == perLinePlan.imageReplacementUnitMask);

    const UiText::LineRun* lines = viewModel->GetLines();
    DALI_TEST_CHECK(lines);
    std::vector<uint32_t> glyphToLine(glyphCount, Reveal::NO_UNIT);
    auto mapRun = [&](uint32_t lineIndex, const UiText::GlyphRun& run)
    {
      const uint32_t begin = run.glyphIndex;
      const uint32_t count = run.numberOfGlyphs;
      DALI_TEST_CHECK(begin <= glyphCount && count <= glyphCount - begin);
      for(uint32_t glyph = begin; glyph < begin + count; ++glyph)
      {
        DALI_TEST_CHECK(glyphToLine[glyph] == Reveal::NO_UNIT || glyphToLine[glyph] == lineIndex);
        glyphToLine[glyph] = lineIndex;
      }
    };
    for(uint32_t line = 0u; line < lineCount; ++line)
    {
      mapRun(line, lines[line].glyphRun);
      if(lines[line].isSplitToTwoHalves)
      {
        mapRun(line, lines[line].glyphRunSecondHalf);
      }
    }
    for(uint32_t glyph = 0u; glyph < glyphCount; ++glyph)
    {
      if(perLinePlan.glyphToUnit[glyph] != Reveal::NO_UNIT)
      {
        DALI_TEST_CHECK(glyphToLine[glyph] != Reveal::NO_UNIT);
      }
    }

    std::vector<float> lineStart(lineCount, std::numeric_limits<float>::max());
    for(uint32_t glyph = 0u; glyph < glyphCount; ++glyph)
    {
      const uint32_t revealUnit = perLinePlan.glyphToUnit[glyph];
      const uint32_t line       = glyphToLine[glyph];
      if(revealUnit != Reveal::NO_UNIT && line != Reveal::NO_UNIT)
      {
        lineStart[line] = std::min(lineStart[line], perLinePlan.unitStart[revealUnit]);
      }
    }
    uint32_t activeLineCount = 0u;
    float    previousStart   = -1.0f;
    for(float start : lineStart)
    {
      if(start == std::numeric_limits<float>::max())
      {
        continue;
      }
      DALI_TEST_CHECK(std::isfinite(start) && start > previousStart);
      previousStart = start;
      ++activeLineCount;
    }
    DALI_TEST_CHECK(activeLineCount >= 2u);

    const UiText::GlyphInfo* finalGlyphs        = viewModel->GetGlyphs();
    const UiText::GlyphIndex* finalToSourceGlyph = viewModel->GetFinalToSourceGlyphIndices();
    DALI_TEST_CHECK(finalGlyphs);
    UiText::GlyphIndex imageFinalGlyph = UiText::FinalElisionResult::INVALID_GLYPH_INDEX;
    for(UiText::GlyphIndex glyph = 0u; glyph < glyphCount; ++glyph)
    {
      const UiText::GlyphIndex sourceGlyph = finalToSourceGlyph ? finalToSourceGlyph[glyph] : glyph;
      if(UiText::IsSyntheticReplacementGlyph(finalGlyphs[glyph]) &&
         sourceGlyph == placements[0u].syntheticGlyphIndex)
      {
        imageFinalGlyph = glyph;
        break;
      }
    }
    DALI_TEST_CHECK(imageFinalGlyph < glyphCount);
    DALI_TEST_EQUALS(glyphToLine[imageFinalGlyph], expectedImageLine, TEST_LOCATION);
    const uint32_t imageUnit = perLinePlan.glyphToUnit[imageFinalGlyph];
    DALI_TEST_CHECK(imageUnit < perLinePlan.GetUnitCount());
    DALI_TEST_EQUALS(perLinePlan.imageReplacementUnitMask[imageUnit], 1u, TEST_LOCATION);

    Vector<UiText::ReplacementRevealTiming> timings;
    DALI_TEST_CHECK(typesetter->ExtractReplacementRevealTimings(perLinePlan,
                                                                 sourceSnapshot,
                                                                 placements,
                                                                 timings));
    DALI_TEST_EQUALS(timings.Count(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(timings[0u].occurrenceIdentity,
                     placements[0u].occurrenceIdentity,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(timings[0u].fadeDuration, perLinePlan.fadeDuration, EPSILON, TEST_LOCATION);

    if(expectEllipsis)
    {
      DALI_TEST_CHECK(finalElision && finalElision->HasAuthoritativeLayout() && finalElision->applied);
      const UiText::GlyphIndex ellipsisGlyph = viewModel->GetEllipsisFinalGlyphIndex();
      DALI_TEST_CHECK(ellipsisGlyph < glyphCount);
      DALI_TEST_EQUALS(glyphToLine[ellipsisGlyph], expectedEllipsisLine, TEST_LOCATION);
      const uint32_t ellipsisUnit = perLinePlan.glyphToUnit[ellipsisGlyph];
      DALI_TEST_CHECK(ellipsisUnit < perLinePlan.GetUnitCount());
      DALI_TEST_EQUALS(perLinePlan.imageReplacementUnitMask[ellipsisUnit], 0u, TEST_LOCATION);
      if(unit == Reveal::Unit::WORD && requireSeparateWordUnits)
      {
        DALI_TEST_CHECK(imageUnit != ellipsisUnit);
      }
    }

    if(unit == Reveal::Unit::PIXEL)
    {
      DALI_TEST_CHECK(perLinePlan.HasPixelTiming());
      DALI_TEST_EQUALS(perLinePlan.pixelUnitTiming.size(),
                       static_cast<std::size_t>(perLinePlan.GetUnitCount()),
                       TEST_LOCATION);
      const Reveal::PixelUnitTiming& pixelTiming = perLinePlan.pixelUnitTiming[imageUnit];
      DALI_TEST_CHECK(std::isfinite(pixelTiming.visualMinimum));
      DALI_TEST_CHECK(std::isfinite(pixelTiming.visualMaximum));
      DALI_TEST_CHECK(std::isfinite(pixelTiming.progressionSpan));
      DALI_TEST_CHECK(pixelTiming.visualMaximum - pixelTiming.visualMinimum + EPSILON >= placements[0u].size.width);
      DALI_TEST_CHECK(pixelTiming.progressionSpan > 0.0f);
      DALI_TEST_EQUALS(timings[0u].progressionSpan,
                       pixelTiming.progressionSpan,
                       EPSILON,
                       TEST_LOCATION);
      DALI_TEST_EQUALS(timings[0u].rightToLeft, pixelTiming.rightToLeft, TEST_LOCATION);
    }
    else
    {
      DALI_TEST_EQUALS(timings[0u].progressionSpan, 0.0f, EPSILON, TEST_LOCATION);
    }

    return CheckedPlan{timings[0u]};
  };

  auto buildCase = [&imageUrl](const char* text,
                               uint32_t    imageCharacter,
                               uint32_t    maximumLines,
                               const Size& size,
                               bool        elideText = true)
  {
    UiText::StyledTextBuilder builder = UiText::StyledTextBuilder::New(text);
    DALI_TEST_CHECK(builder.SetSpan(
      UiText::ImageSpan::New(UiText::ImageAttributes(imageUrl.GetUrl(), Vector2(32.0f, 24.0f))),
      imageCharacter,
      imageCharacter + 1u));

    UiText::ControllerPtr controller = UiText::Controller::New();
    controller->SetDefaultFontSize(20.0f, UiText::Controller::PIXEL_SIZE);
    controller->SetStyledText(builder.Build());
    controller->SetTextElideEnabled(elideText);
    controller->SetMultiLineEnabled(true);
    if(elideText)
    {
      controller->SetMaximumNumberOfLines(maximumLines);
      controller->SetEllipsisPosition(UiText::EllipsisPosition::END);
    }
    controller->Relayout(size);
    return controller;
  };

  // The no-ellipsis cross product is the control: the same projected model
  // already supplies complete final line ownership without an END result.
  UiText::ControllerPtr noEllipsis = buildCase("A X\nB C",
                                               2u,
                                               UiText::MAXIMUM_LINES_UNLIMITED,
                                               Size(320.0f, 200.0f),
                                               false);
  for(Reveal::Unit unit : {Reveal::Unit::CHARACTER, Reveal::Unit::WORD, Reveal::Unit::PIXEL})
  {
    checkPlan(*noEllipsis->GetRenderTextModel(),
              noEllipsis->GetFinalElisionResult(),
              noEllipsis->GetReplacementSourceSnapshot(),
              noEllipsis->GetReplacementRenderState().placements,
              unit,
              false,
              0u,
              Reveal::NO_UNIT,
              false);
  }

  struct PositionCase
  {
    const char* text;
    uint32_t    imageCharacter;
    uint32_t    maximumLines;
    uint32_t    imageLine;
    uint32_t    ellipsisLine;
    bool        requireSeparateWordUnits;
  };
  const std::array<PositionCase, 4u> positionCases{{
    {"X alpha\nbeta gamma\nhidden continuation", 0u, 2u, 0u, 1u, false},
    {"alpha\nX beta\ngamma delta\nhidden continuation", 6u, 3u, 1u, 2u, false},
    {"alpha\nbeta X\nhidden continuation", 11u, 2u, 1u, 1u, true},
    {"alpha\nX beta\nhidden continuation", 6u, 2u, 1u, 1u, false},
  }};

  UiText::ControllerPtr parityController;
  for(std::size_t positionCase = 0u; positionCase < positionCases.size(); ++positionCase)
  {
    const PositionCase& spec = positionCases[positionCase];
    UiText::ControllerPtr controller = buildCase(spec.text,
                                                 spec.imageCharacter,
                                                 spec.maximumLines,
                                                 Size(320.0f, 240.0f));
    const UiText::ReplacementRenderState& state = controller->GetReplacementRenderState();
    DALI_TEST_CHECK(state.finalElision.textElided && state.finalElision.applied);
    DALI_TEST_CHECK(state.finalElision.authoritativeLines);
    for(Reveal::Unit unit : {Reveal::Unit::CHARACTER, Reveal::Unit::WORD, Reveal::Unit::PIXEL})
    {
      checkPlan(*controller->GetRenderTextModel(),
                controller->GetFinalElisionResult(),
                controller->GetReplacementSourceSnapshot(),
                state.placements,
                unit,
                true,
                spec.imageLine,
                spec.ellipsisLine,
                spec.requireSeparateWordUnits);
    }
    if(positionCase == 2u)
    {
      parityController = controller;
    }
  }

  // Mixed-direction content keeps both the RTL replacement line and the final
  // ellipsis line authoritative for every unit, including PIXEL direction.
  UiText::ControllerPtr bidi = buildCase(u8"עברית X العربية\nEnglish text\nmore hidden words",
                                         6u,
                                         2u,
                                         Size(320.0f, 240.0f));
  bidi->SetLayoutDirectionMode(UiText::LayoutDirectionMode::CONTENTS);
  bidi->Relayout(Size(320.0f, 240.0f));
  const UiText::ReplacementRenderState& bidiState = bidi->GetReplacementRenderState();
  DALI_TEST_CHECK(bidiState.finalElision.textElided && bidiState.finalElision.applied);
  DALI_TEST_EQUALS(bidiState.placements[0u].lineIndex, 0u, TEST_LOCATION);
  // U+FFFC is direction-neutral. Pin this fixture's already-resolved ImageSpan
  // character to the RTL embedding used by the first line so the PIXEL
  // direction contract remains deterministic across bidi implementations.
  Vector<UiText::CharacterDirection>& bidiDirections =
    bidiState.processingModel->mLogicalModel->mCharacterDirections;
  bidiDirections.Resize(bidiState.processingModel->mLogicalModel->mText.Count());
  const UiText::CharacterIndex projectedImageCharacter =
    bidiState.projection.GetReplacementRuns()[0u].projectedCharacterIndex;
  DALI_TEST_CHECK(projectedImageCharacter < bidiDirections.Count());
  bidiDirections[projectedImageCharacter] = true;
  for(Reveal::Unit unit : {Reveal::Unit::CHARACTER, Reveal::Unit::WORD, Reveal::Unit::PIXEL})
  {
    const CheckedPlan checked = checkPlan(*bidi->GetRenderTextModel(),
                                          bidi->GetFinalElisionResult(),
                                          bidi->GetReplacementSourceSnapshot(),
                                          bidiState.placements,
                                          unit,
                                          true,
                                          0u,
                                          1u,
                                          false);
    if(unit == Reveal::Unit::PIXEL)
    {
      DALI_TEST_CHECK(checked.replacementTiming.rightToLeft);
    }
  }

  // Sync and worker-owned async paths must publish the same PER_LINE timing
  // from the same ImageSpan-immediately-before-ellipsis final layout.
  DALI_TEST_CHECK(parityController);
  const char* parityText = "alpha\nbeta X\nhidden continuation";
  for(Reveal::Unit unit : {Reveal::Unit::CHARACTER, Reveal::Unit::WORD, Reveal::Unit::PIXEL})
  {
    const UiText::ReplacementRenderState& syncState = parityController->GetReplacementRenderState();
    const CheckedPlan sync = checkPlan(*parityController->GetRenderTextModel(),
                                       parityController->GetFinalElisionResult(),
                                       parityController->GetReplacementSourceSnapshot(),
                                       syncState.placements,
                                       unit,
                                       true,
                                       1u,
                                       1u,
                                       true);

    UiText::AsyncTextParameters parameters;
    parameters.text                         = parityText;
    parameters.textWidth                    = 320.0f;
    parameters.textHeight                   = 240.0f;
    parameters.fontSize                     = parityController->GetDefaultFontSize(UiText::Controller::POINT_SIZE);
    parameters.maxTextureSize               = 4096;
    parameters.maximumNumberOfLines         = 2u;
    parameters.ellipsis                     = true;
    parameters.ellipsisPosition             = UiText::EllipsisPosition::END;
    parameters.isMultiLine                   = true;
    parameters.isTextRevealEnabled           = true;
    parameters.textRevealUnit                = unit;
    parameters.textRevealSequence            = Reveal::Sequence::PER_LINE;
    parameters.textRevealFadeDurationRatio   = 0.25f;
    parameters.textRevealSequenceStaggerRatio = 0.25f;
    parameters.replacementSourceSnapshot     = parityController->GetReplacementSourceSnapshot();
    parameters.replacementLayoutGeneration   = 73u;

    UiText::AsyncTextLoader asyncLoader = UiText::AsyncTextLoader::New();
    const UiText::AsyncTextRenderInfo asyncInfo = asyncLoader.RenderText(parameters,
                                                                         false,
                                                                         Size::ZERO);
    const UiText::ReplacementRenderState* asyncState =
      UiText::GetImplementation(asyncLoader).GetReplacementRenderState();
    DALI_TEST_CHECK(asyncState && asyncState->processingModel);
    DALI_TEST_CHECK(asyncState->finalElision.authoritativeLines);
    const CheckedPlan async = checkPlan(*asyncState->processingModel,
                                        &asyncState->finalElision,
                                        parameters.replacementSourceSnapshot,
                                        asyncState->placements,
                                        unit,
                                        true,
                                        1u,
                                        1u,
                                        true);
    DALI_TEST_CHECK(asyncInfo.isTextRevealEnabled);
    DALI_TEST_EQUALS(asyncInfo.replacementRevealTimings.Count(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(asyncInfo.replacementSourceRevision,
                     parameters.replacementSourceSnapshot.sourceRevision,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(asyncInfo.replacementRevealTimings[0u].start,
                     sync.replacementTiming.start,
                     EPSILON,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(asyncInfo.replacementRevealTimings[0u].fadeDuration,
                     sync.replacementTiming.fadeDuration,
                     EPSILON,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(asyncInfo.replacementRevealTimings[0u].progressionSpan,
                     sync.replacementTiming.progressionSpan,
                     EPSILON,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(asyncInfo.replacementRevealTimings[0u].rightToLeft,
                     sync.replacementTiming.rightToLeft,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(async.replacementTiming.start,
                     sync.replacementTiming.start,
                     EPSILON,
                     TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliTextRevealImageReplacementIntegrationP(void)
{
  UiTestApplication application;
  const std::string sourcePath = GetRepositoryResourcePath("dali-ui-foundation/images/broken.png");

  UiText::StyledTextBuilder builder = UiText::StyledTextBuilder::New("A X B");
  DALI_TEST_CHECK(builder.SetSpan(
    UiText::ImageSpan::New(UiText::ImageAttributes(Dali::String(sourcePath.c_str()),
                                                   Vector2(32.0f, 24.0f))),
    2u,
    3u));

  Ui::Label label = Ui::Label::New();
  label.SetStyledText(builder.Build());
  label.SetProperty(Actor::Property::SIZE, Vector2(320.0f, 100.0f));
  UiText::Reveal reveal;
  reveal.SetUnit(UiText::Reveal::Unit::CHARACTER);
  reveal.SetFadeDurationRatio(0.25f);
  label.SetTextReveal(reveal);
  label.SetTextRevealProgress(0.0f);
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                   1u,
                   TEST_LOCATION);
  DALI_TEST_CHECK(label.GetRendererCount() > 0u);
  DALI_TEST_CHECK(label.GetRendererAt(0u).GetPropertyIndex("uTextRevealProgress") != Property::INVALID_INDEX);

  Animation forward = Animation::New(0.20f);
  label.Animate(forward).TextRevealProgress(1.0f, Dali::Ui::Duration(0.20f));
  forward.Play();
  for(uint32_t frame = 0u; frame < 8u; ++frame)
  {
    application.SendNotification();
    application.Render(32);
    DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                     1u,
                     TEST_LOCATION);
  }
  DALI_TEST_EQUALS(label.GetCurrentProperty<float>(label.GetPropertyIndex("uTextRevealProgress")),
                   1.0f,
                   EPSILON,
                   TEST_LOCATION);

  Animation reverse = Animation::New(0.20f);
  label.Animate(reverse).TextRevealProgress(0.0f, Dali::Ui::Duration(0.20f));
  reverse.Play();
  for(uint32_t frame = 0u; frame < 8u; ++frame)
  {
    application.SendNotification();
    application.Render(32);
    DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                     1u,
                     TEST_LOCATION);
  }
  DALI_TEST_EQUALS(label.GetCurrentProperty<float>(label.GetPropertyIndex("uTextRevealProgress")),
                   0.0f,
                   EPSILON,
                   TEST_LOCATION);

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.EnableTextureCallTrace(true);
  gl.ResetTextureCallStack();
  for(uint32_t update = 0u; update < 1000u; ++update)
  {
    label.SetTextRevealProgress(static_cast<float>(update % 101u) * 0.01f);
  }
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexImage2D"), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexSubImage2D"), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                   1u,
                   TEST_LOCATION);

  label.SetTextReveal(UiText::Reveal::None());
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                   0u,
                   TEST_LOCATION);

  reveal.SetUnit(UiText::Reveal::Unit::PIXEL);
  reveal.SetSequence(UiText::Reveal::Sequence::PER_LINE);
  label.SetTextReveal(reveal);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  Ui::Integration::Visual::Base localPixelVisual = FindInlineReplacementVisual(label);
  DALI_TEST_CHECK(localPixelVisual);
  DALI_TEST_CHECK(Ui::GetImplementation(localPixelVisual).IsUsingCustomShader());
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealTimingCount(label),
                   1u,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                   1u,
                   TEST_LOCATION);

  label.SetTextReveal(UiText::Reveal::None());
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                   0u,
                   TEST_LOCATION);
  label.SetAsyncRendering(true);
  label.SetTextReveal(reveal);
  application.SendNotification();
  application.Render();
  for(uint32_t completion = 0u;
      completion < 4u && Dali::Ui::Internal::GetInlineReplacementRevealTimingCount(label) == 0u;
      ++completion)
  {
    DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealTimingCount(label),
                   1u,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                   1u,
                   TEST_LOCATION);

  // A pending result cannot restore replacement timing after Reveal is
  // disabled under a newer revision.
  UiText::StyledTextBuilder staleBuilder = UiText::StyledTextBuilder::New("new X source");
  DALI_TEST_CHECK(staleBuilder.SetSpan(
    UiText::ImageSpan::New(UiText::ImageAttributes(Dali::String(sourcePath.c_str()),
                                                   Vector2(48.0f, 32.0f))),
    4u,
    5u));
  label.SetStyledText(staleBuilder.Build());
  application.SendNotification();
  application.Render();
  label.SetTextReveal(UiText::Reveal::None());
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                   0u,
                   TEST_LOCATION);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealTimingCount(label),
                   0u,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                   0u,
                   TEST_LOCATION);

  label.SetAsyncRendering(false);
  UiText::StyledTextBuilder multipleBuilder = UiText::StyledTextBuilder::New("A X B X C");
  DALI_TEST_CHECK(multipleBuilder.SetSpan(
    UiText::ImageSpan::New(UiText::ImageAttributes(Dali::String(sourcePath.c_str()),
                                                   Vector2(24.0f, 20.0f))),
    2u,
    3u));
  DALI_TEST_CHECK(multipleBuilder.SetSpan(
    UiText::ImageSpan::New(UiText::ImageAttributes(Dali::String(sourcePath.c_str()),
                                                   Vector2(72.0f, 48.0f))),
    6u,
    7u));
  label.SetStyledText(multipleBuilder.Build());
  label.SetTextRevealProgress(0.55f);
  for(const auto& configuration : {
        std::make_pair(UiText::Reveal::Unit::CHARACTER, UiText::Reveal::Sequence::WHOLE_TEXT),
        std::make_pair(UiText::Reveal::Unit::WORD, UiText::Reveal::Sequence::WHOLE_TEXT),
        std::make_pair(UiText::Reveal::Unit::PIXEL, UiText::Reveal::Sequence::PER_LINE)})
  {
    reveal.SetUnit(configuration.first);
    reveal.SetSequence(configuration.second);
    label.SetTextReveal(reveal);
    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealTimingCount(label),
                     2u,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                     2u,
                     TEST_LOCATION);
  }

  Renderer sceneStableRenderer = FindInlineReplacementRevealRenderer(label);
  DALI_TEST_CHECK(sceneStableRenderer);
  const uint32_t        sceneStablePropertyCount = sceneStableRenderer.GetPropertyCount();
  const Property::Index sceneStableBaseOpacityIndex =
    sceneStableRenderer.GetPropertyIndex("__dali_ui_inline_replacement_reveal_base_opacity");
  DALI_TEST_CHECK(sceneStableBaseOpacityIndex != Property::INVALID_INDEX);
  for(uint32_t cycle = 0u; cycle < 100u; ++cycle)
  {
    application.GetScene().Remove(label);
    label.SetTextRevealProgress(static_cast<float>(cycle % 101u) * 0.01f);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealTimingCount(label),
                     2u,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                     2u,
                     TEST_LOCATION);

    application.GetScene().Add(label);
    application.SendNotification();
    application.Render();
    Renderer reconnectedRenderer = FindInlineReplacementRevealRenderer(label);
    DALI_TEST_CHECK(reconnectedRenderer == sceneStableRenderer);
    DALI_TEST_EQUALS(reconnectedRenderer.GetPropertyCount(), sceneStablePropertyCount, TEST_LOCATION);
    DALI_TEST_EQUALS(reconnectedRenderer.GetPropertyIndex("__dali_ui_inline_replacement_reveal_base_opacity"),
                     sceneStableBaseOpacityIndex,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                     2u,
                     TEST_LOCATION);
  }

  label.SetTextReveal(UiText::Reveal::None());
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                   0u,
                   TEST_LOCATION);
  label.SetTextReveal(reveal);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                   2u,
                   TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextRevealImageReplacementAsyncPixelAutoP(void)
{
  UiTestApplication application;

  Texture      imageTexture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 4u, 4u);
  Ui::ImageUrl imageUrl     = Ui::ImageUrl::New(imageTexture, true);

  auto runCase = [&](const char*                              text,
                     const std::vector<UiText::CharacterRun>& ranges,
                     UiText::Reveal::Sequence                 sequence,
                     float                                    fadeDurationRatio)
  {
    UiText::StyledTextBuilder builder = UiText::StyledTextBuilder::New(text);
    for(const UiText::CharacterRun& range : ranges)
    {
      DALI_TEST_CHECK(builder.SetSpan(
        UiText::ImageSpan::New(UiText::ImageAttributes(imageUrl.GetUrl(), Vector2(32.0f, 24.0f))),
        range.characterIndex,
        range.characterIndex + range.numberOfCharacters));
    }

    Ui::Label label = Ui::Label::New();
    label.SetAsyncRendering(true);
    label.SetStyledText(builder.Build());
    label.SetProperty(Actor::Property::SIZE, Vector2(360.0f, 120.0f));

    UiText::Reveal reveal;
    reveal.SetUnit(UiText::Reveal::Unit::PIXEL);
    reveal.SetSequence(sequence);
    reveal.SetFadeDurationRatio(fadeDurationRatio);
    label.SetTextReveal(reveal);
    label.SetTextRevealProgress(0.0f);

    application.GetScene().Add(label);
    application.SendNotification();
    application.Render();

    const std::size_t expectedReplacementCount = ranges.size();
    if(expectedReplacementCount == 0u)
    {
      DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
      application.SendNotification();
      application.Render();
    }
    else
    {
      for(uint32_t completion = 0u;
          completion < 8u &&
          (Dali::Ui::Internal::GetInlineReplacementRevealTimingCount(label) != expectedReplacementCount ||
           Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label) != expectedReplacementCount);
          ++completion)
      {
        DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
        application.SendNotification();
        application.Render();
      }
    }

    DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealTimingCount(label),
                     expectedReplacementCount,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                     expectedReplacementCount,
                     TEST_LOCATION);

    bool hasRevealRenderer = false;
    for(uint32_t rendererIndex = 0u; rendererIndex < label.GetRendererCount(); ++rendererIndex)
    {
      hasRevealRenderer |= label.GetRendererAt(rendererIndex).GetPropertyIndex("uTextRevealProgress") !=
                           Property::INVALID_INDEX;
    }
    DALI_TEST_CHECK(hasRevealRenderer);

    label.SetTextRevealProgress(1.0f);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(label.GetCurrentProperty<float>(label.GetPropertyIndex("uTextRevealProgress")),
                     1.0f,
                     EPSILON,
                     TEST_LOCATION);

    application.GetScene().Remove(label);
    application.SendNotification();
    application.Render();
  };

  const std::vector<UiText::CharacterRun> mixed{{2u, 1u}};
  runCase("A X B", mixed, UiText::Reveal::Sequence::WHOLE_TEXT,
          UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  runCase("A X B", mixed, UiText::Reveal::Sequence::PER_LINE,
          UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  runCase("X", {{0u, 1u}}, UiText::Reveal::Sequence::WHOLE_TEXT,
          UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  runCase("X", {{0u, 1u}}, UiText::Reveal::Sequence::PER_LINE,
          UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  runCase("A X B X C", {{2u, 1u}, {6u, 1u}}, UiText::Reveal::Sequence::WHOLE_TEXT,
          UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  runCase("A X B", mixed, UiText::Reveal::Sequence::WHOLE_TEXT, 0.25f);
  runCase("ordinary async pixel auto", {}, UiText::Reveal::Sequence::WHOLE_TEXT,
          UiText::Reveal::AUTO_FADE_DURATION_RATIO);

  END_TEST;
}

int UtcDaliTextRevealImageReplacementAsyncOwnerDestructionP(void)
{
  UiTestApplication application;

  // A texture-backed URL avoids an unrelated image-loader completion. The
  // only deferred callback below is therefore the TextVisual render request
  // whose observer lifetime this test targets.
  Texture                   imageTexture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 4u, 4u);
  Ui::ImageUrl              imageUrl     = Ui::ImageUrl::New(imageTexture, true);
  UiText::StyledTextBuilder builder      = UiText::StyledTextBuilder::New("A X B lifecycle ownership");
  DALI_TEST_CHECK(builder.SetSpan(
    UiText::ImageSpan::New(UiText::ImageAttributes(imageUrl.GetUrl(), Vector2(24.0f, 20.0f))),
    2u,
    3u));
  const UiText::StyledText styledText = builder.Build();

  for(uint32_t cycle = 0u; cycle < 100u; ++cycle)
  {
    Ui::Label label = Ui::Label::New();
    label.SetStyledText(styledText);
    label.SetProperty(Actor::Property::SIZE, Vector2(360.0f, 96.0f));

    UiText::Reveal reveal;
    switch(cycle % 3u)
    {
      case 0u:
        reveal.SetUnit(UiText::Reveal::Unit::CHARACTER);
        break;
      case 1u:
        reveal.SetUnit(UiText::Reveal::Unit::WORD);
        break;
      default:
        reveal.SetUnit(UiText::Reveal::Unit::PIXEL);
        break;
    }
    reveal.SetSequence((cycle & 1u) ? UiText::Reveal::Sequence::PER_LINE
                                    : UiText::Reveal::Sequence::WHOLE_TEXT);
    reveal.SetFadeDurationRatio(0.25f);
    label.SetTextReveal(reveal);
    label.SetTextRevealProgress(0.4f);
    application.GetScene().Add(label);
    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealTimingCount(label),
                     1u,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                     1u,
                     TEST_LOCATION);

    // Keep the currently valid ImageSpan binding while a replacement schedule
    // is rendered asynchronously. Consume the event trigger without invoking
    // its completion so Label destruction is guaranteed to happen first.
    label.SetAsyncRendering(true);
    reveal.SetFadeDurationRatio(0.1f);
    label.SetTextReveal(reveal);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                     1u,
                     TEST_LOCATION);
    DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, false));

    WeakHandle<Ui::Label> weakLabel(label);
    application.GetScene().Remove(label);
    label.Reset();
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK(!weakLabel.GetHandle());

    // TextLoadObserver destruction removes the raw observer from
    // AsyncTextManager before this queued completion is dispatched.
    Test::AsyncTaskManager::ProcessAllCompletedTasks();
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK(!weakLabel.GetHandle());
  }

  END_TEST;
}

int UtcDaliTextRevealImageReplacementReconfigurationAtomicityP(void)
{
  UiTestApplication application;
  const std::string sourceAPath = GetRepositoryResourcePath("dali-ui-foundation/images/broken.png");
  const std::string sourceBPath = GetRepositoryResourcePath("samples/text/res/flag_us_alt.png");

  UiText::StyledTextBuilder builder = UiText::StyledTextBuilder::New("A\nX\nB");
  DALI_TEST_CHECK(builder.SetSpan(
    UiText::ImageSpan::New(UiText::ImageAttributes(Dali::String(sourceAPath.c_str()),
                                                   Vector2(32.0f, 24.0f))),
    2u,
    3u));

  Ui::Label label = Ui::Label::New();
  label.SetMultiLine(true);
  label.SetStyledText(builder.Build());
  label.SetProperty(Actor::Property::SIZE, Vector2(240.0f, 180.0f));
  UiText::Reveal reveal;
  reveal.SetUnit(UiText::Reveal::Unit::CHARACTER);
  reveal.SetFadeDurationRatio(0.25f);
  label.SetTextReveal(reveal);
  label.SetTextRevealProgress(0.2f);
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  Renderer imageRenderer = FindInlineReplacementRevealRenderer(label);
  DALI_TEST_CHECK(imageRenderer);
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                   1u,
                   TEST_LOCATION);
  DALI_TEST_CHECK(imageRenderer.GetCurrentProperty<float>(Dali::DevelRenderer::Property::OPACITY) < 0.99f);

  auto checkPartialRevealBinding = [&]()
  {
    if(Dali::Ui::Internal::IsInlineReplacementRevealPixelSpatial(label))
    {
      const Property::Index progressIndex =
        imageRenderer.GetPropertyIndex("uInlineReplacementRevealProgress");
      DALI_TEST_CHECK(progressIndex != Property::INVALID_INDEX);
      DALI_TEST_EQUALS(imageRenderer.GetCurrentProperty<float>(progressIndex),
                       0.2f,
                       EPSILON,
                       TEST_LOCATION);
    }
    else
    {
      DALI_TEST_CHECK(imageRenderer.GetCurrentProperty<float>(Dali::DevelRenderer::Property::OPACITY) < 0.99f);
    }
  };

  auto applyConfiguration = [&]()
  {
    checkPartialRevealBinding();
    label.SetTextReveal(reveal);

    // An enabled-to-enabled change retains the old valid binding until the
    // replacement schedule is ready. The atomic path must not restore full
    // opacity, and the PIXEL path must retain the authored shader progress.
    DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                     1u,
                     TEST_LOCATION);
    checkPartialRevealBinding();

    application.SendNotification();
    application.Render();
    imageRenderer = FindInlineReplacementRevealRenderer(label);
    DALI_TEST_CHECK(imageRenderer);
    checkPartialRevealBinding();

    application.SendNotification();
    application.Render();
    checkPartialRevealBinding();
    DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.2f, EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetCurrentProperty<float>(label.GetPropertyIndex("uTextRevealProgress")),
                     0.2f,
                     EPSILON,
                     TEST_LOCATION);
  };

  reveal.SetUnit(UiText::Reveal::Unit::WORD);
  applyConfiguration();
  reveal.SetUnit(UiText::Reveal::Unit::PIXEL);
  applyConfiguration();
  reveal.SetSequence(UiText::Reveal::Sequence::PER_LINE);
  applyConfiguration();
  reveal.SetSequenceStaggerRatio(0.25f);
  applyConfiguration();
  reveal.SetFadeDurationRatio(0.1f);
  applyConfiguration();
  // Replacing the styled source unregisters the old visual immediately. The
  // new visual is constrained only after timings for its source revision have
  // been published, without restarting the authored progress.
  const uint64_t sourceRevisionA = Dali::Ui::Internal::GetInlineReplacementEntrySourceRevision(label);
  DALI_TEST_CHECK(sourceRevisionA != 0u);
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealSourceRevision(label),
                   sourceRevisionA,
                   TEST_LOCATION);
  Renderer oldImageRenderer = imageRenderer;

  UiText::StyledTextBuilder sourceB = UiText::StyledTextBuilder::New("A\nY\nB");
  DALI_TEST_CHECK(sourceB.SetSpan(
    UiText::ImageSpan::New(UiText::ImageAttributes(Dali::String(sourceBPath.c_str()),
                                                   Vector2(32.0f, 24.0f))),
    2u,
    3u));
  label.SetStyledText(sourceB.Build());
  DALI_TEST_CHECK(!FindInlineReplacementVisual(label));
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                   0u,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.2f, EPSILON, TEST_LOCATION);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  imageRenderer = FindInlineReplacementRevealRenderer(label);
  DALI_TEST_CHECK(imageRenderer);
  DALI_TEST_CHECK(imageRenderer != oldImageRenderer);
  checkPartialRevealBinding();
  const uint64_t sourceRevisionB = Dali::Ui::Internal::GetInlineReplacementEntrySourceRevision(label);
  DALI_TEST_CHECK(sourceRevisionB > sourceRevisionA);
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealSourceRevision(label),
                   sourceRevisionB,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealTimingCount(label),
                   1u,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                   1u,
                   TEST_LOCATION);

  // Restore source A before the async portion so the configuration test starts
  // from a resource-ready visual with a known source revision.
  UiText::StyledTextBuilder asyncBase = UiText::StyledTextBuilder::New("A\nX\nB");
  DALI_TEST_CHECK(asyncBase.SetSpan(
    UiText::ImageSpan::New(UiText::ImageAttributes(Dali::String(sourceAPath.c_str()),
                                                   Vector2(32.0f, 24.0f))),
    2u,
    3u));
  label.SetStyledText(asyncBase.Build());
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  imageRenderer = FindInlineReplacementRevealRenderer(label);
  DALI_TEST_CHECK(imageRenderer);
  checkPartialRevealBinding();
  const uint64_t asyncBaseSourceRevision = Dali::Ui::Internal::GetInlineReplacementEntrySourceRevision(label);
  DALI_TEST_CHECK(asyncBaseSourceRevision > sourceRevisionB);
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealSourceRevision(label),
                   asyncBaseSourceRevision,
                   TEST_LOCATION);

  label.SetAsyncRendering(true);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
  application.SendNotification();
  application.Render();
  imageRenderer = FindInlineReplacementRevealRenderer(label);
  DALI_TEST_CHECK(imageRenderer);
  checkPartialRevealBinding();

  reveal.SetFadeDurationRatio(0.25f);
  applyConfiguration();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
  application.SendNotification();
  application.Render();
  imageRenderer = FindInlineReplacementRevealRenderer(label);
  DALI_TEST_CHECK(imageRenderer);
  checkPartialRevealBinding();
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                   1u,
                   TEST_LOCATION);

  // A second source edit supersedes an already pending async result. Only the
  // newest source revision may create and bind the replacement visual.
  oldImageRenderer = imageRenderer;
  UiText::StyledTextBuilder staleSource = UiText::StyledTextBuilder::New("A\nZ\nB");
  DALI_TEST_CHECK(staleSource.SetSpan(
    UiText::ImageSpan::New(UiText::ImageAttributes(Dali::String(sourceBPath.c_str()),
                                                   Vector2(32.0f, 24.0f))),
    2u,
    3u));
  label.SetStyledText(staleSource.Build());

  UiText::StyledTextBuilder sourceC = UiText::StyledTextBuilder::New("A\nW\nB");
  DALI_TEST_CHECK(sourceC.SetSpan(
    UiText::ImageSpan::New(UiText::ImageAttributes(Dali::String(sourceAPath.c_str()),
                                                   Vector2(32.0f, 24.0f))),
    2u,
    3u));
  label.SetStyledText(sourceC.Build());
  DALI_TEST_CHECK(!FindInlineReplacementVisual(label));
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                   0u,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.2f, EPSILON, TEST_LOCATION);

  application.SendNotification();
  application.Render();
  for(uint32_t completion = 0u;
      completion < 8u && Dali::Ui::Internal::GetInlineReplacementEntrySourceRevision(label) <= asyncBaseSourceRevision;
      ++completion)
  {
    DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
    application.SendNotification();
    application.Render();
  }
  application.SendNotification();
  application.Render();

  imageRenderer = FindInlineReplacementRevealRenderer(label);
  DALI_TEST_CHECK(imageRenderer);
  DALI_TEST_CHECK(imageRenderer != oldImageRenderer);
  checkPartialRevealBinding();
  const uint64_t sourceRevisionC = Dali::Ui::Internal::GetInlineReplacementEntrySourceRevision(label);
  DALI_TEST_CHECK(sourceRevisionC > asyncBaseSourceRevision);
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealSourceRevision(label),
                   sourceRevisionC,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealTimingCount(label),
                   1u,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                   1u,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.2f, EPSILON, TEST_LOCATION);

  const bool pixelSpatial = Dali::Ui::Internal::IsInlineReplacementRevealPixelSpatial(label);
  const Property::Index resourceOpacityIndex =
    pixelSpatial
      ? Dali::DevelRenderer::Property::OPACITY
      : imageRenderer.GetPropertyIndex("__dali_ui_inline_replacement_reveal_base_opacity");
  DALI_TEST_CHECK(resourceOpacityIndex != Property::INVALID_INDEX);
  for(uint32_t completion = 0u;
      completion < 4u && imageRenderer.GetCurrentProperty<float>(resourceOpacityIndex) < 0.99f;
      ++completion)
  {
    DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_CHECK(imageRenderer.GetCurrentProperty<float>(resourceOpacityIndex) >= 0.99f);
  label.SetAsyncRendering(false);

  // Reveal -> None is intentionally different: remove the constraint and
  // restore ordinary ImageVisual resource-ready visibility.
  label.SetTextReveal(UiText::Reveal::None());
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                   0u,
                   TEST_LOCATION);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(imageRenderer.GetCurrentProperty<float>(Dali::DevelRenderer::Property::OPACITY),
                   1.0f,
                   0.01f,
                   TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextRevealImageReplacementStressP(void)
{
  UiTestApplication application;
  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.EnableTextureCallTrace(true);

  for(uint32_t imageCount : {1u, 10u, 50u})
  {
    std::string                   text;
    std::vector<UiText::CharacterRun> ranges;
    for(uint32_t image = 0u; image < imageCount; ++image)
    {
      if(!text.empty())
      {
        text += ' ';
      }
      const uint32_t imageIndex = static_cast<uint32_t>(text.size());
      text += 'X';
      ranges.push_back({imageIndex, 1u});
    }

    UiText::StyledTextBuilder builder = UiText::StyledTextBuilder::New(text.c_str());
    for(const UiText::CharacterRun& range : ranges)
    {
      DALI_TEST_CHECK(builder.SetSpan(
        UiText::ImageSpan::New(UiText::ImageAttributes("dali-ui-foundation/images/broken.png",
                                                       Vector2(20.0f, 20.0f))),
        range.characterIndex,
        range.characterIndex + range.numberOfCharacters));
    }
    Ui::Label label = Ui::Label::New();
    label.SetMultiLine(true);
    label.SetProperty(Actor::Property::SIZE, Vector2(1200.0f, 600.0f));
    label.SetStyledText(builder.Build());
    UiText::Reveal reveal;
    reveal.SetUnit(UiText::Reveal::Unit::PIXEL);
    reveal.SetSequence(UiText::Reveal::Sequence::PER_LINE);
    reveal.SetFadeDurationRatio(0.25f);
    label.SetTextReveal(reveal);
    label.SetTextRevealProgress(0.5f);
    application.GetScene().Add(label);
    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealTimingCount(label),
                     static_cast<std::size_t>(imageCount),
                     TEST_LOCATION);
    DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                     static_cast<std::size_t>(imageCount),
                     TEST_LOCATION);
    DALI_TEST_EQUALS(CountInlineReplacementRevealBaseOpacityProperties(label, imageCount),
                     0u,
                     TEST_LOCATION);

    gl.ResetTextureCallStack();
    for(uint32_t progressUpdate = 0u; progressUpdate < 1000u; ++progressUpdate)
    {
      label.SetTextRevealProgress(static_cast<float>(progressUpdate % 101u) * 0.01f);
    }
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexImage2D"), 0, TEST_LOCATION);
    DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexSubImage2D"), 0, TEST_LOCATION);
    DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                     static_cast<std::size_t>(imageCount),
                     TEST_LOCATION);
    DALI_TEST_EQUALS(CountInlineReplacementRevealBaseOpacityProperties(label, imageCount),
                     0u,
                     TEST_LOCATION);

    label.SetTextReveal(UiText::Reveal::None());
    DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                     0u,
                     TEST_LOCATION);
    label.SetTextReveal(reveal);
    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(label),
                     static_cast<std::size_t>(imageCount),
                     TEST_LOCATION);
    DALI_TEST_EQUALS(CountInlineReplacementRevealBaseOpacityProperties(label, imageCount),
                     0u,
                     TEST_LOCATION);
    application.GetScene().Remove(label);
  }

  END_TEST;
}

int UtcDaliTextRevealImageReplacementIsolationP(void)
{
  UiTestApplication application;

  Ui::Label ordinary = Ui::Label::New("ordinary text-only Reveal");
  ordinary.SetProperty(Actor::Property::SIZE, Vector2(420.0f, 80.0f));
  UiText::Reveal reveal;
  reveal.SetUnit(UiText::Reveal::Unit::PIXEL);
  ordinary.SetTextReveal(reveal);
  application.GetScene().Add(ordinary);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Dali::Ui::Internal::Text::GetInlineReplacementData(ordinary) == nullptr);
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealTimingCount(ordinary),
                   0u,
                   TEST_LOCATION);

  UiText::ControllerPtr textOnlyController = BuildReplacementController(
    "Alpha beta gamma", {}, Size(420.0f, 80.0f));
  const UiText::ModelInterface* textOnlyModel = textOnlyController->GetRenderTextModel();
  DALI_TEST_CHECK(textOnlyModel);
  UiText::TypesetterPtr textOnlyTypesetter = UiText::Typesetter::New(textOnlyModel);
  textOnlyTypesetter->SetFinalElisionResult(textOnlyController->GetFinalElisionResult());
  TextAbstraction::Segmentation segmentation = TextAbstraction::Segmentation::New();
  for(Reveal::Unit unit : {Reveal::Unit::CHARACTER, Reveal::Unit::WORD, Reveal::Unit::PIXEL})
  {
    Reveal::Plan sourcePlan = unit == Reveal::Unit::WORD
                                ? Reveal::BuildPlan(*textOnlyModel, unit, 0.25f, segmentation)
                              : unit == Reveal::Unit::PIXEL
                                ? Reveal::BuildPixelPlan(*textOnlyModel, 0.25f)
                                : Reveal::BuildCharacterPlan(*textOnlyModel, 0.25f);
    DALI_TEST_CHECK(sourcePlan.imageReplacementUnitMask.empty());
    DALI_TEST_EQUALS(sourcePlan.imageReplacementUnitMask.capacity(), 0u, TEST_LOCATION);
    for(Reveal::Sequence sequence : {Reveal::Sequence::WHOLE_TEXT, Reveal::Sequence::PER_LINE})
    {
      const Reveal::Plan finalPlan = textOnlyTypesetter->CreateFinalRevealPlan(sourcePlan,
                                                                               unit,
                                                                               sequence,
                                                                               0.25f);
      DALI_TEST_CHECK(finalPlan.imageReplacementUnitMask.empty());
      DALI_TEST_EQUALS(finalPlan.imageReplacementUnitMask.capacity(), 0u, TEST_LOCATION);
    }
  }

  UiText::StyledTextBuilder builder = UiText::StyledTextBuilder::New("A X B");
  DALI_TEST_CHECK(builder.SetSpan(
    UiText::ImageSpan::New(UiText::ImageAttributes("dali-ui-foundation/images/broken.png",
                                                   Vector2(24.0f, 20.0f))),
    2u,
    3u));
  Ui::Label revealDisabled = Ui::Label::New();
  revealDisabled.SetStyledText(builder.Build());
  revealDisabled.SetProperty(Actor::Property::SIZE, Vector2(320.0f, 80.0f));
  application.GetScene().Add(revealDisabled);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Dali::Ui::Internal::Text::GetInlineReplacementData(revealDisabled));
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealTimingCount(revealDisabled),
                   0u,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::Internal::GetInlineReplacementRevealConstraintCount(revealDisabled),
                   0u,
                   TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextRevealPixelMixedLineAutoP(void)
{
  UiTestApplication application;

  // The long 20px line is identical in all fixtures. Changing or reordering
  // the unrelated short line must not change that line's PER_LINE AUTO cadence.
  const std::string longLine =
    "Your reservation is confirmed. Check in before 18:30 and keep your boarding pass ready.";

  auto buildController = [&](float shortLineSize, bool shortLineFirst)
  {
    const std::string shortLine = "Gate 4";
    const std::string text      = shortLineFirst ? shortLine + "\n" + longLine
                                                 : longLine + "\n" + shortLine;
    UiText::StyledTextBuilder builder = UiText::StyledTextBuilder::New(text.c_str());
    UiText::FontAttributes    shortLineAttributes;
    shortLineAttributes.SetSize(shortLineSize);
    const uint32_t shortLineStart = shortLineFirst ? 0u : longLine.size() + 1u;
    DALI_TEST_CHECK(builder.SetSpan(UiText::FontSpan::New(shortLineAttributes),
                                   shortLineStart,
                                   shortLineStart + shortLine.size()));

    UiText::ControllerPtr controller = UiText::Controller::New();
    controller->SetDefaultFontSize(20.0f, UiText::Controller::PIXEL_SIZE);
    controller->SetStyledText(builder.Build());
    controller->SetMultiLineEnabled(true);
    controller->Relayout(Size(1200.0f, 360.0f));
    return controller;
  };

  auto buildPlan = [](UiText::ControllerPtr controller,
                      float                 fadeRatio,
                      Reveal::Sequence      sequence = Reveal::Sequence::PER_LINE)
  {
    const UiText::ModelInterface* model = controller->GetRenderTextModel();
    DALI_TEST_CHECK(model && model->GetNumberOfLines() == 2u);
    UiText::TypesetterPtr typesetter = UiText::Typesetter::New(model);
    return typesetter->CreateFinalRevealPlan(
      Reveal::BuildPixelPlan(*model, fadeRatio),
      Reveal::Unit::PIXEL,
      sequence,
      0.0f);
  };

  auto getLineCompletion = [](const Reveal::Plan&          plan,
                              const UiText::ModelInterface& model,
                              uint32_t                      line)
  {
    DALI_TEST_CHECK(line < model.GetNumberOfLines());
    const UiText::LineRun* lines = model.GetLines();
    std::vector<bool>      seen(plan.GetUnitCount(), false);
    float                  completion = 0.0f;
    auto                   includeRun = [&](const UiText::GlyphRun& run)
    {
      for(uint32_t glyph = run.glyphIndex; glyph < run.glyphIndex + run.numberOfGlyphs; ++glyph)
      {
        const uint32_t unit = plan.glyphToUnit[glyph];
        if(unit == Reveal::NO_UNIT || seen[unit])
        {
          continue;
        }
        seen[unit] = true;
        completion = std::max(completion,
                              plan.unitStart[unit] + plan.pixelUnitTiming[unit].progressionSpan + plan.fadeDuration);
      }
    };
    includeRun(lines[line].glyphRun);
    if(lines[line].isSplitToTwoHalves)
    {
      includeRun(lines[line].glyphRunSecondHalf);
    }
    return completion;
  };

  UiText::ControllerPtr control = buildController(20.0f, false);
  UiText::ControllerPtr mixed   = buildController(84.0f, false);
  UiText::ControllerPtr inverse = buildController(84.0f, true);
  const Reveal::Plan    controlPlan = buildPlan(control, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  const Reveal::Plan    mixedPlan   = buildPlan(mixed, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  const Reveal::Plan    inversePlan = buildPlan(inverse, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  DALI_TEST_CHECK(controlPlan.HasPixelTiming() && mixedPlan.HasPixelTiming() && inversePlan.HasPixelTiming());

  DALI_TEST_EQUALS(mixedPlan.fadeDuration, controlPlan.fadeDuration, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(inversePlan.fadeDuration, controlPlan.fadeDuration, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(getLineCompletion(controlPlan, *control->GetRenderTextModel(), 0u),
                   getLineCompletion(mixedPlan, *mixed->GetRenderTextModel(), 0u),
                   EPSILON,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(getLineCompletion(controlPlan, *control->GetRenderTextModel(), 0u),
                   getLineCompletion(inversePlan, *inverse->GetRenderTextModel(), 1u),
                   EPSILON,
                   TEST_LOCATION);

  for(float explicitRatio : {0.0f, 0.25f, 0.5f, 1.0f})
  {
    const Reveal::Plan controlExplicit = buildPlan(control, explicitRatio);
    const Reveal::Plan mixedExplicit   = buildPlan(mixed, explicitRatio);
    const Reveal::Plan inverseExplicit = buildPlan(inverse, explicitRatio);
    DALI_TEST_EQUALS(controlExplicit.fadeDuration, explicitRatio, EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(mixedExplicit.fadeDuration, explicitRatio, EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(inverseExplicit.fadeDuration, explicitRatio, EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(getLineCompletion(controlExplicit, *control->GetRenderTextModel(), 0u),
                     getLineCompletion(mixedExplicit, *mixed->GetRenderTextModel(), 0u),
                     EPSILON,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(getLineCompletion(controlExplicit, *control->GetRenderTextModel(), 0u),
                     getLineCompletion(inverseExplicit, *inverse->GetRenderTextModel(), 1u),
                     EPSILON,
                     TEST_LOCATION);
  }

  const Reveal::Plan explicitWholeTextPlan = buildPlan(mixed, 0.25f, Reveal::Sequence::WHOLE_TEXT);
  DALI_TEST_EQUALS(explicitWholeTextPlan.fadeDuration, 0.25f, EPSILON, TEST_LOCATION);

  // Per-line text scale remains text-only when replacement geometry inflates
  // a line: 24px and 240px ImageSpan heights resolve the same text cadence.
  const std::string replacementText = longLine + "\nA X B";
  const uint32_t    replacementIndex = longLine.size() + 3u;
  UiText::ControllerPtr replacement = BuildReplacementController(replacementText.c_str(),
                                                                 {{replacementIndex, 1u}},
                                                                 Size(1200.0f, 360.0f),
                                                                 false,
                                                                 24.0f);
  UiText::ControllerPtr tallReplacement = BuildReplacementController(replacementText.c_str(),
                                                                     {{replacementIndex, 1u}},
                                                                     Size(1200.0f, 360.0f),
                                                                     false,
                                                                     240.0f);
  for(UiText::ControllerPtr controller : {replacement, tallReplacement})
  {
    controller->SetMultiLineEnabled(true);
    controller->Relayout(Size(1200.0f, 360.0f));
  }
  const Reveal::Plan replacementPlan = buildPlan(replacement, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  const Reveal::Plan tallReplacementPlan = buildPlan(tallReplacement,
                                                      UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  DALI_TEST_EQUALS(tallReplacementPlan.fadeDuration,
                   replacementPlan.fadeDuration,
                   EPSILON,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(getLineCompletion(tallReplacementPlan, *tallReplacement->GetRenderTextModel(), 0u),
                   getLineCompletion(replacementPlan, *replacement->GetRenderTextModel(), 0u),
                   EPSILON,
                   TEST_LOCATION);

  const Reveal::Plan explicitReplacementPlan     = buildPlan(replacement, 0.25f);
  const Reveal::Plan explicitTallReplacementPlan = buildPlan(tallReplacement, 0.25f);
  DALI_TEST_EQUALS(explicitReplacementPlan.fadeDuration, 0.25f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(explicitTallReplacementPlan.fadeDuration, 0.25f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(getLineCompletion(explicitTallReplacementPlan,
                                     *tallReplacement->GetRenderTextModel(),
                                     0u),
                   getLineCompletion(explicitReplacementPlan, *replacement->GetRenderTextModel(), 0u),
                   EPSILON,
                   TEST_LOCATION);

  END_TEST;
}
