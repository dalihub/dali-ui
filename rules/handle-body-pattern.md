# Handle-Body Pattern Rules

## Rule: Public Handles Stay Lightweight

- Status: required
- Scope: public-api, extension-api, integration-api, internal
- Applies To: Public handle classes and implementation classes

### Intent

Public API classes should remain lightweight handles. State and behavior live in
implementation objects.

`extension-api` is not limited to storing the body type for a public handle. It
also contains ABI-stable interfaces, implementation base classes, and helper
APIs that module providers and `dali-ui-components` may depend on. Those
extension contracts must remain usable without depending on `integration-api`
or `internal` headers.

### Preferred

Typical layout:

```text
public-api/foo.h
extension-api/foo-impl.h
internal/foo-impl.cpp
```

or, when the implementation is integration-facing:

```text
public-api/foo.h
integration-api/foo-impl.h
internal/foo-impl.cpp
```

An `extension-api` header may instead define a provider-facing interface,
implementation base, or helper API with no corresponding public handle. Keep
such declarations focused on the stable extension contract and keep private
implementation details under `internal`.

### Avoid

- Adding data members to public handles.
- Adding logic-heavy public API methods that bypass the implementation layer.
- Duplicating state between handle and impl.
- Treating every `extension-api` declaration as if it were only an internal
  public-handle body.
- Exposing internal implementation details through an extension interface,
  implementation base, or helper API.

### Validation

Future validation can inspect public handle headers for data members, check
that setters/getters forward into an implementation class, and review exported
`extension-api` interfaces, implementation bases, and helpers for accidental
dependencies on `integration-api` or `internal` details.
