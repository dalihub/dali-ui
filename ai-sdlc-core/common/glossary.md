# Glossary

A single source of truth for all AI-SDLC terminology. When rules or templates use any term below, they MUST mean exactly what is defined here.

## Workflow Structure

| Term | Definition |
|------|------------|
| **Workflow** | One end-to-end execution from request → release for a single delivery cycle. Identified by `WF-YYYYMMDD-###`. |
| **Loop** | A major phase within a workflow. Two loops exist: **Inception Loop** (A0–A5) and **Construction Loop** (C1–C5). |
| **Inception Loop** | Planning and design phase. Activities: A0 (Context Analysis), A1 (Requirements Elaboration), A2 (Requirements Analysis), A3 (Workflow Planning), A4 (Architecture Design), A5 (Units Generation). Gated by **Inception Review**. |
| **Construction Loop** | Implementation and release phase. Activities: C1 (Implementation Design), C2 (Code Generation), C3 (Code Verification), C4 (Build & Testing), C5 (Release). Gated by **Construction Review**. |
| **Activity** | A single executable unit of work (A0–A5 in Inception, C1–C5 in Construction). Each Activity has a rule file. |
| **Review** | A mandatory checkpoint that produces a `PASS` / `BLOCKED` / `FAIL` verdict. Two Reviews: **Inception Review** (after Inception Loop) and **Construction Review** (after Construction Loop). |
| **Integration Ready Check** | A mandatory checkpoint between Code Verification (C3) and Build & Testing (C4). Verifies all units are code_verified with PASS unit tests and no blocking items before integration testing. |
| **Cycle** | The full sequence of workflows on the same system over time. New work after a release is a new workflow within the same cycle. |

## Activities (11 total)

AI-SDLC consists of 11 Activities organized into two loops.

### Inception Loop Activities (A0–A5)

| ID | Activity |
|----|----------|
| **A0** | Context Analysis |
| **A1** | Requirements Elaboration |
| **A2** | Requirements Analysis |
| **A3** | Workflow Planning |
| **A4** | Architecture Design |
| **A5** | Units Generation |

### Construction Loop Activities (C1–C5)

| ID | Activity |
|----|----------|
| **C1** | Implementation Design |
| **C2** | Code Generation |
| **C3** | Code Verification |
| **C4** | Build & Testing |
| **C5** | Release |

### Execution Markers

When an activity is selected for execution, it is assigned an execution marker that clarifies whether to execute, skip, or make conditional:

| Marker | Meaning |
|--------|---------|
| **MUST** | Execute without exception; no conditions |
| **COND** | Execute only if specified condition is true; when condition is met, execution is mandatory |
| **SKIP** | Do not execute; activity not applicable to this work |

## Work Types

| Code | Meaning |
|------|---------|
| `NEW_BUILD` | New service, application, module, or major capability |
| `REQ_ADD` | New requirement, enhancement, or feature addition to an existing system | 
| `DEFECT_FIX` | Bug analysis, corrective action, regression prevention |
| `TECH_CHANGE` | Refactoring, migration, modernization, optimization |

---

## Workspace Modes

| Mode | Definition |
|------|------------|
| **NEW** | No source code, no build configuration, no project manifest exists in the workspace. |
| **EXISTING** | Source code, build configuration, or project manifest exists in the workspace. Reverse-engineering is required when behavior, ownership, dependencies, or interfaces are not obvious. |

---

## Depth Levels

| Level | When |
|-------|------|
| **Minimal** | Small, low-risk, narrow scope, request already clear |
| **Standard** | Normal feature work, moderate impact, ≥1 module/interface affected |
| **Comprehensive** | High risk, architecture changes, unclear root cause, multi-team work, compliance/security/performance constraints |

## Operating Modes

