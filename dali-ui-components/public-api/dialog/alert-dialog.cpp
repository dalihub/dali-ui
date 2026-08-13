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
#include <dali-ui-components/public-api/dialog/alert-dialog.h>

// EXTERNAL INCLUDES
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/dialog/alert-dialog-impl.h>

namespace Dali
{
namespace Ui
{

AlertDialog::AlertDialog()
{
}

AlertDialog AlertDialog::New()
{
  return Integration::AlertDialogImpl::New();
}

AlertDialog::AlertDialog(const AlertDialog& alertDialog)
: Dialog(alertDialog)
{
}

AlertDialog::AlertDialog(AlertDialog&& rhs) noexcept
: Dialog(std::move(rhs))
{
}

AlertDialog::~AlertDialog()
{
}

AlertDialog& AlertDialog::operator=(const AlertDialog& handle)
{
  if(&handle != this)
  {
    Ui::View::operator=(handle);
  }
  return *this;
}

AlertDialog& AlertDialog::operator=(AlertDialog&& rhs) noexcept
{
  Ui::View::operator=(std::move(rhs));
  return *this;
}

AlertDialog AlertDialog::DownCast(BaseHandle handle)
{
  AlertDialog result;
  Ui::View    control = Ui::View::DownCast(handle);
  if(control)
  {
    CustomActorImpl&              customImpl = control.GetImplementation();
    Integration::AlertDialogImpl* impl       = dynamic_cast<Integration::AlertDialogImpl*>(&customImpl);
    if(impl)
    {
      result = AlertDialog(customImpl.GetOwner());
    }
  }
  return result;
}

void AlertDialog::SetTitle(const Dali::String& title)
{
  GetImpl(*this).SetTitle(title);
}

Dali::String AlertDialog::GetTitle() const
{
  return GetImpl(*this).GetTitle();
}

void AlertDialog::SetMessage(const Dali::String& message)
{
  GetImpl(*this).SetMessage(message);
}

Dali::String AlertDialog::GetMessage() const
{
  return GetImpl(*this).GetMessage();
}

TextButton AlertDialog::AddActionButton(const Dali::String& text)
{
  return GetImpl(*this).AddActionButton(text);
}

void AlertDialog::ClearActionButtons()
{
  GetImpl(*this).ClearActionButtons();
}

AlertDialog::AlertDialog(Integration::AlertDialogImpl& implementation)
: Dialog(implementation)
{
}

AlertDialog::AlertDialog(Dali::Internal::CustomActor* internal)
: Dialog(internal)
{
  VerifyCustomActorPointer<Integration::AlertDialogImpl>(internal);
}

} // namespace Ui
} // namespace Dali
