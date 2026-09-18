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

namespace DALI_NAMESPACE
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

// Attachment storage preserves the published implementation object's size. A layout can
// report width, height and viewport separately; only a new geometry may rederive a page.
const AttachmentId PAGE_GEOMETRY = AttachmentId::Alloc();
struct PageGeometry
{
  float           primary{0};
  float           pageLength{0};
  ScrollDirection direction{ScrollDirection::Vertical};
  bool            valid{false};
};
PageGeometry& LastPageGeometry(Ui::View view)
{
  auto* geometry = view.GetAttachment<PageGeometry>(PAGE_GEOMETRY);
  if(!geometry)
  {
    view.SetAttachment(PAGE_GEOMETRY, Dali::MakeUnique<PageGeometry>());
    geometry = view.GetAttachment<PageGeometry>(PAGE_GEOMETRY);
  }

  DALI_ASSERT_ALWAYS(geometry && "PageScrollView geometry attachment creation failed");
  return *geometry;
}

// Restores mNotifyInProgress on every exit, including an exception thrown by an application
// handler reached through SetScrollable*/ScrollToPage (pattern: PendingBatchRollbackScope,
// public-api/layouts/layout-controller.cpp). The previous value is restored, not false.
struct NotifyInProgressScope
{
  explicit NotifyInProgressScope(bool& flag)
  : mFlag(flag),
    mPrevious(flag)
  {
    mFlag = true;
  }
  ~NotifyInProgressScope()
  {
    mFlag = mPrevious;
  }
  NotifyInProgressScope(const NotifyInProgressScope&)            = delete;
  NotifyInProgressScope& operator=(const NotifyInProgressScope&) = delete;

  bool& mFlag;
  bool  mPrevious;
};

constexpr float PAGE_SNAP_DURATION  = 0.30f; ///< Fixed snap animation duration (seconds)
constexpr float PAGE_SNAP_THRESHOLD = 0.30f; ///< Fraction of page crossed to trigger page advance on slow drag

BaseHandle Create()
{
  return BaseHandle();
}

DALI_TYPE_REGISTRATION_BEGIN_FULL(Ui::PageScrollView, Ui::Integration::PageScrollViewImpl, Ui::ScrollView, Create)
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
  const Vector2 effective  = GetEffectivePageSize();
  const float   pageLength = dir == ScrollDirection::Horizontal ? effective.x : effective.y;
  if(pageLength >= 1.0f)
    LastPageGeometry(Ui::View::DownCast(Self())) = {primary, pageLength, dir, true};

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
    // A target within half a unit of the current position, or a pager without content,
    // finishes synchronously: no scroll starts (SendScrollStarted is what sets the
    // scrolling state), so no OnScrollFinished will ever clear the target. Nothing is in
    // flight, so nothing may stay pending.
    if(!IsScrolling())
    {
      mSnapTargetPage = -1;
    }
  }
  else
    ScrollTo(targetPos, false);
}

PageScrollableInterface::PageChangedSignalType& PageScrollViewImpl::PageChangedSignal()
{
  return mPageChangedSignal;
}

