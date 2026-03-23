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
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/events/long-press-gesture.h>
#include <dali/public-api/events/tap-gesture.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/input-event-type.h>

namespace Dali
{

struct TouchEvent;
struct KeyEvent;

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
   * @brief Creates a new InputEvent with NONE type, representing a programmatic (non-input) change.
   *
   * The returned event has InputEventType::NONE. Use this when emitting signals that
   * do not originate from user input (e.g. programmatic state changes).
   *
   * @return A handle to the new InputEvent
   */
  static InputEvent New();

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
   * @brief Get the type of this event.
   *
   * @return The type of the input event
   */
  InputEventType GetInputEventType() const;

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

public: // Not intended for Application developers
  /**
   * @brief This constructor is used by InputEvent::New() methods.
   *
   * @param[in] impl The implementation of the input event
   */
  explicit InputEvent(Internal::InputEventImpl* impl);
};

} // namespace Ui
} // namespace Dali
