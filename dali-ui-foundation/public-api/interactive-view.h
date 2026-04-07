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
#include <dali-ui-foundation/public-api/input-event.h>
#include <dali-ui-foundation/public-api/key-click-policy.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{
class InteractiveViewImpl;
}

#include "interactive-view.autogen.h"
/**
 * @brief InteractiveView is a View subclass with interactive behavior built in.
 *
 * InteractiveView guarantees that an InteractiveTrait is attached for the
 * lifetime of the view. It exposes the InteractiveTrait API directly, so
 * callers do not need to go through EnsureInteractiveTrait().
 *
 * InteractiveView is intended as a base class for clickable UI components
 * such as Button and Checkbox. It can also be instantiated directly when
 * a plain interactive container is needed.
 *
 * @see InteractiveTrait
 */
class DALI_UI_API InteractiveView : public View
{
public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized InteractiveView handle.
   */
  InteractiveView();

  /**
   * @brief Creates an initialized InteractiveView.
   *
   * @return A handle to a newly allocated InteractiveView
   */
  static InteractiveView New();

  /**
   * @brief Downcasts a handle to an InteractiveView handle.
   *
   * If the handle refers to an InteractiveView, the downcast produces a valid handle.
   * Otherwise the returned handle is uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to an InteractiveView or an uninitialized handle
   */
  static InteractiveView DownCast(BaseHandle handle);

  /**
   * @brief Copy constructor.
   *
   * @param[in] view Handle to copy
   */
  InteractiveView(const InteractiveView& view);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  InteractiveView(InteractiveView&& rhs) noexcept;

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~InteractiveView();

public: // Operators
  /**
   * @brief Copy assignment operator.
   *
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  InteractiveView& operator=(const InteractiveView& handle) = default;

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  InteractiveView& operator=(InteractiveView&& rhs) noexcept = default;

public: // Signals
  /**
   * @brief Emitted when the pressed state changes.
   *
   * @return The pressed changed signal
   * @see InteractiveTrait::PressedChangedSignal
   */
  Signal<void(View, bool, const InputEvent&)>& PressedChangedSignal();

  /**
   * @brief Emitted when the pseudo disabled state changes.
   *
   * @return The pseudo disabled changed signal
   * @see InteractiveTrait::PseudoDisabledChangedSignal
   */
  Signal<void(View, bool)>& PseudoDisabledChangedSignal();

  /**
   * @brief Emitted when the view is clicked.
   *
   * @return The clicked signal
   * @see InteractiveTrait::ClickedSignal
   */
  Signal<void(View, const InputEvent&)>& ClickedSignal();

  /**
   * @brief Emitted when the view receives a long press gesture.
   *
   * @return The long pressed signal
   * @see InteractiveTrait::LongPressedSignal
   */
  Signal<bool(View, const InputEvent&)>& LongPressedSignal();

public: // API
  // @CHAIN_START(InteractiveView, View)

  /**
   * @brief Returns whether the view is currently in the pressed state.
   *
   * @return True if pressed
   * @see InteractiveTrait::IsPressed
   */
  bool IsPressed() const;

  /**
   * @brief Returns whether the view is in the pseudo disabled state.
   *
   * @return True if pseudo disabled
   * @see InteractiveTrait::IsPseudoDisabled
   */
  bool IsPseudoDisabled() const;

  /**
   * @brief Sets the pseudo disabled state.
   *
   * @param[in] pseudoDisabled True to set pseudo disabled
   * @see InteractiveTrait::SetPseudoDisabled
   */
  InteractiveView& SetPseudoDisabled(bool pseudoDisabled);

  /**
   * @brief Returns whether clicking is currently allowed.
   *
   * @return True if clicking is allowed
   * @see InteractiveTrait::IsClickable
   */
  bool IsClickable() const;

  /**
   * @brief Sets whether clicking is allowed.
   *
   * @param[in] clickable True to allow clicking
   * @see InteractiveTrait::SetClickable
   */
  InteractiveView& SetClickable(bool clickable);

  /**
   * @brief Returns the current key click policy.
   *
   * @return The active KeyClickPolicy
   * @see InteractiveTrait::GetKeyClickPolicy
   */
  KeyClickPolicy GetKeyClickPolicy() const;

  /**
   * @brief Sets the key click policy.
   *
   * @param[in] policy The KeyClickPolicy to apply
   * @see InteractiveTrait::SetKeyClickPolicy
   */
  InteractiveView& SetKeyClickPolicy(KeyClickPolicy policy);

public: // Signal connection helpers
  // @CHAIN_MANUAL
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
   * view.ConnectClickedSignal(this, [](View v, const InputEvent& e) { ... });
   * @endcode
   *
   * @param[in] obj  The object to connect to (must implement ConnectionTrackerInterface for auto-disconnect)
   * @param[in] func Member function pointer or functor with signature void(View, const InputEvent&)
   * @return Reference to this for method chaining
   */
  template<typename T, typename Func>
  InteractiveView& ConnectClickedSignal(T* obj, Func func)
  {
    ClickedSignal().Connect(obj, func);
    return *this;
  }

  // @CHAIN_MANUAL
  /**
   * @brief Connects a handler to PressedChangedSignal.
   *
   * Multiple calls accumulate — each call adds an additional handler.
   * The handler is called with @c pressed=true on press-down and @c pressed=false on release.
   * If @p obj implements ConnectionTrackerInterface, the connection is auto-removed on @p obj destruction.
   *
   * @param[in] obj  The object to connect to
   * @param[in] func Member function pointer or functor with signature void(View, bool, const InputEvent&)
   * @return Reference to this for method chaining
   */
  template<typename T, typename Func>
  InteractiveView& ConnectPressedChangedSignal(T* obj, Func func)
  {
    PressedChangedSignal().Connect(obj, func);
    return *this;
  }

  // @CHAIN_MANUAL
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
   * @param[in] func Member function pointer or functor with signature bool(View, const InputEvent&)
   * @return Reference to this for method chaining
   */
  template<typename T, typename Func>
  InteractiveView& ConnectLongPressedSignal(T* obj, Func func)
  {
    LongPressedSignal().Connect(obj, func);
    return *this;
  }

  // @CHAIN_END

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The InteractiveView implementation
   */
  explicit DALI_UI_API InteractiveView(Integration::InteractiveViewImpl& implementation);

  /**
   * @brief Allows the creation of this InteractiveView from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_UI_API InteractiveView(Dali::Internal::CustomActor* internal);
  /// @endcond

public:
  DALI_UI_CHAIN_VIEW_METHODS(InteractiveView)
};

} // namespace Ui

} // namespace Dali
