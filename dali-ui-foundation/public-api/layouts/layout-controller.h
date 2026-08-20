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

// EXTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/signals/dali-signal.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>

namespace Dali
{
namespace Ui
{

// Forward declarations
class ViewImpl;
namespace Integration
{
class LayoutControllerImpl;
} // namespace Integration

/**
 * @brief Layout controller manages layout invalidation and processing.
 *
 * The layout controller is responsible for scheduling layout passes when
 * layouts become invalid and processing them during the render loop.
 * It integrates with the DALi adaptor system as a processor.
 *
 * Each Window has its own LayoutController instance. The controller
 * batches layout requests and processes them once per frame to optimize
 * performance.
 */
class DALI_UI_API LayoutController
{
public:
  /**
   * @brief Gets the layout controller instance for a window.
   *
   * Creates the controller if it doesn't exist for the given window.
   *
   * @param[in] window The window to get the controller for
   * @return Reference to the layout controller
   */
  static LayoutController& Get(Window window);

  /**
   * @brief Destructor.
   */
  ~LayoutController();

  /**
   * @brief Schedules a view with layout capability for layout processing.
   *
   * Layout roots are views that have a LayoutManager and are at the top
   * of the layout hierarchy (e.g. directly under the window).
   * The controller will batch these requests and process them
   * during the next frame.
   *
   * @param[in] view The view with layout capability to schedule
   */
  void RequestLayout(ViewImpl* view);

  /**
   * @brief Removes the layout controller for the given window.
   *
   * Should be called when a window is being closed to release the
   * associated LayoutController and prevent stale entries.
   *
   * @param[in] window The window whose controller should be removed
   */
  static void Remove(Window window);

  /**
   * @brief Unregisters a view from the layout controller.
   *
   * Should be called when a layout root is being destroyed to prevent
   * dangling pointer access.
   *
   * @param[in] view The view to unregister
   */
  void UnregisterView(ViewImpl* view);

  /**
   * @brief Unregisters a view from all known layout controllers.
   *
   * Used when a view is being destroyed but its window cannot be
   * determined (e.g. the view is already off-scene).
   *
   * @param[in] view The view to unregister
   */
  static void UnregisterFromAll(ViewImpl* view);

  /**
   * @brief Called when the window is resized.
   *
   * This invalidates all layout roots and triggers a layout pass.
   *
   * @param[in] width The new window width
   * @param[in] height The new window height
   */
  void OnWindowResize(int32_t width, int32_t height);

  /**
   * @brief Processes all pending layout requests immediately.
   *
   * This is called automatically by the system once per frame,
   * but can be called manually if immediate layout is needed.
   */
  void ProcessLayouts();

  /**
   * @brief Signal type of LayoutFinishedSignal().
   */
  using LayoutFinishedSignalType = Signal<void(Dali::Window)>;

  /**
   * @brief This signal is emitted when this window's layout calculation has
   * fully settled: every layout root's Measure and Arrange for the window
   * have completed and no further Measure/Arrange work is pending.
   *
   * A slot connects with the signature:
   * @code
   *   void OnLayoutFinished(Dali::Window window);
   * @endcode
   *
   * Semantics:
   * - Fires once per "dirty -> quiescent" transition, i.e. each time pending
   *   layout work drains to nothing. It recurs whenever layout is invalidated
   *   again and settles again; it is not a one-shot for the application
   *   lifetime.
   * - Emitted during the post-process phase, i.e. AFTER DALi core size
   *   negotiation (Relayout) for the frame in which layout settled. Measure and
   *   Arrange still run in the pre-process phase; only the emit is deferred to
   *   post-process. A manual ProcessLayouts() therefore performs layout
   *   synchronously but does NOT emit this signal in the same call; the emit
   *   fires on the next post-process pass (e.g. after the next ProcessEvents).
   * - Reflects Measure/Arrange completion ONLY. It does NOT wait for layout
   *   transition animations to finish; use a transition-finished callback if
   *   post-animation geometry is required.
   * - If a slot invalidates layout again (e.g. triggers InvalidateMeasure),
   *   that schedules another pass and this signal fires again on a later
   *   frame. Avoid unconditionally re-laying-out inside the slot, which
   *   creates a self-perpetuating per-frame emit cycle.
   * - Destroying this controller from within the slot (LayoutController::Remove)
   *   is supported. The controller is detached immediately - it stops processing
   *   and emitting at once - but the object is not freed until the event loop
   *   next goes idle, because DALi core still holds a processor pointer to it
   *   for the remainder of the current processing pass.
   *
   * @return The layout-finished signal
   * @note The @c Dali::Window is passed (not the LayoutController) because the
   * controller is a non-copyable, per-window infrastructure object; the window
   * identifies which controller settled and the controller can be re-obtained
   * via Get(). Adding parameters later would require migrating to a struct
   * argument.
   */
  LayoutFinishedSignalType& LayoutFinishedSignal();

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Records @p view as arranged for View::LayoutFinishedSignal during the
   * currently active LayoutController pass. No-op outside a managed root pass.
   *
   * @param[in] view The view whose Arrange() has just completed
   */
  DALI_INTERNAL static void NotifyViewArranged(ViewImpl* view);

