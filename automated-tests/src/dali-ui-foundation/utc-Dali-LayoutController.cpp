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
  RemoveControllerInSlotFunctor(int& count)
  : count(count)
  {
  }
  void operator()(Window window)
  {
    ++count;
    LayoutController::Remove(window);
  }
  int& count;
};

// ---- View::LayoutFinishedSignal test helpers ----
struct ViewLayoutFinishedSignalData
{
  ViewLayoutFinishedSignalData()
  : called(false), count(0), bounds(0.0f, 0.0f, 0.0f, 0.0f)
  {
  }
  bool       called;
  int        count;
  View       view;
  LayoutRect bounds;
};

struct ViewLayoutFinishedSignalFunctor
{
  explicit ViewLayoutFinishedSignalFunctor(ViewLayoutFinishedSignalData& data)
  : d(data)
  {
  }
  void operator()(View view, LayoutRect bounds)
  {
    d.called = true;
    ++d.count;
    d.view   = view;
    d.bounds = bounds;
  }
  ViewLayoutFinishedSignalData& d;
};

// Re-invalidates `target` once on first emit (window-suppress test).
struct ReinvalidateOnceFunctor
{
  ReinvalidateOnceFunctor(View target, int& count, bool& done)
  : target(target), count(count), done(done)
  {
  }
  void operator()(View, LayoutRect)
  {
    ++count;
    if(!done)
    {
      done = true;
      target.SetRequestedWidth(260.0f);
    }
  }
  View  target;
  int&  count;
  bool& done;
};

// On first emit, re-invalidates `target` and RE-ENTERS ProcessLayouts (b1
// stale-skip regression guard).
struct ReenterProcessLayoutsFunctor
{
  ReenterProcessLayoutsFunctor(LayoutController& c, View target, int& count, bool& done)
  : controller(c), target(target), count(count), done(done)
  {
  }
  void operator()(View, LayoutRect)
  {
    ++count;
    if(!done)
    {
      done = true;
      target.SetRequestedWidth(120.0f);
      controller.ProcessLayouts();
    }
  }
  LayoutController& controller;
  View              target;
  int&              count;
  bool&             done;
};

// A View slot that removes the controller for `window`.
struct ViewSlotRemoveControllerFunctor
{
  ViewSlotRemoveControllerFunctor(Window window, int& count)
  : window(window), count(count)
  {
  }
  void operator()(View, LayoutRect)
  {
    ++count;
    LayoutController::Remove(window);
  }
  Window window;
  int&   count;
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

  int                           removeCount = 0;
  RemoveControllerInSlotFunctor functor(removeCount);
  controller.LayoutFinishedSignal().Connect(&application, functor);

  View root = View::New();
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(100.0f);
  window.Add(root);

  // The slot destroys the controller during the emit; the deferred
  // self-destruct must not cause a use-after-free.
  controller.ProcessLayouts();
  DALI_TEST_EQUALS(removeCount, 1, TEST_LOCATION); // window signal fired exactly once before destroy
  // 'controller' is now dangling and must not be touched again.

  // A fresh Get() recreates the controller for the window without crashing.
  LayoutController& fresh = LayoutController::Get(window);
  fresh.ProcessLayouts();
  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliViewLayoutFinishedSignalP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  View              root   = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(40.0f);
  root.Add(child);
  window.Add(root);

  ViewLayoutFinishedSignalData    data;
  ViewLayoutFinishedSignalFunctor functor(data);
  child.LayoutFinishedSignal().Connect(&application, functor);

  LayoutController& controller = LayoutController::Get(window);
  controller.ProcessLayouts();

  DALI_TEST_EQUALS(data.called, true, TEST_LOCATION);
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);
  DALI_TEST_CHECK(data.view == child);
  Actor a = child;
  DALI_TEST_EQUALS(data.bounds.x, a.GetProperty<float>(Actor::Property::POSITION_X), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(data.bounds.y, a.GetProperty<float>(Actor::Property::POSITION_Y), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(data.bounds.width, a.GetProperty<float>(Actor::Property::SIZE_WIDTH), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(data.bounds.height, a.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutFinishedSignalNoSpuriousEmitP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  View              root   = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  window.Add(root);

  ViewLayoutFinishedSignalData    data;
  ViewLayoutFinishedSignalFunctor functor(data);
  root.LayoutFinishedSignal().Connect(&application, functor);

  LayoutController& controller = LayoutController::Get(window);
  controller.ProcessLayouts();
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);
  controller.ProcessLayouts();
  controller.ProcessLayouts();
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutFinishedSignalRefiresP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  View              root   = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  window.Add(root);

  ViewLayoutFinishedSignalData    data;
  ViewLayoutFinishedSignalFunctor functor(data);
  root.LayoutFinishedSignal().Connect(&application, functor);

  LayoutController& controller = LayoutController::Get(window);
  controller.ProcessLayouts();
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);
  root.SetRequestedWidth(240.0f);
  controller.ProcessLayouts();
  DALI_TEST_EQUALS(data.count, 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutFinishedSignalSubscribedViewsOnlyP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  View              root   = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  View childA = View::New();
  childA.SetRequestedWidth(50.0f);
  childA.SetRequestedHeight(40.0f);
  View childB = View::New();
  childB.SetRequestedWidth(60.0f);
  childB.SetRequestedHeight(30.0f);
  root.Add(childA);
  root.Add(childB);
  window.Add(root);

  ViewLayoutFinishedSignalData    data;
  ViewLayoutFinishedSignalFunctor functor(data);
  childA.LayoutFinishedSignal().Connect(&application, functor);

  LayoutController& controller = LayoutController::Get(window);
  controller.ProcessLayouts();
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);
  DALI_TEST_CHECK(data.view == childA);
  END_TEST;
}

int UtcDaliViewLayoutFinishedSignalRtlTargetBoundsP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  View              root   = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  root.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(40.0f);
  root.Add(child);
  window.Add(root);

  ViewLayoutFinishedSignalData    data;
  ViewLayoutFinishedSignalFunctor functor(data);
  child.LayoutFinishedSignal().Connect(&application, functor);

  LayoutController& controller = LayoutController::Get(window);
  controller.ProcessLayouts();

  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);
  Actor a = child;
  DALI_TEST_EQUALS(data.bounds.x, a.GetProperty<float>(Actor::Property::POSITION_X), 0.01f, TEST_LOCATION);
  DALI_TEST_CHECK(data.bounds.x > 0.0f); // mirrored (LTR x would be 0)
  END_TEST;
}

