# ABI Rules

## Public Handle Classes

For public API handle classes:

- Do not add data members.
- Do not add virtual functions.
- Keep destructors non-virtual.
- Do not change existing public method signatures.
- Do not remove or reorder existing methods.
- When adding a new public method, append it after existing methods.
- Apply `DALI_UI_API` to exported public classes and symbols following local patterns.
- Store all state in the impl class.

## Impl Classes

For `ViewImpl`-derived classes:

- Put data members and logic in the impl.
- Prefer extending non-virtual API unless virtual dispatch is required.
- Avoid changing ABI-frozen virtual API in `ViewImpl`.
- Use `DALI_INTERNAL` for constructors not intended for app developers when local patterns require it.

## API Levels

- `public-api`: app developer API. Keep ABI stable.
- `integration-api`: framework/extension developer API. Recompile is allowed, but avoid unnecessary churn.
- `internal`: implementation details only.

Before editing, inspect similar existing files and preserve their API level boundaries.
