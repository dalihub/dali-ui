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

#include "window-operation-adapter.h"

#include <dali-ui-components/public-api/window/window-frame-options.h>

#include <optional>

namespace Dali::Ui::Internal
{

enum class LaunchStateResolution
{
  UNRESOLVED                        = 0,
  WAITING_FOR_TARGET_BOUNDS         = 1,
  RESOLVED_NORMAL                   = 2,
  RESOLVED_MAXIMIZED                = 3,
  RESOLVED_FULL_SIZE_NOT_DISPATCHED = 4
};

class InitialWindowStateResolver
{
public:
  explicit InitialWindowStateResolver(WindowFrameInitialStatePolicy policy = WindowFrameInitialStatePolicy::AUTO, float fullSizeTolerance = 1.0f);

  LaunchStateResolution Resolve(const Dali::Window::WindowSize&         currentSurfaceSize,
                                WindowOperationAdapter&                 windowOperations,
                                std::optional<Dali::Window::WindowSize> restoreSurfaceSize = std::nullopt);

  WindowFrameInitialStatePolicy GetPolicy() const;
  LaunchStateResolution         GetResolution() const;
  bool                          IsResolved() const;

  /**
   * @brief Returns the size the first restore has to ask for, and forgets it.
   *
   * Only a window that launched already covering the screen has one, because it
   * has no earlier normal geometry for the window system to come back to. A
   * window that launched at a partial size does, so the compositor owns its
   * restore geometry and nothing is held here.
   *
   * Held for the restore rather than sent with the launch maximize, which is
   * where the window system drops it - see RequestInitialMaximize().
   *
   * @return The size for the first restore, or nullopt once spent
   */
  std::optional<Dali::Window::WindowSize> TakeInitialRestoreSize();

private:
  LaunchStateResolution RequestInitialMaximize(WindowOperationAdapter&                 windowOperations,
                                               std::optional<Dali::Window::WindowSize> restoreSurfaceSize);

private:
  WindowFrameInitialStatePolicy           mPolicy;
  LaunchStateResolution                   mResolution{LaunchStateResolution::UNRESOLVED};
  float                                   mFullSizeTolerance{1.0f};
  std::optional<Dali::Window::WindowSize> mInitialRestoreSurfaceSize;
};

const char* LaunchStateResolutionName(LaunchStateResolution resolution);

} // namespace Dali::Ui::Internal
