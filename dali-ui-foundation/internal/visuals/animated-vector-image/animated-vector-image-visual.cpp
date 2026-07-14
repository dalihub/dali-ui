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
#include <dali-ui-foundation/internal/visuals/animated-vector-image/animated-vector-image-visual.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/rendering/decorated-visual-renderer.h>
#include <dali/integration-api/string-utils.h>
#include <dali/integration-api/texture-integ.h>
#include <dali/public-api/math/math-utils.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/visuals/animated-vector-image-visual-signals-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-actions-integ.h>
#include <dali-ui-foundation/internal/visuals/animated-vector-image/vector-animation-manager.h>
#include <dali-ui-foundation/internal/visuals/image/image-visual-shader-factory.h>
#include <dali-ui-foundation/internal/visuals/image/image-visual-shader-feature-builder.h>
#include <dali-ui-foundation/internal/visuals/visual-base-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-cache.h>
#include <dali-ui-foundation/internal/visuals/visual-string-constants.h>
#include <dali-ui-foundation/public-api/visuals/image-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

using Dali::Integration::GetStdString;
using Dali::Integration::ToDaliString;
using Dali::Integration::ToDaliStringView;
using Dali::Integration::ToPropertyValue;

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
const Dali::Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

constexpr float ALPHA_VALUE_PREMULTIPLIED(1.0f);

// stop behavior
DALI_ENUM_TO_STRING_TABLE_BEGIN(STOP_BEHAVIOR)
  DALI_ENUM_CLASS_TO_STRING_WITH_SCOPE(Dali::Ui::AnimatedImage::StopBehavior, CURRENT_FRAME)
  DALI_ENUM_CLASS_TO_STRING_WITH_SCOPE(Dali::Ui::AnimatedImage::StopBehavior, FIRST_FRAME)
  DALI_ENUM_CLASS_TO_STRING_WITH_SCOPE(Dali::Ui::AnimatedImage::StopBehavior, LAST_FRAME)
DALI_ENUM_TO_STRING_TABLE_END(STOP_BEHAVIOR)

// looping mode
DALI_ENUM_TO_STRING_TABLE_BEGIN(LOOPING_MODE)
  DALI_ENUM_CLASS_TO_STRING_WITH_SCOPE(Dali::Ui::LottieAnimation::LoopingMode, RESTART)
  DALI_ENUM_CLASS_TO_STRING_WITH_SCOPE(Dali::Ui::LottieAnimation::LoopingMode, AUTO_REVERSE)
DALI_ENUM_TO_STRING_TABLE_END(LOOPING_MODE)

// release policies
DALI_ENUM_TO_STRING_TABLE_BEGIN(RELEASE_POLICY)
  DALI_ENUM_CLASS_TO_STRING_WITH_SCOPE(Dali::Ui::Image::ReleasePolicy, DETACHED)
  DALI_ENUM_CLASS_TO_STRING_WITH_SCOPE(Dali::Ui::Image::ReleasePolicy, DESTROYED)
  DALI_ENUM_CLASS_TO_STRING_WITH_SCOPE(Dali::Ui::Image::ReleasePolicy, NEVER)
DALI_ENUM_TO_STRING_TABLE_END(RELEASE_POLICY)

constexpr float MINIMUM_FRAME_SPEED_FACTOR(0.01f);
constexpr float MAXIMUM_FRAME_SPEED_FACTOR(100.0f);

struct NameIndexMatch
{
  const char* const name;
  Property::Index   index;
};

const NameIndexMatch NAME_INDEX_MATCH_TABLE[] = {
  {SYNCHRONOUS_LOADING, Ui::ImageVisualPropertyIndex::SYNCHRONOUS_LOADING},
  {IMAGE_DESIRED_WIDTH, Ui::ImageVisualPropertyIndex::DESIRED_WIDTH},
  {IMAGE_DESIRED_HEIGHT, Ui::ImageVisualPropertyIndex::DESIRED_HEIGHT},
  {RELEASE_POLICY_NAME, Ui::ImageVisualPropertyIndex::RELEASE_POLICY},
  {FITTING_MODE, Ui::ImageVisualPropertyIndex::FITTING_MODE},
  {LOOP_COUNT_NAME, Ui::ImageVisualPropertyIndex::LOOP_COUNT},
  {PLAY_RANGE_NAME, Ui::ImageVisualPropertyIndex::PLAY_RANGE},
  {STOP_BEHAVIOR_NAME, Ui::ImageVisualPropertyIndex::STOP_BEHAVIOR},
  {LOOPING_MODE_NAME, Ui::ImageVisualPropertyIndex::LOOPING_MODE},
  {REDRAW_IN_SCALING_DOWN_NAME, Ui::ImageVisualPropertyIndex::REDRAW_IN_SCALING_DOWN},
  {REDRAW_IN_SCALING_UP_NAME, Ui::ImageVisualPropertyIndex::REDRAW_IN_SCALING_UP},
  {ENABLE_FRAME_CACHE, Ui::ImageVisualPropertyIndex::ENABLE_FRAME_CACHE},
  {NOTIFY_AFTER_RASTERIZATION, Ui::ImageVisualPropertyIndex::NOTIFY_AFTER_RASTERIZATION},
  {FRAME_SPEED_FACTOR, Ui::ImageVisualPropertyIndex::FRAME_SPEED_FACTOR},
  {RENDER_SCALE_NAME, Ui::ImageVisualPropertyIndex::RENDER_SCALE},
  {ENABLE_ASPECT_FIT_NAME, Ui::ImageVisualPropertyIndex::ENABLE_ASPECT_FIT},
};
const int NAME_INDEX_MATCH_TABLE_SIZE = sizeof(NAME_INDEX_MATCH_TABLE) / sizeof(NAME_INDEX_MATCH_TABLE[0]);

