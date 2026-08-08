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
 *
 */

// CLASS HEADER
#include <dali-ui-foundation/internal/visuals/animated-image/animated-image-visual.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/rendering/decorated-visual-renderer.h>
#include <dali/integration-api/string-utils.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/image-loader/texture-manager.h>
#include <dali-ui-foundation/integration-api/visuals/animated-image-visual-signals-integ.h>
#include <dali-ui-foundation/integration-api/visuals/image-visual-actions-integ.h>
#include <dali-ui-foundation/internal/visuals/animated-image/fixed-image-cache.h>
#include <dali-ui-foundation/internal/visuals/animated-image/rolling-animated-image-cache.h>
#include <dali-ui-foundation/internal/visuals/animated-image/rolling-image-cache.h>
#include <dali-ui-foundation/internal/visuals/image/image-visual-shader-factory.h>
#include <dali-ui-foundation/internal/visuals/image/image-visual-shader-feature-builder.h>
#include <dali-ui-foundation/internal/visuals/visual-base-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-cache.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-string-constants.h>
#include <dali-ui-foundation/public-api/visuals/image-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

using Dali::Integration::GetStdString;
using Dali::Integration::ToDaliStringView;
using Dali::Integration::ToPropertyValue;
using Dali::Integration::ToStdString;

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
// sampling modes
DALI_ENUM_TO_STRING_TABLE_BEGIN(SAMPLING_MODE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::SamplingMode, BOX)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::SamplingMode, NEAREST)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::SamplingMode, LINEAR)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::SamplingMode, BOX_THEN_NEAREST)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::SamplingMode, BOX_THEN_LINEAR)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::SamplingMode, NO_FILTER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::SamplingMode, DONT_CARE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::SamplingMode, LANCZOS)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::SamplingMode, BOX_THEN_LANCZOS)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::SamplingMode, DEFAULT)
DALI_ENUM_TO_STRING_TABLE_END(SAMPLING_MODE)

// stop behavior
DALI_ENUM_TO_STRING_TABLE_BEGIN(STOP_BEHAVIOR)
  DALI_ENUM_CLASS_TO_STRING_WITH_SCOPE(Dali::Ui::AnimatedImage::StopBehavior, CURRENT_FRAME)
  DALI_ENUM_CLASS_TO_STRING_WITH_SCOPE(Dali::Ui::AnimatedImage::StopBehavior, FIRST_FRAME)
  DALI_ENUM_CLASS_TO_STRING_WITH_SCOPE(Dali::Ui::AnimatedImage::StopBehavior, LAST_FRAME)
DALI_ENUM_TO_STRING_TABLE_END(STOP_BEHAVIOR)

// wrap modes
DALI_ENUM_TO_STRING_TABLE_BEGIN(WRAP_MODE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::WrapMode, DEFAULT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::WrapMode, CLAMP_TO_EDGE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::WrapMode, REPEAT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::WrapMode, MIRRORED_REPEAT)
DALI_ENUM_TO_STRING_TABLE_END(WRAP_MODE)

// load policies
DALI_ENUM_TO_STRING_TABLE_BEGIN(LOAD_POLICY)
  DALI_ENUM_CLASS_TO_STRING_WITH_SCOPE(Dali::Ui::Image::LoadPolicy, IMMEDIATE)
  DALI_ENUM_CLASS_TO_STRING_WITH_SCOPE(Dali::Ui::Image::LoadPolicy, ATTACHED)
DALI_ENUM_TO_STRING_TABLE_END(LOAD_POLICY)

// release policies
DALI_ENUM_TO_STRING_TABLE_BEGIN(RELEASE_POLICY)
  DALI_ENUM_CLASS_TO_STRING_WITH_SCOPE(Dali::Ui::Image::ReleasePolicy, DETACHED)
  DALI_ENUM_CLASS_TO_STRING_WITH_SCOPE(Dali::Ui::Image::ReleasePolicy, DESTROYED)
  DALI_ENUM_CLASS_TO_STRING_WITH_SCOPE(Dali::Ui::Image::ReleasePolicy, NEVER)
DALI_ENUM_TO_STRING_TABLE_END(RELEASE_POLICY)

// fitting mode
DALI_ENUM_TO_STRING_TABLE_BEGIN(FITTING_MODE)
  DALI_ENUM_CLASS_TO_STRING_WITH_SCOPE(Dali::Ui::Image::FittingMode, FIT_KEEP_ASPECT_RATIO)
  DALI_ENUM_CLASS_TO_STRING_WITH_SCOPE(Dali::Ui::Image::FittingMode, FILL)
  DALI_ENUM_CLASS_TO_STRING_WITH_SCOPE(Dali::Ui::Image::FittingMode, OVER_FIT_KEEP_ASPECT_RATIO)
  DALI_ENUM_CLASS_TO_STRING_WITH_SCOPE(Dali::Ui::Image::FittingMode, CENTER)
DALI_ENUM_TO_STRING_TABLE_END(FITTING_MODE)

static constexpr uint32_t SINGLE_IMAGE_COUNT = 1u;
static constexpr uint32_t FIRST_FRAME_INDEX  = 0u;
static constexpr uint16_t MINIMUM_CACHESIZE  = 1;
static constexpr Vector4  FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);
static constexpr auto     LOOP_FOREVER = -1;
static constexpr auto     FIRST_LOOP   = 0u;

constexpr float MINIMUM_FRAME_SPEED_FACTOR(0.01f);
constexpr float MAXIMUM_FRAME_SPEED_FACTOR(100.0f);

constexpr float ALPHA_VALUE_PREMULTIPLIED(1.0f);

constexpr uint32_t TEXTURE_COUNT_FOR_GPU_ALPHA_MASK = 2u;
constexpr uint32_t TEXTURE_COUNT_FOR_GPU_YUV_TO_RGB = 3u;

Ui::AnimatedImage::PlayState ConvertActionStatusToPlayState(Dali::Ui::Integration::AnimatedImageVisual::Action::Type actionStatus)
{
  // TODO : We'd better remove action status and use PlayState instead in future.
  switch(actionStatus)
  {
    case Dali::Ui::Integration::AnimatedImageVisual::Action::PLAY:
    {
      return Ui::AnimatedImage::PlayState::PLAYING;
    }
    case Dali::Ui::Integration::AnimatedImageVisual::Action::PAUSE:
    {
      return Ui::AnimatedImage::PlayState::PAUSED;
    }
    case Dali::Ui::Integration::AnimatedImageVisual::Action::STOP:
    default:
    {
      return Ui::AnimatedImage::PlayState::STOPPED;
    }
  }
}

struct NameIndexMatch
{
  const char* const name;
  Property::Index   index;
};

const NameIndexMatch NAME_INDEX_MATCH_TABLE[] = {
  {IMAGE_SAMPLING_MODE, Ui::ImageVisualPropertyIndex::SAMPLING_MODE},
  {IMAGE_DESIRED_WIDTH, Ui::ImageVisualPropertyIndex::DESIRED_WIDTH},
  {IMAGE_DESIRED_HEIGHT, Ui::ImageVisualPropertyIndex::DESIRED_HEIGHT},
  {PIXEL_AREA_UNIFORM_NAME, Ui::ImageVisualPropertyIndex::PIXEL_AREA},
  {IMAGE_WRAP_MODE_U, Ui::ImageVisualPropertyIndex::WRAP_MODE_U},
  {IMAGE_WRAP_MODE_V, Ui::ImageVisualPropertyIndex::WRAP_MODE_V},
  {PRE_MULTIPLIED_ALPHA, Ui::ImageVisualPropertyIndex::PRE_MULTIPLIED_ALPHA},
  {SYNCHRONOUS_LOADING, Ui::ImageVisualPropertyIndex::SYNCHRONOUS_LOADING},
  {BATCH_SIZE_NAME, Ui::ImageVisualPropertyIndex::BATCH_SIZE},
  {CACHE_SIZE_NAME, Ui::ImageVisualPropertyIndex::CACHE_SIZE},
  {FRAME_DELAY_NAME, Ui::ImageVisualPropertyIndex::FRAME_DELAY},
  {ALPHA_MASK_URL, Ui::ImageVisualPropertyIndex::ALPHA_MASK_URL},
  {MASK_CONTENT_SCALE_NAME, Ui::ImageVisualPropertyIndex::MASK_CONTENT_SCALE},
  {CROP_TO_MASK_NAME, Ui::ImageVisualPropertyIndex::CROP_TO_MASK},
  {MASKING_TYPE_NAME, Ui::ImageVisualPropertyIndex::MASKING_TYPE},
  {ENABLE_BROKEN_IMAGE, Ui::ImageVisualPropertyIndex::ENABLE_BROKEN_IMAGE},
  {LOAD_POLICY_NAME, Ui::ImageVisualPropertyIndex::LOAD_POLICY},
  {RELEASE_POLICY_NAME, Ui::ImageVisualPropertyIndex::RELEASE_POLICY},
  {FITTING_MODE, Ui::ImageVisualPropertyIndex::FITTING_MODE},
  {LOOP_COUNT_NAME, Ui::ImageVisualPropertyIndex::LOOP_COUNT},
  {STOP_BEHAVIOR_NAME, Ui::ImageVisualPropertyIndex::STOP_BEHAVIOR},
  {FRAME_SPEED_FACTOR, Ui::ImageVisualPropertyIndex::FRAME_SPEED_FACTOR},
  {SYNCHRONOUS_SIZING, Ui::ImageVisualPropertyIndex::SYNCHRONOUS_SIZING},
};
const int NAME_INDEX_MATCH_TABLE_SIZE = sizeof(NAME_INDEX_MATCH_TABLE) / sizeof(NAME_INDEX_MATCH_TABLE[0]);

