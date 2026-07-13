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
#include <dali-ui-components/public-api/check-box.h>

// INTERNAL INCLUDES
#include <dali-ui-components/internal/check-box-impl.h>

namespace Dali
{
namespace Ui
{

CheckBox::CheckBox()  = default;
CheckBox::~CheckBox() = default;

CheckBox CheckBox::New()
{
  return New(CheckBoxStyle::Default());
}

CheckBox CheckBox::New(CheckBoxStyle style)
{
  return Internal::CheckBoxImpl::New(style);
}

CheckBox CheckBox::New(const Dali::String& text)
{
  CheckBox checkBox = New(CheckBoxStyle::Default());
  checkBox.SetText(text);
  return checkBox;
}

CheckBox CheckBox::New(const Dali::String& text, CheckBoxStyle style)
{
  CheckBox checkBox = New(style);
  checkBox.SetText(text);
  return checkBox;
}

CheckBox CheckBox::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<CheckBox, Internal::CheckBoxImpl>(handle);
}

CheckBox::CheckBox(const CheckBox& handle)
: SelectableView(handle)
{
}

CheckBox::CheckBox(CheckBox&& rhs) noexcept = default;

CheckBox& CheckBox::operator=(const CheckBox& handle)
{
  if(&handle != this)
  {
    SelectableView::operator=(handle);
  }
  return *this;
}

CheckBox& CheckBox::operator=(CheckBox&& rhs) noexcept = default;

void CheckBox::SetText(const Dali::String& text)
{
  GetImpl(*this).SetText(text);
}

Dali::String CheckBox::GetText() const
{
  return GetImpl(*this).GetText();
}

void CheckBox::SetSelectionAnimationMode(SelectionAnimationMode mode)
{
  GetImpl(*this).SetSelectionAnimationMode(mode);
}

SelectionAnimationMode CheckBox::GetSelectionAnimationMode() const
{
  return GetImpl(*this).GetSelectionAnimationMode();
}

CheckBox::CheckBox(Internal::CheckBoxImpl& implementation)
: SelectableView(implementation)
{
}

CheckBox::CheckBox(Dali::Internal::CustomActor* internal)
: SelectableView(internal)
{
  VerifyCustomActorPointer<Internal::CheckBoxImpl>(internal);
}

} // namespace Ui
} // namespace Dali
