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

#include <dali-ui-foundation/provider-api/group-selectable-view-impl.h>
#include <dali-ui-foundation/public-api/group-selectable-view.h>

namespace Dali
{

namespace Ui
{

GroupSelectableView::GroupSelectableView()
{
}

GroupSelectableView GroupSelectableView::New()
{
  Provider::GroupSelectableViewImplPtr impl = Provider::GroupSelectableViewImpl::New();

  GroupSelectableView handle(*impl);

  impl->Initialize();

  return handle;
}

GroupSelectableView GroupSelectableView::DownCast(BaseHandle handle)
{
  return View::DownCast<GroupSelectableView, Provider::GroupSelectableViewImpl>(handle);
}

GroupSelectableView::GroupSelectableView(const GroupSelectableView& view) = default;

GroupSelectableView::GroupSelectableView(GroupSelectableView&& rhs) noexcept = default;

GroupSelectableView::~GroupSelectableView()
{
}

GroupSelectableView::GroupSelectableView(Provider::GroupSelectableViewImpl& implementation)
: SelectableView(implementation)
{
}

GroupSelectableView::GroupSelectableView(Dali::Internal::CustomActor* internal)
: SelectableView(internal)
{
  VerifyCustomActorPointer<Provider::GroupSelectableViewImpl>(internal);
}

void GroupSelectableView::SetGroupName(const Dali::String& name)
{
  Provider::GetImpl(*this).SetGroupName(name);
}

Dali::String GroupSelectableView::GetGroupName() const
{
  return Provider::GetImpl(*this).GetGroupName();
}

SelectionGroup GroupSelectableView::GetGroup() const
{
  return Provider::GetImpl(*this).GetGroup();
}

} // namespace Ui

} // namespace Dali
