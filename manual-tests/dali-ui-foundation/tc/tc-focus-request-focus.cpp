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
    mStatusLabel = Label::New();
    mStatusLabel.SetText("Press button to test");
    mStatusLabel.SetFontSize(FONT_SIZE);
    mStatusLabel.SetTextColor(UiColor(COLOR_TEXT));
    mStatusLabel.SetRequestedWidth(MATCH_PARENT);
    mStatusLabel.SetRequestedHeight(60.0f);
    mStatusLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mStatusLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mStatusLabel.SetMultiLine(true);

    Label nonFocusable = Label::New();
    nonFocusable.SetText("Non-focusable");
    nonFocusable.SetFontSize(FONT_SIZE);
    nonFocusable.SetBackgroundColor(UiColor(COLOR_DEFAULT));
    nonFocusable.SetLayoutParams(StackLayoutParams::New().SetWeight(1));
    nonFocusable.SetRequestedHeight(60.0f);
    mNonFocusable = nonFocusable;

    Label focusableChild = Label::New();
    focusableChild.SetText("Focusable");
    focusableChild.SetFontSize(FONT_SIZE);
    focusableChild.SetBackgroundColor(UiColor(COLOR_DEFAULT));
    focusableChild.SetLayoutParams(StackLayoutParams::New().SetWeight(1));
    focusableChild.SetRequestedHeight(60.0f);
    focusableChild.SetFocusable(true);
    focusableChild.FocusChangedSignal().Connect(this, [](View focusedView, bool focused)
    {
      focusedView.SetBackgroundColor(UiColor(focused ? COLOR_FOCUSED : COLOR_DEFAULT));
    });
    mFocusableChild = focusableChild;

    mLayout = StackLayout::New(StackOrientation::HORIZONTAL);
    mLayout.SetRequestedWidth(MATCH_PARENT);
    mLayout.SetRequestedHeight(60.0f);
    mLayout.Add(mNonFocusable);
    mLayout.Add(mFocusableChild);

    auto btnRequestFocus = Label::New();
    btnRequestFocus.SetText("RequestFocus on Layout");
    btnRequestFocus.SetFontSize(FONT_SIZE);
    btnRequestFocus.SetTextColor(UiColor(0xFFFFFF));
    btnRequestFocus.SetBackgroundColor(UiColor(0x4285F4));
    btnRequestFocus.SetRequestedWidth(MATCH_PARENT);
    btnRequestFocus.SetRequestedHeight(44.0f);
    btnRequestFocus.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    btnRequestFocus.SetVerticalTextAlignment(Text::Alignment::CENTER);

    btnRequestFocus.TouchEventSignal().Connect(this, [this](Actor, TouchEvent e) -> bool {
      if(e.GetState(0) == PointState::UP)
      {
        FocusManager::Get().ClearFocus();
        bool ok = FocusManager::Get().RequestFocus(mLayout);
        mStatusLabel.SetText(ok ? "SUCCESS: Focusable child got focus" : "FAILED");
      }
      return true;
    });

    Label layoutLabel = Label::New();
    layoutLabel.SetText("Layout:");
    layoutLabel.SetFontSize(FONT_SIZE);
    layoutLabel.SetTextColor(UiColor(COLOR_TEXT));

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetBackgroundColor(UiColor(COLOR_BG));
    root.SetPadding(Insets(GAP, GAP, GAP, GAP));
    root.Add(mStatusLabel);
    root.Add(btnRequestFocus);
    root.Add(layoutLabel);
    root.Add(mLayout);
    contentArea.Add(root);

  }

private:
  Label mStatusLabel;
  View  mNonFocusable;
  View  mFocusableChild;
  View  mLayout;
};

REGISTER_MANUAL_TEST(TcFocusRequestFocus)
