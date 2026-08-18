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

#include <dali-ui-foundation/integration-api/items-layouter-impl.h>
#include <dali/public-api/common/dali-vector.h>
#include <cstdint>

namespace Dali
{
namespace Ui
{

namespace Internal
{

class LinearItemsLayouterImpl;
using LinearItemsLayouterImplPtr = IntrusivePtr<LinearItemsLayouterImpl>;

/**
 * @brief Concrete implementation for LinearItemsLayouter.
 *
 * Implements a single-axis (vertical or horizontal) list layout.
 * Fill and LayoutChunk are non-virtual methods of this end class —
 * they are not part of the ItemsLayouterImpl interface and cannot
 * be overridden.
 */
class DALI_UI_API LinearItemsLayouterImpl : public Integration::ItemsLayouterImpl
{
public:
  static LinearItemsLayouterImplPtr New(Orientation orientation);

  explicit LinearItemsLayouterImpl(Orientation orientation);
  ~LinearItemsLayouterImpl() override;

  // ItemsLayouterImpl overrides
  void  OnLayoutChildren(Recycler& recycler) override;
  float ScrollVerticallyBy(float dy, Recycler& recycler) override;
  float ScrollHorizontallyBy(float dx, Recycler& recycler) override;
  bool  CanScrollVertically() const override;
  bool  CanScrollHorizontally() const override;
  float ComputeScrollOffset() const override;
  float ComputeScrollExtent() const override;
  float ComputeScrollRange() const override;

  uint32_t   GetFirstVisiblePosition() const override;
  uint32_t   GetLastVisiblePosition() const override;
  LayoutRect GetItemBounds(uint32_t position, float crossExtent) const override;

  Orientation GetOrientation() const override;
  void        SetOrientation(Orientation orientation) override;
  void        SetItemExtent(float extent) override;
  float       GetItemExtent() const override;
  void        SetItemSpacing(float spacing) override;
  float       GetItemSpacing() const override;

  void OnAdapterChanged() override;

private:
  struct LayoutState
  {
    uint32_t nextPosition{0u};
    uint32_t endPosition{0u};     // exclusive upper bound
    float    currentOffset{0.0f}; // running main-axis accumulator
  };

  // Non-virtual end-class methods: not part of ItemsLayouterImpl interface.
  void Fill(LayoutState& state, Recycler& recycler);
  bool LayoutChunk(Recycler& recycler, LayoutState& state);
  void MeasureUpdate(uint32_t position, float extent);

  float GetItemExtentAt(uint32_t position) const;
  float GetItemOffset(uint32_t position) const;
  // Walks forward (targetPos > anchorPos) or backward using cached extents.
  float    GetItemOffsetFrom(uint32_t anchorPos, float anchorOffset, uint32_t targetPos) const;
  float    GetEstimatedContentExtent(uint32_t itemCount) const;
  uint32_t FindFirstVisiblePosition(float scrollOffset, float cacheBefore, uint32_t itemCount) const;
  uint32_t FindLastVisiblePosition(float scrollOffset, float viewportExtent, float cacheAfter, uint32_t itemCount) const;
  float    ScrollBy(float delta, Recycler& recycler);

  Orientation         mOrientation;
  float               mItemExtent;
  float               mItemSpacing;
  float               mScrollOffset;
  float               mViewportExtent;
  float               mCrossExtent;
  float               mCacheBefore;
  float               mCacheAfter;
  uint32_t            mCachedItemCount;
  Dali::Vector<float> mExtentCache; // 0.0f = not yet measured

  // Active window tracking.
  uint32_t mFirstActive;
  uint32_t mLastActive;
  bool     mHasActiveItems;

  // Main-axis offset of mFirstActive (start of first active item).
  float mFirstActiveOffset;
  // Main-axis offset just past mLastActive (start of next potential item, including spacing).
  float mLastActiveEndOffset;

  // Running statistics for O(1) GetEstimatedContentExtent.
  float    mTotalMeasuredExtent; // sum of extents for measured items
  uint32_t mMeasuredItemCount;   // count of items measured so far
};

} // namespace Internal

} // namespace Ui
} // namespace Dali
