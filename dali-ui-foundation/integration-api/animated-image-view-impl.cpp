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
#include "animated-image-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/object/property-array.h>

#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/extension-api/property-registration-helper.h>
#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/animated-image-visual-actions-integ.h>
#include <dali-ui-foundation/integration-api/visuals/animated-image-visual-signals-integ.h>
#include <dali-ui-foundation/integration-api/visuals/image-visual-actions-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-actions-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/public-api/types/align-enumerations.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/image/animated-image-view.h>
#include <dali-ui-foundation/public-api/visuals/image-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

namespace
{
BaseHandle Create()
{
  AnimatedImageViewImplPtr impl = AnimatedImageViewImpl::New();
  Ui::View                 view(*impl);
  impl->Initialize();
  return view;
}
// clang-format off
#define ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION(text, valueType, enumIndex) \
  DALI_PROPERTY_REGISTRATION_EXTERNAL(Ui, AnimatedImageViewPropertyIndex, Ui::Integration, AnimatedImageViewImpl, text, valueType, enumIndex)

DALI_TYPE_REGISTRATION_BEGIN_FULL(Ui::AnimatedImageView, Ui::Integration::AnimatedImageViewImpl, Ui::View, Create)

ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("image",                 STRING,  IMAGE)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("imageUrls",             ARRAY,   IMAGE_URLS)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("loopCount",             INTEGER, LOOP_COUNT)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("stopBehavior",          INTEGER, STOP_BEHAVIOR)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("frameSpeedFactor",      FLOAT,   FRAME_SPEED_FACTOR)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("batchSize",             INTEGER, BATCH_SIZE)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("cacheSize",             INTEGER, CACHE_SIZE)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("frameDelay",            INTEGER, FRAME_DELAY)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("imageColor",            VECTOR4, IMAGE_COLOR)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("preMultipliedAlpha",    BOOLEAN, PRE_MULTIPLIED_ALPHA)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("fittingMode",           INTEGER, FITTING_MODE)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("samplingMode",          INTEGER, SAMPLING_MODE)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("desiredWidth",          INTEGER, DESIRED_WIDTH)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("desiredHeight",         INTEGER, DESIRED_HEIGHT)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("loadPolicy",            INTEGER, LOAD_POLICY)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("releasePolicy",         INTEGER, RELEASE_POLICY)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("synchronousLoading",    BOOLEAN, SYNCHRONOUS_LOADING)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("imageLoadWithViewSize", BOOLEAN, IMAGE_LOAD_WITH_VIEW_SIZE)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("alphaMaskUrl",          STRING,  ALPHA_MASK_URL)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("cropToMask",            BOOLEAN, CROP_TO_MASK)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("maskingMode",           INTEGER, MASKING_MODE)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("placeholderImage",      STRING,  PLACEHOLDER_IMAGE)

DALI_ANIMATABLE_PROPERTY_REGISTRATION(Ui::Integration, AnimatedImageViewImpl, "pixelArea", VECTOR4, PIXEL_AREA)

DALI_TYPE_REGISTRATION_END()
#undef ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION
// clang-format on
} // namespace

AnimatedImageViewImpl::AnimatedImageViewImpl()
: ViewImpl(),
  mVisual(),
  mUrl(),
  mAlphaMaskUrl(),
  mPlaceholderUrl(),
  mUrls(),
  mPixelArea(0.f, 0.f, 1.f, 1.f),
  mImageColor(Color::WHITE),
  mStopBehavior(Ui::AnimatedImage::StopBehavior::CURRENT_FRAME),
  mFittingMode(Ui::Image::FittingMode::FILL),
  mSamplingMode(Ui::Image::SamplingMode::LINEAR),
  mMaskingMode(Ui::Image::MaskingType::MASKING_ON_LOADING),
  mLoadPolicy(Ui::Image::LoadPolicy::ATTACHED),
  mReleasePolicy(Ui::Image::ReleasePolicy::DETACHED),
  mLoopCount(-1),
  mBatchSize(1),
  mCacheSize(1),
  mFrameDelay(-1),
  mDesiredWidth(0),
  mDesiredHeight(0),
  mFrameSpeedFactor(1.0f),
  mPreMultipliedAlpha(false), ///< Default as false for AnimatedImageView.
  mImageLoadWithViewSize(false),
  mCropToMask(false),
  mSynchronousLoading(false),
  mVisualDirty(false),
  mAnimationFinishedSignal()
{
}

AnimatedImageViewImpl::~AnimatedImageViewImpl() = default;

Dali::String AnimatedImageViewImpl::GetResourceUrl() const
{
  return mUrl;
}

