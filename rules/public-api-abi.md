# Public API and ABI Rules

## Rule: Respect API Level Stability

- Status: required
- Scope: public-api, extension-api, integration-api, internal
- Applies To: C++ API changes

### Intent

`dali-ui` has several API levels with different stability expectations:

- `public-api`: app developer surface.
- `extension-api`: ABI-stable extension surface.
- `integration-api`: extension library surface; recompilation is acceptable.
- `internal`: implementation detail.

Ordinary feature work should preserve public ABI. Explicit pre-release
redesigns may intentionally rename, remove, or reshape APIs when the result is
clearer.

### Preferred

- Keep real state in impl classes, not public handles.
- Apply `DALI_UI_API` to exported symbols.
- Use `DALI_INTERNAL` for constructors or factories not intended for app
  developers.
- Call out public API changes in PR descriptions.

### Validation

Future validation can check exported public headers for added data members,
virtual method changes, and missing API export macros.
