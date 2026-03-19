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
#include <dali-ui-foundation/public-api/stack-layout.h>
#include <dali-ui-foundation/public-api/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layout-types.h>

using namespace Dali;
using namespace Dali::Ui;

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
  DALI_TEST_EQUALS(layout.GetOrientation(), StackOrientation::VERTICAL, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutNewVerticalP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  DALI_TEST_CHECK(layout);
  DALI_TEST_EQUALS(layout.GetOrientation(), StackOrientation::VERTICAL, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutNewHorizontalP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  DALI_TEST_CHECK(layout);
  DALI_TEST_EQUALS(layout.GetOrientation(), StackOrientation::HORIZONTAL, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutCopyConstructorP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
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
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetOrientation(StackOrientation::HORIZONTAL);
  DALI_TEST_EQUALS(layout.GetOrientation(), StackOrientation::HORIZONTAL, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutGetOrientationP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  DALI_TEST_EQUALS(layout.GetOrientation(), StackOrientation::HORIZONTAL, TEST_LOCATION);
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
  child.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
  DALI_TEST_EQUALS(child.GetLayoutParams<StackLayoutParams>().GetWeight(), 1.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutGetLayoutWeightP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New();
  View child = View::New();
  layout.AddView(child);
  child.SetLayoutParams(StackLayoutParams::New().SetWeight(0.0f));
  DALI_TEST_EQUALS(child.GetLayoutParams<StackLayoutParams>().GetWeight(), 0.0f, TEST_LOCATION);
  child.SetLayoutParams(StackLayoutParams::New().SetWeight(0.5f));
  DALI_TEST_EQUALS(child.GetLayoutParams<StackLayoutParams>().GetWeight(), 0.5f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutOrientationChainingP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New();
  StackLayout& result = layout.Orientation(StackOrientation::HORIZONTAL);
  DALI_TEST_EQUALS(&result, &layout, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.GetOrientation(), StackOrientation::HORIZONTAL, TEST_LOCATION);
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
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
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
  c1.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
  c2.SetLayoutParams(StackLayoutParams::New().SetWeight(2.0f));
  DALI_TEST_EQUALS(c1.GetLayoutParams<StackLayoutParams>().GetWeight(), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(c2.GetLayoutParams<StackLayoutParams>().GetWeight(), 2.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutMeasureArrangeP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetSpacing(5.0f);
  View v1 = View::New();
  v1.SetRequestedWidth(100.0f);
  v1.SetRequestedHeight(40.0f);
  layout.AddView(v1);
  View v2 = View::New();
  v2.SetRequestedWidth(100.0f);
  v2.SetRequestedHeight(40.0f);
  layout.AddView(v2);
  layout.SetRequestedWidth(150.0f);
  layout.SetRequestedHeight(200.0f);
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
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  View v1 = View::New();
  v1.SetRequestedWidth(50.0f);
  v1.SetRequestedHeight(50.0f);
  layout.AddView(v1);
  View v2 = View::New();
  v2.SetRequestedWidth(50.0f);
  v2.SetRequestedHeight(50.0f);
  layout.AddView(v2);
  MeasuredSize m = layout.Measure(300.0f, 100.0f);
  DALI_TEST_EQUALS(m.GetWidth(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m.GetHeight(), 50.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutChildNoWeightP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  View noWeight = View::New();
  layout.AddView(noWeight);
  noWeight.SetRequestedWidth(80.0f);
  noWeight.SetRequestedHeight(40.0f);
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(200.0f);
  MeasuredSize m = layout.Measure(200.0f, 200.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 200));
  DALI_TEST_EQUALS(m.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m.GetHeight(), 200.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutChildMatchParentP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  View child = View::New();
  child.SetRequestedWidth(MATCH_PARENT);
  child.SetRequestedHeight(MATCH_PARENT);
  layout.AddView(child);
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(120.0f);
  layout.Measure(200.0f, 120.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 120));
  DALI_TEST_EQUALS(child.GetSize().width, 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().height, 120.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutArrangeHorizontalP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  layout.SetSpacing(10.0f);
  View v1 = View::New();
  v1.SetRequestedWidth(60.0f);
  v1.SetRequestedHeight(40.0f);
  layout.AddView(v1);
  View v2 = View::New();
  v2.SetRequestedWidth(60.0f);
  v2.SetRequestedHeight(40.0f);
  layout.AddView(v2);
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
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
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetSpacing(5.0f);
  View fixedChild = View::New();
  fixedChild.SetRequestedWidth(100.0f);
  fixedChild.SetRequestedHeight(40.0f);
  layout.AddView(fixedChild);
  View weightChild = View::New();
  weightChild.SetRequestedWidth(MATCH_PARENT);
  weightChild.SetRequestedHeight(50.0f);
  layout.AddView(weightChild);
  weightChild.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(120.0f);
  MeasuredSize m = layout.Measure(200.0f, 120.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 120));
  DALI_TEST_EQUALS(m.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m.GetHeight(), 120.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(weightChild.GetSize().width, 200.0f, TEST_LOCATION);
  DALI_TEST_CHECK(weightChild.GetSize().height >= 50.0f);
  END_TEST;
}

// Cross-axis alignment (HorizontalAlignment in vertical stack)
int UtcDaliStackLayoutCrossAxisHorizontalStartP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedWidth(80.0f);
  child.SetRequestedHeight(40.0f);
  layout.AddView(child);
  child.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::START));
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 100));
  DALI_TEST_EQUALS(child.GetPositionX(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().width, 80.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutCrossAxisHorizontalCenterP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedWidth(80.0f);
  child.SetRequestedHeight(40.0f);
  layout.AddView(child);
  child.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER));
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 100));
  const float expectedX = (200.0f - 80.0f) * 0.5f;
  DALI_TEST_EQUALS(child.GetPositionX(), expectedX, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().width, 80.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutCrossAxisHorizontalEndP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedWidth(80.0f);
  child.SetRequestedHeight(40.0f);
  layout.AddView(child);
  child.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::END));
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 100));
  DALI_TEST_EQUALS(child.GetPositionX(), 120.0f, TEST_LOCATION); // 200 - 80
  DALI_TEST_EQUALS(child.GetSize().width, 80.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutCrossAxisHorizontalMatchParentP(void)
{
  // MATCH_PARENT on cross axis: child expands to fill the available cross-axis space.
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedWidth(MATCH_PARENT);
  child.SetRequestedHeight(40.0f);
  layout.AddView(child);
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 100));
  DALI_TEST_EQUALS(child.GetPositionX(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().width, 200.0f, TEST_LOCATION); // Fill expands to parent width
  END_TEST;
}

