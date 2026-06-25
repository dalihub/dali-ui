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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
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
  UiTestApplication application;
  GridLayout        layout;
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliGridLayoutNewP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  DALI_TEST_CHECK(layout);
  END_TEST;
}

int UtcDaliGridLayoutCopyConstructorP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  GridLayout        copy(layout);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(layout == copy);
  END_TEST;
}

int UtcDaliGridLayoutMoveConstructor(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  GridLayout        moved  = std::move(layout);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliGridLayoutAssignmentOperatorP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  GridLayout        copy;
  copy = layout;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(layout == copy);
  END_TEST;
}

int UtcDaliGridLayoutDownCastP(void)
{
  UiTestApplication application;
  GridLayout        layout  = GridLayout::New();
  GridLayout        layout2 = GridLayout::DownCast(layout);
  DALI_TEST_CHECK(layout2);
  DALI_TEST_CHECK(layout == layout2);
  END_TEST;
}

int UtcDaliGridLayoutDownCastN(void)
{
  UiTestApplication application;
  BaseHandle        unInitialized;
  GridLayout        layout = GridLayout::DownCast(unInitialized);
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliGridLayoutAddRowDefinitionP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  layout.AddRowDefinition(GridLength::Absolute(50.0f));
  layout.AddRowDefinition(GridLength::Star(2.0f));
  DALI_TEST_EQUALS(layout.GetRowCount(), 2u, TEST_LOCATION);
  Dali::Vector<GridLength> rows = layout.GetRowDefinitions();
  DALI_TEST_EQUALS(rows.Size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(rows[0].GetType(), GridLengthType::ABSOLUTE, TEST_LOCATION);
  DALI_TEST_EQUALS(rows[0].GetValue(), 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(rows[1].GetType(), GridLengthType::STAR, TEST_LOCATION);
  DALI_TEST_EQUALS(rows[1].GetValue(), 2.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutAddColumnDefinitionP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  layout.AddColumnDefinition(GridLength::Auto());
  layout.AddColumnDefinition(GridLength::Star(1.0f));
  DALI_TEST_EQUALS(layout.GetColumnCount(), 2u, TEST_LOCATION);
  Dali::Vector<GridLength> cols = layout.GetColumnDefinitions();
  DALI_TEST_EQUALS(cols.Size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(cols[0].GetType(), GridLengthType::AUTO, TEST_LOCATION);
  DALI_TEST_EQUALS(cols[1].GetType(), GridLengthType::STAR, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutSetRowDefinitionsP(void)
{
  UiTestApplication        application;
  GridLayout               layout = GridLayout::New();
  Dali::Vector<GridLength> rows;
  rows.PushBack(GridLength::Absolute(100.0f));
  rows.PushBack(GridLength::Star(1.0f));
  layout.SetRowDefinitions(rows);
  DALI_TEST_EQUALS(layout.GetRowCount(), 2u, TEST_LOCATION);
  Dali::Vector<GridLength> got = layout.GetRowDefinitions();
  DALI_TEST_EQUALS(got[0].GetValue(), 100.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutSetColumnDefinitionsP(void)
{
  UiTestApplication        application;
  GridLayout               layout = GridLayout::New();
  Dali::Vector<GridLength> cols;
  cols.PushBack(GridLength::Star(1.0f));
  layout.SetColumnDefinitions(cols);
  DALI_TEST_EQUALS(layout.GetColumnCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutGetRowDefinitionsP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  layout.AddRowDefinition(GridLength::Absolute(30.0f));
  Dali::Vector<GridLength> rows = layout.GetRowDefinitions();
  DALI_TEST_EQUALS(rows.Size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(rows[0].GetValue(), 30.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutGetColumnDefinitionsP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  layout.AddColumnDefinition(GridLength::Star(2.0f));
  Dali::Vector<GridLength> cols = layout.GetColumnDefinitions();
  DALI_TEST_EQUALS(cols.Size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(cols[0].GetValue(), 2.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutGetRowCountP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  DALI_TEST_EQUALS(layout.GetRowCount(), 0u, TEST_LOCATION);
  layout.AddRowDefinition(GridLength::Absolute(10.0f));
  DALI_TEST_EQUALS(layout.GetRowCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutGetColumnCountP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  DALI_TEST_EQUALS(layout.GetColumnCount(), 0u, TEST_LOCATION);
  layout.AddColumnDefinition(GridLength::Auto());
  DALI_TEST_EQUALS(layout.GetColumnCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutClearRowDefinitionsP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  layout.AddRowDefinition(GridLength::Absolute(10.0f));
  layout.ClearRowDefinitions();
  DALI_TEST_EQUALS(layout.GetRowCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutClearColumnDefinitionsP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  layout.AddColumnDefinition(GridLength::Star(1.0f));
  layout.ClearColumnDefinitions();
  DALI_TEST_EQUALS(layout.GetColumnCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutSetRowSpacingP(void)
{
  UiTestApplication application;
  GridLayout        layout  = GridLayout::New();
  const float       spacing = 8.0f;
  layout.SetRowSpacing(spacing);
  DALI_TEST_EQUALS(layout.GetRowSpacing(), spacing, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutGetRowSpacingP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  DALI_TEST_EQUALS(layout.GetRowSpacing(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutSetColumnSpacingP(void)
{
  UiTestApplication application;
  GridLayout        layout  = GridLayout::New();
  const float       spacing = 12.0f;
  layout.SetColumnSpacing(spacing);
  DALI_TEST_EQUALS(layout.GetColumnSpacing(), spacing, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutGetColumnSpacingP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  DALI_TEST_EQUALS(layout.GetColumnSpacing(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutSetRowP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  View              child  = View::New();
  layout.Add(child);
  child.SetLayoutParams(GridLayoutParams::New().SetRow(2));
  DALI_TEST_EQUALS(child.GetLayoutParams<GridLayoutParams>().GetRow(), 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutGetRowP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  View              child  = View::New();
  layout.Add(child);
  child.SetLayoutParams(GridLayoutParams::New());
  DALI_TEST_EQUALS(child.GetLayoutParams<GridLayoutParams>().GetRow(), 0u, TEST_LOCATION);
  child.SetLayoutParams(GridLayoutParams::New().SetRow(1));
  DALI_TEST_EQUALS(child.GetLayoutParams<GridLayoutParams>().GetRow(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutSetColumnP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  View              child  = View::New();
  layout.Add(child);
  child.SetLayoutParams(GridLayoutParams::New().SetColumn(3));
  DALI_TEST_EQUALS(child.GetLayoutParams<GridLayoutParams>().GetColumn(), 3u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutGetColumnP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  View              child  = View::New();
  layout.Add(child);
  child.SetLayoutParams(GridLayoutParams::New());
  DALI_TEST_EQUALS(child.GetLayoutParams<GridLayoutParams>().GetColumn(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutSetRowSpanP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  View              child  = View::New();
  layout.Add(child);
  child.SetLayoutParams(GridLayoutParams::New().SetRowSpan(2));
  DALI_TEST_EQUALS(child.GetLayoutParams<GridLayoutParams>().GetRowSpan(), 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutGetRowSpanP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  View              child  = View::New();
  layout.Add(child);
  child.SetLayoutParams(GridLayoutParams::New());
  DALI_TEST_EQUALS(child.GetLayoutParams<GridLayoutParams>().GetRowSpan(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutSetColumnSpanP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  View              child  = View::New();
  layout.Add(child);
  child.SetLayoutParams(GridLayoutParams::New().SetColumnSpan(3));
  DALI_TEST_EQUALS(child.GetLayoutParams<GridLayoutParams>().GetColumnSpan(), 3u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutGetColumnSpanP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  View              child  = View::New();
  layout.Add(child);
  child.SetLayoutParams(GridLayoutParams::New());
  DALI_TEST_EQUALS(child.GetLayoutParams<GridLayoutParams>().GetColumnSpan(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutSpanZeroClampedP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  View              child  = View::New();
  layout.Add(child);
  child.SetLayoutParams(GridLayoutParams::New().SetRowSpan(0).SetColumnSpan(0));
  // Span has a minimum of 1; a 0 input must be clamped so the child keeps a cell.
  DALI_TEST_EQUALS(child.GetLayoutParams<GridLayoutParams>().GetRowSpan(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetLayoutParams<GridLayoutParams>().GetColumnSpan(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutRowSpacingSetterP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  layout.SetRowSpacing(5.0f);
  DALI_TEST_EQUALS(layout.GetRowSpacing(), 5.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutColumnSpacingSetterP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  layout.SetColumnSpacing(7.0f);
  DALI_TEST_EQUALS(layout.GetColumnSpacing(), 7.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutRowsSetterP(void)
{
  UiTestApplication        application;
  GridLayout               layout = GridLayout::New();
  Dali::Vector<GridLength> rows;
  rows.PushBack(GridLength::Absolute(20.0f));
  layout.SetRowDefinitions(rows);
  DALI_TEST_EQUALS(layout.GetRowCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutColumnsSetterP(void)
{
  UiTestApplication        application;
  GridLayout               layout = GridLayout::New();
  Dali::Vector<GridLength> cols;
  cols.PushBack(GridLength::Star(1.0f));
  layout.SetColumnDefinitions(cols);
  DALI_TEST_EQUALS(layout.GetColumnCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutEmptyRowDefinitionsP(void)
{
  UiTestApplication        application;
  GridLayout               layout = GridLayout::New();
  Dali::Vector<GridLength> rows   = layout.GetRowDefinitions();
  DALI_TEST_EQUALS(rows.Size(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutEmptyColumnDefinitionsP(void)
{
  UiTestApplication        application;
  GridLayout               layout = GridLayout::New();
  Dali::Vector<GridLength> cols   = layout.GetColumnDefinitions();
  DALI_TEST_EQUALS(cols.Size(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutSetEmptyRowsP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  layout.AddRowDefinition(GridLength::Absolute(10.0f));
  layout.SetRowDefinitions(Dali::Vector<GridLength>());
  DALI_TEST_EQUALS(layout.GetRowCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutSetEmptyColumnsP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  layout.AddColumnDefinition(GridLength::Star(1.0f));
  layout.SetColumnDefinitions(Dali::Vector<GridLength>());
  DALI_TEST_EQUALS(layout.GetColumnCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutMeasureArrangeP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  layout.AddRowDefinition(GridLength::Absolute(50.0f));
  layout.AddRowDefinition(GridLength::Absolute(50.0f));
  layout.AddColumnDefinition(GridLength::Absolute(80.0f));
  layout.AddColumnDefinition(GridLength::Absolute(80.0f));
  View c1 = View::New();
  layout.Add(c1);
  c1.SetLayoutParams(GridLayoutParams::New().SetRow(0).SetColumn(0));
  View c2 = View::New();
  layout.Add(c2);
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
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  layout.AddRowDefinition(GridLength::Star(1.0f));
  layout.AddRowDefinition(GridLength::Star(1.0f));
  layout.AddColumnDefinition(GridLength::Star(1.0f));
  layout.AddColumnDefinition(GridLength::Star(1.0f));
  View c1 = View::New();
  layout.Add(c1);
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
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  layout.AddRowDefinition(GridLength::Auto());
  layout.AddColumnDefinition(GridLength::Auto());
  View c1 = View::New();
  c1.SetRequestedWidth(60.0f);
  c1.SetRequestedHeight(40.0f);
  layout.Add(c1);
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
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  layout.AddRowDefinition(GridLength::Absolute(40.0f));
  layout.AddRowDefinition(GridLength::Absolute(40.0f));
  layout.AddColumnDefinition(GridLength::Absolute(60.0f));
  layout.AddColumnDefinition(GridLength::Absolute(60.0f));
  View c1 = View::New();
  layout.Add(c1);
  c1.SetLayoutParams(GridLayoutParams::New().SetRow(0).SetColumn(0).SetRowSpan(2).SetColumnSpan(1));
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  layout.Measure(200.0f, 100.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 100));
  DALI_TEST_EQUALS(c1.GetLayoutParams<GridLayoutParams>().GetRowSpan(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(c1.GetLayoutParams<GridLayoutParams>().GetColumnSpan(), 1u, TEST_LOCATION);
  END_TEST;
}

// A spanning child across an AUTO + ABSOLUTE column pair must grow
// the AUTO track only by the deficit over the ABSOLUTE track, not by the whole
// child width. Buggy code saw the ABSOLUTE track as 0 during the span pass and
// over-grew AUTO to the full child width.
int UtcDaliGridLayoutSpanMixedAutoAbsoluteP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  layout.AddRowDefinition(GridLength::Absolute(50.0f));
  layout.AddRowDefinition(GridLength::Absolute(50.0f));
  layout.AddColumnDefinition(GridLength::Auto());         // col0 AUTO
  layout.AddColumnDefinition(GridLength::Absolute(60.0f)); // col1 ABSOLUTE(60)
  layout.SetColumnSpacing(0.0f);

  // Spanning child needs 120 across both columns.
  View span = View::New();
  span.SetRequestedWidth(120.0f);
  span.SetRequestedHeight(40.0f);
  layout.Add(span);
  span.SetLayoutParams(GridLayoutParams::New().SetRow(0).SetColumn(0).SetColumnSpan(2));

  // Probe occupies the AUTO column (col0, row1), FILL -> width == AUTO track.
  View probe = View::New();
  probe.SetRequestedWidth(MATCH_PARENT);
  probe.SetRequestedHeight(MATCH_PARENT);
  layout.Add(probe);
  probe.SetLayoutParams(GridLayoutParams::New().SetRow(1).SetColumn(0));

  layout.SetRequestedWidth(MATCH_PARENT);
  layout.SetRequestedHeight(MATCH_PARENT);
  application.GetScene().Add(layout);
  layout.Measure(300.0f, 200.0f);
  layout.Arrange(LayoutRect(0, 0, 300, 200));

  // AUTO column absorbs only the deficit over ABSOLUTE: 120 - 60 = 60.
  // (A MATCH_PARENT layout view reports GetMinimum (0) as its own measured
  // width, so the load-bearing checks are the arranged track/child sizes.)
  DALI_TEST_EQUALS(probe.GetSize().width, 60.0f, TEST_LOCATION); // buggy: 120
  // Spanning child fills col0 + col1 = 60 + 60 = 120.
  DALI_TEST_EQUALS(span.GetSize().width, 120.0f, TEST_LOCATION); // buggy: 180
  END_TEST;
}

// Documented contract: STAR tracks are NOT pre-seeded in the span
// pass (their size is unknown then), so AUTO absorbs the full deficit relative
// to the STAR base of 0; STAR then fills the leftover of the definite grid.
int UtcDaliGridLayoutSpanMixedAutoStarP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  layout.AddRowDefinition(GridLength::Absolute(50.0f));
  layout.AddRowDefinition(GridLength::Absolute(50.0f));
  layout.AddColumnDefinition(GridLength::Auto());     // col0 AUTO
  layout.AddColumnDefinition(GridLength::Star(1.0f)); // col1 STAR
  layout.SetColumnSpacing(0.0f);

  View span = View::New();
  span.SetRequestedWidth(120.0f);
  span.SetRequestedHeight(40.0f);
  layout.Add(span);
  span.SetLayoutParams(GridLayoutParams::New().SetRow(0).SetColumn(0).SetColumnSpan(2));

  View probe = View::New();
  probe.SetRequestedWidth(MATCH_PARENT);
  probe.SetRequestedHeight(MATCH_PARENT);
  layout.Add(probe);
  probe.SetLayoutParams(GridLayoutParams::New().SetRow(1).SetColumn(0));

  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(200.0f);
  application.GetScene().Add(layout);
  layout.Measure(200.0f, 200.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 200));

  // STAR base is 0 at span time, so AUTO absorbs the full deficit -> 120.
  DALI_TEST_EQUALS(probe.GetSize().width, 120.0f, TEST_LOCATION);
  // Spanning child fills both columns of the 200-wide definite grid.
  DALI_TEST_EQUALS(span.GetSize().width, 200.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutStandaloneIgnoresParentPaddingP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  layout.AddRowDefinition(GridLength::Star(1.0f));
  layout.AddColumnDefinition(GridLength::Star(1.0f));
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

  DALI_TEST_EQUALS(standalone.GetSize().width, 200.0f - 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetSize().height, 150.0f - 14.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetPositionX(), 5.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetPositionY(), 7.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutStandaloneExcludedFromCellsP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  layout.AddRowDefinition(GridLength::Absolute(50.0f));
  layout.AddRowDefinition(GridLength::Absolute(50.0f));
  layout.AddColumnDefinition(GridLength::Absolute(60.0f));

  View cellChild = View::New();
  cellChild.SetLayoutParams(GridLayoutParams::New().SetRow(0).SetColumn(0));
  layout.Add(cellChild);

  View standalone = View::New();
  standalone.SetLayoutMode(LayoutMode::STANDALONE);
  standalone.SetRequestedWidth(20.0f);
  standalone.SetRequestedHeight(20.0f);
  standalone.SetRequestedPositionX(70.0f);
  standalone.SetRequestedPositionY(80.0f);
  // Even with grid params set, Standalone takes precedence and bypasses cell placement.
  standalone.SetLayoutParams(GridLayoutParams::New().SetRow(1).SetColumn(0));
  layout.Add(standalone);

  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(150.0f);
  layout.Measure(200.0f, 150.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 150));

  // Standalone child placed at requested position, not in row 1 col 0.
  DALI_TEST_EQUALS(standalone.GetPositionX(), 70.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetPositionY(), 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetSize().width, 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetSize().height, 20.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutDirectionLtrP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(150.0f);
  application.GetScene().Add(layout);
  // 1 row x 2 columns of 100 each.
  layout.AddRowDefinition(GridLength::Absolute(100.0f));
  layout.AddColumnDefinition(GridLength::Absolute(100.0f));
  layout.AddColumnDefinition(GridLength::Absolute(100.0f));

  View left = View::New();
  left.SetLayoutParams(GridLayoutParams::New().SetColumn(0));
  layout.Add(left);
  View right = View::New();
  right.SetLayoutParams(GridLayoutParams::New().SetColumn(1));
  layout.Add(right);

  layout.Measure(200.0f, 150.0f);
  layout.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 150.0f));

  DALI_TEST_EQUALS(left.GetPositionX(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(left.GetSize().width, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(left.GetSize().height, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(right.GetPositionX(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(right.GetSize().width, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(right.GetSize().height, 100.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutDirectionRtlP(void)
{
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(150.0f);
  layout.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  application.GetScene().Add(layout);
  layout.AddRowDefinition(GridLength::Absolute(100.0f));
  layout.AddColumnDefinition(GridLength::Absolute(100.0f));
  layout.AddColumnDefinition(GridLength::Absolute(100.0f));

  View left = View::New();
  left.SetLayoutParams(GridLayoutParams::New().SetColumn(0));
  layout.Add(left);
  View right = View::New();
  right.SetLayoutParams(GridLayoutParams::New().SetColumn(1));
  layout.Add(right);

  layout.Measure(200.0f, 150.0f);
  layout.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 150.0f));

  // LTR: left at 0, right at 100. Mirrored: cells swap physical x.
  DALI_TEST_EQUALS(left.GetPositionX(), 200.0f - 0.0f - 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(left.GetSize().width, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(left.GetSize().height, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(right.GetPositionX(), 200.0f - 100.0f - 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(right.GetSize().width, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(right.GetSize().height, 100.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutColumnSpanGrowsAutoTracksP(void)
{
  // A child spanning two auto columns must grow those columns so it is not
  // clipped. The auto columns have no other content, so their content floor is
  // zero and the spanning child's width is shared equally across both columns.
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  layout.AddRowDefinition(GridLength::Auto());
  layout.AddColumnDefinition(GridLength::Auto());
  layout.AddColumnDefinition(GridLength::Auto());
  application.GetScene().Add(layout);

  View wide = View::New();
  wide.SetRequestedWidth(120.0f);
  wide.SetRequestedHeight(40.0f);
  layout.Add(wide);
  wide.SetLayoutParams(GridLayoutParams::New().SetRow(0).SetColumn(0).SetColumnSpan(2));

  // Size the grid to its content (auto on both axes).
  layout.SetRequestedWidth(WRAP_CONTENT);
  layout.SetRequestedHeight(WRAP_CONTENT);
  MeasuredSize m = layout.Measure(500.0f, 500.0f);
  layout.Arrange(LayoutRect(0.0f, 0.0f, m.GetWidth(), m.GetHeight()));

  // Two auto columns must together provide at least the child's 120 width.
  DALI_TEST_CHECK(m.GetWidth() >= 120.0f);
  DALI_TEST_EQUALS(wide.GetSize().width, 120.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(wide.GetSize().height, 40.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGridLayoutRowSpanGrowsAutoTracksP(void)
{
  // Row-axis counterpart: a child spanning two auto rows grows those rows.
  UiTestApplication application;
  GridLayout        layout = GridLayout::New();
  layout.AddRowDefinition(GridLength::Auto());
  layout.AddRowDefinition(GridLength::Auto());
  layout.AddColumnDefinition(GridLength::Auto());
  application.GetScene().Add(layout);

  View tall = View::New();
  tall.SetRequestedWidth(50.0f);
  tall.SetRequestedHeight(100.0f);
  layout.Add(tall);
  tall.SetLayoutParams(GridLayoutParams::New().SetRow(0).SetColumn(0).SetRowSpan(2));

  layout.SetRequestedWidth(WRAP_CONTENT);
  layout.SetRequestedHeight(WRAP_CONTENT);
  MeasuredSize m = layout.Measure(500.0f, 500.0f);
  layout.Arrange(LayoutRect(0.0f, 0.0f, m.GetWidth(), m.GetHeight()));

  DALI_TEST_CHECK(m.GetHeight() >= 100.0f);
  DALI_TEST_EQUALS(tall.GetSize().width, 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(tall.GetSize().height, 100.0f, TEST_LOCATION);
  END_TEST;
}
