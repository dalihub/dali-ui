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
constexpr float STACK_SPACING   = 8.0f;
constexpr float STACK_PADDING   = 16.0f;
constexpr float BUTTON_HEIGHT   = 44.0f;
constexpr float INPUT_HEIGHT    = 80.0f;

constexpr uint32_t COLOR_DARK_TEXT    = 0x222222;
constexpr uint32_t COLOR_DARK_GRAY    = 0x404040;
constexpr uint32_t COLOR_LIGHT_BG     = 0xF2F2F2;
constexpr uint32_t COLOR_LIGHT_BLUE   = 0xADD8E6;
constexpr uint32_t COLOR_YELLOW       = 0xFFFF00;
constexpr uint32_t COLOR_CYAN         = 0x00FFFF;
constexpr uint32_t COLOR_MAGENTA      = 0xFF00FF;

Label CreateButton(const char* text, float fontSize = 14.0f)
{
  return Label::New(text)
    .SetFontSize(fontSize)
    .SetHorizontalTextAlignment(Text::Alignment::CENTER)
    .SetVerticalTextAlignment(Text::Alignment::CENTER)
    .SetBackgroundColor(UiColor(0x4A90D9))
    .SetRequestedWidth(MATCH_PARENT)
    .SetRequestedHeight(BUTTON_HEIGHT)
    .SetPadding(Extents(10, 10, 10, 10));
}
} // namespace

class InputFieldController : public ConnectionTracker
{
public:
  explicit InputFieldController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &InputFieldController::OnInit);
  }

