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
#include <dali-ui-foundation/integration-api/visuals/animated-vector-image-visual-actions-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>

namespace Dali
{
namespace Ui
{
LottieAnimationVisual LottieAnimationVisual::New()
{
  Internal::VisualBaseImplPtr internal = Internal::VisualBaseImpl::New(Dali::Ui::VisualType::ANIMATED_VECTOR_IMAGE);
  return LottieAnimationVisual(internal.Get());
}

LottieAnimationVisual LottieAnimationVisual::DownCast(BaseHandle handle)
{
  Internal::VisualBaseImpl* visualBaseImpl = dynamic_cast<Internal::VisualBaseImpl*>(handle.GetObjectPtr());
  if(visualBaseImpl && visualBaseImpl->GetVisualType() == Dali::Ui::VisualType::ANIMATED_VECTOR_IMAGE)
  {
    return LottieAnimationVisual(visualBaseImpl);
  }
  return LottieAnimationVisual();
}

// =============================================================================
// Properties
// =============================================================================

Dali::String LottieAnimationVisual::GetResourceUrl() const
{
  return VisualBase::GetProperty<Dali::String>(LottieAnimationVisual::Property::URL);
}

void LottieAnimationVisual::SetResourceUrl(const Dali::String& resourceUrl)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::URL, resourceUrl);
}

bool LottieAnimationVisual::IsSynchronousLoading() const
{
  return VisualBase::GetProperty<bool>(LottieAnimationVisual::Property::SYNCHRONOUS_LOADING);
}

void LottieAnimationVisual::SetSynchronousLoading(bool synchronous)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::SYNCHRONOUS_LOADING, synchronous);
}

int LottieAnimationVisual::GetDesiredWidth() const
{
  return VisualBase::GetProperty<int>(LottieAnimationVisual::Property::DESIRED_WIDTH);
}

void LottieAnimationVisual::SetDesiredWidth(int desiredWidth)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::DESIRED_WIDTH, desiredWidth);
}

int LottieAnimationVisual::GetDesiredHeight() const
{
  return VisualBase::GetProperty<int>(LottieAnimationVisual::Property::DESIRED_HEIGHT);
}

void LottieAnimationVisual::SetDesiredHeight(int desiredHeight)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::DESIRED_HEIGHT, desiredHeight);
}

Image::SamplingMode LottieAnimationVisual::GetSamplingMode() const
{
  return VisualBase::GetProperty<Image::SamplingMode>(LottieAnimationVisual::Property::SAMPLING_MODE);
}

void LottieAnimationVisual::SetSamplingMode(Image::SamplingMode samplingMode)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::SAMPLING_MODE, samplingMode);
}

Dali::Vector4 LottieAnimationVisual::GetPixelArea() const
{
  return VisualBase::GetProperty<Dali::Vector4>(LottieAnimationVisual::Property::PIXEL_AREA);
}

void LottieAnimationVisual::SetPixelArea(const Dali::Vector4& pixelArea)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::PIXEL_AREA, pixelArea);
}

Dali::WrapMode::Type LottieAnimationVisual::GetWrapModeU() const
{
  return VisualBase::GetProperty<Dali::WrapMode::Type>(LottieAnimationVisual::Property::WRAP_MODE_U);
}

void LottieAnimationVisual::SetWrapModeU(Dali::WrapMode::Type wrapModeU)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::WRAP_MODE_U, wrapModeU);
}

Dali::WrapMode::Type LottieAnimationVisual::GetWrapModeV() const
{
  return VisualBase::GetProperty<Dali::WrapMode::Type>(LottieAnimationVisual::Property::WRAP_MODE_V);
}

void LottieAnimationVisual::SetWrapModeV(Dali::WrapMode::Type wrapModeV)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::WRAP_MODE_V, wrapModeV);
}

bool LottieAnimationVisual::IsBrokenImageEnabled() const
{
  return VisualBase::GetProperty<bool>(LottieAnimationVisual::Property::ENABLE_BROKEN_IMAGE);
}

void LottieAnimationVisual::SetBrokenImageEnabled(bool brokenImageEnabled)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::ENABLE_BROKEN_IMAGE, brokenImageEnabled);
}

Image::LoadPolicy LottieAnimationVisual::GetLoadPolicy() const
{
  return VisualBase::GetProperty<Image::LoadPolicy>(LottieAnimationVisual::Property::LOAD_POLICY);
}

