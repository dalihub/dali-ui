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
 */

// CLASS HEADER
#include <dali-ui-foundation/internal/focus-manager/focus-navigation-context-impl.h>

// EXTERNAL INCLUDES
#include <utility>

namespace Dali
{
namespace Ui
{
namespace Internal
{

FocusNavigationContextImpl::FocusNavigationContextImpl(FocusDirection direction,
                                                       FocusDevice    device,
                                                       Dali::String   deviceName,
                                                       InputEvent     inputEvent,
                                                       Window         window,
                                                       View           focusGroup)
: mDirection(direction),
  mDevice(device),
  mDeviceName(std::move(deviceName)),
  mInputEvent(std::move(inputEvent)),
  mWindow(std::move(window)),
  mFocusGroup(std::move(focusGroup))
{
}

FocusDirection FocusNavigationContextImpl::GetDirection() const
{
  return mDirection;
}

FocusDevice FocusNavigationContextImpl::GetDevice() const
{
  return mDevice;
}

const Dali::String& FocusNavigationContextImpl::GetDeviceName() const
{
  return mDeviceName;
}

InputEvent FocusNavigationContextImpl::GetInputEvent() const
{
  return mInputEvent;
}

Window FocusNavigationContextImpl::GetWindow() const
{
  return mWindow;
}

View FocusNavigationContextImpl::GetFocusGroup() const
{
  return mFocusGroup;
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
