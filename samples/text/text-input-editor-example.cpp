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
constexpr float EDITOR_HEIGHT   = 120.0f;
constexpr float BUTTON_SPACING  = 4.0f;

const char* GetLineHeightModeName(Text::LineHeightMode mode)
{
  switch(mode)
  {
    case Text::LineHeightMode::RELATIVE:
      return "RELATIVE";
    case Text::LineHeightMode::ABSOLUTE:
      return "ABSOLUTE";
    default:
      return "UNKNOWN";
  }
}

Dali::String GetLineHeightText(float lineHeight)
{
  if(Dali::Equals(lineHeight, Text::LINE_HEIGHT_AUTO, Math::MACHINE_EPSILON_1000))
  {
    return "AUTO";
  }
  return std::to_string(lineHeight).substr(0, 5).c_str();
}

const char* GetLineWrapModeName(Text::LineWrapMode mode)
{
  switch(mode)
  {
    case Text::LineWrapMode::WORD:
      return "WORD";
    case Text::LineWrapMode::CHARACTER:
      return "CHARACTER";
    case Text::LineWrapMode::HYPHENATION:
      return "HYPHENATION";
    case Text::LineWrapMode::MIXED:
      return "MIXED";
    default:
      return "UNKNOWN";
  }
}

const char* GetHorizontalAlignmentName(Text::Alignment alignment)
{
  switch(alignment)
  {
    case Text::Alignment::START:
      return "START";
    case Text::Alignment::CENTER:
      return "CENTER";
    case Text::Alignment::END:
      return "END";
    default:
      return "UNKNOWN";
  }
}

const char* GetVerticalAlignmentName(Text::Alignment alignment)
{
  switch(alignment)
  {
    case Text::Alignment::START:
      return "START";
    case Text::Alignment::CENTER:
      return "CENTER";
    case Text::Alignment::END:
      return "END";
    default:
      return "UNKNOWN";
  }
}

enum class LineHeightStep
{
  AUTO,
  ONE,
  ONE_AND_HALF,
  TWO
};

LineHeightStep GetLineHeightStep(float lineHeight, float fontSize, Text::LineHeightMode mode)
{
  if(Dali::Equals(lineHeight, Text::LINE_HEIGHT_AUTO, Math::MACHINE_EPSILON_1000))
  {
    return LineHeightStep::AUTO;
  }

  float target1, target1_5, target2;
  if(mode == Text::LineHeightMode::RELATIVE)
  {
    target1 = 1.0f;
    target1_5 = 1.5f;
    target2 = 2.0f;
  }
  else // ABSOLUTE
  {
    target1 = fontSize * 1.0f;
    target1_5 = fontSize * 1.5f;
    target2 = fontSize * 2.0f;
  }

  if(Dali::Equals(lineHeight, target1, Math::MACHINE_EPSILON_1000))
  {
    return LineHeightStep::ONE;
  }
  if(Dali::Equals(lineHeight, target1_5, Math::MACHINE_EPSILON_1000))
  {
    return LineHeightStep::ONE_AND_HALF;
  }
  if(Dali::Equals(lineHeight, target2, Math::MACHINE_EPSILON_1000))
  {
    return LineHeightStep::TWO;
  }

  // Default to AUTO for unrecognized values
  return LineHeightStep::AUTO;
}

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

