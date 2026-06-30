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
#include <dali-ui-foundation/public-api/input/input-event.h>

namespace Dali
{

namespace Ui
{

namespace Internal
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
   * @brief Create a new InputEvent with NONE type.
   */
  InputEventImpl(bool programmatic, bool cancellation);

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

  InputEventImpl(const WheelEvent& originEvent);

  /**
   * @brief Create a new InputEvent with NONE type.
   */
  static InputEventImplPtr New(bool programmatic, bool cancellation);

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

  static InputEventImplPtr New(const WheelEvent& originEvent);

  InputEventImplPtr WithCancellation() const;

  /**
   * @copydoc Dali::Ui::InputEvent::GetInputEventType
   */
  InputEventType GetInputEventType() const;

  /**
   * @copydoc Dali::Ui::InputEvent::IsProgrammatic
   */
  bool IsProgrammatic() const;

  /**
   * @copydoc Dali::Ui::InputEvent::IsCancellation
   */
  bool IsCancellation() const;

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

  const WheelEvent& GetWheelEvent() const;

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
  InputEventType                                                                               mEventType;
  bool                                                                                         mProgrammatic : 1;
  bool                                                                                         mCancellation : 1;
  std::variant<std::monostate, TouchEvent, KeyEvent, TapGesture, LongPressGesture, WheelEvent> mEvent;
};

} // namespace Internal

inline Internal::InputEventImpl& GetImpl(InputEvent& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Internal::InputEventImpl&>(handle);
}

inline const Internal::InputEventImpl& GetImpl(const InputEvent& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Internal::InputEventImpl&>(handle);
}

} // namespace Ui

} // namespace Dali
