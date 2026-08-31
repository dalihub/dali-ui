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

// This is an integration/stress harness, not a visual showcase.  Keep the
// target Label, measurement diagnostics, and deterministic repro controls in
// one process so MaximumLines behavior can be checked across layout paths.

#include <dali-ui-foundation/dali-ui-foundation.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr int      WINDOW_WIDTH          = 1280;
constexpr int      WINDOW_HEIGHT         = 800;
constexpr float    INSPECTOR_MIN_WIDTH   = 380.0f;
constexpr float    INSPECTOR_MAX_WIDTH   = 640.0f;
constexpr float    INSPECTOR_WIDTH_RATIO = 0.34f;
constexpr float    DEFAULT_TARGET_WIDTH  = 360.0f;
constexpr float    DEFAULT_TARGET_HEIGHT = 180.0f;
constexpr float    SIZE_STEP             = 10.0f;
constexpr float    MIN_TARGET_SIZE       = 20.0f;
constexpr uint32_t QUERY_DELAY_MS         = 32u;
constexpr uint32_t TRANSITION_DELAY_MS    = 400u;
constexpr uint32_t ABA_DELAY_MS           = 100u;
constexpr uint32_t RESIZE_STRESS_DELAY_MS = 50u;
constexpr uint32_t FULL_STRESS_DELAY_MS   = 350u;
constexpr uint32_t AUTO_STEP_DELAY_MS     = 64u;
constexpr uint32_t AUTO_TIMEOUT_TICKS     = 125u;
constexpr float    AUTO_RESTORE_EPSILON   = 0.5f;

enum class ScenarioKind
{
  PLAIN,
  STYLED,
  IMAGE_SPAN
};

struct Scenario
{
  const char*           name;
  const char*           purpose;
  const char*           text;
  ScenarioKind          kind;
  Text::LineWrapMode    recommendedWrap;
  LayoutDirection::Type recommendedDirection;
};

constexpr std::array<Scenario, 20u> SCENARIOS{{
  {"BASIC MULTILINE", "Representative long paragraph; default MaximumLines=3.",
   "MaximumLines must constrain rendering and every measurement path. This representative paragraph wraps over several lines at the default width. Resize it, change the limit, and compare Natural, HFW, and both line-count queries for stable results.",
   ScenarioKind::PLAIN, Text::LineWrapMode::WORD, LayoutDirection::LEFT_TO_RIGHT},
  {"EXPLICIT FIVE LINES", "Five mandatory lines expose each MaximumLines boundary.",
   "line one\nline two\nline three\nline four\nline five",
   ScenarioKind::PLAIN, Text::LineWrapMode::WORD, LayoutDirection::LEFT_TO_RIGHT},
  {"HIDDEN LONG FOURTH", "The fourth line is much wider but must not leak into capped measurement.",
   "short one\nshort two\nshort three\nFOURTH LINE IS INTENTIONALLY VERY VERY VERY VERY VERY VERY VERY LONG AND SHOULD BE HIDDEN WHEN MAX IS THREE",
   ScenarioKind::PLAIN, Text::LineWrapMode::WORD, LayoutDirection::LEFT_TO_RIGHT},
  {"WORD WRAP", "Word-boundary wrapping under repeated width changes.",
   "alpha bravo charlie delta echo foxtrot golf hotel india juliet kilo lima mike november oscar papa quebec romeo sierra tango uniform victor whiskey xray yankee zulu",
   ScenarioKind::PLAIN, Text::LineWrapMode::WORD, LayoutDirection::LEFT_TO_RIGHT},
  {"CHARACTER WRAP", "No-space text forces character wrapping and cluster boundaries.",
   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
   ScenarioKind::PLAIN, Text::LineWrapMode::CHARACTER, LayoutDirection::LEFT_TO_RIGHT},
  {"MIXED LTR / RTL", "LTR paragraph with Arabic and Hebrew directional runs.",
   "English prefix 123 before العربية تكشف الكلمات 456 and עברית continues, then English trailing words force several wrapped lines around mixed-direction boundaries.",
   ScenarioKind::PLAIN, Text::LineWrapMode::WORD, LayoutDirection::LEFT_TO_RIGHT},
  {"MIXED RTL / LTR", "RTL actor direction with embedded English runs.",
   "العربية تبدأ السطر ثم English middle section 123 continues for a long distance وبعد ذلك עברית ثم English trailing words exercise the final visible boundary",
   ScenarioKind::PLAIN, Text::LineWrapMode::WORD, LayoutDirection::RIGHT_TO_LEFT},
  {"ARABIC JOINING", "Arabic shaping and joining at capped line boundaries.",
   "العربية تكشف الكلمات تدريجيا بينما يستمر النص الطويل لاختبار عدد الأسطر الأقصى والقياس وتغيير العرض بشكل متكرر دون كسر اتصال الحروف",
   ScenarioKind::PLAIN, Text::LineWrapMode::WORD, LayoutDirection::RIGHT_TO_LEFT},
  {"EMOJI ZWJ", "ZWJ families, modifiers, flags, and variation sequences.",
   "family 👨‍👩‍👧‍👦 astronaut 👩🏽‍🚀 developer 👨🏻‍💻 flags 🇰🇷 🇺🇸 rainbow 🏳️‍🌈 repeat 👨‍👩‍👧‍👦 👩🏽‍🚀 👨🏻‍💻 until the text spans many lines",
   ScenarioKind::PLAIN, Text::LineWrapMode::CHARACTER, LayoutDirection::LEFT_TO_RIGHT},
  {"COMBINING MARKS", "Combining clusters must stay intact near truncation.",
   "cafe\u0301 nai\u0308ve A\u030A e\u0301 o\u0302 n\u0303 cafe\u0301 nai\u0308ve A\u030A e\u0301 o\u0302 n\u0303 repeated combining sequences cross each visible line boundary safely",
   ScenarioKind::PLAIN, Text::LineWrapMode::CHARACTER, LayoutDirection::LEFT_TO_RIGHT},
  {"STYLED BUILDER", "StyledTextBuilder path with spans crossing wrapped lines.",
   "StyledTextBuilder content", ScenarioKind::STYLED, Text::LineWrapMode::WORD, LayoutDirection::LEFT_TO_RIGHT},
  {"TWO IMAGE SPANS", "Two inline ImageSpan objects participate in layout and capping.",
   "ImageSpan content", ScenarioKind::IMAGE_SPAN, Text::LineWrapMode::WORD, LayoutDirection::LEFT_TO_RIGHT},
  {"EXACT A\\nB", "Exactly two mandatory lines; useful with MaximumLines=2.",
   "A\nB", ScenarioKind::PLAIN, Text::LineWrapMode::WORD, LayoutDirection::LEFT_TO_RIGHT},
  {"TRAILING A\\nB\\n", "Trailing newline semantics at the MaximumLines boundary.",
   "A\nB\n", ScenarioKind::PLAIN, Text::LineWrapMode::WORD, LayoutDirection::LEFT_TO_RIGHT},
  {"N+1 A\\nB\\nC", "Three mandatory lines; compare MaximumLines=2 against exact boundary.",
   "A\nB\nC", ScenarioKind::PLAIN, Text::LineWrapMode::WORD, LayoutDirection::LEFT_TO_RIGHT},
  {"HEIGHT CONFLICT", "Use small fixed height while MaximumLines permits more lines.",
   "first mandatory line\nsecond mandatory line\nthird mandatory line\nfourth mandatory line",
   ScenarioKind::PLAIN, Text::LineWrapMode::WORD, LayoutDirection::LEFT_TO_RIGHT},
  {"TEXTFIT IMPOSSIBLE", "Mandatory lines exceed MaximumLines even at the smallest fit candidate.",
   "fit line one\nfit line two\nfit line three\nfit line four",
   ScenarioKind::PLAIN, Text::LineWrapMode::WORD, LayoutDirection::LEFT_TO_RIGHT},
  {"EXACT MAX BOUNDARY", "Exactly three explicit lines with default MaximumLines=3.",
   "exact first line\nexact second line\nexact third line",
   ScenarioKind::PLAIN, Text::LineWrapMode::WORD, LayoutDirection::LEFT_TO_RIGHT},
  {"VERY LONG TEXT", "Large logical model stresses repeated layout invalidation.",
   "generated at runtime", ScenarioKind::PLAIN, Text::LineWrapMode::WORD, LayoutDirection::LEFT_TO_RIGHT},
  {"COMPLEX MIXED", "BiDi, emoji, combining text, newlines, and long words together.",
   "English 123 العربية 456 עברית — cafe\u0301 👨‍👩‍👧‍👦 punctuation [mix].\n한국어 두 번째 문단 has Supercalifragilisticexpialidocious and العربية ثم English trailing text.\nFinal עברית العربية English 👩🏽‍🚀 sequence continues past the visible limit.",
   ScenarioKind::PLAIN, Text::LineWrapMode::MIXED, LayoutDirection::LEFT_TO_RIGHT},
}};

enum class FitMode
{
  OFF,
  RANGE,
  CANDIDATES
};

enum class TargetLayoutMode
{
  FIXED_FIXED,
  FIXED_WRAP,
  WRAP_WRAP,
  MATCH_WRAP,
  WRAP_FIXED
};

enum class AutomationMode
{
  NONE,
  TRANSITION,
  ABA
};

enum class StressMode
{
  NONE,
  RESIZE,
  FULL
};

enum class FeedbackTone
{
  READY,
  CHANGE,
  ACTION,
  INFO
};

struct TestState
{
  int                   maximumLines  = 3;
  float                 width     = DEFAULT_TARGET_WIDTH;
  float                 height    = DEFAULT_TARGET_HEIGHT;
  bool                  multiLine = true;
  bool                  async     = false;
  Text::OverflowMode    overflow  = Text::OverflowMode::ELLIPSIS;
  Text::LineWrapMode    wrap      = Text::LineWrapMode::WORD;
  Text::Alignment       alignment = Text::Alignment::START;
  LayoutDirection::Type direction = LayoutDirection::LEFT_TO_RIGHT;
  FitMode               fit       = FitMode::OFF;
  TargetLayoutMode      layout    = TargetLayoutMode::FIXED_FIXED;
  uint32_t              padding   = 0u;
};

enum class AutoBaselineMode
{
  NONE,
  CAPTURE,
  COMPARE
};

struct AutoCase
{
  std::string       id;
  std::string       name;
  std::size_t       scenarioIndex = 0u;
  TestState         state;
  uint32_t          queryOrder       = 0u;
  int               expectedLineCount = -1;
  bool              requireAsync      = false;
  bool              requireAsyncLineMatch = false;
  AutoBaselineMode  baselineMode      = AutoBaselineMode::NONE;
  std::string       baselineKey;
  std::vector<int>  rapidMaximumLines;
};

struct AutoMeasurement
{
  Vector3 natural{0.0f, 0.0f, 0.0f};
  float   heightForWidth = 0.0f;
  int     lineCount      = -1;
  int     lineWidthCount = -1;
};

const Dali::Vector<Text::Fit::Candidate>& FitCandidates()
{
  static Dali::Vector<Text::Fit::Candidate> candidates = [] {
    Dali::Vector<Text::Fit::Candidate> values;
    values.PushBack(Text::Fit::Candidate(10.0f, 14.0f));
    values.PushBack(Text::Fit::Candidate(14.0f, 19.0f));
    values.PushBack(Text::Fit::Candidate(18.0f, 24.0f));
    values.PushBack(Text::Fit::Candidate(24.0f, 31.0f));
    values.PushBack(Text::Fit::Candidate(30.0f, 39.0f));
    return values;
  }();
  return candidates;
}

const char* OverflowName(Text::OverflowMode mode)
{
  return mode == Text::OverflowMode::ELLIPSIS ? "ELLIPSIS" : "CLIP";
}

const char* WrapName(Text::LineWrapMode mode)
{
  switch(mode)
  {
    case Text::LineWrapMode::WORD:        return "WORD";
    case Text::LineWrapMode::CHARACTER:   return "CHARACTER";
    case Text::LineWrapMode::HYPHENATION: return "HYPHENATION";
    case Text::LineWrapMode::MIXED:       return "MIXED";
  }
  return "UNKNOWN";
}

const char* AlignmentName(Text::Alignment alignment)
{
  switch(alignment)
  {
    case Text::Alignment::START:  return "START";
    case Text::Alignment::CENTER: return "CENTER";
    case Text::Alignment::END:    return "END";
  }
  return "UNKNOWN";
}

const char* DirectionName(LayoutDirection::Type direction)
{
  return direction == LayoutDirection::RIGHT_TO_LEFT ? "RTL" : "LTR";
}

const char* FitName(FitMode mode)
{
  switch(mode)
  {
    case FitMode::OFF:        return "OFF";
    case FitMode::RANGE:      return "RANGE(8..32/2)";
    case FitMode::CANDIDATES: return "CANDIDATES(5)";
  }
  return "UNKNOWN";
}

const char* LayoutName(TargetLayoutMode mode)
{
  switch(mode)
  {
    case TargetLayoutMode::FIXED_FIXED: return "FIXED/FIXED";
    case TargetLayoutMode::FIXED_WRAP:  return "FIXED/WRAP";
    case TargetLayoutMode::WRAP_WRAP:   return "WRAP/WRAP";
    case TargetLayoutMode::MATCH_WRAP:  return "MATCH/WRAP";
    case TargetLayoutMode::WRAP_FIXED:  return "WRAP/FIXED";
  }
  return "UNKNOWN";
}

const char* PaddingName(uint32_t padding)
{
  switch(padding % 3u)
  {
    case 0u: return "ZERO(0/0/0/0)";
    case 1u: return "COMPACT(4/4/2/2)";
    case 2u: return "WIDE(20/20/12/12)";
  }
  return "UNKNOWN";
}

std::string MaximumLinesName(int maximumLines)
{
  return maximumLines == Text::MAXIMUM_LINES_UNLIMITED ? "UNLIMITED(0)" : std::to_string(maximumLines);
}

