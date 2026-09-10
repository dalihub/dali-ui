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

#include "common/image-capture.h"
#include "text-test-case.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <map>
#include <sstream>
#include <utility>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;
namespace IC = TextManualTest::ImageCheck;

namespace
{
using Clock = std::chrono::steady_clock;
double Seconds(Clock::time_point from, Clock::time_point to = Clock::now())
{
  return std::chrono::duration<double>(to - from).count();
}

enum class Action
{
  ELLIPSIS,
  IMMEDIATE,
  FINISH_LOOP,
  LOOP_ONE,
  LOOP_TWO,
  LOOP_THREE,
  INFINITE_TWO,
  RESTART,
  WRONG_ORIENTATION,
  MANUAL_FIT,
  AUTO_SOURCE,
  AUTO_RESIZE,
  AUTO_OVERFLOW,
  AUTO_FIT,
  LIFECYCLE
};
struct Case
{
  std::string        name, text;
  bool               rtl{false}, multiline{false}, padding{false}, fits{false};
  Text::Alignment    alignment{Text::Alignment::START};
  Action             action{Action::ELLIPSIS};
  bool               async{false};
  Text::LineWrapMode wrap{Text::LineWrapMode::WORD};
  int                maximumLines{2};
  float              height{96}, fontSize{24}, renderScale{1}, widthRatio{1};
  int                boundary{0}; // -1/+1: natural width -/+ 4px, 2: resize policy fixture
  int                gap{40};
  float              delay{1};
  bool               invisibleOverflow{false}; // Only a trailing empty line is clipped.
  bool               rigidOrigin{true}; // Fixture contract: retained runs do not reorder at the elision boundary.
  bool               autoTrigger{false}, finishStop{false};
};

// Unique sections make motion distinguishable from repeated identical glyphs.
const char* LATIN  = "START Abc 123 | wider WXYZ | narrow ijkl | finish END";
const char* ARABIC = "البداية العربية 123 ثم كلمات مختلفة 456 حتى نهاية النص الطويل";
const char* LINES  = "FIRST line alpha bravo\nSECOND line charlie delta\nTHIRD echo foxtrot\nFOURTH golf hotel\nFIFTH india juliet";

std::vector<Case> MakeCases(bool marquee)
{
  std::vector<Case> base;
  auto              add = [&](const char* name, const char* text, bool rtl = false, bool multi = false,
                 Text::Alignment align = Text::Alignment::START, bool padding = false,
                 Action action = Action::ELLIPSIS, bool fits = false)
  { base.push_back({name, text, rtl, multi, padding, fits, align, action, false}); };
  if(!marquee)
  {
    add("ltr-start", LATIN);
    add("ltr-center", LATIN, false, false, Text::Alignment::CENTER);
    add("ltr-end-padding", LATIN, false, false, Text::Alignment::END, true);
    add("rtl-arabic", ARABIC, true);
    add("bidi-ltr-neutral", "Prefix [123] العربية (456) עברית -- trailing English words fill the available width");
    add("bidi-rtl-neutral", "العربية [123] English (456) עברית ثم نهاية النص الطويل جدا", true);
    add("combining-emoji", "cafe\u0301 A\u030A 👩🏽‍🚀 👨‍👩‍👧‍👦 abcdefghijklmnopqrstuvwxyz trailing text");
    add("multiline-max2", "First paragraph alpha bravo charlie delta echo foxtrot golf hotel india juliet kilo lima. Second paragraph mike november oscar papa quebec romeo sierra tango uniform victor whiskey xray yankee zulu. Final paragraph must exceed two wrapped lines at every supported fixture width.", false, true);
    add("explicit-lines-max2", LINES, false, true);
    add("fits-no-ellipsis", "Ab", false, false, Text::Alignment::START, false, Action::ELLIPSIS, true);
    add("rtl-center", ARABIC, true, false, Text::Alignment::CENTER);
    add("rtl-end-padding", ARABIC, true, false, Text::Alignment::END, true);
    add("hebrew", "עברית עם מילים ומספרים 12345 וסימני פיסוק לבדיקת טקסט ארוך מאוד", true);
    add("korean-character-wrap", "한글 텍스트에서 줄바꿈과 말줄임표가 올바르게 동작하는지 확인합니다. 폭과 높이 제한을 적용한 여러 줄의 긴 텍스트입니다.", false, true);
    base.back().wrap = Text::LineWrapMode::CHARACTER;
    add("bidi-isolates", "English [\u2067العربية 123\u2069] [\u2066Latin 456\u2069] tail with enough characters for truncation");
    add("numbers-punctuation", "1234567890 / [123] (456) +789 -- abcdefghijklmnopqrstuvwxyz ... END");
    add("ligature-combining", "office affinity ffi fi fl cafe\u0301 nai\u0308ve A\u030A e\u0301 repeated office affinity until overflow");
    add("emoji-zwj-flags", "Family 👨‍👩‍👧‍👦 astronaut 👩🏽‍🚀 flag 🇰🇷 🇺🇸 rainbow 🏳️‍🌈 developer 👨🏻‍💻 repeated text");
    add("multiline-character-max1", LATIN, false, true);
    base.back().wrap         = Text::LineWrapMode::CHARACTER;
    base.back().maximumLines = 1;
    add("multiline-rtl-max3", LINES, true, true);
    base.back().maximumLines = 3;
    add("multiline-height-limit", LINES, false, true);
    base.back().maximumLines = 0;
    base.back().height       = 64;
    add("exact-two-lines", "First\nSecond", false, true, Text::Alignment::START, false, Action::ELLIPSIS, true);
    add("trailing-newline", "First\nSecond\n", false, true);
    base.back().invisibleOverflow = true;
    add("width-below-fit", "Boundary label");
    base.back().boundary = -1;
    add("width-above-fit", "Boundary label", false, false, Text::Alignment::START, false, Action::ELLIPSIS, true);
    base.back().boundary = 1;
    add("narrow-large-font", LATIN);
    base.back().widthRatio = 0.55f;
    base.back().fontSize   = 32;
    add("render-scale-2", LATIN);
    base.back().renderScale = 2;
  }
  else
  {
    add("immediate-ltr", LATIN, false, false, Text::Alignment::START, false, Action::IMMEDIATE);
    add("immediate-rtl", ARABIC, true, false, Text::Alignment::START, false, Action::IMMEDIATE);
    base.back().rigidOrigin = false; // Arabic + LTR numbers: not a pure RTL fixture.
    add("immediate-center", LATIN, false, false, Text::Alignment::CENTER, false, Action::IMMEDIATE);
    add("immediate-end-padding", LATIN, false, false, Text::Alignment::END, true, Action::IMMEDIATE);
    add("finish-loop", LATIN, false, false, Text::Alignment::START, false, Action::FINISH_LOOP);
    add("loop-count-1", LATIN, false, false, Text::Alignment::START, false, Action::LOOP_ONE);
    add("loop-count-2", LATIN, false, false, Text::Alignment::START, false, Action::LOOP_TWO);
    add("on-overflow-start", LATIN, false, false, Text::Alignment::START, false, Action::AUTO_OVERFLOW);
    add("on-overflow-fits", "Ab", false, false, Text::Alignment::START, false, Action::AUTO_FIT, true);
    add("vertical-finish-loop", LINES, false, true, Text::Alignment::START, false, Action::FINISH_LOOP);
    add("vertical-immediate", LINES, false, true, Text::Alignment::START, false, Action::IMMEDIATE);
    add("vertical-loop-2", LINES, false, true, Text::Alignment::START, false, Action::LOOP_TWO);
    add("rtl-finish-loop", ARABIC, true, false, Text::Alignment::START, true, Action::FINISH_LOOP);
    base.back().rigidOrigin = false;
    add("loop-count-3", LATIN, false, false, Text::Alignment::START, false, Action::LOOP_THREE);
    add("infinite-observe-2", LATIN, false, false, Text::Alignment::START, false, Action::INFINITE_TWO);
    add("restart-after-stop", LATIN, false, false, Text::Alignment::START, false, Action::RESTART);
    add("wrong-vertical-single-line", LATIN, false, false, Text::Alignment::START, false, Action::WRONG_ORIENTATION);
    add("wrong-horizontal-multiline", LINES, false, true, Text::Alignment::START, false, Action::WRONG_ORIENTATION);
    add("manual-fitting-text", "Fitting text", false, false, Text::Alignment::START, false, Action::MANUAL_FIT, true);
    add("on-overflow-source-cycle", "Ab", false, false, Text::Alignment::START, false, Action::AUTO_SOURCE, true);
    add("on-overflow-resize-cycle", "Width test", false, false, Text::Alignment::START, false, Action::AUTO_RESIZE);
    base.back().boundary = 2;
    add("on-overflow-source-cycle-finish", "Ab", false, false, Text::Alignment::START, false, Action::AUTO_SOURCE, true);
    base.back().finishStop = true;
    add("on-overflow-resize-cycle-finish", "Width test", false, false, Text::Alignment::START, false, Action::AUTO_RESIZE);
    base.back().boundary = 2;
    base.back().finishStop = true;
    for(bool finish : {false, true})
    {
      add(finish ? "on-overflow-vertical-source-cycle-finish" : "on-overflow-vertical-source-cycle",
          "Ab", false, true, Text::Alignment::START, false, Action::AUTO_SOURCE, true);
      base.back().finishStop = finish;
    }
    for(bool vertical : {false, true})
      for(bool twoLoops : {false, true})
      {
        const std::string name = std::string("on-overflow-") + (vertical ? "vertical-" : "horizontal-") + (twoLoops ? "loop-2" : "loop-1");
        add(name.c_str(), vertical ? LINES : LATIN, false, vertical, Text::Alignment::START, false, twoLoops ? Action::LOOP_TWO : Action::LOOP_ONE);
        base.back().autoTrigger = true;
      }
    add("large-gap-delay", LATIN, false, false, Text::Alignment::START, false, Action::LOOP_TWO);
    base.back().gap   = 100;
    base.back().delay = 1.5f;
    add("render-scale-2", LATIN, false, false, Text::Alignment::START, false, Action::IMMEDIATE);
    base.back().renderScale = 2;
    add("immediate-rtl-pure", "البداية العربية ثم كلمات مختلفة حتى نهاية النص الطويل جدا", true, false, Text::Alignment::START, false, Action::IMMEDIATE);
    add("immediate-hebrew-pure", "עברית עם מילים שונות לבדיקת טקסט ארוך מאוד לאורך השורה", true, false, Text::Alignment::START, false, Action::IMMEDIATE);
    add("immediate-bidi-ltr", "English [العربية] [עברית] more words and numbers 123 until the end of the long sentence", false, false, Text::Alignment::START, false, Action::IMMEDIATE);
    base.back().rigidOrigin = false;
  }
  if(marquee)
  {
    for(bool vertical : {false, true})
      for(bool automatic : {false, true})
        for(bool finish : {false, true})
        {
          const std::string name = std::string("lifecycle-") + (vertical ? "vertical-" : "horizontal-") +
                                   (automatic ? "auto-" : "manual-") + (finish ? "finish" : "immediate");
          add(name.c_str(), vertical ? LINES : LATIN, false, vertical, Text::Alignment::START, false, Action::LIFECYCLE);
          base.back().autoTrigger = automatic;
          base.back().finishStop = finish;
        }
  }
  auto cases = base;
  for(auto item : base)
  {
    item.async = true;
    cases.push_back(std::move(item));
  }
  return cases;
}

class PixelTextCase : public TextManualTest::TestCase, public ConnectionTracker
{
public:
  explicit PixelTextCase(bool marquee)
  : mMarquee(marquee)
  {
  }
  const char* GetId() const override
  {
    return mMarquee ? "text-marquee" : "text-ellipsis";
  }
  Dali::String GetName() const override
  {
    return mMarquee ? "Marquee" : "Ellipsis";
  }
  Dali::String GetDescription() const override
  {
    return mMarquee ? "Pixel validation of start, motion, stop, loops and ellipsis restoration · Sync / Async"
                    : "Pixel validation of overflow, restoration and Sync / Async parity · LTR / RTL / BiDi";
  }

