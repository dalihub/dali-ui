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
#include <dali-ui-foundation/public-api/visuals/animated-image-visual.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/visuals/animated-image-visual-actions-integ.h>
#include <dali-ui-foundation/integration-api/visuals/image-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>

namespace Dali
{
namespace Ui
{
AnimatedImageVisual AnimatedImageVisual::New()
{
  Internal::VisualBaseImplPtr internal = Internal::VisualBaseImpl::New(Dali::Ui::VisualType::ANIMATED_IMAGE);
  return AnimatedImageVisual(internal.Get());
}

AnimatedImageVisual AnimatedImageVisual::DownCast(BaseHandle handle)
{
  Internal::VisualBaseImpl* visualBaseImpl = dynamic_cast<Internal::VisualBaseImpl*>(handle.GetObjectPtr());
  if(visualBaseImpl && visualBaseImpl->GetVisualType() == Dali::Ui::VisualType::ANIMATED_IMAGE)
  {
    return AnimatedImageVisual(visualBaseImpl);
  }
  return AnimatedImageVisual();
}

// =============================================================================
// Properties
// =============================================================================

Dali::String AnimatedImageVisual::GetResourceUrl() const
{
  Dali::Property::Value value = GetImplementation(*this).GetProperty(Dali::Ui::Integration::ImageVisual::Property::URL);
  if(value.GetType() == Dali::Property::STRING)
  {
    return value.Get<Dali::String>();
  }
  return Dali::String();
}

void AnimatedImageVisual::SetResourceUrl(const Dali::String& resourceUrl)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::URL, resourceUrl);
}

Dali::Vector<Dali::String> AnimatedImageVisual::GetResourceUrlList() const
{
  Dali::Vector<Dali::String> result;
  Dali::Property::Value      value = GetImplementation(*this).GetProperty(Dali::Ui::Integration::ImageVisual::Property::URL);
  if(value.GetType() == Dali::Property::ARRAY)
  {
    const Dali::Property::Array& array = *(value.GetArray());
    result.Reserve(array.Count());
    for(uint32_t i = 0u; i < array.Count(); ++i)
    {
      const Dali::Property::Value& arrayValue = array[i];
      if(DALI_LIKELY(arrayValue.GetType() == Dali::Property::STRING))
      {
        result.PushBack(arrayValue.Get<Dali::String>());
      }
    }
    return result;
  }
  return result;
}

void AnimatedImageVisual::SetResourceUrlList(const Dali::Vector<Dali::String>& resourceUrlList)
{
  Dali::Property::Array array;
  array.Reserve(static_cast<Dali::Property::Array::SizeType>(resourceUrlList.Count()));
  for(const auto& resourceUrl : resourceUrlList)
  {
    array.PushBack(resourceUrl);
  }
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::URL, array);
}

bool AnimatedImageVisual::IsSynchronousLoading() const
{
  return GetImplementation(*this).GetProperty<bool>(Dali::Ui::Integration::ImageVisual::Property::SYNCHRONOUS_LOADING);
}

void AnimatedImageVisual::SetSynchronousLoading(bool synchronous)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::SYNCHRONOUS_LOADING, synchronous);
}

int AnimatedImageVisual::GetDesiredWidth() const
{
  return GetImplementation(*this).GetProperty<int>(Dali::Ui::Integration::ImageVisual::Property::DESIRED_WIDTH);
}

void AnimatedImageVisual::SetDesiredWidth(int desiredWidth)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::DESIRED_WIDTH, desiredWidth);
}

int AnimatedImageVisual::GetDesiredHeight() const
{
  return GetImplementation(*this).GetProperty<int>(Dali::Ui::Integration::ImageVisual::Property::DESIRED_HEIGHT);
}

void AnimatedImageVisual::SetDesiredHeight(int desiredHeight)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::DESIRED_HEIGHT, desiredHeight);
}

Image::SamplingMode AnimatedImageVisual::GetSamplingMode() const
{
  return GetImplementation(*this).GetProperty<Image::SamplingMode>(Dali::Ui::Integration::ImageVisual::Property::SAMPLING_MODE);
}

void AnimatedImageVisual::SetSamplingMode(Image::SamplingMode samplingMode)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::SAMPLING_MODE, samplingMode);
}

Dali::Vector4 AnimatedImageVisual::GetPixelArea() const
{
  return GetImplementation(*this).GetProperty<Dali::Vector4>(Dali::Ui::Integration::ImageVisual::Property::PIXEL_AREA);
}

void AnimatedImageVisual::SetPixelArea(const Dali::Vector4& pixelArea)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::PIXEL_AREA, pixelArea);
}

Dali::WrapMode::Type AnimatedImageVisual::GetWrapModeU() const
{
  return GetImplementation(*this).GetProperty<Dali::WrapMode::Type>(Dali::Ui::Integration::ImageVisual::Property::WRAP_MODE_U);
}

void AnimatedImageVisual::SetWrapModeU(Dali::WrapMode::Type wrapModeU)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::WRAP_MODE_U, wrapModeU);
}

