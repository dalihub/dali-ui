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
#include <dali/public-api/adaptor-framework/capture.h>
#include <dali/public-api/adaptor-framework/timer.h>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{

#ifndef DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR
#define DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "automated-tests/src/dali-ui-foundation-internal/resources"
#endif

constexpr float HEADER_HEIGHT                    = 96.0f;
constexpr float ROW_HEIGHT                       = 230.0f;
constexpr float ROW_SPACING                      = 8.0f;
constexpr float PAGE_PADDING_X                   = 12.0f;
constexpr float PAGE_PADDING_Y                   = 12.0f;
constexpr float ROW_PADDING_X                    = 10.0f;
constexpr float ROW_PADDING_Y                    = 8.0f;
constexpr float PREVIEW_WIDTH                    = 160.0f;
constexpr float PREVIEW_SIZE                     = 52.0f;
constexpr float DETAIL_SIZE                      = 15.0f;
constexpr float MIN_DETAIL_WIDTH                 = 220.0f;
constexpr float HEADER_SIZE                      = 17.0f;
constexpr float NAV_HEIGHT                       = 86.0f;
constexpr float NAV_BUTTON_WIDTH                 = 170.0f;
constexpr float NAV_BUTTON_HEIGHT                = 30.0f;
constexpr float NAV_SIZE                         = 12.0f;
constexpr float PREVIEW_POPUP_MARGIN             = 64.0f;
constexpr float PREVIEW_POPUP_MAX_WIDTH          = 800.0f;
constexpr float PREVIEW_POPUP_MAX_HEIGHT         = 800.0f;
constexpr float PREVIEW_POPUP_TEXT_FIT_MIN_SIZE  = 20.0f;
constexpr float PREVIEW_POPUP_TEXT_FIT_MAX_SIZE  = 600.0f;
constexpr float PREVIEW_POPUP_TEXT_FIT_STEP_SIZE = 10.0f;
constexpr size_t DEFAULT_PAGE_SIZE               = 100u;
constexpr uint32_t DETAIL_DIAGNOSTIC_TIMER_MS    = 16u;
constexpr size_t DETAIL_DIAGNOSTIC_BATCH_SIZE    = 2u;

struct VisualCase
{
  std::vector<uint32_t> codepoints;
  std::string           section;
  std::string           status;
  std::string           signature;
  std::string           name;
  std::string           reason;
};

struct VisualOptions
{
  std::string           inputPath;
  std::string           fixturePath;
  std::string           mode{"review"};
  std::string           exportHtmlPath;
  std::string           capturePath;
  std::set<std::string> signatureFilter;
  size_t                maxItems{0u};
  size_t                pageSize{DEFAULT_PAGE_SIZE};
  uint32_t              captureDelayMs{700u};
  float                 fontSize{PREVIEW_SIZE};
  bool                  exportOnly{false};
  bool                  exitAfterExport{false};
  bool                  exitAfterCapture{false};
};

const char* GetEnv(const char* name)
{
  return std::getenv(name);
}

size_t GetEnvSize(const char* name, size_t fallback)
{
  const char* value = GetEnv(name);
  return value ? static_cast<size_t>(std::strtoull(value, nullptr, 10)) : fallback;
}

float GetEnvFloat(const char* name, float fallback)
{
  const char* value = GetEnv(name);
  return value ? std::strtof(value, nullptr) : fallback;
}

bool GetEnvBool(const char* name)
{
  const char* value = GetEnv(name);
  if(!value || value[0] == '\0')
  {
    return false;
  }

  const std::string text(value);
  return text == "1" || text == "true" || text == "TRUE" || text == "yes" || text == "YES" || text == "on" || text == "ON";
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
    if(item.compare(0u, 2u, "U+") == 0u)
    {
      item = item.substr(2u);
    }
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

std::string Basename(const std::string& path)
{
  const size_t slash = path.find_last_of('/');
  return slash == std::string::npos ? path : path.substr(slash + 1u);
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

std::string FontRunsToString(Dali::TextAbstraction::FontClient& fontClient,
                             const Dali::Vector<Dali::Ui::Text::FontRun>& fonts)
{
  std::stringstream stream;
  for(auto it = fonts.Begin(), endIt = fonts.End(); it != endIt; ++it)
  {
    if(it != fonts.Begin())
    {
      stream << " | ";
    }

    Dali::TextAbstraction::FontDescription description;
    fontClient.GetDescription(it->fontId, description);

    stream << '[' << it->characterRun.characterIndex << ", "
           << it->characterRun.characterIndex + it->characterRun.numberOfCharacters
           << ") fontId=" << it->fontId;
    if(!description.family.empty())
    {
      stream << " family=" << description.family;
    }
    if(!description.path.empty())
    {
      stream << " file=" << Basename(description.path);
    }
    if(it->isBoldRequired)
    {
      stream << " bold";
    }
    if(it->isItalicRequired)
    {
      stream << " italic";
    }
  }
  return stream.str();
}

std::string MissingGlyphsToString(const Dali::Vector<Dali::Ui::Text::GlyphInfo>&      glyphs,
                                  const Dali::Vector<Dali::Ui::Text::CharacterIndex>& glyphToCharacterMap,
                                  const Dali::Vector<Dali::Ui::Text::Length>&         charactersPerGlyph,
                                  uint32_t                                            characterRangeBegin,
                                  uint32_t                                            characterRangeEnd)
{
  std::stringstream stream;
  bool              hasMissingGlyph = false;

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
      if(hasMissingGlyph)
      {
        stream << " | ";
      }
      stream << "glyph#" << glyphIndex << " chars=[" << glyphCharacterBegin << "," << glyphCharacterEnd << ")"
             << " fontId=" << glyphs[glyphIndex].fontId;
      hasMissingGlyph = true;
    }
  }

  return hasMissingGlyph ? stream.str() : "none";
}

bool IsExpectedComposedEmojiSequence(const VisualCase& item)
{
  if(item.codepoints.size() < 2u || item.status == "component")
  {
    return false;
  }

  return item.signature == "keycap" ||
         item.signature == "modifier_sequence" ||
         item.signature == "regional_indicator_flag" ||
         item.signature == "tag_sequence" ||
         item.signature.compare(0u, 4u, "zwj:") == 0;
}

bool IsCompositionFormatCodepoint(uint32_t codepoint)
{
  return codepoint == 0x200Du ||
         codepoint == 0xFE0Eu ||
         codepoint == 0xFE0Fu ||
         (codepoint >= 0xE0020u && codepoint <= 0xE007Fu);
}

bool GlyphRangeCoversVisibleItemCodepoint(const VisualCase& item, uint32_t glyphCharacterBegin, uint32_t glyphCharacterEnd)
{
  const uint32_t itemCharacterBegin = 1u;
  const uint32_t itemCharacterEnd   = 1u + static_cast<uint32_t>(item.codepoints.size());
  const uint32_t begin              = std::max(glyphCharacterBegin, itemCharacterBegin);
  const uint32_t end                = std::min(glyphCharacterEnd, itemCharacterEnd);

  for(uint32_t characterIndex = begin; characterIndex < end; ++characterIndex)
  {
    const uint32_t codepoint = item.codepoints[characterIndex - itemCharacterBegin];
    if(!IsCompositionFormatCodepoint(codepoint))
    {
      return true;
    }
  }

  return false;
}

std::string ItemCodepointsForCharacterRange(const VisualCase& item, uint32_t glyphCharacterBegin, uint32_t glyphCharacterEnd)
{
  const uint32_t itemCharacterBegin = 1u;
  const uint32_t itemCharacterEnd   = 1u + static_cast<uint32_t>(item.codepoints.size());
  const uint32_t begin              = std::max(glyphCharacterBegin, itemCharacterBegin);
  const uint32_t end                = std::min(glyphCharacterEnd, itemCharacterEnd);

  std::vector<uint32_t> codepoints;
  for(uint32_t characterIndex = begin; characterIndex < end; ++characterIndex)
  {
    codepoints.push_back(item.codepoints[characterIndex - itemCharacterBegin]);
  }

  return codepoints.empty() ? "none" : CodepointsToString(codepoints);
}

std::string CompositionGlyphsToString(const VisualCase&                                      item,
                                      const Dali::Vector<Dali::Ui::Text::GlyphInfo>&         glyphs,
                                      const Dali::Vector<Dali::Ui::Text::CharacterIndex>&    glyphToCharacterMap,
                                      const Dali::Vector<Dali::Ui::Text::Length>&            charactersPerGlyph)
{
  const uint32_t itemCharacterBegin = 1u;
  const uint32_t itemCharacterEnd   = 1u + static_cast<uint32_t>(item.codepoints.size());

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
    if(!GlyphRangeCoversVisibleItemCodepoint(item, glyphCharacterBegin, glyphCharacterEnd))
    {
      continue;
    }

    relevantGlyphs.push_back({glyphIndex, glyphCharacterBegin, glyphCharacterEnd, glyphs[glyphIndex].advance, glyphs[glyphIndex].width, glyphs[glyphIndex].height});
  }

  std::stringstream stream;
  stream << "expected: single visible glyph, ";
  if(relevantGlyphs.size() == 1u)
  {
    return std::string();
  }
  else if(relevantGlyphs.empty())
  {
    stream << "actual: no visible glyph";
    return stream.str();
  }
  else
  {
    stream << "actual: split visible glyphs (" << relevantGlyphs.size() << ")";
  }

  stream << '\n';
  for(size_t index = 0u; index < relevantGlyphs.size(); ++index)
  {
    if(index > 0u)
    {
      stream << " | ";
    }
    stream << "glyph#" << relevantGlyphs[index].glyphIndex
           << " chars=[" << relevantGlyphs[index].characterBegin
           << "," << relevantGlyphs[index].characterEnd << ")"
           << " codepoints=" << ItemCodepointsForCharacterRange(item, relevantGlyphs[index].characterBegin, relevantGlyphs[index].characterEnd)
           << " advance=" << relevantGlyphs[index].advance
           << " size=" << relevantGlyphs[index].width
           << "x" << relevantGlyphs[index].height;
  }
  return stream.str();
}

