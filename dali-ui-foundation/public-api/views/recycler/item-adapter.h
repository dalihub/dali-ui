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

#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/views/recycler/item-view-holder.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>
#include <cstdint>

namespace Dali
{
namespace Ui
{

class ItemAdapterImpl;

/**
 * @brief Application-supplied source of item views for RecyclerView.
 *
 * ItemAdapter is a reference-counted handle (BaseHandle). Multiple handles may
 * refer to the same underlying adapter. The adapter is destroyed when all handles
 * go out of scope. Create via ItemAdapter::New().
 *
 * Applications connect handlers to its signals rather than subclassing.
 *
 * Required signals: GetItemCountSignal, CreateViewHolderSignal, BindViewHolderSignal.
 * Optional signals: GetItemViewTypeSignal, RecycleViewHolderSignal.
 *
 * CreateViewHolderSignal handler must populate holder.view before returning.
 */
class DALI_UI_API ItemAdapter : public Dali::BaseHandle
{
public:
  // Fine-grained data change notifications.
  enum class ChangeType
  {
    FULL,            ///< All data changed — full relayout
    CONTENT_CHANGED, ///< Items changed; sizes unchanged — rebind only
    CHANGED,         ///< Items changed; sizes may differ — rebind + relayout
    INSERTED,        ///< count items inserted starting at position
    REMOVED,         ///< count items removed starting at position
    MOVED,           ///< Item at position moved to toPosition
    REPLACED,        ///< Adapter implementation changed; discard recycled views
  };

  struct ChangeInfo
  {
    ChangeType changeType{ChangeType::FULL};
    uint32_t   position{0u};
    uint32_t   count{1u};
    uint32_t   toPosition{0u}; // MOVED only
  };

  /**
   * @brief Creates a new adapter. Returns an initialized handle.
   */
  static ItemAdapter New();

  static ItemAdapter DownCast(BaseHandle handle);

  /**
   * @brief Creates an uninitialized (null) handle. Evaluates to false.
   */
  ItemAdapter() = default;

  ~ItemAdapter() = default;

  ItemAdapter(const ItemAdapter&)            = default;
  ItemAdapter(ItemAdapter&&)                 = default;
  ItemAdapter& operator=(const ItemAdapter&) = default;
  ItemAdapter& operator=(ItemAdapter&&)      = default;

  // Required operation signals.
  Dali::Signal<uint32_t()>&            GetItemCountSignal();
  Dali::Signal<void(ItemViewHolder&)>& CreateViewHolderSignal();
  Dali::Signal<void(ItemViewHolder&)>& BindViewHolderSignal();

  // Optional operation signals — RecyclerView uses defaults when not connected.
  Dali::Signal<uint32_t(uint32_t)>&    GetItemViewTypeSignal();
  Dali::Signal<void(ItemViewHolder&)>& RecycleViewHolderSignal();

  // Fine-grained data change notification signal.
  Dali::Signal<void(const ChangeInfo&)>& DataChangedSignal();

  void NotifyDataSetChanged();
  // Item data and size changed — triggers relayout. Use when item height/width may differ.
  void NotifyItemChanged(uint32_t position, uint32_t count = 1u);
  // Item data changed but size is guaranteed unchanged — rebind only, no relayout.
  void NotifyItemContentChanged(uint32_t position, uint32_t count = 1u);
  void NotifyItemInserted(uint32_t position, uint32_t count = 1u);
  void NotifyItemRemoved(uint32_t position, uint32_t count = 1u);
  void NotifyItemMoved(uint32_t fromPosition, uint32_t toPosition);

  // Called by RecyclerView internals.
  uint32_t GetItemCount() const;
  uint32_t GetItemViewType(uint32_t position) const;
  void     CreateViewHolder(ItemViewHolder& holder);
  void     BindViewHolder(ItemViewHolder& holder);
  void     RecycleViewHolder(ItemViewHolder& holder);

public: // Not intended for application developers
  /// @cond internal
  explicit ItemAdapter(ItemAdapterImpl* impl);

  ItemAdapterImpl&       GetImpl();
  const ItemAdapterImpl& GetImpl() const;
  /// @endcond
};

} // namespace Ui
} // namespace Dali
