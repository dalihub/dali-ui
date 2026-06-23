# group-selectable-trait sample

Demonstrates single-selection (radio-button style) grouping with
`GroupSelectableTrait` and a `SelectionGroup`, using plain circular Views stacked
vertically with a status label on top.

- Four options are ordinary `View`s made round with a relative corner radius
  (`SetCornerRadiusPolicyRelative()` + `SetCornerRadius(0.5f)`).
- Each option declares its membership with
  `circle.AsGroupSelectable().SetGroupName("options")`. Giving every option the
  SAME name binds them into one shared named group. `AsGroupSelectable()` makes
  the View group-selectable (`GroupSelectableTrait` composes a `SelectableTrait`,
  which implies an `InteractiveTrait`), and joining the named group applies the
  select-only click policy plus the `RADIO_BUTTON` accessibility costume.
- The shared group is obtained (not created) with `SelectionGroup::Find("options")`.
- Tapping an option selects it and unselects the previously selected one;
  re-tapping the selected option is a no-op (true radio semantics), and a gesture
  can never empty the group.
- `SelectedMemberChangedSignal(previous, current, event)` recolours the previous
  option grey and the current option blue, and updates the top `Label`
  (e.g. "1st View is selected"; empty -> "No View is selected").

There are exactly two ways to form a group (a set name wins over parent-auto):

1. Named grouping (this sample): `SetGroupName(name)` joins
   `SelectionGroup::Find(name)` (cross-parent, persistent).
2. Parent auto-grouping (default): `AsGroupSelectable()` children added under one
   on-scene `View` parent auto-join that parent's group with no name at all;
   obtain it via `SelectionGroup::Find(parentView)`.

## Programmatic control

`SelectionGroup` has no group-level "select" setter; programmatic selection goes
through the member's `SelectableTrait`, and the group is emptied explicitly with
`ClearSelection()`:

- `view.AsSelectable().SetSelected(true)` selects a member (the group observes the
  change and enforces single selection).
- `SelectionGroup::ClearSelection()` empties the group.
- `SelectionGroup::GetSelectedMember()` returns the current winner (or an empty
  handle).

## Keys

- `1` .. `4` : select that option programmatically (via its `SelectableTrait`)
- `c`        : clear the selection (`SelectionGroup::ClearSelection`)
- `Escape` / `Back` : quit

`SelectionGroup` is the backing single-selection controller, not a visual
radio-button component; the colour/shape here is just sample styling.