#if defined(DEBUG_ENABLED)
Debug::Filter* gVectorAnimationLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_VECTOR_ANIMATION");
#endif

#if defined(GPU_MEMORY_PROFILE_ENABLED)
Dali::PixelData GetDummyRGBAPixelData()
{
  static Dali::PixelData pixelDataRGBA = PixelData::New(new uint8_t[4]{0x00, 0x00, 0x00, 0x00}, 4, 1, 1, Pixel::RGBA8888, PixelData::DELETE_ARRAY);
  return pixelDataRGBA;
}
#endif
} // unnamed namespace

AnimatedVectorImageVisualPtr AnimatedVectorImageVisual::New(VisualFactoryCache&                             factoryCache,
                                                            ImageVisualShaderFactory&                       shaderFactory,
                                                            Ui::Integration::VisualFactory::CreationOptions creationOptions,
                                                            const VisualUrl&                                imageUrl,
                                                            const Property::Map&                            properties)
{
  AnimatedVectorImageVisualPtr visual(
    new AnimatedVectorImageVisual(factoryCache, shaderFactory, creationOptions, imageUrl, ImageDimensions{}));
  visual->SetProperties(properties);
  visual->Initialize();
  return visual;
}

AnimatedVectorImageVisualPtr AnimatedVectorImageVisual::New(VisualFactoryCache&                             factoryCache,
                                                            ImageVisualShaderFactory&                       shaderFactory,
                                                            Ui::Integration::VisualFactory::CreationOptions creationOptions,
                                                            const VisualUrl&                                imageUrl,
                                                            ImageDimensions                                 size)
{
  AnimatedVectorImageVisualPtr visual(new AnimatedVectorImageVisual(factoryCache, shaderFactory, creationOptions, imageUrl, size));
  visual->Initialize();
  return visual;
}

AnimatedVectorImageVisual::AnimatedVectorImageVisual(VisualFactoryCache&                             factoryCache,
                                                     ImageVisualShaderFactory&                       shaderFactory,
                                                     Ui::Integration::VisualFactory::CreationOptions creationOptions,
                                                     const VisualUrl&                                imageUrl,
                                                     ImageDimensions                                 size)
: Visual::Base(factoryCache, Ui::Integration::InternalVisualType::ANIMATED_VECTOR_IMAGE),
  mImageUrl(imageUrl),
  mAnimationData(),
  mVectorAnimationTask(new VectorAnimationTask(factoryCache)),
  mImageVisualShaderFactory(shaderFactory),
  mVisualSize(),
  mVisualScale(Vector2::ONE),
  mDesiredSize(size),
  mPlacementActor(),
  mEventCallback(nullptr),
  mPixelArea(0.f, 0.f, 1.f, 1.f),
  mPixelAreaIndex(Property::INVALID_INDEX),
  mFrameSpeedFactor(1.0f),
  mRenderScale(1.0f),
  mPlayState(Ui::AnimatedImage::PlayState::STOPPED),
  mReleasePolicy(Ui::Image::ReleasePolicy::DETACHED),
  mLastSentPlayStateId(0u),
  mRasterizeCompleted(false),
  mLoadFailed(false),
  mRendererAdded(false),
  mRedrawInScalingDown(true),
  mRedrawInScalingUp(true),
  mFrameCacheEnabled(false),
  mUseNativeImage(false),
  mNotifyAfterRasterization(false)
{
  // the rasterized image is with pre-multiplied alpha format
  mImpl->mFlags |= Visual::Base::Impl::IS_PRE_MULTIPLIED_ALPHA;

  // By default, load a file synchronously
  mImpl->mFlags |= Visual::Base::Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;

  mImpl->mFittingModeRequired = true;

  if(creationOptions & Ui::Integration::VisualFactory::CreationOptions::IMAGE_VISUAL_IGNORE_VIEW_PADDING)
  {
    mImpl->mFlags |= Visual::Base::Impl::IS_FITTING_MODE_IGNORE_VIEW_PADDING;
  }
}

AnimatedVectorImageVisual::~AnimatedVectorImageVisual()
{
  if(Dali::Adaptor::IsAvailable())
  {
    if(mImageUrl.IsBufferResource())
    {
      TextureManager& textureManager = mFactoryCache.GetTextureManager();
      textureManager.RemoveEncodedImageBuffer(mImageUrl);
    }

    if(mEventCallback)
    {
      mFactoryCache.GetVectorAnimationManager().UnregisterEventCallback(mEventCallback);
      mEventCallback = nullptr;
    }

    // Finalize animation task and disconnect the signal in the main thread
    mVectorAnimationTask->ResourceReadySignal().Disconnect(this, &AnimatedVectorImageVisual::OnResourceReady);
    mVectorAnimationTask->Finalize();
  }
}

