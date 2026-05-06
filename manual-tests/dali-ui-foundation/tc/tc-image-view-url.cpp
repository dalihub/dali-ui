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
const char* const IMG_A = TEST_RESOURCE_DIR "/gallery-large-3.jpg";
const char* const IMG_B = TEST_RESOURCE_DIR "/landscape-sample.jpg";

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
    return "ImageView: URL / Reload / ResourceReady";
  }

  Dali::String GetDescription() const override
  {
    return "Verify URL change, Reload, and ResourceReadySignal counter";
  }

  void OnEnter(View contentArea) override
  {
    mReadyCount = 0;

    mImage = ImageView::New(IMG_A)
               .SetRequestedWidth(PREVIEW_SIZE)
               .SetRequestedHeight(PREVIEW_SIZE);

    mImage.ResourceReadySignal().Connect(this, [this](View) {
      ++mReadyCount;
      UpdateLabels();
    });

    mUrlLabel    = MakeStatusLabel("URL: A");
    mStatusLabel = MakeStatusLabel("Status: - | ResourceReadySignal: 0");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL)
                            .SetRequestedWidth(MATCH_PARENT)
                            .SetRequestedHeight(WRAP_CONTENT)
                            .SetBackgroundColor(UiColor(C_BG))
                            .SetPadding(Extents(8, 8, 8, 8));

    content.Add(MakeCenteredPreview(mImage));
    content.Add(mUrlLabel);
    content.Add(mStatusLabel);
    content.Add(MakeButtonRow({
      MakeButton("Set\nURL A", [this] { OnSetUrl(IMG_A, "A"); }),
      MakeButton("Set\nURL B", [this] { OnSetUrl(IMG_B, "B"); }),
      MakeButton("Reload",     [this] { mImage.Reload(); }),
    }));

    contentArea.Add(content);
  }

private:
  void OnSetUrl(const char* url, const char* name)
  {
    mCurrentName = name;
    mImage.SetResourceUrl(url);
    UpdateLabels();
  }

  void UpdateLabels()
  {
    mUrlLabel.SetText(Dali::String("SetResourceUrl: ") + Dali::String(mCurrentName) +
                      Dali::String(" | GetResourceUrl match: ") +
                      Dali::String(mImage.GetResourceUrl() == mImage.GetResourceUrl() ? "OK" : "NG"));

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
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(PREVIEW_SIZE + 16)
                        .SetPadding(Extents(0, 0, 8, 8));
    row.Add(View::New().SetRequestedWidth(WRAP_CONTENT).SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f)));
    row.Add(preview);
    row.Add(View::New().SetRequestedWidth(WRAP_CONTENT).SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f)));
    return row;
  }

  Label MakeStatusLabel(const Dali::String& text)
  {
    return Label::New(text)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(STATUS_H)
      .SetFontSize(12.0f)
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
                            .SetFontSize(12.0f)
                            .SetTextColor(UiColor(C_BTN_TEXT))
                            .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                            .SetVerticalTextAlignment(Text::Alignment::CENTER)
                            .SetMultiLine(true),
                        });
    btn.SetFocusable(true).AsInteractive([this, onClick = std::move(onClick)](InteractiveTrait& trait) {
      trait.ClickedSignal().Connect(this, [onClick](View, InputEvent) -> bool {
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
    for(auto& b : buttons) row.Add(b);
    return row;
  }

  ImageView    mImage;
  Label        mUrlLabel;
  Label        mStatusLabel;
  int          mReadyCount{0};
  const char*  mCurrentName{"A"};
};

REGISTER_MANUAL_TEST(TcImageViewUrl)
