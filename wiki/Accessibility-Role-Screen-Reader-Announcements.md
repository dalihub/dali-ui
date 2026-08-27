# Dali UI Accessibility Roles and Screen Reader Announcements

This document describes the behaviour implemented by the current Dali UI and
screen-reader sources.  It uses the `SCREEN_READER_TV` build and the English
`en_US` PO catalogue as the reference configuration.

[한국어 버전](Accessibility-Role-Screen-Reader-Announcements-(kr).md)

## Scope and terminology

- **Focus announcement** is the description composed when the accessibility
  highlight moves to an object.
- **Name** is, in priority order, the `LABELLED_BY` relation name, accessible
  name, or text-interface name.  It is emitted only when `reading_info_type`
  enables `name` (all four reading-info categories are enabled by default).
- **Role** is emitted only when `reading_info_type` enables `role`.
- **State** is emitted only when `reading_info_type` enables `state`.
- `<ATSPI localized role>` means the English localized role returned by AT-SPI;
  it is not a string supplied by the screen-reader PO catalogue.

The Dali UI conversion is implemented in
`dali-ui-foundation/integration-api/view-accessible.cpp`, and the screen-reader
composition and event handling are implemented in `src/reading_composer.c` and
`src/app_tracker.c`.

## Focus-announcement ordering (TV)

The components below are separated by a pause (the implementation appends
`" , "`).  A missing component is simply omitted.

| Role group | English focus-announcement order |
| --- | --- |
| `ENTRY`, `PASSWORD_TEXT` | **Label/name → role → current text content**. For a password the content is spoken as a character count, not the characters. The generated `Editing` state is deliberately not appended in this TV-specific order. |
| `PROGRESS_BAR` | **Role → value/state**. The accessible name is not appended by the current TV composer. |
| `TOGGLE_BUTTON`, `SWITCH`, `RADIO_BUTTON`, `CHECK_BOX`, `SLIDER`, `SPIN_BUTTON` | **Name → role → state/value**. |
| All other roles | **(State) → name → role**. `State` is emitted only when that role produces a state trait. |
| Any role whose `described_by_accessible` has a non-empty role/state trait | The described object's corresponding trait replaces this object's role/state trait (one relation level only). |

On TV, an extended description follows the primary traits, and `disabled` is
always the final trait when the object lacks `ATSPI_STATE_ENABLED`.

## Role mapping and English focus speech

The following table covers every `Dali::Ui::Accessibility::Role`.  Text in
angle brackets is data-dependent; `—` means that the role itself adds no
role-specific English text beyond the common rules above.

