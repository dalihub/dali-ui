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

#include <dali-ui-components/public-api/radio-button.h>
#include <dali-ui-components/public-api/styles/radio-button-style.h>
#include <dali-ui-components/public-api/text-button.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float    RADIO_SIZE   = 52.0f;
constexpr uint32_t COLOR_BODY   = 0x5F6368u;
constexpr uint32_t COLOR_STATUS = 0x137333u;

Label MakeLabel(const Dali::String& text, float fontSize = 14.0f, uint32_t color = COLOR_BODY)
{
  Label label = Label::New(text);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(WRAP_CONTENT);
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(color));
  return label;
}

StackLayout MakeRow()
{
  StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
  row.SetRequestedWidth(MATCH_PARENT);
  row.SetRequestedHeight(56.0f);
  row.SetSpacing(12.0f);
  return row;
}
} // unnamed namespace

/**
 * Interactive checks for parent-auto and named mutual exclusion, component-owned
 * select-only click behaviour, animation modes, disabled/read-only handling,
 * non-square layout, RTL, programmatic clearing, theme changes and accessibility.
 */
class TcRadioButtonBasics : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "RadioButton: grouping and states";
  }

  Dali::String GetDescription() const override
  {
    return "Verify single selection, animation/state variants, non-square and RTL layout. "
           "Clear selections programmatically and switch the system theme while resting/animating. "
           "With a screen reader, verify the radio role, checked state and supplied names.";
  }

  void OnEnter(View contentArea) override
  {
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(WRAP_CONTENT);
    root.SetSpacing(8.0f);
    root.SetPadding(Extents(16, 16, 16, 16));

    mStatus = MakeLabel("Select a radio button", 14.0f, COLOR_STATUS);

    root.Add(MakeLabel("Parent auto-group: only one direct child may be selected.", 16.0f));
    StackLayout autoRow = MakeRow();
    autoRow.Add(MakeRadio("Auto A"));
    autoRow.Add(MakeRadio("Auto B"));
    autoRow.Add(MakeRadio("Auto C"));
    root.Add(autoRow);
    mAutoGroup = SelectionGroup::Find(autoRow);

    root.Add(MakeLabel("Named group: members have different immediate parents.", 16.0f));
    StackLayout namedRow = MakeRow();
    for(const char* name : {"Named A", "Named B", "Named C"})
    {
      StackLayout wrapper = MakeRow();
      wrapper.SetRequestedWidth(72.0f);
      RadioButton radio = MakeRadio(name);
      radio.SetGroupName("radio-button-manual-named");
      wrapper.Add(radio);
      namedRow.Add(wrapper);
    }
    root.Add(namedRow);
    mNamedGroup = SelectionGroup::Find("radio-button-manual-named");

    root.Add(MakeLabel("AUTO / always animate / never animate", 16.0f));
    StackLayout animationRow = MakeRow();
    RadioButton automatic    = MakeRadio("AUTO");
    RadioButton always       = MakeRadio("Always animate");
    RadioButton never        = MakeRadio("Never animate");
    always.SetSelectionAnimationMode(SelectionAnimationMode::ENABLED);
    never.SetSelectionAnimationMode(SelectionAnimationMode::DISABLED);
    animationRow.Add(automatic);
    animationRow.Add(always);
    animationRow.Add(never);
    root.Add(animationRow);

    root.Add(MakeLabel("Read-only / disabled preselected / custom 48 x 32 RTL", 16.0f));
    StackLayout stateRow = MakeRow();
    RadioButton readOnly = MakeRadio("Read-only");
    readOnly.SetToggleByClickEnabled(false);

    RadioButton disabled = MakeRadio("Disabled preselected");
    disabled.SetSelected(true);
    disabled.SetEnabled(false);

    RadioButtonStyle nonSquareStyle = RadioButtonStyle::Default()
                                        .Configure()
                                        .SetIconWidth(48.0f)
                                        .SetIconHeight(32.0f)
                                        .Build();
    RadioButton nonSquare = RadioButton::New(nonSquareStyle);
    ConfigureRadio(nonSquare, "48 x 32 RTL");
    nonSquare.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);

    stateRow.Add(readOnly);
    stateRow.Add(disabled);
    stateRow.Add(nonSquare);
    root.Add(stateRow);

    TextButton clear = TextButton::New("Clear both groups");
    clear.SetRequestedWidth(MATCH_PARENT);
    clear.SetRequestedHeight(44.0f);
    clear.ClickedSignal().Connect(this, &TcRadioButtonBasics::OnClearClicked);
    root.Add(clear);
    root.Add(mStatus);
    root.Add(MakeLabel("Also switch the system theme during and after a selection animation."));

    contentArea.Add(root);
  }

private:
  RadioButton MakeRadio(const Dali::String& name)
  {
    RadioButton radio = RadioButton::New();
    ConfigureRadio(radio, name);
    return radio;
  }

  void ConfigureRadio(RadioButton radio, const Dali::String& name)
  {
    radio.SetRequestedWidth(RADIO_SIZE);
    radio.SetRequestedHeight(RADIO_SIZE);
    radio.SetAccessibilityName(name);
    radio.SetProperty(Actor::Property::NAME, name);
    radio.SelectionChangedSignal().Connect(this, &TcRadioButtonBasics::OnSelectionChanged);
  }

  void OnSelectionChanged(View view, bool selected, InputEvent /*event*/)
  {
    if(!mStatus || !selected)
    {
      return;
    }
    mStatus.SetText(view.GetProperty<Dali::String>(Actor::Property::NAME) + " selected");
  }

  void OnClearClicked(View /*view*/, InputEvent /*event*/)
  {
    mAutoGroup.ClearSelection();
    mNamedGroup.ClearSelection();
    mStatus.SetText("Parent and named groups cleared programmatically");
  }

  Label          mStatus;
  SelectionGroup mAutoGroup;
  SelectionGroup mNamedGroup;
};

REGISTER_MANUAL_TEST(TcRadioButtonBasics)
