#ifndef DALI_UI_IMAGE_URL_H
#define DALI_UI_IMAGE_URL_H

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
 */

// EXTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/encoded-image-buffer.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{
namespace Internal DALI_INTERNAL
{
class ImageUrl;
}
/**
 * @brief ImageUrl can wrap an external buffer or keep a cached image resource available.
 *
 * An instance of ImageUrl can be created from ImageUrlUtils::GenerateUrl().
 * Application can get url from ImageUrl.
 * When application does not use this anymore, the destructor of the ImageUrl is called.
 * At this time, the external buffer or cache pin is removed.
 * @note Visuals also hold resource references. A resource in active use is released after the visual is deleted.
 * @note Path-based cache pinning supports static raster, N-patch, SVG, and TVG resources only.
 * @note Fast-track uploading is disabled while a path-based ImageUrl is alive,
 *       so the resource can use its shared cache.
 * @note This handle must be created, accessed, and released on the main event thread,
 *       consistently with other Dali handles.
 */
class DALI_UI_API ImageUrl : public BaseHandle
{
public:
  /**
   * Default Constructor.
   * Resulting URL is not valid
   */
  ImageUrl();

  /**
   * Destructor
   */
  ~ImageUrl();

  /**
   * @brief Create an initialized ImageUrl.
   *
   * @param[in] texture The texture url is got from external buffer.
   * @param[in] preMultiplied Whether this external texture preMultiplied or not. Default as false.
   * @return A handle to a newly allocated Dali resource.
   */
  static ImageUrl New(Texture& texture, bool preMultiplied = false);

  /**
   * @brief Create an initialized ImageUrl.
   *
   * @param[in] encodedImageBuffer The encoded image buffer url is got from external buffer.
   * @return A handle to a newly allocated Dali resource.
   */
  static ImageUrl New(const EncodedImageBuffer& encodedImageBuffer);

  /**
   * @brief Create an initialized ImageUrl that keeps the most recently used resource cached for the URL.
   *
   * The returned URL can be assigned to an image visual. While this handle remains alive,
   * recreating a visual with the same image URL can reuse the cached resource.
   * GetUrl() returns the original local or remote URL without exposing an internal texture ID.
   * Static raster, N-patch, SVG, and TVG resources are pinned by their existing cache owners.
   * GIF, WEBP, and JSON animated resources do not support cache pinning. For those types the returned URL
   * remains usable by an image visual, and a warning is logged when this method is called.
   *
   * @param[in] url A local or remote image URL
   * @return A handle that keeps the image resource cached
   */
  static ImageUrl New(const Dali::String& url);

  /**
   * @brief Downcast an Object handle to ImageUrl handle.
   *
   * If handle points to a ImageUrl object the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle to An object.
   * @return handle to a ImageUrl object or an uninitialized handle.
   */
  static ImageUrl DownCast(BaseHandle handle);

  /**
   * Copy constructor
   * @param[in] url The url to copy
   */
  ImageUrl(const ImageUrl& url);

  /**
   * Assignment operator
   * @param[in] url The url to copy
   */
  ImageUrl& operator=(const ImageUrl& url);

  /**
   * @brief Move constructor.
   * @param[in] rhs A reference to the moved handle
   */
  ImageUrl(ImageUrl&& rhs) noexcept;

  /**
   * @brief Move assignment operator.
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this handle
   */
  ImageUrl& operator=(ImageUrl&& rhs) noexcept;

  /**
   * Get the url
   * @return Returns url's string
   */
  const Dali::String& GetUrl() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used by New() methods.
   *
   * @param[in] internal A pointer to a newly allocated Dali resource.
   */
  explicit DALI_INTERNAL ImageUrl(Ui::Internal::ImageUrl* internal);
  /// @endcond
};

} // namespace Ui

} // namespace Dali

#endif /* DALI_UI_IMAGE_URL_H */
