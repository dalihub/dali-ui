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
#include <dali-ui-foundation/integration-api/visuals/image-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/internal/visuals/animated-image/animated-image-visual.h>
#include <dali-ui-foundation/internal/visuals/animated-image/image-cache.h>
#include <dali-ui-foundation/internal/visuals/visual-base-data-impl.h>
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
    return mFirstFrameTextureSet;
  }

  Dali::TextureSet Frame(uint32_t frameIndex) override
  {
    mCurrentFrameIndex = frameIndex;
    return mFrameTextureSet;
  }

  uint32_t GetFrameInterval(uint32_t) const override
  {
    return 0u;
  }

  int32_t GetCurrentFrameIndex() const override
  {
    return mCurrentFrameIndex;
  }

  int32_t GetTotalFrameCount() const override
  {
    return mTotalFrameCount;
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

  Dali::TextureSet mFirstFrameTextureSet;
  Dali::TextureSet mFrameTextureSet;
  int32_t mCurrentFrameIndex{-1};
  int32_t mTotalFrameCount{0};

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
  properties.Add(Dali::Ui::Integration::Visual::Property::TYPE, UiIntegration::InternalVisualType::ANIMATED_IMAGE);
  properties.Add(Dali::Ui::Integration::ImageVisual::Property::URL, "release-policy.gif");

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
  properties.Add(Dali::Ui::Integration::Visual::Property::TYPE, UiIntegration::InternalVisualType::ANIMATED_IMAGE);
  properties.Add(Dali::Ui::Integration::ImageVisual::Property::URL, "release-policy.gif");

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

int UtcDaliAnimatedImageVisualUrlAndInstancePropertiesP(void)
{
  UiTestApplication application;
  UiIntegration::VisualFactory factory = UiIntegration::VisualFactory::Get();
  UiIntegration::Visual::Base visual = factory.CreateVisual(
    "../samples/visual-base/res/animatedLoading.gif", Dali::ImageDimensions(80u, 60u),
    UiIntegration::VisualFactory::NONE);
  DALI_TEST_CHECK(visual);
  auto& animated = static_cast<UiInternal::AnimatedImageVisual&>(
    Dali::Ui::GetImplementation(visual).GetVisualObject());

  Dali::Property::Map properties;
  animated.DoCreateInstancePropertyMap(properties);
  const auto* width = properties.Find(UiIntegration::ImageVisual::Property::DESIRED_WIDTH);
  const auto* height = properties.Find(UiIntegration::ImageVisual::Property::DESIRED_HEIGHT);
  DALI_TEST_CHECK(width && height);
  DALI_TEST_EQUALS(width->Get<int>(), 80, TEST_LOCATION);
  DALI_TEST_EQUALS(height->Get<int>(), 60, TEST_LOCATION);

  animated.SetFittingMode(Dali::Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
  animated.OnApplyFittingMode(Dali::Vector2(160.0f, 120.0f), Dali::Insets(), 1.0f);
  animated.UpdateShader();
  DALI_TEST_CHECK(animated.OnGetPropertyObject(
    Dali::Property::Key(UiIntegration::ImageVisual::Property::PIXEL_AREA), false).propertyIndex != Dali::Property::INVALID_INDEX);
  END_TEST;
}

int UtcDaliAnimatedImageVisualFrameTransitionsP(void)
{
  UiTestApplication application;
  Dali::Property::Map properties;
  properties.Add(Dali::Ui::Integration::Visual::Property::TYPE, UiIntegration::InternalVisualType::ANIMATED_IMAGE);
  properties.Add(UiIntegration::ImageVisual::Property::URL, "frame-transitions.gif");
  UiIntegration::Visual::Base visual = UiIntegration::VisualFactory::Get().CreateVisual(properties);
  auto& animated = static_cast<UiInternal::AnimatedImageVisual&>(
    Dali::Ui::GetImplementation(visual).GetVisualObject());

  bool keepUnusedTexture = false;
  delete animated.mImageCache;
  auto* cache = new TestImageCache(animated.mFactoryCache.GetTextureManager(), animated.mMaskingData,
                                  animated, keepUnusedTexture);
  animated.mImageCache = cache;
  cache->mTotalFrameCount = 3;

  Dali::TextureSet frame = Dali::TextureSet::New();
  frame.SetTexture(0u, Dali::Texture::New(Dali::TextureType::TEXTURE_2D, Dali::Pixel::RGBA8888, 20u, 12u));
  cache->mFirstFrameTextureSet = frame;
  cache->mFrameTextureSet = frame;
  animated.mStartFirstFrame = true;
  animated.FrameReady(frame, 60u, true);
  DALI_TEST_EQUALS(animated.mFrameCount, 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(animated.mImageSize.GetWidth(), 20u, TEST_LOCATION);
  DALI_TEST_CHECK(animated.mFrameDelayTimer);
  DALI_TEST_CHECK(!animated.mStartFirstFrame);

  animated.OnDoAction(Dali::Ui::Integration::AnimatedImageVisual::Action::PAUSE, {});
  DALI_TEST_CHECK(!animated.DisplayNextFrame());
  animated.OnDoAction(Dali::Ui::Integration::AnimatedImageVisual::Action::JUMP_TO, 2);
  DALI_TEST_CHECK(animated.mIsJumpTo);
  DALI_TEST_CHECK(!animated.DisplayNextFrame());
  DALI_TEST_EQUALS(cache->mCurrentFrameIndex, 2, TEST_LOCATION);
  animated.OnDoAction(Dali::Ui::Integration::AnimatedImageVisual::Action::JUMP_TO, -1);
  DALI_TEST_CHECK(!animated.mIsJumpTo);
  animated.OnDoAction(Dali::Ui::Integration::AnimatedImageVisual::Action::JUMP_TO, 3);
  DALI_TEST_CHECK(!animated.mIsJumpTo);

  animated.OnDoAction(Dali::Ui::Integration::AnimatedImageVisual::Action::PLAY, {});
  cache->mCurrentFrameIndex = 1;
  DALI_TEST_CHECK(animated.DisplayNextFrame());
  DALI_TEST_EQUALS(cache->mCurrentFrameIndex, 2, TEST_LOCATION);
  animated.mLoopCount = 1;
  DALI_TEST_CHECK(!animated.DisplayNextFrame());
  DALI_TEST_EQUALS(animated.mActionStatus, Dali::Ui::Integration::AnimatedImageVisual::Action::STOP, TEST_LOCATION);

  animated.mStopBehavior = Dali::Ui::AnimatedImage::StopBehavior::FIRST_FRAME;
  DALI_TEST_CHECK(!animated.DisplayNextFrame());
  DALI_TEST_EQUALS(cache->mCurrentFrameIndex, 0, TEST_LOCATION);
  animated.mStopBehavior = Dali::Ui::AnimatedImage::StopBehavior::LAST_FRAME;
  DALI_TEST_CHECK(!animated.DisplayNextFrame());
  DALI_TEST_EQUALS(cache->mCurrentFrameIndex, 2, TEST_LOCATION);

  animated.AllocateMaskData();
  animated.mMaskingData->mPreappliedMasking = false;
  DALI_TEST_CHECK(animated.CheckMaskTexture());
  DALI_TEST_CHECK(!animated.CheckMaskTexture());
  Dali::TextureSet masked = Dali::TextureSet::New();
  masked.SetTexture(0u, frame.GetTexture(0u));
  masked.SetTexture(1u, Dali::Texture::New(Dali::TextureType::TEXTURE_2D, Dali::Pixel::L8, 8u, 6u));
  animated.mMaskingData->mCropToMask = true;
  animated.mMaskingData->mContentScaleFactor = 0.5f;
  animated.SetImageSize(masked);
  DALI_TEST_EQUALS(animated.mImageSize.GetWidth(), 8u, TEST_LOCATION);
  animated.mImpl->mRenderer.SetTextures(masked);
  DALI_TEST_CHECK(animated.CheckMaskTexture());
  DALI_TEST_CHECK(!animated.CheckMaskTexture());

  Dali::TextureSet yuv = Dali::TextureSet::New();
  yuv.SetTexture(0u, Dali::Texture::New(Dali::TextureType::TEXTURE_2D, Dali::Pixel::L8, 8u, 8u));
  yuv.SetTexture(1u, Dali::Texture::New(Dali::TextureType::TEXTURE_2D, Dali::Pixel::CHROMINANCE_U, 4u, 4u));
  yuv.SetTexture(2u, Dali::Texture::New(Dali::TextureType::TEXTURE_2D, Dali::Pixel::CHROMINANCE_V, 4u, 4u));
  DALI_TEST_CHECK(animated.UpdateYuvInformation(yuv));
  DALI_TEST_CHECK(animated.mNeedYuvToRgb);
  DALI_TEST_CHECK(!animated.UpdateYuvInformation(yuv));
  DALI_TEST_CHECK(animated.UpdateYuvInformation(frame));
  DALI_TEST_CHECK(!animated.mNeedYuvToRgb);

  animated.mFrameDelayTimer.Stop();
  animated.mFrameDelayTimer.Reset();
  END_TEST;
}

int UtcDaliAnimatedImageVisualPropertyBoundaryMatrixP(void)
{
  UiTestApplication application;
  Dali::Property::Map properties;
  properties.Add(UiIntegration::Visual::Property::TYPE, UiIntegration::InternalVisualType::ANIMATED_IMAGE);
  properties.Add(UiIntegration::ImageVisual::Property::URL, "property-boundaries.gif");
  UiIntegration::Visual::Base visual = UiIntegration::VisualFactory::Get().CreateVisual(properties);
  auto& animated = static_cast<UiInternal::AnimatedImageVisual&>(
    Dali::Ui::GetImplementation(visual).GetVisualObject());

  Dali::Property::Map updates;
  updates.Add("batchSize", 4);
  updates.Add("notAVisualProperty", 17);
  updates.Add(UiIntegration::ImageVisual::Property::CACHE_SIZE, 5);
  animated.DoSetProperties(updates);
  DALI_TEST_EQUALS(animated.mBatchSize, 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(animated.mCacheSize, 5u, TEST_LOCATION);

  animated.DoSetProperty(UiIntegration::ImageVisual::Property::PIXEL_AREA, Dali::Vector4(0.1f, 0.2f, 0.6f, 0.5f));
  DALI_TEST_CHECK(animated.mPixelAreaIndex != Dali::Property::INVALID_INDEX);
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::PIXEL_AREA, Dali::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::WRAP_MODE_U, "REPEAT");
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::WRAP_MODE_V, "invalid-wrap-mode");
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::PRE_MULTIPLIED_ALPHA, false);
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::PRE_MULTIPLIED_ALPHA, true);
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::BATCH_SIZE, 1);
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::CACHE_SIZE, 1);
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::FRAME_DELAY, 30);
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::LOOP_COUNT, 2);
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::STOP_BEHAVIOR, "LAST_FRAME");
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::SYNCHRONOUS_LOADING, true);
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::SYNCHRONOUS_LOADING, false);
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::ALPHA_MASK_URL, "");
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::CONTENT_SCALE_FOR_MASKING, 0.5f);
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::CROP_TO_MASK, true);
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::MASKING_POLICY,
                         static_cast<int>(Dali::Ui::Image::MaskingPolicy::ON_RENDERING));
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::ENABLE_BROKEN_IMAGE, false);
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::RELEASE_POLICY, "DESTROYED");
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::LOAD_POLICY, "ATTACHED");
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::FITTING_MODE, "FIT_KEEP_ASPECT_RATIO");
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::SAMPLING_MODE, "BOX_THEN_LINEAR");
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::DESIRED_WIDTH, "invalid");
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::DESIRED_HEIGHT, "invalid");
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::DESIRED_WIDTH, 42.0f);
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::DESIRED_HEIGHT, 24.0f);
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::FRAME_SPEED_FACTOR, 100.0f);
  animated.DoSetProperty(UiIntegration::ImageVisual::Property::IMAGE_LOAD_WITH_VIEW_SIZE, true);

  DALI_TEST_EQUALS(animated.mDesiredSize.GetWidth(), 42u, TEST_LOCATION);
  DALI_TEST_EQUALS(animated.mDesiredSize.GetHeight(), 24u, TEST_LOCATION);
  DALI_TEST_CHECK(animated.mMaskingData);
  DALI_TEST_CHECK(animated.mMaskingData->mCropToMask);
  DALI_TEST_CHECK(animated.mImageLoadWithViewSize);
  DALI_TEST_CHECK(!animated.mBrokenImageEnabled);
  animated.mImpl->AddCustomShader(Dali::Property::Map{});
  animated.UpdateShader();
  DALI_TEST_CHECK(animated.GenerateShader());
  END_TEST;
}

