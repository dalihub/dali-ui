[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/WindowFrame-(kr))

# DALi UI Components - WindowFrame

`WindowFrame` adds client-rendered decoration, window controls, move regions, and resize handles to an existing `Dali::Window`. Application content is placed in a dedicated `ContentRoot`. WindowFrame owns decoration layout and size conversion, while the window backend and compositor perform native move, resize, minimize, maximize, and restore operations.

> [Open the visual HTML guide (Korean)](./assets/WindowFrame/window-frame-guide.html)<br>
> The HTML guide presents the sample structure, layout behavior, public API map, and a DesktopWindow-style wrapper in a single visual document.

![WindowFrame layout](./assets/WindowFrame/window-frame-layout.svg)

## 1. Responsibilities

| Owner | Responsibility |
|---|---|
| Application | Creates decoration `View` objects, assigns roles, configures style, and adds content to `ContentRoot` |
| `WindowFrame` | Owns view lifetime, slot layout, content coordinates, role input, state tracking, and signals |
| Window backend / compositor | Performs native move, resize, minimize, maximize, and restore and decides final geometry |

The primary usage rules are:

- Do not subtract decoration thickness or shadow size from application content.
- Add application content only to `GetContentRoot()`.
- Leave normal maximize/restore position and size under compositor ownership.
- Treat the return value from `RequestMinimize()`, `RequestMaximize()`, or `RequestRestore()` as a dispatch result, not presentation completion.
- Decoration views and frame styles can change at runtime without replacing the content root.

## 2. Quick Start

One public header provides the complete API:

```cpp
#include <dali-ui-components/public-api/window/window-frame.h>
```

The following skeleton shows the minimum flow from creation to content insertion.

```cpp
using namespace Dali;
using namespace Dali::Ui;

class MyWindowController : public ConnectionTracker
{
public:
  void Initialize(Application application)
  {
    mApplication = application;
    mWindow      = application.GetWindow();
    mWindow.SetBackgroundColor(Color::TRANSPARENT);
    mWindow.SetTransparency(true);

    WindowFrameOptions options;
    options.SetInitialStatePolicy(WindowFrameInitialStatePolicy::AUTO);
    options.SetInitialRestoreFrameSize(Vector2(400.0f, 260.0f));

    mWindowFrame = WindowFrame::New(
      mWindow,
      WindowFrame::CloseCallback::New(this, &MyWindowController::OnClose),
      options);
    mWindowFrame.SetOverlayEnabled(true);
    mWindowFrame.SetOverlayAutoHideDelay(2500u);

    InstallDecoration();
    ConnectSignals();

    mWindowFrame.RequestFrameResize(Vector2(400.0f, 260.0f));
    mWindowFrame.SetMinimumFrameSize(Vector2(200.0f, 130.0f));
    mWindowFrame.Attach();

    View appContent = CreateAppContent();
    mWindowFrame.GetContentRoot().Add(appContent);
  }

private:
  void OnClose()
  {
    mApplication.Quit();
  }

  // Application-specific helpers implemented by the product.
  void InstallDecoration();
  void ConnectSignals();
  View CreateAppContent();

  Application  mApplication;
  Window       mWindow;
  WindowFrame mWindowFrame;
};
```

Store `Application` and `Window` handles by value. Retaining a pointer or reference to a caller's local handle can outlive that handle. A transparent native background and surface are required when client shadows or rounded outer corners must remain visible.

Recommended initialization order:

1. Configure creation-only launch and feature settings in `WindowFrameOptions`.
2. Call `WindowFrame::New()` with an existing `Dali::Window`.
3. Configure mutable presentation and interaction settings on `WindowFrame`.
4. Install decoration, layout, and frame style.
5. Connect signals and an optional command interceptor.
6. Configure size and constraints, then call `Attach()`.
7. Add application views to `GetContentRoot()`.

`GetContentRoot()` is available before `Attach()`. The same handle survives decoration replacement and detach/attach cycles.

### Handle and attachment lifetime

| API | Contract |
|---|---|
| `WindowFrame()` | Creates an uninitialized handle. Calling instance APIs on it is invalid until it is assigned an initialized handle |
| `New()` | Creates the implementation around an existing `Dali::Window`; the overload without options uses default creation options |
| Copy construction/assignment | Shares the same WindowFrame implementation, signals, frame, and content root |
| `DownCast()` | Returns an initialized WindowFrame only when the `BaseHandle` contains a WindowFrame implementation; otherwise returns an empty handle |
| `Attach()` | Idempotently parents the persistent frame tree, connects window callbacks, and applies stored native size constraints |
| `Detach()` | Idempotently disconnects callbacks, clears native constraints, cancels the scheduled frame callback and any interaction in progress, and unparents the frame tree |
| `IsAttached()` | Reports the frame/callback attachment state, not native window visibility |

`Detach()` does not destroy application content or custom decoration. A later `Attach()` restores the same view tree. The persistent `ContentRoot` remains valid until the last handle sharing that WindowFrame implementation is destroyed.

## 2.4. Two Samples

| Sample | What it shows |
|---|---|
| `samples/window-frame` | Every feature, as a showcase: both frame styles, overlay, resize policies, size constraints |
| `samples/desktop-window` | A product wrapper that owns its decoration and exposes application extension points. Meant to be copied into a product framework |

## 2.5. The Ready-made Decoration

`DefaultWindowDecoration` builds, installs and maintains a complete window decoration, so a working
window takes one extra call:

```cpp
#include <dali-ui-components/public-api/window/default-window-decoration.h>

mWindowFrame  = WindowFrame::New(window, closeCallback);
mDefaultDecoration = DefaultWindowDecoration::New(mWindowFrame);   // <- ready-made decoration
mWindowFrame.Attach();

mWindowFrame.GetContentRoot().Add(myContent);
```

It draws thin edges along the top and both sides and a bar along the bottom.
From left to right, the bottom bar contains the bottom-left resize handle, an
expandable move region, application actions, the built-in minimize,
maximize/restore, and close buttons, and the bottom-right resize handle.

Only the decoration paints. The frame background is transparent, so the area
the application draws into is not tinted, and a window created with
`SetTransparency(true)` stays see-through wherever the application has not
drawn.

The frame also casts a shadow. It is drawn in the window surface outside the
frame, so **the window has to be created transparent**:

```cpp
window.SetBackgroundColor(Color::TRANSPARENT);
window.SetTransparency(true);
```

Without that, the space the shadow needs shows the opaque window background as
a margin around the frame. Turn the shadow off with
`SetShadowEnabled(false)` for a window that cannot be transparent. The shadow
is not drawn while maximized.

The icons ship with the component and install to
`<dataReadOnlyDir>/ui/images/components/border/`. A build that does not use the
default resource directory resolves that path from the
`DALI_UI_COMPONENTS_IMAGE_DIR` environment variable instead, matching every
other DALi asset directory. They are white glyphs with an alpha channel. The
default dark theme leaves them white, while the light theme tints them dark.

