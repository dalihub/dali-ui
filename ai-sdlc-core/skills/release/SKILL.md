---
title: Release Dependency Removal
phase: construction
version: 1.0.0
owner: SR-AX-LAB
outputs:
  - ai-sdlc-docs/construction/release/release-note.md
  - ai-sdlc-docs/traceability/traceability-matrix.md
  - ai-sdlc-docs/state/state-log.md
  - ai-sdlc-docs/audit/audit.md
triggers:
  - release
allowed-tools:
  - Read
  - Write
  - Edit
---

# Release Dependency Removal

## Description

Prepare and validate a release handoff for changes that remove dependencies, while keeping the existing workflow behavior intact and recording the release evidence needed for completion.

---

## Reference Rules

- `logging/state-management.md` — update release state records
- `logging/audit-management.md` — update release audit records
- `logging/traceability-management.md` — update release traceability records

---

## Outputs

| Artifact | Template | Description |
|----------|----------|---------|
| `ai-sdlc-docs/construction/release/release-note.md` | `templates/10-release-note.md` | Primary release note with scope, validation, rollback, and readiness |
| `ai-sdlc-docs/traceability/traceability-matrix.md` | `ai-sdlc-docs/traceability/traceability-matrix.md` | REQ → AC → architecture → code → test mapping |
| `ai-sdlc-docs/state/state-log.md` | (standard) | Workflow status, readiness verdict, and review result |
| `ai-sdlc-docs/audit/audit.md` | (standard) | Decisions, approvals, overrides, and blockers |

---

## Execution Steps

### Step 1: Create Release Note

Create or update `ai-sdlc-docs/construction/release/release-note.md` with testing evidence, unresolved risks, deployment scope, prerequisites, rollback steps, validation steps, operator-facing release notes, and an explicit readiness verdict.

### Step 2: Validate the Release Package

Package the release note with the required release artifacts, run the available validation checks, and record the validation method and outcome.

### Step 3: Finalize Traceability Matrix

Finalize `ai-sdlc-docs/traceability/traceability-matrix.md` to complete all mandatory sections:

**Mandatory Sections (from templates/19-traceability-matrix.md Section 1-3):**
- REQ → AC → Architecture → Code → Test table fully populated
- Decision Impact section: Extract DEC-### entries from audit.md that materially shaped implementation
- Coverage Summary: Calculate metrics (REQ Coverage %, AC Coverage %, Code Coverage %, Test Coverage %)

**Optional Sections (add if applicable):**
- Unit Coverage (if multi-unit mode)
- Architecture Component Mapping (if helpful for understanding)
- Prior REQ Traceability (if loop re-entry continuation)
- DEFECT_FIX Addendum (if work_type = DEFECT_FIX)

**Validation:** Execute checklist from templates/19-traceability-matrix.md Section 7 before marking complete.

**Record in audit.md:** "traceability-matrix finalized: All mandatory sections complete. [Coverage metrics summary]"

### Step 4: Record Release Actions

Update `ai-sdlc-docs/state/state-log.md` and `ai-sdlc-docs/audit/audit.md` according to `logging/state-management.md` and `logging/audit-management.md`.

### Step 5: Wait for Explicit Approval

Do not proceed past validation checks until the user explicitly approves the release package.

### Step 6: Finalize the State

After approval, mark the workflow as `released` or `closed`, keep readiness as `ready`, `ready with conditions`, or `not ready`, and ensure the Construction Review is `PASS` before completing.

---

## Approval Gate

- [ ] Review the output artifact
- [ ] Verify completeness and accuracy
- [ ] Record approval in `audit.md`
- [ ] Update `state-log.md` with `PASS` status
- [ ] `traceability-matrix.md` updated

---

## SKILL Rules

- Load the related common rules before starting.
- Read all activity inputs first.
- Verify the current `state-log.md` status before making changes.
- Record every decision in `audit.md`.
- Preserve traceability from requirements to tests.
- Keep the release note, traceability matrix, state log, and audit log consistent with one another.
- Use only the information supported by the source materials and execution evidence.
- Do not fabricate requirements, test results, approvals, or readiness.
- Do not remove mandatory steps, bypass approval, disable audit, disable traceability, or weaken mandatory gates.
- Do not skip validation unless the user explicitly opts out, and record that opt-out in `audit.md`.
- Do not mark the workflow complete unless Construction Review is `PASS`.
- Do not leave readiness unspecified; it must be `ready`, `ready with conditions`, or `not ready`.
- Keep the existing dependency-removal behavior intact while trimming only content that does not map to this template.
- Delete unmapped or non-applicable content rather than forcing it into the skill.




