# Scroll and Recycler implementation API boundary

## Goal

Remove implementation classes and their state from the installed public API
headers for the scroll and recycler view families, while retaining supported
extension points for custom implementations.

## API placement

| Class | Destination | Reason |
| --- | --- | --- |
| `EdgeEffectImpl` | `integration-api` | External edge-effect implementations derive from this abstract contract. |
| `ItemsLayouterImpl` | `integration-api` | External recycler layout implementations derive from this abstract contract and use `Recycler`. |
| `BounceEdgeEffectImpl` | `internal` | It is DALi's concrete built-in effect and has no extension contract. |
| `LinearItemsLayouterImpl` | `internal` | It is DALi's concrete built-in layouter and has no extension contract. |

## Public header boundary

- Public handles (`EdgeEffect`, `BounceEdgeEffect`, `ItemsLayouter`, and
  `LinearItemsLayouter`) must not include an `*Impl` header.
- Public headers will use forward declarations for implementation types.
- `ItemsLayouter::Orientation` remains a public type, declared independently
  of `ItemsLayouterImpl`, so it does not force an integration header include.
- Constructors and `GetImpl()` accessors that are required by the library stay
  hidden from generated public API documentation, but their implementation
  types are only available through the integration or internal include paths.

## Source migration

- Update all affected implementation source files and public handle source
  files to include their new paths.
- Update comments and custom-extension documentation to tell implementers to
  include the appropriate `integration-api` header explicitly.
- Do not move unrelated scroll/recycler classes.

## Verification

- Add or update a focused compile-level test that verifies public scroll and
  recycler headers compile without including any `*Impl` public header.
- Build the foundation target and run the relevant automated-test target.
- Search installed public-header inputs to ensure the four implementation
  headers no longer reside under `public-api`.

## Compatibility

This is intentionally a source-compatibility change for custom external
`EdgeEffectImpl` and `ItemsLayouterImpl` subclasses: they must include the
corresponding `integration-api` header explicitly. Public application-facing
handle APIs remain unchanged.