**Keep the `mDefaultDecoration` handle as a member.** It owns application action
callbacks and the connection that follows window state, so taking it in a local
or discarding the return value destroys that behavior on the spot.
`WindowFrame` still owns the decoration and built-in window-control wiring, so
the decoration remains drawn and minimize/maximize/close still work. Actions added
with `AddBarAction()` stop, however, and the maximize icon and corner radius no
longer follow state changes.

### Adjusting it

```cpp
DefaultWindowDecorationOptions options;
options.SetTheme(DefaultWindowDecorationTheme::LIGHT);
options.SetBarHeight(40.0f);
options.SetMinimizeControlEnabled(false);
mDefaultDecoration = DefaultWindowDecoration::New(mWindowFrame, options);
```

| Option | Default |
|---|---|
| `SetTheme()` | `DARK` (dark translucent frame with light icons) |
| `SetBarHeight()` | `50` |
| `SetEdgeThickness()` | `5`, applied to the top and both sides |
| `SetCornerRadius()` | `12`, squared off while maximized |
| `SetBackgroundColor()` | Chosen by the theme |
| `SetMinimizeControlEnabled()` / `SetMaximizeRestoreControlEnabled()` / `SetCloseControlEnabled()` | all `true` |
| `SetResizeHandlesEnabled()` | `true` |
| `SetShadowEnabled()` | `true`, requires a transparent window |
| `SetTopAreaHeight()` | `0`, meaning the top stays a thin edge |

### A top area and application actions

Two things can be added without giving up the ready-made decoration.

`SetTopAreaHeight()` turns the thin top edge into a strip the application fills,
reachable through `GetTopArea()`. Unlike `GetMoveRegion()` it is not the move
region, so it does not start a window move **and its children keep working while
the window is maximized**.

```cpp
DefaultWindowDecorationOptions options;
options.SetTopAreaHeight(44.0f);
mDefaultDecoration = DefaultWindowDecoration::New(mWindowFrame, options);
mDefaultDecoration.GetTopArea().Add(myBranding);
```

`AddBarAction()` puts an application button in the bottom bar, in a column of
its own between the move region and the built-in minimize,
maximize/restore, and close buttons. It is beside the move region rather than
inside it so the action remains interactive while maximized.

```cpp
const Dali::String shareIconUrl = Dali::String(RESOURCES_DIR) + "share.png";
mDefaultDecoration.AddBarAction(shareIconUrl, Callback<void()>::New(this, &App::Share));
mWindowFrame.SetMinimumFrameSize(mDefaultDecoration.GetMinimumFrameSize());
```

Each action reserves a column, so `GetMinimumFrameSize()` grows with it; apply
it again afterwards. Actions appear left to right in the order they were added,
and anything already in `GetMoveRegion()` or `GetTopArea()` is kept. Pass the
full application resource URL. Unlike the built-in white glyphs, an application
icon is not tinted by `DefaultWindowDecorationTheme`.

### Its minimum size

The bar gives every control and every corner resize handle a fixed column, so
the decoration has a width below which it cannot lay out: the move region collapses
and the close button ends up underneath the corner handle. Ask for it rather
than guessing:

```cpp
mWindowFrame.SetMinimumFrameSize(mDefaultDecoration.GetMinimumFrameSize());
```

With default options that is `276 x 55`. Turning off controls or the resize
handles removes their columns and lowers it. The decoration does not apply this
itself, because the window's size constraints belong to the application; take
the larger of it and your own minimum when you have one.

`GetMoveRegion()` returns the expandable move region of the bottom bar. Add a
non-interactive title or branding there; the rest of the region still starts a
move.

WindowFrame makes this region insensitive while the window is maximized, since
a maximized window cannot be moved. An insensitive view hides its children from
hit testing too, so a button placed here stops responding as soon as the window
is maximized. Use
`AddBarAction()` for an icon action in the bar, or `GetTopArea()` for custom
interactive content. Build a `WindowFrameDecoration` directly only when those
ready-made locations do not fit the product layout.

```cpp
mDefaultDecoration.GetMoveRegion().Add(Label::New("My Application"));
```

For finer shadow control than on or off, read the installed style with
`WindowFrame::GetFrameStyle()`, adjust it, and apply it with
`SetFrameStyle()`; that changes only the style and leaves the decoration alone.

`GetDecoration()` returns a **copy** of the installed `WindowFrameDecoration`.
The copy holds the same view handles, so reaching an view through it and
changing a property, a child or a colour changes the installed decoration. Assigning
a different view to a slot or a role does not — that only rewrites the copy. To
swap an view, set it on the copy and reinstall the copy with
`SetDecoration()`.

To replace the frame outright, build a `WindowFrameDecoration` and install it
with `SetDecoration()` as described below; release the `DefaultWindowDecoration` handle
first so it stops tracking views that are gone.

The ready-made decoration installs with `WindowFrameSizePolicy::KEEP_WINDOW_SIZE`, so the
window keeps its size and the content shrinks by the decoration. Switching a custom
frame for a `DefaultWindowDecoration` at runtime therefore changes the content size.

## 3. Building Custom Decoration

### 3.1 Slots and roles

`WindowFrameDecoration` separates visible layout **slots** from behavioral **roles**.
`SetMoveRegion()` and the three `Set...Control()` APIs do not add layout areas.
They only assign operation roles to Views already contained in a slot tree. Here,
control means the conventional minimize, maximize/restore, or close button in a
window title bar.

| Kind | API | Purpose |
|---|---|---|
| Slot | `SetTopSlot()`, `SetBottomSlot()`, `SetLeftSlot()`, `SetRightSlot()` | Root views for the four frame edges |
| Slot and role | `SetBottomLeftResizeHandle()`, `SetBottomRightResizeHandle()` | Explicit corner resize handles |
| Role | `SetMoveRegion()` | Starts native move from mouse or touch drag |
| Role | `SetMinimizeControl()` | Requests minimize |
| Role | `SetMaximizeRestoreControl()` | Toggles maximize and restore |
| Role | `SetCloseControl()` | Invokes the close callback |

A role view must be contained by one of the supplied slots. Slot roots must be unparented when installed, or already installed in this WindowFrame so that the frame can be resubmitted with new layout or style. Duplicate slots, duplicate roles, and roles outside the decoration tree reject the complete installation and preserve the current frame.

```cpp
WindowFrameDecoration decoration;
decoration.SetTopSlot(top);
decoration.SetBottomSlot(bottom);
decoration.SetLeftSlot(left);
decoration.SetRightSlot(right);

decoration.SetMoveRegion(moveRegion);             // Child of bottom
decoration.SetMinimizeControl(minimizeButton);    // Child of bottom
decoration.SetMaximizeRestoreControl(maximizeButton);    // Child of bottom
decoration.SetCloseControl(closeButton);          // Child of bottom
decoration.SetBottomLeftResizeHandle(leftHandle);
decoration.SetBottomRightResizeHandle(rightHandle);
```

