# dali-ui Rules

This directory documents development rules for `dali-ui`.

The rules are written for two audiences:

- Humans reviewing and changing `dali-ui`.
- AI tools that may later validate changes through skills, scripts, or MCP servers.

Rules should be precise enough to turn into checks, but still explain the intent
behind the convention. Prefer examples from the current codebase over abstract
style advice.

## Rule Format

Use this structure when adding or updating a rule:

```md
## Rule: Short Rule Name

- Status: required | recommended | contextual
- Scope: public-api | extension-api | integration-api | internal | docs | tests
- Applies To: Concrete code or document pattern

### Intent
Why this rule exists.

### Preferred
Examples of the pattern to use.

### Avoid
Examples of patterns to avoid.

### Exceptions
Known cases where the rule should not apply.

### Validation
Searches, scripts, or manual checks that can detect likely violations.
```

## Status Levels

- `required`: Follow unless there is an explicit design decision to do otherwise.
- `recommended`: Prefer this pattern, but local context may justify another shape.
- `contextual`: Apply only when the described context is present.

## Current Rule Areas

- [API Naming](api-naming.md)
- [Public API and ABI](public-api-abi.md)
- [Handle-Body Pattern](handle-body-pattern.md)
- [Component Boundaries](component-boundaries.md)
- [Docs and Wiki](docs-and-wiki.md)
- [Validation Checks](validation-checks.md)

## Maintenance Notes

When a rule is created from a real code review or refactoring, include the
motivating pattern and the exceptions discovered during the change. This keeps
the rules useful for future automation instead of turning them into generic
style notes.
