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
#include <dali-ui-test-suite-utils.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/integration-api/view-integration.h>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Integration;

namespace
{

// A record of one StateChangedSignal (or WhenStateChanged) callback invocation.
struct CallRecord
{
  std::string tag;    ///< Which handler was called (for ordering verification)
  ViewState     prev;
  ViewState     cur;
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

View CreateChildView(UiTestApplication& application, View& parent)
{
  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
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
  UiTestApplication   application;
  View              view = CreateView(application);
  ConnectionTracker tracker;

  ViewState receivedPrev, receivedCur;
  int     callCount = 0;

  IntegrationView::WhenStateChanged(GetImpl(view), "observer", &tracker, [&](View, const StateEvent& e) {
    ++callCount;
    receivedPrev = e.GetPrev();
    receivedCur  = e.GetCurrent();
  });

  IntegrationView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  DALI_TEST_EQUALS(callCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!receivedPrev.Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(receivedCur.Contains(ViewState::FOCUSED));

  END_TEST;
}

// =============================================================================
// Basic: No dispatch when state does not actually change
// =============================================================================

int UtcDaliViewStateNoDispatchUnchangedN(void)
{
  UiTestApplication   application;
  View              view = CreateView(application);
  ConnectionTracker tracker;
  int               callCount = 0;

  IntegrationView::WhenStateChanged(GetImpl(view), "observer", &tracker, [&](View, const StateEvent&) {
    ++callCount;
  });

  IntegrationView::SetState(GetImpl(view), ViewState::FOCUSED, true);
  DALI_TEST_EQUALS(callCount, 1, TEST_LOCATION);

  // Setting the same state again must not dispatch
  IntegrationView::SetState(GetImpl(view), ViewState::FOCUSED, true);
  DALI_TEST_EQUALS(callCount, 1, TEST_LOCATION);

  END_TEST;
}

// =============================================================================
// Deferred notification: handler triggering a new state change must NOT
// interrupt the remaining handlers of the current notification.
//
// Scenario (3 handlers registered: "h1", "h2", "h3"):
//   - Initial state A (Normal)
//   - SetState(Focused, true)  →  A→B (B = Focused)
//     - h1 receives (A, B)  ← ok
//     - h2 receives (A, B), then calls SetState(Pressed, true)  →  B→C
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
  UiTestApplication   application;
  View              view = CreateView(application);
  ConnectionTracker tracker;

  std::vector<CallRecord> log;

  IntegrationView::WhenStateChanged(GetImpl(view), "h1", &tracker, [&](View, const StateEvent& e) {
    log.push_back({"h1", e.GetPrev(), e.GetCurrent()});
  });

  IntegrationView::WhenStateChanged(GetImpl(view), "h2", &tracker, [&](View v, const StateEvent& e) {
    log.push_back({"h2", e.GetPrev(), e.GetCurrent()});
    // Trigger a second state change from inside the handler
    if(e.Added(ViewState::FOCUSED))
    {
      IntegrationView::SetState(GetImpl(v), ViewState::PRESSED, true);
    }
  });

  IntegrationView::WhenStateChanged(GetImpl(view), "h3", &tracker, [&](View, const StateEvent& e) {
    log.push_back({"h3", e.GetPrev(), e.GetCurrent()});
  });

  const ViewState stateA = ViewState::NORMAL;
  const ViewState stateB = ViewState::FOCUSED;
  const ViewState stateC = ViewState::FOCUSED + ViewState::PRESSED;

  IntegrationView::SetState(GetImpl(view), ViewState::FOCUSED, true);

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
  UiTestApplication   application;
  View              view = CreateView(application);
  ConnectionTracker tracker;

  std::vector<CallRecord> log;

  // Signal connection that triggers a re-entrant state change
  view.StateChangedSignal().Connect(&tracker, [&](View v, const StateEvent& e) {
    log.push_back({"signal-1", e.GetPrev(), e.GetCurrent()});
    if(e.Added(ViewState::FOCUSED))
    {
      IntegrationView::SetState(GetImpl(v), ViewState::PRESSED, true);
    }
  });

  view.StateChangedSignal().Connect(&tracker, [&](View, const StateEvent& e) {
    log.push_back({"signal-2", e.GetPrev(), e.GetCurrent()});
  });

  IntegrationView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  // signal-1(A→B) → signal-2(A→B) → signal-1(B→C) → signal-2(B→C)
  DALI_TEST_EQUALS(static_cast<int>(log.size()), 4, TEST_LOCATION);

  DALI_TEST_EQUALS(log[0].tag, std::string("signal-1"), TEST_LOCATION);
  DALI_TEST_CHECK(log[0].cur.Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(!log[0].cur.Contains(ViewState::PRESSED));

  DALI_TEST_EQUALS(log[1].tag, std::string("signal-2"), TEST_LOCATION);
  DALI_TEST_CHECK(log[1].cur.Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(!log[1].cur.Contains(ViewState::PRESSED));

  DALI_TEST_EQUALS(log[2].tag, std::string("signal-1"), TEST_LOCATION);
  DALI_TEST_CHECK(log[2].cur.Contains(ViewState::PRESSED));

  DALI_TEST_EQUALS(log[3].tag, std::string("signal-2"), TEST_LOCATION);
  DALI_TEST_CHECK(log[3].cur.Contains(ViewState::PRESSED));

  END_TEST;
}

// =============================================================================
// Orthogonal state: SetEnabled(false) clears Focused (via FocusManager)
// =============================================================================

int UtcDaliViewStateDisabledClearsFocusedP(void)
{
  UiTestApplication application;
  View            view = CreateView(application);

  view.SetFocusable(true);
  FocusManager::Get().SetCurrentFocusView(view);
  DALI_TEST_CHECK(GetImpl(view).GetState().Contains(ViewState::FOCUSED));

  view.SetEnabled(false);

  DALI_TEST_CHECK(GetImpl(view).GetState().Contains(ViewState::DISABLED));
  DALI_TEST_CHECK(!GetImpl(view).GetState().Contains(ViewState::FOCUSED));

  END_TEST;
}

// =============================================================================
// Orthogonal state: becoming Disabled immediately clears Pressed
// =============================================================================

int UtcDaliViewStateDisabledClearsPressedP(void)
{
  UiTestApplication application;
  View            view = CreateView(application);

  IntegrationView::SetState(GetImpl(view), ViewState::PRESSED, true);
  DALI_TEST_CHECK(GetImpl(view).GetState().Contains(ViewState::PRESSED));

  IntegrationView::SetState(GetImpl(view), ViewState::DISABLED, true);

  DALI_TEST_CHECK(GetImpl(view).GetState().Contains(ViewState::DISABLED));
  DALI_TEST_CHECK(!GetImpl(view).GetState().Contains(ViewState::PRESSED));

  END_TEST;
}

// =============================================================================
// Orthogonal state: SetEnabled(false) results in a state with Disabled
// but without Focused or Pressed.
// =============================================================================

int UtcDaliViewStateDisabledOrthogonalSignalP(void)
{
  UiTestApplication   application;
  View              view = CreateView(application);
  ConnectionTracker tracker;

  view.SetFocusable(true);
  FocusManager::Get().SetCurrentFocusView(view);
  DALI_TEST_CHECK(GetImpl(view).GetState().Contains(ViewState::FOCUSED));

  view.SetEnabled(false);

  // Final state: Disabled only — Focused and Pressed must not be present
  DALI_TEST_CHECK(GetImpl(view).GetState().Contains(ViewState::DISABLED));
  DALI_TEST_CHECK(!GetImpl(view).GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(!GetImpl(view).GetState().Contains(ViewState::PRESSED));

  END_TEST;
}

// =============================================================================
// Orthogonal state: PRESSED is cleared atomically with DISABLED — single event
// =============================================================================

int UtcDaliViewStateDisabledClearsPressedSingleEventP(void)
{
  UiTestApplication   application;
  View              view = CreateView(application);
  ConnectionTracker tracker;

  IntegrationView::SetState(GetImpl(view), ViewState::PRESSED, true);

  std::vector<CallRecord> log;
  IntegrationView::WhenStateChanged(GetImpl(view), "observer", &tracker, [&](View, const StateEvent& e) {
    log.push_back({"observer", e.GetPrev(), e.GetCurrent()});
  });

  IntegrationView::SetState(GetImpl(view), ViewState::DISABLED, true);

  // Exactly one notification: [Pressed] -> [Disabled]
  DALI_TEST_EQUALS(static_cast<int>(log.size()), 1, TEST_LOCATION);
  DALI_TEST_CHECK(log[0].prev.Contains(ViewState::PRESSED));
  DALI_TEST_CHECK(!log[0].prev.Contains(ViewState::DISABLED));
  DALI_TEST_CHECK(log[0].cur.Contains(ViewState::DISABLED));
  DALI_TEST_CHECK(!log[0].cur.Contains(ViewState::PRESSED));

  END_TEST;
}

// =============================================================================
// Orthogonal state: SetEnabled(false) clears both FOCUSED and PRESSED
// =============================================================================

int UtcDaliViewStateDisabledClearsFocusedAndPressedP(void)
{
  UiTestApplication   application;
  View              view = CreateView(application);
  ConnectionTracker tracker;

  view.EnsureInteractiveTrait();
  view.SetFocusable(true);
  FocusManager::Get().SetCurrentFocusView(view);
  IntegrationView::SetState(GetImpl(view), ViewState::PRESSED, true);

  DALI_TEST_CHECK(GetImpl(view).GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(GetImpl(view).GetState().Contains(ViewState::PRESSED));

  view.SetEnabled(false);

  // Final state: Disabled only
  DALI_TEST_CHECK(GetImpl(view).GetState().Contains(ViewState::DISABLED));
  DALI_TEST_CHECK(!GetImpl(view).GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(!GetImpl(view).GetState().Contains(ViewState::PRESSED));

  END_TEST;
}

// =============================================================================
// Orthogonal state: InteractiveTrait.IsPressed() is false after disabling
// =============================================================================

int UtcDaliViewStateDisabledClearsInteractiveTraitPressedP(void)
{
  UiTestApplication   application;
  View              view = CreateView(application);
  ConnectionTracker tracker;

  InteractiveTrait trait = view.EnsureInteractiveTrait();
  IntegrationView::SetState(GetImpl(view), ViewState::PRESSED, true);

  std::vector<CallRecord> log;
  view.StateChangedSignal().Connect(&tracker, [&](View, const StateEvent& e) {
    log.push_back({"signal", e.GetPrev(), e.GetCurrent()});
  });

  view.SetEnabled(false);

  // PRESSED must be cleared in the DISABLED transition, not in a follow-up event
  DALI_TEST_EQUALS(static_cast<int>(log.size()), 1, TEST_LOCATION);
  DALI_TEST_CHECK(log[0].cur.Contains(ViewState::DISABLED));
  DALI_TEST_CHECK(!log[0].cur.Contains(ViewState::PRESSED));

  // InteractiveTrait internal state must also be cleaned up
  DALI_TEST_CHECK(!trait.IsPressed());

  END_TEST;
}

// =============================================================================
// Orthogonal state: PseudoDisabled clears PRESSED but keeps FOCUSED
// =============================================================================

int UtcDaliViewStatePseudoDisabledClearsPressedKeepsFocusedP(void)
{
  UiTestApplication   application;
  View              view = CreateView(application);
  ConnectionTracker tracker;

  InteractiveTrait trait = view.EnsureInteractiveTrait();
  IntegrationView::SetState(GetImpl(view), ViewState::FOCUSED, true);
  IntegrationView::SetState(GetImpl(view), ViewState::PRESSED, true);

  std::vector<CallRecord> log;
  view.StateChangedSignal().Connect(&tracker, [&](View, const StateEvent& e) {
    log.push_back({"signal", e.GetPrev(), e.GetCurrent()});
  });

  trait.SetPseudoDisabled(true);

  const ViewState& finalState = GetImpl(view).GetState();

  // PRESSED must be cleared
  DALI_TEST_CHECK(!finalState.Contains(ViewState::PRESSED));
  DALI_TEST_CHECK(!trait.IsPressed());

  // FOCUSED must be kept
  DALI_TEST_CHECK(finalState.Contains(ViewState::FOCUSED));

  // PSEUDO_DISABLED must be set
  DALI_TEST_CHECK(finalState.Contains(ViewState::PSEUDO_DISABLED));

  END_TEST;
}

// =============================================================================
// Orthogonal state: PseudoDisabled does NOT clear FOCUSED (unlike DISABLED)
// =============================================================================

int UtcDaliViewStatePseudoDisabledKeepsFocusedN(void)
{
  UiTestApplication application;
  View            view = CreateView(application);

  view.EnsureInteractiveTrait();
  IntegrationView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  InteractiveTrait trait = view.EnsureInteractiveTrait();
  trait.SetPseudoDisabled(true);

  // FOCUSED must still be present
  DALI_TEST_CHECK(GetImpl(view).GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(GetImpl(view).GetState().Contains(ViewState::PSEUDO_DISABLED));

  END_TEST;
}

// =============================================================================
// IsEffectivelyEnabled: own Disabled state → returns false
// =============================================================================

int UtcDaliViewIsEffectivelyEnabledSelfN(void)
{
  UiTestApplication application;
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
  UiTestApplication application;
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
  UiTestApplication application;
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
  UiTestApplication application;
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
  UiTestApplication application;
  View            view = CreateView(application);

  DALI_TEST_CHECK(!view.IsEffectivelyFocused());

  IntegrationView::SetState(GetImpl(view), ViewState::FOCUSED, true);

  DALI_TEST_CHECK(view.IsEffectivelyFocused());

  END_TEST;
}

// =============================================================================
// IsEffectivelyFocused: parent focused → child returns true
// =============================================================================

int UtcDaliViewIsEffectivelyFocusedAncestorP(void)
{
  UiTestApplication application;
  View            parent = CreateView(application);
  View            child  = CreateChildView(application, parent);

  DALI_TEST_CHECK(!child.IsEffectivelyFocused());

  IntegrationView::SetState(GetImpl(parent), ViewState::FOCUSED, true);

  DALI_TEST_CHECK(child.IsEffectivelyFocused());
  DALI_TEST_CHECK(!GetImpl(child).GetState().Contains(ViewState::FOCUSED)); // own state unchanged

  END_TEST;
}

// =============================================================================
// IsEffectivelyFocused: neither self nor any ancestor focused → returns false
// =============================================================================

int UtcDaliViewIsEffectivelyFocusedNoneN(void)
{
  UiTestApplication application;
  View            parent = CreateView(application);
  View            child  = CreateChildView(application, parent);

  DALI_TEST_CHECK(!parent.IsEffectivelyFocused());
  DALI_TEST_CHECK(!child.IsEffectivelyFocused());

  END_TEST;
}

// =============================================================================
// FocusManager integration: SetCurrentFocusView sets FOCUSED state
// =============================================================================

int UtcDaliViewStateFocusedViaFocusManagerP(void)
{
  UiTestApplication application;
  View              view = CreateView(application);
  ConnectionTracker tracker;

  ViewState receivedCur;
  int     callCount = 0;
  IntegrationView::WhenStateChanged(GetImpl(view), "observer", &tracker, [&](View, const StateEvent& e) {
    ++callCount;
    receivedCur = e.GetCurrent();
  });

  view.SetFocusable(true);
  FocusManager::Get().SetCurrentFocusView(view);

  DALI_TEST_CHECK(GetImpl(view).GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(view.IsEffectivelyFocused());
  DALI_TEST_EQUALS(callCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(receivedCur.Contains(ViewState::FOCUSED));

  END_TEST;
}

// =============================================================================
// FocusManager integration: moving focus clears FOCUSED on old view
// =============================================================================

int UtcDaliViewStateFocusedViaFocusManagerClearOnMoveP(void)
{
  UiTestApplication application;
  View              view1 = CreateView(application);
  View              view2 = CreateView(application);

  view1.SetFocusable(true);
  view2.SetFocusable(true);
  FocusManager::Get().SetCurrentFocusView(view1);
  DALI_TEST_CHECK(GetImpl(view1).GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(!GetImpl(view2).GetState().Contains(ViewState::FOCUSED));

  FocusManager::Get().SetCurrentFocusView(view2);

  DALI_TEST_CHECK(!GetImpl(view1).GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(GetImpl(view2).GetState().Contains(ViewState::FOCUSED));

  END_TEST;
}
