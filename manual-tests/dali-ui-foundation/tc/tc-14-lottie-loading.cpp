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

#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
const char* const LOTTIE_WALKER   = TEST_RESOURCE_DIR "/jolly_walker.json";
const char* const IMG_PLACEHOLDER = TEST_RESOURCE_DIR "/placeholder_image.png";
// A path that never resolves — holds the loading/failure state open so the
// placeholder-while-loading criterion has something to photograph.
const char* const LOTTIE_MISSING  = TEST_RESOURCE_DIR "/no-such-animation.json";

const char* LoadingStatusName(Ui::Visual::ResourceStatus s)
{
  switch(s)
  {
    case Ui::Visual::ResourceStatus::PREPARING: return "PREPARING";
    case Ui::Visual::ResourceStatus::READY:     return "READY";
    case Ui::Visual::ResourceStatus::FAILED:    return "FAILED";
    default:                                    return "?";
  }
}

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
    return "14. Lottie: ReleasePolicy / SyncLoading / Placeholder";
  }

  Dali::String GetDescription() const override
  {
    return "NEVER policy remove -> re-add shows immediately; placeholder displayed during loading; verify SyncLoading";
  }

  void OnEnter(View contentArea) override
  {
    mReadyCount = 0;

    mView = LottieAnimationView::New(LOTTIE_WALKER);
    mView.SetRequestedWidth(PREVIEW_SIZE);
    mView.SetRequestedHeight(PREVIEW_SIZE);

    mView.ResourceReadySignal().Connect(this, [this](View) {
      ++mReadyCount;
      UpdateLabel();
    });

    mView.Play();

    mContainer = StackLayout::New(StackOrientation::VERTICAL);
    mContainer.SetRequestedWidth(PREVIEW_SIZE);
    mContainer.SetRequestedHeight(PREVIEW_SIZE);
    mContainer.Add(mView);

    mStatusLabel = MakeStatusLabel("ReleasePolicy: DETACHED | Sync: OFF | Placeholder: none");
    mReadyLabel  = MakeStatusLabel("ResourceReadySignal: 0 | View: in scene");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetBackgroundColor(UiColor(C_BG));
    content.SetPadding(Insets(8.0f, 8.0f, 8.0f, 8.0f));

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
    }));
    content.Add(MakeButtonRow({
      MakeButton("Set\nPlaceholder",   [this] { mView.SetPlaceholderUrl(IMG_PLACEHOLDER); UpdateLabel(); }),
      MakeButton("Clear\nPlaceholder", [this] { mView.SetPlaceholderUrl(""); UpdateLabel(); }),
      MakeButton("Reload\nURL",        [this] { mView.SetResourceUrl(LOTTIE_WALKER); mView.Play(); UpdateLabel(); }),
      MakeButton("Bad\nURL",           [this] { mView.SetResourceUrl(LOTTIE_MISSING); mView.Play(); UpdateLabel(); }),
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
      Dali::String(" | PH: ") + Dali::String(phUrl.Empty() ? "none" : "SET"));

    Dali::String inScene = mView.GetParent() ? "in scene" : "REMOVED";
    // GetLoadingStatus() answers "cache hit or real load?" DIRECTLY — the .md
    // always promised this field; the signal-counting above is the indirect
    // route that had to substitute for it.
    mReadyLabel.SetText(
      Dali::String("ResourceReadySignal: ") + Dali::String(std::to_string(mReadyCount).c_str()) +
      Dali::String(" | View: ") + inScene +
      Dali::String(" | Load: ") + Dali::String(LoadingStatusName(mView.GetLoadingStatus())));
  }

  View MakeCentered(View child)
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(PREVIEW_SIZE + 16);
    row.SetPadding(Insets(0.0f, 0.0f, 8.0f, 8.0f));
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

    buttonLabel.SetFontSize(11.0f);

    buttonLabel.SetTextColor(UiColor(C_BTN_TEXT));

    buttonLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);

    buttonLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);

    buttonLabel.SetMultiLine(true);

    btn.Add(buttonLabel);
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
    row.SetPadding(Insets(0.0f, 0.0f, 2.0f, 2.0f));
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
