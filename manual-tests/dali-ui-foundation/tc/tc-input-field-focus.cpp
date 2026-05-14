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
constexpr uint32_t COLOR_TEXT       = 0x3C3C3C;
constexpr uint32_t COLOR_BG         = 0xF7F5F3;
constexpr uint32_t COLOR_FIELD_BG   = 0xFFFFFF;
constexpr uint32_t COLOR_FIELD_EDGE = 0xCCCCCC;
constexpr uint32_t COLOR_FOCUSED_BG = 0xFFF3E0;
constexpr float    FIELD_HEIGHT     = 48.0f;
constexpr float    LABEL_HEIGHT     = 60.0f;
constexpr float    GAP              = 12.0f;
constexpr float    PAD              = 16.0f;
constexpr float    FONT_SIZE        = 16.0f;
} // namespace

/**
 * @brief Verifies InputField focus behavior: tap, and ESC to clear.
 *
 * Two InputFields and a status Label are displayed.
 * Tap an InputField to give it focus.  Press ESC to clear focus.
 * The status label shows which field (if any) is currently focused.
 */
class TcInputFieldFocus : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "InputField Focus";
  }

  Dali::String GetDescription() const override
  {
    return "Verifies InputField tap focus and ESC to clear";
  }

  void OnEnter(View contentArea) override
  {
    mStatusLabel = Label::New()
      .SetText("Tap an InputField to give it focus")
      .SetFontSize(FONT_SIZE)
      .SetTextColor(UiColor(COLOR_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(LABEL_HEIGHT)
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .SetMultiLine(true);

    mField1 = CreateInputField("Field 1", "Type here...");
    mField2 = CreateInputField("Field 2", "Or here...");

    contentArea.Add(
      StackLayout::New(StackOrientation::VERTICAL)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(MATCH_PARENT)
        .SetBackgroundColor(UiColor(COLOR_BG))
        .SetPadding(Extents(PAD, PAD, PAD, PAD))
        .Children({
          mStatusLabel,
          Label::New()
            .SetText("Field 1:")
            .SetFontSize(FONT_SIZE)
            .SetTextColor(UiColor(COLOR_TEXT))
            .SetRequestedWidth(MATCH_PARENT)
            .SetRequestedHeight(30.0f),
          mField1,
          Label::New()
            .SetText("Field 2:")
            .SetFontSize(FONT_SIZE)
            .SetTextColor(UiColor(COLOR_TEXT))
            .SetRequestedWidth(MATCH_PARENT)
            .SetRequestedHeight(30.0f)
            .SetMargin(Extents(0, 0, GAP, 0)),
          mField2
        }));

    FocusManager::Get().FocusChangedSignal().Connect(this, &TcInputFieldFocus::OnFocusChanged);
  }

private:
  InputField CreateInputField(const char* name, const char* placeholder)
  {
    auto field = InputField::New()
      .SetPlaceholder(Dali::String(placeholder))
      .SetFontSize(FONT_SIZE)
      .SetTextColor(UiColor(COLOR_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(FIELD_HEIGHT)
      .SetBackgroundColor(UiColor(COLOR_FIELD_BG));

    field.FocusChangedSignal().Connect(this, [](View view, bool focused)
    {
      if(focused)
      {
        view.SetBackgroundColor(UiColor(COLOR_FOCUSED_BG));
      }
      else
      {
        view.SetBackgroundColor(UiColor(COLOR_FIELD_BG));
      }
    });

    return field;
  }

  void OnFocusChanged(View /*previous*/, View current)
  {
    if(current == mField1)
    {
      mStatusLabel.SetText("Focused: Field 1");
    }
    else if(current == mField2)
    {
      mStatusLabel.SetText("Focused: Field 2");
    }
    else if(!current)
    {
      mStatusLabel.SetText("Focused: (none) - ESC cleared focus");
    }
    else
    {
      mStatusLabel.SetText("Focused: (other view)");
    }
  }

  Label      mStatusLabel;
  InputField mField1;
  InputField mField2;
};

REGISTER_MANUAL_TEST(TcInputFieldFocus)
