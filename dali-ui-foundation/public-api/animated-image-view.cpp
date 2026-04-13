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

  // Second-phase initialization
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

AnimatedImageView& AnimatedImageView::SetResourceUrl(const Dali::String& url)
{
  Ui::GetImpl(*this).SetResourceUrl(url);
  return *this;
}

Dali::String AnimatedImageView::GetResourceUrl() const
{
  return Ui::GetImpl(*this).GetResourceUrl();
}

AnimatedImageView& AnimatedImageView::Play()
{
  Ui::GetImpl(*this).Play();
  return *this;
}

AnimatedImageView& AnimatedImageView::Pause()
{
  Ui::GetImpl(*this).Pause();
  return *this;
}

AnimatedImageView& AnimatedImageView::Stop()
{
  Ui::GetImpl(*this).Stop();
  return *this;
}

AnimatedImageView& AnimatedImageView::SetLoopCount(int count)
{
  Ui::GetImpl(*this).SetLoopCount(count);
  return *this;
}

int AnimatedImageView::GetLoopCount() const
{
  return Ui::GetImpl(*this).GetLoopCount();
}

AnimatedImageView& AnimatedImageView::SetImageColor(const UiColor& color)
{
  Ui::GetImpl(*this).SetImageColor(color);
  return *this;
}

UiColor AnimatedImageView::GetImageColor() const
{
  return Ui::GetImpl(*this).GetImageColor();
}

AnimatedImageView& AnimatedImageView::JumpToFrame(int frame)
{
  Ui::GetImpl(*this).JumpToFrame(frame);
  return *this;
}

AnimatedImageView& AnimatedImageView::SetStopBehavior(StopBehavior::Type behavior)
{
  Ui::GetImpl(*this).SetStopBehavior(behavior);
  return *this;
}

AnimatedImageView::StopBehavior::Type AnimatedImageView::GetStopBehavior() const
{
  return Ui::GetImpl(*this).GetStopBehavior();
}

AnimatedImageView& AnimatedImageView::SetFrameSpeedFactor(float factor)
{
  Ui::GetImpl(*this).SetFrameSpeedFactor(factor);
  return *this;
}

float AnimatedImageView::GetFrameSpeedFactor() const
{
  return Ui::GetImpl(*this).GetFrameSpeedFactor();
}

AnimatedImageView& AnimatedImageView::SetBatchSize(int size)
{
  Ui::GetImpl(*this).SetBatchSize(size);
  return *this;
}

int AnimatedImageView::GetBatchSize() const
{
  return Ui::GetImpl(*this).GetBatchSize();
}

AnimatedImageView& AnimatedImageView::SetCacheSize(int size)
{
  Ui::GetImpl(*this).SetCacheSize(size);
  return *this;
}

int AnimatedImageView::GetCacheSize() const
{
  return Ui::GetImpl(*this).GetCacheSize();
}

AnimatedImageView& AnimatedImageView::SetFrameDelay(int milliseconds)
{
  Ui::GetImpl(*this).SetFrameDelay(milliseconds);
  return *this;
}

int AnimatedImageView::GetFrameDelay() const
{
  return Ui::GetImpl(*this).GetFrameDelay();
}

AnimatedImageView::PlayState::Type AnimatedImageView::GetPlayState() const
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

AnimatedImageView& AnimatedImageView::SetDesiredWidth(int width)
{
  Ui::GetImpl(*this).SetDesiredWidth(width);
  return *this;
}

int AnimatedImageView::GetDesiredWidth() const
{
  return Ui::GetImpl(*this).GetDesiredWidth();
}

AnimatedImageView& AnimatedImageView::SetDesiredHeight(int height)
{
  Ui::GetImpl(*this).SetDesiredHeight(height);
  return *this;
}

int AnimatedImageView::GetDesiredHeight() const
{
  return Ui::GetImpl(*this).GetDesiredHeight();
}

AnimatedImageView& AnimatedImageView::SetLoadPolicy(Ui::LoadPolicy::Type loadPolicy)
{
  Ui::GetImpl(*this).SetLoadPolicy(loadPolicy);
  return *this;
}

Ui::LoadPolicy::Type AnimatedImageView::GetLoadPolicy() const
{
  return Ui::GetImpl(*this).GetLoadPolicy();
}

AnimatedImageView& AnimatedImageView::SetReleasePolicy(Ui::ReleasePolicy::Type releasePolicy)
{
  Ui::GetImpl(*this).SetReleasePolicy(releasePolicy);
  return *this;
}

