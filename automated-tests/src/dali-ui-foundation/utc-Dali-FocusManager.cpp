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

#include <stdlib.h>
#include <dali.h>
#include <dali/integration-api/events/hover-event-integ.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/extension-api/focus-manager.h>
#include <dali-ui-foundation/extension-api/focus-indication-policy.h>
#include <dali-ui-foundation/extension-api/ui-config-impl.h>
#include <dali-ui-foundation/integration-api/layouts/layout-impl.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

namespace UiExtension = Dali::Ui::Extension;

namespace
{
struct FocusIndicationPolicyCall
{
  View previousFocusView;
  View focusedView;
  FocusDevice device{FocusDevice::UNKNOWN};
  InputEventType inputEventType{InputEventType::NONE};
  bool previousFocusIndicated{false};
  bool proposedIndicated{false};
};

FocusIndicationPolicyCall gFocusIndicationPolicyCall;
int gFocusIndicationPolicyCallCount = 0;

bool RecordFocusIndicationPolicy(const UiExtension::FocusIndicationContext& context)
{
  ++gFocusIndicationPolicyCallCount;
  gFocusIndicationPolicyCall.previousFocusView = context.previousFocusView;
  gFocusIndicationPolicyCall.focusedView = context.focusedView;
  gFocusIndicationPolicyCall.device = context.device;
  gFocusIndicationPolicyCall.inputEventType = context.inputEvent.GetInputEventType();
  gFocusIndicationPolicyCall.previousFocusIndicated = context.previousFocusIndicated;
  gFocusIndicationPolicyCall.proposedIndicated = context.proposedIndicated;
  return context.proposedIndicated;
}

bool NeverIndicateFocus(const UiExtension::FocusIndicationContext& context)
{
  RecordFocusIndicationPolicy(context);
  return false;
}

Dali::Integration::TouchEvent GenerateFocusManagerTouch(PointState::Type state, const Vector2& screenPosition)
{
  Dali::Integration::TouchEvent touchEvent;
  Dali::Integration::Point point;
  point.SetState(state);
  point.SetDeviceId(1);
  point.SetScreenPosition(screenPosition);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);
  touchEvent.time = 100u;
  return touchEvent;
}

Dali::Integration::HoverEvent GenerateFocusManagerHover(PointState::Type state, const Vector2& screenPosition)
{
  Dali::Integration::HoverEvent hoverEvent(100u);
  Dali::Integration::Point point;
  point.SetState(state);
  point.SetDeviceId(2);
  point.SetScreenPosition(screenPosition);
  point.SetDeviceClass(Device::Class::MOUSE);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  hoverEvent.points.push_back(point);
  return hoverEvent;
}
} // namespace

int UtcDaliFocusManagerFocusIndicationPolicyContextP(void)
{
  UiConfig config = UiConfig::New();
  DALI_TEST_CHECK(GetImpl(config).GetFocusIndicationPolicy() == &UiExtension::FocusIndicationPolicy::Default);
  GetImpl(config).SetFocusIndicationPolicy(nullptr);
  DALI_TEST_CHECK(GetImpl(config).GetFocusIndicationPolicy() == &UiExtension::FocusIndicationPolicy::Default);
  GetImpl(config).SetFocusIndicationPolicy(&RecordFocusIndicationPolicy);
  DALI_TEST_CHECK(GetImpl(config).GetFocusIndicationPolicy() == &RecordFocusIndicationPolicy);

  UiTestApplication application(config);

  View first = View::New();
  first.SetFocusable(true);
  View second = View::New();
  second.SetFocusable(true);
  application.GetScene().Add(first);
  application.GetScene().Add(second);
  application.SendNotification();
  application.Render();

  gFocusIndicationPolicyCallCount = 0;
  FocusManager focusManager = FocusManager::Get();
  DALI_TEST_CHECK(focusManager.SetCurrentFocusView(first));
  DALI_TEST_CHECK(gFocusIndicationPolicyCallCount == 1);
  DALI_TEST_CHECK(!gFocusIndicationPolicyCall.previousFocusView);
  DALI_TEST_CHECK(gFocusIndicationPolicyCall.focusedView == first);
  DALI_TEST_CHECK(gFocusIndicationPolicyCall.device == FocusDevice::PROGRAMMATIC);
  DALI_TEST_CHECK(gFocusIndicationPolicyCall.inputEventType == InputEventType::NONE);
  DALI_TEST_CHECK(!gFocusIndicationPolicyCall.previousFocusIndicated);
  DALI_TEST_CHECK(!gFocusIndicationPolicyCall.proposedIndicated);

  DALI_TEST_CHECK(focusManager.SetCurrentFocusView(second));
  DALI_TEST_CHECK(gFocusIndicationPolicyCallCount == 2);
  DALI_TEST_CHECK(gFocusIndicationPolicyCall.previousFocusView == first);
  DALI_TEST_CHECK(gFocusIndicationPolicyCall.focusedView == second);
  DALI_TEST_CHECK(!gFocusIndicationPolicyCall.previousFocusIndicated);
  DALI_TEST_CHECK(!gFocusIndicationPolicyCall.proposedIndicated);

  Dali::Integration::KeyEvent keyDown(
    "Right", "", "", 0, 0, 100u, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::KEYBOARD, Device::Subclass::NONE);
  application.ProcessEvent(keyDown);

  DALI_TEST_CHECK(gFocusIndicationPolicyCallCount == 3);
  DALI_TEST_CHECK(gFocusIndicationPolicyCall.previousFocusView == second);
  DALI_TEST_CHECK(gFocusIndicationPolicyCall.focusedView == second);
  DALI_TEST_CHECK(gFocusIndicationPolicyCall.device == FocusDevice::KEYBOARD);
  DALI_TEST_CHECK(gFocusIndicationPolicyCall.inputEventType == InputEventType::KEY_EVENT);
  DALI_TEST_CHECK(!gFocusIndicationPolicyCall.previousFocusIndicated);
  DALI_TEST_CHECK(gFocusIndicationPolicyCall.proposedIndicated);
  DALI_TEST_CHECK(GetImpl(second).GetState().Contains(ViewState::FOCUS_INDICATED));

  focusManager.ClearFocusIndication();
  DALI_TEST_CHECK(gFocusIndicationPolicyCallCount == 3);
  DALI_TEST_CHECK(!GetImpl(second).GetState().Contains(ViewState::FOCUS_INDICATED));

  gFocusIndicationPolicyCall.previousFocusView.Reset();
  gFocusIndicationPolicyCall.focusedView.Reset();

  END_TEST;
}

