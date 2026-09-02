---
title: Code Verification
phase: construction
version: 1.0.0
owner: SR-AX-LAB
outputs:
  - ai-sdlc-docs/construction/code/{unit-name}-code-verification-summary.md
  - ai-sdlc-docs/state/state-log.md
  - ai-sdlc-docs/audit/audit.md
triggers:
  - code-verification
allowed-tools:
  - Read
  - Write
  - Edit
  - Bash
---

# Code Verification

## Description

Generate or update unit test code, execute unit tests, and verify alignment of the generated implementation code with the implementation design before claiming unit completion. This is the unit-level quality gate: a unit reaches `code_verified` only when required unit tests are executable, executed, and PASS, and the self-review checklist is complete or explicitly blocked.

---

## Reference Rules

- `common/ai-execution-rule.md` — subagent parallelization for multi-unit code generation and verification
- `logging/state-management.md` — how to update `state-log.md`
- `logging/audit-management.md` — how to update `audit.md`
- `logging/traceability-management.md` — how to update the traceability matrix

---

## Outputs

| Artifact | Template | Description |
|----------|----------|---------|
| `ai-sdlc-docs/construction/code/{unit-name}-code-verification-summary.md` | `templates/` | Primary output artifact |
| `ai-sdlc-docs/state/state-log.md` | (standard) | Record activity completion |
| `ai-sdlc-docs/audit/audit.md` | (standard) | Record decisions |

---

## Execution Steps

### Step 1: Load Code Generation Plan

- Read the complete plan from `ai-sdlc-docs/construction/plans/{unit-name}-code-generation-plan.md`
- Read the code generation summary from `ai-sdlc-docs/construction/code/{unit-name}-code-generation-summary.md`
- Load the full unit context before verifying code
- Confirm implementation files, planned unit test files, unit test framework, and test commands from the plan

### Step 2: Unit Test Code Generation

- Check target test directories from the plan
- Generate or modify unit test code
- The generated or updated unit tests must cover normal cases, error cases, and boundary cases, verify acceptance criteria, and mock or isolate external dependencies where appropriate

### Step 3: Execute Unit Tests

- Execute all unit test commands identified in the approved plan
- If dependencies are missing, attempt dependency installation or document the exact blocker
- If unit tests fail: analyze the failure, update the implementation or tests within the approved scope, rerun the affected unit tests, and repeat until tests pass or a concrete blocker is reached
- If implementation code is changed during this step, update `code-generation-summary.md` with the material change

### Step 4: Self Code Review

Perform a structured self-review after unit test execution (stricter than a superficial check, but not a full architecture review):

- Code Quality: naming conventions followed; no code duplication (DRY); functions not too long (< 50 lines recommended); no magic numbers/strings; proper (non-generic) error messages
- Design Compliance: all designed functions implemented; all data structures match design; all error cases from design handled; algorithm implementation matches pseudocode
- Security: no hardcoded credentials; input validation present; no SQL injection risks; no XSS risks

If any check fails, update implementation or tests within the approved scope, rerun affected unit tests before proceeding, and document the change and re-verification in `code-verification-summary.md`.

### Step 5: Code Verification Check

Verify the generated code with the checklist: planned source files changed as expected; planned unit test files created or updated; acceptance criteria covered by unit tests; unit test command executed; unit test result recorded; self code review completed; no unexplained scope creep; error handling updated where needed; regression-sensitive paths identified; unresolved items explicitly listed; unit verification gate is `PASS` or `BLOCKED`.

### Step 6: Update Code Verification Summary

Create or update `ai-sdlc-docs/construction/code/{unit-name}-code-verification-summary.md`, including: implemented source files reviewed; created or updated unit test files; unit test framework; exact unit test command executed; unit test execution result; pass/fail status; failure details (if any); fixes applied after failed tests; self code review result; unresolved blockers; residual risks; final unit verification gate `PASS` or `BLOCKED`.

If no unit test was executed, the summary must explicitly state:

```text
No automated unit test executed.
Unit verification gate: BLOCKED.
```

### Step 7: Update Traceability Matrix

Code Verification (C3) populates the Test column with unit test evidence:

- Load `ai-sdlc-docs/traceability/traceability-matrix.md`
- For each AC, record verification method and result:
  - Test name + evidence label from glossary.md: `executed-pass`, `executed-fail`, `code-review-only`, `manual-pass`, `not-applicable`, etc.
  - Format: `test_name (executed-pass)` or `code-review-only [reason]`
- Validation: All test references exist and evidence is recorded
- Record in audit.md: "traceability-matrix updated: Test column populated with unit test evidence for [COUNT] ACs"

### Step 8: Record Results

- Update `ai-sdlc-docs/state/state-log.md` following `logging/state-management.md`
- Update `ai-sdlc-docs/audit/audit.md` following `logging/audit-management.md`

### Step 9: Wait for Explicit Approval

- Do not proceed until the user explicitly approves the generated code and unit verification result
- Approval must be clear and unambiguous
- If the user requests changes, update the code or tests and repeat unit verification

Standard completion prompt — choose one:
1. Request changes to the generated solution
2. Continue to build and testing

---

## Approval Gate

- [ ] Review the code verification summary artifact
- [ ] Verify completeness and accuracy
- [ ] Record approval in `audit.md`
- [ ] Update `state-log.md` with PASS status

---

## SKILL Rules

### Preconditions
- Load the related common rules before starting
- Read all activity inputs first
- Verify current status in `state-log.md`
- Do not begin until `construction/code-generation` is complete for the target unit and the summary exists at `ai-sdlc-docs/construction/code/{unit-name}-code-generation-summary.md`; if missing or the gate is not recorded, this task is `BLOCKED`

### Must Do
- Generate or update unit tests before claiming unit-level verification; if unit tests are missing, create appropriate unit tests
- Execute all required unit test commands, or mark the unit as `BLOCKED` with the exact blocker
- Preserve existing behavior unless the approved scope changes it
- Keep modifications traceable to the plan
- Record all decisions in `audit.md`
- Ensure the unit verification gate is explicitly `PASS` or `BLOCKED` (never left blank or ambiguous)

### Must Not Do
- Do not fabricate information
- Do not claim execution or testing that did not occur
- Do not mark code verification complete based only on generated code, generated test code, static review, manual inspection, unexecuted commands, or instructions for the user to run tests later
- Do not mark a unit `implemented`, `unit-tested-pass`, `code_verified`, or `complete` unless automated unit tests were executed and passed; if not executed, status must be `BLOCKED`
- User approval cannot override failed or missing unit test evidence; if unit tests are not executed or not passed, the unit remains `BLOCKED`
- Lack of existing unit tests is not a valid reason to skip unit testing



