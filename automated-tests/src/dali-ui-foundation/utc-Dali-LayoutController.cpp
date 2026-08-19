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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

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
  : called(false),
    count(0),
    bounds(0.0f, 0.0f, 0.0f, 0.0f)
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
  : target(target),
    count(count),
    done(done)
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
  : controller(c),
    target(target),
    count(count),
    done(done)
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
  : window(window),
    count(count)
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

// Counts window layout-finished emits. Connected to a NEW controller created
// from inside another controller's emit.
struct CountLayoutFinishedFunctor
{
  explicit CountLayoutFinishedFunctor(int& count)
  : count(count)
  {
  }
  void operator()(Window)
  {
    ++count;
  }
  int& count;
};

// Slot that removes the controller for the emitting window and immediately
// re-obtains a NEW controller for the same window, connecting a counting slot
// to the new controller's signal. Exercises the idempotence of the deferred
// detach: the pass at the outermost Process unwind must leave the new
// controller alone.
struct RemoveThenGetInSlotFunctor
{
  RemoveThenGetInSlotFunctor(ConnectionTracker* tracker, int& count, int& newControllerCount)
  : tracker(tracker),
    count(count),
    newControllerCount(newControllerCount)
  {
  }
  void operator()(Window window)
  {
    ++count;
    LayoutController::Remove(window);
    LayoutController::Get(window).LayoutFinishedSignal().Connect(tracker, CountLayoutFinishedFunctor(newControllerCount));
  }
  ConnectionTracker* tracker;
  int&               count;
  int&               newControllerCount;
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

  // Drive a full pre+post cycle: the pre phase drains the pending work and the
  // post phase (after size negotiation) emits the signal exactly once.
  application.SendNotification();

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

  application.SendNotification();
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);

  // Subsequent cycles with nothing pending must NOT re-fire (latch cleared;
  // the post phase sees no scheduled emit).
  application.SendNotification();
  application.SendNotification();
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

  application.SendNotification();
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);

  // Invalidate again: a new dirty->quiescent transition fires the signal again.
  root.SetRequestedWidth(200.0f);
  application.SendNotification();
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

  // The slot destroys the controller during the post-phase emit; the deferred
  // self-destruct (at the outermost Process unwind) must not cause a
  // use-after-free.
  application.SendNotification();
  DALI_TEST_EQUALS(removeCount, 1, TEST_LOCATION); // window signal fired exactly once before destroy
  // 'controller' is now dangling and must not be touched again.

  // A fresh Get() recreates the controller for the window without crashing.
  LayoutController& fresh = LayoutController::Get(window);
  (void)fresh;
  application.SendNotification();
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

  // Drive a full pre+post cycle; the View signal emits in the post phase.
  application.SendNotification();

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

  application.SendNotification();
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);
  application.SendNotification();
  application.SendNotification();
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

  application.SendNotification();
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);
  root.SetRequestedWidth(240.0f);
  application.SendNotification();
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

  application.SendNotification();
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

  application.SendNotification();

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

  // Post phase emits the View signal; its slot re-invalidates, so the window
  // signal is suppressed this episode and deferred to the next cycle.
  application.SendNotification();
  DALI_TEST_EQUALS(winData.count, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(viewEmitCount, 1, TEST_LOCATION);
  application.SendNotification();
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

  // The slot re-enters ProcessLayouts() during the post-phase emit; drive
  // several full cycles so the re-invalidated + re-entered episode settles and
  // b's final (width 120) bounds are delivered.
  application.SendNotification();
  application.SendNotification();
  application.SendNotification();
  application.SendNotification();

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

  application.SendNotification();
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);

  root.Remove(child);
  int before = data.count;
  root.SetRequestedWidth(240.0f);
  application.SendNotification();
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

  int                             slotCount = 0;
  ViewSlotRemoveControllerFunctor functor(window, slotCount);
  root.LayoutFinishedSignal().Connect(&application, functor);

  // The View slot destroys the controller during the post-phase View emit; the
  // deferred self-destruct at the outermost Process unwind must be UAF-safe.
  application.SendNotification();
  DALI_TEST_EQUALS(slotCount, 1, TEST_LOCATION);

  LayoutController& fresh = LayoutController::Get(window);
  (void)fresh;
  application.SendNotification();
  DALI_TEST_CHECK(true);
  END_TEST;
}

int UtcDaliLayoutControllerRemoveThenGetInSlotP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  int                        slotCount          = 0;
  int                        newControllerCount = 0;
  RemoveThenGetInSlotFunctor functor(&application, slotCount, newControllerCount);
  LayoutController::Get(window).LayoutFinishedSignal().Connect(&application, functor);

  View root = View::New();
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(100.0f);
  window.Add(root);

  // The slot removes the controller and creates a NEW one for the same window
  // during the emit. The deferred detach that runs when the old controller's
  // Process frame unwinds is idempotent and must NOT queue the new controller
  // for the deferred free.
  application.SendNotification();
  DALI_TEST_EQUALS(slotCount, 1, TEST_LOCATION);

  // Drain the deferred free; only the OLD controller may be freed here.
  application.RunIdles();

  // The new controller must be alive and functional: a fresh layout pass has
  // to emit through the slot connected from inside the removal slot.
  root.SetRequestedWidth(150.0f);
  application.SendNotification();
  DALI_TEST_EQUALS(newControllerCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(slotCount, 1, TEST_LOCATION); // detached controller stays silent
  END_TEST;
}

int UtcDaliLayoutControllerRemoveInSlotIdleReapP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  int                           removeCount = 0;
  RemoveControllerInSlotFunctor functor(removeCount);
  LayoutController::Get(window).LayoutFinishedSignal().Connect(&application, functor);

  View root = View::New();
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(100.0f);
  window.Add(root);

  // The slot detaches the controller during the emit; the free is queued for
  // the idle callback.
  application.SendNotification();
  DALI_TEST_EQUALS(removeCount, 1, TEST_LOCATION);

  // Idle runs the deferred free of the detached controller. Must not crash.
  application.RunIdles();

  // A fresh controller works end-to-end after the reap.
  LayoutFinishedSignalData    data;
  LayoutFinishedSignalFunctor freshFunctor(data);
  LayoutController::Get(window).LayoutFinishedSignal().Connect(&application, freshFunctor);
  root.SetRequestedWidth(180.0f);
  application.SendNotification();
  DALI_TEST_EQUALS(data.called, true, TEST_LOCATION);
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);

  // Removing from ordinary (non-slot) code also frees only at idle; afterwards
  // the freed controller's signal is gone and a third controller takes over
  // without re-firing the old slot.
  LayoutController::Remove(window);
  application.RunIdles();

  root.SetRequestedWidth(120.0f);
  application.SendNotification();
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);
  END_TEST;
}

namespace
{
// Records the constraint the producer was run at, so a test can assert the UNIT of that
// constraint and not merely the size that came out of it.
float gRecordedMeasureWidthConstraint  = -1.0f;
float gRecordedMeasureHeightConstraint = -1.0f;

MeasuredSize RecordMeasureConstraint(View, float widthConstraint, float heightConstraint)
{
  gRecordedMeasureWidthConstraint  = widthConstraint;
  gRecordedMeasureHeightConstraint = heightConstraint;
  return MeasuredSize(200.0f, 160.0f);
}
} // namespace

// A layout root's requested size is stored in NATURAL units while every constraint the
// layout pass carries is VISUAL, so the controller must scale it. The other two sources
// of a root constraint -- the window size and the parent actor SIZE -- are visual
// already, and so is the parameter Measure() takes, so a root measured at its raw
// requested size is run at a constraint 1/s too small.
//
// UiScaleManagerImpl keeps the scale in a process-wide singleton, so the incoming scale
// is recorded and restored.
//
// Non-vacuity (verified by mutation): dropping the * scale from the FIXED width branch
// of ProcessLayoutRoot makes the recorded width constraint 100 instead of 200.
int UtcDaliLayoutControllerFixedRootMeasuredAtVisualConstraintP(void)
{
  UiTestApplication application;
  tet_infoline("A FIXED-size layout root is measured at a VISUAL constraint");

  const float originalScale = UiScaleManager::Get().GetScale();
  UiScaleManager::Get().SetScale(2.0f);

  gRecordedMeasureWidthConstraint  = -1.0f;
  gRecordedMeasureHeightConstraint = -1.0f;

  Window window = application.GetWindow();

  View root = View::New();
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(80.0f);
  root.SetMeasureCallback(MeasureCallback::New(&RecordMeasureConstraint));
  window.Add(root);

  application.SendNotification();
  application.Render();

  // Exact, not epsilon: the value under test is a factor of the scale apart from the
  // wrong one, and an epsilon compare here would only weaken the assertion.
  DALI_TEST_EQUALS(gRecordedMeasureWidthConstraint, 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(gRecordedMeasureHeightConstraint, 160.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(root.GetProperty<float>(Actor::Property::SIZE_WIDTH), 200.0f, TEST_LOCATION);

  UiScaleManager::Get().SetScale(originalScale);
  END_TEST;
}

// The same root at the unit scale, where natural and visual units coincide: the
// conversion must be a no-op there, or every existing app would see a changed
// constraint.
int UtcDaliLayoutControllerFixedRootConstraintUnchangedAtUnitScaleP(void)
{
  UiTestApplication application;
  tet_infoline("A FIXED-size layout root sees its requested size unchanged at scale 1");

  const float originalScale = UiScaleManager::Get().GetScale();
  UiScaleManager::Get().SetScale(1.0f);

  gRecordedMeasureWidthConstraint  = -1.0f;
  gRecordedMeasureHeightConstraint = -1.0f;

  Window window = application.GetWindow();

  View root = View::New();
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(80.0f);
  root.SetMeasureCallback(MeasureCallback::New(&RecordMeasureConstraint));
  window.Add(root);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gRecordedMeasureWidthConstraint, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(gRecordedMeasureHeightConstraint, 80.0f, TEST_LOCATION);

  UiScaleManager::Get().SetScale(originalScale);
  END_TEST;
}

// The end-to-end geometry of the same shape at a non-unit scale: a FIXED root with a
// WRAP_CONTENT boundary child that wraps a 90 wide grandchild. Every extent on the path
// (the root's own visual size, the extent the boundary child is handed, and the child's
// wrapped content) is in visual units, so the child settles at 90 * 2.
//
// Unlike the two tests above this one does NOT discriminate the FIXED-branch conversion,
// and deliberately so: it is the geometry regression guard for the shape the conversion
// touches. A FIXED root's own size comes from its requested size rather than from the
// constraint, and a boundary child is re-measured by its own root pass from the parent's
// (already visual) actor SIZE, so the constraint the root's producer is run at -- what the
// two tests above assert directly -- is not observable in the settled geometry here.
int UtcDaliLayoutControllerFixedRootStandaloneChildGetsFullExtentP(void)
{
  UiTestApplication application;
  tet_infoline("A FIXED-size root hands its boundary child the full visual extent");

  const float originalScale = UiScaleManager::Get().GetScale();
  UiScaleManager::Get().SetScale(2.0f);

  Window window = application.GetWindow();

  View root = View::New();
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(80.0f);
  window.Add(root);

  View standalone = View::New();
  standalone.SetLayoutMode(LayoutMode::STANDALONE);
  standalone.SetRequestedWidth(WRAP_CONTENT);
  standalone.SetMaximumWidth(1000.0f);
  root.Add(standalone);

  View grandChild = View::New();
  grandChild.SetRequestedWidth(90.0f);
  standalone.Add(grandChild);

  application.SendNotification();
  application.Render();

  // 90 natural content at scale 2, and a boundary child that wraps it.
  DALI_TEST_EQUALS(standalone.GetProperty<float>(Actor::Property::SIZE_WIDTH), 180.0f, TEST_LOCATION);

  UiScaleManager::Get().SetScale(originalScale);
  END_TEST;
}
