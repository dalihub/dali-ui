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
  Dali::Property::Value value = VisualBase::GetProperty(AnimatedImageVisual::Property::URL);
  if(value.GetType() == Dali::Property::STRING)
  {
    return value.Get<Dali::String>();
  }
  return Dali::String();
}

void AnimatedImageVisual::SetResourceUrl(const Dali::String& resourceUrl)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::URL, resourceUrl);
}

Dali::Vector<Dali::String> AnimatedImageVisual::GetResourceUrlList() const
{
  Dali::Vector<Dali::String> result;
  Dali::Property::Value      value = VisualBase::GetProperty(AnimatedImageVisual::Property::URL);
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
  array.Reserve(resourceUrlList.Count());
  for(const auto& resourceUrl : resourceUrlList)
  {
    array.PushBack(resourceUrl);
  }
  VisualBase::SetProperty(AnimatedImageVisual::Property::URL, array);
}

bool AnimatedImageVisual::IsSynchronousLoading() const
{
  return VisualBase::GetProperty<bool>(AnimatedImageVisual::Property::SYNCHRONOUS_LOADING);
}

void AnimatedImageVisual::SetSynchronousLoading(bool synchronous)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::SYNCHRONOUS_LOADING, synchronous);
}

int AnimatedImageVisual::GetDesiredWidth() const
{
  return VisualBase::GetProperty<int>(AnimatedImageVisual::Property::DESIRED_WIDTH);
}

void AnimatedImageVisual::SetDesiredWidth(int desiredWidth)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::DESIRED_WIDTH, desiredWidth);
}

int AnimatedImageVisual::GetDesiredHeight() const
{
  return VisualBase::GetProperty<int>(AnimatedImageVisual::Property::DESIRED_HEIGHT);
}

void AnimatedImageVisual::SetDesiredHeight(int desiredHeight)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::DESIRED_HEIGHT, desiredHeight);
}

Image::SamplingMode AnimatedImageVisual::GetSamplingMode() const
{
  return VisualBase::GetProperty<Image::SamplingMode>(AnimatedImageVisual::Property::SAMPLING_MODE);
}

void AnimatedImageVisual::SetSamplingMode(Image::SamplingMode samplingMode)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::SAMPLING_MODE, samplingMode);
}

Dali::Vector4 AnimatedImageVisual::GetPixelArea() const
{
  return VisualBase::GetProperty<Dali::Vector4>(AnimatedImageVisual::Property::PIXEL_AREA);
}

void AnimatedImageVisual::SetPixelArea(const Dali::Vector4& pixelArea)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::PIXEL_AREA, pixelArea);
}

Dali::WrapMode::Type AnimatedImageVisual::GetWrapModeU() const
{
  return VisualBase::GetProperty<Dali::WrapMode::Type>(AnimatedImageVisual::Property::WRAP_MODE_U);
}

void AnimatedImageVisual::SetWrapModeU(Dali::WrapMode::Type wrapModeU)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::WRAP_MODE_U, wrapModeU);
}

Dali::WrapMode::Type AnimatedImageVisual::GetWrapModeV() const
{
  return VisualBase::GetProperty<Dali::WrapMode::Type>(AnimatedImageVisual::Property::WRAP_MODE_V);
}

void AnimatedImageVisual::SetWrapModeV(Dali::WrapMode::Type wrapModeV)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::WRAP_MODE_V, wrapModeV);
}

bool AnimatedImageVisual::IsBrokenImageEnabled() const
{
  return VisualBase::GetProperty<bool>(AnimatedImageVisual::Property::ENABLE_BROKEN_IMAGE);
}

void AnimatedImageVisual::SetBrokenImageEnabled(bool brokenImageEnabled)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::ENABLE_BROKEN_IMAGE, brokenImageEnabled);
}

Image::LoadPolicy AnimatedImageVisual::GetLoadPolicy() const
{
  return VisualBase::GetProperty<Image::LoadPolicy>(AnimatedImageVisual::Property::LOAD_POLICY);
}

