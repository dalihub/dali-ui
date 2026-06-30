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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/input/input-event.h>
#include <dali-ui-foundation/public-api/types/view-state.h>

namespace Dali
{

namespace Ui
{

namespace Internal
{
class StateEventImpl;
}

/**
 * @brief Describes a single View state transition, passed to StateChangedSignal handlers.
 *
 * Provides the previous and current ViewState together with ergonomic helpers for
 * detecting specific transitions, and optional information about the input event
 * that caused the state change.
 *
 * @code
 * view.StateChangedSignal.Connect(this, [](View, const StateEvent& e) {
 *   if(e.Added(ViewState::FOCUSED))   { ... } // gained focus
 *   if(e.Removed(ViewState::PRESSED)) { ... } // released
 * });
 * @endcode
 */
class DALI_UI_API StateEvent : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized StateEvent handle.
   */
  StateEvent() = default;

  /**
   * @brief Destructor.
   */
  ~StateEvent() = default;

  StateEvent(const StateEvent&)            = default;
  StateEvent& operator=(const StateEvent&) = default;
  StateEvent(StateEvent&&)                 = default;
  StateEvent& operator=(StateEvent&&)      = default;

  /**
   * @brief Returns the state before this transition.
   */
  const ViewState& GetPrev() const;

  /**
   * @brief Returns the state after this transition (the current state).
   */
  const ViewState& GetCurrent() const;

  /**
   * @brief Returns true if @a state was newly added in this transition.
   *
   * Equivalent to: !GetPrev().Contains(state) && GetCurrent().Contains(state)
   */
  bool Added(const ViewState& state) const
  {
    return !GetPrev().Contains(state) && GetCurrent().Contains(state);
  }

  /**
   * @brief Returns true if @a state was removed in this transition.
   *
   * Equivalent to: GetPrev().Contains(state) && !GetCurrent().Contains(state)
   */
  bool Removed(const ViewState& state) const
  {
    return GetPrev().Contains(state) && !GetCurrent().Contains(state);
  }

  /**
   * @brief Returns true if @a state was either added or removed in this transition.
   */
  bool Changed(const ViewState& state) const
  {
    return Added(state) || Removed(state);
  }

  /**
   * @brief Returns the input event that caused this transition.
   *
   * Always returns a valid InputEvent. Causes without a concrete input event
   * return an InputEvent with InputEventType::NONE. Use
   * InputEvent::IsProgrammatic() and InputEvent::IsCancellation() to inspect
   * the cause flags.
   */
  const InputEvent& GetCause() const;

  /**
   * @brief Returns the type of the input event that caused this transition.
   *
   * Convenience shortcut for GetCause().GetInputEventType().
   * Returns InputEventType::NONE if the cause has no concrete input event payload.
   */
  InputEventType GetInputEventType() const;

  /**
   * @brief Returns the touch event that caused this transition.
   *
   * @pre GetInputEventType() == InputEventType::TOUCH_EVENT
   */
  const TouchEvent& GetTouchEvent() const;

  /**
   * @brief Returns the key event that caused this transition.
   *
   * @pre GetInputEventType() == InputEventType::KEY_EVENT
   */
  const KeyEvent& GetKeyEvent() const;

  /**
   * @brief Returns the tap gesture that caused this transition.
   *
   * @pre GetInputEventType() == InputEventType::TAP_GESTURE
   */
  const TapGesture& GetTapGesture() const;

  /**
   * @brief Returns the long-press gesture that caused this transition.
   *
   * @pre GetInputEventType() == InputEventType::LONG_PRESS_GESTURE
   */
  const LongPressGesture& GetLongPressGesture() const;

  /**
   * @brief Returns the wheel event that caused this transition.
   *
   * @pre GetInputEventType() == InputEventType::WHEEL_EVENT
   */
  const WheelEvent& GetWheelEvent() const;

public: // Not intended for application developers
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The implementation
   */
  explicit StateEvent(Internal::StateEventImpl* implementation);
};

} // namespace Ui

} // namespace Dali
