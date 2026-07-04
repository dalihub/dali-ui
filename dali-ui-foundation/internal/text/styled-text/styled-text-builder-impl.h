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
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/object/base-object.h>
#include <cstdint>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/styled-text/styled-text-impl.h>
#include <dali-ui-foundation/public-api/text/styled-text/span.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace Text
{

/**
 * @brief Mutable builder implementation for StyledText snapshots.
 */
class StyledTextBuilder : public BaseObject
{
public:
  explicit StyledTextBuilder(const Dali::String& text);
  explicit StyledTextBuilder(const StyledText& styledText);
  ~StyledTextBuilder() override;

  void SetText(const Dali::String& text);
  void AppendText(const Dali::String& text);
  bool SetSpan(const Dali::Ui::Text::Span& span, uint32_t startIndex, uint32_t endIndex);
  bool RemoveSpan(const Dali::Ui::Text::Span& span);
  bool RemoveSpanAt(uint32_t index);
  void ClearSpans();

  const Dali::String&  GetText() const;
  uint32_t             GetSpanCount() const;
  Dali::Ui::Text::Span GetSpanAt(uint32_t index) const;
  uint32_t             GetSpanStartIndexAt(uint32_t index) const;
  uint32_t             GetSpanEndIndexAt(uint32_t index) const;

  const std::vector<SpanAttachment>& GetAttachments() const;

private:
  StyledTextBuilder(const StyledTextBuilder&)            = delete;
  StyledTextBuilder(StyledTextBuilder&&)                 = delete;
  StyledTextBuilder& operator=(const StyledTextBuilder&) = delete;
  StyledTextBuilder& operator=(StyledTextBuilder&&)      = delete;

private:
  Dali::String                mText;
  std::vector<SpanAttachment> mAttachments;
  uint32_t                    mNextInsertionOrder{0u};
};

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali
