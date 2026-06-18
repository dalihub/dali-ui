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
#include <dali-ui-foundation/public-api/i-page-scrollable.h>
#include <dali-ui-foundation/public-api/scroll-view.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
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
 *  - After a drag or fling the content animates to the nearest page boundary.
 *  - The page size defaults to the viewport size; use SetPageSize() to override.
 *  - Pages are laid out sequentially along the scroll axis starting at position 0.
 *
 * PageScrollView implements IPageScrollable, so it can be bound to a
 * PageIndicator (or any other IPageScrollable consumer) via
 * PageIndicator::Bind().
 *
 * Example:
 * @code
 * auto pager = PageScrollView::New();
 * pager.SetScrollDirection(ScrollDirection::Horizontal);
 * pager.SetContent(myHorizontalContent);
 *
 * auto indicator = PageIndicator::New();
 * indicator.Bind(pager);
 * @endcode
 */
class DALI_UI_API PageScrollView : public ScrollView, public IPageScrollable
{
public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized PageScrollView handle.
   */
  PageScrollView();

  /**
   * @brief Creates an initialized PageScrollView.
   *
   * @return A handle to a newly allocated PageScrollView.
   */
  static PageScrollView New();

  /**
   * @brief Copy constructor.
   */
  PageScrollView(const PageScrollView& rhs);

  /**
   * @brief Move constructor.
   */
  PageScrollView(PageScrollView&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~PageScrollView();

  /**
   * @brief Copy assignment operator.
   */
  PageScrollView& operator=(const PageScrollView& rhs);

  /**
   * @brief Move assignment operator.
   */
  PageScrollView& operator=(PageScrollView&& rhs) noexcept;

  DALI_UI_VIEW_WITH(PageScrollView)

public: // Static Methods
  /**
   * @brief Downcasts a handle to PageScrollView.
   *
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
   * @param[in] size Page size in pixels.
   */
  void SetPageSize(const Vector2& size);

  /**
   * @brief Gets the explicitly set page size.
   *
   * Returns (0, 0) if no explicit page size has been set (viewport-size mode).
   */
  Vector2 GetPageSize() const;

  // IPageScrollable

  /**
   * @brief Returns the zero-based index of the currently visible page.
   *
   * Returns -1 when GetPageCount() == 0.
   */
  int GetCurrentPage() const override;

  /**
   * @brief Returns the total number of pages derived from content / page size.
   *
   * Returns 0 when the view has no content pages (empty state after
   * NotifyPagesRemoved removes all pages).
   */
  int GetPageCount() const override;

  /**
   * @brief Animates or snaps to the given page.
   *
   * @param[in] page    Target page index, clamped to [0, GetPageCount()-1].
   * @param[in] animate True to animate, false for instant jump.
   */
  void ScrollToPage(int page, bool animate = true) override;

  /**
   * @brief Signal emitted when the active page changes.
   *
   * Signature: void(int currentPage, int pageCount)
   */
  IPageScrollable::PageChangedSignalType& PageChangedSignal() override;

  /**
   * @brief Signal emitted from the destructor before the underlying object is freed.
   */
  IPageScrollable::DestroyingSignalType& DestroyingSignal() override;

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
   * next layout pass) so that PageIndicator updates its dot row at once.
   *
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

} // namespace Dali

#endif // DALI_UI_FOUNDATION_PAGE_SCROLL_VIEW_H
