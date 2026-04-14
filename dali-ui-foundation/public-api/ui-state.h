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
#include <dali/public-api/common/dali-string-view.h>
#include <dali/public-api/common/dali-string.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{

namespace Ui
{

/**
 * @brief UiState is a value type representing one or more view states as a bitmask.
 *
 * Predefined states (Focused, Pressed, Disabled, etc.) are registered at static
 * initialization time. Custom states can be created at runtime via Create().
 *
 * States can be combined with operator+, removed with operator-, and tested with
 * Contains() or HasIntersectionWith().
 *
 * @code
 * // Single state check
 * if(state.Contains(UiState::PRESSED)) { ... }
 *
 * // Composite state
 * auto selectedPressed = UiState::SELECTED + UiState::PRESSED;
 *
 * // Custom state
 * static const UiState Loading = UiState::Create("Loading");
 * @endcode
 */
class DALI_UI_API UiState
{
public:
  /**
   * @brief Creates a Normal state (no bits set).
   */
  constexpr UiState()
  : mBits(0ULL)
  {
  }

  // --- Predefined States ---

  static const UiState ALL;             ///< Represents all possible states
  static const UiState NORMAL;          ///< No state (0)
  static const UiState FOCUSED;         ///< View has keyboard focus
  static const UiState PRESSED;         ///< View is being pressed (touch or key)
  static const UiState DISABLED;        ///< View is disabled
  static const UiState PSEUDO_DISABLED; ///< View appears disabled but still interactive
  static const UiState SELECTED;        ///< View is selected

  // --- Predefined Composite States ---

  static const UiState SELECTED_PRESSED;  ///< SELECTED + PRESSED
  static const UiState DISABLED_SELECTED; ///< DISABLED + SELECTED
  static const UiState SELECTED_FOCUSED;  ///< SELECTED + FOCUSED

  /**
   * @brief Registers and creates a custom state with the given name.
   *
   * If the name was already registered, the same bitmask is returned.
   * "Normal" always returns zero, "All" always returns full mask.
   *
   * @param[in] name The unique name for the state
   * @return The UiState with the assigned bitmask
   * @throws DaliException if no more bits are available (max 62 custom states)
   */
  static UiState Create(StringView name);

  // --- Query ---

  /**
   * @brief Checks if this state fully contains the specified state.
   *
   * @param[in] other The state to check for
   * @return True if all bits of @p other are set in this state
   */
  bool Contains(const UiState& other) const
  {
    return (mBits & other.mBits) == other.mBits;
  }

  /**
   * @brief Checks if there is any overlap between this and the other state.
   *
   * @param[in] other The state to check against
   * @return True if any common bit is set
   */
  bool HasIntersectionWith(const UiState& other) const
  {
    return (mBits & other.mBits) != 0ULL;
  }

  /**
   * @brief Checks if this is the Normal state (no bits set).
   *
   * @return True if no state bits are set
   */
  bool IsNormal() const
  {
    return mBits == 0ULL;
  }

  /**
   * @brief Checks if this state contains DISABLED or PSEUDO_DISABLED.
   *
   * @return True if either DISABLED or PSEUDO_DISABLED is set
   */
  bool IsAnyDisabled() const
  {
    return Contains(DISABLED) || Contains(PSEUDO_DISABLED);
  }

  /**
   * @brief Returns a human-readable string representation for debugging.
   *
   * @return A string like "Focused, Pressed" or "Normal"
   */
  String ToString() const;

  // --- State Change Filters ---
  // Useful with StateChangedSignal(prev, cur) to detect specific transitions.

  /**
   * @brief Checks if this state was newly added (not in prev, present in cur).
   */
  bool WasAdded(const UiState& prev, const UiState& cur) const
  {
    return !prev.Contains(*this) && cur.Contains(*this);
  }

  /**
   * @brief Checks if this state was removed (present in prev, not in cur).
   */
  bool WasRemoved(const UiState& prev, const UiState& cur) const
  {
    return prev.Contains(*this) && !cur.Contains(*this);
  }

  /**
   * @brief Checks if this state was either added or removed.
   */
  bool WasChanged(const UiState& prev, const UiState& cur) const
  {
    return WasAdded(prev, cur) || WasRemoved(prev, cur);
  }

  /**
   * @brief Checks if any bit in this state changed between prev and cur.
   *
   * Unlike WasChanged(), this works with composite states — it returns true
   * if any constituent bit was toggled.
   */
  bool AnyChanged(const UiState& prev, const UiState& cur) const
  {
    return ((prev ^ cur) & *this).mBits != 0ULL;
  }

  // --- Operators ---

  UiState operator+(const UiState& rhs) const
  {
    return UiState(mBits | rhs.mBits);
  }

  UiState operator-(const UiState& rhs) const
  {
    return UiState(mBits & ~rhs.mBits);
  }

  UiState operator|(const UiState& rhs) const
  {
    return UiState(mBits | rhs.mBits);
  }

  UiState operator&(const UiState& rhs) const
  {
    return UiState(mBits & rhs.mBits);
  }

  UiState operator^(const UiState& rhs) const
  {
    return UiState(mBits ^ rhs.mBits);
  }

  UiState operator~() const
  {
    return UiState(~mBits);
  }

  bool operator==(const UiState& rhs) const
  {
    return mBits == rhs.mBits;
  }

  bool operator!=(const UiState& rhs) const
  {
    return mBits != rhs.mBits;
  }

  explicit operator bool() const
  {
    return mBits != 0ULL;
  }

private:
  explicit constexpr UiState(uint64_t bits)
  : mBits(bits)
  {
  }

  static constexpr int MAX_BITS = 62;
  static uint64_t      Register(StringView name);

  uint64_t mBits;
};

} // namespace Ui

} // namespace Dali
