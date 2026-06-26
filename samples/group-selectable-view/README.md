# group-selectable-view sample

Demonstrates single-selection (radio-button style) grouping with the
`GroupSelectableView` class and a `SelectionGroup`, using circular views stacked
vertically with a status label on top.

`GroupSelectableView` is a `SelectableView` subclass that has the
`GroupSelectableTrait` built in, so no explicit `AsGroupSelectable()` call is
needed - every option is simply created with `GroupSelectableView::New()` and is
group-selectable from birth (`GroupSelectableTrait` composes a `SelectableTrait`,
which implies an `InteractiveTrait`).

There are two samples, one per grouping mechanism (a set name wins over
parent-auto):

1. **Parent auto-grouping** (`group-selectable-view-example.cpp`): each option is a
   `GroupSelectableView circle = GroupSelectableView::New();`. Children added under
   one on-scene `View` parent auto-join that parent's group with no name at all.
   The group is obtained AFTER `window.Add(root)` via `mCircles[0].GetGroup()`
   (equivalent to `SelectionGroup::Find(parentView)`); `GetGroup()` is only valid
   once the member is on-scene.
2. **Named cross-parent grouping** (`group-selectable-view-group-name-example.cpp`):
   each `circle.SetGroupName("cross-hierarchy-group")` joins one shared named group
   spanning multiple parents; obtain it with `SelectionGroup::Find(name)`.

In both samples:

- Tapping an option selects it and unselects the previously selected one;
  re-tapping the selected option is a no-op (true radio semantics), and a gesture
  can never empty the group.
- `SelectedMemberChangedSignal(previous, current, event)` recolours the previous
  option grey and the current option blue, and updates the top `Label`
  (e.g. "1st View is selected"; empty -> "No View is selected").

## Programmatic control

`GroupSelectableView` exposes the selection API directly, and `SelectionGroup` is
emptied explicitly with `ClearSelection()`:

- `circle.SetSelected(true)` selects a member (no `.AsSelectable()` needed; the
  group observes the change and enforces single selection).
- `SelectionGroup::ClearSelection()` empties the group.
- `SelectionGroup::GetSelectedMember()` returns the current winner (or an empty
  handle).

## Keys

- `1` .. `N` : select that option programmatically (via `SetSelected`)
- `c`        : clear the selection (`SelectionGroup::ClearSelection`)
- `Escape` / `Back` : quit

`SelectionGroup` is the backing single-selection controller, not a visual
radio-button component; the colour/shape here is just sample styling.
