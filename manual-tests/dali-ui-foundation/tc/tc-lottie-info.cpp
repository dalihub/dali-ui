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
constexpr float    STATUS_H      = 48.0f;
constexpr uint32_t C_BTN_BG      = 0x555555;
constexpr uint32_t C_BTN_TEXT    = 0xEEEEEE;
constexpr uint32_t C_STATUS_BG   = 0x222222;
constexpr uint32_t C_STATUS_TEXT = 0xCCCCCC;
constexpr uint32_t C_BG          = 0x1A1A1A;
} // namespace

/**
 * @brief Verifies LottieAnimationView info query API:
 *   GetContentInfo  — layer information (Property::Map)
 *   GetMarkerInfo   — marker information (Property::Map)
 *
 * Steps:
 *   1. Click [Play] -> animation starts playing
 *   2. Click [GetContentInfo] -> ContentInfo label shows layer count and first item name
 *   3. Click [GetMarkerInfo] -> MarkerInfo label shows marker count
 *      (shows "MarkerInfo: 0 items" if jolly_walker.json has no markers)
 *
 * Expected result:
 *   GetContentInfo: returned Property::Map has Count() > 0 and layer name is displayed.
 *   GetMarkerInfo: shows marker name if present, otherwise "0 items".
 */
class TcLottieInfo : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "Lottie: GetContentInfo / GetMarkerInfo";
  }

  Dali::String GetDescription() const override
  {
    return "Verify layer/marker info is shown in labels after clicking [GetContentInfo] / [GetMarkerInfo]";
  }

  void OnEnter(View contentArea) override
  {
    mView = LottieAnimationView::New(LOTTIE_WALKER);
    mView.SetRequestedWidth(PREVIEW_SIZE);
    mView.SetRequestedHeight(PREVIEW_SIZE);

    mView.ResourceReadySignal().Connect(this, [this](View) {
      // Automatically refresh info after load completes
      OnGetContentInfo();
      OnGetMarkerInfo();
    });

    mView.Play();

    mContentLabel = MakeStatusLabel("ContentInfo: (tap button after load)");
    mMarkerLabel  = MakeStatusLabel("MarkerInfo: (tap button after load)");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetBackgroundColor(UiColor(C_BG));
    content.SetPadding(Extents(8, 8, 8, 8));

    content.Add(MakeCentered(mView));
    content.Add(mContentLabel);
    content.Add(mMarkerLabel);
    content.Add(MakeButtonRow({
      MakeButton("GetContent\nInfo", [this] { OnGetContentInfo(); }),
      MakeButton("GetMarker\nInfo",  [this] { OnGetMarkerInfo(); }),
    }));

    contentArea.Add(content);
  }

private:
  void OnGetContentInfo()
  {
    auto map = mView.GetContentInfo();
    int  count = static_cast<int>(map.Count());
    if(count == 0)
    {
      mContentLabel.SetText("ContentInfo: 0 items (not ready yet?)");
      return;
    }

    // Show the key (layer name) of the first entry
    Dali::String  firstName;
    Property::Key key = map.GetKeyAt(0);
    if(key.type == Property::Key::STRING)
    {
      firstName = key.stringKey;
    }
    mContentLabel.SetText(
      Dali::String("ContentInfo: ") + Dali::String(std::to_string(count).c_str()) +
      Dali::String(" items | first: \"") + firstName + Dali::String("\""));
  }

  void OnGetMarkerInfo()
  {
    auto map   = mView.GetMarkerInfo();
    int  count = static_cast<int>(map.Count());
    if(count == 0)
    {
      mMarkerLabel.SetText("MarkerInfo: 0 items");
      return;
    }

    Dali::String  firstName;
    Property::Key key = map.GetKeyAt(0);
    if(key.type == Property::Key::STRING)
    {
      firstName = key.stringKey;
    }
    mMarkerLabel.SetText(
      Dali::String("MarkerInfo: ") + Dali::String(std::to_string(count).c_str()) +
      Dali::String(" items | first: \"") + firstName + Dali::String("\""));
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
    label.SetFontSize(11.0f);
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
  Label               mContentLabel;
  Label               mMarkerLabel;
};

REGISTER_MANUAL_TEST(TcLottieInfo)
