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

#include <dali-ui-test-suite-utils.h>
#include <dali-ui-components/dali-ui-components.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>

using namespace Dali;
using namespace Dali::Ui;

namespace UiAccessibility = Dali::Ui::Accessibility;

void utc_dali_text_button_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_button_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextButtonConstructorP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  TextButton        button;
  DALI_TEST_CHECK(!button);
  END_TEST;
}

int UtcDaliTextButtonNewP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  TextButton        button = TextButton::New();
  DALI_TEST_CHECK(button);
  DALI_TEST_CHECK(InteractiveView::DownCast(button));
  DALI_TEST_EQUALS(button.GetHorizontalAlignment(), LayoutAlignment::CENTER, TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetVerticalAlignment(), LayoutAlignment::CENTER, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextButtonNewWithTextP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  TextButton        button = TextButton::New("OK");
  DALI_TEST_EQUALS(button.GetText(), std::string("OK"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextButtonNewWithStyleP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  TextButtonStyle style = TextButtonStyle::Builder()
                            .SetMinimumWidth(80.0f)
                            .SetMinimumHeight(40.0f)
                            .SetFontSize(18.0f)
                            .Build();

  TextButton button = TextButton::New(style);
  DALI_TEST_EQUALS(button.GetMinimumWidth(), 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetMinimumHeight(), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetFontSize(), 18.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextButtonNewWithTextAndStyleP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  TextButtonStyle style = TextButtonStyle::Builder()
                            .SetTextColor(UiColor(Color::RED))
                            .Build();

  TextButton button = TextButton::New("OK", style);
  DALI_TEST_EQUALS(button.GetText(), std::string("OK"), TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetTextColor().GetRgba(), Color::RED, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextButtonTextP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  TextButton        button = TextButton::New();
  button.SetText("Apply");
  DALI_TEST_EQUALS(button.GetText(), std::string("Apply"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextButtonCopyAndMoveP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  TextButton        button = TextButton::New("OK");
  TextButton        copy(button);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(copy == button);

  TextButton moved(std::move(button));
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(moved.GetText(), std::string("OK"), TEST_LOCATION);
  DALI_TEST_CHECK(!button);
  END_TEST;
}

int UtcDaliTextButtonAssignmentP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  TextButton        button = TextButton::New("OK");
  TextButton        copy;
  copy = button;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(copy == button);

  TextButton moved;
  moved = std::move(button);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(moved.GetText(), std::string("OK"), TEST_LOCATION);
  DALI_TEST_CHECK(!button);
  END_TEST;
}

int UtcDaliTextButtonDownCastP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  TextButton        button = TextButton::New();
  BaseHandle        object(button);

  TextButton downcast1 = TextButton::DownCast(object);
  TextButton downcast2 = DownCast<TextButton>(object);
  DALI_TEST_CHECK(downcast1);
  DALI_TEST_CHECK(downcast2);
  END_TEST;
}

int UtcDaliTextButtonDownCastN(void)
{
  UiTestApplication application(Components::UiConfig::New());
  BaseHandle        object;
  DALI_TEST_CHECK(!TextButton::DownCast(object));
  DALI_TEST_CHECK(!DownCast<TextButton>(object));
  END_TEST;
}

int UtcDaliTextButtonAlignmentP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  TextButton        button = TextButton::New();
  button.SetHorizontalAlignment(LayoutAlignment::END);
  button.SetVerticalAlignment(LayoutAlignment::START);
  DALI_TEST_EQUALS(button.GetHorizontalAlignment(), LayoutAlignment::END, TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetVerticalAlignment(), LayoutAlignment::START, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextButtonStyleP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  Text::Underline underline;
  underline.SetThickness(2.0f);

  TextButtonStyle style = TextButtonStyle::Builder()
                            .SetMinimumWidth(80.0f)
                            .SetMinimumHeight(40.0f)
                            .SetMaximumWidth(200.0f)
                            .SetMaximumHeight(100.0f)
                            .SetCornerRadius(6.0f)
                            .SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE)
                            .SetPadding(Insets(4.0f, 5.0f, 6.0f, 7.0f))
                            .SetBackgroundColor(UiColor(Color::BLUE))
                            .SetHorizontalAlignment(LayoutAlignment::END)
                            .SetVerticalAlignment(LayoutAlignment::START)
                            .SetTextColor(UiColor(Color::RED))
                            .SetFontSize(18.0f)
                            .SetFontFamily("Roboto")
                            .SetStateEffect(StateEffect::None())
                            .SetTextUnderline(underline)
                            .Build();

  TextButton button = TextButton::New(style);

  DALI_TEST_EQUALS(button.GetMinimumWidth(), 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetMinimumHeight(), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetMaximumWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetMaximumHeight(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetCornerRadius(), Vector4(6.0f, 6.0f, 6.0f, 6.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetCornerRadiusPolicy(), CornerRadiusPolicy::RELATIVE, TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetPadding(), Insets(4.0f, 5.0f, 6.0f, 7.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetBackgroundColor().GetRgba(), Color::BLUE, TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetHorizontalAlignment(), LayoutAlignment::END, TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetVerticalAlignment(), LayoutAlignment::START, TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetTextColor().GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetFontSize(), 18.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetFontFamily(), std::string("Roboto"), TEST_LOCATION);
  DALI_TEST_CHECK(style.GetStateEffect().IsNone());
  DALI_TEST_CHECK(button.GetTextUnderline() != Text::Underline::None());
  DALI_TEST_EQUALS(button.GetTextUnderline().GetThickness(), 2.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextButtonStylePaddingHelpersP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  TextButtonStyle::Builder horizontalVerticalBuilder;
  horizontalVerticalBuilder.SetPadding(4, 6);
  TextButtonStyle horizontalVertical = std::move(horizontalVerticalBuilder).Build();
  DALI_TEST_EQUALS(horizontalVertical.GetPadding(), Insets(4.0f, 4.0f, 6.0f, 6.0f), TEST_LOCATION);

  TextButtonStyle horizontalVerticalRvalue = TextButtonStyle::Builder()
                                              .SetPadding(5, 7)
                                              .Build();
  DALI_TEST_EQUALS(horizontalVerticalRvalue.GetPadding(), Insets(5.0f, 5.0f, 7.0f, 7.0f), TEST_LOCATION);

  TextButtonStyle::Builder builder;
  builder.SetPadding(8);
  TextButtonStyle all = std::move(builder).Build();
  DALI_TEST_EQUALS(all.GetPadding(), Insets(8.0f, 8.0f, 8.0f, 8.0f), TEST_LOCATION);

  TextButtonStyle allRvalue = TextButtonStyle::Builder()
                                .SetPadding(9)
                                .Build();
  DALI_TEST_EQUALS(allRvalue.GetPadding(), Insets(9.0f, 9.0f, 9.0f, 9.0f), TEST_LOCATION);

  TextButtonStyle fractional = TextButtonStyle::Builder()
                                 .SetPadding(0.5f, 1.5f)
                                 .Build();
  DALI_TEST_EQUALS(fractional.GetPadding(), Insets(0.5f, 0.5f, 1.5f, 1.5f), TEST_LOCATION);

  TextButtonStyle fractionalAll = TextButtonStyle::Builder()
                                    .SetPadding(2.5f)
                                    .Build();
  DALI_TEST_EQUALS(fractionalAll.GetPadding(), Insets(2.5f, 2.5f, 2.5f, 2.5f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextButtonStyleConfigureP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  TextButtonStyle style = TextButtonStyle::Default()
                            .Configure()
                            .SetFontSize(20.0f)
                            .SetTextColor(UiColor(Color::WHITE))
                            .SetStateEffect(StateEffect::None())
                            .Build();

  DALI_TEST_EQUALS(style.GetMinimumWidth(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetMinimumHeight(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetPadding(), Insets(16.0f, 16.0f, 12.0f, 12.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetFontSize(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetTextColor().GetRgba(), Color::WHITE, TEST_LOCATION);
  DALI_TEST_CHECK(style.GetStateEffect().IsNone());
  END_TEST;
}

int UtcDaliTextButtonTextPropertiesP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  TextButton        button = TextButton::New();

  button.SetTextColor(UiColor(Color::GREEN));
  button.SetFontSize(22.0f);
  button.SetFontFamily("Sans");

  Text::Underline underline;
  underline.SetThickness(3.0f);
  button.SetTextUnderline(underline);

  DALI_TEST_EQUALS(button.GetTextColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetFontSize(), 22.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetFontFamily(), std::string("Sans"), TEST_LOCATION);
  DALI_TEST_CHECK(button.GetTextUnderline() != Text::Underline::None());
  DALI_TEST_EQUALS(button.GetTextUnderline().GetThickness(), 3.0f, TEST_LOCATION);

  button.SetTextUnderline(Text::Underline::None());
  DALI_TEST_CHECK(button.GetTextUnderline() == Text::Underline::None());
  END_TEST;
}

int UtcDaliTextButtonStyleDefaultKeyP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  UiStyleSheet styleSheet = Components::StyleSheet::New();
  DALI_TEST_CHECK(!styleSheet.GetStyle(TextButtonStyle::DefaultKey()));

  TextButtonStyle style = TextButtonStyle::Default();
  DALI_TEST_CHECK(style);
  DALI_TEST_EQUALS(style.GetMinimumWidth(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetMinimumHeight(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetCornerRadius(), Vector4(0.5f, 0.5f, 0.5f, 0.5f), TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetCornerRadiusPolicy(), CornerRadiusPolicy::RELATIVE, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetPadding(), Insets(16.0f, 16.0f, 12.0f, 12.0f), TEST_LOCATION);
  DALI_TEST_CHECK(style.GetBackgroundColor().HasColorId());
  DALI_TEST_EQUALS(style.GetBackgroundColor().GetColorId(), UiColor::PRIMARY.GetColorId(), TEST_LOCATION);
  DALI_TEST_CHECK(style.GetTextColor().HasColorId());
  DALI_TEST_EQUALS(style.GetTextColor().GetColorId(), UiColor::ON_PRIMARY.GetColorId(), TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetFontSize(), 16.0f, TEST_LOCATION);
  DALI_TEST_CHECK(style.GetStateEffect());
  DALI_TEST_CHECK(!style.GetStateEffect().IsNone());
  END_TEST;
}

int UtcDaliTextButtonAccessibilityP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  TextButton        button = TextButton::New("Apply");

  // Rendering a TextButton alone does not make TV Screen Reader recognize it as a button.
  // Verify that the component provides the default role and highlighting policy, and that its
  // internal visual Label is hidden from the accessibility tree to prevent duplicate announcements.
  DALI_TEST_CHECK(button.GetAccessibilityRole() == UiAccessibility::Role::BUTTON);
  DALI_TEST_CHECK(button.IsAccessibilityHighlightable());
  DALI_TEST_CHECK(button.HasAccessibilityState(UiAccessibility::State::ENABLED));
  DALI_TEST_EQUALS(button.GetChildCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(button.GetChildViewAt(0u).IsAccessibilityHidden());

  auto* accessible = Dali::Accessibility::Accessible::Get(button);
  DALI_TEST_CHECK(accessible);

  // Without an explicit name, the currently displayed text must become the name and reflect
  // SetText() changes immediately on the next query. Conversely, an application-provided name
  // must take precedence over displayed text.
  DALI_TEST_EQUALS(accessible->GetName(), std::string("Apply"), TEST_LOCATION);
  button.SetText("Apply changes");
  DALI_TEST_EQUALS(accessible->GetName(), std::string("Apply changes"), TEST_LOCATION);
  button.SetAccessibilityName("Confirm settings");
  DALI_TEST_EQUALS(accessible->GetName(), std::string("Confirm settings"), TEST_LOCATION);

  int            clickCount       = 0;
  InputEventType lastEventType    = InputEventType::NONE;
  bool           wasProgrammatic  = true;
  bool           receivedSameView = true;
  button.ClickedSignal().Connect(&application, [&clickCount, &lastEventType, &wasProgrammatic, &receivedSameView, button](View view, InputEvent event)
  {
    ++clickCount;
    lastEventType    = event.GetInputEventType();
    wasProgrammatic  = event.IsProgrammatic();
    receivedSameView = receivedSameView && view == button;
  });

  Property::Map attributes;

  // AT-SPI activation must join the existing ClickedSignal through the foundation's default
  // interactive path so it runs the same business logic as touch and remote-control callbacks.
  // Accessibility input must preserve a source distinct from ordinary programmatic changes.
  DALI_TEST_CHECK(button.DoAction("activate", attributes));
  DALI_TEST_EQUALS(clickCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(lastEventType == InputEventType::ACCESSIBILITY_ACTIVATION);
  DALI_TEST_CHECK(!wasProgrammatic);
  DALI_TEST_CHECK(receivedSameView);

  // A disabled button must not be allowed to click only through the Screen Reader path.
  // DoAction's return value can also include a default focus-request result, so determine
  // action rejection by verifying that no additional ClickedSignal is emitted.
  button.SetEnabled(false);
  DALI_TEST_CHECK(!button.HasAccessibilityState(UiAccessibility::State::ENABLED));
  button.DoAction("activate", attributes);
  DALI_TEST_EQUALS(clickCount, 1, TEST_LOCATION);
  END_TEST;
}
