# Scroll and Recycler Implementation Namespace Boundary

## Goal

Align the namespaces of the scroll and recycler implementation classes with
their API layer so that their C++ names communicate the same boundary as their
header locations.

## Namespace placement

| Class | Header layer | Namespace |
| --- | --- | --- |
| `EdgeEffectImpl` | `integration-api` | `Dali::Ui::Integration` |
| `ItemsLayouterImpl` | `integration-api` | `Dali::Ui::Integration` |
| `BounceEdgeEffectImpl` | `internal` | `Dali::Ui::Internal` |
| `LinearItemsLayouterImpl` | `internal` | `Dali::Ui::Internal` |

The internal concrete classes derive from their corresponding integration
contract. No implementation class remains in the `Dali::Ui` public-handle
namespace.

## Handle boundary

Public handle headers retain only nested namespace forward declarations:

- `namespace Integration { class EdgeEffectImpl; }`
- `namespace Integration { class ItemsLayouterImpl; }`
- `namespace Internal { class BounceEdgeEffectImpl; }`

Their hidden constructors and `GetImpl()` methods use fully qualified types.
Implementation source files use the matching nested namespace and update
downcasts and intrusive-pointer aliases accordingly.

## Compatibility

This intentionally changes source and binary names for integration extension
implementations. External subclasses must include the integration header and
derive from `Dali::Ui::Integration::EdgeEffectImpl` or
`Dali::Ui::Integration::ItemsLayouterImpl`, then be rebuilt.

Application-facing handle methods remain unchanged.

## Verification

- Extend the CMake boundary check to reject the four legacy
  `Dali::Ui::*Impl` declarations in their relocated headers.
- Configure from `build/tizen` with the requested install prefix.
- Run `ninja install` and confirm the boundary check runs as a dependency and
  the installation completes.
