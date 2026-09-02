---
title: State Management
rule_id: STATE_MANAGEMENT_RULE
phase: common
version: 1.0.0
owner: SR-AX-LAB
criticality: mandatory
source_file: ai-sdlc-core/logging/state-management.md
depends_on:
  - ai-sdlc.md (workflow orchestration)
  - ai-execution-rule.md (state trust principle)
  - loop-continuation.md (state inheritance)
outputs:
  - ai-sdlc-docs/state/state-log.md (workflow execution log)
---

# State Management

## Purpose

Define the workflow state lifecycle and the structure of `state-log.md`, which is the authoritative record of workflow execution status, activity progress, and review verdicts.

---

## Reference Rules

- `ai-sdlc.md` — Workflow orchestration (Steps 1-4, state updates)
- `ai-execution-rule.md` Part 3 — State is Authoritative principle
- `loop-continuation.md` — State inheritance across workflow continuations
- `audit-management.md` — How state changes are documented in audit.md
- Template: `templates/11-state-log.md`

---

## State Model

### Workflow State

A workflow has eight state fields:

| Field | Type | Values | When Updated |
|-------|------|--------|--------------|
| `workflow_id` | string | `WF-YYYYMMDD-###` | At Step 1 (initialization) |
| `work_type` | enum | `NEW_BUILD`, `REQ_ADD`, `DEFECT_FIX`, `TECH_CHANGE`| At Step 2 (path-finder determines) |
| `operating_mode` | enum | `single_developer`, `team` | At Step 2 (path-finder determines) |
| `inception_status` | enum | `not_started`, `in_progress`, `blocked`, `paused`, `completed` | After each inception activity |
| `construction_status` | enum | `not_started`, `in_progress`, `blocked`, `paused`, `completed` | After each construction activity |
| `current_loop` | enum | `inception`, `construction` | Updated when loop changes |
| `current_activity` | string | Activity code (A0-A5, C1-C5) | Updated before/after each activity |
| `overall_status` | enum | `new`, `in_progress`, `blocked`, `paused`, `completed`, `released` | Aggregated from loop statuses |

### Unit State Summary (Multi-Unit Mode)

When `operating_mode = team` (multiple units):

| Field | Type | Description |
|-------|------|-------------|
| `units[]` | array | Array of unit objects |
| `units[].unit_id` | string | Unit identifier |
| `units[].status` | enum | Unit status (`in_progress`, `blocked`, `completed`) |
| `units[].code_verification_verdict` | enum | `PASS`, `BLOCKED`, `FAIL` (set by code-verification C3, evaluated by integration-ready-check-rule) |
| `units[].last_activity` | string | Last completed activity for this unit |

### Review State

Three review checkpoints with verdict recording:

| Review | When | Verdict Field | Values |
|--------|------|---------------|--------|
| Inception Review | After A5 completes | `inception_review_verdict` | `PASS`, `BLOCKED`, `FAIL` |
| Integration Ready Check | After C3 completes (per unit) | `integration_ready_check_verdict` | `PASS`, `BLOCKED`, `FAIL` |
| Construction Review | After C5 completes | `construction_review_verdict` | `PASS`, `BLOCKED`, `FAIL` |

### Integration Snapshot

Final summary state before release:

| Field | Description |
|-------|-------------|
| `all_units_pass_integration_ready_check` | Boolean (true only if all units PASS integration-ready-check-rule) |
| `release_readiness` | `ready`, `ready_with_conditions`, `not_ready` (from release-note.md) |
| `final_status` | `completed` or `released` |

---

## State-log.md Structure

**Location:** `ai-sdlc-docs/state/state-log.md`  
**Template:** See `templates/11-state-log.md`

The file must contain all the state fields listed above in a human-readable format (typically a YAML header + markdown table for unit summary).

---

## State Update Rules

### When to Initialize state-log.md

**At Step 1 of ai-sdlc.md (Artifact Status Check):**

1. Check if `ai-sdlc-docs/state/state-log.md` exists
2. If NO (new workflow):
   - Mint a new `workflow_id: WF-YYYYMMDD-###`
   - Initialize all fields to `not_started` or `null`
   - Set `overall_status: new`
   - Use template: `templates/11-state-log.md`
3. If YES (continuation or resumption):
   - See `loop-continuation.md` for inheritance or resumption rules
   - If continuation: mint a NEW `workflow_id`, record `predecessor` field
   - If resumption: preserve existing `workflow_id`

### When and How to Update current_activity

**Before activity starts:**
- Update `current_activity: A#` or `C#`
- Record timestamp

**After activity completes:**
- Update activity status in `inception_status` or `construction_status`
- Record verdict (PASS / BLOCKED / FAIL) from that activity's completion
- Update `current_activity: [next activity]` (to prepare for next step)

### When and How to Record Verdicts

**After review gate executes (review-protocol.md Phase 4):**

1. **Inception Review** (after A5):
   - Update `inception_review_verdict: [PASS / BLOCKED / FAIL]`
   - Update `inception_status: completed`
   - Record timestamp

2. **Integration Ready Check** (after C3, per unit):
   - Update `units[unit_id].code_verification_verdict: [PASS / BLOCKED / FAIL]`
   - If multi-unit: check if all units PASS → update `all_units_pass_integration_ready_check: true/false`
   - Update gate verdict `integration_ready_check_verdict: [PASS / BLOCKED / FAIL]`

3. **Construction Review** (after C5):
   - Update `construction_review_verdict: [PASS / BLOCKED / FAIL]`
   - Update `construction_status: completed`
   - Record timestamp

### State Transition Rules