std::string UpperKey(const KeyEvent& event)
{
  std::string key(event.GetKeyName().CStr());
  std::transform(key.begin(), key.end(), key.begin(), [](unsigned char value) {
    return static_cast<char>(std::toupper(value));
  });
  return key;
}
} // namespace

class TextMaximumLinesController : public ConnectionTracker
{
public:
  TextMaximumLinesController(Application& application, bool autoVerify)
  : mApplication(application),
    mAutoMode(autoVerify)
  {
    mApplication.InitSignal().Connect(this, &TextMaximumLinesController::OnInit);
  }

  int GetExitCode() const
  {
    return mAutoExitCode;
  }

private:
  void OnInit(Application application)
  {
    mWindow = application.GetWindow();
    mWindow.SetPositionSize(PositionSize(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
    mWindow.SetBackgroundColor(UiColor(0xE2E8F0));

    BuildVeryLongText();
    BuildUi();

    mTestLabel.AsyncRenderFinishedSignal().Connect(this, &TextMaximumLinesController::OnAsyncRenderFinished);
    mTestLabel.AsyncNaturalSizeComputedSignal().Connect(this, &TextMaximumLinesController::OnAsyncNaturalSize);
    mTestLabel.AsyncHeightForWidthComputedSignal().Connect(this, &TextMaximumLinesController::OnAsyncHeightForWidth);

    mQueryTimer = Timer::New(QUERY_DELAY_MS);
    mQueryTimer.TickSignal().Connect(this, &TextMaximumLinesController::OnQueryTimer);

    mWindow.KeyEventSignal().Connect(this, &TextMaximumLinesController::OnKeyEvent);
    mWindow.ResizedSignal().Connect(this, &TextMaximumLinesController::OnWindowResized);

    mLastAction = "initial state";
    ApplyState(true, !mAutoMode);

    if(mAutoMode)
    {
      StartAutoVerification(false);
    }
  }

  void BuildVeryLongText()
  {
    mVeryLongText.clear();
    for(uint32_t index = 0u; index < 80u; ++index)
    {
      mVeryLongText += "block" + std::to_string(index) +
                       " MaximumLines measurement invalidation uses repeated words العربية cafe\u0301 👨‍👩‍👧‍👦. ";
      if(index % 7u == 6u)
      {
        mVeryLongText += "\n";
      }
    }
  }

  Label NewChromeLabel(const char* text, float fontSize, uint32_t textColor, uint32_t background) const
  {
    Label label = Label::New(text);
    label.SetFontSize(fontSize);
    label.SetTextColor(UiColor(textColor));
    label.SetBackgroundColor(UiColor(background));
    label.SetMultiLine(true);
    label.SetLineWrapMode(Text::LineWrapMode::WORD);
    label.SetTextOverflowMode(Text::OverflowMode::CLIP);
    return label;
  }

  void BuildUi()
  {
    mRoot = StackLayout::New(StackOrientation::VERTICAL);
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    mRoot.SetSpacing(8.0f);
    mRoot.SetPadding(Insets(14.0f, 14.0f, 12.0f, 12.0f));
    mRoot.SetBackgroundColor(UiColor(0xE2E8F0));

    mHeader = NewChromeLabel("", 16.0f, 0xF8FAFC, 0x0F172A);
    mHeader.SetRequestedWidth(MATCH_PARENT);
    mHeader.SetRequestedHeight(76.0f);
    mHeader.SetPadding(Insets(14.0f, 14.0f, 8.0f, 8.0f));
    mHeader.SetVerticalTextAlignment(Text::Alignment::CENTER);

    StackLayout content = StackLayout::New(StackOrientation::HORIZONTAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(MATCH_PARENT);
    content.SetSpacing(10.0f);
    content.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    StackLayout targetColumn = StackLayout::New(StackOrientation::VERTICAL);
    targetColumn.SetRequestedWidth(0.0f);
    targetColumn.SetRequestedHeight(MATCH_PARENT);
    targetColumn.SetSpacing(8.0f);
    targetColumn.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    mScenarioLabel = NewChromeLabel("", 14.0f, 0x1E293B, 0xF8FAFC);
    mScenarioLabel.SetRequestedWidth(MATCH_PARENT);
    mScenarioLabel.SetRequestedHeight(62.0f);
    mScenarioLabel.SetPadding(Insets(10.0f, 10.0f, 6.0f, 6.0f));
    mScenarioLabel.SetBorderlineWidth(1.0f);
    mScenarioLabel.SetBorderlineOffset(-1.0f);
    mScenarioLabel.SetBorderlineColor(UiColor(0xCBD5E1));

    mStage = StackLayout::New(StackOrientation::VERTICAL);
    mStage.SetRequestedWidth(MATCH_PARENT);
    mStage.SetRequestedHeight(MATCH_PARENT);
    mStage.SetPadding(Insets(18.0f, 18.0f, 18.0f, 18.0f));
    mStage.SetBackgroundColor(UiColor(0xCBD5E1));
    mStage.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    mStage.SetClippingMode(ClippingMode::CLIP_CHILDREN);

    mTestLabel = Label::New();
    mTestLabel.SetFontSize(24.0f);
    mTestLabel.SetTextColor(UiColor(0x111827));
    mTestLabel.SetVerticalTextAlignment(Text::Alignment::START);
    mTestLabel.SetLineHeightMode(Text::LineHeightMode::RELATIVE);
    mTestLabel.SetLineHeight(1.1f);
    mTestLabel.SetBackgroundColor(UiColor(0xFFFFFF));
    mTestLabel.SetBorderlineWidth(2.0f);
    mTestLabel.SetBorderlineOffset(-1.0f);
    mTestLabel.SetBorderlineColor(UiColor(0xDC2626));
    mTestLabel.SetCornerRadius(4.0f);
    mStage.Add(mTestLabel);

    mInspector = StackLayout::New(StackOrientation::VERTICAL);
    mInspector.SetRequestedWidth(INSPECTOR_MIN_WIDTH);
    mInspector.SetRequestedHeight(MATCH_PARENT);
    mInspector.SetSpacing(8.0f);
    mInspector.SetPadding(Insets(10.0f, 10.0f, 10.0f, 10.0f));
    mInspector.SetBackgroundColor(UiColor(0x0F172A));
    mInspector.SetBorderlineWidth(1.0f);
    mInspector.SetBorderlineOffset(-1.0f);
    mInspector.SetBorderlineColor(UiColor(0x334155));
    mInspector.SetCornerRadius(6.0f);

    mChangeBanner = NewChromeLabel("", 14.0f, 0xF8FAFC, 0x0F766E);
    mChangeBanner.SetRequestedWidth(MATCH_PARENT);
    mChangeBanner.SetRequestedHeight(72.0f);
    mChangeBanner.SetPadding(Insets(12.0f, 12.0f, 8.0f, 8.0f));
    mChangeBanner.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mChangeBanner.SetLineHeight(1.08f);
    mChangeBanner.SetLineHeightMode(Text::LineHeightMode::RELATIVE);
    mChangeBanner.SetBorderlineWidth(2.0f);
    mChangeBanner.SetBorderlineOffset(-1.0f);
    mChangeBanner.SetBorderlineColor(UiColor(0x5EEAD4));
    mChangeBanner.SetCornerRadius(5.0f);

    mSettings = NewChromeLabel("", 13.0f, 0xE2E8F0, 0x172033);
    mSettings.SetRequestedWidth(MATCH_PARENT);
    mSettings.SetRequestedHeight(0.0f);
    mSettings.SetPadding(Insets(10.0f, 10.0f, 8.0f, 8.0f));
    mSettings.SetLineHeight(1.07f);
    mSettings.SetLineHeightMode(Text::LineHeightMode::RELATIVE);
    mSettings.SetLayoutParams(StackLayoutParams::New().SetWeight(1.15f).SetAlignment(LayoutAlignment::FILL));

    mMeasurements = NewChromeLabel("", 13.0f, 0xE2E8F0, 0x172033);
    mMeasurements.SetRequestedWidth(MATCH_PARENT);
    mMeasurements.SetRequestedHeight(0.0f);
    mMeasurements.SetPadding(Insets(10.0f, 10.0f, 8.0f, 8.0f));
    mMeasurements.SetLineHeight(1.07f);
    mMeasurements.SetLineHeightMode(Text::LineHeightMode::RELATIVE);
    mMeasurements.SetLayoutParams(StackLayoutParams::New().SetWeight(0.9f).SetAlignment(LayoutAlignment::FILL));

    mRuntime = NewChromeLabel("", 13.0f, 0xE2E8F0, 0x172033);
    mRuntime.SetRequestedWidth(MATCH_PARENT);
    mRuntime.SetRequestedHeight(0.0f);
    mRuntime.SetPadding(Insets(10.0f, 10.0f, 8.0f, 8.0f));
    mRuntime.SetLineHeight(1.07f);
    mRuntime.SetLineHeightMode(Text::LineHeightMode::RELATIVE);
    mRuntime.SetLayoutParams(StackLayoutParams::New().SetWeight(0.65f).SetAlignment(LayoutAlignment::FILL));

    mInspector.Add(mChangeBanner);
    mInspector.Add(mSettings);
    mInspector.Add(mMeasurements);
    mInspector.Add(mRuntime);

    targetColumn.Add(mScenarioLabel);
    targetColumn.Add(mStage);
    content.Add(targetColumn);
    content.Add(mInspector);

    mFooter = NewChromeLabel("", 12.0f, 0x334155, 0xF8FAFC);
    mFooter.SetRequestedWidth(MATCH_PARENT);
    mFooter.SetRequestedHeight(82.0f);
    mFooter.SetPadding(Insets(10.0f, 10.0f, 6.0f, 6.0f));
    mFooter.SetBorderlineWidth(1.0f);
    mFooter.SetBorderlineOffset(-1.0f);
    mFooter.SetBorderlineColor(UiColor(0xCBD5E1));
    mFooter.SetStyledText(Text::StyledText::FromMarkup(
      "<font weight='bold'><color value='#2563EB'>QUICK</color></font>  <color value='#2563EB'><font weight='bold'>[0-5]</font></color> Maximum Lines   <color value='#2563EB'><font weight='bold'>[arrows]</font></color> Width / Height   <color value='#2563EB'><font weight='bold'>[N/B]</font></color> Scenario   <color value='#2563EB'><font weight='bold'>[Z]</font></color> Reset\n"
      "<font weight='bold'><color value='#2563EB'>TEXT</color></font>   <color value='#2563EB'><font weight='bold'>[E]</font></color> Overflow   <color value='#2563EB'><font weight='bold'>[W]</font></color> Wrap   <color value='#2563EB'><font weight='bold'>[H]</font></color> Align   <color value='#2563EB'><font weight='bold'>[D]</font></color> Direction   <color value='#2563EB'><font weight='bold'>[F]</font></color> Fit   <color value='#2563EB'><font weight='bold'>[L]</font></color> Layout   <color value='#2563EB'><font weight='bold'>[G]</font></color> Padding   <color value='#2563EB'><font weight='bold'>[M]</font></color> Multiline   <color value='#2563EB'><font weight='bold'>[A]</font></color> Async\n"
      "<font weight='bold'><color value='#2563EB'>RUN</color></font>    <color value='#2563EB'><font weight='bold'>[V]</font></color> Auto verify   <color value='#2563EB'><font weight='bold'>[Q]</font></color> Query order   <color value='#2563EB'><font weight='bold'>[T]</font></color> Transition   <color value='#2563EB'><font weight='bold'>[R]</font></color> ABA   <color value='#2563EB'><font weight='bold'>[S]</font></color> Resize stress   <color value='#2563EB'><font weight='bold'>[X]</font></color> Full stress   <color value='#2563EB'><font weight='bold'>[Esc]</font></color> Quit"));

    mRoot.Add(mHeader);
    mRoot.Add(content);
    mRoot.Add(mFooter);
    mWindow.Add(mRoot);

    UpdateResponsiveUi(static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT));
  }

  void UpdateResponsiveUi(float windowWidth, float windowHeight)
  {
    mWindowWidth  = std::max(1.0f, windowWidth);
    mWindowHeight = std::max(1.0f, windowHeight);

    const float scale = std::clamp(std::min(mWindowWidth / static_cast<float>(WINDOW_WIDTH),
                                            mWindowHeight / static_cast<float>(WINDOW_HEIGHT)),
                                   0.9f,
                                   1.35f);
    const float inspectorWidth = std::clamp(mWindowWidth * INSPECTOR_WIDTH_RATIO,
                                            INSPECTOR_MIN_WIDTH,
                                            INSPECTOR_MAX_WIDTH);

    mInspector.SetRequestedWidth(inspectorWidth);
    mHeader.SetFontSize(16.0f * scale);
    mHeader.SetRequestedHeight(76.0f * scale);
    mScenarioLabel.SetFontSize(14.0f * scale);
    mScenarioLabel.SetRequestedHeight(62.0f * scale);
    mChangeBanner.SetFontSize(14.0f * scale);
    mChangeBanner.SetRequestedHeight(72.0f * scale);
    mSettings.SetFontSize(13.0f * scale);
    mMeasurements.SetFontSize(13.0f * scale);
    mRuntime.SetFontSize(13.0f * scale);
    mFooter.SetFontSize(12.0f * scale);
    mFooter.SetRequestedHeight(82.0f * scale);
  }

  void ApplyScenarioText()
  {
    const Scenario& scenario = SCENARIOS[mScenarioIndex];
    switch(scenario.kind)
    {
      case ScenarioKind::STYLED:
      {
        Text::StyledTextBuilder builder = Text::StyledTextBuilder::FromMarkup(
          "<u>Styled prefix crosses a boundary</u> then normal text continues across several wrapped lines. "
          "A second <u>underlined range approaches the final visible line</u> and trailing words verify MaximumLines measurement parity.");
        mTestLabel.SetStyledText(builder.Build());
        break;
      }
      case ScenarioKind::IMAGE_SPAN:
      {
        Text::StyledTextBuilder builder = Text::StyledTextBuilder::New();
        builder.AppendText("before first image ");
        uint32_t first = builder.GetUtf32Length();
        builder.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);
        builder.AppendText(" words between image spans and enough wrapping text ");
        uint32_t second = builder.GetUtf32Length();
        builder.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);
        builder.AppendText(" after second image with trailing text that must cross the configured MaximumLines boundary.");

        Text::ImageAttributes firstAttributes(RESOURCES_DIR "flag_kr.png", Vector2(48.0f, 30.0f));
        firstAttributes.SetAlignment(Text::ImageAttributes::InlineAlignment::TEXT_CENTER);
        Text::ImageAttributes secondAttributes(RESOURCES_DIR "flag_us.png", Vector2(48.0f, 30.0f));
        secondAttributes.SetAlignment(Text::ImageAttributes::InlineAlignment::TEXT_CENTER);
        builder.SetSpan(Text::ImageSpan::New(firstAttributes), first, first + 1u);
        builder.SetSpan(Text::ImageSpan::New(secondAttributes), second, second + 1u);
        mTestLabel.SetStyledText(builder.Build());
        break;
      }
      case ScenarioKind::PLAIN:
      {
        mTestLabel.SetText(mScenarioIndex == 18u ? mVeryLongText.c_str() : scenario.text);
        break;
      }
    }
    mAppliedScenarioIndex = mScenarioIndex;
  }

