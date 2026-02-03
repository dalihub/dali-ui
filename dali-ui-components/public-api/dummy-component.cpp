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
#include <dali-ui-components/public-api/dummy-component.h>
#include <dali-ui-components/internal/dummy-component-impl.h>

namespace Dali
{

namespace UI
{

DummyComponent::DummyComponent()
{
}

DummyComponent DummyComponent::New()
{
  return Internal::DummyComponentImpl::New();
}

DummyComponent::DummyComponent(const DummyComponent& DummyComponent)
  : View(DummyComponent)
{
}

DummyComponent::DummyComponent(DummyComponent&& rhs) noexcept
  : View(std::move(rhs))
{
}

DummyComponent::~DummyComponent()
{
}

DummyComponent& DummyComponent::operator=(const DummyComponent& handle)
{
  if (&handle != this)
  {
    Toolkit::Control::operator=(handle);
  }
  return *this;
}

DummyComponent& DummyComponent::operator=(DummyComponent&& rhs) noexcept
{
  Toolkit::Control::operator=(std::move(rhs));
  return *this;
}

DummyComponent DummyComponent::DownCast(BaseHandle handle)
{
  DummyComponent result;
  Toolkit::Control control = Toolkit::Control::DownCast(handle);
  if (control)
  {
    CustomActorImpl& customImpl = control.GetImplementation();
    Internal::DummyComponentImpl* impl = dynamic_cast<Internal::DummyComponentImpl*>(&customImpl);
    if (impl)
    {
      result = DummyComponent(customImpl.GetOwner());
    }
  }
  return result;
}

DummyComponent::DummyComponent(Internal::DummyComponentImpl& implementation)
  : View(implementation)
{
}

DummyComponent::DummyComponent(Dali::Internal::CustomActor* internal)
  : View(internal)
{
  VerifyCustomActorPointer<Internal::DummyComponentImpl>(internal);
}

} // namespace UI

} // namespace Dali
