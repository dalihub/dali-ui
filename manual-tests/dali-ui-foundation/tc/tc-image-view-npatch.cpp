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

    mPreviewBg = View::New()
                   .SetRequestedWidth(mCurrentW)
                   .SetRequestedHeight(mCurrentH)
                   .SetBackgroundColor(UiColor(C_PREVIEW_BG));

    mImage = ImageView::New(IMG_NPATCH1)
               .SetRequestedWidth(mCurrentW)
               .SetRequestedHeight(mCurrentH);

    mPreviewBg.Add(mImage);

    mStatusLabel = MakeStatusLabel("Size: 300x80 | Border: auto | BorderOnly: OFF");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL)
                            .SetRequestedWidth(MATCH_PARENT)
                            .SetRequestedHeight(WRAP_CONTENT)
                            .SetBackgroundColor(UiColor(C_BG))
                            .SetPadding(Extents(8, 8, 8, 8));

    // Preview row
    StackLayout previewRow = StackLayout::New(StackOrientation::HORIZONTAL)
                               .SetRequestedWidth(MATCH_PARENT)
                               .SetRequestedHeight(140)
                               .SetPadding(Extents(0, 0, 8, 8));
    previewRow.Add(View::New().SetRequestedWidth(WRAP_CONTENT).SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f)));
    previewRow.Add(mPreviewBg);
    previewRow.Add(View::New().SetRequestedWidth(WRAP_CONTENT).SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f)));

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

    contentArea.Add(
      ScrollView::New()
        .SetScrollDirection(ScrollDirection::Vertical)
        .SetRequestedWidth(MATCH_PARENT)
        .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
        .SetContent(content));
  }

private:
  void OnSetSize(int idx)
  {
    mCurrentW = SIZES[idx].w;
    mCurrentH = SIZES[idx].h;
    mImage.SetRequestedWidth(mCurrentW).SetRequestedHeight(mCurrentH);
    mPreviewBg.SetRequestedWidth(mCurrentW).SetRequestedHeight(mCurrentH);
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

  ImageView mImage;
  View      mPreviewBg;
  Label     mStatusLabel;
  float     mCurrentW{300.f};
  float     mCurrentH{80.f};
  bool      mBorderOnly{false};
};

REGISTER_MANUAL_TEST(TcImageViewNPatch)
