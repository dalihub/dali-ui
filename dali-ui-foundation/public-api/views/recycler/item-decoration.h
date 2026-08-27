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
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/views/recycler/item-view-holder.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Optional capability for decorations whose configuration changes affect layout.
 *
 * This interface is separate from ItemDecoration so future lifecycle and
 * invalidation capabilities do not require changing ItemDecoration's vtable.
 */
class DALI_UI_API ItemDecorationInvalidationSupport
{
public:
  virtual ~ItemDecorationInvalidationSupport() = default;

  virtual Dali::Signal<void()>& LayoutInvalidatedSignal() = 0;
};

/**
 * @brief Per-item offsets returned by ItemDecoration::GetItemOffsets.
 *
 * Equivalent to Android's RecyclerView.ItemDecoration.getItemOffsets() Rect.
 * For a vertical list: top/bottom add space in the scroll direction; left/right
 * add space along the cross axis.
 *
 * Multiple decorations' offsets are summed before being applied to each item.
 * The item view is placed inside the offset area; the total slot consumed in the
 * layout axis is (top + itemHeight + bottom) for VERTICAL (or left + itemWidth + right
 * for HORIZONTAL).
 *
 * If a decoration changes its offsets at runtime, the caller is responsible for
 * triggering a RecyclerView layout pass to pick up the new values.
 */
struct DALI_UI_API ItemOffsets
{
  float left{0.0f};
  float top{0.0f};
  float right{0.0f};
  float bottom{0.0f};
};

/**
 * @brief Observer interface for item lifetime and layout events in RecyclerView.
 *
 * Implement this to draw decorative content (backgrounds, dividers, shadows)
 * around items or to update auxiliary views that track item positions.
 * RecyclerView calls these methods synchronously on the main thread.
 *
 * All methods have default no-op implementations so subclasses only need to
 * override what they use.
 *
 * Add/remove via RecyclerView::AddItemDecoration / RemoveItemDecoration.
 * RecyclerView tracks lifetime via DestroyedSignal and auto-removes the
 * decoration when it is destroyed, so manual RemoveItemDecoration is optional.
 * All callbacks are synchronous on the main thread — do not destroy the
 * decoration object from inside any of its own callbacks.
 *
 * @note This is a public subclassing ABI. Its virtual function set and object
 * layout are frozen after the first release. Future optional decoration hooks
 * must be introduced through a separate versioned extension interface rather
 * than by adding virtual functions or data members to this class.
 */
class DALI_UI_API ItemDecoration
{
public:
  virtual ~ItemDecoration();

  // Emitted in the destructor. RecyclerView connects to this to auto-remove
  // the decoration from its list without requiring manual RemoveItemDecoration.
  Dali::Signal<void(ItemDecoration&)>& DestroyedSignal();

  /**
   * @brief Returns the offsets this decoration wants to reserve around @a holder.
   *
   * Called by the layouter for every item during each layout and scroll pass,
   * before the item is positioned. Multiple decorations' results are summed.
   * The default returns zero offsets (no extra space).
   *
   * Equivalent to Android's RecyclerView.ItemDecoration.getItemOffsets().
   *
   * @param holder  The item being laid out (position, rowType, groupIndex populated).
   * @return Offsets in pixels to reserve around the item.
   */
  virtual ItemOffsets GetItemOffsets(const ItemViewHolder& holder) const
  {
    return {};
  }

  /**
   * @brief Called once at the start of each decoration bounds update pass.
   *
   * Called before any OnItemBoundsUpdated calls for the current layout pass.
   * Use this to reset per-frame tracking state (e.g. running min/max accumulators).
   */
  virtual void OnLayoutStart()
  {
  }

  /**
   * @brief Called after an item view is added to the scroller and bound.
   * @param holder  The new active item.
   * @param scroller  The scroll container; add decoration views here.
   */
  virtual void OnItemActivated(ItemViewHolder& holder, View scroller)
  {
  }

  /**
   * @brief Called before an item view is removed from the scroller.
   * @param holder  The item being recycled (holder.view is still valid).
   */
  virtual void OnItemRecycled(const ItemViewHolder& holder)
  {
  }

  /**
   * @brief Called for every active item after each layout or incremental scroll pass.
   *
   * @param holder  Active item (bounds are current as of this call).
   * @param bounds  Slot bounds in scroller-local content coordinates.
   *                The slot is the full space reserved for the item including decoration
   *                offsets (GetItemOffsets). For a VERTICAL list: x=0, width=full cross
   *                extent, y=slot start (before top offset), height=total slot height.
   *                The actual item view occupies the interior of this slot.
   */
  virtual void OnItemBoundsUpdated(const ItemViewHolder& holder, const LayoutRect& bounds)
  {
  }

private:
  Dali::Signal<void(ItemDecoration&)> mDestroyedSignal;
};

} // namespace Ui
} // namespace Dali
