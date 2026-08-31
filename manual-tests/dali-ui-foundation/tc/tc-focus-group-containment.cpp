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

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr uint32_t COLOR_DEFAULT = 0xE8D5E0;
constexpr uint32_t COLOR_FOCUSED = 0xFF9800;
constexpr uint32_t COLOR_GROUP   = 0xD5DBE8;
constexpr uint32_t COLOR_TEXT    = 0x3C3C3C;
constexpr uint32_t COLOR_BG     = 0xF7F5F3;
constexpr float    FONT_SIZE    = 14.0f;
constexpr float    GAP          = 12.0f;
} // namespace

/**
 * @brief Verifies FocusGroup traps keyboard focus within its subtree.
 */
class TcFocusGroupContainment : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "Focus Group Containment";
  }

  Dali::String GetDescription() const override
  {
    return "FocusGroup scopes default FocusFinder navigation";
  }

  void OnEnter(View contentArea) override
  {
    mStatusLabel = Label::New();
    mStatusLabel.SetText("Set FocusGroup state, then use arrow keys");
    mStatusLabel.SetFontSize(FONT_SIZE);
    mStatusLabel.SetTextColor(UiColor(COLOR_TEXT));
    mStatusLabel.SetRequestedWidth(MATCH_PARENT);
    mStatusLabel.SetRequestedHeight(60.0f);
    mStatusLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mStatusLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mStatusLabel.SetMultiLine(true);

    Label insideA = Label::New();
    insideA.SetText("Inside A");
    insideA.SetFontSize(FONT_SIZE);
    insideA.SetBackgroundColor(UiColor(COLOR_GROUP));
    insideA.SetRequestedWidth(MATCH_PARENT);
    insideA.SetRequestedHeight(60.0f);
    insideA.SetFocusable(true);
    insideA.SetFocusOnTouchEnabled(true);
    ConnectFocusColor(insideA, COLOR_GROUP);
    mInsideA = insideA;

    Label insideB = Label::New();
    insideB.SetText("Inside B");
    insideB.SetFontSize(FONT_SIZE);
    insideB.SetBackgroundColor(UiColor(COLOR_GROUP));
    insideB.SetRequestedWidth(MATCH_PARENT);
    insideB.SetRequestedHeight(60.0f);
    insideB.SetFocusable(true);
    insideB.SetFocusOnTouchEnabled(true);
    ConnectFocusColor(insideB, COLOR_GROUP);
    mInsideB = insideB;

    mGroup = StackLayout::New(StackOrientation::VERTICAL);
    mGroup.SetRequestedWidth(MATCH_PARENT);
    mGroup.SetRequestedHeight(132.0f);
    mGroup.Add(mInsideA);
    mGroup.Add(mInsideB);

    Label outside = Label::New();
    outside.SetText("Outside");
    outside.SetFontSize(FONT_SIZE);
    outside.SetBackgroundColor(UiColor(COLOR_DEFAULT));
    outside.SetRequestedWidth(MATCH_PARENT);
    outside.SetRequestedHeight(60.0f);
    outside.SetFocusable(true);
    outside.SetFocusOnTouchEnabled(true);
    ConnectFocusColor(outside, COLOR_DEFAULT);
    mOutside = outside;

    Label groupLabel = Label::New();
    groupLabel.SetText("Group:");
    groupLabel.SetFontSize(FONT_SIZE);
    groupLabel.SetTextColor(UiColor(COLOR_TEXT));

    Label outsideLabel = Label::New();
    outsideLabel.SetText("Outside:");
    outsideLabel.SetFontSize(FONT_SIZE);
    outsideLabel.SetTextColor(UiColor(COLOR_TEXT));

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetBackgroundColor(UiColor(COLOR_BG));
    root.SetPadding(Insets(GAP, GAP, GAP, GAP));
    root.Add(mStatusLabel);
    root.Add(CreateButton("Set FocusGroup OFF", [this]() {
                        FocusManager::Get().SetAsFocusGroup(mGroup, false);
                        UpdateModeLabel();
                      }));
    root.Add(CreateButton("Set FocusGroup ON", [this]() {
                        FocusManager::Get().SetAsFocusGroup(mGroup, true);
                        UpdateModeLabel();
                      }));
    root.Add(groupLabel);
    root.Add(mGroup);
    root.Add(outsideLabel);
    root.Add(mOutside);
    contentArea.Add(root);

    // Start focus on Inside A
    FocusManager::Get().RequestFocus(mInsideA);
  }

private:
  void UpdateModeLabel()
  {
    // Read the mode back through IsFocusGroup() — the old label printed the
    // button's own string right after SetAsFocusGroup(), so a stubbed setter
    // kept all three exported assertions green (review 30; the getter always
    // existed, focus-manager.h).
    mStatusLabel.SetText(FocusManager::Get().IsFocusGroup(mGroup)
                           ? "FocusGroup: ON (default navigation scoped)"
                           : "FocusGroup: OFF");
  }

  View CreateButton(const char* text, std::function<void()> onClick)
  {
    auto btn = Label::New();
    btn.SetText(text);
    btn.SetFontSize(FONT_SIZE);
    btn.SetTextColor(UiColor(0xFFFFFF));
    btn.SetBackgroundColor(UiColor(0x4285F4));
    btn.SetRequestedWidth(MATCH_PARENT);
    btn.SetRequestedHeight(44.0f);
    btn.SetMargin(Insets(0.0f, 0.0f, 4.0f, 4.0f));
    btn.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    btn.SetVerticalTextAlignment(Text::Alignment::CENTER);

    btn.TouchEventSignal().Connect(this, [onClick](Actor, TouchEvent e) -> bool {
      if(e.GetState(0) == PointState::UP) onClick();
      return true;
    });
    return btn;
  }

  void ConnectFocusColor(View view, uint32_t baseColor)
  {
    view.FocusChangedSignal().Connect(this, [baseColor](View changedView, bool focused)
    {
      changedView.SetBackgroundColor(UiColor(focused ? COLOR_FOCUSED : baseColor));
    });
  }

  Label mStatusLabel;
  View  mInsideA;
  View  mInsideB;
  View  mGroup;
  View  mOutside;
};

REGISTER_MANUAL_TEST(TcFocusGroupContainment)
