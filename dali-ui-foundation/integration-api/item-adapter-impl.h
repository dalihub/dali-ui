#pragma once

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

// Include item-adapter.h for ChangeInfo/ChangeType nested types.
// item-adapter.h only forward-declares ItemAdapterImpl, so there is no circular dependency.
#include <dali-ui-foundation/public-api/views/recycler/item-adapter.h>
#include <dali-ui-foundation/public-api/views/recycler/item-view-holder.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/dali-signal.h>
#include <cstdint>

namespace Dali
{
namespace Ui
{

class ItemAdapterImpl;
using ItemAdapterImplPtr = IntrusivePtr<ItemAdapterImpl>;

/**
 * @brief Reference-counted implementation object for ItemAdapter.
 *
 * Holds all signals. Application code connects to the signals via the
 * ItemAdapter handle; RecyclerView calls the Emit helpers directly.
 */
class DALI_UI_API ItemAdapterImpl : public Dali::BaseObject
{
public:
  static ItemAdapterImplPtr New();

  // Required operation signals.
  Dali::Signal<uint32_t()>&            GetItemCountSignal();
  Dali::Signal<void(ItemViewHolder&)>& CreateViewHolderSignal();
  Dali::Signal<void(ItemViewHolder&)>& BindViewHolderSignal();

  // Optional operation signals.
  Dali::Signal<uint32_t(uint32_t)>&    GetItemViewTypeSignal();
  Dali::Signal<void(ItemViewHolder&)>& RecycleViewHolderSignal();

  // Fine-grained change notification signal.
  Dali::Signal<void(const ItemAdapter::ChangeInfo&)>& DataChangedSignal();

  // Called by RecyclerView internals — emit the corresponding signal.
  uint32_t GetItemCount() const;
  uint32_t GetItemViewType(uint32_t position) const;
  void     CreateViewHolder(ItemViewHolder& holder);
  void     BindViewHolder(ItemViewHolder& holder);
  void     RecycleViewHolder(ItemViewHolder& holder);

  void NotifyDataSetChanged();
  void NotifyItemChanged(uint32_t position, uint32_t count);
  void NotifyItemContentChanged(uint32_t position, uint32_t count);
  void NotifyItemInserted(uint32_t position, uint32_t count);
  void NotifyItemRemoved(uint32_t position, uint32_t count);
  void NotifyItemMoved(uint32_t fromPosition, uint32_t toPosition);

protected:
  ItemAdapterImpl()           = default;
  ~ItemAdapterImpl() override = default;

private:
  friend class GroupAdapterImpl;

  void SetInternalKeepAlive(Dali::BaseHandle keepAlive);
  void NotifyAdapterReplaced();

  mutable Dali::Signal<uint32_t()>                           mGetItemCount;
  mutable Dali::Signal<void(ItemViewHolder&)>                mCreateViewHolder;
  mutable Dali::Signal<void(ItemViewHolder&)>                mBindViewHolder;
  mutable Dali::Signal<uint32_t(uint32_t)>                   mGetItemViewType;
  mutable Dali::Signal<void(ItemViewHolder&)>                mRecycleViewHolder;
  mutable Dali::Signal<void(const ItemAdapter::ChangeInfo&)> mDataChanged;
  Dali::BaseHandle                                           mInternalKeepAlive;
};

} // namespace Ui
} // namespace Dali
