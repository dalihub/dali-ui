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

// CLASS HEADER
#include <dali-ui-foundation/integration-api/page-scroll-view-impl.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <cmath>
// INTERNAL INCLUDES
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_PAGE_SCROLL");
#endif

constexpr float PAGE_SNAP_DURATION  = 0.30f; ///< Fixed snap animation duration (seconds)
constexpr float PAGE_SNAP_THRESHOLD = 0.30f; ///< Fraction of page crossed to trigger page advance on slow drag

BaseHandle Create()
{
  return BaseHandle();
}

DALI_TYPE_REGISTRATION_BEGIN(PageScrollViewImpl, ScrollViewImpl, Create)
DALI_TYPE_REGISTRATION_END()

} // namespace

PageScrollViewImplPtr PageScrollViewImpl::New()
{
  return PageScrollViewImplPtr(new PageScrollViewImpl());
}

PageScrollViewImpl::PageScrollViewImpl()
: ScrollViewImpl()
{
}

PageScrollViewImpl::~PageScrollViewImpl()
{
  // Notify observers (e.g. PageIndicator) before any members are destroyed.
  // Handlers must not call back into this object.
  if(!mDestroyingSignal.Empty())
    mDestroyingSignal.Emit();
}

void PageScrollViewImpl::OnInitialize()
{
  ScrollViewImpl::OnInitialize();

  // Connect to ScrollFinishedSignal to keep mCurrentPage in sync after every
  // animation (fling or snap) settles.
  Ui::ScrollView handle = Ui::ScrollView::DownCast(Self());
  ScrollFinishedSignal().Connect(this, &PageScrollViewImpl::OnScrollFinished);
}

void PageScrollViewImpl::SetPageSize(const Vector2& size)
{
  if(mPageSize == size) return;
  mPageSize = size;

  // Page size change immediately affects GetPageCount() and page boundaries.
  // If layout has already run (primary scrollable dimension > 0), re-derive
  // mCurrentPage from the current scroll position with the new page size and
  // notify observers if count or page changed.
  // Pre-layout: the primary dimension is 0 here; OnScrollableAreaChanged from
  // the first layout pass will emit the correct values.
  ScrollDirection dir     = GetScrollDirection();
  float           primary = (dir == ScrollDirection::Horizontal) ? GetScrollableWidth() : GetScrollableHeight();
  if(primary < 1.0f) return;

  int newCount = GetPageCount();
  int newPage  = (newCount > 0)
                   ? std::max(0, std::min(newCount - 1, PageForScrollPosition(GetScrollPosition())))
                   : 0;

  if(newCount != mLastNotifiedPageCount || newPage != mCurrentPage)
  {
    mCurrentPage = newPage;
    EmitPageChanged(GetCurrentPage(), newCount);
  }
}

Vector2 PageScrollViewImpl::GetPageSize() const
{
  return mPageSize;
}

int PageScrollViewImpl::GetCurrentPage() const
{
  // Returns -1 when the view is empty (GetPageCount() == 0) to distinguish
  // "no pages" from "first page (index 0)".
  return (GetPageCount() <= 0) ? -1 : mCurrentPage;
}

int PageScrollViewImpl::GetPageCount() const
{
  // mExpectedPageCount >= 0: set by NotifyPages* — use as authoritative count
  // (including 0, which means "truly empty after explicit removal").
  // mExpectedPageCount == -1: sentinel, fall through to layout-based computation.
  if(mExpectedPageCount >= 0)
    return mExpectedPageCount;

  Vector2         pageSize = GetEffectivePageSize();
  ScrollDirection dir      = GetScrollDirection();

  float contentLen = (dir == ScrollDirection::Horizontal)
                       ? GetScrollableWidth()
                       : GetScrollableHeight();
  float pageLen    = (dir == ScrollDirection::Horizontal) ? pageSize.x : pageSize.y;

  // Viewport not yet measured — return 1 as a conservative pre-layout value.
  if(pageLen < 1.0f) return 1;

  int count = static_cast<int>(std::ceil(contentLen / pageLen));
  return std::max(0, count);
}

void PageScrollViewImpl::ScrollToPage(int page, bool animate)
{
  int total  = GetPageCount();
  int target = std::max(0, std::min(total - 1, page));

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[PageScrollView] ScrollToPage %d/%d\n", target, total);

  Vector2 targetPos = ScrollPositionForPage(target);

  if(animate)
  {
    mSnapTargetPage = target;
    ScrollToWithDuration(targetPos, PAGE_SNAP_DURATION);
  }
  else
    ScrollTo(targetPos, false);
}

