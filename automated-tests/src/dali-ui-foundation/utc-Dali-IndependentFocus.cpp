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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/extension-api/focus-manager.h>
#include <dali-ui-foundation/extension-api/ui-config-impl.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-adaptor-impl.h>
#include <dali.h>
#include <dali/integration-api/adaptor-framework/focused-actor-provider.h>
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/touch-integ.h>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;
namespace KeyFocus = Dali::Ui::Extension::FocusManager;

namespace
{
struct InitialNavigation
{
  FocusNavigationResult Navigate(View current, FocusNavigationContext)
  {
    ++calls;
    currentWasEmpty = !current;
    return FocusNavigationResult::MoveTo(target);
  }
  View target;
  int  calls{0};
  bool currentWasEmpty{false};
};

WeakHandle<View> gHiddenByPolicyTarget;

bool HideFocusCandidate(const Extension::FocusIndicationContext& context)
{
  if(context.focusedView == gHiddenByPolicyTarget.GetHandle())
  {
    context.focusedView.GetParent().SetProperty(Actor::Property::VISIBLE, false);
  }
  return context.proposedIndicated;
}

struct FocusRecorder : ConnectionTracker
{
  void OnView(View, bool focused)
  {
    focused ? ++gained : ++lost;
  }
  void OnGlobal(View, View)
  {
    ++globalChanges;
  }
  void OnWindow(Window window, View previous, View current)
  {
    windows.push_back(window);
    oldViews.push_back(previous);
    newViews.push_back(current);
  }
  int                 gained{0};
  int                 lost{0};
  int                 globalChanges{0};
  std::vector<Window> windows;
  std::vector<View>   oldViews;
  std::vector<View>   newViews;
};

struct KeyRecorder : ConnectionTracker
{
  bool OnKey(View view, KeyEvent)
  {
    views.push_back(view);
    return consume;
  }
  std::vector<View> views;
  bool              consume{false};
};

struct ReplaceOnLoss : ConnectionTracker
{
  void OnChanged(View, bool focused)
  {
    if(!focused && !done)
    {
      done = true;
      FocusManager::Get().SetCurrentFocusView(target);
    }
  }
  View target;
  bool done{false};
};

struct StateRecorder : ConnectionTracker
{
  void OnState(View, StateEvent event)
  {
    if(event.Changed(ViewState::FOCUSED))
    {
      events.push_back(event);
    }
  }
  std::vector<StateEvent> events;
};

struct ReparentOnKey : ConnectionTracker
{
  bool OnKey(View view, KeyEvent)
  {
    view.Unparent();
    destination.Add(view);
    return false;
  }
  View destination;
};

struct FocusScenario
{
  FocusScenario(UiConfig config = UiConfig::New())
  : application(config),
    main(application.GetWindow()),
    sub(Window::New(PositionSize(0, 0, 480, 800), "independent-focus")),
    manager(FocusManager::Get())
  {
    // The test Adaptor creates additional Windows inactive. Native
    // SetAcceptFocus and compositor behavior are exercised by the sample.
    Dali::Internal::Adaptor::Adaptor::GetScene(main).SetNativeId(101);
    Dali::Internal::Adaptor::Adaptor::GetScene(sub).SetNativeId(202);
    sub.Show();
    mainRoot = View::New();
    subRoot  = View::New();
    main.Add(mainRoot);
    sub.Add(subRoot);
    mainView = Add(mainRoot);
    subA     = Add(subRoot);
    subB     = Add(subRoot);
    subA.SetProperty(View::Property::RIGHT_FOCUSABLE_VIEW_ID, subB.GetProperty<int>(Actor::Property::ID));
    subB.SetProperty(View::Property::LEFT_FOCUSABLE_VIEW_ID, subA.GetProperty<int>(Actor::Property::ID));
    manager.SetCurrentFocusView(mainView);
  }

  View Add(View parent)
  {
    View view = View::New();
    view.SetFocusable(true);
    parent.Add(view);
    return view;
  }

  bool Enable()
  {
    return manager.SetIndependentFocusEnabled(sub, true) && manager.SetCurrentFocusView(subA);
  }

  void Send(Window window, const char* key, uint32_t id)
  {
    auto                        scene = Dali::Internal::Adaptor::Adaptor::GetScene(window);
    Dali::Integration::KeyEvent event(key, "", "", 0, 0, 100u, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::KEYBOARD, Device::Subclass::NONE);
    event.windowId = id;
    scene.QueueEvent(event);
    scene.ProcessEvents();
  }

  void Send(Window window, const char* key)
  {
    Send(window, key, static_cast<uint32_t>(window.GetNativeId()));
  }

  void Touch(View hit, PointState::Type state)
  {
    Dali::Integration::Point point;
    point.SetDeviceId(1);
    point.SetState(state);
    point.SetDeviceClass(Device::Class::TOUCH);
    point.SetHitActor(hit);
    TouchEvent event = Dali::Integration::NewTouchEvent(100u, point);
    auto       scene = Dali::Integration::SceneHolder::Get(hit);
    scene.TouchEventSignal().Emit(scene, event);
  }

  UiTestApplication application;
  Window            main;
  Window            sub;
  FocusManager      manager;
  View              mainRoot;
  View              subRoot;
  View              mainView;
  View              subA;
  View              subB;
};
} // namespace

