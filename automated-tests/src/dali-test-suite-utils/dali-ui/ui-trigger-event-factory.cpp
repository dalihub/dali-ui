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

#include <dali/integration-api/adaptor-framework/trigger-event-factory.h>
#include <dali/integration-api/adaptor-framework/trigger-event-interface.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/signals/callback.h>

namespace DALI_NAMESPACE
{
namespace
{
// Unit tests have no platform event loop. Match Toolkit's immediate trigger.
class TestTriggerEvent : public TriggerEventInterface
{
public:
  explicit TestTriggerEvent(CallbackBase* callback)
  : mCallback(callback)
  {
  }

  void Trigger() override
  {
    CallbackBase::Execute(*mCallback);
  }

private:
  UniquePtr<CallbackBase> mCallback;
};
} //namespace
TriggerEventFactory::TriggerEventPtr TriggerEventFactory::CreateTriggerEvent(CallbackBase* callback)
{
  return TriggerEventPtr(new TestTriggerEvent(callback));
}

void TriggerEventFactory::DestroyTriggerEvent(TriggerEventInterface* trigger)
{
  delete trigger;
}
} //namespace DALI_NAMESPACE
