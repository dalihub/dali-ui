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

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/scroll-view-impl.h>
#include <dali-ui-foundation/public-api/views/scroll/i-page-scrollable.h>
#include <dali-ui-foundation/public-api/views/scroll/page-scroll-view.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

class PageScrollViewImpl;
using PageScrollViewImplPtr = IntrusivePtr<PageScrollViewImpl>;

/**
 * @brief Internal implementation of PageScrollView.
 *
 * Subclasses ScrollViewImpl to add page-snap behaviour:
 *  - OnBeforeScrollAnimation() is overridden to redirect any fling or slow-drag
 *    finish to the nearest page boundary.
 *  - A PageChangedSignal is emitted whenever the settled page index changes.
 *
 * External libraries that need only public-api access should implement
 * IPageScrollable on their own View subclass instead of inheriting this class.
 *
 * @note ABI note: do NOT reorder or remove virtual methods after first publication.
 */
class DALI_UI_API PageScrollViewImpl : public ScrollViewImpl, public IPageScrollable
{
public:
  /**
   * @brief Creates a new PageScrollViewImpl.
   */
  static PageScrollViewImplPtr New();

protected:
  /**
   * @brief Destructor.
   */
  ~PageScrollViewImpl() override;

  /**
   * @brief Constructor.
   */
  PageScrollViewImpl();

public: // From ViewImpl
  /**
   * @brief Connects post-scroll signals after the base class is initialized.
   */
  void OnInitialize() override;

public: // Page API
  /**
   * @brief Sets the explicit page size.
   *
   * Only the component on the active scroll axis is used.
   * Set to (0, 0) to derive the page size from the current viewport.
   */
  void SetPageSize(const Vector2& size);

  /**
   * @brief Gets the explicitly configured page size ((0,0) means auto).
   */
  Vector2 GetPageSize() const;

  // IPageScrollable

  /**
   * @brief Returns the zero-based current page index, or -1 if empty.
   */
  int GetCurrentPage() const override;

  /**
   * @brief Returns the total page count derived from content / effective page size.
   *
   * When NotifyPagesInserted / NotifyPagesRemoved has been called but the
   * layout pass has not yet run, returns the pre-computed expected count
   * stored by those methods so that PageChangedSignal consumers see a
   * consistent value immediately.
   */
  int GetPageCount() const override;

  /**
   * @brief Scrolls to the given page with an optional animation.
   */
  void ScrollToPage(int page, bool animate = true) override;

  /**
   * @brief Call after dynamically inserting pages into the scroll content.
   *
   * Adjusts the current-page index (shift right if the insertion was at or
   * before the current page), scrolls instantly to keep the same content
   * page visible, and emits PageChangedSignal with the updated state.
   *
   * Must be called AFTER the pages have been added to the content layout.
   * The scroll-view layout pass may not have run yet — this method
   * computes the new page count from mScrollableSize + inserted count to
   * avoid the timing ambiguity.
   *
   * @param[in] atIndex       Zero-based index at which pages were inserted.
   * @param[in] insertedCount Number of pages inserted (default 1).
   */
  void NotifyPagesInserted(int atIndex, int insertedCount = 1);

  /**
   * @brief Call after dynamically removing pages from the scroll content.
   *
   * Adjusts the current-page index, snaps to the nearest valid page if
   * the current page was removed, and emits PageChangedSignal.
   *
   * Must be called AFTER the pages have been removed from the content layout.
   *
   * @param[in] atIndex      Zero-based index of the first removed page.
   * @param[in] removedCount Number of pages removed (default 1).
   */
  void NotifyPagesRemoved(int atIndex, int removedCount = 1);

  /**
   * @brief Signal emitted when the settled page changes.
   */
  IPageScrollable::PageChangedSignalType& PageChangedSignal() override;

  /**
   * @brief Signal emitted from the destructor before this object is freed.
   *
   * Observers (e.g. PageIndicator) connect here to null any raw pointer they
   * hold to this object.  Do not call back into the sender from the handler.
   */
  IPageScrollable::DestroyingSignalType& DestroyingSignal() override;

protected:
  /**
   * @brief Overrides ScrollViewImpl hook to snap fling/drag targets to page boundaries.
   *
   * @note ABI note: do NOT reorder or remove after first publication.
   */
  void OnBeforeScrollAnimation(Vector2& targetPosition, float& durationSec) override;

  /**
   * @brief Overrides ScrollViewImpl hook to detect when the layout pass first sets
   *        accurate viewport / scrollable-area dimensions.
   *
   * Emits PageChangedSignal when the layout-derived page count differs from the
   * last emitted count — most importantly on the very first layout pass when
   * PageIndicator::Bind() was called before layout ran.
   *
   * @note ABI note: do NOT reorder or remove after first publication.
   */
  void OnScrollableAreaChanged() override;

private:
  /**
   * @brief Returns the effective page size.
   *
   * Uses mPageSize if non-zero; falls back to the current viewport dimensions.
   */
  Vector2 GetEffectivePageSize() const;

  /**
   * @brief Returns the scroll position that centers the given page in the viewport.
   */
  Vector2 ScrollPositionForPage(int page) const;

  /**
   * @brief Returns the page index that best matches the given scroll position.
   */
  int PageForScrollPosition(const Vector2& scrollPos) const;

  /**
   * @brief Updates mCurrentPage if changed and emits PageChangedSignal.
   */
  void CommitPage(int newPage);

  /**
   * @brief Emits PageChangedSignal and records the notified page count.
   *
   * All signal emissions must go through here so that mLastNotifiedPageCount
   * stays in sync and OnScrollableAreaChanged can suppress redundant emissions.
   */
  void EmitPageChanged(int currentPage, int pageCount);

  /**
   * @brief Callback connected to ScrollFinishedSignal to keep mCurrentPage in sync.
   */
  void OnScrollFinished(Ui::ScrollView scrollView);

private:
  // Not copyable or movable
  PageScrollViewImpl(const PageScrollViewImpl&)            = delete;
  PageScrollViewImpl(PageScrollViewImpl&&)                 = delete;
  PageScrollViewImpl& operator=(const PageScrollViewImpl&) = delete;
  PageScrollViewImpl& operator=(PageScrollViewImpl&&)      = delete;

private:
  Vector2                                mPageSize{0.0f, 0.0f};      ///< Explicit page size; (0,0) = use viewport
  int                                    mCurrentPage{0};            ///< Settled page index
  int                                    mSnapTargetPage{-1};        ///< Page targeted by the in-flight snap animation; -1 = not animating
  int                                    mExpectedPageCount{-1};     ///< >=0: overrides layout-based count; -1: use layout (sentinel)
  int                                    mLastNotifiedPageCount{-2}; ///< Page count last passed to PageChangedSignal; -2 = never emitted
  bool                                   mNotifyInProgress{false};   ///< Suppress OnScrollFinished re-derive during NotifyPages*
  IPageScrollable::PageChangedSignalType mPageChangedSignal;         ///< Emitted on page settle
  IPageScrollable::DestroyingSignalType  mDestroyingSignal;          ///< Emitted from destructor for observer lifetime management
};

} // namespace Integration

} // namespace Ui

} // namespace Dali
