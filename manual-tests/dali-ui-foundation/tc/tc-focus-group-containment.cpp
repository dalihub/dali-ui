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
    mStatusLabel = Label::New()
                     .SetText("Set FocusGroup state, then use arrow keys")
                     .SetFontSize(FONT_SIZE)
                     .SetTextColor(UiColor(COLOR_TEXT))
                     .SetRequestedWidth(MATCH_PARENT)
                     .SetRequestedHeight(60.0f)
                     .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                     .SetVerticalTextAlignment(Text::Alignment::CENTER)
                     .SetMultiLine(true);

    mInsideA = Label::New()
                 .SetText("Inside A")
                 .SetFontSize(FONT_SIZE)
                 .SetBackgroundColor(UiColor(COLOR_GROUP))
                 .SetRequestedWidth(MATCH_PARENT)
                 .SetRequestedHeight(60.0f)
                 .SetFocusable(true)
                 .SetTouchFocusable(true)
                 .With([this](View& view)
                 {
                   ConnectFocusColor(view, COLOR_GROUP);
                 });

    mInsideB = Label::New()
                 .SetText("Inside B")
                 .SetFontSize(FONT_SIZE)
                 .SetBackgroundColor(UiColor(COLOR_GROUP))
                 .SetRequestedWidth(MATCH_PARENT)
                 .SetRequestedHeight(60.0f)
                 .SetFocusable(true)
                 .SetTouchFocusable(true)
                 .With([this](View& view)
                 {
                   ConnectFocusColor(view, COLOR_GROUP);
                 });

    mGroup = StackLayout::New(StackOrientation::VERTICAL)
               .SetRequestedWidth(MATCH_PARENT)
               .SetRequestedHeight(132.0f)
               .Children({mInsideA, mInsideB});

    mOutside = Label::New()
                 .SetText("Outside")
                 .SetFontSize(FONT_SIZE)
                 .SetBackgroundColor(UiColor(COLOR_DEFAULT))
                 .SetRequestedWidth(MATCH_PARENT)
                 .SetRequestedHeight(60.0f)
                 .SetFocusable(true)
                 .SetTouchFocusable(true)
                 .With([this](View& view)
                 {
                   ConnectFocusColor(view, COLOR_DEFAULT);
                 });

    contentArea.Add(
      StackLayout::New(StackOrientation::VERTICAL)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(MATCH_PARENT)
        .SetBackgroundColor(UiColor(COLOR_BG))
        .SetPadding(Extents(GAP, GAP, GAP, GAP))
        .Children({mStatusLabel,
                   CreateButton("Set FocusGroup OFF", [this]() {
                     FocusManager::Get().SetAsFocusGroup(mGroup, false);
                     mStatusLabel.SetText("FocusGroup: OFF");
                   }),
                   CreateButton("Set FocusGroup ON", [this]() {
                     FocusManager::Get().SetAsFocusGroup(mGroup, true);
                     mStatusLabel.SetText("FocusGroup: ON (default navigation scoped)");
                   }),
                   Label::New().SetText("Group:").SetFontSize(FONT_SIZE).SetTextColor(UiColor(COLOR_TEXT)),
                   mGroup,
                   Label::New().SetText("Outside:").SetFontSize(FONT_SIZE).SetTextColor(UiColor(COLOR_TEXT)),
                   mOutside}));

    // Start focus on Inside A
    FocusManager::Get().RequestFocus(mInsideA);
  }

private:
  View CreateButton(const char* text, std::function<void()> onClick)
  {
    auto btn = Label::New()
                 .SetText(text)
                 .SetFontSize(FONT_SIZE)
                 .SetTextColor(UiColor(0xFFFFFF))
                 .SetBackgroundColor(UiColor(0x4285F4))
                 .SetRequestedWidth(MATCH_PARENT)
                 .SetRequestedHeight(44.0f)
                 .SetMargin(Extents(0, 0, 4, 4))
                 .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                 .SetVerticalTextAlignment(Text::Alignment::CENTER);

    btn.TouchedSignal().Connect(this, [onClick](Actor, const TouchEvent& e) -> bool {
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