WindowFrame does not create unused eight-direction invisible hit regions. An explicit resize begins only when the application-provided bottom-left or bottom-right handle is pressed and dragged.

### 3.2 `SetDecorationInsets()`

`WindowFrameLayout::SetDecorationInsets()` reserves decoration space **inside** the visible frame.

```cpp
WindowFrameLayout layout;
layout.SetDecorationInsets(Insets(8.0f, 8.0f, 64.0f, 56.0f));
//                              start  end   top    bottom
```

This configuration means:

- Left border: 8 px
- Right border: 8 px
- Top border: 64 px
- Bottom border: 56 px
- Normal inset content: the center area after removing those insets

The layout drive both slot layout and `ContentRoot` size. Subtracting the same values in application code shrinks content twice. Place content from `(0, 0)` inside the root instead.

### 3.3 Resize handle size

Configure each handle's layout and input area separately from its visual image.

```cpp
layout.SetBottomLeftResizeHandleSize(Vector2(36.0f, 36.0f));
layout.SetBottomRightResizeHandleSize(Vector2(36.0f, 36.0f));
```

A handle view should provide a comfortable touch target even when the icon is smaller. Native interaction also depends on the `RESIZE` feature and `WindowFrameInteractiveResizePolicy`.

### 3.4 Value semantics and defaults

`WindowFrameDecoration` starts with every slot and role empty. Passing an empty `View` to a setter clears that entry in the value being prepared. Its getters are useful for a product wrapper that builds a base decoration and then inspects or replaces selected roles before installation.

Copying `WindowFrameDecoration` copies its stored View handles; it does not clone the view tree. Copies therefore refer to the same views and cannot be installed simultaneously as independent frames. Build a new unparented tree when a second window or replacement frame needs independent views.

`WindowFrameLayout` is a deeply copied value object with these defaults:

| Metric | Default |
|---|---|
| Decoration insets (`start`, `end`, `top`, `bottom`) | `8, 8, 56, 30` |
| Bottom-left resize handle | `28 x 28` |
| Bottom-right resize handle | `28 x 28` |

Negative metric components are treated as zero during layout. Insets are clamped to available frame size, and if both resize handles are wider than the frame their widths are reduced proportionally. Explicitly set every product-visible metric instead of relying on defaults when layout must be stable across themes.

## 4. Changing the Frame

Five entry points change the frame. Pick the narrowest one that does what you
need: the narrower calls keep the installed views alive, so anything the
application attached to them survives.

| Call | Replaces | Keeps | Returns |
|---|---|---|---|
| `SetDecoration(decoration, layout, sizePolicy)` | Decoration views, layout | Frame style | `WindowFrameDecorationResult` |
| `SetDecoration(decoration, layout, style, sizePolicy)` | Decoration views, layout, style | — | `WindowFrameDecorationResult` |
| `SetDecorationLayout(layout, sizePolicy)` | Layout | **Decoration views**, style | `void` |
| `SetFrameStyle(style, sizePolicy)` | Style | **Decoration views**, layout | `void` |
| `ClearDecoration()` | Decoration views and layout with empty ones | Frame style | `void` |

What that means in practice:

| You want to | Use |
|---|---|
| Install the first frame, or swap the whole look | `SetDecoration()` with a style |
| Swap the views but keep the current colours and shadow | `SetDecoration()` without a style |
| Change a bar height, an inset, or a handle size | `SetDecorationLayout()` |
| Change colour, corner radius, or shadow | `SetFrameStyle()` |
| Give the content the whole frame with no decoration | `ClearDecoration()` |

Only the `SetDecoration()` forms validate slot and role topology, because only
they take views, and only they return a result worth checking. The two narrow
setters cannot fail.

`ClearDecoration()` always keeps the window size, so the content root
grows into the space the decoration occupied. Every other call takes a
`WindowFrameSizePolicy`.

### `SetDecoration()`

`SetDecoration()` validates and installs decoration, layout, and frame style as one transaction.

```cpp
WindowFrameStyle style;
style.SetFrameBackgroundColor(UiColor(0x17212B));
style.SetFrameCornerRadius(Vector4(14.0f, 14.0f, 14.0f, 14.0f));

WindowFrameDecorationResult result = mWindowFrame.SetDecoration(
  decoration,
  layout,
  style,
  WindowFrameSizePolicy::KEEP_CONTENT_SIZE);

if(result != WindowFrameDecorationResult::INSTALLED)
{
  // Apply a fallback frame based on the typed result.
}
```

Atomic installation prevents partial updates such as applying a style before later decoration validation fails.

| Result | Meaning |
|---|---|
| `INSTALLED` | The complete frame was installed |
| `SLOT_ALREADY_PARENTED` | A slot root belongs to a parent other than this WindowFrame |
| `DUPLICATE_SLOT` | One view was assigned to multiple slots |
| `DUPLICATE_ROLE` | One view was assigned duplicate roles |
| `ROLE_OUTSIDE_DECORATION` | A role view is outside the supplied decoration tree |

Use the `SetDecoration()` overload without style to replace decoration and layout while preserving the current style. Use `SetFrameStyle()` to replace only style. `ClearDecoration()` removes custom decoration, resets decoration layout to zero, and preserves the content root and current frame style.

### Changing layout without replacing decoration

`SetDecoration()` replaces slot views. To change only decoration insets or
resize-handle sizes, use `SetDecorationLayout()`. The installed slot views, their
contents, and any signal connections the application made to them stay alive.

```cpp
WindowFrameLayout compact;
compact.SetDecorationInsets(Insets(6.0f, 6.0f, 44.0f, 44.0f));
compact.SetBottomLeftResizeHandleSize(Vector2(36.0f, 36.0f));
compact.SetBottomRightResizeHandleSize(Vector2(36.0f, 36.0f));
mWindowFrame.SetDecorationLayout(compact, WindowFrameSizePolicy::KEEP_WINDOW_SIZE);
```

`SetDecoration()` also accepts the decoration that is currently installed, so
resubmitting it with a new style or new layout is valid. A slot owned by any
other parent is still rejected with `SLOT_ALREADY_PARENTED`.

### Size preservation policy

| Policy | Stable value during decoration or shadow changes | Typical use |
|---|---|---|
| `KEEP_CONTENT_SIZE` | Application content size | A theme change must not disturb content layout |
| `KEEP_WINDOW_SIZE` | Window size | The externally visible window size must remain fixed |

`KEEP_WINDOW_SIZE` is usually appropriate for initial installation into an already-sized native window. `KEEP_CONTENT_SIZE` is often more useful for runtime theme changes.

The policy is evaluated only by the call that receives it; it is not stored as
a setting for later changes. For example:

```cpp
mWindowFrame.SetDecorationLayout(
  layout,
  WindowFrameSizePolicy::KEEP_CONTENT_SIZE);
mWindowFrame.SetFrameStyle(
  style,
  WindowFrameSizePolicy::KEEP_WINDOW_SIZE);
```