  void OnEnter(View content) override
  {
    mManualCases = MakeCases(mMarquee);
    mManualCases.resize(mManualCases.size() / 2); // Async is a control, not duplicate manual pages.
    mRoot = StackLayout::New(StackOrientation::VERTICAL);
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    mRoot.SetSpacing(6);
    mRoot.SetPadding(Insets(8, 8, 6, 6));
    mRoot.SetBackgroundColor(UiColor(0xE2E8F0));
    mRunButton = Button("Run auto: Sync + Async [V]", "run", "V");
    mRunButton.SetRequestedWidth(MATCH_PARENT);
    mRunButton.SetLayoutParams(StackLayoutParams::New());
    mRunButton.SetBackgroundColor(UiColor(0x0F766E));
    mRoot.Add(mRunButton);
    mProgress = Chrome("", "progress", 15, 0xF8FAFC, 0x0F172A);
    mRoot.Add(mProgress);
    mStatus = Chrome("", "status", 15, 0xF8FAFC, 0x075985);
    mRoot.Add(mStatus);

    mBody = StackLayout::New(StackOrientation::VERTICAL);
    mBody.SetRequestedWidth(MATCH_PARENT);
    mBody.SetRequestedHeight(WRAP_CONTENT);
    mBody.SetSpacing(10);
    mPreview = StackLayout::New(StackOrientation::VERTICAL);
    mPreview.SetRequestedWidth(MATCH_PARENT);
    mPreview.SetRequestedHeight(WRAP_CONTENT);
    mPreview.SetSpacing(6);
    AddButtons(mPreview, {{"Previous [B]", "B"}, {"Next [N]", "N"}, {"Reset [Z]", "Z"}});
    mScenario = Chrome("", "scenario", 16, 0x1E293B, 0xF8FAFC);
    mPreview.Add(mScenario);

    mStage = StackLayout::New(StackOrientation::VERTICAL);
    mStage.SetUiScalePolicy(UiScalePolicy::DISABLED);
    mStage.SetPadding(Insets(12, 12, 12, 12));
    mStage.SetBackgroundColor(UiColor(0xCBD5E1));
    // The manual viewport clips this stage. Automatic capture moves the stage
    // outside all clipping ancestors so their stencil cannot hide the fixture.
    // The red outline belongs to the UI frame, never to the captured fixture.
    // Keep the existing calibration markers and pixel comparison ROIs intact.
    mFrame = StackLayout::New(StackOrientation::VERTICAL);
    mFrame.SetUiScalePolicy(UiScalePolicy::DISABLED);
    mFrame.SetPadding(Insets(2, 2, 2, 2));
    mFrame.SetBackgroundColor(UiColor(0xDC2626));
    mFixture = AbsoluteLayout::New();
    mFixture.SetUiScalePolicy(UiScalePolicy::DISABLED);
    mFixture.SetLayoutDirection(LayoutDirection::LEFT_TO_RIGHT);
    mFixture.SetPivot(Pivot::TOP_LEFT);
    mFixture.SetRequestedWidth(304);
    mFixture.SetRequestedHeight(112);
    mFixture.SetBackgroundColor(UiColor(0xFFFFFF));
    mFixture.SetAutomationId((std::string(GetId()) + "-fixture").c_str());
    mFrame.Add(mFixture);
    mStage.Add(mFrame);
    mViewport = ScrollView::New();
    mViewport.SetRequestedWidth(MATCH_PARENT);
    mViewport.SetScrollDirection(ScrollDirection::Both);
    mViewport.SetContent(mStage);
    mViewport.SetAutomationId((std::string(GetId()) + "-preview").c_str());
    mPreview.Add(mViewport);
    mManualMetrics = Chrome("", "measurements");
    mPreview.Add(mManualMetrics);
    mDetails = Chrome("No verification run yet.", "details", 15, 0x1E293B, 0xF8FAFC);
    mDetails.SetAccessibilityValue("NOT_RUN");
    mPreview.Add(mDetails);

    mInspector = StackLayout::New(StackOrientation::VERTICAL);
    mInspector.SetRequestedWidth(MATCH_PARENT);
    mInspector.SetRequestedHeight(WRAP_CONTENT);
    mInspector.SetSpacing(6);
    mInspector.SetAutomationId((std::string(GetId()) + "-controls").c_str());
    auto controls = mInspector;
    if(mMarquee) AddButtons(controls, {{"Start [R]", "R"}, {"Stop [S]", "S"}});
    AddButtons(controls, {{"Width - [-]", "-"}, {"Width + [+]", "+"}});
    AddButtons(controls, {{"Height -  [", "["}, {"Height +  ]", "]"}});
    AddButtons(controls, {{"Rendering [A]", "A"}, {"Overflow [E]", "E"}});
    AddButtons(controls, {{"Direction [D]", "D"}, {"Alignment [H]", "H"}});
    AddButtons(controls, {{"Multiline [M]", "M"}, {"Max lines [K]", "K"}});
    AddButtons(controls, {{"Wrap [W]", "W"}, {"Padding [P]", "P"}});
    AddButtons(controls, {{"Font size [F]", "F"}, {"Render scale [U]", "U"}});
    AddButtons(controls, {{"Short / original [Y]", "Y"}});
    if(mMarquee)
    {
      AddButtons(controls, {{"Orientation [O]", "O"}, {"Stop mode [I]", "I"}});
      AddButtons(controls, {{"Trigger policy [T]", "T"}, {"Loop count [L]", "L"}});
      AddButtons(controls, {{"Speed [C]", "C"}, {"Gap [G]", "G"}});
      AddButtons(controls, {{"Delay [J]", "J"}});
    }
    mSettings = Chrome("", "settings");
    mRuntime  = Chrome("", "runtime");
    controls.Add(mSettings);
    controls.Add(mRuntime);
    mBody.Add(mPreview);
    mBody.Add(mInspector);
    mScroll = ScrollView::New();
    mScroll.SetRequestedWidth(MATCH_PARENT);
    mScroll.SetLayoutParams(StackLayoutParams::New().SetWeight(1));
    mScroll.SetScrollDirection(ScrollDirection::Vertical);
    mScroll.SetAutomationId((std::string(GetId()) + "-body").c_str());
    mScroll.SetContent(mBody);
    mRoot.Add(mScroll);
    content.Add(mRoot);
    mWindow.ResizedSignal().Connect(this, &PixelTextCase::OnWindowResized);
    mWindow.VisibilityChangedSignal().Connect(this, [this](Window, bool)
    {
      // Record actual minimization before a target suspends its timers.
      if(mRunning && mStep == Step::WINDOW_CYCLE) ObserveWindowMinimized();
    });
    UpdateResponsiveUi();
    LoadManualCase();
    mManualTimer = Timer::New(200);
    mManualTimer.TickSignal().Connect(this, &PixelTextCase::RefreshManual);
    mManualTimer.Start();
  }

  void OnExit() override
  {
    if(mRunning && mStep == Step::WINDOW_CYCLE) mWindow.Minimize(false);
    if(mRunning) Log("[AUTO][CANCELLED]");
    if(mTimer) mTimer.Stop();
    if(mManualTimer) mManualTimer.Stop();
    mCapture.Cancel();
    if(mTarget)
    {
      mTarget.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
      mTarget.StopMarquee();
    }
    DisconnectAll();
    if(mRoot) mRoot.Unparent();
    mRunning = false;
    mReport.Close();
  }

  void OnKeyEvent(KeyEvent key) override
  {
    if(key.GetState() != KeyEvent::UP) return;
    std::string action = key.GetKeyString().CStr();
    if(action.size() == 1) action[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(action[0])));
    ManualAction(action);
  }
  bool IsAutomationFinished() const override
  {
    return mFinished;
  }
  int GetExitCode() const override
  {
    return mFinished ? mExitCode : 3;
  }

