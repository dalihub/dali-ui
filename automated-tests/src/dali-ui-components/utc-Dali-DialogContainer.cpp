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
#include <dali-ui-components/public-api/dialog/dialog-container.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_dialog_container_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_dialog_container_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliDialogContainerConstructorP(void)
{
  UiTestApplication application;
  DialogContainer   container;
  DALI_TEST_CHECK(!container);
  END_TEST;
}

int UtcDaliDialogContainerNewP(void)
{
  UiTestApplication application;
  DialogContainer   container = DialogContainer::New();
  DALI_TEST_CHECK(container);
  END_TEST;
}

int UtcDaliDialogContainerCopyConstructorP(void)
{
  UiTestApplication application;
  DialogContainer   container = DialogContainer::New();
  DialogContainer   copy(container);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(container == copy);
  END_TEST;
}

int UtcDaliDialogContainerMoveConstructor(void)
{
  UiTestApplication application;
  DialogContainer   container = DialogContainer::New();
  DALI_TEST_EQUALS(1, container.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  DialogContainer moved = std::move(container);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!container);
  END_TEST;
}

int UtcDaliDialogContainerAssignmentOperatorP(void)
{
  UiTestApplication application;
  DialogContainer   container = DialogContainer::New();
  DialogContainer   copy;
  copy = container;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(container == copy);
  END_TEST;
}

int UtcDaliDialogContainerMoveAssignment(void)
{
  UiTestApplication application;
  DialogContainer   container = DialogContainer::New();
  DALI_TEST_EQUALS(1, container.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  DialogContainer moved;
  moved = std::move(container);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!container);
  END_TEST;
}

int UtcDaliDialogContainerDownCastP(void)
{
  UiTestApplication application;
  DialogContainer   container = DialogContainer::New();
  BaseHandle        object(container);
  DialogContainer   container2 = DialogContainer::DownCast(object);
  DialogContainer   container3 = DownCast<DialogContainer>(object);
  DALI_TEST_CHECK(container2);
  DALI_TEST_CHECK(container3);
  END_TEST;
}

int UtcDaliDialogContainerDownCastN(void)
{
  UiTestApplication application;
  BaseHandle        unInitializedObject;
  DialogContainer   container2 = DialogContainer::DownCast(unInitializedObject);
  DialogContainer   container3 = DownCast<DialogContainer>(unInitializedObject);
  DALI_TEST_CHECK(!container2);
  DALI_TEST_CHECK(!container3);
  END_TEST;
}

// A DialogContainer is created with a default scrim.
int UtcDaliDialogContainerDefaultScrimP(void)
{
  UiTestApplication application;
  DialogContainer   container = DialogContainer::New();
  DALI_TEST_CHECK(container.GetScrim());
  END_TEST;
}

int UtcDaliDialogContainerSetGetModalContentP(void)
{
  UiTestApplication application;
  DialogContainer   container = DialogContainer::New();
  DALI_TEST_CHECK(!container.GetModalContent());

  View content = View::New();
  container.SetModalContent(content);
  DALI_TEST_CHECK(container.GetModalContent() == content);
  END_TEST;
}

int UtcDaliDialogContainerSetGetScrimP(void)
{
  UiTestApplication application;
  DialogContainer   container = DialogContainer::New();

  View scrim = View::New();
  container.SetScrim(scrim);
  DALI_TEST_CHECK(container.GetScrim() == scrim);
  END_TEST;
}

// The ScrimClicked signal is accessible.
int UtcDaliDialogContainerScrimClickedSignalP(void)
{
  UiTestApplication application;
  DialogContainer   container = DialogContainer::New();
  DALI_TEST_CHECK(container.ScrimClickedSignal().GetConnectionCount() == 0);
  END_TEST;
}
