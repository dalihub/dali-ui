/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali-ui-foundation/public-api/absolute-layout.h>
#include <dali-ui-foundation/public-api/absolute-layout-params.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * AbsoluteLayout sample: absolute pixel positioning.
 *
 * Children are placed at explicit (x, y) coordinates with explicit sizes.
 * - Red box at top-left (20, 20)
 * - Green box at center area (150, 200)
 * - Blue box at bottom-right area (280, 400)
 *
 * Press Escape or Back to quit.
 */
class AbsoluteLayoutController : public ConnectionTracker
{
public:
  AbsoluteLayoutController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &AbsoluteLayoutController::Create);
  }

  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Root: AbsoluteLayout filling the window
    AbsoluteLayout root = AbsoluteLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    // Red box: top-left corner, absolute position and size
    View redBox = View::New();
    redBox.SetBackgroundColor(Color::RED);
    redBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetBounds(LayoutRect(20.0f, 20.0f, 120.0f, 80.0f)));
    root.AddView(redBox);

    // Green box: center area
    View greenBox = View::New();
    greenBox.SetBackgroundColor(Color::GREEN);
    greenBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetBounds(LayoutRect(150.0f, 200.0f, 160.0f, 100.0f)));
    root.AddView(greenBox);

    // Blue box: lower-right area
    View blueBox = View::New();
    blueBox.SetBackgroundColor(Color::BLUE);
    blueBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetBounds(LayoutRect(280.0f, 400.0f, 100.0f, 60.0f)));
    root.AddView(blueBox);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &AbsoluteLayoutController::OnKeyEvent);
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
  AbsoluteLayoutController controller(application);
  application.MainLoop();
  return 0;
}
