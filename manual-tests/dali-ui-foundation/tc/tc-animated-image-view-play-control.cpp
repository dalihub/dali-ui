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

#include <dali-ui-foundation/public-api/animated-image-view.h>

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

const char* PlayStateName(AnimatedImageView::PlayState::Type state)
{
  switch(state)
  {
    case AnimatedImageView::PlayState::PLAYING: return "PLAYING";
    case AnimatedImageView::PlayState::PAUSED:  return "PAUSED";
    case AnimatedImageView::PlayState::STOPPED: return "STOPPED";
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

    mView = AnimatedImageView::New(ANIM_WEBP)
              .SetRequestedWidth(PREVIEW_SIZE)
              .SetRequestedHeight(PREVIEW_SIZE);

    mView.AnimationFinishedSignal().Connect(this, [this](View) {
      ++mFinishedCount;
      mFinishedLabel.SetText(
        Dali::String("AnimationFinishedSignal: ") +
        Dali::String(std::to_string(mFinishedCount).c_str()));
    });

    mStatusLabel   = MakeStatusLabel("State: STOPPED | Frame: 0/0");
    mFinishedLabel = MakeStatusLabel("AnimationFinishedSignal: 0");

    mPollTimer = Timer::New(100);
    mPollTimer.TickSignal().Connect(this, &TcAnimatedImageViewPlayControl::OnPollTick);
    mPollTimer.Start();

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL)
                            .SetRequestedWidth(MATCH_PARENT)
                            .SetRequestedHeight(WRAP_CONTENT)
                            .SetBackgroundColor(UiColor(C_BG))
                            .SetPadding(Extents(8, 8, 8, 8));

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
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(PREVIEW_SIZE + 16)
                        .SetPadding(Extents(0, 0, 8, 8));
    row.Add(View::New().SetRequestedWidth(WRAP_CONTENT).SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f)));
    row.Add(child);
    row.Add(View::New().SetRequestedWidth(WRAP_CONTENT).SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f)));
    return row;
  }

  Label MakeStatusLabel(const Dali::String& text)
  {
    return Label::New(text)
      .SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(STATUS_H)
      .SetFontSize(12.0f).SetTextColor(UiColor(C_STATUS_TEXT))
      .SetBackgroundColor(UiColor(C_STATUS_BG))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER);
  }

  View MakeButton(const Dali::String& label, std::function<void()> onClick)
  {
    StackLayout btn = StackLayout::New(StackOrientation::VERTICAL)
                        .SetRequestedHeight(BTN_H)
                        .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                        .SetBackgroundColor(UiColor(C_BTN_BG))
                        .Children({
                          Label::New(label)
                            .SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(MATCH_PARENT)
                            .SetFontSize(12.0f).SetTextColor(UiColor(C_BTN_TEXT))
                            .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                            .SetVerticalTextAlignment(Text::Alignment::CENTER)
                            .SetMultiLine(true),
                        });
    btn.SetFocusable(true).AsInteractive([this, onClick = std::move(onClick)](InteractiveTrait& trait) {
      trait.ClickedSignal().Connect(this, [onClick](View, const InputEvent&) -> bool {
        onClick(); return true;
      });
    });
    return btn;
  }

  StackLayout MakeButtonRow(std::initializer_list<View> buttons)
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(BTN_H)
                        .SetPadding(Extents(0, 0, 2, 2));
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
