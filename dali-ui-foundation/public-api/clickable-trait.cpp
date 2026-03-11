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
#include <dali-ui-foundation/public-api/clickable-trait.h>
#include <dali-ui-foundation/integration-api/clickable-trait-impl.h>

namespace Dali
{

namespace Ui
{

ClickableTrait::ClickableTrait()
{
}

ClickableTrait ClickableTrait::New()
{
  IntrusivePtr<Integration::ClickableTraitImpl> impl = new Integration::ClickableTraitImpl();
  return ClickableTrait(impl.Get());
}

ClickableTrait::ClickableTrait(const ClickableTrait& trait)
  : Trait(trait)
{
}

ClickableTrait::~ClickableTrait()
{
}

ClickableTrait::ClickableTrait(Integration::ClickableTraitImpl* implementation)
  : Trait(implementation)
{
}

ClickableTrait ClickableTrait::DownCast(BaseHandle handle)
{
  return ClickableTrait(dynamic_cast<Integration::ClickableTraitImpl*>(handle.GetObjectPtr()));
}

Signal<bool(View, const InputEvent&)>& ClickableTrait::PressedChangedSignal()
{
  return GetImpl(*this).PressedChangedSignal();
}

Signal<void(View)>& ClickableTrait::PseudoDisabledChangedSignal()
{
  return GetImpl(*this).PseudoDisabledChangedSignal();
}

Signal<bool(View, const InputEvent&)>& ClickableTrait::ClickedSignal()
{
  return GetImpl(*this).ClickedSignal();
}

Signal<bool(View, const InputEvent&)>& ClickableTrait::LongPressedSignal()
{
  return GetImpl(*this).LongPressedSignal();
}

bool ClickableTrait::IsPressed() const
{
  return GetImpl(*this).IsPressed();
}

bool ClickableTrait::IsPseudoDisabled() const
{
  return GetImpl(*this).IsPseudoDisabled();
}

void ClickableTrait::SetPseudoDisabled(bool pseudoDisabled)
{
  GetImpl(*this).SetPseudoDisabled(pseudoDisabled);
}

bool ClickableTrait::IsClickable() const
{
  return GetImpl(*this).IsClickable();
}

void ClickableTrait::SetClickable(bool clickable)
{
  GetImpl(*this).SetClickable(clickable);
}

KeyClickPolicy ClickableTrait::GetKeyClickPolicy() const
{
  return GetImpl(*this).GetKeyClickPolicy();
}

void ClickableTrait::SetKeyClickPolicy(KeyClickPolicy policy)
{
  GetImpl(*this).SetKeyClickPolicy(policy);
}

} // namespace Ui

} // namespace Dali
