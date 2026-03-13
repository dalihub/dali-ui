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
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <variant>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/input-event.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

class InputEventImpl;
using InputEventImplPtr = IntrusivePtr<InputEventImpl>;

/**
 * @brief Internal implementation of InputEvent.
 */
class DALI_UI_API InputEventImpl : public BaseObject
{
public:
  /**
   * @brief Create a new InputEvent.
   *
   * @param[in] originEvent The handle of originated event
   * @return An InputEvent handle
   */
  InputEventImpl(const TouchEvent& originEvent);

  /**
   * @brief Create a new InputEvent.
   *
   * @param[in] originEvent The handle of originated event
   * @return An InputEvent handle
   */
  InputEventImpl(const KeyEvent& originEvent);

  /**
   * @brief Create a new InputEvent.
   *
   * @param[in] originEvent The handle of originated event
   * @return An InputEvent handle
   */
  InputEventImpl(const TapGesture& originEvent);

  /**
   * @brief Create a new InputEvent.
   *
   * @param[in] originEvent The handle of originated event
   * @return An InputEvent handle
   */
  InputEventImpl(const LongPressGesture& originEvent);

  /**
   * @brief Create a new InputEvent implementation.
   *
   * @param[in] originEvent The handle of originated event
   * @return An IntrusivePtr to the new InputEventImpl
   */
  static InputEventImplPtr New(const TouchEvent& originEvent);

  /**
   * @brief Create a new InputEvent implementation.
   *
   * @param[in] originEvent The handle of originated event
   * @return An IntrusivePtr to the new InputEventImpl
   */
  static InputEventImplPtr New(const KeyEvent& originEvent);

  /**
   * @brief Create a new InputEvent implementation.
   *
   * @param[in] originEvent The handle of originated event
   * @return An IntrusivePtr to the new InputEventImpl
   */
  static InputEventImplPtr New(const TapGesture& originEvent);

  /**
   * @brief Create a new InputEvent implementation.
   *
   * @param[in] originEvent The handle of originated event
   * @return An IntrusivePtr to the new InputEventImpl
   */
  static InputEventImplPtr New(const LongPressGesture& originEvent);

  /**
   * @copydoc Dali::Ui::InputEvent::GetInputEventType
   */
  InputEventType GetInputEventType() const;

  /**
   * @copydoc Dali::Ui::InputEvent::GetTouchEvent
   */
  const TouchEvent& GetTouchEvent() const;

  /**
   * @copydoc Dali::Ui::InputEvent::GetKeyEvent
   */
  const KeyEvent& GetKeyEvent() const;

  /**
   * @copydoc Dali::Ui::InputEvent::GetTapGesture
   */
  const TapGesture& GetTapGesture() const;

  /**
   * @copydoc Dali::Ui::InputEvent::GetLongPressedGesture
   */
  const LongPressGesture& GetLongPressGesture() const;

private:
  /**
   * @brief Destructor.
   */
  virtual ~InputEventImpl() override;

  InputEventImpl(const InputEventImpl& rhs)            = delete; ///< Deleted copy constructor
  InputEventImpl(InputEventImpl&& rhs)                 = delete; ///< Deleted move constructor
  InputEventImpl& operator=(const InputEventImpl& rhs) = delete; ///< Deleted copy assignment operator
  InputEventImpl& operator=(InputEventImpl&& rhs)      = delete; ///< Deleted move assignment operator

private:
  InputEventType                                                   mEventType;
  std::variant<TouchEvent, KeyEvent, TapGesture, LongPressGesture> mEvent;
};

} // namespace Integration

inline Integration::InputEventImpl& GetImpl(InputEvent& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Integration::InputEventImpl&>(handle);
}

inline const Integration::InputEventImpl& GetImpl(const InputEvent& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Integration::InputEventImpl&>(handle);
}

} // namespace Ui

} // namespace Dali
