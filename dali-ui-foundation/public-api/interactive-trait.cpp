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
#include <dali/devel-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/interactive-trait-impl.h>
#include <dali-ui-foundation/public-api/interactive-trait.h>

namespace Dali
{

namespace Ui
{

InteractiveTrait::InteractiveTrait()
{
}

InteractiveTrait InteractiveTrait::New()
{
  IntrusivePtr<Integration::InteractiveTraitImpl> impl = new Integration::InteractiveTraitImpl();
  return InteractiveTrait(impl.Get());
}

InteractiveTrait::InteractiveTrait(const InteractiveTrait& trait)
: Trait(trait)
{
}

InteractiveTrait::~InteractiveTrait()
{
}

InteractiveTrait::InteractiveTrait(Integration::InteractiveTraitImpl* implementation)
: Trait(implementation)
{
}

InteractiveTrait InteractiveTrait::DownCast(BaseHandle handle)
{
  return InteractiveTrait(dynamic_cast<Integration::InteractiveTraitImpl*>(handle.GetObjectPtr()));
}

Signal<void(View, bool, const InputEvent&)>& InteractiveTrait::PressedChangedSignal()
{
  return GetImpl(*this).PressedChangedSignal();
}

Signal<void(View, bool)>& InteractiveTrait::PseudoDisabledChangedSignal()
{
  return GetImpl(*this).PseudoDisabledChangedSignal();
}

Signal<void(View, const InputEvent&)>& InteractiveTrait::ClickedSignal()
{
  return GetImpl(*this).ClickedSignal();
}

Signal<bool(View, const InputEvent&)>& InteractiveTrait::LongPressedSignal()
{
  return GetImpl(*this).LongPressedSignal();
}

bool InteractiveTrait::IsPressed() const
{
  return GetImpl(*this).IsPressed();
}

bool InteractiveTrait::IsPseudoDisabled() const
{
  return GetImpl(*this).IsPseudoDisabled();
}

void InteractiveTrait::SetPseudoDisabled(bool pseudoDisabled)
{
  GetImpl(*this).SetPseudoDisabled(pseudoDisabled);
}

bool InteractiveTrait::IsClickable() const
{
  return GetImpl(*this).IsClickable();
}

void InteractiveTrait::SetClickable(bool clickable)
{
  GetImpl(*this).SetClickable(clickable);
}

KeyClickPolicy InteractiveTrait::GetKeyClickPolicy() const
{
  return GetImpl(*this).GetKeyClickPolicy();
}

void InteractiveTrait::SetKeyClickPolicy(KeyClickPolicy policy)
{
  GetImpl(*this).SetKeyClickPolicy(policy);
}

} // namespace Ui

} // namespace Dali
