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

namespace Dali
{
namespace Ui
{

namespace Internal
{

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
  // Save anchor before clearing active-item flag (used below for O(delta) offset lookup).
  const bool     hasAnchor = mHasActiveItems;
  const uint32_t anchorPos = mFirstActive;
  const float    anchorOff = mFirstActiveOffset;

  recycler.RecycleAllViews();
  mHasActiveItems = false;

  const uint32_t itemCount = recycler.GetItemCount();
  mCachedItemCount         = itemCount;
  mViewportExtent          = recycler.GetViewportExtent();
  mCrossExtent             = recycler.GetCrossExtent();
  mCacheBefore             = recycler.GetCacheBefore();
  mCacheAfter              = recycler.GetCacheAfter();

  if(itemCount == 0u || mViewportExtent <= 0.0f)
  {
    mFirstActiveOffset   = 0.0f;
    mLastActiveEndOffset = 0.0f;
    return;
  }

  const float maxOffset = std::max(0.0f, GetEstimatedContentExtent(itemCount) - mViewportExtent);
  mScrollOffset         = std::min(mScrollOffset, maxOffset);

  const uint32_t first = FindFirstVisiblePosition(mScrollOffset, mCacheBefore, itemCount);
  const uint32_t last  = FindLastVisiblePosition(mScrollOffset, mViewportExtent, mCacheAfter, itemCount);

  // Compute starting offset for 'first' using the saved anchor when possible.
  // This keeps OnLayoutChildren O(|first - anchorPos|) after a simple resize,
  // instead of O(first) from a cold GetItemOffset walk.
  float firstOffset;
  if(hasAnchor)
  {
    firstOffset = GetItemOffsetFrom(anchorPos, anchorOff, first);
  }
  else
  {
    firstOffset = GetItemOffset(first);
  }

  LayoutState state;
  state.nextPosition  = first;
  state.endPosition   = last + 1u;
  state.currentOffset = firstOffset;
  Fill(state, recycler);

