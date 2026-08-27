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

#include <dali-ui-foundation/integration-api/item-adapter-impl.h>
#include <dali-ui-foundation/public-api/views/recycler/item-adapter.h>
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Ui
{

ItemAdapter ItemAdapter::New()
{
  ItemAdapterImplPtr impl = ItemAdapterImpl::New();
  return ItemAdapter(impl.Get());
}

ItemAdapter ItemAdapter::DownCast(BaseHandle handle)
{
  if(!handle)
  {
    return {};
  }

  auto* impl = dynamic_cast<ItemAdapterImpl*>(&handle.GetBaseObject());
  return impl ? ItemAdapter(impl) : ItemAdapter{};
}

ItemAdapter::ItemAdapter(ItemAdapterImpl* impl)
: BaseHandle(impl)
{
}

ItemAdapterImpl& ItemAdapter::GetImpl()
{
  DALI_ASSERT_ALWAYS(*this && "ItemAdapter handle is empty");
  return static_cast<ItemAdapterImpl&>(GetBaseObject());
}

const ItemAdapterImpl& ItemAdapter::GetImpl() const
{
  DALI_ASSERT_ALWAYS(*this && "ItemAdapter handle is empty");
  return static_cast<const ItemAdapterImpl&>(GetBaseObject());
}

Signal<uint32_t()>& ItemAdapter::GetItemCountSignal()
{
  return GetImpl().GetItemCountSignal();
}

Signal<void(ItemViewHolder&)>& ItemAdapter::CreateViewHolderSignal()
{
  return GetImpl().CreateViewHolderSignal();
}

Signal<void(ItemViewHolder&)>& ItemAdapter::BindViewHolderSignal()
{
  return GetImpl().BindViewHolderSignal();
}

Signal<uint32_t(uint32_t)>& ItemAdapter::GetItemViewTypeSignal()
{
  return GetImpl().GetItemViewTypeSignal();
}

Signal<void(ItemViewHolder&)>& ItemAdapter::RecycleViewHolderSignal()
{
  return GetImpl().RecycleViewHolderSignal();
}

Signal<void(const ItemAdapter::ChangeInfo&)>& ItemAdapter::DataChangedSignal()
{
  return GetImpl().DataChangedSignal();
}

void ItemAdapter::NotifyDataSetChanged()
{
  GetImpl().NotifyDataSetChanged();
}

void ItemAdapter::NotifyItemChanged(uint32_t position, uint32_t count)
{
  GetImpl().NotifyItemChanged(position, count);
}

void ItemAdapter::NotifyItemContentChanged(uint32_t position, uint32_t count)
{
  GetImpl().NotifyItemContentChanged(position, count);
}

void ItemAdapter::NotifyItemInserted(uint32_t position, uint32_t count)
{
  GetImpl().NotifyItemInserted(position, count);
}

void ItemAdapter::NotifyItemRemoved(uint32_t position, uint32_t count)
{
  GetImpl().NotifyItemRemoved(position, count);
}

void ItemAdapter::NotifyItemMoved(uint32_t fromPosition, uint32_t toPosition)
{
  GetImpl().NotifyItemMoved(fromPosition, toPosition);
}

uint32_t ItemAdapter::GetItemCount() const
{
  return GetImpl().GetItemCount();
}

uint32_t ItemAdapter::GetItemViewType(uint32_t position) const
{
  return GetImpl().GetItemViewType(position);
}

void ItemAdapter::CreateViewHolder(ItemViewHolder& holder)
{
  GetImpl().CreateViewHolder(holder);
}

void ItemAdapter::BindViewHolder(ItemViewHolder& holder)
{
  GetImpl().BindViewHolder(holder);
}

void ItemAdapter::RecycleViewHolder(ItemViewHolder& holder)
{
  GetImpl().RecycleViewHolder(holder);
}

} // namespace Ui
} // namespace Dali
