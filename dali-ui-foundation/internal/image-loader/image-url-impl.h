#ifndef DALI_UI_INTERNAL_IMAGE_URL_H
#define DALI_UI_INTERNAL_IMAGE_URL_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/adaptor-framework/encoded-image-buffer.h>
#include <dali/public-api/object/base-object.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/image-loader/image-url.h>

namespace Dali
{
namespace UI
{
namespace Internal
{
class ImageUrl;
using ImageUrlPtr = IntrusivePtr<ImageUrl>;

class ImageUrl : public BaseObject
{
public:
  /**
   * @brief Constructor.
   */
  ImageUrl(Texture& texture, bool preMultiplied);

  /**
   * @brief Constructor.
   */
  ImageUrl(const EncodedImageBuffer& encodedImageBuffer);

  /**
   * @copydoc Dali::UI::ImageUrl::New
   */
  static ImageUrlPtr New(Texture& texture, bool preMultiplied);

  /**
   * @copydoc Dali::UI::ImageUrl::New
   */
  static ImageUrlPtr New(const EncodedImageBuffer& encodedImageBuffer);

  /**
   * @copydoc Dali::UI::ImageUrl::GetUrl
   */
  const std::string& GetUrl() const;

protected:
  /**
   * @brief A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ImageUrl();

private:
  // Undefined
  ImageUrl(const ImageUrl&);

  // Undefined
  ImageUrl& operator=(const ImageUrl& rhs);

private:
  std::string mUrl;
};

} // namespace Internal
} // namespace UI

// Helpers for public-api forwarding methods

inline UI::Internal::ImageUrl& GetImpl(Dali::UI::ImageUrl& imageUrl)
{
  DALI_ASSERT_ALWAYS(imageUrl && "ImageUrl handle is empty");

  BaseObject& handle = imageUrl.GetBaseObject();

  return static_cast<UI::Internal::ImageUrl&>(handle);
}

inline const UI::Internal::ImageUrl& GetImpl(const Dali::UI::ImageUrl& imageUrl)
{
  DALI_ASSERT_ALWAYS(imageUrl && "ImageUrl handle is empty");

  const BaseObject& handle = imageUrl.GetBaseObject();

  return static_cast<const UI::Internal::ImageUrl&>(handle);
}

} // End of namespace Dali

#endif // DALI_UI_INTERNAL_IMAGE_URL_H
