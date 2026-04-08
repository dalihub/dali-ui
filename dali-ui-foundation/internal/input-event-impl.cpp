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

#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-handle.h>

// CLASS HEADER
#include <dali-ui-foundation/internal/input-event-impl.h>

namespace Dali
{

namespace Ui
{

namespace Internal
{

InputEventImpl::InputEventImpl()
: BaseObject(),
  mEventType(InputEventType::NONE),
  mEvent(std::monostate{})
{
}

InputEventImpl::InputEventImpl(const TouchEvent& originEvent)
: BaseObject(),
  mEventType(InputEventType::TOUCH_EVENT),
  mEvent(originEvent)
{
}

InputEventImpl::InputEventImpl(const KeyEvent& originEvent)
: BaseObject(),
  mEventType(InputEventType::KEY_EVENT),
  mEvent(originEvent)
{
}

InputEventImpl::InputEventImpl(const TapGesture& originEvent)
: BaseObject(),
  mEventType(InputEventType::TAP_GESTURE),
  mEvent(originEvent)
{
}

InputEventImpl::InputEventImpl(const LongPressGesture& originEvent)
: BaseObject(),
  mEventType(InputEventType::LONG_PRESS_GESTURE),
  mEvent(originEvent)
{
}

InputEventImpl::InputEventImpl(const WheelEvent& originEvent)
: BaseObject(),
  mEventType(InputEventType::WHEEL_EVENT),
  mEvent(originEvent)
{
}

InputEventImpl::~InputEventImpl()
{
}

InputEventImplPtr InputEventImpl::New()
{
  return InputEventImplPtr(new InputEventImpl());
}

InputEventImplPtr InputEventImpl::New(const TouchEvent& originEvent)
{
  return InputEventImplPtr(new InputEventImpl(originEvent));
}

InputEventImplPtr InputEventImpl::New(const KeyEvent& originEvent)
{
  return InputEventImplPtr(new InputEventImpl(originEvent));
}

InputEventImplPtr InputEventImpl::New(const TapGesture& originEvent)
{
  return InputEventImplPtr(new InputEventImpl(originEvent));
}

InputEventImplPtr InputEventImpl::New(const LongPressGesture& originEvent)
{
  return InputEventImplPtr(new InputEventImpl(originEvent));
}

InputEventImplPtr InputEventImpl::New(const WheelEvent& originEvent)
{
  return InputEventImplPtr(new InputEventImpl(originEvent));
}

InputEventType InputEventImpl::GetInputEventType() const
{
  return mEventType;
}

const TouchEvent& InputEventImpl::GetTouchEvent() const
{
  DALI_ASSERT_ALWAYS(mEventType == InputEventType::TOUCH_EVENT);
  return std::get<TouchEvent>(mEvent);
}

const KeyEvent& InputEventImpl::GetKeyEvent() const
{
  DALI_ASSERT_ALWAYS(mEventType == InputEventType::KEY_EVENT);
  return std::get<KeyEvent>(mEvent);
}

const TapGesture& InputEventImpl::GetTapGesture() const
{
  DALI_ASSERT_ALWAYS(mEventType == InputEventType::TAP_GESTURE);
  return std::get<TapGesture>(mEvent);
}

const LongPressGesture& InputEventImpl::GetLongPressGesture() const
{
  DALI_ASSERT_ALWAYS(mEventType == InputEventType::LONG_PRESS_GESTURE);
  return std::get<LongPressGesture>(mEvent);
}

const WheelEvent& InputEventImpl::GetWheelEvent() const
{
  DALI_ASSERT_ALWAYS(mEventType == InputEventType::WHEEL_EVENT);
  return std::get<WheelEvent>(mEvent);
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
