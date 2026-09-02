# WindowFrame Sample

This sample demonstrates client-rendered decoration for a partial DALi window.
It keeps application content in a dedicated content root and composes the
window decoration above it.

## Features

- Replaceable top, bottom, left, and right decoration views
- Replaceable bottom-left and bottom-right resize handle views
- Runtime decoration replacement with different layout
- A decoration-provided bottom-bar move region
- Explicit bottom-left and bottom-right resize handles
- Image-based minimize, maximize/restore, and close commands in the bottom bar
- Rounded top and bottom outer corners
- Frame minimum and maximum size conversion
- Optional maximize overlay layout
- Configurable overlay decoration auto-hide while maximized; touch to reveal
- Free and fixed resize policies

The Style button replaces the complete decoration and frame style in one
transaction while preserving the content root and window signal connections.
The Overlay and Resize buttons change window-frame policy without rebuilding the
decoration. Resize cycles through `FREE`, `KEEP_ASPECT_RATIO`, and `DISABLED`.

The sample requests a `400 x 260` visible frame through `RequestFrameResize()`. A client
shadow adds its transparent outsets to the normal native surface; the default
`(14, 14, 14, 18)` outsets produce a `428 x 292` default surface.

The minimum is set per installed frame by `ApplyFrameMinimumSize()`, because each
frame has its own floor. The application's own content needs `200 x 130`; a
`DefaultWindowDecoration` needs `276 x 55` for its control and handle columns, which it
reports through `GetMinimumFrameSize()`. The sample takes the larger of the two
and reapplies it whenever the frame changes.
Runtime Style changes preserve content size while the window is in its normal
state.

Use `RequestFrameResize()` when the application specifies an outer window-frame
size. The component converts it to the native surface size including client shadow
outsets. `RequestContentResize()` remains available when application content is the
stable sizing contract.

Use `SetMinimumFrameSize()` and `SetMaximumFrameSize()` for persistent
product-visible window limits. Constraints are converted to native surface
space and recalculated after runtime shadow changes.

Touching the status area between the resize handles and window-control
buttons starts server-side window movement. The resize handles and control
buttons remain separate input views, so they do not initiate a move. Each
window-control input view fills its complete bottom-grid cell while the
visible `34 x 34` button surface stays centered in that cell.
The sample always installs all window-control and resize role views so its
decoration layout is consistent across backends. WindowFrame enables every
feature by default instead of inferring support from the compiled backend.
Applications can explicitly override a feature through `WindowFrameOptions`
before creating the WindowFrame.

## Default window decoration

The sample starts on `DefaultWindowDecoration`, the ready-made decoration, installed by
`InstallDefaultDecoration()`:

```cpp
mDefaultDecoration = DefaultWindowDecoration::New(mWindowFrame);
mDefaultDecoration.GetMoveRegion().Add(title);
```

That is the complete decoration: thin edges along the top and both sides and a
bottom bar ordered as bottom-left resize handle, expandable move region,
application actions, built-in minimize, maximize/restore and close buttons, and
bottom-right resize handle. The frame
background is transparent, so only the decoration paints, and the frame casts a
shadow into the surface outside it. The sample's window is created transparent,
which the shadow requires. Its icons ship with the component and are
installed under `<dataReadOnlyDir>/ui/images/components/border/`; the copies in
this sample's `res/` are used by the custom frames below.

The handle owns the connection that follows the window state - the
maximize/restore icon and the corner radius that squares off when maximized - so
the sample keeps it in the `mDefaultDecoration` member and releases it before
installing a custom frame over the top.

## Adding an application action to the bar

`WindowFrame` has no `GetDecoration()`, so an application holding only the
window handle cannot reach the installed views. Whoever builds the decoration
has to hand out an entry point. `AddBarAction()` is this sample's version of
that seam: the application supplies an icon and what to do, and the frame builder
decides where the button goes.

```cpp
AddBarAction("theme.png", [this] { ToggleContentTheme(); });
```

The button gets **its own column beside the move region**, not a place inside it.
`WindowFrame` makes the move region insensitive while the window is maximized,
because a maximized window cannot be moved, and an insensitive view hides its
children from hit testing as well. A button parented to the move region - or to
`DefaultWindowDecoration::GetMoveRegion()`, which is the same view - therefore stops
responding the moment the window is maximized. The built-in minimize,
maximize/restore, and close buttons do not have this problem, so sitting beside
them is the safe placement.

