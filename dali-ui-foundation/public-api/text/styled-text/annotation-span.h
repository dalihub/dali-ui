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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/text/styled-text/span.h>

namespace Dali
{
namespace Ui
{
namespace Text
{

/**
 * @brief Span payload that marks a text range with annotation metadata.
 *
 * AnnotationSpan stores one annotation key/value pair. The text range is owned
 * by the styled-text attachment that uses the span. AnnotationSpan is a Span
 * subclass, so it can be attached, removed, pushed, and popped through the same
 * span APIs as visual style spans.
 *
 * When converted from DALi markup, each attribute in an annotation markup tag is
 * represented as a separate AnnotationSpan over the same text range.
 */
class DALI_UI_API AnnotationSpan : public Span
{
public:
  /**
   * @brief Creates an uninitialized AnnotationSpan handle.
   */
  AnnotationSpan() = default;

  /**
   * @brief Creates an AnnotationSpan with the given annotation key/value pair.
   *
   * @param[in] key The annotation key
   * @param[in] value The annotation payload
   * @return A new AnnotationSpan
   */
  static AnnotationSpan New(const Dali::String& key, const Dali::String& value);

  /**
   * @brief Downcasts a handle to AnnotationSpan.
   *
   * @param[in] handle The handle to downcast
   * @return An initialized AnnotationSpan on success, otherwise empty
   */
  static AnnotationSpan DownCast(BaseHandle handle);

  /**
   * @brief Gets the annotation key.
   *
   * @return The annotation key
   */
  Dali::String GetKey() const;

  /**
   * @brief Gets the annotation value.
   *
   * @return The annotation value
   */
  Dali::String GetValue() const;

private:
  /**
   * @brief Creates an AnnotationSpan handle from its implementation.
   *
   * @param[in] impl The implementation object
   */
  explicit DALI_INTERNAL AnnotationSpan(BaseObject* impl);
};

} // namespace Text
} // namespace Ui
} // namespace Dali
