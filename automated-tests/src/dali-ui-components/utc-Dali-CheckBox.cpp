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
#include <dali-ui-foundation/public-api/types/selectable-lottie-image.h>
#include <dali-ui-foundation/public-api/views/image/selectable-image-interface.h>
#include <dali-ui-foundation/public-api/views/image/selectable-lottie-animation-view.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>

#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace UiAccessibility = Dali::Ui::Accessibility;

void utc_dali_check_box_startup(void)
{
  test_return_value = TET_UNDEF;
}
void utc_dali_check_box_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
// Observes SelectionChangedSignal by reference.
struct SelectionSpy
{
  SelectionSpy(int& count, bool& last)
  : mCount(count), mLast(last)
  {
  }
  void operator()(View, bool selected, InputEvent)
  {
    ++mCount;
    mLast = selected;
  }
  int&  mCount;
  bool& mLast;
};

// Capture-less creator registered for CheckBoxStyle::DefaultKey() overrides. Must be a free
// function (UiStyleCreator = UiStyle(*)()); a capturing lambda would not convert.
UiStyle CreateCheckBoxOverride()
{
  return CheckBoxStyle::Builder().SetIconWidth(99.0f).SetIconHeight(99.0f).Build();
}

// Capture-less icon generator for CheckBoxStyle::SetIconGenerator(). Must be a free function
// (IconGenerator = Ui::Callback<SelectableImageInterface()>); a capturing lambda would not convert.
SelectableImageInterface MakeTestIcon()
{
  return SelectableLottieAnimationView::New(
    SelectableLottieImage("i_check_box.json",
                          SelectableLottieImage::FrameRange(0, 30),
                          SelectableLottieImage::FrameRange(30, 48)));
}
} // namespace

int UtcDaliCheckBoxNewP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  CheckBox          cb = CheckBox::New();
  DALI_TEST_CHECK(cb);
  DALI_TEST_CHECK(SelectableView::DownCast(cb)); // is-a selectable
  DALI_TEST_CHECK(!cb.IsSelected());             // default unchecked
  END_TEST;
}

int UtcDaliCheckBoxCopyMoveDownCast(void)
{
  UiTestApplication application(Components::UiConfig::New());
  CheckBox          cb = CheckBox::New();

  CheckBox copy(cb);
  DALI_TEST_CHECK(copy == cb);
  CheckBox moved(std::move(copy));
  DALI_TEST_CHECK(moved == cb);

  BaseHandle handle(cb);
  DALI_TEST_CHECK(CheckBox::DownCast(handle));
  DALI_TEST_CHECK(!CheckBox::DownCast(BaseHandle())); // empty for unrelated
  END_TEST;
}