std::string HtmlEscape(const std::string& text)
{
  std::string escaped;
  escaped.reserve(text.size());
  for(char character : text)
  {
    switch(character)
    {
      case '&':
        escaped += "&amp;";
        break;
      case '<':
        escaped += "&lt;";
        break;
      case '>':
        escaped += "&gt;";
        break;
      case '"':
        escaped += "&quot;";
        break;
      case '\'':
        escaped += "&#39;";
        break;
      default:
        escaped.push_back(character);
        break;
    }
  }
  return escaped;
}

void AppendUtf8(std::string& output, uint32_t codepoint)
{
  if(codepoint <= 0x7Fu)
  {
    output.push_back(static_cast<char>(codepoint));
  }
  else if(codepoint <= 0x7FFu)
  {
    output.push_back(static_cast<char>(0xC0u | (codepoint >> 6u)));
    output.push_back(static_cast<char>(0x80u | (codepoint & 0x3Fu)));
  }
  else if(codepoint <= 0xFFFFu)
  {
    output.push_back(static_cast<char>(0xE0u | (codepoint >> 12u)));
    output.push_back(static_cast<char>(0x80u | ((codepoint >> 6u) & 0x3Fu)));
    output.push_back(static_cast<char>(0x80u | (codepoint & 0x3Fu)));
  }
  else
  {
    output.push_back(static_cast<char>(0xF0u | (codepoint >> 18u)));
    output.push_back(static_cast<char>(0x80u | ((codepoint >> 12u) & 0x3Fu)));
    output.push_back(static_cast<char>(0x80u | ((codepoint >> 6u) & 0x3Fu)));
    output.push_back(static_cast<char>(0x80u | (codepoint & 0x3Fu)));
  }
}

std::string CodepointsToUtf8(const std::vector<uint32_t>& codepoints)
{
  std::string text;
  for(uint32_t codepoint : codepoints)
  {
    AppendUtf8(text, codepoint);
  }
  return text;
}

std::string SectionForCase(const VisualCase& item)
{
  return item.section.empty() ? item.signature : item.section;
}

size_t ShownCountForOptions(const std::vector<VisualCase>& items, const VisualOptions& options)
{
  return options.maxItems == 0u ? items.size() : std::min(items.size(), options.maxItems);
}

