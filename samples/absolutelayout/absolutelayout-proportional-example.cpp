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
 * AbsoluteLayout proportional positioning and sizing sample.
 *
 * Demonstrates the composite AbsoluteLayoutFlags values
 * (SIZE_PROPORTIONAL, ALL, POSITION_PROPORTIONAL) stacked in add order:
 *
 *   - Blue  : SIZE_PROPORTIONAL     bounds(  0,   0, 1.0, 1.0)
 *             → fills the entire root (w = parent*1.0, h = parent*1.0)
 *   - Red   : ALL                   bounds(0.5, 0.5, 0.5, 0.5)
 *             → centered, half-size (drawn on top of blue)
 *   - Green : POSITION_PROPORTIONAL bounds(0.5, 0.5, 100, 100)
 *             → centered 100x100 box (drawn on top of red)
 *
 * Press Escape or Back to quit.
 */
class AbsoluteLayoutProportionalController : public ConnectionTracker
{
public:
  AbsoluteLayoutProportionalController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &AbsoluteLayoutProportionalController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    AbsoluteLayout root = AbsoluteLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);

    // Blue box: SIZE_PROPORTIONAL fills the entire root.
    View blueBox = View::New();
    blueBox.SetBackgroundColor(Color::BLUE);
    blueBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetBounds(LayoutRect(0.0f, 0.0f, 1.0f, 1.0f))
      .SetFlags(AbsoluteLayoutFlags::SIZE_PROPORTIONAL));
    root.Add(blueBox);

    // Red box: ALL centers a half-size child.
    View redBox = View::New();
    redBox.SetBackgroundColor(Color::RED);
    redBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetBounds(LayoutRect(0.5f, 0.5f, 0.5f, 0.5f))
      .SetFlags(AbsoluteLayoutFlags::ALL));
    root.Add(redBox);

    // Green box: POSITION_PROPORTIONAL centers a fixed 100x100 box.
    View greenBox = View::New();
    greenBox.SetBackgroundColor(Color::GREEN);
    greenBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetBounds(LayoutRect(0.5f, 0.5f, 100.0f, 100.0f))
      .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));
    root.Add(greenBox);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &AbsoluteLayoutProportionalController::OnKeyEvent);
  }

  void OnKeyEvent(Window window, KeyEvent event)
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
  AbsoluteLayoutProportionalController controller(application);
  application.MainLoop();
  return 0;
}
