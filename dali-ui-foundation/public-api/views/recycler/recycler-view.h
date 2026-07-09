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
#include <dali-ui-foundation/public-api/views/recycler/item-adapter.h>
#include <dali-ui-foundation/public-api/views/recycler/item-decoration.h>
#include <dali-ui-foundation/public-api/views/recycler/items-layouter.h>
#include <dali-ui-foundation/public-api/views/scroll/edge-effect.h>
#include <dali-ui-foundation/public-api/views/scroll/scrollable-enum.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{
class RecyclerViewImpl;
}

/**
 * @brief Virtualized scrollable item view.
 *
 * RecyclerView keeps only the item views required for the current viewport and
 * cache area. It scrolls an internal content view using DALi render animation
 * and recycles item views as the relative viewport anchor moves.
 */
class DALI_UI_API RecyclerView : public View
{
public:
  RecyclerView();
  static RecyclerView New();

  RecyclerView(const RecyclerView& recyclerView);
  RecyclerView(RecyclerView&& rhs) noexcept;
  ~RecyclerView();

  RecyclerView& operator=(const RecyclerView& handle);
  RecyclerView& operator=(RecyclerView&& rhs) noexcept;

  DALI_UI_VIEW_WITH(RecyclerView)

  static RecyclerView DownCast(BaseHandle handle);

  void        SetAdapter(ItemAdapter adapter);
  ItemAdapter GetAdapter() const;
  void        ClearAdapter();

  void          SetItemsLayouter(ItemsLayouter layouter);
  ItemsLayouter GetItemsLayouter() const;

  void SetCacheExtent(float before, float after);
  void GetCacheExtent(float& before, float& after) const;

  void  ScrollToPosition(uint32_t position, bool animation = true);
  void  ScrollBy(float distance, bool animation = true);
  void  SetScrollOffset(float offset);
  float GetScrollOffset() const;

  uint32_t GetFirstVisiblePosition() const;
  uint32_t GetLastVisiblePosition() const;

  void                SetVerticalScrollBarVisibility(ScrollBarVisibility visibility);
  ScrollBarVisibility GetVerticalScrollBarVisibility() const;

  void                SetHorizontalScrollBarVisibility(ScrollBarVisibility visibility);
  ScrollBarVisibility GetHorizontalScrollBarVisibility() const;

  void           SetOverScrollMode(OverScrollMode mode);
  OverScrollMode GetOverScrollMode() const;

  void       SetStartEdgeEffect(EdgeEffect effect);
  EdgeEffect GetStartEdgeEffect() const;

  void       SetEndEdgeEffect(EdgeEffect effect);
  EdgeEffect GetEndEdgeEffect() const;

  bool IsScrolling() const;

  // Key-scroll configuration — mirrors ScrollView API.
  void  SetKeyScrollEnabled(bool enable);
  bool  IsKeyScrollEnabled() const;
  void  SetKeyScrollStep(float step);
  float GetKeyScrollStep() const;

  // Focus-scroll configuration — scroll to make focused item visible.
  void  SetScrollOnFocus(bool enable);
  bool  GetScrollOnFocus() const;
  void  SetFocusScrollPeek(float peek);
  float GetFocusScrollPeek() const;

  // Item decoration — add overlay/background views around items.
  // RecyclerView auto-removes the decoration when it is destroyed (via DestroyedSignal),
  // so manual RemoveItemDecoration is optional. Adding the same decoration twice is a no-op
  // (logged as an error). Do not destroy a decoration from inside its own callbacks.
  void AddItemDecoration(ItemDecoration& decoration);
  void RemoveItemDecoration(ItemDecoration& decoration);

public: // Scroll state signals — match ScrollView API
  using ScrollStartedSignalType  = Signal<void(RecyclerView)>;
  using ScrollFinishedSignalType = Signal<void(RecyclerView)>;
  using DragStartedSignalType    = Signal<void(RecyclerView)>;
  using DragFinishedSignalType   = Signal<void(RecyclerView)>;

  ScrollStartedSignalType&  ScrollStartedSignal();
  ScrollFinishedSignalType& ScrollFinishedSignal();
  DragStartedSignalType&    DragStartedSignal();
  DragFinishedSignalType&   DragFinishedSignal();

public: // Not intended for application developers
  /// @cond internal
  explicit RecyclerView(Integration::RecyclerViewImpl& implementation);
  explicit RecyclerView(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