  mFirstActive         = first;
  mFirstActiveOffset   = firstOffset;
  mLastActive          = last;
  mLastActiveEndOffset = state.currentOffset;
  mHasActiveItems      = true;
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

// ---------------------------------------------------------------------------
// Non-virtual end-class methods: Fill / LayoutChunk / MeasureUpdate
// ---------------------------------------------------------------------------

void LinearItemsLayouterImpl::Fill(LayoutState& state, Recycler& recycler)
{
  while(state.nextPosition < state.endPosition)
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

  View view = recycler.GetViewForPosition(pos);
  if(!view)
  {
    DALI_LOG_ERROR("LinearItemsLayouterImpl: adapter returned null view for position %u\n", pos);
    return false;
  }

  constexpr float kUnconstrained = std::numeric_limits<float>::max() * 0.25f;

  float        itemExtent;
  MeasuredSize measured;
  if(mOrientation == Orientation::HORIZONTAL)
  {
    measured   = view.Measure(kUnconstrained, mCrossExtent);
    itemExtent = measured.width;
    MeasureUpdate(pos, itemExtent);
    view.Arrange(LayoutRect(state.currentOffset, 0.0f, itemExtent, mCrossExtent));
  }
  else
  {
    measured   = view.Measure(mCrossExtent, kUnconstrained);
    itemExtent = measured.height;
    MeasureUpdate(pos, itemExtent);
    view.Arrange(LayoutRect(0.0f, state.currentOffset, mCrossExtent, itemExtent));
  }

  state.currentOffset += itemExtent + mItemSpacing;
  ++state.nextPosition;
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
  else
  {
    // Re-measure (e.g. after dynamic content change).
    mTotalMeasuredExtent -= mExtentCache[position];
    mExtentCache[position] = clamped;
    mTotalMeasuredExtent += clamped;
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

  // Use measured average; fall back to default extent when no measurements yet.
  const float avgExtent = (mMeasuredItemCount > 0u)
                            ? (mTotalMeasuredExtent / static_cast<float>(mMeasuredItemCount))
                            : mItemExtent;

  const uint32_t unmeasured   = itemCount - std::min(itemCount, mMeasuredItemCount);
  const float    totalExtent  = mTotalMeasuredExtent + static_cast<float>(unmeasured) * avgExtent;
  const float    totalSpacing = mItemSpacing * static_cast<float>(itemCount - 1u);

  return totalExtent + totalSpacing;
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

uint32_t LinearItemsLayouterImpl::FindLastVisiblePosition(float scrollOffset, float viewportExtent, float cacheAfter, uint32_t itemCount) const
{
  if(itemCount == 0u)
  {
    return 0u;
  }
  const float end    = scrollOffset + viewportExtent + cacheAfter;
  float       cursor = 0.0f;
  for(uint32_t i = 0u; i < itemCount; ++i)
  {
    if(cursor >= end)
    {
      return i == 0u ? 0u : i - 1u;
    }
    cursor += GetItemExtentAt(i) + mItemSpacing;
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

  const float maxOffset = std::max(0.0f, GetEstimatedContentExtent(itemCount) - mViewportExtent);
  const float newOffset = std::min(maxOffset, std::max(0.0f, mScrollOffset + delta));
  const float consumed  = newOffset - mScrollOffset;
  mScrollOffset         = newOffset;

  const uint32_t newFirst = FindFirstVisiblePosition(mScrollOffset, mCacheBefore, itemCount);
  const uint32_t newLast  = FindLastVisiblePosition(mScrollOffset, mViewportExtent, mCacheAfter, itemCount);

  if(!mHasActiveItems)
  {
    // Cold start: compute starting offset from scratch.
    const float firstOffset = GetItemOffset(newFirst);
    LayoutState state;
    state.nextPosition  = newFirst;
    state.endPosition   = newLast + 1u;
    state.currentOffset = firstOffset;
    Fill(state, recycler);
    mFirstActive         = newFirst;
    mFirstActiveOffset   = firstOffset;
    mLastActive          = newLast;
    mLastActiveEndOffset = state.currentOffset;
    mHasActiveItems      = true;
    return consumed;
  }

  const uint32_t oldFirst      = mFirstActive;
  const uint32_t oldLast       = mLastActive;
  const float    oldFirstOff   = mFirstActiveOffset;
  const float    oldLastEndOff = mLastActiveEndOffset;

  // ---------------------------------------------------------------------------
  // Recycle items that scroll out of range.
  // Update anchor offsets incrementally as we remove items (O(recycled count)).
  // ---------------------------------------------------------------------------

  // Recycle from top (items that are now above the visible+cache window).
  float newFirstOff = oldFirstOff;
  for(uint32_t pos = oldFirst; pos < newFirst && pos <= oldLast; ++pos)
  {
    recycler.RecycleViewForPosition(pos);
    newFirstOff += GetItemExtentAt(pos) + mItemSpacing;
  }

  // Recycle from bottom (items now below the visible+cache window).
  float newLastEndOff = oldLastEndOff;
  if(oldLast > newLast)
  {
    for(uint32_t pos = oldLast; pos > newLast; --pos)
    {
      recycler.RecycleViewForPosition(pos);
      newLastEndOff -= GetItemExtentAt(pos) + mItemSpacing;
    }
  }

  // ---------------------------------------------------------------------------
  // Fill newly visible items at top (items before old first).
  // ---------------------------------------------------------------------------
  if(newFirst < oldFirst)
  {
    // Walk backward from the old anchor to find the starting offset for newFirst.
    const float fillStartOffset = GetItemOffsetFrom(oldFirst, oldFirstOff, newFirst);
    LayoutState state;
    state.nextPosition  = newFirst;
    state.endPosition   = std::min(oldFirst, newLast + 1u);
    state.currentOffset = fillStartOffset;
    Fill(state, recycler);
    newFirstOff = fillStartOffset;
  }

  // ---------------------------------------------------------------------------
  // Fill newly visible items at bottom (items after old last).
  // ---------------------------------------------------------------------------
  if(newLast > oldLast)
  {
    LayoutState state;
    state.nextPosition  = oldLast + 1u;
    state.endPosition   = newLast + 1u;
    state.currentOffset = newLastEndOff; // O(1): end of surviving window
    Fill(state, recycler);
    newLastEndOff = state.currentOffset;
  }

  mFirstActive         = newFirst;
  mFirstActiveOffset   = newFirstOff;
  mLastActive          = newLast;
  mLastActiveEndOffset = newLastEndOff;
  mHasActiveItems      = (newFirst <= newLast);

  return consumed;
}

} // namespace Internal

} // namespace Ui
} // namespace Dali