void AnimatedImageVisual::SetLoadPolicy(Image::LoadPolicy loadPolicy)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::LOAD_POLICY, loadPolicy);
}

Image::ReleasePolicy AnimatedImageVisual::GetReleasePolicy() const
{
  return VisualBase::GetProperty<Image::ReleasePolicy>(AnimatedImageVisual::Property::RELEASE_POLICY);
}

void AnimatedImageVisual::SetReleasePolicy(Image::ReleasePolicy releasePolicy)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::RELEASE_POLICY, releasePolicy);
}

Image::FittingMode AnimatedImageVisual::GetFittingMode() const
{
  return VisualBase::GetProperty<Image::FittingMode>(AnimatedImageVisual::Property::FITTING_MODE);
}

void AnimatedImageVisual::SetFittingMode(Image::FittingMode fittingMode)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::FITTING_MODE, fittingMode);
}

bool AnimatedImageVisual::IsOrientationCorrection() const
{
  return VisualBase::GetProperty<bool>(AnimatedImageVisual::Property::ORIENTATION_CORRECTION);
}

void AnimatedImageVisual::SetOrientationCorrection(bool orientationCorrection)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::ORIENTATION_CORRECTION, orientationCorrection);
}

bool AnimatedImageVisual::IsSynchronousSizing() const
{
  return VisualBase::GetProperty<bool>(AnimatedImageVisual::Property::SYNCHRONOUS_SIZING);
}

void AnimatedImageVisual::SetSynchronousSizing(bool synchronousSizing)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::SYNCHRONOUS_SIZING, synchronousSizing);
}

bool AnimatedImageVisual::IsPreMultipliedAlpha() const
{
  return VisualBase::GetProperty<bool>(AnimatedImageVisual::Property::PRE_MULTIPLIED_ALPHA);
}

void AnimatedImageVisual::SetPreMultipliedAlpha(bool preMultiplied)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::PRE_MULTIPLIED_ALPHA, preMultiplied);
}

Dali::String AnimatedImageVisual::GetAlphaMaskUrl() const
{
  return VisualBase::GetProperty<Dali::String>(AnimatedImageVisual::Property::ALPHA_MASK_URL);
}

void AnimatedImageVisual::SetAlphaMaskUrl(const Dali::String& alphaMaskUrl)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::ALPHA_MASK_URL, alphaMaskUrl);
}

float AnimatedImageVisual::GetMaskContentScale() const
{
  return VisualBase::GetProperty<float>(AnimatedImageVisual::Property::MASK_CONTENT_SCALE);
}

void AnimatedImageVisual::SetMaskContentScale(float maskContentScale)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::MASK_CONTENT_SCALE, maskContentScale);
}

bool AnimatedImageVisual::IsCropToMask() const
{
  return VisualBase::GetProperty<bool>(AnimatedImageVisual::Property::CROP_TO_MASK);
}

void AnimatedImageVisual::SetCropToMask(bool cropToMask)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::CROP_TO_MASK, cropToMask);
}

Image::MaskingType AnimatedImageVisual::GetMaskingType() const
{
  return VisualBase::GetProperty<Image::MaskingType>(AnimatedImageVisual::Property::MASKING_TYPE);
}

void AnimatedImageVisual::SetMaskingType(Image::MaskingType maskingType)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::MASKING_TYPE, maskingType);
}

int AnimatedImageVisual::GetLoopCount() const
{
  return VisualBase::GetProperty<int>(AnimatedImageVisual::Property::LOOP_COUNT);
}

void AnimatedImageVisual::SetLoopCount(int loopCount)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::LOOP_COUNT, loopCount);
}

Dali::Property::Array AnimatedImageVisual::GetPlayRange() const
{
  Dali::Property::Value  value    = VisualBase::GetProperty(AnimatedImageVisual::Property::PLAY_RANGE);
  Dali::Property::Array* arrayPtr = value.GetArray();
  if(arrayPtr)
  {
    return *arrayPtr;
  }
  return Dali::Property::Array();
}

