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
#include <dali-ui-foundation/public-api/views/effects/overlay-effect.h>
#include <dali-ui-foundation/public-api/views/image/lottie-animation-view.h>
#include <dali-ui-foundation/public-api/views/image/selectable-lottie-animation-view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>

#include <limits>
#include <string>
#include <utility>

using namespace Dali;
using namespace Dali::Ui;

namespace UiAccessibility = Dali::Ui::Accessibility;

namespace
{
SelectableImageInterface MakeRadioButtonTestIcon()
{
  return SelectableLottieAnimationView::New(
    SelectableLottieImage("test-radio-button.json",
                          SelectableLottieImage::FrameRange(0, 19),
                          SelectableLottieImage::FrameRange(20, 38)));
}

UiStyle CreateRadioButtonStyleOverride()
{
  return RadioButtonStyle::Builder().SetIconWidth(47.0f).SetIconHeight(31.0f).Build();
}

RadioButtonStyle MakeRadioButtonLayoutStyle()
{
  return RadioButtonStyle::Builder()
    .SetPadding(Insets(10.0f, 4.0f, 6.0f, 2.0f))
    .SetIconWidth(40.0f)
    .SetIconHeight(24.0f)
    .SetIconGenerator(RadioButtonStyle::IconGenerator::New(&MakeRadioButtonTestIcon))
    .SetStateEffect(StateEffect::None())
    .Build();
}

void PrepareRadioButtonForTap(UiTestApplication& application, RadioButton radioButton)
{
  radioButton.SetRequestedWidth(100.0f);
  radioButton.SetRequestedHeight(100.0f);
  radioButton.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  radioButton.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  application.GetScene().Add(radioButton);
  application.SendNotification();
  application.Render();
}
} // unnamed namespace

void utc_dali_radio_button_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_radio_button_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliRadioButtonStyleRoundTripP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  RadioButtonStyle empty;
  DALI_TEST_CHECK(!empty);

  RadioButtonStyle style = RadioButtonStyle::Builder()
                             .SetMinimumWidth(52.0f)
                             .SetMinimumHeight(48.0f)
                             .SetPadding(Insets(1.0f, 2.0f, 3.0f, 4.0f))
                             .SetIconWidth(30.0f)
                             .SetIconHeight(28.0f)
                             .SetIconGenerator(RadioButtonStyle::IconGenerator::New(&MakeRadioButtonTestIcon))
                             .SetIconColor(UiColor(Color::RED))
                             .SetSelectedIconColor(UiColor(Color::BLUE))
                             .SetStateEffect(StateEffect::None())
                             .Build();

  DALI_TEST_CHECK(style);
  DALI_TEST_EQUALS(style.GetMinimumWidth(), 52.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetMinimumHeight(), 48.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetPadding(), Insets(1.0f, 2.0f, 3.0f, 4.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetIconWidth(), 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetIconHeight(), 28.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetIconColor().GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetSelectedIconColor().GetRgba(), Color::BLUE, TEST_LOCATION);
  DALI_TEST_CHECK(style.GetStateEffect().IsNone());
  DALI_TEST_CHECK(style.CreateIcon());

  RadioButtonStyle copy(style);
  RadioButtonStyle assigned;
  assigned = copy;
  RadioButtonStyle moved(std::move(copy));
  RadioButtonStyle moveAssigned;
  moveAssigned = std::move(assigned);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(moveAssigned);
  DALI_TEST_EQUALS(moveAssigned.GetIconWidth(), 30.0f, TEST_LOCATION);
  DALI_TEST_CHECK(RadioButtonStyle::DownCast(style));
  DALI_TEST_CHECK(RadioButtonStyle::StaticDownCast(style));

  RadioButtonStyle emptyGenerator = RadioButtonStyle::Builder()
                                      .SetIconGenerator(RadioButtonStyle::IconGenerator())
                                      .Build();
  DALI_TEST_CHECK(!emptyGenerator.CreateIcon());
  END_TEST;
}

