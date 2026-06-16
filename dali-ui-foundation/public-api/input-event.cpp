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

// CLASS HEADER
#include <dali-ui-foundation/internal/input-event-impl.h>
#include <dali-ui-foundation/public-api/input-event.h>

namespace Dali
{

namespace Ui
{

const InputEvent& InputEvent::Programmatic()
{
  static const InputEvent instance(Internal::InputEventImpl::New(true, false).Get());
  return instance;
}

InputEvent InputEvent::New(const TouchEvent& originEvent)
{
  Internal::InputEventImplPtr impl = Internal::InputEventImpl::New(originEvent);

  // Pass ownership to handle
  return InputEvent(impl.Get());
}

InputEvent InputEvent::New(const KeyEvent& originEvent)
{
  Internal::InputEventImplPtr impl = Internal::InputEventImpl::New(originEvent);

  // Pass ownership to handle
  return InputEvent(impl.Get());
}

InputEvent InputEvent::New(const TapGesture& originEvent)
{
  Internal::InputEventImplPtr impl = Internal::InputEventImpl::New(originEvent);

  // Pass ownership to handle
  return InputEvent(impl.Get());
}

InputEvent InputEvent::New(const LongPressGesture& originEvent)
{
  Internal::InputEventImplPtr impl = Internal::InputEventImpl::New(originEvent);

  // Pass ownership to handle
  return InputEvent(impl.Get());
}

InputEvent InputEvent::New(const WheelEvent& originEvent)
{
  Internal::InputEventImplPtr impl = Internal::InputEventImpl::New(originEvent);

  // Pass ownership to handle
  return InputEvent(impl.Get());
}

InputEvent::InputEvent(Internal::InputEventImpl* impl)
: BaseHandle(impl)
{
}

InputEventType InputEvent::GetInputEventType() const
{
  return GetImpl(*this).GetInputEventType();
}

bool InputEvent::IsProgrammatic() const
{
  return GetImpl(*this).IsProgrammatic();
}

bool InputEvent::IsCancellation() const
{
  return GetImpl(*this).IsCancellation();
}

InputEvent InputEvent::WithCancellation() const
{
  Internal::InputEventImplPtr impl = GetImpl(*this).WithCancellation();
  return InputEvent(impl.Get());
}

const TouchEvent& InputEvent::GetTouchEvent() const
{
  return GetImpl(*this).GetTouchEvent();
}

const KeyEvent& InputEvent::GetKeyEvent() const
{
  return GetImpl(*this).GetKeyEvent();
}

const TapGesture& InputEvent::GetTapGesture() const
{
  return GetImpl(*this).GetTapGesture();
}

const LongPressGesture& InputEvent::GetLongPressGesture() const
{
  return GetImpl(*this).GetLongPressGesture();
}

const WheelEvent& InputEvent::GetWheelEvent() const
{
  return GetImpl(*this).GetWheelEvent();
}

} // namespace Ui
} // namespace Dali
