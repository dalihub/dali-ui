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

// CLASS HEADER
#include <dali-ui-foundation/public-api/image-loader/image-url-utils.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/image-loader/texture-manager.h>
#include <dali-ui-foundation/public-api/image-loader/image-url.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/rendering/frame-buffer-devel.h>
#include <dali/integration-api/texture-integ.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace ImageUrlUtils
{
namespace
{
Dali::Ui::ImageUrl GeneratePixelDataUrl(Dali::PixelData pixelData, bool preMultiplied)
{
  Texture texture =
    Texture::New(TextureType::TEXTURE_2D, pixelData.GetPixelFormat(), pixelData.GetWidth(), pixelData.GetHeight());
  // The url must exist before the upload tags the content, so Upload() has not recorded
  // the flag on the texture yet when the url snapshots it.
  Dali::Integration::SetTextureAlphaPreMultiplied(texture, preMultiplied);
  Dali::Ui::ImageUrl imageUrl = Dali::Ui::ImageUrl::New(texture);
  if(DALI_UNLIKELY(Dali::Integration::IsGpuMemoryProfileEnabled()))
  {
    Dali::Integration::TextureUploadWithContent(texture, pixelData, imageUrl.GetUrl(), Dali::Integration::TextureContextTypeHint::EXTERNAL_IMAGE);
  }
  else
  {
    texture.Upload(pixelData);
  }
  return imageUrl;
}
} // unnamed namespace

Dali::Ui::ImageUrl GenerateUrl(Dali::FrameBuffer frameBuffer, Pixel::Format pixelFormat, uint32_t width,
                               uint32_t height)
{
  Texture texture = Texture::New(Dali::TextureType::TEXTURE_2D, pixelFormat, width, height);
  frameBuffer.AttachColorTexture(texture, 0u, 0u);
  // DALi renders pre-multiplied output, so a frame buffer result is pre-multiplied.
  Dali::Integration::SetTextureAlphaPreMultiplied(texture, true);
  Dali::Ui::ImageUrl imageUrl = Dali::Ui::ImageUrl::New(texture);
  return imageUrl;
}

Dali::Ui::ImageUrl GenerateUrl(Dali::FrameBuffer frameBuffer, uint8_t index)
{
  Texture texture = Dali::DevelFrameBuffer::GetColorTexture(frameBuffer, index);
  // DALi renders pre-multiplied output, so a frame buffer result is pre-multiplied.
  if(texture)
  {
    Dali::Integration::SetTextureAlphaPreMultiplied(texture, true);
  }
  Dali::Ui::ImageUrl imageUrl = Dali::Ui::ImageUrl::New(texture);
  return imageUrl;
}

Dali::Ui::ImageUrl GenerateUrl(Dali::PixelData pixelData)
{
  return GeneratePixelDataUrl(pixelData, pixelData.IsAlphaPreMultiplied());
}

Dali::Ui::ImageUrl GenerateUrl(Dali::PixelData pixelData, bool preMultiplied)
{
  return GeneratePixelDataUrl(pixelData, preMultiplied || pixelData.IsAlphaPreMultiplied());
}

Dali::Ui::ImageUrl GenerateUrl(Dali::NativeImageInterfacePtr nativeImageInterface)
{
  // The texture takes the flag the native image declares.
  Texture            texture  = Dali::Texture::New(*nativeImageInterface);
  Dali::Ui::ImageUrl imageUrl = Dali::Ui::ImageUrl::New(texture);
  return imageUrl;
}

Dali::Ui::ImageUrl GenerateUrl(Dali::NativeImageInterfacePtr nativeImageInterface, bool preMultiplied)
{
  // The texture takes the flag the native image declares; the caller can only add to it.
  Texture texture = Dali::Texture::New(*nativeImageInterface);
  if(preMultiplied)
  {
    Dali::Integration::SetTextureAlphaPreMultiplied(texture, true);
  }
  Dali::Ui::ImageUrl imageUrl = Dali::Ui::ImageUrl::New(texture);
  return imageUrl;
}

Dali::Ui::ImageUrl GenerateUrl(Dali::EncodedImageBuffer encodedImageBuffer)
{
  return Dali::Ui::ImageUrl::New(encodedImageBuffer);
}

Dali::Ui::ImageUrl GenerateDepthUrl(Dali::FrameBuffer frameBuffer)
{
  Texture            texture  = Dali::DevelFrameBuffer::GetDepthTexture(frameBuffer);
  Dali::Ui::ImageUrl imageUrl = Dali::Ui::ImageUrl::New(texture);
  return imageUrl;
}

} //namespace ImageUrlUtils

} // namespace Ui

} //namespace DALI_NAMESPACE
