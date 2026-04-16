[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/API-Levels-&-View-Hierarchy-(kr))

## API Levels

`dali-ui` exposes three API levels with different stability guarantees and intended audiences:

| Level | Location | Audience | ABI Stability |
|---|---|---|---|
| **Public API** | `public-api/` | App developers | **Guaranteed** — must not break across releases |
| **Integration API** | `integration-api/` | Framework developers | Not required — consumers rebuild alongside dali-ui |
| **Internal API** | `internal/` | dali-ui internals only | Not required |

**As an app developer**, use only `public-api/`. This is the only layer with guaranteed ABI stability.

**As a framework developer**, you may also depend on `integration-api/`, with the understanding
that it may change between dali-ui versions and will require a rebuild.

Direct use of `internal/` from outside the library is not supported.

<br/>

## View Architecture (Handle / Impl Pattern)

dali-ui `View`s are built on DALi's **p-impl (pointer-to-implementation) pattern**. Framework developers must understand this pattern to define new components correctly.

![view-pimpl-class](https://github.sec.samsung.net/NUI/dali-ui/assets/1000/45850c41-206f-4c15-8c67-a77e93f2345d)

<br/>

### Handle Class: `View` (public-api)

`View` is a **handle class** that extends `Dali::CustomActor`. It holds no implementation data directly — it references a `ViewImpl` instance through a ref-counted pointer.

- Copying a `View` creates a new handle pointing to the same `ViewImpl`.
- The `ViewImpl` is automatically destroyed when the last `View` handle goes out of scope.
- `~View()` is **non-virtual**. Handle classes must not contain virtual functions or member data.

```cpp
// public-api/view.h  (namespace Dali::Ui)
class View : public CustomActor
{
public:
  static View New();
  static View DownCast(BaseHandle handle);

  ~View(); // non-virtual

  // No data, no virtual functions — everything delegates to ViewImpl
};
```

<br/>

### Implementation Class: `ViewImpl` (integration-api)

`ViewImpl` is the **implementation class** that extends `Dali::CustomActorImpl`. All state and logic live here. It resides in the `Dali::Ui::Integration` namespace.

- Override virtual methods such as `OnMeasure()` / `OnArrange()` to customize behavior.
- Ref-counting is managed through `IntrusivePtr<ViewImpl>` (`ViewImplPtr`).

```cpp
// integration-api/view-impl.h  (namespace Dali::Ui::Integration)
class ViewImpl : public CustomActorImpl, public ConnectionTrackerInterface
{
public:
  static ViewImplPtr New();

protected:
  virtual MeasuredSize OnMeasure(float widthConstraint, float heightConstraint);
  virtual MeasuredSize OnArrange(const LayoutRect& bounds);

  // Update state bit (called by traits)
  void SetViewState(UiState state, bool on, InputEvent cause = {});
};
```

<br/>

### Accessing Impl from Handle: `GetImpl()`

Framework developers using the integration-api obtain an impl reference from a handle via the `GetImpl()` helper.

```cpp
// Defined in integration-api/view-impl.h
Integration::ViewImpl& GetImpl(Ui::View& view);
const Integration::ViewImpl& GetImpl(const Ui::View& view);

// Usage
void MyFunction(Ui::View& view)
{
  Integration::ViewImpl& impl = Integration::GetImpl(view);
  impl.SetViewState(UiState::FOCUSED, true);
}
```

<br/>

## Defining a New Component

To create a new component, define a **handle class** and an **impl class** together. `Label` / `LabelImpl` follows the same convention.

### 1. Define the Impl class (integration-api)

```cpp
// my-widget-impl.h
namespace Dali::Ui::Integration {

class MyWidgetImpl : public ViewImpl
{
public:
  static IntrusivePtr<MyWidgetImpl> New();

protected:
  MeasuredSize OnMeasure(float w, float h) override;
  MeasuredSize OnArrange(const LayoutRect& bounds) override;
  void OnInitialize() override;
};

} // namespace Dali::Ui::Integration
```

### 2. Define the Handle class (public-api)

```cpp
// my-widget.h
namespace Dali::Ui {

class MyWidget : public View
{
public:
  static MyWidget New();

  // Use the View::DownCast<T, I>() template
  static MyWidget DownCast(BaseHandle handle)
  {
    return View::DownCast<MyWidget, Integration::MyWidgetImpl>(handle);
  }

  void DoSomething();

  ~MyWidget(); // non-virtual
};

} // namespace Dali::Ui
```

### 3. Implement `New()`

```cpp
// my-widget.cpp
MyWidget MyWidget::New()
{
  IntrusivePtr<Integration::MyWidgetImpl> impl = Integration::MyWidgetImpl::New();
  MyWidget handle(*impl);   // handle takes ownership
  impl->Initialize();
  return handle;
}
```

<br/>

## Rules to Follow

| Rule | Details |
|------|---------|
| Non-virtual destructor | `~MyWidget()` must be **non-virtual** |
| No member data in handle | Never add member variables to a handle class |
| No virtual functions in handle | Never add virtual functions to a handle class |
| Logic belongs in Impl | All state and behavior must live in the Impl |
| ABI stability | The public-api handle class ABI must be protected at all costs |

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#documents)
