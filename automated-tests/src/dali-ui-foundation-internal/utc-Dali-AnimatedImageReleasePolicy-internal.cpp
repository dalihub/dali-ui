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
#include <dali-ui-foundation/integration-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>
#include <dali-ui-foundation/internal/visuals/animated-image/animated-image-visual.h>
#include <dali-ui-foundation/internal/visuals/animated-image/image-cache.h>
#undef protected
#undef private

namespace
{
namespace UiInternal    = Dali::Ui::Internal;
namespace UiIntegration = Dali::Ui::Integration;

class TestImageCache : public UiInternal::ImageCache
{
public:
  TestImageCache(UiInternal::TextureManager&                     textureManager,
                 UiInternal::TextureManager::MaskingDataPointer& maskingData,
                 ImageCache::FrameReadyObserver&                 observer,
                 bool&                                           keepUnusedTexture,
                 UiInternal::TextureManager::TextureId           textureId =
                   UiInternal::TextureManager::INVALID_TEXTURE_ID)
  : ImageCache(textureManager, Dali::ImageDimensions(), Dali::SamplingMode::BOX_THEN_LINEAR,
               maskingData, observer, 1u, 0u, false),
    mKeepUnusedTexture(keepUnusedTexture),
    mTextureId(textureId)
  {
  }

  Dali::TextureSet FirstFrame() override
  {
    return {};
  }

  Dali::TextureSet Frame(uint32_t) override
  {
    return {};
  }

  uint32_t GetFrameInterval(uint32_t) const override
  {
    return 0u;
  }

  int32_t GetCurrentFrameIndex() const override
  {
    return -1;
  }

  int32_t GetTotalFrameCount() const override
  {
    return 0;
  }

  void ClearCache(bool keepUnusedTexture) override
  {
    mKeepUnusedTexture = keepUnusedTexture;
    if(mTextureId != UiInternal::TextureManager::INVALID_TEXTURE_ID)
    {
      mTextureManager.RequestRemove(mTextureId, nullptr, keepUnusedTexture);
      mTextureId = UiInternal::TextureManager::INVALID_TEXTURE_ID;
    }
  }

private:
  bool&                                         mKeepUnusedTexture;
  UiInternal::TextureManager::TextureId         mTextureId;
};

UiInternal::TextureManager::TextureId AddUploadedTexture(UiInternal::TextureManager& textureManager,
                                                         const char*                 url)
{
  auto&      cache     = textureManager.mTextureCacheManager;
  auto       textureId = cache.GenerateTextureId();
  const auto visualUrl = UiInternal::VisualUrl(url);
  auto       hash      = cache.GenerateHash(visualUrl, Dali::ImageDimensions(), Dali::SamplingMode::BOX_THEN_LINEAR,
                                            UiInternal::TextureManager::INVALID_TEXTURE_ID, false, true, 0u);

  UiInternal::TextureManager::TextureInfo textureInfo(
    textureId, UiInternal::TextureManager::INVALID_TEXTURE_ID, visualUrl, Dali::ImageDimensions(), 1.0f,
    Dali::SamplingMode::BOX_THEN_LINEAR, false, false, hash, true, false, Dali::AnimatedImageLoading(), 0u, false);
  textureInfo.referenceCount = 1;
  textureInfo.loadState      = UiInternal::TextureManager::LoadState::UPLOADED;
  cache.AppendCache(textureInfo);

  return textureId;
}

bool IsTextureCached(UiInternal::TextureManager& textureManager, UiInternal::TextureManager::TextureId textureId)
{
  return textureManager.mTextureCacheManager.GetCacheIndexFromId(textureId) !=
         UiInternal::TextureManager::INVALID_CACHE_INDEX;
}

bool DestroyNeverVisualWithFrameCount(uint32_t frameCount)
{
  Dali::Property::Map properties;
  properties.Add(Dali::Ui::VisualBasePropertyIndex::TYPE, UiIntegration::InternalVisualType::ANIMATED_IMAGE);
  properties.Add(Dali::Ui::ImageVisualPropertyIndex::URL, "release-policy.gif");

  UiIntegration::Visual::Base visual        = UiIntegration::VisualFactory::Get().CreateVisual(properties);
  auto&                       animatedImage = static_cast<UiInternal::AnimatedImageVisual&>(
    Dali::Ui::GetImplementation(visual).GetVisualObject());

  delete animatedImage.mImageCache;

  bool keepUnusedTexture       = false;
  animatedImage.mImageCache    = new TestImageCache(animatedImage.mFactoryCache.GetTextureManager(),
                                                    animatedImage.mMaskingData, animatedImage,
                                                    keepUnusedTexture);
  animatedImage.mFrameCount    = frameCount;
  animatedImage.mReleasePolicy = Dali::Ui::Image::ReleasePolicy::NEVER;

  visual.Reset();
  return keepUnusedTexture;
}
} // unnamed namespace

void utc_dali_animated_image_release_policy_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_animated_image_release_policy_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliAnimatedImageReleasePolicyNeverBeforeFrameCountKnown(void)
{
  UiTestApplication application;

  DALI_TEST_CHECK(DestroyNeverVisualWithFrameCount(0u));
  DALI_TEST_CHECK(DestroyNeverVisualWithFrameCount(1u));
  DALI_TEST_CHECK(!DestroyNeverVisualWithFrameCount(2u));

  END_TEST;
}

int UtcDaliAnimatedImageReleasePolicyNeverSingleFrameClearedExplicitly(void)
{
  UiTestApplication application;

  Dali::Property::Map properties;
  properties.Add(Dali::Ui::VisualBasePropertyIndex::TYPE, UiIntegration::InternalVisualType::ANIMATED_IMAGE);
  properties.Add(Dali::Ui::ImageVisualPropertyIndex::URL, "release-policy.gif");

  UiIntegration::Visual::Base visual        = UiIntegration::VisualFactory::Get().CreateVisual(properties);
  auto&                       animatedImage = static_cast<UiInternal::AnimatedImageVisual&>(
    Dali::Ui::GetImplementation(visual).GetVisualObject());
  auto& textureManager = animatedImage.mFactoryCache.GetTextureManager();
  auto  textureId      = AddUploadedTexture(textureManager, "release-policy-single-frame.jpg");

  delete animatedImage.mImageCache;

  bool keepUnusedTexture       = false;
  animatedImage.mImageCache    = new TestImageCache(textureManager, animatedImage.mMaskingData, animatedImage,
                                                    keepUnusedTexture, textureId);
  animatedImage.mFrameCount    = 1u;
  animatedImage.mReleasePolicy = Dali::Ui::Image::ReleasePolicy::NEVER;

  visual.Reset();
  application.SendNotification();

  DALI_TEST_CHECK(keepUnusedTexture);
  DALI_TEST_CHECK(IsTextureCached(textureManager, textureId));

  Dali::Ui::ImageCacheUtils::ClearUnusedTextures();
  application.SendNotification();
  DALI_TEST_CHECK(!IsTextureCached(textureManager, textureId));

  END_TEST;
}