int UtcDaliAnimatedImageVisualNaturalSizeAndFailureP(void)
{
  UiTestApplication application;
  Dali::Property::Map properties;
  properties.Add(UiIntegration::Visual::Property::TYPE, UiIntegration::InternalVisualType::ANIMATED_IMAGE);
  properties.Add(UiIntegration::ImageVisual::Property::URL, "natural-size.gif");
  UiIntegration::Visual::Base visual = UiIntegration::VisualFactory::Get().CreateVisual(properties);
  auto& animated = static_cast<UiInternal::AnimatedImageVisual&>(
    Dali::Ui::GetImplementation(visual).GetVisualObject());

  Dali::TextureSet frame = Dali::TextureSet::New();
  frame.SetTexture(0u, Dali::Texture::New(Dali::TextureType::TEXTURE_2D, Dali::Pixel::RGBA8888, 20u, 12u));
  Dali::Vector2 naturalSize;
  animated.mImageLoadWithViewSize = true;
  animated.mLastRequiredSize = Dali::ImageDimensions(30u, 40u);
  animated.mImageSize = Dali::ImageDimensions(20u, 12u);
  animated.mImpl->mRenderer.SetTextures(frame);
  animated.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Dali::Vector2(20.0f, 12.0f), TEST_LOCATION);

  animated.mImpl->mRenderer.RemoveTextures();
  animated.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Dali::Vector2(30.0f, 40.0f), TEST_LOCATION);
  animated.mImageLoadWithViewSize = false;
  animated.mDesiredSize = Dali::ImageDimensions(32u, 24u);
  animated.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Dali::Vector2(32.0f, 24.0f), TEST_LOCATION);
  animated.mImpl->mRenderer.SetTextures(frame);
  animated.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Dali::Vector2(20.0f, 12.0f), TEST_LOCATION);

  animated.AllocateMaskData();
  animated.mMaskingData->mCropToMask = true;
  Dali::Property::Map result;
  animated.DoCreatePropertyMap(result);
  DALI_TEST_CHECK(result.Find(UiIntegration::ImageVisual::Property::CROP_TO_MASK));
  DALI_TEST_CHECK(result.Find(UiIntegration::ImageVisual::Property::URL));
  animated.mFrameCount = 0u;
  animated.DoCreatePropertyMap(result);
  DALI_TEST_CHECK(result.Find(UiIntegration::ImageVisual::Property::TOTAL_FRAME_COUNT));

  Dali::Actor actor = Dali::Actor::New();
  actor.SetProperty(Dali::Actor::Property::SIZE, Dali::Vector2(100.0f, 80.0f));
  animated.mPlacementActor = actor;
  animated.mBrokenImageEnabled = true;
  animated.SetLoadingFailed();
  DALI_TEST_EQUALS(animated.mImpl->mResourceStatus, Dali::Ui::Visual::ResourceStatus::FAILED, TEST_LOCATION);
  DALI_TEST_CHECK(animated.mUseBrokenImageRenderer);
  DALI_TEST_CHECK(animated.mRendererAdded);
  END_TEST;
}