The first call preserves the content size by resizing the normal window when
necessary. The second call independently preserves the window surface size at
that moment, so changed shadow outsets can change the resulting frame and
content sizes.

## 5. Frame Style and Shadow

`WindowFrameStyle` is the requested visual configuration for the frame background, corner radius, and client-rendered shadow. WindowFrame suppresses the configured shadow and corner radius while maximized before applying the actual frame views.

### Defaults

| Property | Default |
|---|---|
| Shadow source | `NONE` |
| Shadow outsets | Zero on every side |
| Frame background | Transparent |
| Frame corner radius and policy | Zero, `ABSOLUTE` |
| Shadow image and stretch border | Empty URL and zero border |
| Shadow image border-only | `false` |

`SetShadow()` selects a color shadow generated from `Dali::Ui::Shadow` color,
blur, offset, extents, and cutout properties. `SetShadowImageUrl()` selects an
image shadow when the URL is not empty. `ClearShadow()` or an empty image URL
selects `NONE`. No separate ownership or renderer selection is required.

### Color shadow

```cpp
WindowFrameStyle style;
style.SetShadowOutsets(Insets(14.0f, 14.0f, 14.0f, 18.0f));
style.SetShadow(Shadow(
  14.0f,
  Vector2(0.0f, 3.0f),
  UiColor(0x000000, 0.48f),
  Vector2::ZERO,
  CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS));
style.SetFrameBackgroundColor(UiColor(0x17212B));
style.SetFrameCornerRadius(Vector4(14.0f, 14.0f, 14.0f, 14.0f));
style.SetFrameCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
```

`SetShadow()` selects `WindowFrameShadowSource::COLOR` and applies the configured `Shadow` to an internal view. The frame corner radius is also applied to that view. `SetFrameBackgroundColor()` colors the complete frame background; it does not replace the colors of the top, bottom, left, or right decoration slots supplied by the application.

In normal state, frame corner radius is applied to the frame root, background, and View shadow. Maximized state uses zero radius so the frame reaches the screen edges. `SetFrameCornerRadiusPolicy()` controls how the radius values are interpreted.

### Shadow outsets

Shadow outsets reserve native surface space outside the frame for an effect. They are geometry, not blur parameters, and should be large enough to prevent a View shadow or N-patch image from being clipped. They are excluded from `GetFrameSize()` but included in native surface size while a client shadow is active in normal state.

```text
surface width  = start outset + frame width  + end outset
surface height = top outset   + frame height + bottom outset
```

Negative outset components are resolved as zero. Maximized state removes all client shadow outsets; restoring to normal reapplies the configured values.

Color and image shadows are visible only in the normal state. Maximized state removes the client effect, outsets, and corner radius, and restoring to normal reapplies the configured values. WindowFrame does not request, disable, or report a shadow owned by the window system.

### Image shadow

Existing NUI shadow images, including N-patch assets, can be reused. Configure an outer frame effect once in frame style instead of applying separate shadows to top, bottom, and side views.

```cpp
WindowFrameStyle style;
style.SetShadowOutsets(Insets(20.0f, 20.0f, 20.0f, 24.0f));
style.SetShadowImageBorder(Insets(start, end, top, bottom));
style.SetShadowImageBorderOnly(true);
style.SetShadowImageUrl("/opt/usr/apps/.../res/window-shadow.9.png");
```

`SetShadowImageUrl()` selects `WindowFrameShadowSource::IMAGE`. An empty URL
disables the client-rendered shadow. `SetShadowImageBorder()` accepts public
layout order `Insets(start, end, top, bottom)` and enables N-patch stretching
through the internal `ImageView`. WindowFrame converts it at the N-patch
boundary to the renderer order `(start, top, end, bottom)`.
`SetShadowImageBorderOnly(true)` omits the image center and renders only the
border regions; leave it `false` for an ordinary image.

### Applying and changing style

Pass style to `SetDecoration()` when decoration, layout, and style form one theme that must be validated and installed atomically. Use `SetFrameStyle()` when only visual style changes.

```cpp
mWindowFrame.SetFrameStyle(
  style,
  WindowFrameSizePolicy::KEEP_CONTENT_SIZE);
```

Use `KEEP_CONTENT_SIZE` for a runtime theme change that must preserve application layout, or `KEEP_WINDOW_SIZE` when the externally visible window size must remain fixed.

`WindowFrameStyle` is a deeply copied value object backed by a private implementation. Changing a copy does not mutate a style already installed on WindowFrame. Do not access an object after moving from it.

## 6. Content and Size Coordinates

### Application content

```cpp
AbsoluteLayout content = AbsoluteLayout::New();
content.SetLayoutParams(AbsoluteLayoutParams::New()
  .SetBounds(LayoutRect(0.0f, 0.0f, 1.0f, 1.0f))
  .SetFlags(AbsoluteLayoutFlags::SIZE_PROPORTIONAL));

mWindowFrame.GetContentRoot().Add(content);
```

Application coordinates are local to `ContentRoot`. A `500 x 500` native surface can produce a smaller content root after decoration and shadow are applied. WindowFrame performs this conversion; the application does not subtract insets from 500 itself.

### Size APIs

| API | Coordinate contract |
|---|---|
| `RequestFrameResize()`, `GetFrameSize()` | Complete frame excluding shadow outsets |
| `RequestContentResize()`, `GetContentSize()` | Application content root excluding decoration |
| `SetMinimumFrameSize()`, `SetMaximumFrameSize()` | Frame constraints |

`RequestContentResize()` converts content size through current decoration and shadow layout before requesting native resize. While attached, native resize is deferred to the next event-loop tick to avoid re-entering event processing. Repeated requests are coalesced.

```cpp
mWindowFrame.RequestFrameResize(Vector2(400.0f, 260.0f));
mWindowFrame.SetMinimumFrameSize(Vector2(200.0f, 130.0f));
mWindowFrame.SetMaximumFrameSize(Vector2(1400.0f, 900.0f));
```

If a new minimum exceeds the current maximum, or constraints otherwise conflict, the setter returns `false` and preserves the previous values. A successful update returns `true`.

Constraint and size details:

- Negative minimum, maximum, frame, or content size components are treated as zero.
- Constraints are stored on WindowFrame before attachment, applied to the native window by `Attach()`, cleared natively by `Detach()`, and reapplied by the next `Attach()`.
- `GetMinimumFrameSize(out)` and `GetMaximumFrameSize(out)` return `false` when absent and do not provide a configured value; use the boolean result before reading `out`.
- `ClearMinimumFrameSize()` and `ClearMaximumFrameSize()` remove only the selected constraint.
- `RequestFrameResize()` and `RequestContentResize()` do not override maximized geometry; they return `false` there. Restore first when a programmatic normal size must be applied.
- The size getters report the current laid-out geometry, which can differ from a request after constraint clamping or compositor processing.

### Geometry snapshots

`GetGeometry()` and both geometry signals return a `WindowFrameGeometry` snapshot with three coordinate contracts.

| Getter | Coordinates | Includes |
|---|---|---|
| `GetSurfaceBounds()` | Screen coordinates | Native surface position and size, including client-shadow space when active |
| `GetFrameBounds()` | Surface-local coordinates | Visible frame excluding client-shadow outsets |
| `GetContentBounds()` | Surface-local coordinates | Layout-managed application content after decoration/overlay resolution |

`GetContentBounds()` is useful for observation and diagnostics. Application children still use coordinates local to `GetContentRoot()`, where their origin is `(0, 0)`; do not apply the surface-local content offset to those children.

`GeometryChangedSignal()` emits snapshots for distinct intermediate geometry changes. `GeometryChangeCompletedSignal()` emits the final snapshot reported by native move or resize completion and can occur after pointer release.

## 7. Initial Full-Size Launch

If an application does not specify window geometry before launch, the target can create it at full size. `WindowFrameInitialStatePolicy::AUTO` compares native bounds with target maximized bounds and interprets this case as initially maximized.

| Initial policy | Behavior |
|---|---|
| `AUTO` | Uses native maximized state first, otherwise compares current surface size with valid target maximized bounds |
| `USE_CURRENT` | Uses the state already reported by `Dali::Window` without full-size inference |
| `REQUEST_MAXIMIZE` | Uses an existing maximized state or dispatches an ordinary maximize request when the feature is enabled |

```cpp
WindowFrameOptions options;
options.SetInitialStatePolicy(WindowFrameInitialStatePolicy::AUTO);
options.SetInitialRestoreFrameSize(Vector2(400.0f, 260.0f));
```

`SetInitialRestoreFrameSize()` is retained **only when `AUTO` detects an initial
full-size launch where no previous normal size exists**. The launch itself uses
ordinary maximize. The configured size is supplied through
`MaximizeWithRestoreSize(false, size)` on the first `RequestRestore()` and is
then consumed. Later maximize/restore cycles, and every cycle after a normal-size
launch, leave restore position and size to the compositor.

`WindowFrameOptions` defaults to `AUTO`, has no restore frame size, and enables every `WindowFrameFeature`. `GetInitialRestoreFrameSize(out)` returns `false` when no size is configured and leaves `out` unchanged. `ClearInitialRestoreFrameSize()` removes the value. Options are copied by `New()`, so modifying the source options afterward does not change an existing WindowFrame. Runtime presentation and interaction settings belong on the created WindowFrame handle.

Applications observe only the
compositor-confirmed final state through `WindowStateChangedSignal()` or
`GetWindowState()`.

## 8. Move and Resize

Applications do not calculate native window position from pointer coordinates. A pointer down on `SetMoveRegion()` or either resize handle causes WindowFrame to request a native interactive operation. The compositor then decides final position and size.

```cpp
mWindowFrame.InteractionChangedSignal().Connect(
  this,
  [](WindowFrame sender,
     const WindowFrameInteraction& interaction)
  {
    static_cast<void>(sender);
    const WindowFrameInteractionType type = interaction.GetType();
    const WindowFrameInteractionState state = interaction.GetState();
    // Observe STARTED, POINTER_RELEASED, COMPLETED, or CANCELLED.
  });
```

Resize policy selects how explicit handle interaction behaves:

| Policy | Behavior |
|---|---|
| `FREE` | Allows compositor-controlled interactive resize without requesting a fixed ratio |
| `KEEP_ASPECT_RATIO` | Keeps resize handles active and asks the window system to preserve the native surface aspect ratio |
| `DISABLED` | Cancels an active resize and blocks further interactive resize requests |

```cpp
mWindowFrame.SetInteractiveResizePolicy(
  WindowFrameInteractiveResizePolicy::KEEP_ASPECT_RATIO);
```

On Tizen, `KEEP_ASPECT_RATIO` maps to the `wm.policy.win.resize_aspect_ratio` auxiliary hint. WindowFrame captures a pre-existing hint value when it applies ratio preservation and restores that value when the policy changes to `FREE` or `DISABLED`, or when detached. The default `FREE` policy does not modify an application-owned hint. If the backend rejects the hint, interactive resize remains available and can resize freely.

The compositor preserves the **native surface** ratio. Client shadow outsets occupy part of that surface, so the visible frame or content ratio can differ slightly. Use zero client-shadow outsets or a system shadow when an exact visible-frame ratio is required.

Interaction states describe input/native lifecycle rather than geometry values:

| State | Meaning |
|---|---|
| `STARTED` | A move or resize request was dispatched and interaction tracking started |
| `POINTER_RELEASED` | The initiating pointer ended, but the native operation has not completed |
| `COMPLETED` | The corresponding native move/resize completion event arrived |
| `CANCELLED` | Tracking was interrupted by input interruption, window hide, maximize, detach, a policy change, or focus loss after the pointer was released |

`WindowFrameInteractionType` distinguishes move, bottom-left resize, and bottom-right resize.

The default resize policy is `FREE`. Both `FREE` and `KEEP_ASPECT_RATIO` keep resize handles visible and sensitive in normal layout. `DISABLED` hides them. None of these policies prohibit programmatic `RequestFrameResize()` or `RequestContentResize()` calls.

## 9. Minimize, Maximize, Restore, and Close

Assigning control roles connects their default operations. The same operations can be requested directly from product code.

```cpp
WindowFrameCommandResult result = mWindowFrame.ToggleMaximize();

if(result == WindowFrameCommandResult::DISPATCHED)
{
  // A native request was dispatched. Completion is not confirmed yet.
}
```

`ToggleMaximize()` picks the opposite of the current maximize state. Prefer it
over branching on `IsMaximized()`: it also covers states that cannot be
toggled, such as a minimized window.

| Result | Meaning |
|---|---|
| `DISPATCHED` | The default window operation or close callback was invoked. Neither window-system acceptance nor completion is implied |
| `NOT_DISPATCHED` | The default operation was unavailable or disabled, so nothing was invoked |
| `HANDLED` | The interceptor took the command over |

For window-system commands, `DISPATCHED` cannot promise more than it says. `Window::Maximize()`, `RequestMoveToServer()` and the rest return no result, so there is nothing to report acceptance from. It guarantees only that the interceptor did not take the command and the underlying operation was invoked.

WindowFrame holds no in-flight state of its own: the client never reports a state the window system has not reported, and applications observe the result through signals.

Which signal depends on the command, so `DISPATCHED` does not point at one place:

| Command | Where the outcome appears |
|---|---|
| `MAXIMIZE`, `RESTORE` | `WindowStateChangedSignal()`, then `WindowStatePresentedSignal()` once drawn |
| `MINIMIZE` | `WindowStateChangedSignal()` only. A minimized window presents no frame |
| `MOVE`, `RESIZE` | `InteractionChangedSignal()` reaching `COMPLETED`, and `GeometryChangedSignal()` with `GeometryChangeCompletedSignal()` |
| `CLOSE` | No signal. The `CloseCallback` given to `New()` owns what closing means |

