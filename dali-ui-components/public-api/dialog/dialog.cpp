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
#include <dali-ui-components/public-api/dialog/dialog.h>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/dialog/dialog-impl.h>

namespace Dali
{
namespace Ui
{

Dialog::Dialog()
{
}

Dialog Dialog::New()
{
  return Integration::DialogImpl::New();
}

Dialog::Dialog(const Dialog& dialog)
: View(dialog)
{
}

Dialog::Dialog(Dialog&& rhs) noexcept
: View(std::move(rhs))
{
}

Dialog::~Dialog()
{
}

Dialog& Dialog::operator=(const Dialog& handle)
{
  if(&handle != this)
  {
    Ui::View::operator=(handle);
  }
  return *this;
}

Dialog& Dialog::operator=(Dialog&& rhs) noexcept
{
  Ui::View::operator=(std::move(rhs));
  return *this;
}

Dialog Dialog::DownCast(BaseHandle handle)
{
  Dialog   result;
  Ui::View control = Ui::View::DownCast(handle);
  if(control)
  {
    CustomActorImpl&         customImpl = control.GetImplementation();
    Integration::DialogImpl* impl       = dynamic_cast<Integration::DialogImpl*>(&customImpl);
    if(impl)
    {
      result = Dialog(customImpl.GetOwner());
    }
  }
  return result;
}

void Dialog::SetHeaderView(View headerView)
{
  GetImpl(*this).SetHeaderView(headerView);
}

View Dialog::GetHeaderView() const
{
  return GetImpl(*this).GetHeaderView();
}

void Dialog::SetBodyView(View bodyView)
{
  GetImpl(*this).SetBodyView(bodyView);
}

View Dialog::GetBodyView() const
{
  return GetImpl(*this).GetBodyView();
}

void Dialog::SetFooterView(View footerView)
{
  GetImpl(*this).SetFooterView(footerView);
}

View Dialog::GetFooterView() const
{
  return GetImpl(*this).GetFooterView();
}

void Dialog::SetSpacing(float spacing)
{
  GetImpl(*this).SetSpacing(spacing);
}

float Dialog::GetSpacing() const
{
  return GetImpl(*this).GetSpacing();
}

void Dialog::SetLayoutAlignment(LayoutAlignment alignment)
{
  GetImpl(*this).SetLayoutAlignment(alignment);
}

LayoutAlignment Dialog::GetLayoutAlignment() const
{
  return GetImpl(*this).GetLayoutAlignment();
}

Dialog::Dialog(Integration::DialogImpl& implementation)
: View(implementation)
{
}

Dialog::Dialog(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Integration::DialogImpl>(internal);
}

} // namespace Ui
} // namespace Dali
