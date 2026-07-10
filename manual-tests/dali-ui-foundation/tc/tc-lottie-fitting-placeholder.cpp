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

#include <dali-ui-foundation/public-api/views/image/lottie-animation-view.h>
#include <dali/integration-api/debug.h>

#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
const char* const LOTTIE_WALKER    = TEST_RESOURCE_DIR "/jolly_walker.json";
const char* const PLACEHOLDER_IMG  = TEST_RESOURCE_DIR "/placeholder_image.png";

constexpr float    PREVIEW_W    = 240.0f;
constexpr float    PREVIEW_H    = 160.0f; // non-square to make fitting-mode differences visible
constexpr float    BTN_H        = 48.0f;
constexpr float    STATUS_H     = 28.0f;
constexpr uint32_t C_BTN_BG     = 0x555555;
constexpr uint32_t C_BTN_TEXT   = 0xEEEEEE;
constexpr uint32_t C_STATUS_BG  = 0x222222;
constexpr uint32_t C_STATUS_TEXT = 0xCCCCCC;
constexpr uint32_t C_BG         = 0x1A1A1A;
constexpr uint32_t C_FRAME_BG   = 0x2A2A2A;
} // namespace

/**
 * @brief Verifies LottieAnimationView PlaceholderUrl:
 *
 * [PlaceholderUrl verification]:
 *   1. [SetPlaceholder] → set placeholder image, then reload the Lottie URL
 *      → placeholder shows immediately while Lottie is loading
 *      → placeholder disappears once ResourceReady fires
 *   2. [ClearURL] → set URL to "" — placeholder shown (if set), main visual removed
 *   3. [ClearPlaceholder] → remove placeholder URL → nothing shown until URL is set again
 *
 * Expected result:
 *   Placeholder: shown while loading, removed on ResourceReady.
 *   GetPlaceholderUrl() returns the last-set value.
 */
class TcLottiePlaceholder : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "Lottie: PlaceholderUrl";
  }

  Dali::String GetDescription() const override
  {
    return "Verify PlaceholderUrl show/hide behaviour";
  }

  void OnEnter(View contentArea) override
  {
    mView = LottieAnimationView::New(LOTTIE_WALKER);
    mView.SetRequestedWidth(PREVIEW_W);
    mView.SetRequestedHeight(PREVIEW_H);
    mView.SetLoopCount(-1);

    mView.ResourceReadySignal().Connect(this, &TcLottiePlaceholder::OnResourceReady);
    mView.Play();

    mHolderLabel = MakeStatusLabel("Placeholder: (none)");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetBackgroundColor(UiColor(C_BG));
    content.SetPadding(Extents(8, 8, 8, 8));

    StackLayout frame = StackLayout::New(StackOrientation::VERTICAL);
    frame.SetRequestedWidth(MATCH_PARENT);
    frame.SetRequestedHeight(PREVIEW_H + 16);
    frame.SetBackgroundColor(UiColor(C_FRAME_BG));
    frame.SetPadding(Extents(0, 0, 8, 8));

    StackLayout centreRow = StackLayout::New(StackOrientation::HORIZONTAL);
    centreRow.SetRequestedWidth(MATCH_PARENT);
    centreRow.SetRequestedHeight(PREVIEW_H);
    centreRow.Add(ManualTest::MakeWeightedSpacer());
    centreRow.Add(mView);
    centreRow.Add(ManualTest::MakeWeightedSpacer());

    frame.Add(centreRow);
    content.Add(frame);
    content.Add(mHolderLabel);

    content.Add(MakeButtonRow({
      MakeButton("Set\nPlaceholder", [this] { OnSetPlaceholder(); }),
      MakeButton("Reload\nLottie",   [this] { OnReload(); }),
      MakeButton("Clear\nURL",       [this] { OnClearUrl(); }),
      MakeButton("Clear\nHolder",    [this] { OnClearPlaceholder(); }),
    }));

    contentArea.Add(content);
  }

private:
  void OnSetPlaceholder()
  {
    mView.SetPlaceholderUrl(PLACEHOLDER_IMG);
    mHolderLabel.SetText("Placeholder: placeholder_image.png");
  }

  void OnReload()
  {
    mView.SetResourceUrl(LOTTIE_WALKER);
    mView.Play();
    mHolderLabel.SetText("Placeholder: showing while loading...");
  }

  void OnClearUrl()
  {
    mView.SetResourceUrl("");
    mHolderLabel.SetText("URL cleared — placeholder visible (if set)");
  }

  void OnClearPlaceholder()
  {
    mView.SetPlaceholderUrl("");
    mHolderLabel.SetText("Placeholder: (none)");
  }

  void OnResourceReady(View)
  {
    mHolderLabel.SetText("ResourceReady — placeholder removed");
  }

  // ── Helpers ──────────────────────────────────────────────────────────────

  Label MakeStatusLabel(const Dali::String& text)
  {
    Label label = Label::New(text);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(STATUS_H);
    label.SetFontSize(11.0f);
    label.SetTextColor(UiColor(C_STATUS_TEXT));
    label.SetBackgroundColor(UiColor(C_STATUS_BG));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
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

    buttonLabel.SetFontSize(11.0f);

    buttonLabel.SetTextColor(UiColor(C_BTN_TEXT));

    buttonLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);

    buttonLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);

    buttonLabel.SetMultiLine(true);

    btn.Add(buttonLabel);
    btn.SetFocusable(true);

    InteractiveTrait interactive = btn.AsInteractive();

    interactive.ClickedSignal().Connect(this, [onClick = std::move(onClick)](View, const InputEvent&) -> bool {

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

  LottieAnimationView mView;
  Label               mHolderLabel;
};

REGISTER_MANUAL_TEST(TcLottiePlaceholder)