int UtcDaliFocusManagerDefaultIndicatorOverridePreservesConfiguredPolicyP(void)
{
  UiConfig config = UiConfig::New();
  GetImpl(config).SetFocusIndicationPolicy(&RecordFocusIndicationPolicy);
  UiTestApplication application(config);

  FocusManager focusManager = FocusManager::Get();
  focusManager.SetDefaultFocusIndicatorEnabled(false);
  DALI_TEST_CHECK(!focusManager.IsDefaultFocusIndicatorEnabled());

  View view = View::New();
  view.SetFocusable(true);
  application.GetScene().Add(view);

  gFocusIndicationPolicyCallCount = 0;
  DALI_TEST_CHECK(focusManager.SetCurrentFocusView(view));
  DALI_TEST_CHECK(gFocusIndicationPolicyCallCount == 1);

  END_TEST;
}

int UtcDaliFocusManagerTouchOverridePreservesConfiguredPolicyP(void)
{
  UiConfig config = UiConfig::New();
  GetImpl(config).SetFocusIndicationPolicy(&RecordFocusIndicationPolicy);
  UiTestApplication application(config);

  FocusManager focusManager = FocusManager::Get();
  focusManager.SetClearFocusIndicationOnTouch(false);
  DALI_TEST_CHECK(!focusManager.IsClearFocusIndicationOnTouchEnabled());

  View view = View::New();
  view.SetFocusable(true);
  application.GetScene().Add(view);

  gFocusIndicationPolicyCallCount = 0;
  DALI_TEST_CHECK(focusManager.SetCurrentFocusView(view));
  DALI_TEST_CHECK(gFocusIndicationPolicyCallCount == 1);

  END_TEST;
}

int UtcDaliFocusManagerHoverOverridePreservesConfiguredPolicyP(void)
{
  UiConfig config = UiConfig::New();
  GetImpl(config).SetFocusIndicationPolicy(&RecordFocusIndicationPolicy);
  UiTestApplication application(config);

  FocusManager focusManager = FocusManager::Get();
  focusManager.SetClearFocusIndicationOnHover(true);
  DALI_TEST_CHECK(focusManager.IsClearFocusIndicationOnHoverEnabled());

  View view = View::New();
  view.SetFocusable(true);
  application.GetScene().Add(view);

  gFocusIndicationPolicyCallCount = 0;
  DALI_TEST_CHECK(focusManager.SetCurrentFocusView(view));
  DALI_TEST_CHECK(gFocusIndicationPolicyCallCount == 1);

  END_TEST;
}

int UtcDaliFocusManagerNavigationIgnoresFocusIndicationP(void)
{
  UiConfig config = UiConfig::New();
  GetImpl(config).SetFocusIndicationPolicy(&NeverIndicateFocus);

  UiTestApplication application(config);

  View current = View::New();
  current.SetFocusable(true);
  View next = View::New();
  next.SetFocusable(true);
  current.SetRightFocusableView(next);
  application.GetScene().Add(current);
  application.GetScene().Add(next);
  application.SendNotification();
  application.Render();

  FocusManager focusManager = FocusManager::Get();
  DALI_TEST_CHECK(focusManager.SetCurrentFocusView(current));
  DALI_TEST_CHECK(!GetImpl(current).GetState().Contains(ViewState::FOCUS_INDICATED));

  gFocusIndicationPolicyCallCount = 0;
  gFocusIndicationPolicyCall.previousFocusView.Reset();
  gFocusIndicationPolicyCall.focusedView.Reset();

  Dali::Integration::KeyEvent keyDown(
    "Right", "", "", 0, 0, 100u, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::KEYBOARD, Device::Subclass::NONE);
  application.ProcessEvent(keyDown);

  DALI_TEST_CHECK(focusManager.GetCurrentFocusView() == next);
  DALI_TEST_CHECK(gFocusIndicationPolicyCallCount == 1);
  DALI_TEST_CHECK(gFocusIndicationPolicyCall.previousFocusView == current);
  DALI_TEST_CHECK(gFocusIndicationPolicyCall.focusedView == next);
  DALI_TEST_CHECK(gFocusIndicationPolicyCall.device == FocusDevice::KEYBOARD);
  DALI_TEST_CHECK(gFocusIndicationPolicyCall.inputEventType == InputEventType::KEY_EVENT);
  DALI_TEST_CHECK(!gFocusIndicationPolicyCall.previousFocusIndicated);
  DALI_TEST_CHECK(gFocusIndicationPolicyCall.proposedIndicated);
  DALI_TEST_CHECK(!GetImpl(next).GetState().Contains(ViewState::FOCUS_INDICATED));

  gFocusIndicationPolicyCall.previousFocusView.Reset();
  gFocusIndicationPolicyCall.focusedView.Reset();

  END_TEST;
}

int UtcDaliFocusManagerFollowFocusPolicyMovesImmediatelyP(void)
{
  UiConfig config = UiConfig::New();
  config.SetClearFocusIndicationOnHover(true);
  GetImpl(config).SetFocusIndicationPolicy(&UiExtension::FocusIndicationPolicy::FollowFocus);
  DALI_TEST_CHECK(GetImpl(config).GetFocusIndicationPolicy() == &UiExtension::FocusIndicationPolicy::FollowFocus);

  UiTestApplication application(config);

  View current = View::New();
  current.SetFocusable(true);
  current.SetPivot(Pivot::TOP_LEFT);
  current.SetParentOrigin(ParentOrigin::TOP_LEFT);
  current.SetRequestedWidth(100.0f);
  current.SetRequestedHeight(100.0f);
  current.TouchEventSignal().Connect([](Actor, TouchEvent) { return false; });
  View next = View::New();
  next.SetFocusable(true);
  next.SetPivot(Pivot::TOP_LEFT);
  next.SetParentOrigin(ParentOrigin::TOP_LEFT);
  next.SetRequestedX(200.0f);
  next.SetRequestedY(200.0f);
  next.SetRequestedWidth(100.0f);
  next.SetRequestedHeight(100.0f);
  next.HoverEventSignal().Connect([](Actor, HoverEvent) { return false; });
  current.SetRightFocusableView(next);
  application.GetScene().Add(current);
  application.GetScene().Add(next);
  application.SendNotification();
  application.Render();

  FocusManager focusManager = FocusManager::Get();
  DALI_TEST_CHECK(focusManager.SetCurrentFocusView(current));
  DALI_TEST_CHECK(GetImpl(current).GetState().Contains(ViewState::FOCUS_INDICATED));

  application.ProcessEvent(GenerateFocusManagerTouch(PointState::DOWN, Vector2(50.0f, 50.0f)));
  DALI_TEST_CHECK(GetImpl(current).GetState().Contains(ViewState::FOCUS_INDICATED));

  application.ProcessEvent(GenerateFocusManagerHover(PointState::STARTED, Vector2(250.0f, 250.0f)));
  DALI_TEST_CHECK(GetImpl(current).GetState().Contains(ViewState::FOCUS_INDICATED));

  Dali::Integration::KeyEvent keyDown(
    "Right", "", "", 0, 0, 100u, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::KEYBOARD, Device::Subclass::NONE);
  application.ProcessEvent(keyDown);

  DALI_TEST_CHECK(focusManager.GetCurrentFocusView() == next);
  DALI_TEST_CHECK(GetImpl(next).GetState().Contains(ViewState::FOCUS_INDICATED));

  END_TEST;
}

