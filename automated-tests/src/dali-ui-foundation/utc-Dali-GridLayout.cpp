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
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_gridlayout_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_gridlayout_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliGridLayoutConstructorP(void)
{
  TestApplication application;
  GridLayout layout;
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliGridLayoutNewP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  DALI_TEST_CHECK(layout);
  END_TEST;
}

int UtcDaliGridLayoutCopyConstructorP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  GridLayout copy(layout);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(layout == copy);
  END_TEST;
}

int UtcDaliGridLayoutMoveConstructor(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  GridLayout moved = std::move(layout);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliGridLayoutAssignmentOperatorP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  GridLayout copy;
  copy = layout;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(layout == copy);
  END_TEST;
}

int UtcDaliGridLayoutDownCastP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  GridLayout layout2 = GridLayout::DownCast(layout);
  DALI_TEST_CHECK(layout2);
  DALI_TEST_CHECK(layout == layout2);
  END_TEST;
}

int UtcDaliGridLayoutDownCastN(void)
{
  TestApplication application;
  BaseHandle unInitialized;
  GridLayout layout = GridLayout::DownCast(unInitialized);
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliGridLayoutAddRowDefinitionP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  layout.AddRowDefinition(GridLength::Absolute(50.0f));
  layout.AddRowDefinition(GridLength::Star(2.0f));
  DALI_TEST_EQUALS(layout.GetRowCount(), 2u, TEST_LOCATION);
  std::vector<GridLength> rows = layout.GetRowDefinitions();
  DALI_TEST_EQUALS(rows.size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(rows[0].GetType(), GridLengthType::ABSOLUTE, TEST_LOCATION);
  DALI_TEST_EQUALS(rows[0].GetValue(), 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(rows[1].GetType(), GridLengthType::STAR, TEST_LOCATION);
  DALI_TEST_EQUALS(rows[1].GetValue(), 2.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutAddColumnDefinitionP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  layout.AddColumnDefinition(GridLength::Auto());
  layout.AddColumnDefinition(GridLength::Star(1.0f));
  DALI_TEST_EQUALS(layout.GetColumnCount(), 2u, TEST_LOCATION);
  std::vector<GridLength> cols = layout.GetColumnDefinitions();
  DALI_TEST_EQUALS(cols.size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(cols[0].GetType(), GridLengthType::AUTO, TEST_LOCATION);
  DALI_TEST_EQUALS(cols[1].GetType(), GridLengthType::STAR, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutSetRowDefinitionsP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  std::vector<GridLength> rows;
  rows.push_back(GridLength::Absolute(100.0f));
  rows.push_back(GridLength::Star(1.0f));
  layout.SetRowDefinitions(rows);
  DALI_TEST_EQUALS(layout.GetRowCount(), 2u, TEST_LOCATION);
  std::vector<GridLength> got = layout.GetRowDefinitions();
  DALI_TEST_EQUALS(got[0].GetValue(), 100.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutSetColumnDefinitionsP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  std::vector<GridLength> cols;
  cols.push_back(GridLength::Star(1.0f));
  layout.SetColumnDefinitions(cols);
  DALI_TEST_EQUALS(layout.GetColumnCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutGetRowDefinitionsP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  layout.AddRowDefinition(GridLength::Absolute(30.0f));
  std::vector<GridLength> rows = layout.GetRowDefinitions();
  DALI_TEST_EQUALS(rows.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(rows[0].GetValue(), 30.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutGetColumnDefinitionsP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  layout.AddColumnDefinition(GridLength::Star(2.0f));
  std::vector<GridLength> cols = layout.GetColumnDefinitions();
  DALI_TEST_EQUALS(cols.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(cols[0].GetValue(), 2.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutGetRowCountP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  DALI_TEST_EQUALS(layout.GetRowCount(), 0u, TEST_LOCATION);
  layout.AddRowDefinition(GridLength::Absolute(10.0f));
  DALI_TEST_EQUALS(layout.GetRowCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutGetColumnCountP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  DALI_TEST_EQUALS(layout.GetColumnCount(), 0u, TEST_LOCATION);
  layout.AddColumnDefinition(GridLength::Auto());
  DALI_TEST_EQUALS(layout.GetColumnCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutClearRowDefinitionsP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  layout.AddRowDefinition(GridLength::Absolute(10.0f));
  layout.ClearRowDefinitions();
  DALI_TEST_EQUALS(layout.GetRowCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutClearColumnDefinitionsP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  layout.AddColumnDefinition(GridLength::Star(1.0f));
  layout.ClearColumnDefinitions();
  DALI_TEST_EQUALS(layout.GetColumnCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutSetRowSpacingP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  const float spacing = 8.0f;
  layout.SetRowSpacing(spacing);
  DALI_TEST_EQUALS(layout.GetRowSpacing(), spacing, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutGetRowSpacingP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  DALI_TEST_EQUALS(layout.GetRowSpacing(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutSetColumnSpacingP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  const float spacing = 12.0f;
  layout.SetColumnSpacing(spacing);
  DALI_TEST_EQUALS(layout.GetColumnSpacing(), spacing, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutGetColumnSpacingP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  DALI_TEST_EQUALS(layout.GetColumnSpacing(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutSetRowP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  View child = View::New();
  layout.AddView(child);
  child.SetLayoutParams(GridLayoutParams::New().SetRow(2));
  DALI_TEST_EQUALS(child.GetLayoutParams<GridLayoutParams>().GetRow(), 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutGetRowP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  View child = View::New();
  layout.AddView(child);
  child.SetLayoutParams(GridLayoutParams::New());
  DALI_TEST_EQUALS(child.GetLayoutParams<GridLayoutParams>().GetRow(), 0u, TEST_LOCATION);
  child.SetLayoutParams(GridLayoutParams::New().SetRow(1));
  DALI_TEST_EQUALS(child.GetLayoutParams<GridLayoutParams>().GetRow(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutSetColumnP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  View child = View::New();
  layout.AddView(child);
  child.SetLayoutParams(GridLayoutParams::New().SetColumn(3));
  DALI_TEST_EQUALS(child.GetLayoutParams<GridLayoutParams>().GetColumn(), 3u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutGetColumnP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  View child = View::New();
  layout.AddView(child);
  child.SetLayoutParams(GridLayoutParams::New());
  DALI_TEST_EQUALS(child.GetLayoutParams<GridLayoutParams>().GetColumn(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutSetRowSpanP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  View child = View::New();
  layout.AddView(child);
  child.SetLayoutParams(GridLayoutParams::New().SetRowSpan(2));
  DALI_TEST_EQUALS(child.GetLayoutParams<GridLayoutParams>().GetRowSpan(), 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutGetRowSpanP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  View child = View::New();
  layout.AddView(child);
  child.SetLayoutParams(GridLayoutParams::New());
  DALI_TEST_EQUALS(child.GetLayoutParams<GridLayoutParams>().GetRowSpan(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutSetColumnSpanP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  View child = View::New();
  layout.AddView(child);
  child.SetLayoutParams(GridLayoutParams::New().SetColumnSpan(3));
  DALI_TEST_EQUALS(child.GetLayoutParams<GridLayoutParams>().GetColumnSpan(), 3u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutGetColumnSpanP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  View child = View::New();
  layout.AddView(child);
  child.SetLayoutParams(GridLayoutParams::New());
  DALI_TEST_EQUALS(child.GetLayoutParams<GridLayoutParams>().GetColumnSpan(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutRowSpacingChainingP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  GridLayout& result = layout.RowSpacing(5.0f);
  DALI_TEST_EQUALS(&result, &layout, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.GetRowSpacing(), 5.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutColumnSpacingChainingP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  GridLayout& result = layout.ColumnSpacing(7.0f);
  DALI_TEST_EQUALS(&result, &layout, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.GetColumnSpacing(), 7.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutRowsChainingP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  std::vector<GridLength> rows;
  rows.push_back(GridLength::Absolute(20.0f));
  GridLayout& result = layout.Rows(rows);
  DALI_TEST_EQUALS(&result, &layout, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.GetRowCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutColumnsChainingP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  std::vector<GridLength> cols;
  cols.push_back(GridLength::Star(1.0f));
  GridLayout& result = layout.Columns(cols);
  DALI_TEST_EQUALS(&result, &layout, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.GetColumnCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutEmptyRowDefinitionsP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  std::vector<GridLength> rows = layout.GetRowDefinitions();
  DALI_TEST_EQUALS(rows.size(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutEmptyColumnDefinitionsP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  std::vector<GridLength> cols = layout.GetColumnDefinitions();
  DALI_TEST_EQUALS(cols.size(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutSetEmptyRowsP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  layout.AddRowDefinition(GridLength::Absolute(10.0f));
  layout.SetRowDefinitions(std::vector<GridLength>());
  DALI_TEST_EQUALS(layout.GetRowCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutSetEmptyColumnsP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  layout.AddColumnDefinition(GridLength::Star(1.0f));
  layout.SetColumnDefinitions(std::vector<GridLength>());
  DALI_TEST_EQUALS(layout.GetColumnCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutMeasureArrangeP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  layout.AddRowDefinition(GridLength::Absolute(50.0f));
  layout.AddRowDefinition(GridLength::Absolute(50.0f));
  layout.AddColumnDefinition(GridLength::Absolute(80.0f));
  layout.AddColumnDefinition(GridLength::Absolute(80.0f));
  View c1 = View::New();
  layout.AddView(c1);
  c1.SetLayoutParams(GridLayoutParams::New().SetRow(0).SetColumn(0));
  View c2 = View::New();
  layout.AddView(c2);
  c2.SetLayoutParams(GridLayoutParams::New().SetRow(0).SetColumn(1));
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(120.0f);
  MeasuredSize m = layout.Measure(200.0f, 120.0f);
  MeasuredSize a = layout.Arrange(LayoutRect(0, 0, 200, 120));
  DALI_TEST_EQUALS(m.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m.GetHeight(), 120.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetHeight(), 120.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutStarDefinitionsP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  layout.AddRowDefinition(GridLength::Star(1.0f));
  layout.AddRowDefinition(GridLength::Star(1.0f));
  layout.AddColumnDefinition(GridLength::Star(1.0f));
  layout.AddColumnDefinition(GridLength::Star(1.0f));
  View c1 = View::New();
  layout.AddView(c1);
  c1.SetLayoutParams(GridLayoutParams::New().SetRow(0).SetColumn(0));
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  MeasuredSize m = layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 100));
  DALI_TEST_EQUALS(m.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m.GetHeight(), 100.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutAutoDefinitionsP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  layout.AddRowDefinition(GridLength::Auto());
  layout.AddColumnDefinition(GridLength::Auto());
  View c1 = View::New();
  c1.SetRequestedWidth(60.0f);
  c1.SetRequestedHeight(40.0f);
  layout.AddView(c1);
  c1.SetLayoutParams(GridLayoutParams::New().SetRow(0).SetColumn(0));
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(120.0f);
  MeasuredSize m = layout.Measure(200.0f, 120.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 120));
  DALI_TEST_CHECK(m.GetWidth() >= 60.0f);
  DALI_TEST_CHECK(m.GetHeight() >= 40.0f);
  END_TEST;
}

int UtcDaliGridLayoutRowColumnSpanP(void)
{
  TestApplication application;
  GridLayout layout = GridLayout::New();
  layout.AddRowDefinition(GridLength::Absolute(40.0f));
  layout.AddRowDefinition(GridLength::Absolute(40.0f));
  layout.AddColumnDefinition(GridLength::Absolute(60.0f));
  layout.AddColumnDefinition(GridLength::Absolute(60.0f));
  View c1 = View::New();
  layout.AddView(c1);
  c1.SetLayoutParams(GridLayoutParams::New().SetRow(0).SetColumn(0).SetRowSpan(2).SetColumnSpan(1));
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 100));
  DALI_TEST_EQUALS(c1.GetLayoutParams<GridLayoutParams>().GetRowSpan(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(c1.GetLayoutParams<GridLayoutParams>().GetColumnSpan(), 1u, TEST_LOCATION);
  END_TEST;
}
