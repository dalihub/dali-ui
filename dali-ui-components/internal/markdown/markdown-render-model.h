#pragma once

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
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

namespace Dali
{
namespace Ui
{
namespace Internal
{

constexpr uint32_t MARKDOWN_INVALID_NODE_INDEX = std::numeric_limits<uint32_t>::max();

/**
 * @brief Defines the default seed used by Markdown render hashing.
 */
constexpr uint64_t MARKDOWN_HASH_SEED = 14695981039346656037ull;

/**
 * @brief Identifies the semantic role of a parsed Markdown render node.
 */
enum class MarkdownRenderRole : uint8_t
{
  DOCUMENT,
  PARAGRAPH,
  LIST_ITEM_PARAGRAPH,
  QUOTE_PARAGRAPH,
  TABLE_CELL_PARAGRAPH,
  HEADING,
  CODE_BLOCK,
  BLOCK_IMAGE,
  LIST,
  LIST_ITEM,
  QUOTE,
  TABLE,
  TABLE_HEAD,
  TABLE_BODY,
  TABLE_ROW,
  TABLE_CELL,
  THEMATIC_BREAK,
  RAW_HTML
};

/**
 * @brief Identifies the list style associated with a Markdown list node.
 */
enum class MarkdownListKind : uint8_t
{
  NONE,
  UNORDERED,
  ORDERED
};

/**
 * @brief Identifies the text alignment requested by a Markdown table cell.
 */
enum class MarkdownTableAlignment : uint8_t
{
  DEFAULT,
  LEFT,
  CENTER,
  RIGHT
};

/**
 * @brief Identifies the kind of inline object embedded in Markdown text.
 */
enum class MarkdownInlineObjectType : uint8_t
{
  IMAGE
};

/**
 * @brief Defines text style flags applied to ranges of Markdown text.
 */
enum MarkdownTextStyleFlag : uint32_t
{
  MARKDOWN_TEXT_STYLE_NONE          = 0u,
  MARKDOWN_TEXT_STYLE_EMPHASIS      = 1u << 0u,
  MARKDOWN_TEXT_STYLE_STRONG        = 1u << 1u,
  MARKDOWN_TEXT_STYLE_INLINE_CODE   = 1u << 2u,
  MARKDOWN_TEXT_STYLE_STRIKETHROUGH = 1u << 3u,
  MARKDOWN_TEXT_STYLE_UNDERLINE     = 1u << 4u
};

/**
 * @brief Stores a styled range within a Markdown text node.
 *
 * The range is expressed as UTF-32 character indices.
 */
struct MarkdownTextStyleRun
{
  uint32_t start{0u};
  uint32_t end{0u};
  uint32_t flags{MARKDOWN_TEXT_STYLE_NONE};
};

/**
 * @brief Stores a link range within a Markdown text node.
 *
 * The range is expressed as UTF-32 character indices.
 */
struct MarkdownLinkRange
{
  uint32_t    start{0u};
  uint32_t    end{0u};
  std::string href;
  std::string title;
  bool        isAutolink{false};
};

/**
 * @brief Stores an inline object range within a Markdown text node.
 *
 * The position and length are expressed as UTF-32 character indices.
 */
struct MarkdownInlineObject
{
  MarkdownInlineObjectType type{MarkdownInlineObjectType::IMAGE};
  uint32_t                 position{0u};
  uint32_t                 length{0u};
  std::string              sourceUrl;
  std::string              title;
  std::string              altText;
};

/**
 * @brief Describes a classified text update between two render nodes.
 */
struct MarkdownTextUpdate
{
  enum class Type : uint8_t
  {
    UNCHANGED,
    APPEND,
    REPLACE_TAIL,
    REPLACE_ALL
  };