int UtcDaliStackLayoutCrossAxisHorizontalFillP(void)
{
  // FILL alignment on cross axis: child stretches to fill the available cross-axis space.
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedHeight(40.0f);
  child.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  layout.AddView(child);
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 100));
  DALI_TEST_EQUALS(child.GetPositionX(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().width, 200.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutCrossAxisHorizontalFillWithExplicitSizeP(void)
{
  // FILL alignment with explicit cross-axis size: explicit size takes priority.
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedWidth(80.0f);
  child.SetRequestedHeight(40.0f);
  child.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  layout.AddView(child);
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 100));
  DALI_TEST_EQUALS(child.GetSize().width, 80.0f, TEST_LOCATION); // explicit size wins
  END_TEST;
}

// Cross-axis alignment (VerticalAlignment in horizontal stack)
int UtcDaliStackLayoutCrossAxisVerticalStartP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedWidth(60.0f);
  child.SetRequestedHeight(40.0f);
  layout.AddView(child);
  child.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::START));
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 100));
  DALI_TEST_EQUALS(child.GetPositionY(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().height, 40.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutCrossAxisVerticalCenterP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedWidth(60.0f);
  child.SetRequestedHeight(40.0f);
  layout.AddView(child);
  child.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER));
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 100));
  const float expectedY = (100.0f - 40.0f) * 0.5f;
  DALI_TEST_EQUALS(child.GetPositionY(), expectedY, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().height, 40.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutCrossAxisVerticalEndP(void)
{
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedWidth(60.0f);
  child.SetRequestedHeight(40.0f);
  layout.AddView(child);
  child.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::END));
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 100));
  DALI_TEST_EQUALS(child.GetPositionY(), 60.0f, TEST_LOCATION); // 100 - 40
  DALI_TEST_EQUALS(child.GetSize().height, 40.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutCrossAxisVerticalMatchParentP(void)
{
  // MATCH_PARENT on cross axis: child expands to fill the available cross-axis space.
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedWidth(60.0f);
  child.SetRequestedHeight(MATCH_PARENT);
  layout.AddView(child);
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 100));
  DALI_TEST_EQUALS(child.GetPositionY(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().height, 100.0f, TEST_LOCATION); // Fill expands to parent height
  END_TEST;
}

int UtcDaliStackLayoutCrossAxisVerticalFillP(void)
{
  // FILL alignment on cross axis: child stretches to fill the available cross-axis space.
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedWidth(60.0f);
  child.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  layout.AddView(child);
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 100));
  DALI_TEST_EQUALS(child.GetPositionY(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().height, 100.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutCrossAxisVerticalFillWithExplicitSizeP(void)
{
  // FILL alignment with explicit cross-axis size: explicit size takes priority.
  TestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedWidth(60.0f);
  child.SetRequestedHeight(40.0f);
  child.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  layout.AddView(child);
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 100));
  DALI_TEST_EQUALS(child.GetSize().height, 40.0f, TEST_LOCATION); // explicit size wins
  END_TEST;
}
