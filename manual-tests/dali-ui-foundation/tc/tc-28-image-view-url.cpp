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
const char* const IMG_A       = TEST_RESOURCE_DIR "/gallery-large-3.jpg";
const char* const IMG_B       = TEST_RESOURCE_DIR "/landscape-sample.jpg";
// A path that cannot load: Status: FAILED is the control that gives the READY
// assertions their meaning — a GetLoadingStatus() stub returning READY passes
// every green-path step (review 28).
const char* const IMG_MISSING = TEST_RESOURCE_DIR "/definitely-not-here.jpg";

constexpr float    PREVIEW_SIZE  = 200.0f;
constexpr float    BTN_H         = 52.0f;
constexpr float    STATUS_H      = 32.0f;
constexpr uint32_t C_BTN_BG      = 0x555555;
constexpr uint32_t C_BTN_TEXT    = 0xEEEEEE;
constexpr uint32_t C_STATUS_BG   = 0x222222;
constexpr uint32_t C_STATUS_TEXT = 0xCCCCCC;
constexpr uint32_t C_BG          = 0x1A1A1A;
} // namespace

/**
 * @brief Verifies ImageView URL / Reload / ResourceReadySignal.
 *
 * Steps:
 *   1. [Set URL A] -> Switch to image A; verify URL label is updated.
 *   2. [Set URL B] -> Switch to image B and verify.
 *   3. [Reload]    -> ResourceReadySignal counter increments by 1.
 *
 * Expected result:
 *   Image is swapped immediately on URL change; GetResourceUrl() returns the new URL.
 *   Calling Reload fires ResourceReadySignal once more.
 */
class TcImageViewUrl : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "28. ImageView: URL / Reload / ResourceReady";
  }

  Dali::String GetDescription() const override
  {
    return "Verify URL change, Reload, and ResourceReadySignal counter";
  }

  void OnEnter(View contentArea) override
  {
    mReadyCount = 0;

    mImage = ImageView::New(IMG_A);
    mImage.SetRequestedWidth(PREVIEW_SIZE);
    mImage.SetRequestedHeight(PREVIEW_SIZE);
    mImage.SetAccessibilityName("ImagePreview");
    mImage.SetAccessibilityRole(Accessibility::Role::IMAGE);
    mCurrentUrl = IMG_A;

    mImage.ResourceReadySignal().Connect(this, [this](View) {
      ++mReadyCount;
      UpdateLabels();
    });

    mUrlLabel    = MakeStatusLabel("URL: A");
    mStatusLabel = MakeStatusLabel("Status: - | ResourceReadySignal: 0");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetBackgroundColor(UiColor(C_BG));
    content.SetPadding(Insets(8.0f, 8.0f, 8.0f, 8.0f));

    content.Add(MakeCenteredPreview(mImage));
    content.Add(mUrlLabel);
    content.Add(mStatusLabel);
    content.Add(MakeButtonRow({
      MakeButton("Set\nURL A",       [this] { OnSetUrl(IMG_A, "A"); }),
      MakeButton("Set\nURL B",       [this] { OnSetUrl(IMG_B, "B"); }),
      MakeButton("Set URL\nMissing", [this] { OnSetUrl(IMG_MISSING, "Missing"); }),
      MakeButton("Reload",           [this] { mImage.Reload(); }),
    }));

    contentArea.Add(content);
  }

private:
  void OnSetUrl(const char* url, const char* name)
  {
    mCurrentName = name;
    mCurrentUrl  = url;
    mImage.SetResourceUrl(url);
    UpdateLabels();
  }

  void UpdateLabels()
  {
    // Compare the getter against the URL that was just SET — the old line
    // compared GetResourceUrl() with itself, so OK was unconditional and the
    // getter could return garbage unnoticed (review 28; the sibling fitting
    // and sampling screens always had the real comparison).
    mUrlLabel.SetText(Dali::String("SetResourceUrl: ") + Dali::String(mCurrentName) +
                      Dali::String(" | GetResourceUrl match: ") +
                      Dali::String(mImage.GetResourceUrl() == mCurrentUrl ? "OK" : "NG"));

    Dali::String stStr;
    switch(mImage.GetLoadingStatus())
    {
      case Ui::Visual::ResourceStatus::PREPARING: stStr = "PREPARING"; break;
      case Ui::Visual::ResourceStatus::READY:     stStr = "READY";     break;
      case Ui::Visual::ResourceStatus::FAILED:    stStr = "FAILED";    break;
      default:                                    stStr = "UNKNOWN";   break;
    }
    mStatusLabel.SetText(Dali::String("Status: ") + stStr +
                         Dali::String(" | ResourceReadySignal: ") +
                         Dali::String(std::to_string(mReadyCount).c_str()));
  }

  View MakeCenteredPreview(View preview)
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(PREVIEW_SIZE + 16);
    row.SetPadding(Insets(0.0f, 0.0f, 8.0f, 8.0f));
    row.Add(ManualTest::MakeWeightedSpacer());
    row.Add(preview);
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
    for(auto& b : buttons) row.Add(b);
    return row;
  }

  ImageView    mImage;
  Label        mUrlLabel;
  Label        mStatusLabel;
  int          mReadyCount{0};
  const char*  mCurrentName{"A"};
  Dali::String mCurrentUrl;
};

REGISTER_MANUAL_TEST(TcImageViewUrl)
