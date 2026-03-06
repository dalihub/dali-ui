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
#include <dali-ui-components/public-api/dummy-component.h>
#include <dali-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_dummy_components_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_dummy_components_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliDummyComponentConstructorP(void)
{
  TestApplication application;
  DummyComponent dummyComponent;
  DALI_TEST_CHECK(!dummyComponent);
  END_TEST;
}

int UtcDaliDummyComponentNewP(void)
{
  TestApplication application;
  DummyComponent dummyComponent = DummyComponent::New();
  DALI_TEST_CHECK(dummyComponent);
  END_TEST;
}

int UtcDaliDummyComponentCopyConstructorP(void)
{
  TestApplication application;
  DummyComponent dummyComponent = DummyComponent::New();
  DummyComponent copy(dummyComponent);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(dummyComponent == copy);
  END_TEST;
}

int UtcDaliDummyComponentMoveConstructor(void)
{
  TestApplication application;
  DummyComponent dummyComponent = DummyComponent::New();
  DALI_TEST_EQUALS(1, dummyComponent.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  DummyComponent moved = std::move(dummyComponent);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!dummyComponent);
  END_TEST;
}

int UtcDaliDummyComponentAssignmentOperatorP(void)
{
  TestApplication application;
  DummyComponent dummyComponent = DummyComponent::New();
  DummyComponent copy;
  copy = dummyComponent;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(dummyComponent == copy);
  END_TEST;
}

int UtcDaliDummyComponentMoveAssignment(void)
{
  TestApplication application;
  DummyComponent dummyComponent = DummyComponent::New();
  DALI_TEST_EQUALS(1, dummyComponent.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  DummyComponent moved;
  moved = std::move(dummyComponent);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!dummyComponent);
  END_TEST;
}

int UtcDaliDummyComponentDownCastP(void)
{
  TestApplication application;
  DummyComponent dummyComponent = DummyComponent::New();
  BaseHandle object(dummyComponent);
  DummyComponent dummyComponent2 = DummyComponent::DownCast(object);
  DummyComponent dummyComponent3 = DownCast<DummyComponent>(object);
  DALI_TEST_CHECK(dummyComponent2);
  DALI_TEST_CHECK(dummyComponent3);
  END_TEST;
}

int UtcDaliDummyComponentDownCastN(void)
{
  TestApplication application;
  BaseHandle unInitializedObject;
  DummyComponent dummyComponent2 = DummyComponent::DownCast(unInitializedObject);
  DummyComponent dummyComponent3 = DownCast<DummyComponent>(unInitializedObject);
  DALI_TEST_CHECK(!dummyComponent2);
  DALI_TEST_CHECK(!dummyComponent3);
  END_TEST;
}
