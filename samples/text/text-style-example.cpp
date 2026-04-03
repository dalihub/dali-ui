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
#include <dali/integration-api/debug.h>
#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float STACK_SPACING = 10.0f;
constexpr float STACK_PADDING = 20.0f;
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

    window.Add(StackLayout::New(StackOrientation::VERTICAL)
      .Spacing(STACK_SPACING)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetViewPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
      .Children({
        Label::New("Text Style Example"),
        Label::New("Italic Lorem ipsum dolor sit amet.")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetFontSlant(Text::FontSlant::ITALIC)
          .SetFontSize(20)
          .As(mSlantLabel),
        Label::New("Bold Lorem ipsum dolor sit amet.")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetFontWeight(Text::FontWeight::BOLD)
          .SetFontSize(20)
          .As(mWeightLabel),
        Label::New("Semi condensed Lorem ipsum dolor sit amet.")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetFontWidth(Text::FontWidth::SEMI_CONDENSED)
          .SetFontSize(20)
          .As(mWidthLabel),
        // Underline
        Label::New("Underline Label Minimum")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetFontSize(20)
          .SetUnderline(Text::Underline()),
        Label::New("Dashed Underline Label")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetFontSize(20)
          .SetRequestedWidth(MATCH_PARENT)
          .SetUnderline(Text::Underline()
            .SetColor(UiColor(0x0088FF))
            .SetThickness(2)
            .SetType(Text::Underline::Type::DASHED)
            .SetDashLength(4.0f)
            .SetDashGap(4.0f))
          .As(mUnderlineLabel),
        // Shadow
        Label::New("Shadow Label Minimum")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetFontSize(20)
          .SetShadow(Text::Shadow().SetOffset(Vector2(1, 1))),
        Label::New("Shadow Label Properties")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetFontSize(20)
          .SetRequestedWidth(MATCH_PARENT)
          .SetShadow(Text::Shadow()
            .SetColor(UiColor(0xFF5500))
            .SetOffset(Vector2(3.0f, 3.0f))
            .SetBlurRadius(2.0f))
          .As(mShadowLabel),
        // Outline
        Label::New("Outline Label Minimum")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetFontSize(20)
          .SetOutline(Text::Outline().SetWidth(2.0f)),
        Label::New("Outline Label Properties")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetFontSize(20)
          .SetRequestedWidth(MATCH_PARENT)
          .SetOutline(Text::Outline()
            .SetColor(UiColor(0x0066FF))
            .SetOffset(Vector2(1.0f, 1.0f))
            .SetWidth(2.0f)
            .SetBlurRadius(1.0f))
          .As(mOutlineLabel),
        // LineThrough
        Label::New("LineThrough Label Minimum")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetFontSize(20)
          .SetLineThrough(Text::LineThrough()),
        Label::New("LineThrough Label Properties")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetFontSize(20)
          .SetRequestedWidth(MATCH_PARENT)
          .SetLineThrough(Text::LineThrough()
            .SetColor(UiColor(0xFF00FF))
            .SetThickness(3.0f))
          .As(mLineThroughLabel),
        // InputField with text styles
        Label::New("InputField with Styles:"),
        InputField::New()
          .SetText("InputField with Underline")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetFontSize(20)
          .SetRequestedWidth(MATCH_PARENT)
          .SetUnderline(Text::Underline()
            .SetColor(UiColor(0x0088FF))
            .SetThickness(2))
          .As(mInputFieldUnderline),
        InputField::New()
          .SetText("InputField with Shadow")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetFontSize(20)
          .SetRequestedWidth(MATCH_PARENT)
          .SetShadow(Text::Shadow()
            .SetColor(UiColor(0xFF5500))
            .SetOffset(Vector2(2.0f, 2.0f)))
          .As(mInputFieldShadow),
        InputField::New()
          .SetText("InputField with Outline")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetFontSize(20)
          .SetRequestedWidth(MATCH_PARENT)
          .SetOutline(Text::Outline()
            .SetColor(UiColor(0x0066FF))
            .SetWidth(2.0f))
          .As(mInputFieldOutline),
        InputField::New()
          .SetText("InputField with LineThrough")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetFontSize(20)
          .SetRequestedWidth(MATCH_PARENT)
          .SetLineThrough(Text::LineThrough()
            .SetColor(UiColor(0xFF00FF))
            .SetThickness(2.0f))
          .As(mInputFieldLineThrough),
        }));

    PrintLabelInfo(mSlantLabel, "Slant Label");
    PrintLabelInfo(mWeightLabel, "Weight Label");
    PrintLabelInfo(mWidthLabel, "Width Label");

    PlayAnimation(mUnderlineLabel, Dali::Color::RED, Dali::Color::BLUE);

    window.KeyEventSignal().Connect(this, &TextController::OnKeyEvent);
  }

  void PlayAnimation(Label label, Vector4 startColor, Vector4 endColor)
  {
    label.SetProperty(Label::Property::TEXT_COLOR, startColor);

    Animation animation = Animation::New(1.f);
    animation.AnimateTo(Property(label, Label::Property::TEXT_COLOR), endColor, AlphaFunction::SIN);
    animation.SetLoopCount(0);
    animation.Play();
  }

  void PrintLabelInfo(Label label, const char* title)
  {
    DALI_LOG_ERROR("----------------------------------------------------------------\n");
    DALI_LOG_ERROR("%s\n", title);
    DALI_LOG_ERROR("Text       : %s\n", label.GetText().CStr());
    DALI_LOG_ERROR("FontWeight : %s\n", TextAbstraction::FontWeight::Name[label.GetFontWeight()]);
    DALI_LOG_ERROR("FontWidth  : %s\n", TextAbstraction::FontWidth::Name[label.GetFontWidth()]);
    DALI_LOG_ERROR("FontSlant  : %s\n", TextAbstraction::FontSlant::Name[label.GetFontSlant()]);
    DALI_LOG_ERROR("----------------------------------------------------------------\n");
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      DALI_LOG_ERROR("DALI_KEY_ESCAPE:%d, DALI_KEY_BACK:%d\n", IsKey(event, Dali::DALI_KEY_ESCAPE), IsKey(event, Dali::DALI_KEY_BACK));
      mApplication.Quit();
      return;
    }

    if(event.GetKeyName() == "1")
    {
      mUnderlineLabel.ResetUnderline();
      mInputFieldUnderline.ResetUnderline();
    }
    else if(event.GetKeyName() == "2")
    {
      mShadowLabel.ResetShadow();
      mInputFieldShadow.ResetShadow();
    }
    else if(event.GetKeyName() == "3")
    {
      mOutlineLabel.ResetOutline();
      mInputFieldOutline.ResetOutline();
    }
    else if(event.GetKeyName() == "4")
    {
      mLineThroughLabel.ResetLineThrough();
      mInputFieldLineThrough.ResetLineThrough();
    }
  }

private:
  Application& mApplication;
  Label        mWeightLabel;
  Label        mWidthLabel;
  Label        mSlantLabel;
  Label        mUnderlineLabel;
  Label        mShadowLabel;
  Label        mOutlineLabel;
  Label        mLineThroughLabel;
  InputField   mInputFieldUnderline;
  InputField   mInputFieldShadow;
  InputField   mInputFieldOutline;
  InputField   mInputFieldLineThrough;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextController controller(application);
  application.MainLoop();

  return 0;
}
