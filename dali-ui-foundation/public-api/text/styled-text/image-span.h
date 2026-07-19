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

#include <dali-ui-foundation/public-api/text/style/image-attributes.h>
#include <dali-ui-foundation/public-api/text/styled-text/replacement-span.h>

namespace Dali
{
namespace Ui
{
namespace Text
{

/**
 * @brief Replaces a text range with an atomic inline image box.
 *
 * Attach an ImageSpan with StyledTextBuilder::SetSpan(). The half-open range
 * [start, end) uses UTF-32 code-point indices. When the replacement is applied,
 * the reserved image box is one atomic layout unit. The underlying text remains
 * in StyledText but is not rendered.
 *
 * The canonical authoring form covers exactly one U+FFFC OBJECT REPLACEMENT
 * CHARACTER. For example:
 *
 * @code
 * StyledTextBuilder builder = StyledTextBuilder::New();
 * builder.AppendText("before ");
 * const uint32_t imageIndex = builder.GetUtf32Length();
 * builder.AppendText(ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);
 *
 * ImageAttributes attributes("icon.png", Vector2(24.0f, 24.0f));
 * builder.SetSpan(ImageSpan::New(attributes), imageIndex, imageIndex + 1u);
 * builder.AppendText(" after");
 *
 * StyledText text = builder.Build();
 * @endcode
 *
 * If the range or mandatory attributes are invalid, the replacement is not
 * applied and the underlying text remains visible.
 *
 * ImageSpan::New() snapshots the supplied ImageAttributes. Each consuming text
 * control manages its runtime image resource independently.
 *
 * @note A range containing more than one code point is supported. The exact
 *       authored range is used without grapheme-cluster or shaping-boundary
 *       normalization.
 */
class DALI_UI_API ImageSpan : public ReplacementSpan
{
public:
  /**
   * @brief Creates an uninitialized ImageSpan handle.
   */
  ImageSpan() = default;

  /**
   * @brief Creates an ImageSpan that snapshots @p attributes.
   *
   * Later changes to @p attributes do not affect the new span. Invalid or
   * incomplete attributes still create an initialized handle, but the
   * replacement is not applied when attached.
   *
   * @param[in] attributes The image attributes to snapshot
   * @return A new ImageSpan
   */
  static ImageSpan New(const ImageAttributes& attributes);

  /**
   * @brief Downcasts a handle to ImageSpan.
   *
   * @param[in] handle The handle to downcast
   * @return An initialized ImageSpan on success, otherwise an empty handle
   */
  static ImageSpan DownCast(BaseHandle handle);

  /**
   * @brief Retrieves a copy of the snapshotted image attributes.
   *
   * Changing the returned value does not affect this ImageSpan. An
   * uninitialized handle returns a default-constructed ImageAttributes value.
   *
   * @return An independent copy of the image attributes
   */
  ImageAttributes GetImageAttributes() const;

private:
  /**
   * @brief Creates an ImageSpan handle from its implementation.
   *
   * @param[in] impl The implementation object
   */
  explicit DALI_INTERNAL ImageSpan(BaseObject* impl);
};

} // namespace Text
} // namespace Ui
} // namespace Dali
