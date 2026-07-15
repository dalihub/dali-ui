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
 * GridLayout sample with a clickable LayoutDirection toggle.
 *
 * Same content as gridlayout-example. A 100x50 yellow standalone child
 * at the top-left toggles the root view's LayoutDirection between
 * LEFT_TO_RIGHT and RIGHT_TO_LEFT on click. RTL flips direct children
 * horizontally; the standalone toggle button is excluded from mirroring
 * and stays at the top-left.
 *
 * Press Escape or Back to quit.
 */
class GridLayoutDirectionController : public ConnectionTracker
{
public:
  GridLayoutDirectionController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &GridLayoutDirectionController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Root: GridLayout filling the window
    mRoot = GridLayout::New();
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    mRoot.SetPadding(Extents(50, 50, 50, 50));
    mRoot.SetRowSpacing(10.0f);
    mRoot.SetColumnSpacing(10.0f);

    // Define 3 rows and 2 columns with absolute sizes
    mRoot.AddRowDefinition(GridLength::Absolute(50.0f));
    mRoot.AddRowDefinition(GridLength::Absolute(100.0f));
    mRoot.AddRowDefinition(GridLength::Absolute(200.0f));
    mRoot.AddColumnDefinition(GridLength::Absolute(50.0f));
    mRoot.AddColumnDefinition(GridLength::Absolute(100.0f));

    // Cell (0,0): Red
    View cell00 = View::New();
    cell00.SetBackgroundColor(Color::RED);
    cell00.SetLayoutParams(GridLayoutParams::New());
    mRoot.Add(cell00);

    // Cell (0,1): Green
    View cell01 = View::New();
    cell01.SetBackgroundColor(Color::GREEN);
    cell01.SetLayoutParams(GridLayoutParams::New().SetColumn(1));
    mRoot.Add(cell01);

    // Cell (1,0): Blue
    View cell10 = View::New();
    cell10.SetBackgroundColor(Color::BLUE);
    cell10.SetLayoutParams(GridLayoutParams::New().SetRow(1));
    mRoot.Add(cell10);

    // Cell (1,1): Yellow
    View cell11 = View::New();
    cell11.SetBackgroundColor(Color::YELLOW);
    cell11.SetLayoutParams(GridLayoutParams::New().SetRow(1).SetColumn(1));
    mRoot.Add(cell11);

    // Cell (2,0): Cyan
    View cell20 = View::New();
    cell20.SetBackgroundColor(Color::CYAN);
    cell20.SetLayoutParams(GridLayoutParams::New().SetRow(2));
    mRoot.Add(cell20);

    // Cell (2,1): Magenta
    View cell21 = View::New();
    cell21.SetBackgroundColor(Color::MAGENTA);
    cell21.SetLayoutParams(GridLayoutParams::New().SetRow(2).SetColumn(1));
    mRoot.Add(cell21);

    // Toggle button: 200x50 translucent-black standalone child at top-left
    // with a centered white "Change LayoutDirection" label.
    InteractiveView toggleBtn = InteractiveView::New();
    toggleBtn.SetBackgroundColor(Vector4(0.0f, 0.0f, 0.0f, 0.5f));
    toggleBtn.SetRequestedWidth(200.0f);
    toggleBtn.SetRequestedHeight(50.0f);
    toggleBtn.SetRequestedX(0.0f);
    toggleBtn.SetRequestedY(0.0f);
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
    window.KeyEventSignal().Connect(this, &GridLayoutDirectionController::OnKeyEvent);
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
  GridLayout   mRoot;
  bool         mIsRtl{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig config = UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();
  GridLayoutDirectionController controller(application);
  application.MainLoop();
  return 0;
}
