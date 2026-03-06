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
 * GridLayout sample: margin and padding.
 *
 * All margin and padding values use a uniform 25px for easy visual verification.
 *
 * 1. Padding: root GridLayout has 25px padding (gray background visible as border).
 * 2. Margin: cells alternate between no margin and 25px uniform margin.
 *    - (0,0) Red: no margin (fills cell completely).
 *    - (0,1) Green: 25px margin all sides.
 *    - (1,0) Blue: 25px margin all sides.
 *    - (1,1) Yellow: no margin.
 *    - (2,0~1) Cyan spanning 2 columns: 25px margin all sides.
 * 3. Spacing is set to 0 so margin effects are clearly visible.
 *
 * Press Escape or Back to quit.
 */
class GridLayoutMarginPaddingController : public ConnectionTracker
{
public:
  GridLayoutMarginPaddingController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &GridLayoutMarginPaddingController::Create);
  }

  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Root: GridLayout with padding
    GridLayout root = GridLayout::New();
    root.SetBackgroundColor(Color::GRAY);
    root.SetLayoutWidth(LayoutDimension::MatchParent);
    root.SetLayoutHeight(LayoutDimension::MatchParent);
    root.SetViewPadding(Extents(25, 25, 25, 25)); // start, end, top, bottom

    // 3 rows x 2 columns, no spacing (margin only)
    root.AddRowDefinition(GridLength::Absolute(120.0f));
    root.AddRowDefinition(GridLength::Absolute(120.0f));
    root.AddRowDefinition(GridLength::Absolute(120.0f));
    root.AddColumnDefinition(GridLength::Star(1.0f));
    root.AddColumnDefinition(GridLength::Star(1.0f));

    root.SetRowSpacing(0.0f);
    root.SetColumnSpacing(0.0f);

    // Cell (0,0): Red - no margin (flush with grid padding edge)
    View cell00 = View::New();
    cell00.SetBackgroundColor(Color::RED);
    GridLayout::SetRow(cell00, 0);
    GridLayout::SetColumn(cell00, 0);
    root.AddView(cell00);

    // Cell (0,1): Green - 25px margin all sides
    View cell01 = View::New();
    cell01.SetBackgroundColor(Color::GREEN);
    cell01.SetViewMargin(Extents(25, 25, 25, 25));
    GridLayout::SetRow(cell01, 0);
    GridLayout::SetColumn(cell01, 1);
    root.AddView(cell01);

    // Cell (1,0): Blue - 25px margin all sides
    View cell10 = View::New();
    cell10.SetBackgroundColor(Color::BLUE);
    cell10.SetViewMargin(Extents(25, 25, 25, 25));
    GridLayout::SetRow(cell10, 1);
    GridLayout::SetColumn(cell10, 0);
    root.AddView(cell10);

    // Cell (1,1): Yellow - no margin (fills cell completely)
    View cell11 = View::New();
    cell11.SetBackgroundColor(Color::YELLOW);
    GridLayout::SetRow(cell11, 1);
    GridLayout::SetColumn(cell11, 1);
    root.AddView(cell11);

    // Cell (2,0~1): Cyan - spanning 2 columns with 25px margin
    View cell20 = View::New();
    cell20.SetBackgroundColor(Color::CYAN);
    cell20.SetViewMargin(Extents(25, 25, 25, 25));
    GridLayout::SetRow(cell20, 2);
    GridLayout::SetColumn(cell20, 0);
    GridLayout::SetColumnSpan(cell20, 2);
    root.AddView(cell20);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &GridLayoutMarginPaddingController::OnKeyEvent);
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
  GridLayoutMarginPaddingController controller(application);
  application.MainLoop();
  return 0;
}
