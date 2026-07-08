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
#include <dali-ui-foundation/public-api/text/text-utils.h>

namespace Dali
{
namespace Ui
{
namespace Text
{

namespace
{

struct Utf8Step
{
  uint32_t byteLength{0u};
  bool     valid{false};
};

bool IsContinuationByte(unsigned char byte)
{
  return (byte & 0xC0u) == 0x80u;
}

bool IsByteInRange(unsigned char byte, unsigned char start, unsigned char end)
{
  return (byte >= start) && (byte <= end);
}

Utf8Step GetUtf8Step(const unsigned char* data, uint32_t remainingBytes)
{
  if((nullptr == data) || (0u == remainingBytes))
  {
    return {};
  }

  const unsigned char leadByte = data[0u];

  if((leadByte & 0x80u) == 0x00u)
  {
    return {1u, true};
  }

  if(IsByteInRange(leadByte, 0xC2u, 0xDFu))
  {
    if((remainingBytes >= 2u) &&
       IsContinuationByte(data[1u]))
    {
      return {2u, true};
    }
    return {};
  }

  if(0xE0u == leadByte)
  {
    if((remainingBytes >= 3u) &&
       IsByteInRange(data[1u], 0xA0u, 0xBFu) &&
       IsContinuationByte(data[2u]))
    {
      return {3u, true};
    }
    return {};
  }

  if(IsByteInRange(leadByte, 0xE1u, 0xECu))
  {
    if((remainingBytes >= 3u) &&
       IsContinuationByte(data[1u]) &&
       IsContinuationByte(data[2u]))
    {
      return {3u, true};
    }
    return {};
  }

  if(0xEDu == leadByte)
  {
    if((remainingBytes >= 3u) &&
       IsByteInRange(data[1u], 0x80u, 0x9Fu) &&
       IsContinuationByte(data[2u]))
    {
      return {3u, true};
    }
    return {};
  }

  if(IsByteInRange(leadByte, 0xEEu, 0xEFu))
  {
    if((remainingBytes >= 3u) &&
       IsContinuationByte(data[1u]) &&
       IsContinuationByte(data[2u]))
    {
      return {3u, true};
    }
    return {};
  }

  if(0xF0u == leadByte)
  {
    if((remainingBytes >= 4u) &&
       IsByteInRange(data[1u], 0x90u, 0xBFu) &&
       IsContinuationByte(data[2u]) &&
       IsContinuationByte(data[3u]))
    {
      return {4u, true};
    }
    return {};
  }

  if(IsByteInRange(leadByte, 0xF1u, 0xF3u))
  {
    if((remainingBytes >= 4u) &&
       IsContinuationByte(data[1u]) &&
       IsContinuationByte(data[2u]) &&
       IsContinuationByte(data[3u]))
    {
      return {4u, true};
    }
    return {};
  }

  if(0xF4u == leadByte)
  {
    if((remainingBytes >= 4u) &&
       IsByteInRange(data[1u], 0x80u, 0x8Fu) &&
       IsContinuationByte(data[2u]) &&
       IsContinuationByte(data[3u]))
    {
      return {4u, true};
    }
    return {};
  }

  return {};
}

bool HasReadableData(const char* data, uint32_t size)
{
  return (nullptr != data) || (0u == size);
}

bool ScanUtf8ToUtf32Range(
  const unsigned char* bytes,
  uint32_t             size,
  uint32_t             utf8StartIndex,
  uint32_t             utf8EndIndex,
  uint32_t&            utf32StartIndex,
  uint32_t&            utf32EndIndex)
{
  uint32_t localStartIndex = 0u;
  uint32_t localEndIndex   = 0u;
  bool     hasStartIndex   = false;

  uint32_t byteIndex  = 0u;
  uint32_t utf32Index = 0u;

  while(byteIndex < utf8EndIndex)
  {
    if(byteIndex == utf8StartIndex)
    {
      localStartIndex = utf32Index;
      hasStartIndex   = true;
    }

    const Utf8Step step = GetUtf8Step(bytes + byteIndex, size - byteIndex);
    if(!step.valid)
    {
      return false;
    }

    const uint32_t nextByteIndex = byteIndex + step.byteLength;
    if(((byteIndex < utf8StartIndex) && (utf8StartIndex < nextByteIndex)) ||
       ((byteIndex < utf8EndIndex) && (utf8EndIndex < nextByteIndex)))
    {
      return false;
    }

    byteIndex = nextByteIndex;
    ++utf32Index;
  }

  if(byteIndex != utf8EndIndex)
  {
    return false;
  }

  if(byteIndex == utf8StartIndex)
  {
    localStartIndex = utf32Index;
    hasStartIndex   = true;
  }

  if(!hasStartIndex)
  {
    return false;
  }

  localEndIndex = utf32Index;

  utf32StartIndex = localStartIndex;
  utf32EndIndex   = localEndIndex;
  return true;
}

bool ScanUtf32ToUtf8Range(
  const unsigned char* bytes,
  uint32_t             size,
  uint32_t             utf32StartIndex,
  uint32_t             utf32EndIndex,
  uint32_t&            utf8StartIndex,
  uint32_t&            utf8EndIndex)
{
  uint32_t localStartIndex = 0u;
  bool     hasStartIndex   = false;

  uint32_t byteIndex  = 0u;
  uint32_t utf32Index = 0u;

  while(utf32Index < utf32EndIndex)
  {
    if(utf32Index == utf32StartIndex)
    {
      localStartIndex = byteIndex;
      hasStartIndex   = true;
    }

    if(byteIndex >= size)
    {
      return false;
    }

    const Utf8Step step = GetUtf8Step(bytes + byteIndex, size - byteIndex);
    if(!step.valid)
    {
      return false;
    }

    byteIndex += step.byteLength;
    ++utf32Index;
  }

  if(utf32Index == utf32StartIndex)
  {
    localStartIndex = byteIndex;
    hasStartIndex   = true;
  }

  if(!hasStartIndex)
  {
    return false;
  }

  utf8StartIndex = localStartIndex;
  utf8EndIndex   = byteIndex;
  return true;
}

} // unnamed namespace

uint32_t Utf8ToUtf32Length(Dali::StringView utf8Text)
{
  const char* const data = utf8Text.Data();
  const uint32_t    size = utf8Text.Size();
  if(data == nullptr || size == 0u)
  {
    return 0u;
  }

  const unsigned char* const bytes       = reinterpret_cast<const unsigned char*>(data);
  uint32_t                   byteIndex   = 0u;
  uint32_t                   utf32Length = 0u;
  while(byteIndex < size)
  {
    const Utf8Step step = GetUtf8Step(bytes + byteIndex, size - byteIndex);
    byteIndex += step.valid ? step.byteLength : 1u;
    ++utf32Length;
  }

  return utf32Length;
}

bool Utf8ToUtf32Index(Dali::StringView utf8Text, uint32_t utf8Index, uint32_t& utf32Index)
{
  const char* const data = utf8Text.Data();
  const uint32_t    size = utf8Text.Size();
  if(!HasReadableData(data, size) || (utf8Index > size))
  {
    return false;
  }

  if(0u == size)
  {
    if(0u == utf8Index)
    {
      utf32Index = 0u;
      return true;
    }
    return false;
  }

  uint32_t localIndex = 0u;
  uint32_t endIndex   = 0u;
  if(!ScanUtf8ToUtf32Range(reinterpret_cast<const unsigned char*>(data), size, utf8Index, utf8Index, localIndex, endIndex))
  {
    return false;
  }

  utf32Index = localIndex;
  return true;
}

bool Utf8ToUtf32Range(
  Dali::StringView utf8Text,
  uint32_t         utf8StartIndex,
  uint32_t         utf8EndIndex,
  uint32_t&        utf32StartIndex,
  uint32_t&        utf32EndIndex)
{
  const char* const data = utf8Text.Data();
  const uint32_t    size = utf8Text.Size();
  if(!HasReadableData(data, size) ||
     (utf8StartIndex > utf8EndIndex) ||
     (utf8EndIndex > size))
  {
    return false;
  }

  if(0u == size)
  {
    if((0u == utf8StartIndex) && (0u == utf8EndIndex))
    {
      utf32StartIndex = 0u;
      utf32EndIndex   = 0u;
      return true;
    }
    return false;
  }

  uint32_t localStartIndex = 0u;
  uint32_t localEndIndex   = 0u;
  if(!ScanUtf8ToUtf32Range(reinterpret_cast<const unsigned char*>(data), size, utf8StartIndex, utf8EndIndex, localStartIndex, localEndIndex))
  {
    return false;
  }

  utf32StartIndex = localStartIndex;
  utf32EndIndex   = localEndIndex;
  return true;
}

bool Utf32ToUtf8Index(Dali::StringView utf8Text, uint32_t utf32Index, uint32_t& utf8Index)
{
  const char* const data = utf8Text.Data();
  const uint32_t    size = utf8Text.Size();
  if(!HasReadableData(data, size))
  {
    return false;
  }

  if(0u == size)
  {
    if(0u == utf32Index)
    {
      utf8Index = 0u;
      return true;
    }
    return false;
  }

  uint32_t localStartIndex = 0u;
  uint32_t localEndIndex   = 0u;
  if(!ScanUtf32ToUtf8Range(reinterpret_cast<const unsigned char*>(data), size, utf32Index, utf32Index, localStartIndex, localEndIndex))
  {
    return false;
  }

  utf8Index = localStartIndex;
  return true;
}

bool Utf32ToUtf8Range(
  Dali::StringView utf8Text,
  uint32_t         utf32StartIndex,
  uint32_t         utf32EndIndex,
  uint32_t&        utf8StartIndex,
  uint32_t&        utf8EndIndex)
{
  const char* const data = utf8Text.Data();
  const uint32_t    size = utf8Text.Size();
  if(!HasReadableData(data, size) ||
     (utf32StartIndex > utf32EndIndex))
  {
    return false;
  }

  if(0u == size)
  {
    if((0u == utf32StartIndex) && (0u == utf32EndIndex))
    {
      utf8StartIndex = 0u;
      utf8EndIndex   = 0u;
      return true;
    }
    return false;
  }

  uint32_t localStartIndex = 0u;
  uint32_t localEndIndex   = 0u;
  if(!ScanUtf32ToUtf8Range(reinterpret_cast<const unsigned char*>(data), size, utf32StartIndex, utf32EndIndex, localStartIndex, localEndIndex))
  {
    return false;
  }

  utf8StartIndex = localStartIndex;
  utf8EndIndex   = localEndIndex;
  return true;
}

} // namespace Text
} // namespace Ui
} // namespace Dali
