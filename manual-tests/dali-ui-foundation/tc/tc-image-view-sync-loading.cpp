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

#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
const char* const IMG_A    = TEST_RESOURCE_DIR "/gallery-large-3.jpg";
const char* const IMG_EXIF = TEST_RESOURCE_DIR "/exif-rotated.jpg";

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
 * @brief Verifies ImageView synchronous-loading related flags:
 *   SetSynchronousLoading / IsSynchronousLoading
 *   SetFastTrackUpload / IsFastTrackUploadEnabled
 *   SetOrientationCorrection / IsOrientationCorrectionEnabled
 *   SetPreMultipliedAlpha / IsPreMultipliedAlpha
 *
 * Steps:
 *   [SynchronousLoading verification]:
 *   1. Click [Sync ON] -> status label shows Sync: ON
 *   2. Click [Load IMG_A] -> image appears immediately (no ResourceReady counter increment)
 *   3. Click [Sync OFF] then [Reload] -> ResourceReady counter increments
 *
 *   [OrientationCorrection verification]:
 *   1. Click [Load EXIF] -> observe current orientation
 *   2. Toggle [OrientCorr ON/OFF] then [Reload] -> observe image orientation change
 *
 *   [PreMultipliedAlpha verification]:
 *   1. Toggle [PreMult ON] / [PreMult OFF] then [Reload] -> observe visual difference
 *
 * Expected result:
 *   SynchronousLoading ON: image appears immediately on button click, no ResourceReady fired.
 *   OrientationCorrection ON: exif-rotated.jpg is displayed in the correct orientation.
 */
class TcImageViewSyncLoading : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "ImageView: SyncLoading / FastTrack / OrientCorr / PreMult";
  }

  Dali::String GetDescription() const override
  {
    return "Verify immediate display with SynchronousLoading ON; compare OrientationCorrection ON/OFF on exif-rotated.jpg";
  }

  void OnEnter(View contentArea) override
  {
    mReadyCount = 0;

    mImage = ImageView::New(IMG_A);
    mImage.SetRequestedWidth(PREVIEW_SIZE);
    mImage.SetRequestedHeight(PREVIEW_SIZE);

    mImage.ResourceReadySignal().Connect(this, [this](View) {
      ++mReadyCount;
      UpdateReadyLabel();
    });

    mFlagsLabel = MakeStatusLabel("Sync: OFF | FastTrack: OFF | OrientCorr: ON | PreMult: OFF");
    mReadyLabel = MakeStatusLabel("ResourceReadySignal: 0");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetBackgroundColor(UiColor(C_BG));
    content.SetPadding(Insets(8.0f, 8.0f, 8.0f, 8.0f));

    content.Add(MakeCentered(mImage));
    content.Add(mFlagsLabel);
    content.Add(mReadyLabel);

    content.Add(MakeButtonRow({
      MakeButton("Sync\nON",  [this] { mImage.SetSynchronousLoading(true);  UpdateFlags(); }),
      MakeButton("Sync\nOFF", [this] { mImage.SetSynchronousLoading(false); UpdateFlags(); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("FastTrack\nON",  [this] { mImage.SetFastTrackUpload(true);  UpdateFlags(); }),
      MakeButton("FastTrack\nOFF", [this] { mImage.SetFastTrackUpload(false); UpdateFlags(); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("OrientCorr\nON",  [this] { mImage.SetOrientationCorrection(true);  mImage.Reload(); UpdateFlags(); }),
      MakeButton("OrientCorr\nOFF", [this] { mImage.SetOrientationCorrection(false); mImage.Reload(); UpdateFlags(); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("PreMult\nON",  [this] { mImage.SetPreMultipliedAlpha(true);  mImage.Reload(); UpdateFlags(); }),
      MakeButton("PreMult\nOFF", [this] { mImage.SetPreMultipliedAlpha(false); mImage.Reload(); UpdateFlags(); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Load\nIMG_A",      [this] { mImage.SetResourceUrl(IMG_A);    UpdateFlags(); }),
      MakeButton("Load\nEXIF",       [this] { mImage.SetResourceUrl(IMG_EXIF); UpdateFlags(); }),
      MakeButton("Reload",           [this] { mImage.Reload(); UpdateFlags(); }),
    }));

    contentArea.Add(content);
  }

private:
  void UpdateFlags()
  {
    mFlagsLabel.SetText(
      Dali::String("Sync: ") + Dali::String(mImage.IsSynchronousLoading() ? "ON" : "OFF") +
      Dali::String(" | FastTrack: ") + Dali::String(mImage.IsFastTrackUploadEnabled() ? "ON" : "OFF") +
      Dali::String(" | OrientCorr: ") + Dali::String(mImage.IsOrientationCorrectionEnabled() ? "ON" : "OFF") +
      Dali::String(" | PreMult: ") + Dali::String(mImage.IsPreMultipliedAlpha() ? "ON" : "OFF"));
  }

  void UpdateReadyLabel()
  {
    mReadyLabel.SetText(
      Dali::String("ResourceReadySignal: ") +
      Dali::String(std::to_string(mReadyCount).c_str()));
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

  ImageView mImage;
  Label     mFlagsLabel;
  Label     mReadyLabel;
  int       mReadyCount{0};
};

REGISTER_MANUAL_TEST(TcImageViewSyncLoading)
