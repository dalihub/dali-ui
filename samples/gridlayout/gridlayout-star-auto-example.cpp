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
 * GridLayout star and auto sizing sample.
 *
 * Demonstrates GridLength::Star() and GridLength::Auto() sizing modes.
 *
 * - 3 rows: fixed 80px header, Star(1) flexible middle, fixed 60px footer
 * - 3 columns: fixed 100px sidebar, Star(2) wide content, Star(1) narrow panel
 *
 * Star columns distribute remaining space proportionally:
 *   Star(2) gets twice the width of Star(1).
 *
 * Press Escape or Back to quit.
 */
class GridLayoutStarAutoController : public ConnectionTracker
{
public:
  GridLayoutStarAutoController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &GridLayoutStarAutoController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    GridLayout root = GridLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Extents(50, 50, 50, 50));
    root.SetRowSpacing(10.0f);
    root.SetColumnSpacing(10.0f);

    // Rows: fixed header, flexible middle, fixed footer
    root.AddRowDefinition(GridLength::Absolute(100.0f));
    root.AddRowDefinition(GridLength::Star(1.0f));
    root.AddRowDefinition(GridLength::Absolute(50.0f));

    // Columns: fixed sidebar, Star(2) content, Star(1) panel
    root.AddColumnDefinition(GridLength::Absolute(100.0f));
    root.AddColumnDefinition(GridLength::Star(2.0f));
    root.AddColumnDefinition(GridLength::Star(1.0f));

    // Header: spans all 3 columns
    View header = View::New();
    header.SetBackgroundColor(Color::RED);
    header.SetLayoutParams(GridLayoutParams::New().SetColumnSpan(3));
    root.Add(header);

    // Sidebar: row 1, column 0
    View sidebar = View::New();
    sidebar.SetBackgroundColor(Color::GREEN);
    sidebar.SetLayoutParams(GridLayoutParams::New().SetRow(1));
    root.Add(sidebar);

    // Content: row 1, column 1 (Star(2) - wider)
    View content = View::New();
    content.SetBackgroundColor(Color::BLUE);
    content.SetLayoutParams(GridLayoutParams::New().SetRow(1).SetColumn(1));
    root.Add(content);

    // Panel: row 1, column 2 (Star(1) - narrower)
    View panel = View::New();
    panel.SetBackgroundColor(Color::YELLOW);
    panel.SetLayoutParams(GridLayoutParams::New().SetRow(1).SetColumn(2));
    root.Add(panel);

    // Footer: spans all 3 columns
    View footer = View::New();
    footer.SetBackgroundColor(Color::CYAN);
    footer.SetLayoutParams(GridLayoutParams::New().SetRow(2).SetColumnSpan(3));
    root.Add(footer);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &GridLayoutStarAutoController::OnKeyEvent);
  }

  void OnKeyEvent(Window window, KeyEvent event)
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
  GridLayoutStarAutoController controller(application);
  application.MainLoop();
  return 0;
}