int UtcDaliIndependentFocusPreservesMainAndSignalsP(void)
{
  FocusScenario s;
  FocusRecorder mainRecorder;
  FocusRecorder subRecorder;
  s.mainView.FocusChangedSignal().Connect(&mainRecorder, &FocusRecorder::OnView);
  s.subA.FocusChangedSignal().Connect(&subRecorder, &FocusRecorder::OnView);
  s.manager.FocusChangedSignal().Connect(&mainRecorder, &FocusRecorder::OnGlobal);
  s.manager.WindowFocusChangedSignal().Connect(&subRecorder, &FocusRecorder::OnWindow);
  DALI_TEST_CHECK(s.Enable());
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.mainView);
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.main) == s.mainView);
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subA);
  DALI_TEST_CHECK(s.mainView.GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(s.subA.GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(KeyFocus::IsKeyInputTarget(s.mainView));
  DALI_TEST_CHECK(KeyFocus::IsKeyInputTarget(s.subA));
  DALI_TEST_CHECK(s.main.IsFocused() && !s.sub.IsFocused());
  DALI_TEST_CHECK(Dali::Integration::GetFocusedActorProvider()->GetFocusedActor() == s.mainView);
  DALI_TEST_EQUALS(mainRecorder.lost, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(mainRecorder.globalChanges, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(subRecorder.gained, 1, TEST_LOCATION);
  DALI_TEST_CHECK(subRecorder.windows.size() == 1u && subRecorder.windows[0] == s.sub);
  DALI_TEST_CHECK(!subRecorder.oldViews[0] && subRecorder.newViews[0] == s.subA);
  DALI_TEST_CHECK(s.manager.SetCurrentFocusView(s.subA));
  DALI_TEST_EQUALS(subRecorder.gained, 1, TEST_LOCATION);
  DALI_TEST_CHECK(subRecorder.windows.size() == 1u);
  END_TEST;
}

int UtcDaliIndependentFocusEnableDoesNotApplyStoredP(void)
{
  FocusScenario s;
  DALI_TEST_CHECK(s.manager.SetCurrentFocusView(s.subA));
  DALI_TEST_CHECK(!s.manager.GetCurrentFocusView(s.sub));
  DALI_TEST_CHECK(s.manager.SetIndependentFocusEnabled(s.sub, true));
  DALI_TEST_CHECK(!s.manager.GetCurrentFocusView(s.sub));
  DALI_TEST_CHECK(!s.subA.GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.mainView);
  END_TEST;
}

int UtcDaliIndependentFocusRoutesByDeliveryWindowP(void)
{
  FocusScenario s;
  DALI_TEST_CHECK(s.Enable());
  KeyRecorder recorder;
  s.mainView.KeyEventSignal().Connect(&recorder, &KeyRecorder::OnKey);
  s.subA.KeyEventSignal().Connect(&recorder, &KeyRecorder::OnKey);
  s.subRoot.KeyEventSignal().Connect(&recorder, &KeyRecorder::OnKey);
  s.Send(s.sub, "a");
  DALI_TEST_CHECK(recorder.views.size() == 2u && recorder.views[0] == s.subA && recorder.views[1] == s.subRoot);
  recorder.views.clear();
  s.Send(s.main, "a");
  DALI_TEST_CHECK(recorder.views.size() == 1u && recorder.views[0] == s.mainView);
  recorder.views.clear();
  s.Send(s.sub, "a", static_cast<uint32_t>(s.main.GetNativeId()));
  DALI_TEST_CHECK(recorder.views.empty());
  s.Send(s.sub, "a", 0u);
  DALI_TEST_CHECK(recorder.views.size() == 2u && recorder.views[0] == s.subA);
  recorder.views.clear();
  recorder.consume = true;
  s.Send(s.sub, "a");
  DALI_TEST_CHECK(recorder.views.size() == 1u && recorder.views[0] == s.subA);
  END_TEST;
}

int UtcDaliIndependentFocusNavigationAndHistoryAreLocalP(void)
{
  FocusScenario s;
  DALI_TEST_CHECK(s.Enable());
  s.Send(s.sub, "Right");
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subB);
  DALI_TEST_CHECK(s.subB.GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(!s.subA.GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(s.mainView.GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.mainView);
  s.manager.MoveFocusBackward(s.sub);
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subA);
  DALI_TEST_CHECK(s.manager.MoveFocus(s.sub, FocusDirection::RIGHT));
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subB);
  KeyRecorder recorder;
  recorder.consume = true;
  s.subB.KeyEventSignal().Connect(&recorder, &KeyRecorder::OnKey);
  s.Send(s.sub, "Left");
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subB);
  END_TEST;
}

int UtcDaliIndependentFocusOrdinaryInactiveBackwardStoresEachStepP(void)
{
  FocusScenario s;
  for(bool clearOnLoss : {true, false})
  {
    s.manager.SetClearFocusOnWindowFocusLost(clearOnLoss);
    View first  = s.Add(s.mainRoot);
    View second = s.Add(s.mainRoot);
    View third  = s.Add(s.mainRoot);
    DALI_TEST_CHECK(s.manager.SetCurrentFocusView(first));
    DALI_TEST_CHECK(s.manager.SetCurrentFocusView(second));
    DALI_TEST_CHECK(s.manager.SetCurrentFocusView(third));
    s.main.Lower();
    View actual = s.manager.GetCurrentFocusView(s.main);
    DALI_TEST_CHECK(actual == (clearOnLoss ? View() : third));
    FocusRecorder recorder;
    s.manager.FocusChangedSignal().Connect(&recorder, &FocusRecorder::OnGlobal);
    s.manager.WindowFocusChangedSignal().Connect(&recorder, &FocusRecorder::OnWindow);
    s.manager.MoveFocusBackward(s.main);
    DALI_TEST_CHECK(!s.main.IsFocused());
    DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.main) == actual);
    s.main.Raise();
    DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.main) == second);
    s.main.Lower();
    recorder.globalChanges = 0;
    recorder.windows.clear();
    s.manager.MoveFocusBackward(s.main);
    s.manager.MoveFocusBackward(s.main);
    DALI_TEST_CHECK(!s.main.IsFocused());
    DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.main) == (clearOnLoss ? View() : second));
    DALI_TEST_EQUALS(recorder.globalChanges, 0, TEST_LOCATION);
    DALI_TEST_CHECK(recorder.windows.empty());
    s.main.Raise();
    DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.main) == s.mainView);
  }
  END_TEST;
}

