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
#include <vector>

#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>
#include <test-gesture-generator.h>
#include <dali/integration-api/events/hover-event-integ.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Integration;

namespace
{

// ============================================================================
// Signal callback helpers
// ============================================================================

struct ClickedSignalData
{
  ClickedSignalData()
  : called(false),
    consumed(false)
  {
  }

  void Reset()
  {
    called   = false;
    consumed = false;
    view     = View();
    event    = InputEvent();
  }

  bool called;
  bool consumed;
  View view;
  InputEvent event;
};

struct ClickedSignalFunctor
{
  ClickedSignalFunctor(ClickedSignalData& data, bool consume = false)
  : signalData(data)
  {
    signalData.consumed = consume;
  }

  bool operator()(View view, InputEvent event)
  {
    signalData.called = true;
    signalData.view   = view;
    signalData.event  = event;
    return signalData.consumed;
  }

  ClickedSignalData& signalData;
};

struct PressedChangedSignalData
{
  PressedChangedSignalData()
  : called(false),
    pressed(false)
  {
  }

  void Reset()
  {
    called  = false;
    pressed = false;
    view    = View();
    event   = InputEvent();
  }

  bool called;
  bool pressed;
  View view;
  InputEvent event;
};

struct PressedChangedSignalFunctor
{
  PressedChangedSignalFunctor(PressedChangedSignalData& data)
  : signalData(data)
  {
  }

  void operator()(View view, bool pressed, InputEvent event)
  {
    signalData.called  = true;
    signalData.pressed = pressed;
    signalData.view    = view;
    signalData.event   = event;
  }

  PressedChangedSignalData& signalData;
};

struct HoveredChangedSignalData
{
  HoveredChangedSignalData()
  : called(false),
    hovered(false)
  {
  }

  void Reset()
  {
    called  = false;
    hovered = false;
    view    = View();
    event   = InputEvent();
  }

  bool called;
  bool hovered;
  View view;
  InputEvent event;
};

struct HoveredChangedSignalFunctor
{
  HoveredChangedSignalFunctor(HoveredChangedSignalData& data)
  : signalData(data)
  {
  }

  void operator()(View view, bool hovered, InputEvent event)
  {
    signalData.called = true;
    signalData.hovered = hovered;
    signalData.view = view;
    signalData.event = event;
  }

  HoveredChangedSignalData& signalData;
};

struct LongPressedSignalData
{
  LongPressedSignalData()
  : called(false),
    consumed(false)
  {
  }

  void Reset()
  {
    called   = false;
    consumed = false;
    view     = View();
  }

  bool called;
  bool consumed;
  View view;
};

struct LongPressedSignalFunctor
{
  LongPressedSignalFunctor(LongPressedSignalData& data, bool consume = false)
  : signalData(data)
  {
    signalData.consumed = consume;
  }

  bool operator()(View view, InputEvent event)
  {
    signalData.called = true;
    signalData.view   = view;
    return signalData.consumed;
  }

  LongPressedSignalData& signalData;
};

/**
 * @brief Helper to create a View, add it to the scene, attach clickable, and render.
 */
View CreateInteractiveView(TestApplication& application, float width = 100.0f, float height = 100.0f)
{
  View view = View::New();
  view.SetRequestedWidth(width);
  view.SetRequestedHeight(height);
  view.SetPivot(Pivot::TOP_LEFT);
  view.SetParentOrigin(ParentOrigin::TOP_LEFT);

  application.GetScene().Add(view);
  view.AsInteractive();

  application.SendNotification();
  application.Render();

  return view;
}

void ProcessTouch(TestApplication& application, PointState::Type state, const Vector2& position, uint32_t time = 100u, int deviceId = 1)
{
  Dali::Integration::TouchEvent touchEvent;
  Dali::Integration::Point      point;
  point.SetState(state);
  point.SetScreenPosition(position);
  point.SetDeviceId(deviceId);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);
  touchEvent.time = time;
  application.ProcessEvent(touchEvent);
}

void ProcessHover(TestApplication& application, PointState::Type state, const Vector2& position, uint32_t time = 100u, int deviceId = 5)
{
  Dali::Integration::HoverEvent hoverEvent(time);
  Dali::Integration::Point      point;
  point.SetState(state);
  point.SetScreenPosition(position);
  point.SetDeviceId(deviceId);
  point.SetDeviceClass(Device::Class::MOUSE);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  hoverEvent.points.push_back(point);
  application.ProcessEvent(hoverEvent);
}

} // namespace

void utc_dali_interactivetrait_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_interactivetrait_cleanup(void)
{
  test_return_value = TET_PASS;
}

// ============================================================================
// Construction / Handle tests
// ============================================================================

int UtcDaliInteractiveTraitDownCastN(void)
{
  UiTestApplication application;
  BaseHandle     handle;
  InteractiveTrait downcast = InteractiveTrait::DownCast(handle);
  DALI_TEST_CHECK(!downcast);
  END_TEST;
}

// ============================================================================
// AsInteractive (View integration)
// ============================================================================

int UtcDaliViewAsInteractiveP(void)
{
  UiTestApplication application;
  View view = View::New();

  InteractiveTrait result = view.AsInteractive();
  DALI_TEST_CHECK(result);

  DALI_TEST_CHECK(view.IsInteractive());
  END_TEST;
}

