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

#include <dali-ui-components/internal/window/window-frame-state-controller.h>
#include <dali-ui-test-suite-utils.h>

#include "window-frame-test-adapter.h"

using namespace Dali::Ui;
using namespace Dali::Ui::Internal;

int UtcDaliWindowFrameStateLaunchResolvesOnce(void)
{
  FakeWindowOperationAdapter adapter = MakeSupportedAdapter();
  WindowFrameStateController controller;

  // A full-size launch asks for maximize once. The window system completes a
  // request it received, so the launch state settles immediately.
  controller.ResolveInitialState(Dali::Window::WindowSize(1920, 1080), adapter, Dali::Window::WindowSize(400, 260));
  DALI_TEST_CHECK(controller.GetLaunchResolution() == LaunchStateResolution::RESOLVED_MAXIMIZED);
  DALI_TEST_EQUALS(adapter.maximizeRequestCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(adapter.requestedMaximize);

  adapter.nativeState.maximized                          = true;
  const std::optional<WindowFrame::WindowState> reported = controller.SynchronizeNativeState(Dali::Window::WindowSize(1920, 1080), adapter, std::nullopt);
  DALI_TEST_CHECK(reported && *reported == WindowFrame::WindowState::MAXIMIZED);

  // Repeating a state the controller already observed publishes nothing.
  DALI_TEST_CHECK(!controller.SynchronizeNativeState(Dali::Window::WindowSize(1920, 1080), adapter, std::nullopt));
  END_TEST;
}

int UtcDaliWindowFrameStatePartialLaunchStaysNormal(void)
{
  FakeWindowOperationAdapter adapter = MakeSupportedAdapter();
  WindowFrameStateController controller;

  controller.ResolveInitialState(Dali::Window::WindowSize(400, 260), adapter, std::nullopt);
  DALI_TEST_EQUALS(adapter.maximizeRequestCount, 0, TEST_LOCATION);
  DALI_TEST_CHECK(controller.GetState() == WindowFrame::WindowState::NORMAL);
  END_TEST;
}

int UtcDaliWindowFrameStateFollowsTheWindowSystem(void)
{
  FakeWindowOperationAdapter adapter = MakeSupportedAdapter();
  WindowFrameStateController controller(WindowFrameInitialStatePolicy::USE_CURRENT);
  controller.ResolveInitialState(Dali::Window::WindowSize(400, 260), adapter, std::nullopt);

  adapter.nativeState.maximized                           = true;
  const std::optional<WindowFrame::WindowState> maximized = controller.SynchronizeNativeState(Dali::Window::WindowSize(1920, 1080), adapter, std::nullopt);
  DALI_TEST_CHECK(maximized && *maximized == WindowFrame::WindowState::MAXIMIZED);
  DALI_TEST_CHECK(controller.IsMaximized());
  DALI_TEST_CHECK(controller.GetState() == WindowFrame::WindowState::MAXIMIZED);

  adapter.nativeState.minimized = true;
  controller.SynchronizeNativeState(Dali::Window::WindowSize(1920, 1080), adapter, std::nullopt);
  DALI_TEST_CHECK(controller.IsMinimized());
  DALI_TEST_CHECK(!controller.IsMaximized());
  DALI_TEST_CHECK(!controller.GetToggleTarget());

  // Leaving minimized follows whatever the window system reports. There is no
  // remembered mode that can disagree with it.
  adapter.nativeState.minimized = false;
  adapter.nativeState.maximized = false;
  controller.SynchronizeNativeState(Dali::Window::WindowSize(400, 260), adapter, std::nullopt);
  DALI_TEST_CHECK(controller.GetState() == WindowFrame::WindowState::NORMAL);
  DALI_TEST_CHECK(controller.GetToggleTarget() && *controller.GetToggleTarget());
  END_TEST;
}

int UtcDaliWindowFrameStateFullSizeWithoutMaximizeCannotToggle(void)
{
  FakeWindowOperationAdapter adapter           = MakeSupportedAdapter();
  adapter.featureConfig.maximizeRestoreEnabled = false;
  WindowFrameStateController controller;

  controller.ResolveInitialState(Dali::Window::WindowSize(1920, 1080), adapter, std::nullopt);
  DALI_TEST_CHECK(controller.GetLaunchResolution() == LaunchStateResolution::RESOLVED_FULL_SIZE_NOT_DISPATCHED);
  DALI_TEST_CHECK(!controller.GetToggleTarget());
  END_TEST;
}
