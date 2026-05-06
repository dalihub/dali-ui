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
#include <dali-ui-foundation/integration-api/interactive-view-impl.h>
#include <dali/devel-api/object/type-registry.h>
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
  : called(false)
  {
  }

  void Reset()
  {
    called = false;
    view   = View();
  }

  bool called;
  View view;
};

struct ClickedSignalFunctor
{
  ClickedSignalFunctor(ClickedSignalData& data)
  : signalData(data)
  {
  }

  void operator()(View view, const InputEvent& event)
  {
    signalData.called = true;
    signalData.view   = view;
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

// ============================================================================
// InteractiveEventReceiverInterface test subclass
// ============================================================================

class TestInteractiveViewImpl : public InteractiveViewImpl
{
public:
  static IntrusivePtr<TestInteractiveViewImpl> New()
  {
    return new TestInteractiveViewImpl();
  }

  bool clickedCalled{false};
  bool pressedChangedCalled{false};
  bool pressedValue{false};
  bool longPressedCalled{false};
  bool longPressedConsumed{false};

  void ResetFlags()
  {
    clickedCalled        = false;
    pressedChangedCalled = false;
    pressedValue         = false;
    longPressedCalled    = false;
  }

  void OnInitialize() override
  {
    InteractiveViewImpl::OnInitialize();
    EnableLongPressDetection();
  }

protected:
  void OnClicked(View view, const InputEvent& event) override
  {
    clickedCalled = true;
  }

  void OnPressedChanged(View view, bool pressed, const InputEvent& event) override
  {
    pressedChangedCalled = true;
    pressedValue         = pressed;
  }

  bool OnLongPressed(View view, const InputEvent& event) override
  {
    longPressedCalled = true;
    return longPressedConsumed;
  }
};

// Register TestInteractiveViewImpl so that DALi TypeInfo lookup can walk
// the chain TestInteractiveViewImpl -> InteractiveViewImpl -> ViewImpl -> View
// and find View-registered properties (REQUESTED_WIDTH, etc.).
Dali::TypeRegistration testInteractiveViewTypeReg(typeid(TestInteractiveViewImpl), typeid(InteractiveViewImpl), nullptr);

InteractiveView CreateTestInteractiveView(TestApplication& application, float width = 100.0f, float height = 100.0f)
{
  InteractiveView view = InteractiveView::New();
  view.SetRequestedWidth(width);
  view.SetRequestedHeight(height);
  view.SetPivot(Pivot::TOP_LEFT);
  view.SetParentOrigin(ParentOrigin::TOP_LEFT);

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  return view;
}

InteractiveView CreateTestInteractiveViewFromImpl(TestApplication& application, IntrusivePtr<TestInteractiveViewImpl> impl)
{
  // Handle must be created BEFORE Initialize() — wrapping in CustomActor makes Self() valid
  InteractiveView view(*impl);
  impl->Initialize();

  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetPivot(Pivot::TOP_LEFT);
  view.SetParentOrigin(ParentOrigin::TOP_LEFT);

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  return view;
}

} // namespace

void utc_dali_interactiveview_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_interactiveview_cleanup(void)
{
  test_return_value = TET_PASS;
}

// ============================================================================
// Construction / Handle tests
// ============================================================================

int UtcDaliInteractiveViewNewP(void)
{
  UiTestApplication application;
  InteractiveView   view = InteractiveView::New();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliInteractiveViewCopyConstructorP(void)
{
  UiTestApplication application;
  InteractiveView   view = InteractiveView::New();
  InteractiveView   copy(view);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(copy == view);
  END_TEST;
}

int UtcDaliInteractiveViewMoveConstructorP(void)
{
  UiTestApplication application;
  InteractiveView   view = InteractiveView::New();
  InteractiveView   moved(std::move(view));
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!view); // NOLINT(bugprone-use-after-move)
  END_TEST;
}

int UtcDaliInteractiveViewDownCastP(void)
{
  UiTestApplication application;
  InteractiveView   view = InteractiveView::New();
  BaseHandle        handle(view);
  InteractiveView   downcast = InteractiveView::DownCast(handle);
  DALI_TEST_CHECK(downcast);
  END_TEST;
}

int UtcDaliInteractiveViewDownCastN(void)
{
  UiTestApplication application;
  BaseHandle        handle;
  InteractiveView   downcast = InteractiveView::DownCast(handle);
  DALI_TEST_CHECK(!downcast);
  END_TEST;
}

// ============================================================================
// InteractiveView has InteractiveTrait by default
// ============================================================================

int UtcDaliInteractiveViewIsInteractiveByDefaultP(void)
{
  UiTestApplication application;
  InteractiveView   view = InteractiveView::New();
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(view.IsClickable());
  DALI_TEST_CHECK(!view.IsPressed());
  END_TEST;
}

// ============================================================================
// API delegation tests
// ============================================================================

int UtcDaliInteractiveViewSetClickableP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);

  view.SetClickable(false);
  DALI_TEST_CHECK(!view.IsClickable());

  view.SetClickable(true);
  DALI_TEST_CHECK(view.IsClickable());
  END_TEST;
}

