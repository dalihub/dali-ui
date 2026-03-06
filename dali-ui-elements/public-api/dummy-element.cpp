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
#include <dali-ui-elements/public-api/dummy-element.h>
#include <dali-ui-elements/internal/dummy-element-impl.h>

namespace Dali
{

namespace Ui
{

DummyElement::DummyElement()
{
}

DummyElement DummyElement::New()
{
  return Internal::DummyElementImpl::New();
}

DummyElement::DummyElement(const DummyElement& DummyElement)
  : View(DummyElement)
{
}

DummyElement::DummyElement(DummyElement&& rhs) noexcept
  : View(std::move(rhs))
{
}

DummyElement::~DummyElement()
{
}

DummyElement& DummyElement::operator=(const DummyElement& handle)
{
  if (&handle != this)
  {
    Ui::Control::operator=(handle);
  }
  return *this;
}

DummyElement& DummyElement::operator=(DummyElement&& rhs) noexcept
{
  Ui::Control::operator=(std::move(rhs));
  return *this;
}

DummyElement DummyElement::DownCast(BaseHandle handle)
{
  DummyElement result;
  Ui::Control control = Ui::Control::DownCast(handle);
  if (control)
  {
    CustomActorImpl& customImpl = control.GetImplementation();
    Internal::DummyElementImpl* impl = dynamic_cast<Internal::DummyElementImpl*>(&customImpl);
    if (impl)
    {
      result = DummyElement(customImpl.GetOwner());
    }
  }
  return result;
}

DummyElement::DummyElement(Internal::DummyElementImpl& implementation)
  : View(implementation)
{
}

DummyElement::DummyElement(Dali::Internal::CustomActor* internal)
  : View(internal)
{
  VerifyCustomActorPointer<Internal::DummyElementImpl>(internal);
}

} // namespace Ui

} // namespace Dali
