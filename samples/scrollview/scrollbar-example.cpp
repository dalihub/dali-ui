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
#include <dali-ui-foundation/extension-api/layout-scale-manager.h>
#include <dali/devel-api/adaptor-framework/application.h>
#include <cstdio>

using namespace Dali;
using namespace Dali::Ui;

// This example shows how to use ScrollView with ScrollBar component
// Keys: m feeds the small red bar a long content and prints its minimum size; q/w/e/r/t set the UI scale.
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
    content.SetPadding(Insets(20.0f, 20.0f, 20.0f, 20.0f));

    // Add multiple colored views to demonstrate scrolling
    for(int i = 0; i < 12; i++)
    {
      View view = View::New();
      view.SetBackgroundColor(Color::BLUE);
      view.SetRequestedWidth(MATCH_PARENT);
      view.SetRequestedHeight(180.0f);
      content.Add(view);
    }

    // Create a ScrollView with vertical scrolling and scroll bars
    ScrollView scrollView = ScrollView::New();
    scrollView.SetScrollDirection(ScrollDirection::Vertical);
    scrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Auto);
    scrollView.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never);
    scrollView.SetMaxFlingDistance(6000.0f);
    scrollView.SetMinimumFlingDuration(1000);
    scrollView.SetMaximumFlingDuration(2000);
    scrollView.SetFlingSensitivity(1.0f);
    scrollView.SetDecelerationRate(0.998f);
    scrollView.SetOverScrollMode(OverScrollMode::ContentScrolls);
    scrollView.SetRequestedWidth(600.0f);
    scrollView.SetRequestedHeight(1200.0f);
    scrollView.SetBackgroundColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f));
    scrollView.SetContent(content);

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
    // scrollBar.SetRequestedX(0.0f);
    // scrollBar.SetRequestedY(1250.0f);
    // window.Add(scrollBar);

    // A standalone ScrollBar whose minimum thumb length is the fallback (SetBarMinSize(0)): a
    // twentieth of the bar's longer side. Press m to feed it a content 500 times its height, so
    // the thumb shows that minimum.
    View barTrack = View::New();
    barTrack.SetRequestedWidth(40.0f);
    barTrack.SetRequestedHeight(200.0f);
    barTrack.SetRequestedX(20.0f);
    barTrack.SetRequestedY(20.0f);
    barTrack.SetBackgroundColor(Color::WHITE);
    mMinSizeBar = ScrollBar::New();
    mMinSizeBar.SetBarThickness(12.0f);
    mMinSizeBar.SetBarColor(Color::RED);
    mMinSizeBar.SetVerticalScrollBarVisibility(ScrollBarVisibility::Always);
    mMinSizeBar.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never);
    mMinSizeBar.SetBarMinSize(0.0f);
    barTrack.Add(mMinSizeBar);
    window.Add(barTrack);
  }

  void OnKeyEvent(Window window, KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
      else if(event.GetKeyName() == "m")
      {
        // The proportional thumb rounds to 0, so the minimum length wins.
        const Vector3 size = mMinSizeBar.GetProperty<Vector3>(Actor::Property::SIZE);
        mMinSizeBar.UpdateBarSize(size.width, size.height * 500.0f, size.width, size.height);
        // A new bar starts transparent; a position update fades it in.
        mMinSizeBar.UpdateScrollPosition(Vector2::ZERO);
        std::printf("[scrollbar-example] bar minimum size %.1f (natural units)\n", mMinSizeBar.GetBarMinSize());
      }
      else if(event.GetKeyName() == "q")
      {
        Ui::Extension::LayoutScaleManager::Get().SetUiScale(0.8f);
      }
      else if(event.GetKeyName() == "w")
      {
        Ui::Extension::LayoutScaleManager::Get().SetUiScale(1.0f);
      }
      else if(event.GetKeyName() == "e")
      {
        Ui::Extension::LayoutScaleManager::Get().SetUiScale(1.2f);
      }
      else if(event.GetKeyName() == "r")
      {
        Ui::Extension::LayoutScaleManager::Get().SetUiScale(1.5f);
      }
      else if(event.GetKeyName() == "t")
      {
        Ui::Extension::LayoutScaleManager::Get().SetUiScale(2.0f);
      }
    }
  }

private:
  Application& mApplication;
  ScrollBar    mMinSizeBar;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application                application = Application::New(&argc, &argv);
  ScrollBarExampleController test(application);
  application.MainLoop();
  return 0;
}
