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
#include <dali-ui-components/public-api/radio-button.h>

// INTERNAL INCLUDES
#include <dali-ui-components/internal/radio-button-impl.h>

namespace Dali
{
namespace Ui
{
RadioButton::RadioButton() = default;

RadioButton::~RadioButton() = default;

RadioButton RadioButton::New()
{
  return New(RadioButtonStyle::Default());
}

RadioButton RadioButton::New(RadioButtonStyle style)
{
  return Internal::RadioButtonImpl::New(style);
}

RadioButton RadioButton::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<RadioButton, Internal::RadioButtonImpl>(handle);
}

RadioButton::RadioButton(const RadioButton& handle)
: GroupSelectableView(handle)
{
}

RadioButton::RadioButton(RadioButton&& rhs) noexcept = default;

RadioButton& RadioButton::operator=(const RadioButton& handle)
{
  if(&handle != this)
  {
    GroupSelectableView::operator=(handle);
  }
  return *this;
}

RadioButton& RadioButton::operator=(RadioButton&& rhs) noexcept = default;

void RadioButton::SetSelectionAnimationMode(SelectionAnimationMode mode)
{
  GetImpl(*this).SetSelectionAnimationMode(mode);
}

SelectionAnimationMode RadioButton::GetSelectionAnimationMode() const
{
  return GetImpl(*this).GetSelectionAnimationMode();
}

void RadioButton::SetIconWidth(float width)
{
  GetImpl(*this).SetIconWidth(width);
}

float RadioButton::GetIconWidth() const
{
  return GetImpl(*this).GetIconWidth();
}

void RadioButton::SetIconHeight(float height)
{
  GetImpl(*this).SetIconHeight(height);
}

float RadioButton::GetIconHeight() const
{
  return GetImpl(*this).GetIconHeight();
}

RadioButton::RadioButton(Internal::RadioButtonImpl& implementation)
: GroupSelectableView(implementation)
{
}

RadioButton::RadioButton(Dali::Internal::CustomActor* internal)
: GroupSelectableView(internal)
{
  VerifyCustomActorPointer<Internal::RadioButtonImpl>(internal);
}

} // namespace Ui
} // namespace Dali
