[한국어](https://github.sec.samsung.net/NUI/dali-ui/wiki/DummyComponent-%28kr%29) · [Components](https://github.sec.samsung.net/NUI/dali-ui/wiki/Components)

# DummyComponent

`Dali::Ui::DummyComponent` is a compatibility type showing the minimal structure of a View-based component. It adds no dedicated behavior, style, or signals. Use `View::New()` for ordinary visual elements or child containers.

## Available API

Use `New()` to create an initialized handle and `DownCast(BaseHandle)` to check the type. A mismatched downcast returns an empty handle. The default constructor creates an empty handle; copying shares the same object rather than constructing a new component. Background, size, and child management are inherited from View.

The umbrella components header does not include this compatibility type. Include `public-api/dummy-component.h` explicitly when maintaining code that uses it.

```cpp
#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-components/public-api/dummy-component.h>

using namespace Dali;
using namespace Dali::Ui;

View AddContainer(View parent)
{
  // Prefer View for a general-purpose container.
  auto container = View::New();
  parent.Add(container);
  return container;
}

DummyComponent AddLegacyComponent(View parent)
{
  auto component = DummyComponent::New();
  parent.Add(component);
  return DummyComponent::DownCast(component);
}
```

The example assumes an initialized application and parent View. DummyComponent itself resolves no dedicated style; applications using other styled components should apply Components::UiConfig before creating those controls.

## Intended use and limits

- Public creation/casting APIs and the header remain for compatibility with existing clients.
- There is no `DummyComponentStyle`, `New(style)`, or dedicated click/selection API.
- Choose InteractiveView for clickable containers or TextButton for text actions.
- No dedicated accessibility role or naming policy is added; do not assume interactive-control behavior.
- When implementing a new component, follow the creation guide rather than treating this structural example as a finished control.

## References

- [Public API](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/dummy-component.h)
- [Implementation](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/internal/dummy-component-impl.cpp)
- [UTC](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/automated-tests/src/dali-ui-components/utc-Dali-DummyComponent.cpp)
- [View](https://github.sec.samsung.net/NUI/dali-ui/wiki/View)
- [Create a component](https://github.sec.samsung.net/NUI/dali-ui/wiki/Create-New-Component)
