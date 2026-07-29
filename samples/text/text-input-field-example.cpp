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

#include <cstdio>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float STACK_SPACING   = 6.0f;
constexpr float STACK_PADDING   = 12.0f;
constexpr float BUTTON_HEIGHT   = 36.0f;
constexpr float INPUT_HEIGHT    = 60.0f;
constexpr float BUTTON_SPACING  = 4.0f;

constexpr uint32_t COLOR_DARK_TEXT    = 0x222222;
constexpr uint32_t COLOR_DARK_GRAY    = 0x404040;
constexpr uint32_t COLOR_LIGHT_BLUE   = 0xADD8E6;
constexpr uint32_t COLOR_YELLOW       = 0xFFFF00;
constexpr uint32_t COLOR_CYAN         = 0x00FFFF;
constexpr uint32_t COLOR_MAGENTA      = 0xFF00FF;

Label CreateButton(const char* text, uint32_t bgColor)
{
  Label button = Label::New(text);
  button.SetFontSize(11.0f);
  button.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  button.SetVerticalTextAlignment(Text::Alignment::CENTER);
  button.SetBackgroundColor(UiColor(bgColor));
  button.SetRequestedWidth(0.0f);
  button.SetRequestedHeight(BUTTON_HEIGHT);
  button.SetPadding(Extents(4, 4, 4, 4));
  button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
  return button;
}

