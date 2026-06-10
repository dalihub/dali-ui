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
    Text::Underline underline;
    underline.SetColor(UiColor(0x0088FF));
    underline.SetThickness(2.0f);
    underline.SetType(Text::Underline::Type::DASHED);
    underline.SetDashLength(4.0f);
    underline.SetDashGap(4.0f);
    mUnderlineLabel.SetUnderline(underline);

    Label shadowMinLabel = CreateBaseLabel("Shadow Label Minimum");
    Text::Shadow shadowMin;
    shadowMin.SetOffset(Vector2(1.0f, 1.0f));
    shadowMinLabel.SetShadow(shadowMin);
    mShadowLabel = CreateBaseLabel("Shadow Label Properties");
    Text::Shadow shadow;
    shadow.SetColor(UiColor(0xFF5500));
    shadow.SetOffset(Vector2(3.0f, 3.0f));
    shadow.SetBlurRadius(2.0f);
    mShadowLabel.SetShadow(shadow);

    Label outlineMinLabel = CreateBaseLabel("Outline Label Minimum");
    Text::Outline outlineMin;
    outlineMin.SetWidth(2.0f);
    outlineMinLabel.SetOutline(outlineMin);
    mOutlineLabel = CreateBaseLabel("Outline Label Properties");
    Text::Outline outline;
    outline.SetColor(UiColor(0x0066FF));
    outline.SetOffset(Vector2(1.0f, 1.0f));
    outline.SetWidth(2.0f);
    outline.SetBlurRadius(1.0f);
    mOutlineLabel.SetOutline(outline);

    Label lineThroughMinLabel = CreateBaseLabel("LineThrough Label Minimum");
    lineThroughMinLabel.SetLineThrough(Text::LineThrough());
    mLineThroughLabel = CreateBaseLabel("LineThrough Label Properties");
    Text::LineThrough lineThrough;
    lineThrough.SetColor(UiColor(0xFF00FF));
    lineThrough.SetThickness(3.0f);
    mLineThroughLabel.SetLineThrough(lineThrough);

    mTextBackgroundColorLabel = CreateBaseLabel("Text Background Color Label");
    mTextBackgroundColorLabel.SetTextBackgroundColor(UiColor(0xFFFF00));

    mInputFieldUnderline = CreateBaseInputField("InputField with Underline");
    Text::Underline inputFieldUnderline;
    inputFieldUnderline.SetColor(UiColor(0x0088FF));
    inputFieldUnderline.SetThickness(2.0f);
    mInputFieldUnderline.SetUnderline(inputFieldUnderline);
    mInputFieldShadow = CreateBaseInputField("InputField with Shadow");
    Text::Shadow inputFieldShadow;
    inputFieldShadow.SetColor(UiColor(0xFF5500));
    inputFieldShadow.SetOffset(Vector2(2.0f, 2.0f));
    mInputFieldShadow.SetShadow(inputFieldShadow);
    mInputFieldOutline = CreateBaseInputField("InputField with Outline");
    Text::Outline inputFieldOutline;
    inputFieldOutline.SetColor(UiColor(0x0066FF));
    inputFieldOutline.SetWidth(2.0f);
    mInputFieldOutline.SetOutline(inputFieldOutline);
    mInputFieldLineThrough = CreateBaseInputField("InputField with LineThrough");
    Text::LineThrough inputFieldLineThrough;
    inputFieldLineThrough.SetColor(UiColor(0xFF00FF));
    inputFieldLineThrough.SetThickness(2.0f);
    mInputFieldLineThrough.SetLineThrough(inputFieldLineThrough);
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
      Text::Underline underline;
      underline.SetColor(UiColor(0xFF0000));
      underline.SetThickness(4.0f);
      underline.SetType(Text::Underline::Type::DASHED);
      underline.SetDashLength(8.0f);
      underline.SetDashGap(2.0f);
      mUnderlineLabel.SetUnderline(underline);

      Text::Shadow shadow;
      shadow.SetColor(UiColor(0x00FF00));
      shadow.SetOffset(Vector2(-2.0f, -2.0f));
      shadow.SetBlurRadius(4.0f);
      mShadowLabel.SetShadow(shadow);

      Text::Outline outline;
      outline.SetColor(UiColor(0xFF00FF));
      outline.SetOffset(Vector2(-1.0f, -1.0f));
      outline.SetWidth(3.0f);
      outline.SetBlurRadius(2.0f);
      mOutlineLabel.SetOutline(outline);

      Text::LineThrough lineThrough;
      lineThrough.SetColor(UiColor(0x00FFFF));
      lineThrough.SetThickness(1.0f);
      mLineThroughLabel.SetLineThrough(lineThrough);
    }
    else if(event.GetKeyName() == "6")
    {
      Text::Underline underline;
      underline.SetColor(UiColor(0xFF0000));
      underline.SetThickness(4.0f);
      underline.SetType(Text::Underline::Type::DASHED);
      underline.SetDashLength(8.0f);
      underline.SetDashGap(2.0f);
      mInputFieldUnderline.SetUnderline(underline);

      Text::Shadow shadow;
      shadow.SetColor(UiColor(0x00FF00));
      shadow.SetOffset(Vector2(-2.0f, -2.0f));
      mInputFieldShadow.SetShadow(shadow);

      Text::Outline outline;
      outline.SetColor(UiColor(0xFF00FF));
      outline.SetWidth(3.0f);
      mInputFieldOutline.SetOutline(outline);

      Text::LineThrough lineThrough;
      lineThrough.SetColor(UiColor(0x00FFFF));
      lineThrough.SetThickness(1.0f);
      mInputFieldLineThrough.SetLineThrough(lineThrough);
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