#if defined(DEBUG_ENABLED)
Debug::Filter* gAnimImgLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_ANIMATED_IMAGE");
#endif

/**
 * @brief Safety method to calculate interval with speed factor.
 */
template<typename T>
inline uint32_t CalculateInterval(const T interval, const float frameSpeedFactor)
{
  return DALI_LIKELY(Dali::Equals(frameSpeedFactor, 1.0f))
           ? static_cast<uint32_t>(interval)
           : static_cast<uint32_t>(static_cast<float>(interval) / (frameSpeedFactor));
}
} // namespace

/**
 * Multi-image  Flow of execution
 *
 *   | New
 *   |   DoSetProperties()
 *   |   OnInitialize()
 *   |     CreateImageCache()
 *   |
 *   | DoSetOnScene()
 *   |   PrepareTextureSet()
 *   |     cache->FirstFrame()
 *   |
 *   | FrameReady(textureSet)
 *   |   StartFirstFrame:
 *   |     actor.AddRenderer
 *   |     start timer
 *   |   mRenderer.SetTextures(textureSet)
 *   |
 *   | Timer ticks
 *   |   DisplayNextFrame()
 *   |     if front frame is ready,
 *   |       mRenderer.SetTextures( front frame's texture )
 *   |     else
 *   |       Waiting for frame ready.
 *   |
 *   | FrameReady(textureSet)
 *   |   mRenderer.SetTextures(textureSet)
 *   V
 *  Time
 */

AnimatedImageVisualPtr AnimatedImageVisual::New(VisualFactoryCache&                             factoryCache,
                                                ImageVisualShaderFactory&                       shaderFactory,
                                                Ui::Integration::VisualFactory::CreationOptions creationOptions,
                                                const VisualUrl&                                imageUrl,
                                                const Property::Map&                            properties)
{
  AnimatedImageVisualPtr visual(new AnimatedImageVisual(factoryCache, shaderFactory, creationOptions, ImageDimensions()));
  visual->InitializeAnimatedImage(imageUrl);
  visual->SetProperties(properties);

  visual->Initialize();

  return visual;
}

AnimatedImageVisualPtr AnimatedImageVisual::New(VisualFactoryCache&                             factoryCache,
                                                ImageVisualShaderFactory&                       shaderFactory,
                                                Ui::Integration::VisualFactory::CreationOptions creationOptions,
                                                const Property::Array&                          imageUrls,
                                                const Property::Map&                            properties)
{
  AnimatedImageVisualPtr visual(new AnimatedImageVisual(factoryCache, shaderFactory, creationOptions, ImageDimensions()));
  visual->mImageUrls = new ImageCache::UrlList();
  visual->mImageUrls->reserve(imageUrls.Count());

  for(unsigned int i = 0; i < imageUrls.Count(); ++i)
  {
    ImageCache::UrlStore urlStore;
    urlStore.mTextureId = TextureManager::INVALID_TEXTURE_ID;
    urlStore.mUrl       = ToStdString(imageUrls[i]);
    if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
    {
      // Increase reference count of External Resources :
      // EncodedImageBuffer or ExternalTextures.
      // Reference count will be decreased at destructor of the visual.
      urlStore.mUrl.IncreaseExternalResourceReference(factoryCache.GetTextureManager());
    }
    visual->mImageUrls->push_back(std::move(urlStore));
  }
  visual->mFrameCount = imageUrls.Count();
  visual->SetProperties(properties);

  visual->Initialize();

  return visual;
}

AnimatedImageVisualPtr AnimatedImageVisual::New(VisualFactoryCache&                             factoryCache,
                                                ImageVisualShaderFactory&                       shaderFactory,
                                                Ui::Integration::VisualFactory::CreationOptions creationOptions,
                                                const VisualUrl&                                imageUrl,
                                                ImageDimensions                                 size)
{
  AnimatedImageVisualPtr visual(new AnimatedImageVisual(factoryCache, shaderFactory, creationOptions, size));
  visual->InitializeAnimatedImage(imageUrl);

  visual->Initialize();

  return visual;
}

void AnimatedImageVisual::InitializeAnimatedImage(const VisualUrl& imageUrl)
{
  mImageUrl             = imageUrl;
  mAnimatedImageLoading = AnimatedImageLoading::New(imageUrl.GetUrl(), imageUrl.IsLocalResource());

  // If we fail to load the animated image, we will try to load as a normal image.
  if(!mAnimatedImageLoading)
  {
    mImageUrls = new ImageCache::UrlList();
    mImageUrls->reserve(SINGLE_IMAGE_COUNT);

    for(unsigned int i = 0; i < SINGLE_IMAGE_COUNT; ++i)
    {
      ImageCache::UrlStore urlStore;
      urlStore.mTextureId = TextureManager::INVALID_TEXTURE_ID;
      urlStore.mUrl       = imageUrl;
      if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
      {
        // Increase reference count of External Resources :
        // EncodedImageBuffer or ExternalTextures.
        // Reference count will be decreased at destructor of the visual.
        urlStore.mUrl.IncreaseExternalResourceReference(mFactoryCache.GetTextureManager());
      }
      mImageUrls->push_back(std::move(urlStore));
    }
    mFrameCount = SINGLE_IMAGE_COUNT;
  }
}

void AnimatedImageVisual::CreateImageCache(TextureManager::ReloadPolicy reloadPolicy)
{
  DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise,
                "AnimatedImageVisual::CreateImageCache()  batchSize:%d  cacheSize:%d\n", mBatchSize, mCacheSize);

  TextureManager& textureManager = mFactoryCache.GetTextureManager();

  if(mAnimatedImageLoading)
  {
    mImageCache = new RollingAnimatedImageCache(
      textureManager, mDesiredSize, static_cast<Dali::SamplingMode::Type>(mSamplingMode), mAnimatedImageLoading, mMaskingData, *this,
      mCacheSize, mBatchSize, mWrapModeU, mWrapModeV, IsSynchronousLoadingRequired(), IsPreMultipliedAlphaEnabled(), reloadPolicy);
  }
  else if(mImageUrls)
  {
    // Ensure the batch size and cache size are no bigger than the number of URLs,
    // and that the cache is at least as big as the batch size.
    uint16_t numUrls   = static_cast<uint16_t>(mImageUrls->size());
    uint16_t batchSize = std::max(std::min(mBatchSize, numUrls), MINIMUM_CACHESIZE);
    uint16_t cacheSize = std::max(std::min(std::max(batchSize, mCacheSize), numUrls), MINIMUM_CACHESIZE);

    if(cacheSize < numUrls)
    {
      mImageCache =
        new RollingImageCache(textureManager, mDesiredSize, static_cast<Dali::SamplingMode::Type>(mSamplingMode), *mImageUrls, mMaskingData,
                              *this, cacheSize, batchSize, mFrameDelay, IsPreMultipliedAlphaEnabled());
    }
    else
    {
      mImageCache = new FixedImageCache(textureManager, mDesiredSize, static_cast<Dali::SamplingMode::Type>(mSamplingMode), *mImageUrls,
                                        mMaskingData, *this, batchSize, mFrameDelay, IsPreMultipliedAlphaEnabled());
    }
  }

  if(DALI_UNLIKELY(!mImageCache))
  {
    DALI_LOG_ERROR("mImageCache is null\n");
  }
  else
  {
    mLastRequiredSize = mDesiredSize;
  }
}

