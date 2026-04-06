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
 * FlexLayout justify content and align items sample.
 *
 * Demonstrates SpaceBetween justify and Center cross-axis alignment
 * in a horizontal flex row.
 *
 * - Three fixed-size boxes with different heights
 * - JustifyContent: SpaceBetween (even spacing between items)
 * - AlignItems: Center (vertically centered on the cross axis)
 * - One child uses AlignSelf to override to FlexEnd
 *
 * Press Escape or Back to quit.
 */
class FlexLayoutJustifyAlignController : public ConnectionTracker
{
public:
  FlexLayoutJustifyAlignController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &FlexLayoutJustifyAlignController::Create);
  }

  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    FlexLayout root = FlexLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetDirection(FlexDirection::ROW);
    root.SetJustifyContent(FlexJustify::SPACE_BETWEEN);
    root.SetAlignItems(FlexAlign::CENTER);
    root.SetViewPadding(Extents(50, 50, 50, 50));

    // Red box
    View redBox = View::New();
    redBox.SetBackgroundColor(Color::RED);
    redBox.SetRequestedWidth(50.0f);
    redBox.SetRequestedHeight(200.0f);
    root.Add(redBox);

    // Green box
    View greenBox = View::New();
    greenBox.SetBackgroundColor(Color::GREEN);
    greenBox.SetRequestedWidth(50.0f);
    greenBox.SetRequestedHeight(200.0f);
    greenBox.SetLayoutParams(FlexLayoutParams::New().SetAlignSelf(FlexAlign::FLEX_START));
    root.Add(greenBox);

    // Blue box
    View blueBox = View::New();
    blueBox.SetBackgroundColor(Color::BLUE);
    blueBox.SetRequestedWidth(50.0f);
    blueBox.SetRequestedHeight(200.0f);
    blueBox.SetLayoutParams(FlexLayoutParams::New().SetAlignSelf(FlexAlign::CENTER));
    root.Add(blueBox);

    // Yellow box
    View yellowBox = View::New();
    yellowBox.SetBackgroundColor(Color::YELLOW);
    yellowBox.SetRequestedWidth(50.0f);
    yellowBox.SetRequestedHeight(200.0f);
    yellowBox.SetLayoutParams(FlexLayoutParams::New().SetAlignSelf(FlexAlign::FLEX_END));
    root.Add(yellowBox);

    // Cyan box
    View cyanBox = View::New();
    cyanBox.SetBackgroundColor(Color::CYAN);
    cyanBox.SetRequestedWidth(50.0f);
    cyanBox.SetRequestedHeight(200.0f);
    cyanBox.SetLayoutParams(FlexLayoutParams::New().SetAlignSelf(FlexAlign::BASELINE));
    root.Add(cyanBox);

    // Magenta box
    View magentaBox = View::New();
    magentaBox.SetBackgroundColor(Color::MAGENTA);
    magentaBox.SetRequestedWidth(50.0f);
    magentaBox.SetRequestedHeight(200.0f);
    root.Add(magentaBox);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &FlexLayoutJustifyAlignController::OnKeyEvent);
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
  FlexLayoutJustifyAlignController controller(application);
  application.MainLoop();
  return 0;
}
