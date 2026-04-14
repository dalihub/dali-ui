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

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float STACK_SPACING = 12.0f;
constexpr float STACK_PADDING = 20.0f;
constexpr float LABEL_HEIGHT  = 66.0f;

const char* SINGLE_LINE_TEXT = "TextFit candidate single line example";
const char* MULTI_LINE_TEXT  = "This is a multi line TextFit candidate example. The font size and line height should be adjusted to fit the available space.";

const Dali::Vector<Text::FitCandidate>& GetFitCandidates()
{
  static Dali::Vector<Text::FitCandidate> candidates = [] {
    Dali::Vector<Text::FitCandidate> values;
    values.PushBack(Text::FitCandidate(16.0f, 32.0f));
    values.PushBack(Text::FitCandidate(20.0f, 40.0f));
    values.PushBack(Text::FitCandidate(24.0f, 48.0f));
    values.PushBack(Text::FitCandidate(28.0f, 56.0f));
    values.PushBack(Text::FitCandidate(32.0f, 64.0f));
    return values;
  }();
  return candidates;
}

const Dali::Vector<Text::FitCandidate>& GetAlternativeCandidates()
{
  static Dali::Vector<Text::FitCandidate> candidates = [] {
    Dali::Vector<Text::FitCandidate> values;
    values.PushBack(Text::FitCandidate(16.0f, 30.0f));
    values.PushBack(Text::FitCandidate(20.0f, 30.0f));
    values.PushBack(Text::FitCandidate(24.0f, 40.0f));
    values.PushBack(Text::FitCandidate(28.0f, 40.0f));
    values.PushBack(Text::FitCandidate(32.0f, 60.0f));
    return values;
  }();
  return candidates;
}

const Dali::Vector<Text::FitCandidate>& GetFontSizeCandidates()
{
  static Dali::Vector<Text::FitCandidate> candidates = [] {
    Dali::Vector<Text::FitCandidate> values;
    values.PushBack(Text::FitCandidate().SetFontSize(16.0f));
    values.PushBack(Text::FitCandidate().SetFontSize(20.0f));
    values.PushBack(Text::FitCandidate().SetFontSize(24.0f));
    values.PushBack(Text::FitCandidate().SetFontSize(28.0f));
    values.PushBack(Text::FitCandidate().SetFontSize(32.0f));
    return values;
  }();
  return candidates;
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
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xFFFFFF));

    window.Add(
      StackLayout::New(StackOrientation::VERTICAL)
        .Spacing(STACK_SPACING)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(MATCH_PARENT)
        .SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
        .Children({

          Label::New("TextFit Candidate Example")
            .SetFontSize(24.0f),

          Label::New("Press '1' to clear TextFit, '2' and '3' to set alternative candidates")
            .SetFontSize(14.0f)
            .SetBackgroundColor(UiColor(0xE0E0E0))
            .SetPadding(Extents(10, 10, 10, 10)),

          // 1. MATCH_PARENT + fixed height / single line
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
            .SetTextFit(GetFitCandidates())
            .As(mFixedSingleLineLabel),

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
            .SetTextFit(GetFitCandidates())
            .As(mFixedMultiLineLabel),

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
            .SetTextFit(GetFitCandidates())
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
            .SetTextFit(GetFitCandidates())
            .As(mWrapWrapMultiLine),
        }));

    window.KeyEventSignal().Connect(this, &TextFitCandidateController::OnKeyEvent);
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
      mFixedSingleLineLabel.ClearTextFit();
      mFixedMultiLineLabel.ClearTextFit();
      mWrapWrapSingleLine.ClearTextFit();
      mWrapWrapMultiLine.ClearTextFit();
      DALI_LOG_ERROR("Cleared TextFit on all labels\n");
    }
    else if(event.GetKeyName() == "2")
    {
      mFixedSingleLineLabel.SetTextFit(GetAlternativeCandidates());
      mFixedMultiLineLabel.SetTextFit(GetAlternativeCandidates());
      mWrapWrapSingleLine.SetTextFit(GetAlternativeCandidates());
      mWrapWrapMultiLine.SetTextFit(GetAlternativeCandidates());
      DALI_LOG_ERROR("Set alternative candidates on all labels\n");
    }
    else if(event.GetKeyName() == "3")
    {
      mFixedSingleLineLabel.SetTextFit(GetFontSizeCandidates());
      mFixedMultiLineLabel.SetTextFit(GetFontSizeCandidates());
      mWrapWrapSingleLine.SetTextFit(GetFontSizeCandidates());
      mWrapWrapMultiLine.SetTextFit(GetFontSizeCandidates());
      DALI_LOG_ERROR("Set only font size candidates on all labels\n");
    }
  }

private:
  Application& mApplication;
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
