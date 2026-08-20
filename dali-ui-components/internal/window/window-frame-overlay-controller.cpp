/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include "window-frame-overlay-controller.h"

namespace Dali::Ui::Internal
{

OverlayStateUpdate WindowFrameOverlayController::SetAttached(bool attached)
{
  const bool previousActive        = IsOverlayActive();
  const bool previousWindowVisible = mWindowVisible;
  mAttached                        = attached;
  return UpdateActivation(previousActive, previousWindowVisible);
}

OverlayStateUpdate WindowFrameOverlayController::SetOverlayEnabled(bool enabled)
{
  const bool previousActive        = IsOverlayActive();
  const bool previousWindowVisible = mWindowVisible;
  mOverlayEnabled                  = enabled;
  return UpdateActivation(previousActive, previousWindowVisible);
}

OverlayStateUpdate WindowFrameOverlayController::SetEffectiveMaximized(bool maximized)
{
  const bool previousActive        = IsOverlayActive();
  const bool previousWindowVisible = mWindowVisible;
  mEffectiveMaximized              = maximized;
  return UpdateActivation(previousActive, previousWindowVisible);
}

OverlayStateUpdate WindowFrameOverlayController::SetWindowVisible(bool visible)
{
  const bool previousActive        = IsOverlayActive();
  const bool previousWindowVisible = mWindowVisible;
  mWindowVisible                   = visible;
  return UpdateActivation(previousActive, previousWindowVisible);
}

OverlayStateUpdate WindowFrameOverlayController::SetRequestedVisible(bool visible)
{
  mRequestedVisible = visible;
  mAutoHidden       = false;
  if(!visible)
  {
    return OverlayStateUpdate{false, true};
  }
  return OverlayStateUpdate{IsOverlayActive() && mWindowVisible, false};
}

OverlayStateUpdate WindowFrameOverlayController::RevealTemporarily()
{
  if(!IsOverlayActive() || !mWindowVisible || !mRequestedVisible)
  {
    return {};
  }

  mAutoHidden = false;
  return OverlayStateUpdate{true, false};
}

OverlayStateUpdate WindowFrameOverlayController::OnAutoHideTimeout()
{
  if(!ShouldTimerRun())
  {
    return {};
  }

  mAutoHidden = true;
  return OverlayStateUpdate{false, true};
}

bool WindowFrameOverlayController::SetAutoHideDelay(uint32_t milliseconds)
{
  if(mAutoHideDelayMilliseconds == milliseconds)
  {
    return false;
  }
  mAutoHideDelayMilliseconds = milliseconds;
  if(milliseconds == 0u)
  {
    // Auto-hide is off, so any decoration hidden by it becomes visible again.
    mAutoHidden = false;
  }
  return true;
}

uint32_t WindowFrameOverlayController::GetAutoHideDelay() const
{
  return mAutoHideDelayMilliseconds;
}

bool WindowFrameOverlayController::IsOverlayEnabled() const
{
  return mOverlayEnabled;
}

bool WindowFrameOverlayController::IsOverlayActive() const
{
  return mAttached && mOverlayEnabled && mEffectiveMaximized;
}

bool WindowFrameOverlayController::IsRequestedVisible() const
{
  return mRequestedVisible;
}

bool WindowFrameOverlayController::IsAutoHidden() const
{
  return mAutoHidden;
}

bool WindowFrameOverlayController::IsEffectivelyVisible() const
{
  return mRequestedVisible && !mAutoHidden;
}

bool WindowFrameOverlayController::ShouldTimerRun() const
{
  return mAutoHideDelayMilliseconds != 0u && IsOverlayActive() && mWindowVisible && mRequestedVisible && !mAutoHidden;
}

OverlayStateUpdate WindowFrameOverlayController::UpdateActivation(bool previousActive, bool previousWindowVisible)
{
  const bool active = IsOverlayActive();
  if(!active)
  {
    mAutoHidden = false;
    return OverlayStateUpdate{false, previousActive};
  }

  const bool becameActive  = !previousActive;
  const bool becameVisible = !previousWindowVisible && mWindowVisible;
  if(becameActive || becameVisible)
  {
    mAutoHidden = false;
  }

  if(!mWindowVisible || !mRequestedVisible)
  {
    return OverlayStateUpdate{false, previousWindowVisible};
  }
  return OverlayStateUpdate{becameActive || becameVisible, false};
}

} // namespace Dali::Ui::Internal