IPageScrollable::PageChangedSignalType& PageScrollViewImpl::PageChangedSignal()
{
  return mPageChangedSignal;
}

IPageScrollable::DestroyingSignalType& PageScrollViewImpl::DestroyingSignal()
{
  return mDestroyingSignal;
}

// ─── ScrollViewImpl hook ───────────────────────────────────────────────────

void PageScrollViewImpl::OnBeforeScrollAnimation(Vector2& targetPosition, float& durationSec)
{
  int total = GetPageCount();
  if(total <= 1) return;

  int snapPage;

  if(durationSec < 0.0001f)
  {
    // Slow drag (speed < FLING_VMIN): decide snap direction from how far the
    // current position has moved past the nearest page boundary.
    Vector2         curPos = GetScrollPosition();
    Vector2         pgSize = GetEffectivePageSize();
    ScrollDirection dir    = GetScrollDirection();

    float pos     = (dir == ScrollDirection::Horizontal) ? curPos.x : curPos.y;
    float pageLen = (dir == ScrollDirection::Horizontal) ? pgSize.x : pgSize.y;

    if(pageLen < 1.0f)
    {
      return;
    }

    int   basePage = (mSnapTargetPage >= 0) ? mSnapTargetPage : mCurrentPage;
    float basePos  = (dir == ScrollDirection::Horizontal)
                       ? ScrollPositionForPage(basePage).x
                       : ScrollPositionForPage(basePage).y;
    float offset   = (pos - basePos) / pageLen; // signed distance from basePage's actual position

    // Advance one page if the user has dragged past PAGE_SNAP_THRESHOLD of a
    // page width from the base page.  Measuring from basePage (not round()) avoids
    // snapping backward in the 50–70 % region where round() already rounds up.
    if(offset > PAGE_SNAP_THRESHOLD)
      snapPage = basePage + 1;
    else if(offset < -PAGE_SNAP_THRESHOLD)
      snapPage = basePage - 1;
    else
      snapPage = basePage;

    snapPage = std::max(0, std::min(total - 1, snapPage));
  }
  else
  {
    Vector2         pgSize = GetEffectivePageSize();
    ScrollDirection dir    = GetScrollDirection();

    float target  = (dir == ScrollDirection::Horizontal) ? targetPosition.x : targetPosition.y;
    float pageLen = (dir == ScrollDirection::Horizontal) ? pgSize.x : pgSize.y;

    if(pageLen < 1.0f) return;

    int projected = static_cast<int>(std::round(target / pageLen));
    int basePage  = (mSnapTargetPage >= 0) ? mSnapTargetPage : mCurrentPage;
    snapPage      = std::max(basePage - 1, std::min(basePage + 1, projected));
    snapPage      = std::max(0, std::min(total - 1, snapPage));
  }

  mSnapTargetPage = snapPage;
  targetPosition  = ScrollPositionForPage(snapPage);
  durationSec     = PAGE_SNAP_DURATION;

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[PageScrollView] OnBeforeScrollAnimation → page %d pos=(%.1f, %.1f)\n",
                snapPage, targetPosition.x, targetPosition.y);
}

// ─── Private helpers ──────────────────────────────────────────────────────

Vector2 PageScrollViewImpl::GetEffectivePageSize() const
{
  if(mPageSize.x > 0.0f || mPageSize.y > 0.0f)
    return mPageSize;

  return Vector2(GetViewportWidth(), GetViewportHeight());
}

Vector2 PageScrollViewImpl::ScrollPositionForPage(int page) const
{
  Vector2         pgSize = GetEffectivePageSize();
  ScrollDirection dir    = GetScrollDirection();

  if(dir == ScrollDirection::Horizontal)
    return AdjustScrollPosition(Vector2(page * pgSize.x, 0.0f));
  else
    return AdjustScrollPosition(Vector2(0.0f, page * pgSize.y));
}

int PageScrollViewImpl::PageForScrollPosition(const Vector2& scrollPos) const
{
  Vector2         pgSize = GetEffectivePageSize();
  ScrollDirection dir    = GetScrollDirection();

  float pos     = (dir == ScrollDirection::Horizontal) ? scrollPos.x : scrollPos.y;
  float pageLen = (dir == ScrollDirection::Horizontal) ? pgSize.x : pgSize.y;

  if(pageLen < 1.0f) return 0;

  return std::max(0, std::min(GetPageCount() - 1, static_cast<int>(std::round(pos / pageLen))));
}

