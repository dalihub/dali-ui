# Accessibility View API

`accessibility-view-api.example` is a self-checking sample for the direct
accessibility API on `Dali::Ui::View` and the accessibility behavior hooks on
`Dali::Ui::ViewImpl`. It displays a pass/fail report and writes every virtual
action and reading-status dispatch to stdout.

## Build

From `dali-ui/samples`:

```sh
cmake --fresh -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX .
make -j8
```

Run `accessibility-view-api/bin/accessibility-view-api.example` from a DALi
desktop environment.

## Verification

The sample checks:

- Static name, description, value, role, flags, automation id, state, reading
  information, relation, language-span, translation, collection, and raw
  attribute methods.
- Dynamic name, description, and value virtuals for handled values,
  intentionally empty values, and fallback to stored values.
- Direct virtual dispatch for activate, escape, increment, decrement,
  scroll-to-child, pan, and zoom.
- All five reading lifecycle events through one
  `AccessibilityReadingStatusChangedSignal()` and their exact enum order.
- The existing bool-based accessibility highlight Signal.
- `Dali::Ui::Extension::GrabAccessibilityHighlight()` when the sample subject
  receives keyboard focus. With a Screen Reader active, this emits the
  highlighted state change used to announce the subject; otherwise it reports
  an inactive bridge.
- Clear/remove methods after the checks complete.

Every displayed result row must begin with `PASS`, and the final report status must be `Overall: PASS`.
The stdout log must contain `virtual activate`, `virtual escape`, both value
change directions, `virtual scroll to child`, and reading status values
`0, 1, 2, 3, 4` in order.

## Runtime Result

Verified on 2026-07-22 with the Ubuntu-20.04 WSL DALi desktop environment at
480x800. The rendered report contained 16 `PASS` rows and `Overall: PASS`.
The stdout sequence also confirmed all action virtuals, reading statuses 0
through 4 in order, both highlight signal states, and the focus-triggered
highlight request. The baseline run reported an inactive accessibility bridge,
so no visual highlight overlay was added and the existing screenshot remains
valid.

![Accessibility View API passing all checks](accessibility-view-api-result.png)

The concise captured output is available in
[`accessibility-view-api-result.log`](accessibility-view-api-result.log).