int UtcDaliViewAsInteractiveWithConfigureP(void)
{
  UiTestApplication application;
  bool configureCalled = false;

  View view = View::New();
  InteractiveTrait configuredTrait = view.AsInteractive();
  configureCalled                  = true;
  configuredTrait.SetKeyClickPolicy(KeyClickPolicy::ON_PRESS);

  DALI_TEST_CHECK(configureCalled);

  InteractiveTrait clickable = view.AsInteractive();
  DALI_TEST_CHECK(clickable);
  DALI_TEST_EQUALS(static_cast<uint32_t>(clickable.GetKeyClickPolicy()),
                   static_cast<uint32_t>(KeyClickPolicy::ON_PRESS),
                   TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewAsInteractiveIdempotentP(void)
{
  UiTestApplication application;
  View view = View::New();

  view.AsInteractive();
  InteractiveTrait first = view.AsInteractive();

  view.AsInteractive();
  InteractiveTrait second = view.AsInteractive();

  DALI_TEST_CHECK(first == second);
  END_TEST;
}

int UtcDaliViewIsInteractiveWithoutAttachN(void)
{
  UiTestApplication application;
  View view = View::New();

  DALI_TEST_CHECK(!view.IsInteractive());
  END_TEST;
}

int UtcDaliViewEnsureInteractiveTraitP(void)
{
  UiTestApplication application;
  View view = View::New();

  InteractiveTrait clickable = view.AsInteractive();
  DALI_TEST_CHECK(clickable);

  // Second call returns the same trait
  InteractiveTrait again = view.AsInteractive();
  DALI_TEST_CHECK(clickable == again);
  END_TEST;
}

// ============================================================================
// State API tests
// ============================================================================

int UtcDaliInteractiveTraitLongPressEnabledP(void)
{
  UiTestApplication application;
  View              view        = CreateInteractiveView(application);
  InteractiveTrait  interactive = view.AsInteractive();

  DALI_TEST_CHECK(interactive.IsLongPressEnabled());

  interactive.SetLongPressEnabled(false);
  DALI_TEST_CHECK(!interactive.IsLongPressEnabled());

  interactive.SetLongPressEnabled(true);
  DALI_TEST_CHECK(interactive.IsLongPressEnabled());
  END_TEST;
}

// ============================================================================
// Tap gesture → ClickedSignal
// ============================================================================

int UtcDaliInteractiveTraitTapClickedSignalP(void)
{
  UiTestApplication application;
  View view = CreateInteractiveView(application);

  ClickedSignalData    data;
  ClickedSignalFunctor functor(data);
  view.AsInteractive().ClickedSignal().Connect(&application, functor);

  // Generate tap at center of the view
  TestGenerateTap(application, 50.0f, 50.0f, 100);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.view == view);
  END_TEST;
}

int UtcDaliInteractiveTraitTapOutsideN(void)
{
  UiTestApplication application;
  View view = CreateInteractiveView(application);

  ClickedSignalData    data;
  ClickedSignalFunctor functor(data);
  view.AsInteractive().ClickedSignal().Connect(&application, functor);

  // Generate tap outside the view (view is 100x100 at top-left)
  TestGenerateTap(application, 200.0f, 200.0f, 100);

  DALI_TEST_CHECK(!data.called);
  END_TEST;
}

int UtcDaliInteractiveTraitSetClickableFalseBlocksTapP(void)
{
  UiTestApplication application;
  View view = CreateInteractiveView(application);
  view.AsInteractive().SetClickable(false);

  ClickedSignalData    data;
  ClickedSignalFunctor functor(data);
  view.AsInteractive().ClickedSignal().Connect(&application, functor);

  TestGenerateTap(application, 50.0f, 50.0f, 100);

  DALI_TEST_CHECK(!data.called);
  END_TEST;
}

int UtcDaliInteractiveTraitSetClickableFalsePassesTouchToParentP(void)
{
  UiTestApplication application;
  View              parent = CreateInteractiveView(application);
  View              child  = View::New();

  child.SetRequestedWidth(100.0f);
  child.SetRequestedHeight(100.0f);
  child.SetPivot(Pivot::TOP_LEFT);
  child.SetParentOrigin(ParentOrigin::TOP_LEFT);
  parent.Add(child);

  InteractiveTrait parentInteractive = parent.AsInteractive();
  InteractiveTrait childInteractive  = child.AsInteractive();
  childInteractive.SetClickable(false);

  application.SendNotification();
  application.Render();

  ClickedSignalData    parentClickedData;
  ClickedSignalFunctor parentClickedFunctor(parentClickedData);
  parentInteractive.ClickedSignal().Connect(&application, parentClickedFunctor);

  ClickedSignalData    childClickedData;
  ClickedSignalFunctor childClickedFunctor(childClickedData);
  childInteractive.ClickedSignal().Connect(&application, childClickedFunctor);

  PressedChangedSignalData    childPressedData;
  PressedChangedSignalFunctor childPressedFunctor(childPressedData);
  childInteractive.PressedChangedSignal().Connect(&application, childPressedFunctor);

  uint32_t childTouchCount = 0u;
  child.TouchEventSignal().Connect(&application, [&childTouchCount](Actor, const TouchEvent&)
  {
    ++childTouchCount;
    return false;
  });

  TestGenerateTap(application, 50.0f, 50.0f, 100u);

  DALI_TEST_CHECK(childTouchCount > 0u);
  DALI_TEST_CHECK(parentClickedData.called);
  DALI_TEST_CHECK(!childClickedData.called);
  DALI_TEST_CHECK(!childPressedData.called);
  DALI_TEST_CHECK(!childInteractive.IsPressed());
  END_TEST;
}

int UtcDaliInteractiveTraitSetClickableFalseAllowsExplicitTouchConsumptionP(void)
{
  UiTestApplication application;
  View              parent = CreateInteractiveView(application);
  View              child  = View::New();

  child.SetRequestedWidth(100.0f);
  child.SetRequestedHeight(100.0f);
  child.SetPivot(Pivot::TOP_LEFT);
  child.SetParentOrigin(ParentOrigin::TOP_LEFT);
  parent.Add(child);

  InteractiveTrait parentInteractive = parent.AsInteractive();
  InteractiveTrait childInteractive  = child.AsInteractive();
  childInteractive.SetClickable(false);

  application.SendNotification();
  application.Render();

  ClickedSignalData    parentClickedData;
  ClickedSignalFunctor parentClickedFunctor(parentClickedData);
  parentInteractive.ClickedSignal().Connect(&application, parentClickedFunctor);

  ClickedSignalData    childClickedData;
  ClickedSignalFunctor childClickedFunctor(childClickedData);
  childInteractive.ClickedSignal().Connect(&application, childClickedFunctor);

  uint32_t childTouchCount = 0u;
  child.TouchEventSignal().Connect(&application, [&childTouchCount](Actor, const TouchEvent&)
  {
    ++childTouchCount;
    return true;
  });

  TestGenerateTap(application, 50.0f, 50.0f, 100u);

  DALI_TEST_CHECK(childTouchCount > 0u);
  DALI_TEST_CHECK(!parentClickedData.called);
  DALI_TEST_CHECK(!childClickedData.called);
  DALI_TEST_CHECK(!childInteractive.IsPressed());
  END_TEST;
}

int UtcDaliInteractiveTraitSetClickableFalseAllowsLongPressP(void)
{
  UiTestApplication application;
  View              parent = CreateInteractiveView(application);
  View              child  = View::New();

  child.SetRequestedWidth(100.0f);
  child.SetRequestedHeight(100.0f);
  child.SetPivot(Pivot::TOP_LEFT);
  child.SetParentOrigin(ParentOrigin::TOP_LEFT);
  parent.Add(child);

  InteractiveTrait parentInteractive = parent.AsInteractive();
  InteractiveTrait childInteractive  = child.AsInteractive();
  childInteractive.SetClickable(false);

  application.SendNotification();
  application.Render();

  ClickedSignalData    childClickedData;
  ClickedSignalFunctor childClickedFunctor(childClickedData);
  childInteractive.ClickedSignal().Connect(&application, childClickedFunctor);

  LongPressedSignalData    childLongPressedData;
  LongPressedSignalFunctor childLongPressedFunctor(childLongPressedData, true);
  childInteractive.LongPressedSignal().Connect(&application, childLongPressedFunctor);

  PressedChangedSignalData    childPressedData;
  PressedChangedSignalFunctor childPressedFunctor(childPressedData);
  childInteractive.PressedChangedSignal().Connect(&application, childPressedFunctor);

  PressedChangedSignalData    parentPressedData;
  PressedChangedSignalFunctor parentPressedFunctor(parentPressedData);
  parentInteractive.PressedChangedSignal().Connect(&application, parentPressedFunctor);

  TestGenerateLongPress(application, 50.0f, 50.0f, 100u);

  DALI_TEST_CHECK(childPressedData.called);
  DALI_TEST_CHECK(childPressedData.pressed);
  DALI_TEST_CHECK(!parentPressedData.called);
  DALI_TEST_CHECK(childLongPressedData.called);
  DALI_TEST_CHECK(!childClickedData.called);

  TestEndLongPress(application, 50.0f, 50.0f, 600u);

  DALI_TEST_CHECK(!childInteractive.IsPressed());
  DALI_TEST_CHECK(!childClickedData.called);

  childInteractive.SetClickable(true);
  TestGenerateTap(application, 50.0f, 50.0f, 700u);
  DALI_TEST_CHECK(childClickedData.called);
  END_TEST;
}

int UtcDaliInteractiveTraitSetClickableFalseLongPressEnabledControlsTouchP(void)
{
  UiTestApplication application;
  View              parent = CreateInteractiveView(application);
  View              child  = View::New();

  child.SetRequestedWidth(100.0f);
  child.SetRequestedHeight(100.0f);
  child.SetPivot(Pivot::TOP_LEFT);
  child.SetParentOrigin(ParentOrigin::TOP_LEFT);
  parent.Add(child);

  InteractiveTrait parentInteractive = parent.AsInteractive();
  InteractiveTrait childInteractive  = child.AsInteractive();
  childInteractive.SetClickable(false);

  application.SendNotification();
  application.Render();

  ClickedSignalData    parentClickedData;
  ClickedSignalFunctor parentClickedFunctor(parentClickedData);
  parentInteractive.ClickedSignal().Connect(&application, parentClickedFunctor);

  ClickedSignalData    childClickedData;
  ClickedSignalFunctor childClickedFunctor(childClickedData);
  childInteractive.ClickedSignal().Connect(&application, childClickedFunctor);

  LongPressedSignalData    childLongPressedData;
  LongPressedSignalFunctor childLongPressedFunctor(childLongPressedData);
  childInteractive.LongPressedSignal().Connect(&application, childLongPressedFunctor);

  PressedChangedSignalData    childPressedData;
  PressedChangedSignalFunctor childPressedFunctor(childPressedData);
  childInteractive.PressedChangedSignal().Connect(&application, childPressedFunctor);

  childInteractive.SetLongPressEnabled(false);
  TestGenerateTap(application, 50.0f, 50.0f, 100u);

  DALI_TEST_CHECK(parentClickedData.called);
  DALI_TEST_CHECK(!childPressedData.called);
  DALI_TEST_CHECK(!childLongPressedData.called);

  parentClickedData.Reset();
  childInteractive.SetClickable(true);
  TestGenerateTap(application, 50.0f, 50.0f, 200u);

  DALI_TEST_CHECK(!parentClickedData.called);
  DALI_TEST_CHECK(childClickedData.called);
  DALI_TEST_CHECK(!childLongPressedData.called);

  childClickedData.Reset();
  childPressedData.Reset();
  childLongPressedData.Reset();
  childInteractive.SetClickable(false);
  childInteractive.SetLongPressEnabled(true);
  TestGenerateLongPress(application, 50.0f, 50.0f, 300u);

  DALI_TEST_CHECK(!parentClickedData.called);
  DALI_TEST_CHECK(childPressedData.called);
  DALI_TEST_CHECK(childPressedData.pressed);
  DALI_TEST_CHECK(childLongPressedData.called);
  DALI_TEST_CHECK(!childClickedData.called);

  TestEndLongPress(application, 50.0f, 50.0f, 800u);
  DALI_TEST_CHECK(!childInteractive.IsPressed());
  END_TEST;
}

// ============================================================================
// Accessibility activation → ClickedSignal
// ============================================================================

int UtcDaliInteractiveTraitAccessibilityActivateP(void)
{
  UiTestApplication application;
  View              view = CreateInteractiveView(application);
  view.SetFocusable(true);

  ClickedSignalData data;
  ClickedSignalFunctor functor(data);
  PressedChangedSignalData pressedData;
  PressedChangedSignalFunctor pressedFunctor(pressedData);
  InteractiveTrait interactive = view.AsInteractive();
  interactive.ClickedSignal().Connect(&application, functor);
  interactive.PressedChangedSignal().Connect(&application, pressedFunctor);

  InputEvent focusCause;
  view.StateChangedSignal().Connect(&application, [&](View, const StateEvent& event) {
    if(ViewState::FOCUSED.WasAdded(event.GetPrev(), event.GetCurrent()))
    {
      focusCause = event.GetCause();
    }
  });

  Property::Map attributes;
  DALI_TEST_CHECK(view.DoAction("activate", attributes));
  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.view == view);
  DALI_TEST_CHECK(data.event);
  DALI_TEST_CHECK(data.event.GetInputEventType() == InputEventType::ACCESSIBILITY_ACTIVATION);
  DALI_TEST_CHECK(!data.event.IsProgrammatic());
  DALI_TEST_CHECK(!pressedData.called);
  DALI_TEST_CHECK(!interactive.IsPressed());
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == view);
  DALI_TEST_CHECK(focusCause);
  DALI_TEST_CHECK(focusCause.GetInputEventType() == InputEventType::ACCESSIBILITY_ACTIVATION);
  END_TEST;
}

