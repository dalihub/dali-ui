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
constexpr float STACK_SPACING = 12.0f;
constexpr float STACK_PADDING = 20.0f;
constexpr float LABEL_HEIGHT  = 66.0f;

const char* SINGLE_LINE_TEXT = "TextFit single line example text";
const char* MULTI_LINE_TEXT  = "This is a multi line TextFit example. The font size should be adjusted to fit the available space.";
} // namespace

class TextFitController : public ConnectionTracker
{
public:
  explicit TextFitController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextFitController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xFFFFFF));

    window.Add(
      StackLayout::New(StackOrientation::VERTICAL)
        .SetSpacing(STACK_SPACING)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(MATCH_PARENT)
        .SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
        .Children({

          Label::New("TextFit Example")
            .SetFontSize(24.0f),

          Label::New("Press '1' to clear TextFit, '2' to set TextFit")
            .SetFontSize(14.0f)
            .SetBackgroundColor(UiColor(0xE0E0E0))
            .SetPadding(Extents(10, 10, 10, 10)),

          // 1. MATCH_PARENT  fixed height / single line
          Label::New("1. MATCH_PARENT + fixed height / single line")
            .SetFontSize(16.0f)
            .SetBackgroundColor(UiColor(0xE0E0E0))
            .SetPadding(Extents(10, 10, 10, 10)),

          Label::New(SINGLE_LINE_TEXT)
            .SetRequestedWidth(MATCH_PARENT)
            .SetRequestedHeight(LABEL_HEIGHT)
            .SetMultiLine(false)
            .SetFontSize(20.0f)
            .SetBackgroundColor(UiColor(0xEFEFEF))
            .SetPadding(Extents(10, 10, 10, 10))
            .SetTextFit(Text::FitRange())
            .As(mMatchFixedSingleLine),

          // 2. MATCH_PARENT + fixed height / multi line
          Label::New("2. MATCH_PARENT + fixed height / multi line")
            .SetFontSize(16.0f)
            .SetBackgroundColor(UiColor(0xE0E0E0))
            .SetPadding(Extents(10, 10, 10, 10)),

          Label::New(MULTI_LINE_TEXT)
            .SetRequestedWidth(MATCH_PARENT)
            .SetRequestedHeight(LABEL_HEIGHT * 2.0f)
            .SetMultiLine(true)
            .SetFontSize(20.0f)
            .SetBackgroundColor(UiColor(0xEFEFEF))
            .SetPadding(Extents(10, 10, 10, 10))
            .SetTextFit(Text::FitRange(16.0f, 32.0f, 4.0f))
            .As(mMatchFixedMultiLine),

          // 3. WRAP_CONTENT + WRAP_CONTENT / single line
          Label::New("3. WRAP_CONTENT + WRAP_CONTENT / single line")
            .SetFontSize(16.0f)
            .SetBackgroundColor(UiColor(0xE0E0E0))
            .SetPadding(Extents(10, 10, 10, 10)),

          Label::New(SINGLE_LINE_TEXT)
            .SetRequestedWidth(WRAP_CONTENT)
            .SetRequestedHeight(WRAP_CONTENT)
            .SetMultiLine(false)
            .SetFontSize(20.0f)
            .SetBackgroundColor(UiColor(0xEFEFEF))
            .SetPadding(Extents(10, 10, 10, 10))
            .SetTextFit(Text::FitRange())
            .As(mWrapWrapSingleLine),

          // 4. WRAP_CONTENT + WRAP_CONTENT / multi line
          Label::New("4. WRAP_CONTENT + WRAP_CONTENT / multi line")
            .SetFontSize(16.0f)
            .SetBackgroundColor(UiColor(0xE0E0E0))
            .SetPadding(Extents(10, 10, 10, 10)),

          Label::New(MULTI_LINE_TEXT)
            .SetRequestedWidth(WRAP_CONTENT)
            .SetRequestedHeight(WRAP_CONTENT)
            .SetMaximumHeight(200)
            .SetMultiLine(true)
            .SetFontSize(20.0f)
            .SetBackgroundColor(UiColor(0xEFEFEF))
            .SetPadding(Extents(10, 10, 10, 10))
            .SetTextFit(Text::FitRange(16.0f, 32.0f, 4.0f))
            .As(mWrapWrapMultiLine),

          // 5. MATCH_PARENT + fixed height / multi line / relative line height
          Label::New("5. MATCH_PARENT + fixed height / multi line / relative line height")
            .SetFontSize(16.0f)
            .SetBackgroundColor(UiColor(0xE0E0E0))
            .SetPadding(Extents(10, 10, 10, 10)),

          Label::New(MULTI_LINE_TEXT)
            .SetRequestedWidth(MATCH_PARENT)
            .SetRequestedHeight(LABEL_HEIGHT * 2.0f)
            .SetMultiLine(true)
            .SetFontSize(20.0f)
            .SetLineHeight(1.5f)
            .SetLineHeightMode(Text::LineHeightMode::RELATIVE)
            .SetBackgroundColor(UiColor(0xEFEFEF))
            .SetPadding(Extents(10, 10, 10, 10))
            .SetTextFit(Text::FitRange(16.0f, 32.0f, 4.0f))
            .As(mFixedMultiLineRelativeLineHeight),

          // 6. MATCH_PARENT + fixed height / multi line / absolute line height
          Label::New("6. MATCH_PARENT + fixed height / multi line / absolute line height")
            .SetFontSize(16.0f)
            .SetBackgroundColor(UiColor(0xE0E0E0))
            .SetPadding(Extents(10, 10, 10, 10)),

          Label::New(MULTI_LINE_TEXT)
            .SetRequestedWidth(MATCH_PARENT)
            .SetRequestedHeight(LABEL_HEIGHT * 2.0f)
            .SetMultiLine(true)
            .SetFontSize(20.0f)
            .SetLineHeight(30.0f)
            .SetLineHeightMode(Text::LineHeightMode::ABSOLUTE)
            .SetBackgroundColor(UiColor(0xEFEFEF))
            .SetPadding(Extents(10, 10, 10, 10))
            .SetTextFit(Text::FitRange(16.0f, 32.0f, 4.0f))
            .As(mFixedMultiLineAbsoluteLineHeight),
        }));

    window.KeyEventSignal().Connect(this, &TextFitController::OnKeyEvent);
  }

  void OnKeyEvent(KeyEvent event)
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
      mMatchFixedSingleLine.ClearTextFit();
      mMatchFixedMultiLine.ClearTextFit();
      mWrapWrapSingleLine.ClearTextFit();
      mWrapWrapMultiLine.ClearTextFit();
      mFixedMultiLineRelativeLineHeight.ClearTextFit();
      mFixedMultiLineAbsoluteLineHeight.ClearTextFit();
    }
    else if(event.GetKeyName() == "2")
    {
      mMatchFixedSingleLine.SetTextFit(Text::FitRange(16.0f, 32.0f, 8.0f));
      mMatchFixedMultiLine.SetTextFit(Text::FitRange());
      mWrapWrapSingleLine.SetTextFit(Text::FitRange(16.0f, 32.0f, 8.0f));
      mWrapWrapMultiLine.SetTextFit(Text::FitRange());
      mFixedMultiLineRelativeLineHeight.SetTextFit(Text::FitRange(16.0f, 32.0f, 4.0f));
      mFixedMultiLineAbsoluteLineHeight.SetTextFit(Text::FitRange(16.0f, 32.0f, 4.0f));
    }
  }

private:
  Application& mApplication;
  Label        mMatchFixedSingleLine;
  Label        mMatchFixedMultiLine;
  Label        mWrapWrapSingleLine;
  Label        mWrapWrapMultiLine;
  Label        mFixedMultiLineRelativeLineHeight;
  Label        mFixedMultiLineAbsoluteLineHeight;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextFitController controller(application);
  application.MainLoop();

  return 0;
}
