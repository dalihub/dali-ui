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

#include <dali-ui-foundation/public-api/lottie-animation-view.h>

#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
const char* const LOTTIE_WALKER = TEST_RESOURCE_DIR "/jolly_walker.json";

constexpr float    PREVIEW_SIZE  = 240.0f;
constexpr float    BTN_H         = 52.0f;
constexpr float    STATUS_H      = 36.0f;
constexpr uint32_t C_BTN_BG      = 0x555555;
constexpr uint32_t C_BTN_TEXT    = 0xEEEEEE;
constexpr uint32_t C_STATUS_BG   = 0x222222;
constexpr uint32_t C_STATUS_TEXT = 0xCCCCCC;
constexpr uint32_t C_BG          = 0x1A1A1A;

const char* StopBehaviorName(LottieAnimationView::StopBehavior::Type b)
{
  switch(b)
  {
    case LottieAnimationView::StopBehavior::CURRENT_FRAME: return "CURRENT_FRAME";
    case LottieAnimationView::StopBehavior::FIRST_FRAME:   return "FIRST_FRAME";
    case LottieAnimationView::StopBehavior::LAST_FRAME:    return "LAST_FRAME";
    default:                                               return "?";
  }
}

const char* LoopingModeName(LottieAnimationView::LoopingMode::Type m)
{
  switch(m)
  {
    case LottieAnimationView::LoopingMode::RESTART:      return "RESTART";
    case LottieAnimationView::LoopingMode::AUTO_REVERSE: return "AUTO_REVERSE";
    default:                                             return "?";
  }
}
} // namespace

/**
 * @brief Verifies LottieAnimationView loop control:
 *   SetLoopCount / GetLoopCount
 *   SetLoopingMode / GetLoopingMode
 *   SetStopBehavior / GetStopBehavior
 *   AnimationFinishedSignal (fired when a loop ends)
 *
 * Steps:
 *   [LoopCount verification]:
 *   1. [Loop: 2] -> [Play] -> auto-stops after 2 loops; AnimationFinishedSignal counter=1 confirmed
 *   2. [Loop: -1] -> [Play] -> repeats infinitely
 *
 *   [LoopingMode verification]:
 *   1. [AUTO_REVERSE] -> [Play] -> confirm forward/reverse alternating playback
 *   2. [RESTART] -> [Play] -> confirm restart from beginning each loop
 *
 *   [StopBehavior verification]:
 *   1. [Stop: LAST] -> [Play] -> [Stop] -> confirm freeze on last frame
 *
 * Expected result:
 *   LoopCount=2: AnimationFinishedSignal fires after exactly 2 loops.
 *   AUTO_REVERSE: animation plays forward then backward (ping-pong).
 */
class TcLottieLoop : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "Lottie: LoopCount / LoopingMode / StopBehavior";
  }

  Dali::String GetDescription() const override
  {
    return "Verify limited LoopCount + AnimationFinishedSignal; AUTO_REVERSE ping-pong; StopBehavior frame position";
  }

  void OnEnter(View contentArea) override
  {
    mFinishedCount = 0;

    mView = LottieAnimationView::New(LOTTIE_WALKER)
              .SetRequestedWidth(PREVIEW_SIZE)
              .SetRequestedHeight(PREVIEW_SIZE);

    mView.AnimationFinishedSignal().Connect(this, [this](View) {
      ++mFinishedCount;
      UpdateLabel();
    });

    mStatusLabel = MakeStatusLabel("LoopCount: -1 | LoopingMode: RESTART | Stop: CURRENT_FRAME | Finished: 0");

    mPollTimer = Timer::New(100);
    mPollTimer.TickSignal().Connect(this, &TcLottieLoop::OnPollTick);
    mPollTimer.Start();

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL)
                            .SetRequestedWidth(MATCH_PARENT)
                            .SetRequestedHeight(WRAP_CONTENT)
                            .SetBackgroundColor(UiColor(C_BG))
                            .SetPadding(Extents(8, 8, 8, 8));

    content.Add(MakeCentered(mView));
    content.Add(mStatusLabel);

    content.Add(MakeButtonRow({
      MakeButton("Play",  [this] { mView.Play(); }),
      MakeButton("Stop",  [this] { mView.Stop(); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Loop:\n-1 (inf)", [this] { mView.SetLoopCount(-1); UpdateLabel(); }),
      MakeButton("Loop: 1",         [this] { mView.SetLoopCount(1);  UpdateLabel(); }),
      MakeButton("Loop: 2",         [this] { mView.SetLoopCount(2);  UpdateLabel(); }),
      MakeButton("Loop: 3",         [this] { mView.SetLoopCount(3);  UpdateLabel(); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("LoopMode:\nRESTART",      [this] { mView.SetLoopingMode(LottieAnimationView::LoopingMode::RESTART);      UpdateLabel(); }),
      MakeButton("LoopMode:\nAUTO_REVERSE", [this] { mView.SetLoopingMode(LottieAnimationView::LoopingMode::AUTO_REVERSE); UpdateLabel(); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Stop:\nCURRENT", [this] { mView.SetStopBehavior(LottieAnimationView::StopBehavior::CURRENT_FRAME); UpdateLabel(); }),
      MakeButton("Stop:\nFIRST",   [this] { mView.SetStopBehavior(LottieAnimationView::StopBehavior::FIRST_FRAME);   UpdateLabel(); }),
      MakeButton("Stop:\nLAST",    [this] { mView.SetStopBehavior(LottieAnimationView::StopBehavior::LAST_FRAME);    UpdateLabel(); }),
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
    UpdateLabel();
    return true;
  }

  void UpdateLabel()
  {
    int loop = mView.GetLoopCount();
    Dali::String loopStr = (loop < 0) ? Dali::String("-1 (inf)") : Dali::String(std::to_string(loop).c_str());
    mStatusLabel.SetText(
      Dali::String("Loop: ") + loopStr +
      Dali::String(" | LoopMode: ") + Dali::String(LoopingModeName(mView.GetLoopingMode())) +
      Dali::String("\nStop: ") + Dali::String(StopBehaviorName(mView.GetStopBehavior())) +
      Dali::String(" | Finished: ") + Dali::String(std::to_string(mFinishedCount).c_str()));
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
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .SetMultiLine(true);
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

  LottieAnimationView mView;
  Label               mStatusLabel;
  Timer               mPollTimer;
  int                 mFinishedCount{0};
};

REGISTER_MANUAL_TEST(TcLottieLoop)
