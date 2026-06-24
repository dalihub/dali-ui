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
constexpr float STACK_SPACING = 10.0f;
constexpr float STACK_PADDING = 20.0f;
const char* BACKGROUND_TEXT = "HELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLDHELLOWORLD";
Label CreateBackgroundLabel()
{
  Label label = Label::New(BACKGROUND_TEXT);
  label.SetRequestedWidth(400);
  label.SetRequestedHeight(100);
  label.SetTextColor(UiColor(0x3A6D75));
  label.SetFontSize(10.0f);
  label.SetMultiLine(true);
  label.SetLineHeight(0.8f);
  return label;
}

Label CreateMaskLabel()
{
  Label label = Label::New("Hello Mask");
  label.SetFontFamily("SamsungOneUI_700");
  label.SetRequestedWidth(400);
  label.SetRequestedHeight(100);
  label.SetTextColor(UiColor(0xff0000));
  label.SetBackgroundColor(UiColor(0x1F2A36).WithAlpha(0.25f));
  label.SetTextFit(Text::FitRange(20, 100, 10));
  label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  return label;
}
} // namespace

class TextController : public ConnectionTracker
{
public:
  explicit TextController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xFFFFFF));

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetSpacing(STACK_SPACING);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING));

    root.Add(Label::New("Text Cutout Example"));

    AbsoluteLayout cutoutLayout = AbsoluteLayout::New();
    cutoutLayout.SetRequestedWidth(MATCH_PARENT);
    cutoutLayout.SetRequestedHeight(WRAP_CONTENT);

    Label backgroundLabel = Label::New(BACKGROUND_TEXT);
    backgroundLabel.SetTextColor(UiColor(0x3A6D75));
    backgroundLabel.SetBackgroundColor(UiColor(0x9ED8DB));
    backgroundLabel.SetFontSize(10.0f);
    backgroundLabel.SetMultiLine(true);
    backgroundLabel.SetLineHeight(0.8f);
    backgroundLabel.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(0.0f, 0.0f, 400.0f, 100.0f)));
    cutoutLayout.Add(backgroundLabel);

    mLabel = Label::New("Hello World");
    mLabel.SetFontFamily("SamsungOneUI_700");
    mLabel.SetTextColor(UiColor(0x000000).WithAlpha(0.0f));
    mLabel.SetBackgroundColor(UiColor(0x1F2A36));
    mLabel.SetTextFit(Text::FitRange(20, 100, 10));
    mLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mLabel.SetTextCutoutEnabled(true);
    mLabel.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(0.0f, 0.0f, 400.0f, 100.0f)));
    cutoutLayout.Add(mLabel);
    root.Add(cutoutLayout);

    root.Add(Label::New("Text Mask Effect Example"));
    root.Add(Label::New("Label::SetMaskEffect"));

    mMaskLabel = CreateMaskLabel();
    // This produces the same masking result as CreateLabelAsMaskSource().
    mMaskLabel.SetMaskEffect(CreateBackgroundLabel());
    root.Add(mMaskLabel);

    root.Add(Label::New("View::SetRenderEffect(MaskEffect)"));
    root.Add(CreateLabelAsMaskTarget());
    root.Add(CreateLabelAsMaskSource());

    window.Add(root);
  
    window.KeyEventSignal().Connect(this, &TextController::OnKeyEvent);
  }

  Label CreateLabelAsMaskTarget()
  {
    auto maskSource = CreateBackgroundLabel();
    auto target     = CreateMaskLabel();

    target.Add(maskSource);
    target.SetRenderEffect(MaskEffect::New(maskSource));

    return target;
  }

  Label CreateLabelAsMaskSource()
  {
    auto maskTarget = CreateBackgroundLabel();
    auto source     = CreateMaskLabel();

    maskTarget.Add(source);
    maskTarget.SetRenderEffect(MaskEffect::New(source));

    return maskTarget;
  }

  void OnKeyEvent(Window window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
      return;
    }
    if(event.GetKeyName() == "1")
    {
      mLabel.SetTextCutoutEnabled(false);
      mMaskLabel.ClearMaskEffect();
    }
    else if(event.GetKeyName() == "2")
    {
      mLabel.SetTextCutoutEnabled(true);
      mMaskLabel.SetMaskEffect(CreateBackgroundLabel());
    }
  }

private:
  Application& mApplication;
  Label        mLabel;
  Label        mMaskLabel;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextController controller(application);
  application.MainLoop();

  return 0;
}
