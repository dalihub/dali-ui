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
#include "../../../dali-ui-foundation/public-api/view.h"
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation-test-suite-utils.h>

using namespace Dali;
using namespace Dali::UI;

void utc_dali_actor_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_actor_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliViewConstructorP(void)
{
  UIFoundationTestApplication application;
  View view;
  DALI_TEST_CHECK(!view);
  END_TEST;
}

int UtcDaliViewNewP(void)
{
  UIFoundationTestApplication application;
  View view = View::New();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliViewCopyConstructorP(void)
{
  UIFoundationTestApplication application;
  View view = View::New();
  View copy(view);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliViewMoveConstructor(void)
{
  UIFoundationTestApplication application;
  View view = View::New();
  DALI_TEST_EQUALS(1, view.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  View moved = std::move(view);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!view);
  END_TEST;
}

int UtcDaliViewAssignmentOperatorP(void)
{
  UIFoundationTestApplication application;
  View view = View::New();
  View copy;
  copy = view;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliViewMoveAssignment(void)
{
  UIFoundationTestApplication application;
  View view = View::New();
  DALI_TEST_EQUALS(1, view.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  View moved;
  moved = std::move(view);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!view);
  END_TEST;
}

int UtcDaliViewDownCastP(void)
{
  UIFoundationTestApplication application;
  View view = View::New();
  BaseHandle object(view);
  View view2 = View::DownCast(object);
  View view3 = DownCast<View>(object);
  DALI_TEST_CHECK(view2);
  DALI_TEST_CHECK(view3);
  END_TEST;
}

int UtcDaliViewDownCastN(void)
{
  UIFoundationTestApplication application;
  BaseHandle unInitializedObject;
  View view1 = View::DownCast(unInitializedObject);
  View view2 = DownCast<View>(unInitializedObject);
  DALI_TEST_CHECK(!view1);
  DALI_TEST_CHECK(!view2);
  END_TEST;
}

int UtcDaliViewGetSizeWidthP(void)
{
  UIFoundationTestApplication application;
  View view = View::New();
  const float testWidth = 100.0f;

  view.SizeWidth(testWidth);
  DALI_TEST_EQUALS(view.GetSizeWidth(), testWidth, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetSizeHeightP(void)
{
  UIFoundationTestApplication application;
  View view = View::New();
  const float testHeight = 200.0f;

  view.SizeHeight(testHeight);
  DALI_TEST_EQUALS(view.GetSizeHeight(), testHeight, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetPositionXP(void)
{
  UIFoundationTestApplication application;
  View view = View::New();
  const float testX = 50.0f;

  view.PositionX(testX);
  DALI_TEST_EQUALS(view.GetPositionX(), testX, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewGetPositionYP(void)
{
  UIFoundationTestApplication application;
  View view = View::New();
  const float testY = 75.0f;

  view.PositionY(testY);
  DALI_TEST_EQUALS(view.GetPositionY(), testY, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSizeWidthChainingP(void)
{
  UIFoundationTestApplication application;
  View view = View::New();
  const float testWidth = 150.0f;

  View& result = view.SizeWidth(testWidth);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetSizeWidth(), testWidth, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewSizeHeightChainingP(void)
{
  UIFoundationTestApplication application;
  View view = View::New();
  const float testHeight = 250.0f;

  View& result = view.SizeHeight(testHeight);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetSizeHeight(), testHeight, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewPositionXChainingP(void)
{
  UIFoundationTestApplication application;
  View view = View::New();
  const float testX = 125.0f;

  View& result = view.PositionX(testX);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPositionX(), testX, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewPositionYChainingP(void)
{
  UIFoundationTestApplication application;
  View view = View::New();
  const float testY = 175.0f;

  View& result = view.PositionY(testY);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPositionY(), testY, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewBackgroundColorChainingP(void)
{
  UIFoundationTestApplication application;
  View view = View::New();
  const Vector4 testColor(1.0f, 0.0f, 0.0f, 0.5f);

  View& result = view.BackgroundColor(testColor);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewMultipleChainingP(void)
{
  UIFoundationTestApplication application;
  View view = View::New();
  const float testWidth = 300.0f;
  const float testHeight = 200.0f;
  const float testX = 100.0f;
  const float testY = 50.0f;
  const Vector4 testColor(0.0f, 1.0f, 0.0f, 0.8f);

  View& result = view.SizeWidth(testWidth)
                    .SizeHeight(testHeight)
                    .PositionX(testX)
                    .PositionY(testY)
                    .BackgroundColor(testColor);

  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetSizeWidth(), testWidth, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetSizeHeight(), testHeight, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPositionX(), testX, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPositionY(), testY, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewContentsP(void)
{
  UIFoundationTestApplication application;
  View parent = View::New();
  View child1 = View::New();
  View child2 = View::New();

  View& result = parent.Contents({child1, child2});
  DALI_TEST_EQUALS(&result, &parent, TEST_LOCATION);
  DALI_TEST_EQUALS(parent.GetChildCount(), 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliViewAsP(void)
{
  UIFoundationTestApplication application;
  View view = View::New();
  View reference;

  View& result = view.As(reference);
  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_CHECK(reference == view);
  END_TEST;
}

int UtcDaliViewWithP(void)
{
  UIFoundationTestApplication application;
  View view = View::New();
  const float testWidth = 400.0f;
  bool actionCalled = false;

  View& result = view.With([&actionCalled, testWidth](View& v) {
    v.SizeWidth(testWidth);
    actionCalled = true;
  });

  DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);
  DALI_TEST_CHECK(actionCalled);
  DALI_TEST_EQUALS(view.GetSizeWidth(), testWidth, TEST_LOCATION);
  END_TEST;
}