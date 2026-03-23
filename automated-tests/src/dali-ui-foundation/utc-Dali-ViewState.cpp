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
#include <tuple>
#include <vector>

#include <dali.h>
#include <dali-test-suite-utils.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/integration-api/view-impl.h>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Integration;

namespace
{

// A record of one StateChangedSignal (or WhenStateChanged) callback invocation.
struct CallRecord
{
  std::string tag;    ///< Which handler was called (for ordering verification)
  UiState     prev;
  UiState     cur;
};

View CreateView(TestApplication& application)
{
  View view = View::New();
  view.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  return view;
}

View CreateChildView(TestApplication& application, View& parent)
{
  View child = View::New();
  child.SetProperty(Actor::Property::SIZE, Vector2(50.0f, 50.0f));
  parent.Add(child);
  application.SendNotification();
  application.Render();
  return child;
}

} // namespace

void utc_dali_viewstate_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_viewstate_cleanup(void)
{
  test_return_value = TET_PASS;
}

// =============================================================================
// Basic: StateChangedSignal fires with correct prev/cur
// =============================================================================

int UtcDaliViewStateBasicDispatchP(void)
{
  TestApplication   application;
  View              view = CreateView(application);
  ConnectionTracker tracker;

  UiState receivedPrev, receivedCur;
  int     callCount = 0;

  GetImpl(view).WhenStateChanged("observer", &tracker, [&](View, const StateEvent& e) {
    ++callCount;
    receivedPrev = e.GetPrev();
    receivedCur  = e.GetCurrent();
  });

  GetImpl(view).SetViewState(UiState::Focused, true);

  DALI_TEST_EQUALS(callCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!receivedPrev.Contains(UiState::Focused));
  DALI_TEST_CHECK(receivedCur.Contains(UiState::Focused));

  END_TEST;
}

// =============================================================================
// Basic: No dispatch when state does not actually change
// =============================================================================

int UtcDaliViewStateNoDispatchUnchangedN(void)
{
  TestApplication   application;
  View              view = CreateView(application);
  ConnectionTracker tracker;
  int               callCount = 0;

  GetImpl(view).WhenStateChanged("observer", &tracker, [&](View, const StateEvent&) {
    ++callCount;
  });

  GetImpl(view).SetViewState(UiState::Focused, true);
  DALI_TEST_EQUALS(callCount, 1, TEST_LOCATION);

  // Setting the same state again must not dispatch
  GetImpl(view).SetViewState(UiState::Focused, true);
  DALI_TEST_EQUALS(callCount, 1, TEST_LOCATION);

  END_TEST;
}

// =============================================================================
// Deferred notification: handler triggering a new state change must NOT
// interrupt the remaining handlers of the current notification.
//
// Scenario (3 handlers registered: "h1", "h2", "h3"):
//   - Initial state A (Normal)
//   - SetViewState(Focused, true)  →  A→B (B = Focused)
//     - h1 receives (A, B)  ← ok
//     - h2 receives (A, B), then calls SetViewState(Pressed, true)  →  B→C
//     - h3 must still receive (A, B), NOT (B, C)   ← deferred guarantee
//   - After A→B dispatch completes, B→C is dispatched:
//     - h1, h2, h3 all receive (B, C)
//
// Without the fix the old behaviour was:
//   h1(A,B) → h2(A,B) → [re-entrant: h1(B,C) h2(B,C) h3(B,C)] → h3(A,B)
// which produces the wrong interleaved call order.
// =============================================================================

