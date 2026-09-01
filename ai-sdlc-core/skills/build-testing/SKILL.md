---
title: Build and Testing
phase: construction
version: 1.0.0
owner: SR-AX-LAB
outputs:
  - ai-sdlc-docs/construction/test/test-plan.md
  - ai-sdlc-docs/construction/test/test-report.md
  - ai-sdlc-docs/construction/test/regression-report.md
  - ai-sdlc-docs/state/state-log.md
  - ai-sdlc-docs/audit/audit.md
  - ai-sdlc-docs/traceability/traceability-matrix.md
triggers:
  - build-testing
allowed-tools:
  - Bash
  - Read
  - Write
  - Edit
---

# Build and Testing

## Description

Build all units and execute a comprehensive, automated-first testing strategy. Defines and executes the validation needed to prove the build is ready for release.

---

## Reference Rules

- `logging/state-management.md` — how to update state-log.md
- `logging/audit-management.md` — how to update audit.md
- `logging/traceability-management.md` — how to update traceability-matrix.md

---

## Outputs

| Artifact | Template | Description |
|----------|----------|---------|
| `ai-sdlc-docs/construction/test/test-plan.md` | `templates/08-test-plan.md` | Automated test plan |
| `ai-sdlc-docs/construction/test/test-report.md` | `templates/09-test-report.md` | Test summary / evidence report |
| `ai-sdlc-docs/construction/test/regression-report.md` | (standard) | Regression report (when applicable) |
| `ai-sdlc-docs/state/state-log.md` | (standard) | Record activity completion and verdict |
| `ai-sdlc-docs/audit/audit.md` | (standard) | Record decisions |
| `ai-sdlc-docs/traceability/traceability-matrix.md` | (standard) | REQ → AC → Architecture → Code → Test mapping (canonical handoff) |

---

## Execution Steps

### Step 1: Generate Automated Build/Test Environment Instructions

Detect the project stack, package manager, test framework, runtime, and required local services from the repository before planning any manual steps. Prefer executable commands, scripts, containers, fixtures, mocks, service emulators, or generated test data that let validation run automatically.

Document the setup steps:
- [ ] Prerequisites
- [ ] Install dependencies
- [ ] Configure environment variables, local config, secrets placeholders, and test data
- [ ] Start required local services, containers, mocks, or emulators
- [ ] Build all units
- [ ] Run automated tests
- [ ] Verify build and test success
- [ ] Troubleshooting / error handling

If the environment cannot be configured automatically: attempt the closest executable validation command first, record the exact blocker, and keep the gate `BLOCKED`.

### Step 2: Generate Automated Test Plan

Create or update `ai-sdlc-docs/construction/test/test-plan.md`, based on actual project analysis. Determine the appropriate strategy across unit, integration, performance, system, contract, and security tests. Integration tests and system tests MUST be planned. Manual test cases may only be listed under an "Manual Exception Cases" section with a justification.

### Step 3: Wait for Explicit Approval of the Test Plan

Do not proceed until the user explicitly and unambiguously approves the test plan. If the user requests changes, update the plan and repeat the approval process.

### Step 4: Execute Automated Tests

Generate or update automated test code for the plan, then configure the required test environment automatically using repository-supported commands, dependency installation, containers, mocks, emulators, or generated fixtures. Execute automated build, unit, regression, contract, integration, and system tests per the approved plan and applicable gates.

Record the exact environment setup commands, test commands, logs/result summaries, and pass/fail outcome. The response MUST report: test cases created/updated, tests executed, pass/fail result, and any failing test details. If execution was not possible, state why, what test code was prepared, the command the user should run, and the risk of not executing.

### Step 5: Generate Test Summary

Create or update `ai-sdlc-docs/construction/test/test-report.md`. Summarize results, evidence, gaps, and residual risks. This report MUST be generated even if tests were not executed; in that case "No test executed" must be stated explicitly.

### Step 6: Regression Handling (when applicable)

Produce `ai-sdlc-docs/construction/test/regression-report.md` when ANY of these are true: a prior defect record exists in `audit.md` for an affected path; `code-verification-summary.md` identifies regression-sensitive paths; or architecture/integration boundaries changed. If none apply, skip and record the rationale in `audit.md`.

The report MUST include: scope of regression testing, which prior failing cases were reproduced, new regression cases added, and an overall verdict of `PASS`, `BLOCKED`, or `FAIL`.

