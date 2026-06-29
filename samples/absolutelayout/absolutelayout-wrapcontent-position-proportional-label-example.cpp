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
 *
 */

#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float TOGGLE_BUTTON_WIDTH  = 220.0f;
constexpr float TOGGLE_BUTTON_HEIGHT = 50.0f;
constexpr float FIXED_PARENT_WIDTH   = 400.0f;
constexpr float FIXED_PARENT_HEIGHT  = 200.0f;
} // namespace

/**
 * AbsoluteLayout sample: WRAP_CONTENT parent with a position-proportional label.
 *
 * Demonstrates that a WRAP_CONTENT AbsoluteLayout parent sizes to a WRAP_CONTENT
 * Label child when the child's bounds use a proportional x/y position and
 * WRAP_CONTENT width/height.
 *
 *   - Root view is MATCH_PARENT and green.
 *   - Parent AbsoluteLayout is WRAP_CONTENT and red.
 *   - Child Label is WRAP_CONTENT and blue.
 *   - Child bounds are (0.5, 0.5, -1.0, -1.0) with POSITION_PROPORTIONAL.
 *
 * Press Escape or Back to quit.
 */
class AbsoluteLayoutWrapContentPositionProportionalLabelController : public ConnectionTracker
{
public:
  AbsoluteLayoutWrapContentPositionProportionalLabelController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &AbsoluteLayoutWrapContentPositionProportionalLabelController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    View root = View::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetBackgroundColor(Color::GREEN);

    mParent = AbsoluteLayout::New();
    mParent.SetRequestedWidth(WRAP_CONTENT);
    mParent.SetRequestedHeight(WRAP_CONTENT);
    mParent.SetPadding(Extents(50, 50, 50, 50));
    mParent.SetBackgroundColor(Color::RED);
    root.Add(mParent);

    Label child = Label::New("WRAP Label");
    child.SetRequestedWidth(WRAP_CONTENT);
    child.SetRequestedHeight(WRAP_CONTENT);
    child.SetBackgroundColor(Color::BLUE);
    child.SetTextColor(UiColor(1.0f, 1.0f, 1.0f, 1.0f));
    child.SetLayoutParams(AbsoluteLayoutParams::New()
                            .SetBounds(LayoutRect(0.5f, 0.5f, -1.0f, -1.0f))
                            .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));
    mParent.Add(child);

    CreateToggleButton(window, root);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &AbsoluteLayoutWrapContentPositionProportionalLabelController::OnKeyEvent);
    window.ResizedSignal().Connect(this, &AbsoluteLayoutWrapContentPositionProportionalLabelController::OnWindowResized);
  }

  void OnKeyEvent(Window window, KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

  void OnWindowResized(Window, Window::WindowSize windowSize)
  {
    PositionToggleButton(windowSize);
  }

private:
  void CreateToggleButton(Window window, View root)
  {
    mToggleButton = InteractiveView::New();
    mToggleButton.SetBackgroundColor(Vector4(0.0f, 0.0f, 0.0f, 0.65f));
    mToggleButton.SetRequestedWidth(TOGGLE_BUTTON_WIDTH);
    mToggleButton.SetRequestedHeight(TOGGLE_BUTTON_HEIGHT);
    mToggleButton.SetRequestedPositionY(0.0f);
    mToggleButton.SetLayoutMode(LayoutMode::STANDALONE);
    mToggleButton.SetLayoutDirection(Dali::LayoutDirection::LEFT_TO_RIGHT);

    mToggleLabel = Label::New("Click to set size");
    mToggleLabel.SetTextColor(UiColor(1.0f, 1.0f, 1.0f, 1.0f));
    mToggleLabel.SetRequestedWidth(MATCH_PARENT);
    mToggleLabel.SetRequestedHeight(MATCH_PARENT);
    mToggleLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mToggleLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mToggleButton.Add(mToggleLabel);

    PositionSize positionSize = window.GetPositionSize();
    PositionToggleButton(Window::WindowSize(positionSize.width, positionSize.height));
    mToggleButton.ConnectClickedSignal(this, [this](View view, InputEvent event) -> bool {
      mFixedSize = !mFixedSize;

      if(mFixedSize)
      {
        mParent.SetRequestedWidth(FIXED_PARENT_WIDTH);
        mParent.SetRequestedHeight(FIXED_PARENT_HEIGHT);
        mToggleLabel.SetText("Click to set wrap");
      }
      else
      {
        mParent.SetRequestedWidth(WRAP_CONTENT);
        mParent.SetRequestedHeight(WRAP_CONTENT);
        mToggleLabel.SetText("Click to set size");
      }

      return true;
    });
    root.Add(mToggleButton);
  }

  void PositionToggleButton(Window::WindowSize windowSize)
  {
    mToggleButton.SetRequestedPositionX((static_cast<float>(windowSize.GetWidth()) - TOGGLE_BUTTON_WIDTH) * 0.5f);
  }

  Application& mApplication;
  AbsoluteLayout mParent;
  InteractiveView mToggleButton;
  Label          mToggleLabel;
  bool           mFixedSize{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  AbsoluteLayoutWrapContentPositionProportionalLabelController controller(application);
  application.MainLoop();
  return 0;
}
