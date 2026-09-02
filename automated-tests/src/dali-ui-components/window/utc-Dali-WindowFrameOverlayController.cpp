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

#include <dali-ui-components/internal/window/window-frame-overlay-controller.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali::Ui;
using namespace Dali::Ui::Internal;

namespace
{
// An overlay only becomes active once it is enabled, maximized and attached.
WindowFrameOverlayController ActiveOverlay()
{
  WindowFrameOverlayController controller;
  controller.SetOverlayEnabled(true);
  controller.SetEffectiveMaximized(true);
  return controller;
}
} // unnamed namespace

int UtcDaliWindowFrameOverlayActivationTimeoutAndReveal(void)
{
  WindowFrameOverlayController controller = ActiveOverlay();

  DALI_TEST_CHECK(controller.SetAttached(true).restartTimer);
  DALI_TEST_CHECK(controller.IsOverlayEnabled());
  DALI_TEST_CHECK(controller.IsOverlayActive());
  DALI_TEST_CHECK(controller.IsEffectivelyVisible());
  DALI_TEST_CHECK(controller.ShouldTimerRun());

  const OverlayStateUpdate timeout = controller.OnAutoHideTimeout();
  DALI_TEST_CHECK(timeout.stopTimer);
  DALI_TEST_CHECK(controller.IsAutoHidden());
  DALI_TEST_CHECK(!controller.IsEffectivelyVisible());
  DALI_TEST_CHECK(!controller.ShouldTimerRun());

  const OverlayStateUpdate reveal = controller.RevealTemporarily();
  DALI_TEST_CHECK(reveal.restartTimer);
  DALI_TEST_CHECK(controller.IsEffectivelyVisible());
  END_TEST;
}

int UtcDaliWindowFrameOverlayRequestedVisibilityWins(void)
{
  WindowFrameOverlayController controller = ActiveOverlay();
  controller.SetAttached(true);

  const OverlayStateUpdate hidden = controller.SetRequestedVisible(false);
  DALI_TEST_CHECK(hidden.stopTimer);
  DALI_TEST_CHECK(!controller.IsEffectivelyVisible());

  const OverlayStateUpdate reveal = controller.RevealTemporarily();
  DALI_TEST_CHECK(!reveal.restartTimer);
  DALI_TEST_CHECK(!controller.IsEffectivelyVisible());

  const OverlayStateUpdate shown = controller.SetRequestedVisible(true);
  DALI_TEST_CHECK(shown.restartTimer);
  DALI_TEST_CHECK(controller.IsEffectivelyVisible());
  END_TEST;
}

int UtcDaliWindowFrameOverlayWindowVisibilityAndRestore(void)
{
  WindowFrameOverlayController controller = ActiveOverlay();
  controller.SetAttached(true);
  controller.OnAutoHideTimeout();

  DALI_TEST_CHECK(controller.SetWindowVisible(false).stopTimer);

  const OverlayStateUpdate shownWindow = controller.SetWindowVisible(true);
  DALI_TEST_CHECK(shownWindow.restartTimer);
  DALI_TEST_CHECK(controller.IsEffectivelyVisible());

  const OverlayStateUpdate restored = controller.SetEffectiveMaximized(false);
  DALI_TEST_CHECK(restored.stopTimer);
  DALI_TEST_CHECK(!controller.IsOverlayActive());
  DALI_TEST_CHECK(controller.IsEffectivelyVisible());
  END_TEST;
}

int UtcDaliWindowFrameOverlayDetachCancelsWithoutChangingRequest(void)
{
  WindowFrameOverlayController controller = ActiveOverlay();
  controller.SetAttached(true);

  DALI_TEST_CHECK(controller.SetAttached(false).stopTimer);
  DALI_TEST_CHECK(controller.IsRequestedVisible());
  DALI_TEST_CHECK(!controller.IsAutoHidden());
  END_TEST;
}
