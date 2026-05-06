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

    mImage = ImageView::New(IMG_A)
               .SetRequestedWidth(PREVIEW_SIZE)
               .SetRequestedHeight(PREVIEW_SIZE);

    mImage.ResourceReadySignal().Connect(this, [this](View) {
      ++mReadyCount;
      UpdateReadyLabel();
    });

    mFlagsLabel = MakeStatusLabel("Sync: OFF | FastTrack: OFF | OrientCorr: ON | PreMult: OFF");
    mReadyLabel = MakeStatusLabel("ResourceReadySignal: 0");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL)
                            .SetRequestedWidth(MATCH_PARENT)
                            .SetRequestedHeight(WRAP_CONTENT)
                            .SetBackgroundColor(UiColor(C_BG))
                            .SetPadding(Extents(8, 8, 8, 8));

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
      trait.ClickedSignal().Connect(this, [onClick](View, InputEvent) -> bool {
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

  ImageView mImage;
  Label     mFlagsLabel;
  Label     mReadyLabel;
  int       mReadyCount{0};
};

REGISTER_MANUAL_TEST(TcImageViewSyncLoading)
