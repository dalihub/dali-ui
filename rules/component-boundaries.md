# Component Boundary Rules

## Rule: Components Use Foundation Public and Extension APIs

- Status: required
- Scope: dali-ui-components
- Applies To: Includes and dependencies from components to foundation

### Intent

`dali-ui-components` is built on top of `dali-ui-foundation`. It should depend
only on the foundation APIs intended for component and extension use.

### Preferred

Components may include foundation headers from:

```text
dali-ui-foundation/public-api/
dali-ui-foundation/extension-api/
```

### Avoid

Components should not depend on foundation headers from:

```text
dali-ui-foundation/integration-api/
dali-ui-foundation/internal/
```

### Validation

```sh
rg -n '#include <dali-ui-foundation/(integration-api|internal)/' dali-ui-components
```
