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
#include <dali-ui-components/public-api/navigator/navigation-transition-spec.h>
#include <dali-ui-components/public-api/navigator/navigator-properties.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/views/view.h>

// EXTERNAL INCLUDES
#include <dali/public-api/signals/dali-signal.h>
#include <cstdint>

#define DALI_UI_NAVIGATOR_HAS_PAGE_ANIMATION_SWITCH 1
#define DALI_UI_NAVIGATOR_HAS_MODAL_TRANSITION_SPEC 1
#define DALI_UI_NAVIGATOR_HAS_MODAL_ANIMATION_SWITCH 1

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
class DALI_UI_COMPONENTS_API Navigator : public View
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
   * @brief Signal emitted before Navigator performs automatic back navigation.
   *
   * Parameters are this Navigator and the current view. A callback may return
   * true to consume Back and keep the current view in place. When multiple
   * callbacks are connected, Back is consumed if any callback returns true.
   */
  using BackRequestedSignalType = Signal<bool(Navigator, View)>;

  /**
   * @brief Navigates back: dismisses the top modal, else pops the navigation stack.
   *
   * A callback connected to BackRequestedSignal() may consume the event.
   * @return True if the back navigation was handled, false if there was nothing to go back to
   */
  bool NavigateBack();

  /**
   * @brief Gets the signal emitted before automatic back navigation.
   *
   * The current view is supplied to each callback so a callback can limit
   * itself to one page. Return true to consume Back, or false to let Navigator
   * dismiss the current modal or pop the current page.
   * @return The back-requested signal
   */
  BackRequestedSignalType& BackRequestedSignal();

public: // Transition customization
  /**
   * @brief Enables or disables page transition animation.
   *
   * When disabled, Push/Pop finish immediately even if their @p animated
   * parameter is true. PushModal/PopModal are controlled independently by
   * SetModalTransitionAnimationEnabled().
   * @param[in] enabled True to allow page transition animation
   */
  void SetPageTransitionAnimationEnabled(bool enabled);

  /**
   * @brief Gets whether page transition animation is enabled.
   * @return True if page transition animation is enabled
   */
  bool IsPageTransitionAnimationEnabled() const;

  /**
   * @brief Enables or disables modal transition animation.
   *
   * When disabled, PushModal/PopModal finish immediately even if their
   * @p animated parameter is true. This also affects automatic modal dismiss
   * paths, such as tapping a DialogContainer scrim.
   * @param[in] enabled True to allow modal transition animation
   */
  void SetModalTransitionAnimationEnabled(bool enabled);

  /**
   * @brief Gets whether modal transition animation is enabled.
   * @return True if modal transition animation is enabled
   */
  bool IsModalTransitionAnimationEnabled() const;

  /**
   * @brief Sets the default transition specification for all pages.
   *
   * Per-page specifications registered with SetPageTransitionSpec() take
   * precedence.
   * @param[in] spec The transition specification
   */
  void SetTransitionSpec(NavigationTransitionSpec spec);

  /**
   * @brief Clears the default page transition specification.
   *
   * Navigator uses its built-in fade transition after the specification is
   * cleared.
   */
  void ClearTransitionSpec();

  /**
   * @brief Sets a transition specification for one page.
   *
   * When the page is removed from Navigator, its per-page specification is
   * removed automatically.
   * @param[in] page The target page
   * @param[in] spec The transition specification
   */
  void SetPageTransitionSpec(View page, NavigationTransitionSpec spec);

  /**
   * @brief Clears the transition specification for one page.
   * @param[in] page The target page
   */
  void ClearPageTransitionSpec(View page);

  /**
   * @brief Sets the default modal transition specification for all modal views.
   *
   * This specification is used only for PushModal/PopModal. It is independent
   * from SetTransitionSpec(), which is used only for Push/Pop.
   * @param[in] spec The modal transition specification
   */
  void SetModalTransitionSpec(NavigationTransitionSpec spec);

  /**
   * @brief Clears the default modal transition specification.
   *
   * Navigator uses its built-in modal fade transition after the specification
   * is cleared.
   */
  void ClearModalTransitionSpec();

  /**
   * @brief Sets a modal transition specification for one view.
   *
   * This override is used only when the view participates in PushModal/PopModal.
   * When the view is removed from Navigator, its per-page modal specification
   * is removed automatically.
   * @param[in] page The target page or modal view
   * @param[in] spec The modal transition specification
   */
  void SetPageModalTransitionSpec(View page, NavigationTransitionSpec spec);

  /**
   * @brief Clears the modal transition specification for one view.
   * @param[in] page The target page or modal view
   */
  void ClearPageModalTransitionSpec(View page);

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
