/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include "manual-test-case.h"

#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr uint32_t COLOR_BG              = 0xF6F7F9;
constexpr uint32_t COLOR_TEXT            = 0x263238;
constexpr uint32_t COLOR_SUB_TEXT       = 0x607D8B;
constexpr uint32_t COLOR_NORMAL          = 0xD6E5F0;
constexpr uint32_t COLOR_PRESSED         = 0xFFAB91;
constexpr uint32_t COLOR_DISABLED        = 0xE2E5E7;
constexpr uint32_t COLOR_PSEUDO_DISABLED = 0xD5DBDF;
constexpr uint32_t COLOR_BUTTON          = 0x455A64;
constexpr uint32_t COLOR_BUTTON_ALT      = 0x00796B;
constexpr uint32_t COLOR_WHITE           = 0xFFFFFF;

constexpr float PAD         = 20.0f;
constexpr float GAP         = 12.0f;
constexpr float TARGET_H    = 136.0f;
constexpr float BUTTON_H    = 56.0f;
constexpr float LOG_H       = 220.0f;
constexpr float FONT_TITLE  = 20.0f;
constexpr float FONT_BODY   = 15.0f;
constexpr float FONT_LOG    = 14.0f;
constexpr float FONT_BUTTON = 16.0f;

Dali::String BoolText(bool value)
{
  return value ? "true" : "false";
}
} // namespace

/**
 * @brief Disabled vs pseudo-disabled policy, judged by TAP + state string.
 *
 * Split out of "Interactive Hover State" (review 37): the emulator has no
 * pointer device, so a human cannot perform any hover procedure — but hover
 * was only a PRECONDITION of the old scenario 5.4, never its subject. What
 * the policies actually promise needs no pointer at all:
 *
 *   pseudo-disabled: "visually presented as disabled, but still accepts user
 *   interaction" (interactive-trait.h) — a tap lands ([PseudoDisabled, Focused]).
 *   disabled: refuses input — the same tap changes nothing ([Disabled]).
 *
 * The hover screen itself is untouched so scenario 5.3 can be revived intact
 * when a pointer-equipped target exists.
 *
 * Test steps:
 *   1. Entry -> State: [Normal]
 *   2. [Toggle pseudo-disabled] -> State: [PseudoDisabled], card dims
 *   3. Tap the card -> State: [PseudoDisabled, Focused]  (input ACCEPTED)
 *   4. [Reset] -> [Toggle enabled] -> State: [Disabled], card dims
 *   5. Tap the card -> State: [Disabled] unchanged        (input REFUSED)
 *
 * Expected result:
 *   The same tap is accepted in pseudo-disabled and refused in disabled, and
 *   the three renders (Normal / PseudoDisabled / Disabled) all differ.
 */
class TcDisabledPseudoDisabledPolicy : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "Disabled / PseudoDisabled Policy";
  }

  Dali::String GetDescription() const override
  {
    return "Pseudo-disabled looks disabled but accepts input; disabled refuses it";
  }

  void OnEnter(View contentArea) override
  {
    // Fresh state every visit — the hover screen kept these in members and
    // never reset them, so a target left disabled leaked into the next visit
    // (the F5 trap this screen must not inherit).
    mEnabled        = true;
    mPseudoDisabled = false;
    mLogText        = "Event log:\n";

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetBackgroundColor(UiColor(COLOR_BG));
    root.SetPadding(Extents(PAD, PAD, PAD, PAD));
    root.SetSpacing(GAP);

    Label header = Label::New("Tap the card in each policy state and compare what happens.");
    header.SetMultiLine(true);
    header.SetFontSize(FONT_BODY);
    header.SetTextColor(UiColor(COLOR_SUB_TEXT));
    header.SetRequestedWidth(MATCH_PARENT);
    header.SetRequestedHeight(WRAP_CONTENT);
    root.Add(header);

    root.Add(CreateTarget());
    root.Add(MakeButtonRow());

    mLog = Label::New(mLogText);
    mLog.SetMultiLine(true);
    mLog.SetFontSize(FONT_LOG);
    mLog.SetTextColor(UiColor(COLOR_TEXT));
    mLog.SetRequestedWidth(MATCH_PARENT);
    mLog.SetRequestedHeight(LOG_H);
    mLog.SetBackgroundColor(UiColor(COLOR_WHITE));
    mLog.SetPadding(Extents(12.0f, 12.0f, 10.0f, 10.0f));
    root.Add(mLog);

    UpdateTarget();

    contentArea.Add(root);
  }