int UtcDaliViewStateDeferredNotificationOrderP(void)
{
  TestApplication   application;
  View              view = CreateView(application);
  ConnectionTracker tracker;

  std::vector<CallRecord> log;

  GetImpl(view).WhenStateChanged("h1", &tracker, [&](View, const StateEvent& e) {
    log.push_back({"h1", e.GetPrev(), e.GetCurrent()});
  });

  GetImpl(view).WhenStateChanged("h2", &tracker, [&](View v, const StateEvent& e) {
    log.push_back({"h2", e.GetPrev(), e.GetCurrent()});
    // Trigger a second state change from inside the handler
    if(e.Added(UiState::Focused))
    {
      GetImpl(v).SetViewState(UiState::Pressed, true);
    }
  });

  GetImpl(view).WhenStateChanged("h3", &tracker, [&](View, const StateEvent& e) {
    log.push_back({"h3", e.GetPrev(), e.GetCurrent()});
  });

  const UiState stateA = UiState::Normal;
  const UiState stateB = UiState::Focused;
  const UiState stateC = UiState::Focused + UiState::Pressed;

  GetImpl(view).SetViewState(UiState::Focused, true);

  // Expect 6 records: 3 for A→B, then 3 for B→C (in registration order)
  DALI_TEST_EQUALS(static_cast<int>(log.size()), 6, TEST_LOCATION);

  // --- A→B batch: h1, h2, h3 each with (stateA, stateB) ---
  DALI_TEST_EQUALS(log[0].tag, std::string("h1"), TEST_LOCATION);
  DALI_TEST_CHECK(log[0].prev == stateA);
  DALI_TEST_CHECK(log[0].cur == stateB);

  DALI_TEST_EQUALS(log[1].tag, std::string("h2"), TEST_LOCATION);
  DALI_TEST_CHECK(log[1].prev == stateA);
  DALI_TEST_CHECK(log[1].cur == stateB);

  DALI_TEST_EQUALS(log[2].tag, std::string("h3"), TEST_LOCATION);  // h3 must NOT be skipped
  DALI_TEST_CHECK(log[2].prev == stateA);
  DALI_TEST_CHECK(log[2].cur == stateB);

  // --- B→C batch: h1, h2, h3 each with (stateB, stateC) ---
  DALI_TEST_EQUALS(log[3].tag, std::string("h1"), TEST_LOCATION);
  DALI_TEST_CHECK(log[3].prev == stateB);
  DALI_TEST_CHECK(log[3].cur == stateC);

  DALI_TEST_EQUALS(log[4].tag, std::string("h2"), TEST_LOCATION);
  DALI_TEST_CHECK(log[4].prev == stateB);
  DALI_TEST_CHECK(log[4].cur == stateC);

  DALI_TEST_EQUALS(log[5].tag, std::string("h3"), TEST_LOCATION);
  DALI_TEST_CHECK(log[5].prev == stateB);
  DALI_TEST_CHECK(log[5].cur == stateC);

  END_TEST;
}

// =============================================================================
// Deferred notification: StateChangedSignal connections also participate
// in the deferred ordering guarantee
// =============================================================================

int UtcDaliViewStateDeferredSignalOrderP(void)
{
  TestApplication   application;
  View              view = CreateView(application);
  ConnectionTracker tracker;

  std::vector<CallRecord> log;

  // Signal connection that triggers a re-entrant state change
  view.StateChangedSignal().Connect(&tracker, [&](View v, const StateEvent& e) {
    log.push_back({"signal-1", e.GetPrev(), e.GetCurrent()});
    if(e.Added(UiState::Focused))
    {
      GetImpl(v).SetViewState(UiState::Pressed, true);
    }
  });

  view.StateChangedSignal().Connect(&tracker, [&](View, const StateEvent& e) {
    log.push_back({"signal-2", e.GetPrev(), e.GetCurrent()});
  });

  GetImpl(view).SetViewState(UiState::Focused, true);

  // signal-1(A→B) → signal-2(A→B) → signal-1(B→C) → signal-2(B→C)
  DALI_TEST_EQUALS(static_cast<int>(log.size()), 4, TEST_LOCATION);

  DALI_TEST_EQUALS(log[0].tag, std::string("signal-1"), TEST_LOCATION);
  DALI_TEST_CHECK(log[0].cur.Contains(UiState::Focused));
  DALI_TEST_CHECK(!log[0].cur.Contains(UiState::Pressed));

  DALI_TEST_EQUALS(log[1].tag, std::string("signal-2"), TEST_LOCATION);
  DALI_TEST_CHECK(log[1].cur.Contains(UiState::Focused));
  DALI_TEST_CHECK(!log[1].cur.Contains(UiState::Pressed));

  DALI_TEST_EQUALS(log[2].tag, std::string("signal-1"), TEST_LOCATION);
  DALI_TEST_CHECK(log[2].cur.Contains(UiState::Pressed));

  DALI_TEST_EQUALS(log[3].tag, std::string("signal-2"), TEST_LOCATION);
  DALI_TEST_CHECK(log[3].cur.Contains(UiState::Pressed));

  END_TEST;
}

// =============================================================================
// Orthogonal state: becoming Disabled immediately clears Focused
// =============================================================================

int UtcDaliViewStateDisabledClearsFocusedP(void)
{
  TestApplication application;
  View            view = CreateView(application);

  GetImpl(view).SetViewState(UiState::Focused, true);
  DALI_TEST_CHECK(GetImpl(view).GetState().Contains(UiState::Focused));

  GetImpl(view).SetViewState(UiState::Disabled, true);

  DALI_TEST_CHECK(GetImpl(view).GetState().Contains(UiState::Disabled));
  DALI_TEST_CHECK(!GetImpl(view).GetState().Contains(UiState::Focused));

  END_TEST;
}

// =============================================================================
// Orthogonal state: becoming Disabled immediately clears Pressed
// =============================================================================

int UtcDaliViewStateDisabledClearsPressedP(void)
{
  TestApplication application;
  View            view = CreateView(application);

  GetImpl(view).SetViewState(UiState::Pressed, true);
  DALI_TEST_CHECK(GetImpl(view).GetState().Contains(UiState::Pressed));

  GetImpl(view).SetViewState(UiState::Disabled, true);

  DALI_TEST_CHECK(GetImpl(view).GetState().Contains(UiState::Disabled));
  DALI_TEST_CHECK(!GetImpl(view).GetState().Contains(UiState::Pressed));

  END_TEST;
}

