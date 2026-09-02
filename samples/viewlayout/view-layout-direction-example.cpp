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

using namespace Dali;
using namespace Dali::Ui;

/**
 * View sample with a clickable LayoutDirection toggle.
 *
 * Same content as view-margin-example. A 200x50 translucent-black
 * standalone child at the top-left toggles the root view's
 * LayoutDirection between LEFT_TO_RIGHT and RIGHT_TO_LEFT on click.
 *
 * The root is a plain View (no LayoutManager). The framework arranges
 * children in a LEFT_TO_RIGHT frame and then mirrors direct children
 * horizontally when the resolved direction is RIGHT_TO_LEFT. The
 * standalone toggle button is excluded from mirroring and stays at
 * the top-left.
 *
 * Press Escape or Back to quit.
 */
class ViewLayoutDirectionController : public ConnectionTracker
{
public:
  ViewLayoutDirectionController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &ViewLayoutDirectionController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Root: MATCH_PARENT
    mRoot = View::New();
    mRoot.SetBackgroundColor(Color::GRAY);
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);

    // Child 1: Red, width=MATCH_PARENT, height=200, margin=50
    View child1 = View::New();
    child1.SetBackgroundColor(Color::RED);
    child1.SetRequestedWidth(MATCH_PARENT);
    child1.SetRequestedHeight(200.0f);
    child1.SetMargin(Insets(50.0f, 50.0f, 50.0f, 50.0f));
    mRoot.Add(child1);

    // Child 2: Green, width=WRAP_CONTENT, height=200, pos=(0,300), margin=50
    View child2 = View::New();
    child2.SetBackgroundColor(Color::GREEN);
    child2.SetRequestedWidth(WRAP_CONTENT);
    child2.SetRequestedHeight(200.0f);
    child2.SetRequestedY(300.0f);
    child2.SetMargin(Insets(50.0f, 50.0f, 50.0f, 50.0f));

    // Child 2's child: Yellow, 100x100
    View grandchild = View::New();
    grandchild.SetBackgroundColor(Color::YELLOW);
    grandchild.SetRequestedWidth(100.0f);
    grandchild.SetRequestedHeight(100.0f);
    child2.Add(grandchild);

    mRoot.Add(child2);

    // Child 3: Blue, width=200, height=MATCH_PARENT, pos=(0,600)
    View child3 = View::New();
    child3.SetBackgroundColor(Color::BLUE);
    child3.SetRequestedWidth(200.0f);
    child3.SetRequestedHeight(MATCH_PARENT);
    child3.SetRequestedY(600.0f);
    child3.SetMargin(Insets(50.0f, 50.0f, 50.0f, 50.0f));
    mRoot.Add(child3);

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
    window.KeyEventSignal().Connect(this, &ViewLayoutDirectionController::OnKeyEvent);
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

private:
  Application& mApplication;
  View         mRoot;
  bool         mIsRtl{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig config = UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();
  ViewLayoutDirectionController controller(application);
  application.MainLoop();
  return 0;
}
