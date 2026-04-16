[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/State-Management-(kr))

# State Management

## Overview

A View in dali-ui tracks its current condition as a **`UiState`** bitmask. Multiple states can be active simultaneously. States can be combined with `+` and removed with `-`.

Whenever the state changes, `StateChangedSignal` fires and delivers a `StateEvent` that carries the previous and current states along with the optional input event that caused the transition.

<br/>

## Predefined States

| State | Description |
|-------|-------------|
| `UiState::NORMAL` | Default state — no bits set |
| `UiState::FOCUSED` | View has keyboard focus |
| `UiState::PRESSED` | View is pressed via touch or key |
| `UiState::DISABLED` | View is disabled |
| `UiState::PSEUDO_DISABLED` | Appears disabled visually but remains interactive |
| `UiState::SELECTED` | View is selected |

Commonly used composite states are also predefined:

| Composite State | Composition |
|----------------|-------------|
| `UiState::SELECTED_PRESSED` | `SELECTED + PRESSED` |
| `UiState::SELECTED_FOCUSED` | `SELECTED + FOCUSED` |
| `UiState::DISABLED_SELECTED` | `DISABLED + SELECTED` |

<br/>

### States Each View Type Can Have

| View type | States it can have |
|-----------|-------------------|
| View | `FOCUSED`, `DISABLED` |
| View (Interactive) | + `PRESSED`, `PSEUDO_DISABLED` |
| View (Selectable) | + `SELECTED` |

> [!NOTE]
> Use `AsInteractive()` or `AsSelectable()` to attach the corresponding Trait and expand available states.

<br/>

## Querying State

```cpp
UiState state = view.GetState();

// All bits of the argument must be set
if(state.Contains(UiState::PRESSED)) { ... }

// At least one bit in common
if(state.HasIntersectionWith(UiState::DISABLED + UiState::PSEUDO_DISABLED)) { ... }

// Check for NORMAL
if(state.IsNormal()) { ... }

// Debug string
Dali::String str = state.ToString(); // e.g. "Focused, Pressed"
```

When working with raw `prev`/`cur` arguments you can use helpers on `UiState` itself:

```cpp
bool added      = UiState::FOCUSED.WasAdded(prev, cur);
bool removed    = UiState::FOCUSED.WasRemoved(prev, cur);
bool changed    = UiState::FOCUSED.WasChanged(prev, cur);

// True if any constituent bit of the composite state changed
bool anyChanged = (UiState::PRESSED + UiState::FOCUSED).AnyChanged(prev, cur);
```

<br/>

## Reacting to State Changes

Connect to `View::StateChangedSignal()` to receive a `StateEvent` on every transition:

```cpp
view.StateChangedSignal().Connect(tracker, [](View v, const StateEvent& e) {
  if(e.Added(UiState::FOCUSED))    { /* focus gained */ }
  if(e.Removed(UiState::PRESSED))  { /* released */ }
  if(e.Changed(UiState::DISABLED)) { /* enabled/disabled toggled */ }

  UiState prev = e.GetPrev();
  UiState cur  = e.GetCurrent();

  if(e.HasCause()) {
    const InputEvent& cause = e.GetCause();
  }
});
```

> `HasCause()` returns `false` for programmatic changes such as `SetEnabled()`.

<br/>

## Triggering State Changes

Predefined states are managed automatically by the system:

| State | Managed by | How to change |
|-------|-----------|---------------|
| `FOCUSED` | Focus system | Automatic |
| `PRESSED` | `InteractiveTrait` | Automatic on touch/key input |
| `DISABLED` | `View` | `view.SetEnabled(false / true)` |
| `PSEUDO_DISABLED` | `InteractiveTrait` | `interactiveTrait.SetPseudoDisabled(true / false)` |
| `SELECTED` | `SelectableTrait` | `selectableTrait.SetSelected(true / false)` |

```cpp
// DISABLED
view.SetEnabled(false);

// PSEUDO_DISABLED
view.AsInteractive([](InteractiveTrait& t) {
  t.SetPseudoDisabled(true);
});

// SELECTED (toggle on click)
view.AsSelectable([](SelectableTrait& t) {
  t.SetSelected(true);
  t.EnableToggleByClick(true);
});
```

<br/>

## Custom States

Up to 62 custom states can be registered with `UiState::Create()`. Calling `Create()` with the same name again returns the same bitmask.

```cpp
static const UiState Loading = UiState::Create("Loading");
static const UiState Error   = UiState::Create("Error");

auto loadingOrError = Loading + Error;
```

> Activating and deactivating custom states is done via the integration-api (`ViewImpl::SetViewState()`), which targets Framework developers.

> Registering more than 62 custom states throws a `DaliException`.

<br/>

## State Propagation

(TBD)

<br/>

## Framework Developer Notes

Use `ViewImpl::SetViewState()` to turn a custom state on or off. `StateChangedSignal` is emitted automatically.

```cpp
// integration-api (view-impl.h)
SetViewState(Loading, true);   // activate Loading state
SetViewState(Loading, false);  // deactivate Loading state
```

For theme/color integration based on state, refer to the Framework Developer Notes in the Color & Theme document.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#documents)
