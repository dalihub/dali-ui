[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Focus-&-Key-(kr))

## Overview

DALi UI provides a focus management system built around two core concepts:

- **FocusManager** — a singleton that controls which View currently has focus
- **View signals** — per-view callbacks for key events, focus changes, and state transitions

Views must have focus to receive key events.

<br/>

## Focus Eligibility

A View can receive focus only when the focus system can resolve it as a valid focus target:

1. The View must be initialized.
2. The View must be attached to the View tree / scene.
3. The View must be visible.
4. The View must be enabled.
5. The View must be focusable. `View` is **not focusable by default**, so call `SetFocusable(true)` unless the View type enables it for you.
6. None of its ancestors may block descendant focus with `SetDescendantFocusBlocked(true)`.

```cpp
View button = View::New()
  .SetFocusable(true);

scene.Add(button);
FocusManager::Get().RequestFocus(button);
```

> [!IMPORTANT]
> For focus performance, mark containers that never contain focusable descendants with `SetDescendantFocusBlocked(true)`. This lets focus search skip the whole subtree instead of scanning children that cannot receive focus.

> [!NOTE]
> Some Views are focusable by default, such as `InteractiveView` and `InputField`. Calling `AsInteractive()` on a plain View also enables focusable automatically.

### Blocking descendant focus

`SetDescendantFocusBlocked(true)` prevents every descendant of a View from receiving focus:

```cpp
container.SetDescendantFocusBlocked(true);
```

Use this when a subtree is decorative or known not to contain focusable Views. `RequestFocus()`, `SetCurrentFocusView()`, and focus navigation reject descendants under such an ancestor.

<br/>

## Moving Focus

### Programmatic Focus

```cpp
auto focusMgr = FocusManager::Get();

// Set focus directly to one specific View
focusMgr.SetCurrentFocusView(button);

// Or, request focus with child delegation
focusMgr.RequestFocus(containerOrButton);

// Query the currently focused View
View focused = focusMgr.GetCurrentFocusView();

// Clear focus entirely
focusMgr.ClearFocus();
```

### SetCurrentFocusView vs RequestFocus

Use `RequestFocus()` for normal application focus requests. It resolves the request like below:

1. The target View receives a focus request.
2. Layout classes may delegate the request to an eligible focusable descendant.
3. The resolved View is committed as the current focus.
4. The request fails if the target or resolved candidate is blocked, not on scene, disabled, invisible, or not focusable.

Use `SetCurrentFocusView()` when you intentionally want to focus exactly that View with no child delegation. If the View itself is not focusable, the call fails even if it has focusable children.

```cpp
// Delegates to the first focusable child if layout itself is not focusable.
focusMgr.RequestFocus(layout);
```
```cpp
// Tries to focus layout itself. Does not delegate to children.
focusMgr.SetCurrentFocusView(layout);
```

<br/>

### Directional Focus

The framework automatically moves focus when the user presses arrow keys. You can also trigger it in code:

```cpp
focusMgr.MoveFocus(FocusDirection::RIGHT);
focusMgr.MoveFocus(FocusDirection::DOWN);
focusMgr.MoveFocusBackward();  // Return to the previously focused View
```

Available directions: `LEFT`, `RIGHT`, `UP`, `DOWN`, `PAGE_UP`, `PAGE_DOWN`, `FORWARD`, `BACKWARD`, `CLOCKWISE`, `COUNTER_CLOCKWISE`.

<br/>

### Explicit Focus Neighbors

You can wire up directional focus relationships between specific Views:

```cpp
viewA.SetRightFocusableView(viewB);
viewB.SetLeftFocusableView(viewA);
viewA.SetForwardFocusableView(next);
next.SetBackwardFocusableView(viewA);
```

Available setters: `SetLeftFocusableView`, `SetRightFocusableView`, `SetUpFocusableView`, `SetDownFocusableView`, `SetClockwiseFocusableView`, `SetCounterClockwiseFocusableView`, `SetForwardFocusableView`, `SetBackwardFocusableView`.

<br/>

### Focus Groups

```cpp
focusMgr.SetAsFocusGroup(container, true);  // Contain focus within this subtree
```

