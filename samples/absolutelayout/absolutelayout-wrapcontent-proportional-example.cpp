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
 * AbsoluteLayout sample: WRAP_CONTENT with minSize and proportional child.
 *
 * Demonstrates that a proportional-size child correctly resolves
 * against a WRAP_CONTENT parent whose final size is driven by minSize.
 *
 *   - Root is an AbsoluteLayout with WRAP_CONTENT width, minWidth=400,
 *     and fixed height 200.
 *   - Child 1 has fixed size 200x200 at position (0, 0).
 *   - Child 2 (AbsoluteLayout) has proportional size (0.5, 1.0)
 *     — half parent width and full parent height — positioned at x=200.
 *     It contains two grandchildren: one fixed (50x100) and one
 *     proportional (0.5 * child2 width) at x=50.
 *
 * Expected result:
 *   The layout wraps to 400 (driven by minWidth, since child 1 is only
 *   200 wide). Child 2 gets 0.5 * 400 = 200 wide in the Arrange phase.
 *   Inside child 2: grandChild1 is 50x100 at (0,0), grandChild2 is
 *   0.5 * 200 = 100 wide at (50,0).
 *
 * Press Escape or Back to quit.
 */
class AbsoluteLayoutWrapContentProportionalController : public ConnectionTracker
{
public:
  AbsoluteLayoutWrapContentProportionalController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &AbsoluteLayoutWrapContentProportionalController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    AbsoluteLayout root = AbsoluteLayout::New()
      .SetBackgroundColor(Color::GAINSBORO)
      .SetMinimumWidth(400.0f)
      .SetRequestedHeight(200.0f);

    // Child 1: fixed 200x200 at (0, 0).
    View child1 = View::New();
    child1.SetBackgroundColor(Color::RED);
    child1.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetBounds(LayoutRect(0.0f, 0.0f, 200.0f, 200.0f)));
    root.Add(child1);

    // Child 2: AbsoluteLayout with proportional size (0.5 width, 1.0 height) at x=200.
    // Contains two grandchildren: one fixed, one proportional (0.5 of child2).
    AbsoluteLayout child2 = AbsoluteLayout::New()
      .SetBackgroundColor(Color::GREEN)
      .SetLayoutParams(AbsoluteLayoutParams::New()
        .SetBounds(LayoutRect(200.0f, 0.0f, 0.5f, 1.0f))
        .SetFlags(AbsoluteLayoutFlags::SIZE_PROPORTIONAL));
    root.Add(child2);

    View grandChild1 = View::New();
    grandChild1.SetBackgroundColor(Color::BLUE);
    grandChild1.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetBounds(LayoutRect(0.0f, 0.0f, 50.0f, 100.0f)));
    child2.Add(grandChild1);

    View grandChild2 = View::New();
    grandChild2.SetBackgroundColor(Color::YELLOW);
    grandChild2.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetBounds(LayoutRect(50.0f, 0.0f, 0.5f, 1.0f))
      .SetFlags(AbsoluteLayoutFlags::SIZE_PROPORTIONAL));
    child2.Add(grandChild2);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &AbsoluteLayoutWrapContentProportionalController::OnKeyEvent);
  }

  void OnKeyEvent(KeyEvent event)
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
  AbsoluteLayoutWrapContentProportionalController controller(application);
  application.MainLoop();
  return 0;
}
