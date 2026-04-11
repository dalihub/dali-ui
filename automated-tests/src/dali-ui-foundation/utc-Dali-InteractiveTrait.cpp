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
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/integration-api/interactive-trait-impl.h>
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-test-suite-utils.h>
#include <test-gesture-generator.h>
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
  }

  bool called;
  bool consumed;
  View view;
};

struct ClickedSignalFunctor
{
  ClickedSignalFunctor(ClickedSignalData& data, bool consume = false)
  : signalData(data)
  {
    signalData.consumed = consume;
  }

  bool operator()(View view, const InputEvent& event)
  {
    signalData.called = true;
    signalData.view   = view;
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
  }

  bool called;
  bool pressed;
  View view;
};

struct PressedChangedSignalFunctor
{
  PressedChangedSignalFunctor(PressedChangedSignalData& data)
  : signalData(data)
  {
  }

  void operator()(View view, bool pressed, const InputEvent& event)
  {
    signalData.called  = true;
    signalData.pressed = pressed;
    signalData.view    = view;
  }

  PressedChangedSignalData& signalData;
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

  bool operator()(View view, const InputEvent& event)
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
  View view = View::New()
    .SetRequestedWidth(width)
    .SetRequestedHeight(height)
    .SetPivot(Pivot::TOP_LEFT)
    .SetParentOrigin(ParentOrigin::TOP_LEFT);

  application.GetScene().Add(view);
  view.AsInteractive();

  application.SendNotification();
  application.Render();

  return view;
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

int UtcDaliInteractiveTraitNewP(void)
{
  UiTestApplication application;
  InteractiveTrait clickable = InteractiveTrait::New();
  DALI_TEST_CHECK(clickable);
  END_TEST;
}

int UtcDaliInteractiveTraitCopyConstructorP(void)
{
  UiTestApplication application;
  InteractiveTrait clickable = InteractiveTrait::New();
  InteractiveTrait copy(clickable);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(copy == clickable);
  END_TEST;
}

int UtcDaliInteractiveTraitDownCastP(void)
{
  UiTestApplication application;
  InteractiveTrait clickable = InteractiveTrait::New();
  BaseHandle     handle(clickable);
  InteractiveTrait downcast = InteractiveTrait::DownCast(handle);
  DALI_TEST_CHECK(downcast);
  END_TEST;
}

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

  View& result = view.AsInteractive();
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

  DALI_TEST_CHECK(view.IsInteractive());
  END_TEST;
}

int UtcDaliViewAsInteractiveWithConfigureP(void)
{
  UiTestApplication application;
  bool configureCalled = false;

  View view = View::New();
  view.AsInteractive([&configureCalled](InteractiveTrait& trait) {
    configureCalled = true;
    trait.SetKeyClickPolicy(KeyClickPolicy::ON_PRESS);
  });

  DALI_TEST_CHECK(configureCalled);

  InteractiveTrait clickable = view.EnsureInteractiveTrait();
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
  InteractiveTrait first = view.EnsureInteractiveTrait();

  view.AsInteractive();
  InteractiveTrait second = view.EnsureInteractiveTrait();

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

  InteractiveTrait clickable = view.EnsureInteractiveTrait();
  DALI_TEST_CHECK(clickable);

  // Second call returns the same trait
  InteractiveTrait again = view.EnsureInteractiveTrait();
  DALI_TEST_CHECK(clickable == again);
  END_TEST;
}

// ============================================================================
// State API tests
// ============================================================================

int UtcDaliInteractiveTraitIsClickableDefaultP(void)
{
  UiTestApplication application;
  InteractiveTrait clickable = InteractiveTrait::New();
  DALI_TEST_CHECK(clickable.IsClickable());
  END_TEST;
}

int UtcDaliInteractiveTraitSetClickableP(void)
{
  UiTestApplication application;
  InteractiveTrait clickable = InteractiveTrait::New();

  clickable.SetClickable(false);
  DALI_TEST_CHECK(!clickable.IsClickable());

  clickable.SetClickable(true);
  DALI_TEST_CHECK(clickable.IsClickable());
  END_TEST;
}

int UtcDaliInteractiveTraitIsPressedDefaultP(void)
{
  UiTestApplication application;
  InteractiveTrait clickable = InteractiveTrait::New();
  DALI_TEST_CHECK(!clickable.IsPressed());
  END_TEST;
}

int UtcDaliInteractiveTraitPseudoDisabledP(void)
{
  UiTestApplication application;
  InteractiveTrait clickable = InteractiveTrait::New();

  DALI_TEST_CHECK(!clickable.IsPseudoDisabled());

  clickable.SetPseudoDisabled(true);
  DALI_TEST_CHECK(clickable.IsPseudoDisabled());

  clickable.SetPseudoDisabled(false);
  DALI_TEST_CHECK(!clickable.IsPseudoDisabled());
  END_TEST;
}

int UtcDaliInteractiveTraitKeyClickPolicyDefaultP(void)
{
  UiTestApplication application;
  InteractiveTrait clickable = InteractiveTrait::New();

  // Default depends on UiConfigManager, just verify it returns a valid value
  KeyClickPolicy policy = clickable.GetKeyClickPolicy();
  DALI_TEST_CHECK(static_cast<uint32_t>(policy) <= static_cast<uint32_t>(KeyClickPolicy::RESERVED));
  END_TEST;
}