  /**
   * @brief Schedules an EXIT-slot layout transition for @p child under
   * @p parent.
   *
   * Called by ViewImpl::Remove / RemoveAll when the parent
   * has a LayoutTransition EXIT slot configured through a visual spec,
   * animator, or active bounds effect. The dispatcher fires the EXIT
   * animation and unparents the child only when the animation finishes.
   *
   * @param[in] parent          The child's direct (visual) parent (ghost host
   *                            / unparent target)
   * @param[in] child           The child view to remove (kept alive by a
   *                            strong ref inside the dispatcher until EXIT
   *                            completes)
   * @param[in] transitionOwner The view whose LayoutTransition drives the EXIT
   *                            effect; @c nullptr means @p parent (direct EXIT).
   *                            Differs from @p parent only for SUBTREE-scope
   *                            inherited EXIT.
   */
  DALI_INTERNAL void ScheduleLayoutExit(ViewImpl* parent, Ui::View child, ViewImpl* transitionOwner = nullptr);

  /**
   * @brief Notifies the layout transition dispatcher that @p child was
   * just attached to a (new) parent.
   *
   * Called by @c ViewImpl::OnChildAdd. When the child has an in-flight
   * transition under an old parent (reparent during EXIT), the dispatcher
   * cancels it so the application callback does not keep firing against
   * the old parent's coordinate system. No-op when there is no in-flight
   * state for @p child (the common fresh-add case).
   *
   * @param[in] child The child view whose actor was just attached
   */
  DALI_INTERNAL void NotifyChildReparented(ViewImpl* child);

  /**
   * @brief Notifies the dispatcher that @p child was added under @p directParent,
   * which has no LayoutTransition of its own, so an inherited (SUBTREE-scope)
   * ENTER candidate can be registered against the closest governing ancestor.
   *
   * Called by @c ViewImpl::OnChildAdd. No-op when no ancestor SUBTREE owner with
   * an ENTER effect governs the child.
   *
   * @param[in] directParent The child's direct (no-transition) parent
   * @param[in] child         The freshly added child
   */
  DALI_INTERNAL void NotifyChildAdded(ViewImpl* directParent, Ui::View child);

  /**
   * @brief Drops inherited-ENTER candidates registered against @p owner in the
   * dispatcher, called when @p owner detaches its LayoutTransition.
   *
   * @param[in] owner The view whose transition was just detached
   */
  DALI_INTERNAL void ClearPendingInheritedEnters(ViewImpl* owner);
  /// @endcond

private:
  /**
   * @brief Private constructor.
   *
   * Use LayoutController::Get() to obtain an instance.
   *
   * @param[in] window The window this controller manages
   */
  explicit DALI_INTERNAL LayoutController(Window window);

  // Not copyable or movable
  LayoutController(const LayoutController&)            = delete;
  LayoutController(LayoutController&&)                 = delete;
  LayoutController& operator=(const LayoutController&) = delete;
  LayoutController& operator=(LayoutController&&)      = delete;

private: // Not be opened for application developer
  /**
   * @brief Gets the current window handle managed by this layout controller.
   *
   * Retrieves the window that this layout controller instance is associated with.
   * This is used internally to verify if the window has been replaced.
   *
   * @return The current window handle
   */
  DALI_INTERNAL Dali::Window GetCurrentWindow() const;

  /**
   * @brief Replaces the current window with a new one.
   *
   * Updates the layout controller to manage a different window instance.
   * This is called when a window object has been replaced but the same
   * LayoutController instance should continue managing layouts for the new window.
   * The method reconnects the window resize signal to ensure layout invalidation
   * continues to work correctly.
   *
   * @param[in] window The new window to manage
   */
  DALI_INTERNAL void ReplaceCurrentWindow(Dali::Window window);

private:
  Dali::UniquePtr<Integration::LayoutControllerImpl> mImpl;
};

} // namespace Ui
} // namespace Dali
