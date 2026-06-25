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
#include <dali-ui-foundation/integration-api/view-integ.h>

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
// WhenStateChanged — basic registration and dispatch (member function, #2)
// ============================================================================

int UtcDaliViewWhenStateChangedMemberFunctionP(void)
{
  UiTestApplication    application;
  View               view = CreateView(application);
  StateChangedData   data;
  StateHandlerTracker tracker(data);

  IntegrationView::WhenStateChanged(GetImpl(view), "TestHandler", &tracker, &StateHandlerTracker::HandleStateChanged);

  // Trigger a state change
  IntegrationView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  DALI_TEST_EQUALS(data.callCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(data.view == view);
  DALI_TEST_CHECK(ViewState::FOCUSED.WasAdded(data.prev, data.cur));

  END_TEST;
}

// ============================================================================
// WhenStateChanged — lambda form (#4)
// ============================================================================

int UtcDaliViewWhenStateChangedLambdaP(void)
{
  UiTestApplication  application;
  View             view = CreateView(application);
  StateChangedData data;
  ConnectionTracker tracker;

  IntegrationView::WhenStateChanged(GetImpl(view), "TestHandler", &tracker, StateChangedFunctor(data));

  IntegrationView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  DALI_TEST_EQUALS(data.callCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(ViewState::FOCUSED.WasAdded(data.prev, data.cur));

  END_TEST;
}

// ============================================================================
// WhenStateChanged — method chaining returns *this
// ============================================================================

int UtcDaliViewWhenStateChangedChainingP(void)
{
  UiTestApplication  application;
  View             view = CreateView(application);
  ConnectionTracker tracker;
  StateChangedData data;

  IntegrationView::WhenStateChanged(GetImpl(view), "Handler1", &tracker, StateChangedFunctor(data));
  IntegrationView::WhenStateChanged(GetImpl(view), "Handler2", &tracker, StateChangedFunctor(data));
  IntegrationView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  // Both handlers should fire
  DALI_TEST_EQUALS(data.callCount, 2, TEST_LOCATION);

  END_TEST;
}

// ============================================================================
// WhenStateChanged — same ID replaces previous handler
// ============================================================================

int UtcDaliViewWhenStateChangedReplaceP(void)
{
  UiTestApplication  application;
  View             view = CreateView(application);
  ConnectionTracker tracker;
  StateChangedData data1, data2;

  IntegrationView::WhenStateChanged(GetImpl(view), "MyHandler", &tracker, StateChangedFunctor(data1));
  // Register with the same ID — should replace the first
  IntegrationView::WhenStateChanged(GetImpl(view), "MyHandler", &tracker, StateChangedFunctor(data2));

  IntegrationView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  DALI_TEST_EQUALS(data1.callCount, 0, TEST_LOCATION); // old handler NOT called
  DALI_TEST_EQUALS(data2.callCount, 1, TEST_LOCATION); // new handler called

  END_TEST;
}

// ============================================================================
// WhenStateChanged — no dispatch when state does not change
// ============================================================================

int UtcDaliViewWhenStateChangedNoChangeN(void)
{
  UiTestApplication  application;
  View             view = CreateView(application);
  ConnectionTracker tracker;
  StateChangedData data;

  IntegrationView::WhenStateChanged(GetImpl(view), "MyHandler", &tracker, StateChangedFunctor(data));

  // Set same state twice — second call should not dispatch
  IntegrationView::SetState(GetImpl(view), ViewState::FOCUSED, true);
  IntegrationView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  DALI_TEST_EQUALS(data.callCount, 1, TEST_LOCATION);

  END_TEST;
}

// ============================================================================
// UnsetStateHandler — removes a handler by ID
// ============================================================================

int UtcDaliViewUnsetStateHandlerP(void)
{
  UiTestApplication  application;
  View             view = CreateView(application);
  ConnectionTracker tracker;
  StateChangedData data;

  IntegrationView::WhenStateChanged(GetImpl(view), "MyHandler", &tracker, StateChangedFunctor(data));

  bool removed = IntegrationView::UnsetStateHandler(GetImpl(view), "MyHandler");
  DALI_TEST_CHECK(removed);

  IntegrationView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  DALI_TEST_EQUALS(data.callCount, 0, TEST_LOCATION); // not called after removal

  END_TEST;
}

int UtcDaliViewUnsetStateHandlerNotFoundN(void)
{
  UiTestApplication application;
  View            view = CreateView(application);

  // No handlers registered — should return false
  bool removed = IntegrationView::UnsetStateHandler(GetImpl(view), "NonExistent");
  DALI_TEST_CHECK(!removed);

  END_TEST;
}

// ============================================================================
// UnsetStateHandler — only removes the specified ID, others intact
// ============================================================================

int UtcDaliViewUnsetStateHandlerSeleciveP(void)
{
  UiTestApplication  application;
  View             view = CreateView(application);
  ConnectionTracker tracker;
  StateChangedData dataA, dataB;

  IntegrationView::WhenStateChanged(GetImpl(view), "HandlerA", &tracker, StateChangedFunctor(dataA));
  IntegrationView::WhenStateChanged(GetImpl(view), "HandlerB", &tracker, StateChangedFunctor(dataB));

  IntegrationView::UnsetStateHandler(GetImpl(view), "HandlerA");

  IntegrationView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  DALI_TEST_EQUALS(dataA.callCount, 0, TEST_LOCATION); // removed
  DALI_TEST_EQUALS(dataB.callCount, 1, TEST_LOCATION); // still active

  END_TEST;
}

// ============================================================================
// UnsetStateHandlerWhenNotProcessing — skip removal during own handler
// ============================================================================

int UtcDaliViewUnsetStateHandlerWhenNotProcessingSkipsP(void)
{
  UiTestApplication  application;
  View             view    = CreateView(application);
  ConnectionTracker tracker;
  bool             handlerCalled = false;
  bool             unsetResult   = true; // intentionally set to true to verify it becomes false

  IntegrationView::WhenStateChanged(GetImpl(view), "BackgroundColor", &tracker, [&](View v, const StateEvent&) {
    handlerCalled = true;
    // Try to unset self during processing — should be skipped
    unsetResult = IntegrationView::UnsetStateHandlerWhenNotProcessing(GetImpl(v), "BackgroundColor");
  });

  IntegrationView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  DALI_TEST_CHECK(handlerCalled);
  DALI_TEST_CHECK(!unsetResult); // returns false because it is processing

  // Handler should still be active after skipped removal
  handlerCalled = false;
  IntegrationView::SetState(GetImpl(view), ViewState::PRESSED, true);
  DALI_TEST_CHECK(handlerCalled);

  END_TEST;
}

int UtcDaliViewUnsetStateHandlerWhenNotProcessingSucceedsP(void)
{
  UiTestApplication  application;
  View             view = CreateView(application);
  ConnectionTracker tracker;
  StateChangedData data;

  IntegrationView::WhenStateChanged(GetImpl(view), "BackgroundColor", &tracker, StateChangedFunctor(data));

  // Called from outside a handler — should succeed
  bool removed = IntegrationView::UnsetStateHandlerWhenNotProcessing(GetImpl(view), "BackgroundColor");
  DALI_TEST_CHECK(removed);

  IntegrationView::SetState(GetImpl(view), ViewState::FOCUSED, true);
  DALI_TEST_EQUALS(data.callCount, 0, TEST_LOCATION);

  END_TEST;
}

// ============================================================================
// Auto-disconnect when tracker is destroyed
// ============================================================================

int UtcDaliViewWhenStateChangedAutoDisconnectP(void)
{
  UiTestApplication application;
  View            view = CreateView(application);
  StateChangedData data;

  {
    ConnectionTracker tracker;
    IntegrationView::WhenStateChanged(GetImpl(view), "MyHandler", &tracker, StateChangedFunctor(data));

    IntegrationView::SetState(GetImpl(view), ViewState::FOCUSED, true);
    DALI_TEST_EQUALS(data.callCount, 1, TEST_LOCATION);
    data.Reset();

    // tracker goes out of scope — auto-disconnect
  }

  // After tracker destroyed, handler should no longer fire
  IntegrationView::SetState(GetImpl(view), ViewState::PRESSED, true);
  DALI_TEST_EQUALS(data.callCount, 0, TEST_LOCATION);

  END_TEST;
}

// ============================================================================
// Multiple state changes — prev/cur correctness
// ============================================================================

int UtcDaliViewWhenStateChangedPrevCurP(void)
{
  UiTestApplication  application;
  View             view = CreateView(application);
  ConnectionTracker tracker;
  StateChangedData data;

  IntegrationView::WhenStateChanged(GetImpl(view), "Observer", &tracker, StateChangedFunctor(data));

  IntegrationView::SetState(GetImpl(view), ViewState::FOCUSED, true);
  DALI_TEST_CHECK(!data.prev.Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(data.cur.Contains(ViewState::FOCUSED));
  data.Reset();

  IntegrationView::SetState(GetImpl(view), ViewState::PRESSED, true);
  DALI_TEST_CHECK(data.prev.Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(data.cur.Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(data.cur.Contains(ViewState::PRESSED));

  END_TEST;
}

// ============================================================================
// Multiple handlers with different IDs all called on state change
// ============================================================================

int UtcDaliViewWhenStateChangedMultipleHandlersP(void)
{
  UiTestApplication  application;
  View             view = CreateView(application);
  ConnectionTracker tracker;
  StateChangedData dataA, dataB, dataC;

  IntegrationView::WhenStateChanged(GetImpl(view), "A", &tracker, StateChangedFunctor(dataA));
  IntegrationView::WhenStateChanged(GetImpl(view), "B", &tracker, StateChangedFunctor(dataB));
  IntegrationView::WhenStateChanged(GetImpl(view), "C", &tracker, StateChangedFunctor(dataC));

  IntegrationView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  DALI_TEST_EQUALS(dataA.callCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(dataB.callCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(dataC.callCount, 1, TEST_LOCATION);

  END_TEST;
}

// ============================================================================
// Named handlers and StateChangedSignal both fire
// ============================================================================

int UtcDaliViewWhenStateChangedAndSignalBothFireP(void)
{
  UiTestApplication  application;
  View             view = CreateView(application);
  ConnectionTracker tracker;
  StateChangedData namedData, signalData;

  // Named handler
  IntegrationView::WhenStateChanged(GetImpl(view), "Observer", &tracker, StateChangedFunctor(namedData));
  // Anonymous signal
  view.StateChangedSignal().Connect(&tracker, StateChangedFunctor(signalData));

  IntegrationView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  DALI_TEST_EQUALS(namedData.callCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signalData.callCount, 1, TEST_LOCATION);

  END_TEST;
}
