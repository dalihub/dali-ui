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
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/input/input-event.h>
#include <dali-ui-foundation/public-api/input/key-click-policy.h>
#include <dali-ui-foundation/public-api/traits/trait-object.h>

namespace Dali
{

namespace Ui
{

// Forward declaration (view.h provides full definition when needed)
class View;

// Forward declarations
namespace Internal
{
class CoreInteractionObject;
}

/**
 * @brief InteractiveTrait is an interaction trait that provides clickable behavior to a View.
 *
 * InteractiveTrait provides click, long-press, and pressed-state handling
 * functionality. It can be attached to any View to make it interactive.
 *
 * @note InteractiveTrait, SelectableTrait, and GroupSelectableTrait are facets of a
 * single shared interaction object on a View. Comparing handles with operator== compares
 * that same underlying object, and DownCast is presence-based (does the requested facet's
 * sub-implementation exist?) rather than identity-based.
 *
 */
class DALI_UI_API InteractiveTrait : public BaseHandle
{
public:
  // Typedefs

public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized InteractiveTrait handle.
   */
  InteractiveTrait();

  /**
   * @brief Downcasts a handle to InteractiveTrait handle.
   *
   * If the handle refers to a InteractiveTrait (e.g. a trait from GetTrait),
   * the downcast produces a valid handle. Otherwise the returned handle is uninitialized.
   *
   * @param[in] handle Handle to an object stored in View's interaction trait slot
   * @return A handle to InteractiveTrait or an uninitialized handle
   */
  static InteractiveTrait DownCast(BaseHandle handle);

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   * @param[in] interactiveTrait Handle to copy
   */
  InteractiveTrait(const InteractiveTrait& interactiveTrait);

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~InteractiveTrait();

public: // Signals
  /**
   * @brief Emitted when the pressed state changes.
   *
   * This signal is emitted when the view's pressed state changes due to touch or keyboard input.
   * The pressed state is true when the user is interacting with the view (e.g., pressing down),
   * and false when the interaction ends (e.g., releasing the touch or key).
   *
   * @return The pressed changed signal
   */
  Signal<void(View, bool, InputEvent)>& PressedChangedSignal();

  /**
   * @brief Emitted when the pseudo disabled state changes.
   *
   * This signal is emitted when the pseudo disabled state is changed via SetPseudoDisabled().
   * The view is visually presented as disabled when in pseudo disabled state, but it still
   * accepts user interaction to provide guidance or an alternative flow.
   *
   * @return The pseudo disabled changed signal
   */
  Signal<void(View, bool)>& PseudoDisabledChangedSignal();

  /**
   * @brief Emitted when the view is clicked.
   *
   * This signal is emitted when the view receives a tap gesture, indicating a click action.
   * The click action is triggered when the user taps on the view.
   *
   * @note This signal may not be emitted if the one of LongPressed signal handler consumes the event (returns true).
   *
   * @return The clicked signal
   */
  Signal<void(View, InputEvent)>& ClickedSignal();

  /**
   * @brief Emitted when the view receives a long press gesture.
   *
   * This signal is emitted when the user performs a long press on the view.
   * A long press is detected when the user presses and holds on the view for a specified duration.
   *
   * @note If the handler returns true to consume the event, the Clicked signal will not be emitted.
   * @note This signal is not emitted when ClickedKeyType is set to PRESS.
   *
   * @return The long pressed signal
   */
  Signal<bool(View, InputEvent)>& LongPressedSignal();

  /**
   * @brief Emitted when the hovered state changes.
   *
   * @return The hovered changed signal
   */
  Signal<void(View, bool, InputEvent)>& HoveredChangedSignal();

public: // API
  /**
   * @brief The boolean flag for pressed state.
   *
   * @return True if the object is pressed
   */
  bool IsPressed() const;

  /**
   * @brief Checks if the view is pseudo disabled state.
   * The view is visually presented as disabled, but it still accepts user interaction in order to provide guidance or
   * an alternative flow.
   *
   * @return True if the object is pseudo disabled
   */
  bool IsPseudoDisabled() const;

  /**
   * @brief The boolean flag for hovered state.
   *
   * @return True if the object is hovered
   */
  bool IsHovered() const;

