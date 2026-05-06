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
#include <dali-ui-foundation/public-api/visuals/lottie-animation-visual.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/property-array.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visuals/animated-vector-image-visual-actions-devel.h>
#include <dali-ui-foundation/devel-api/visuals/visual-base-impl.h>

namespace Dali
{
namespace Ui
{
LottieAnimationVisual LottieAnimationVisual::New()
{
  Internal::VisualBaseImplPtr internal = Internal::VisualBaseImpl::New(Dali::Ui::Visual::ANIMATED_VECTOR_IMAGE);
  return LottieAnimationVisual(internal.Get());
}

// =============================================================================
// Properties
// =============================================================================

Dali::String LottieAnimationVisual::GetResourceUrl() const
{
  return VisualBase::GetProperty<Dali::String>(LottieAnimationVisual::Property::URL);
}

LottieAnimationVisual& LottieAnimationVisual::SetResourceUrl(const Dali::String& resourceUrl)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::URL, resourceUrl);
  return *this;
}

bool LottieAnimationVisual::IsSynchronousLoading() const
{
  return VisualBase::GetProperty<bool>(LottieAnimationVisual::Property::SYNCHRONOUS_LOADING);
}

LottieAnimationVisual& LottieAnimationVisual::SetSynchronousLoading(bool synchronous)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::SYNCHRONOUS_LOADING, synchronous);
  return *this;
}

int LottieAnimationVisual::GetDesiredWidth() const
{
  return VisualBase::GetProperty<int>(LottieAnimationVisual::Property::DESIRED_WIDTH);
}

LottieAnimationVisual& LottieAnimationVisual::SetDesiredWidth(int desiredWidth)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::DESIRED_WIDTH, desiredWidth);
  return *this;
}

int LottieAnimationVisual::GetDesiredHeight() const
{
  return VisualBase::GetProperty<int>(LottieAnimationVisual::Property::DESIRED_HEIGHT);
}

LottieAnimationVisual& LottieAnimationVisual::SetDesiredHeight(int desiredHeight)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::DESIRED_HEIGHT, desiredHeight);
  return *this;
}

Image::SamplingMode LottieAnimationVisual::GetSamplingMode() const
{
  return VisualBase::GetProperty<Image::SamplingMode>(LottieAnimationVisual::Property::SAMPLING_MODE);
}

LottieAnimationVisual& LottieAnimationVisual::SetSamplingMode(Image::SamplingMode samplingMode)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::SAMPLING_MODE, samplingMode);
  return *this;
}

Dali::Vector4 LottieAnimationVisual::GetPixelArea() const
{
  return VisualBase::GetProperty<Dali::Vector4>(LottieAnimationVisual::Property::PIXEL_AREA);
}

LottieAnimationVisual& LottieAnimationVisual::SetPixelArea(const Dali::Vector4& pixelArea)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::PIXEL_AREA, pixelArea);
  return *this;
}

Dali::WrapMode::Type LottieAnimationVisual::GetWrapModeU() const
{
  return VisualBase::GetProperty<Dali::WrapMode::Type>(LottieAnimationVisual::Property::WRAP_MODE_U);
}

LottieAnimationVisual& LottieAnimationVisual::SetWrapModeU(Dali::WrapMode::Type wrapModeU)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::WRAP_MODE_U, wrapModeU);
  return *this;
}

Dali::WrapMode::Type LottieAnimationVisual::GetWrapModeV() const
{
  return VisualBase::GetProperty<Dali::WrapMode::Type>(LottieAnimationVisual::Property::WRAP_MODE_V);
}

LottieAnimationVisual& LottieAnimationVisual::SetWrapModeV(Dali::WrapMode::Type wrapModeV)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::WRAP_MODE_V, wrapModeV);
  return *this;
}

bool LottieAnimationVisual::IsBrokenImageEnabled() const
{
  return VisualBase::GetProperty<bool>(LottieAnimationVisual::Property::ENABLE_BROKEN_IMAGE);
}

LottieAnimationVisual& LottieAnimationVisual::SetEnableBrokenImage(bool enableBrokenImage)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::ENABLE_BROKEN_IMAGE, enableBrokenImage);
  return *this;
}

Image::LoadPolicy LottieAnimationVisual::GetLoadPolicy() const
{
  return VisualBase::GetProperty<Image::LoadPolicy>(LottieAnimationVisual::Property::LOAD_POLICY);
}

LottieAnimationVisual& LottieAnimationVisual::SetLoadPolicy(Image::LoadPolicy loadPolicy)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::LOAD_POLICY, loadPolicy);
  return *this;
}

Image::ReleasePolicy LottieAnimationVisual::GetReleasePolicy() const
{
  return VisualBase::GetProperty<Image::ReleasePolicy>(LottieAnimationVisual::Property::RELEASE_POLICY);
}

