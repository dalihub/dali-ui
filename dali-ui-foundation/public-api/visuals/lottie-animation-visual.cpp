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
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/visuals/animated-vector-image-visual-actions-integ.h>
#include <dali-ui-foundation/integration-api/visuals/image-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
LottieAnimationVisual LottieAnimationVisual::New()
{
  Internal::VisualBaseImplPtr internal = Internal::VisualBaseImpl::New(Dali::Ui::VisualType::LOTTIE_ANIMATION);
  return LottieAnimationVisual(internal.Get());
}

LottieAnimationVisual LottieAnimationVisual::DownCast(BaseHandle handle)
{
  Internal::VisualBaseImpl* visualBaseImpl = dynamic_cast<Internal::VisualBaseImpl*>(handle.GetObjectPtr());
  if(visualBaseImpl && visualBaseImpl->GetVisualType() == Dali::Ui::VisualType::LOTTIE_ANIMATION)
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
  return GetImplementation(*this).GetProperty<Dali::String>(Dali::Ui::Integration::ImageVisual::Property::URL);
}

void LottieAnimationVisual::SetResourceUrl(const Dali::String& resourceUrl)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::URL, resourceUrl);
}

bool LottieAnimationVisual::IsSynchronousLoading() const
{
  return GetImplementation(*this).GetProperty<bool>(Dali::Ui::Integration::ImageVisual::Property::SYNCHRONOUS_LOADING);
}

void LottieAnimationVisual::SetSynchronousLoading(bool synchronous)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::SYNCHRONOUS_LOADING, synchronous);
}

int LottieAnimationVisual::GetDesiredWidth() const
{
  return GetImplementation(*this).GetProperty<int>(Dali::Ui::Integration::ImageVisual::Property::DESIRED_WIDTH);
}

void LottieAnimationVisual::SetDesiredWidth(int desiredWidth)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::DESIRED_WIDTH, desiredWidth);
}

int LottieAnimationVisual::GetDesiredHeight() const
{
  return GetImplementation(*this).GetProperty<int>(Dali::Ui::Integration::ImageVisual::Property::DESIRED_HEIGHT);
}

void LottieAnimationVisual::SetDesiredHeight(int desiredHeight)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::DESIRED_HEIGHT, desiredHeight);
}

Image::SamplingMode LottieAnimationVisual::GetSamplingMode() const
{
  return GetImplementation(*this).GetProperty<Image::SamplingMode>(Dali::Ui::Integration::ImageVisual::Property::SAMPLING_MODE);
}

void LottieAnimationVisual::SetSamplingMode(Image::SamplingMode samplingMode)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::SAMPLING_MODE, samplingMode);
}

Dali::Vector4 LottieAnimationVisual::GetPixelArea() const
{
  return GetImplementation(*this).GetProperty<Dali::Vector4>(Dali::Ui::Integration::ImageVisual::Property::PIXEL_AREA);
}

void LottieAnimationVisual::SetPixelArea(const Dali::Vector4& pixelArea)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::PIXEL_AREA, pixelArea);
}

Dali::WrapMode::Type LottieAnimationVisual::GetWrapModeU() const
{
  return GetImplementation(*this).GetProperty<Dali::WrapMode::Type>(Dali::Ui::Integration::ImageVisual::Property::WRAP_MODE_U);
}

void LottieAnimationVisual::SetWrapModeU(Dali::WrapMode::Type wrapModeU)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::WRAP_MODE_U, wrapModeU);
}

Dali::WrapMode::Type LottieAnimationVisual::GetWrapModeV() const
{
  return GetImplementation(*this).GetProperty<Dali::WrapMode::Type>(Dali::Ui::Integration::ImageVisual::Property::WRAP_MODE_V);
}

void LottieAnimationVisual::SetWrapModeV(Dali::WrapMode::Type wrapModeV)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::WRAP_MODE_V, wrapModeV);
}

bool LottieAnimationVisual::IsBrokenImageEnabled() const
{
  return GetImplementation(*this).GetProperty<bool>(Dali::Ui::Integration::ImageVisual::Property::ENABLE_BROKEN_IMAGE);
}

void LottieAnimationVisual::SetBrokenImageEnabled(bool brokenImageEnabled)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::ENABLE_BROKEN_IMAGE, brokenImageEnabled);
}

Image::LoadPolicy LottieAnimationVisual::GetLoadPolicy() const
{
  return GetImplementation(*this).GetProperty<Image::LoadPolicy>(Dali::Ui::Integration::ImageVisual::Property::LOAD_POLICY);
}

void LottieAnimationVisual::SetLoadPolicy(Image::LoadPolicy loadPolicy)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::LOAD_POLICY, loadPolicy);
}

Image::ReleasePolicy LottieAnimationVisual::GetReleasePolicy() const
{
  return GetImplementation(*this).GetProperty<Image::ReleasePolicy>(Dali::Ui::Integration::ImageVisual::Property::RELEASE_POLICY);
}

