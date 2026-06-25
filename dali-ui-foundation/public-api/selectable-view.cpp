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

#include <dali-ui-foundation/provider-api/selectable-view-impl.h>
#include <dali-ui-foundation/public-api/selectable-view.h>

namespace Dali
{

namespace Ui
{

SelectableView::SelectableView()
{
}

SelectableView SelectableView::New()
{
  Provider::SelectableViewImplPtr impl = Provider::SelectableViewImpl::New();

  SelectableView handle(*impl);

  impl->Initialize();

  return handle;
}

SelectableView SelectableView::DownCast(BaseHandle handle)
{
  return View::DownCast<SelectableView, Provider::SelectableViewImpl>(handle);
}

SelectableView::SelectableView(const SelectableView& view) = default;

SelectableView::SelectableView(SelectableView&& rhs) noexcept = default;

SelectableView::~SelectableView()
{
}

SelectableView::SelectableView(Provider::SelectableViewImpl& implementation)
: InteractiveView(implementation)
{
}

SelectableView::SelectableView(Dali::Internal::CustomActor* internal)
: InteractiveView(internal)
{
  VerifyCustomActorPointer<Provider::SelectableViewImpl>(internal);
}

Signal<void(View, bool, InputEvent)>& SelectableView::SelectionChangedSignal()
{
  return Provider::GetImpl(*this).SelectionChangedSignal();
}

bool SelectableView::IsSelected() const
{
  return Provider::GetImpl(*this).IsSelected();
}

void SelectableView::SetSelected(bool selected)
{
  Provider::GetImpl(*this).SetSelected(selected);
}

bool SelectableView::IsToggleByClickEnabled() const
{
  return Provider::GetImpl(*this).IsToggleByClickEnabled();
}

void SelectableView::EnableToggleByClick(bool enabled)
{
  Provider::GetImpl(*this).EnableToggleByClick(enabled);
}

} // namespace Ui

} // namespace Dali
