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

#include <dali-ui-components/internal/window/window-operation-adapter.h>

namespace
{
/**
 * @brief A window operation adapter that records requests instead of dispatching them.
 *
 * Launch resolution and state observation are decided entirely by what the
 * adapter reports, so a fake adapter is enough to drive both.
 */
class FakeWindowOperationAdapter : public Dali::Ui::Internal::WindowOperationAdapter
{
public:
  const Dali::Ui::Internal::WindowFrameFeatureConfig& GetFeatureConfig() const override
  {
    return featureConfig;
  }

  std::optional<Dali::PositionSize> GetTargetMaximizedBounds() const override
  {
    return targetBounds;
  }

  Dali::Ui::Internal::NativeWindowState GetWindowState() const override
  {
    return nativeState;
  }

  bool RequestMove() override
  {
    return false;
  }

  bool RequestResize(Dali::WindowResizeDirection /*direction*/) override
  {
    return false;
  }

  bool RequestMinimize() override
  {
    return false;
  }

  bool RequestMaximize(bool maximize, std::optional<Dali::Window::WindowSize> restoreSurfaceSize) override
  {
    ++maximizeRequestCount;
    requestedMaximize    = maximize;
    requestedRestoreSize = restoreSurfaceSize;
    return maximizeRequestResult;
  }

  bool RequestClose() override
  {
    return false;
  }

  bool ApplyResizeAspectRatioHint(bool /*enabled*/) override
  {
    return false;
  }

  void RestoreResizeAspectRatioHint() override
  {
  }

  Dali::Ui::Internal::WindowFrameFeatureConfig featureConfig;
  std::optional<Dali::PositionSize>            targetBounds;
  Dali::Ui::Internal::NativeWindowState        nativeState;
  bool                                         maximizeRequestResult{true};
  int                                          maximizeRequestCount{0};
  bool                                         requestedMaximize{false};
  std::optional<Dali::Window::WindowSize>      requestedRestoreSize;
};

/**
 * @brief Returns an adapter on a 1920x1080 screen with every launch feature on.
 */
FakeWindowOperationAdapter MakeSupportedAdapter()
{
  FakeWindowOperationAdapter adapter;
  adapter.featureConfig.maximizeRestoreEnabled = true;
  adapter.targetBounds                         = Dali::PositionSize(0, 0, 1920, 1080);
  return adapter;
}
} // unnamed namespace
