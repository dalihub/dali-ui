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
constexpr float INPUT_FIELD_HEIGHT = 64.0f;
constexpr float INPUT_EDITOR_HEIGHT = 170.0f;

constexpr const char* INPUT_FIELD_MARKUP =
  "<color value='red'>Red</color> <font weight='bold' size='22'>Font</font> <i>Italic</i> "
  "<u color='green' height='2' type='dashed' dash-gap='3' dash-width='5'>Underline</u> "
  "<s color='blue' height='3'>Strike</s> <background color='yellow'>BG</background>";

constexpr const char* INPUT_EDITOR_MARKUP =
  "<color value='red'>Red</color> <font family='SamsungOneUI_700' size='24'>Font</font> <b>Bold</b> <i>Italic</i>\n"
  "<u color='green' height='2' type='dashed' dash-gap='3' dash-width='5'>Underline</u> "
  "<s color='blue' height='3'>Strike</s> <background color='yellow'>Background</background>\n"
  "<a href='https://www.samsung.com' color='blue' clicked-color='red'>Anchor color</a> &lt;entity&gt;";
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

    mAnchorLabel      = CreateStyledTextLabel("클릭하면 사이트로 이동 합니다: <a href='https://www.samsung.com'>Samsung Electronics</a>");
    mAnchorColorLabel = CreateStyledTextLabel("색깔이 다른 Anchor: <a href='https://research.samsung.com'>Samsung Research</a>");
    root.Add(Label::New("Text Markup Example"));
    root.Add(CreateStyledTextLabel("<font size='24' family='SamsungOneUI_700'>Markup </font><color value='red'>Text</color>"));
    root.Add(mAnchorLabel);
    root.Add(mAnchorColorLabel);
    root.Add(CreatePlainLabel("InputField markup"));
    root.Add(CreateMarkupInputField(INPUT_FIELD_MARKUP));
    root.Add(CreatePlainLabel("InputEditor markup"));
    root.Add(CreateMarkupInputEditor(INPUT_EDITOR_MARKUP));
    window.Add(root);

    mAnchorColorLabel.SetAnchorColor(UiColor(0xFF0000));
    mAnchorColorLabel.SetAnchorClickedColor(UiColor(0x00FF00));

    PrintLabelInfo(mAnchorLabel, "Anchor Label 1");
    PrintLabelInfo(mAnchorColorLabel, "Anchor Label 2");

    mAnchorLabel.AnchorClickedSignal().Connect(this, &TextController::OnAnchorClicked);
    mAnchorColorLabel.AnchorClickedSignal().Connect(this, &TextController::OnAnchorClicked);

    window.KeyEventSignal().Connect(this, &TextController::OnKeyEvent);
  }

  void OnAnchorClicked(View view, const Dali::String& href)
  {
    Label label = Label::DownCast(view);
    if(label)
    {
    }
  }

  Label CreateStyledTextLabel(Dali::String text)
  {
    Label label = Label::New();
    label.SetStyledText(Text::StyledText::FromMarkup(text));
    label.SetFontSize(20.0f);
    label.SetFontFamily("SamsungOneUI_400");
    return label;
  }

  Label CreatePlainLabel(const char* text)
  {
    Label label = Label::New(text);
    label.SetFontSize(16.0f);
    label.SetFontFamily("SamsungOneUI_700");
    label.SetTextColor(UiColor(0x333333));
    return label;
  }

  InputField CreateMarkupInputField(const Dali::String& markup)
  {
    InputField inputField = InputField::New();
    inputField.SetStyledText(Text::StyledText::FromMarkup(markup));
    inputField.SetFontSize(20.0f);
    inputField.SetFontFamily("SamsungOneUI_400");
    inputField.SetTextColor(UiColor(0x202020));
    inputField.SetCursorWidth(2);
    inputField.SetCursorColor(UiColor(0x202020));
    inputField.SetSelectionColor(UiColor(0xA7D8FF));
    inputField.SetRequestedWidth(MATCH_PARENT);
    inputField.SetRequestedHeight(INPUT_FIELD_HEIGHT);
    inputField.SetBackgroundColor(UiColor(0xF3F3F3));
    inputField.SetPadding(Extents(12, 12, 8, 8));
    inputField.SetVerticalTextAlignment(Text::Alignment::CENTER);
    inputField.SetFocusable(true);
    inputField.SetMaximumLength(500);
    return inputField;
  }

  InputEditor CreateMarkupInputEditor(const Dali::String& markup)
  {
    InputEditor inputEditor = InputEditor::New();
    inputEditor.SetStyledText(Text::StyledText::FromMarkup(markup));
    inputEditor.SetFontSize(20.0f);
    inputEditor.SetFontFamily("SamsungOneUI_400");
    inputEditor.SetTextColor(UiColor(0x202020));
    inputEditor.SetCursorWidth(2);
    inputEditor.SetCursorColor(UiColor(0x202020));
    inputEditor.SetSelectionColor(UiColor(0xA7D8FF));
    inputEditor.SetRequestedWidth(MATCH_PARENT);
    inputEditor.SetRequestedHeight(INPUT_EDITOR_HEIGHT);
    inputEditor.SetBackgroundColor(UiColor(0xF3F3F3));
    inputEditor.SetPadding(Extents(12, 12, 8, 8));
    inputEditor.SetFocusable(true);
    inputEditor.SetMaximumLength(500);
    return inputEditor;
  }

  void PrintLabelInfo(Label label, const char* title)
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
    }
  }

private:
  Application& mApplication;
  Label        mAnchorLabel;
  Label        mAnchorColorLabel;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextController controller(application);
  application.MainLoop();

  return 0;
}