| Dali UI role | AT-SPI role sent by Dali UI | Role phrase in English | State/value included on focus | Role-specific runtime state announcement |
| --- | --- | --- | --- | --- |
| `ADJUSTABLE` | `SLIDER` | `Slider` | Current value text/percent/number | No special rule; see common events. |
| `ALERT` | `ALERT` | `<ATSPI localized role>` | — | No special rule. |
| `BUTTON` | `PUSH_BUTTON` | `Button` | `Expanded` or `Collapsed` when expandable | `expanded` → `Expanded` / `Collapsed`. |
| `CHECK_BOX` | `CHECK_BOX` | `Check box` | `Checked` or `Not checked` | `checked` → `Checked` / `Not checked`. |
| `COMBO_BOX` | `COMBO_BOX` | `Dropdown button` | — | No special rule. |
| `CONTAINER` | `FILLER` | `<ATSPI localized role>` | — | No special rule. |
| `DIALOG` | `DIALOG` | Usually no role phrase; `Alert` is prefixed when `sub-role=Alert` | Number of shown items may be appended as description | `expanded` → `Expanded` / `Collapsed`. |
| `ENTRY` | `ENTRY` | `Edit box` | Current text content; `Editing` is not appended on TV | `expanded` → `Expanded` / `Collapsed` if emitted. Text insertion/deletion is separately handled while keyboard feedback is enabled. |
| `HEADER` | `HEADING` | `Heading` | Tree-table selection/count context where applicable | `expanded` → `Expanded` / `Collapsed`. |
| `IMAGE` | `IMAGE` | `Image` on TV | — | No special rule. |
| `LINK` | `LINK` | `<ATSPI localized role>` | — | `expanded` → `Expanded` / `Collapsed`; non-TV additionally appends an activation hint. |
| `LIST` | `LIST` | `<ATSPI localized role>` | — | No special rule. |
| `LIST_ITEM` | `LIST_ITEM` | Normally no role phrase; `Reorder, Button` when parent is animated | `Selected` in a tree table; selected/not-selected plus page position under a scroll bar | `selected` is announced only for a tree-table item, including its name and selection context. `animated` announces drag start/drop position. |
| `MENU` | `MENU` | `<ATSPI localized role>` | — | No special rule. |
| `MENU_BAR` | `MENU_BAR` | `<ATSPI localized role>` | — | No special rule. |
| `MENU_ITEM` | `MENU_ITEM` | `<ATSPI localized role>` | `Selected` when selected | `selected` is announced only when the parent is not `MENU`: name, tab position, then `Selected` (or the non-TV selection hint). |
| `NONE` | `UNKNOWN` | — | — | No special rule. |
| `NOTIFICATION` | `NOTIFICATION` | `<ATSPI localized role>` | — | No special rule. |
| `PASSWORD_TEXT` | `PASSWORD_TEXT` | `Edit box` | Character count only; never password text | See `ENTRY`; keyboard text feedback intentionally excludes passwords. |
| `POPUP_MENU` | `POPUP_MENU` | Usually no role phrase; `Alert` is prefixed when `sub-role=Alert` | On TV, item count is appended unless parent is a combo box | `expanded` → `Expanded` / `Collapsed`. |
| `PROGRESS_BAR` | `PROGRESS_BAR` | `progress bar` | Value text/percent/number; if numeric and below 100%, also `In progress` | `accessible-value` invokes the value-reading path. |
| `RADIO_BUTTON` | `RADIO_BUTTON` | `Radio button` | `Selected` when checked; otherwise `Not selected` except for editable controls or a color-chooser parent | `checked` falls through to the common `Selected` / `Not selected` rule. |
| `SCROLL_BAR` | `SCROLL_BAR` | Style dependent: alphabetical index bar, page `n of m`, or — | The style-specific page/index text is part of the role phrase | No special rule. |
| `SPIN_BUTTON` | `SPIN_BUTTON` | `<ATSPI localized role>` | Current value text/percent/number | No special rule; see common events. |
| `TAB` | `PAGE_TAB` | `Tab` | `Selected` only when parent is `PAGE_TAB_LIST` and the object is not legacy Elm | `selected=true` → name then `Selected`. |
| `TAB_LIST` | `PAGE_TAB_LIST` | `Tab` on TV | —; item count is supplied by the component/item description when needed | No special rule. |
| `TEXT` | `TEXT` | `Text box` | — | No special role rule. Text insertion/deletion uses the keyboard-feedback path when enabled. |
| `TOGGLE_BUTTON` | `TOGGLE_BUTTON` | `Toggle button` on TV; `Switch` on non-TV | `On` or `Off` | `checked` → `On` / `Off`. |
| `TOOL_BAR` | `TOOL_BAR` | `<ATSPI localized role>` | — | No special rule. |
| `SCENE_3D` | `FILLER` | `<ATSPI localized role>` | — | No special rule. |
| `MODEL` | `IMAGE` | `<ATSPI localized role>` | — | No special rule. |
| `SWITCH` | `SWITCH` | `Switch` on both TV and non-TV | `On` or `Off` | `checked` → `On` / `Off`. |
| `LABEL` | `LABEL` | — | — | No special rule. |
| `SCROLL_PANE` | `SCROLL_PANE` | `<ATSPI localized role>` | — | No special rule. |
| `TABLE` | `TABLE` | `Grid` on TV | — | No special rule. |
| `TABLE_CELL` | `TABLE_CELL` | `<ATSPI localized role>` | — | No special rule. |
| `TABLE_COLUMN_HEADER` | `TABLE_COLUMN_HEADER` | `<ATSPI localized role>` | — | No special rule. |
| `TABLE_ROW_HEADER` | `TABLE_ROW_HEADER` | `<ATSPI localized role>` | — | No special rule. |
| `EMBEDDED` | `EMBEDDED` | `Screen` on TV | — | No special rule. |

## Runtime state-change announcements

The state events below are processed only when the object's
`reading_info_type` includes `state`.  They are event announcements; they do
not repeat the whole focus description.

| AT-SPI event | Roles / condition | English announcement |
| --- | --- | --- |
| `state-changed:checked` | `TOGGLE_BUTTON`, `SWITCH` | `On` when set; `Off` when cleared. |
| `state-changed:checked` | `CHECK_BOX` | `Checked` when set; `Not checked` when cleared. |
| `state-changed:checked` | Every other role, including `RADIO_BUTTON` | `Selected` when set; `Not selected` when cleared. |
| `state-changed:selected` | `MENU_ITEM` whose parent is not `MENU` | `<name>, <tab index> of <count>, Selected` when set; non-TV uses an activation hint when cleared. |
| `state-changed:selected` | `LIST_ITEM` whose parent is `TREE_TABLE` | Only when set: name, image/selection context, and selected-count information. |
| `state-changed:selected` | `PAGE_TAB` | Only when set: `<name>, Selected`. |
| `state-changed:expanded` | Any role that emits this event | `Expanded` when set; `Collapsed` when cleared. |
| `property-change:accessible-value` | Any role | Calls the value-reading path. `PROGRESS_BAR`, `SLIDER`, and `SPIN_BUTTON` have the corresponding focus-time value traits described above. |
| `property-change:accessible-name` | Highlighted object whose reading info enables `name` | Speaks the new accessible name. |

## Important implementation notes

1. `LABEL`, `DIALOG`, `POPUP_MENU`, and several structural roles intentionally
   contribute no direct role text in `generate_role_trait_from_role()`; this
   is why their role phrase is blank even though an AT-SPI role exists.
2. `TEXT` is TV-specific: the role phrase `Text box` comes from
   `COM_SID_PRINCIPLE_CFJUL_TEXT_BOX`; non-TV leaves the role trait empty.
3. `TOGGLE_BUTTON` and `SWITCH` deliberately differ in role wording on TV:
   the former says `Toggle button`, while the latter always says `Switch`.
   Both use the same `On`/`Off` focus state and checked-state event speech.
4. This is a code-behaviour inventory, not a platform accessibility
   recommendation. A role can receive a generic checked or expanded event
   even when that state would not normally be semantically appropriate for
   that role.
