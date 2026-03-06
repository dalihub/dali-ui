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

#include <dali-test-suite-utils.h>
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/layout.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_layout_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_layout_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliLayoutConstructorP(void)
{
  TestApplication application;
  Layout layout;
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliLayoutNewP(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  DALI_TEST_CHECK(layout);
  END_TEST;
}

int UtcDaliLayoutCopyConstructorP(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  Layout copy(layout);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(layout == copy);
  END_TEST;
}

int UtcDaliLayoutMoveConstructor(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  Layout moved = std::move(layout);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliLayoutAssignmentOperatorP(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  Layout copy;
  copy = layout;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(layout == copy);
  END_TEST;
}

int UtcDaliLayoutDownCastP(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  Layout layout2 = Layout::DownCast(layout);
  DALI_TEST_CHECK(layout2);
  DALI_TEST_CHECK(layout == layout2);
  END_TEST;
}

int UtcDaliLayoutDownCastN(void)
{
  TestApplication application;
  BaseHandle unInitialized;
  Layout layout = Layout::DownCast(unInitialized);
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliLayoutAddViewP(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  View child = View::New();
  layout.AddView(child);
  DALI_TEST_EQUALS(layout.GetChildCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(layout.GetChildAt(0) == child);
  END_TEST;
}

int UtcDaliLayoutAddViewAtIndexP(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  View child0 = View::New();
  View child1 = View::New();
  layout.AddView(child0);
  layout.AddView(child1, 0);
  DALI_TEST_EQUALS(layout.GetChildCount(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(layout.GetChildAt(0) == child1);
  DALI_TEST_CHECK(layout.GetChildAt(1) == child0);
  END_TEST;
}

int UtcDaliLayoutRemoveViewP(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  View child = View::New();
  layout.AddView(child);
  DALI_TEST_EQUALS(layout.GetChildCount(), 1u, TEST_LOCATION);
  layout.RemoveView(child);
  DALI_TEST_EQUALS(layout.GetChildCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutRemoveViewAtP(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  View child = View::New();
  layout.AddView(child);
  layout.RemoveViewAt(0);
  DALI_TEST_EQUALS(layout.GetChildCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutRemoveAllViewsP(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  layout.AddView(View::New());
  layout.AddView(View::New());
  DALI_TEST_EQUALS(layout.GetChildCount(), 2u, TEST_LOCATION);
  layout.RemoveAllViews();
  DALI_TEST_EQUALS(layout.GetChildCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutGetChildCountP(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  DALI_TEST_EQUALS(layout.GetChildCount(), 0u, TEST_LOCATION);
  layout.AddView(View::New());
  DALI_TEST_EQUALS(layout.GetChildCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutGetChildAtP(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  View child = View::New();
  layout.AddView(child);
  DALI_TEST_CHECK(layout.GetChildAt(0) == child);
  END_TEST;
}

int UtcDaliLayoutIndexOfChildP(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  View child = View::New();
  layout.AddView(child);
  DALI_TEST_EQUALS(layout.IndexOfChild(child), 0, TEST_LOCATION);
  View notChild = View::New();
  DALI_TEST_EQUALS(layout.IndexOfChild(notChild), -1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutContentsP(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  View a = View::New();
  View b = View::New();
  Layout& result = layout.Contents({a, b});
  DALI_TEST_CHECK(result == layout);
  DALI_TEST_EQUALS(layout.GetChildCount(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(layout.GetChildAt(0) == a);
  DALI_TEST_CHECK(layout.GetChildAt(1) == b);
  END_TEST;
}

int UtcDaliLayoutContentsEmptyP(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  Layout& result = layout.Contents({});
  DALI_TEST_EQUALS(&result, &layout, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.GetChildCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutRemoveViewNonChildP(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  View child = View::New();
  layout.AddView(child);
  View notChild = View::New();
  layout.RemoveView(notChild);
  DALI_TEST_EQUALS(layout.GetChildCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutGetChildAtEmptyP(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  View v = layout.GetChildAt(0);
  DALI_TEST_CHECK(!v);
  END_TEST;
}

int UtcDaliLayoutAddViewIndexClampP(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  View child = View::New();
  layout.AddView(child, 99u);
  DALI_TEST_EQUALS(layout.GetChildCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(layout.GetChildAt(0) == child);
  END_TEST;
}

int UtcDaliLayoutRemoveViewAtInvalidIndexP(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  layout.AddView(View::New());
  layout.RemoveViewAt(5u);
  DALI_TEST_EQUALS(layout.GetChildCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutIndexOfChildNotInLayoutP(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  View outside = View::New();
  DALI_TEST_EQUALS(layout.IndexOfChild(outside), -1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutGetChildAtOutOfRangeP(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  layout.AddView(View::New());
  View v = layout.GetChildAt(1);
  DALI_TEST_CHECK(!v);
  END_TEST;
}

int UtcDaliLayoutAddViewEmptyHandleP(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  View empty;
  layout.AddView(empty);
  DALI_TEST_EQUALS(layout.GetChildCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutIndexOfChildEmptyHandleP(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  layout.AddView(View::New());
  DALI_TEST_EQUALS(layout.IndexOfChild(View()), -1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutContentsChainingP(void)
{
  TestApplication application;
  Layout layout = Layout::New();
  View a = View::New();
  Layout& ref = layout.Contents({a});
  DALI_TEST_EQUALS(&ref, &layout, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.GetChildCount(), 1u, TEST_LOCATION);
  END_TEST;
}
