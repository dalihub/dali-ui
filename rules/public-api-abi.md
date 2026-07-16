# Public API and ABI Rules

## Rule: Respect API Level Stability

- Status: required
- Scope: public-api, extension-api, integration-api, internal
- Applies To: C++ API changes

### Intent

`dali-ui` has several API categories with different audiences and stability
expectations:

- `public-api`: app developer surface.
- `extension-api`: ABI-stable extension points and implementation bases used by
  module providers and `dali-ui-components`.
- `integration-api`: API-stable surface for independent extension library
  developers; recompilation is acceptable.
- `internal`: implementation detail.

An API category and a C++ member access level are independent concepts.
`public-api`, `extension-api`, and `integration-api` describe the role of a
header. `public:`, `protected:`, and `private:` describe access to a declaration
inside a class or struct. For example, a `private:` member in an
`extension-api` header is not an extension API declaration merely because of
the header path. Namespace-level declarations do not have a C++ member access
level.

Ordinary feature work should preserve both public and extension ABI. Explicit
pre-release redesigns may intentionally rename, remove, or reshape APIs when
the result is clearer and the API/ABI break is approved and documented.

### Preferred

- Keep real state in impl classes, not public handles.
- For existing `public-api` and `extension-api` classes, preserve class size,
  base classes, and data-member layout.
- Do not add, remove, reorder, or change virtual functions in existing
  `public-api` or `extension-api` classes during ordinary feature work.
- Do not remove or change existing public or protected function signatures in
  `public-api` or `extension-api` during ordinary feature work.
- Add new non-virtual APIs without disturbing the order or ABI of existing
  declarations.
- Apply `DALI_UI_API` to exported symbols.
- Use `DALI_INTERNAL` for constructors or factories not intended for app
  developers.
- Call out public and extension API changes in PR descriptions.

A private member function is not part of the source API contract. A private
non-static data member can still affect class layout and ABI, so layout changes
must be reviewed even though that member is not reported as a public or
extension API declaration.

For `integration-api`, avoid unnecessary API changes during ordinary work, but
ABI breaks are acceptable because dependent extension libraries can be
recompiled. When changing a virtual function, update every known subclass.

### Validation

Validation should inspect exported `public-api` and `extension-api` headers for
class-layout changes, base-class changes, added data members, virtual method
changes, changed public or protected signatures, and missing API export macros.
