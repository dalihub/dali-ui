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
#include <dali-ui-components/public-api/navigator/navigator-properties.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/views/view.h>

// EXTERNAL INCLUDES
#include <dali/public-api/signals/dali-signal.h>
#include <cstdint>
#include <functional>

namespace Dali
{
namespace Ui
{
namespace Integration DALI_INTERNAL
{
class NavigatorImpl;
}

/**
 * @brief Navigator is a page-stack based navigation container.
 *
 * It manages a navigation stack and a modal stack. Pages are pushed and popped
 * with optional fade transitions; modal content (e.g. a DialogContainer) is
 * shown above the navigation stack and dismissed via NavigateBack or by tapping
 * a DialogContainer's scrim.
 */
class DALI_UI_API Navigator : public View
{
public:
  Navigator();
  static Navigator New();
  Navigator(const Navigator& navigator);
  Navigator(Navigator&& rhs) noexcept;
  ~Navigator();
  Navigator& operator=(const Navigator& handle);
  Navigator& operator=(Navigator&& rhs) noexcept;

  DALI_UI_VIEW_WITH(Navigator)

  static Navigator DownCast(BaseHandle handle);

public: // Navigation stack
  /**
   * @brief Pushes a page onto the navigation stack.
   * @param[in] page     The page view to show
   * @param[in] animated Whether to fade the transition (default true)
   */
  void Push(View page, bool animated = true);

  /**
   * @brief Pops the top page off the navigation stack.
   *
   * The last page may be popped, leaving the navigation stack empty.
   * (NavigateBack(), by contrast, never pops the last page and returns false.)
   * @param[in] animated Whether to fade the transition (default true)
   * @return The popped page, or an empty handle if the stack was already empty
   */
  View Pop(bool animated = true);

  /**
   * @brief Inserts a page below the top of the navigation stack (no transition).
   * @param[in] page   The page to insert
   * @param[in] before An existing page; @p page is inserted before it
   */
  void InsertBefore(View page, View before);

  /**
   * @brief Removes a page from either stack. Removing the top page pops it.
   * @param[in] page The page to remove
   */
  void Remove(View page);

  /**
   * @brief Removes all pages from both stacks.
   */
  void Clear();

public: // Modal stack
  /**
   * @brief Pushes modal content (shown above the navigation stack).
   * @param[in] modal    The modal content (e.g. a DialogContainer)
   * @param[in] animated Whether to fade the transition (default true)
   */
  void PushModal(View modal, bool animated = true);

  /**
   * @brief Pops the top modal content.
   * @param[in] animated Whether to fade the transition (default true)
   * @return The popped modal content, or an empty handle if the modal stack is empty
   */
  View PopModal(bool animated = true);

public: // Queries
  /**
   * @brief Gets the current top-most view (modal top if any, else navigation top).
   * @return The current view, or an empty handle if both stacks are empty
   */
  View GetCurrentView() const;

  /// @brief Returns the number of pages in the navigation stack.
  uint32_t GetNavigationStackCount() const;
  /// @brief Returns the number of items in the modal stack.
  uint32_t GetModalStackCount() const;
  /// @brief Returns the navigation-stack page at @p index (bottom = 0), or an empty handle.
  View GetNavigationStackItem(uint32_t index) const;
  /// @brief Returns the modal-stack item at @p index (bottom = 0), or an empty handle.
  View GetModalStackItem(uint32_t index) const;

public: // Back navigation
  /**
   * @brief Navigates back: dismisses the top modal, else pops the navigation stack.
   *
   * A page's registered back handler (see SetBackHandler) may consume the event.
   * @return True if the back navigation was handled, false if there was nothing to go back to
   */
  bool NavigateBack();

  /**
   * @brief Registers a back handler for a page.
   * @param[in] page    The page
   * @param[in] handler Returns true to consume Back (Navigator does not pop), false to let Navigator pop
   */
  void SetBackHandler(View page, std::function<bool()> handler);

public: // Signals
  using PageEventSignalType          = Signal<void(Navigator, View, bool /*byPop*/)>;
  using TransitionFinishedSignalType = Signal<void(Navigator)>;

  PageEventSignalType&          PageWillAppearSignal();
  PageEventSignalType&          PageDidAppearSignal();
  PageEventSignalType&          PageWillDisappearSignal();
  PageEventSignalType&          PageDidDisappearSignal();
  TransitionFinishedSignalType& TransitionFinishedSignal();

public: // Not intended for application developers
  /// @cond internal
  DALI_INTERNAL          Navigator(Integration::NavigatorImpl& implementation);
  explicit DALI_INTERNAL Navigator(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