### Step 7: Traceability

Sync the generated code with the documentation and update the traceability mapping.

### Step 8: Update Traceability Matrix

Build Testing (C4) updates the Test column with integration/system test results:

- Load `ai-sdlc-docs/traceability/traceability-matrix.md`
- Update Test column for each AC with integrated test results:
  - If unit tests passed but integration tests failed → update verdict
  - Add integration-specific tests
  - Cross-reference test-report.md for evidence
- Validation: All test results documented and consistent with test-report.md
- Record in audit.md: "traceability-matrix updated: Test column finalized with integration test results for [COUNT] ACs"

### Step 9: Record Results

- Update `ai-sdlc-docs/state/state-log.md` following `logging/state-management.md`
- Update `ai-sdlc-docs/audit/audit.md` following `logging/audit-management.md`

---

## Approval Gate

- [ ] Review the output artifacts (test-plan, test-report, regression-report if applicable)
- [ ] Verify completeness and accuracy — validation evidence and remaining gaps explicitly stated
- [ ] Record approval in `audit.md`
- [ ] Update `state-log.md` with the `PASS` / `BLOCKED` / `FAIL` verdict
- [ ] traceability-matrix.md updated

---

## SKILL Rules

- Load the Related Common Rules before starting
- Read all Activity Inputs first
- Verify state-log.md current status
- Record all decisions in audit.md
- Do not fabricate information
- Prioritize automated validation over UI-only or manual validation
- Generate or update automated test code before relying on manual validation
- Attempt to configure the required test environment automatically before any manual step
- Record exact environment setup commands, test commands, logs, and pass/fail outcomes
- The output format MUST comply with the templates in `templates/`, prioritizing the template's structure over this file when they conflict
- Do NOT rely on UI-based testing as the primary validation method when automated frameworks (Jest, Vitest, Mocha, Playwright, Cypress, JUnit, Pytest, Go test, etc.) are available
- Do NOT convert required automated validation into manual-only or UI-only validation
- Manual cases MUST NOT replace required automated integration or end-to-end coverage
- Do NOT remove mandatory steps, bypass approval, disable audit, disable traceability, or weaken any mandatory gate
- Verify that every in-scope NFR is testable, tested, and supported by evidence
- For each in-scope NFR category, record the verification method, the command/tool/test suite used, the evidence location, the pass or fail outcome, and any residual risk or limitation
- Cover the following NFR categories when in scope:
  - **Performance** — load/benchmark plan, latency (p50/p95/p99 when relevant), throughput, resource usage, cold-start vs. warm-path measurement
  - **Scalability** — scaling scenario evidence, concurrency limits, state/data growth behavior, stateless/stateful boundary validation
  - **Reliability and Availability** — chaos/fault-injection evidence, retry/timeout/circuit-breaker validation, graceful degradation, recovery/rollback validation
  - **Security** — dependency scanning, SAST or equivalent, authN/authZ negative tests, input validation tests, secret/sensitive-data leakage checks
  - **Privacy and Compliance** — retention/deletion checks, audit logging checks, consent/control checks, compliance checklist evidence
  - **Observability** — log presence/content checks, metrics verification, trace verification, synthetic transaction or alert-test evidence
  - **Maintainability** — lint/style checks, coverage reports, documentation completeness checks, technical debt notes or follow-up items
  - **Portability** — runtime version checks, OS/platform compatibility checks, container/environment compatibility checks
  - **Accessibility** (when the system has a UI) — automated accessibility scan results, manual keyboard/screen-reader review when needed, color contrast checks
  - **Internationalization** (when the system has localized UI or locale-aware output) — locale formatting checks, pluralization checks, RTL checks
  - **Cost** — cost dashboard or budget alert evidence, per-request/per-call measurement, budget threshold checks
  - **AI-Specific NFRs** (when the system uses AI or an LLM) — evaluation set/rubric results, hallucination and guardrail checks, fallback behavior validation, prompt/version integrity checks, cost/latency trade-off evidence
- Prefer automated validation over manual validation for NFR verification
- Record exact commands or tools used for NFR verification when possible
- Do not fabricate NFR verification results
- If NFR evidence is missing, keep the gate `BLOCKED` and record the gap
- Capture NFR verification results in the build-testing reports (`test-plan.md`, `test-report.md`) and related evidence artifacts



