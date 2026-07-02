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
 *
 */

#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
struct LayoutFinishedSignalData
{
  LayoutFinishedSignalData()
  : called(false),
    count(0)
  {
  }

  bool   called;
  int    count;
  Window window;
};

struct LayoutFinishedSignalFunctor
{
  LayoutFinishedSignalFunctor(LayoutFinishedSignalData& data)
  : signalData(data)
  {
  }

  void operator()(Window window)
  {
    signalData.called = true;
    ++signalData.count;
    signalData.window = window;
  }

  LayoutFinishedSignalData& signalData;
};

// Slot that removes the layout controller for its window from within the
// emit, exercising the deferred self-destruct path.
struct RemoveControllerInSlotFunctor
{
  void operator()(Window window)
  {
    LayoutController::Remove(window);
  }
};
} // namespace

void utc_dali_layoutcontroller_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_layoutcontroller_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliLayoutControllerGetP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  LayoutController& controller = LayoutController::Get(window);
  controller.ProcessLayouts();
  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliLayoutControllerOnWindowResizeP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  LayoutController& controller = LayoutController::Get(window);
  controller.OnWindowResize(320, 240);
  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliLayoutControllerProcessLayoutsP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  // Add a view to the window so the controller has something to process.
  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  window.Add(view);

  LayoutController& controller = LayoutController::Get(window);
  controller.ProcessLayouts();

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliLayoutControllerGetSameWindowReturnsSameInstanceP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  LayoutController& a = LayoutController::Get(window);
  LayoutController& b = LayoutController::Get(window);
  DALI_TEST_CHECK(&a == &b);
  END_TEST;
}

int UtcDaliLayoutControllerLayoutFinishedSignalP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  LayoutController& controller = LayoutController::Get(window);

  LayoutFinishedSignalData    data;
  LayoutFinishedSignalFunctor functor(data);
  controller.LayoutFinishedSignal().Connect(&application, functor);

  // Create a layout root under the window; this invalidates layout and queues
  // the root with the controller.
  View root = View::New();
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(100.0f);
  window.Add(root);

  // Drive a layout pass: the pending work drains to nothing, so the signal
  // fires exactly once with this window.
  controller.ProcessLayouts();

  DALI_TEST_EQUALS(data.called, true, TEST_LOCATION);
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);
  DALI_TEST_CHECK(data.window == window);
  END_TEST;
}

int UtcDaliLayoutControllerLayoutFinishedSignalNoSpuriousEmitP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  LayoutController& controller = LayoutController::Get(window);

  LayoutFinishedSignalData    data;
  LayoutFinishedSignalFunctor functor(data);
  controller.LayoutFinishedSignal().Connect(&application, functor);

  View root = View::New();
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(100.0f);
  window.Add(root);

  controller.ProcessLayouts();
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);

  // Subsequent passes with nothing pending must NOT re-fire (latch cleared).
  controller.ProcessLayouts();
  controller.ProcessLayouts();
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutControllerLayoutFinishedSignalRefiresP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  LayoutController& controller = LayoutController::Get(window);

  LayoutFinishedSignalData    data;
  LayoutFinishedSignalFunctor functor(data);
  controller.LayoutFinishedSignal().Connect(&application, functor);

  View root = View::New();
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(100.0f);
  window.Add(root);

  controller.ProcessLayouts();
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);

  // Invalidate again: a new dirty->quiescent transition fires the signal again.
  root.SetRequestedWidth(200.0f);
  controller.ProcessLayouts();
  DALI_TEST_EQUALS(data.count, 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutControllerLayoutFinishedSignalRemoveInSlotP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  LayoutController& controller = LayoutController::Get(window);

  RemoveControllerInSlotFunctor functor;
  controller.LayoutFinishedSignal().Connect(&application, functor);

  View root = View::New();
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(100.0f);
  window.Add(root);

  // The slot destroys the controller during the emit; the deferred
  // self-destruct must not cause a use-after-free.
  controller.ProcessLayouts();
  // 'controller' is now dangling and must not be touched again.

  // A fresh Get() recreates the controller for the window without crashing.
  LayoutController& fresh = LayoutController::Get(window);
  fresh.ProcessLayouts();
  DALI_TEST_CHECK(true);
  END_TEST;
}
