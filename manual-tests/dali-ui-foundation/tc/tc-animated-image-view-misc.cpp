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

#include <dali-ui-foundation/public-api/animated-image-view.h>

#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
const char* const ANIM_WEBP      = TEST_RESOURCE_DIR "/dog-anim.webp";
const char* const ANIM_GIF       = TEST_RESOURCE_DIR "/dali-logo-anim.gif";
const char* const IMG_PLACEHOLDER= TEST_RESOURCE_DIR "/placeholder_image.png";

constexpr float    PREVIEW_SIZE  = 200.0f;
constexpr float    BTN_H         = 52.0f;
constexpr float    STATUS_H      = 36.0f;
constexpr uint32_t C_BTN_BG      = 0x555555;
constexpr uint32_t C_BTN_TEXT    = 0xEEEEEE;
constexpr uint32_t C_STATUS_BG   = 0x222222;
constexpr uint32_t C_STATUS_TEXT = 0xCCCCCC;
constexpr uint32_t C_BG          = 0x1A1A1A;
constexpr uint32_t C_IMG_BG      = 0xCC3333;

const char* SamplingModeName(Ui::Image::SamplingMode m)
{
  switch(m)
  {
    case Ui::Image::SamplingMode::BOX:              return "BOX";
    case Ui::Image::SamplingMode::NEAREST:          return "NEAREST";
    case Ui::Image::SamplingMode::LINEAR:           return "LINEAR";
    case Ui::Image::SamplingMode::BOX_THEN_NEAREST: return "BOX_THEN_NEAREST";
    case Ui::Image::SamplingMode::BOX_THEN_LINEAR:  return "BOX_THEN_LINEAR";
    case Ui::Image::SamplingMode::NO_FILTER:        return "NO_FILTER";
    case Ui::Image::SamplingMode::DONT_CARE:        return "DONT_CARE";
    default:                                 return "?";
  }
}
} // namespace

/**
 * @brief Verifies AnimatedImageView miscellaneous APIs not covered by other TCs:
 *   SetDepthIndex
 *   SetPreMultipliedAlpha / IsPreMultipliedAlpha
 *   SetDesiredWidth / GetDesiredWidth / SetDesiredHeight / GetDesiredHeight
 *   SetPlaceholderUrl / GetPlaceholderUrl
 *   SetImageLoadWithViewSize / IsImageLoadWithViewSizeEnabled
 *   SetSamplingMode / GetSamplingMode
 *
 * Steps:
 *   [DepthIndex]:
 *   1. [Depth: -1] -> animation drawn behind the red background fill.
 *   2. [Depth: 0]  -> default order; animation visible.
 *
 *   [DesiredSize]:
 *   1. [Desired 50x50]  -> rasterised at low resolution (blurry).
 *   2. [Desired 0x0]    -> use natural size.
 *
 *   [Placeholder]:
 *   1. [Set Placeholder] -> [Reload URL] -> placeholder briefly visible during loading.
 *   2. [Clear Placeholder] -> GetPlaceholderUrl returns empty.
 *
 *   [PreMultipliedAlpha / ImageLoadWithViewSize / SamplingMode]:
 *   Toggle and confirm the Get/Is return values in the status label.
 *
 * Expected result:
 *   After every Set call, the matching Get/Is return value is reflected in the label.
 *   Negative depth index hides the animation behind the background fill.
 *   DesiredSize 50x50 yields a visibly low-resolution image.
 */
