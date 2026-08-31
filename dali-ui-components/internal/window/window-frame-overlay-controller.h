#pragma once

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

#include <cstdint>

namespace Dali::Ui::Internal
{

struct OverlayStateUpdate
{
  bool restartTimer{false};
  bool stopTimer{false};
};

class WindowFrameOverlayController
{
public:
  OverlayStateUpdate SetAttached(bool attached);
  OverlayStateUpdate SetOverlayEnabled(bool enabled);
  OverlayStateUpdate SetEffectiveMaximized(bool maximized);
  OverlayStateUpdate SetWindowVisible(bool visible);
  OverlayStateUpdate SetRequestedVisible(bool visible);
  OverlayStateUpdate RevealTemporarily();
  OverlayStateUpdate OnAutoHideTimeout();

  /**
   * @brief Sets the auto-hide delay.
   * @param[in] milliseconds Delay, or 0 to keep decoration visible indefinitely
   * @return true when the delay changed
   */
  bool     SetAutoHideDelay(uint32_t milliseconds);
  uint32_t GetAutoHideDelay() const;

  bool IsOverlayEnabled() const;
  bool IsOverlayActive() const;
  bool IsRequestedVisible() const;
  bool IsAutoHidden() const;
  bool IsEffectivelyVisible() const;
  bool ShouldTimerRun() const;

private:
  OverlayStateUpdate UpdateActivation(bool previousActive, bool previousWindowVisible);

private:
  bool     mAttached{false};
  bool     mOverlayEnabled{false};
  bool     mEffectiveMaximized{false};
  bool     mWindowVisible{true};
  bool     mRequestedVisible{true};
  bool     mAutoHidden{false};
  uint32_t mAutoHideDelayMilliseconds{3000u};
};

} // namespace Dali::Ui::Internal
