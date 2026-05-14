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
#include "lottie-animation-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/property-array.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/devel-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/devel-api/visuals/animated-vector-image-visual-actions-devel.h>
#include <dali-ui-foundation/devel-api/visuals/animated-vector-image-visual-signals-devel.h>
#include <dali-ui-foundation/devel-api/visuals/visual-actions-devel.h>
#include <dali-ui-foundation/devel-api/visuals/visual-properties-devel.h>
#include <dali-ui-foundation/integration-api/property-registration-helper.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/public-api/align-enumerations.h>
#include <dali-ui-foundation/public-api/ui-color.h>
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
  LottieAnimationViewImplPtr impl = LottieAnimationViewImpl::New();
  Ui::View                   view(*impl);
  impl->Initialize();
  return view;
}
// clang-format off
#define LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION(text, valueType, enumIndex) \
  DALI_PROPERTY_REGISTRATION_EXTERNAL(Ui, LottieAnimationViewPropertyIndex, Ui::Integration, LottieAnimationViewImpl, text, valueType, enumIndex)

DALI_TYPE_REGISTRATION_BEGIN(LottieAnimationViewImpl, ViewImpl, Create)

LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("image",                    STRING,  IMAGE)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("loopCount",                INTEGER, LOOP_COUNT)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("loopingMode",              INTEGER, LOOPING_MODE)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("stopBehavior",             INTEGER, STOP_BEHAVIOR)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("frameSpeedFactor",         FLOAT,   FRAME_SPEED_FACTOR)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("imageColor",               VECTOR4, IMAGE_COLOR)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("desiredWidth",             INTEGER, DESIRED_WIDTH)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("desiredHeight",            INTEGER, DESIRED_HEIGHT)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("releasePolicy",            INTEGER, RELEASE_POLICY)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("synchronousLoading",       BOOLEAN, SYNCHRONOUS_LOADING)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("preMultipliedAlpha",       BOOLEAN, PRE_MULTIPLIED_ALPHA)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("redrawInScalingDown",      BOOLEAN, REDRAW_IN_SCALING_DOWN)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("redrawInScalingUp",        BOOLEAN, REDRAW_IN_SCALING_UP)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("enableFrameCache",         BOOLEAN, ENABLE_FRAME_CACHE)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("notifyAfterRasterization", BOOLEAN, NOTIFY_AFTER_RASTERIZATION)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("renderScale",              FLOAT,   RENDER_SCALE)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("enableAspectFit",          BOOLEAN, ENABLE_ASPECT_FIT)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("placeholderImage",         STRING,  PLACEHOLDER_IMAGE)

DALI_ANIMATABLE_PROPERTY_REGISTRATION(Ui::Integration, LottieAnimationViewImpl, "pixelArea", VECTOR4, PIXEL_AREA)

DALI_TYPE_REGISTRATION_END()
#undef LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION
// clang-format on
} // namespace

LottieAnimationViewImpl::LottieAnimationViewImpl()
: ViewImpl(),
  mVisual(),
  mUrl(),
  mMinFrameMarker(),
  mMaxFrameMarker(),
  mPlaceholderUrl(),
  mPixelArea(0.0f, 0.0f, 1.0f, 1.0f),
  mImageColor(Color::WHITE),
  mStopBehavior(Ui::AnimatedImage::StopBehavior::CURRENT_FRAME),
  mLoopingMode(Ui::LottieAnimation::LoopingMode::RESTART),
  mPlayRangeType(PlayRangeType::NONE),
  mReleasePolicy(Ui::Image::ReleasePolicy::DETACHED),
  mLoopCount(-1),
  mMinFrame(0),
  mMaxFrame(0),
  mDesiredWidth(0),
  mDesiredHeight(0),
  mDepthIndex(DepthIndex::CONTENT),
  mFrameSpeedFactor(1.0f),
  mRenderScale(1.0f),
  mRedrawInScalingDown(true),
  mRedrawInScalingUp(true),
  mEnableFrameCache(false),
  mNotifyAfterRasterization(false),
  mSynchronousLoading(false),
  mPreMultipliedAlpha(false),
  mEnableAspectFit(true),
  mVisualDirty(false),
  mAnimationFinishedSignal()
{
}

