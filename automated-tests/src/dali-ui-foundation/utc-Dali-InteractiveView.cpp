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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/provider-api/interactive-view-impl.h>
#include <dali-ui-foundation/integration-api/reserved-trait-id.h>
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <test-gesture-generator.h>

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

  void operator()(View view, InputEvent event)
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

  void operator()(View view, bool pressed, InputEvent event)
  {
    signalData.called  = true;
    signalData.pressed = pressed;
    signalData.view    = view;
  }

  PressedChangedSignalData& signalData;
};

struct PseudoDisabledChangedSignalData
{
  PseudoDisabledChangedSignalData()
  : called(false),
    pseudoDisabled(false)
  {
  }

  bool called;
  bool pseudoDisabled;
  View view;
};

struct PseudoDisabledChangedSignalFunctor
{
  PseudoDisabledChangedSignalFunctor(PseudoDisabledChangedSignalData& data)
  : signalData(data)
  {
  }

  void operator()(View view, bool pseudoDisabled)
  {
    signalData.called         = true;
    signalData.pseudoDisabled = pseudoDisabled;
    signalData.view           = view;
  }

  PseudoDisabledChangedSignalData& signalData;
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

class TestInteractiveViewImpl : public InteractiveViewImpl
{
public:
  static IntrusivePtr<TestInteractiveViewImpl> New()
  {
    return new TestInteractiveViewImpl();
  }
};

Dali::TypeRegistration testInteractiveViewImplTypeReg(typeid(TestInteractiveViewImpl), typeid(InteractiveViewImpl), nullptr);

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

View CreateTestView(TestApplication& application, float width = 100.0f, float height = 100.0f)
{
  View view = View::New();
  view.SetRequestedWidth(width);
  view.SetRequestedHeight(height);
  view.SetPivot(Pivot::TOP_LEFT);
  view.SetParentOrigin(ParentOrigin::TOP_LEFT);

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  return view;
}

void ProcessTouch(UiTestApplication& application, PointState::Type state, uint32_t time = 100u)
{
  Dali::Integration::TouchEvent touchEvent;
  Dali::Integration::Point      point;
  point.SetState(state);
  point.SetScreenPosition(Vector2(50.0f, 50.0f));
  point.SetDeviceId(1);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);
  touchEvent.time = time;
  application.ProcessEvent(touchEvent);
}

void SetFocusIndicated(View view)
{
  FocusManager::Get().SetCurrentFocusView(view);
  IntegrationView::SetState(GetImpl(view), ViewState::FOCUS_INDICATED, true);
}

constexpr auto OVERLAY_VISUAL_RANGE = Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT;
constexpr float OVERLAY_RECOIL_SCALE_FACTOR = 0.98f;
constexpr float OVERLAY_DISABLED_OPACITY_FACTOR = 0.4f;

ColorVisual GetOverlayVisual(View view)
{
  if(view.GetVisualCount(OVERLAY_VISUAL_RANGE) == 0u)
  {
    return ColorVisual();
  }
  return ColorVisual::DownCast(view.GetVisualAt(OVERLAY_VISUAL_RANGE, 0u));
}

void FinishRecoilAnimation(UiTestApplication& application)
{
  application.SendNotification();
  application.Render(0);
  application.Render(150);
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

int UtcDaliInteractiveViewPseudoDisabledChangedSignalP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);

  PseudoDisabledChangedSignalData    data;
  PseudoDisabledChangedSignalFunctor functor(data);
  view.PseudoDisabledChangedSignal().Connect(&application, functor);

  view.SetPseudoDisabled(true);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.pseudoDisabled);
  DALI_TEST_CHECK(data.view == view);
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
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetPivot(Pivot::TOP_LEFT);
  view.SetParentOrigin(ParentOrigin::TOP_LEFT);
  view.ConnectClickedSignal(&application, functor);

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
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetPivot(Pivot::TOP_LEFT);
  view.SetParentOrigin(ParentOrigin::TOP_LEFT);
  view.ConnectPressedChangedSignal(&application, functor);

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
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetPivot(Pivot::TOP_LEFT);
  view.SetParentOrigin(ParentOrigin::TOP_LEFT);
  view.ConnectLongPressedSignal(&application, functor);

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  TestGenerateLongPress(application, 50.0f, 50.0f);

  DALI_TEST_CHECK(data.called);
  END_TEST;
}

int UtcDaliInteractiveViewLongPressedSignalSuppressesClickP(void)
{
  UiTestApplication application;

  InteractiveView view = CreateTestInteractiveView(application);

  ClickedSignalData    clickData;
  ClickedSignalFunctor clickFunctor(clickData);
  view.ClickedSignal().Connect(&application, clickFunctor);

  LongPressedSignalData    longPressData;
  LongPressedSignalFunctor longPressFunctor(longPressData, true);
  view.LongPressedSignal().Connect(&application, longPressFunctor);

  TestGenerateLongPress(application, 50.0f, 50.0f);
  TestEndLongPress(application, 50.0f, 50.0f);

  DALI_TEST_CHECK(longPressData.called);
  DALI_TEST_CHECK(!clickData.called);
  END_TEST;
}

