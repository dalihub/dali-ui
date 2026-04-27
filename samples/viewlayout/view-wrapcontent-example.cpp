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
 * View sample: WRAP_CONTENT sizing.
 *
 * Root View has WRAP_CONTENT size. It contains two children:
 * - Red box at (0, 0) with size (50, 50).
 * - Green box at (100, 100) with size (100, 100).
 *
 * The root should shrink-wrap to fit both children (200 x 200).
 *
 * Press Escape or Back to quit.
 */
class ViewWrapContentController : public ConnectionTracker
{
public:
  ViewWrapContentController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &ViewWrapContentController::Create);
  }

  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Root: WRAP_CONTENT View (background visible as bounding area)
    View root = View::New();
    root.SetBackgroundColor(Color::BLUE);
    root.SetRequestedWidth(WRAP_CONTENT);
    root.SetRequestedHeight(WRAP_CONTENT);

    // Child 1: Red box at (0, 0), size 50x50
    View child1 = View::New();
    child1.SetBackgroundColor(Color::RED);
    child1.SetRequestedWidth(50.0f);
    child1.SetRequestedHeight(50.0f);
    root.Add(child1);

    // Child 2: Green box at (100, 100), size 100x100
    View child2 = View::New();
    child2.SetBackgroundColor(Color::GREEN);
    child2.SetRequestedWidth(100.0f);
    child2.SetRequestedHeight(100.0f);
    child2.SetRequestedPositionX(100.0f);
    child2.SetRequestedPositionY(100.0f);
    root.Add(child2);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &ViewWrapContentController::OnKeyEvent);
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
  ViewWrapContentController controller(application);
  application.MainLoop();
  return 0;
}
