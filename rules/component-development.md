# Component Development Rules

These rules extract the normative component-development guidance from
[`wiki/Create-New-Component.md`](../wiki/Create-New-Component.md). The wiki is
the explanatory developer guide; this file is the workflow and review contract.

## Rule: Match Public and Implementation Base Classes

- Status: required
- Scope: dali-ui-components
- Applies To: New public component handles and their internal implementations

### Intent

A component must inherit the foundation behavior that it actually needs, and
its public handle and internal implementation hierarchies must remain aligned.

### Preferred

Choose the matching pair:

| Component behavior | Public base | Internal implementation base |
|---|---|---|
| Basic visual or container | `View` | `ViewImpl` |
| Clickable or focusable interaction | `InteractiveView` | `Extension::InteractiveViewImpl` |
| Selectable state | `SelectableView` | `Extension::SelectableViewImpl` |
| Single-selection group member | `GroupSelectableView` | `Extension::GroupSelectableViewImpl` |

Keep component-specific state and behavior in the implementation class. Apply
the [Handle-Body Pattern](handle-body-pattern.md) and
[Component Boundaries](component-boundaries.md) rules as part of the same
design decision.

### Avoid

- Choosing a more capable base class without a behavioral requirement.
- Pairing a public base with an unrelated implementation base.
- Adding component state to the public handle.
- Reimplementing click, focus, selection, or group-selection behavior already
  provided by the selected foundation base.

### Exceptions

A different hierarchy requires an explicit architecture decision explaining
why the standard base classes cannot express the component behavior.

### Validation

Review the public and implementation class declarations together. Confirm that
the selected pair matches the component's required interaction and selection
semantics.

## Rule: Use the Immutable Component Style Pattern

- Status: required
- Scope: public-api, extension-api, integration-api, internal
- Applies To: New component style types and default component styling

### Intent

Component visual and layout defaults must be reusable, type-safe, configurable,
and independent from mutable component state.

### Preferred

- Define a component style as an immutable `UiStyle` handle.
- Put style files under the `styles/` directory of their corresponding API
  level.
- Provide `DefaultKey()`, `DefaultPreset()`, `Default()`, `DownCast()`,
  `StaticDownCast()`, and `Configure()` with the semantics documented in the
  source wiki.
- Allocate the typed default key once in static storage.
- Cache the built-in default preset instead of constructing it on every call.
- Access initialized styles only after `UiConfig::Apply()`.
- Resolve `Default()` from the current style sheet and fall back to
  `DefaultPreset()`.

### Avoid

- Returning a newly allocated built-in preset on every `DefaultPreset()` call.
- Repeating a runtime `dynamic_cast` after `UiStyleSheet` has already verified
  the concrete type.
- Creating or resolving initialized style objects before `UiConfig::Apply()`.
- Placing style declarations outside the API level's `styles/` directory.

### Exceptions

A component that has no visual or layout defaults may omit a component-specific
style type. Record that decision in the component architecture.

### Validation

Compare the style API and implementation with the patterns in
`wiki/Create-New-Component.md`. Verify key allocation, preset caching, config
application preconditions, and style-file locations.

## Rule: Builders Produce Complete Immutable Styles

- Status: required
- Scope: component styles
- Applies To: Nested component style builders

### Intent

The builder is a mutable construction helper, while the built style remains an
immutable handle with a complete set of values.

### Preferred

- Construct `Builder` directly; do not add `Builder::New()`.
- Start a default builder from built-in defaults.
- Return a complete immutable style from `Build()`.
- Use `Configure()` to clone an existing style for partial modification.
- Expose builder setters for configuration.

### Avoid

- Builder getter APIs.
- A `Build()` result with unspecified required values.
- Mutating a built style instead of configuring a builder.

### Exceptions

None for component styles using this pattern.

### Validation

Inspect the builder API for constructor-based creation, setter-only mutation,
complete defaults, and an immutable `Build()` result.

## Rule: Apply Style at Component Construction

- Status: required
- Scope: dali-ui-components
- Applies To: Component factories, initial style application, and runtime setters

### Intent

Initial styling must be type-safe and deterministic without introducing a
generic styling hook into the foundation implementation hierarchy.

### Preferred

- Provide typed component factory overloads for styled construction where
  needed.
- Apply the concrete style through a typed private implementation helper after
  the component implementation is initialized.
- Let runtime property setters update actual component state directly.
- Distinguish style-only visual defaults from frequently changed content
  properties when designing public setters.

### Avoid

- Adding an untyped `UiStyle` initialization hook to `ViewImpl`.
- Adding runtime `SetStyle()` unless the component tracks whether each affected
  property was changed by the user after construction.
- Reapplying a style in a way that silently overwrites user-set runtime values.

### Exceptions

A runtime style API is allowed only with an approved policy and implementation
for preserving or overriding user-set properties.

### Validation

Trace default and explicitly styled factory paths into the implementation.
Verify initialization order, concrete style typing, and user-property
preservation behavior.

## Rule: Configure Component Defaults Through Components UiConfig

- Status: required
- Scope: dali-ui-components, applications, style providers
- Applies To: Component style-sheet registration and application

### Intent

Components must use one coherent configuration and style-sheet lifecycle.

### Preferred

- Applications using components apply `Components::UiConfig` rather than also
  applying a separate foundation `UiConfig`.
- Change required style-sheet entries before calling `UiConfig::Apply()`.
- Use `StyleSheet()` to modify selected entries from the config preset.
- Use `ResetStyleSheet()` or `StyleSheet::New()` only when a provider must
  replace the complete sheet.
- Keep style creators free of calls to the same style's `Default()`.

### Avoid

- Mutating a style sheet after `UiConfig::Apply()` freezes it.
- Applying both component and foundation configs in a component application.
- Calling a style's `Default()` from that style's creator, which causes
  recursion.

### Exceptions

None for applications using `dali-ui-components`.

### Validation

Review config setup order and style creator call paths. Confirm that all style
registrations precede `Apply()` and that creators cannot recursively resolve
their own defaults.

## Rule: Do Not Invent the Stateful Style Contract

- Status: required
- Scope: dali-ui-components
- Applies To: Style values that vary by pressed, disabled, selected, or other component state

### Intent

The source component guide marks the stateful-style contract as TBD. A workflow
must not infer a public API or shared representation that has not been agreed.

### Preferred

- Identify state-dependent visual requirements during requirements analysis.
- Treat a new reusable stateful-style API as an architecture decision requiring
  explicit approval.
- Keep a component-local solution internal when it does not establish a shared
  contract.

### Avoid

- Presenting an invented stateful-style convention as an existing DALi rule.
- Adding a public shared abstraction without architecture review.

### Exceptions

An already approved project design or existing component contract may be
followed when its source and scope are recorded.

### Validation

Search the design and public API changes for state-specific style structures.
Require an approved decision whenever they establish a new shared contract.
