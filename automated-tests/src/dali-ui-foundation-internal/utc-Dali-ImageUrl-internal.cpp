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

#include <dali-ui-foundation/integration-api/image-view-impl.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/internal/image-loader/image-url-tracker.h>
#include <dali-ui-foundation/internal/texture-manager/texture-manager-impl.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/npatch/npatch-loader.h>
#include <dali-ui-foundation/internal/visuals/svg/svg-loader.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-url.h>
#include <dali-ui-foundation/public-api/image-loader/image-url.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-event-thread-callback.h>
#include <cstdio>
#include <fstream>

#include <dali-ui-foundation/internal/visuals/visual-factory-cache.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-impl.h>

using Dali::Ui::Internal::ImageUrlTracker;
using Dali::Ui::Internal::VisualUrl;

namespace
{
class TestPngFile
{
public:
  explicit TestPngFile(std::string url)
  : mUrl(std::move(url))
  {
    static constexpr unsigned char PNG_DATA[] = {
      0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
      0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03,
      0x08, 0x06, 0x00, 0x00, 0x00, 0x56, 0x28, 0xb5, 0xbf, 0x00, 0x00, 0x00,
      0x13, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0x63, 0x60, 0x80, 0x80, 0xff,
      0x0c, 0x30, 0x06, 0x08, 0x30, 0xa0, 0x03, 0x00, 0x82, 0xa4, 0x05, 0xfb,
      0x9f, 0xb0, 0x15, 0x34, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44,
      0xae, 0x42, 0x60, 0x82};

    std::ofstream output(mUrl, std::ios::binary | std::ios::trunc);
    output.write(reinterpret_cast<const char*>(PNG_DATA), sizeof(PNG_DATA));
    mValid = output.good();
  }

  ~TestPngFile()
  {
    std::remove(mUrl.c_str());
  }

  const std::string& GetUrl() const
  {
    return mUrl;
  }

  bool IsValid() const
  {
    return mValid;
  }

private:
  const std::string mUrl;
  bool              mValid{false};
};

class TestSvgFile
{
public:
  TestSvgFile()
  {
    std::ofstream output(mUrl, std::ios::trunc);
    output << R"(<svg xmlns="http://www.w3.org/2000/svg" width="3" height="3"><rect width="3" height="3"/></svg>)";
    mValid = output.good();
  }

  ~TestSvgFile()
  {
    std::remove(mUrl.c_str());
  }

  const std::string& GetUrl() const
  {
    return mUrl;
  }

  bool IsValid() const
  {
    return mValid;
  }

private:
  const std::string mUrl{"/tmp/dali-ui-image-url-cache-pinning-test.svg"};
  bool              mValid{false};
};

Dali::Ui::Internal::VisualFactoryCache& GetFactoryCache()
{
  auto visualFactory = Dali::Ui::Integration::VisualFactory::Get();
  return Dali::Ui::GetImplementation(visualFactory).GetFactoryCache();
}

Dali::Ui::Internal::TextureManager& GetTextureManager()
{
  return GetFactoryCache().GetTextureManager();
}

Dali::Ui::Internal::TextureManager::TextureId RequestTexture(
  Dali::Ui::Internal::TextureManager& textureManager,
  const VisualUrl&                    url,
  const Dali::ImageDimensions&        desiredSize        = Dali::ImageDimensions(),
  bool                                synchronousLoading = true)
{
  auto multiplyOnLoad = Dali::Ui::Internal::TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  return textureManager.RequestLoad(url,
                                    desiredSize,
                                    Dali::SamplingMode::BOX,
                                    nullptr,
                                    true,
                                    Dali::Ui::Internal::TextureManager::ReloadPolicy::CACHED,
                                    multiplyOnLoad,
                                    synchronousLoading);
}

void ProcessQueues(UiTestApplication& application)
{
  application.SendNotification();
}

Dali::Ui::Internal::ViewDataImpl& GetViewData(Dali::Ui::ImageView imageView)
{
  return Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(imageView));
}

} // unnamed namespace