AnimatedImageVisual::AnimatedImageVisual(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, Ui::Integration::VisualFactory::CreationOptions creationOptions,
                                         ImageDimensions desiredSize)
: Visual::Base(factoryCache, Ui::Integration::InternalVisualType::ANIMATED_IMAGE),
  mFrameDelayTimer(),
  mPlacementActor(),
  mImageVisualShaderFactory(shaderFactory),
  mNativeTexture(),
  mPixelArea(FULL_TEXTURE_RECT),
  mPixelAreaIndex(Property::INVALID_INDEX),
  mPreMultipliedAlphaIndex(Property::INVALID_INDEX),
  mImageUrl(),
  mAnimatedImageLoading(),
  mFrameIndexForJumpTo(0),
  mCurrentFrameIndex(FIRST_FRAME_INDEX),
  mImageUrls(nullptr),
  mImageCache(nullptr),
  mCacheSize(2),
  mBatchSize(2),
  mFrameDelay(100),
  mLoopCount(LOOP_FOREVER),
  mCurrentLoopIndex(FIRST_LOOP),
  mLoadPolicy(Ui::Image::LoadPolicy::ATTACHED),
  mReleasePolicy(Ui::Image::ReleasePolicy::DETACHED),
  mFittingMode(Ui::Image::FittingMode::FILL),
  mMaskingData(),
  mDesiredSize(desiredSize),
  mFrameSpeedFactor(1.0f),
  mFrameCount(0),
  mImageSize(),
  mActionStatus(Dali::Ui::Integration::AnimatedImageVisual::Action::STOP),
  mWrapModeU(WrapMode::DEFAULT),
  mWrapModeV(WrapMode::DEFAULT),
  mStopBehavior(Ui::AnimatedImage::StopBehavior::CURRENT_FRAME),
  mSamplingMode(Ui::Image::SamplingMode::BOX_THEN_LINEAR),
  mStartFirstFrame(false),
  mIsJumpTo(false),
  mNeedYuvToRgb(false),
  mNeedYuva(false),
  mBrokenImageEnabled(true),
  mRendererAdded(false),
  mUseBrokenImageRenderer(false),
  mUseSynchronousSizing(false)
{
  // Default PRE_MULTIPLIED_ALPHA is false.
  EnablePreMultipliedAlpha(false);

  mImpl->mFittingModeRequired = true;

  if(creationOptions & Ui::Integration::VisualFactory::CreationOptions::IMAGE_VISUAL_IGNORE_VIEW_PADDING)
  {
    mImpl->mFlags |= Visual::Base::Impl::IS_FITTING_MODE_IGNORE_VIEW_PADDING;
  }
}

AnimatedImageVisual::~AnimatedImageVisual()
{
  if(DALI_LIKELY(mImageCache))
  {
    // AnimatedImageVisual destroyed so remove texture unless ReleasePolicy is set to never release
    // If this is animated image, clear cache always.
    // Else if this is single frame image, this is affected be release policy.
    if(mFrameCount > SINGLE_IMAGE_COUNT || mReleasePolicy != Ui::Image::ReleasePolicy::NEVER)
    {
      mImageCache->ClearCache();
    }
  }

  if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
  {
    TextureManager& textureManager = mFactoryCache.GetTextureManager();

    if(mImageUrls != nullptr && !mImageUrls->empty())
    {
      for(const auto& urlStore : *mImageUrls)
      {
        urlStore.mUrl.DecreaseExternalResourceReference(textureManager);
      }
    }

    if(mMaskingData)
    {
      mMaskingData->mAlphaMaskUrl.DecreaseExternalResourceReference(textureManager);
    }
  }
  delete mImageCache;
  delete mImageUrls;
}

void AnimatedImageVisual::GetNaturalSize(Vector2& naturalSize)
{
  if(mUseSynchronousSizing && (mLastRequiredSize.GetWidth() > 0 && mLastRequiredSize.GetHeight() > 0))
  {
    if(mImpl->mRenderer)
    {
      auto textureSet = mImpl->mRenderer.GetTextures();
      if(textureSet && textureSet.GetTextureCount())
      {
        auto texture = textureSet.GetTexture(0);
        if(texture)
        {
          if(mImageSize != ImageDimensions(0, 0))
          {
            naturalSize.x = mImageSize.GetWidth();
            naturalSize.y = mImageSize.GetHeight();
            return;
          }
        }
      }
    }

    naturalSize.x = mLastRequiredSize.GetWidth();
    naturalSize.y = mLastRequiredSize.GetHeight();
    return;
  }
  else if(mDesiredSize.GetWidth() > 0 && mDesiredSize.GetHeight() > 0)
  {
    if(mImpl->mRenderer)
    {
      auto textureSet = mImpl->mRenderer.GetTextures();
      if(textureSet && textureSet.GetTextureCount())
      {
        auto texture = textureSet.GetTexture(0);
        if(texture)
        {
          Dali::Vector2 textureSize;
          textureSize.x = static_cast<float>(texture.GetWidth());
          textureSize.y = static_cast<float>(texture.GetHeight());
          if(textureSize != Vector2::ZERO)
          {
            naturalSize = textureSize;
            return;
          }
        }
      }
    }

    naturalSize.x = mDesiredSize.GetWidth();
    naturalSize.y = mDesiredSize.GetHeight();
    return;
  }

  naturalSize = Vector2::ZERO;
  if(mImageSize.GetWidth() == 0 && mImageSize.GetHeight() == 0)
  {
    if(mMaskingData && mMaskingData->mAlphaMaskUrl.IsValid() && mMaskingData->mCropToMask)
    {
      ImageDimensions dimensions = Dali::GetClosestImageSize(mMaskingData->mAlphaMaskUrl.GetUrl());
      if(dimensions != ImageDimensions(0, 0))
      {
        mImageSize    = dimensions;
        naturalSize.x = dimensions.GetWidth();
        naturalSize.y = dimensions.GetHeight();
        return;
      }
    }

    if(mImageUrl.IsValid() && mAnimatedImageLoading)
    {
      mImageSize = mAnimatedImageLoading.GetImageSize();
    }
    else if(mImageUrls && mImageUrls->size() > 0)
    {
      mImageSize = Dali::GetClosestImageSize((*mImageUrls)[0].mUrl.GetUrl());
    }
  }

  naturalSize.width  = mImageSize.GetWidth();
  naturalSize.height = mImageSize.GetHeight();
}