Dali::WrapMode::Type AnimatedImageVisual::GetWrapModeV() const
{
  return GetImplementation(*this).GetProperty<Dali::WrapMode::Type>(Dali::Ui::Integration::ImageVisual::Property::WRAP_MODE_V);
}

void AnimatedImageVisual::SetWrapModeV(Dali::WrapMode::Type wrapModeV)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::WRAP_MODE_V, wrapModeV);
}

bool AnimatedImageVisual::IsBrokenImageEnabled() const
{
  return GetImplementation(*this).GetProperty<bool>(Dali::Ui::Integration::ImageVisual::Property::ENABLE_BROKEN_IMAGE);
}

void AnimatedImageVisual::SetBrokenImageEnabled(bool brokenImageEnabled)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::ENABLE_BROKEN_IMAGE, brokenImageEnabled);
}

Image::LoadPolicy AnimatedImageVisual::GetLoadPolicy() const
{
  return GetImplementation(*this).GetProperty<Image::LoadPolicy>(Dali::Ui::Integration::ImageVisual::Property::LOAD_POLICY);
}

void AnimatedImageVisual::SetLoadPolicy(Image::LoadPolicy loadPolicy)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::LOAD_POLICY, loadPolicy);
}

Image::ReleasePolicy AnimatedImageVisual::GetReleasePolicy() const
{
  return GetImplementation(*this).GetProperty<Image::ReleasePolicy>(Dali::Ui::Integration::ImageVisual::Property::RELEASE_POLICY);
}

void AnimatedImageVisual::SetReleasePolicy(Image::ReleasePolicy releasePolicy)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::RELEASE_POLICY, releasePolicy);
}

Image::FittingMode AnimatedImageVisual::GetFittingMode() const
{
  return GetImplementation(*this).GetProperty<Image::FittingMode>(Dali::Ui::Integration::ImageVisual::Property::FITTING_MODE);
}

void AnimatedImageVisual::SetFittingMode(Image::FittingMode fittingMode)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::FITTING_MODE, fittingMode);
}

bool AnimatedImageVisual::IsOrientationCorrection() const
{
  return GetImplementation(*this).GetProperty<bool>(Dali::Ui::Integration::ImageVisual::Property::ORIENTATION_CORRECTION);
}

void AnimatedImageVisual::SetOrientationCorrection(bool orientationCorrection)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::ORIENTATION_CORRECTION, orientationCorrection);
}

bool AnimatedImageVisual::IsSynchronousSizing() const
{
  return GetImplementation(*this).GetProperty<bool>(Dali::Ui::Integration::ImageVisual::Property::SYNCHRONOUS_SIZING);
}

void AnimatedImageVisual::SetSynchronousSizing(bool synchronousSizing)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::SYNCHRONOUS_SIZING, synchronousSizing);
}

bool AnimatedImageVisual::IsPreMultipliedAlpha() const
{
  return GetImplementation(*this).GetProperty<bool>(Dali::Ui::Integration::ImageVisual::Property::PRE_MULTIPLIED_ALPHA);
}

void AnimatedImageVisual::SetPreMultipliedAlpha(bool preMultiplied)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::PRE_MULTIPLIED_ALPHA, preMultiplied);
}

Dali::String AnimatedImageVisual::GetAlphaMaskUrl() const
{
  return GetImplementation(*this).GetProperty<Dali::String>(Dali::Ui::Integration::ImageVisual::Property::ALPHA_MASK_URL);
}

void AnimatedImageVisual::SetAlphaMaskUrl(const Dali::String& alphaMaskUrl)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::ALPHA_MASK_URL, alphaMaskUrl);
}

float AnimatedImageVisual::GetMaskContentScale() const
{
  return GetImplementation(*this).GetProperty<float>(Dali::Ui::Integration::ImageVisual::Property::MASK_CONTENT_SCALE);
}

void AnimatedImageVisual::SetMaskContentScale(float maskContentScale)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::MASK_CONTENT_SCALE, maskContentScale);
}

bool AnimatedImageVisual::IsCropToMask() const
{
  return GetImplementation(*this).GetProperty<bool>(Dali::Ui::Integration::ImageVisual::Property::CROP_TO_MASK);
}

void AnimatedImageVisual::SetCropToMask(bool cropToMask)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::CROP_TO_MASK, cropToMask);
}

Image::MaskingType AnimatedImageVisual::GetMaskingType() const
{
  return GetImplementation(*this).GetProperty<Image::MaskingType>(Dali::Ui::Integration::ImageVisual::Property::MASKING_TYPE);
}

void AnimatedImageVisual::SetMaskingType(Image::MaskingType maskingType)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::MASKING_TYPE, maskingType);
}

int AnimatedImageVisual::GetLoopCount() const
{
  return GetImplementation(*this).GetProperty<int>(Dali::Ui::Integration::ImageVisual::Property::LOOP_COUNT);
}

void AnimatedImageVisual::SetLoopCount(int loopCount)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::LOOP_COUNT, loopCount);
}