void utc_dali_image_url_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_image_url_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliImageUrlTrackerCountsActiveImageUrls(void)
{
  ImageUrlTracker   tracker;
  const std::string url{"/tmp/image.png"};

  DALI_TEST_CHECK(tracker.TrackImageUrl(url));
  DALI_TEST_CHECK(!tracker.TrackImageUrl(url));
  DALI_TEST_CHECK(tracker.HasActiveImageUrl(url));

  DALI_TEST_CHECK(!tracker.UntrackImageUrl(url));
  DALI_TEST_CHECK(tracker.HasActiveImageUrl(url));

  DALI_TEST_CHECK(tracker.UntrackImageUrl(url));
  DALI_TEST_CHECK(!tracker.HasActiveImageUrl(url));
  DALI_TEST_CHECK(!tracker.UntrackImageUrl(url));

  END_TEST;
}

int UtcDaliImageUrlCachePinningResourceTypes(void)
{
  DALI_TEST_CHECK(VisualUrl{"/tmp/image.png"}.SupportsImageUrlCachePinning());
  DALI_TEST_CHECK(VisualUrl{"/tmp/image.9.png"}.SupportsImageUrlCachePinning());
  DALI_TEST_CHECK(VisualUrl{"/tmp/image.svg"}.SupportsImageUrlCachePinning());
  DALI_TEST_CHECK(VisualUrl{"/tmp/image.tvg"}.SupportsImageUrlCachePinning());

  DALI_TEST_CHECK(!VisualUrl{"/tmp/image.gif"}.SupportsImageUrlCachePinning());
  DALI_TEST_CHECK(!VisualUrl{"/tmp/image.webp"}.SupportsImageUrlCachePinning());
  DALI_TEST_CHECK(!VisualUrl{"/tmp/image.json"}.SupportsImageUrlCachePinning());

  END_TEST;
}

int UtcDaliImageUrlTracksRemoteImageUrl(void)
{
  UiTestApplication application;
  const VisualUrl   url{"https://example.com/image.png"};
  auto&             factoryCache = GetFactoryCache();

  DALI_TEST_CHECK(!factoryCache.HasActiveImageUrl(url));

  Dali::Ui::ImageUrl imageUrl = Dali::Ui::ImageUrl::New(url.GetUrl().c_str());
  DALI_TEST_CHECK(factoryCache.HasActiveImageUrl(url));

  imageUrl.Reset();
  DALI_TEST_CHECK(!factoryCache.HasActiveImageUrl(url));

  END_TEST;
}

int UtcDaliImageUrlDoesNotTrackAnimatedImageUrl(void)
{
  UiTestApplication application;
  auto&             factoryCache = GetFactoryCache();

  const VisualUrl gifUrl{"/tmp/animated.gif"};
  const VisualUrl webpUrl{"/tmp/animated.webp"};
  const VisualUrl jsonUrl{"/tmp/animated.json"};

  Dali::Ui::ImageUrl gif  = Dali::Ui::ImageUrl::New(gifUrl.GetUrl().c_str());
  Dali::Ui::ImageUrl webp = Dali::Ui::ImageUrl::New(webpUrl.GetUrl().c_str());
  Dali::Ui::ImageUrl json = Dali::Ui::ImageUrl::New(jsonUrl.GetUrl().c_str());

  DALI_TEST_CHECK(!factoryCache.HasActiveImageUrl(gifUrl));
  DALI_TEST_CHECK(!factoryCache.HasActiveImageUrl(webpUrl));
  DALI_TEST_CHECK(!factoryCache.HasActiveImageUrl(jsonUrl));

  END_TEST;
}

int UtcDaliImageUrlKeepsRasterUntilLastImageUrlReset(void)
{
  TestPngFile        testImage{"/tmp/dali-ui-image-url-keeps-raster-test.png"};
  UiTestApplication  application;
  const std::string& sourceUrl = testImage.GetUrl();
  DALI_TEST_CHECK(testImage.IsValid());

  Dali::Ui::ImageUrl first          = Dali::Ui::ImageUrl::New(sourceUrl.c_str());
  Dali::Ui::ImageUrl second         = Dali::Ui::ImageUrl::New(sourceUrl.c_str());
  auto&              factoryCache   = GetFactoryCache();
  auto&              textureManager = GetTextureManager();
  const VisualUrl    url{sourceUrl};

  DALI_TEST_CHECK(factoryCache.HasActiveImageUrl(url));

  const auto textureId = RequestTexture(textureManager, url);
  DALI_TEST_CHECK(textureId != Dali::Ui::Internal::TextureManager::INVALID_TEXTURE_ID);

  textureManager.RequestRemove(textureId, nullptr);
  ProcessQueues(application); // Drop the request reference, leaving only the ImageUrl cache pin reference.

  const auto reusedTextureId = RequestTexture(textureManager, url);
  DALI_TEST_EQUALS(reusedTextureId, textureId, TEST_LOCATION);
  textureManager.RequestRemove(reusedTextureId, nullptr);
  ProcessQueues(application);

  first.Reset();
  DALI_TEST_CHECK(factoryCache.HasActiveImageUrl(url));
  const auto cachedForRemainingImageUrl = RequestTexture(textureManager, url);
  DALI_TEST_EQUALS(cachedForRemainingImageUrl, textureId, TEST_LOCATION);
  textureManager.RequestRemove(cachedForRemainingImageUrl, nullptr);
  ProcessQueues(application);

  second.Reset();
  DALI_TEST_CHECK(!factoryCache.HasActiveImageUrl(url));
  ProcessQueues(application);
  DALI_TEST_CHECK(!textureManager.GetVisualUrl(textureId).IsValid());

  const auto rawTextureId = RequestTexture(textureManager, url);
  DALI_TEST_CHECK(rawTextureId != Dali::Ui::Internal::TextureManager::INVALID_TEXTURE_ID);
  textureManager.RequestRemove(rawTextureId, nullptr);
  ProcessQueues(application);
  DALI_TEST_CHECK(!textureManager.GetVisualUrl(rawTextureId).IsValid());

  END_TEST;
}

int UtcDaliImageUrlKeepsMultipleConsumersAliveAfterImageUrlsReleased(void)
{
  TestPngFile        testImage{"/tmp/dali-ui-image-url-multiple-consumers-test.png"};
  UiTestApplication  application;
  const std::string& sourceUrl = testImage.GetUrl();
  DALI_TEST_CHECK(testImage.IsValid());

  Dali::Ui::ImageUrl firstImageUrl  = Dali::Ui::ImageUrl::New(sourceUrl.c_str());
  Dali::Ui::ImageUrl secondImageUrl = Dali::Ui::ImageUrl::New(sourceUrl.c_str());
  auto&              textureManager = GetTextureManager();
  const VisualUrl    url{sourceUrl};

  // Two independent consumers of the same URL must share one cache entry while
  // each keeps its own TextureManager reference.
  const auto firstConsumerId  = RequestTexture(textureManager, url);
  const auto secondConsumerId = RequestTexture(textureManager, url);
  DALI_TEST_CHECK(firstConsumerId != Dali::Ui::Internal::TextureManager::INVALID_TEXTURE_ID);
  DALI_TEST_EQUALS(secondConsumerId, firstConsumerId, TEST_LOCATION);

  // Dropping one ImageUrl and one consumer must not affect the remaining ImageUrl or consumer.
  firstImageUrl.Reset();
  textureManager.RequestRemove(firstConsumerId, nullptr);
  ProcessQueues(application);
  DALI_TEST_CHECK(textureManager.GetVisualUrl(secondConsumerId).IsValid());

  // Dropping the final ImageUrl releases only the cache pin reference. The second
  // active consumer must continue to own the texture.
  secondImageUrl.Reset();
  ProcessQueues(application);
  DALI_TEST_CHECK(textureManager.GetVisualUrl(secondConsumerId).IsValid());

  // The cache entry disappears only after the final consumer is released.
  textureManager.RequestRemove(secondConsumerId, nullptr);
  ProcessQueues(application);
  DALI_TEST_CHECK(!textureManager.GetVisualUrl(secondConsumerId).IsValid());

  END_TEST;
}

int UtcDaliImageUrlKeepsAsynchronouslyLoadedRasterUntilReset(void)
{
  TestPngFile       testImage{"/tmp/dali-ui-image-url-async-cache-pinning-test.png"};
  UiTestApplication application;
  DALI_TEST_CHECK(testImage.IsValid());

  const VisualUrl    url{testImage.GetUrl()};
  Dali::Ui::ImageUrl imageUrl       = Dali::Ui::ImageUrl::New(testImage.GetUrl().c_str());
  auto&              textureManager = GetTextureManager();
  const auto         textureId      = RequestTexture(textureManager, url, Dali::ImageDimensions(), false);
  DALI_TEST_CHECK(textureId != Dali::Ui::Internal::TextureManager::INVALID_TEXTURE_ID);

  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
  application.SendNotification();
  application.Render();

  textureManager.RequestRemove(textureId, nullptr);
  ProcessQueues(application);

  const auto reusedTextureId = RequestTexture(textureManager, url, Dali::ImageDimensions(), false);
  DALI_TEST_EQUALS(reusedTextureId, textureId, TEST_LOCATION);

  textureManager.RequestRemove(reusedTextureId, nullptr);
  ProcessQueues(application);
  imageUrl.Reset();
  ProcessQueues(application);
  DALI_TEST_CHECK(!textureManager.GetVisualUrl(textureId).IsValid());

  END_TEST;
}

int UtcDaliImageUrlKeepsExistingResourceUntilLastImageUrlReset(void)
{
  TestPngFile        testImage{"/tmp/dali-ui-image-url-existing-resource-test.png"};
  UiTestApplication  application;
  const std::string& sourceUrl = testImage.GetUrl();
  DALI_TEST_CHECK(testImage.IsValid());

  auto&           textureManager = GetTextureManager();
  const VisualUrl url{sourceUrl};

  // Exercise the opposite ordering: consumers already exist before ImageUrl
  // acquires a cache pin reference for the cached resource.
  const auto firstConsumerId  = RequestTexture(textureManager, url);
  const auto secondConsumerId = RequestTexture(textureManager, url);
  DALI_TEST_CHECK(firstConsumerId != Dali::Ui::Internal::TextureManager::INVALID_TEXTURE_ID);
  DALI_TEST_EQUALS(secondConsumerId, firstConsumerId, TEST_LOCATION);

  Dali::Ui::ImageUrl firstImageUrl  = Dali::Ui::ImageUrl::New(sourceUrl.c_str());
  Dali::Ui::ImageUrl secondImageUrl = Dali::Ui::ImageUrl::New(sourceUrl.c_str());

  // Release both consumers first. The ImageUrl cache pin
  // must keep the pre-existing cache entry alive.
  textureManager.RequestRemove(firstConsumerId, nullptr);
  textureManager.RequestRemove(secondConsumerId, nullptr);
  ProcessQueues(application);
  DALI_TEST_CHECK(textureManager.GetVisualUrl(firstConsumerId).IsValid());

  firstImageUrl.Reset();
  ProcessQueues(application);
  DALI_TEST_CHECK(textureManager.GetVisualUrl(firstConsumerId).IsValid());

  secondImageUrl.Reset();
  ProcessQueues(application);
  DALI_TEST_CHECK(!textureManager.GetVisualUrl(firstConsumerId).IsValid());

  END_TEST;
}

int UtcDaliImageUrlMovesCachePinToLatestDesiredSize(void)
{
  TestPngFile        testImage{"/tmp/dali-ui-image-url-latest-size-test.png"};
  UiTestApplication  application;
  const std::string& sourceUrl = testImage.GetUrl();
  DALI_TEST_CHECK(testImage.IsValid());

  Dali::Ui::ImageUrl imageUrl       = Dali::Ui::ImageUrl::New(sourceUrl.c_str());
  auto&              textureManager = GetTextureManager();
  const VisualUrl    url{sourceUrl};

  const auto firstTextureId = RequestTexture(textureManager, url, Dali::ImageDimensions(16, 16));
  DALI_TEST_CHECK(firstTextureId != Dali::Ui::Internal::TextureManager::INVALID_TEXTURE_ID);

  textureManager.RequestRemove(firstTextureId, nullptr);
  ProcessQueues(application);

  const auto secondTextureId = RequestTexture(textureManager, url, Dali::ImageDimensions(32, 32));
  DALI_TEST_CHECK(secondTextureId != Dali::Ui::Internal::TextureManager::INVALID_TEXTURE_ID);
  DALI_TEST_CHECK(secondTextureId != firstTextureId);

  ProcessQueues(application); // Release the cache pin reference that moved away from the 16x16 resource.
  DALI_TEST_CHECK(!textureManager.GetVisualUrl(firstTextureId).IsValid());
  DALI_TEST_CHECK(textureManager.GetVisualUrl(secondTextureId).IsValid());

  textureManager.RequestRemove(secondTextureId, nullptr);
  ProcessQueues(application);
  DALI_TEST_CHECK(textureManager.GetVisualUrl(secondTextureId).IsValid());

  imageUrl.Reset();
  ProcessQueues(application);
  DALI_TEST_CHECK(!textureManager.GetVisualUrl(secondTextureId).IsValid());

  END_TEST;
}

int UtcDaliImageUrlKeepsRasterAndMaskUntilReset(void)
{
  TestPngFile       testImage{"/tmp/dali-ui-image-url-masked-image-test.png"};
  TestPngFile       testMask{"/tmp/dali-ui-image-url-mask-test.png"};
  UiTestApplication application;
  DALI_TEST_CHECK(testImage.IsValid());
  DALI_TEST_CHECK(testMask.IsValid());

  auto&              textureManager = GetTextureManager();
  const VisualUrl    imageUrl{testImage.GetUrl()};
  const VisualUrl    maskUrl{testMask.GetUrl()};
  Dali::Ui::ImageUrl pinnedImageUrl = Dali::Ui::ImageUrl::New(testImage.GetUrl().c_str());

  Dali::Ui::Internal::TextureManager::MaskingDataPointer maskInfo(
    new Dali::Ui::Internal::TextureManager::MaskingData());
  maskInfo->mAlphaMaskUrl              = maskUrl;
  maskInfo->mPreappliedMasking         = false;
  auto                  textureId      = Dali::Ui::Internal::TextureManager::INVALID_TEXTURE_ID;
  bool                  loadingStatus  = false;
  auto                  multiplyOnLoad = Dali::Ui::Internal::TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  [[maybe_unused]] auto textures       = textureManager.LoadTexture(
    imageUrl,
    Dali::ImageDimensions(),
    Dali::SamplingMode::BOX,
    maskInfo,
    true,
    textureId,
    loadingStatus,
    nullptr,
    true,
    Dali::Ui::Internal::TextureManager::ReloadPolicy::CACHED,
    multiplyOnLoad);
  DALI_TEST_CHECK(textureId != Dali::Ui::Internal::TextureManager::INVALID_TEXTURE_ID);
  const auto maskTextureId = maskInfo->mAlphaMaskId;
  DALI_TEST_CHECK(maskTextureId != Dali::Ui::Internal::TextureManager::INVALID_TEXTURE_ID);

  textureManager.RequestRemove(textureId, nullptr);
  ProcessQueues(application);
  DALI_TEST_CHECK(textureManager.GetVisualUrl(textureId).IsValid());
  DALI_TEST_CHECK(textureManager.GetVisualUrl(maskTextureId).IsValid());

  pinnedImageUrl.Reset();
  ProcessQueues(application);
  DALI_TEST_CHECK(!textureManager.GetVisualUrl(textureId).IsValid());
  DALI_TEST_CHECK(!textureManager.GetVisualUrl(maskTextureId).IsValid());

  END_TEST;
}

int UtcDaliImageUrlKeepsNPatchUntilReset(void)
{
  TestPngFile       testImage{"/tmp/dali-ui-image-url-cache-pinning-test.9.png"};
  UiTestApplication application;
  DALI_TEST_CHECK(testImage.IsValid());

  const VisualUrl     url{testImage.GetUrl()};
  auto&               factoryCache   = GetFactoryCache();
  auto&               loader         = factoryCache.GetNPatchLoader();
  auto&               textureManager = GetTextureManager();
  bool                preMultiplyOnLoad{true};
  const Dali::Extents border;

  factoryCache.TrackImageUrl(url);
  const auto id = loader.Load(textureManager, nullptr, url, border, preMultiplyOnLoad, true);
  DALI_TEST_CHECK(id != Dali::Ui::Internal::NPatchData::INVALID_NPATCH_DATA_ID);

  loader.RequestRemove(id, nullptr);
  ProcessQueues(application);
  const auto reusedId = loader.Load(textureManager, nullptr, url, border, preMultiplyOnLoad, true);
  DALI_TEST_EQUALS(reusedId, id, TEST_LOCATION);

  loader.RequestRemove(reusedId, nullptr);
  ProcessQueues(application);
  factoryCache.UntrackImageUrl(url);
  ProcessQueues(application);

  const auto reloadedId = loader.Load(textureManager, nullptr, url, border, preMultiplyOnLoad, true);
  DALI_TEST_CHECK(reloadedId != id);
  loader.RequestRemove(reloadedId, nullptr);
  ProcessQueues(application);

  END_TEST;
}

int UtcDaliImageUrlKeepsVectorLoadUntilReset(void)
{
  TestSvgFile       testImage;
  UiTestApplication application;
  DALI_TEST_CHECK(testImage.IsValid());

  const VisualUrl url{testImage.GetUrl()};
  auto&           factoryCache = GetFactoryCache();
  auto&           loader       = factoryCache.GetSvgLoader();

  factoryCache.TrackImageUrl(url);
  const auto id = loader.Load(url, nullptr, true);
  DALI_TEST_CHECK(id != Dali::Ui::Internal::SvgLoader::INVALID_SVG_LOAD_ID);

  loader.RequestLoadRemove(id, nullptr);
  ProcessQueues(application);
  const auto reusedId = loader.Load(url, nullptr, true);
  DALI_TEST_EQUALS(reusedId, id, TEST_LOCATION);

  loader.RequestLoadRemove(reusedId, nullptr);
  ProcessQueues(application);
  factoryCache.UntrackImageUrl(url);
  ProcessQueues(application);

  const auto reloadedId = loader.Load(url, nullptr, true);
  DALI_TEST_CHECK(reloadedId != id);
  loader.RequestLoadRemove(reloadedId, nullptr);
  ProcessQueues(application);

  END_TEST;
}

int UtcDaliImageUrlKeepsVectorRasterizeUntilReset(void)
{
  TestSvgFile       testImage;
  UiTestApplication application;
  DALI_TEST_CHECK(testImage.IsValid());

  const VisualUrl url{testImage.GetUrl()};
  auto&           factoryCache = GetFactoryCache();
  auto&           loader       = factoryCache.GetSvgLoader();

  Dali::Ui::ImageUrl imageUrl    = Dali::Ui::ImageUrl::New(testImage.GetUrl().c_str());
  const auto         loadId      = loader.Load(url, nullptr, true);
  const auto         rasterizeId = loader.Rasterize(loadId, 16u, 16u, nullptr, true);
  DALI_TEST_CHECK(loadId != Dali::Ui::Internal::SvgLoader::INVALID_SVG_LOAD_ID);
  DALI_TEST_CHECK(rasterizeId != Dali::Ui::Internal::SvgLoader::INVALID_SVG_RASTERIZE_ID);

  // Release the caller references. The active ImageUrl must keep the rasterized
  // result, and its corresponding vector load, alive.
  loader.RequestRasterizeRemove(rasterizeId, nullptr, false);
  loader.RequestLoadRemove(loadId, nullptr);
  ProcessQueues(application);

  const auto reusedLoadId      = loader.Load(url, nullptr, true);
  const auto reusedRasterizeId = loader.Rasterize(reusedLoadId, 16u, 16u, nullptr, true);
  DALI_TEST_EQUALS(reusedLoadId, loadId, TEST_LOCATION);
  DALI_TEST_EQUALS(reusedRasterizeId, rasterizeId, TEST_LOCATION);

  loader.RequestRasterizeRemove(reusedRasterizeId, nullptr, false);
  loader.RequestLoadRemove(reusedLoadId, nullptr);
  ProcessQueues(application);

  imageUrl.Reset();
  ProcessQueues(application);

  const auto reloadedLoadId      = loader.Load(url, nullptr, true);
  const auto reloadedRasterizeId = loader.Rasterize(reloadedLoadId, 16u, 16u, nullptr, true);
  DALI_TEST_CHECK(reloadedLoadId != loadId);
  DALI_TEST_CHECK(reloadedRasterizeId != rasterizeId);

  loader.RequestRasterizeRemove(reloadedRasterizeId, nullptr, false);
  loader.RequestLoadRemove(reloadedLoadId, nullptr);
  ProcessQueues(application);

  END_TEST;
}

int UtcDaliImageViewKeepsEncodedImageBufferHandle(void)
{
  UiTestApplication application;

  Dali::EncodedImageBuffer::RawBufferType rawBuffer;
  rawBuffer.PushBack(0x11u);
  rawBuffer.PushBack(0x22u);
  rawBuffer.PushBack(0x33u);

  Dali::EncodedImageBuffer encodedImageBuffer = Dali::EncodedImageBuffer::New(std::move(rawBuffer));
  Dali::Ui::ImageUrl       imageUrl           = Dali::Ui::ImageUrl::New(encodedImageBuffer);
  const VisualUrl          visualUrl{imageUrl.GetUrl().CStr()};
  auto&                    textureManager = GetTextureManager();

  Dali::Ui::ImageView imageView = Dali::Ui::ImageView::New();
  imageView.SetResourceUrl(imageUrl);
  imageUrl.Reset();

  // ImageView must own the ImageUrl before its deferred visual is created.
  DALI_TEST_CHECK(textureManager.GetEncodedImageBuffer(visualUrl));

  // Replacing the resource through the string overload releases the stored handle.
  imageView.SetResourceUrl(Dali::String());
  ProcessQueues(application);
  DALI_TEST_CHECK(!textureManager.GetEncodedImageBuffer(visualUrl));

  END_TEST;
}

int UtcDaliImageViewNewKeepsEncodedImageBufferHandle(void)
{
  UiTestApplication application;

  Dali::EncodedImageBuffer::RawBufferType rawBuffer;
  rawBuffer.PushBack(0x11u);
  rawBuffer.PushBack(0x22u);
  rawBuffer.PushBack(0x33u);

  Dali::EncodedImageBuffer encodedImageBuffer = Dali::EncodedImageBuffer::New(std::move(rawBuffer));
  Dali::Ui::ImageUrl       imageUrl           = Dali::Ui::ImageUrl::New(encodedImageBuffer);
  const VisualUrl          visualUrl{imageUrl.GetUrl().CStr()};
  auto&                    textureManager = GetTextureManager();

  Dali::Ui::ImageView imageView = Dali::Ui::ImageView::New(imageUrl);
  imageUrl.Reset();

  DALI_TEST_EQUALS(imageView.GetResourceUrl(), Dali::String(visualUrl.GetUrl().c_str()), TEST_LOCATION);
  DALI_TEST_CHECK(textureManager.GetEncodedImageBuffer(visualUrl));

  imageView.SetResourceUrl(Dali::String());
  ProcessQueues(application);
  DALI_TEST_CHECK(!textureManager.GetEncodedImageBuffer(visualUrl));

  END_TEST;
}

