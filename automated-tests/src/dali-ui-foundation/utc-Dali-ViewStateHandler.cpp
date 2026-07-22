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

#include <stdlib.h>
#include <iostream>

#include <dali.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-foundation/integration-api/view-integ.h>

namespace ExtensionView = Dali::Ui::Extension;
namespace IntegrationView = Dali::Ui::Integration::View;

using namespace Dali;
using namespace Dali::Ui;

namespace
{

// ============================================================================
// Helpers
// ============================================================================

struct StateChangedData
{
  StateChangedData()
  : callCount(0)
  {
  }

  void Reset()
  {
    callCount = 0;
    view      = View();
    prev      = ViewState::NORMAL;
    cur       = ViewState::NORMAL;
  }

  int     callCount;
  View    view;
  ViewState prev;
  ViewState cur;
};

struct StateChangedFunctor
{
  StateChangedFunctor(StateChangedData& data)
  : mData(data)
  {
  }

  void operator()(View view, const StateEvent& e)
  {
    ++mData.callCount;
    mData.view = view;
    mData.prev = e.GetPrev();
    mData.cur  = e.GetCurrent();
  }

  StateChangedData& mData;
};

/**
 * @brief A helper class that implements ConnectionTracker for tracking signal lifetime.
 */
class StateHandlerTracker : public ConnectionTracker
{
public:
  StateHandlerTracker(StateChangedData& data)
  : mData(data)
  {
  }

  void HandleStateChanged(View view, const StateEvent& e)
  {
    ++mData.callCount;
    mData.view = view;
    mData.prev = e.GetPrev();
    mData.cur  = e.GetCurrent();
  }

  StateChangedData& mData;
};

View CreateView(UiTestApplication& application)
{
  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  return view;
}

} // namespace

void utc_dali_viewstatehandler_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_viewstatehandler_cleanup(void)
{
  test_return_value = TET_PASS;
}

// ============================================================================
// SetNamedStateObserver — basic registration and dispatch (member function, #2)
// ============================================================================

