#ifndef DALI_UI_EDITABLE_STYLED_TEXT_DATA_H
#define DALI_UI_EDITABLE_STYLED_TEXT_DATA_H

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
 */

// EXTERNAL INCLUDES
#include <cstdint>
#include <string>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/text/styled-text/span.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text.h>

namespace Dali::Ui::Text
{
/**
 * @brief Stores authored span attachments for an editable StyledText source.
 */
class EditableStyledTextData
{
public:
  /**
   * @brief Replaces the authored snapshot.
   *
   * @param[in] styledText The authored snapshot.
   */
  void Set(const StyledText& styledText);

  /**
   * @brief Applies a logical text edit to all attachment ranges.
   *
   * @param[in] start The edit start boundary.
   * @param[in] removedLength The removed logical length.
   * @param[in] insertedLength The inserted logical length.
   */
  void ApplyEdit(uint32_t start, uint32_t removedLength, uint32_t insertedLength);

  /**
   * @brief Builds the current immutable snapshot.
   *
   * @param[in] text The current UTF-8 logical text.
   * @return The current StyledText snapshot.
   */
  StyledText Build(const std::string& text) const;

private:
  struct Attachment
  {
    Span     span;
    uint32_t start{0u};
    uint32_t end{0u};
    bool     replacement{false};
  };

  std::vector<Attachment> mAttachments;
};

} // namespace Dali::Ui::Text

#endif // DALI_UI_EDITABLE_STYLED_TEXT_DATA_H
