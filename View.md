[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/View-(kr))

## View Architecture (Handle / Impl Pattern)

`View` is designed based on DALi's **p-impl (pointer-to-implementation) pattern**. Framework developers must understand this pattern to define new components correctly.

<img src="./assets/view-structure.png" style="display:block;margin:0 auto"/>

<br/>

### Handle Class: `View`

`View` is a **handle class** that extends `Dali::CustomActor`. It does not directly own implementation data; instead, it references a `ViewImpl` instance through a ref-counted pointer.

- Copying a `View` creates a new handle pointing to the same `ViewImpl`.
- `ViewImpl` is automatically destroyed when all `View` handles are destroyed.

```cpp
// public-api/view.h
class View : public CustomActor
{
public:
  static View New();
  static View DownCast(BaseHandle handle);

  /* ... */
};
```

<br/>

### Impl Class: `ViewImpl`

`ViewImpl` is the **implementation class** that extends `Dali::CustomActorImpl`. All actual state and logic live here.

```cpp
// public-api/view-impl.h
class ViewImpl : public CustomActorImpl, public ConnectionTrackerInterface
{
public:
  static IntrusivePtr<ViewImpl> New();

  /* ... */
};
```

<br/>

### Accessing ViewImpl from View: `GetImpl()`

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

### Accessing View from ViewImpl: `Self()` + `DownCast()`

To access API from View, ViewImpl needs to create another handle from its instance.

```cpp
// 사용 예
View view = View::DownCast(impl.Self());
```

<br/>

## Data Attachment

Most DALi objects are defined with the p-impl structure, and `View` follows the same pattern. When writing native applications, unlike patterns often used with NUI or FLUX, simply deriving from a class such as View, Layout, or Button and adding application-specific data to the derived handle is not a valid native pattern. Data added to a p-impl handle can be lost during upcast operations.

As an alternative, `View` provides APIs for attaching data to a view. Attached data is managed so that it is destroyed together with the view instance.

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

<br/>

## Defining a Custom Component

When framework developers create a new component, or when an application needs a custom component, inherit View's **handle class** and **impl class** repectively.

<img src="./assets/custom-view.png" style="display:block;margin:0 auto"/>

### 1. Define the Impl Class

```cpp
// my-view-impl.h

class MyViewImpl : public ViewImpl
{
public:
  static IntrusivePtr<MyViewImpl> New();

protected:
  MeasuredSize OnMeasure(float w, float h) override;
  MeasuredSize OnArrange(const LayoutRect& bounds) override;
  void OnInitialize() override;
};
```

### 2. Define the Handle Class

```cpp
// my-view.h
class MyView : public View
{
public:
  static MyView New();

  // DownCast: use the View::DownCast<T, I>() template
  static MyView DownCast(BaseHandle handle)
  {
    return View::DownCast<MyView, MyViewImpl>(handle);
  }

  void DoSomething();

  ~MyView(); // non-virtual
};
```

### 3. Implement `New()`

```cpp
// my-view.cpp
MyView MyView::New()
{
  IntrusivePtr<MyViewImpl> impl = MyViewImpl::New();
  MyView handle(*impl);   // handle takes ownership of impl
  impl->Initialize();
  return handle;
}
```

<br/>

### Notes

| Item | Rule |
|------|------|
| Handle destructor | `~MyView()` must be **non-virtual** |
| Handle data | Do not add member variables to the handle class |
| Logic location | All state and logic must live in Impl |

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#documents)
