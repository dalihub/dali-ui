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
#include <dali-ui-components/public-api/switch.h>

// INTERNAL INCLUDES
#include <dali-ui-components/internal/switch-impl.h>

namespace Dali
{
namespace Ui
{

Switch::Switch() = default;

Switch::~Switch() = default;

Switch Switch::New()
{
  return Internal::SwitchImpl::New(SwitchStyle::Default());
}

Switch Switch::New(SwitchStyle style)
{
  return Internal::SwitchImpl::New(style);
}

Switch Switch::DownCast(BaseHandle handle)
{
  return View::DownCast<Switch, Internal::SwitchImpl>(handle);
}

Switch::Switch(const Switch& handle) = default;

Switch::Switch(Switch&& rhs) noexcept = default;

Switch& Switch::operator=(const Switch& handle) = default;

Switch& Switch::operator=(Switch&& rhs) noexcept = default;

void Switch::SetSelectionAnimationMode(SelectionAnimationMode mode)
{
  GetImpl(*this).SetSelectionAnimationMode(mode);
}

SelectionAnimationMode Switch::GetSelectionAnimationMode() const
{
  return GetImpl(*this).GetSelectionAnimationMode();
}

Switch::Switch(Internal::SwitchImpl& implementation)
: SelectableView(implementation)
{
}

Switch::Switch(Dali::Internal::CustomActor* internal)
: SelectableView(internal)
{
  VerifyCustomActorPointer<Internal::SwitchImpl>(internal);
}

} // namespace Ui
} // namespace Dali
