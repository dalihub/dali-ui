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
const char* const IMG = TEST_RESOURCE_DIR "/gallery-large-3.jpg";

constexpr float    PREVIEW_SIZE  = 180.0f;
constexpr float    BTN_H         = 52.0f;
constexpr float    STATUS_H      = 48.0f;
constexpr uint32_t C_BTN_BG      = 0x555555;
constexpr uint32_t C_BTN_TEXT    = 0xEEEEEE;
constexpr uint32_t C_STATUS_BG   = 0x222222;
constexpr uint32_t C_STATUS_TEXT = 0xCCCCCC;
constexpr uint32_t C_BG          = 0x1A1A1A;
} // namespace

/**
 * @brief Verifies ImageView LoadPolicy / ReleasePolicy.
 *
 * Steps:
 *   [ReleasePolicy: NEVER] verification:
 *   1. Select [Release: NEVER]
 *   2. [Remove View] -> image disappears from screen
 *   3. [Re-Add View] -> image appears immediately (restored from cache, no ResourceReady fired)
 *
 *   [ReleasePolicy: DETACHED] verification:
 *   1. Select [Release: DETACHED]
 *   2. [Remove View] -> [Re-Add View] -> image is reloaded (ResourceReady fired)
 *
 * Expected result:
 *   NEVER policy: re-add shows image instantly without incrementing the ResourceReadySignal counter.
 *   DETACHED/DESTROYED policy: re-add triggers a reload and the counter increments.
 */
class TcImageViewLoadPolicy : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "ImageView: LoadPolicy / ReleasePolicy";
  }

  Dali::String GetDescription() const override
  {
    return "NEVER policy: remove -> re-add shows image immediately from cache (no ResourceReady fired)";
  }

  void OnEnter(View contentArea) override
  {
    mReadyCount = 0;

    mImage = ImageView::New(IMG)
               .SetRequestedWidth(PREVIEW_SIZE)
               .SetRequestedHeight(PREVIEW_SIZE);

    mImage.ResourceReadySignal().Connect(this, [this](View) {
      ++mReadyCount;
      UpdateLabel();
    });

    mContainer = StackLayout::New(StackOrientation::VERTICAL)
                   .SetRequestedWidth(PREVIEW_SIZE)
                   .SetRequestedHeight(PREVIEW_SIZE);
    mContainer.Add(mImage);

    mLabel = MakeStatusLabel("LoadPolicy IMMEDIATE | ReleasePolicy: DETACHED | ResourceReady: 0");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL)
                            .SetRequestedWidth(MATCH_PARENT)
                            .SetRequestedHeight(WRAP_CONTENT)
                            .SetBackgroundColor(UiColor(C_BG))
                            .SetPadding(Extents(8, 8, 8, 8));

    content.Add(MakeCentered(mContainer));
    content.Add(mLabel);
    content.Add(MakeButtonRow({
      MakeButton("Load:\nIMMEDIATE", [this] { OnLoadPolicy(Ui::Image::LoadPolicy::IMMEDIATE); }),
      MakeButton("Load:\nATTACHED",  [this] { OnLoadPolicy(Ui::Image::LoadPolicy::ATTACHED); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Release:\nDETACHED",  [this] { OnReleasePolicy(Ui::Image::ReleasePolicy::DETACHED); }),
      MakeButton("Release:\nDESTROYED", [this] { OnReleasePolicy(Ui::Image::ReleasePolicy::DESTROYED); }),
      MakeButton("Release:\nNEVER",     [this] { OnReleasePolicy(Ui::Image::ReleasePolicy::NEVER); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Remove\nView",  [this] { OnRemove(); }),
      MakeButton("Re-Add\nView",  [this] { OnReAdd(); }),
    }));

    contentArea.Add(content);
  }

private:
  void OnLoadPolicy(Ui::Image::LoadPolicy p)
  {
    mImage.SetLoadPolicy(p);
    UpdateLabel();
  }

  void OnReleasePolicy(Ui::Image::ReleasePolicy p)
  {
    mImage.SetReleasePolicy(p);
    UpdateLabel();
  }

  void OnRemove()
  {
    if(mImage.GetParent())
    {
      mContainer.Remove(mImage);
      UpdateLabel();
    }
  }

  void OnReAdd()
  {
    if(!mImage.GetParent())
    {
      mContainer.Add(mImage);
      UpdateLabel();
    }
  }

  void UpdateLabel()
  {
    Dali::String lpStr = (mImage.GetLoadPolicy() == Ui::Image::LoadPolicy::IMMEDIATE) ? "IMMEDIATE" : "ATTACHED";
    Dali::String rpStr;
    switch(mImage.GetReleasePolicy())
    {
      case Ui::Image::ReleasePolicy::DETACHED:  rpStr = "DETACHED";  break;
      case Ui::Image::ReleasePolicy::DESTROYED: rpStr = "DESTROYED"; break;
      case Ui::Image::ReleasePolicy::NEVER:     rpStr = "NEVER";     break;
      default:                           rpStr = "?";         break;
    }
    Dali::String inScene = mImage.GetParent() ? "in scene" : "REMOVED";
    mLabel.SetText(
      Dali::String("LoadPolicy ") + lpStr +
      Dali::String(" | ReleasePolicy: ") + rpStr +
      Dali::String("\nView: ") + inScene +
      Dali::String(" | ResourceReadySignal: ") +
      Dali::String(std::to_string(mReadyCount).c_str()));
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

  ImageView   mImage;
  StackLayout mContainer;
  Label       mLabel;
  int         mReadyCount{0};
};

REGISTER_MANUAL_TEST(TcImageViewLoadPolicy)