int UtcDaliCheckBoxSelectionChangedFiresOncePerChange(void)
{
  UiTestApplication application(Components::UiConfig::New());
  CheckBox          cb = CheckBox::New();

  int  count = 0;
  bool last  = false;
  cb.SelectionChangedSignal().Connect(&application, SelectionSpy(count, last));

  cb.SetSelected(true);
  DALI_TEST_EQUALS(count, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(last, true, TEST_LOCATION);

  cb.SetSelected(true); // same value -> no emit (short-circuit)
  DALI_TEST_EQUALS(count, 1, TEST_LOCATION);

  cb.SetSelected(false);
  DALI_TEST_EQUALS(count, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(last, false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCheckBoxTextBoxOnlyAndLabel(void)
{
  UiTestApplication application(Components::UiConfig::New());

  CheckBox boxOnly = CheckBox::New();
  DALI_TEST_EQUALS(boxOnly.GetText(), std::string(""), TEST_LOCATION);

  CheckBox labelled = CheckBox::New("Agree");
  DALI_TEST_EQUALS(labelled.GetText(), std::string("Agree"), TEST_LOCATION);

  labelled.SetText("");
  DALI_TEST_EQUALS(labelled.GetText(), std::string(""), TEST_LOCATION);
  END_TEST;
}

int UtcDaliCheckBoxSelectionAnimationMode(void)
{
  UiTestApplication application(Components::UiConfig::New());
  CheckBox          cb = CheckBox::New();

  DALI_TEST_EQUALS(static_cast<int>(cb.GetSelectionAnimationMode()),
                   static_cast<int>(SelectionAnimationMode::AUTO), TEST_LOCATION); // default

  cb.SetSelectionAnimationMode(SelectionAnimationMode::DISABLED);
  DALI_TEST_EQUALS(static_cast<int>(cb.GetSelectionAnimationMode()),
                   static_cast<int>(SelectionAnimationMode::DISABLED), TEST_LOCATION);

  // Off-scene programmatic changes must snap without crashing regardless of mode.
  cb.SetSelectionAnimationMode(SelectionAnimationMode::ENABLED);
  cb.SetSelected(true);
  DALI_TEST_CHECK(cb.IsSelected());
  END_TEST;
}

int UtcDaliCheckBoxStyleBuilderRoundTrip(void)
{
  UiTestApplication application(Components::UiConfig::New());

  CheckBoxStyle style = CheckBoxStyle::Builder()
                          .SetIconWidth(40.0f)
                          .SetIconHeight(28.0f)
                          .SetLabelGap(8.0f)
                          .SetMinimumWidth(48.0f)
                          .SetIconGenerator(CheckBoxStyle::IconGenerator::New(&MakeTestIcon))
                          .Build();

  DALI_TEST_EQUALS(style.GetIconWidth(), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetIconHeight(), 28.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetLabelGap(), 8.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetMinimumWidth(), 48.0f, TEST_LOCATION);
  SelectableImageInterface icon = style.CreateIcon();
  DALI_TEST_CHECK(icon);
  DALI_TEST_CHECK(icon.GetView());

  CheckBox cb = CheckBox::New(style);
  DALI_TEST_CHECK(cb);
  DALI_TEST_EQUALS(cb.GetMinimumWidth(), 48.0f, TEST_LOCATION); // inherited from View
  END_TEST;
}

int UtcDaliCheckBoxViewIconAndTextPropertiesP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  CheckBox          cb = CheckBox::New("Agree");

  // Icon width/height are runtime-mutable on the view (mirror CheckBoxStyle); the getters return
  // the requested value.
  cb.SetIconWidth(40.0f);
  cb.SetIconHeight(28.0f);
  DALI_TEST_EQUALS(cb.GetIconWidth(), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(cb.GetIconHeight(), 28.0f, TEST_LOCATION);

  // Trailing-label text style mirrors TextButton.
  cb.SetTextColor(UiColor(Color::GREEN));
  cb.SetFontSize(22.0f);
  cb.SetFontFamily("Sans");

  Text::Underline underline;
  underline.SetThickness(3.0f);
  cb.SetTextUnderline(underline);

  DALI_TEST_EQUALS(cb.GetTextColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(cb.GetFontSize(), 22.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(cb.GetFontFamily(), std::string("Sans"), TEST_LOCATION);
  DALI_TEST_CHECK(cb.GetTextUnderline() != Text::Underline::None());
  DALI_TEST_EQUALS(cb.GetTextUnderline().GetThickness(), 3.0f, TEST_LOCATION);

  cb.SetTextUnderline(Text::Underline::None());
  DALI_TEST_CHECK(cb.GetTextUnderline() == Text::Underline::None());
  END_TEST;
}

int UtcDaliCheckBoxDisabledState(void)
{
  UiTestApplication application(Components::UiConfig::New());
  CheckBox          cb = CheckBox::New();

  // §1.4.3: disabling uses the inherited View::SetEnabled(false), which blocks user
  // toggles via disabled interaction (DALi does NOT auto-deselect on disable).
  cb.SetEnabled(false);
  DALI_TEST_CHECK(!cb.IsEnabled());

  // Programmatic SetSelected is unaffected by disable.
  cb.SetSelected(true);
  DALI_TEST_CHECK(cb.IsSelected());

  // Note: asserting that a user CLICK is suppressed while disabled requires touch/key
  // injection (see §7 notes); the deterministic checks above cover the disable API.
  END_TEST;
}

int UtcDaliCheckBoxNewWithStyleP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  CheckBoxStyle style = CheckBoxStyle::Builder()
                          .SetMinimumWidth(60.0f)
                          .SetMinimumHeight(40.0f)
                          .SetPadding(Insets(2.0f, 3.0f, 4.0f, 5.0f))
                          .Build();

  CheckBox cb = CheckBox::New(style);
  DALI_TEST_CHECK(cb);
  // View-inherited fields are pushed onto the widget by ApplyInitialStyle().
  DALI_TEST_EQUALS(cb.GetMinimumWidth(), 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(cb.GetMinimumHeight(), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(cb.GetPadding(), Insets(2.0f, 3.0f, 4.0f, 5.0f), TEST_LOCATION);

  CheckBoxStyle fractionalStyle = CheckBoxStyle::Builder()
                                    .SetPadding(Insets(0.5f, 1.5f, 2.5f, 3.5f))
                                    .Build();
  CheckBox fractional = CheckBox::New(fractionalStyle);
  DALI_TEST_EQUALS(fractional.GetPadding(), Insets(0.5f, 1.5f, 2.5f, 3.5f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliCheckBoxNewWithTextAndStyleP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  CheckBoxStyle style = CheckBoxStyle::Builder()
                          .SetMinimumWidth(72.0f)
                          .Build();

  CheckBox cb = CheckBox::New("Agree", style);
  DALI_TEST_EQUALS(cb.GetText(), std::string("Agree"), TEST_LOCATION);
  DALI_TEST_EQUALS(cb.GetMinimumWidth(), 72.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCheckBoxStyleP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  // Exercise every builder setter. Icon width/height are set independently to cover a
  // non-square glyph, and the text label properties mirror TextButtonStyle.
  Text::Underline underline;
  underline.SetThickness(2.0f);

  CheckBoxStyle style = CheckBoxStyle::Builder()
                          .SetMinimumWidth(50.0f)
                          .SetMinimumHeight(44.0f)
                          .SetPadding(Insets(4.0f, 5.0f, 6.0f, 7.0f))
                          .SetIconWidth(40.0f)
                          .SetIconHeight(28.0f)
                          .SetLabelGap(10.0f)
                          .SetIconGenerator(CheckBoxStyle::IconGenerator::New(&MakeTestIcon))
                          .SetIconColor(UiColor(Color::RED))
                          .SetSelectedIconColor(UiColor(Color::BLUE))
                          .SetTextColor(UiColor(Color::GREEN))
                          .SetFontSize(20.0f)
                          .SetFontFamily("SamsungOne")
                          .SetTextUnderline(underline)
                          .SetStateEffect(StateEffect::None())
                          .Build();

  // Icon/label/colour/text fields are read back through the style (the widget does not expose
  // them).
  DALI_TEST_EQUALS(style.GetMinimumWidth(), 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetMinimumHeight(), 44.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetIconWidth(), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetIconHeight(), 28.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetLabelGap(), 10.0f, TEST_LOCATION);
  SelectableImageInterface icon = style.CreateIcon();
  DALI_TEST_CHECK(icon);
  DALI_TEST_CHECK(icon.GetView());
  DALI_TEST_EQUALS(style.GetIconColor().GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetSelectedIconColor().GetRgba(), Color::BLUE, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetTextColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetFontSize(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetFontFamily(), std::string("SamsungOne"), TEST_LOCATION);
  DALI_TEST_CHECK(style.GetTextUnderline() != Text::Underline::None());
  DALI_TEST_EQUALS(style.GetTextUnderline().GetThickness(), 2.0f, TEST_LOCATION);
  DALI_TEST_CHECK(style.GetStateEffect().IsNone());

  // View-inherited fields round-trip onto the constructed widget.
  CheckBox cb = CheckBox::New(style);
  DALI_TEST_EQUALS(cb.GetMinimumWidth(), 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(cb.GetMinimumHeight(), 44.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(cb.GetPadding(), Insets(4.0f, 5.0f, 6.0f, 7.0f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliCheckBoxStyleConfigureP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  // Edit-from-existing: change two fields, leave the rest at DefaultPreset values.
  CheckBoxStyle style = CheckBoxStyle::Default()
                          .Configure()
                          .SetIconWidth(28.0f)
                          .SetIconHeight(28.0f)
                          .SetTextColor(UiColor(Color::WHITE))
                          .Build();

  DALI_TEST_EQUALS(style.GetIconWidth(), 28.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetIconHeight(), 28.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetTextColor().GetRgba(), Color::WHITE, TEST_LOCATION);

  // Unset fields preserve the DefaultPreset defaults (labelGap 8.0f, padding 8, zero min-size).
  DALI_TEST_EQUALS(style.GetLabelGap(), 8.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetPadding(), Insets(8.0f, 8.0f, 8.0f, 8.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetMinimumWidth(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCheckBoxStyleDefaultKeyP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  UiStyleSheet styleSheet = Components::StyleSheet::New();
  DALI_TEST_CHECK(!styleSheet.GetStyle(CheckBoxStyle::DefaultKey()));

  // No DefaultKey override registered -> Default() falls back to DefaultPreset().
  CheckBoxStyle style = CheckBoxStyle::Default();
  DALI_TEST_CHECK(style);
  DALI_TEST_EQUALS(style.GetMinimumWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetMinimumHeight(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetLabelGap(), 8.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetIconWidth(), 36.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetIconHeight(), 36.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetFontSize(), 16.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetFontFamily(), std::string("SamsungOneUI600"), TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetPadding(), Insets(8.0f, 8.0f, 8.0f, 8.0f), TEST_LOCATION);
  DALI_TEST_CHECK(style.GetIconColor().HasColorId());
  DALI_TEST_EQUALS(style.GetIconColor().GetColorId(), UiColor::OUTLINE.GetColorId(), TEST_LOCATION);
  DALI_TEST_CHECK(style.GetSelectedIconColor().HasColorId());
  DALI_TEST_EQUALS(style.GetSelectedIconColor().GetColorId(), UiColor::PRIMARY.GetColorId(), TEST_LOCATION);
  DALI_TEST_CHECK(style.GetStateEffect());
  END_TEST;
}

int UtcDaliCheckBoxStyleDefaultKeyOverrideP(void)
{
  // Register a DefaultKey() creator BEFORE constructing the application (its ctor calls
  // config.Apply(), which freezes the sheet). Registering after Apply would assert.
  Components::UiConfig config = Components::UiConfig::New();
  config.StyleSheet().SetStyle(CheckBoxStyle::DefaultKey(), &CreateCheckBoxOverride);

  UiTestApplication application(config);

  CheckBoxStyle style = CheckBoxStyle::Default();
  DALI_TEST_CHECK(style);
  DALI_TEST_EQUALS(style.GetIconWidth(), 99.0f, TEST_LOCATION);  // resolved from the override
  DALI_TEST_EQUALS(style.GetIconHeight(), 99.0f, TEST_LOCATION); // resolved from the override
  END_TEST;
}

int UtcDaliCheckBoxAccessibilityP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  CheckBox          checkBox = CheckBox::New("Receive notifications");

  // The CheckBox root must represent the label, Lottie icon, and selection state. Verify
  // the initial accessibility-tree contract: the root is discoverable as CHECK_BOX and its
  // internal visual children are not announced as separate items.
  DALI_TEST_CHECK(checkBox.GetAccessibilityRole() == UiAccessibility::Role::CHECK_BOX);
  DALI_TEST_CHECK(checkBox.IsAccessibilityHighlightable());
  DALI_TEST_EQUALS(checkBox.GetChildCount(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(checkBox.GetChildViewAt(0u).IsAccessibilityHidden());
  DALI_TEST_CHECK(checkBox.GetChildViewAt(1u).IsAccessibilityHidden());
  DALI_TEST_CHECK(checkBox.HasAccessibilityState(UiAccessibility::State::ENABLED));
  DALI_TEST_CHECK(!checkBox.HasAccessibilityState(UiAccessibility::State::CHECKED));

  auto* accessible = Dali::Accessibility::Accessible::Get(checkBox);
  DALI_TEST_CHECK(accessible);

  // Use label text as a dynamic fallback only when no explicit name exists. The explicit
  // accessibility name must remain unchanged after the application updates displayed text.
  DALI_TEST_EQUALS(accessible->GetName(), std::string("Receive notifications"), TEST_LOCATION);
  checkBox.SetAccessibilityName("Notification preference");
  checkBox.SetText("Marketing notifications");
  DALI_TEST_EQUALS(accessible->GetName(), std::string("Notification preference"), TEST_LOCATION);

  int            selectionCount   = 0;
  int            clickCount       = 0;
  InputEventType lastEventType    = InputEventType::NONE;
  bool           lastSelected     = false;
  bool           wasProgrammatic  = true;
  bool           receivedSameView = true;
  checkBox.SelectionChangedSignal().Connect(&application, [&selectionCount, &lastEventType, &lastSelected, &wasProgrammatic, &receivedSameView, checkBox](View view, bool selected, InputEvent event)
  {
    ++selectionCount;
    lastEventType    = event.GetInputEventType();
    lastSelected     = selected;
    wasProgrammatic  = event.IsProgrammatic();
    receivedSameView = receivedSameView && view == checkBox;
  });
  checkBox.ClickedSignal().Connect(&application, [&clickCount](View, InputEvent)
  {
    ++clickCount;
  });

  Property::Map attributes;

  // Screen Reader activation must use the existing SelectableTrait toggle policy through
  // ClickedSignal on the foundation's default interactive path. Verify that logical selection
  // and the AT-SPI CHECKED state change together and preserve the accessibility input source.
  DALI_TEST_CHECK(checkBox.DoAction("activate", attributes));
  DALI_TEST_CHECK(checkBox.IsSelected());
  DALI_TEST_CHECK(checkBox.HasAccessibilityState(UiAccessibility::State::CHECKED));
  DALI_TEST_EQUALS(selectionCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(clickCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(lastSelected);
  DALI_TEST_CHECK(lastEventType == InputEventType::ACCESSIBILITY_ACTIVATION);
  DALI_TEST_CHECK(!wasProgrammatic);
  DALI_TEST_CHECK(receivedSameView);

  DALI_TEST_CHECK(checkBox.DoAction("activate", attributes));
  DALI_TEST_CHECK(!checkBox.IsSelected());
  DALI_TEST_CHECK(!checkBox.HasAccessibilityState(UiAccessibility::State::CHECKED));
  DALI_TEST_EQUALS(selectionCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(clickCount, 2, TEST_LOCATION);
  DALI_TEST_CHECK(!lastSelected);

  // Disabling toggle-by-click stops only automatic selection; the InteractiveTrait click
  // contract remains. A Screen Reader double tap must reach the application click callback
  // without changing selection or CHECKED as a side effect.
  checkBox.SetToggleByClickEnabled(false);
  DALI_TEST_CHECK(checkBox.DoAction("activate", attributes));
  DALI_TEST_CHECK(!checkBox.IsSelected());
  DALI_TEST_EQUALS(selectionCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(clickCount, 3, TEST_LOCATION);

  // SetClickable(false) is the actual click-blocking policy and must not invoke the application
  // callback. DoAction's return value can include successful focus requests, so verify the signal count.
  checkBox.SetClickable(false);
  checkBox.DoAction("activate", attributes);
  DALI_TEST_EQUALS(clickCount, 3, TEST_LOCATION);

  // Verify that inherited SetEnabled() and the accessibility ENABLED state stay synchronized
  // in both directions during the same transition.
  checkBox.SetEnabled(false);
  DALI_TEST_CHECK(!checkBox.HasAccessibilityState(UiAccessibility::State::ENABLED));
  checkBox.SetEnabled(true);
  DALI_TEST_CHECK(checkBox.HasAccessibilityState(UiAccessibility::State::ENABLED));
  END_TEST;
}