int UtcDaliInteractiveViewAccessibilityActivateP(void)
{
  UiTestApplication application;
  InteractiveView view = InteractiveView::New();
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  ClickedSignalData data;
  ClickedSignalFunctor functor(data);
  view.ClickedSignal().Connect(&application, functor);

  Property::Map attributes;
  DALI_TEST_CHECK(view.DoAction("activate", attributes));
  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.view == view);
  DALI_TEST_CHECK(data.event.GetInputEventType() == InputEventType::ACCESSIBILITY_ACTIVATION);
  END_TEST;
}

int UtcDaliInteractiveTraitAccessibilityActivateRespectsEnabledAndClickableP(void)
{
  UiTestApplication application;
  View view = CreateInteractiveView(application);

  ClickedSignalData data;
  ClickedSignalFunctor functor(data);
  InteractiveTrait    interactive = view.AsInteractive();
  interactive.ClickedSignal().Connect(&application, functor);

  Property::Map attributes;

  view.SetEnabled(false);
  view.DoAction("activate", attributes);
  DALI_TEST_CHECK(!data.called);

  view.SetEnabled(true);
  interactive.SetClickable(false);
  view.DoAction("activate", attributes);
  DALI_TEST_CHECK(!data.called);

  interactive.SetClickable(true);
  interactive.SetPseudoDisabled(true);
  DALI_TEST_CHECK(view.DoAction("activate", attributes));
  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.event.GetInputEventType() == InputEventType::ACCESSIBILITY_ACTIVATION);
  END_TEST;
}

