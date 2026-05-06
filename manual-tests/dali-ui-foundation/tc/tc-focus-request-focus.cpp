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
constexpr uint32_t COLOR_TEXT    = 0x3C3C3C;
constexpr uint32_t COLOR_BG     = 0xF7F5F3;
constexpr float    FONT_SIZE    = 14.0f;
constexpr float    GAP          = 12.0f;
} // namespace

/**
 * @brief Verifies FocusManager::RequestFocus delegates to focusable descendants.
 */
class TcFocusRequestFocus : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "Focus RequestFocus";
  }

  Dali::String GetDescription() const override
  {
    return "RequestFocus on Layout delegates to first focusable child";
  }

  void OnEnter(View contentArea) override
  {
    mStatusLabel = Label::New()
                     .SetText("Press button to test")
                     .SetFontSize(FONT_SIZE)
                     .SetTextColor(UiColor(COLOR_TEXT))
                     .SetRequestedWidth(MATCH_PARENT)
                     .SetRequestedHeight(60.0f)
                     .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                     .SetVerticalTextAlignment(Text::Alignment::CENTER)
                     .SetMultiLine(true);

    mNonFocusable = Label::New()
                      .SetText("Non-focusable")
                      .SetFontSize(FONT_SIZE)
                      .SetBackgroundColor(UiColor(COLOR_DEFAULT))
                      .SetLayoutParams(StackLayoutParams::New().SetWeight(1))
                      .SetRequestedHeight(60.0f);

    mFocusableChild = Label::New()
                        .SetText("Focusable")
                        .SetFontSize(FONT_SIZE)
                        .SetBackgroundColor(UiColor(COLOR_DEFAULT))
                        .SetLayoutParams(StackLayoutParams::New().SetWeight(1))
                        .SetRequestedHeight(60.0f)
                        .SetFocusable(true)
                        .With([this](View& view)
                        {
                          view.FocusChangedSignal().Connect(this, [](View focusedView, bool focused)
                          {
                            focusedView.SetBackgroundColor(UiColor(focused ? COLOR_FOCUSED : COLOR_DEFAULT));
                          });
                        });

    mLayout = StackLayout::New(StackOrientation::HORIZONTAL)
                .SetRequestedWidth(MATCH_PARENT)
                .SetRequestedHeight(60.0f)
                .Children({mNonFocusable, mFocusableChild});

    auto btnRequestFocus = Label::New()
                             .SetText("RequestFocus on Layout")
                             .SetFontSize(FONT_SIZE)
                             .SetTextColor(UiColor(0xFFFFFF))
                             .SetBackgroundColor(UiColor(0x4285F4))
                             .SetRequestedWidth(MATCH_PARENT)
                             .SetRequestedHeight(44.0f)
                             .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                             .SetVerticalTextAlignment(Text::Alignment::CENTER);

    btnRequestFocus.TouchedSignal().Connect(this, [this](Actor, TouchEvent e) -> bool {
      if(e.GetState(0) == PointState::UP)
      {
        FocusManager::Get().ClearFocus();
        bool ok = FocusManager::Get().RequestFocus(mLayout);
        mStatusLabel.SetText(ok ? "SUCCESS: Focusable child got focus" : "FAILED");
      }
      return true;
    });

    contentArea.Add(
      StackLayout::New(StackOrientation::VERTICAL)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(MATCH_PARENT)
        .SetBackgroundColor(UiColor(COLOR_BG))
        .SetPadding(Extents(GAP, GAP, GAP, GAP))
        .Children({mStatusLabel, btnRequestFocus,
                   Label::New().SetText("Layout:").SetFontSize(FONT_SIZE).SetTextColor(UiColor(COLOR_TEXT)),
                   mLayout}));

  }

private:
  Label mStatusLabel;
  View  mNonFocusable;
  View  mFocusableChild;
  View  mLayout;
};

REGISTER_MANUAL_TEST(TcFocusRequestFocus)