int UtcDaliInteractiveViewPseudoDisabledP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);

  DALI_TEST_CHECK(!view.IsPseudoDisabled());

  view.SetPseudoDisabled(true);
  DALI_TEST_CHECK(view.IsPseudoDisabled());

  view.SetPseudoDisabled(false);
  DALI_TEST_CHECK(!view.IsPseudoDisabled());
  END_TEST;
}

int UtcDaliInteractiveViewKeyClickPolicyP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);

  view.SetKeyClickPolicy(KeyClickPolicy::ON_PRESS);
  DALI_TEST_EQUALS(static_cast<uint32_t>(view.GetKeyClickPolicy()),
                   static_cast<uint32_t>(KeyClickPolicy::ON_PRESS),
                   TEST_LOCATION);
  END_TEST;
}

// ============================================================================
// ClickedSignal via tap
// ============================================================================

int UtcDaliInteractiveViewClickedSignalP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);

  ClickedSignalData    data;
  ClickedSignalFunctor functor(data);
  view.ClickedSignal().Connect(&application, functor);

  TestGenerateTap(application, 50.0f, 50.0f, 100);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.view == view);
  END_TEST;
}

// ============================================================================
// PressedChangedSignal via touch
// ============================================================================

int UtcDaliInteractiveViewPressedChangedSignalP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);

  PressedChangedSignalData    data;
  PressedChangedSignalFunctor functor(data);
  view.PressedChangedSignal().Connect(&application, functor);

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
  DALI_TEST_CHECK(data.pressed);
  DALI_TEST_CHECK(view.IsPressed());

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
  DALI_TEST_CHECK(!data.pressed);
  DALI_TEST_CHECK(!view.IsPressed());
  END_TEST;
}

int UtcDaliInteractiveViewSceneDisconnectionClearsPressedP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);

  PressedChangedSignalData    data;
  PressedChangedSignalFunctor functor(data);
  view.PressedChangedSignal().Connect(&application, functor);

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
  DALI_TEST_CHECK(data.pressed);
  DALI_TEST_CHECK(view.IsPressed());

  data.Reset();

  application.GetScene().Remove(view);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(!data.pressed);
  DALI_TEST_CHECK(!view.IsPressed());
  END_TEST;
}

// ============================================================================
// LongPressedSignal
// ============================================================================

int UtcDaliInteractiveViewLongPressedSignalP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);

  LongPressedSignalData    data;
  LongPressedSignalFunctor functor(data);
  view.LongPressedSignal().Connect(&application, functor);

  TestGenerateLongPress(application, 50.0f, 50.0f);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.view == view);
  END_TEST;
}

// ============================================================================
// ConnectClickedSignal chain helper
// ============================================================================

int UtcDaliInteractiveViewConnectClickedSignalP(void)
{
  UiTestApplication application;

  ClickedSignalData    data;
  ClickedSignalFunctor functor(data);

  InteractiveView view = InteractiveView::New();
  view.SetRequestedWidth(100.0f)
      .SetRequestedHeight(100.0f)
      .SetPivot(Pivot::TOP_LEFT)
      .SetParentOrigin(ParentOrigin::TOP_LEFT)
      .ConnectClickedSignal(&application, functor);

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  TestGenerateTap(application, 50.0f, 50.0f, 100);

  DALI_TEST_CHECK(data.called);
  END_TEST;
}

int UtcDaliInteractiveViewConnectClickedSignalAccumulatesP(void)
{
  UiTestApplication application;

  ClickedSignalData    data1;
  ClickedSignalFunctor functor1(data1);
  ClickedSignalData    data2;
  ClickedSignalFunctor functor2(data2);

  InteractiveView view = CreateTestInteractiveView(application);
  view.ConnectClickedSignal(&application, functor1);
  view.ConnectClickedSignal(&application, functor2);

  TestGenerateTap(application, 50.0f, 50.0f, 100);

  DALI_TEST_CHECK(data1.called);
  DALI_TEST_CHECK(data2.called);
  END_TEST;
}

