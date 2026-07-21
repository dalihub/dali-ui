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
 */

#include <dali-ui-foundation/integration-api/input-field-impl.h>
#include <dali-ui-foundation/internal/controls/text-controls/input-field-accessible.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali::Ui::Integration
{
std::pair<std::string, bool> InputFieldAccessible::GetNameRaw() const
{
  if(GetTextController()->IsShowingPlaceholderText())
  {
    return {GetCurrentPlaceholderText(), true};
  }

  if(IsHiddenInput())
  {
    return {"", true};
  }

  return {GetWholeText(), true};
}

const std::vector<Ui::TextAnchor>& InputFieldAccessible::GetTextAnchors() const
{
  auto  self = Ui::View::DownCast(Self());
  auto& impl = static_cast<InputFieldImpl&>(Ui::GetImpl(self));
  return impl.mAnchorActors;
}

Ui::Text::ControllerPtr InputFieldAccessible::GetTextController() const
{
  auto  self = Ui::View::DownCast(Self());
  auto& impl = static_cast<InputFieldImpl&>(Ui::GetImpl(self));
  return impl.mController;
}

std::uint32_t InputFieldAccessible::GetSubstituteCharacter() const
{
  auto  self = Ui::View::DownCast(Self());
  auto& impl = static_cast<InputFieldImpl&>(Ui::GetImpl(self));
  return impl.GetPasswordMaskCharacter();
}

bool InputFieldAccessible::IsHiddenInput() const
{
  auto  self = Ui::View::DownCast(Self());
  auto& impl = static_cast<InputFieldImpl&>(Ui::GetImpl(self));
  return impl.GetPasswordMode() != Ui::Text::PasswordMode::NONE;
}

void InputFieldAccessible::RequestTextRelayout()
{
  auto  self = Ui::View::DownCast(Self());
  auto& impl = static_cast<InputFieldImpl&>(Ui::GetImpl(self));
  impl.RequestTextRelayout();
}

} // namespace Dali::Ui::Integration
