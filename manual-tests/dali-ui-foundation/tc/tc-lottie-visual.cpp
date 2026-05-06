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

#include <dali-ui-foundation/public-api/lottie-animation-view.h>

#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
const char* const LOTTIE_WALKER = TEST_RESOURCE_DIR "/jolly_walker.json";

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
 * @brief Verifies LottieAnimationView visual properties:
 *   SetImageColor / GetImageColor
 *   SetDesiredWidth / SetDesiredHeight / GetDesiredWidth / GetDesiredHeight
 *
 * Steps:
 *   [ImageColor verification]:
 *   1. [Play] -> plays with original colours
 *   2. [Red] -> confirm red tint applied to entire animation
 *   3. [50% Alpha] -> confirm semi-transparent
 *   4. [White] -> remove tint; verify GetImageColor
 *
 *   [DesiredSize verification]:
 *   1. [Desired 50x50] -> rasterised at low resolution (blurry)
 *   2. [Desired 0x0] -> no limit (rasterised to match view size)
 *
 * Expected result:
 *   After setting ImageColor, GetImageColor returns the same value.
 *   A small DesiredSize makes the image appear blurry.
 */
class TcLottieVisual : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "Lottie: SetImageColor / SetDesiredSize";
  }

  Dali::String GetDescription() const override
  {
    return "Verify SetImageColor tint application and resolution change with SetDesiredSize";
  }

  void OnEnter(View contentArea) override
  {
    mView = LottieAnimationView::New(LOTTIE_WALKER)
              .SetRequestedWidth(PREVIEW_SIZE)
              .SetRequestedHeight(PREVIEW_SIZE);

    mView.Play();

    mColorLabel = MakeStatusLabel("Color: R255 G255 B255 A255");
    mSizeLabel  = MakeStatusLabel("DesiredSize: 0x0");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL)
                            .SetRequestedWidth(MATCH_PARENT)
                            .SetRequestedHeight(WRAP_CONTENT)
                            .SetBackgroundColor(UiColor(C_BG))
                            .SetPadding(Extents(8, 8, 8, 8));

    content.Add(MakeCentered(mView));
    content.Add(mColorLabel);
    content.Add(mSizeLabel);

    content.Add(MakeButtonRow({
      MakeButton("White",      [this] { OnColor(UiColor(1.f, 1.f, 1.f, 1.f)); }),
      MakeButton("Red",        [this] { OnColor(UiColor(1.f, 0.f, 0.f, 1.f)); }),
      MakeButton("Blue",       [this] { OnColor(UiColor(0.f, 0.f, 1.f, 1.f)); }),
      MakeButton("50%\nAlpha", [this] { OnColor(UiColor(1.f, 1.f, 1.f, 0.5f)); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Desired\n0x0",     [this] { OnDesiredSize(0,   0); }),
      MakeButton("Desired\n50x50",   [this] { OnDesiredSize(50,  50); }),
      MakeButton("Desired\n200x200", [this] { OnDesiredSize(200, 200); }),
    }));

    contentArea.Add(content);
  }

private:
  void OnColor(const UiColor& color)
  {
    mView.SetImageColor(color);
    Vector4 c = static_cast<Vector4>(mView.GetImageColor());
    mColorLabel.SetText(
      Dali::String("Color: R") + Dali::String(std::to_string((int)(c.r * 255)).c_str()) +
      Dali::String(" G") + Dali::String(std::to_string((int)(c.g * 255)).c_str()) +
      Dali::String(" B") + Dali::String(std::to_string((int)(c.b * 255)).c_str()) +
      Dali::String(" A") + Dali::String(std::to_string((int)(c.a * 255)).c_str()));
  }

  void OnDesiredSize(int w, int h)
  {
    mView.SetDesiredWidth(w);
    mView.SetDesiredHeight(h);
    mSizeLabel.SetText(
      Dali::String("DesiredSize: ") + Dali::String(std::to_string(mView.GetDesiredWidth()).c_str()) +
      Dali::String("x") + Dali::String(std::to_string(mView.GetDesiredHeight()).c_str()));
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
      .SetFontSize(12.0f).SetTextColor(UiColor(C_STATUS_TEXT))
      .SetBackgroundColor(UiColor(C_STATUS_BG))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER);
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

  LottieAnimationView mView;
  Label               mColorLabel;
  Label               mSizeLabel;
};

REGISTER_MANUAL_TEST(TcLottieVisual)
