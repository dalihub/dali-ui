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
#include <dali-toolkit/dali-toolkit.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/stack-layout.h>
#include <dali-ui-foundation/public-api/layout-types.h>

using namespace Dali;
using namespace Dali::UI;

void utc_dali_stacklayout_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_stacklayout_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliStackLayoutConstructorP(void)
{
  TestApplication application;
  StackLayout layout;
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliStackLayoutNewDefaultP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New();
  DALI_TEST_CHECK(layout);
  DALI_TEST_EQUALS(layout.GetOrientation(), StackOrientation::Vertical, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutNewVerticalP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::Vertical);
  DALI_TEST_CHECK(layout);
  DALI_TEST_EQUALS(layout.GetOrientation(), StackOrientation::Vertical, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutNewHorizontalP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::Horizontal);
  DALI_TEST_CHECK(layout);
  DALI_TEST_EQUALS(layout.GetOrientation(), StackOrientation::Horizontal, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutCopyConstructorP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::Horizontal);
  StackLayout copy(layout);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(layout == copy);
  END_TEST;
}

int UtcDaliStackLayoutMoveConstructor(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New();
  StackLayout moved = std::move(layout);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliStackLayoutAssignmentOperatorP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New();
  StackLayout copy;
  copy = layout;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(layout == copy);
  END_TEST;
}

int UtcDaliStackLayoutDownCastP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New();
  StackLayout layout2 = StackLayout::DownCast(layout);
  DALI_TEST_CHECK(layout2);
  DALI_TEST_CHECK(layout == layout2);
  END_TEST;
}