  void StartAutomation() override
  {
    if(mRunning) return;
#if !TEXT_PIXEL_CHECKS
    Error("Pixel validation is not built. Use manual controls or rebuild with TEXT_PIXEL_CHECKS=ON.");
    return;
#endif
    mManualActive = false;
    DropTarget();
    PinAutoPreview(true);
    mFinished     = false;
    mExitCode     = 3;
    mFailedChecks = mFailedCases = mChecks = mCompleted = 0;
    mFailures.clear();
    mBaselines.clear();
    mIndex = 0;
    mCases = MakeCases(mMarquee);
    // Public-only runner selection; both render paths remain in every subset.
    if(const char* filter = std::getenv("DALI_TEXT_CASE_FILTER"))
    {
      mCases.erase(std::remove_if(mCases.begin(), mCases.end(), [&](const Case& c)
      { return c.name.find(filter) == std::string::npos; }), mCases.end());
      if(mCases.empty())
      {
        Error("DALI_TEXT_CASE_FILTER matched no cases");
        return;
      }
    }
    mReportFailed = false;
    if(!OpenReport())
    {
      Error("cannot open report: " + mReportPath);
      return;
    }
    std::string       pattern = mReportPath + ".images-XXXXXX";
    std::vector<char> writable(pattern.begin(), pattern.end());
    writable.push_back('\0');
    const char* directory = mkdtemp(writable.data());
    if(!directory)
    {
      Error("cannot create artifact directory beside report");
      return;
    }
    mArtifacts        = directory;
    const auto window = mWindow.GetPositionSize();
    mWindowWidth      = window.width;
    mWindowHeight     = window.height;
    if(window.width < 240 || window.height < 280)
    {
      Error("window must provide at least 240 x 280 pixels; resize then rerun");
      return;
    }
    mWidth = std::min(360, window.width - 32);
    SetFixtureSize(mWidth + 16, 112);
    mScroll.SetScrollPosition(Vector2::ZERO);
    mFixture.RemoveAll();
    AddMarker(2, 2, UiColor(0xFF0000));
    AddMarker(mWidth + 10, 106, UiColor(0x0000FF));
    mRunning = true;
    SetBanner("AUTO VERIFY RUNNING\nSync + Async verification · press V to cancel", "RUNNING");
    mDetails.SetText("Verification running. Failures will be listed here.");
    mDetails.SetAccessibilityValue("RUNNING");
    Log("[AUTO][BEGIN] cases=" + std::to_string(mCases.size()) + " artifacts=" + mArtifacts);
    Log("[CONFIG] system-font size=24 scale=1 renderScale=1 viewport=" + std::to_string(mWidth) + "x96 channelTolerance=16 changedForegroundFraction=0.001 maximumChangedPixels=4");
    SetStep(Step::CALIBRATION);
    mTimer = Timer::New(80);
    mTimer.TickSignal().Connect(this, &PixelTextCase::Tick);
    mTimer.Start();
  }

private:
  enum class Step
  {
    CALIBRATION,
    CLIP_REFERENCE,
    REFERENCE_TO_ELLIPSIS,
    BASELINE,
    CLIP,
    RESTORE,
    START,
    OBSERVE,
    STOP_RESTORE,
    FIT,
    FIT_REFERENCE,
    AUTO_RETURN,
    LIFECYCLE_HIDDEN,
    LIFECYCLE_IDLE,
    WINDOW_CYCLE
  };

  const char* PhaseName() const
  {
    switch(mStep)
    {
      case Step::CALIBRATION:
        return "Capture calibration";
      case Step::CLIP_REFERENCE:
        return "Fresh CLIP reference";
      case Step::REFERENCE_TO_ELLIPSIS:
        return "CLIP to ellipsis reference";
      case Step::BASELINE:
        return "Ellipsis baseline";
      case Step::CLIP:
        return "Switch to CLIP";
      case Step::RESTORE:
        return "Restore ellipsis";
      case Step::START:
        return "Marquee start position";
      case Step::OBSERVE:
        return "Observe movement / loops";
      case Step::STOP_RESTORE:
        return "Stop and restore";
      case Step::FIT:
        return "Check start condition";
      case Step::FIT_REFERENCE:
        return "Fitting text reference";
      case Step::AUTO_RETURN:
        return "Return to fitting text";
      case Step::LIFECYCLE_HIDDEN:
        return "Hidden content / restore";
      case Step::LIFECYCLE_IDLE:
        return "Stopped state / restart";
      case Step::WINDOW_CYCLE:
        return "Window minimize / restore";
    }
    return "Unknown";
  }

  void UpdateResponsiveUi()
  {
    const auto size = mWindow.GetPositionSize();
    const bool wide = size.width >= 1000;
    mBody.SetOrientation(wide ? StackOrientation::HORIZONTAL : StackOrientation::VERTICAL);
    mPreview.SetRequestedWidth(wide ? std::max(320.0f, float(size.width) - 410) : MATCH_PARENT);
    mInspector.SetRequestedWidth(wide ? 360 : MATCH_PARENT);
    mViewport.SetRequestedHeight(mAutoPreviewPinned ? 0 : std::clamp(size.height * 0.4f, 150.0f, 360.0f));
    UpdateStageSize();
  }
  void UpdateStageSize()
  {
    const auto  size      = mWindow.GetPositionSize();
    const float available = !mAutoPreviewPinned && size.width >= 1000 ? size.width - 410.0f : size.width - 16.0f;
    mFrame.SetRequestedWidth(float(mFixtureWidth + 4));
    mFrame.SetRequestedHeight(float(mFixtureHeight + 4));
    mStage.SetRequestedWidth(std::max(available, float(mFixtureWidth + 28)));
    mStage.SetRequestedHeight(mAutoPreviewPinned ? float(mFixtureHeight + 28) : std::max(std::clamp(size.height * 0.4f, 150.0f, 360.0f), float(mFixtureHeight + 28)));
  }
  void PinAutoPreview(bool pin)
  {
    if(mAutoPreviewPinned == pin) return;
    mAutoPreviewPinned = pin;
    if(pin)
      mViewport.SetContent(View());
    else
      mStage.Unparent();
    // Reuse every view and the same fixture. Only its UI parent changes,
    // before a run or after capture has finished/cancelled.
    mRoot.RemoveAll();
    mRoot.Add(mRunButton);
    mRoot.Add(mProgress);
    mRoot.Add(mStatus);
    if(pin)
      mRoot.Add(mStage);
    else
    {
      mViewport.SetContent(mStage);
      mViewport.SetScrollPosition(Vector2::ZERO);
    }
    mRoot.Add(mScroll);
    UpdateResponsiveUi();
  }
  void SetFixtureSize(int width, int height)
  {
    mFixtureWidth  = width;
    mFixtureHeight = height;
    mFixture.SetRequestedWidth(float(width));
    mFixture.SetRequestedHeight(float(height));
    UpdateStageSize();
  }
  void SetBanner(const std::string& text, const char* state)
  {
    const std::string value(state);
    const uint32_t    color   = value == "RUNNING" ? 0x6D28D9 : value == "PASS"                       ? 0x166534
                                                              : (value == "FAIL" || value == "ERROR") ? 0x991B1B
                                                              : value == "CANCELLED"                  ? 0x92400E
                                                                                                      : 0x075985;
    const bool        compact = mRunning && mWindow.GetPositionSize().height < 450;
    const std::string caption = compact ? "AUTO VERIFY RUNNING · " + std::to_string(mChecks) + " checks · " + std::to_string(mFailedChecks) + " fails" : text;
    mStatus.SetText(caption.c_str());
    mStatus.SetBackgroundColor(UiColor(color));
    mStatus.SetAccessibilityValue(state);
    mRunButton.SetText(mRunning ? "Cancel auto [V]" : "Run auto: Sync + Async [V]");
  }
  void UpdateAutoPanels()
  {
    if(mCases.empty()) return;
    const auto        index   = std::min(mIndex, mCases.size() - 1);
    const auto&       c       = mCases[index];
    const bool        compact = mRunning && mWindow.GetPositionSize().height < 450;
    const std::string phase   = mRunning ? PhaseName() : mExitCode == 3 ? "Verification cancelled"
                                                       : mExitCode == 2 ? "Verification incomplete"
                                                                        : "Sync + Async verification finished";
    mProgress.SetText((std::string(mRunning ? "AUTO " : "RESULT · ") + (c.async ? "ASYNC" : "SYNC") +
                       "   CASE " + std::to_string(index + 1) + " / " + std::to_string(mCases.size()) + (compact ? "" : "\n" + phase))
                        .c_str());
    mScenario.SetText(("[" + std::to_string(index + 1) + "] " + c.name + (mRunning ? "\n" + phase : "")).c_str());
    if(mRunning) SetBanner("AUTO VERIFY RUNNING\nSync + Async verification · press V to cancel", "RUNNING");
    mManualMetrics.SetText(("LIVE RESULT\nCompleted: " + std::to_string(mCompleted) + " / " + std::to_string(mCases.size()) +
                            "   Checks: " + std::to_string(mChecks) + "\nPASS: " + std::to_string(mChecks - mFailedChecks) +
                            "   FAIL: " + std::to_string(mFailedChecks))
                             .c_str());
    const auto perPath = std::to_string(mCases.size() / 2);
    mSettings.SetText(("AUTOMATIC COVERAGE\nSync: " + perPath + "   Async: " + perPath +
                       "\n" + (c.rtl ? "RTL" : "LTR") + " · " + (c.multiline ? "Multiline" : "Single line") +
                       " · Font " + std::to_string(int(c.fontSize)) + " · Scale " + std::to_string(int(c.renderScale)))
                        .c_str());
    mRuntime.SetText(mRunning ? "RUN CONTROL\n[V] Cancel automatic verification\nManual options are locked while running."
                              : "RUN CONTROL\n[V] Run Sync + Async again\n[Z] Return to manual preview\nFull results and PNG paths are in the result card.");
    mAutoUiDirty = false;
  }

