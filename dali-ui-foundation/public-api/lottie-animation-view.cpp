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

  // Second-phase initialization
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

LottieAnimationView& LottieAnimationView::SetResourceUrl(const Dali::String& url)
{
  Ui::GetImpl(*this).SetResourceUrl(url);
  return *this;
}

Dali::String LottieAnimationView::GetResourceUrl() const
{
  return Ui::GetImpl(*this).GetResourceUrl();
}

LottieAnimationView& LottieAnimationView::Play()
{
  Ui::GetImpl(*this).Play();
  return *this;
}

LottieAnimationView& LottieAnimationView::Pause()
{
  Ui::GetImpl(*this).Pause();
  return *this;
}

LottieAnimationView& LottieAnimationView::Stop()
{
  Ui::GetImpl(*this).Stop();
  return *this;
}

LottieAnimationView& LottieAnimationView::SetLoopCount(int count)
{
  Ui::GetImpl(*this).SetLoopCount(count);
  return *this;
}

int LottieAnimationView::GetLoopCount() const
{
  return Ui::GetImpl(*this).GetLoopCount();
}

LottieAnimationView& LottieAnimationView::JumpToFrame(int frame)
{
  Ui::GetImpl(*this).JumpToFrame(frame);
  return *this;
}

LottieAnimationView& LottieAnimationView::SetMinMaxFrame(int minFrame, int maxFrame)
{
  Ui::GetImpl(*this).SetMinMaxFrame(minFrame, maxFrame);
  return *this;
}

LottieAnimationView& LottieAnimationView::SetMinMaxFrameByMarker(const Dali::String& minMarker, const Dali::String& maxMarker)
{
  Ui::GetImpl(*this).SetMinMaxFrameByMarker(minMarker, maxMarker);
  return *this;
}

LottieAnimationView& LottieAnimationView::SetStopBehavior(AnimatedImage::StopBehavior behavior)
{
  Ui::GetImpl(*this).SetStopBehavior(behavior);
  return *this;
}

AnimatedImage::StopBehavior LottieAnimationView::GetStopBehavior() const
{
  return Ui::GetImpl(*this).GetStopBehavior();
}

LottieAnimationView& LottieAnimationView::SetLoopingMode(LottieAnimation::LoopingMode mode)
{
  Ui::GetImpl(*this).SetLoopingMode(mode);
  return *this;
}

LottieAnimation::LoopingMode LottieAnimationView::GetLoopingMode() const
{
  return Ui::GetImpl(*this).GetLoopingMode();
}

LottieAnimationView& LottieAnimationView::SetFrameSpeedFactor(float factor)
{
  Ui::GetImpl(*this).SetFrameSpeedFactor(factor);
  return *this;
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

LottieAnimationView& LottieAnimationView::SetRedrawOnScaleDown(bool redraw)
{
  Ui::GetImpl(*this).SetRedrawOnScaleDown(redraw);
  return *this;
}

bool LottieAnimationView::IsRedrawOnScaleDown() const
{
  return Ui::GetImpl(*this).IsRedrawOnScaleDown();
}

LottieAnimationView& LottieAnimationView::SetRedrawOnScaleUp(bool redraw)
{
  Ui::GetImpl(*this).SetRedrawOnScaleUp(redraw);
  return *this;
}

bool LottieAnimationView::IsRedrawOnScaleUp() const
{
  return Ui::GetImpl(*this).IsRedrawOnScaleUp();
}

LottieAnimationView& LottieAnimationView::SetEnableFrameCache(bool enable)
{
  Ui::GetImpl(*this).SetEnableFrameCache(enable);
  return *this;
}

bool LottieAnimationView::IsFrameCacheEnabled() const
{
  return Ui::GetImpl(*this).IsFrameCacheEnabled();
}

LottieAnimationView& LottieAnimationView::SetNotifyAfterRasterization(bool notify)
{
  Ui::GetImpl(*this).SetNotifyAfterRasterization(notify);
  return *this;
}

bool LottieAnimationView::IsNotifyAfterRasterizationEnabled() const
{
  return Ui::GetImpl(*this).IsNotifyAfterRasterizationEnabled();
}

LottieAnimationView& LottieAnimationView::SetRenderScale(float scale)
{
  Ui::GetImpl(*this).SetRenderScale(scale);
  return *this;
}

float LottieAnimationView::GetRenderScale() const
{
  return Ui::GetImpl(*this).GetRenderScale();
}

LottieAnimationView& LottieAnimationView::SetEnableAspectFit(bool enableAspectFit)
{
  Ui::GetImpl(*this).SetEnableAspectFit(enableAspectFit);
  return *this;
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

LottieAnimationView& LottieAnimationView::SetDynamicProperty(const LottieAnimation::DynamicPropertyInfo& info)
{
  Ui::GetImpl(*this).SetDynamicProperty(info);
  return *this;
}

LottieAnimationView& LottieAnimationView::SetDesiredWidth(int width)
{
  Ui::GetImpl(*this).SetDesiredWidth(width);
  return *this;
}

int LottieAnimationView::GetDesiredWidth() const
{
  return Ui::GetImpl(*this).GetDesiredWidth();
}

LottieAnimationView& LottieAnimationView::SetDesiredHeight(int height)
{
  Ui::GetImpl(*this).SetDesiredHeight(height);
  return *this;
}

int LottieAnimationView::GetDesiredHeight() const
{
  return Ui::GetImpl(*this).GetDesiredHeight();
}

LottieAnimationView& LottieAnimationView::SetReleasePolicy(Ui::Image::ReleasePolicy releasePolicy)
{
  Ui::GetImpl(*this).SetReleasePolicy(releasePolicy);
  return *this;
}

Ui::Image::ReleasePolicy LottieAnimationView::GetReleasePolicy() const
{
  return Ui::GetImpl(*this).GetReleasePolicy();
}

LottieAnimationView& LottieAnimationView::SetSynchronousLoading(bool synchronous)
{
  Ui::GetImpl(*this).SetSynchronousLoading(synchronous);
  return *this;
}

bool LottieAnimationView::IsSynchronousLoading() const
{
  return Ui::GetImpl(*this).IsSynchronousLoading();
}

LottieAnimationView& LottieAnimationView::SetImageColor(const UiColor& color)
{
  Ui::GetImpl(*this).SetImageColor(color);
  return *this;
}

UiColor LottieAnimationView::GetImageColor() const
{
  return Ui::GetImpl(*this).GetImageColor();
}

LottieAnimationView& LottieAnimationView::SetPlaceholderUrl(const Dali::String& url)
{
  Ui::GetImpl(*this).SetPlaceholderUrl(url);
  return *this;
}

Dali::String LottieAnimationView::GetPlaceholderUrl() const
{
  return Ui::GetImpl(*this).GetPlaceholderUrl();
}

LottieAnimationView& LottieAnimationView::SetPixelArea(const Dali::Vector4& pixelArea)
{
  Ui::GetImpl(*this).SetPixelArea(pixelArea);
  return *this;
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