  void ApplyLayoutMode()
  {
    float requestedWidth  = mState.width;
    float requestedHeight = mState.height;
    LayoutAlignment crossAlignment = LayoutAlignment::CENTER;

    switch(mState.layout)
    {
      case TargetLayoutMode::FIXED_FIXED:
        break;
      case TargetLayoutMode::FIXED_WRAP:
        requestedHeight = WRAP_CONTENT;
        break;
      case TargetLayoutMode::WRAP_WRAP:
        requestedWidth  = WRAP_CONTENT;
        requestedHeight = WRAP_CONTENT;
        break;
      case TargetLayoutMode::MATCH_WRAP:
        requestedWidth  = MATCH_PARENT;
        requestedHeight = WRAP_CONTENT;
        crossAlignment  = LayoutAlignment::FILL;
        break;
      case TargetLayoutMode::WRAP_FIXED:
        requestedWidth = WRAP_CONTENT;
        break;
    }

    mTestLabel.SetRequestedWidth(requestedWidth);
    mTestLabel.SetRequestedHeight(requestedHeight);
    mTestLabel.SetLayoutParams(StackLayoutParams::New().SetAlignment(crossAlignment));
  }

  void ApplyPadding()
  {
    switch(mState.padding % 3u)
    {
      case 0u: mTestLabel.SetPadding(Insets(0.0f, 0.0f, 0.0f, 0.0f)); break;
      case 1u: mTestLabel.SetPadding(Insets(4.0f, 4.0f, 2.0f, 2.0f)); break;
      case 2u: mTestLabel.SetPadding(Insets(20.0f, 20.0f, 12.0f, 12.0f)); break;
    }
  }

  void ApplyTextFit()
  {
    switch(mState.fit)
    {
      case FitMode::OFF:        mTestLabel.SetTextFit(Text::Fit::None()); break;
      case FitMode::RANGE:      mTestLabel.SetTextFit(Text::Fit::Range(8.0f, 32.0f, 2.0f)); break;
      case FitMode::CANDIDATES: mTestLabel.SetTextFit(FitCandidates()); break;
    }
  }

  void ApplyState(bool forceText = false, bool verbose = true)
  {
    if(forceText || mAppliedScenarioIndex != mScenarioIndex)
    {
      ApplyScenarioText();
    }

    mTestLabel.SetMultiLine(mState.multiLine);
    mTestLabel.SetMaximumLines(mState.maximumLines);
    mTestLabel.SetTextOverflowMode(mState.overflow);
    mTestLabel.SetLineWrapMode(mState.wrap);
    mTestLabel.SetHorizontalTextAlignment(mState.alignment);
    mTestLabel.SetLayoutDirection(mState.direction);
    mTestLabel.SetLayoutDirectionMode(Text::LayoutDirectionMode::INHERIT);
    mTestLabel.SetAsyncRendering(mState.async);
    ApplyPadding();
    ApplyTextFit();
    ApplyLayoutMode();

    ++mUpdateCount;
    BeginQueries();
    if(!mAutoRunning)
    {
      UpdateChrome();
    }

    if(mState.async)
    {
      ++mAsyncNaturalRequests;
      ++mAsyncHfwRequests;
      mTestLabel.RequestAsyncNaturalSize();
      mTestLabel.RequestAsyncHeightForWidth(mState.width);
    }

    if(verbose)
    {
      PrintReproState();
    }
  }

  void BeginQueries()
  {
    if(mAutoRunning)
    {
      mAutoQueryReady = false;
    }
    mNaturalValid   = false;
    mHfwValid       = false;
    mLineBefore     = -1;
    mLineWidthBefore = -1;
    mLineAfter      = -1;
    mLineWidthAfter = -1;
    mQueryPhase     = "before Render(next frame)";

    switch(mQueryOrder)
    {
      case 0u:
        QueryNatural(); QueryHfw(); QueryLinesBefore();
        mQueryOrderName = "Natural > HFW > Lines > Render";
        break;
      case 1u:
        QueryLinesBefore(); QueryNatural(); QueryHfw();
        mQueryOrderName = "Lines > Natural > HFW > Render";
        break;
      case 2u:
        QueryNatural();
        mQueryOrderName = "Natural > Render > HFW > Lines";
        break;
      case 3u:
        QueryHfw();
        mQueryOrderName = "HFW > Render > Natural > Lines";
        break;
      case 4u:
        mLineBefore = mTestLabel.GetLineCount();
        mQueryOrderName = "Line(current) > Render > Line(current/width)";
        break;
      case 5u:
        QueryHfw();
        mLineWidthBefore = mTestLabel.GetLineCount(mState.width);
        mQueryOrderName = "HFW > Line(width) > Render > remaining";
        break;
      case 6u:
        mQueryOrderName = "Render > Natural > HFW > Lines";
        break;
    }

    if(!mAutoRunning)
    {
      UpdateDiagnostics();
    }
    mQueryTimer.Stop();
    mQueryTimer.Start();
  }

  void QueryNatural()
  {
    mNatural      = mTestLabel.GetNaturalSize();
    mNaturalValid = true;
  }

  void QueryHfw()
  {
    mHeightForWidth = mTestLabel.GetHeightForWidth(mState.width);
    mHfwValid       = true;
  }

  void QueryLinesBefore()
  {
    mLineBefore      = mTestLabel.GetLineCount();
    mLineWidthBefore = mTestLabel.GetLineCount(mState.width);
  }

  bool OnQueryTimer()
  {
    mQueryPhase = "after Render(next frame)";
    mActualSize = mTestLabel.GetSize();

    switch(mQueryOrder)
    {
      case 0u:
      case 1u:
        mLineAfter      = mTestLabel.GetLineCount();
        mLineWidthAfter = mTestLabel.GetLineCount(mState.width);
        break;
      case 2u:
        QueryHfw();
        mLineAfter      = mTestLabel.GetLineCount();
        mLineWidthAfter = mTestLabel.GetLineCount(mState.width);
        break;
      case 3u:
        QueryNatural();
        mLineAfter      = mTestLabel.GetLineCount();
        mLineWidthAfter = mTestLabel.GetLineCount(mState.width);
        break;
      case 4u:
        mLineAfter      = mTestLabel.GetLineCount();
        mLineWidthAfter = mTestLabel.GetLineCount(mState.width);
        QueryNatural(); QueryHfw();
        break;
      case 5u:
        QueryNatural();
        mLineAfter      = mTestLabel.GetLineCount();
        mLineWidthAfter = mTestLabel.GetLineCount(mState.width);
        break;
      case 6u:
        QueryNatural(); QueryHfw();
        mLineAfter      = mTestLabel.GetLineCount();
        mLineWidthAfter = mTestLabel.GetLineCount(mState.width);
        break;
    }

    mAutoQueryReady = mAutoRunning;
    if(!mAutoRunning)
    {
      UpdateDiagnostics();
    }
    return false;
  }

  void UpdateChrome()
  {
    const Scenario& scenario = SCENARIOS[mScenarioIndex];

    std::ostringstream header;
    header << "<font weight='bold'><color value='#F8FAFC'>LABEL MAXIMUM LINES</color></font>"
           << "   <color value='#38BDF8'>CASE " << (mScenarioIndex + 1u) << " / " << SCENARIOS.size() << "</color>\n"
           << "<color value='#94A3B8'>Public Label API · red border = arranged bounds · measurements refresh after state, signal, and frame changes</color>";
    mHeader.SetStyledText(Text::StyledText::FromMarkup(header.str().c_str()));

    std::ostringstream scenarioText;
    scenarioText << "<font weight='bold'><color value='#0F172A'>[" << std::setw(2) << std::setfill('0') << (mScenarioIndex + 1u) << "] " << scenario.name << "</color></font>\n"
                 << "<color value='#475569'>" << scenario.purpose << "</color>";
    mScenarioLabel.SetStyledText(Text::StyledText::FromMarkup(scenarioText.str().c_str()));
    UpdateDiagnostics();
  }

  std::string RequestedSizeName() const
  {
    std::ostringstream stream;
    stream << LayoutName(mState.layout) << " base=" << std::fixed << std::setprecision(0) << mState.width << 'x' << mState.height;
    return stream.str();
  }

  std::string VectorSizeName(const Vector3& size) const
  {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(1) << size.width << 'x' << size.height;
    return stream.str();
  }

  std::string OptionalLine(int value) const
  {
    return value < 0 ? "-" : std::to_string(value);
  }

  std::string ScalarName(float value) const
  {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(0) << value;
    return stream.str();
  }

  std::string ScenarioName(std::size_t index) const
  {
    std::ostringstream stream;
    stream << '[' << std::setw(2) << std::setfill('0') << (index + 1u) << "] " << SCENARIOS[index].name;
    return stream.str();
  }

  void RecordFeedback(const std::string& key,
                      const std::string& title,
                      const std::string& detail,
                      const std::string& changedField,
                      FeedbackTone       tone = FeedbackTone::CHANGE)
  {
    ++mFeedbackCount;
    mFeedbackKey    = key;
    mFeedbackTitle  = title;
    mFeedbackDetail = detail;
    mChangedField   = changedField;
    mFeedbackTone   = tone;
    mLastAction     = key + " " + title + ": " + detail;
  }

  void RecordChange(const std::string& key,
                    const std::string& title,
                    const std::string& before,
                    const std::string& after,
                    const std::string& changedField)
  {
    RecordFeedback(key, title, before + "  ->  " + after, changedField, FeedbackTone::CHANGE);
  }

  void AppendInspectorRow(std::ostringstream& stream,
                          const char*         key,
                          const char*         field,
                          const std::string&  title,
                          const std::string&  value) const
  {
    const bool changed = !mChangedField.empty() && mChangedField == field;
    stream << (changed ? "<color value='#FBBF24'><font weight='bold'>● " : "<color value='#94A3B8'><font weight='bold'>");
    if(key && key[0] != '\0')
    {
      stream << '[' << key << "] ";
    }
    stream << title << "</font></color>  <color value='#F8FAFC'>" << value << "</color>\n";
  }

  std::string AutomationName() const
  {
    if(mAutomation == AutomationMode::TRANSITION) return "TRANSITION T";
    if(mAutomation == AutomationMode::ABA) return "ASYNC ABA R";
    if(mStress == StressMode::RESIZE) return "RESIZE S";
    if(mStress == StressMode::FULL) return "FULL X";
    return "IDLE";
  }

  void UpdateAutoGuiProgress()
  {
    if(!mAutoInteractive || !mSettings || mAutoCases.empty() || mAutoCaseIndex >= mAutoCases.size())
    {
      return;
    }

    const AutoCase& testCase      = mAutoCases[mAutoCaseIndex];
    const std::size_t completed   = mAutoCaseIndex;
    const std::size_t total       = mAutoCases.size();
    const std::size_t percent     = total == 0u ? 0u : (completed * 100u) / total;
    constexpr std::size_t BAR_SIZE = 24u;
    const std::size_t filled      = (percent * BAR_SIZE) / 100u;
    const std::string progressBar = std::string(filled, '#') + std::string(BAR_SIZE - filled, '.');

    std::ostringstream header;
    header << "<font weight='bold'><color value='#F8FAFC'>LABEL MAXIMUM LINES · AUTO VERIFY</color></font>"
           << "   <color value='#38BDF8'>CASE " << (mAutoCaseIndex + 1u) << " / " << total << "</color>\n"
           << "<color value='#94A3B8'>Live Scene / Layout / Renderer verification · press V to cancel</color>";
    mHeader.SetStyledText(Text::StyledText::FromMarkup(header.str().c_str()));

    std::ostringstream scenario;
    scenario << "<font weight='bold'><color value='#0F172A'>[" << std::setw(2) << std::setfill('0')
             << (testCase.scenarioIndex + 1u) << "] " << SCENARIOS[testCase.scenarioIndex].name << "</color></font>\n"
             << "<color value='#475569'>" << testCase.id << " · " << testCase.name << " · "
             << SCENARIOS[testCase.scenarioIndex].purpose << "</color>";
    mScenarioLabel.SetStyledText(Text::StyledText::FromMarkup(scenario.str().c_str()));

    mChangeBanner.SetBackgroundColor(UiColor(0x6D28D9));
    mChangeBanner.SetBorderlineColor(UiColor(0xC4B5FD));
    std::ostringstream banner;
    banner << "<font weight='bold'><color value='#DDD6FE'>AUTO VERIFY RUNNING</color></font>\n"
           << "<color value='#FFFFFF'>Case " << (mAutoCaseIndex + 1u) << " / " << total
           << " · " << testCase.id << " · " << testCase.name << "</color>";
    mChangeBanner.SetStyledText(Text::StyledText::FromMarkup(banner.str().c_str()));

    std::ostringstream settings;
    settings << "<font weight='bold'><color value='#38BDF8'>AUTOMATIC COVERAGE</color></font>\n"
             << "<color value='#F8FAFC'><font weight='bold'>PROGRESS</font>  " << percent << "%</color>\n"
             << "<color value='#A5B4FC'>[" << progressBar << "]</color>\n"
             << "<color value='#94A3B8'><font weight='bold'>SCENARIO</font></color>  <color value='#F8FAFC'>"
             << (testCase.scenarioIndex + 1u) << " / " << SCENARIOS.size() << " · "
             << SCENARIOS[testCase.scenarioIndex].name << "</color>\n"
             << "<color value='#94A3B8'><font weight='bold'>STATE</font></color>  <color value='#F8FAFC'>Max "
             << MaximumLinesName(mState.maximumLines) << " · " << OverflowName(mState.overflow)
             << " · " << WrapName(mState.wrap) << "</color>";
    mSettings.SetStyledText(Text::StyledText::FromMarkup(settings.str().c_str()));

    std::ostringstream measurements;
    measurements << "<font weight='bold'><color value='#38BDF8'>LIVE RESULT</color></font>\n"
                 << "<color value='#94A3B8'><font weight='bold'>COMPLETED</font></color>  <color value='#F8FAFC'>" << completed << " / " << total << "</color>\n"
                 << "<color value='#94A3B8'><font weight='bold'>CHECKS</font></color>  <color value='#F8FAFC'>" << mAutoCheckCount << "</color>\n"
                 << "<color value='#86EFAC'><font weight='bold'>PASS</font></color>  <color value='#F8FAFC'>" << mAutoPassCount << "</color>\n"
                 << "<color value='" << (mAutoFailCount == 0u ? "#94A3B8" : "#FCA5A5")
                 << "'><font weight='bold'>FAIL</font></color>  <color value='#F8FAFC'>" << mAutoFailCount << "</color>";
    mMeasurements.SetStyledText(Text::StyledText::FromMarkup(measurements.str().c_str()));

    std::ostringstream runtime;
    runtime << "<font weight='bold'><color value='#38BDF8'>RUN CONTROL</color></font>\n"
            << "<color value='#FBBF24'><font weight='bold'>[V] CANCEL</font></color>\n"
            << "<color value='#94A3B8'>Controls are locked while verification is running.</color>\n"
            << "<color value='#64748B'>CLI equivalent: --auto-verify</color>";
    mRuntime.SetStyledText(Text::StyledText::FromMarkup(runtime.str().c_str()));
  }