int UtcDaliIndependentFocusBackwardRestoresLatestAfterClearP(void)
{
  FocusScenario s;
  for(bool independent : {false, true})
  {
    DALI_TEST_CHECK(s.manager.SetCurrentFocusView(s.mainView));
    Window window = independent ? s.sub : s.main;
    View   root   = independent ? s.subRoot : s.mainRoot;
    if(independent)
    {
      DALI_TEST_CHECK(s.manager.SetIndependentFocusEnabled(window, true));
    }
    View first  = s.Add(root);
    View second = s.Add(root);
    DALI_TEST_CHECK(s.manager.SetCurrentFocusView(first));
    DALI_TEST_CHECK(s.manager.SetCurrentFocusView(second));
    s.manager.ClearFocus(window);
    DALI_TEST_CHECK(!s.manager.GetCurrentFocusView(window));
    s.manager.MoveFocusBackward(window);
    DALI_TEST_CHECK(s.manager.GetCurrentFocusView(window) == second);
    s.manager.MoveFocusBackward(window);
    DALI_TEST_CHECK(s.manager.GetCurrentFocusView(window) == first);
    if(independent)
    {
      DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.mainView);
      DALI_TEST_CHECK(!s.sub.IsFocused());
    }
  }
  END_TEST;
}

int UtcDaliIndependentFocusInactiveClearRestoresLatestHistoryP(void)
{
  FocusScenario s;
  s.manager.SetClearFocusOnWindowFocusLost(true);
  View first  = s.Add(s.mainRoot);
  View second = s.Add(s.mainRoot);
  DALI_TEST_CHECK(s.manager.SetCurrentFocusView(first));
  DALI_TEST_CHECK(s.manager.SetCurrentFocusView(second));
  s.main.Lower();
  s.manager.ClearFocus(s.main);
  s.manager.MoveFocusBackward(s.main);
  DALI_TEST_CHECK(!s.main.IsFocused());
  DALI_TEST_CHECK(!s.manager.GetCurrentFocusView(s.main));
  s.main.Raise();
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.main) == second);
  s.main.Lower();
  s.manager.MoveFocusBackward(s.main);
  s.main.Raise();
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.main) == first);
  END_TEST;
}

int UtcDaliIndependentFocusBackwardRestoresSingleEntryP(void)
{
  FocusScenario s;
  for(bool independent : {false, true})
  {
    s.sub.Lower();
    s.main.Raise();
    if(independent)
    {
      DALI_TEST_CHECK(s.Enable());
    }
    else
    {
      s.main.Lower();
      s.sub.Raise();
      DALI_TEST_CHECK(s.manager.SetCurrentFocusView(s.subA));
    }
    s.manager.ClearFocus(s.sub);
    DALI_TEST_CHECK(!s.manager.GetCurrentFocusView(s.sub));
    s.manager.MoveFocusBackward(s.sub);
    DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subA);
    s.manager.MoveFocusBackward(s.sub);
    DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subA);
    if(independent)
    {
      DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.mainView);
    }
  }
  END_TEST;
}

int UtcDaliIndependentFocusBackwardSkipsInvalidHistoryP(void)
{
  FocusScenario s;
  for(bool independent : {false, true})
  {
    for(int invalidation : {0, 1, 2})
    {
      DALI_TEST_CHECK(s.manager.SetCurrentFocusView(s.mainView));
      Window window = independent ? s.sub : s.main;
      View   root   = independent ? s.subRoot : s.mainRoot;
      if(independent)
      {
        DALI_TEST_CHECK(s.manager.SetIndependentFocusEnabled(window, true));
      }
      View first  = s.Add(root);
      View second = s.Add(root);
      View third  = s.Add(root);
      DALI_TEST_CHECK(s.manager.SetCurrentFocusView(first));
      DALI_TEST_CHECK(s.manager.SetCurrentFocusView(second));
      DALI_TEST_CHECK(s.manager.SetCurrentFocusView(third));
      if(invalidation == 0)
      {
        second.SetEnabled(false);
      }
      else if(invalidation == 1)
      {
        second.SetFocusable(false);
      }
      else
      {
        second.Unparent();
      }
      s.manager.MoveFocusBackward(window);
      DALI_TEST_CHECK(s.manager.GetCurrentFocusView(window) == first);
      if(independent)
      {
        DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.mainView);
      }
    }
  }
  END_TEST;
}

int UtcDaliIndependentFocusBackwardPreservesOtherWindowHistoryP(void)
{
  FocusScenario s;
  View          mainSecond = s.Add(s.mainRoot);
  View          mainThird  = s.Add(s.mainRoot);
  DALI_TEST_CHECK(s.manager.SetCurrentFocusView(mainSecond));
  DALI_TEST_CHECK(s.manager.SetCurrentFocusView(mainThird));
  s.main.Lower();
  s.sub.Raise();
  DALI_TEST_CHECK(s.manager.SetCurrentFocusView(s.subA));
  DALI_TEST_CHECK(s.manager.SetCurrentFocusView(s.subB));
  s.sub.Lower();
  s.main.Raise();
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.main) == mainThird);
  s.manager.MoveFocusBackward(s.main);
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.main) == mainSecond);
  s.manager.MoveFocusBackward(s.sub);
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == mainSecond);
  DALI_TEST_CHECK(!s.manager.GetCurrentFocusView(s.sub));
  s.manager.MoveFocusBackward(s.main);
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.main) == s.mainView);
  s.main.Lower();
  s.sub.Raise();
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subA);
  END_TEST;
}

