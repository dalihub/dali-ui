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
#include <dali-ui-foundation/public-api/absolute-layout.h>

using namespace Dali;
using namespace Dali::UI;

/**
 * AbsoluteLayout overlapping children sample.
 *
 * Demonstrates that AbsoluteLayout allows children to overlap freely.
 * Children are rendered in the order they are added (later children
 * are drawn on top of earlier ones).
 *
 * - Large red background box
 * - Medium green box overlapping the red box
 * - Small blue box overlapping both
 * - Cyan box using proportional position centered in the layout
 *
 * Press Escape or Back to quit.
 */
class AbsoluteLayoutOverlapController : public ConnectionTracker
{
public:
  AbsoluteLayoutOverlapController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &AbsoluteLayoutOverlapController::Create);
  }

  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    AbsoluteLayout root = AbsoluteLayout::New();
    root.SetLayoutWidth(LayoutDimension::MatchParent);
    root.SetLayoutHeight(LayoutDimension::MatchParent);

    // Large red background box
    View redBox = View::New();
    redBox.SetBackgroundColor(Color::RED);
    AbsoluteLayout::SetLayoutBounds(redBox, LayoutRect(30.0f, 30.0f, 300.0f, 200.0f));
    AbsoluteLayout::SetLayoutFlags(redBox, AbsoluteLayoutFlags::NONE);
    root.AddView(redBox);

    // Medium green box overlapping the red box
    View greenBox = View::New();
    greenBox.SetBackgroundColor(Color::GREEN);
    AbsoluteLayout::SetLayoutBounds(greenBox, LayoutRect(100.0f, 80.0f, 200.0f, 200.0f));
    AbsoluteLayout::SetLayoutFlags(greenBox, AbsoluteLayoutFlags::NONE);
    root.AddView(greenBox);

    // Small blue box overlapping both
    View blueBox = View::New();
    blueBox.SetBackgroundColor(Color::BLUE);
    AbsoluteLayout::SetLayoutBounds(blueBox, LayoutRect(160.0f, 130.0f, 100.0f, 100.0f));
    AbsoluteLayout::SetLayoutFlags(blueBox, AbsoluteLayoutFlags::NONE);
    root.AddView(blueBox);

    // Cyan box: proportional position centered, absolute size
    View cyanBox = View::New();
    cyanBox.SetBackgroundColor(Color::CYAN);
    AbsoluteLayout::SetLayoutBounds(cyanBox, LayoutRect(0.5f, 0.7f, 140.0f, 80.0f));
    AbsoluteLayout::SetLayoutFlags(cyanBox, AbsoluteLayoutFlags::POSITION_PROPORTIONAL);
    root.AddView(cyanBox);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &AbsoluteLayoutOverlapController::OnKeyEvent);
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
  AbsoluteLayoutOverlapController controller(application);
  application.MainLoop();
  return 0;
}