void AnimatedVectorImageVisual::GetNaturalSize(Vector2& naturalSize)
{
  if(mDesiredSize.GetWidth() > 0 && mDesiredSize.GetHeight() > 0)
  {
    naturalSize.x = mDesiredSize.GetWidth();
    naturalSize.y = mDesiredSize.GetHeight();
  }
  else
  {
    if(mLoadFailed && mImpl->mRenderer)
    {
      // Load failed, use broken image size
      auto textureSet = mImpl->mRenderer.GetTextures();
      if(textureSet && textureSet.GetTextureCount())
      {
        auto texture = textureSet.GetTexture(0);
        if(texture)
        {
          naturalSize.x = texture.GetWidth();
          naturalSize.y = texture.GetHeight();
          return;
        }
      }
    }
    else
    {
      uint32_t width, height;
      mVectorAnimationTask->GetDefaultSize(width, height);
      if(width > 0u && height > 0u)
      {
        naturalSize.x = width;
        naturalSize.y = height;
      }
      else if(mVisualSize != Vector2::ZERO)
      {
        // Animation not yet loaded: fall back to last rendered size
        naturalSize = mVisualSize;
      }
    }
  }

  DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose,
                "AnimatedVectorImageVisual::GetNaturalSize: w = %f, h = %f [%p]\n", naturalSize.width,
                naturalSize.height, this);
}

void AnimatedVectorImageVisual::DoCreatePropertyMap(Property::Map& map) const
{
  map.Clear();
  map.Insert(Ui::VisualBasePropertyIndex::TYPE, Ui::Integration::InternalVisualType::ANIMATED_VECTOR_IMAGE);
  if(mImageUrl.IsValid())
  {
    map.Insert(Ui::ImageVisualPropertyIndex::URL, ToPropertyValue(mImageUrl.GetUrl()));
  }
  map.Insert(Ui::ImageVisualPropertyIndex::LOOP_COUNT, mAnimationData.loopCount);

  uint32_t startFrame, endFrame;
  mVectorAnimationTask->GetPlayRange(startFrame, endFrame);

  Property::Array playRange;
  playRange.PushBack(static_cast<int32_t>(startFrame));
  playRange.PushBack(static_cast<int32_t>(endFrame));
  map.Insert(Ui::ImageVisualPropertyIndex::PLAY_RANGE, playRange);

  map.Insert(Ui::ImageVisualPropertyIndex::PLAY_STATE, static_cast<int32_t>(mPlayState));
  map.Insert(Ui::ImageVisualPropertyIndex::CURRENT_FRAME_NUMBER,
             static_cast<int32_t>(mVectorAnimationTask->GetCurrentFrameNumber()));
  map.Insert(Ui::ImageVisualPropertyIndex::TOTAL_FRAME_NUMBER,
             static_cast<int32_t>(mVectorAnimationTask->GetTotalFrameNumber()));

  map.Insert(Ui::ImageVisualPropertyIndex::STOP_BEHAVIOR, mAnimationData.stopBehavior);
  map.Insert(Ui::ImageVisualPropertyIndex::LOOPING_MODE, mAnimationData.loopingMode);
  map.Insert(Ui::ImageVisualPropertyIndex::REDRAW_IN_SCALING_DOWN, mRedrawInScalingDown);
  map.Insert(Ui::ImageVisualPropertyIndex::REDRAW_IN_SCALING_UP, mRedrawInScalingUp);

  Property::Map layerInfo;
  mVectorAnimationTask->GetLayerInfo(layerInfo);
  map.Insert(Ui::ImageVisualPropertyIndex::CONTENT_INFO, layerInfo);

  Property::Map markerInfo;
  mVectorAnimationTask->GetMarkerInfo(markerInfo);
  map.Insert(Ui::ImageVisualPropertyIndex::MARKER_INFO, markerInfo);

  map.Insert(Ui::ImageVisualPropertyIndex::SYNCHRONOUS_LOADING, IsSynchronousLoadingRequired());
  map.Insert(Ui::ImageVisualPropertyIndex::DESIRED_WIDTH, mDesiredSize.GetWidth());
  map.Insert(Ui::ImageVisualPropertyIndex::DESIRED_HEIGHT, mDesiredSize.GetHeight());
  map.Insert(Ui::ImageVisualPropertyIndex::RELEASE_POLICY, mReleasePolicy);

  map.Insert(Ui::ImageVisualPropertyIndex::ENABLE_FRAME_CACHE, mFrameCacheEnabled);
  map.Insert(Ui::ImageVisualPropertyIndex::NOTIFY_AFTER_RASTERIZATION, mNotifyAfterRasterization);
  map.Insert(Ui::ImageVisualPropertyIndex::FRAME_SPEED_FACTOR, mFrameSpeedFactor);
  map.Insert(Ui::ImageVisualPropertyIndex::RENDER_SCALE, mRenderScale);
  map.Insert(Ui::ImageVisualPropertyIndex::ENABLE_ASPECT_FIT, mVectorAnimationTask->IsAspectFitEnabled());
}

void AnimatedVectorImageVisual::DoCreateInstancePropertyMap(Property::Map& map) const
{
  // Do nothing
}

void AnimatedVectorImageVisual::DoSetProperties(const Property::Map& propertyMap)
{
  // url already passed in from constructor
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

  TriggerVectorRasterization();
}

