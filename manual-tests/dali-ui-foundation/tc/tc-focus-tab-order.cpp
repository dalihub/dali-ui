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
constexpr uint32_t COLORS[]       = {0xE8D5E0, 0xD5E0D8, 0xD5DBE8, 0xE0D8D5, 0xE0E0D5, 0xD5D5E0};
constexpr uint32_t COLOR_FOCUSED  = 0xFF9800;
constexpr uint32_t COLOR_TEXT     = 0x3C3C3C;
constexpr uint32_t COLOR_BG       = 0xF7F5F3;
constexpr uint32_t COLOR_DISABLED = 0xBDBDBD;
constexpr float    FONT_SIZE      = 14.0f;
constexpr float    GAP            = 8.0f;
constexpr float    STATUS_HEIGHT  = 60.0f;
} // namespace

/**
 * @brief Verifies Tab (FORWARD) and Shift+Tab (BACKWARD) focus ordering
 * with per-container geometry ordering corner cases.
 *
 * Layout:
 *
 *   [Scenario 1: Reverse-index position]
 *   Child index 0 is placed at x=200, index 1 at x=100, index 2 at x=0.
 *   Tab order should follow screen position (left→right), NOT child index.
 *   Expected: v2 → v1 → v0
 *
 *   [Scenario 2: Nested layouts]
 *   parent
 *     ├─ groupA  (x=0)
 *     │   ├─ a1
 *     │   └─ a2
 *     └─ groupB  (x=200)
 *         ├─ b1
 *         └─ b2
 *   Siblings are ordered by screen position, then each subtree is traversed.
 *   Expected: a1 → a2 → b1 → b2
 *
 *   [Scenario 3: Overlapping (centered squares)]
 *   Three squares centered: large(outer) > medium > small(inner).
 *   Tab order follows top-left position.
 *   Expected: large → medium → small
 */
