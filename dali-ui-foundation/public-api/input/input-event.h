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
#include <dali/public-api/events/hover-event.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/events/long-press-gesture.h>
#include <dali/public-api/events/tap-gesture.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/events/wheel-event.h>
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/input/input-event-type.h>

namespace Dali
{

namespace Ui
{

namespace Internal
{
class InputEventImpl;
}

/**
 * @brief
 */
class DALI_UI_API InputEvent : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized InputEvent handle.
   */
  InputEvent() = default;

  /**
   * @brief Destructor.
   */
  ~InputEvent() = default;

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle Handle to copy
   */
  InputEvent(const InputEvent& handle) = default;

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  InputEvent(InputEvent&& rhs) noexcept = default;

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  InputEvent& operator=(const InputEvent& handle) = default;

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  InputEvent& operator=(InputEvent&& rhs) noexcept = default;

  /**
   * @brief Returns a shared InputEvent for programmatic changes.
   *
   * The returned event has InputEventType::PROGRAMMATIC. Use this when emitting
   * signals for explicit API calls or framework initiated state changes.
   *
   * @return A const reference to the shared programmatic InputEvent
   */
  static const InputEvent& Programmatic();

  /**
   * @brief Creates a new InputEvent from a touch event.
   *
   * @param[in] originEvent The originating touch event
   * @return A handle to the new InputEvent
   */
  static InputEvent New(const TouchEvent& originEvent);

  /**
   * @brief Creates a new InputEvent from a key event.
   *
   * @param[in] originEvent The originating key event
   * @return A handle to the new InputEvent
   */
  static InputEvent New(const KeyEvent& originEvent);

  /**
   * @brief Creates a new InputEvent from a tap gesture.
   *
   * @param[in] originEvent The originating tap gesture
   * @return A handle to the new InputEvent
   */
  static InputEvent New(const TapGesture& originEvent);

  /**
   * @brief Creates a new InputEvent from a long-press gesture.
   *
   * @param[in] originEvent The originating long-press gesture
   * @return A handle to the new InputEvent
   */
  static InputEvent New(const LongPressGesture& originEvent);

  /**
   * @brief Creates a new InputEvent from a wheel event.
   *
   * @param[in] originEvent The originating wheel event
   * @return A handle to the new InputEvent
   */
  static InputEvent New(const WheelEvent& originEvent);

  /**
   * @brief Creates a new InputEvent from a hover event.
   *
   * @param[in] originEvent The originating hover event
   * @return A handle to the new InputEvent
   */
  static InputEvent New(const HoverEvent& originEvent);

  /**
   * @brief Get the type of this event.
   *
   * @return The type of the input event
   */
  InputEventType GetInputEventType() const;

  /**
   * @brief Returns true if this event represents a programmatic change.
   *
   * Programmatic changes are not caused by a concrete input event. This can be
   * true together with IsCancellation().
   *
   * @return true if this event represents a programmatic change
   */
  bool IsProgrammatic() const;

  /**
   * @brief Returns true if this event represents a cancellation.
   *
   * Cancellation changes include lifecycle or system resets without a concrete
   * input event, and concrete input events whose sequence was interrupted. This
   * can be true together with IsProgrammatic().
   *
   * @return true if this event represents a cancellation
   */
  bool IsCancellation() const;

  /**
   * @brief Returns a copy of this event marked as a cancellation.
   *
   * The input event type and payload are preserved.
   *
   * @return A copy of this event with the cancellation flag set
   */
  InputEvent WithCancellation() const;

  /**
   * @brief
   */
  const TouchEvent& GetTouchEvent() const;

  /**
   * @brief
   */
  const KeyEvent& GetKeyEvent() const;

  /**
   * @brief
   */
  const TapGesture& GetTapGesture() const;

  /**
   * @brief
   */
  const LongPressGesture& GetLongPressGesture() const;

  /**
   * @brief
   */
  const WheelEvent& GetWheelEvent() const;

  /**
   * @brief
   */
  const HoverEvent& GetHoverEvent() const;

public: // Not intended for Application developers
  /**
   * @brief This constructor is used by InputEvent factory methods.
   *
   * @param[in] impl The implementation of the input event
   */
  explicit InputEvent(Internal::InputEventImpl* impl);
};

} // namespace Ui
} // namespace Dali
