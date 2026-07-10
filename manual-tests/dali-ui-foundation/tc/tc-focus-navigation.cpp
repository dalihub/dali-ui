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
constexpr uint32_t COLORS[]       = {0xE8D5E0, 0xD5E0D8, 0xD5DBE8, 0xE0D8D5};
constexpr uint32_t COLOR_FOCUSED  = 0xFF9800;
constexpr uint32_t COLOR_TEXT     = 0x3C3C3C;
constexpr uint32_t COLOR_BG       = 0xF7F5F3;
constexpr float    CELL_SIZE      = 160.0f;
constexpr float    GAP            = 12.0f;
constexpr float    FONT_SIZE      = 14.0f;
constexpr float    STATUS_HEIGHT  = 80.0f;
constexpr uint32_t VIEW_COUNT     = 4;
} // namespace

/**
 * @brief Verifies directional focus navigation (arrow keys).
 *
 * Four focusable Views are displayed in a 2x2 grid with explicit
 * directional focus wiring.  A status label at the top shows
 * which view currently has focus.
 *
 * Use arrow keys to move in 4 directions.
 */
class TcFocusNavigation : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "Focus Navigation";
  }

  Dali::String GetDescription() const override
  {
    return "Verifies directional (arrow keys)";
  }

  void OnEnter(View contentArea) override
  {
    mStatusLabel = Label::New();
    mStatusLabel.SetText("Press arrow keys to move focus");
    mStatusLabel.SetFontSize(FONT_SIZE);
    mStatusLabel.SetTextColor(UiColor(COLOR_TEXT));
    mStatusLabel.SetRequestedWidth(MATCH_PARENT);
    mStatusLabel.SetRequestedHeight(STATUS_HEIGHT);
    mStatusLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mStatusLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mStatusLabel.SetMultiLine(true);

    // Create 4 views in a 2x2 arrangement
    for(uint32_t i = 0; i < VIEW_COUNT; ++i)
    {
      mViews[i] = CreateFocusableView(i);
    }

    // Wire directional focus: 2x2 grid
    //   [0] [1]
    //   [2] [3]
    mViews[0].SetRightFocusableView(mViews[1]);
    mViews[0].SetDownFocusableView(mViews[2]);
    mViews[1].SetLeftFocusableView(mViews[0]);
    mViews[1].SetDownFocusableView(mViews[3]);
    mViews[2].SetRightFocusableView(mViews[3]);
    mViews[2].SetUpFocusableView(mViews[0]);
    mViews[3].SetLeftFocusableView(mViews[2]);
    mViews[3].SetUpFocusableView(mViews[1]);

    auto row0 = StackLayout::New(StackOrientation::HORIZONTAL);
    row0.SetRequestedWidth(MATCH_PARENT);
    row0.SetRequestedHeight(CELL_SIZE);
    row0.Add(mViews[0]);
    row0.Add(mViews[1]);

    auto row1 = StackLayout::New(StackOrientation::HORIZONTAL);
    row1.SetRequestedWidth(MATCH_PARENT);
    row1.SetRequestedHeight(CELL_SIZE);
    row1.Add(mViews[2]);
    row1.Add(mViews[3]);

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetBackgroundColor(UiColor(COLOR_BG));
    root.SetPadding(Extents(GAP, GAP, GAP, GAP));
    root.Add(mStatusLabel);
    root.Add(row0);
    root.Add(row1);
    contentArea.Add(root);

    FocusManager::Get().RequestFocus(mViews[0]);
  }

private:
  View CreateFocusableView(uint32_t index)
  {
    auto label = Label::New();
    label.SetText(Dali::String("View ") + Dali::String(std::to_string(index + 1).c_str()));
    label.SetFontSize(FONT_SIZE);
    label.SetTextColor(UiColor(COLOR_TEXT));
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetLayoutParams(StackLayoutParams::New().SetWeight(1));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    label.SetBackgroundColor(UiColor(COLORS[index]));
    label.SetMargin(Extents(GAP / 2, GAP / 2, GAP / 2, GAP / 2));
    label.SetFocusable(true);

    label.FocusChangedSignal().Connect(this, [this, index](View view, bool focused)
    {
      if(focused)
      {
        view.SetBackgroundColor(UiColor(COLOR_FOCUSED));
        mStatusLabel.SetText(Dali::String("Focused: View ") + Dali::String(std::to_string(index + 1).c_str()));
      }
      else
      {
        view.SetBackgroundColor(UiColor(COLORS[index]));
      }
    });

    return label;
  }

  Label mStatusLabel;
  View  mViews[VIEW_COUNT];
};

REGISTER_MANUAL_TEST(TcFocusNavigation)
