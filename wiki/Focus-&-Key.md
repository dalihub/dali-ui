[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Focus-&-Key-(kr))

## Overview

DALi UI provides a focus management system built around two core concepts:

- **FocusManager** — a singleton that controls which View currently has focus
- **View signals** — per-view callbacks for key events, focus changes, and state transitions

View have to gain focus to receives key events.

<br/>

## Making a View Focusable

By default Views are **not** focusable. You must opt in explicitly:

```cpp
View view = View::New();
view.SetFocusable(true);       // Keyboard navigation can reach this View
view.SetTouchFocusable(true);  // Touching the View also grants focus
```

Note that some Views are focusable by default — `InteractiveView`, `InputField`, etc. Additionally, calling `AsInteractive()` on a plain View to give it interactive traits also enables focusable automatically.

<br/>

## Moving Focus

### Programmatic Focus

```cpp
auto focusMgr = FocusManager::Get();

// Move focus to a specific View
focusMgr.SetCurrentFocusActor(button);

// Query the currently focused actor
Actor focused = focusMgr.GetCurrentFocusActor();

// Clear focus entirely
focusMgr.ClearFocus();
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
```

Available directions: `SetLeftFocusableView`, `SetRightFocusableView`, `SetUpFocusableView`, `SetDownFocusableView`, `SetClockwiseFocusableView`, `SetCounterClockwiseFocusableView`.

<br/>

### Focus Groups & Looping

```cpp
focusMgr.SetAsFocusGroup(container, true);  // Contain focus within this subtree
focusMgr.SetFocusGroupLoop(true);            // Wrap around at group boundaries
```

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
focusMgr.FocusChangedSignal().Connect(&tracker, [](Actor oldFocus, Actor newFocus) {
  // Focus moved from oldFocus to newFocus
});
```

<br/>

### Custom Focus Algorithm

Override the default focus-movement logic via `PreFocusChangeSignal`:

```cpp
focusMgr.PreFocusChangeSignal().Connect(&tracker,
  [](Actor current, Actor proposed, FocusDirection direction) -> Actor {
    // Return the actor that should receive focus.
    // Return proposed to accept the default, or a different actor to override.
    return proposed;
  });
```

<br/>

## InteractiveView: Clicked & LongPressed signals from Keys

`InteractiveView` converts key events into high-level interactions (click, long-press, pressed state). It is focusable by default and provides `ClickedSignal`, `LongPressedSignal`, and other signals directly:

```cpp
InteractiveView view = InteractiveView::New();

// Control when key-click fires
view.SetKeyClickPolicy(KeyClickPolicy::ON_PRESS);    // Fire Clicked signal on key-down

// Clicked signal — fires on Enter/Return key release (by default)
view.ClickedSignal().Connect(&tracker, [](View v, const InputEvent& event) {
  // Clicked (via touch tap or keyboard Enter)
});
```

<br/>

### Making a Plain View Interactive

Even without `InteractiveView`, you can give any `View` the same interaction capabilities by calling `AsInteractive()`. This also enables focusable automatically:

```cpp
View view = View::New();
view.AsInteractive([&](InteractiveTrait trait) {
  trait.ClickedSignal().Connect(&tracker, [](View v, const InputEvent& event) {
    // Clicked (via touch tap or keyboard Enter)
  });
});
```

If no configuration is needed, simply call `view.AsInteractive()` without a callback.

<br/>

### Global Defaults via UiConfig

`UiConfig` allows you to set default behavior for all interactive views:

| Setting | Description |
|---|---|
| `SetKeyClickPolicy` | Default key-click timing (ON_RELEASE / ON_PRESS / DISABLED) |
| `SetExecutionKeyPredicate` | Function that determines which keys trigger clicked/long-pressed execution (default: "Return") |
| `SetKeyLongPressThreshold` | Minimum threshold for recognizing a key long-press (number of consecutive key-pressed events) |

```cpp
bool MyKeyPredicate(const Dali::String& keyName)
{
  return keyName == "Return" || keyName == "KP_Enter";
}

UiConfig::New()
  .SetKeyClickPolicy(KeyClickPolicy::ON_RELEASE)   // Fire clicked signal when the key is released
  .SetExecutionKeyPredicate(MyKeyPredicate)        // Treat Return and KP_Enter as click keys
  .SetKeyLongPressThreshold(3)                     // Recognize as long-press when 3+ consecutive key-pressed events are detected
  .Apply();
```

> [!WARNING]
> `SetExecutionKeyPredicate` only accepts a plain function pointer (no capturing lambdas). Pass `nullptr` to restore the default ("Return").

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#documents)