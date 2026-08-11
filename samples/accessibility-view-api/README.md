# Accessibility View API

`accessibility-view-api.example` is a self-checking sample for the direct
accessibility API on `Dali::Ui::View`, the accessibility behavior hooks on
`Dali::Ui::ViewImpl`, and per-instance accessibility callbacks. It displays a
pass/fail report and writes virtual, callback, and reading-status dispatch to
stdout.

The sample callback subject is a plain `View`, demonstrating customization
without subclassing `ViewImpl`. The assertion code normally queries DALi's
generic platform `Accessible` interface. It downcasts to the integration
`ViewAccessible` type only to drive the scroll-to-child request because that
operation is not exposed by the generic `Accessible` interface.

## Name Resolution Contract

The effective accessible name is resolved in this order:

1. `OnAccessibilityRequestName()` when it returns `true`, including an
   intentionally empty value.
2. A non-empty explicitly configured (or translated) accessibility name.
3. `OnAccessibilityRequestDefaultName()` when it returns `true`, including an
   intentionally empty value.
4. The integration-only legacy raw-name fallback, then `Actor::Property::NAME`.

`OnAccessibilityRequestName()` is for an authoritative value resolved for each
accessibility request. `OnAccessibilityRequestDefaultName()` supplies subclass
semantics only when the application did not configure an explicit name.

## Description Resolution Contract

The effective accessible description follows the corresponding order:

1. `OnAccessibilityRequestDescription()` when it returns `true`, including an
   intentionally empty value.
2. A non-empty explicitly configured (or translated) accessibility description.
3. `OnAccessibilityRequestDefaultDescription()` when it returns `true`,
   including an intentionally empty value.
4. The integration-only legacy raw-description fallback.

## Build

From `dali-ui/samples`:

```sh
source ~/setenv
cmake --fresh -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX .
make accessibility-view-api.example -j8
DISPLAY=:0 ./accessibility-view-api/bin/accessibility-view-api.example
```

## Verification

The sample checks:

- Static name, description, value, role, flags, automation id, state, reading
  information, relation, language-span, translation, collection, and raw
  attribute methods.
- Dynamic name, description, and value virtuals for handled values,
  intentionally empty values, and fallback to stored values.
- Explicit-name precedence over the default-name hook, handled-empty
  defaults, and fallback from the default-name hook to the Actor name.
- Explicit-description precedence over the default-description hook,
  handled-empty defaults, and the legacy raw-description fallback.
- Direct virtual dispatch for activate, escape, increment, decrement,
  scroll-to-child, pan, and zoom.
- Per-instance activate, escape, value-change, scroll-to-child, name,
  default-name, description, default-description, and value callbacks on a
  plain `View` that does not use a custom `ViewImpl` subclass.
- Callback argument forwarding, dynamic/default request behavior, and restoring
  the existing virtual/property paths by setting empty callbacks.
- Pan and zoom callback setters are not exercised because `dali-ui` does not
  currently expose production accessibility entry points for those dispatchers.
- All five reading lifecycle events through one
  `AccessibilityReadingStatusChangedSignal()` and their exact enum order.
- The existing bool-based accessibility highlight Signal.
- `Dali::Ui::Extension::View::GrabAccessibilityHighlight()` when the sample subject
  receives keyboard focus. With a Screen Reader active, this emits the
  highlighted state change used to announce the subject; otherwise it reports
  an inactive bridge.
- Clear/remove methods after the checks complete.

Every displayed result row must begin with `PASS`, and the final report status must be `Overall: PASS`.
The stdout log must contain `virtual activate`, `virtual escape`, both value
change directions, `virtual scroll to child`, and reading status values
`0, 1, 2, 3, 4` in order. It must also contain `callback activate`,
`callback escape`, both callback value-change directions, and
`callback scroll to child`.

## Runtime Result

The checked-in screenshot and concise output were verified on 2026-08-04 with
the Ubuntu DALi desktop environment at 480x800, before the per-instance callback
checks were added. That baseline report contains 24 `PASS` rows. The current
source adds four callback rows, so a current run should contain 28 `PASS` rows
and `Overall: PASS`.

The baseline stdout sequence confirmed all action virtuals, reading statuses 0
through 4 in order, both highlight signal states, and the focus-triggered
highlight request. The baseline run reported an inactive accessibility bridge,
so no visual highlight overlay was added. The screenshot below was captured
from this run and shows the complete passing report.

![Accessibility View API passing all checks](accessibility-view-api-result.png)

The concise captured output is available in
[`accessibility-view-api-result.log`](accessibility-view-api-result.log).
