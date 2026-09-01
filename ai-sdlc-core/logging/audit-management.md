# Audit Management Rule

## Purpose
Define the audit control policy for AI-SDLC workflows.

Audit management provides traceability for:
- decisions
- scope changes
- artifact lifecycle changes
- recovery actions
- approvals
- validation evidence

All workflow decisions that affect downstream execution MUST be recorded here.

## Template
Use `templates/12-audit.md` as the structural template for `audit.md`.

## Required Audit Artifacts
- `ai-sdlc-docs/audit/audit.md`

## Decision Log Requirements
DRecovery-related DEC-### entries are created according to the Decision Trigger Matrix.

For BLOCKED or FAIL recovery:
Record:
- trigger_source
- workflow_context
- failed_review
- failed_checklist_item
- root_cause
- backtrack_target
- affected_artifacts
- re-execution_scope
- verification_result

## Decision Trigger Matrix

Create a DEC-### entry when any of the following occurs:

| Category | Trigger |
|---|---|
| Workflow | Work type classification changes |
| Artifact | Artifact reuse, regeneration, or retirement decision |
| Drift | Existing artifact no longer matches current state |
| Recovery | BLOCKED/FAIL recovery execution begins |
| Backtracking | Returning to an earlier workflow task |
| Scope | Scope inclusion, exclusion, or deferral changes |
| Architecture | Boundary, ownership, or dependency changes |

## Review & Gate Entry Requirements

Beyond `DEC-###` decision entries, review gates and integration checks create dedicated audit entries. Entry ID formats are defined in `ai-sdlc-org/config/naming-rules.md` > "Audit Entry ID Format":

| Entry ID | Created When | Owner |
|---|---|---|
| `INC-REV-###` | Inception Review verdict recorded | review-rule.md (loop=inception) |
| `CON-REV-###` | Construction Review verdict recorded | review-rule.md (loop=construction) |
| `INT-READY-###` | Integration Ready Check verdict recorded | integration-ready-check-rule.md |
| `USR-TRANS-###` | User transition decision after a gate | review-protocol.md (Phase 5) |

## Change Log Requirements
Record:
- major artifact changes
- scope changes
- approval outcomes
- known issue registration
- release readiness changes

## Artifact Inheritance Decision Requirements

When inheriting artifacts from a previous workflow:
Record:
- artifact name
- lifecycle decision:
  - reuse
  - update
  - regenerate
  - obsolete
- validation result
- reason
- downstream impact

## Recovery Decision Requirements

For BLOCKED or FAIL recovery:
Record:
- trigger_source
- workflow_context
- failed_review
- failed_checklist_item
- root_cause
- backtrack_target
- affected_artifacts
- re-execution_scope
- verification_result

Recovery must always identify the smallest re-execution scope.

## Defect Work Rule
For DEFECT_FIX, explicitly record:
- symptom summary
- root cause status: confirmed, likely, or hypothesis
- recurrence prevention approach
- regression protection added

## Constraint
Do not claim approvals, execution, tests, or release actions that did not occur. Any audit entry that records an action must be backed by observable evidence in the current session. Entries based on inference, assumption, or prior-session memory are invalid.
