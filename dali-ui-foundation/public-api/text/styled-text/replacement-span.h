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
 */

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/text/styled-text/span.h>

namespace Dali
{
namespace Ui
{
namespace Text
{

/**
 * @brief Base handle for built-in spans that replace a text range with an
 *        atomic inline object.
 *
 * Attach a concrete subtype with StyledTextBuilder::SetSpan(). The half-open
 * range [start, end) uses UTF-32 code-point indices. When the replacement is
 * applied, the covered logical text remains in StyledText but is represented
 * by one atomic layout unit.
 *
 * Applications create a supported concrete subtype, such as ImageSpan. Custom
 * measurement and drawing hooks are not provided.
 */
class DALI_UI_API ReplacementSpan : public Span
{
public:
  /**
   * @brief The UTF-8 encoded U+FFFC OBJECT REPLACEMENT CHARACTER used for
   *        canonical one-character replacement authoring.
   *
   * Pass this value to StyledTextBuilder::AppendText().
   */
  inline static constexpr char OBJECT_REPLACEMENT_CHARACTER[] = "\xEF\xBF\xBC";

  /**
   * @brief Creates an uninitialized ReplacementSpan handle.
   */
  ReplacementSpan() = default;

  /**
   * @brief Downcasts a handle to ReplacementSpan.
   *
   * @param[in] handle The handle to downcast
   * @return An initialized ReplacementSpan on success, otherwise an empty handle
   */
  static ReplacementSpan DownCast(BaseHandle handle);

protected:
  /**
   * @brief Creates a ReplacementSpan handle from its implementation.
   *
   * @param[in] impl The implementation object
   */
  explicit DALI_INTERNAL ReplacementSpan(BaseObject* impl);
};

} // namespace Text
} // namespace Ui
} // namespace Dali
