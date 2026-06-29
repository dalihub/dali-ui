# Docs and Wiki Rules

## Rule: Update User-Facing References With API Changes

- Status: required
- Scope: docs, wiki, samples, manual-tests, automated-tests
- Applies To: Public API changes

### Intent

Public API changes should leave the repository internally consistent. Tests,
samples, manual tests, and wiki pages are part of the developer experience and
should be updated with the API.

### Preferred

When a public API is renamed or removed, update:

- Automated tests.
- Manual tests.
- Samples.
- Wiki pages.
- PR descriptions.

For PR descriptions, summarize public API changes by class with `Type`, `API`,
and `Purpose` columns.

### Serialized Property Keys

C++ API names and serialized property keys are not the same compatibility
surface. Do not rename property keys only because C++ methods were renamed.

If property keys must change, document the migration impact explicitly.

### Validation

```sh
rg -n 'OldApiName' automated-tests manual-tests samples wiki dali-ui-foundation dali-ui-components
rg -n 'NewApiName' automated-tests manual-tests samples wiki dali-ui-foundation dali-ui-components
```
