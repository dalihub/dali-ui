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
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>

using namespace Dali;
using namespace Dali::Ui;

// This example shows how to use ScrollView component with pan gesture support
class ScrollViewController : public ConnectionTracker
{
public:

  ScrollViewController(Application& application)
    : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &ScrollViewController::Create);
  }

  ~ScrollViewController() = default;

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Connect to key event for exit
    window.KeyEventSignal().Connect(this, &ScrollViewController::OnKeyEvent);

    // Create content view larger than scroll view
    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(2400.0f);
    content.SetSpacing(10.0f);
    content.SetViewPadding(Extents(20, 20, 20, 20));

    View top = View::New();
    top.SetBackgroundColor(Color::RED);
    top.SetRequestedWidth(MATCH_PARENT);
    top.SetRequestedHeight(WRAP_CONTENT);
    top.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    content.AddView(top);

    View middle = View::New();
    middle.SetBackgroundColor(Color::GREEN);
    middle.SetRequestedWidth(MATCH_PARENT);
    middle.SetRequestedHeight(WRAP_CONTENT);
    middle.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    content.AddView(middle);

    View bottom = View::New();
    bottom.SetBackgroundColor(Color::BLUE);
    bottom.SetRequestedWidth(MATCH_PARENT);
    bottom.SetRequestedHeight(WRAP_CONTENT);
    bottom.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    content.AddView(bottom);

    // Create a ScrollView with vertical scrolling using chaining methods
    ScrollView scrollView = ScrollView::New()
      .SetScrollDirection(ScrollDirection::Vertical)
      .SetMaxFlingDistance(10000.0f)
      .SetFlingSensitivity(1.0f)
      .SetDecelerationRate(0.998f)
      .SetOverScrollMode(OverScrollMode::ContentScrolls)
      .SetRequestedWidth(600.0f)
      .SetRequestedHeight(1200.0f)
      .SetPositionX(0.0f)
      .SetPositionY(0.0f)
      .SetBackgroundColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f))
      .SetContent(content);

    // Add scroll view to window
    window.Add(scrollView);
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
  ScrollView mScrollView;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  ScrollViewController test(application);
  application.MainLoop();
  return 0;
}
