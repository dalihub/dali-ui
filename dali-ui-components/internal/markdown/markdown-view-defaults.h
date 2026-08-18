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

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace MarkdownViewDefaults
{

// Layout
constexpr float VIEW_PADDING  = 10.0f;
constexpr float BLOCK_SPACING = 20.0f;

// Typography
constexpr char  TEXT_FONT_FAMILY[]         = "SamsungOneUI_400";
constexpr char  HEADING_FONT_FAMILY[]      = "SamsungOneUI_700";
constexpr char  CODE_FONT_FAMILY[]         = "SamsungOneUI_300";
constexpr float TEXT_FONT_SIZE             = 20.0f;
constexpr float HEADING1_FONT_SIZE         = 28.0f;
constexpr float HEADING2_FONT_SIZE         = 24.0f;
constexpr float HEADING3_FONT_SIZE         = 20.0f;
constexpr float HEADING4_FONT_SIZE         = 16.0f;
constexpr float HEADING5_FONT_SIZE         = 12.0f;
constexpr float HEADING6_FONT_SIZE         = 10.0f;
constexpr float CODE_BLOCK_FONT_SIZE       = 20.0f;
constexpr float CODE_BLOCK_TITLE_FONT_SIZE = 16.0f;
constexpr float BODY_LINE_HEIGHT_RATIO     = 1.6f;
constexpr float DEFAULT_BODY_LINE_HEIGHT   = TEXT_FONT_SIZE * BODY_LINE_HEIGHT_RATIO;
constexpr float STRIKETHROUGH_THICKNESS    = 2.0f;

// Default colors
constexpr uint32_t TEXT_COLOR                        = 0x000000u;
constexpr uint32_t HEADING_TEXT_COLOR                = TEXT_COLOR;
constexpr uint32_t QUOTE_TEXT_COLOR                  = 0x2F2F2Fu;
constexpr uint32_t CODE_TEXT_COLOR                   = 0x121212u;
constexpr uint32_t CODE_BLOCK_TITLE_TEXT_COLOR       = 0x454545u;
constexpr uint32_t INLINE_CODE_BACKGROUND_COLOR      = 0x000000u;
constexpr float    INLINE_CODE_BACKGROUND_ALPHA      = 0.0f;
constexpr uint32_t CODE_BLOCK_BACKGROUND_COLOR       = 0xCCCCCCu;
constexpr float    CODE_BLOCK_BACKGROUND_ALPHA       = 0x33u / 255.0f;
constexpr uint32_t CODE_BLOCK_TITLE_BACKGROUND_COLOR = 0xCCCCCCu;
constexpr float    CODE_BLOCK_TITLE_BACKGROUND_ALPHA = 0x55u / 255.0f;
constexpr uint32_t QUOTE_BAR_COLOR                   = 0xDFDFDFu;
constexpr uint32_t THEMATIC_BREAK_COLOR              = 0xDFDFDFu;
constexpr uint32_t TABLE_RULE_COLOR                  = 0x000000u;

// Component geometry
constexpr float LIST_ITEM_MARGIN_BOTTOM = 10.0f;
constexpr float QUOTE_PADDING           = 10.0f;
constexpr float QUOTE_BAR_WIDTH         = 6.0f;
constexpr float QUOTE_BAR_CORNER_RADIUS = 3.0f;
constexpr float CODE_PADDING            = 10.0f;
constexpr float CODE_CORNER_RADIUS      = 12.0f;
constexpr float TABLE_CELL_PADDING      = 5.0f;
constexpr float TABLE_RULE_HEIGHT       = 1.0f;
constexpr float THEMATIC_BREAK_HEIGHT   = 1.0f;

} // namespace MarkdownViewDefaults
} // namespace Internal
} // namespace Ui
} // namespace Dali
