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
constexpr uint32_t COLOR_BLOCKED = 0xCC3333;
constexpr uint32_t COLOR_CONTAINER = 0xD5DBE8;
constexpr uint32_t COLOR_TEXT    = 0x3C3C3C;
constexpr uint32_t COLOR_BG     = 0xF7F5F3;
constexpr float    FONT_SIZE    = 14.0f;
constexpr float    GAP          = 12.0f;
} // namespace

/**
 * @brief Verifies SetDescendantFocusBlocked prevents children from receiving focus.
 */
class TcFocusDescendantBlocked : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "Focus DescendantBlocked";
  }

  Dali::String GetDescription() const override
  {
    return "DescendantFocusBlocked prevents child focus";
  }

  void OnEnter(View contentArea) override
  {
    mStatusLabel = Label::New();
    mStatusLabel.SetText("Set block state, then try RequestFocus");
    mStatusLabel.SetFontSize(FONT_SIZE);
    mStatusLabel.SetTextColor(UiColor(COLOR_TEXT));
    mStatusLabel.SetRequestedWidth(MATCH_PARENT);
    mStatusLabel.SetRequestedHeight(60.0f);
    mStatusLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mStatusLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mStatusLabel.SetMultiLine(true);

    Label containerLabel = Label::New();
    containerLabel.SetText("Container (blue/red area):");
    containerLabel.SetFontSize(FONT_SIZE);
    containerLabel.SetTextColor(UiColor(COLOR_TEXT));

    Label childLabel = Label::New();
    childLabel.SetText("Focusable Child");
    childLabel.SetFontSize(FONT_SIZE);
    childLabel.SetBackgroundColor(UiColor(COLOR_DEFAULT));
    childLabel.SetRequestedWidth(MATCH_PARENT);
    childLabel.SetRequestedHeight(60.0f);
    childLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    childLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    childLabel.SetFocusable(true);
    childLabel.FocusChangedSignal().Connect(this, [](View child, bool focused)
    {
      child.SetBackgroundColor(UiColor(focused ? COLOR_FOCUSED : COLOR_DEFAULT));
    });
    mChild = childLabel;

    mContainer = StackLayout::New(StackOrientation::VERTICAL);
    mContainer.SetRequestedWidth(MATCH_PARENT);
    mContainer.SetRequestedHeight(96.0f);
    mContainer.SetBackgroundColor(UiColor(COLOR_CONTAINER));
    mContainer.SetPadding(Insets(GAP, GAP, GAP, GAP));
    mContainer.Add(mChild);

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetBackgroundColor(UiColor(COLOR_BG));
    root.SetPadding(Insets(GAP, GAP, GAP, GAP));
    root.Add(mStatusLabel);
    root.Add(CreateButton("Set Block OFF", [this]() {
        SetBlocked(false);
        mStatusLabel.SetText("BLOCKED: OFF");
      }));
    root.Add(CreateButton("Set Block ON", [this]() {
        SetBlocked(true);
        mStatusLabel.SetText("BLOCKED: ON");
      }));
    root.Add(CreateButton("RequestFocus on Child", [this]() {
        bool ok = FocusManager::Get().RequestFocus(mChild);
        mStatusLabel.SetText(ok ? "Child focused: SUCCESS" : "Child focus REJECTED");
      }));
    root.Add(containerLabel);
    root.Add(mContainer);
    contentArea.Add(root);
  }

private:
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

  void SetBlocked(bool blocked)
  {
    FocusManager::Get().ClearFocus();
    mContainer.SetAllowDescendantFocusEnabled(!blocked);
    mContainer.SetBackgroundColor(UiColor(blocked ? COLOR_BLOCKED : COLOR_CONTAINER));
    mChild.SetBackgroundColor(UiColor(COLOR_DEFAULT));
  }

  Label mStatusLabel;
  View  mChild;
  View  mContainer;
};

REGISTER_MANUAL_TEST(TcFocusDescendantBlocked)