PageScrollableInterface::DestroyingSignalType& PageScrollViewImpl::DestroyingSignal()
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

  // LayoutManager calls SetScrollableWidth then SetScrollableHeight in separate
  // steps, and RefreshViewport adds a third call for a viewport-only resize; each
  // triggers this callback.  Skip a call where the primary-axis scrollable dimension
  // or the page length is not real yet, so we neither emit a page count derived from a
  // partially-updated layout nor resolve a page position against a zero page length.
  //
  // The guard runs BEFORE the override is released: a cross-axis call must leave the
  // pre-layout state intact for the call that does have real dimensions.
  const ScrollDirection dir      = GetScrollDirection();
  const Vector2         pageSize = GetEffectivePageSize();
  const float           primary  = (dir == ScrollDirection::Horizontal) ? GetScrollableWidth() : GetScrollableHeight();
  const float           pageLen  = (dir == ScrollDirection::Horizontal) ? pageSize.x : pageSize.y;
  if(primary < 1.0f || pageLen < 1.0f) return;
  auto& geometry = LastPageGeometry(Ui::View::DownCast(Self()));
  // 0.01 is the tolerance ScrollViewImpl itself uses for these dimensions
  // (SetScrollableWidth, SetScrollableHeight, RefreshViewport).
  const bool geometryChanged = !geometry.valid || geometry.direction != dir ||
                               std::abs(geometry.primary - primary) >= 0.01f ||
                               std::abs(geometry.pageLength - pageLen) >= 0.01f;
  // Publish before applying a pending selection: instant scrolling may re-enter layout.
  geometry = {primary, pageLen, dir, true};

  // Layout has now set correct dimensions — release the expected-count override that
  // NotifyPages* left in place, and apply the page it selected.
  //
  // An override reaches here after ANY NotifyPages*, not only a pre-layout one: the
  // instant ScrollToPage the notify method issues re-enters OnScrollFinished while
  // mNotifyInProgress is set, and that handler returns early without clearing the
  // override, so the next layout pass finds it either way.
  //
  //  - Notified BEFORE the first layout: the page length was still 0, ScrollToPage
  //    resolved every page to scroll position 0, and the selected page was recorded in
  //    mCurrentPage but never applied. Applying it here is what keeps the re-derivation
  //    below from silently resetting the selection to page 0.
  //  - Notified AFTER a layout: the target equals the current position, the 0.5 unit
  //    test below fails and nothing moves. If the geometry did change in the meantime,
  //    re-applying the selected page is exactly the wanted outcome.
  //
  // Only when nothing else owns the scroll position: a scroll in flight, or a snap
  // target waiting for its OnScrollFinished, is a newer intent than the recorded one.
  // Known residual: a user ScrollTo issued pre-layout after NotifyPages* clears the
  // override through OnScrollFinished, and is then indistinguishable from a settled
  // post-layout state.
  const bool pendingSelection = (mExpectedPageCount >= 0);
  mExpectedPageCount          = -1;
  if(pendingSelection && !IsScrolling() && mSnapTargetPage < 0)
  {
    const int total = GetPageCount();
    if(total > 0)
    {
      const int     page    = std::max(0, std::min(total - 1, mCurrentPage));
      const Vector2 target  = ScrollPositionForPage(page);
      const Vector2 current = GetScrollPosition();
      mCurrentPage          = page;
      if(std::abs(target.x - current.x) >= 0.5f || std::abs(target.y - current.y) >= 0.5f)
      {
        // A partial last page can clamp to an offset rounding to the previous page.
        // Keep the explicit NotifyPages selection while applying its physical position.
        NotifyInProgressScope notify(mNotifyInProgress);
        ScrollToPage(page, false);
      }
    }
  }

  // A count change is always reported. The reported page is the page the view will settle
  // on: while a snap is in flight it is the snap target clamped to the new count, which is
  // exactly what OnScrollFinished commits, so the transient and the settled notification
  // agree even when the last page is partial (round(offset / pageLength) would name the
  // page below it). A free scroll (no snap target) or a settled view re-derives from the
  // scroll position; during an animated scroll that is the LIVE current position kept by
  // OnContentPositionChanged, not the target. A NotifyPages* selection just applied above
  // is the newer intent and is kept. A geometry change that keeps the count re-derives
  // only on a settled view, because a snap target that is still valid must not be
  // replaced by the nearest page. ScrollToPage clears a target whose animated scroll never
  // started, so an explicit page request that had nothing to scroll leaves no stale target
  // behind.
  const int  newCount     = GetPageCount();
  const bool countChanged = (newCount != mLastNotifiedPageCount);
  const bool settled      = !IsScrolling() && mSnapTargetPage < 0;
  int        newPage      = mCurrentPage;
  if(!pendingSelection && (countChanged || (geometryChanged && settled)))
  {
    newPage = (mSnapTargetPage >= 0) ? mSnapTargetPage : PageForScrollPosition(GetScrollPosition());
  }
  newPage = (newCount > 0) ? std::max(0, std::min(newCount - 1, newPage)) : 0;
  if(!countChanged && newPage == mCurrentPage) return;
  mCurrentPage = newPage;

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

  // The count can shrink while a snap keeps its original target. Preserve a valid
  // partial-page selection, but never commit an index outside the current page range.
  const int total = GetPageCount();
  settled         = total > 0 ? std::max(0, std::min(total - 1, settled)) : 0;
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
  {
    NotifyInProgressScope notify(mNotifyInProgress);

    // Proactively update the scroll-view's bounds so that AdjustScrollPosition
    // in the subsequent ScrollToPage call uses the correct (post-insert) range,
    // not the stale layout-measured width/height.
    // P1: skip when pageSize is zero (pre-layout); Arrange() will set correct
    //     bounds on the next layout pass, and the page selected below is applied
    //     to the scroll position by OnScrollableAreaChanged once they exist.
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
  }

  DALI_LOG_INFO(gLogFilter, Debug::Verbose,
                "[PageScrollView] NotifyPagesInserted atIndex=%d count=%d → page %d/%d\n",
                atIndex, insertedCount, mCurrentPage, newTotal);

  // P2: emit GetCurrentPage() so that -1 is propagated when the view is empty.
  EmitPageChanged(GetCurrentPage(), newTotal);
  // mExpectedPageCount is cleared by OnScrollableAreaChanged when the natural
  // layout pass fires (mNotifyInProgress is false at that point), which also
  // applies the page selected here if this ran before the first layout.
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

  {
    NotifyInProgressScope notify(mNotifyInProgress);

    // P1: skip when pageSize is zero (pre-layout); Arrange() will set correct
    //     bounds on the next layout pass, and the page selected below is applied
    //     to the scroll position by OnScrollableAreaChanged once they exist.
    //     Post-layout this always executes.
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
  }

  DALI_LOG_INFO(gLogFilter, Debug::Verbose,
                "[PageScrollView] NotifyPagesRemoved atIndex=%d count=%d → page %d/%d\n",
                atIndex, removedCount, mCurrentPage, newTotal);

  // P2: emit GetCurrentPage() so that -1 is propagated when the view is empty.
  EmitPageChanged(GetCurrentPage(), newTotal);
  // mExpectedPageCount is cleared by OnScrollableAreaChanged when the natural
  // layout pass fires (mNotifyInProgress is false at that point), which also
  // applies the page selected here if this ran before the first layout.
}

} // namespace Integration

} // namespace Ui

} //namespace DALI_NAMESPACE
