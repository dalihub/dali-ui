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

#include <dali-ui-foundation/public-api/views/image/animated-image-view.h>

#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
const char* const ANIM_WEBP = TEST_RESOURCE_DIR "/dog-anim.webp";

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
 * @brief Verifies AnimatedImageView load/release policy:
 *   SetLoadPolicy / GetLoadPolicy
 *   SetReleasePolicy / GetReleasePolicy
 *   SetSynchronousLoading / IsSynchronousLoading
 *
 * Steps:
 *   [ReleasePolicy: NEVER] verification:
 *   1. Click [Release: NEVER]
 *   2. [Remove View] -> animation disappears from screen
 *   3. [Re-Add View] -> reappears immediately (restored from cache)
 *
 *   [ReleasePolicy: DETACHED] verification:
 *   1. Click [Release: DETACHED]
 *   2. [Remove View] -> [Re-Add View] -> confirm reload occurs
 *
 *   [SynchronousLoading verification]:
 *   1. [Sync ON] -> view shown; [Set URL] -> confirm immediate load
 *
 * Expected result:
 *   NEVER policy: re-add reuses already-cached texture.
 *   DETACHED policy: re-add triggers a reload.
 */
class TcAnimatedImageViewLoadPolicy : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "AnimatedImageView: LoadPolicy / ReleasePolicy / SyncLoading";
  }

  Dali::String GetDescription() const override
  {
    return "NEVER policy: remove -> re-add shows immediately; DETACHED: reloads; verify SyncLoading behaviour";
  }

  void OnEnter(View contentArea) override
  {
    mView = AnimatedImageView::New(ANIM_WEBP);
    mView.SetRequestedWidth(PREVIEW_SIZE);
    mView.SetRequestedHeight(PREVIEW_SIZE);

    mView.Play();

    mContainer = StackLayout::New(StackOrientation::VERTICAL);
    mContainer.SetRequestedWidth(PREVIEW_SIZE);
    mContainer.SetRequestedHeight(PREVIEW_SIZE);
    mContainer.Add(mView);

    mStatusLabel = MakeStatusLabel("LoadPolicy IMMEDIATE | ReleasePolicy: DETACHED | Sync: OFF | View: in scene");

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetBackgroundColor(UiColor(C_BG));
    content.SetPadding(Extents(8, 8, 8, 8));

    content.Add(MakeCentered(mContainer));
    content.Add(mStatusLabel);

    content.Add(MakeButtonRow({
      MakeButton("Load:\nIMMEDIATE", [this] { mView.SetLoadPolicy(Ui::Image::LoadPolicy::IMMEDIATE);  UpdateLabel(); }),
      MakeButton("Load:\nATTACHED",  [this] { mView.SetLoadPolicy(Ui::Image::LoadPolicy::ATTACHED);   UpdateLabel(); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Release:\nDETACHED",  [this] { mView.SetReleasePolicy(Ui::Image::ReleasePolicy::DETACHED);  UpdateLabel(); }),
      MakeButton("Release:\nDESTROYED", [this] { mView.SetReleasePolicy(Ui::Image::ReleasePolicy::DESTROYED); UpdateLabel(); }),
      MakeButton("Release:\nNEVER",     [this] { mView.SetReleasePolicy(Ui::Image::ReleasePolicy::NEVER);     UpdateLabel(); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Sync ON",  [this] { mView.SetSynchronousLoading(true);  UpdateLabel(); }),
      MakeButton("Sync OFF", [this] { mView.SetSynchronousLoading(false); UpdateLabel(); }),
    }));
    content.Add(MakeButtonRow({
      MakeButton("Remove\nView",  [this] { OnRemove(); }),
      MakeButton("Re-Add\nView",  [this] { OnReAdd(); }),
      MakeButton("Play",          [this] { mView.Play(); }),
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
    auto lp = mView.GetLoadPolicy();
    auto rp = mView.GetReleasePolicy();

    Dali::String lpStr = (lp == Ui::Image::LoadPolicy::IMMEDIATE) ? "IMMEDIATE" : "ATTACHED";
    Dali::String rpStr;
    switch(rp)
    {
      case Ui::Image::ReleasePolicy::DETACHED:  rpStr = "DETACHED";  break;
      case Ui::Image::ReleasePolicy::DESTROYED: rpStr = "DESTROYED"; break;
      case Ui::Image::ReleasePolicy::NEVER:     rpStr = "NEVER";     break;
      default:                           rpStr = "?";         break;
    }

    Dali::String inScene = mView.GetParent() ? "in scene" : "REMOVED";
    mStatusLabel.SetText(
      Dali::String("LoadPolicy ") + lpStr +
      Dali::String(" | Release: ") + rpStr +
      Dali::String("\nSync: ") + Dali::String(mView.IsSynchronousLoading() ? "ON" : "OFF") +
      Dali::String(" | View: ") + inScene);
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

  AnimatedImageView mView;
  StackLayout       mContainer;
  Label             mStatusLabel;
};

REGISTER_MANUAL_TEST(TcAnimatedImageViewLoadPolicy)
