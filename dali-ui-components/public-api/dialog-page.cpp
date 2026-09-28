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
#include <dali-ui-components/public-api/dialog-page.h>
#include <dali-ui-components/internal/dialog-page-impl.h>

namespace Dali
{

namespace UI
{

DialogPage::DialogPage()
{
}

DialogPage DialogPage::New()
{
  return Internal::DialogPageImpl::New();
}

DialogPage::DialogPage(const DialogPage& dialogPage)
  : Page(dialogPage)
{
}

DialogPage::DialogPage(DialogPage&& rhs) noexcept
  : Page(std::move(rhs))
{
}

DialogPage::~DialogPage()
{
}

DialogPage& DialogPage::operator=(const DialogPage& handle)
{
  if (&handle != this)
  {
    Toolkit::Control::operator=(handle);
  }
  return *this;
}

DialogPage& DialogPage::operator=(DialogPage&& rhs) noexcept
{
  Toolkit::Control::operator=(std::move(rhs));
  return *this;
}

DialogPage DialogPage::DownCast(BaseHandle handle)
{
  DialogPage result;
  Toolkit::Control control = Toolkit::Control::DownCast(handle);
  if (control)
  {
    CustomActorImpl& customImpl = control.GetImplementation();
    Internal::DialogPageImpl* impl = dynamic_cast<Internal::DialogPageImpl*>(&customImpl);
    if (impl)
    {
      result = DialogPage(customImpl.GetOwner());
    }
  }
  return result;
}

DialogPage::DialogPage(Internal::DialogPageImpl& implementation)
  : Page(implementation)
{
}

DialogPage::DialogPage(Dali::Internal::CustomActor* internal)
  : Page(internal)
{
  VerifyCustomActorPointer<Internal::DialogPageImpl>(internal);
}

} // namespace UI

} // namespace Dali