int UtcDaliViewLayoutFinishedSignalSuppressWindowSignalWhenSlotInvalidatesP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  View              root   = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  window.Add(root);

  LayoutFinishedSignalData    winData;
  LayoutFinishedSignalFunctor winFunctor(winData);
  LayoutController&           controller = LayoutController::Get(window);
  controller.LayoutFinishedSignal().Connect(&application, winFunctor);

  int                     viewEmitCount = 0;
  bool                    didInvalidate = false;
  ReinvalidateOnceFunctor viewFunctor(root, viewEmitCount, didInvalidate);
  root.LayoutFinishedSignal().Connect(&application, viewFunctor);

  controller.ProcessLayouts();
  DALI_TEST_EQUALS(winData.count, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(viewEmitCount, 1, TEST_LOCATION);
  controller.ProcessLayouts();
  DALI_TEST_EQUALS(winData.count, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutFinishedSignalSlotReentersProcessLayoutsP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  View              root   = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  View a = View::New();
  a.SetRequestedWidth(50.0f);
  a.SetRequestedHeight(40.0f);
  View b = View::New();
  b.SetRequestedWidth(60.0f);
  b.SetRequestedHeight(30.0f);
  root.Add(a);
  root.Add(b);
  window.Add(root);

  LayoutController& controller = LayoutController::Get(window);

  ViewLayoutFinishedSignalData    bData;
  ViewLayoutFinishedSignalFunctor bFunctor(bData);
  b.LayoutFinishedSignal().Connect(&application, bFunctor);

  int                          aCount = 0;
  bool                         done   = false;
  ReenterProcessLayoutsFunctor aFunctor(controller, b, aCount, done);
  a.LayoutFinishedSignal().Connect(&application, aFunctor);

  controller.ProcessLayouts();
  controller.ProcessLayouts();
  controller.ProcessLayouts();

  // A slot that re-invalidates a sibling AND re-enters ProcessLayouts starts a
  // new dirty->settled episode, so exact emit counts are implementation-defined
  // (a view may legitimately recur per episode). The guaranteed invariants are:
  // no crash / no permanent stranding (each subscriber is delivered), and b's
  // LAST delivered bounds reflect its final (re-invalidated, width 120) layout.
  DALI_TEST_CHECK(aCount >= 1);
  DALI_TEST_CHECK(bData.count >= 1);
  DALI_TEST_EQUALS(bData.bounds.width, b.GetProperty<float>(Actor::Property::SIZE_WIDTH), 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutFinishedSignalRemoveViewBeforeEmitP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  View              root   = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(40.0f);
  root.Add(child);
  window.Add(root);

  ViewLayoutFinishedSignalData    data;
  ViewLayoutFinishedSignalFunctor functor(data);
  child.LayoutFinishedSignal().Connect(&application, functor);

  LayoutController& controller = LayoutController::Get(window);
  controller.ProcessLayouts();
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);

  root.Remove(child);
  int before = data.count;
  root.SetRequestedWidth(240.0f);
  controller.ProcessLayouts();
  DALI_TEST_EQUALS(data.count, before, TEST_LOCATION); // no emit for removed child, no UAF
  END_TEST;
}

int UtcDaliViewLayoutFinishedSignalManualArrangeNoOpP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  View              root   = View::New();
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(100.0f);
  window.Add(root);

  ViewLayoutFinishedSignalData    data;
  ViewLayoutFinishedSignalFunctor functor(data);
  root.LayoutFinishedSignal().Connect(&application, functor);

  GetImpl(root).Arrange(LayoutRect(0.0f, 0.0f, 100.0f, 100.0f)); // no active pass
  DALI_TEST_EQUALS(data.called, false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewLayoutFinishedSignalRemoveControllerInSlotP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  View              root   = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  window.Add(root);

  LayoutController& controller = LayoutController::Get(window);

  int                             slotCount = 0;
  ViewSlotRemoveControllerFunctor functor(window, slotCount);
  root.LayoutFinishedSignal().Connect(&application, functor);

  controller.ProcessLayouts();
  DALI_TEST_EQUALS(slotCount, 1, TEST_LOCATION);

  LayoutController& fresh = LayoutController::Get(window);
  fresh.ProcessLayouts();
  DALI_TEST_CHECK(true);
  END_TEST;
}