int UtcDaliRadioButtonStyleConfigureP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  RadioButtonStyle original   = RadioButtonStyle::Default();
  RadioButtonStyle configured = original.Configure()
                                  .SetIconWidth(44.0f)
                                  .SetIconColor(UiColor(Color::GREEN))
                                  .Build();

  DALI_TEST_EQUALS(original.GetIconWidth(), 36.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(configured.GetIconWidth(), 44.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(configured.GetIconHeight(), 36.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(configured.GetIconColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(configured.GetPadding(), Insets(8.0f, 8.0f, 8.0f, 8.0f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliRadioButtonStyleDefaultP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  RadioButtonStyle style = RadioButtonStyle::Default();
  DALI_TEST_CHECK(style);
  DALI_TEST_CHECK(RadioButtonStyle::DefaultPreset());
  DALI_TEST_CHECK(RadioButtonStyle::DefaultKey() == RadioButtonStyle::DefaultKey());
  DALI_TEST_EQUALS(style.GetMinimumWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetMinimumHeight(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetPadding(), Insets(8.0f, 8.0f, 8.0f, 8.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetIconWidth(), 36.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetIconHeight(), 36.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetIconColor().GetColorId(), UiColor::OUTLINE.GetColorId(), TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetSelectedIconColor().GetColorId(), UiColor::PRIMARY.GetColorId(), TEST_LOCATION);

  OverlayEffect overlay = OverlayEffect::DownCast(style.GetStateEffect());
  DALI_TEST_CHECK(overlay);
  DALI_TEST_EQUALS(overlay.GetCornerRadius(), Vector4(0.5f, 0.5f, 0.5f, 0.5f), TEST_LOCATION);
  DALI_TEST_EQUALS(overlay.GetCornerRadiusPolicy(), CornerRadiusPolicy::RELATIVE, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRadioButtonStyleDefaultKeyOverrideP(void)
{
  Components::UiConfig config = Components::UiConfig::New();
  config.StyleSheet().SetStyle(RadioButtonStyle::DefaultKey(), &CreateRadioButtonStyleOverride);
  UiTestApplication application(config);

  RadioButtonStyle style = RadioButtonStyle::Default();
  DALI_TEST_EQUALS(style.GetIconWidth(), 47.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetIconHeight(), 31.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRadioButtonStyleDefaultIconP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  SelectableImageInterface icon = RadioButtonStyle::Default().CreateIcon();
  DALI_TEST_CHECK(icon);

  LottieAnimationView iconView = LottieAnimationView::DownCast(icon.GetView());
  DALI_TEST_CHECK(iconView);

  const std::string resourceUrl = iconView.GetResourceUrl().CStr();
  constexpr char    suffix[]    = "/components/radio-button.json";
  DALI_TEST_CHECK(resourceUrl.size() >= sizeof(suffix) - 1u);
  DALI_TEST_EQUALS(resourceUrl.compare(resourceUrl.size() - (sizeof(suffix) - 1u),
                                       sizeof(suffix) - 1u,
                                       suffix),
                   0,
                   TEST_LOCATION);
  END_TEST;
}

int UtcDaliRadioButtonNewP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  RadioButton       radioButton = RadioButton::New();

  DALI_TEST_CHECK(radioButton);
  DALI_TEST_CHECK(GroupSelectableView::DownCast(radioButton));
  DALI_TEST_CHECK(SelectableView::DownCast(radioButton));
  DALI_TEST_CHECK(!radioButton.IsSelected());
  DALI_TEST_CHECK(radioButton.IsToggleByClickEnabled());
  DALI_TEST_EQUALS(radioButton.GetChildCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(radioButton.GetChildViewAt(0u).IsAccessibilityHidden());
  END_TEST;
}

int UtcDaliRadioButtonCopyMoveDownCastP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  RadioButton       radioButton = RadioButton::New();

  RadioButton copy(radioButton);
  RadioButton assigned;
  assigned = copy;
  RadioButton moved(std::move(copy));
  RadioButton moveAssigned;
  moveAssigned = std::move(assigned);

  DALI_TEST_CHECK(moved == radioButton);
  DALI_TEST_CHECK(moveAssigned == radioButton);
  DALI_TEST_CHECK(RadioButton::DownCast(BaseHandle(radioButton)) == radioButton);
  DALI_TEST_CHECK(!RadioButton::DownCast(BaseHandle()));
  DALI_TEST_CHECK(!RadioButton::DownCast(CheckBox::New()));
  END_TEST;
}

int UtcDaliRadioButtonSelectionAnimationModeP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  RadioButton       radioButton = RadioButton::New();

  DALI_TEST_EQUALS(static_cast<int>(radioButton.GetSelectionAnimationMode()),
                   static_cast<int>(SelectionAnimationMode::AUTO),
                   TEST_LOCATION);
  radioButton.SetSelectionAnimationMode(SelectionAnimationMode::DISABLED);
  DALI_TEST_EQUALS(static_cast<int>(radioButton.GetSelectionAnimationMode()),
                   static_cast<int>(SelectionAnimationMode::DISABLED),
                   TEST_LOCATION);
  radioButton.SetSelectionAnimationMode(SelectionAnimationMode::ENABLED);
  radioButton.SetSelected(true);
  DALI_TEST_CHECK(radioButton.IsSelected());
  END_TEST;
}

int UtcDaliRadioButtonNaturalSizeP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  RadioButton       radioButton = RadioButton::New();

  MeasuredSize measured = radioButton.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(measured.GetWidth(), 52.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.GetHeight(), 52.0f, TEST_LOCATION);

  radioButton.SetIconWidth(48.0f);
  radioButton.SetIconHeight(32.0f);
  measured = radioButton.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(measured.GetWidth(), 64.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.GetHeight(), 48.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRadioButtonIconSizeUnsetNormalizationP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  RadioButtonStyle style = RadioButtonStyle::Builder()
                             .SetIconWidth(-10.0f)
                             .SetIconHeight(std::numeric_limits<float>::infinity())
                             .Build();
  DALI_TEST_EQUALS(style.GetIconWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetIconHeight(), 0.0f, TEST_LOCATION);

  RadioButton radioButton = RadioButton::New();
  radioButton.SetIconWidth(std::numeric_limits<float>::quiet_NaN());
  radioButton.SetIconHeight(-1.0f);
  DALI_TEST_EQUALS(radioButton.GetIconWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(radioButton.GetIconHeight(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRadioButtonNonSquareIconArrangeP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  RadioButton       radioButton = RadioButton::New(MakeRadioButtonLayoutStyle());
  radioButton.SetRequestedWidth(100.0f);
  radioButton.SetRequestedHeight(60.0f);
  application.GetScene().Add(radioButton);

  radioButton.Measure(100.0f, 60.0f);
  radioButton.Arrange(LayoutRect(0.0f, 0.0f, 100.0f, 60.0f));
  View icon = radioButton.GetChildViewAt(0u);

  DALI_TEST_EQUALS(icon.GetProperty<float>(Actor::Property::POSITION_X), 33.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(icon.GetProperty<float>(Actor::Property::POSITION_Y), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(icon.GetProperty<float>(Actor::Property::SIZE_WIDTH), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(icon.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 24.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRadioButtonRtlCenteredExactlyOnceP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  RadioButton       radioButton = RadioButton::New(MakeRadioButtonLayoutStyle());
  radioButton.SetRequestedWidth(100.0f);
  radioButton.SetRequestedHeight(60.0f);
  radioButton.SetLayoutDirection(Dali::LayoutDirection::RIGHT_TO_LEFT);
  application.GetScene().Add(radioButton);

  radioButton.Measure(100.0f, 60.0f);
  radioButton.Arrange(LayoutRect(0.0f, 0.0f, 100.0f, 60.0f));
  View icon = radioButton.GetChildViewAt(0u);

  DALI_TEST_EQUALS(icon.GetProperty<float>(Actor::Property::POSITION_X), 27.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(icon.GetProperty<float>(Actor::Property::POSITION_Y), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(icon.GetProperty<float>(Actor::Property::SIZE_WIDTH), 40.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRadioButtonSelectOnlyClickP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  RadioButton       radioButton    = RadioButton::New(MakeRadioButtonLayoutStyle());
  int               clickedCount   = 0;
  int               selectionCount = 0;

  radioButton.ClickedSignal().Connect(&application, [&clickedCount](View, InputEvent)
  {
    ++clickedCount;
  });
  radioButton.SelectionChangedSignal().Connect(&application, [&selectionCount](View, bool, InputEvent)
  {
    ++selectionCount;
  });
  PrepareRadioButtonForTap(application, radioButton);

  TestGenerateTap(application, 50.0f, 50.0f, 100);
  DALI_TEST_CHECK(radioButton.IsSelected());
  DALI_TEST_EQUALS(clickedCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(selectionCount, 1, TEST_LOCATION);

  TestGenerateTap(application, 50.0f, 50.0f, 300);
  DALI_TEST_CHECK(radioButton.IsSelected());
  DALI_TEST_EQUALS(clickedCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(selectionCount, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRadioButtonTextRowIndicatorTapSelectsOuterRowP(void)
{
  UiTestApplication   application(Components::UiConfig::New());
  GroupSelectableView row       = GroupSelectableView::New();
  RadioButton         indicator = RadioButton::New(MakeRadioButtonLayoutStyle());

  row.SetRequestedWidth(100.0f);
  row.SetRequestedHeight(100.0f);
  row.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  row.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  row.SetAccessibilityRole(UiAccessibility::Role::RADIO_BUTTON);
  row.SetGroupName("UtcRadioTextRow");
  row.SetAccessibilityName("Text radio row");

  indicator.SetRequestedWidth(40.0f);
  indicator.SetRequestedHeight(40.0f);
  indicator.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  indicator.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  indicator.SetClickable(false);
  indicator.SetSensitive(false);
  indicator.SetFocusable(false);
  indicator.SetAccessibilityHidden(true);

  row.Add(indicator);
  row.SelectionChangedSignal().Connect(&application, [indicator](View, bool selected, InputEvent) mutable
  {
    indicator.SetSelected(selected);
  });

  application.GetScene().Add(row);
  application.SendNotification();
  application.Render();

  TestGenerateTap(application, 20.0f, 20.0f, 100);
  DALI_TEST_CHECK(row.IsSelected());
  DALI_TEST_CHECK(indicator.IsSelected());
  DALI_TEST_CHECK(!indicator.IsSensitive());
  DALI_TEST_CHECK(!indicator.IsFocusable());
  DALI_TEST_CHECK(indicator.IsAccessibilityHidden());
  DALI_TEST_CHECK(row.GetAccessibilityRole() == UiAccessibility::Role::RADIO_BUTTON);
  DALI_TEST_CHECK(row.HasAccessibilityState(UiAccessibility::State::CHECKED));
  DALI_TEST_EQUALS(row.GetAccessibilityName(), Dali::String("Text radio row"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliRadioButtonProgrammaticDeselectP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  RadioButton       radioButton    = RadioButton::New();
  int               selectionCount = 0;
  radioButton.SelectionChangedSignal().Connect(&application, [&selectionCount](View, bool, InputEvent)
  {
    ++selectionCount;
  });

  radioButton.SetSelected(true);
  radioButton.SetSelected(false);
  DALI_TEST_CHECK(!radioButton.IsSelected());
  DALI_TEST_EQUALS(selectionCount, 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRadioButtonToggleByClickDisabledP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  RadioButton       radioButton  = RadioButton::New(MakeRadioButtonLayoutStyle());
  int               clickedCount = 0;
  radioButton.ClickedSignal().Connect(&application, [&clickedCount](View, InputEvent)
  {
    ++clickedCount;
  });
  radioButton.SetToggleByClickEnabled(false);
  PrepareRadioButtonForTap(application, radioButton);

  TestGenerateTap(application, 50.0f, 50.0f, 100);
  DALI_TEST_CHECK(!radioButton.IsSelected());
  DALI_TEST_EQUALS(clickedCount, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRadioButtonDisabledGestureProgrammaticSelectionP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  RadioButton       radioButton = RadioButton::New(MakeRadioButtonLayoutStyle());
  radioButton.SetEnabled(false);
  PrepareRadioButtonForTap(application, radioButton);

  TestGenerateTap(application, 50.0f, 50.0f, 100);
  DALI_TEST_CHECK(!radioButton.IsSelected());
  DALI_TEST_CHECK(!radioButton.HasAccessibilityState(UiAccessibility::State::ENABLED));

  radioButton.SetSelected(true);
  DALI_TEST_CHECK(radioButton.IsSelected());
  DALI_TEST_CHECK(radioButton.HasAccessibilityState(UiAccessibility::State::CHECKED));
  END_TEST;
}

int UtcDaliRadioButtonNamedGroupMutualExclusionP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  RadioButton       first  = RadioButton::New();
  RadioButton       second = RadioButton::New();
  first.SetGroupName("UtcRadioNamedGroup");
  second.SetGroupName("UtcRadioNamedGroup");
  SelectionGroup group = SelectionGroup::Find("UtcRadioNamedGroup");

  int  groupSignalCount = 0;
  View previous;
  View current;
  group.SelectedMemberChangedSignal().Connect(&application, [&](View oldMember, View newMember, InputEvent)
  {
    ++groupSignalCount;
    previous = oldMember;
    current  = newMember;
  });

  first.SetSelected(true);
  DALI_TEST_CHECK(group.GetSelectedMember() == first);
  second.SetSelected(true);
  DALI_TEST_CHECK(!first.IsSelected());
  DALI_TEST_CHECK(second.IsSelected());
  DALI_TEST_CHECK(group.GetSelectedMember() == second);
  DALI_TEST_EQUALS(groupSignalCount, 2, TEST_LOCATION);
  DALI_TEST_CHECK(previous == first);
  DALI_TEST_CHECK(current == second);
  END_TEST;
}

int UtcDaliRadioButtonGroupedClickMovesSelectionP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  RadioButton       first  = RadioButton::New(MakeRadioButtonLayoutStyle());
  RadioButton       second = RadioButton::New(MakeRadioButtonLayoutStyle());

  first.SetGroupName("UtcRadioGroupedClick");
  second.SetGroupName("UtcRadioGroupedClick");

  SelectionGroup group = SelectionGroup::Find("UtcRadioGroupedClick");

  PrepareRadioButtonForTap(application, first);
  TestGenerateTap(application, 50.0f, 50.0f, 100);
  DALI_TEST_CHECK(first.IsSelected());
  DALI_TEST_CHECK(!second.IsSelected());
  DALI_TEST_CHECK(group.GetSelectedMember() == first);

  // Named-group membership survives scene disconnect. Show only the second button so the next
  // tap targets it unambiguously, then verify that selecting it clears the off-scene winner.
  application.GetScene().Remove(first);
  PrepareRadioButtonForTap(application, second);
  TestGenerateTap(application, 50.0f, 50.0f, 300);
  DALI_TEST_CHECK(!first.IsSelected());
  DALI_TEST_CHECK(second.IsSelected());
  DALI_TEST_CHECK(group.GetSelectedMember() == second);

  // Touching the current winner again cannot empty the group.
  TestGenerateTap(application, 50.0f, 50.0f, 500);
  DALI_TEST_CHECK(second.IsSelected());
  DALI_TEST_CHECK(group.GetSelectedMember() == second);

  // Explicit programmatic deselection remains available.
  second.SetSelected(false);
  DALI_TEST_CHECK(!second.IsSelected());
  DALI_TEST_CHECK(!group.GetSelectedMember());
  END_TEST;
}

int UtcDaliRadioButtonParentAutoGroupLifecycleP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  View              parent = View::New();
  RadioButton       first  = RadioButton::New();
  RadioButton       second = RadioButton::New();
  parent.Add(first);
  parent.Add(second);
  application.GetScene().Add(parent);
  application.SendNotification();

  SelectionGroup group = SelectionGroup::Find(parent);
  DALI_TEST_EQUALS(group.GetMemberCount(), 2u, TEST_LOCATION);
  first.SetSelected(true);
  second.SetSelected(true);
  DALI_TEST_CHECK(!first.IsSelected());
  DALI_TEST_CHECK(group.GetSelectedMember() == second);

  application.GetScene().Remove(parent);
  application.SendNotification();
  DALI_TEST_EQUALS(group.GetMemberCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!first.GetGroup());
  DALI_TEST_CHECK(!second.GetGroup());

  application.GetScene().Add(parent);
  application.SendNotification();
  DALI_TEST_EQUALS(group.GetMemberCount(), 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRadioButtonGroupLeavePreservesPersistentPolicyP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  RadioButton       radioButton = RadioButton::New(MakeRadioButtonLayoutStyle());
  radioButton.SetGroupName("UtcRadioLeaveGroup");
  radioButton.SetGroupName("");
  PrepareRadioButtonForTap(application, radioButton);

  TestGenerateTap(application, 50.0f, 50.0f, 100);
  TestGenerateTap(application, 50.0f, 50.0f, 300);
  DALI_TEST_CHECK(radioButton.IsSelected());
  END_TEST;
}

int UtcDaliRadioButtonSelectionCallbackSeesSettledWinnerP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  RadioButton       first  = RadioButton::New();
  RadioButton       second = RadioButton::New();
  first.SetGroupName("UtcRadioSettledWinner");
  second.SetGroupName("UtcRadioSettledWinner");
  first.SetSelected(true);

  bool callbackRan   = false;
  bool winnerSettled = false;
  bool firstCleared  = false;
  second.SelectionChangedSignal().Connect(&application, [&](View, bool selected, InputEvent)
  {
    if(selected)
    {
      callbackRan   = true;
      winnerSettled = SelectionGroup::Find("UtcRadioSettledWinner").GetSelectedMember() == second;
      firstCleared  = !first.IsSelected();
    }
  });
  second.SetSelected(true);

  DALI_TEST_CHECK(callbackRan);
  DALI_TEST_CHECK(winnerSettled);
  DALI_TEST_CHECK(firstCleared);
  END_TEST;
}

int UtcDaliRadioButtonAccessibilityP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  RadioButton       radioButton = RadioButton::New();

  DALI_TEST_CHECK(radioButton.GetAccessibilityRole() == UiAccessibility::Role::RADIO_BUTTON);
  DALI_TEST_CHECK(radioButton.IsAccessibilityHighlightable());
  DALI_TEST_CHECK(radioButton.HasAccessibilityState(UiAccessibility::State::ENABLED));
  DALI_TEST_CHECK(!radioButton.HasAccessibilityState(UiAccessibility::State::CHECKED));
  DALI_TEST_EQUALS(radioButton.GetChildCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(radioButton.GetChildViewAt(0u).IsAccessibilityHidden());

  auto* accessible = Dali::Accessibility::Accessible::Get(radioButton);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_CHECK(accessible->GetDescription().empty());

  radioButton.SetSelected(true);
  radioButton.SetEnabled(false);
  DALI_TEST_CHECK(radioButton.HasAccessibilityState(UiAccessibility::State::CHECKED));
  DALI_TEST_CHECK(!radioButton.HasAccessibilityState(UiAccessibility::State::ENABLED));

  radioButton.SetAccessibilityDescription("Choose delivery speed");
  DALI_TEST_EQUALS(accessible->GetDescription(), std::string("Choose delivery speed"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliRadioButtonDefaultAssetP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  RadioButton       radioButton = RadioButton::New();
  DALI_TEST_EQUALS(radioButton.GetChildCount(), 1u, TEST_LOCATION);

  LottieAnimationView icon = LottieAnimationView::DownCast(radioButton.GetChildViewAt(0u));
  DALI_TEST_CHECK(icon);

  const std::string resourceUrl = icon.GetResourceUrl().CStr();
  constexpr char    suffix[]    = "/components/radio-button.json";
  DALI_TEST_CHECK(resourceUrl.size() >= sizeof(suffix) - 1u);
  DALI_TEST_EQUALS(resourceUrl.compare(resourceUrl.size() - (sizeof(suffix) - 1u),
                                       sizeof(suffix) - 1u,
                                       suffix),
                   0,
                   TEST_LOCATION);
  END_TEST;
}