int UtcDaliIndependentFocusBackwardWithoutDestinationPreservesHistoryP(void)
{
  FocusScenario s;
  for(bool independent : {false, true})
  {
    Window window = independent ? s.sub : s.main;
    View   first  = independent ? s.subA : s.mainView;
    View   second = independent ? s.subB : s.Add(s.mainRoot);
    if(independent)
    {
      DALI_TEST_CHECK(s.Enable());
    }
    DALI_TEST_CHECK(s.manager.SetCurrentFocusView(second));
    first.SetFocusable(false);
    s.manager.MoveFocusBackward(window);
    DALI_TEST_CHECK(s.manager.GetCurrentFocusView(window) == second);
    s.manager.ClearFocus(window);
    s.manager.MoveFocusBackward(window);
    DALI_TEST_CHECK(s.manager.GetCurrentFocusView(window) == second);
    first.SetFocusable(true);
    s.manager.MoveFocusBackward(window);
    DALI_TEST_CHECK(s.manager.GetCurrentFocusView(window) == first);
  }
  END_TEST;
}

int UtcDaliIndependentFocusBackwardKeepsReentrantTargetP(void)
{
  FocusScenario s;
  DALI_TEST_CHECK(s.Enable());
  View third  = s.Add(s.subRoot);
  View latest = s.Add(s.subRoot);
  DALI_TEST_CHECK(s.manager.SetCurrentFocusView(s.subB));
  DALI_TEST_CHECK(s.manager.SetCurrentFocusView(third));
  ReplaceOnLoss callback;
  callback.target = latest;
  third.FocusChangedSignal().Connect(&callback, &ReplaceOnLoss::OnChanged);
  s.manager.MoveFocusBackward(s.sub);
  DALI_TEST_CHECK(callback.done);
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == latest);
  DALI_TEST_CHECK(KeyFocus::IsKeyInputTarget(latest));
  DALI_TEST_CHECK(!s.subB.GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.mainView);
  s.manager.MoveFocusBackward(s.sub);
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subB);
  END_TEST;
}

int UtcDaliIndependentFocusBackwardAllowsPolicyHiddenHistoryP(void)
{
  UiConfig config = UiConfig::New();
  GetImpl(config).SetFocusIndicationPolicy(&HideFocusCandidate);
  FocusScenario s(config);
  DALI_TEST_CHECK(s.Enable());
  View parent = View::New();
  s.subRoot.Add(parent);
  View second = s.Add(parent);
  View third  = s.Add(s.subRoot);
  DALI_TEST_CHECK(s.manager.SetCurrentFocusView(second));
  DALI_TEST_CHECK(s.manager.SetCurrentFocusView(third));
  gHiddenByPolicyTarget = second;
  s.manager.MoveFocusBackward(s.sub);
  gHiddenByPolicyTarget.Reset();
  DALI_TEST_CHECK(!second.IsEffectivelyVisible());
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == second);
  DALI_TEST_CHECK(KeyFocus::IsKeyInputTarget(second));
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.mainView);
  END_TEST;
}

int UtcDaliIndependentFocusHideAndDisableClearOnlySubP(void)
{
  FocusScenario s;
  DALI_TEST_CHECK(s.Enable());
  s.sub.Hide();
  DALI_TEST_CHECK(!s.manager.GetCurrentFocusView(s.sub));
  DALI_TEST_CHECK(!s.subA.GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.mainView);
  s.sub.Show();
  DALI_TEST_CHECK(!s.manager.GetCurrentFocusView(s.sub));
  DALI_TEST_CHECK(s.manager.SetCurrentFocusView(s.subA));
  DALI_TEST_CHECK(s.manager.SetIndependentFocusEnabled(s.sub, false));
  DALI_TEST_CHECK(!s.manager.IsIndependentFocusEnabled(s.sub));
  DALI_TEST_CHECK(!s.manager.GetCurrentFocusView(s.sub));
  DALI_TEST_CHECK(s.mainView.GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(s.manager.SetIndependentFocusEnabled(s.sub, true));
  DALI_TEST_CHECK(!s.manager.GetCurrentFocusView(s.sub));
  END_TEST;
}

int UtcDaliIndependentFocusSetBeforeWindowShowP(void)
{
  FocusScenario s;
  DALI_TEST_CHECK(s.Enable());
  s.sub.Hide();
  DALI_TEST_CHECK(!s.sub.IsVisible());
  FocusRecorder recorder;
  s.subA.FocusChangedSignal().Connect(&recorder, &FocusRecorder::OnView);
  DALI_TEST_CHECK(s.manager.SetCurrentFocusView(s.subA));
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subA);
  DALI_TEST_CHECK(KeyFocus::IsKeyInputTarget(s.subA));
  DALI_TEST_CHECK(s.subA.GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.mainView);
  s.sub.Show();
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subA);
  DALI_TEST_CHECK(KeyFocus::IsKeyInputTarget(s.subA));
  DALI_TEST_EQUALS(recorder.gained, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.lost, 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliIndependentFocusSetHiddenViewP(void)
{
  FocusScenario s;
  DALI_TEST_CHECK(s.Enable());
  s.subB.SetVisible(false);
  DALI_TEST_CHECK(s.manager.SetCurrentFocusView(s.subB));
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subB);
  DALI_TEST_CHECK(KeyFocus::IsKeyInputTarget(s.subB));
  DALI_TEST_CHECK(s.subB.GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.mainView);
  s.subB.SetVisible(true);
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subB);
  DALI_TEST_CHECK(KeyFocus::IsKeyInputTarget(s.subB));
  END_TEST;
}