  void UpdateAutoGuiResult()
  {
    if(!mAutoResultVisible || !mSettings)
    {
      return;
    }

    const bool passed = mAutoFailCount == 0u;
    mChangeBanner.SetBackgroundColor(UiColor(passed ? 0x166534 : 0x991B1B));
    mChangeBanner.SetBorderlineColor(UiColor(passed ? 0x86EFAC : 0xFCA5A5));
    std::ostringstream banner;
    banner << "<font weight='bold'><color value='" << (passed ? "#BBF7D0" : "#FECACA") << "'>AUTO VERIFY "
           << (passed ? "PASS" : "FAIL") << "</color></font>\n"
           << "<color value='#FFFFFF'>" << mAutoCases.size() << " cases · " << mAutoCheckCount
           << " checks · " << mAutoFailCount << " failures</color>";
    mChangeBanner.SetStyledText(Text::StyledText::FromMarkup(banner.str().c_str()));

    std::ostringstream settings;
    settings << "<font weight='bold'><color value='#38BDF8'>AUTOMATIC SUMMARY</color></font>\n"
             << "<color value='#94A3B8'><font weight='bold'>CASES</font></color>  <color value='#F8FAFC'>" << mAutoCases.size() << "</color>\n"
             << "<color value='#94A3B8'><font weight='bold'>CHECKS</font></color>  <color value='#F8FAFC'>" << mAutoCheckCount << "</color>\n"
             << "<color value='#86EFAC'><font weight='bold'>PASS</font></color>  <color value='#F8FAFC'>" << mAutoPassCount << "</color>\n"
             << "<color value='" << (passed ? "#94A3B8" : "#FCA5A5")
             << "'><font weight='bold'>FAIL</font></color>  <color value='#F8FAFC'>" << mAutoFailCount << "</color>";
    mSettings.SetStyledText(Text::StyledText::FromMarkup(settings.str().c_str()));

    std::ostringstream measurements;
    measurements << "<font weight='bold'><color value='#38BDF8'>FINAL STATUS</color></font>\n"
                 << "<color value='" << (passed ? "#86EFAC" : "#FCA5A5") << "'><font weight='bold'>"
                 << (passed ? "ALL AUTOMATIC CHECKS PASSED" : "ONE OR MORE CHECKS FAILED") << "</font></color>\n"
                 << "<color value='#94A3B8'>The previous manual scenario and settings were restored.</color>\n"
                 << "<color value='#64748B'>Detailed expected / observed records remain in stdout.</color>";
    mMeasurements.SetStyledText(Text::StyledText::FromMarkup(measurements.str().c_str()));

    std::ostringstream runtime;
    runtime << "<font weight='bold'><color value='#38BDF8'>NEXT ACTION</color></font>\n"
            << "<color value='#FBBF24'><font weight='bold'>[V] RUN AGAIN</font></color>\n"
            << "<color value='#F8FAFC'>Press any normal test control to return to live diagnostics.</color>\n"
            << "<color value='#64748B'>Command line mode still exits with status 0 / 1.</color>";
    mRuntime.SetStyledText(Text::StyledText::FromMarkup(runtime.str().c_str()));
  }

  void UpdateDiagnostics()
  {
    if(mAutoRunning)
    {
      return;
    }
    if(!mSettings)
    {
      return;
    }
    if(mAutoResultVisible)
    {
      UpdateAutoGuiResult();
      return;
    }

    const Text::Fit::Type actualFit = mTestLabel.GetTextFit().GetType();
    const char* actualFitName = actualFit == Text::Fit::Type::RANGE ? "RANGE" :
                                actualFit == Text::Fit::Type::CANDIDATES ? "CANDIDATES" : "OFF";

    uint32_t bannerBackground = 0x0F766E;
    uint32_t bannerBorder     = 0x5EEAD4;
    const char* bannerAccent  = "#99F6E4";
    if(mFeedbackTone == FeedbackTone::ACTION)
    {
      bannerBackground = 0x6D28D9;
      bannerBorder     = 0xC4B5FD;
      bannerAccent     = "#DDD6FE";
    }
    else if(mFeedbackTone == FeedbackTone::INFO || mFeedbackTone == FeedbackTone::READY)
    {
      bannerBackground = 0x075985;
      bannerBorder     = 0x7DD3FC;
      bannerAccent     = "#BAE6FD";
    }
    mChangeBanner.SetBackgroundColor(UiColor(bannerBackground));
    mChangeBanner.SetBorderlineColor(UiColor(bannerBorder));

    std::ostringstream banner;
    banner << "<font weight='bold'><color value='" << bannerAccent << "'>"
           << (mFeedbackTone == FeedbackTone::READY ? "READY" : "LATEST CHANGE #" + std::to_string(mFeedbackCount))
           << "  [" << mFeedbackKey << "]  " << mFeedbackTitle << "</color></font>\n"
           << "<color value='#FFFFFF'>" << mFeedbackDetail << "</color>";
    mChangeBanner.SetStyledText(Text::StyledText::FromMarkup(banner.str().c_str()));

    std::ostringstream settings;
    settings << "<font weight='bold'><color value='#38BDF8'>CURRENT SETTINGS</color></font>  <color value='#64748B'>requested / actual</color>\n";
    AppendInspectorRow(settings, "0-5", "max", "MAXIMUM LINES", MaximumLinesName(mState.maximumLines) + " / " + MaximumLinesName(mTestLabel.GetMaximumLines()));
    AppendInspectorRow(settings, "M", "multiline", "MULTILINE", std::string(mState.multiLine ? "true" : "false") + " / " + (mTestLabel.IsMultiLine() ? "true" : "false"));
    AppendInspectorRow(settings, "arrows", "size", "BASE SIZE", RequestedSizeName());
    AppendInspectorRow(settings, "E", "overflow", "OVERFLOW", std::string(OverflowName(mState.overflow)) + " / " + OverflowName(mTestLabel.GetTextOverflowMode()));
    AppendInspectorRow(settings, "W", "wrap", "WRAP", std::string(WrapName(mState.wrap)) + " / " + WrapName(mTestLabel.GetLineWrapMode()));
    AppendInspectorRow(settings, "H", "align", "ALIGN", std::string(AlignmentName(mState.alignment)) + " / " + AlignmentName(mTestLabel.GetHorizontalTextAlignment()));
    AppendInspectorRow(settings, "D", "direction", "DIRECTION", std::string(DirectionName(mState.direction)) + " · INHERIT");
    AppendInspectorRow(settings, "F", "fit", "TEXT FIT", std::string(FitName(mState.fit)) + " / " + actualFitName);
    AppendInspectorRow(settings, "A", "async", "ASYNC", mState.async ? "ON" : "OFF");
    AppendInspectorRow(settings, "G", "padding", "PADDING", PaddingName(mState.padding));
    mSettings.SetStyledText(Text::StyledText::FromMarkup(settings.str().c_str()));

    std::ostringstream measurements;
    measurements << "<font weight='bold'><color value='#38BDF8'>MEASUREMENTS</color></font>  <color value='#64748B'>before  ->  after frame</color>\n";
    AppendInspectorRow(measurements, "", "actual", "ARRANGED", VectorSizeName(mActualSize));
    AppendInspectorRow(measurements, "", "natural", "NATURAL", mNaturalValid ? VectorSizeName(mNatural) : "pending");
    AppendInspectorRow(measurements, "", "hfw", "HEIGHT FOR WIDTH", "w=" + ScalarName(mState.width) + " · h=" + (mHfwValid ? ScalarName(mHeightForWidth) : "pending"));
    AppendInspectorRow(measurements, "", "lines", "LINE COUNT", OptionalLine(mLineBefore) + "  ->  " + OptionalLine(mLineAfter));
    AppendInspectorRow(measurements, "", "lines", "LINE COUNT(width)", OptionalLine(mLineWidthBefore) + "  ->  " + OptionalLine(mLineWidthAfter));
    AppendInspectorRow(measurements, "", "phase", "PHASE", mQueryPhase);
    mMeasurements.SetStyledText(Text::StyledText::FromMarkup(measurements.str().c_str()));

    std::ostringstream asyncSummary;
    asyncSummary << std::fixed << std::setprecision(1)
                 << (mState.async ? "ON" : "OFF") << " · render " << mAsyncRenderCompletions << " (" << mAsyncRenderWidth << 'x' << mAsyncRenderHeight << ')';
    std::ostringstream asyncResults;
    asyncResults << std::fixed << std::setprecision(1)
                 << "natural " << mAsyncNaturalCompletions << '/' << mAsyncNaturalRequests
                 << " · hfw " << mAsyncHfwCompletions << '/' << mAsyncHfwRequests
                 << " · lines " << mTestLabel.GetAsyncLineCount();
    std::ostringstream runtime;
    runtime << "<font weight='bold'><color value='#38BDF8'>RUN + QUERY</color></font>\n";
    AppendInspectorRow(runtime, "Q", "query", "ORDER " + std::to_string(mQueryOrder), mQueryOrderName);
    AppendInspectorRow(runtime, "", "async", "ASYNC", asyncSummary.str());
    AppendInspectorRow(runtime, "", "async", "ASYNC RESULT", asyncResults.str());
    AppendInspectorRow(runtime, "", "automation", "UPDATE", std::to_string(mUpdateCount) + " · " + AutomationName());
    mRuntime.SetStyledText(Text::StyledText::FromMarkup(runtime.str().c_str()));
  }

  void PrintAutoLine(const std::string& line) const
  {
    std::printf("%s\n", line.c_str());
  }

  void StartAutoVerification(bool interactive)
  {
    if(mAutoRunning)
    {
      return;
    }

    StopAutomation();
    mAutoInteractive = interactive;
    mAutoResultVisible = false;
    if(interactive)
    {
      mAutoSavedState         = mState;
      mAutoSavedScenarioIndex = mScenarioIndex;
      mAutoSavedQueryOrder    = mQueryOrder;
      mAutoSavedManualState   = true;
    }

    BuildAutoCases();
    mAutoBaselines.clear();
    mAutoCaseIndex          = 0u;
    mAutoCaseTicks          = 0u;
    mAutoCheckCount         = 0u;
    mAutoPassCount          = 0u;
    mAutoFailCount          = 0u;
    mAutoStableObservations = 0u;
    mAutoPreviousValid      = false;
    mAutoQueryReady         = false;
    mAutoExitCode           = 0;
    mAutoRunning            = true;

    std::ostringstream begin;
    begin << "[MAXIMUM_LINES][AUTO][BEGIN] cases=" << mAutoCases.size();
    PrintAutoLine(begin.str());

    if(mAutoTimer)
    {
      mAutoTimer.Stop();
    }
    mAutoTimer = Timer::New(AUTO_STEP_DELAY_MS);
    mAutoTimer.TickSignal().Connect(this, &TextMaximumLinesController::OnAutoTimer);
    StartNextAutoCase();
    mAutoTimer.Start();
  }

  TestState MakeAutoState(std::size_t scenarioIndex) const
  {
    TestState state;
    state.width     = 320.0f;
    state.height    = 220.0f;
    state.wrap      = SCENARIOS[scenarioIndex].recommendedWrap;
    state.direction = SCENARIOS[scenarioIndex].recommendedDirection;
    return state;
  }

  void AddAutoCase(AutoCase testCase)
  {
    std::ostringstream id;
    id << 'C' << std::setw(4) << std::setfill('0') << (mAutoCases.size() + 1u);
    testCase.id = id.str();
    mAutoCases.push_back(testCase);
  }

