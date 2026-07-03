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
| `ViewState::FOCUS_INDICATED` | View has focus and its focus should be visibly indicated |
| `ViewState::PRESSED` | View is pressed via touch or key |
| `ViewState::DISABLED` | View is disabled |
| `ViewState::PSEUDO_DISABLED` | Appears disabled visually but remains interactive |
| `ViewState::SELECTED` | View is selected |

> [!NOTE]
> `FOCUSED` means the View currently has input focus. `FOCUS_INDICATED` means that the focused View should show visible focus feedback. For example, when the input context changes from key to touch, the View can keep `FOCUSED` while only `FOCUS_INDICATED` is cleared. This is similar to CSS [`:focus-visible`](https://www.w3.org/TR/selectors-4/#the-focus-visible-pseudo).

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
| View | `FOCUSED`, `FOCUS_INDICATED`, `DISABLED` |
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

  const InputEvent& cause = e.GetCause();
  if(!cause.IsProgrammatic()) {
    // input event caused this transition
  }

  if(cause.IsCancellation()) {
    // transition was caused by cancellation or reset
  }
});
```

> `InputEvent::IsProgrammatic()` returns `true` for code-driven changes such as `SetEnabled()`.

<br/>

## Triggering State Changes

Predefined states are managed automatically by the system:

| State | Managed by | How to change |
|-------|-----------|---------------|
| `FOCUSED` | Focus system | Automatic |
| `FOCUS_INDICATED` | Focus system | Automatic, based on how focus was reached and whether visible focus feedback should be shown |
| `PRESSED` | `InteractiveTrait` | Automatic on touch/key input |
| `DISABLED` | `View` | `view.SetEnabled(false / true)` |
| `PSEUDO_DISABLED` | `InteractiveTrait` | `interactiveTrait.SetPseudoDisabled(true / false)` |
| `SELECTED` | `SelectableTrait` | `selectableTrait.SetSelected(true / false)` |

```cpp
// DISABLED
view.SetEnabled(false);

// PSEUDO_DISABLED
InteractiveTrait interactive = view.AsInteractive();
interactive.SetPseudoDisabled(true);

// SELECTED (toggle on click)
SelectableTrait selectable = view.AsSelectable();
selectable.SetSelected(true);
selectable.SetToggleByClickEnabled(true);
```

<br/>

## Custom States

Custom states can be registered with `ViewState::Create()`. Calling `Create()` with the same name again returns the same bitmask.

```cpp
static const ViewState Loading = ViewState::Create("Loading");
static const ViewState Error   = ViewState::Create("Error");

auto loadingOrError = Loading + Error;
```

> Activating and deactivating custom states is done via `Integration::SetState()`, which targets Framework developers.

> `ViewState` has 32 total bit slots. Predefined states use some of those slots, and the remaining slots can be used for custom states. Registering beyond the available bit space throws a `DaliException`.

<br/>

## State Propagation

(TBD)

<br/>

## Framework Developer Notes

Use `Integration::SetState()` to turn a custom state on or off. `StateChangedSignal` is emitted automatically.

```cpp
Integration::SetState(viewImpl, Loading, true);   // activate Loading state
Integration::SetState(viewImpl, Loading, false);  // deactivate Loading state
```

For theme/color integration based on state, refer to the Framework Developer Notes in the Color & Theme document.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)