int UtcDaliStackLayoutDownCastN(void)
{
  TestApplication application;
  BaseHandle unInitialized;
  StackLayout layout = StackLayout::DownCast(unInitialized);
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliStackLayoutSetOrientationP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::Vertical);
  layout.SetOrientation(StackOrientation::Horizontal);
  DALI_TEST_EQUALS(layout.GetOrientation(), StackOrientation::Horizontal, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutGetOrientationP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::Horizontal);
  DALI_TEST_EQUALS(layout.GetOrientation(), StackOrientation::Horizontal, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutSetSpacingP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New();
  const float spacing = 10.0f;
  layout.SetSpacing(spacing);
  DALI_TEST_EQUALS(layout.GetSpacing(), spacing, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutGetSpacingP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New();
  DALI_TEST_EQUALS(layout.GetSpacing(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutSetLayoutWeightP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New();
  View child = View::New();
  layout.AddView(child);
  StackLayout::SetLayoutWeight(child, 1.0f);
  DALI_TEST_EQUALS(StackLayout::GetLayoutWeight(child), 1.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutGetLayoutWeightP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New();
  View child = View::New();
  layout.AddView(child);
  DALI_TEST_EQUALS(StackLayout::GetLayoutWeight(child), 0.0f, TEST_LOCATION);
  StackLayout::SetLayoutWeight(child, 0.5f);
  DALI_TEST_EQUALS(StackLayout::GetLayoutWeight(child), 0.5f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutOrientationChainingP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New();
  StackLayout& result = layout.Orientation(StackOrientation::Horizontal);
  DALI_TEST_EQUALS(&result, &layout, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.GetOrientation(), StackOrientation::Horizontal, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutSpacingChainingP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New();
  const float spacing = 5.0f;
  StackLayout& result = layout.Spacing(spacing);
  DALI_TEST_EQUALS(&result, &layout, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.GetSpacing(), spacing, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutContentsP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::Horizontal);
  View a = View::New();
  layout.Contents({a});
  DALI_TEST_EQUALS(layout.GetChildCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(layout.GetChildAt(0) == a);
  END_TEST;
}

int UtcDaliStackLayoutWeightMultipleP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New();
  View c1 = View::New();
  View c2 = View::New();
  layout.AddView(c1);
  layout.AddView(c2);
  StackLayout::SetLayoutWeight(c1, 1.0f);
  StackLayout::SetLayoutWeight(c2, 2.0f);
  DALI_TEST_EQUALS(StackLayout::GetLayoutWeight(c1), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(StackLayout::GetLayoutWeight(c2), 2.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutMeasureArrangeP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::Vertical);
  layout.SetSpacing(5.0f);
  View v1 = View::New();
  v1.SetLayoutWidth(100.0f);
  v1.SetLayoutHeight(40.0f);
  layout.AddView(v1);
  View v2 = View::New();
  v2.SetLayoutWidth(100.0f);
  v2.SetLayoutHeight(40.0f);
  layout.AddView(v2);
  layout.SetLayoutWidth(150.0f);
  layout.SetLayoutHeight(200.0f);
  MeasuredSize m = layout.Measure(150.0f, 200.0f);
  MeasuredSize a = layout.Arrange(LayoutRect(0, 0, 150, 200));
  DALI_TEST_EQUALS(m.GetWidth(), 150.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m.GetHeight(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetWidth(), 150.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetHeight(), 200.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutMeasureHorizontalP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::Horizontal);
  View v1 = View::New();
  v1.SetLayoutWidth(50.0f);
  v1.SetLayoutHeight(50.0f);
  layout.AddView(v1);
  View v2 = View::New();
  v2.SetLayoutWidth(50.0f);
  v2.SetLayoutHeight(50.0f);
  layout.AddView(v2);
  MeasuredSize m = layout.Measure(300.0f, 100.0f);
  DALI_TEST_EQUALS(m.GetWidth(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m.GetHeight(), 50.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutChildNoWeightP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::Vertical);
  View noWeight = View::New();
  layout.AddView(noWeight);
  noWeight.SetLayoutWidth(80.0f);
  noWeight.SetLayoutHeight(40.0f);
  layout.SetLayoutWidth(200.0f);
  layout.SetLayoutHeight(200.0f);
  MeasuredSize m = layout.Measure(200.0f, 200.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 200));
  DALI_TEST_EQUALS(m.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m.GetHeight(), 200.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutChildMatchParentP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::Vertical);
  View child = View::New();
  layout.AddView(child);
  child.SetLayoutWidth(LayoutDimension::MatchParent);
  child.SetLayoutHeight(LayoutDimension::MatchParent);
  layout.SetLayoutWidth(200.0f);
  layout.SetLayoutHeight(120.0f);
  layout.Measure(200.0f, 120.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 120));
  DALI_TEST_EQUALS(child.GetSizeWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSizeHeight(), 120.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutArrangeHorizontalP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::Horizontal);
  layout.SetSpacing(10.0f);
  View v1 = View::New();
  v1.SetLayoutWidth(60.0f);
  v1.SetLayoutHeight(40.0f);
  layout.AddView(v1);
  View v2 = View::New();
  v2.SetLayoutWidth(60.0f);
  v2.SetLayoutHeight(40.0f);
  layout.AddView(v2);
  layout.SetLayoutWidth(200.0f);
  layout.SetLayoutHeight(100.0f);
  MeasuredSize m = layout.Measure(200.0f, 100.0f);
  MeasuredSize a = layout.Arrange(LayoutRect(0, 0, 200, 100));
  DALI_TEST_EQUALS(m.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m.GetHeight(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetHeight(), 100.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutChildWithWeightP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::Vertical);
  layout.SetSpacing(5.0f);
  View fixedChild = View::New();
  fixedChild.SetLayoutWidth(100.0f);
  fixedChild.SetLayoutHeight(40.0f);
  layout.AddView(fixedChild);
  View weightChild = View::New();
  weightChild.SetLayoutWidth(LayoutDimension::MatchParent);
  weightChild.SetLayoutHeight(50.0f);
  layout.AddView(weightChild);
  StackLayout::SetLayoutWeight(weightChild, 1.0f);
  layout.SetLayoutWidth(200.0f);
  layout.SetLayoutHeight(120.0f);
  MeasuredSize m = layout.Measure(200.0f, 120.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 120));
  DALI_TEST_EQUALS(m.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m.GetHeight(), 120.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(weightChild.GetSizeWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_CHECK(weightChild.GetSizeHeight() >= 50.0f);
  END_TEST;
}
