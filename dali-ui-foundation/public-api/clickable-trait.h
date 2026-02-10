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
#include <dali-ui-foundation/public-api/trait.h>
#include <dali-ui-foundation/public-api/key-click-policy.h>
#include <dali-ui-foundation/public-api/input-event.h>

namespace Dali
{

namespace UI
{

// Forward declaration (view.h provides full definition when needed)
class View;

// Forward declarations
namespace Integration
{
class ClickableTraitImpl;
}

/**
 * @brief ClickableTrait is an interaction trait that provides clickable behavior to a View.
 *
 * ClickableTrait provides click, long-press, and pressed-state handling
 * functionality. It can be attached to any View to make it interactive.
 *
 */
class DALI_UI_API ClickableTrait : public Trait
{
public:

  // Typedefs

public: // Creation & Destruction

  /**
   * @brief Creates an uninitialized Trait handle.
   */
  ClickableTrait();

  /**
   * @brief Creates an initialized Trait.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static ClickableTrait New();

  /**
   * @brief Downcasts a handle to ClickableTrait handle.
   *
   * If the handle refers to a ClickableTrait (e.g. a trait from GetTrait),
   * the downcast produces a valid handle. Otherwise the returned handle is uninitialized.
   *
   * @param[in] handle Handle to an object (e.g. Trait from View's interaction trait)
   * @return A handle to ClickableTrait or an uninitialized handle
   */
  static ClickableTrait DownCast(BaseHandle handle);

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   * @param[in] clickableTrait Handle to copy
   */
  ClickableTrait(const ClickableTrait& clickableTrait);

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ClickableTrait();

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
  Signal<bool(View, const InputEvent&)>& GetPressedChangedSignal();

  /**
   * @brief Emitted when the pseudo disabled state changes.
   *
   * This signal is emitted when the pseudo disabled state is changed via SetPseudoDisabled().
   * The view is visually presented as disabled when in pseudo disabled state, but it still
   * accepts user interaction to provide guidance or an alternative flow.
   *
   * @return The pseudo disabled changed signal
   */
  Signal<void(View)>& GetPseudoDisabledChangedSignal();

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
  Signal<bool(View, const InputEvent&)>& GetClickedSignal();

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
  Signal<bool(View, const InputEvent&)>& GetLongPressedSignal();

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

public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The implementation
   */
  explicit ClickableTrait(Integration::ClickableTraitImpl* implementation);
};

} // namespace UI

} // namespace Dali
