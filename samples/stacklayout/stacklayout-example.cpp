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
#include <dali-ui-foundation/public-api/stack-layout.h>

using namespace Dali;
using namespace Dali::UI;

/**
 * StackLayout sample: a vertical stack with fixed-height and weighted children.
 * - Root is a StackLayout that fills the window.
 * - Three rows: top (fixed), middle (weight 1), bottom (fixed).
 * - Press Escape or Back to quit.
 */
class StackLayoutController : public ConnectionTracker
{
public:
  StackLayoutController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &StackLayoutController::Create);
  }

  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Root: vertical StackLayout filling the window
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetLayoutWidth(LayoutDimension::MatchParent);
    root.SetLayoutHeight(LayoutDimension::MatchParent);
    root.SetSpacing(10.0f);
    root.SetViewPadding(Extents(20, 20, 20, 20));

    // Top bar: fixed height
    View topBar = View::New();
    topBar.SetBackgroundColor(Color::RED);
    topBar.SetLayoutWidth(LayoutDimension::MatchParent);
    topBar.SetLayoutHeight(80.0f);
    root.AddView(topBar);

    // Middle: weighted to take remaining space
    View middle = View::New();
    middle.SetBackgroundColor(Color::GREEN);
    middle.SetLayoutWidth(LayoutDimension::MatchParent);
    middle.SetLayoutHeight(LayoutDimension::WrapContent);
    StackLayout::SetLayoutWeight(middle, 1.0f);
    root.AddView(middle);

    // Bottom bar: fixed height
    View bottomBar = View::New();
    bottomBar.SetBackgroundColor(Color::BLUE);
    bottomBar.SetLayoutWidth(LayoutDimension::MatchParent);
    bottomBar.SetLayoutHeight(60.0f);
    root.AddView(bottomBar);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &StackLayoutController::OnKeyEvent);
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
  StackLayoutController controller(application);
  application.MainLoop();
  return 0;
}