`CommandProcessedSignal()` carries this result for every command, so it reports what was dispatched rather than what completed.

### An enabled explicit request is always dispatched

`RequestMinimize()`, `RequestMaximize()` and `RequestRestore()` reach the window system even when the window already appears to be in the requested state. Suppressing them would mean comparing against the last state the window system reported, which drops a request sent before the previous one was reported:

```cpp
mWindowFrame.RequestMaximize();  // DISPATCHED
mWindowFrame.RequestRestore();   // DISPATCHED - the window ends up restored
```

Repeating a request the window system has already applied is harmless, so nothing is lost by not filtering.

`ToggleMaximize()` is the exception, because a toggle has no target of its own and has to read the last reported state. Two toggles dispatched before that state changes ask for the same thing twice.


## 10. Completion Timing and Signals

Use `WindowStatePresentedSignal()` when processing must happen after a requested maximize or restore has reached a presented frame, similar to the timing expected from NUI `OnMaximize`.

```cpp
mWindowFrame.WindowStateChangedSignal().Connect(
  this,
  [](WindowFrame sender, WindowFrame::WindowState state)
  {
    static_cast<void>(sender);
    // The compositor confirmed the native state.
  });

mWindowFrame.WindowStatePresentedSignal().Connect(
  this,
  [](WindowFrame sender, WindowFrame::WindowState state)
  {
    static_cast<void>(sender);
    // A frame showing the new maximize or restore state has been presented.
  });
```

| Signal | Timing and purpose |
|---|---|
| `WindowStateChangedSignal()` | The observed window state changed, whatever caused it |
| `WindowStatePresentedSignal()` | A frame showing a newly observed `NORMAL` or `MAXIMIZED` state has been drawn |
| `GeometryChangedSignal()` | Surface, frame, or content changes, including intermediate resize |
| `GeometryChangeCompletedSignal()` | Compositor move or resize completion |
| `DecorationVisibilityChangedSignal()` | Effective decoration visibility change |
| `InteractionChangedSignal()` | Move and resize input lifecycle |
| `CommandProcessedSignal()` | Result after every command dispatch |

Every WindowFrame signal passes the emitting `WindowFrame` as its first
argument. A shared observer can therefore identify which frame produced an
event.

Use `Window::FocusChangedSignal()` when decoration needs an active or inactive
appearance. WindowFrame does not duplicate the underlying window signal.

`IsMaximized()` and `IsMinimized()` return the state the window system reported.
Do not treat their value immediately after a request as completion; wait for the
state signal.

`GetWindowState()` returns `NORMAL`, `MINIMIZED`, or `MAXIMIZED`. The convenience predicates are equivalent state queries.

`WindowStatePresentedSignal()` follows `WindowStateChangedSignal()` and carries the same state, so it fires for a compositor-driven change as well as for one the application requested. Two states never present: `MINIMIZED`, because a minimized window draws no frame, and a state that a newer state supersedes while its frame is still awaited.

WindowFrame registers the underlying frame callback only while
`WindowStatePresentedSignal()` has an observer. No separate feature switch is
required.

## 11. Overlay Mode

In maximized state, overlay mode keeps content bounds at the full frame and draws decoration above content. Decoration hides after the configured delay. The next touch on the window surface reveals it and restarts the timer.

```cpp
mWindowFrame.SetOverlayEnabled(true);
mWindowFrame.SetOverlayAutoHideDelay(2500u);

// Product code can reveal it without synthetic input.
mWindowFrame.ShowOverlayTemporarily();
```

`SetDecorationVisible(true)` records the application's visibility request. While overlay auto-hide is active, effective visibility can still be `false`. Read `IsDecorationVisible()` or `DecorationVisibilityChangedSignal()` for effective visibility, and use `IsOverlayAutoHidden()` only when the auto-hide reason matters.

Overlay layout is active only while maximized. Normal windows use decoration inset layout.

The overlay APIs expose policy, visibility, and auto-hide state separately.

| API | Meaning |
|---|---|
| `IsOverlayEnabled()` | Requested overlay policy; can be true in normal state |
| `IsOverlayAutoHidden()` | Active overlay decoration was suppressed by the auto-hide timeout |
| `IsDecorationVisible()` | Effective visibility after requested visibility and auto-hide are resolved |

Defaults are overlay disabled, decoration requested visible, and a `3000 ms` auto-hide delay. `SetOverlayAutoHideDelay(0)` disables auto-hide, keeping overlay decoration visible indefinitely and revealing decoration that auto-hide had already hidden; a non-zero delay starts a fresh timeout. `ShowOverlayTemporarily()` has an effect only for an active, window-visible overlay whose decoration is requested visible.

## 12. Feature Gates

Every `WindowFrameFeature` is `true` by default regardless of backend type, and only the caller changes that. **These are gates a product declares, not capabilities WindowFrame detects.** Nothing here asks the window system what it supports, so an enabled feature means the request is dispatched, not that the window system acts on it — a backend that has not implemented a request still leaves the UI sensitive and still returns `DISPATCHED`. Turn a feature off before `WindowFrame::New()` for anything a product does not want to offer.

```cpp
WindowFrameOptions options;
options.SetFeatureEnabled(WindowFrameFeature::MINIMIZE, false);

WindowFrame windowFrame = WindowFrame::New(window, closeCallback, options);

if(windowFrame.IsFeatureEnabled(WindowFrameFeature::RESIZE))
{
  // Enable product resize UI or behavior.
}
```

A feature gate describes what the product configured, not whether the latest request succeeded. Each disabled feature has a different effect.

| Feature | Behavior when `false` |
|---|---|
| `MOVE` | Disables move-region input; move command returns `NOT_DISPATCHED` |
| `RESIZE` | Handles can remain visible but insensitive; resize command returns `NOT_DISPATCHED` |
| `MINIMIZE` | Disables minimize control; `RequestMinimize()` returns `NOT_DISPATCHED` |
| `MAXIMIZE_RESTORE` | Disables maximize control; maximize/restore requests return `NOT_DISPATCHED` |

Even when a feature is configured `true`, a backend or compositor that does not process the native request must be diagnosed in the native integration layer.

## 13. Intercepting Default Commands

`SetCommandInterceptor()` provides the policy point used by a DesktopWindow-style wrapper before WindowFrame performs its default operation. Its first callback argument is the originating `WindowFrame`, matching the sender-first signal convention.

| Disposition | Contract |
|---|---|
| `CONTINUE_DEFAULT` | Continue the default WindowFrame native or callback operation |
| `HANDLED` | The application completed or rejected the command synchronously; stop default handling |

```cpp
WindowFrameCommandDisposition OnCommand(
  WindowFrame sender,
  const WindowFrameCommandRequest& request)
{
  if(request.GetCommand() == WindowFrameCommand::MAXIMIZE)
  {
    HideTransientPopup();
  }

  if(request.GetCommand() == WindowFrameCommand::CLOSE && HasUnsavedWork())
  {
    ShowCloseConfirmation();
    return WindowFrameCommandDisposition::HANDLED;
  }

  return WindowFrameCommandDisposition::CONTINUE_DEFAULT;
}
```

