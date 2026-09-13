# OneUIComponents Reference Rules

## Rule: Resolve the Component Reference Before Context Analysis

- Status: required
- Scope: `dali_component_feature` workflow profile only
- Applies To: Component feature workflows before A0 Context Analysis

### Intent

OneUIComponents is a required behavioral and UX reference for new DALi
component development. The workflow must not assume that a checkout already
exists on the developer's machine, and it must not proceed using guessed
behavior when the reference cannot be inspected.

### Reference

- SSH repository: `git@github.sec.samsung.net:NUI/OneUIComponents.git`
- HTTPS repository: `https://github.sec.samsung.net/NUI/OneUIComponents.git`
- Archive fallback: `https://github.sec.samsung.net/NUI/OneUIComponents/archive/refs/heads/devel.zip`
- Required branch: `devel`

### Preferred

After selecting `dali_component_feature` and before starting A0:

1. Explain why the reference source is required and present the repository and
   branch.
2. Ask the user to choose one of these acquisition modes:
   - Provide an existing local checkout path.
   - Approve an AI-managed clone into a dedicated reference directory.
   - Download or clone the source manually and then provide its path.
3. Treat a user-provided checkout as read-only. Do not fetch, pull, switch
   branches, reset, or otherwise modify it without explicit approval.
4. For an approved AI-managed clone, prefer
   `../.references/OneUIComponents` relative to the `dali-ui` repository unless
   the user chooses another location.
5. Validate the repository identity, selected branch, and readable source.
6. Record the exact referenced commit in the workflow artifacts.

Use OneUIComponents as the source for behavior, state, properties, events,
accessibility expectations, tests, and samples. DALi C++ architecture, API/ABI,
Handle-Body, and component-boundary rules remain authoritative for the new
implementation.

### Avoid

- Assuming a machine-specific checkout path.
- Silently cloning or updating a repository without user approval.
- Mutating a user-owned checkout as part of reference inspection.
- Mechanically copying C# implementation structure into the DALi C++ API and
  implementation hierarchy.
- Starting A0 without readable reference source.

### Exceptions

If the requested component has no corresponding OneUIComponents implementation,
record the search evidence and obtain explicit approval to proceed from other
requirements and references. If repository access is unavailable, keep the
workflow `BLOCKED` until the user supplies an accessible source or approves an
alternative.

### Validation

- The active profile is `dali_component_feature`.
- A readable reference source is available before A0.
- The requirements and architecture artifacts identify the corresponding
  OneUI component or document the approved absence.
- The reference branch and commit are recorded.
- Intentional differences from the reference are documented.