class TcFocusTabOrder : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "Focus Tab Order";
  }

  Dali::String GetDescription() const override
  {
    return "Tab/Shift+Tab ordering with geometry corner cases";
  }

  void OnEnter(View contentArea) override
  {
    mStatusLabel = Label::New()
      .SetText("Press Tab / Shift+Tab to move focus")
      .SetFontSize(FONT_SIZE)
      .SetTextColor(UiColor(COLOR_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(STATUS_HEIGHT)
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .SetMultiLine(true);

    auto root = StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetBackgroundColor(UiColor(COLOR_BG))
      .SetPadding(Extents(GAP, GAP, GAP, GAP));

    root.Add(mStatusLabel);
    root.Add(CreateSectionLabel("Scenario 1: Reverse-index position"));
    root.Add(CreateScenario1());
    root.Add(CreateSectionLabel("Scenario 2: Nested layouts"));
    root.Add(CreateScenario2());
    root.Add(CreateSectionLabel("Scenario 3: Overlapping squares"));
    root.Add(CreateScenario3());

    contentArea.Add(root);

    FocusManager::Get().RequestFocus(mS1[2]);
  }

private:
  Label CreateSectionLabel(const char* text)
  {
    return Label::New()
      .SetText(text)
      .SetFontSize(FONT_SIZE)
      .SetTextColor(UiColor(COLOR_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(30.0f)
      .SetMargin(Extents(0, 0, GAP, 0));
  }

  // Scenario 1: child index 0 at rightmost, 2 at leftmost
  View CreateScenario1()
  {
    auto container = View::New()
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(60.0f);
    FocusManager::Get().SetAsFocusGroup(container, true);

    // Index 0 at x=200, index 1 at x=100, index 2 at x=0
    mS1[0] = CreateView("v0(idx0)", 0).SetRequestedPositionX(200.0f).SetRequestedPositionY(0.0f);
    mS1[1] = CreateView("v1(idx1)", 1).SetRequestedPositionX(100.0f).SetRequestedPositionY(0.0f);
    mS1[2] = CreateView("v2(idx2)", 2).SetRequestedPositionX(0.0f).SetRequestedPositionY(0.0f);
    container.Add(mS1[0]);
    container.Add(mS1[1]);
    container.Add(mS1[2]);
    return container;
  }

  // Scenario 2: two nested groups side by side
  View CreateScenario2()
  {
    auto container = View::New()
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(60.0f);
    FocusManager::Get().SetAsFocusGroup(container, true);

    auto groupA = View::New()
      .SetRequestedPositionX(0.0f)
      .SetRequestedPositionY(0.0f)
      .SetRequestedWidth(100.0f)
      .SetRequestedHeight(MATCH_PARENT);
    mS2[0] = CreateView("a1", 0).SetRequestedPositionY(0.0f);
    mS2[1] = CreateView("a2", 1).SetRequestedPositionY(30.0f);
    groupA.Add(mS2[0]);
    groupA.Add(mS2[1]);

    auto groupB = View::New()
      .SetRequestedPositionX(120.0f)
      .SetRequestedPositionY(0.0f)
      .SetRequestedWidth(100.0f)
      .SetRequestedHeight(MATCH_PARENT);
    mS2[2] = CreateView("b1", 2).SetRequestedPositionY(0.0f);
    mS2[3] = CreateView("b2", 3).SetRequestedPositionY(30.0f);
    groupB.Add(mS2[2]);
    groupB.Add(mS2[3]);

    container.Add(groupA);
    container.Add(groupB);
    return container;
  }

  // Scenario 3: three overlapping centered squares
  View CreateScenario3()
  {
    auto container = View::New()
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(160.0f);
    FocusManager::Get().SetAsFocusGroup(container, true);

    float cx = 150.0f, cy = 80.0f;

    mS3[0] = CreateView("large", 0)
      .SetRequestedPositionX(cx - 70.0f).SetRequestedPositionY(cy - 70.0f)
      .SetRequestedWidth(140.0f).SetRequestedHeight(140.0f);
    mS3[1] = CreateView("medium", 1)
      .SetRequestedPositionX(cx - 45.0f).SetRequestedPositionY(cy - 45.0f)
      .SetRequestedWidth(90.0f).SetRequestedHeight(90.0f);
    mS3[2] = CreateView("small", 2)
      .SetRequestedPositionX(cx - 20.0f).SetRequestedPositionY(cy - 20.0f)
      .SetRequestedWidth(40.0f).SetRequestedHeight(40.0f);

    container.Add(mS3[0]);
    container.Add(mS3[1]);
    container.Add(mS3[2]);
    return container;
  }

  View CreateView(const char* name, uint32_t colorIndex)
  {
    uint32_t ci = colorIndex % 6;
    auto label = Label::New()
      .SetText(name)
      .SetFontSize(FONT_SIZE)
      .SetTextColor(UiColor(COLOR_TEXT))
      .SetRequestedWidth(90.0f)
      .SetRequestedHeight(28.0f)
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .SetBackgroundColor(UiColor(COLORS[ci]))
      .SetFocusable(true)
      .SetTouchFocusable(true);

    mAllViews.push_back({label, name, COLORS[ci]});

    label.FocusChangedSignal().Connect(this, [this, idx = mAllViews.size() - 1](View view, bool focused)
    {
      if(focused)
      {
        view.SetBackgroundColor(UiColor(COLOR_FOCUSED));
        mStatusLabel.SetText(Dali::String("Focused: ") + mAllViews[idx].name);
      }
      else
      {
        view.SetBackgroundColor(UiColor(mAllViews[idx].originalColor));
      }
    });

    return label;
  }

  struct ViewEntry
  {
    Label        view;
    Dali::String name;
    uint32_t     originalColor;
  };

  Label                    mStatusLabel;
  View                     mS1[3]; // Scenario 1
  View                     mS2[4]; // Scenario 2
  View                     mS3[3]; // Scenario 3
  std::vector<ViewEntry>   mAllViews;
};

REGISTER_MANUAL_TEST(TcFocusTabOrder)
