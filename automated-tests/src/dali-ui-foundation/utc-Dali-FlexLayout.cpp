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
#include <dali-ui-foundation/public-api/flex-layout.h>
#include <dali-ui-foundation/public-api/layout-types.h>
#include <dali-ui-foundation/public-api/view.h>

using namespace Dali;
using namespace Dali::UI;

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
  layout.SetDirection(FlexDirection::Row);
  DALI_TEST_EQUALS(layout.GetDirection(), FlexDirection::Row, TEST_LOCATION);
  layout.SetDirection(FlexDirection::ColumnReverse);
  DALI_TEST_EQUALS(layout.GetDirection(), FlexDirection::ColumnReverse, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutGetDirectionP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  DALI_TEST_EQUALS(layout.GetDirection(), FlexDirection::Row, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutSetWrapP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  layout.SetWrap(FlexWrap::Wrap);
  DALI_TEST_EQUALS(layout.GetWrap(), FlexWrap::Wrap, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutGetWrapP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  DALI_TEST_EQUALS(layout.GetWrap(), FlexWrap::NoWrap, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutSetJustifyContentP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  layout.SetJustifyContent(FlexJustify::SpaceBetween);
  DALI_TEST_EQUALS(layout.GetJustifyContent(), FlexJustify::SpaceBetween, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutGetJustifyContentP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  DALI_TEST_EQUALS(layout.GetJustifyContent(), FlexJustify::FlexStart, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutSetAlignItemsP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  layout.SetAlignItems(FlexAlign::Center);
  DALI_TEST_EQUALS(layout.GetAlignItems(), FlexAlign::Center, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutGetAlignItemsP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  DALI_TEST_EQUALS(layout.GetAlignItems(), FlexAlign::Stretch, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutSetAlignContentP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  layout.SetAlignContent(FlexAlign::FlexEnd);
  DALI_TEST_EQUALS(layout.GetAlignContent(), FlexAlign::FlexEnd, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutGetAlignContentP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  DALI_TEST_EQUALS(layout.GetAlignContent(), FlexAlign::Stretch, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutSetFlexGrowP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  View child = View::New();
  layout.AddView(child);
  FlexLayout::SetFlexGrow(child, 1.0f);
  DALI_TEST_EQUALS(FlexLayout::GetFlexGrow(child), 1.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutGetFlexGrowP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  View child = View::New();
  layout.AddView(child);
  DALI_TEST_EQUALS(FlexLayout::GetFlexGrow(child), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutSetFlexShrinkP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  View child = View::New();
  layout.AddView(child);
  FlexLayout::SetFlexShrink(child, 0.5f);
  DALI_TEST_EQUALS(FlexLayout::GetFlexShrink(child), 0.5f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutGetFlexShrinkP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  View child = View::New();
  layout.AddView(child);
  DALI_TEST_EQUALS(FlexLayout::GetFlexShrink(child), 1.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutSetFlexBasisP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  View child = View::New();
  layout.AddView(child);
  FlexLayout::SetFlexBasis(child, 100.0f);
  DALI_TEST_EQUALS(FlexLayout::GetFlexBasis(child), 100.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutGetFlexBasisP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  View child = View::New();
  layout.AddView(child);
  DALI_TEST_EQUALS(FlexLayout::GetFlexBasis(child), LayoutDimension::WrapContent, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutSetAlignSelfP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  View child = View::New();
  layout.AddView(child);
  FlexLayout::SetAlignSelf(child, FlexAlign::Baseline);
  DALI_TEST_EQUALS(FlexLayout::GetAlignSelf(child), FlexAlign::Baseline, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutGetAlignSelfP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  View child = View::New();
  layout.AddView(child);
  DALI_TEST_EQUALS(FlexLayout::GetAlignSelf(child), FlexAlign::Auto, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutDirectionChainingP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  FlexLayout& result = layout.Direction(FlexDirection::Column);
  DALI_TEST_EQUALS(&result, &layout, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.GetDirection(), FlexDirection::Column, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutWrapChainingP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  FlexLayout& result = layout.Wrap(FlexWrap::WrapReverse);
  DALI_TEST_EQUALS(&result, &layout, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.GetWrap(), FlexWrap::WrapReverse, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutJustifyContentChainingP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  FlexLayout& result = layout.JustifyContent(FlexJustify::SpaceEvenly);
  DALI_TEST_EQUALS(&result, &layout, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.GetJustifyContent(), FlexJustify::SpaceEvenly, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutAlignItemsChainingP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  FlexLayout& result = layout.AlignItems(FlexAlign::Stretch);
  DALI_TEST_EQUALS(&result, &layout, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.GetAlignItems(), FlexAlign::Stretch, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutAlignContentChainingP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  FlexLayout& result = layout.AlignContent(FlexAlign::Center);
  DALI_TEST_EQUALS(&result, &layout, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.GetAlignContent(), FlexAlign::Center, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutAllDirectionsP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  layout.SetDirection(FlexDirection::Row);
  DALI_TEST_EQUALS(layout.GetDirection(), FlexDirection::Row, TEST_LOCATION);
  layout.SetDirection(FlexDirection::RowReverse);
  DALI_TEST_EQUALS(layout.GetDirection(), FlexDirection::RowReverse, TEST_LOCATION);
  layout.SetDirection(FlexDirection::Column);
  DALI_TEST_EQUALS(layout.GetDirection(), FlexDirection::Column, TEST_LOCATION);
  layout.SetDirection(FlexDirection::ColumnReverse);
  DALI_TEST_EQUALS(layout.GetDirection(), FlexDirection::ColumnReverse, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutAllJustifyP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  layout.SetJustifyContent(FlexJustify::FlexEnd);
  DALI_TEST_EQUALS(layout.GetJustifyContent(), FlexJustify::FlexEnd, TEST_LOCATION);
  layout.SetJustifyContent(FlexJustify::Center);
  DALI_TEST_EQUALS(layout.GetJustifyContent(), FlexJustify::Center, TEST_LOCATION);
  layout.SetJustifyContent(FlexJustify::SpaceAround);
  DALI_TEST_EQUALS(layout.GetJustifyContent(), FlexJustify::SpaceAround, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutAllAlignItemsP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  layout.SetAlignItems(FlexAlign::FlexStart);
  DALI_TEST_EQUALS(layout.GetAlignItems(), FlexAlign::FlexStart, TEST_LOCATION);
  layout.SetAlignItems(FlexAlign::FlexEnd);
  DALI_TEST_EQUALS(layout.GetAlignItems(), FlexAlign::FlexEnd, TEST_LOCATION);
  layout.SetAlignItems(FlexAlign::Baseline);
  DALI_TEST_EQUALS(layout.GetAlignItems(), FlexAlign::Baseline, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFlexLayoutMeasureArrangeP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
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

int UtcDaliFlexLayoutJustifyContentVariantsP(void)
{
  TestApplication application;
  FlexLayout layout = FlexLayout::New();
  View v1 = View::New();
  v1.SetLayoutWidth(40.0f);
  v1.SetLayoutHeight(30.0f);
  layout.AddView(v1);
  View v2 = View::New();
  v2.SetLayoutWidth(40.0f);
  v2.SetLayoutHeight(30.0f);
  layout.AddView(v2);
  layout.SetLayoutWidth(200.0f);
  layout.SetLayoutHeight(80.0f);
  layout.SetJustifyContent(FlexJustify::SpaceBetween);
  MeasuredSize m1 = layout.Measure(200.0f, 80.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 80));
  layout.SetJustifyContent(FlexJustify::SpaceAround);
  layout.Measure(200.0f, 80.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 80));
  layout.SetJustifyContent(FlexJustify::SpaceEvenly);
  layout.Measure(200.0f, 80.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 80));
  layout.SetJustifyContent(FlexJustify::FlexEnd);
  layout.Measure(200.0f, 80.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 80));
  layout.SetJustifyContent(FlexJustify::Center);
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
  v1.SetLayoutWidth(50.0f);
  v1.SetLayoutHeight(30.0f);
  layout.AddView(v1);
  View v2 = View::New();
  v2.SetLayoutWidth(50.0f);
  v2.SetLayoutHeight(50.0f);
  layout.AddView(v2);
  layout.SetLayoutWidth(200.0f);
  layout.SetLayoutHeight(100.0f);
  layout.SetAlignItems(FlexAlign::FlexEnd);
  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 100));
  layout.SetAlignItems(FlexAlign::Center);
  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 100));
  layout.SetAlignItems(FlexAlign::Stretch);
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
  layout.SetDirection(FlexDirection::RowReverse);
  View v1 = View::New();
  v1.SetLayoutWidth(40.0f);
  v1.SetLayoutHeight(40.0f);
  layout.AddView(v1);
  View v2 = View::New();
  v2.SetLayoutWidth(40.0f);
  v2.SetLayoutHeight(40.0f);
  layout.AddView(v2);
  layout.SetLayoutWidth(200.0f);
  layout.SetLayoutHeight(80.0f);
  layout.Measure(200.0f, 80.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 80));
  layout.SetDirection(FlexDirection::ColumnReverse);
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
  layout.SetWrap(FlexWrap::Wrap);
  View v1 = View::New();
  v1.SetLayoutWidth(60.0f);
  v1.SetLayoutHeight(40.0f);
  layout.AddView(v1);
  View v2 = View::New();
  v2.SetLayoutWidth(60.0f);
  v2.SetLayoutHeight(40.0f);
  layout.AddView(v2);
  View v3 = View::New();
  v3.SetLayoutWidth(60.0f);
  v3.SetLayoutHeight(40.0f);
  layout.AddView(v3);
  layout.SetLayoutWidth(100.0f);
  layout.SetLayoutHeight(150.0f);
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
  layout.SetWrap(FlexWrap::WrapReverse);
  View v1 = View::New();
  v1.SetLayoutWidth(50.0f);
  v1.SetLayoutHeight(30.0f);
  layout.AddView(v1);
  View v2 = View::New();
  v2.SetLayoutWidth(50.0f);
  v2.SetLayoutHeight(30.0f);
  layout.AddView(v2);
  layout.SetLayoutWidth(80.0f);
  layout.SetLayoutHeight(100.0f);
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
  v1.SetLayoutWidth(40.0f);
  v1.SetLayoutHeight(40.0f);
  layout.AddView(v1);
  View collapsed = View::New();
  collapsed.SetLayoutWidth(40.0f);
  collapsed.SetLayoutHeight(40.0f);
  layout.AddView(collapsed);
  View v2 = View::New();
  v2.SetLayoutWidth(40.0f);
  v2.SetLayoutHeight(40.0f);
  layout.AddView(v2);
  layout.SetLayoutWidth(200.0f);
  layout.SetLayoutHeight(80.0f);
  MeasuredSize m = layout.Measure(200.0f, 80.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 80));
  DALI_TEST_EQUALS(m.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m.GetHeight(), 80.0f, TEST_LOCATION);
  END_TEST;
}
