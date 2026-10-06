#pragma once

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-ui-foundation/public-api/focus-manager/focus-navigation-callback.h>
#include <dali-ui-foundation/public-api/views/view-focus-enums.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali/public-api/adaptor-framework/window.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal DALI_INTERNAL
{
class FocusManager;
}
/**
 * @addtogroup dali_ui_managers
 * @{
 */

/**
 * @brief Provides the functionality of handling focus navigation
 * and maintaining the two dimensional focus chain.
 *
 * It provides functionality of setting the focus and moving the focus
 * in four directions (i.e. Left, Right, Up and Down). It also draws a
 * highlight for the focused view and emits a signal when the focus
 * is changed.
 *
 * Signals
 * | %Signal Name            | Method                             |
 * |-------------------------|------------------------------------|
 * | focusChanged            | @ref FocusChangedSignal()          |
 */
class DALI_UI_API FocusManager : public BaseHandle
{
public:
  /// @brief Focus changed signal
  typedef Signal<void(View, View)> FocusChangedSignalType;

  /**
   * @brief Creates a FocusManager handle; this can be initialized with FocusManager::New().
   *
   * Calling member functions with an uninitialized handle is not allowed.
   */
  FocusManager();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~FocusManager();

  /**
   * @brief Gets the singleton of FocusManager object.
   *
   * @return A handle to the FocusManager view
   */
  static FocusManager Get();

  /**
   * @brief Sets the focus target directly in the given View's Window.
   *
   * The target Window is determined from the View's scene connection. The focus
   * is set exactly to the specified View without child delegation.
   * If the view is not focusable or not on the scene, the call fails.
   * Use RequestFocus() if you want automatic child delegation for containers.
   * If the View's Window does not hold platform input focus, only its stored
   * focus target is updated. Actual focus, indication and FocusChangedSignal
   * are unchanged until that Window gains focus. A visible Window explicitly
   * enabled with SetIndependentFocusEnabled() instead applies its own actual
   * focus while preserving focus in other Windows. Independent targets must also
   * be visible through their ancestor chain. This does not activate a Window;
   * use Window::Activate() to request an intentional Window switch.
   * Setting focus in an inactive independent Window does not change the global
   * target returned by GetCurrentFocusView(). Use GetCurrentFocusView(Window)
   * with the View's Window to query the actual result in that scope.
   * Removing or disabling a stored target cancels its reservation. Reattaching
   * or re-enabling it does not restore that reservation without a new request.
   *
   * For example, if mainView is the global target and the visible subWindow
   * allows independent focus, a successful request for subView has these results:
   * @code
   * focusManager.SetCurrentFocusView(subView);
   * View primary = focusManager.GetCurrentFocusView();          // mainView
   * View subActual = focusManager.GetCurrentFocusView(subWindow); // subView
   * @endcode
   *
   * @param[in] view The target View, which also determines the Window to update
   * @return true if the target was applied or stored for later activation, false
   *         if the request was rejected. Success alone does not guarantee actual focus.
   * @pre The FocusManager has been initialized.
   * @pre The View has been initialized.
   * @see GetCurrentFocusView(Window)
   * @see SetIndependentFocusEnabled(Window,bool)
   */
  bool SetCurrentFocusView(View view);

  /**
   * @brief Requests focus on the given view with child delegation.
   *
   * If the view is a container, focus is delegated to a focusable
   * descendant (child-first). If no descendant accepts focus and
   * the view itself is focusable, it receives focus.
   * If an ancestor has DescendantFocusBlocked set, the request is rejected.
   * An inactive Window normally stores the resolved target without changing
   * actual focus. The independent option applies the resolved target within its
   * Window, as with SetCurrentFocusView().
   *
   * @param view The view to request focus on
   * @return true if the resolved target was applied or stored for later activation
   * @pre The FocusManager has been initialized.
   * @pre The View has been initialized.
   */
  bool RequestFocus(View view);

