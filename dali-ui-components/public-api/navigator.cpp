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

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/navigator.h>
#include <dali-ui-components/internal/navigator-impl.h>

namespace Dali
{

namespace UI
{

Navigator::Navigator()
{
}

Navigator Navigator::New()
{
  return Internal::NavigatorImpl::New();
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
  if (&handle != this)
  {
    Toolkit::Control::operator=(handle);
  }
  return *this;
}

Navigator& Navigator::operator=(Navigator&& rhs) noexcept
{
  Toolkit::Control::operator=(std::move(rhs));
  return *this;
}

Navigator Navigator::DownCast(BaseHandle handle)
{
  Navigator result;
  Toolkit::Control control = Toolkit::Control::DownCast(handle);
  if (control)
  {
    CustomActorImpl& customImpl = control.GetImplementation();
    Internal::NavigatorImpl* impl = dynamic_cast<Internal::NavigatorImpl*>(&customImpl);
    if (impl)
    {
      result = Navigator(customImpl.GetOwner());
    }
  }
  return result;
}

void Navigator::Push(Page page)
{
  Internal::GetImpl(*this).Push(page);
}

Page Navigator::Pop()
{
  return Internal::GetImpl(*this).Pop();
}

void Navigator::Insert(Page page, uint32_t index)
{
  Internal::GetImpl(*this).Insert(page, index);
}

void Navigator::Remove(Page page)
{
  Internal::GetImpl(*this).Remove(page);
}

Page Navigator::GetPage(uint32_t index) const
{
  return Internal::GetImpl(*this).GetPage(index);
}

uint32_t Navigator::GetPageCount() const
{
  return Internal::GetImpl(*this).GetPageCount();
}

Navigator::Navigator(Internal::NavigatorImpl& implementation)
  : View(implementation)
{
}

Navigator::Navigator(Dali::Internal::CustomActor* internal)
  : View(internal)
{
  VerifyCustomActorPointer<Internal::NavigatorImpl>(internal);
}

} // namespace UI

} // namespace Dali
