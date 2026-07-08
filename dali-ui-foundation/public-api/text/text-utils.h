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
#include <dali/public-api/common/dali-string-view.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{

namespace Text
{

/**
 * @brief Gets the UTF-32 length produced by decoding UTF-8 text.
 *
 * The returned value is the number of elements in the decoded UTF-32 sequence.
 * This is the same unit used by StyledText span range indexes.
 *
 * This is not the UTF-8 byte size returned by std::string::size(),
 * Dali::String::Size(), or Dali::StringView::Size().
 *
 * Invalid or truncated UTF-8 is handled defensively without asserting. This
 * helper is intended for range calculation and is not a UTF-8 validator.
 *
 * @param[in] utf8Text The UTF-8 text view
 * @return The decoded UTF-32 length
 */
DALI_UI_API uint32_t Utf8ToUtf32Length(Dali::StringView utf8Text);

/**
 * @brief Converts a UTF-8 index to a decoded UTF-32 index.
 *
 * The UTF-8 index is a byte offset in utf8Text. The UTF-32 index is an index
 * in the decoded UTF-32 sequence.
 *
 * @param[in] utf8Text The UTF-8 text view
 * @param[in] utf8Index The UTF-8 byte offset to convert
 * @param[out] utf32Index The converted UTF-32 index
 * @return true if utf8Index is in range and is not inside a multi-byte UTF-8
 *         sequence, false otherwise. If false is returned, utf32Index is not
 *         modified.
 */
DALI_UI_API bool Utf8ToUtf32Index(
  Dali::StringView utf8Text,
  uint32_t         utf8Index,
  uint32_t&        utf32Index);

/**
 * @brief Converts a UTF-8 range to a decoded UTF-32 range.
 *
 * The input range is [utf8StartIndex, utf8EndIndex) in UTF-8 byte offsets.
 * The output range is [utf32StartIndex, utf32EndIndex) in the decoded UTF-32
 * sequence.
 *
 * The converted output range can be passed to StyledTextBuilder::SetSpan().
 *
 * @param[in] utf8Text The UTF-8 text view
 * @param[in] utf8StartIndex The inclusive UTF-8 byte start offset
 * @param[in] utf8EndIndex The exclusive UTF-8 byte end offset
 * @param[out] utf32StartIndex The converted UTF-32 start index
 * @param[out] utf32EndIndex The converted UTF-32 end index
 * @return true if the input range is valid and neither UTF-8 index is inside a
 *         multi-byte UTF-8 sequence, false otherwise. If false is returned,
 *         output indexes are not modified.
 */
DALI_UI_API bool Utf8ToUtf32Range(
  Dali::StringView utf8Text,
  uint32_t         utf8StartIndex,
  uint32_t         utf8EndIndex,
  uint32_t&        utf32StartIndex,
  uint32_t&        utf32EndIndex);

/**
 * @brief Converts a decoded UTF-32 index to a UTF-8 index.
 *
 * The UTF-32 index is an index in the decoded UTF-32 sequence of utf8Text.
 * The UTF-8 index is a byte offset in utf8Text.
 *
 * @param[in] utf8Text The UTF-8 text view
 * @param[in] utf32Index The UTF-32 index to convert
 * @param[out] utf8Index The converted UTF-8 byte offset
 * @return true if utf32Index is in range, false otherwise. If false is returned,
 *         utf8Index is not modified.
 */
DALI_UI_API bool Utf32ToUtf8Index(
  Dali::StringView utf8Text,
  uint32_t         utf32Index,
  uint32_t&        utf8Index);

/**
 * @brief Converts a decoded UTF-32 range to a UTF-8 range.
 *
 * The input range is [utf32StartIndex, utf32EndIndex) in the decoded UTF-32
 * sequence. The output range is [utf8StartIndex, utf8EndIndex) in UTF-8 byte
 * offsets.
 *
 * @param[in] utf8Text The UTF-8 text view
 * @param[in] utf32StartIndex The inclusive UTF-32 start index
 * @param[in] utf32EndIndex The exclusive UTF-32 end index
 * @param[out] utf8StartIndex The converted UTF-8 byte start offset
 * @param[out] utf8EndIndex The converted UTF-8 byte end offset
 * @return true if the UTF-32 range is valid, false otherwise. If false is
 *         returned, output indexes are not modified.
 */
DALI_UI_API bool Utf32ToUtf8Range(
  Dali::StringView utf8Text,
  uint32_t         utf32StartIndex,
  uint32_t         utf32EndIndex,
  uint32_t&        utf8StartIndex,
  uint32_t&        utf8EndIndex);

} // namespace Text
} // namespace Ui
} // namespace Dali
