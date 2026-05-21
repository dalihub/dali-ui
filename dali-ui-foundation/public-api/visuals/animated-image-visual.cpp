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
#include <dali-ui-foundation/devel-api/visuals/animated-image-visual-actions-devel.h>
#include <dali-ui-foundation/devel-api/visuals/visual-base-impl.h>

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

AnimatedImageVisual& AnimatedImageVisual::SetResourceUrl(const Dali::String& resourceUrl)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::URL, resourceUrl);
  return *this;
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

AnimatedImageVisual& AnimatedImageVisual::SetResourceUrlList(const Dali::Vector<Dali::String>& resourceUrlList)
{
  Dali::Property::Array array;
  array.Reserve(resourceUrlList.Count());
  for(const auto& resourceUrl : resourceUrlList)
  {
    array.PushBack(resourceUrl);
  }
  VisualBase::SetProperty(AnimatedImageVisual::Property::URL, array);
  return *this;
}

bool AnimatedImageVisual::IsSynchronousLoading() const
{
  return VisualBase::GetProperty<bool>(AnimatedImageVisual::Property::SYNCHRONOUS_LOADING);
}

AnimatedImageVisual& AnimatedImageVisual::SetSynchronousLoading(bool synchronous)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::SYNCHRONOUS_LOADING, synchronous);
  return *this;
}

int AnimatedImageVisual::GetDesiredWidth() const
{
  return VisualBase::GetProperty<int>(AnimatedImageVisual::Property::DESIRED_WIDTH);
}

AnimatedImageVisual& AnimatedImageVisual::SetDesiredWidth(int desiredWidth)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::DESIRED_WIDTH, desiredWidth);
  return *this;
}

int AnimatedImageVisual::GetDesiredHeight() const
{
  return VisualBase::GetProperty<int>(AnimatedImageVisual::Property::DESIRED_HEIGHT);
}

AnimatedImageVisual& AnimatedImageVisual::SetDesiredHeight(int desiredHeight)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::DESIRED_HEIGHT, desiredHeight);
  return *this;
}

Image::SamplingMode AnimatedImageVisual::GetSamplingMode() const
{
  return VisualBase::GetProperty<Image::SamplingMode>(AnimatedImageVisual::Property::SAMPLING_MODE);
}

AnimatedImageVisual& AnimatedImageVisual::SetSamplingMode(Image::SamplingMode samplingMode)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::SAMPLING_MODE, samplingMode);
  return *this;
}

Dali::Vector4 AnimatedImageVisual::GetPixelArea() const
{
  return VisualBase::GetProperty<Dali::Vector4>(AnimatedImageVisual::Property::PIXEL_AREA);
}

AnimatedImageVisual& AnimatedImageVisual::SetPixelArea(const Dali::Vector4& pixelArea)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::PIXEL_AREA, pixelArea);
  return *this;
}

Dali::WrapMode::Type AnimatedImageVisual::GetWrapModeU() const
{
  return VisualBase::GetProperty<Dali::WrapMode::Type>(AnimatedImageVisual::Property::WRAP_MODE_U);
}

AnimatedImageVisual& AnimatedImageVisual::SetWrapModeU(Dali::WrapMode::Type wrapModeU)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::WRAP_MODE_U, wrapModeU);
  return *this;
}

Dali::WrapMode::Type AnimatedImageVisual::GetWrapModeV() const
{
  return VisualBase::GetProperty<Dali::WrapMode::Type>(AnimatedImageVisual::Property::WRAP_MODE_V);
}

AnimatedImageVisual& AnimatedImageVisual::SetWrapModeV(Dali::WrapMode::Type wrapModeV)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::WRAP_MODE_V, wrapModeV);
  return *this;
}

bool AnimatedImageVisual::IsBrokenImageEnabled() const
{
  return VisualBase::GetProperty<bool>(AnimatedImageVisual::Property::ENABLE_BROKEN_IMAGE);
}

AnimatedImageVisual& AnimatedImageVisual::SetEnableBrokenImage(bool enableBrokenImage)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::ENABLE_BROKEN_IMAGE, enableBrokenImage);
  return *this;
}

Image::LoadPolicy AnimatedImageVisual::GetLoadPolicy() const
{
  return VisualBase::GetProperty<Image::LoadPolicy>(AnimatedImageVisual::Property::LOAD_POLICY);
}

AnimatedImageVisual& AnimatedImageVisual::SetLoadPolicy(Image::LoadPolicy loadPolicy)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::LOAD_POLICY, loadPolicy);
  return *this;
}

Image::ReleasePolicy AnimatedImageVisual::GetReleasePolicy() const
{
  return VisualBase::GetProperty<Image::ReleasePolicy>(AnimatedImageVisual::Property::RELEASE_POLICY);
}

AnimatedImageVisual& AnimatedImageVisual::SetReleasePolicy(Image::ReleasePolicy releasePolicy)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::RELEASE_POLICY, releasePolicy);
  return *this;
}

Image::FittingMode AnimatedImageVisual::GetFittingMode() const
{
  return VisualBase::GetProperty<Image::FittingMode>(AnimatedImageVisual::Property::FITTING_MODE);
}

AnimatedImageVisual& AnimatedImageVisual::SetFittingMode(Image::FittingMode fittingMode)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::FITTING_MODE, fittingMode);
  return *this;
}

bool AnimatedImageVisual::IsOrientationCorrection() const
{
  return VisualBase::GetProperty<bool>(AnimatedImageVisual::Property::ORIENTATION_CORRECTION);
}

AnimatedImageVisual& AnimatedImageVisual::SetOrientationCorrection(bool orientationCorrection)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::ORIENTATION_CORRECTION, orientationCorrection);
  return *this;
}

