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

#include <dali-ui-foundation/integration-api/recycler.h>
#include <dali-ui-foundation/internal/linear-items-layouter-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali/integration-api/debug.h>
#include <algorithm>
#include <cmath>
#include <limits>

namespace DALI_NAMESPACE
{
namespace Ui
{

namespace Internal
{
namespace
{
// A scroll settles in one window pass, or two when measurement moves an edge; later passes only
// follow measurement that moved it again, and the bound is never reached in practice.
constexpr uint32_t MAX_SETTLE_PASSES = 8u;
constexpr float    SETTLE_EPSILON    = 0.001f;
} // namespace

LinearItemsLayouterImplPtr LinearItemsLayouterImpl::New(Orientation orientation)
{
  return new LinearItemsLayouterImpl(orientation);
}

LinearItemsLayouterImpl::LinearItemsLayouterImpl(Orientation orientation)
: mOrientation(orientation),
  mItemExtent(64.0f),
  mItemSpacing(0.0f),
  mScrollOffset(0.0f),
  mViewportExtent(0.0f),
  mCrossExtent(0.0f),
  mCacheBefore(0.0f),
  mCacheAfter(0.0f),
  mCachedItemCount(0u),
  mFirstActive(0u),
  mLastActive(0u),
  mHasActiveItems(false),
  mFirstActiveOffset(0.0f),
  mLastActiveEndOffset(0.0f),
  mTotalMeasuredExtent(0.0f),
  mMeasuredItemCount(0u)
{
}

LinearItemsLayouterImpl::~LinearItemsLayouterImpl() = default;

// ---------------------------------------------------------------------------
// ItemsLayouterImpl overrides
// ---------------------------------------------------------------------------

void LinearItemsLayouterImpl::OnLayoutChildren(Recycler& recycler)
{
  const uint32_t itemCount = recycler.GetItemCount();
  mCachedItemCount         = itemCount;
  mViewportExtent          = recycler.GetViewportExtent();
  mCrossExtent             = recycler.GetCrossExtent();
  mCacheBefore             = recycler.GetCacheBefore();
  mCacheAfter              = recycler.GetCacheAfter();

  if(itemCount == 0u || mViewportExtent <= 0.0f)
  {
    recycler.RecycleAllViews();
    mHasActiveItems      = false;
    mFirstActiveOffset   = 0.0f;
    mLastActiveEndOffset = 0.0f;
    return;
  }

  // Rebuild every window item from the saved anchor (O(|first - anchor|) after a simple resize),
  // re-measuring each. Nothing above the window is measured here, so the anchor stays exact; a
  // re-measured end of the content is settled like a scroll.
  SettleBy(0.0f, recycler, itemCount, true);
}

float LinearItemsLayouterImpl::ScrollVerticallyBy(float dy, Recycler& recycler)
{
  if(!CanScrollVertically())
  {
    return 0.0f;
  }
  return ScrollBy(dy, recycler);
}

float LinearItemsLayouterImpl::ScrollHorizontallyBy(float dx, Recycler& recycler)
{
  if(!CanScrollHorizontally())
  {
    return 0.0f;
  }
  return ScrollBy(dx, recycler);
}

bool LinearItemsLayouterImpl::CanScrollVertically() const
{
  return mOrientation == Orientation::VERTICAL;
}

bool LinearItemsLayouterImpl::CanScrollHorizontally() const
{
  return mOrientation == Orientation::HORIZONTAL;
}

float LinearItemsLayouterImpl::ComputeScrollOffset() const
{
  return mScrollOffset;
}

float LinearItemsLayouterImpl::ComputeScrollExtent() const
{
  return mViewportExtent;
}

float LinearItemsLayouterImpl::ComputeScrollRange() const
{
  return GetEstimatedContentExtent(mCachedItemCount);
}

uint32_t LinearItemsLayouterImpl::GetFirstVisiblePosition() const
{
  return mHasActiveItems ? mFirstActive : 0u;
}

uint32_t LinearItemsLayouterImpl::GetLastVisiblePosition() const
{
  return mHasActiveItems ? mLastActive : 0u;
}

LayoutRect LinearItemsLayouterImpl::GetItemBounds(uint32_t position, float crossExtent) const
{
  const float main     = GetItemOffset(position);
  const float mainSize = GetItemExtentAt(position);
  if(mOrientation == Orientation::VERTICAL)
  {
    return LayoutRect(0.0f, main, crossExtent, mainSize);
  }
  return LayoutRect(main, 0.0f, mainSize, crossExtent);
}

Integration::ItemsLayouterImpl::Orientation LinearItemsLayouterImpl::GetOrientation() const
{
  return mOrientation;
}

void LinearItemsLayouterImpl::SetOrientation(Orientation orientation)
{
  if(mOrientation == orientation)
  {
    return;
  }
  mOrientation = orientation;
  // Extents measured in old axis are invalid for new axis.
  mExtentCache.Clear();
  mTotalMeasuredExtent = 0.0f;
  mMeasuredItemCount   = 0u;
  mScrollOffset        = 0.0f;
  mFirstActiveOffset   = 0.0f;
  mLastActiveEndOffset = 0.0f;
  mHasActiveItems      = false;
  InvalidateLayout();
}

void LinearItemsLayouterImpl::SetItemExtent(float extent)
{
  mItemExtent = std::max(1.0f, extent);
  // All cached extents and derived offsets are invalid.
  mExtentCache.Clear();
  mTotalMeasuredExtent = 0.0f;
  mMeasuredItemCount   = 0u;
  mFirstActiveOffset   = 0.0f;
  mLastActiveEndOffset = 0.0f;
  mHasActiveItems      = false;
  InvalidateLayout();
}

float LinearItemsLayouterImpl::GetItemExtent() const
{
  return mItemExtent;
}

void LinearItemsLayouterImpl::SetItemSpacing(float spacing)
{
  mItemSpacing = std::max(0.0f, spacing);
  // Extents are still valid but all computed offsets are wrong.
  mFirstActiveOffset   = 0.0f;
  mLastActiveEndOffset = 0.0f;
  mHasActiveItems      = false;
  InvalidateLayout();
}

float LinearItemsLayouterImpl::GetItemSpacing() const
{
  return mItemSpacing;
}

void LinearItemsLayouterImpl::OnAdapterChanged()
{
  mExtentCache.Clear();
  mScrollOffset        = 0.0f;
  mCachedItemCount     = 0u;
  mHasActiveItems      = false;
  mFirstActive         = 0u;
  mLastActive          = 0u;
  mFirstActiveOffset   = 0.0f;
  mLastActiveEndOffset = 0.0f;
  mTotalMeasuredExtent = 0.0f;
  mMeasuredItemCount   = 0u;
}

void LinearItemsLayouterImpl::OnDecorationChanged()
{
  // Clear extent cache: cached slot sizes include decoration offsets and are now stale.
  // Scroll offset is preserved (items haven't changed, only their surrounding space).
  mExtentCache.Clear();
  mTotalMeasuredExtent = 0.0f;
  mMeasuredItemCount   = 0u;
  mFirstActiveOffset   = 0.0f;
  mLastActiveEndOffset = 0.0f;
  mHasActiveItems      = false;
  InvalidateLayout();
}

LinearItemsLayouterImpl::ItemInset LinearItemsLayouterImpl::GetItemCrossInset(uint32_t /*position*/) const
{
  return {};
}

// ---------------------------------------------------------------------------
// Non-virtual end-class methods: Fill / LayoutChunk / MeasureUpdate
// ---------------------------------------------------------------------------

void LinearItemsLayouterImpl::Fill(LayoutState& state, Recycler& recycler)
{
  while(state.nextPosition < state.endPosition && state.currentOffset < state.limitOffset)
  {
    if(!LayoutChunk(recycler, state))
    {
      break;
    }
  }
}

bool LinearItemsLayouterImpl::LayoutChunk(Recycler& recycler, LayoutState& state)
{
  const uint32_t pos = state.nextPosition;
  if(pos >= recycler.GetItemCount() || pos >= state.endPosition)
  {
    return false;
  }

  ItemSlot slot;
  if(!MeasureSlot(recycler, pos, slot))
  {
    return false;
  }

  const float main = state.currentOffset + slot.leading;
  slot.view.Arrange(mOrientation == Orientation::HORIZONTAL ? LayoutRect(main, slot.crossStart, slot.extent, slot.cross)
                                                            : LayoutRect(slot.crossStart, main, slot.cross, slot.extent));
  // Advance by the recorded slot extent: placement and GetItemOffset then use the same value.
  state.currentOffset += GetItemExtentAt(pos) + mItemSpacing;

  ++state.nextPosition;
  return true;
}

bool LinearItemsLayouterImpl::MeasureSlot(Recycler& recycler, uint32_t position, ItemSlot& slot)
{
  slot.view = recycler.GetViewForPosition(position);
  if(!slot.view)
  {
    DALI_LOG_ERROR("LinearItemsLayouterImpl: adapter returned null view for position %u\n", position);
    return false;
  }

  constexpr float kUnconstrained = std::numeric_limits<float>::max() * 0.25f;

  const ItemInset crossInset = GetItemCrossInset(position);
  const Insets    decoOffs   = recycler.GetDecorationOffsets(position);
  const bool      horizontal = (mOrientation == Orientation::HORIZONTAL);

  // Main axis = X with start/end offsets for a horizontal list, Y with top/bottom otherwise.
  slot.leading              = horizontal ? decoOffs.start : decoOffs.top;
  slot.trailing             = horizontal ? decoOffs.end : decoOffs.bottom;
  slot.crossStart           = crossInset.leadingCrossOffset + (horizontal ? decoOffs.top : decoOffs.start);
  const float trailingCross = crossInset.trailingCrossOffset + (horizontal ? decoOffs.bottom : decoOffs.end);
  slot.cross                = std::max(0.0f, mCrossExtent - slot.crossStart - trailingCross);

  const MeasuredSize measured = horizontal ? slot.view.Measure(kUnconstrained, slot.cross) : slot.view.Measure(slot.cross, kUnconstrained);
  slot.extent                 = horizontal ? measured.width : measured.height;
  MeasureUpdate(position, slot.leading + slot.extent + slot.trailing);
  return true;
}

void LinearItemsLayouterImpl::MeasureUpdate(uint32_t position, float extent)
{
  const float clamped = std::max(1.0f, extent);

  if(position >= mExtentCache.Size())
  {
    mExtentCache.Resize(position + 1u, 0.0f);
    mExtentCache[position] = clamped;
    mTotalMeasuredExtent += clamped;
    ++mMeasuredItemCount;
  }
  else if(mExtentCache[position] == 0.0f)
  {
    // First measurement for this position.
    mExtentCache[position] = clamped;
    mTotalMeasuredExtent += clamped;
    ++mMeasuredItemCount;
  }
  else if(mExtentCache[position] != clamped)
  {
    // Re-measure (e.g. after dynamic content change). An unchanged value leaves the total alone,
    // so that repeated re-measures do not accumulate rounding.
    mTotalMeasuredExtent += clamped - mExtentCache[position];
    mExtentCache[position] = clamped;
  }
}

void LinearItemsLayouterImpl::RecycleLeading(Recycler& recycler, uint32_t& first, uint32_t end, float& firstOffset, float windowStart)
{
  // Items that end at or above the window start leave the window. The last realised item stays: it
  // is the boundary an anchor placed past the end of shorter content glues upward from.
  while(first + 1u < end && firstOffset + GetItemExtentAt(first) <= windowStart)
  {
    recycler.RecycleViewForPosition(first);
    firstOffset += GetItemExtentAt(first) + mItemSpacing;
    ++first;
  }
}

// ---------------------------------------------------------------------------
// Geometry helpers
// ---------------------------------------------------------------------------

float LinearItemsLayouterImpl::GetItemExtentAt(uint32_t position) const
{
  if(position < mExtentCache.Size() && mExtentCache[position] > 0.0f)
  {
    return mExtentCache[position];
  }
  return mItemExtent;
}

float LinearItemsLayouterImpl::GetItemOffset(uint32_t position) const
{
  float offset = 0.0f;
  for(uint32_t i = 0u; i < position; ++i)
  {
    offset += GetItemExtentAt(i) + mItemSpacing;
  }
  return offset;
}

float LinearItemsLayouterImpl::GetItemOffsetFrom(uint32_t anchorPos, float anchorOffset, uint32_t targetPos) const
{
  if(targetPos == anchorPos)
  {
    return anchorOffset;
  }

  if(targetPos > anchorPos)
  {
    // Walk forward from anchor.
    float offset = anchorOffset;
    for(uint32_t i = anchorPos; i < targetPos; ++i)
    {
      offset += GetItemExtentAt(i) + mItemSpacing;
    }
    return offset;
  }

  // Walk backward from anchor.
  float offset = anchorOffset;
  for(uint32_t i = anchorPos; i > targetPos; --i)
  {
    offset -= GetItemExtentAt(i - 1u) + mItemSpacing;
  }
  return offset;
}

float LinearItemsLayouterImpl::GetEstimatedContentExtent(uint32_t itemCount) const
{
  if(itemCount == 0u)
  {
    return 0.0f;
  }

  // The per-item rule of GetItemExtentAt, kept in O(1): measured extents plus the estimate for every
  // item not measured yet. The content therefore ends exactly where the last item's offset says.
  const uint32_t unmeasured   = itemCount - std::min(itemCount, mMeasuredItemCount);
  const float    totalExtent  = mTotalMeasuredExtent + static_cast<float>(unmeasured) * mItemExtent;
  const float    totalSpacing = mItemSpacing * static_cast<float>(itemCount - 1u);

  return totalExtent + totalSpacing;
}

float LinearItemsLayouterImpl::ClampScrollOffset(float offset, uint32_t itemCount) const
{
  return std::min(std::max(0.0f, offset), std::max(0.0f, GetEstimatedContentExtent(itemCount) - mViewportExtent));
}

uint32_t LinearItemsLayouterImpl::FindFirstVisiblePosition(float scrollOffset, float cacheBefore, uint32_t itemCount) const
{
  if(itemCount == 0u)
  {
    return 0u;
  }
  const float start  = std::max(0.0f, scrollOffset - cacheBefore);
  float       cursor = 0.0f;
  for(uint32_t i = 0u; i < itemCount; ++i)
  {
    const float end = cursor + GetItemExtentAt(i);
    if(end > start)
    {
      return i;
    }
    cursor = end + mItemSpacing;
  }
  return itemCount - 1u;
}

float LinearItemsLayouterImpl::ScrollBy(float delta, Recycler& recycler)
{
  const uint32_t itemCount = recycler.GetItemCount();
  mCachedItemCount         = itemCount;
  mViewportExtent          = recycler.GetViewportExtent();
  mCrossExtent             = recycler.GetCrossExtent();
  mCacheBefore             = recycler.GetCacheBefore();
  mCacheAfter              = recycler.GetCacheAfter();

  if(itemCount == 0u || mViewportExtent <= 0.0f)
  {
    return 0.0f;
  }

  return SettleBy(delta, recycler, itemCount, false);
}

float LinearItemsLayouterImpl::SettleBy(float delta, Recycler& recycler, uint32_t itemCount, bool rebuild)
{
  // Measuring can move the end of the content, and a correction moves the coordinates the request
  // was made in, so the window pass repeats until the clamped target stops moving. A correction is
  // carried in the request but is not scroll.
  const float origin     = mScrollOffset;
  const bool  inRange    = (ClampScrollOffset(origin + delta, itemCount) == origin + delta);
  float       shift      = 0.0f;
  float       correction = 0.0f;
  for(uint32_t pass = 0u; pass < MAX_SETTLE_PASSES; ++pass)
  {
    const float request = origin + shift + delta;
    const float target  = ClampScrollOffset(request, itemCount);
    // A pass that corrected is followed by one more, which drops what the correction moved out.
    if(pass > 0u && correction == 0.0f && std::abs(target - mScrollOffset) <= SETTLE_EPSILON)
    {
      break;
    }
    mScrollOffset = target;
    correction    = LayoutWindow(recycler, itemCount, rebuild && pass == 0u, request);
    shift += correction;
  }

  // A request inside the content as estimated is consumed whole: an edge that measurement moved
  // toward it is not overscroll. Past an edge, report the movement, within the request.
  const float moved = mScrollOffset - shift - origin;
  return inRange ? delta : std::min(std::max(moved, std::min(0.0f, delta)), std::max(0.0f, delta));
}

float LinearItemsLayouterImpl::LayoutWindow(Recycler& recycler, uint32_t itemCount, bool rebuild, float request)
{
  const float windowStart = std::max(0.0f, mScrollOffset - mCacheBefore);
  const float windowEnd   = mScrollOffset + mViewportExtent + mCacheAfter;

  uint32_t first    = mFirstActive;
  uint32_t end      = mLastActive + 1u;
  float    firstOff = mFirstActiveOffset;
  float    endOff   = mLastActiveEndOffset;

  // A window with no item in the new range has no surviving boundary: filling from it would
  // materialize the skipped gap. Rebuild from the first item the estimated offsets place in the
  // range; its offset is exact because anchor coordinates always equal those offsets.
  const bool rebuilt = rebuild || !mHasActiveItems || endOff - mItemSpacing <= windowStart || firstOff >= windowEnd;
  if(rebuilt)
  {
    const uint32_t anchor = FindFirstVisiblePosition(mScrollOffset, mCacheBefore, itemCount);
    firstOff              = mHasActiveItems ? GetItemOffsetFrom(mFirstActive, mFirstActiveOffset, anchor) : GetItemOffset(anchor);
    recycler.RecycleAllViews();
    first  = anchor;
    end    = anchor;
    endOff = firstOff;
  }
  else
  {
    RecycleLeading(recycler, first, end, firstOff, windowStart);
    // Items that start at or below the window end leave it as well.
    while(end > first && endOff - mItemSpacing - GetItemExtentAt(end - 1u) >= windowEnd)
    {
      --end;
      recycler.RecycleViewForPosition(end);
      endOff -= GetItemExtentAt(end) + mItemSpacing;
    }
  }

  // Below: each item starts where the previous measured one ends. A request past the end fills
  // to the last item, so that the end of the content is measured rather than estimated.
  LayoutState below;
  below.nextPosition  = end;
  below.endPosition   = itemCount;
  below.currentOffset = endOff;
  below.limitOffset   = (request > mScrollOffset) ? std::numeric_limits<float>::max() : windowEnd;
  Fill(below, recycler);
  end    = below.nextPosition;
  endOff = below.currentOffset;
  if(rebuilt)
  {
    // The anchor was chosen with estimated extents: drop leading items that measured short.
    RecycleLeading(recycler, first, end, firstOff, windowStart);
  }

  // Above: glue upward from the first item's top edge with measured extents, so the boundary
  // neither opens nor overlaps. Every glued item moves each offset below it by its measured minus
  // its previously assumed extent; the window top is compared in those corrected coordinates, and
  // a request that reaches the start glues up to the first item.
  const uint32_t survivors  = first;
  float          start      = firstOff;
  float          correction = 0.0f;
  ItemSlot       slot;
  while(first > 0u)
  {
    const float windowTop = (request <= 0.0f) ? 0.0f : std::max(0.0f, mScrollOffset + correction - mCacheBefore);
    const float assumed   = GetItemExtentAt(first - 1u);
    if(start + correction - mItemSpacing <= windowTop || !MeasureSlot(recycler, first - 1u, slot))
    {
      break;
    }
    --first;
    start -= GetItemExtentAt(first) + mItemSpacing;
    correction += GetItemExtentAt(first) - assumed;
  }
  if(first < survivors)
  {
    // Arrange top-down from the corrected start: the glued items and, when the correction moved
    // the anchor, every item below them, so that each realised item sits at its item offset.
    LayoutState above;
    above.nextPosition  = first;
    above.endPosition   = (correction != 0.0f) ? end : survivors;
    above.currentOffset = start + correction;
    Fill(above, recycler);
    firstOff = start + correction;
    endOff   = (correction != 0.0f) ? above.currentOffset : endOff;
  }

  mFirstActive         = first;
  mFirstActiveOffset   = firstOff;
  mLastActive          = end - 1u;
  mLastActiveEndOffset = endOff;
  mHasActiveItems      = (first < end);
  // The visible items keep their screen position: the offset moves with them.
  mScrollOffset += correction;
  return correction;
}

} // namespace Internal

} // namespace Ui
} //namespace DALI_NAMESPACE