private:
  void OnInit(Application& application)
  {
    DALI_LOG_ERROR("Application OnInit\n");

    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xF5F5F5));

    // Target InputField
    mInputField = InputField::New()
      .SetPlaceholder("Type here")
      .SetPlaceholderColor(UiColor(COLOR_DARK_GRAY))
      .SetFontSize(24.0f)
      .SetCursorWidth(2)
      .SetCursorColor(UiColor(COLOR_DARK_TEXT))
      .SetSelectionColor(UiColor(COLOR_LIGHT_BLUE))
      .SetMaximumLength(50)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(INPUT_HEIGHT)
      .SetBackgroundColor(UiColor(0xFFFFFF))
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetPadding(Extents(16, 16, 16, 16))
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .SetFocusable(true);

    // Connect signals
    mInputField.TextChangedSignal().Connect(this, &InputFieldController::OnTextChanged);
    mInputField.MaximumLengthReachedSignal().Connect(this, &InputFieldController::OnMaximumLengthReached);
    mInputField.CursorPositionChangedSignal().Connect(this, &InputFieldController::OnCursorPositionChanged);

    // Status label
    mStatusLabel = Label::New()
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(80)
      .SetFontSize(12.0f)
      .SetMultiLine(true)
      .SetBackgroundColor(UiColor(0xE8E8E8))
      .SetPadding(Extents(16, 16, 16, 16));

    UpdateStatus();

    // Title button
    Label titleButton = CreateButton("InputField Test", 16.0f)
      .SetBackgroundColor(UiColor(0x2C3E50))
      .SetFocusable(true);

    // Test buttons
    Label btn1 = CreateButton("1. Toggle Cursor Blink")
                   .SetBackgroundColor(UiColor(0x3498DB));

    Label btn2 = CreateButton("2. Change Cursor Blink Interval")
                   .SetBackgroundColor(UiColor(0x2ECC71));

    Label btn3 = CreateButton("3. Move Cursor Position")
                   .SetBackgroundColor(UiColor(0xE74C3C));

    Label btn4 = CreateButton("4. Toggle Show Placeholder On Focus")
                   .SetBackgroundColor(UiColor(0xE67E22));

    Label btn5 = CreateButton("5. Change Placeholder Color")
                   .SetBackgroundColor(UiColor(0x9B59B6));

    Label btn6 = CreateButton("6. Change Cursor Width")
                   .SetBackgroundColor(UiColor(0x1ABC9C));

    Label btn7 = CreateButton("7. Change Selection Color")
                   .SetBackgroundColor(UiColor(0x7F8C8D));

    Label btn8 = CreateButton("8. Change Maximum Length")
                   .SetBackgroundColor(UiColor(0xD35400));

    Label btn9 = CreateButton("9. Toggle Editable")
                   .SetBackgroundColor(UiColor(0x16A085));

    Label btn10 = CreateButton("10. Print Info (log)")
                    .SetBackgroundColor(UiColor(0x34495E));

    Label btn11 = CreateButton("11. Toggle Selection Enabled")
                    .SetBackgroundColor(UiColor(0x8E44AD));

    Label btn12 = CreateButton("12. Select Text Range")
                    .SetBackgroundColor(UiColor(0x27AE60));

    Label btn13 = CreateButton("13. Select Whole Text")
                    .SetBackgroundColor(UiColor(0xE74C3C));

    Label btn14 = CreateButton("14. Clear Selection")
                    .SetBackgroundColor(UiColor(0x95A5A6));

    window.Add(
      StackLayout::New(StackOrientation::VERTICAL)
        .SetSpacing(STACK_SPACING)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(MATCH_PARENT)
        .SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
        .Children({
          titleButton,
          Label::New("Target InputField:").SetFontSize(14.0f),
          mInputField,
          Label::New("Current Status:").SetFontSize(14.0f),
          mStatusLabel,
          Label::New("Test Actions:").SetFontSize(14.0f),
          btn1, btn2, btn3, btn4, btn5, btn6, btn7, btn8, btn9, btn10,
          btn11, btn12, btn13, btn14,
        }));

    // Connect button touch signals
    btn1.TouchedSignal().Connect(this, &InputFieldController::OnButton1Touched);
    btn2.TouchedSignal().Connect(this, &InputFieldController::OnButton2Touched);
    btn3.TouchedSignal().Connect(this, &InputFieldController::OnButton3Touched);
    btn4.TouchedSignal().Connect(this, &InputFieldController::OnButton4Touched);
    btn5.TouchedSignal().Connect(this, &InputFieldController::OnButton5Touched);
    btn6.TouchedSignal().Connect(this, &InputFieldController::OnButton6Touched);
    btn7.TouchedSignal().Connect(this, &InputFieldController::OnButton7Touched);
    btn8.TouchedSignal().Connect(this, &InputFieldController::OnButton8Touched);
    btn9.TouchedSignal().Connect(this, &InputFieldController::OnButton9Touched);
    btn10.TouchedSignal().Connect(this, &InputFieldController::OnButton10Touched);
    btn11.TouchedSignal().Connect(this, &InputFieldController::OnButton11Touched);
    btn12.TouchedSignal().Connect(this, &InputFieldController::OnButton12Touched);
    btn13.TouchedSignal().Connect(this, &InputFieldController::OnButton13Touched);
    btn14.TouchedSignal().Connect(this, &InputFieldController::OnButton14Touched);

    // Also support key events
    window.KeyEventSignal().Connect(this, &InputFieldController::OnKeyEvent);
  }

  void UpdateStatus()
  {
    bool  cursorBlinkEnabled    = mInputField.IsCursorBlinkEnabled();
    float cursorBlinkInterval   = mInputField.GetCursorBlinkInterval();
    uint32_t cursorPosition     = mInputField.GetCursorPosition();
    bool  showPlaceholderOnFocus = mInputField.IsPlaceholderShownOnFocus();
    int   cursorWidth           = mInputField.GetCursorWidth();
    int   maximumLength         = mInputField.GetMaximumLength();
    bool  editable              = mInputField.IsEditable();

    Dali::String status;

    status += "CursorBlink: ";
    status += (cursorBlinkEnabled ? "ON" : "OFF");
    status += ", Interval: ";
    status += std::to_string(cursorBlinkInterval).substr(0, 5).c_str();
    status += "\nCursorPos: ";
    status += std::to_string(cursorPosition).c_str();
    status += ", CursorWidth: ";
    status += std::to_string(cursorWidth).c_str();
    status += "\nShowPlaceholderOnFocus: ";
    status += (showPlaceholderOnFocus ? "ON" : "OFF");
    status += ", MaxLength: ";
    status += std::to_string(maximumLength).c_str();
    status += "\nEditable: ";
    status += (editable ? "ON" : "OFF");

    mStatusLabel.SetText(status);
  }

  // --- Signals ---

  void OnTextChanged(View view)
  {
    InputField field = InputField::DownCast(view);
    if(field)
    {
      DALI_LOG_ERROR("OnTextChanged: %s\n", field.GetText().CStr());
      UpdateStatus();
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

  void OnCursorPositionChanged(View view, uint32_t position)
  {
    DALI_LOG_ERROR("OnCursorPositionChanged: %u\n", position);
    UpdateStatus();
  }

  // --- Action functions ---

  void ActionToggleCursorBlink()
  {
    bool enabled = mInputField.IsCursorBlinkEnabled();
    mInputField.SetCursorBlinkEnabled(!enabled);
    DALI_LOG_ERROR("CursorBlinkEnabled: %d -> %d\n", enabled, !enabled);
    UpdateStatus();
  }

  void ActionChangeCursorBlinkInterval()
  {
    float interval = mInputField.GetCursorBlinkInterval();
    float newInterval = (interval < 0.8f) ? interval + 0.2f : 0.2f;
    mInputField.SetCursorBlinkInterval(newInterval);
    DALI_LOG_ERROR("CursorBlinkInterval: %f -> %f\n", interval, newInterval);
    UpdateStatus();
  }

  void ActionMoveCursorPosition()
  {
    uint32_t position = mInputField.GetCursorPosition();
    uint32_t textLength = static_cast<uint32_t>(mInputField.GetText().Size());
    uint32_t newPosition = (position < textLength) ? position + 1u : 0u;
    mInputField.SetCursorPosition(newPosition);
    DALI_LOG_ERROR("CursorPosition: %u -> %u (textLength: %u)\n", position, newPosition, textLength);
    UpdateStatus();
  }

  void ActionToggleShowPlaceholderOnFocus()
  {
    bool shown = mInputField.IsPlaceholderShownOnFocus();
    mInputField.SetShowPlaceholderOnFocus(!shown);
    DALI_LOG_ERROR("ShowPlaceholderOnFocus: %d -> %d\n", shown, !shown);
    UpdateStatus();
  }

  void ActionChangePlaceholderColor()
  {
    Vector4 currentColor = mInputField.GetPlaceholderColor().Resolve();
    if(currentColor == UiColor(COLOR_DARK_GRAY))
    {
      mInputField.SetPlaceholderColor(UiColor(COLOR_LIGHT_BLUE));
      DALI_LOG_ERROR("PlaceholderColor: DARK_GRAY -> LIGHT_BLUE\n");
    }
    else if(currentColor == UiColor(COLOR_LIGHT_BLUE))
    {
      mInputField.SetPlaceholderColor(UiColor(COLOR_YELLOW));
      DALI_LOG_ERROR("PlaceholderColor: LIGHT_BLUE -> YELLOW\n");
    }
    else
    {
      mInputField.SetPlaceholderColor(UiColor(COLOR_DARK_GRAY));
      DALI_LOG_ERROR("PlaceholderColor: -> DARK_GRAY\n");
    }
    UpdateStatus();
  }

  void ActionChangeCursorWidth()
  {
    int width = mInputField.GetCursorWidth();
    int newWidth = (width < 6) ? width + 1 : 1;
    mInputField.SetCursorWidth(newWidth);
    DALI_LOG_ERROR("CursorWidth: %d -> %d\n", width, newWidth);
    UpdateStatus();
  }

  void ActionChangeSelectionColor()
  {
    Vector4 currentColor = mInputField.GetSelectionColor().Resolve();
    if(currentColor == UiColor(COLOR_LIGHT_BLUE))
    {
      mInputField.SetSelectionColor(UiColor(COLOR_CYAN));
      DALI_LOG_ERROR("SelectionColor: LIGHT_BLUE -> CYAN\n");
    }
    else if(currentColor == UiColor(COLOR_CYAN))
    {
      mInputField.SetSelectionColor(UiColor(COLOR_MAGENTA));
      DALI_LOG_ERROR("SelectionColor: CYAN -> MAGENTA\n");
    }
    else
    {
      mInputField.SetSelectionColor(UiColor(COLOR_LIGHT_BLUE));
      DALI_LOG_ERROR("SelectionColor: -> LIGHT_BLUE\n");
    }
    UpdateStatus();
  }

  void ActionChangeMaximumLength()
  {
    int maxLength = mInputField.GetMaximumLength();
    int newMaxLength = (maxLength <= 10) ? 20 : (maxLength <= 20) ? 50 : 10;
    mInputField.SetMaximumLength(newMaxLength);
    DALI_LOG_ERROR("MaximumLength: %d -> %d\n", maxLength, newMaxLength);
    UpdateStatus();
  }

  void ActionToggleEditable()
  {
    bool editable = mInputField.IsEditable();
    mInputField.SetEditable(!editable);
    DALI_LOG_ERROR("Editable: %d -> %d\n", editable, !editable);
    UpdateStatus();
  }

  void ActionToggleSelectionEnabled()
  {
    bool enabled = mInputField.IsSelectionEnabled();
    mInputField.SetSelectionEnabled(!enabled);
    DALI_LOG_ERROR("SelectionEnabled: %d -> %d\n", enabled, !enabled);
    UpdateStatus();
  }

  void ActionSelectTextRange()
  {
    // Select a range of text (e.g., characters 1-5)
    uint32_t textLength = static_cast<uint32_t>(mInputField.GetText().Size());
    if(textLength > 0)
    {
      uint32_t start = 0u;
      uint32_t end = std::min(5u, textLength);
      mInputField.SelectText(start, end);
      DALI_LOG_ERROR("SelectText: %u-%u (textLength: %u)\n", start, end, textLength);
      DALI_LOG_ERROR("SelectedTextStart: %u, SelectedTextEnd: %u\n", mInputField.GetSelectedTextStart(), mInputField.GetSelectedTextEnd());
    }
    else
    {
      DALI_LOG_ERROR("No text to select\n");
    }
    UpdateStatus();
  }

  void ActionSelectWholeText()
  {
    mInputField.SelectWholeText();
    DALI_LOG_ERROR("SelectWholeText: textLength=%zu\n", mInputField.GetText().Size());
    DALI_LOG_ERROR("SelectedTextStart: %u, SelectedTextEnd: %u\n", mInputField.GetSelectedTextStart(), mInputField.GetSelectedTextEnd());
    UpdateStatus();
  }

  void ActionClearSelection()
  {
    mInputField.ClearSelection();
    DALI_LOG_ERROR("ClearSelection\n");
    DALI_LOG_ERROR("SelectedTextStart: %u, SelectedTextEnd: %u\n", mInputField.GetSelectedTextStart(), mInputField.GetSelectedTextEnd());
    UpdateStatus();
  }

  // --- Button handlers ---

  bool OnButton1Touched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      ActionToggleCursorBlink();
    }
    return true;
  }

  bool OnButton2Touched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      ActionChangeCursorBlinkInterval();
    }
    return true;
  }

  bool OnButton3Touched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      ActionMoveCursorPosition();
    }
    return true;
  }

  bool OnButton4Touched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      ActionToggleShowPlaceholderOnFocus();
    }
    return true;
  }

  bool OnButton5Touched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      ActionChangePlaceholderColor();
    }
    return true;
  }

  bool OnButton6Touched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      ActionChangeCursorWidth();
    }
    return true;
  }

  bool OnButton7Touched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      ActionChangeSelectionColor();
    }
    return true;
  }

  bool OnButton8Touched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      ActionChangeMaximumLength();
    }
    return true;
  }

  bool OnButton9Touched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      ActionToggleEditable();
    }
    return true;
  }

  bool OnButton10Touched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      PrintInputFieldInfo();
    }
    return true;
  }

  bool OnButton11Touched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      ActionToggleSelectionEnabled();
    }
    return true;
  }

  bool OnButton12Touched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      ActionSelectTextRange();
    }
    return true;
  }

  bool OnButton13Touched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      ActionSelectWholeText();
    }
    return true;
  }

  bool OnButton14Touched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      ActionClearSelection();
    }
    return true;
  }

  // --- Key events ---

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
      ActionToggleCursorBlink();
    }
    else if(event.GetKeyName() == "2")
    {
      ActionChangeCursorBlinkInterval();
    }
    else if(event.GetKeyName() == "3")
    {
      ActionMoveCursorPosition();
    }
    else if(event.GetKeyName() == "4")
    {
      ActionToggleShowPlaceholderOnFocus();
    }
    else if(event.GetKeyName() == "5")
    {
      ActionChangePlaceholderColor();
    }
    else if(event.GetKeyName() == "6")
    {
      ActionChangeCursorWidth();
    }
    else if(event.GetKeyName() == "7")
    {
      ActionChangeSelectionColor();
    }
    else if(event.GetKeyName() == "8")
    {
      ActionChangeMaximumLength();
    }
    else if(event.GetKeyName() == "9")
    {
      ActionToggleEditable();
    }
    else if(event.GetKeyName() == "0")
    {
      PrintInputFieldInfo();
    }
  }

  void PrintInputFieldInfo()
  {
    Vector4 textColor        = mInputField.GetTextColor().Resolve();
    Vector4 placeholderColor = mInputField.GetPlaceholderColor().Resolve();
    Vector4 cursorColor      = mInputField.GetCursorColor().Resolve();
    Vector4 selectionColor   = mInputField.GetSelectionColor().Resolve();

    DALI_LOG_ERROR("----------------------------------------------------------------\n");
    DALI_LOG_ERROR("InputField Info\n");
    DALI_LOG_ERROR("Text                  : %s\n", mInputField.GetText().CStr());
    DALI_LOG_ERROR("Placeholder           : %s\n", mInputField.GetPlaceholder().CStr());
    DALI_LOG_ERROR("PlaceholderColor      : %.2f, %.2f, %.2f, %.2f\n", placeholderColor.r, placeholderColor.g, placeholderColor.b, placeholderColor.a);
    DALI_LOG_ERROR("ShowPlaceholderOnFocus: %d\n", mInputField.IsPlaceholderShownOnFocus());
    DALI_LOG_ERROR("CursorWidth           : %d\n", mInputField.GetCursorWidth());
    DALI_LOG_ERROR("CursorBlinkEnabled    : %d\n", mInputField.IsCursorBlinkEnabled());
    DALI_LOG_ERROR("CursorBlinkInterval   : %f\n", mInputField.GetCursorBlinkInterval());
    DALI_LOG_ERROR("CursorPosition        : %u\n", mInputField.GetCursorPosition());
    DALI_LOG_ERROR("CursorColor           : %.2f, %.2f, %.2f, %.2f\n", cursorColor.r, cursorColor.g, cursorColor.b, cursorColor.a);
    DALI_LOG_ERROR("SelectionColor        : %.2f, %.2f, %.2f, %.2f\n", selectionColor.r, selectionColor.g, selectionColor.b, selectionColor.a);
    DALI_LOG_ERROR("SelectionEnabled      : %d\n", mInputField.IsSelectionEnabled());
    DALI_LOG_ERROR("SelectedText          : %s\n", mInputField.GetSelectedText().CStr());
    DALI_LOG_ERROR("SelectedTextStart     : %u\n", mInputField.GetSelectedTextStart());
    DALI_LOG_ERROR("SelectedTextEnd       : %u\n", mInputField.GetSelectedTextEnd());
    DALI_LOG_ERROR("MaximumLength         : %d\n", mInputField.GetMaximumLength());
    DALI_LOG_ERROR("Editable              : %d\n", mInputField.IsEditable());
    DALI_LOG_ERROR("TextColor             : %.2f, %.2f, %.2f, %.2f\n", textColor.r, textColor.g, textColor.b, textColor.a);
    DALI_LOG_ERROR("FontSize              : %f\n", mInputField.GetFontSize());
    DALI_LOG_ERROR("----------------------------------------------------------------\n");
  }

private:
  Application& mApplication;
  InputField   mInputField;
  Label        mStatusLabel;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  DALI_LOG_ERROR("DaliUiFoundationPreInitialize START\n");
  DaliUiFoundationPreInitialize(nullptr, nullptr, nullptr);
  DALI_LOG_ERROR("DaliUiFoundationPreInitialize END\n");

  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  InputFieldController controller(application);
  application.MainLoop();

  return 0;
}