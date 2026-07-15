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
using Dali::Ui::View;

/**
 * Demonstrates the typed animation API with both Bridge and Spec patterns.
 *
 * - Tap the red box: fade out using View-first Bridge pattern
 * - Tap the blue box: fade in using Spec-first pattern
 */
class AnimationExample : public ConnectionTracker
{
public:
  AnimationExample(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &AnimationExample::Create);
  }

  ~AnimationExample() = default;

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Build a reusable fade-in spec
    mFadeInSpec = View::NewAnimationSpec()
      .Opacity(1.0f, 500_ms, AlphaFunction::EASE_IN_OUT);

    InteractiveView redBox = InteractiveView::New();
    redBox.SetBackgroundColor(UiColor(0xFF0000));
    redBox.SetRequestedWidth(200_spx);
    redBox.SetRequestedHeight(200_spx);
    redBox.ConnectClickedSignal(this, &AnimationExample::OnRedClicked);
    mRedBox = redBox;

    InteractiveView blueBox = InteractiveView::New();
    blueBox.SetBackgroundColor(UiColor(0x0000FF));
    blueBox.SetOpacity(0.3f);
    blueBox.SetRequestedWidth(200_spx);
    blueBox.SetRequestedHeight(200_spx);
    blueBox.SetRequestedX(220_spx);
    blueBox.ConnectClickedSignal(this, &AnimationExample::OnBlueClicked);
    mBlueBox = blueBox;

    Layout root = Layout::New();
    root.Add(mRedBox);
    root.Add(mBlueBox);
    window.Add(root);
  }

  void OnRedClicked(View view, InputEvent e)
  {
    // View-first: Bridge pattern (stack-allocated, zero heap)
    auto anim = Animation::New();
    view.Animate(anim)
      .BackgroundColor(UiColor::PRIMARY, 500_ms)
      .PositionY(50.0f, 300_ms, AlphaFunction::EASE_OUT);

    anim.Play();
  }

  void OnBlueClicked(View view, InputEvent e)
  {
    // Spec-first: reusable spec applied to Animation
    auto anim = Animation::New();
    mFadeInSpec.ApplyTo(anim, view);
    anim.Play();
  }

private:
  Application&       mApplication;
  View               mRedBox;
  View               mBlueBox;
  ViewAnimationSpec  mFadeInSpec;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig config = UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();
  AnimationExample example(application);
  application.MainLoop();
  return 0;
}
