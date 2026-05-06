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
const char* const IMG_A           = TEST_RESOURCE_DIR "/gallery-large-3.jpg";
const char* const IMG_B           = TEST_RESOURCE_DIR "/landscape-sample.jpg";
const char* const IMG_PLACEHOLDER = TEST_RESOURCE_DIR "/placeholder_image.png";

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
 * @brief Verifies ImageView SetPlaceholderUrl / GetPlaceholderUrl.
 *
 * Steps:
 *   1. [Set Placeholder] -> placeholder image is set; verify GetPlaceholderUrl returns the path.
 *   2. [Reload] -> placeholder is briefly shown while the main image reloads.
 *   3. [Switch to B] -> placeholder shown during load, then replaced by image B.
 *   4. [Clear Placeholder] -> GetPlaceholderUrl returns an empty string.
 *
 * Expected result:
 *   After setting a placeholder, triggering a load shows the placeholder image during
 *   loading, then replaces it with the actual image on completion.
 */
class TcImageViewPlaceholder : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "ImageView: PlaceholderUrl";
  }

  Dali::String GetDescription() const override
  {
    return "Verify placeholder is shown during load, then replaced by actual image";
  }

  void OnEnter(View contentArea) override
  {
    mReadyCount = 0;

    mImage = ImageView::New(IMG_A)
               .SetRequestedWidth(PREVIEW_SIZE)
               .SetRequestedHeight(PREVIEW_SIZE);

    mImage.ResourceReadySignal().Connect(this, [this](View) {
      ++mReadyCount;
      UpdateLabel();
    });

    mLabel = MakeStatusLabel("Placeholder: none | ResourceReadySignal: 0");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL)
                            .SetRequestedWidth(MATCH_PARENT)
                            .SetRequestedHeight(WRAP_CONTENT)
                            .SetBackgroundColor(UiColor(C_BG))
                            .SetPadding(Extents(8, 8, 8, 8));

    content.Add(MakeCenteredPreview(mImage));
    content.Add(mLabel);
    content.Add(MakeButtonRow({
      MakeButton("Set\nPlaceholder",   [this] { OnSetPlaceholder(IMG_PLACEHOLDER); }),
      MakeButton("Clear\nPlaceholder", [this] { OnSetPlaceholder(""); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Reload",      [this] { mImage.Reload(); }),
      MakeButton("Switch to B", [this] { mImage.SetResourceUrl(IMG_B); UpdateLabel(); }),
      MakeButton("Switch to A", [this] { mImage.SetResourceUrl(IMG_A); UpdateLabel(); }),
    }));

    contentArea.Add(content);
  }

private:
  void OnSetPlaceholder(const char* url)
  {
    mImage.SetPlaceholderUrl(url);
    UpdateLabel();
  }

  void UpdateLabel()
  {
    Dali::String ph = mImage.GetPlaceholderUrl();
    mLabel.SetText(
      Dali::String("Placeholder: ") + Dali::String(ph.Empty() ? "none" : "SET") +
      Dali::String(" | GetPlaceholderUrl: ") + Dali::String(ph.Empty() ? "(empty)" : ph) +
      Dali::String("\nResourceReadySignal: ") +
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
      .SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(STATUS_H)
      .SetFontSize(12.0f).SetTextColor(UiColor(C_STATUS_TEXT))
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
                            .SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(MATCH_PARENT)
                            .SetFontSize(12.0f).SetTextColor(UiColor(C_BTN_TEXT))
                            .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                            .SetVerticalTextAlignment(Text::Alignment::CENTER)
                            .SetMultiLine(true),
                        });
    btn.SetFocusable(true).AsInteractive([this, onClick = std::move(onClick)](InteractiveTrait& trait) {
      trait.ClickedSignal().Connect(this, [onClick](View, InputEvent) -> bool {
        onClick(); return true;
      });
    });
    return btn;
  }

  StackLayout MakeButtonRow(std::initializer_list<View> buttons)
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(BTN_H)
                        .SetPadding(Extents(0, 0, 2, 2));
    for(auto& b : buttons) row.Add(b);
    return row;
  }

  ImageView mImage;
  Label     mLabel;
  int       mReadyCount{0};
};

REGISTER_MANUAL_TEST(TcImageViewPlaceholder)
