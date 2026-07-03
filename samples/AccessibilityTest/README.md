# AccessibilityTest

`accessibility-test.example` is a small DALi UI Foundation accessibility smoke app.

It creates several `Dali::Ui` views, configures accessibility role, name, description, value, state, scrollable, modal, hidden, and automation-id properties, then uses a mock accessibility client inside the same process to query the generated `Accessible` objects and invoke the `activate` action.

The sample is intended to validate the DALi UI -> DALi Adaptor accessibility path after the Phase 1 API refactoring. It does not require a real screen reader or external AT-SPI client.

## Build

From `dali-ui/samples`:

```sh
cmake --fresh -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX .
make -j8 install
```

The parent samples CMake file auto-discovers this directory.

## What It Covers

- Public `Dali::Ui::View::Property::ACCESSIBILITY_*` properties.
- `Dali::Ui::Accessibility::Role` and `Dali::Ui::Accessibility::State` conversion into adaptor accessibility data.
- Accessibility attributes through `AUTOMATION_ID` and `ACCESSIBILITY_ATTRIBUTES`.
- Hidden object handling.
- Mock action invocation through `Dali::Accessibility::Action`.
- Focus change caused by the default `activate` accessibility action.