void AnimatedVectorImageVisual::DoSetProperty(Property::Index index, const Property::Value& value)
{
  switch(index)
  {
    case Ui::ImageVisualPropertyIndex::LOOP_COUNT:
    {
      int32_t loopCount;
      if(value.Get(loopCount))
      {
        mAnimationData.loopCount = loopCount;
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_LOOP_COUNT;
      }
      break;
    }
    case Ui::ImageVisualPropertyIndex::PLAY_RANGE:
    {
      const Property::Array* array = value.GetArray();
      if(array)
      {
        mAnimationData.playRange = *array;
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_PLAY_RANGE;
      }
      else if(value.GetType() == Property::STRING)
      {
        std::string markerName;
        if(GetStdString(value, markerName))
        {
          Property::Array array;
          array.Add(ToPropertyValue(markerName));
          mAnimationData.playRange = std::move(array);
          mAnimationData.resendFlag |= VectorAnimationTask::RESEND_PLAY_RANGE;
        }
      }
      break;
    }
    case Ui::ImageVisualPropertyIndex::STOP_BEHAVIOR:
    {
      int32_t stopBehavior = static_cast<int32_t>(mAnimationData.stopBehavior);
      if(Scripting::GetEnumerationProperty(value, STOP_BEHAVIOR_TABLE, STOP_BEHAVIOR_TABLE_COUNT, stopBehavior))
      {
        mAnimationData.stopBehavior = static_cast<Ui::AnimatedImage::StopBehavior>(stopBehavior);
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_STOP_BEHAVIOR;
      }
      break;
    }
    case Ui::ImageVisualPropertyIndex::LOOPING_MODE:
    {
      int32_t loopingMode = static_cast<int32_t>(mAnimationData.loopingMode);
      if(Scripting::GetEnumerationProperty(value, LOOPING_MODE_TABLE, LOOPING_MODE_TABLE_COUNT, loopingMode))
      {
        mAnimationData.loopingMode = static_cast<Ui::LottieAnimation::LoopingMode>(loopingMode);
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_LOOPING_MODE;
      }
      break;
    }
    case Ui::ImageVisualPropertyIndex::REDRAW_IN_SCALING_DOWN:
    {
      bool redraw;
      if(value.Get(redraw))
      {
        mRedrawInScalingDown = redraw;
      }
      break;
    }
    case Ui::ImageVisualPropertyIndex::REDRAW_IN_SCALING_UP:
    {
      bool redraw;
      if(value.Get(redraw))
      {
        mRedrawInScalingUp = redraw;
      }
      break;
    }
    case Ui::ImageVisualPropertyIndex::SYNCHRONOUS_LOADING:
    {
      bool sync = false;
      if(value.Get(sync))
      {
        if(sync)
        {
          mImpl->mFlags |= Visual::Base::Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
        }
        else
        {
          mImpl->mFlags &= ~Visual::Base::Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
        }
      }
      break;
    }
    case Ui::ImageVisualPropertyIndex::DESIRED_WIDTH:
    {
      int32_t desiredWidth = 0;
      if(value.Get(desiredWidth))
      {
        mDesiredSize.SetWidth(desiredWidth);
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::DESIRED_HEIGHT:
    {
      int32_t desiredHeight = 0;
      if(value.Get(desiredHeight))
      {
        mDesiredSize.SetHeight(desiredHeight);
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::RELEASE_POLICY:
    {
      int32_t releasePolicy = static_cast<int32_t>(mReleasePolicy);
      if(DALI_LIKELY(Scripting::GetEnumerationProperty(value, RELEASE_POLICY_TABLE, RELEASE_POLICY_TABLE_COUNT,
                                                       releasePolicy)))
      {
        mReleasePolicy = static_cast<Ui::Image::ReleasePolicy>(releasePolicy);
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::ENABLE_FRAME_CACHE:
    {
      bool frameCacheEnabled = false;
      if(value.Get(frameCacheEnabled))
      {
        mFrameCacheEnabled = frameCacheEnabled;
        if(mVectorAnimationTask)
        {
          mVectorAnimationTask->KeepRasterizedBuffer(mFrameCacheEnabled);
        }
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::NOTIFY_AFTER_RASTERIZATION:
    {
      bool notifyAfterRasterization = false;
      if(value.Get(notifyAfterRasterization))
      {
        if(mNotifyAfterRasterization != notifyAfterRasterization)
        {
          mNotifyAfterRasterization = notifyAfterRasterization;

          mAnimationData.notifyAfterRasterization = mNotifyAfterRasterization;
          mAnimationData.resendFlag |= VectorAnimationTask::RESEND_NOTIFY_AFTER_RASTERIZATION;
        }
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

          mAnimationData.frameSpeedFactor = mFrameSpeedFactor;
          mAnimationData.resendFlag |= VectorAnimationTask::RESEND_FRAME_SPEED_FACTOR;
        }
      }
      break;
    }

    case Ui::ImageVisualPropertyIndex::RENDER_SCALE:
    {
      float renderScale = 1.0f;
      if(value.Get(renderScale))
      {
        mRenderScale = renderScale;
        SetVectorImageSize();
      }
      break;
    }
    case Ui::ImageVisualPropertyIndex::ENABLE_ASPECT_FIT:
    {
      bool aspectFitEnabled = true;
      if(value.Get(aspectFitEnabled))
      {
        if(mVectorAnimationTask)
        {
          mVectorAnimationTask->SetAspectFitEnabled(aspectFitEnabled);
        }
      }
      break;
    }
    case Ui::ImageVisualPropertyIndex::PIXEL_AREA:
    {
      value.Get(mPixelArea);
      if(mImpl->mRenderer)
      {
        if(mPixelAreaIndex != Property::INVALID_INDEX)
        {
          mImpl->mRenderer.SetProperty(mPixelAreaIndex, mPixelArea);
        }
        else
        {
          mPixelAreaIndex = mImpl->mRenderer.RegisterUniqueProperty(Ui::ImageVisualPropertyIndex::PIXEL_AREA,
                                                                    PIXEL_AREA_UNIFORM_NAME, mPixelArea);
        }
      }
      break;
    }
  }
}

Dali::Property AnimatedVectorImageVisual::OnGetPropertyObject(Dali::Property::Key key, bool changeProperties)
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

void AnimatedVectorImageVisual::OnInitialize(void)
{
  mVectorAnimationTask->ResourceReadySignal().Connect(this, &AnimatedVectorImageVisual::OnResourceReady);
  mVectorAnimationTask->SetAnimationFinishedCallback(
    MakeCallback(this, &AnimatedVectorImageVisual::OnAnimationFinished));

  EncodedImageBuffer encodedImageBuffer;

  if(mImageUrl.IsBufferResource())
  {
    // Increase reference count of External Resources :
    // EncodedImageBuffer.
    // Reference count will be decreased at destructor of the visual.
    TextureManager& textureManager = mFactoryCache.GetTextureManager();
    textureManager.UseExternalResource(mImageUrl);

    encodedImageBuffer = textureManager.GetEncodedImageBuffer(mImageUrl);
  }

  mVectorAnimationTask->KeepRasterizedBuffer(mFrameCacheEnabled);
  mVectorAnimationTask->RequestLoad(mImageUrl, encodedImageBuffer, IsSynchronousLoadingRequired());

  Shader shader = GenerateShader();

  Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);

  mImpl->mRenderer = DecoratedVisualRenderer::New(geometry, shader);

  TextureSet textureSet = TextureSet::New();
  mImpl->mRenderer.SetTextures(textureSet);

  // Register transform properties
  mImpl->SetTransformUniforms(mImpl->mRenderer, Dali::Ui::Integration::Direction::LEFT_TO_RIGHT);

  if(mPixelArea != FULL_TEXTURE_RECT)
  {
    mPixelAreaIndex = mImpl->mRenderer.RegisterUniqueProperty(Ui::ImageVisualPropertyIndex::PIXEL_AREA,
                                                              PIXEL_AREA_UNIFORM_NAME, mPixelArea);
  }

  mVectorAnimationTask->SetRenderer(mImpl->mRenderer);
}

void AnimatedVectorImageVisual::DoSetOnScene(Actor& actor)
{
  // Defer the rasterisation task until we get given a size (by Size Negotiation algorithm)

  // Hold the weak handle of the placement actor and delay the adding of renderer until the rasterization is finished.
  mPlacementActor = actor;

  if(mLoadFailed)
  {
    Vector2 imageSize = actor.GetProperty(Actor::Property::SIZE).Get<Vector2>();
    mFactoryCache.UpdateBrokenImageRenderer(mImpl->mRenderer, imageSize, false);
    actor.AddRenderer(mImpl->mRenderer);
    mRendererAdded = true;
    ResourceReady(Ui::Visual::ResourceStatus::FAILED);
  }
  else
  {
    // Add property notification for scaling & size
    mScaleNotification = actor.AddPropertyNotification(Actor::Property::WORLD_SCALE, StepCondition(0.1f, 1.0f));
    mScaleNotification.NotifySignal().Connect(this, &AnimatedVectorImageVisual::OnScaleNotification);

    mSizeNotification = actor.AddPropertyNotification(Actor::Property::SIZE, StepCondition(3.0f));
    mSizeNotification.NotifySignal().Connect(this, &AnimatedVectorImageVisual::OnSizeNotification);

    actor.EffectiveVisibilityChangedSignal().Connect(this,
                                                     &AnimatedVectorImageVisual::OnControlEffectiveVisibilityChanged);

    if(mImpl->mEventObserver)
    {
      // The visual needs it's size set before it can be rasterized hence request relayout once on stage
      mImpl->mEventObserver->RelayoutRequest(*this);
    }

    // Check whether we can re-use latest frame.
    if(mReleasePolicy != Ui::Image::ReleasePolicy::DETACHED && mRasterizeCompleted)
    {
      DALI_LOG_DEBUG_INFO("[%p] Reuse cached texture (url:%s)\n", this, mImageUrl.GetEllipsedUrl().c_str());

      // Call OnResourceReady synchronously.
      OnResourceReady(VectorAnimationTask::ResourceStatus::READY);
    }
    else
    {
      mAnimationData.resendFlag |= VectorAnimationTask::RESEND_NEED_RESOURCE_READY;
      TriggerVectorRasterization();
    }
  }

  DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "AnimatedVectorImageVisual::DoSetOnScene [%p]\n", this);
}

void AnimatedVectorImageVisual::DoSetOffScene(Actor& actor)
{
  StopAnimation();
  TriggerVectorRasterization();

  if(mImpl->mRenderer)
  {
    actor.RemoveRenderer(mImpl->mRenderer);
    mRendererAdded = false;
  }

  // Remove property notification
  actor.RemovePropertyNotification(mScaleNotification);
  actor.RemovePropertyNotification(mSizeNotification);

  actor.EffectiveVisibilityChangedSignal().Disconnect(this,
                                                      &AnimatedVectorImageVisual::OnControlEffectiveVisibilityChanged);

  mPlacementActor.Reset();

  if(mReleasePolicy == Ui::Image::ReleasePolicy::DETACHED)
  {
    mImpl->mResourceStatus = Ui::Visual::ResourceStatus::PREPARING;

    // Reset the visual size to zero so that when adding the actor back to stage the rasterization is forced
    mRasterizeCompleted   = false;
    mVisualSize           = Vector2::ZERO;
    mVisualScale          = Vector2::ONE;
    mAnimationData.width  = 0;
    mAnimationData.height = 0;

    // Remove textureset now.
    // TODO : Could we reset textureset here?
    // mImpl->mRenderer.RemoveTextures();
  }

  DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "AnimatedVectorImageVisual::DoSetOffScene [%p]\n", this);
}

void AnimatedVectorImageVisual::OnApplyFittingMode(const Vector2& controlSize, const Insets& padding, float effectiveScale)
{
  DoApplyFittingMode(controlSize, padding, effectiveScale, Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
}

void AnimatedVectorImageVisual::OnSetTransform()
{
  if(mImpl->mRenderer && mImpl->mTransformMapChanged)
  {
    mImpl->SetTransformUniforms(mImpl->mRenderer, Dali::Ui::Integration::Direction::LEFT_TO_RIGHT);
  }

  if(IsOnScene())
  {
    Vector2 visualSize = mImpl->GetTransformVisualSize(mImpl->mControlSize);

    if(visualSize != mVisualSize)
    {
      DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose,
                    "AnimatedVectorImageVisual::OnSetTransform: width = %f, height = %f [%p]\n", visualSize.width,
                    visualSize.height, this);

      mVisualSize = visualSize;

      SetVectorImageSize();

      if(mPlayState == Ui::AnimatedImage::PlayState::PLAYING &&
         mAnimationData.playState != Ui::AnimatedImage::PlayState::PLAYING)
      {
        mAnimationData.playState = Ui::AnimatedImage::PlayState::PLAYING;
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_PLAY_STATE;
      }

      TriggerVectorRasterization();
    }
  }
}

void AnimatedVectorImageVisual::UpdateShader()
{
  if(mImpl->mRenderer)
  {
    Shader shader = GenerateShader();
    mImpl->mRenderer.SetShader(shader);
  }
}

void AnimatedVectorImageVisual::OnDoAction(const Property::Index actionId, const Property::Value& attributes)
{
  // Check if action is valid for this visual type and perform action if possible
  switch(actionId)
  {
    case Dali::Ui::Integration::AnimatedVectorImageVisual::Action::PLAY:
    {
      if(IsOnScene() && mVisualSize != Vector2::ZERO)
      {
        // Always resend Playing state. If task is already playing, it will be ignored at Rasterize time.
        mAnimationData.playState = Ui::AnimatedImage::PlayState::PLAYING;
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_PLAY_STATE;
      }
      DALI_LOG_DEBUG_INFO("[%p] Play request (url:%s)\n", this, mImageUrl.GetEllipsedUrl().c_str());
      mPlayState = Ui::AnimatedImage::PlayState::PLAYING;
      break;
    }
    case Dali::Ui::Integration::AnimatedVectorImageVisual::Action::PAUSE:
    {
      if(mAnimationData.playState == Ui::AnimatedImage::PlayState::PLAYING)
      {
        mAnimationData.playState = Ui::AnimatedImage::PlayState::PAUSED;
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_PLAY_STATE;
      }
      DALI_LOG_DEBUG_INFO("[%p] Pause request (url:%s)\n", this, mImageUrl.GetEllipsedUrl().c_str());
      mPlayState = Ui::AnimatedImage::PlayState::PAUSED;
      break;
    }
    case Dali::Ui::Integration::AnimatedVectorImageVisual::Action::STOP:
    {
      if(mAnimationData.playState != Ui::AnimatedImage::PlayState::STOPPED)
      {
        mAnimationData.playState = Ui::AnimatedImage::PlayState::STOPPED;
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_PLAY_STATE;
      }
      DALI_LOG_DEBUG_INFO("[%p] Stop request (url:%s)\n", this, mImageUrl.GetEllipsedUrl().c_str());
      mPlayState = Ui::AnimatedImage::PlayState::STOPPED;
      break;
    }
    case Dali::Ui::Integration::AnimatedVectorImageVisual::Action::JUMP_TO:
    {
      int32_t frameNumber;
      if(attributes.Get(frameNumber))
      {
        mAnimationData.currentFrame = frameNumber;
        mAnimationData.resendFlag |= VectorAnimationTask::RESEND_CURRENT_FRAME;
        DALI_LOG_DEBUG_INFO("[%p] Jump to (%d) request (url:%s)\n", this, frameNumber,
                            mImageUrl.GetEllipsedUrl().c_str());
      }
      break;
    }
    case Dali::Ui::Integration::AnimatedVectorImageVisual::Action::FLUSH:
    {
      if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
      {
        DALI_LOG_DEBUG_INFO("[%p] Flush request (url:%s)\n", this, mImageUrl.GetEllipsedUrl().c_str());
        SendAnimationData();
      }
      break;
    }
  }

  TriggerVectorRasterization();
}

void AnimatedVectorImageVisual::OnDoActionExtension(const Property::Index actionId, const Dali::Any& attributes)
{
  switch(actionId)
  {
    case Dali::Ui::Integration::AnimatedVectorImageVisual::Action::SET_DYNAMIC_PROPERTY:
    {
      Dali::Ui::Integration::AnimatedVectorImageVisual::DynamicPropertyInfo info =
        AnyCast<Dali::Ui::Integration::AnimatedVectorImageVisual::DynamicPropertyInfo>(attributes);
      mAnimationData.dynamicProperties.push_back(info);
      mAnimationData.resendFlag |= VectorAnimationTask::RESEND_DYNAMIC_PROPERTY;
      break;
    }
  }

  TriggerVectorRasterization();
}

void AnimatedVectorImageVisual::OnResourceReady(VectorAnimationTask::ResourceStatus status)
{
  DALI_LOG_DEBUG_INFO("[%p] OnResourceReady (status:%d) (url:%s)\n", this, static_cast<int>(status),
                      mImageUrl.GetEllipsedUrl().c_str());

  AnimatedVectorImageVisualPtr self = this; // Keep reference until this API finished

  if(status == VectorAnimationTask::ResourceStatus::LOADED)
  {
    if(mImpl->mEventObserver)
    {
      mImpl->mEventObserver->RelayoutRequest(*this);
    }
  }
  else
  {
    mRasterizeCompleted = true;

    mLoadFailed = status == VectorAnimationTask::ResourceStatus::FAILED ? true : false;
    if(status == VectorAnimationTask::ResourceStatus::READY)
    {
      // Texture was ready. Change the shader if we need.
      bool useNativeImage = false;
      if(mImpl->mRenderer)
      {
        auto textureSet = mImpl->mRenderer.GetTextures();
        if(textureSet && textureSet.GetTextureCount() > 0)
        {
          auto texture = textureSet.GetTexture(0u);
          if(texture)
          {
            useNativeImage = DevelTexture::IsNative(texture);

#if defined(GPU_MEMORY_PROFILE_ENABLED)
            if(useNativeImage)
            {
              // Call Upload API, only for add informations of GPU memory usage.
              Dali::Integration::TextureUploadWithContent(texture, GetDummyRGBAPixelData(), ToDaliString(mImageUrl.GetUrl()), Dali::Integration::TextureContextTypeHint::NATIVE_LOTTIE_IMAGE, true);
            }
#endif

            if(mUseNativeImage != useNativeImage)
            {
              mUseNativeImage = useNativeImage;
              UpdateShader();
            }
          }
        }
      }
    }

    // If weak handle is holding a placement actor, it is the time to add the renderer to actor.
    Actor actor = mPlacementActor.GetHandle();
    if(actor && !mRendererAdded)
    {
      if(!mLoadFailed)
      {
        actor.AddRenderer(mImpl->mRenderer);
        ResourceReady(Ui::Visual::ResourceStatus::READY);
      }
      else
      {
        Vector2 imageSize = actor.GetProperty(Actor::Property::SIZE).Get<Vector2>();
        mFactoryCache.UpdateBrokenImageRenderer(mImpl->mRenderer, imageSize, false);
        actor.AddRenderer(mImpl->mRenderer);
        ResourceReady(Ui::Visual::ResourceStatus::FAILED);
      }

      mRendererAdded = true;
    }
  }

  DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "status = %d [%p]\n", status, this);
}

void AnimatedVectorImageVisual::OnAnimationFinished(uint32_t playStateId)
{
  DALI_LOG_DEBUG_INFO("[%p] OnAnimationFinished (id : %u vs %u) (url:%s)\n", this, mLastSentPlayStateId, playStateId,
                      mImageUrl.GetEllipsedUrl().c_str());

  // Only send event when animation is finished by the last Play/Pause/Stop request.
  if(mLastSentPlayStateId != playStateId)
  {
    return;
  }

  AnimatedVectorImageVisualPtr self = this; // Keep reference until this API finished

  DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose,
                "AnimatedVectorImageVisual::OnAnimationFinished: action state = %d [%p]\n", mPlayState, this);

  if(mPlayState != Ui::AnimatedImage::PlayState::STOPPED)
  {
    mPlayState = Ui::AnimatedImage::PlayState::STOPPED;

    mAnimationData.playState = Ui::AnimatedImage::PlayState::STOPPED;

    if(mImpl->mEventObserver)
    {
      mImpl->mEventObserver->NotifyVisualEvent(*this, Dali::Ui::Integration::AnimatedVectorImageVisual::Signal::ANIMATION_FINISHED);
    }
  }

  if(!mNotifyAfterRasterization && mImpl->mRenderer)
  {
    mImpl->mRenderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::IF_REQUIRED);
  }
}

void AnimatedVectorImageVisual::SendAnimationData()
{
  if(mAnimationData.resendFlag)
  {
    if(mAnimationData.resendFlag & VectorAnimationTask::RESEND_PLAY_STATE)
    {
      // Keep last sent playId. It will be used when we try to emit AnimationFinished signal.
      // The OnAnimationFinished signal what before Play/Pause/Stop action send could be come after action sent.
      // To ensure the OnAnimationFinished signal comes belong to what we sent, we need to keep last sent playId.
      mAnimationData.playStateId = ++mLastSentPlayStateId;
      DALI_LOG_DEBUG_INFO("[%p] Resend play state request (id:%u) (state:%d,%d) (url:%s)\n", this, mLastSentPlayStateId,
                          static_cast<int>(mPlayState), static_cast<int>(mAnimationData.playState),
                          mImageUrl.GetEllipsedUrl().c_str());
    }
    mVectorAnimationTask->SetAnimationData(mAnimationData);

    if(mAnimationData.resendFlag & VectorAnimationTask::RESEND_DYNAMIC_PROPERTY)
    {
      // Remove applied dynamic properties
      mAnimationData.dynamicProperties.clear();
    }

    if(mImpl->mRenderer && ((mAnimationData.resendFlag & VectorAnimationTask::RESEND_PLAY_STATE) ||
                            (mAnimationData.resendFlag & VectorAnimationTask::RESEND_NOTIFY_AFTER_RASTERIZATION)))
    {
      if(!mNotifyAfterRasterization && mPlayState == Ui::AnimatedImage::PlayState::PLAYING)
      {
        // Make rendering behaviour if we don't notify after rasterization, but animation playing.
        mImpl->mRenderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR,
                                     DevelRenderer::Rendering::CONTINUOUSLY);
      }
      else
      {
        // Otherwise, notify will be sended after rasterization. Make behaviour as required.
        mImpl->mRenderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR,
                                     DevelRenderer::Rendering::IF_REQUIRED);
      }
    }

    mAnimationData.resendFlag = 0;
  }
}

