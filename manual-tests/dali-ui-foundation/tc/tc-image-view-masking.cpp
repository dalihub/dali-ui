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
const char* const IMG_A            = TEST_RESOURCE_DIR "/gallery-large-3.jpg";
const char* const IMG_MASK_CIRCLE  = TEST_RESOURCE_DIR "/contact-cards-mask.png";
const char* const IMG_MASK_BUBBLE  = TEST_RESOURCE_DIR "/mask.png";

constexpr float    PREVIEW_SIZE  = 200.0f;
constexpr float    BTN_H         = 52.0f;
constexpr float    STATUS_H      = 36.0f;
constexpr float    LABEL_H       = 30.0f;
constexpr float    FONT_BTN      = 12.0f;
constexpr float    FONT_LABEL    = 12.0f;
constexpr float    FONT_STATUS   = 12.0f;
constexpr uint32_t C_BTN_BG      = 0x555555;
constexpr uint32_t C_BTN_TEXT    = 0xEEEEEE;
constexpr uint32_t C_STATUS_BG   = 0x222222;
constexpr uint32_t C_STATUS_TEXT = 0xCCCCCC;
constexpr uint32_t C_BG          = 0x1A1A1A;
constexpr uint32_t C_LABEL_BG    = 0x333333;
} // namespace

/**
 * @brief Verifies ImageView alpha masking API:
 *   SetAlphaMaskUrl / GetAlphaMaskUrl
 *   SetCropToMask / IsCropToMask
 *   SetMaskingMode / GetMaskingMode
 *
 * Two ImageViews are shown side by side:
 *   Left:  CropToMask = OFF
 *   Right: CropToMask = ON
 * Both receive the same mask so the difference is immediately visible.
 */
class TcImageViewMasking : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "ImageView Alpha Masking";
  }

  Dali::String GetDescription() const override
  {
    return "Verifies AlphaMaskUrl, CropToMask, MaskingMode (ON_RENDERING vs ON_LOADING)";
  }

  void OnEnter(View contentArea) override
  {
    mMaskUrl     = "";
    mMaskingMode = Ui::Image::MaskingType::MASKING_ON_RENDERING;

    // Left: CropToMask OFF
    mImageLeft = ImageView::New(IMG_A)
                   .SetRequestedWidth(PREVIEW_SIZE)
                   .SetRequestedHeight(PREVIEW_SIZE)
                   .SetCropToMask(false);

    // Right: CropToMask ON
    mImageRight = ImageView::New(IMG_A)
                    .SetRequestedWidth(PREVIEW_SIZE)
                    .SetRequestedHeight(PREVIEW_SIZE)
                    .SetCropToMask(true);

    mStatusLabel = MakeStatusLabel("Mask: none | MaskingMode: ON_RENDERING");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL)
                            .SetRequestedWidth(MATCH_PARENT)
                            .SetRequestedHeight(WRAP_CONTENT)
                            .SetBackgroundColor(UiColor(C_BG))
                            .SetPadding(Extents(8, 8, 8, 8));

    // Column labels
    StackLayout colLabels = StackLayout::New(StackOrientation::HORIZONTAL)
                              .SetRequestedWidth(MATCH_PARENT)
                              .SetRequestedHeight(LABEL_H);
    colLabels.Add(
      Label::New("CropToMask: OFF")
        .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
        .SetRequestedHeight(LABEL_H)
        .SetFontSize(FONT_LABEL)
        .SetTextColor(UiColor(C_STATUS_TEXT))
        .SetBackgroundColor(UiColor(C_LABEL_BG))
        .SetHorizontalTextAlignment(Text::Alignment::CENTER)
        .SetVerticalTextAlignment(Text::Alignment::CENTER));
    colLabels.Add(View::New().SetRequestedWidth(8).SetRequestedHeight(LABEL_H)); // spacer
    colLabels.Add(
      Label::New("CropToMask: ON")
        .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
        .SetRequestedHeight(LABEL_H)
        .SetFontSize(FONT_LABEL)
        .SetTextColor(UiColor(C_STATUS_TEXT))
        .SetBackgroundColor(UiColor(C_LABEL_BG))
        .SetHorizontalTextAlignment(Text::Alignment::CENTER)
        .SetVerticalTextAlignment(Text::Alignment::CENTER));
    content.Add(colLabels);

    // Preview row (side by side)
    StackLayout previewRow = StackLayout::New(StackOrientation::HORIZONTAL)
                               .SetRequestedWidth(MATCH_PARENT)
                               .SetRequestedHeight(PREVIEW_SIZE + 16)
                               .SetPadding(Extents(0, 0, 4, 4));
    previewRow.Add(StackLayout::New(StackOrientation::HORIZONTAL)
                     .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                     .SetRequestedHeight(PREVIEW_SIZE)
                     .Children({
                       View::New().SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f)),
                       mImageLeft,
                       View::New().SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f)),
                     }));
    previewRow.Add(View::New().SetRequestedWidth(8).SetRequestedHeight(PREVIEW_SIZE)); // spacer
    previewRow.Add(StackLayout::New(StackOrientation::HORIZONTAL)
                     .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                     .SetRequestedHeight(PREVIEW_SIZE)
                     .Children({
                       View::New().SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f)),
                       mImageRight,
                       View::New().SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f)),
                     }));

    content.Add(previewRow);
    content.Add(mStatusLabel);

    // Row 1: Mask selection
    content.Add(MakeButtonRow({
      MakeButton("Circle Mask", [this] { OnSetMask(IMG_MASK_CIRCLE, "circle"); }),
      MakeButton("Bubble Mask", [this] { OnSetMask(IMG_MASK_BUBBLE, "bubble"); }),
      MakeButton("No Mask",     [this] { OnSetMask("", "none"); }),
    }));

    // Row 2: MaskingMode
    content.Add(MakeButtonRow({
      MakeButton("MaskingMode\nON_RENDERING", [this] { OnMaskingMode(Ui::Image::MaskingType::MASKING_ON_RENDERING); }),
      MakeButton("MaskingMode\nON_LOADING",   [this] { OnMaskingMode(Ui::Image::MaskingType::MASKING_ON_LOADING); }),
    }));

    contentArea.Add(
      ScrollView::New()
        .SetScrollDirection(ScrollDirection::Vertical)
        .SetRequestedWidth(MATCH_PARENT)
        .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
        .SetContent(content));
  }

