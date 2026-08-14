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

#include <dali-ui-foundation/internal/text/replacement/replacement-run-snapshot.h>
#include <dali-ui-foundation/internal/text/text-model-interface.h>
#include <dali/devel-api/text-abstraction/script.h>
#include <dali/devel-api/text-abstraction/segmentation.h>

#include <algorithm>
#include <cmath>

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
  return unit == Text::Reveal::Unit::WORD ? Unit::WORD : Unit::CHARACTER;
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

  if(unit == Unit::CHARACTER)
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
void ExcludeSyntheticReplacementGlyphs(Plan& plan, const GlyphInfo* glyphs, Length glyphCount)
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
  // Remove their layout-only glyphs from the reveal timeline, then compact
  // only units that still own an ordinary glyph in logical unit order.
  const uint32_t        oldUnitCount = plan.GetUnitCount();
  std::vector<bool>     survivingUnits(oldUnitCount, false);
  std::vector<uint32_t> oldToNewUnit(oldUnitCount, NO_UNIT);
  for(Length glyph = 0u; glyph < glyphCount; ++glyph)
  {
    uint32_t& unit = plan.glyphToUnit[glyph];
    if(IsSyntheticReplacementGlyph(glyphs[glyph]))
    {
      unit = NO_UNIT;
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
  PopulateSchedule(plan, newUnitCount, plan.fadeDurationRatio);
}

Plan BuildModelPlan(const ModelInterface& model,
                    Unit                  unit,
                    float                 fadeDurationRatio,
                    const WordBreakInfo*  wordBreakInfo)
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

  Plan plan = BuildPlan(text.Begin(), characterCount, glyphMap.Begin(), glyphCount, unit, fadeDurationRatio, wordBreakInfo);
  ExcludeSyntheticReplacementGlyphs(plan, model.GetGlyphs(), glyphCount);
  return plan;
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

Plan BuildCharacterPlan(const ModelInterface& model, float fadeDurationRatio)
{
  return BuildModelPlan(model, Unit::CHARACTER, fadeDurationRatio, nullptr);
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

  const bool hasEllipsis = ellipsisFinalGlyph < finalGlyphCount;

  std::vector<uint32_t> oldToNew(oldUnitCount, NO_UNIT);
  uint32_t              finalUnitCount = static_cast<uint32_t>(visibleOldUnits.size());
  if(hasEllipsis && unit == Unit::CHARACTER)
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
    if(unit == Unit::CHARACTER || visibleOldUnits.empty())
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

} // namespace Reveal
} // namespace Internal
} // namespace Text
} // namespace Ui
} // namespace Dali
