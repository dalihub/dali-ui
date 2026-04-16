[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/State-Management-(kr))

# State Management

## Overview

A View in dali-ui tracks its current condition as a **`ViewState`** bitmask. Multiple states can be active simultaneously. States can be combined with `+` and removed with `-`.

Whenever the state changes, `StateChangedSignal` fires and delivers a `StateEvent` that carries the previous and current states along with the optional input event that caused the transition.

<br/>

## Predefined States

| State | Description |
|-------|-------------|
| `ViewState::NORMAL` | Default state — no bits set |
| `ViewState::FOCUSED` | View has keyboard focus |
| `ViewState::PRESSED` | View is pressed via touch or key |
| `ViewState::DISABLED` | View is disabled |
| `ViewState::PSEUDO_DISABLED` | Appears disabled visually but remains interactive |
| `ViewState::SELECTED` | View is selected |

Commonly used composite states are also predefined:

| Composite State | Composition |
|----------------|-------------|
| `ViewState::SELECTED_PRESSED` | `SELECTED + PRESSED` |
| `ViewState::SELECTED_FOCUSED` | `SELECTED + FOCUSED` |
| `ViewState::DISABLED_SELECTED` | `DISABLED + SELECTED` |

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
ViewState state = view.GetState();

// All bits of the argument must be set
if(state.Contains(ViewState::PRESSED)) { ... }

// At least one bit in common
if(state.HasIntersectionWith(ViewState::DISABLED + ViewState::PSEUDO_DISABLED)) { ... }

// Check for NORMAL
if(state.IsNormal()) { ... }

// Debug string
Dali::String str = state.ToString(); // e.g. "Focused, Pressed"
```

When working with raw `prev`/`cur` arguments you can use helpers on `ViewState` itself:

```cpp
bool added      = ViewState::FOCUSED.WasAdded(prev, cur);
bool removed    = ViewState::FOCUSED.WasRemoved(prev, cur);
bool changed    = ViewState::FOCUSED.WasChanged(prev, cur);

// True if any constituent bit of the composite state changed
bool anyChanged = (ViewState::PRESSED + ViewState::FOCUSED).AnyChanged(prev, cur);
```

<br/>

## Reacting to State Changes

Connect to `View::StateChangedSignal()` to receive a `StateEvent` on every transition:

```cpp
view.StateChangedSignal().Connect(tracker, [](View v, const StateEvent& e) {
  if(e.Added(ViewState::FOCUSED))    { /* focus gained */ }
  if(e.Removed(ViewState::PRESSED))  { /* released */ }
  if(e.Changed(ViewState::DISABLED)) { /* enabled/disabled toggled */ }

  ViewState prev = e.GetPrev();
  ViewState cur  = e.GetCurrent();

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

Up to 62 custom states can be registered with `ViewState::Create()`. Calling `Create()` with the same name again returns the same bitmask.

```cpp
static const ViewState Loading = ViewState::Create("Loading");
static const ViewState Error   = ViewState::Create("Error");

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
