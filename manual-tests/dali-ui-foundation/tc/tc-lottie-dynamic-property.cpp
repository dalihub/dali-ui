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
#include <dali/devel-api/adaptor-framework/vector-animation-renderer.h>

#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
const char* const LOTTIE_WALKER = TEST_RESOURCE_DIR "/jolly_walker.json";

constexpr float    PREVIEW_SIZE  = 240.0f;
constexpr float    BTN_H         = 52.0f;
constexpr float    STATUS_H      = 32.0f;
constexpr uint32_t C_BTN_BG      = 0x555555;
constexpr uint32_t C_BTN_TEXT    = 0xEEEEEE;
constexpr uint32_t C_STATUS_BG   = 0x222222;
constexpr uint32_t C_STATUS_TEXT = 0xCCCCCC;
constexpr uint32_t C_BG          = 0x1A1A1A;

constexpr int32_t PROP_ID_RED    = 1;
constexpr int32_t PROP_ID_BLUE   = 2;
constexpr int32_t PROP_ID_GREEN  = 3;

/**
 * @note Called on a worker thread — do not call DALi APIs here.
 */
static Dali::Property::Value OnGetFillColor(int32_t                                     id,
                                             Dali::VectorAnimationRenderer::VectorProperty /*property*/,
                                             uint32_t /*frameNumber*/)
{
  if(id == PROP_ID_RED)   return Dali::Property::Value(Dali::Vector4(1.f, 0.f, 0.f, 1.f));
  if(id == PROP_ID_BLUE)  return Dali::Property::Value(Dali::Vector4(0.f, 0.f, 1.f, 1.f));
  if(id == PROP_ID_GREEN) return Dali::Property::Value(Dali::Vector4(0.f, 1.f, 0.f, 1.f));
  return Dali::Property::Value(Dali::Vector4(1.f, 1.f, 1.f, 1.f));
}
} // namespace

/**
 * @brief Verifies LottieAnimationView SetDynamicProperty (FILL_COLOR).
 *
 * SetDynamicProperty changes the fill colour of layers dynamically on every
 * frame via a worker-thread callback.
 *
 * Steps:
 *   1. Click [Play] -> confirm playback with original colours
 *   2. Click [Fill: Red] -> confirm all layers (keyPath="**") turn red
 *   3. Click [Fill: Blue] -> confirm change to blue
 *   4. Click [Fill: Green] -> confirm change to green
 *   5. Click [Clear Dynamic] -> reload URL to reset dynamic properties
 *
 * Expected result:
 *   Immediately after clicking a Fill button the animation colour changes during playback.
 *   After Clear the original colours are restored.
 */
class TcLottieDynamicProperty : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "Lottie: SetDynamicProperty (FILL_COLOR)";
  }

  Dali::String GetDescription() const override
  {
    return "Verify per-frame callback dynamically changes the fill colour of all layers";
  }

  void OnEnter(View contentArea) override
  {
    mView = LottieAnimationView::New(LOTTIE_WALKER);
    mView.SetRequestedWidth(PREVIEW_SIZE);
    mView.SetRequestedHeight(PREVIEW_SIZE);

    mView.Play();

    mStatusLabel = MakeStatusLabel("DynamicProperty: none (original colors)");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetBackgroundColor(UiColor(C_BG));
    content.SetPadding(Extents(8, 8, 8, 8));

    content.Add(MakeCentered(mView));
    content.Add(mStatusLabel);

    content.Add(MakeButtonRow({
      MakeButton("Play",  [this] { mView.Play(); }),
      MakeButton("Pause", [this] { mView.Pause(); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Fill:\nRed",   [this] { OnDynamicFill(PROP_ID_RED,   "Red"); }),
      MakeButton("Fill:\nGreen", [this] { OnDynamicFill(PROP_ID_GREEN, "Green"); }),
      MakeButton("Fill:\nBlue",  [this] { OnDynamicFill(PROP_ID_BLUE,  "Blue"); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Clear Dynamic\n(reload)", [this] { OnClearDynamic(); }),
    }));

    contentArea.Add(content);
  }

private:
  void OnDynamicFill(int32_t propId, const char* colorName)
  {
    Ui::LottieAnimation::DynamicPropertyInfo info;
    info.id       = propId;
    info.keyPath  = "**";
    info.property = Ui::LottieAnimation::VectorProperty::FILL_COLOR;
    info.callback = MakeCallback(&OnGetFillColor);
    mView.SetDynamicProperty(info);

    mStatusLabel.SetText(Dali::String("DynamicProperty: FILL_COLOR = ") + Dali::String(colorName));
  }

  void OnClearDynamic()
  {
    // Reset dynamic properties by reloading the URL
    mView.SetResourceUrl(LOTTIE_WALKER);
    mView.Play();
    mStatusLabel.SetText("DynamicProperty: cleared (reloaded)");
  }

  View MakeCentered(View child)
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(PREVIEW_SIZE + 16);
    row.SetPadding(Extents(0, 0, 8, 8));
    row.Add(ManualTest::MakeWeightedSpacer());
    row.Add(child);
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

    btn.AddChildren({buttonLabel});
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
    row.SetPadding(Extents(0, 0, 2, 2));
    for(auto& b : buttons) row.Add(b);
    return row;
  }

  LottieAnimationView mView;
  Label               mStatusLabel;
};

REGISTER_MANUAL_TEST(TcLottieDynamicProperty)
