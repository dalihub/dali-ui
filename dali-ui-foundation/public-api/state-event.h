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
#include <dali-ui-foundation/public-api/input-event.h>
#include <dali-ui-foundation/public-api/ui-state.h>

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
 * Provides the previous and current UiState together with ergonomic helpers for
 * detecting specific transitions, and optional information about the input event
 * that caused the state change.
 *
 * @code
 * view.StateChangedSignal.Connect(this, [](View, const StateEvent& e) {
 *   if(e.Added(UiState::Focused))   { ... } // gained focus
 *   if(e.Removed(UiState::Pressed)) { ... } // released
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
  const UiState& GetPrev() const;

  /**
   * @brief Returns the state after this transition (the current state).
   */
  const UiState& GetCurrent() const;

  /**
   * @brief Returns true if @a state was newly added in this transition.
   *
   * Equivalent to: !GetPrev().Contains(state) && GetCurrent().Contains(state)
   */
  bool Added(const UiState& state) const
  {
    return !GetPrev().Contains(state) && GetCurrent().Contains(state);
  }

  /**
   * @brief Returns true if @a state was removed in this transition.
   *
   * Equivalent to: GetPrev().Contains(state) && !GetCurrent().Contains(state)
   */
  bool Removed(const UiState& state) const
  {
    return GetPrev().Contains(state) && !GetCurrent().Contains(state);
  }

  /**
   * @brief Returns true if @a state was either added or removed in this transition.
   */
  bool Changed(const UiState& state) const
  {
    return Added(state) || Removed(state);
  }

  /**
   * @brief Returns true if this transition was triggered by a user input event.
   *
   * Programmatic state changes (e.g. SetEnabled()) return false.
   */
  bool HasCause() const;

  /**
   * @brief Returns the input event that caused this transition.
   *
   * @note Check HasCause() before calling. Returns an invalid handle if there is no cause.
   */
  const InputEvent& GetCause() const;

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
