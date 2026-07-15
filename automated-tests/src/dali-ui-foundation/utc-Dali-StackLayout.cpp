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

template<typename T>
T GetRequiredLayoutParams(View view)
{
  T params;
  DALI_TEST_CHECK(view.TryGetLayoutParams(params));
  return params;
}

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
  UiTestApplication application;
  StackLayout layout;
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliStackLayoutNewDefaultP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New();
  DALI_TEST_CHECK(layout);
  DALI_TEST_EQUALS(layout.GetOrientation(), StackOrientation::VERTICAL, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutNewVerticalP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  DALI_TEST_CHECK(layout);
  DALI_TEST_EQUALS(layout.GetOrientation(), StackOrientation::VERTICAL, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutNewHorizontalP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  DALI_TEST_CHECK(layout);
  DALI_TEST_EQUALS(layout.GetOrientation(), StackOrientation::HORIZONTAL, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutCopyConstructorP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  StackLayout copy(layout);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(layout == copy);
  END_TEST;
}

int UtcDaliStackLayoutMoveConstructor(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New();
  StackLayout moved = std::move(layout);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliStackLayoutAssignmentOperatorP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New();
  StackLayout copy;
  copy = layout;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(layout == copy);
  END_TEST;
}

int UtcDaliStackLayoutDownCastP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New();
  StackLayout layout2 = StackLayout::DownCast(layout);
  DALI_TEST_CHECK(layout2);
  DALI_TEST_CHECK(layout == layout2);
  END_TEST;
}

int UtcDaliStackLayoutDownCastN(void)
{
  UiTestApplication application;
  BaseHandle unInitialized;
  StackLayout layout = StackLayout::DownCast(unInitialized);
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliStackLayoutSetOrientationP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetOrientation(StackOrientation::HORIZONTAL);
  DALI_TEST_EQUALS(layout.GetOrientation(), StackOrientation::HORIZONTAL, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutGetOrientationP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  DALI_TEST_EQUALS(layout.GetOrientation(), StackOrientation::HORIZONTAL, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutSetSpacingP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New();
  const float spacing = 10.0f;
  layout.SetSpacing(spacing);
  DALI_TEST_EQUALS(layout.GetSpacing(), spacing, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutGetSpacingP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New();
  DALI_TEST_EQUALS(layout.GetSpacing(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutSetLayoutWeightP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New();
  View child = View::New();
  layout.Add(child);
  child.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
  DALI_TEST_EQUALS(GetRequiredLayoutParams<StackLayoutParams>(child).GetWeight(), 1.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutGetLayoutWeightP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New();
  View child = View::New();
  layout.Add(child);
  child.SetLayoutParams(StackLayoutParams::New().SetWeight(0.0f));
  DALI_TEST_EQUALS(GetRequiredLayoutParams<StackLayoutParams>(child).GetWeight(), 0.0f, TEST_LOCATION);
  child.SetLayoutParams(StackLayoutParams::New().SetWeight(0.5f));
  DALI_TEST_EQUALS(GetRequiredLayoutParams<StackLayoutParams>(child).GetWeight(), 0.5f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutParamsValueSemanticsP(void)
{
  UiTestApplication application;
  View              a      = View::New();
  View              b      = View::New();
  View              empty  = View::New();
  StackLayoutParams source = StackLayoutParams::New()
                               .SetWeight(1.0f)
                               .SetAlignment(LayoutAlignment::CENTER);

  StackLayoutParams copied(source);
  StackLayoutParams assigned;
  assigned = source;

  a.SetLayoutParams(source);
  b.SetLayoutParams(source);
  source.SetWeight(2.0f).SetAlignment(LayoutAlignment::END);
  DALI_TEST_EQUALS(copied.GetWeight(), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(assigned.GetAlignment(), LayoutAlignment::CENTER, TEST_LOCATION);

  auto storedA = GetRequiredLayoutParams<StackLayoutParams>(a);
  auto storedB = GetRequiredLayoutParams<StackLayoutParams>(b);
  DALI_TEST_EQUALS(storedA.GetWeight(), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(storedA.GetAlignment(), LayoutAlignment::CENTER, TEST_LOCATION);
  DALI_TEST_EQUALS(storedB.GetWeight(), 1.0f, TEST_LOCATION);

  storedA.SetWeight(3.0f).SetAlignment(LayoutAlignment::FILL);
  auto unchangedA = GetRequiredLayoutParams<StackLayoutParams>(a);
  DALI_TEST_EQUALS(unchangedA.GetWeight(), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(unchangedA.GetAlignment(), LayoutAlignment::CENTER, TEST_LOCATION);

  a.SetLayoutParams(storedA);
  auto committedA = GetRequiredLayoutParams<StackLayoutParams>(a);
  auto unchangedB = GetRequiredLayoutParams<StackLayoutParams>(b);
  DALI_TEST_EQUALS(committedA.GetWeight(), 3.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(committedA.GetAlignment(), LayoutAlignment::FILL, TEST_LOCATION);
  DALI_TEST_EQUALS(unchangedB.GetWeight(), 1.0f, TEST_LOCATION);
  StackLayoutParams missingParams = StackLayoutParams::New().SetWeight(7.0f);
  DALI_TEST_CHECK(!empty.TryGetLayoutParams(missingParams));
  DALI_TEST_EQUALS(missingParams.GetWeight(), 7.0f, TEST_LOCATION);
  AbsoluteLayoutParams wrongTypeParams;
  DALI_TEST_CHECK(!a.TryGetLayoutParams(wrongTypeParams));
  END_TEST;
}

int UtcDaliStackLayoutOrientationSetterP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New();
  layout.SetOrientation(StackOrientation::HORIZONTAL);
  DALI_TEST_EQUALS(layout.GetOrientation(), StackOrientation::HORIZONTAL, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutSpacingSetterP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New();
  const float spacing = 5.0f;
  layout.SetSpacing(spacing);
  DALI_TEST_EQUALS(layout.GetSpacing(), spacing, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutContentsP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  View a = View::New();
  layout.Add(a);
  DALI_TEST_EQUALS(layout.GetChildCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(layout.GetChildViewAt(0) == a);
  END_TEST;
}

int UtcDaliStackLayoutWeightMultipleP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New();
  View c1 = View::New();
  View c2 = View::New();
  layout.Add(c1);
  layout.Add(c2);
  c1.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
  c2.SetLayoutParams(StackLayoutParams::New().SetWeight(2.0f));
  DALI_TEST_EQUALS(GetRequiredLayoutParams<StackLayoutParams>(c1).GetWeight(), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRequiredLayoutParams<StackLayoutParams>(c2).GetWeight(), 2.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutMeasureArrangeP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetSpacing(5.0f);
  View v1 = View::New();
  v1.SetRequestedWidth(100.0f);
  v1.SetRequestedHeight(40.0f);
  layout.Add(v1);
  View v2 = View::New();
  v2.SetRequestedWidth(100.0f);
  v2.SetRequestedHeight(40.0f);
  layout.Add(v2);
  layout.SetRequestedWidth(150.0f);
  layout.SetRequestedHeight(200.0f);
  MeasuredSize m = layout.Measure(150.0f, 200.0f);
  LayoutRect a = layout.Arrange(LayoutRect(0, 0, 150, 200));
  DALI_TEST_EQUALS(m.GetWidth(), 150.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m.GetHeight(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetWidth(), 150.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetHeight(), 200.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutMeasureHorizontalP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  View v1 = View::New();
  v1.SetRequestedWidth(50.0f);
  v1.SetRequestedHeight(50.0f);
  layout.Add(v1);
  View v2 = View::New();
  v2.SetRequestedWidth(50.0f);
  v2.SetRequestedHeight(50.0f);
  layout.Add(v2);
  MeasuredSize m = layout.Measure(300.0f, 100.0f);
  DALI_TEST_EQUALS(m.GetWidth(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m.GetHeight(), 50.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutChildNoWeightP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  View noWeight = View::New();
  layout.Add(noWeight);
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
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  View child = View::New();
  child.SetRequestedWidth(MATCH_PARENT);
  child.SetRequestedHeight(MATCH_PARENT);
  layout.Add(child);
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
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  layout.SetSpacing(10.0f);
  View v1 = View::New();
  v1.SetRequestedWidth(60.0f);
  v1.SetRequestedHeight(40.0f);
  layout.Add(v1);
  View v2 = View::New();
  v2.SetRequestedWidth(60.0f);
  v2.SetRequestedHeight(40.0f);
  layout.Add(v2);
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  MeasuredSize m = layout.Measure(200.0f, 100.0f);
  LayoutRect a = layout.Arrange(LayoutRect(0, 0, 200, 100));
  DALI_TEST_EQUALS(m.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m.GetHeight(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetHeight(), 100.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutChildWithWeightP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetSpacing(5.0f);
  View fixedChild = View::New();
  fixedChild.SetRequestedWidth(100.0f);
  fixedChild.SetRequestedHeight(40.0f);
  layout.Add(fixedChild);
  View weightChild = View::New();
  weightChild.SetRequestedWidth(MATCH_PARENT);
  weightChild.SetRequestedHeight(50.0f);
  layout.Add(weightChild);
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
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedWidth(80.0f);
  child.SetRequestedHeight(40.0f);
  layout.Add(child);
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
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedWidth(80.0f);
  child.SetRequestedHeight(40.0f);
  layout.Add(child);
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
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedWidth(80.0f);
  child.SetRequestedHeight(40.0f);
  layout.Add(child);
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
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedWidth(MATCH_PARENT);
  child.SetRequestedHeight(40.0f);
  layout.Add(child);
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
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedHeight(40.0f);
  child.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  layout.Add(child);
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
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedWidth(80.0f);
  child.SetRequestedHeight(40.0f);
  child.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  layout.Add(child);
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
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedWidth(60.0f);
  child.SetRequestedHeight(40.0f);
  layout.Add(child);
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
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedWidth(60.0f);
  child.SetRequestedHeight(40.0f);
  layout.Add(child);
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
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedWidth(60.0f);
  child.SetRequestedHeight(40.0f);
  layout.Add(child);
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
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedWidth(60.0f);
  child.SetRequestedHeight(MATCH_PARENT);
  layout.Add(child);
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
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedWidth(60.0f);
  child.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  layout.Add(child);
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
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  layout.SetSpacing(0.0f);
  View child = View::New();
  child.SetRequestedWidth(60.0f);
  child.SetRequestedHeight(40.0f);
  child.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  layout.Add(child);
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 100));
  DALI_TEST_EQUALS(child.GetSize().height, 40.0f, TEST_LOCATION); // explicit size wins
  END_TEST;
}

int UtcDaliStackLayoutStandaloneExcludedFromAccumulationP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetSpacing(10.0f);

  View normal1 = View::New();
  normal1.SetRequestedWidth(50.0f);
  normal1.SetRequestedHeight(30.0f);
  layout.Add(normal1);

  View standalone = View::New();
  standalone.SetLayoutMode(LayoutMode::STANDALONE);
  standalone.SetRequestedWidth(40.0f);
  standalone.SetRequestedHeight(40.0f);
  standalone.SetRequestedX(80.0f);
  standalone.SetRequestedY(90.0f);
  layout.Add(standalone);

  View normal2 = View::New();
  normal2.SetRequestedWidth(50.0f);
  normal2.SetRequestedHeight(30.0f);
  layout.Add(normal2);

  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(200.0f);
  layout.Measure(200.0f, 200.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 200));

  // Standalone child does not advance the stack cursor or contribute to spacing.
  // normal1 at y=0 height=30, spacing=10, normal2 at y=40.
  DALI_TEST_EQUALS(normal1.GetPositionY(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(normal2.GetPositionY(), 40.0f, TEST_LOCATION);

  // Standalone is at its requested position (parent padding ignored, no margin).
  DALI_TEST_EQUALS(standalone.GetPositionX(), 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetPositionY(), 90.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetSize().width, 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetSize().height, 40.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutStandaloneIgnoresParentPaddingP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetPadding(Extents(10, 10, 10, 10));

  View standalone = View::New();
  standalone.SetLayoutMode(LayoutMode::STANDALONE);
  standalone.SetMargin(Extents(5, 5, 7, 7));
  standalone.SetRequestedWidth(MATCH_PARENT);
  standalone.SetRequestedHeight(MATCH_PARENT);
  layout.Add(standalone);

  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(150.0f);
  layout.Measure(200.0f, 150.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 150));

  // Parent padding ignored: full parent inner size minus own margin.
  DALI_TEST_EQUALS(standalone.GetSize().width, 200.0f - 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetSize().height, 150.0f - 14.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetPositionX(), 5.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetPositionY(), 7.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutHorizontalDirectionLtrP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  application.GetScene().Add(layout);

  View a = View::New();
  a.SetRequestedWidth(40.0f);
  a.SetRequestedHeight(50.0f);
  a.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  layout.Add(a);
  View b = View::New();
  b.SetRequestedWidth(50.0f);
  b.SetRequestedHeight(50.0f);
  b.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  layout.Add(b);
  View c = View::New();
  c.SetRequestedWidth(60.0f);
  c.SetRequestedHeight(50.0f);
  c.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  layout.Add(c);

  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 100.0f));

  // Horizontal stack in LTR: a at 0, b at 40, c at 90.
  DALI_TEST_EQUALS(a.GetPositionX(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetSize().width, 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetPositionX(), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetSize().width, 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(c.GetPositionX(), 90.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(c.GetSize().width, 60.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStackLayoutHorizontalDirectionRtlP(void)
{
  UiTestApplication application;
  StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL);
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  layout.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  application.GetScene().Add(layout);

  View a = View::New();
  a.SetRequestedWidth(40.0f);
  a.SetRequestedHeight(50.0f);
  a.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  layout.Add(a);
  View b = View::New();
  b.SetRequestedWidth(50.0f);
  b.SetRequestedHeight(50.0f);
  b.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  layout.Add(b);
  View c = View::New();
  c.SetRequestedWidth(60.0f);
  c.SetRequestedHeight(50.0f);
  c.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  layout.Add(c);

  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 100.0f));

  // Mirrored from LTR (0, 40, 90); sizes unchanged.
  DALI_TEST_EQUALS(a.GetPositionX(), 200.0f - 0.0f - 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetSize().width, 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetPositionX(), 200.0f - 40.0f - 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(b.GetSize().width, 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(c.GetPositionX(), 200.0f - 90.0f - 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(c.GetSize().width, 60.0f, TEST_LOCATION);
  END_TEST;
}

// FIX 2: VERTICAL stack — a weighted child with RequestedHeight=MATCH_PARENT must
// receive its weight-allocated height, not the full available height.
//
// Container 200x120, spacing 0.
// Child A: weight=1, RequestedHeight=MATCH_PARENT, RequestedWidth=100.
// Child B: weight=0, RequestedHeight=40,           RequestedWidth=100.
//
// Measure pass: B is fixed at 40; A has MATCH_PARENT so it contributes 0 to
// the natural accumulation.  Total natural = 40; remaining = 120 - 40 = 80.
// Weight distribution: A gets all 80 (weight share 1/1).
// Arrange pass: A height = 80 (weight allocation), NOT 120 (MATCH_PARENT full).
// A.y = 0; B.y = 80; B.height = 40.
int UtcDaliStackLayoutWeightWithMainAxisMatchParentP(void)
{
  UiTestApplication application;
  StackLayout       layout = StackLayout::New(StackOrientation::VERTICAL);
  layout.SetSpacing(0.0f);
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(120.0f);

  View childA = View::New();
  childA.SetRequestedWidth(100.0f);
  childA.SetRequestedHeight(MATCH_PARENT);
  childA.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
  layout.Add(childA);

  View childB = View::New();
  childB.SetRequestedWidth(100.0f);
  childB.SetRequestedHeight(40.0f);
  layout.Add(childB);

  layout.Measure(200.0f, 120.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 120));

  DALI_TEST_EQUALS(childA.GetSize().height, 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(childA.GetPositionY(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(childB.GetPositionY(), 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(childB.GetSize().height, 40.0f, TEST_LOCATION);
  END_TEST;
}

// FIX 2: HORIZONTAL stack — mirror of the VERTICAL test above.
//
// Container 120x200, spacing 0.
// Child A: weight=1, RequestedWidth=MATCH_PARENT, RequestedHeight=100.
// Child B: weight=0, RequestedWidth=40,           RequestedHeight=100.
//
// Remaining after B = 120 - 40 = 80; A gets all 80 via weight.
// A.x = 0; A.width = 80; B.x = 80; B.width = 40.
int UtcDaliStackLayoutWeightWithMainAxisMatchParentHorizontalP(void)
{
  UiTestApplication application;
  StackLayout       layout = StackLayout::New(StackOrientation::HORIZONTAL);
  layout.SetSpacing(0.0f);
  layout.SetRequestedWidth(120.0f);
  layout.SetRequestedHeight(200.0f);

  View childA = View::New();
  childA.SetRequestedWidth(MATCH_PARENT);
  childA.SetRequestedHeight(100.0f);
  childA.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
  layout.Add(childA);

  View childB = View::New();
  childB.SetRequestedWidth(40.0f);
  childB.SetRequestedHeight(100.0f);
  layout.Add(childB);

  layout.Measure(120.0f, 200.0f);
  layout.Arrange(LayoutRect(0, 0, 120, 200));

  DALI_TEST_EQUALS(childA.GetSize().width, 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(childA.GetPositionX(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(childB.GetPositionX(), 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(childB.GetSize().width, 40.0f, TEST_LOCATION);
  END_TEST;
}
