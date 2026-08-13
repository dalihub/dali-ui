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
#include <dali-ui-components/public-api/navigator/navigator.h>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/navigator/navigator-impl.h>

// EXTERNAL INCLUDES
#include <utility>

namespace Dali
{
namespace Ui
{

Navigator::Navigator()
{
}

Navigator Navigator::New()
{
  return Integration::NavigatorImpl::New();
}

Navigator::Navigator(const Navigator& navigator)
: View(navigator)
{
}

Navigator::Navigator(Navigator&& rhs) noexcept
: View(std::move(rhs))
{
}

Navigator::~Navigator()
{
}

Navigator& Navigator::operator=(const Navigator& handle)
{
  if(&handle != this)
  {
    Ui::View::operator=(handle);
  }
  return *this;
}

Navigator& Navigator::operator=(Navigator&& rhs) noexcept
{
  Ui::View::operator=(std::move(rhs));
  return *this;
}

Navigator Navigator::DownCast(BaseHandle handle)
{
  Navigator result;
  Ui::View  control = Ui::View::DownCast(handle);
  if(control)
  {
    CustomActorImpl&            customImpl = control.GetImplementation();
    Integration::NavigatorImpl* impl       = dynamic_cast<Integration::NavigatorImpl*>(&customImpl);
    if(impl)
    {
      result = Navigator(customImpl.GetOwner());
    }
  }
  return result;
}

void Navigator::Push(View page, bool animated)
{
  GetImpl(*this).Push(page, animated);
}

View Navigator::Pop(bool animated)
{
  return GetImpl(*this).Pop(animated);
}

void Navigator::InsertBefore(View page, View before)
{
  GetImpl(*this).InsertBefore(page, before);
}

void Navigator::Remove(View page)
{
  GetImpl(*this).Remove(page);
}

void Navigator::Clear()
{
  GetImpl(*this).Clear();
}

void Navigator::PushModal(View modal, bool animated)
{
  GetImpl(*this).PushModal(modal, animated);
}

View Navigator::PopModal(bool animated)
{
  return GetImpl(*this).PopModal(animated);
}

View Navigator::GetCurrentView() const
{
  return GetImpl(*this).GetCurrentView();
}

uint32_t Navigator::GetNavigationStackCount() const
{
  return GetImpl(*this).GetNavigationStackCount();
}

uint32_t Navigator::GetModalStackCount() const
{
  return GetImpl(*this).GetModalStackCount();
}

View Navigator::GetNavigationStackItem(uint32_t index) const
{
  return GetImpl(*this).GetNavigationStackItem(index);
}

View Navigator::GetModalStackItem(uint32_t index) const
{
  return GetImpl(*this).GetModalStackItem(index);
}

bool Navigator::NavigateBack()
{
  return GetImpl(*this).NavigateBack();
}

Navigator::BackRequestedSignalType& Navigator::BackRequestedSignal()
{
  return GetImpl(*this).BackRequestedSignal();
}

void Navigator::SetPageTransitionAnimationEnabled(bool enabled)
{
  GetImpl(*this).SetPageTransitionAnimationEnabled(enabled);
}

bool Navigator::IsPageTransitionAnimationEnabled() const
{
  return GetImpl(*this).IsPageTransitionAnimationEnabled();
}

void Navigator::SetModalTransitionAnimationEnabled(bool enabled)
{
  GetImpl(*this).SetModalTransitionAnimationEnabled(enabled);
}

bool Navigator::IsModalTransitionAnimationEnabled() const
{
  return GetImpl(*this).IsModalTransitionAnimationEnabled();
}

void Navigator::SetTransitionSpec(NavigationTransitionSpec spec)
{
  GetImpl(*this).SetTransitionSpec(std::move(spec));
}

void Navigator::ClearTransitionSpec()
{
  GetImpl(*this).ClearTransitionSpec();
}

void Navigator::SetPageTransitionSpec(View page, NavigationTransitionSpec spec)
{
  GetImpl(*this).SetPageTransitionSpec(page, std::move(spec));
}

void Navigator::ClearPageTransitionSpec(View page)
{
  GetImpl(*this).ClearPageTransitionSpec(page);
}

void Navigator::SetModalTransitionSpec(NavigationTransitionSpec spec)
{
  GetImpl(*this).SetModalTransitionSpec(std::move(spec));
}

void Navigator::ClearModalTransitionSpec()
{
  GetImpl(*this).ClearModalTransitionSpec();
}

void Navigator::SetPageModalTransitionSpec(View page, NavigationTransitionSpec spec)
{
  GetImpl(*this).SetPageModalTransitionSpec(page, std::move(spec));
}

void Navigator::ClearPageModalTransitionSpec(View page)
{
  GetImpl(*this).ClearPageModalTransitionSpec(page);
}

Navigator::PageEventSignalType& Navigator::PageWillAppearSignal()
{
  return GetImpl(*this).PageWillAppearSignal();
}

Navigator::PageEventSignalType& Navigator::PageDidAppearSignal()
{
  return GetImpl(*this).PageDidAppearSignal();
}

Navigator::PageEventSignalType& Navigator::PageWillDisappearSignal()
{
  return GetImpl(*this).PageWillDisappearSignal();
}

Navigator::PageEventSignalType& Navigator::PageDidDisappearSignal()
{
  return GetImpl(*this).PageDidDisappearSignal();
}

Navigator::TransitionFinishedSignalType& Navigator::TransitionFinishedSignal()
{
  return GetImpl(*this).TransitionFinishedSignal();
}

Navigator::Navigator(Integration::NavigatorImpl& implementation)
: View(implementation)
{
}

Navigator::Navigator(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Integration::NavigatorImpl>(internal);
}

} // namespace Ui
} // namespace Dali
