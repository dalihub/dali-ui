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
#include "ui-event-thread-callback.h"

// EXTERNAL INCLUDES
#include <math.h>
#include <algorithm>
#include <chrono>
#include <climits>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <memory>
#include <mutex>
#include <vector>

namespace
{
// Note, this is not thread safe - however, should not be using
// triggers from multiple threads - they should all be created on
// event thread.
std::vector<Dali::EventThreadCallback*> gEventThreadCallbacks;
} // namespace

namespace Dali
{
struct EventThreadCallback::Impl
{
  std::unique_ptr<CallbackBase> callback;
  std::mutex                    mutex;
  std::condition_variable       condition;
  uint32_t                      pendingTriggers{0u};
};

EventThreadCallback::EventThreadCallback(CallbackBase* callback)
: mImpl(new Impl())
{
  mImpl->callback.reset(callback);

  gEventThreadCallbacks.push_back(this);
}

EventThreadCallback::~EventThreadCallback()
{
  std::vector<EventThreadCallback*>::iterator iter =
    std::find(gEventThreadCallbacks.begin(), gEventThreadCallbacks.end(), this);
  if(iter != gEventThreadCallbacks.end())
  {
    gEventThreadCallbacks.erase(iter);
  }
  delete mImpl;
}

void EventThreadCallback::Trigger()
{
  {
    std::lock_guard<std::mutex> lock(mImpl->mutex);
    ++mImpl->pendingTriggers;
  }
  mImpl->condition.notify_one();
}

// returns true if timed out rather than triggered
bool EventThreadCallback::WaitingForTrigger()
{
  std::unique_lock<std::mutex> lock(mImpl->mutex);
  const bool triggered = mImpl->condition.wait_for(lock, std::chrono::microseconds(1), [this]
  {
    return mImpl->pendingTriggers > 0u;
  });
  if(triggered)
  {
    --mImpl->pendingTriggers;
  }
  return !triggered;
}

CallbackBase* EventThreadCallback::GetCallback()
{
  return mImpl->callback.get();
}

} // namespace Dali

namespace Test
{
bool WaitForEventThreadTrigger(int triggerCount, int timeoutInSeconds, int executeCallbacks)
{
  struct timespec startTime;
  struct timespec now;
  timespec_get(&startTime, TIME_UTC);
  now.tv_sec  = startTime.tv_sec;
  now.tv_nsec = startTime.tv_nsec;

  // Round robin poll of each semaphore:
  while(triggerCount > 0)
  {
    if(gEventThreadCallbacks.size() > 0)
    {
      for(std::vector<Dali::EventThreadCallback*>::iterator iter = gEventThreadCallbacks.begin();
          iter != gEventThreadCallbacks.end();
          ++iter)
      {
        Dali::EventThreadCallback* eventTrigger = (*iter);
        Dali::CallbackBase*        callback     = eventTrigger->GetCallback();
        bool                       timedout     = eventTrigger->WaitingForTrigger();
        if(!timedout)
        {
          if(executeCallbacks)
          {
            // Semaphore was unlocked - execute the trigger
            Dali::CallbackBase::Execute(*callback);
          }
          triggerCount--;
        }
        if(triggerCount <= 0)
        {
          break;
        }
      }
    }
    timespec_get(&now, TIME_UTC);
    if(now.tv_sec - startTime.tv_sec > timeoutInSeconds)
    {
      // Ensure we break out of the loop if elapsed time has passed
      break;
    }
  }

  timespec_get(&now, TIME_UTC);
  if(now.tv_sec > startTime.tv_sec + 1)
  {
    fprintf(stderr,
            "WaitForEventThreadTrigger took %lld seconds\n",
            static_cast<long long>(now.tv_sec - startTime.tv_sec));
  }
  return triggerCount == 0;
}

} // namespace Test
