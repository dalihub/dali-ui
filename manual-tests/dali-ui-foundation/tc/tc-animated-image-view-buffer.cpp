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
// URL array (dog-anim PNG frames) — clearly demonstrates BatchSize/CacheSize effect
const char* const DOG_URLS[] = {
  TEST_RESOURCE_DIR "/dog-anim-001.png",
  TEST_RESOURCE_DIR "/dog-anim-002.png",
  TEST_RESOURCE_DIR "/dog-anim-003.png",
  TEST_RESOURCE_DIR "/dog-anim-004.png",
  TEST_RESOURCE_DIR "/dog-anim-005.png",
  TEST_RESOURCE_DIR "/dog-anim-006.png",
  TEST_RESOURCE_DIR "/dog-anim-007.png",
  TEST_RESOURCE_DIR "/dog-anim-008.png",
};
constexpr int DOG_URL_COUNT = 8;

constexpr float    PREVIEW_SIZE  = 200.0f;
constexpr float    BTN_H         = 52.0f;
constexpr float    STATUS_H      = 36.0f;
constexpr uint32_t C_BTN_BG      = 0x555555;
constexpr uint32_t C_BTN_TEXT    = 0xEEEEEE;
constexpr uint32_t C_STATUS_BG   = 0x222222;
constexpr uint32_t C_STATUS_TEXT = 0xCCCCCC;
constexpr uint32_t C_BG          = 0x1A1A1A;
} // namespace

/**
 * @brief Verifies AnimatedImageView buffer parameters:
 *   SetBatchSize / GetBatchSize
 *   SetCacheSize / GetCacheSize
 *
 * Uses a URL array (dog-anim 8 PNG frames) to verify BatchSize/CacheSize behaviour.
 *
 * Steps:
 *   1. [Load URLs] -> load dog-anim 8-frame PNG sequence and Play
 *   2. [Batch:1] -> prefetch only 1 frame at a time (may reduce smoothness)
 *   3. [Batch:4] -> prefetch 4 frames at a time (smoother)
 *   4. [Cache:1] -> set cache to minimum
 *   5. [Cache:8] -> cache all frames
 *   6. Confirm GetBatchSize / GetCacheSize return values shown in label
 *
 * Expected result:
 *   Get* values in the label match the Set* values immediately after each call.
 *   When CacheSize > TotalFrame all frames are cached in memory.
 */
class TcAnimatedImageViewBuffer : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "AnimatedImageView: BatchSize / CacheSize";
  }

  Dali::String GetDescription() const override
  {
    return "Verify Get* return values after calling SetBatchSize / SetCacheSize";
  }

  void OnEnter(View contentArea) override
  {
    mView = AnimatedImageView::New();
    mView.SetRequestedWidth(PREVIEW_SIZE);
    mView.SetRequestedHeight(PREVIEW_SIZE);

    mStatusLabel = MakeStatusLabel("Batch: 2 | Cache: 3 | Frame: 0/0");

    mPollTimer = Timer::New(100);
    mPollTimer.Start();

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetBackgroundColor(UiColor(C_BG));
    content.SetPadding(Insets(8.0f, 8.0f, 8.0f, 8.0f));

    content.Add(MakeCentered(mView));
    content.Add(mStatusLabel);

    content.Add(MakeButtonRow({
      MakeButton("Load URLs\n(dog 8)", [this] { OnLoadUrls(); }),
      MakeButton("Play",                 [this] { mView.Play(); }),
      MakeButton("Stop",                 [this] { mView.Stop(); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Batch:1", [this] { OnBatch(1); }),
      MakeButton("Batch:2", [this] { OnBatch(2); }),
      MakeButton("Batch:4", [this] { OnBatch(4); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Cache:1", [this] { OnCache(1); }),
      MakeButton("Cache:3", [this] { OnCache(3); }),
      MakeButton("Cache:8", [this] { OnCache(8); }),
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

  void OnLoadUrls()
  {
    Dali::Vector<Dali::String> urls;
    for(int i = 0; i < DOG_URL_COUNT; ++i) urls.PushBack(DOG_URLS[i]);
    mView.SetResourceUrls(urls);
    mView.Play();
    UpdateLabel();
  }

  void OnBatch(int size)
  {
    mView.SetBatchSize(size);
    UpdateLabel();
  }

  void OnCache(int size)
  {
    mView.SetCacheSize(size);
    UpdateLabel();
  }

  void UpdateLabel()
  {
    mStatusLabel.SetText(
      Dali::String("Batch: ") + Dali::String(std::to_string(mView.GetBatchSize()).c_str()) +
      Dali::String(" | Cache: ") + Dali::String(std::to_string(mView.GetCacheSize()).c_str()) +
      Dali::String(" | Frame: ") + Dali::String(std::to_string(mView.GetCurrentFrame()).c_str()) +
      Dali::String("/") + Dali::String(std::to_string(mView.GetTotalFrame()).c_str()));
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

REGISTER_MANUAL_TEST(TcAnimatedImageViewBuffer)
