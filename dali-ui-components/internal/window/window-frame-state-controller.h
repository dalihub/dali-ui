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

#include "initial-window-state-resolver.h"

#include <dali-ui-components/public-api/window/window-frame.h>

#include <optional>

namespace Dali::Ui::Internal
{

// Tracks the window state the window system reports, and resolves the launch
// policy once at start-up.
//
// The window system is trusted to complete a request it received, so there is
// no in-flight state to track: the client never holds a state the window
// system has not reported.
class WindowFrameStateController
{
public:
  WindowFrameStateController(WindowFrameInitialStatePolicy policy = WindowFrameInitialStatePolicy::AUTO, float fullSizeTolerance = 1.0f);

  /**
   * @brief Resolves the launch policy once the window has a surface.
   * @return The new state when it changed
   */
  std::optional<WindowFrame::WindowState> ResolveInitialState(const Dali::Window::WindowSize&         currentSurfaceSize,
                                                              WindowOperationAdapter&                 windowOperations,
                                                              std::optional<Dali::Window::WindowSize> restoreSurfaceSize);

  /**
   * @brief Reads the current native state, continuing launch resolution if needed.
   * @return The new state when it changed
   */
  std::optional<WindowFrame::WindowState> SynchronizeNativeState(const Dali::Window::WindowSize&         currentSurfaceSize,
                                                                 WindowOperationAdapter&                 windowOperations,
                                                                 std::optional<Dali::Window::WindowSize> restoreSurfaceSize);

  LaunchStateResolution GetLaunchResolution() const;

  /**
   * @brief Returns the size the first restore has to ask for, and forgets it.
   * @see InitialWindowStateResolver::TakeInitialRestoreSize()
   */
  std::optional<Dali::Window::WindowSize> TakeInitialRestoreSize();

  /**
   * @brief Returns the maximize state a toggle should request.
   * @return The requested value, or nullopt when the state cannot be toggled
   */
  std::optional<bool> GetToggleTarget() const;

  WindowFrame::WindowState GetState() const;
  bool                     IsMaximized() const;
  bool                     IsMinimized() const;

private:
  std::optional<WindowFrame::WindowState> Observe(const NativeWindowState& nativeState);

  InitialWindowStateResolver mInitialStateResolver;
  bool                       mTrackingLaunchState{true};
  bool                       mNativeMaximized{false};
  bool                       mNativeMinimized{false};
};

} // namespace Dali::Ui::Internal
