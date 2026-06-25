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
  DALI_TEST_CHECK(dialog.GetChildAt(0) == header);
  DALI_TEST_CHECK(dialog.GetChildAt(1) == body);
  DALI_TEST_CHECK(dialog.GetChildAt(2) == footer);
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
  DALI_TEST_CHECK(dialog.GetChildAt(0) == body);
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
  DALI_TEST_CHECK(dialog.GetChildAt(0) == header);
  DALI_TEST_CHECK(dialog.GetChildAt(1) == newBody);
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
