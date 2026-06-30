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
#include <dali-ui-foundation/internal/interactive-trait/pending-press-manager.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/scroll-state-observer.h>
#include <dali-ui-foundation/internal/views/view/interactive-trait-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{

uint32_t GetPressDelayTime()
{
  return UiConfig::GetCurrent().GetAmbiguousPressDelay();
}

uint32_t GetPressDurationTime()
{
  return UiConfig::GetCurrent().GetAmbiguousPressDuration();
}

} // unnamed namespace

PendingPressManager& PendingPressManager::Get()
{
  static PendingPressManager manager;
  return manager;
}

bool PendingPressManager::HasPendingPress() const
{
  return mPendingPressTrait != nullptr;
}

void PendingPressManager::AddPendingPress(Internal::InteractiveTraitImpl& trait, InputEvent pressEvent, uint32_t deviceId)
{
  CancelPendingPress();
  CompletePendingRelease();
  mPendingPressTrait    = &trait;
  mPendingPressEvent    = pressEvent;
  mPendingPressDeviceId = deviceId;
  mPendingPressApplied  = false;
  StartPendingPressTimer();
}

bool PendingPressManager::FinishPendingPress(Internal::InteractiveTraitImpl& trait, uint32_t deviceId, InputEvent releaseEvent)
{
  if(mPendingPressTrait != &trait || mPendingPressDeviceId != deviceId)
  {
    return false;
  }

  bool wasPressed = mPendingPressApplied;
  FlushPendingPress();
  ClearPendingPress();
  if(wasPressed)
  {
    trait.SetPressedInternal(false, releaseEvent);
  }
  else
  {
    SchedulePendingRelease(trait, releaseEvent);
  }
  return true;
}

bool PendingPressManager::CompletePendingPressWithDuration(Internal::InteractiveTraitImpl& trait, InputEvent releaseEvent)
{
  if(mPendingPressTrait != &trait)
  {
    return false;
  }

  bool wasPressed = mPendingPressApplied;
  FlushPendingPress();
  ClearPendingPress();
  if(!wasPressed)
  {
    SchedulePendingRelease(trait, releaseEvent);
  }
  return true;
}

bool PendingPressManager::HasPendingRelease(Internal::InteractiveTraitImpl& trait) const
{
  return mPendingReleaseTrait == &trait;
}

void PendingPressManager::ClearPendingPress(Internal::InteractiveTraitImpl& trait)
{
  if(mPendingPressTrait == &trait)
  {
    ClearPendingPress();
  }
}

void PendingPressManager::ClearAll(Internal::InteractiveTraitImpl& trait)
{
  if(mPendingPressTrait == &trait)
  {
    ClearPendingPress();
  }
  if(mPendingReleaseTrait == &trait)
  {
    ClearPendingRelease();
  }
}

void PendingPressManager::Cancel(Internal::InteractiveTraitImpl& trait)
{
  if(mPendingPressTrait == &trait)
  {
    CancelPendingPress();
  }
  if(mPendingReleaseTrait == &trait)
  {
    CancelPendingRelease();
  }
}

void PendingPressManager::FlushPendingPress()
{
  if(mPendingPressTrait && !mPendingPressApplied)
  {
    StopPendingPressTimer();
    mPendingPressTrait->SetPressedInternal(true, mPendingPressEvent);
    mPendingPressApplied = true;
  }
}

void PendingPressManager::FlushPendingPress(Internal::InteractiveTraitImpl& trait)
{
  if(mPendingPressTrait == &trait)
  {
    FlushPendingPress();
  }
}

PendingPressManager::PendingPressManager()
{
  ScrollStateObserver::Get().DragStartedSignal().Connect(this, &PendingPressManager::OnDragStarted);
}

void PendingPressManager::StartPendingPressTimer()
{
  if(!mPendingPressTimer)
  {
    mPendingPressTimer = Timer::New(GetPressDelayTime());
    mPendingPressTimer.TickSignal().Connect(this, &PendingPressManager::OnPendingPressTimerTick);
  }
  else
  {
    mPendingPressTimer.SetInterval(GetPressDelayTime());
  }
  mPendingPressTimer.Start();
}

void PendingPressManager::StopPendingPressTimer()
{
  if(mPendingPressTimer)
  {
    mPendingPressTimer.Stop();
  }
}

bool PendingPressManager::OnPendingPressTimerTick()
{
  FlushPendingPress();
  return false;
}

void PendingPressManager::StartPendingReleaseTimer()
{
  if(GetPressDurationTime() == 0u)
  {
    CompletePendingRelease();
    return;
  }

  if(!mPendingReleaseTimer)
  {
    mPendingReleaseTimer = Timer::New(GetPressDurationTime());
    mPendingReleaseTimer.TickSignal().Connect(this, &PendingPressManager::OnPendingReleaseTimerTick);
  }
  else
  {
    mPendingReleaseTimer.SetInterval(GetPressDurationTime());
  }
  mPendingReleaseTimer.Start();
}

void PendingPressManager::StopPendingReleaseTimer()
{
  if(mPendingReleaseTimer)
  {
    mPendingReleaseTimer.Stop();
  }
}

bool PendingPressManager::OnPendingReleaseTimerTick()
{
  CompletePendingRelease();
  return false;
}

void PendingPressManager::OnDragStarted()
{
  CancelPendingPress();
}

void PendingPressManager::CancelPendingPress()
{
  if(mPendingPressTrait && mPendingPressApplied)
  {
    mPendingPressTrait->SetPressedInternal(false, InputEvent::Programmatic().WithCancellation());
  }
  ClearPendingPress();
}

void PendingPressManager::ClearPendingPress()
{
  StopPendingPressTimer();
  mPendingPressTrait    = nullptr;
  mPendingPressEvent    = InputEvent();
  mPendingPressDeviceId = 0u;
  mPendingPressApplied  = false;
}

void PendingPressManager::SchedulePendingRelease(Internal::InteractiveTraitImpl& trait, InputEvent releaseEvent)
{
  CompletePendingRelease();
  mPendingReleaseTrait = &trait;
  mPendingReleaseEvent = releaseEvent;
  StartPendingReleaseTimer();
}

void PendingPressManager::CompletePendingRelease()
{
  if(mPendingReleaseTrait)
  {
    mPendingReleaseTrait->SetPressedInternal(false, mPendingReleaseEvent);
    ClearPendingRelease();
  }
}

void PendingPressManager::ClearPendingRelease()
{
  StopPendingReleaseTimer();
  mPendingReleaseTrait = nullptr;
  mPendingReleaseEvent = InputEvent();
}

void PendingPressManager::CancelPendingRelease()
{
  if(mPendingReleaseTrait)
  {
    mPendingReleaseTrait->SetPressedInternal(false, InputEvent::Programmatic().WithCancellation());
    ClearPendingRelease();
  }
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
