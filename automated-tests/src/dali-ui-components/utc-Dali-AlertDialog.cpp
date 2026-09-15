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
#include <dali-ui-components/public-api/navigator/navigator.h>
#include <dali-ui-components/public-api/styles/alert-dialog-style.h>
#include <dali-ui-components/public-api/components-ui-config.h>
#include <dali-ui-components/integration-api/dialog/alert-dialog-impl.h>
#include <dali/devel-api/object/type-registry.h>
#include <limits>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

int UtcDaliAlertDialogOneUiLayoutDefaultsP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto dialog = AlertDialog::New();
  dialog.SetTitle("Title");
  dialog.SetMessage("Message");
  auto action = dialog.AddActionButton("OK");
  DALI_TEST_EQUALS(dialog.GetPadding(), Insets(44.0f, 44.0f, 36.0f, 32.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(dialog.GetRequestedWidth(), 908.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(dialog.GetRequestedHeight(), WRAP_CONTENT, TEST_LOCATION);
  DALI_TEST_EQUALS(dialog.GetSpacing(), 12.0f, TEST_LOCATION);
  DALI_TEST_CHECK(Label::DownCast(dialog.GetHeaderView()).IsMultiLine());
  DALI_TEST_CHECK(Label::DownCast(dialog.GetBodyView()).IsMultiLine());
  DALI_TEST_EQUALS(dialog.GetHeaderView().GetPadding(), Insets(), TEST_LOCATION);
  DALI_TEST_EQUALS(dialog.GetBodyView().GetPadding(), Insets(), TEST_LOCATION);
  DALI_TEST_EQUALS(dialog.GetFooterView().GetRequestedHeight(), WRAP_CONTENT, TEST_LOCATION);
  DALI_TEST_EQUALS(dialog.GetFooterView().GetPadding(), Insets(0.0f, 0.0f, 12.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(action.GetMinimumWidth(), 144.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(action.GetMinimumHeight(), 64.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAlertDialogActionRowStyleOverridesP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto original = AlertDialogStyle::Default().Configure()
    .SetActionRowPadding(Insets(3.0f, 5.0f, 7.0f, 9.0f)).SetActionRowHeight(WRAP_CONTENT).Build();
  auto changed = original.Configure().SetActionRowPadding(Insets()).SetActionRowHeight(90.0f).Build();
  auto dialog = AlertDialog::New(original);
  dialog.AddActionButton("OK");
  DALI_TEST_EQUALS(dialog.GetFooterView().GetPadding(), original.GetActionRowPadding(), TEST_LOCATION);
  auto customFooter = View::New();
  customFooter.SetPadding(Insets(17.0f, 19.0f));
  dialog.SetFooterView(customFooter);
  DALI_TEST_EQUALS(customFooter.GetPadding(), Insets(17.0f, 19.0f), TEST_LOCATION);
  dialog.AddActionButton("New group");
  DALI_TEST_EQUALS(dialog.GetFooterView().GetPadding(), Insets(3.0f, 5.0f, 7.0f, 9.0f), TEST_LOCATION);
  auto other = AlertDialog::New(changed);
  other.AddActionButton("OK");
  DALI_TEST_EQUALS(other.GetFooterView().GetPadding(), Insets(), TEST_LOCATION);
  DALI_TEST_EQUALS(other.GetFooterView().GetRequestedHeight(), 90.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetActionRowHeight(), WRAP_CONTENT, TEST_LOCATION);
  for(float value : {-1.0f, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::quiet_NaN()})
  {
    DALI_TEST_ASSERTION(AlertDialogStyle::Builder().SetActionRowPadding(Insets(value, 0, 0, 0)), "");
    DALI_TEST_ASSERTION(AlertDialogStyle::Builder().SetActionRowPadding(Insets(0, value, 0, 0)), "");
    DALI_TEST_ASSERTION(AlertDialogStyle::Builder().SetActionRowPadding(Insets(0, 0, value, 0)), "");
    DALI_TEST_ASSERTION(AlertDialogStyle::Builder().SetActionRowPadding(Insets(0, 0, 0, value)), "");
  }
  END_TEST;
}

int UtcDaliAlertDialogOneUiLayoutGeometryP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto host = Navigator::New();
  host.SetRequestedWidth(480.0f);
  host.SetRequestedHeight(800.0f);
  application.GetScene().Add(host);
  auto dialog = AlertDialog::New();
  dialog.SetRequestedWidth(440.0f);
  dialog.SetLayoutParams(AbsoluteLayoutParams::New()
    .SetBounds(LayoutRect(0.5f, 0.5f, WRAP_CONTENT, WRAP_CONTENT))
    .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));
  dialog.SetTitle("Delete item?");
  dialog.SetMessage("This action cannot be undone.");
  auto first = dialog.AddActionButton("Cancel");
  auto second = dialog.AddActionButton("OK");
  DialogPostOptions options;
  options.animated = false;
  float shortHeight = 0.0f;
  float shortTitleHeight = 0.0f;
  for(bool longText : {false, true})
  {
    if(longText)
    {
      dialog.SetTitle("A long title that must wrap inside the dialog instead of crossing its rounded boundary");
      dialog.SetMessage("A long message that must grow vertically and move the action row down without overlapping the title or the buttons. The dialog height follows its content.");
    }
    DALI_TEST_CHECK(dialog.Post(host, options));
    host.Measure(480.0f, 800.0f);
    host.Arrange(LayoutRect(0, 0, 480, 800));
    application.SendNotification();
    application.Render(500);
    application.SendNotification();
    application.Render(500);
    auto title = dialog.GetHeaderView();
    auto body = dialog.GetBodyView();
    auto footer = dialog.GetFooterView();
    DALI_TEST_EQUALS(title.GetCurrentPosition().x, 44.0f, 0.01f, TEST_LOCATION);
    DALI_TEST_EQUALS(title.GetCurrentPosition().y, 36.0f, 0.01f, TEST_LOCATION);
    DALI_TEST_EQUALS(title.GetCurrentSize().x, 352.0f, 0.01f, TEST_LOCATION);
    DALI_TEST_EQUALS(body.GetCurrentPosition().y, 36.0f + title.GetCurrentSize().y + 12.0f, 0.01f, TEST_LOCATION);
    DALI_TEST_EQUALS(footer.GetCurrentPosition().y, body.GetCurrentPosition().y + body.GetCurrentSize().y + 12.0f, 0.01f, TEST_LOCATION);
    DALI_TEST_EQUALS(dialog.GetCurrentSize().y, footer.GetCurrentPosition().y + footer.GetCurrentSize().y + 32.0f, 0.01f, TEST_LOCATION);
    DALI_TEST_EQUALS(first.GetCurrentSize().x, 144.0f, 0.01f, TEST_LOCATION);
    DALI_TEST_EQUALS(second.GetCurrentSize().x, 144.0f, 0.01f, TEST_LOCATION);
    DALI_TEST_EQUALS(first.GetCurrentSize().y, 64.0f, 0.01f, TEST_LOCATION);
    DALI_TEST_EQUALS(first.GetCurrentPosition().y, 12.0f, 0.01f, TEST_LOCATION);
    DALI_TEST_EQUALS(second.GetCurrentPosition().x - first.GetCurrentPosition().x - first.GetCurrentSize().x, 32.0f, 0.01f, TEST_LOCATION);
    DALI_TEST_EQUALS(first.GetCurrentPosition().x, footer.GetCurrentSize().x - second.GetCurrentPosition().x - second.GetCurrentSize().x, 0.01f, TEST_LOCATION);
    DALI_TEST_EQUALS(dialog.GetCurrentPosition().y, (800.0f - dialog.GetCurrentSize().y) * 0.5f, 0.01f, TEST_LOCATION);
    if(longText)
    {
      DALI_TEST_CHECK(title.GetCurrentSize().y > shortTitleHeight);
      DALI_TEST_CHECK(dialog.GetCurrentSize().y > shortHeight);
    }
    else
    {
      shortHeight = dialog.GetCurrentSize().y;
      shortTitleHeight = title.GetCurrentSize().y;
    }
    dialog.Dismiss(false);
  }
  END_TEST;
}

int UtcDaliAlertDialogOneUiLayoutRtlAndRebuildP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto dialog = AlertDialog::New();
  dialog.SetProperty(Actor::Property::LAYOUT_DIRECTION, LayoutDirection::RIGHT_TO_LEFT);
  dialog.SetRequestedWidth(440.0f);
  dialog.SetRequestedHeight(WRAP_CONTENT);
  application.GetScene().Add(dialog);
  dialog.SetTitle("Title");
  auto first = dialog.AddActionButton("OK");
  auto second = dialog.AddActionButton("Cancel");
  auto third = dialog.AddActionButton("Third");
  // A wider dialog accommodates three natural-size actions without wrapping.
  dialog.SetRequestedWidth(700.0f);
  auto arrange = [&]() {
    auto measured = dialog.Measure(700.0f, 800.0f);
    dialog.Arrange(LayoutRect(0, 0, measured.width, measured.height));
    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();
  };
  arrange();
  DALI_TEST_CHECK(first.GetCurrentPosition().x > second.GetCurrentPosition().x);
  DALI_TEST_CHECK(second.GetCurrentPosition().x > third.GetCurrentPosition().x);
  DALI_TEST_EQUALS(first.GetCurrentPosition().x - second.GetCurrentPosition().x - second.GetCurrentSize().x, 32.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(second.GetCurrentPosition().x - third.GetCurrentPosition().x - third.GetCurrentSize().x, 32.0f, 0.01f, TEST_LOCATION);
  auto footer = dialog.GetFooterView();
  DALI_TEST_EQUALS(third.GetCurrentPosition().x, footer.GetCurrentSize().x - first.GetCurrentPosition().x - first.GetCurrentSize().x, 0.01f, TEST_LOCATION);
  auto title = dialog.GetHeaderView();
  DALI_TEST_EQUALS(footer.GetCurrentPosition().y, title.GetCurrentPosition().y + title.GetCurrentSize().y + 12.0f, 0.01f, TEST_LOCATION);
  dialog.SetTitle("");
  dialog.ClearActionButtons();
  auto only = dialog.AddActionButton("Only");
  arrange();
  footer = dialog.GetFooterView();
  DALI_TEST_EQUALS(footer.GetChildCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(only.GetMargin(), Insets(), TEST_LOCATION);
  DALI_TEST_EQUALS(footer.GetCurrentPosition().y, 36.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(only.GetCurrentPosition().x, (footer.GetCurrentSize().x - only.GetCurrentSize().x) * 0.5f, 0.01f, TEST_LOCATION);
  END_TEST;
}

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
  auto row = dialog.GetFooterView();
  DALI_TEST_EQUALS(row.GetRequestedHeight(), 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(dialog.AddActionButton("Second").GetMargin().start, 14.0f, TEST_LOCATION);
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
  DALI_TEST_EQUALS(alert.AddActionButton("Second").GetMargin().start, 11.0f, TEST_LOCATION);
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
    if(value != WRAP_CONTENT)
    {
      DALI_TEST_ASSERTION(AlertDialogStyle::Builder().SetActionRowHeight(value), "");
    }
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
  DALI_TEST_EQUALS(alert.AddActionButton("Second").GetMargin().start, 0.0f, TEST_LOCATION);
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
  DALI_TEST_ASSERTION(AlertDialogStyle::Builder().SetActionRowHeight(MATCH_PARENT), "");
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