LottieAnimationVisual& LottieAnimationVisual::SetReleasePolicy(Image::ReleasePolicy releasePolicy)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::RELEASE_POLICY, releasePolicy);
  return *this;
}

Image::FittingMode LottieAnimationVisual::GetFittingMode() const
{
  return VisualBase::GetProperty<Image::FittingMode>(LottieAnimationVisual::Property::FITTING_MODE);
}

LottieAnimationVisual& LottieAnimationVisual::SetFittingMode(Image::FittingMode fittingMode)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::FITTING_MODE, fittingMode);
  return *this;
}

bool LottieAnimationVisual::IsOrientationCorrection() const
{
  return VisualBase::GetProperty<bool>(LottieAnimationVisual::Property::ORIENTATION_CORRECTION);
}

LottieAnimationVisual& LottieAnimationVisual::SetOrientationCorrection(bool orientationCorrection)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::ORIENTATION_CORRECTION, orientationCorrection);
  return *this;
}

bool LottieAnimationVisual::IsSynchronousSizing() const
{
  return VisualBase::GetProperty<bool>(LottieAnimationVisual::Property::SYNCHRONOUS_SIZING);
}

LottieAnimationVisual& LottieAnimationVisual::SetSynchronousSizing(bool synchronousSizing)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::SYNCHRONOUS_SIZING, synchronousSizing);
  return *this;
}

int LottieAnimationVisual::GetLoopCount() const
{
  return VisualBase::GetProperty<int>(LottieAnimationVisual::Property::LOOP_COUNT);
}

LottieAnimationVisual& LottieAnimationVisual::SetLoopCount(int loopCount)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::LOOP_COUNT, loopCount);
  return *this;
}

Dali::Property::Array LottieAnimationVisual::GetPlayRange() const
{
  Dali::Property::Value  value    = VisualBase::GetProperty(LottieAnimationVisual::Property::PLAY_RANGE);
  Dali::Property::Array* arrayPtr = value.GetArray();
  if(arrayPtr)
  {
    return *arrayPtr;
  }
  return Dali::Property::Array();
}

LottieAnimationVisual& LottieAnimationVisual::SetPlayRange(const Dali::Property::Array& playRange)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::PLAY_RANGE, playRange);
  return *this;
}

AnimatedImage::StopBehavior LottieAnimationVisual::GetStopBehavior() const
{
  return VisualBase::GetProperty<AnimatedImage::StopBehavior>(LottieAnimationVisual::Property::STOP_BEHAVIOR);
}

LottieAnimationVisual& LottieAnimationVisual::SetStopBehavior(AnimatedImage::StopBehavior stopBehavior)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::STOP_BEHAVIOR, stopBehavior);
  return *this;
}

float LottieAnimationVisual::GetFrameSpeedFactor() const
{
  return VisualBase::GetProperty<float>(LottieAnimationVisual::Property::FRAME_SPEED_FACTOR);
}

LottieAnimationVisual& LottieAnimationVisual::SetFrameSpeedFactor(float frameSpeedFactor)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::FRAME_SPEED_FACTOR, frameSpeedFactor);
  return *this;
}

LottieAnimation::LoopingMode LottieAnimationVisual::GetLoopingMode() const
{
  return VisualBase::GetProperty<LottieAnimation::LoopingMode>(LottieAnimationVisual::Property::LOOPING_MODE);
}

LottieAnimationVisual& LottieAnimationVisual::SetLoopingMode(LottieAnimation::LoopingMode loopingMode)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::LOOPING_MODE, loopingMode);
  return *this;
}

bool LottieAnimationVisual::IsRedrawInScalingDown() const
{
  return VisualBase::GetProperty<bool>(LottieAnimationVisual::Property::REDRAW_IN_SCALING_DOWN);
}

LottieAnimationVisual& LottieAnimationVisual::SetRedrawInScalingDown(bool redrawInScalingDown)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::REDRAW_IN_SCALING_DOWN, redrawInScalingDown);
  return *this;
}

bool LottieAnimationVisual::IsRedrawInScalingUp() const
{
  return VisualBase::GetProperty<bool>(LottieAnimationVisual::Property::REDRAW_IN_SCALING_UP);
}

LottieAnimationVisual& LottieAnimationVisual::SetRedrawInScalingUp(bool redrawInScalingUp)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::REDRAW_IN_SCALING_UP, redrawInScalingUp);
  return *this;
}

bool LottieAnimationVisual::IsEnableFrameCache() const
{
  return VisualBase::GetProperty<bool>(LottieAnimationVisual::Property::ENABLE_FRAME_CACHE);
}

LottieAnimationVisual& LottieAnimationVisual::SetEnableFrameCache(bool enableFrameCache)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::ENABLE_FRAME_CACHE, enableFrameCache);
  return *this;
}

bool LottieAnimationVisual::IsNotifyAfterRasterization() const
{
  return VisualBase::GetProperty<bool>(LottieAnimationVisual::Property::NOTIFY_AFTER_RASTERIZATION);
}