VisualCase MakeVisualCase(const std::vector<uint32_t>& codepoints,
                          const std::string&           section,
                          const std::string&           status,
                          const std::string&           signature,
                          const std::string&           name,
                          const std::string&           reason = std::string())
{
  VisualCase item;
  item.codepoints = codepoints;
  item.section    = section;
  item.status     = status;
  item.signature  = signature;
  item.name       = name;
  item.reason     = reason;
  return item;
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
  const char* envPath = GetEnv("DALI_EMOJI_VISUAL_FIXTURE");
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

std::string ExtractValue(const std::string& line, const std::string& key)
{
  const std::string marker = key + "=";
  const size_t      begin  = line.find(marker);
  if(begin == std::string::npos)
  {
    return std::string();
  }

  size_t valueBegin = begin + marker.size();
  if(valueBegin >= line.size())
  {
    return std::string();
  }

  if(line[valueBegin] == '"')
  {
    ++valueBegin;
    const size_t valueEnd = line.find('"', valueBegin);
    return line.substr(valueBegin, valueEnd == std::string::npos ? std::string::npos : valueEnd - valueBegin);
  }

  const size_t valueEnd = line.find(' ', valueBegin);
  return line.substr(valueBegin, valueEnd == std::string::npos ? std::string::npos : valueEnd - valueBegin);
}

VisualOptions LoadOptions()
{
  VisualOptions options;

  const char* inputPath = GetEnv("DALI_EMOJI_VISUAL_INPUT");
  if(inputPath && inputPath[0] != '\0')
  {
    options.inputPath = inputPath;
  }

  const char* visualMode = GetEnv("DALI_EMOJI_VISUAL_MODE");
  if(visualMode && visualMode[0] != '\0')
  {
    options.mode = visualMode;
  }

  const char* maxItems = GetEnv("DALI_EMOJI_VISUAL_MAX_ITEMS");
  options.fixturePath  = FindFixturePath();
  options.maxItems     = maxItems ? GetEnvSize("DALI_EMOJI_VISUAL_MAX_ITEMS", 0u) : 0u;
  options.pageSize     = GetEnvSize("DALI_EMOJI_VISUAL_PAGE_SIZE", DEFAULT_PAGE_SIZE);
  options.fontSize    = GetEnvFloat("DALI_EMOJI_VISUAL_FONT_SIZE", PREVIEW_SIZE);
  options.captureDelayMs = static_cast<uint32_t>(GetEnvSize("DALI_EMOJI_VISUAL_CAPTURE_DELAY_MS", options.captureDelayMs));
  options.exportOnly       = GetEnvBool("DALI_EMOJI_VISUAL_EXPORT_ONLY");
  options.exitAfterExport  = GetEnvBool("DALI_EMOJI_VISUAL_EXIT_AFTER_EXPORT");
  options.exitAfterCapture = GetEnvBool("DALI_EMOJI_VISUAL_EXIT_AFTER_CAPTURE");

  const char* exportHtmlPath = GetEnv("DALI_EMOJI_VISUAL_EXPORT_HTML");
  if(exportHtmlPath && exportHtmlPath[0] != '\0')
  {
    options.exportHtmlPath = exportHtmlPath;
  }

  const char* capturePath = GetEnv("DALI_EMOJI_VISUAL_CAPTURE");
  if(capturePath && capturePath[0] != '\0')
  {
    options.capturePath = capturePath;
  }

  const char* signatureFilter = GetEnv("DALI_EMOJI_VISUAL_SIGNATURE_FILTER");
  if(signatureFilter && signatureFilter[0] != '\0')
  {
    for(const std::string& signature : Split(signatureFilter, ','))
    {
      options.signatureFilter.insert(signature);
    }
  }

  return options;
}

bool ShouldInclude(const VisualCase& item, const VisualOptions& options)
{
  return options.signatureFilter.empty() ||
         options.signatureFilter.count(item.signature) > 0u ||
         options.signatureFilter.count(SectionForCase(item)) > 0u;
}

bool IsFontMissingGlyphFailure(const std::string& reason)
{
  return reason.find("missing glyph") != std::string::npos;
}

std::vector<VisualCase> LoadFailureLog(const std::string& path, const VisualOptions& options)
{
  std::ifstream input(path.c_str());
  if(!input.good())
  {
    throw std::runtime_error("failed to open visual failure log: " + path);
  }

  std::vector<VisualCase> sweepFailures;
  std::vector<VisualCase> fontMissingGlyphFailures;
  std::string             line;
  while(std::getline(input, line))
  {
    if(line.find("EMOJI_SWEEP_FAILURE") != 0u)
    {
      continue;
    }

    VisualCase item;
    item.signature  = ExtractValue(line, "signature");
    item.status     = ExtractValue(line, "status");
    item.name       = ExtractValue(line, "name");
    item.reason     = ExtractValue(line, "reason");
    item.section    = IsFontMissingGlyphFailure(item.reason) ? "font missing glyph" : "sweep failure";
    item.codepoints = ParseCodepoints(ExtractValue(line, "codepoints"));
    if(!item.codepoints.empty())
    {
      if(item.section == "font missing glyph")
      {
        fontMissingGlyphFailures.push_back(item);
      }
      else
      {
        sweepFailures.push_back(item);
      }
    }
  }

  sweepFailures.insert(sweepFailures.end(), fontMissingGlyphFailures.begin(), fontMissingGlyphFailures.end());
  return sweepFailures;
}

std::vector<VisualCase> LoadFixture(const VisualOptions& options)
{
  std::ifstream input(options.fixturePath.c_str());
  if(!input.good())
  {
    throw std::runtime_error("failed to open visual fixture: " + options.fixturePath);
  }

  std::vector<VisualCase> items;
  std::string             line;
  while(std::getline(input, line))
  {
    if(line.empty() || line[0] == '#')
    {
      continue;
    }

    const std::vector<std::string> fields = SplitTabs(line);
    if(fields.size() < 4u)
    {
      continue;
    }

    VisualCase item;
    item.codepoints = ParseCodepoints(fields[0]);
    item.status     = fields[1];
    item.signature  = fields[2];
    item.name       = fields[3];
    items.push_back(item);
  }

  return items;
}

std::vector<VisualCase> BuildRepresentativeCases(const std::vector<VisualCase>& fixtureItems)
{
  std::set<std::string>    seenSignatures;
  std::vector<VisualCase>  items;

  for(const VisualCase& fixtureItem : fixtureItems)
  {
    if(seenSignatures.insert(fixtureItem.signature).second)
    {
      VisualCase item = fixtureItem;
      item.section    = "unicode representative";
      items.push_back(item);
    }
  }

  return items;
}

std::vector<VisualCase> GroupCasesBySignature(const std::vector<VisualCase>& fixtureItems)
{
  std::set<std::string>    seenSignatures;
  std::vector<std::string> signatureOrder;
  for(const VisualCase& item : fixtureItems)
  {
    if(seenSignatures.insert(item.signature).second)
    {
      signatureOrder.push_back(item.signature);
    }
  }

  std::vector<VisualCase> groupedItems;
  for(const std::string& signature : signatureOrder)
  {
    for(const VisualCase& item : fixtureItems)
    {
      if(item.signature == signature)
      {
        groupedItems.push_back(item);
      }
    }
  }

  return groupedItems;
}

std::vector<VisualCase> BuildCompatibilityCases()
{
  const std::vector<std::pair<std::string, uint32_t>> bases =
  {
    {"grinning face", 0x1F600u},
    {"robot", 0x1F916u},
    {"heart", 0x2764u}
  };

  std::vector<VisualCase> items;
  for(const auto& base : bases)
  {
    items.push_back(MakeVisualCase({base.second, 0xFE0Eu}, "DALI compatibility fallback", "dali-compat", "compat:variation", base.first + " + VS15"));
    items.push_back(MakeVisualCase({base.second, 0xFE0Fu}, "DALI compatibility fallback", "dali-compat", "compat:variation", base.first + " + VS16"));
    items.push_back(MakeVisualCase({base.second, 0xFE0Eu, 0xFE0Eu}, "DALI compatibility fallback", "dali-compat", "compat:variation", base.first + " + VS15 + VS15"));
    items.push_back(MakeVisualCase({base.second, 0xFE0Fu, 0xFE0Fu}, "DALI compatibility fallback", "dali-compat", "compat:variation", base.first + " + VS16 + VS16"));
    items.push_back(MakeVisualCase({base.second, 0xFE0Eu, 0xFE0Fu}, "DALI compatibility fallback", "dali-compat", "compat:variation", base.first + " + VS15 + VS16"));
    items.push_back(MakeVisualCase({base.second, 0xFE0Fu, 0xFE0Eu}, "DALI compatibility fallback", "dali-compat", "compat:variation", base.first + " + VS16 + VS15"));
    items.push_back(MakeVisualCase({base.second, 0x200Du}, "DALI compatibility fallback", "dali-compat", "compat:zwj", base.first + " + dangling ZWJ"));
    items.push_back(MakeVisualCase({base.second, 0xFE0Eu, 0x200Du, 0x1F525u}, "DALI compatibility fallback", "dali-compat", "compat:zwj", base.first + " + VS15 + ZWJ + fire"));
    items.push_back(MakeVisualCase({base.second, 0xFE0Eu, 0xFE0Fu, 0x200Du, 0x1F525u}, "DALI compatibility fallback", "dali-compat", "compat:zwj", base.first + " + VS15 + VS16 + ZWJ + fire"));
  }

  const std::vector<std::pair<std::string, uint32_t>> keycapBases =
  {
    {"digit one", 0x0031u},
    {"number sign", 0x0023u},
    {"asterisk", 0x002Au}
  };

  for(const auto& base : keycapBases)
  {
    items.push_back(MakeVisualCase({base.second, 0xFE0Eu, 0x20E3u}, "DALI compatibility fallback", "dali-compat", "compat:keycap", base.first + " + VS15 + keycap"));
    items.push_back(MakeVisualCase({base.second, 0xFE0Fu, 0x20E3u}, "DALI compatibility fallback", "dali-compat", "compat:keycap", base.first + " + VS16 + keycap"));
    items.push_back(MakeVisualCase({base.second, 0xFE0Eu, 0xFE0Fu, 0x20E3u}, "DALI compatibility fallback", "dali-compat", "compat:keycap", base.first + " + VS15 + VS16 + keycap"));
  }

  return items;
}

std::vector<VisualCase> BuildKeycapEvidenceCases()
{
  const std::vector<std::pair<std::string, uint32_t>> bases =
  {
    {"number sign", 0x0023u},
    {"asterisk", 0x002Au},
    {"digit zero", 0x0030u},
    {"digit one", 0x0031u},
    {"digit two", 0x0032u},
    {"digit three", 0x0033u},
    {"digit four", 0x0034u},
    {"digit five", 0x0035u},
    {"digit six", 0x0036u},
    {"digit seven", 0x0037u},
    {"digit eight", 0x0038u},
    {"digit nine", 0x0039u}
  };

  std::vector<VisualCase> items;
  for(const auto& base : bases)
  {
    items.push_back(MakeVisualCase({base.second, 0xFE0Fu, 0x20E3u}, "keycap evidence", "evidence", "keycap", base.first + " + VS16 + keycap"));
  }
  items.push_back(MakeVisualCase({0x0031u, 0xFE0Eu, 0x20E3u}, "keycap evidence", "evidence", "keycap", "digit one + VS15 + keycap"));
  items.push_back(MakeVisualCase({0x0031u, 0xFE0Eu, 0xFE0Fu, 0x20E3u}, "keycap evidence", "evidence", "keycap", "digit one + VS15 + VS16 + keycap"));
  return items;
}

std::vector<VisualCase> BuildSymbolVariationEvidenceCases()
{
  std::vector<VisualCase> items;
  items.push_back(MakeVisualCase({0x2728u}, "symbol variation evidence", "evidence", "symbol_variation", "sparkles plain"));
  items.push_back(MakeVisualCase({0x2728u, 0xFE0Eu}, "symbol variation evidence", "evidence", "symbol_variation", "sparkles + VS15"));
  items.push_back(MakeVisualCase({0x2728u, 0xFE0Fu}, "symbol variation evidence", "evidence", "symbol_variation", "sparkles + VS16"));
  items.push_back(MakeVisualCase({0x0048u, 0x2728u, 0x0045u, 0x2728u, 0xFE0Fu, 0x004Cu, 0x2728u, 0xFE0Eu, 0x004Cu, 0x004Fu},
                                 "symbol variation evidence",
                                 "evidence",
                                 "symbol_variation",
                                 "Latin mixed with plain/VS16/VS15 sparkles"));
  items.push_back(MakeVisualCase({0x1F975u, 0x1F636u, 0x200Du, 0x1F32Bu, 0xFE0Fu, 0x1F915u, 0x2728u, 0xFE0Eu, 0x2728u, 0x2728u, 0xFE0Fu, 0x1F973u},
                                 "symbol variation evidence",
                                 "evidence",
                                 "symbol_variation",
                                 "emoji sequence mixed with plain/VS16/VS15 sparkles"));
  items.push_back(MakeVisualCase({0x0048u, 0x0065u, 0x006Cu, 0x006Cu, 0x006Fu, 0x0020u, 0x0077u, 0x006Fu, 0x0072u, 0x006Cu, 0x0064u, 0x2728u, 0xFE0Fu, 0x2728u, 0xFE0Eu, 0x2728u, 0x1F973u, 0x1F975u, 0x1F915u},
                                 "symbol variation evidence",
                                 "evidence",
                                 "symbol_variation",
                                 "Latin, mixed sparkle variation, then emoji"));
  return items;
}

std::vector<VisualCase> BuildMixedTextEvidenceCases()
{
  std::vector<VisualCase> items;
  items.push_back(MakeVisualCase({0x0031u, 0x0032u, 0x003Au, 0x0033u, 0x0030u, 0x002Cu, 0x0020u, 0x0048u, 0x0065u, 0x006Cu, 0x006Cu, 0x006Fu, 0x003Au, 0x0057u, 0x006Fu, 0x0072u, 0x006Cu, 0x0064u, 0x003Au, 0x003Au, 0x0020u, 0x0031u, 0x0032u, 0x003Au, 0x0033u, 0x0034u, 0x003Au, 0x003Au, 0x0035u, 0x0036u},
                                 "mixed text evidence",
                                 "evidence",
                                 "mixed_text",
                                 "digits, colon punctuation, Latin text"));
  items.push_back(MakeVisualCase({0x0041u, 0x0020u, 0x0023u, 0x0020u, 0x002Au, 0x0020u, 0x0031u, 0x0020u, 0x0023u, 0xFE0Fu, 0x20E3u, 0x0020u, 0x002Au, 0xFE0Fu, 0x20E3u, 0x0020u, 0x0031u, 0xFE0Fu, 0x20E3u, 0x0020u, 0x005Au},
                                 "mixed text evidence",
                                 "evidence",
                                 "mixed_text",
                                 "ASCII #/*/digit next to keycap sequences"));
  items.push_back(MakeVisualCase({0x05D0u, 0x05D1u, 0x0020u, 0x2764u, 0xFE0Fu, 0x200Du, 0x1F525u, 0x0020u, 0x0645u, 0x0631u},
                                 "mixed text evidence",
                                 "evidence",
                                 "mixed_text",
                                 "Hebrew and Arabic guards around ZWJ emoji"));
  return items;
}

std::vector<VisualCase> BuildReviewCases(const std::vector<VisualCase>& fixtureItems)
{
  std::vector<VisualCase> items = BuildRepresentativeCases(fixtureItems);

  const std::vector<VisualCase> compatibilityCases = BuildCompatibilityCases();
  items.insert(items.end(), compatibilityCases.begin(), compatibilityCases.end());

  const std::vector<VisualCase> keycapCases = BuildKeycapEvidenceCases();
  items.insert(items.end(), keycapCases.begin(), keycapCases.end());

  const std::vector<VisualCase> symbolVariationCases = BuildSymbolVariationEvidenceCases();
  items.insert(items.end(), symbolVariationCases.begin(), symbolVariationCases.end());

  const std::vector<VisualCase> mixedTextCases = BuildMixedTextEvidenceCases();
  items.insert(items.end(), mixedTextCases.begin(), mixedTextCases.end());

  return items;
}

std::vector<VisualCase> ApplyCaseFilter(const std::vector<VisualCase>& items, const VisualOptions& options)
{
  std::vector<VisualCase> filtered;
  for(const VisualCase& item : items)
  {
    if(ShouldInclude(item, options))
    {
      filtered.push_back(item);
    }
  }
  return filtered;
}

std::vector<VisualCase> LoadVisualCases(const VisualOptions& options)
{
  std::vector<VisualCase> items;
  if(!options.inputPath.empty())
  {
    items = LoadFailureLog(options.inputPath, options);
  }
  else
  {
    const std::vector<VisualCase> fixtureItems = LoadFixture(options);
    if(options.mode == "review")
    {
      items = BuildReviewCases(fixtureItems);
    }
    else if(options.mode == "representative")
    {
      items = BuildRepresentativeCases(fixtureItems);
    }
    else if(options.mode == "full")
    {
      items = GroupCasesBySignature(fixtureItems);
    }
    else
    {
      throw std::runtime_error("unknown DALI_EMOJI_VISUAL_MODE: " + options.mode + " (expected review, representative, or full)");
    }
  }

  return ApplyCaseFilter(items, options);
}

Label MakeLabel(const std::string& text, float fontSize, const Vector4& color)
{
  Label label = Label::New();
  label.SetText(text.c_str());
  label.SetFontSize(fontSize);
  label.SetTextColor(color);
  label.SetMultiLine(true);
  label.SetHorizontalTextAlignment(Text::Alignment::START);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  return label;
}

std::string BuildRunDiagnosticText(Text::MultilanguageSupport& multilanguageSupport,
                                   TextAbstraction::FontClient& fontClient,
                                   const VisualCase&            item)
{
  try
  {
    Vector<Text::Character> text = MakeGuardedText(item.codepoints);
    Vector<Text::ScriptRun> scripts;
    multilanguageSupport.SetScripts(text, 0u, text.Count(), scripts);

    Vector<Text::FontDescriptionRun>       fontDescriptions;
    Vector<Text::FontRun>                  fonts;
    TextAbstraction::FontDescription       defaultFontDescription;
    const TextAbstraction::PointSize26Dot6 defaultFontPointSize = TextAbstraction::FontClient::DEFAULT_POINT_SIZE;

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

    std::stringstream stream;
    stream << "scriptRuns: " << ScriptRunsToString(scripts) << '\n'
           << "fontRuns: " << FontRunsToString(fontClient, fonts);

    Vector<Text::LineBreakInfo> lineBreakInfo;
    lineBreakInfo.Resize(text.Count(), TextAbstraction::LINE_NO_BREAK);

    Vector<Text::GlyphInfo>      glyphs;
    Vector<Text::CharacterIndex> glyphToCharacterMap;
    Vector<Text::Length>         charactersPerGlyph;
    Vector<Text::GlyphIndex>     newParagraphGlyphs;
    TextAbstraction::Shaping     shaping = TextAbstraction::Shaping::New();

    Text::ShapeText(shaping,
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

    const bool needsCompositionDiagnostics = IsExpectedComposedEmojiSequence(item);
    if(glyphToCharacterMap.Count() == glyphs.Count() && charactersPerGlyph.Count() == glyphs.Count())
    {
      stream << '\n'
             << "missingGlyphs: "
             << MissingGlyphsToString(glyphs, glyphToCharacterMap, charactersPerGlyph, 1u, text.Count() - 1u);

      if(needsCompositionDiagnostics)
      {
        fontClient.GetGlyphMetrics(glyphs.Begin(), glyphs.Count(), TextAbstraction::BITMAP_GLYPH, true);
        const std::string compositionGlyphs = CompositionGlyphsToString(item, glyphs, glyphToCharacterMap, charactersPerGlyph);
        if(!compositionGlyphs.empty())
        {
          stream << '\n'
                 << "compositionGlyphs: "
                 << compositionGlyphs;
        }
      }
    }
    else
    {
      stream << '\n' << "missingGlyphs: unavailable, inconsistent glyph maps";
      if(needsCompositionDiagnostics)
      {
        stream << '\n' << "compositionGlyphs: unavailable, inconsistent glyph maps";
      }
    }
    return stream.str();
  }
  catch(const std::exception& exception)
  {
    return std::string("run diagnostics failed: ") + exception.what();
  }
}

std::string MakeDetailText(Text::MultilanguageSupport& multilanguageSupport,
                           TextAbstraction::FontClient& fontClient,
                           const VisualCase&            item)
{
  std::stringstream stream;
  stream << SectionForCase(item) << '\n'
         << item.signature << " / " << item.status << '\n'
         << CodepointsToString(item.codepoints) << '\n'
         << item.name;
  if(!item.reason.empty())
  {
    stream << '\n' << item.reason;
  }
  stream << '\n' << BuildRunDiagnosticText(multilanguageSupport, fontClient, item);
  return stream.str();
}

std::string MakeBasicDetailText(const VisualCase& item)
{
  std::stringstream stream;
  stream << SectionForCase(item) << '\n'
         << item.signature << " / " << item.status << '\n'
         << CodepointsToString(item.codepoints) << '\n'
         << item.name;
  if(!item.reason.empty())
  {
    stream << '\n' << item.reason;
  }
  stream << "\nrun diagnostics: pending";
  return stream.str();
}

std::string SourcePathForOptions(const VisualOptions& options)
{
  if(!options.inputPath.empty())
  {
    return options.inputPath;
  }

  return "mode=" + options.mode + " fixture=" + options.fixturePath;
}

std::string SignatureFilterToString(const VisualOptions& options)
{
  std::stringstream stream;
  bool              first = true;
  for(const std::string& signature : options.signatureFilter)
  {
    stream << (first ? "" : ",") << signature;
    first = false;
  }
  return stream.str();
}

std::string MaxItemsToString(const VisualOptions& options)
{
  return options.maxItems == 0u ? "all" : std::to_string(options.maxItems);
}

std::string AnchorIdForSection(const std::string& section)
{
  std::string anchor("section-");
  for(char character : section)
  {
    const unsigned char value = static_cast<unsigned char>(character);
    if(std::isalnum(value))
    {
      anchor.push_back(static_cast<char>(std::tolower(value)));
    }
    else if(anchor.empty() || anchor.back() != '-')
    {
      anchor.push_back('-');
    }
  }
  return anchor;
}

std::vector<std::string> SectionsForShownItems(const std::vector<VisualCase>& items, size_t shownCount)
{
  std::set<std::string>   seenSections;
  std::vector<std::string> sections;

  for(size_t index = 0u; index < shownCount; ++index)
  {
    const std::string section = SectionForCase(items[index]);
    if(seenSections.insert(section).second)
    {
      sections.push_back(section);
    }
  }

  return sections;
}

std::vector<std::pair<std::string, size_t>> SectionAnchorsForShownItems(const std::vector<VisualCase>& items, size_t shownCount)
{
  std::set<std::string>                    seenSections;
  std::vector<std::pair<std::string, size_t>> sections;

  for(size_t index = 0u; index < shownCount; ++index)
  {
    const std::string section = SectionForCase(items[index]);
    if(seenSections.insert(section).second)
    {
      sections.push_back(std::make_pair(section, index));
    }
  }

  return sections;
}

void WriteHtmlExport(const VisualOptions&             options,
                     const std::vector<VisualCase>&  items,
                     Text::MultilanguageSupport&     multilanguageSupport,
                     TextAbstraction::FontClient&    fontClient)
{
  std::ofstream output(options.exportHtmlPath.c_str());
  if(!output.good())
  {
    throw std::runtime_error("failed to open visual export path: " + options.exportHtmlPath);
  }

  const size_t              shownCount = ShownCountForOptions(items, options);
  const std::vector<std::string> sections = SectionsForShownItems(items, shownCount);
  output << "<!doctype html>\n"
         << "<html lang=\"ko\">\n"
         << "<head>\n"
         << "<meta charset=\"utf-8\">\n"
         << "<title>Emoji Sequence Visual Export</title>\n"
         << "<style>\n"
         << "body{margin:0;background:#f5f7fb;color:#101828;font-family:Arial,'Noto Sans',sans-serif;}\n"
         << "header{padding:20px 24px;background:#172033;color:white;}\n"
         << "h1{margin:0 0 8px;font-size:22px;}\n"
         << "h2{margin:26px 0 12px;font-size:18px;}\n"
         << ".meta{font-size:13px;color:#d0d5dd;line-height:1.5;}\n"
         << ".nav{display:flex;flex-wrap:wrap;gap:8px;margin:16px 0 0;}\n"
         << ".nav a{display:inline-flex;align-items:center;min-height:28px;padding:0 10px;border-radius:999px;background:#344054;color:white;text-decoration:none;font-size:12px;}\n"
         << "main{padding:18px 24px;}\n"
         << ".case{display:grid;grid-template-columns:150px 1fr;gap:16px;margin:0 0 14px;padding:14px;background:white;border:1px solid #d0d5dd;border-radius:8px;}\n"
         << ".preview{display:flex;align-items:center;justify-content:center;min-height:126px;border:1px solid #eaecf0;border-radius:6px;background:#fff;font-size:52px;}\n"
         << ".label{font-size:13px;color:#667085;margin-bottom:6px;}\n"
         << "pre{white-space:pre-wrap;word-break:break-word;margin:0;font-family:'Ubuntu Mono',monospace;font-size:13px;line-height:1.45;}\n"
         << ".reason{color:#b42318;}\n"
         << "</style>\n"
         << "</head>\n"
         << "<body>\n"
         << "<header>\n"
         << "<h1>Emoji Sequence Visual Export</h1>\n"
         << "<div class=\"meta\">source=" << HtmlEscape(SourcePathForOptions(options))
         << "<br>mode=" << HtmlEscape(options.inputPath.empty() ? options.mode : "input")
         << "<br>selected=" << items.size()
         << " shown=" << shownCount
         << " max=" << HtmlEscape(MaxItemsToString(options));
  if(!options.signatureFilter.empty())
  {
    output << "<br>filter=" << HtmlEscape(SignatureFilterToString(options));
  }
  output << "</div>\n";

  if(!sections.empty())
  {
    output << "<nav class=\"nav\">";
    for(const std::string& section : sections)
    {
      output << "<a href=\"#" << HtmlEscape(AnchorIdForSection(section)) << "\">" << HtmlEscape(section) << "</a>";
    }
    output << "</nav>\n";
  }
  output << "</header>\n"
         << "<main>\n";

  std::string previousSection;
  for(size_t index = 0u; index < shownCount; ++index)
  {
    const VisualCase& item = items[index];
    const std::string section = SectionForCase(item);
    if(section != previousSection)
    {
      output << "<h2 id=\"" << HtmlEscape(AnchorIdForSection(section)) << "\">" << HtmlEscape(section) << "</h2>\n";
      previousSection = section;
    }
    output << "<section class=\"case\">\n"
           << "<div class=\"preview\">" << HtmlEscape(CodepointsToUtf8(item.codepoints)) << "</div>\n"
           << "<div>\n"
           << "<div class=\"label\">#" << index << " "
           << HtmlEscape(item.signature) << " / " << HtmlEscape(item.status) << "</div>\n"
           << "<pre>"
           << HtmlEscape(CodepointsToString(item.codepoints)) << "\n"
           << HtmlEscape(item.name);
    if(!item.reason.empty())
    {
      output << "\n<span class=\"reason\">" << HtmlEscape(item.reason) << "</span>";
    }
    output << "\n" << HtmlEscape(BuildRunDiagnosticText(multilanguageSupport, fontClient, item))
           << "</pre>\n"
           << "</div>\n"
           << "</section>\n";
  }

  if(shownCount == 0u)
  {
    output << "<section class=\"case\"><div class=\"preview\">-</div><pre>No emoji cases selected.</pre></section>\n";
  }

  output << "</main>\n"
         << "</body>\n"
         << "</html>\n";
}

int RunExportOnly(const VisualOptions& options)
{
  Text::MultilanguageSupport multilanguageSupport = Text::MultilanguageSupport::New(false);
  TextAbstraction::FontClient fontClient          = TextAbstraction::FontClient::New();
  const std::vector<VisualCase> items              = LoadVisualCases(options);
  WriteHtmlExport(options, items, multilanguageSupport, fontClient);
  std::cout << "Emoji visual export written: " << options.exportHtmlPath
            << " selected=" << items.size()
            << " shown=" << ShownCountForOptions(items, options) << '\n';
  return 0;
}

class EmojiVisualController : public ConnectionTracker
{
public:
  EmojiVisualController(Application& application, VisualOptions options)
  : mApplication(application),
    mOptions(std::move(options))
  {
    mApplication.InitSignal().Connect(this, &EmojiVisualController::OnInit);
  }

private:
  struct RowActors
  {
    AbsoluteLayout       row;
    Label                preview;
    Label                detail;
    AbsoluteLayoutParams rowParams;
    AbsoluteLayoutParams previewParams;
    AbsoluteLayoutParams detailParams;
    size_t               itemIndex{static_cast<size_t>(-1)};
  };

  void OnInit(Application application)
  {
    mWindow = application.GetWindow();
    mWindow.SetBackgroundColor(Color::WHITE);
    mWindow.KeyEventSignal().Connect(this, &EmojiVisualController::OnKeyEvent);
    mWindow.ResizeSignal().Connect(this, &EmojiVisualController::OnWindowResized);

    mMultilanguageSupport = Text::MultilanguageSupport::New(false);
    mFontClient           = TextAbstraction::FontClient::New();

    try
    {
      mItems = LoadVisualCases(mOptions);
    }
    catch(const std::exception& exception)
    {
      mItems.clear();
      mLoadError = exception.what();
    }

    BuildUi();

    if(!mOptions.exportHtmlPath.empty())
    {
      try
      {
        WriteHtmlExport(mOptions, mItems, mMultilanguageSupport, mFontClient);
        std::cout << "Emoji visual export written: " << mOptions.exportHtmlPath
                  << " selected=" << mItems.size()
                  << " shown=" << ShownCountForOptions(mItems, mOptions) << '\n';
      }
      catch(const std::exception& exception)
      {
        std::cerr << "emoji visual export failed: " << exception.what() << '\n';
      }
    }

    if(!mOptions.capturePath.empty())
    {
      mCaptureTimer = Timer::New(mOptions.captureDelayMs);
      mCaptureTimer.TickSignal().Connect(this, &EmojiVisualController::OnCaptureTimer);
      mCaptureTimer.Start();
    }
    else if(mOptions.exitAfterExport)
    {
      mApplication.Quit();
    }
  }

  void OnKeyEvent(Window /*window*/, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    const Dali::String keyName = event.GetKeyName();
    if(keyName == "Escape" || keyName == "BackSpace" || keyName == "XF86Back")
    {
      if(mIsPreviewOverlayVisible)
      {
        HidePreviewOverlay();
        return;
      }
      mApplication.Quit();
      return;
    }

    if(keyName == "n" || keyName == "N" || keyName == "Page_Down" || keyName == "PageDown")
    {
      NextPage();
    }
    else if(keyName == "p" || keyName == "P" || keyName == "Page_Up" || keyName == "PageUp")
    {
      PreviousPage();
    }
    else if(keyName == "bracketleft" || keyName == "BracketLeft" || keyName == "[")
    {
      PreviousSection();
    }
    else if(keyName == "bracketright" || keyName == "BracketRight" || keyName == "]")
    {
      NextSection();
    }
    else if(keyName == "Home")
    {
      GoToPageStart(0u);
    }
    else if(keyName == "End")
    {
      GoToPageStart(LastPageStart());
    }
  }

  void OnWindowResized(Window /*window*/, Window::WindowSize /*size*/)
  {
    UpdateLayoutBounds();
    UpdatePreviewOverlayBounds();
  }

  void BuildUi()
  {
    mRoot = StackLayout::New();
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    mRoot.SetBackgroundColor(Color::WHITE);

    mShownCount = ShownCountForOptions(mItems, mOptions);
    mSectionAnchors = SectionAnchorsForShownItems(mItems, mShownCount);
    mDetailTextCache.resize(mItems.size());
    mBasicDetailTextCache.resize(mItems.size());
    mDetailTextQueued.assign(mItems.size(), false);

    mHeader = MakeLabel(BuildHeaderText(), HEADER_SIZE, Color::BLACK);
    mHeader.SetRequestedWidth(MATCH_PARENT);
    mHeader.SetRequestedHeight(HEADER_HEIGHT);
    mHeader.SetBackgroundColor(Vector4(0.92f, 0.94f, 0.96f, 1.0f));
    mHeader.SetPadding(Extents(16, 16, 10, 10));
    mRoot.Add(mHeader);

    BuildNavigationBar();

    mScrollView = ScrollView::New()
      .SetScrollDirection(ScrollDirection::Vertical)
      .SetOverScrollMode(OverScrollMode::ContentScrolls)
      .SetBackgroundColor(Color::WHITE)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    mRoot.Add(mScrollView);

    BuildDetailDiagnosticTimer();
    BuildPageContentPool();
    BuildPreviewOverlay();

    mWindow.Add(mRoot);
  }

  std::string BuildHeaderText() const
  {
    std::stringstream stream;
    stream << "Emoji Visual Manual\n";
    stream << "source=" << SourcePathForOptions(mOptions)
           << " mode=" << (mOptions.inputPath.empty() ? mOptions.mode : "input")
           << " selected=" << mItems.size()
           << " shown=" << mShownCount
           << " max=" << MaxItemsToString(mOptions)
           << " pageSize=" << PageSize();
    if(mShownCount > 0u)
    {
      stream << " page=" << CurrentPageNumber() << "/" << PageCount()
             << " range=[" << mPageStartIndex << "," << PageEndIndex() << ")";
    }
    if(!mOptions.signatureFilter.empty())
    {
      stream << " filter=" << SignatureFilterToString(mOptions);
    }
    if(!mSectionAnchors.empty())
    {
      const size_t startSectionIndex = SectionIndexForItemIndex(mPageStartIndex);
      const size_t endSectionIndex   = SectionIndexForItemIndex(PageEndIndex() > 0u ? PageEndIndex() - 1u : mPageStartIndex);
      stream << "\n";
      if(startSectionIndex == endSectionIndex)
      {
        stream << "section=" << (startSectionIndex + 1u) << "/" << mSectionAnchors.size()
               << " " << mSectionAnchors[startSectionIndex].first;
      }
      else
      {
        stream << "sections=" << (startSectionIndex + 1u) << "-" << (endSectionIndex + 1u) << "/" << mSectionAnchors.size()
               << " " << mSectionAnchors[startSectionIndex].first
               << " -> " << mSectionAnchors[endSectionIndex].first;
      }
      stream << " keys: N/P page, [/ ] section, Home/End";
    }
    if(!mOptions.exportHtmlPath.empty())
    {
      stream << "\nexportHtml=" << mOptions.exportHtmlPath;
    }
    if(!mOptions.capturePath.empty())
    {
      stream << "\ncapture=" << mOptions.capturePath;
    }
    return stream.str();
  }

  void BuildNavigationBar()
  {
    mNavigation = FlexLayout::New()
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(NAV_HEIGHT)
      .SetDirection(FlexDirection::ROW)
      .SetWrap(FlexWrap::WRAP)
      .SetAlignItems(FlexAlign::FLEX_START);
    mNavigation.SetPadding(Extents(12, 12, 8, 6));
    mNavigation.SetBackgroundColor(Vector4(0.96f, 0.97f, 0.99f, 1.0f));

    mNavigation.Add(MakeNavButton("Prev page", [this]() { PreviousPage(); }, 120.0f, Vector4(0.84f, 0.88f, 0.95f, 1.0f)));
    mNavigation.Add(MakeNavButton("Next page", [this]() { NextPage(); }, 120.0f, Vector4(0.84f, 0.88f, 0.95f, 1.0f)));

    for(size_t sectionIndex = 0u; sectionIndex < mSectionAnchors.size(); ++sectionIndex)
    {
      std::stringstream label;
      label << (sectionIndex + 1u) << ". " << mSectionAnchors[sectionIndex].first;
      mNavigation.Add(MakeNavButton(label.str(), [this, sectionIndex]() { JumpToSection(sectionIndex); }, NAV_BUTTON_WIDTH, Vector4(0.90f, 0.93f, 0.98f, 1.0f)));
    }

    mRoot.Add(mNavigation);
  }

  Label MakeNavButton(const std::string& text,
                      const std::function<void()>& action,
                      float width,
                      const Vector4& backgroundColor)
  {
    Label button = Label::New()
      .SetRequestedWidth(width)
      .SetRequestedHeight(NAV_BUTTON_HEIGHT)
      .SetText(text.c_str())
      .SetFontSize(NAV_SIZE)
      .SetTextColor(Color::BLACK)
      .SetMultiLine(false)
      .SetOverflowMode(Text::OverflowMode::ELLIPSIS)
      .SetAsyncRendering(false)
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .SetParentOrigin(ParentOrigin::TOP_LEFT)
      .SetPivot(Pivot::TOP_LEFT);
    button.SetBackgroundColor(backgroundColor);
    button.SetMargin(Extents(0, 6, 0, 6));
    button.SetPadding(Extents(6, 6, 0, 0));
    button.TouchedSignal().Connect(this, [action, backgroundColor](Actor actor, const TouchEvent& touch) {
      Label button = Label::DownCast(actor);
      if(button)
      {
        button.SetBackgroundColor(backgroundColor);
      }

      const PointState::Type state = touch.GetState(0u);
      if(state == PointState::UP || state == PointState::FINISHED)
      {
        action();
      }
      return true;
    });
    return button;
  }

  size_t PageSize() const
  {
    return mOptions.pageSize == 0u ? std::max<size_t>(1u, mShownCount) : mOptions.pageSize;
  }

  size_t PageCount() const
  {
    if(mShownCount == 0u)
    {
      return 0u;
    }
    const size_t pageSize = PageSize();
    return (mShownCount + pageSize - 1u) / pageSize;
  }

  size_t CurrentPageNumber() const
  {
    return mShownCount == 0u ? 0u : (mPageStartIndex / PageSize()) + 1u;
  }

  size_t PageEndIndex() const
  {
    return std::min(mShownCount, mPageStartIndex + PageSize());
  }

  size_t LastPageStart() const
  {
    if(mShownCount == 0u)
    {
      return 0u;
    }
    const size_t pageSize = PageSize();
    return ((mShownCount - 1u) / pageSize) * pageSize;
  }

  size_t NormalizePageStart(size_t index) const
  {
    if(mShownCount == 0u)
    {
      return 0u;
    }
    const size_t pageSize = PageSize();
    return std::min((index / pageSize) * pageSize, LastPageStart());
  }

  void GoToPageStart(size_t index)
  {
    const size_t pageStart = NormalizePageStart(index);
    if(pageStart == mPageStartIndex && mScrollView)
    {
      mScrollView.SetScrollPosition(Vector2::ZERO);
      return;
    }
    mPageStartIndex = pageStart;
    UpdatePageContent();
  }

  void NextPage()
  {
    if(mShownCount == 0u || mPageStartIndex >= LastPageStart())
    {
      return;
    }
    GoToPageStart(mPageStartIndex + PageSize());
  }

  void PreviousPage()
  {
    if(mShownCount == 0u || mPageStartIndex == 0u)
    {
      return;
    }
    const size_t pageSize = PageSize();
    GoToPageStart(mPageStartIndex > pageSize ? mPageStartIndex - pageSize : 0u);
  }

  size_t CurrentSectionIndex() const
  {
    return SectionIndexForItemIndex(mPageStartIndex);
  }

  size_t SectionIndexForItemIndex(size_t itemIndex) const
  {
    if(mSectionAnchors.empty())
    {
      return 0u;
    }

    const size_t clampedItemIndex = mShownCount == 0u ? 0u : std::min(itemIndex, mShownCount - 1u);
    size_t current = 0u;
    for(size_t index = 0u; index < mSectionAnchors.size(); ++index)
    {
      if(mSectionAnchors[index].second <= clampedItemIndex)
      {
        current = index;
      }
      else
      {
        break;
      }
    }
    return current;
  }

  void JumpToSection(size_t sectionIndex)
  {
    if(sectionIndex >= mSectionAnchors.size())
    {
      return;
    }
    GoToPageStart(mSectionAnchors[sectionIndex].second);
  }

  void NextSection()
  {
    if(mSectionAnchors.empty())
    {
      return;
    }
    const size_t sectionIndex = CurrentSectionIndex();
    if(sectionIndex + 1u < mSectionAnchors.size())
    {
      JumpToSection(sectionIndex + 1u);
    }
  }

  void PreviousSection()
  {
    if(mSectionAnchors.empty())
    {
      return;
    }
    const size_t sectionIndex = CurrentSectionIndex();
    if(sectionIndex > 0u)
    {
      JumpToSection(sectionIndex - 1u);
    }
  }

  float RowContentHeight() const
  {
    return std::max(0.0f, ROW_HEIGHT - ROW_PADDING_Y * 2.0f);
  }

  float MinimumRowWidth() const
  {
    return ROW_PADDING_X * 2.0f + PREVIEW_WIDTH + MIN_DETAIL_WIDTH;
  }

  float PageContentWidth() const
  {
    float windowWidth = 0.0f;
    if(mWindow)
    {
      const auto size = mWindow.GetSize();
      windowWidth     = static_cast<float>(size.GetWidth());
    }
    return std::max(MinimumRowWidth() + PAGE_PADDING_X * 2.0f, windowWidth);
  }

  float RowWidth() const
  {
    return std::max(MinimumRowWidth(), PageContentWidth() - PAGE_PADDING_X * 2.0f);
  }

  float DetailWidth(float rowWidth) const
  {
    return std::max(MIN_DETAIL_WIDTH, rowWidth - ROW_PADDING_X * 2.0f - PREVIEW_WIDTH);
  }

  float PageContentHeight(size_t rowCount) const
  {
    if(rowCount == 0u)
    {
      return PAGE_PADDING_Y * 2.0f;
    }
    return PAGE_PADDING_Y * 2.0f + static_cast<float>(rowCount) * ROW_HEIGHT + static_cast<float>(rowCount - 1u) * ROW_SPACING;
  }

  size_t CurrentVisibleRowCount() const
  {
    return PageEndIndex() - mPageStartIndex;
  }

  float RowY(size_t slot) const
  {
    return PAGE_PADDING_Y + static_cast<float>(slot) * (ROW_HEIGHT + ROW_SPACING);
  }

  void UpdateLayoutBounds()
  {
    if(!mPageContent)
    {
      return;
    }

    const size_t visibleRows = mShownCount == 0u ? 1u : CurrentVisibleRowCount();
    const float  pageWidth   = PageContentWidth();
    const float  rowWidth    = std::max(MinimumRowWidth(), pageWidth - PAGE_PADDING_X * 2.0f);
    const bool   widthChanged = pageWidth != mLastPageContentWidth;
    const bool   rowCountChanged = visibleRows != mLastVisibleRowCount;

    if(widthChanged)
    {
      mPageContent.SetRequestedWidth(pageWidth);
    }
    if(widthChanged || rowCountChanged)
    {
      mPageContent.SetRequestedHeight(PageContentHeight(visibleRows));
    }

    if(mShownCount == 0u)
    {
      if(widthChanged && mEmptyLabel && mEmptyLabelParams)
      {
        mEmptyLabel.SetRequestedWidth(rowWidth);
        mEmptyLabelParams.SetBounds(LayoutRect(PAGE_PADDING_X, PAGE_PADDING_Y, rowWidth, ROW_HEIGHT));
      }
      mLastPageContentWidth = pageWidth;
      mLastVisibleRowCount  = visibleRows;
      return;
    }

    if(widthChanged)
    {
      for(size_t slot = 0u; slot < mRowPool.size(); ++slot)
      {
        UpdateRowBounds(mRowPool[slot], slot, rowWidth);
      }
    }

    mLastPageContentWidth = pageWidth;
    mLastVisibleRowCount  = visibleRows;
  }

  void UpdateRowBounds(RowActors& row, size_t slot, float rowWidth)
  {
    const float contentHeight = RowContentHeight();
    const float detailWidth   = DetailWidth(rowWidth);

    row.row.SetRequestedWidth(rowWidth);
    row.row.SetRequestedHeight(ROW_HEIGHT);
    row.rowParams.SetBounds(LayoutRect(PAGE_PADDING_X, RowY(slot), rowWidth, ROW_HEIGHT));

    row.preview.SetRequestedWidth(PREVIEW_WIDTH);
    row.preview.SetRequestedHeight(contentHeight);
    row.previewParams.SetBounds(LayoutRect(ROW_PADDING_X, ROW_PADDING_Y, PREVIEW_WIDTH, contentHeight));

    row.detail.SetRequestedWidth(detailWidth);
    row.detail.SetRequestedHeight(contentHeight);
    row.detailParams.SetBounds(LayoutRect(ROW_PADDING_X + PREVIEW_WIDTH, ROW_PADDING_Y, detailWidth, contentHeight));
  }

  void BuildPageContentPool()
  {
    mPageContent = AbsoluteLayout::New();
    mPageContent.SetRequestedWidth(PageContentWidth());
    mPageContent.SetRequestedHeight(PageContentHeight(mShownCount == 0u ? 1u : std::min(PageSize(), mShownCount)));

    if(mShownCount == 0u)
    {
      mEmptyLabel = MakeLabel(mLoadError.empty() ? "No emoji cases selected." : mLoadError, DETAIL_SIZE, Color::BLACK);
      mEmptyLabel.SetRequestedWidth(RowWidth());
      mEmptyLabel.SetRequestedHeight(ROW_HEIGHT);
      mEmptyLabel.SetBackgroundColor(Vector4(0.98f, 0.88f, 0.88f, 1.0f));
      mEmptyLabel.SetPadding(Extents(16, 16, 10, 10));
      mEmptyLabelParams = AbsoluteLayoutParams::New();
      mEmptyLabel.SetLayoutParams(mEmptyLabelParams);
      mPageContent.Add(mEmptyLabel);
    }
    else
    {
      const size_t rowCount = std::min(PageSize(), mShownCount);
      mRowPool.reserve(rowCount);
      for(size_t slot = 0u; slot < rowCount; ++slot)
      {
        RowActors row = MakeRowSlot();
        mPageContent.Add(row.row);
        mRowPool.push_back(row);
      }
    }

    mScrollView.SetContent(mPageContent);
    UpdatePageContent();
  }

  void BuildDetailDiagnosticTimer()
  {
    mDetailDiagnosticTimer = Timer::New(DETAIL_DIAGNOSTIC_TIMER_MS);
    mDetailDiagnosticTimer.TickSignal().Connect(this, &EmojiVisualController::OnDetailDiagnosticTimer);
  }

  void BuildPreviewOverlay()
  {
    mPreviewOverlay = AbsoluteLayout::New();
    mPreviewOverlay.SetLayoutMode(LayoutMode::STANDALONE);
    mPreviewOverlay.SetRequestedPositionX(0.0f);
    mPreviewOverlay.SetRequestedPositionY(0.0f);
    mPreviewOverlay.SetBackgroundColor(Vector4(0.0f, 0.0f, 0.0f, 0.70f));
    mPreviewOverlay.SetProperty(Actor::Property::VISIBLE, false);
    mPreviewOverlay.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
    mPreviewOverlay.TouchedSignal().Connect(this, &EmojiVisualController::OnPreviewOverlayTouched);

    mLargePreview = MakeLabel(std::string(), PREVIEW_POPUP_TEXT_FIT_MAX_SIZE, Color::BLACK);
    mLargePreview.SetMultiLine(false);
    mLargePreview.SetTextFit(Text::FitRange(PREVIEW_POPUP_TEXT_FIT_MIN_SIZE,
                                            PREVIEW_POPUP_TEXT_FIT_MAX_SIZE,
                                            PREVIEW_POPUP_TEXT_FIT_STEP_SIZE));
    mLargePreview.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mLargePreview.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mLargePreview.SetBackgroundColor(Color::WHITE);
    mLargePreview.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
    mLargePreview.TouchedSignal().Connect(this, &EmojiVisualController::OnPreviewOverlayTouched);
    mLargePreviewParams = AbsoluteLayoutParams::New();
    mLargePreview.SetLayoutParams(mLargePreviewParams);
    mPreviewOverlay.Add(mLargePreview);

    mRoot.Add(mPreviewOverlay);
    UpdatePreviewOverlayBounds();
  }

  void UpdatePreviewOverlayBounds()
  {
    if(!mPreviewOverlay)
    {
      return;
    }

    const auto  size   = mWindow.GetSize();
    const float width  = static_cast<float>(size.GetWidth());
    const float height = static_cast<float>(size.GetHeight());
    const float inset  = std::min(PREVIEW_POPUP_MARGIN, std::min(width, height) * 0.10f);

    mPreviewOverlay.SetRequestedWidth(width);
    mPreviewOverlay.SetRequestedHeight(height);

    if(mLargePreview && mLargePreviewParams)
    {
      const float previewWidth  = std::max(1.0f, std::min(width - inset * 2.0f, PREVIEW_POPUP_MAX_WIDTH));
      const float previewHeight = std::max(1.0f, std::min(height - inset * 2.0f, PREVIEW_POPUP_MAX_HEIGHT));
      const float previewX      = (width - previewWidth) * 0.5f;
      const float previewY      = (height - previewHeight) * 0.5f;
      mLargePreview.SetRequestedWidth(previewWidth);
      mLargePreview.SetRequestedHeight(previewHeight);
      mLargePreviewParams.SetBounds(LayoutRect(previewX, previewY, previewWidth, previewHeight));
    }
  }

  void UpdatePageContent()
  {
    ResetPendingDetailDiagnostics();
    UpdateLayoutBounds();

    const size_t pageEnd = PageEndIndex();
    for(size_t slot = 0u; slot < mRowPool.size(); ++slot)
    {
      const size_t itemIndex = mPageStartIndex + slot;
      RowActors& row = mRowPool[slot];
      if(itemIndex < pageEnd)
      {
        UpdateRow(row, mItems[itemIndex], itemIndex);
      }
      else
      {
        HideRow(row);
      }
    }

    mScrollView.SetScrollPosition(Vector2::ZERO);
    if(mHeader)
    {
      mHeader.SetText(BuildHeaderText().c_str());
    }
    StartDetailDiagnosticTimerIfNeeded();
  }

  void ResetPendingDetailDiagnostics()
  {
    if(mDetailDiagnosticTimer && mDetailDiagnosticTimer.IsRunning())
    {
      mDetailDiagnosticTimer.Stop();
    }

    for(size_t pendingIndex = mPendingDetailTextIndex; pendingIndex < mPendingDetailTextIndices.size(); ++pendingIndex)
    {
      const size_t itemIndex = mPendingDetailTextIndices[pendingIndex];
      if(itemIndex < mDetailTextQueued.size() && mDetailTextCache[itemIndex].empty())
      {
        mDetailTextQueued[itemIndex] = false;
      }
    }
    mPendingDetailTextIndices.clear();
    mPendingDetailTextIndex = 0u;
  }

  void StartDetailDiagnosticTimerIfNeeded()
  {
    if(mPendingDetailTextIndex >= mPendingDetailTextIndices.size())
    {
      return;
    }

    if(mDetailDiagnosticTimer && !mDetailDiagnosticTimer.IsRunning())
    {
      mDetailDiagnosticTimer.Start();
    }
  }

  bool OnDetailDiagnosticTimer()
  {
    size_t processedCount = 0u;
    while(mPendingDetailTextIndex < mPendingDetailTextIndices.size() &&
          processedCount < DETAIL_DIAGNOSTIC_BATCH_SIZE)
    {
      const size_t itemIndex = mPendingDetailTextIndices[mPendingDetailTextIndex++];
      if(itemIndex >= mItems.size())
      {
        continue;
      }

      if(itemIndex < mDetailTextQueued.size())
      {
        mDetailTextQueued[itemIndex] = false;
      }

      if(mDetailTextCache[itemIndex].empty())
      {
        mDetailTextCache[itemIndex] = ::MakeDetailText(mMultilanguageSupport, mFontClient, mItems[itemIndex]);
        ++processedCount;
      }

      UpdateVisibleDetailText(itemIndex);
    }

    if(mPendingDetailTextIndex >= mPendingDetailTextIndices.size())
    {
      mPendingDetailTextIndices.clear();
      mPendingDetailTextIndex = 0u;
      return false;
    }

    return true;
  }

  bool OnCaptureTimer()
  {
    if(!mRoot)
    {
      return false;
    }

    const auto size = mWindow.GetSize();
    mCapture = Capture::New();
    mCapture.FinishedSignal().Connect(this, &EmojiVisualController::OnCaptureFinished);
    mCapture.Start(mRoot,
                   Vector2(static_cast<float>(size.GetWidth()), static_cast<float>(size.GetHeight())),
                   mOptions.capturePath.c_str(),
                   Color::WHITE);
    return false;
  }

  void OnCaptureFinished(Capture /*capture*/, Capture::FinishState state)
  {
    if(state == Capture::FinishState::SUCCEEDED)
    {
      std::cout << "Emoji visual capture written: " << mOptions.capturePath << '\n';
    }
    else
    {
      std::cerr << "emoji visual capture failed: " << mOptions.capturePath << '\n';
    }

    if(mOptions.exitAfterCapture || mOptions.exitAfterExport)
    {
      mApplication.Quit();
    }
  }

  const std::string& BasicDetailTextForIndex(size_t index)
  {
    if(mBasicDetailTextCache[index].empty())
    {
      mBasicDetailTextCache[index] = ::MakeBasicDetailText(mItems[index]);
    }
    return mBasicDetailTextCache[index];
  }

  void QueueDetailDiagnostic(size_t index)
  {
    if(index >= mItems.size() || !mDetailTextCache[index].empty() ||
       (index < mDetailTextQueued.size() && mDetailTextQueued[index]))
    {
      return;
    }

    if(index < mDetailTextQueued.size())
    {
      mDetailTextQueued[index] = true;
    }
    mPendingDetailTextIndices.push_back(index);
  }

  const std::string& VisibleDetailTextForIndex(size_t index)
  {
    if(mDetailTextCache[index].empty())
    {
      QueueDetailDiagnostic(index);
      return BasicDetailTextForIndex(index);
    }
    return mDetailTextCache[index];
  }

  void UpdateVisibleDetailText(size_t itemIndex)
  {
    for(RowActors& row : mRowPool)
    {
      if(row.itemIndex == itemIndex)
      {
        row.detail.SetText(VisibleDetailTextForIndex(itemIndex).c_str());
        return;
      }
    }
  }

  RowActors MakeRowSlot()
  {
    RowActors actors;

    actors.row = AbsoluteLayout::New()
      .SetRequestedWidth(RowWidth())
      .SetRequestedHeight(ROW_HEIGHT);
    actors.rowParams = AbsoluteLayoutParams::New();
    actors.row.SetLayoutParams(actors.rowParams);

    actors.preview = MakeLabel(std::string(), mOptions.fontSize, Color::BLACK);
    actors.preview.SetRequestedWidth(PREVIEW_WIDTH);
    actors.preview.SetRequestedHeight(RowContentHeight());
    actors.preview.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    actors.preview.SetVerticalTextAlignment(Text::Alignment::CENTER);
    actors.preview.SetBackgroundColor(Color::WHITE);
    actors.previewParams = AbsoluteLayoutParams::New();
    actors.preview.SetLayoutParams(actors.previewParams);
    actors.preview.TouchedSignal().Connect(this, &EmojiVisualController::OnPreviewTouched);
    actors.row.Add(actors.preview);

    actors.detail = MakeLabel(std::string(), DETAIL_SIZE, Color::BLACK);
    actors.detail.SetFontFamily("Ubuntu Mono");
    actors.detail.SetRequestedWidth(DetailWidth(RowWidth()));
    actors.detail.SetRequestedHeight(RowContentHeight());
    actors.detail.SetPadding(Extents(14, 8, 0, 0));
    actors.detailParams = AbsoluteLayoutParams::New();
    actors.detail.SetLayoutParams(actors.detailParams);
    actors.row.Add(actors.detail);

    return actors;
  }

  void UpdateRow(RowActors& row, const VisualCase& item, size_t index)
  {
    row.itemIndex = index;
    row.row.SetProperty(Actor::Property::VISIBLE, true);
    row.row.SetBackgroundColor(index % 2u == 0u ? Vector4(0.98f, 0.98f, 0.98f, 1.0f) : Vector4(0.94f, 0.96f, 0.98f, 1.0f));
    row.preview.SetText(CodepointsToUtf8(item.codepoints).c_str());
    row.detail.SetText(VisibleDetailTextForIndex(index).c_str());
  }

  void HideRow(RowActors& row)
  {
    row.itemIndex = static_cast<size_t>(-1);
    row.row.SetProperty(Actor::Property::VISIBLE, false);
  }

  bool OnPreviewTouched(Actor actor, TouchEvent touch)
  {
    const PointState::Type state = touch.GetState(0u);
    if(state != PointState::UP && state != PointState::FINISHED)
    {
      return true;
    }

    for(const RowActors& row : mRowPool)
    {
      if(row.preview == actor && row.itemIndex < mShownCount)
      {
        ShowPreviewOverlay(mItems[row.itemIndex]);
        return true;
      }
    }

    return true;
  }

  bool OnPreviewOverlayTouched(Actor /*actor*/, TouchEvent touch)
  {
    const PointState::Type state = touch.GetState(0u);
    if(state == PointState::UP || state == PointState::FINISHED)
    {
      HidePreviewOverlay();
    }
    return true;
  }

  void ShowPreviewOverlay(const VisualCase& item)
  {
    if(!mPreviewOverlay || !mLargePreview)
    {
      return;
    }

    mLargePreview.SetText(CodepointsToUtf8(item.codepoints).c_str());
    mPreviewOverlay.SetProperty(Actor::Property::VISIBLE, true);
    mPreviewOverlay.RaiseToTop();
    mIsPreviewOverlayVisible = true;
  }

  void HidePreviewOverlay()
  {
    if(mPreviewOverlay)
    {
      mPreviewOverlay.SetProperty(Actor::Property::VISIBLE, false);
    }
    mIsPreviewOverlayVisible = false;
  }

private:
  Application&               mApplication;
  VisualOptions              mOptions;
  Window                     mWindow;
  StackLayout                mRoot;
  Label                      mHeader;
  AbsoluteLayout             mPageContent;
  AbsoluteLayout             mPreviewOverlay;
  FlexLayout                 mNavigation;
  ScrollView                 mScrollView;
  Label                      mEmptyLabel;
  Label                      mLargePreview;
  AbsoluteLayoutParams       mEmptyLabelParams;
  AbsoluteLayoutParams       mLargePreviewParams;
  Timer                      mCaptureTimer;
  Timer                      mDetailDiagnosticTimer;
  Capture                    mCapture;
  Text::MultilanguageSupport mMultilanguageSupport;
  TextAbstraction::FontClient mFontClient;
  std::vector<VisualCase>    mItems;
  std::vector<RowActors>     mRowPool;
  std::vector<std::string>   mDetailTextCache;
  std::vector<std::string>   mBasicDetailTextCache;
  std::vector<bool>          mDetailTextQueued;
  std::vector<size_t>        mPendingDetailTextIndices;
  std::vector<std::pair<std::string, size_t>> mSectionAnchors;
  std::string                mLoadError;
  size_t                     mShownCount{0u};
  size_t                     mPageStartIndex{0u};
  size_t                     mPendingDetailTextIndex{0u};
  float                      mLastPageContentWidth{-1.0f};
  size_t                     mLastVisibleRowCount{static_cast<size_t>(-1)};
  bool                       mIsPreviewOverlayVisible{false};
};

} // namespace

int DALI_EXPORT_API main(int argc, char** argv)
{
  try
  {
    const VisualOptions options = LoadOptions();
    if(options.exportOnly)
    {
      if(options.exportHtmlPath.empty())
      {
        throw std::runtime_error("DALI_EMOJI_VISUAL_EXPORT_ONLY requires DALI_EMOJI_VISUAL_EXPORT_HTML");
      }
      return RunExportOnly(options);
    }

    Application application = Application::New(&argc, &argv);
    UiConfig config = UiConfig::New();
    config.SetLabelAsyncRendering(true);
    config.Apply();
    EmojiVisualController controller(application, options);
    application.MainLoop();
  }
  catch(const std::exception& exception)
  {
    std::cerr << "emoji visual manual failed: " << exception.what() << '\n';
    return 2;
  }

  return 0;
}
