[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/View-(kr))

## What is View?

`View` is the basic UI building block of `dali-ui`. It provides the following main features:

* Multi-layer visual composition such as Shadow, Background, Content, and Border
* A target object arranged by layouts
* State management such as FOCUSED and DISABLED
* Accessibility support

## Data Attachment

Most DALi objects are defined with the p-impl structure, and `View` follows the same pattern. When writing native applications, unlike patterns often used with NUI or FLUX, simply deriving from a component such as View, Layout, or Button and adding application-specific data to the derived handle is not a valid native pattern. Data added to a p-impl handle can be lost during upcast operations.

As an alternative, `View` provides APIs for attaching data to a view. Attached data is managed so that it is destroyed together with the view instance.

<ins>Define data class</ins>
```cpp
class MyData
{
  /* ... */
};
```

<ins>Create a data and attach to a view</ins>
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

## View Architecture (Handle / Impl Pattern)

`View` is designed based on DALi's **p-impl (pointer-to-implementation) pattern**. Framework developers must understand this pattern to define new components correctly.

![view-pimpl-class](https://github.sec.samsung.net/NUI/dali-ui/assets/1000/45850c41-206f-4c15-8c67-a77e93f2345d)

<br/>

### Handle Class: `View`

`View` is a **handle class** that extends `Dali::CustomActor`. It does not directly own implementation data; instead, it references a `ViewImpl` instance through a ref-counted pointer.

- Copying a `View` creates a new handle pointing to the same `ViewImpl`.
- `ViewImpl` is automatically destroyed when all `View` handles are destroyed.

```cpp
// public-api/view.h (Dali::Ui namespace)
class View : public CustomActor
{
public:
  static View New();
  static View DownCast(BaseHandle handle);

  // Handle has a non-virtual destructor
  ~View();

  // No data or virtual functions — everything delegates to ViewImpl
};
```

<br/>

### Implementation Class: `ViewImpl`

`ViewImpl` is the **implementation class** that extends `Dali::CustomActorImpl`. All actual state and logic live here.

```cpp
// public-api/view-impl.h
class ViewImpl : public CustomActorImpl, public ConnectionTrackerInterface
{
public:
  static ViewImplPtr New();

protected:
  virtual MeasuredSize OnMeasure(float widthConstraint, float heightConstraint);
  virtual MeasuredSize OnArrange(const LayoutRect& bounds);
};
```

<br/>

### Accessing Impl from Handle: `GetImpl()`

The `View` class provides the `GetImpl()` helper to obtain an impl reference from a handle.

```cpp
// Defined in public-api/view-impl.h
ViewImpl& GetImpl(View& view);
const ViewImpl& GetImpl(const View& view);
```
```cpp
// Usage example
void MyFunction(View& view)
{
  ViewImpl& impl = GetImpl(view);
  // ...
}
```

<br/>

## Defining a Custom Component

When framework developers create a new component, or when an application needs a custom component, define a **handle class** and an **impl class** together.

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
    return View::DownCast<MyView, Integration::MyViewImpl>(handle);
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
  IntrusivePtr<Integration::MyViewImpl> impl = MyViewImpl::New();
  MyView handle(*impl);   // handle takes ownership of impl
  impl->Initialize();
  return handle;
}
```

<br/>

## Notes

| Item | Rule |
|------|------|
| Handle destructor | `~MyView()` must be **non-virtual** |
| Handle data | Do not add member variables to the handle class |
| Handle virtual functions | Do not add virtual functions to the handle class |
| Logic location | All state and logic must live in Impl |

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#documents)