void AnimatedImageViewImpl::Reload()
{
  if(mVisualDirty)
  {
    mVisualDirty = false;
    UpdateVisual();
  }
  if(mVisual)
  {
    auto& viewData = Internal::ViewDataImpl::Get(*this);
    viewData.DoAction(AnimatedImageViewImpl::Property::IMAGE, Ui::Integration::ImageVisual::Action::RELOAD, Dali::Property::Map());
  }
}

AnimatedImageViewImplPtr AnimatedImageViewImpl::New()
{
  return new AnimatedImageViewImpl();
}

void AnimatedImageViewImpl::SetProperty(Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value)
{
  Ui::View view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    AnimatedImageViewImpl& impl = static_cast<AnimatedImageViewImpl&>(GetImpl(view));
    switch(index)
    {
      case AnimatedImageViewImpl::Property::IMAGE:
      {
        Dali::String url;
        if(value.Get(url))
        {
          impl.SetResourceUrl(url);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::LOOP_COUNT:
      {
        int count;
        if(value.Get(count))
        {
          impl.SetLoopCount(count);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::IMAGE_COLOR:
      {
        Vector4 color;
        if(value.Get(color))
        {
          impl.SetImageColor(UiColor(color));
        }
        break;
      }
      case AnimatedImageViewImpl::Property::STOP_BEHAVIOR:
      {
        int behavior;
        if(value.Get(behavior))
        {
          impl.SetStopBehavior(static_cast<Ui::AnimatedImage::StopBehavior>(behavior));
        }
        break;
      }
      case AnimatedImageViewImpl::Property::FRAME_SPEED_FACTOR:
      {
        float factor;
        if(value.Get(factor))
        {
          impl.SetFrameSpeedFactor(factor);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::BATCH_SIZE:
      {
        int size;
        if(value.Get(size))
        {
          impl.SetBatchSize(size);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::CACHE_SIZE:
      {
        int size;
        if(value.Get(size))
        {
          impl.SetCacheSize(size);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::FRAME_DELAY:
      {
        int delay;
        if(value.Get(delay))
        {
          impl.SetFrameDelay(delay);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::DESIRED_WIDTH:
      {
        int width;
        if(value.Get(width))
        {
          impl.SetDesiredWidth(width);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::DESIRED_HEIGHT:
      {
        int height;
        if(value.Get(height))
        {
          impl.SetDesiredHeight(height);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::LOAD_POLICY:
      {
        int policy;
        if(value.Get(policy))
        {
          impl.SetLoadPolicy(static_cast<Ui::Image::LoadPolicy>(policy));
        }
        break;
      }
      case AnimatedImageViewImpl::Property::RELEASE_POLICY:
      {
        int policy;
        if(value.Get(policy))
        {
          impl.SetReleasePolicy(static_cast<Ui::Image::ReleasePolicy>(policy));
        }
        break;
      }
      case AnimatedImageViewImpl::Property::SYNCHRONOUS_LOADING:
      {
        bool sync;
        if(value.Get(sync))
        {
          impl.SetSynchronousLoading(sync);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::PRE_MULTIPLIED_ALPHA:
      {
        bool preMultiplied;
        if(value.Get(preMultiplied))
        {
          impl.SetPreMultipliedAlpha(preMultiplied);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::FITTING_MODE:
      {
        int mode;
        if(value.Get(mode))
        {
          impl.SetFittingMode(static_cast<Ui::Image::FittingMode>(mode));
        }
        break;
      }
      case AnimatedImageViewImpl::Property::SAMPLING_MODE:
      {
        int mode;
        if(value.Get(mode))
        {
          impl.SetSamplingMode(static_cast<Ui::Image::SamplingMode>(mode));
        }
        break;
      }
      case AnimatedImageViewImpl::Property::IMAGE_LOAD_WITH_VIEW_SIZE:
      {
        bool enabled;
        if(value.Get(enabled))
        {
          impl.SetImageLoadWithViewSize(enabled);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::ALPHA_MASK_URL:
      {
        Dali::String url;
        if(value.Get(url))
        {
          impl.SetAlphaMaskUrl(url);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::CROP_TO_MASK:
      {
        bool crop;
        if(value.Get(crop))
        {
          impl.SetCropToMask(crop);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::MASKING_MODE:
      {
        int mode;
        if(value.Get(mode))
        {
          impl.SetMaskingMode(static_cast<Ui::Image::MaskingType>(mode));
        }
        break;
      }
      case AnimatedImageViewImpl::Property::PLACEHOLDER_IMAGE:
      {
        Dali::String url;
        if(value.Get(url))
        {
          impl.SetPlaceholderUrl(url);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::IMAGE_URLS:
      {
        Dali::Property::Array array;
        if(value.Get(array))
        {
          Dali::Vector<Dali::String> urls;
          for(auto i = 0u; i < array.Count(); ++i)
          {
            Dali::String url;
            if(array[i].Get(url))
            {
              urls.PushBack(url);
            }
          }
          impl.SetResourceUrls(urls);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::PIXEL_AREA:
      {
        Vector4 area;
        if(value.Get(area))
        {
          impl.SetPixelArea(area);
        }
        break;
      }
    }
  }
}

Dali::Property::Value AnimatedImageViewImpl::GetProperty(Dali::BaseObject* object, Dali::Property::Index index)
{
  Dali::Property::Value value;
  Ui::View              view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    AnimatedImageViewImpl& impl = static_cast<AnimatedImageViewImpl&>(GetImpl(view));
    switch(index)
    {
      case AnimatedImageViewImpl::Property::IMAGE:
        value = impl.GetResourceUrl();
        break;
      case AnimatedImageViewImpl::Property::LOOP_COUNT:
        value = impl.GetLoopCount();
        break;
      case AnimatedImageViewImpl::Property::IMAGE_COLOR:
        value = impl.GetImageColor().GetRgba();
        break;
      case AnimatedImageViewImpl::Property::STOP_BEHAVIOR:
        value = static_cast<int>(impl.GetStopBehavior());
        break;
      case AnimatedImageViewImpl::Property::FRAME_SPEED_FACTOR:
        value = impl.GetFrameSpeedFactor();
        break;
      case AnimatedImageViewImpl::Property::BATCH_SIZE:
        value = impl.GetBatchSize();
        break;
      case AnimatedImageViewImpl::Property::CACHE_SIZE:
        value = impl.GetCacheSize();
        break;
      case AnimatedImageViewImpl::Property::FRAME_DELAY:
        value = impl.GetFrameDelay();
        break;
      case AnimatedImageViewImpl::Property::DESIRED_WIDTH:
        value = impl.GetDesiredWidth();
        break;
      case AnimatedImageViewImpl::Property::DESIRED_HEIGHT:
        value = impl.GetDesiredHeight();
        break;
      case AnimatedImageViewImpl::Property::LOAD_POLICY:
        value = static_cast<int>(impl.GetLoadPolicy());
        break;
      case AnimatedImageViewImpl::Property::RELEASE_POLICY:
        value = static_cast<int>(impl.GetReleasePolicy());
        break;
      case AnimatedImageViewImpl::Property::SYNCHRONOUS_LOADING:
        value = impl.IsSynchronousLoading();
        break;
      case AnimatedImageViewImpl::Property::PRE_MULTIPLIED_ALPHA:
        value = impl.IsPreMultipliedAlpha();
        break;
      case AnimatedImageViewImpl::Property::FITTING_MODE:
        value = static_cast<int>(impl.GetFittingMode());
        break;
      case AnimatedImageViewImpl::Property::SAMPLING_MODE:
        value = static_cast<int>(impl.GetSamplingMode());
        break;
      case AnimatedImageViewImpl::Property::IMAGE_LOAD_WITH_VIEW_SIZE:
        value = impl.IsImageLoadWithViewSizeEnabled();
        break;
      case AnimatedImageViewImpl::Property::ALPHA_MASK_URL:
        value = impl.GetAlphaMaskUrl();
        break;
      case AnimatedImageViewImpl::Property::CROP_TO_MASK:
        value = impl.IsCropToMask();
        break;
      case AnimatedImageViewImpl::Property::MASKING_MODE:
        value = static_cast<int>(impl.GetMaskingMode());
        break;
      case AnimatedImageViewImpl::Property::PLACEHOLDER_IMAGE:
        value = impl.GetPlaceholderUrl();
        break;
      case AnimatedImageViewImpl::Property::IMAGE_URLS:
      {
        Dali::Property::Array array;
        const auto&           urls = impl.GetResourceUrls();
        for(auto i = 0u; i < urls.Size(); ++i)
        {
          array.PushBack(urls[i]);
        }
        value = array;
        break;
      }
      case AnimatedImageViewImpl::Property::PIXEL_AREA:
        value = impl.GetPixelArea();
        break;
    }
  }
  return value;
}

void AnimatedImageViewImpl::OnInitialize()
{
  ViewImpl::OnInitialize();
  Internal::ViewDataImpl::Get(*this).VisualEventSignal().Connect(this, &AnimatedImageViewImpl::OnVisualEvent);

  // Connect to View::ResourceReadySignal to handle placeholder removal
  Ui::View::DownCast(Self()).ResourceReadySignal().Connect(this, &AnimatedImageViewImpl::OnViewResourceReady);
}

MeasuredSize AnimatedImageViewImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  // widthConstraint/heightConstraint are visual sizes; convert to natural for image measurement.
  float s    = GetEffectiveScale();
  float natW = (widthConstraint >= 0.f && s > 0.f) ? widthConstraint / s : widthConstraint;
  float natH = (heightConstraint >= 0.f && s > 0.f) ? heightConstraint / s : heightConstraint;

  Vector2 naturalSize = GetNaturalSize().GetVectorXY();

  float w = naturalSize.width;
  float h = naturalSize.height;

  float layoutW = GetRequestedWidth();
  float layoutH = GetRequestedHeight();

  if(layoutW == MATCH_PARENT)
  {
    w = natW;
  }
  else if(layoutW > 0)
  {
    w = layoutW;
  }

  if(layoutH == MATCH_PARENT)
  {
    h = natH;
  }
  else if(layoutH > 0)
  {
    h = layoutH;
  }

  if(naturalSize.width > 0.0f && naturalSize.height > 0.0f)
  {
    const float aspectRatio = naturalSize.height / naturalSize.width;
    const bool  widthFixed  = (layoutW == MATCH_PARENT || layoutW > 0);
    const bool  heightFixed = (layoutH == MATCH_PARENT || layoutH > 0);
    if(widthFixed && !heightFixed)
    {
      h = w * aspectRatio;
    }
    else if(!widthFixed && heightFixed)
    {
      w = h / aspectRatio;
    }
  }

  return MeasuredSize(w * s, h * s);
}

LayoutRect AnimatedImageViewImpl::OnArrange(const LayoutRect& bounds)
{
  return ViewImpl::OnArrange(bounds);
}

Vector3 AnimatedImageViewImpl::GetNaturalSize() const
{
  AnimatedImageViewImpl& self = *const_cast<AnimatedImageViewImpl*>(this);
  if(self.mVisualDirty)
  {
    self.mVisualDirty = false;
    self.UpdateVisual();
  }

  Vector2 naturalSize;
  if(self.mVisual)
  {
    self.mVisual.GetNaturalSize(naturalSize);
  }
  return Vector3(naturalSize);
}

void AnimatedImageViewImpl::SetResourceUrl(const Dali::String& url)
{
  if(mUrl != url)
  {
    mUrl = url;
    mUrls.Clear();
    // Re-show placeholder while new image loads
    UpdatePlaceholderVisual();
    mVisualDirty = true;
    InvalidateMeasure();
  }
}

void AnimatedImageViewImpl::Play()
{
  // Flush any pending configuration before issuing the playback action.
  if(mVisualDirty)
  {
    mVisualDirty = false;
    UpdateVisual();
  }
  if(mVisual)
  {
    auto& viewData = Internal::ViewDataImpl::Get(*this);
    viewData.DoAction(AnimatedImageViewImpl::Property::IMAGE, Ui::Integration::AnimatedImageVisual::Action::PLAY, Dali::Property::Map());
  }
}

void AnimatedImageViewImpl::Pause()
{
  if(mVisualDirty)
  {
    mVisualDirty = false;
    UpdateVisual();
  }
  if(mVisual)
  {
    auto& viewData = Internal::ViewDataImpl::Get(*this);
    viewData.DoAction(AnimatedImageViewImpl::Property::IMAGE, Ui::Integration::AnimatedImageVisual::Action::PAUSE, Dali::Property::Map());
  }
}

void AnimatedImageViewImpl::Stop()
{
  if(mVisualDirty)
  {
    mVisualDirty = false;
    UpdateVisual();
  }
  if(mVisual)
  {
    auto& viewData = Internal::ViewDataImpl::Get(*this);
    viewData.DoAction(AnimatedImageViewImpl::Property::IMAGE, Ui::Integration::AnimatedImageVisual::Action::STOP, Dali::Property::Map());
  }
}

void AnimatedImageViewImpl::JumpToFrame(int frame)
{
  if(mVisualDirty)
  {
    mVisualDirty = false;
    UpdateVisual();
  }
  if(mVisual)
  {
    auto& viewData = Internal::ViewDataImpl::Get(*this);
    viewData.DoAction(AnimatedImageViewImpl::Property::IMAGE, Ui::Integration::AnimatedImageVisual::Action::JUMP_TO, frame);
  }
}

void AnimatedImageViewImpl::SetStopBehavior(Ui::AnimatedImage::StopBehavior behavior)
{
  if(mStopBehavior != behavior)
  {
    mStopBehavior = behavior;
    mVisualDirty  = true;
    InvalidateMeasure();
  }
}

Ui::AnimatedImage::StopBehavior AnimatedImageViewImpl::GetStopBehavior() const
{
  return mStopBehavior;
}

void AnimatedImageViewImpl::SetFrameSpeedFactor(float factor)
{
  if(mFrameSpeedFactor != factor)
  {
    mFrameSpeedFactor = factor;
    mVisualDirty      = true;
    InvalidateMeasure();
  }
}

float AnimatedImageViewImpl::GetFrameSpeedFactor() const
{
  return mFrameSpeedFactor;
}

void AnimatedImageViewImpl::SetBatchSize(int size)
{
  if(size < 1)
  {
    DALI_LOG_WARNING("AnimatedImageView: BatchSize must be >= 1 (got %d), clamped to 1\n", size);
    size = 1;
  }
  if(mBatchSize != size)
  {
    mBatchSize   = size;
    mVisualDirty = true;
    InvalidateMeasure();
  }
}

int AnimatedImageViewImpl::GetBatchSize() const
{
  return mBatchSize;
}

void AnimatedImageViewImpl::SetCacheSize(int size)
{
  if(size < 1)
  {
    DALI_LOG_WARNING("AnimatedImageView: CacheSize must be >= 1 (got %d), clamped to 1\n", size);
    size = 1;
  }
  if(mCacheSize != size)
  {
    mCacheSize   = size;
    mVisualDirty = true;
    InvalidateMeasure();
  }
}

int AnimatedImageViewImpl::GetCacheSize() const
{
  return mCacheSize;
}

void AnimatedImageViewImpl::SetFrameDelay(int milliseconds)
{
  if(mFrameDelay != milliseconds)
  {
    mFrameDelay  = milliseconds;
    mVisualDirty = true;
    InvalidateMeasure();
  }
}

int AnimatedImageViewImpl::GetFrameDelay() const
{
  return mFrameDelay;
}

Ui::AnimatedImage::PlayState AnimatedImageViewImpl::GetPlayState() const
{
  if(mVisual)
  {
    Dali::Property::Map map;
    mVisual.CreatePropertyMap(map);
    if(auto* value = map.Find(Ui::ImageVisualPropertyIndex::PLAY_STATE))
    {
      return static_cast<Ui::AnimatedImage::PlayState>(value->Get<int>());
    }
  }
  return Ui::AnimatedImage::PlayState::STOPPED;
}

int AnimatedImageViewImpl::GetCurrentFrame() const
{
  if(mVisual)
  {
    Dali::Property::Map map;
    mVisual.CreatePropertyMap(map);
    if(auto* value = map.Find(Ui::ImageVisualPropertyIndex::CURRENT_FRAME_NUMBER))
    {
      return value->Get<int>();
    }
  }
  return 0;
}

int AnimatedImageViewImpl::GetTotalFrame() const
{
  if(mVisual)
  {
    Dali::Property::Map map;
    mVisual.CreatePropertyMap(map);
    if(auto* value = map.Find(Ui::ImageVisualPropertyIndex::TOTAL_FRAME_NUMBER))
    {
      return value->Get<int>();
    }
  }
  return 0;
}

Dali::Signal<void(Dali::Ui::View)>& AnimatedImageViewImpl::AnimationFinishedSignal()
{
  return mAnimationFinishedSignal;
}

void AnimatedImageViewImpl::OnVisualEvent(View view, Dali::Property::Index visualIndex, Dali::Property::Index signalId)
{
  if(visualIndex == AnimatedImageViewImpl::Property::IMAGE &&
     signalId == Ui::Integration::AnimatedImageVisual::Signal::ANIMATION_FINISHED)
  {
    Ui::View handle(GetOwner());
    mAnimationFinishedSignal.Emit(handle);
  }
}

void AnimatedImageViewImpl::SetLoopCount(int count)
{
  if(mLoopCount != count)
  {
    mLoopCount   = count;
    mVisualDirty = true;
    InvalidateMeasure();
  }
}

int AnimatedImageViewImpl::GetLoopCount() const
{
  return mLoopCount;
}

void AnimatedImageViewImpl::SetImageColor(const UiColor& color)
{
  if(mImageColor.GetRgba() != color.GetRgba())
  {
    mImageColor = color;
    if(mVisual)
    {
      // Update MIX_COLOR directly on the existing visual without rebuilding it.
      Dali::Property::Map map;
      map.Insert(Ui::VisualBasePropertyIndex::MIX_COLOR, mImageColor.GetRgba());
      mVisual.DoAction(Dali::Ui::Integration::Visual::Action::UPDATE_PROPERTY, map);
    }
    else
    {
      mVisualDirty = true;
      InvalidateMeasure();
    }
  }
}

UiColor AnimatedImageViewImpl::GetImageColor() const
{
  return mImageColor;
}

void AnimatedImageViewImpl::SetPixelArea(const Vector4& pixelArea)
{
  if(mPixelArea != pixelArea)
  {
    mPixelArea = pixelArea;
    if(mVisual)
    {
      Dali::Property::Map map;
      map.Insert(Ui::ImageVisualPropertyIndex::PIXEL_AREA, mPixelArea);
      mVisual.DoAction(Dali::Ui::Integration::Visual::Action::UPDATE_PROPERTY, map);
    }
  }
}

Vector4 AnimatedImageViewImpl::GetPixelArea() const
{
  return mPixelArea;
}

void AnimatedImageViewImpl::SetDesiredWidth(int width)
{
  if(mDesiredWidth != width)
  {
    mDesiredWidth = width;
    mVisualDirty  = true;
    InvalidateMeasure();
  }
}

int AnimatedImageViewImpl::GetDesiredWidth() const
{
  return mDesiredWidth;
}

void AnimatedImageViewImpl::SetDesiredHeight(int height)
{
  if(mDesiredHeight != height)
  {
    mDesiredHeight = height;
    mVisualDirty   = true;
    InvalidateMeasure();
  }
}

int AnimatedImageViewImpl::GetDesiredHeight() const
{
  return mDesiredHeight;
}

void AnimatedImageViewImpl::SetLoadPolicy(Ui::Image::LoadPolicy loadPolicy)
{
  if(mLoadPolicy != loadPolicy)
  {
    mLoadPolicy = loadPolicy;
    if(mLoadPolicy == Ui::Image::LoadPolicy::IMMEDIATE && (!mUrl.Empty() || !mUrls.Empty()))
    {
      mVisualDirty = false;
      UpdateVisual();
    }
    else
    {
      mVisualDirty = true;
      InvalidateMeasure();
    }
  }
}

Ui::Image::LoadPolicy AnimatedImageViewImpl::GetLoadPolicy() const
{
  return mLoadPolicy;
}

void AnimatedImageViewImpl::SetReleasePolicy(Ui::Image::ReleasePolicy releasePolicy)
{
  if(mReleasePolicy != releasePolicy)
  {
    mReleasePolicy = releasePolicy;
    mVisualDirty   = true;
    InvalidateMeasure();
  }
}

Ui::Image::ReleasePolicy AnimatedImageViewImpl::GetReleasePolicy() const
{
  return mReleasePolicy;
}

void AnimatedImageViewImpl::SetSynchronousLoading(bool synchronous)
{
  if(mSynchronousLoading != synchronous)
  {
    mSynchronousLoading = synchronous;
    mVisualDirty        = true;
    InvalidateMeasure();
  }
}

bool AnimatedImageViewImpl::IsSynchronousLoading() const
{
  return mSynchronousLoading;
}

void AnimatedImageViewImpl::SetPreMultipliedAlpha(bool preMultiplied)
{
  if(mPreMultipliedAlpha != preMultiplied)
  {
    mPreMultipliedAlpha = preMultiplied;
    mVisualDirty        = true;
    InvalidateMeasure();
  }
}

bool AnimatedImageViewImpl::IsPreMultipliedAlpha() const
{
  return mPreMultipliedAlpha;
}

void AnimatedImageViewImpl::SetFittingMode(Ui::Image::FittingMode fittingMode)
{
  if(mFittingMode != fittingMode)
  {
    mFittingMode = fittingMode;
    if(mVisual)
    {
      Ui::GetImplementation(mVisual).SetFittingMode(fittingMode);

      // FittingMode only changes the visual transform, not the visual itself.
      // Apply immediately if the actor size is already available; otherwise
      // defer to the next OnArrange via InvalidateMeasure.
      Actor self = Self();
      float w    = self.GetProperty<float>(Actor::Property::SIZE_WIDTH);
      float h    = self.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
      if(w > 0.0f && h > 0.0f)
      {
        Ui::GetImplementation(mVisual).ApplyFittingMode(Vector2(w, h), GetPadding(), GetEffectiveScale());
      }
      else
      {
        InvalidateMeasure();
      }
    }
    else
    {
      InvalidateMeasure();
    }
  }
}

Ui::Image::FittingMode AnimatedImageViewImpl::GetFittingMode() const
{
  return mFittingMode;
}

void AnimatedImageViewImpl::SetSamplingMode(Ui::Image::SamplingMode samplingMode)
{
  if(mSamplingMode != samplingMode)
  {
    mSamplingMode = samplingMode;
    mVisualDirty  = true;
    InvalidateMeasure();
  }
}

Ui::Image::SamplingMode AnimatedImageViewImpl::GetSamplingMode() const
{
  return mSamplingMode;
}

void AnimatedImageViewImpl::SetImageLoadWithViewSize(bool enabled)
{
  if(mImageLoadWithViewSize != enabled)
  {
    mImageLoadWithViewSize = enabled;
    mVisualDirty           = true;
    InvalidateMeasure();
  }
}

bool AnimatedImageViewImpl::IsImageLoadWithViewSizeEnabled() const
{
  return mImageLoadWithViewSize;
}

void AnimatedImageViewImpl::SetAlphaMaskUrl(const Dali::String& maskUrl)
{
  if(mAlphaMaskUrl != maskUrl)
  {
    mAlphaMaskUrl = maskUrl;
    mVisualDirty  = true;
    InvalidateMeasure();
  }
}

Dali::String AnimatedImageViewImpl::GetAlphaMaskUrl() const
{
  return mAlphaMaskUrl;
}

void AnimatedImageViewImpl::SetCropToMask(bool cropToMask)
{
  if(mCropToMask != cropToMask)
  {
    mCropToMask  = cropToMask;
    mVisualDirty = true;
    InvalidateMeasure();
  }
}

bool AnimatedImageViewImpl::IsCropToMask() const
{
  return mCropToMask;
}

void AnimatedImageViewImpl::SetMaskingMode(Ui::Image::MaskingType maskingMode)
{
  if(mMaskingMode != maskingMode)
  {
    mMaskingMode = maskingMode;
    mVisualDirty = true;
    InvalidateMeasure();
  }
}

Ui::Image::MaskingType AnimatedImageViewImpl::GetMaskingMode() const
{
  return mMaskingMode;
}

void AnimatedImageViewImpl::SetPlaceholderUrl(const Dali::String& url)
{
  if(mPlaceholderUrl != url)
  {
    mPlaceholderUrl = url;
    UpdatePlaceholderVisual();
  }
}

Dali::String AnimatedImageViewImpl::GetPlaceholderUrl() const
{
  return mPlaceholderUrl;
}

void AnimatedImageViewImpl::SetResourceUrls(const Dali::Vector<Dali::String>& urls)
{
  mUrls = urls;
  mUrl  = Dali::String();
  // Re-show placeholder while new images load
  UpdatePlaceholderVisual();
  mVisualDirty = true;
  InvalidateMeasure();
}

const Dali::Vector<Dali::String>& AnimatedImageViewImpl::GetResourceUrls() const
{
  return mUrls;
}

Ui::Visual::ResourceStatus AnimatedImageViewImpl::GetLoadingStatus() const
{
  return Internal::ViewDataImpl::Get(*this).GetVisualResourceStatus(AnimatedImageViewImpl::Property::IMAGE);
}

void AnimatedImageViewImpl::UpdateVisual()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(mVisual)
  {
    viewData.UnregisterVisual(AnimatedImageViewImpl::Property::IMAGE);
    mVisual.Reset();
  }

  const bool hasUrlArray = !mUrls.Empty();
  if(!hasUrlArray && mUrl.Empty())
  {
    DALI_LOG_ERROR("AnimatedImageView must be supplied with either a valid URL or an array of URLs.\n");
    return;
  }

  Dali::Property::Map map;
  map.Insert(Ui::VisualBasePropertyIndex::TYPE, Ui::Integration::InternalVisualType::ANIMATED_IMAGE);

  if(hasUrlArray)
  {
    Dali::Property::Array urlArray;
    for(auto i = 0u; i < mUrls.Size(); ++i)
    {
      urlArray.PushBack(mUrls[i]);
    }
    map.Insert(Ui::ImageVisualPropertyIndex::URL, urlArray);
  }
  else
  {
    map.Insert(Ui::ImageVisualPropertyIndex::URL, mUrl);
  }

  map.Insert(Ui::ImageVisualPropertyIndex::LOOP_COUNT, mLoopCount);

  map.Insert(Ui::ImageVisualPropertyIndex::STOP_BEHAVIOR, static_cast<int>(mStopBehavior));
  map.Insert(Ui::ImageVisualPropertyIndex::FRAME_SPEED_FACTOR, mFrameSpeedFactor);

  map.Insert(Ui::ImageVisualPropertyIndex::BATCH_SIZE, mBatchSize);
  map.Insert(Ui::ImageVisualPropertyIndex::CACHE_SIZE, mCacheSize);

  if(mFrameDelay >= 0)
  {
    map.Insert(Ui::ImageVisualPropertyIndex::FRAME_DELAY, mFrameDelay);
  }

  map.Insert(Ui::VisualBasePropertyIndex::MIX_COLOR, mImageColor.GetRgba());
  map.Insert(Ui::ImageVisualPropertyIndex::PRE_MULTIPLIED_ALPHA, mPreMultipliedAlpha);

  if(mDesiredWidth > 0)
  {
    map.Insert(Ui::ImageVisualPropertyIndex::DESIRED_WIDTH, mDesiredWidth);
  }

  if(mDesiredHeight > 0)
  {
    map.Insert(Ui::ImageVisualPropertyIndex::DESIRED_HEIGHT, mDesiredHeight);
  }

  map.Insert(Ui::ImageVisualPropertyIndex::LOAD_POLICY, static_cast<int>(mLoadPolicy));
  map.Insert(Ui::ImageVisualPropertyIndex::RELEASE_POLICY, static_cast<int>(mReleasePolicy));
  map.Insert(Ui::ImageVisualPropertyIndex::SYNCHRONOUS_LOADING, mSynchronousLoading);
  map.Insert(Ui::ImageVisualPropertyIndex::SAMPLING_MODE, static_cast<int>(mSamplingMode));
  map.Insert(Ui::ImageVisualPropertyIndex::SYNCHRONOUS_SIZING, mImageLoadWithViewSize);
  map.Insert(Ui::ImageVisualPropertyIndex::FITTING_MODE, static_cast<int>(mFittingMode));

  if(!mAlphaMaskUrl.Empty())
  {
    map.Insert(Ui::ImageVisualPropertyIndex::ALPHA_MASK_URL, mAlphaMaskUrl);
    map.Insert(Ui::ImageVisualPropertyIndex::CROP_TO_MASK, mCropToMask);
    map.Insert(Ui::ImageVisualPropertyIndex::MASKING_TYPE, static_cast<int>(mMaskingMode));
  }

  static const Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);
  if(mPixelArea != FULL_TEXTURE_RECT)
  {
    map.Insert(Ui::ImageVisualPropertyIndex::PIXEL_AREA, mPixelArea);
  }

  auto visualFactory = Ui::Integration::VisualFactory::Get();
  if(visualFactory)
  {
    mVisual = visualFactory.CreateVisual(map);
    if(mVisual)
    {
      viewData.RegisterVisual(AnimatedImageViewImpl::Property::IMAGE, mVisual, Dali::Ui::Integration::DepthIndex::CONTENT);
      viewData.EnableCornerPropertiesOverridden(mVisual, true);
    }
  }
}

void AnimatedImageViewImpl::UpdatePlaceholderVisual()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  viewData.UnregisterVisual(AnimatedImageViewImpl::Property::PLACEHOLDER_IMAGE);

  if(mPlaceholderUrl.Empty())
  {
    return;
  }

  // Don't show placeholder if main image is already loaded
  if(viewData.GetVisualResourceStatus(AnimatedImageViewImpl::Property::IMAGE) == Ui::Visual::ResourceStatus::READY)
  {
    return;
  }

  auto visualFactory = Ui::Integration::VisualFactory::Get();
  if(!visualFactory)
  {
    return;
  }

  Dali::Property::Map map;
  map.Insert(Ui::VisualBasePropertyIndex::TYPE, Ui::Integration::InternalVisualType::IMAGE);
  map.Insert(Ui::ImageVisualPropertyIndex::URL, mPlaceholderUrl);

  auto visual = visualFactory.CreateVisual(map);
  if(visual)
  {
    viewData.RegisterVisual(AnimatedImageViewImpl::Property::PLACEHOLDER_IMAGE, visual, Dali::Ui::Integration::DepthIndex::CONTENT + 1);
    viewData.EnableCornerPropertiesOverridden(visual, true);
  }
}

void AnimatedImageViewImpl::OnViewResourceReady(Ui::View view)
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(viewData.GetVisualResourceStatus(AnimatedImageViewImpl::Property::IMAGE) != Ui::Visual::ResourceStatus::READY)
  {
    return;
  }

  // Main image is ready: remove placeholder
  viewData.UnregisterVisual(AnimatedImageViewImpl::Property::PLACEHOLDER_IMAGE);

  // Request a re-layout now that the natural size is known
  InvalidateMeasure();
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
