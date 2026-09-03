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
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/views/view/core-interaction-object.h>
#include <dali-ui-foundation/internal/views/view/interactive-trait-impl.h>
#include <dali-ui-foundation/public-api/traits/interactive-trait.h>

namespace Dali
{

namespace Ui
{

InteractiveTrait::InteractiveTrait()
{
}

InteractiveTrait::InteractiveTrait(const InteractiveTrait& trait)
: BaseHandle(trait)
{
}

InteractiveTrait::~InteractiveTrait()
{
}

InteractiveTrait InteractiveTrait::New()
{
  IntrusivePtr<Internal::CoreInteractionObject> traitObject = new Internal::CoreInteractionObject();
  return New(traitObject.Get());
}

InteractiveTrait InteractiveTrait::New(Internal::CoreInteractionObject* container)
{
  DALI_ASSERT_ALWAYS(container && "InteractiveTrait::New requires CoreInteractionObject");
  container->EnsureInteractiveTraitImpl();
  return InteractiveTrait(container);
}

InteractiveTrait::InteractiveTrait(Internal::CoreInteractionObject* container)
: BaseHandle(container)
{
}

InteractiveTrait InteractiveTrait::DownCast(BaseHandle handle)
{
  if(auto* traitObject = dynamic_cast<Internal::CoreInteractionObject*>(handle.GetObjectPtr()))
  {
    return traitObject->GetInteractiveTraitImpl() ? InteractiveTrait(traitObject) : InteractiveTrait();
  }

  return InteractiveTrait();
}

Internal::InteractiveTraitImpl& GetImpl(InteractiveTrait& obj)
{
  BaseObject& baseObject  = obj.GetBaseObject();
  auto&       traitObject = static_cast<Internal::CoreInteractionObject&>(baseObject);
  auto*       traitImpl   = traitObject.GetInteractiveTraitImpl();
  DALI_ASSERT_ALWAYS(traitImpl && "InteractiveTrait handle does not contain an InteractiveTraitImpl");
  return *traitImpl;
}

const Internal::InteractiveTraitImpl& GetImpl(const InteractiveTrait& obj)
{
  const BaseObject& baseObject  = obj.GetBaseObject();
  auto&             traitObject = static_cast<const Internal::CoreInteractionObject&>(baseObject);
  auto*             traitImpl   = traitObject.GetInteractiveTraitImpl();
  DALI_ASSERT_ALWAYS(traitImpl && "InteractiveTrait handle does not contain an InteractiveTraitImpl");
  return *traitImpl;
}

Signal<void(View, bool, InputEvent)>& InteractiveTrait::PressedChangedSignal()
{
  return GetImpl(*this).PressedChangedSignal();
}

Signal<void(View, bool)>& InteractiveTrait::PseudoDisabledChangedSignal()
{
  return GetImpl(*this).PseudoDisabledChangedSignal();
}

Signal<void(View, InputEvent)>& InteractiveTrait::ClickedSignal()
{
  return GetImpl(*this).ClickedSignal();
}

Signal<bool(View, InputEvent)>& InteractiveTrait::LongPressedSignal()
{
  return GetImpl(*this).LongPressedSignal();
}

Signal<void(View, bool, InputEvent)>& InteractiveTrait::HoveredChangedSignal()
{
  return GetImpl(*this).HoveredChangedSignal();
}

bool InteractiveTrait::IsPressed() const
{
  return GetImpl(*this).IsPressed();
}

bool InteractiveTrait::IsPseudoDisabled() const
{
  return GetImpl(*this).IsPseudoDisabled();
}

bool InteractiveTrait::IsHovered() const
{
  return GetImpl(*this).IsHovered();
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

bool InteractiveTrait::IsLongPressEnabled() const
{
  return GetImpl(*this).IsLongPressEnabled();
}

void InteractiveTrait::SetLongPressEnabled(bool enabled)
{
  GetImpl(*this).SetLongPressEnabled(enabled);
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
