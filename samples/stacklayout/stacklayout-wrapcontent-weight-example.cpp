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
 * StackLayout sample: MATCH_PARENT children in a horizontal stack.
 *
 * Demonstrates MATCH_PARENT behaviour in a StackLayout:
 *   - Root is a Horizontal StackLayout with minimum width 400 and
 *     requested height 200.
 *   - Child 1 has a fixed width of 200 and MATCH_PARENT height
 *     — it fills the parent's cross-axis (200px).
 *   - Child 2 has weight=1 and MATCH_PARENT height.
 *     On the main axis (width) it fills the remaining space via weight
 *     (400 - 200 = 200). On the cross axis (height) it fills the
 *     parent height (200).
 *
 * Press Escape or Back to quit.
 */
class StackLayoutMatchParentController : public ConnectionTracker
{
public:
  StackLayoutMatchParentController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &StackLayoutMatchParentController::Create);
  }

  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Root: Horizontal StackLayout, minWidth=400, height=200.
    StackLayout root = StackLayout::New(StackOrientation::HORIZONTAL);
    root.SetBackgroundColor(Color::GAINSBORO);
    root.SetMinimumWidth(400.0f);
    root.SetRequestedHeight(200.0f);

    // Child 1: fixed width 200, MATCH_PARENT height (fills cross-axis = 200).
    View child1 = View::New();
    child1.SetBackgroundColor(Color::RED);
    child1.SetRequestedWidth(200.0f);
    child1.SetRequestedHeight(MATCH_PARENT);
    root.Add(child1);

    // Child 2: MATCH_PARENT width and height.
    // Main axis (width): fills the full available width.
    // Cross axis (height): fills the parent height (200).
    StackLayout child2 = StackLayout::New(StackOrientation::HORIZONTAL);
    child2.SetBackgroundColor(Color::GREEN);
    child2.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    child2.SetRequestedHeight(MATCH_PARENT);
    root.Add(child2);

    View grandChild1 = View::New();
    grandChild1.SetBackgroundColor(Color::BLUE);
    grandChild1.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    grandChild1.SetRequestedHeight(MATCH_PARENT);
    child2.Add(grandChild1);

    View grandChild2 = View::New();
    grandChild2.SetBackgroundColor(Color::YELLOW);
    grandChild2.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    grandChild2.SetRequestedHeight(MATCH_PARENT);
    child2.Add(grandChild2);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &StackLayoutMatchParentController::OnKeyEvent);
  }

  void OnKeyEvent(const KeyEvent& event)
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
  StackLayoutMatchParentController controller(application);
  application.MainLoop();
  return 0;
}