void AnimatedImageVisual::DoCreatePropertyMap(Property::Map& map) const
{
  map.Clear();

  bool sync = IsSynchronousLoadingRequired();
  map.Insert(Ui::ImageVisualPropertyIndex::SYNCHRONOUS_LOADING, sync);

  map.Insert(Ui::VisualBasePropertyIndex::TYPE, Ui::Integration::InternalVisualType::ANIMATED_IMAGE);

  if(mImageUrl.IsValid())
  {
    map.Insert(Ui::ImageVisualPropertyIndex::URL, ToPropertyValue(mImageUrl.GetUrl()));
  }
  if(mImageUrls != nullptr && !mImageUrls->empty())
  {
    Property::Array urls;
    for(unsigned int i = 0; i < mImageUrls->size(); ++i)
    {
      urls.Add(ToPropertyValue((*mImageUrls)[i].mUrl.GetUrl()));
    }
    Property::Value value(const_cast<Property::Array&>(urls));
    map.Insert(Ui::ImageVisualPropertyIndex::URL, value);
  }

  map.Insert(Ui::ImageVisualPropertyIndex::PRE_MULTIPLIED_ALPHA, IsPreMultipliedAlphaEnabled());

  if(mImpl->mRenderer && mPixelAreaIndex != Property::INVALID_INDEX)
  {
    // Update values from Renderer
    Vector4 pixelArea = mImpl->mRenderer.GetProperty<Vector4>(mPixelAreaIndex);
    map.Insert(Ui::ImageVisualPropertyIndex::PIXEL_AREA, pixelArea);
  }
  else
  {
    map.Insert(Ui::ImageVisualPropertyIndex::PIXEL_AREA, mPixelArea);
  }

  map.Insert(Ui::ImageVisualPropertyIndex::WRAP_MODE_U, mWrapModeU);
  map.Insert(Ui::ImageVisualPropertyIndex::WRAP_MODE_V, mWrapModeV);

  map.Insert(Ui::ImageVisualPropertyIndex::BATCH_SIZE, static_cast<int>(mBatchSize));
  map.Insert(Ui::ImageVisualPropertyIndex::CACHE_SIZE, static_cast<int>(mCacheSize));
  map.Insert(Ui::ImageVisualPropertyIndex::FRAME_DELAY, static_cast<int>(mFrameDelay));
  map.Insert(Ui::ImageVisualPropertyIndex::LOOP_COUNT, static_cast<int>(mLoopCount));

  map.Insert(Ui::ImageVisualPropertyIndex::PLAY_STATE, static_cast<int32_t>(ConvertActionStatusToPlayState(mActionStatus)));

  map.Insert(Ui::ImageVisualPropertyIndex::CURRENT_FRAME_NUMBER,
             (mImageCache) ? static_cast<int32_t>(mImageCache->GetCurrentFrameIndex()) : -1);

  // This returns -1 until the loading is finished.
  auto frameCount = int32_t(mFrameCount);
  if(mImageCache && frameCount == 0)
  {
    frameCount = mImageCache->GetTotalFrameCount();

    if(frameCount <= int32_t(SINGLE_IMAGE_COUNT) && mAnimatedImageLoading &&
       mAnimatedImageLoading.HasLoadingSucceeded())
    {
      frameCount = int32_t(mAnimatedImageLoading.GetImageCount());
    }
    else
    {
      frameCount = -1;
    }
  }

  map.Insert(Ui::ImageVisualPropertyIndex::TOTAL_FRAME_NUMBER, static_cast<int>(frameCount));

  map.Insert(Ui::ImageVisualPropertyIndex::STOP_BEHAVIOR, mStopBehavior);

  if(mMaskingData != nullptr)
  {
    map.Insert(Ui::ImageVisualPropertyIndex::ALPHA_MASK_URL, ToPropertyValue(mMaskingData->mAlphaMaskUrl.GetUrl()));
    map.Insert(Ui::ImageVisualPropertyIndex::MASK_CONTENT_SCALE, mMaskingData->mContentScaleFactor);
    map.Insert(Ui::ImageVisualPropertyIndex::CROP_TO_MASK, mMaskingData->mCropToMask);
    map.Insert(Ui::ImageVisualPropertyIndex::MASKING_TYPE, mMaskingData->mPreappliedMasking
                                                             ? Ui::Image::MaskingType::MASKING_ON_LOADING
                                                             : Ui::Image::MaskingType::MASKING_ON_RENDERING);
  }

  map.Insert(Ui::ImageVisualPropertyIndex::LOAD_POLICY, mLoadPolicy);
  map.Insert(Ui::ImageVisualPropertyIndex::RELEASE_POLICY, mReleasePolicy);
  map.Insert(Ui::ImageVisualPropertyIndex::FITTING_MODE, mFittingMode);
  map.Insert(Ui::ImageVisualPropertyIndex::SAMPLING_MODE, mSamplingMode);
  map.Insert(Ui::ImageVisualPropertyIndex::ENABLE_BROKEN_IMAGE, mBrokenImageEnabled);

  Dali::ImageDimensions size = mUseSynchronousSizing ? mLastRequiredSize : mDesiredSize;

  map.Insert(Ui::ImageVisualPropertyIndex::DESIRED_WIDTH, size.GetWidth());
  map.Insert(Ui::ImageVisualPropertyIndex::DESIRED_HEIGHT, size.GetHeight());

  map.Insert(Ui::ImageVisualPropertyIndex::FRAME_SPEED_FACTOR, mFrameSpeedFactor);
  map.Insert(Ui::ImageVisualPropertyIndex::SYNCHRONOUS_SIZING, mUseSynchronousSizing);
}

void AnimatedImageVisual::DoCreateInstancePropertyMap(Property::Map& map) const
{
  map.Clear();
  map.Insert(Ui::VisualBasePropertyIndex::TYPE, Ui::Integration::InternalVisualType::ANIMATED_IMAGE);

  Dali::ImageDimensions size = mUseSynchronousSizing ? mLastRequiredSize : mDesiredSize;

  map.Insert(Ui::ImageVisualPropertyIndex::DESIRED_WIDTH, size.GetWidth());
  map.Insert(Ui::ImageVisualPropertyIndex::DESIRED_HEIGHT, size.GetHeight());
}

void AnimatedImageVisual::EnablePreMultipliedAlpha(bool preMultiplied)
{
  if(mImpl->mRenderer)
  {
    if(mPreMultipliedAlphaIndex != Property::INVALID_INDEX)
    {
      mImpl->mRenderer.SetProperty(mPreMultipliedAlphaIndex, preMultiplied ? 1.0f : 0.0f);
    }
    else if(!preMultiplied)
    {
      // Register PRE_MULTIPLIED_ALPHA only if it become false.
      // Default PRE_MULTIPLIED_ALPHA value is 1.0f, at image-visual-shader-factory.cpp
      mPreMultipliedAlphaIndex =
        mImpl->mRenderer.RegisterProperty(Ui::ImageVisualPropertyIndex::PRE_MULTIPLIED_ALPHA, PRE_MULTIPLIED_ALPHA, 0.0f);
    }

    mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, preMultiplied);
  }

  if(preMultiplied)
  {
    mImpl->mFlags |= Visual::Base::Impl::IS_PRE_MULTIPLIED_ALPHA;
  }
  else
  {
    mImpl->mFlags &= ~Visual::Base::Impl::IS_PRE_MULTIPLIED_ALPHA;
  }
}

void AnimatedImageVisual::OnDoAction(const Dali::Property::Index actionId, const Dali::Property::Value& attributes)
{
  // Make not set any action when the resource status is already failed.
  if(mImpl->mResourceStatus == Ui::Visual::ResourceStatus::FAILED && actionId != Dali::Ui::Integration::ImageVisual::Action::RELOAD)
  {
    return;
  }

  // Check if action is valid for this visual type and perform action if possible
  switch(actionId)
  {
    case Dali::Ui::Integration::ImageVisual::Action::RELOAD:
    {
      DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "RELOAD\n");

      if(mFrameDelayTimer)
      {
        mFrameDelayTimer.Stop();
        mFrameDelayTimer.Reset();
      }

      if(mImageCache)
      {
        mImageCache->ClearCache();
        delete mImageCache;
        mImageCache = nullptr;
      }

      if(mImageUrl.IsValid())
      {
        mAnimatedImageLoading = AnimatedImageLoading::New(mImageUrl.GetUrl(), mImageUrl.IsLocalResource());
      }

      CreateImageCache(TextureManager::ReloadPolicy::FORCED);

      ResourceReady(Ui::Visual::ResourceStatus::PREPARING);
      mStartFirstFrame   = true;
      mCurrentFrameIndex = FIRST_FRAME_INDEX;
      mCurrentLoopIndex  = FIRST_LOOP;
      mIsJumpTo          = false;

      PrepareTextureSet();
      break;
    }
    case Dali::Ui::Integration::AnimatedImageVisual::Action::PAUSE:
    {
      DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "PAUSE\n");
      // Pause will be executed on next timer tick
      mActionStatus = Dali::Ui::Integration::AnimatedImageVisual::Action::PAUSE;
      break;
    }
    case Dali::Ui::Integration::AnimatedImageVisual::Action::PLAY:
    {
      DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "PLAY\n");
      if(mFrameDelayTimer && IsOnScene() && mActionStatus != Dali::Ui::Integration::AnimatedImageVisual::Action::PLAY)
      {
        mFrameDelayTimer.Start();
      }
      mActionStatus = Dali::Ui::Integration::AnimatedImageVisual::Action::PLAY;
      break;
    }
    case Dali::Ui::Integration::AnimatedImageVisual::Action::STOP:
    {
      // STOP reset functionality will actually be done in a future change
      // Stop will be executed on next timer tick
      mActionStatus     = Dali::Ui::Integration::AnimatedImageVisual::Action::STOP;
      mCurrentLoopIndex = FIRST_LOOP;
      DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "STOP\n");
      if(IsOnScene())
      {
        DisplayNextFrame();
      }
      break;
    }
    case Dali::Ui::Integration::AnimatedImageVisual::Action::JUMP_TO:
    {
      int32_t frameNumber;
      if(attributes.Get(frameNumber))
      {
        if(frameNumber < 0 || frameNumber >= static_cast<int32_t>(mFrameCount))
        {
          DALI_LOG_ERROR("Invalid frame index used.\n");
        }
        else
        {
          mIsJumpTo            = true;
          mFrameIndexForJumpTo = frameNumber;
          DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "JUMP_TO : %u\n", mFrameIndexForJumpTo);
          if(IsOnScene())
          {
            DisplayNextFrame();
          }
        }
      }
      break;
    }
  }
}

