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

#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/dali-ui-foundation.h>

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
  UiTestApplication application;
  Layout layout;
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliLayoutNewP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  DALI_TEST_CHECK(layout);
  END_TEST;
}

int UtcDaliLayoutCopyConstructorP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  Layout copy(layout);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(layout == copy);
  END_TEST;
}

int UtcDaliLayoutMoveConstructor(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  Layout moved = std::move(layout);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliLayoutAssignmentOperatorP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  Layout copy;
  copy = layout;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(layout == copy);
  END_TEST;
}

int UtcDaliLayoutDownCastP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  Layout layout2 = Layout::DownCast(layout);
  DALI_TEST_CHECK(layout2);
  DALI_TEST_CHECK(layout == layout2);
  END_TEST;
}

int UtcDaliLayoutDownCastN(void)
{
  UiTestApplication application;
  BaseHandle unInitialized;
  Layout layout = Layout::DownCast(unInitialized);
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliLayoutAddP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  View child = View::New();
  layout.Add(child);
  DALI_TEST_EQUALS(layout.GetChildCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(layout.GetChildAt(0) == child);
  END_TEST;
}

int UtcDaliLayoutInsertAtIndexP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  View child0 = View::New();
  View child1 = View::New();
  layout.Add(child0);
  layout.Insert(0, child1);
  DALI_TEST_EQUALS(layout.GetChildCount(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(layout.GetChildAt(0) == child1);
  DALI_TEST_CHECK(layout.GetChildAt(1) == child0);
  END_TEST;
}

int UtcDaliLayoutRemoveP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  View child = View::New();
  layout.Add(child);
  DALI_TEST_EQUALS(layout.GetChildCount(), 1u, TEST_LOCATION);
  layout.Remove(child);
  DALI_TEST_EQUALS(layout.GetChildCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutRemoveAtP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  View child = View::New();
  layout.Add(child);
  layout.Remove(layout.GetChildAt(0));
  DALI_TEST_EQUALS(layout.GetChildCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutRemoveAllChildrenP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  layout.Add(View::New());
  layout.Add(View::New());
  DALI_TEST_EQUALS(layout.GetChildCount(), 2u, TEST_LOCATION);
  layout.RemoveAllChildren();
  DALI_TEST_EQUALS(layout.GetChildCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutGetChildCountP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  DALI_TEST_EQUALS(layout.GetChildCount(), 0u, TEST_LOCATION);
  layout.Add(View::New());
  DALI_TEST_EQUALS(layout.GetChildCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutGetChildAtP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  View child = View::New();
  layout.Add(child);
  DALI_TEST_CHECK(layout.GetChildAt(0) == child);
  END_TEST;
}

int UtcDaliLayoutIndexOfChildP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  View child = View::New();
  layout.Add(child);
  DALI_TEST_EQUALS(layout.IndexOfChild(child), 0, TEST_LOCATION);
  View notChild = View::New();
  DALI_TEST_EQUALS(layout.IndexOfChild(notChild), -1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutContentsP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  View a = View::New();
  View b = View::New();
  layout.AddChildren({a, b});
  DALI_TEST_EQUALS(layout.GetChildCount(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(layout.GetChildAt(0) == a);
  DALI_TEST_CHECK(layout.GetChildAt(1) == b);
  END_TEST;
}

int UtcDaliLayoutContentsEmptyP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  layout.AddChildren({});
  DALI_TEST_EQUALS(layout.GetChildCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutRemoveNonChildP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  View child = View::New();
  layout.Add(child);
  View notChild = View::New();
  layout.Remove(notChild);
  DALI_TEST_EQUALS(layout.GetChildCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutGetChildAtEmptyP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  View v = layout.GetChildAt(0);
  DALI_TEST_CHECK(!v);
  END_TEST;
}

int UtcDaliLayoutInsertIndexClampP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  View child = View::New();
  layout.Insert(99u, child);
  DALI_TEST_EQUALS(layout.GetChildCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(layout.GetChildAt(0) == child);
  END_TEST;
}

int UtcDaliLayoutRemoveAtInvalidIndexP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  layout.Add(View::New());
  View invalid = layout.GetChildAt(5u);
  DALI_TEST_CHECK(!invalid); // Out-of-bounds returns empty handle
  DALI_TEST_EQUALS(layout.GetChildCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutIndexOfChildNotInLayoutP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  View outside = View::New();
  DALI_TEST_EQUALS(layout.IndexOfChild(outside), -1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutGetChildAtOutOfRangeP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  layout.Add(View::New());
  View v = layout.GetChildAt(1);
  DALI_TEST_CHECK(!v);
  END_TEST;
}

int UtcDaliLayoutIndexOfChildEmptyHandleP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  layout.Add(View::New());
  DALI_TEST_EQUALS(layout.IndexOfChild(View()), -1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutContentsSetterP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  View a = View::New();
  layout.AddChildren({a});
  DALI_TEST_EQUALS(layout.GetChildCount(), 1u, TEST_LOCATION);
  END_TEST;
}

namespace
{
MeasuredSize HorizontalLineMeasure(View self, float widthConstraint, float heightConstraint)
{
  float maxHeight = 0.0f;
  for(uint32_t i = 0; i < self.GetChildCount(); ++i)
  {
    View         child = self.GetChildAt(i);
    MeasuredSize sz    = child.Measure(widthConstraint, heightConstraint);
    maxHeight          = std::max(maxHeight, sz.height);
  }
  return {widthConstraint, maxHeight};
}

MeasuredSize HorizontalLineArrange(View self, const LayoutRect& bounds)
{
  float x = bounds.x;
  for(uint32_t i = 0; i < self.GetChildCount(); ++i)
  {
    View         child = self.GetChildAt(i);
    MeasuredSize sz    = child.GetMeasuredSize();
    child.Arrange({x, bounds.y, sz.width, sz.height});
    x += sz.width;
  }
  return {bounds.width, bounds.height};
}
} // namespace

int UtcDaliLayoutCallbackDirectionLtrP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  layout.SetMeasureCallback(MeasureCallback::New(&HorizontalLineMeasure));
  layout.SetArrangeCallback(ArrangeCallback::New(&HorizontalLineArrange));
  application.GetScene().Add(layout);

  View a = View::New();
  a.SetRequestedWidth(40.0f);
  a.SetRequestedHeight(30.0f);
  layout.Add(a);
  View b = View::New();
  b.SetRequestedWidth(50.0f);
  b.SetRequestedHeight(30.0f);
  layout.Add(b);

  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 100.0f));

  DALI_TEST_EQUALS(a.GetPositionX(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetSize().width, 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetPositionX(), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetSize().width, 50.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayoutCallbackDirectionRtlP(void)
{
  UiTestApplication application;
  Layout layout = Layout::New();
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  layout.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  layout.SetMeasureCallback(MeasureCallback::New(&HorizontalLineMeasure));
  layout.SetArrangeCallback(ArrangeCallback::New(&HorizontalLineArrange));
  application.GetScene().Add(layout);

  View a = View::New();
  a.SetRequestedWidth(40.0f);
  a.SetRequestedHeight(30.0f);
  layout.Add(a);
  View b = View::New();
  b.SetRequestedWidth(50.0f);
  b.SetRequestedHeight(30.0f);
  layout.Add(b);

  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 100.0f));

  // Callback arranges in LTR (0, 40); framework mirrors after callback.
  DALI_TEST_EQUALS(a.GetPositionX(), 200.0f - 0.0f - 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetSize().width, 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetPositionX(), 200.0f - 40.0f - 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetSize().width, 50.0f, TEST_LOCATION);
  END_TEST;
}