| Mode | Definition |
|------|------------|
| **Single-Unit Mode** | One developer/session can complete the work safely; A5 (Units Generation) is not needed |
| **Multi-Unit Team Mode** | Work is split across units, parallel sessions, or distinct roles; A5 produces multiple units with dependencies and ownership |
| **NEW Mode** | NEW_BUILD work: start from scratch with no prior artifacts or code |
| **EXISTING Mode** | IMPROVE work (REQ_ADD, DEFECT_FIX, TECH_CHANGE): work with existing code and artifacts; A0 evaluates artifact status |

## State Vocabulary

### Workflow States
`reported` → `elaborating` → `analyzed` → `planned` → `architecture-defined` → `units-defined` → `in-development` → `integrating` → `validating` → `release-ready` → `released` → `closed`

### Unit States
`planned` → `design_ready` → `code_generated` → `code_verified` → `integrated` → `released`

Special: `blocked` (any state can transition to blocked).

### Review Verdicts

| Verdict | Meaning |
|---------|---------|
| `PASS` | All required evidence present, all checks satisfied (human-driven workflow) |
| `PASS (self-play)` | All checks satisfied but the workflow was self-played; manual verification still pending |
| `PASS with manual-pending` | Automated checks pass but specific ACs require human smoke testing |
| `BLOCKED` | Required evidence missing or external dependency unresolved |
| `FAIL` | Evidence shows the Review criteria are not met |

### AC Evidence Labels

| Label | Meaning |
|-------|---------|
| `executed-pass` / `executed-fail` | Automated test ran end-to-end |
| `logic-verified` | Pure logic tests cover the underlying functions; user-observable behavior NOT exercised end-to-end |
| `code-review-only` | Code path inspected; no test executed |
| `manual-pending` | Requires human verification, not yet performed |
| `manual-pass` / `manual-fail` | Human exercised the AC |
| `not-applicable` | AC does not apply |

**Banned in self-play**: `manual-pass` (the agent was not a human user)  
**Banned everywhere**: `auto-verifiable`, `auto-PASS`, `inferred-pass`, `should work`

### Release Readiness

| Status | Meaning |
|--------|---------|
| `ready` | All required construction evidence passed |
| `ready with conditions` | Minor non-blocking conditions documented and accepted |
| `not ready` | Any required Review is missing or failing |

## Identifiers

| Prefix | Meaning | Example |
|--------|---------|---------|
| `WF-YYYYMMDD-###` | Workflow id | `WF-20260503-001` |
| `UNIT-###` | Unit of work id | `UNIT-007` |
| `REQ-###` | Requirement id | `REQ-042` |
| `DEC-###` | Decision id | `DEC-013` |
| `DEF-###` | Defect id | `DEF-019` |
| `TC-###` | Test case id | `TC-101` |

## Confidence Vocabulary (DEFECT_FIX)

When stating root cause, use ONE of these labels:

| Label | Meaning |
|-------|---------|
| `confirmed root cause` | Reproduced and isolated with evidence |
| `likely root cause` | Supported by evidence but not isolated |
| `hypothesis under investigation` | Proposed explanation, evidence pending |

**Rule**: Never state root cause as fact without one of these labels.

## Integration Roles

| Term | Definition |
|------|------------|
| **ROLE-INT** | Primary Integration Owner responsible for integration coordination |
| **ROLE-INT-BACKUP** | Backup Integration Owner who assumes authority when Primary is unavailable |
| **Delegation** | Temporary transfer of specific Integration Owner authorities |

## Banned Phrases

These phrases create false validation claims and MUST NOT appear in any artifact:

| Banned Phrase | What to Say Instead |
|---------------|-------------------|
| "tests should pass" / "tests would pass" | `PASS`, `FAIL`, or `not executed` |
| "everything looks good" | Describe what was checked |
| "comprehensive testing was done" (without listing what) | List actual test execution |
| "production-ready" (without Construction Review PASS) | Include Construction Review verdict |
| `auto-verifiable`, `auto-PASS`, `inferred-pass` | Use official labels from `build-testing.md` |
| "fully verified" / "all ACs pass" (in self-play) | Add `(self-play)` qualifier |