void LottieAnimationVisual::SetReleasePolicy(Image::ReleasePolicy releasePolicy)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::RELEASE_POLICY, releasePolicy);
}

bool LottieAnimationVisual::IsOrientationCorrectionEnabled() const
{
  return GetImplementation(*this).GetProperty<bool>(Dali::Ui::Integration::ImageVisual::Property::ORIENTATION_CORRECTION);
}

void LottieAnimationVisual::SetOrientationCorrection(bool orientationCorrection)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::ORIENTATION_CORRECTION, orientationCorrection);
}

bool LottieAnimationVisual::IsImageLoadWithViewSizeEnabled() const
{
  return GetImplementation(*this).GetProperty<bool>(Dali::Ui::Integration::ImageVisual::Property::IMAGE_LOAD_WITH_VIEW_SIZE);
}

void LottieAnimationVisual::SetImageLoadWithViewSize(bool enabled)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::IMAGE_LOAD_WITH_VIEW_SIZE, enabled);
}

int LottieAnimationVisual::GetLoopCount() const
{
  return GetImplementation(*this).GetProperty<int>(Dali::Ui::Integration::ImageVisual::Property::LOOP_COUNT);
}

void LottieAnimationVisual::SetLoopCount(int loopCount)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::LOOP_COUNT, loopCount);
}

void LottieAnimationVisual::SetMinMaxFrame(int minFrame, int maxFrame)
{
  Dali::Property::Array range;
  range.PushBack(minFrame);
  range.PushBack(maxFrame);
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::PLAY_RANGE, range);
}

void LottieAnimationVisual::GetMinMaxFrame(int& minFrame, int& maxFrame) const
{
  Dali::Property::Value  value    = GetImplementation(*this).GetProperty(Dali::Ui::Integration::ImageVisual::Property::PLAY_RANGE);
  Dali::Property::Array* arrayPtr = value.GetArray();
  if(arrayPtr && arrayPtr->Count() >= 2u &&
     arrayPtr->GetElementAt(0).Get(minFrame) && arrayPtr->GetElementAt(1).Get(maxFrame))
  {
    return;
  }

  // No numeric range is set, including when marker names were used.
  minFrame = 0;
  maxFrame = GetTotalFrameCount();
}

void LottieAnimationVisual::SetMinMaxFrameByMarker(const Dali::String& minMarker, const Dali::String& maxMarker)
{
  Dali::Property::Array range;
  range.PushBack(minMarker);
  if(!maxMarker.Empty())
  {
    range.PushBack(maxMarker);
  }
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::PLAY_RANGE, range);
}

AnimatedImage::StopBehavior LottieAnimationVisual::GetStopBehavior() const
{
  return GetImplementation(*this).GetProperty<AnimatedImage::StopBehavior>(Dali::Ui::Integration::ImageVisual::Property::STOP_BEHAVIOR);
}

void LottieAnimationVisual::SetStopBehavior(AnimatedImage::StopBehavior stopBehavior)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::STOP_BEHAVIOR, stopBehavior);
}

float LottieAnimationVisual::GetFrameSpeedFactor() const
{
  return GetImplementation(*this).GetProperty<float>(Dali::Ui::Integration::ImageVisual::Property::FRAME_SPEED_FACTOR);
}

void LottieAnimationVisual::SetFrameSpeedFactor(float frameSpeedFactor)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::FRAME_SPEED_FACTOR, frameSpeedFactor);
}

LottieAnimation::LoopingMode LottieAnimationVisual::GetLoopingMode() const
{
  return GetImplementation(*this).GetProperty<LottieAnimation::LoopingMode>(Dali::Ui::Integration::ImageVisual::Property::LOOPING_MODE);
}

void LottieAnimationVisual::SetLoopingMode(LottieAnimation::LoopingMode loopingMode)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::LOOPING_MODE, loopingMode);
}

bool LottieAnimationVisual::IsRedrawOnScaleDown() const
{
  return GetImplementation(*this).GetProperty<bool>(Dali::Ui::Integration::ImageVisual::Property::REDRAW_IN_SCALING_DOWN);
}

void LottieAnimationVisual::SetRedrawOnScaleDown(bool redrawInScalingDown)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::REDRAW_IN_SCALING_DOWN, redrawInScalingDown);
}

bool LottieAnimationVisual::IsRedrawOnScaleUp() const
{
  return GetImplementation(*this).GetProperty<bool>(Dali::Ui::Integration::ImageVisual::Property::REDRAW_IN_SCALING_UP);
}

void LottieAnimationVisual::SetRedrawOnScaleUp(bool redrawInScalingUp)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::REDRAW_IN_SCALING_UP, redrawInScalingUp);
}

bool LottieAnimationVisual::IsNotifyAfterRasterizationEnabled() const
{
  return GetImplementation(*this).GetProperty<bool>(Dali::Ui::Integration::ImageVisual::Property::NOTIFY_AFTER_RASTERIZATION);
}