void AnimatedVectorImageVisual::SetVectorImageSize()
{
  uint32_t width, height;
  if(mDesiredSize.GetWidth() > 0 && mDesiredSize.GetHeight() > 0)
  {
    width  = mDesiredSize.GetWidth();
    height = mDesiredSize.GetHeight();
  }
  else
  {
    width  = static_cast<uint32_t>(std::roundf(mVisualSize.width * mVisualScale.width * std::fabs(mRenderScale)));
    height = static_cast<uint32_t>(std::roundf(mVisualSize.height * mVisualScale.height * std::fabs(mRenderScale)));
  }

  if(mAnimationData.width != width || mAnimationData.height != height)
  {
    mAnimationData.width  = width;
    mAnimationData.height = height;
    mAnimationData.resendFlag |= VectorAnimationTask::RESEND_SIZE;
  }
}

void AnimatedVectorImageVisual::StopAnimation()
{
  if(mAnimationData.playState != Ui::AnimatedImage::PlayState::STOPPED)
  {
    mAnimationData.playState = Ui::AnimatedImage::PlayState::STOPPED;
    mAnimationData.resendFlag |= VectorAnimationTask::RESEND_PLAY_STATE;

    mPlayState = Ui::AnimatedImage::PlayState::STOPPED;
  }
}

