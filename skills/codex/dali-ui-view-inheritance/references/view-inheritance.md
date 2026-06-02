# View Inheritance Reference

## Reference Template

Use the self-contained template below.

Template class names are sample names only. Replace them with the user's requested class name.

## Target Directory and Namespace

Before creating files, determine the target directory and namespace from the user's request and the local project structure.

- If the local project uses the `dali-ui` API-level layout, follow its existing public-api / integration-api / internal patterns.
- In an application or extension project, do not assume the `public-api`, `integration-api`, or `internal` directory structure exists.
- If the target directory or namespace is unclear, ask the user before creating files.

## Self-contained Template

### Handle Header

```cpp
#pragma once

#include <dali-ui-foundation/dali-ui-foundation.h>

namespace MyNamespace
{

class ProductCardViewImpl;

class ProductCardView : public Dali::Ui::View
{
public:
  static ProductCardView New();
  static ProductCardView DownCast(Dali::BaseHandle handle);

  ProductCardView();
  ProductCardView(const ProductCardView& view);
  ProductCardView(ProductCardView&& rhs) noexcept;
  DALI_INTERNAL explicit ProductCardView(ProductCardViewImpl& impl);
  DALI_INTERNAL explicit ProductCardView(Dali::Internal::CustomActor* customActor);

  ~ProductCardView(); // non-virtual

  ProductCardView& operator=(const ProductCardView& handle) = default;
  ProductCardView& operator=(ProductCardView&& rhs) noexcept = default;

  void DoSomething();

  DALI_UI_CHAIN_VIEW_METHODS(ProductCardView)

  // Do not add data fields to the handle class.
};

} // namespace MyNamespace
```

### Handle Source

```cpp
#include "product-card-view.h"
#include "product-card-view-impl.h"

namespace MyNamespace
{

namespace
{
inline ProductCardViewImpl& GetImpl(ProductCardView& view)
{
  DALI_ASSERT_ALWAYS(view);
  Dali::RefObject& handle = view.GetImplementation();
  return static_cast<ProductCardViewImpl&>(handle);
}

inline const ProductCardViewImpl& GetImpl(const ProductCardView& view)
{
  DALI_ASSERT_ALWAYS(view);
  const Dali::RefObject& handle = view.GetImplementation();
  return static_cast<const ProductCardViewImpl&>(handle);
}
} // unnamed namespace

ProductCardView ProductCardView::New()
{
  Dali::IntrusivePtr<ProductCardViewImpl> impl = ProductCardViewImpl::New();
  ProductCardView handle(*impl);
  impl->Initialize();
  return handle;
}

ProductCardView ProductCardView::DownCast(Dali::BaseHandle handle)
{
  return Dali::Ui::View::DownCast<ProductCardView, ProductCardViewImpl>(handle);
}

ProductCardView::ProductCardView() = default;

ProductCardView::ProductCardView(const ProductCardView& view)
: Dali::Ui::View(view)
{
}

ProductCardView::ProductCardView(ProductCardView&& rhs) noexcept
: Dali::Ui::View(std::move(rhs))
{
}

ProductCardView::ProductCardView(ProductCardViewImpl& impl)
: Dali::Ui::View(impl)
{
}

ProductCardView::ProductCardView(Dali::Internal::CustomActor* customActor)
: Dali::Ui::View(customActor)
{
  VerifyCustomActorPointer<ProductCardViewImpl>(customActor);
}

ProductCardView::~ProductCardView() = default;

void ProductCardView::DoSomething()
{
  GetImpl(*this).DoSomething();
}

} // namespace MyNamespace
```

### Impl Header

```cpp
#pragma once

#include <dali-ui-foundation/dali-ui-foundation.h>
#include "product-card-view.h"

namespace MyNamespace
{

class ProductCardViewImpl : public Dali::Ui::ViewImpl
{
public:
  static Dali::IntrusivePtr<ProductCardViewImpl> New();

  ProductCardViewImpl();
  ~ProductCardViewImpl() override = default;

  void DoSomething();

protected:
  void OnInitialize() override;
};

} // namespace MyNamespace
```

### Impl Source