// ============================================================
// Extension key-input target
// ============================================================

int UtcDaliFocusManagerExtensionSetAndClearKeyInputTargetP(void)
{
  UiTestApplication application;

  View navigationFocus = View::New();
  navigationFocus.SetFocusable(true);
  View firstTarget = View::New();
  View secondTarget = View::New();
  application.GetScene().Add(navigationFocus);
  application.GetScene().Add(firstTarget);
  application.GetScene().Add(secondTarget);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(Ui::FocusManager::Get().SetCurrentFocusView(navigationFocus));
  DALI_TEST_CHECK(Ui::Extension::FocusManager::SetKeyInputTarget(firstTarget));
  DALI_TEST_CHECK(Ui::Extension::FocusManager::IsKeyInputTarget(firstTarget));
  DALI_TEST_CHECK(!Ui::Extension::FocusManager::IsKeyInputTarget(secondTarget));
  DALI_TEST_CHECK(Ui::FocusManager::Get().GetCurrentFocusView() == navigationFocus);

  DALI_TEST_CHECK(!Ui::Extension::FocusManager::ClearKeyInputTarget(secondTarget));
  DALI_TEST_CHECK(Ui::Extension::FocusManager::SetKeyInputTarget(secondTarget));
  DALI_TEST_CHECK(!Ui::Extension::FocusManager::IsKeyInputTarget(firstTarget));
  DALI_TEST_CHECK(Ui::Extension::FocusManager::IsKeyInputTarget(secondTarget));
  DALI_TEST_CHECK(Ui::FocusManager::Get().GetCurrentFocusView() == navigationFocus);

  DALI_TEST_CHECK(Ui::Extension::FocusManager::ClearKeyInputTarget(secondTarget));
  DALI_TEST_CHECK(!Ui::Extension::FocusManager::IsKeyInputTarget(secondTarget));
  DALI_TEST_CHECK(!Ui::Extension::FocusManager::ClearKeyInputTarget(secondTarget));
  DALI_TEST_CHECK(!Ui::Extension::FocusManager::ClearKeyInputTarget(firstTarget));
  END_TEST;
}

int UtcDaliFocusManagerExtensionSetKeyInputTargetDisconnectedN(void)
{
  UiTestApplication application;

  View disconnectedView = View::New();

  DALI_TEST_CHECK(!Ui::Extension::FocusManager::SetKeyInputTarget(disconnectedView));
  DALI_TEST_CHECK(!Ui::Extension::FocusManager::SetKeyInputTarget(View()));
  DALI_TEST_CHECK(!Ui::Extension::FocusManager::ClearKeyInputTarget(View()));
  DALI_TEST_CHECK(!Ui::Extension::FocusManager::IsKeyInputTarget(View()));
  END_TEST;
}

// ============================================================
// RequestFocus — basic
// ============================================================

int UtcDaliFocusManagerRequestFocusBasicP(void)
{
  UiTestApplication application;

  View view = View::New();
  view.SetFocusable(true);
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  bool result = FocusManager::Get().RequestFocus(view);
  DALI_TEST_CHECK(result);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == view);
  END_TEST;
}

int UtcDaliFocusManagerRequestFocusNonFocusableN(void)
{
  UiTestApplication application;

  View view = View::New(); // focusable=false by default
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  bool result = FocusManager::Get().RequestFocus(view);
  DALI_TEST_CHECK(!result);
  END_TEST;
}

// ============================================================
// RequestFocus — descendant delegation (Layout)
// ============================================================

int UtcDaliFocusManagerRequestFocusOnLayoutDelegatesToChildP(void)
{
  UiTestApplication application;

  auto layout = Layout::New();
  View child1 = View::New(); // not focusable
  View child2 = View::New();
  child2.SetFocusable(true);
  layout.Add(child1);
  layout.Add(child2);
  application.GetScene().Add(layout);
  application.SendNotification();
  application.Render();

  bool result = FocusManager::Get().RequestFocus(layout);
  DALI_TEST_CHECK(result);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == child2);
  END_TEST;
}

int UtcDaliFocusManagerRequestFocusOnLayoutNoFocusableChildN(void)
{
  UiTestApplication application;

  auto layout = Layout::New();
  View child1 = View::New(); // not focusable
  View child2 = View::New(); // not focusable
  layout.Add(child1);
  layout.Add(child2);
  application.GetScene().Add(layout);
  application.SendNotification();
  application.Render();

  // Layout itself is also not focusable by default
  bool result = FocusManager::Get().RequestFocus(layout);
  DALI_TEST_CHECK(!result);
  END_TEST;
}

int UtcDaliFocusManagerRequestFocusOnLayoutFallbackToSelfP(void)
{
  UiTestApplication application;

  // Layout (focusable) with depth-2 tree of 6 descendants.
  // None qualifies for focus (mix of not-focusable, disabled, invisible).
  auto layout = Layout::New();
  layout.SetFocusable(true);

  View child1 = View::New();                                          // not focusable
  View child2 = View::New();
  child2.SetFocusable(true);
  child2.SetEnabled(false); // disabled
  View child3 = View::New();
  child3.SetFocusable(true);
  child3.SetVisible(false); // invisible

  auto nested = Layout::New();                                        // sub-layout, not focusable
  View grand1 = View::New();
  grand1.SetFocusable(true);
  grand1.SetEnabled(false); // disabled
  View grand2 = View::New();
  grand2.SetFocusable(true);
  grand2.SetVisible(false); // invisible

  nested.Add(grand1);
  nested.Add(grand2);
  layout.Add(child1);
  layout.Add(child2);
  layout.Add(child3);
  layout.Add(nested);
  application.GetScene().Add(layout);
  application.SendNotification();
  application.Render();

  // No eligible descendant → layout itself should take focus
  bool result = FocusManager::Get().RequestFocus(layout);
  DALI_TEST_CHECK(result);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == layout);
  END_TEST;
}

// ============================================================
// DescendantFocusBlocked
// ============================================================

int UtcDaliFocusManagerDescendantFocusBlockedP(void)
{
  UiTestApplication application;

  auto layout = Layout::New();
  layout.SetAllowDescendantFocusEnabled(false);
  View child = View::New();
  child.SetFocusable(true);
  layout.Add(child);
  application.GetScene().Add(layout);
  application.SendNotification();
  application.Render();

  // Child should not get focus because parent blocks descendants
  bool result = FocusManager::Get().RequestFocus(child);
  DALI_TEST_CHECK(!result);
  END_TEST;
}

