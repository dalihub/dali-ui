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
#include <dali-ui-foundation/public-api/grid-layout-params.h>

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

  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    GridLayout root = GridLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetViewPadding(Extents(12, 12, 12, 12));
    root.SetRowSpacing(6.0f);
    root.SetColumnSpacing(6.0f);

    // Use batch API: SetRowDefinitions / SetColumnDefinitions
    root.SetRowDefinitions({GridLength::Auto(), GridLength::Star(1.0f), GridLength::Auto()});
    root.SetColumnDefinitions({GridLength::Auto(), GridLength::Star(1.0f)});

    // (0,0): Auto-width label - narrow fixed-size box simulating a label
    View label00 = View::New();
    label00.SetBackgroundColor(Color::RED);
    label00.SetRequestedWidth(80.0f);
    label00.SetRequestedHeight(40.0f);
    label00.SetLayoutParams(GridLayoutParams::New());
    root.AddView(label00);

    // (0,1): Header area in Auto row, Star column
    View header = View::New();
    header.SetBackgroundColor(Vector4(1.0f, 0.6f, 0.6f, 1.0f));
    header.SetRequestedHeight(40.0f);
    header.SetLayoutParams(GridLayoutParams::New().SetColumn(1));
    root.AddView(header);

    // (1,0): Sidebar in Auto column, Star row
    View sidebar = View::New();
    sidebar.SetBackgroundColor(Color::GREEN);
    sidebar.SetRequestedWidth(80.0f);
    sidebar.SetLayoutParams(GridLayoutParams::New().SetRow(1));
    root.AddView(sidebar);

    // (1,1): Main content area (Star row x Star column)
    View content = View::New();
    content.SetBackgroundColor(Color::BLUE);
    content.SetLayoutParams(GridLayoutParams::New().SetRow(1).SetColumn(1));
    root.AddView(content);

    // (2,0): Footer label in Auto row, Auto column
    View label20 = View::New();
    label20.SetBackgroundColor(Color::YELLOW);
    label20.SetRequestedWidth(80.0f);
    label20.SetRequestedHeight(30.0f);
    label20.SetLayoutParams(GridLayoutParams::New().SetRow(2));
    root.AddView(label20);

    // (2,1): Footer content spanning the Star column
    View footer = View::New();
    footer.SetBackgroundColor(Color::CYAN);
    footer.SetRequestedHeight(30.0f);
    footer.SetLayoutParams(GridLayoutParams::New().SetRow(2).SetColumn(1));
    root.AddView(footer);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &GridLayoutAutoController::OnKeyEvent);
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
  GridLayoutAutoController controller(application);
  application.MainLoop();
  return 0;
}