View CreateButtonRow(std::initializer_list<Label> buttons)
{
  StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
  row.SetRequestedWidth(MATCH_PARENT);
  row.SetRequestedHeight(WRAP_CONTENT);
  row.SetSpacing(BUTTON_SPACING);

  for(auto& btn : buttons)
  {
    row.Add(btn);
  }
  return row;
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
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xF5F5F5));

    // Target InputField
    mInputField = InputField::New();
    mInputField.SetPlaceholder("Type here");
    mInputField.SetPlaceholderColor(UiColor(COLOR_DARK_GRAY));
    mInputField.SetFontSize(20.0f);
    mInputField.SetCursorWidth(2);
    mInputField.SetCursorColor(UiColor(COLOR_DARK_TEXT));
    mInputField.SetSelectionColor(UiColor(COLOR_LIGHT_BLUE));
    mInputField.SetTextHandleEnabled(true);
    mInputField.SetTextHandleColor(UiColor(0x000080));
    mInputField.SetMaximumLength(50);
    mInputField.SetRequestedWidth(MATCH_PARENT);
    mInputField.SetRequestedHeight(INPUT_HEIGHT);
    mInputField.SetBackgroundColor(UiColor(0xFFFFFF));
    mInputField.SetTextColor(UiColor(COLOR_DARK_TEXT));
    mInputField.SetPadding(Extents(12, 12, 12, 12));
    mInputField.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mInputField.SetFocusable(true);

    // RTL test
    // mInputField.SetText("ABC العربية DEF עברית GHI 12345 JKL שלום MNO مرحبا PQR 67890 STU");
    // mInputField.SetLayoutDirection(Dali::LayoutDirection::RIGHT_TO_LEFT);
    // mInputField.SetLayoutDirectionMode(Text::LayoutDirectionMode::INHERIT);

    // Set text handle images
    mInputField.SetCursorHandleImage(RESOURCES_DIR "cursor_handle.png");
    mInputField.SetCursorHandlePressedImage(RESOURCES_DIR "cursor_handle_pressed.png");
    mInputField.SetSelectionHandleImageLeft(RESOURCES_DIR "selection_handle_left.png");
    mInputField.SetSelectionHandleImageRight(RESOURCES_DIR "selection_handle_right.png");
    mInputField.SetSelectionHandlePressedImageLeft(RESOURCES_DIR "selection_handle_left_pressed.png");
    mInputField.SetSelectionHandlePressedImageRight(RESOURCES_DIR "selection_handle_right_pressed.png");

    // Connect signals
    mInputField.TextChangedSignal().Connect(this, &InputFieldController::OnTextChanged);
    mInputField.MaximumLengthReachedSignal().Connect(this, &InputFieldController::OnMaximumLengthReached);
    mInputField.CursorPositionChangedSignal().Connect(this, &InputFieldController::OnCursorPositionChanged);
    mInputField.SelectionStartedSignal().Connect(this, &InputFieldController::OnSelectionStarted);
    mInputField.SelectionChangedSignal().Connect(this, &InputFieldController::OnSelectionChanged);
    mInputField.SelectionClearedSignal().Connect(this, &InputFieldController::OnSelectionCleared);

    // Status label
    mStatusLabel = Label::New();
    mStatusLabel.SetRequestedWidth(MATCH_PARENT);
    mStatusLabel.SetRequestedHeight(90);
    mStatusLabel.SetFontSize(10.0f);
    mStatusLabel.SetMultiLine(true);
    mStatusLabel.SetBackgroundColor(UiColor(0xE8E8E8));
    mStatusLabel.SetPadding(Extents(8, 8, 8, 8));

    UpdateStatus();

    // Title
    Label titleLabel = Label::New("InputField Test");
    titleLabel.SetFontSize(14.0f);
    titleLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    titleLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    titleLabel.SetTextColor(UiColor(0xFFFFFF));
    titleLabel.SetBackgroundColor(UiColor(0x2C3E50));
    titleLabel.SetRequestedWidth(MATCH_PARENT);
    titleLabel.SetRequestedHeight(32);

    // Cursor buttons row
    Label btnCursorBlink = CreateButton("Cursor Blink", 0x3498DB);
    Label btnCursorInterval = CreateButton("Blink Interval", 0x2ECC71);
    Label btnCursorPos = CreateButton("Cursor Pos", 0xE74C3C);
    Label btnCursorWidth = CreateButton("Cursor Width", 0x1ABC9C);
    View cursorRow1 = CreateButtonRow({btnCursorBlink, btnCursorInterval});
    View cursorRow2 = CreateButtonRow({btnCursorPos, btnCursorWidth});

    // Placeholder buttons row
    Label btnPlaceholderFocus = CreateButton("Placeholder Focus", 0xE67E22);
    Label btnPlaceholderColor = CreateButton("Placeholder Color", 0x9B59B6);
    View placeholderRow = CreateButtonRow({btnPlaceholderFocus, btnPlaceholderColor});

    // Selection buttons row
    Label btnSelectionColor = CreateButton("Selection Color", 0x7F8C8D);
    Label btnSelectionEnabled = CreateButton("Selection Enable", 0x8E44AD);
    View selectionRow1 = CreateButtonRow({btnSelectionColor, btnSelectionEnabled});

    Label btnSelectRange = CreateButton("Select Range", 0x27AE60);
    Label btnSelectWhole = CreateButton("Select Whole", 0xE74C3C);
    View selectionRow2 = CreateButtonRow({btnSelectRange, btnSelectWhole});

    Label btnClearSelection = CreateButton("Clear Selection", 0x95A5A6);
    View selectionRow3 = CreateButtonRow({btnClearSelection});

    // Input filter buttons row
    Label btnSetInputFilter = CreateButton("Set InputFilter", 0x2980B9);
    Label btnResetInputFilter = CreateButton("Reset InputFilter", 0x7F8C8D);
    View inputFilterRow = CreateButtonRow({btnSetInputFilter, btnResetInputFilter});

    // Other buttons row
    Label btnMaxLen = CreateButton("Max Length", 0xD35400);
    Label btnEditable = CreateButton("Editable", 0x16A085);
    View otherRow = CreateButtonRow({btnMaxLen, btnEditable});

    // Password buttons row
    Label btnPasswordMode = CreateButton("Password Mode", 0x8E44AD);
    Label btnPasswordChar = CreateButton("Password Char", 0x2980B9);
    Label btnPasswordReveal = CreateButton("Reveal Time", 0xC0392B);
    View passwordRow1 = CreateButtonRow({btnPasswordMode, btnPasswordChar});
    View passwordRow2 = CreateButtonRow({btnPasswordReveal});

    // Info button
    Label btnInfo = CreateButton("Print Info (log)", 0x34495E);
    View infoRow = CreateButtonRow({btnInfo});

    // Fixed header area (title, input field, status label)
    StackLayout fixedHeader = StackLayout::New(StackOrientation::VERTICAL);
    fixedHeader.SetSpacing(STACK_SPACING);
    fixedHeader.SetRequestedWidth(MATCH_PARENT);
    fixedHeader.SetRequestedHeight(WRAP_CONTENT);
    fixedHeader.Add(titleLabel);
    fixedHeader.Add(mInputField);
    fixedHeader.Add(mStatusLabel);

    // Scrollable content area (all test buttons)
    StackLayout scrollContent = StackLayout::New(StackOrientation::VERTICAL);
    scrollContent.SetSpacing(STACK_SPACING);
    scrollContent.SetRequestedWidth(MATCH_PARENT);
    scrollContent.SetRequestedHeight(WRAP_CONTENT);
    scrollContent.SetPadding(Extents(0, 0, 0, static_cast<int16_t>(STACK_PADDING)));
    scrollContent.Add(cursorRow1);
    scrollContent.Add(cursorRow2);
    scrollContent.Add(placeholderRow);
    scrollContent.Add(selectionRow1);
    scrollContent.Add(selectionRow2);
    scrollContent.Add(selectionRow3);
    scrollContent.Add(inputFilterRow);
    scrollContent.Add(otherRow);
    scrollContent.Add(passwordRow1);
    scrollContent.Add(passwordRow2);
    scrollContent.Add(infoRow);

    // ScrollView for test buttons
    ScrollView scrollView = ScrollView::New();
    scrollView.SetScrollDirection(ScrollDirection::Vertical);
    scrollView.SetRequestedWidth(MATCH_PARENT);
    scrollView.SetRequestedHeight(0.0f);
    scrollView.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    scrollView.SetContent(scrollContent);

    // Root layout
    StackLayout rootLayout = StackLayout::New(StackOrientation::VERTICAL);
    rootLayout.SetSpacing(STACK_SPACING);
    rootLayout.SetRequestedWidth(MATCH_PARENT);
    rootLayout.SetRequestedHeight(MATCH_PARENT);
    rootLayout.SetPadding(Extents(static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING)));
    rootLayout.Add(fixedHeader);
    rootLayout.Add(scrollView);

    window.Add(rootLayout);

    // Connect button touch signals - Cursor
    btnCursorBlink.TouchEventSignal().Connect(this, &InputFieldController::OnButtonCursorBlinkTouched);
    btnCursorInterval.TouchEventSignal().Connect(this, &InputFieldController::OnButtonCursorIntervalTouched);
    btnCursorPos.TouchEventSignal().Connect(this, &InputFieldController::OnButtonCursorPosTouched);
    btnCursorWidth.TouchEventSignal().Connect(this, &InputFieldController::OnButtonCursorWidthTouched);

    // Connect button touch signals - Placeholder
    btnPlaceholderFocus.TouchEventSignal().Connect(this, &InputFieldController::OnButtonPlaceholderFocusTouched);
    btnPlaceholderColor.TouchEventSignal().Connect(this, &InputFieldController::OnButtonPlaceholderColorTouched);

    // Connect button touch signals - Selection
    btnSelectionColor.TouchEventSignal().Connect(this, &InputFieldController::OnButtonSelectionColorTouched);
    btnSelectionEnabled.TouchEventSignal().Connect(this, &InputFieldController::OnButtonSelectionEnabledTouched);
    btnSelectRange.TouchEventSignal().Connect(this, &InputFieldController::OnButtonSelectRangeTouched);
    btnSelectWhole.TouchEventSignal().Connect(this, &InputFieldController::OnButtonSelectWholeTouched);
    btnClearSelection.TouchEventSignal().Connect(this, &InputFieldController::OnButtonClearSelectionTouched);

    // Connect button touch signals - Input filter
    btnSetInputFilter.TouchEventSignal().Connect(this, &InputFieldController::OnButtonSetInputFilterTouched);
    btnResetInputFilter.TouchEventSignal().Connect(this, &InputFieldController::OnButtonResetInputFilterTouched);

    // Connect button touch signals - Other
    btnMaxLen.TouchEventSignal().Connect(this, &InputFieldController::OnButtonMaxLenTouched);
    btnEditable.TouchEventSignal().Connect(this, &InputFieldController::OnButtonEditableTouched);

    // Connect button touch signals - Password
    btnPasswordMode.TouchEventSignal().Connect(this, &InputFieldController::OnButtonPasswordModeTouched);
    btnPasswordChar.TouchEventSignal().Connect(this, &InputFieldController::OnButtonPasswordCharTouched);
    btnPasswordReveal.TouchEventSignal().Connect(this, &InputFieldController::OnButtonPasswordRevealTouched);

    btnInfo.TouchEventSignal().Connect(this, &InputFieldController::OnButtonInfoTouched);

    // Also support key events
    window.KeyEventSignal().Connect(this, &InputFieldController::OnKeyEvent);
  }

  // Helper function to convert PasswordMode to string
  const char* PasswordModeToString(Text::PasswordMode mode)
  {
    switch(mode)
    {
      case Text::PasswordMode::NONE:
        return "NONE";
      case Text::PasswordMode::HIDE_ALL:
        return "HIDE_ALL";
      case Text::PasswordMode::REVEAL_LAST_CHARACTER:
        return "REVEAL_LAST_CHARACTER";
    }
    return "UNKNOWN";
  }

  // Helper function to append password status to string
  void AppendPasswordStatus(Dali::String& status)
  {
    status += "\nPwdMode:";
    status += PasswordModeToString(mInputField.GetPasswordMode());

    status += " MaskChar:U+";
    char hexChar[8];
    snprintf(hexChar, sizeof(hexChar), "%04X", mInputField.GetPasswordMaskCharacter());
    status += hexChar;

    status += " Reveal:";
    status += std::to_string(mInputField.GetPasswordRevealDuration()).c_str();
    status += "ms";
  }

  void UpdateStatus()
  {
    bool  cursorBlinkEnabled    = mInputField.IsCursorBlinkEnabled();
    float cursorBlinkInterval   = mInputField.GetCursorBlinkInterval();
    uint32_t cursorPosition     = mInputField.GetCursorPosition();
    int   cursorWidth           = mInputField.GetCursorWidth();
    int   maximumLength         = mInputField.GetMaximumLength();
    bool  editable              = mInputField.IsEditable();
    bool  selectionEnabled      = mInputField.IsSelectionEnabled();
    uint32_t selStart           = mInputField.GetSelectedTextStart();
    uint32_t selEnd             = mInputField.GetSelectedTextEnd();

    Dali::String status;
    status += "Blink:";
    status += (cursorBlinkEnabled ? "ON" : "OFF");
    status += "(";
    status += std::to_string(cursorBlinkInterval).substr(0, 4).c_str();
    status += "s) Pos:";
    status += std::to_string(cursorPosition).c_str();
    status += " W:";
    status += std::to_string(cursorWidth).c_str();
    status += "\nMax:";
    status += std::to_string(maximumLength).c_str();
    status += " Edit:";
    status += (editable ? "ON" : "OFF");
    status += " Sel:";
    status += (selectionEnabled ? "ON" : "OFF");
    status += " [";
    status += std::to_string(selStart).c_str();
    status += "-";
    status += std::to_string(selEnd).c_str();
    status += "]";

    // Add password status using helper
    AppendPasswordStatus(status);

    if(mInputFilterSet)
    {
      status += "\nFilter: allow=[\\d] deny=[0-5]";
    }

    mStatusLabel.SetText(status);
  }

  // --- Signals ---

  void OnTextChanged(View view)
  {
    InputField field = InputField::DownCast(view);
    if(field)
    {
      UpdateStatus();
    }
  }

  void OnMaximumLengthReached(View view)
  {
    InputField field = InputField::DownCast(view);
    if(field)
    {
    }
  }

  void OnCursorPositionChanged(View view, uint32_t position)
  {
    UpdateStatus();
  }

  void OnSelectionStarted(View view)
  {
    UpdateStatusWithSelection();
  }

  void OnSelectionChanged(View view, uint32_t start, uint32_t end)
  {
    UpdateStatusWithSelection();
  }

  void OnSelectionCleared(View view)
  {
    UpdateStatus();
  }

  void OnInputRejected(View view, Text::InputFilter::RejectReason reason)
  {
    [[maybe_unused]] const char* reasonStr = (reason == Text::InputFilter::RejectReason::NOT_ALLOWED) ? "NOT_ALLOWED" : "DENIED";
  }

  void UpdateStatusWithSelection()
  {
    bool  cursorBlinkEnabled    = mInputField.IsCursorBlinkEnabled();
    float cursorBlinkInterval   = mInputField.GetCursorBlinkInterval();
    uint32_t cursorPosition     = mInputField.GetCursorPosition();
    int   cursorWidth           = mInputField.GetCursorWidth();
    int   maximumLength         = mInputField.GetMaximumLength();
    bool  editable              = mInputField.IsEditable();
    bool  selectionEnabled      = mInputField.IsSelectionEnabled();
    uint32_t selStart           = mInputField.GetSelectedTextStart();
    uint32_t selEnd             = mInputField.GetSelectedTextEnd();
    Text::PasswordMode passwordMode = mInputField.GetPasswordMode();

    Dali::String status;
    status += "Blink:";
    status += (cursorBlinkEnabled ? "ON" : "OFF");
    status += "(";
    status += std::to_string(cursorBlinkInterval).substr(0, 4).c_str();
    status += "s) Pos:";
    status += std::to_string(cursorPosition).c_str();
    status += " W:";
    status += std::to_string(cursorWidth).c_str();
    status += "\nMax:";
    status += std::to_string(maximumLength).c_str();
    status += " Edit:";
    status += (editable ? "ON" : "OFF");
    status += " Sel:";
    status += (selectionEnabled ? "ON" : "OFF");
    status += " [";
    status += std::to_string(selStart).c_str();
    status += "-";
    status += std::to_string(selEnd).c_str();
    status += "]";

    // Add password status using helper
    AppendPasswordStatus(status);

    // Only show selected text when password mode is NONE
    if(passwordMode == Text::PasswordMode::NONE)
    {
      Dali::String selectedText = mInputField.GetSelectedText();
      if(selectedText.Size() > 0)
      {
        status += "\nSelected: \"";
        status += selectedText.CStr();
        status += "\"";
      }
    }

    mStatusLabel.SetText(status);
  }

  // --- Button handlers ---

  bool OnButtonCursorBlinkTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      bool enabled = mInputField.IsCursorBlinkEnabled();
      mInputField.SetCursorBlinkEnabled(!enabled);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonCursorIntervalTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      float interval = mInputField.GetCursorBlinkInterval();
      float newInterval = (interval < 0.8f) ? interval + 0.2f : 0.2f;
      mInputField.SetCursorBlinkInterval(newInterval);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonCursorPosTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      uint32_t position = mInputField.GetCursorPosition();
      uint32_t textLength = static_cast<uint32_t>(mInputField.GetText().Size());
      uint32_t newPosition = (position < textLength) ? position + 1u : 0u;
      mInputField.SetCursorPosition(newPosition);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonCursorWidthTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      int width = mInputField.GetCursorWidth();
      int newWidth = (width < 6) ? width + 1 : 1;
      mInputField.SetCursorWidth(newWidth);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonPlaceholderFocusTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      bool shown = mInputField.IsPlaceholderShownOnFocus();
      mInputField.SetShowPlaceholderOnFocus(!shown);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonPlaceholderColorTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      Vector4 currentColor = mInputField.GetPlaceholderColor().GetRgba();
      if(currentColor == UiColor(COLOR_DARK_GRAY))
      {
        mInputField.SetPlaceholderColor(UiColor(COLOR_LIGHT_BLUE));
      }
      else if(currentColor == UiColor(COLOR_LIGHT_BLUE))
      {
        mInputField.SetPlaceholderColor(UiColor(COLOR_YELLOW));
      }
      else
      {
        mInputField.SetPlaceholderColor(UiColor(COLOR_DARK_GRAY));
      }
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonSelectionColorTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      Vector4 currentColor = mInputField.GetSelectionColor().GetRgba();
      if(currentColor == UiColor(COLOR_LIGHT_BLUE))
      {
        mInputField.SetSelectionColor(UiColor(COLOR_CYAN));
      }
      else if(currentColor == UiColor(COLOR_CYAN))
      {
        mInputField.SetSelectionColor(UiColor(COLOR_MAGENTA));
      }
      else
      {
        mInputField.SetSelectionColor(UiColor(COLOR_LIGHT_BLUE));
      }
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonSelectionEnabledTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      bool enabled = mInputField.IsSelectionEnabled();
      mInputField.SetSelectionEnabled(!enabled);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonSelectRangeTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      uint32_t textLength = static_cast<uint32_t>(mInputField.GetText().Size());
      if(textLength > 0)
      {
        uint32_t start = 0u;
        uint32_t end = 0u;

        // Cycle through different selection patterns:
        // 0: [0, min(3, len)]       - first 3 chars
        // 1: [0, min(5, len)]       - first 5 chars
        // 2: [2, min(7, len)]       - middle section
        // 3: [min(3, len), 0]       - REVERSED: end > start (first 3 chars reversed)
        // 4: [min(5, len), min(2, len)] - REVERSED: end > start (chars 2-5 reversed)
        // 5: [0, len]               - whole text
        // Then cycle back to 0

        switch(mSelectRangeIndex)
        {
          case 0:
            start = 0u;
            end = std::min(3u, textLength);
            break;
          case 1:
            start = 0u;
            end = std::min(5u, textLength);
            break;
          case 2:
            start = std::min(2u, textLength);
            end = std::min(7u, textLength);
            break;
          case 3:
            // REVERSED case: end > start
            start = std::min(3u, textLength);
            end = 0u;
            break;
          case 4:
            // REVERSED case: end > start
            start = std::min(5u, textLength);
            end = std::min(2u, textLength);
            break;
          case 5:
            start = 0u;
            end = textLength;
            break;
          default:
            mSelectRangeIndex = 0;
            start = 0u;
            end = std::min(3u, textLength);
            break;
        }

        mInputField.SelectText(start, end);
        mSelectRangeIndex = (mSelectRangeIndex + 1) % 6;
      }
      else
      {
      }
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonSelectWholeTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputField.SelectWholeText();
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonClearSelectionTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputField.ClearSelection();
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonMaxLenTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      int maxLength = mInputField.GetMaximumLength();
      int newMaxLength = (maxLength <= 10) ? 20 : (maxLength <= 20) ? 50 : 10;
      mInputField.SetMaximumLength(newMaxLength);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonEditableTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      bool editable = mInputField.IsEditable();
      mInputField.SetEditable(!editable);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonSetInputFilterTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      Text::InputFilter inputFilter;
      inputFilter.SetAllowPattern("[\\d]");
      inputFilter.SetDenyPattern("[0-5]");
      mInputField.SetInputFilter(inputFilter);
      mInputFilterSet = true;
      mInputField.InputRejectedSignal().Connect(this, &InputFieldController::OnInputRejected);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonResetInputFilterTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputField.SetInputFilter(Text::InputFilter::None());
      mInputFilterSet = false;
      mInputField.InputRejectedSignal().Disconnect(this, &InputFieldController::OnInputRejected);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonInfoTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      PrintInputFieldInfo();
    }
    return true;
  }

  bool OnButtonPasswordModeTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      // Cycle through password modes: NONE -> HIDE_ALL -> REVEAL_LAST_CHARACTER -> NONE
      Text::PasswordMode currentMode = mInputField.GetPasswordMode();
      Text::PasswordMode newMode;

      switch(currentMode)
      {
        case Text::PasswordMode::NONE:
          newMode = Text::PasswordMode::HIDE_ALL;
          break;
        case Text::PasswordMode::HIDE_ALL:
          newMode = Text::PasswordMode::REVEAL_LAST_CHARACTER;
          // Set default reveal duration if current duration is 0
          if(mInputField.GetPasswordRevealDuration() == 0u)
          {
            mInputField.SetPasswordRevealDuration(1000u);
          }
          break;
        case Text::PasswordMode::REVEAL_LAST_CHARACTER:
        default:
          newMode = Text::PasswordMode::NONE;
          break;
      }

      mInputField.SetPasswordMode(newMode);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonPasswordCharTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      // Cycle through common password mask characters: '*' (U+002A) -> '•' (U+2022) -> '●' (U+25CF) -> '*'
      uint32_t currentChar = mInputField.GetPasswordMaskCharacter();
      uint32_t newChar;

      if(currentChar == 0x2A) // '*'
      {
        newChar = 0x2022; // '•' BULLET
      }
      else if(currentChar == 0x2022) // '•'
      {
        newChar = 0x25CF; // '●' BLACK CIRCLE
      }
      else
      {
        newChar = 0x2A; // '*'
      }

      mInputField.SetPasswordMaskCharacter(newChar);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonPasswordRevealTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      // Cycle through reveal durations: 0 -> 500 -> 1000 -> 2000 -> 0
      uint32_t currentDuration = mInputField.GetPasswordRevealDuration();
      uint32_t newDuration = 0u;

      if(currentDuration == 0u)
      {
        newDuration = 500u;
      }
      else if(currentDuration == 500u)
      {
        newDuration = 1000u;
      }
      else if(currentDuration == 1000u)
      {
        newDuration = 2000u;
      }
      else
      {
        newDuration = 0u;
      }

      // Set to REVEAL_LAST_CHARACTER mode for testing reveal duration
      mInputField.SetPasswordMode(Text::PasswordMode::REVEAL_LAST_CHARACTER);
      mInputField.SetPasswordRevealDuration(newDuration);
      UpdateStatus();
    }
    return true;
  }

  // --- Key events ---

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

  void PrintInputFieldInfo()
  {
  }

private:
  Application& mApplication;
  InputField   mInputField;
  Label        mStatusLabel;
  uint32_t     mSelectRangeIndex = 0;  // For cycling through selection ranges
  bool         mInputFilterSet = false;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  InputFieldController controller(application);
  application.MainLoop();

  return 0;
}
