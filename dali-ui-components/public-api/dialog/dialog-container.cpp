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
#include <dali-ui-components/public-api/dialog/dialog-container.h>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/dialog/dialog-container-impl.h>

namespace Dali
{
namespace Ui
{

DialogContainer::DialogContainer()
{
}

DialogContainer DialogContainer::New()
{
  return Integration::DialogContainerImpl::New();
}

DialogContainer::DialogContainer(const DialogContainer& dialogContainer)
: View(dialogContainer)
{
}

DialogContainer::DialogContainer(DialogContainer&& rhs) noexcept
: View(std::move(rhs))
{
}

DialogContainer::~DialogContainer()
{
}

DialogContainer& DialogContainer::operator=(const DialogContainer& handle)
{
  if(&handle != this)
  {
    Ui::View::operator=(handle);
  }
  return *this;
}

DialogContainer& DialogContainer::operator=(DialogContainer&& rhs) noexcept
{
  Ui::View::operator=(std::move(rhs));
  return *this;
}

DialogContainer DialogContainer::DownCast(BaseHandle handle)
{
  DialogContainer result;
  Ui::View        control = Ui::View::DownCast(handle);
  if(control)
  {
    CustomActorImpl&                  customImpl = control.GetImplementation();
    Integration::DialogContainerImpl* impl       = dynamic_cast<Integration::DialogContainerImpl*>(&customImpl);
    if(impl)
    {
      result = DialogContainer(customImpl.GetOwner());
    }
  }
  return result;
}

void DialogContainer::SetModalContent(View modalContent)
{
  GetImpl(*this).SetModalContent(modalContent);
}

View DialogContainer::GetModalContent() const
{
  return GetImpl(*this).GetModalContent();
}

void DialogContainer::SetScrim(View scrim)
{
  GetImpl(*this).SetScrim(scrim);
}

View DialogContainer::GetScrim() const
{
  return GetImpl(*this).GetScrim();
}

DialogContainer::ScrimClickedSignalType& DialogContainer::ScrimClickedSignal()
{
  return GetImpl(*this).ScrimClickedSignal();
}

DialogContainer::DialogContainer(Integration::DialogContainerImpl& implementation)
: View(implementation)
{
}

DialogContainer::DialogContainer(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Integration::DialogContainerImpl>(internal);
}

} // namespace Ui
} // namespace Dali
