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
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xFFFFFF));

    window.Add(
      StackLayout::New(StackOrientation::VERTICAL)
        .SetSpacing(STACK_SPACING)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(MATCH_PARENT)
        .SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
        .Children({
          Label::New("Text Cutout Example"),

          AbsoluteLayout::New()
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(WRAP_CONTENT)
          .Children({
            Label::New(BACKGROUND_TEXT)
            .SetTextColor(UiColor(0x3A6D75))
            .SetBackgroundColor(UiColor(0x9ED8DB))
            .SetFontSize(10.0f)
            .SetMultiLine(true)
            .SetLineHeight(0.8f)
            .SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(0.0f, 0.0f, 400.0f, 100.0f))),

            Label::New("Hello World")
            .SetFontFamily("SamsungOneUI_700")
            .SetTextColor(UiColor(0x000000).WithAlpha(0.0f))
            .SetBackgroundColor(UiColor(0x1F2A36))
            .SetTextFit(Text::FitRange(20, 100, 10))
            .SetHorizontalTextAlignment(Text::Alignment::CENTER)
            .SetVerticalTextAlignment(Text::Alignment::CENTER)
            .SetCutoutEnabled(true)
            .SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(0.0f, 0.0f, 400.0f, 100.0f)))
            .As(mLabel),
          }),

          Label::New("Text Mask Effect Example"),

          Label::New("Label::SetMaskEffect"),

          Label::New("Hello Mask")
          .SetFontFamily("SamsungOneUI_700")
          .SetRequestedWidth(400)
          .SetRequestedHeight(100)
          .SetTextColor(UiColor(0xff0000))
          .SetBackgroundColor(UiColor(0x1F2A36).WithAlpha(0.25f))
          .SetTextFit(Text::FitRange(20, 100, 10))
          .SetHorizontalTextAlignment(Text::Alignment::CENTER)
          .SetVerticalTextAlignment(Text::Alignment::CENTER)
          // This produces the same masking result as CreateLabelAsMaskSource().
          .SetMaskEffect(
            Label::New(BACKGROUND_TEXT)
            .SetRequestedWidth(400)
            .SetRequestedHeight(100)
            .SetTextColor(UiColor(0x3A6D75))
            .SetFontSize(10.0f)
            .SetMultiLine(true)
            .SetLineHeight(0.8f))
          .As(mMaskLabel),

          Label::New("View::SetRenderEffect(MaskEffect)"),

          CreateLabelAsMaskTarget(),

          // Manual equivalent of Label::SetMaskEffect().
          CreateLabelAsMaskSource(),
        }));
  
    window.KeyEventSignal().Connect(this, &TextController::OnKeyEvent);
  }

  Label CreateLabelAsMaskTarget()
  {
    auto maskSource = Label::New(BACKGROUND_TEXT)
      .SetRequestedWidth(400)
      .SetRequestedHeight(100)
      .SetTextColor(UiColor(0x3A6D75))
      .SetFontSize(10.0f)
      .SetMultiLine(true)
      .SetLineHeight(0.8f);

    auto target = Label::New("Hello Mask")
      .SetFontFamily("SamsungOneUI_700")
      .SetRequestedWidth(400)
      .SetRequestedHeight(100)
      .SetTextColor(UiColor(0xff0000))
      .SetBackgroundColor(UiColor(0x1F2A36).WithAlpha(0.25f))
      .SetTextFit(Text::FitRange(20, 100, 10))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER);

    target.Add(maskSource);
    target.SetRenderEffect(MaskEffect::New(maskSource));

    return target;
  }

  Label CreateLabelAsMaskSource()
  {
    auto maskTarget = Label::New(BACKGROUND_TEXT)
      .SetRequestedWidth(400)
      .SetRequestedHeight(100)
      .SetTextColor(UiColor(0x3A6D75))
      .SetFontSize(10.0f)
      .SetMultiLine(true)
      .SetLineHeight(0.8f);

    auto source = Label::New("Hello Mask")
      .SetFontFamily("SamsungOneUI_700")
      .SetRequestedWidth(400)
      .SetRequestedHeight(100)
      .SetTextColor(UiColor(0xff0000))
      .SetBackgroundColor(UiColor(0x1F2A36).WithAlpha(0.25f))
      .SetTextFit(Text::FitRange(20, 100, 10))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER);

    maskTarget.Add(source);
    maskTarget.SetRenderEffect(MaskEffect::New(source));

    return maskTarget;
  }

  void OnKeyEvent(const KeyEvent& event)
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
      mLabel.SetCutoutEnabled(false);
      mMaskLabel.ClearMaskEffect();
    }
    else if(event.GetKeyName() == "2")
    {
      mLabel.SetCutoutEnabled(true);
      mMaskLabel.SetMaskEffect(
        Label::New(BACKGROUND_TEXT)
        .SetRequestedWidth(400)
        .SetRequestedHeight(100)
        .SetTextColor(UiColor(0x3A6D75))
        .SetFontSize(10.0f)
        .SetMultiLine(true)
        .SetLineHeight(0.8f));
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