void AnimatedVectorImageVisual::TriggerVectorRasterization()
{
  if(!mEventCallback && Dali::Adaptor::IsAvailable())
  {
    mEventCallback               = MakeCallback(this, &AnimatedVectorImageVisual::OnProcessEvents);
    auto& vectorAnimationManager = mFactoryCache.GetVectorAnimationManager();
    vectorAnimationManager.RegisterEventCallback(mEventCallback);

    Dali::Adaptor::Get().RequestProcessEventsAndUpdate(); // Trigger event processing
  }
}

void AnimatedVectorImageVisual::OnScaleNotification(PropertyNotification source)
{
  Actor actor = mPlacementActor.GetHandle();
  if(actor)
  {
    Vector3 scale = actor.GetProperty<Vector3>(Actor::Property::WORLD_SCALE);

    if((!Dali::Equals(mVisualScale.width, scale.width) || !Dali::Equals(mVisualScale.height, scale.height)))
    {
      bool redrawInScalingDown = mRedrawInScalingDown && (scale.width <= 1.0f || scale.height <= 1.0f);
      bool redrawInScalingUp   = mRedrawInScalingUp && (scale.width >= 1.0f || scale.height >= 1.0f);

      if(redrawInScalingDown || redrawInScalingUp)
      {
        mVisualScale.width  = scale.width;
        mVisualScale.height = scale.height;

        DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose,
                      "AnimatedVectorImageVisual::OnScaleNotification: scale = %f, %f [%p]\n", mVisualScale.width,
                      mVisualScale.height, this);

        SetVectorImageSize();
        SendAnimationData();

        Dali::Adaptor::Get().RequestProcessEventsAndUpdate(); // Trigger event processing
      }
    }
  }
}