  Type     type{Type::REPLACE_ALL};
  uint32_t unchangedPrefixLength{0u};
  uint32_t removedCharacterCount{0u};
};

/**
 * @brief Stores the parsed content and rendering attributes of a Markdown node.
 */
struct MarkdownRenderNode
{
  uint32_t           index{MARKDOWN_INVALID_NODE_INDEX};
  uint32_t           parentIndex{MARKDOWN_INVALID_NODE_INDEX};
  uint32_t           depth{0u};
  MarkdownRenderRole role{MarkdownRenderRole::PARAGRAPH};
  std::string        path;

  std::string text;
  uint32_t    utf32Length{0u};

  uint32_t headingLevel{0u};

  MarkdownListKind listKind{MarkdownListKind::NONE};
  uint32_t         listStart{1u};
  uint32_t         listOrdinal{0u};
  uint32_t         listDepth{0u};
  uint32_t         listMarkerColumnLength{0u};
  bool             tightList{false};
  bool             taskListItem{false};
  bool             taskChecked{false};

  MarkdownTableAlignment tableAlignment{MarkdownTableAlignment::DEFAULT};

  std::string language;
  std::string sourceUrl;
  std::string title;
  std::string altText;

  std::vector<MarkdownTextStyleRun> styleRuns;
  std::vector<MarkdownLinkRange>    linkRanges;
  std::vector<MarkdownInlineObject> inlineObjects;

  uint64_t contentHash{0u};
  uint64_t attributeHash{0u};
  uint64_t styleHash{0u};
  uint64_t structureHash{0u};
  uint64_t subtreeHash{0u};
};

/**
 * @brief Identifies a line-break position used during plain-text conversion.
 *
 * The position is expressed as a UTF-32 index within the referenced node.
 */
struct MarkdownPlainTextLineBreak
{
  uint32_t nodeIndex{MARKDOWN_INVALID_NODE_INDEX};
  uint32_t position{0u};
};

/**
 * @brief Stores the parsed Markdown document and its render metadata.
 */
struct MarkdownRenderSnapshot
{
  uint64_t                                revision{0u};
  uint32_t                                sourceSize{0u};
  bool                                    parseSucceeded{true};
  std::string                             errorMessage;
  std::vector<MarkdownRenderNode>         nodes;
  std::vector<MarkdownPlainTextLineBreak> plainTextLineBreaks;
};

/**
 * @brief Hashes a block of bytes using the Markdown render hash seed.
 *
 * @param[in] data The byte data to hash.
 * @param[in] size The number of bytes to hash.
 * @param[in] seed The initial hash seed.
 * @return The calculated hash value.
 */
uint64_t MarkdownHashBytes(const void* data, uint32_t size, uint64_t seed = MARKDOWN_HASH_SEED);

/**
 * @brief Hashes a string using the Markdown render hash seed.
 *
 * @param[in] value The string to hash.
 * @param[in] seed The initial hash seed.
 * @return The calculated hash value.
 */
uint64_t MarkdownHashString(const std::string& value, uint64_t seed = MARKDOWN_HASH_SEED);

/**
 * @brief Counts UTF-32 characters represented by a UTF-8 string.
 *
 * @param[in] text The UTF-8 text to count.
 * @return The number of UTF-32 characters.
 */
uint32_t MarkdownUtf8Length(const std::string& text);

/**
 * @brief Checks whether a render role carries text content.
 *
 * @param[in] role The render role.
 * @return @c true if the role carries text content.
 */
bool MarkdownIsTextRole(MarkdownRenderRole role);

/**
 * @brief Checks whether a render role can contain child nodes.
 *
 * @param[in] role The render role.
 * @return @c true if the role can contain child nodes.
 */
bool MarkdownIsContainerRole(MarkdownRenderRole role);

/**
 * @brief Checks whether a render role can create a view component.
 *
 * @param[in] role The render role.
 * @return @c true if the role can create a view component.
 */
bool MarkdownCanMaterializeRole(MarkdownRenderRole role);

/**
 * @brief Gets the name for a render role.
 *
 * @param[in] role The render role.
 * @return The role name.
 */
const char* MarkdownRenderRoleName(MarkdownRenderRole role);

} // namespace Internal
} // namespace Ui
} // namespace Dali
