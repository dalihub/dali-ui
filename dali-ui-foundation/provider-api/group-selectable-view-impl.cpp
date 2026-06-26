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
#include <dali-ui-foundation/internal/views/view/core-interaction-object.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/provider-api/group-selectable-view-impl.h>
#include <dali-ui-foundation/public-api/group-selectable-view.h>

namespace Dali
{

namespace Ui
{
namespace Provider
{

namespace
{

BaseHandle Create()
{
  return GroupSelectableView::New();
}

// Type Registration
DALI_TYPE_REGISTRATION_BEGIN(GroupSelectableViewImpl, SelectableViewImpl, Create)
DALI_TYPE_REGISTRATION_END()

} // namespace

GroupSelectableViewImplPtr GroupSelectableViewImpl::New()
{
  return new GroupSelectableViewImpl();
}

void GroupSelectableViewImpl::OnInitialize()
{
  SelectableViewImpl::OnInitialize();
  EnsureGroupSelectableTrait();
}

void GroupSelectableViewImpl::SetGroupName(const Dali::String& name)
{
  GetGroupSelectableTrait().SetGroupName(name);
}

Dali::String GroupSelectableViewImpl::GetGroupName() const
{
  return GetGroupSelectableTrait().GetGroupName();
}

SelectionGroup GroupSelectableViewImpl::GetGroup() const
{
  return GetGroupSelectableTrait().GetGroup();
}

GroupSelectableTrait GroupSelectableViewImpl::GetGroupSelectableTrait() const
{
  auto* traitObject = Internal::ViewDataImpl::Get(*this).GetCoreInteractionObject();
  DALI_ASSERT_ALWAYS(traitObject && "GroupSelectableViewImpl requires GroupSelectableTrait");

  GroupSelectableTrait trait = GroupSelectableTrait::DownCast(BaseHandle(static_cast<BaseObject*>(traitObject)));
  DALI_ASSERT_ALWAYS(trait && "GroupSelectableViewImpl requires GroupSelectableTrait");

  return trait;
}

GroupSelectableViewImpl::GroupSelectableViewImpl()
{
}

GroupSelectableViewImpl::~GroupSelectableViewImpl()
{
}

} // namespace Provider
} // namespace Ui

} // namespace Dali
