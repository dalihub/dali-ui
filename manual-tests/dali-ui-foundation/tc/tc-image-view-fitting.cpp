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

using namespace Dali;
using namespace Dali::Ui;

namespace
{
// Intentional aspect-ratio mismatch: landscape box, portrait image
const char* const IMG = TEST_RESOURCE_DIR "/gallery-large-3.jpg";

constexpr float    BOX_W        = 280.0f;
constexpr float    BOX_H        = 160.0f;
constexpr float    BTN_H        = 52.0f;
constexpr float    STATUS_H     = 32.0f;
constexpr uint32_t C_BTN_BG     = 0x555555;
constexpr uint32_t C_BTN_ACT    = 0x1565C0;
constexpr uint32_t C_BTN_TEXT   = 0xEEEEEE;
constexpr uint32_t C_STATUS_BG  = 0x222222;
constexpr uint32_t C_STATUS_TEXT= 0xCCCCCC;
constexpr uint32_t C_BG         = 0x1A1A1A;
constexpr uint32_t C_BOX_BORDER = 0xFF4444; // box boundary indicator

struct FittingEntry
{
  const char*           name;
  Ui::Image::FittingMode mode;
};

const FittingEntry FITTING[] = {
  {"FIT_KEEP",  Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO},
  {"FILL",      Ui::Image::FittingMode::FILL},
  {"OVER_FIT",  Ui::Image::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO},
  {"CENTER",    Ui::Image::FittingMode::CENTER},
};
constexpr int FITTING_COUNT = 4;
} // namespace

/**
 * @brief Verifies ImageView FittingMode (SetFittingMode / GetFittingMode).
 *
 * The preview box is 280x160 (landscape). The image is portrait, creating an
 * aspect-ratio mismatch. The red border shows the view boundary.
 *
 * Steps:
 *   1. [FIT_KEEP]  -> aspect ratio preserved; letterbox padding visible.
 *   2. [FILL]      -> image stretched to fill the box; aspect ratio ignored.
 *   3. [OVER_FIT]  -> aspect ratio preserved; image crops outside the box.
 *   4. [CENTER]    -> original size, centred; may overflow or have padding.
 *
 * Expected result:
 *   Each mode produces a visually distinct result.
 *   GetFittingMode() returns the value that was set.
 */
class TcImageViewFitting : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "ImageView: FittingMode";
  }

  Dali::String GetDescription() const override
  {
    return "Visual comparison of 4 fitting modes (FIT_KEEP / FILL / OVER_FIT / CENTER)";
  }

  void OnEnter(View contentArea) override
  {
    mCurrentIdx = 0;

    StackLayout boxContainer = StackLayout::New(StackOrientation::VERTICAL);
    boxContainer.SetRequestedWidth(BOX_W);
    boxContainer.SetRequestedHeight(BOX_H);
    boxContainer.SetBackgroundColor(UiColor(C_BOX_BORDER));

    mImage = ImageView::New(IMG);
    mImage.SetRequestedWidth(BOX_W);
    mImage.SetRequestedHeight(BOX_H);
    mImage.SetFittingMode(FITTING[0].mode);
    mImage.SetMargin(Extents(2, 2, 2, 2));

    boxContainer.Add(mImage);

    mModeLabel = MakeStatusLabel("FittingMode: FIT_KEEP_ASPECT_RATIO");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetBackgroundColor(UiColor(C_BG));
    content.SetPadding(Extents(8, 8, 8, 8));

    content.Add(MakeCentered(boxContainer));
    content.Add(mModeLabel);

    StackLayout btnRow = StackLayout::New(StackOrientation::HORIZONTAL);
    btnRow.SetRequestedWidth(MATCH_PARENT);
    btnRow.SetRequestedHeight(BTN_H);
    btnRow.SetPadding(Extents(0, 0, 2, 2));
    for(int i = 0; i < FITTING_COUNT; ++i)
    {
      mBtns[i] = MakeToggleBtn(FITTING[i].name, i == 0);
      int idx   = i;
      mBtns[i].SetFocusable(true);

      InteractiveTrait interactive = mBtns[i].AsInteractive();

      interactive.ClickedSignal().Connect(this, [this, idx](View, InputEvent) -> bool {

        OnFitting(idx);

        return true;

      });
      btnRow.Add(mBtns[i]);
    }
    content.Add(btnRow);

    contentArea.Add(content);
  }

private:
  void OnFitting(int idx)
  {
    mCurrentIdx = idx;
    mImage.SetFittingMode(FITTING[idx].mode);
    mModeLabel.SetText(Dali::String("FittingMode: ") + Dali::String(FITTING[idx].name) +
                       Dali::String(" | GetFittingMode match: ") +
                       Dali::String(mImage.GetFittingMode() == FITTING[idx].mode ? "OK" : "NG"));
    for(int i = 0; i < FITTING_COUNT; ++i)
    {
      mBtns[i].SetBackgroundColor(UiColor(i == idx ? C_BTN_ACT : C_BTN_BG));
    }
  }

  View MakeCentered(View child)
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(BOX_H + 16);
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
    return label;
  }

  StackLayout MakeToggleBtn(const char* label, bool active)
  {
    StackLayout layout = StackLayout::New(StackOrientation::VERTICAL);
    layout.SetRequestedHeight(BTN_H);
    layout.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    layout.SetBackgroundColor(UiColor(active ? C_BTN_ACT : C_BTN_BG));
    Label buttonLabel = Label::New(label);
    buttonLabel.SetRequestedWidth(MATCH_PARENT);
    buttonLabel.SetRequestedHeight(MATCH_PARENT);
    buttonLabel.SetFontSize(11.0f);
    buttonLabel.SetTextColor(UiColor(C_BTN_TEXT));
    buttonLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    buttonLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    buttonLabel.SetMultiLine(true);
    layout.Add(buttonLabel);
    return layout;
  }

  ImageView   mImage;
  Label       mModeLabel;
  StackLayout mBtns[FITTING_COUNT];
  int         mCurrentIdx{0};
};

REGISTER_MANUAL_TEST(TcImageViewFitting)
