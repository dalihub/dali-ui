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

#include <algorithm>

namespace Dali::Ui::Internal
{
namespace
{
bool IsValid(const Dali::Window::WindowSize& size)
{
  return size.GetWidth() > 0 && size.GetHeight() > 0;
}

bool IsValid(const std::optional<Dali::PositionSize>& bounds)
{
  return bounds && bounds->width > 0 && bounds->height > 0;
}

bool CoversTarget(const Dali::Window::WindowSize& current, const Dali::PositionSize& target, float tolerance)
{
  return static_cast<float>(current.GetWidth()) >= static_cast<float>(target.width) - tolerance &&
         static_cast<float>(current.GetHeight()) >= static_cast<float>(target.height) - tolerance;
}
} // unnamed namespace

InitialWindowStateResolver::InitialWindowStateResolver(WindowFrameInitialStatePolicy policy, float fullSizeTolerance)
: mPolicy(policy),
  mFullSizeTolerance(std::max(0.0f, fullSizeTolerance))
{
}

LaunchStateResolution InitialWindowStateResolver::Resolve(const Dali::Window::WindowSize&         currentSurfaceSize,
                                                          WindowOperationAdapter&                 windowOperations,
                                                          std::optional<Dali::Window::WindowSize> restoreSurfaceSize)
{
  if(mResolution != LaunchStateResolution::UNRESOLVED && mResolution != LaunchStateResolution::WAITING_FOR_TARGET_BOUNDS)
  {
    return mResolution;
  }

  if(mPolicy == WindowFrameInitialStatePolicy::USE_CURRENT)
  {
    mResolution = LaunchStateResolution::RESOLVED_NORMAL;
    return mResolution;
  }

  const NativeWindowState nativeState = windowOperations.GetWindowState();
  if(nativeState.maximized)
  {
    mResolution = LaunchStateResolution::RESOLVED_MAXIMIZED;
    return mResolution;
  }

  const WindowFrameFeatureConfig& featureConfig = windowOperations.GetFeatureConfig();
  if(mPolicy == WindowFrameInitialStatePolicy::REQUEST_MAXIMIZE)
  {
    if(!featureConfig.maximizeRestoreEnabled)
    {
      mResolution = LaunchStateResolution::RESOLVED_NORMAL;
      return mResolution;
    }
    return RequestInitialMaximize(windowOperations, std::nullopt);
  }

  const std::optional<Dali::PositionSize> targetBounds = windowOperations.GetTargetMaximizedBounds();
  if(!IsValid(currentSurfaceSize) || !IsValid(targetBounds))
  {
    mResolution = LaunchStateResolution::WAITING_FOR_TARGET_BOUNDS;
    return mResolution;
  }

  if(!CoversTarget(currentSurfaceSize, *targetBounds, mFullSizeTolerance))
  {
    mResolution = LaunchStateResolution::RESOLVED_NORMAL;
    return mResolution;
  }

  if(!featureConfig.maximizeRestoreEnabled)
  {
    mResolution = LaunchStateResolution::RESOLVED_FULL_SIZE_NOT_DISPATCHED;
    return mResolution;
  }

  return RequestInitialMaximize(windowOperations, restoreSurfaceSize);
}

WindowFrameInitialStatePolicy InitialWindowStateResolver::GetPolicy() const
{
  return mPolicy;
}

LaunchStateResolution InitialWindowStateResolver::GetResolution() const
{
  return mResolution;
}

bool InitialWindowStateResolver::IsResolved() const
{
  return mResolution == LaunchStateResolution::RESOLVED_NORMAL ||
         mResolution == LaunchStateResolution::RESOLVED_MAXIMIZED ||
         mResolution == LaunchStateResolution::RESOLVED_FULL_SIZE_NOT_DISPATCHED;
}

LaunchStateResolution InitialWindowStateResolver::RequestInitialMaximize(WindowOperationAdapter&                 windowOperations,
                                                                         std::optional<Dali::Window::WindowSize> restoreSurfaceSize)
{
  // The restore size is deliberately not sent with this request. The window
  // system takes it and then loses it, because this same call is what moves the
  // window into the maximized state and that transition resets the size it
  // remembers to the window's current geometry - which is the full screen here.
  // It is held for the first restore instead, where the transition runs the
  // other way and leaves the size alone.
  //
  // A successfully dispatched request settles this one-shot launch policy here.
  // The actual native state remains independently observed from window events.
  if(windowOperations.RequestMaximize(true))
  {
    mResolution                = LaunchStateResolution::RESOLVED_MAXIMIZED;
    mInitialRestoreSurfaceSize = restoreSurfaceSize;
  }
  else
  {
    mResolution = LaunchStateResolution::RESOLVED_NORMAL;
  }
  return mResolution;
}

std::optional<Dali::Window::WindowSize> InitialWindowStateResolver::TakeInitialRestoreSize()
{
  std::optional<Dali::Window::WindowSize> size;
  mInitialRestoreSurfaceSize.swap(size);
  return size;
}

const char* LaunchStateResolutionName(LaunchStateResolution resolution)
{
  switch(resolution)
  {
    case LaunchStateResolution::UNRESOLVED:
      return "unresolved";
    case LaunchStateResolution::WAITING_FOR_TARGET_BOUNDS:
      return "waiting-for-target-bounds";
    case LaunchStateResolution::RESOLVED_NORMAL:
      return "resolved-normal";
    case LaunchStateResolution::RESOLVED_MAXIMIZED:
      return "resolved-maximized";
    case LaunchStateResolution::RESOLVED_FULL_SIZE_NOT_DISPATCHED:
      return "resolved-full-size-not-dispatched";
  }
  return "unknown";
}

} // namespace Dali::Ui::Internal
