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

#ifndef DALI_UI_FOUNDATION_PAGE_SCROLL_VIEW_H
#define DALI_UI_FOUNDATION_PAGE_SCROLL_VIEW_H

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/views/scroll/scroll-view.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/signals/dali-signal.h>

namespace DALI_NAMESPACE
{

namespace Ui
{

namespace Integration
{
class PageScrollViewImpl;
}

/**
 * @brief A ScrollView that snaps content to discrete page boundaries.
 *
 * PageScrollView extends ScrollView with page-based navigation:
 *  - After a drag or fling the content snaps to a page selected from the drag
 *    distance or projected fling position, clamped to the valid page range.
 *  - The page size defaults to the viewport size; use SetPageSize() to override.
 *  - Pages are laid out sequentially along the scroll axis starting at position 0.
 *
 * PageScrollView provides logical page navigation.
 * Continuous scroll position and scrolling/dragging signals remain available
 * through the inherited ScrollView API. PageChangedSignal reports logical page
 * state; it is not an animation-completion signal.
 *
 * Example:
 * @code
 * auto pager = PageScrollView::New();
 * pager.SetScrollDirection(ScrollDirection::Horizontal);
 * pager.SetContent(myHorizontalContent);
 *
 * // After adding the pager to the scene and allowing layout to run:
 * pager.ScrollToPage(2, false);
 * int currentPage = pager.GetCurrentPage();
 * @endcode
 * @SINCE_3_0.0
 */
class DALI_UI_API PageScrollView : public ScrollView
{
public:
  /**
   * @brief Reports the logical current page (-1 when empty) and total page count.
   * @SINCE_3_0.0
   */
  using PageChangedSignalType = Dali::Signal<void(int, int)>;

public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized PageScrollView handle.
   * @SINCE_3_0.0
   */
  PageScrollView();

  /**
   * @brief Creates an initialized PageScrollView.
   *
   * @SINCE_3_0.0
   * @return A handle to a newly allocated PageScrollView.
   */
  static PageScrollView New();

  /**
   * @brief Copy constructor.
   * @SINCE_3_0.0
   */
  PageScrollView(const PageScrollView& rhs);

