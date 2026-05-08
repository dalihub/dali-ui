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
 * GridLayout sample: WRAP_CONTENT with minSize and STAR column.
 *
 * Demonstrates that a STAR column correctly receives the extra space
 * created by a minimum size on a WRAP_CONTENT parent.
 *
 *   - Root is a GridLayout with WRAP_CONTENT width, minWidth=400,
 *     and fixed height 200.
 *   - Columns: [AUTO, 1*STAR]
 *   - Row: [1*STAR]
 *   - Child 1 (col 0) has fixed width 200 — drives the AUTO column.
 *   - Child 2 (col 1, GridLayout) sits in the STAR column.
 *     It contains two grandchildren in 1*STAR columns, each getting
 *     half of the available 200px.
 *
 * Expected result:
 *   The grid wraps to 400 (driven by minWidth, since AUTO content
 *   is only 200). The STAR column receives 400 - 200 = 200.
 *   Inside child 2, each grandchild gets 100px via nested STAR.
 *
 * Press Escape or Back to quit.
 */
class GridLayoutWrapContentStarController : public ConnectionTracker
{
public:
  GridLayoutWrapContentStarController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &GridLayoutWrapContentStarController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Columns: AUTO, 1*STAR.  Row: 1*STAR.
    GridLayout root = GridLayout::New()
      .SetBackgroundColor(Color::GAINSBORO)
      .SetMinimumWidth(400.0f)
      .SetRequestedHeight(200.0f);
    root.AddColumnDefinition(GridLength::Auto());
    root.AddColumnDefinition(GridLength::Star(1.0f));
    root.AddRowDefinition(GridLength::Star(1.0f));

    // Child 1: col 0 (AUTO) — fixed 200 wide, drives the AUTO column size.
    View child1 = View::New();
    child1.SetBackgroundColor(Color::RED);
    child1.SetRequestedWidth(200.0f);
    child1.SetLayoutParams(GridLayoutParams::New().SetRow(0).SetColumn(0));
    root.Add(child1);

    // Child 2: col 1 (STAR) — GridLayout that fills the remaining 200px (400 - 200).
    // Contains two grandchildren in 1*STAR columns, each getting 100px.
    GridLayout child2 = GridLayout::New()
      .SetBackgroundColor(Color::GREEN)
      .SetLayoutParams(GridLayoutParams::New().SetRow(0).SetColumn(1));
    child2.AddColumnDefinition(GridLength::Star(1.0f));
    child2.AddColumnDefinition(GridLength::Star(1.0f));
    child2.AddRowDefinition(GridLength::Star(1.0f));
    root.Add(child2);

    View grandChild1 = View::New();
    grandChild1.SetBackgroundColor(Color::BLUE);
    grandChild1.SetLayoutParams(GridLayoutParams::New().SetRow(0).SetColumn(0));
    child2.Add(grandChild1);

    View grandChild2 = View::New();
    grandChild2.SetBackgroundColor(Color::YELLOW);
    grandChild2.SetLayoutParams(GridLayoutParams::New().SetRow(0).SetColumn(1));
    child2.Add(grandChild2);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &GridLayoutWrapContentStarController::OnKeyEvent);
  }

  void OnKeyEvent(Window window, KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
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
  GridLayoutWrapContentStarController controller(application);
  application.MainLoop();
  return 0;
}
