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
 * View sample: padding.
 *
 * Same structure as view-margin but using padding instead of margin.
 * All 3 children have their own child view.
 *
 * 1. Red: width=MATCH_PARENT, height=200, padding=(50,50,50,50).
 *    -> Yellow grandchild: width=MATCH_PARENT, height=100.
 * 2. Green (WRAP_CONTENT): width=WRAP_CONTENT, height=200, pos=(0,300),
 *    padding=(50,50,50,50).
 *    -> Cyan grandchild: width=100, height=100 (NOT MATCH_PARENT).
 * 3. Blue: width=200, height=200, pos=(0,600), padding=(50,50,50,50).
 *    -> Magenta grandchild: width=MATCH_PARENT, height=100.
 *
 * Press Escape or Back to quit.
 */
class ViewPaddingController : public ConnectionTracker
{
public:
  ViewPaddingController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &ViewPaddingController::Create);
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

    // Child 1: Red, width=MATCH_PARENT, height=200, padding=50
    View child1 = View::New();
    child1.SetBackgroundColor(Color::RED);
    child1.SetRequestedWidth(MATCH_PARENT);
    child1.SetRequestedHeight(200.0f);
    child1.SetPadding(Extents(50, 50, 50, 50));

    // Child 1's child: Yellow, width=MATCH_PARENT, height=100
    View grandchild1 = View::New();
    grandchild1.SetBackgroundColor(Color::YELLOW);
    grandchild1.SetRequestedWidth(MATCH_PARENT);
    grandchild1.SetRequestedHeight(100.0f);
    child1.Add(grandchild1);

    root.Add(child1);

    // Child 2: Green, width=WRAP_CONTENT, height=200, pos=(0,300), padding=50
    View child2 = View::New();
    child2.SetBackgroundColor(Color::GREEN);
    child2.SetRequestedWidth(WRAP_CONTENT);
    child2.SetRequestedHeight(200.0f);
    child2.SetRequestedPositionY(200.0f);
    child2.SetPadding(Extents(50, 50, 50, 50));

    // Child 2's child: Cyan, width=100, height=100 (NOT MATCH_PARENT)
    View grandchild2 = View::New();
    grandchild2.SetBackgroundColor(Color::CYAN);
    grandchild2.SetRequestedWidth(100.0f);
    grandchild2.SetRequestedHeight(100.0f);
    child2.Add(grandchild2);

    root.Add(child2);

    // Child 3: Blue, width=200, height=200, pos=(0,600), padding=50
    View child3 = View::New();
    child3.SetBackgroundColor(Color::BLUE);
    child3.SetRequestedWidth(200.0f);
    child3.SetRequestedHeight(200.0f);
    child3.SetRequestedPositionY(400.0f);
    child3.SetPadding(Extents(50, 50, 50, 50));

    // Child 3's child: Magenta, width=MATCH_PARENT, height=100
    View grandchild3 = View::New();
    grandchild3.SetBackgroundColor(Color::MAGENTA);
    grandchild3.SetRequestedWidth(MATCH_PARENT);
    grandchild3.SetRequestedHeight(100.0f);
    child3.Add(grandchild3);

    root.Add(child3);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &ViewPaddingController::OnKeyEvent);
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
  ViewPaddingController controller(application);
  application.MainLoop();
  return 0;
}
