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
    mImageLeft = ImageView::New(IMG_A);
    mImageLeft.SetRequestedWidth(PREVIEW_SIZE);
    mImageLeft.SetRequestedHeight(PREVIEW_SIZE);
    mImageLeft.SetCropToMask(false);

    // Right: CropToMask ON
    mImageRight = ImageView::New(IMG_A);
    mImageRight.SetRequestedWidth(PREVIEW_SIZE);
    mImageRight.SetRequestedHeight(PREVIEW_SIZE);
    mImageRight.SetCropToMask(true);

    mStatusLabel = MakeStatusLabel("Mask: none | MaskingMode: ON_RENDERING");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetBackgroundColor(UiColor(C_BG));
    content.SetPadding(Extents(8, 8, 8, 8));

    // Column labels
    StackLayout colLabels = StackLayout::New(StackOrientation::HORIZONTAL);
    colLabels.SetRequestedWidth(MATCH_PARENT);
    colLabels.SetRequestedHeight(LABEL_H);
    Label cropOffLabel = Label::New("CropToMask: OFF");
    cropOffLabel.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    cropOffLabel.SetRequestedHeight(LABEL_H);
    cropOffLabel.SetFontSize(FONT_LABEL);
    cropOffLabel.SetTextColor(UiColor(C_STATUS_TEXT));
    cropOffLabel.SetBackgroundColor(UiColor(C_LABEL_BG));
    cropOffLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    cropOffLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    colLabels.Add(cropOffLabel);

    View labelSpacer = View::New();
    labelSpacer.SetRequestedWidth(8);
    labelSpacer.SetRequestedHeight(LABEL_H);
    colLabels.Add(labelSpacer);

    Label cropOnLabel = Label::New("CropToMask: ON");
    cropOnLabel.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    cropOnLabel.SetRequestedHeight(LABEL_H);
    cropOnLabel.SetFontSize(FONT_LABEL);
    cropOnLabel.SetTextColor(UiColor(C_STATUS_TEXT));
    cropOnLabel.SetBackgroundColor(UiColor(C_LABEL_BG));
    cropOnLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    cropOnLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    colLabels.Add(cropOnLabel);
    content.Add(colLabels);

    // Preview row (side by side)
    StackLayout previewRow = StackLayout::New(StackOrientation::HORIZONTAL);
    previewRow.SetRequestedWidth(MATCH_PARENT);
    previewRow.SetRequestedHeight(PREVIEW_SIZE + 16);
    previewRow.SetPadding(Extents(0, 0, 4, 4));
    View leftLeadingSpacer = View::New();
    leftLeadingSpacer.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    View leftTrailingSpacer = View::New();
    leftTrailingSpacer.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));

    StackLayout leftPreview = StackLayout::New(StackOrientation::HORIZONTAL);
    leftPreview.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    leftPreview.SetRequestedHeight(PREVIEW_SIZE);
    leftPreview.Add(leftLeadingSpacer);
    leftPreview.Add(mImageLeft);
    leftPreview.Add(leftTrailingSpacer);
    previewRow.Add(leftPreview);

    View previewSpacer = View::New();
    previewSpacer.SetRequestedWidth(8);
    previewSpacer.SetRequestedHeight(PREVIEW_SIZE);
    previewRow.Add(previewSpacer);

    View rightLeadingSpacer = View::New();
    rightLeadingSpacer.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    View rightTrailingSpacer = View::New();
    rightTrailingSpacer.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));

    StackLayout rightPreview = StackLayout::New(StackOrientation::HORIZONTAL);
    rightPreview.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    rightPreview.SetRequestedHeight(PREVIEW_SIZE);
    rightPreview.Add(rightLeadingSpacer);
    rightPreview.Add(mImageRight);
    rightPreview.Add(rightTrailingSpacer);
    previewRow.Add(rightPreview);

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

    ScrollView scrollView = ScrollView::New();
    scrollView.SetScrollDirection(ScrollDirection::Vertical);
    scrollView.SetRequestedWidth(MATCH_PARENT);
    scrollView.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    scrollView.SetContent(content);
    contentArea.Add(scrollView);
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
    Label label = Label::New(text);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(STATUS_H);
    label.SetFontSize(FONT_STATUS);
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

    buttonLabel.SetFontSize(FONT_BTN);

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
    row.SetPadding(Extents(0, 0, 2, 2));
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