```cpp
#include <dali/devel-api/object/type-registry-helper.h>
#include "product-card-view-impl.h"

namespace MyNamespace
{

namespace
{
Dali::BaseHandle Create()
{
  return ProductCardView::New();
}

DALI_TYPE_REGISTRATION_BEGIN(ProductCardViewImpl, Dali::Ui::ViewImpl, Create)
DALI_TYPE_REGISTRATION_END()
} // unnamed namespace

Dali::IntrusivePtr<ProductCardViewImpl> ProductCardViewImpl::New()
{
  return Dali::IntrusivePtr<ProductCardViewImpl>(new ProductCardViewImpl());
}

ProductCardViewImpl::ProductCardViewImpl()
: Dali::Ui::ViewImpl()
{
  // The handle is not available here. Use OnInitialize() or later.
}

void ProductCardViewImpl::OnInitialize()
{
  Dali::Ui::ViewImpl::OnInitialize();

  ProductCardView handle = ProductCardView::DownCast(Self());
  handle.SetRequestedWidth(200_spx);
  handle.SetRequestedHeight(200_spx);
}

void ProductCardViewImpl::DoSomething()
{
  ProductCardView handle = ProductCardView::DownCast(Self());
  handle.SetBackgroundColor(Dali::Ui::UiColor(0x00FFFF));
}

} // namespace MyNamespace
```

## Required Pattern

The handle class inherits from `Dali::Ui::View`.

```cpp
class ProductCardView : public Dali::Ui::View
{
public:
  static ProductCardView New();
  static ProductCardView DownCast(Dali::BaseHandle handle);

  ~ProductCardView(); // non-virtual
};
```

The impl class inherits from `Dali::Ui::ViewImpl`.

```cpp
class ProductCardViewImpl : public Dali::Ui::ViewImpl
{
public:
  static Dali::IntrusivePtr<ProductCardViewImpl> New();

protected:
  ~ProductCardViewImpl() override;
};
```

## Fluent API Macro

When a new handle class derives from `View`, include `DALI_UI_CHAIN_VIEW_METHODS(Handle)` in the handle class.

This macro wraps inherited `View` setters so they return the new handle type. Without it, chaining can break after an inherited setter:

```cpp
ProductCardView card = ProductCardView::New()
  .SetBackgroundColor(Dali::Ui::UiColor::PRIMARY); // returns ProductCardView& via macro
```

This skill does not require new methods in the derived class to follow a specific setter or chaining style. Define action methods, setters, and getters according to the user's requested API.

## Inheritance Boundary

This skill is for direct inheritance from `Dali::Ui::View`.

`ViewImpl` is a special exception because its impl class is exposed. Other classes such as `Label` and `ImageView` do not generally expose public impl classes for app developers, so external inheritance from those classes is not supported through the normal public API.

If the user asks to inherit from `Label`, `ImageView`, or another View-derived class, explain this boundary and recommend data attachment instead. If a separate data-attachment skill is available, use that skill for the task.

## Handle to Impl

In the handle source file, provide `GetImpl()` helpers.

```cpp
inline ProductCardViewImpl& GetImpl(ProductCardView& view)
{
  DALI_ASSERT_ALWAYS(view);
  Dali::RefObject& handle = view.GetImplementation();
  return static_cast<ProductCardViewImpl&>(handle);
}
```

Use `View::DownCast<Handle, Impl>()` for DownCast.

```cpp
ProductCardView ProductCardView::DownCast(Dali::BaseHandle handle)
{
  return Dali::Ui::View::DownCast<ProductCardView, ProductCardViewImpl>(handle);
}
```

## Creation

Follow the `New()` flow:

1. Create impl with `Impl::New()`.
2. Create handle from impl.
3. Call `impl->Initialize()`.
4. Return handle.

## Type Registration

Register the new type in the impl source file or the source file used by the local pattern.

```cpp
namespace
{
Dali::BaseHandle Create()
{
  return ProductCardView::New();
}

DALI_TYPE_REGISTRATION_BEGIN(ProductCardViewImpl, Dali::Ui::ViewImpl, Create)
DALI_TYPE_REGISTRATION_END()
}
```

Keep type registration in the impl source file unless the local project has an explicit alternative pattern.

## Self()

Inside impl code, call `Self()` when a DALi handle is needed, then downcast if required.

```cpp
Dali::Ui::View view = Dali::Ui::View::DownCast(Self());
```
