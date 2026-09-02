---
title: Code Generation
phase: construction
version: 1.0.0
owner: SR-AX-LAB
outputs:
  - ai-sdlc-docs/construction/plans/{unit-name}-code-generation-plan.md
  - ai-sdlc-docs/construction/code/{unit-name}-code-generation-summary.md
  - ai-sdlc-docs/state/state-log.md
  - ai-sdlc-docs/audit/audit.md
triggers:
  - code-generation
allowed-tools:
  - Read
  - Write
  - Edit
  - Bash
---

# Code Generation

## Description

Generate or modify implementation code according to the approved implementation design. This activity identifies unit test targets, framework, and commands, while unit test code generation and execution are performed during `code-verification`.

---

## Reference Rules

- `common/ai-execution-rule.md` — subagent parallelization for multi-unit code generation
- `logging/state-management.md` — state-log update rules
- `logging/audit-management.md` — audit update rules
- `logging/traceability-management.md` — traceability update rules

---

## Outputs

| Artifact | Template | Description |
|----------|----------|---------|
| `ai-sdlc-docs/construction/plans/{unit-name}-code-generation-plan.md` | `templates/07-code-generation-plan.md` | Per-unit code generation plan |
| `ai-sdlc-docs/construction/code/{unit-name}-code-generation-summary.md` | (standard) | Code generation summary |
| `ai-sdlc-docs/state/state-log.md` | (standard) | Record activity completion |
| `ai-sdlc-docs/audit/audit.md` | (standard) | Record decisions |

---

## Execution Steps

### Step 1: Analysis Init Context

- Identify unit dependencies, interfaces, contracts, and acceptance criteria
- Validate that the unit is ready for code generation
- Identify the repository stack, package manager, runtime, and available unit test framework before planning code generation

### Step 2: Create Detailed Unit Code Generation Plan

- Read workspace root and project type
- Determine implementation code location
- Document exact paths
- Create explicit steps for unit code generation
- Create or update the complete plan as `ai-sdlc-docs/construction/plans/{unit-name}-code-generation-plan.md`

Context must include: dependencies on other units/services, expected interfaces and contracts, service boundaries and responsibilities, acceptance criteria, target source files, target unit test files to be created or updated during `code-verification`, unit test framework, and unit test execution command for `code-verification`.

### Step 3: Load Unit Code Generation Plan

- Read the complete plan from `ai-sdlc-docs/construction/plans/{unit-name}-code-generation-plan.md`
- Load the full unit context before generating code
- Confirm the implementation files, test target files, and verification commands from the plan

### Step 4: Code Generation

- Check target directories from the plan
- Generate or modify the planned implementation files
- The generated code must align with the implementation design, satisfy acceptance criteria, follow expected interfaces and contracts, handle errors and edge cases, and preserve backward compatibility where required

### Step 5: Generate Code Generation Summary

Create or update `ai-sdlc-docs/construction/code/{unit-name}-code-generation-summary.md`, including: implemented source files, planned or affected unit test files for `code-verification`, unit test framework identified for `code-verification`, exact unit test command to execute during `code-verification`, implementation notes and scope boundaries, unresolved blockers, residual risks, and final code generation gate (`PASS` or `BLOCKED`).

If unit tests were not executed in this task, state: `Unit test execution is pending code-verification.`

### Step 6: Update Traceability Matrix

Code Generation (C2) populates the Code (file:symbol) column:

- Load `ai-sdlc-docs/traceability/traceability-matrix.md`
- For each AC, map to implementation:
  - Which file/module implements this AC?
  - Which functions/classes are involved?
  - Format: `src/module/file.ts:functionName`
- Validation: All file references exist in repository
- Record in audit.md: "traceability-matrix updated: Code column populated for [COUNT] ACs"

### Step 7: Record Results

- Update `ai-sdlc-docs/state/state-log.md` following `logging/state-management.md`
- Update `ai-sdlc-docs/audit/audit.md` following `logging/audit-management.md`

### Step 8: Wait for Explicit Approval

- Do not proceed until the user explicitly approves the generated code and verification plan
- Approval must be clear and unambiguous
- If the user requests changes, update the code generation output and summary

---

## Approval Gate

- [ ] Complete code generation plan created and approved
- [ ] Planned implementation code generated or updated
- [ ] Unit test targets, framework, and execution command identified for `code-verification`
- [ ] Code generation summary created or updated
- [ ] Record approval in `audit.md`
- [ ] Update `state-log.md` with PASS status

---

## SKILL Rules

- Load the Related Common Rules before starting
- Read all activity inputs first
- Verify state-log.md current status
- Record all decisions in audit.md
- Do not generate code before an implementation approach exists
- Do not generate code without loading the approved plan
- Do not claim execution or testing that did not occur
- Do not claim unit test execution unless tests were actually executed
- Leave unit test code generation and execution to `code-verification`; user approval does not replace this
- Preserve existing behavior unless the approved scope changes it
- Keep modifications traceable to the plan
- Do not fabricate information



