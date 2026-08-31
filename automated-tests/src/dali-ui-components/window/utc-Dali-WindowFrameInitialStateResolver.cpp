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

#include <dali-ui-components/internal/window/initial-window-state-resolver.h>
#include <dali-ui-test-suite-utils.h>

#include "window-frame-test-adapter.h"

using namespace Dali::Ui;
using namespace Dali::Ui::Internal;

int UtcDaliWindowFrameResolverAutoFullSizeRequestsMaximizeOnce(void)
{
  FakeWindowOperationAdapter     adapter = MakeSupportedAdapter();
  InitialWindowStateResolver     resolver;
  const Dali::Window::WindowSize restoreSize(400, 260);

  DALI_TEST_CHECK(resolver.Resolve(Dali::Window::WindowSize(1920, 1080), adapter, restoreSize) == LaunchStateResolution::RESOLVED_MAXIMIZED);
  DALI_TEST_CHECK(resolver.Resolve(Dali::Window::WindowSize(1920, 1080), adapter, restoreSize) == LaunchStateResolution::RESOLVED_MAXIMIZED);
  DALI_TEST_EQUALS(adapter.maximizeRequestCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(adapter.requestedMaximize);

  // The launch maximize carries no size: the window system resets the size it
  // remembers when it maximizes. It is held for the first restore instead.
  DALI_TEST_CHECK(!adapter.requestedRestoreSize);
  END_TEST;
}

int UtcDaliWindowFrameResolverFullSizeLaunchHoldsRestoreSizeForFirstRestore(void)
{
  FakeWindowOperationAdapter     adapter = MakeSupportedAdapter();
  InitialWindowStateResolver     resolver;
  const Dali::Window::WindowSize restoreSize(400, 260);

  DALI_TEST_CHECK(resolver.Resolve(Dali::Window::WindowSize(1920, 1080), adapter, restoreSize) == LaunchStateResolution::RESOLVED_MAXIMIZED);

  const std::optional<Dali::Window::WindowSize> deferred = resolver.TakeInitialRestoreSize();
  DALI_TEST_CHECK(deferred);
  DALI_TEST_EQUALS(deferred->GetWidth(), 400u, TEST_LOCATION);
  DALI_TEST_EQUALS(deferred->GetHeight(), 260u, TEST_LOCATION);

  // Spent once. Later restores fall back to the geometry the compositor now
  // remembers, so nothing is forced on them.
  DALI_TEST_CHECK(!resolver.TakeInitialRestoreSize());
  END_TEST;
}

int UtcDaliWindowFrameResolverExplicitPartialSizeIsNormal(void)
{
  FakeWindowOperationAdapter adapter = MakeSupportedAdapter();
  InitialWindowStateResolver resolver;

  DALI_TEST_CHECK(resolver.Resolve(Dali::Window::WindowSize(400, 260), adapter) == LaunchStateResolution::RESOLVED_NORMAL);
  DALI_TEST_EQUALS(adapter.maximizeRequestCount, 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWindowFrameResolverScreenSizeToleranceCoversTarget(void)
{
  FakeWindowOperationAdapter adapter = MakeSupportedAdapter();
  InitialWindowStateResolver resolver(WindowFrameInitialStatePolicy::AUTO, 1.0f);

  DALI_TEST_CHECK(resolver.Resolve(Dali::Window::WindowSize(1919, 1079), adapter) == LaunchStateResolution::RESOLVED_MAXIMIZED);
  END_TEST;
}

int UtcDaliWindowFrameResolverUseCurrentPolicySkipsAutoMaximize(void)
{
  FakeWindowOperationAdapter adapter = MakeSupportedAdapter();
  InitialWindowStateResolver resolver(WindowFrameInitialStatePolicy::USE_CURRENT);

  DALI_TEST_CHECK(resolver.Resolve(Dali::Window::WindowSize(1920, 1080), adapter) == LaunchStateResolution::RESOLVED_NORMAL);
  DALI_TEST_EQUALS(adapter.maximizeRequestCount, 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWindowFrameResolverMaximizedPolicyOverridesPartialSize(void)
{
  FakeWindowOperationAdapter     adapter = MakeSupportedAdapter();
  InitialWindowStateResolver     resolver(WindowFrameInitialStatePolicy::REQUEST_MAXIMIZE);
  const Dali::Window::WindowSize restoreSize(320, 200);

  DALI_TEST_CHECK(resolver.Resolve(Dali::Window::WindowSize(400, 260), adapter, restoreSize) == LaunchStateResolution::RESOLVED_MAXIMIZED);
  DALI_TEST_EQUALS(adapter.maximizeRequestCount, 1, TEST_LOCATION);

  // A partial launch already has normal geometry, so the compositor owns the
  // restore size and the first restore has nothing to supply.
  DALI_TEST_CHECK(!adapter.requestedRestoreSize);
  DALI_TEST_CHECK(!resolver.TakeInitialRestoreSize());
  END_TEST;
}

int UtcDaliWindowFrameResolverWaitsForValidTargetBounds(void)
{
  FakeWindowOperationAdapter adapter = MakeSupportedAdapter();
  adapter.targetBounds.reset();
  InitialWindowStateResolver resolver;

  DALI_TEST_CHECK(resolver.Resolve(Dali::Window::WindowSize(1920, 1080), adapter) == LaunchStateResolution::WAITING_FOR_TARGET_BOUNDS);

  adapter.targetBounds = Dali::PositionSize(0, 0, 1920, 1080);
  DALI_TEST_CHECK(resolver.Resolve(Dali::Window::WindowSize(1920, 1080), adapter) == LaunchStateResolution::RESOLVED_MAXIMIZED);

  resolver.Resolve(Dali::Window::WindowSize(1920, 1080), adapter);
  DALI_TEST_EQUALS(adapter.maximizeRequestCount, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWindowFrameResolverResolvedNormalIsOneShot(void)
{
  FakeWindowOperationAdapter adapter = MakeSupportedAdapter();
  InitialWindowStateResolver resolver;
  resolver.Resolve(Dali::Window::WindowSize(400, 260), adapter);

  // A later full-size resize is the user's, not a launch, so it must not
  // reopen the launch decision.
  DALI_TEST_CHECK(resolver.Resolve(Dali::Window::WindowSize(1920, 1080), adapter) == LaunchStateResolution::RESOLVED_NORMAL);
  DALI_TEST_EQUALS(adapter.maximizeRequestCount, 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWindowFrameResolverAlreadyMaximizedNeedsNoRequest(void)
{
  FakeWindowOperationAdapter adapter = MakeSupportedAdapter();
  adapter.nativeState.maximized      = true;
  InitialWindowStateResolver resolver;

  DALI_TEST_CHECK(resolver.Resolve(Dali::Window::WindowSize(1920, 1080), adapter) == LaunchStateResolution::RESOLVED_MAXIMIZED);
  DALI_TEST_EQUALS(adapter.maximizeRequestCount, 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWindowFrameResolverRejectedRequestRollsBackN(void)
{
  FakeWindowOperationAdapter adapter = MakeSupportedAdapter();
  adapter.maximizeRequestResult      = false;
  InitialWindowStateResolver resolver;

  DALI_TEST_CHECK(resolver.Resolve(Dali::Window::WindowSize(1920, 1080), adapter) == LaunchStateResolution::RESOLVED_NORMAL);
  END_TEST;
}

int UtcDaliWindowFrameResolverNotDispatchedFullSizeIsExplicitN(void)
{
  FakeWindowOperationAdapter adapter           = MakeSupportedAdapter();
  adapter.featureConfig.maximizeRestoreEnabled = false;
  InitialWindowStateResolver     resolver;
  const Dali::Window::WindowSize restoreSize(400, 260);

  DALI_TEST_CHECK(resolver.Resolve(Dali::Window::WindowSize(1920, 1080), adapter, restoreSize) == LaunchStateResolution::RESOLVED_FULL_SIZE_NOT_DISPATCHED);
  DALI_TEST_EQUALS(adapter.maximizeRequestCount, 0, TEST_LOCATION);
  DALI_TEST_CHECK(!resolver.TakeInitialRestoreSize());
  END_TEST;
}
