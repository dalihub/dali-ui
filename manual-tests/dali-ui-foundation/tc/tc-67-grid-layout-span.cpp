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
 */

#include "manual-test-case.h"

#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * GridLayout row and column span sample.
 *
 * Demonstrates RowSpan and ColumnSpan to make cells span multiple
 * rows or columns in a 3x3 grid.
 *
 * Layout (3 rows x 3 columns, all Star(1)):
 *   +--------+--------+--------+
 *   | Red (spans 2 cols)| Green |
 *   +--------+--------+--------+
 *   | Blue   | Yellow (spans    |
 *   |        |  2 cols, 2 rows) |
 *   +--------+                  |
 *   | Cyan   |                  |
 *   +--------+--------+--------+
 *
 */
class TcGridLayoutSpan : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "67. GridLayout: Span";
  }

  Dali::String GetDescription() const override
  {
    return "Verify row-span and column-span placement";
  }

  void OnEnter(View contentArea) override
  {

    GridLayout root = GridLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Insets(50.0f, 50.0f, 50.0f, 50.0f));
    root.SetRowSpacing(10.0f);
    root.SetColumnSpacing(10.0f);

    // 3 rows x 3 columns, all proportional
    root.AddRowDefinition(GridLength::Star(1.0f));
    root.AddRowDefinition(GridLength::Star(1.0f));
    root.AddRowDefinition(GridLength::Star(1.0f));
    root.AddColumnDefinition(GridLength::Star(1.0f));
    root.AddColumnDefinition(GridLength::Star(1.0f));
    root.AddColumnDefinition(GridLength::Star(1.0f));

    // Red: row 0, column 0, spans 2 columns
    View red = View::New();
    red.SetBackgroundColor(Color::RED);
    red.SetLayoutParams(GridLayoutParams::New().SetColumnSpan(2));
    root.Add(red);

    // Green: row 0, column 2
    View green = View::New();
    green.SetBackgroundColor(Color::GREEN);
    green.SetLayoutParams(GridLayoutParams::New().SetColumn(2));
    root.Add(green);

    // Blue: row 1, column 0
    View blue = View::New();
    blue.SetBackgroundColor(Color::BLUE);
    blue.SetLayoutParams(GridLayoutParams::New().SetRow(1));
    root.Add(blue);

    // Yellow: row 1, column 1, spans 2 columns and 2 rows
    View yellow = View::New();
    yellow.SetBackgroundColor(Color::YELLOW);
    yellow.SetLayoutParams(GridLayoutParams::New().SetRow(1).SetColumn(1).SetColumnSpan(2).SetRowSpan(2));
    root.Add(yellow);

    // Cyan: row 2, column 0
    View cyan = View::New();
    cyan.SetBackgroundColor(Color::CYAN);
    cyan.SetLayoutParams(GridLayoutParams::New().SetRow(2));
    root.Add(cyan);

    contentArea.Add(root);
  }

  void OnExit() override
  {
    DisconnectAll();
  }

};

REGISTER_MANUAL_TEST(TcGridLayoutSpan)
