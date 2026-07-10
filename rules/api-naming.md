# API Naming Rules

## Rule: Boolean Enabled API Naming

- Status: required
- Scope: public-api, extension-api, integration-api
- Applies To: C++ methods that expose boolean option properties

### Intent

Boolean option APIs should read as property setters and predicates. The dominant
pattern in `dali-ui` is:

```cpp
void SetXxxEnabled(bool enabled);
bool IsXxxEnabled() const;
```

This makes call sites easy to scan and keeps public, extension, and integration
API names consistent.

### Preferred

Use `Set*Enabled(bool)` for setters and `Is*Enabled() const` for getters:

```cpp
void SetSelectionEnabled(bool enabled);
bool IsSelectionEnabled() const;

void SetFrameCacheEnabled(bool enabled);
bool IsFrameCacheEnabled() const;

void SetClearFocusOnEscapeEnabled(bool enabled);
bool IsClearFocusOnEscapeEnabled() const;
```

For option names that are already meaningful predicates without `Enabled`, a
matching `Set*` / `Is*` pair is also acceptable:

```cpp
void SetVisible(bool visible);
bool IsVisible() const;

void SetSelected(bool selected);
bool IsSelected() const;
```

### Avoid

Avoid boolean property setters or getters that use these shapes:

```cpp
void EnableXxx(bool enabled);
void SetEnableXxx(bool enabled);
bool GetXxxEnabled() const;
bool IsEnableXxx() const;
```

These forms are harder to read at call sites, especially when disabling:

```cpp
// Avoid: reads awkwardly.
view.EnableToggleByClick(false);

// Preferred.
view.SetToggleByClickEnabled(false);
```

### Getter and Setter Pairs

Keep the noun phrase stable between setter and getter:

```cpp
void SetLegendToggleEnabled(bool enabled);
bool IsLegendToggleEnabled() const;
```

Avoid pairing a setter with a differently shaped getter:

```cpp
// Avoid.
void SetLegendToggleEnabled(bool enabled);
bool GetLegendToggleEnabled() const;
```

### Private Fields

When a private field directly stores the same option, prefer the same phrase:

```cpp
bool mFrameCacheEnabled;
bool mAspectFitEnabled;
bool mBrokenImageEnabled : 1;
```

Do not rename unrelated runtime state just to match an API. For example, a
cached decision such as `mShouldClearFocusOnEscape` can keep its behavior-based
name even when the configuration API is named `SetClearFocusOnEscapeEnabled()`.

### Exceptions

Do not apply this rule mechanically to these cases:

- Serialized property keys, JSON keys, builder keys, or style keys. Keep them
  stable unless the user explicitly requests a property format migration.
- Calls into external DALi, adaptor, or platform APIs that still expose older
  names.
- Imperative commands that are not properties.
- Existing methods whose names intentionally match external standards.

Example: `VectorAnimationRenderer::SetEnableAspectFit()` is an adaptor API.
`dali-ui` wrappers may use `SetAspectFitEnabled()`, but calls into the adaptor
must keep the adaptor method name.

### Documentation Updates

When renaming public boolean APIs, update:

- Public, extension, integration, and internal forwarding declarations.
- Tests and samples.
- Wiki pages and manual-test documents.
- PR description public API tables.

Mention whether serialized property keys are intentionally unchanged.

## Rule: Natural Boolean Phrase Order

- Status: recommended
- Scope: public-api, extension-api, integration-api
- Applies To: New or renamed boolean APIs

### Intent

Boolean API names should read like a natural English phrase.

### Preferred

Prefer the action phrase when it is clearer:

```cpp
void SetClearFocusOnEscapeEnabled(bool enabled);
bool IsClearFocusOnEscapeEnabled() const;
```

This reads better than:

```cpp
void SetFocusClearOnEscapeEnabled(bool enabled);
bool IsFocusClearOnEscapeEnabled() const;
```

### Validation

This rule usually needs human review. Automated checks can identify old names,
but they cannot reliably judge phrase quality.
