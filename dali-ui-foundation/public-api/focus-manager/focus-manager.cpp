/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/focus-manager/focus-manager-impl.h>

namespace Dali
{
namespace Ui
{
FocusManager::FocusManager()
{
}

FocusManager::~FocusManager()
{
}

FocusManager FocusManager::Get()
{
  return Internal::FocusManager::Get();
}

FocusManager::FocusManager(Internal::FocusManager* impl)
: BaseHandle(impl)
{
}

bool FocusManager::SetCurrentFocusView(View view)
{
  return GetImpl(*this).SetCurrentFocusView(view);
}

View FocusManager::GetCurrentFocusView()
{
  return GetImpl(*this).GetCurrentFocusView();
}

bool FocusManager::MoveFocus(Ui::FocusDirection direction)
{
  return GetImpl(*this).MoveFocus(direction);
}

void FocusManager::ClearFocus()
{
  GetImpl(*this).ClearFocus();
}

void FocusManager::SetAsFocusGroup(View view, bool isFocusGroup)
{
  // deprecated method.
  GetImpl(*this).SetAsFocusGroup(view, isFocusGroup);
}

bool FocusManager::IsFocusGroup(View view) const
{
  // deprecated method.
  return GetImpl(*this).IsFocusGroup(view);
}

View FocusManager::GetFocusGroup(View view)
{
  return GetImpl(*this).GetFocusGroup(view);
}

void FocusManager::SetFocusGroupLoop(bool enabled)
{
  GetImpl(*this).SetFocusGroupLoop(enabled);
}

bool FocusManager::GetFocusGroupLoop() const
{
  return GetImpl(*this).GetFocusGroupLoop();
}

void FocusManager::SetFocusIndicatorActor(View indicator)
{
  GetImpl(*this).SetFocusIndicatorActor(indicator);
}

View FocusManager::GetFocusIndicatorView()
{
  return GetImpl(*this).GetFocusIndicatorView();
}

void FocusManager::MoveFocusBackward()
{
  return GetImpl(*this).MoveFocusBackward();
}

FocusManager::PreFocusChangeSignalType& FocusManager::PreFocusChangeSignal()
{
  return GetImpl(*this).PreFocusChangeSignal();
}

FocusManager::FocusChangedSignalType& FocusManager::FocusChangedSignal()
{
  return GetImpl(*this).FocusChangedSignal();
}

FocusManager::FocusGroupChangedSignalType& FocusManager::FocusGroupChangedSignal()
{
  return GetImpl(*this).FocusGroupChangedSignal();
}

FocusDevice FocusManager::GetLastFocusChangeDevice() const
{
  const auto& internalCtx = GetImpl(*this).FocusChangedContext();
  return internalCtx.device;
}

const Dali::String& FocusManager::GetLastFocusChangeDeviceName() const
{
  const auto& internalCtx = GetImpl(*this).FocusChangedContext();
  return internalCtx.deviceName;
}

void FocusManager::SetClearFocusOnWindowFocusLost(bool enabled)
{
  GetImpl(*this).SetClearFocusOnWindowFocusLost(enabled);
}

bool FocusManager::GetClearFocusOnWindowFocusLost() const
{
  return GetImpl(*this).GetClearFocusOnWindowFocusLost();
}

} // namespace Ui

} // namespace Dali
