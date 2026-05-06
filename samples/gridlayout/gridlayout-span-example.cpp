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

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    GridLayout root = GridLayout::New()
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetPadding(Extents(50, 50, 50, 50))
      .SetRowSpacing(10.0f)
      .SetColumnSpacing(10.0f);

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

    window.Add(root);
    window.KeyEventSignal().Connect(this, &GridLayoutSpanController::OnKeyEvent);
  }

  void OnKeyEvent(KeyEvent event)
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
