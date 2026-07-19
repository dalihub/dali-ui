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

// CLASS HEADER
#include <dali-ui-foundation/internal/text/styled-text/markup-to-styled-text.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/scripting/scripting.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>
#include <utility>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/markup-processor/markup-processor-helper-functions.h>
#include <dali-ui-foundation/internal/text/markup-tags-and-attributes.h>
#include <dali-ui-foundation/internal/text/text-effects-style.h>
#include <dali-ui-foundation/internal/text/text-font-style.h>
#include <dali-ui-foundation/internal/text/xhtml-entities.h>
#include <dali-ui-foundation/public-api/text/style/image-attributes.h>
#include <dali-ui-foundation/public-api/text/styled-text/anchor-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/annotation-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/background-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/font-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/foreground-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/image-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/line-through-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/replacement-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text-builder.h>
#include <dali-ui-foundation/public-api/text/styled-text/underline-span.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace Text
{
namespace
{

enum class TagType
{
  UNKNOWN,
  UNSUPPORTED,
  COLOR,
  FONT,
  BOLD,
  ITALIC,
  UNDERLINE,
  STRIKETHROUGH,
  BACKGROUND,
  ANCHOR,
  ANNOTATION,
  IMAGE,
};

struct Attribute
{
  std::string name;
  std::string value;
};

struct ParsedTag
{
  std::string            name;
  std::vector<Attribute> attributes;
  bool                   isEndTag{false};
  bool                   isSelfClosing{false};
};

struct OpenTag
{
  TagType                          type{TagType::UNKNOWN};
  uint32_t                         startIndex{0u};
  uint32_t                         openOrder{0u};
  bool                             createSpan{true};
  Dali::Ui::UiColor                color;
  Dali::Ui::Text::FontAttributes   fontAttributes;
  Dali::Ui::Text::Underline        underline;
  Dali::Ui::Text::LineThrough      lineThrough;
  Dali::Ui::Text::AnchorAttributes anchorAttributes;
  std::vector<Attribute>           annotationAttributes;
};

struct CompletedSpan
{
  Dali::Ui::Text::Span span;
  uint32_t             startIndex{0u};
  uint32_t             endIndex{0u};
  uint32_t             openOrder{0u};
  uint32_t             attributeOrder{0u};
};

bool IsAsciiAlpha(char c)
{
  return ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
}

bool IsAsciiDigit(char c)
{
  return (c >= '0') && (c <= '9');
}

bool IsHexDigit(char c)
{
  return ((c >= '0') && (c <= '9')) ||
         ((c >= 'a') && (c <= 'f')) ||
         ((c >= 'A') && (c <= 'F'));
}

bool IsNameChar(char c)
{
  return IsAsciiAlpha(c) || IsAsciiDigit(c) || (c == '-') || (c == '_');
}

char ToLowerAscii(char c)
{
  return ((c >= 'A') && (c <= 'Z')) ? static_cast<char>(c + ('a' - 'A')) : c;
}

std::string ToLowerAscii(const std::string& value)
{
  std::string lower(value);
  for(char& c : lower)
  {
    c = ToLowerAscii(c);
  }
  return lower;
}

bool EqualsIgnoreCase(const std::string& lhs, const char* rhs)
{
  if(!rhs || (lhs.size() != std::strlen(rhs)))
  {
    return false;
  }

  for(std::size_t index = 0u; index < lhs.size(); ++index)
  {
    if(ToLowerAscii(lhs[index]) != ToLowerAscii(rhs[index]))
    {
      return false;
    }
  }

  return true;
}

std::string ToStdString(const Dali::String& value)
{
  return value.Empty() ? std::string() : std::string(value.CStr(), value.Size());
}

std::size_t Utf8CharacterLength(unsigned char lead)
{
  if((lead & 0x80u) == 0u)
  {
    return 1u;
  }
  if((lead & 0xE0u) == 0xC0u)
  {
    return 2u;
  }
  if((lead & 0xF0u) == 0xE0u)
  {
    return 3u;
  }
  if((lead & 0xF8u) == 0xF0u)
  {
    return 4u;
  }
  return 1u;
}

uint32_t CountUtf8CodePoints(const std::string& text)
{
  uint32_t count = 0u;
  for(std::size_t index = 0u; index < text.size();)
  {
    std::size_t length = Utf8CharacterLength(static_cast<unsigned char>(text[index]));
    if(index + length > text.size())
    {
      length = 1u;
    }
    index += length;
    ++count;
  }
  return count;
}

bool TryBuildPlainTextMarkupResult(const Dali::String& markup, StyledTextMarkupResult& result)
{
  if(markup.Empty())
  {
    result.text        = markup;
    result.utf32Length = 0u;
    return true;
  }

  const char* const data = markup.CStr();
  const std::size_t size = markup.Size();

  uint32_t utf32Length = 0u;
  for(std::size_t index = 0u; index < size;)
  {
    const char c = data[index];
    if((c == '<') || (c == '&'))
    {
      return false;
    }

    std::size_t length = Utf8CharacterLength(static_cast<unsigned char>(c));
    if(index + length > size)
    {
      length = 1u;
    }

    index += length;
    ++utf32Length;
  }

  result.text        = markup;
  result.utf32Length = utf32Length;
  return true;
}

void AppendUtf8CodePoint(uint32_t codePoint, std::string& output)
{
  if(codePoint <= 0x7Fu)
  {
    output.push_back(static_cast<char>(codePoint));
  }
  else if(codePoint <= 0x7FFu)
  {
    output.push_back(static_cast<char>(0xC0u | ((codePoint >> 6u) & 0x1Fu)));
    output.push_back(static_cast<char>(0x80u | (codePoint & 0x3Fu)));
  }
  else if(codePoint <= 0xFFFFu)
  {
    output.push_back(static_cast<char>(0xE0u | ((codePoint >> 12u) & 0x0Fu)));
    output.push_back(static_cast<char>(0x80u | ((codePoint >> 6u) & 0x3Fu)));
    output.push_back(static_cast<char>(0x80u | (codePoint & 0x3Fu)));
  }
  else
  {
    output.push_back(static_cast<char>(0xF0u | ((codePoint >> 18u) & 0x07u)));
    output.push_back(static_cast<char>(0x80u | ((codePoint >> 12u) & 0x3Fu)));
    output.push_back(static_cast<char>(0x80u | ((codePoint >> 6u) & 0x3Fu)));
    output.push_back(static_cast<char>(0x80u | (codePoint & 0x3Fu)));
  }
}

bool TryDecodeNumericEntity(const std::string& entity, std::string& decoded)
{
  if((entity.size() < 4u) || (entity[0u] != '&') || (entity[1u] != '#') || (entity.back() != ';'))
  {
    return false;
  }

  const bool        isHex      = (entity.size() > 4u) && ((entity[2u] == 'x') || (entity[2u] == 'X'));
  const std::size_t beginIndex = isHex ? 3u : 2u;
  const std::size_t endIndex   = entity.size() - 1u;
  if(beginIndex >= endIndex)
  {
    return false;
  }

  uint32_t codePoint = 0u;
  for(std::size_t index = beginIndex; index < endIndex; ++index)
  {
    const char c     = entity[index];
    uint32_t   digit = 0u;
    if(isHex)
    {
      if(!IsHexDigit(c))
      {
        return false;
      }
      digit = (c >= '0' && c <= '9') ? static_cast<uint32_t>(c - '0') : (c >= 'a' && c <= 'f') ? static_cast<uint32_t>(c - 'a' + 10)
                                                                                               : static_cast<uint32_t>(c - 'A' + 10);
      if(codePoint > ((std::numeric_limits<uint32_t>::max() - digit) / 16u))
      {
        return false;
      }
      codePoint = codePoint * 16u + digit;
    }
    else
    {
      if(!IsAsciiDigit(c))
      {
        return false;
      }
      digit = static_cast<uint32_t>(c - '0');
      if(codePoint > ((std::numeric_limits<uint32_t>::max() - digit) / 10u))
      {
        return false;
      }
      codePoint = codePoint * 10u + digit;
    }
  }

  if((codePoint == 0u) || (codePoint > 0x10FFFFu) || ((codePoint >= 0xD800u) && (codePoint <= 0xDFFFu)))
  {
    return false;
  }

  AppendUtf8CodePoint(codePoint, decoded);
  return true;
}

bool TryDecodeEntityAt(const std::string& input, std::size_t index, std::string& decoded, std::size_t& consumed)
{
  const std::size_t semicolon = input.find(';', index);
  if(semicolon == std::string::npos)
  {
    return false;
  }

  const std::size_t length = semicolon - index + 1u;
  const std::string entity = input.substr(index, length);

  if((entity.size() >= 3u) && (entity[1u] == '#') && TryDecodeNumericEntity(entity, decoded))
  {
    consumed = length;
    return true;
  }

  if(const char* const named = Dali::Ui::Text::NamedEntityToUtf8(entity.c_str(), static_cast<unsigned int>(entity.size())))
  {
    decoded  = named;
    consumed = length;
    return true;
  }

  return false;
}

std::string DecodeEntities(const std::string& input)
{
  std::string output;
  output.reserve(input.size());

  for(std::size_t index = 0u; index < input.size();)
  {
    if(input[index] == '&')
    {
      std::string decoded;
      std::size_t consumed = 0u;
      if(TryDecodeEntityAt(input, index, decoded, consumed))
      {
        output += decoded;
        index += consumed;
        continue;
      }

      output.push_back(input[index++]);
      continue;
    }

    output.push_back(input[index++]);
  }

  return output;
}

bool TryParseFloat(const std::string& value, float& result)
{
  if(value.empty())
  {
    return false;
  }

  errno     = 0;
  char* end = nullptr;
  result    = std::strtof(value.c_str(), &end);
  if((end == value.c_str()) || (errno == ERANGE) || !std::isfinite(result))
  {
    return false;
  }

  if(*end == '\0')
  {
    return true;
  }

  if(((*end == 'f') || (*end == 'F')) && (*(end + 1) == '\0'))
  {
    return true;
  }

  return false;
}

bool IsColorTokenSupported(const std::string& value)
{
  if(value.empty())
  {
    return false;
  }

  const std::string lower = ToLowerAscii(value);
  if((lower == "black") || (lower == "white") || (lower == "red") || (lower == "green") ||
     (lower == "blue") || (lower == "yellow") || (lower == "magenta") || (lower == "cyan") ||
     (lower == "transparent"))
  {
    return true;
  }

  if(value[0u] == '#')
  {
    if((value.size() != 4u) && (value.size() != 7u) && (value.size() != 9u))
    {
      return false;
    }
    return std::all_of(value.begin() + 1, value.end(), IsHexDigit);
  }

  if((value.size() > 2u) && (lower[0u] == '0') && (lower[1u] == 'x'))
  {
    return std::all_of(value.begin() + 2, value.end(), IsHexDigit);
  }

  return false;
}

bool TryParseColor(const std::string& value, Dali::Ui::UiColor& color)
{
  if(!IsColorTokenSupported(value))
  {
    return false;
  }

  Vector4 rgba(Color::TRANSPARENT);
  Dali::Ui::Text::ColorStringToVector4(value.c_str(), static_cast<Dali::Ui::Text::Length>(value.size()), rgba);
  color = Dali::Ui::UiColor(rgba);
  return true;
}

bool TryCanonicalEnumToken(const std::string& value, const Scripting::StringEnum* table, uint32_t count, std::string& canonical)
{
  for(uint32_t index = 0u; index < count; ++index)
  {
    if(EqualsIgnoreCase(value, table[index].string))
    {
      canonical = table[index].string;
      return true;
    }
  }
  return false;
}

TagType GetTagType(const std::string& name)
{
  namespace MarkupTag = Dali::Ui::Text::MARKUP::TAG;

  if(name == MarkupTag::COLOR)
  {
    return TagType::COLOR;
  }
  if(name == MarkupTag::FONT)
  {
    return TagType::FONT;
  }
  if(name == MarkupTag::BOLD)
  {
    return TagType::BOLD;
  }
  if(name == MarkupTag::ITALIC)
  {
    return TagType::ITALIC;
  }
  if(name == MarkupTag::UNDERLINE)
  {
    return TagType::UNDERLINE;
  }
  if(name == MarkupTag::STRIKETHROUGH)
  {
    return TagType::STRIKETHROUGH;
  }
  if(name == MarkupTag::BACKGROUND)
  {
    return TagType::BACKGROUND;
  }
  if(name == MarkupTag::ANCHOR)
  {
    return TagType::ANCHOR;
  }
  if(name == MarkupTag::ANNOTATION)
  {
    return TagType::ANNOTATION;
  }
  if(name == MarkupTag::IMAGE)
  {
    return TagType::IMAGE;
  }
  if((name == MarkupTag::PARAGRAPH) || (name == MarkupTag::EMBEDDED_ITEM) ||
     (name == MarkupTag::SPAN) || (name == MarkupTag::CHARACTER_SPACING))
  {
    return TagType::UNSUPPORTED;
  }
  return TagType::UNKNOWN;
}

void SkipWhitespace(const std::string& input, std::size_t& index)
{
  while((index < input.size()) && (static_cast<unsigned char>(input[index]) <= 0x20u))
  {
    ++index;
  }
}

std::size_t FindTagEnd(const std::string& input, std::size_t startIndex)
{
  char quote = '\0';
  for(std::size_t index = startIndex + 1u; index < input.size(); ++index)
  {
    const char c = input[index];
    if(quote != '\0')
    {
      if(c == quote)
      {
        quote = '\0';
      }
      continue;
    }

    if((c == '\'') || (c == '"'))
    {
      quote = c;
      continue;
    }
    if(c == '>')
    {
      return index;
    }
  }

  return std::string::npos;
}

bool ParseTagContent(const std::string& content, ParsedTag& tag, MarkupParseInfo* info)
{
  std::size_t index = 0u;
  SkipWhitespace(content, index);
  if(index >= content.size())
  {
    return false;
  }

  if(content[index] == '/')
  {
    tag.isEndTag = true;
    ++index;
    SkipWhitespace(content, index);
  }

  if((index >= content.size()) || !IsAsciiAlpha(content[index]))
  {
    return false;
  }

  const std::size_t nameBegin = index;
  while((index < content.size()) && IsNameChar(content[index]))
  {
    ++index;
  }
  tag.name = ToLowerAscii(content.substr(nameBegin, index - nameBegin));

  if(tag.isEndTag)
  {
    return true;
  }

  while(index < content.size())
  {
    SkipWhitespace(content, index);
    if(index >= content.size())
    {
      break;
    }

    if(content[index] == '/')
    {
      tag.isSelfClosing = true;
      ++index;
      SkipWhitespace(content, index);
      break;
    }

    if(!IsNameChar(content[index]))
    {
      if(info)
      {
        ++info->malformedTagCount;
      }
      ++index;
      continue;
    }

    const std::size_t attributeNameBegin = index;
    while((index < content.size()) && IsNameChar(content[index]))
    {
      ++index;
    }

    Attribute attribute;
    attribute.name = ToLowerAscii(content.substr(attributeNameBegin, index - attributeNameBegin));

    SkipWhitespace(content, index);
    if((index < content.size()) && (content[index] == '='))
    {
      ++index;
      SkipWhitespace(content, index);

      if((index < content.size()) && ((content[index] == '\'') || (content[index] == '"')))
      {
        const char        quote      = content[index++];
        const std::size_t valueBegin = index;
        while((index < content.size()) && (content[index] != quote))
        {
          ++index;
        }

        attribute.value = DecodeEntities(content.substr(valueBegin, index - valueBegin));
        if(index < content.size())
        {
          ++index;
        }
        else if(info)
        {
          ++info->malformedTagCount;
        }
      }
      else
      {
        const std::size_t valueBegin = index;
        while((index < content.size()) && (static_cast<unsigned char>(content[index]) > 0x20u))
        {
          ++index;
        }
        attribute.value = DecodeEntities(content.substr(valueBegin, index - valueBegin));
      }
    }

    tag.attributes.push_back(std::move(attribute));
  }

  return true;
}

bool TryParseTagAt(const std::string& input, std::size_t startIndex, ParsedTag& tag, std::size_t& tagEndIndex, MarkupParseInfo* info)
{
  if((startIndex + 1u >= input.size()) || ((input[startIndex + 1u] != '/') && !IsAsciiAlpha(input[startIndex + 1u])))
  {
    return false;
  }

  tagEndIndex = FindTagEnd(input, startIndex);
  if(tagEndIndex == std::string::npos)
  {
    if(info)
    {
      ++info->malformedTagCount;
    }
    return false;
  }

  ParsedTag parsed;
  if(!ParseTagContent(input.substr(startIndex + 1u, tagEndIndex - startIndex - 1u), parsed, info))
  {
    return false;
  }

  tag = std::move(parsed);
  return true;
}

const Attribute* FindAttribute(const ParsedTag& tag, const std::string& name)
{
  const Attribute* found = nullptr;
  for(const auto& attribute : tag.attributes)
  {
    if(attribute.name == name)
    {
      found = &attribute;
    }
  }
  return found;
}

void CountInvalidAttribute(MarkupParseInfo* info)
{
  if(info)
  {
    ++info->invalidAttributeCount;
  }
}

bool BuildColorOpenTag(const ParsedTag& parsedTag, OpenTag& openTag, MarkupParseInfo* info)
{
  const Attribute* attribute = FindAttribute(parsedTag, Dali::Ui::Text::MARKUP::COLOR_ATTRIBUTES::VALUE);
  if(!attribute)
  {
    CountInvalidAttribute(info);
    return false;
  }

  if(!TryParseColor(attribute->value, openTag.color))
  {
    CountInvalidAttribute(info);
    return false;
  }

  return true;
}

bool BuildBackgroundOpenTag(const ParsedTag& parsedTag, OpenTag& openTag, MarkupParseInfo* info)
{
  const Attribute* attribute = FindAttribute(parsedTag, Dali::Ui::Text::MARKUP::BACKGROUND_ATTRIBUTES::COLOR);
  if(!attribute)
  {
    CountInvalidAttribute(info);
    return false;
  }

  if(!TryParseColor(attribute->value, openTag.color))
  {
    CountInvalidAttribute(info);
    return false;
  }

  return true;
}

bool ApplyFontAttribute(const Attribute& attribute, Dali::Ui::Text::FontAttributes& fontAttributes, MarkupParseInfo* info)
{
  namespace FontAttribute = Dali::Ui::Text::MARKUP::FONT_ATTRIBUTES;

  if(attribute.name == FontAttribute::FAMILY)
  {
    fontAttributes.SetFamily(Dali::String(attribute.value.c_str()));
    return true;
  }

  if(attribute.name == FontAttribute::SIZE)
  {
    float size = 0.0f;
    if(!TryParseFloat(attribute.value, size))
    {
      CountInvalidAttribute(info);
      return false;
    }
    fontAttributes.SetSize(size);
    return true;
  }

  if(attribute.name == FontAttribute::WEIGHT)
  {
    std::string canonical;
    if(!TryCanonicalEnumToken(attribute.value, Dali::Ui::Text::FONT_WEIGHT_STRING_TABLE, Dali::Ui::Text::FONT_WEIGHT_STRING_TABLE_COUNT, canonical))
    {
      CountInvalidAttribute(info);
      return false;
    }
    fontAttributes.SetWeight(Dali::Ui::Text::StringToWeight(canonical.c_str()));
    return true;
  }

  if(attribute.name == FontAttribute::WIDTH)
  {
    std::string canonical;
    if(!TryCanonicalEnumToken(attribute.value, Dali::Ui::Text::FONT_WIDTH_STRING_TABLE, Dali::Ui::Text::FONT_WIDTH_STRING_TABLE_COUNT, canonical))
    {
      CountInvalidAttribute(info);
      return false;
    }
    fontAttributes.SetWidth(Dali::Ui::Text::StringToWidth(canonical.c_str()));
    return true;
  }

  if(attribute.name == FontAttribute::SLANT)
  {
    std::string canonical;
    if(!TryCanonicalEnumToken(attribute.value, Dali::Ui::Text::FONT_SLANT_STRING_TABLE, Dali::Ui::Text::FONT_SLANT_STRING_TABLE_COUNT, canonical))
    {
      CountInvalidAttribute(info);
      return false;
    }
    fontAttributes.SetSlant(Dali::Ui::Text::StringToSlant(canonical.c_str()));
    return true;
  }

  return false;
}

bool BuildFontOpenTag(const ParsedTag& parsedTag, OpenTag& openTag, MarkupParseInfo* info)
{
  for(const auto& attribute : parsedTag.attributes)
  {
    ApplyFontAttribute(attribute, openTag.fontAttributes, info);
  }

  return openTag.fontAttributes.HasAttributes();
}

bool TryParseUnderlineType(const std::string& value, Dali::Ui::Text::Underline::Type& type)
{
  std::string canonical;
  if(!TryCanonicalEnumToken(value, Dali::Ui::Text::UNDERLINE_TYPE_STRING_TABLE, Dali::Ui::Text::UNDERLINE_TYPE_STRING_TABLE_COUNT, canonical))
  {
    return false;
  }

  type = Dali::Ui::Text::StringToUnderlineType(canonical.c_str());
  return true;
}

void ApplyUnderlineAttributes(const ParsedTag& parsedTag, Dali::Ui::Text::Underline& underline, MarkupParseInfo* info)
{
  namespace UnderlineAttribute = Dali::Ui::Text::MARKUP::UNDERLINE_ATTRIBUTES;

  for(const auto& attribute : parsedTag.attributes)
  {
    if(attribute.name == UnderlineAttribute::COLOR)
    {
      Dali::Ui::UiColor color;
      if(TryParseColor(attribute.value, color))
      {
        underline.SetColor(color);
      }
      else
      {
        CountInvalidAttribute(info);
      }
    }
    else if(attribute.name == UnderlineAttribute::HEIGHT)
    {
      float thickness = 0.0f;
      if(TryParseFloat(attribute.value, thickness))
      {
        underline.SetThickness(thickness);
      }
      else
      {
        CountInvalidAttribute(info);
      }
    }
    else if(attribute.name == UnderlineAttribute::TYPE)
    {
      Dali::Ui::Text::Underline::Type type = Dali::Ui::Text::Underline::Type::SOLID;
      if(TryParseUnderlineType(attribute.value, type))
      {
        underline.SetType(type);
      }
      else
      {
        CountInvalidAttribute(info);
      }
    }
    else if(attribute.name == UnderlineAttribute::DASH_GAP)
    {
      float dashGap = 0.0f;
      if(TryParseFloat(attribute.value, dashGap))
      {
        underline.SetDashGap(dashGap);
      }
      else
      {
        CountInvalidAttribute(info);
      }
    }
    else if(attribute.name == UnderlineAttribute::DASH_WIDTH)
    {
      float dashWidth = 0.0f;
      if(TryParseFloat(attribute.value, dashWidth))
      {
        underline.SetDashLength(dashWidth);
      }
      else
      {
        CountInvalidAttribute(info);
      }
    }
  }
}

void ApplyLineThroughAttributes(const ParsedTag& parsedTag, Dali::Ui::Text::LineThrough& lineThrough, MarkupParseInfo* info)
{
  namespace LineThroughAttribute = Dali::Ui::Text::MARKUP::STRIKETHROUGH_ATTRIBUTES;

  for(const auto& attribute : parsedTag.attributes)
  {
    if(attribute.name == LineThroughAttribute::COLOR)
    {
      Dali::Ui::UiColor color;
      if(TryParseColor(attribute.value, color))
      {
        lineThrough.SetColor(color);
      }
      else
      {
        CountInvalidAttribute(info);
      }
    }
    else if(attribute.name == LineThroughAttribute::HEIGHT)
    {
      float thickness = 0.0f;
      if(TryParseFloat(attribute.value, thickness))
      {
        lineThrough.SetThickness(thickness);
      }
      else
      {
        CountInvalidAttribute(info);
      }
    }
  }
}

bool BuildAnchorOpenTag(const ParsedTag& parsedTag, OpenTag& openTag, MarkupParseInfo* info)
{
  namespace AnchorAttribute = Dali::Ui::Text::MARKUP::ANCHOR_ATTRIBUTES;

  const Attribute* href = FindAttribute(parsedTag, AnchorAttribute::HREF);
  if(!href)
  {
    CountInvalidAttribute(info);
    return false;
  }

  openTag.anchorAttributes.SetHref(Dali::String(href->value.c_str()));

  if(const Attribute* colorAttribute = FindAttribute(parsedTag, AnchorAttribute::COLOR))
  {
    Dali::Ui::UiColor color;
    if(TryParseColor(colorAttribute->value, color))
    {
      openTag.anchorAttributes.SetColor(color);
    }
    else
    {
      CountInvalidAttribute(info);
    }
  }

  if(const Attribute* clickedColorAttribute = FindAttribute(parsedTag, AnchorAttribute::CLICKED_COLOR))
  {
    Dali::Ui::UiColor color;
    if(TryParseColor(clickedColorAttribute->value, color))
    {
      openTag.anchorAttributes.SetClickedColor(color);
    }
    else
    {
      CountInvalidAttribute(info);
    }
  }

  return true;
}

bool BuildAnnotationOpenTag(const ParsedTag& parsedTag, OpenTag& openTag)
{
  openTag.annotationAttributes = parsedTag.attributes;
  return !openTag.annotationAttributes.empty();
}

bool TryBuildImageSpan(const ParsedTag& parsedTag, Dali::Ui::Text::ImageSpan& imageSpan, MarkupParseInfo* info)
{
  namespace ImageAttribute = Dali::Ui::Text::MARKUP::IMAGE_ATTRIBUTES;

  const Attribute* sourceAttribute = FindAttribute(parsedTag, ImageAttribute::SOURCE);
  const Attribute* widthAttribute  = FindAttribute(parsedTag, ImageAttribute::WIDTH);
  const Attribute* heightAttribute = FindAttribute(parsedTag, ImageAttribute::HEIGHT);

  bool valid = true;
  if(!sourceAttribute || sourceAttribute->value.empty())
  {
    CountInvalidAttribute(info);
    valid = false;
  }

  float width = 0.0f;
  if(!widthAttribute || !TryParseFloat(widthAttribute->value, width) || (width <= 0.0f))
  {
    CountInvalidAttribute(info);
    valid = false;
  }

  float height = 0.0f;
  if(!heightAttribute || !TryParseFloat(heightAttribute->value, height) || (height <= 0.0f))
  {
    CountInvalidAttribute(info);
    valid = false;
  }

  if(!valid)
  {
    return false;
  }

  Dali::Ui::Text::ImageAttributes attributes(Dali::String(sourceAttribute->value.c_str()), Vector2(width, height));
  imageSpan = Dali::Ui::Text::ImageSpan::New(attributes);
  return static_cast<bool>(imageSpan);
}

class MarkupParser
{
public:
  MarkupParser(const Dali::String& markup, MarkupParseInfo* info)
  : mInput(ToStdString(markup)),
    mInfo(info)
  {
    mPlainText.reserve(mInput.size());
  }

  StyledTextMarkupResult Parse()
  {
    for(std::size_t index = 0u; index < mInput.size();)
    {
      if(mInput[index] == '<')
      {
        ParsedTag   parsedTag;
        std::size_t tagEndIndex = 0u;
        if(TryParseTagAt(mInput, index, parsedTag, tagEndIndex, mInfo))
        {
          ProcessTag(parsedTag);
          index = tagEndIndex + 1u;
          continue;
        }

        AppendTextAt(index);
        continue;
      }

      if(mInput[index] == '&')
      {
        AppendTextAt(index);
        continue;
      }

      AppendPlainTextRun(index);
    }

    CompleteOpenTagsAtEndOfFile();
    return BuildResult();
  }

private:
  void AppendPlainText(const std::string& text)
  {
    mPlainText += text;
    mPlainIndex += CountUtf8CodePoints(text);
  }

  void AppendPlainTextRun(std::size_t& index)
  {
    const std::size_t begin = index;

    while((index < mInput.size()) && (mInput[index] != '<') && (mInput[index] != '&'))
    {
      std::size_t length = Utf8CharacterLength(static_cast<unsigned char>(mInput[index]));
      if(index + length > mInput.size())
      {
        length = 1u;
      }

      index += length;
      ++mPlainIndex;
    }

    mPlainText.append(mInput, begin, index - begin);
  }

  void AppendTextAt(std::size_t& index)
  {
    if(mInput[index] == '&')
    {
      std::string decoded;
      std::size_t consumed = 0u;
      if(TryDecodeEntityAt(mInput, index, decoded, consumed))
      {
        AppendPlainText(decoded);
        index += consumed;
        return;
      }

      AppendPlainText("&");
      ++index;
      return;
    }

    const std::size_t length = std::min(Utf8CharacterLength(static_cast<unsigned char>(mInput[index])), mInput.size() - index);
    AppendPlainText(mInput.substr(index, length));
    index += length;
  }

  void ProcessTag(const ParsedTag& parsedTag)
  {
    const TagType tagType = GetTagType(parsedTag.name);
    if((tagType == TagType::UNKNOWN) || (tagType == TagType::UNSUPPORTED))
    {
      if(!parsedTag.isEndTag && mInfo)
      {
        ++mInfo->unsupportedTagCount;
      }
      return;
    }

    if(tagType == TagType::IMAGE)
    {
      if(!parsedTag.isEndTag)
      {
        AppendImage(parsedTag);
      }
      return;
    }

    if(parsedTag.isEndTag)
    {
      CloseTag(tagType);
      return;
    }

    if(parsedTag.isSelfClosing)
    {
      return;
    }

    OpenTag openTag;
    openTag.type       = tagType;
    openTag.startIndex = mPlainIndex;
    openTag.openOrder  = mNextOpenOrder++;

    openTag.createSpan = BuildOpenTag(parsedTag, openTag);
    mOpenTags.push_back(std::move(openTag));
  }

  void AppendImage(const ParsedTag& parsedTag)
  {
    Dali::Ui::Text::ImageSpan imageSpan;
    if(!TryBuildImageSpan(parsedTag, imageSpan, mInfo))
    {
      return;
    }

    const uint32_t startIndex = mPlainIndex;
    AppendPlainText(Dali::Ui::Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);
    mCompletedSpans.push_back({imageSpan, startIndex, mPlainIndex, mNextOpenOrder++, 0u});
  }

  bool BuildOpenTag(const ParsedTag& parsedTag, OpenTag& openTag)
  {
    switch(openTag.type)
    {
      case TagType::COLOR:
      {
        return BuildColorOpenTag(parsedTag, openTag, mInfo);
      }
      case TagType::BACKGROUND:
      {
        return BuildBackgroundOpenTag(parsedTag, openTag, mInfo);
      }
      case TagType::FONT:
      {
        return BuildFontOpenTag(parsedTag, openTag, mInfo);
      }
      case TagType::BOLD:
      {
        openTag.fontAttributes.SetWeight(Dali::Ui::Text::FontWeight::BOLD);
        return true;
      }
      case TagType::ITALIC:
      {
        openTag.fontAttributes.SetSlant(Dali::Ui::Text::FontSlant::ITALIC);
        return true;
      }
      case TagType::UNDERLINE:
      {
        openTag.underline = GetActiveUnderline();
        ApplyUnderlineAttributes(parsedTag, openTag.underline, mInfo);
        return true;
      }
      case TagType::STRIKETHROUGH:
      {
        openTag.lineThrough = GetActiveLineThrough();
        ApplyLineThroughAttributes(parsedTag, openTag.lineThrough, mInfo);
        return true;
      }
      case TagType::ANCHOR:
      {
        return BuildAnchorOpenTag(parsedTag, openTag, mInfo);
      }
      case TagType::ANNOTATION:
      {
        return BuildAnnotationOpenTag(parsedTag, openTag);
      }
      case TagType::IMAGE:
      case TagType::UNKNOWN:
      case TagType::UNSUPPORTED:
      {
        return false;
      }
    }

    return false;
  }

  Dali::Ui::Text::Underline GetActiveUnderline() const
  {
    for(auto iter = mOpenTags.rbegin(); iter != mOpenTags.rend(); ++iter)
    {
      if(iter->type == TagType::UNDERLINE)
      {
        return iter->underline;
      }
    }
    return Dali::Ui::Text::Underline();
  }

  Dali::Ui::Text::LineThrough GetActiveLineThrough() const
  {
    for(auto iter = mOpenTags.rbegin(); iter != mOpenTags.rend(); ++iter)
    {
      if(iter->type == TagType::STRIKETHROUGH)
      {
        return iter->lineThrough;
      }
    }
    return Dali::Ui::Text::LineThrough();
  }

  void CloseTag(TagType tagType)
  {
    auto iter = std::find_if(mOpenTags.rbegin(), mOpenTags.rend(), [tagType](const OpenTag& openTag)
    {
      return openTag.type == tagType;
    });

    if(iter == mOpenTags.rend())
    {
      if(mInfo)
      {
        ++mInfo->malformedTagCount;
      }
      return;
    }

    const bool mismatchedClose = (iter != mOpenTags.rbegin());
    auto       baseIter        = std::next(iter).base();
    CompleteOpenTag(*baseIter, mPlainIndex);
    mOpenTags.erase(baseIter);

    if(mismatchedClose && mInfo)
    {
      ++mInfo->malformedTagCount;
    }
  }

  void CompleteOpenTagsAtEndOfFile()
  {
    if(!mOpenTags.empty() && mInfo)
    {
      mInfo->malformedTagCount += static_cast<uint32_t>(mOpenTags.size());
    }

    for(const auto& openTag : mOpenTags)
    {
      CompleteOpenTag(openTag, mPlainIndex);
    }
    mOpenTags.clear();
  }

  void CompleteOpenTag(const OpenTag& openTag, uint32_t endIndex)
  {
    if(!openTag.createSpan)
    {
      return;
    }

    if(openTag.startIndex >= endIndex)
    {
      return;
    }

    if(openTag.type == TagType::ANNOTATION)
    {
      CompleteAnnotationOpenTag(openTag, endIndex);
      return;
    }

    Dali::Ui::Text::Span span = CreateSpan(openTag);
    if(!span)
    {
      return;
    }

    mCompletedSpans.push_back({span, openTag.startIndex, endIndex, openTag.openOrder, 0u});
  }

  void CompleteAnnotationOpenTag(const OpenTag& openTag, uint32_t endIndex)
  {
    for(uint32_t attributeIndex = 0u; attributeIndex < openTag.annotationAttributes.size(); ++attributeIndex)
    {
      const Attribute& attribute = openTag.annotationAttributes[attributeIndex];

      Dali::Ui::Text::Span span = Dali::Ui::Text::AnnotationSpan::New(Dali::String(attribute.name.c_str()), Dali::String(attribute.value.c_str()));
      if(span)
      {
        mCompletedSpans.push_back({span, openTag.startIndex, endIndex, openTag.openOrder, attributeIndex});
      }
    }
  }

  Dali::Ui::Text::Span CreateSpan(const OpenTag& openTag) const
  {
    switch(openTag.type)
    {
      case TagType::COLOR:
      {
        return Dali::Ui::Text::ForegroundColorSpan::New(openTag.color);
      }
      case TagType::BACKGROUND:
      {
        return Dali::Ui::Text::BackgroundColorSpan::New(openTag.color);
      }
      case TagType::FONT:
      case TagType::BOLD:
      case TagType::ITALIC:
      {
        return Dali::Ui::Text::FontSpan::New(openTag.fontAttributes);
      }
      case TagType::UNDERLINE:
      {
        return Dali::Ui::Text::UnderlineSpan::New(openTag.underline);
      }
      case TagType::STRIKETHROUGH:
      {
        return Dali::Ui::Text::LineThroughSpan::New(openTag.lineThrough);
      }
      case TagType::ANCHOR:
      {
        return Dali::Ui::Text::AnchorSpan::New(openTag.anchorAttributes);
      }
      case TagType::ANNOTATION:
      {
        return Dali::Ui::Text::Span();
      }
      case TagType::IMAGE:
      case TagType::UNKNOWN:
      case TagType::UNSUPPORTED:
      {
        return Dali::Ui::Text::Span();
      }
    }

    return Dali::Ui::Text::Span();
  }

  StyledTextMarkupResult BuildResult()
  {
    std::sort(mCompletedSpans.begin(), mCompletedSpans.end(), [](const CompletedSpan& lhs, const CompletedSpan& rhs)
    {
      if(lhs.openOrder != rhs.openOrder)
      {
        return lhs.openOrder < rhs.openOrder;
      }
      return lhs.attributeOrder < rhs.attributeOrder;
    });

    StyledTextMarkupResult result;
    result.text        = Dali::String(mPlainText.c_str());
    result.utf32Length = mPlainIndex;
    result.attachments.reserve(mCompletedSpans.size());

    uint32_t insertionOrder = 0u;
    for(const auto& completedSpan : mCompletedSpans)
    {
      if(!completedSpan.span || completedSpan.startIndex >= completedSpan.endIndex)
      {
        continue;
      }

      result.attachments.push_back({completedSpan.span, completedSpan.startIndex, completedSpan.endIndex, insertionOrder++});
    }

    return result;
  }

private:
  std::string                mInput;
  std::string                mPlainText;
  MarkupParseInfo*           mInfo{nullptr};
  std::vector<OpenTag>       mOpenTags;
  std::vector<CompletedSpan> mCompletedSpans;
  uint32_t                   mPlainIndex{0u};
  uint32_t                   mNextOpenOrder{0u};
};

} // unnamed namespace

StyledTextMarkupResult ParseStyledTextMarkup(const Dali::String& markup, MarkupParseInfo* info)
{
  StyledTextMarkupResult fastResult;
  if(TryBuildPlainTextMarkupResult(markup, fastResult))
  {
    return fastResult;
  }

  MarkupParser parser(markup, info);
  return parser.Parse();
}

Dali::Ui::Text::StyledText MarkupToStyledText(const Dali::String& markup, MarkupParseInfo* info)
{
  StyledTextMarkupResult result = ParseStyledTextMarkup(markup, info);

  Dali::Ui::Text::StyledTextBuilder builder = Dali::Ui::Text::StyledTextBuilder::New(result.text);
  for(const auto& attachment : result.attachments)
  {
    builder.SetSpan(attachment.span, attachment.startIndex, attachment.endIndex);
  }
  return builder.Build();
}

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali
