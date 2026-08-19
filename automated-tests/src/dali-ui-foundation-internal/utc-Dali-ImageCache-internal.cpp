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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>

#define private public
#define protected public
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/internal/texture-manager/texture-manager-impl.h>
#include <dali-ui-foundation/internal/visuals/npatch/npatch-loader.h>
#include <dali-ui-foundation/internal/visuals/svg/svg-loader.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-cache.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-impl.h>
#undef protected
#undef private

namespace
{
Dali::Ui::Internal::VisualFactoryCache& GetFactoryCache()
{
  auto visualFactory = Dali::Ui::Integration::VisualFactory::Get();
  return Dali::Ui::GetImplementation(visualFactory).GetFactoryCache();
}

Dali::Ui::Internal::TextureManager::TextureId AddUploadedTexture(Dali::Ui::Internal::TextureManager& textureManager,
                                                                 const char*                         url,
                                                                 int32_t                             referenceCount)
{
  auto&      cache     = textureManager.mTextureCacheManager;
  auto       textureId = cache.GenerateTextureId();
  const auto visualUrl = Dali::Ui::Internal::VisualUrl(url);
  auto       hash      = cache.GenerateHash(visualUrl, Dali::ImageDimensions(), Dali::SamplingMode::BOX_THEN_LINEAR,
                                            Dali::Ui::Internal::TextureManager::INVALID_TEXTURE_ID, false, true, 0u);

  Dali::Ui::Internal::TextureManager::TextureInfo textureInfo(
    textureId, Dali::Ui::Internal::TextureManager::INVALID_TEXTURE_ID, visualUrl, Dali::ImageDimensions(), 1.0f,
    Dali::SamplingMode::BOX_THEN_LINEAR, false, false, hash, true, false, Dali::AnimatedImageLoading(), 0u, false);
  textureInfo.referenceCount = referenceCount;
  textureInfo.loadState      = Dali::Ui::Internal::TextureManager::LoadState::UPLOADED;
  cache.AppendCache(textureInfo);

  return textureId;
}

bool IsTextureCached(Dali::Ui::Internal::TextureManager&           textureManager,
                     Dali::Ui::Internal::TextureManager::TextureId textureId)
{
  return textureManager.mTextureCacheManager.GetCacheIndexFromId(textureId) !=
         Dali::Ui::Internal::TextureManager::INVALID_CACHE_INDEX;
}

Dali::Ui::Internal::TextureManager::TextureId ReuseTexture(Dali::Ui::Internal::TextureManager& textureManager,
                                                           const char*                         url)
{
  auto preMultiplyOnLoad = Dali::Ui::Internal::TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  return textureManager.RequestLoad(Dali::Ui::Internal::VisualUrl(url), Dali::ImageDimensions(),
                                    Dali::SamplingMode::BOX_THEN_LINEAR, nullptr, true,
                                    Dali::Ui::Internal::TextureManager::ReloadPolicy::CACHED,
                                    preMultiplyOnLoad, true);
}
} // unnamed namespace

void utc_dali_image_cache_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_image_cache_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliImageCacheClearUnusedTexturesProcessesPendingRemovals(void)
{
  UiTestApplication application;

  constexpr auto URL = "image-cache-pending.png";

  auto& textureManager = GetFactoryCache().GetTextureManager();
  auto  textureId      = AddUploadedTexture(textureManager, URL, 2);

  textureManager.RequestRemove(textureId, nullptr, true);
  Dali::Ui::ImageCacheUtils::ClearUnusedTextures();
  textureManager.RequestRemove(textureId, nullptr);
  application.SendNotification();

  DALI_TEST_CHECK(!IsTextureCached(textureManager, textureId));

  END_TEST;
}

int UtcDaliImageCacheClearUnusedTexturesSkipsActiveSharedTexture(void)
{
  UiTestApplication application;

  constexpr auto URL = "image-cache-shared.png";

  auto& textureManager = GetFactoryCache().GetTextureManager();
  auto  textureId      = AddUploadedTexture(textureManager, URL, 2);

  textureManager.RequestRemove(textureId, nullptr, true);
  application.SendNotification();

  Dali::Ui::ImageCacheUtils::ClearUnusedTextures();
  application.SendNotification();
  DALI_TEST_CHECK(IsTextureCached(textureManager, textureId));

  textureManager.RequestRemove(textureId, nullptr);
  application.SendNotification();
  DALI_TEST_CHECK(IsTextureCached(textureManager, textureId));

  auto reusedId = ReuseTexture(textureManager, URL);
  DALI_TEST_EQUALS(reusedId, textureId, TEST_LOCATION);
  textureManager.RequestRemove(reusedId, nullptr);
  application.SendNotification();

  Dali::Ui::ImageCacheUtils::ClearUnusedTextures();
  application.SendNotification();
  DALI_TEST_CHECK(!IsTextureCached(textureManager, textureId));

  END_TEST;
}

int UtcDaliImageCacheClearUnusedTexturesRetainsReverseReleaseOrder(void)
{
  UiTestApplication application;

  constexpr auto URL = "image-cache-reverse-order.png";

  auto& textureManager = GetFactoryCache().GetTextureManager();
  auto  textureId      = AddUploadedTexture(textureManager, URL, 2);

  textureManager.RequestRemove(textureId, nullptr);
  application.SendNotification();
  textureManager.RequestRemove(textureId, nullptr, true);
  application.SendNotification();
  DALI_TEST_CHECK(IsTextureCached(textureManager, textureId));

  Dali::Ui::ImageCacheUtils::ClearUnusedTextures();
  application.SendNotification();
  DALI_TEST_CHECK(!IsTextureCached(textureManager, textureId));

  END_TEST;
}