Dali::Property::Array AnimatedImageVisual::GetPlayRange() const
{
  Dali::Property::Value  value    = GetImplementation(*this).GetProperty(Dali::Ui::Integration::ImageVisual::Property::PLAY_RANGE);
  Dali::Property::Array* arrayPtr = value.GetArray();
  if(arrayPtr)
  {
    return *arrayPtr;
  }
  return Dali::Property::Array();
}

void AnimatedImageVisual::SetPlayRange(const Dali::Property::Array& playRange)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::PLAY_RANGE, playRange);
}

AnimatedImage::StopBehavior AnimatedImageVisual::GetStopBehavior() const
{
  return GetImplementation(*this).GetProperty<AnimatedImage::StopBehavior>(Dali::Ui::Integration::ImageVisual::Property::STOP_BEHAVIOR);
}

void AnimatedImageVisual::SetStopBehavior(AnimatedImage::StopBehavior stopBehavior)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::STOP_BEHAVIOR, stopBehavior);
}

float AnimatedImageVisual::GetFrameSpeedFactor() const
{
  return GetImplementation(*this).GetProperty<float>(Dali::Ui::Integration::ImageVisual::Property::FRAME_SPEED_FACTOR);
}

void AnimatedImageVisual::SetFrameSpeedFactor(float frameSpeedFactor)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::FRAME_SPEED_FACTOR, frameSpeedFactor);
}

int AnimatedImageVisual::GetBatchSize() const
{
  return GetImplementation(*this).GetProperty<int>(Dali::Ui::Integration::ImageVisual::Property::BATCH_SIZE);
}

void AnimatedImageVisual::SetBatchSize(int batchSize)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::BATCH_SIZE, batchSize);
}

int AnimatedImageVisual::GetCacheSize() const
{
  return GetImplementation(*this).GetProperty<int>(Dali::Ui::Integration::ImageVisual::Property::CACHE_SIZE);
}

void AnimatedImageVisual::SetCacheSize(int cacheSize)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::CACHE_SIZE, cacheSize);
}

int AnimatedImageVisual::GetFrameDelay() const
{
  return GetImplementation(*this).GetProperty<int>(Dali::Ui::Integration::ImageVisual::Property::FRAME_DELAY);
}

void AnimatedImageVisual::SetFrameDelay(int frameDelay)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::FRAME_DELAY, frameDelay);
}

// =============================================================================
// Read-only properties
// =============================================================================

AnimatedImage::PlayState AnimatedImageVisual::GetPlayState() const
{
  AnimatedImage::PlayState result = GetImplementation(*this).GetProperty<AnimatedImage::PlayState>(Dali::Ui::Integration::ImageVisual::Property::PLAY_STATE);
  // We should not cache this property. Remove cache now.
  // TODO : Maybe need to make API to get proeprty without cache as method.
  GetImplementation(*this).RemoveCache(Dali::Ui::Integration::ImageVisual::Property::PLAY_STATE);
  return result;
}

int AnimatedImageVisual::GetCurrentFrameNumber() const
{
  int result = GetImplementation(*this).GetProperty<int>(Dali::Ui::Integration::ImageVisual::Property::CURRENT_FRAME_NUMBER);
  // We should not cache this property. Remove cache now.
  // TODO : Maybe need to make API to get proeprty without cache as method.
  GetImplementation(*this).RemoveCache(Dali::Ui::Integration::ImageVisual::Property::CURRENT_FRAME_NUMBER);
  return result;
}

int AnimatedImageVisual::GetTotalFrameNumber() const
{
  int result = GetImplementation(*this).GetProperty<int>(Dali::Ui::Integration::ImageVisual::Property::TOTAL_FRAME_NUMBER);
  if(result == -1)
  {
    // We should not cache this property only if result is -1. Remove cache now.
    GetImplementation(*this).RemoveCache(Dali::Ui::Integration::ImageVisual::Property::TOTAL_FRAME_NUMBER);
  }
  return result;
}

// =============================================================================
// Action API
// =============================================================================

void AnimatedImageVisual::Play()
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  GetImplementation(*this).DoAction(Ui::Integration::AnimatedImageVisual::Action::PLAY, Dali::Property::Value());
}

void AnimatedImageVisual::Pause()
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  GetImplementation(*this).DoAction(Ui::Integration::AnimatedImageVisual::Action::PAUSE, Dali::Property::Value());
}

void AnimatedImageVisual::Stop()
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  GetImplementation(*this).DoAction(Ui::Integration::AnimatedImageVisual::Action::STOP, Dali::Property::Value());
}

void AnimatedImageVisual::JumpTo(int frame)
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  GetImplementation(*this).DoAction(Ui::Integration::AnimatedImageVisual::Action::JUMP_TO, static_cast<int32_t>(frame));
}

// =============================================================================
// Internal API
// =============================================================================

AnimatedImageVisual::AnimatedImageVisual(Dali::Ui::Internal::VisualBaseImpl* object)
: VisualBase(object)
{
}
} // namespace Ui
} // namespace Dali