int UtcDaliFocusManagerDescendantFocusBlockedGetterP(void)
{
  UiTestApplication application;

  View view = View::New();
  DALI_TEST_CHECK(view.IsAllowDescendantFocusEnabled());
  view.SetAllowDescendantFocusEnabled(false);
  DALI_TEST_CHECK(!view.IsAllowDescendantFocusEnabled());
  view.SetAllowDescendantFocusEnabled(true);
  DALI_TEST_CHECK(view.IsAllowDescendantFocusEnabled());
  END_TEST;
}

// ============================================================
// FocusGroup containment
// ============================================================

int UtcDaliFocusManagerFocusGroupContainmentP(void)
{
  UiTestApplication application;

  View group = View::New();
  group.SetFocusable(false);
  FocusManager::Get().SetAsFocusGroup(group, true);
  View insideA = View::New();
  insideA.SetFocusable(true);
  insideA.SetRequestedY(0.0f);
  insideA.SetRequestedWidth(50.0f);
  insideA.SetRequestedHeight(50.0f);
  View insideB = View::New();
  insideB.SetFocusable(true);
  insideB.SetRequestedY(60.0f);
  insideB.SetRequestedWidth(50.0f);
  insideB.SetRequestedHeight(50.0f);
  group.Add(insideA);
  group.Add(insideB);

  View outside = View::New();
  outside.SetFocusable(true);
  outside.SetRequestedY(120.0f);
  outside.SetRequestedWidth(50.0f);
  outside.SetRequestedHeight(50.0f);

  application.GetScene().Add(group);
  application.GetScene().Add(outside);
  application.SendNotification();
  application.Render();

  // Focus inside group first
  FocusManager::Get().RequestFocus(insideA);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == insideA);

  // MoveFocus should be scoped to the focus group.
  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::DOWN));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == insideB);
  DALI_TEST_CHECK(!FocusManager::Get().MoveFocus(FocusDirection::DOWN));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == insideB);

  // Direct RequestFocus to outside should work (programmatic path has no containment)
  FocusManager::Get().RequestFocus(outside);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == outside);
  END_TEST;
}

int UtcDaliFocusManagerFocusGroupForwardBackwardContainmentP(void)
{
  UiTestApplication application;

  View group = View::New();
  FocusManager::Get().SetAsFocusGroup(group, true);

  View insideA = View::New();
  insideA.SetFocusable(true);
  insideA.SetRequestedX(0.0f);
  insideA.SetRequestedWidth(50.0f);
  insideA.SetRequestedHeight(50.0f);
  View insideB = View::New();
  insideB.SetFocusable(true);
  insideB.SetRequestedX(60.0f);
  insideB.SetRequestedWidth(50.0f);
  insideB.SetRequestedHeight(50.0f);
  View outside = View::New();
  outside.SetFocusable(true);
  outside.SetRequestedX(120.0f);
  outside.SetRequestedWidth(50.0f);
  outside.SetRequestedHeight(50.0f);

  group.Add(insideA);
  group.Add(insideB);
  application.GetScene().Add(group);
  application.GetScene().Add(outside);
  application.SendNotification();
  application.Render();

  FocusManager::Get().RequestFocus(insideA);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == insideA);

  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::FORWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == insideB);
  DALI_TEST_CHECK(!FocusManager::Get().MoveFocus(FocusDirection::FORWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == insideB);

  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::BACKWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == insideA);
  DALI_TEST_CHECK(!FocusManager::Get().MoveFocus(FocusDirection::BACKWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == insideA);
  END_TEST;
}

namespace
{
View gOuterNavigationTarget;
int  gOuterNavigationCallbackCount = 0;

View OuterNavigationCallbackFunc(View, FocusDirection)
{
  ++gOuterNavigationCallbackCount;
  return gOuterNavigationTarget;
}
} // namespace

int UtcDaliFocusManagerFocusGroupStopsParentNavigationP(void)
{
  UiTestApplication application;

  View outer   = View::New();
  View group   = View::New();
  View current = View::New();
  current.SetFocusable(true);
  current.SetRequestedX(0.0f);
  current.SetRequestedWidth(50.0f);
  current.SetRequestedHeight(50.0f);
  View inside = View::New();
  inside.SetFocusable(true);
  inside.SetRequestedX(60.0f);
  inside.SetRequestedWidth(50.0f);
  inside.SetRequestedHeight(50.0f);
  View outside = View::New();
  outside.SetFocusable(true);
  outside.SetRequestedX(120.0f);
  outside.SetRequestedWidth(50.0f);
  outside.SetRequestedHeight(50.0f);

  FocusManager::Get().SetAsFocusGroup(group, true);
  group.Add(current);
  group.Add(inside);
  outer.Add(group);
  outer.Add(outside);
  application.GetScene().Add(outer);
  application.SendNotification();
  application.Render();

  gOuterNavigationCallbackCount = 0;
  gOuterNavigationTarget        = outside;
  outer.SetFocusNavigationCallback(
    FocusNavigationCallback::New(&OuterNavigationCallbackFunc));

  FocusManager::Get().RequestFocus(current);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == current);

  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::FORWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == inside);
  DALI_TEST_CHECK(gOuterNavigationCallbackCount == 0);

  gOuterNavigationTarget.Reset();
  END_TEST;
}

// ============================================================
// SetCurrentFocusView — direct (no delegation)
// ============================================================

int UtcDaliFocusManagerSetCurrentFocusViewDirectP(void)
{
  UiTestApplication application;

  View view = View::New();
  view.SetFocusable(true);
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  // SetCurrentFocusView directly sets focus without delegation
  bool result = FocusManager::Get().SetCurrentFocusView(view);
  DALI_TEST_CHECK(result);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == view);
  END_TEST;
}

int UtcDaliFocusManagerSetCurrentFocusViewOnLayoutNoDelegationN(void)
{
  UiTestApplication application;

  auto layout = Layout::New(); // not focusable by default
  View child = View::New();
  child.SetFocusable(true);
  layout.Add(child);
  application.GetScene().Add(layout);
  application.SendNotification();
  application.Render();

  // SetCurrentFocusView does NOT delegate to children — layout is not focusable, so it fails
  bool result = FocusManager::Get().SetCurrentFocusView(layout);
  DALI_TEST_CHECK(!result);
  END_TEST;
}

// ============================================================
// FocusNavigationCallback
// ============================================================

namespace
{
View gCallbackTarget;

View FocusNavigationCallbackFunc(View, FocusDirection)
{
  return gCallbackTarget;
}
} // namespace

