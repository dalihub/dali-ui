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

#include <dali-ui-foundation/integration-api/lottie-animation-view-impl.h>
#include <dali-ui-foundation/public-api/lottie-animation-view.h>

namespace Dali
{
namespace Ui
{

inline Integration::LottieAnimationViewImpl& GetImpl(LottieAnimationView& lottieAnimationView)
{
  DALI_ASSERT_ALWAYS(lottieAnimationView);
  return static_cast<Integration::LottieAnimationViewImpl&>(lottieAnimationView.GetImplementation());
}

inline const Integration::LottieAnimationViewImpl& GetImpl(const LottieAnimationView& lottieAnimationView)
{
  DALI_ASSERT_ALWAYS(lottieAnimationView);
  return static_cast<const Integration::LottieAnimationViewImpl&>(lottieAnimationView.GetImplementation());
}

LottieAnimationView::LottieAnimationView()                                              = default;
LottieAnimationView::~LottieAnimationView()                                             = default;
LottieAnimationView::LottieAnimationView(const LottieAnimationView& rhs)                = default;
LottieAnimationView::LottieAnimationView(LottieAnimationView&& rhs) noexcept            = default;
LottieAnimationView& LottieAnimationView::operator=(const LottieAnimationView& rhs)     = default;
LottieAnimationView& LottieAnimationView::operator=(LottieAnimationView&& rhs) noexcept = default;

LottieAnimationView LottieAnimationView::New(const Dali::String& url)
{
  Integration::LottieAnimationViewImplPtr impl = Integration::LottieAnimationViewImpl::New();
  LottieAnimationView                     view(*impl);
  impl->Initialize();
  if(!url.Empty())
  {
    view.SetResourceUrl(url);
  }
  return view;
}

LottieAnimationView LottieAnimationView::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<LottieAnimationView, Ui::Integration::LottieAnimationViewImpl>(handle);
}

void LottieAnimationView::SetResourceUrl(const Dali::String& url)
{
  Ui::GetImpl(*this).SetResourceUrl(url);
}

Dali::String LottieAnimationView::GetResourceUrl() const
{
  return Ui::GetImpl(*this).GetResourceUrl();
}

void LottieAnimationView::Play()
{
  Ui::GetImpl(*this).Play();
}

void LottieAnimationView::Pause()
{
  Ui::GetImpl(*this).Pause();
}

void LottieAnimationView::Stop()
{
  Ui::GetImpl(*this).Stop();
}

void LottieAnimationView::SetLoopCount(int count)
{
  Ui::GetImpl(*this).SetLoopCount(count);
}

int LottieAnimationView::GetLoopCount() const
{
  return Ui::GetImpl(*this).GetLoopCount();
}

void LottieAnimationView::JumpToFrame(int frame)
{
  Ui::GetImpl(*this).JumpToFrame(frame);
}

void LottieAnimationView::SetMinMaxFrame(int minFrame, int maxFrame)
{
  Ui::GetImpl(*this).SetMinMaxFrame(minFrame, maxFrame);
}

void LottieAnimationView::SetMinMaxFrameByMarker(const Dali::String& minMarker, const Dali::String& maxMarker)
{
  Ui::GetImpl(*this).SetMinMaxFrameByMarker(minMarker, maxMarker);
}

void LottieAnimationView::SetStopBehavior(AnimatedImage::StopBehavior behavior)
{
  Ui::GetImpl(*this).SetStopBehavior(behavior);
}

AnimatedImage::StopBehavior LottieAnimationView::GetStopBehavior() const
{
  return Ui::GetImpl(*this).GetStopBehavior();
}

void LottieAnimationView::SetLoopingMode(LottieAnimation::LoopingMode mode)
{
  Ui::GetImpl(*this).SetLoopingMode(mode);
}

LottieAnimation::LoopingMode LottieAnimationView::GetLoopingMode() const
{
  return Ui::GetImpl(*this).GetLoopingMode();
}

void LottieAnimationView::SetFrameSpeedFactor(float factor)
{
  Ui::GetImpl(*this).SetFrameSpeedFactor(factor);
}

float LottieAnimationView::GetFrameSpeedFactor() const
{
  return Ui::GetImpl(*this).GetFrameSpeedFactor();
}

AnimatedImage::PlayState LottieAnimationView::GetPlayState() const
{
  return Ui::GetImpl(*this).GetPlayState();
}

int LottieAnimationView::GetCurrentFrame() const
{
  return Ui::GetImpl(*this).GetCurrentFrame();
}

int LottieAnimationView::GetTotalFrame() const
{
  return Ui::GetImpl(*this).GetTotalFrame();
}

void LottieAnimationView::SetRedrawOnScaleDown(bool redraw)
{
  Ui::GetImpl(*this).SetRedrawOnScaleDown(redraw);
}

bool LottieAnimationView::IsRedrawOnScaleDown() const
{
  return Ui::GetImpl(*this).IsRedrawOnScaleDown();
}

void LottieAnimationView::SetRedrawOnScaleUp(bool redraw)
{
  Ui::GetImpl(*this).SetRedrawOnScaleUp(redraw);
}

bool LottieAnimationView::IsRedrawOnScaleUp() const
{
  return Ui::GetImpl(*this).IsRedrawOnScaleUp();
}

void LottieAnimationView::SetEnableFrameCache(bool enable)
{
  Ui::GetImpl(*this).SetEnableFrameCache(enable);
}

bool LottieAnimationView::IsFrameCacheEnabled() const
{
  return Ui::GetImpl(*this).IsFrameCacheEnabled();
}

void LottieAnimationView::SetNotifyAfterRasterization(bool notify)
{
  Ui::GetImpl(*this).SetNotifyAfterRasterization(notify);
}

bool LottieAnimationView::IsNotifyAfterRasterizationEnabled() const
{
  return Ui::GetImpl(*this).IsNotifyAfterRasterizationEnabled();
}

void LottieAnimationView::SetRenderScale(float scale)
{
  Ui::GetImpl(*this).SetRenderScale(scale);
}

float LottieAnimationView::GetRenderScale() const
{
  return Ui::GetImpl(*this).GetRenderScale();
}

void LottieAnimationView::SetEnableAspectFit(bool enableAspectFit)
{
  Ui::GetImpl(*this).SetEnableAspectFit(enableAspectFit);
}
bool LottieAnimationView::IsAspectFitEnabled() const
{
  return Ui::GetImpl(*this).IsAspectFitEnabled();
}

Dali::Property::Map LottieAnimationView::GetContentInfo()
{
  return Ui::GetImpl(*this).GetContentInfo();
}

Dali::Property::Map LottieAnimationView::GetMarkerInfo()
{
  return Ui::GetImpl(*this).GetMarkerInfo();
}

void LottieAnimationView::SetDynamicProperty(const LottieAnimation::DynamicPropertyInfo& info)
{
  Ui::GetImpl(*this).SetDynamicProperty(info);
}

void LottieAnimationView::SetDesiredWidth(int width)
{
  Ui::GetImpl(*this).SetDesiredWidth(width);
}

int LottieAnimationView::GetDesiredWidth() const
{
  return Ui::GetImpl(*this).GetDesiredWidth();
}

void LottieAnimationView::SetDesiredHeight(int height)
{
  Ui::GetImpl(*this).SetDesiredHeight(height);
}

int LottieAnimationView::GetDesiredHeight() const
{
  return Ui::GetImpl(*this).GetDesiredHeight();
}

void LottieAnimationView::SetReleasePolicy(Ui::Image::ReleasePolicy releasePolicy)
{
  Ui::GetImpl(*this).SetReleasePolicy(releasePolicy);
}

Ui::Image::ReleasePolicy LottieAnimationView::GetReleasePolicy() const
{
  return Ui::GetImpl(*this).GetReleasePolicy();
}

void LottieAnimationView::SetSynchronousLoading(bool synchronous)
{
  Ui::GetImpl(*this).SetSynchronousLoading(synchronous);
}

bool LottieAnimationView::IsSynchronousLoading() const
{
  return Ui::GetImpl(*this).IsSynchronousLoading();
}

void LottieAnimationView::SetImageColor(const UiColor& color)
{
  Ui::GetImpl(*this).SetImageColor(color);
}

UiColor LottieAnimationView::GetImageColor() const
{
  return Ui::GetImpl(*this).GetImageColor();
}

void LottieAnimationView::SetPlaceholderUrl(const Dali::String& url)
{
  Ui::GetImpl(*this).SetPlaceholderUrl(url);
}

Dali::String LottieAnimationView::GetPlaceholderUrl() const
{
  return Ui::GetImpl(*this).GetPlaceholderUrl();
}

void LottieAnimationView::SetPixelArea(const Dali::Vector4& pixelArea)
{
  Ui::GetImpl(*this).SetPixelArea(pixelArea);
}

Dali::Vector4 LottieAnimationView::GetPixelArea() const
{
  return Ui::GetImpl(*this).GetPixelArea();
}

View::ResourceReadySignalType& LottieAnimationView::ResourceReadySignal()
{
  return View::ResourceReadySignal();
}

LottieAnimationView::AnimationFinishedSignalType& LottieAnimationView::AnimationFinishedSignal()
{
  return Ui::GetImpl(*this).AnimationFinishedSignal();
}

Ui::Visual::ResourceStatus LottieAnimationView::GetLoadingStatus() const
{
  return Ui::GetImpl(*this).GetLoadingStatus();
}

LottieAnimationView::LottieAnimationView(Integration::LottieAnimationViewImpl& implementation)
: View(implementation)
{
}

LottieAnimationView::LottieAnimationView(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Integration::LottieAnimationViewImpl>(internal);
}

} // namespace Ui
} // namespace Dali
