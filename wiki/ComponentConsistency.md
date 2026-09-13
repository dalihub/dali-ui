# Component creation, styling, and verification

This guide covers the eight core styled components, the three Dialog types, and
DummyComponent. It describes API usage and verification entry points, not a claim
that the samples, desktop GUI, or assistive technology have been run successfully.
Foundation types such as Label and ScrollView are dependencies, not part of this
core-eight list. See also [Configuration](Configuration.md), [View](View.md),
[Focus and keys](Focus-&-Key.md), and [Accessibility](Accessibility.md).

## Apply one configuration

Use the highest-level configuration your application depends on. For these
components, apply `Dali::Ui::Components::UiConfig` once, before constructing styles
or controls. It includes the foundation configuration. Do not separately apply
the foundation `UiConfig` as well.

Register style creators before `Apply()`; construct their style values after
configuration is applied. A creator replacing a default must start from
`DefaultPreset()` rather than recursively requesting its own `Default()`.

```cpp
#include <dali-ui-components/dali-ui-components.h>

using namespace Dali;
using namespace Dali::Ui;

UiStyle CreateAppTextButtonStyle()
{
  return TextButtonStyle::DefaultPreset().Configure().SetFontSize(20.0f).Build();
}

// At application startup, before creating controls:
Components::UiConfig config = Components::UiConfig::New();
config.StyleSheet().SetStyle(TextButtonStyle::DefaultKey(), &CreateAppTextButtonStyle);
config.Apply();

// After Apply(): preserve the configured default and override one field locally.
TextButtonStyle local = TextButtonStyle::Default().Configure().SetFontSize(24.0f).Build();
TextButton button = TextButton::New("Continue", local);
button.SetText("Retry"); // Mutable content belongs to the component.
```

See the [configuration API](../dali-ui-components/public-api/components-ui-config.h)
and [style-sheet API](../dali-ui-components/public-api/styles/components-style-sheet.h).
`Default()` resolves the configured key, with the type's fallback where provided;
`DefaultPreset()` requests the built-in preset. `Configure()` clones an initialized
immutable style into a move-only Builder; `Build()` produces the new immutable
style. Copying a style handle does not make its contents mutable. Start from
`Default().Configure()` to preserve application overrides, or use `Builder()` when
you deliberately want builder defaults. Pass the resulting typed style to `New`.
Do not invoke instance methods on an uninitialized control or style handle.

## Core components: choose the matching state API

The links below refer to repository files; the public headers are the source of
truth for overloads, inherited APIs, and individual field preconditions.

| Component and public API | Style API | Content/state usage | Runnable example or scenario |
|---|---|---|---|
| [TextButton](../dali-ui-components/public-api/text-button.h) | [TextButtonStyle](../dali-ui-components/public-api/styles/text-button-style.h) | `New("Text", style)`, `SetText`, inherited `ClickedSignal`; layout and text setters remain mutable. | [Manual scenario](../manual-tests/dali-ui-components/tc/tc-text-button-basics.md) |
| [IconButton](../dali-ui-components/public-api/icon-button.h) | [IconButtonStyle](../dali-ui-components/public-api/styles/icon-button-style.h) | `New(style)` or `New(url, style)`, `SetIconUrl`, inherited `ClickedSignal`; supply a meaningful accessibility name. | [Sample instructions](../samples/icon-button/README.md), [guide](IconButton.md) |
| [CheckBox](../dali-ui-components/public-api/check-box.h) | [CheckBoxStyle](../dali-ui-components/public-api/styles/check-box-style.h) | `New("Text", style)` or box-only `New(style)`; inherited `SetSelected` / `SelectionChangedSignal`. Set outer requested dimensions explicitly as in the example. | [Sample](../samples/check-box/README.md), [manual scenario](../manual-tests/dali-ui-components/tc/tc-check-box-basics.md) |
| [RadioButton](../dali-ui-components/public-api/radio-button.h) | [RadioButtonStyle](../dali-ui-components/public-api/styles/radio-button-style.h) | Icon-only `New(style)`; parent-auto or named `SelectionGroup`; selected user re-click does not clear. `SetSelected(false)` or `ClearSelection()` can clear programmatically. | [Sample](../samples/radio-button/README.md), [manual scenario](../manual-tests/dali-ui-components/tc/tc-radio-button-basics.md), [guide](RadioButton.md) |
| [Switch](../dali-ui-components/public-api/switch.h) | [SwitchStyle](../dali-ui-components/public-api/styles/switch-style.h) | `New(style)`, inherited `SetSelected` / `SelectionChangedSignal`; tap, horizontal drag, and focused Enter operation. | [Sample](../samples/switch/README.md), [guide](Switch.md) |
| [ProgressBar](../dali-ui-components/public-api/progress-bar.h) | [ProgressBarStyle](../dali-ui-components/public-api/styles/progress-bar-style.h) | `New(min, max, style)`; inherited `SetValue` / `SetDeterminate`; local trail-end/divider geometry supports overlay synchronization. | [Manual scenario](../manual-tests/dali-ui-components/tc/tc-progress-bar-basics.md) |
| [Toast](../dali-ui-components/public-api/toast.h) | [ToastStyle](../dali-ui-components/public-api/styles/toast-style.h) | `New("Text", duration, style)`, `Post(window)`, `Dismiss`; `ShownSignal` / `HiddenSignal`. A Window overlay presentation, not a selection control. | [Sample](../samples/toast/README.md), [guide](Toast.md) |
| [MarkdownView](../dali-ui-components/public-api/markdown/markdown-view.h) | [MarkdownViewStyle](../dali-ui-components/public-api/styles/markdown-view-style.h) | `New(style)`, then `SetMarkdown(source)`; `GetMarkdown()` retrieves source and static `ToPlainText(source)` converts it. | [Sample instructions](../samples/markdown/README.md) |

