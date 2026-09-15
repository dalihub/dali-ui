# Dialog

[한국어](Dialog-(kr).md) · [Navigator](Navigator.md)

## Managed presentation (recommended)

`Dialog::Post(navigator)` creates its internal `DialogContainer` and registers it
with the supplied Navigator. `AlertDialog` inherits the same API. The host is
explicit: no automatic root-Navigator lookup or separate Window modal stack.

Use this path for ordinary dialogs, including scrim color/blur changes and
invisible scrims. Only use the [manual container path](#manual-presentation-with-navigator-advanced)
when the application needs a custom scrim View or complex scrim composition.
Do not mix the two ownership paths during a presentation.

```cpp
auto alert = AlertDialog::New();
alert.SetTitle("Delete item?");
alert.SetMessage("This action cannot be undone.");
alert.SetRequestedWidth(440.0f);
alert.SetLayoutParams(AbsoluteLayoutParams::New()
  .SetBounds(LayoutRect(0.5f, 0.5f, WRAP_CONTENT, WRAP_CONTENT))
  .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));
WeakHandle<AlertDialog> weakAlert(alert);
alert.AddActionButton("Cancel").ClickedSignal().Connect(this,
  [weakAlert](View, InputEvent) {
    if(auto value = weakAlert.GetHandle()) value.Dismiss();
  });
alert.Post(navigator);
```

Buttons do not dismiss automatically. Use a weak handle in a callback owned by
the Dialog's child to avoid an ownership cycle. Validate/save first if needed,
then dismiss that Dialog, not whichever modal happens to be on top.

| API | Contract |
|---|---|
| `Post(navigator[, options])` | Returns true on registration or an unchanged repeated Post to the same host. Returns false for another active host, a closing session, or a rejected registration. |
| `Dismiss(animated = true)` | Closes this presentation, including a covered Dialog. Repeated dismissal while closing, or while unparented and unposted, is a no-op. |
| `IsPosted()` | True while registered or closing, including when covered. Not a visibility query. |
| `SetDismissPolicy` / `GetDismissPolicy` | `NONE`, `BACK`, `SCRIM`, or default `BACK_AND_SCRIM`. |
| `DismissRequestedSignal()` | `bool(Dialog, DialogDismissReason)`; true vetoes a user request. |
| `ShownSignal()` | `void(Dialog)`; once after first appearance completes. |
| `HiddenSignal()` | `void(Dialog, DialogDismissReason)`; once after cleanup; reposting here is supported. |

Closing before appearance can produce Hidden without Shown. Covering and
uncovering an existing presentation does not emit Hidden or repeat Shown.
Reasons are `PROGRAMMATIC`, `BACK`, `SCRIM`, and `HOST_REMOVED`.
Navigator's BackRequested veto runs first. Explicit Dismiss, Navigator
PopModal/Remove/Clear, and host destruction bypass user-request vetoes.

### Presentation options

```cpp
DialogPostOptions options;
options.animated = false;
options.containerStyle = DialogContainerStyle::NoScrimPreset();
// options.transitionSpec = customModalTransition;
alert.Post(navigator, options);
```

Empty style/spec handles use the existing defaults. `NoScrimPreset()` removes
the tint and blur, not the input surface: background input stays blocked and
outside taps still follow the dismiss policy. Options apply only to a new
session. LayoutParams and child appearance are preserved; Post adds neither
automatic centering nor a new size clamp, focus trap, or scroll wrapper.

### Do not mix ownership paths

Prefer Post/Dismiss for ordinary dialogs, including tint/blur changes through
`DialogPostOptions::containerStyle`. Use the manual path below when you need a
custom scrim View or complex scrim composition owned by the application.
Do not obtain and customize the internal container of a posted Dialog.

Use either manual Add/Remove (including manual containers) or managed
Post/Dismiss. Posting/dismissing manually parented content always asserts.
While managed, directly Add/Remove/Unparent-ing the Dialog or its internal
container, or clearing/replacing the internal content, always asserts when
detected. Protection includes closing animations. Wait for Hidden before
switching paths. Legal Navigator removal APIs remain supported.

Assertions can occur after partial Actor hierarchy mutation: catching the
exception does not provide rollback or safe continued use. Manual container
presentations are not automatically adopted by Post.

`Dialog` is a three-section container component in `dali-ui-components`.
It provides header, body, and footer slots. `AlertDialog` builds on top of it
for common title/message/action-button dialogs, and `DialogContainer` presents
dialog content above a scrim.

Dialogs are usually shown modally through [Navigator](Navigator.md).

## Reusable styles

For an interactive comparison and modal lifecycle checks, see the registered
[Dialog manual scenario](../manual-tests/dali-ui-components/tc/tc-dialog-basics.md).

All three types accept an immutable typed style: `Dialog::New(DialogStyle)`,
`AlertDialog::New(AlertDialogStyle)`, and `DialogContainer::New(DialogContainerStyle)`.
`New()` uses the matching `Default()`. Each style supports `DefaultKey()`,
`DefaultPreset()`, `Default()`, `Configure()`, and a move-only `Builder`.
`Build()` consumes the builder. Passing an empty style to `New(style)` asserts.

```cpp
auto appearance = DialogStyle::Default().Configure()
  .SetBackgroundColor(UiColor::SURFACE)
  .SetPadding(Insets(24.0f, 16.0f))
  .SetSpacing(12.0f).Build();
auto actions = TextButtonStyle::Default().Configure()
  .SetTextColor(UiColor::PRIMARY).Build();
auto style = AlertDialogStyle::Default().Configure()
  .SetDialogStyle(appearance).SetActionButtonStyle(actions)
  .SetTitleFontSize(24.0f).Build();
auto alert = AlertDialog::New(style);
alert.SetTitle("Delete item?");
alert.SetMessage("This action cannot be undone.");
alert.AddActionButton("Cancel");

DialogPostOptions options;
options.containerStyle = DialogContainerStyle::Default().Configure()
  .SetScrimColor(UiColor(0x000000u, 0.6f)).Build();
alert.Post(navigator, options);
```

Register custom default creators with `config.StyleSheet().SetStyle(TypeStyle::DefaultKey(), creator)`
before applying the Components config. A creator must use `DefaultPreset()` or a
builder, not recursively call its own `Default()`.

`DialogStyle` owns the dialog's requested size, background, overall padding,
section spacing, corners, shadows, borders, and blur. The supplied header, body,
and footer Views own their padding and appearance. Configure each View directly
before attaching it; Dialog does not override those values when attaching,
replacing, or reordering sections. Section layout and alignment remain managed
by Dialog.

```cpp
auto header = Label::New("Custom header");
header.SetPadding(Insets(8.0f, 4.0f));
dialog.SetHeaderView(header);
```

`AlertDialogStyle` composes a `DialogStyle` and `TextButtonStyle`, and adds title/message
font/color and action-row height/spacing. Empty nested styles select the documented
defaults at creation. An explicit action style is used without Alert's legacy color/font
overrides. The implicit action style retains configured TextButton geometry/effects
and the existing Alert color/font defaults. Existing generated labels retain their
identity and custom font/color on text updates; clearing text removes that section.

`DialogContainerStyle` affects only its generated scrim. Replacing the scrim preserves
the supplied view's color/effect. The default scrim remains clickable but does not take
keyboard or touch focus.

Both blur-radius setters accept finite values in `[0, 4096]`; zero disables blur,
and fractional parts are truncated when creating the effect. Larger values assert
to prevent unsafe conversion and excessive renderer-side radius reduction.

`Dialog` and `AlertDialog` expose the accessibility Dialog role. `DialogContainer`
temporarily marks its content modal, preserves its role, and restores the prior
explicit modal flag on removal, transfer, or destruction. Reinstalling the same
handle is a no-op. This is distinct from the implicit modal state of a Dialog role.
Showing notifications follow actual accessible visibility, including scene removal,
ancestor visibility, world alpha, and culling. Applications still own focus placement,
focus restoration, accessible names, dismissal callbacks, and keyboard policy.
State UTCs are not evidence of actual screen-reader speech or D-Bus delivery.

The built-in Dialog surface uses the OneUI fallback appearance: absolute corner
radius 44, white at alpha 0.6, two rounded-cutout shadows (blur 8/32, offset Y 0/16,
black alpha 0.05/0.1), and an inner border (width 2, offset -1, #FCFCFF at alpha 0.05).
AlertDialog inherits this surface unless its nested DialogStyle is supplied.
The default generated scrim uses black at alpha 0.4 and blur radius 100;
`NoScrimPreset()` explicitly disables both. Shadow and corner values use DALi UI's
existing scaling path; no additional OneUI pixel conversion is applied.

These are fallback values, not a full OneUI light/dark theme implementation.
Use the existing style creators for theme-specific colors and geometry.
Title/message typography and the Alert action palette remain unchanged.
To obtain an unstyled surface, explicitly set transparent
background, zero radius/border width and an empty ShadowStack on a configured style.
Styles apply at construction, not as a live replacement of instance overrides.
See the [buildable sample](../samples/dialog/README.md), which retains the default surface.

### Content layout defaults

The OneUI layout stacks title, body and actions from the top; it does not pin the
footer or assign a weight to the body. Dialog defaults to width 908 and
`WRAP_CONTENT` height, own padding `(44, 44, 36, 32)` and section spacing 12.
The common OneUI horizontal section insets are folded into Dialog's own padding;
padding on supplied section Views is never overwritten.

Generated Alert title/message labels are multiline. Actions form a centered
horizontal group at their natural/minimum sizes, not equal stretched widths.
The fallback buttons use minimum size 144×64 and padding `(24, 4)`; an explicit
ActionButtonStyle or configured TextButtonStyle provider takes precedence for
button geometry. The generated action row defaults to `WRAP_CONTENT`, top
padding 12 and spacing 32. `SetActionRowHeight`, `SetActionRowPadding` and
`SetActionButtonSpacing` customize that generated row only, not an external Footer.
An explicit non-negative row height remains supported.

Choose a width suitable for the host and action content through existing style
and layout APIs (the example uses 440). Fixed heights remain available but can
leave unused space or constrain content. Post preserves these settings; it does
not shrink oversized content, wrap the action row, or add a scroll container.
For content-height dialogs, use RequestedWidth for the desired width and leave
the absolute bounds dimensions at WRAP_CONTENT. A conflicting fixed bounds width
can make text measurement and final arrangement use different widths.

---

## Basic Setup

```cpp
#include <dali-ui-components/dali-ui-components.h>

using namespace Dali;
using namespace Dali::Ui;

Application application = Application::New(&argc, &argv);
Components::UiConfig::New().Apply();
```

---

## Dialog

`Dialog` has three optional sections:

| Section | API | Typical content |
|---|---|---|
| Header | `SetHeaderView()` | Title, icon, or heading content |
| Body | `SetBodyView()` | Message, form, list, or custom content |
| Footer | `SetFooterView()` | Action buttons or status content |

```cpp
Dialog dialog = Dialog::New();
dialog.SetBackgroundColor(UiColor(0xFFFFFFu));
dialog.SetSpacing(12.0f);
dialog.SetLayoutAlignment(LayoutAlignment::FILL);

Label title = Label::New("Network settings");
Label message = Label::New("Choose how this device connects to the network.");

dialog.SetHeaderView(title);
dialog.SetBodyView(message);
```

Section and layout APIs:

```cpp
dialog.SetHeaderView(headerView);
dialog.SetBodyView(bodyView);
dialog.SetFooterView(footerView);

View header = dialog.GetHeaderView();
View body   = dialog.GetBodyView();
View footer = dialog.GetFooterView();

dialog.SetSpacing(8.0f);
dialog.SetLayoutAlignment(LayoutAlignment::CENTER);
```

Passing an empty handle clears a section.

---

## AlertDialog

`AlertDialog` is a convenience dialog for title, message, and action buttons.

```cpp
AlertDialog alert = AlertDialog::New();
alert.SetBackgroundColor(UiColor(0xFFFFFFu));
alert.SetSpacing(8.0f);

alert.SetTitle("Delete item?");
alert.SetMessage("This action cannot be undone.");
TextButton cancel = alert.AddActionButton("Cancel");
cancel.ClickedSignal().Connect(this, [](View, InputEvent) {
  // Handle cancel.
});
TextButton remove = alert.AddActionButton("Delete");
remove.ClickedSignal().Connect(this, [](View, InputEvent) {
  // Handle delete.
});
```

| API | Description |
|---|---|
| `SetTitle(text)` / `GetTitle()` | Sets or reads the header title. |
| `SetMessage(text)` / `GetMessage()` | Sets or reads the body message. |
| `AddActionButton(text)` | Adds a natural/minimum-size button to the centered action group and returns it for signal connection or customization. Does not dismiss automatically. |
| `ClearActionButtons()` | Removes all action buttons and clears the footer. |

---

## DialogContainer

`DialogContainer` wraps modal content with a scrim. The scrim fills the
container and the built-in scrim can be tapped for dismiss behavior. The snippets
in this section describe manual assembly only; `alert` must not be posted or
owned by another parent. Use Post/Dismiss for ordinary presentation.

```cpp
DialogContainer container = DialogContainer::New();
container.SetModalContent(alert);
```

Scrim APIs:

```cpp
View scrim = View::New();
scrim.SetBackgroundColor(UiColor(0x00000080u));

container.SetScrim(scrim);
View currentScrim = container.GetScrim();
```

Scrim click signal:

```cpp
container.ScrimClickedSignal().Connect(
  this,
  [](DialogContainer container) {
    // Dismiss the container here.
  });
```

When a `DialogContainer` is pushed through `Navigator::PushModal()`,
`Navigator` can dismiss it when the scrim is tapped.
For a custom scrim, click forwarding requires an `InteractiveView`; a plain
`View` such as the one above supplies appearance only. Its color/effects are
preserved, while its requested size is set to fill the container.

---

## Manual presentation with Navigator (advanced)

Use this alternative for a custom scrim View, complex scrim composition, or
explicit container ownership. A color/blur change alone does not require it;
use Post options instead. This is a separate example, not a continuation of
an active Post session. Do not call Post/Dismiss on manually attached content.

```cpp
AlertDialog alert = AlertDialog::New();
alert.SetTitle("Delete item?");
alert.SetMessage("This action cannot be undone.");

DialogContainer container = DialogContainer::New();
View customScrim = View::New();
customScrim.SetBackgroundColor(UiColor(0x000000u, 0.6f));
// Compose custom scrim content/effects before attaching it.
container.SetScrim(customScrim);
container.SetModalContent(alert);

navigator.PushModal(container);
```

Dismiss it from an action button:

```cpp
TextButton cancel = alert.AddActionButton("Cancel");
WeakHandle<Navigator> weakNavigator(navigator);
WeakHandle<DialogContainer> weakContainer(container);
cancel.ClickedSignal().Connect(this, [weakNavigator, weakContainer](View, InputEvent) {
  auto host = weakNavigator.GetHandle();
  auto modal = weakContainer.GetHandle();
  if(host && modal) host.Remove(modal);
});

TextButton remove = alert.AddActionButton("Delete");
remove.ClickedSignal().Connect(this, [weakNavigator, weakContainer](View, InputEvent) {
  auto host = weakNavigator.GetHandle();
  auto modal = weakContainer.GetHandle();
  if(host && modal) host.Remove(modal);
});
```

`Navigator::NavigateBack()` also dismisses the top modal item before popping a
normal page.

`Remove(container)` removes that exact container immediately. Use PopModal
only when the target is the top modal. If the app retains the manual container,
its content stays parented until `container.SetModalContent(View{})` or container
destruction. Finish removal and detach content before switching to Post. In the
other direction, wait for Hidden after Dismiss; closing animations still belong
to the managed session. Dialog's managed lifecycle signals/policy do not manage
manually assembled presentations.

---

## Layout and Sizing

Dialog content follows its own layout parameters inside `DialogContainer`.
To center a dialog over the scrim, use `AbsoluteLayoutParams` with proportional
positioning.

```cpp
alert.SetRequestedWidth(600.0f);
alert.SetLayoutParams(AbsoluteLayoutParams::New()
                        .SetBounds(LayoutRect(0.5f, 0.5f, WRAP_CONTENT, WRAP_CONTENT))
                        .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));
```

---

## Modal Transition

Dialog transitions use Navigator in both paths. For the recommended managed
path, select a per-session transition with `DialogPostOptions::transitionSpec`
and use `options.animated` / `Dismiss(animated)` for animation choice. The
following lower-level examples are for manually owned containers only.

```cpp
navigator.SetModalTransitionAnimationEnabled(false);
navigator.PushModal(container, true);  // Finishes immediately while disabled.
```

Use a modal transition spec when dialogs need a different transition from
normal page navigation:

```cpp
NavigationTransitionSpec modalSpec = NavigationTransitionSpec::New();
modalSpec.SetDuration(0.25f);
modalSpec.EnterSignal().Connect(this, [](Animation& anim, View view) {
  view.SetProperty(Actor::Property::OPACITY, 0.0f);
  anim.AnimateTo(Property(view, Actor::Property::OPACITY), 1.0f);
});

navigator.SetModalTransitionSpec(modalSpec);
navigator.SetPageModalTransitionSpec(container, modalSpec);
```

See [Navigator](Navigator.md#transition-specification) for the full transition API.

For both managed and manual DialogContainer presentations, transition animator
and snap callbacks receive the **modal content**, not the container. Navigator
adds a separate linear scrim fade to the same Animation. The scrim stays in
place while the content scales or moves; the container continues blocking
background input. Only the newly pushed or removed scrim fades, not a covered
or revealed lower scrim. A custom scrim's original opacity is restored afterwards.

The existing NavigationTransitionSpec, keyframe callbacks and duration API are
unchanged; no additional ViewAnimationSpec API is required. Existing callbacks
that downcast the target to DialogContainer must be updated. Keep content layout
coordinates intact, and use snap callbacks to settle custom transformations.
Before removal, Navigator restores content position, scale and opacity to their
pre-exit values so a Hidden callback can repost it. Other properties remain the
application's responsibility. Shown/Hidden wait for the full transition and the
existing presentation lifecycle conditions, not just the scrim fade.

---

## Notes

- Use `Dialog` for custom header, body, and footer layouts.
- Use `AlertDialog` for common title/message/action dialogs.
- Prefer `Dialog::Post()` / `Dismiss()` for managed presentation.
- Use `DialogContainer` and Navigator modal APIs for manual ownership and custom assembly.

The new presentation APIs preserve existing public handle sizes, inheritance,
and function signatures. External subclasses that depend directly on Integration
implementations must be rebuilt because their internal data and overrides changed.
This is not a claim of complete ABI certification using binary comparison tools.

<br/>

---

[Back to Components](Components.md)
