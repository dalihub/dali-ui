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

#include "window-frame-state-controller.h"

namespace Dali::Ui::Internal
{

WindowFrameStateController::WindowFrameStateController(WindowFrameInitialStatePolicy policy, float fullSizeTolerance)
: mInitialStateResolver(policy, fullSizeTolerance)
{
}

std::optional<WindowFrame::WindowState> WindowFrameStateController::ResolveInitialState(const Dali::Window::WindowSize&         currentSurfaceSize,
                                                                                        WindowOperationAdapter&                 windowOperations,
                                                                                        std::optional<Dali::Window::WindowSize> restoreSurfaceSize)
{
  mInitialStateResolver.Resolve(currentSurfaceSize, windowOperations, restoreSurfaceSize);
  mTrackingLaunchState = !mInitialStateResolver.IsResolved();
  return Observe(windowOperations.GetWindowState());
}

std::optional<WindowFrame::WindowState> WindowFrameStateController::SynchronizeNativeState(const Dali::Window::WindowSize&         currentSurfaceSize,
                                                                                           WindowOperationAdapter&                 windowOperations,
                                                                                           std::optional<Dali::Window::WindowSize> restoreSurfaceSize)
{
  if(mTrackingLaunchState)
  {
    // Resolve() can dispatch a native maximize request, so read the state after it.
    mInitialStateResolver.Resolve(currentSurfaceSize, windowOperations, restoreSurfaceSize);
    mTrackingLaunchState = !mInitialStateResolver.IsResolved();
  }
  return Observe(windowOperations.GetWindowState());
}

std::optional<WindowFrame::WindowState> WindowFrameStateController::Observe(const NativeWindowState& nativeState)
{
  if(mNativeMaximized == nativeState.maximized && mNativeMinimized == nativeState.minimized)
  {
    return std::nullopt;
  }

  mNativeMaximized = nativeState.maximized;
  mNativeMinimized = nativeState.minimized;
  return GetState();
}

LaunchStateResolution WindowFrameStateController::GetLaunchResolution() const
{
  return mInitialStateResolver.GetResolution();
}

std::optional<Dali::Window::WindowSize> WindowFrameStateController::TakeInitialRestoreSize()
{
  return mInitialStateResolver.TakeInitialRestoreSize();
}

std::optional<bool> WindowFrameStateController::GetToggleTarget() const
{
  // A window that launched full size on a backend without maximize support has
  // no maximized state to toggle, and a minimized window has to come back
  // before its window mode means anything.
  if(mNativeMinimized || mInitialStateResolver.GetResolution() == LaunchStateResolution::RESOLVED_FULL_SIZE_NOT_DISPATCHED)
  {
    return std::nullopt;
  }
  return !mNativeMaximized;
}

WindowFrame::WindowState WindowFrameStateController::GetState() const
{
  if(mNativeMinimized)
  {
    return WindowFrame::WindowState::MINIMIZED;
  }
  return mNativeMaximized ? WindowFrame::WindowState::MAXIMIZED : WindowFrame::WindowState::NORMAL;
}

bool WindowFrameStateController::IsMaximized() const
{
  return GetState() == WindowFrame::WindowState::MAXIMIZED;
}

bool WindowFrameStateController::IsMinimized() const
{
  return GetState() == WindowFrame::WindowState::MINIMIZED;
}

} // namespace Dali::Ui::Internal
