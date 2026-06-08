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
constexpr float ITEM_PADDING  = 10.0f;
constexpr float FONT_SIZE     = 20.0f;

const UiColor BACKGROUND_COLOR(0xefefef);
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

    mSlantLabel = CreateBaseLabel("Italic Lorem ipsum dolor sit amet.");
    mSlantLabel.SetFontSlant(Text::FontSlant::ITALIC);
    mWeightLabel = CreateBaseLabel("Bold Lorem ipsum dolor sit amet.");
    mWeightLabel.SetFontWeight(Text::FontWeight::BOLD);
    mWidthLabel = CreateBaseLabel("Semi condensed Lorem ipsum dolor sit amet.");
    mWidthLabel.SetFontWidth(Text::FontWidth::SEMI_CONDENSED);

    Label underlineMinLabel = CreateBaseLabel("Underline Label Minimum");
    underlineMinLabel.SetUnderline(Text::Underline());
    mUnderlineLabel = CreateBaseLabel("Dashed Underline Label");
    mUnderlineLabel.SetUnderline(Text::Underline()
                                   .SetColor(UiColor(0x0088FF))
                                   .SetThickness(2.0f)
                                   .SetType(Text::Underline::Type::DASHED)
                                   .SetDashLength(4.0f)
                                   .SetDashGap(4.0f));

    Label shadowMinLabel = CreateBaseLabel("Shadow Label Minimum");
    shadowMinLabel.SetShadow(Text::Shadow().SetOffset(Vector2(1.0f, 1.0f)));
    mShadowLabel = CreateBaseLabel("Shadow Label Properties");
    mShadowLabel.SetShadow(Text::Shadow()
                             .SetColor(UiColor(0xFF5500))
                             .SetOffset(Vector2(3.0f, 3.0f))
                             .SetBlurRadius(2.0f));

    Label outlineMinLabel = CreateBaseLabel("Outline Label Minimum");
    outlineMinLabel.SetOutline(Text::Outline().SetWidth(2.0f));
    mOutlineLabel = CreateBaseLabel("Outline Label Properties");
    mOutlineLabel.SetOutline(Text::Outline()
                               .SetColor(UiColor(0x0066FF))
                               .SetOffset(Vector2(1.0f, 1.0f))
                               .SetWidth(2.0f)
                               .SetBlurRadius(1.0f));

    Label lineThroughMinLabel = CreateBaseLabel("LineThrough Label Minimum");
    lineThroughMinLabel.SetLineThrough(Text::LineThrough());
    mLineThroughLabel = CreateBaseLabel("LineThrough Label Properties");
    mLineThroughLabel.SetLineThrough(Text::LineThrough()
                                       .SetColor(UiColor(0xFF00FF))
                                       .SetThickness(3.0f));

    mTextBackgroundColorLabel = CreateBaseLabel("Text Background Color Label");
    mTextBackgroundColorLabel.SetTextBackgroundColor(UiColor(0xFFFF00));

    mInputFieldUnderline = CreateBaseInputField("InputField with Underline");
    mInputFieldUnderline.SetUnderline(Text::Underline()
                                        .SetColor(UiColor(0x0088FF))
                                        .SetThickness(2.0f));
    mInputFieldShadow = CreateBaseInputField("InputField with Shadow");
    mInputFieldShadow.SetShadow(Text::Shadow()
                                  .SetColor(UiColor(0xFF5500))
                                  .SetOffset(Vector2(2.0f, 2.0f)));
    mInputFieldOutline = CreateBaseInputField("InputField with Outline");
    mInputFieldOutline.SetOutline(Text::Outline()
                                    .SetColor(UiColor(0x0066FF))
                                    .SetWidth(2.0f));
    mInputFieldLineThrough = CreateBaseInputField("InputField with LineThrough");
    mInputFieldLineThrough.SetLineThrough(Text::LineThrough()
                                            .SetColor(UiColor(0xFF00FF))
                                            .SetThickness(2.0f));
    mInputFieldTextBackgroundColor = CreateBaseInputField("InputField Text Background Color");
    mInputFieldTextBackgroundColor.SetTextBackgroundColor(UiColor(0x00FFFF));

    root.AddChildren({
      Label::New("Text Style Example"),
      mSlantLabel,
      mWeightLabel,
      mWidthLabel,
      underlineMinLabel,
      mUnderlineLabel,
      shadowMinLabel,
      mShadowLabel,
      outlineMinLabel,
      mOutlineLabel,
      lineThroughMinLabel,
      mLineThroughLabel,
      mTextBackgroundColorLabel,
      Label::New("InputField with Styles:"),
      mInputFieldUnderline,
      mInputFieldShadow,
      mInputFieldOutline,
      mInputFieldLineThrough,
      mInputFieldTextBackgroundColor,
    });
    window.Add(root);


    PrintTextBackgroundColorInfo(mTextBackgroundColorLabel, "TextBackgroundColor Label");
    PrintTextBackgroundColorInfo(mInputFieldTextBackgroundColor, "TextBackgroundColor InputField");

    PrintLabelInfo(mSlantLabel, "Slant Label");
    PrintLabelInfo(mWeightLabel, "Weight Label");
    PrintLabelInfo(mWidthLabel, "Width Label");

    PlayAnimation(mUnderlineLabel, Dali::Color::RED, Dali::Color::BLUE);

    window.KeyEventSignal().Connect(this, &TextController::OnKeyEvent);
  }

  Label CreateBaseLabel(const char* text)
  {
    Label label = Label::New(text);
    label.SetBackgroundColor(BACKGROUND_COLOR);
    label.SetPadding(Extents(ITEM_PADDING, ITEM_PADDING, ITEM_PADDING, ITEM_PADDING));
    label.SetFontSize(FONT_SIZE);
    label.SetRequestedWidth(MATCH_PARENT);
    return label;
  }

  InputField CreateBaseInputField(const char* text)
  {
    InputField field = InputField::New();
    field.SetText(text);
    field.SetBackgroundColor(BACKGROUND_COLOR);
    field.SetPadding(Extents(ITEM_PADDING, ITEM_PADDING, ITEM_PADDING, ITEM_PADDING));
    field.SetFontSize(FONT_SIZE);
    field.SetRequestedWidth(MATCH_PARENT);
    return field;
  }

  void PlayAnimation(Label label, Vector4 startColor, Vector4 endColor)
  {
    label.SetProperty(Label::Property::TEXT_COLOR, startColor);

    Animation animation = Animation::New();
    label.Animate(animation).TextColor(endColor, 1000_ms, AlphaFunction::SIN);
    animation.SetLoopCount(0);
    animation.Play();
  }

  void PrintLabelInfo(Label label, const char* title)
  {
  }

  void PrintTextBackgroundColorInfo(Label label, const char* title)
  {
  }

  void PrintTextBackgroundColorInfo(InputField inputField, const char* title)
  {
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
      mUnderlineLabel.ClearUnderline();
      mInputFieldUnderline.ClearUnderline();
    }
    else if(event.GetKeyName() == "2")
    {
      mShadowLabel.ClearShadow();
      mInputFieldShadow.ClearShadow();
    }
    else if(event.GetKeyName() == "3")
    {
      mOutlineLabel.ClearOutline();
      mInputFieldOutline.ClearOutline();
    }
    else if(event.GetKeyName() == "4")
    {
      mLineThroughLabel.ClearLineThrough();
      mInputFieldLineThrough.ClearLineThrough();
    }
    else if(event.GetKeyName() == "5")
    {
      mUnderlineLabel.SetUnderline(
        Text::Underline()
          .SetColor(UiColor(0xFF0000))
          .SetThickness(4.0f)
          .SetType(Text::Underline::Type::DASHED)
          .SetDashLength(8.0f)
          .SetDashGap(2.0f));

      mShadowLabel.SetShadow(
        Text::Shadow()
          .SetColor(UiColor(0x00FF00))
          .SetOffset(Vector2(-2.0f, -2.0f))
          .SetBlurRadius(4.0f));

      mOutlineLabel.SetOutline(
        Text::Outline()
          .SetColor(UiColor(0xFF00FF))
          .SetOffset(Vector2(-1.0f, -1.0f))
          .SetWidth(3.0f)
          .SetBlurRadius(2.0f));

      mLineThroughLabel.SetLineThrough(
        Text::LineThrough()
          .SetColor(UiColor(0x00FFFF))
          .SetThickness(1.0f));
    }
    else if(event.GetKeyName() == "6")
    {
      mInputFieldUnderline.SetUnderline(
        Text::Underline()
          .SetColor(UiColor(0xFF0000))
          .SetThickness(4.0f)
          .SetType(Text::Underline::Type::DASHED)
          .SetDashLength(8.0f)
          .SetDashGap(2.0f));

      mInputFieldShadow.SetShadow(
        Text::Shadow()
          .SetColor(UiColor(0x00FF00))
          .SetOffset(Vector2(-2.0f, -2.0f)));

      mInputFieldOutline.SetOutline(
        Text::Outline()
          .SetColor(UiColor(0xFF00FF))
          .SetWidth(3.0f));

      mInputFieldLineThrough.SetLineThrough(
        Text::LineThrough()
          .SetColor(UiColor(0x00FFFF))
          .SetThickness(1.0f));
    }
    else if(event.GetKeyName() == "7")
    {
      mTextBackgroundColorLabel.ClearTextBackgroundColor();
      mInputFieldTextBackgroundColor.ClearTextBackgroundColor();
    }
    else if(event.GetKeyName() == "8")
    {
      mTextBackgroundColorLabel.SetTextBackgroundColor(UiColor(0x00FF00));
      mInputFieldTextBackgroundColor.SetTextBackgroundColor(UiColor(0xFF00FF));

      PrintTextBackgroundColorInfo(mTextBackgroundColorLabel, "TextBackgroundColor Label");
      PrintTextBackgroundColorInfo(mInputFieldTextBackgroundColor, "TextBackgroundColor InputField");
    }
  }

private:
  Application& mApplication;

  Label      mWeightLabel;
  Label      mWidthLabel;
  Label      mSlantLabel;
  Label      mUnderlineLabel;
  Label      mShadowLabel;
  Label      mOutlineLabel;
  Label      mLineThroughLabel;
  Label      mTextBackgroundColorLabel;

  InputField mInputFieldUnderline;
  InputField mInputFieldShadow;
  InputField mInputFieldOutline;
  InputField mInputFieldLineThrough;
  InputField mInputFieldTextBackgroundColor;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextController controller(application);
  application.MainLoop();

  return 0;
}
