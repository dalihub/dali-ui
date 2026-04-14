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
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xFFFFFF));

    window.Add(
      StackLayout::New(StackOrientation::VERTICAL)
        .Spacing(STACK_SPACING)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(MATCH_PARENT)
        .SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
        .Children({
          Label::New("Text Style Example"),

          // Font styles
          CreateBaseLabel("Italic Lorem ipsum dolor sit amet.")
            .SetFontSlant(Text::FontSlant::ITALIC)
            .As(mSlantLabel),
          CreateBaseLabel("Bold Lorem ipsum dolor sit amet.")
            .SetFontWeight(Text::FontWeight::BOLD)
            .As(mWeightLabel),
          CreateBaseLabel("Semi condensed Lorem ipsum dolor sit amet.")
            .SetFontWidth(Text::FontWidth::SEMI_CONDENSED)
            .As(mWidthLabel),

          // Underline
          CreateBaseLabel("Underline Label Minimum")
            .SetUnderline(Text::Underline()),
          CreateBaseLabel("Dashed Underline Label")
            .SetUnderline(
              Text::Underline()
                .SetColor(UiColor(0x0088FF))
                .SetThickness(2.0f)
                .SetType(Text::Underline::Type::DASHED)
                .SetDashLength(4.0f)
                .SetDashGap(4.0f))
            .As(mUnderlineLabel),

          // Shadow
          CreateBaseLabel("Shadow Label Minimum")
            .SetShadow(
              Text::Shadow()
                .SetOffset(Vector2(1.0f, 1.0f))),
          CreateBaseLabel("Shadow Label Properties")
            .SetShadow(
              Text::Shadow()
                .SetColor(UiColor(0xFF5500))
                .SetOffset(Vector2(3.0f, 3.0f))
                .SetBlurRadius(2.0f))
            .As(mShadowLabel),

          // Outline
          CreateBaseLabel("Outline Label Minimum")
            .SetOutline(
              Text::Outline()
                .SetWidth(2.0f)),
          CreateBaseLabel("Outline Label Properties")
            .SetOutline(
              Text::Outline()
                .SetColor(UiColor(0x0066FF))
                .SetOffset(Vector2(1.0f, 1.0f))
                .SetWidth(2.0f)
                .SetBlurRadius(1.0f))
            .As(mOutlineLabel),

          // LineThrough
          CreateBaseLabel("LineThrough Label Minimum")
            .SetLineThrough(Text::LineThrough()),
          CreateBaseLabel("LineThrough Label Properties")
            .SetLineThrough(
              Text::LineThrough()
                .SetColor(UiColor(0xFF00FF))
                .SetThickness(3.0f))
            .As(mLineThroughLabel),

          // Text background color
          CreateBaseLabel("Text Background Color Label")
            .SetTextBackgroundColor(UiColor(0xFFFF00))
            .As(mTextBackgroundColorLabel),

          // InputField styles
          Label::New("InputField with Styles:"),

          CreateBaseInputField("InputField with Underline")
            .SetUnderline(
              Text::Underline()
                .SetColor(UiColor(0x0088FF))
                .SetThickness(2.0f))
            .As(mInputFieldUnderline),
          CreateBaseInputField("InputField with Shadow")
            .SetShadow(
              Text::Shadow()
                .SetColor(UiColor(0xFF5500))
                .SetOffset(Vector2(2.0f, 2.0f)))
            .As(mInputFieldShadow),
          CreateBaseInputField("InputField with Outline")
            .SetOutline(
              Text::Outline()
                .SetColor(UiColor(0x0066FF))
                .SetWidth(2.0f))
            .As(mInputFieldOutline),
          CreateBaseInputField("InputField with LineThrough")
            .SetLineThrough(
              Text::LineThrough()
                .SetColor(UiColor(0xFF00FF))
                .SetThickness(2.0f))
            .As(mInputFieldLineThrough),
          CreateBaseInputField("InputField Text Background Color")
            .SetTextBackgroundColor(UiColor(0x00FFFF))
            .As(mInputFieldTextBackgroundColor),
        }));
    

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
    return Label::New(text)
      .SetBackgroundColor(BACKGROUND_COLOR)
      .SetPadding(Extents(ITEM_PADDING, ITEM_PADDING, ITEM_PADDING, ITEM_PADDING))
      .SetFontSize(FONT_SIZE)
      .SetRequestedWidth(MATCH_PARENT);
  }

  InputField CreateBaseInputField(const char* text)
  {
    return InputField::New()
      .SetText(text)
      .SetBackgroundColor(BACKGROUND_COLOR)
      .SetPadding(Extents(ITEM_PADDING, ITEM_PADDING, ITEM_PADDING, ITEM_PADDING))
      .SetFontSize(FONT_SIZE)
      .SetRequestedWidth(MATCH_PARENT);
  }

  void PlayAnimation(Label label, Vector4 startColor, Vector4 endColor)
  {
    label.SetProperty(Label::Property::TEXT_COLOR, startColor);

    Animation animation = Animation::New(1.0f);
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

  void PrintTextBackgroundColorInfo(Label label, const char* title)
  {
    const Vector4 color = label.GetTextBackgroundColor().Resolve();
    DALI_LOG_ERROR("%s - R:%f G:%f B:%f A:%f\n", title, color.r, color.g, color.b, color.a);
  }

  void PrintTextBackgroundColorInfo(InputField inputField, const char* title)
  {
    const Vector4 color = inputField.GetTextBackgroundColor().Resolve();
    DALI_LOG_ERROR("%s - R:%f G:%f B:%f A:%f\n", title, color.r, color.g, color.b, color.a);
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      DALI_LOG_ERROR("DALI_KEY_ESCAPE:%d, DALI_KEY_BACK:%d\n",
                     IsKey(event, Dali::DALI_KEY_ESCAPE),
                     IsKey(event, Dali::DALI_KEY_BACK));
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

      DALI_LOG_ERROR("Changed Label styles to alternate form\n");
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

      DALI_LOG_ERROR("Changed InputField styles to alternate form\n");
    }
    else if(event.GetKeyName() == "7")
    {
      mTextBackgroundColorLabel.ClearTextBackgroundColor();
      mInputFieldTextBackgroundColor.ClearTextBackgroundColor();
      DALI_LOG_ERROR("Cleared TextBackgroundColor\n");

      PrintTextBackgroundColorInfo(mTextBackgroundColorLabel, "TextBackgroundColor Label");
      PrintTextBackgroundColorInfo(mInputFieldTextBackgroundColor, "TextBackgroundColor InputField");
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
