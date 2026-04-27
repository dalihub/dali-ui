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
} // namespace

/**
 * @brief Verifies LottieAnimationView frame range control:
 *   SetMinMaxFrame / SetMinMaxFrameByMarker
 *   SetFrameSpeedFactor / GetFrameSpeedFactor
 *
 * Steps:
 *   [SetMinMaxFrame verification]:
 *   1. [Play] -> confirm full frame range (0~TotalFrame) plays
 *   2. [First Half] -> confirm only first half plays (frame counter loops at midpoint)
 *   3. [Second Half] -> confirm only second half plays
 *   4. [First 10] -> confirm only frames 0~10 repeat
 *   5. [Full] -> confirm full range restored
 *
 *   [FrameSpeedFactor verification]:
 *   1. [Speed 0.25x] -> slow playback
 *   2. [Speed 2.0x] -> fast playback
 *
 * Expected result:
 *   After SetMinMaxFrame the frame counter changes only within the specified range during playback.
 */
class TcLottieFrameRange : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "Lottie: SetMinMaxFrame / FrameSpeedFactor";
  }

  Dali::String GetDescription() const override
  {
    return "Limit playback range with SetMinMaxFrame; verify speed adjustment with FrameSpeedFactor";
  }

  void OnEnter(View contentArea) override
  {
    mView = LottieAnimationView::New(LOTTIE_WALKER)
              .SetRequestedWidth(PREVIEW_SIZE)
              .SetRequestedHeight(PREVIEW_SIZE);

    mView.Play();

    mStatusLabel = MakeStatusLabel("Frame: 0/0 | Range: full | Speed: 1.0");

    mPollTimer = Timer::New(100);
    mPollTimer.TickSignal().Connect(this, &TcLottieFrameRange::OnPollTick);
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
      MakeButton("Full",        [this] { OnRange("full");        int total = mView.GetTotalFrame(); mView.SetMinMaxFrame(0, total); }),
      MakeButton("First Half",  [this] { OnRange("first half");  int total = mView.GetTotalFrame(); mView.SetMinMaxFrame(0, total / 2); }),
      MakeButton("Second Half", [this] { OnRange("second half"); int total = mView.GetTotalFrame(); mView.SetMinMaxFrame(total / 2, total); }),
      MakeButton("First 10",    [this] { OnRange("0~10");        mView.SetMinMaxFrame(0, 10); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Speed\n0.25x", [this] { mView.SetFrameSpeedFactor(0.25f); }),
      MakeButton("Speed\n0.5x",  [this] { mView.SetFrameSpeedFactor(0.5f); }),
      MakeButton("Speed\n1.0x",  [this] { mView.SetFrameSpeedFactor(1.0f); }),
      MakeButton("Speed\n2.0x",  [this] { mView.SetFrameSpeedFactor(2.0f); }),
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
      Dali::String("Frame: ") + Dali::String(std::to_string(mView.GetCurrentFrame()).c_str()) +
      Dali::String("/") + Dali::String(std::to_string(mView.GetTotalFrame()).c_str()) +
      Dali::String(" | Range: ") + Dali::String(mRangeDesc.c_str()) +
      Dali::String(" | Speed: ") + Dali::String(std::to_string(mView.GetFrameSpeedFactor()).c_str()));
    return true;
  }

  void OnRange(const char* desc)
  {
    mRangeDesc = desc;
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
                            .SetFontSize(11.0f).SetTextColor(UiColor(C_BTN_TEXT))
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
  std::string         mRangeDesc{"full"};
};

REGISTER_MANUAL_TEST(TcLottieFrameRange)
