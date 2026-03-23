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
 * Demonstrates the use of AbsoluteLayoutFlags to position and size children
 * as a proportion (0.0 - 1.0) of the parent layout area.
 *
 * - Red box: position proportional (top-left 10%), size absolute (120x80 px)
 * - Green box: all proportional (centered at 50%, size 30% x 20%)
 * - Blue box: size proportional only (position absolute at 10,400, size 80% x 10%)
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

  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    AbsoluteLayout root = AbsoluteLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    // Red box: proportional position, absolute size
    View redBox = View::New();
    redBox.SetBackgroundColor(Color::RED);
    redBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetBounds(LayoutRect(0.1f, 0.1f, 120.0f, 80.0f))
      .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));
    root.Add(redBox);

    // Green box: all proportional (position and size)
    View greenBox = View::New();
    greenBox.SetBackgroundColor(Color::GREEN);
    greenBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetBounds(LayoutRect(0.5f, 0.5f, 0.3f, 0.2f))
      .SetFlags(AbsoluteLayoutFlags::ALL));
    root.Add(greenBox);

    // Blue box: proportional size, absolute position
    View blueBox = View::New();
    blueBox.SetBackgroundColor(Color::BLUE);
    blueBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetBounds(LayoutRect(10.0f, 400.0f, 0.8f, 0.1f))
      .SetFlags(AbsoluteLayoutFlags::SIZE_PROPORTIONAL));
    root.Add(blueBox);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &AbsoluteLayoutProportionalController::OnKeyEvent);
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
  AbsoluteLayoutProportionalController controller(application);
  application.MainLoop();
  return 0;
}
