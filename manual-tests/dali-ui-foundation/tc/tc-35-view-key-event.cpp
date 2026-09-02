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
    return "35. View KeyEvent Signal";
  }

  Dali::String GetDescription() const override
  {
    return "Verifies KeyEventSignal and FocusChangedSignal on focusable Views";
  }

  void OnEnter(View contentArea) override
  {
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetBackgroundColor(UiColor(COLOR_BG));
    root.SetPadding(Insets(CONTAINER_PAD, CONTAINER_PAD, CONTAINER_PAD, CONTAINER_PAD));
    root.Add(CreateLabel(0));
    root.Add(CreateLabel(1));
    root.Add(CreateLabel(2));
    contentArea.Add(root);
  }

  Label CreateLabel(int index)
  {
    mLabels[index] = Label::New();
    mLabels[index].SetFontSize(FONT_SIZE);
    mLabels[index].SetTextColor(UiColor(COLOR_TEXT));
    mLabels[index].SetRequestedWidth(MATCH_PARENT);
    mLabels[index].SetRequestedHeight(LABEL_HEIGHT);
    mLabels[index].SetBackgroundColor(UiColor(LABEL_COLORS[index]));
    mLabels[index].SetMargin(Insets(0.0f, 0.0f, 0.0f, LABEL_MARGIN));
    mLabels[index].SetMultiLine(true);
    mLabels[index].SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mLabels[index].SetVerticalTextAlignment(Text::Alignment::CENTER);
    mLabels[index].SetFocusable(true);

    UpdateText(index);

    mLabels[index].KeyEventSignal().Connect(this, [this, index](View, KeyEvent event) mutable -> bool
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
  // {"NONE", } only filled index 0, so Labels 2 and 3 rendered an empty value
  // after the colon (review 35, confirmed on device).
  Dali::String mLastKeyState[LABEL_COUNT] = {"NONE", "NONE", "NONE"};
  Dali::String mLastKeyName[LABEL_COUNT] = {"NONE", "NONE", "NONE"};
};

REGISTER_MANUAL_TEST(TcViewKeyEvent)
