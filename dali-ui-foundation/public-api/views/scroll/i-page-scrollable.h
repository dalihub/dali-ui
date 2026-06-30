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

#ifndef DALI_UI_FOUNDATION_I_PAGE_SCROLLABLE_H
#define DALI_UI_FOUNDATION_I_PAGE_SCROLLABLE_H

// EXTERNAL INCLUDES
#include <dali/public-api/signals/dali-signal.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{

namespace Ui
{

/**
 * @brief Abstract interface for views that support discrete page-based navigation.
 *
 * Any component that presents content as a sequence of pages — including
 * PageScrollView, a custom CarouselView, or a 3-D cube-transition view —
 * implements this interface.  PageIndicator and other consumers bind to
 * IPageScrollable rather than to a concrete type, so they work with any
 * compliant implementation.
 *
 * Implementors MUST:
 *  - emit PageChangedSignal(newPage, totalPages) when the active page changes.
 *  - keep GetCurrentPage() and GetPageCount() accurate at all times.
 *  - clamp the @p page argument of ScrollToPage() to [0, GetPageCount()-1].
 *
 * @note ABI stability: once this interface is published, no methods may be
 * added, removed, or reordered in a binary-compatible release.
 */
class DALI_UI_API IPageScrollable
{
public:
  /**
   * @brief Signal type emitted when the active page changes.
   *
   * First argument:  new current page index (0-based).
   * Second argument: total number of pages.
   */
  using PageChangedSignalType = Dali::Signal<void(int /*currentPage*/, int /*pageCount*/)>;

  /**
   * @brief Signal type emitted from the destructor body, before any members
   * are torn down.  Observers (e.g. PageIndicator) connect here to null their
   * raw pointer before the object is freed.
   */
  using DestroyingSignalType = Dali::Signal<void()>;

  /**
   * @brief Virtual destructor.
   */
  virtual ~IPageScrollable() = default;

  /**
   * @brief Returns the zero-based index of the currently visible page.
   *
   * Returns -1 when GetPageCount() == 0 (no pages present).
   */
  virtual int GetCurrentPage() const = 0;

  /**
   * @brief Returns the total number of pages.
   *
   * Returns 0 when the view has no content pages (empty state).
   */
  virtual int GetPageCount() const = 0;

  /**
   * @brief Scrolls to the given page index.
   *
   * @param[in] page    Target page index, clamped to [0, GetPageCount()-1].
   * @param[in] animate True to animate the transition, false for an instant jump.
   */
  virtual void ScrollToPage(int page, bool animate = true) = 0;

  /**
   * @brief Signal emitted when the active page changes.
   */
  virtual PageChangedSignalType& PageChangedSignal() = 0;

  /**
   * @brief Signal emitted from the destructor before the object is freed.
   *
   * Connect here (rather than storing a raw pointer) to null any observer
   * references before this object's memory is reclaimed.  The signal fires
   * with no arguments so observers must not call back into the sender.
   */
  virtual DestroyingSignalType& DestroyingSignal() = 0;
};

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_FOUNDATION_I_PAGE_SCROLLABLE_H