class TcAnimatedImageViewMisc : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "AnimatedImageView: DepthIndex / PreMult / DesiredSize / Placeholder / Sampling";
  }

  Dali::String GetDescription() const override
  {
    return "Toggle DepthIndex, PreMultipliedAlpha, DesiredSize, Placeholder, ImageLoadWithViewSize, SamplingMode and verify Get/Is return values";
  }

  void OnEnter(View contentArea) override
  {
    mView = AnimatedImageView::New(ANIM_WEBP)
              .SetRequestedWidth(PREVIEW_SIZE)
              .SetRequestedHeight(PREVIEW_SIZE)
              .SetBackgroundColor(UiColor(C_IMG_BG));

    mView.Play();

    mStatusLabel = MakeStatusLabel("Depth: 0 | DesiredSize: 0x0 | Sampling: BOX");
    mFlagsLabel  = MakeStatusLabel("PreMult: OFF | LoadWithViewSize: OFF | Placeholder: none");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL)
                            .SetRequestedWidth(MATCH_PARENT)
                            .SetRequestedHeight(WRAP_CONTENT)
                            .SetBackgroundColor(UiColor(C_BG))
                            .SetPadding(Extents(8, 8, 8, 8));

    content.Add(MakeCentered(mView));
    content.Add(mStatusLabel);
    content.Add(mFlagsLabel);

    content.Add(MakeButtonRow({
      MakeButton("Depth\n-1", [this] { mView.SetDepthIndex(-1); mDepth = -1; UpdateLabels(); }),
      MakeButton("Depth\n0",  [this] { mView.SetDepthIndex(0);  mDepth = 0;  UpdateLabels(); }),
      MakeButton("Depth\n1",  [this] { mView.SetDepthIndex(1);  mDepth = 1;  UpdateLabels(); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Desired\n0x0",    [this] { OnDesired(0,   0); }),
      MakeButton("Desired\n50x50",  [this] { OnDesired(50,  50); }),
      MakeButton("Desired\n200x200",[this] { OnDesired(200, 200); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Sampling\nBOX",     [this] { mView.SetSamplingMode(Ui::Image::SamplingMode::BOX);     UpdateLabels(); }),
      MakeButton("Sampling\nNEAREST", [this] { mView.SetSamplingMode(Ui::Image::SamplingMode::NEAREST); UpdateLabels(); }),
      MakeButton("Sampling\nLINEAR",  [this] { mView.SetSamplingMode(Ui::Image::SamplingMode::LINEAR);  UpdateLabels(); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("PreMult\nON",  [this] { mView.SetPreMultipliedAlpha(true);  UpdateLabels(); }),
      MakeButton("PreMult\nOFF", [this] { mView.SetPreMultipliedAlpha(false); UpdateLabels(); }),
      MakeButton("LoadWith\nViewSize ON",  [this] { mView.SetImageLoadWithViewSize(true);  UpdateLabels(); }),
      MakeButton("LoadWith\nViewSize OFF", [this] { mView.SetImageLoadWithViewSize(false); UpdateLabels(); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Set\nPlaceholder",   [this] { mView.SetPlaceholderUrl(IMG_PLACEHOLDER); UpdateLabels(); }),
      MakeButton("Clear\nPlaceholder", [this] { mView.SetPlaceholderUrl(""); UpdateLabels(); }),
      MakeButton("Reload\nURL",        [this] { mView.SetResourceUrl(ANIM_GIF); mView.SetResourceUrl(ANIM_WEBP); mView.Play(); }),
    }));

    contentArea.Add(content);
  }

private:
  void OnDesired(int w, int h)
  {
    mView.SetDesiredWidth(w);
    mView.SetDesiredHeight(h);
    UpdateLabels();
  }

  void UpdateLabels()
  {
    mStatusLabel.SetText(
      Dali::String("Depth: ") + Dali::String(std::to_string(mDepth).c_str()) +
      Dali::String(" | DesiredSize: ") + Dali::String(std::to_string(mView.GetDesiredWidth()).c_str()) +
      Dali::String("x") + Dali::String(std::to_string(mView.GetDesiredHeight()).c_str()) +
      Dali::String(" | Sampling: ") + Dali::String(SamplingModeName(mView.GetSamplingMode())));

    Dali::String phUrl = mView.GetPlaceholderUrl();
    mFlagsLabel.SetText(
      Dali::String("PreMult: ") + Dali::String(mView.IsPreMultipliedAlpha() ? "ON" : "OFF") +
      Dali::String(" | LoadWithViewSize: ") + Dali::String(mView.IsImageLoadWithViewSizeEnabled() ? "ON" : "OFF") +
      Dali::String(" | Placeholder: ") + Dali::String(phUrl.Empty() ? "none" : "SET"));
  }

  View MakeCentered(View child)
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(PREVIEW_SIZE + 16)
                        .SetPadding(Extents(0, 0, 8, 8));
    row.Add(View::New().SetRequestedWidth(WRAP_CONTENT).SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f)));
    row.Add(child);
    row.Add(View::New().SetRequestedWidth(WRAP_CONTENT).SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f)));
    return row;
  }

  Label MakeStatusLabel(const Dali::String& text)
  {
    return Label::New(text)
      .SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(STATUS_H)
      .SetFontSize(11.0f).SetTextColor(UiColor(C_STATUS_TEXT))
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
                            .SetFontSize(11.0f).SetTextColor(UiColor(C_BTN_TEXT))
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

  AnimatedImageView mView;
  Label             mStatusLabel;
  Label             mFlagsLabel;
  int               mDepth{0};
};

REGISTER_MANUAL_TEST(TcAnimatedImageViewMisc)
