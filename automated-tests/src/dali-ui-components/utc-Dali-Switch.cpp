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

#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <dali-ui-test-suite-utils.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <test-gesture-generator.h>

#include <utility>

using namespace Dali;
using namespace Dali::Ui;

namespace UiAccessibility = Dali::Ui::Accessibility;

void utc_dali_switch_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_switch_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
constexpr float SWITCH_WIDTH  = 76.0f;
constexpr float SWITCH_HEIGHT = 52.0f;
constexpr float TRACK_X       = 10.0f;
constexpr float TRACK_Y       = 10.0f;
constexpr float TRACK_WIDTH   = 56.0f;
constexpr float TRACK_HEIGHT  = 32.0f;
constexpr float TRAVEL        = TRACK_WIDTH - TRACK_HEIGHT;

struct SelectionData
{
  int            count{0};
  bool           selected{false};
  bool           programmatic{false};
  InputEventType type{InputEventType::NONE};
  View           view;
};

void ObserveSelection(SelectionData& data, View view, bool selected, InputEvent event)
{
  ++data.count;
  data.selected     = selected;
  data.programmatic = event.IsProgrammatic();
  data.type         = event.GetInputEventType();
  data.view         = view;
}

void ArrangeOnScene(UiTestApplication& application, Switch control)
{
  control.SetRequestedWidth(SWITCH_WIDTH);
  control.SetRequestedHeight(SWITCH_HEIGHT);
  control.SetPivot(Pivot::TOP_LEFT);
  control.SetParentOrigin(ParentOrigin::TOP_LEFT);
  application.GetScene().Add(control);
  control.Measure(SWITCH_WIDTH, SWITCH_HEIGHT);
  control.Arrange(LayoutRect(0.0f, 0.0f, SWITCH_WIDTH, SWITCH_HEIGHT));
  application.SendNotification();
  application.Render();
}

float PositionX(Actor actor)
{
  return actor.GetProperty<float>(Actor::Property::POSITION_X);
}

float PositionY(Actor actor)
{
  return actor.GetProperty<float>(Actor::Property::POSITION_Y);
}

float Width(Actor actor)
{
  return actor.GetProperty<float>(Actor::Property::SIZE_WIDTH);
}

float Height(Actor actor)
{
  return actor.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
}

void PressReturn(UiTestApplication& application, uint32_t time)
{
  Dali::Integration::KeyEvent keyDown(
    "Return", "", "", 0, 0, time, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::KEYBOARD, Device::Subclass::NONE);
  application.ProcessEvent(keyDown);

  Dali::Integration::KeyEvent keyUp(
    "Return", "", "", 0, 0, time + 20u, Dali::Integration::KeyEvent::UP, "", "", Device::Class::KEYBOARD, Device::Subclass::NONE);
  application.ProcessEvent(keyUp);
}
} // namespace

int UtcDaliSwitchNewCopyMoveDownCastP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Switch            control = Switch::New();

  DALI_TEST_CHECK(control);
  DALI_TEST_CHECK(SelectableView::DownCast(control));
  DALI_TEST_CHECK(!control.IsSelected());
  DALI_TEST_CHECK(control.GetSelectionAnimationMode() == SelectionAnimationMode::AUTO);

  Switch copy(control);
  DALI_TEST_CHECK(copy == control);
  Switch moved(std::move(copy));
  DALI_TEST_CHECK(moved == control);
  DALI_TEST_CHECK(Switch::DownCast(BaseHandle(control)) == control);
  DALI_TEST_CHECK(!Switch::DownCast(BaseHandle()));
  END_TEST;
}

