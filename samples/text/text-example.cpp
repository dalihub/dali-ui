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

constexpr float INPUT_FONT_SIZE = 20.0f;
constexpr int   CURSOR_WIDTH    = 2;

constexpr float LABEL1_FONT_SIZE = 40.0f;
constexpr float LABEL2_FONT_SIZE = 30.0f;
constexpr float LABEL3_FONT_SIZE = 20.0f;

constexpr uint32_t COLOR_WHITE        = 0xFFFFFF;
constexpr uint32_t COLOR_BLACK        = 0x000000;
constexpr uint32_t COLOR_LIGHT_TEXT   = 0xF5F5F5;
constexpr uint32_t COLOR_DARK_TEXT    = 0x222222;
constexpr uint32_t COLOR_LIGHT_BG     = 0xF2F2F2;
constexpr uint32_t COLOR_DARK_BG      = 0x1E1E1E;
constexpr uint32_t COLOR_MID_GRAY     = 0x808080;
constexpr uint32_t COLOR_DARK_GRAY    = 0x404040;
constexpr uint32_t COLOR_LIGHT_BLUE   = 0xADD8E6;
constexpr uint32_t COLOR_BLUE         = 0x0000FF;
constexpr uint32_t COLOR_LABEL_DARK1  = 0x2C2C2C;
constexpr uint32_t COLOR_LABEL_DARK2  = 0x3A2A2A;
constexpr uint32_t COLOR_LABEL_DARK3  = 0x1F2A38;
constexpr uint32_t COLOR_LABEL_LIGHT1 = 0xFFE8E8;
constexpr uint32_t COLOR_LABEL_LIGHT2 = 0xF0F0F0;
constexpr uint32_t COLOR_LABEL_LIGHT3 = 0xEAF4FF;
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
    window.SetBackgroundColor(UiColor(COLOR_WHITE));

    window.Add(CreateContents());

    PrintLabelInfo(mLabel, "Label 1");
    PrintLabelInfo(mLabel2, "Label 2");
    PrintLabelInfo(mLabel3, "Label 3");
    PrintLabelInfo(mLineHeightLabel, "Label Relative Line Height");
    PrintLabelInfo(mLineHeightLabel2, "Label Absolute Line Height");
    PrintInputFieldInfo(mField, "InputField");


    mLabel2.OnRelayoutSignal().Connect(this, &TextController::OnRelayout);
    mLabel3.OnRelayoutSignal().Connect(this, &TextController::OnRelayout);

    mField.TextChangedSignal().Connect(this, &TextController::OnTextChanged);
    mField.SetMaximumLength(20);
    mField.MaximumLengthReachedSignal().Connect(this, &TextController::OnMaximumLengthReached);

    mLabel.AsyncRenderFinishedSignal().Connect(this, &TextController::OnAsyncRenderFinished);
    mLabel2.AsyncRenderFinishedSignal().Connect(this, &TextController::OnAsyncRenderFinished);
    mLabel3.AsyncRenderFinishedSignal().Connect(this, &TextController::OnAsyncRenderFinished);

    mLabel2.AsyncNaturalSizeComputedSignal().Connect(this, &TextController::OnAsyncNaturalSize);
    mLabel3.AsyncNaturalSizeComputedSignal().Connect(this, &TextController::OnAsyncNaturalSize);

    mLabel2.AsyncHeightForWidthComputedSignal().Connect(this, &TextController::OnAsyncHeightForWidth);
    mLabel3.AsyncHeightForWidthComputedSignal().Connect(this, &TextController::OnAsyncHeightForWidth);

    window.KeyEventSignal().Connect(this, &TextController::OnKeyEvent);
  }

  void OnAsyncRenderFinished(View view, float width, float height)
  {
    Label label = Label::DownCast(view);
    if(label)
    {
    }
  }

  void OnAsyncNaturalSize(View view, float width, float height)
  {
    Label label = Label::DownCast(view);
    if(label)
    {
    }
  }

  void OnAsyncHeightForWidth(View view, float width, float height)
  {
    Label label = Label::DownCast(view);
    if(label)
    {
    }
  }

  void OnRelayout(Actor actor)
  {
    Label label = Label::DownCast(actor);
    if(label)
    {
    }
  }

  void OnTextChanged(View view)
  {
    InputField field = InputField::DownCast(view);
    if(field)
    {
    }
  }

  void OnMaximumLengthReached(View view)
  {
    InputField field = InputField::DownCast(view);
    if(field)
    {
    }
  }

  View CreateContents()
  {
    StackLayout contents = StackLayout::New(StackOrientation::VERTICAL);
    contents.SetSpacing(STACK_SPACING);
    contents.SetRequestedWidth(MATCH_PARENT);
    contents.SetRequestedHeight(MATCH_PARENT);
    contents.SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING));

    mField            = CreateInputField();
    mLabel            = CreateLayoutLabel1();
    mLabel2           = CreateLayoutLabel2();
    mLabel3           = CreateLayoutLabel3();
    mLineHeightLabel  = CreateLineHeightLabel("LineHeight:2, LineHeightMode:Relative", 2, Text::LineHeightMode::RELATIVE);
    mLineHeightLabel2 = CreateLineHeightLabel("LineHeight:50, LineHeightMode:Absolute", 50, Text::LineHeightMode::ABSOLUTE);

    contents.AddChildren({
      Label::New("Text Example"),
      mField,
      mLabel,
      mLabel2,
      mLabel3,
      mLineHeightLabel,
      mLineHeightLabel2,
      CreateSeparator(),
      CreateAlignmentLabel(Text::Alignment::START),
      CreateAlignmentLabel(Text::Alignment::CENTER),
      CreateAlignmentLabel(Text::Alignment::END),
    });
    return contents;
  }

  Label CreateLabel(const Dali::String& text, const Dali::String& fontFamily, float fontSize)
  {
    Label label = Label::New();
    label.SetText(text);
    label.SetFontFamily(fontFamily);
    label.SetFontSize(fontSize);
    label.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);
    return label;
  }

  InputField CreateInputField()
  {
    InputField field = InputField::New();
    field.SetPlaceholder("Enter your text here");
    field.SetPlaceholderColor(UiColor(COLOR_DARK_GRAY));
    field.SetFontSize(INPUT_FONT_SIZE);
    field.SetCursorWidth(CURSOR_WIDTH);
    field.SetSelectionColor(UiColor(COLOR_LIGHT_BLUE));
    field.SetRequestedWidth(MATCH_PARENT);
    field.SetRequestedHeight(WRAP_CONTENT);
    field.SetPadding(Extents(20, 20, 20, 20));
    field.SetBackgroundColor(UiColor(COLOR_LIGHT_BG));
    field.SetTextColor(UiColor(COLOR_DARK_TEXT));
    field.SetCursorColor(UiColor(COLOR_DARK_TEXT));
    field.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return field;
  }

  Label CreateLayoutLabel1()
  {
    // Verify wrap-content width with fixed height
    Label label = CreateLabel("Hello world", "SamsungOneUI_400", LABEL1_FONT_SIZE);
    label.SetRequestedWidth(WRAP_CONTENT);
    label.SetRequestedHeight(100.0f);
    label.SetPadding(Extents(10, 10, 10, 10));
    label.SetBackgroundColor(UiColor(COLOR_LABEL_LIGHT1));
    label.SetTextColor(UiColor(COLOR_DARK_TEXT));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return label;
  }

  Label CreateLayoutLabel2()
  {
    // Verify min/max size and padding with wrap-content (width & height)
    Label label = CreateLabel("Hello world, this is a multi-line enabled long long text", "Ubuntu Mono", LABEL2_FONT_SIZE);
    label.SetMinimumWidth(100);
    label.SetMinimumHeight(100);
    label.SetMaximumWidth(600);
    label.SetMaximumHeight(300);
    label.SetPadding(Extents(20, 20, 20, 20));
    label.SetBackgroundColor(UiColor(COLOR_LABEL_LIGHT2));
    label.SetTextColor(UiColor(COLOR_DARK_TEXT));
    label.SetMultiLine(true);
    label.SetLineWrapMode(Text::LineWrapMode::WORD);
    return label;
  }

  Label CreateLayoutLabel3()
  {
    // Verify match-parent width with fixed height
    Label label = CreateLabel("변화는 한 순간에 일어나지 않습니다. 매일의 작은 실천이 모여 지속가능한 삶을 이루는 것. 이것이 우리가 꿈꾸는 지속가능성입니다.",
                              "SamsungOneUI_700",
                              LABEL3_FONT_SIZE);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(150.0f);
    label.SetPadding(Extents(20, 20, 20, 20));
    label.SetBackgroundColor(UiColor(COLOR_LABEL_LIGHT3));
    label.SetTextColor(UiColor(COLOR_DARK_TEXT));
    label.SetMultiLine(true);
    label.SetLineWrapMode(Text::LineWrapMode::WORD);
    return label;
  }

  Label CreateLineHeightLabel(Dali::String text, float lineHeight, Text::LineHeightMode mode)
  {
    Label label = CreateLabel(text, "SamsungOneUI_400", 20);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetBackgroundColor(UiColor(0xEFEFEF));
    label.SetTextColor(UiColor(COLOR_DARK_TEXT));
    label.SetLineHeight(lineHeight);
    label.SetLineHeightMode(mode);
    return label;
  }

  View CreateSeparator()
  {
    View separator = View::New();
    separator.SetBackgroundColor(UiColor(COLOR_BLUE));
    separator.SetRequestedWidth(MATCH_PARENT);
    separator.SetRequestedHeight(4.0f);
    return separator;
  }

  Label CreateAlignmentLabel(Text::Alignment alignment)
  {
    Label label = Label::New("Label Alignment");
    label.SetBackgroundColor(UiColor(COLOR_MID_GRAY));
    label.SetTextColor(UiColor(COLOR_WHITE));
    label.SetFontSize(10.0f);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(30.0f);
    label.SetHorizontalTextAlignment(alignment);
    label.SetVerticalTextAlignment(alignment);
    return label;
  }

  void PrintLabelInfo(Label label, const char* title)
  {
  }

  void PrintInputFieldInfo(InputField field, const char* title)
  {
  }

  void ApplyDarkColor()
  {
    mLabel.SetBackgroundColor(UiColor(COLOR_LABEL_DARK1));
    mLabel.SetTextColor(UiColor(COLOR_LIGHT_TEXT));

    mLabel2.SetBackgroundColor(UiColor(COLOR_LABEL_DARK2));
    mLabel2.SetTextColor(UiColor(COLOR_LIGHT_TEXT));

    mLabel3.SetBackgroundColor(UiColor(COLOR_LABEL_DARK3));
    mLabel3.SetTextColor(UiColor(COLOR_LIGHT_TEXT));

    mField.SetBackgroundColor(UiColor(COLOR_DARK_BG));
    mField.SetTextColor(UiColor(COLOR_LIGHT_TEXT));
    mField.SetCursorColor(UiColor(COLOR_LIGHT_TEXT));
    mField.SetPlaceholderColor(UiColor(0xA0A0A0));
  }

  void ApplyLightColor()
  {
    mLabel.SetBackgroundColor(UiColor(COLOR_LABEL_LIGHT1));
    mLabel.SetTextColor(UiColor(COLOR_DARK_TEXT));

    mLabel2.SetBackgroundColor(UiColor(COLOR_LABEL_LIGHT2));
    mLabel2.SetTextColor(UiColor(COLOR_DARK_TEXT));

    mLabel3.SetBackgroundColor(UiColor(COLOR_LABEL_LIGHT3));
    mLabel3.SetTextColor(UiColor(COLOR_DARK_TEXT));

    mField.SetBackgroundColor(UiColor(COLOR_LIGHT_BG));
    mField.SetTextColor(UiColor(COLOR_DARK_TEXT));
    mField.SetCursorColor(UiColor(COLOR_DARK_TEXT));
    mField.SetPlaceholderColor(UiColor(COLOR_DARK_GRAY));
  }

  void ToggleColor()
  {
    const bool isLightColor = (mLabel.GetTextColor().GetRgba() == UiColor(COLOR_DARK_TEXT));
    if(isLightColor)
    {
      ApplyDarkColor();
      mLabel3.SetTextOverflowMode(Text::OverflowMode::CLIP);
      mField.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);
    }
    else
    {
      ApplyLightColor();
      mLabel3.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);
      mField.SetTextOverflowMode(Text::OverflowMode::CLIP);
    }
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
      ToggleColor();
    }
    else if(event.GetKeyName() == "2")
    {
      mLabel.SetAsyncRendering(true);
      mLabel2.SetAsyncRendering(true);
      mLabel3.SetAsyncRendering(true);
      mLineHeightLabel.SetAsyncRendering(true);
      mLineHeightLabel2.SetAsyncRendering(true);
    }
    else if(event.GetKeyName() == "3")
    {
      mLabel.SetAsyncRendering(false);
      mLabel2.SetAsyncRendering(false);
      mLabel3.SetAsyncRendering(false);
      mLineHeightLabel.SetAsyncRendering(false);
      mLineHeightLabel2.SetAsyncRendering(false);
    }
    else if(event.GetKeyName() == "4")
    {
      mLabel2.RequestAsyncNaturalSize();
      mLabel3.RequestAsyncNaturalSize();
    }
    else if(event.GetKeyName() == "5")
    {
      mLabel2.RequestAsyncHeightForWidth(220);
      mLabel3.RequestAsyncHeightForWidth(220);
    }
    else if(event.GetKeyName() == "6")
    {
      mLabel2.RequestAsyncHeightForWidth(440);
      mLabel3.RequestAsyncHeightForWidth(440);
    }
    else if(event.GetKeyName() == "7")
    {
      mLabel2.SetPadding(Extents(0, 0, 0, 0));
      mLabel3.SetPadding(Extents(0, 0, 0, 0));
    }
    else if(event.GetKeyName() == "8")
    {
      mLabel2.SetPadding(Extents(20, 20, 20, 20));
      mLabel3.SetPadding(Extents(20, 20, 20, 20));
    }

    else if(event.GetKeyName() == "q")
    {
      UiScaleManager::Get().SetScale(0.8f);
    }
    else if(event.GetKeyName() == "w")
    {
      UiScaleManager::Get().SetScale(1.0f);
    }
    else if(event.GetKeyName() == "e")
    {
      UiScaleManager::Get().SetScale(1.2f);
    }
    else if(event.GetKeyName() == "r")
    {
      UiScaleManager::Get().SetScale(1.5f);
    }
    else if(event.GetKeyName() == "t")
    {
      UiScaleManager::Get().SetScale(2.0f);
    }
  }

private:
  Application& mApplication;
  Label        mLabel;
  Label        mLabel2;
  Label        mLabel3;
  Label        mLineHeightLabel;
  Label        mLineHeightLabel2;
  InputField   mField;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig config = UiConfig::New();
  config.SetLabelAsyncRendering(true);
  config.SetDefaultPlaceholderTextColor(Color::DARK_GRAY);
  config.SetShowPlaceholderTextOnFocus(false);
  config.Apply();

  TextController controller(application);
  application.MainLoop();

  return 0;
}
