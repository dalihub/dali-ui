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
#include <dali-ui-foundation/extension-api/focus-manager.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptor-framework/scene-holder.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/focus-manager/keyinput-focus-manager.h>

namespace Dali
{
namespace Ui
{
namespace Extension
{
namespace FocusManager
{
bool SetKeyInputTarget(View view)
{
  if(!view || !view.IsConnectedToScene() || !Dali::Integration::SceneHolder::Get(view))
  {
    return false;
  }

  Internal::KeyInputFocusManager manager = Internal::KeyInputFocusManager::Get();
  if(!manager)
  {
    return false;
  }

  manager.SetFocus(view);
  return manager.GetCurrentFocusView() == view;
}

bool ClearKeyInputTarget(View view)
{
  if(!view)
  {
    return false;
  }

  Internal::KeyInputFocusManager manager = Internal::KeyInputFocusManager::Get();
  if(!manager || manager.GetCurrentFocusView() != view)
  {
    return false;
  }

  manager.RemoveFocus(view);
  return true;
}

bool IsKeyInputTarget(View view)
{
  if(!view)
  {
    return false;
  }

  Internal::KeyInputFocusManager manager = Internal::KeyInputFocusManager::Get();
  return manager && manager.GetCurrentFocusView() == view;
}

} // namespace FocusManager
} // namespace Extension
} // namespace Ui
} // namespace Dali
