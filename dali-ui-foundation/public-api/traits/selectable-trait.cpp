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
#include <dali-ui-foundation/internal/views/view/selectable-trait-impl.h>
#include <dali-ui-foundation/public-api/traits/selectable-trait.h>

namespace Dali
{

namespace Ui
{

SelectableTrait::SelectableTrait()
{
}

SelectableTrait::SelectableTrait(const SelectableTrait& trait)
: InteractiveTrait(trait)
{
}

SelectableTrait::~SelectableTrait()
{
}

SelectableTrait SelectableTrait::New()
{
  IntrusivePtr<Internal::CoreInteractionObject> traitObject = new Internal::CoreInteractionObject();
  return New(traitObject.Get());
}

SelectableTrait SelectableTrait::New(Internal::CoreInteractionObject* container)
{
  DALI_ASSERT_ALWAYS(container && "SelectableTrait::New requires CoreInteractionObject");
  container->EnsureSelectableTraitImpl();
  return SelectableTrait(container);
}

SelectableTrait::SelectableTrait(Internal::CoreInteractionObject* container)
: InteractiveTrait(container)
{
}

SelectableTrait SelectableTrait::DownCast(BaseHandle handle)
{
  if(auto* traitObject = dynamic_cast<Internal::CoreInteractionObject*>(handle.GetObjectPtr()))
  {
    return traitObject->GetSelectableTraitImpl() ? SelectableTrait(traitObject) : SelectableTrait();
  }

  return SelectableTrait();
}

Internal::SelectableTraitImpl& GetImpl(SelectableTrait& obj)
{
  BaseObject& baseObject  = obj.GetBaseObject();
  auto&       traitObject = static_cast<Internal::CoreInteractionObject&>(baseObject);
  auto*       traitImpl   = traitObject.GetSelectableTraitImpl();
  DALI_ASSERT_ALWAYS(traitImpl && "SelectableTrait handle does not contain a SelectableTraitImpl");
  return *traitImpl;
}

const Internal::SelectableTraitImpl& GetImpl(const SelectableTrait& obj)
{
  const BaseObject& baseObject  = obj.GetBaseObject();
  auto&             traitObject = static_cast<const Internal::CoreInteractionObject&>(baseObject);
  auto*             traitImpl   = traitObject.GetSelectableTraitImpl();
  DALI_ASSERT_ALWAYS(traitImpl && "SelectableTrait handle does not contain a SelectableTraitImpl");
  return *traitImpl;
}

Signal<void(View, bool, InputEvent)>& SelectableTrait::SelectionChangedSignal()
{
  return GetImpl(*this).SelectionChangedSignal();
}

bool SelectableTrait::IsSelected() const
{
  return GetImpl(*this).IsSelected();
}

void SelectableTrait::SetSelected(bool selected)
{
  GetImpl(*this).SetSelected(selected);
}

bool SelectableTrait::IsToggleByClickEnabled() const
{
  return GetImpl(*this).IsToggleByClickEnabled();
}

void SelectableTrait::SetToggleByClickEnabled(bool enabled)
{
  GetImpl(*this).SetToggleByClickEnabled(enabled);
}

} // namespace Ui

} // namespace Dali
