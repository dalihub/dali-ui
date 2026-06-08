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
 * @brief Verifies LottieAnimationView render quality settings:
 *   SetRenderScale / GetRenderScale
 *   SetEnableFrameCache / IsFrameCacheEnabled
 *   SetRedrawOnScaleDown / IsRedrawOnScaleDown
 *   SetRedrawOnScaleUp / IsRedrawOnScaleUp
 *   SetNotifyAfterRasterization / IsNotifyAfterRasterizationEnabled
 *
 * Steps:
 *   [RenderScale verification]:
 *   1. [Scale 0.25x] -> lower resolution (pixelation/blurriness visible)
 *   2. [Scale 1.0x] -> normal resolution
 *   3. [Scale 2.0x] -> higher resolution (sharper but uses more memory)
 *   4. Confirm GetRenderScale return value in label
 *
 *   [FrameCache verification]:
 *   1. [FrameCache ON] -> performance improves when reusing same size
 *   2. [FrameCache OFF] -> re-rasterises every frame
 *
 *   [RedrawOnScale verification]:
 *   1. [RedrawSD OFF] -> no re-rasterisation on scale-down (existing texture scaled)
 *   2. [RedrawSD ON] -> re-rasterises on scale-down
 *
 * Expected result:
 *   RenderScale 0.25x: image appears blurry/pixelated.
 *   RenderScale 2.0x: sharper but uses more memory.
 *   After each Set, the Is.../Get... return values are shown correctly in the label.
 */
class TcLottieRenderQuality : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "Lottie: RenderScale / FrameCache / RedrawOnScale";
  }

  Dali::String GetDescription() const override
  {
    return "Visual quality comparison of RenderScale; verify Get* return values for FrameCache/RedrawOnScale flags";
  }

  void OnEnter(View contentArea) override
  {
    mView = LottieAnimationView::New(LOTTIE_WALKER);
    mView.SetRequestedWidth(PREVIEW_SIZE);
    mView.SetRequestedHeight(PREVIEW_SIZE);

    mView.Play();

    mScaleLabel = MakeStatusLabel("RenderScale: 1.0 | FrameCache: OFF");
    mFlagsLabel = MakeStatusLabel("RedrawSD: ON | RedrawSU: ON | NotifyRast: OFF");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetBackgroundColor(UiColor(C_BG));
    content.SetPadding(Extents(8, 8, 8, 8));

    content.Add(MakeCentered(mView));
    content.Add(mScaleLabel);
    content.Add(mFlagsLabel);

    content.Add(MakeButtonRow({
      MakeButton("Scale\n0.25x", [this] { OnRenderScale(0.25f); }),
      MakeButton("Scale\n0.5x",  [this] { OnRenderScale(0.5f); }),
      MakeButton("Scale\n1.0x",  [this] { OnRenderScale(1.0f); }),
      MakeButton("Scale\n2.0x",  [this] { OnRenderScale(2.0f); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("FrameCache\nON",  [this] { mView.SetEnableFrameCache(true);  UpdateLabels(); }),
      MakeButton("FrameCache\nOFF", [this] { mView.SetEnableFrameCache(false); UpdateLabels(); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("RedrawSD\nON",  [this] { mView.SetRedrawOnScaleDown(true);  UpdateLabels(); }),
      MakeButton("RedrawSD\nOFF", [this] { mView.SetRedrawOnScaleDown(false); UpdateLabels(); }),
      MakeButton("RedrawSU\nON",  [this] { mView.SetRedrawOnScaleUp(true);    UpdateLabels(); }),
      MakeButton("RedrawSU\nOFF", [this] { mView.SetRedrawOnScaleUp(false);   UpdateLabels(); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("NotifyRast\nON",  [this] { mView.SetNotifyAfterRasterization(true);  UpdateLabels(); }),
      MakeButton("NotifyRast\nOFF", [this] { mView.SetNotifyAfterRasterization(false); UpdateLabels(); }),
    }));

    contentArea.Add(content);
  }

private:
  void OnRenderScale(float scale)
  {
    mView.SetRenderScale(scale);
    UpdateLabels();
  }

  void UpdateLabels()
  {
    mScaleLabel.SetText(
      Dali::String("RenderScale: ") + Dali::String(std::to_string(mView.GetRenderScale()).c_str()) +
      Dali::String(" | FrameCache: ") + Dali::String(mView.IsFrameCacheEnabled() ? "ON" : "OFF"));
    mFlagsLabel.SetText(
      Dali::String("RedrawSD: ") + Dali::String(mView.IsRedrawOnScaleDown() ? "ON" : "OFF") +
      Dali::String(" | RedrawSU: ") + Dali::String(mView.IsRedrawOnScaleUp() ? "ON" : "OFF") +
      Dali::String(" | NotifyRast: ") + Dali::String(mView.IsNotifyAfterRasterizationEnabled() ? "ON" : "OFF"));
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

    buttonLabel.SetFontSize(11.0f);

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

  LottieAnimationView mView;
  Label               mScaleLabel;
  Label               mFlagsLabel;
};

REGISTER_MANUAL_TEST(TcLottieRenderQuality)
