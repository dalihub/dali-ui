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
#include <dali-ui-components/public-api/dialog/alert-dialog.h>
#include <dali-ui-components/public-api/dialog/dialog.h>
#include <dali-ui-components/public-api/styles/alert-dialog-style.h>
#include <dali-ui-components/public-api/components-ui-config.h>
#include <dali-ui-components/integration-api/dialog/alert-dialog-impl.h>
#include <dali/devel-api/object/type-registry.h>
#include <limits>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_alert_dialog_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_alert_dialog_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliAlertDialogStyleContentAndActionsP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto buttonStyle = TextButtonStyle::Default().Configure()
    .SetFontSize(25.0f).SetTextColor(UiColor(Color::RED))
    .SetBackgroundColor(UiColor(Color::GREEN)).Build();
  auto style = AlertDialogStyle::Builder()
    .SetDialogStyle(DialogStyle::Builder().SetSpacing(13.0f).SetPadding(Insets(6.0f, 6.0f)).Build())
    .SetTitleFontSize(30.0f).SetTitleTextColor(UiColor(Color::BLUE))
    .SetMessageFontSize(20.0f).SetMessageTextColor(UiColor(Color::YELLOW))
    .SetActionButtonStyle(buttonStyle).SetActionRowHeight(80.0f).SetActionButtonSpacing(14.0f).Build();
  auto dialog = AlertDialog::New(style);
  dialog.SetTitle("Title");
  dialog.SetMessage("Message");
  auto title = Label::DownCast(dialog.GetHeaderView());
  auto message = Label::DownCast(dialog.GetBodyView());
  DALI_TEST_EQUALS(dialog.GetSpacing(), 13.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(title.GetFontSize(), 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(title.GetTextColor().GetRgba(), Color::BLUE, TEST_LOCATION);
  DALI_TEST_EQUALS(dialog.GetPadding(), Insets(6.0f, 6.0f), TEST_LOCATION);
  title.SetPadding(Insets(7.0f, 9.0f));
  message.SetPadding(Insets(11.0f, 13.0f));
  DALI_TEST_EQUALS(message.GetFontSize(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(message.GetTextColor().GetRgba(), Color::YELLOW, TEST_LOCATION);
  title.SetFontSize(33.0f);
  dialog.SetTitle("Updated");
  DALI_TEST_CHECK(dialog.GetHeaderView() == title);
  DALI_TEST_EQUALS(title.GetFontSize(), 33.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(title.GetPadding(), Insets(7.0f, 9.0f), TEST_LOCATION);
  dialog.SetMessage("Updated message");
  DALI_TEST_CHECK(dialog.GetBodyView() == message);
  DALI_TEST_EQUALS(message.GetPadding(), Insets(11.0f, 13.0f), TEST_LOCATION);
  auto action = dialog.AddActionButton("Action");
  DALI_TEST_EQUALS(action.GetFontSize(), 25.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(action.GetTextColor().GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(action.GetBackgroundColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  auto row = StackLayout::DownCast(dialog.GetFooterView());
  DALI_TEST_EQUALS(row.GetRequestedHeight(), 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(row.GetSpacing(), 14.0f, TEST_LOCATION);
  dialog.ClearActionButtons();
  DALI_TEST_EQUALS(dialog.AddActionButton("Again").GetFontSize(), 25.0f, TEST_LOCATION);
  auto copy = style.Configure().SetTitleFontSize(41.0f).Build();
  auto other = AlertDialog::New(copy);
  other.SetTitle("Other");
  DALI_TEST_EQUALS(Label::DownCast(other.GetHeaderView()).GetFontSize(), 41.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetTitleFontSize(), 30.0f, TEST_LOCATION);
  END_TEST;
}

namespace
{
class DerivedAlertDialogImpl : public Dali::Ui::Integration::AlertDialogImpl
{
public:
  static AlertDialog Create()
  {
    IntrusivePtr<DerivedAlertDialogImpl> impl = new DerivedAlertDialogImpl();
    // Establish ownership through the public extension API before initialization.
    CustomActor handle(*impl);
    impl->Initialize();
    return AlertDialog::DownCast(handle);
  }

protected:
  DerivedAlertDialogImpl() = default;
  ~DerivedAlertDialogImpl() override = default;
};

Dali::TypeRegistration derivedAlertDialogTypeRegistration(
  typeid(DerivedAlertDialogImpl), typeid(Dali::Ui::Integration::AlertDialogImpl), nullptr);

UiStyle CreateAlertStyleFixture()
{
  return AlertDialogStyle::DefaultPreset().Configure()
    .SetTitleFontSize(29.0f).SetTitleFontFamily("DejaVu Sans")
    .SetMessageFontFamily("DejaVu Sans").SetMessageFontSize(21.0f).Build();
}
UiStyle CreateAlertButtonFixture()
{
  return TextButtonStyle::DefaultPreset().Configure().SetPadding(Insets(17.0f, 19.0f))
    .SetFontSize(35.0f).SetBackgroundColor(UiColor(Color::YELLOW)).Build();
}
UiStyle CreateWrongAlertStyleFixture()
{
  return DialogStyle::Builder().Build();
}
UiStyle CreateEmptyAlertStyleFixture()
{
  return UiStyle();
}
}

int UtcDaliAlertDialogStyleWrongProviderN(void)
{
  auto config = Components::UiConfig::New();
  config.StyleSheet().SetStyle(AlertDialogStyle::DefaultKey(), &CreateWrongAlertStyleFixture);
  UiTestApplication application(config);
  // UiStyleSheet rejects mismatched provider types as an empty result.
  DALI_TEST_CHECK(!config.GetStyle(AlertDialogStyle::DefaultKey()));
  DALI_TEST_CHECK(AlertDialogStyle::Default() == AlertDialogStyle::DefaultPreset());
  END_TEST;
}

int UtcDaliAlertDialogStyleEmptyProviderFallbackP(void)
{
  auto config = Components::UiConfig::New();
  config.StyleSheet().SetStyle(AlertDialogStyle::DefaultKey(), &CreateEmptyAlertStyleFixture);
  UiTestApplication application(config);
  auto alert = AlertDialog::New();
  alert.SetTitle("Fallback title");
  alert.SetMessage("Fallback message");
  DALI_TEST_EQUALS(Label::DownCast(alert.GetHeaderView()).GetFontSize(), 22.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Label::DownCast(alert.GetBodyView()).GetFontSize(), 16.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAlertDialogDerivedInitializationP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  // Extension factories use the protected constructor, not the base New().
  auto alert = DerivedAlertDialogImpl::Create();
  alert.SetTitle("Derived title");
  alert.SetMessage("Derived message");
  auto action = alert.AddActionButton("Derived action");
  DALI_TEST_EQUALS(Label::DownCast(alert.GetHeaderView()).GetText(), Dali::String("Derived title"), TEST_LOCATION);
  DALI_TEST_EQUALS(Label::DownCast(alert.GetHeaderView()).GetFontSize(), 22.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Label::DownCast(alert.GetBodyView()).GetText(), Dali::String("Derived message"), TEST_LOCATION);
  DALI_TEST_EQUALS(Label::DownCast(alert.GetBodyView()).GetFontSize(), 16.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(action.GetFontSize(), 16.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(action.GetBackgroundColor(), UiColor(0x3367D6u), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAlertDialogStyleMoveConsumptionAndCastP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto original = AlertDialogStyle::Builder().SetTitleFontSize(31.0f)
    .SetMessageFontSize(23.0f).SetTitleTextColor(UiColor::PRIMARY)
    .SetMessageTextColor(UiColor::ON_SURFACE).SetActionRowHeight(77.0f)
    .SetActionButtonSpacing(11.0f).Build();
  auto builder = original.Configure();
  AlertDialogStyle::Builder moved(std::move(builder));
  AlertDialogStyle::Builder assigned;
  assigned = std::move(moved);
  auto changed = std::move(assigned).SetTitleFontSize(35.0f).Build();
  DALI_TEST_ASSERTION(builder.SetTitleFontSize(1.0f), "");
  DALI_TEST_ASSERTION(moved.SetMessageFontSize(1.0f), "");
  DALI_TEST_ASSERTION(assigned.SetActionRowHeight(1.0f), "");
  DALI_TEST_ASSERTION(std::move(assigned).Build(), "");
  DALI_TEST_ASSERTION(AlertDialogStyle().Configure(), "");
  DALI_TEST_CHECK(AlertDialogStyle::DownCast(changed) == changed);
  DALI_TEST_CHECK(AlertDialogStyle::StaticDownCast(UiStyle(changed)) == changed);
  DALI_TEST_CHECK(!AlertDialogStyle::DownCast(DialogStyle::Builder().Build()));
  DALI_TEST_CHECK(!AlertDialogStyle::DownCast(BaseHandle()));
  DALI_TEST_EQUALS(original.GetTitleFontSize(), 31.0f, TEST_LOCATION);
  auto alert = AlertDialog::New(changed);
  alert.SetTitle("Title");
  alert.SetMessage("Message");
  alert.AddActionButton("Action");
  DALI_TEST_EQUALS(Label::DownCast(alert.GetHeaderView()).GetFontSize(), 35.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Label::DownCast(alert.GetHeaderView()).GetTextColor(), UiColor(UiColor::PRIMARY), TEST_LOCATION);
  DALI_TEST_EQUALS(Label::DownCast(alert.GetBodyView()).GetFontSize(), 23.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Label::DownCast(alert.GetBodyView()).GetTextColor(), UiColor(UiColor::ON_SURFACE), TEST_LOCATION);
  DALI_TEST_EQUALS(alert.GetFooterView().GetRequestedHeight(), 77.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(StackLayout::DownCast(alert.GetFooterView()).GetSpacing(), 11.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAlertDialogStyleNumericValidationN(void)
{
  UiTestApplication application(Components::UiConfig::New());
  const float invalid[] = {-1.0f, std::numeric_limits<float>::quiet_NaN(),
                           std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()};
  for(float value : invalid)
  {
    DALI_TEST_ASSERTION(AlertDialogStyle::Builder().SetTitleFontSize(value), "");
    DALI_TEST_ASSERTION(AlertDialogStyle::Builder().SetMessageFontSize(value), "");
    DALI_TEST_ASSERTION(AlertDialogStyle::Builder().SetActionRowHeight(value), "");
    DALI_TEST_ASSERTION(AlertDialogStyle::Builder().SetActionButtonSpacing(value), "");
  }
  auto alert = AlertDialog::New(AlertDialogStyle::Builder().SetTitleFontSize(0.0f)
    .SetMessageFontSize(0.0f).SetActionRowHeight(0.0f).SetActionButtonSpacing(0.0f).Build());
  alert.SetTitle("Zero font");
  alert.SetMessage("Zero font");
  alert.AddActionButton("Zero row");
  DALI_TEST_EQUALS(Label::DownCast(alert.GetHeaderView()).GetFontSize(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Label::DownCast(alert.GetBodyView()).GetFontSize(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(alert.GetFooterView().GetRequestedHeight(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(StackLayout::DownCast(alert.GetFooterView()).GetSpacing(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAlertDialogStyleProviderAndLegacyActionP(void)
{
  auto config = Components::UiConfig::New();
  config.StyleSheet().SetStyle(AlertDialogStyle::DefaultKey(), &CreateAlertStyleFixture);
  config.StyleSheet().SetStyle(TextButtonStyle::DefaultKey(), &CreateAlertButtonFixture);
  UiTestApplication application(config);
  auto alert = AlertDialog::New();
  alert.SetTitle("Title");
  alert.SetMessage("Body");
  DALI_TEST_EQUALS(Label::DownCast(alert.GetHeaderView()).GetFontSize(), 29.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Label::DownCast(alert.GetHeaderView()).GetFontFamily(), Dali::String("DejaVu Sans"), TEST_LOCATION);
  DALI_TEST_EQUALS(Label::DownCast(alert.GetBodyView()).GetFontSize(), 21.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Label::DownCast(alert.GetBodyView()).GetFontFamily(), Dali::String("DejaVu Sans"), TEST_LOCATION);
  auto action = alert.AddActionButton("Legacy action");
  DALI_TEST_EQUALS(action.GetPadding(), Insets(17.0f, 19.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(action.GetFontSize(), 16.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(action.GetBackgroundColor(), UiColor(0x3367D6u), TEST_LOCATION);
  auto custom = AlertDialog::New(AlertDialogStyle::Builder().SetActionButtonStyle(TextButtonStyle::Default()).Build());
  DALI_TEST_EQUALS(custom.AddActionButton("Explicit action").GetFontSize(), 35.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(TextButtonStyle::Default().GetFontSize(), 35.0f, TEST_LOCATION);
  auto explicitDefault = AlertDialog::New(AlertDialogStyle::Default());
  explicitDefault.SetTitle("Explicit default");
  DALI_TEST_EQUALS(Label::DownCast(explicitDefault.GetHeaderView()).GetFontSize(), 29.0f, TEST_LOCATION);
  auto preset = AlertDialog::New(AlertDialogStyle::DefaultPreset());
  preset.SetTitle("Preset");
  DALI_TEST_EQUALS(Label::DownCast(preset.GetHeaderView()).GetFontSize(), 22.0f, TEST_LOCATION);
  DALI_TEST_ASSERTION(AlertDialog::New(AlertDialogStyle()), "");
  DALI_TEST_ASSERTION(AlertDialogStyle::Builder().SetActionRowHeight(-1.0f), "");
  DALI_TEST_ASSERTION(AlertDialogStyle::Builder().SetMessageFontSize(-1.0f), "");
  END_TEST;
}

int UtcDaliAlertDialogConstructorP(void)
{
  UiTestApplication application;
  AlertDialog       alertDialog;
  DALI_TEST_CHECK(!alertDialog);
  END_TEST;
}

int UtcDaliAlertDialogNewP(void)
{
  UiTestApplication application;
  AlertDialog       alertDialog = AlertDialog::New();
  DALI_TEST_CHECK(alertDialog);
  END_TEST;
}

int UtcDaliAlertDialogCopyConstructorP(void)
{
  UiTestApplication application;
  AlertDialog       alertDialog = AlertDialog::New();
  AlertDialog       copy(alertDialog);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(alertDialog == copy);
  END_TEST;
}

int UtcDaliAlertDialogMoveConstructor(void)
{
  UiTestApplication application;
  AlertDialog       alertDialog = AlertDialog::New();
  DALI_TEST_EQUALS(1, alertDialog.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  AlertDialog moved = std::move(alertDialog);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!alertDialog);
  END_TEST;
}

int UtcDaliAlertDialogAssignmentOperatorP(void)
{
  UiTestApplication application;
  AlertDialog       alertDialog = AlertDialog::New();
  AlertDialog       copy;
  copy = alertDialog;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(alertDialog == copy);
  END_TEST;
}

int UtcDaliAlertDialogMoveAssignment(void)
{
  UiTestApplication application;
  AlertDialog       alertDialog = AlertDialog::New();
  DALI_TEST_EQUALS(1, alertDialog.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  AlertDialog moved;
  moved = std::move(alertDialog);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!alertDialog);
  END_TEST;
}

int UtcDaliAlertDialogDownCastP(void)
{
  UiTestApplication application;
  AlertDialog       alertDialog = AlertDialog::New();
  BaseHandle        object(alertDialog);
  AlertDialog       alertDialog2 = AlertDialog::DownCast(object);
  AlertDialog       alertDialog3 = DownCast<AlertDialog>(object);
  DALI_TEST_CHECK(alertDialog2);
  DALI_TEST_CHECK(alertDialog3);
  END_TEST;
}

int UtcDaliAlertDialogDownCastN(void)
{
  UiTestApplication application;
  BaseHandle        unInitializedObject;
  AlertDialog       alertDialog2 = AlertDialog::DownCast(unInitializedObject);
  AlertDialog       alertDialog3 = DownCast<AlertDialog>(unInitializedObject);
  DALI_TEST_CHECK(!alertDialog2);
  DALI_TEST_CHECK(!alertDialog3);
  END_TEST;
}

int UtcDaliAlertDialogTitleP(void)
{
  UiTestApplication application;
  AlertDialog       alertDialog = AlertDialog::New();
  DALI_TEST_CHECK(!alertDialog.GetHeaderView());

  alertDialog.SetTitle("Delete?");
  DALI_TEST_CHECK(alertDialog.GetTitle() == "Delete?");
  DALI_TEST_CHECK(alertDialog.GetHeaderView()); // header auto-created

  alertDialog.SetTitle("");
  DALI_TEST_CHECK(!alertDialog.GetHeaderView()); // empty clears the header
  END_TEST;
}

int UtcDaliAlertDialogMessageP(void)
{
  UiTestApplication application;
  AlertDialog       alertDialog = AlertDialog::New();
  DALI_TEST_CHECK(!alertDialog.GetBodyView());

  alertDialog.SetMessage("This cannot be undone.");
  DALI_TEST_CHECK(alertDialog.GetMessage() == "This cannot be undone.");
  DALI_TEST_CHECK(alertDialog.GetBodyView());

  alertDialog.SetMessage("");
  DALI_TEST_CHECK(!alertDialog.GetBodyView());
  END_TEST;
}

int UtcDaliAlertDialogActionButtonsP(void)
{
  UiTestApplication application;
  AlertDialog       alertDialog = AlertDialog::New();
  DALI_TEST_CHECK(!alertDialog.GetFooterView());

  int        cancelCalls  = 0;
  int        okCalls      = 0;
  TextButton cancelButton = alertDialog.AddActionButton("Cancel");
  TextButton okButton     = alertDialog.AddActionButton("OK");
  cancelButton.ClickedSignal().Connect(&application, [&cancelCalls](View, InputEvent)
  { ++cancelCalls; });
  okButton.ClickedSignal().Connect(&application, [&okCalls](View, InputEvent)
  { ++okCalls; });

  View footer = alertDialog.GetFooterView();
  DALI_TEST_CHECK(footer);
  DALI_TEST_EQUALS(2u, footer.GetChildCount(), TEST_LOCATION); // two buttons
  DALI_TEST_CHECK(TextButton::DownCast(footer.GetChildAt(0u)) == cancelButton);
  DALI_TEST_CHECK(TextButton::DownCast(footer.GetChildAt(1u)) == okButton);
  DALI_TEST_CHECK(cancelButton.GetText() == "Cancel");
  DALI_TEST_CHECK(okButton.GetText() == "OK");

  cancelButton.ClickedSignal().Emit(cancelButton, InputEvent::Programmatic());
  okButton.ClickedSignal().Emit(okButton, InputEvent::Programmatic());
  DALI_TEST_EQUALS(1, cancelCalls, TEST_LOCATION);
  DALI_TEST_EQUALS(1, okCalls, TEST_LOCATION);

  // Clearing removes the footer.
  alertDialog.ClearActionButtons();
  DALI_TEST_CHECK(!alertDialog.GetFooterView());

  // Adding after a clear recreates the footer and its action row.
  TextButton retryButton = alertDialog.AddActionButton("Retry");
  footer                 = alertDialog.GetFooterView();
  DALI_TEST_CHECK(footer);
  DALI_TEST_EQUALS(1u, footer.GetChildCount(), TEST_LOCATION);
  DALI_TEST_CHECK(TextButton::DownCast(footer.GetChildAt(0u)) == retryButton);
  END_TEST;
}

// Validates handle inheritance: an AlertDialog IS-A Dialog.
int UtcDaliAlertDialogIsADialogP(void)
{
  UiTestApplication application;
  AlertDialog       alertDialog = AlertDialog::New();

  // Implicit upcast to the base Dialog handle.
  Dialog asDialog = alertDialog;
  DALI_TEST_CHECK(asDialog);

  // Dialog::DownCast of an AlertDialog succeeds (it is a Dialog).
  BaseHandle object(alertDialog);
  Dialog     downCastDialog = Dialog::DownCast(object);
  DALI_TEST_CHECK(downCastDialog);

  // AlertDialog::DownCast of a plain Dialog fails (it is not an AlertDialog).
  Dialog      plainDialog = Dialog::New();
  BaseHandle  plainObject(plainDialog);
  AlertDialog notAnAlert = AlertDialog::DownCast(plainObject);
  DALI_TEST_CHECK(!notAnAlert);
  END_TEST;
}

int UtcDaliAlertDialogTitleUpdatePreservesLabelP(void)
{
  UiTestApplication application;
  AlertDialog dialog = AlertDialog::New();
  dialog.SetTitle("Original");
  Label label = Label::DownCast(dialog.GetHeaderView());
  label.SetFontSize(30.0f);
  label.SetTextColor(UiColor(0xFF0000u));
  dialog.SetTitle("Updated");
  DALI_TEST_CHECK(dialog.GetHeaderView() == label);
  DALI_TEST_EQUALS(label.GetText(), std::string("Updated"), TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetFontSize(), 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextColor(), UiColor(0xFF0000u), TEST_LOCATION);
  dialog.SetTitle("Updated");
  DALI_TEST_CHECK(dialog.GetHeaderView() == label);
  dialog.SetTitle("");
  DALI_TEST_CHECK(!dialog.GetHeaderView());
  dialog.SetTitle("Recreated");
  DALI_TEST_CHECK(dialog.GetHeaderView() != label);
  Label custom = Label::New("Custom");
  dialog.SetHeaderView(custom);
  dialog.SetTitle("Replaced");
  DALI_TEST_CHECK(dialog.GetHeaderView() != custom);
  DALI_TEST_EQUALS(custom.GetText(), std::string("Custom"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAlertDialogMessageUpdatePreservesLabelP(void)
{
  UiTestApplication application;
  AlertDialog dialog = AlertDialog::New();
  dialog.SetMessage("Original");
  Label label = Label::DownCast(dialog.GetBodyView());
  label.SetFontSize(25.0f);
  label.SetTextColor(UiColor(0x0000FFu));
  dialog.SetMessage("Updated");
  DALI_TEST_CHECK(dialog.GetBodyView() == label);
  DALI_TEST_EQUALS(label.GetText(), std::string("Updated"), TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetFontSize(), 25.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextColor(), UiColor(0x0000FFu), TEST_LOCATION);
  dialog.SetMessage("Updated");
  DALI_TEST_CHECK(dialog.GetBodyView() == label);
  dialog.SetMessage("");
  DALI_TEST_CHECK(!dialog.GetBodyView());
  dialog.SetMessage("Recreated");
  DALI_TEST_CHECK(dialog.GetBodyView() != label);
  Label custom = Label::New("Custom");
  dialog.SetBodyView(custom);
  dialog.SetMessage("Replaced");
  DALI_TEST_CHECK(dialog.GetBodyView() != custom);
  DALI_TEST_EQUALS(custom.GetText(), std::string("Custom"), TEST_LOCATION);
  END_TEST;
}
