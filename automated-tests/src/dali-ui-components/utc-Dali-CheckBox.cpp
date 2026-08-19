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
#include <dali-ui-foundation/public-api/types/selectable-lottie-image.h>
#include <dali-ui-foundation/public-api/views/image/selectable-image-interface.h>
#include <dali-ui-foundation/public-api/views/image/selectable-lottie-animation-view.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>
#include <dali-ui-test-suite-utils.h>

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
  : mCount(count),
    mLast(last)
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

// ---------------------------------------------------------------------------
// Layout fixture shared by the arrange tests below.
//
// Every number is chosen so that the LEFT_TO_RIGHT and the RIGHT_TO_LEFT placements
// are distinct and asymmetric: start padding differs from end padding, so a layout
// that forgets to flip (or flips twice) cannot accidentally land on the mirrored
// answer.
constexpr float FIXTURE_WIDTH      = 200.0f;
constexpr float FIXTURE_HEIGHT     = 50.0f;
constexpr float FIXTURE_PAD_START  = 10.0f;
constexpr float FIXTURE_PAD_END    = 4.0f;
constexpr float FIXTURE_PAD_TOP    = 2.0f;
constexpr float FIXTURE_PAD_BOTTOM = 2.0f;
constexpr float FIXTURE_ICON       = 20.0f; // square glyph
constexpr float FIXTURE_GAP        = 5.0f;

// Derived, at an effective scale of 1: the content band is [10, 196) and 46 tall.
constexpr float FIXTURE_CONTENT_W = FIXTURE_WIDTH - (FIXTURE_PAD_START + FIXTURE_PAD_END);   // 186
constexpr float FIXTURE_CONTENT_H = FIXTURE_HEIGHT - (FIXTURE_PAD_TOP + FIXTURE_PAD_BOTTOM); // 46
constexpr float FIXTURE_LABEL_W   = FIXTURE_CONTENT_W - FIXTURE_ICON - FIXTURE_GAP;          // 161

// LOGICAL (left-to-right) x of each child, i.e. what CheckBoxImpl::OnArrange produces.
constexpr float FIXTURE_LOGICAL_ICON_X  = FIXTURE_PAD_START;                              // 10
constexpr float FIXTURE_LOGICAL_LABEL_X = FIXTURE_PAD_START + FIXTURE_ICON + FIXTURE_GAP; // 35

// The mirror the framework applies once under RIGHT_TO_LEFT: x' = W - x - w.
constexpr float Mirrored(float x, float w)
{
  return FIXTURE_WIDTH - x - w;
}

CheckBoxStyle MakeFixtureStyle()
{
  return CheckBoxStyle::Builder()
    .SetPadding(Insets(FIXTURE_PAD_START, FIXTURE_PAD_END, FIXTURE_PAD_TOP, FIXTURE_PAD_BOTTOM))
    .SetIconWidth(FIXTURE_ICON)
    .SetIconHeight(FIXTURE_ICON)
    .SetLabelGap(FIXTURE_GAP)
    .SetIconGenerator(CheckBoxStyle::IconGenerator::New(&MakeTestIcon))
    .SetStateEffect(StateEffect::None()) // no overlay actor/visual in the way
    .Build();
}

// The trailing label is not exposed on the handle, so it is located by type among the
// CheckBox's direct children (the only other one is the icon's drawing view).
Ui::Label FindLabelChild(Ui::View parent)
{
  for(uint32_t i = 0, count = parent.GetChildCount(); i < count; ++i)
  {
    Ui::Label label = Ui::Label::DownCast(parent.GetChildAt(i));
    if(label)
    {
      return label;
    }
  }
  return Ui::Label();
}

// The icon's drawing view is the only direct child that is not the trailing label.
Ui::View FindIconChild(Ui::View parent)
{
  for(uint32_t i = 0, count = parent.GetChildCount(); i < count; ++i)
  {
    Dali::Actor child = parent.GetChildAt(i);
    if(!Ui::Label::DownCast(child))
    {
      return Ui::View::DownCast(child);
    }
  }
  return Ui::View();
}

// Measures and arranges a CheckBox as a layout root into the fixture slot.
void ArrangeFixture(CheckBox cb)
{
  cb.Measure(FIXTURE_WIDTH, FIXTURE_HEIGHT);
  cb.Arrange(LayoutRect(0.0f, 0.0f, FIXTURE_WIDTH, FIXTURE_HEIGHT));
}

