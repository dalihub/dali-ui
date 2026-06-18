#pragma once

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

// EXTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/input-event.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{
class InteractiveTraitImpl;
}

namespace Internal
{

class PendingPressManager : public ConnectionTracker
{
public:
  static PendingPressManager& Get();

  bool HasPendingPress() const;
  void AddPendingPress(Integration::InteractiveTraitImpl& trait, InputEvent pressEvent, uint32_t deviceId);
  bool FinishPendingPress(Integration::InteractiveTraitImpl& trait, uint32_t deviceId, InputEvent releaseEvent);
  bool CompletePendingPressWithDuration(Integration::InteractiveTraitImpl& trait, InputEvent releaseEvent);
  bool HasPendingRelease(Integration::InteractiveTraitImpl& trait) const;
  void ClearPendingPress(Integration::InteractiveTraitImpl& trait);
  void ClearAll(Integration::InteractiveTraitImpl& trait);
  void Cancel(Integration::InteractiveTraitImpl& trait);
  void FlushPendingPress();
  void FlushPendingPress(Integration::InteractiveTraitImpl& trait);

private:
  PendingPressManager();

  void StartPendingPressTimer();
  void StopPendingPressTimer();
  bool OnPendingPressTimerTick();
  void StartPendingReleaseTimer();
  void StopPendingReleaseTimer();
  bool OnPendingReleaseTimerTick();
  void OnDragStarted();
  void CancelPendingPress();
  void ClearPendingPress();
  void SchedulePendingRelease(Integration::InteractiveTraitImpl& trait, InputEvent releaseEvent);
  void CompletePendingRelease();
  void ClearPendingRelease();
  void CancelPendingRelease();

private:
  Integration::InteractiveTraitImpl* mPendingPressTrait{nullptr};
  InputEvent                         mPendingPressEvent;
  uint32_t                           mPendingPressDeviceId{0u};
  Timer                              mPendingPressTimer;
  Integration::InteractiveTraitImpl* mPendingReleaseTrait{nullptr};
  InputEvent                         mPendingReleaseEvent;
  Timer                              mPendingReleaseTimer;
  bool                               mPendingPressApplied{false};
};

} // namespace Internal
} // namespace Ui
} // namespace Dali