void LottieAnimationVisual::SetLoadPolicy(Image::LoadPolicy loadPolicy)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::LOAD_POLICY, loadPolicy);
}

Image::ReleasePolicy LottieAnimationVisual::GetReleasePolicy() const
{
  return VisualBase::GetProperty<Image::ReleasePolicy>(LottieAnimationVisual::Property::RELEASE_POLICY);
}

void LottieAnimationVisual::SetReleasePolicy(Image::ReleasePolicy releasePolicy)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::RELEASE_POLICY, releasePolicy);
}

bool LottieAnimationVisual::IsOrientationCorrection() const
{
  return VisualBase::GetProperty<bool>(LottieAnimationVisual::Property::ORIENTATION_CORRECTION);
}

void LottieAnimationVisual::SetOrientationCorrection(bool orientationCorrection)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::ORIENTATION_CORRECTION, orientationCorrection);
}

bool LottieAnimationVisual::IsSynchronousSizing() const
{
  return VisualBase::GetProperty<bool>(LottieAnimationVisual::Property::SYNCHRONOUS_SIZING);
}

void LottieAnimationVisual::SetSynchronousSizing(bool synchronousSizing)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::SYNCHRONOUS_SIZING, synchronousSizing);
}

int LottieAnimationVisual::GetLoopCount() const
{
  return VisualBase::GetProperty<int>(LottieAnimationVisual::Property::LOOP_COUNT);
}

void LottieAnimationVisual::SetLoopCount(int loopCount)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::LOOP_COUNT, loopCount);
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

void LottieAnimationVisual::SetPlayRange(const Dali::Property::Array& playRange)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::PLAY_RANGE, playRange);
}

AnimatedImage::StopBehavior LottieAnimationVisual::GetStopBehavior() const
{
  return VisualBase::GetProperty<AnimatedImage::StopBehavior>(LottieAnimationVisual::Property::STOP_BEHAVIOR);
}

void LottieAnimationVisual::SetStopBehavior(AnimatedImage::StopBehavior stopBehavior)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::STOP_BEHAVIOR, stopBehavior);
}

float LottieAnimationVisual::GetFrameSpeedFactor() const
{
  return VisualBase::GetProperty<float>(LottieAnimationVisual::Property::FRAME_SPEED_FACTOR);
}

void LottieAnimationVisual::SetFrameSpeedFactor(float frameSpeedFactor)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::FRAME_SPEED_FACTOR, frameSpeedFactor);
}

LottieAnimation::LoopingMode LottieAnimationVisual::GetLoopingMode() const
{
  return VisualBase::GetProperty<LottieAnimation::LoopingMode>(LottieAnimationVisual::Property::LOOPING_MODE);
}

void LottieAnimationVisual::SetLoopingMode(LottieAnimation::LoopingMode loopingMode)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::LOOPING_MODE, loopingMode);
}

bool LottieAnimationVisual::IsRedrawInScalingDown() const
{
  return VisualBase::GetProperty<bool>(LottieAnimationVisual::Property::REDRAW_IN_SCALING_DOWN);
}

void LottieAnimationVisual::SetRedrawInScalingDown(bool redrawInScalingDown)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::REDRAW_IN_SCALING_DOWN, redrawInScalingDown);
}

bool LottieAnimationVisual::IsRedrawInScalingUp() const
{
  return VisualBase::GetProperty<bool>(LottieAnimationVisual::Property::REDRAW_IN_SCALING_UP);
}

void LottieAnimationVisual::SetRedrawInScalingUp(bool redrawInScalingUp)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::REDRAW_IN_SCALING_UP, redrawInScalingUp);
}

bool LottieAnimationVisual::IsFrameCacheEnabled() const
{
  return VisualBase::GetProperty<bool>(LottieAnimationVisual::Property::ENABLE_FRAME_CACHE);
}

void LottieAnimationVisual::SetFrameCacheEnabled(bool frameCacheEnabled)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::ENABLE_FRAME_CACHE, frameCacheEnabled);
}

bool LottieAnimationVisual::IsNotifyAfterRasterization() const
{
  return VisualBase::GetProperty<bool>(LottieAnimationVisual::Property::NOTIFY_AFTER_RASTERIZATION);
}

void LottieAnimationVisual::SetNotifyAfterRasterization(bool notifyAfterRasterization)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::NOTIFY_AFTER_RASTERIZATION, notifyAfterRasterization);
}

float LottieAnimationVisual::GetRenderScale() const
{
  return VisualBase::GetProperty<float>(LottieAnimationVisual::Property::RENDER_SCALE);
}