A focus group is a containment boundary for user-initiated navigation. Candidates
from View policies, explicit neighbors, the application fallback, and the default
finder must remain in the nearest focus group's subtree. The focus group View's
own local policy is included, but policies above it are not.

`RequestFocus()` and `SetCurrentFocusView()` are explicit programmatic operations,
so they may leave a focus group. This allows an application to close a dialog or
popup and restore focus elsewhere without a separate escape API.

<br/>

## Handling Key Events

A focused View emits `KeyEventSignal` for every key press/release. Return `true` to consume the event (preventing further propagation).

Using a member function:

```cpp
class MyController : public ConnectionTracker
{
public:
  void SetupKeyHandler(View view)
  {
    view.SetFocusable(true);
    view.KeyEventSignal().Connect(this, &MyController::OnKeyEvent);
  }

private:
  bool OnKeyEvent(View view, const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::State::DOWN)
    {
      if(event.GetKeyName() == "Return")
      {
        // Handle Enter key
        return true;  // consumed
      }
    }
    return false;  // let the event propagate
  }
};
```

The same can be written concisely with a lambda:

```cpp
view.KeyEventSignal().Connect(&tracker, [](View view, const KeyEvent& event) -> bool {
  if(event.GetState() == KeyEvent::State::DOWN)
  {
    if(event.GetKeyName() == "Return")
    {
      // Handle Enter key
      return true;  // consumed
    }
  }
  return false;  // let the event propagate
});
```

<br/>

## Reacting to Focus Changes

### Per-View Signal

```cpp
view.FocusChangedSignal().Connect(&tracker, [](View view, bool focused) {
  if(focused)
  {
    // View just gained focus
  }
  else
  {
    // View just lost focus
  }
});
```

<br/>

### Global Signal (FocusManager)

```cpp
focusMgr.FocusChangedSignal().Connect(&tracker, [](View oldFocus, View newFocus) {
  // Focus moved from oldFocus to newFocus
});
```

<br/>

### Custom Focus Navigation

Choose the narrowest extension point that owns the rule:

| Requirement | API | Scope and priority |
|---|---|---|
| A fixed relationship between a few Views | Directional neighbor setters | After local View policies |
| Policy for an existing container instance | `View::SetFocusNavigationCallback()` | That container's subtree |
| Policy implemented by a custom View class | `ViewImpl::OnFocusNavigationRequested()` | That View type; used only when no callback is set on the instance |
| Application-wide algorithm or initial focus selection | `FocusManager::SetFocusNavigationFallback()` | After local policies and explicit neighbors, before `FocusFinder` |
| Ordinary geometric or linear navigation | Framework `FocusFinder` | Final fallback |

Both View-local and application callbacks use this signature:

```cpp
FocusNavigationResult GetNextFocusableView(
  View current,
  FocusNavigationContext context);
```

`FocusNavigationResult` makes handling explicit:

- `NotHandled()` continues to the next policy.
- `MoveTo(view)` stops policy lookup and asks the framework to validate, resolve,
  and focus that candidate. An invalid candidate fails the request; it does not
  silently fall through.
- `Stay()` consumes the request without moving focus. `MoveFocus()` consequently
  returns `false`.

The immutable context exposes direction, device category, device name, original
`InputEvent`, source `Window`, and the nearest focus group. Policies must return a
result and must not call focus-changing or navigation APIs recursively.

#### Container policy

```cpp
FocusNavigationResult NavigatePanel(View current, FocusNavigationContext context)
{
  if(context.GetDirection() == FocusDirection::RIGHT)
  {
    if(View candidate = FindCustomRightTarget(current))
    {
      return FocusNavigationResult::MoveTo(candidate);
    }
    return FocusNavigationResult::Stay();
  }
  return FocusNavigationResult::NotHandled();
}

panel.SetFocusNavigationCallback(FocusNavigationCallback::New(&NavigatePanel));
```

Setting a callback replaces the instance's virtual policy; passing an empty
callback restores the virtual policy. The callback does not retain its target,
so clear it before a member-function target is destroyed.

#### Application fallback and initial focus

