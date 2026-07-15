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
#include <dali-ui-components/internal/markdown/markdown-parser.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-string-view.h>
#include <third-party/md4c/md4c.h>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <sstream>
#include <utility>

extern "C" {
#include <third-party/md4c/entity.h>
}

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{

std::string ToString(const MD_CHAR* text, MD_SIZE size)
{
  return (text && size > 0u) ? std::string(text, text + size) : std::string();
}

constexpr uint32_t UNICODE_REPLACEMENT_CHARACTER = 0xFFFDU;

bool IsSurrogateCodepoint(uint32_t codepoint)
{
  return codepoint >= 0xD800u && codepoint <= 0xDFFFu;
}

void AppendUtf8(uint32_t codepoint, std::string& out)
{
  if(codepoint <= 0x7Fu)
  {
    out.push_back(static_cast<char>(codepoint));
  }
  else if(codepoint <= 0x7FFu)
  {
    out.push_back(static_cast<char>(0xC0u | ((codepoint >> 6u) & 0x1Fu)));
    out.push_back(static_cast<char>(0x80u | (codepoint & 0x3Fu)));
  }
  else if(codepoint <= 0xFFFFu)
  {
    out.push_back(static_cast<char>(0xE0u | ((codepoint >> 12u) & 0x0Fu)));
    out.push_back(static_cast<char>(0x80u | ((codepoint >> 6u) & 0x3Fu)));
    out.push_back(static_cast<char>(0x80u | (codepoint & 0x3Fu)));
  }
  else
  {
    out.push_back(static_cast<char>(0xF0u | ((codepoint >> 18u) & 0x07u)));
    out.push_back(static_cast<char>(0x80u | ((codepoint >> 12u) & 0x3Fu)));
    out.push_back(static_cast<char>(0x80u | ((codepoint >> 6u) & 0x3Fu)));
    out.push_back(static_cast<char>(0x80u | (codepoint & 0x3Fu)));
  }
}

void AppendValidatedUtf8(uint32_t codepoint, std::string& out)
{
  if(codepoint == 0u || codepoint > 0x10FFFFu || IsSurrogateCodepoint(codepoint))
  {
    codepoint = UNICODE_REPLACEMENT_CHARACTER;
  }
  AppendUtf8(codepoint, out);
}

std::string DecodeNumericEntity(const std::string& entity)
{
  if(entity.size() > 3u && entity[0] == '&' && entity[1] == '#')
  {
    const bool hex   = entity.size() > 4u && (entity[2] == 'x' || entity[2] == 'X');
    const auto begin = hex ? 3u : 2u;
    const auto end   = entity.size() - 1u;
    if(begin >= end)
    {
      return entity;
    }

    uint64_t codepoint = 0u;
    for(std::size_t i = begin; i < end; ++i)
    {
      const unsigned char c = static_cast<unsigned char>(entity[i]);
      uint32_t            digit{0u};
      if(hex && c >= '0' && c <= '9')
      {
        digit = c - '0';
      }
      else if(hex && c >= 'a' && c <= 'f')
      {
        digit = c - 'a' + 10u;
      }
      else if(hex && c >= 'A' && c <= 'F')
      {
        digit = c - 'A' + 10u;
      }
      else if(!hex && c >= '0' && c <= '9')
      {
        digit = c - '0';
      }
      else
      {
        return entity;
      }

      codepoint = codepoint * (hex ? 16u : 10u) + digit;
      if(codepoint > 0x10FFFFu)
      {
        break;
      }
    }

    std::string decoded;
    AppendValidatedUtf8(static_cast<uint32_t>(codepoint), decoded);
    return decoded;
  }

  return entity;
}

std::string DecodeEntity(const std::string& entity)
{
  if(entity.size() > 3u && entity[0] == '&' && entity[1] == '#')
  {
    return DecodeNumericEntity(entity);
  }

  const ENTITY* named = entity_lookup(entity.c_str(), entity.size());
  if(named)
  {
    std::string decoded;
    AppendValidatedUtf8(named->codepoints[0], decoded);
    if(named->codepoints[1] != 0u)
    {
      AppendValidatedUtf8(named->codepoints[1], decoded);
    }
    return decoded;
  }

  return entity;
}

std::string DecodeAttribute(const MD_ATTRIBUTE& attribute)
{
  std::string out;
  if(!attribute.text || attribute.size == 0u)
  {
    return out;
  }

  if(!attribute.substr_offsets || !attribute.substr_types)
  {
    return ToString(attribute.text, attribute.size);
  }

  for(MD_SIZE i = 0u; attribute.substr_offsets[i] < attribute.size; ++i)
  {
    const MD_OFFSET begin = attribute.substr_offsets[i];
    const MD_OFFSET end   = attribute.substr_offsets[i + 1u];
    std::string     text  = ToString(attribute.text + begin, end - begin);
    switch(attribute.substr_types[i])
    {
      case MD_TEXT_ENTITY:
        out += DecodeEntity(text);
        break;
      case MD_TEXT_NULLCHAR:
        out += "\xEF\xBF\xBD";
        break;
      default:
        out += text;
        break;
    }
  }
  return out;
}

uint64_t HashStyleRuns(const std::vector<MarkdownTextStyleRun>& runs, uint64_t seed)
{
  uint64_t hash = seed;
  for(const auto& run : runs)
  {
    hash = MarkdownHashBytes(&run.start, static_cast<uint32_t>(sizeof(run.start)), hash);
    hash = MarkdownHashBytes(&run.end, static_cast<uint32_t>(sizeof(run.end)), hash);
    hash = MarkdownHashBytes(&run.flags, static_cast<uint32_t>(sizeof(run.flags)), hash);
  }
  return hash;
}

uint64_t HashLinkRanges(const std::vector<MarkdownLinkRange>& links, uint64_t seed)
{
  uint64_t hash = seed;
  for(const auto& link : links)
  {
    hash = MarkdownHashBytes(&link.start, static_cast<uint32_t>(sizeof(link.start)), hash);
    hash = MarkdownHashBytes(&link.end, static_cast<uint32_t>(sizeof(link.end)), hash);
    hash = MarkdownHashString(link.href, hash);
    hash = MarkdownHashString(link.title, hash);
    hash = MarkdownHashBytes(&link.isAutolink, static_cast<uint32_t>(sizeof(link.isAutolink)), hash);
  }
  return hash;
}

uint64_t HashInlineObjects(const std::vector<MarkdownInlineObject>& objects, uint64_t seed)
{
  uint64_t hash = seed;
  for(const auto& object : objects)
  {
    const auto type = static_cast<uint32_t>(object.type);
    hash            = MarkdownHashBytes(&type, static_cast<uint32_t>(sizeof(type)), hash);
    hash            = MarkdownHashBytes(&object.position, static_cast<uint32_t>(sizeof(object.position)), hash);
    hash            = MarkdownHashBytes(&object.length, static_cast<uint32_t>(sizeof(object.length)), hash);
    hash            = MarkdownHashString(object.sourceUrl, hash);
    hash            = MarkdownHashString(object.title, hash);
    hash            = MarkdownHashString(object.altText, hash);
  }
  return hash;
}

uint64_t HashUint64Value(uint64_t value, uint64_t seed)
{
  return MarkdownHashBytes(&value, static_cast<uint32_t>(sizeof(value)), seed);
}

bool IsRoleInAncestry(const MarkdownRenderSnapshot& snapshot, const std::vector<uint32_t>& stack, MarkdownRenderRole role)
{
  for(auto it = stack.rbegin(); it != stack.rend(); ++it)
  {
    if(snapshot.nodes[*it].role == role)
    {
      return true;
    }
  }
  return false;
}

uint32_t CountListAncestors(const MarkdownRenderSnapshot& snapshot, const std::vector<uint32_t>& stack)
{
  uint32_t depth = 0u;
  for(uint32_t index : stack)
  {
    if(snapshot.nodes[index].role == MarkdownRenderRole::LIST)
    {
      ++depth;
    }
  }
  return depth;
}

bool IsWhitespaceOnly(const std::string& text)
{
  return std::all_of(text.begin(), text.end(), [](unsigned char c)
  {
    return std::isspace(c) != 0;
  });
}

uint32_t DecimalDigitCount(uint32_t value)
{
  uint32_t count = 1u;
  while(value >= 10u)
  {
    value /= 10u;
    ++count;
  }
  return count;
}

struct ActiveSpan
{
  MD_SPANTYPE type{MD_SPAN_EM};
  uint32_t    start{0u};
  uint32_t    flags{MARKDOWN_TEXT_STYLE_NONE};
  std::string href;
  std::string title;
  std::string sourceUrl;
  bool        isAutolink{false};
};

/**
 * @brief Builds a Markdown render snapshot from md4c callbacks.
 */
class SnapshotBuilder
{
public:
  SnapshotBuilder(uint64_t revision, uint32_t sourceSize, bool plainTextMode)
  : mPlainTextMode(plainTextMode)
  {
    snapshot.revision   = revision;
    snapshot.sourceSize = sourceSize;
  }

  MarkdownRenderSnapshot snapshot;

  int EnterBlock(MD_BLOCKTYPE type, void* detail)
  {
    MarkdownRenderNode node;
    node.index       = static_cast<uint32_t>(snapshot.nodes.size());
    node.parentIndex = blockStack.empty() ? MARKDOWN_INVALID_NODE_INDEX : blockStack.back();
    node.depth       = static_cast<uint32_t>(blockStack.size());
    node.role        = ResolveRole(type, detail);
    node.path        = MakePath(node.parentIndex);

    ApplyBlockDetails(node, type, detail);
    snapshot.nodes.push_back(std::move(node));
    childCounts.push_back(0u);
    childIndices.emplace_back();
    listItemCounts.push_back(0u);
    if(snapshot.nodes.back().parentIndex != MARKDOWN_INVALID_NODE_INDEX)
    {
      childIndices[snapshot.nodes.back().parentIndex].push_back(snapshot.nodes.back().index);
    }
    blockStack.push_back(static_cast<uint32_t>(snapshot.nodes.size() - 1u));
    return 0;
  }

  int LeaveBlock(MD_BLOCKTYPE type)
  {
    if(blockStack.empty())
    {
      return 1;
    }

    auto& node = snapshot.nodes[blockStack.back()];
    if(type == MD_BLOCK_P && node.role == MarkdownRenderRole::PARAGRAPH && node.inlineObjects.size() == 1u)
    {
      const auto& image = node.inlineObjects.front();
      if(IsWhitespaceOutsideImage(node, image))
      {
        node.role      = MarkdownRenderRole::BLOCK_IMAGE;
        node.sourceUrl = image.sourceUrl;
        node.title     = image.title;
        node.altText   = image.altText;
      }
    }

    ApplyListMarkerColumnLength(node);
    FinalizeNode(node);
    blockStack.pop_back();
    return 0;
  }

  int EnterSpan(MD_SPANTYPE type, void* detail)
  {
    auto* node = CurrentTextNode();
    if(!node)
    {
      return 0;
    }

    ActiveSpan span;
    span.type  = type;
    span.start = node->utf32Length;

    switch(type)
    {
      case MD_SPAN_EM:
        span.flags = MARKDOWN_TEXT_STYLE_EMPHASIS;
        break;
      case MD_SPAN_STRONG:
        span.flags = MARKDOWN_TEXT_STYLE_STRONG;
        break;
      case MD_SPAN_CODE:
        span.flags = MARKDOWN_TEXT_STYLE_INLINE_CODE;
        break;
      case MD_SPAN_DEL:
        span.flags = MARKDOWN_TEXT_STYLE_STRIKETHROUGH;
        break;
      case MD_SPAN_U:
        span.flags = MARKDOWN_TEXT_STYLE_UNDERLINE;
        break;
      case MD_SPAN_A:
      {
        const auto* a = static_cast<const MD_SPAN_A_DETAIL*>(detail);
        if(a)
        {
          span.href       = DecodeAttribute(a->href);
          span.title      = DecodeAttribute(a->title);
          span.isAutolink = a->is_autolink != 0;
        }
        break;
      }
      case MD_SPAN_IMG:
      {
        const auto* image = static_cast<const MD_SPAN_IMG_DETAIL*>(detail);
        if(image)
        {
          span.sourceUrl = DecodeAttribute(image->src);
          span.title     = DecodeAttribute(image->title);
        }
        break;
      }
      default:
        break;
    }

    spanStack.push_back(std::move(span));
    return 0;
  }

  int LeaveSpan(MD_SPANTYPE type)
  {
    auto* node = CurrentTextNode();
    if(!node)
    {
      return 0;
    }

    for(auto it = spanStack.rbegin(); it != spanStack.rend(); ++it)
    {
      if(it->type == type)
      {
        ActiveSpan span = *it;
        spanStack.erase(std::next(it).base());
        const uint32_t end     = node->utf32Length;
        const bool     hasText = end > span.start;
        if(type == MD_SPAN_A)
        {
          node->linkRanges.push_back({span.start, end, span.href, span.title, span.isAutolink});
        }
        else if(type == MD_SPAN_IMG)
        {
          if(hasText || mPlainTextMode)
          {
            MarkdownInlineObject image;
            image.position  = span.start;
            image.length    = end - span.start;
            image.sourceUrl = span.sourceUrl;
            image.title     = span.title;
            image.altText   = ExtractUtf8Range(node->text, span.start, end);
            node->inlineObjects.push_back(std::move(image));
          }
        }
        else if(hasText && span.flags != MARKDOWN_TEXT_STYLE_NONE)
        {
          node->styleRuns.push_back({span.start, end, span.flags});
        }
        return 0;
      }
    }
    return 0;
  }

  int Text(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size)
  {
    auto* node = CurrentTextNode();
    if(!node)
    {
      return 0;
    }

    std::string fragment;
    switch(type)
    {
      case MD_TEXT_NULLCHAR:
        fragment = "\xEF\xBF\xBD";
        break;
      case MD_TEXT_BR:
        fragment = "\n";
        if(mPlainTextMode)
        {
          snapshot.plainTextLineBreaks.push_back({node->index, node->utf32Length});
        }
        break;
      case MD_TEXT_SOFTBR:
        fragment = " ";
        if(mPlainTextMode)
        {
          snapshot.plainTextLineBreaks.push_back({node->index, node->utf32Length});
        }
        break;
      case MD_TEXT_HTML:
        if(!mPlainTextMode)
        {
          fragment = ToString(text, size);
        }
        break;
      case MD_TEXT_ENTITY:
        fragment = DecodeEntity(ToString(text, size));
        break;
      default:
        fragment = ToString(text, size);
        break;
    }

    node->text += fragment;
    node->utf32Length += MarkdownUtf8Length(fragment);
    return 0;
  }

  void DebugLog(const char* message)
  {
    if(message)
    {
      if(!snapshot.errorMessage.empty())
      {
        snapshot.errorMessage += "\n";
      }
      snapshot.errorMessage += message;
    }
  }

private:
  bool mPlainTextMode{false};

  MarkdownRenderRole ResolveRole(MD_BLOCKTYPE type, void* detail) const
  {
    switch(type)
    {
      case MD_BLOCK_DOC:
        return MarkdownRenderRole::DOCUMENT;
      case MD_BLOCK_QUOTE:
        return MarkdownRenderRole::QUOTE;
      case MD_BLOCK_UL:
      case MD_BLOCK_OL:
        return MarkdownRenderRole::LIST;
      case MD_BLOCK_LI:
        return MarkdownRenderRole::LIST_ITEM;
      case MD_BLOCK_HR:
        return MarkdownRenderRole::THEMATIC_BREAK;
      case MD_BLOCK_H:
        return MarkdownRenderRole::HEADING;
      case MD_BLOCK_CODE:
        return MarkdownRenderRole::CODE_BLOCK;
      case MD_BLOCK_HTML:
        return MarkdownRenderRole::RAW_HTML;
      case MD_BLOCK_P:
        return ResolveParagraphRole();
      case MD_BLOCK_TABLE:
        return MarkdownRenderRole::TABLE;
      case MD_BLOCK_THEAD:
        return MarkdownRenderRole::TABLE_HEAD;
      case MD_BLOCK_TBODY:
        return MarkdownRenderRole::TABLE_BODY;
      case MD_BLOCK_TR:
        return MarkdownRenderRole::TABLE_ROW;
      case MD_BLOCK_TH:
      case MD_BLOCK_TD:
        return MarkdownRenderRole::TABLE_CELL;
    }
    return MarkdownRenderRole::PARAGRAPH;
  }

  MarkdownRenderRole ResolveParagraphRole() const
  {
    if(!blockStack.empty())
    {
      const auto parentRole = snapshot.nodes[blockStack.back()].role;
      if(parentRole == MarkdownRenderRole::LIST_ITEM)
      {
        return MarkdownRenderRole::LIST_ITEM_PARAGRAPH;
      }
      if(parentRole == MarkdownRenderRole::TABLE_CELL)
      {
        return MarkdownRenderRole::TABLE_CELL_PARAGRAPH;
      }
    }

    if(IsRoleInAncestry(snapshot, blockStack, MarkdownRenderRole::QUOTE))
    {
      return MarkdownRenderRole::QUOTE_PARAGRAPH;
    }

    return MarkdownRenderRole::PARAGRAPH;
  }

  void ApplyBlockDetails(MarkdownRenderNode& node, MD_BLOCKTYPE type, void* detail)
  {
    switch(type)
    {
      case MD_BLOCK_UL:
      {
        const auto* ul = static_cast<const MD_BLOCK_UL_DETAIL*>(detail);
        node.listKind  = MarkdownListKind::UNORDERED;
        node.tightList = ul && ul->is_tight != 0;
        node.listDepth = CountListAncestors(snapshot, blockStack) + 1u;
        break;
      }
      case MD_BLOCK_OL:
      {
        const auto* ol = static_cast<const MD_BLOCK_OL_DETAIL*>(detail);
        node.listKind  = MarkdownListKind::ORDERED;
        node.listStart = ol ? ol->start : 1u;
        node.tightList = ol && ol->is_tight != 0;
        node.listDepth = CountListAncestors(snapshot, blockStack) + 1u;
        break;
      }
      case MD_BLOCK_LI:
      {
        const auto* li = static_cast<const MD_BLOCK_LI_DETAIL*>(detail);
        if(li && li->is_task)
        {
          node.taskListItem = true;
          node.taskChecked  = li->task_mark == 'x' || li->task_mark == 'X';
        }
        if(node.parentIndex != MARKDOWN_INVALID_NODE_INDEX)
        {
          auto& parent     = snapshot.nodes[node.parentIndex];
          node.listKind    = parent.listKind;
          node.listStart   = parent.listStart;
          node.listDepth   = parent.listDepth;
          node.listOrdinal = parent.listStart + listItemCounts[node.parentIndex]++;
        }
        break;
      }
      case MD_BLOCK_H:
      {
        const auto* h     = static_cast<const MD_BLOCK_H_DETAIL*>(detail);
        node.headingLevel = h ? h->level : 1u;
        break;
      }
      case MD_BLOCK_CODE:
      {
        const auto* code = static_cast<const MD_BLOCK_CODE_DETAIL*>(detail);
        if(code)
        {
          node.language = DecodeAttribute(code->lang);
          if(node.language.empty())
          {
            node.language = DecodeAttribute(code->info);
          }
        }
        break;
      }
      case MD_BLOCK_TH:
      case MD_BLOCK_TD:
      {
        const auto* cell = static_cast<const MD_BLOCK_TD_DETAIL*>(detail);
        if(cell)
        {
          switch(cell->align)
          {
            case MD_ALIGN_LEFT:
              node.tableAlignment = MarkdownTableAlignment::LEFT;
              break;
            case MD_ALIGN_CENTER:
              node.tableAlignment = MarkdownTableAlignment::CENTER;
              break;
            case MD_ALIGN_RIGHT:
              node.tableAlignment = MarkdownTableAlignment::RIGHT;
              break;
            default:
              node.tableAlignment = MarkdownTableAlignment::DEFAULT;
              break;
          }
        }
        break;
      }
      default:
        break;
    }
  }

  std::string MakePath(uint32_t parentIndex)
  {
    if(parentIndex == MARKDOWN_INVALID_NODE_INDEX)
    {
      return "r";
    }

    const uint32_t     ordinal = childCounts[parentIndex]++;
    std::ostringstream stream;
    stream << snapshot.nodes[parentIndex].path << "/" << ordinal;
    return stream.str();
  }

  MarkdownRenderNode* CurrentTextNode()
  {
    for(auto it = blockStack.rbegin(); it != blockStack.rend(); ++it)
    {
      auto& node = snapshot.nodes[*it];
      if(MarkdownIsTextRole(node.role))
      {
        return &node;
      }
    }
    return nullptr;
  }

  std::string ExtractUtf8Range(const std::string& text, uint32_t utf32Start, uint32_t utf32End) const
  {
    if(utf32Start >= utf32End)
    {
      return std::string();
    }

    uint32_t    utf32Index = 0u;
    std::size_t byteStart  = text.size();
    std::size_t byteEnd    = text.size();
    bool        foundStart = false;
    bool        foundEnd   = false;

    for(std::size_t i = 0u; i < text.size(); ++i)
    {
      const auto byte = static_cast<unsigned char>(text[i]);
      if((byte & 0xC0u) != 0x80u)
      {
        if(utf32Index == utf32Start)
        {
          byteStart  = i;
          foundStart = true;
        }
        if(utf32Index == utf32End)
        {
          byteEnd  = i;
          foundEnd = true;
          break;
        }
        ++utf32Index;
      }
    }

    if(!foundStart && utf32Start == utf32Index)
    {
      byteStart = text.size();
    }
    if(!foundEnd && utf32End == utf32Index)
    {
      byteEnd = text.size();
    }
    if(byteStart > byteEnd)
    {
      return std::string();
    }
    return text.substr(byteStart, byteEnd - byteStart);
  }

  bool IsWhitespaceOutsideImage(const MarkdownRenderNode& node, const MarkdownInlineObject& image) const
  {
    const std::string before = ExtractUtf8Range(node.text, 0u, image.position);
    const std::string after  = ExtractUtf8Range(node.text, image.position + image.length, node.utf32Length);
    return IsWhitespaceOnly(before) && IsWhitespaceOnly(after);
  }

  void FinalizeNode(MarkdownRenderNode& node)
  {
    uint64_t contentHash = MarkdownHashString(node.text);
    contentHash          = HashStyleRuns(node.styleRuns, contentHash);
    contentHash          = HashLinkRanges(node.linkRanges, contentHash);
    contentHash          = HashInlineObjects(node.inlineObjects, contentHash);
    node.contentHash     = contentHash;

    uint64_t attrHash  = MarkdownHashBytes(&node.role, static_cast<uint32_t>(sizeof(node.role)));
    attrHash           = MarkdownHashBytes(&node.headingLevel, static_cast<uint32_t>(sizeof(node.headingLevel)), attrHash);
    attrHash           = MarkdownHashBytes(&node.listKind, static_cast<uint32_t>(sizeof(node.listKind)), attrHash);
    attrHash           = MarkdownHashBytes(&node.listStart, static_cast<uint32_t>(sizeof(node.listStart)), attrHash);
    attrHash           = MarkdownHashBytes(&node.listOrdinal, static_cast<uint32_t>(sizeof(node.listOrdinal)), attrHash);
    attrHash           = MarkdownHashBytes(&node.listDepth, static_cast<uint32_t>(sizeof(node.listDepth)), attrHash);
    attrHash           = MarkdownHashBytes(&node.listMarkerColumnLength, static_cast<uint32_t>(sizeof(node.listMarkerColumnLength)), attrHash);
    attrHash           = MarkdownHashBytes(&node.tightList, static_cast<uint32_t>(sizeof(node.tightList)), attrHash);
    attrHash           = MarkdownHashBytes(&node.taskListItem, static_cast<uint32_t>(sizeof(node.taskListItem)), attrHash);
    attrHash           = MarkdownHashBytes(&node.taskChecked, static_cast<uint32_t>(sizeof(node.taskChecked)), attrHash);
    attrHash           = MarkdownHashBytes(&node.tableAlignment, static_cast<uint32_t>(sizeof(node.tableAlignment)), attrHash);
    attrHash           = MarkdownHashString(node.language, attrHash);
    attrHash           = MarkdownHashString(node.sourceUrl, attrHash);
    attrHash           = MarkdownHashString(node.title, attrHash);
    attrHash           = MarkdownHashString(node.altText, attrHash);
    node.attributeHash = attrHash;

    node.styleHash = HashStyleRuns(node.styleRuns, HashLinkRanges(node.linkRanges, MARKDOWN_HASH_SEED));

    uint64_t structureHash = attrHash;
    structureHash          = HashUint64Value(static_cast<uint64_t>(childIndices[node.index].size()), structureHash);
    for(uint32_t childIndex : childIndices[node.index])
    {
      const auto& child = snapshot.nodes[childIndex];
      structureHash     = HashUint64Value(static_cast<uint64_t>(child.role), structureHash);
      structureHash     = HashUint64Value(child.structureHash, structureHash);
    }
    node.structureHash = structureHash;

    uint64_t subtreeHash = contentHash;
    subtreeHash          = HashUint64Value(attrHash, subtreeHash);
    subtreeHash          = HashUint64Value(node.styleHash, subtreeHash);
    subtreeHash          = HashUint64Value(static_cast<uint64_t>(childIndices[node.index].size()), subtreeHash);
    for(uint32_t childIndex : childIndices[node.index])
    {
      subtreeHash = HashUint64Value(snapshot.nodes[childIndex].subtreeHash, subtreeHash);
    }
    node.subtreeHash = subtreeHash;
  }

  void ApplyListMarkerColumnLength(MarkdownRenderNode& node)
  {
    if(node.role != MarkdownRenderRole::LIST || node.listKind != MarkdownListKind::ORDERED)
    {
      return;
    }

    uint32_t maxMarkerLength = 0u;
    for(uint32_t childIndex : childIndices[node.index])
    {
      const auto& child = snapshot.nodes[childIndex];
      if(child.role == MarkdownRenderRole::LIST_ITEM && child.listKind == MarkdownListKind::ORDERED)
      {
        maxMarkerLength = std::max(maxMarkerLength, DecimalDigitCount(child.listOrdinal) + 1u);
      }
    }

    if(maxMarkerLength == 0u)
    {
      return;
    }

    for(uint32_t childIndex : childIndices[node.index])
    {
      auto& child = snapshot.nodes[childIndex];
      if(child.role == MarkdownRenderRole::LIST_ITEM && child.listKind == MarkdownListKind::ORDERED)
      {
        child.listMarkerColumnLength = maxMarkerLength;
        FinalizeNode(child);
      }
    }
  }

private:
  std::vector<uint32_t>              blockStack;
  std::vector<uint32_t>              childCounts;
  std::vector<std::vector<uint32_t>> childIndices;
  std::vector<uint32_t>              listItemCounts;
  std::vector<ActiveSpan>            spanStack;
};

int EnterBlockCallback(MD_BLOCKTYPE type, void* detail, void* userdata)
{
  return static_cast<SnapshotBuilder*>(userdata)->EnterBlock(type, detail);
}

int LeaveBlockCallback(MD_BLOCKTYPE type, void* detail, void* userdata)
{
  return static_cast<SnapshotBuilder*>(userdata)->LeaveBlock(type);
}

int EnterSpanCallback(MD_SPANTYPE type, void* detail, void* userdata)
{
  return static_cast<SnapshotBuilder*>(userdata)->EnterSpan(type, detail);
}

int LeaveSpanCallback(MD_SPANTYPE type, void* detail, void* userdata)
{
  return static_cast<SnapshotBuilder*>(userdata)->LeaveSpan(type);
}

int TextCallback(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size, void* userdata)
{
  return static_cast<SnapshotBuilder*>(userdata)->Text(type, text, size);
}

void DebugLogCallback(const char* message, void* userdata)
{
  static_cast<SnapshotBuilder*>(userdata)->DebugLog(message);
}

MarkdownRenderSnapshot MakeFallbackSnapshot(const Dali::String& markdown, uint64_t revision, const std::string& error)
{
  MarkdownRenderSnapshot snapshot;
  snapshot.revision       = revision;
  snapshot.sourceSize     = markdown.Size();
  snapshot.parseSucceeded = false;
  snapshot.errorMessage   = error;

  MarkdownRenderNode root;
  root.index         = 0u;
  root.role          = MarkdownRenderRole::DOCUMENT;
  root.path          = "r";
  root.attributeHash = MarkdownHashBytes(&root.role, static_cast<uint32_t>(sizeof(root.role)));
  snapshot.nodes.push_back(root);

  MarkdownRenderNode paragraph;
  paragraph.index         = 1u;
  paragraph.parentIndex   = 0u;
  paragraph.depth         = 1u;
  paragraph.role          = MarkdownRenderRole::PARAGRAPH;
  paragraph.path          = "r/0";
  paragraph.text          = markdown.CStr();
  paragraph.utf32Length   = MarkdownUtf8Length(paragraph.text);
  paragraph.contentHash   = MarkdownHashString(paragraph.text);
  paragraph.attributeHash = MarkdownHashBytes(&paragraph.role, static_cast<uint32_t>(sizeof(paragraph.role)));
  paragraph.styleHash     = MARKDOWN_HASH_SEED;
  paragraph.structureHash = paragraph.attributeHash;
  paragraph.subtreeHash   = HashUint64Value(paragraph.attributeHash, paragraph.contentHash);
  root.structureHash      = HashUint64Value(static_cast<uint64_t>(paragraph.role), root.attributeHash);
  root.subtreeHash        = HashUint64Value(paragraph.subtreeHash, root.structureHash);
  snapshot.nodes[0]       = root;
  snapshot.nodes.push_back(paragraph);
  return snapshot;
}

} // namespace

