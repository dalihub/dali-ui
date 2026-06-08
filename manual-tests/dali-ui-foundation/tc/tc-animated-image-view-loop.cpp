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
constexpr uint32_t C_BTN_ACT     = 0x1565C0;
constexpr uint32_t C_BTN_TEXT    = 0xEEEEEE;
constexpr uint32_t C_STATUS_BG   = 0x222222;
constexpr uint32_t C_STATUS_TEXT = 0xCCCCCC;
constexpr uint32_t C_BG          = 0x1A1A1A;

const char* StopBehaviorName(Ui::AnimatedImage::StopBehavior b)
{
  switch(b)
  {
    case Ui::AnimatedImage::StopBehavior::CURRENT_FRAME: return "CURRENT_FRAME";
    case Ui::AnimatedImage::StopBehavior::FIRST_FRAME:   return "FIRST_FRAME";
    case Ui::AnimatedImage::StopBehavior::LAST_FRAME:    return "LAST_FRAME";
    default:                                             return "?";
  }
}
} // namespace

/**
 * @brief Verifies AnimatedImageView loop control:
 *   SetLoopCount / GetLoopCount
 *   SetStopBehavior / GetStopBehavior
 *   AnimationFinishedSignal (fired when a loop ends)
 *
 * Steps:
 *   [LoopCount verification]:
 *   1. Click [Loop: 2] -> [Play] -> auto-stops after 2 loops
 *   2. AnimationFinishedSignal counter = 1 confirmed
 *   3. [Loop: -1] -> [Play] -> repeats infinitely
 *
 *   [StopBehavior verification]:
 *   1. Select [Stop: FIRST] -> [Play] -> [Stop] -> confirm freeze on first frame
 *   2. Select [Stop: LAST]  -> [Play] -> [Stop] -> confirm freeze on last frame
 *
 * Expected result:
 *   LoopCount=2: AnimationFinishedSignal fires after exactly 2 loops.
 *   StopBehavior: frame shown after Stop matches the configured value.
 */
class TcAnimatedImageViewLoop : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "AnimatedImageView: LoopCount / StopBehavior";
  }

  Dali::String GetDescription() const override
  {
    return "Verify AnimationFinishedSignal fires after limited LoopCount; confirm StopBehavior frame position";
  }

  void OnEnter(View contentArea) override
  {
    mFinishedCount = 0;

    mView = AnimatedImageView::New(ANIM_WEBP);
    mView.SetRequestedWidth(PREVIEW_SIZE);
    mView.SetRequestedHeight(PREVIEW_SIZE);

    mView.AnimationFinishedSignal().Connect(this, [this](View) {
      ++mFinishedCount;
      UpdateLabel();
    });

    mStatusLabel = MakeStatusLabel("LoopCount: -1 | StopBehavior: CURRENT_FRAME | Finished: 0");

    mPollTimer = Timer::New(100);
    mPollTimer.Start();

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetBackgroundColor(UiColor(C_BG));
    content.SetPadding(Extents(8, 8, 8, 8));

    content.Add(MakeCentered(mView));
    content.Add(mStatusLabel);

    content.Add(MakeButtonRow({
      MakeButton("Play",  [this] { mView.Play(); }),
      MakeButton("Stop",  [this] { mView.Stop(); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Loop:\n-1 (inf)", [this] { OnLoopCount(-1); }),
      MakeButton("Loop: 1",         [this] { OnLoopCount(1); }),
      MakeButton("Loop: 2",         [this] { OnLoopCount(2); }),
      MakeButton("Loop: 5",         [this] { OnLoopCount(5); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Stop:\nCURRENT", [this] { OnStopBehavior(Ui::AnimatedImage::StopBehavior::CURRENT_FRAME); }),
      MakeButton("Stop:\nFIRST",   [this] { OnStopBehavior(Ui::AnimatedImage::StopBehavior::FIRST_FRAME); }),
      MakeButton("Stop:\nLAST",    [this] { OnStopBehavior(Ui::AnimatedImage::StopBehavior::LAST_FRAME); }),
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

  void OnLoopCount(int count)
  {
    mView.SetLoopCount(count);
    UpdateLabel();
  }

  void OnStopBehavior(Ui::AnimatedImage::StopBehavior b)
  {
    mView.SetStopBehavior(b);
    UpdateLabel();
  }

  void UpdateLabel()
  {
    int loop = mView.GetLoopCount();
    Dali::String loopStr = (loop < 0) ? Dali::String("-1 (inf)") : Dali::String(std::to_string(loop).c_str());
    mStatusLabel.SetText(
      Dali::String("LoopCount: ") + loopStr +
      Dali::String(" | Stop: ") + Dali::String(StopBehaviorName(mView.GetStopBehavior())) +
      Dali::String(" | Finished: ") + Dali::String(std::to_string(mFinishedCount).c_str()));
  }

  View MakeCentered(View child)
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(PREVIEW_SIZE + 16);
    row.SetPadding(Extents(0, 0, 8, 8));
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
    label.SetMultiLine(true);
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

    btn.AddChildren({buttonLabel});
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
    row.SetPadding(Extents(0, 0, 2, 2));
    for(auto& b : buttons) row.Add(b);
    return row;
  }

  AnimatedImageView mView;
  Label             mStatusLabel;
  Timer             mPollTimer;
  int               mFinishedCount{0};
};

REGISTER_MANUAL_TEST(TcAnimatedImageViewLoop)
