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
#include <dali-ui-foundation/integration-api/selectable-trait-impl.h>
#include <dali-ui-foundation/public-api/selectable-trait.h>

namespace Dali
{

namespace Ui
{

SelectableTrait::SelectableTrait()
{
}

SelectableTrait SelectableTrait::New()
{
  IntrusivePtr<Integration::SelectableTraitImpl> impl = new Integration::SelectableTraitImpl();
  return SelectableTrait(impl.Get());
}

SelectableTrait::SelectableTrait(const SelectableTrait& trait)
: Trait(trait)
{
}

SelectableTrait::~SelectableTrait()
{
}

SelectableTrait::SelectableTrait(Integration::SelectableTraitImpl* implementation)
: Trait(implementation)
{
}

SelectableTrait SelectableTrait::DownCast(BaseHandle handle)
{
  return SelectableTrait(dynamic_cast<Integration::SelectableTraitImpl*>(handle.GetObjectPtr()));
}

Signal<void(View, bool, const InputEvent&)>& SelectableTrait::SelectionChangedSignal()
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

void SelectableTrait::EnableToggleByClick(bool enabled)
{
  GetImpl(*this).EnableToggleByClick(enabled);
}

} // namespace Ui

} // namespace Dali
