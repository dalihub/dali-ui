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
} // namespace

/**
 * @brief Verifies AnimatedImageView speed control:
 *   SetFrameSpeedFactor / GetFrameSpeedFactor
 *   SetFrameDelay / GetFrameDelay
 *
 * Steps:
 *   [FrameSpeedFactor verification]:
 *   1. Click [Play] -> confirm default speed (1.0x)
 *   2. [Speed 0.25x] -> confirm very slow playback
 *   3. [Speed 4.0x] -> confirm very fast playback
 *   4. Confirm GetFrameSpeedFactor return value is shown in label
 *
 *   [FrameDelay verification]:
 *   1. [Delay 0ms] -> plays as fast as possible
 *   2. [Delay 500ms] -> each frame is displayed for 0.5 s
 *
 * Expected result:
 *   After clicking a Speed button the animation speed changes visibly.
 *   After clicking a Delay button the frame transition interval changes.
 */
class TcAnimatedImageViewSpeed : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "AnimatedImageView: FrameSpeedFactor / FrameDelay";
  }

  Dali::String GetDescription() const override
  {
    return "Visual comparison of playback speed (FrameSpeedFactor) and frame interval (FrameDelay)";
  }

  void OnEnter(View contentArea) override
  {
    mView = AnimatedImageView::New(ANIM_WEBP);
    mView.SetRequestedWidth(PREVIEW_SIZE);
    mView.SetRequestedHeight(PREVIEW_SIZE);

    mStatusLabel = MakeStatusLabel("Speed: 1.0 | Delay: 0ms | Frame: 0/0");

    mPollTimer = Timer::New(100);
    mPollTimer.TickSignal().Connect(this, &TcAnimatedImageViewSpeed::OnPollTick);
    mPollTimer.Start();

    mView.Play();

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetBackgroundColor(UiColor(C_BG));
    content.SetPadding(Insets(8.0f, 8.0f, 8.0f, 8.0f));

    content.Add(MakeCentered(mView));
    content.Add(mStatusLabel);

    content.Add(MakeButtonRow({
      MakeButton("Speed\n0.25x", [this] { OnSpeed(0.25f); }),
      MakeButton("Speed\n0.5x",  [this] { OnSpeed(0.5f); }),
      MakeButton("Speed\n1.0x",  [this] { OnSpeed(1.0f); }),
      MakeButton("Speed\n2.0x",  [this] { OnSpeed(2.0f); }),
      MakeButton("Speed\n4.0x",  [this] { OnSpeed(4.0f); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Delay\n0ms",   [this] { OnDelay(0); }),
      MakeButton("Delay\n100ms", [this] { OnDelay(100); }),
      MakeButton("Delay\n300ms", [this] { OnDelay(300); }),
      MakeButton("Delay\n500ms", [this] { OnDelay(500); }),
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
      Dali::String("Speed: ") + Dali::String(std::to_string(mView.GetFrameSpeedFactor()).c_str()) +
      Dali::String(" | Delay: ") + Dali::String(std::to_string(mView.GetFrameDelay()).c_str()) + Dali::String("ms") +
      Dali::String(" | Frame: ") + Dali::String(std::to_string(mView.GetCurrentFrame()).c_str()) +
      Dali::String("/") + Dali::String(std::to_string(mView.GetTotalFrame()).c_str()));
    return true;
  }

  void OnSpeed(float factor)
  {
    mView.SetFrameSpeedFactor(factor);
  }

  void OnDelay(int ms)
  {
    mView.SetFrameDelay(ms);
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

    buttonLabel.SetFontSize(11.0f);

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
  Timer             mPollTimer;
};

REGISTER_MANUAL_TEST(TcAnimatedImageViewSpeed)