```cpp
class ApplicationNavigation
{
public:
  FocusNavigationResult GetNextFocusableView(
    View current,
    FocusNavigationContext context)
  {
    if(!current)
    {
      return FocusNavigationResult::MoveTo(FindEntryView(context.GetWindow()));
    }

    if(View candidate = FindApplicationCandidate(current, context))
    {
      return FocusNavigationResult::MoveTo(candidate);
    }
    return FocusNavigationResult::NotHandled();
  }
};

manager.SetFocusNavigationFallback(
  FocusNavigationCallback::New(&algorithm, &ApplicationNavigation::GetNextFocusableView));
```

Only one application fallback is stored. A new callback replaces it and an
empty callback clears it. It is invoked even when the framework default
algorithm is disabled. With no current focus, it runs first once a navigation
Window has been determined; if it returns `NotHandled()`, the default finder
runs when enabled. Programmatic `MoveFocus()` uses the current focus Window or
the last focused Window. If neither is available, the request fails without
invoking the fallback. Input-driven navigation can instead use its source
Window.

The complete priority is:

1. Local policy on each containing View, from nearest to outermost, including
   the nearest focus group View.
2. Explicit directional neighbor.
3. Application fallback.
4. Framework `FocusFinder`.

`OnFocusRequested()` serves a different purpose: after a policy selects a
container with `MoveTo()`, it resolves that container to the actual focusable
View. It is not a navigation-algorithm extension point.

#### Migrating from dali-toolkit

Replace `DevelKeyboardFocusManager::CustomAlgorithmInterface` as follows:

- Move component- or region-specific rules to the owning container callback or
  `OnFocusNavigationRequested()` override.
- Register application-wide rules with `SetFocusNavigationFallback()`.
- Handle an empty `current` in the same fallback to select initial focus.
- Return `Stay()` to stop default navigation, or `NotHandled()` to delegate.

Unlike the old raw algorithm pointer, the callback object is owned by the
manager, provides input/window/scope context, and produces an explicit result.
The callback's member-function target still remains caller-owned.

<br/>

## InteractiveView: Clicked & LongPressed signals from Keys

`InteractiveView` converts key events into high-level interactions (click, long-press, pressed state). It is focusable by default and provides `ClickedSignal`, `LongPressedSignal`, and other signals directly:

```cpp
InteractiveView view = InteractiveView::New();

// Control when key-click fires
view.SetKeyClickPolicy(KeyClickPolicy::ON_PRESS);    // Fire Clicked signal on key-down

// Clicked signal — fires on Enter/Return key release (by default)
view.ClickedSignal().Connect(&tracker, [](View v, const InputEvent& event) {
  // Clicked (via touch tap or execution key Enter)
});
```

<br/>

### Making a Plain View Interactive

Even without `InteractiveView`, you can give any `View` the same interaction capabilities by calling `AsInteractive()`. This also enables focusable automatically:

```cpp
View view = View::New();
InteractiveTrait interactive = view.AsInteractive();
interactive.ClickedSignal().Connect(&tracker, [](View v, const InputEvent& event) {
  // Clicked (via touch tap or execution key Enter)
});
```

If no configuration is needed, simply call `view.AsInteractive()` and ignore the returned trait.

<br/>

### Global Defaults via UiConfig

`UiConfig` allows you to set default behavior for all interactive views:

| Setting | Description |
|---|---|
| `SetKeyClickPolicy` | Default key-click timing (ON_RELEASE / ON_PRESS / DISABLED) |
| `SetExecutionKeyPredicate` | Function that determines which keys trigger clicked/long-pressed execution (default: "Return") |
| `SetLongPressKeyEventMinimumCount` | Minimum number of consecutive key-pressed events, including the initial key down, required to recognize a key long-press |

```cpp
bool MyKeyPredicate(const Dali::String& keyName)
{
  return keyName == "Return" || keyName == "KP_Enter";
}

UiConfig config = UiConfig::New();
config.SetKeyClickPolicy(KeyClickPolicy::ON_RELEASE);   // Fire clicked signal when the key is released
config.SetExecutionKeyPredicate(MyKeyPredicate);        // Treat Return and KP_Enter as click keys
config.SetLongPressKeyEventMinimumCount(3);             // Recognize as long-press when 3+ consecutive key-pressed events are detected
config.Apply();
```

> [!WARNING]
> `SetExecutionKeyPredicate` only accepts a plain function pointer (no capturing lambdas). Pass `nullptr` to restore the default ("Return").

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)