int UtcDaliFocusManagerFocusNavigationCallbackP(void)
{
  UiTestApplication application;

  View parent = View::New();
  View child1 = View::New();
  child1.SetFocusable(true);
  View child2 = View::New();
  child2.SetFocusable(true);
  parent.Add(child1);
  parent.Add(child2);
  application.GetScene().Add(parent);
  application.SendNotification();
  application.Render();

  gCallbackTarget = child2;
  parent.SetFocusNavigationCallback(
    FocusNavigationCallback::New(&FocusNavigationCallbackFunc));

  FocusManager::Get().RequestFocus(child1);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == child1);

  bool result = FocusManager::Get().MoveFocus(FocusDirection::RIGHT);
  DALI_TEST_CHECK(result);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == child2);

  gCallbackTarget.Reset();
  END_TEST;
}

// ============================================================
// MoveFocus step priority
// ============================================================
// MoveFocus resolves the next focus target in this order:
//   Step 1: Parent chain navigation (FocusNavigationCallback)
//   Step 2: Directional property (e.g. RIGHT_FOCUSABLE_VIEW_ID)
//   Step 3: FocusFinder (geometry-based or linear ordering)
//
// The following tests verify that a higher-priority step always
// wins over lower ones, and that removing a step falls through
// to the next.

namespace
{
View gPriorityCallbackTarget;

View PriorityCallbackFunc(View, FocusDirection)
{
  return gPriorityCallbackTarget;
}
} // namespace

int UtcDaliFocusManagerMoveFocusPriorityStep1WinsOverStep2And3P(void)
{
  UiTestApplication application;

  // Layout with the focused view and three candidates.
  //
  //   [current]  [viewProp]        ← Step 2 target (directional property)
  //              [viewCallback]    ← Step 1 target (parent callback)
  //              [viewFinder]      ← Step 3 target (FocusFinder geometry, nearest right)
  //
  View current = View::New();
  current.SetFocusable(true);
  current.SetRequestedX(0.0f);
  current.SetRequestedY(0.0f);
  current.SetRequestedWidth(50.0f);
  current.SetRequestedHeight(50.0f);
  View viewProp = View::New();
  viewProp.SetFocusable(true);
  viewProp.SetRequestedX(200.0f);
  viewProp.SetRequestedY(0.0f);
  viewProp.SetRequestedWidth(50.0f);
  viewProp.SetRequestedHeight(50.0f);
  View viewCallback = View::New();
  viewCallback.SetFocusable(true);
  viewCallback.SetRequestedX(200.0f);
  viewCallback.SetRequestedY(60.0f);
  viewCallback.SetRequestedWidth(50.0f);
  viewCallback.SetRequestedHeight(50.0f);
  View viewFinder = View::New();
  viewFinder.SetFocusable(true);
  viewFinder.SetRequestedX(60.0f);
  viewFinder.SetRequestedY(0.0f);
  viewFinder.SetRequestedWidth(50.0f);
  viewFinder.SetRequestedHeight(50.0f);

  auto layout = Layout::New();
  layout.Add(current);
  layout.Add(viewProp);
  layout.Add(viewCallback);
  layout.Add(viewFinder);
  application.GetScene().Add(layout);
  application.SendNotification();
  application.Render();

  // Step 1: parent callback → viewCallback
  gPriorityCallbackTarget = viewCallback;
  layout.SetFocusNavigationCallback(
    FocusNavigationCallback::New(&PriorityCallbackFunc));

  // Step 2: directional property — current RIGHT → viewProp
  current.SetRightFocusableView(viewProp);

  // Step 3: FocusFinder would find viewFinder (nearest to the right)

  FocusManager::Get().RequestFocus(current);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == current);

  // MoveFocus RIGHT — Step 1 should win
  bool result = FocusManager::Get().MoveFocus(FocusDirection::RIGHT);
  DALI_TEST_CHECK(result);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == viewCallback);

  gPriorityCallbackTarget.Reset();
  END_TEST;
}

int UtcDaliFocusManagerMoveFocusPriorityStep2WinsOverStep3P(void)
{
  UiTestApplication application;

  // Same layout but NO parent callback set.
  View current = View::New();
  current.SetFocusable(true);
  current.SetRequestedX(0.0f);
  current.SetRequestedY(0.0f);
  current.SetRequestedWidth(50.0f);
  current.SetRequestedHeight(50.0f);
  View viewProp = View::New();
  viewProp.SetFocusable(true);
  viewProp.SetRequestedX(200.0f);
  viewProp.SetRequestedY(0.0f);
  viewProp.SetRequestedWidth(50.0f);
  viewProp.SetRequestedHeight(50.0f);
  View viewFinder = View::New();
  viewFinder.SetFocusable(true);
  viewFinder.SetRequestedX(60.0f);
  viewFinder.SetRequestedY(0.0f);
  viewFinder.SetRequestedWidth(50.0f);
  viewFinder.SetRequestedHeight(50.0f);

  View parent = View::New();
  parent.Add(current);
  parent.Add(viewProp);
  parent.Add(viewFinder);
  application.GetScene().Add(parent);
  application.SendNotification();
  application.Render();

  // Step 1: no parent callback
  // Step 2: directional property — current RIGHT → viewProp
  current.SetRightFocusableView(viewProp);

  // Step 3: FocusFinder would find viewFinder (nearest to the right)

  FocusManager::Get().RequestFocus(current);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == current);

  // MoveFocus RIGHT — Step 2 should win (Step 1 absent)
  bool result = FocusManager::Get().MoveFocus(FocusDirection::RIGHT);
  DALI_TEST_CHECK(result);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == viewProp);

  gPriorityCallbackTarget.Reset();
  END_TEST;
}

int UtcDaliFocusManagerMoveFocusPriorityStep3FallbackP(void)
{
  UiTestApplication application;

  // No directional property, no callback — only FocusFinder.
  View current = View::New();
  current.SetFocusable(true);
  current.SetRequestedX(0.0f);
  current.SetRequestedY(0.0f);
  current.SetRequestedWidth(50.0f);
  current.SetRequestedHeight(50.0f);
  View viewFinder = View::New();
  viewFinder.SetFocusable(true);
  viewFinder.SetRequestedX(60.0f);
  viewFinder.SetRequestedY(0.0f);
  viewFinder.SetRequestedWidth(50.0f);
  viewFinder.SetRequestedHeight(50.0f);

  View parent = View::New();
  parent.Add(current);
  parent.Add(viewFinder);
  application.GetScene().Add(parent);
  application.SendNotification();
  application.Render();

  // Step 1: no directional property
  // Step 2: no callback
  // Step 3: FocusFinder finds viewFinder (only candidate to the right)

  FocusManager::Get().RequestFocus(current);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == current);

  bool result = FocusManager::Get().MoveFocus(FocusDirection::RIGHT);
  DALI_TEST_CHECK(result);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == viewFinder);
  END_TEST;
}