**Inception Loop Transitions:**
- `not_started` → `in_progress` (when A0 or A1 starts)
- `in_progress` → `blocked` (if any activity blocked, waiting for user action)
- `in_progress` → `paused` (if user pauses workflow)
- `in_progress` → `completed` (after A5 completes)
- `blocked` → `in_progress` (when blocker is resolved)
- `paused` → `in_progress` (when user resumes)

**Construction Loop Transitions:**
- `not_started` → `in_progress` (when C1 starts, after Inception Review PASS)
- `in_progress` → `blocked` (if any unit or activity blocked)
- `in_progress` → `paused` (if user pauses workflow)
- `in_progress` → `completed` (after C5 completes)
- `blocked` → `in_progress` (when blocker is resolved)

**Overall Status Aggregation:**
- If `inception_status == blocked` or `construction_status == blocked` → `overall_status: blocked`
- If `inception_status == paused` or `construction_status == paused` → `overall_status: paused`
- If `inception_status == in_progress` → `overall_status: in_progress`
- If `inception_status == completed` and `construction_status == not_started` → `overall_status: in_progress`
- If `construction_status == in_progress` → `overall_status: in_progress`
- If `construction_status == completed` → `overall_status: completed` (after Construction Review PASS)
- If workflow is released → `overall_status: released`

### Error State Handling

**When a Review Verdict is BLOCKED or FAIL:**

According to `recovery-paths.md`:
1. Set `overall_status: blocked` (or `paused` if user chooses to pause)
2. Set the corresponding loop status to `blocked`
3. Record the blocker details in state-log.md
4. Record recovery action in audit.md
5. Do NOT automatically advance to next loop or activity
6. Wait for user decision (recorded in audit.md)

---

## State Inheritance (Loop Re-entry)

When starting a **continuation workflow** (see `loop-continuation.md`):

### Step 1: Verify Prior Workflow Status
- Load the most recent `state-log.md` from prior workflow
- Check `overall_status` field:
  - If `completed` or `released` → This is a **continuation** (new workflow)
  - If `in_progress`, `blocked`, or `paused` → This is a **resumption** (same workflow)

### Step 2: Continuation-Specific Inheritance

When starting a new workflow after prior completion:

1. **Mint a NEW workflow_id**: `WF-YYYYMMDD-###` (increment from prior)
2. **Record the predecessor relationship**:
   ```yaml
   workflow_id: WF-20260503-002
   predecessor: WF-20260501-001
   predecessor_status: released
   ```
3. **Inherit context** (from prior workflow's final state):
   - All completed artifact paths
   - All decisions from audit.md (especially DEC-### entries marked "downstream impact")
   - Architecture and unit definitions (unless drift detected)
   - Release status
4. **Initialize new loop state**:
   - `inception_status: not_started` (path-finder will determine if A0-A5 needed)
   - `construction_status: not_started`
   - `current_activity: null` (path-finder will determine entry point)
5. **Path Finder generates work-path**:
   - Uses inherited artifacts
   - Determines which activities are MUST/COND/SKIP for the new request
   - Updates state-log.md with new work-path

### Step 3: Resumption-Specific Inheritance

When resuming a mid-execution workflow:

1. **Preserve existing workflow_id**
2. **Load the exact state from prior session**:
   - `current_activity` (where we left off)
   - `inception_status` or `construction_status` (current loop state)
   - All unit-level state (if multi-unit)
3. **Present resume summary** to user:
   - Current loop, current task, last completed activity
   - Blocked items and reasons
   - Active approvals pending
   - Unit-level status (if multi-unit)
4. **Propose safe resume point** based on state-log
5. **Continue from that exact point** (do NOT restart)

---

## Mandatory Constraints

**Never:**
- ❌ Update state-log without recording the change in audit.md
- ❌ Leave state-log with incomplete activity status (must be one of: `in_progress`, `blocked`, `paused`, `completed`)
- ❌ Advance to next loop without explicit user approval after review verdict
- ❌ Overwrite prior state without documenting the reason in audit.md
- ❌ Flatten multi-unit state into a single overall status (preserve unit-level evidence)
- ❌ Mark an activity complete without completion evidence

**Always:**
- ✅ Timestamp every state change (ISO 8601 format)
- ✅ Record who made the state change (AI Agent or User)
- ✅ Document the reason for state transitions in audit.md
- ✅ Verify state consistency before executing next activity (load state-log first)

---

## State-log Verification Checklist

Before executing any activity, verify state-log.md is in a consistent state:

- [ ] `workflow_id` is set and follows `WF-YYYYMMDD-###` format?
- [ ] `work_type` is one of the 4 valid types?
- [ ] `operating_mode` is either `single_developer` or `team`?
- [ ] `current_activity` matches the activity about to execute?
- [ ] `inception_status` and `construction_status` are valid enum values?
- [ ] All unit state (if multi-unit) has consistent verdicts and status?
- [ ] Last timestamp is recent (no stale state)?
- [ ] All review verdicts (if recorded) are `PASS`, `BLOCKED`, or `FAIL` (not ambiguous)?

If ANY check fails → **HALT**: state-log is corrupted. Document the issue in audit.md and wait for user direction.

---

## Summary

**state-management.md** defines:
1. **State Model**: Workflow, unit, and review state fields
2. **Initialization**: When and how to create new state-log.md
3. **Updates**: When and how to update each state field during workflow execution
4. **Transitions**: Valid state transitions for inception/construction loops
5. **Inheritance**: How state carries forward in loop re-entry
6. **Constraints**: What MUST and MUST NOT be done with state
7. **Verification**: Checklist to validate state consistency

The authoritative record is always `state-log.md` — when in-context belief conflicts with state-log, trust state-log (per ai-execution-rule.md Part 3).