// ============================================================================
// ConnectPressedChangedSignal chain helper
// ============================================================================

int UtcDaliInteractiveViewConnectPressedChangedSignalP(void)
{
  UiTestApplication application;

  PressedChangedSignalData    data;
  PressedChangedSignalFunctor functor(data);

  InteractiveView view = InteractiveView::New();
  view.SetRequestedWidth(100.0f)
      .SetRequestedHeight(100.0f)
      .SetPivot(Pivot::TOP_LEFT)
      .SetParentOrigin(ParentOrigin::TOP_LEFT)
      .ConnectPressedChangedSignal(&application, functor);

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

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
  DALI_TEST_CHECK(data.pressed);
  END_TEST;
}

// ============================================================================
// ConnectLongPressedSignal chain helper
// ============================================================================

int UtcDaliInteractiveViewConnectLongPressedSignalP(void)
{
  UiTestApplication application;

  LongPressedSignalData    data;
  LongPressedSignalFunctor functor(data);

  InteractiveView view = InteractiveView::New();
  view.SetRequestedWidth(100.0f)
      .SetRequestedHeight(100.0f)
      .SetPivot(Pivot::TOP_LEFT)
      .SetParentOrigin(ParentOrigin::TOP_LEFT)
      .ConnectLongPressedSignal(&application, functor);

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  TestGenerateLongPress(application, 50.0f, 50.0f);

  DALI_TEST_CHECK(data.called);
  END_TEST;
}

// ============================================================================
// InteractiveEventReceiverInterface virtual dispatch tests
// ============================================================================

int UtcDaliInteractiveViewOnClickedVirtualP(void)
{
  UiTestApplication application;

  auto impl = TestInteractiveViewImpl::New();

  InteractiveView view = CreateTestInteractiveViewFromImpl(application, impl);

  TestGenerateTap(application, 50.0f, 50.0f, 100);

  DALI_TEST_CHECK(impl->clickedCalled);
  END_TEST;
}

int UtcDaliInteractiveViewOnPressedChangedVirtualP(void)
{
  UiTestApplication application;

  auto impl = TestInteractiveViewImpl::New();

  InteractiveView view = CreateTestInteractiveViewFromImpl(application, impl);

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

  DALI_TEST_CHECK(impl->pressedChangedCalled);
  DALI_TEST_CHECK(impl->pressedValue);
  END_TEST;
}

int UtcDaliInteractiveViewOnLongPressedVirtualP(void)
{
  UiTestApplication application;

  auto impl = TestInteractiveViewImpl::New();

  InteractiveView view = CreateTestInteractiveViewFromImpl(application, impl);

  TestGenerateLongPress(application, 50.0f, 50.0f);

  DALI_TEST_CHECK(impl->longPressedCalled);
  END_TEST;
}

int UtcDaliInteractiveViewOnLongPressedSuppressesClickP(void)
{
  UiTestApplication application;

  auto impl             = TestInteractiveViewImpl::New();
  impl->longPressedConsumed = true;

  InteractiveView view = CreateTestInteractiveViewFromImpl(application, impl);

  ClickedSignalData    clickData;
  ClickedSignalFunctor clickFunctor(clickData);
  view.ClickedSignal().Connect(&application, clickFunctor);

  TestGenerateLongPress(application, 50.0f, 50.0f);
  TestEndLongPress(application, 50.0f, 50.0f);

  DALI_TEST_CHECK(impl->longPressedCalled);
  DALI_TEST_CHECK(!clickData.called);
  END_TEST;
}

// ============================================================================
// Virtual + Signal both called
// ============================================================================

int UtcDaliInteractiveViewVirtualAndSignalBothCalledP(void)
{
  UiTestApplication application;

  auto impl = TestInteractiveViewImpl::New();

  InteractiveView view = CreateTestInteractiveViewFromImpl(application, impl);

  ClickedSignalData    signalData;
  ClickedSignalFunctor signalFunctor(signalData);
  view.ClickedSignal().Connect(&application, signalFunctor);

  TestGenerateTap(application, 50.0f, 50.0f, 100);

  // Both virtual and signal should be called
  DALI_TEST_CHECK(impl->clickedCalled);
  DALI_TEST_CHECK(signalData.called);
  END_TEST;
}
