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
constexpr uint32_t LABEL_COLORS[] = {0xE8D5E0, 0xD5E0D8, 0xD5DBE8};
constexpr uint32_t COLOR_TEXT     = 0x3C3C3C;
constexpr uint32_t COLOR_BG       = 0xF7F5F3;
constexpr float    LABEL_HEIGHT   = 120.0f;
constexpr float    LABEL_MARGIN   = 10.0f;
constexpr float    CONTAINER_PAD  = 16.0f;
constexpr float    FONT_SIZE      = 16.0f;
constexpr uint32_t LABEL_COUNT    = 3;
} // namespace

/**
 * @brief Verifies View::KeyEventSignal and View::FocusChangedSignal.
 *
 * Three focusable Labels are displayed.  Use Tab / arrow keys to move
 * focus between them, and press any key to verify that the focused Label
 * receives the key event.  Each Label shows:
 *   - Whether it currently has focus
 *   - The last key state (DOWN / UP)
 *   - The last key name
 */
class TcViewKeyEvent : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "View KeyEvent Signal";
  }

  Dali::String GetDescription() const override
  {
    return "Verifies KeyEventSignal and FocusChangedSignal on focusable Views";
  }

  void OnEnter(View contentArea) override
  {
    contentArea.Add(
      StackLayout::New(StackOrientation::VERTICAL)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(MATCH_PARENT)
        .SetBackgroundColor(UiColor(COLOR_BG))
        .SetPadding(Extents(CONTAINER_PAD, CONTAINER_PAD, CONTAINER_PAD, CONTAINER_PAD))
        .Children({
          CreateLabel(0),
          CreateLabel(1),
          CreateLabel(2)
        }));
  }

  Label CreateLabel(int index)
  {
    mLabels[index] = Label::New()
      .SetFontSize(FONT_SIZE)
      .SetTextColor(UiColor(COLOR_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(LABEL_HEIGHT)
      .SetBackgroundColor(UiColor(LABEL_COLORS[index]))
      .SetMargin(Extents(0, 0, 0, LABEL_MARGIN))
      .SetMultiLine(true)
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .SetFocusable(true);

    UpdateText(index);

    mLabels[index].KeyEventSignal().Connect(this, [this, index](View, const KeyEvent& event) mutable -> bool
    {
      UpdateLabelKeyState(index, event.GetState(), event.GetKeyName());
      return false;
    });

    mLabels[index].FocusChangedSignal().Connect(this, [this, index](View, bool focused) mutable
    {
      UpdateLabelFocusState(index, focused);
    });

    return mLabels[index];
  }

private:
  void UpdateLabelFocusState(uint32_t index, bool focused)
  {
    mLastFocusState[index] = focused;
    UpdateText(index);
  }

  void UpdateLabelKeyState(uint32_t index, KeyEvent::State state, Dali::String name)
  {
    mLastKeyState[index] = state == KeyEvent::DOWN ? "Press" : "Release";
    mLastKeyName[index] = name;
    UpdateText(index);
  }

  void UpdateText(uint32_t index)
  {
    Dali::String result = Dali::String("Label ") + Dali::String(std::to_string(index + 1).c_str());
    result += Dali::String("\nFocused: ");
    result += Dali::String(mLastFocusState[index] ? "YES" : "NO");
    result += Dali::String("\nKey State: ") + mLastKeyState[index];
    result += Dali::String("\nKey Name: ") + mLastKeyName[index];
    mLabels[index].SetText(result);
  }

  Label mLabels[LABEL_COUNT];
  bool mLastFocusState[LABEL_COUNT] = {false, };
  Dali::String mLastKeyState[LABEL_COUNT] = {"NONE", };
  Dali::String mLastKeyName[LABEL_COUNT] = {"NONE", };
};

REGISTER_MANUAL_TEST(TcViewKeyEvent)