int UtcDaliIndependentFocusInvalidationCancelsActualP(void)
{
  FocusScenario s;
  DALI_TEST_CHECK(s.Enable());
  s.subA.SetEnabled(false);
  DALI_TEST_CHECK(!s.manager.GetCurrentFocusView(s.sub));
  DALI_TEST_CHECK(!KeyFocus::IsKeyInputTarget(s.subA));
  s.subA.SetEnabled(true);
  DALI_TEST_CHECK(!s.manager.GetCurrentFocusView(s.sub));
  DALI_TEST_CHECK(s.manager.SetCurrentFocusView(s.subA));
  s.subRoot.Remove(s.subA);
  DALI_TEST_CHECK(!s.manager.GetCurrentFocusView(s.sub));
  DALI_TEST_CHECK(!s.subA.GetState().Contains(ViewState::FOCUSED));
  s.subRoot.Add(s.subA);
  DALI_TEST_CHECK(!s.manager.GetCurrentFocusView(s.sub));
  DALI_TEST_CHECK(s.manager.SetCurrentFocusView(s.subA));
  s.subA.SetVisible(false);
  DALI_TEST_CHECK(!s.manager.GetCurrentFocusView(s.sub));
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.mainView);
  END_TEST;
}

int UtcDaliIndependentFocusNativePromotionDoesNotRefocusP(void)
{
  FocusScenario s;
  for(bool visible : {true, false})
  {
    s.subA.SetVisible(visible);
    DALI_TEST_CHECK(s.Enable());
    DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.mainView);
    DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subA);
    DALI_TEST_CHECK(KeyFocus::IsKeyInputTarget(s.subA));

    FocusRecorder recorder;
    s.subA.FocusChangedSignal().Connect(&recorder, &FocusRecorder::OnView);
    s.manager.FocusChangedSignal().Connect(&recorder, &FocusRecorder::OnGlobal);
    s.sub.Raise();
    DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.subA);
    DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subA);
    DALI_TEST_CHECK(KeyFocus::IsKeyInputTarget(s.subA));
    DALI_TEST_CHECK(s.subA.GetState().Contains(ViewState::FOCUSED));
    DALI_TEST_CHECK(!s.mainView.GetState().Contains(ViewState::FOCUSED));
    DALI_TEST_EQUALS(recorder.globalChanges, 1, TEST_LOCATION);
    DALI_TEST_EQUALS(recorder.gained, 0, TEST_LOCATION);
    DALI_TEST_EQUALS(recorder.lost, 0, TEST_LOCATION);

    s.subA.SetVisible(true);
    DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.subA);
    DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subA);
    DALI_TEST_CHECK(KeyFocus::IsKeyInputTarget(s.subA));
    DALI_TEST_CHECK(s.subA.GetState().Contains(ViewState::FOCUSED));
    DALI_TEST_EQUALS(recorder.globalChanges, 1, TEST_LOCATION);
    DALI_TEST_EQUALS(recorder.gained, 0, TEST_LOCATION);
    DALI_TEST_EQUALS(recorder.lost, 0, TEST_LOCATION);

    s.main.Lower();
    DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.subA);
    s.sub.Lower();
    s.main.Raise();
    DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.mainView);
    DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subA);
    DALI_TEST_CHECK(KeyFocus::IsKeyInputTarget(s.subA));
    DALI_TEST_CHECK(s.mainView.GetState().Contains(ViewState::FOCUSED));
    DALI_TEST_CHECK(s.subA.GetState().Contains(ViewState::FOCUSED));
    DALI_TEST_EQUALS(recorder.globalChanges, 2, TEST_LOCATION);
    DALI_TEST_EQUALS(recorder.gained, 0, TEST_LOCATION);
    DALI_TEST_EQUALS(recorder.lost, 0, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliIndependentFocusClearHandlesSeparateKeyTargetP(void)
{
  FocusScenario s;
  DALI_TEST_CHECK(s.Enable());
  DALI_TEST_CHECK(KeyFocus::SetKeyInputTarget(s.subB));
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subA);
  DALI_TEST_CHECK(KeyFocus::IsKeyInputTarget(s.mainView));
  DALI_TEST_CHECK(KeyFocus::IsKeyInputTarget(s.subB));
  s.manager.ClearFocus(s.sub);
  DALI_TEST_CHECK(!s.manager.GetCurrentFocusView(s.sub));
  DALI_TEST_CHECK(!KeyFocus::IsKeyInputTarget(s.subB));
  DALI_TEST_CHECK(!s.subB.GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(s.mainView.GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(s.manager.IsIndependentFocusEnabled(s.sub));
  END_TEST;
}

int UtcDaliIndependentFocusReentrantLossKeepsNewestTargetP(void)
{
  FocusScenario s;
  DALI_TEST_CHECK(s.Enable());
  ReplaceOnLoss replacer;
  replacer.target = s.Add(s.subRoot);
  s.subA.FocusChangedSignal().Connect(&replacer, &ReplaceOnLoss::OnChanged);
  s.manager.SetCurrentFocusView(s.subB);
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == replacer.target);
  DALI_TEST_CHECK(KeyFocus::IsKeyInputTarget(replacer.target));
  DALI_TEST_CHECK(replacer.target.GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(!s.subB.GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(s.mainView.GetState().Contains(ViewState::FOCUSED));
  END_TEST;
}

int UtcDaliIndependentFocusRejectsEditableAndInvalidTargetsN(void)
{
  FocusScenario s;
  DALI_TEST_CHECK(!s.manager.SetIndependentFocusEnabled(Window(), true));
  DALI_TEST_CHECK(!s.manager.IsIndependentFocusEnabled(Window()));
  DALI_TEST_CHECK(!s.manager.GetCurrentFocusView(Window()));
  s.manager.ClearFocus(Window());
  DALI_TEST_CHECK(!s.manager.MoveFocus(Window(), FocusDirection::RIGHT));
  DALI_TEST_CHECK(s.Enable());
  InputField  field  = InputField::New();
  InputEditor editor = InputEditor::New();
  field.SetFocusable(true);
  editor.SetFocusable(true);
  s.subRoot.Add(field);
  s.subRoot.Add(editor);
  DALI_TEST_CHECK(!s.manager.SetCurrentFocusView(field));
  DALI_TEST_CHECK(!s.manager.SetCurrentFocusView(editor));
  DALI_TEST_CHECK(!KeyFocus::SetKeyInputTarget(field));
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subA);
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.mainView);
  END_TEST;
}

int UtcDaliIndependentFocusIndicatorsAndContextAreLocalP(void)
{
  FocusScenario s;
  DALI_TEST_CHECK(s.Enable());
  s.manager.SetDefaultFocusIndicatorEnabled(true);
  s.Send(s.main, "Left");
  s.Send(s.sub, "Left");
  DALI_TEST_CHECK(s.mainView.GetState().Contains(ViewState::FOCUS_INDICATED));
  DALI_TEST_CHECK(s.subA.GetState().Contains(ViewState::FOCUS_INDICATED));
  DALI_TEST_CHECK(s.mainView.GetChildCount() > 0u);
  DALI_TEST_CHECK(s.subA.GetChildCount() > 0u);
  DALI_TEST_CHECK(s.mainView.GetChildAt(0) != s.subA.GetChildAt(0));
  s.manager.ClearFocusIndication(s.sub);
  DALI_TEST_CHECK(!s.subA.GetState().Contains(ViewState::FOCUS_INDICATED));
  DALI_TEST_CHECK(s.mainView.GetState().Contains(ViewState::FOCUS_INDICATED));
  DALI_TEST_EQUALS(s.mainView.GetChildCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(s.subA.GetChildCount(), 0u, TEST_LOCATION);
  s.manager.SetDefaultFocusIndicatorEnabled(false);
  DALI_TEST_EQUALS(s.mainView.GetChildCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliIndependentFocusCauseDoesNotReplaceGlobalContextP(void)
{
  FocusScenario s;
  DALI_TEST_CHECK(s.Enable());
  StateRecorder recorder;
  s.subA.StateChangedSignal().Connect(&recorder, &StateRecorder::OnState);
  s.subB.StateChangedSignal().Connect(&recorder, &StateRecorder::OnState);
  s.Send(s.sub, "Right");
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subB);
  DALI_TEST_CHECK(recorder.events.size() == 2u);
  for(const auto& event : recorder.events)
  {
    DALI_TEST_CHECK(event.GetInputEventType() == InputEventType::KEY_EVENT);
    DALI_TEST_EQUALS(event.GetKeyEvent().GetWindowId(), 202u, TEST_LOCATION);
  }
  DALI_TEST_CHECK(s.manager.GetLastFocusChangeDevice() == FocusDevice::PROGRAMMATIC);
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.mainView);
  END_TEST;
}

int UtcDaliIndependentFocusExistingActiveTargetAndDisableP(void)
{
  FocusScenario s;
  FocusRecorder recorder;
  s.mainView.FocusChangedSignal().Connect(&recorder, &FocusRecorder::OnView);
  DALI_TEST_CHECK(s.manager.SetIndependentFocusEnabled(s.main, true));
  DALI_TEST_CHECK(s.mainView.GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_EQUALS(recorder.gained, 0, TEST_LOCATION);
  s.mainView.SetProperty(Actor::Property::VISIBLE, false);
  DALI_TEST_CHECK(!s.manager.GetCurrentFocusView(s.main));
  DALI_TEST_EQUALS(recorder.lost, 1, TEST_LOCATION);
  s.mainView.SetProperty(Actor::Property::VISIBLE, true);
  DALI_TEST_CHECK(s.manager.SetCurrentFocusView(s.mainView));
  DALI_TEST_CHECK(s.manager.SetIndependentFocusEnabled(s.main, false));
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.mainView);
  DALI_TEST_CHECK(KeyFocus::IsKeyInputTarget(s.mainView));
  DALI_TEST_EQUALS(recorder.gained, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.lost, 1, TEST_LOCATION);
  s.main.Lower();
  DALI_TEST_CHECK(!s.manager.GetCurrentFocusView());
  DALI_TEST_EQUALS(recorder.lost, 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliIndependentFocusHiddenKeyOnlyTargetIsReleasedP(void)
{
  FocusScenario s;
  DALI_TEST_CHECK(s.Enable());
  DALI_TEST_CHECK(KeyFocus::SetKeyInputTarget(s.subB));
  s.subB.SetProperty(Actor::Property::VISIBLE, false);
  DALI_TEST_CHECK(!KeyFocus::IsKeyInputTarget(s.subB));
  DALI_TEST_CHECK(!s.subB.GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subA);
  DALI_TEST_CHECK(KeyFocus::IsKeyInputTarget(s.mainView));
  s.subB.SetProperty(Actor::Property::VISIBLE, true);
  DALI_TEST_CHECK(!KeyFocus::IsKeyInputTarget(s.subB));
  END_TEST;
}

int UtcDaliIndependentFocusReparentDoesNotBubbleAcrossWindowsP(void)
{
  FocusScenario s;
  DALI_TEST_CHECK(s.Enable());
  ReparentOnKey reparent;
  reparent.destination = s.mainRoot;
  KeyRecorder recorder;
  s.subA.KeyEventSignal().Connect(&reparent, &ReparentOnKey::OnKey);
  s.mainRoot.KeyEventSignal().Connect(&recorder, &KeyRecorder::OnKey);
  s.subRoot.KeyEventSignal().Connect(&recorder, &KeyRecorder::OnKey);
  s.Send(s.sub, "a");
  DALI_TEST_CHECK(recorder.views.empty());
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.mainView);
  DALI_TEST_CHECK(!s.manager.GetCurrentFocusView(s.sub));
  END_TEST;
}

int UtcDaliIndependentFocusGlobalClearPreservesSubP(void)
{
  FocusScenario s;
  DALI_TEST_CHECK(s.Enable());
  s.manager.ClearFocus();
  DALI_TEST_CHECK(!s.manager.GetCurrentFocusView());
  DALI_TEST_CHECK(!s.mainView.GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subA);
  DALI_TEST_CHECK(s.subA.GetState().Contains(ViewState::FOCUSED));
  KeyRecorder recorder;
  s.subA.KeyEventSignal().Connect(&recorder, &KeyRecorder::OnKey);
  s.Send(s.sub, "a");
  DALI_TEST_CHECK(recorder.views.size() == 1u && recorder.views[0] == s.subA);
  END_TEST;
}

int UtcDaliIndependentFocusWindowDeletionAndNativeIdReuseP(void)
{
  FocusScenario s;
  DALI_TEST_CHECK(s.Enable());
  WeakHandle<Window> oldWindow(s.sub);
  s.sub.Reset();
  DALI_TEST_CHECK(!oldWindow.GetHandle());
  DALI_TEST_CHECK(!s.subA.IsConnectedToScene());
  DALI_TEST_CHECK(!s.subA.GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.mainView);
  Window replacement = Window::New(PositionSize(0, 0, 480, 800), "replacement");
  Dali::Internal::Adaptor::Adaptor::GetScene(replacement).SetNativeId(202);
  View replacementView = View::New();
  replacementView.SetFocusable(true);
  replacement.Add(replacementView);
  DALI_TEST_CHECK(!s.manager.IsIndependentFocusEnabled(replacement));
  DALI_TEST_CHECK(!s.manager.GetCurrentFocusView(replacement));
  DALI_TEST_CHECK(s.manager.SetCurrentFocusView(replacementView));
  DALI_TEST_CHECK(!replacementView.GetState().Contains(ViewState::FOCUSED));
  END_TEST;
}

int UtcDaliIndependentFocusNormalRemovalReportsOwningWindowP(void)
{
  FocusScenario s;
  FocusRecorder recorder;
  s.manager.WindowFocusChangedSignal().Connect(&recorder, &FocusRecorder::OnWindow);
  s.mainRoot.Remove(s.mainView);
  DALI_TEST_CHECK(!s.manager.GetCurrentFocusView());
  DALI_TEST_CHECK(!s.mainView.GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(recorder.windows.size() == 1u && recorder.windows[0] == s.main);
  DALI_TEST_CHECK(recorder.oldViews[0] == s.mainView && !recorder.newViews[0]);
  END_TEST;
}

int UtcDaliIndependentFocusStoredThenEnableNavigationP(void)
{
  FocusScenario s;
  DALI_TEST_CHECK(s.manager.SetCurrentFocusView(s.subA));
  DALI_TEST_CHECK(s.manager.SetIndependentFocusEnabled(s.sub, true));
  InitialNavigation navigation;
  navigation.target = s.subB;
  s.manager.SetFocusNavigationFallback(FocusNavigationCallback::New(&navigation, &InitialNavigation::Navigate));
  const bool moved = s.manager.MoveFocus(s.sub, FocusDirection::RIGHT);
  s.manager.SetFocusNavigationFallback({});
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(navigation.calls, 1, TEST_LOCATION);
  DALI_TEST_CHECK(navigation.currentWasEmpty);
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subB);
  DALI_TEST_CHECK(KeyFocus::IsKeyInputTarget(s.subB));
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.mainView);
  END_TEST;
}

int UtcDaliIndependentFocusHideShowNavigationP(void)
{
  FocusScenario s;
  DALI_TEST_CHECK(s.Enable());
  s.sub.Hide();
  s.sub.Show();
  DALI_TEST_CHECK(!s.manager.GetCurrentFocusView(s.sub));
  InitialNavigation navigation;
  navigation.target = s.subB;
  s.manager.SetFocusNavigationFallback(FocusNavigationCallback::New(&navigation, &InitialNavigation::Navigate));
  const bool moved = s.manager.MoveFocus(s.sub, FocusDirection::RIGHT);
  s.manager.SetFocusNavigationFallback({});
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(navigation.calls, 1, TEST_LOCATION);
  DALI_TEST_CHECK(navigation.currentWasEmpty);
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == s.subB);
  DALI_TEST_CHECK(s.mainView.GetState().Contains(ViewState::FOCUSED));
  END_TEST;
}

int UtcDaliIndependentFocusSetHiddenAncestorsP(void)
{
  FocusScenario s;
  DALI_TEST_CHECK(s.Enable());
  View grandparent = View::New();
  View parent      = StackLayout::New();
  s.subRoot.Add(grandparent);
  grandparent.Add(parent);
  View target = s.Add(parent);
  for(View ancestor : {parent, grandparent})
  {
    ancestor.SetVisible(false);
    DALI_TEST_CHECK(target.IsVisible() && !target.IsEffectivelyVisible());
    DALI_TEST_CHECK(s.manager.SetCurrentFocusView(target));
    DALI_TEST_CHECK(KeyFocus::SetKeyInputTarget(target));
    DALI_TEST_CHECK(s.manager.RequestFocus(parent));
    DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == target);
    DALI_TEST_CHECK(KeyFocus::IsKeyInputTarget(target));
    DALI_TEST_CHECK(target.GetState().Contains(ViewState::FOCUSED));
    ancestor.SetVisible(true);
    DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == target);
  }
  DALI_TEST_CHECK(s.manager.RequestFocus(parent));
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == target);
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView() == s.mainView);
  END_TEST;
}