int UtcDaliSwitchStyleAndGeometryP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  OverlayEffect overlay = OverlayEffect::Round()
                            .Configure()
                            .SetOverlayPadding(Insets(1.0f, 2.0f, 3.0f, 4.0f))
                            .SetRecoilScaleFactor(0.9f)
                            .Build();
  SwitchStyle style = SwitchStyle::Builder()
                        .SetMinimumWidth(80.0f)
                        .SetMinimumHeight(60.0f)
                        .SetPadding(Insets(11.0f, 12.0f, 13.0f, 14.0f))
                        .SetTrackSize(58.0f, 34.0f)
                        .SetThumbPadding(Insets(4.0f, 4.0f, 5.0f, 5.0f))
                        .SetTrackColors(UiColor(Color::RED), UiColor(Color::GREEN))
                        .SetThumbColor(UiColor(Color::BLUE))
                        .SetOverlayEffect(overlay)
                        .SetDragThreshold(7.0f)
                        .Build();

  DALI_TEST_EQUALS(style.GetMinimumWidth(), 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetMinimumHeight(), 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetPadding(), Insets(11.0f, 12.0f, 13.0f, 14.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetTrackWidth(), 58.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetTrackHeight(), 34.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetThumbPadding(), Insets(4.0f, 4.0f, 5.0f, 5.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetTrackColor().GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetSelectedTrackColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetThumbColor().GetRgba(), Color::BLUE, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetOverlayEffect().GetOverlayPadding(), Insets(1.0f, 2.0f, 3.0f, 4.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetOverlayEffect().GetRecoilScaleFactor(), 0.9f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetDragThreshold(), 7.0f, TEST_LOCATION);

  Switch control = Switch::New(style);
  DALI_TEST_EQUALS(control.GetMinimumWidth(), 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(control.GetMinimumHeight(), 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(control.GetPadding(), Insets(11.0f, 12.0f, 13.0f, 14.0f), TEST_LOCATION);

  Switch defaultControl = Switch::New();
  ArrangeOnScene(application, defaultControl);
  DALI_TEST_EQUALS(defaultControl.GetChildCount(), 2u, TEST_LOCATION);

  View track       = defaultControl.GetChildViewAt(0u);
  View thumbTarget = defaultControl.GetStateEffectTarget();
  DALI_TEST_CHECK(thumbTarget == defaultControl.GetChildViewAt(1u));
  DALI_TEST_EQUALS(PositionX(track), TRACK_X, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionY(track), TRACK_Y, TEST_LOCATION);
  DALI_TEST_EQUALS(Width(track), TRACK_WIDTH, TEST_LOCATION);
  DALI_TEST_EQUALS(Height(track), TRACK_HEIGHT, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionX(thumbTarget), TRACK_X, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionY(thumbTarget), TRACK_Y, TEST_LOCATION);
  DALI_TEST_EQUALS(Width(thumbTarget), TRACK_HEIGHT, TEST_LOCATION);
  DALI_TEST_EQUALS(Height(thumbTarget), TRACK_HEIGHT, TEST_LOCATION);
  END_TEST;
}

int UtcDaliSwitchProgrammaticSelectionAndAccessibilityP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Switch            control = Switch::New();
  SelectionData     data;
  control.SelectionChangedSignal().Connect(&application, [&data](View view, bool selected, InputEvent event)
  {
    ObserveSelection(data, view, selected, event);
  });

  DALI_TEST_CHECK(control.GetAccessibilityRole() == UiAccessibility::Role::SWITCH);
  DALI_TEST_CHECK(control.HasAccessibilityState(UiAccessibility::State::ENABLED));
  DALI_TEST_CHECK(!control.HasAccessibilityState(UiAccessibility::State::CHECKED));
  DALI_TEST_CHECK(Dali::Accessibility::Accessible::Get(control));

  control.SetSelected(true);
  DALI_TEST_CHECK(control.IsSelected());
  DALI_TEST_CHECK(control.HasAccessibilityState(UiAccessibility::State::CHECKED));
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);
  DALI_TEST_CHECK(data.selected);
  DALI_TEST_CHECK(data.programmatic);
  DALI_TEST_CHECK(data.type == InputEventType::NONE);
  DALI_TEST_CHECK(data.view == control);

  control.SetSelected(true);
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);

  control.SetSelected(false);
  DALI_TEST_CHECK(!control.IsSelected());
  DALI_TEST_CHECK(!control.HasAccessibilityState(UiAccessibility::State::CHECKED));
  DALI_TEST_EQUALS(data.count, 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliSwitchTapAndEnterToggleP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Switch            control = Switch::New();
  SelectionData     data;
  control.SelectionChangedSignal().Connect(&application, [&data](View view, bool selected, InputEvent event)
  {
    ObserveSelection(data, view, selected, event);
  });
  control.SetSelectionAnimationMode(SelectionAnimationMode::DISABLED);
  ArrangeOnScene(application, control);

  TestGenerateTap(application, SWITCH_WIDTH * 0.5f, SWITCH_HEIGHT * 0.5f, 100u);
  DALI_TEST_CHECK(control.IsSelected());
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!data.programmatic);
  DALI_TEST_CHECK(data.type == InputEventType::TAP_GESTURE);

  control.SetFocusable(true);
  DALI_TEST_CHECK(FocusManager::Get().SetCurrentFocusView(control));
  application.SendNotification();
  application.Render();
  PressReturn(application, 200u);

  DALI_TEST_CHECK(!control.IsSelected());
  DALI_TEST_EQUALS(data.count, 2, TEST_LOCATION);
  DALI_TEST_CHECK(!data.programmatic);
  DALI_TEST_CHECK(data.type == InputEventType::KEY_EVENT);

  FocusManager::Get().ClearFocus();
  application.SendNotification();
  application.Render(0u);
  application.Render(150u);
  END_TEST;
}

int UtcDaliSwitchDragCommitAndInterruptionSettleP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Switch            control = Switch::New();
  SelectionData     data;
  control.SelectionChangedSignal().Connect(&application, [&data](View view, bool selected, InputEvent event)
  {
    ObserveSelection(data, view, selected, event);
  });
  control.SetSelectionAnimationMode(SelectionAnimationMode::DISABLED);
  ArrangeOnScene(application, control);

  uint32_t time = 100u;
  TestStartPan(application, Vector2(20.0f, 26.0f), Vector2(60.0f, 26.0f), time);
  TestEndPan(application, Vector2(60.0f, 26.0f), time);
  application.SendNotification();
  control.Arrange(LayoutRect(0.0f, 0.0f, SWITCH_WIDTH, SWITCH_HEIGHT));

  DALI_TEST_CHECK(control.IsSelected());
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);
  DALI_TEST_CHECK(data.selected);
  DALI_TEST_CHECK(data.programmatic);
  DALI_TEST_EQUALS(PositionX(control.GetStateEffectTarget()), TRACK_X + TRAVEL, TEST_LOCATION);

  control.SetSelected(false);
  control.Arrange(LayoutRect(0.0f, 0.0f, SWITCH_WIDTH, SWITCH_HEIGHT));
  data = SelectionData();

  time = 300u;
  TestStartPan(application, Vector2(20.0f, 26.0f), Vector2(30.0f, 26.0f), time);
  TestCancelPan(application, Vector2(30.0f, 26.0f), time);
  application.SendNotification();
  control.Arrange(LayoutRect(0.0f, 0.0f, SWITCH_WIDTH, SWITCH_HEIGHT));

  DALI_TEST_CHECK(!control.IsSelected());
  DALI_TEST_EQUALS(data.count, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionX(control.GetStateEffectTarget()), TRACK_X, TEST_LOCATION);

  // Cancelling a drag must not affect the next independent touch sequence.
  TestGenerateTap(application, SWITCH_WIDTH * 0.5f, SWITCH_HEIGHT * 0.5f, 500u);
  DALI_TEST_CHECK(control.IsSelected());
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!data.programmatic);
  DALI_TEST_CHECK(data.type == InputEventType::TAP_GESTURE);
  END_TEST;
}

