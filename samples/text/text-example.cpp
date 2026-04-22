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
#include <dali-ui-foundation/devel-api/ui-foundation-pre-initialize.h>

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
  void OnInit(Application& application)
  {
    DALI_LOG_ERROR("Application OnInit\n");

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
      DALI_LOG_ERROR("OnAsyncRenderFinished: %s\n", label.GetText().CStr());
      DALI_LOG_ERROR("size:%f, %f, line count:%d\n", width, height, label.GetAsyncLineCount());
    }
  }

  void OnAsyncNaturalSize(View view, float width, float height)
  {
    Label label = Label::DownCast(view);
    if(label)
    {
      DALI_LOG_ERROR("OnAsyncNaturalSize: %s\n", label.GetText().CStr());
      DALI_LOG_ERROR("size:%f, %f, line count:%d\n", width, height, label.GetAsyncLineCount());
    }
  }

  void OnAsyncHeightForWidth(View view, float width, float height)
  {
    Label label = Label::DownCast(view);
    if(label)
    {
      DALI_LOG_ERROR("OnAsyncHeightForWidth: %s\n", label.GetText().CStr());
      DALI_LOG_ERROR("size:%f, %f, line count:%d\n", width, height, label.GetAsyncLineCount());
    }
  }

  void OnRelayout(Actor actor)
  {
    Label label = Label::DownCast(actor);
    if(label)
    {
      DALI_LOG_ERROR("[%s]\n", label.GetText().CStr());
      DALI_LOG_ERROR("  -> LineCount: %d, Width: %.2f, Min width: %.2f, Max width: %.2f\n", label.GetLineCount(), label.GetSize().GetWidth(), label.GetMinimumWidth(), label.GetMaximumWidth());
    }
  }

  void OnTextChanged(View view)
  {
    InputField field = InputField::DownCast(view);
    if(field)
    {
      DALI_LOG_ERROR("OnTextChanged: %s\n", field.GetText().CStr());
    }
  }

  void OnMaximumLengthReached(View view)
  {
    InputField field = InputField::DownCast(view);
    if(field)
    {
      DALI_LOG_ERROR("OnMaximumLengthReached, length: %zu\n", field.GetText().Size());
    }
  }

  View CreateContents()
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetSpacing(STACK_SPACING)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
      .Children({
        Label::New("Text Example"),
        CreateInputField().As(mField),
        CreateLayoutLabel1().As(mLabel),
        CreateLayoutLabel2().As(mLabel2),
        CreateLayoutLabel3().As(mLabel3),
        CreateLineHeightLabel("LineHeight:2, LineHeightMode:Relative", 2, Text::LineHeightMode::RELATIVE).As(mLineHeightLabel),
        CreateLineHeightLabel("LineHeight:50, LineHeightMode:Absolute", 50, Text::LineHeightMode::ABSOLUTE).As(mLineHeightLabel2),
        CreateSeparator(),
        CreateAlignmentLabel(Text::Alignment::START),
        CreateAlignmentLabel(Text::Alignment::CENTER),
        CreateAlignmentLabel(Text::Alignment::END),
      });
  }

  Label CreateLabel(const Dali::String& text, const Dali::String& fontFamily, float fontSize)
  {
    return Label::New()
      .SetText(text)
      .SetFontFamily(fontFamily)
      .SetFontSize(fontSize)
      .SetOverflowMode(Text::OverflowMode::ELLIPSIS);
  }

  InputField CreateInputField()
  {
    return InputField::New()
      .SetPlaceholder("Enter your text here")
      .SetPlaceholderColor(UiColor(COLOR_DARK_GRAY))
      .SetFontSize(INPUT_FONT_SIZE)
      .SetCursorWidth(CURSOR_WIDTH)
      .SetSelectionColor(UiColor(COLOR_LIGHT_BLUE))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetPadding(Extents(20, 20, 20, 20))
      .SetBackgroundColor(UiColor(COLOR_LIGHT_BG))
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetCursorColor(UiColor(COLOR_DARK_TEXT))
      .SetVerticalTextAlignment(Text::Alignment::CENTER);
  }

  Label CreateLayoutLabel1()
  {
    // Verify wrap-content width with fixed height
    return CreateLabel("Hello world", "SamsungOneUI_400", LABEL1_FONT_SIZE)
      .SetRequestedWidth(WRAP_CONTENT)
      .SetRequestedHeight(100.0f)
      .SetPadding(Extents(10, 10, 10, 10))
      .SetBackgroundColor(UiColor(COLOR_LABEL_LIGHT1))
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER);
  }

  Label CreateLayoutLabel2()
  {
    // Verify min/max size and padding with wrap-content (width & height)
    return CreateLabel("Hello world, this is a multi-line enabled long long text", "Ubuntu Mono", LABEL2_FONT_SIZE)
      .SetMinimumWidth(100)
      .SetMinimumHeight(100)
      .SetMaximumWidth(600)
      .SetMaximumHeight(300)
      .SetPadding(Extents(20, 20, 20, 20))
      .SetBackgroundColor(UiColor(COLOR_LABEL_LIGHT2))
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetMultiLine(true)
      .SetLineWrapMode(Text::LineWrapMode::WORD);
  }

  Label CreateLayoutLabel3()
  {
    // Verify match-parent width with fixed height
    return CreateLabel("변화는 한 순간에 일어나지 않습니다. 매일의 작은 실천이 모여 지속가능한 삶을 이루는 것. 이것이 우리가 꿈꾸는 지속가능성입니다.",
                       "SamsungOneUI_700",
                       LABEL3_FONT_SIZE)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(150.0f)
      .SetPadding(Extents(20, 20, 20, 20))
      .SetBackgroundColor(UiColor(COLOR_LABEL_LIGHT3))
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetMultiLine(true)
      .SetLineWrapMode(Text::LineWrapMode::WORD);
  }

  Label CreateLineHeightLabel(Dali::String text, float lineHeight, Text::LineHeightMode mode)
  {
    return CreateLabel(text, "SamsungOneUI_400", 20)
      .SetRequestedWidth(MATCH_PARENT)
      .SetBackgroundColor(UiColor(0xEFEFEF))
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetLineHeight(lineHeight)
      .SetLineHeightMode(mode);
  }

  View CreateSeparator()
  {
    return View::New()
      .SetBackgroundColor(UiColor(COLOR_BLUE))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(4.0f);
  }

  Label CreateAlignmentLabel(Text::Alignment alignment)
  {
    return Label::New("Label Alignment")
      .SetBackgroundColor(UiColor(COLOR_MID_GRAY))
      .SetTextColor(UiColor(COLOR_WHITE))
      .SetFontSize(10.0f)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(30.0f)
      .SetHorizontalTextAlignment(alignment)
      .SetVerticalTextAlignment(alignment);
  }

  void PrintLabelInfo(Label label, const char* title)
  {
    Vector4 textColor = label.GetTextColor().Resolve();

    DALI_LOG_ERROR("----------------------------------------------------------------\n");
    DALI_LOG_ERROR("%s\n", title);
    DALI_LOG_ERROR("Text            : %s\n", label.GetText().CStr());
    DALI_LOG_ERROR("FontFamily      : %s\n", label.GetFontFamily().CStr());
    DALI_LOG_ERROR("FontSize        : %f\n", label.GetFontSize());
    DALI_LOG_ERROR("MultiLine       : %d\n", label.IsMultiLine());
    DALI_LOG_ERROR("LineWrapMode    : %d\n", label.GetLineWrapMode());
    DALI_LOG_ERROR("TextColor       : %.2f, %.2f, %.2f, %.2f\n", textColor.r, textColor.g, textColor.b, textColor.a);
    DALI_LOG_ERROR("H Align         : %d\n", label.GetHorizontalTextAlignment());
    DALI_LOG_ERROR("V Align         : %d\n", label.GetVerticalTextAlignment());
    DALI_LOG_ERROR("Overflow Mode   : %d\n", label.GetOverflowMode());
    DALI_LOG_ERROR("LineHeight      : %f\n", label.GetLineHeight());
    DALI_LOG_ERROR("LineHeight Mode : %d\n", label.GetLineHeightMode());
    DALI_LOG_ERROR("LayoutDir Mode  : %d\n", label.GetLayoutDirectionMode());
    DALI_LOG_ERROR("Natural         : %f, %f\n", label.GetNaturalSize().x, label.GetNaturalSize().y);
    DALI_LOG_ERROR("H for W         : 100, %f\n", label.GetHeightForWidth(100));
    DALI_LOG_ERROR("----------------------------------------------------------------\n");
  }

  void PrintInputFieldInfo(InputField field, const char* title)
  {
    Vector4 textColor = field.GetTextColor().Resolve();
    Vector4 placeholderColor = field.GetPlaceholderColor().Resolve();
    Vector4 cursorColor = field.GetCursorColor().Resolve();
    Vector4 selectionColor = field.GetSelectionColor().Resolve();

    DALI_LOG_ERROR("----------------------------------------------------------------\n");
    DALI_LOG_ERROR("%s\n", title);
    DALI_LOG_ERROR("Text             : %s\n", field.GetText().CStr());
    DALI_LOG_ERROR("FontFamily       : %s\n", field.GetFontFamily().CStr());
    DALI_LOG_ERROR("FontSize         : %f\n", field.GetFontSize());
    DALI_LOG_ERROR("TextColor        : %.2f, %.2f, %.2f, %.2f\n", textColor.r, textColor.g, textColor.b, textColor.a);
    DALI_LOG_ERROR("H Align          : %d\n", field.GetHorizontalTextAlignment());
    DALI_LOG_ERROR("V Align          : %d\n", field.GetVerticalTextAlignment());
    DALI_LOG_ERROR("Placeholder      : %s\n", field.GetPlaceholder().CStr());
    DALI_LOG_ERROR("PlaceholderColor : %.2f, %.2f, %.2f, %.2f\n", placeholderColor.r, placeholderColor.g, placeholderColor.b, placeholderColor.a);
    DALI_LOG_ERROR("CursorWidth      : %d\n", field.GetCursorWidth());
    DALI_LOG_ERROR("CursorColor      : %.2f, %.2f, %.2f, %.2f\n", cursorColor.r, cursorColor.g, cursorColor.b, cursorColor.a);
    DALI_LOG_ERROR("SelectionColor   : %.2f, %.2f, %.2f, %.2f\n", selectionColor.r, selectionColor.g, selectionColor.b, selectionColor.a);
    DALI_LOG_ERROR("MaximumLength    : %d\n", field.GetMaximumLength());
    DALI_LOG_ERROR("Natural          : %f, %f\n", field.GetNaturalSize().x, field.GetNaturalSize().y);
    DALI_LOG_ERROR("----------------------------------------------------------------\n");
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
    const bool isLightColor = (mLabel.GetTextColor().Resolve() == UiColor(COLOR_DARK_TEXT));
    if(isLightColor)
    {
      ApplyDarkColor();
      mLabel3.SetOverflowMode(Text::OverflowMode::CLIP);
      mField.SetOverflowMode(Text::OverflowMode::ELLIPSIS);
    }
    else
    {
      ApplyLightColor();
      mLabel3.SetOverflowMode(Text::OverflowMode::ELLIPSIS);
      mField.SetOverflowMode(Text::OverflowMode::CLIP);
    }
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
      ToggleColor();
    }
    else if(event.GetKeyName() == "2")
    {
      mLabel.SetAsyncRendering(true);
      mLabel2.SetAsyncRendering(true);
      mLabel3.SetAsyncRendering(true);
      mLineHeightLabel.SetAsyncRendering(true);
      mLineHeightLabel2.SetAsyncRendering(true);
      DALI_LOG_ERROR("AsyncRendering\n");
    }
    else if(event.GetKeyName() == "3")
    {
      mLabel.SetAsyncRendering(false);
      mLabel2.SetAsyncRendering(false);
      mLabel3.SetAsyncRendering(false);
      mLineHeightLabel.SetAsyncRendering(false);
      mLineHeightLabel2.SetAsyncRendering(false);
      DALI_LOG_ERROR("SyncRendering\n");
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
  DALI_LOG_ERROR("DaliUiFoundationPreInitialize START\n");
  DaliUiFoundationPreInitialize(nullptr, nullptr, nullptr);
  DALI_LOG_ERROR("DaliUiFoundationPreInitialize END\n");

  Application application = Application::New(&argc, &argv);
  UiConfig::New()
    .SetLabelAsyncRendering(true)
    .SetDefaultPlaceholderTextColor(Color::DARK_GRAY)
    .SetShowPlaceholderTextOnFocus(false)
    .Apply();

  TextController controller(application);
  application.MainLoop();

  return 0;
}
