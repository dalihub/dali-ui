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
#include "keyinput-focus-manager.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/common/singleton-service.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/focus-manager/keyinput-focus-manager-impl.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
KeyInputFocusManager::KeyInputFocusManager()
{
}

KeyInputFocusManager::~KeyInputFocusManager()
{
}

KeyInputFocusManager KeyInputFocusManager::Get()
{
  KeyInputFocusManager manager;

  // Check whether the focus manager is already created
  SingletonService singletonService(SingletonService::Get());
  if(singletonService)
  {
    Dali::BaseHandle handle = singletonService.GetSingleton(typeid(KeyInputFocusManager));
    if(handle)
    {
      // If so, downcast the handle of singleton to focus manager
      manager = KeyInputFocusManager(dynamic_cast<KeyInputFocusManagerImpl*>(handle.GetObjectPtr()));
    }

    if(!manager)
    {
      // If not, create the focus manager and register it as a singleton
      manager = KeyInputFocusManager(new KeyInputFocusManagerImpl());
      singletonService.Register(typeid(manager), manager);
    }
  }

  return manager;
}

KeyInputFocusManager::KeyInputFocusManager(KeyInputFocusManagerImpl* impl)
: BaseHandle(impl)
{
}

void KeyInputFocusManager::SetFocus(View view)
{
  GetImpl(*this).SetFocus(view);
}

View KeyInputFocusManager::GetCurrentFocusView() const
{
  return GetImpl(*this).GetCurrentFocusView();
}

void KeyInputFocusManager::RemoveFocus(View view)
{
  GetImpl(*this).RemoveFocus(view);
}

KeyInputFocusManager::KeyInputFocusChangedSignalType& KeyInputFocusManager::KeyInputFocusChangedSignal()
{
  return GetImpl(*this).KeyInputFocusChangedSignal();
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
