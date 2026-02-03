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
#include <dali-toolkit/dali-toolkit.h>
#include <dali-ui-elements/public-api/dummy-element.h>
#include <dali-test-suite-utils.h>

using namespace Dali;
using namespace Dali::UI;

void utc_dali_dummy_elements_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_dummy_elements_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliDummyElementConstructorP(void)
{
  TestApplication application;
  DummyElement dummyElement;
  DALI_TEST_CHECK(!dummyElement);
  END_TEST;
}

int UtcDaliDummyElementNewP(void)
{
  TestApplication application;
  DummyElement dummyElement = DummyElement::New();
  DALI_TEST_CHECK(dummyElement);
  END_TEST;
}

int UtcDaliDummyElementCopyConstructorP(void)
{
  TestApplication application;
  DummyElement dummyElement = DummyElement::New();
  DummyElement copy(dummyElement);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(dummyElement == copy);
  END_TEST;
}

int UtcDaliDummyElementMoveConstructor(void)
{
  TestApplication application;
  DummyElement dummyElement = DummyElement::New();
  DALI_TEST_EQUALS(1, dummyElement.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  DummyElement moved = std::move(dummyElement);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!dummyElement);
  END_TEST;
}

int UtcDaliDummyElementAssignmentOperatorP(void)
{
  TestApplication application;
  DummyElement dummyElement = DummyElement::New();
  DummyElement copy;
  copy = dummyElement;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(dummyElement == copy);
  END_TEST;
}

int UtcDaliDummyElementMoveAssignment(void)
{
  TestApplication application;
  DummyElement dummyElement = DummyElement::New();
  DALI_TEST_EQUALS(1, dummyElement.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  DummyElement moved;
  moved = std::move(dummyElement);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!dummyElement);
  END_TEST;
}

int UtcDaliDummyElementDownCastP(void)
{
  TestApplication application;
  DummyElement dummyElement = DummyElement::New();
  BaseHandle object(dummyElement);
  DummyElement dummyElement2 = DummyElement::DownCast(object);
  DummyElement dummyElement3 = DownCast<DummyElement>(object);
  DALI_TEST_CHECK(dummyElement2);
  DALI_TEST_CHECK(dummyElement3);
  END_TEST;
}

int UtcDaliDummyElementDownCastN(void)
{
  TestApplication application;
  BaseHandle unInitializedObject;
  DummyElement dummyElement2 = DummyElement::DownCast(unInitializedObject);
  DummyElement dummyElement3 = DownCast<DummyElement>(unInitializedObject);
  DALI_TEST_CHECK(!dummyElement2);
  DALI_TEST_CHECK(!dummyElement3);
  END_TEST;
}
