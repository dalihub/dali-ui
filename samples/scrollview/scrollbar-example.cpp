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

// This example shows how to use ScrollView with ScrollBar component
class ScrollBarExampleController : public ConnectionTracker
{
public:

  ScrollBarExampleController(Application& application)
    : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &ScrollBarExampleController::Create);
  }

  ~ScrollBarExampleController() = default;

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Connect to key event for exit
    window.KeyEventSignal().Connect(this, &ScrollBarExampleController::OnKeyEvent);

    // Create content view larger than scroll view
    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetSpacing(10.0f);
    content.SetPadding(Extents(20, 20, 20, 20));

    // Add multiple colored views to demonstrate scrolling
    for (int i = 0; i < 12; i++)
    {
      View view = View::New();
      view.SetBackgroundColor(Color::BLUE);
      view.SetRequestedWidth(MATCH_PARENT);
      view.SetRequestedHeight(180.0f);
      content.Add(view);
    }

    // Create a ScrollView with vertical scrolling and scroll bars
    ScrollView scrollView = ScrollView::New()
      .SetScrollDirection(ScrollDirection::Vertical)
      .SetVerticalScrollBarVisibility(ScrollBarVisibility::Auto)
      .SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never)
      .SetMaxFlingDistance(6000.0f)
      .SetMinimumFlingDuration(1000)
      .SetMaximumFlingDuration(2000)
      .SetFlingSensitivity(1.0f)
      .SetDecelerationRate(0.998f)
      .SetOverScrollMode(OverScrollMode::ContentScrolls)
      .SetRequestedWidth(600.0f)
      .SetRequestedHeight(1200.0f)
      .SetBackgroundColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f))
      .SetContent(content);

    // Add scroll view to window
    window.Add(scrollView);

    // Create a separate ScrollBar to demonstrate standalone usage
    // ScrollBar scrollBar = ScrollBar::New();
    // scrollBar.SetBarThickness(12.0f);
    // scrollBar.SetBarColor(Color::BLUE);
    // scrollBar.SetVerticalScrollBarVisibility(ScrollBarVisibility::Auto);
    // scrollBar.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never);
    // scrollBar.SetRequestedWidth(600.0f);
    // scrollBar.SetRequestedHeight(50.0f);
    // scrollBar.SetPositionX(0.0f);
    // scrollBar.SetPositionY(1250.0f);
    // window.Add(scrollBar);
  }

  void OnKeyEvent(KeyEvent event)
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
  ScrollBarExampleController test(application);
  application.MainLoop();
  return 0;
}