/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/focus-manager/focus-manager-devel.h>
#include <dali-ui-foundation/internal/focus-manager/focus-manager-impl.h>

namespace Dali
{
namespace Ui
{
namespace DevelFocusManager
{
void EnableFocusIndicator(FocusManager focusManager, bool enable)
{
  GetImpl(focusManager).EnableFocusIndicator(enable);
}

bool IsFocusIndicatorEnabled(FocusManager focusManager)
{
  return GetImpl(focusManager).IsFocusIndicatorEnabled();
}

void EnableDefaultAlgorithm(FocusManager focusManager, bool enable)
{
  GetImpl(focusManager).EnableDefaultAlgorithm(enable);
}

bool IsDefaultAlgorithmEnabled(FocusManager focusManager)
{
  return GetImpl(focusManager).IsDefaultAlgorithmEnabled();
}

bool MoveFocus(FocusManager focusManager, Ui::FocusDirection direction, const Dali::String& deviceName)
{
  return GetImpl(focusManager).MoveFocus(direction, deviceName);
}

} // namespace DevelFocusManager

} // namespace Ui

} // namespace Dali