CheckBox, RadioButton, and Switch share
[SelectionAnimationMode](../dali-ui-components/public-api/selection-animation-mode.h):
AUTO animates visible, on-scene user changes and snaps programmatic changes;
ENABLED animates visible, on-scene changes regardless of origin; DISABLED snaps.
`SetToggleByClickEnabled(false)` is not equivalent to `SetEnabled(false)` and does
not forbid programmatic state updates. Keep application state synchronized through
the state-change signal instead of assuming every state change came from a tap.
Icon-only controls need an application-provided accessible name. A compound radio
row should have one semantic/focus owner; see the RadioButton header before copying
the illustrative text-row sample into an application.

## Dialog family

| Type | Responsibility | Usage reference |
|---|---|---|
| [Dialog](../dali-ui-components/public-api/dialog/dialog.h) | Header/body/footer composition via `SetHeaderView`, `SetBodyView`, `SetFooterView`. | [Dialog guide](Dialog.md), [sample source](../samples/dialog/dialog-example.cpp) |
| [AlertDialog](../dali-ui-components/public-api/dialog/alert-dialog.h) | Convenience content through `SetTitle`, `SetMessage`, `AddActionButton`; wire the returned TextButton's action explicitly. | [Dialog guide](Dialog.md), [sample source](../samples/dialog/dialog-example.cpp) |
| [DialogContainer](../dali-ui-components/public-api/dialog/dialog-container.h) | Modal wrapper, scrim, and presentation ownership; distinct from the dialog's internal sections. | [Dialog guide](Dialog.md), [sample source](../samples/dialog/dialog-example.cpp) |

The typed-style creation APIs are `Dialog::New(DialogStyle)`,
`AlertDialog::New(AlertDialogStyle)`, and
`DialogContainer::New(DialogContainerStyle)`, while retaining parameterless `New()`.
DialogStyle owns the dialog's overall padding, section spacing, background, and
border effects; supplied section Views retain their own padding and appearance.
AlertDialogStyle adds typography/action-button policy for its generated content;
DialogContainerStyle owns the default scrim appearance. Start a local Alert style
from `AlertDialogStyle::Default().Configure()` after Components configuration.
See the linked public headers, [Dialog sample instructions](../samples/dialog/README.md),
and Dialog guide for the API. Modal role/state, content replacement, focus,
and screen-reader announcements need separate behavioral checks; a visual scrim
alone does not prove accessible modal behavior.

## Numeric input boundaries

For the remediated ordinary TextButton/IconButton/CheckBox geometry, and
RadioButton padding/minimum-size fields, the corresponding component-specific
setters require finite, nonnegative values. Negative
values, NaN, and infinities are not interchangeable with named layout sentinels.
The individual header remains authoritative about which fields allow a sentinel.

IconButton icon dimensions allow `WRAP_CONTENT` and `MATCH_PARENT` sentinels.
CheckBox icon dimensions require finite, nonnegative values without sentinels.
RadioButton retains its established normalization: nonpositive or nonfinite icon
dimensions normalize to zero (unset); its padding and minimum size still assert on
invalid values. These policies must not be generalized across all icon fields.
TextButton maximum dimensions allow `UNCONSTRAINED_MAX_SIZE`; constrained min/max
pairs must satisfy minimum ≤ maximum when the style is built, regardless of
builder setter order. Do not generalize this rule to every View setter or other
style family: for example, ProgressBar track thickness is finite and strictly
positive, and Toast duration is an integer number of milliseconds greater than
zero. Tests of invalid values should use assertion-aware UTCs, not feed invalid
values into an interactive demo and expect silent clamping.

## DummyComponent is not a feature control

[DummyComponent](../dali-ui-components/public-api/dummy-component.h) is a minimal
View-derived example retained as a public type. `DummyComponent::New()` is valid;
subclassing is not required. It adds no component-specific behavior, signals, or
typed style. Use `View::New()` for an ordinary visual node or container. There is
no dedicated Dummy sample and no claim of extra functionality based on its name.
The public header, symbols, and umbrella exposure remain available.

## Build, run, and record evidence separately

Use installed DALi core/adaptor/foundation/components packages from a matching
build. On desktop Linux, CMake uses pkg-config; configure `PKG_CONFIG_PATH` for a
non-system installation and make its shared libraries available to the loader.
The environment must also provide a supported display backend, fonts, and
installed component SVG/Lottie resources. See each sample README for exact targets.

```sh
cmake -S samples/icon-button -B /tmp/dali-ui-icon-button-build
cmake --build /tmp/dali-ui-icon-button-build --target icon-button.example
./samples/icon-button/bin/icon-button.example

cmake -S manual-tests/dali-ui-components -B /tmp/dali-ui-components-manual-build
cmake --build /tmp/dali-ui-components-manual-build
./manual-tests/dali-ui-components/bin/manual-test-dali-ui-components
```

The sample helper writes executables into the sample source directory's `bin/`,
even with an out-of-source build. The manual launcher provides TextButton,
CheckBox, RadioButton, and ProgressBar cases. Select a case, follow its same-basename
Markdown scenario, use `< Back` or hardware Back for the list, or Escape to exit.
There are no corresponding manual-launcher cases for the other types in this
guide; use their samples and explicitly record the uncovered scenarios.

Record build/link success, automated assertion results, visual interaction,
keyboard/focus, and actual screen-reader checks as separate evidence. A successful
build does not establish resource rendering, clipping, input feel, or announcements.
Do not infer a completed GUI/AT test from these instructions. Existing samples are
illustrative: the Markdown sample currently applies foundation `UiConfig` and
passes an explicit style rather than demonstrating the recommended Components
default-provider path; its README calls this out.
