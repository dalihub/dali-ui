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

#include <dali-ui-foundation/internal/text/reveal/text-reveal.h>

#include <dali-ui-foundation/internal/text/glyph-metrics-helper.h>
#include <dali-ui-foundation/internal/text/line-run.h>
#include <dali-ui-foundation/internal/text/rendering/styles/character-spacing-helper-functions.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-run-snapshot.h>
#include <dali-ui-foundation/internal/text/text-model-interface.h>
#include <dali/devel-api/text-abstraction/script.h>
#include <dali/devel-api/text-abstraction/segmentation.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace Internal
{
namespace Reveal
{
namespace
{
bool IsWordPunctuation(Character character)
{
  // DALi's segmentation intentionally exposes punctuation boundaries. For a
  // visual word reveal those boundaries are folded into an adjacent word so
  // punctuation does not consume a surprising standalone timeline step. The
  // text stack has no Unicode general-category API, so this deliberately local
  // table covers punctuation code points from the scripts handled here. Keep
  // the ASCII and fullwidth-ASCII sets explicit so currency, mathematical,
  // modifier and general symbols such as '$', '+', '=', '^', '|' and '~' do
  // not silently become punctuation-only reveal units.
  return (character >= 0x0021u && character <= 0x0023u) ||
         (character >= 0x0025u && character <= 0x002Au) ||
         (character >= 0x002Cu && character <= 0x002Fu) ||
         (character >= 0x003Au && character <= 0x003Bu) ||
         (character >= 0x003Fu && character <= 0x0040u) ||
         (character >= 0x005Bu && character <= 0x005Du) ||
         character == 0x005Fu || character == 0x007Bu || character == 0x007Du ||
         character == 0x00A1u || character == 0x00A7u || character == 0x00ABu ||
         character == 0x00B6u || character == 0x00B7u || character == 0x00BBu ||
         character == 0x00BFu ||
         (character >= 0x055Au && character <= 0x055Fu) ||
         (character >= 0x0589u && character <= 0x058Au) ||
         (character >= 0x05BEu && character <= 0x05BEu) ||
         (character >= 0x05C0u && character <= 0x05C0u) ||
         (character >= 0x05C3u && character <= 0x05C3u) ||
         (character >= 0x05F3u && character <= 0x05F4u) ||
         (character >= 0x0609u && character <= 0x060Au) ||
         (character >= 0x060Cu && character <= 0x060Du) ||
         (character >= 0x061Bu && character <= 0x061Bu) ||
         (character >= 0x061Du && character <= 0x061Fu) ||
         (character >= 0x066Au && character <= 0x066Du) ||
         (character >= 0x2010u && character <= 0x2027u) ||
         (character >= 0x2030u && character <= 0x2043u) ||
         (character >= 0x2045u && character <= 0x2051u) ||
         (character >= 0x2053u && character <= 0x205Eu) ||
         (character >= 0x2E00u && character <= 0x2E7Fu) ||
         (character >= 0x3001u && character <= 0x3003u) ||
         (character >= 0x3008u && character <= 0x3011u) ||
         (character >= 0x3014u && character <= 0x301Fu) ||
         character == 0x3030u || character == 0x303Du ||
         (character >= 0xFE10u && character <= 0xFE1Fu) ||
         (character >= 0xFE30u && character <= 0xFE4Fu) ||
         (character >= 0xFF01u && character <= 0xFF03u) ||
         (character >= 0xFF05u && character <= 0xFF0Au) ||
         (character >= 0xFF0Cu && character <= 0xFF0Fu) ||
         (character >= 0xFF1Au && character <= 0xFF1Bu) ||
         (character >= 0xFF1Fu && character <= 0xFF20u) ||
         (character >= 0xFF3Bu && character <= 0xFF3Du) ||
         character == 0xFF3Fu || character == 0xFF5Bu || character == 0xFF5Du ||
         (character >= 0xFF61u && character <= 0xFF65u);
}

float CalculateAdaptiveFadeDurationRatio(uint32_t unitCount)
{
  if(unitCount == 0u)
  {
    return 0.0f;
  }

  // Express the fade window as a bounded number of adjacent start intervals.
  // Growing it with sqrt(unitCount) keeps short strings sequential instead of
  // turning them into a whole-text fade, while preventing long strings from
  // degrading into flickering one-frame fades. This exact formula is an
  // internal AUTO policy rather than a public numeric promise.
  const float fadeSpan    = std::min(10.0f, std::max(1.0f, std::sqrt(static_cast<float>(unitCount))));
  const float denominator = static_cast<float>(unitCount - 1u) + fadeSpan;
  return fadeSpan / denominator;
}

float ResolveFadeDurationRatio(float authoredRatio, uint32_t unitCount)
{
  if(authoredRatio == Text::Reveal::AUTO_FADE_DURATION_RATIO)
  {
    return CalculateAdaptiveFadeDurationRatio(unitCount);
  }
  return authoredRatio;
}

float ResolveSpatialFadeDurationRatio(float authoredRatio, float effectiveUnitCount)
{
  if(authoredRatio != Text::Reveal::AUTO_FADE_DURATION_RATIO)
  {
    return authoredRatio;
  }

  effectiveUnitCount      = std::max(1.0f, effectiveUnitCount);
  const float fadeSpan    = std::min(10.0f, std::max(1.0f, std::sqrt(effectiveUnitCount)));
  const float denominator = std::max(0.0f, effectiveUnitCount - 1.0f) + fadeSpan;
  return denominator > 0.0f ? fadeSpan / denominator : 0.0f;
}

void PopulateSchedule(Plan& plan, uint32_t unitCount, float authoredRatio)
{
  plan.fadeDurationRatio = authoredRatio;
  plan.unitStart.resize(unitCount);
  if(unitCount == 0u)
  {
    plan.fadeDuration = 0.0f;
    return;
  }

  plan.fadeDuration         = ResolveFadeDurationRatio(authoredRatio, unitCount);
  const float startInterval = unitCount > 1u
                                ? (1.0f - plan.fadeDuration) / static_cast<float>(unitCount - 1u)
                                : 0.0f;
  for(uint32_t index = 0u; index < unitCount; ++index)
  {
    plan.unitStart[index] = static_cast<float>(index) * startInterval;
  }
}
} // unnamed namespace

Unit ToInternalUnit(Text::Reveal::Unit unit)
{
  switch(unit)
  {
    case Text::Reveal::Unit::WORD:
      return Unit::WORD;
    case Text::Reveal::Unit::LINE:
      return Unit::LINE;
    case Text::Reveal::Unit::PIXEL:
      return Unit::PIXEL;
    case Text::Reveal::Unit::CHARACTER:
    default:
      return Unit::CHARACTER;
  }
}

Sequence ToInternalSequence(Text::Reveal::Sequence sequence)
{
  return sequence == Text::Reveal::Sequence::PER_LINE ? Sequence::PER_LINE : Sequence::WHOLE_TEXT;
}

Plan BuildPlan(const Character*      text,
               Length                characterCount,
               const CharacterIndex* glyphToCharacter,
               Length                glyphCount,
               Unit                  unit,
               float                 fadeDurationRatio,
               const WordBreakInfo*  wordBreakInfo)
{
  Plan plan;
  plan.fadeDurationRatio = fadeDurationRatio;
  plan.glyphToUnit.assign(glyphCount, NO_UNIT);
  if(unit == Unit::DISABLED || !text || !glyphToCharacter || characterCount == 0u || glyphCount == 0u)
  {
    return plan;
  }

  if(unit == Unit::CHARACTER || unit == Unit::LINE || unit == Unit::PIXEL)
  {
    // glyphToCharacter contains shaping-cluster starts. Reusing that identity
    // keeps combining sequences and indivisible ligatures/emoji clusters atomic.
    std::vector<uint32_t> clusterToUnit(characterCount, NO_UNIT);
    for(Length glyph = 0u; glyph < glyphCount; ++glyph)
    {
      const CharacterIndex cluster = glyphToCharacter[glyph];
      if(cluster >= characterCount || TextAbstraction::IsWhiteSpace(text[cluster]))
      {
        continue;
      }
      clusterToUnit[cluster] = 0u; // Mark a cluster that owns at least one visible glyph.
    }

    uint32_t unitCount = 0u;
    for(CharacterIndex cluster = 0u; cluster < characterCount; ++cluster)
    {
      if(clusterToUnit[cluster] != NO_UNIT)
      {
        clusterToUnit[cluster] = unitCount++;
      }
    }
    for(Length glyph = 0u; glyph < glyphCount; ++glyph)
    {
      const CharacterIndex cluster = glyphToCharacter[glyph];
      if(cluster < characterCount)
      {
        plan.glyphToUnit[glyph] = clusterToUnit[cluster];
      }
    }
    PopulateSchedule(plan, unitCount, fadeDurationRatio);
    return plan;
  }

  std::vector<uint32_t> characterToRawUnit(characterCount, NO_UNIT);
  std::vector<uint32_t> rawBegin;
  std::vector<uint32_t> rawEnd;
  std::vector<bool>     rawPunctuationOnly;
  uint32_t              rawUnitCount = 0u;
  bool                  inWord       = false;
  for(Length character = 0u; character < characterCount; ++character)
  {
    if(TextAbstraction::IsWhiteSpace(text[character]))
    {
      inWord = false;
      continue;
    }

    if(!inWord)
    {
      rawBegin.push_back(character);
      rawEnd.push_back(character);
      rawPunctuationOnly.push_back(true);
      ++rawUnitCount;
      inWord = true;
    }
    const uint32_t rawUnit        = rawUnitCount - 1u;
    characterToRawUnit[character] = rawUnit;
    rawEnd[rawUnit]               = character;
    rawPunctuationOnly[rawUnit]   = rawPunctuationOnly[rawUnit] && IsWordPunctuation(text[character]);
    if(wordBreakInfo && wordBreakInfo[character] == TextAbstraction::WORD_BREAK)
    {
      inWord = false;
    }
  }

  // Fold each adjacent punctuation-only run into the preceding word, or into
  // the following word for opening punctuation. Isolated punctuation remains
  // one reveal unit instead of becoming one unit per punctuation character.
  std::vector<uint32_t> mergeTarget(rawUnitCount);
  for(uint32_t rawUnit = 0u; rawUnit < rawUnitCount; ++rawUnit)
  {
    mergeTarget[rawUnit] = rawUnit;
  }
  for(uint32_t runBegin = 0u; runBegin < rawUnitCount;)
  {
    if(!rawPunctuationOnly[runBegin])
    {
      ++runBegin;
      continue;
    }

    uint32_t runEnd = runBegin;
    while(runEnd + 1u < rawUnitCount && rawPunctuationOnly[runEnd + 1u] &&
          rawEnd[runEnd] + 1u == rawBegin[runEnd + 1u])
    {
      ++runEnd;
    }

    uint32_t target = runBegin;
    if(runBegin > 0u && !rawPunctuationOnly[runBegin - 1u] &&
       rawEnd[runBegin - 1u] + 1u == rawBegin[runBegin])
    {
      target = runBegin - 1u;
    }
    else if(runEnd + 1u < rawUnitCount && !rawPunctuationOnly[runEnd + 1u] &&
            rawEnd[runEnd] + 1u == rawBegin[runEnd + 1u])
    {
      target = runEnd + 1u;
    }

    for(uint32_t rawUnit = runBegin; rawUnit <= runEnd; ++rawUnit)
    {
      mergeTarget[rawUnit] = target;
    }
    runBegin = runEnd + 1u;
  }

  std::vector<uint32_t> targetToUnit(rawUnitCount, NO_UNIT);
  uint32_t              unitCount = 0u;
  for(uint32_t rawUnit = 0u; rawUnit < rawUnitCount; ++rawUnit)
  {
    const uint32_t target = mergeTarget[rawUnit];
    if(targetToUnit[target] == NO_UNIT)
    {
      targetToUnit[target] = unitCount++;
    }
  }

  std::vector<uint32_t> characterToUnit(characterCount, NO_UNIT);
  for(Length character = 0u; character < characterCount; ++character)
  {
    const uint32_t rawUnit = characterToRawUnit[character];
    if(rawUnit != NO_UNIT)
    {
      characterToUnit[character] = targetToUnit[mergeTarget[rawUnit]];
    }
  }

  for(Length glyph = 0u; glyph < glyphCount; ++glyph)
  {
    const CharacterIndex character = glyphToCharacter[glyph];
    if(character < characterCount)
    {
      plan.glyphToUnit[glyph] = characterToUnit[character];
    }
  }
  PopulateSchedule(plan, unitCount, fadeDurationRatio);
  return plan;
}

namespace
{
void FilterSyntheticReplacementGlyphs(Plan&                    plan,
                                      const GlyphInfo*         glyphs,
                                      Length                   glyphCount,
                                      const std::vector<bool>* eligibleImageGlyphs)
{
  if(!glyphs || plan.glyphToUnit.size() < glyphCount)
  {
    return;
  }

  bool hasSyntheticReplacement = false;
  for(Length glyph = 0u; glyph < glyphCount; ++glyph)
  {
    if(IsSyntheticReplacementGlyph(glyphs[glyph]))
    {
      hasSyntheticReplacement = true;
      break;
    }
  }
  if(!hasSyntheticReplacement)
  {
    return;
  }

  // Replacement visuals are published independently from the text texture.
  // The established path removes every layout-only glyph. ImageSpan Reveal
  // retains explicitly eligible image occurrences in the shared timeline
  // while all other synthetic replacements remain excluded.
  const uint32_t oldUnitCount         = plan.GetUnitCount();
  const bool     retainEligibleImages = eligibleImageGlyphs &&
                                    std::find(eligibleImageGlyphs->begin(),
                                              eligibleImageGlyphs->end(),
                                              true) != eligibleImageGlyphs->end();
  std::vector<bool> survivingUnits(oldUnitCount, false);
  std::vector<bool> replacementUnits;
  if(retainEligibleImages)
  {
    replacementUnits.assign(oldUnitCount, false);
  }
  std::vector<uint32_t> oldToNewUnit(oldUnitCount, NO_UNIT);
  for(Length glyph = 0u; glyph < glyphCount; ++glyph)
  {
    uint32_t& unit = plan.glyphToUnit[glyph];
    if(IsSyntheticReplacementGlyph(glyphs[glyph]))
    {
      const bool eligible = retainEligibleImages && glyph < eligibleImageGlyphs->size() &&
                            (*eligibleImageGlyphs)[glyph];
      if(eligible && unit < oldUnitCount)
      {
        survivingUnits[unit]   = true;
        replacementUnits[unit] = true;
      }
      else
      {
        unit = NO_UNIT;
      }
    }
    else if(unit < oldUnitCount)
    {
      survivingUnits[unit] = true;
    }
  }

  uint32_t newUnitCount = 0u;
  for(uint32_t oldUnit = 0u; oldUnit < oldUnitCount; ++oldUnit)
  {
    if(survivingUnits[oldUnit])
    {
      oldToNewUnit[oldUnit] = newUnitCount++;
    }
  }
  for(uint32_t& unit : plan.glyphToUnit)
  {
    if(unit != NO_UNIT)
    {
      unit = unit < oldUnitCount ? oldToNewUnit[unit] : NO_UNIT;
    }
  }
  const bool hasImageReplacementUnit = retainEligibleImages &&
                                       std::find(replacementUnits.begin(), replacementUnits.end(), true) !=
                                         replacementUnits.end();
  if(hasImageReplacementUnit)
  {
    plan.imageReplacementUnitMask.assign(newUnitCount, 0u);
    for(uint32_t oldUnit = 0u; oldUnit < oldUnitCount; ++oldUnit)
    {
      if(oldToNewUnit[oldUnit] != NO_UNIT && replacementUnits[oldUnit])
      {
        plan.imageReplacementUnitMask[oldToNewUnit[oldUnit]] = 1u;
      }
    }
  }
  PopulateSchedule(plan, newUnitCount, plan.fadeDurationRatio);
}

Plan BuildModelPlan(const ModelInterface&    model,
                    Unit                     unit,
                    float                    fadeDurationRatio,
                    const WordBreakInfo*     wordBreakInfo,
                    const std::vector<bool>* eligibleImageGlyphs = nullptr)
{
  const Length characterCount = model.GetNumberOfCharacters();
  const Length glyphCount     = model.GetNumberOfGlyphs();
  const auto&  glyphMap       = model.GetGlyphsToCharacters();
  if(characterCount == 0u || glyphCount == 0u || glyphMap.Count() < glyphCount)
  {
    Plan plan;
    plan.fadeDurationRatio = fadeDurationRatio;
    return plan;
  }

  Vector<Character> text;
  text.Resize(characterCount);
  std::copy(model.GetTextBuffer(), model.GetTextBuffer() + characterCount, text.Begin());

  Vector<WordBreakInfo> adjustedWordBreakInfo;
  if(unit == Unit::WORD && eligibleImageGlyphs && wordBreakInfo)
  {
    adjustedWordBreakInfo.Resize(characterCount);
    std::copy(wordBreakInfo, wordBreakInfo + characterCount, adjustedWordBreakInfo.Begin());
    for(Length glyph = 0u; glyph < glyphCount; ++glyph)
    {
      if(glyph >= eligibleImageGlyphs->size() || !(*eligibleImageGlyphs)[glyph])
      {
        continue;
      }
      const CharacterIndex character = glyphMap[glyph];
      if(character < characterCount)
      {
        adjustedWordBreakInfo[character] = TextAbstraction::WORD_BREAK;
        if(character > 0u)
        {
          adjustedWordBreakInfo[character - 1u] = TextAbstraction::WORD_BREAK;
        }
      }
    }
    wordBreakInfo = adjustedWordBreakInfo.Begin();
  }

  Plan plan = BuildPlan(text.Begin(), characterCount, glyphMap.Begin(), glyphCount, unit, fadeDurationRatio, wordBreakInfo);
  FilterSyntheticReplacementGlyphs(plan, model.GetGlyphs(), glyphCount, eligibleImageGlyphs);
  return plan;
}

std::vector<bool> BuildEligibleImageGlyphs(const ModelInterface&               model,
                                           const ReplacementSourceSnapshot&    source,
                                           const Vector<ReplacementPlacement>& placements)
{
  std::vector<bool> eligible;
  for(const ReplacementPlacement& placement : placements)
  {
    if(placement.syntheticGlyphIndex >= model.GetNumberOfGlyphs() ||
       placement.sourceRunIndex >= source.runs.Count())
    {
      continue;
    }
    const ReplacementRunSnapshot& run = source.runs[placement.sourceRunIndex];
    if(run.type == ReplacementType::IMAGE && !run.image.source.empty() &&
       run.occurrenceIdentity == placement.occurrenceIdentity)
    {
      if(eligible.empty())
      {
        eligible.assign(model.GetNumberOfGlyphs(), false);
      }
      eligible[placement.syntheticGlyphIndex] = true;
    }
  }
  return eligible;
}

} // unnamed namespace

Plan BuildPlan(const ModelInterface&          model,
               Unit                           unit,
               float                          fadeDurationRatio,
               TextAbstraction::Segmentation& segmentation)
{
  if(unit != Unit::WORD)
  {
    return BuildModelPlan(model, unit, fadeDurationRatio, nullptr);
  }

  const Length          characterCount = model.GetNumberOfCharacters();
  Vector<WordBreakInfo> wordBreakInfo;
  wordBreakInfo.Resize(characterCount);
  if(characterCount > 0u)
  {
    std::fill(wordBreakInfo.Begin(), wordBreakInfo.End(), TextAbstraction::WORD_NO_BREAK);
    segmentation.GetWordBreakPositions(model.GetTextBuffer(), characterCount, wordBreakInfo.Begin());
  }
  return BuildModelPlan(model, unit, fadeDurationRatio,
                        wordBreakInfo.Count() == characterCount ? wordBreakInfo.Begin() : nullptr);
}

Plan BuildPlanWithImageReplacements(const ModelInterface&               model,
                                    Unit                                unit,
                                    float                               fadeDurationRatio,
                                    TextAbstraction::Segmentation&      segmentation,
                                    const ReplacementSourceSnapshot&    source,
                                    const Vector<ReplacementPlacement>& placements)
{
  const std::vector<bool> eligibleImageGlyphs = BuildEligibleImageGlyphs(model, source, placements);
  if(unit != Unit::WORD)
  {
    return BuildModelPlan(model, unit, fadeDurationRatio, nullptr, &eligibleImageGlyphs);
  }

  const Length          characterCount = model.GetNumberOfCharacters();
  Vector<WordBreakInfo> wordBreakInfo;
  wordBreakInfo.Resize(characterCount);
  if(characterCount > 0u)
  {
    std::fill(wordBreakInfo.Begin(), wordBreakInfo.End(), TextAbstraction::WORD_NO_BREAK);
    segmentation.GetWordBreakPositions(model.GetTextBuffer(), characterCount, wordBreakInfo.Begin());
  }
  return BuildModelPlan(model,
                        unit,
                        fadeDurationRatio,
                        wordBreakInfo.Count() == characterCount ? wordBreakInfo.Begin() : nullptr,
                        &eligibleImageGlyphs);
}

Plan BuildCharacterPlan(const ModelInterface& model, float fadeDurationRatio)
{
  return BuildModelPlan(model, Unit::CHARACTER, fadeDurationRatio, nullptr);
}

Plan BuildLinePlan(const ModelInterface& model, float fadeDurationRatio)
{
  return BuildModelPlan(model, Unit::LINE, fadeDurationRatio, nullptr);
}

Plan BuildPixelPlan(const ModelInterface& model, float fadeDurationRatio)
{
  return BuildModelPlan(model, Unit::PIXEL, fadeDurationRatio, nullptr);
}

Plan ProjectToFinalGlyphs(const Plan&       sourcePlan,
                          Length            finalGlyphCount,
                          const GlyphIndex* finalToSourceGlyph,
                          GlyphIndex        ellipsisFinalGlyph,
                          Unit              unit)
{
  Plan finalPlan;
  finalPlan.fadeDurationRatio = sourcePlan.fadeDurationRatio;
  finalPlan.glyphToUnit.assign(finalGlyphCount, NO_UNIT);

  const uint32_t    oldUnitCount = sourcePlan.GetUnitCount();
  std::vector<bool> visibleOld(oldUnitCount, false);
  for(GlyphIndex finalGlyph = 0u; finalGlyph < finalGlyphCount; ++finalGlyph)
  {
    if(finalGlyph == ellipsisFinalGlyph)
    {
      continue;
    }
    const GlyphIndex sourceGlyph = finalToSourceGlyph ? finalToSourceGlyph[finalGlyph] : finalGlyph;
    if(sourceGlyph >= sourcePlan.glyphToUnit.size())
    {
      continue;
    }
    const uint32_t oldUnit = sourcePlan.glyphToUnit[sourceGlyph];
    if(oldUnit != NO_UNIT && oldUnit < oldUnitCount)
    {
      visibleOld[oldUnit] = true;
    }
  }

  std::vector<uint32_t> visibleOldUnits;
  visibleOldUnits.reserve(oldUnitCount);
  for(uint32_t oldUnit = 0u; oldUnit < oldUnitCount; ++oldUnit)
  {
    if(visibleOld[oldUnit])
    {
      visibleOldUnits.push_back(oldUnit);
    }
  }

  const bool hasEllipsis                = ellipsisFinalGlyph < finalGlyphCount;
  const bool ellipsisFollowsReplacement = hasEllipsis && unit == Unit::WORD && !visibleOldUnits.empty() &&
                                          visibleOldUnits.back() < sourcePlan.imageReplacementUnitMask.size() &&
                                          sourcePlan.imageReplacementUnitMask[visibleOldUnits.back()] != 0u;

  std::vector<uint32_t> oldToNew(oldUnitCount, NO_UNIT);
  uint32_t              finalUnitCount = static_cast<uint32_t>(visibleOldUnits.size());
  if(hasEllipsis && (unit == Unit::CHARACTER || unit == Unit::LINE || unit == Unit::PIXEL || ellipsisFollowsReplacement))
  {
    ++finalUnitCount;
  }
  else if(hasEllipsis && visibleOldUnits.empty())
  {
    finalUnitCount = 1u;
  }

  for(uint32_t index = 0u; index < visibleOldUnits.size(); ++index)
  {
    oldToNew[visibleOldUnits[index]] = index;
  }
  if(!sourcePlan.imageReplacementUnitMask.empty())
  {
    for(uint32_t index = 0u; index < visibleOldUnits.size(); ++index)
    {
      const uint32_t oldUnit = visibleOldUnits[index];
      if(oldUnit < sourcePlan.imageReplacementUnitMask.size() &&
         sourcePlan.imageReplacementUnitMask[oldUnit] != 0u)
      {
        if(finalPlan.imageReplacementUnitMask.empty())
        {
          finalPlan.imageReplacementUnitMask.assign(finalUnitCount, 0u);
        }
        finalPlan.imageReplacementUnitMask[index] = 1u;
      }
    }
  }

  for(GlyphIndex finalGlyph = 0u; finalGlyph < finalGlyphCount; ++finalGlyph)
  {
    if(finalGlyph == ellipsisFinalGlyph)
    {
      continue;
    }
    const GlyphIndex sourceGlyph = finalToSourceGlyph ? finalToSourceGlyph[finalGlyph] : finalGlyph;
    if(sourceGlyph < sourcePlan.glyphToUnit.size())
    {
      const uint32_t oldUnit = sourcePlan.glyphToUnit[sourceGlyph];
      if(oldUnit != NO_UNIT && oldUnit < oldToNew.size())
      {
        finalPlan.glyphToUnit[finalGlyph] = oldToNew[oldUnit];
      }
    }
  }

  if(hasEllipsis)
  {
    if(unit == Unit::CHARACTER || unit == Unit::LINE || unit == Unit::PIXEL || visibleOldUnits.empty() || ellipsisFollowsReplacement)
    {
      finalPlan.glyphToUnit[ellipsisFinalGlyph] = static_cast<uint32_t>(visibleOldUnits.size());
    }
    else
    {
      // END ellipsis follows closing-punctuation behavior and joins the last
      // preceding visible word.
      finalPlan.glyphToUnit[ellipsisFinalGlyph] = static_cast<uint32_t>(visibleOldUnits.size() - 1u);
    }
  }

  PopulateSchedule(finalPlan, finalUnitCount, finalPlan.fadeDurationRatio);
  return finalPlan;
}

bool ApplyLineUnitSchedule(Plan&          plan,
                           const LineRun* lines,
                           Length         lineCount)
{
  const uint32_t oldUnitCount = plan.GetUnitCount();
  const uint32_t glyphCount   = static_cast<uint32_t>(plan.glyphToUnit.size());
  if(oldUnitCount == 0u)
  {
    return std::all_of(plan.glyphToUnit.begin(), plan.glyphToUnit.end(), [](uint32_t unit)
    {
      return unit == NO_UNIT;
    });
  }
  if(glyphCount == 0u || !lines || lineCount == 0u)
  {
    return false;
  }
  if(!plan.imageReplacementUnitMask.empty() && plan.imageReplacementUnitMask.size() != oldUnitCount)
  {
    return false;
  }

  std::vector<uint32_t> glyphToLine(glyphCount, NO_UNIT);
  auto                  mapRun = [&](uint32_t lineIndex, const GlyphRun& run)
  {
    const uint32_t begin = run.glyphIndex;
    const uint32_t count = run.numberOfGlyphs;
    if(begin > glyphCount || count > glyphCount - begin)
    {
      return false;
    }
    for(uint32_t glyph = begin; glyph < begin + count; ++glyph)
    {
      if(glyphToLine[glyph] != NO_UNIT && glyphToLine[glyph] != lineIndex)
      {
        return false;
      }
      glyphToLine[glyph] = lineIndex;
    }
    return true;
  };

  for(uint32_t lineIndex = 0u; lineIndex < lineCount; ++lineIndex)
  {
    if(!mapRun(lineIndex, lines[lineIndex].glyphRun) ||
       (lines[lineIndex].isSplitToTwoHalves && !mapRun(lineIndex, lines[lineIndex].glyphRunSecondHalf)))
    {
      return false;
    }
  }

  std::vector<uint8_t> activeLines(lineCount, 0u);
  std::vector<uint8_t> backedUnits(oldUnitCount, 0u);
  for(uint32_t glyph = 0u; glyph < glyphCount; ++glyph)
  {
    const uint32_t oldUnit = plan.glyphToUnit[glyph];
    if(oldUnit == NO_UNIT)
    {
      continue;
    }
    if(oldUnit >= oldUnitCount || glyphToLine[glyph] == NO_UNIT)
    {
      return false;
    }
    backedUnits[oldUnit]            = 1u;
    activeLines[glyphToLine[glyph]] = 1u;
  }
  if(std::find(backedUnits.begin(), backedUnits.end(), 0u) != backedUnits.end())
  {
    return false;
  }

  std::vector<uint32_t> lineToUnit(lineCount, NO_UNIT);
  uint32_t              lineUnitCount = 0u;
  for(uint32_t lineIndex = 0u; lineIndex < lineCount; ++lineIndex)
  {
    if(activeLines[lineIndex] != 0u)
    {
      lineToUnit[lineIndex] = lineUnitCount++;
    }
  }

  Plan linePlan;
  linePlan.fadeDurationRatio = plan.fadeDurationRatio;
  linePlan.glyphToUnit.assign(glyphCount, NO_UNIT);
  if(!plan.imageReplacementUnitMask.empty())
  {
    linePlan.imageReplacementUnitMask.assign(lineUnitCount, 0u);
  }
  for(uint32_t glyph = 0u; glyph < glyphCount; ++glyph)
  {
    const uint32_t oldUnit = plan.glyphToUnit[glyph];
    if(oldUnit == NO_UNIT)
    {
      continue;
    }
    const uint32_t lineUnit     = lineToUnit[glyphToLine[glyph]];
    linePlan.glyphToUnit[glyph] = lineUnit;
    if(oldUnit < plan.imageReplacementUnitMask.size() && plan.imageReplacementUnitMask[oldUnit] != 0u)
    {
      linePlan.imageReplacementUnitMask[lineUnit] = 1u;
    }
  }
  PopulateSchedule(linePlan, lineUnitCount, linePlan.fadeDurationRatio);
  plan = std::move(linePlan);
  return true;
}

bool ApplyPixelSpatialSchedule(Plan&                       plan,
                               const ModelInterface&       finalModel,
                               TextAbstraction::FontClient fontClient,
                               const GlyphIndex*           finalToSourceGlyph,
                               GlyphIndex                  ellipsisFinalGlyph,
                               Sequence                    sequence,
                               float                       sequenceStaggerRatio)
{
  const uint32_t   oldUnitCount = plan.GetUnitCount();
  const uint32_t   glyphCount   = static_cast<uint32_t>(plan.glyphToUnit.size());
  const Length     lineCount    = finalModel.GetNumberOfLines();
  const LineRun*   lines        = finalModel.GetLines();
  const GlyphInfo* glyphs       = finalModel.GetGlyphs();
  const Vector2*   positions    = finalModel.GetLayout();
  if(oldUnitCount == 0u || glyphCount == 0u)
  {
    return true;
  }
  if(!lines || lineCount == 0u || !glyphs || !positions || finalModel.GetNumberOfGlyphs() != glyphCount)
  {
    return false;
  }

  // All PIXEL-specific mutations are built separately and committed only
  // after every mapping, geometry, direction, and timing check succeeds.
  std::vector<uint32_t> glyphToUnit(plan.glyphToUnit);
  std::vector<uint32_t> glyphToLine(glyphCount, NO_UNIT);
  auto                  mapLineRun = [&](uint32_t lineIndex, const GlyphRun& run)
  {
    const uint32_t begin = run.glyphIndex;
    const uint32_t count = run.numberOfGlyphs;
    if(begin > glyphCount || count > glyphCount - begin)
    {
      return false;
    }
    for(uint32_t glyph = begin; glyph < begin + count; ++glyph)
    {
      if(glyphToLine[glyph] != NO_UNIT && glyphToLine[glyph] != lineIndex)
      {
        return false;
      }
      glyphToLine[glyph] = lineIndex;
    }
    return true;
  };

  for(uint32_t line = 0u; line < lineCount; ++line)
  {
    if(!mapLineRun(line, lines[line].glyphRun) ||
       (lines[line].isSplitToTwoHalves && !mapLineRun(line, lines[line].glyphRunSecondHalf)))
    {
      return false;
    }
  }
  for(uint32_t glyph = 0u; glyph < glyphCount; ++glyph)
  {
    if(glyphToUnit[glyph] != NO_UNIT && glyphToLine[glyph] == NO_UNIT)
    {
      return false;
    }
  }

  struct LineUnitPair
  {
    uint32_t line;
    uint32_t oldUnit;
    uint32_t originalIndex;
  };

  uint32_t              unitCount = oldUnitCount;
  std::vector<uint32_t> unitLine;
  std::vector<uint8_t>  imageReplacementUnitMask = plan.imageReplacementUnitMask;
  if(sequence == Sequence::PER_LINE)
  {
    std::vector<LineUnitPair> pairs;
    std::vector<uint32_t>     glyphToPair(glyphCount, NO_UNIT);
    std::vector<uint32_t>     lastLineForUnit(oldUnitCount, NO_UNIT);
    std::vector<uint32_t>     pairForUnit(oldUnitCount, NO_UNIT);
    for(uint32_t line = 0u; line < lineCount; ++line)
    {
      auto collectRun = [&](const GlyphRun& run)
      {
        for(uint32_t glyph = run.glyphIndex; glyph < run.glyphIndex + run.numberOfGlyphs; ++glyph)
        {
          const uint32_t oldUnit = glyphToUnit[glyph];
          if(oldUnit == NO_UNIT)
          {
            continue;
          }
          if(oldUnit >= oldUnitCount)
          {
            return false;
          }
          if(lastLineForUnit[oldUnit] != line)
          {
            const uint32_t pairIndex = static_cast<uint32_t>(pairs.size());
            pairs.push_back({line, oldUnit, pairIndex});
            lastLineForUnit[oldUnit] = line;
            pairForUnit[oldUnit]     = pairIndex;
          }
          glyphToPair[glyph] = pairForUnit[oldUnit];
        }
        return true;
      };
      if(!collectRun(lines[line].glyphRun) ||
         (lines[line].isSplitToTwoHalves && !collectRun(lines[line].glyphRunSecondHalf)))
      {
        return false;
      }
    }

    std::stable_sort(pairs.begin(), pairs.end(), [](const LineUnitPair& lhs, const LineUnitPair& rhs)
    {
      return lhs.line < rhs.line || (lhs.line == rhs.line && lhs.oldUnit < rhs.oldUnit);
    });
    std::vector<uint32_t> newUnitByPair(pairs.size(), NO_UNIT);
    if(!plan.imageReplacementUnitMask.empty())
    {
      imageReplacementUnitMask.assign(pairs.size(), 0u);
    }
    unitLine.resize(pairs.size(), NO_UNIT);
    for(uint32_t newUnit = 0u; newUnit < pairs.size(); ++newUnit)
    {
      newUnitByPair[pairs[newUnit].originalIndex] = newUnit;
      unitLine[newUnit]                           = pairs[newUnit].line;
      if(pairs[newUnit].oldUnit < plan.imageReplacementUnitMask.size())
      {
        imageReplacementUnitMask[newUnit] = plan.imageReplacementUnitMask[pairs[newUnit].oldUnit];
      }
    }
    for(uint32_t glyph = 0u; glyph < glyphCount; ++glyph)
    {
      if(glyphToPair[glyph] != NO_UNIT)
      {
        glyphToUnit[glyph] = newUnitByPair[glyphToPair[glyph]];
      }
    }
    unitCount = static_cast<uint32_t>(pairs.size());
  }
  else
  {
    unitLine.assign(unitCount, NO_UNIT);
    for(uint32_t glyph = 0u; glyph < glyphCount; ++glyph)
    {
      const uint32_t unit = glyphToUnit[glyph];
      if(unit != NO_UNIT && unit < unitCount && unitLine[unit] == NO_UNIT)
      {
        unitLine[unit] = glyphToLine[glyph];
      }
    }
  }

  struct UnitAccumulator
  {
    float          visualMinimum{std::numeric_limits<float>::max()};
    float          visualMaximum{-std::numeric_limits<float>::max()};
    float          advance{0.0f};
    CharacterIndex logicalCharacter{std::numeric_limits<CharacterIndex>::max()};
    GlyphIndex     finalGlyph{std::numeric_limits<GlyphIndex>::max()};
    bool           rightToLeft{false};
    bool           directionKnown{false};
    bool           hasGlyph{false};
  };

  std::vector<UnitAccumulator> accumulators(unitCount);
  const auto&                  glyphToCharacter = finalModel.GetGlyphsToCharacters();
  const auto&                  directions       = finalModel.GetCharacterDirections();
  const auto&                  spacingRuns      = finalModel.GetCharacterSpacingGlyphRuns();
  const Character*             text             = finalModel.GetTextBuffer();
  const Length                 characterCount   = finalModel.GetNumberOfCharacters();
  const float                  defaultSpacing   = finalModel.GetCharacterSpacing();

  auto getSourceGlyph = [&](GlyphIndex finalGlyph)
  {
    if(finalGlyph == ellipsisFinalGlyph)
    {
      return std::numeric_limits<GlyphIndex>::max();
    }
    return finalToSourceGlyph ? finalToSourceGlyph[finalGlyph] : finalGlyph;
  };
  auto getSourceCharacter = [&](GlyphIndex finalGlyph)
  {
    const GlyphIndex sourceGlyph = getSourceGlyph(finalGlyph);
    return sourceGlyph < glyphToCharacter.Count()
             ? glyphToCharacter[sourceGlyph]
             : std::numeric_limits<CharacterIndex>::max();
  };
  auto getAdvance = [&](GlyphIndex finalGlyph)
  {
    const GlyphIndex     sourceGlyph = getSourceGlyph(finalGlyph);
    const CharacterIndex character   = getSourceCharacter(finalGlyph);
    float                advance     = glyphs[finalGlyph].advance;
    if(sourceGlyph < glyphToCharacter.Count() && character < characterCount && text)
    {
      const float spacing = GetGlyphCharacterSpacing(sourceGlyph, spacingRuns, defaultSpacing);
      advance             = GetCalculatedAdvance(text[character], spacing, advance);
    }
    return std::max(0.0f, advance);
  };

  for(uint32_t glyph = 0u; glyph < glyphCount; ++glyph)
  {
    const uint32_t unit = glyphToUnit[glyph];
    if(unit == NO_UNIT)
    {
      continue;
    }
    if(unit >= unitCount)
    {
      return false;
    }

    UnitAccumulator& accumulator = accumulators[unit];
    accumulator.visualMinimum    = std::min(accumulator.visualMinimum, positions[glyph].x);
    accumulator.visualMaximum    = std::max(accumulator.visualMaximum, positions[glyph].x + glyphs[glyph].width);
    accumulator.advance += getAdvance(glyph);
    accumulator.finalGlyph = std::min(accumulator.finalGlyph, static_cast<GlyphIndex>(glyph));
    accumulator.hasGlyph   = true;

    const CharacterIndex character = getSourceCharacter(glyph);
    if(character < characterCount)
    {
      accumulator.logicalCharacter = std::min(accumulator.logicalCharacter, character);
      if(!accumulator.directionKnown && character < directions.Count())
      {
        accumulator.rightToLeft    = directions[character];
        accumulator.directionKnown = true;
      }
    }
  }

  for(uint32_t unit = 0u; unit < unitCount; ++unit)
  {
    UnitAccumulator& accumulator = accumulators[unit];
    if(!accumulator.hasGlyph || unitLine[unit] == NO_UNIT)
    {
      return false;
    }
    if(!(accumulator.visualMaximum > accumulator.visualMinimum))
    {
      accumulator.visualMaximum = accumulator.visualMinimum + std::max(1.0f, accumulator.advance);
    }
    if(!accumulator.directionKnown)
    {
      const uint32_t   line   = unitLine[unit];
      const GlyphIndex anchor = accumulator.finalGlyph;
      // Pure unidirectional models may omit the direction table entirely. In
      // that compact representation LineRun::direction is authoritative, so
      // avoid an otherwise quadratic search for neighbors that cannot exist.
      for(uint32_t distance = 1u;
          !directions.Empty() && distance < glyphCount && !accumulator.directionKnown;
          ++distance)
      {
        for(int side = -1; side <= 1; side += 2)
        {
          const int64_t candidate = static_cast<int64_t>(anchor) + static_cast<int64_t>(side) * distance;
          if(candidate < 0 || candidate >= glyphCount)
          {
            continue;
          }
          const GlyphIndex candidateGlyph = static_cast<GlyphIndex>(candidate);
          if(glyphToLine[candidateGlyph] != line)
          {
            continue;
          }
          const CharacterIndex character = getSourceCharacter(candidateGlyph);
          if(character < directions.Count())
          {
            accumulator.rightToLeft    = directions[character];
            accumulator.directionKnown = true;
            break;
          }
        }
      }
      if(!accumulator.directionKnown)
      {
        accumulator.rightToLeft    = lines[line].direction;
        accumulator.directionKnown = true;
      }
    }
  }

  std::vector<float> visualWeight(unitCount, 0.0f);
  for(uint32_t unit = 0u; unit < unitCount; ++unit)
  {
    const UnitAccumulator& accumulator = accumulators[unit];
    visualWeight[unit]                 = std::max(accumulator.advance,
                                                  accumulator.visualMaximum - accumulator.visualMinimum);
    visualWeight[unit]                 = std::max(0.001f, visualWeight[unit]);
  }

  std::vector<float> gapBefore(unitCount, 0.0f);
  if(text)
  {
    struct LogicalUnit
    {
      CharacterIndex character;
      uint32_t       unit;
      uint32_t       line;
    };
    std::vector<LogicalUnit> logicalUnits;
    logicalUnits.reserve(unitCount);
    for(uint32_t unit = 0u; unit < unitCount; ++unit)
    {
      if(accumulators[unit].logicalCharacter < characterCount)
      {
        logicalUnits.push_back({accumulators[unit].logicalCharacter, unit, unitLine[unit]});
      }
    }
    std::sort(logicalUnits.begin(), logicalUnits.end(), [](const LogicalUnit& lhs, const LogicalUnit& rhs)
    {
      return lhs.character < rhs.character || (lhs.character == rhs.character && lhs.unit < rhs.unit);
    });

    for(uint32_t glyph = 0u; glyph < glyphCount; ++glyph)
    {
      if(glyphToUnit[glyph] != NO_UNIT || glyphToLine[glyph] == NO_UNIT)
      {
        continue;
      }
      const CharacterIndex character = getSourceCharacter(glyph);
      if(character >= characterCount || !TextAbstraction::IsWhiteSpace(text[character]))
      {
        continue;
      }
      auto next        = std::lower_bound(logicalUnits.begin(), logicalUnits.end(), character,
                                          [](const LogicalUnit& entry, CharacterIndex value)
             {
        return entry.character < value;
      });
      auto previous    = next;
      bool hasPrevious = false;
      while(previous != logicalUnits.begin())
      {
        --previous;
        if(previous->line == glyphToLine[glyph])
        {
          hasPrevious = true;
          break;
        }
      }
      while(next != logicalUnits.end() && next->line != glyphToLine[glyph])
      {
        ++next;
      }
      if(hasPrevious && next != logicalUnits.end())
      {
        gapBefore[next->unit] += getAdvance(glyph);
      }
    }
  }

  const std::vector<float>& scheduleWeight = visualWeight;

  std::vector<std::vector<uint32_t>> unitsByLine(lineCount);
  for(uint32_t unit = 0u; unit < unitCount; ++unit)
  {
    unitsByLine[unitLine[unit]].push_back(unit);
  }

  std::vector<float> lineScheduleWeight(lineCount, 0.0f);
  uint32_t           activeSequenceCount = 0u;
  float              maxScheduleWeight   = 0.0f;
  for(uint32_t line = 0u; line < lineCount; ++line)
  {
    if(unitsByLine[line].empty())
    {
      continue;
    }
    ++activeSequenceCount;
    for(uint32_t unit : unitsByLine[line])
    {
      lineScheduleWeight[line] += gapBefore[unit] + scheduleWeight[unit];
    }
    maxScheduleWeight = std::max(maxScheduleWeight, lineScheduleWeight[line]);
  }

  float referenceScheduleWeight = maxScheduleWeight;
  if(sequence == Sequence::WHOLE_TEXT)
  {
    referenceScheduleWeight = 0.0f;
    for(uint32_t line = 0u; line < lineCount; ++line)
    {
      referenceScheduleWeight += lineScheduleWeight[line];
    }
    activeSequenceCount = unitCount > 0u ? 1u : 0u;
  }
  if(!(referenceScheduleWeight > 0.0f) || activeSequenceCount == 0u)
  {
    return false;
  }

  float fadeDuration = plan.fadeDurationRatio;
  if(fadeDuration == Text::Reveal::AUTO_FADE_DURATION_RATIO)
  {
    bool hasInlineReplacement = false;
    for(uint32_t glyph = 0u; glyph < glyphCount && !hasInlineReplacement; ++glyph)
    {
      hasInlineReplacement = IsSyntheticReplacementGlyph(glyphs[glyph]);
    }

    // PER_LINE sequences are independent, so AUTO must pair each line's
    // progression distance with that line's own text scale. WHOLE_TEXT keeps
    // the existing global reference because it is one sequence across all
    // final lines.
    const bool         usePerLinePairedAuto = sequence == Sequence::PER_LINE;
    std::vector<float> lineReferenceHeight;
    float              representativeHeight = ResolveTextForegroundReferencePixelSize(
      finalModel,
      hasInlineReplacement,
      fontClient,
      usePerLinePairedAuto ? &lineReferenceHeight : nullptr);
    const bool needsReplacementOnlyReference = !(representativeHeight > 0.0f) ||
                                               !std::isfinite(representativeHeight);
    std::vector<bool> replacementOnlyLine;
    if(usePerLinePairedAuto)
    {
      replacementOnlyLine.resize(lineReferenceHeight.size(), false);
      for(uint32_t line = 0u; line < lineReferenceHeight.size(); ++line)
      {
        if(!(lineReferenceHeight[line] > 0.0f) || !std::isfinite(lineReferenceHeight[line]))
        {
          lineReferenceHeight[line] = 0.0f;
          replacementOnlyLine[line] = true;
        }
      }
    }
    if(hasInlineReplacement && (needsReplacementOnlyReference || usePerLinePairedAuto))
    {
      // Keep text-backed references unchanged. An ImageSpan-only label, or an
      // ImageSpan-only active line inside mixed content, uses its authored
      // reserved box height as the local spatial reference.
      if(needsReplacementOnlyReference)
      {
        representativeHeight = 0.0f;
      }
      for(uint32_t glyph = 0u; glyph < glyphCount; ++glyph)
      {
        if(glyphToUnit[glyph] == NO_UNIT || !IsSyntheticReplacementGlyph(glyphs[glyph]))
        {
          continue;
        }
        const float reservedHeight = std::max(0.0f, glyphs[glyph].height);
        if(needsReplacementOnlyReference)
        {
          representativeHeight = std::max(representativeHeight, reservedHeight);
        }
        if(usePerLinePairedAuto && glyphToLine[glyph] < lineReferenceHeight.size() &&
           replacementOnlyLine[glyphToLine[glyph]])
        {
          lineReferenceHeight[glyphToLine[glyph]] =
            std::max(lineReferenceHeight[glyphToLine[glyph]], reservedHeight);
        }
      }
    }
    if(!(representativeHeight > 0.0f) || !std::isfinite(representativeHeight))
    {
      return false;
    }

    float effectiveUnitCount = referenceScheduleWeight / representativeHeight;
    if(usePerLinePairedAuto)
    {
      effectiveUnitCount = 0.0f;
      for(uint32_t line = 0u; line < lineCount; ++line)
      {
        if(unitsByLine[line].empty())
        {
          continue;
        }
        if(line >= lineReferenceHeight.size() || !(lineReferenceHeight[line] > 0.0f) ||
           !std::isfinite(lineReferenceHeight[line]))
        {
          return false;
        }
        effectiveUnitCount = std::max(effectiveUnitCount,
                                      lineScheduleWeight[line] / lineReferenceHeight[line]);
      }
    }
    fadeDuration = ResolveSpatialFadeDurationRatio(fadeDuration, effectiveUnitCount);
  }
  if(!std::isfinite(fadeDuration))
  {
    return false;
  }

  sequenceStaggerRatio = std::isnan(sequenceStaggerRatio)
                           ? 0.0f
                           : std::max(0.0f, std::min(1.0f, sequenceStaggerRatio));
  float totalDuration  = 1.0f;
  if(sequence == Sequence::PER_LINE)
  {
    totalDuration           = 0.0f;
    uint32_t activeSequence = 0u;
    for(uint32_t line = 0u; line < lineCount; ++line)
    {
      if(unitsByLine[line].empty())
      {
        continue;
      }
      const float offset = static_cast<float>(activeSequence++) * sequenceStaggerRatio;
      const float span   = (1.0f - fadeDuration) * lineScheduleWeight[line] / referenceScheduleWeight + fadeDuration;
      totalDuration      = std::max(totalDuration, offset + span);
    }
  }
  if(!(totalDuration > 0.0f) || !std::isfinite(totalDuration))
  {
    return false;
  }

  std::vector<float>           unitStart(unitCount, 0.0f);
  std::vector<PixelUnitTiming> pixelUnitTiming(unitCount);
  const float                  resolvedFadeDuration = fadeDuration / totalDuration;

  float    textCursor     = 0.0f;
  uint32_t activeSequence = 0u;
  for(uint32_t line = 0u; line < lineCount; ++line)
  {
    if(unitsByLine[line].empty())
    {
      continue;
    }
    const float offset = sequence == Sequence::PER_LINE
                           ? static_cast<float>(activeSequence++) * sequenceStaggerRatio
                           : 0.0f;
    float       cursor = sequence == Sequence::PER_LINE ? 0.0f : textCursor;
    for(uint32_t unit : unitsByLine[line])
    {
      cursor += gapBefore[unit];
      unitStart[unit]       = (offset + (1.0f - fadeDuration) * cursor / referenceScheduleWeight) / totalDuration;
      pixelUnitTiming[unit] = {
        accumulators[unit].visualMinimum,
        accumulators[unit].visualMaximum,
        (1.0f - fadeDuration) * scheduleWeight[unit] / referenceScheduleWeight / totalDuration,
        accumulators[unit].rightToLeft};
      cursor += scheduleWeight[unit];
    }
    if(sequence == Sequence::WHOLE_TEXT)
    {
      textCursor = cursor;
    }
  }

  plan.glyphToUnit              = std::move(glyphToUnit);
  plan.unitStart                = std::move(unitStart);
  plan.pixelUnitTiming          = std::move(pixelUnitTiming);
  plan.imageReplacementUnitMask = std::move(imageReplacementUnitMask);
  plan.fadeDuration             = resolvedFadeDuration;
  return true;
}

float ResolvePixelStart(const Plan& plan, uint32_t unit, float visualX)
{
  if(unit >= plan.unitStart.size())
  {
    return 0.0f;
  }
  if(unit >= plan.pixelUnitTiming.size())
  {
    return plan.unitStart[unit];
  }

  const PixelUnitTiming& timing = plan.pixelUnitTiming[unit];
  const float            extent = timing.visualMaximum - timing.visualMinimum;
  float                  local  = extent > 0.0f ? (visualX - timing.visualMinimum) / extent : 0.0f;
  local                         = std::max(0.0f, std::min(1.0f, local));
  if(timing.rightToLeft)
  {
    local = 1.0f - local;
  }
  return plan.unitStart[unit] + timing.progressionSpan * local;
}

bool ApplyPerLineSequenceSchedule(Plan&          plan,
                                  const LineRun* lines,
                                  Length         lineCount,
                                  float          sequenceStaggerRatio)
{
  const uint32_t oldUnitCount = plan.GetUnitCount();
  const uint32_t glyphCount   = static_cast<uint32_t>(plan.glyphToUnit.size());
  if(oldUnitCount == 0u || glyphCount == 0u)
  {
    return true;
  }
  if(!lines || lineCount == 0u)
  {
    return false;
  }

  struct LineUnitPair
  {
    uint32_t lineIndex;
    uint32_t oldUnit;
    uint32_t originalIndex;
  };

  std::vector<LineUnitPair> pairs;
  pairs.reserve(std::min(static_cast<size_t>(glyphCount),
                         static_cast<size_t>(oldUnitCount) + static_cast<size_t>(lineCount)));
  std::vector<uint32_t> glyphToPair(glyphCount, NO_UNIT);
  std::vector<uint32_t> lastLineForUnit(oldUnitCount, NO_UNIT);
  std::vector<uint32_t> pairForUnit(oldUnitCount, NO_UNIT);
  std::vector<uint32_t> lineUnitCount(lineCount, 0u);

  auto mapRun = [&](uint32_t lineIndex, const GlyphRun& run)
  {
    const uint32_t begin = run.glyphIndex;
    const uint32_t count = run.numberOfGlyphs;
    if(begin > glyphCount || count > glyphCount - begin)
    {
      return false;
    }

    for(uint32_t glyph = begin; glyph < begin + count; ++glyph)
    {
      const uint32_t oldUnit = plan.glyphToUnit[glyph];
      if(oldUnit == NO_UNIT)
      {
        continue;
      }
      if(oldUnit >= oldUnitCount)
      {
        return false;
      }

      if(glyphToPair[glyph] != NO_UNIT)
      {
        const LineUnitPair& existing = pairs[glyphToPair[glyph]];
        if(existing.lineIndex != lineIndex || existing.oldUnit != oldUnit)
        {
          return false;
        }
        continue;
      }

      if(lastLineForUnit[oldUnit] != lineIndex)
      {
        const uint32_t pairIndex = static_cast<uint32_t>(pairs.size());
        pairs.push_back({lineIndex, oldUnit, pairIndex});
        lastLineForUnit[oldUnit] = lineIndex;
        pairForUnit[oldUnit]     = pairIndex;
        ++lineUnitCount[lineIndex];
      }
      glyphToPair[glyph] = pairForUnit[oldUnit];
    }
    return true;
  };

  for(uint32_t lineIndex = 0u; lineIndex < lineCount; ++lineIndex)
  {
    if(!mapRun(lineIndex, lines[lineIndex].glyphRun) ||
       (lines[lineIndex].isSplitToTwoHalves && !mapRun(lineIndex, lines[lineIndex].glyphRunSecondHalf)))
    {
      return false;
    }
  }

  for(uint32_t glyph = 0u; glyph < glyphCount; ++glyph)
  {
    if(plan.glyphToUnit[glyph] != NO_UNIT && glyphToPair[glyph] == NO_UNIT)
    {
      // A final unit outside the supplied line runs cannot be scheduled
      // without guessing its visual line. Keep the existing WHOLE_TEXT plan.
      return false;
    }
  }

  uint32_t activeSequenceCount  = 0u;
  uint32_t maxSequenceUnitCount = 0u;
  for(uint32_t count : lineUnitCount)
  {
    if(count > 0u)
    {
      ++activeSequenceCount;
      maxSequenceUnitCount = std::max(maxSequenceUnitCount, count);
    }
  }
  if(activeSequenceCount <= 1u)
  {
    return true;
  }

  // Stable counting sorts produce (line, oldUnit) order in O(pairs + units +
  // lines). This preserves the existing logical unit order inside a line even
  // when its glyphs are visually reordered by bidi layout.
  std::vector<LineUnitPair> byOldUnit(pairs.size());
  std::vector<uint32_t>     counts(oldUnitCount + 1u, 0u);
  for(const LineUnitPair& pair : pairs)
  {
    ++counts[pair.oldUnit + 1u];
  }
  for(uint32_t index = 1u; index < counts.size(); ++index)
  {
    counts[index] += counts[index - 1u];
  }
  for(const LineUnitPair& pair : pairs)
  {
    byOldUnit[counts[pair.oldUnit]++] = pair;
  }

  std::vector<LineUnitPair> orderedPairs(pairs.size());
  counts.assign(static_cast<uint32_t>(lineCount) + 1u, 0u);
  for(const LineUnitPair& pair : byOldUnit)
  {
    ++counts[pair.lineIndex + 1u];
  }
  for(uint32_t index = 1u; index < counts.size(); ++index)
  {
    counts[index] += counts[index - 1u];
  }
  for(const LineUnitPair& pair : byOldUnit)
  {
    orderedPairs[counts[pair.lineIndex]++] = pair;
  }

  std::vector<uint32_t> newUnitByPair(pairs.size(), NO_UNIT);
  std::vector<uint8_t>  imageReplacementUnitMask;
  if(!plan.imageReplacementUnitMask.empty())
  {
    imageReplacementUnitMask.assign(pairs.size(), 0u);
  }
  for(uint32_t newUnit = 0u; newUnit < orderedPairs.size(); ++newUnit)
  {
    newUnitByPair[orderedPairs[newUnit].originalIndex] = newUnit;
    const uint32_t oldUnit                             = orderedPairs[newUnit].oldUnit;
    if(oldUnit < plan.imageReplacementUnitMask.size())
    {
      imageReplacementUnitMask[newUnit] = plan.imageReplacementUnitMask[oldUnit];
    }
  }

  sequenceStaggerRatio      = std::isnan(sequenceStaggerRatio)
                                ? 0.0f
                                : std::max(0.0f, std::min(1.0f, sequenceStaggerRatio));
  const float fadeDuration  = ResolveFadeDurationRatio(plan.fadeDurationRatio, maxSequenceUnitCount);
  const float startInterval = maxSequenceUnitCount > 1u
                                ? (1.0f - fadeDuration) / static_cast<float>(maxSequenceUnitCount - 1u)
                                : 0.0f;

  float totalDuration = 0.0f;
  if(maxSequenceUnitCount == 1u)
  {
    // A singleton Reveal retains the complete normalized 0..1 domain even
    // when an explicit fade finishes early. Use the same canonical envelope
    // so STEP sequences still have a meaningful stagger.
    totalDuration = static_cast<float>(activeSequenceCount - 1u) * sequenceStaggerRatio + 1.0f;
  }
  else
  {
    uint32_t activeSequence = 0u;
    for(uint32_t count : lineUnitCount)
    {
      if(count == 0u)
      {
        continue;
      }
      const float offset = static_cast<float>(activeSequence++) * sequenceStaggerRatio;
      const float span   = static_cast<float>(count - 1u) * startInterval + fadeDuration;
      totalDuration      = std::max(totalDuration, offset + span);
    }
  }

  if(!(totalDuration > 0.0f) || !std::isfinite(totalDuration))
  {
    return false;
  }

  plan.unitStart.assign(orderedPairs.size(), 0.0f);
  std::vector<uint32_t> perLineSequenceIndex(lineCount, NO_UNIT);
  uint32_t              activeSequence = 0u;
  for(uint32_t lineIndex = 0u; lineIndex < lineCount; ++lineIndex)
  {
    if(lineUnitCount[lineIndex] > 0u)
    {
      perLineSequenceIndex[lineIndex] = activeSequence++;
    }
  }

  uint32_t currentLine = NO_UNIT;
  uint32_t localRank   = 0u;
  for(uint32_t newUnit = 0u; newUnit < orderedPairs.size(); ++newUnit)
  {
    const LineUnitPair& pair = orderedPairs[newUnit];
    if(pair.lineIndex != currentLine)
    {
      currentLine = pair.lineIndex;
      localRank   = 0u;
    }
    const float offset      = static_cast<float>(perLineSequenceIndex[pair.lineIndex]) * sequenceStaggerRatio;
    plan.unitStart[newUnit] = (offset + static_cast<float>(localRank++) * startInterval) / totalDuration;
  }
  plan.fadeDuration = fadeDuration / totalDuration;

  for(uint32_t glyph = 0u; glyph < glyphCount; ++glyph)
  {
    if(glyphToPair[glyph] != NO_UNIT)
    {
      plan.glyphToUnit[glyph] = newUnitByPair[glyphToPair[glyph]];
    }
  }
  plan.imageReplacementUnitMask = std::move(imageReplacementUnitMask);
  return true;
}

} // namespace Reveal
} // namespace Internal
} // namespace Text
} // namespace Ui
} // namespace Dali