int UtcDaliSwitchUserInputPoliciesP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Switch            control = Switch::New();
  ArrangeOnScene(application, control);

  control.SetEnabled(false);
  TestGenerateTap(application, 38.0f, 26.0f, 100u);
  DALI_TEST_CHECK(!control.IsSelected());
  DALI_TEST_CHECK(!control.HasAccessibilityState(UiAccessibility::State::ENABLED));

  control.SetSelected(true);
  DALI_TEST_CHECK(control.IsSelected());
  control.SetSelected(false);
  control.SetEnabled(true);

  control.SetToggleByClickEnabled(false);
  TestGenerateTap(application, 38.0f, 26.0f, 200u);
  DALI_TEST_CHECK(!control.IsSelected());

  control.SetToggleByClickEnabled(true);
  control.SetClickable(false);
  TestGenerateTap(application, 38.0f, 26.0f, 300u);
  DALI_TEST_CHECK(!control.IsSelected());
  END_TEST;
}

int UtcDaliSwitchSelectionAnimationModeP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Switch            control = Switch::New();

  DALI_TEST_CHECK(control.GetSelectionAnimationMode() == SelectionAnimationMode::AUTO);
  control.SetSelectionAnimationMode(SelectionAnimationMode::DISABLED);
  DALI_TEST_CHECK(control.GetSelectionAnimationMode() == SelectionAnimationMode::DISABLED);
  control.SetSelected(true);
  DALI_TEST_CHECK(control.IsSelected());
  control.SetSelectionAnimationMode(SelectionAnimationMode::ENABLED);
  DALI_TEST_CHECK(control.GetSelectionAnimationMode() == SelectionAnimationMode::ENABLED);
  control.SetSelected(false);
  DALI_TEST_CHECK(!control.IsSelected());
  END_TEST;
}

int UtcDaliSwitchOverlayTargetP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  SwitchStyle       style   = SwitchStyle::Default();
  Switch            control = Switch::New(style);
  ArrangeOnScene(application, control);

  OverlayEffect effect = style.GetOverlayEffect();
  DALI_TEST_CHECK(effect);
  DALI_TEST_EQUALS(effect.GetOverlayPadding(), Insets(10.0f, 10.0f, 10.0f, 10.0f), TEST_LOCATION);
  DALI_TEST_CHECK(effect.GetRecoilScope() == RecoilScope::OVERLAY_TARGET_CHILDREN);
  DALI_TEST_EQUALS(effect.GetRecoilScaleFactor(), 0.96f, TEST_LOCATION);

  View target = control.GetStateEffectTarget();
  DALI_TEST_CHECK(target);
  DALI_TEST_CHECK(target != control);
  DALI_TEST_EQUALS(target.GetChildCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(target.GetChildViewAt(0u).IsAccessibilityHidden());
  DALI_TEST_CHECK(target.IsAccessibilityHidden());
  END_TEST;
}
