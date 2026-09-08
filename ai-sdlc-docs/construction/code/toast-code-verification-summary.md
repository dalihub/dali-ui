# Toast Code Verification Summary

## Scope

The Toast component provides transient OneUI-style feedback with message text,
an optional icon, finite display durations, explicit dismissal, and shown/hidden
lifecycle signals. Snackbar actions, expansion, and indefinite duration are not
part of this component.

## Layout and visual behavior

- The root uses a horizontal `StackLayoutManager` for icon/message placement and
  layout-direction-aware RTL mirroring.
- Width uses `WRAP_CONTENT` up to 68% of the posted Window width.
- Height uses `WRAP_CONTENT` up to 112 logical pixels by default.
- Message text uses word wrapping with no fixed maximum line count. Available
  content height determines the visible lines, and overflow uses ellipsis.
- The optional icon is 36 by 36 logical pixels by default and is removed from
  effective layout when its URL is empty.
- The Toast is centered horizontally, uses a 5% bottom offset, and moves above
  a visible input panel.

## Public API and documentation

- `Toast` and every public constructor, content property, duration API,
  presentation method, and lifecycle signal have Doxygen documentation.
- `ToastStyle`, every public getter, and every lvalue/rvalue builder setter have
  Doxygen documentation including validation and units where applicable.
- The sample README and English/Korean Wiki pages describe the optional-icon and
  long-text variants, finite durations, style defaults, accessibility,
  localization, and packaging.

## Verification coverage

- Default and custom `ToastStyle` values and validation
- Message wrapping without a fixed line-count limit
- Maximum width/height and responsive Window geometry
- Optional icon lifecycle, StackLayout placement, RTL, and UI scale
- Post/dismiss timing, reposting, external removal, IME avoidance, and signals
- Accessibility name and localization binding
- Toast sample build and packaged icon resource

## Latest verification

Verified on 2026-09-08 against `toast-refact` rebased on `upstream/devel`:

- The no-fixed-line-limit regression test failed before the implementation
  change because the Label reported `3` rather than unlimited.
- Message-only configuration, long-text height limiting, and four explicit
  line breaks passed after removing the fixed limit.
- The complete DALi UI Components suite passed 400 of 400 tests.
- The Toast sample built successfully.
- The wider Foundation suite passed 2385 of 2387 tests. The two failures are
  hover tests on paths unchanged from `upstream/devel` by this branch:
  `UtcDaliInteractiveTraitHoverMovesAcrossOverlappingSiblingsP` and
  `UtcDaliViewGeometryHoverFallsThroughOverlappingSiblingsP`.
