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
    return "34. InputField Focus";
  }

  Dali::String GetDescription() const override
  {
    return "Verifies InputField tap focus and ESC to clear";
  }

  void OnEnter(View contentArea) override
  {
    mStatusLabel = Label::New();
    mStatusLabel.SetText("Tap an InputField to give it focus");
    mStatusLabel.SetFontSize(FONT_SIZE);
    mStatusLabel.SetTextColor(UiColor(COLOR_TEXT));
    mStatusLabel.SetRequestedWidth(MATCH_PARENT);
    mStatusLabel.SetRequestedHeight(LABEL_HEIGHT);
    mStatusLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mStatusLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mStatusLabel.SetMultiLine(true);

    mField1 = CreateInputField("Field 1", "Type here...");
    mField2 = CreateInputField("Field 2", "Or here...");

    Label field1Label = Label::New();
    field1Label.SetText("Field 1:");
    field1Label.SetFontSize(FONT_SIZE);
    field1Label.SetTextColor(UiColor(COLOR_TEXT));
    field1Label.SetRequestedWidth(MATCH_PARENT);
    field1Label.SetRequestedHeight(30.0f);

    Label field2Label = Label::New();
    field2Label.SetText("Field 2:");
    field2Label.SetFontSize(FONT_SIZE);
    field2Label.SetTextColor(UiColor(COLOR_TEXT));
    field2Label.SetRequestedWidth(MATCH_PARENT);
    field2Label.SetRequestedHeight(30.0f);
    field2Label.SetMargin(Insets(0.0f, 0.0f, GAP, 0.0f));

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetBackgroundColor(UiColor(COLOR_BG));
    root.SetPadding(Insets(PAD, PAD, PAD, PAD));
    root.Add(mStatusLabel);
    root.Add(field1Label);
    root.Add(mField1);
    root.Add(field2Label);
    root.Add(mField2);
    contentArea.Add(root);

    FocusManager::Get().FocusChangedSignal().Connect(this, &TcInputFieldFocus::OnFocusChanged);
  }

private:
  InputField CreateInputField(const char* name, const char* placeholder)
  {
    auto field = InputField::New();
    field.SetPlaceholder(Dali::String(placeholder));
    // A fixed handle: with only a placeholder, the accessibility name IS the
    // content, so a selector written before typing matches nothing after
    // typing (review 34) — the automation id survives any input.
    field.SetAutomationId(name);
    field.SetFontSize(FONT_SIZE);
    field.SetTextColor(UiColor(COLOR_TEXT));
    field.SetRequestedWidth(MATCH_PARENT);
    field.SetRequestedHeight(FIELD_HEIGHT);
    field.SetBackgroundColor(UiColor(COLOR_FIELD_BG));

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
      // Say only what this handler can know: it fires for EVERY transition
      // to no-focus, and naming ESC as the cause was a claim the code never
      // verified (review 34).
      mStatusLabel.SetText("Focused: (none)");
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
