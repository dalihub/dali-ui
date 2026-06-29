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

// EXTERNAL INCLUDES
#include <dali/integration-api/string-utils.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/text/font-variation/font-variation.h>

#include <cmath>
#include <cstdlib>
#include <string>

using Dali::Integration::ToStdString;

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace
{
constexpr std::size_t AXIS_TAG_LENGTH = 4u;

bool IsWhiteSpace(char c)
{
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool IsPrintableAscii(char c)
{
  return c >= 0x20 && c <= 0x7E;
}

void SkipWhiteSpace(const std::string& text, std::size_t& pos)
{
  while(pos < text.size() && IsWhiteSpace(text[pos]))
  {
    ++pos;
  }
}

bool ContainsSpace(const std::string& text)
{
  for(char c : text)
  {
    if(c == ' ')
    {
      return true;
    }
  }
  return false;
}

bool IsValidTag(const Dali::String& tag)
{
  const std::string stdTag = ToStdString(tag);

  if(stdTag.size() != AXIS_TAG_LENGTH)
  {
    return false;
  }

  bool seenTrailingSpace = false;
  for(char c : stdTag)
  {
    if(!IsPrintableAscii(c))
    {
      return false;
    }

    if(c == ' ')
    {
      seenTrailingSpace = true;
    }
    else if(seenTrailingSpace)
    {
      // Spaces are allowed only as trailing characters.
      return false;
    }
  }

  return true;
}

bool ParseTag(const std::string& text, std::size_t& pos, Dali::String& tag, bool& quoted)
{
  SkipWhiteSpace(text, pos);

  if(pos >= text.size())
  {
    return false;
  }

  quoted = false;

  if(text[pos] == '"' || text[pos] == '\'')
  {
    quoted             = true;
    const char   quote = text[pos++];
    const size_t start = pos;

    while(pos < text.size() && text[pos] != quote)
    {
      ++pos;
    }

    if(pos >= text.size())
    {
      return false;
    }

    tag = text.substr(start, pos - start).c_str();
    ++pos;

    return IsValidTag(tag);
  }

  const std::size_t start = pos;
  while(pos < text.size() && !IsWhiteSpace(text[pos]) && text[pos] != '=' && text[pos] != ',')
  {
    ++pos;
  }

  if(start == pos)
  {
    return false;
  }

  tag = text.substr(start, pos - start).c_str();
  return IsValidTag(tag);
}

bool ParseValue(const std::string& text, std::size_t& pos, float& value)
{
  SkipWhiteSpace(text, pos);

  if(pos >= text.size())
  {
    return false;
  }

  const char* begin = text.c_str() + pos;
  char*       end   = nullptr;
  value             = std::strtof(begin, &end);

  if(end == begin)
  {
    return false;
  }

  if(!std::isfinite(value))
  {
    return false;
  }

  pos = static_cast<std::size_t>(end - text.c_str());
  return true;
}

bool ValidateValueTerminator(const std::string& text, std::size_t pos)
{
  if(pos >= text.size())
  {
    return true;
  }

  const char c = text[pos];
  return IsWhiteSpace(c) || c == ',';
}

bool ConsumeSeparatorOrEnd(const std::string& text, std::size_t& pos)
{
  SkipWhiteSpace(text, pos);

  if(pos >= text.size())
  {
    return true;
  }

  if(text[pos] != ',')
  {
    return false;
  }

  ++pos;
  SkipWhiteSpace(text, pos);

  // Trailing comma is not allowed.
  return pos < text.size();
}

void UpsertAxis(Dali::Vector<FontVariationAxis>& axes, const FontVariationAxis& axis)
{
  const auto& tag = axis.GetTag();

  for(uint32_t i = 0u; i < axes.Count(); ++i)
  {
    if(axes[i].GetTag() == tag)
    {
      axes[i].SetValue(axis.GetValue());
      return;
    }
  }

  axes.PushBack(axis);
}

std::string FloatToString(float value)
{
  std::string text = std::to_string(value);

  while(!text.empty() && text.back() == '0')
  {
    text.pop_back();
  }

  if(!text.empty() && text.back() == '.')
  {
    text.pop_back();
  }

  if(text.empty() || text == "-")
  {
    text += '0';
  }

  return text;
}

} // namespace

const Dali::Vector<FontVariationAxis>& FontVariation::None()
{
  static const Dali::Vector<FontVariationAxis> none;
  return none;
}

Dali::Vector<FontVariationAxis> FontVariation::FromString(const Dali::String& settings)
{
  Dali::Vector<FontVariationAxis> axes;

  const std::string text(ToStdString(settings));
  std::size_t       pos = 0u;

  SkipWhiteSpace(text, pos);
  if(pos >= text.size())
  {
    return axes;
  }

  while(pos < text.size())
  {
    Dali::String tag;
    bool         quoted = false;
    if(!ParseTag(text, pos, tag, quoted))
    {
      axes.Clear();
      return axes;
    }

    if(quoted)
    {
      // Quoted format requires at least one whitespace separator before value.
      if(pos >= text.size() || !IsWhiteSpace(text[pos]))
      {
        axes.Clear();
        return axes;
      }

      SkipWhiteSpace(text, pos);
    }
    else
    {
      SkipWhiteSpace(text, pos);

      // Unquoted format requires '=' separator.
      if(pos >= text.size() || text[pos] != '=')
      {
        axes.Clear();
        return axes;
      }

      ++pos;
    }

    float value = 0.0f;
    if(!ParseValue(text, pos, value))
    {
      axes.Clear();
      return axes;
    }

    if(!ValidateValueTerminator(text, pos))
    {
      axes.Clear();
      return axes;
    }

    UpsertAxis(axes, FontVariationAxis(tag, value));

    if(!ConsumeSeparatorOrEnd(text, pos))
    {
      axes.Clear();
      return axes;
    }
  }

  return axes;
}

Dali::String FontVariation::ToString(const Dali::Vector<FontVariationAxis>& axes)
{
  std::string result;

  for(uint32_t i = 0u; i < axes.Count(); ++i)
  {
    if(i > 0u)
    {
      result += ",";
    }

    const std::string tag = ToStdString(axes[i].GetTag());
    if(ContainsSpace(tag))
    {
      result += "\"";
      result += tag;
      result += "\" ";
      result += FloatToString(axes[i].GetValue());
    }
    else
    {
      result += tag;
      result += "=";
      result += FloatToString(axes[i].GetValue());
    }
  }

  return Dali::String(result.c_str());
}

} // namespace Text
} // namespace Ui
} // namespace Dali