Actions are stored rather than added to the live frame, because the frame is
rebuilt whenever the style changes and each rebuild has to reproduce them. Each
one also reserves a fixed column, so `CustomFrameMinimumSize()` counts them and
the minimum frame size grows with them.

A theme toggle is deliberately not a window operation: it changes the
application's own content, so it is an ordinary touch handler rather than a
command routed through `SetCommandInterceptor()`.

This showcase stores the action in its custom-frame model, so the button appears
on the two custom frames only. `DefaultWindowDecoration` users can add the same kind of
button directly with `DefaultWindowDecoration::AddBarAction()`. The sample starts on
`DefaultWindowDecoration`, so press **Style** once to see the custom action.

The **Style** button in the content area cycles default decoration -> custom 1 ->
custom 2, which shows the same window with the ready-made frame and with a
fully hand-built one. It sits in the content root rather than in the decoration, so
replacing the frame is driven from the application side exactly as a product
would do it.

## Custom decoration

Applications create their own decoration views and install them by composition:

```cpp
WindowFrameOptions options;
options.SetInitialRestoreFrameSize(Vector2(400.0f, 260.0f));

WindowFrame windowFrame = WindowFrame::New(window, closeCallback, options);
windowFrame.SetInteractiveResizePolicy(WindowFrameInteractiveResizePolicy::FREE);
windowFrame.SetOverlayEnabled(false);

WindowFrameDecoration decoration;
decoration.SetTopSlot(customHeader);
decoration.SetBottomSlot(customFooter);
decoration.SetLeftSlot(customLeftBorder);
decoration.SetRightSlot(customRightBorder);
decoration.SetMoveRegion(titleMoveRegion);
decoration.SetMinimizeControl(minimizeButton);
decoration.SetMaximizeRestoreControl(maximizeButton);
decoration.SetCloseControl(closeButton);
decoration.SetBottomLeftResizeHandle(customBottomLeftHandle);
decoration.SetBottomRightResizeHandle(customBottomRightHandle);

WindowFrameStyle style;
style.SetShadowOutsets(Insets(14.0f, 14.0f, 14.0f, 18.0f));
style.SetShadow(Shadow(
  14.0f,
  Vector2(0.0f, 3.0f),
  UiColor(0x000000, 0.48f)));

const auto result = windowFrame.SetDecoration(decoration, layout, style);
windowFrame.WindowStatePresentedSignal().Connect(
  tracker,
  [](WindowFrame sender, WindowFrame::WindowState state) {
    static_cast<void>(sender);
    // Update product UI after the confirmed frame is presented.
  });
windowFrame.Attach();
```

Every WindowFrame signal passes the emitting `WindowFrame` as its first
argument. This lets one observer safely distinguish events when it is connected
to more than one frame.

`SetDecoration()` is the preferred initial and runtime installation API. It
validates the complete view topology before modifying the current frame, then
applies decoration, layout, and shadow style with one final native resize.
The default size policy preserves content size; callers can pass
`WindowFrameSizePolicy::KEEP_WINDOW_SIZE` when the window size must
remain unchanged. `SetDecorationLayout()` and `SetFrameStyle()` remain useful
for isolated updates where the other half of the frame contract is unchanged.
The policy belongs only to the call that receives it; it is not retained for a
later layout or style update.

Configure options and the initial frame before `Attach()`. `GetContentRoot()`
returns the stable application-content parent before or after attachment;
replacing or clearing decoration and a `Detach()`/`Attach()` cycle do not replace
this root or its content. Stop retaining the root after the last WindowFrame
handle is destroyed. A frame replacement that changes native
geometry is safe from a DALi input callback: WindowFrame defers the resulting
native resize to the next event-loop tick and coalesces repeated requests.

Top, bottom, left, right, and both resize handles are unparented slot roots
owned by the WindowFrame after installation. Move and window-control views are role
references that must be one of those roots or a descendant; the component connects
their standard operations without reparenting them. Invalid parentage,
duplicate slots or roles, and roles outside the decoration are rejected before
the current decoration is changed. `WindowFrameDecorationResult` reports the
reason.