void AnimatedImageVisual::SetPlayRange(const Dali::Property::Array& playRange)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::PLAY_RANGE, playRange);
}

AnimatedImage::StopBehavior AnimatedImageVisual::GetStopBehavior() const
{
  return VisualBase::GetProperty<AnimatedImage::StopBehavior>(AnimatedImageVisual::Property::STOP_BEHAVIOR);
}

void AnimatedImageVisual::SetStopBehavior(AnimatedImage::StopBehavior stopBehavior)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::STOP_BEHAVIOR, stopBehavior);
}

float AnimatedImageVisual::GetFrameSpeedFactor() const
{
  return VisualBase::GetProperty<float>(AnimatedImageVisual::Property::FRAME_SPEED_FACTOR);
}

void AnimatedImageVisual::SetFrameSpeedFactor(float frameSpeedFactor)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::FRAME_SPEED_FACTOR, frameSpeedFactor);
}

int AnimatedImageVisual::GetBatchSize() const
{
  return VisualBase::GetProperty<int>(AnimatedImageVisual::Property::BATCH_SIZE);
}

void AnimatedImageVisual::SetBatchSize(int batchSize)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::BATCH_SIZE, batchSize);
}

int AnimatedImageVisual::GetCacheSize() const
{
  return VisualBase::GetProperty<int>(AnimatedImageVisual::Property::CACHE_SIZE);
}

void AnimatedImageVisual::SetCacheSize(int cacheSize)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::CACHE_SIZE, cacheSize);
}

int AnimatedImageVisual::GetFrameDelay() const
{
  return VisualBase::GetProperty<int>(AnimatedImageVisual::Property::FRAME_DELAY);
}

void AnimatedImageVisual::SetFrameDelay(int frameDelay)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::FRAME_DELAY, frameDelay);
}

// =============================================================================
// Read-only properties
// =============================================================================

AnimatedImage::PlayState AnimatedImageVisual::GetPlayState() const
{
  AnimatedImage::PlayState result = VisualBase::GetProperty<AnimatedImage::PlayState>(AnimatedImageVisual::Property::PLAY_STATE);
  // We should not cache this property. Remove cache now.
  // TODO : Maybe need to make API to get proeprty without cache as method.
  GetImplementation(*this).RemoveCache(AnimatedImageVisual::Property::PLAY_STATE);
  return result;
}

int AnimatedImageVisual::GetCurrentFrameNumber() const
{
  int result = VisualBase::GetProperty<int>(AnimatedImageVisual::Property::CURRENT_FRAME_NUMBER);
  // We should not cache this property. Remove cache now.
  // TODO : Maybe need to make API to get proeprty without cache as method.
  GetImplementation(*this).RemoveCache(AnimatedImageVisual::Property::CURRENT_FRAME_NUMBER);
  return result;
}

int AnimatedImageVisual::GetTotalFrameNumber() const
{
  int result = VisualBase::GetProperty<int>(AnimatedImageVisual::Property::TOTAL_FRAME_NUMBER);
  if(result == -1)
  {
    // We should not cache this property only if result is -1. Remove cache now.
    GetImplementation(*this).RemoveCache(AnimatedImageVisual::Property::TOTAL_FRAME_NUMBER);
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
  VisualBase::DoAction(Ui::Integration::AnimatedImageVisual::Action::PLAY, Dali::Property::Value());
}

void AnimatedImageVisual::Pause()
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  VisualBase::DoAction(Ui::Integration::AnimatedImageVisual::Action::PAUSE, Dali::Property::Value());
}

void AnimatedImageVisual::Stop()
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  VisualBase::DoAction(Ui::Integration::AnimatedImageVisual::Action::STOP, Dali::Property::Value());
}

void AnimatedImageVisual::JumpTo(int frame)
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  VisualBase::DoAction(Ui::Integration::AnimatedImageVisual::Action::JUMP_TO, static_cast<int32_t>(frame));
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