int UtcDaliInteractiveViewImplSubclassSmokeP(void)
{
  UiTestApplication application;

  auto            impl = TestInteractiveViewImpl::New();
  InteractiveView view = CreateTestInteractiveViewFromImpl(application, impl);

  DALI_TEST_CHECK(view.IsInteractive());
  DALI_TEST_CHECK(view.IsClickable());

  view.SetClickable(false);
  DALI_TEST_CHECK(!view.IsClickable());

  view.SetKeyClickPolicy(KeyClickPolicy::ON_PRESS);
  DALI_TEST_EQUALS(static_cast<uint32_t>(view.GetKeyClickPolicy()),
                   static_cast<uint32_t>(KeyClickPolicy::ON_PRESS),
                   TEST_LOCATION);

  ClickedSignalData    data;
  ClickedSignalFunctor functor(data);
  view.ClickedSignal().Connect(&application, functor);

  view.SetClickable(true);
  TestGenerateTap(application, 50.0f, 50.0f, 100);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.view == view);
  END_TEST;
}

// ============================================================================
// Default OverlayEffect
// ============================================================================

int UtcDaliInteractiveViewDefaultOverlayEffectP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);

  uint32_t initialChildCount = view.GetChildCount();

  ProcessTouch(application, PointState::DOWN);

  DALI_TEST_EQUALS(view.GetChildCount(), initialChildCount, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualCount(OVERLAY_VISUAL_RANGE), 1u, TEST_LOCATION);
  ColorVisual overlay = GetOverlayVisual(view);
  DALI_TEST_CHECK(overlay);
  DALI_TEST_EQUALS(overlay.GetColor().GetRgba().a, 0.1f, 0.001f, TEST_LOCATION);

  ProcessTouch(application, PointState::FINISHED, 120u);

  DALI_TEST_EQUALS(view.GetChildCount(), initialChildCount, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualCount(OVERLAY_VISUAL_RANGE), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInteractiveViewOverlayEffectTargetP(void)
{
  UiTestApplication application;
  InteractiveView   owner  = CreateTestInteractiveView(application);
  View              target = View::New();
  target.SetRequestedWidth(80.0f);
  target.SetRequestedHeight(80.0f);
  owner.Add(target);
  owner.SetStateEffectTarget(target);

  ProcessTouch(application, PointState::DOWN);

  DALI_TEST_EQUALS(owner.GetVisualCount(OVERLAY_VISUAL_RANGE), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(target.GetVisualCount(OVERLAY_VISUAL_RANGE), 1u, TEST_LOCATION);

  ProcessTouch(application, PointState::FINISHED, 120u);

  DALI_TEST_EQUALS(target.GetVisualCount(OVERLAY_VISUAL_RANGE), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInteractiveViewOverlayEffectCleansTargetOnOwnerDestroyP(void)
{
  UiTestApplication application;
  View              target;

  {
    InteractiveView owner = CreateTestInteractiveView(application);
    target                = View::New();
    target.SetRequestedWidth(80.0f);
    target.SetRequestedHeight(80.0f);
    owner.Add(target);
    owner.SetStateEffectTarget(target);

    ProcessTouch(application, PointState::DOWN);

    DALI_TEST_EQUALS(target.GetVisualCount(OVERLAY_VISUAL_RANGE), 1u, TEST_LOCATION);

    owner.Remove(target);
    DALI_TEST_EQUALS(owner.GetStateEffectTarget(), owner, TEST_LOCATION);
    DALI_TEST_EQUALS(owner.GetVisualCount(OVERLAY_VISUAL_RANGE), 0u, TEST_LOCATION);
    DALI_TEST_EQUALS(target.GetVisualCount(OVERLAY_VISUAL_RANGE), 1u, TEST_LOCATION);

    application.GetScene().Remove(owner);
    owner.Reset();
  }

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(target.GetVisualCount(OVERLAY_VISUAL_RANGE), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInteractiveViewOverlayEffectRetargetsTargetP(void)
{
  UiTestApplication application;
  InteractiveView   owner   = CreateTestInteractiveView(application);
  View              target1 = View::New();
  View              target2 = View::New();
  target1.SetRequestedWidth(80.0f);
  target1.SetRequestedHeight(80.0f);
  target2.SetRequestedWidth(80.0f);
  target2.SetRequestedHeight(80.0f);
  owner.Add(target1);
  owner.Add(target2);
  owner.SetStateEffectTarget(target1);

  ProcessTouch(application, PointState::DOWN);

  ColorVisual overlay = GetOverlayVisual(target1);
  DALI_TEST_CHECK(overlay);
  DALI_TEST_EQUALS(target1.GetVisualCount(OVERLAY_VISUAL_RANGE), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(target2.GetVisualCount(OVERLAY_VISUAL_RANGE), 0u, TEST_LOCATION);

  owner.SetStateEffectTarget(target2);

  DALI_TEST_EQUALS(target1.GetVisualCount(OVERLAY_VISUAL_RANGE), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(target2.GetVisualCount(OVERLAY_VISUAL_RANGE), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetOverlayVisual(target2), overlay, TEST_LOCATION);

  ProcessTouch(application, PointState::FINISHED, 120u);
  END_TEST;
}

int UtcDaliInteractiveViewOverlayEffectFollowsTargetCornerRadiusP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);
  view.SetCornerRadius(Vector4(4.0f, 5.0f, 6.0f, 7.0f));
  view.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
  SetFocusIndicated(view);

  ProcessTouch(application, PointState::DOWN);

  ColorVisual overlay = GetOverlayVisual(view);
  DALI_TEST_EQUALS(overlay.GetCornerRadius(), Vector4(4.0f, 5.0f, 6.0f, 7.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(overlay.GetCornerRadiusPolicy(), CornerRadiusPolicy::ABSOLUTE, TEST_LOCATION);

  ProcessTouch(application, PointState::FINISHED, 120u);
  END_TEST;
}

int UtcDaliInteractiveViewOverlayEffectRefreshesActiveCornerRadiusP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);
  view.SetCornerRadius(Vector4(4.0f, 5.0f, 6.0f, 7.0f));
  view.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
  SetFocusIndicated(view);

  ProcessTouch(application, PointState::DOWN);

  ColorVisual overlay = GetOverlayVisual(view);
  DALI_TEST_EQUALS(overlay.GetCornerRadius(), Vector4(4.0f, 5.0f, 6.0f, 7.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(overlay.GetCornerRadiusPolicy(), CornerRadiusPolicy::ABSOLUTE, TEST_LOCATION);

  view.SetCornerRadius(Vector4(0.1f, 0.2f, 0.3f, 0.4f));
  view.SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);

  ProcessTouch(application, PointState::FINISHED, 120u);

  DALI_TEST_EQUALS(view.GetVisualCount(OVERLAY_VISUAL_RANGE), 0u, TEST_LOCATION);

  ProcessTouch(application, PointState::DOWN, 140u);

  ColorVisual refreshedOverlay = GetOverlayVisual(view);
  DALI_TEST_CHECK(refreshedOverlay);
  DALI_TEST_EQUALS(refreshedOverlay.GetCornerRadius(), Vector4(0.1f, 0.2f, 0.3f, 0.4f), TEST_LOCATION);
  DALI_TEST_EQUALS(refreshedOverlay.GetCornerRadiusPolicy(), CornerRadiusPolicy::RELATIVE, TEST_LOCATION);

  ProcessTouch(application, PointState::FINISHED, 160u);
  END_TEST;
}

int UtcDaliInteractiveViewOverlayEffectExplicitCornerRadiusP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);
  view.SetCornerRadius(20.0f);

  view.SetStateEffect(OverlayEffect::Plain().Configure().SetCornerRadius(Vector4(1.0f, 2.0f, 3.0f, 4.0f)).SetCornerRadiusPolicyRelative().Build());

  ProcessTouch(application, PointState::DOWN);

  ColorVisual overlay = GetOverlayVisual(view);
  DALI_TEST_EQUALS(overlay.GetCornerRadius(), Vector4(1.0f, 2.0f, 3.0f, 4.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(overlay.GetCornerRadiusPolicy(), CornerRadiusPolicy::RELATIVE, TEST_LOCATION);

  ProcessTouch(application, PointState::FINISHED, 120u);
  END_TEST;
}

int UtcDaliInteractiveViewOverlayEffectRoundP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);
  view.SetCornerRadius(20.0f);
  view.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
  view.SetStateEffect(OverlayEffect::Round());

  ProcessTouch(application, PointState::DOWN);

  ColorVisual overlay = GetOverlayVisual(view);
  DALI_TEST_EQUALS(overlay.GetCornerRadius(), Vector4(0.5f, 0.5f, 0.5f, 0.5f), TEST_LOCATION);
  DALI_TEST_EQUALS(overlay.GetCornerRadiusPolicy(), CornerRadiusPolicy::RELATIVE, TEST_LOCATION);

  ProcessTouch(application, PointState::FINISHED, 120u);
  END_TEST;
}

int UtcDaliInteractiveViewOverlayEffectConfigureP(void)
{
  OverlayEffect configured = OverlayEffect::Plain().Configure()
                               .SetOverlayColor(UiColor(0x000000, 0.2f))
                               .SetRecoilScope(RecoilScope::OVERLAY_TARGET_CHILDREN)
                               .Build();
  OverlayEffect explicitCorner = OverlayEffect::Plain().Configure().SetCornerRadius(12.0f).Build();
  OverlayEffect restoredTargetCorner = explicitCorner.Configure().SetUseTargetCornerRadius(true).Build();
  OverlayEffect configuredExplicitCorner = explicitCorner.Configure().Build();
  OverlayEffect defaultBuilderEffect = OverlayEffect::Builder().Build();
  OverlayEffect relativeCornerBuilder = OverlayEffect::Builder().SetCornerRadiusPolicyRelative().Build();
  OverlayEffect targetCornerBuilder = OverlayEffect::Builder().SetCornerRadiusPolicyRelative().SetUseTargetCornerRadius(true).Build();

  DALI_TEST_EQUALS(OverlayEffect::Plain().GetOverlayColor().GetRgba(), UiColor(0x000000, 0.1f).GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(OverlayEffect::Plain().GetRecoilScope(), RecoilScope::OVERLAY_TARGET, TEST_LOCATION);
  DALI_TEST_CHECK(OverlayEffect::Plain().IsUsingTargetCornerRadius());
  DALI_TEST_CHECK(!OverlayEffect::Plain().IsNone());
  DALI_TEST_EQUALS(OverlayEffect::ListItem().GetOverlayColor().GetRgba(), UiColor(0x000000, 0.1f).GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(OverlayEffect::ListItem().GetRecoilScope(), RecoilScope::OVERLAY_TARGET_CHILDREN, TEST_LOCATION);
  DALI_TEST_CHECK(OverlayEffect::ListItem().IsUsingTargetCornerRadius());
  DALI_TEST_CHECK(!OverlayEffect::ListItem().IsNone());
  DALI_TEST_EQUALS(configured.GetOverlayColor().GetRgba(), UiColor(0x000000, 0.2f).GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(configured.GetRecoilScope(), RecoilScope::OVERLAY_TARGET_CHILDREN, TEST_LOCATION);
  DALI_TEST_CHECK(configured.IsUsingTargetCornerRadius());
  DALI_TEST_CHECK(!explicitCorner.IsUsingTargetCornerRadius());
  DALI_TEST_CHECK(!configuredExplicitCorner.IsUsingTargetCornerRadius());
  DALI_TEST_CHECK(restoredTargetCorner.IsUsingTargetCornerRadius());
  DALI_TEST_CHECK(!configured.IsNone());
  DALI_TEST_EQUALS(defaultBuilderEffect.GetOverlayColor().GetRgba(), UiColor(0x000000, 0.1f).GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(defaultBuilderEffect.GetRecoilScope(), RecoilScope::OVERLAY_TARGET, TEST_LOCATION);
  DALI_TEST_CHECK(defaultBuilderEffect.IsUsingTargetCornerRadius());
  DALI_TEST_CHECK(!relativeCornerBuilder.IsUsingTargetCornerRadius());
  DALI_TEST_CHECK(targetCornerBuilder.IsUsingTargetCornerRadius());
  END_TEST;
}

int UtcDaliInteractiveViewOverlayEffectRecoilOwnerTargetP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);
  view.SetScale(1.2f, 0.8f);

  ProcessTouch(application, PointState::DOWN);
  FinishRecoilAnimation(application);

  DALI_TEST_EQUALS(view.GetCurrentScaleX(), 1.2f * OVERLAY_RECOIL_SCALE_FACTOR, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetCurrentScaleY(), 0.8f * OVERLAY_RECOIL_SCALE_FACTOR, 0.001f, TEST_LOCATION);

  ProcessTouch(application, PointState::FINISHED, 120u);
  FinishRecoilAnimation(application);

  DALI_TEST_EQUALS(view.GetCurrentScaleX(), 1.2f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetCurrentScaleY(), 0.8f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInteractiveViewOverlayEffectRecoilRestoreInterruptedByPressP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);
  view.SetScale(1.2f, 0.8f);

  ProcessTouch(application, PointState::DOWN);
  FinishRecoilAnimation(application);

  DALI_TEST_EQUALS(view.GetCurrentScaleX(), 1.2f * OVERLAY_RECOIL_SCALE_FACTOR, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetCurrentScaleY(), 0.8f * OVERLAY_RECOIL_SCALE_FACTOR, 0.001f, TEST_LOCATION);

  FocusManager::Get().ClearFocus();
  ProcessTouch(application, PointState::FINISHED, 120u);
  application.SendNotification();
  application.Render(0);
  application.Render(50);

  ProcessTouch(application, PointState::DOWN, 180u);
  FinishRecoilAnimation(application);

  DALI_TEST_EQUALS(view.GetCurrentScaleX(), 1.2f * OVERLAY_RECOIL_SCALE_FACTOR, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetCurrentScaleY(), 0.8f * OVERLAY_RECOIL_SCALE_FACTOR, 0.001f, TEST_LOCATION);

  FocusManager::Get().ClearFocus();
  ProcessTouch(application, PointState::FINISHED, 240u);
  FinishRecoilAnimation(application);

  DALI_TEST_EQUALS(view.GetCurrentScaleX(), 1.2f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetCurrentScaleY(), 0.8f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInteractiveViewOverlayEffectRecoilStateEffectTargetP(void)
{
  UiTestApplication application;
  InteractiveView   owner  = CreateTestInteractiveView(application);
  View              target = View::New();
  target.SetRequestedWidth(80.0f);
  target.SetRequestedHeight(80.0f);
  target.SetScale(1.5f, 0.5f);
  owner.Add(target);
  owner.SetStateEffectTarget(target);

  application.SendNotification();
  application.Render();

  ProcessTouch(application, PointState::DOWN);
  FinishRecoilAnimation(application);

  DALI_TEST_EQUALS(owner.GetCurrentScaleX(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(owner.GetCurrentScaleY(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(target.GetCurrentScaleX(), 1.5f * OVERLAY_RECOIL_SCALE_FACTOR, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(target.GetCurrentScaleY(), 0.5f * OVERLAY_RECOIL_SCALE_FACTOR, 0.001f, TEST_LOCATION);

  ProcessTouch(application, PointState::FINISHED, 120u);
  FinishRecoilAnimation(application);

  DALI_TEST_EQUALS(target.GetCurrentScaleX(), 1.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(target.GetCurrentScaleY(), 0.5f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInteractiveViewOverlayEffectRecoilChildrenP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);
  view.SetStateEffect(OverlayEffect::ListItem());

  View child1 = View::New();
  child1.SetRequestedWidth(20.0f);
  child1.SetRequestedHeight(20.0f);
  child1.SetRequestedPositionX(10.0f);
  child1.SetRequestedPositionY(20.0f);
  child1.SetScale(2.0f, 1.0f);

  View child2 = View::New();
  child2.SetRequestedWidth(30.0f);
  child2.SetRequestedHeight(30.0f);
  child2.SetRequestedPositionX(50.0f);
  child2.SetRequestedPositionY(40.0f);

  view.Add(child1);
  view.Add(child2);

  application.SendNotification();
  application.Render();

  const Vector3 child1OriginalPivot = child1.GetPivot();
  const Vector3 child2OriginalPivot = child2.GetPivot();

  ProcessTouch(application, PointState::DOWN);
  FinishRecoilAnimation(application);

  DALI_TEST_EQUALS(view.GetCurrentScaleX(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetCurrentScaleY(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(child1.GetCurrentScaleX(), 2.0f * OVERLAY_RECOIL_SCALE_FACTOR, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(child1.GetCurrentScaleY(), OVERLAY_RECOIL_SCALE_FACTOR, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(child2.GetCurrentScaleX(), OVERLAY_RECOIL_SCALE_FACTOR, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(child2.GetCurrentScaleY(), OVERLAY_RECOIL_SCALE_FACTOR, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(child1.GetPivot(), Vector3(2.0f, 1.5f, child1OriginalPivot.z), TEST_LOCATION);
  DALI_TEST_EQUALS(child2.GetPivot(), Vector3(0.0f, 0.333333f, child2OriginalPivot.z), 0.001f, TEST_LOCATION);

  ProcessTouch(application, PointState::FINISHED, 120u);
  FinishRecoilAnimation(application);

  DALI_TEST_EQUALS(child1.GetCurrentScaleX(), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(child1.GetCurrentScaleY(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(child2.GetCurrentScaleX(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(child2.GetCurrentScaleY(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(child1.GetPivot(), child1OriginalPivot, TEST_LOCATION);
  DALI_TEST_EQUALS(child2.GetPivot(), child2OriginalPivot, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInteractiveViewOverlayEffectRecoilChildrenSkipsMoreThanThreeP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);
  view.SetStateEffect(OverlayEffect::Plain().Configure().SetRecoilScope(RecoilScope::OVERLAY_TARGET_CHILDREN).Build());

  View children[4];
  for(uint32_t i = 0u; i < 4u; ++i)
  {
    children[i] = View::New();
    children[i].SetRequestedWidth(20.0f);
    children[i].SetRequestedHeight(20.0f);
    children[i].SetScale(1.0f + static_cast<float>(i), 1.0f + static_cast<float>(i));
    view.Add(children[i]);
  }

  application.SendNotification();
  application.Render();

  ProcessTouch(application, PointState::DOWN);
  FinishRecoilAnimation(application);

  for(uint32_t i = 0u; i < 4u; ++i)
  {
    const float originalScale = 1.0f + static_cast<float>(i);
    DALI_TEST_EQUALS(children[i].GetCurrentScaleX(), originalScale, 0.001f, TEST_LOCATION);
    DALI_TEST_EQUALS(children[i].GetCurrentScaleY(), originalScale, 0.001f, TEST_LOCATION);
  }

  ProcessTouch(application, PointState::FINISHED, 120u);
  FinishRecoilAnimation(application);
  END_TEST;
}

int UtcDaliInteractiveViewOverlayEffectFocusIndicatedAndPressedP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);

  view.SetStateEffect(OverlayEffect::Plain().Configure().SetOverlayColor(UiColor(0x000000, 0.4f)).Build());

  FocusManager::Get().SetCurrentFocusView(view);

  DALI_TEST_CHECK(view.GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(!view.GetState().Contains(ViewState::FOCUS_INDICATED));
  DALI_TEST_EQUALS(view.GetVisualCount(OVERLAY_VISUAL_RANGE), 0u, TEST_LOCATION);

  IntegrationView::SetState(GetImpl(view), ViewState::FOCUS_INDICATED, true);

  DALI_TEST_EQUALS(view.GetVisualCount(OVERLAY_VISUAL_RANGE), 1u, TEST_LOCATION);
  ColorVisual overlay = GetOverlayVisual(view);
  DALI_TEST_CHECK(overlay);
  DALI_TEST_EQUALS(overlay.GetColor().GetRgba().a, 0.4f, 0.001f, TEST_LOCATION);

  ProcessTouch(application, PointState::DOWN);

  DALI_TEST_CHECK(GetOverlayVisual(view) == overlay);
  DALI_TEST_CHECK(!view.GetState().Contains(ViewState::FOCUS_INDICATED));
  DALI_TEST_CHECK(view.GetState().Contains(ViewState::PRESSED));
  DALI_TEST_EQUALS(overlay.GetColor().GetRgba().a, 0.4f, 0.001f, TEST_LOCATION);

  ProcessTouch(application, PointState::FINISHED, 120u);

  DALI_TEST_CHECK(!view.GetState().Contains(ViewState::PRESSED));
  DALI_TEST_EQUALS(view.GetVisualCount(OVERLAY_VISUAL_RANGE), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInteractiveViewOverlayEffectDisabledClearsFocusIndicatedEffectP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);

  view.SetStateEffect(OverlayEffect::Plain());

  DALI_TEST_CHECK(FocusManager::Get().SetCurrentFocusView(view));
  DALI_TEST_CHECK(view.GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(!view.GetState().Contains(ViewState::FOCUS_INDICATED));
  DALI_TEST_EQUALS(view.GetVisualCount(OVERLAY_VISUAL_RANGE), 0u, TEST_LOCATION);

  IntegrationView::SetState(GetImpl(view), ViewState::FOCUS_INDICATED, true);

  DALI_TEST_CHECK(view.GetState().Contains(ViewState::FOCUS_INDICATED));
  DALI_TEST_EQUALS(view.GetVisualCount(OVERLAY_VISUAL_RANGE), 1u, TEST_LOCATION);

  view.SetEnabled(false);

  DALI_TEST_CHECK(view.GetState().Contains(ViewState::DISABLED));
  DALI_TEST_CHECK(!view.GetState().Contains(ViewState::FOCUSED));
  DALI_TEST_CHECK(!view.GetState().Contains(ViewState::FOCUS_INDICATED));
  DALI_TEST_EQUALS(view.GetVisualCount(OVERLAY_VISUAL_RANGE), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() != view);
  END_TEST;
}

int UtcDaliInteractiveViewOverlayEffectDisabledClearsPressEffectP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);

  view.SetStateEffect(OverlayEffect::Plain());
  view.SetScale(1.2f, 0.8f);

  ProcessTouch(application, PointState::DOWN);
  FinishRecoilAnimation(application);

  DALI_TEST_CHECK(view.GetState().Contains(ViewState::PRESSED));
  DALI_TEST_EQUALS(view.GetVisualCount(OVERLAY_VISUAL_RANGE), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetCurrentScaleX(), 1.2f * OVERLAY_RECOIL_SCALE_FACTOR, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetCurrentScaleY(), 0.8f * OVERLAY_RECOIL_SCALE_FACTOR, 0.001f, TEST_LOCATION);

  view.SetEnabled(false);
  FinishRecoilAnimation(application);

  DALI_TEST_CHECK(view.GetState().Contains(ViewState::DISABLED));
  DALI_TEST_CHECK(!view.GetState().Contains(ViewState::PRESSED));
  DALI_TEST_EQUALS(view.GetVisualCount(OVERLAY_VISUAL_RANGE), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetCurrentScaleX(), 1.2f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetCurrentScaleY(), 0.8f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInteractiveViewOverlayEffectDisabledOpacityP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);

  view.SetStateEffect(OverlayEffect::Plain());
  view.SetOpacity(0.75f);

  view.SetEnabled(false);

  DALI_TEST_CHECK(view.GetState().Contains(ViewState::DISABLED));
  DALI_TEST_EQUALS(view.GetOpacity(), 0.75f * OVERLAY_DISABLED_OPACITY_FACTOR, 0.001f, TEST_LOCATION);

  view.SetEnabled(false);

  DALI_TEST_EQUALS(view.GetOpacity(), 0.75f * OVERLAY_DISABLED_OPACITY_FACTOR, 0.001f, TEST_LOCATION);

  view.SetEnabled(true);

  DALI_TEST_CHECK(!view.GetState().Contains(ViewState::DISABLED));
  DALI_TEST_EQUALS(view.GetOpacity(), 0.75f, 0.001f, TEST_LOCATION);

  view.SetEnabled(false);
  view.SetOpacity(0.8f);
  view.SetEnabled(true);

  DALI_TEST_EQUALS(view.GetOpacity(), 0.8f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInteractiveViewOverlayEffectPseudoDisabledOpacityP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);

  view.SetStateEffect(OverlayEffect::Plain());
  view.SetOpacity(0.6f);

  IntegrationView::SetState(GetImpl(view), ViewState::PSEUDO_DISABLED, true);

  DALI_TEST_CHECK(view.GetState().Contains(ViewState::PSEUDO_DISABLED));
  DALI_TEST_EQUALS(view.GetOpacity(), 0.6f * OVERLAY_DISABLED_OPACITY_FACTOR, 0.001f, TEST_LOCATION);

  IntegrationView::SetState(GetImpl(view), ViewState::PSEUDO_DISABLED, false);

  DALI_TEST_CHECK(!view.GetState().Contains(ViewState::PSEUDO_DISABLED));
  DALI_TEST_EQUALS(view.GetOpacity(), 0.6f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInteractiveViewOverlayEffectAnyDisabledOpacityRestoresWhenAllDisabledStatesClearP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);

  view.SetStateEffect(OverlayEffect::Plain());
  view.SetOpacity(0.5f);

  IntegrationView::SetState(GetImpl(view), ViewState::PSEUDO_DISABLED, true);
  view.SetEnabled(false);

  DALI_TEST_CHECK(view.GetState().Contains(ViewState::PSEUDO_DISABLED));
  DALI_TEST_CHECK(view.GetState().Contains(ViewState::DISABLED));
  DALI_TEST_EQUALS(view.GetOpacity(), 0.5f * OVERLAY_DISABLED_OPACITY_FACTOR, 0.001f, TEST_LOCATION);

  IntegrationView::SetState(GetImpl(view), ViewState::PSEUDO_DISABLED, false);

  DALI_TEST_CHECK(!view.GetState().Contains(ViewState::PSEUDO_DISABLED));
  DALI_TEST_CHECK(view.GetState().Contains(ViewState::DISABLED));
  DALI_TEST_EQUALS(view.GetOpacity(), 0.5f * OVERLAY_DISABLED_OPACITY_FACTOR, 0.001f, TEST_LOCATION);

  view.SetEnabled(true);

  DALI_TEST_CHECK(!view.GetState().Contains(ViewState::DISABLED));
  DALI_TEST_EQUALS(view.GetOpacity(), 0.5f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInteractiveViewStateEffectNoneDoesNotApplyDisabledOpacityP(void)
{
  UiTestApplication application;
  View              view = CreateTestView(application);

  view.SetStateEffect(StateEffect::None());
  view.AsInteractive();
  view.SetOpacity(0.75f);

  view.SetEnabled(false);

  DALI_TEST_CHECK(view.GetState().Contains(ViewState::DISABLED));
  DALI_TEST_EQUALS(view.GetOpacity(), 0.75f, 0.001f, TEST_LOCATION);

  IntegrationView::SetState(GetImpl(view), ViewState::PSEUDO_DISABLED, true);

  DALI_TEST_CHECK(view.GetState().Contains(ViewState::PSEUDO_DISABLED));
  DALI_TEST_EQUALS(view.GetOpacity(), 0.75f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliInteractiveViewStateEffectBeforeAsInteractiveP(void)
{
  UiTestApplication application;
  View              view = CreateTestView(application);
  view.SetStateEffect(OverlayEffect::Round());

  DALI_TEST_CHECK(!view.IsInteractive());
  view.AsInteractive();

  ProcessTouch(application, PointState::DOWN);

  ColorVisual overlay = GetOverlayVisual(view);
  DALI_TEST_CHECK(overlay);
  DALI_TEST_EQUALS(overlay.GetCornerRadius(), Vector4(0.5f, 0.5f, 0.5f, 0.5f), TEST_LOCATION);
  DALI_TEST_EQUALS(overlay.GetCornerRadiusPolicy(), CornerRadiusPolicy::RELATIVE, TEST_LOCATION);

  ProcessTouch(application, PointState::FINISHED, 120u);
  END_TEST;
}

int UtcDaliInteractiveViewStateEffectAfterAsInteractiveP(void)
{
  UiTestApplication application;
  View              view = CreateTestView(application);

  view.AsInteractive();
  view.SetStateEffect(OverlayEffect::Round());

  ProcessTouch(application, PointState::DOWN);

  ColorVisual overlay = GetOverlayVisual(view);
  DALI_TEST_CHECK(overlay);
  DALI_TEST_EQUALS(overlay.GetCornerRadius(), Vector4(0.5f, 0.5f, 0.5f, 0.5f), TEST_LOCATION);
  DALI_TEST_EQUALS(overlay.GetCornerRadiusPolicy(), CornerRadiusPolicy::RELATIVE, TEST_LOCATION);

  ProcessTouch(application, PointState::FINISHED, 120u);
  END_TEST;
}

int UtcDaliInteractiveViewStateEffectNoneP(void)
{
  UiTestApplication application;
  View              view = CreateTestView(application);

  StateEffect none = StateEffect::None();
  DALI_TEST_CHECK(none);
  DALI_TEST_CHECK(none.IsNone());

  view.SetStateEffect(none);

  DALI_TEST_CHECK(IntegrationView::GetTrait(GetImpl(view), ReservedTraitId::STATE_EFFECT));

  view.AsInteractive();
  ProcessTouch(application, PointState::DOWN);

  DALI_TEST_EQUALS(view.GetVisualCount(OVERLAY_VISUAL_RANGE), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(IntegrationView::GetTrait(GetImpl(view), ReservedTraitId::STATE_EFFECT));

  ProcessTouch(application, PointState::FINISHED, 120u);
  END_TEST;
}

int UtcDaliInteractiveViewStateEffectEmptyHandleAsNoneP(void)
{
  UiTestApplication application;
  View              view = CreateTestView(application);

  view.SetStateEffect(StateEffect());

  DALI_TEST_CHECK(IntegrationView::GetTrait(GetImpl(view), ReservedTraitId::STATE_EFFECT));

  view.AsInteractive();
  ProcessTouch(application, PointState::DOWN);

  DALI_TEST_EQUALS(view.GetVisualCount(OVERLAY_VISUAL_RANGE), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(IntegrationView::GetTrait(GetImpl(view), ReservedTraitId::STATE_EFFECT));

  ProcessTouch(application, PointState::FINISHED, 120u);
  END_TEST;
}

int UtcDaliInteractiveViewStateEffectReplaceAndNoneP(void)
{
  UiTestApplication application;
  InteractiveView   view = CreateTestInteractiveView(application);

  ProcessTouch(application, PointState::DOWN);

  DALI_TEST_EQUALS(view.GetVisualCount(OVERLAY_VISUAL_RANGE), 1u, TEST_LOCATION);

  view.SetStateEffect(StateEffect());

  DALI_TEST_EQUALS(view.GetVisualCount(OVERLAY_VISUAL_RANGE), 0u, TEST_LOCATION);

  view.SetStateEffect(OverlayEffect::Round());

  DALI_TEST_EQUALS(view.GetVisualCount(OVERLAY_VISUAL_RANGE), 1u, TEST_LOCATION);
  ColorVisual overlay = GetOverlayVisual(view);
  DALI_TEST_CHECK(overlay);
  DALI_TEST_EQUALS(overlay.GetCornerRadius(), Vector4(0.5f, 0.5f, 0.5f, 0.5f), TEST_LOCATION);
  DALI_TEST_EQUALS(overlay.GetCornerRadiusPolicy(), CornerRadiusPolicy::RELATIVE, TEST_LOCATION);

  ProcessTouch(application, PointState::FINISHED, 120u);
  END_TEST;
}

int UtcDaliInteractiveViewOverlayEffectSuppressesDefaultFocusIndicatorP(void)
{
  UiTestApplication application;
  View              view = CreateTestView(application);
  view.SetStateEffect(OverlayEffect::Plain());
  view.AsInteractive();
  view.SetFocusable(true);

  FocusManager focusManager = FocusManager::Get();
  SetFocusIndicated(view);
  focusManager.SetDefaultFocusIndicatorEnabled(true);

  DALI_TEST_EQUALS(view.GetChildCount(), 0u, TEST_LOCATION);

  focusManager.ClearFocus();
  END_TEST;
}

int UtcDaliInteractiveViewOverlayEffectStopsSuppressingDefaultFocusIndicatorP(void)
{
  UiTestApplication application;
  View              view = CreateTestView(application);
  view.SetStateEffect(OverlayEffect::Plain());
  view.AsInteractive();
  view.SetFocusable(true);

  FocusManager focusManager = FocusManager::Get();
  SetFocusIndicated(view);
  focusManager.SetDefaultFocusIndicatorEnabled(true);

  DALI_TEST_EQUALS(view.GetChildCount(), 0u, TEST_LOCATION);

  view.SetStateEffect(StateEffect::None());
  DALI_TEST_EQUALS(view.GetChildCount(), 1u, TEST_LOCATION);

  focusManager.ClearFocus();
  END_TEST;
}