## Window state

`GetWindowState()` returns the compositor-confirmed `NORMAL`, `MAXIMIZED`, or
`MINIMIZED` state. `IsMaximized()` and `IsMinimized()` are convenience queries
for that confirmed state, so a dispatched command is never mistaken for
compositor completion.

`RequestMinimize()`, `RequestMaximize()`, and `RequestRestore()` returning
`DISPATCHED` means only that the underlying operation was invoked. Window-system
calls return no result, so neither acceptance nor completion is implied. Observe
`WindowStateChangedSignal()` for the resulting state, and observe
`WindowStatePresentedSignal()` when that state must also have reached a
presented frame.

An explicit request is dispatched even when the window already appears to be in the
requested state, so a `RequestRestore()` sent immediately after
`RequestMaximize()` is the one that lands. `ToggleMaximize()` is the exception:
a toggle has no target of its own and reads the last reported state.

Use `IsFeatureEnabled()` when a product decides whether to expose a native
control or rely on a presentation callback. These are gates the caller declares,
not runtime backend detection: every feature is `true` by default whatever the
backend implements. A product turns off an operation it does not want to offer
before creation, for example
`options.SetFeatureEnabled(WindowFrameFeature::MINIMIZE, false)`.

Normal maximize and restore operations leave both position and size under
compositor ownership. The component does not call `SetPositionSize()` from a
restore notification. This avoids racing a compositor configure sequence or
overwriting the compositor's restored position.

Shadow outsets have one source of truth: `WindowFrameStyle`. `WindowFrameLayout`
contains only decoration insets and explicit resize-handle sizes. Size
constraints can be queried and cleared individually. A conflicting constraint
returns `false` without changing the previous configuration. Native constraints are active only while WindowFrame is
attached; detach clears them and reattach reapplies the stored configuration.

The size set by `WindowFrameOptions::SetInitialRestoreFrameSize()` is retained only
when the `AUTO` launch policy detects that the application's initial surface
already covers the full target bounds. That launch uses ordinary maximize. The
size is passed through `MaximizeWithRestoreSize(false, size)` on the first
restore and then consumed. Explicit maximize policy, normal-size launches and
later maximize/restore cycles leave restore geometry to the compositor.
`RequestFrameResize()` and `RequestContentResize()` return `false` while maximized and leave
the compositor-owned geometry alone.

`WindowFrameGeometry::GetDecorationOverlayInsets()` reports how much of the
content overlay decoration covers. `UpdateContentInsets()` in the sample offsets
only the text rows by it, so the heading stays clear of the title bar while the
content background keeps the whole frame and extends under the decoration.
Applying the same values to the content root instead would shrink the content by
the decoration area and undo overlay layout.

The full-surface root observes touch through a non-consuming intercept signal.
When overlay decoration has auto-hidden, the first pointer down reveals it and
restarts the hide timer without taking the event away from application content.

The window-control and resize PNGs in `res/` are copied without modification
from the TizenFX `BorderWindowTest` sample resources. The maximize image is
replaced with the restore image while the window is maximized.

Use `WindowFrameInteractiveResizePolicy::KEEP_ASPECT_RATIO` to ask the window system to preserve
the native surface aspect ratio during interactive resize. On Tizen this maps to
the `wm.policy.win.resize_aspect_ratio` auxiliary hint. A backend that does not
support the hint can continue to resize freely. Client shadow outsets are part
of the native surface, so a frame or content ratio can differ slightly from the
preserved surface ratio.

The two resize handle views are the complete resize input surface. Touching
either icon starts server-side `BOTTOM_LEFT` or `BOTTOM_RIGHT` resize; there
are no implicit edge or corner hit regions. The component hides the handles while
resize is fixed or the window is maximized. When explicit resize is configured
as unsupported, the handles remain visible in normal `FREE` or
`KEEP_ASPECT_RATIO` state but are insensitive.

