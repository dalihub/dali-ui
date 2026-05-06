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
const char* const IMG = TEST_RESOURCE_DIR "/people-small-10.jpg"; // small image displayed large to show sampling differences

constexpr float    PREVIEW_SIZE  = 240.0f;
constexpr float    BTN_H         = 52.0f;
constexpr float    STATUS_H      = 32.0f;
constexpr uint32_t C_BTN_BG      = 0x555555;
constexpr uint32_t C_BTN_ACT     = 0x1565C0;
constexpr uint32_t C_BTN_TEXT    = 0xEEEEEE;
constexpr uint32_t C_STATUS_BG   = 0x222222;
constexpr uint32_t C_STATUS_TEXT = 0xCCCCCC;
constexpr uint32_t C_BG          = 0x1A1A1A;
} // namespace

/**
 * @brief Verifies ImageView SamplingMode / DesiredSize / ImageLoadWithViewSize.
 *
 * Displays people-small-10.jpg enlarged to make SamplingMode differences visible.
 *
 * Steps:
 *   1. [BOX] / [NEAREST] / [LINEAR] -> observe sharpness / smoothness differences
 *   2. [Desired 50x50] -> image loaded at 50x50 (low-resolution visible)
 *   3. [Desired 0x0]   -> no limit (original resolution)
 *   4. [LoadWithViewSize ON] -> loaded to match view size (240x240)
 *
 * Expected result:
 *   NEAREST produces sharp pixel blocks; BOX/LINEAR appear smoother.
 *   Restricting DesiredSize loads the image at lower resolution, saving memory.
 */
class TcImageViewSampling : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "ImageView: SamplingMode / DesiredSize";
  }

  Dali::String GetDescription() const override
  {
    return "Visual quality comparison of SamplingMode; verify DesiredSize and ImageLoadWithViewSize";
  }

  void OnEnter(View contentArea) override
  {
    mImage = ImageView::New(IMG)
               .SetRequestedWidth(PREVIEW_SIZE)
               .SetRequestedHeight(PREVIEW_SIZE)
               .SetFittingMode(Ui::Image::FittingMode::FILL);

    mSamplingLabel = MakeStatusLabel("SamplingMode: BOX");
    mSizeLabel     = MakeStatusLabel("DesiredSize: 0x0 | LoadWithViewSize: OFF");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL)
                            .SetRequestedWidth(MATCH_PARENT)
                            .SetRequestedHeight(WRAP_CONTENT)
                            .SetBackgroundColor(UiColor(C_BG))
                            .SetPadding(Extents(8, 8, 8, 8));

    content.Add(MakeCentered(mImage));
    content.Add(mSamplingLabel);
    content.Add(mSizeLabel);
    content.Add(MakeButtonRow({
      MakeButton("BOX",     [this] { OnSampling(Ui::Image::SamplingMode::BOX,            "BOX"); }),
      MakeButton("NEAREST", [this] { OnSampling(Ui::Image::SamplingMode::NEAREST,        "NEAREST"); }),
      MakeButton("LINEAR",  [this] { OnSampling(Ui::Image::SamplingMode::LINEAR,         "LINEAR"); }),
      MakeButton("BOX_LIN", [this] { OnSampling(Ui::Image::SamplingMode::BOX_THEN_LINEAR,"BOX_LIN"); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Desired\n0x0",   [this] { OnDesiredSize(0,  0); }),
      MakeButton("Desired\n50x50", [this] { OnDesiredSize(50, 50); }),
      MakeButton("Desired\n150x150",[this] { OnDesiredSize(150,150); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("LoadWithViewSize\nON",  [this] { OnLoadWithViewSize(true); }),
      MakeButton("LoadWithViewSize\nOFF", [this] { OnLoadWithViewSize(false); }),
    }));

    contentArea.Add(content);
  }

private:
  void OnSampling(Ui::Image::SamplingMode mode, const char* name)
  {
    mImage.SetSamplingMode(mode);
    mImage.Reload();
    mSamplingLabel.SetText(Dali::String("SamplingMode: ") + Dali::String(name) +
                           Dali::String(" | GetSamplingMode match: ") +
                           Dali::String(mImage.GetSamplingMode() == mode ? "OK" : "NG"));
  }

  void OnDesiredSize(int w, int h)
  {
    mImage.SetDesiredWidth(w);
    mImage.SetDesiredHeight(h);
    mImage.Reload();
    mSizeLabel.SetText(
      Dali::String("DesiredSize: ") + Dali::String(std::to_string(mImage.GetDesiredWidth()).c_str()) +
      Dali::String("x") + Dali::String(std::to_string(mImage.GetDesiredHeight()).c_str()) +
      Dali::String(" | LoadWithViewSize: ") +
      Dali::String(mImage.IsImageLoadWithViewSizeEnabled() ? "ON" : "OFF"));
  }

  void OnLoadWithViewSize(bool enabled)
  {
    mImage.SetImageLoadWithViewSize(enabled);
    mImage.Reload();
    mSizeLabel.SetText(
      Dali::String("DesiredSize: ") + Dali::String(std::to_string(mImage.GetDesiredWidth()).c_str()) +
      Dali::String("x") + Dali::String(std::to_string(mImage.GetDesiredHeight()).c_str()) +
      Dali::String(" | LoadWithViewSize: ") +
      Dali::String(mImage.IsImageLoadWithViewSizeEnabled() ? "ON" : "OFF"));
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
                            .SetFontSize(11.0f).SetTextColor(UiColor(C_BTN_TEXT))
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
  Label     mSamplingLabel;
  Label     mSizeLabel;
};

REGISTER_MANUAL_TEST(TcImageViewSampling)
