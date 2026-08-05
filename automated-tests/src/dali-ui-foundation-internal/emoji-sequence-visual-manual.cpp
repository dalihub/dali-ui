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
#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-foundation/internal/text/multi-language-support.h>
#include <dali-ui-foundation/internal/text/shaper.h>
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/devel-api/text-abstraction/shaping.h>
#include <dali/public-api/adaptor-framework/capture.h>
#include <dali/public-api/adaptor-framework/clipboard.h>
#include <dali/public-api/adaptor-framework/clipboard-data.h>
#include <dali/public-api/adaptor-framework/timer.h>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
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

constexpr float HEADER_MIN_HEIGHT                = 50.0f;
constexpr float HEADER_LINE_HEIGHT               = 17.0f;
constexpr float ROW_HEIGHT                       = 230.0f;
constexpr float ROW_SPACING                      = 8.0f;
constexpr float PAGE_PADDING_X                   = 12.0f;
constexpr float PAGE_PADDING_Y                   = 12.0f;
constexpr float ROW_PADDING_X                    = 10.0f;
constexpr float ROW_PADDING_Y                    = 8.0f;
constexpr float PREVIEW_WIDTH                    = 160.0f;
constexpr float PREVIEW_SIZE                     = 52.0f;
constexpr float SAMPLE_PREVIEW_GAP               = 8.0f;
constexpr float SAMPLE_IMAGE_SCALE               = 1.12f;
constexpr float SAMPLE_SET_LABEL_HEIGHT          = 22.0f;
constexpr float SAMPLE_SET_LABEL_SIZE            = 11.0f;
constexpr float SAMPLE_PLACEHOLDER_SIZE           = 13.0f;
constexpr float DETAIL_SIZE                      = 15.0f;
constexpr float MIN_DETAIL_WIDTH                 = 220.0f;
constexpr float HEADER_SIZE                      = 12.0f;
constexpr float NAV_BUTTON_WIDTH                 = 150.0f;
constexpr float NAV_PAGE_BUTTON_WIDTH            = 96.0f;
constexpr float NAV_BUTTON_HEIGHT                = 26.0f;
constexpr float NAV_SIZE                         = 12.0f;
constexpr float NAV_PADDING_X                    = 8.0f;
constexpr float NAV_PADDING_Y                    = 6.0f;
constexpr float NAV_BUTTON_GAP_X                 = 4.0f;
constexpr float NAV_BUTTON_GAP_Y                 = 4.0f;
constexpr float FLOATING_SEARCH_MARGIN           = 18.0f;
constexpr float FLOATING_SEARCH_BUTTON_SIZE      = 48.0f;
constexpr float FLOATING_SEARCH_INPUT_HEIGHT     = 42.0f;
constexpr float FLOATING_SEARCH_INPUT_MAX_WIDTH  = 420.0f;
constexpr float FLOATING_SEARCH_INPUT_MIN_WIDTH  = 220.0f;
constexpr float FLOATING_SEARCH_FONT_SIZE        = 14.0f;
constexpr float FLOATING_SEARCH_ICON_SIZE        = 23.0f;
constexpr float FLOATING_SEARCH_ANIMATION_TIME   = 0.18f;
constexpr float PREVIEW_POPUP_MARGIN             = 64.0f;
constexpr float PREVIEW_POPUP_MAX_WIDTH          = 800.0f;
constexpr float PREVIEW_POPUP_MAX_HEIGHT         = 800.0f;
constexpr float PREVIEW_POPUP_TEXT_FIT_MIN_SIZE  = 20.0f;
constexpr float PREVIEW_POPUP_TEXT_FIT_MAX_SIZE  = 600.0f;
constexpr float PREVIEW_POPUP_TEXT_FIT_STEP_SIZE = 10.0f;
constexpr size_t DEFAULT_PAGE_SIZE               = 100u;
constexpr uint32_t DETAIL_DIAGNOSTIC_TIMER_MS    = 16u;
constexpr size_t DETAIL_DIAGNOSTIC_BATCH_SIZE    = 2u;
constexpr const char* MIME_TYPE_TEXT_PLAIN        = "text/plain;charset=utf-8";
constexpr const char* SEARCH_BUTTON_EMOJI         = "\xF0\x9F\x94\x8D\xEF\xB8\x8F"; // U+1F50D U+FE0F

struct VisualCase
{
  std::vector<uint32_t> codepoints;
  std::string           section;
  std::string           status;
  std::string           signature;
  std::string           name;
  std::string           reason;
};

struct SampleOptions
{
  bool        enabled{false};
  std::string directory;
  std::string sampleSet{"sample"};
  std::string displaySampleName{"Unicode chart sample"};
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
  SampleOptions         sample;
  bool                  exportOnly{false};
  bool                  exitAfterExport{false};
  bool                  exitAfterCapture{false};
};

