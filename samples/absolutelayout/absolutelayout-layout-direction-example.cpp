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
 * AbsoluteLayout sample with a clickable LayoutDirection toggle.
 *
 * Same content as absolutelayout-example. A 100x50 yellow standalone
 * child at the top-left toggles the root view's LayoutDirection between
 * LEFT_TO_RIGHT and RIGHT_TO_LEFT on click. RTL flips direct children
 * horizontally; the standalone toggle button is excluded from mirroring
 * and stays at the top-left.
 *
 * Press Escape or Back to quit.
 */
class AbsoluteLayoutDirectionController : public ConnectionTracker
{
public:
  AbsoluteLayoutDirectionController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &AbsoluteLayoutDirectionController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Root: AbsoluteLayout filling the window
    mRoot = AbsoluteLayout::New();
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    // Red box: top-left corner, absolute position and size
    View redBox = View::New();
    redBox.SetBackgroundColor(Color::RED);
    redBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetBounds(LayoutRect(50.0f, 50.0f, 50.0f, 50.0f)));
    mRoot.Add(redBox);

    // Green box: center area
    View greenBox = View::New();
    greenBox.SetBackgroundColor(Color::GREEN);
    greenBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetBounds(LayoutRect(100.0f, 100.0f, 100.0f, 100.0f)));
    mRoot.Add(greenBox);

    // Blue box: lower-right area
    View blueBox = View::New();
    blueBox.SetBackgroundColor(Color::BLUE);
    blueBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetBounds(LayoutRect(200.0f, 200.0f, 100.0f, 50.0f)));
    mRoot.Add(blueBox);

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
    window.KeyEventSignal().Connect(this, &AbsoluteLayoutDirectionController::OnKeyEvent);
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
  Application&   mApplication;
  AbsoluteLayout mRoot;
  bool           mIsRtl{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig config = UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();
  AbsoluteLayoutDirectionController controller(application);
  application.MainLoop();
  return 0;
}