int UtcDaliFocusManagerMoveFocusPriorityAllStepsFailN(void)
{
  UiTestApplication application;

  // current is the only focusable view — no candidate in any step.
  View current = View::New();
  current.SetFocusable(true);
  current.SetRequestedX(0.0f);
  current.SetRequestedY(0.0f);
  current.SetRequestedWidth(50.0f);
  current.SetRequestedHeight(50.0f);

  View parent = View::New();
  parent.Add(current);
  application.GetScene().Add(parent);
  application.SendNotification();
  application.Render();

  FocusManager::Get().RequestFocus(current);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == current);

  bool result = FocusManager::Get().MoveFocus(FocusDirection::RIGHT);
  DALI_TEST_CHECK(!result);
  // Focus should remain on current
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == current);
  END_TEST;
}

// ============================================================
// Directional focus properties
// ============================================================

int UtcDaliFocusManagerDirectionalPropertyP(void)
{
  UiTestApplication application;

  View view1 = View::New();
  view1.SetFocusable(true);
  View view2 = View::New();
  view2.SetFocusable(true);
  view1.SetRightFocusableView(view2);
  application.GetScene().Add(view1);
  application.GetScene().Add(view2);
  application.SendNotification();
  application.Render();

  FocusManager::Get().RequestFocus(view1);
  bool result = FocusManager::Get().MoveFocus(FocusDirection::RIGHT);
  DALI_TEST_CHECK(result);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == view2);
  END_TEST;
}

int UtcDaliFocusManagerMoveFocusRejectsBlockedAncestorCandidateN(void)
{
  UiTestApplication application;

  View current = View::New();
  current.SetFocusable(true);
  View blockedParent  = View::New();
  View blockedChild = View::New();
  blockedChild.SetFocusable(true);
  View fallbackFinder = View::New();
  fallbackFinder.SetFocusable(true);
  fallbackFinder.SetRequestedX(60.0f);
  fallbackFinder.SetRequestedWidth(50.0f);
  fallbackFinder.SetRequestedHeight(50.0f);

  blockedParent.SetAllowDescendantFocusEnabled(false);
  blockedParent.Add(blockedChild);

  current.SetRightFocusableView(blockedChild);

  application.GetScene().Add(current);
  application.GetScene().Add(blockedParent);
  application.GetScene().Add(fallbackFinder);
  application.SendNotification();
  application.Render();

  FocusManager::Get().RequestFocus(current);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == current);

  DALI_TEST_CHECK(!FocusManager::Get().MoveFocus(FocusDirection::RIGHT));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == current);
  END_TEST;
}

// ============================================================
// FORWARD / BACKWARD focusable view ID
// ============================================================

int UtcDaliFocusManagerForwardFocusableViewIdP(void)
{
  UiTestApplication application;

  View view1 = View::New();
  view1.SetFocusable(true);
  View view2 = View::New();
  view2.SetFocusable(true);
  view1.SetForwardFocusableView(view2);
  application.GetScene().Add(view1);
  application.GetScene().Add(view2);
  application.SendNotification();
  application.Render();

  FocusManager::Get().RequestFocus(view1);
  bool result = FocusManager::Get().MoveFocus(FocusDirection::FORWARD);
  DALI_TEST_CHECK(result);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == view2);
  END_TEST;
}

int UtcDaliFocusManagerBackwardFocusableViewIdP(void)
{
  UiTestApplication application;

  View view1 = View::New();
  view1.SetFocusable(true);
  View view2 = View::New();
  view2.SetFocusable(true);
  view2.SetBackwardFocusableView(view1);
  application.GetScene().Add(view1);
  application.GetScene().Add(view2);
  application.SendNotification();
  application.Render();

  FocusManager::Get().RequestFocus(view2);
  bool result = FocusManager::Get().MoveFocus(FocusDirection::BACKWARD);
  DALI_TEST_CHECK(result);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == view1);
  END_TEST;
}

// ============================================================
// FORWARD / BACKWARD — FocusFinder linear ordering
// ============================================================

int UtcDaliFocusManagerForwardRootLayerOrderingP(void)
{
  UiTestApplication application;

  View parentA = View::New();
  parentA.SetRequestedX(0.0f);
  parentA.SetRequestedWidth(120.0f);
  parentA.SetRequestedHeight(50.0f);
  View parentB = View::New();
  parentB.SetRequestedX(140.0f);
  parentB.SetRequestedWidth(120.0f);
  parentB.SetRequestedHeight(50.0f);
  View a1 = View::New();
  a1.SetFocusable(true);
  a1.SetRequestedX(0.0f);
  a1.SetRequestedWidth(50.0f);
  a1.SetRequestedHeight(50.0f);
  View a2 = View::New();
  a2.SetFocusable(true);
  a2.SetRequestedX(60.0f);
  a2.SetRequestedWidth(50.0f);
  a2.SetRequestedHeight(50.0f);
  View b1 = View::New();
  b1.SetFocusable(true);
  b1.SetRequestedX(0.0f);
  b1.SetRequestedWidth(50.0f);
  b1.SetRequestedHeight(50.0f);

  parentA.Add(a1);
  parentA.Add(a2);
  parentB.Add(b1);
  application.GetScene().Add(parentA);
  application.GetScene().Add(parentB);
  application.SendNotification();
  application.Render();

  FocusManager::Get().RequestFocus(a1);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == a1);

  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::FORWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == a2);

  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::FORWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == b1);

  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::BACKWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == a2);
  END_TEST;
}

int UtcDaliFocusManagerForwardSiblingsP(void)
{
  UiTestApplication application;

  // Three focusable siblings in a plain View container.
  View parent = View::New();
  View v1 = View::New();
  v1.SetFocusable(true);
  v1.SetRequestedX(0.0f);
  v1.SetRequestedWidth(50.0f);
  v1.SetRequestedHeight(50.0f);
  View v2 = View::New();
  v2.SetFocusable(true);
  v2.SetRequestedX(60.0f);
  v2.SetRequestedWidth(50.0f);
  v2.SetRequestedHeight(50.0f);
  View v3 = View::New();
  v3.SetFocusable(true);
  v3.SetRequestedX(120.0f);
  v3.SetRequestedWidth(50.0f);
  v3.SetRequestedHeight(50.0f);
  parent.Add(v1);
  parent.Add(v2);
  parent.Add(v3);
  application.GetScene().Add(parent);
  application.SendNotification();
  application.Render();

  FocusManager::Get().RequestFocus(v1);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v1);

  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::FORWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v2);

  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::FORWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v3);
  END_TEST;
}