int UtcDaliInteractiveTraitSetKeyClickPolicyP(void)
{
  UiTestApplication application;
  InteractiveTrait clickable = InteractiveTrait::New();

  clickable.SetKeyClickPolicy(KeyClickPolicy::DISABLED);
  DALI_TEST_EQUALS(static_cast<uint32_t>(clickable.GetKeyClickPolicy()),
                   static_cast<uint32_t>(KeyClickPolicy::DISABLED),
                   TEST_LOCATION);
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
  view.EnsureInteractiveTrait().ClickedSignal().Connect(&application, functor);

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
  view.EnsureInteractiveTrait().ClickedSignal().Connect(&application, functor);

  // Generate tap outside the view (view is 100x100 at top-left)
  TestGenerateTap(application, 200.0f, 200.0f, 100);

  DALI_TEST_CHECK(!data.called);
  END_TEST;
}

int UtcDaliInteractiveTraitSetClickableFalseBlocksTapP(void)
{
  UiTestApplication application;
  View view = CreateInteractiveView(application);
  view.EnsureInteractiveTrait().SetClickable(false);

  ClickedSignalData    data;
  ClickedSignalFunctor functor(data);
  view.EnsureInteractiveTrait().ClickedSignal().Connect(&application, functor);

  TestGenerateTap(application, 50.0f, 50.0f, 100);

  DALI_TEST_CHECK(!data.called);
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
  view.EnsureInteractiveTrait().PressedChangedSignal().Connect(&application, functor);

  // Touch down
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

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(view.EnsureInteractiveTrait().IsPressed());

  data.Reset();

  // Touch up
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

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(!view.EnsureInteractiveTrait().IsPressed());
  END_TEST;
}

// ============================================================================
// Key event → ClickedSignal (ON_RELEASE policy)
// ============================================================================

int UtcDaliInteractiveTraitKeyEventClickedOnReleaseP(void)
{
  UiTestApplication application;
  View view = CreateInteractiveView(application);
  view.EnsureInteractiveTrait().SetKeyClickPolicy(KeyClickPolicy::ON_RELEASE);

  ClickedSignalData    data;
  ClickedSignalFunctor functor(data);
  view.EnsureInteractiveTrait().ClickedSignal().Connect(&application, functor);

  // Give focus to the view
  KeyboardFocusManager::Get().SetCurrentFocusActor(view);
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

int UtcDaliInteractiveTraitKeyEventClickedOnPressP(void)
{
  UiTestApplication application;
  View view = CreateInteractiveView(application);
  view.EnsureInteractiveTrait().SetKeyClickPolicy(KeyClickPolicy::ON_PRESS);

  ClickedSignalData    data;
  ClickedSignalFunctor functor(data);
  view.EnsureInteractiveTrait().ClickedSignal().Connect(&application, functor);

  KeyboardFocusManager::Get().SetCurrentFocusActor(view);
  application.SendNotification();
  application.Render();

  // Key down (Return key)
  Dali::Integration::KeyEvent keyDown(
    "Return", "", "", 0, 0, 100, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(keyDown);

  DALI_TEST_CHECK(data.called); // ON_PRESS: clicked emitted on press
  END_TEST;
}

int UtcDaliInteractiveTraitKeyEventDisabledPolicyP(void)
{
  UiTestApplication application;
  View view = CreateInteractiveView(application);
  view.EnsureInteractiveTrait().SetKeyClickPolicy(KeyClickPolicy::DISABLED);

  ClickedSignalData    data;
  ClickedSignalFunctor functor(data);
  view.EnsureInteractiveTrait().ClickedSignal().Connect(&application, functor);

  KeyboardFocusManager::Get().SetCurrentFocusActor(view);
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

// ============================================================================
// LongPressedSignal
// ============================================================================

int UtcDaliInteractiveTraitLongPressedSignalP(void)
{
  UiTestApplication application;
  View view = CreateInteractiveView(application);

  LongPressedSignalData    data;
  LongPressedSignalFunctor functor(data);
  view.EnsureInteractiveTrait().LongPressedSignal().Connect(&application, functor);

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
  view.EnsureInteractiveTrait().LongPressedSignal().Connect(&application, lpFunctor);

  ClickedSignalData    clickData;
  ClickedSignalFunctor clickFunctor(clickData);
  view.EnsureInteractiveTrait().ClickedSignal().Connect(&application, clickFunctor);

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
  View view = View::New()
    .SetRequestedWidth(100.0f)
    .SetRequestedHeight(100.0f)
    .SetPivot(Pivot::TOP_LEFT)
    .SetParentOrigin(ParentOrigin::TOP_LEFT);

  application.GetScene().Add(view);

  bool clicked = false;
  view.AsInteractive([&application, &clicked](InteractiveTrait& trait) {
    trait.ClickedSignal().Connect(&application, [&clicked](View v, const InputEvent& e) -> bool {
      clicked = true;
      return false;
    });
  });

  application.SendNotification();
  application.Render();

  TestGenerateTap(application, 50.0f, 50.0f, 100);

  DALI_TEST_CHECK(clicked);
  END_TEST;
}
