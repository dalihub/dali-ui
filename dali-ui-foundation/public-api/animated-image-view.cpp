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

namespace Dali
{
namespace Ui
{

inline Integration::AnimatedImageViewImpl& GetImpl(AnimatedImageView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  return static_cast<Integration::AnimatedImageViewImpl&>(obj.GetImplementation());
}

inline const Integration::AnimatedImageViewImpl& GetImpl(const AnimatedImageView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  return static_cast<const Integration::AnimatedImageViewImpl&>(obj.GetImplementation());
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
Ui::Visual::ResourceStatus AnimatedImageView::GetLoadingStatus() const
{
  return Ui::GetImpl(*this).GetLoadingStatus();
}
View::ResourceReadySignalType& AnimatedImageView::ResourceReadySignal()
{
  return View::ResourceReadySignal();
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