int UtcDaliFocusManagerBackwardSiblingsP(void)
{
  UiTestApplication application;

  View parent = View::New();
  View v1 = View::New();
  v1.SetFocusable(true);
  v1.SetRequestedX(0.0f);
  v1.SetRequestedWidth(50.0f);
  v1.SetRequestedHeight(50.0f);
  View v2 = View::New();
  v2.SetFocusable(true);
  v2.SetRequestedX(60.0f);
  v2.SetRequestedWidth(50.0f);
  v2.SetRequestedHeight(50.0f);
  View v3 = View::New();
  v3.SetFocusable(true);
  v3.SetRequestedX(120.0f);
  v3.SetRequestedWidth(50.0f);
  v3.SetRequestedHeight(50.0f);
  parent.Add(v1);
  parent.Add(v2);
  parent.Add(v3);
  application.GetScene().Add(parent);
  application.SendNotification();
  application.Render();

  FocusManager::Get().RequestFocus(v3);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v3);

  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::BACKWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v2);

  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::BACKWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v1);
  END_TEST;
}

int UtcDaliFocusManagerForwardBackwardRtlSiblingsP(void)
{
  UiTestApplication application;

  View parent = View::New();
  parent.SetRequestedWidth(180.0f);
  parent.SetRequestedHeight(50.0f);
  parent.SetLayoutDirection(Dali::LayoutDirection::RIGHT_TO_LEFT);

  View v1 = View::New();
  v1.SetFocusable(true);
  v1.SetRequestedX(0.0f);
  v1.SetRequestedWidth(50.0f);
  v1.SetRequestedHeight(50.0f);
  View v2 = View::New();
  v2.SetFocusable(true);
  v2.SetRequestedX(60.0f);
  v2.SetRequestedWidth(50.0f);
  v2.SetRequestedHeight(50.0f);
  View v3 = View::New();
  v3.SetFocusable(true);
  v3.SetRequestedX(120.0f);
  v3.SetRequestedWidth(50.0f);
  v3.SetRequestedHeight(50.0f);

  parent.Add(v1);
  parent.Add(v2);
  parent.Add(v3);
  application.GetScene().Add(parent);

  parent.Measure(180.0f, 50.0f);
  parent.Arrange(LayoutRect(0.0f, 0.0f, 180.0f, 50.0f));

  DALI_TEST_EQUALS(v1.GetPositionX(), 130.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(v2.GetPositionX(), 70.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(v3.GetPositionX(), 10.0f, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  FocusManager::Get().RequestFocus(v1);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v1);

  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::FORWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v2);

  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::FORWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v3);

  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::BACKWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v2);
  END_TEST;
}

int UtcDaliFocusManagerForwardBackwardRtlStandaloneSiblingsP(void)
{
  UiTestApplication application;

  View parent = View::New();
  parent.SetRequestedWidth(180.0f);
  parent.SetRequestedHeight(50.0f);
  parent.SetLayoutDirection(Dali::LayoutDirection::RIGHT_TO_LEFT);

  View v1 = View::New();
  v1.SetLayoutMode(LayoutMode::STANDALONE);
  v1.SetFocusable(true);
  v1.SetRequestedX(120.0f);
  v1.SetRequestedWidth(50.0f);
  v1.SetRequestedHeight(50.0f);
  View v2 = View::New();
  v2.SetLayoutMode(LayoutMode::STANDALONE);
  v2.SetFocusable(true);
  v2.SetRequestedX(60.0f);
  v2.SetRequestedWidth(50.0f);
  v2.SetRequestedHeight(50.0f);
  View v3 = View::New();
  v3.SetLayoutMode(LayoutMode::STANDALONE);
  v3.SetFocusable(true);
  v3.SetRequestedX(0.0f);
  v3.SetRequestedWidth(50.0f);
  v3.SetRequestedHeight(50.0f);

  parent.Add(v1);
  parent.Add(v2);
  parent.Add(v3);
  application.GetScene().Add(parent);

  parent.Measure(180.0f, 50.0f);
  parent.Arrange(LayoutRect(0.0f, 0.0f, 180.0f, 50.0f));

  DALI_TEST_EQUALS(v1.GetPositionX(), 120.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(v2.GetPositionX(), 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(v3.GetPositionX(), 0.0f, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  FocusManager::Get().RequestFocus(v1);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v1);

  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::FORWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v2);

  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::FORWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v3);

  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::BACKWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v2);
  END_TEST;
}

int UtcDaliFocusManagerForwardBackwardRtlHorizontalStackP(void)
{
  UiTestApplication application;

  StackLayout parent = StackLayout::New(StackOrientation::HORIZONTAL);
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(50.0f);
  parent.SetLayoutDirection(Dali::LayoutDirection::RIGHT_TO_LEFT);

  View v1 = View::New();
  v1.SetFocusable(true);
  v1.SetRequestedWidth(40.0f);
  v1.SetRequestedHeight(50.0f);
  View v2 = View::New();
  v2.SetFocusable(true);
  v2.SetRequestedWidth(50.0f);
  v2.SetRequestedHeight(50.0f);
  View v3 = View::New();
  v3.SetFocusable(true);
  v3.SetRequestedWidth(60.0f);
  v3.SetRequestedHeight(50.0f);

  v1.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  v2.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  v3.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));

  parent.Add(v1);
  parent.Add(v2);
  parent.Add(v3);
  application.GetScene().Add(parent);

  parent.Measure(200.0f, 50.0f);
  parent.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 50.0f));

  DALI_TEST_EQUALS(v1.GetPositionX(), 160.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(v2.GetPositionX(), 110.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(v3.GetPositionX(), 50.0f, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  FocusManager::Get().RequestFocus(v1);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v1);

  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::FORWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v2);

  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::FORWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v3);

  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::BACKWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v2);
  END_TEST;
}

int UtcDaliFocusManagerForwardSkipsNonFocusableP(void)
{
  UiTestApplication application;

  // v1(focusable) → v2(NOT focusable) → v3(focusable)
  // FORWARD from v1 should skip v2 and land on v3.
  View parent = View::New();
  View v1 = View::New();
  v1.SetFocusable(true);
  v1.SetRequestedX(0.0f);
  v1.SetRequestedWidth(50.0f);
  v1.SetRequestedHeight(50.0f);
  View v2 = View::New();
  v2.SetRequestedX(60.0f);
  v2.SetRequestedWidth(50.0f);
  v2.SetRequestedHeight(50.0f); // not focusable
  View v3 = View::New();
  v3.SetFocusable(true);
  v3.SetRequestedX(120.0f);
  v3.SetRequestedWidth(50.0f);
  v3.SetRequestedHeight(50.0f);
  parent.Add(v1);
  parent.Add(v2);
  parent.Add(v3);
  application.GetScene().Add(parent);
  application.SendNotification();
  application.Render();

  FocusManager::Get().RequestFocus(v1);
  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::FORWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v3);
  END_TEST;
}