  StackLayout ButtonRow()
  {
    auto row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(WRAP_CONTENT);
    row.SetSpacing(6);
    return row;
  }
  Label Button(const char* text, const char* id, const std::string& action)
  {
    auto button = Chrome(text, id, 16, 0xFFFFFF, 0x1E3A5F);
    button.SetRequestedWidth(0);
    button.SetRequestedHeight(46);
    button.SetPadding(Insets(4, 4, 2, 2));
    button.SetLayoutParams(StackLayoutParams::New().SetWeight(1));
    button.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    button.SetVerticalTextAlignment(Text::Alignment::CENTER);
    button.SetBackgroundColor(UiColor(0x1E3A5F));
    button.SetTextColor(UiColor(0xFFFFFF));
    button.SetCornerRadius(5);
    button.SetFocusable(true);
    button.SetStateEffect(OverlayEffect::Plain());
    auto clickable = button.AsInteractive();
    clickable.SetClickable(true);
    clickable.ClickedSignal().Connect(this, [this, action](View, InputEvent)
    { ManualAction(action); });
    if(action != "V" && action != "X") mManualButtons[action] = button;
    return button;
  }
  void AddButtons(StackLayout parent, std::initializer_list<std::pair<const char*, const char*>> items)
  {
    auto row = ButtonRow();
    for(const auto& item : items) row.Add(Button(item.first, item.second, item.second));
    parent.Add(row);
  }
  void DropTarget()
  {
    if(mTarget)
    {
      mTarget.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
      mTarget.StopMarquee();
      mTarget.Unparent();
      mTarget.Reset();
    }
    if(mTargetParent)
    {
      mTargetParent.Unparent();
      mTargetParent.Reset();
    }
  }
  void LoadManualCase()
  {
    DropTarget();
    PinAutoPreview(false);
    mManual            = mManualCases[mManualIndex];
    mManual.async      = mManual.renderScale > 1; // RenderScale is an async-only public option.
    mManualActive      = true;
    mManualDirty       = true;
    mFinished          = false;
    mManualWidth       = std::min(360, std::max(80, mWindow.GetPositionSize().width - 32));
    mManualHeight      = static_cast<int>(mManual.height);
    mManualShort       = false;
    mManualOverflow    = Text::OverflowMode::ELLIPSIS;
    mManualPolicy      = (mManual.action == Action::AUTO_OVERFLOW || mManual.action == Action::AUTO_FIT ||
                     mManual.action == Action::AUTO_SOURCE || mManual.action == Action::AUTO_RESIZE || mManual.autoTrigger)
                           ? Text::MarqueeTriggerPolicy::ON_OVERFLOW
                           : Text::MarqueeTriggerPolicy::MANUAL;
    mManualOrientation = (mManual.multiline != (mManual.action == Action::WRONG_ORIENTATION)) ? Text::MarqueeOrientation::VERTICAL : Text::MarqueeOrientation::HORIZONTAL;
    mManualStop        = mManual.action == Action::FINISH_LOOP || mManual.finishStop ? Text::MarqueeStopMode::FINISH_LOOP : Text::MarqueeStopMode::IMMEDIATE;
    mManualLoops       = mManual.action == Action::LOOP_ONE ? 1 : mManual.action == Action::LOOP_TWO   ? 2
                                                                : mManual.action == Action::LOOP_THREE ? 3
                                                                                                       : 0;
    mManualSpeed       = 80;
    mFixture.RemoveAll();
    mTarget = Label::New(mManual.text.c_str());
    mTarget.SetUiScalePolicy(UiScalePolicy::DISABLED);
    mTarget.SetSystemFontSizeScaleEnabled(false);
    mTarget.SetFontSize(mManual.fontSize);
    mManualWidth = std::max(40, int(mManualWidth * mManual.widthRatio));
    if(mManual.boundary != 0)
    {
      const float natural = mTarget.GetNaturalSize().x;
      const int   width   = mManual.boundary == 2 ? int(natural * 0.7f) : int(std::ceil(natural)) + 4 * mManual.boundary;
      mManualWidth        = std::clamp(width, 40, mManualWidth);
    }
    mTarget.SetRenderScale(mManual.renderScale);
    mTarget.SetTextColor(UiColor(0x000000));
    mTarget.SetBackgroundColor(UiColor(0xFFFFFF));
    mTarget.SetVerticalTextAlignment(Text::Alignment::START);
    mTarget.SetHorizontalTextAlignment(mManual.alignment);
    mTarget.SetLayoutDirection(mManual.rtl ? LayoutDirection::RIGHT_TO_LEFT : LayoutDirection::LEFT_TO_RIGHT);
    mTarget.SetLayoutDirectionMode(Text::LayoutDirectionMode::INHERIT);
    mTarget.SetMultiLine(mManual.multiline);
    if(mMarquee) mManual.maximumLines = 0;
    mTarget.SetMaximumLines(mManual.maximumLines);
    mTarget.SetLineWrapMode(mManual.wrap);
    mTarget.SetPadding(mManual.padding ? Insets(13, 7, 5, 3) : Insets());
    mTarget.SetAsyncRendering(mManual.async);
    mTarget.SetTextOverflowMode(mManualOverflow);
    mTarget.SetMarqueeTriggerPolicy(mManualPolicy);
    mTarget.SetMarqueeOrientation(mManualOrientation);
    mTarget.SetMarqueeStopMode(mManualStop);
    mTarget.SetMarqueeLoopCount(mManualLoops);
    mTarget.SetMarqueeSpeed(mManualSpeed);
    mTarget.SetMarqueeLoopDelay(mManual.delay);
    mTarget.SetMarqueeGap(mManual.gap);
    mTarget.AsyncRenderFinishedSignal().Connect(this, [this](View source, float, float)
    { if(source == mTarget && mManualActive) mManualDirty = true; });
    mFixture.Add(mTarget);
    ManualBounds();
    RefreshManual();
  }
  void ManualBounds()
  {
    SetFixtureSize(mManualWidth + 16, mManualHeight + 16);
    mTarget.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(8, 8, float(mManualWidth), float(mManualHeight))));
    mManualDirty = true;
  }
  void OnWindowResized(Window, Window::WindowSize)
  {
    UpdateResponsiveUi();
    if(mManualActive && !mRunning)
    {
      mManualWidth = std::min(mManualWidth, std::max(80, mWindow.GetPositionSize().width - 32));
      ManualBounds();
    }
  }
  void ManualAction(const std::string& action)
  {
    if(action == "V" && !mRunning)
    {
      StartAutomation();
      return;
    }
    if(action == "V" || action == "X")
    {
      if(!mRunning) return;
      Log("[AUTO][CANCELLED]");
      mRunning  = false;
      mFinished = true;
      mExitCode = 3;
      mTimer.Stop();
      mCapture.Cancel();
      DropTarget();
      PinAutoPreview(false);
      SetBanner("AUTO CANCELLED\nIncomplete run · Reset [Z] for manual", "CANCELLED");
      mDetails.SetText(("CANCELLED: incomplete run; not a PASS.\nReport: " + mReportPath + "\nImages: " + mArtifacts).c_str());
      mDetails.SetAccessibilityValue("CANCELLED");
      UpdateAutoPanels();
      return;
    }
    if(mRunning || mManualButtons.find(action) == mManualButtons.end()) return;
    if(!mManualActive) LoadManualCase();
    if(action == "B" || action == "N" || action == "Z")
    {
      if(action == "B") mManualIndex = (mManualIndex + mManualCases.size() - 1) % mManualCases.size();
      if(action == "N") mManualIndex = (mManualIndex + 1) % mManualCases.size();
      LoadManualCase();
      return;
    }
    if(action == "+" || action == "-")
    {
      mManualWidth = std::clamp(mManualWidth + (action == "+" ? 20 : -20), 40, std::max(80, mWindow.GetPositionSize().width - 32));
      ManualBounds();
    }
    else if(action == "[" || action == "]")
    {
      mManualHeight = std::clamp(mManualHeight + (action == "]" ? 16 : -16), 32, 160);
      ManualBounds();
    }
    else if(action == "A")
    {
      mManual.async = !mManual.async;
      mTarget.SetAsyncRendering(mManual.async);
    }
    else if(action == "E")
    {
      mManualOverflow = mManualOverflow == Text::OverflowMode::ELLIPSIS ? Text::OverflowMode::CLIP : Text::OverflowMode::ELLIPSIS;
      mTarget.SetTextOverflowMode(mManualOverflow);
    }
    else if(action == "D")
    {
      mManual.rtl = !mManual.rtl;
      mTarget.SetLayoutDirection(mManual.rtl ? LayoutDirection::RIGHT_TO_LEFT : LayoutDirection::LEFT_TO_RIGHT);
    }
    else if(action == "H")
    {
      mManual.alignment = static_cast<Text::Alignment>((int(mManual.alignment) + 1) % 3);
      mTarget.SetHorizontalTextAlignment(mManual.alignment);
    }
    else if(action == "M")
    {
      mManual.multiline = !mManual.multiline;
      mTarget.SetMultiLine(mManual.multiline);
    }
    else if(action == "K")
    {
      mManual.maximumLines = (mManual.maximumLines + 1) % 5;
      mTarget.SetMaximumLines(mManual.maximumLines);
    }
    else if(action == "W")
    {
      mManual.wrap = static_cast<Text::LineWrapMode>((int(mManual.wrap) + 1) % 4);
      mTarget.SetLineWrapMode(mManual.wrap);
    }
    else if(action == "P")
    {
      mManual.padding = !mManual.padding;
      mTarget.SetPadding(mManual.padding ? Insets(13, 7, 5, 3) : Insets());
    }
    else if(action == "F")
    {
      mManual.fontSize = mManual.fontSize >= 40 ? 16 : mManual.fontSize + 8;
      mTarget.SetFontSize(mManual.fontSize);
    }
    else if(action == "U")
    {
      mManual.renderScale = mManual.renderScale == 1 ? 2 : 1;
      mTarget.SetRenderScale(mManual.renderScale);
    }
    else if(action == "Y")
    {
      mManualShort = !mManualShort;
      mTarget.SetText(mManualShort ? "Ab" : mManual.text.c_str());
    }
    else if(action == "R")
      mTarget.StartMarquee();
    else if(action == "S")
      mTarget.StopMarquee();
    else if(action == "O")
    {
      mManualOrientation = mManualOrientation == Text::MarqueeOrientation::HORIZONTAL ? Text::MarqueeOrientation::VERTICAL : Text::MarqueeOrientation::HORIZONTAL;
      mTarget.SetMarqueeOrientation(mManualOrientation);
    }
    else if(action == "I")
    {
      mManualStop = mManualStop == Text::MarqueeStopMode::IMMEDIATE ? Text::MarqueeStopMode::FINISH_LOOP : Text::MarqueeStopMode::IMMEDIATE;
      mTarget.SetMarqueeStopMode(mManualStop);
    }
    else if(action == "T")
    {
      mManualPolicy = mManualPolicy == Text::MarqueeTriggerPolicy::MANUAL ? Text::MarqueeTriggerPolicy::ON_OVERFLOW : Text::MarqueeTriggerPolicy::MANUAL;
      mTarget.SetMarqueeTriggerPolicy(mManualPolicy);
    }
    else if(action == "L")
    {
      mManualLoops = (mManualLoops + 1) % 4;
      mTarget.SetMarqueeLoopCount(mManualLoops);
    }
    else if(action == "C")
    {
      mManualSpeed = mManualSpeed >= 200 ? 40 : mManualSpeed + 40;
      mTarget.SetMarqueeSpeed(mManualSpeed);
    }
    else if(action == "G")
    {
      mManual.gap = (mManual.gap + 40) % 200;
      mTarget.SetMarqueeGap(mManual.gap);
    }
    else if(action == "J")
    {
      mManual.delay = mManual.delay >= 2 ? 0 : mManual.delay + 0.5f;
      mTarget.SetMarqueeLoopDelay(mManual.delay);
    }
    mManualDirty = true;
    RefreshManual();
  }
  bool RefreshManual()
  {
    if(mControlsLocked != mRunning)
    {
      mControlsLocked = mRunning;
      for(auto& button : mManualButtons) button.second.SetOpacity(mRunning ? 0.45f : 1.0f);
    }
    if(mAutoUiDirty && !mManualActive) UpdateAutoPanels();
    if(!mManualActive || mRunning || !mTarget) return true;
    const bool running = mTarget.IsMarqueeRunning();
    if(!mManualDirty && running == mManualWasRunning) return true;
    mManualDirty        = false;
    mManualWasRunning   = running;
    const char* align[] = {"START", "CENTER", "END"};
    const char* wrap[]  = {"WORD", "CHARACTER", "HYPHENATION", "MIXED"};
    const auto  set     = [&](const char* key, const std::string& text)
    { auto it = mManualButtons.find(key); if(it != mManualButtons.end()) it->second.SetText(text.c_str()); };
    set("A", mManual.async ? "Async [A]" : "Sync [A]");
    set("E", mManualOverflow == Text::OverflowMode::ELLIPSIS ? "ELLIPSIS [E]" : "CLIP [E]");
    set("D", mManual.rtl ? "RTL [D]" : "LTR [D]");
    set("H", std::string(align[int(mManual.alignment)]) + " [H]");
    set("M", mManual.multiline ? "Multiline ON [M]" : "Multiline OFF [M]");
    set("K", "Max lines: " + std::to_string(mManual.maximumLines) + " [K]");
    set("W", std::string(wrap[int(mManual.wrap)]) + " [W]");
    set("P", mManual.padding ? "Padding ON [P]" : "Padding OFF [P]");
    set("F", "Font: " + std::to_string(int(mManual.fontSize)) + " [F]");
    set("U", "Scale: " + std::to_string(int(mManual.renderScale)) + (mManual.async ? " [U]" : " (async) [U]"));
    set("O", mManualOrientation == Text::MarqueeOrientation::HORIZONTAL ? "Horizontal [O]" : "Vertical [O]");
    set("I", mManualStop == Text::MarqueeStopMode::IMMEDIATE ? "IMMEDIATE [I]" : "FINISH_LOOP [I]");
    set("T", mManualPolicy == Text::MarqueeTriggerPolicy::MANUAL ? "MANUAL [T]" : "ON_OVERFLOW [T]");
    set("L", "Loops: " + (mManualLoops ? std::to_string(mManualLoops) : "infinite") + " [L]");
    set("C", "Speed: " + std::to_string(mManualSpeed) + " [C]");
    set("G", "Gap: " + std::to_string(mManual.gap) + " [G]");
    std::ostringstream delay;
    delay << "Delay: " << mManual.delay << " [J]";
    set("J", delay.str());
    mProgress.SetText((std::string("MANUAL ") + (mManual.async ? "ASYNC" : "SYNC") + "   CASE " +
                       std::to_string(mManualIndex + 1) + " / " + std::to_string(mManualCases.size()) +
                       "\nPublic API · interactive preview")
                        .c_str());
    mScenario.SetText(("[" + std::to_string(mManualIndex + 1) + "] " + mManual.name).c_str());
    SetBanner(std::string("MANUAL · ") + (mManualOverflow == Text::OverflowMode::ELLIPSIS ? "ELLIPSIS" : "CLIP") +
                " · " + (mManual.rtl ? "RTL" : "LTR") + "\n" + std::to_string(mManualWidth) + " x " + std::to_string(mManualHeight) +
                (mMarquee ? (running ? " · Marquee RUNNING" : " · Marquee STOPPED") : (mManual.multiline ? " · Multiline" : " · Single line")),
              "MANUAL");
    const auto         size = mTarget.GetNaturalSize();
    std::ostringstream metrics;
    metrics << "MEASUREMENTS\nLabel: " << mManualWidth << " x " << mManualHeight << "   Natural: " << size.x << " x " << size.y
            << "\nHeightForWidth: " << mTarget.GetHeightForWidth(float(mManualWidth)) << "   Lines: " << (mManual.async ? mTarget.GetAsyncLineCount() : mTarget.GetLineCount());
    mManualMetrics.SetText(metrics.str().c_str());
    std::ostringstream settings;
    settings << "CURRENT SETTINGS\n"
             << (mManual.async ? "ASYNC" : "SYNC") << " · " << (mManual.rtl ? "RTL" : "LTR")
             << " · " << align[int(mManual.alignment)] << "\nMax lines: " << mManual.maximumLines << " · Wrap: " << wrap[int(mManual.wrap)]
             << "\nFont: " << mManual.fontSize << " · RenderScale: " << mManual.renderScale << " · Padding: " << (mManual.padding ? "ON" : "OFF");
    if(mMarquee) settings << "\n"
                          << (mManualPolicy == Text::MarqueeTriggerPolicy::MANUAL ? "MANUAL" : "ON_OVERFLOW")
                          << " · " << (mManualStop == Text::MarqueeStopMode::IMMEDIATE ? "IMMEDIATE" : "FINISH_LOOP")
                          << "\nLoops: " << mManualLoops << " (0 = infinite) · Speed: " << mManualSpeed
                          << "\nGap: " << mManual.gap << " · Delay: " << mManual.delay;
    mSettings.SetText(settings.str().c_str());
    mRuntime.SetText("RUN CONTROL\n[V] Run all Sync + Async cases\n[B / N] Previous / Next   [Z] Reset\nOptions update the same Label; case navigation creates a fresh Label.");
    return true;
  }

  Label Chrome(const char* text, const char* suffix, float size = 15, uint32_t color = 0xE2E8F0, uint32_t background = 0x172033)
  {
    auto label = Label::New(text);
    label.SetFontSize(size);
    label.SetTextColor(UiColor(color));
    label.SetBackgroundColor(UiColor(background));
    label.SetMultiLine(true);
    label.SetLineWrapMode(Text::LineWrapMode::WORD);
    label.SetTextOverflowMode(Text::OverflowMode::CLIP);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(WRAP_CONTENT);
    label.SetPadding(Insets(8, 8, 6, 6));
    label.SetAutomationId((std::string(GetId()) + "-" + suffix).c_str());
    return label;
  }
  void AddMarker(int x, int y, UiColor color)
  {
    auto marker = View::New();
    marker.SetBackgroundColor(color);
    marker.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(float(x), float(y), 4, 4)));
    mFixture.Add(marker);
  }
  IC::Rect Roi() const
  {
    return {8, 8, mWidth, 96};
  }
  const Case& Current() const
  {
    return mCases[mIndex];
  }
  std::string CaseId() const
  {
    return (Current().async ? "async/" : "sync/") + Current().name;
  }
  std::string FileId() const
  {
    return (Current().async ? "async-" : "sync-") + Current().name;
  }
  void Log(const std::string& line)
  {
    const std::string message = std::string(mMarquee ? "[TEXT_MARQUEE]" : "[TEXT_ELLIPSIS]") + line;
    std::printf("%s\n", message.c_str());
    std::fflush(stdout);
    if(mReport.IsOpen())
    {
      if(!mReport.Write(message)) mReportFailed = true;
    }
  }
  bool Save(const IC::Image& image, const std::string& name)
  {
    std::string error;
    if(!IC::SavePng(image, mArtifacts + "/" + name + ".png", error))
    {
      Error("artifact write failed: " + error);
      return false;
    }
    return true;
  }
  void Check(bool condition, const std::string& name, const std::string& observed)
  {
    ++mChecks;
    mAutoUiDirty = true;
    Log(std::string(condition ? "[CHECK][PASS] " : "[CHECK][FAIL] ") + CaseId() + " " + name + " " + observed);
    if(!condition)
    {
      ++mFailedChecks;
      mCaseFailed = true;
      if(mFailures.size() < 12) mFailures.push_back(CaseId() + " " + name + ": " + observed);
      if(mLast.IsValid()) Save(mLast, FileId() + "-failure-" + std::to_string(mChecks));
    }
  }
  void Compare(const IC::Image& expected, const IC::Image& actual, const char* name, IC::Rect region = {})
  {
    if(region.width == 0) region = Roi();
    auto result = IC::Compare(expected, actual, region);
    if(!result.valid)
    {
      Error(std::string(name) + ": " + result.error);
      return;
    }
    const bool pass = result.equal;
    Check(pass, name, "expected=same " + result.Describe() + " roi=" + std::to_string(region.x) + "," + std::to_string(region.y) + "," + std::to_string(region.width) + "," + std::to_string(region.height));
    if(!pass && mRunning)
    {
      Save(expected, FileId() + "-" + name + "-expected");
      Save(actual, FileId() + "-" + name + "-actual");
      Save(IC::Difference(expected, actual), FileId() + "-" + name + "-diff");
    }
  }
  void Error(const std::string& error)
  {
    mExitCode     = 2;
    mFinished     = true;
    mRunning      = false;
    mManualActive = false;
    if(mTimer) mTimer.Stop();
    mCapture.Cancel();
    PinAutoPreview(false);
    Log("[AUTO][ERROR] " + error);
    if(mStatus)
    {
      SetBanner("AUTO VERIFY ERROR\nVerification incomplete · see result card", "ERROR");
    }
    if(mDetails)
    {
      mDetails.SetText(("No PASS: verification incomplete.\n" + error + "\nReport: " + mReportPath + "\nImages: " + mArtifacts).c_str());
      mDetails.SetAccessibilityValue("ERROR");
    }
    UpdateAutoPanels();
  }
  void SetStep(Step step, bool freshAsync = false)
  {
    mStep        = step;
    mStepAt      = Clock::now();
    mNotBefore   = mStepAt + std::chrono::milliseconds(240);
    mStable      = {};
    mStableCount = 0;
    // Overflow changes submit a new async rasterization. Two identical old
    // snapshots alone must not be mistaken for completion on a slow target.
    mRequiredAsync = mAsyncCompletions + ((freshAsync || step == Step::CLIP || step == Step::RESTORE) ? 1 : 0);
    UpdateAutoPanels();
  }
  void BeginCase()
  {
    if(!mRunning) return;
    if(mIndex == mCases.size())
    {
      Finish();
      return;
    }
    DropTarget();
    mCaseFailed         = false;
    mAsyncCompletions   = 0;
    mRestarted          = false;
    mLifecycle          = 0;
    mAutoCycles         = 0;
    mPendingWidthRestore = false;
    mIconifyDone         = false;
    mWindowMinimized    = false;
    const auto& c       = Current();
    auto        measure = Label::New(c.text.c_str());
    measure.SetFontSize(c.fontSize);
    measure.SetUiScalePolicy(UiScalePolicy::DISABLED);
    measure.SetSystemFontSizeScaleEnabled(false);
    measure.SetMultiLine(c.multiline);
    measure.SetLineWrapMode(c.wrap);
    mTargetWidth             = std::floor(mWidth * c.widthRatio);
    const float naturalWidth = measure.GetNaturalSize().x;
    mFitWidth                = std::ceil(naturalWidth) + 4;
    if(c.boundary == -1 || c.boundary == 1) mTargetWidth = std::ceil(naturalWidth) + 4 * c.boundary;
    if(c.boundary == 2) mTargetWidth = std::floor(naturalWidth * 0.7f);
    if(mTargetWidth < 40 || mTargetWidth > mWidth || (c.boundary == 2 && mFitWidth > mWidth))
    {
      Error("font measurement does not fit capture fixture: " + CaseId());
      return;
    }
    const float availableWidth = mTargetWidth - (c.padding ? 20 : 0);
    const float length         = c.multiline ? measure.GetHeightForWidth(availableWidth) : naturalWidth;
    const bool  overflows      = c.multiline ? (length > c.height - (c.padding ? 8 : 0) ||
                                          (!mMarquee && c.maximumLines > 0 && measure.GetLineCount(availableWidth) > c.maximumLines))
                                             : length > availableWidth;
    if(overflows == c.fits)
    {
      Error("font/layout does not satisfy fixture overflow precondition: " + CaseId());
      return;
    }
    // Measurement uses a separate Label; do not perturb the rendered subject's
    // layout cache by querying GetLineCount before testing property changes.
    if(c.action == Action::AUTO_SOURCE) measure.SetText(c.multiline ? LINES : LATIN);
    const float motionLength = c.action == Action::AUTO_SOURCE ? (c.multiline ? measure.GetHeightForWidth(availableWidth) : measure.GetNaturalSize().x) : length;
    mSpeed                   = std::max(20, int((motionLength + c.gap) / 3.0f));
    mMinimumCycle            = (motionLength + c.gap) / mSpeed;
    Log("[CASE][BEGIN] " + CaseId() + " direction=" + (c.rtl ? "RTL" : "LTR") + " speed=" + std::to_string(mSpeed) +
        " viewport=" + std::to_string(mTargetWidth) + "x" + std::to_string(c.height) +
        " font=" + std::to_string(c.fontSize) + " renderScale=" + std::to_string(c.renderScale));
    CreateTarget(Text::OverflowMode::CLIP, mTargetWidth);
    SetStep(Step::CLIP_REFERENCE, true);
  }
  void CreateTarget(Text::OverflowMode overflow, float width)
  {
    DropTarget();
    const auto& c = Current();
    mTarget = Label::New(c.text.c_str());
    mTarget.SetUiScalePolicy(UiScalePolicy::DISABLED);
    mTarget.SetSystemFontSizeScaleEnabled(false);
    mTarget.SetRenderScale(c.renderScale);
    mTarget.SetFontSize(c.fontSize);
    mTarget.SetTextColor(UiColor(0x000000));
    mTarget.SetBackgroundColor(UiColor(0xFFFFFF));
    mTarget.SetHorizontalTextAlignment(c.alignment);
    mTarget.SetVerticalTextAlignment(Text::Alignment::START);
    mTarget.SetLayoutDirection(c.rtl ? LayoutDirection::RIGHT_TO_LEFT : LayoutDirection::LEFT_TO_RIGHT);
    mTarget.SetLayoutDirectionMode(Text::LayoutDirectionMode::INHERIT);
    mTarget.SetMultiLine(c.multiline);
    mTarget.SetMaximumLines(c.multiline && !mMarquee ? c.maximumLines : 0);
    mTarget.SetLineWrapMode(c.wrap);
    mTarget.SetTextOverflowMode(overflow);
    mTarget.SetPadding(c.padding ? Insets(13, 7, 5, 3) : Insets());
    mTarget.SetClippingMode(ClippingMode::CLIP_CHILDREN);
    SetTargetBounds(width);
    mTarget.SetAsyncRendering(c.async);
    mTarget.AsyncRenderFinishedSignal().Connect(this, [this](View source, float, float)
    { if(source == mTarget) ++mAsyncCompletions; });
    mTarget.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::MANUAL);
    const bool vertical = c.multiline != (c.action == Action::WRONG_ORIENTATION);
    mTarget.SetMarqueeOrientation(vertical ? Text::MarqueeOrientation::VERTICAL : Text::MarqueeOrientation::HORIZONTAL);
    mTarget.SetMarqueeLoopDelay(c.delay);
    mTarget.SetMarqueeGap(c.gap);
    mTarget.SetMarqueeLoopCount(ExpectedLoops());
    mTarget.SetMarqueeStopMode(c.finishStop ? Text::MarqueeStopMode::FINISH_LOOP : Text::MarqueeStopMode::IMMEDIATE);
    mTarget.SetMarqueeSpeed(mSpeed);
    mTargetParent = AbsoluteLayout::New();
    mTargetParent.SetUiScalePolicy(UiScalePolicy::DISABLED);
    mTargetParent.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(0, 0, mWidth + 16, 112)));
    mTargetParent.Add(mTarget);
    mFixture.Add(mTargetParent);
  }
  void SetTargetBounds(float width)
  {
    mTarget.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(8, 8, width, Current().height)));
  }
  int ExpectedLoops() const
  {
    switch(Current().action)
    {
      case Action::LOOP_ONE:
        return 1;
      case Action::LOOP_TWO:
        return 2;
      case Action::LOOP_THREE:
        return 3;
      default:
        return Text::MARQUEE_LOOP_COUNT_INFINITE;
    }
  }
  void ArmMotion(Step step, bool freshAsync = false)
  {
    mMotionAt        = Clock::now();
    mStopRequested   = false;
    mDepartures      = 0;
    mInCycle         = false;
    mAtOriginFrames  = 0;
    mMotionSamples   = 0;
    mAfterStopMotion = 0;
    mPrevious        = {};
    mPreviousAt      = {};
    SetStep(step, freshAsync);
  }
  void EndCase()
  {
    if(!mRunning) return;
    if(mCaseFailed) ++mFailedCases;
    ++mCompleted;
    Log(std::string(mCaseFailed ? "[CASE][FAIL] " : "[CASE][PASS] ") + CaseId());
    ++mIndex;
    BeginCase();
  }
  void Finish()
  {
    mRunning  = false;
    mFinished = true;
    mExitCode = mFailedChecks ? 1 : 0;
    mTimer.Stop();
    PinAutoPreview(false);
    const std::string verdict = mFailedChecks ? "FAIL" : "PASS";
    const auto        perPath = std::to_string(mCases.size() / 2);
    Log("[SUMMARY] cases=" + std::to_string(mCompleted) + " syncCases=" + perPath + " asyncCases=" + perPath + " checks=" + std::to_string(mChecks) +
        " fail=" + std::to_string(mFailedChecks) + " failedCases=" + std::to_string(mFailedCases));
    Log("[AUTO][" + verdict + "]");
    if(mReportFailed)
    {
      Error("report write/flush failed; result could not be saved");
      return;
    }
    SetBanner("AUTO VERIFY " + verdict + "\n" + std::to_string(mCompleted) + " cases · " + std::to_string(mFailedCases) + " failed cases", verdict.c_str());
    std::string details = "Sync: " + perPath + " cases | Async: " + perPath + " cases\nReport: " + mReportPath + "\nImages: " + mArtifacts;
    for(const auto& failure : mFailures) details += "\n" + failure;
    mDetails.SetText(details.c_str());
    mDetails.SetAccessibilityValue(verdict.c_str());
    UpdateAutoPanels();
  }

  bool ObserveWindowMinimized()
  {
    if(!mWindowMinimized && mWindow.IsMinimized())
    {
      mWindowMinimized = true;
      Log("[WINDOW] minimized; requesting public Window.Minimize(false)");
      mWindow.Minimize(false);
      return true;
    }
    return false;
  }

  bool Tick()
  {
    if(!mRunning) return false;
    if(mStep == Step::WINDOW_CYCLE)
    {
      if(ObserveWindowMinimized()) return mRunning;
      if(mWindowMinimized && !mWindow.IsMinimized() && mWindow.IsVisible())
      {
        Check(true, "window-iconify-restored", "observed minimized then visible");
        ArmMotion(Step::START);
      }
      else if(Seconds(mStepAt) > 8.0)
      {
        mWindow.Minimize(false);
        Error("Window minimize/restore not observed: backend/WM support required; not a PASS. Restore window externally if suspended.");
      }
      return mRunning;
    }
    if(mReportFailed)
    {
      Error("report write/flush failed");
      return false;
    }
    const auto window = mWindow.GetPositionSize();
    if(window.width != mWindowWidth || window.height != mWindowHeight)
    {
      Error("window resized during run; rerun at the new size");
      return false;
    }
    const double timeout = mStep == Step::OBSERVE ? 35.0 : 8.0;
    if(Seconds(mStepAt) > timeout)
    {
      if(mCapture.IsPending())
        Error("capture timeout");
      else if(mStep == Step::CALIBRATION || (Current().async && mAsyncCompletions < mRequiredAsync))
        Error("render readiness/calibration timeout: expectedCompletions=" + std::to_string(mRequiredAsync) + " observed=" + std::to_string(mAsyncCompletions));
      else
      {
        Check(false, "phase-timeout", "phase=" + std::to_string(int(mStep)));
        EndCase();
      }
      return mRunning;
    }
    TextManualTest::ImageCapture::Result result;
    if(mCapture.Take(result))
    {
      if(!result.Ok())
      {
        Error(result.error);
        return false;
      }
      mLast = std::move(result.image);
      if(!MarkersValid(mLast))
      {
        Save(mLast, "calibration-error");
        Error("capture coordinates, orientation, colors or geometry differ from fixture");
        return false;
      }
      if(mStep == Step::OBSERVE)
        Observe(result);
      else
      {
        // Stable snapshots are a readiness condition, NOT a correctness oracle.
        IC::CompareOptions options;
        options.minimumInkPixels = mStep == Step::CALIBRATION || mStep == Step::LIFECYCLE_HIDDEN ? 0 : 12;
        const auto same          = IC::Compare(mStable, mLast, Roi(), options);
        mStableCount             = same.valid && same.equal ? mStableCount + 1 : 1;
        mStable                  = mLast;
        if(mStableCount >= 2) Evaluate();
      }
      mNotBefore = std::max(mNotBefore, Clock::now() + std::chrono::milliseconds(80));
    }
    if(!mRunning) return false;
    if(!mCapture.IsPending() && Clock::now() >= mNotBefore &&
       (mStep == Step::CALIBRATION || !Current().async || mAsyncCompletions >= mRequiredAsync))
    {
      const auto size = mFixture.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
      if(std::abs(size.x - (mWidth + 16)) > 0.1f || std::abs(size.y - 112) > 0.1f) return true;
      std::string error;
      if(!mCapture.Request(mFixture, mFixture.GetScreenPosition(), Vector2(float(mWidth + 16), 112), error)) Error(error);
    }
    return mRunning;
  }
  bool MarkersValid(const IC::Image& image) const
  {
    return IC::CountColor(image, {2, 2, 4, 4}, {255, 0, 0, 255}) == 16 &&
           IC::CountColor(image, {mWidth + 10, 106, 4, 4}, {0, 0, 255, 255}) == 16;
  }
  void Evaluate()
  {
    if(mStep == Step::LIFECYCLE_HIDDEN)
    {
      if(Seconds(mStepAt) < 0.6) return;
      Check(IC::CountInk(mLast, Roi()) == 0, "hidden-no-foreground", "expected=empty");
      Check(!mTarget.IsMarqueeRunning(), "hidden-stopped", "expected=stopped");
      // Exercise queued source changes while hidden; the latest source wins.
      mTarget.SetText("temporary hidden source");
      mTarget.SetText(Current().text.c_str());
      mTargetParent.SetProperty(Actor::Property::VISIBLE, true);
      mTarget.SetProperty(Actor::Property::VISIBLE, true);
      ArmMotion(Step::START, true);
      return;
    }
    if(mStep == Step::CALIBRATION)
    {
      if(IC::CountInk(mLast, Roi()) != 0)
      {
        Error("calibration viewport is not opaque white");
        return;
      }
      Save(mLast, "calibration");
      Log("[CALIBRATION][PASS] red top-left / blue bottom-right / opaque white viewport");
      BeginCase();
      return;
    }
    if(IC::CountInk(mLast, Roi()) < 12)
    {
      Error("insufficient text pixels");
      return;
    }
    if(Current().async && (mStep == Step::CLIP || mStep == Step::RESTORE))
      Check(mAsyncCompletions >= mRequiredAsync, "async-generation-ready", "required=" + std::to_string(mRequiredAsync) + " observed=" + std::to_string(mAsyncCompletions));
    switch(mStep)
    {
      case Step::CLIP_REFERENCE:
        mClipReference = mLast;
        Save(mLast, FileId() + "-fresh-clip");
        mTarget.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);
        SetStep(Step::REFERENCE_TO_ELLIPSIS, true);
        break;
      case Step::REFERENCE_TO_ELLIPSIS:
        mClipToEllipsis = mLast;
        Save(mLast, FileId() + "-clip-to-ellipsis");
        CreateTarget(Text::OverflowMode::ELLIPSIS, Current().action == Action::AUTO_RESIZE ? mFitWidth : mTargetWidth);
        SetStep(Current().action == Action::AUTO_RESIZE ? Step::FIT_REFERENCE : Step::BASELINE, true);
        break;
      case Step::FIT_REFERENCE:
        mFitReference = mLast;
        Save(mLast, FileId() + "-fitting-reference");
        SetTargetBounds(mTargetWidth);
        SetStep(Step::BASELINE, true);
        break;
      case Step::BASELINE:
      {
        Check(!mTarget.IsMarqueeRunning(), "manual-idle", "expected=stopped observed=" + std::to_string(mTarget.IsMarqueeRunning()));
        Check(!Current().async || mAsyncCompletions > 0, "render-path-ready", "asyncCompletions=" + std::to_string(mAsyncCompletions));
        mBaseline = mLast;
        Save(mBaseline, FileId() + "-ellipsis");
        Compare(mBaseline, mClipToEllipsis, "clip-to-ellipsis-reference");
        if(Current().async && Current().renderScale == 1)
        {
          const auto baseline = mBaselines.find(Current().name);
          if(baseline == mBaselines.end())
          {
            Error("sync baseline unavailable; parity cannot be evaluated");
            return;
          }
          Compare(baseline->second, mBaseline, "sync-async-parity");
        }
        else if(!Current().async)
          mBaselines[Current().name] = mBaseline;
        else
          Log("[INFO] " + CaseId() + " sync/async exact pixel parity is inapplicable: RenderScale is async-only; same-scale restoration remains checked");
        mTarget.SetTextOverflowMode(Text::OverflowMode::CLIP);
        SetStep(Step::CLIP);
        break;
      }
      case Step::CLIP:
        mClip = mLast;
        Save(mClip, FileId() + "-clip");
        // Overflow alone does not prove that visible pixels must change (e.g.
        // an empty trailing line or a font-dependent elision boundary). The
        // oracle is an independently created CLIP Label at the SAME size/path.
        Compare(mClipReference, mClip, "ellipsis-to-clip-reference");
        if(Current().fits || Current().invisibleOverflow)
          Compare(mBaseline, mClip, "fitting-overflow-unchanged");
        else
        {
          const auto effect = IC::Compare(mBaseline, mClipReference, Roi());
          Log("[INFO] " + CaseId() + " fresh-clip-vs-ellipsis " + effect.Describe());
        }
        mTarget.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);
        SetStep(Step::RESTORE);
        break;
      case Step::RESTORE:
        Compare(mBaseline, mLast, "overflow-restore");
        if(!mMarquee)
        {
          if(Current().multiline && Current().maximumLines > 0)
          {
            const int lines = Current().async ? mTarget.GetAsyncLineCount() : mTarget.GetLineCount();
            Check(lines > 0 && lines <= Current().maximumLines, "maximum-lines", "limit=" + std::to_string(Current().maximumLines) + " observed=" + std::to_string(lines));
          }
          EndCase();
          break;
        }
        if(Current().action == Action::AUTO_OVERFLOW || Current().action == Action::AUTO_FIT ||
           Current().action == Action::AUTO_SOURCE || Current().action == Action::AUTO_RESIZE ||
           Current().autoTrigger)
          mTarget.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::ON_OVERFLOW);
        else
          mTarget.StartMarquee();
        ArmMotion(Current().action == Action::AUTO_FIT || Current().action == Action::AUTO_SOURCE || Current().action == Action::WRONG_ORIENTATION ? Step::FIT : Step::START);
        break;
      case Step::START:
        Check(mTarget.IsMarqueeRunning(), "marquee-start", "expected=running observed=" + std::to_string(mTarget.IsMarqueeRunning()));
        // New glyphs replace the ellipsis at start. Compare only the retained
        // interior, not the changed elision boundary. This also handles CENTER
        // and END without assuming their CLIP origin equals the marquee origin.
        if(Current().action != Action::AUTO_SOURCE && Current().rigidOrigin &&
           !(Current().action == Action::LIFECYCLE && mLifecycle == 1))
        {
          auto retained = Roi();
          if(Current().multiline)
            retained.height = 32; // first complete line
          else
          {
            retained.width = int(mTargetWidth) * 2 / 3;
            if(Current().rtl) retained.x += int(mTargetWidth) - retained.width;
          }
          Compare(mBaseline, mLast, "marquee-retained-origin", retained);
        }
        else
          Log("[NOT_APPLICABLE] " + CaseId() + " marquee-retained-origin: " +
              (Current().action == Action::AUTO_SOURCE ? "source changed" :
               Current().action == Action::LIFECYCLE && mLifecycle == 1 ? "viewport changed" :
               "mixed BiDi elision can reorder retained runs; no rigid pixel-origin contract") +
              "; start/motion/direction/stop/restoration are still checked");
        mOrigin = mLast;
        mMotionCompletions = mAsyncCompletions;
        Save(mOrigin, FileId() + "-marquee-origin");
        SetStep(Step::OBSERVE);
        break;
      case Step::FIT:
        if(Seconds(mMotionAt) < 1.4) break;
        Check(!mTarget.IsMarqueeRunning(), "start-condition-idle", "expected=stopped observed=" + std::to_string(mTarget.IsMarqueeRunning()));
        Compare(mBaseline, mLast, "fits-still-unchanged");
        if(Current().action == Action::AUTO_SOURCE)
        {
          mTarget.SetText(Current().multiline ? LINES : LATIN); // No StartMarquee: policy must react to source growth.
          ArmMotion(Step::START, true);
        }
        else
          EndCase();
        break;
      case Step::AUTO_RETURN:
        if(Seconds(mStepAt) < 1.4) break;
        Check(!mTarget.IsMarqueeRunning(), "on-overflow-return-idle", "expected=stopped observed=" + std::to_string(mTarget.IsMarqueeRunning()));
        Compare(Current().action == Action::AUTO_RESIZE ? mFitReference : mBaseline, mLast, "on-overflow-return-restore");
        Save(mLast, FileId() + "-return-to-fit");
        Log("[INFO] " + CaseId() + " fit-overflow-fit cycle=" + std::to_string(++mAutoCycles));
        if(mAutoCycles < 3)
        {
          // Content-driven retirement must not suppress the next automatic run.
          if(Current().action == Action::AUTO_SOURCE)
            mTarget.SetText(Current().multiline ? LINES : LATIN);
          else
            SetTargetBounds(mTargetWidth);
          ArmMotion(Step::START, true);
        }
        else
          EndCase();
        break;
      case Step::STOP_RESTORE:
        Check(!mTarget.IsMarqueeRunning(), "stopped", "expected=stopped observed=" + std::to_string(mTarget.IsMarqueeRunning()));
        Compare(mBaseline, mLast, "marquee-stop-restore");
        Save(mLast, FileId() + "-restored");
        if(Current().action == Action::LIFECYCLE || Current().autoTrigger)
        {
          SetStep(Step::LIFECYCLE_IDLE);
        }
        else if(Current().action == Action::RESTART && !mRestarted)
        {
          mRestarted = true;
          mTarget.StartMarquee();
          ArmMotion(Step::START);
        }
        else
          EndCase();
        break;
      case Step::LIFECYCLE_IDLE:
        if(Seconds(mStepAt) < 1.4) break;
        Check(!mTarget.IsMarqueeRunning(), "no-unrequested-restart", "expected=stopped");
        Compare(mBaseline, mLast, "idle-ellipsis");
        if(mLifecycle == 5)
        {
          mTarget.StartMarquee();
          mTarget.StartMarquee(); // idempotent while already requested
          ArmMotion(Step::START);
        }
        else
          EndCase();
        break;
      default:
        break;
    }
  }

  void Observe(const TextManualTest::ImageCapture::Result& capture)
  {
    // Capture timestamps bound observation uncertainty. Missing an entire
    // delay/cycle must be reported as ERROR, not counted as a passing loop.
    if(Seconds(capture.requested, capture.completed) > 0.5 ||
       (mPrevious.IsValid() && Seconds(mPreviousAt, capture.completed) > 0.65))
    {
      Error("capture cadence insufficient to observe marquee cycles (need <650ms gaps, <500ms readback)");
      return;
    }
    const bool   running = mTarget.IsMarqueeRunning();
    const double elapsed = Seconds(mMotionAt);
    const auto&  c       = Current();
    if(!running)
    {
      Check(mMotionSamples >= 2, "actual-motion", "expected>=2 translated samples observed=" + std::to_string(mMotionSamples));
      if(ExpectedLoops() > 0)
      {
        const int expected = ExpectedLoops();
        Check(mDepartures == expected, "observed-cycles", "expected=" + std::to_string(expected) + " departures=" + std::to_string(mDepartures));
        Check(elapsed >= expected * mMinimumCycle * 0.8, "not-premature-loop-stop", "elapsed=" + std::to_string(elapsed) + " min=" + std::to_string(expected * mMinimumCycle * 0.8));
      }
      else
        Check(mStopRequested, "explicit-stop-only", "expected=stop-requested observed=" + std::to_string(mStopRequested));
      if(c.action == Action::FINISH_LOOP || (c.action == Action::LIFECYCLE && c.finishStop))
        Check(mAfterStopMotion >= 2, "finish-loop-continued-motion", "expected>=2 post-request moving samples observed=" + std::to_string(mAfterStopMotion));
      SetStep(Step::STOP_RESTORE);
      return;
    }
    const auto origin = IC::Compare(mOrigin, mLast, Roi());
    if(!origin.valid)
    {
      Error(origin.error);
      return;
    }
    if(origin.equal)
    {
      if(++mAtOriginFrames >= 2) mInCycle = false;
    }
    else if(origin.changedFraction > 0.10)
    {
      mAtOriginFrames = 0;
      if(!mInCycle)
      {
        ++mDepartures;
        mInCycle = true;
        Log("[MOTION] " + CaseId() + " departure=" + std::to_string(mDepartures) + " elapsed=" + std::to_string(elapsed));
      }
    }
    if(mPrevious.IsValid())
    {
      const int  range    = std::max(8, int(std::ceil(mSpeed * (Seconds(mPreviousAt, capture.completed) + 0.08))) + 4);
      const int  maximumX = c.multiline ? 0 : std::min(range, mWidth / 3);
      const int  maximumY = c.multiline ? std::min(range, 32) : 0;
      const auto shift    = IC::EstimateTranslation(mPrevious, mLast, Roi(), maximumX, maximumY);
      // A best match on the search boundary could hide an even larger shift.
      const bool insideSearch = c.multiline ? std::abs(shift.dy) < maximumY : std::abs(shift.dx) < maximumX;
      if(shift.valid && insideSearch && (c.multiline ? std::abs(shift.dy) >= 2 : std::abs(shift.dx) >= 2))
      {
        ++mMotionSamples;
        if(mStopRequested && capture.requested > mStopAt) ++mAfterStopMotion;
        Log("[MOTION] " + CaseId() + " dx=" + std::to_string(shift.dx) + " dy=" + std::to_string(shift.dy) + " mismatch=" + std::to_string(shift.mismatch));
        if(mMotionSamples == 2)
        {
          if(c.action == Action::LIFECYCLE && mLifecycle == 0)
            Check(mAsyncCompletions == mMotionCompletions, "no-render-during-steady-motion", "before=" + std::to_string(mMotionCompletions) + " after=" + std::to_string(mAsyncCompletions));
          Check(c.multiline ? shift.dy < 0 : c.rtl ? shift.dx > 0
                                                   : shift.dx < 0,
                "motion-direction", "dx=" + std::to_string(shift.dx) + " dy=" + std::to_string(shift.dy));
          Save(mLast, FileId() + "-moving");
          if(c.action == Action::LIFECYCLE)
          {
            ++mLifecycle;
            if(mLifecycle == 1 || mLifecycle == 2)
            {
              SetTargetBounds(mLifecycle == 1 ? mTargetWidth * 0.8f : mTargetWidth);
              ArmMotion(Step::START, true);
            }
            else if(mLifecycle == 3 || mLifecycle == 4)
            {
              // Opt-in: X11 currently implements Minimize as a no-op. A target
              // run must observe the actual window state, never simulate it
              // with Label visibility or silently count unsupported as PASS.
              const char* iconify = std::getenv("DALI_TEXT_TEST_ICONIFY");
              if(mLifecycle == 3 && !mIconifyDone && iconify && std::string(iconify) == "1")
              {
                --mLifecycle;
                mIconifyDone = true;
                SetStep(Step::WINDOW_CYCLE);
                mWindow.Minimize(true);
                return;
              }
              // Capture must keep the calibrated fixture visible. The first
              // cycle hides Label; the second uses a separate public parent.
              if(mLifecycle == 3)
                mTarget.SetProperty(Actor::Property::VISIBLE, false);
              else
                mTargetParent.SetProperty(Actor::Property::VISIBLE, false);
              SetStep(Step::LIFECYCLE_HIDDEN);
            }
            else
            {
              mTarget.StopMarquee();
              mStopRequested = true;
              mStopAt = Clock::now();
              if(c.finishStop)
              {
                Check(mTarget.IsMarqueeRunning(), "finish-loop-not-immediate", "expected=running");
                // Resize while FINISH_LOOP defers relayout; no update may be lost.
                SetTargetBounds(mTargetWidth * 0.8f);
                mPendingWidthRestore = true;
              }
            }
            return;
          }
          if(c.action == Action::AUTO_SOURCE || c.action == Action::AUTO_RESIZE)
          {
            if(c.action == Action::AUTO_SOURCE)
              mTarget.SetText(c.text.c_str());
            else
              SetTargetBounds(mFitWidth);
            // No StopMarquee: fitting content must stop automatically. Returning
            // to the cached fitting frame need not produce another async signal.
            // The saved fit reference differs from the moving/overflow source,
            // so a stale frame cannot pass the restoration comparison.
            SetStep(Step::AUTO_RETURN);
            return;
          }
          const bool explicitStop = c.action == Action::IMMEDIATE || c.action == Action::FINISH_LOOP ||
                                    c.action == Action::AUTO_OVERFLOW || c.action == Action::RESTART || c.action == Action::MANUAL_FIT;
          if(explicitStop)
          {
            const bool finish = c.action == Action::FINISH_LOOP;
            mTarget.SetMarqueeStopMode(finish ? Text::MarqueeStopMode::FINISH_LOOP : Text::MarqueeStopMode::IMMEDIATE);
            mTarget.StopMarquee();
            mStopRequested = true;
            mStopAt        = Clock::now();
            if(finish) Check(mTarget.IsMarqueeRunning(), "finish-loop-not-immediate", "expected=running observed=" + std::to_string(mTarget.IsMarqueeRunning()));
          }
        }
      }
    }
    if(mPendingWidthRestore && mAfterStopMotion >= 2)
    {
      // Keep the changed allocation for real frames while FINISH_LOOP is
      // pending, rather than coalescing both widths in the same event turn.
      const auto size = mTarget.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
      Check(std::abs(size.x - mTargetWidth * 0.8f) < 1.0f, "finish-loop-resize-applied", "width=" + std::to_string(size.x));
      SetTargetBounds(mTargetWidth);
      mPendingWidthRestore = false;
    }
    if(c.action == Action::INFINITE_TWO && !mStopRequested && mDepartures >= 2 && mMotionSamples >= 2)
    {
      Check(running, "infinite-still-running", "observedCycles=" + std::to_string(mDepartures));
      mTarget.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
      mTarget.StopMarquee();
      mStopRequested = true;
      mStopAt        = Clock::now();
    }
    mPrevious   = mLast;
    mPreviousAt = capture.completed;
  }

  bool                             mMarquee;
  std::vector<Case>                mManualCases;
  Case                             mManual;
  std::size_t                      mManualIndex{0};
  std::map<std::string, Label>     mManualButtons;
  Timer                            mManualTimer;
  Label                            mManualMetrics;
  int                              mManualWidth{288}, mManualHeight{96}, mManualSpeed{80}, mManualLoops{0};
  bool                             mManualActive{false}, mManualDirty{false}, mManualShort{false}, mManualWasRunning{false}, mControlsLocked{false};
  Text::OverflowMode               mManualOverflow{Text::OverflowMode::ELLIPSIS};
  Text::MarqueeTriggerPolicy       mManualPolicy{Text::MarqueeTriggerPolicy::MANUAL};
  Text::MarqueeOrientation         mManualOrientation{Text::MarqueeOrientation::HORIZONTAL};
  Text::MarqueeStopMode            mManualStop{Text::MarqueeStopMode::IMMEDIATE};
  StackLayout                      mRoot, mBody, mPreview, mInspector, mStage, mFrame;
  ScrollView                       mScroll, mViewport;
  Label                            mRunButton, mScenario, mSettings, mRuntime;
  int                              mFixtureWidth{304}, mFixtureHeight{112};
  bool                             mAutoUiDirty{false}, mAutoPreviewPinned{false};
  AbsoluteLayout                   mFixture;
  AbsoluteLayout                   mTargetParent;
  Label                            mTarget, mStatus, mProgress, mDetails;
  Timer                            mTimer;
  TextManualTest::ImageCapture     mCapture;
  std::string                      mArtifacts;
  std::vector<Case>                mCases;
  std::vector<std::string>         mFailures;
  std::map<std::string, IC::Image> mBaselines;
  IC::Image                        mLast, mStable, mBaseline, mClip, mClipReference, mClipToEllipsis, mOrigin, mPrevious, mFitReference;
  Clock::time_point                mStepAt{}, mNotBefore{}, mMotionAt{}, mPreviousAt{}, mStopAt{};
  Step                             mStep{Step::CALIBRATION};
  std::size_t                      mIndex{0};
  int                              mWidth{288}, mWindowWidth{0}, mWindowHeight{0}, mSpeed{100};
  int                              mStableCount{0}, mAsyncCompletions{0}, mRequiredAsync{0}, mChecks{0}, mFailedChecks{0}, mFailedCases{0}, mCompleted{0};
  int                              mDepartures{0}, mAtOriginFrames{0}, mMotionSamples{0}, mAfterStopMotion{0}, mExitCode{3};
  double                           mMinimumCycle{0};
  float                            mTargetWidth{288}, mFitWidth{288};
  bool                             mRestarted{false};
  int                              mLifecycle{0};
  int                              mAutoCycles{0};
  bool                             mPendingWidthRestore{false};
  int                              mMotionCompletions{0};
  bool                             mIconifyDone{false}, mWindowMinimized{false};
  bool                             mRunning{false}, mFinished{false}, mCaseFailed{false}, mStopRequested{false}, mInCycle{false}, mReportFailed{false};
};
} // namespace

class TcTextEllipsis : public PixelTextCase
{
public:
  TcTextEllipsis()
  : PixelTextCase(false)
  {
  }
};
class TcTextMarquee : public PixelTextCase
{
public:
  TcTextMarquee()
  : PixelTextCase(true)
  {
  }
};
REGISTER_MANUAL_TEST(TcTextEllipsis)
REGISTER_MANUAL_TEST(TcTextMarquee)
