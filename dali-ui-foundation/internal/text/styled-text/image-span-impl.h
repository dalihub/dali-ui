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

#include <dali-ui-foundation/internal/text/styled-text/replacement-span-impl.h>
#include <dali-ui-foundation/public-api/text/style/image-attributes.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace Text
{
/**
 * @brief Implements an image span.
 */
class ImageSpan : public ReplacementSpan
{
public:
  /**
   * @brief Creates an image span implementation.
   *
   * @param[in] attributes The image attributes to copy.
   */
  explicit ImageSpan(const Dali::Ui::Text::ImageAttributes& attributes);

  /**
   * @brief Destroys the image span implementation.
   */
  ~ImageSpan() override;

  /**
   * @brief Gets the stored image attributes.
   *
   * @return The image attributes.
   */
  const Dali::Ui::Text::ImageAttributes& GetImageAttributes() const;

private:
  Dali::Ui::Text::ImageAttributes mAttributes;
};
} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali
