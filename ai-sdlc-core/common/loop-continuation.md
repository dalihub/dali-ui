# Loop Continuation Rule

## Purpose
Define how a follow-up workflow inherits context from a prior workflow on the same system, so that AI Native development can iterate continuously without losing decisions, traceability, or architectural intent.

## When to Apply
Apply this rule whenever ANY of the following is true at workflow start:
- `ai-sdlc-docs/state/state-log.md` already exists from a prior workflow
- The user references prior work ("the feature we shipped last week", "the bug from yesterday's release")
- The current request is naturally a follow-up to a prior completed workflow

## Continuation vs. Resumption (do not confuse)

| Concept | Definition | How to handle |
|---------|------------|---------------|
| **Resumption** | Continuing an INCOMPLETE workflow that was interrupted | Load prior state; present resume summary; continue from last known point |
| **Continuation** | Starting a NEW workflow that builds on a COMPLETED prior workflow | Inherit completed artifacts; mint new workflow ID; execute Path Finder |

Check `state-log.md` first:
- If the latest workflow's status is `closed` or `released` → this is **continuation** (use this file)
- If the latest workflow is mid-execution (e.g., `in_progress`, `blocked`) → this is **resumption** (follow resumption rules below)

## Resumption Rule (When workflow is mid-execution)

If the latest workflow is mid-execution:
1. **Present Resume Summary**: Load `state-log.md` and present:
   - Current loop and current task
   - Completed tasks with evidence
   - Skipped tasks and reasons
   - Open questions and blockers
   - Pending approvals
   - Active units and their states
2. **Check for Workspace Drift**: If the codebase or prior artifacts changed since the last state, call out possible drift
3. **Multi-Unit Status**: If multiple units exist, preserve unit-level evidence; identify integration blockers
4. **Propose Safe Resume Point**: Based on state-log, propose the safest next activity
5. **Record Recovery Decision**: Log the resume decision according to `logging/audit-management.md`
6. **Continue Execution**: Resume from the agreed recovery point, do NOT restart from the beginning

## Continuation Procedure (When workflow is complete)

### Step 1: Inherit Prior Context
Load the following from the most recent workflow:
- `ai-sdlc-docs/state/state-log.md` — final state, Reviews, release readiness
- `ai-sdlc-docs/audit/audit.md` — all decisions (especially DEC-### entries marked "downstream impact")
- `ai-sdlc-docs/inception/architecture/architecture-design.md` — existing architecture (still authoritative)
- `ai-sdlc-docs/inception/units/unit-generation.md` — existing units (still authoritative)
- `ai-sdlc-docs/construction/release/release-note.md` (latest) — what was shipped
- `ai-sdlc-docs/traceability/traceability-matrix.md` — requirement → code links

### Step 2: Mint a New Workflow ID
- Generate a NEW id: `WF-YYYYMMDD-###` (increment from the prior id)
- Do NOT reuse the prior id
- Record the relationship in the new `state-log.md`:
  ```
  workflow_id: WF-20260503-002
  predecessor: WF-20260501-001
  predecessor_status: released
  ```

### Step 3: Trigger Drift Detection
Check if reverse-engineering artifacts exist and need validation:

**IF `ai-sdlc-docs/inception/reverse-engineering/reverse-index.md` exists**:
- Execute drift detection per `reverse-engineering.md` (Step 1: Determine Depth and Scope)
- Reverse-engineering task will validate artifact freshness and identify stale artifacts
- Document drift findings in `inheritance-manifest.md`

**IF it does NOT exist and workspace is brownfield**:
- Reverse-engineering is required before continuation
- See Greenfield-to-Brownfield rule below
- Do NOT proceed until reverse-engineering is complete

#### Greenfield-to-Brownfield Transition Rule

When the prior workflow's first activity was A1 (Requirements Elaboration) — meaning no A0 (Context Analysis) was needed — and the current request is a continuation:

1. Check if `ai-sdlc-docs/inception/reverse-engineering/` exists
2. If it does NOT exist:
   - The project is now effectively brownfield — code exists from the prior workflow
   - Reverse-engineering is required before proceeding with the new request
   - This is NOT re-doing work — it is creating the baseline understanding that was not needed for the initial workflow
   - Record the decision according to `logging/audit-management.md`
3. If it DOES exist:
   - Apply normal drift detection (execute reverse-engineering task per above)

### Step 4: Handoff to Path Finder
Pass continuation context to Path Finder:
- User's new request
- Inherited artifacts from prior workflow (Step 1)
- Drift detection findings from Step 3
- Prior workflow state from `state-log.md`

Path Finder will:
- Determine which inherited artifacts can be reused
- Generate a new work-path for this continuation workflow
- Execute the work-path
- Update `ai-sdlc-docs/inception/plans/work-path.md`

## Inheritance Manifest (Required Output)
At the end of Step 3 (Drift Detection), produce `ai-sdlc-docs/state/inheritance-manifest.md` listing:
- predecessor workflow id and status
- list of inherited artifacts and their last-validated date
- list of artifacts being re-derived (and why)
- list of detected drift items and their resolution


## Closing the Loop
At the end of a continuation workflow:
- Update `state-log.md` to mark this workflow `closed` or `released`
- Record `workflow_id`, `predecessor`, and `predecessor_status` for the next continuation
- Update `traceability-matrix.md` with new requirement→code links
- Record all continuation decisions through `logging/audit-management.md`
- The next continuation will inherit from this completed workflow

## Mandatory Constraints
- **Never silently restart**: If resumable state exists in `state-log.md`, present it to the user before continuing
- **Never mark incomplete as complete**: Do NOT mark an action complete unless completion evidence exists
- **Never overwrite silently**: Do NOT overwrite prior decisions without logging the reason in `audit.md`
- **Never flatten multi-unit state**: If multiple units exist, preserve unit-level evidence; do NOT merge everything into one summary

## Mandatory Constraints
- **Never silently restart**: If resumable state exists in `state-log.md`, present it to the user before continuing
- **Never mark incomplete as complete**: Do NOT mark an action complete unless completion evidence exists
- **Never overwrite silently**: Do NOT overwrite prior decisions without logging the reason in `audit.md`
- **Never flatten multi-unit state**: If multiple units exist, preserve unit-level evidence; do NOT merge everything into one summary

## Anti-Patterns
- ❌ Starting a new workflow without inspecting prior state
- ❌ Reusing prior architecture without drift detection
- ❌ Deleting or rewriting prior audit entries
- ❌ Silently restarting a mid-execution workflow from the beginning
- ❌ Marking a task complete without completion evidence
