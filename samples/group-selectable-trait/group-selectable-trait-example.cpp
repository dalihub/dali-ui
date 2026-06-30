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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/traits/group-selectable-trait.h>
#include <dali-ui-foundation/public-api/traits/selectable-trait.h>
#include <dali-ui-foundation/public-api/views/selection-group.h>

#include <cstdio>
#include <string>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;
using Dali::Ui::View;

namespace
{
const UiColor UNSELECTED_COLOR(0xE0E0E0); ///< Light grey for an unselected option
const UiColor SELECTED_COLOR(0x2962FF);   ///< Blue for the selected option
const UiColor LABEL_COLOR(0x000000);      ///< Black label text

constexpr int   OPTION_COUNT = 4;
constexpr float CIRCLE_SIZE  = 48.0f;
constexpr float CIRCLE_GAP   = 24.0f;

const char* const GROUP_NAME = "options"; ///< Named group shared by every option

/// English ordinal suffix for n (1 -> "st", 2 -> "nd", 3 -> "rd", else "th").
const char* OrdinalSuffix(int n)
{
  const int mod100 = n % 100;
  const int mod10  = n % 10;
  if(mod100 < 11 || mod100 > 13)
  {
    if(mod10 == 1) return "st";
    if(mod10 == 2) return "nd";
    if(mod10 == 3) return "rd";
  }
  return "th";
}
} // namespace

/**
 * GroupSelectableTrait sample: single-selection (radio-button style) grouping of
 * plain circular Views, stacked vertically, formed by parent auto-grouping and
 * observed through a SelectionGroup handle.
 *
 * Each option is an ordinary View made round with a relative corner radius. Each
 * option is made group-selectable with circle.AsGroupSelectable(), which composes
 * a SelectableTrait (which implies an InteractiveTrait). Children added under one
 * on-scene View parent auto-join that parent's group with no explicit group name.
 * This applies the select-only click policy, so:
 *   - tapping a circle selects it and auto-unselects the previously selected one;
 *   - re-tapping the selected circle is a no-op (true radio semantics);
 *   - a gesture can never empty the group.
 *
 * The parent auto-group is OBTAINED with SelectionGroup::Find(parent).
 * SelectionGroup::SelectedMemberChangedSignal(previous, current, event) recolours
 * the previous option grey and the current option blue, and updates the top label.
 *
 * The group has no group-level "select" setter; programmatic selection goes through
 * the member's SelectableTrait (view.AsSelectable().SetSelected(true)), and the
 * group can be emptied explicitly with SelectionGroup::ClearSelection().
 *
 * Keys:
 *   - 1 .. 4 : programmatically select that option (via its SelectableTrait)
 *   - c      : clear the selection (SelectionGroup::ClearSelection)
 *   - Escape / Back : quit
 */
class GroupSelectableTraitController : public ConnectionTracker
{
public:
  GroupSelectableTraitController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &GroupSelectableTraitController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Root vertical stack: the status label on top, then the options below it.
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetSpacing(CIRCLE_GAP);
    root.SetPadding(Extents(40, 40, 40, 40)); // start, end, top, bottom

    // Status label at the top, reporting the current selection.
    mLabel = Label::New("No View is selected");
    mLabel.SetTextColor(LABEL_COLOR);
    root.Add(mLabel);

    for(int i = 0; i < OPTION_COUNT; ++i)
    {
      View circle = View::New();
      circle.SetRequestedWidth(CIRCLE_SIZE);
      circle.SetRequestedHeight(CIRCLE_SIZE);
      circle.SetBackgroundColor(UNSELECTED_COLOR);

      // Make the (square) View a circle: a relative corner radius of 0.5 rounds
      // each corner to half the size.
      circle.SetCornerRadiusPolicyRelative();
      circle.SetCornerRadius(0.5f);

      // CENTER keeps the circle at its requested size (FILL would stretch it
      // across the stack width and turn it into an ellipse).
      circle.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER));

      // Make the View group-selectable: children added under the root will auto-join
      // the root's selection group, applying the select-only click policy.
      circle.AsGroupSelectable();

      root.Add(circle);
      mCircles.push_back(circle);
    }

    window.Add(root);

    // Obtain the parent auto-group to observe and control it.
    mGroup = SelectionGroup::Find(root);

    // On every selection change: previous option -> grey, current -> blue, and
    // update the top label to report which option (if any) is now selected.
    mGroup.SelectedMemberChangedSignal().Connect(
      this, [this](View previous, View current, InputEvent /*event*/)
    {
      if(previous)
      {
        previous.SetBackgroundColor(UNSELECTED_COLOR);
      }
      if(current)
      {
        current.SetBackgroundColor(SELECTED_COLOR);
      }
      UpdateLabel(current);
    });

    // Start with the first option selected. SelectionGroup has no group-level
    // setter, so programmatic selection goes through the member's SelectableTrait;
    // the group observes the change and enforces exclusivity.
    mCircles[0].AsSelectable().SetSelected(true);

    window.KeyEventSignal().Connect(this, &GroupSelectableTraitController::OnKeyEvent);
  }

  void UpdateLabel(View selected)
  {
    for(std::size_t i = 0; i < mCircles.size(); ++i)
    {
      if(selected && mCircles[i] == selected)
      {
        const int n = static_cast<int>(i) + 1;
        char      text[64];
        std::snprintf(text, sizeof(text), "%d%s View is selected", n, OrdinalSuffix(n));
        mLabel.SetText(text);
        return;
      }
    }
    mLabel.SetText("No View is selected");
  }

  void OnKeyEvent(Window window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
      return;
    }

    const std::string keyName = event.GetKeyName().CStr();

    // 'c' clears the selection through the only API route to an empty group.
    if(keyName == "c" || keyName == "C")
    {
      mGroup.ClearSelection();
      return;
    }

    // Number keys 1..OPTION_COUNT select that option programmatically.
    if(keyName.size() == 1 && keyName[0] >= '1' && keyName[0] <= static_cast<char>('0' + OPTION_COUNT))
    {
      const std::size_t index = static_cast<std::size_t>(keyName[0] - '1');
      if(index < mCircles.size())
      {
        mCircles[index].AsSelectable().SetSelected(true);
      }
    }
  }

private:
  Application&      mApplication;
  SelectionGroup    mGroup;
  Label             mLabel;
  std::vector<View> mCircles;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);

  // Initialise the UI configuration before any trait is created. Group membership
  // (AsGroupSelectable) creates an InteractiveTrait, which reads UiConfig (e.g. the
  // key-click policy) on construction.
  UiConfig config = UiConfig::New();
  config.Apply();

  GroupSelectableTraitController controller(application);
  application.MainLoop();
  return 0;
}
