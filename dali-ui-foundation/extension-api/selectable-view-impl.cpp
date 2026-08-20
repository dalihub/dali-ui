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
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/extension-api/selectable-view-impl.h>
#include <dali-ui-foundation/internal/views/view/core-interaction-object.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/views/selectable-view.h>

namespace Dali
{

namespace Ui
{
namespace Extension
{

namespace
{

BaseHandle Create()
{
  return SelectableView::New();
}

// Type Registration
DALI_TYPE_REGISTRATION_BEGIN_FULL(Ui::SelectableView, Ui::Extension::SelectableViewImpl, Ui::InteractiveView, Create)
DALI_TYPE_REGISTRATION_END()

} // namespace

SelectableViewImplPtr SelectableViewImpl::New()
{
  return new SelectableViewImpl();
}

void SelectableViewImpl::OnInitialize()
{
  InteractiveViewImpl::OnInitialize();
  EnsureSelectableTrait();
}

Signal<void(Ui::View, bool, InputEvent)>& SelectableViewImpl::SelectionChangedSignal()
{
  return GetSelectableTrait().SelectionChangedSignal();
}

bool SelectableViewImpl::IsSelected() const
{
  return GetSelectableTrait().IsSelected();
}

void SelectableViewImpl::SetSelected(bool selected)
{
  GetSelectableTrait().SetSelected(selected);
}

bool SelectableViewImpl::IsToggleByClickEnabled() const
{
  return GetSelectableTrait().IsToggleByClickEnabled();
}

void SelectableViewImpl::SetToggleByClickEnabled(bool enabled)
{
  GetSelectableTrait().SetToggleByClickEnabled(enabled);
}

SelectableTrait SelectableViewImpl::GetSelectableTrait() const
{
  auto* traitObject = Internal::ViewDataImpl::Get(*this).GetCoreInteractionObject();
  DALI_ASSERT_ALWAYS(traitObject && "SelectableViewImpl requires SelectableTrait");

  SelectableTrait trait = SelectableTrait::DownCast(BaseHandle(static_cast<BaseObject*>(traitObject)));
  DALI_ASSERT_ALWAYS(trait && "SelectableViewImpl requires SelectableTrait");

  return trait;
}

SelectableViewImpl::SelectableViewImpl()
{
}

SelectableViewImpl::~SelectableViewImpl()
{
}

} // namespace Extension
} // namespace Ui

} // namespace Dali
