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
#include <dali-ui-components/public-api/dialog/dialog.h>
#include <dali-ui-components/public-api/styles/dialog-style.h>
#include <dali-ui-components/public-api/components-ui-config.h>
#include <limits>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_dialog_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_dialog_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliDialogStyleAppliedP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto style = DialogStyle::Builder()
    .SetRequestedWidth(320.0f).SetRequestedHeight(200.0f)
    .SetBackgroundColor(UiColor(Color::BLUE)).SetPadding(Insets(3.0f, 3.0f))
    .SetSpacing(12.0f)
    .SetCornerRadius(Vector4(1.0f, 2.0f, 3.0f, 4.0f))
    .SetBorderlineWidth(2.0f).SetBorderlineOffset(-1.0f)
    .SetBorderlineColor(UiColor(Color::RED))
    .SetShadow(ShadowStack{Shadow(8.0f)}).SetBackgroundBlurRadius(4.0f).Build();
  Dialog dialog = Dialog::New(style);
  DALI_TEST_EQUALS(dialog.GetRequestedWidth(), 320.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(dialog.GetRequestedHeight(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(dialog.GetSpacing(), 12.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(dialog.GetPadding(), Insets(3.0f, 3.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(dialog.GetBackgroundColor().GetRgba(), Color::BLUE, TEST_LOCATION);
  DALI_TEST_EQUALS(dialog.GetCornerRadius(), Vector4(1.0f, 2.0f, 3.0f, 4.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(dialog.GetBorderlineWidth(), 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(dialog.GetBorderlineOffset(), -1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(dialog.GetBorderlineColor().GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_CHECK(dialog.GetRenderEffect());
  auto header = View::New();
  auto body = View::New();
  auto footer = View::New();
  header.SetPadding(Insets(11.0f, 13.0f));
  body.SetPadding(Insets(17.0f, 19.0f));
  footer.SetPadding(Insets());
  dialog.SetHeaderView(header);
  dialog.SetBodyView(body);
  dialog.SetFooterView(footer);
  DALI_TEST_EQUALS(header.GetPadding(), Insets(11.0f, 13.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(body.GetPadding(), Insets(17.0f, 19.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(footer.GetPadding(), Insets(), TEST_LOCATION);
  header.SetPadding(Insets(11.0f, 11.0f));
  dialog.SetBodyView(View::New());
  DALI_TEST_EQUALS(header.GetPadding(), Insets(11.0f, 11.0f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliDialogStyleConfigureAndSectionPaddingPreservedP(void)
{
  UiTestApplication application;
  auto original = DialogStyle::Builder().SetSpacing(4.0f).SetPadding(Insets(3.0f, 5.0f)).Build();
  auto changed = original.Configure().SetSpacing(8.0f).SetPadding(Insets()).Build();
  auto first = Dialog::New(original);
  auto second = Dialog::New(changed);
  auto header = View::New();
  header.SetPadding(Insets(15.0f, 15.0f));
  first.SetHeaderView(header);
  DALI_TEST_EQUALS(header.GetPadding(), Insets(15.0f, 15.0f), TEST_LOCATION);
  second.SetHeaderView(header);
  DALI_TEST_EQUALS(header.GetPadding(), Insets(15.0f, 15.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(first.GetSpacing(), 4.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(second.GetSpacing(), 8.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(first.GetPadding(), Insets(3.0f, 5.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(second.GetPadding(), Insets(), TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetPadding(), Insets(3.0f, 5.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetSpacing(), 4.0f, TEST_LOCATION);
  END_TEST;
}

namespace
{
UiStyle CreateDialogFixtureStyle()
{
  return DialogStyle::DefaultPreset().Configure().SetSpacing(19.0f)
    .SetBackgroundColor(UiColor::PRIMARY).Build();
}
}

int UtcDaliDialogDefaultStyleProviderP(void)
{
  auto config = Components::UiConfig::New();
  config.StyleSheet().SetStyle(DialogStyle::DefaultKey(), &CreateDialogFixtureStyle);
  UiTestApplication application(config);
  auto dialog = Dialog::New();
  DALI_TEST_EQUALS(dialog.GetSpacing(), 19.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(dialog.GetBackgroundColor(), UiColor(UiColor::PRIMARY), TEST_LOCATION);
  DALI_TEST_EQUALS(Dialog::New(DialogStyle::DefaultPreset()).GetSpacing(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDialogStyleValidationN(void)
{
  UiTestApplication application;
  DALI_TEST_ASSERTION(Dialog::New(DialogStyle()), "");
  DALI_TEST_ASSERTION(DialogStyle::Builder().SetSpacing(-1.0f), "");
  DALI_TEST_ASSERTION(DialogStyle::Builder().SetRequestedWidth(std::numeric_limits<float>::infinity()), "");
  DALI_TEST_ASSERTION(DialogStyle::Builder().SetBackgroundBlurRadius(-1.0f), "");
  auto builder = DialogStyle::Builder();
  auto style = std::move(builder).Build();
  DALI_TEST_CHECK(style);
  DALI_TEST_ASSERTION(std::move(builder).Build(), "");
  DALI_TEST_CHECK(!DialogStyle::DownCast(View::New()));
  END_TEST;
}

int UtcDaliDialogStyleVisualValuesAndMoveP(void)
{
  UiTestApplication application;
  ShadowStack shadows{Shadow(8.0f)};
  auto original = DialogStyle::Builder().SetShadow(shadows)
    .SetCornerRadius(Vector4(0.1f, 0.2f, 0.3f, 0.4f))
    .SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE).Build();
  shadows.Clear();
  auto returned = original.GetShadow();
  returned.Clear();
  DALI_TEST_EQUALS(original.GetShadow().GetShadowCount(), 1u, TEST_LOCATION);
  auto builder = original.Configure();
  DialogStyle::Builder moved(std::move(builder));
  DialogStyle::Builder assigned;
  assigned = std::move(moved);
  auto copy = std::move(assigned).Build();
  auto dialog = Dialog::New(copy);
  DALI_TEST_EQUALS(dialog.GetCornerRadiusPolicy(), CornerRadiusPolicy::RELATIVE, TEST_LOCATION);
  DALI_TEST_EQUALS(dialog.GetCornerRadius(), Vector4(0.1f, 0.2f, 0.3f, 0.4f), TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetShadow().GetShadowCount(), 1u, TEST_LOCATION);
  DALI_TEST_ASSERTION(builder.SetSpacing(1.0f), "");
  DALI_TEST_ASSERTION(moved.SetSpacing(1.0f), "");
  END_TEST;
}

int UtcDaliDialogSectionAppearancePreservedP(void)
{
  UiTestApplication application;
  auto style = DialogStyle::Builder().SetPadding(Insets(3.0f, 5.0f))
    .SetSpacing(12.0f).SetBackgroundColor(UiColor(Color::BLUE))
    .SetCornerRadius(Vector4(4.0f, 4.0f, 4.0f, 4.0f)).Build();
  auto first = Dialog::New(style);
  auto second = Dialog::New(style.Configure().SetPadding(Insets()).SetSpacing(0.0f).Build());
  using SectionSetter = void (Dialog::*)(View);
  const SectionSetter setters[] = {&Dialog::SetHeaderView, &Dialog::SetBodyView, &Dialog::SetFooterView};
  for(auto setter : setters)
  {
    auto section = View::New();
    section.SetPadding(Insets(11.0f, 13.0f));
    section.SetBackgroundColor(UiColor(Color::RED));
    section.SetCornerRadius(Vector4(7.0f, 7.0f, 7.0f, 7.0f));
    (first.*setter)(section);
    DALI_TEST_EQUALS(section.GetPadding(), Insets(11.0f, 13.0f), TEST_LOCATION);
    DALI_TEST_EQUALS(section.GetBackgroundColor().GetRgba(), Color::RED, TEST_LOCATION);
    DALI_TEST_EQUALS(section.GetCornerRadius(), Vector4(7.0f, 7.0f, 7.0f, 7.0f), TEST_LOCATION);

    // Reinstalling the same view must also retain subsequent caller changes.
    section.SetPadding(Insets(17.0f, 19.0f));
    (first.*setter)(section);
    DALI_TEST_EQUALS(section.GetPadding(), Insets(17.0f, 19.0f), TEST_LOCATION);

    auto replacement = View::New();
    replacement.SetPadding(Insets());
    (first.*setter)(replacement);
    DALI_TEST_EQUALS(replacement.GetPadding(), Insets(), TEST_LOCATION);
    DALI_TEST_EQUALS(section.GetPadding(), Insets(17.0f, 19.0f), TEST_LOCATION);
    DALI_TEST_CHECK(!section.GetParent());
    (first.*setter)(View());
    DALI_TEST_CHECK(!replacement.GetParent());
    DALI_TEST_EQUALS(replacement.GetPadding(), Insets(), TEST_LOCATION);

    // A different dialog must not reinterpret the section's appearance.
    (second.*setter)(section);
    DALI_TEST_EQUALS(section.GetPadding(), Insets(17.0f, 19.0f), TEST_LOCATION);
    DALI_TEST_EQUALS(section.GetBackgroundColor().GetRgba(), Color::RED, TEST_LOCATION);
    DALI_TEST_EQUALS(section.GetCornerRadius(), Vector4(7.0f, 7.0f, 7.0f, 7.0f), TEST_LOCATION);
    (second.*setter)(View());
    (first.*setter)(section);
    DALI_TEST_EQUALS(section.GetPadding(), Insets(17.0f, 19.0f), TEST_LOCATION);
    (first.*setter)(View());
  }
  DALI_TEST_EQUALS(first.GetPadding(), Insets(3.0f, 5.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(first.GetSpacing(), 12.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(first.GetBackgroundColor().GetRgba(), Color::BLUE, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDialogStyleNumericFieldsValidationN(void)
{
  UiTestApplication application;
  const float invalid[] = {-3.0f, std::numeric_limits<float>::quiet_NaN(),
                           std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()};
  for(float value : invalid)
  {
    DALI_TEST_ASSERTION(DialogStyle::Builder().SetRequestedWidth(value), "");
    DALI_TEST_ASSERTION(DialogStyle::Builder().SetRequestedHeight(value), "");
    DALI_TEST_ASSERTION(DialogStyle::Builder().SetSpacing(value), "");
    DALI_TEST_ASSERTION(Dialog::New().SetSpacing(value), "");
    DALI_TEST_ASSERTION(DialogStyle::Builder().SetBorderlineWidth(value), "");
    DALI_TEST_ASSERTION(DialogStyle::Builder().SetBackgroundBlurRadius(value), "");
    const Insets paddings[] = {Insets(value, 0.0f, 0.0f, 0.0f), Insets(0.0f, value, 0.0f, 0.0f),
                               Insets(0.0f, 0.0f, value, 0.0f), Insets(0.0f, 0.0f, 0.0f, value)};
    for(const auto& padding : paddings)
    {
      DALI_TEST_ASSERTION(DialogStyle::Builder().SetPadding(padding), "");
    }
    const Vector4 radii[] = {Vector4(value, 0.0f, 0.0f, 0.0f), Vector4(0.0f, value, 0.0f, 0.0f),
                             Vector4(0.0f, 0.0f, value, 0.0f), Vector4(0.0f, 0.0f, 0.0f, value)};
    for(const auto& radius : radii)
    {
      DALI_TEST_ASSERTION(DialogStyle::Builder().SetCornerRadius(radius), "");
    }
  }
  const float nonfinite[] = {std::numeric_limits<float>::quiet_NaN(),
                             std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()};
  for(float value : nonfinite)
  {
    DALI_TEST_ASSERTION(DialogStyle::Builder().SetBorderlineOffset(value), "");
  }
  DALI_TEST_ASSERTION(DialogStyle::Builder().SetCornerRadiusPolicy(static_cast<CornerRadiusPolicy>(99)), "");
  DALI_TEST_ASSERTION(DialogStyle().Configure(), "");
  auto valid = Dialog::New(DialogStyle::Builder().SetRequestedWidth(WRAP_CONTENT)
    .SetRequestedHeight(MATCH_PARENT).SetBorderlineOffset(-3.0f).Build());
  DALI_TEST_EQUALS(valid.GetRequestedWidth(), WRAP_CONTENT, TEST_LOCATION);
  DALI_TEST_EQUALS(valid.GetRequestedHeight(), MATCH_PARENT, TEST_LOCATION);
  DALI_TEST_EQUALS(valid.GetBorderlineOffset(), -3.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDialogStyleBlurRadiusUpperBoundN(void)
{
  UiTestApplication application;
  // Validate before effect construction: huge values must never reach the
  // renderer's float-to-integer conversion or radius-reduction loop.
  DALI_TEST_ASSERTION(DialogStyle::Builder().SetBackgroundBlurRadius(4097.0f), "");
  DALI_TEST_ASSERTION(DialogStyle::Builder().SetBackgroundBlurRadius(1.0e9f), "");
  DALI_TEST_ASSERTION(DialogStyle::Builder().SetBackgroundBlurRadius(std::numeric_limits<float>::max()), "");
  auto zero = Dialog::New(DialogStyle::Builder().SetBackgroundBlurRadius(0.0f).Build());
  DALI_TEST_CHECK(!zero.GetRenderEffect());
  auto maximumStyle = DialogStyle::Builder().SetBackgroundBlurRadius(4096.0f).Build();
  DALI_TEST_EQUALS(maximumStyle.GetBackgroundBlurRadius(), 4096.0f, TEST_LOCATION);
  DALI_TEST_CHECK(Dialog::New(maximumStyle).GetRenderEffect());
  END_TEST;
}

int UtcDaliDialogConstructorP(void)
{
  UiTestApplication application;
  Dialog            dialog;
  DALI_TEST_CHECK(!dialog);
  END_TEST;
}

int UtcDaliDialogNewP(void)
{
  UiTestApplication application;
  Dialog            dialog = Dialog::New();
  DALI_TEST_CHECK(dialog);
  END_TEST;
}

int UtcDaliDialogCopyConstructorP(void)
{
  UiTestApplication application;
  Dialog            dialog = Dialog::New();
  Dialog            copy(dialog);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(dialog == copy);
  END_TEST;
}

int UtcDaliDialogMoveConstructor(void)
{
  UiTestApplication application;
  Dialog            dialog = Dialog::New();
  DALI_TEST_EQUALS(1, dialog.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  Dialog moved = std::move(dialog);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!dialog);
  END_TEST;
}

int UtcDaliDialogAssignmentOperatorP(void)
{
  UiTestApplication application;
  Dialog            dialog = Dialog::New();
  Dialog            copy;
  copy = dialog;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(dialog == copy);
  END_TEST;
}

int UtcDaliDialogMoveAssignment(void)
{
  UiTestApplication application;
  Dialog            dialog = Dialog::New();
  DALI_TEST_EQUALS(1, dialog.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  Dialog moved;
  moved = std::move(dialog);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!dialog);
  END_TEST;
}

int UtcDaliDialogDownCastP(void)
{
  UiTestApplication application;
  Dialog            dialog = Dialog::New();
  BaseHandle        object(dialog);
  Dialog            dialog2 = Dialog::DownCast(object);
  Dialog            dialog3 = DownCast<Dialog>(object);
  DALI_TEST_CHECK(dialog2);
  DALI_TEST_CHECK(dialog3);
  END_TEST;
}

int UtcDaliDialogDownCastN(void)
{
  UiTestApplication application;
  BaseHandle        unInitializedObject;
  Dialog            dialog2 = Dialog::DownCast(unInitializedObject);
  Dialog            dialog3 = DownCast<Dialog>(unInitializedObject);
  DALI_TEST_CHECK(!dialog2);
  DALI_TEST_CHECK(!dialog3);
  END_TEST;
}

int UtcDaliDialogSetGetHeaderViewP(void)
{
  UiTestApplication application;
  Dialog            dialog = Dialog::New();
  DALI_TEST_CHECK(!dialog.GetHeaderView());

  View header = View::New();
  dialog.SetHeaderView(header);
  DALI_TEST_CHECK(dialog.GetHeaderView() == header);
  DALI_TEST_EQUALS(1u, dialog.GetChildCount(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliDialogSetGetBodyViewP(void)
{
  UiTestApplication application;
  Dialog            dialog = Dialog::New();
  DALI_TEST_CHECK(!dialog.GetBodyView());

  View body = View::New();
  dialog.SetBodyView(body);
  DALI_TEST_CHECK(dialog.GetBodyView() == body);
  DALI_TEST_EQUALS(1u, dialog.GetChildCount(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliDialogSetGetFooterViewP(void)
{
  UiTestApplication application;
  Dialog            dialog = Dialog::New();
  DALI_TEST_CHECK(!dialog.GetFooterView());

  View footer = View::New();
  dialog.SetFooterView(footer);
  DALI_TEST_CHECK(dialog.GetFooterView() == footer);
  DALI_TEST_EQUALS(1u, dialog.GetChildCount(), TEST_LOCATION);
  END_TEST;
}

// Sections must always render in header -> body -> footer order, regardless of
// the order in which they were assigned.
int UtcDaliDialogSectionOrderP(void)
{
  UiTestApplication application;
  Dialog            dialog = Dialog::New();

  View header = View::New();
  View body   = View::New();
  View footer = View::New();

  // Assign out of order.
  dialog.SetFooterView(footer);
  dialog.SetHeaderView(header);
  dialog.SetBodyView(body);

  DALI_TEST_EQUALS(3u, dialog.GetChildCount(), TEST_LOCATION);
  DALI_TEST_CHECK(dialog.GetChildViewAt(0) == header);
  DALI_TEST_CHECK(dialog.GetChildViewAt(1) == body);
  DALI_TEST_CHECK(dialog.GetChildViewAt(2) == footer);
  END_TEST;
}

// Assigning an empty handle clears the section.
int UtcDaliDialogClearSectionP(void)
{
  UiTestApplication application;
  Dialog            dialog = Dialog::New();

  View header = View::New();
  View body   = View::New();
  dialog.SetHeaderView(header);
  dialog.SetBodyView(body);
  DALI_TEST_EQUALS(2u, dialog.GetChildCount(), TEST_LOCATION);

  dialog.SetHeaderView(View());
  DALI_TEST_CHECK(!dialog.GetHeaderView());
  DALI_TEST_EQUALS(1u, dialog.GetChildCount(), TEST_LOCATION);
  DALI_TEST_CHECK(dialog.GetChildViewAt(0) == body);
  END_TEST;
}

// Replacing a section swaps the child and preserves ordering.
int UtcDaliDialogReplaceSectionP(void)
{
  UiTestApplication application;
  Dialog            dialog = Dialog::New();

  View header  = View::New();
  View body    = View::New();
  View newBody = View::New();
  dialog.SetHeaderView(header);
  dialog.SetBodyView(body);
  dialog.SetBodyView(newBody);

  DALI_TEST_EQUALS(2u, dialog.GetChildCount(), TEST_LOCATION);
  DALI_TEST_CHECK(dialog.GetBodyView() == newBody);
  DALI_TEST_CHECK(dialog.GetChildViewAt(0) == header);
  DALI_TEST_CHECK(dialog.GetChildViewAt(1) == newBody);
  DALI_TEST_CHECK(!body.GetParent());
  END_TEST;
}

int UtcDaliDialogSetGetSpacingP(void)
{
  UiTestApplication application;
  Dialog            dialog = Dialog::New();
  DALI_TEST_EQUALS(0.0f, dialog.GetSpacing(), TEST_LOCATION);

  dialog.SetSpacing(24.0f);
  DALI_TEST_EQUALS(24.0f, dialog.GetSpacing(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliDialogSetGetLayoutAlignmentP(void)
{
  UiTestApplication application;
  Dialog            dialog = Dialog::New();
  DALI_TEST_CHECK(dialog.GetLayoutAlignment() == LayoutAlignment::FILL);

  dialog.SetLayoutAlignment(LayoutAlignment::CENTER);
  DALI_TEST_CHECK(dialog.GetLayoutAlignment() == LayoutAlignment::CENTER);
  END_TEST;
}

#include <dali-ui-components/public-api/dialog/alert-dialog.h>

int UtcDaliDialogAndAlertDialogAccessibilityRoleP(void)
{
  UiTestApplication application;
  Dialog dialog = Dialog::New();
  AlertDialog alert = AlertDialog::New();
  DALI_TEST_CHECK(dialog.GetAccessibilityRole() == Dali::Ui::Accessibility::Role::DIALOG);
  DALI_TEST_CHECK(alert.GetAccessibilityRole() == Dali::Ui::Accessibility::Role::DIALOG);
  // The role default does not claim ownership of the explicit modal flag.
  DALI_TEST_CHECK(!dialog.IsAccessibilityModal());
  DALI_TEST_CHECK(!alert.IsAccessibilityModal());
  END_TEST;
}
