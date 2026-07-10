# Handle-Body Pattern Rules

## Rule: Public Handles Stay Lightweight

- Status: required
- Scope: public-api, extension-api, integration-api, internal
- Applies To: Public handle classes and implementation classes

### Intent

Public API classes should remain lightweight handles. State and behavior live in
implementation objects.

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

### Avoid

- Adding data members to public handles.
- Adding logic-heavy public API methods that bypass the implementation layer.
- Duplicating state between handle and impl.

### Validation

Future validation can inspect public handle headers for data members and check
that setters/getters forward into an implementation class.