void LottieAnimationVisual::SetNotifyAfterRasterizationEnabled(bool notifyAfterRasterization)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::NOTIFY_AFTER_RASTERIZATION, notifyAfterRasterization);
}

float LottieAnimationVisual::GetRenderScale() const
{
  return GetImplementation(*this).GetProperty<float>(Dali::Ui::Integration::ImageVisual::Property::RENDER_SCALE);
}

void LottieAnimationVisual::SetRenderScale(float renderScale)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::RENDER_SCALE, renderScale);
}

bool LottieAnimationVisual::IsAspectFitEnabled() const
{
  return GetImplementation(*this).GetProperty<bool>(Dali::Ui::Integration::ImageVisual::Property::ENABLE_ASPECT_FIT);
}

void LottieAnimationVisual::SetAspectFitEnabled(bool aspectFitEnabled)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::ENABLE_ASPECT_FIT, aspectFitEnabled);
}

// =============================================================================
// Read-only properties
// =============================================================================

AnimatedImage::PlayState LottieAnimationVisual::GetPlayState() const
{
  // We should not cache this property. Remove cache now.
  // TODO : Maybe need to make API to get proeprty without cache as method.
  GetImplementation(*this).RemoveCache(Dali::Ui::Integration::ImageVisual::Property::PLAY_STATE);
  AnimatedImage::PlayState result = GetImplementation(*this).GetProperty<AnimatedImage::PlayState>(Dali::Ui::Integration::ImageVisual::Property::PLAY_STATE);
  return result;
}

int LottieAnimationVisual::GetCurrentFrameNumber() const
{
  // We should not cache this property. Remove cache now.
  // TODO : Maybe need to make API to get proeprty without cache as method.
  GetImplementation(*this).RemoveCache(Dali::Ui::Integration::ImageVisual::Property::CURRENT_FRAME_NUMBER);
  int result = GetImplementation(*this).GetProperty<int>(Dali::Ui::Integration::ImageVisual::Property::CURRENT_FRAME_NUMBER);
  return result;
}

int LottieAnimationVisual::GetTotalFrameCount() const
{
  int result = GetImplementation(*this).GetProperty<int>(Dali::Ui::Integration::ImageVisual::Property::TOTAL_FRAME_COUNT);
  if(result <= 0)
  {
    // We should not cache this property only if result is less or equal than 0. Remove cache now.
    GetImplementation(*this).RemoveCache(Dali::Ui::Integration::ImageVisual::Property::TOTAL_FRAME_COUNT);
  }
  return result;
}

Dali::Property::Map LottieAnimationVisual::GetContentInfo() const
{
  Dali::Property::Map result = GetImplementation(*this).GetProperty<Dali::Property::Map>(Dali::Ui::Integration::ImageVisual::Property::CONTENT_INFO);
  // TODO : We need to separate whether value not loaded yet, or value is not exist actually.
  if(result.Empty())
  {
    GetImplementation(*this).RemoveCache(Dali::Ui::Integration::ImageVisual::Property::CONTENT_INFO);
  }
  return result;
}

Dali::Property::Map LottieAnimationVisual::GetMarkerInfo() const
{
  Dali::Property::Map result = GetImplementation(*this).GetProperty<Dali::Property::Map>(Dali::Ui::Integration::ImageVisual::Property::MARKER_INFO);
  // TODO : We need to separate whether value not loaded yet, or value is not exist actually.
  if(result.Empty())
  {
    GetImplementation(*this).RemoveCache(Dali::Ui::Integration::ImageVisual::Property::MARKER_INFO);
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
  GetImplementation(*this).DoAction(Ui::Integration::AnimatedVectorImageVisual::Action::PLAY, Dali::Property::Value());
}

void LottieAnimationVisual::Pause()
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  GetImplementation(*this).DoAction(Ui::Integration::AnimatedVectorImageVisual::Action::PAUSE, Dali::Property::Value());
}

void LottieAnimationVisual::Stop()
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  GetImplementation(*this).DoAction(Ui::Integration::AnimatedVectorImageVisual::Action::STOP, Dali::Property::Value());
}

void LottieAnimationVisual::JumpToFrame(int frame)
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  GetImplementation(*this).DoAction(Ui::Integration::AnimatedVectorImageVisual::Action::JUMP_TO, static_cast<int32_t>(frame));
}

void LottieAnimationVisual::SetDynamicProperty(LottieAnimation::DynamicProperty info)
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  Ui::Integration::AnimatedVectorImageVisual::DynamicProperty dynamicInfo;
  dynamicInfo.id       = info.GetId();
  dynamicInfo.keyPath  = info.GetKeyPath().CStr();
  dynamicInfo.property = static_cast<int32_t>(info.GetProperty());
  dynamicInfo.callback = Ui::Integration::AnimatedVectorImageVisual::WrapDynamicPropertyCallback(std::move(info.GetCallback()));
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
} //namespace DALI_NAMESPACE