LottieAnimationViewImpl::~LottieAnimationViewImpl() = default;

LottieAnimationViewImplPtr LottieAnimationViewImpl::New()
{
  return new LottieAnimationViewImpl();
}

void LottieAnimationViewImpl::SetProperty(Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value)
{
  Ui::View view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    LottieAnimationViewImpl& impl = static_cast<LottieAnimationViewImpl&>(GetImpl(view));
    switch(index)
    {
      case LottieAnimationViewImpl::Property::IMAGE:
      {
        Dali::String url;
        if(value.Get(url))
        {
          impl.SetResourceUrl(url);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::LOOP_COUNT:
      {
        int count;
        if(value.Get(count))
        {
          impl.SetLoopCount(count);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::LOOPING_MODE:
      {
        int mode;
        if(value.Get(mode))
        {
          impl.SetLoopingMode(static_cast<Ui::LottieAnimation::LoopingMode>(mode));
        }
        break;
      }
      case LottieAnimationViewImpl::Property::STOP_BEHAVIOR:
      {
        int behavior;
        if(value.Get(behavior))
        {
          impl.SetStopBehavior(static_cast<Ui::AnimatedImage::StopBehavior>(behavior));
        }
        break;
      }
      case LottieAnimationViewImpl::Property::FRAME_SPEED_FACTOR:
      {
        float factor;
        if(value.Get(factor))
        {
          impl.SetFrameSpeedFactor(factor);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::IMAGE_COLOR:
      {
        Vector4 color;
        if(value.Get(color))
        {
          impl.SetImageColor(UiColor(color));
        }
        break;
      }
      case LottieAnimationViewImpl::Property::DESIRED_WIDTH:
      {
        int width;
        if(value.Get(width))
        {
          impl.SetDesiredWidth(width);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::DESIRED_HEIGHT:
      {
        int height;
        if(value.Get(height))
        {
          impl.SetDesiredHeight(height);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::RELEASE_POLICY:
      {
        int policy;
        if(value.Get(policy))
        {
          impl.SetReleasePolicy(static_cast<Ui::Image::ReleasePolicy>(policy));
        }
        break;
      }
      case LottieAnimationViewImpl::Property::SYNCHRONOUS_LOADING:
      {
        bool sync;
        if(value.Get(sync))
        {
          impl.SetSynchronousLoading(sync);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::PRE_MULTIPLIED_ALPHA:
      {
        bool preMultiplied;
        if(value.Get(preMultiplied))
        {
          impl.SetPreMultipliedAlpha(preMultiplied);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::REDRAW_IN_SCALING_DOWN:
      {
        bool redraw;
        if(value.Get(redraw))
        {
          impl.SetRedrawOnScaleDown(redraw);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::REDRAW_IN_SCALING_UP:
      {
        bool redraw;
        if(value.Get(redraw))
        {
          impl.SetRedrawOnScaleUp(redraw);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::ENABLE_FRAME_CACHE:
      {
        bool enable;
        if(value.Get(enable))
        {
          impl.SetEnableFrameCache(enable);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::NOTIFY_AFTER_RASTERIZATION:
      {
        bool notify;
        if(value.Get(notify))
        {
          impl.SetNotifyAfterRasterization(notify);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::RENDER_SCALE:
      {
        float scale;
        if(value.Get(scale))
        {
          impl.SetRenderScale(scale);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::ENABLE_ASPECT_FIT:
      {
        bool enableAspectFit;
        if(value.Get(enableAspectFit))
        {
          impl.SetEnableAspectFit(enableAspectFit);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::PLACEHOLDER_IMAGE:
      {
        Dali::String url;
        if(value.Get(url))
        {
          impl.SetPlaceholderUrl(url);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::PIXEL_AREA:
      {
        Dali::Vector4 pixelArea;
        if(value.Get(pixelArea))
        {
          impl.SetPixelArea(pixelArea);
        }
        break;
      }
    }
  }
}

Dali::Property::Value LottieAnimationViewImpl::GetProperty(Dali::BaseObject* object, Dali::Property::Index index)
{
  Dali::Property::Value value;
  Ui::View              view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    LottieAnimationViewImpl& impl = static_cast<LottieAnimationViewImpl&>(GetImpl(view));
    switch(index)
    {
      case LottieAnimationViewImpl::Property::IMAGE:
        value = impl.GetResourceUrl();
        break;
      case LottieAnimationViewImpl::Property::LOOP_COUNT:
        value = impl.GetLoopCount();
        break;
      case LottieAnimationViewImpl::Property::LOOPING_MODE:
        value = static_cast<int>(impl.GetLoopingMode());
        break;
      case LottieAnimationViewImpl::Property::STOP_BEHAVIOR:
        value = static_cast<int>(impl.GetStopBehavior());
        break;
      case LottieAnimationViewImpl::Property::FRAME_SPEED_FACTOR:
        value = impl.GetFrameSpeedFactor();
        break;
      case LottieAnimationViewImpl::Property::IMAGE_COLOR:
        value = impl.GetImageColor().GetRgba();
        break;
      case LottieAnimationViewImpl::Property::DESIRED_WIDTH:
        value = impl.GetDesiredWidth();
        break;
      case LottieAnimationViewImpl::Property::DESIRED_HEIGHT:
        value = impl.GetDesiredHeight();
        break;
      case LottieAnimationViewImpl::Property::RELEASE_POLICY:
        value = static_cast<int>(impl.GetReleasePolicy());
        break;
      case LottieAnimationViewImpl::Property::SYNCHRONOUS_LOADING:
        value = impl.IsSynchronousLoading();
        break;
      case LottieAnimationViewImpl::Property::PRE_MULTIPLIED_ALPHA:
        value = impl.IsPreMultipliedAlpha();
        break;
      case LottieAnimationViewImpl::Property::REDRAW_IN_SCALING_DOWN:
        value = impl.IsRedrawOnScaleDown();
        break;
      case LottieAnimationViewImpl::Property::REDRAW_IN_SCALING_UP:
        value = impl.IsRedrawOnScaleUp();
        break;
      case LottieAnimationViewImpl::Property::ENABLE_FRAME_CACHE:
        value = impl.IsFrameCacheEnabled();
        break;
      case LottieAnimationViewImpl::Property::NOTIFY_AFTER_RASTERIZATION:
        value = impl.IsNotifyAfterRasterizationEnabled();
        break;
      case LottieAnimationViewImpl::Property::RENDER_SCALE:
        value = impl.GetRenderScale();
        break;
      case LottieAnimationViewImpl::Property::ENABLE_ASPECT_FIT:
        value = impl.IsAspectFitEnabled();
        break;
      case LottieAnimationViewImpl::Property::PLACEHOLDER_IMAGE:
        value = impl.GetPlaceholderUrl();
        break;
      case LottieAnimationViewImpl::Property::PIXEL_AREA:
        value = impl.GetPixelArea();
        break;
    }
  }
  return value;
}

void LottieAnimationViewImpl::OnInitialize()
{
  ViewImpl::OnInitialize();
  Internal::ViewDataImpl::Get(*this).VisualEventSignal().Connect(this, &LottieAnimationViewImpl::OnVisualEvent);

  // Connect to View::ResourceReadySignal to handle placeholder removal
  Ui::View::DownCast(Self()).ResourceReadySignal().Connect(this, &LottieAnimationViewImpl::OnViewResourceReady);
}

MeasuredSize LottieAnimationViewImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  if(mVisualDirty)
  {
    mVisualDirty = false;
    UpdateVisual();
  }

  // widthConstraint/heightConstraint are visual sizes; convert to natural for image measurement.
  float s    = GetEffectiveScale();
  float natW = (widthConstraint >= 0.f && s > 0.f) ? widthConstraint / s : widthConstraint;
  float natH = (heightConstraint >= 0.f && s > 0.f) ? heightConstraint / s : heightConstraint;

  Vector2 naturalSize;
  if(mVisual)
  {
    mVisual.GetNaturalSize(naturalSize);
  }

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

  // Maintain aspect ratio when only one dimension is constrained.
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

MeasuredSize LottieAnimationViewImpl::OnArrange(const LayoutRect& bounds)
{
  MeasuredSize result = ViewImpl::OnArrange(bounds);
  ApplyLayout(Vector2(bounds.width, bounds.height));
  return result;
}

void LottieAnimationViewImpl::ApplyLayout(const Vector2& size)
{
  if(!mVisual)
  {
    return;
  }

  Ui::GetImplementation(mVisual).ApplyFittingMode(size, GetPadding());
}

void LottieAnimationViewImpl::SetResourceUrl(const Dali::String& url)
{
  if(mUrl != url)
  {
    mUrl = url;
    // Re-show placeholder while new animation loads
    UpdatePlaceholderVisual();
    mVisualDirty = true;
    InvalidateMeasure();
  }
}

Dali::String LottieAnimationViewImpl::GetResourceUrl() const
{
  return mUrl;
}

void LottieAnimationViewImpl::Play()
{
  // Flush any pending configuration before issuing the playback action.
  // This guarantees that SetResourceUrl / SetMinMaxFrame / etc. called before
  // Play() are reflected in the visual that actually starts playing.
  if(mVisualDirty)
  {
    mVisualDirty = false;
    UpdateVisual();
  }
  if(mVisual)
  {
    auto& viewData = Internal::ViewDataImpl::Get(*this);
    viewData.DoAction(LottieAnimationViewImpl::Property::IMAGE, Ui::DevelAnimatedVectorImageVisual::Action::PLAY, Dali::Property::Map());
  }
}

void LottieAnimationViewImpl::Pause()
{
  if(mVisualDirty)
  {
    mVisualDirty = false;
    UpdateVisual();
  }
  if(mVisual)
  {
    auto& viewData = Internal::ViewDataImpl::Get(*this);
    viewData.DoAction(LottieAnimationViewImpl::Property::IMAGE, Ui::DevelAnimatedVectorImageVisual::Action::PAUSE, Dali::Property::Map());
  }
}

void LottieAnimationViewImpl::Stop()
{
  if(mVisualDirty)
  {
    mVisualDirty = false;
    UpdateVisual();
  }
  if(mVisual)
  {
    auto& viewData = Internal::ViewDataImpl::Get(*this);
    viewData.DoAction(LottieAnimationViewImpl::Property::IMAGE, Ui::DevelAnimatedVectorImageVisual::Action::STOP, Dali::Property::Map());
  }
}

void LottieAnimationViewImpl::SetLoopCount(int count)
{
  if(mLoopCount != count)
  {
    mLoopCount   = count;
    mVisualDirty = true;
    InvalidateMeasure();
  }
}

int LottieAnimationViewImpl::GetLoopCount() const
{
  return mLoopCount;
}

void LottieAnimationViewImpl::JumpToFrame(int frame)
{
  if(mVisualDirty)
  {
    mVisualDirty = false;
    UpdateVisual();
  }
  if(mVisual)
  {
    auto& viewData = Internal::ViewDataImpl::Get(*this);
    viewData.DoAction(LottieAnimationViewImpl::Property::IMAGE, Ui::DevelAnimatedVectorImageVisual::Action::JUMP_TO, frame);
  }
}

void LottieAnimationViewImpl::SetMinMaxFrame(int minFrame, int maxFrame)
{
  if(mPlayRangeType == PlayRangeType::FRAME && mMinFrame == minFrame && mMaxFrame == maxFrame)
  {
    return;
  }
  mPlayRangeType = PlayRangeType::FRAME;
  mMinFrame      = minFrame;
  mMaxFrame      = maxFrame;
  mVisualDirty   = true;
  InvalidateMeasure();
}

void LottieAnimationViewImpl::SetMinMaxFrameByMarker(const Dali::String& minMarker, const Dali::String& maxMarker)
{
  if(mPlayRangeType == PlayRangeType::MARKER && mMinFrameMarker == minMarker && mMaxFrameMarker == maxMarker)
  {
    return;
  }
  mPlayRangeType  = PlayRangeType::MARKER;
  mMinFrameMarker = minMarker;
  mMaxFrameMarker = maxMarker;
  mVisualDirty    = true;
  InvalidateMeasure();
}

void LottieAnimationViewImpl::SetStopBehavior(Ui::AnimatedImage::StopBehavior behavior)
{
  if(mStopBehavior != behavior)
  {
    mStopBehavior = behavior;
    mVisualDirty  = true;
    InvalidateMeasure();
  }
}

Ui::AnimatedImage::StopBehavior LottieAnimationViewImpl::GetStopBehavior() const
{
  return mStopBehavior;
}

void LottieAnimationViewImpl::SetLoopingMode(Ui::LottieAnimation::LoopingMode mode)
{
  if(mLoopingMode != mode)
  {
    mLoopingMode = mode;
    mVisualDirty = true;
    InvalidateMeasure();
  }
}

Ui::LottieAnimation::LoopingMode LottieAnimationViewImpl::GetLoopingMode() const
{
  return mLoopingMode;
}

void LottieAnimationViewImpl::SetFrameSpeedFactor(float factor)
{
  if(mFrameSpeedFactor != factor)
  {
    mFrameSpeedFactor = factor;
    mVisualDirty      = true;
    InvalidateMeasure();
  }
}

float LottieAnimationViewImpl::GetFrameSpeedFactor() const
{
  return mFrameSpeedFactor;
}

Dali::Ui::AnimatedImage::PlayState LottieAnimationViewImpl::GetPlayState() const
{
  if(mVisual)
  {
    Dali::Property::Map map;
    mVisual.CreatePropertyMap(map);
    if(auto* value = map.Find(Ui::ImageVisualPropertyIndex::PLAY_STATE))
    {
      return static_cast<Dali::Ui::AnimatedImage::PlayState>(value->Get<int>());
    }
  }
  return Dali::Ui::AnimatedImage::PlayState::STOPPED;
}

int LottieAnimationViewImpl::GetCurrentFrame() const
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

int LottieAnimationViewImpl::GetTotalFrame() const
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

void LottieAnimationViewImpl::SetRedrawOnScaleDown(bool redraw)
{
  if(mRedrawInScalingDown != redraw)
  {
    mRedrawInScalingDown = redraw;
    mVisualDirty         = true;
    InvalidateMeasure();
  }
}

bool LottieAnimationViewImpl::IsRedrawOnScaleDown() const
{
  return mRedrawInScalingDown;
}

void LottieAnimationViewImpl::SetRedrawOnScaleUp(bool redraw)
{
  if(mRedrawInScalingUp != redraw)
  {
    mRedrawInScalingUp = redraw;
    mVisualDirty       = true;
    InvalidateMeasure();
  }
}

bool LottieAnimationViewImpl::IsRedrawOnScaleUp() const
{
  return mRedrawInScalingUp;
}

void LottieAnimationViewImpl::SetEnableFrameCache(bool enable)
{
  if(mEnableFrameCache != enable)
  {
    mEnableFrameCache = enable;
    mVisualDirty      = true;
    InvalidateMeasure();
  }
}

bool LottieAnimationViewImpl::IsFrameCacheEnabled() const
{
  return mEnableFrameCache;
}

void LottieAnimationViewImpl::SetNotifyAfterRasterization(bool notify)
{
  if(mNotifyAfterRasterization != notify)
  {
    mNotifyAfterRasterization = notify;
    mVisualDirty              = true;
    InvalidateMeasure();
  }
}

bool LottieAnimationViewImpl::IsNotifyAfterRasterizationEnabled() const
{
  return mNotifyAfterRasterization;
}

void LottieAnimationViewImpl::SetRenderScale(float scale)
{
  if(mRenderScale != scale)
  {
    mRenderScale = scale;
    mVisualDirty = true;
    InvalidateMeasure();
  }
}

float LottieAnimationViewImpl::GetRenderScale() const
{
  return mRenderScale;
}

void LottieAnimationViewImpl::SetEnableAspectFit(bool enableAspectFit)
{
  if(mEnableAspectFit != enableAspectFit)
  {
    mEnableAspectFit = enableAspectFit;
    mVisualDirty     = true;
    InvalidateMeasure();
  }
}

bool LottieAnimationViewImpl::IsAspectFitEnabled() const
{
  return mEnableAspectFit;
}

Dali::Property::Map LottieAnimationViewImpl::GetContentInfo()
{
  Dali::Property::Map result;
  if(mVisual)
  {
    Dali::Property::Map map;
    mVisual.CreatePropertyMap(map);
    if(auto* value = map.Find(Ui::ImageVisualPropertyIndex::CONTENT_INFO))
    {
      value->Get(result);
    }
  }
  return result;
}

Dali::Property::Map LottieAnimationViewImpl::GetMarkerInfo()
{
  Dali::Property::Map result;
  if(mVisual)
  {
    Dali::Property::Map map;
    mVisual.CreatePropertyMap(map);
    if(auto* value = map.Find(Ui::ImageVisualPropertyIndex::MARKER_INFO))
    {
      value->Get(result);
    }
  }
  return result;
}

void LottieAnimationViewImpl::SetDynamicProperty(const Ui::LottieAnimation::DynamicPropertyInfo& info)
{
  if(mVisualDirty)
  {
    mVisualDirty = false;
    UpdateVisual();
  }

  if(mVisual)
  {
    Ui::DevelAnimatedVectorImageVisual::DynamicPropertyInfo dynamicInfo;
    dynamicInfo.id       = info.id;
    dynamicInfo.keyPath  = info.keyPath.CStr();
    dynamicInfo.property = static_cast<int32_t>(info.property);
    dynamicInfo.callback = info.callback;
    auto& viewData       = Internal::ViewDataImpl::Get(*this);
    viewData.DoActionExtension(LottieAnimationViewImpl::Property::IMAGE,
                               Ui::DevelAnimatedVectorImageVisual::Action::SET_DYNAMIC_PROPERTY,
                               Dali::Any(dynamicInfo));
  }
  else
  {
    DALI_LOG_WARNING("LottieAnimationView: SetDynamicProperty ignored — no visual (set ResourceUrl first)\n");
  }
}

Dali::Signal<void(Dali::Ui::View)>& LottieAnimationViewImpl::AnimationFinishedSignal()
{
  return mAnimationFinishedSignal;
}

Ui::Visual::ResourceStatus LottieAnimationViewImpl::GetLoadingStatus() const
{
  return Internal::ViewDataImpl::Get(*this).GetVisualResourceStatus(LottieAnimationViewImpl::Property::IMAGE);
}

void LottieAnimationViewImpl::OnVisualEvent(View view, Dali::Property::Index visualIndex, Dali::Property::Index signalId)
{
  if(visualIndex == LottieAnimationViewImpl::Property::IMAGE &&
     signalId == static_cast<Dali::Property::Index>(Ui::DevelAnimatedVectorImageVisual::Signal::ANIMATION_FINISHED))
  {
    Ui::View handle(GetOwner());
    mAnimationFinishedSignal.Emit(handle);
  }
}

void LottieAnimationViewImpl::UpdateVisual()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);

  if(mVisual)
  {
    viewData.UnregisterVisual(LottieAnimationViewImpl::Property::IMAGE);
    mVisual.Reset();
  }

  if(mUrl.Empty())
  {
    DALI_LOG_ERROR("LottieAnimationView must be supplied with a valid URL.\n");
    return;
  }

  Dali::Property::Map map;
  map.Insert(Ui::VisualBasePropertyIndex::TYPE, static_cast<int>(Ui::DevelVisual::ANIMATED_VECTOR_IMAGE));
  map.Insert(Ui::ImageVisualPropertyIndex::URL, mUrl);

  map.Insert(Ui::ImageVisualPropertyIndex::LOOP_COUNT, mLoopCount);
  map.Insert(Ui::ImageVisualPropertyIndex::STOP_BEHAVIOR, static_cast<int>(mStopBehavior));
  map.Insert(Ui::ImageVisualPropertyIndex::LOOPING_MODE, static_cast<int>(mLoopingMode));
  map.Insert(Ui::ImageVisualPropertyIndex::FRAME_SPEED_FACTOR, mFrameSpeedFactor);

  if(mPlayRangeType == PlayRangeType::FRAME)
  {
    Dali::Property::Array range;
    range.PushBack(mMinFrame);
    range.PushBack(mMaxFrame);
    map.Insert(Ui::ImageVisualPropertyIndex::PLAY_RANGE, range);
  }
  else if(mPlayRangeType == PlayRangeType::MARKER)
  {
    Dali::Property::Array range;
    range.PushBack(mMinFrameMarker);
    if(!mMaxFrameMarker.Empty())
    {
      range.PushBack(mMaxFrameMarker);
    }
    map.Insert(Ui::ImageVisualPropertyIndex::PLAY_RANGE, range);
  }

  map.Insert(Ui::ImageVisualPropertyIndex::REDRAW_IN_SCALING_DOWN, mRedrawInScalingDown);
  map.Insert(Ui::ImageVisualPropertyIndex::REDRAW_IN_SCALING_UP, mRedrawInScalingUp);
  map.Insert(Ui::ImageVisualPropertyIndex::ENABLE_FRAME_CACHE, mEnableFrameCache);
  map.Insert(Ui::ImageVisualPropertyIndex::NOTIFY_AFTER_RASTERIZATION, mNotifyAfterRasterization);
  map.Insert(Ui::ImageVisualPropertyIndex::RENDER_SCALE, mRenderScale);
  map.Insert(Ui::ImageVisualPropertyIndex::ENABLE_ASPECT_FIT, mEnableAspectFit);
  if(mDesiredWidth > 0)
  {
    map.Insert(Ui::ImageVisualPropertyIndex::DESIRED_WIDTH, mDesiredWidth);
  }

  if(mDesiredHeight > 0)
  {
    map.Insert(Ui::ImageVisualPropertyIndex::DESIRED_HEIGHT, mDesiredHeight);
  }

  map.Insert(Ui::ImageVisualPropertyIndex::RELEASE_POLICY, static_cast<int>(mReleasePolicy));
  map.Insert(Ui::ImageVisualPropertyIndex::SYNCHRONOUS_LOADING, mSynchronousLoading);
  map.Insert(Ui::ImageVisualPropertyIndex::PIXEL_AREA, mPixelArea);
  map.Insert(Ui::VisualBasePropertyIndex::MIX_COLOR, mImageColor.GetRgba());

  auto visualFactory = Ui::VisualFactory::Get();
  if(visualFactory)
  {
    mVisual = visualFactory.CreateVisual(map);
    if(mVisual)
    {
      viewData.RegisterVisual(LottieAnimationViewImpl::Property::IMAGE, mVisual, mDepthIndex);
    }
  }
}

void LottieAnimationViewImpl::SetDesiredWidth(int width)
{
  if(mDesiredWidth != width)
  {
    mDesiredWidth = width;
    mVisualDirty  = true;
    InvalidateMeasure();
  }
}

int LottieAnimationViewImpl::GetDesiredWidth() const
{
  return mDesiredWidth;
}

void LottieAnimationViewImpl::SetDesiredHeight(int height)
{
  if(mDesiredHeight != height)
  {
    mDesiredHeight = height;
    mVisualDirty   = true;
    InvalidateMeasure();
  }
}

int LottieAnimationViewImpl::GetDesiredHeight() const
{
  return mDesiredHeight;
}

void LottieAnimationViewImpl::SetReleasePolicy(Ui::Image::ReleasePolicy releasePolicy)
{
  if(mReleasePolicy != releasePolicy)
  {
    mReleasePolicy = releasePolicy;
    mVisualDirty   = true;
    InvalidateMeasure();
  }
}

Ui::Image::ReleasePolicy LottieAnimationViewImpl::GetReleasePolicy() const
{
  return mReleasePolicy;
}

void LottieAnimationViewImpl::SetSynchronousLoading(bool synchronous)
{
  if(mSynchronousLoading != synchronous)
  {
    mSynchronousLoading = synchronous;
    mVisualDirty        = true;
    InvalidateMeasure();
  }
}

bool LottieAnimationViewImpl::IsSynchronousLoading() const
{
  return mSynchronousLoading;
}

void LottieAnimationViewImpl::SetPreMultipliedAlpha(bool preMultiplied)
{
  if(mPreMultipliedAlpha != preMultiplied)
  {
    mPreMultipliedAlpha = preMultiplied;
    mVisualDirty        = true;
    InvalidateMeasure();
  }
}

bool LottieAnimationViewImpl::IsPreMultipliedAlpha() const
{
  return mPreMultipliedAlpha;
}

void LottieAnimationViewImpl::SetImageColor(const UiColor& color)
{
  if(mImageColor.GetRgba() != color.GetRgba())
  {
    mImageColor = color;
    if(mVisual)
    {
      // Update MIX_COLOR directly on the existing visual without rebuilding it.
      Dali::Property::Map map;
      map.Insert(Ui::VisualBasePropertyIndex::MIX_COLOR, mImageColor.GetRgba());
      mVisual.DoAction(DevelVisual::Action::UPDATE_PROPERTY, map);
    }
    else
    {
      // Visual not yet created: defer to next OnMeasure pass
      mVisualDirty = true;
      InvalidateMeasure();
    }
  }
}

UiColor LottieAnimationViewImpl::GetImageColor() const
{
  return mImageColor;
}

void LottieAnimationViewImpl::SetDepthIndex(int depthIndex)
{
  if(mDepthIndex != depthIndex)
  {
    mDepthIndex = depthIndex;
    if(mVisual)
    {
      auto& viewData = Internal::ViewDataImpl::Get(*this);
      viewData.RegisterVisual(LottieAnimationViewImpl::Property::IMAGE, mVisual, mDepthIndex);
      UpdatePlaceholderVisual();
    }
  }
}

void LottieAnimationViewImpl::SetPlaceholderUrl(const Dali::String& url)
{
  if(mPlaceholderUrl != url)
  {
    mPlaceholderUrl = url;
    UpdatePlaceholderVisual();
  }
}

Dali::String LottieAnimationViewImpl::GetPlaceholderUrl() const
{
  return mPlaceholderUrl;
}

void LottieAnimationViewImpl::SetPixelArea(const Dali::Vector4& pixelArea)
{
  mPixelArea = pixelArea;
  if(mVisual)
  {
    Dali::Property::Map map;
    map.Insert(Ui::ImageVisualPropertyIndex::PIXEL_AREA, pixelArea);
    mVisual.DoAction(DevelVisual::Action::UPDATE_PROPERTY, map);
  }
}

Dali::Vector4 LottieAnimationViewImpl::GetPixelArea() const
{
  return mPixelArea;
}

void LottieAnimationViewImpl::UpdatePlaceholderVisual()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  viewData.UnregisterVisual(LottieAnimationViewImpl::Property::PLACEHOLDER_IMAGE);

  if(mPlaceholderUrl.Empty())
  {
    return;
  }

  // Don't show placeholder if main image is already loaded
  if(viewData.GetVisualResourceStatus(LottieAnimationViewImpl::Property::IMAGE) == Ui::Visual::ResourceStatus::READY)
  {
    return;
  }

  auto visualFactory = Ui::VisualFactory::Get();
  if(!visualFactory)
  {
    return;
  }

  Dali::Property::Map map;
  map.Insert(Ui::VisualBasePropertyIndex::TYPE, Visual::IMAGE);
  map.Insert(Ui::ImageVisualPropertyIndex::URL, mPlaceholderUrl);

  auto visual = visualFactory.CreateVisual(map);
  if(visual)
  {
    viewData.RegisterVisual(LottieAnimationViewImpl::Property::PLACEHOLDER_IMAGE, visual, mDepthIndex + 1);
  }
}

void LottieAnimationViewImpl::OnViewResourceReady(Ui::View view)
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(viewData.GetVisualResourceStatus(LottieAnimationViewImpl::Property::IMAGE) != Ui::Visual::ResourceStatus::READY)
  {
    return;
  }

  // Main image is ready: remove placeholder
  viewData.UnregisterVisual(LottieAnimationViewImpl::Property::PLACEHOLDER_IMAGE);

  // Request a re-layout now that the natural size is known, so aspect-ratio adjustment applies.
  InvalidateMeasure();
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
