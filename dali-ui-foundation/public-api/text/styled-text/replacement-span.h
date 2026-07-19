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

#include <dali-ui-foundation/public-api/text/styled-text/span.h>

namespace Dali
{
namespace Ui
{
namespace Text
{

/**
 * @brief Marker base handle for built-in replacement spans.
 *
 * Attach a concrete subtype with StyledTextBuilder::SetSpan(), using an exact
 * half-open range [start, end) measured in UTF-32 code-point indices. An
 * accepted replacement is represented by one atomic layout unit. The covered
 * code points remain part of the StyledText logical text but are not rendered
 * while the replacement is applied.
 *
 * This class does not expose public construction, measurement, drawing or
 * custom replacement hooks for application-defined subclasses. Applications
 * create a concrete built-in replacement, such as ImageSpan, through that
 * type's factory.
 */
class DALI_UI_API ReplacementSpan : public Span
{
public:
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