  void AddRestoreSequence(const std::string& name,
                          const std::string& baselineKey,
                          std::size_t        scenarioIndex,
                          TestState          state,
                          const std::string& cappedBaselineKey = std::string())
  {
    AutoCase capture;
    capture.name          = name + "-capture";
    capture.scenarioIndex = scenarioIndex;
    capture.state         = state;
    capture.state.maximumLines = Text::MAXIMUM_LINES_UNLIMITED;
    capture.baselineMode  = AutoBaselineMode::CAPTURE;
    capture.baselineKey   = baselineKey;
    AddAutoCase(capture);

    AutoCase capped = capture;
    capped.name          = name + "-capped";
    capped.state.maximumLines = 2;
    capped.baselineMode  = cappedBaselineKey.empty() ? AutoBaselineMode::NONE : AutoBaselineMode::CAPTURE;
    capped.baselineKey   = cappedBaselineKey;
    AddAutoCase(capped);

    AutoCase restore = capture;
    restore.name         = name + "-restore";
    restore.baselineMode = AutoBaselineMode::COMPARE;
    AddAutoCase(restore);
  }

  void BuildAutoCases()
  {
    mAutoCases.clear();

    constexpr std::array<std::size_t, 12u> CORE_SCENARIOS{{0u, 1u, 3u, 4u, 15u, 16u, 5u, 6u, 8u, 9u, 10u, 11u}};
    constexpr std::array<int, 5u>           MAXIMUM_LINES_VALUES{{0, 1, 2, 3, 5}};
    constexpr std::array<Text::OverflowMode, 2u> OVERFLOW_VALUES{{Text::OverflowMode::CLIP, Text::OverflowMode::ELLIPSIS}};

    for(std::size_t scenarioIndex : CORE_SCENARIOS)
    {
      for(int maximumLines : MAXIMUM_LINES_VALUES)
      {
        for(Text::OverflowMode overflow : OVERFLOW_VALUES)
        {
          AutoCase testCase;
          testCase.name          = "core-matrix";
          testCase.scenarioIndex = scenarioIndex;
          testCase.state         = MakeAutoState(scenarioIndex);
          testCase.state.maximumLines = maximumLines;
          testCase.state.overflow = overflow;
          AddAutoCase(testCase);
        }
      }
    }

    for(std::size_t scenarioIndex = 0u; scenarioIndex < SCENARIOS.size(); ++scenarioIndex)
    {
      AutoCase smoke;
      smoke.name          = "all-scenarios-smoke";
      smoke.scenarioIndex = scenarioIndex;
      smoke.state         = MakeAutoState(scenarioIndex);
      smoke.state.maximumLines = 3;
      AddAutoCase(smoke);
    }

    for(Text::LineWrapMode wrap : std::array<Text::LineWrapMode, 4u>{{Text::LineWrapMode::WORD,
                                                                      Text::LineWrapMode::CHARACTER,
                                                                      Text::LineWrapMode::HYPHENATION,
                                                                      Text::LineWrapMode::MIXED}})
    {
      AutoCase option;
      option.name           = "wrap-mode-smoke";
      option.scenarioIndex  = 0u;
      option.state          = MakeAutoState(0u);
      option.state.maximumLines = 3;
      option.state.wrap     = wrap;
      AddAutoCase(option);
    }

    for(Text::Alignment alignment : std::array<Text::Alignment, 3u>{{Text::Alignment::START,
                                                                      Text::Alignment::CENTER,
                                                                      Text::Alignment::END}})
    {
      AutoCase option;
      option.name            = "alignment-smoke";
      option.scenarioIndex   = 0u;
      option.state           = MakeAutoState(0u);
      option.state.maximumLines  = 3;
      option.state.alignment = alignment;
      AddAutoCase(option);
    }

    for(LayoutDirection::Type direction : std::array<LayoutDirection::Type, 2u>{{LayoutDirection::LEFT_TO_RIGHT,
                                                                                 LayoutDirection::RIGHT_TO_LEFT}})
    {
      AutoCase option;
      option.name            = "direction-smoke";
      option.scenarioIndex   = 5u;
      option.state           = MakeAutoState(5u);
      option.state.maximumLines  = 3;
      option.state.direction = direction;
      AddAutoCase(option);
    }

    for(TargetLayoutMode layout : std::array<TargetLayoutMode, 5u>{{TargetLayoutMode::FIXED_FIXED,
                                                                     TargetLayoutMode::FIXED_WRAP,
                                                                     TargetLayoutMode::WRAP_WRAP,
                                                                     TargetLayoutMode::MATCH_WRAP,
                                                                     TargetLayoutMode::WRAP_FIXED}})
    {
      AutoCase option;
      option.name           = "layout-mode-smoke";
      option.scenarioIndex  = 0u;
      option.state          = MakeAutoState(0u);
      option.state.maximumLines = 3;
      option.state.layout   = layout;
      AddAutoCase(option);
    }

    for(uint32_t padding : std::array<uint32_t, 3u>{{0u, 1u, 2u}})
    {
      AutoCase option;
      option.name           = "padding-smoke";
      option.scenarioIndex  = 0u;
      option.state          = MakeAutoState(0u);
      option.state.maximumLines = 3;
      option.state.padding  = padding;
      AddAutoCase(option);
    }

    for(int maximumLines : MAXIMUM_LINES_VALUES)
    {
      AutoCase canonical;
      canonical.name          = "explicit-five-lines";
      canonical.scenarioIndex = 1u;
      canonical.state         = MakeAutoState(1u);
      canonical.state.width   = 900.0f;
      canonical.state.height  = 400.0f;
      canonical.state.maximumLines = maximumLines;
      canonical.expectedLineCount = maximumLines == Text::MAXIMUM_LINES_UNLIMITED ? 5 : maximumLines;
      AddAutoCase(canonical);
    }

    for(int maximumLines : std::array<int, 2u>{{1, 3}})
    {
      AutoCase singleLine;
      singleLine.name          = "single-line-semantics";
      singleLine.scenarioIndex = 1u;
      singleLine.state         = MakeAutoState(1u);
      singleLine.state.width   = 900.0f;
      singleLine.state.height  = 160.0f;
      singleLine.state.multiLine = false;
      singleLine.state.maximumLines  = maximumLines;
      singleLine.expectedLineCount = 1;
      AddAutoCase(singleLine);
    }

    for(uint32_t queryOrder = 0u; queryOrder < 7u; ++queryOrder)
    {
      for(int maximumLines : std::array<int, 2u>{{0, 3}})
      {
        AutoCase query;
        query.name          = "query-order";
        query.scenarioIndex = 0u;
        query.state         = MakeAutoState(0u);
        query.state.width   = 260.0f;
        query.state.maximumLines = maximumLines;
        query.queryOrder    = queryOrder;
        AddAutoCase(query);
      }
    }

    for(std::size_t scenarioIndex : std::array<std::size_t, 5u>{{0u, 1u, 5u, 10u, 11u}})
    {
      std::ostringstream key;
      key << "restore-s" << (scenarioIndex + 1u);
      TestState restoreState = MakeAutoState(scenarioIndex);
      // Keep the arranged height above the unlimited HFW so this restoration
      // check is not conflated with GetLineCount's height-limited shared model.
      restoreState.height = 420.0f;
      AddRestoreSequence("unlimited-restore", key.str(), scenarioIndex, restoreState);
    }

    for(FitMode fit : std::array<FitMode, 2u>{{FitMode::RANGE, FitMode::CANDIDATES}})
    {
      TestState state = MakeAutoState(0u);
      state.width  = 240.0f;
      state.height = 160.0f;
      state.fit    = fit;
      const std::string fitName      = fit == FitMode::RANGE ? "fit-range" : "fit-candidates";
      const std::string fitCappedKey = fitName + "-capped";
      AddRestoreSequence("text-fit-restore", fitName, 0u, state, fitCappedKey);

      TestState impossible = MakeAutoState(16u);
      impossible.width    = 180.0f;
      impossible.height   = 42.0f;
      impossible.maximumLines = 2;
      impossible.fit      = fit;

      AutoCase stableCapture;
      stableCapture.name          = "text-fit-impossible-capture";
      stableCapture.scenarioIndex = 16u;
      stableCapture.state         = impossible;
      stableCapture.baselineMode  = AutoBaselineMode::CAPTURE;
      stableCapture.baselineKey   = fit == FitMode::RANGE ? "impossible-range" : "impossible-candidates";
      AddAutoCase(stableCapture);

      AutoCase stableCompare = stableCapture;
      stableCompare.name         = "text-fit-impossible-repeat";
      stableCompare.baselineMode = AutoBaselineMode::COMPARE;
      AddAutoCase(stableCompare);

      AutoCase asyncFit;
      asyncFit.name           = "text-fit-async";
      asyncFit.scenarioIndex  = 0u;
      asyncFit.state          = state;
      asyncFit.state.maximumLines = 2;
      asyncFit.state.async    = true;
      asyncFit.requireAsync   = true;
      asyncFit.baselineMode   = AutoBaselineMode::COMPARE;
      asyncFit.baselineKey    = fitCappedKey;
      AddAutoCase(asyncFit);
    }

    for(Text::OverflowMode overflow : OVERFLOW_VALUES)
    {
      AutoCase conflict;
      conflict.name          = "height-conflict";
      conflict.scenarioIndex = 15u;
      conflict.state         = MakeAutoState(15u);
      conflict.state.width   = 320.0f;
      conflict.state.height  = 54.0f;
      conflict.state.maximumLines = 3;
      conflict.state.overflow = overflow;
      AddAutoCase(conflict);
    }

    for(float width : std::array<float, 2u>{{180.0f, 480.0f}})
    {
      for(int maximumLines : std::array<int, 4u>{{0, 1, 2, 3}})
      {
        AutoCase image;
        image.name          = "image-span-width";
        image.scenarioIndex = 11u;
        image.state         = MakeAutoState(11u);
        image.state.width   = width;
        image.state.maximumLines = maximumLines;
        if(width == 180.0f && maximumLines == 2)
        {
          image.baselineMode = AutoBaselineMode::CAPTURE;
          image.baselineKey  = "image-span-sync-capped";
        }
        AddAutoCase(image);
      }
    }

    AutoCase syncBefore;
    syncBefore.name          = "sync-to-async-sync";
    syncBefore.scenarioIndex = 0u;
    syncBefore.state         = MakeAutoState(0u);
    syncBefore.state.maximumLines = 3;
    AddAutoCase(syncBefore);

    AutoCase asyncState = syncBefore;
    asyncState.state.async  = true;
    asyncState.requireAsync = true;
    AddAutoCase(asyncState);

    AddAutoCase(syncBefore);

    AutoCase asyncImage;
    asyncImage.name          = "image-span-async";
    asyncImage.scenarioIndex = 11u;
    asyncImage.state         = MakeAutoState(11u);
    asyncImage.state.width   = 180.0f;
    asyncImage.state.maximumLines = 2;
    asyncImage.state.async    = true;
    asyncImage.requireAsync   = true;
    asyncImage.baselineMode   = AutoBaselineMode::COMPARE;
    asyncImage.baselineKey    = "image-span-sync-capped";
    AddAutoCase(asyncImage);

    AutoCase abaReference;
    abaReference.name          = "async-rapid-aba-sync-reference";
    abaReference.scenarioIndex = 0u;
    abaReference.state         = MakeAutoState(0u);
    abaReference.state.maximumLines = 5;
    abaReference.baselineMode  = AutoBaselineMode::CAPTURE;
    abaReference.baselineKey   = "async-aba-final";
    AddAutoCase(abaReference);

    AutoCase aba;
    aba.name          = "async-rapid-aba";
    aba.scenarioIndex = 0u;
    aba.state         = MakeAutoState(0u);
    aba.state.async   = true;
    aba.state.maximumLines = 5;
    aba.requireAsync   = true;
    aba.requireAsyncLineMatch = true;
    aba.baselineMode   = AutoBaselineMode::COMPARE;
    aba.baselineKey    = "async-aba-final";
    aba.rapidMaximumLines  = {5, 2, 5};
    AddAutoCase(aba);

    AutoCase rapid = aba;
    rapid.name          = "async-rapid-transition";
    rapid.state.height  = 420.0f;
    rapid.state.maximumLines = Text::MAXIMUM_LINES_UNLIMITED;
    rapid.baselineMode  = AutoBaselineMode::COMPARE;
    rapid.baselineKey   = "restore-s1";
    rapid.rapidMaximumLines  = {5, 2, 5, 1, Text::MAXIMUM_LINES_UNLIMITED};
    AddAutoCase(rapid);
  }

  bool StartNextAutoCase()
  {
    if(mAutoCaseIndex >= mAutoCases.size())
    {
      FinishAutoVerification();
      return false;
    }

    const AutoCase& testCase = mAutoCases[mAutoCaseIndex];
    mAutoCaseTicks            = 0u;
    mAutoQueryReady           = false;
    mAutoPreviousValid        = false;
    mAutoStableObservations   = 0u;
    mAutoCasePassStart        = mAutoPassCount;
    mAutoCaseFailStart        = mAutoFailCount;
    mAutoNaturalStart         = mAsyncNaturalCompletions;
    mAutoHfwStart             = mAsyncHfwCompletions;
    mAutoRenderStart          = mAsyncRenderCompletions;

    mScenarioIndex = testCase.scenarioIndex;
    mState         = testCase.state;
    mQueryOrder    = testCase.queryOrder;

    std::ostringstream begin;
    begin << "[MAXIMUM_LINES][CASE][BEGIN] id=" << testCase.id
          << " index=" << (mAutoCaseIndex + 1u)
          << " name=" << testCase.name
          << " scenario=" << (testCase.scenarioIndex + 1u);
    PrintAutoLine(begin.str());

    if(testCase.rapidMaximumLines.empty())
    {
      ApplyState(false, false);
    }
    else
    {
      for(int maximumLines : testCase.rapidMaximumLines)
      {
        mState.maximumLines = maximumLines;
        ApplyState(false, false);
      }
    }
    UpdateAutoGuiProgress();
    return true;
  }

  bool AutoAsyncReady(const AutoCase& testCase) const
  {
    return !testCase.requireAsync ||
           (mAsyncRenderCompletions > mAutoRenderStart &&
            mAsyncNaturalCompletions > mAutoNaturalStart &&
            mAsyncHfwCompletions > mAutoHfwStart);
  }