LottieAnimationVisual& LottieAnimationVisual::SetNotifyAfterRasterization(bool notifyAfterRasterization)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::NOTIFY_AFTER_RASTERIZATION, notifyAfterRasterization);
  return *this;
}

float LottieAnimationVisual::GetRenderScale() const
{
  return VisualBase::GetProperty<float>(LottieAnimationVisual::Property::RENDER_SCALE);
}

LottieAnimationVisual& LottieAnimationVisual::SetRenderScale(float renderScale)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::RENDER_SCALE, renderScale);
  return *this;
}

// =============================================================================
// Read-only properties
// =============================================================================

AnimatedImage::PlayState LottieAnimationVisual::GetPlayState() const
{
  AnimatedImage::PlayState result = VisualBase::GetProperty<AnimatedImage::PlayState>(LottieAnimationVisual::Property::PLAY_STATE);
  // We should not cache this property. Remove cache now.
  // TODO : Maybe need to make API to get proeprty without cache as method.
  GetImplementation(*this).RemoveCache(LottieAnimationVisual::Property::PLAY_STATE);
  return result;
}

int LottieAnimationVisual::GetCurrentFrameNumber() const
{
  int result = VisualBase::GetProperty<int>(LottieAnimationVisual::Property::CURRENT_FRAME_NUMBER);
  // We should not cache this property. Remove cache now.
  // TODO : Maybe need to make API to get proeprty without cache as method.
  GetImplementation(*this).RemoveCache(LottieAnimationVisual::Property::CURRENT_FRAME_NUMBER);
  return result;
}

int LottieAnimationVisual::GetTotalFrameNumber() const
{
  int result = VisualBase::GetProperty<int>(LottieAnimationVisual::Property::TOTAL_FRAME_NUMBER);
  if(result == -1)
  {
    // We should not cache this property only if result is -1. Remove cache now.
    GetImplementation(*this).RemoveCache(LottieAnimationVisual::Property::TOTAL_FRAME_NUMBER);
  }
  return result;
}

Dali::Property::Map LottieAnimationVisual::GetContentInfo() const
{
  Dali::Property::Map result = VisualBase::GetProperty<Dali::Property::Map>(LottieAnimationVisual::Property::CONTENT_INFO);
  // TODO : We need to seperate whether value not loaded yet, or value is not exist actually.
  if(result.Empty())
  {
    GetImplementation(*this).RemoveCache(LottieAnimationVisual::Property::CONTENT_INFO);
  }
  return result;
}

Dali::Property::Map LottieAnimationVisual::GetMarkerInfo() const
{
  Dali::Property::Map result = VisualBase::GetProperty<Dali::Property::Map>(LottieAnimationVisual::Property::MARKER_INFO);
  // TODO : We need to seperate whether value not loaded yet, or value is not exist actually.
  if(result.Empty())
  {
    GetImplementation(*this).RemoveCache(LottieAnimationVisual::Property::MARKER_INFO);
  }
  return result;
}

// =============================================================================
// Action API
// =============================================================================

LottieAnimationVisual& LottieAnimationVisual::Play()
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  VisualBase::DoAction(Ui::DevelAnimatedVectorImageVisual::Action::PLAY, Dali::Property::Value());
  return *this;
}

LottieAnimationVisual& LottieAnimationVisual::Pause()
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  VisualBase::DoAction(Ui::DevelAnimatedVectorImageVisual::Action::PAUSE, Dali::Property::Value());
  return *this;
}

LottieAnimationVisual& LottieAnimationVisual::Stop()
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  VisualBase::DoAction(Ui::DevelAnimatedVectorImageVisual::Action::STOP, Dali::Property::Value());
  return *this;
}

LottieAnimationVisual& LottieAnimationVisual::JumpTo(int frame)
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  VisualBase::DoAction(Ui::DevelAnimatedVectorImageVisual::Action::JUMP_TO, static_cast<int32_t>(frame));
  return *this;
}

LottieAnimationVisual& LottieAnimationVisual::SetDynamicProperty(const LottieAnimation::DynamicPropertyInfo& info)
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  Ui::DevelAnimatedVectorImageVisual::DynamicPropertyInfo dynamicInfo;
  dynamicInfo.id       = info.id;
  dynamicInfo.keyPath  = info.keyPath.CStr();
  dynamicInfo.property = static_cast<int32_t>(info.property);
  dynamicInfo.callback = info.callback;
  GetImplementation(*this).DoActionExtension(Ui::DevelAnimatedVectorImageVisual::Action::SET_DYNAMIC_PROPERTY, Dali::Any(dynamicInfo));
  return *this;
}

// =============================================================================
// Internal API
// =============================================================================

LottieAnimationVisual::LottieAnimationVisual(Dali::Ui::Internal::VisualBaseImpl* object)
: VisualBase(object)
{
}
} // namespace Ui
} // namespace Dali
