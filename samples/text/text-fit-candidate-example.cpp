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

const char* SINGLE_LINE_TEXT = "TextFit candidate single line example";
const char* MULTI_LINE_TEXT  = "This is a multi line TextFit candidate example. The font size and line height should be adjusted to fit the available space.";

const Dali::Vector<Text::Fit::Candidate>& GetFitCandidates()
{
  static Dali::Vector<Text::Fit::Candidate> candidates = [] {
    Dali::Vector<Text::Fit::Candidate> values;
    values.PushBack(Text::Fit::Candidate(16.0f, 32.0f));
    values.PushBack(Text::Fit::Candidate(20.0f, 40.0f));
    values.PushBack(Text::Fit::Candidate(24.0f, 48.0f));
    values.PushBack(Text::Fit::Candidate(28.0f, 56.0f));
    values.PushBack(Text::Fit::Candidate(32.0f, 64.0f));
    return values;
  }();
  return candidates;
}

const Dali::Vector<Text::Fit::Candidate>& GetAlternativeCandidates()
{
  static Dali::Vector<Text::Fit::Candidate> candidates = [] {
    Dali::Vector<Text::Fit::Candidate> values;
    values.PushBack(Text::Fit::Candidate(16.0f, 30.0f));
    values.PushBack(Text::Fit::Candidate(20.0f, 30.0f));
    values.PushBack(Text::Fit::Candidate(24.0f, 40.0f));
    values.PushBack(Text::Fit::Candidate(28.0f, 40.0f));
    values.PushBack(Text::Fit::Candidate(32.0f, 60.0f));
    return values;
  }();
  return candidates;
}

const Dali::Vector<Text::Fit::Candidate>& GetFontSizeCandidates()
{
  static Dali::Vector<Text::Fit::Candidate> candidates = [] {
    Dali::Vector<Text::Fit::Candidate> values;
    auto addCandidate = [&values](float fontSize) {
      Text::Fit::Candidate candidate;
      candidate.SetFontSize(fontSize);
      values.PushBack(candidate);
    };
    addCandidate(16.0f);
    addCandidate(20.0f);
    addCandidate(24.0f);
    addCandidate(28.0f);
    addCandidate(32.0f);
    return values;
  }();
  return candidates;
}

Label CreateSectionLabel(const char* text)
{
  Label label = Label::New(text);
  label.SetFontSize(16.0f);
  label.SetBackgroundColor(UiColor(0xE0E0E0));
  label.SetPadding(Extents(10, 10, 10, 10));
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
  label.SetPadding(Extents(10, 10, 10, 10));
  label.SetTextFit(GetFitCandidates());
  return label;
}

Dali::String FormatTextFit(const Text::Fit& fit)
{
  char buffer[192];

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
      if(candidates.Empty())
      {
        return Dali::String("GetTextFit: CANDIDATES count=0");
      }

      const Text::Fit::Candidate& first = candidates[0];
      std::snprintf(buffer,
                    sizeof(buffer),
                    "GetTextFit: CANDIDATES count=%u first=(font %.1f, line %.1f)",
                    static_cast<unsigned int>(candidates.Count()),
                    first.GetFontSize(),
                    first.GetLineHeight());
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

class TextFitCandidateController : public ConnectionTracker
{
public:
  explicit TextFitCandidateController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextFitCandidateController::OnInit);
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
    root.SetPadding(Extents(static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING)));

    Label titleLabel = Label::New("TextFit Candidate Example");
    titleLabel.SetFontSize(24.0f);
    root.Add(titleLabel);

    Label instructionLabel = Label::New("Press '1' to clear TextFit, '2' and '3' to set alternative candidates");
    instructionLabel.SetFontSize(14.0f);
    instructionLabel.SetBackgroundColor(UiColor(0xE0E0E0));
    instructionLabel.SetPadding(Extents(10, 10, 10, 10));
    root.Add(instructionLabel);

    mStatusLabel = Label::New("");
    mStatusLabel.SetFontSize(14.0f);
    mStatusLabel.SetBackgroundColor(UiColor(0xE8F2FF));
    mStatusLabel.SetPadding(Extents(10, 10, 8, 8));
    root.Add(mStatusLabel);

    root.Add(CreateSectionLabel("1. MATCH_PARENT + fixed height / single line"));
    mFixedSingleLineLabel = CreateFitLabel(SINGLE_LINE_TEXT, MATCH_PARENT, LABEL_HEIGHT, false);
    root.Add(mFixedSingleLineLabel);

    root.Add(CreateSectionLabel("2. MATCH_PARENT + fixed height / multi line"));
    mFixedMultiLineLabel = CreateFitLabel(MULTI_LINE_TEXT, MATCH_PARENT, LABEL_HEIGHT * 2.0f, true);
    root.Add(mFixedMultiLineLabel);

    root.Add(CreateSectionLabel("3. WRAP_CONTENT + WRAP_CONTENT / single line"));
    mWrapWrapSingleLine = CreateFitLabel(SINGLE_LINE_TEXT, WRAP_CONTENT, WRAP_CONTENT, false);
    root.Add(mWrapWrapSingleLine);

    root.Add(CreateSectionLabel("4. WRAP_CONTENT + WRAP_CONTENT / multi line"));
    mWrapWrapMultiLine = CreateFitLabel(MULTI_LINE_TEXT, WRAP_CONTENT, WRAP_CONTENT, true);
    mWrapWrapMultiLine.SetMaximumHeight(200);
    root.Add(mWrapWrapMultiLine);

    UpdateStatusLabel();

    window.Add(root);

    window.KeyEventSignal().Connect(this, &TextFitCandidateController::OnKeyEvent);
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
      mFixedSingleLineLabel.SetTextFit(Text::Fit::None());
      mFixedMultiLineLabel.SetTextFit(Text::Fit::None());
      mWrapWrapSingleLine.SetTextFit(Text::Fit::None());
      mWrapWrapMultiLine.SetTextFit(Text::Fit::None());
      UpdateStatusLabel();
    }
    else if(event.GetKeyName() == "2")
    {
      mFixedSingleLineLabel.SetTextFit(GetAlternativeCandidates());
      mFixedMultiLineLabel.SetTextFit(GetAlternativeCandidates());
      mWrapWrapSingleLine.SetTextFit(GetAlternativeCandidates());
      mWrapWrapMultiLine.SetTextFit(GetAlternativeCandidates());
      UpdateStatusLabel();
    }
    else if(event.GetKeyName() == "3")
    {
      mFixedSingleLineLabel.SetTextFit(GetFontSizeCandidates());
      mFixedMultiLineLabel.SetTextFit(GetFontSizeCandidates());
      mWrapWrapSingleLine.SetTextFit(GetFontSizeCandidates());
      mWrapWrapMultiLine.SetTextFit(GetFontSizeCandidates());
      UpdateStatusLabel();
    }
  }

  void UpdateStatusLabel()
  {
    mStatusLabel.SetText(FormatTextFit(mFixedSingleLineLabel.GetTextFit()));
  }

private:
  Application& mApplication;
  Label        mStatusLabel;
  Label        mFixedSingleLineLabel;
  Label        mFixedMultiLineLabel;
  Label        mWrapWrapSingleLine;
  Label        mWrapWrapMultiLine;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextFitCandidateController controller(application);
  application.MainLoop();

  return 0;
}