  AutoMeasurement CurrentAutoMeasurement() const
  {
    AutoMeasurement current;
    current.natural        = mNatural;
    current.heightForWidth = mHeightForWidth;
    current.lineCount      = mLineAfter;
    current.lineWidthCount = mLineWidthAfter;
    return current;
  }

  bool AutoMeasurementMatches(const AutoMeasurement& first, const AutoMeasurement& second) const
  {
    return Near(first.natural.width, second.natural.width) &&
           Near(first.natural.height, second.natural.height) &&
           Near(first.heightForWidth, second.heightForWidth) &&
           first.lineCount == second.lineCount &&
           first.lineWidthCount == second.lineWidthCount;
  }

  bool AutoCaseNeedsSettling(const AutoCase& testCase) const
  {
    return testCase.baselineMode != AutoBaselineMode::NONE || testCase.state.fit != FitMode::OFF;
  }

  bool AutoCaseSettled(const AutoCase& testCase)
  {
    if(!AutoCaseNeedsSettling(testCase))
    {
      return true;
    }

    const AutoMeasurement current = CurrentAutoMeasurement();
    if(mAutoPreviousValid && AutoMeasurementMatches(current, mAutoPreviousMeasurement))
    {
      ++mAutoStableObservations;
    }
    else
    {
      mAutoStableObservations = 0u;
    }
    mAutoPreviousMeasurement = current;
    mAutoPreviousValid       = true;
    return mAutoStableObservations >= 1u;
  }

  void AutoCheck(bool               passed,
                 const char*        category,
                 const char*        name,
                 const std::string& expected,
                 const std::string& observed)
  {
    ++mAutoCheckCount;
    if(passed)
    {
      ++mAutoPassCount;
    }
    else
    {
      ++mAutoFailCount;
    }

    const AutoCase& testCase = mAutoCases[mAutoCaseIndex];
    std::ostringstream line;
    line << "[MAXIMUM_LINES][CHECK][" << (passed ? "PASS" : "FAIL") << "]"
         << " case=" << testCase.id
         << " category=" << category
         << " name=" << name
         << " expected=" << expected
         << " observed=" << observed;
    PrintAutoLine(line.str());
    if(!passed)
    {
      std::fflush(stdout);
      std::fflush(stderr);
    }
  }

  bool FiniteNonNegative(float value) const
  {
    return std::isfinite(value) && value >= 0.0f;
  }

  bool Near(float first, float second) const
  {
    return std::fabs(first - second) <= AUTO_RESTORE_EPSILON;
  }

  void LogAutoObservation(const AutoCase& testCase) const
  {
    const Text::Fit::Type actualFit = mTestLabel.GetTextFit().GetType();
    const char* actualFitName = actualFit == Text::Fit::Type::RANGE ? "RANGE" :
                                actualFit == Text::Fit::Type::CANDIDATES ? "CANDIDATES" : "OFF";

    std::ostringstream state;
    state << "[MAXIMUM_LINES][STATE] case=" << testCase.id
          << " scenario=" << (testCase.scenarioIndex + 1u)
          << " scenarioName=\"" << SCENARIOS[testCase.scenarioIndex].name << '\"'
          << " maxRequested=" << mState.maximumLines
          << " maxActual=" << mTestLabel.GetMaximumLines()
          << " multiRequested=" << (mState.multiLine ? 1 : 0)
          << " multiActual=" << (mTestLabel.IsMultiLine() ? 1 : 0)
          << " width=" << ScalarName(mState.width)
          << " height=" << ScalarName(mState.height)
          << " layout=" << LayoutName(mState.layout)
          << " overflowRequested=" << OverflowName(mState.overflow)
          << " overflowActual=" << OverflowName(mTestLabel.GetTextOverflowMode())
          << " wrapRequested=" << WrapName(mState.wrap)
          << " wrapActual=" << WrapName(mTestLabel.GetLineWrapMode())
          << " alignRequested=" << AlignmentName(mState.alignment)
          << " alignActual=" << AlignmentName(mTestLabel.GetHorizontalTextAlignment())
          << " direction=" << DirectionName(mState.direction)
          << " fitRequested=" << FitName(mState.fit)
          << " fitActual=" << actualFitName
          << " async=" << (mState.async ? 1 : 0);
    PrintAutoLine(state.str());

    std::ostringstream measure;
    measure << std::fixed << std::setprecision(1)
            << "[MAXIMUM_LINES][MEASURE] case=" << testCase.id
            << " natural=" << mNatural.width << 'x' << mNatural.height
            << " hfwWidth=" << mState.width
            << " hfwHeight=" << mHeightForWidth
            << " line=" << mLineAfter
            << " lineWidth=" << mLineWidthAfter
            << " actual=" << mActualSize.width << 'x' << mActualSize.height;
    PrintAutoLine(measure.str());

    std::ostringstream query;
    query << "[MAXIMUM_LINES][QUERY] case=" << testCase.id
          << " order=" << mQueryOrder
          << " beforeLine=" << mLineBefore
          << " beforeLineWidth=" << mLineWidthBefore
          << " afterLine=" << mLineAfter
          << " afterLineWidth=" << mLineWidthAfter
          << " phase=after-frame";
    PrintAutoLine(query.str());

    std::ostringstream async;
    async << std::fixed << std::setprecision(1)
          << "[MAXIMUM_LINES][ASYNC] case=" << testCase.id
          << " enabled=" << (mState.async ? 1 : 0)
          << " renderCompletions=" << mAsyncRenderCompletions
          << " renderSize=" << mAsyncRenderWidth << 'x' << mAsyncRenderHeight
          << " naturalRequests=" << mAsyncNaturalRequests
          << " naturalCompletions=" << mAsyncNaturalCompletions
          << " hfwRequests=" << mAsyncHfwRequests
          << " hfwCompletions=" << mAsyncHfwCompletions
          << " asyncLine=" << mTestLabel.GetAsyncLineCount();
    PrintAutoLine(async.str());
  }

  void CheckAutoBaseline(const AutoCase& testCase)
  {
    const AutoMeasurement current = CurrentAutoMeasurement();

    if(testCase.baselineMode == AutoBaselineMode::CAPTURE)
    {
      mAutoBaselines[testCase.baselineKey] = current;
      AutoCheck(true, "RESTORE", "baseline-captured", "stored", testCase.baselineKey);
      return;
    }
    if(testCase.baselineMode != AutoBaselineMode::COMPARE)
    {
      return;
    }

    const auto found = mAutoBaselines.find(testCase.baselineKey);
    AutoCheck(found != mAutoBaselines.end(), "RESTORE", "baseline-present", "present", found == mAutoBaselines.end() ? "missing" : "present");
    if(found == mAutoBaselines.end())
    {
      return;
    }

    const AutoMeasurement& baseline = found->second;
    AutoCheck(Near(current.natural.width, baseline.natural.width) && Near(current.natural.height, baseline.natural.height),
              "RESTORE", "natural-restored", VectorSizeName(baseline.natural), VectorSizeName(current.natural));
    AutoCheck(Near(current.heightForWidth, baseline.heightForWidth), "RESTORE", "hfw-restored", ScalarName(baseline.heightForWidth), ScalarName(current.heightForWidth));
    AutoCheck(current.lineCount == baseline.lineCount, "RESTORE", "line-restored", std::to_string(baseline.lineCount), std::to_string(current.lineCount));
    AutoCheck(current.lineWidthCount == baseline.lineWidthCount, "RESTORE", "line-width-restored", std::to_string(baseline.lineWidthCount), std::to_string(current.lineWidthCount));
  }

  void EvaluateAutoCase(bool timedOut)
  {
    const AutoCase& testCase = mAutoCases[mAutoCaseIndex];
    LogAutoObservation(testCase);

    if(timedOut)
    {
      AutoCheck(false, "TIMEOUT", "case-ready", "query-and-async-ready", mAutoQueryReady ? "async-pending" : "query-pending");
    }

    const bool sizesValid = mNaturalValid && mHfwValid &&
                            FiniteNonNegative(mNatural.width) && FiniteNonNegative(mNatural.height) &&
                            FiniteNonNegative(mHeightForWidth) &&
                            FiniteNonNegative(mActualSize.width) && FiniteNonNegative(mActualSize.height);
    AutoCheck(sizesValid, "MEASUREMENT", "finite-nonnegative-sizes", "all-finite-and-nonnegative", sizesValid ? "valid" : "invalid");
    AutoCheck(mLineAfter >= 0 && mLineWidthAfter >= 0, "LINE_COUNT", "nonnegative-line-counts", ">=0", std::to_string(mLineAfter) + "/" + std::to_string(mLineWidthAfter));
    AutoCheck(mTestLabel.GetMaximumLines() == mState.maximumLines, "STATE", "maximum-lines-roundtrip", std::to_string(mState.maximumLines), std::to_string(mTestLabel.GetMaximumLines()));
    AutoCheck(mTestLabel.IsMultiLine() == mState.multiLine, "STATE", "multiline-roundtrip", mState.multiLine ? "1" : "0", mTestLabel.IsMultiLine() ? "1" : "0");
    AutoCheck(mTestLabel.GetTextOverflowMode() == mState.overflow, "STATE", "overflow-roundtrip", OverflowName(mState.overflow), OverflowName(mTestLabel.GetTextOverflowMode()));
    AutoCheck(mTestLabel.GetLineWrapMode() == mState.wrap, "STATE", "wrap-roundtrip", WrapName(mState.wrap), WrapName(mTestLabel.GetLineWrapMode()));
    AutoCheck(mTestLabel.GetHorizontalTextAlignment() == mState.alignment, "STATE", "alignment-roundtrip", AlignmentName(mState.alignment), AlignmentName(mTestLabel.GetHorizontalTextAlignment()));

    const Text::Fit::Type expectedFit = mState.fit == FitMode::RANGE ? Text::Fit::Type::RANGE :
                                         mState.fit == FitMode::CANDIDATES ? Text::Fit::Type::CANDIDATES : Text::Fit::Type::NONE;
    AutoCheck(mTestLabel.GetTextFit().GetType() == expectedFit, "TEXT_FIT", "fit-roundtrip", FitName(mState.fit), FitName(mState.fit));

    if(mState.multiLine && mState.maximumLines > Text::MAXIMUM_LINES_UNLIMITED)
    {
      AutoCheck(mLineAfter <= mState.maximumLines, "LINE_COUNT", "current-line-cap", "<=" + std::to_string(mState.maximumLines), std::to_string(mLineAfter));
      AutoCheck(mLineWidthAfter <= mState.maximumLines, "LINE_COUNT", "width-line-cap", "<=" + std::to_string(mState.maximumLines), std::to_string(mLineWidthAfter));
    }

    if(testCase.expectedLineCount >= 0)
    {
      AutoCheck(mLineAfter == testCase.expectedLineCount, "LINE_COUNT", "canonical-current-lines", std::to_string(testCase.expectedLineCount), std::to_string(mLineAfter));
      AutoCheck(mLineWidthAfter == testCase.expectedLineCount, "LINE_COUNT", "canonical-width-lines", std::to_string(testCase.expectedLineCount), std::to_string(mLineWidthAfter));
    }

    if(testCase.requireAsync && !timedOut)
    {
      AutoCheck(mAsyncRenderCompletions > mAutoRenderStart, "ASYNC", "render-completion", ">" + std::to_string(mAutoRenderStart), std::to_string(mAsyncRenderCompletions));
      AutoCheck(mAsyncNaturalCompletions > mAutoNaturalStart, "ASYNC", "natural-completion", ">" + std::to_string(mAutoNaturalStart), std::to_string(mAsyncNaturalCompletions));
      AutoCheck(mAsyncHfwCompletions > mAutoHfwStart, "ASYNC", "hfw-completion", ">" + std::to_string(mAutoHfwStart), std::to_string(mAsyncHfwCompletions));
      AutoCheck(mTestLabel.GetAsyncLineCount() >= 0, "ASYNC", "nonnegative-async-lines", ">=0", std::to_string(mTestLabel.GetAsyncLineCount()));
      if(testCase.requireAsyncLineMatch)
      {
        AutoCheck(mTestLabel.GetAsyncLineCount() == mLineAfter, "ASYNC", "final-line-match", std::to_string(mLineAfter), std::to_string(mTestLabel.GetAsyncLineCount()));
      }
      if(mState.multiLine && mState.maximumLines > Text::MAXIMUM_LINES_UNLIMITED)
      {
        AutoCheck(mTestLabel.GetAsyncLineCount() <= mState.maximumLines, "ASYNC", "async-line-cap", "<=" + std::to_string(mState.maximumLines), std::to_string(mTestLabel.GetAsyncLineCount()));
      }
    }

    CheckAutoBaseline(testCase);

    std::ostringstream end;
    end << "[MAXIMUM_LINES][CASE][END] id=" << testCase.id
        << " pass=" << (mAutoPassCount - mAutoCasePassStart)
        << " fail=" << (mAutoFailCount - mAutoCaseFailStart);
    PrintAutoLine(end.str());
  }

  bool OnAutoTimer()
  {
    if(!mAutoRunning || mAutoCaseIndex >= mAutoCases.size())
    {
      return false;
    }

    ++mAutoCaseTicks;
    const AutoCase& testCase = mAutoCases[mAutoCaseIndex];
    if(mAutoQueryReady && AutoAsyncReady(testCase))
    {
      if(!AutoCaseSettled(testCase))
      {
        BeginQueries();
        return true;
      }
      EvaluateAutoCase(false);
      ++mAutoCaseIndex;
      return StartNextAutoCase();
    }

    if(mAutoCaseTicks >= AUTO_TIMEOUT_TICKS)
    {
      EvaluateAutoCase(true);
      ++mAutoCaseIndex;
      return StartNextAutoCase();
    }
    return true;
  }

