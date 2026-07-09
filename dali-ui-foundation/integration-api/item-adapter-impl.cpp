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

namespace Dali
{
namespace Ui
{

ItemAdapterImplPtr ItemAdapterImpl::New()
{
  return new ItemAdapterImpl();
}

Signal<uint32_t()>& ItemAdapterImpl::GetItemCountSignal()
{
  return mGetItemCount;
}

Signal<void(ItemViewHolder&)>& ItemAdapterImpl::CreateViewHolderSignal()
{
  return mCreateViewHolder;
}

Signal<void(ItemViewHolder&)>& ItemAdapterImpl::BindViewHolderSignal()
{
  return mBindViewHolder;
}

Signal<uint32_t(uint32_t)>& ItemAdapterImpl::GetItemViewTypeSignal()
{
  return mGetItemViewType;
}

Signal<void(ItemViewHolder&)>& ItemAdapterImpl::RecycleViewHolderSignal()
{
  return mRecycleViewHolder;
}

Signal<void(const ItemAdapter::ChangeInfo&)>& ItemAdapterImpl::DataChangedSignal()
{
  return mDataChanged;
}

uint32_t ItemAdapterImpl::GetItemCount() const
{
  return mGetItemCount.Emit();
}

uint32_t ItemAdapterImpl::GetItemViewType(uint32_t position) const
{
  if(mGetItemViewType.GetConnectionCount() > 0u)
  {
    return mGetItemViewType.Emit(position);
  }
  return 0u;
}

void ItemAdapterImpl::CreateViewHolder(ItemViewHolder& holder)
{
  mCreateViewHolder.Emit(holder);
}

void ItemAdapterImpl::BindViewHolder(ItemViewHolder& holder)
{
  mBindViewHolder.Emit(holder);
}

void ItemAdapterImpl::RecycleViewHolder(ItemViewHolder& holder)
{
  if(mRecycleViewHolder.GetConnectionCount() > 0u)
  {
    mRecycleViewHolder.Emit(holder);
  }
}

void ItemAdapterImpl::NotifyDataSetChanged()
{
  mDataChanged.Emit(ItemAdapter::ChangeInfo{ItemAdapter::ChangeType::FULL, 0u, 0u, 0u});
}

void ItemAdapterImpl::NotifyItemChanged(uint32_t position, uint32_t count)
{
  mDataChanged.Emit(ItemAdapter::ChangeInfo{ItemAdapter::ChangeType::CHANGED, position, count, 0u});
}

void ItemAdapterImpl::NotifyItemContentChanged(uint32_t position, uint32_t count)
{
  mDataChanged.Emit(ItemAdapter::ChangeInfo{ItemAdapter::ChangeType::CONTENT_CHANGED, position, count, 0u});
}

void ItemAdapterImpl::NotifyItemInserted(uint32_t position, uint32_t count)
{
  mDataChanged.Emit(ItemAdapter::ChangeInfo{ItemAdapter::ChangeType::INSERTED, position, count, 0u});
}

void ItemAdapterImpl::NotifyItemRemoved(uint32_t position, uint32_t count)
{
  mDataChanged.Emit(ItemAdapter::ChangeInfo{ItemAdapter::ChangeType::REMOVED, position, count, 0u});
}

void ItemAdapterImpl::NotifyItemMoved(uint32_t fromPosition, uint32_t toPosition)
{
  mDataChanged.Emit(ItemAdapter::ChangeInfo{ItemAdapter::ChangeType::MOVED, fromPosition, 1u, toPosition});
}

void ItemAdapterImpl::SetInternalKeepAlive(Dali::BaseHandle keepAlive)
{
  mInternalKeepAlive = std::move(keepAlive);
}

void ItemAdapterImpl::NotifyAdapterReplaced()
{
  mDataChanged.Emit(ItemAdapter::ChangeInfo{ItemAdapter::ChangeType::REPLACED, 0u, 0u, 0u});
}

} // namespace Ui
} // namespace Dali