private:
  View CreateTarget()
  {
    mTarget = InteractiveView::New();
    mTarget.SetRequestedWidth(MATCH_PARENT);
    mTarget.SetRequestedHeight(TARGET_H);
    mTarget.SetBackgroundColor(UiColor(COLOR_NORMAL));
    mTarget.SetPadding(Extents(16.0f, 16.0f, 12.0f, 12.0f));
    mTarget.SetFocusable(true);

    mTrait = mTarget.AsInteractive();

    mTarget.StateChangedSignal().Connect(this, [this](View, StateEvent event)
    {
      AppendLog("Policy target: state [" + event.GetPrev().ToString() + "] -> [" + event.GetCurrent().ToString() + "]\n");
      UpdateTarget();
    });

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(MATCH_PARENT);
    content.SetSpacing(4.0f);

    Label title = Label::New("Policy target");
    title.SetFontSize(FONT_TITLE);
    title.SetTextColor(UiColor(COLOR_TEXT));
    title.SetRequestedWidth(MATCH_PARENT);
    title.SetRequestedHeight(WRAP_CONTENT);

    mStateLabel = Label::New();
    mStateLabel.SetMultiLine(true);
    mStateLabel.SetFontSize(FONT_BODY);
    mStateLabel.SetTextColor(UiColor(COLOR_TEXT));
    mStateLabel.SetRequestedWidth(MATCH_PARENT);
    mStateLabel.SetRequestedHeight(WRAP_CONTENT);

    content.Add(title);
    content.Add(mStateLabel);
    mTarget.Add(content);

    return mTarget;
  }

  View MakeButtonRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(BUTTON_H);
    row.SetSpacing(GAP);

    row.Add(MakeButton("Reset", COLOR_BUTTON, [this]()
    {
      mEnabled        = true;
      mPseudoDisabled = false;
      mTarget.SetEnabled(true);
      mTrait.SetPseudoDisabled(false);
      AppendLog("-- reset policy target --\n");
      UpdateTarget();
    }));
    row.Add(MakeButton("Toggle enabled", COLOR_BUTTON_ALT, [this]()
    {
      mEnabled = !mEnabled;
      mTarget.SetEnabled(mEnabled);
      AppendLog(Dali::String("-- policy enabled=") + BoolText(mEnabled) + " --\n");
      UpdateTarget();
    }));
    row.Add(MakeButton("Toggle pseudo-disabled", COLOR_BUTTON_ALT, [this]()
    {
      mPseudoDisabled = !mPseudoDisabled;
      mTrait.SetPseudoDisabled(mPseudoDisabled);
      AppendLog(Dali::String("-- policy pseudoDisabled=") + BoolText(mPseudoDisabled) + " --\n");
      UpdateTarget();
    }));

    return row;
  }

  View MakeButton(const Dali::String& text, uint32_t color, std::function<void()> action)
  {
    InteractiveView button = InteractiveView::New();
    button.SetRequestedHeight(MATCH_PARENT);
    button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    button.SetBackgroundColor(UiColor(color));
    button.ClickedSignal().Connect(this, [action](View, InputEvent)
    {
      action();
    });

    Label label = Label::New(text);
    label.SetFontSize(FONT_BUTTON);
    label.SetTextColor(UiColor(COLOR_WHITE));
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    button.Add(label);
    return button;
  }

  void UpdateTarget()
  {
    const ViewState state = mTarget.GetState();

    if(state.Contains(ViewState::DISABLED))
    {
      mTarget.SetBackgroundColor(UiColor(COLOR_DISABLED));
    }
    else if(state.Contains(ViewState::PSEUDO_DISABLED))
    {
      mTarget.SetBackgroundColor(UiColor(COLOR_PSEUDO_DISABLED));
    }
    else if(mTrait.IsPressed())
    {
      mTarget.SetBackgroundColor(UiColor(COLOR_PRESSED));
    }
    else
    {
      mTarget.SetBackgroundColor(UiColor(COLOR_NORMAL));
    }

    Dali::String text = "State: [" + state.ToString() + "]";
    text += "\nIsPressed: " + BoolText(mTrait.IsPressed());
    text += "  Clickable: " + BoolText(mTrait.IsClickable());
    mStateLabel.SetText(text);
  }

  void AppendLog(const Dali::String& text)
  {
    mLogText += text;
    mLog.SetText(mLogText);
  }

  InteractiveView  mTarget;
  InteractiveTrait mTrait;
  Label            mStateLabel;
  Label            mLog;
  Dali::String     mLogText{"Event log:\n"};
  bool             mEnabled{true};
  bool             mPseudoDisabled{false};
};

REGISTER_MANUAL_TEST(TcDisabledPseudoDisabledPolicy)
