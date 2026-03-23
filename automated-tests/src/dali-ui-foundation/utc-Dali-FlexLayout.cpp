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
#include <dali-ui-foundation/public-api/view.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_flexlayout_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_flexlayout_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliFlexLayoutConstructorP(void)
{
  TestApplication application;
  FlexLayout layout;
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliFlexLayoutNewP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  DALI_TEST_CHECK(layout);
  END_TEST;
}

int UtcDaliFlexLayoutCopyConstructorP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  FlexLayout copy(layout);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(layout == copy);
  END_TEST;
}

int UtcDaliFlexLayoutMoveConstructor(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  FlexLayout moved = std::move(layout);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliFlexLayoutAssignmentOperatorP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  FlexLayout copy;
  copy = layout;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(layout == copy);
  END_TEST;
}

int UtcDaliFlexLayoutDownCastP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  FlexLayout layout2 = FlexLayout::DownCast(layout);
  DALI_TEST_CHECK(layout2);
  DALI_TEST_CHECK(layout == layout2);
  END_TEST;
}

int UtcDaliFlexLayoutDownCastN(void)
{
  TestApplication application;
  BaseHandle unInitialized;
  FlexLayout layout = FlexLayout::DownCast(unInitialized);
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliFlexLayoutSetDirectionP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  layout.SetDirection(FlexDirection::ROW);
  DALI_TEST_EQUALS(layout.GetDirection(), FlexDirection::ROW, TEST_LOCATION);
  layout.SetDirection(FlexDirection::COLUMN_REVERSE);
  DALI_TEST_EQUALS(layout.GetDirection(), FlexDirection::COLUMN_REVERSE, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutGetDirectionP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  DALI_TEST_EQUALS(layout.GetDirection(), FlexDirection::ROW, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutSetWrapP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  layout.SetWrap(FlexWrap::WRAP);
  DALI_TEST_EQUALS(layout.GetWrap(), FlexWrap::WRAP, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutGetWrapP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  DALI_TEST_EQUALS(layout.GetWrap(), FlexWrap::NO_WRAP, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutSetJustifyContentP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  layout.SetJustifyContent(FlexJustify::SPACE_BETWEEN);
  DALI_TEST_EQUALS(layout.GetJustifyContent(), FlexJustify::SPACE_BETWEEN, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutGetJustifyContentP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  DALI_TEST_EQUALS(layout.GetJustifyContent(), FlexJustify::FLEX_START, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutSetAlignItemsP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  layout.SetAlignItems(FlexAlign::CENTER);
  DALI_TEST_EQUALS(layout.GetAlignItems(), FlexAlign::CENTER, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutGetAlignItemsP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  DALI_TEST_EQUALS(layout.GetAlignItems(), FlexAlign::STRETCH, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutSetAlignContentP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  layout.SetAlignContent(FlexAlign::FLEX_END);
  DALI_TEST_EQUALS(layout.GetAlignContent(), FlexAlign::FLEX_END, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutGetAlignContentP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  DALI_TEST_EQUALS(layout.GetAlignContent(), FlexAlign::STRETCH, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutSetFlexGrowP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  View child = View::New();
  layout.AddView(child);
  child.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
  DALI_TEST_EQUALS(child.GetLayoutParams<FlexLayoutParams>().GetFlexGrow(), 1.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutGetFlexGrowP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  View child = View::New();
  layout.AddView(child);
  child.SetLayoutParams(FlexLayoutParams::New());
  DALI_TEST_EQUALS(child.GetLayoutParams<FlexLayoutParams>().GetFlexGrow(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutSetFlexShrinkP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  View child = View::New();
  layout.AddView(child);
  child.SetLayoutParams(FlexLayoutParams::New().SetFlexShrink(0.5f));
  DALI_TEST_EQUALS(child.GetLayoutParams<FlexLayoutParams>().GetFlexShrink(), 0.5f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutGetFlexShrinkP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  View child = View::New();
  layout.AddView(child);
  child.SetLayoutParams(FlexLayoutParams::New());
  DALI_TEST_EQUALS(child.GetLayoutParams<FlexLayoutParams>().GetFlexShrink(), 1.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutSetFlexBasisP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  View child = View::New();
  layout.AddView(child);
  child.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(100.0f));
  DALI_TEST_EQUALS(child.GetLayoutParams<FlexLayoutParams>().GetFlexBasis(), 100.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutGetFlexBasisP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  View child = View::New();
  layout.AddView(child);
  child.SetLayoutParams(FlexLayoutParams::New());
  DALI_TEST_EQUALS(child.GetLayoutParams<FlexLayoutParams>().GetFlexBasis(), WRAP_CONTENT, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutSetAlignSelfP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  View child = View::New();
  layout.AddView(child);
  child.SetLayoutParams(FlexLayoutParams::New().SetAlignSelf(FlexAlign::BASELINE));
  DALI_TEST_EQUALS(child.GetLayoutParams<FlexLayoutParams>().GetAlignSelf(), FlexAlign::BASELINE, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutGetAlignSelfP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  View child = View::New();
  layout.AddView(child);
  child.SetLayoutParams(FlexLayoutParams::New());
  DALI_TEST_EQUALS(child.GetLayoutParams<FlexLayoutParams>().GetAlignSelf(), FlexAlign::AUTO, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutDirectionChainingP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  FlexLayout& result = layout.Direction(FlexDirection::COLUMN);
  DALI_TEST_EQUALS(&result, &layout, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.GetDirection(), FlexDirection::COLUMN, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutWrapChainingP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  FlexLayout& result = layout.Wrap(FlexWrap::WRAP_REVERSE);
  DALI_TEST_EQUALS(&result, &layout, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.GetWrap(), FlexWrap::WRAP_REVERSE, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutJustifyContentChainingP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  FlexLayout& result = layout.JustifyContent(FlexJustify::SPACE_EVENLY);
  DALI_TEST_EQUALS(&result, &layout, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.GetJustifyContent(), FlexJustify::SPACE_EVENLY, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutAlignItemsChainingP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  FlexLayout& result = layout.AlignItems(FlexAlign::STRETCH);
  DALI_TEST_EQUALS(&result, &layout, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.GetAlignItems(), FlexAlign::STRETCH, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutAlignContentChainingP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  FlexLayout& result = layout.AlignContent(FlexAlign::CENTER);
  DALI_TEST_EQUALS(&result, &layout, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.GetAlignContent(), FlexAlign::CENTER, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutAllDirectionsP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  layout.SetDirection(FlexDirection::ROW);
  DALI_TEST_EQUALS(layout.GetDirection(), FlexDirection::ROW, TEST_LOCATION);
  layout.SetDirection(FlexDirection::ROW_REVERSE);
  DALI_TEST_EQUALS(layout.GetDirection(), FlexDirection::ROW_REVERSE, TEST_LOCATION);
  layout.SetDirection(FlexDirection::COLUMN);
  DALI_TEST_EQUALS(layout.GetDirection(), FlexDirection::COLUMN, TEST_LOCATION);
  layout.SetDirection(FlexDirection::COLUMN_REVERSE);
  DALI_TEST_EQUALS(layout.GetDirection(), FlexDirection::COLUMN_REVERSE, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutAllJustifyP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  layout.SetJustifyContent(FlexJustify::FLEX_END);
  DALI_TEST_EQUALS(layout.GetJustifyContent(), FlexJustify::FLEX_END, TEST_LOCATION);
  layout.SetJustifyContent(FlexJustify::CENTER);
  DALI_TEST_EQUALS(layout.GetJustifyContent(), FlexJustify::CENTER, TEST_LOCATION);
  layout.SetJustifyContent(FlexJustify::SPACE_AROUND);
  DALI_TEST_EQUALS(layout.GetJustifyContent(), FlexJustify::SPACE_AROUND, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutAllAlignItemsP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  layout.SetAlignItems(FlexAlign::FLEX_START);
  DALI_TEST_EQUALS(layout.GetAlignItems(), FlexAlign::FLEX_START, TEST_LOCATION);
  layout.SetAlignItems(FlexAlign::FLEX_END);
  DALI_TEST_EQUALS(layout.GetAlignItems(), FlexAlign::FLEX_END, TEST_LOCATION);
  layout.SetAlignItems(FlexAlign::BASELINE);
  DALI_TEST_EQUALS(layout.GetAlignItems(), FlexAlign::BASELINE, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutMeasureArrangeP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
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

int UtcDaliFlexLayoutJustifyContentVariantsP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  View v1 = View::New();
  v1.SetRequestedWidth(40.0f);
  v1.SetRequestedHeight(30.0f);
  layout.AddView(v1);
  View v2 = View::New();
  v2.SetRequestedWidth(40.0f);
  v2.SetRequestedHeight(30.0f);
  layout.AddView(v2);
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(80.0f);
  layout.SetJustifyContent(FlexJustify::SPACE_BETWEEN);
  MeasuredSize m1 = layout.Measure(200.0f, 80.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 80));
  layout.SetJustifyContent(FlexJustify::SPACE_AROUND);
  layout.Measure(200.0f, 80.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 80));
  layout.SetJustifyContent(FlexJustify::SPACE_EVENLY);
  layout.Measure(200.0f, 80.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 80));
  layout.SetJustifyContent(FlexJustify::FLEX_END);
  layout.Measure(200.0f, 80.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 80));
  layout.SetJustifyContent(FlexJustify::CENTER);
  layout.Measure(200.0f, 80.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 80));
  DALI_TEST_EQUALS(m1.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m1.GetHeight(), 80.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutAlignItemsVariantsP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  View v1 = View::New();
  v1.SetRequestedWidth(50.0f);
  v1.SetRequestedHeight(30.0f);
  layout.AddView(v1);
  View v2 = View::New();
  v2.SetRequestedWidth(50.0f);
  v2.SetRequestedHeight(50.0f);
  layout.AddView(v2);
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  layout.SetAlignItems(FlexAlign::FLEX_END);
  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 100));
  layout.SetAlignItems(FlexAlign::CENTER);
  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 100));
  layout.SetAlignItems(FlexAlign::STRETCH);
  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 100));
  MeasuredSize ma = layout.Measure(200.0f, 100.0f);
  DALI_TEST_EQUALS(layout.GetChildCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(ma.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(ma.GetHeight(), 100.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutDirectionReverseP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  layout.SetDirection(FlexDirection::ROW_REVERSE);
  View v1 = View::New();
  v1.SetRequestedWidth(40.0f);
  v1.SetRequestedHeight(40.0f);
  layout.AddView(v1);
  View v2 = View::New();
  v2.SetRequestedWidth(40.0f);
  v2.SetRequestedHeight(40.0f);
  layout.AddView(v2);
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(80.0f);
  layout.Measure(200.0f, 80.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 80));
  layout.SetDirection(FlexDirection::COLUMN_REVERSE);
  MeasuredSize m2 = layout.Measure(200.0f, 80.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 80));
  DALI_TEST_EQUALS(m2.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m2.GetHeight(), 80.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutWrapP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  layout.SetWrap(FlexWrap::WRAP);
  View v1 = View::New();
  v1.SetRequestedWidth(60.0f);
  v1.SetRequestedHeight(40.0f);
  layout.AddView(v1);
  View v2 = View::New();
  v2.SetRequestedWidth(60.0f);
  v2.SetRequestedHeight(40.0f);
  layout.AddView(v2);
  View v3 = View::New();
  v3.SetRequestedWidth(60.0f);
  v3.SetRequestedHeight(40.0f);
  layout.AddView(v3);
  layout.SetRequestedWidth(100.0f);
  layout.SetRequestedHeight(150.0f);
  MeasuredSize m = layout.Measure(100.0f, 150.0f);
  layout.Arrange(LayoutRect(0, 0, 100, 150));
  DALI_TEST_EQUALS(m.GetWidth(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m.GetHeight(), 150.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutWrapReverseP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  layout.SetWrap(FlexWrap::WRAP_REVERSE);
  View v1 = View::New();
  v1.SetRequestedWidth(50.0f);
  v1.SetRequestedHeight(30.0f);
  layout.AddView(v1);
  View v2 = View::New();
  v2.SetRequestedWidth(50.0f);
  v2.SetRequestedHeight(30.0f);
  layout.AddView(v2);
  layout.SetRequestedWidth(80.0f);
  layout.SetRequestedHeight(100.0f);
  MeasuredSize m = layout.Measure(80.0f, 100.0f);
  layout.Arrange(LayoutRect(0, 0, 80, 100));
  DALI_TEST_EQUALS(m.GetWidth(), 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m.GetHeight(), 100.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutCollapsedChildP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  View v1 = View::New();
  v1.SetRequestedWidth(40.0f);
  v1.SetRequestedHeight(40.0f);
  layout.AddView(v1);
  View collapsed = View::New();
  collapsed.SetRequestedWidth(40.0f);
  collapsed.SetRequestedHeight(40.0f);
  layout.AddView(collapsed);
  View v2 = View::New();
  v2.SetRequestedWidth(40.0f);
  v2.SetRequestedHeight(40.0f);
  layout.AddView(v2);
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(80.0f);
  MeasuredSize m = layout.Measure(200.0f, 80.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 80));
  DALI_TEST_EQUALS(m.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m.GetHeight(), 80.0f, TEST_LOCATION);
  END_TEST;
}
