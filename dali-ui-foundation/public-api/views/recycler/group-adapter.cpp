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

#include <dali-ui-foundation/integration-api/group-adapter-impl.h>
#include <dali-ui-foundation/public-api/views/recycler/group-adapter.h>
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Ui
{

GroupAdapter GroupAdapter::New()
{
  GroupAdapterImplPtr impl = GroupAdapterImpl::New();
  return GroupAdapter(impl.Get());
}

GroupAdapter::GroupAdapter(GroupAdapterImpl* impl)
: BaseHandle(impl)
{
}

GroupAdapterImpl& GroupAdapter::GetImpl()
{
  DALI_ASSERT_ALWAYS(*this && "GroupAdapter handle is empty");
  return static_cast<GroupAdapterImpl&>(GetBaseObject());
}

const GroupAdapterImpl& GroupAdapter::GetImpl() const
{
  DALI_ASSERT_ALWAYS(*this && "GroupAdapter handle is empty");
  return static_cast<const GroupAdapterImpl&>(GetBaseObject());
}

void GroupAdapter::SetDataSource(std::shared_ptr<GroupDataSource> source)
{
  GetImpl().SetDataSource(std::move(source));
}

void GroupAdapter::ClearDataSource()
{
  GetImpl().ClearDataSource();
}

void GroupAdapter::SetInnerAdapter(ItemAdapter inner)
{
  GetImpl().SetInnerAdapter(std::move(inner));
}

void GroupAdapter::SetGapHeight(float height)
{
  GetImpl().SetGapHeight(height);
}

float GroupAdapter::GetGapHeight() const
{
  return GetImpl().GetGapHeight();
}

Signal<void(ItemViewHolder&)>& GroupAdapter::CreateHeaderViewHolderSignal()
{
  return GetImpl().CreateHeaderViewHolderSignal();
}

Signal<void(ItemViewHolder&)>& GroupAdapter::BindHeaderViewHolderSignal()
{
  return GetImpl().BindHeaderViewHolderSignal();
}

Signal<void(ItemViewHolder&)>& GroupAdapter::RecycleHeaderViewHolderSignal()
{
  return GetImpl().RecycleHeaderViewHolderSignal();
}

void GroupAdapter::NotifyDataSetChanged()
{
  GetImpl().NotifyDataSetChanged();
}

ItemAdapter GroupAdapter::GetAdapter() const
{
  return GetImpl().GetFlatAdapter();
}

} // namespace Ui
} // namespace Dali
