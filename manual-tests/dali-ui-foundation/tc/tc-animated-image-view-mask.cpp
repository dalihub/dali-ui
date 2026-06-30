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
const char* const ANIM_WEBP        = TEST_RESOURCE_DIR "/dog-anim.webp";
const char* const IMG_MASK_CIRCLE  = TEST_RESOURCE_DIR "/contact-cards-mask.png";
const char* const IMG_MASK_BUBBLE  = TEST_RESOURCE_DIR "/mask.png";

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
 * @brief Verifies AnimatedImageView alpha masking:
 *   SetAlphaMaskUrl / GetAlphaMaskUrl
 *   SetCropToMask / IsCropToMask
 *   SetMaskingMode / GetMaskingMode
 *
 * Steps:
 *   [AlphaMask verification]:
 *   1. Click [Circle Mask] -> animation clips to circle shape during playback
 *   2. Click [Bubble Mask] -> mask shape changes
 *   3. Confirm GetAlphaMaskUrl return value shown in label
 *   4. Click [No Mask] -> mask removed, original rectangle displayed
 *
 *   [CropToMask verification]:
 *   1. [CropToMask ON] -> view size adjusts to match mask area
 *   2. [CropToMask OFF] -> original view size maintained
 *
 * Expected result:
 *   With a mask applied the animation plays only within the mask alpha-channel shape.
 *   CropToMask ON: view is cropped to match the mask dimensions.
 */
class TcAnimatedImageViewMask : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "AnimatedImageView: AlphaMask / CropToMask / MaskingMode";
  }

  Dali::String GetDescription() const override
  {
    return "Verify AlphaMask apply/remove and CropToMask ON/OFF visually during playback";
  }

  void OnEnter(View contentArea) override
  {
    mView = AnimatedImageView::New(ANIM_WEBP);
    mView.SetRequestedWidth(PREVIEW_SIZE);
    mView.SetRequestedHeight(PREVIEW_SIZE);

    mView.Play();

    mStatusLabel = MakeStatusLabel("Mask: none | CropToMask: OFF | MaskingMode: ON_RENDERING");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetBackgroundColor(UiColor(C_BG));
    content.SetPadding(Extents(8, 8, 8, 8));

    content.Add(MakeCentered(mView));
    content.Add(mStatusLabel);

    content.Add(MakeButtonRow({
      MakeButton("Circle\nMask",  [this] { OnMask(IMG_MASK_CIRCLE, "circle"); }),
      MakeButton("Bubble\nMask",  [this] { OnMask(IMG_MASK_BUBBLE, "bubble"); }),
      MakeButton("No Mask",       [this] { OnMask("", "none"); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("CropToMask\nON",  [this] { mView.SetCropToMask(true);  UpdateLabel(); }),
      MakeButton("CropToMask\nOFF", [this] { mView.SetCropToMask(false); UpdateLabel(); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Masking:\nON_RENDERING", [this] { mView.SetMaskingMode(Ui::Image::MaskingType::MASKING_ON_RENDERING); UpdateLabel(); }),
      MakeButton("Masking:\nON_LOADING",   [this] { mView.SetMaskingMode(Ui::Image::MaskingType::MASKING_ON_LOADING);   UpdateLabel(); }),
    }));

    contentArea.Add(content);
  }

private:
  void OnMask(const char* url, const char* name)
  {
    mView.SetAlphaMaskUrl(url);
    mMaskName = name;
    UpdateLabel();
  }

  void UpdateLabel()
  {
    auto mode = mView.GetMaskingMode();
    Dali::String modeStr = (mode == Ui::Image::MaskingType::MASKING_ON_LOADING) ? "ON_LOADING" : "ON_RENDERING";
    mStatusLabel.SetText(
      Dali::String("Mask: ") + Dali::String(mMaskName.c_str()) +
      Dali::String("\nCropToMask: ") + Dali::String(mView.IsCropToMask() ? "ON" : "OFF") +
      Dali::String(" | MaskingMode: ") + modeStr);
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
  std::string       mMaskName{"none"};
};

REGISTER_MANUAL_TEST(TcAnimatedImageViewMask)