private:
  void OnSetMask(const Dali::String& maskUrl, const char* displayName)
  {
    mMaskUrl = maskUrl;
    mImageLeft.SetAlphaMaskUrl(maskUrl);
    mImageLeft.SetMaskingMode(mMaskingMode);
    mImageRight.SetAlphaMaskUrl(maskUrl);
    mImageRight.SetMaskingMode(mMaskingMode);
    UpdateStatus(displayName);
  }

  void OnMaskingMode(Ui::Image::MaskingType mode)
  {
    mMaskingMode = mode;
    mImageLeft.SetMaskingMode(mode);
    mImageRight.SetMaskingMode(mode);
    // Re-apply mask to trigger reload with new mode
    if(!mMaskUrl.Empty())
    {
      mImageLeft.SetAlphaMaskUrl(mMaskUrl);
      mImageRight.SetAlphaMaskUrl(mMaskUrl);
    }
    Dali::String modeStr = (mode == Ui::Image::MaskingType::MASKING_ON_RENDERING) ? "ON_RENDERING" : "ON_LOADING";
    UpdateStatus(modeStr.CStr());
  }

  void UpdateStatus(const char* maskName)
  {
    Dali::String modeStr = (mMaskingMode == Ui::Image::MaskingType::MASKING_ON_RENDERING) ? "ON_RENDERING" : "ON_LOADING";
    Dali::String maskStr = mImageLeft.GetAlphaMaskUrl().Empty() ? "none" : Dali::String(maskName);

    mStatusLabel.SetText(
      Dali::String("Mask: ") + maskStr +
      Dali::String(" | MaskingMode: ") + modeStr +
      Dali::String(" | CropL: ") + Dali::String(mImageLeft.IsCropToMask() ? "ON" : "OFF") +
      Dali::String(" | CropR: ") + Dali::String(mImageRight.IsCropToMask() ? "ON" : "OFF"));
  }

  // -----------------------------------------------------------------------
  // UI helpers
  // -----------------------------------------------------------------------

  Label MakeStatusLabel(const Dali::String& text)
  {
    return Label::New(text)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(STATUS_H)
      .SetFontSize(FONT_STATUS)
      .SetTextColor(UiColor(C_STATUS_TEXT))
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
                            .SetRequestedWidth(MATCH_PARENT)
                            .SetRequestedHeight(MATCH_PARENT)
                            .SetFontSize(FONT_BTN)
                            .SetTextColor(UiColor(C_BTN_TEXT))
                            .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                            .SetVerticalTextAlignment(Text::Alignment::CENTER)
                            .SetMultiLine(true),
                        });
    btn.SetFocusable(true).AsInteractive([this, onClick = std::move(onClick)](InteractiveTrait& trait) {
      trait.ClickedSignal().Connect(this, [onClick](View, const InputEvent&) -> bool {
        onClick();
        return true;
      });
    });
    return btn;
  }

  StackLayout MakeButtonRow(std::initializer_list<View> buttons)
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(BTN_H)
                        .SetPadding(Extents(0, 0, 2, 2));
    for(auto& b : buttons)
    {
      row.Add(b);
    }
    return row;
  }

  // -----------------------------------------------------------------------
  // Members
  // -----------------------------------------------------------------------

  ImageView             mImageLeft;
  ImageView             mImageRight;
  Label                 mStatusLabel;
  Dali::String          mMaskUrl;
  Ui::Image::MaskingType mMaskingMode{Ui::Image::MaskingType::MASKING_ON_RENDERING};
};

REGISTER_MANUAL_TEST(TcImageViewMasking)