void LottieAnimationVisual::SetRenderScale(float renderScale)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::RENDER_SCALE, renderScale);
}

bool LottieAnimationVisual::IsAspectFitEnabled() const
{
  return VisualBase::GetProperty<bool>(LottieAnimationVisual::Property::ENABLE_ASPECT_FIT);
}

void LottieAnimationVisual::SetAspectFitEnabled(bool aspectFitEnabled)
{
  VisualBase::SetProperty(LottieAnimationVisual::Property::ENABLE_ASPECT_FIT, aspectFitEnabled);
}

// =============================================================================
// Read-only properties
// =============================================================================

AnimatedImage::PlayState LottieAnimationVisual::GetPlayState() const
{
  // We should not cache this property. Remove cache now.
  // TODO : Maybe need to make API to get proeprty without cache as method.
  GetImplementation(*this).RemoveCache(LottieAnimationVisual::Property::PLAY_STATE);
  AnimatedImage::PlayState result = VisualBase::GetProperty<AnimatedImage::PlayState>(LottieAnimationVisual::Property::PLAY_STATE);
  return result;
}

int LottieAnimationVisual::GetCurrentFrameNumber() const
{
  // We should not cache this property. Remove cache now.
  // TODO : Maybe need to make API to get proeprty without cache as method.
  GetImplementation(*this).RemoveCache(LottieAnimationVisual::Property::CURRENT_FRAME_NUMBER);
  int result = VisualBase::GetProperty<int>(LottieAnimationVisual::Property::CURRENT_FRAME_NUMBER);
  return result;
}

int LottieAnimationVisual::GetTotalFrameNumber() const
{
  int result = VisualBase::GetProperty<int>(LottieAnimationVisual::Property::TOTAL_FRAME_NUMBER);
  if(result <= 0)
  {
    // We should not cache this property only if result is less or equal than 0. Remove cache now.
    GetImplementation(*this).RemoveCache(LottieAnimationVisual::Property::TOTAL_FRAME_NUMBER);
  }
  return result;
}

Dali::Property::Map LottieAnimationVisual::GetContentInfo() const
{
  Dali::Property::Map result = VisualBase::GetProperty<Dali::Property::Map>(LottieAnimationVisual::Property::CONTENT_INFO);
  // TODO : We need to separate whether value not loaded yet, or value is not exist actually.
  if(result.Empty())
  {
    GetImplementation(*this).RemoveCache(LottieAnimationVisual::Property::CONTENT_INFO);
  }
  return result;
}

Dali::Property::Map LottieAnimationVisual::GetMarkerInfo() const
{
  Dali::Property::Map result = VisualBase::GetProperty<Dali::Property::Map>(LottieAnimationVisual::Property::MARKER_INFO);
  // TODO : We need to separate whether value not loaded yet, or value is not exist actually.
  if(result.Empty())
  {
    GetImplementation(*this).RemoveCache(LottieAnimationVisual::Property::MARKER_INFO);
  }
  return result;
}

// =============================================================================
// Action API
// =============================================================================

void LottieAnimationVisual::Play()
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  VisualBase::DoAction(Ui::Integration::AnimatedVectorImageVisual::Action::PLAY, Dali::Property::Value());
}

void LottieAnimationVisual::Pause()
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  VisualBase::DoAction(Ui::Integration::AnimatedVectorImageVisual::Action::PAUSE, Dali::Property::Value());
}

void LottieAnimationVisual::Stop()
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  VisualBase::DoAction(Ui::Integration::AnimatedVectorImageVisual::Action::STOP, Dali::Property::Value());
}

void LottieAnimationVisual::JumpTo(int frame)
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  VisualBase::DoAction(Ui::Integration::AnimatedVectorImageVisual::Action::JUMP_TO, static_cast<int32_t>(frame));
}

void LottieAnimationVisual::SetDynamicProperty(const LottieAnimation::DynamicPropertyInfo& info)
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  Ui::Integration::AnimatedVectorImageVisual::DynamicPropertyInfo dynamicInfo;
  dynamicInfo.id       = info.id;
  dynamicInfo.keyPath  = info.keyPath.CStr();
  dynamicInfo.property = static_cast<int32_t>(info.property);
  dynamicInfo.callback = info.callback;
  GetImplementation(*this).DoActionExtension(Ui::Integration::AnimatedVectorImageVisual::Action::SET_DYNAMIC_PROPERTY, Dali::Any(dynamicInfo));
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
