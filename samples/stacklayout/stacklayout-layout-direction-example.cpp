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
 * StackLayout sample with a clickable LayoutDirection toggle.
 *
 * Same content as stacklayout-example but with horizontal orientation
 * (left/middle/right). A 200x50 translucent-black standalone child at
 * the top-left toggles the root view's LayoutDirection between
 * LEFT_TO_RIGHT and RIGHT_TO_LEFT on click. RTL flips direct children
 * horizontally; the standalone toggle button is excluded from mirroring
 * and stays at the top-left.
 *
 * Press Escape or Back to quit.
 */
class StackLayoutDirectionController : public ConnectionTracker
{
public:
  StackLayoutDirectionController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &StackLayoutDirectionController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Root: horizontal StackLayout filling the window
    mRoot = StackLayout::New(StackOrientation::HORIZONTAL);
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    mRoot.SetSpacing(10.0f);
    mRoot.SetPadding(Extents(50, 50, 50, 50));

    // Left bar: fixed width, fill cross-axis
    View leftBar = View::New();
    leftBar.SetBackgroundColor(Color::RED);
    leftBar.SetRequestedWidth(100.0f);
    leftBar.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
    mRoot.Add(leftBar);

    // Middle: weighted to take remaining space, fill cross-axis
    View middle = View::New();
    middle.SetBackgroundColor(Color::GREEN);
    middle.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    mRoot.Add(middle);

    // Right bar: fixed width, fill cross-axis
    View rightBar = View::New();
    rightBar.SetBackgroundColor(Color::BLUE);
    rightBar.SetRequestedWidth(100.0f);
    rightBar.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
    mRoot.Add(rightBar);

    // Toggle button: 200x50 translucent-black standalone child at top-left
    // with a centered white "Change LayoutDirection" label.
    InteractiveView toggleBtn = InteractiveView::New();
    toggleBtn.SetBackgroundColor(Vector4(0.0f, 0.0f, 0.0f, 0.5f));
    toggleBtn.SetRequestedWidth(200.0f);
    toggleBtn.SetRequestedHeight(50.0f);
    toggleBtn.SetRequestedPositionX(0.0f);
    toggleBtn.SetRequestedPositionY(0.0f);
    toggleBtn.SetLayoutMode(LayoutMode::STANDALONE);
    toggleBtn.SetLayoutDirection(Dali::LayoutDirection::LEFT_TO_RIGHT);
    Label toggleLabel = Label::New("Change LayoutDirection");
    toggleLabel.SetTextColor(UiColor(1.0f, 1.0f, 1.0f, 1.0f));
    toggleLabel.SetRequestedWidth(MATCH_PARENT);
    toggleLabel.SetRequestedHeight(MATCH_PARENT);
    toggleLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    toggleLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    toggleBtn.Add(toggleLabel);
    toggleBtn.ConnectClickedSignal(this, [this](View view, InputEvent event) -> bool {
      mIsRtl = !mIsRtl;
      mRoot.SetLayoutDirection(mIsRtl ? Dali::LayoutDirection::RIGHT_TO_LEFT : Dali::LayoutDirection::LEFT_TO_RIGHT);
      return true;
    });
    mRoot.Add(toggleBtn);

    window.Add(mRoot);
    window.KeyEventSignal().Connect(this, &StackLayoutDirectionController::OnKeyEvent);
  }

  void OnKeyEvent(Window window, KeyEvent event)
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
  StackLayout  mRoot;
  bool         mIsRtl{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  StackLayoutDirectionController controller(application);
  application.MainLoop();
  return 0;
}