void AnimatedImageVisual::DoSetProperties(const Property::Map& propertyMap)
{
  // url[s] already passed in from constructor
  for(Property::Map::SizeType iter = 0; iter < propertyMap.Count(); ++iter)
  {
    KeyValuePair keyValue = propertyMap.GetKeyValue(iter);
    if(keyValue.first.type == Property::Key::INDEX)
    {
      DoSetProperty(keyValue.first.indexKey, keyValue.second);
    }
    else
    {
      for(int i = 0; i < NAME_INDEX_MATCH_TABLE_SIZE; ++i)
      {
        if(keyValue.first == NAME_INDEX_MATCH_TABLE[i].name)
        {
          DoSetProperty(NAME_INDEX_MATCH_TABLE[i].index, keyValue.second);
          break;
        }
      }
    }
  }

  // Load image immediately if LOAD_POLICY requires it
  if(mLoadPolicy == Ui::Image::LoadPolicy::IMMEDIATE)
  {
    PrepareTextureSet();
  }
}

void AnimatedImageVisual::DoSetProperty(Property::Index index, const Property::Value& value)
{
  switch(index)
  {
    case Ui::ImageVisualPropertyIndex::PIXEL_AREA:
    {
      value.Get(mPixelArea);

      if(DALI_UNLIKELY(mImpl->mRenderer))
      {
        // Unusual case. SetProperty called after OnInitialize().
        // Assume that DoAction call UPDATE_PROPERTY.
        if(mPixelAreaIndex != Property::INVALID_INDEX)
        {
          mImpl->mRenderer.SetProperty(mPixelAreaIndex, mPixelArea);
        }
        else if(mPixelArea != FULL_TEXTURE_RECT)
        {
          mPixelAreaIndex = mImpl->mRenderer.RegisterProperty(Ui::ImageVisualPropertyIndex::PIXEL_AREA,
                                                              PIXEL_AREA_UNIFORM_NAME, mPixelArea);
        }
      }
      break;
    }
    case Ui::ImageVisualPropertyIndex::WRAP_MODE_U:
    {
      int wrapMode = 0;
      if(DALI_LIKELY(Scripting::GetEnumerationProperty(value, WRAP_MODE_TABLE, WRAP_MODE_TABLE_COUNT, wrapMode)))
      {
        mWrapModeU = Dali::WrapMode::Type(wrapMode);
      }
      else
      {
        mWrapModeU = Dali::WrapMode::Type::DEFAULT;
      }
      break;
    }
    case Ui::ImageVisualPropertyIndex::WRAP_MODE_V:
    {
      int wrapMode = 0;
      if(DALI_LIKELY(Scripting::GetEnumerationProperty(value, WRAP_MODE_TABLE, WRAP_MODE_TABLE_COUNT, wrapMode)))
      {
        mWrapModeV = Dali::WrapMode::Type(wrapMode);
      }
      else
      {
        mWrapModeV = Dali::WrapMode::Type::DEFAULT;
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::PRE_MULTIPLIED_ALPHA:
    {
      bool premultipliedAlpha = false;
      if(value.Get(premultipliedAlpha))
      {
        EnablePreMultipliedAlpha(premultipliedAlpha);
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::BATCH_SIZE:
    {
      int batchSize;
      if(value.Get(batchSize))
      {
        if(batchSize < 2)
        {
          DALI_LOG_ERROR("The minimum value of batch size is 2.");
        }
        else
        {
          mBatchSize = batchSize;
        }
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::CACHE_SIZE:
    {
      int cacheSize;
      if(value.Get(cacheSize))
      {
        if(cacheSize < 2)
        {
          DALI_LOG_ERROR("The minimum value of cache size is 2.");
        }
        else
        {
          mCacheSize = cacheSize;
        }
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::FRAME_DELAY:
    {
      int frameDelay;
      if(value.Get(frameDelay))
      {
        mFrameDelay = frameDelay;
        if(DALI_LIKELY(mImageCache))
        {
          mImageCache->SetInterval(CalculateInterval(mFrameDelay, mFrameSpeedFactor));
        }
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::LOOP_COUNT:
    {
      int loopCount;
      if(value.Get(loopCount))
      {
        mLoopCount = loopCount;
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::STOP_BEHAVIOR:
    {
      int32_t stopBehavior = static_cast<int32_t>(mStopBehavior);
      if(DALI_LIKELY(Scripting::GetEnumerationProperty(value, STOP_BEHAVIOR_TABLE, STOP_BEHAVIOR_TABLE_COUNT, stopBehavior)))
      {
        mStopBehavior = static_cast<Ui::AnimatedImage::StopBehavior>(stopBehavior);
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::SYNCHRONOUS_LOADING:
    {
      bool sync = false;
      value.Get(sync);
      if(sync)
      {
        mImpl->mFlags |= Visual::Base::Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
      }
      else
      {
        mImpl->mFlags &= ~Visual::Base::Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::ALPHA_MASK_URL:
    {
      std::string alphaUrl = "";
      if(GetStdString(value, alphaUrl))
      {
        AllocateMaskData();
        mMaskingData->mAlphaMaskUrl = alphaUrl;
        if(mMaskingData->mAlphaMaskUrl.IsValid())
        {
          if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
          {
            // Increase reference count of External Resources :
            // EncodedImageBuffer or ExternalTextures.
            // Reference count will be decreased at destructor of the visual.
            mMaskingData->mAlphaMaskUrl.IncreaseExternalResourceReference(mFactoryCache.GetTextureManager());
          }
          if(mMaskingData->mAlphaMaskUrl.GetProtocolType() == VisualUrl::TEXTURE)
          {
            mMaskingData->mPreappliedMasking = false;
          }
        }
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::MASK_CONTENT_SCALE:
    {
      float scale = 1.0f;
      if(value.Get(scale))
      {
        AllocateMaskData();
        mMaskingData->mContentScaleFactor = scale;
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::CROP_TO_MASK:
    {
      bool crop = false;
      if(value.Get(crop))
      {
        AllocateMaskData();
        mMaskingData->mCropToMask = crop;
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::MASKING_TYPE:
    {
      int maskingType = 0;
      if(value.Get(maskingType))
      {
        AllocateMaskData();

        bool externalTextureUsed = false;
        if(mMaskingData->mAlphaMaskUrl.IsValid() &&
           mMaskingData->mAlphaMaskUrl.GetProtocolType() == VisualUrl::TEXTURE)
        {
          externalTextureUsed = true;
        }
        else if(mImageUrls != nullptr && !mImageUrls->empty())
        {
          for(const auto& urlStore : *mImageUrls)
          {
            const auto& imageUrl = urlStore.mUrl;
            if(imageUrl.IsValid() && imageUrl.GetProtocolType() == VisualUrl::TEXTURE)
            {
              externalTextureUsed = true;
              break;
            }
          }
        }

        if(externalTextureUsed)
        {
          // For external textures, only gpu masking is available.
          // Therefore, MASKING_TYPE is set to MASKING_ON_RENDERING forcelly.
          mMaskingData->mPreappliedMasking = false;
        }
        else
        {
          mMaskingData->mPreappliedMasking = (Ui::Image::MaskingType(maskingType) ==
                                              Ui::Image::MaskingType::MASKING_ON_LOADING);
        }
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::ENABLE_BROKEN_IMAGE:
    {
      bool brokenImageEnabled = true;
      if(value.Get(brokenImageEnabled))
      {
        mBrokenImageEnabled = brokenImageEnabled;
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::RELEASE_POLICY:
    {
      int32_t releasePolicy = static_cast<int32_t>(mReleasePolicy);
      if(DALI_LIKELY(Scripting::GetEnumerationProperty(value, RELEASE_POLICY_TABLE, RELEASE_POLICY_TABLE_COUNT, releasePolicy)))
      {
        mReleasePolicy = static_cast<Ui::Image::ReleasePolicy>(releasePolicy);
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::LOAD_POLICY:
    {
      int32_t loadPolicy = static_cast<int32_t>(mLoadPolicy);
      if(DALI_LIKELY(Scripting::GetEnumerationProperty(value, LOAD_POLICY_TABLE, LOAD_POLICY_TABLE_COUNT, loadPolicy)))
      {
        mLoadPolicy = static_cast<Ui::Image::LoadPolicy>(loadPolicy);
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::FITTING_MODE:
    {
      int32_t fittingMode = static_cast<int32_t>(mFittingMode);
      if(DALI_LIKELY(Scripting::GetEnumerationProperty(value, FITTING_MODE_TABLE, FITTING_MODE_TABLE_COUNT, fittingMode)))
      {
        mFittingMode = static_cast<Ui::Image::FittingMode>(fittingMode);
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::SAMPLING_MODE:
    {
      int32_t samplingMode = static_cast<int32_t>(mSamplingMode);
      if(DALI_LIKELY(Scripting::GetEnumerationProperty(value, SAMPLING_MODE_TABLE, SAMPLING_MODE_TABLE_COUNT, samplingMode)))
      {
        mSamplingMode = static_cast<Ui::Image::SamplingMode>(samplingMode);
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::DESIRED_WIDTH:
    {
      float desiredWidth = 0.0f;
      if(value.Get(desiredWidth))
      {
        mDesiredSize.SetWidth(static_cast<Dali::ImageDimensions::IntType>(desiredWidth));
      }
      else
      {
        DALI_LOG_ERROR("AnimatedImageVisual: desiredWidth property has incorrect type\n");
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::DESIRED_HEIGHT:
    {
      float desiredHeight = 0.0f;
      if(value.Get(desiredHeight))
      {
        mDesiredSize.SetHeight(static_cast<Dali::ImageDimensions::IntType>(desiredHeight));
      }
      else
      {
        DALI_LOG_ERROR("AnimatedImageVisual: desiredHeight property has incorrect type\n");
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::FRAME_SPEED_FACTOR:
    {
      float frameSpeedFactor = 1.0f;
      if(value.Get(frameSpeedFactor))
      {
        // TODO : Could we remove this limitation?
        Dali::ClampInPlace(frameSpeedFactor, MINIMUM_FRAME_SPEED_FACTOR, MAXIMUM_FRAME_SPEED_FACTOR);

        if(!Dali::Equals(mFrameSpeedFactor, frameSpeedFactor))
        {
          mFrameSpeedFactor = frameSpeedFactor;
        }
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::SYNCHRONOUS_SIZING:
    {
      bool useSynchronousSizing = false;
      if(value.Get(useSynchronousSizing))
      {
        mUseSynchronousSizing = useSynchronousSizing;
      }
      break;
    }
  }
}

void AnimatedImageVisual::DoSetOnScene(Actor& actor)
{
  mStartFirstFrame = true;
  mPlacementActor  = actor;
  Dali::DevelActor::OnSceneVisibilityChangedSignal(actor).Connect(this, &AnimatedImageVisual::OnControlEffectiveVisibilityChanged);

  // We should clear cached informations before mImageCache->FirstFrame();
  // TODO : Could we remove this cache clearing code?
  if(mReleasePolicy != Ui::Image::ReleasePolicy::DETACHED)
  {
    if(DALI_LIKELY(mImageCache))
    {
      mImageCache->ClearCache();
    }
    mImpl->mResourceStatus = Ui::Visual::ResourceStatus::PREPARING;

    mImpl->mRenderer.RemoveTextures();
  }

  PrepareTextureSet();
}

void AnimatedImageVisual::DoSetOffScene(Actor& actor)
{
  DALI_ASSERT_DEBUG((bool)mImpl->mRenderer && "There should always be a renderer whilst on stage");

  if(mFrameDelayTimer)
  {
    mFrameDelayTimer.Stop();
    mFrameDelayTimer.Reset();
  }

  actor.RemoveRenderer(mImpl->mRenderer);
  mRendererAdded = false;

  if(mReleasePolicy == Ui::Image::ReleasePolicy::DETACHED)
  {
    if(DALI_LIKELY(mImageCache))
    {
      mImageCache->ClearCache();
    }
    mImpl->mResourceStatus = Ui::Visual::ResourceStatus::PREPARING;

    // Remove textureset now.
    mImpl->mRenderer.RemoveTextures();
  }

  mPlacementActor.Reset();
  mStartFirstFrame   = false;
  mCurrentFrameIndex = FIRST_FRAME_INDEX;
  mCurrentLoopIndex  = FIRST_LOOP;

  Dali::DevelActor::OnSceneVisibilityChangedSignal(actor).Disconnect(this, &AnimatedImageVisual::OnControlEffectiveVisibilityChanged);
}

void AnimatedImageVisual::OnSetTransform()
{
  if(mImpl->mRenderer && mImpl->mTransformMapChanged)
  {
    mImpl->SetTransformUniforms(mImpl->mRenderer, Dali::Ui::Integration::Direction::LEFT_TO_RIGHT);
  }

  if(mUseSynchronousSizing)
  {
    // Get current visual size
    Vector2  size                    = mImpl->GetTransformVisualSize(mImpl->mControlSize);
    uint32_t maximumNumber           = std::numeric_limits<uint16_t>::max();
    uint32_t sizeWidth               = static_cast<uint32_t>(roundf(size.width));
    sizeWidth                        = std::min(sizeWidth, maximumNumber);
    uint32_t sizeHeight              = static_cast<uint32_t>(roundf(size.height));
    sizeHeight                       = std::min(sizeHeight, maximumNumber);
    Dali::ImageDimensions visualSize = Dali::ImageDimensions(sizeWidth, sizeHeight);

    // Reload if visual size is updated
    if(mLastRequiredSize != visualSize)
    {
      mLastRequiredSize = visualSize;

      if(DALI_LIKELY(mImageCache))
      {
        DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "mLastRequiredSize : %hux%hu\n", mLastRequiredSize.GetWidth(),
                      mLastRequiredSize.GetHeight());
        mImageCache->SetDesiredSize(mLastRequiredSize);

        // TODO : Is their more good way to request current frame again??
        if(mStartFirstFrame)
        {
          PrepareTextureSet();
        }
        else
        {
          OnDoAction(Dali::Ui::Integration::AnimatedImageVisual::Action::JUMP_TO, static_cast<int32_t>(mCurrentFrameIndex));
        }
      }
    }
  }
}

void AnimatedImageVisual::UpdateShader()
{
  if(mImpl->mRenderer)
  {
    Shader shader = GenerateShader();
    mImpl->mRenderer.SetShader(shader);
  }
}

Shader AnimatedImageVisual::GenerateShader() const
{
  Shader shader;
  if(IsUsingCustomShader())
  {
    shader = Shader::New(ToDaliStringView(mImpl->GetCustomShaderAt(0)->mVertexShader.empty()
                                            ? mImageVisualShaderFactory.GetVertexShaderSource().data()
                                            : mImpl->GetCustomShaderAt(0)->mVertexShader),
                         ToDaliStringView(mImpl->GetCustomShaderAt(0)->mFragmentShader.empty()
                                            ? mImageVisualShaderFactory.GetFragmentShaderSource().data()
                                            : mImpl->GetCustomShaderAt(0)->mFragmentShader),
                         mImpl->GetCustomShaderAt(0)->mHints);

    shader.ReserveCustomProperties(4);
    shader.RegisterUniqueProperty("viewEffectiveScale", 1.0f);
    shader.RegisterUniqueProperty("visualTransformUseEffectiveScale", 1.0f);
    shader.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT);

    // Most of image visual shader user (like svg, animated vector image visual) use pre-multiplied alpha.
    // If the visual dont want to using pre-multiplied alpha, it should be set as 0.0f as renderer side.
    shader.RegisterProperty(PRE_MULTIPLIED_ALPHA, ALPHA_VALUE_PREMULTIPLIED);
  }
  else
  {
    bool defaultWrapMode = mWrapModeU <= WrapMode::CLAMP_TO_EDGE && mWrapModeV <= WrapMode::CLAMP_TO_EDGE;
    bool requiredAlphaMaskingOnRendering =
      (mMaskingData && !mMaskingData->mMaskImageLoadingFailed) ? !mMaskingData->mPreappliedMasking : false;

    shader = mImageVisualShaderFactory.GetShader(
      mFactoryCache, ImageVisualShaderFeature::FeatureBuilder()
                       .ApplyDefaultTextureWrapMode(defaultWrapMode)
                       .EnableRoundedCorner(IsRoundedCornerRequired(), IsSquircleCornerRequired())
                       .EnableBorderline(IsBorderlineRequired())
                       .SetTextureForFragmentShaderCheck(mNativeTexture)
                       .EnableAlphaMaskingOnRendering(requiredAlphaMaskingOnRendering)
                       .EnableYuvToRgb(mNeedYuvToRgb, mNeedYuva, false));
  }
  return shader;
}

Dali::Property AnimatedImageVisual::OnGetPropertyObject(Dali::Property::Key key, bool changeProperties)
{
  if((key.type == Property::Key::INDEX && key.indexKey == Ui::ImageVisualPropertyIndex::PIXEL_AREA) ||
     (key.type == Property::Key::STRING && key.stringKey == PIXEL_AREA_UNIFORM_NAME))
  {
    if(DALI_LIKELY(mImpl->mRenderer))
    {
      if(mPixelAreaIndex == Property::INVALID_INDEX)
      {
        mPixelAreaIndex = mImpl->mRenderer.RegisterProperty(Ui::ImageVisualPropertyIndex::PIXEL_AREA,
                                                            PIXEL_AREA_UNIFORM_NAME, mPixelArea);
      }
      return Dali::Property(mImpl->mRenderer, mPixelAreaIndex);
    }
  }

  Handle handle;
  return Dali::Property(handle, Property::INVALID_INDEX);
}

void AnimatedImageVisual::OnInitialize()
{
  CreateImageCache();

  bool   defaultWrapMode = mWrapModeU <= WrapMode::CLAMP_TO_EDGE && mWrapModeV <= WrapMode::CLAMP_TO_EDGE;
  Shader shader          = GenerateShader();

  Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);

  mImpl->mRenderer = DecoratedVisualRenderer::New(geometry, shader);

  // Register transform properties
  mImpl->SetTransformUniforms(mImpl->mRenderer, Dali::Ui::Integration::Direction::LEFT_TO_RIGHT);

  if(!defaultWrapMode) // custom wrap mode
  {
    Vector2 wrapMode(static_cast<float>(mWrapModeU - WrapMode::CLAMP_TO_EDGE),
                     static_cast<float>(mWrapModeV - WrapMode::CLAMP_TO_EDGE));
    wrapMode.Clamp(Vector2::ZERO, Vector2(2.f, 2.f));
    mImpl->mRenderer.RegisterUniqueProperty(WRAP_MODE_UNIFORM_NAME, wrapMode);
  }

  if(mPixelArea != FULL_TEXTURE_RECT)
  {
    mPixelAreaIndex = mImpl->mRenderer.RegisterUniqueProperty(Ui::ImageVisualPropertyIndex::PIXEL_AREA,
                                                              PIXEL_AREA_UNIFORM_NAME, mPixelArea);
  }

  // Enable PreMultipliedAlpha if it need.
  auto preMultiplyOnLoad = IsPreMultipliedAlphaEnabled() && !IsUsingCustomShader()
                             ? TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD
                             : TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  EnablePreMultipliedAlpha(preMultiplyOnLoad == TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD);

  if(mMaskingData)
  {
    mImpl->mRenderer.RegisterUniqueProperty(Ui::ImageVisualPropertyIndex::CROP_TO_MASK, CROP_TO_MASK_NAME,
                                            static_cast<float>(mMaskingData->mCropToMask));
  }
}

void AnimatedImageVisual::StartFirstFrame(TextureSet& textureSet, uint32_t firstInterval)
{
  DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "AnimatedImageVisual::StartFirstFrame()\n");

  mStartFirstFrame = false;
  if(mImpl->mRenderer && DALI_LIKELY(textureSet))
  {
    SetTexturesToRenderer(textureSet);

    if(!mRendererAdded)
    {
      Actor actor = mPlacementActor.GetHandle();
      if(actor)
      {
        mRendererAdded = true;
        actor.AddRenderer(mImpl->mRenderer);
        mPlacementActor.Reset();
      }
    }
  }

  mCurrentFrameIndex = FIRST_FRAME_INDEX;
  if(mImpl->mResourceStatus != Ui::Visual::ResourceStatus::FAILED)
  {
    if(mFrameCount > SINGLE_IMAGE_COUNT)
    {
      mFrameDelayTimer = Timer::New(CalculateInterval(firstInterval, mFrameSpeedFactor));
      mFrameDelayTimer.TickSignal().Connect(this, &AnimatedImageVisual::DisplayNextFrame);
      if(mActionStatus == Dali::Ui::Integration::AnimatedImageVisual::Action::PLAY)
      {
        mFrameDelayTimer.Start();
      }
    }

    DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "ResourceReady(ResourceStatus::READY)\n");
    ResourceReady(Ui::Visual::ResourceStatus::READY);
  }
}

void AnimatedImageVisual::PrepareTextureSet()
{
  TextureSet textureSet;
  if(DALI_LIKELY(mImageCache))
  {
    textureSet = mImageCache->FirstFrame();
  }
  else
  {
    // preMultiplied should be false because broken image don't premultiply alpha on load
    FrameReady(TextureSet(), 0, false);
  }

  // Check whether synchronous loading is true or false for the first frame.
  if(textureSet)
  {
    SetImageSize(textureSet);
  }
}

void AnimatedImageVisual::SetImageSize(TextureSet& textureSet)
{
  if(DALI_LIKELY(textureSet && textureSet.GetTextureCount() > 0u))
  {
    Texture texture = textureSet.GetTexture(0);
    if(texture)
    {
      mImageSize.SetWidth(texture.GetWidth());
      mImageSize.SetHeight(texture.GetHeight());
    }

    if(textureSet.GetTextureCount() >= TEXTURE_COUNT_FOR_GPU_ALPHA_MASK && mMaskingData && mMaskingData->mCropToMask)
    {
      Texture maskTexture = textureSet.GetTexture(1);
      if(maskTexture)
      {
        mImageSize.SetWidth(std::min(static_cast<uint32_t>(mImageSize.GetWidth() * mMaskingData->mContentScaleFactor),
                                     maskTexture.GetWidth()));
        mImageSize.SetHeight(std::min(static_cast<uint32_t>(mImageSize.GetHeight() * mMaskingData->mContentScaleFactor),
                                      maskTexture.GetHeight()));

        float   textureWidth  = std::max(static_cast<float>(texture.GetWidth() * mMaskingData->mContentScaleFactor),
                                         Dali::Math::MACHINE_EPSILON_1);
        float   textureHeight = std::max(static_cast<float>(texture.GetHeight() * mMaskingData->mContentScaleFactor),
                                         Dali::Math::MACHINE_EPSILON_1);
        Vector2 textureRatio(std::min(static_cast<float>(maskTexture.GetWidth()), textureWidth) / textureWidth,
                             std::min(static_cast<float>(maskTexture.GetHeight()), textureHeight) / textureHeight);
        mImpl->mRenderer.RegisterProperty(MASK_TEXTURE_RATIO_NAME, textureRatio);
      }
    }
  }

  DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "mImageSize : %hux%hu\n", mImageSize.GetWidth(),
                mImageSize.GetHeight());
}

void AnimatedImageVisual::FrameReady(TextureSet textureSet, uint32_t interval, bool preMultiplied)
{
  EnablePreMultipliedAlpha(preMultiplied);

  // When image visual requested to load new frame to mImageCache and it is failed.
  if(!mImageCache || !textureSet)
  {
    SetLoadingFailed();
    return;
  }
  if(mImpl->mResourceStatus == Ui::Visual::ResourceStatus::FAILED)
  {
    // When loading is failed, FrameReady can be called with valid textureSet because of the asynchronous loading.
    // In this case, just ignore it because ResourceReady with FAILED status is already sent.
    return;
  }

  SetImageSize(textureSet);

  if(mStartFirstFrame)
  {
    if(DALI_LIKELY(mImageCache))
    {
      mFrameCount = mImageCache->GetTotalFrameCount();
    }
    StartFirstFrame(textureSet, interval);
  }
  else
  {
    if(mImpl->mRenderer)
    {
      SetTexturesToRenderer(textureSet);

      if(mFrameDelayTimer && interval > 0u)
      {
        mFrameDelayTimer.SetInterval(CalculateInterval(interval, mFrameSpeedFactor));
      }
    }
  }
}

bool AnimatedImageVisual::DisplayNextFrame()
{
  TextureSet textureSet;
  bool       continueTimer = false;

  if(DALI_LIKELY(mImageCache))
  {
    uint32_t frameIndex = mImageCache->GetCurrentFrameIndex();

    if(mIsJumpTo)
    {
      mIsJumpTo  = false;
      frameIndex = mFrameIndexForJumpTo;
    }
    else if(mActionStatus == Dali::Ui::Integration::AnimatedImageVisual::Action::PAUSE)
    {
      return false;
    }
    else if(mActionStatus == Dali::Ui::Integration::AnimatedImageVisual::Action::STOP)
    {
      mCurrentLoopIndex = FIRST_LOOP;
      if(mStopBehavior == Ui::AnimatedImage::StopBehavior::FIRST_FRAME)
      {
        frameIndex = FIRST_FRAME_INDEX;
      }
      else if(mStopBehavior == Ui::AnimatedImage::StopBehavior::LAST_FRAME)
      {
        frameIndex = mFrameCount - 1;
      }
      else
      {
        return false; // Do not draw already rendered scene twice.
      }
    }
    else
    {
      if(mFrameCount > SINGLE_IMAGE_COUNT)
      {
        frameIndex++;
        if(frameIndex >= mFrameCount)
        {
          frameIndex = FIRST_FRAME_INDEX;
          ++mCurrentLoopIndex;
        }

        if(mLoopCount >= 0 && mCurrentLoopIndex >= mLoopCount)
        {
          // This will stop timer
          mActionStatus      = Dali::Ui::Integration::AnimatedImageVisual::Action::STOP;
          bool continueTimer = DisplayNextFrame();

          // Naturally stopped animation. Send signal.
          if(mImpl->mEventObserver)
          {
            mImpl->mEventObserver->NotifyVisualEvent(*this, Dali::Ui::Integration::AnimatedImageVisual::Signal::ANIMATION_FINISHED);
          }

          return continueTimer;
        }
      }
    }

    DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise,
                  "AnimatedImageVisual::DisplayNextFrame(this:%p) CurrentFrameIndex:%d\n", this, frameIndex);

    textureSet = mImageCache->Frame(frameIndex);

    if(textureSet)
    {
      SetImageSize(textureSet);

      SetTexturesToRenderer(textureSet);

      if(mFrameDelayTimer)
      {
        mFrameDelayTimer.SetInterval(CalculateInterval(mImageCache->GetFrameInterval(frameIndex), mFrameSpeedFactor));
      }
    }

    mCurrentFrameIndex = frameIndex;
    continueTimer      = (mActionStatus == Dali::Ui::Integration::AnimatedImageVisual::Action::PLAY && textureSet) ? true : false;
  }

  return continueTimer;
}

void AnimatedImageVisual::SetLoadingFailed()
{
  Actor   actor     = mPlacementActor.GetHandle();
  Vector2 imageSize = Vector2::ZERO;
  if(actor)
  {
    imageSize = actor.GetProperty(Actor::Property::SIZE).Get<Vector2>();
  }

  if(mBrokenImageEnabled)
  {
    if(DALI_LIKELY(mImpl->mRenderer))
    {
      mUseBrokenImageRenderer = true;
      mFactoryCache.UpdateBrokenImageRenderer(mImpl->mRenderer, imageSize);
      TextureSet textureSet = mImpl->mRenderer.GetTextures();

      SetImageSize(textureSet);

      if(!mRendererAdded)
      {
        if(actor)
        {
          mRendererAdded = true;
          actor.AddRenderer(mImpl->mRenderer);
          mPlacementActor.Reset();
        }
      }
    }
  }
  else
  {
    if(mRendererAdded)
    {
      if(actor)
      {
        actor.RemoveRenderer(mImpl->mRenderer);
        mRendererAdded = false;
      }
    }
  }

  if(mFrameDelayTimer)
  {
    mFrameDelayTimer.Stop();
    mFrameDelayTimer.Reset();
  }

  DALI_LOG_INFO(gAnimImgLogFilter, Debug::Concise, "ResourceReady(ResourceStatus::FAILED)\n");
  ResourceReady(Ui::Visual::ResourceStatus::FAILED);
}

void AnimatedImageVisual::AllocateMaskData()
{
  if(!mMaskingData)
  {
    mMaskingData.reset(new TextureManager::MaskingData());

    // Note : If input url was TEXTURE protocol, it will fail to create AnimatedImageLoading.
    // So it should be added at mImageUrls.
    if(mImageUrls != nullptr && !mImageUrls->empty())
    {
      for(const auto& urlStore : *mImageUrls)
      {
        const auto& imageUrl = urlStore.mUrl;
        if(imageUrl.IsValid() && imageUrl.GetProtocolType() == VisualUrl::TEXTURE)
        {
          mMaskingData->mPreappliedMasking = false;
          break;
        }
      }
    }
  }
}

bool AnimatedImageVisual::CheckMaskTexture()
{
  bool needShaderUpdate = false;
  if(mMaskingData && !mMaskingData->mPreappliedMasking)
  {
    bool       maskLoadFailed = true;
    TextureSet textures       = mImpl->mRenderer.GetTextures();
    if(textures && textures.GetTextureCount() >= TEXTURE_COUNT_FOR_GPU_ALPHA_MASK)
    {
      maskLoadFailed = false;
    }
    if(mMaskingData->mMaskImageLoadingFailed != maskLoadFailed)
    {
      mMaskingData->mMaskImageLoadingFailed = maskLoadFailed;
      needShaderUpdate                      = true;
    }
  }
  return needShaderUpdate;
}

bool AnimatedImageVisual::UpdateNativeTextureInfomation(TextureSet& textureSet)
{
  const bool wasNativeTexture = !!mNativeTexture;

  // Reset previous flags and infomations.
  mNativeTexture.Reset();

  if(textureSet && textureSet.GetTextureCount() > 0u)
  {
    Texture texture = textureSet.GetTexture(0u);
    if(DevelTexture::IsNative(texture))
    {
      // Keep native texture handle.
      mNativeTexture = texture;
    }
  }

  return (wasNativeTexture != (!!mNativeTexture));
}

bool AnimatedImageVisual::UpdateYuvInformation(TextureSet& textureSet)
{
  const bool wasYuv  = mNeedYuvToRgb;
  const bool wasYuva = mNeedYuva;

  mNeedYuvToRgb = false;
  mNeedYuva     = false;

  if(DALI_LIKELY(textureSet) && textureSet.GetTextureCount() >= TEXTURE_COUNT_FOR_GPU_YUV_TO_RGB)
  {
    if(textureSet.GetTexture(0).GetPixelFormat() == Pixel::L8 &&
       textureSet.GetTexture(1).GetPixelFormat() == Pixel::CHROMINANCE_U &&
       textureSet.GetTexture(2).GetPixelFormat() == Pixel::CHROMINANCE_V)
    {
      mNeedYuvToRgb = true;
    }
    mNeedYuva = (textureSet.GetTextureCount() > TEXTURE_COUNT_FOR_GPU_YUV_TO_RGB) ? true : false;
  }

  return (wasYuv != mNeedYuvToRgb || wasYuva != mNeedYuva);
}

void AnimatedImageVisual::SetTexturesToRenderer(TextureSet& textureSet)
{
  if(mImpl->mRenderer && DALI_LIKELY(textureSet))
  {
    mImpl->mRenderer.SetTextures(textureSet);

    bool needToUpdateShader = mUseBrokenImageRenderer;

    // TODO : Change shader whenever information changes for image sequence cases, might be heavy operation.
    if(!IsUsingCustomShader() || needToUpdateShader)
    {
      needToUpdateShader |= UpdateNativeTextureInfomation(textureSet);
      needToUpdateShader |= UpdateYuvInformation(textureSet);
      needToUpdateShader |= CheckMaskTexture();
      if(needToUpdateShader)
      {
        UpdateShader();
      }
    }

    if(DALI_UNLIKELY(mUseBrokenImageRenderer))
    {
      // We need to re-generate geometry only if it was broken image before, and result changed after Reload.
      auto geometry = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);

      // Update geometry only if we need.
      if(geometry)
      {
        mImpl->mRenderer.SetGeometry(geometry);
      }
    }

    // We don't use broken image anymore.
    mUseBrokenImageRenderer = false;
  }
}

void AnimatedImageVisual::OnControlEffectiveVisibilityChanged(Actor actor, bool visible)
{
  if(!visible && mActionStatus != Dali::Ui::Integration::AnimatedImageVisual::Action::STOP)
  {
    mActionStatus = Dali::Ui::Integration::AnimatedImageVisual::Action::STOP;
    DisplayNextFrame();
    DALI_LOG_INFO(gAnimImgLogFilter, Debug::Verbose,
                  "AnimatedImageVisual::OnControlEffectiveVisibilityChanged: invisibile. Pause animation [%p]\n", this);
  }
}

void AnimatedImageVisual::SetFittingMode(Ui::Image::FittingMode fittingMode)
{
  mFittingMode = fittingMode;
}

void AnimatedImageVisual::OnApplyFittingMode(const Vector2& controlSize, const Insets& padding, float effectiveScale)
{
  DoApplyFittingMode(controlSize, padding, effectiveScale, mFittingMode);
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
