---
title: AI-SDLC Workflow
version: 1.0.0
owner: SR-AX-LAB
triggers:
  - "ai-sdlc start"
  - "ai-sdlc execute"
allowed-tools:
  - Read
  - Write
  - Edit
  - Bash
  - AskUserQuestion
  - Agent
  - Skill
---

# AI-SDLC Main Workflow

## Description

Execute the unified AI-SDLC workflow. This workflow:
1. Scans workspace to determine artifact status
2. Executes inception loop (requirements, design, planning)
3. Executes construction loop (implementation, code, build, release)
4. Gates each loop with review checkpoints

---

## Reference Rules

- **ai-sdlc-org/config/naming-rules.md** — All artifact identifiers (REQ, DEC, UNIT, TC, etc.) and file/directory naming conventions [[naming-rules]]
- common/loop-continuation.md — Distinguish resumption (interrupted workflow) from continuation (completed → new workflow); resumption preserves prior decisions and MUST NOT restart path-finder
- logging/state-management.md — State tracking and workflow state lifecycle
- logging/audit-management.md — Audit recording and decision tracking
- logging/traceability-management.md — Requirement-to-code-to-test traceability
- glossary.md — Terminology
- review/review-protocol.md — 5-phase review protocol (Preparation → Evaluation → Verdict → Recording → User Choice) [relocated to ai-sdlc-core/review/]
- review/review-rule.md — Loop Review Skill (Inception & Construction, enforces 3 principles + protocol) [relocated to ai-sdlc-core/review/]

---

## Outputs

| Artifact | Location | Description |
|----------|----------|---------|
| state-log.md | `ai-sdlc-docs/state/state-log.md` | Workflow execution log and activity tracking |
| audit.md | `ai-sdlc-docs/audit/audit.md` | Workflow decisions and rationale |
| traceability-matrix.md | `ai-sdlc-docs/traceability/traceability-matrix.md` | REQ → AC → Architecture → Code → Test mapping (canonical handoff) |

---

### Output Templates

**Location:** `ai-sdlc-org/templates/` — All output templates  
**Reference:** Used by all activities to generate consistent artifacts

---

## Execution Steps

### Step 1: Artifact Status Check

Verify existing AI-SDLC artifacts.

**Check for:**
- state-log.md (workflow execution log)
- audit.md (decision audit trail)
- traceability-matrix.md (requirements ↔ architecture ↔ code mapping)

**Result:**
- All exist → Check state-log.md for resumption vs continuation (per [[loop-continuation]])
  - If resumption (in_progress/blocked/paused status): Load prior state from state-log.md and audit.md; follow Resumption Rule; resume from agreed recovery point; do NOT restart path-finder; skip Step 2 and proceed directly to Step 3
  - If continuation (completed/released status): Proceed to Step 2
- Some missing → Initialize missing artifacts → Proceed to Step 2
- None exist → New project, proceed with Step 2

---

### Step 2: Generate Execution Plan

**Purpose:** Generate complete workflow (Inception + Construction) dynamically based on requirements and context.

**Execute:** path-finder

**Inputs:**
- Artifact status from Step 1
- work_type (NEW_BUILD, REQ_ADD, DEFECT_FIX, TECH_CHANGE)
- operating_mode (single-developer or team)
- depth (minimal, standard, comprehensive)
- run_context flags (boundary changes, multi_unit, ...)

**Output:** `ai-sdlc-docs/inception/plans/work-path.md`

**Next:** User reviews and approves work-path.md

---

### Step 3: Execute Approved Plan

**Purpose:** Execute the activities specified in work-path.md in order.

**Execute:** Activities from work-path.md, sequentially

**For each activity:**
1. Load activity's rule file
2. Execute the activity
3. Record completion in state-log.md (PASS/BLOCKED/FAIL)
4. Record decisions in audit.md
5. Continue to next activity

**Handle gates:**
- INCEPTION_REVIEW:
  - If PASS: Continue to construction
  - If BLOCKED: Fix and repeat failed activity
  - If FAIL: Stop, manual intervention

- INTEGRATION_READY_CHECK (after code-verification, before build-testing):
  - If PASS: Continue to build-testing
  - If BLOCKED/FAIL: Fix and repeat failed activity

- CONSTRUCTION_REVIEW:
  - If PASS: Proceed to Step 4
  - If BLOCKED/FAIL: Fix and repeat failed activity

---

### Step 4: Finalize Traceability Matrix

After the execution plan completes and CONSTRUCTION REVIEW passes, finalize traceability-matrix.md.

#### Traceability Matrix Finalization

Complete and validate traceability-matrix.md as the canonical handoff artifact.