float PositionX(Dali::Actor actor)
{
  return actor.GetProperty<float>(Dali::Actor::Property::POSITION_X);
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

// ---------------------------------------------------------------------------
// RTL: the icon and the label are mirrored EXACTLY ONCE.
//
// CheckBoxImpl::OnArrange places its two children in the LOGICAL (left-to-right)
// frame and mirrors nothing. The framework does the mirroring, once per pass, in
// ViewDataImpl::ApplyLayoutDirection: after the producer returns it flips every
// direct, non-standalone child's x about this view's arranged width
// (x' = W - x - w), reading the child's LOGICAL arranged bounds. Both children
// here are direct non-standalone Ui::View children of the CheckBox, so both are
// flipped there.
//
// Under LEFT_TO_RIGHT the mirror is skipped entirely, so the logical placement is
// also the final placement -- which is why the same fixture is asserted in both
// directions: it pins that the fix changed RTL only.
//
// Non-vacuity (verified by mutation): restoring CheckBoxImpl::OnArrange's own
// mirror -- the `std::swap(padding.start, padding.end)` plus the `mapX` flip it
// used to apply under RIGHT_TO_LEFT -- makes this producer's flip and the
// framework's flip cancel, and the RIGHT_TO_LEFT half of this test then reports
// exactly the LEFT_TO_RIGHT numbers (icon x 10, label x 35 instead of 170 and 4).
int UtcDaliCheckBoxArrangeMirrorsChildrenExactlyOnceUnderRtlP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  tet_infoline("A CheckBox under RIGHT_TO_LEFT places its icon and label mirrored exactly once");

  // --- LEFT_TO_RIGHT: icon leads at the start padding, label follows after the gap.
  CheckBox ltr = CheckBox::New("Agree", MakeFixtureStyle());
  ltr.SetRequestedWidth(FIXTURE_WIDTH);
  ltr.SetRequestedHeight(FIXTURE_HEIGHT);
  application.GetScene().Add(ltr);
  ArrangeFixture(ltr);

  Ui::View  ltrIcon  = FindIconChild(ltr);
  Ui::Label ltrLabel = FindLabelChild(ltr);
  DALI_TEST_CHECK(ltrIcon);
  DALI_TEST_CHECK(ltrLabel);

  DALI_TEST_EQUALS(ltr.GetEffectiveLayoutDirection(), Dali::LayoutDirection::LEFT_TO_RIGHT, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionX(ltrIcon), FIXTURE_LOGICAL_ICON_X, TEST_LOCATION);   // 10
  DALI_TEST_EQUALS(PositionX(ltrLabel), FIXTURE_LOGICAL_LABEL_X, TEST_LOCATION); // 35
  DALI_TEST_EQUALS(ltrIcon.GetProperty<float>(Dali::Actor::Property::SIZE_WIDTH), FIXTURE_ICON, TEST_LOCATION);
  DALI_TEST_EQUALS(ltrLabel.GetProperty<float>(Dali::Actor::Property::SIZE_WIDTH), FIXTURE_LABEL_W, TEST_LOCATION);

  // The icon is vertically centred in the content band; both children sit below the top
  // padding. Neither axis is touched by layout direction, so they are asserted once.
  DALI_TEST_EQUALS(ltrIcon.GetProperty<float>(Dali::Actor::Property::POSITION_Y),
                   FIXTURE_PAD_TOP + (FIXTURE_CONTENT_H - FIXTURE_ICON) * 0.5f,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(ltrLabel.GetProperty<float>(Dali::Actor::Property::POSITION_Y), FIXTURE_PAD_TOP, TEST_LOCATION);

  // --- RIGHT_TO_LEFT: the same logical placement, flipped once about the width.
  CheckBox rtl = CheckBox::New("Agree", MakeFixtureStyle());
  rtl.SetRequestedWidth(FIXTURE_WIDTH);
  rtl.SetRequestedHeight(FIXTURE_HEIGHT);
  rtl.SetLayoutDirection(Dali::LayoutDirection::RIGHT_TO_LEFT);
  application.GetScene().Add(rtl);
  ArrangeFixture(rtl);

  Ui::View  rtlIcon  = FindIconChild(rtl);
  Ui::Label rtlLabel = FindLabelChild(rtl);
  DALI_TEST_CHECK(rtlIcon);
  DALI_TEST_CHECK(rtlLabel);

  DALI_TEST_EQUALS(rtl.GetEffectiveLayoutDirection(), Dali::LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);

  // Icon: leading means the RIGHT edge under RTL, start padding away from it.
  // 200 - 10 - 20 = 170, i.e. its right edge sits at 190 = 200 - start padding.
  DALI_TEST_EQUALS(PositionX(rtlIcon), Mirrored(FIXTURE_LOGICAL_ICON_X, FIXTURE_ICON), TEST_LOCATION);
  DALI_TEST_EQUALS(PositionX(rtlIcon), 170.0f, TEST_LOCATION);

  // Label: trailing means the LEFT under RTL, flush against the end padding.
  // 200 - 35 - 161 = 4 = end padding.
  DALI_TEST_EQUALS(PositionX(rtlLabel), Mirrored(FIXTURE_LOGICAL_LABEL_X, FIXTURE_LABEL_W), TEST_LOCATION);
  DALI_TEST_EQUALS(PositionX(rtlLabel), FIXTURE_PAD_END, TEST_LOCATION);

  // Sizes are direction-independent; the flip moves the children, it does not resize them.
  DALI_TEST_EQUALS(rtlIcon.GetProperty<float>(Dali::Actor::Property::SIZE_WIDTH), FIXTURE_ICON, TEST_LOCATION);
  DALI_TEST_EQUALS(rtlLabel.GetProperty<float>(Dali::Actor::Property::SIZE_WIDTH), FIXTURE_LABEL_W, TEST_LOCATION);

  // ...and the icon really is on the far side of the label, not merely somewhere else.
  DALI_TEST_CHECK(PositionX(rtlIcon) > PositionX(rtlLabel));
  DALI_TEST_CHECK(PositionX(ltrIcon) < PositionX(ltrLabel));

  END_TEST;
}

// ---------------------------------------------------------------------------
// A settled CheckBox serves its whole subtree from the arrange cache.
//
// CheckBox uses the default ArrangePolicy::IF_CHANGED, so re-arranging a settled CheckBox
// into the SAME slot elides its producer and replays the cached subtree instead
// (ViewDataImpl::ReplayArrangeSubtreeFromCache): each node is reconciled against its
// OWN cached bounds rather than against bounds the producer recomputes.
//
// That is observed here by moving one child out of band first. `label.Arrange(shifted)`
// republishes the LABEL's own arrange entry at a shifted x -- and only x, so no SIZE_*
// write reaches OnSizeSet and nothing invalidates the CheckBox. The subtree gate
// (CanReplayArrangeSubtreeFromCache) deliberately does NOT re-test a descendant's cache
// KEY, so the CheckBox still HITS, and the replay re-applies the label's own shifted
// entry. A MISS would instead re-run CheckBoxImpl::OnArrange, which hands the label the
// slot it computes and snaps it back.
//
// Non-vacuity (verified by mutation): selecting ALWAYS in the implementation
// constructor makes the second Arrange miss and pulls the label back to
// FIXTURE_LOGICAL_LABEL_X -- the final assertion fails.
int UtcDaliCheckBoxSettledArrangeIsServedFromCacheP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  tet_infoline("A settled CheckBox's same-slot Arrange is result-identical to a forced miss, out-of-band writes included");

  CheckBox cb = CheckBox::New("Agree", MakeFixtureStyle());
  cb.SetRequestedWidth(FIXTURE_WIDTH);
  cb.SetRequestedHeight(FIXTURE_HEIGHT);
  application.GetScene().Add(cb);

  ArrangeFixture(cb); // settles and publishes the arrange entry

  Ui::Label label = FindLabelChild(cb);
  Ui::View  icon  = FindIconChild(cb);
  DALI_TEST_CHECK(label);
  DALI_TEST_CHECK(icon);
  DALI_TEST_EQUALS(PositionX(label), FIXTURE_LOGICAL_LABEL_X, TEST_LOCATION);

  // An out-of-band public Arrange on the label rewrites the very records a
  // container hit would replay it from, so it retracts the CheckBox's entry: the
  // next same-slot Arrange must RE-RUN the producer and restore the producer's
  // slot, exactly as a forced miss would. (Serving the old entry here would make
  // hit and miss diverge, which View::Arrange's contract rules out.)
  const float shiftedLabelX = FIXTURE_LOGICAL_LABEL_X + 33.0f;
  label.Arrange(LayoutRect(shiftedLabelX, FIXTURE_PAD_TOP, FIXTURE_LABEL_W, FIXTURE_CONTENT_H));
  DALI_TEST_EQUALS(PositionX(label), shiftedLabelX, TEST_LOCATION);

  cb.Arrange(LayoutRect(0.0f, 0.0f, FIXTURE_WIDTH, FIXTURE_HEIGHT));

  DALI_TEST_EQUALS(PositionX(cb), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(cb.GetProperty<float>(Dali::Actor::Property::SIZE_WIDTH), FIXTURE_WIDTH, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionX(icon), FIXTURE_LOGICAL_ICON_X, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionX(label), FIXTURE_LOGICAL_LABEL_X, TEST_LOCATION);

  // Settled again, the entry is live: a bare ACTOR write (no arrange records
  // touched) does not retract it, and the served hit still reconciles the child
  // back onto its cached bounds -- a hit is not a no-op.
  label.SetProperty(Dali::Actor::Property::POSITION_X, shiftedLabelX);
  DALI_TEST_EQUALS(PositionX(label), shiftedLabelX, TEST_LOCATION);

  cb.Arrange(LayoutRect(0.0f, 0.0f, FIXTURE_WIDTH, FIXTURE_HEIGHT));
  DALI_TEST_EQUALS(PositionX(label), FIXTURE_LOGICAL_LABEL_X, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionX(icon), FIXTURE_LOGICAL_ICON_X, TEST_LOCATION);

  // And the equivalence all of the above serves: a forced miss lands on exactly
  // the same geometry.
  cb.InvalidateArrange();
  cb.Arrange(LayoutRect(0.0f, 0.0f, FIXTURE_WIDTH, FIXTURE_HEIGHT));
  DALI_TEST_EQUALS(PositionX(label), FIXTURE_LOGICAL_LABEL_X, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionX(icon), FIXTURE_LOGICAL_ICON_X, TEST_LOCATION);

  END_TEST;
}
