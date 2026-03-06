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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/grid-layout.h>

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
 * Press Escape or Back to quit.
 */
class GridLayoutSpanController : public ConnectionTracker
{
public:
  GridLayoutSpanController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &GridLayoutSpanController::Create);
  }

  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    GridLayout root = GridLayout::New();
    root.SetLayoutWidth(LayoutDimension::MatchParent);
    root.SetLayoutHeight(LayoutDimension::MatchParent);
    root.SetViewPadding(Extents(15, 15, 15, 15));
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
    GridLayout::SetRow(red, 0);
    GridLayout::SetColumn(red, 0);
    GridLayout::SetColumnSpan(red, 2);
    root.AddView(red);

    // Green: row 0, column 2
    View green = View::New();
    green.SetBackgroundColor(Color::GREEN);
    GridLayout::SetRow(green, 0);
    GridLayout::SetColumn(green, 2);
    root.AddView(green);

    // Blue: row 1, column 0
    View blue = View::New();
    blue.SetBackgroundColor(Color::BLUE);
    GridLayout::SetRow(blue, 1);
    GridLayout::SetColumn(blue, 0);
    root.AddView(blue);

    // Yellow: row 1, column 1, spans 2 columns and 2 rows
    View yellow = View::New();
    yellow.SetBackgroundColor(Color::YELLOW);
    GridLayout::SetRow(yellow, 1);
    GridLayout::SetColumn(yellow, 1);
    GridLayout::SetColumnSpan(yellow, 2);
    GridLayout::SetRowSpan(yellow, 2);
    root.AddView(yellow);

    // Cyan: row 2, column 0
    View cyan = View::New();
    cyan.SetBackgroundColor(Color::CYAN);
    GridLayout::SetRow(cyan, 2);
    GridLayout::SetColumn(cyan, 0);
    root.AddView(cyan);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &GridLayoutSpanController::OnKeyEvent);
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if (event.GetState() == KeyEvent::DOWN)
    {
      if (IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application& mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  GridLayoutSpanController controller(application);
  application.MainLoop();
  return 0;
}