// =============================================================================
// Orthogonal state: StateChangedSignal reflects the corrected (post-clear) state
// i.e. the signal shows Disabled without Focused, not the intermediate state.
// =============================================================================

int UtcDaliViewStateDisabledOrthogonalSignalP(void)
{
  TestApplication   application;
  View              view = CreateView(application);
  ConnectionTracker tracker;

  GetImpl(view).SetViewState(UiState::Focused, true);

  UiState signalPrev, signalCur;
  GetImpl(view).WhenStateChanged("observer", &tracker, [&](View, const StateEvent& e) {
    signalPrev = e.GetPrev();
    signalCur  = e.GetCurrent();
  });

  GetImpl(view).SetViewState(UiState::Disabled, true);

  // prev should contain Focused (state before Disabled was applied)
  DALI_TEST_CHECK(signalPrev.Contains(UiState::Focused));
  // cur should contain Disabled but NOT Focused (cleared atomically)
  DALI_TEST_CHECK(signalCur.Contains(UiState::Disabled));
  DALI_TEST_CHECK(!signalCur.Contains(UiState::Focused));

  END_TEST;
}

// =============================================================================
// IsEffectivelyEnabled: own Disabled state → returns false
// =============================================================================

int UtcDaliViewIsEffectivelyEnabledSelfN(void)
{
  TestApplication application;
  View            view = CreateView(application);

  DALI_TEST_CHECK(view.IsEffectivelyEnabled());

  view.SetEnabled(false);

  DALI_TEST_CHECK(!view.IsEffectivelyEnabled());

  END_TEST;
}

// =============================================================================
// IsEffectivelyEnabled: parent disabled → child returns false
// =============================================================================

int UtcDaliViewIsEffectivelyEnabledAncestorN(void)
{
  TestApplication application;
  View            parent = CreateView(application);
  View            child  = CreateChildView(application, parent);

  DALI_TEST_CHECK(child.IsEffectivelyEnabled());

  parent.SetEnabled(false);

  DALI_TEST_CHECK(!child.IsEffectivelyEnabled());
  DALI_TEST_CHECK(child.IsEnabled()); // own state unchanged

  END_TEST;
}

// =============================================================================
// IsEffectivelyEnabled: both parent and self enabled → returns true
// =============================================================================

int UtcDaliViewIsEffectivelyEnabledAllEnabledP(void)
{
  TestApplication application;
  View            parent = CreateView(application);
  View            child  = CreateChildView(application, parent);

  DALI_TEST_CHECK(parent.IsEffectivelyEnabled());
  DALI_TEST_CHECK(child.IsEffectivelyEnabled());

  END_TEST;
}

// =============================================================================
// IsEffectivelyEnabled: grandparent disabled → grandchild returns false
// =============================================================================

int UtcDaliViewIsEffectivelyEnabledGrandAncestorN(void)
{
  TestApplication application;
  View            grandparent = CreateView(application);
  View            parent      = CreateChildView(application, grandparent);
  View            child       = CreateChildView(application, parent);

  grandparent.SetEnabled(false);

  DALI_TEST_CHECK(!child.IsEffectivelyEnabled());

  END_TEST;
}

// =============================================================================
// IsEffectivelyFocused: own Focused state → returns true
// =============================================================================

int UtcDaliViewIsEffectivelyFocusedSelfP(void)
{
  TestApplication application;
  View            view = CreateView(application);

  DALI_TEST_CHECK(!view.IsEffectivelyFocused());

  GetImpl(view).SetViewState(UiState::Focused, true);

  DALI_TEST_CHECK(view.IsEffectivelyFocused());

  END_TEST;
}

// =============================================================================
// IsEffectivelyFocused: parent focused → child returns true
// =============================================================================

int UtcDaliViewIsEffectivelyFocusedAncestorP(void)
{
  TestApplication application;
  View            parent = CreateView(application);
  View            child  = CreateChildView(application, parent);

  DALI_TEST_CHECK(!child.IsEffectivelyFocused());

  GetImpl(parent).SetViewState(UiState::Focused, true);

  DALI_TEST_CHECK(child.IsEffectivelyFocused());
  DALI_TEST_CHECK(!GetImpl(child).GetState().Contains(UiState::Focused)); // own state unchanged

  END_TEST;
}

// =============================================================================
// IsEffectivelyFocused: neither self nor any ancestor focused → returns false
// =============================================================================

int UtcDaliViewIsEffectivelyFocusedNoneN(void)
{
  TestApplication application;
  View            parent = CreateView(application);
  View            child  = CreateChildView(application, parent);

  DALI_TEST_CHECK(!parent.IsEffectivelyFocused());
  DALI_TEST_CHECK(!child.IsEffectivelyFocused());

  END_TEST;
}
