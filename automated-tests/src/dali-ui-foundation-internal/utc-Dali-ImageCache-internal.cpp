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
#include <dali-ui-foundation/integration-api/visuals/image-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#define protected public
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/internal/texture-manager/texture-manager-impl.h>
#include <dali-ui-foundation/internal/visuals/npatch/npatch-loader.h>
#include <dali-ui-foundation/internal/visuals/npatch/npatch-visual.h>
#include <dali-ui-foundation/internal/visuals/animated-image/rolling-image-cache.h>
#include <dali-ui-foundation/internal/visuals/animated-image/rolling-animated-image-cache.h>
#include <dali-ui-foundation/internal/visuals/animated-image/fixed-image-cache.h>
#include <dali-ui-foundation/internal/visuals/svg/svg-loader.h>
#include <dali-ui-foundation/internal/visuals/svg/svg-visual.h>
#include <dali-ui-foundation/internal/visuals/image/image-visual-shader-factory.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-cache.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-impl.h>
#undef protected
#undef private

namespace Test
{
bool WaitForEventThreadTrigger(int triggerCount, int timeoutInSeconds, int executeCallbacks);
}

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

class TestFrameReadyObserver : public Dali::Ui::Internal::ImageCache::FrameReadyObserver
{
public:
  void FrameReady(Dali::TextureSet textureSet, uint32_t interval, bool preMultiplied) override
  {
    ++callCount;
    lastTextureSet   = textureSet;
    lastInterval     = interval;
    lastPremultiplied = preMultiplied;
  }

  uint32_t         callCount{0u};
  Dali::TextureSet lastTextureSet;
  uint32_t         lastInterval{0u};
  bool             lastPremultiplied{false};
};
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
                                           Dali::Insets(), preMultiply, true);

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
  auto       firstId        = npatchLoader.Load(textureManager, nullptr, url, Dali::Insets(), preMultiply, true);
  auto       secondId       = npatchLoader.Load(textureManager, nullptr, url, Dali::Insets(), preMultiply, true);

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

