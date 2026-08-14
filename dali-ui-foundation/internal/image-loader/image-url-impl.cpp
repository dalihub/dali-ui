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

// CLASS HEADER
#include <dali-ui-foundation/internal/image-loader/image-url-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/image-loader/texture-manager.h>
#include <dali-ui-foundation/internal/texture-manager/texture-manager-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-url.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>

using Dali::Integration::ToDaliString;
using Dali::Integration::ToStdString;

namespace Dali
{
namespace Ui
{
namespace Internal
{
ImageUrl::ImageUrl(Texture& texture, bool preMultiplied)
{
  mUrl = Dali::Ui::Integration::TextureManager::AddTexture(texture, preMultiplied);
}

ImageUrl::ImageUrl(const EncodedImageBuffer& encodedImageBuffer)
: mUrl("")
{
  auto visualFactory = Dali::Ui::Integration::VisualFactory::Get();
  if(visualFactory)
  {
    auto& textureManager = GetImplementation(visualFactory).GetTextureManager();
    mUrl                 = ToDaliString(textureManager.AddEncodedImageBuffer(encodedImageBuffer));
  }
}

ImageUrl::ImageUrl(const Dali::String& url)
: mUrl(),
  mIsPathBased(true)
{
  const VisualUrl visualUrl(ToStdString(url));
  if(!visualUrl.IsValid())
  {
    return;
  }

  if(visualUrl.GetProtocolType() != VisualUrl::LOCAL && visualUrl.GetProtocolType() != VisualUrl::REMOTE)
  {
    DALI_LOG_WARNING("ImageUrl::New(url) supports local and remote image URLs only.\n");
    return;
  }

  mUrl                            = url;
  const bool supportsCachePinning = visualUrl.SupportsImageUrlCachePinning();
  if(!supportsCachePinning)
  {
    DALI_LOG_WARNING(
      "ImageUrl cache pinning is not supported for animated image URL '%s'; "
      "the returned URL remains usable without cache pinning.\n",
      visualUrl.GetUrl().c_str());
  }

  auto visualFactory = Dali::Ui::Integration::VisualFactory::Get();
  if(visualFactory && supportsCachePinning)
  {
    GetImplementation(visualFactory).GetFactoryCache().TrackImageUrl(visualUrl);
  }
}

ImageUrl::~ImageUrl()
{
  if(mUrl.Size() > 0)
  {
    auto visualFactory = Dali::Ui::Integration::VisualFactory::Get();
    if(visualFactory)
    {
      if(mIsPathBased)
      {
        const VisualUrl visualUrl(ToStdString(mUrl));
        if(visualUrl.SupportsImageUrlCachePinning())
        {
          GetImplementation(visualFactory).GetFactoryCache().UntrackImageUrl(visualUrl);
        }
      }
      else
      {
        auto& textureManager = GetImplementation(visualFactory).GetTextureManager();
        textureManager.RequestRemoveExternalResourceByUrl(ToStdString(mUrl));
      }
    }
  }
}

ImageUrlPtr ImageUrl::New(Texture& texture, bool preMultiplied)
{
  ImageUrlPtr imageUrlPtr = new ImageUrl(texture, preMultiplied);
  return imageUrlPtr;
}

ImageUrlPtr ImageUrl::New(const EncodedImageBuffer& encodedImageBuffer)
{
  ImageUrlPtr imageUrlPtr = new ImageUrl(encodedImageBuffer);
  return imageUrlPtr;
}

ImageUrlPtr ImageUrl::New(const Dali::String& url)
{
  ImageUrlPtr imageUrlPtr = new ImageUrl(url);
  return imageUrlPtr;
}

const Dali::String& ImageUrl::GetUrl() const
{
  return mUrl;
}

} // End of namespace Internal

} // End of namespace Ui

} // End of namespace Dali