Move and resize are mutually exclusive interactions. A request begins on the
first pointer `DOWN`, remains active after pointer release, and finishes on the
matching native move/resize completion signal. Interrupted input, window hide,
maximize, and detach cancel the tracked interaction. Focus loss cancels it only
after the pointer was released, because a compositor grab can take window focus
while the pointer is still down. Applications can subscribe to
`InteractionChangedSignal()` and inspect the emitted `WindowFrameInteraction` snapshot
with `GetType()` and `GetState()`.
`GeometryChangedSignal()` reports intermediate bounds and
`GeometryChangeCompletedSignal()` reports final compositor completion bounds.

## Keys

| Key | Action |
|---|---|
| `b` | Toggle decoration visibility |
| `c` | Cycle decoration layout through `SetDecorationLayout()`, keeping the installed views |
| `m` | `ToggleMaximize()` |
| `Escape` / `Back` | Quit |

`Window::FocusChangedSignal()` drives the active/inactive label in the footer.

## Wrapper integration

A product wrapper such as `DesktopWindow` should own one
`WindowFrame` handle, translate launch and feature options into
`WindowFrameOptions`, apply mutable presentation and interaction settings to
the created handle before `Attach()`,
and expose `GetContentRoot()` as the only content insertion point. Product
themes can build their own `WindowFrameDecoration` and `WindowFrameStyle`
then call `SetDecoration()`; they do not need to duplicate geometry,
maximize/restore, overlay, or move/resize state machines. Product-specific
commands can use `SetCommandInterceptor()` with a
`WindowFrame::CommandInterceptor::New(object, method)` callback and observe
`CommandProcessedSignal()`, `WindowStateChangedSignal()`,
`WindowStatePresentedSignal()`, and `InteractionChangedSignal()` without taking
ownership of internal views.

The command interceptor and every WindowFrame signal receive the originating
`WindowFrame` as their first argument. A shared policy or observer can therefore
distinguish multiple framed windows without storing one callback per window.

An interceptor returns `HANDLED` to take a command over entirely, whether it
completes it, rejects it, or starts its own operation. WindowFrame performs no
default operation and starts no interaction tracking for it.
`CONTINUE_DEFAULT` delegates to WindowFrame's native adapter.

The sample's `WindowFrameExample` follows that composition model. Its
`OnCommand()` and `OnStatePresented()` virtual methods demonstrate where a
product wrapper can insert behavior before a default command or after the
resulting frame is presented. Subclasses still use only the public component
API; no internal controller or backend type is required.

When overlay mode and maximize are both active, the decoration remains visible
for the delay configured by `WindowFrame::SetOverlayAutoHideDelay()` and then
hides without changing content layout. The delay can be changed at any time.
Any new
window touch reveals it and restarts the timeout. Restoring or disabling
overlay mode cancels auto-hide and restores the configured decoration
visibility. An explicit `SetDecorationVisible(false)` request has higher
priority than touch reveal. Hiding the native window stops the timer; showing
it reveals only an app-requested decoration and starts a fresh timeout.
`ShowOverlayTemporarily()` provides the same reveal behavior for a product
command without synthesizing input.

## Build

### Ubuntu

From the samples build directory:

```sh
cmake -DDALI_UI_SAMPLE_LIST=window-frame ..
cmake --build . --target window-frame.example
```

### GBS (Tizen)

From the dali-ui repository root:

```sh
gbs build -A aarch64 --include-all \
  --packaging-dir packaging

gbs build -A aarch64 --include-all \
  --packaging-dir samples/window-frame/packaging \
  --spec com.samsung.dali.window-frame.spec
```

The resulting package is named
`com.samsung.dali.window-frame-2.0.0-1.<arch>.rpm`. It installs the
application as `com.samsung.dali.window-frame` and runs
`/usr/apps/com.samsung.dali.window-frame/bin/window-frame.example`.

The component package must be built first because the sample no longer
compiles a private copy of WindowFrame. The sample keeps an unversioned
`pkgconfig(dali2-ui-components)` BuildRequires, so the rootstrap must provide a
component package that contains the WindowFrame public API. It must also contain compatible
`dali2-core` and `dali2-adaptor` packages; an older core can prevent the
component package from building before WindowFrame sources are compiled.

## Platform validation

Ubuntu builds validate source integration, geometry, layout, and event
routing. Display-server move and resize requests may not operate on the
Ubuntu backend. Validate interactive move, both bottom-corner icon resize
directions, minimum/maximum constraints, and completion signals on a
supported Tizen device.