  /**
   * @brief Gets the global current navigation focus View.
   *
   * This overload retains its global scope. Setting or moving independent focus
   * in an inactive Window does not change this result, even if that Window was
   * the target of the most recent SetCurrentFocusView() call. The global target
   * follows the existing platform focus and retention policies and is not
   * permanently associated with the main Window.
   * Use GetCurrentFocusView(Window) to query actual navigation focus within a
   * particular Window. Stored targets awaiting activation are not returned.
   *
   * @return The global current navigation focus View, or an empty handle if no
   *         global target exists. Other Windows may still have independent focus.
   * @pre The FocusManager has been initialized.
   * @see GetCurrentFocusView(Window)
   */
  View GetCurrentFocusView();

  /**
   * @brief Moves the focus to the next focusable actor in the focus
   * chain in the given direction (according to the focus traversal
   * order).
   *
   * The request is offered to containing View policies, an explicit
   * directional target, the application fallback, and finally FocusFinder.
   * If there is no current focus and a navigation Window can be determined,
   * processing starts at the application fallback. A Stay() result consumes
   * the request but returns false because no focus movement occurred.
   * Navigation stays within its Window. In an inactive Window it starts from
   * and updates that Window's stored target without changing actual focus.
   * Key navigation uses the input event's Window; this overload uses the last
   * Window that received platform focus. This does not activate a Window.
   *
   * @param direction The direction of focus movement
   * @return true if the actual or stored focus target moved
   * @pre The FocusManager has been initialized.
   */
  bool MoveFocus(FocusDirection direction);

  /**
   * @brief Clears the focus from the current focused actor if any, so
   * that no actor is focused in the focus chain.
   *
   * It will emit focus changed signal without current focused actor.
   * The stored target of the current View's Window is also removed, including
   * any deferred replacement. Other Windows' stored targets are preserved.
   * If no View is focused, the last platform-focused Window's target is removed.
   * This does not change platform Window activation.
   * @pre The FocusManager has been initialized.
   */
  void ClearFocus();

  /**
   * @brief Clears focus indication from the current focused view without clearing focus.
   *
   * This clears FOCUS_INDICATED from the current focused view. It also hides
   * the FocusManager-managed default focus indicator, if any.
   *
   * @pre The FocusManager has been initialized.
   */
  void ClearFocusIndication();

  /**
   * @brief Sets whether a view is a focus group (focus trap).
   *
   * When a view is set as a focus group, user-initiated focus navigation
   * cannot leave the view's subtree. Explicit programmatic calls such as
   * RequestFocus() and SetCurrentFocusView() may move focus outside it.
   *
   * @param view The view to be set as a focus group
   * @param isFocusGroup Whether to set the view as a focus group or not
   * @pre The FocusManager has been initialized.
   * @pre The View has been initialized.
   */
  void SetAsFocusGroup(View view, bool isFocusGroup);

  /**
   * @brief Checks whether the view is set as a focus group or not.
   *
   * @param view The view to be checked
   * @return Whether the view is set as a focus group
   * @pre The FocusManager has been initialized.
   * @pre The View has been initialized.
   */
  bool IsFocusGroup(View view) const;

  /**
   * @brief Returns the closest ancestor of the given view that is a focus group.
   *
   * @param view The view to be checked for its focus group
   * @return The focus group the given view belongs to or an empty handle if the given view
   * doesn't belong to any focus group
   */
  View GetFocusGroup(View view);

  /**
   * @brief Sets whether FocusManager's default focus indicator is enabled.
   *
   * This controls only the default focus indicator actor managed by
   * FocusManager. It does not change focus state or FOCUS_INDICATED state.
   *
   * @param enabled Whether to enable the default focus indicator
   * @pre The FocusManager has been initialized.
   */
  void SetDefaultFocusIndicatorEnabled(bool enabled);

  /**
   * @brief Gets whether FocusManager's default focus indicator is enabled.
   *
   * @return True if the default focus indicator is enabled
   * @pre The FocusManager has been initialized.
   */
  bool IsDefaultFocusIndicatorEnabled() const;

  /**
   * @brief Move the focus to prev focused actor
   *
   * An inactive target Window only updates its stored target. Repeated calls
   * continue walking the history even while actual focus is unchanged.
   */
  void MoveFocusBackward();

  /**
   * @brief Gets the device of the last focus change.
   *
   * This method returns what caused the most recent focus change,
   * allowing applications to differentiate between different input methods.
   *
   * @return The device of the last focus change
   * @pre The FocusManager has been initialized.
   */
  FocusDevice GetLastFocusChangeDevice() const;

