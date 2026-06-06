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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/internal/text/multi-language-support.h>
#include <dali-ui-foundation/internal/text/shaper.h>
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/devel-api/text-abstraction/shaping.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{

#ifndef DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR
#define DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "automated-tests/src/dali-ui-foundation-internal/resources"
#endif

struct FixtureRow
{
  std::vector<uint32_t> codepoints;
  std::string           status;
  std::string           signature;
  std::string           name;
};

struct FixtureData
{
  std::map<std::string, std::string> metadata;
  std::vector<FixtureRow>            rows;
};

struct Failure
{
  size_t      index{0u};
  FixtureRow row;
  std::string reason;
};

struct CompatibilityCase
{
  std::vector<uint32_t>              codepoints;
  Dali::TextAbstraction::Script      expectedScript;
  std::string                        signature;
  std::string                        label;
};

struct SweepOptions
{
  std::string           mode{"script"};
  size_t                maxRows{0u};
  size_t                maxFailures{50u};
  std::set<std::string> signatureFilter;
  std::string           failureLogPath{"/tmp/dali-emoji-sequence-sweep-failures.log"};
  bool                  profile{false};
};

struct SweepProfileStats
{
  std::chrono::steady_clock::duration validateScript{};
  std::chrono::steady_clock::duration setScriptsForFont{};
  std::chrono::steady_clock::duration validateFonts{};
  std::chrono::steady_clock::duration shapeText{};
  std::chrono::steady_clock::duration glyphMetrics{};
  std::chrono::steady_clock::duration createBitmap{};
  size_t                             metricGlyphs{0u};
  size_t                             bitmapGlyphs{0u};
  size_t                             colorBitmaps{0u};
};

struct SweepStats
{
  size_t                            totalRows{0u};
  size_t                            executed{0u};
  size_t                            passed{0u};
  size_t                            failed{0u};
  size_t                            skipped{0u};
  std::map<std::string, size_t>     failuresBySignature;
  std::map<std::string, size_t>     failuresByStatus;
  std::vector<Failure>              failures;
  std::vector<Failure>              firstFailures;
  std::chrono::steady_clock::duration elapsed{};
  SweepProfileStats                 profile;
};

const char* GetEnv(const char* name)
{
  return std::getenv(name);
}

bool IsEnvEnabledForSweep(const char* name)
{
  const char* const value = GetEnv(name);
  return value && 0 == std::strcmp(value, "1");
}

size_t GetEnvSize(const char* name, size_t fallback)
{
  const char* value = GetEnv(name);
  return value ? static_cast<size_t>(std::strtoull(value, nullptr, 10)) : fallback;
}

std::vector<std::string> Split(const std::string& text, char delimiter)
{
  std::vector<std::string> result;
  std::string              item;
  std::stringstream        stream(text);
  while(std::getline(stream, item, delimiter))
  {
    if(!item.empty())
    {
      result.push_back(item);
    }
  }
  return result;
}

std::vector<std::string> SplitTabs(const std::string& line)
{
  return Split(line, '\t');
}

std::vector<uint32_t> ParseCodepoints(const std::string& text)
{
  std::vector<uint32_t> codepoints;
  std::stringstream     stream(text);
  std::string           item;
  while(stream >> item)
  {
    codepoints.push_back(static_cast<uint32_t>(std::stoul(item, nullptr, 16)));
  }
  return codepoints;
}

std::string CodepointsToString(const std::vector<uint32_t>& codepoints)
{
  std::stringstream stream;
  for(size_t index = 0u; index < codepoints.size(); ++index)
  {
    if(index > 0u)
    {
      stream << ' ';
    }
    stream << std::uppercase << std::hex << codepoints[index] << std::dec;
  }
  return stream.str();
}

std::string MakeFixturePathFromTestResourceDir()
{
  const std::string resourceDir(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR);
  const std::string resourceSuffix = "/src/dali-ui-foundation-internal/resources";
  if(resourceDir.size() >= resourceSuffix.size() &&
     resourceDir.compare(resourceDir.size() - resourceSuffix.size(), resourceSuffix.size(), resourceSuffix) == 0)
  {
    return resourceDir.substr(0u, resourceDir.size() - resourceSuffix.size()) + "/emoji/res/emoji-test-latest.tsv";
  }

  return std::string();
}

std::string FindFixturePath()
{
  const char* envPath = GetEnv("DALI_EMOJI_SWEEP_FIXTURE");
  if(envPath && envPath[0] != '\0')
  {
    return envPath;
  }

  const std::vector<std::string> candidates =
  {
    MakeFixturePathFromTestResourceDir(),
    "automated-tests/emoji/res/emoji-test-latest.tsv",
    "../emoji/res/emoji-test-latest.tsv",
    std::string(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR) + "/emoji/emoji-test-latest.tsv",
    "automated-tests/src/dali-ui-foundation-internal/resources/emoji/emoji-test-latest.tsv",
    "../src/dali-ui-foundation-internal/resources/emoji/emoji-test-latest.tsv"
  };

  for(const std::string& candidate : candidates)
  {
    if(candidate.empty())
    {
      continue;
    }

    std::ifstream input(candidate.c_str());
    if(input.good())
    {
      return candidate;
    }
  }

  return "automated-tests/emoji/res/emoji-test-latest.tsv";
}

FixtureData LoadFixture(const std::string& fixturePath)
{
  std::ifstream input(fixturePath.c_str());
  if(!input.good())
  {
    throw std::runtime_error("failed to open fixture: " + fixturePath);
  }

  FixtureData fixture;
  std::string line;
  while(std::getline(input, line))
  {
    if(line.empty())
    {
      continue;
    }
    if(line[0] == '#')
    {
      const std::string marker = "# ";
      if(line.compare(0u, marker.size(), marker) == 0u)
      {
        const size_t separator = line.find(':');
        if(separator != std::string::npos)
        {
          const std::string key   = line.substr(marker.size(), separator - marker.size());
          const std::string value = line.substr(separator + 2u);
          fixture.metadata[key]   = value;
        }
      }
      continue;
    }

    const std::vector<std::string> fields = SplitTabs(line);
    if(fields.size() < 4u)
    {
      continue;
    }

    FixtureRow row;
    row.codepoints = ParseCodepoints(fields[0]);
    row.status     = fields[1];
    row.signature  = fields[2];
    row.name       = fields[3];
    fixture.rows.push_back(row);
  }

  return fixture;
}

SweepOptions LoadOptions()
{
  SweepOptions options;

  const char* mode = GetEnv("DALI_EMOJI_SWEEP_MODE");
  if(mode && mode[0] != '\0')
  {
    options.mode = mode;
  }

  options.maxRows     = GetEnvSize("DALI_EMOJI_SWEEP_MAX_ROWS", 0u);
  options.maxFailures = GetEnvSize("DALI_EMOJI_SWEEP_MAX_FAILURES", 50u);

  const char* failureLogPath = GetEnv("DALI_EMOJI_SWEEP_FAILURE_LOG");
  if(failureLogPath && failureLogPath[0] != '\0')
  {
    options.failureLogPath = failureLogPath;
  }

  options.profile = IsEnvEnabledForSweep("DALI_EMOJI_SWEEP_PROFILE");

  const char* signatureFilter = GetEnv("DALI_EMOJI_SWEEP_SIGNATURE_FILTER");
  if(signatureFilter && signatureFilter[0] != '\0')
  {
    for(const std::string& signature : Split(signatureFilter, ','))
    {
      options.signatureFilter.insert(signature);
    }
  }

  return options;
}

bool RunsFontValidation(const std::string& mode)
{
  return mode == "font" || mode == "shape" || mode == "all";
}

bool RunsShaping(const std::string& mode)
{
  return mode == "shape" || mode == "all";
}

bool RunsCompatibilitySweep(const std::string& mode)
{
  return mode == "compat";
}

bool IsSupportedSweepMode(const std::string& mode)
{
  return mode == "script" ||
         mode == "font" ||
         mode == "shape" ||
         mode == "all" ||
         mode == "compat";
}

bool IsEmojiScriptForSweep(Dali::TextAbstraction::Script script)
{
  return script == Dali::TextAbstraction::EMOJI ||
         script == Dali::TextAbstraction::EMOJI_COLOR ||
         script == Dali::TextAbstraction::EMOJI_TEXT;
}

const char* ScriptToString(Dali::TextAbstraction::Script script)
{
  switch(script)
  {
    case Dali::TextAbstraction::UNKNOWN:
      return "UNKNOWN";
    case Dali::TextAbstraction::LATIN:
      return "LATIN";
    case Dali::TextAbstraction::COMMON:
      return "COMMON";
    case Dali::TextAbstraction::EMOJI:
      return "EMOJI";
    case Dali::TextAbstraction::EMOJI_COLOR:
      return "EMOJI_COLOR";
    case Dali::TextAbstraction::EMOJI_TEXT:
      return "EMOJI_TEXT";
    default:
      return "OTHER";
  }
}

bool FixtureRowRequiresEmojiScript(const FixtureRow& row)
{
  if(row.status == "fully-qualified" || row.status == "component")
  {
    return true;
  }
  return row.signature != "single";
}

Dali::Vector<Dali::Ui::Text::Character> MakeGuardedText(const std::vector<uint32_t>& codepoints)
{
  Dali::Vector<Dali::Ui::Text::Character> text;
  text.PushBack(0x41u);
  for(uint32_t codepoint : codepoints)
  {
    text.PushBack(static_cast<Dali::Ui::Text::Character>(codepoint));
  }
  text.PushBack(0x42u);
  return text;
}

std::string ScriptRunsToString(const Dali::Vector<Dali::Ui::Text::ScriptRun>& scripts)
{
  std::stringstream stream;
  for(auto it = scripts.Begin(), endIt = scripts.End(); it != endIt; ++it)
  {
    if(it != scripts.Begin())
    {
      stream << " | ";
    }
    stream << '[' << it->characterRun.characterIndex << ", "
           << it->characterRun.characterIndex + it->characterRun.numberOfCharacters
           << ") " << ScriptToString(it->script);
  }
  return stream.str();
}

bool GetScriptAtCharacter(const Dali::Vector<Dali::Ui::Text::ScriptRun>& scripts,
                          uint32_t                                  characterIndex,
                          Dali::TextAbstraction::Script&            script)
{
  for(auto it = scripts.Begin(), endIt = scripts.End(); it != endIt; ++it)
  {
    const uint32_t begin = it->characterRun.characterIndex;
    const uint32_t end   = begin + it->characterRun.numberOfCharacters;
    if(characterIndex >= begin && characterIndex < end)
    {
      script = it->script;
      return true;
    }
  }
  return false;
}

std::vector<CompatibilityCase> BuildCompatibilityMutationCases()
{
  const std::vector<std::pair<std::string, uint32_t>> bases =
  {
    {"grinning face", 0x1F600u},
    {"robot", 0x1F916u},
    {"heart", 0x2764u}
  };

  std::vector<CompatibilityCase> cases;
  for(const auto& base : bases)
  {
    cases.push_back({{base.second, 0xFE0Eu}, Dali::TextAbstraction::EMOJI_TEXT, "compat:variation", base.first + " + VS15"});
    cases.push_back({{base.second, 0xFE0Fu}, Dali::TextAbstraction::EMOJI_COLOR, "compat:variation", base.first + " + VS16"});
    cases.push_back({{base.second, 0xFE0Eu, 0xFE0Eu}, Dali::TextAbstraction::EMOJI_TEXT, "compat:variation", base.first + " + VS15 + VS15"});
    cases.push_back({{base.second, 0xFE0Fu, 0xFE0Fu}, Dali::TextAbstraction::EMOJI_COLOR, "compat:variation", base.first + " + VS16 + VS16"});
    cases.push_back({{base.second, 0xFE0Eu, 0xFE0Fu}, Dali::TextAbstraction::EMOJI_COLOR, "compat:variation", base.first + " + VS15 + VS16"});
    cases.push_back({{base.second, 0xFE0Fu, 0xFE0Eu}, Dali::TextAbstraction::EMOJI_COLOR, "compat:variation", base.first + " + VS16 + VS15"});
    cases.push_back({{base.second, 0x200Du}, Dali::TextAbstraction::EMOJI_COLOR, "compat:zwj", base.first + " + dangling ZWJ"});
    cases.push_back({{base.second, 0xFE0Eu, 0x200Du, 0x1F525u},
                     Dali::TextAbstraction::EMOJI_COLOR,
                     "compat:zwj",
                     base.first + " + VS15 + ZWJ + fire"});
    cases.push_back({{base.second, 0xFE0Eu, 0xFE0Fu, 0x200Du, 0x1F525u},
                     Dali::TextAbstraction::EMOJI_COLOR,
                     "compat:zwj",
                     base.first + " + VS15 + VS16 + ZWJ + fire"});
  }

  const std::vector<std::pair<std::string, uint32_t>> keycapBases =
  {
    {"digit one", 0x0031u},
    {"number sign", 0x0023u},
    {"asterisk", 0x002Au}
  };

  for(const auto& base : keycapBases)
  {
    cases.push_back({{base.second, 0xFE0Eu, 0x20E3u}, Dali::TextAbstraction::EMOJI_TEXT, "compat:keycap", base.first + " + VS15 + keycap"});
    cases.push_back({{base.second, 0xFE0Fu, 0x20E3u}, Dali::TextAbstraction::EMOJI_COLOR, "compat:keycap", base.first + " + VS16 + keycap"});
    cases.push_back({{base.second, 0xFE0Eu, 0xFE0Fu, 0x20E3u},
                     Dali::TextAbstraction::EMOJI_COLOR,
                     "compat:keycap",
                     base.first + " + VS15 + VS16 + keycap"});
  }

  return cases;
}

bool ValidateScriptRuns(Dali::Ui::Text::MultilanguageSupport& multilanguageSupport,
                        const FixtureRow&                 row,
                        std::string&                      reason,
                        Dali::Vector<Dali::Ui::Text::ScriptRun>* outScripts = nullptr)
{
  Dali::Vector<Dali::Ui::Text::Character> text = MakeGuardedText(row.codepoints);
  Dali::Vector<Dali::Ui::Text::ScriptRun> scripts;
  multilanguageSupport.SetScripts(text, 0u, text.Count(), scripts);
  if(outScripts)
  {
    *outScripts = scripts;
  }

  auto fail = [&](const std::string& message)
  {
    reason = message + " scripts=\"" + ScriptRunsToString(scripts) + "\"";
    return false;
  };

  if(scripts.Empty())
  {
    return fail("produced no script runs");
  }

  Dali::TextAbstraction::Script guardScript = Dali::TextAbstraction::UNKNOWN;
  if(!GetScriptAtCharacter(scripts, 0u, guardScript) || guardScript != Dali::TextAbstraction::LATIN)
  {
    return fail("lost leading Latin guard");
  }
  if(!GetScriptAtCharacter(scripts, text.Count() - 1u, guardScript) || guardScript != Dali::TextAbstraction::LATIN)
  {
    return fail("lost trailing Latin guard");
  }

  if(!FixtureRowRequiresEmojiScript(row))
  {
    return true;
  }

  const uint32_t sequenceBegin = 1u;
  const uint32_t sequenceEnd   = text.Count() - 1u;
  bool           foundSequenceRun = false;
  for(auto it = scripts.Begin(), endIt = scripts.End(); it != endIt; ++it)
  {
    const uint32_t runBegin = it->characterRun.characterIndex;
    const uint32_t runEnd   = runBegin + it->characterRun.numberOfCharacters;
    const bool     overlaps = runBegin < sequenceEnd && runEnd > sequenceBegin;
    if(!overlaps)
    {
      continue;
    }

    if(runBegin != sequenceBegin || runEnd != sequenceEnd)
    {
      return fail("emoji sequence was split across script runs");
    }

    if(!IsEmojiScriptForSweep(it->script))
    {
      return fail("emoji sequence did not get an emoji script");
    }

    foundSequenceRun = true;
  }

  if(!foundSequenceRun)
  {
    return fail("emoji sequence script run was not found");
  }

  return true;
}

bool ValidateCompatibilityMutationCase(Dali::Ui::Text::MultilanguageSupport& multilanguageSupport,
                                       const CompatibilityCase&               testCase,
                                       std::string&                           reason)
{
  Dali::Vector<Dali::Ui::Text::Character> text = MakeGuardedText(testCase.codepoints);
  Dali::Vector<Dali::Ui::Text::ScriptRun> scripts;
  multilanguageSupport.SetScripts(text, 0u, text.Count(), scripts);

  auto fail = [&](const std::string& message)
  {
    reason = message + " scripts=\"" + ScriptRunsToString(scripts) + "\"";
    return false;
  };

  if(scripts.Count() < 3u)
  {
    return fail("did not keep guard/emoji/guard script runs");
  }

  Dali::TextAbstraction::Script guardScript = Dali::TextAbstraction::UNKNOWN;
  if(!GetScriptAtCharacter(scripts, 0u, guardScript) || guardScript != Dali::TextAbstraction::LATIN)
  {
    return fail("lost leading Latin guard");
  }
  if(!GetScriptAtCharacter(scripts, text.Count() - 1u, guardScript) || guardScript != Dali::TextAbstraction::LATIN)
  {
    return fail("lost trailing Latin guard");
  }

  const uint32_t sequenceBegin = 1u;
  const uint32_t sequenceEnd   = text.Count() - 1u;
  bool           foundSequenceRun = false;

  for(auto it = scripts.Begin(), endIt = scripts.End(); it != endIt; ++it)
  {
    const uint32_t runBegin = it->characterRun.characterIndex;
    const uint32_t runEnd   = runBegin + it->characterRun.numberOfCharacters;
    if(runBegin >= sequenceEnd || runEnd <= sequenceBegin)
    {
      continue;
    }

    if(runBegin != sequenceBegin || runEnd != sequenceEnd)
    {
      return fail("emoji sequence was split across script runs");
    }

    if(it->script != testCase.expectedScript)
    {
      std::stringstream message;
      message << "got " << ScriptToString(it->script)
              << ", expected " << ScriptToString(testCase.expectedScript);
      return fail(message.str());
    }

    foundSequenceRun = true;
  }

  return foundSequenceRun ? true : fail("emoji sequence script run was not found");
}

bool CheckNoMissingGlyphsInCharacterRange(const Dali::Vector<Dali::Ui::Text::GlyphInfo>&      glyphs,
                                          const Dali::Vector<Dali::Ui::Text::CharacterIndex>& glyphToCharacterMap,
                                          const Dali::Vector<Dali::Ui::Text::Length>&         charactersPerGlyph,
                                          uint32_t                                            characterRangeBegin,
                                          uint32_t                                            characterRangeEnd,
                                          std::string&                                        reason)
{
  for(uint32_t glyphIndex = 0u; glyphIndex < glyphs.Count(); ++glyphIndex)
  {
    const uint32_t glyphCharacterBegin = glyphToCharacterMap[glyphIndex];
    const uint32_t glyphCharacterSpan  = charactersPerGlyph[glyphIndex] == 0u ? 1u : charactersPerGlyph[glyphIndex];
    const uint32_t glyphCharacterEnd   = glyphCharacterBegin + glyphCharacterSpan;
    if(glyphCharacterBegin >= characterRangeEnd || glyphCharacterEnd <= characterRangeBegin)
    {
      continue;
    }

    if(glyphs[glyphIndex].index == 0u)
    {
      reason = "shaped missing glyph inside emoji sequence at glyph " + std::to_string(glyphIndex) +
               ", characterRange=[" + std::to_string(glyphCharacterBegin) + "," +
               std::to_string(glyphCharacterEnd) + ")";
      return false;
    }
  }

  return true;
}

bool IsExpectedComposedEmojiSequence(const FixtureRow& row)
{
  if(row.codepoints.size() < 2u || row.status == "component")
  {
    return false;
  }

  return row.signature == "keycap" ||
         row.signature == "modifier_sequence" ||
         row.signature == "regional_indicator_flag" ||
         row.signature == "tag_sequence" ||
         row.signature.compare(0u, 4u, "zwj:") == 0;
}

bool IsCompositionFormatCodepoint(uint32_t codepoint)
{
  return codepoint == 0x200Du ||
         codepoint == 0xFE0Eu ||
         codepoint == 0xFE0Fu ||
         (codepoint >= 0xE0020u && codepoint <= 0xE007Fu);
}

bool GlyphRangeCoversVisibleItemCodepoint(const FixtureRow& row, uint32_t glyphCharacterBegin, uint32_t glyphCharacterEnd)
{
  const uint32_t itemCharacterBegin = 1u;
  const uint32_t itemCharacterEnd   = 1u + static_cast<uint32_t>(row.codepoints.size());
  const uint32_t begin              = std::max(glyphCharacterBegin, itemCharacterBegin);
  const uint32_t end                = std::min(glyphCharacterEnd, itemCharacterEnd);

  for(uint32_t characterIndex = begin; characterIndex < end; ++characterIndex)
  {
    const uint32_t codepoint = row.codepoints[characterIndex - itemCharacterBegin];
    if(!IsCompositionFormatCodepoint(codepoint))
    {
      return true;
    }
  }

  return false;
}

bool CheckComposedEmojiSequence(const FixtureRow&                                      row,
                                const Dali::Vector<Dali::Ui::Text::GlyphInfo>&         glyphs,
                                const Dali::Vector<Dali::Ui::Text::CharacterIndex>&    glyphToCharacterMap,
                                const Dali::Vector<Dali::Ui::Text::Length>&            charactersPerGlyph,
                                std::string&                                           reason)
{
  if(!IsExpectedComposedEmojiSequence(row))
  {
    return true;
  }

  const uint32_t itemCharacterBegin = 1u;
  const uint32_t itemCharacterEnd   = 1u + static_cast<uint32_t>(row.codepoints.size());

  struct RelevantGlyph
  {
    uint32_t glyphIndex;
    uint32_t characterBegin;
    uint32_t characterEnd;
    float    advance;
    float    width;
    float    height;
  };
  std::vector<RelevantGlyph> relevantGlyphs;

  for(uint32_t glyphIndex = 0u; glyphIndex < glyphs.Count(); ++glyphIndex)
  {
    const uint32_t glyphCharacterBegin = glyphToCharacterMap[glyphIndex];
    const uint32_t glyphCharacterSpan  = charactersPerGlyph[glyphIndex] == 0u ? 1u : charactersPerGlyph[glyphIndex];
    const uint32_t glyphCharacterEnd   = glyphCharacterBegin + glyphCharacterSpan;
    if(glyphCharacterBegin >= itemCharacterEnd || glyphCharacterEnd <= itemCharacterBegin)
    {
      continue;
    }
    if(glyphs[glyphIndex].index == 0u)
    {
      continue;
    }
    if(glyphs[glyphIndex].advance <= 0.f && glyphs[glyphIndex].width <= 0.f && glyphs[glyphIndex].height <= 0.f)
    {
      continue;
    }
    if(!GlyphRangeCoversVisibleItemCodepoint(row, glyphCharacterBegin, glyphCharacterEnd))
    {
      continue;
    }

    relevantGlyphs.push_back({glyphIndex, glyphCharacterBegin, glyphCharacterEnd, glyphs[glyphIndex].advance, glyphs[glyphIndex].width, glyphs[glyphIndex].height});
  }

  if(relevantGlyphs.size() == 1u)
  {
    return true;
  }

  std::stringstream stream;
  stream << "emoji sequence was not rendered as a single visible glyph"
         << ", glyphCount=" << relevantGlyphs.size()
         << ", itemRange=[" << itemCharacterBegin << "," << itemCharacterEnd << ")"
         << ", glyphRanges=";
  if(relevantGlyphs.empty())
  {
    stream << "none";
  }
  else
  {
    for(size_t index = 0u; index < relevantGlyphs.size(); ++index)
    {
      if(index > 0u)
      {
        stream << " | ";
      }
      stream << "glyph#" << relevantGlyphs[index].glyphIndex
             << " chars=[" << relevantGlyphs[index].characterBegin
             << "," << relevantGlyphs[index].characterEnd << ")"
             << " advance=" << relevantGlyphs[index].advance
             << " size=" << relevantGlyphs[index].width
             << "x" << relevantGlyphs[index].height;
    }
  }
  reason = stream.str();
  return false;
}

bool ValidateFontsAndShape(Dali::Ui::Text::MultilanguageSupport&  multilanguageSupport,
                           Dali::TextAbstraction::FontClient& fontClient,
                           Dali::TextAbstraction::Shaping&    shaping,
                           const FixtureRow&                  row,
                           bool                               runShape,
                           std::string&                       reason,
                           SweepProfileStats*                 profileStats = nullptr)
{
  Dali::Vector<Dali::Ui::Text::Character> text = MakeGuardedText(row.codepoints);
  Dali::Vector<Dali::Ui::Text::ScriptRun> scripts;

  const auto setScriptsStart = profileStats ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point{};
  multilanguageSupport.SetScripts(text, 0u, text.Count(), scripts);
  if(profileStats)
  {
    profileStats->setScriptsForFont += std::chrono::steady_clock::now() - setScriptsStart;
  }

  Dali::Vector<Dali::Ui::Text::FontDescriptionRun> fontDescriptions;
  Dali::Vector<Dali::Ui::Text::FontRun>            fonts;
  Dali::TextAbstraction::FontDescription       defaultFontDescription;
  const Dali::TextAbstraction::PointSize26Dot6 defaultFontPointSize = Dali::TextAbstraction::FontClient::DEFAULT_POINT_SIZE;

  const auto validateFontsStart = profileStats ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point{};
  multilanguageSupport.ValidateFonts(fontClient,
                                     text,
                                     scripts,
                                     fontDescriptions,
                                     defaultFontDescription,
                                     defaultFontPointSize,
                                     1.f,
                                     0u,
                                     text.Count(),
                                     fonts);
  if(profileStats)
  {
    profileStats->validateFonts += std::chrono::steady_clock::now() - validateFontsStart;
  }

  if(fonts.Empty())
  {
    reason = "produced no font runs";
    return false;
  }

  const Dali::Ui::Text::FontRun& lastFontRun = fonts[fonts.Count() - 1u];
  if(lastFontRun.characterRun.characterIndex + lastFontRun.characterRun.numberOfCharacters != text.Count())
  {
    reason = "font runs do not cover all characters";
    return false;
  }

  if(!runShape)
  {
    return true;
  }

  Dali::Vector<Dali::Ui::Text::LineBreakInfo> lineBreakInfo;
  lineBreakInfo.Resize(text.Count(), Dali::TextAbstraction::LINE_NO_BREAK);

  Dali::Vector<Dali::Ui::Text::GlyphInfo>      glyphs;
  Dali::Vector<Dali::Ui::Text::CharacterIndex> glyphToCharacterMap;
  Dali::Vector<Dali::Ui::Text::Length>         charactersPerGlyph;
  Dali::Vector<Dali::Ui::Text::GlyphIndex>     newParagraphGlyphs;

  const auto shapeTextStart = profileStats ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point{};
  Dali::Ui::Text::ShapeText(shaping,
                        fontClient,
                        text,
                        lineBreakInfo,
                        scripts,
                        fonts,
                        0u,
                        0u,
                        text.Count(),
                        glyphs,
                        glyphToCharacterMap,
                        charactersPerGlyph,
                        newParagraphGlyphs);
  if(profileStats)
  {
    profileStats->shapeText += std::chrono::steady_clock::now() - shapeTextStart;
  }

  if(glyphs.Empty())
  {
    reason = "produced no glyphs";
    return false;
  }

  if(glyphToCharacterMap.Count() != glyphs.Count() || charactersPerGlyph.Count() != glyphs.Count())
  {
    reason = "produced inconsistent glyph maps";
    return false;
  }

  if(!CheckNoMissingGlyphsInCharacterRange(glyphs,
                                           glyphToCharacterMap,
                                           charactersPerGlyph,
                                           1u,
                                           1u + static_cast<uint32_t>(row.codepoints.size()),
                                           reason))
  {
    return false;
  }

  const bool needsCompositionCheck = IsExpectedComposedEmojiSequence(row);
  const bool needsGlyphMetrics     = profileStats || needsCompositionCheck;
  if(needsGlyphMetrics)
  {
    const auto glyphMetricsStart = profileStats ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point{};
    fontClient.GetGlyphMetrics(glyphs.Begin(), glyphs.Count(), Dali::TextAbstraction::BITMAP_GLYPH, true);
    if(profileStats)
    {
      profileStats->glyphMetrics += std::chrono::steady_clock::now() - glyphMetricsStart;
      profileStats->metricGlyphs += glyphs.Count();
    }
  }

  if(needsCompositionCheck && !CheckComposedEmojiSequence(row, glyphs, glyphToCharacterMap, charactersPerGlyph, reason))
  {
    return false;
  }

  if(profileStats)
  {
    const auto createBitmapStart = std::chrono::steady_clock::now();
    for(auto it = glyphs.Begin(), endIt = glyphs.End(); it != endIt; ++it)
    {
      if(0u == it->fontId || 0u == it->index)
      {
        continue;
      }

      Dali::TextAbstraction::GlyphBufferData glyphBufferData;
      glyphBufferData.width  = it->width > 0.f ? static_cast<uint32_t>(it->width) : 0u;
      glyphBufferData.height = it->height > 0.f ? static_cast<uint32_t>(it->height) : 0u;
      fontClient.CreateBitmap(it->fontId,
                              it->index,
                              it->isItalicRequired,
                              it->isBoldRequired,
                              glyphBufferData,
                              0);

      if(glyphBufferData.buffer)
      {
        ++profileStats->bitmapGlyphs;
        if(glyphBufferData.isColorEmoji || glyphBufferData.isColorBitmap)
        {
          ++profileStats->colorBitmaps;
        }
      }
    }
    profileStats->createBitmap += std::chrono::steady_clock::now() - createBitmapStart;
  }

  return true;
}

bool ShouldRunRow(const FixtureRow& row, const SweepOptions& options)
{
  return options.signatureFilter.empty() || options.signatureFilter.count(row.signature) > 0u;
}

bool ShouldRunCase(const CompatibilityCase& testCase, const SweepOptions& options)
{
  return options.signatureFilter.empty() || options.signatureFilter.count(testCase.signature) > 0u;
}

std::string FormatFailure(size_t index, const Failure& failure)
{
  std::stringstream stream;
  stream << "EMOJI_SWEEP_FAILURE"
         << " index=" << index
         << " signature=" << failure.row.signature
         << " status=" << failure.row.status
         << " codepoints=\"" << CodepointsToString(failure.row.codepoints) << "\""
         << " name=\"" << failure.row.name << "\""
         << " reason=\"" << failure.reason << "\"";
  return stream.str();
}

SweepStats RunSweep(const FixtureData& fixture, const SweepOptions& options)
{
  SweepStats stats;
  stats.totalRows = fixture.rows.size();

  Dali::Ui::Text::MultilanguageSupport  multilanguageSupport = Dali::Ui::Text::MultilanguageSupport::New(false);
  Dali::TextAbstraction::FontClient fontClient           = Dali::TextAbstraction::FontClient::New(96u, 96u);
  Dali::TextAbstraction::Shaping    shaping              = Dali::TextAbstraction::Shaping::New();

  const auto startTime = std::chrono::steady_clock::now();
  for(size_t index = 0u; index < fixture.rows.size(); ++index)
  {
    const FixtureRow& row = fixture.rows[index];
    if(!ShouldRunRow(row, options))
    {
      ++stats.skipped;
      continue;
    }
    if(options.maxRows > 0u && stats.executed >= options.maxRows)
    {
      ++stats.skipped;
      continue;
    }

    ++stats.executed;

    std::string reason;
    const auto validateScriptStart = options.profile ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point{};
    bool        passed             = ValidateScriptRuns(multilanguageSupport, row, reason);
    if(options.profile)
    {
      stats.profile.validateScript += std::chrono::steady_clock::now() - validateScriptStart;
    }

    if(passed && RunsFontValidation(options.mode))
    {
      passed = ValidateFontsAndShape(multilanguageSupport,
                                     fontClient,
                                     shaping,
                                     row,
                                     RunsShaping(options.mode),
                                     reason,
                                     options.profile ? &stats.profile : nullptr);
    }

    if(passed)
    {
      ++stats.passed;
    }
    else
    {
      ++stats.failed;
      ++stats.failuresBySignature[row.signature];
      ++stats.failuresByStatus[row.status];
      stats.failures.push_back({index, row, reason});
      if(stats.firstFailures.size() < options.maxFailures)
      {
        stats.firstFailures.push_back({index, row, reason});
      }
    }
  }
  stats.elapsed = std::chrono::steady_clock::now() - startTime;
  return stats;
}

SweepStats RunCompatibilitySweep(const SweepOptions& options)
{
  SweepStats stats;
  const std::vector<CompatibilityCase> cases = BuildCompatibilityMutationCases();
  stats.totalRows = cases.size();

  Dali::Ui::Text::MultilanguageSupport multilanguageSupport = Dali::Ui::Text::MultilanguageSupport::New(false);

  const auto startTime = std::chrono::steady_clock::now();
  for(size_t index = 0u; index < cases.size(); ++index)
  {
    const CompatibilityCase& testCase = cases[index];
    if(!ShouldRunCase(testCase, options))
    {
      ++stats.skipped;
      continue;
    }
    if(options.maxRows > 0u && stats.executed >= options.maxRows)
    {
      ++stats.skipped;
      continue;
    }

    ++stats.executed;

    std::string reason;
    const bool  passed = ValidateCompatibilityMutationCase(multilanguageSupport, testCase, reason);
    if(passed)
    {
      ++stats.passed;
      continue;
    }

    FixtureRow row;
    row.codepoints = testCase.codepoints;
    row.status     = "dali-compat";
    row.signature  = testCase.signature;
    row.name       = testCase.label;

    ++stats.failed;
    ++stats.failuresBySignature[row.signature];
    ++stats.failuresByStatus[row.status];
    stats.failures.push_back({index, row, reason});
    if(stats.firstFailures.size() < options.maxFailures)
    {
      stats.firstFailures.push_back({index, row, reason});
    }
  }

  stats.elapsed = std::chrono::steady_clock::now() - startTime;
  return stats;
}

void PrintMetadata(const FixtureData& fixture)
{
  auto printValue = [&](const char* key)
  {
    const auto iter = fixture.metadata.find(key);
    if(iter != fixture.metadata.end())
    {
      std::cout << key << ": " << iter->second << '\n';
    }
  };

  printValue("unicode_emoji_version");
  printValue("emoji_test_date");
  printValue("row_count");
  printValue("source_emoji-test");
}

void PrintStats(const SweepStats& stats, const SweepOptions& options)
{
  const double elapsedMs = std::chrono::duration_cast<std::chrono::microseconds>(stats.elapsed).count() / 1000.0;
  auto toMs = [](std::chrono::steady_clock::duration duration) -> double
  {
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count() / 1000.0;
  };

  std::cout << "EMOJI_SWEEP_SUMMARY"
            << " mode=" << options.mode
            << " totalRows=" << stats.totalRows
            << " executed=" << stats.executed
            << " passed=" << stats.passed
            << " failed=" << stats.failed
            << " skipped=" << stats.skipped
            << " elapsedMs=" << elapsedMs
            << '\n';

  if(options.profile)
  {
    std::cout << "EMOJI_SWEEP_PROFILE"
              << " validateScriptMs=" << toMs(stats.profile.validateScript)
              << " setScriptsForFontMs=" << toMs(stats.profile.setScriptsForFont)
              << " validateFontsMs=" << toMs(stats.profile.validateFonts)
              << " shapeTextMs=" << toMs(stats.profile.shapeText)
              << " glyphMetricsMs=" << toMs(stats.profile.glyphMetrics)
              << " createBitmapMs=" << toMs(stats.profile.createBitmap)
              << " metricGlyphs=" << stats.profile.metricGlyphs
              << " bitmapGlyphs=" << stats.profile.bitmapGlyphs
              << " colorBitmaps=" << stats.profile.colorBitmaps
              << '\n';
  }

  for(const auto& entry : stats.failuresBySignature)
  {
    std::cout << "EMOJI_SWEEP_FAILURE_SIGNATURE signature=" << entry.first << " count=" << entry.second << '\n';
  }
  for(const auto& entry : stats.failuresByStatus)
  {
    std::cout << "EMOJI_SWEEP_FAILURE_STATUS status=" << entry.first << " count=" << entry.second << '\n';
  }

  if(stats.failed > stats.firstFailures.size())
  {
    std::cout << "Showing first " << stats.firstFailures.size() << " of " << stats.failed << " failures\n";
  }
  for(const Failure& failure : stats.firstFailures)
  {
    std::cout << FormatFailure(failure.index, failure) << '\n';
  }
}

void WriteFailureLog(const SweepStats& stats, const SweepOptions& options)
{
  std::ofstream output(options.failureLogPath.c_str());
  if(!output.good())
  {
    std::cerr << "Failed to write failure log: " << options.failureLogPath << '\n';
    return;
  }

  for(const Failure& failure : stats.failures)
  {
    output << FormatFailure(failure.index, failure) << '\n';
  }

  std::cout << "failureLogPath: " << options.failureLogPath << '\n';
}

} // namespace