Ui::ReleasePolicy::Type AnimatedImageView::GetReleasePolicy() const
{
  return Ui::GetImpl(*this).GetReleasePolicy();
}

AnimatedImageView& AnimatedImageView::SetSynchronousLoading(bool synchronous)
{
  Ui::GetImpl(*this).SetSynchronousLoading(synchronous);
  return *this;
}

bool AnimatedImageView::IsSynchronousLoading() const
{
  return Ui::GetImpl(*this).IsSynchronousLoading();
}

AnimatedImageView& AnimatedImageView::SetPreMultipliedAlpha(bool preMultiplied)
{
  Ui::GetImpl(*this).SetPreMultipliedAlpha(preMultiplied);
  return *this;
}

bool AnimatedImageView::IsPreMultipliedAlpha() const
{
  return Ui::GetImpl(*this).IsPreMultipliedAlpha();
}

AnimatedImageView& AnimatedImageView::SetFittingMode(Ui::FittingMode::Type fittingMode)
{
  Ui::GetImpl(*this).SetFittingMode(fittingMode);
  return *this;
}

Ui::FittingMode::Type AnimatedImageView::GetFittingMode() const
{
  return Ui::GetImpl(*this).GetFittingMode();
}

AnimatedImageView& AnimatedImageView::SetSamplingMode(Ui::SamplingMode::Type samplingMode)
{
  Ui::GetImpl(*this).SetSamplingMode(samplingMode);
  return *this;
}

Ui::SamplingMode::Type AnimatedImageView::GetSamplingMode() const
{
  return Ui::GetImpl(*this).GetSamplingMode();
}

AnimatedImageView& AnimatedImageView::SetImageLoadWithViewSize(bool enabled)
{
  Ui::GetImpl(*this).SetImageLoadWithViewSize(enabled);
  return *this;
}

bool AnimatedImageView::IsImageLoadWithViewSizeEnabled() const
{
  return Ui::GetImpl(*this).IsImageLoadWithViewSizeEnabled();
}

AnimatedImageView& AnimatedImageView::SetAlphaMaskUrl(const Dali::String& maskUrl)
{
  Ui::GetImpl(*this).SetAlphaMaskUrl(maskUrl);
  return *this;
}

Dali::String AnimatedImageView::GetAlphaMaskUrl() const
{
  return Ui::GetImpl(*this).GetAlphaMaskUrl();
}

AnimatedImageView& AnimatedImageView::SetCropToMask(bool cropToMask)
{
  Ui::GetImpl(*this).SetCropToMask(cropToMask);
  return *this;
}

bool AnimatedImageView::IsCropToMask() const
{
  return Ui::GetImpl(*this).IsCropToMask();
}

AnimatedImageView& AnimatedImageView::SetMaskingMode(Ui::MaskingType::Type maskingMode)
{
  Ui::GetImpl(*this).SetMaskingMode(maskingMode);
  return *this;
}

Ui::MaskingType::Type AnimatedImageView::GetMaskingMode() const
{
  return Ui::GetImpl(*this).GetMaskingMode();
}

AnimatedImageView& AnimatedImageView::SetPlaceholderUrl(const Dali::String& url)
{
  Ui::GetImpl(*this).SetPlaceholderUrl(url);
  return *this;
}

Dali::String AnimatedImageView::GetPlaceholderUrl() const
{
  return Ui::GetImpl(*this).GetPlaceholderUrl();
}

AnimatedImageView& AnimatedImageView::SetResourceUrls(const Dali::Vector<Dali::String>& urls)
{
  Ui::GetImpl(*this).SetResourceUrls(urls);
  return *this;
}

const Dali::Vector<Dali::String>& AnimatedImageView::GetResourceUrls() const
{
  return Ui::GetImpl(*this).GetResourceUrls();
}

AnimatedImageView& AnimatedImageView::SetPixelArea(const Vector4& pixelArea)
{
  Ui::GetImpl(*this).SetPixelArea(pixelArea);
  return *this;
}

Vector4 AnimatedImageView::GetPixelArea() const
{
  return Ui::GetImpl(*this).GetPixelArea();
}

AnimatedImageView& AnimatedImageView::SetDepthIndex(int depthIndex)
{
  Ui::GetImpl(*this).SetDepthIndex(depthIndex);
  return *this;
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