  /**
   * @brief Sets the pseudo disabled state to the view.
   * The view is visually presented as disabled, but it still accepts user interaction in order to provide guidance or
   * an alternative flow.
   *
   * @param[in] pseudoDisabled True if the object is pseudo disabled
   */
  void SetPseudoDisabled(bool pseudoDisabled);

  /**
   * @brief Checks if clicking is currently allowed (not blocked).
   *
   * @return True if clicking is allowed, false if clicking is blocked
   */
  bool IsClickable() const;

  /**
   * @brief Sets whether clicking is allowed.
   *
   * @param[in] clickable True to allow clicking, false to block clicking
   */
  void SetClickable(bool clickable);

  /**
   * @brief Retrieves the current key click policy.
   *
   * @return The active KeyClickPolicy for this button.
   */
  KeyClickPolicy GetKeyClickPolicy() const;

  /**
   * @brief Configures the trigger timing for key-driven click events.
   *
   * @param policy The KeyClickPolicy to be applied (OnRelease, OnPress, or Disabled).
   */
  void SetKeyClickPolicy(KeyClickPolicy policy);

  /**
   * @brief Connects a handler to ClickedSignal.
   *
   * Multiple calls accumulate — each call adds an additional handler.
   * If @p obj implements ConnectionTrackerInterface (e.g. inherits ConnectionTracker),
   * the connection is automatically removed when @p obj is destroyed.
   *
   * Supports member functions and stateless lambdas:
   * @code
   * view.ConnectClickedSignal(this, &MyClass::OnButtonClicked);
   * view.ConnectClickedSignal(this, [](View v, InputEvent e) { ... });
   * @endcode
   *
   * @param[in] obj  The object to connect to (must implement ConnectionTrackerInterface for auto-disconnect)
   * @param[in] func Member function pointer or functor with signature void(View, InputEvent)
   */
  template<typename T, typename Func>
  void ConnectClickedSignal(T* obj, Func func)
  {
    ClickedSignal().Connect(obj, func);
  }

  /**
   * @brief Connects a handler to PressedChangedSignal.
   *
   * Multiple calls accumulate — each call adds an additional handler.
   * The handler is called with @c pressed=true on press-down and @c pressed=false on release.
   * If @p obj implements ConnectionTrackerInterface, the connection is auto-removed on @p obj destruction.
   *
   * @param[in] obj  The object to connect to
   * @param[in] func Member function pointer or functor with signature void(View, bool, InputEvent)
   */
  template<typename T, typename Func>
  void ConnectPressedChangedSignal(T* obj, Func func)
  {
    PressedChangedSignal().Connect(obj, func);
  }

  /**
   * @brief Connects a handler to LongPressedSignal.
   *
   * Multiple calls accumulate — each call adds an additional handler.
   * Long press fires once when the gesture is detected (on press-down after the time threshold).
   * Press release is reported separately via PressedChangedSignal.
   *
   * The handler must return bool: return @c true to suppress the subsequent click event,
   * or @c false to allow it.
   *
   * If @p obj implements ConnectionTrackerInterface, the connection is auto-removed on @p obj destruction.
   *
   * @param[in] obj  The object to connect to
   * @param[in] func Member function pointer or functor with signature bool(View, InputEvent)
   */
  template<typename T, typename Func>
  void ConnectLongPressedSignal(T* obj, Func func)
  {
    LongPressedSignal().Connect(obj, func);
  }

public: // Not intended for application developers
  /**
   * @brief Creates an internal InteractiveTrait handle.
   *
   * The returned handle stores a CoreInteractionObject and owns an interactive trait implementation.
   * Application developers should obtain this trait through View::AsInteractive().
   *
   * @return A handle to a newly allocated InteractiveTrait.
   */
  DALI_INTERNAL static InteractiveTrait New();

  /**
   * @brief Creates a handle using the internal core interaction trait object.
   *
   * @param[in] container The core interaction trait object
   * @return A handle to InteractiveTrait
   */
  DALI_INTERNAL static InteractiveTrait New(Internal::CoreInteractionObject* container);

protected:
  explicit DALI_INTERNAL InteractiveTrait(Internal::CoreInteractionObject* container);
};

} // namespace Ui

} // namespace Dali
