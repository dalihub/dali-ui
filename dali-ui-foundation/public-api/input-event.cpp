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
#include <dali-ui-foundation/public-api/input-event.h>
#include <dali-ui-foundation/integration-api/input-event-impl.h>

namespace Dali
{

namespace Ui
{

InputEvent::InputEvent(Integration::InputEventImpl* impl)
  : BaseHandle(impl)
{
}

InputEventType InputEvent::GetInputEventType() const
{
  return GetImpl(*this).GetInputEventType();
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

} // namespace Ui
} // namespace Dali
