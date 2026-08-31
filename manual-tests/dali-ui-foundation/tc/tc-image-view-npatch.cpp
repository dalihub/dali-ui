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
// 9-patch material, 249x169 -> 247x167 after the guide frame is cropped.
// Auto-detected border is left 31 / right 30 / top 31 / bottom 30, so every corner
// heart is large enough to see whether it stays unstretched.
const char* const IMG_NPATCH1 = TEST_RESOURCE_DIR "/heartsframe.9.png";
// Same artwork with the 1px guide frame removed. A manual BORDER only makes sense on a
// plain image: NPatchData crops the guide frame on the auto path only, so forcing a
// border onto a .9.png leaves the guide pixels inside the rendered texture.
const char* const IMG_PLAIN = TEST_RESOURCE_DIR "/heartsframe-plain.png";
// Kept for comparison: 25x29 -> 23x27 content. Too small for a meaningful border test.
const char* const IMG_SMALL = TEST_RESOURCE_DIR "/button-up-1.9.png";

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

// Every size keeps both stretch regions positive for the 31/30 border:
// width  > 61 and height > 61, so the centre never collapses.
struct SizeEntry { float w, h; const char* label; };
const SizeEntry SIZES[] = {
  {120,  90, "120x90"},   // smaller than natural: centre squeezed to 59x29
  {250, 170, "250x170"},  // natural size (247x167)
  {400, 200, "400x200"},  // stretched mostly horizontally
  {560, 260, "560x260"},  // stretched both ways
};
constexpr int SIZE_COUNT = 4;

// Vector4 order is left, right, top, bottom. Both manual values stay inside the smallest
// preview (120x90) and, more importantly, inside the artwork's own frame, which reaches
// about 25-26px on every side. A border wider than that pulls the white interior into the
// border band, which reads as broken rendering even though the geometry is exact.
// The symmetric value is deliberately NOT 30: the auto-detected border is 31/30/31/30, so
// B(30) would render identically to Auto and prove nothing about the override.
struct BorderEntry { Vector4 border; const char* label; };
const BorderEntry BORDERS[] = {
  {Vector4(0,0,0,0),      "Auto"},
  {Vector4(20,20,20,20),  "B(20)"},
  {Vector4(30,20,30,20),  "B(30,20)"},
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
    mCurrentW     = SIZES[1].w;
    mCurrentH     = SIZES[1].h;
    mBorderOnly   = false;
    mImageName    = "heartsframe.9.png";

    mPreviewBg = View::New();
    mPreviewBg.SetRequestedWidth(mCurrentW);
    mPreviewBg.SetRequestedHeight(mCurrentH);
    mPreviewBg.SetBackgroundColor(UiColor(C_PREVIEW_BG));

    mImage = ImageView::New(IMG_NPATCH1);
    mImage.SetRequestedWidth(mCurrentW);
    mImage.SetRequestedHeight(mCurrentH);
    mImage.SetAccessibilityName("ImagePreview");
    mImage.SetAccessibilityRole(Accessibility::Role::IMAGE);

    mPreviewBg.Add(mImage);

    mStatusLabel = MakeStatusLabel("Img: heartsframe.9.png | Size: 250x170 | Border: auto | BorderOnly: OFF");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetBackgroundColor(UiColor(C_BG));
    content.SetPadding(Insets(8.0f, 8.0f, 8.0f, 8.0f));

    // Preview row
    StackLayout previewRow = StackLayout::New(StackOrientation::HORIZONTAL);
    previewRow.SetRequestedWidth(MATCH_PARENT);
    previewRow.SetRequestedHeight(280);
    previewRow.SetPadding(Insets(0.0f, 0.0f, 8.0f, 8.0f));
    previewRow.Add(ManualTest::MakeWeightedSpacer());
    previewRow.Add(mPreviewBg);
    previewRow.Add(ManualTest::MakeWeightedSpacer());

    content.Add(previewRow);
    content.Add(mStatusLabel);

    // Size buttons
    content.Add(MakeButtonRow({
      MakeButton("120x90",  [this] { OnSetSize(0); }),
      MakeButton("250x170", [this] { OnSetSize(1); }),
      MakeButton("400x200", [this] { OnSetSize(2); }),
      MakeButton("560x260", [this] { OnSetSize(3); }),
    }));

    // BorderOnly toggle
    content.Add(MakeButtonRow({
      MakeButton("BorderOnly ON",  [this] { OnBorderOnly(true); }),
      MakeButton("BorderOnly OFF", [this] { OnBorderOnly(false); }),
    }));

    // Manual border override
    content.Add(MakeButtonRow({
      MakeButton("Border: Auto",     [this] { OnSetBorder(0); }),
      MakeButton("Border: (20)",     [this] { OnSetBorder(1); }),
      MakeButton("Border: (30,20)",  [this] { OnSetBorder(2); }),
    }));

    // Image switch
    content.Add(MakeButtonRow({
      MakeButton("heartsframe.9.png",     [this] { OnSetImage(IMG_NPATCH1, "heartsframe.9.png"); }),
      MakeButton("heartsframe-plain.png", [this] { OnSetImage(IMG_PLAIN,   "heartsframe-plain.png"); }),
      MakeButton("button-up-1.9.png",     [this] { OnSetImage(IMG_SMALL,   "button-up-1.9.png"); }),
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

  void OnSetImage(const char* url, const char* name)
  {
    mImageName = name;
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

    // Read the size back from the component, not from the app's own member: a label that
    // echoes mCurrentW would stay green even if SetRequestedWidth did nothing.
    mStatusLabel.SetText(
      Dali::String("Img: ") + Dali::String(mImageName) +
      Dali::String(" | Size: ") + Dali::String(std::to_string((int)mImage.GetRequestedWidth()).c_str()) +
      Dali::String("x") + Dali::String(std::to_string((int)mImage.GetRequestedHeight()).c_str()) +
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
  float       mCurrentW{250.f};
  float       mCurrentH{170.f};
  bool        mBorderOnly{false};
  const char* mImageName{"heartsframe.9.png"};
};

REGISTER_MANUAL_TEST(TcImageViewNPatch)
