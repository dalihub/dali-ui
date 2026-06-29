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
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali/public-api/signals/dali-signal.h>
#include <cstdint>

namespace Dali
{
namespace Ui
{

/**
 * @brief Application-supplied source of item views for RecyclerView.
 *
 * ItemAdapter is a concrete, non-virtual class. Applications connect
 * handlers to its signals rather than subclassing it. The opaque Impl
 * pointer keeps the class layout fixed across library updates so
 * compiled application code never needs to be rebuilt.
 *
 * Required signals: GetItemCountSignal, CreateItemViewSignal, BindItemViewSignal.
 * Optional signals: GetItemViewTypeSignal, ItemViewRecycledSignal.
 */
class DALI_UI_API ItemAdapter
{
public:
  class Extension;

  ItemAdapter();
  ~ItemAdapter();

  ItemAdapter(const ItemAdapter&)            = delete;
  ItemAdapter& operator=(const ItemAdapter&) = delete;

  // Required operation signals.
  Dali::Signal<uint32_t()>&           GetItemCountSignal();
  Dali::Signal<View(uint32_t)>&       CreateItemViewSignal();
  Dali::Signal<void(View, uint32_t)>& BindItemViewSignal();

  // Optional operation signals — RecyclerView uses defaults when not connected.
  Dali::Signal<uint32_t(uint32_t)>&   GetItemViewTypeSignal();
  Dali::Signal<void(View, uint32_t)>& ItemViewRecycledSignal();

  // Emitted just before the adapter is destroyed. Observers (e.g. RecyclerView)
  // should call ClearAdapter() in response to avoid dangling pointer access.
  Dali::Signal<void(ItemAdapter&)>& DestroyedSignal();

  // Fine-grained data change notifications.
  // RecyclerView uses these to decide how to update the layout efficiently.
  enum class ChangeType
  {
    FULL,            ///< All data changed — full relayout
    CONTENT_CHANGED, ///< Items changed; caller guarantees sizes are unchanged — rebind only
    CHANGED,         ///< Items changed; sizes may differ — rebind + relayout
    INSERTED,        ///< count items inserted starting at position
    REMOVED,         ///< count items removed starting at position
    MOVED,           ///< Item at position moved to toPosition
  };

  struct ChangeInfo
  {
    ChangeType changeType{ChangeType::FULL};
    uint32_t   position{0u};
    uint32_t   count{1u};
    uint32_t   toPosition{0u}; // MOVED only
  };

  void NotifyDataSetChanged();
  // Item data and size changed — triggers relayout. Use when item height/width may differ.
  void NotifyItemChanged(uint32_t position, uint32_t count = 1u);
  // Item data changed but size is guaranteed unchanged — rebind only, no relayout.
  void NotifyItemContentChanged(uint32_t position, uint32_t count = 1u);
  void NotifyItemInserted(uint32_t position, uint32_t count = 1u);
  void NotifyItemRemoved(uint32_t position, uint32_t count = 1u);
  void NotifyItemMoved(uint32_t fromPosition, uint32_t toPosition);

  Dali::Signal<void(const ChangeInfo&)>& DataChangedSignal();

  // Called by RecyclerView internals.
  uint32_t GetItemCount() const;
  uint32_t GetItemViewType(uint32_t position) const;
  View     CreateItemView(uint32_t viewType);
  void     BindItemView(View itemView, uint32_t position);
  void     ItemViewRecycled(View itemView, uint32_t viewType);

  Extension* GetExtension();

private:
  struct Impl;
  Impl* mImpl;
};

} // namespace Ui
} // namespace Dali
