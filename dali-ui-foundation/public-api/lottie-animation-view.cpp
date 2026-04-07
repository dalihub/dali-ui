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

inline Integration::LottieAnimationViewImpl& GetImpl(LottieAnimationView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  return static_cast<Integration::LottieAnimationViewImpl&>(obj.GetImplementation());
}

inline const Integration::LottieAnimationViewImpl& GetImpl(const LottieAnimationView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  return static_cast<const Integration::LottieAnimationViewImpl&>(obj.GetImplementation());
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
LottieAnimationView& LottieAnimationView::SetResourceUrl(const Dali::String& url)
{
  Ui::GetImpl(*this).SetResourceUrl(url);
  return *this;
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
LottieAnimationView::LottieAnimationView(Integration::LottieAnimationViewImpl& implementation)
: View(implementation)
{
}
LottieAnimationView::LottieAnimationView(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Integration::LottieAnimationViewImpl>(internal);
}
} //namespace Ui
} //namespace Dali
