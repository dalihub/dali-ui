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
 * AbsoluteLayout sample: per-axis proportional flags.
 *
 * Four children, each demonstrating a single axis-only proportional flag,
 * anchored to a distinct edge of the root (MATCH_PARENT of the window)
 * so that they do not overlap for reasonable window sizes (>= 400 x 400).
 *
 *   - Yellow : HEIGHT_PROPORTIONAL bounds(  0, 100, 100, 0.5)
 *              → x=0, y=100, w=100, h=windowHeight*0.5 (left strip)
 *   - Blue   : WIDTH_PROPORTIONAL  bounds(100,   0, 0.5, 100)
 *              → x=100, y=0, w=windowWidth*0.5, h=100 (top strip)
 *   - Red    : X_PROPORTIONAL      bounds(1.0,   0, 100, 100)
 *              → x=(windowWidth-100)*1.0 (top-right corner)
 *   - Green  : Y_PROPORTIONAL      bounds(  0, 1.0, 100, 100)
 *              → y=(windowHeight-100)*1.0 (bottom-left corner)
 *
 * Press Escape or Back to quit.
 */
class AbsoluteLayoutPerAxisProportionalController : public ConnectionTracker
{
public:
  AbsoluteLayoutPerAxisProportionalController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &AbsoluteLayoutPerAxisProportionalController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    AbsoluteLayout root = AbsoluteLayout::New()
                            .SetBackgroundColor(Color::GAINSBORO)
                            .SetRequestedWidth(MATCH_PARENT)
                            .SetRequestedHeight(MATCH_PARENT);

    // Child 1: Height proportional only.
    View yellowBox = View::New();
    yellowBox.SetBackgroundColor(Color::YELLOW);
    yellowBox.SetLayoutParams(AbsoluteLayoutParams::New()
                                .SetBounds(LayoutRect(0.0f, 100.0f, 100.0f, 0.5f))
                                .SetFlags(AbsoluteLayoutFlags::HEIGHT_PROPORTIONAL));
    root.Add(yellowBox);

    // Child 2: Width proportional only.
    View blueBox = View::New();
    blueBox.SetBackgroundColor(Color::BLUE);
    blueBox.SetLayoutParams(AbsoluteLayoutParams::New()
                              .SetBounds(LayoutRect(100.0f, 0.0f, 0.5f, 100.0f))
                              .SetFlags(AbsoluteLayoutFlags::WIDTH_PROPORTIONAL));
    root.Add(blueBox);

    // Child 3: X proportional only.
    View redBox = View::New();
    redBox.SetBackgroundColor(Color::RED);
    redBox.SetLayoutParams(AbsoluteLayoutParams::New()
                             .SetBounds(LayoutRect(1.0f, 0.0f, 100.0f, 100.0f))
                             .SetFlags(AbsoluteLayoutFlags::X_PROPORTIONAL));
    root.Add(redBox);

    // Child 4: Y proportional only.
    View greenBox = View::New();
    greenBox.SetBackgroundColor(Color::GREEN);
    greenBox.SetLayoutParams(AbsoluteLayoutParams::New()
                               .SetBounds(LayoutRect(0.0f, 1.0f, 100.0f, 100.0f))
                               .SetFlags(AbsoluteLayoutFlags::Y_PROPORTIONAL));
    root.Add(greenBox);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &AbsoluteLayoutPerAxisProportionalController::OnKeyEvent);
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
  Application                                 application = Application::New(&argc, &argv);
  AbsoluteLayoutPerAxisProportionalController controller(application);
  application.MainLoop();
  return 0;
}
