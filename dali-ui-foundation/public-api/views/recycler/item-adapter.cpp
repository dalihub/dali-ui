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

#include <dali-ui-foundation/public-api/views/recycler/item-adapter.h>

namespace Dali
{
namespace Ui
{

struct ItemAdapter::Impl
{
  Signal<uint32_t()>                           getItemCount;
  Signal<View(uint32_t)>                       createItemView;
  Signal<void(View, uint32_t)>                 bindItemView;
  Signal<uint32_t(uint32_t)>                   getItemViewType;
  Signal<void(View, uint32_t)>                 itemViewRecycled;
  Signal<void(ItemAdapter&)>                   destroyed;
  Signal<void(const ItemAdapter::ChangeInfo&)> dataChanged;
};

ItemAdapter::ItemAdapter()
: mImpl(new Impl())
{
}

ItemAdapter::~ItemAdapter()
{
  mImpl->destroyed.Emit(*this);
  delete mImpl;
}

Signal<uint32_t()>& ItemAdapter::GetItemCountSignal()
{
  return mImpl->getItemCount;
}

Signal<View(uint32_t)>& ItemAdapter::CreateItemViewSignal()
{
  return mImpl->createItemView;
}

Signal<void(View, uint32_t)>& ItemAdapter::BindItemViewSignal()
{
  return mImpl->bindItemView;
}

Signal<uint32_t(uint32_t)>& ItemAdapter::GetItemViewTypeSignal()
{
  return mImpl->getItemViewType;
}

Signal<void(View, uint32_t)>& ItemAdapter::ItemViewRecycledSignal()
{
  return mImpl->itemViewRecycled;
}

uint32_t ItemAdapter::GetItemCount() const
{
  return mImpl->getItemCount.Emit();
}

uint32_t ItemAdapter::GetItemViewType(uint32_t position) const
{
  if(mImpl->getItemViewType.GetConnectionCount() > 0u)
  {
    return mImpl->getItemViewType.Emit(position);
  }
  return 0u;
}

View ItemAdapter::CreateItemView(uint32_t viewType)
{
  return mImpl->createItemView.Emit(viewType);
}

void ItemAdapter::BindItemView(View itemView, uint32_t position)
{
  mImpl->bindItemView.Emit(itemView, position);
}

void ItemAdapter::ItemViewRecycled(View itemView, uint32_t viewType)
{
  mImpl->itemViewRecycled.Emit(itemView, viewType);
}

Signal<void(ItemAdapter&)>& ItemAdapter::DestroyedSignal()
{
  return mImpl->destroyed;
}

void ItemAdapter::NotifyDataSetChanged()
{
  mImpl->dataChanged.Emit(ChangeInfo{ChangeType::FULL, 0u, 0u, 0u});
}

void ItemAdapter::NotifyItemChanged(uint32_t position, uint32_t count)
{
  mImpl->dataChanged.Emit(ChangeInfo{ChangeType::CHANGED, position, count, 0u});
}

void ItemAdapter::NotifyItemContentChanged(uint32_t position, uint32_t count)
{
  mImpl->dataChanged.Emit(ChangeInfo{ChangeType::CONTENT_CHANGED, position, count, 0u});
}

void ItemAdapter::NotifyItemInserted(uint32_t position, uint32_t count)
{
  mImpl->dataChanged.Emit(ChangeInfo{ChangeType::INSERTED, position, count, 0u});
}

void ItemAdapter::NotifyItemRemoved(uint32_t position, uint32_t count)
{
  mImpl->dataChanged.Emit(ChangeInfo{ChangeType::REMOVED, position, count, 0u});
}

void ItemAdapter::NotifyItemMoved(uint32_t fromPosition, uint32_t toPosition)
{
  mImpl->dataChanged.Emit(ChangeInfo{ChangeType::MOVED, fromPosition, 1u, toPosition});
}

Signal<void(const ItemAdapter::ChangeInfo&)>& ItemAdapter::DataChangedSignal()
{
  return mImpl->dataChanged;
}

ItemAdapter::Extension* ItemAdapter::GetExtension()
{
  return nullptr;
}

} // namespace Ui
} // namespace Dali
