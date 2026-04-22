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
constexpr float TITLE_FONT_SIZE = 30.0f;
constexpr float INFO_FONT_SIZE  = 14.0f;

constexpr uint32_t COLOR_WHITE        = 0xFFFFFF;
constexpr uint32_t COLOR_BLACK        = 0x000000;
constexpr uint32_t COLOR_LIGHT_TEXT   = 0xF5F5F5;
constexpr uint32_t COLOR_DARK_TEXT    = 0x222222;
constexpr uint32_t COLOR_LIGHT_BG     = 0xF2F2F2;
constexpr uint32_t COLOR_DARK_GRAY    = 0x404040;
constexpr uint32_t COLOR_LIGHT_BLUE   = 0xADD8E6;
constexpr uint32_t COLOR_YELLOW       = 0xFFFF00;
constexpr uint32_t COLOR_CYAN         = 0x00FFFF;
constexpr uint32_t COLOR_MAGENTA      = 0xFF00FF;
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
    window.SetBackgroundColor(UiColor(COLOR_WHITE));

    window.Add(CreateContents());

    // Enable focus on InputField so focus can be gained/lost easily
    mInputField.SetFocusable(true);

    // Connect signals
    mInputField.TextChangedSignal().Connect(this, &InputFieldController::OnTextChanged);
    mInputField.MaximumLengthReachedSignal().Connect(this, &InputFieldController::OnMaximumLengthReached);

    PrintInputFieldInfo();

    window.KeyEventSignal().Connect(this, &InputFieldController::OnKeyEvent);
  }

  View CreateContents()
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetSpacing(STACK_SPACING)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
      .Children({
        // Title
        Label::New("InputField Example")
          .SetFontSize(TITLE_FONT_SIZE)
          .SetTextColor(UiColor(COLOR_DARK_TEXT))
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(WRAP_CONTENT)
          .SetFocusable(true),
        // InputField
        CreateInputField().As(mInputField),
        // Info label to show key bindings
        CreateInfoLabel().As(mInfoLabel),
      });
  }

  InputField CreateInputField()
  {
    return InputField::New()
      .SetPlaceholder("Type here...")
      .SetPlaceholderColor(UiColor(COLOR_DARK_GRAY))
      .SetFontSize(INPUT_FONT_SIZE)
      .SetCursorWidth(2)
      .SetCursorColor(UiColor(COLOR_DARK_TEXT))
      .SetSelectionColor(UiColor(COLOR_LIGHT_BLUE))
      .SetMaximumLength(50)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetPadding(Extents(20, 20, 20, 20))
      .SetBackgroundColor(UiColor(COLOR_LIGHT_BG))
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetVerticalTextAlignment(Text::Alignment::CENTER);
  }

  Label CreateInfoLabel()
  {
    return Label::New()
      .SetText(GetKeyHelpText())
      .SetFontSize(INFO_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetMultiLine(true);
  }

  Dali::String GetKeyHelpText() const
  {
    return Dali::String(
      "[Key bindings]\n"
      "1: Toggle cursor blink\n"
      "2: Change cursor blink interval\n"
      "3: Change cursor position\n"
      "4: Toggle show placeholder on focus\n"
      "5: Change placeholder color\n"
      "6: Change cursor width\n"
      "7: Change selection color\n"
      "8: Change maximum length\n"
      "9: Print InputField info\n"
      "ESC: Quit"
    );
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
    DALI_LOG_ERROR("MaximumLength         : %d\n", mInputField.GetMaximumLength());
    DALI_LOG_ERROR("TextColor             : %.2f, %.2f, %.2f, %.2f\n", textColor.r, textColor.g, textColor.b, textColor.a);
    DALI_LOG_ERROR("FontSize              : %f\n", mInputField.GetFontSize());
    DALI_LOG_ERROR("----------------------------------------------------------------\n");
  }

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
      // Toggle cursor blink enabled
      bool enabled = mInputField.IsCursorBlinkEnabled();
      mInputField.SetCursorBlinkEnabled(!enabled);
      DALI_LOG_ERROR("CursorBlinkEnabled: %d -> %d\n", enabled, !enabled);
    }
    else if(event.GetKeyName() == "2")
    {
      // Cycle cursor blink interval
      float interval = mInputField.GetCursorBlinkInterval();
      float newInterval = (interval < 0.8f) ? interval + 0.2f : 0.2f;
      mInputField.SetCursorBlinkInterval(newInterval);
      DALI_LOG_ERROR("CursorBlinkInterval: %f -> %f\n", interval, newInterval);
    }
    else if(event.GetKeyName() == "3")
    {
      // Move cursor position
      uint32_t position = mInputField.GetCursorPosition();
      uint32_t textLength = static_cast<uint32_t>(mInputField.GetText().Size());
      uint32_t newPosition = (position < textLength) ? position + 1u : 0u;
      mInputField.SetCursorPosition(newPosition);
      DALI_LOG_ERROR("CursorPosition: %u -> %u (textLength: %u)\n", position, newPosition, textLength);
    }
    else if(event.GetKeyName() == "4")
    {
      // Toggle show placeholder on focus
      bool shown = mInputField.IsPlaceholderShownOnFocus();
      mInputField.SetShowPlaceholderOnFocus(!shown);
      DALI_LOG_ERROR("ShowPlaceholderOnFocus: %d -> %d\n", shown, !shown);
    }
    else if(event.GetKeyName() == "5")
    {
      // Cycle placeholder color
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
    }
    else if(event.GetKeyName() == "6")
    {
      // Cycle cursor width
      int width = mInputField.GetCursorWidth();
      int newWidth = (width < 6) ? width + 2 : 1;
      mInputField.SetCursorWidth(newWidth);
      DALI_LOG_ERROR("CursorWidth: %d -> %d\n", width, newWidth);
    }
    else if(event.GetKeyName() == "7")
    {
      // Cycle selection color
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
    }
    else if(event.GetKeyName() == "8")
    {
      // Cycle maximum length
      int maxLength = mInputField.GetMaximumLength();
      int newMaxLength = (maxLength <= 10) ? 20 : (maxLength <= 20) ? 50 : 10;
      mInputField.SetMaximumLength(newMaxLength);
      DALI_LOG_ERROR("MaximumLength: %d -> %d\n", maxLength, newMaxLength);
    }
    else if(event.GetKeyName() == "9")
    {
      PrintInputFieldInfo();
    }
  }

private:
  Application& mApplication;
  InputField   mInputField;
  Label        mInfoLabel;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  DALI_LOG_ERROR("DaliUiFoundationPreInitialize START\n");
  DaliUiFoundationPreInitialize(nullptr, nullptr, nullptr);
  DALI_LOG_ERROR("DaliUiFoundationPreInitialize END\n");

  Application application = Application::New(&argc, &argv);
  UiConfig::New()
    .SetDefaultPlaceholderTextColor(Color::DARK_GRAY)
    .SetShowPlaceholderTextOnFocus(false)
    .Apply();

  InputFieldController controller(application);
  application.MainLoop();

  return 0;
}