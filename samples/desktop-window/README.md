# Desktop Window sample

A product window chrome built on `WindowFrame`, and a minimal application
written against it.

This sample exists to be **copied**, not linked. A product framework takes the
wrapper, adds what it needs, and every application in the product uses that
instead of touching `WindowFrame` directly.

## What to copy

| File | Take it? |
|---|---|
| `desktop-window.h` | Yes - the wrapper's interface |
| `desktop-window.cpp` | Yes - the wrapper |
| `desktop-window-example.cpp` | No - this is the demo application |
| `res/` | Icons; replace with the product's own |

Copying means forking: fixes made here later will not reach the copy. The
wrapper is deliberately small so that is a fair trade.

## What an application sees

```cpp
mWindow = std::make_unique<DesktopWindow>(window, DesktopWindow::CloseCallback::New(this, &App::Quit));
mWindow->SetTitle("Gallery");
mWindow->GetContentRoot().Add(myContent);
mWindow->AddBarAction("theme.png", DesktopWindow::ActionCallback::New(this, &App::ToggleTheme));
```

That is the whole window-related surface. No border thickness arithmetic, no
window-state tracking, no decoration handle to keep alive.

## What the wrapper owns

### The look

`BuildDecoration()` is the only place the decoration is decided: a top area,
side edges, corner resize handles, the shadow, and a bottom bar containing the
move region, application actions, and built-in minimize,
maximize/restore, and close buttons.
Changing the product's window appearance is an edit there, and no application
changes.

`WindowFrame` does the hard parts - geometry, native move and resize requests,
interaction lifecycle, overlay, size constraints - so what is left here is
layout.

### Window state

`UpdateStateDependentVisuals()` swaps the maximize icon for a restore icon and squares the
corners while maximized, driven by `WindowStateChangedSignal()`. Applications
never see it.

### Product policy

`OnCommand()` is the seam in front of the six default commands, reached through
`SetCommandInterceptor()`. Returning `HANDLED` means the wrapper owns that
command and `WindowFrame` performs no default operation for it. This is where a
confirmation before closing, or a profile that forbids minimizing, belongs -
somewhere an application cannot bypass.

`WindowFrameOptions::SetFeatureEnabled()` in the constructor is the other half:
a control the product does not offer is turned off once, not per application.

The wrapper deliberately does **not** hand out its `WindowFrame`. An
application holding one could replace the interceptor and bypass the policy
above, and a handle outliving the wrapper would leave `WindowFrame` calling
into a destroyed object - the interceptor is a plain callback bound to the
wrapper, not a tracked signal connection, which is why the destructor clears it.
A product that needs something more from the window should widen this class
rather than open the window up.

One thing a copied wrapper has to watch: a bar action's callback runs while the
wrapper is alive, but nothing stops that callback from destroying it. A plain
C++ object cannot retain itself the way a DALi handle-body can, so an action
that tears the window down should defer it rather than do it in place.

## Two things the layout has to get right

Both were found the hard way; the wrapper already handles them, and a product
extending it should keep them handled.

### Bar actions go beside the move region, never inside it

`WindowFrame` makes the move region insensitive while the window is maximized,
because a maximized window cannot be moved. In DALi an insensitive view hides
its children from hit testing as well, so a button parented to the move region
stops responding there - while still reporting `IsSensitive() == true`, which
makes it hard to spot.

Each action therefore takes a column of its own between the move region and
the built-in minimize, maximize/restore, and close buttons. The top area is safe
for the same reason: it is not the move region.

### The minimum frame size follows the columns

Every column in the bar except the move region is a fixed width the bar reserves
whatever the frame size is. Below their total the move region collapses and the
close control slides under the corner resize handle - and since the handle is
drawn on top, that area starts a **resize** instead of closing.

`RequiredFrameSize()` adds them up and `ApplyMinimumFrameSize()` applies the
larger of that and whatever the product asked for, so **adding an action raises
the floor**:

```
no actions   2x50 handles + 3x44 controls + 44 move region = 276
one action                 + 44                            = 320
```

The demo asks for `300 x 200` and ends up with `320 x 200` after its one action.

## Launching full size

With `WindowFrameInitialStatePolicy::AUTO` a window that comes up at screen size
is treated as maximized from the start. There is no earlier normal geometry for
the window system to restore to, so the constructor supplies one through
`WindowFrameOptions::SetInitialRestoreFrameSize()`. Keep it at or above what the bar
needs; below that the close control ends up under the corner resize handle.

It is consulted only for that first restore. Ordinary maximize and restore
geometry stays compositor-owned, which is what stops repeated toggling from
drifting the window across the screen.

## Overlay mode

`GetContentBounds()` is the whole frame in overlay mode, and
`WindowFrameGeometry::GetDecorationOverlayInsets()` reports how much decoration
covers. Apply it to **individual elements that must not be covered**, never as a
margin on the content root - insetting the root makes overlay layout identical
to ordinary inset layout, which defeats it.

## Building

```sh
samples/build.sh
```

The script configures and builds all samples. After it has configured
`samples/_build`, rebuild only this target with
`cmake --build samples/_build --target desktop-window.example`.

## Related

- `samples/window-frame` - every `WindowFrame` feature, as a showcase
- `wiki/WindowFrame.md` - the component reference, including `DefaultWindowDecoration`
  for applications that want a ready-made decoration instead of a product wrapper