  /**
   * @brief Gets the device name that caused the last focus change.
   *
   * This method returns the name of the input device that caused
   * the most recent focus change. For non-device inputs (like programmatic
   * focus changes), an empty string may be returned.
   *
   * @return The device name that caused the last focus change
   * @pre The FocusManager has been initialized.
   */
  const Dali::String& GetLastFocusChangeDeviceName() const;

  /**
   * @brief Sets whether to clear focus when window loses focus.
   *
   * By default, this is enabled.
   * When enabled, actual focus is cleared on Window focus loss, but the stored
   * target is preserved for restoration when the Window gains focus again.
   * When disabled, existing actual focus is retained on Window focus loss.
   * This does not allow new actual focus changes in an inactive Window:
   * SetCurrentFocusView(), RequestFocus() and navigation normally only update its
   * stored target. SetIndependentFocusEnabled() supplies a separate exception:
   * its actual focus survives native focus loss until cleared, hidden or
   * invalidated. When another Window gains focus, ordinary actual focus is replaced
   * by its stored target, or cleared if it has no valid target. No default target
   * is automatically selected on Window focus gain.
   *
   * @param enabled Whether to clear focus when window loses focus
   */
  void SetClearFocusOnWindowFocusLost(bool enabled);

  /**
   * @brief Gets whether to clear focus when window loses focus.
   *
   * @return Whether clear focus is enabled when window loses focus
   */
  bool GetClearFocusOnWindowFocusLost() const;

  /**
   * @brief Sets whether touch interaction proposes clearing focus indication.
   *
   * The final focus indication state may be overridden by a configured
   * integration focus indication policy.
   *
   * @param clear Whether touch interaction proposes clearing focus indication from the focused view
   */
  void SetClearFocusIndicationOnTouch(bool clear);

  /**
   * @brief Gets whether touch interaction proposes clearing focus indication.
   *
   * @return True if touch interaction proposes clearing focus indication from the focused view
   */
  bool IsClearFocusIndicationOnTouchEnabled() const;

  /**
   * @brief Sets whether hover outside the focused view proposes clearing focus indication.
   *
   * The final focus indication state may be overridden by a configured
   * integration focus indication policy.
   *
   * @param clear Whether hover outside the focused view proposes clearing focus indication
   */
  void SetClearFocusIndicationOnHover(bool clear);

  /**
   * @brief Gets whether hover outside the focused view proposes clearing focus indication.
   *
   * @return True if hover outside the focused view proposes clearing focus indication
   */
  bool IsClearFocusIndicationOnHoverEnabled() const;

  /**
   * @brief Sets the application-wide fallback for focus navigation.
   *
   * The fallback is invoked after View-local navigation policies and an
   * explicit directional target have not handled the request, and before the
   * framework FocusFinder. It is also the first policy invoked when there is
   * no current focus and a navigation Window can be determined. Return
   * NotHandled() to use FocusFinder, MoveTo() to select a candidate, or Stay()
   * to consume the request without moving focus.
   *
   * Only one fallback is stored. Setting a new callback replaces the previous
   * one, and passing an empty callback clears it. The callback target must
   * remain alive until the callback is replaced or cleared. The callback must
   * return a result instead of changing focus directly.
   *
   * @param[in] callback The move-only fallback callback
   */
  void SetFocusNavigationFallback(FocusNavigationCallback callback);

public: // Signals
  /**
   * @brief This signal is emitted after the current focused view has been changed.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(View originalFocusedView, View currentFocusedView);
   * @endcode
   * @return The signal to connect to
   * @pre The Object has been initialized.
   */
  FocusChangedSignalType& FocusChangedSignal();

  /**
   * @brief Allows a Window to retain actual View focus independently of platform activation.
   *
   * Disabled by default. Enabling does not apply a stored target; explicitly
   * request focus afterward. An inactive enabled Window does not replace the
   * global focus. Disabling an inactive Window clears its actual and stored
   * targets. Disabling the active Window preserves its global focus.
   * Hidden Windows release independent actual focus and require a new request
   * after showing, including an explicit MoveFocus() to choose an initial target.
   * Targets must be effectively visible through their ancestor chain.
   * InputField, InputEditor and custom IME sessions are not
   * supported in an independent Window.
   * @param[in] window The Window whose policy is changed
   * @param[in] enabled Whether independent focus is enabled
   * @return True if applied, false for an invalid Window, unsupported target,
   *         or a request made during a navigation callback
   * @pre Called on the UI thread.
   */
  bool SetIndependentFocusEnabled(Window window, bool enabled);

