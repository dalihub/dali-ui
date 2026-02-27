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
#include <dali-ui-foundation/public-api/stack-layout-params.h>

using namespace Dali;
using namespace Dali::Ui;

// This example shows how to use ScrollView with the new ScrollViewLayoutManager
class ScrollViewLayoutTest : public ConnectionTracker
{
public:

  ScrollViewLayoutTest(Application& application)
    : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &ScrollViewLayoutTest::Create);
  }

  ~ScrollViewLayoutTest() = default;

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Connect to key event for exit
    window.KeyEventSignal().Connect(this, &ScrollViewLayoutTest::OnKeyEvent);

    // Create content view larger than scroll view to test ScrollViewLayoutManager
    View content = View::New();
    content.SetBackgroundColor(Color::BLUE);
    content.SetLayoutWidth(800.0f);  // Natural width, not constrained by ScrollView
    content.SetLayoutHeight(1200.0f); // Natural height, not constrained by ScrollView

    // Create a ScrollView with vertical scrolling
    ScrollView scrollView = ScrollView::New()
      .SetScrollDirection(ScrollDirection::Vertical)
      .SetLayoutWidth(400.0f)
      .SetLayoutHeight(600.0f)
      .SetPosition(100.0f, 100.0f)
      .BackgroundColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f))
      .SetContent(content);

    // Add scroll view to window
    window.Add(scrollView);

    // Test with MatchParent content
    View matchParentContent = View::New();
    matchParentContent.SetBackgroundColor(Color::RED);
    matchParentContent.SetLayoutWidth(LayoutDimension::MatchParent);  // Should match ScrollView width
    matchParentContent.SetLayoutHeight(LayoutDimension::MatchParent); // Should match ScrollView height

    ScrollView scrollView2 = ScrollView::New()
      .SetScrollDirection(ScrollDirection::Vertical)
      .SetLayoutWidth(400.0f)
      .SetLayoutHeight(600.0f)
      .SetPosition(600.0f, 100.0f)
      .BackgroundColor(Vector4(0.8f, 0.8f, 0.8f, 1.0f))
      .SetContent(matchParentContent);

    window.Add(scrollView2);
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
  ScrollViewLayoutTest test(application);
  application.MainLoop();
  return 0;
}