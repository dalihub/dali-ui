[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/View-Annotation-(kr))

# View Annotation for App Entities

This guide explains how a dali-ui application attaches application-defined
entity metadata to a `Dali::Ui::View`. The Tizen application entity service can
then expose annotated Views through `Tizen.Action.View`.

The annotation APIs are used through `Dali::Ui::View`, and application UI code
should attach the metadata to the View that represents the entity.

<br/>

## 1. Setting an Annotation

An annotation consists of one application-defined identifier, type, and
free-form information string.

```cpp
#include <dali-ui-foundation/dali-ui-foundation.h>

Dali::Ui::View lightCard = Dali::Ui::View::New();
lightCard.SetAnnotation(
  "living-room.light",
  "Tizen.Entity.Light",
  R"({"power":true,"level":72})");
```

Calling `SetAnnotation()` again replaces the complete annotation. The strings
are stored on the View and remain valid for the View lifetime or until
`ClearAnnotation()` is called.

> [!NOTE]
> Annotation data is event-side metadata. It is not a registered View property and does
> not participate in property notifications, serialization, or the
> update/render thread.

<br/>

## 2. Reading and Clearing

```cpp
Dali::String entityId;
Dali::String entityType;
Dali::String entityInfo;

if(lightCard.GetAnnotation(entityId, entityType, entityInfo))
{
  // Use the stored values.
}

lightCard.ClearAnnotation();
```

`GetAnnotation()` returns `false` when no annotation is set. In that case, all
three output parameters remain unchanged. `entityInfo` may be empty even when
an annotation exists.

<br/>

## 3. View ID and Entity ID

The two identifiers have different lifetimes and purposes:

| Identifier | Source | Purpose |
|---|---|---|
| View ID | `view.GetId()` | Runtime identifier used by `FindById`. Valid for the View lifetime. |
| Entity ID | First value passed to `SetAnnotation()` | Stable, application-defined identity such as `living-room.light`. |

Do not store a View ID as the application entity ID. Recreating the View may
produce a different View ID while its logical entity ID remains the same.

<br/>

## 4. Focus Integration

Use `Dali::Ui::FocusManager` normally:

```cpp
lightCard.SetFocusable(true);
window.Add(lightCard);

Dali::Ui::FocusManager::Get().SetCurrentFocusView(lightCard);
```

dali-ui automatically supplies the View currently focused by its `FocusManager`
to dali-adaptor. Applications do not register a provider. Consequently,
`GetFocusedView` reflects the current dali-ui focus after normal focus setup.

<br/>

## 5. Tizen Action Mapping

On Tizen, dali-adaptor maps View state to the canonical action contract:

| Action | Meaning |
|---|---|
| `FindById` | Finds a View using its decimal View ID. |
| `GetAnnotatedViews` | Returns annotated Views that satisfy visibility requirements. |
| `GetFocusedView` | Returns the View currently owned by `Ui::FocusManager`. |
| `ToPresentation` | Produces the platform presentation document. |

The `View` terminology in these action names belongs to the external
`Tizen.Action.View` contract. Application code continues to use normal
`Dali::Ui::View` APIs.

<br/>

## 6. Tizen RPC Permissions

A Tizen client that connects to the action through rpc-port must declare the
standard RPC connection privileges in its application manifest:

```xml
<privileges>
  <privilege>http://tizen.org/privilege/appmanager.launch</privilege>
  <privilege>http://tizen.org/privilege/datasharing</privilege>
</privileges>
```

These privileges belong to the Tizen client/transport path and are not needed
by the portable Ubuntu or Windows sample. Any additional method privilege is
owned by the canonical `Tizen.Action.View` contract rather than dali-ui.

<br/>

## 7. Portable Interactive Sample

The [`app-entity-annotation`](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/app-entity-annotation) sample
runs on Ubuntu and Windows. It uses real View annotation and focus APIs and
provides a local Action Console in place of the Tizen TIDL/rpc-port transport.

The sample demonstrates:

- Three annotated View cards in a realistic dashboard.
- Click, touch, and keyboard focus changes.
- Runtime annotation removal and restoration.
- View ID versus application entity ID.
- Local `FindById`, `GetAnnotatedViews`, `GetFocusedView`, and
  `ToPresentation` request flows.

The local simulator does not claim to validate Tizen RPC serialization,
privileges, or host lifecycle. Those are covered by dali-adaptor integration
tests.

<br/>

## 8. Recommendations

- Choose an entity ID that remains stable when the View is recreated.
- Use a namespaced entity type owned by the application or platform contract.
- Keep `entityInfo` compact and version its format when consumers depend on it.
- Clear the annotation when a View should no longer be discoverable.
- Do not depend on View IDs across process or View lifetimes.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)
