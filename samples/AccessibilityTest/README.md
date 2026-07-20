# AccessibilityTest

`accessibility-test.example` is a small DALi UI Foundation accessibility smoke app.

It creates several `Dali::Ui` views, configures accessibility role, name, description, value, state, scrollable, modal, hidden, and automation-id methods, then uses a mock accessibility client inside the same process to query the generated `Accessible` objects and invoke the `activate` action.

The sample validates the DALi UI -> DALi Adaptor accessibility path for the Phase 2 direct View API. It does not require a real screen reader or external AT-SPI client.

## Build

From `dali-ui/samples`:

```sh
cmake --fresh -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX .
make -j8 install
```

The parent samples CMake file auto-discovers this directory.

## What It Covers

- Public direct accessibility methods on `Dali::Ui::View`.
- `Dali::Ui::Accessibility::Role` and `Dali::Ui::Accessibility::State` conversion into adaptor accessibility data.
- Accessibility attributes and View automation-id metadata through direct methods.
- Hidden object handling.
- Mock action invocation through `Dali::Accessibility::Action`.
- Focus change caused by the default `OnAccessibilityActivate()` virtual path.
