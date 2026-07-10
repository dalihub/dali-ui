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

#include <dali-ui-foundation/extension-api/selectable-view-impl.h>
#include <dali-ui-foundation/public-api/views/selectable-view.h>

namespace Dali
{

namespace Ui
{

SelectableView::SelectableView()
{
}

SelectableView SelectableView::New()
{
  Extension::SelectableViewImplPtr impl = Extension::SelectableViewImpl::New();

  SelectableView handle(*impl);

  impl->Initialize();

  return handle;
}

SelectableView SelectableView::DownCast(BaseHandle handle)
{
  return View::DownCast<SelectableView, Extension::SelectableViewImpl>(handle);
}

SelectableView::SelectableView(const SelectableView& view) = default;

SelectableView::SelectableView(SelectableView&& rhs) noexcept = default;

SelectableView::~SelectableView()
{
}

SelectableView::SelectableView(Extension::SelectableViewImpl& implementation)
: InteractiveView(implementation)
{
}

SelectableView::SelectableView(Dali::Internal::CustomActor* internal)
: InteractiveView(internal)
{
  VerifyCustomActorPointer<Extension::SelectableViewImpl>(internal);
}

Signal<void(View, bool, InputEvent)>& SelectableView::SelectionChangedSignal()
{
  return Extension::GetImpl(*this).SelectionChangedSignal();
}

bool SelectableView::IsSelected() const
{
  return Extension::GetImpl(*this).IsSelected();
}

void SelectableView::SetSelected(bool selected)
{
  Extension::GetImpl(*this).SetSelected(selected);
}

bool SelectableView::IsToggleByClickEnabled() const
{
  return Extension::GetImpl(*this).IsToggleByClickEnabled();
}

void SelectableView::SetToggleByClickEnabled(bool enabled)
{
  Extension::GetImpl(*this).SetToggleByClickEnabled(enabled);
}

} // namespace Ui

} // namespace Dali
