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

#include <dali-ui-foundation/integration-api/animated-image-view-impl.h>
#include <dali-ui-foundation/public-api/animated-image-view.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{
namespace Ui
{

inline Integration::AnimatedImageViewImpl& GetImpl(AnimatedImageView& animatedImageView)
{
  DALI_ASSERT_ALWAYS(animatedImageView);
  return static_cast<Integration::AnimatedImageViewImpl&>(animatedImageView.GetImplementation());
}

inline const Integration::AnimatedImageViewImpl& GetImpl(const AnimatedImageView& animatedImageView)
{
  DALI_ASSERT_ALWAYS(animatedImageView);
  return static_cast<const Integration::AnimatedImageViewImpl&>(animatedImageView.GetImplementation());
}

AnimatedImageView::AnimatedImageView()                                            = default;
AnimatedImageView::~AnimatedImageView()                                           = default;
AnimatedImageView::AnimatedImageView(const AnimatedImageView& rhs)                = default;
AnimatedImageView::AnimatedImageView(AnimatedImageView&& rhs) noexcept            = default;
AnimatedImageView& AnimatedImageView::operator=(const AnimatedImageView& rhs)     = default;
AnimatedImageView& AnimatedImageView::operator=(AnimatedImageView&& rhs) noexcept = default;
AnimatedImageView  AnimatedImageView::New(const Dali::String& url)
{
  Integration::AnimatedImageViewImplPtr impl = Integration::AnimatedImageViewImpl::New();
  AnimatedImageView                     view(*impl);
  impl->Initialize();
  if(!url.Empty())
  {
    view.SetResourceUrl(url);
  }
  return view;
}

AnimatedImageView AnimatedImageView::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<AnimatedImageView, Ui::Integration::AnimatedImageViewImpl>(handle);
}

void AnimatedImageView::SetResourceUrl(const Dali::String& url)
{
  Ui::GetImpl(*this).SetResourceUrl(url);
}

Dali::String AnimatedImageView::GetResourceUrl() const
{
  return Ui::GetImpl(*this).GetResourceUrl();
}

void AnimatedImageView::Play()
{
  Ui::GetImpl(*this).Play();
}

void AnimatedImageView::Pause()
{
  Ui::GetImpl(*this).Pause();
}

void AnimatedImageView::Stop()
{
  Ui::GetImpl(*this).Stop();
}

void AnimatedImageView::SetLoopCount(int count)
{
  Ui::GetImpl(*this).SetLoopCount(count);
}

int AnimatedImageView::GetLoopCount() const
{
  return Ui::GetImpl(*this).GetLoopCount();
}

void AnimatedImageView::SetImageColor(const UiColor& color)
{
  Ui::GetImpl(*this).SetImageColor(color);
}

UiColor AnimatedImageView::GetImageColor() const
{
  return Ui::GetImpl(*this).GetImageColor();
}

void AnimatedImageView::JumpToFrame(int frame)
{
  Ui::GetImpl(*this).JumpToFrame(frame);
}

void AnimatedImageView::SetStopBehavior(AnimatedImage::StopBehavior behavior)
{
  Ui::GetImpl(*this).SetStopBehavior(behavior);
}

AnimatedImage::StopBehavior AnimatedImageView::GetStopBehavior() const
{
  return Ui::GetImpl(*this).GetStopBehavior();
}

void AnimatedImageView::SetFrameSpeedFactor(float factor)
{
  Ui::GetImpl(*this).SetFrameSpeedFactor(factor);
}

float AnimatedImageView::GetFrameSpeedFactor() const
{
  return Ui::GetImpl(*this).GetFrameSpeedFactor();
}

void AnimatedImageView::SetBatchSize(int size)
{
  Ui::GetImpl(*this).SetBatchSize(size);
}

int AnimatedImageView::GetBatchSize() const
{
  return Ui::GetImpl(*this).GetBatchSize();
}

void AnimatedImageView::SetCacheSize(int size)
{
  Ui::GetImpl(*this).SetCacheSize(size);
}

int AnimatedImageView::GetCacheSize() const
{
  return Ui::GetImpl(*this).GetCacheSize();
}

void AnimatedImageView::SetFrameDelay(int milliseconds)
{
  Ui::GetImpl(*this).SetFrameDelay(milliseconds);
}

int AnimatedImageView::GetFrameDelay() const
{
  return Ui::GetImpl(*this).GetFrameDelay();
}

Ui::AnimatedImage::PlayState AnimatedImageView::GetPlayState() const
{
  return Ui::GetImpl(*this).GetPlayState();
}

int AnimatedImageView::GetCurrentFrame() const
{
  return Ui::GetImpl(*this).GetCurrentFrame();
}

int AnimatedImageView::GetTotalFrame() const
{
  return Ui::GetImpl(*this).GetTotalFrame();
}

void AnimatedImageView::SetDesiredWidth(int width)
{
  Ui::GetImpl(*this).SetDesiredWidth(width);
}

int AnimatedImageView::GetDesiredWidth() const
{
  return Ui::GetImpl(*this).GetDesiredWidth();
}

void AnimatedImageView::SetDesiredHeight(int height)
{
  Ui::GetImpl(*this).SetDesiredHeight(height);
}