MarkdownParser::Options MarkdownParser::Options::Default()
{
  Options options;
  options.flags = MD_FLAG_TABLES |
                  MD_FLAG_STRIKETHROUGH |
                  MD_FLAG_TASKLISTS |
                  MD_FLAG_NOHTML;
  return options;
}

MarkdownParser::Options MarkdownParser::Options::PlainText()
{
  Options options       = Default();
  options.plainTextMode = true;

  // Plain-text conversion recognizes raw HTML callbacks so that
  // HTML markup itself can be excluded from the result.
  options.flags &= ~static_cast<unsigned>(MD_FLAG_NOHTML);
  return options;
}

MarkdownRenderSnapshot MarkdownParser::Parse(const Dali::String& markdown, uint64_t revision, const Options& options) const
{
  SnapshotBuilder builder(revision, markdown.Size(), options.plainTextMode);

  MD_PARSER parser;
  std::memset(&parser, 0, sizeof(parser));
  parser.abi_version = 0u;
  parser.flags       = options.flags;
  parser.enter_block = EnterBlockCallback;
  parser.leave_block = LeaveBlockCallback;
  parser.enter_span  = EnterSpanCallback;
  parser.leave_span  = LeaveSpanCallback;
  parser.text        = TextCallback;
  parser.debug_log   = DebugLogCallback;

  const int ret = md_parse(markdown.CStr(), markdown.Size(), &parser, &builder);
  if(ret != 0)
  {
    return MakeFallbackSnapshot(markdown, revision, builder.snapshot.errorMessage.empty() ? "MD4C parse failed" : builder.snapshot.errorMessage);
  }

  return std::move(builder.snapshot);
}

