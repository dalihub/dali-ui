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
const char* const IMG_NPATCH1 = TEST_RESOURCE_DIR "/button-up-1.9.png";
const char* const IMG_NPATCH2 = TEST_RESOURCE_DIR "/tooltip.9.png";

constexpr float    BTN_H         = 52.0f;
constexpr float    STATUS_H      = 32.0f;
constexpr float    FONT_BTN      = 13.0f;
constexpr float    FONT_STATUS   = 13.0f;
constexpr uint32_t C_BTN_BG      = 0x555555;
constexpr uint32_t C_BTN_TEXT    = 0xEEEEEE;
constexpr uint32_t C_STATUS_BG   = 0x222222;
constexpr uint32_t C_STATUS_TEXT = 0xCCCCCC;
constexpr uint32_t C_BG          = 0x1A1A1A;
constexpr uint32_t C_PREVIEW_BG  = 0x444444; // visible background for BorderOnly test

struct SizeEntry { float w, h; const char* label; };
const SizeEntry SIZES[] = {
  {100, 40,  "100x40"},
  {200, 60,  "200x60"},
  {300, 80,  "300x80"},
  {400, 120, "400x120"},
};
constexpr int SIZE_COUNT = 4;

struct BorderEntry { Vector4 border; const char* label; };
const BorderEntry BORDERS[] = {
  {Vector4(0,0,0,0),   "Auto"},
  {Vector4(10,10,10,10), "B(10)"},
  {Vector4(20,5,20,5),   "B(20,5)"},
};
constexpr int BORDER_COUNT = 3;
} // namespace

/**
 * @brief Verifies ImageView N-Patch border API:
 *   SetNPatchBorder / GetNPatchBorder
 *   SetNPatchBorderOnly / IsNPatchBorderOnly
 *
 * Resize the view to confirm that the border region stays fixed while the
 * centre stretches.  Enable BorderOnly to confirm the centre is transparent.
 */
class TcImageViewNPatch : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "ImageView N-Patch Border";
  }

  Dali::String GetDescription() const override
  {
    return "Verifies NPatchBorder sizing and BorderOnly rendering";
  }

  void OnEnter(View contentArea) override
  {
    mCurrentW     = SIZES[2].w;
    mCurrentH     = SIZES[2].h;
    mBorderOnly   = false;

    mPreviewBg = View::New();
    mPreviewBg.SetRequestedWidth(mCurrentW);
    mPreviewBg.SetRequestedHeight(mCurrentH);
    mPreviewBg.SetBackgroundColor(UiColor(C_PREVIEW_BG));

    mImage = ImageView::New(IMG_NPATCH1);
    mImage.SetRequestedWidth(mCurrentW);
    mImage.SetRequestedHeight(mCurrentH);

    mPreviewBg.Add(mImage);

    mStatusLabel = MakeStatusLabel("Size: 300x80 | Border: auto | BorderOnly: OFF");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetBackgroundColor(UiColor(C_BG));
    content.SetPadding(Insets(8.0f, 8.0f, 8.0f, 8.0f));

    // Preview row
    StackLayout previewRow = StackLayout::New(StackOrientation::HORIZONTAL);
    previewRow.SetRequestedWidth(MATCH_PARENT);
    previewRow.SetRequestedHeight(140);
    previewRow.SetPadding(Insets(0.0f, 0.0f, 8.0f, 8.0f));
    previewRow.Add(ManualTest::MakeWeightedSpacer());
    previewRow.Add(mPreviewBg);
    previewRow.Add(ManualTest::MakeWeightedSpacer());

    content.Add(previewRow);
    content.Add(mStatusLabel);

    // Size buttons
    content.Add(MakeButtonRow({
      MakeButton("100x40",  [this] { OnSetSize(0); }),
      MakeButton("200x60",  [this] { OnSetSize(1); }),
      MakeButton("300x80",  [this] { OnSetSize(2); }),
      MakeButton("400x120", [this] { OnSetSize(3); }),
    }));

    // BorderOnly toggle
    content.Add(MakeButtonRow({
      MakeButton("BorderOnly ON",  [this] { OnBorderOnly(true); }),
      MakeButton("BorderOnly OFF", [this] { OnBorderOnly(false); }),
    }));

    // Manual border override
    content.Add(MakeButtonRow({
      MakeButton("Border: Auto",    [this] { OnSetBorder(0); }),
      MakeButton("Border: (10)",    [this] { OnSetBorder(1); }),
      MakeButton("Border: (20,5)",  [this] { OnSetBorder(2); }),
    }));

    // Image switch
    content.Add(MakeButtonRow({
      MakeButton("button-up-1.9.png", [this] { OnSetImage(IMG_NPATCH1); }),
      MakeButton("tooltip.9.png",     [this] { OnSetImage(IMG_NPATCH2); }),
    }));

    ScrollView scrollView = ScrollView::New();
    scrollView.SetScrollDirection(ScrollDirection::Vertical);
    scrollView.SetRequestedWidth(MATCH_PARENT);
    scrollView.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    scrollView.SetContent(content);
    contentArea.Add(scrollView);
  }

private:
  void OnSetSize(int idx)
  {
    mCurrentW = SIZES[idx].w;
    mCurrentH = SIZES[idx].h;
    mImage.SetRequestedWidth(mCurrentW);
    mImage.SetRequestedHeight(mCurrentH);
    mPreviewBg.SetRequestedWidth(mCurrentW);
    mPreviewBg.SetRequestedHeight(mCurrentH);
    UpdateStatus();
  }

  void OnBorderOnly(bool enabled)
  {
    mBorderOnly = enabled;
    mImage.SetNPatchBorderOnly(enabled);
    UpdateStatus();
  }

  void OnSetBorder(int idx)
  {
    mImage.SetNPatchBorder(BORDERS[idx].border);
    UpdateStatus();
  }

  void OnSetImage(const char* url)
  {
    mImage.SetResourceUrl(url);
    UpdateStatus();
  }

  void UpdateStatus()
  {
    Vector4 b = mImage.GetNPatchBorder();
    Dali::String borderStr;
    if(b.x == 0.f && b.y == 0.f && b.z == 0.f && b.w == 0.f)
    {
      borderStr = "auto";
    }
    else
    {
      borderStr = Dali::String("(") +
                  Dali::String(std::to_string((int)b.x).c_str()) + Dali::String(",") +
                  Dali::String(std::to_string((int)b.y).c_str()) + Dali::String(",") +
                  Dali::String(std::to_string((int)b.z).c_str()) + Dali::String(",") +
                  Dali::String(std::to_string((int)b.w).c_str()) + Dali::String(")");
    }

    mStatusLabel.SetText(
      Dali::String("Size: ") + Dali::String(std::to_string((int)mCurrentW).c_str()) +
      Dali::String("x") + Dali::String(std::to_string((int)mCurrentH).c_str()) +
      Dali::String(" | Border: ") + borderStr +
      Dali::String(" | BorderOnly: ") +
      Dali::String(mImage.IsNPatchBorderOnly() ? "ON" : "OFF"));
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
    row.SetPadding(Insets(0.0f, 0.0f, 2.0f, 2.0f));
    for(auto& b : buttons)
    {
      row.Add(b);
    }
    return row;
  }

  // -----------------------------------------------------------------------
  // Members
  // -----------------------------------------------------------------------

  ImageView mImage;
  View      mPreviewBg;
  Label     mStatusLabel;
  float     mCurrentW{300.f};
  float     mCurrentH{80.f};
  bool      mBorderOnly{false};
};

REGISTER_MANUAL_TEST(TcImageViewNPatch)