bool AnimatedImageVisual::IsSynchronousSizing() const
{
  return VisualBase::GetProperty<bool>(AnimatedImageVisual::Property::SYNCHRONOUS_SIZING);
}

AnimatedImageVisual& AnimatedImageVisual::SetSynchronousSizing(bool synchronousSizing)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::SYNCHRONOUS_SIZING, synchronousSizing);
  return *this;
}

bool AnimatedImageVisual::IsPreMultipliedAlpha() const
{
  return VisualBase::GetProperty<bool>(AnimatedImageVisual::Property::PRE_MULTIPLIED_ALPHA);
}

AnimatedImageVisual& AnimatedImageVisual::SetPreMultipliedAlpha(bool preMultiplied)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::PRE_MULTIPLIED_ALPHA, preMultiplied);
  return *this;
}

Dali::String AnimatedImageVisual::GetAlphaMaskUrl() const
{
  return VisualBase::GetProperty<Dali::String>(AnimatedImageVisual::Property::ALPHA_MASK_URL);
}

AnimatedImageVisual& AnimatedImageVisual::SetAlphaMaskUrl(const Dali::String& alphaMaskUrl)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::ALPHA_MASK_URL, alphaMaskUrl);
  return *this;
}

float AnimatedImageVisual::GetMaskContentScale() const
{
  return VisualBase::GetProperty<float>(AnimatedImageVisual::Property::MASK_CONTENT_SCALE);
}

AnimatedImageVisual& AnimatedImageVisual::SetMaskContentScale(float maskContentScale)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::MASK_CONTENT_SCALE, maskContentScale);
  return *this;
}

bool AnimatedImageVisual::IsCropToMask() const
{
  return VisualBase::GetProperty<bool>(AnimatedImageVisual::Property::CROP_TO_MASK);
}

AnimatedImageVisual& AnimatedImageVisual::SetCropToMask(bool cropToMask)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::CROP_TO_MASK, cropToMask);
  return *this;
}

Image::MaskingType AnimatedImageVisual::GetMaskingType() const
{
  return VisualBase::GetProperty<Image::MaskingType>(AnimatedImageVisual::Property::MASKING_TYPE);
}

AnimatedImageVisual& AnimatedImageVisual::SetMaskingType(Image::MaskingType maskingType)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::MASKING_TYPE, maskingType);
  return *this;
}

int AnimatedImageVisual::GetLoopCount() const
{
  return VisualBase::GetProperty<int>(AnimatedImageVisual::Property::LOOP_COUNT);
}

AnimatedImageVisual& AnimatedImageVisual::SetLoopCount(int loopCount)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::LOOP_COUNT, loopCount);
  return *this;
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

AnimatedImageVisual& AnimatedImageVisual::SetPlayRange(const Dali::Property::Array& playRange)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::PLAY_RANGE, playRange);
  return *this;
}

AnimatedImage::StopBehavior AnimatedImageVisual::GetStopBehavior() const
{
  return VisualBase::GetProperty<AnimatedImage::StopBehavior>(AnimatedImageVisual::Property::STOP_BEHAVIOR);
}

AnimatedImageVisual& AnimatedImageVisual::SetStopBehavior(AnimatedImage::StopBehavior stopBehavior)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::STOP_BEHAVIOR, stopBehavior);
  return *this;
}

float AnimatedImageVisual::GetFrameSpeedFactor() const
{
  return VisualBase::GetProperty<float>(AnimatedImageVisual::Property::FRAME_SPEED_FACTOR);
}

AnimatedImageVisual& AnimatedImageVisual::SetFrameSpeedFactor(float frameSpeedFactor)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::FRAME_SPEED_FACTOR, frameSpeedFactor);
  return *this;
}

int AnimatedImageVisual::GetBatchSize() const
{
  return VisualBase::GetProperty<int>(AnimatedImageVisual::Property::BATCH_SIZE);
}

AnimatedImageVisual& AnimatedImageVisual::SetBatchSize(int batchSize)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::BATCH_SIZE, batchSize);
  return *this;
}

int AnimatedImageVisual::GetCacheSize() const
{
  return VisualBase::GetProperty<int>(AnimatedImageVisual::Property::CACHE_SIZE);
}

AnimatedImageVisual& AnimatedImageVisual::SetCacheSize(int cacheSize)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::CACHE_SIZE, cacheSize);
  return *this;
}

int AnimatedImageVisual::GetFrameDelay() const
{
  return VisualBase::GetProperty<int>(AnimatedImageVisual::Property::FRAME_DELAY);
}

AnimatedImageVisual& AnimatedImageVisual::SetFrameDelay(int frameDelay)
{
  VisualBase::SetProperty(AnimatedImageVisual::Property::FRAME_DELAY, frameDelay);
  return *this;
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

AnimatedImageVisual& AnimatedImageVisual::Play()
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  VisualBase::DoAction(Ui::DevelAnimatedImageVisual::Action::PLAY, Dali::Property::Value());
  return *this;
}

AnimatedImageVisual& AnimatedImageVisual::Pause()
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  VisualBase::DoAction(Ui::DevelAnimatedImageVisual::Action::PAUSE, Dali::Property::Value());
  return *this;
}

AnimatedImageVisual& AnimatedImageVisual::Stop()
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  VisualBase::DoAction(Ui::DevelAnimatedImageVisual::Action::STOP, Dali::Property::Value());
  return *this;
}

AnimatedImageVisual& AnimatedImageVisual::JumpTo(int frame)
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  VisualBase::DoAction(Ui::DevelAnimatedImageVisual::Action::JUMP_TO, static_cast<int32_t>(frame));
  return *this;
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
