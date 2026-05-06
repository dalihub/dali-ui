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
    contentArea.Add(
      StackLayout::New(StackOrientation::VERTICAL)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(MATCH_PARENT)
        .SetBackgroundColor(UiColor(COLOR_BG))
        .SetPadding(Extents(GAP, GAP, GAP, GAP))
        .Children({
          Label::New()
            .As(mStatusLabel)
            .SetText("Set block state, then try RequestFocus")
            .SetFontSize(FONT_SIZE)
            .SetTextColor(UiColor(COLOR_TEXT))
            .SetRequestedWidth(MATCH_PARENT)
            .SetRequestedHeight(60.0f)
            .SetHorizontalTextAlignment(Text::Alignment::CENTER)
            .SetVerticalTextAlignment(Text::Alignment::CENTER)
            .SetMultiLine(true),
          CreateButton("Set Block OFF", [this]() {
            SetBlocked(false);
            mStatusLabel.SetText("BLOCKED: OFF");
          }),
          CreateButton("Set Block ON", [this]() {
            SetBlocked(true);
            mStatusLabel.SetText("BLOCKED: ON");
          }),
          CreateButton("RequestFocus on Child", [this]() {
            bool ok = FocusManager::Get().RequestFocus(mChild);
            mStatusLabel.SetText(ok ? "Child focused: SUCCESS" : "Child focus REJECTED");
          }),
          Label::New().SetText("Container (blue/red area):").SetFontSize(FONT_SIZE).SetTextColor(UiColor(COLOR_TEXT)),
          StackLayout::New(StackOrientation::VERTICAL)
            .As(mContainer)
            .SetRequestedWidth(MATCH_PARENT)
            .SetRequestedHeight(96.0f)
            .SetBackgroundColor(UiColor(COLOR_CONTAINER))
            .SetPadding(Extents(GAP, GAP, GAP, GAP))
            .Children({
              Label::New()
                .As(mChild)
                .SetText("Focusable Child")
                .SetFontSize(FONT_SIZE)
                .SetBackgroundColor(UiColor(COLOR_DEFAULT))
                .SetRequestedWidth(MATCH_PARENT)
                .SetRequestedHeight(60.0f)
                .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                .SetVerticalTextAlignment(Text::Alignment::CENTER)
                .SetFocusable(true)
                .With([this](View& view)
                {
                  view.FocusChangedSignal().Connect(this, [](View child, bool focused)
                  {
                    child.SetBackgroundColor(UiColor(focused ? COLOR_FOCUSED : COLOR_DEFAULT));
                  });
                }),
            }),
        }));
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

    btn.TouchedSignal().Connect(this, [onClick](Actor, TouchEvent e) -> bool {
      if(e.GetState(0) == PointState::UP) onClick();
      return true;
    });
    return btn;
  }

  void SetBlocked(bool blocked)
  {
    FocusManager::Get().ClearFocus();
    mContainer.SetDescendantFocusBlocked(blocked);
    mContainer.SetBackgroundColor(UiColor(blocked ? COLOR_BLOCKED : COLOR_CONTAINER));
    mChild.SetBackgroundColor(UiColor(COLOR_DEFAULT));
  }

  Label mStatusLabel;
  View  mChild;
  View  mContainer;
};

REGISTER_MANUAL_TEST(TcFocusDescendantBlocked)