int UtcDaliIndependentFocusSetAncestorHiddenByPolicyP(void)
{
  UiConfig config = UiConfig::New();
  GetImpl(config).SetFocusIndicationPolicy(&HideFocusCandidate);
  FocusScenario s(config);
  DALI_TEST_CHECK(s.Enable());
  View parent = View::New();
  s.subRoot.Add(parent);
  View target           = s.Add(parent);
  gHiddenByPolicyTarget = target;
  const bool accepted   = s.manager.SetCurrentFocusView(target);
  gHiddenByPolicyTarget.Reset();
  DALI_TEST_CHECK(accepted);
  DALI_TEST_CHECK(!target.IsEffectivelyVisible());
  DALI_TEST_CHECK(s.manager.GetCurrentFocusView(s.sub) == target);
  DALI_TEST_CHECK(KeyFocus::IsKeyInputTarget(target));
  DALI_TEST_CHECK(s.mainView.GetState().Contains(ViewState::FOCUSED));
  END_TEST;
}

int UtcDaliIndependentFocusActiveKeyTargetNotifiesOnceP(void)
{
  FocusScenario s;
  DALI_TEST_CHECK(s.manager.SetIndependentFocusEnabled(s.main, true));
  View          target = s.Add(s.mainRoot);
  FocusRecorder previousRecorder;
  FocusRecorder targetRecorder;
  s.mainView.FocusChangedSignal().Connect(&previousRecorder, &FocusRecorder::OnView);
  target.FocusChangedSignal().Connect(&targetRecorder, &FocusRecorder::OnView);
  DALI_TEST_CHECK(KeyFocus::SetKeyInputTarget(target));
  DALI_TEST_EQUALS(previousRecorder.lost, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(targetRecorder.gained, 1, TEST_LOCATION);
  DALI_TEST_CHECK(KeyFocus::IsKeyInputTarget(target));
  DALI_TEST_CHECK(KeyFocus::SetKeyInputTarget(target));
  DALI_TEST_EQUALS(previousRecorder.lost, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(targetRecorder.gained, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliIndependentFocusActiveKeyLossKeepsNewestTargetP(void)
{
  FocusScenario s;
  DALI_TEST_CHECK(s.manager.SetIndependentFocusEnabled(s.main, true));
  View          requested = s.Add(s.mainRoot);
  View          latest    = s.Add(s.mainRoot);
  FocusRecorder previousRecorder;
  FocusRecorder requestedRecorder;
  FocusRecorder latestRecorder;
  s.mainView.FocusChangedSignal().Connect(&previousRecorder, &FocusRecorder::OnView);
  requested.FocusChangedSignal().Connect(&requestedRecorder, &FocusRecorder::OnView);
  latest.FocusChangedSignal().Connect(&latestRecorder, &FocusRecorder::OnView);
  bool replaced = false;
  s.mainView.FocusChangedSignal().Connect(&s.application, [&replaced, latest](View, bool focused)
  {
    if(!focused && !replaced)
    {
      replaced = true;
      KeyFocus::SetKeyInputTarget(latest);
    }
  });
  KeyFocus::SetKeyInputTarget(requested);
  DALI_TEST_EQUALS(previousRecorder.lost, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(requestedRecorder.gained, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(latestRecorder.gained, 1, TEST_LOCATION);
  DALI_TEST_CHECK(KeyFocus::IsKeyInputTarget(latest));
  DALI_TEST_CHECK(!KeyFocus::IsKeyInputTarget(requested));
  DALI_TEST_CHECK(!requested.GetState().Contains(ViewState::FOCUSED));
  END_TEST;
}

int UtcDaliIndependentFocusPrimaryPromotionKeepsNewestKeyTargetP(void)
{
  FocusScenario s;
  DALI_TEST_CHECK(s.Enable());
  View previousKey = s.Add(s.mainRoot);
  View latest      = s.Add(s.subRoot);
  DALI_TEST_CHECK(KeyFocus::SetKeyInputTarget(previousKey));
  s.manager.SetClearFocusOnWindowFocusLost(false);
  FocusRecorder previousRecorder;
  FocusRecorder latestRecorder;
  FocusRecorder subRecorder;
  previousKey.FocusChangedSignal().Connect(&previousRecorder, &FocusRecorder::OnView);
  latest.FocusChangedSignal().Connect(&latestRecorder, &FocusRecorder::OnView);
  s.subA.FocusChangedSignal().Connect(&subRecorder, &FocusRecorder::OnView);
  previousKey.FocusChangedSignal().Connect(&s.application, [latest](View, bool focused)
  {
    if(!focused)
    {
      KeyFocus::SetKeyInputTarget(latest);
    }
  });
  s.main.Lower();
  s.sub.Raise();
  DALI_TEST_EQUALS(previousRecorder.lost, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(latestRecorder.gained, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(subRecorder.gained, 0, TEST_LOCATION);
  DALI_TEST_CHECK(KeyFocus::IsKeyInputTarget(latest));
  DALI_TEST_CHECK(!KeyFocus::IsKeyInputTarget(previousKey));
  KeyRecorder keyRecorder;
  latest.KeyEventSignal().Connect(&keyRecorder, &KeyRecorder::OnKey);
  s.Send(s.sub, "a");
  DALI_TEST_CHECK(keyRecorder.views.size() == 1u && keyRecorder.views[0] == latest);
  END_TEST;
}
