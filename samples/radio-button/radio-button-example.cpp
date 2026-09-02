/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// RadioButton sample: component-owned radio click semantics, parent-auto and named groups,
// explicit group clearing, animation modes, a non-square icon, and the recommended
// icon-plus-text row composition.

#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float RADIO_SIZE = 52.0f;

Label MakeLabel(const Dali::String& text, float fontSize = 15.0f)
{
  Label label = Label::New(text);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(WRAP_CONTENT);
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(0x202124u));
  return label;
}

StackLayout MakeHorizontalRow(float height = 56.0f)
{
  StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
  row.SetRequestedWidth(MATCH_PARENT);
  row.SetRequestedHeight(height);
  row.SetSpacing(12.0f);
  return row;
}
} // unnamed namespace

class RadioButtonExample : public ConnectionTracker
{
public:
  explicit RadioButtonExample(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &RadioButtonExample::Create);
  }

  void Create(Application application)
  {
    Components::UiConfig::New().Apply();

    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // A direct Window child has no Ui::View parent and therefore is not parent-auto-grouped.
    // Re-click remains a no-op because RadioButton owns a component-level select-only policy.
    RadioButton standalone = NewRadio("Standalone radio (component select-only)");
    standalone.SetRequestedWidth(RADIO_SIZE);
    standalone.SetRequestedHeight(RADIO_SIZE);
    standalone.SetRequestedX(24.0f);
    standalone.SetRequestedY(52.0f);
    standalone.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    standalone.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    window.Add(standalone);

    Label standaloneLabel = MakeLabel("Standalone: tap twice; it stays selected", 14.0f);
    standaloneLabel.SetRequestedWidth(360.0f);
    standaloneLabel.SetRequestedX(88.0f);
    standaloneLabel.SetRequestedY(64.0f);
    standaloneLabel.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    standaloneLabel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    window.Add(standaloneLabel);

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetSpacing(7.0f);
    root.SetPadding(Extents(24, 24, 120, 20));

    mStatus = MakeLabel("Select an option", 14.0f);
    mStatus.SetTextColor(UiColor(0x137333u));

    // Parent-auto group: direct RadioButton children of the same View parent.
    root.Add(MakeLabel("Parent auto-group", 17.0f));
    StackLayout autoRow = MakeHorizontalRow();
    autoRow.Add(NewRadio("Auto A"));
    autoRow.Add(NewRadio("Auto B"));
    autoRow.Add(NewRadio("Auto C"));
    root.Add(autoRow);
    mAutoGroup = SelectionGroup::Find(autoRow);

    // Named group: each RadioButton has a different parent but shares one explicit name.
    root.Add(MakeLabel("Named group across different parents", 17.0f));
    StackLayout namedRow = MakeHorizontalRow();
    for(const char* name : {"Named A", "Named B", "Named C"})
    {
      StackLayout wrapper = MakeHorizontalRow();
      wrapper.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
      RadioButton radio = NewRadio(name);
      radio.SetGroupName("radio-button-sample-named");
      wrapper.Add(radio);
      namedRow.Add(wrapper);
    }
    root.Add(namedRow);
    mNamedGroup = SelectionGroup::Find("radio-button-sample-named");

    // AUTO, ENABLED, and DISABLED animation modes.
    root.Add(MakeLabel("SelectionAnimationMode: AUTO / ENABLED / DISABLED", 17.0f));
    StackLayout animationRow = MakeHorizontalRow();
    RadioButton automatic    = NewRadio("AUTO");
    RadioButton enabled      = NewRadio("ENABLED");
    RadioButton disabled     = NewRadio("DISABLED");
    enabled.SetSelectionAnimationMode(SelectionAnimationMode::ENABLED);
    disabled.SetSelectionAnimationMode(SelectionAnimationMode::DISABLED);
    animationRow.Add(automatic);
    animationRow.Add(enabled);
    animationRow.Add(disabled);
    root.Add(animationRow);

    // The layout supports independent icon width and height.
    root.Add(MakeLabel("Custom 48 x 32 icon", 17.0f));
    RadioButtonStyle nonSquareStyle = RadioButtonStyle::Default()
                                        .Configure()
                                        .SetIconWidth(48.0f)
                                        .SetIconHeight(32.0f)
                                        .Build();
    RadioButton nonSquare = RadioButton::New(nonSquareStyle);
    ConfigureRadio(nonSquare, "Non-square 48 x 32");
    root.Add(nonSquare);

    // Recommended text row: the outer GroupSelectableView owns group/click/a11y.
    root.Add(MakeLabel("Text row composition", 17.0f));
    GroupSelectableView textRow = MakeTextRadioRow("Radio with a trailing label");
    root.Add(textRow);
    mTextGroup = SelectionGroup::Find("radio-button-sample-text");

    TextButton clear = TextButton::New("Clear auto / named / text groups");
    clear.SetRequestedWidth(MATCH_PARENT);
    clear.SetRequestedHeight(44.0f);
    clear.ClickedSignal().Connect(this, &RadioButtonExample::OnClearClicked);
    root.Add(clear);
    root.Add(mStatus);

    window.Add(root);
  }

private:
  RadioButton NewRadio(const Dali::String& name)
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
    radio.SelectionChangedSignal().Connect(this, &RadioButtonExample::OnSelectionChanged);
  }

  GroupSelectableView MakeTextRadioRow(const Dali::String& text)
  {
    GroupSelectableView row = GroupSelectableView::New();
    row.AttachLayoutManager(Dali::MakeUnique<StackLayoutManager>(StackOrientation::HORIZONTAL, 8.0f));
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(56.0f);
    row.SetPadding(Extents(8, 8, 2, 2));
    row.SetAccessibilityRole(Accessibility::Role::RADIO_BUTTON);
    row.SetGroupName("radio-button-sample-text");
    row.SetAccessibilityName(text);
    row.SetProperty(Actor::Property::NAME, text);

    RadioButton indicator = RadioButton::New();
    indicator.SetRequestedWidth(RADIO_SIZE);
    indicator.SetRequestedHeight(RADIO_SIZE);
    indicator.SetClickable(false);
    indicator.SetSensitive(false);
    indicator.SetFocusable(false);
    indicator.SetSelectionAnimationMode(SelectionAnimationMode::ENABLED);
    indicator.SetAccessibilityHidden(true);

    Label label = MakeLabel(text);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    label.SetAccessibilityHidden(true);
    label.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    row.Add(indicator);
    row.Add(label);
    row.SelectionChangedSignal().Connect(this, [indicator](View, bool selected, InputEvent) mutable
    {
      indicator.SetSelected(selected);
    });
    row.SelectionChangedSignal().Connect(this, &RadioButtonExample::OnSelectionChanged);
    return row;
  }

  void OnSelectionChanged(View view, bool selected, InputEvent /*event*/)
  {
    if(!selected)
    {
      return;
    }
    Dali::String name = view.GetProperty<Dali::String>(Actor::Property::NAME);
    mStatus.SetText(name + " selected");
  }

  void OnClearClicked(View /*view*/, InputEvent /*event*/)
  {
    mAutoGroup.ClearSelection();
    mNamedGroup.ClearSelection();
    mTextGroup.ClearSelection();
    mStatus.SetText("Groups cleared programmatically");
  }

private:
  Application&   mApplication;
  Label          mStatus;
  SelectionGroup mAutoGroup;
  SelectionGroup mNamedGroup;
  SelectionGroup mTextGroup;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application        application = Application::New(&argc, &argv);
  RadioButtonExample example(application);
  application.MainLoop();
  return 0;
}
