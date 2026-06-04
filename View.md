[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/View-(kr))

## View Architecture

`View` is designed based on the **p-impl (pointer-to-implementation) pattern**. Developers must understand this pattern to define new components correctly.

<img src="./assets/view-structure.png" style="display:block;margin:0 auto"/>

<br/>

Class | Role | Description
--|--|--
`View` | Handle | Holds only a pointer to a `ViewImpl` instance
`ViewImpl` | Implementation | Actual implementation object that owns data and logic

<br/>

<ins>Memory management structure</ins>
- Copying a `View` creates a new handle pointing to the same `ViewImpl`.
- `ViewImpl` is automatically destroyed when all `View` handles are destroyed.

<br/>

<details>
<summary><ins>Mutual access methods</ins></summary>

#### Accessing ViewImpl from View: `GetImpl()`

The `View` class provides the `GetImpl()` helper to obtain an impl reference from a handle.

```cpp
// Defined in public-api/view-impl.h
ViewImpl& GetImpl(View& view);
const ViewImpl& GetImpl(const View& view);
```
```cpp
// Usage example
ViewImpl& impl = GetImpl(view);
```

<br/>

#### Accessing View from ViewImpl: `Self()` + `DownCast()`

To use View APIs from a ViewImpl instance, create an additional View handle as follows.

```cpp
// Usage example
View view = View::DownCast(impl.Self());
```

</details>

<br/>

## View Extensibility

This section explains how to attach user data or specific features to `View`, and how to define a new class by inheriting from `View`. It is intended to help users choose an appropriate way to extend `View` depending on their purpose.

<br/>

### 1. Data Attachment

`dali-ui` provides APIs for attaching user data to a view. User data is managed so that it is destroyed together with the view instance.

<ins>Define data class</ins>
```cpp
class MyData
{
  /* ... */
};
```

<ins>Create a data instance and attach to a view</ins>
```cpp
const AttachmentId MY_DATA_ID = AttachmentId::Alloc();
```
```cpp
view.SetAttachment(MY_DATA_ID, Dali::MakeUnique<MyData>());
```

<ins>Access to data</ins>
```cpp
MyData* data = view.GetAttachment(MY_DATA_ID);
```

If the only purpose is to store data, **data attachment** is preferred over inheritance.

<br/>

### 2. Layout Attachment

Users can make a `View` instance act as a layout by attaching a `LayoutManager` to it. For example, if `GridLayoutManager` is connected to a view, that view arranges its children like a `GridLayout`.

<img src="./assets/layout-manager.png"/>


```cpp
// Make view to arrange its children like a GridLayout
view.AttachLayoutManager(MakeUnique<GridLayoutManager>());
```

With this feature, a view can take on a layout role without adding a separate layout view as a child. **This can help reduce the number of views** when implementing applications or components.

<br/>

### 3. Feature Attachment

`dali-ui` provides several feature modules that can be attached to a view.

Feature | Usage | Description
-- | -- | --
Interaction | `view.AsInteractive()` | Converts View key/touch inputs into high-level signals <ul><li>Provided signals: `Clicked`, `LongPressed`, etc.</li><li>Provided state: `PRESSED`</li></ul>
Selection | `view.AsSelectable()` | Toggles the selection state using the View's clicked interaction as a trigger <ul><li>Provided signal: `SelectionChanged`</li><li>Provided state: `SELECTED`</li></ul>

```cpp
view.AsInteractive([&](InteractiveTrait trait)
{
  trait.ClickedSignal().Connect(&tracker, [](View v, const InputEvent& e)
  {
    // Clicked (touch tap or execution key press)
  });
});
```

With this feature, users can make a view perform the role of a `Button` or `Switch` component without inheriting from those components, while freely composing the inside of the view.

<br/>

### 4. View Inheritance

When framework developers create a new component, or when an application needs a custom component, inherit from `View`'s **handle class** and **impl class** respectively.

> [!NOTE]
> <b>Special nature of ViewImpl</b> <br/>
> In principle, impl classes are not exposed outside the library in order to preserve ABI stability. `ViewImpl` is an exception and is exposed at the public level. dali-ui controls changes to the object size and vtable of `ViewImpl` so that ABI stability can be maintained.

<img src="./assets/custom-view.png" style="display:block;margin:0 auto"/>

<br/>


#### MyView Example ([Link](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/samples/my-view))

* [my-view.h](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/samples/my-view/my-view.h)
* [my-view.cpp](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/samples/my-view/my-view.cpp)
* [my-view-impl.h](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/samples/my-view/my-view-impl.h)
* [my-view-impl.cpp](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/samples/my-view/my-view-impl.cpp)

<br/>

#### Class Description

Name | Role | Main Methods
-- | -- | --
`MyView` | <ul><li><b>Handle class</b></li><li>Inherits from View</li></ul> | <ul><li>`static New()`</li><li>`MyView DownCast(BaseHandle)`</li><li>`MyView(MyViewImpl)` constructor</li></ul>
`MyViewImpl` | <ul><li><b>Impl class</b></li><li>Inherits from ViewImpl</li></ul> | <ul><li>`static New()`</li><li>`void OnInitialize()`</li></ul>

<br/>

#### Other Important Points

* [GetImpl(MyView&)](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/samples/my-view/my-view.cpp#L13): obtains the impl from the handle
* [Self()](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/samples/my-view/my-view-impl.cpp#L57): obtains the handle from the impl
* [DALI_TYPE_REGISTRATION_BEGIN(..)](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/samples/my-view/my-view-impl.cpp#L19): macro for DALi type registration


<br/>

#### Notes

| Item | Rule |
|------|------|
| Handle destructor | `~MyView()` must be **non-virtual** |
| Handle data | Do not add member variables to the handle class |
| Logic location | All state and logic must live in Impl |

<br/>

#### AI Agent Skill

To generate a `View`-derived class with an AI agent, see [AI Agent Skills - View Inheritance](https://github.sec.samsung.net/NUI/dali-ui/wiki/AI-Agent-Skills#view-inheritance).

<br/>

### 5. Inheriting from Classes Derived from View

Applications need a small trick to inherit from classes such as `ImageView` or `Label`. Since `View` is the only class for which both the handle and impl classes are provided, inheriting from other classes such as `Label` requires a limited approach using data attachment. For details and precautions, see [#327](https://github.sec.samsung.net/NUI/dali-ui/pull/327).

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#view)