void PageScrollViewImpl::CommitPage(int newPage)
{
  if(newPage == mCurrentPage) return;

  mCurrentPage = newPage;
  int total    = GetPageCount();

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[PageScrollView] page changed → %d / %d\n", mCurrentPage, total);

  // P2: use GetCurrentPage() so that -1 is emitted when the view is empty.
  EmitPageChanged(GetCurrentPage(), total);
}

void PageScrollViewImpl::EmitPageChanged(int currentPage, int pageCount)
{
  mLastNotifiedPageCount = pageCount;
  mPageChangedSignal.Emit(currentPage, pageCount);
}

// ─── ScrollViewImpl::OnScrollableAreaChanged override ────────────────────────

void PageScrollViewImpl::OnScrollableAreaChanged()
{
  // During NotifyPages*, mNotifyInProgress suppresses the intermediate
  // OnScrollableAreaChanged that fires from SetScrollable* — mCurrentPage has
  // not been adjusted yet and the emit happens explicitly at the end of the
  // notify method.
  if(mNotifyInProgress) return;

  // Layout has now set correct dimensions — release any pre-layout
  // expected-count override that NotifyPages* left in place.
  mExpectedPageCount = -1;

  // LayoutManager calls SetScrollableWidth then SetScrollableHeight in separate
  // steps, each triggering this callback.  Skip the intermediate call where
  // the primary-axis scrollable dimension has not been set yet, so we don't
  // emit a wrong page count derived from a partially-updated layout.
  {
    ScrollDirection dir     = GetScrollDirection();
    float           primary = (dir == ScrollDirection::Horizontal) ? GetScrollableWidth() : GetScrollableHeight();
    if(primary < 1.0f) return;
  }

  int newCount = GetPageCount();
  if(newCount == mLastNotifiedPageCount) return;

  // The layout pass just changed the scrollable area (most commonly: the
  // very first layout pass after Bind() was called pre-layout).  Re-derive
  // the current page from the current scroll position and emit so that
  // PageIndicator rebuilds with the accurate count.
  if(newCount > 0)
    mCurrentPage = std::max(0, std::min(newCount - 1, PageForScrollPosition(GetScrollPosition())));

  DALI_LOG_INFO(gLogFilter, Debug::Verbose,
                "[PageScrollView] OnScrollableAreaChanged count %d → %d, page=%d\n",
                mLastNotifiedPageCount, newCount, mCurrentPage);

  EmitPageChanged(GetCurrentPage(), newCount);
}

void PageScrollViewImpl::OnScrollFinished(Ui::ScrollView /*scrollView*/)
{
  // Suppress the re-derive while a NotifyPages* call is in progress.
  // The notify method has already set mCurrentPage correctly; we do not
  // want an OnScrollFinished triggered by the internal instant-scroll to
  // override it before the layout pass has updated mScrollableWidth/Height.
  if(mNotifyInProgress)
  {
    // A NotifyPages* call repositioned us instantly; discard any in-flight
    // snap target so the next real OnScrollFinished re-derives from position.
    mSnapTargetPage = -1;
    return;
  }

  // Layout has now updated — reset sentinel so GetPageCount() returns the
  // authoritative layout-derived value.
  mExpectedPageCount = -1;

  // When a snap animation was in flight (mSnapTargetPage >= 0), use the intended
  // target page rather than the current scroll position.  CancelScrollAnimation()
  // calls SendScrollFinished() while content is mid-animation, so
  // PageForScrollPosition on the intermediate position would otherwise round
  // incorrectly (e.g. 0.5 → page 1 while actually heading back to page 0).
  int settled;
  if(mSnapTargetPage >= 0)
  {
    settled         = mSnapTargetPage;
    mSnapTargetPage = -1;
  }
  else
  {
    settled = PageForScrollPosition(GetScrollPosition());
  }

  CommitPage(settled);
}

// ─── Dynamic page insertion / removal ────────────────────────────────────────

