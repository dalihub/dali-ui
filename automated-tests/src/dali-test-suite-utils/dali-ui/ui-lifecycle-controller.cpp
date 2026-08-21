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

#include "ui-lifecycle-controller.h"

#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
/********************************************************************************
 * Stub for Dali::Internal::Adaptor::LifecycleController
 ********************************************************************************/
namespace Internal
{
namespace Adaptor
{
class LifecycleController : public BaseObject
{
public: // Creation & Destruction
  LifecycleController();
  ~LifecycleController();
  static Dali::LifecycleController Get();

public: // Signals
  Dali::LifecycleController::LifecycleSignalType& PreInitSignal();
  Dali::LifecycleController::LifecycleSignalType& InitSignal();
  Dali::LifecycleController::LifecycleSignalType& TerminateSignal();

private:
  Dali::LifecycleController::LifecycleSignalType mPreInitSignal;
  Dali::LifecycleController::LifecycleSignalType mInitSignal;
  Dali::LifecycleController::LifecycleSignalType mTerminateSignal;
};

LifecycleController::LifecycleController()
{
}

LifecycleController::~LifecycleController()
{
}

Dali::LifecycleController LifecycleController::Get()
{
  // Mirrors dali-adaptor: the instance lives for the lifetime of the process and does not depend on
  // Core, so it is available before the application has been created.
  static Dali::LifecycleController lifecycleController(new Internal::Adaptor::LifecycleController());
  return lifecycleController;
}

Dali::LifecycleController::LifecycleSignalType& LifecycleController::PreInitSignal()
{
  return mPreInitSignal;
}

Dali::LifecycleController::LifecycleSignalType& LifecycleController::InitSignal()
{
  return mInitSignal;
}

Dali::LifecycleController::LifecycleSignalType& LifecycleController::TerminateSignal()
{
  return mTerminateSignal;
}

} // namespace Adaptor
} // namespace Internal

/********************************************************************************
 * Stub for Dali::LifecycleController
 ********************************************************************************/

LifecycleController::LifecycleController()
{
}
LifecycleController::~LifecycleController()
{
}

LifecycleController LifecycleController::Get()
{
  // Get the lifecycle controller handle
  LifecycleController handle = Internal::Adaptor::LifecycleController::Get();
  return handle;
}

LifecycleController::LifecycleSignalType& LifecycleController::PreInitSignal()
{
  BaseObject&                             object     = GetBaseObject();
  Internal::Adaptor::LifecycleController& controller = static_cast<Internal::Adaptor::LifecycleController&>(object);
  return controller.PreInitSignal();
}

LifecycleController::LifecycleSignalType& LifecycleController::InitSignal()
{
  BaseObject&                             object     = GetBaseObject();
  Internal::Adaptor::LifecycleController& controller = static_cast<Internal::Adaptor::LifecycleController&>(object);
  return controller.InitSignal();
}

LifecycleController::LifecycleSignalType& LifecycleController::TerminateSignal()
{
  BaseObject&                             object     = GetBaseObject();
  Internal::Adaptor::LifecycleController& controller = static_cast<Internal::Adaptor::LifecycleController&>(object);
  return controller.TerminateSignal();
}

LifecycleController::LifecycleController(Internal::Adaptor::LifecycleController* impl)
: BaseHandle(impl)
{
}

} // namespace Dali
