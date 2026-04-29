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
const char* const LOTTIE_WALKER   = TEST_RESOURCE_DIR "/jolly_walker.json";
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
 * @brief Verifies LottieAnimationView loading options:
 *   SetReleasePolicy / GetReleasePolicy
 *   SetSynchronousLoading / IsSynchronousLoading
 *   SetPreMultipliedAlpha / IsPreMultipliedAlpha
 *   SetPlaceholderUrl / GetPlaceholderUrl
 *   GetLoadingStatus
 *
 * Steps:
 *   [ReleasePolicy verification]:
 *   1. [Release: NEVER] -> [Remove View] -> [Re-Add View] -> confirm immediate display
 *   2. [Release: DETACHED] -> [Remove View] -> [Re-Add View] -> confirm reload occurs
 *
 *   [Placeholder verification]:
 *   1. [Set Placeholder] -> [Reload URL] -> confirm placeholder shown during loading
 *   2. [Clear Placeholder] -> confirm GetPlaceholderUrl returns empty string
 *
 *   [SyncLoading verification]:
 *   1. [Sync ON] -> [Reload URL] -> confirm immediate load
 *
 * Expected result:
 *   NEVER policy: re-add shows immediately without reloading.
 *   After setting a placeholder, reloading the URL shows the placeholder during loading.
 */
class TcLottieLoading : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "Lottie: ReleasePolicy / SyncLoading / PreMult / Placeholder";
  }

  Dali::String GetDescription() const override
  {
    return "NEVER policy remove -> re-add shows immediately; placeholder displayed during loading; verify SyncLoading";
  }

  void OnEnter(View contentArea) override
  {
    mReadyCount = 0;

    mView = LottieAnimationView::New(LOTTIE_WALKER)
              .SetRequestedWidth(PREVIEW_SIZE)
              .SetRequestedHeight(PREVIEW_SIZE);

    mView.ResourceReadySignal().Connect(this, [this](View) {
      ++mReadyCount;
      UpdateLabel();
    });

    mView.Play();

    mContainer = StackLayout::New(StackOrientation::VERTICAL)
                   .SetRequestedWidth(PREVIEW_SIZE)
                   .SetRequestedHeight(PREVIEW_SIZE);
    mContainer.Add(mView);

    mStatusLabel = MakeStatusLabel("ReleasePolicy: DETACHED | Sync: OFF | PreMult: OFF | Placeholder: none");
    mReadyLabel  = MakeStatusLabel("ResourceReadySignal: 0 | View: in scene");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL)
                            .SetRequestedWidth(MATCH_PARENT)
                            .SetRequestedHeight(WRAP_CONTENT)
                            .SetBackgroundColor(UiColor(C_BG))
                            .SetPadding(Extents(8, 8, 8, 8));

    content.Add(MakeCentered(mContainer));
    content.Add(mStatusLabel);
    content.Add(mReadyLabel);

    content.Add(MakeButtonRow({
      MakeButton("Release:\nDETACHED",  [this] { mView.SetReleasePolicy(Ui::Image::ReleasePolicy::DETACHED);  UpdateLabel(); }),
      MakeButton("Release:\nDESTROYED", [this] { mView.SetReleasePolicy(Ui::Image::ReleasePolicy::DESTROYED); UpdateLabel(); }),
      MakeButton("Release:\nNEVER",     [this] { mView.SetReleasePolicy(Ui::Image::ReleasePolicy::NEVER);     UpdateLabel(); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Sync\nON",  [this] { mView.SetSynchronousLoading(true);  UpdateLabel(); }),
      MakeButton("Sync\nOFF", [this] { mView.SetSynchronousLoading(false); UpdateLabel(); }),
      MakeButton("PreMult\nON",  [this] { mView.SetPreMultipliedAlpha(true);  UpdateLabel(); }),
      MakeButton("PreMult\nOFF", [this] { mView.SetPreMultipliedAlpha(false); UpdateLabel(); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Set\nPlaceholder",   [this] { mView.SetPlaceholderUrl(IMG_PLACEHOLDER); UpdateLabel(); }),
      MakeButton("Clear\nPlaceholder", [this] { mView.SetPlaceholderUrl(""); UpdateLabel(); }),
      MakeButton("Reload\nURL",        [this] { mView.SetResourceUrl(LOTTIE_WALKER); mView.Play(); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Remove\nView",  [this] { OnRemove(); }),
      MakeButton("Re-Add\nView",  [this] { OnReAdd(); }),
    }));

    contentArea.Add(content);
  }

private:
  void OnRemove()
  {
    if(mView.GetParent())
    {
      mContainer.Remove(mView);
      UpdateLabel();
    }
  }

  void OnReAdd()
  {
    if(!mView.GetParent())
    {
      mContainer.Add(mView);
      mView.Play();
      UpdateLabel();
    }
  }

  void UpdateLabel()
  {
    auto rp = mView.GetReleasePolicy();
    Dali::String rpStr;
    switch(rp)
    {
      case Ui::Image::ReleasePolicy::DETACHED:  rpStr = "DETACHED";  break;
      case Ui::Image::ReleasePolicy::DESTROYED: rpStr = "DESTROYED"; break;
      case Ui::Image::ReleasePolicy::NEVER:     rpStr = "NEVER";     break;
      default:                           rpStr = "?";         break;
    }

    Dali::String phUrl = mView.GetPlaceholderUrl();
    mStatusLabel.SetText(
      Dali::String("Release: ") + rpStr +
      Dali::String(" | Sync: ") + Dali::String(mView.IsSynchronousLoading() ? "ON" : "OFF") +
      Dali::String("\nPreMult: ") + Dali::String(mView.IsPreMultipliedAlpha() ? "ON" : "OFF") +
      Dali::String(" | PH: ") + Dali::String(phUrl.Empty() ? "none" : "SET"));

    Dali::String inScene = mView.GetParent() ? "in scene" : "REMOVED";
    mReadyLabel.SetText(
      Dali::String("ResourceReadySignal: ") + Dali::String(std::to_string(mReadyCount).c_str()) +
      Dali::String(" | View: ") + inScene);
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
                            .SetFontSize(11.0f).SetTextColor(UiColor(C_BTN_TEXT))
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

  LottieAnimationView mView;
  StackLayout         mContainer;
  Label               mStatusLabel;
  Label               mReadyLabel;
  int                 mReadyCount{0};
};

REGISTER_MANUAL_TEST(TcLottieLoading)
