# Dialog

`Dialog` is a three-section container component in `dali-ui-components`.
It provides header, body, and footer slots. `AlertDialog` builds on top of it
for common title/message/action-button dialogs, and `DialogContainer` presents
dialog content above a scrim.

Dialogs are usually shown modally through [Navigator](Navigator.md).

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
alert.SetActionButtons({
  {"Cancel", []() {
    // Handle cancel.
  }},
  {"Delete", []() {
    // Handle delete.
  }}
});
```

| API | Description |
|---|---|
| `SetTitle(text)` / `GetTitle()` | Sets or reads the header title. |
| `SetMessage(text)` / `GetMessage()` | Sets or reads the body message. |
| `SetActionButtons(buttons)` | Builds footer action buttons from `(label, callback)` pairs. |

---

## DialogContainer

`DialogContainer` wraps modal content with a scrim. The scrim fills the
container and can be tapped for dismiss behavior.

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

---

## Presenting with Navigator

Use `Navigator::PushModal()` to show a dialog as modal content.

```cpp
AlertDialog alert = AlertDialog::New();
alert.SetTitle("Delete item?");
alert.SetMessage("This action cannot be undone.");

DialogContainer container = DialogContainer::New();
container.SetModalContent(alert);

navigator.PushModal(container);
```

Dismiss it from an action button:

```cpp
alert.SetActionButtons({
  {"Cancel", [navigator]() mutable {
    navigator.PopModal();
  }},
  {"Delete", [navigator]() mutable {
    navigator.PopModal();
  }}
});
```

`Navigator::NavigateBack()` also dismisses the top modal item before popping a
normal page.

---

## Layout and Sizing

Dialog content follows its own layout parameters inside `DialogContainer`.
To center a dialog over the scrim, use `AbsoluteLayoutParams` with proportional
positioning.

```cpp
alert.SetLayoutParams(AbsoluteLayoutParams::New()
                        .SetBounds(LayoutRect(0.5f, 0.5f, 600.0f, 340.0f))
                        .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));
```

---

## Modal Transition

Dialog transition behavior is controlled by `Navigator`, because dialogs are
usually presented through `PushModal()` and `PopModal()`.

```cpp
navigator.SetModalTransitionAnimationEnabled(false);
navigator.PushModal(container, true);  // Finishes immediately while disabled.
```

Use a modal transition spec when dialogs need a different transition from
normal page navigation:

```cpp
auto modalSpec = std::make_shared<NavigationTransitionSpec>();
modalSpec->duration = 0.25f;
modalSpec->enter = [](Animation& anim, View view) {
  view.SetProperty(Actor::Property::OPACITY, 0.0f);
  anim.AnimateTo(Property(view, Actor::Property::OPACITY), 1.0f);
};

navigator.SetModalTransitionSpec(modalSpec);
navigator.SetPageModalTransitionSpec(container, modalSpec);
```

See [Navigator](Navigator.md#transition-basics) for the full transition API.

---

## Notes

- Use `Dialog` for custom header, body, and footer layouts.
- Use `AlertDialog` for common title/message/action dialogs.
- Use `DialogContainer` when the dialog should be shown over a scrim.
- Use `Navigator::PushModal()` and `Navigator::PopModal()` for modal dialog
  presentation and modal transition behavior.

<br/>

---

[Back to Components](Components.md)
