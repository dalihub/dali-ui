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
 * GridLayout Auto sizing sample.
 *
 * Demonstrates GridLength::Auto() which sizes rows/columns to fit their
 * content, mixed with Absolute and Star sizing.
 *
 * Grid layout:
 *   Row 0: Auto    - heights determined by content (label-like row)
 *   Row 1: Star(1) - takes remaining vertical space
 *   Row 2: Auto    - heights determined by content (footer-like row)
 *
 *   Col 0: Auto    - width determined by content
 *   Col 1: Star(1) - takes remaining horizontal space
 *
 * Also demonstrates SetRowDefinitions/SetColumnDefinitions batch API.
 *
 * Press Escape or Back to quit.
 */
class GridLayoutAutoController : public ConnectionTracker
{
public:
  GridLayoutAutoController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &GridLayoutAutoController::Create);
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

    // Use batch API: SetRowDefinitions / SetColumnDefinitions
    Dali::Vector<GridLength> rowDefinition;
    rowDefinition.Reserve(3);
    rowDefinition.PushBack(GridLength::Auto());
    rowDefinition.PushBack(GridLength::Star(1.0f));
    rowDefinition.PushBack(GridLength::Auto());
    root.SetRowDefinitions(rowDefinition);

    Dali::Vector<GridLength> columnDefinition;
    columnDefinition.Reserve(2);
    columnDefinition.PushBack(GridLength::Auto());
    columnDefinition.PushBack(GridLength::Star(1.0f));
    root.SetColumnDefinitions(columnDefinition);

    // (0,0): Auto-width label - narrow fixed-size box simulating a label
    View label00 = View::New();
    label00.SetBackgroundColor(Color::RED);
    label00.SetRequestedWidth(100.0f);
    label00.SetRequestedHeight(50.0f);
    label00.SetLayoutParams(GridLayoutParams::New());
    root.Add(label00);

    // (0,1): Header area in Auto row, Star column
    View header = View::New();
    header.SetBackgroundColor(Vector4(1.0f, 0.6f, 0.6f, 1.0f));
    header.SetRequestedHeight(50.0f);
    header.SetLayoutParams(GridLayoutParams::New().SetColumn(1));
    root.Add(header);

    // (1,0): Sidebar in Auto column, Star row
    View sidebar = View::New();
    sidebar.SetBackgroundColor(Color::GREEN);
    sidebar.SetRequestedWidth(100.0f);
    sidebar.SetLayoutParams(GridLayoutParams::New().SetRow(1));
    root.Add(sidebar);

    // (1,1): Main content area (Star row x Star column)
    View content = View::New();
    content.SetBackgroundColor(Color::BLUE);
    content.SetLayoutParams(GridLayoutParams::New().SetRow(1).SetColumn(1));
    root.Add(content);

    // (2,0): Footer label in Auto row, Auto column
    View label20 = View::New();
    label20.SetBackgroundColor(Color::YELLOW);
    label20.SetRequestedWidth(100.0f);
    label20.SetRequestedHeight(50.0f);
    label20.SetLayoutParams(GridLayoutParams::New().SetRow(2));
    root.Add(label20);

    // (2,1): Footer content spanning the Star column
    View footer = View::New();
    footer.SetBackgroundColor(Color::CYAN);
    footer.SetRequestedHeight(50.0f);
    footer.SetLayoutParams(GridLayoutParams::New().SetRow(2).SetColumn(1));
    root.Add(footer);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &GridLayoutAutoController::OnKeyEvent);
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
  GridLayoutAutoController controller(application);
  application.MainLoop();
  return 0;
}