void PageScrollViewImpl::NotifyPagesInserted(int atIndex, int insertedCount)
{
  if(insertedCount <= 0) return;

  int oldTotal = GetPageCount();

  // P3: clamp atIndex to valid insertion range [0, oldTotal].
  atIndex = std::max(0, std::min(oldTotal, atIndex));

  // GetPageCount() may still reflect the pre-insert layout value — use it
  // as the "before" count and add insertedCount to get the expected total.
  int newTotal       = oldTotal + insertedCount;
  mExpectedPageCount = newTotal;

  // Suppress both OnScrollableAreaChanged (triggered by SetScrollable* below)
  // and OnScrollFinished (triggered by ScrollToPage) while we are still
  // adjusting mCurrentPage and the layout hasn't reflected the new count yet.
  mNotifyInProgress = true;

  // Proactively update the scroll-view's bounds so that AdjustScrollPosition
  // in the subsequent ScrollToPage call uses the correct (post-insert) range,
  // not the stale layout-measured width/height.
  // P1: skip when pageSize is zero (pre-layout); Arrange() will set correct
  //     bounds on the next layout pass.
  Vector2         pgSize = GetEffectivePageSize();
  ScrollDirection dir    = GetScrollDirection();
  float           pgLen  = (dir == ScrollDirection::Horizontal) ? pgSize.x : pgSize.y;
  if(pgLen > 0.0f)
  {
    if(dir == ScrollDirection::Horizontal)
      SetScrollableWidth(pgLen * newTotal);
    else
      SetScrollableHeight(pgLen * newTotal);
  }

  // If the insertion happened at or before the current page, the content
  // page the user was viewing has shifted right — track it.
  if(atIndex <= mCurrentPage)
    mCurrentPage += insertedCount;
  mCurrentPage = std::max(0, std::min(newTotal - 1, mCurrentPage));

  ScrollToPage(mCurrentPage, false);
  mNotifyInProgress = false;

  DALI_LOG_INFO(gLogFilter, Debug::Verbose,
                "[PageScrollView] NotifyPagesInserted atIndex=%d count=%d → page %d/%d\n",
                atIndex, insertedCount, mCurrentPage, newTotal);

  // P2: emit GetCurrentPage() so that -1 is propagated when the view is empty.
  EmitPageChanged(GetCurrentPage(), newTotal);
  // mExpectedPageCount is cleared by OnScrollableAreaChanged when the natural
  // layout pass fires (mNotifyInProgress is false at that point).
}

void PageScrollViewImpl::NotifyPagesRemoved(int atIndex, int removedCount)
{
  if(removedCount <= 0) return;

  int oldTotal = GetPageCount(); // may be pre-layout but is our best "before" value

  // P3: clamp to valid removal range so that out-of-bounds calls don't corrupt state.
  atIndex      = std::max(0, std::min(oldTotal > 0 ? oldTotal - 1 : 0, atIndex));
  removedCount = std::min(removedCount, oldTotal - atIndex);
  if(removedCount <= 0) return;

  int newTotal       = std::max(0, oldTotal - removedCount); // 0 = valid empty state
  mExpectedPageCount = newTotal;

  mNotifyInProgress = true;

  // P1: skip when pageSize is zero (pre-layout); Arrange() will set correct
  //     bounds on the next layout pass. Post-layout this always executes.
  Vector2         pgSize = GetEffectivePageSize();
  ScrollDirection dir    = GetScrollDirection();
  float           pgLen  = (dir == ScrollDirection::Horizontal) ? pgSize.x : pgSize.y;
  if(pgLen > 0.0f)
  {
    if(dir == ScrollDirection::Horizontal)
      SetScrollableWidth(pgLen * newTotal);
    else
      SetScrollableHeight(pgLen * newTotal);
  }

  int atEnd = atIndex + removedCount; // first index past the removed range

  if(atIndex > mCurrentPage)
  {
    // Removal entirely after current page — no index change.
  }
  else if(atEnd <= mCurrentPage)
  {
    // Removal entirely before current page — shift index left.
    mCurrentPage -= removedCount;
  }
  else
  {
    // Removal overlaps the current page — snap to the start of the removed
    // range (or the last valid page if that's past the new end).
    mCurrentPage = atIndex;
  }

  mCurrentPage = (newTotal > 0) ? std::max(0, std::min(newTotal - 1, mCurrentPage)) : 0;

  // ScrollToPage handles 0-page case naturally (target=0, position=(0,0)).
  ScrollToPage(mCurrentPage, false);
  mNotifyInProgress = false;

  DALI_LOG_INFO(gLogFilter, Debug::Verbose,
                "[PageScrollView] NotifyPagesRemoved atIndex=%d count=%d → page %d/%d\n",
                atIndex, removedCount, mCurrentPage, newTotal);

  // P2: emit GetCurrentPage() so that -1 is propagated when the view is empty.
  EmitPageChanged(GetCurrentPage(), newTotal);
  // mExpectedPageCount is cleared by OnScrollableAreaChanged when the natural
  // layout pass fires (mNotifyInProgress is false at that point).
}

} // namespace Integration

} // namespace Ui

} // namespace Dali
