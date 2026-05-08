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
 * View sample: MATCH_PARENT child under WRAP_CONTENT parent.
 *
 * Demonstrates the MATCH_PARENT "follower" semantics:
 *   - Root View uses WRAP_CONTENT for both width and height.
 *   - Child 1 has a fixed size of 300x300 with margin 50.
 *   - Child 2 uses MATCH_PARENT for both axes with margin 100.
 *
 * Expected result:
 *   The root wraps to 400x400 (driven by child 1: 300 + 50*2 margin).
 *   The MATCH_PARENT child fills 400x400 minus its own margin (200x200),
 *   sitting behind the fixed child (both are positioned at the same origin
 *   since a plain View overlaps its children).
 *
 * Press Escape or Back to quit.
 */
class ViewMatchParentWrapContentController : public ConnectionTracker
{
public:
  ViewMatchParentWrapContentController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &ViewMatchParentWrapContentController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Root: WRAP_CONTENT on both axes — size is determined by children.
    View root = View::New();
    root.SetBackgroundColor(Color::GAINSBORO);
    root.SetRequestedWidth(WRAP_CONTENT);
    root.SetRequestedHeight(WRAP_CONTENT);

    // Child 1: Fixed 300x300 — drives the WRAP_CONTENT parent's size.
    View child1 = View::New();
    child1.SetBackgroundColor(Color::RED);
    child1.SetRequestedWidth(300.0f);
    child1.SetRequestedHeight(300.0f);
    child1.SetMargin(Extents(50, 50, 50, 50));
    root.Add(child1);

    // Child 2: MATCH_PARENT — follows the parent size (which is driven by child 1).
    View child2 = View::New();
    child2.SetBackgroundColor(Color::GREEN);
    child2.SetRequestedWidth(MATCH_PARENT);
    child2.SetRequestedHeight(MATCH_PARENT);
    child2.SetMargin(Extents(100, 100, 100, 100));
    root.Add(child2);

    View grandChild1 = View::New();
    grandChild1.SetBackgroundColor(Color::BLUE);
    grandChild1.SetRequestedWidth(MATCH_PARENT);
    grandChild1.SetRequestedHeight(MATCH_PARENT);
    grandChild1.SetMargin(Extents(50, 50, 50, 50));
    child2.Add(grandChild1);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &ViewMatchParentWrapContentController::OnKeyEvent);
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
  ViewMatchParentWrapContentController controller(application);
  application.MainLoop();
  return 0;
}
