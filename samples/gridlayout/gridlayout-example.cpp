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
 * GridLayout sample: basic 2-column, 3-row grid with fixed sizes.
 *
 * - 3 rows (100px, 120px, 80px) x 2 columns (200px, 160px)
 * - 6 colored cells placed at explicit row/column positions
 * - Row and column spacing of 10px
 *
 * Press Escape or Back to quit.
 */
class GridLayoutController : public ConnectionTracker
{
public:
  GridLayoutController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &GridLayoutController::Create);
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

    // Cell (1,0): Blue
    View cell10 = View::New();
    cell10.SetBackgroundColor(Color::BLUE);
    cell10.SetLayoutParams(GridLayoutParams::New().SetRow(1));
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
    window.KeyEventSignal().Connect(this, &GridLayoutController::OnKeyEvent);
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
  GridLayoutController controller(application);
  application.MainLoop();
  return 0;
}
