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

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float STACK_SPACING = 12.0f;
constexpr float STACK_PADDING = 20.0f;
constexpr float LABEL_HEIGHT  = 66.0f;

const char* SINGLE_LINE_TEXT = "TextFit single line example text";
const char* MULTI_LINE_TEXT  = "This is a multi line TextFit example. The font size should be adjusted to fit the available space.";

Label CreateSectionLabel(const char* text)
{
  Label label = Label::New(text);
  label.SetFontSize(16.0f);
  label.SetBackgroundColor(UiColor(0xE0E0E0));
  label.SetPadding(Insets(10.0f, 10.0f, 10.0f, 10.0f));
  return label;
}

Label CreateFitLabel(const char* text, float width, float height, bool multiLine)
{
  Label label = Label::New(text);
  label.SetRequestedWidth(width);
  label.SetRequestedHeight(height);
  label.SetMultiLine(multiLine);
  label.SetFontSize(20.0f);
  label.SetBackgroundColor(UiColor(0xEFEFEF));
  label.SetPadding(Insets(10.0f, 10.0f, 10.0f, 10.0f));
  return label;
}

Dali::String FormatTextFit(const Text::Fit& fit)
{
  char buffer[160];

  switch(fit.GetType())
  {
    case Text::Fit::Type::RANGE:
    {
      const Text::Fit::Range& range = fit.GetRange();
      std::snprintf(buffer,
                    sizeof(buffer),
                    "GetTextFit: RANGE min=%.1f max=%.1f step=%.1f",
                    range.GetMinimumFontSize(),
                    range.GetMaximumFontSize(),
                    range.GetFontSizeStep());
      return Dali::String(buffer);
    }
    case Text::Fit::Type::CANDIDATES:
    {
      const Dali::Vector<Text::Fit::Candidate>& candidates = fit.GetCandidates();
      std::snprintf(buffer, sizeof(buffer), "GetTextFit: CANDIDATES count=%u", static_cast<unsigned int>(candidates.Count()));
      return Dali::String(buffer);
    }
    case Text::Fit::Type::NONE:
    default:
    {
      return Dali::String("GetTextFit: NONE");
    }
  }
}
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

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetSpacing(STACK_SPACING);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Insets(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING));

    Label titleLabel = Label::New("TextFit Example");
    titleLabel.SetFontSize(24.0f);
    root.Add(titleLabel);

    Label instructionLabel = Label::New("Press '1' to clear TextFit, '2' to set TextFit");
    instructionLabel.SetFontSize(14.0f);
    instructionLabel.SetBackgroundColor(UiColor(0xE0E0E0));
    instructionLabel.SetPadding(Insets(10.0f, 10.0f, 10.0f, 10.0f));
    root.Add(instructionLabel);

    mStatusLabel = Label::New("");
    mStatusLabel.SetFontSize(14.0f);
    mStatusLabel.SetBackgroundColor(UiColor(0xE8F2FF));
    mStatusLabel.SetPadding(Insets(10.0f, 10.0f, 8.0f, 8.0f));
    root.Add(mStatusLabel);

    root.Add(CreateSectionLabel("1. MATCH_PARENT + fixed height / single line"));
    mMatchFixedSingleLine = CreateFitLabel(SINGLE_LINE_TEXT, MATCH_PARENT, LABEL_HEIGHT, false);
    mMatchFixedSingleLine.SetTextFit(Text::Fit::Range());
    root.Add(mMatchFixedSingleLine);

    root.Add(CreateSectionLabel("2. MATCH_PARENT + fixed height / multi line"));
    mMatchFixedMultiLine = CreateFitLabel(MULTI_LINE_TEXT, MATCH_PARENT, LABEL_HEIGHT * 2.0f, true);
    mMatchFixedMultiLine.SetTextFit(Text::Fit::Range(16.0f, 32.0f, 4.0f));
    root.Add(mMatchFixedMultiLine);

    root.Add(CreateSectionLabel("3. WRAP_CONTENT + WRAP_CONTENT / single line"));
    mWrapWrapSingleLine = CreateFitLabel(SINGLE_LINE_TEXT, WRAP_CONTENT, WRAP_CONTENT, false);
    mWrapWrapSingleLine.SetTextFit(Text::Fit::Range());
    root.Add(mWrapWrapSingleLine);

    root.Add(CreateSectionLabel("4. WRAP_CONTENT + WRAP_CONTENT / multi line"));
    mWrapWrapMultiLine = CreateFitLabel(MULTI_LINE_TEXT, WRAP_CONTENT, WRAP_CONTENT, true);
    mWrapWrapMultiLine.SetMaximumHeight(200);
    mWrapWrapMultiLine.SetTextFit(Text::Fit::Range(16.0f, 32.0f, 4.0f));
    root.Add(mWrapWrapMultiLine);

    root.Add(CreateSectionLabel("5. MATCH_PARENT + fixed height / multi line / relative line height"));
    mFixedMultiLineRelativeLineHeight = CreateFitLabel(MULTI_LINE_TEXT, MATCH_PARENT, LABEL_HEIGHT * 2.0f, true);
    mFixedMultiLineRelativeLineHeight.SetLineHeight(1.5f);
    mFixedMultiLineRelativeLineHeight.SetLineHeightMode(Text::LineHeightMode::RELATIVE);
    mFixedMultiLineRelativeLineHeight.SetTextFit(Text::Fit::Range(16.0f, 32.0f, 4.0f));
    root.Add(mFixedMultiLineRelativeLineHeight);

    root.Add(CreateSectionLabel("6. MATCH_PARENT + fixed height / multi line / absolute line height"));
    mFixedMultiLineAbsoluteLineHeight = CreateFitLabel(MULTI_LINE_TEXT, MATCH_PARENT, LABEL_HEIGHT * 2.0f, true);
    mFixedMultiLineAbsoluteLineHeight.SetLineHeight(30.0f);
    mFixedMultiLineAbsoluteLineHeight.SetLineHeightMode(Text::LineHeightMode::ABSOLUTE);
    mFixedMultiLineAbsoluteLineHeight.SetTextFit(Text::Fit::Range(16.0f, 32.0f, 4.0f));
    root.Add(mFixedMultiLineAbsoluteLineHeight);

    UpdateStatusLabel();

    window.Add(root);

    window.KeyEventSignal().Connect(this, &TextFitController::OnKeyEvent);
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
      mMatchFixedSingleLine.SetTextFit(Text::Fit::None());
      mMatchFixedMultiLine.SetTextFit(Text::Fit::None());
      mWrapWrapSingleLine.SetTextFit(Text::Fit::None());
      mWrapWrapMultiLine.SetTextFit(Text::Fit::None());
      mFixedMultiLineRelativeLineHeight.SetTextFit(Text::Fit::None());
      mFixedMultiLineAbsoluteLineHeight.SetTextFit(Text::Fit::None());
      UpdateStatusLabel();
    }
    else if(event.GetKeyName() == "2")
    {
      mMatchFixedSingleLine.SetTextFit(Text::Fit::Range(16.0f, 32.0f, 8.0f));
      mMatchFixedMultiLine.SetTextFit(Text::Fit::Range());
      mWrapWrapSingleLine.SetTextFit(Text::Fit::Range(16.0f, 32.0f, 8.0f));
      mWrapWrapMultiLine.SetTextFit(Text::Fit::Range());
      mFixedMultiLineRelativeLineHeight.SetTextFit(Text::Fit::Range(16.0f, 32.0f, 4.0f));
      mFixedMultiLineAbsoluteLineHeight.SetTextFit(Text::Fit::Range(16.0f, 32.0f, 4.0f));
      UpdateStatusLabel();
    }
  }

  void UpdateStatusLabel()
  {
    mStatusLabel.SetText(FormatTextFit(mMatchFixedSingleLine.GetTextFit()));
  }

private:
  Application& mApplication;
  Label        mStatusLabel;
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
