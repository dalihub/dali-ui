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

#include "manual-test-case.h"

#include <dali-ui-foundation/public-api/views/image/animated-image-view.h>

#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
const char* const ANIM_WEBP = TEST_RESOURCE_DIR "/dog-anim.webp";

constexpr float    PREVIEW_SIZE  = 200.0f;
constexpr float    BTN_H         = 52.0f;
constexpr float    STATUS_H      = 32.0f;
constexpr uint32_t C_BTN_BG      = 0x555555;
constexpr uint32_t C_BTN_TEXT    = 0xEEEEEE;
constexpr uint32_t C_STATUS_BG   = 0x222222;
constexpr uint32_t C_STATUS_TEXT = 0xCCCCCC;
constexpr uint32_t C_BG          = 0x1A1A1A;

const char* PlayStateName(Ui::AnimatedImage::PlayState state)
{
  switch(state)
  {
    case Ui::AnimatedImage::PlayState::PLAYING: return "PLAYING";
    case Ui::AnimatedImage::PlayState::PAUSED:  return "PAUSED";
    case Ui::AnimatedImage::PlayState::STOPPED: return "STOPPED";
    default:                                    return "?";
  }
}
} // namespace

/**
 * @brief Verifies AnimatedImageView basic playback control:
 *   Play / Pause / Stop / JumpToFrame
 *   GetPlayState / GetCurrentFrame / GetTotalFrame
 *   AnimationFinishedSignal
 *
 * Steps:
 *   1. [Play]   -> State: PLAYING, frame counter increments
 *   2. [Pause]  -> State: PAUSED, frame counter frozen
 *   3. [Play]   -> State: PLAYING, frame counter resumes
 *   4. [Jump->5] -> State: PAUSED, Frame: 5
 *   5. [Stop]   -> State: STOPPED, AnimationFinishedSignal counter increments
 *
 * Expected result:
 *   After each button press the State/Frame label updates immediately with the correct value.
 *   AnimationFinishedSignal is fired on Stop.
 */
class TcAnimatedImageViewPlayControl : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "AnimatedImageView: Play / Pause / Stop / JumpToFrame";
  }

  Dali::String GetDescription() const override
  {
    return "Verify Play/Pause/Stop/JumpToFrame behaviour and AnimationFinishedSignal firing";
  }

  void OnEnter(View contentArea) override
  {
    mFinishedCount = 0;

    mView = AnimatedImageView::New(ANIM_WEBP);
    mView.SetRequestedWidth(PREVIEW_SIZE);
    mView.SetRequestedHeight(PREVIEW_SIZE);

    mView.AnimationFinishedSignal().Connect(this, [this](View) {
      ++mFinishedCount;
      mFinishedLabel.SetText(
        Dali::String("AnimationFinishedSignal: ") +
        Dali::String(std::to_string(mFinishedCount).c_str()));
    });

    mStatusLabel   = MakeStatusLabel("State: STOPPED | Frame: 0/0");
    mFinishedLabel = MakeStatusLabel("AnimationFinishedSignal: 0");

    mPollTimer = Timer::New(100);
    mPollTimer.Start();

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetBackgroundColor(UiColor(C_BG));
    content.SetPadding(Insets(8.0f, 8.0f, 8.0f, 8.0f));

    content.Add(MakeCentered(mView));
    content.Add(mStatusLabel);
    content.Add(mFinishedLabel);
    content.Add(MakeButtonRow({
      MakeButton("Play",    [this] { mView.Play(); }),
      MakeButton("Pause",   [this] { mView.Pause(); }),
      MakeButton("Stop",    [this] { mView.Stop(); }),
      MakeButton("Jump\n→5", [this] { mView.JumpToFrame(5); }),
    }));

    contentArea.Add(content);
  }

  void OnExit() override
  {
    if(mPollTimer)
    {
      mPollTimer.Stop();
      mPollTimer.Reset();
    }
  }

private:
  bool OnPollTick()
  {
    mStatusLabel.SetText(
      Dali::String("State: ") + Dali::String(PlayStateName(mView.GetPlayState())) +
      Dali::String(" | Frame: ") + Dali::String(std::to_string(mView.GetCurrentFrame()).c_str()) +
      Dali::String("/") + Dali::String(std::to_string(mView.GetTotalFrame()).c_str()));
    return true;
  }

  View MakeCentered(View child)
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(PREVIEW_SIZE + 16);
    row.SetPadding(Insets(0.0f, 0.0f, 8.0f, 8.0f));
    row.Add(ManualTest::MakeWeightedSpacer());
    row.Add(child);
    row.Add(ManualTest::MakeWeightedSpacer());
    return row;
  }

  Label MakeStatusLabel(const Dali::String& text)
  {
    Label label = Label::New(text);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(STATUS_H);
    label.SetFontSize(12.0f);
    label.SetTextColor(UiColor(C_STATUS_TEXT));
    label.SetBackgroundColor(UiColor(C_STATUS_BG));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return label;
  }

  View MakeButton(const Dali::String& label, std::function<void()> onClick)
  {
    StackLayout btn = StackLayout::New(StackOrientation::VERTICAL);
    btn.SetRequestedHeight(BTN_H);
    btn.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    btn.SetBackgroundColor(UiColor(C_BTN_BG));
    Label buttonLabel = Label::New(label);

    buttonLabel.SetRequestedWidth(MATCH_PARENT);

    buttonLabel.SetRequestedHeight(MATCH_PARENT);

    buttonLabel.SetFontSize(12.0f);

    buttonLabel.SetTextColor(UiColor(C_BTN_TEXT));

    buttonLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);

    buttonLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);

    buttonLabel.SetMultiLine(true);

    btn.Add(buttonLabel);
    btn.SetFocusable(true);

    InteractiveTrait interactive = btn.AsInteractive();

    interactive.ClickedSignal().Connect(this, [onClick = std::move(onClick)](View, InputEvent) -> bool {

      onClick();

      return true;

    });
    return btn;
  }

  StackLayout MakeButtonRow(std::initializer_list<View> buttons)
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(BTN_H);
    row.SetPadding(Insets(0.0f, 0.0f, 2.0f, 2.0f));
    for(auto& b : buttons) row.Add(b);
    return row;
  }

  AnimatedImageView mView;
  Label             mStatusLabel;
  Label             mFinishedLabel;
  Timer             mPollTimer;
  int               mFinishedCount{0};
};

REGISTER_MANUAL_TEST(TcAnimatedImageViewPlayControl)