  void RestoreInteractiveAutoState()
  {
    if(!mAutoSavedManualState)
    {
      return;
    }
    mState                = mAutoSavedState;
    mScenarioIndex        = mAutoSavedScenarioIndex;
    mAppliedScenarioIndex = SCENARIOS.size();
    mQueryOrder           = mAutoSavedQueryOrder;
    mAutoSavedManualState = false;
  }

  void CancelInteractiveAutoVerification()
  {
    if(!mAutoRunning || !mAutoInteractive)
    {
      return;
    }

    mAutoRunning = false;
    mAutoTimer.Stop();
    PrintAutoLine("[MAXIMUM_LINES][AUTO][CANCEL]");
    std::fflush(stdout);
    RestoreInteractiveAutoState();
    mAutoInteractive = false;
    mAutoExitCode     = 0;
    RecordFeedback("V", "AUTO VERIFY CANCELLED", "Manual scenario and settings restored", "automation", FeedbackTone::ACTION);
    ApplyState(true, false);
  }

  void FinishAutoVerification()
  {
    const bool interactive = mAutoInteractive;
    mAutoRunning           = false;
    mAutoExitCode          = mAutoFailCount == 0u ? 0 : 1;

    std::ostringstream summary;
    summary << "[MAXIMUM_LINES][SUMMARY] cases=" << mAutoCases.size()
            << " checks=" << mAutoCheckCount
            << " pass=" << mAutoPassCount
            << " fail=" << mAutoFailCount;
    PrintAutoLine(summary.str());
    PrintAutoLine(mAutoFailCount == 0u ? "[MAXIMUM_LINES][AUTO][PASS]" : "[MAXIMUM_LINES][AUTO][FAIL]");
    std::fflush(stdout);
    std::fflush(stderr);
    if(!interactive)
    {
      mApplication.Quit();
      return;
    }

    RestoreInteractiveAutoState();
    mAutoInteractive   = false;
    mAutoResultVisible = true;
    RecordFeedback("V",
                   mAutoFailCount == 0u ? "AUTO VERIFY PASS" : "AUTO VERIFY FAIL",
                   std::to_string(mAutoCases.size()) + " cases · " + std::to_string(mAutoCheckCount) + " checks · " + std::to_string(mAutoFailCount) + " failures",
                   "automation",
                   mAutoFailCount == 0u ? FeedbackTone::CHANGE : FeedbackTone::ACTION);
    ApplyState(true, false);
    UpdateAutoGuiResult();
  }

  void PrintReproState() const
  {
    std::ostringstream line;
    line << std::fixed << std::setprecision(0)
         << "[MAXIMUM_LINES][MANUAL][STATE] update=" << mUpdateCount
         << " scenario=" << (mScenarioIndex + 1u)
         << " max=" << mState.maximumLines
         << " multi=" << (mState.multiLine ? 1 : 0)
         << " size=" << mState.width << 'x' << mState.height
         << " layout=" << LayoutName(mState.layout)
         << " overflow=" << OverflowName(mState.overflow)
         << " wrap=" << WrapName(mState.wrap)
         << " align=" << AlignmentName(mState.alignment)
         << " direction=" << DirectionName(mState.direction)
         << " fit=" << FitName(mState.fit)
         << " async=" << (mState.async ? 1 : 0)
         << " query=" << mQueryOrder
         << " action=\"" << mLastAction << '\"';
    std::printf("%s\n", line.str().c_str());
    std::fflush(stdout);
  }

  void OnAsyncRenderFinished(View, float width, float height)
  {
    ++mAsyncRenderCompletions;
    mAsyncRenderWidth  = width;
    mAsyncRenderHeight = height;
    UpdateDiagnostics();
  }

  void OnAsyncNaturalSize(View, float width, float height)
  {
    ++mAsyncNaturalCompletions;
    mAsyncNaturalWidth  = width;
    mAsyncNaturalHeight = height;
    UpdateDiagnostics();
  }

  void OnAsyncHeightForWidth(View, float width, float height)
  {
    ++mAsyncHfwCompletions;
    mAsyncHfwWidth  = width;
    mAsyncHfwHeight = height;
    UpdateDiagnostics();
  }

  void OnWindowResized(Window, Window::WindowSize windowSize)
  {
    const std::string before = ScalarName(mWindowWidth) + "x" + ScalarName(mWindowHeight);
    UpdateResponsiveUi(static_cast<float>(windowSize.GetWidth()), static_cast<float>(windowSize.GetHeight()));
    if(mAutoRunning)
    {
      BeginQueries();
      return;
    }
    RecordChange("WINDOW", "WINDOW SIZE", before, ScalarName(mWindowWidth) + "x" + ScalarName(mWindowHeight), "window");
    BeginQueries();
  }

  void SelectScenario(int delta, bool applyRecommended)
  {
    const int count = static_cast<int>(SCENARIOS.size());
    mScenarioIndex = static_cast<std::size_t>((static_cast<int>(mScenarioIndex) + delta + count) % count);
    if(applyRecommended)
    {
      const Scenario& scenario = SCENARIOS[mScenarioIndex];
      mState.wrap      = scenario.recommendedWrap;
      mState.direction = scenario.recommendedDirection;

      if(mScenarioIndex == 12u || mScenarioIndex == 13u || mScenarioIndex == 14u)
      {
        mState.maximumLines = 2;
      }
      else if(mScenarioIndex == 15u)
      {
        mState.maximumLines = 3;
        mState.width    = 260.0f;
        mState.height   = 54.0f;
        mState.layout   = TargetLayoutMode::FIXED_FIXED;
      }
      else if(mScenarioIndex == 16u)
      {
        mState.maximumLines = 2;
        mState.width    = 180.0f;
        mState.height   = 42.0f;
        mState.fit      = FitMode::RANGE;
        mState.layout   = TargetLayoutMode::FIXED_FIXED;
      }
      else if(mScenarioIndex == 17u)
      {
        mState.maximumLines = 3;
      }
    }
  }

  void StopAutomation()
  {
    if(mAutomationTimer)
    {
      mAutomationTimer.Stop();
    }
    if(mStressTimer)
    {
      mStressTimer.Stop();
    }
    mAutomation = AutomationMode::NONE;
    mStress     = StressMode::NONE;
  }

  void StartTransition()
  {
    StopAutomation();
    mAutomation      = AutomationMode::TRANSITION;
    mAutomationIndex = 0u;
    mAutomationTimer = Timer::New(TRANSITION_DELAY_MS);
    mAutomationTimer.TickSignal().Connect(this, &TextMaximumLinesController::OnAutomationTick);
    RecordFeedback("T", "TRANSITION STARTED", "UNLIMITED -> 5 -> 3 -> 1 -> 3 -> 5 -> UNLIMITED", "automation", FeedbackTone::ACTION);
    OnAutomationTick();
    mAutomationTimer.Start();
  }

  void StartAba()
  {
    StopAutomation();
    mAutomation      = AutomationMode::ABA;
    mAutomationIndex = 0u;
    mState.async     = true;
    mAutomationTimer = Timer::New(ABA_DELAY_MS);
    mAutomationTimer.TickSignal().Connect(this, &TextMaximumLinesController::OnAutomationTick);
    RecordFeedback("R", "ASYNC ABA STARTED", "Maximum Lines 5 -> 2 -> 5, repeated 18 times", "automation", FeedbackTone::ACTION);
    OnAutomationTick();
    mAutomationTimer.Start();
  }

  bool OnAutomationTick()
  {
    static constexpr std::array<int, 7u> transition{{0, 5, 3, 1, 3, 5, 0}};
    static constexpr std::array<int, 3u> aba{{5, 2, 5}};

    if(mAutomation == AutomationMode::TRANSITION)
    {
      if(mAutomationIndex >= transition.size())
      {
        mAutomation = AutomationMode::NONE;
        RecordFeedback("T", "TRANSITION COMPLETE", "Final Maximum Lines = UNLIMITED", "automation", FeedbackTone::ACTION);
        UpdateDiagnostics();
        return false;
      }
      const std::string before = MaximumLinesName(mState.maximumLines);
      mState.maximumLines = transition[mAutomationIndex++];
      RecordFeedback("T", "MAXIMUM LINES · TRANSITION", before + "  ->  " + MaximumLinesName(mState.maximumLines), "max", FeedbackTone::ACTION);
      ApplyState();
      return true;
    }

    if(mAutomation == AutomationMode::ABA)
    {
      constexpr uint32_t ABA_STEPS = 18u;
      if(mAutomationIndex >= ABA_STEPS)
      {
        mAutomation = AutomationMode::NONE;
        RecordFeedback("R", "ASYNC ABA COMPLETE", "18 state transitions finished", "automation", FeedbackTone::ACTION);
        UpdateDiagnostics();
        return false;
      }
      const std::string before = MaximumLinesName(mState.maximumLines);
      mState.maximumLines = aba[mAutomationIndex % aba.size()];
      ++mAutomationIndex;
      RecordFeedback("R", "MAXIMUM LINES · ASYNC ABA", before + "  ->  " + MaximumLinesName(mState.maximumLines), "max", FeedbackTone::ACTION);
      ApplyState();
      return true;
    }
    return false;
  }

  void ToggleResizeStress()
  {
    if(mStress == StressMode::RESIZE)
    {
      StopAutomation();
      RecordFeedback("S", "RESIZE STRESS STOPPED", "Target width remains " + ScalarName(mState.width), "automation", FeedbackTone::ACTION);
      UpdateDiagnostics();
      return;
    }

    StopAutomation();
    mStress          = StressMode::RESIZE;
    mStressIteration = 0u;
    mResizeGrowing   = true;
    mState.width     = 80.0f;
    mState.layout    = TargetLayoutMode::FIXED_FIXED;
    mStressTimer     = Timer::New(RESIZE_STRESS_DELAY_MS);
    mStressTimer.TickSignal().Connect(this, &TextMaximumLinesController::OnStressTick);
    RecordFeedback("S", "RESIZE STRESS STARTED", "Width 80 to 500 · Maximum Lines alternates 2 / 3", "automation", FeedbackTone::ACTION);
    ApplyState();
    mStressTimer.Start();
  }

  void ToggleFullStress()
  {
    if(mStress == StressMode::FULL)
    {
      StopAutomation();
      RecordFeedback("X", "FULL STRESS STOPPED", "Current state retained", "automation", FeedbackTone::ACTION);
      UpdateDiagnostics();
      return;
    }

    StopAutomation();
    mStress          = StressMode::FULL;
    mStressIteration = 0u;
    mStressTimer     = Timer::New(FULL_STRESS_DELAY_MS);
    mStressTimer.TickSignal().Connect(this, &TextMaximumLinesController::OnStressTick);
    RecordFeedback("X", "FULL STRESS STARTED", "12 deterministic state axes cycle continuously", "automation", FeedbackTone::ACTION);
    UpdateDiagnostics();
    mStressTimer.Start();
  }

  bool OnStressTick()
  {
    if(mStress == StressMode::RESIZE)
    {
      const std::string beforeWidth = ScalarName(mState.width);
      const std::string beforeMax   = MaximumLinesName(mState.maximumLines);
      mState.width += mResizeGrowing ? 10.0f : -10.0f;
      if(mState.width >= 500.0f)
      {
        mState.width   = 500.0f;
        mResizeGrowing = false;
      }
      else if(mState.width <= 80.0f)
      {
        mState.width   = 80.0f;
        mResizeGrowing = true;
      }
      if(mStressIteration % 12u == 0u)
      {
        mState.maximumLines = mState.maximumLines == 2 ? 3 : 2;
      }
      ++mStressIteration;
      std::string detail = beforeWidth + "  ->  " + ScalarName(mState.width);
      if(beforeMax != MaximumLinesName(mState.maximumLines))
      {
        detail += " · Maximum Lines " + beforeMax + " -> " + MaximumLinesName(mState.maximumLines);
      }
      RecordFeedback("S", "WIDTH · RESIZE TICK " + std::to_string(mStressIteration), detail, "size", FeedbackTone::ACTION);
      ApplyState(false, mStressIteration % 20u == 0u);
      return true;
    }

    if(mStress == StressMode::FULL)
    {
      const uint32_t axis = mStressIteration % 12u;
      switch(axis)
      {
        case 0u:
        {
          const std::string before = ScalarName(mState.width);
          mState.width = 80.0f + static_cast<float>((mStressIteration * 37u) % 421u);
          RecordFeedback("X", "WIDTH · FULL STRESS", before + "  ->  " + ScalarName(mState.width), "size", FeedbackTone::ACTION);
          break;
        }
        case 1u:
        {
          const std::string before = ScalarName(mState.height);
          mState.height = 30.0f + static_cast<float>((mStressIteration * 29u) % 231u);
          RecordFeedback("X", "HEIGHT · FULL STRESS", before + "  ->  " + ScalarName(mState.height), "size", FeedbackTone::ACTION);
          break;
        }
        case 2u:
        {
          const std::string before = MaximumLinesName(mState.maximumLines);
          mState.maximumLines = static_cast<int>((mStressIteration / 2u) % 6u);
          RecordFeedback("X", "MAXIMUM LINES · FULL STRESS", before + "  ->  " + MaximumLinesName(mState.maximumLines), "max", FeedbackTone::ACTION);
          break;
        }
        case 3u:
        {
          const std::string before = OverflowName(mState.overflow);
          mState.overflow = mState.overflow == Text::OverflowMode::ELLIPSIS ? Text::OverflowMode::CLIP : Text::OverflowMode::ELLIPSIS;
          RecordFeedback("X", "OVERFLOW · FULL STRESS", before + "  ->  " + OverflowName(mState.overflow), "overflow", FeedbackTone::ACTION);
          break;
        }
        case 4u:
        {
          const std::string before = WrapName(mState.wrap);
          mState.wrap = static_cast<Text::LineWrapMode>((static_cast<uint32_t>(mState.wrap) + 1u) % 4u);
          RecordFeedback("X", "WRAP · FULL STRESS", before + "  ->  " + WrapName(mState.wrap), "wrap", FeedbackTone::ACTION);
          break;
        }
        case 5u:
        {
          const std::string before = AlignmentName(mState.alignment);
          mState.alignment = static_cast<Text::Alignment>((static_cast<uint32_t>(mState.alignment) + 1u) % 3u);
          RecordFeedback("X", "ALIGN · FULL STRESS", before + "  ->  " + AlignmentName(mState.alignment), "align", FeedbackTone::ACTION);
          break;
        }
        case 6u:
        {
          const std::string before = DirectionName(mState.direction);
          mState.direction = mState.direction == LayoutDirection::LEFT_TO_RIGHT ? LayoutDirection::RIGHT_TO_LEFT : LayoutDirection::LEFT_TO_RIGHT;
          RecordFeedback("X", "DIRECTION · FULL STRESS", before + "  ->  " + DirectionName(mState.direction), "direction", FeedbackTone::ACTION);
          break;
        }
        case 7u:
        {
          const std::string before = FitName(mState.fit);
          mState.fit = static_cast<FitMode>((static_cast<uint32_t>(mState.fit) + 1u) % 3u);
          RecordFeedback("X", "TEXT FIT · FULL STRESS", before + "  ->  " + FitName(mState.fit), "fit", FeedbackTone::ACTION);
          break;
        }
        case 8u:
        {
          const std::string before = mState.async ? "ON" : "OFF";
          mState.async = !mState.async;
          RecordFeedback("X", "ASYNC · FULL STRESS", before + "  ->  " + (mState.async ? "ON" : "OFF"), "async", FeedbackTone::ACTION);
          break;
        }
        case 9u:
        {
          const std::string before = LayoutName(mState.layout);
          mState.layout = static_cast<TargetLayoutMode>((static_cast<uint32_t>(mState.layout) + 1u) % 5u);
          RecordFeedback("X", "LAYOUT · FULL STRESS", before + "  ->  " + LayoutName(mState.layout), "size", FeedbackTone::ACTION);
          break;
        }
        case 10u:
        {
          const std::string before = ScenarioName(mScenarioIndex);
          SelectScenario(1, false);
          RecordFeedback("X", "SCENARIO · FULL STRESS", before + "  ->  " + ScenarioName(mScenarioIndex), "scenario", FeedbackTone::ACTION);
          break;
        }
        case 11u:
        {
          const std::string beforePadding = PaddingName(mState.padding);
          const std::string beforeMulti   = mState.multiLine ? "true" : "false";
          mState.padding = (mState.padding + 1u) % 3u;
          mState.multiLine = !mState.multiLine;
          RecordFeedback("X", "PADDING + MULTILINE · FULL STRESS",
                         beforePadding + " -> " + PaddingName(mState.padding) + " · " + beforeMulti + " -> " + (mState.multiLine ? "true" : "false"),
                         "multiline", FeedbackTone::ACTION);
          break;
        }
      }
      ++mStressIteration;
      ApplyState(false, true);
      return true;
    }
    return false;
  }

