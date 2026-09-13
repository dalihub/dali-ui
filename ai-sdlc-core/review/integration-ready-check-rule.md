---
title: Integration Ready Check

loop: construction
version: 1.0.0
owner: SR-AX-LAB
criticality: mandatory
enforcement_point: build-testing-entry-gate
outputs:
  - ai-sdlc-docs/state/state-log.md [MUST record verdict]
  - ai-sdlc-docs/audit/audit.md [MUST record decision]
mandatory-trigger: true
trigger-point: After code-verification completes, before build-testing begins
skip-allowed: false
enforcement: ai-sdlc.md Step 3 must pause and execute this rule. No exceptions.
depends_on:
  - {org-dir}/config/review/integration-ready-checklist.md
  - ai-sdlc-docs/state/state-log.md
trigger_condition: |
  MANDATORY execution point in ai-sdlc.md Step 3.
  Triggered after code-verification activity completes.
  Gates entry to build-testing activity.
  CANNOT be skipped.
allowed-exceptions: None
auto-halt-conditions:
  - integration-ready-checklist.md missing
  - A source required by the checklist's active operating-mode mapping is missing
  - Any Required condition violated for any unit (→ BLOCKED)
  - Any Conditional condition triggered but violated (→ BLOCKED)
  - Evidence contradiction detected (→ FAIL)
---

# Integration Ready Check Rule

## Purpose
Evaluate whether all required units are ready for integration and build-testing. This gate must pass before `build-testing` begins.

## Trigger
When user attempts to start the `build-testing` activity.

## Load Checklist

Load `{org-dir}/config/review/integration-ready-checklist.md`.

Where `{org-dir}` is the discovered organization directory (see path-finder.md for dynamic resolution).

This file defines **WHAT** to evaluate (conditions, expected values, enforcement
modes) and is customizable by business unit. This rule defines **HOW & WHEN** the
evaluation runs. If the checklist is missing → **HALT** (verdict = BLOCKED).

## Evaluation Procedure

Read `operating_mode` from `state-log.md`, then resolve the required-unit roster
and condition sources from the checklist's mapping for that mode. The core rule
does not assume that A5 or team-status artifacts exist in every workflow.

For each required unit:
1. For each condition in the checklist:
   - Select the source declared for the active operating mode
   - Read the condition's source field from that live tracking file
   - Apply the operator against the expected value
   - Mark ✓ (holds) / ✗ (violated) / ⊘ (N/A — only if a Conditional trigger is false)
2. Evaluate project-level conditions (e.g. Integration Owner) once
3. Advisory (WARN) conditions are logged but do not block

If the active mode has no source mapping, or a mapped required source/field is
missing, record the affected condition as ✗ and return `BLOCKED`. Do not borrow
a source from another operating mode or infer a PASS from an unrelated artifact.

## Verdict (calculated from checklist results, not hardcoded)

**PASS** when:
- Every **Required** condition holds (✓) for all required units
- Every **Conditional** condition holds or is ⊘ N/A with justification

**BLOCKED** when:
- Any **Required** condition is ✗ for any unit
- OR a **Conditional** condition is triggered but violated

**FAIL** when:
- Evidence contradicts itself (e.g. state = `code_verified` but test_result = FAIL)
- OR user explicitly rejects integration readiness

## State Management (MUST record)

Update `ai-sdlc-docs/state/state-log.md`:
- Field: `integration_ready_check_verdict`
- Value: [PASS / BLOCKED / FAIL]
- Timestamp: ISO 8601 format

## Audit Management (MUST record)

Create entry in `ai-sdlc-docs/audit/audit.md`:

```markdown
### INT-READY-[001]

| Field | Value |
|-------|-------|
| Timestamp | [ISO 8601] |
| Verdict | [PASS / BLOCKED / FAIL] |
| Units Evaluated | [count] |
| Ready Units | [count] |
| Blocked Units (if any) | [unit-id: violated condition # and name] |
| Violated Conditions (if any) | [checklist item # → expected vs actual] |
| Recovery Actions (if BLOCKED) | [what user must do] |
| Owner | AI Agent (Integration Ready Check) |
| Impact | Proceed to build-testing or halt for recovery |
```

## Team Mode Requirement

When team mode is active:
- Integration Owner (or assigned integration session) MUST start `build-testing`, not individual unit developers
- Integration Owner MUST verify all required unit statuses before triggering this check

## Single-Unit Mode

Single-unit projects evaluate exactly one required unit. Resolve its roster and
condition fields from the checklist's `single_developer` source mapping. Do not
require A5 or team-status artifacts unless that mapping explicitly names them.

## If BLOCKED or FAIL

Load `ai-sdlc-core/common/recovery-paths.md` for recovery actions.
