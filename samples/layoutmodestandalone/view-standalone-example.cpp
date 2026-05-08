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
 * View sample (LayoutMode::STANDALONE): based on view-margin-example.
 *
 * Same structure as view-margin-example, but the first child (Red) is set to
 * LayoutMode::STANDALONE with RequestedWidth/Height = (100, 100) and
 * SetRequestedPositionX/Y = (300, 300). It is excluded from the parent View's
 * WRAP_CONTENT accumulation and placed at (300, 300) in the parent's
 * coordinate space.
 *
 * Press Escape or Back to quit.
 */
class ViewStandaloneController : public ConnectionTracker
{
public:
  ViewStandaloneController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &ViewStandaloneController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Root: MATCH_PARENT
    View root = View::New();
    root.SetBackgroundColor(Color::GRAY);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);

    // Child 1: Red, width=MATCH_PARENT, height=200, margin=50
    View child1 = View::New();
    child1.SetBackgroundColor(Color::RED);
    child1.SetRequestedWidth(MATCH_PARENT);
    child1.SetRequestedHeight(200.0f);
    child1.SetMargin(Extents(50, 50, 50, 50));
    root.Add(child1);

    // Child 2: Green, width=WRAP_CONTENT, height=200, pos=(0,300), margin=50
    View child2 = View::New();
    child2.SetBackgroundColor(Color::GREEN);
    child2.SetRequestedWidth(WRAP_CONTENT);
    child2.SetRequestedHeight(200.0f);
    child2.SetRequestedPositionY(300.0f);
    child2.SetMargin(Extents(50, 50, 50, 50));

    // Child 2's child: Yellow, 100x100
    View grandchild = View::New();
    grandchild.SetBackgroundColor(Color::YELLOW);
    grandchild.SetRequestedWidth(100.0f);
    grandchild.SetRequestedHeight(100.0f);
    child2.Add(grandchild);

    root.Add(child2);

    // Child 3: Blue, Standalone (100x100 at (300, 300))
    View child3 = View::New();
    child3.SetBackgroundColor(Color::BLUE);
    child3.SetRequestedWidth(100.0f);
    child3.SetRequestedHeight(100.0f);
    child3.SetRequestedPositionX(300.0f);
    child3.SetRequestedPositionY(300.0f);
    child3.SetLayoutMode(LayoutMode::STANDALONE);
    root.Add(child3);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &ViewStandaloneController::OnKeyEvent);
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
  ViewStandaloneController controller(application);
  application.MainLoop();
  return 0;
}
