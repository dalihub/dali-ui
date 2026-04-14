/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 * GridLayout sample (LayoutMode::STANDALONE): based on gridlayout-example.
 *
 * Same structure as gridlayout-example, but the first child (Red, cell 0,0) is
 * set to LayoutMode::STANDALONE with RequestedWidth/Height = (100, 100) and
 * SetPositionX/Y = (300, 300). It is excluded from the grid cell placement
 * and instead drawn at (300, 300) in the parent's coordinate space, while the
 * other cells still occupy their grid positions.
 *
 * Press Escape or Back to quit.
 */
class GridLayoutStandaloneController : public ConnectionTracker
{
public:
  GridLayoutStandaloneController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &GridLayoutStandaloneController::Create);
  }

  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Root: GridLayout filling the window
    GridLayout root = GridLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Extents(50, 50, 50, 50));

    // Define 3 rows and 2 columns with absolute sizes
    root.AddRowDefinition(GridLength::Absolute(50.0f));
    root.AddRowDefinition(GridLength::Absolute(100.0f));
    root.AddRowDefinition(GridLength::Absolute(200.0f));
    root.AddColumnDefinition(GridLength::Absolute(50.0f));
    root.AddColumnDefinition(GridLength::Absolute(100.0f));

    root.SetRowSpacing(10.0f);
    root.SetColumnSpacing(10.0f);

    // Cell (0,0): Red
    View cell00 = View::New();
    cell00.SetBackgroundColor(Color::RED);
    cell00.SetLayoutParams(GridLayoutParams::New());
    root.Add(cell00);

    // Cell (0,1): Green
    View cell01 = View::New();
    cell01.SetBackgroundColor(Color::GREEN);
    cell01.SetLayoutParams(GridLayoutParams::New().SetColumn(1));
    root.Add(cell01);

    // Cell (1,0): Blue, Standalone (100x100 at (300, 300))
    View cell10 = View::New();
    cell10.SetBackgroundColor(Color::BLUE);
    cell10.SetRequestedWidth(100.0f);
    cell10.SetRequestedHeight(100.0f);
    cell10.SetPositionX(300.0f);
    cell10.SetPositionY(300.0f);
    cell10.SetLayoutMode(LayoutMode::STANDALONE);
    root.Add(cell10);

    // Cell (1,1): Yellow
    View cell11 = View::New();
    cell11.SetBackgroundColor(Color::YELLOW);
    cell11.SetLayoutParams(GridLayoutParams::New().SetRow(1).SetColumn(1));
    root.Add(cell11);

    // Cell (2,0): Cyan
    View cell20 = View::New();
    cell20.SetBackgroundColor(Color::CYAN);
    cell20.SetLayoutParams(GridLayoutParams::New().SetRow(2));
    root.Add(cell20);

    // Cell (2,1): Magenta
    View cell21 = View::New();
    cell21.SetBackgroundColor(Color::MAGENTA);
    cell21.SetLayoutParams(GridLayoutParams::New().SetRow(2).SetColumn(1));
    root.Add(cell21);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &GridLayoutStandaloneController::OnKeyEvent);
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
  GridLayoutStandaloneController controller(application);
  application.MainLoop();
  return 0;
}