  /**
   * @brief Move constructor.
   * @SINCE_3_0.0
   */
  PageScrollView(PageScrollView&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~PageScrollView();

  /**
   * @brief Copy assignment operator.
   * @SINCE_3_0.0
   */
  PageScrollView& operator=(const PageScrollView& rhs);

  /**
   * @brief Move assignment operator.
   * @SINCE_3_0.0
   */
  PageScrollView& operator=(PageScrollView&& rhs) noexcept;

  DALI_UI_VIEW_WITH(PageScrollView)

public: // Static Methods
  /**
   * @brief Downcasts a handle to PageScrollView.
   *
   * @SINCE_3_0.0
   * @param[in] handle Handle to an object.
   * @return A valid PageScrollView handle if the object is a PageScrollView,
   *         otherwise an uninitialized handle.
   */
  static PageScrollView DownCast(BaseHandle handle);

public: // Page API
  /**
   * @brief Sets the page size used for snapping.
   *
   * Only the axis component relevant to the scroll direction is used:
   *  - Vertical scrolling  → size.y
   *  - Horizontal scrolling → size.x
   *
   * Set to (0, 0) (the default) to use the viewport size as the page size.
   *
   * @SINCE_3_0.0
   * @param[in] size Page size in natural units, like a requested size: the view multiplies it by
   *            its effective scale, so a page keeps its share of the content at every scale.
   */
  void SetPageSize(const Vector2& size);

  /**
   * @brief Gets the explicitly set page size.
   *
   * Returns (0, 0) if no explicit page size has been set (viewport-size mode). The size is
   * returned as set.
   * @SINCE_3_0.0
   */
  Vector2 GetPageSize() const;

  /**
   * @brief Returns the zero-based logical current page index.
   *
   * Returns -1 when GetPageCount() == 0.
   * During an uninterrupted animated page request, the previous page normally
   * remains current until scrolling finishes. Content notifications, page-size
   * or layout changes, and cancellation or replacement of a scroll can update
   * the logical page before the requested animation reaches its endpoint.
   * In particular, a count change during a page snap reports the snap target
   * clamped to the new count. This is not a most-visible-page calculation.
   *
   * An explicitly selected partial last page keeps its index even when its
   * clamped resting offset is closer to the preceding page boundary.
   * @SINCE_3_0.0
   */
  int GetCurrentPage() const;

  /**
   * @brief Returns the total number of pages derived from content / page size.
   *
   * Returns 0 when the view has no content pages (empty state after
   * NotifyPagesRemoved removes all pages).
   * Otherwise the layout-derived count is the content length divided by the
   * effective page length, rounded up, along the paging axis. A partial last
   * page counts as a page; this is not a count of child views.
   *
   * Before the effective page length has been measured (less than one pixel),
   * the layout-derived result is provisionally 1. NotifyPagesInserted() and
   * NotifyPagesRemoved() supply an immediate expected count until it is
   * reconciled with layout or scrolling finishes.
   * @SINCE_3_0.0
   */
  int GetPageCount() const;

  /**
   * @brief Animates or snaps to the given page.
   *
   * Clamps the requested index to the current nonempty page range and the
   * resulting offset to the scrollable bounds. A new request cancels the
   * existing scroll rather than being queued. With @p animate false, the
   * position and logical page are updated synchronously for measured content.
   * A request requiring no movement can also update the logical page immediately.
   *
   * PageChangedSignal may be emitted inside this call, including when replacing
   * an animated request. It does not acknowledge successful arrival for each
   * request, and an unchanged logical state need not produce a notification.
   * An empty sequence has no valid target and GetCurrentPage() remains -1.
   * Configure content and allow layout to establish page geometry before using
   * this method to navigate to a measured page offset.
   *
   * @SINCE_3_0.0
   * @param[in] page    Target page index, clamped to [0, GetPageCount()-1].
   * @param[in] animate True to animate, false for instant jump.
   */
  void ScrollToPage(int page, bool animate = true);

  /**
   * @brief Signal reporting logical current-page or page-count changes.
   *
   * Signature: void(int currentPage, int pageCount)
   *
   * Count-only changes are reported too. State is updated before emission;
   * instant navigation and NotifyPagesInserted()/NotifyPagesRemoved() can emit
   * synchronously. Layout and page-size changes can also emit without navigation.
   * A notification during a transition does not imply that scrolling has stopped.
   * @SINCE_3_0.0
   */
  PageChangedSignalType& PageChangedSignal();

  /**
   * @brief Notify the view that pages have been inserted into the content.
   *
   * Call this AFTER adding new page views to the scroll content.  The view
   * will:
   *  - Shift the current-page index if @p atIndex <= current page.
   *  - Jump the scroll position to keep the same content page visible.
   *  - Emit PageChangedSignal with the updated state.
   *
   * GetPageCount() returns the new expected count immediately (before the
   * next layout pass) so that observers can update their page count at once.
   *
   * @SINCE_3_0.0
   * @param[in] atIndex       First index at which pages were inserted (0-based).
   * @param[in] insertedCount Number of pages inserted (default 1).
   */
  void NotifyPagesInserted(int atIndex, int insertedCount = 1);

  /**
   * @brief Notify the view that pages have been removed from the content.
   *
   * Call this AFTER removing page views from the scroll content.  The view
   * will:
   *  - Shift the current-page index left if removal was before it.
   *  - Snap to @p atIndex (or the last valid page) if the current page was removed.
   *  - Emit PageChangedSignal with the updated state.
   *
   * @SINCE_3_0.0
   * @param[in] atIndex      Index of the first removed page (0-based).
   * @param[in] removedCount Number of pages removed (default 1).
   */
  void NotifyPagesRemoved(int atIndex, int removedCount = 1);

public: // Not intended for application developers
  /// @cond internal
  explicit PageScrollView(Integration::PageScrollViewImpl& impl);
  explicit PageScrollView(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace Ui

} //namespace DALI_NAMESPACE

#endif // DALI_UI_FOUNDATION_PAGE_SCROLL_VIEW_H
