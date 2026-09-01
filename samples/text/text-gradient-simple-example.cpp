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

namespace
{
constexpr int   WINDOW_WIDTH   = 760;
constexpr int   WINDOW_HEIGHT  = 300;
constexpr float STACK_PADDING  = 32.0f;
constexpr float STACK_SPACING  = 32.0f;
constexpr float ANIMATION_TIME = 2.2f;
constexpr float ANIMATED_GRADIENT_START_OFFSET = 0.0f;
constexpr float ANIMATED_GRADIENT_END_OFFSET   = 1.0f;

Label CreateLabel(const char* text, float fontSize)
{
  Label label = Label::New(text);
  label.SetFontSize(fontSize);
  label.SetFontFamily("SamsungOneUI_500");
  label.SetMultiLine(true);
  return label;
}

void ApplyStaticGradient(Label label)
{
  // 1. Linear TextGradient를 생성한다.
  //    OBJECT_BOUNDING_BOX 단위에서는 -0.5 ~ 0.5 근처 값을 사용하면
  //    Label의 텍스트 영역을 기준으로 gradient 방향을 잡기 쉽다.
  Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(Gradient::SpreadMethod::PAD);

  // 2. stop node를 설정한다.
  //    stop이 2개 미만이면 Label은 TextGradient를 제거하고 일반 텍스트 색상으로 돌아간다.
  gradient.SetStopNodes({
    Gradient::StopNode(0.0f, UiColor(0x2563EB)),
    Gradient::StopNode(0.45f, UiColor(0x14B8A6)),
    Gradient::StopNode(1.0f, UiColor(0xF97316)),
  });

  // 3. 완성된 authored gradient 값을 Label에 적용한다.
  label.SetTextGradient(gradient);
}

void InspectAuthoredGradient(Label label)
{
  // Label은 authored gradient 값을 저장하므로, GetTextGradient()로 다시 읽을 수 있다.
  Gradient::Base authoredGradient = label.GetTextGradient();

  // concrete 타입의 값이 필요하면 DownCast()를 사용한다.
  // source type이 맞지 않으면 returned value의 GetType()은 Gradient::Type::NONE이다.
  Gradient::Linear linear = Gradient::Linear::DownCast(authoredGradient);
  if(linear.GetType() == Gradient::Type::LINEAR)
  {
    // DownCast 이후에는 Linear 고유 값과 공통 값을 조회할 수 있다.
    const Vector2 startPosition = linear.GetStartPosition();
    const Vector2 endPosition   = linear.GetEndPosition();
    const Gradient::SpreadMethod spreadMethod = linear.GetSpreadMethod();
    const float startOffset                   = linear.GetStartOffset();
    const auto  stopNodes                     = linear.GetStopNodes();

    (void)startPosition;
    (void)endPosition;
    (void)spreadMethod;
    (void)startOffset;
    (void)stopNodes;
  }
}

void ApplyAnimatedGradient(Label label)
{
  Gradient::Linear gradient(Vector2(-0.35f, 0.0f), Vector2(0.35f, 0.0f));
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(Gradient::SpreadMethod::REFLECT);

  // TextGradientStartOffset animation은 이 authored startOffset에서 시작한다.
  gradient.SetStartOffset(ANIMATED_GRADIENT_START_OFFSET);
  gradient.SetStopNodes({
    Gradient::StopNode(0.0f, UiColor(0x7C3AED)),
    Gradient::StopNode(0.35f, UiColor(0x2563EB)),
    Gradient::StopNode(0.65f, UiColor(0x06B6D4)),
    Gradient::StopNode(1.0f, UiColor(0xF97316)),
  });
  label.SetTextGradient(gradient);

  // 앱 코드에서 현재 authored 값을 확인해야 할 때의 Get/DownCast 사용 예시.
  InspectAuthoredGradient(label);
}

} // namespace

class TextGradientSimpleController : public ConnectionTracker
{
public:
  explicit TextGradientSimpleController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextGradientSimpleController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetPositionSize(PositionSize(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
    window.SetBackgroundColor(UiColor(0xF8FAFC));

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetSpacing(STACK_SPACING);
    root.SetPadding(Insets(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING));
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetBackgroundColor(UiColor(0xF8FAFC));

    mStaticLabel = CreateLabel("Static Linear Gradient", 60.0f);
    ApplyStaticGradient(mStaticLabel);

    mAnimatedLabel = CreateLabel("Animated Gradient", 60.0f);
    ApplyAnimatedGradient(mAnimatedLabel);

    root.Add(mStaticLabel);
    root.Add(mAnimatedLabel);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &TextGradientSimpleController::OnKeyEvent);

    StartAnimation();
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    const Dali::String& keyName = event.GetKeyName();
    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
    else if(keyName == "a" || keyName == "A")
    {
      ToggleAnimation();
    }
    else if(keyName == "r" || keyName == "R")
    {
      Reset();
    }
    else if(keyName == "c" || keyName == "C")
    {
      Clear();
    }
  }

  void StartAnimation()
  {
    StopAnimation();
    ApplyAnimatedGradient(mAnimatedLabel);

    mAnimation = Animation::New(ANIMATION_TIME);
    mAnimatedLabel.Animate(mAnimation)
      .TextGradientStartOffset(ANIMATED_GRADIENT_END_OFFSET, Duration(ANIMATION_TIME));
    mAnimation.SetLoopCount(Animation::INFINITE_LOOP);
    mAnimation.SetLoopingMode(Animation::AUTO_REVERSE);
    mAnimation.Play();
    mAnimationRunning = true;
  }

  void StopAnimation()
  {
    if(mAnimation)
    {
      mAnimation.Stop();
      mAnimation.Clear();
    }
    mAnimationRunning = false;
  }

  void ToggleAnimation()
  {
    if(mAnimationRunning)
    {
      StopAnimation();
    }
    else
    {
      StartAnimation();
    }
  }

  void Reset()
  {
    StopAnimation();
    ApplyAnimatedGradient(mAnimatedLabel);
  }

  void Clear()
  {
    StopAnimation();
    mAnimatedLabel.SetTextGradient(Gradient::Base::None());
  }

private:
  Application& mApplication;
  Label        mStaticLabel;
  Label        mAnimatedLabel;
  Label        mInstructionLabel;
  Animation    mAnimation;
  bool         mAnimationRunning{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextGradientSimpleController controller(application);
  application.MainLoop();
  return 0;
}