int main()
{
  try
  {
    const SweepOptions options = LoadOptions();
    if(!IsSupportedSweepMode(options.mode))
    {
      throw std::runtime_error("unsupported DALI_EMOJI_SWEEP_MODE: " + options.mode);
    }

    SweepStats stats;
    if(RunsCompatibilitySweep(options.mode))
    {
      stats = RunCompatibilitySweep(options);
    }
    else
    {
      const std::string fixturePath = FindFixturePath();
      const FixtureData fixture     = LoadFixture(fixturePath);

      std::cout << "fixturePath=" << fixturePath << '\n';
      PrintMetadata(fixture);
      stats = RunSweep(fixture, options);
    }

    PrintStats(stats, options);
    WriteFailureLog(stats, options);

    if(GetEnv("DALI_EMOJI_SWEEP_VISUAL_FAILURES"))
    {
      std::cout << "Open visual viewer with:\n"
                << "DALI_EMOJI_VISUAL_INPUT=" << options.failureLogPath
                << " ./automated-tests/build/src/dali-ui-foundation-internal/dali-emoji-sequence-visual\n";
    }

    return stats.failed == 0u ? 0 : 1;
  }
  catch(const std::exception& exception)
  {
    std::cerr << "emoji sequence sweep failed: " << exception.what() << '\n';
    return 2;
  }
}