struct FloatingSearchBounds
{
  float buttonX{0.0f};
  float buttonY{0.0f};
  float buttonSize{FLOATING_SEARCH_BUTTON_SIZE};
  float inputX{0.0f};
  float inputY{0.0f};
  float inputWidth{FLOATING_SEARCH_INPUT_MIN_WIDTH};
  float inputHeight{FLOATING_SEARCH_INPUT_HEIGHT};
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

std::string CodepointsToString(const std::vector<uint32_t>& codepoints, bool prefixed = false)
{
  std::stringstream stream;
  for(size_t index = 0u; index < codepoints.size(); ++index)
  {
    if(index > 0u)
    {
      stream << ' ';
    }
    if(prefixed)
    {
      stream << "U+";
    }
    stream << std::uppercase << std::hex << codepoints[index] << std::dec;
  }
  return stream.str();
}

std::string ToLowerAscii(const std::string& text)
{
  std::string lower;
  lower.reserve(text.size());
  for(char character : text)
  {
    const unsigned char value = static_cast<unsigned char>(character);
    if(value >= 'A' && value <= 'Z')
    {
      lower.push_back(static_cast<char>(value - 'A' + 'a'));
    }
    else
    {
      lower.push_back(character);
    }
  }
  return lower;
}

std::string TrimAscii(const std::string& text)
{
  size_t begin = 0u;
  while(begin < text.size() && std::isspace(static_cast<unsigned char>(text[begin])))
  {
    ++begin;
  }

  size_t end = text.size();
  while(end > begin && std::isspace(static_cast<unsigned char>(text[end - 1u])))
  {
    --end;
  }

  return text.substr(begin, end - begin);
}

std::string JoinPath(const std::string& directory, const std::string& name)
{
  if(directory.empty())
  {
    return name;
  }
  if(name.empty())
  {
    return directory;
  }
  if(directory[directory.size() - 1u] == '/')
  {
    return directory + name;
  }
  return directory + "/" + name;
}

bool FileExists(const std::string& path)
{
  std::ifstream input(path.c_str(), std::ios::binary);
  return input.good();
}

std::string SequenceKeyForCodepoints(const std::vector<uint32_t>& codepoints)
{
  if(codepoints.empty())
  {
    return std::string();
  }

  std::stringstream stream;
  for(size_t index = 0u; index < codepoints.size(); ++index)
  {
    if(index > 0u)
    {
      stream << '-';
    }
    stream << std::nouppercase << std::hex << codepoints[index] << std::dec;
  }
  return stream.str();
}

std::string SampleDisplayName(const std::string& sampleSet)
{
  if(sampleSet == "sample")
  {
    return "Unicode chart sample";
  }
  if(sampleSet == "google")
  {
    return "Google sample";
  }
  if(sampleSet == "apple")
  {
    return "Apple sample";
  }
  if(sampleSet == "samsung")
  {
    return "Samsung sample";
  }
  if(sampleSet == "windows")
  {
    return "Windows sample";
  }
  if(sampleSet == "twitter")
  {
    return "Twitter sample";
  }
  if(sampleSet == "facebook")
  {
    return "Facebook sample";
  }
  return sampleSet + " sample";
}

SampleOptions LoadSampleOptions()
{
  SampleOptions options;

  const char* sampleDir = GetEnv("DALI_EMOJI_VISUAL_SAMPLE_DIR");
  if(!sampleDir || sampleDir[0] == '\0')
  {
    return options;
  }

  options.enabled   = true;
  options.directory = sampleDir;

  const char* sampleSet = GetEnv("DALI_EMOJI_VISUAL_SAMPLE_SET");
  if(!sampleSet || sampleSet[0] == '\0')
  {
    sampleSet = GetEnv("DALI_EMOJI_VISUAL_SAMPLE_VENDOR");
  }
  if(sampleSet && sampleSet[0] != '\0')
  {
    options.sampleSet = ToLowerAscii(TrimAscii(sampleSet));
  }
  if(options.sampleSet.empty())
  {
    options.sampleSet = "sample";
  }
  options.displaySampleName = SampleDisplayName(options.sampleSet);
  return options;
}

std::string SampleImagePathForCase(const SampleOptions& options, const VisualCase& item)
{
  if(!options.enabled)
  {
    return std::string();
  }

  const std::string sequenceKey = SequenceKeyForCodepoints(item.codepoints);
  if(sequenceKey.empty())
  {
    return std::string();
  }

  return JoinPath(JoinPath(options.directory, options.sampleSet), sequenceKey + ".png");
}

std::string NormalizeSearchQuery(const std::string& text)
{
  const std::string trimmed = TrimAscii(ToLowerAscii(text));
  std::string       normalized;
  normalized.reserve(trimmed.size());

  bool previousWasSpace = false;
  for(char character : trimmed)
  {
    const bool isSpace = std::isspace(static_cast<unsigned char>(character));
    if(isSpace)
    {
      if(!previousWasSpace)
      {
        normalized.push_back(' ');
      }
      previousWasSpace = true;
    }
    else
    {
      normalized.push_back(character);
      previousWasSpace = false;
    }
  }

  return normalized;
}

std::string RemoveUnicodePrefixes(const std::string& text)
{
  std::string withoutPrefix;
  withoutPrefix.reserve(text.size());

  for(size_t index = 0u; index < text.size();)
  {
    if(index + 1u < text.size() && text[index] == 'u' && text[index + 1u] == '+')
    {
      index += 2u;
      continue;
    }

    withoutPrefix.push_back(text[index++]);
  }

  return NormalizeSearchQuery(withoutPrefix);
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

bool ConvertUtf8ByteRangeToUtf32Range(const std::string& text,
                                       size_t             utf8StartIndex,
                                       size_t             utf8EndIndex,
                                       uint32_t&          utf32StartIndex,
                                       uint32_t&          utf32EndIndex)
{
  if(utf8StartIndex > utf8EndIndex ||
     utf8EndIndex > text.size() ||
     text.size() > std::numeric_limits<uint32_t>::max())
  {
    return false;
  }

  return Text::Utf8ToUtf32Range(Dali::StringView(text.data(), static_cast<uint32_t>(text.size())),
                                static_cast<uint32_t>(utf8StartIndex),
                                static_cast<uint32_t>(utf8EndIndex),
                                utf32StartIndex,
                                utf32EndIndex);
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
  options.sample         = LoadSampleOptions();
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
  label.SetTextOverflowMode(Text::OverflowMode::CLIP);
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

std::string MakeDetailText(const VisualCase& item, const std::string& diagnostics)
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
  stream << '\n' << diagnostics;
  return stream.str();
}

std::string MakeDetailText(Text::MultilanguageSupport& multilanguageSupport,
                           TextAbstraction::FontClient& fontClient,
                           const VisualCase&            item)
{
  return MakeDetailText(item, BuildRunDiagnosticText(multilanguageSupport, fontClient, item));
}

std::string SourcePathForOptions(const VisualOptions& options)
{
  if(!options.inputPath.empty())
  {
    return options.inputPath;
  }

  return "mode=" + options.mode + " fixture=" + options.fixturePath;
}

Property::Map CreateSoftShadowMap(float alpha, float offsetY, float blurRadius, const Vector2& sizeScale)
{
  Property::Map transform;
  transform.Add(Ui::Visual::Transform::Property::OFFSET, Vector2(0.0f, offsetY));
  transform.Add(Ui::Visual::Transform::Property::OFFSET_POLICY,
                Vector2(static_cast<float>(Ui::Visual::Transform::Policy::ABSOLUTE),
                        static_cast<float>(Ui::Visual::Transform::Policy::ABSOLUTE)));
  transform.Add(Ui::Visual::Transform::Property::SIZE, sizeScale);
  transform.Add(Ui::Visual::Transform::Property::SIZE_POLICY,
                Vector2(static_cast<float>(Ui::Visual::Transform::Policy::RELATIVE),
                        static_cast<float>(Ui::Visual::Transform::Policy::RELATIVE)));

  Property::Map shadow;
  shadow.Add(Ui::VisualBasePropertyIndex::TYPE, Ui::VisualType::COLOR);
  shadow.Add(Ui::VisualBasePropertyIndex::MIX_COLOR, Vector4(0.0f, 0.0f, 0.0f, alpha));
  shadow.Add(Ui::ColorVisualPropertyIndex::BLUR_RADIUS, blurRadius);
  shadow.Add(Ui::VisualBasePropertyIndex::TRANSFORM, transform);
  return shadow;
}

std::string CompactPath(const std::string& path, size_t maxLength = 88u)
{
  std::string compact = path;
  const char* home    = GetEnv("HOME");
  if(home && home[0] != '\0')
  {
    const std::string homePath(home);
    if(compact.compare(0u, homePath.size(), homePath) == 0)
    {
      compact = "~" + compact.substr(homePath.size());
  }
}

  if(compact.size() <= maxLength)
  {
    return compact;
  }

  const std::string prefix = compact.compare(0u, 2u, "~/") == 0 ? "~/..." : "...";
  const size_t      keep   = maxLength > prefix.size() ? maxLength - prefix.size() : 0u;
  return prefix + compact.substr(compact.size() - keep);
}

std::string DisplaySourceForOptions(const VisualOptions& options)
{
  if(!options.inputPath.empty())
  {
    return "input: " + CompactPath(options.inputPath);
  }

  return "mode=" + options.mode + " fixture: " + CompactPath(options.fixturePath);
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
  const std::string         maxItemsText = options.maxItems == 0u ? "all" : std::to_string(options.maxItems);
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
         << " max=" << HtmlEscape(maxItemsText);
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
    AbsoluteLayout       sampleContainer;
    ImageView            sampleImage;
    Label                samplePlaceholder;
    Label                sampleSetLabel;
    Label                detail;
    AbsoluteLayoutParams rowParams;
    AbsoluteLayoutParams previewParams;
    AbsoluteLayoutParams sampleContainerParams;
    AbsoluteLayoutParams sampleImageParams;
    AbsoluteLayoutParams samplePlaceholderParams;
    AbsoluteLayoutParams sampleSetLabelParams;
    AbsoluteLayoutParams detailParams;
    size_t               itemIndex{static_cast<size_t>(-1)};
  };

  void OnInit(Application application)
  {
    mWindow = application.GetWindow();
    mWindow.SetBackgroundColor(Color::WHITE);
    mWindow.KeyEventSignal().Connect(this, &EmojiVisualController::OnKeyEvent);
    mWindow.ResizedSignal().Connect(this, &EmojiVisualController::OnWindowResized);
    FocusManager::Get().SetDefaultFocusIndicatorEnabled(false);

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
    if(mIsSearchOpen)
    {
      if(keyName == "Escape" || keyName == "XF86Back")
      {
        CloseSearchInput();
        return;
      }
      if(keyName == "BackSpace")
      {
        return;
      }
    }

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
    UpdateHeaderText();
    UpdateNavigationHeight();
    UpdateLayoutBounds();
    UpdateFloatingSearchBounds();
    UpdatePreviewOverlayBounds();
  }

  void BuildUi()
  {
    mRoot = StackLayout::New();
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    mRoot.SetBackgroundColor(Color::WHITE);

    mBaseShownCount = ShownCountForOptions(mItems, mOptions);
    mShownCount     = mBaseShownCount;
    mDetailTextCache.resize(mItems.size());
    mBasicDetailTextCache.resize(mItems.size());
    mDetailTextQueued.assign(mItems.size(), false);
    BuildSearchIndex();
    mSectionAnchors = BuildCurrentSectionAnchors();

    mHeader = MakeLabel(BuildHeaderText(), HEADER_SIZE, Color::BLACK);
    mHeader.SetRequestedWidth(MATCH_PARENT);
    mHeader.SetRequestedHeight(HeaderHeightForText(BuildHeaderText()));
    mHeader.SetBackgroundColor(Vector4(0.92f, 0.94f, 0.96f, 1.0f));
    mHeader.SetPadding(Extents(12, 12, 7, 7));
    mRoot.Add(mHeader);

    BuildNavigationBar();

    mScrollView = ScrollView::New();
    mScrollView.SetScrollDirection(ScrollDirection::Vertical);
    mScrollView.SetOverScrollMode(OverScrollMode::ContentScrolls);
    mScrollView.SetBackgroundColor(Color::WHITE);
    mScrollView.SetRequestedWidth(MATCH_PARENT);
    mScrollView.SetRequestedHeight(WRAP_CONTENT);
    mScrollView.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    mRoot.Add(mScrollView);

    BuildDetailDiagnosticTimer();
    BuildPageContentPool();
    BuildPreviewOverlay();

    mWindow.Add(mRoot);
    BuildFloatingSearch();
  }

  std::string BuildHeaderText() const
  {
    std::stringstream stream;
    stream << "Source: " << DisplaySourceForOptions(mOptions)
           << " | Items: " << mBaseShownCount;
    if(IsSearchActive())
    {
      stream << " | Search: \"" << mSearchText << "\""
             << " | Matches: " << mShownCount;
    }
    if(mShownCount > 0u)
    {
      stream << " | Page: " << CurrentPageNumber() << "/" << PageCount();
    }
    if(!mOptions.signatureFilter.empty())
    {
      stream << " | Filter: " << SignatureFilterToString(mOptions);
    }
    if(mOptions.sample.enabled)
    {
      stream << "\nSample: " << mOptions.sample.displaySampleName
             << " dir=" << CompactPath(mOptions.sample.directory);
    }
    if(!mSectionAnchors.empty())
    {
      const size_t startSectionIndex = SectionIndexForItemIndex(mPageStartIndex);
      const size_t endSectionIndex   = SectionIndexForItemIndex(PageEndIndex() > 0u ? PageEndIndex() - 1u : mPageStartIndex);
      stream << "\n";
      if(startSectionIndex == endSectionIndex)
      {
        stream << "Section: " << (startSectionIndex + 1u) << "/" << mSectionAnchors.size()
               << " " << mSectionAnchors[startSectionIndex].first;
      }
      else
      {
        stream << "Sections: " << (startSectionIndex + 1u) << "-" << (endSectionIndex + 1u) << "/" << mSectionAnchors.size()
               << " " << mSectionAnchors[startSectionIndex].first
               << " -> " << mSectionAnchors[endSectionIndex].first;
      }
      stream << " | Keys: N/P page, [/ ] section, Home/End";
    }
    if(!mOptions.exportHtmlPath.empty())
    {
      stream << "\nExport HTML: " << CompactPath(mOptions.exportHtmlPath);
    }
    if(!mOptions.capturePath.empty())
    {
      stream << "\nCapture: " << CompactPath(mOptions.capturePath);
    }
    return stream.str();
  }

  float HeaderHeightForText(const std::string& text) const
  {
    const float availableWidth      = std::max(120.0f, CurrentWindowWidth() - 24.0f);
    const size_t estimatedCharsPerLine = std::max<size_t>(24u, static_cast<size_t>(availableWidth / (HEADER_SIZE * 0.62f)));
    size_t      lineCount           = 0u;
    size_t      lineLength          = 0u;

    auto addEstimatedLine = [&]() {
      lineCount += std::max<size_t>(1u, (lineLength + estimatedCharsPerLine - 1u) / estimatedCharsPerLine);
      lineLength = 0u;
    };

    for(char character : text)
    {
      if(character == '\n')
      {
        addEstimatedLine();
      }
      else
      {
        ++lineLength;
      }
    }
    addEstimatedLine();

    return std::max(HEADER_MIN_HEIGHT, HEADER_LINE_HEIGHT * static_cast<float>(lineCount) + 14.0f);
  }

  void UpdateHeaderText()
  {
    if(!mHeader)
    {
      return;
    }

    const std::string text = BuildHeaderText();
    mHeader.SetText(text.c_str());
    mHeader.SetRequestedHeight(HeaderHeightForText(text));
  }

  void BuildNavigationBar()
  {
    mNavigation = FlexLayout::New();
    mNavigation.SetRequestedWidth(MATCH_PARENT);
    mNavigation.SetDirection(FlexDirection::ROW);
    mNavigation.SetWrap(FlexWrap::WRAP);
    mNavigation.SetAlignItems(FlexAlign::FLEX_START);
    mNavigation.SetPadding(Extents(static_cast<uint16_t>(NAV_PADDING_X),
                                   static_cast<uint16_t>(NAV_PADDING_X),
                                   static_cast<uint16_t>(NAV_PADDING_Y),
                                   static_cast<uint16_t>(NAV_PADDING_Y)));
    mNavigation.SetBackgroundColor(Vector4(0.96f, 0.97f, 0.99f, 1.0f));

    PopulateNavigationBar();
    UpdateNavigationHeight();
    mRoot.Add(mNavigation);
  }

  void PopulateNavigationBar()
  {
    if(!mNavigation)
    {
      return;
    }

    mNavigation.RemoveAllChildren();
    mNavigation.Add(MakeNavButton("Prev page", [this]() { PreviousPage(); }, NAV_PAGE_BUTTON_WIDTH, Vector4(0.84f, 0.88f, 0.95f, 1.0f)));
    mNavigation.Add(MakeNavButton("Next page", [this]() { NextPage(); }, NAV_PAGE_BUTTON_WIDTH, Vector4(0.84f, 0.88f, 0.95f, 1.0f)));

    for(size_t sectionIndex = 0u; sectionIndex < mSectionAnchors.size(); ++sectionIndex)
    {
      std::stringstream label;
      label << (sectionIndex + 1u) << ". " << mSectionAnchors[sectionIndex].first;
      mNavigation.Add(MakeNavButton(label.str(), [this, sectionIndex]() { JumpToSection(sectionIndex); }, NAV_BUTTON_WIDTH, Vector4(0.90f, 0.93f, 0.98f, 1.0f)));
    }
  }

  void RebuildNavigationBar()
  {
    PopulateNavigationBar();
    UpdateNavigationHeight();
  }

  float CurrentWindowWidth() const
  {
    if(!mWindow)
    {
      return MinimumRowWidth() + PAGE_PADDING_X * 2.0f;
    }

    const auto size = mWindow.GetPositionSize();
    return static_cast<float>(size.width);
  }

  void UpdateNavigationHeight()
  {
    if(!mNavigation)
    {
      return;
    }

    const float windowWidth  = std::max(1.0f, CurrentWindowWidth());
    const float usableWidth  = std::max(1.0f, windowWidth - NAV_PADDING_X * 2.0f);
    size_t      rowCount     = 1u;
    float       currentWidth = 0.0f;

    auto addButtonWidth = [&](float width) {
      const float buttonWidth = width + NAV_BUTTON_GAP_X;
      if(currentWidth > 0.0f && currentWidth + buttonWidth > usableWidth)
      {
        ++rowCount;
        currentWidth = buttonWidth;
      }
      else
      {
        currentWidth += buttonWidth;
      }
    };

    addButtonWidth(NAV_PAGE_BUTTON_WIDTH);
    addButtonWidth(NAV_PAGE_BUTTON_WIDTH);
    for(size_t index = 0u; index < mSectionAnchors.size(); ++index)
    {
      addButtonWidth(NAV_BUTTON_WIDTH);
    }

    const float height = NAV_PADDING_Y * 2.0f +
                         static_cast<float>(rowCount) * NAV_BUTTON_HEIGHT +
                         static_cast<float>(rowCount - 1u) * NAV_BUTTON_GAP_Y;
    mNavigation.SetRequestedHeight(height);
  }

  Label MakeNavButton(const std::string& text,
                      const std::function<void()>& action,
                      float width,
                      const Vector4& backgroundColor)
  {
    Label button = Label::New();
    button.SetRequestedWidth(width);
    button.SetRequestedHeight(NAV_BUTTON_HEIGHT);
    button.SetText(text.c_str());
    button.SetFontSize(NAV_SIZE);
    button.SetTextColor(Color::BLACK);
    button.SetMultiLine(false);
    button.SetTextOverflowMode(Text::OverflowMode::CLIP);
    button.SetAsyncRendering(false);
    button.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    button.SetVerticalTextAlignment(Text::Alignment::CENTER);
    button.SetParentOrigin(ParentOrigin::TOP_LEFT);
    button.SetPivot(Pivot::TOP_LEFT);
    button.SetBackgroundColor(backgroundColor);
    button.SetMargin(Extents(0,
                             static_cast<uint16_t>(NAV_BUTTON_GAP_X),
                             0,
                             static_cast<uint16_t>(NAV_BUTTON_GAP_Y)));
    button.SetPadding(Extents(4, 4, 0, 0));
    button.TouchEventSignal().Connect(this, [action, backgroundColor](Actor actor, const TouchEvent& touch) {
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

  std::string BuildSearchableText(const VisualCase& item) const
  {
    std::stringstream stream;
    stream << CodepointsToUtf8(item.codepoints) << ' '
           << CodepointsToString(item.codepoints) << ' '
           << CodepointsToString(item.codepoints, true) << ' '
           << SectionForCase(item) << ' '
           << item.signature << ' '
           << item.status << ' '
           << item.name;
    if(!item.reason.empty())
    {
      stream << ' ' << item.reason;
    }
    return NormalizeSearchQuery(stream.str());
  }

  void BuildSearchIndex()
  {
    mSearchIndex.clear();
    mSearchIndex.reserve(mItems.size());
    for(const VisualCase& item : mItems)
    {
      mSearchIndex.push_back(BuildSearchableText(item));
    }
  }

  bool IsSearchActive() const
  {
    return !mSearchQuery.empty();
  }

  size_t ItemIndexForVisibleIndex(size_t visibleIndex) const
  {
    if(IsSearchActive())
    {
      return visibleIndex < mSearchMatches.size() ? mSearchMatches[visibleIndex] : static_cast<size_t>(-1);
    }

    return visibleIndex;
  }

  std::vector<std::pair<std::string, size_t>> BuildCurrentSectionAnchors() const
  {
    std::set<std::string>                         seenSections;
    std::vector<std::pair<std::string, size_t>>   sections;

    for(size_t visibleIndex = 0u; visibleIndex < mShownCount; ++visibleIndex)
    {
      const size_t itemIndex = ItemIndexForVisibleIndex(visibleIndex);
      if(itemIndex >= mItems.size())
      {
        continue;
      }

      const std::string section = SectionForCase(mItems[itemIndex]);
      if(seenSections.insert(section).second)
      {
        sections.push_back(std::make_pair(section, visibleIndex));
      }
    }

    return sections;
  }

  std::vector<std::string> HighlightNeedlesForCurrentSearch() const
  {
    std::vector<std::string> needles;
    if(mSearchQuery.empty())
    {
      return needles;
    }

    needles.push_back(mSearchQuery);
    const std::string withoutUnicodePrefixes = RemoveUnicodePrefixes(mSearchQuery);
    if(!withoutUnicodePrefixes.empty() && withoutUnicodePrefixes != mSearchQuery)
    {
      needles.push_back(withoutUnicodePrefixes);
    }
    return needles;
  }

  Text::StyledText BuildHighlightedStyledText(const std::string& text) const
  {
    Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(text.c_str());

    const std::vector<std::string> needles = HighlightNeedlesForCurrentSearch();
    if(needles.empty())
    {
      return builder.Build();
    }

    const std::string lowerText = ToLowerAscii(text);
    size_t            cursor    = 0u;
    while(cursor < text.size())
    {
      size_t matchBegin = std::string::npos;
      size_t matchSize  = 0u;
      for(const std::string& needle : needles)
      {
        if(needle.empty())
        {
          continue;
        }

        const size_t found = lowerText.find(needle, cursor);
        if(found != std::string::npos &&
           (matchBegin == std::string::npos || found < matchBegin || (found == matchBegin && needle.size() > matchSize)))
        {
          matchBegin = found;
          matchSize  = needle.size();
        }
      }

      if(matchBegin == std::string::npos)
      {
        break;
      }

      uint32_t startIndex = 0u;
      uint32_t endIndex   = 0u;
      if(ConvertUtf8ByteRangeToUtf32Range(text, matchBegin, matchBegin + matchSize, startIndex, endIndex) &&
         startIndex < endIndex)
      {
        builder.SetSpan(Text::ForegroundColorSpan::New(UiColor(0xB42318)), startIndex, endIndex);
      }

      cursor = matchBegin + matchSize;
    }

    return builder.Build();
  }

  Text::StyledText VisibleDetailStyledTextForIndex(size_t index)
  {
    return BuildHighlightedStyledText(VisibleDetailTextForIndex(index));
  }

  void SetRowDetailText(RowActors& row, size_t index)
  {
    const bool searchActive = IsSearchActive();
    if(searchActive)
    {
      row.detail.SetStyledText(VisibleDetailStyledTextForIndex(index));
    }
    else
    {
      row.detail.SetText(VisibleDetailTextForIndex(index).c_str());
    }
  }

  void ApplySearchText(const std::string& text)
  {
    const std::string normalized = NormalizeSearchQuery(text);
    if(normalized == mSearchQuery && text == mSearchText)
    {
      return;
    }

    mSearchText  = text;
    mSearchQuery = normalized;
    mSearchMatches.clear();

    if(IsSearchActive())
    {
      for(size_t index = 0u; index < mBaseShownCount && index < mSearchIndex.size(); ++index)
      {
        if(mSearchIndex[index].find(mSearchQuery) != std::string::npos)
        {
          mSearchMatches.push_back(index);
        }
      }
      mShownCount = mSearchMatches.size();
    }
    else
    {
      mShownCount = mBaseShownCount;
    }

    mPageStartIndex = 0u;
    mSectionAnchors = BuildCurrentSectionAnchors();
    RebuildNavigationBar();
    UpdatePageContent();
  }

  void OnSearchTextChanged(View view)
  {
    InputField input = InputField::DownCast(view);
    if(input)
    {
      ApplySearchText(input.GetText().CStr());
    }
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

  float SampleImageSize() const
  {
    const float contentHeight = RowContentHeight();
    const float maxSize       = std::max(1.0f, std::min(PREVIEW_WIDTH, contentHeight - SAMPLE_SET_LABEL_HEIGHT));
    return std::max(1.0f, std::min(mOptions.fontSize * SAMPLE_IMAGE_SCALE, maxSize));
  }

  float SampleBlockWidth() const
  {
    return mOptions.sample.enabled ? SAMPLE_PREVIEW_GAP + PREVIEW_WIDTH : 0.0f;
  }

  float PreviewBlockWidth() const
  {
    return PREVIEW_WIDTH + SampleBlockWidth();
  }

  float MinimumRowWidth() const
  {
    return ROW_PADDING_X * 2.0f + PreviewBlockWidth() + MIN_DETAIL_WIDTH;
  }

  float PageContentWidth() const
  {
    float windowWidth = 0.0f;
    if(mWindow)
    {
      const auto size = mWindow.GetPositionSize();
      windowWidth     = static_cast<float>(size.width);
    }
    return std::max(MinimumRowWidth() + PAGE_PADDING_X * 2.0f, windowWidth);
  }

  float RowWidth() const
  {
    return std::max(MinimumRowWidth(), PageContentWidth() - PAGE_PADDING_X * 2.0f);
  }

  float DetailWidth(float rowWidth) const
  {
    return std::max(MIN_DETAIL_WIDTH, rowWidth - ROW_PADDING_X * 2.0f - PreviewBlockWidth());
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

  template<typename ViewType>
  static void SetLayoutBounds(ViewType& view, AbsoluteLayoutParams& params, const LayoutRect& bounds)
  {
    params.SetBounds(bounds);
    view.SetLayoutParams(params);
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
      if(mEmptyLabel)
      {
        mEmptyLabel.SetRequestedWidth(rowWidth);
        SetLayoutBounds(mEmptyLabel, mEmptyLabelParams, LayoutRect(PAGE_PADDING_X, PAGE_PADDING_Y, rowWidth, ROW_HEIGHT));
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
    SetLayoutBounds(row.row, row.rowParams, LayoutRect(PAGE_PADDING_X, RowY(slot), rowWidth, ROW_HEIGHT));

    row.preview.SetRequestedWidth(PREVIEW_WIDTH);
    row.preview.SetRequestedHeight(contentHeight);
    SetLayoutBounds(row.preview, row.previewParams, LayoutRect(ROW_PADDING_X, ROW_PADDING_Y, PREVIEW_WIDTH, contentHeight));

    if(row.sampleContainer)
    {
      const float sampleX         = ROW_PADDING_X + PREVIEW_WIDTH + SAMPLE_PREVIEW_GAP;
      const float sampleImageSize = SampleImageSize();
      const float sampleImageX    = (PREVIEW_WIDTH - sampleImageSize) * 0.5f;
      const float sampleLabelY    = std::max(0.0f, contentHeight - SAMPLE_SET_LABEL_HEIGHT);
      const float sampleImageY    = std::max(0.0f, std::min((contentHeight - sampleImageSize) * 0.5f,
                                                            sampleLabelY - sampleImageSize));
      row.sampleContainer.SetRequestedWidth(PREVIEW_WIDTH);
      row.sampleContainer.SetRequestedHeight(contentHeight);
      SetLayoutBounds(row.sampleContainer, row.sampleContainerParams, LayoutRect(sampleX, ROW_PADDING_Y, PREVIEW_WIDTH, contentHeight));

      row.sampleImage.SetRequestedWidth(sampleImageSize);
      row.sampleImage.SetRequestedHeight(sampleImageSize);
      SetLayoutBounds(row.sampleImage, row.sampleImageParams, LayoutRect(sampleImageX, sampleImageY, sampleImageSize, sampleImageSize));

      row.samplePlaceholder.SetRequestedWidth(PREVIEW_WIDTH);
      row.samplePlaceholder.SetRequestedHeight(contentHeight);
      SetLayoutBounds(row.samplePlaceholder, row.samplePlaceholderParams, LayoutRect(0.0f, 0.0f, PREVIEW_WIDTH, contentHeight));

      row.sampleSetLabel.SetRequestedWidth(PREVIEW_WIDTH);
      row.sampleSetLabel.SetRequestedHeight(SAMPLE_SET_LABEL_HEIGHT);
      SetLayoutBounds(row.sampleSetLabel,
                      row.sampleSetLabelParams,
                      LayoutRect(0.0f,
                                 std::max(0.0f, contentHeight - SAMPLE_SET_LABEL_HEIGHT),
                                 PREVIEW_WIDTH,
                                 SAMPLE_SET_LABEL_HEIGHT));
    }

    row.detail.SetRequestedWidth(detailWidth);
    row.detail.SetRequestedHeight(contentHeight);
    SetLayoutBounds(row.detail, row.detailParams, LayoutRect(ROW_PADDING_X + PreviewBlockWidth(), ROW_PADDING_Y, detailWidth, contentHeight));
  }

  void BuildPageContentPool()
  {
    mPageContent = AbsoluteLayout::New();
    mPageContent.SetRequestedWidth(PageContentWidth());
    mPageContent.SetRequestedHeight(PageContentHeight(mShownCount == 0u ? 1u : std::min(PageSize(), mShownCount)));

    mEmptyLabel = MakeLabel(EmptyMessageText(), DETAIL_SIZE, Color::BLACK);
    mEmptyLabel.SetRequestedWidth(RowWidth());
    mEmptyLabel.SetRequestedHeight(ROW_HEIGHT);
    mEmptyLabel.SetBackgroundColor(Vector4(0.98f, 0.88f, 0.88f, 1.0f));
    mEmptyLabel.SetPadding(Extents(16, 16, 10, 10));
    mEmptyLabel.SetProperty(Actor::Property::VISIBLE, mShownCount == 0u);
    mEmptyLabelParams = AbsoluteLayoutParams::New();
    mEmptyLabel.SetLayoutParams(mEmptyLabelParams);
    mPageContent.Add(mEmptyLabel);

    if(mBaseShownCount > 0u)
    {
      const size_t rowCount = std::min(PageSize(), mBaseShownCount);
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

  std::string EmptyMessageText() const
  {
    if(!mLoadError.empty())
    {
      return mLoadError;
    }
    if(IsSearchActive())
    {
      return "No matches for \"" + mSearchText + "\".";
    }
    return "No emoji cases selected.";
  }

  void BuildFloatingSearch()
  {
    mSearchOverlay = AbsoluteLayout::New();
    mSearchOverlay.SetLayoutMode(LayoutMode::STANDALONE);
    mSearchOverlay.SetRequestedX(0.0f);
    mSearchOverlay.SetRequestedY(0.0f);
    mSearchOverlay.SetBackgroundColor(Color::TRANSPARENT);

    mSearchDismissLayer = View::New();
    mSearchDismissLayer.SetLayoutMode(LayoutMode::STANDALONE);
    mSearchDismissLayer.SetBackgroundColor(Color::TRANSPARENT);
    mSearchDismissLayer.SetProperty(Actor::Property::VISIBLE, false);
    mSearchDismissLayer.TouchEventSignal().Connect(this, &EmojiVisualController::OnSearchDismissTouched);
    mSearchOverlay.Add(mSearchDismissLayer);

    mSearchInput = InputField::New();
    mSearchInput.SetLayoutMode(LayoutMode::STANDALONE);
    mSearchInput.SetFontSize(FLOATING_SEARCH_FONT_SIZE);
    mSearchInput.SetTextColor(Color::BLACK);
    mSearchInput.SetBackgroundColor(Color::WHITE);
    mSearchInput.SetPlaceholder("Search Unicode or Emoji");
    mSearchInput.SetPlaceholderColor(Vector4(0.45f, 0.48f, 0.55f, 1.0f));
    mSearchInput.SetShowPlaceholderOnFocus(true);
    mSearchInput.SetCursorWidth(2);
    mSearchInput.SetCursorColor(Color::BLACK);
    mSearchInput.SetPadding(Extents(14, 14, 8, 8));
    mSearchInput.SetHorizontalTextAlignment(Text::Alignment::START);
    mSearchInput.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mSearchInput.SetTextOverflowMode(Text::OverflowMode::CLIP);
    mSearchInput.SetCornerRadiusPolicyRelative();
    mSearchInput.SetCornerRadius(0.5f);
    mSearchInput.SetProperty(View::Property::SHADOW, CreateSoftShadowMap(0.14f, 3.0f, 5.0f, Vector2(1.02f, 1.08f)));
    mSearchInput.SetFocusable(true);
    mSearchInput.SetFocusOnTouchEnabled(true);
    mSearchInput.SetProperty(Actor::Property::VISIBLE, false);
    mSearchInput.SetProperty(Actor::Property::SENSITIVE, false);
    mSearchInput.SetProperty(Actor::Property::OPACITY, 0.0f);
    mSearchInput.TouchEventSignal().Connect(this, &EmojiVisualController::OnSearchInputTouched);
    mSearchInput.TextChangedSignal().Connect(this, &EmojiVisualController::OnSearchTextChanged);
    mSearchOverlay.Add(mSearchInput);

    mSearchButton = View::New();
    mSearchButton.SetLayoutMode(LayoutMode::STANDALONE);
    mSearchButton.SetBackgroundColor(Color::WHITE);
    mSearchButton.SetCornerRadiusPolicyRelative();
    mSearchButton.SetCornerRadius(0.5f);
    mSearchButton.SetProperty(View::Property::SHADOW, CreateSoftShadowMap(0.18f, 3.0f, 4.0f, Vector2(1.08f, 1.08f)));
    mSearchButton.TouchEventSignal().Connect(this, &EmojiVisualController::OnSearchButtonTouched);

    mSearchIcon = Label::New(SEARCH_BUTTON_EMOJI);
    mSearchIcon.SetLayoutMode(LayoutMode::STANDALONE);
    mSearchIcon.SetBackgroundColor(Color::TRANSPARENT);
    mSearchIcon.SetTextColor(Color::WHITE);
    mSearchIcon.SetFontSize(FLOATING_SEARCH_ICON_SIZE);
    mSearchIcon.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mSearchIcon.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mSearchIcon.SetTextOverflowMode(Text::OverflowMode::CLIP);
    mSearchIcon.SetRequestedX(0.0f);
    mSearchIcon.SetRequestedY(-1.0f);
    mSearchIcon.SetRequestedWidth(FLOATING_SEARCH_BUTTON_SIZE);
    mSearchIcon.SetRequestedHeight(FLOATING_SEARCH_BUTTON_SIZE);
    mSearchIcon.TouchEventSignal().Connect(this, &EmojiVisualController::OnSearchButtonTouched);
    mSearchButton.Add(mSearchIcon);

    mSearchOverlay.Add(mSearchButton);

    mWindow.Add(mSearchOverlay);
    UpdateFloatingSearchBounds();
  }

  float FloatingSearchInputWidth() const
  {
    const float windowWidth = std::max(1.0f, CurrentWindowWidth());
    const float available   = std::max(FLOATING_SEARCH_BUTTON_SIZE, windowWidth - FLOATING_SEARCH_MARGIN * 2.0f);
    return std::min(FLOATING_SEARCH_INPUT_MAX_WIDTH, std::max(FLOATING_SEARCH_INPUT_MIN_WIDTH, available));
  }

  FloatingSearchBounds CurrentFloatingSearchBounds() const
  {
    const auto  size   = mWindow.GetPositionSize();
    const float width  = std::max(1.0f, static_cast<float>(size.width));
    const float height = std::max(1.0f, static_cast<float>(size.height));
    const float right  = std::max(FLOATING_SEARCH_MARGIN + FLOATING_SEARCH_BUTTON_SIZE, width - FLOATING_SEARCH_MARGIN);
    const float bottom = std::max(FLOATING_SEARCH_MARGIN + FLOATING_SEARCH_BUTTON_SIZE, height - FLOATING_SEARCH_MARGIN);

    FloatingSearchBounds bounds;
    bounds.buttonX    = right - FLOATING_SEARCH_BUTTON_SIZE;
    bounds.buttonY    = bottom - FLOATING_SEARCH_BUTTON_SIZE;
    bounds.buttonSize = FLOATING_SEARCH_BUTTON_SIZE;
    bounds.inputWidth = std::max(FLOATING_SEARCH_BUTTON_SIZE, std::min(FloatingSearchInputWidth(), right - FLOATING_SEARCH_MARGIN));
    bounds.inputHeight = FLOATING_SEARCH_INPUT_HEIGHT;
    bounds.inputX      = right - bounds.inputWidth;
    bounds.inputY      = bottom - bounds.inputHeight;
    return bounds;
  }

  void SetSearchActorBounds(View view, float x, float y, float width, float height)
  {
    if(!view)
    {
      return;
    }

    view.SetRequestedX(x);
    view.SetRequestedY(y);
    view.SetRequestedWidth(width);
    view.SetRequestedHeight(height);
    Dali::Ui::Extension::View::SetPositionX(view, x);
    Dali::Ui::Extension::View::SetPositionY(view, y);
    Dali::Ui::Extension::View::SetSizeWidth(view, width);
    Dali::Ui::Extension::View::SetSizeHeight(view, height);
  }

  void UpdateFloatingSearchBounds()
  {
    if(!mSearchOverlay)
    {
      return;
    }

    const auto  size   = mWindow.GetPositionSize();
    const float width  = std::max(1.0f, static_cast<float>(size.width));
    const float height = std::max(1.0f, static_cast<float>(size.height));
    const FloatingSearchBounds bounds = CurrentFloatingSearchBounds();

    mSearchOverlay.SetRequestedWidth(width);
    mSearchOverlay.SetRequestedHeight(height);
    SetSearchActorBounds(mSearchDismissLayer, 0.0f, 0.0f, width, height);

    SetSearchActorBounds(mSearchButton, bounds.buttonX, bounds.buttonY, bounds.buttonSize, bounds.buttonSize);
    SetSearchActorBounds(mSearchInput, bounds.inputX, bounds.inputY, bounds.inputWidth, bounds.inputHeight);

    mSearchOverlay.RaiseToTop();
  }

  bool OnSearchButtonTouched(Actor /*actor*/, TouchEvent touch)
  {
    const PointState::Type state = touch.GetState(0u);
    if(state == PointState::UP || state == PointState::FINISHED)
    {
      OpenSearchInput();
    }
    return true;
  }

  bool OnSearchDismissTouched(Actor /*actor*/, TouchEvent touch)
  {
    const PointState::Type state = touch.GetState(0u);
    if(state == PointState::UP || state == PointState::FINISHED)
    {
      CloseSearchInput();
    }
    return true;
  }

  bool OnSearchInputTouched(Actor /*actor*/, TouchEvent touch)
  {
    const PointState::Type state = touch.GetState(0u);
    if(state == PointState::UP || state == PointState::FINISHED)
    {
      if(mIsSearchOpen && mSearchInput)
      {
        FocusManager::Get().RequestFocus(mSearchInput);
      }
    }
    return true;
  }

  void OpenSearchInput()
  {
    if(mIsSearchOpen)
    {
      FocusManager::Get().RequestFocus(mSearchInput);
      return;
    }

    StopSearchAnimation();

    const FloatingSearchBounds bounds = CurrentFloatingSearchBounds();
    mIsSearchOpen                    = true;
    mSearchDismissLayer.SetProperty(Actor::Property::VISIBLE, true);
    mSearchButton.SetProperty(Actor::Property::VISIBLE, true);
    mSearchButton.SetProperty(Actor::Property::SENSITIVE, false);
    mSearchButton.SetProperty(Actor::Property::OPACITY, 1.0f);
    mSearchInput.SetProperty(Actor::Property::VISIBLE, true);
    mSearchInput.SetProperty(Actor::Property::SENSITIVE, true);
    mSearchInput.SetProperty(Actor::Property::OPACITY, 0.0f);
    SetSearchActorBounds(mSearchButton, bounds.buttonX, bounds.buttonY, bounds.buttonSize, bounds.buttonSize);
    SetSearchActorBounds(mSearchInput, bounds.buttonX, bounds.buttonY, bounds.buttonSize, bounds.buttonSize);
    mSearchOverlay.RaiseToTop();

    mSearchAnimation = Animation::New(FLOATING_SEARCH_ANIMATION_TIME);
    mSearchAnimation.AnimateTo(Property(mSearchInput, Actor::Property::POSITION_X), bounds.inputX, AlphaFunction::EASE_OUT);
    mSearchAnimation.AnimateTo(Property(mSearchInput, Actor::Property::POSITION_Y), bounds.inputY, AlphaFunction::EASE_OUT);
    mSearchAnimation.AnimateTo(Property(mSearchInput, Actor::Property::SIZE_WIDTH), bounds.inputWidth, AlphaFunction::EASE_OUT);
    mSearchAnimation.AnimateTo(Property(mSearchInput, Actor::Property::SIZE_HEIGHT), bounds.inputHeight, AlphaFunction::EASE_OUT);
    mSearchAnimation.AnimateTo(Property(mSearchInput, Actor::Property::OPACITY), 1.0f, AlphaFunction::EASE_IN);
    mSearchAnimation.AnimateTo(Property(mSearchButton, Actor::Property::OPACITY), 0.0f, AlphaFunction::EASE_OUT);
    mSearchAnimation.FinishedSignal().Connect(this, &EmojiVisualController::OnSearchAnimationFinished);
    mSearchAnimation.Play();

    FocusManager::Get().RequestFocus(mSearchInput);
  }

  void CloseSearchInput()
  {
    if(!mIsSearchOpen)
    {
      return;
    }

    StopSearchAnimation();

    const FloatingSearchBounds bounds = CurrentFloatingSearchBounds();
    mIsSearchOpen = false;
    FocusManager::Get().ClearFocus();
    mSearchButton.SetProperty(Actor::Property::VISIBLE, true);
    mSearchButton.SetProperty(Actor::Property::SENSITIVE, true);
    mSearchButton.SetProperty(Actor::Property::OPACITY, 0.0f);
    mSearchInput.SetProperty(Actor::Property::SENSITIVE, false);
    mSearchDismissLayer.SetProperty(Actor::Property::VISIBLE, false);
    SetSearchActorBounds(mSearchButton, bounds.buttonX, bounds.buttonY, bounds.buttonSize, bounds.buttonSize);
    SetSearchActorBounds(mSearchInput, bounds.inputX, bounds.inputY, bounds.inputWidth, bounds.inputHeight);
    mSearchInput.SetProperty(Actor::Property::VISIBLE, true);
    mSearchInput.SetProperty(Actor::Property::OPACITY, 1.0f);
    mSearchOverlay.RaiseToTop();

    mSearchAnimation = Animation::New(FLOATING_SEARCH_ANIMATION_TIME * 0.85f);
    mSearchAnimation.AnimateTo(Property(mSearchInput, Actor::Property::POSITION_X), bounds.buttonX, AlphaFunction::EASE_OUT);
    mSearchAnimation.AnimateTo(Property(mSearchInput, Actor::Property::POSITION_Y), bounds.buttonY, AlphaFunction::EASE_OUT);
    mSearchAnimation.AnimateTo(Property(mSearchInput, Actor::Property::SIZE_WIDTH), bounds.buttonSize, AlphaFunction::EASE_OUT);
    mSearchAnimation.AnimateTo(Property(mSearchInput, Actor::Property::SIZE_HEIGHT), bounds.buttonSize, AlphaFunction::EASE_OUT);
    mSearchAnimation.AnimateTo(Property(mSearchInput, Actor::Property::OPACITY), 0.0f, AlphaFunction::EASE_OUT);
    mSearchAnimation.AnimateTo(Property(mSearchButton, Actor::Property::OPACITY), 1.0f, AlphaFunction::EASE_IN);
    mSearchAnimation.FinishedSignal().Connect(this, &EmojiVisualController::OnSearchAnimationFinished);
    mSearchAnimation.Play();
  }

  void StopSearchAnimation()
  {
    if(mSearchAnimation)
    {
      mSearchAnimation.Stop();
      mSearchAnimation.Reset();
    }
  }

  void OnSearchAnimationFinished(Animation /*animation*/)
  {
    const FloatingSearchBounds bounds = CurrentFloatingSearchBounds();
    mSearchAnimation.Reset();

    if(mIsSearchOpen)
    {
      SetSearchActorBounds(mSearchInput, bounds.inputX, bounds.inputY, bounds.inputWidth, bounds.inputHeight);
      SetSearchActorBounds(mSearchButton, bounds.buttonX, bounds.buttonY, bounds.buttonSize, bounds.buttonSize);
      mSearchInput.SetProperty(Actor::Property::VISIBLE, true);
      mSearchInput.SetProperty(Actor::Property::SENSITIVE, true);
      mSearchInput.SetProperty(Actor::Property::OPACITY, 1.0f);
      mSearchButton.SetProperty(Actor::Property::VISIBLE, false);
      mSearchButton.SetProperty(Actor::Property::SENSITIVE, false);
      mSearchButton.SetProperty(Actor::Property::OPACITY, 0.0f);
      FocusManager::Get().RequestFocus(mSearchInput);
      return;
    }

    SetSearchActorBounds(mSearchInput, bounds.inputX, bounds.inputY, bounds.inputWidth, bounds.inputHeight);
    SetSearchActorBounds(mSearchButton, bounds.buttonX, bounds.buttonY, bounds.buttonSize, bounds.buttonSize);
    mSearchInput.SetProperty(Actor::Property::VISIBLE, false);
    mSearchInput.SetProperty(Actor::Property::SENSITIVE, false);
    mSearchInput.SetProperty(Actor::Property::OPACITY, 0.0f);
    mSearchButton.SetProperty(Actor::Property::VISIBLE, true);
    mSearchButton.SetProperty(Actor::Property::SENSITIVE, true);
    mSearchButton.SetProperty(Actor::Property::OPACITY, 1.0f);
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
    mPreviewOverlay.SetRequestedX(0.0f);
    mPreviewOverlay.SetRequestedY(0.0f);
    mPreviewOverlay.SetBackgroundColor(Vector4(0.0f, 0.0f, 0.0f, 0.70f));
    mPreviewOverlay.SetProperty(Actor::Property::VISIBLE, false);
    mPreviewOverlay.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
    mPreviewOverlay.TouchEventSignal().Connect(this, &EmojiVisualController::OnPreviewOverlayTouched);

    mLargePreview = MakeLabel(std::string(), PREVIEW_POPUP_TEXT_FIT_MAX_SIZE, Color::BLACK);
    mLargePreview.SetMultiLine(false);
    mLargePreview.SetTextFit(Text::Fit::Range(PREVIEW_POPUP_TEXT_FIT_MIN_SIZE,
                                            PREVIEW_POPUP_TEXT_FIT_MAX_SIZE,
                                            PREVIEW_POPUP_TEXT_FIT_STEP_SIZE));
    mLargePreview.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mLargePreview.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mLargePreview.SetBackgroundColor(Color::WHITE);
    mLargePreview.SetProperty(Actor::Property::DRAW_MODE, DrawMode::OVERLAY_2D);
    mLargePreview.TouchEventSignal().Connect(this, &EmojiVisualController::OnPreviewOverlayTouched);
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

    const auto  size   = mWindow.GetPositionSize();
    const float width  = static_cast<float>(size.width);
    const float height = static_cast<float>(size.height);
    const float inset  = std::min(PREVIEW_POPUP_MARGIN, std::min(width, height) * 0.10f);

    mPreviewOverlay.SetRequestedWidth(width);
    mPreviewOverlay.SetRequestedHeight(height);

    if(mLargePreview)
    {
      const float previewWidth  = std::max(1.0f, std::min(width - inset * 2.0f, PREVIEW_POPUP_MAX_WIDTH));
      const float previewHeight = std::max(1.0f, std::min(height - inset * 2.0f, PREVIEW_POPUP_MAX_HEIGHT));
      const float previewX      = (width - previewWidth) * 0.5f;
      const float previewY      = (height - previewHeight) * 0.5f;
      mLargePreview.SetRequestedWidth(previewWidth);
      mLargePreview.SetRequestedHeight(previewHeight);
      SetLayoutBounds(mLargePreview, mLargePreviewParams, LayoutRect(previewX, previewY, previewWidth, previewHeight));
    }

    if(mIsPreviewOverlayVisible)
    {
      mPreviewOverlay.RaiseToTop();
    }
  }

  void UpdatePageContent()
  {
    ResetPendingDetailDiagnostics();
    UpdateLayoutBounds();

    if(mEmptyLabel)
    {
      mEmptyLabel.SetText(EmptyMessageText().c_str());
      mEmptyLabel.SetProperty(Actor::Property::VISIBLE, mShownCount == 0u);
    }

    const size_t pageEnd = PageEndIndex();
    for(size_t slot = 0u; slot < mRowPool.size(); ++slot)
    {
      const size_t visibleIndex = mPageStartIndex + slot;
      RowActors& row = mRowPool[slot];
      if(visibleIndex < pageEnd)
      {
        const size_t itemIndex = ItemIndexForVisibleIndex(visibleIndex);
        if(itemIndex < mItems.size())
        {
          UpdateRow(row, mItems[itemIndex], itemIndex, visibleIndex);
        }
        else
        {
          HideRow(row);
        }
      }
      else
      {
        HideRow(row);
      }
    }

    mScrollView.SetScrollPosition(Vector2::ZERO);
    UpdateHeaderText();
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

    const auto size = mWindow.GetPositionSize();
    mCapture = Capture::New();
    mCapture.FinishedSignal().Connect(this, &EmojiVisualController::OnCaptureFinished);
    mCapture.Start(mRoot,
                   Vector2(static_cast<float>(size.width), static_cast<float>(size.height)),
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
      mBasicDetailTextCache[index] = ::MakeDetailText(mItems[index], "run diagnostics: pending");
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

  std::string ClipboardTextForIndex(size_t index)
  {
    if(index >= mItems.size())
    {
      return std::string();
    }

    if(mDetailTextCache[index].empty())
    {
      mDetailTextCache[index] = ::MakeDetailText(mMultilanguageSupport, mFontClient, mItems[index]);
      if(index < mDetailTextQueued.size())
      {
        mDetailTextQueued[index] = false;
      }
      UpdateVisibleDetailText(index);
    }

    std::stringstream stream;
    stream << "emoji: " << CodepointsToUtf8(mItems[index].codepoints) << '\n'
           << mDetailTextCache[index];
    return stream.str();
  }

  bool CopyItemToClipboard(size_t index)
  {
    const std::string source = ClipboardTextForIndex(index);
    if(source.empty())
    {
      return false;
    }

    Clipboard clipboard = Clipboard::Get();
    if(!clipboard)
    {
      std::cerr << "emoji visual warning: clipboard is not available\n";
      return false;
    }

    Dali::ClipboardData data(MIME_TYPE_TEXT_PLAIN, source.c_str());
    if(!clipboard.SetData(data))
    {
      std::cerr << "emoji visual warning: failed to copy emoji case to clipboard\n";
      return false;
    }
    return true;
  }

  void UpdateVisibleDetailText(size_t itemIndex)
  {
    for(RowActors& row : mRowPool)
    {
      if(row.itemIndex == itemIndex)
      {
        SetRowDetailText(row, itemIndex);
        return;
      }
    }
  }

  RowActors MakeRowSlot()
  {
    RowActors actors;

    actors.row = AbsoluteLayout::New();
    actors.row.SetRequestedWidth(RowWidth());
    actors.row.SetRequestedHeight(ROW_HEIGHT);
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
    actors.preview.TouchEventSignal().Connect(this, &EmojiVisualController::OnPreviewTouched);
    actors.row.Add(actors.preview);

    if(mOptions.sample.enabled)
    {
      actors.sampleContainer = AbsoluteLayout::New();
      actors.sampleContainer.SetRequestedWidth(PREVIEW_WIDTH);
      actors.sampleContainer.SetRequestedHeight(RowContentHeight());
      actors.sampleContainer.SetBackgroundColor(Color::WHITE);
      actors.sampleContainerParams = AbsoluteLayoutParams::New();
      actors.sampleContainer.SetLayoutParams(actors.sampleContainerParams);

      actors.sampleImage = ImageView::New();
      actors.sampleImage.SetRequestedWidth(SampleImageSize());
      actors.sampleImage.SetRequestedHeight(SampleImageSize());
      actors.sampleImage.SetFittingMode(Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
      actors.sampleImage.SetBackgroundColor(Color::WHITE);
      actors.sampleImage.SetProperty(Actor::Property::VISIBLE, false);
      actors.sampleImageParams = AbsoluteLayoutParams::New();
      actors.sampleImage.SetLayoutParams(actors.sampleImageParams);
      actors.sampleContainer.Add(actors.sampleImage);

      actors.samplePlaceholder = MakeLabel("No sample", SAMPLE_PLACEHOLDER_SIZE, Vector4(0.42f, 0.45f, 0.50f, 1.0f));
      actors.samplePlaceholder.SetHorizontalTextAlignment(Text::Alignment::CENTER);
      actors.samplePlaceholder.SetVerticalTextAlignment(Text::Alignment::CENTER);
      actors.samplePlaceholder.SetBackgroundColor(Vector4(0.94f, 0.95f, 0.97f, 1.0f));
      actors.samplePlaceholderParams = AbsoluteLayoutParams::New();
      actors.samplePlaceholder.SetLayoutParams(actors.samplePlaceholderParams);
      actors.sampleContainer.Add(actors.samplePlaceholder);

      actors.sampleSetLabel = MakeLabel(mOptions.sample.displaySampleName, SAMPLE_SET_LABEL_SIZE, Color::WHITE);
      actors.sampleSetLabel.SetMultiLine(false);
      actors.sampleSetLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
      actors.sampleSetLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
      actors.sampleSetLabel.SetBackgroundColor(Vector4(0.0f, 0.0f, 0.0f, 0.62f));
      actors.sampleSetLabel.SetPadding(Extents(4, 4, 0, 0));
      actors.sampleSetLabelParams = AbsoluteLayoutParams::New();
      actors.sampleSetLabel.SetLayoutParams(actors.sampleSetLabelParams);
      actors.sampleContainer.Add(actors.sampleSetLabel);

      actors.row.Add(actors.sampleContainer);
    }

    actors.detail = MakeLabel(std::string(), DETAIL_SIZE, Color::BLACK);
    actors.detail.SetFontFamily("Ubuntu Mono");
    actors.detail.SetRequestedWidth(DetailWidth(RowWidth()));
    actors.detail.SetRequestedHeight(RowContentHeight());
    actors.detail.SetPadding(Extents(14, 8, 0, 0));
    actors.detailParams = AbsoluteLayoutParams::New();
    actors.detail.SetLayoutParams(actors.detailParams);
    actors.detail.TouchEventSignal().Connect(this, &EmojiVisualController::OnDetailTouched);
    actors.row.Add(actors.detail);

    return actors;
  }

  void UpdateSamplePreview(RowActors& row, const VisualCase& item)
  {
    if(!mOptions.sample.enabled || !row.sampleContainer)
    {
      return;
    }

    row.sampleSetLabel.SetText(mOptions.sample.displaySampleName.c_str());

    const std::string samplePath = SampleImagePathForCase(mOptions.sample, item);
    if(!samplePath.empty() && FileExists(samplePath))
    {
      row.sampleImage.SetResourceUrl(samplePath.c_str());
      row.sampleImage.SetProperty(Actor::Property::VISIBLE, true);
      row.samplePlaceholder.SetProperty(Actor::Property::VISIBLE, false);
      return;
    }

    row.sampleImage.SetProperty(Actor::Property::VISIBLE, false);
    row.samplePlaceholder.SetText("No sample");
    row.samplePlaceholder.SetProperty(Actor::Property::VISIBLE, true);
  }

  void UpdateRow(RowActors& row, const VisualCase& item, size_t index, size_t visibleIndex)
  {
    row.itemIndex = index;
    row.row.SetProperty(Actor::Property::VISIBLE, true);
    row.row.SetBackgroundColor(visibleIndex % 2u == 0u ? Vector4(0.98f, 0.98f, 0.98f, 1.0f) : Vector4(0.94f, 0.96f, 0.98f, 1.0f));
    row.preview.SetText(CodepointsToUtf8(item.codepoints).c_str());
    UpdateSamplePreview(row, item);
    SetRowDetailText(row, index);
  }

  void HideRow(RowActors& row)
  {
    row.itemIndex = static_cast<size_t>(-1);
    row.row.SetProperty(Actor::Property::VISIBLE, false);
  }

  bool OnDetailTouched(Actor actor, TouchEvent touch)
  {
    const PointState::Type state = touch.GetState(0u);
    if(state != PointState::UP && state != PointState::FINISHED)
    {
      return true;
    }

    for(const RowActors& row : mRowPool)
    {
      if(row.detail == actor && row.itemIndex < mItems.size())
      {
        CopyItemToClipboard(row.itemIndex);
        return true;
      }
    }

    return true;
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
      if(row.preview == actor && row.itemIndex < mItems.size())
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
  AbsoluteLayout             mSearchOverlay;
  FlexLayout                 mNavigation;
  ScrollView                 mScrollView;
  View                       mSearchDismissLayer;
  View                       mSearchButton;
  Label                      mSearchIcon;
  Label                      mEmptyLabel;
  Label                      mLargePreview;
  InputField                 mSearchInput;
  AbsoluteLayoutParams       mEmptyLabelParams;
  AbsoluteLayoutParams       mLargePreviewParams;
  Animation                  mSearchAnimation;
  Timer                      mCaptureTimer;
  Timer                      mDetailDiagnosticTimer;
  Capture                    mCapture;
  Text::MultilanguageSupport mMultilanguageSupport;
  TextAbstraction::FontClient mFontClient;
  std::vector<VisualCase>    mItems;
  std::vector<RowActors>     mRowPool;
  std::vector<std::string>   mSearchIndex;
  std::vector<size_t>        mSearchMatches;
  std::vector<std::string>   mDetailTextCache;
  std::vector<std::string>   mBasicDetailTextCache;
  std::vector<bool>          mDetailTextQueued;
  std::vector<size_t>        mPendingDetailTextIndices;
  std::vector<std::pair<std::string, size_t>> mSectionAnchors;
  std::string                mLoadError;
  std::string                mSearchText;
  std::string                mSearchQuery;
  size_t                     mBaseShownCount{0u};
  size_t                     mShownCount{0u};
  size_t                     mPageStartIndex{0u};
  size_t                     mPendingDetailTextIndex{0u};
  float                      mLastPageContentWidth{-1.0f};
  size_t                     mLastVisibleRowCount{static_cast<size_t>(-1)};
  bool                       mIsPreviewOverlayVisible{false};
  bool                       mIsSearchOpen{false};
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
    config.SetDefaultFocusIndicatorEnabled(false);
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
