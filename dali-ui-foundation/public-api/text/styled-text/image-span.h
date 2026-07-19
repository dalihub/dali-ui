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
 * @brief Replaces a UTF-32 code-point range with an atomic inline image box.
 *
 * Attach an ImageSpan with StyledTextBuilder::SetSpan(). The exact half-open
 * range [start, end) is measured in UTF-32 code-point indices. When the
 * replacement is applied, its reserved image box is one atomic layout unit
 * and the covered code points are not rendered.
 *
 * The canonical authoring form covers exactly one U+FFFC OBJECT REPLACEMENT
 * CHARACTER. For example:
 *
 * @code
 * StyledTextBuilder builder = StyledTextBuilder::New();
 * builder.AppendText("before ");
 * const uint32_t imageIndex = builder.GetUtf32Length();
 * builder.AppendText("\uFFFC");
 * builder.AppendText(" after");
 *
 * ImageAttributes attributes("icon.png", Vector2(24.0f, 24.0f));
 * ImageSpan imageSpan = ImageSpan::New(attributes);
 * builder.SetSpan(imageSpan, imageIndex, imageIndex + 1u);
 * StyledText text = builder.Build();
 * @endcode
 *
 * This U+FFFC form reuses the source text during processing and does not need
 * character-index mapping tables. Replacing another single code point keeps
 * identity indices but creates a neutralized processing copy. Replacing a
 * multi-code-point range creates compact processing text and logical-to-render
 * index maps. All forms use the same shaping, layout, and image rendering path.
 *
 * The source logical text remains in StyledText. Removing the span reveals
 * that text. If mandatory attributes or the attached range are invalid, the
 * replacement is not applied and the underlying text remains visible.
 *
 * ImageSpan::New() snapshots the supplied ImageAttributes. An ImageSpan does
 * not store a rendered image resource. Each supporting text control creates
 * and manages its own rendered image instance, so sharing the same ImageSpan
 * or StyledText between controls does not share mutable rendering state.
 *
 * @note A range containing more than one code point is supported. The exact
 *       authored range is used without grapheme-cluster or shaping-boundary
 *       normalization.
 * @note ImageSpan content supports END ellipsis. START and MIDDLE ellipsis use
 *       a replacement-preserving clipping fallback.
 * @note A Label does not run marquee animation while its StyledText contains
 *       an applicable ImageSpan.
 */
class DALI_UI_API ImageSpan : public ReplacementSpan
{
public:
  /**
   * @brief Creates an uninitialized ImageSpan handle.
   */
  ImageSpan() = default;

  /**
   * @brief Creates an ImageSpan that deep-snapshots @p attributes.
   *
   * Later changes to @p attributes do not affect the new ImageSpan. Incomplete
   * or invalid attributes still produce an initialized handle. When attached,
   * such a span is not applied and its underlying text remains visible. Invalid
   * values include an empty source, a reserved size with a non-finite or
   * non-positive component, and a non-finite vertical offset.
   *
   * @param[in] attributes The image attributes to snapshot
   * @return A new ImageSpan containing an independent snapshot
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