int UtcDaliViewAccessibilityActivateRetainsDefaultFocusBehaviorP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetFocusable(true);
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  Property::Map attributes;
  DALI_TEST_CHECK(view.DoAction("activate", attributes));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == view);
  END_TEST;
}

// ============================================================================
// PressedChangedSignal via touch
// ============================================================================

int UtcDaliInteractiveTraitPressedChangedSignalP(void)
{
  UiTestApplication application;
  View view = CreateInteractiveView(application);

  PressedChangedSignalData    data;
  PressedChangedSignalFunctor functor(data);
  view.AsInteractive().PressedChangedSignal().Connect(&application, functor);

  // Touch down
  ProcessTouch(application, PointState::DOWN, Vector2(50.0f, 50.0f), 100u);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(!data.event.IsCancellation());
  DALI_TEST_CHECK(view.AsInteractive().IsPressed());

  data.Reset();

  // Touch up
  ProcessTouch(application, PointState::FINISHED, Vector2(50.0f, 50.0f), 120u);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(!data.event.IsCancellation());
  DALI_TEST_CHECK(!view.AsInteractive().IsPressed());
  END_TEST;
}

int UtcDaliInteractiveTraitTouchHookRunsBeforeTouchSignalP(void)
{
  UiTestApplication application;
  View              view = CreateInteractiveView(application);

  std::vector<Dali::String> dispatchOrder;
  view.AsInteractive().PressedChangedSignal().Connect(
    &application,
    [&dispatchOrder](View, bool pressed, InputEvent) {
      dispatchOrder.push_back(pressed ? "pressed" : "released");
    });
  view.TouchEventSignal().Connect(
    &application,
    [&dispatchOrder, view](Actor, const TouchEvent& event) mutable {
      DALI_TEST_EQUALS(view.AsInteractive().IsPressed(), event.GetState(0) == PointState::STARTED, TEST_LOCATION);
      dispatchOrder.push_back("touch");
      return true;
    });
  view.AsInteractive().ClickedSignal().Connect(
    &application,
    [&dispatchOrder](View, InputEvent) {
      dispatchOrder.push_back("clicked");
    });

  Dali::Integration::TouchEvent touchDown;
  Dali::Integration::Point      point;
  point.SetState(PointState::DOWN);
  point.SetScreenPosition(Vector2(50.0f, 50.0f));
  point.SetDeviceId(1);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchDown.points.push_back(point);
  touchDown.time = 100;
  application.ProcessEvent(touchDown);

  DALI_TEST_EQUALS(dispatchOrder.size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(dispatchOrder[0], Dali::String("pressed"), TEST_LOCATION);
  DALI_TEST_EQUALS(dispatchOrder[1], Dali::String("touch"), TEST_LOCATION);

  dispatchOrder.clear();

  Dali::Integration::TouchEvent touchUp;
  Dali::Integration::Point      pointUp;
  pointUp.SetState(PointState::FINISHED);
  pointUp.SetScreenPosition(Vector2(50.0f, 50.0f));
  pointUp.SetDeviceId(1);
  pointUp.SetDeviceClass(Device::Class::TOUCH);
  pointUp.SetDeviceSubclass(Device::Subclass::NONE);
  touchUp.points.push_back(pointUp);
  touchUp.time = 120;
  application.ProcessEvent(touchUp);

  DALI_TEST_EQUALS(dispatchOrder.size(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(dispatchOrder[0], Dali::String("released"), TEST_LOCATION);
  DALI_TEST_EQUALS(dispatchOrder[1], Dali::String("touch"), TEST_LOCATION);
  DALI_TEST_EQUALS(dispatchOrder[2], Dali::String("clicked"), TEST_LOCATION);
  DALI_TEST_CHECK(!view.AsInteractive().IsPressed());
  END_TEST;
}

int UtcDaliInteractiveTraitSceneDisconnectionClearsPressedP(void)
{
  UiTestApplication application;
  View              view = CreateInteractiveView(application);

  PressedChangedSignalData    data;
  PressedChangedSignalFunctor functor(data);
  view.AsInteractive().PressedChangedSignal().Connect(&application, functor);

  ProcessTouch(application, PointState::DOWN, Vector2(50.0f, 50.0f), 100u);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.pressed);
  DALI_TEST_CHECK(view.AsInteractive().IsPressed());

  data.Reset();

  application.GetScene().Remove(view);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(!data.pressed);
  DALI_TEST_CHECK(data.event.IsCancellation());
  DALI_TEST_CHECK(!view.AsInteractive().IsPressed());
  END_TEST;
}

int UtcDaliInteractiveTraitPseudoDisabledBlocksPressedP(void)
{
  UiTestApplication application;
  View              view = CreateInteractiveView(application);

  PressedChangedSignalData    data;
  PressedChangedSignalFunctor functor(data);
  InteractiveTrait            interactive = view.AsInteractive();
  interactive.PressedChangedSignal().Connect(&application, functor);
  interactive.SetPseudoDisabled(true);

  ProcessTouch(application, PointState::DOWN, Vector2(50.0f, 50.0f), 100u);

  DALI_TEST_CHECK(!data.called);
  DALI_TEST_CHECK(!interactive.IsPressed());
  DALI_TEST_CHECK(!view.GetState().Contains(ViewState::PRESSED));
  END_TEST;
}

int UtcDaliInteractiveTraitDisabledBlocksPressedP(void)
{
  UiTestApplication application;
  View              view = CreateInteractiveView(application);

  PressedChangedSignalData    data;
  PressedChangedSignalFunctor functor(data);
  InteractiveTrait            interactive = view.AsInteractive();
  interactive.PressedChangedSignal().Connect(&application, functor);
  view.SetEnabled(false);

  ProcessTouch(application, PointState::DOWN, Vector2(50.0f, 50.0f), 100u);

  DALI_TEST_CHECK(!data.called);
  DALI_TEST_CHECK(!interactive.IsPressed());
  DALI_TEST_CHECK(!view.GetState().Contains(ViewState::PRESSED));
  END_TEST;
}

int UtcDaliInteractiveTraitVisibilityFalseClearsPressedP(void)
{
  UiTestApplication application;
  View              view = CreateInteractiveView(application);

  PressedChangedSignalData    data;
  PressedChangedSignalFunctor functor(data);
  InteractiveTrait            interactive = view.AsInteractive();
  interactive.PressedChangedSignal().Connect(&application, functor);

  ProcessTouch(application, PointState::DOWN, Vector2(50.0f, 50.0f), 100u);
  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(interactive.IsPressed());

  data.Reset();
  view.SetVisible(false);
  application.SendNotification();
  application.Render();
  ProcessTouch(application, PointState::FINISHED, Vector2(50.0f, 50.0f), 120u);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(!data.pressed);
  DALI_TEST_CHECK(data.event.IsCancellation());
  DALI_TEST_CHECK(!interactive.IsPressed());
  END_TEST;
}

int UtcDaliInteractiveTraitSensitiveFalseClearsPressedP(void)
{
  UiTestApplication application;
  View              view = CreateInteractiveView(application);

  PressedChangedSignalData    data;
  PressedChangedSignalFunctor functor(data);
  InteractiveTrait            interactive = view.AsInteractive();
  interactive.PressedChangedSignal().Connect(&application, functor);

  ProcessTouch(application, PointState::DOWN, Vector2(50.0f, 50.0f), 100u);
  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(interactive.IsPressed());

  data.Reset();
  view.SetSensitive(false);
  ProcessTouch(application, PointState::MOTION, Vector2(50.0f, 50.0f), 120u);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(!data.pressed);
  DALI_TEST_CHECK(data.event.IsCancellation());
  DALI_TEST_CHECK(!interactive.IsPressed());
  END_TEST;
}

int UtcDaliInteractiveTraitParentSensitiveFalseClearsPressedP(void)
{
  UiTestApplication application;
  View              parent = View::New();
  View              view   = View::New();

  parent.SetRequestedWidth(100.0f);
  parent.SetRequestedHeight(100.0f);
  parent.SetPivot(Pivot::TOP_LEFT);
  parent.SetParentOrigin(ParentOrigin::TOP_LEFT);
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetPivot(Pivot::TOP_LEFT);
  view.SetParentOrigin(ParentOrigin::TOP_LEFT);

  application.GetScene().Add(parent);
  parent.Add(view);
  InteractiveTrait interactive = view.AsInteractive();
  application.SendNotification();
  application.Render();

  PressedChangedSignalData    data;
  PressedChangedSignalFunctor functor(data);
  interactive.PressedChangedSignal().Connect(&application, functor);

  ProcessTouch(application, PointState::DOWN, Vector2(50.0f, 50.0f), 100u);
  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(interactive.IsPressed());

  data.Reset();
  parent.SetSensitive(false);
  ProcessTouch(application, PointState::MOTION, Vector2(50.0f, 50.0f), 120u);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(!data.pressed);
  DALI_TEST_CHECK(data.event.IsCancellation());
  DALI_TEST_CHECK(!interactive.IsPressed());
  END_TEST;
}

int UtcDaliInteractiveTraitHoveredChangedSignalP(void)
{
  UiTestApplication application;
  View              view = CreateInteractiveView(application);

  HoveredChangedSignalData    data;
  HoveredChangedSignalFunctor functor(data);
  InteractiveTrait            interactive = view.AsInteractive();
  interactive.HoveredChangedSignal().Connect(&application, functor);

  ProcessHover(application, PointState::STARTED, Vector2(50.0f, 50.0f), 100u);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.hovered);
  DALI_TEST_CHECK(data.view == view);
  DALI_TEST_CHECK(data.event.GetInputEventType() == InputEventType::HOVER_EVENT);
  DALI_TEST_CHECK(interactive.IsHovered());
  DALI_TEST_CHECK(view.GetState().Contains(ViewState::HOVERED));

  data.Reset();
  ProcessHover(application, PointState::MOTION, Vector2(150.0f, 150.0f), 120u);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(!data.hovered);
  DALI_TEST_CHECK(!interactive.IsHovered());
  DALI_TEST_CHECK(!view.GetState().Contains(ViewState::HOVERED));
  END_TEST;
}

int UtcDaliInteractiveTraitHoverMovesBetweenViewsP(void)
{
  UiTestApplication application;
  View              firstView = View::New();
  View              secondView = View::New();

  firstView.SetRequestedWidth(100.0f);
  firstView.SetRequestedHeight(100.0f);
  firstView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  firstView.SetPivot(Pivot::TOP_LEFT);
  firstView.SetParentOrigin(ParentOrigin::TOP_LEFT);
  firstView.SetLayoutMode(LayoutMode::STANDALONE);
  firstView.SetRequestedX(0.0f);
  firstView.SetRequestedY(0.0f);

  secondView.SetRequestedWidth(100.0f);
  secondView.SetRequestedHeight(100.0f);
  secondView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  secondView.SetPivot(Pivot::TOP_LEFT);
  secondView.SetParentOrigin(ParentOrigin::TOP_LEFT);
  secondView.SetLayoutMode(LayoutMode::STANDALONE);
  secondView.SetRequestedX(120.0f);
  secondView.SetRequestedY(0.0f);

  application.GetScene().Add(firstView);
  application.GetScene().Add(secondView);
  InteractiveTrait firstInteractive  = firstView.AsInteractive();
  InteractiveTrait secondInteractive = secondView.AsInteractive();

  application.SendNotification();
  application.Render();

  ProcessHover(application, PointState::STARTED, Vector2(50.0f, 50.0f), 100u);

  DALI_TEST_CHECK(firstInteractive.IsHovered());
  DALI_TEST_CHECK(!secondInteractive.IsHovered());
  DALI_TEST_CHECK(firstView.GetState().Contains(ViewState::HOVERED));
  DALI_TEST_CHECK(!secondView.GetState().Contains(ViewState::HOVERED));

  ProcessHover(application, PointState::MOTION, Vector2(150.0f, 50.0f), 120u);

  DALI_TEST_CHECK(!firstInteractive.IsHovered());
  DALI_TEST_CHECK(secondInteractive.IsHovered());
  DALI_TEST_CHECK(!firstView.GetState().Contains(ViewState::HOVERED));
  DALI_TEST_CHECK(secondView.GetState().Contains(ViewState::HOVERED));
  END_TEST;
}

int UtcDaliInteractiveTraitHoveredCombinesWithPressedP(void)
{
  UiTestApplication application;
  View              view = CreateInteractiveView(application);
  InteractiveTrait  interactive = view.AsInteractive();

  ProcessHover(application, PointState::STARTED, Vector2(50.0f, 50.0f), 100u);
  ProcessTouch(application, PointState::DOWN, Vector2(50.0f, 50.0f), 110u);

  DALI_TEST_CHECK(interactive.IsHovered());
  DALI_TEST_CHECK(interactive.IsPressed());
  DALI_TEST_CHECK(view.GetState().Contains(ViewState::HOVERED));
  DALI_TEST_CHECK(view.GetState().Contains(ViewState::PRESSED));
  END_TEST;
}

int UtcDaliInteractiveTraitHoveredTransitionsToPressedForSameDeviceP(void)
{
  UiTestApplication application;
  View              view = CreateInteractiveView(application);
  InteractiveTrait  interactive = view.AsInteractive();

  ProcessHover(application, PointState::STARTED, Vector2(50.0f, 50.0f), 100u, 5);

  uint32_t stateChangedCount = 0u;
  ViewState previousState;
  ViewState currentState;
  InputEvent stateCause;
  bool sawPressedRelease = false;
  std::vector<std::string> signalOrder;

  HoveredChangedSignalData hoveredData;
  PressedChangedSignalData pressedData;

  view.StateChangedSignal().Connect(&application, [&](View, const StateEvent& event) {
    ++stateChangedCount;
    previousState = event.GetPrev();
    currentState = event.GetCurrent();
    stateCause = event.GetCause();
    sawPressedRelease = sawPressedRelease ||
                        (event.GetPrev().Contains(ViewState::PRESSED) &&
                         !event.GetCurrent().Contains(ViewState::PRESSED));
    signalOrder.push_back("state");
  });
  interactive.HoveredChangedSignal().Connect(&application, [&](View, bool hovered, InputEvent event) {
    hoveredData.called = true;
    hoveredData.hovered = hovered;
    hoveredData.event = event;
    signalOrder.push_back("hovered");
  });
  interactive.PressedChangedSignal().Connect(&application, [&](View, bool pressed, InputEvent event) {
    pressedData.called = true;
    pressedData.pressed = pressed;
    pressedData.event = event;
    signalOrder.push_back("pressed");
  });
  view.TouchEventSignal().Connect(&application, [&](Actor, TouchEvent) {
    signalOrder.push_back("touch");
    return false;
  });

  ProcessTouch(application, PointState::DOWN, Vector2(50.0f, 50.0f), 110u, 5);

  DALI_TEST_EQUALS(stateChangedCount, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(previousState.Contains(ViewState::HOVERED));
  DALI_TEST_CHECK(!previousState.Contains(ViewState::PRESSED));
  DALI_TEST_CHECK(!currentState.Contains(ViewState::HOVERED));
  DALI_TEST_CHECK(currentState.Contains(ViewState::PRESSED));
  DALI_TEST_CHECK(stateCause.GetInputEventType() == InputEventType::TOUCH_EVENT);
  DALI_TEST_CHECK(hoveredData.called);
  DALI_TEST_CHECK(!hoveredData.hovered);
  DALI_TEST_CHECK(hoveredData.event.GetInputEventType() == InputEventType::TOUCH_EVENT);
  DALI_TEST_CHECK(pressedData.called);
  DALI_TEST_CHECK(pressedData.pressed);
  DALI_TEST_CHECK(pressedData.event.GetInputEventType() == InputEventType::TOUCH_EVENT);
  DALI_TEST_EQUALS(signalOrder.size(), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(signalOrder[0], std::string("state"), TEST_LOCATION);
  DALI_TEST_EQUALS(signalOrder[1], std::string("hovered"), TEST_LOCATION);
  DALI_TEST_EQUALS(signalOrder[2], std::string("pressed"), TEST_LOCATION);
  DALI_TEST_EQUALS(signalOrder[3], std::string("touch"), TEST_LOCATION);

  hoveredData.Reset();
  pressedData.Reset();
  signalOrder.clear();

  ProcessHover(application, PointState::FINISHED, Vector2(50.0f, 50.0f), 110u, 5);

  DALI_TEST_EQUALS(stateChangedCount, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!hoveredData.called);
  DALI_TEST_CHECK(!pressedData.called);
  DALI_TEST_CHECK(signalOrder.empty());
  DALI_TEST_CHECK(!interactive.IsHovered());
  DALI_TEST_CHECK(interactive.IsPressed());

  ProcessTouch(application, PointState::UP, Vector2(50.0f, 50.0f), 120u, 5);

  DALI_TEST_CHECK(stateChangedCount >= 2u);
  DALI_TEST_CHECK(sawPressedRelease);
  DALI_TEST_CHECK(!currentState.Contains(ViewState::HOVERED));
  DALI_TEST_CHECK(!currentState.Contains(ViewState::PRESSED));
  DALI_TEST_CHECK(!interactive.IsHovered());
  DALI_TEST_CHECK(!interactive.IsPressed());
  END_TEST;
}

int UtcDaliInteractiveTraitClickableFalseAllowsHoveredP(void)
{
  UiTestApplication application;
  View              view = CreateInteractiveView(application);
  InteractiveTrait  interactive = view.AsInteractive();
  interactive.SetClickable(false);

  ProcessHover(application, PointState::STARTED, Vector2(50.0f, 50.0f), 100u);

  DALI_TEST_CHECK(interactive.IsHovered());
  DALI_TEST_CHECK(view.GetState().Contains(ViewState::HOVERED));
  END_TEST;
}

int UtcDaliInteractiveTraitPseudoDisabledBlocksHoveredP(void)
{
  UiTestApplication application;
  View              view = CreateInteractiveView(application);
  InteractiveTrait  interactive = view.AsInteractive();
  interactive.SetPseudoDisabled(true);

  ProcessHover(application, PointState::STARTED, Vector2(50.0f, 50.0f), 100u);

  DALI_TEST_CHECK(!interactive.IsHovered());
  DALI_TEST_CHECK(!view.GetState().Contains(ViewState::HOVERED));
  END_TEST;
}

int UtcDaliInteractiveTraitDisabledBlocksHoveredP(void)
{
  UiTestApplication application;
  View              view = CreateInteractiveView(application);
  InteractiveTrait  interactive = view.AsInteractive();
  view.SetEnabled(false);

  ProcessHover(application, PointState::STARTED, Vector2(50.0f, 50.0f), 100u);

  DALI_TEST_CHECK(!interactive.IsHovered());
  DALI_TEST_CHECK(!view.GetState().Contains(ViewState::HOVERED));
  END_TEST;
}

int UtcDaliInteractiveTraitSceneDisconnectionClearsHoveredP(void)
{
  UiTestApplication application;
  View              view = CreateInteractiveView(application);

  HoveredChangedSignalData    data;
  HoveredChangedSignalFunctor functor(data);
  InteractiveTrait            interactive = view.AsInteractive();
  interactive.HoveredChangedSignal().Connect(&application, functor);

  ProcessHover(application, PointState::STARTED, Vector2(50.0f, 50.0f), 100u);
  DALI_TEST_CHECK(interactive.IsHovered());

  data.Reset();
  application.GetScene().Remove(view);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(!data.hovered);
  DALI_TEST_CHECK(data.event.IsCancellation());
  DALI_TEST_CHECK(!interactive.IsHovered());
  END_TEST;
}

int UtcDaliInteractiveTraitSensitiveFalseClearsHoveredP(void)
{
  UiTestApplication application;
  View              view = CreateInteractiveView(application);

  HoveredChangedSignalData    data;
  HoveredChangedSignalFunctor functor(data);
  InteractiveTrait            interactive = view.AsInteractive();
  interactive.HoveredChangedSignal().Connect(&application, functor);

  ProcessHover(application, PointState::STARTED, Vector2(50.0f, 50.0f), 100u);
  DALI_TEST_CHECK(interactive.IsHovered());

  data.Reset();
  view.SetSensitive(false);
  ProcessHover(application, PointState::MOTION, Vector2(50.0f, 50.0f), 120u);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(!data.hovered);
  DALI_TEST_CHECK(data.event.IsCancellation());
  DALI_TEST_CHECK(!interactive.IsHovered());
  END_TEST;
}

int UtcDaliInteractiveTraitVisibilityFalseClearsHoveredP(void)
{
  UiTestApplication application;
  View              view = CreateInteractiveView(application);

  HoveredChangedSignalData    data;
  HoveredChangedSignalFunctor functor(data);
  InteractiveTrait            interactive = view.AsInteractive();
  interactive.HoveredChangedSignal().Connect(&application, functor);

  ProcessHover(application, PointState::STARTED, Vector2(50.0f, 50.0f), 100u);
  DALI_TEST_CHECK(interactive.IsHovered());

  data.Reset();
  view.SetVisible(false);
  ProcessHover(application, PointState::MOTION, Vector2(50.0f, 50.0f), 120u);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(!data.hovered);
  DALI_TEST_CHECK(data.event.IsCancellation());
  DALI_TEST_CHECK(!interactive.IsHovered());
  END_TEST;
}

int UtcDaliInteractiveTraitParentSensitiveFalseClearsHoveredP(void)
{
  UiTestApplication application;
  View              parent = View::New();
  View              view = View::New();

  parent.SetRequestedWidth(100.0f);
  parent.SetRequestedHeight(100.0f);
  parent.SetPivot(Pivot::TOP_LEFT);
  parent.SetParentOrigin(ParentOrigin::TOP_LEFT);
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetPivot(Pivot::TOP_LEFT);
  view.SetParentOrigin(ParentOrigin::TOP_LEFT);

  application.GetScene().Add(parent);
  parent.Add(view);
  InteractiveTrait interactive = view.AsInteractive();
  application.SendNotification();
  application.Render();

  HoveredChangedSignalData    data;
  HoveredChangedSignalFunctor functor(data);
  interactive.HoveredChangedSignal().Connect(&application, functor);

  ProcessHover(application, PointState::STARTED, Vector2(50.0f, 50.0f), 100u);
  DALI_TEST_CHECK(interactive.IsHovered());

  data.Reset();
  parent.SetSensitive(false);
  ProcessHover(application, PointState::MOTION, Vector2(50.0f, 50.0f), 120u);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(!data.hovered);
  DALI_TEST_CHECK(data.event.IsCancellation());
  DALI_TEST_CHECK(!interactive.IsHovered());
  END_TEST;
}

// ============================================================================
// Key event → ClickedSignal (ON_RELEASE policy)
// ============================================================================

int UtcDaliInteractiveTraitKeyEventClickedOnReleaseP(void)
{
  UiTestApplication application;
  View view = CreateInteractiveView(application);
  view.AsInteractive().SetKeyClickPolicy(KeyClickPolicy::ON_RELEASE);

  ClickedSignalData    data;
  ClickedSignalFunctor functor(data);
  view.AsInteractive().ClickedSignal().Connect(&application, functor);

  // Give focus to the view
  FocusManager::Get().SetCurrentFocusView(view);
  application.SendNotification();
  application.Render();

  // Key down (Return key)
  Dali::Integration::KeyEvent keyDown(
    "Return", "", "", 0, 0, 100, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(keyDown);

  DALI_TEST_CHECK(!data.called); // ON_RELEASE: clicked not emitted on press

  // Key up
  Dali::Integration::KeyEvent keyUp(
    "Return", "", "", 0, 0, 120, Dali::Integration::KeyEvent::UP, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(keyUp);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.view == view);
  END_TEST;
}

int UtcDaliInteractiveTraitKeyDispatchOrderAndConsumptionP(void)
{
  UiTestApplication application;
  View              view = CreateInteractiveView(application);
  view.AsInteractive().SetKeyClickPolicy(KeyClickPolicy::ON_RELEASE);

  std::vector<Dali::String> dispatchOrder;
  bool                      clicked = false;

  view.AsInteractive().PressedChangedSignal().Connect(
    &application,
    [&dispatchOrder](View, bool pressed, InputEvent) {
      dispatchOrder.push_back(pressed ? "pressed" : "released");
    });
  view.KeyEventSignal().Connect(
    &application,
    [&dispatchOrder, &clicked, view](View, const KeyEvent& event) mutable {
      DALI_TEST_EQUALS(view.AsInteractive().IsPressed(), event.GetState() == KeyEvent::State::DOWN, TEST_LOCATION);
      DALI_TEST_CHECK(!clicked);
      dispatchOrder.push_back("key");
      return true;
    });
  view.AsInteractive().ClickedSignal().Connect(
    &application,
    [&dispatchOrder, &clicked](View, InputEvent) {
      clicked = true;
      dispatchOrder.push_back("clicked");
    });

  FocusManager::Get().SetCurrentFocusView(view);
  application.SendNotification();
  application.Render();

  Dali::Integration::KeyEvent keyDown(
    "Return", "", "", 0, 0, 100, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(keyDown);

  DALI_TEST_EQUALS(dispatchOrder.size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(dispatchOrder[0], Dali::String("pressed"), TEST_LOCATION);
  DALI_TEST_EQUALS(dispatchOrder[1], Dali::String("key"), TEST_LOCATION);
  DALI_TEST_CHECK(!clicked);

  dispatchOrder.clear();

  Dali::Integration::KeyEvent keyUp(
    "Return", "", "", 0, 0, 120, Dali::Integration::KeyEvent::UP, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(keyUp);

  DALI_TEST_EQUALS(dispatchOrder.size(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(dispatchOrder[0], Dali::String("released"), TEST_LOCATION);
  DALI_TEST_EQUALS(dispatchOrder[1], Dali::String("key"), TEST_LOCATION);
  DALI_TEST_EQUALS(dispatchOrder[2], Dali::String("clicked"), TEST_LOCATION);
  DALI_TEST_CHECK(clicked);
  DALI_TEST_CHECK(!view.AsInteractive().IsPressed());
  END_TEST;
}

int UtcDaliInteractiveTraitKeyEventClickedOnPressP(void)
{
  UiTestApplication application;
  View view = CreateInteractiveView(application);
  view.AsInteractive().SetKeyClickPolicy(KeyClickPolicy::ON_PRESS);

  ClickedSignalData    data;
  ClickedSignalFunctor functor(data);
  view.AsInteractive().ClickedSignal().Connect(&application, functor);

  FocusManager::Get().SetCurrentFocusView(view);
  application.SendNotification();
  application.Render();

  // Key down (Return key)
  Dali::Integration::KeyEvent keyDown(
    "Return", "", "", 0, 0, 100, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(keyDown);

  DALI_TEST_CHECK(data.called); // ON_PRESS: clicked emitted on press
  END_TEST;
}

int UtcDaliInteractiveTraitKeyEventOnPressPolicyReleasesPressedStateP(void)
{
  UiTestApplication application;
  View              view = CreateInteractiveView(application);
  view.AsInteractive().SetKeyClickPolicy(KeyClickPolicy::ON_PRESS);

  ClickedSignalData        clickedData;
  ClickedSignalFunctor     clickedFunctor(clickedData);
  PressedChangedSignalData pressedData;
  PressedChangedSignalFunctor pressedFunctor(pressedData);
  view.AsInteractive().ClickedSignal().Connect(&application, clickedFunctor);
  view.AsInteractive().PressedChangedSignal().Connect(&application, pressedFunctor);

  FocusManager::Get().SetCurrentFocusView(view);
  application.SendNotification();
  application.Render();

  Dali::Integration::KeyEvent keyDown(
    "Return", "", "", 0, 0, 100, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(keyDown);

  DALI_TEST_CHECK(clickedData.called);
  DALI_TEST_CHECK(pressedData.called);
  DALI_TEST_CHECK(pressedData.pressed);
  DALI_TEST_CHECK(view.AsInteractive().IsPressed());

  pressedData.Reset();

  Dali::Integration::KeyEvent keyUp(
    "Return", "", "", 0, 0, 120, Dali::Integration::KeyEvent::UP, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(keyUp);

  DALI_TEST_CHECK(pressedData.called);
  DALI_TEST_CHECK(!pressedData.pressed);
  DALI_TEST_CHECK(!view.AsInteractive().IsPressed());
  END_TEST;
}

int UtcDaliInteractiveTraitKeyEventDisabledPolicyP(void)
{
  UiTestApplication application;
  View view = CreateInteractiveView(application);
  view.AsInteractive().SetKeyClickPolicy(KeyClickPolicy::DISABLED);

  ClickedSignalData    data;
  ClickedSignalFunctor functor(data);
  view.AsInteractive().ClickedSignal().Connect(&application, functor);

  FocusManager::Get().SetCurrentFocusView(view);
  application.SendNotification();
  application.Render();

  Dali::Integration::KeyEvent keyDown(
    "Return", "", "", 0, 0, 100, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(keyDown);

  Dali::Integration::KeyEvent keyUp(
    "Return", "", "", 0, 0, 120, Dali::Integration::KeyEvent::UP, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(keyUp);

  DALI_TEST_CHECK(!data.called); // DISABLED: no click from keyboard
  END_TEST;
}

int UtcDaliInteractiveTraitLongPressEnabledControlsKeyP(void)
{
  UiConfig config = UiConfig::New();
  config.SetLongPressKeyEventMinimumCount(2u);

  UiTestApplication application(config);
  View              view        = CreateInteractiveView(application);
  InteractiveTrait  interactive = view.AsInteractive();
  interactive.SetClickable(false);
  interactive.SetKeyClickPolicy(KeyClickPolicy::ON_RELEASE);

  LongPressedSignalData    longPressedData;
  LongPressedSignalFunctor longPressedFunctor(longPressedData);
  interactive.LongPressedSignal().Connect(&application, longPressedFunctor);

  FocusManager::Get().SetCurrentFocusView(view);
  application.SendNotification();
  application.Render();

  interactive.SetLongPressEnabled(false);
  Dali::Integration::KeyEvent firstKeyDown(
    "Return", "", "", 0, 0, 100, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  Dali::Integration::KeyEvent firstKeyRepeat(
    "Return", "", "", 0, 0, 120, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  Dali::Integration::KeyEvent firstKeyUp(
    "Return", "", "", 0, 0, 140, Dali::Integration::KeyEvent::UP, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(firstKeyDown);
  application.ProcessEvent(firstKeyRepeat);
  application.ProcessEvent(firstKeyUp);

  DALI_TEST_CHECK(!longPressedData.called);

  interactive.SetLongPressEnabled(true);
  Dali::Integration::KeyEvent secondKeyDown(
    "Return", "", "", 0, 0, 200, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  Dali::Integration::KeyEvent secondKeyRepeat(
    "Return", "", "", 0, 0, 220, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  Dali::Integration::KeyEvent secondKeyUp(
    "Return", "", "", 0, 0, 240, Dali::Integration::KeyEvent::UP, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(secondKeyDown);
  application.ProcessEvent(secondKeyRepeat);

  DALI_TEST_CHECK(longPressedData.called);
  DALI_TEST_CHECK(longPressedData.view == view);

  application.ProcessEvent(secondKeyUp);
  DALI_TEST_CHECK(!interactive.IsPressed());
  END_TEST;
}

// ============================================================================
// LongPressedSignal
// ============================================================================

int UtcDaliInteractiveTraitLongPressedSignalP(void)
{
  UiTestApplication application;
  View view = CreateInteractiveView(application);

  LongPressedSignalData    data;
  LongPressedSignalFunctor functor(data);
  view.AsInteractive().LongPressedSignal().Connect(&application, functor);

  TestGenerateLongPress(application, 50.0f, 50.0f, 100);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.view == view);
  END_TEST;
}

int UtcDaliInteractiveTraitLongPressBlocksClickP(void)
{
  UiTestApplication application;
  View view = CreateInteractiveView(application);

  LongPressedSignalData    lpData;
  LongPressedSignalFunctor lpFunctor(lpData, true); // consume the long press
  view.AsInteractive().LongPressedSignal().Connect(&application, lpFunctor);

  ClickedSignalData    clickData;
  ClickedSignalFunctor clickFunctor(clickData);
  view.AsInteractive().ClickedSignal().Connect(&application, clickFunctor);

  TestGenerateLongPress(application, 50.0f, 50.0f, 100);
  TestEndLongPress(application, 50.0f, 50.0f, 800);

  DALI_TEST_CHECK(lpData.called);
  // When long press is consumed, subsequent tap/click should be blocked
  END_TEST;
}

// ============================================================================
// AsInteractive with signal connection overloads
// ============================================================================

int UtcDaliViewAsInteractiveWithLambdaP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetPivot(Pivot::TOP_LEFT);
  view.SetParentOrigin(ParentOrigin::TOP_LEFT);

  application.GetScene().Add(view);

  bool clicked = false;
  InteractiveTrait interactive = view.AsInteractive();
  interactive.ClickedSignal().Connect(&application, [&clicked](View v, InputEvent e) -> bool {
    clicked = true;
    return false;
  });

  application.SendNotification();
  application.Render();

  TestGenerateTap(application, 50.0f, 50.0f, 100);

  DALI_TEST_CHECK(clicked);
  END_TEST;
}