  /**
   * @brief Queries whether the Window allows independent actual focus.
   * @param[in] window The Window to query
   * @return True if enabled, false otherwise, including an empty Window
   * @pre Called on the UI thread.
   */
  bool IsIndependentFocusEnabled(Window window) const;

  /**
   * @brief Returns actual navigation focus in the Window, excluding stored targets.
   *
   * This overload queries the specified Window's scope. For an independent
   * Window, it returns that Window's actual navigation target even while the
   * Window is inactive. For an ordinary Window, it returns the global target
   * only when that target belongs to the specified Window.
   * An inactive independent Window may therefore return a different View from
   * GetCurrentFocusView(). A separately configured key input target may also
   * differ from the navigation target returned here. Querying does not activate
   * the Window or apply a stored target.
   * @param[in] window The Window to query
   * @return Its actual navigation focus View, or an empty handle if no actual
   *         target exists or the Window is empty
   * @pre Called on the UI thread.
   * @see GetCurrentFocusView()
   * @see SetCurrentFocusView(View)
   */
  View GetCurrentFocusView(Window window);

  /**
   * @brief Clears actual focus, key input and the stored target in the Window only.
   *
   * Focus history is retained for a later explicit MoveFocusBackward(window).
   * @param[in] window The Window to clear; an empty Window is ignored
   * @pre Called on the UI thread, outside a navigation callback.
   */
  void ClearFocus(Window window);

  /**
   * @brief Navigates within the Window without activating it.
   *
   * Independent Windows navigate from actual focus. If actual focus is empty
   * after enabling or showing, navigation policies receive an empty current View
   * and can select an initial target. An ordinary inactive Window continues to
   * navigate from its stored target and stores the destination for activation.
   * @param[in] window The Window in which navigation is performed
   * @param[in] direction The navigation direction
   * @return True if the destination was applied or stored, false otherwise
   * @pre Called on the UI thread, outside a navigation callback.
   */
  bool MoveFocus(Window window, FocusDirection direction);

  /**
   * @brief Navigates backward through the Window's focus history only.
   *
   * Searches from the most recent entry, skipping the current navigation target
   * and candidates that cannot receive focus. Independent Windows use actual
   * focus as their current target; ordinary Windows use their stored target.
   * Active and visible independent Windows apply actual focus. An ordinary
   * inactive Window only stores the destination, and repeated calls continue
   * through its history without activating the Window.
   * If no current target exists, the most recent valid entry is restored, even
   * when the history contains only one entry. If no valid destination exists,
   * actual focus and the stored target remain unchanged.
   * @param[in] window The Window in which navigation is performed
   * @pre Called on the UI thread, outside a navigation callback.
   * @see MoveFocus(Window, FocusDirection)
   * @see ClearFocus(Window)
   */
  void MoveFocusBackward(Window window);

  /**
   * @brief Clears focus indication in the Window without releasing actual focus.
   * @param[in] window The Window whose focus indication is cleared
   * @pre Called on the UI thread.
   */
  void ClearFocusIndication(Window window);

  /**
   * @brief Actual navigation focus changes within a Window: Window, previous, current.
   */
  using WindowFocusChangedSignalType = Signal<void(Window, View, View)>;

  /**
   * @brief Returns the signal for actual navigation focus changes in a Window.
   * Stored requests and changes of a Window's global role alone do not emit it.
   * The callback receives the Window, previous View and current View.
   * @return The signal to connect to
   * @pre The FocusManager has been initialized.
   */
  WindowFocusChangedSignalType& WindowFocusChangedSignal();

  // Not intended for application developers

  /// @cond internal
  /**
   * @brief Creates a new handle from the implementation.
   *
   * @param[in] impl A pointer to the object
   */
  explicit DALI_INTERNAL FocusManager(Internal::FocusManager* impl);
  /// @endcond

}; // class FocusManager

/**
 * @}
 */
} // namespace Ui

} //namespace DALI_NAMESPACE