void AnimatedVectorImageVisual::OnSizeNotification(PropertyNotification source)
{
  Actor actor = mPlacementActor.GetHandle();
  if(actor)
  {
    Vector3 size    = actor.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
    Vector2 newSize = mImpl->GetTransformVisualSize(Vector2(size));
    if(Dali::EqualsZero(newSize.width) || Dali::EqualsZero(newSize.height))
    {
      newSize = Vector2(size);
    }

    if(!Dali::Equals(mVisualSize.width, newSize.width) || !Dali::Equals(mVisualSize.height, newSize.height))
    {
      mVisualSize.width  = newSize.width;
      mVisualSize.height = newSize.height;

      DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose,
                    "AnimatedVectorImageVisual::OnSizeNotification: size = %f, %f [%p]\n", mVisualSize.width,
                    mVisualSize.height, this);

      SetVectorImageSize();
      SendAnimationData();

      Dali::Adaptor::Get().RequestProcessEventsAndUpdate(); // Trigger event processing
    }
  }
}

void AnimatedVectorImageVisual::OnControlEffectiveVisibilityChanged(Actor actor, bool visible)
{
  if(!visible)
  {
    StopAnimation();
    TriggerVectorRasterization();

    DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose,
                  "AnimatedVectorImageVisual::OnControlEffectiveVisibilityChanged: invisibile. Pause animation [%p]\n",
                  this);
  }
}

void AnimatedVectorImageVisual::OnProcessEvents()
{
  SendAnimationData();

  mEventCallback = nullptr; // The callback will be deleted in the VectorAnimationManager
}

Shader AnimatedVectorImageVisual::GenerateShader() const
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
    shader = mImageVisualShaderFactory.GetShader(
      mFactoryCache, ImageVisualShaderFeature::FeatureBuilder()
                       .EnableRoundedCorner(IsRoundedCornerRequired(), IsSquircleCornerRequired())
                       .EnableBorderline(IsBorderlineRequired())
                       .SetTextureForFragmentShaderCheck(
                         mUseNativeImage ? mImpl->mRenderer.GetTextures().GetTexture(0) : Dali::Texture()));
  }
  return shader;
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
