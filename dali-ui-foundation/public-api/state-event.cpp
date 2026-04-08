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
#include <dali-ui-foundation/public-api/state-event.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/state-event-impl.h>

namespace Dali
{

namespace Ui
{

StateEvent::StateEvent(Internal::StateEventImpl* implementation)
: BaseHandle(implementation)
{
}

const UiState& StateEvent::GetPrev() const
{
  return GetImpl(*this).GetPrev();
}

const UiState& StateEvent::GetCurrent() const
{
  return GetImpl(*this).GetCurrent();
}

const InputEvent& StateEvent::GetCause() const
{
  return GetImpl(*this).GetCause();
}

InputEventType StateEvent::GetInputEventType() const
{
  return GetImpl(*this).GetCause().GetInputEventType();
}

const TouchEvent& StateEvent::GetTouchEvent() const
{
  return GetImpl(*this).GetCause().GetTouchEvent();
}

const KeyEvent& StateEvent::GetKeyEvent() const
{
  return GetImpl(*this).GetCause().GetKeyEvent();
}

const TapGesture& StateEvent::GetTapGesture() const
{
  return GetImpl(*this).GetCause().GetTapGesture();
}

const LongPressGesture& StateEvent::GetLongPressGesture() const
{
  return GetImpl(*this).GetCause().GetLongPressGesture();
}

const WheelEvent& StateEvent::GetWheelEvent() const
{
  return GetImpl(*this).GetCause().GetWheelEvent();
}

} // namespace Ui

} // namespace Dali