int UtcDaliViewSetNamedStateObserverMemberFunctionP(void)
{
  UiTestApplication    application;
  View               view = CreateView(application);
  StateChangedData   data;
  StateHandlerTracker tracker(data);

  ExtensionView::SetNamedStateObserver(GetImpl(view), "TestHandler", &tracker, &StateHandlerTracker::HandleStateChanged);

  // Trigger a state change
  ExtensionView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  DALI_TEST_EQUALS(data.callCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(data.view == view);
  DALI_TEST_CHECK(ViewState::FOCUSED.WasAdded(data.prev, data.cur));

  END_TEST;
}

// ============================================================================
// SetNamedStateObserver — lambda form (#4)
// ============================================================================

int UtcDaliViewSetNamedStateObserverLambdaP(void)
{
  UiTestApplication  application;
  View             view = CreateView(application);
  StateChangedData data;
  ConnectionTracker tracker;

  ExtensionView::SetNamedStateObserver(GetImpl(view), "TestHandler", &tracker, StateChangedFunctor(data));

  ExtensionView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  DALI_TEST_EQUALS(data.callCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(ViewState::FOCUSED.WasAdded(data.prev, data.cur));

  END_TEST;
}

// ============================================================================
// SetNamedStateObserver — method chaining returns *this
// ============================================================================

int UtcDaliViewSetNamedStateObserverChainingP(void)
{
  UiTestApplication  application;
  View             view = CreateView(application);
  ConnectionTracker tracker;
  StateChangedData data;

  ExtensionView::SetNamedStateObserver(GetImpl(view), "Handler1", &tracker, StateChangedFunctor(data));
  ExtensionView::SetNamedStateObserver(GetImpl(view), "Handler2", &tracker, StateChangedFunctor(data));
  ExtensionView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  // Both handlers should fire
  DALI_TEST_EQUALS(data.callCount, 2, TEST_LOCATION);

  END_TEST;
}

// ============================================================================
// SetNamedStateObserver — same ID replaces previous handler
// ============================================================================

int UtcDaliViewSetNamedStateObserverReplaceP(void)
{
  UiTestApplication  application;
  View             view = CreateView(application);
  ConnectionTracker tracker;
  StateChangedData data1, data2;

  ExtensionView::SetNamedStateObserver(GetImpl(view), "MyHandler", &tracker, StateChangedFunctor(data1));
  // Register with the same ID — should replace the first
  ExtensionView::SetNamedStateObserver(GetImpl(view), "MyHandler", &tracker, StateChangedFunctor(data2));

  ExtensionView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  DALI_TEST_EQUALS(data1.callCount, 0, TEST_LOCATION); // old handler NOT called
  DALI_TEST_EQUALS(data2.callCount, 1, TEST_LOCATION); // new handler called

  END_TEST;
}

// ============================================================================
// SetNamedStateObserver — no dispatch when state does not change
// ============================================================================

int UtcDaliViewSetNamedStateObserverNoChangeN(void)
{
  UiTestApplication  application;
  View             view = CreateView(application);
  ConnectionTracker tracker;
  StateChangedData data;

  ExtensionView::SetNamedStateObserver(GetImpl(view), "MyHandler", &tracker, StateChangedFunctor(data));

  // Set same state twice — second call should not dispatch
  ExtensionView::SetState(GetImpl(view), ViewState::FOCUSED, true);
  ExtensionView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  DALI_TEST_EQUALS(data.callCount, 1, TEST_LOCATION);

  END_TEST;
}

// ============================================================================
// UnsetNamedStateObserver — removes a handler by ID
// ============================================================================

int UtcDaliViewUnsetNamedStateObserverP(void)
{
  UiTestApplication  application;
  View             view = CreateView(application);
  ConnectionTracker tracker;
  StateChangedData data;

  ExtensionView::SetNamedStateObserver(GetImpl(view), "MyHandler", &tracker, StateChangedFunctor(data));

  bool removed = ExtensionView::UnsetNamedStateObserver(GetImpl(view), "MyHandler");
  DALI_TEST_CHECK(removed);

  ExtensionView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  DALI_TEST_EQUALS(data.callCount, 0, TEST_LOCATION); // not called after removal

  END_TEST;
}

int UtcDaliViewUnsetNamedStateObserverNotFoundN(void)
{
  UiTestApplication application;
  View            view = CreateView(application);

  // No handlers registered — should return false
  bool removed = ExtensionView::UnsetNamedStateObserver(GetImpl(view), "NonExistent");
  DALI_TEST_CHECK(!removed);

  END_TEST;
}

// ============================================================================
// UnsetNamedStateObserver — only removes the specified ID, others intact
// ============================================================================

int UtcDaliViewUnsetNamedStateObserverSeleciveP(void)
{
  UiTestApplication  application;
  View             view = CreateView(application);
  ConnectionTracker tracker;
  StateChangedData dataA, dataB;

  ExtensionView::SetNamedStateObserver(GetImpl(view), "HandlerA", &tracker, StateChangedFunctor(dataA));
  ExtensionView::SetNamedStateObserver(GetImpl(view), "HandlerB", &tracker, StateChangedFunctor(dataB));

  ExtensionView::UnsetNamedStateObserver(GetImpl(view), "HandlerA");

  ExtensionView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  DALI_TEST_EQUALS(dataA.callCount, 0, TEST_LOCATION); // removed
  DALI_TEST_EQUALS(dataB.callCount, 1, TEST_LOCATION); // still active

  END_TEST;
}

// ============================================================================
// UnsetNamedStateObserverIfNotExecuting — skip removal during own handler
// ============================================================================

int UtcDaliViewUnsetNamedStateObserverIfNotExecutingSkipsP(void)
{
  UiTestApplication  application;
  View             view    = CreateView(application);
  ConnectionTracker tracker;
  bool             handlerCalled = false;
  bool             unsetResult   = true; // intentionally set to true to verify it becomes false

  ExtensionView::SetNamedStateObserver(GetImpl(view), "BackgroundColor", &tracker, [&](View v, const StateEvent&) {
    handlerCalled = true;
    // Try to unset self during processing — should be skipped
    unsetResult = ExtensionView::UnsetNamedStateObserverIfNotExecuting(GetImpl(v), "BackgroundColor");
  });

  ExtensionView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  DALI_TEST_CHECK(handlerCalled);
  DALI_TEST_CHECK(!unsetResult); // returns false because it is processing

  // Handler should still be active after skipped removal
  handlerCalled = false;
  ExtensionView::SetState(GetImpl(view), ViewState::PRESSED, true);
  DALI_TEST_CHECK(handlerCalled);

  END_TEST;
}

int UtcDaliViewUnsetNamedStateObserverIfNotExecutingSucceedsP(void)
{
  UiTestApplication  application;
  View             view = CreateView(application);
  ConnectionTracker tracker;
  StateChangedData data;

  ExtensionView::SetNamedStateObserver(GetImpl(view), "BackgroundColor", &tracker, StateChangedFunctor(data));

  // Called from outside a handler — should succeed
  bool removed = ExtensionView::UnsetNamedStateObserverIfNotExecuting(GetImpl(view), "BackgroundColor");
  DALI_TEST_CHECK(removed);

  ExtensionView::SetState(GetImpl(view), ViewState::FOCUSED, true);
  DALI_TEST_EQUALS(data.callCount, 0, TEST_LOCATION);

  END_TEST;
}

// ============================================================================
// Auto-disconnect when tracker is destroyed
// ============================================================================

int UtcDaliViewSetNamedStateObserverAutoDisconnectP(void)
{
  UiTestApplication application;
  View            view = CreateView(application);
  StateChangedData data;

  {
    ConnectionTracker tracker;
    ExtensionView::SetNamedStateObserver(GetImpl(view), "MyHandler", &tracker, StateChangedFunctor(data));

    ExtensionView::SetState(GetImpl(view), ViewState::FOCUSED, true);
    DALI_TEST_EQUALS(data.callCount, 1, TEST_LOCATION);
    data.Reset();

    // tracker goes out of scope — auto-disconnect
  }

  // After tracker destroyed, handler should no longer fire
  ExtensionView::SetState(GetImpl(view), ViewState::PRESSED, true);
  DALI_TEST_EQUALS(data.callCount, 0, TEST_LOCATION);

  END_TEST;
}

// ============================================================================
// Multiple state changes — prev/cur correctness
// ============================================================================

int UtcDaliViewSetNamedStateObserverPrevCurP(void)
{
  UiTestApplication  application;
  View             view = CreateView(application);
  ConnectionTracker tracker;
  StateChangedData data;

  ExtensionView::SetNamedStateObserver(GetImpl(view), "Observer", &tracker, StateChangedFunctor(data));

  ExtensionView::SetState(GetImpl(view), ViewState::FOCUSED, true);
  DALI_TEST_CHECK(!data.prev.Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(data.cur.Contains(ViewState::FOCUSED));
  data.Reset();

  ExtensionView::SetState(GetImpl(view), ViewState::PRESSED, true);
  DALI_TEST_CHECK(data.prev.Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(data.cur.Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(data.cur.Contains(ViewState::PRESSED));

  END_TEST;
}

// ============================================================================
// Multiple handlers with different IDs all called on state change
// ============================================================================

int UtcDaliViewSetNamedStateObserverMultipleHandlersP(void)
{
  UiTestApplication  application;
  View             view = CreateView(application);
  ConnectionTracker tracker;
  StateChangedData dataA, dataB, dataC;

  ExtensionView::SetNamedStateObserver(GetImpl(view), "A", &tracker, StateChangedFunctor(dataA));
  ExtensionView::SetNamedStateObserver(GetImpl(view), "B", &tracker, StateChangedFunctor(dataB));
  ExtensionView::SetNamedStateObserver(GetImpl(view), "C", &tracker, StateChangedFunctor(dataC));

  ExtensionView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  DALI_TEST_EQUALS(dataA.callCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(dataB.callCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(dataC.callCount, 1, TEST_LOCATION);

  END_TEST;
}

// ============================================================================
// Named handlers and StateChangedSignal both fire
// ============================================================================

int UtcDaliViewSetNamedStateObserverAndSignalBothFireP(void)
{
  UiTestApplication  application;
  View             view = CreateView(application);
  ConnectionTracker tracker;
  StateChangedData namedData, signalData;

  // Named handler
  ExtensionView::SetNamedStateObserver(GetImpl(view), "Observer", &tracker, StateChangedFunctor(namedData));
  // Anonymous signal
  view.StateChangedSignal().Connect(&tracker, StateChangedFunctor(signalData));

  ExtensionView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  DALI_TEST_EQUALS(namedData.callCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signalData.callCount, 1, TEST_LOCATION);

  END_TEST;
}
