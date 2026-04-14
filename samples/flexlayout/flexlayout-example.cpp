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
 * FlexLayout sample: basic row direction with flex grow.
 *
 * A horizontal flex row with three children:
 * - Red box: fixed 100px width
 * - Green box: flex-grow 1 (takes remaining space)
 * - Blue box: fixed 80px width
 *
 * Press Escape or Back to quit.
 */
class FlexLayoutController : public ConnectionTracker
{
public:
  FlexLayoutController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &FlexLayoutController::Create);
  }

  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Root: FlexLayout in row direction
    FlexLayout root = FlexLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetDirection(FlexDirection::ROW);
    root.SetAlignItems(FlexAlign::STRETCH);
    root.SetPadding(Extents(50, 50, 50, 50));

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

    // Blue box: fixed width
    View blueBox = View::New();
    blueBox.SetBackgroundColor(Color::BLUE);
    blueBox.SetRequestedWidth(100.0f);
    root.Add(blueBox);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &FlexLayoutController::OnKeyEvent);
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
  FlexLayoutController controller(application);
  application.MainLoop();
  return 0;
}