int UtcDaliFocusManagerForwardNestedLayoutP(void)
{
  UiTestApplication application;

  // parent
  //   ├─ v1 (focusable)
  //   └─ nested
  //       ├─ v2 (focusable)
  //       └─ v3 (focusable)
  //
  // FORWARD from v1 → v2 → v3 (depth-first child order).
  View parent = View::New();
  View v1 = View::New();
  v1.SetFocusable(true);
  v1.SetRequestedX(0.0f);
  v1.SetRequestedY(0.0f);
  v1.SetRequestedWidth(50.0f);
  v1.SetRequestedHeight(50.0f);
  View nested = View::New();
  nested.SetRequestedX(60.0f);
  nested.SetRequestedY(0.0f);
  nested.SetRequestedWidth(50.0f);
  nested.SetRequestedHeight(120.0f);
  View v2 = View::New();
  v2.SetFocusable(true);
  v2.SetRequestedY(0.0f);
  v2.SetRequestedWidth(50.0f);
  v2.SetRequestedHeight(50.0f);
  View v3 = View::New();
  v3.SetFocusable(true);
  v3.SetRequestedY(60.0f);
  v3.SetRequestedWidth(50.0f);
  v3.SetRequestedHeight(50.0f);
  nested.Add(v2);
  nested.Add(v3);
  parent.Add(v1);
  parent.Add(nested);
  application.GetScene().Add(parent);
  application.SendNotification();
  application.Render();

  FocusManager::Get().RequestFocus(v1);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v1);

  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::FORWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v2);

  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::FORWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v3);
  END_TEST;
}

int UtcDaliFocusManagerForwardAtEndFailsN(void)
{
  UiTestApplication application;

  // Only two views. After reaching the last one, FORWARD should fail.
  View parent = View::New();
  View v1 = View::New();
  v1.SetFocusable(true);
  v1.SetRequestedX(0.0f);
  v1.SetRequestedWidth(50.0f);
  v1.SetRequestedHeight(50.0f);
  View v2 = View::New();
  v2.SetFocusable(true);
  v2.SetRequestedX(60.0f);
  v2.SetRequestedWidth(50.0f);
  v2.SetRequestedHeight(50.0f);
  parent.Add(v1);
  parent.Add(v2);
  application.GetScene().Add(parent);
  application.SendNotification();
  application.Render();

  FocusManager::Get().RequestFocus(v2);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v2);

  // No next view — should fail and stay on v2
  DALI_TEST_CHECK(!FocusManager::Get().MoveFocus(FocusDirection::FORWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v2);
  END_TEST;
}

int UtcDaliFocusManagerBackwardAtBeginFailsN(void)
{
  UiTestApplication application;

  View parent = View::New();
  View v1 = View::New();
  v1.SetFocusable(true);
  v1.SetRequestedX(0.0f);
  v1.SetRequestedWidth(50.0f);
  v1.SetRequestedHeight(50.0f);
  View v2 = View::New();
  v2.SetFocusable(true);
  v2.SetRequestedX(60.0f);
  v2.SetRequestedWidth(50.0f);
  v2.SetRequestedHeight(50.0f);
  parent.Add(v1);
  parent.Add(v2);
  application.GetScene().Add(parent);
  application.SendNotification();
  application.Render();

  FocusManager::Get().RequestFocus(v1);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v1);

  // No previous view — should fail and stay on v1
  DALI_TEST_CHECK(!FocusManager::Get().MoveFocus(FocusDirection::BACKWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v1);
  END_TEST;
}

int UtcDaliFocusManagerForwardSkipsInvisibleAndDisabledP(void)
{
  UiTestApplication application;

  // v1(focusable) → v2(invisible) → v3(disabled) → v4(focusable)
  View parent = View::New();
  View v1 = View::New();
  v1.SetFocusable(true);
  v1.SetRequestedX(0.0f);
  v1.SetRequestedWidth(50.0f);
  v1.SetRequestedHeight(50.0f);
  View v2 = View::New();
  v2.SetFocusable(true);
  v2.SetVisible(false);
  v2.SetRequestedX(60.0f);
  v2.SetRequestedWidth(50.0f);
  v2.SetRequestedHeight(50.0f);
  View v3 = View::New();
  v3.SetFocusable(true);
  v3.SetEnabled(false);
  v3.SetRequestedX(120.0f);
  v3.SetRequestedWidth(50.0f);
  v3.SetRequestedHeight(50.0f);
  View v4 = View::New();
  v4.SetFocusable(true);
  v4.SetRequestedX(180.0f);
  v4.SetRequestedWidth(50.0f);
  v4.SetRequestedHeight(50.0f);
  parent.Add(v1);
  parent.Add(v2);
  parent.Add(v3);
  parent.Add(v4);
  application.GetScene().Add(parent);
  application.SendNotification();
  application.Render();

  FocusManager::Get().RequestFocus(v1);
  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::FORWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v4);
  END_TEST;
}

int UtcDaliFocusManagerForwardDescendantBlockedSkipsSubtreeP(void)
{
  UiTestApplication application;

  // parent
  //   ├─ v1 (focusable)
  //   ├─ blocked (descendant focus blocked)
  //   │   └─ v2 (focusable — but unreachable)
  //   └─ v3 (focusable)
  //
  // FORWARD from v1 should skip the blocked subtree and go to v3.
  View parent  = View::New();
  View v1 = View::New();
  v1.SetFocusable(true);
  v1.SetRequestedX(0.0f);
  v1.SetRequestedWidth(50.0f);
  v1.SetRequestedHeight(50.0f);
  View blocked = View::New();
  blocked.SetRequestedX(60.0f);
  blocked.SetRequestedWidth(50.0f);
  blocked.SetRequestedHeight(50.0f);
  blocked.SetAllowDescendantFocusEnabled(false);
  View v2 = View::New();
  v2.SetFocusable(true);
  v2.SetRequestedWidth(50.0f);
  v2.SetRequestedHeight(50.0f);
  blocked.Add(v2);
  View v3 = View::New();
  v3.SetFocusable(true);
  v3.SetRequestedX(120.0f);
  v3.SetRequestedWidth(50.0f);
  v3.SetRequestedHeight(50.0f);
  parent.Add(v1);
  parent.Add(blocked);
  parent.Add(v3);
  application.GetScene().Add(parent);
  application.SendNotification();
  application.Render();

  FocusManager::Get().RequestFocus(v1);
  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::FORWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v3);
  END_TEST;
}

int UtcDaliFocusManagerForwardSingleViewN(void)
{
  UiTestApplication application;

  // Only one focusable view — FORWARD should fail.
  View parent = View::New();
  View v1 = View::New();
  v1.SetFocusable(true);
  v1.SetRequestedWidth(50.0f);
  v1.SetRequestedHeight(50.0f);
  parent.Add(v1);
  application.GetScene().Add(parent);
  application.SendNotification();
  application.Render();

  FocusManager::Get().RequestFocus(v1);
  DALI_TEST_CHECK(!FocusManager::Get().MoveFocus(FocusDirection::FORWARD));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() == v1);
  END_TEST;
}