int UtcDaliImageCacheClearUnusedTexturesClearsNPatchCache(void)
{
  UiTestApplication application;

  auto& factoryCache   = GetFactoryCache();
  auto& textureManager = factoryCache.GetTextureManager();
  auto& npatchLoader   = factoryCache.GetNPatchLoader();
  bool  preMultiply    = false;
  auto  npatchId       = npatchLoader.Load(textureManager, nullptr,
                                           Dali::Ui::Internal::VisualUrl("image-cache.9.png"),
                                           Dali::Extents(), preMultiply, true);

  Dali::Ui::Internal::NPatchDataPtr data;
  DALI_TEST_CHECK(npatchLoader.GetNPatchData(npatchId, data));

  npatchLoader.RequestRemove(npatchId, nullptr, true);
  application.SendNotification();
  DALI_TEST_CHECK(npatchLoader.GetNPatchData(npatchId, data));

  Dali::Ui::ImageCacheUtils::ClearUnusedTextures();
  application.SendNotification();
  DALI_TEST_CHECK(!npatchLoader.GetNPatchData(npatchId, data));

  END_TEST;
}

int UtcDaliImageCacheClearUnusedTexturesSkipsSharedNPatch(void)
{
  UiTestApplication application;

  auto&      factoryCache   = GetFactoryCache();
  auto&      textureManager = factoryCache.GetTextureManager();
  auto&      npatchLoader   = factoryCache.GetNPatchLoader();
  const auto url            = Dali::Ui::Internal::VisualUrl("image-cache-shared.9.png");
  bool       preMultiply    = false;
  auto       firstId        = npatchLoader.Load(textureManager, nullptr, url, Dali::Extents(), preMultiply, true);
  auto       secondId       = npatchLoader.Load(textureManager, nullptr, url, Dali::Extents(), preMultiply, true);

  DALI_TEST_EQUALS(firstId, secondId, TEST_LOCATION);

  npatchLoader.RequestRemove(firstId, nullptr, true);
  application.SendNotification();

  Dali::Ui::ImageCacheUtils::ClearUnusedTextures();
  application.SendNotification();

  Dali::Ui::Internal::NPatchDataPtr data;
  DALI_TEST_CHECK(npatchLoader.GetNPatchData(firstId, data));

  npatchLoader.RequestRemove(secondId, nullptr);
  application.SendNotification();
  DALI_TEST_CHECK(npatchLoader.GetNPatchData(firstId, data));

  Dali::Ui::ImageCacheUtils::ClearUnusedTextures();
  application.SendNotification();
  DALI_TEST_CHECK(!npatchLoader.GetNPatchData(firstId, data));

  END_TEST;
}

int UtcDaliImageCacheClearUnusedTexturesClearsSvgCache(void)
{
  UiTestApplication application;

  auto& svgLoader = GetFactoryCache().GetSvgLoader();
  auto  loadId    = svgLoader.Load(Dali::Ui::Internal::VisualUrl("image-cache.svg"), nullptr, true);
  auto  rasterId32 = svgLoader.Rasterize(loadId, 32u, 32u, nullptr, true);
  auto  rasterId64 = svgLoader.Rasterize(loadId, 64u, 64u, nullptr, true);

  DALI_TEST_CHECK(svgLoader.GetVectorImageRenderer(loadId));
  DALI_TEST_CHECK(rasterId32 != Dali::Ui::Internal::SvgLoader::INVALID_SVG_RASTERIZE_ID);
  DALI_TEST_CHECK(rasterId64 != Dali::Ui::Internal::SvgLoader::INVALID_SVG_RASTERIZE_ID);
  DALI_TEST_CHECK(rasterId32 != rasterId64);

  svgLoader.RequestRasterizeRemove(rasterId32, nullptr, false, true);
  svgLoader.RequestRasterizeRemove(rasterId64, nullptr, false, true);
  svgLoader.RequestLoadRemove(loadId, nullptr, true);
  application.SendNotification();
  DALI_TEST_CHECK(svgLoader.GetVectorImageRenderer(loadId));

  Dali::Ui::ImageCacheUtils::ClearUnusedTextures();
  application.SendNotification();
  DALI_TEST_CHECK(!svgLoader.GetVectorImageRenderer(loadId));

  auto reloadedLoadId     = svgLoader.Load(Dali::Ui::Internal::VisualUrl("image-cache.svg"), nullptr, true);
  auto reloadedRasterId32 = svgLoader.Rasterize(reloadedLoadId, 32u, 32u, nullptr, true);
  auto reloadedRasterId64 = svgLoader.Rasterize(reloadedLoadId, 64u, 64u, nullptr, true);
  DALI_TEST_CHECK(reloadedLoadId != loadId);
  DALI_TEST_CHECK(reloadedRasterId32 != rasterId32);
  DALI_TEST_CHECK(reloadedRasterId64 != rasterId64);

  svgLoader.RequestRasterizeRemove(reloadedRasterId32, nullptr, false);
  svgLoader.RequestRasterizeRemove(reloadedRasterId64, nullptr, false);
  svgLoader.RequestLoadRemove(reloadedLoadId, nullptr);
  application.SendNotification();

  END_TEST;
}