namespace
{
bool IsAsciiTrailingWhitespace(char c)
{
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

void TrimAsciiTrailingWhitespace(std::string& text)
{
  while(!text.empty() && IsAsciiTrailingWhitespace(text.back()))
  {
    text.pop_back();
  }
}

std::size_t Utf8CharacterByteLength(const std::string& text, std::size_t byteIndex)
{
  if(byteIndex >= text.size())
  {
    return 0u;
  }

  const auto byte = static_cast<unsigned char>(text[byteIndex]);
  if((byte & 0x80u) == 0u)
  {
    return 1u;
  }
  if((byte & 0xE0u) == 0xC0u)
  {
    return std::min<std::size_t>(2u, text.size() - byteIndex);
  }
  if((byte & 0xF0u) == 0xE0u)
  {
    return std::min<std::size_t>(3u, text.size() - byteIndex);
  }
  if((byte & 0xF8u) == 0xF0u)
  {
    return std::min<std::size_t>(4u, text.size() - byteIndex);
  }
  return 1u;
}

struct PlainTextSuffixEvent
{
  uint32_t    position{0u};
  uint32_t    order{0u};
  std::string suffix;
};

/**
 * @brief Serializes a Markdown render snapshot to plain text.
 */
class PlainTextSerializer
{
public:
  explicit PlainTextSerializer(const MarkdownRenderSnapshot& snapshot)
  : mSnapshot(snapshot)
  {
    mChildrenByParent.resize(snapshot.nodes.size());
    for(const auto& node : snapshot.nodes)
    {
      if(node.parentIndex < mChildrenByParent.size())
      {
        mChildrenByParent[node.parentIndex].push_back(node.index);
      }
    }

    BuildLineBreakLookup();
  }

  Dali::String Serialize()
  {
    if(!mSnapshot.parseSucceeded)
    {
      return Dali::String();
    }

    mOutput.reserve(static_cast<std::size_t>(mSnapshot.sourceSize));

    if(!mSnapshot.nodes.empty())
    {
      if(mSnapshot.nodes[0u].role == MarkdownRenderRole::DOCUMENT)
      {
        SerializeChildren(0u);
      }
      else
      {
        for(const auto& node : mSnapshot.nodes)
        {
          if(node.parentIndex == MARKDOWN_INVALID_NODE_INDEX)
          {
            SerializeNode(node.index);
          }
        }
      }
    }

    TrimAsciiTrailingWhitespace(mOutput);
    return Dali::String(Dali::StringView(mOutput.c_str(), static_cast<uint32_t>(mOutput.size())));
  }

private:
  void BuildLineBreakLookup()
  {
    mLineBreaksByNode.resize(mSnapshot.nodes.size());

    for(const auto& event : mSnapshot.plainTextLineBreaks)
    {
      if(event.nodeIndex < mLineBreaksByNode.size())
      {
        mLineBreaksByNode[event.nodeIndex].push_back(event.position);
      }
    }

    for(auto& positions : mLineBreaksByNode)
    {
      if(!std::is_sorted(positions.begin(), positions.end()))
      {
        std::sort(positions.begin(), positions.end());
      }
    }
  }

  void SerializeChildren(uint32_t parentIndex)
  {
    if(parentIndex >= mChildrenByParent.size())
    {
      return;
    }

    for(uint32_t childIndex : mChildrenByParent[parentIndex])
    {
      SerializeNode(childIndex);
    }
  }

  void SerializeNode(uint32_t nodeIndex)
  {
    if(nodeIndex >= mSnapshot.nodes.size())
    {
      return;
    }

    const MarkdownRenderNode& node = mSnapshot.nodes[nodeIndex];
    switch(node.role)
    {
      case MarkdownRenderRole::DOCUMENT:
      case MarkdownRenderRole::LIST:
      case MarkdownRenderRole::QUOTE:
      case MarkdownRenderRole::TABLE:
      case MarkdownRenderRole::TABLE_HEAD:
      case MarkdownRenderRole::TABLE_BODY:
      case MarkdownRenderRole::TABLE_ROW:
        SerializeChildren(node.index);
        break;
      case MarkdownRenderRole::LIST_ITEM:
        SerializeListItem(node);
        break;
      case MarkdownRenderRole::CODE_BLOCK:
        AppendCodeBlock(node);
        break;
      case MarkdownRenderRole::THEMATIC_BREAK:
      case MarkdownRenderRole::RAW_HTML:
        mOutput += '\n';
        break;
      default:
        if(MarkdownIsTextRole(node.role) || node.role == MarkdownRenderRole::BLOCK_IMAGE)
        {
          AppendTextLine(node);
        }
        SerializeChildren(node.index);
        break;
    }
  }

  void SerializeListItem(const MarkdownRenderNode& node)
  {
    if(HasInlinePlainText(node) || node.taskListItem)
    {
      AppendTextLine(node);
    }
    SerializeChildren(node.index);
  }

  bool HasInlinePlainText(const MarkdownRenderNode& node) const
  {
    return !node.text.empty() || !node.linkRanges.empty() || !node.inlineObjects.empty();
  }

  const MarkdownRenderNode* ParentOf(const MarkdownRenderNode& node) const
  {
    if(node.parentIndex < mSnapshot.nodes.size())
    {
      return &mSnapshot.nodes[node.parentIndex];
    }
    return nullptr;
  }

  const MarkdownRenderNode* DirectListItemForTextLine(const MarkdownRenderNode& node) const
  {
    if(node.role == MarkdownRenderRole::LIST_ITEM)
    {
      return &node;
    }

    const MarkdownRenderNode* parent = ParentOf(node);
    if(parent && parent->role == MarkdownRenderRole::LIST_ITEM)
    {
      return parent;
    }
    return nullptr;
  }

  void AppendListPrefix(const MarkdownRenderNode& textNode)
  {
    const MarkdownRenderNode* listItem = DirectListItemForTextLine(textNode);
    if(!listItem)
    {
      return;
    }

    if(listItem->listKind == MarkdownListKind::ORDERED)
    {
      mOutput += std::to_string(listItem->listOrdinal);
      mOutput += ". ";
    }

    if(listItem->taskListItem)
    {
      mOutput += listItem->taskChecked ? "[x] " : "[ ] ";
    }
  }

  void AppendTextLine(const MarkdownRenderNode& node)
  {
    AppendListPrefix(node);
    AppendInlineText(node);
    mOutput += '\n';
  }

  void AppendCodeBlock(const MarkdownRenderNode& node)
  {
    mOutput += node.language;
    mOutput += '\n';

    std::size_t appendSize = node.text.size();
    if(appendSize > 0u && node.text[appendSize - 1u] == '\n')
    {
      --appendSize;
      if(appendSize > 0u && node.text[appendSize - 1u] == '\r')
      {
        --appendSize;
      }
    }
    else if(appendSize > 0u && node.text[appendSize - 1u] == '\r')
    {
      --appendSize;
    }

    mOutput.append(node.text, 0u, appendSize);
    mOutput += '\n';
  }

  void AppendInlineText(const MarkdownRenderNode& node)
  {
    std::vector<PlainTextSuffixEvent> suffixEvents;
    suffixEvents.reserve(node.inlineObjects.size() + node.linkRanges.size());

    uint32_t suffixOrder = 0u;
    for(const auto& object : node.inlineObjects)
    {
      const uint32_t       suffixPosition = std::min(node.utf32Length, object.position + object.length);
      PlainTextSuffixEvent event;
      event.position = suffixPosition;
      event.order    = suffixOrder++;
      event.suffix   = " [";
      event.suffix += object.sourceUrl;
      event.suffix += "]";
      suffixEvents.push_back(std::move(event));
    }
    for(const auto& link : node.linkRanges)
    {
      const uint32_t       suffixPosition = std::min(node.utf32Length, link.end);
      PlainTextSuffixEvent event;
      event.position = suffixPosition;
      event.order    = suffixOrder++;
      event.suffix   = " [";
      event.suffix += link.href;
      event.suffix += "]";
      suffixEvents.push_back(std::move(event));
    }

    std::sort(suffixEvents.begin(), suffixEvents.end(), [](const PlainTextSuffixEvent& lhs, const PlainTextSuffixEvent& rhs)
    {
      if(lhs.position != rhs.position)
      {
        return lhs.position < rhs.position;
      }
      return lhs.order < rhs.order;
    });

    std::size_t suffixEventIndex = 0u;
    auto        appendSuffixesAt = [&](uint32_t position)
    {
      while(suffixEventIndex < suffixEvents.size() && suffixEvents[suffixEventIndex].position == position)
      {
        mOutput += suffixEvents[suffixEventIndex].suffix;
        ++suffixEventIndex;
      }
    };

    const std::vector<uint32_t>* lineBreaks       = node.index < mLineBreaksByNode.size() ? &mLineBreaksByNode[node.index] : nullptr;
    std::size_t                  lineBreakIndex   = 0u;
    auto                         isPlainLineBreak = [&](uint32_t position)
    {
      if(!lineBreaks)
      {
        return false;
      }

      while(lineBreakIndex < lineBreaks->size() &&
            (*lineBreaks)[lineBreakIndex] < position)
      {
        ++lineBreakIndex;
      }
      return lineBreakIndex < lineBreaks->size() &&
             (*lineBreaks)[lineBreakIndex] == position;
    };

    uint32_t    characterIndex = 0u;
    std::size_t byteIndex      = 0u;
    while(byteIndex < node.text.size() && characterIndex < node.utf32Length)
    {
      appendSuffixesAt(characterIndex);

      const std::size_t characterBytes = Utf8CharacterByteLength(node.text, byteIndex);
      if(isPlainLineBreak(characterIndex))
      {
        mOutput += '\n';
      }
      else
      {
        mOutput.append(node.text, byteIndex, characterBytes);
      }

      byteIndex += characterBytes;
      ++characterIndex;
    }

    appendSuffixesAt(characterIndex);
    while(suffixEventIndex < suffixEvents.size())
    {
      mOutput += suffixEvents[suffixEventIndex].suffix;
      ++suffixEventIndex;
    }
  }

private:
  const MarkdownRenderSnapshot&      mSnapshot;
  std::vector<std::vector<uint32_t>> mChildrenByParent;
  std::vector<std::vector<uint32_t>> mLineBreaksByNode;
  std::string                        mOutput;
};
} // namespace

Dali::String MarkdownSnapshotToPlainText(const MarkdownRenderSnapshot& snapshot)
{
  return PlainTextSerializer(snapshot).Serialize();
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