class InputEditorController : public ConnectionTracker
{
public:
  explicit InputEditorController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &InputEditorController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xF5F5F5));

    // Target InputEditor - multi-line text editor
    mInputEditor = InputEditor::New();
    mInputEditor.SetPlaceholder("Type here...This is a multi-line editor.");
    mInputEditor.SetPlaceholderColor(UiColor(COLOR_DARK_GRAY));
    mInputEditor.SetFontSize(18.0f);
    mInputEditor.SetCursorWidth(2);
    mInputEditor.SetCursorColor(UiColor(COLOR_DARK_TEXT));
    mInputEditor.SetSelectionColor(UiColor(COLOR_LIGHT_BLUE));
    mInputEditor.SetTextHandleEnabled(true);
    mInputEditor.SetTextHandleColor(UiColor(0x000080));
    mInputEditor.SetMaximumLength(500);
    mInputEditor.SetRequestedWidth(MATCH_PARENT);
    mInputEditor.SetRequestedHeight(EDITOR_HEIGHT);
    mInputEditor.SetMaximumHeight(200);
    mInputEditor.SetBackgroundColor(UiColor(0xFFFFFF));
    mInputEditor.SetTextColor(UiColor(COLOR_DARK_TEXT));
    mInputEditor.SetPadding(Extents(12, 12, 12, 12));
    mInputEditor.SetFocusable(true);

    // Set text handle images
    mInputEditor.SetCursorHandleImage(RESOURCES_DIR "cursor_handle.png");
    mInputEditor.SetCursorHandlePressedImage(RESOURCES_DIR "cursor_handle_pressed.png");
    mInputEditor.SetSelectionHandleImageLeft(RESOURCES_DIR "selection_handle_left.png");
    mInputEditor.SetSelectionHandleImageRight(RESOURCES_DIR "selection_handle_right.png");
    mInputEditor.SetSelectionHandlePressedImageLeft(RESOURCES_DIR "selection_handle_left_pressed.png");
    mInputEditor.SetSelectionHandlePressedImageRight(RESOURCES_DIR "selection_handle_right_pressed.png");

    // Set initial multi-line text
    mInputEditor.SetText("This is line 1.\nThis is line 2.\nThis is line 3.");

    // RTL test
    // mInputEditor.SetText("Line 1: English starts here, then العربية 12345 مع النص العربي, and back to English.\n"
    //                      "Line 2: Another LTR part before עברית עם מספרים 67890 ואז English continues.\n"
    //                      "Line 3: Mix brackets (العربية inside parentheses) and [עברית inside brackets] with 2026.");

    // mInputEditor.SetLayoutDirection(Dali::LayoutDirection::RIGHT_TO_LEFT);
    // mInputEditor.SetLayoutDirectionMode(Text::LayoutDirectionMode::INHERIT);

    // Connect signals
    mInputEditor.TextChangedSignal().Connect(this, &InputEditorController::OnTextChanged);
    mInputEditor.MaximumLengthReachedSignal().Connect(this, &InputEditorController::OnMaximumLengthReached);
    mInputEditor.CursorPositionChangedSignal().Connect(this, &InputEditorController::OnCursorPositionChanged);
    mInputEditor.SelectionStartedSignal().Connect(this, &InputEditorController::OnSelectionStarted);
    mInputEditor.SelectionChangedSignal().Connect(this, &InputEditorController::OnSelectionChanged);
    mInputEditor.SelectionClearedSignal().Connect(this, &InputEditorController::OnSelectionCleared);

    // Status label
    mStatusLabel = Label::New();
    mStatusLabel.SetRequestedWidth(MATCH_PARENT);
    mStatusLabel.SetRequestedHeight(120);
    mStatusLabel.SetFontSize(10.0f);
    mStatusLabel.SetMultiLine(true);
    mStatusLabel.SetBackgroundColor(UiColor(0xE8E8E8));
    mStatusLabel.SetPadding(Extents(8, 8, 8, 8));

    UpdateStatus();

    // Title
    Label titleLabel = Label::New("InputEditor Test (Multi-line)");
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

    // Other buttons row
    Label btnMaxLen = CreateButton("Max Length", 0xD35400);
    Label btnEditable = CreateButton("Editable", 0x16A085);
    View otherRow = CreateButtonRow({btnMaxLen, btnEditable});

    // Text background color button
    Label btnTextBgColor = CreateButton("Text Bg Color", 0x27AE60);
    Label btnClearTextBg = CreateButton("Clear Text Bg", 0x7F8C8D);
    View textBgRow = CreateButtonRow({btnTextBgColor, btnClearTextBg});

    // Line wrap mode button
    Label btnLineWrapMode = CreateButton("Line Wrap Mode", 0x2980B9);
    View lineWrapRow = CreateButtonRow({btnLineWrapMode});

    // Line height buttons
    Label btnLineHeight = CreateButton("Line Height", 0x8E44AD);
    Label btnLineHeightMode = CreateButton("LineHeightMode", 0x16A085);
    View lineHeightRow = CreateButtonRow({btnLineHeight, btnLineHeightMode});

    // Alignment buttons
    Label btnHorizontalAlignment = CreateButton("H Align", 0xE74C3C);
    Label btnVerticalAlignment = CreateButton("V Align", 0x3498DB);
    View alignmentRow = CreateButtonRow({btnHorizontalAlignment, btnVerticalAlignment});

    // Auto Grow button
    Label btnAutoGrow = CreateButton("Auto Grow", 0x8E44AD);
    View autoGrowRow = CreateButtonRow({btnAutoGrow});

    // Info button
    Label btnInfo = CreateButton("Print Info (log)", 0x34495E);
    View infoRow = CreateButtonRow({btnInfo});

    // Fixed header area (title, input editor, status label)
    StackLayout fixedHeader = StackLayout::New(StackOrientation::VERTICAL);
    fixedHeader.SetSpacing(STACK_SPACING);
    fixedHeader.SetRequestedWidth(MATCH_PARENT);
    fixedHeader.SetRequestedHeight(WRAP_CONTENT);
    fixedHeader.Add(titleLabel);
    fixedHeader.Add(mInputEditor);
    fixedHeader.Add(mStatusLabel);

    // Scrollable content area (all test buttons)
    StackLayout scrollContent = StackLayout::New(StackOrientation::VERTICAL);
    scrollContent.SetSpacing(STACK_SPACING);
    scrollContent.SetRequestedWidth(MATCH_PARENT);
    scrollContent.SetRequestedHeight(WRAP_CONTENT);
    scrollContent.SetPadding(Extents(0, 0, 0, STACK_PADDING));
    scrollContent.Add(cursorRow1);
    scrollContent.Add(cursorRow2);
    scrollContent.Add(placeholderRow);
    scrollContent.Add(selectionRow1);
    scrollContent.Add(selectionRow2);
    scrollContent.Add(selectionRow3);
    scrollContent.Add(otherRow);
    scrollContent.Add(textBgRow);
    scrollContent.Add(lineWrapRow);
    scrollContent.Add(lineHeightRow);
    scrollContent.Add(alignmentRow);
    scrollContent.Add(autoGrowRow);
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
    rootLayout.SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING));
    rootLayout.Add(fixedHeader);
    rootLayout.Add(scrollView);

    window.Add(rootLayout);

    // Connect button touch signals - Cursor
    btnCursorBlink.TouchEventSignal().Connect(this, &InputEditorController::OnButtonCursorBlinkTouched);
    btnCursorInterval.TouchEventSignal().Connect(this, &InputEditorController::OnButtonCursorIntervalTouched);
    btnCursorPos.TouchEventSignal().Connect(this, &InputEditorController::OnButtonCursorPosTouched);
    btnCursorWidth.TouchEventSignal().Connect(this, &InputEditorController::OnButtonCursorWidthTouched);

    // Connect button touch signals - Placeholder
    btnPlaceholderFocus.TouchEventSignal().Connect(this, &InputEditorController::OnButtonPlaceholderFocusTouched);
    btnPlaceholderColor.TouchEventSignal().Connect(this, &InputEditorController::OnButtonPlaceholderColorTouched);

    // Connect button touch signals - Selection
    btnSelectionColor.TouchEventSignal().Connect(this, &InputEditorController::OnButtonSelectionColorTouched);
    btnSelectionEnabled.TouchEventSignal().Connect(this, &InputEditorController::OnButtonSelectionEnabledTouched);
    btnSelectRange.TouchEventSignal().Connect(this, &InputEditorController::OnButtonSelectRangeTouched);
    btnSelectWhole.TouchEventSignal().Connect(this, &InputEditorController::OnButtonSelectWholeTouched);
    btnClearSelection.TouchEventSignal().Connect(this, &InputEditorController::OnButtonClearSelectionTouched);

    // Connect button touch signals - Other
    btnMaxLen.TouchEventSignal().Connect(this, &InputEditorController::OnButtonMaxLenTouched);
    btnEditable.TouchEventSignal().Connect(this, &InputEditorController::OnButtonEditableTouched);

    // Connect button touch signals - Text background
    btnTextBgColor.TouchEventSignal().Connect(this, &InputEditorController::OnButtonTextBgColorTouched);
    btnClearTextBg.TouchEventSignal().Connect(this, &InputEditorController::OnButtonClearTextBgTouched);

    // Connect button touch signals - Line wrap mode
    btnLineWrapMode.TouchEventSignal().Connect(this, &InputEditorController::OnButtonLineWrapModeTouched);

    // Connect button touch signals - Line height
    btnLineHeight.TouchEventSignal().Connect(this, &InputEditorController::OnButtonLineHeightTouched);
    btnLineHeightMode.TouchEventSignal().Connect(this, &InputEditorController::OnButtonLineHeightModeTouched);

    // Connect button touch signals - Alignment
    btnHorizontalAlignment.TouchEventSignal().Connect(this, &InputEditorController::OnButtonHorizontalAlignmentTouched);
    btnVerticalAlignment.TouchEventSignal().Connect(this, &InputEditorController::OnButtonVerticalAlignmentTouched);

    // Connect button touch signals - Auto Grow
    btnAutoGrow.TouchEventSignal().Connect(this, &InputEditorController::OnButtonAutoGrowTouched);

    btnInfo.TouchEventSignal().Connect(this, &InputEditorController::OnButtonInfoTouched);

    // Also support key events
    window.KeyEventSignal().Connect(this, &InputEditorController::OnKeyEvent);
  }

  void UpdateStatus()
  {
    bool     cursorBlinkEnabled  = mInputEditor.IsCursorBlinkEnabled();
    float    cursorBlinkInterval = mInputEditor.GetCursorBlinkInterval();
    uint32_t cursorPosition      = mInputEditor.GetCursorPosition();
    int      cursorWidth         = mInputEditor.GetCursorWidth();
    int      maximumLength       = mInputEditor.GetMaximumLength();
    bool     editable            = mInputEditor.IsEditable();
    bool     selectionEnabled    = mInputEditor.IsSelectionEnabled();
    uint32_t selStart            = mInputEditor.GetSelectedTextStart();
    uint32_t selEnd              = mInputEditor.GetSelectedTextEnd();
    int      lineCount           = mInputEditor.GetLineCount();

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
    status += "\nWrap:";
    status += GetLineWrapModeName(mInputEditor.GetLineWrapMode());
    status += " LineHeight:";
    status += GetLineHeightText(mInputEditor.GetLineHeight());
    status += " Mode:";
    status += GetLineHeightModeName(mInputEditor.GetLineHeightMode());
    status += " LineCount:";
    status += std::to_string(lineCount).c_str();
    status += "\nHAlign:";
    status += GetHorizontalAlignmentName(mInputEditor.GetHorizontalTextAlignment());
    status += " VAlign:";
    status += GetVerticalAlignmentName(mInputEditor.GetVerticalTextAlignment());
    Dali::String selectedText = mInputEditor.GetSelectedText();
    if(selectedText.Size() > 0)
    {
      status += "\nSelected: \"";
      status += selectedText.CStr();
      status += "\"";
    }

    mStatusLabel.SetText(status);
  }

  // --- Signals ---

  void OnTextChanged(View view)
  {
    InputEditor editor = InputEditor::DownCast(view);
    if(editor)
    {
      UpdateStatus();
    }
  }

  void OnMaximumLengthReached(View view)
  {
    InputEditor editor = InputEditor::DownCast(view);
    if(editor)
    {
    }
  }

  void OnCursorPositionChanged(View view, uint32_t position)
  {
    UpdateStatus();
  }

  void OnSelectionStarted(View view)
  {
    UpdateStatus();
  }

  void OnSelectionChanged(View view, uint32_t start, uint32_t end)
  {
    UpdateStatus();
  }

  void OnSelectionCleared(View view)
  {
    UpdateStatus();
  }

  // --- Button handlers ---

  bool OnButtonCursorBlinkTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      bool enabled = mInputEditor.IsCursorBlinkEnabled();
      mInputEditor.SetCursorBlinkEnabled(!enabled);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonCursorIntervalTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      float interval = mInputEditor.GetCursorBlinkInterval();
      float newInterval = (interval < 0.8f) ? interval + 0.2f : 0.2f;
      mInputEditor.SetCursorBlinkInterval(newInterval);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonCursorPosTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      uint32_t position = mInputEditor.GetCursorPosition();
      uint32_t textLength = static_cast<uint32_t>(mInputEditor.GetText().Size());
      uint32_t newPosition = (position < textLength) ? position + 1u : 0u;
      mInputEditor.SetCursorPosition(newPosition);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonCursorWidthTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      int width = mInputEditor.GetCursorWidth();
      int newWidth = (width < 6) ? width + 1 : 1;
      mInputEditor.SetCursorWidth(newWidth);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonPlaceholderFocusTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      bool shown = mInputEditor.IsPlaceholderShownOnFocus();
      mInputEditor.SetShowPlaceholderOnFocus(!shown);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonPlaceholderColorTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      Vector4 currentColor = mInputEditor.GetPlaceholderColor().GetRgba();
      if(currentColor == UiColor(COLOR_DARK_GRAY))
      {
        mInputEditor.SetPlaceholderColor(UiColor(COLOR_LIGHT_BLUE));
      }
      else if(currentColor == UiColor(COLOR_LIGHT_BLUE))
      {
        mInputEditor.SetPlaceholderColor(UiColor(COLOR_YELLOW));
      }
      else
      {
        mInputEditor.SetPlaceholderColor(UiColor(COLOR_DARK_GRAY));
      }
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonSelectionColorTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      Vector4 currentColor = mInputEditor.GetSelectionColor().GetRgba();
      if(currentColor == UiColor(COLOR_LIGHT_BLUE))
      {
        mInputEditor.SetSelectionColor(UiColor(COLOR_CYAN));
      }
      else if(currentColor == UiColor(COLOR_CYAN))
      {
        mInputEditor.SetSelectionColor(UiColor(COLOR_MAGENTA));
      }
      else
      {
        mInputEditor.SetSelectionColor(UiColor(COLOR_LIGHT_BLUE));
      }
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonSelectionEnabledTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      bool enabled = mInputEditor.IsSelectionEnabled();
      mInputEditor.SetSelectionEnabled(!enabled);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonSelectRangeTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      uint32_t textLength = static_cast<uint32_t>(mInputEditor.GetText().Size());
      if(textLength > 0)
      {
        uint32_t start = 0u;
        uint32_t end = std::min(10u, textLength);
        mInputEditor.SelectText(start, end);
      }
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonSelectWholeTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputEditor.SelectWholeText();
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonClearSelectionTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputEditor.ClearSelection();
      UpdateStatus();
    }
    return true;
  }


  bool OnButtonMaxLenTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      int maxLength = mInputEditor.GetMaximumLength();
      int newMaxLength = (maxLength <= 100) ? 250 : (maxLength <= 250) ? 500 : 100;
      mInputEditor.SetMaximumLength(newMaxLength);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonEditableTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      bool editable = mInputEditor.IsEditable();
      mInputEditor.SetEditable(!editable);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonTextBgColorTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      Vector4 currentColor = mInputEditor.GetTextBackgroundColor().GetRgba();
      if(currentColor == Vector4::ZERO)
      {
        mInputEditor.SetTextBackgroundColor(UiColor(0xF0F8FF)); // Alice Blue
      }
      else if(currentColor == UiColor(0xF0F8FF))
      {
        mInputEditor.SetTextBackgroundColor(UiColor(0xFFF0F5)); // Lavender Blush
      }
      else if(currentColor == UiColor(0xFFF0F5))
      {
        mInputEditor.SetTextBackgroundColor(UiColor(0xF0FFF0)); // Honeydew
      }
      else
      {
        mInputEditor.SetTextBackgroundColor(UiColor(0xF0F8FF));
      }
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonClearTextBgTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputEditor.ClearTextBackgroundColor();
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonLineWrapModeTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      Text::LineWrapMode mode = mInputEditor.GetLineWrapMode();
      Text::LineWrapMode newMode;
      // Cycle through: WORD -> CHARACTER -> HYPHENATION -> MIXED -> WORD
      if(mode == Text::LineWrapMode::WORD)
      {
        newMode = Text::LineWrapMode::CHARACTER;
      }
      else if(mode == Text::LineWrapMode::CHARACTER)
      {
        newMode = Text::LineWrapMode::HYPHENATION;
      }
      else if(mode == Text::LineWrapMode::HYPHENATION)
      {
        newMode = Text::LineWrapMode::MIXED;
      }
      else
      {
        newMode = Text::LineWrapMode::WORD;
      }
      mInputEditor.SetLineWrapMode(newMode);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonLineHeightTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      float lineHeight      = mInputEditor.GetLineHeight();
      float fontSize        = mInputEditor.GetFontSize();
      Text::LineHeightMode mode = mInputEditor.GetLineHeightMode();

      LineHeightStep step = GetLineHeightStep(lineHeight, fontSize, mode);
      LineHeightStep newStep;
      // Cycle through: AUTO -> ONE -> ONE_AND_HALF -> TWO -> AUTO
      switch(step)
      {
        case LineHeightStep::AUTO:
          newStep = LineHeightStep::ONE;
          break;
        case LineHeightStep::ONE:
          newStep = LineHeightStep::ONE_AND_HALF;
          break;
        case LineHeightStep::ONE_AND_HALF:
          newStep = LineHeightStep::TWO;
          break;
        case LineHeightStep::TWO:
        default:
          newStep = LineHeightStep::AUTO;
          break;
      }

      float newLineHeight;
      if(newStep == LineHeightStep::AUTO)
      {
        newLineHeight = Text::LINE_HEIGHT_AUTO;
      }
      else if(mode == Text::LineHeightMode::RELATIVE)
      {
        switch(newStep)
        {
          case LineHeightStep::ONE:
            newLineHeight = 1.0f;
            break;
          case LineHeightStep::ONE_AND_HALF:
            newLineHeight = 1.5f;
            break;
          case LineHeightStep::TWO:
            newLineHeight = 2.0f;
            break;
          default:
            newLineHeight = Text::LINE_HEIGHT_AUTO;
            break;
        }
      }
      else // ABSOLUTE
      {
        switch(newStep)
        {
          case LineHeightStep::ONE:
            newLineHeight = fontSize * 1.0f;
            break;
          case LineHeightStep::ONE_AND_HALF:
            newLineHeight = fontSize * 1.5f;
            break;
          case LineHeightStep::TWO:
            newLineHeight = fontSize * 2.0f;
            break;
          default:
            newLineHeight = Text::LINE_HEIGHT_AUTO;
            break;
        }
      }

      mInputEditor.SetLineHeight(newLineHeight);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonLineHeightModeTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      float lineHeight      = mInputEditor.GetLineHeight();
      float fontSize        = mInputEditor.GetFontSize();
      Text::LineHeightMode mode = mInputEditor.GetLineHeightMode();

      Text::LineHeightMode newMode;
      float newLineHeight;

      if(mode == Text::LineHeightMode::RELATIVE)
      {
        // RELATIVE -> ABSOLUTE
        newMode = Text::LineHeightMode::ABSOLUTE;
        if(Dali::Equals(lineHeight, Text::LINE_HEIGHT_AUTO, Math::MACHINE_EPSILON_1000))
        {
          newLineHeight = Text::LINE_HEIGHT_AUTO;
        }
        else
        {
          newLineHeight = lineHeight * fontSize;
        }
      }
      else
      {
        // ABSOLUTE -> RELATIVE
        newMode = Text::LineHeightMode::RELATIVE;
        if(Dali::Equals(lineHeight, Text::LINE_HEIGHT_AUTO, Math::MACHINE_EPSILON_1000))
        {
          newLineHeight = Text::LINE_HEIGHT_AUTO;
        }
        else if(fontSize > Math::MACHINE_EPSILON_1000)
        {
          newLineHeight = lineHeight / fontSize;
        }
        else
        {
          newLineHeight = 1.0f;
        }
      }

      mInputEditor.SetLineHeightMode(newMode);
      mInputEditor.SetLineHeight(newLineHeight);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonHorizontalAlignmentTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      Text::Alignment alignment = mInputEditor.GetHorizontalTextAlignment();
      Text::Alignment newAlignment;
      // Cycle through: START -> CENTER -> END -> START
      if(alignment == Text::Alignment::START)
      {
        newAlignment = Text::Alignment::CENTER;
      }
      else if(alignment == Text::Alignment::CENTER)
      {
        newAlignment = Text::Alignment::END;
      }
      else
      {
        newAlignment = Text::Alignment::START;
      }
      mInputEditor.SetHorizontalTextAlignment(newAlignment);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonVerticalAlignmentTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      Text::Alignment alignment = mInputEditor.GetVerticalTextAlignment();
      Text::Alignment newAlignment;
      // Cycle through: START -> CENTER -> END -> START
      if(alignment == Text::Alignment::START)
      {
        newAlignment = Text::Alignment::CENTER;
      }
      else if(alignment == Text::Alignment::CENTER)
      {
        newAlignment = Text::Alignment::END;
      }
      else
      {
        newAlignment = Text::Alignment::START;
      }
      mInputEditor.SetVerticalTextAlignment(newAlignment);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonAutoGrowTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      bool enabled = mInputEditor.IsAutoGrowEnabled();
      mInputEditor.SetAutoGrowEnabled(!enabled);
      const float height = mInputEditor.IsAutoGrowEnabled() ? WRAP_CONTENT : EDITOR_HEIGHT;
      mInputEditor.SetRequestedHeight(height);
      UpdateStatus();
    }
    return true;
  }

  bool OnButtonInfoTouched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      PrintInputEditorInfo();
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

  void PrintInputEditorInfo()
  {
  }

private:
  Application& mApplication;
  InputEditor  mInputEditor;
  Label        mStatusLabel;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  InputEditorController controller(application);
  application.MainLoop();

  return 0;
}
