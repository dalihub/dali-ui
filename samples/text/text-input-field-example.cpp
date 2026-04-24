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
  return Label::New(text)
    .SetFontSize(11.0f)
    .SetHorizontalTextAlignment(Text::Alignment::CENTER)
    .SetVerticalTextAlignment(Text::Alignment::CENTER)
    .SetBackgroundColor(UiColor(bgColor))
    .SetRequestedWidth(0.0f)
    .SetRequestedHeight(BUTTON_HEIGHT)
    .SetPadding(Extents(4, 4, 4, 4))
    .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
}

View CreateButtonRow(std::initializer_list<Label> buttons)
{
  StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
    .SetRequestedWidth(MATCH_PARENT)
    .SetRequestedHeight(WRAP_CONTENT)
    .SetSpacing(BUTTON_SPACING);

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
  void OnInit(Application& application)
  {
    DALI_LOG_ERROR("Application OnInit\n");

    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xF5F5F5));

    // Target InputField
    mInputField = InputField::New()
      .SetPlaceholder("Type here")
      .SetPlaceholderColor(UiColor(COLOR_DARK_GRAY))
      .SetFontSize(20.0f)
      .SetCursorWidth(2)
      .SetCursorColor(UiColor(COLOR_DARK_TEXT))
      .SetSelectionColor(UiColor(COLOR_LIGHT_BLUE))
      .SetMaximumLength(50)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(INPUT_HEIGHT)
      .SetBackgroundColor(UiColor(0xFFFFFF))
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetPadding(Extents(12, 12, 12, 12))
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .SetFocusable(true);

    // Connect signals
    mInputField.TextChangedSignal().Connect(this, &InputFieldController::OnTextChanged);
    mInputField.MaximumLengthReachedSignal().Connect(this, &InputFieldController::OnMaximumLengthReached);
    mInputField.CursorPositionChangedSignal().Connect(this, &InputFieldController::OnCursorPositionChanged);
    mInputField.SelectionStartedSignal().Connect(this, &InputFieldController::OnSelectionStarted);
    mInputField.SelectionChangedSignal().Connect(this, &InputFieldController::OnSelectionChanged);
    mInputField.SelectionClearedSignal().Connect(this, &InputFieldController::OnSelectionCleared);

    // Status label
    mStatusLabel = Label::New()
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(60)
      .SetFontSize(10.0f)
      .SetMultiLine(true)
      .SetBackgroundColor(UiColor(0xE8E8E8))
      .SetPadding(Extents(8, 8, 8, 8));

    UpdateStatus();

    // Title
    Label titleLabel = Label::New("InputField Test")
      .SetFontSize(14.0f)
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .SetTextColor(UiColor(0xFFFFFF))
      .SetBackgroundColor(UiColor(0x2C3E50))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(32);

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

    // Other buttons row
    Label btnMaxLen = CreateButton("Max Length", 0xD35400);
    Label btnEditable = CreateButton("Editable", 0x16A085);
    View otherRow = CreateButtonRow({btnMaxLen, btnEditable});

    // Info button
    Label btnInfo = CreateButton("Print Info (log)", 0x34495E);
    View infoRow = CreateButtonRow({btnInfo});

    // Fixed header area (title, input field, status label)
    StackLayout fixedHeader = StackLayout::New(StackOrientation::VERTICAL)
      .SetSpacing(STACK_SPACING)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .Children({
        titleLabel,
        mInputField,
        mStatusLabel,
      });

    // Scrollable content area (all test buttons)
    StackLayout scrollContent = StackLayout::New(StackOrientation::VERTICAL)
      .SetSpacing(STACK_SPACING)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetPadding(Extents(0, 0, 0, STACK_PADDING))
      .Children({
        // Cursor controls
        cursorRow1,
        cursorRow2,
        // Placeholder controls
        placeholderRow,
        // Selection controls
        selectionRow1,
        selectionRow2,
        selectionRow3,
        // Other controls
        otherRow,
        infoRow,
      });

    // ScrollView for test buttons
    ScrollView scrollView = ScrollView::New()
      .SetScrollDirection(ScrollDirection::Vertical)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(0.0f)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL))
      .SetContent(scrollContent);

    // Root layout
    StackLayout rootLayout = StackLayout::New(StackOrientation::VERTICAL)
      .SetSpacing(STACK_SPACING)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
      .Children({
        fixedHeader,
        scrollView,
      });

    window.Add(rootLayout);

    // Connect button touch signals - Cursor
    btnCursorBlink.TouchedSignal().Connect(this, &InputFieldController::OnButtonCursorBlinkTouched);
    btnCursorInterval.TouchedSignal().Connect(this, &InputFieldController::OnButtonCursorIntervalTouched);
    btnCursorPos.TouchedSignal().Connect(this, &InputFieldController::OnButtonCursorPosTouched);
    btnCursorWidth.TouchedSignal().Connect(this, &InputFieldController::OnButtonCursorWidthTouched);

    // Connect button touch signals - Placeholder
    btnPlaceholderFocus.TouchedSignal().Connect(this, &InputFieldController::OnButtonPlaceholderFocusTouched);
    btnPlaceholderColor.TouchedSignal().Connect(this, &InputFieldController::OnButtonPlaceholderColorTouched);

    // Connect button touch signals - Selection
    btnSelectionColor.TouchedSignal().Connect(this, &InputFieldController::OnButtonSelectionColorTouched);
    btnSelectionEnabled.TouchedSignal().Connect(this, &InputFieldController::OnButtonSelectionEnabledTouched);
    btnSelectRange.TouchedSignal().Connect(this, &InputFieldController::OnButtonSelectRangeTouched);
    btnSelectWhole.TouchedSignal().Connect(this, &InputFieldController::OnButtonSelectWholeTouched);
    btnClearSelection.TouchedSignal().Connect(this, &InputFieldController::OnButtonClearSelectionTouched);

    // Connect button touch signals - Other
    btnMaxLen.TouchedSignal().Connect(this, &InputFieldController::OnButtonMaxLenTouched);
    btnEditable.TouchedSignal().Connect(this, &InputFieldController::OnButtonEditableTouched);
    btnInfo.TouchedSignal().Connect(this, &InputFieldController::OnButtonInfoTouched);

    // Also support key events
    window.KeyEventSignal().Connect(this, &InputFieldController::OnKeyEvent);
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

  void OnSelectionStarted(View view)
  {
    DALI_LOG_ERROR("OnSelectionStarted\n");
    UpdateStatusWithSelection();
  }

  void OnSelectionChanged(View view, uint32_t start, uint32_t end)
  {
    DALI_LOG_ERROR("OnSelectionChanged: %u-%u\n", start, end);
    UpdateStatusWithSelection();
  }

  void OnSelectionCleared(View view)
  {
    DALI_LOG_ERROR("OnSelectionCleared\n");
    UpdateStatus();
  }

  void UpdateStatusWithSelection()
  {
    // Get selected text and show it in status
    Dali::String selectedText = mInputField.GetSelectedText();
    
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
    
    // Add selected text if any
    if(selectedText.Size() > 0)
    {
      status += "\nSelected: \"";
      status += selectedText.CStr();
      status += "\"";
    }

    mStatusLabel.SetText(status);
  }

  // --- Button handlers ---

  bool OnButtonCursorBlinkTouched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      bool enabled = mInputField.IsCursorBlinkEnabled();
      mInputField.SetCursorBlinkEnabled(!enabled);
      DALI_LOG_ERROR("CursorBlinkEnabled: %d -> %d\n", enabled, !enabled);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonCursorIntervalTouched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      float interval = mInputField.GetCursorBlinkInterval();
      float newInterval = (interval < 0.8f) ? interval + 0.2f : 0.2f;
      mInputField.SetCursorBlinkInterval(newInterval);
      DALI_LOG_ERROR("CursorBlinkInterval: %f -> %f\n", interval, newInterval);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonCursorPosTouched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      uint32_t position = mInputField.GetCursorPosition();
      uint32_t textLength = static_cast<uint32_t>(mInputField.GetText().Size());
      uint32_t newPosition = (position < textLength) ? position + 1u : 0u;
      mInputField.SetCursorPosition(newPosition);
      DALI_LOG_ERROR("CursorPosition: %u -> %u\n", position, newPosition);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonCursorWidthTouched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      int width = mInputField.GetCursorWidth();
      int newWidth = (width < 6) ? width + 1 : 1;
      mInputField.SetCursorWidth(newWidth);
      DALI_LOG_ERROR("CursorWidth: %d -> %d\n", width, newWidth);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonPlaceholderFocusTouched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      bool shown = mInputField.IsPlaceholderShownOnFocus();
      mInputField.SetShowPlaceholderOnFocus(!shown);
      DALI_LOG_ERROR("ShowPlaceholderOnFocus: %d -> %d\n", shown, !shown);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonPlaceholderColorTouched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      Vector4 currentColor = mInputField.GetPlaceholderColor().Resolve();
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
      DALI_LOG_ERROR("PlaceholderColor changed\n");
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonSelectionColorTouched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      Vector4 currentColor = mInputField.GetSelectionColor().Resolve();
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
      DALI_LOG_ERROR("SelectionColor changed\n");
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonSelectionEnabledTouched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      bool enabled = mInputField.IsSelectionEnabled();
      mInputField.SetSelectionEnabled(!enabled);
      DALI_LOG_ERROR("SelectionEnabled: %d -> %d\n", enabled, !enabled);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonSelectRangeTouched(Actor, const TouchEvent& touch)
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
            DALI_LOG_ERROR("SelectText [0-3]: %u-%u\n", start, end);
            break;
          case 1:
            start = 0u;
            end = std::min(5u, textLength);
            DALI_LOG_ERROR("SelectText [0-5]: %u-%u\n", start, end);
            break;
          case 2:
            start = std::min(2u, textLength);
            end = std::min(7u, textLength);
            DALI_LOG_ERROR("SelectText [2-7]: %u-%u\n", start, end);
            break;
          case 3:
            // REVERSED case: end > start
            start = std::min(3u, textLength);
            end = 0u;
            DALI_LOG_ERROR("SelectText REVERSED [3-0]: %u-%u (end < start)\n", start, end);
            break;
          case 4:
            // REVERSED case: end > start
            start = std::min(5u, textLength);
            end = std::min(2u, textLength);
            DALI_LOG_ERROR("SelectText REVERSED [5-2]: %u-%u (end < start)\n", start, end);
            break;
          case 5:
            start = 0u;
            end = textLength;
            DALI_LOG_ERROR("SelectText [0-len]: %u-%u (whole text)\n", start, end);
            break;
          default:
            mSelectRangeIndex = 0;
            start = 0u;
            end = std::min(3u, textLength);
            DALI_LOG_ERROR("SelectText [0-3]: %u-%u\n", start, end);
            break;
        }

        mInputField.SelectText(start, end);
        mSelectRangeIndex = (mSelectRangeIndex + 1) % 6;
      }
      else
      {
        DALI_LOG_ERROR("No text to select\n");
      }
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonSelectWholeTouched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputField.SelectWholeText();
      DALI_LOG_ERROR("SelectWholeText\n");
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonClearSelectionTouched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputField.ClearSelection();
      DALI_LOG_ERROR("ClearSelection\n");
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonMaxLenTouched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      int maxLength = mInputField.GetMaximumLength();
      int newMaxLength = (maxLength <= 10) ? 20 : (maxLength <= 20) ? 50 : 10;
      mInputField.SetMaximumLength(newMaxLength);
      DALI_LOG_ERROR("MaximumLength: %d -> %d\n", maxLength, newMaxLength);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonEditableTouched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      bool editable = mInputField.IsEditable();
      mInputField.SetEditable(!editable);
      DALI_LOG_ERROR("Editable: %d -> %d\n", editable, !editable);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonInfoTouched(Actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      PrintInputFieldInfo();
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
  uint32_t     mSelectRangeIndex = 0;  // For cycling through selection ranges
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