  void Reset()
  {
    StopAutomation();
    mState                = TestState{};
    mScenarioIndex        = 0u;
    mAppliedScenarioIndex = SCENARIOS.size();
    mQueryOrder           = 0u;
    mUpdateCount          = 0u;
    mAsyncRenderCompletions = 0u;
    mAsyncNaturalRequests   = 0u;
    mAsyncNaturalCompletions = 0u;
    mAsyncHfwRequests       = 0u;
    mAsyncHfwCompletions    = 0u;
    mAsyncRenderWidth = mAsyncRenderHeight = 0.0f;
    mAsyncNaturalWidth = mAsyncNaturalHeight = 0.0f;
    mAsyncHfwWidth = mAsyncHfwHeight = 0.0f;
    RecordFeedback("Z", "RESET", "All controls -> representative defaults", "", FeedbackTone::ACTION);
    ApplyState(true, true);
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    const bool escape = IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK);
    if(mAutoRunning)
    {
      if(mAutoInteractive && (escape || UpperKey(event) == "V"))
      {
        CancelInteractiveAutoVerification();
      }
      return;
    }

    if(escape)
    {
      mApplication.Quit();
      return;
    }

    mAutoResultVisible = false;
    bool handled = true;
    if(IsKey(event, DALI_KEY_CURSOR_LEFT))
    {
      const std::string before = ScalarName(mState.width);
      mState.width = std::max(MIN_TARGET_SIZE, mState.width - SIZE_STEP);
      RecordChange("LEFT", "WIDTH", before, ScalarName(mState.width), "size");
    }
    else if(IsKey(event, DALI_KEY_CURSOR_RIGHT))
    {
      const std::string before = ScalarName(mState.width);
      mState.width += SIZE_STEP;
      RecordChange("RIGHT", "WIDTH", before, ScalarName(mState.width), "size");
    }
    else if(IsKey(event, DALI_KEY_CURSOR_DOWN))
    {
      const std::string before = ScalarName(mState.height);
      mState.height = std::max(MIN_TARGET_SIZE, mState.height - SIZE_STEP);
      RecordChange("DOWN", "HEIGHT", before, ScalarName(mState.height), "size");
    }
    else if(IsKey(event, DALI_KEY_CURSOR_UP))
    {
      const std::string before = ScalarName(mState.height);
      mState.height += SIZE_STEP;
      RecordChange("UP", "HEIGHT", before, ScalarName(mState.height), "size");
    }
    else
    {
      const std::string key = UpperKey(event);
      if(key.size() == 1u && key[0] >= '0' && key[0] <= '5')
      {
        const std::string before = MaximumLinesName(mState.maximumLines);
        mState.maximumLines = key[0] - '0';
        RecordChange(key, "MAXIMUM LINES", before, MaximumLinesName(mState.maximumLines), "max");
      }
      else if(key == "E")
      {
        const std::string before = OverflowName(mState.overflow);
        mState.overflow = mState.overflow == Text::OverflowMode::ELLIPSIS ? Text::OverflowMode::CLIP : Text::OverflowMode::ELLIPSIS;
        RecordChange("E", "OVERFLOW", before, OverflowName(mState.overflow), "overflow");
      }
      else if(key == "W")
      {
        const std::string before = WrapName(mState.wrap);
        mState.wrap = static_cast<Text::LineWrapMode>((static_cast<uint32_t>(mState.wrap) + 1u) % 4u);
        RecordChange("W", "WRAP", before, WrapName(mState.wrap), "wrap");
      }
      else if(key == "H")
      {
        const std::string before = AlignmentName(mState.alignment);
        mState.alignment = static_cast<Text::Alignment>((static_cast<uint32_t>(mState.alignment) + 1u) % 3u);
        RecordChange("H", "ALIGN", before, AlignmentName(mState.alignment), "align");
      }
      else if(key == "D")
      {
        const std::string before = DirectionName(mState.direction);
        mState.direction = mState.direction == LayoutDirection::LEFT_TO_RIGHT ? LayoutDirection::RIGHT_TO_LEFT : LayoutDirection::LEFT_TO_RIGHT;
        RecordChange("D", "DIRECTION", before, DirectionName(mState.direction), "direction");
      }
      else if(key == "A")
      {
        const std::string before = mState.async ? "ON" : "OFF";
        mState.async = !mState.async;
        RecordChange("A", "ASYNC", before, mState.async ? "ON" : "OFF", "async");
      }
      else if(key == "F")
      {
        const std::string before = FitName(mState.fit);
        mState.fit = static_cast<FitMode>((static_cast<uint32_t>(mState.fit) + 1u) % 3u);
        RecordChange("F", "TEXT FIT", before, FitName(mState.fit), "fit");
      }
      else if(key == "L")
      {
        const std::string before = LayoutName(mState.layout);
        mState.layout = static_cast<TargetLayoutMode>((static_cast<uint32_t>(mState.layout) + 1u) % 5u);
        RecordChange("L", "LAYOUT", before, LayoutName(mState.layout), "size");
      }
      else if(key == "G")
      {
        const std::string before = PaddingName(mState.padding);
        mState.padding = (mState.padding + 1u) % 3u;
        RecordChange("G", "PADDING", before, PaddingName(mState.padding), "padding");
      }
      else if(key == "M")
      {
        const std::string before = mState.multiLine ? "true" : "false";
        mState.multiLine = !mState.multiLine;
        RecordChange("M", "MULTILINE", before, mState.multiLine ? "true" : "false", "multiline");
      }
      else if(key == "N")
      {
        const std::string before = ScenarioName(mScenarioIndex);
        SelectScenario(1, true);
        RecordFeedback("N", "NEXT SCENARIO", before + "  ->  " + ScenarioName(mScenarioIndex) + " · recommended settings applied", "scenario");
      }
      else if(key == "B")
      {
        const std::string before = ScenarioName(mScenarioIndex);
        SelectScenario(-1, true);
        RecordFeedback("B", "PREVIOUS SCENARIO", before + "  ->  " + ScenarioName(mScenarioIndex) + " · recommended settings applied", "scenario");
      }
      else if(key == "Q")
      {
        const uint32_t before = mQueryOrder;
        mQueryOrder = (mQueryOrder + 1u) % 7u;
        RecordChange("Q", "QUERY ORDER", std::to_string(before), std::to_string(mQueryOrder), "query");
      }
      else if(key == "V")
      {
        StartAutoVerification(true);
        return;
      }
      else if(key == "T")
      {
        StartTransition();
        return;
      }
      else if(key == "R")
      {
        StartAba();
        return;
      }
      else if(key == "S")
      {
        ToggleResizeStress();
        return;
      }
      else if(key == "X")
      {
        ToggleFullStress();
        return;
      }
      else if(key == "Z")
      {
        Reset();
        return;
      }
      else
      {
        handled = false;
      }
    }

    if(handled)
    {
      StopAutomation();
      ApplyState();
    }
  }

private:
  Application& mApplication;
  bool         mAutoMode = false;
  Window       mWindow;
  StackLayout  mRoot;
  StackLayout  mStage;
  StackLayout  mInspector;
  Label        mHeader;
  Label        mScenarioLabel;
  Label        mTestLabel;
  Label        mChangeBanner;
  Label        mSettings;
  Label        mMeasurements;
  Label        mRuntime;
  Label        mFooter;

  TestState  mState;
  std::size_t mScenarioIndex        = 0u;
  std::size_t mAppliedScenarioIndex = SCENARIOS.size();
  std::string mVeryLongText;
  std::string mLastAction;
  std::string mFeedbackKey    = "READY";
  std::string mFeedbackTitle  = "SAMPLE READY";
  std::string mFeedbackDetail = "Press a key; the changed value will appear here as before -> after";
  std::string mChangedField;
  FeedbackTone mFeedbackTone  = FeedbackTone::READY;
  uint32_t     mFeedbackCount = 0u;
  float        mWindowWidth   = static_cast<float>(WINDOW_WIDTH);
  float        mWindowHeight  = static_cast<float>(WINDOW_HEIGHT);

  Timer mQueryTimer;
  Timer mAutomationTimer;
  Timer mStressTimer;
  Timer mAutoTimer;

  std::vector<AutoCase>                 mAutoCases;
  std::map<std::string, AutoMeasurement> mAutoBaselines;
  std::size_t                            mAutoCaseIndex     = 0u;
  uint32_t                               mAutoCaseTicks     = 0u;
  uint32_t                               mAutoCheckCount    = 0u;
  uint32_t                               mAutoPassCount     = 0u;
  uint32_t                               mAutoFailCount     = 0u;
  uint32_t                               mAutoCasePassStart = 0u;
  uint32_t                               mAutoCaseFailStart = 0u;
  uint32_t                               mAutoNaturalStart  = 0u;
  uint32_t                               mAutoHfwStart      = 0u;
  uint32_t                               mAutoRenderStart   = 0u;
  uint32_t                               mAutoStableObservations = 0u;
  AutoMeasurement                        mAutoPreviousMeasurement;
  bool                                   mAutoPreviousValid = false;
  bool                                   mAutoRunning       = false;
  bool                                   mAutoInteractive   = false;
  bool                                   mAutoResultVisible = false;
  bool                                   mAutoQueryReady    = false;
  int                                    mAutoExitCode      = 0;
  TestState                              mAutoSavedState;
  std::size_t                            mAutoSavedScenarioIndex = 0u;
  uint32_t                               mAutoSavedQueryOrder    = 0u;
  bool                                   mAutoSavedManualState   = false;

  AutomationMode mAutomation      = AutomationMode::NONE;
  StressMode     mStress          = StressMode::NONE;
  uint32_t       mAutomationIndex = 0u;
  uint32_t       mStressIteration = 0u;
  bool           mResizeGrowing   = true;

  uint32_t    mQueryOrder = 0u;
  std::string mQueryOrderName;
  std::string mQueryPhase;
  Vector3     mNatural{0.0f, 0.0f, 0.0f};
  Vector3     mActualSize{0.0f, 0.0f, 0.0f};
  float       mHeightForWidth = 0.0f;
  bool        mNaturalValid   = false;
  bool        mHfwValid       = false;
  int         mLineBefore     = -1;
  int         mLineWidthBefore = -1;
  int         mLineAfter      = -1;
  int         mLineWidthAfter = -1;

  uint32_t mUpdateCount             = 0u;
  uint32_t mAsyncRenderCompletions  = 0u;
  uint32_t mAsyncNaturalRequests    = 0u;
  uint32_t mAsyncNaturalCompletions = 0u;
  uint32_t mAsyncHfwRequests        = 0u;
  uint32_t mAsyncHfwCompletions     = 0u;
  float    mAsyncRenderWidth        = 0.0f;
  float    mAsyncRenderHeight       = 0.0f;
  float    mAsyncNaturalWidth       = 0.0f;
  float    mAsyncNaturalHeight      = 0.0f;
  float    mAsyncHfwWidth           = 0.0f;
  float    mAsyncHfwHeight          = 0.0f;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  bool autoVerify = false;
  for(int argumentIndex = 1; argumentIndex < argc; ++argumentIndex)
  {
    if(std::string(argv[argumentIndex]) == "--auto-verify")
    {
      autoVerify = true;
    }
  }

  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextMaximumLinesController controller(application, autoVerify);
  application.MainLoop();
  return controller.GetExitCode();
}
