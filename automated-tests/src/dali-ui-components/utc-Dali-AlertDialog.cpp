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
