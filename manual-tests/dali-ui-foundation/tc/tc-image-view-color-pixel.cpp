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
 * @brief Verifies ImageView SetImageColor / SetPixelArea.
 *
 * Steps:
 *   1. Tap a color button -> tint color is applied; RGBA values shown in label.
 *   2. Tap a PixelArea button -> only the specified region is displayed.
 *   3. [Full] -> restore the full image.
 *
 * Expected result:
 *   [Red] tints the image red. [PA: TopLeft] shows only the top-left quarter.
 */
class TcImageViewColorPixel : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "ImageView: ImageColor / PixelArea";
  }

  Dali::String GetDescription() const override
  {
    return "Verify SetImageColor tint and SetPixelArea region clipping";
  }

  void OnEnter(View contentArea) override
  {
    mImage = ImageView::New(IMG_A)
               .SetRequestedWidth(PREVIEW_SIZE)
               .SetRequestedHeight(PREVIEW_SIZE);

    mColorLabel = MakeStatusLabel("Color: R255 G255 B255 A255");
    mAreaLabel  = MakeStatusLabel("PixelArea: (0,0,100,100)%");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL)
                            .SetRequestedWidth(MATCH_PARENT)
                            .SetRequestedHeight(WRAP_CONTENT)
                            .SetBackgroundColor(UiColor(C_BG))
                            .SetPadding(Extents(8, 8, 8, 8));

    content.Add(MakeCenteredPreview(mImage));
    content.Add(mColorLabel);
    content.Add(mAreaLabel);

    content.Add(MakeButtonRow({
      MakeButton("White",      [this] { OnColor(UiColor(1.f, 1.f, 1.f, 1.f)); }),
      MakeButton("Red",        [this] { OnColor(UiColor(1.f, 0.f, 0.f, 1.f)); }),
      MakeButton("Green",      [this] { OnColor(UiColor(0.f, 1.f, 0.f, 1.f)); }),
      MakeButton("Blue",       [this] { OnColor(UiColor(0.f, 0.f, 1.f, 1.f)); }),
      MakeButton("50%\nAlpha", [this] { OnColor(UiColor(1.f, 1.f, 1.f, 0.5f)); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("PA: Full",    [this] { OnPixelArea(Vector4(0.f,   0.f,   1.f,  1.f)); }),
      MakeButton("PA: TopLeft", [this] { OnPixelArea(Vector4(0.f,   0.f,   0.5f, 0.5f)); }),
      MakeButton("PA: Center",  [this] { OnPixelArea(Vector4(0.25f, 0.25f, 0.5f, 0.5f)); }),
    }));

    contentArea.Add(content);
  }

private:
  void OnColor(const UiColor& color)
  {
    mImage.SetImageColor(color);
    Vector4 c = static_cast<Vector4>(mImage.GetImageColor());
    mColorLabel.SetText(
      Dali::String("Color: R") + Dali::String(std::to_string((int)(c.r * 255)).c_str()) +
      Dali::String(" G")       + Dali::String(std::to_string((int)(c.g * 255)).c_str()) +
      Dali::String(" B")       + Dali::String(std::to_string((int)(c.b * 255)).c_str()) +
      Dali::String(" A")       + Dali::String(std::to_string((int)(c.a * 255)).c_str()));
  }

  void OnPixelArea(const Vector4& pa)
  {
    mImage.SetPixelArea(pa);
    Vector4 v = mImage.GetPixelArea();
    mAreaLabel.SetText(
      Dali::String("PixelArea: (") +
      Dali::String(std::to_string((int)(v.x * 100)).c_str()) + Dali::String(",") +
      Dali::String(std::to_string((int)(v.y * 100)).c_str()) + Dali::String(",") +
      Dali::String(std::to_string((int)(v.z * 100)).c_str()) + Dali::String(",") +
      Dali::String(std::to_string((int)(v.w * 100)).c_str()) + Dali::String(")%"));
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
      trait.ClickedSignal().Connect(this, [onClick](View, const InputEvent&) -> bool {
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
  Label     mColorLabel;
  Label     mAreaLabel;
};

REGISTER_MANUAL_TEST(TcImageViewColorPixel)
