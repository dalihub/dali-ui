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

using namespace Dali;
using namespace Dali::Ui;

/**
 * FlexLayout sample (LayoutMode::STANDALONE): based on flexlayout-example.
 *
 * Same structure as flexlayout-example, but the first child (Red) is set to
 * LayoutMode::STANDALONE with RequestedWidth/Height = (100, 100) and
 * SetPositionX/Y = (300, 300). It is excluded from the flex line and placed
 * at (300, 300) in the parent's coordinate space, while Green and Blue still
 * participate in the flex row layout.
 *
 * Press Escape or Back to quit.
 */
class FlexLayoutStandaloneController : public ConnectionTracker
{
public:
  FlexLayoutStandaloneController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &FlexLayoutStandaloneController::Create);
  }

  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Root: FlexLayout in row direction
    FlexLayout root = FlexLayout::New()
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetDirection(FlexDirection::ROW)
      .SetAlignItems(FlexAlign::STRETCH)
      .SetPadding(Extents(50, 50, 50, 50));

    // Red box: fixed width
    View redBox = View::New();
    redBox.SetBackgroundColor(Color::RED);
    redBox.SetRequestedWidth(100.0f);
    root.Add(redBox);

    // Green box: flex-grow 1 (fills remaining space)
    View greenBox = View::New();
    greenBox.SetBackgroundColor(Color::GREEN);
    greenBox.SetRequestedWidth(WRAP_CONTENT);
    greenBox.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
    root.Add(greenBox);

    // Blue box: Standalone (100x100 at (300, 300))
    View blueBox = View::New();
    blueBox.SetBackgroundColor(Color::BLUE);
    blueBox.SetRequestedWidth(100.0f);
    blueBox.SetRequestedHeight(100.0f);
    blueBox.SetPositionX(300.0f);
    blueBox.SetPositionY(300.0f);
    blueBox.SetLayoutMode(LayoutMode::STANDALONE);
    root.Add(blueBox);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &FlexLayoutStandaloneController::OnKeyEvent);
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
  FlexLayoutStandaloneController controller(application);
  application.MainLoop();
  return 0;
}