int UtcDaliRollingImageCacheStateP(void)
{
  UiTestApplication application;
  auto& textureManager = GetFactoryCache().GetTextureManager();
  auto firstTextureId  = AddUploadedTexture(textureManager, "rolling-first.png", 1);
  auto secondTextureId = AddUploadedTexture(textureManager, "rolling-second.png", 1);

  Dali::Ui::Internal::ImageCache::UrlList urls(2u);
  urls[0].mUrl       = Dali::Ui::Internal::VisualUrl("rolling-first.png");
  urls[0].mTextureId = firstTextureId;
  urls[1].mUrl       = Dali::Ui::Internal::VisualUrl("rolling-second.png");
  urls[1].mTextureId = secondTextureId;
  Dali::Ui::Internal::TextureManager::MaskingDataPointer maskingData;
  TestFrameReadyObserver observer;
  Dali::Ui::Internal::RollingImageCache cache(textureManager,
                                               Dali::ImageDimensions(),
                                               Dali::SamplingMode::BOX_THEN_LINEAR,
                                               urls,
                                               maskingData,
                                               observer,
                                               2u,
                                               1u,
                                               42u,
                                               true);

  DALI_TEST_EQUALS(cache.GetCurrentFrameIndex(), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(cache.GetTotalFrameCount(), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(cache.GetFrameInterval(1u), 42u, TEST_LOCATION);
  DALI_TEST_CHECK(!cache.IsFrontReady());

  cache.mQueue.PushBack({0u, false});
  cache.mRequestingLoad = true;
  Dali::TextureSet readyTextureSet = Dali::TextureSet::New();
  Dali::Ui::TextureUploadObserver::TextureInformation readyInformation(
    Dali::Ui::TextureUploadObserver::ReturnType::TEXTURE,
    firstTextureId,
    readyTextureSet,
    true);
  cache.LoadComplete(true, readyInformation);
  DALI_TEST_CHECK(cache.IsFrontReady());
  DALI_TEST_EQUALS(observer.callCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.lastInterval, 42u, TEST_LOCATION);
  DALI_TEST_CHECK(observer.lastPremultiplied);
  DALI_TEST_EQUALS(cache.GetCurrentFrameIndex(), 0, TEST_LOCATION);
  cache.FirstFrame();
  cache.GetFrontTextureSet();
  DALI_TEST_EQUALS(cache.GetCachedTextureId(0u), firstTextureId, TEST_LOCATION);

  cache.mQueue.PushBack({1u, false});
  cache.mRequestingLoad = false;
  Dali::Ui::TextureUploadObserver::TextureInformation secondInformation(
    Dali::Ui::TextureUploadObserver::ReturnType::TEXTURE,
    secondTextureId,
    Dali::TextureSet::New(),
    false);
  cache.LoadComplete(true, secondInformation);
  DALI_TEST_EQUALS(observer.callCount, 1u, TEST_LOCATION);

  cache.LoadComplete(false, secondInformation);
  DALI_TEST_EQUALS(observer.callCount, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.lastInterval, 0u, TEST_LOCATION);
  cache.ClearCache(true);
  DALI_TEST_EQUALS(cache.GetCurrentFrameIndex(), -1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliSvgLoaderSharedSynchronousRequestsP(void)
{
  UiTestApplication application;
  auto& loader = GetFactoryCache().GetSvgLoader();
  const std::string source(__FILE__);
  const std::size_t marker = source.find("/automated-tests/");
  DALI_TEST_CHECK(marker != std::string::npos);
  const auto url = Dali::Ui::Internal::VisualUrl(source.substr(0u, marker) + "/samples/image-view/res/svg-blocks.svg");

  class Observer : public Dali::Ui::Internal::SvgLoaderObserver
  {
  public:
    void LoadComplete(int32_t id, Dali::VectorImageRenderer renderer) override
    {
      ++loadCount;
      lastLoadId = id;
      loaded = static_cast<bool>(renderer);
    }

    void RasterizeComplete(int32_t id, Dali::TextureSet texture) override
    {
      ++rasterCount;
      lastRasterId = id;
      rasterized = static_cast<bool>(texture);
    }

    int loadCount{0};
    int rasterCount{0};
    int32_t lastLoadId{-1};
    int32_t lastRasterId{-1};
    bool loaded{false};
    bool rasterized{false};
  };

  Observer first;
  Observer second;
  const auto firstLoad = loader.Load(url, &first, true);
  const auto secondLoad = loader.Load(url, &second, true);
  DALI_TEST_EQUALS(firstLoad, secondLoad, TEST_LOCATION);
  DALI_TEST_CHECK(first.loaded);
  DALI_TEST_CHECK(second.loaded);
  DALI_TEST_EQUALS(first.lastLoadId, firstLoad, TEST_LOCATION);
  DALI_TEST_EQUALS(second.lastLoadId, secondLoad, TEST_LOCATION);

  const auto firstRaster = loader.Rasterize(firstLoad, 32u, 32u, &first, true);
  const auto secondRaster = loader.Rasterize(secondLoad, 32u, 32u, &second, true);
  const auto largerRaster = loader.Rasterize(firstLoad, 64u, 48u, &first, true);
  DALI_TEST_EQUALS(firstRaster, secondRaster, TEST_LOCATION);
  DALI_TEST_CHECK(largerRaster != firstRaster);
  DALI_TEST_CHECK(first.rasterized);
  DALI_TEST_CHECK(second.rasterized);
  DALI_TEST_CHECK(first.rasterCount >= 2);
  DALI_TEST_CHECK(second.rasterCount >= 1);
  DALI_TEST_EQUALS(first.lastRasterId, largerRaster, TEST_LOCATION);

  loader.RequestRasterizeRemove(firstRaster, &first, false);
  loader.RequestRasterizeRemove(secondRaster, &second, false);
  loader.RequestRasterizeRemove(largerRaster, &first, false);
  loader.RequestLoadRemove(firstLoad, &first);
  loader.RequestLoadRemove(secondLoad, &second);
  application.SendNotification();
  DALI_TEST_CHECK(!loader.GetVectorImageRenderer(firstLoad));
  END_TEST;
}

int UtcDaliVisualFactoryCacheGeometryAndBrokenImageFallbackP(void)
{
  UiTestApplication application;
  using Cache = Dali::Ui::Internal::VisualFactoryCache;
  Cache cache(false);

  Geometry normalized = Cache::CreateGridGeometry(Uint16Pair(3u, 4u), true);
  Geometry patch = Cache::CreateGridGeometry(Uint16Pair(3u, 3u), false);
  Geometry border = Cache::CreateBorderGeometry(Uint16Pair(5u, 4u));
  DALI_TEST_CHECK(normalized);
  DALI_TEST_CHECK(patch);
  DALI_TEST_CHECK(border);
  DALI_TEST_EQUALS(normalized.GetType(), Geometry::TRIANGLE_STRIP, TEST_LOCATION);
  DALI_TEST_CHECK(normalized.GetNumberOfVertexBuffers() > 0u);

  Geometry quad = cache.GetGeometry(Cache::QUAD_GEOMETRY);
  DALI_TEST_CHECK(quad);
  DALI_TEST_CHECK(cache.GetGeometry(Cache::QUAD_GEOMETRY) == quad);
  cache.SaveGeometry(Cache::NINE_PATCH_GEOMETRY, patch);
  DALI_TEST_CHECK(cache.GetGeometry(Cache::NINE_PATCH_GEOMETRY) == patch);

  const std::string source(__FILE__);
  const std::size_t marker = source.find("/automated-tests/");
  DALI_TEST_CHECK(marker != std::string::npos);
  const std::string root = source.substr(0u, marker);
  std::string defaultUrl = root + "/samples/text/res/cursor_handle.png";
  const std::string patchUrl = root + "/samples/image-view/res/button-up-1.9.png";

  Shader shader = Shader::New("void main() { gl_Position = vec4(0.0); }",
                              "void main() { gl_FragColor = vec4(1.0); }");
  VisualRenderer renderer = VisualRenderer::New(quad, shader);
  cache.SetBrokenImageUrl(defaultUrl, {Dali::Integration::ToDaliString(root + "/missing-broken-image.png")});
  cache.UpdateBrokenImageRenderer(renderer, Vector2(48.0f, 48.0f), false);
  DALI_TEST_CHECK(cache.mUseDefaultBrokenImageOnly);
  DALI_TEST_CHECK(renderer.GetTextures().GetTexture(0u));

  cache.SetBrokenImageUrl(defaultUrl, {Dali::Integration::ToDaliString(patchUrl)});
  cache.UpdateBrokenImageRenderer(renderer, Vector2(36.0f, 36.0f), false);
  DALI_TEST_CHECK(cache.GetBrokenImageVisualType(0) == Dali::Ui::Internal::VisualUrl::N_PATCH);
  DALI_TEST_CHECK(renderer.GetGeometry());
  DALI_TEST_CHECK(renderer.GetShader());
  DALI_TEST_CHECK(renderer.GetTextures().GetTexture(0u));
  END_TEST;
}

int UtcDaliNPatchAuxiliaryImageLifecycleP(void)
{
  UiTestApplication application;
  Property::Map properties;
  properties.Insert(Dali::Ui::Integration::Visual::Property::TYPE,
                    Dali::Ui::Integration::InternalVisualType::N_PATCH);
  properties.Insert(Dali::Ui::Integration::ImageVisual::Property::URL,
                    "../samples/image-view/res/button-up-1.9.png");
  properties.Insert(Dali::Ui::Integration::ImageVisual::Property::AUXILIARY_IMAGE,
                    "../samples/image-view/res/sample.jpg");
  properties.Insert(Dali::Ui::Integration::ImageVisual::Property::AUXILIARY_IMAGE_ALPHA, 0.7f);
  properties.Insert(Dali::Ui::Integration::ImageVisual::Property::SYNCHRONOUS_LOADING, true);

  auto factory = Dali::Ui::Integration::VisualFactory::Get();
  Dali::Ui::Integration::Visual::Base visual = factory.CreateVisual(properties);
  DALI_TEST_CHECK(visual);

  Dali::Ui::View view = Dali::Ui::View::New();
  view.SetRequestedWidth(160.0f);
  view.SetRequestedHeight(100.0f);
  auto& viewData = Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(view));
  viewData.RegisterVisual(Dali::Ui::Integration::View::Property::BACKGROUND, visual);
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(view.IsResourceReady());
  DALI_TEST_CHECK(view.GetRendererCount() > 0u);
  DALI_TEST_CHECK(view.GetRendererAt(0u).GetTextures());

  Property::Map result;
  visual.CreatePropertyMap(result);
  DALI_TEST_CHECK(result.Find(Dali::Ui::Integration::ImageVisual::Property::AUXILIARY_IMAGE));
  float alpha = 0.0f;
  DALI_TEST_CHECK(result.Find(Dali::Ui::Integration::ImageVisual::Property::AUXILIARY_IMAGE_ALPHA)->Get(alpha));
  DALI_TEST_EQUALS(alpha, 0.7f, TEST_LOCATION);

  application.GetScene().Remove(view);
  END_TEST;
}

int UtcDaliSvgLoaderSharedAsynchronousRequestsP(void)
{
  UiTestApplication application;
  auto& loader = GetFactoryCache().GetSvgLoader();
  const std::string source(__FILE__);
  const std::size_t marker = source.find("/automated-tests/");
  DALI_TEST_CHECK(marker != std::string::npos);
  const auto url = Dali::Ui::Internal::VisualUrl(source.substr(0u, marker) + "/samples/image-view/res/svg-symbol.svg");

  class Observer : public Dali::Ui::Internal::SvgLoaderObserver
  {
  public:
    void LoadComplete(int32_t id, Dali::VectorImageRenderer renderer) override
    {
      ++loads;
      loadId = id;
      loaded = static_cast<bool>(renderer);
    }

    void RasterizeComplete(int32_t id, Dali::TextureSet texture) override
    {
      ++rasterizations;
      rasterId = id;
      rasterized = static_cast<bool>(texture);
    }

    int loads{0};
    int rasterizations{0};
    int32_t loadId{-1};
    int32_t rasterId{-1};
    bool loaded{false};
    bool rasterized{false};
  };

  Observer first;
  Observer second;
  const auto firstLoad = loader.Load(url, &first, false);
  const auto secondLoad = loader.Load(url, &second, false);
  DALI_TEST_EQUALS(firstLoad, secondLoad, TEST_LOCATION);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  DALI_TEST_EQUALS(first.loads, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(second.loads, 1, TEST_LOCATION);
  DALI_TEST_CHECK(first.loaded);
  DALI_TEST_CHECK(second.loaded);

  const auto firstRaster = loader.Rasterize(firstLoad, 48u, 32u, &first, false);
  const auto secondRaster = loader.Rasterize(secondLoad, 48u, 32u, &second, false);
  DALI_TEST_EQUALS(firstRaster, secondRaster, TEST_LOCATION);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  DALI_TEST_EQUALS(first.rasterizations, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(second.rasterizations, 1, TEST_LOCATION);
  DALI_TEST_CHECK(first.rasterized);
  DALI_TEST_CHECK(second.rasterized);

  loader.RequestRasterizeRemove(firstRaster, &first, false);
  loader.RequestRasterizeRemove(secondRaster, &second, false);
  loader.RequestLoadRemove(firstLoad, &first);
  loader.RequestLoadRemove(secondLoad, &second);
  application.SendNotification();
  END_TEST;
}

int UtcDaliTextureManagerSharedAsynchronousRequestsP(void)
{
  UiTestApplication application;
  auto& textureManager = GetFactoryCache().GetTextureManager();

  class Observer : public Dali::Ui::TextureUploadObserver
  {
  public:
    void LoadComplete(bool success, TextureInformation information) override
    {
      ++loadCount;
      loaded = success;
      lastId = information.textureId;
      hasTexture = static_cast<bool>(information.textureSet);
    }

    int loadCount{0};
    int32_t lastId{-1};
    bool loaded{false};
    bool hasTexture{false};
  };

  Observer first;
  Observer second;
  auto preMultiply = Dali::Ui::Internal::TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  const auto url = Dali::Ui::Internal::VisualUrl("../samples/image-view/res/sample.jpg");
  const auto firstId = textureManager.RequestLoad(url, Dali::ImageDimensions(64u, 48u),
                                                   Dali::SamplingMode::BOX_THEN_LINEAR, &first, true,
                                                   Dali::Ui::Internal::TextureManager::ReloadPolicy::CACHED,
                                                   preMultiply);
  const auto secondId = textureManager.RequestLoad(url, Dali::ImageDimensions(64u, 48u),
                                                    Dali::SamplingMode::BOX_THEN_LINEAR, &second, true,
                                                    Dali::Ui::Internal::TextureManager::ReloadPolicy::CACHED,
                                                    preMultiply);
  DALI_TEST_EQUALS(firstId, secondId, TEST_LOCATION);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  DALI_TEST_EQUALS(first.loadCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(second.loadCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(first.loaded && first.hasTexture);
  DALI_TEST_CHECK(second.loaded && second.hasTexture);
  DALI_TEST_EQUALS(first.lastId, firstId, TEST_LOCATION);

  const auto reloadedId = textureManager.RequestLoad(url, Dali::ImageDimensions(64u, 48u),
                                                      Dali::SamplingMode::BOX_THEN_LINEAR, &first, true,
                                                      Dali::Ui::Internal::TextureManager::ReloadPolicy::FORCED,
                                                      preMultiply);
  DALI_TEST_EQUALS(reloadedId, firstId, TEST_LOCATION);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  DALI_TEST_EQUALS(first.loadCount, 2, TEST_LOCATION);
  DALI_TEST_CHECK(first.loaded && first.hasTexture);

  Observer missing;
  const auto missingId = textureManager.RequestLoad(
    Dali::Ui::Internal::VisualUrl("../samples/image-view/res/does-not-exist.jpg"), Dali::ImageDimensions(),
    Dali::SamplingMode::BOX_THEN_LINEAR, &missing, true,
    Dali::Ui::Internal::TextureManager::ReloadPolicy::CACHED, preMultiply);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  DALI_TEST_EQUALS(missing.loadCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!missing.loaded);

  textureManager.RequestRemove(firstId, &first);
  textureManager.RequestRemove(secondId, &second);
  textureManager.RequestRemove(reloadedId, &first);
  textureManager.RequestRemove(missingId, &missing);
  application.SendNotification();
  END_TEST;
}

int UtcDaliTextureManagerAsyncPreappliedMaskP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextureManager textureManager;
  class Observer : public Dali::Ui::TextureUploadObserver
  {
  public:
    void LoadComplete(bool success, TextureInformation information) override
    {
      ++count;
      loaded = success;
      texture = information.textureSet;
    }

    int count{0};
    bool loaded{false};
    Dali::TextureSet texture;
  } observer;

  Dali::Ui::Internal::TextureManager::MaskingDataPointer maskInfo(
    new Dali::Ui::Internal::TextureManager::MaskingData());
  maskInfo->mAlphaMaskUrl = Dali::Ui::Internal::VisualUrl("../samples/visual-base/res/mask.png");
  maskInfo->mPreappliedMasking = true;
  maskInfo->mCropToMask = true;
  maskInfo->mContentScaleFactor = 1.0f;

  auto textureId = Dali::Ui::Internal::TextureManager::INVALID_TEXTURE_ID;
  bool loading = false;
  auto preMultiply = Dali::Ui::Internal::TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  Dali::TextureSet immediate = textureManager.LoadTexture(
    Dali::Ui::Internal::VisualUrl("../samples/image-view/res/sample.jpg"),
    Dali::ImageDimensions(64u, 48u), Dali::SamplingMode::BOX_THEN_LINEAR,
    maskInfo, false, textureId, loading, &observer, true,
    Dali::Ui::Internal::TextureManager::ReloadPolicy::FORCED, preMultiply);
  DALI_TEST_CHECK(!immediate);
  DALI_TEST_CHECK(loading);
  DALI_TEST_CHECK(textureId != Dali::Ui::Internal::TextureManager::INVALID_TEXTURE_ID);
  DALI_TEST_CHECK(maskInfo->mAlphaMaskId != Dali::Ui::Internal::TextureManager::INVALID_TEXTURE_ID);

  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(2, 5, true));
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(observer.count, 1, TEST_LOCATION);
  DALI_TEST_CHECK(observer.loaded && observer.texture);
  textureManager.RequestRemove(textureId, &observer);
  textureManager.RequestRemove(maskInfo->mAlphaMaskId, nullptr);
  END_TEST;
}

int UtcDaliTextureManagerSynchronousPixelBufferP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::TextureManager textureManager;
  auto preMultiply = Dali::Ui::Internal::TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  Dali::PixelBuffer image = textureManager.LoadPixelBuffer(
    Dali::Ui::Internal::VisualUrl("../samples/image-view/res/sample.jpg"),
    Dali::ImageDimensions(32u, 24u), Dali::SamplingMode::BOX_THEN_LINEAR,
    true, nullptr, true, preMultiply);
  DALI_TEST_CHECK(image);
  DALI_TEST_EQUALS(image.GetWidth(), 32u, TEST_LOCATION);
  DALI_TEST_CHECK(image.GetHeight() >= 24u);

  Dali::PixelBuffer missing = textureManager.LoadPixelBuffer(
    Dali::Ui::Internal::VisualUrl("../samples/image-view/res/does-not-exist.jpg"),
    Dali::ImageDimensions(), Dali::SamplingMode::BOX_THEN_LINEAR,
    true, nullptr, true, preMultiply);
  DALI_TEST_CHECK(!missing);
  END_TEST;
}

int UtcDaliFixedImageCacheReadyAndFailedFramesP(void)
{
  UiTestApplication application;
  auto& textureManager = GetFactoryCache().GetTextureManager();
  auto textureId = AddUploadedTexture(textureManager, "fixed-first.png", 1);
  Dali::Ui::Internal::ImageCache::UrlList urls(1u);
  urls[0].mUrl = Dali::Ui::Internal::VisualUrl("fixed-first.png");
  Dali::Ui::Internal::TextureManager::MaskingDataPointer maskingData;
  TestFrameReadyObserver observer;
  Dali::Ui::Internal::FixedImageCache cache(textureManager,
                                             Dali::ImageDimensions(),
                                             Dali::SamplingMode::BOX_THEN_LINEAR,
                                             urls,
                                             maskingData,
                                             observer,
                                             1u,
                                             42u,
                                             false);
  DALI_TEST_EQUALS(cache.GetTotalFrameCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(cache.GetCurrentFrameIndex(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(cache.GetFrameInterval(0u), 42u, TEST_LOCATION);
  DALI_TEST_CHECK(!cache.Frame(2u));

  cache.mReadyFlags.push_back(false);
  cache.mRequestingLoad = true;
  Dali::Ui::TextureUploadObserver::TextureInformation ready(
    Dali::Ui::TextureUploadObserver::ReturnType::TEXTURE,
    textureId,
    Dali::TextureSet::New(),
    true);
  cache.LoadComplete(true, ready);
  DALI_TEST_EQUALS(observer.callCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.lastInterval, 42u, TEST_LOCATION);
  DALI_TEST_CHECK(observer.lastPremultiplied);
  cache.FirstFrame();
  cache.Frame(0u);

  cache.ClearCache();
  DALI_TEST_EQUALS(cache.mReadyFlags.size(), 0u, TEST_LOCATION);
  cache.LoadComplete(false, ready);
  DALI_TEST_EQUALS(observer.callCount, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.lastInterval, 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!observer.lastPremultiplied);
  END_TEST;
}

int UtcDaliRollingAnimatedImageCacheCompletionTransitionsP(void)
{
  UiTestApplication application;
  auto& textureManager = GetFactoryCache().GetTextureManager();
  auto animation = Dali::AnimatedImageLoading::New("../samples/image-view/res/dali-logo-anim.gif", true);
  DALI_TEST_CHECK(animation);
  Dali::Ui::Internal::TextureManager::MaskingDataPointer maskingData;
  TestFrameReadyObserver observer;
  Dali::Ui::Internal::RollingAnimatedImageCache cache(textureManager,
                                                      Dali::ImageDimensions(),
                                                      Dali::SamplingMode::BOX_THEN_LINEAR,
                                                      animation,
                                                      maskingData,
                                                      observer,
                                                      2u,
                                                      1u,
                                                      Dali::WrapMode::DEFAULT,
                                                      Dali::WrapMode::DEFAULT,
                                                      false,
                                                      false);
  DALI_TEST_EQUALS(cache.GetCurrentFrameIndex(), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(cache.GetTotalFrameCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(cache.GetFrameInterval(4u), 0u, TEST_LOCATION);

  cache.mTextureIds[0] = AddUploadedTexture(textureManager, "rolling-first.png", 1);
  cache.mQueue.PushBack({0u, false});
  auto textureSet = Dali::TextureSet::New();
  cache.SetImageFrameReady(Dali::Ui::Internal::TextureManager::INVALID_TEXTURE_ID);
  DALI_TEST_CHECK(!cache.IsFrontReady());
  cache.SetImageFrameReady(cache.mTextureIds[0]);
  DALI_TEST_CHECK(cache.IsFrontReady());
  cache.mQueue.Front().mReady = false;
  cache.MakeFrameReady(true, textureSet, 3u, 42u, false);
  DALI_TEST_EQUALS(cache.GetTotalFrameCount(), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(cache.GetCurrentFrameIndex(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(cache.GetFrameInterval(0u), 42u, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.callCount, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(observer.lastTextureSet);
  DALI_TEST_CHECK(cache.GetFrontTextureSet());
  DALI_TEST_EQUALS(cache.GetCachedTextureId(0u), cache.mTextureIds[0], TEST_LOCATION);

  cache.mTextureIds[1] = AddUploadedTexture(textureManager, "rolling-second.png", 1);
  cache.mQueue.PushBack({1u, false});
  cache.mPendingFrameIndex = 1;
  cache.MakeFrameReady(true, textureSet, 3u, 55u, true);
  DALI_TEST_EQUALS(cache.GetCurrentFrameIndex(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(cache.GetFrameInterval(1u), 55u, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.callCount, 2u, TEST_LOCATION);
  DALI_TEST_CHECK(observer.lastPremultiplied);
  cache.MakeFrameReady(false, Dali::TextureSet(), 3u, 0u, false);
  DALI_TEST_EQUALS(observer.callCount, 3u, TEST_LOCATION);
  DALI_TEST_CHECK(!observer.lastTextureSet);
  cache.ClearCache();
  DALI_TEST_EQUALS(cache.GetCurrentFrameIndex(), -1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRollingAnimatedImageCacheSynchronousGifFramesP(void)
{
  UiTestApplication application;
  auto& textureManager = GetFactoryCache().GetTextureManager();
  auto animation = Dali::AnimatedImageLoading::New("../samples/image-view/res/dali-logo-anim.gif", true);
  DALI_TEST_CHECK(animation);
  Dali::Ui::Internal::TextureManager::MaskingDataPointer maskingData;
  TestFrameReadyObserver observer;
  Dali::Ui::Internal::RollingAnimatedImageCache cache(textureManager,
                                                      Dali::ImageDimensions(),
                                                      Dali::SamplingMode::BOX_THEN_LINEAR,
                                                      animation,
                                                      maskingData,
                                                      observer,
                                                      2u,
                                                      1u,
                                                      Dali::WrapMode::REPEAT,
                                                      Dali::WrapMode::REPEAT,
                                                      true,
                                                      true);
  Dali::TextureSet first = cache.FirstFrame();
  DALI_TEST_CHECK(first);
  DALI_TEST_CHECK(cache.GetTotalFrameCount() > 1);
  DALI_TEST_EQUALS(cache.GetCurrentFrameIndex(), 0, TEST_LOCATION);
  DALI_TEST_CHECK(cache.GetFrameInterval(0u) > 0u);
  cache.ClearCache();
  DALI_TEST_EQUALS(cache.GetCurrentFrameIndex(), -1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRollingAnimatedImageCachePendingJumpP(void)
{
  UiTestApplication application;
  auto& textureManager = GetFactoryCache().GetTextureManager();
  auto animation = Dali::AnimatedImageLoading::New("../samples/image-view/res/dali-logo-anim.gif", true);
  Dali::Ui::Internal::TextureManager::MaskingDataPointer maskingData;
  TestFrameReadyObserver observer;
  Dali::Ui::Internal::RollingAnimatedImageCache cache(textureManager, Dali::ImageDimensions(),
                                                      Dali::SamplingMode::BOX_THEN_LINEAR, animation,
                                                      maskingData, observer, 2u, 1u, Dali::WrapMode::DEFAULT,
                                                      Dali::WrapMode::DEFAULT, false, false);
  cache.mTextureIds[0] = AddUploadedTexture(textureManager, "rolling-pending.png", 1);
  cache.mQueue.PushBack({0u, false});
  DALI_TEST_CHECK(!cache.Frame(2u));
  DALI_TEST_EQUALS(cache.mPendingFrameIndex, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(cache.mLoadWaitingQueue.size(), 1u, TEST_LOCATION);
  cache.MakeFrameReady(true, Dali::TextureSet::New(), 3u, 40u, false);
  DALI_TEST_EQUALS(cache.GetCurrentFrameIndex(), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.callCount, 0u, TEST_LOCATION);
  cache.ClearCache();
  END_TEST;
}

int UtcDaliRollingAnimatedImageCacheCallbackStartsNextFrameP(void)
{
  UiTestApplication application;
  auto& textureManager = GetFactoryCache().GetTextureManager();
  auto animation = Dali::AnimatedImageLoading::New("../samples/image-view/res/dali-logo-anim.gif", true);
  Dali::Ui::Internal::TextureManager::MaskingDataPointer maskingData;
  TestFrameReadyObserver observer;
  Dali::Ui::Internal::RollingAnimatedImageCache cache(textureManager, Dali::ImageDimensions(),
                                                      Dali::SamplingMode::BOX_THEN_LINEAR, animation,
                                                      maskingData, observer, 2u, 1u, Dali::WrapMode::REPEAT,
                                                      Dali::WrapMode::REPEAT, false, false);
  cache.mTextureIds[0] = AddUploadedTexture(textureManager, "rolling-callback.png", 1);
  cache.mQueue.PushBack({0u, false});
  cache.mLoadWaitingQueue.push_back(1u);
  Dali::Ui::TextureUploadObserver::TextureInformation ready(
    Dali::Ui::TextureUploadObserver::ReturnType::ANIMATED_IMAGE_TEXTURE,
    cache.mTextureIds[0], Dali::TextureSet::New(), 3u, 45u, false);
  cache.LoadComplete(true, ready);
  DALI_TEST_EQUALS(observer.callCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(cache.GetFrameInterval(0u), 45u, TEST_LOCATION);
  DALI_TEST_CHECK(cache.mLoadWaitingQueue.empty());
  DALI_TEST_EQUALS(cache.GetCurrentFrameIndex(), 0, TEST_LOCATION);
  cache.ClearCache();
  END_TEST;
}

int UtcDaliTextureCacheExternalResourceLifecycleP(void)
{
  UiTestApplication application;
  using Dali::Ui::Internal::VisualUrl;
  auto& cache = GetFactoryCache().GetTextureManager().mTextureCacheManager;
  TextureSet firstSet = TextureSet::New();
  TextureSet secondSet = TextureSet::New();
  Texture firstTexture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 2u, 2u);
  Texture secondTexture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 2u, 2u);
  firstSet.SetTexture(0u, firstTexture);
  secondSet.SetTexture(0u, secondTexture);
  VisualUrl firstUrl(cache.AddExternalTexture(firstSet, false));
  VisualUrl secondUrl(cache.AddExternalTexture(secondSet, true));
  Dali::Ui::Internal::TextureManager::TextureId firstId = 0;
  Dali::Ui::Internal::TextureManager::TextureId secondId = 0;
  DALI_TEST_CHECK(firstUrl.GetLocationAsInteger(firstId));
  DALI_TEST_CHECK(secondUrl.GetLocationAsInteger(secondId));
  DALI_TEST_CHECK(cache.GetTexture(firstId, 0u) == firstTexture);
  DALI_TEST_CHECK(!cache.GetTexture(firstId, 1u));
  DALI_TEST_CHECK(cache.GetTextureState(firstId) == Dali::Ui::Internal::TextureManager::LoadState::UPLOADED);
  DALI_TEST_CHECK(cache.GetVisualUrl(firstId).GetUrl() == firstUrl.GetUrl());
  cache.UseExternalResource(secondUrl);
  DALI_TEST_CHECK(cache.RemoveExternalTexture(firstUrl) == firstSet);
  DALI_TEST_CHECK(cache.GetTexture(secondId, 0u) == secondTexture);
  DALI_TEST_CHECK(cache.RemoveExternalTexture(secondUrl) == secondSet);
  DALI_TEST_CHECK(cache.RemoveExternalTexture(secondUrl) == secondSet);
  DALI_TEST_CHECK(!cache.RemoveExternalTexture(secondUrl));

  Dali::EncodedImageBuffer::RawBufferType raw;
  raw.PushBack(0x11u);
  raw.PushBack(0x22u);
  raw.PushBack(0x33u);
  EncodedImageBuffer buffer = EncodedImageBuffer::New(std::move(raw));
  VisualUrl bufferUrl(cache.AddEncodedImageBuffer(buffer));
  VisualUrl duplicateUrl(cache.AddEncodedImageBuffer(buffer));
  DALI_TEST_CHECK(bufferUrl.GetUrl() == duplicateUrl.GetUrl());
  Dali::Ui::Internal::TextureManager::TextureId bufferId = 0;
  DALI_TEST_CHECK(bufferUrl.GetLocationAsInteger(bufferId));
  DALI_TEST_CHECK(cache.GetEncodedImageBuffer(bufferId) == buffer);
  DALI_TEST_CHECK(cache.GetEncodedImageBuffer(bufferUrl) == buffer);
  DALI_TEST_CHECK(cache.GetVisualUrl(bufferId).GetUrl() == bufferUrl.GetUrl());
  cache.UseExternalResource(bufferUrl);
  DALI_TEST_CHECK(cache.RemoveEncodedImageBuffer(bufferUrl) == buffer);
  DALI_TEST_CHECK(cache.RemoveEncodedImageBuffer(bufferUrl) == buffer);
  DALI_TEST_CHECK(cache.RemoveEncodedImageBuffer(bufferUrl) == buffer);
  DALI_TEST_CHECK(!cache.RemoveEncodedImageBuffer(bufferUrl));
  END_TEST;
}

int UtcDaliSvgVisualPropertyAndCompletionMatrixP(void)
{
  UiTestApplication application;
  auto factory = Dali::Ui::Integration::VisualFactory::Get();
  auto& factoryImpl = Dali::Ui::GetImplementation(factory);
  auto& cache = factoryImpl.GetFactoryCache();
  auto& shaderFactory = factoryImpl.GetImageVisualShaderFactory();
  using Dali::Ui::Internal::SvgVisual;
  using Dali::Ui::Internal::SvgVisualPtr;
  using Dali::Ui::Internal::VisualUrl;
  SvgVisualPtr visual(new SvgVisual(cache, shaderFactory, Dali::Ui::Integration::VisualFactory::NONE,
                                    VisualUrl("missing-svg-visual-test.svg"), ImageDimensions()));
  Property::Map properties;
  properties.Insert(Dali::Ui::Integration::ImageVisual::Property::DESIRED_WIDTH, 48);
  properties.Insert("desiredHeight", 32);
  properties.Insert(Dali::Ui::Integration::ImageVisual::Property::SYNCHRONOUS_LOADING, true);
  properties.Insert("loadPolicy", "ATTACHED");
  properties.Insert("releasePolicy", "NEVER");
  properties.Insert("fittingMode", "CENTER");
  visual->DoSetProperties(properties);
  DALI_TEST_EQUALS(visual->mDesiredSize.GetWidth(), 48u, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mDesiredSize.GetHeight(), 32u, TEST_LOCATION);
  DALI_TEST_CHECK(visual->IsSynchronousLoadingRequired());

  Property::Map result;
  visual->DoCreatePropertyMap(result);
  DALI_TEST_CHECK(result.Count() >= 7u);
  Property::Map instance;
  instance.Insert(1, 2);
  visual->DoCreateInstancePropertyMap(instance);
  DALI_TEST_EQUALS(instance.Count(), 1u, TEST_LOCATION);
  Vector2 natural = Vector2::ZERO;
  visual->GetNaturalSize(natural);
  DALI_TEST_EQUALS(natural, Vector2(48.0f, 32.0f), TEST_LOCATION);

  visual->DoSetProperty(Dali::Ui::Integration::ImageVisual::Property::DESIRED_WIDTH, "bad");
  visual->DoSetProperty(Dali::Ui::Integration::ImageVisual::Property::SYNCHRONOUS_LOADING, "bad");
  visual->DoSetProperty(Dali::Ui::Integration::ImageVisual::Property::SYNCHRONOUS_LOADING, false);
  visual->DoSetProperty(Dali::Ui::Integration::ImageVisual::Property::LOAD_POLICY, "IMMEDIATE");
  visual->DoSetProperty(Dali::Ui::Integration::ImageVisual::Property::RELEASE_POLICY, "DETACHED");
  visual->DoSetProperty(Dali::Ui::Integration::ImageVisual::Property::FITTING_MODE, "FILL");
  visual->LoadComplete(1, Dali::VectorImageRenderer());
  DALI_TEST_CHECK(visual->mLoadFailed);
  visual->mSvgLoadId = Dali::Ui::Internal::SvgLoader::INVALID_SVG_LOAD_ID;
  visual->RasterizeComplete(1, TextureSet());
  visual->mSvgRasterizeId = Dali::Ui::Internal::SvgLoader::INVALID_SVG_RASTERIZE_ID;
  Property::Map shader;
  shader.Insert("vertexShader", "void main() { gl_Position = vec4(0.0); }");
  shader.Insert("fragmentShader", "void main() { gl_FragColor = vec4(1.0); }");
  Property::Map configured;
  configured.Insert(Dali::Ui::Integration::Visual::Property::SHADER, shader);
  configured.Insert(Dali::Ui::Integration::ImageVisual::Property::SYNCHRONOUS_LOADING, true);
  SvgVisualPtr sceneVisual = SvgVisual::New(cache, shaderFactory, Dali::Ui::Integration::VisualFactory::NONE,
                                            VisualUrl(std::string(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR) + "/initial-layout.svg"), configured);
  DALI_TEST_CHECK(sceneVisual->GenerateShader());
  sceneVisual->UpdateShader();
  Actor actor = Actor::New();
  application.GetScene().Add(actor);
  sceneVisual->DoSetOnScene(actor);
  sceneVisual->OnSetTransform();
  sceneVisual->DoSetOffScene(actor);
  Property::Map failedProperties;
  failedProperties.Insert(Dali::Ui::Integration::ImageVisual::Property::SYNCHRONOUS_LOADING, true);
  SvgVisualPtr failedVisual = SvgVisual::New(cache, shaderFactory, Dali::Ui::Integration::VisualFactory::NONE,
                                             VisualUrl("missing-svg-visual-test.svg"), failedProperties);
  Actor failedActor = Actor::New();
  failedActor.SetProperty(Actor::Property::SIZE, Vector2(24.0f, 24.0f));
  application.GetScene().Add(failedActor);
  failedVisual->DoSetOnScene(failedActor);
  failedVisual->DoSetOffScene(failedActor);
  END_TEST;
}

int UtcDaliNPatchVisualBorderAndSceneMatrixP(void)
{
  UiTestApplication application;
  auto factory = Dali::Ui::Integration::VisualFactory::Get();
  auto& factoryImpl = Dali::Ui::GetImplementation(factory);
  auto& cache = factoryImpl.GetFactoryCache();
  auto& shaderFactory = factoryImpl.GetImageVisualShaderFactory();
  using Dali::Ui::Internal::NPatchVisual;
  using Dali::Ui::Internal::NPatchVisualPtr;
  using Dali::Ui::Internal::VisualUrl;
  const VisualUrl url("../samples/image-view/res/button-up-1.9.png");

  for(bool borderOnly : {false, true})
  {
    Property::Map properties;
    properties.Insert(Dali::Ui::Integration::ImageVisual::Property::BORDER_ONLY, borderOnly);
    properties.Insert(Dali::Ui::Integration::ImageVisual::Property::BORDER, Rect<int32_t>(2, 2, 2, 2));
    properties.Insert(Dali::Ui::Integration::ImageVisual::Property::SYNCHRONOUS_LOADING, true);
    properties.Insert(Dali::Ui::Integration::ImageVisual::Property::PRE_MULTIPLIED_ALPHA, true);
    properties.Insert(Dali::Ui::Integration::ImageVisual::Property::RELEASE_POLICY, Dali::Ui::Image::ReleasePolicy::DETACHED);
    if(borderOnly)
    {
      properties.Insert(Dali::Ui::Integration::ImageVisual::Property::AUXILIARY_IMAGE,
                        Dali::String("../samples/image-view/res/sample.jpg"));
      properties.Insert(Dali::Ui::Integration::ImageVisual::Property::AUXILIARY_IMAGE_ALPHA, 0.5f);
      Property::Map shader;
      shader.Insert("vertexShader", "void main() { gl_Position = vec4(0.0); }");
      shader.Insert("fragmentShader", "void main() { gl_FragColor = vec4(1.0); }");
      properties.Insert(Dali::Ui::Integration::Visual::Property::SHADER, shader);
    }
    NPatchVisualPtr visual = NPatchVisual::New(cache, shaderFactory, Dali::Ui::Integration::VisualFactory::NONE,
                                               url, properties);
    DALI_TEST_CHECK(visual->mBorderOnly == borderOnly);
    Property::Map map;
    visual->DoCreatePropertyMap(map);
    DALI_TEST_CHECK(map.Find(Dali::Ui::Integration::ImageVisual::Property::BORDER));
    Property::Map instance;
    visual->DoCreateInstancePropertyMap(instance);
    DALI_TEST_EQUALS(instance.Count(), borderOnly ? 2u : 0u, TEST_LOCATION);
    Vector2 natural = Vector2::ZERO;
    visual->GetNaturalSize(natural);

    Actor actor = Actor::New();
    actor.SetProperty(Actor::Property::SIZE, Vector2(80.0f, 60.0f));
    application.GetScene().Add(actor);
    visual->DoSetOnScene(actor);
    if(borderOnly)
    {
      TextureSet auxiliary = TextureSet::New();
      auxiliary.SetTexture(0u, Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 2u, 2u));
      using Observer = Dali::Ui::TextureUploadObserver;
      Observer::TextureInformation success(Observer::ReturnType::TEXTURE, visual->mAuxiliaryTextureId,
                                           auxiliary, false);
      visual->LoadComplete(true, success);
      DALI_TEST_CHECK(visual->mAuxiliaryResourceStatus == Dali::Ui::Visual::ResourceStatus::READY);
      Observer::TextureInformation failure(Observer::ReturnType::TEXTURE, visual->mAuxiliaryTextureId,
                                           TextureSet(), false);
      visual->LoadComplete(false, failure);
      DALI_TEST_CHECK(visual->mAuxiliaryResourceStatus == Dali::Ui::Visual::ResourceStatus::FAILED);
    }
    Dali::Ui::Internal::NPatchDataPtr patchData;
    DALI_TEST_CHECK(visual->mLoader.GetNPatchData(visual->mId, patchData));
    if(patchData && patchData->GetLoadingState() == Dali::Ui::Internal::NPatchData::LoadingState::LOAD_COMPLETE)
    {
      auto originalX = patchData->GetStretchPixelsX();
      auto originalY = patchData->GetStretchPixelsY();
      Dali::Ui::Integration::NPatchUtility::StretchRanges ranges;
      ranges.PushBack(Uint16Pair(0u, 1u));
      ranges.PushBack(Uint16Pair(2u, 3u));
      patchData->SetStretchPixelsX(ranges);
      patchData->SetStretchPixelsY(ranges);
      DALI_TEST_CHECK(visual->CreateGeometry());
      DALI_TEST_CHECK(visual->CreateShader());
      patchData->SetStretchPixelsX(originalX);
      patchData->SetStretchPixelsY(originalY);
    }
    DALI_TEST_CHECK(visual->CreateGeometry());
    DALI_TEST_CHECK(visual->CreateShader());
    visual->UpdateShader();
    visual->OnSetTransform();
    visual->SetFittingMode(Dali::Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
    visual->OnApplyFittingMode(Vector2(80.0f, 60.0f), Insets(), 1.0f);
    visual->EnablePreMultipliedAlpha(false);
    visual->EnablePreMultipliedAlpha(true);
    visual->DoSetOffScene(actor);
  }
  END_TEST;
}

int UtcDaliSvgLoaderReentrantLoadAndRasterizeP(void)
{
  UiTestApplication application;
  auto& loader = GetFactoryCache().GetSvgLoader();
  const std::string source(__FILE__);
  const auto marker = source.find("/automated-tests/");
  DALI_TEST_CHECK(marker != std::string::npos);
  const auto resourceRoot = source.substr(0u, marker) + "/samples/image-view/res/";
  const Dali::Ui::Internal::VisualUrl firstUrl(resourceRoot + "svg-blocks.svg");
  const Dali::Ui::Internal::VisualUrl secondUrl(resourceRoot + "svg-symbol.svg");

  class Observer : public Dali::Ui::Internal::SvgLoaderObserver
  {
  public:
    Observer(Dali::Ui::Internal::SvgLoader& loader, const Dali::Ui::Internal::VisualUrl& firstUrl,
             const Dali::Ui::Internal::VisualUrl& secondUrl)
    : loader(loader),
      firstUrl(firstUrl),
      secondUrl(secondUrl)
    {
    }

    void LoadComplete(int32_t, Dali::VectorImageRenderer renderer) override
    {
      ++loadCount;
      loaded = static_cast<bool>(renderer);
      if(queueNextLoad)
      {
        queueNextLoad = false;
        queuedLoad = loader.Load(secondUrl, this, false);
        duplicateLoad = loader.Load(firstUrl, this, false);
        {
          Observer discarded(loader, firstUrl, secondUrl);
          discardedLoad = loader.Load(firstUrl, &discarded, false);
        }
      }
    }

    void RasterizeComplete(int32_t, Dali::TextureSet textureSet) override
    {
      ++rasterCount;
      rasterized = static_cast<bool>(textureSet);
      if(queueNextRaster)
      {
        queueNextRaster = false;
        queuedRaster = loader.Rasterize(firstLoad, 64u, 48u, this, false);
        duplicateRaster = loader.Rasterize(firstLoad, 32u, 32u, this, false);
        {
          Observer discarded(loader, firstUrl, secondUrl);
          discardedRaster = loader.Rasterize(firstLoad, 32u, 32u, &discarded, false);
        }
      }
    }

    Dali::Ui::Internal::SvgLoader& loader;
    Dali::Ui::Internal::VisualUrl firstUrl;
    Dali::Ui::Internal::VisualUrl secondUrl;
    int32_t firstLoad{-1};
    int32_t queuedLoad{-1};
    int32_t duplicateLoad{-1};
    int32_t discardedLoad{-1};
    int32_t queuedRaster{-1};
    int32_t duplicateRaster{-1};
    int32_t discardedRaster{-1};
    unsigned int loadCount{0u};
    unsigned int rasterCount{0u};
    bool loaded{false};
    bool rasterized{false};
    bool queueNextLoad{true};
    bool queueNextRaster{true};
  };

  Observer observer(loader, firstUrl, secondUrl);
  observer.firstLoad = loader.Load(firstUrl, &observer, false);
  for(unsigned int attempt = 0u; attempt < 4u && observer.loadCount < 3u; ++attempt)
  {
    DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  }
  DALI_TEST_EQUALS(observer.loadCount, 3u, TEST_LOCATION);
  DALI_TEST_CHECK(observer.loaded);
  DALI_TEST_CHECK(observer.queuedLoad != observer.firstLoad);

  const auto firstRaster = loader.Rasterize(observer.firstLoad, 32u, 32u, &observer, false);
  for(unsigned int attempt = 0u; attempt < 4u && observer.rasterCount < 3u; ++attempt)
  {
    DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  }
  DALI_TEST_EQUALS(observer.rasterCount, 3u, TEST_LOCATION);
  DALI_TEST_CHECK(observer.rasterized);
  DALI_TEST_CHECK(observer.queuedRaster != firstRaster);

  loader.RequestRasterizeRemove(firstRaster, &observer, false);
  loader.RequestRasterizeRemove(observer.duplicateRaster, &observer, false);
  loader.RequestRasterizeRemove(observer.discardedRaster, nullptr, false);
  loader.RequestRasterizeRemove(observer.queuedRaster, &observer, false);
  loader.RequestLoadRemove(observer.firstLoad, &observer);
  loader.RequestLoadRemove(observer.duplicateLoad, &observer);
  loader.RequestLoadRemove(observer.discardedLoad, nullptr);
  loader.RequestLoadRemove(observer.queuedLoad, &observer);
  application.SendNotification();
  END_TEST;
}

int UtcDaliSvgLoaderFailedLoadAndObserverDestructionP(void)
{
  UiTestApplication application;
  auto& loader = GetFactoryCache().GetSvgLoader();
  const std::string source(__FILE__);
  const auto marker = source.find("/automated-tests/");
  DALI_TEST_CHECK(marker != std::string::npos);
  const auto resourceRoot = source.substr(0u, marker) + "/samples/image-view/res/";
  const Dali::Ui::Internal::VisualUrl invalidUrl(resourceRoot + "missing-image.svg");
  const Dali::Ui::Internal::VisualUrl validUrl(resourceRoot + "svg-blocks.svg");

  class Observer : public Dali::Ui::Internal::SvgLoaderObserver
  {
  public:
    void LoadComplete(int32_t, Dali::VectorImageRenderer renderer) override
    {
      ++calls;
      loaded = static_cast<bool>(renderer);
    }

    void RasterizeComplete(int32_t, Dali::TextureSet) override
    {
    }

    unsigned int calls{0u};
    bool loaded{false};
  };

  Observer failure;
  const auto failedLoad = loader.Load(invalidUrl, &failure, true);
  DALI_TEST_EQUALS(failure.calls, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!failure.loaded);
  DALI_TEST_EQUALS(loader.Rasterize(Dali::Ui::Internal::SvgLoader::INVALID_SVG_LOAD_ID,
                                    32u, 32u, &failure, true),
                   Dali::Ui::Internal::SvgLoader::INVALID_SVG_RASTERIZE_ID, TEST_LOCATION);
  loader.RequestRasterizeRemove(Dali::Ui::Internal::SvgLoader::INVALID_SVG_RASTERIZE_ID,
                                &failure, false);
  loader.RequestLoadRemove(Dali::Ui::Internal::SvgLoader::INVALID_SVG_LOAD_ID, &failure);
  const auto retriedLoad = loader.Load(invalidUrl, &failure, true);
  DALI_TEST_EQUALS(retriedLoad, failedLoad, TEST_LOCATION);
  DALI_TEST_EQUALS(failure.calls, 2u, TEST_LOCATION);
  loader.RequestLoadRemove(failedLoad, &failure);
  loader.RequestLoadRemove(retriedLoad, &failure);

  Observer* destroyed = new Observer();
  const auto pendingLoad = loader.Load(validUrl, destroyed, false);
  delete destroyed;
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  loader.RequestLoadRemove(pendingLoad, nullptr);
  application.SendNotification();
  END_TEST;
}

int UtcDaliSvgLoaderCancelPendingLoadP(void)
{
  UiTestApplication application;
  auto& loader = GetFactoryCache().GetSvgLoader();
  const std::string source(__FILE__);
  const auto marker = source.find("/automated-tests/");
  DALI_TEST_CHECK(marker != std::string::npos);
  const auto url = Dali::Ui::Internal::VisualUrl(source.substr(0u, marker) +
                                                  "/samples/image-view/res/svg-symbol.svg");

  const auto loadId = loader.Load(url, nullptr, false);
  DALI_TEST_CHECK(!loader.mLoadCache.empty());
  loader.RequestLoadRemove(loadId, nullptr);
  static_cast<Dali::Integration::Processor&>(loader).Process(true);
  DALI_TEST_EQUALS(loader.mLoadCache.front().mLoadState,
                   Dali::Ui::Internal::SvgLoader::LoadState::CANCELLED, TEST_LOCATION);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  DALI_TEST_CHECK(!loader.GetVectorImageRenderer(loadId));
  END_TEST;
}

int UtcDaliTextureManagerReentrantRequestsP(void)
{
  UiTestApplication application;
  auto& manager = GetFactoryCache().GetTextureManager();
  using TextureManager = Dali::Ui::Internal::TextureManager;
  const Dali::Ui::Internal::VisualUrl firstUrl("../samples/image-view/res/sample.jpg");
  const Dali::Ui::Internal::VisualUrl secondUrl("../samples/image-view/res/gallery-medium-3.jpg");

  class Observer : public Dali::Ui::TextureUploadObserver
  {
  public:
    Observer(TextureManager& manager, const Dali::Ui::Internal::VisualUrl& firstUrl,
             const Dali::Ui::Internal::VisualUrl& secondUrl)
    : manager(manager),
      firstUrl(firstUrl),
      secondUrl(secondUrl)
    {
    }

    void LoadComplete(bool success, TextureInformation information) override
    {
      ++count;
      loaded = success && static_cast<bool>(information.textureSet);
      if(queueMore)
      {
        queueMore = false;
        auto preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
        queuedId = manager.RequestLoad(secondUrl, Dali::ImageDimensions(64u, 48u),
                                       Dali::SamplingMode::BOX_THEN_LINEAR, this, true,
                                       TextureManager::ReloadPolicy::CACHED, preMultiply);
        duplicateId = manager.RequestLoad(firstUrl, Dali::ImageDimensions(64u, 48u),
                                          Dali::SamplingMode::BOX_THEN_LINEAR, this, true,
                                          TextureManager::ReloadPolicy::CACHED, preMultiply);
        {
          Observer discarded(manager, firstUrl, secondUrl);
          discardedId = manager.RequestLoad(firstUrl, Dali::ImageDimensions(64u, 48u),
                                            Dali::SamplingMode::BOX_THEN_LINEAR, &discarded, true,
                                            TextureManager::ReloadPolicy::CACHED, preMultiply);
        }
      }
    }

    TextureManager& manager;
    Dali::Ui::Internal::VisualUrl firstUrl;
    Dali::Ui::Internal::VisualUrl secondUrl;
    TextureManager::TextureId queuedId{TextureManager::INVALID_TEXTURE_ID};
    TextureManager::TextureId duplicateId{TextureManager::INVALID_TEXTURE_ID};
    TextureManager::TextureId discardedId{TextureManager::INVALID_TEXTURE_ID};
    unsigned int count{0u};
    bool loaded{false};
    bool queueMore{true};
  };

  Observer observer(manager, firstUrl, secondUrl);
  auto preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  const auto firstId = manager.RequestLoad(firstUrl, Dali::ImageDimensions(64u, 48u),
                                            Dali::SamplingMode::BOX_THEN_LINEAR, &observer, true,
                                            TextureManager::ReloadPolicy::CACHED, preMultiply);
  for(unsigned int attempt = 0u; attempt < 4u && observer.count < 3u; ++attempt)
  {
    DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  }
  DALI_TEST_EQUALS(observer.count, 3u, TEST_LOCATION);
  DALI_TEST_CHECK(observer.loaded);
  DALI_TEST_EQUALS(observer.duplicateId, firstId, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.discardedId, firstId, TEST_LOCATION);
  DALI_TEST_CHECK(observer.queuedId != firstId);

  manager.RequestRemove(firstId, &observer);
  manager.RequestRemove(observer.duplicateId, &observer);
  manager.RequestRemove(observer.discardedId, nullptr);
  manager.RequestRemove(observer.queuedId, &observer);
  application.SendNotification();
  END_TEST;
}

int UtcDaliTextureManagerMaskCompletionMatrixP(void)
{
  UiTestApplication application;
  using TextureManager = Dali::Ui::Internal::TextureManager;

  class Observer : public Dali::Ui::TextureUploadObserver
  {
  public:
    void LoadComplete(bool success, TextureInformation information) override
    {
      ++count;
      loaded = success;
      textureSet = information.textureSet;
    }

    unsigned int count{0u};
    bool loaded{false};
    Dali::TextureSet textureSet;
  };

  for(bool preapplied : {false, true})
  {
    for(bool validMask : {false, true})
    {
      TextureManager manager;
      TextureManager::MaskingDataPointer maskInfo(new TextureManager::MaskingData());
      maskInfo->mAlphaMaskUrl = Dali::Ui::Internal::VisualUrl(
        validMask ? "../samples/image-view/res/mask.png" : "../samples/image-view/res/missing-mask.png");
      maskInfo->mPreappliedMasking = preapplied;
      maskInfo->mCropToMask = true;
      maskInfo->mContentScaleFactor = 0.8f;

      Observer observer;
      TextureManager::TextureId textureId = TextureManager::INVALID_TEXTURE_ID;
      bool loading = false;
      auto preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
      Dali::TextureSet immediate = manager.LoadTexture(
        Dali::Ui::Internal::VisualUrl("../samples/image-view/res/sample.jpg"),
        Dali::ImageDimensions(64u, 48u), Dali::SamplingMode::BOX_THEN_LINEAR,
        maskInfo, false, textureId, loading, &observer, true,
        TextureManager::ReloadPolicy::CACHED, preMultiply);
      DALI_TEST_CHECK(!immediate);
      DALI_TEST_CHECK(loading);
      DALI_TEST_CHECK(textureId != TextureManager::INVALID_TEXTURE_ID);
      DALI_TEST_CHECK(maskInfo->mAlphaMaskId != TextureManager::INVALID_TEXTURE_ID);

      for(unsigned int attempt = 0u; attempt < 6u && observer.count == 0u; ++attempt)
      {
        DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
      }
      DALI_TEST_EQUALS(observer.count, 1u, TEST_LOCATION);
      DALI_TEST_CHECK(observer.loaded);
      DALI_TEST_CHECK(observer.textureSet);
      manager.RequestRemove(textureId, &observer);
      manager.RequestRemove(maskInfo->mAlphaMaskId, nullptr);
      application.SendNotification();
    }
  }
  END_TEST;
}

int UtcDaliTextureManagerSynchronousAnimatedMaskAndBufferP(void)
{
  UiTestApplication application;
  using TextureManager = Dali::Ui::Internal::TextureManager;
  TextureManager manager(true);
  auto animation = Dali::AnimatedImageLoading::New("../samples/image-view/res/dali-logo-anim.gif", true);
  DALI_TEST_CHECK(animation);

  for(bool preapplied : {false, true})
  {
    TextureManager::MaskingDataPointer maskInfo(new TextureManager::MaskingData());
    maskInfo->mAlphaMaskUrl = Dali::Ui::Internal::VisualUrl("../samples/image-view/res/mask.png");
    maskInfo->mPreappliedMasking = preapplied;
    maskInfo->mContentScaleFactor = 1.0f;
    maskInfo->mCropToMask = true;
    TextureManager::TextureId textureId = TextureManager::INVALID_TEXTURE_ID;
    auto preMultiply = TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD;
    Dali::TextureSet textureSet = manager.LoadAnimatedImageTexture(
      Dali::Ui::Internal::VisualUrl("../samples/image-view/res/dali-logo-anim.gif"),
      animation, 0u, textureId, maskInfo, Dali::ImageDimensions(48u, 48u),
      Dali::SamplingMode::BOX_THEN_LINEAR, true, nullptr, preMultiply,
      TextureManager::ReloadPolicy::CACHED);
    DALI_TEST_CHECK(textureSet);
    DALI_TEST_CHECK(maskInfo->mAlphaMaskId != TextureManager::INVALID_TEXTURE_ID);
    manager.RequestRemove(maskInfo->mAlphaMaskId, nullptr);
    application.SendNotification();
  }

  EncodedImageBuffer::RawBufferType raw;
  raw.PushBack(0x11u);
  raw.PushBack(0x22u);
  EncodedImageBuffer buffer = EncodedImageBuffer::New(std::move(raw));
  Dali::Ui::Internal::VisualUrl bufferUrl(manager.AddEncodedImageBuffer(buffer));
  auto preMultiply = TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD;
  Dali::PixelBuffer pixelBuffer = manager.LoadPixelBuffer(
    bufferUrl, Dali::ImageDimensions(), Dali::SamplingMode::BOX_THEN_LINEAR,
    true, nullptr, true, preMultiply);
  DALI_TEST_CHECK(!pixelBuffer);
  DALI_TEST_CHECK(manager.RemoveEncodedImageBuffer(bufferUrl) == buffer);
  END_TEST;
}

int UtcDaliTextureManagerWaitingForGpuMaskP(void)
{
  UiTestApplication application;
  using TextureManager = Dali::Ui::Internal::TextureManager;
  const Dali::Ui::Internal::VisualUrl maskUrl("../samples/image-view/res/mask.png");
  const Dali::Ui::Internal::VisualUrl imageUrl("../samples/image-view/res/sample.jpg");

  for(bool maskFailed : {false, true})
  {
    TextureManager manager;
    const auto maskId = manager.RequestMaskLoad(maskUrl, TextureManager::StorageType::KEEP_TEXTURE, true);
    DALI_TEST_CHECK(maskId != TextureManager::INVALID_TEXTURE_ID);

    auto preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
    const auto imageId = manager.RequestLoad(
      imageUrl, maskId, TextureManager::INVALID_TEXTURE_ID, 1.0f,
      Dali::ImageDimensions(64u, 48u), Dali::SamplingMode::BOX_THEN_LINEAR,
      false, nullptr, true, TextureManager::ReloadPolicy::CACHED, preMultiply, true);
    DALI_TEST_CHECK(imageId != TextureManager::INVALID_TEXTURE_ID);

    Dali::PixelBuffer pendingPixels = manager.LoadPixelBuffer(
      imageUrl, Dali::ImageDimensions(64u, 48u), Dali::SamplingMode::BOX_THEN_LINEAR,
      true, nullptr, true, preMultiply);
    DALI_TEST_CHECK(pendingPixels);

    auto& cache = manager.mTextureCacheManager;
    const auto maskIndex = cache.GetCacheIndexFromId(maskId);
    const auto imageIndex = cache.GetCacheIndexFromId(imageId);
    DALI_TEST_CHECK(maskIndex != TextureManager::INVALID_CACHE_INDEX);
    DALI_TEST_CHECK(imageIndex != TextureManager::INVALID_CACHE_INDEX);
    auto& imageInfo = cache[imageIndex];
    imageInfo.pixelBuffer = pendingPixels;
    imageInfo.loadState = TextureManager::LoadState::WAITING_FOR_MASK;
    auto& maskInfo = cache[maskIndex];
    if(maskFailed)
    {
      maskInfo.loadState = TextureManager::LoadState::LOAD_FAILED;
    }

    manager.CheckForWaitingTexture(maskInfo);
    DALI_TEST_EQUALS(cache.GetTextureState(imageId), TextureManager::LoadState::UPLOADED, TEST_LOCATION);
    DALI_TEST_CHECK(manager.GetTextureSet(imageId));
    manager.RequestRemove(imageId, nullptr);
    application.SendNotification();
  }
  END_TEST;
}

int UtcDaliTextureManagerAsyncPixelBufferAndWaitingCpuMaskP(void)
{
  UiTestApplication application;
  using TextureManager = Dali::Ui::Internal::TextureManager;
  TextureManager manager;

  class Observer : public Dali::Ui::TextureUploadObserver
  {
  public:
    void LoadComplete(bool success, TextureInformation information) override
    {
      ++count;
      loaded = success;
      pixelBuffer = information.pixelBuffer;
      returnType = information.returnType;
    }

    unsigned int count{0u};
    bool loaded{false};
    Dali::PixelBuffer pixelBuffer;
    ReturnType returnType{ReturnType::TEXTURE};
  };

  Observer observer;
  auto preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  DALI_TEST_CHECK(!manager.LoadPixelBuffer(
    Dali::Ui::Internal::VisualUrl("../samples/image-view/res/sample.jpg"),
    Dali::ImageDimensions(32u, 24u), Dali::SamplingMode::BOX_THEN_LINEAR,
    false, &observer, true, preMultiply));
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  DALI_TEST_EQUALS(observer.count, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(observer.loaded && observer.pixelBuffer);
  DALI_TEST_EQUALS(observer.returnType, Dali::Ui::TextureUploadObserver::ReturnType::PIXEL_BUFFER, TEST_LOCATION);

  const Dali::Ui::Internal::VisualUrl maskUrl("../samples/image-view/res/mask.png");
  const Dali::Ui::Internal::VisualUrl imageUrl("../samples/image-view/res/sample.jpg");
  const auto maskId = manager.RequestMaskLoad(maskUrl, TextureManager::StorageType::KEEP_PIXEL_BUFFER, true);
  DALI_TEST_CHECK(maskId != TextureManager::INVALID_TEXTURE_ID);
  const auto imageId = manager.RequestLoad(
    imageUrl, maskId, TextureManager::INVALID_TEXTURE_ID, 1.0f,
    Dali::ImageDimensions(32u, 24u), Dali::SamplingMode::BOX_THEN_LINEAR,
    false, nullptr, true, TextureManager::ReloadPolicy::CACHED, preMultiply, true);
  DALI_TEST_CHECK(imageId != TextureManager::INVALID_TEXTURE_ID);
  Dali::PixelBuffer pendingPixels = manager.LoadPixelBuffer(
    imageUrl, Dali::ImageDimensions(32u, 24u), Dali::SamplingMode::BOX_THEN_LINEAR,
    true, nullptr, true, preMultiply);
  DALI_TEST_CHECK(pendingPixels);
  auto& cache = manager.mTextureCacheManager;
  auto maskIndex = cache.GetCacheIndexFromId(maskId);
  auto imageIndex = cache.GetCacheIndexFromId(imageId);
  DALI_TEST_CHECK(maskIndex != TextureManager::INVALID_CACHE_INDEX);
  DALI_TEST_CHECK(imageIndex != TextureManager::INVALID_CACHE_INDEX);
  cache[imageIndex].pixelBuffer = pendingPixels;
  cache[imageIndex].loadState = TextureManager::LoadState::WAITING_FOR_MASK;
  manager.CheckForWaitingTexture(cache[maskIndex]);
  DALI_TEST_EQUALS(cache.GetTextureState(imageId), TextureManager::LoadState::MASK_APPLYING, TEST_LOCATION);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5, true));
  DALI_TEST_EQUALS(cache.GetTextureState(imageId), TextureManager::LoadState::UPLOADED, TEST_LOCATION);
  DALI_TEST_CHECK(manager.GetTextureSet(imageId));
  DALI_TEST_CHECK(!manager.GetTextureSet(TextureManager::INVALID_TEXTURE_ID));
  manager.RequestRemove(imageId, nullptr);
  application.SendNotification();
  END_TEST;
}