int AnimatedImageView::GetDesiredHeight() const
{
  return Ui::GetImpl(*this).GetDesiredHeight();
}

void AnimatedImageView::SetLoadPolicy(Ui::Image::LoadPolicy loadPolicy)
{
  Ui::GetImpl(*this).SetLoadPolicy(loadPolicy);
}

Ui::Image::LoadPolicy AnimatedImageView::GetLoadPolicy() const
{
  return Ui::GetImpl(*this).GetLoadPolicy();
}

void AnimatedImageView::SetReleasePolicy(Ui::Image::ReleasePolicy releasePolicy)
{
  Ui::GetImpl(*this).SetReleasePolicy(releasePolicy);
}

Ui::Image::ReleasePolicy AnimatedImageView::GetReleasePolicy() const
{
  return Ui::GetImpl(*this).GetReleasePolicy();
}

void AnimatedImageView::SetSynchronousLoading(bool synchronous)
{
  Ui::GetImpl(*this).SetSynchronousLoading(synchronous);
}

bool AnimatedImageView::IsSynchronousLoading() const
{
  return Ui::GetImpl(*this).IsSynchronousLoading();
}

void AnimatedImageView::SetPreMultipliedAlpha(bool preMultiplied)
{
  Ui::GetImpl(*this).SetPreMultipliedAlpha(preMultiplied);
}

bool AnimatedImageView::IsPreMultipliedAlpha() const
{
  return Ui::GetImpl(*this).IsPreMultipliedAlpha();
}

void AnimatedImageView::SetFittingMode(Ui::Image::FittingMode fittingMode)
{
  Ui::GetImpl(*this).SetFittingMode(fittingMode);
}

Ui::Image::FittingMode AnimatedImageView::GetFittingMode() const
{
  return Ui::GetImpl(*this).GetFittingMode();
}

void AnimatedImageView::SetSamplingMode(Ui::Image::SamplingMode samplingMode)
{
  Ui::GetImpl(*this).SetSamplingMode(samplingMode);
}

Ui::Image::SamplingMode AnimatedImageView::GetSamplingMode() const
{
  return Ui::GetImpl(*this).GetSamplingMode();
}

void AnimatedImageView::SetImageLoadWithViewSize(bool enabled)
{
  Ui::GetImpl(*this).SetImageLoadWithViewSize(enabled);
}

bool AnimatedImageView::IsImageLoadWithViewSizeEnabled() const
{
  return Ui::GetImpl(*this).IsImageLoadWithViewSizeEnabled();
}

void AnimatedImageView::SetAlphaMaskUrl(const Dali::String& maskUrl)
{
  Ui::GetImpl(*this).SetAlphaMaskUrl(maskUrl);
}

Dali::String AnimatedImageView::GetAlphaMaskUrl() const
{
  return Ui::GetImpl(*this).GetAlphaMaskUrl();
}

void AnimatedImageView::SetCropToMask(bool cropToMask)
{
  Ui::GetImpl(*this).SetCropToMask(cropToMask);
}

bool AnimatedImageView::IsCropToMask() const
{
  return Ui::GetImpl(*this).IsCropToMask();
}

void AnimatedImageView::SetMaskingMode(Ui::Image::MaskingType maskingMode)
{
  Ui::GetImpl(*this).SetMaskingMode(maskingMode);
}

Ui::Image::MaskingType AnimatedImageView::GetMaskingMode() const
{
  return Ui::GetImpl(*this).GetMaskingMode();
}

void AnimatedImageView::SetPlaceholderUrl(const Dali::String& url)
{
  Ui::GetImpl(*this).SetPlaceholderUrl(url);
}

Dali::String AnimatedImageView::GetPlaceholderUrl() const
{
  return Ui::GetImpl(*this).GetPlaceholderUrl();
}

void AnimatedImageView::SetResourceUrls(const Dali::Vector<Dali::String>& urls)
{
  Ui::GetImpl(*this).SetResourceUrls(urls);
}

const Dali::Vector<Dali::String>& AnimatedImageView::GetResourceUrls() const
{
  return Ui::GetImpl(*this).GetResourceUrls();
}

void AnimatedImageView::SetPixelArea(const Vector4& pixelArea)
{
  Ui::GetImpl(*this).SetPixelArea(pixelArea);
}

Vector4 AnimatedImageView::GetPixelArea() const
{
  return Ui::GetImpl(*this).GetPixelArea();
}

Ui::Visual::ResourceStatus AnimatedImageView::GetLoadingStatus() const
{
  return Ui::GetImpl(*this).GetLoadingStatus();
}

View::ResourceReadySignalType& AnimatedImageView::ResourceReadySignal()
{
  return View::ResourceReadySignal();
}

AnimatedImageView::AnimationFinishedSignalType& AnimatedImageView::AnimationFinishedSignal()
{
  return Ui::GetImpl(*this).AnimationFinishedSignal();
}

AnimatedImageView::AnimatedImageView(Integration::AnimatedImageViewImpl& implementation)
: View(implementation)
{
}

AnimatedImageView::AnimatedImageView(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Integration::AnimatedImageViewImpl>(internal);
}
} //namespace Ui
} //namespace Dali
