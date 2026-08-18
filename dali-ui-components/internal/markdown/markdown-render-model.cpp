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
#include <dali-ui-components/internal/markdown/markdown-render-model.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{

constexpr uint64_t FNV_PRIME = 1099511628211ull;

uint64_t HashUint64(uint64_t value, uint64_t seed)
{
  return MarkdownHashBytes(&value, static_cast<uint32_t>(sizeof(value)), seed);
}

} // namespace

uint64_t MarkdownHashBytes(const void* data, uint32_t size, uint64_t seed)
{
  const auto* bytes = static_cast<const uint8_t*>(data);
  uint64_t    hash  = seed;
  for(uint32_t i = 0u; i < size; ++i)
  {
    hash ^= static_cast<uint64_t>(bytes[i]);
    hash *= FNV_PRIME;
  }
  return hash;
}

uint64_t MarkdownHashString(const std::string& value, uint64_t seed)
{
  uint64_t hash = MarkdownHashBytes(value.data(), static_cast<uint32_t>(value.size()), seed);
  return HashUint64(static_cast<uint64_t>(value.size()), hash);
}

uint32_t MarkdownUtf8Length(const std::string& text)
{
  uint32_t length = 0u;
  for(unsigned char byte : text)
  {
    if((byte & 0xC0u) != 0x80u)
    {
      ++length;
    }
  }
  return length;
}

bool MarkdownIsTextRole(MarkdownRenderRole role)
{
  switch(role)
  {
    case MarkdownRenderRole::PARAGRAPH:
    case MarkdownRenderRole::LIST_ITEM:
    case MarkdownRenderRole::LIST_ITEM_PARAGRAPH:
    case MarkdownRenderRole::QUOTE_PARAGRAPH:
    case MarkdownRenderRole::TABLE_CELL_PARAGRAPH:
    case MarkdownRenderRole::HEADING:
    case MarkdownRenderRole::CODE_BLOCK:
    case MarkdownRenderRole::TABLE_CELL:
    case MarkdownRenderRole::RAW_HTML:
      return true;
    default:
      return false;
  }
}

bool MarkdownIsContainerRole(MarkdownRenderRole role)
{
  switch(role)
  {
    case MarkdownRenderRole::DOCUMENT:
    case MarkdownRenderRole::LIST:
    case MarkdownRenderRole::LIST_ITEM:
    case MarkdownRenderRole::QUOTE:
    case MarkdownRenderRole::TABLE:
    case MarkdownRenderRole::TABLE_HEAD:
    case MarkdownRenderRole::TABLE_BODY:
    case MarkdownRenderRole::TABLE_ROW:
    case MarkdownRenderRole::TABLE_CELL:
      return true;
    default:
      return false;
  }
}

bool MarkdownCanMaterializeRole(MarkdownRenderRole role)
{
  return role != MarkdownRenderRole::DOCUMENT && role != MarkdownRenderRole::RAW_HTML;
}

const char* MarkdownRenderRoleName(MarkdownRenderRole role)
{
  switch(role)
  {
    case MarkdownRenderRole::DOCUMENT:
      return "DOCUMENT";
    case MarkdownRenderRole::PARAGRAPH:
      return "PARAGRAPH";
    case MarkdownRenderRole::LIST_ITEM_PARAGRAPH:
      return "LIST_ITEM_PARAGRAPH";
    case MarkdownRenderRole::QUOTE_PARAGRAPH:
      return "QUOTE_PARAGRAPH";
    case MarkdownRenderRole::TABLE_CELL_PARAGRAPH:
      return "TABLE_CELL_PARAGRAPH";
    case MarkdownRenderRole::HEADING:
      return "HEADING";
    case MarkdownRenderRole::CODE_BLOCK:
      return "CODE_BLOCK";
    case MarkdownRenderRole::BLOCK_IMAGE:
      return "BLOCK_IMAGE";
    case MarkdownRenderRole::LIST:
      return "LIST";
    case MarkdownRenderRole::LIST_ITEM:
      return "LIST_ITEM";
    case MarkdownRenderRole::QUOTE:
      return "QUOTE";
    case MarkdownRenderRole::TABLE:
      return "TABLE";
    case MarkdownRenderRole::TABLE_HEAD:
      return "TABLE_HEAD";
    case MarkdownRenderRole::TABLE_BODY:
      return "TABLE_BODY";
    case MarkdownRenderRole::TABLE_ROW:
      return "TABLE_ROW";
    case MarkdownRenderRole::TABLE_CELL:
      return "TABLE_CELL";
    case MarkdownRenderRole::THEMATIC_BREAK:
      return "THEMATIC_BREAK";
    case MarkdownRenderRole::RAW_HTML:
      return "RAW_HTML";
  }
  return "UNKNOWN";
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