Do not call the same `WindowFrame::Request...()` API from its interceptor. That call can re-enter the same interceptor. Return `CONTINUE_DEFAULT` when the WindowFrame default request should proceed.

### `GetResizeDirection(direction)`

All commands share `WindowFrameCommandRequest`, but only resize carries a direction payload. A single boolean getter exposes that optional metadata without a separate precondition.

```cpp
WindowFrameCommandDisposition OnCommand(
  WindowFrame sender,
  const WindowFrameCommandRequest& request)
{
  WindowResizeDirection direction;
  if(request.GetResizeDirection(direction))
  {
    AuditResizeDirection(direction);
  }

  return WindowFrameCommandDisposition::CONTINUE_DEFAULT;
}
```

`GetResizeDirection(out)` returns `false` and leaves `out` unchanged when the request has no direction. It reports metadata only, not resize support, native acceptance, or completion.

Command construction is internal to WindowFrame. Applications inspect the `WindowFrameCommandRequest` passed to an interceptor or `CommandProcessedSignal()` instead of manufacturing one. Use `std::optional<WindowFrameCommandRequest>` when an observer needs an initially empty value. The type is public so a product wrapper can keep policy code strongly typed without depending on WindowFrame internals.

`CommandProcessedSignal()` is emitted after every command dispatch, whether the command came from a configured role View or a direct `Request...()` call. Its result describes dispatch only. For asynchronous native commands, pair it with the state, geometry, or interaction signals that describe actual completion.

`HANDLED` covers everything the interceptor does itself, whether it completes the command, rejects it, or starts its own operation. WindowFrame performs no default operation for it and tracks nothing further.

## 14. Building a DesktopWindow Wrapper

A product or third-party component should own WindowFrame by composition and expose selected virtual hooks. It does not need to subclass WindowFrame's internal implementation.

```cpp
class DesktopWindow : public ConnectionTracker
{
public:
  virtual ~DesktopWindow() = default;

  void Initialize(Window window)
  {
    WindowFrameOptions options = CreateOptions();
    mWindowFrame = WindowFrame::New(
      window,
      WindowFrame::CloseCallback::New(this, &DesktopWindow::OnCloseRequested),
      options);
    ConfigureWindowFrame(mWindowFrame);

    mWindowFrame.SetCommandInterceptor(
      WindowFrame::CommandInterceptor::New(this, &DesktopWindow::DispatchCommand));

    InstallFrame(WindowFrameSizePolicy::KEEP_WINDOW_SIZE);
    ConnectSignals();
    mWindowFrame.Attach();
    OnInitialized(mWindowFrame.GetContentRoot());
  }

protected:
  virtual WindowFrameOptions CreateOptions() const;
  virtual void ConfigureWindowFrame(WindowFrame& windowFrame) {}
  virtual WindowFrameDecoration CreateDecoration(WindowFrameLayout& layout);
  virtual WindowFrameStyle CreateFrameStyle() const;
  virtual WindowFrameCommandDisposition OnCommand(
    WindowFrame sender,
    const WindowFrameCommandRequest& request)
  {
    return WindowFrameCommandDisposition::CONTINUE_DEFAULT;
  }
  virtual void OnCloseRequested() = 0;
  virtual void OnStatePresented(WindowFrame::WindowState) {}
  virtual void OnInitialized(View contentRoot) = 0;

private:
  void InstallFrame(WindowFrameSizePolicy sizePolicy);
  void ConnectSignals();

  WindowFrameCommandDisposition DispatchCommand(
    WindowFrame sender,
    const WindowFrameCommandRequest& request)
  {
    return OnCommand(sender, request);
  }

  WindowFrame mWindowFrame;
};
```

This is a structural skeleton showing the public/protected boundary and hook placement. Implement `InstallFrame()` and `ConnectSignals()` in the shared product layer. Derived products implement only factories and typed hooks.

Typical extension points:

- `CreateOptions()`: Product operation gates and launch-state policy
- `ConfigureWindowFrame()`: Overlay, decoration visibility, auto-hide delay, and resize policy
- `CreateDecoration()`: Title bar, footer, controls, and resize handle UI
- `CreateFrameStyle()`: Shadow, N-patch, corner radius, and frame color
- `OnCommand()`: Close confirmation, logging, product policy, or replacement native operation
- `OnStatePresented()`: Work performed after the new state is visible
- `OnInitialized()`: Application content construction

The wrapper owns shared initialization order and invariants. Derived classes override narrow hooks without duplicating view topology, native geometry, overlay, or state machines.

## 15. Public API Map

| Type or API group | Why applications use it |
|---|---|
| `WindowFrameOptions` | Configures creation-only launch state and operation gates |
| Mutable `WindowFrame` settings | Configure overlay, visibility, auto-hide delay, and resize policy before or after `Attach()` |
| `WindowFrameDecoration` | Connects decoration slots and move/control/resize roles |
| `WindowFrameLayout` | Defines decoration insets and two resize-handle sizes |
| `WindowFrameStyle` | Configures frame color, radius, and client-rendered shadow |
| `SetDecoration()` | Atomically validates and replaces decoration, layout, and style |
| `GetContentRoot()` | Returns the application content root isolated from decoration calculations |
| Frame/content size APIs | Select the coordinate contract that matches product requirements |
| Request APIs | Start window operations from code paths outside role controls |
| State and geometry signals | Separate dispatch, compositor confirmation, completion, and presentation |
| `SetDecorationLayout()` | Changes decoration insets and handle sizes while keeping the installed views |
| `ToggleMaximize()` | Requests the opposite maximize state, and reports NOT_DISPATCHED when the state cannot be toggled |
| `WindowFrameGeometry::GetDecorationOverlayInsets()` | Reads how much content overlay decoration covers |
| `DefaultWindowDecoration` | Installs and maintains a ready-made window decoration |
| `DefaultWindowDecorationOptions` | Adjusts the ready-made decoration's size, theme and controls |
| `SetCommandInterceptor()` | Inserts product policy before default operations |
| `WindowFrameFeature` | Configures move, resize, minimize, and maximize/restore gates before creation |

### Public value types and ownership

| Type | Default and copy contract | Practical use |
|---|---|---|
| `WindowFrame` | A default-constructed handle is empty. A copied initialized handle shares the same implementation, content root, frame, and signals. | Pass by value when shared handle semantics are intended. Call instance APIs only on a handle returned by `New()` or a successful `DownCast()`. |
| `WindowFrameOptions` | Value type; defaults to `AUTO`, no restore size, and every feature enabled. `New()` copies it. | Prepare creation-only policy, then discard or reuse the source options independently. |
| `WindowFrameDecoration` | Value type whose fields default to empty View handles. Copying copies View handles, not view trees. | Describe one frame installation. Build a new unparented View tree when two windows or two simultaneous installations need independent decoration. |
| `WindowFrameLayout` | Value type; defaults to insets `(8, 8, 56, 30)` and two `28 x 28` handles. | Keep layout numbers separate from role Views and visual style. |
| `WindowFrameStyle` | Value type; defaults to `NONE` shadow source, zero outsets/radius, an empty shadow image, transparent frame, `ABSOLUTE` radius policy, and shadow-image border-only disabled. | Copy a theme preset, modify the copy, and apply it explicitly with `SetFrameStyle()` or `SetDecoration()`. |
| `WindowFrameGeometry` | Read-only snapshot value; a default object contains zero bounds. Signal arguments and `GetGeometry()` are snapshots, not live references. | Retain or compare a geometry observation without depending on later layout changes. |
| `WindowFrameCommandRequest` | Read-only command metadata value. | Inspect strongly typed command metadata in interceptors and command-processed observers. |
| `WindowFrameInteraction` | Read-only interaction snapshot; an empty default object is `NONE` and `CANCELLED`. | Read move/resize type and lifecycle state from `InteractionChangedSignal()`. |

All Pimpl-backed value types support copying and moving. A moved-from value is valid only for destruction or reassignment; do not call its getters or setters. This keeps public headers ABI-stable without changing their ordinary value semantics.

### Mutable setting pairs

| Setter or action | Read API and distinction |
|---|---|
| `SetDecorationVisible(bool)` | `IsDecorationVisible()` returns **effective** visibility after overlay auto-hide, not merely the requested boolean |
| `SetFrameStyle(style, policy)` | `GetFrameStyle()` returns a copy of the configured style; maximized-state resolution can make the rendered effect differ |
| `SetOverlayEnabled(bool)` | `IsOverlayEnabled()` returns the requested policy; overlay layout takes effect only while attached and maximized |
| `SetOverlayAutoHideDelay(ms)` | `GetOverlayAutoHideDelay()` returns the configured delay; `0` means auto-hide is disabled |
| `ShowOverlayTemporarily()` | Performs a reveal/restart action; it does not enable overlay or change the configured delay |
| `SetInteractiveResizePolicy(policy)` | `GetInteractiveResizePolicy()` returns `FREE`, `KEEP_ASPECT_RATIO`, or `DISABLED`; the policy controls interactive resize only, not programmatic sizing |

These getters intentionally answer different questions. In particular, an enabled overlay policy does not mean the window is maximized, and `GetFrameStyle()` is not a snapshot of backend-resolved rendering views.

### Where to read the answer

| Question | API to use |
|---|---|
| Was a command accepted for dispatch? | `Request...()` return value or `CommandProcessedSignal()` |
| Did the compositor confirm normal, minimized, or maximized state? | `WindowStateChangedSignal()` and `GetWindowState()` |
| Was a requested maximize/restore frame presented? | `WindowStatePresentedSignal()` |
| What is the current surface/frame/content geometry? | `GetGeometry()` |
| Is move/resize input still active? | `InteractionChangedSignal()` |
| Did native move/resize finish? | `GeometryChangeCompletedSignal()` |
| Is decoration actually visible after overlay policy? | `IsDecorationVisible()` or `DecorationVisibilityChangedSignal()` |

See [window-frame.h](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/window/window-frame.h) and the related headers in the same directory for public declarations.

## 16. NUI and DALi UI Usage Comparison

| Area | Reviewed NUI `BorderWindow` usage | DALi UI `WindowFrame` |
|---|---|---|
| Content insertion | Application can need awareness of border/content structure | Add only to persistent `ContentRoot` |
| UI replacement | Relies on subclass or application composition | Atomically replaces slot and role objects at runtime |
| Shadow | Commonly applies N-patch assets around border views | Centralizes View shadow or N-patch in frame style |
| Completion callback | Uses a frame-presented callback before `OnMaximize`-style notification | Separates changed and presented signals |
| Extension | Primarily virtual override | Composition, interceptor, signal, and wrapper hooks |
| Size meaning | Border calculation can be exposed through application implementation | Publicly separates frame and content coordinates |
| Keep ratio | Uses `wm.policy.win.resize_aspect_ratio=1` | `SetInteractiveResizePolicy(WindowFrameInteractiveResizePolicy::KEEP_ASPECT_RATIO)` routes the same native policy through the operation adapter |

The primary usability improvement is separation between application content and window decoration. Application developers do not calculate border thickness or shadow. Only developers customizing the frame need the decoration API.

## 17. Reading the Sample

The complete executable example is [window-frame-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/samples/window-frame/window-frame-example.cpp).

1. Read `OnInit()` for window, options, signals, frame, size, and content order.
2. Read `BuildDecoration()` for top/bottom/side slots and role topology.
3. Read `BuildFrameStyle()` for shadow and corner radius.
4. Read `InstallFrame()` for atomic installation and replacement policy.
5. Read `CycleDecorationStyle()` for runtime UI replacement, driven by a button in the content root.
6. Read `UpdateWindowControlIcons()` for maximize/restore icon changes.
7. Read `OnCommand()` and `OnStatePresented()` as wrapper hook examples.

See [samples/window-frame/README.md](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/samples/window-frame/README.md) for build and interaction instructions.

## 18. Target Validation Checklist

Native move, resize, and state callback behavior can differ on Ubuntu and the target backend. Complete final validation on a Tizen device.

- Initial `400 x 260` frame and `200 x 130` minimum frame
- Resize only while dragging the bottom-left or bottom-right icon
- `KEEP_ASPECT_RATIO` preserves the native surface ratio from both resize handles
- Changing from `KEEP_ASPECT_RATIO` to `FREE` permits free resize without hiding handles
- Move by dragging the bottom move region
- Alignment of minimize, maximize/restore, close, and resize visuals
- Repeated move/resize and maximize/restore returns to compositor-managed geometry
- Unsized full-screen launch resolves as maximized and restores to `400 x 260`
- Overlay hides after its delay while enabled and maximized
- A touch on the hidden overlay surface reveals decoration
- Runtime style replacement preserves the content root and selected size policy
- A request is followed by compositor-confirmed state through `WindowStateChangedSignal()`
- Supported backends emit `WindowStatePresentedSignal()` after a requested maximize or restore is presented
- Resize emits intermediate `GeometryChangedSignal()` and final `GeometryChangeCompletedSignal()`
- Disabled move/resize/minimize/maximize features block their input and operations
- Presented callbacks, restore-size fallback, client controls, shadow, and target bounds use their dedicated APIs rather than operation feature gates

## Related Resources

- [Visual HTML guide (Korean)](./assets/WindowFrame/window-frame-guide.html)
- [WindowFrame public header](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/window/window-frame.h)
- [WindowFrame sample source](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/samples/window-frame/window-frame-example.cpp)
- [WindowFrame sample README](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/samples/window-frame/README.md)