int UtcDaliImageViewKeepsLocalImageUrlHandle(void)
{
  TestPngFile       testImage{"/tmp/dali-ui-image-view-local-image-url-cache-pinning-test.png"};
  UiTestApplication application;
  DALI_TEST_CHECK(testImage.IsValid());

  Dali::Ui::ImageUrl first  = Dali::Ui::ImageUrl::New(testImage.GetUrl().c_str());
  Dali::Ui::ImageUrl second = Dali::Ui::ImageUrl::New(testImage.GetUrl().c_str());
  const VisualUrl    visualUrl{testImage.GetUrl()};
  auto&              textureManager = GetTextureManager();

  Dali::Ui::ImageView imageView = Dali::Ui::ImageView::New();
  imageView.SetResourceUrl(first);
  first.Reset();

  // The attachment must still be replaced when a different ImageUrl has the same URL.
  imageView.SetResourceUrl(second);
  second.Reset();

  const auto textureId = RequestTexture(textureManager, visualUrl);
  DALI_TEST_CHECK(textureId != Dali::Ui::Internal::TextureManager::INVALID_TEXTURE_ID);
  textureManager.RequestRemove(textureId, nullptr);
  ProcessQueues(application);
  DALI_TEST_CHECK(textureManager.GetVisualUrl(textureId).IsValid());

  imageView.SetResourceUrl(Dali::String());
  ProcessQueues(application);
  DALI_TEST_CHECK(!textureManager.GetVisualUrl(textureId).IsValid());

  END_TEST;
}

int UtcDaliImageViewImmediateKeepsLocalImageUrlHandle(void)
{
  TestPngFile       testImage{"/tmp/dali-ui-image-view-immediate-image-url-cache-pinning-test.png"};
  UiTestApplication application;
  DALI_TEST_CHECK(testImage.IsValid());

  const VisualUrl url{testImage.GetUrl()};
  auto&           factoryCache = GetFactoryCache();

  Dali::Ui::ImageUrl  imageUrl  = Dali::Ui::ImageUrl::New(testImage.GetUrl().c_str());
  Dali::Ui::ImageView imageView = Dali::Ui::ImageView::New();
  imageView.SetLoadPolicy(Dali::Ui::Image::LoadPolicy::IMMEDIATE);
  imageView.SetSynchronousLoading(true);
  imageView.SetDesiredWidth(3);
  imageView.SetDesiredHeight(3);
  imageView.SetResourceUrl(imageUrl);
  imageUrl.Reset();

  DALI_TEST_CHECK(factoryCache.HasActiveImageUrl(url));

  imageView.SetResourceUrl(Dali::String());
  DALI_TEST_CHECK(!factoryCache.HasActiveImageUrl(url));
  ProcessQueues(application);

  END_TEST;
}

int UtcDaliImageViewActiveImageUrlDisablesFastTrack(void)
{
  TestPngFile       testImage{"/tmp/dali-ui-image-view-active-image-url-fast-track-test.png"};
  UiTestApplication application;
  DALI_TEST_CHECK(testImage.IsValid());

  Dali::Ui::ImageUrl  imageUrl  = Dali::Ui::ImageUrl::New(testImage.GetUrl().c_str());
  Dali::Ui::ImageView imageView = Dali::Ui::ImageView::New();
  imageView.SetRequestedWidth(3.0f);
  imageView.SetRequestedHeight(3.0f);
  imageView.SetLoadPolicy(Dali::Ui::Image::LoadPolicy::ATTACHED);
  imageView.SetReleasePolicy(Dali::Ui::Image::ReleasePolicy::DETACHED);
  imageView.SetFastTrackUpload(true);
  imageView.SetResourceUrl(imageUrl);

  auto visual = GetViewData(imageView).GetVisual(Dali::Ui::ImageView::Property::IMAGE);
  DALI_TEST_CHECK(visual);

  application.GetScene().Add(imageView);

  // Fast-track installs generated textures on the renderer immediately. A
  // active ImageUrl must instead enter TextureManager with no immediate texture.
  const auto renderer = Dali::Ui::GetImplementation(visual).GetRenderer();
  DALI_TEST_CHECK(renderer);
  const auto textures = renderer.GetTextures();
  DALI_TEST_CHECK(!textures || textures.GetTextureCount() == 0u);

  application.GetScene().Remove(imageView);
  imageView.SetResourceUrl(Dali::String());
  imageUrl.Reset();
  ProcessQueues(application);

  END_TEST;
}