**Content (built incrementally during Step 3):**
- REQ → AC → Architecture Component → Code (file:symbol) → Test mapping (progressively built)
- Decision Impact (DEC-### → Implementation Impact)
- Coverage Summary (Requirements, ACs, Code, Test coverage metrics)
- Unit Coverage (multi-unit mode)
- Architecture Component Mapping (ASCII diagram)
- Prior REQ Traceability (if continuing from prior workflow)
- DEFECT_FIX Addendum (if work_type = DEFECT_FIX)

**Actions:**
- Validate all sections are complete
- Add Decision Impact mappings (from audit.md)
- Calculate Coverage Summary metrics
- Add Unit Coverage section (if multi-unit)
- Add Architecture Component Mapping (if helpful)
- Add Prior REQ Traceability (if applicable)
- Add DEFECT_FIX Addendum (if applicable)

**Location:** `ai-sdlc-docs/traceability/traceability-matrix.md`  
**Template:** `ai-sdlc-org/templates/19-traceability-matrix.md`  
**Purpose:** Single source of truth linking requirement → design → code → test for anyone reading the project later

---

## Loop Completion Reviews

### Inception Loop Review

**When:** After all inception activities complete  
**Gate:** PASS required to proceed to construction  
**Verdict:** PASS / BLOCKED / FAIL  
**Checklist:** ai-sdlc-org/config/review/inception-review-checklist.md  
**Skill:** review/review-rule.md (loop=inception)

### Construction Loop Review

**When:** After all construction activities complete  
**Gate:** PASS required to complete workflow  
**Verdict:** PASS / BLOCKED / FAIL  
**Checklist:** ai-sdlc-org/config/review/construction-review-checklist.md  
**Skill:** review/review-rule.md (loop=construction)

### Integration Ready Check

**When:** After code-verification, before build-testing  
**Gate:** PASS required to proceed to build-testing  
**Verdict:** PASS / BLOCKED / FAIL  
**Checklist:** ai-sdlc-org/config/review/integration-ready-checklist.md  
**Reference:** review/integration-ready-check-rule.md

## State Management

All state changes must be recorded:

- **state-log.md**: Execution log and current activity status
- **audit.md**: All decisions with rationale and timestamps

---

## SKILL Rules

- **Load Tier 1 Rules FIRST:** Load all Tier 1 common rules before any execution (loop-continuation, state-management, audit-management, traceability-management, glossary, review-protocol, ai-execution-rule)
- **Path Finder Required:** For a new or continuation workflow, run path-finder (Step 2) to generate work-path.md BEFORE executing any activity. Never assume the workflow path; derive it from checklists and run_context. **Exception — resumption:** when Step 1 determines the workflow is a resumption, path-finder MUST NOT run and the existing approved work-path.md MUST be preserved.
- **Approve Before Execute:** work-path.md must be explicitly approved before Step 3 execution begins.
- **Load Activity Rules Before Execution:** Load the skill file for each activity BEFORE executing that activity (e.g., load skills/requirements-elaboration/SKILL.md BEFORE Requirements Elaboration activity)
- **Record All Decisions:** Record every decision and state change in audit.md with timestamp and rationale
- **State Management:** Update state-log.md after EVERY activity completion with status (PASS/BLOCKED/FAIL)
- **Traceability Matrix Updates:** Update traceability-matrix.md incrementally as activities complete (only for activities included in work-path.md):
  - Requirements Analysis: Add REQ and AC
  - Architecture Design: Add Architecture Components
  - Code Generation: Add Code (file:symbol) mappings
  - Code Verification: Add and update Test results
  - Build & Testing: Finalize Test results

- **Review Gates are MANDATORY:** Inception Loop Review, Integration Ready Check, and Construction Loop Review must PASS before proceeding. Execute via review-rule.md skill (Inception/Construction) or integration-ready-check-rule.md rule. path-finder MUST include them in every work-path.md and MUST NOT skip them.

- **Activity Selection:** path-finder determines which activities run (MUST/COND/SKIP) via backward-chaining from checklists and run_context. All selection decisions are recorded in work-path.md and audit.md.
- **No Gate Skipping:** If review is BLOCKED or FAIL, do not proceed. Load recovery-paths.md and follow recovery process.
- **Activity Modularity:** Each activity can be implemented via rule file, skill, agent, or custom logic. work-path.md defines WHICH activities to run, not HOW to execute them.

---

## Notes for Execution

- **Incremental Updates:** state-log, audit, and traceability-matrix are updated incrementally as each activity completes (not batched at the end)

- **Activity Ownership:** Each activity rule file is responsible for updating state-log.md, audit.md, and traceability-matrix.md as the activity executes

- **Artifact Lifecycle:** traceability-matrix is progressively built during Step 2-3 (REQ/AC/Architecture → Code/Test) and finalized in Step 4 with metrics and decision impacts

- **Decision Rationale:** Always document WHY decisions were made in audit.md, not just WHAT was decided, to help future maintainers understand context and constraints
