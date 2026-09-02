---
title: Review Protocol
rule_id: REVIEW_PROTOCOL_RULE
applies_to: all_loops
version: 1.0.0
owner: SR-AX-LAB
criticality: mandatory
source_file: ai-sdlc-core/review/review-protocol.md
enforcement_point: review-checkpoint-skill
depends_on:
  - review-rule.md (caller)
  - {org-dir}/config/review/inception-review-checklist.md or construction-review-checklist.md
  - ai-sdlc-docs/state/state-log.md
outputs:
  - ai-sdlc-docs/state/state-log.md (verdict field)
  - ai-sdlc-docs/audit/audit.md (review decision + user choice)
trigger_condition: |
  Executed by review-rule.md skill.
  Determines current_loop from state-log.md and loads corresponding checklist.
  Executes 5-phase protocol: Preparation → Evaluation → Verdict → Recording → User Choice.
allowed-exceptions: None
auto-halt-conditions:
  - Phase 1: state-log.md missing or current_loop invalid
  - Phase 1: Required review files missing
  - Phase 2: Checklist evaluation incomplete (evaluated count ≠ total item count)
  - Phase 2: ⊘ N/A item without documented reason
  - Phase 4: verdict recording fails (state-log.md or audit.md not writable)
  - Phase 5: User provides ambiguous response (does not select explicit option)
---

# Review Protocol

## Purpose

Define the 5-phase execution workflow for review gates. This protocol is loop-agnostic (Inception vs Construction) — the checklist determines what is evaluated.

---

## How This Protocol Works

1. **Load current_loop from state-log.md**
2. **Load corresponding checklist** (inception or construction)
3. **Execute 5-phase protocol** (same steps regardless of phase)
4. **Record verdict** and enforce user choice

---

## PHASE 1: Preparation (Verify System State)

### Action 1.1: Load state-log.md
- **File:** `ai-sdlc-docs/state/state-log.md`
- **Required fields:** `current_loop`
- **Verification:**
  - File exists? ✓
  - current_loop value is valid (inception or construction)? ✓
  - If FAIL → **HALT**: verdict = BLOCKED, message = "state-log.md invalid or missing current_loop"

### Action 1.2: Load Loop-Specific Checklist
- **Check:** current_loop == "inception"?
  - YES → Load {org-dir}/config/review/inception-review-checklist.md
  - NO → Load {org-dir}/config/review/construction-review-checklist.md

Where `{org-dir}` is the discovered organization directory (see path-finder.md for dynamic resolution).

- **If file missing:** **HALT** with verdict = BLOCKED

**Completion Marker: Phase 1 ✓**

---

## PHASE 2: Checklist Evaluation (Evaluate All Items)

### Evaluation Steps

For each required deliverable in checklist:

1. **Read requirement** — understand what is being asked
2. **Check condition** — if conditional, verify condition met
   - Condition NOT met → Mark ⊘ N/A with documented justification
   - Condition IS met → MUST evaluate (cannot mark N/A)
3. **Locate evidence** — navigate to Evidence Location in checklist
4. **Verify existence** — does the file exist in ai-sdlc-docs/?
5. **Validate content** — does it contain what the requirement asks for?
6. **Determine status:**
   - ✓ COMPLETE: Evidence exists and satisfies requirement
   - ✗ MISSING: Evidence does not exist or is insufficient
   - ⊘ N/A: Condition not met, with documented justification

### Completion Check (MANDATORY)

**Before declaring verdict:**
- All required deliverables evaluated?
- Every ⊘ N/A has documented reason?
- If NO → **HALT WORKFLOW**: "Evaluation incomplete. Complete before proceeding."
- If YES → Proceed to Phase 3

**Completion Marker: Phase 2 ✓**

---

## PHASE 3: Verdict Determination (Calculate from Evidence)

### Auto-Calculate Verdict (per review-principles)

```javascript
✓_count = count(✓ items)
✗_count = count(✗ items)
⊘_count_justified = count(⊘ items with documented justification)

if (✗_count > 0) {
  VERDICT = BLOCKED
  Reason: "[number] checklist items missing evidence"
}
else if (✓_count + ⊘_count_justified === total_items) {
  VERDICT = PASS
  Reason: "All checklist items evaluated. [✓_count] complete, [⊘_count] N/A with justification"
}
else {
  // Should not happen if Phase 2 complete
  VERDICT = BLOCKED
  Reason: "Evaluation logic error — incomplete evaluation detected"
}
```

### Special Case: Evidence Contradiction (Principle 1)

If any item's evidence **contradicts** the requirement:
- Override auto-calculation
- VERDICT = FAIL
- Reason: "[Item X] evidence contradicts [requirement]"

**Examples of Evidence Contradiction:**
- Requirement: "Automated unit tests must PASS" → Evidence: "Unit test result = FAILED"
- Requirement: "All code changes documented in PR" → Evidence: "PR exists but description is empty"
- Requirement: "No blocking issues remain" → Evidence: "1 critical blocker recorded in team-status.md"
- User explicit rejection: Requirement states "User approval required" → User says "I reject this output"

### Verdict Result

**VERDICT = [PASS / BLOCKED / FAIL]**

**Completion Marker: Phase 3 ✓**

---

## PHASE 4: Recording (Persist Verdict to Audit Trail)

### Action 4.1: Update state-log.md

- **File:** `ai-sdlc-docs/state/state-log.md`
- **Field to update:** 
  - If inception → `inception_review_verdict`
  - If construction → `construction_review_verdict`
- **Value:** [PASS / BLOCKED / FAIL]
- **Timestamp:** ISO 8601 format

### Action 4.2: Update audit.md

- **File:** `ai-sdlc-docs/audit/audit.md`
- **New entry:**

```markdown
### [LOOP]-REV-[001]  (e.g., INC-REV-001 or CON-REV-001)

| Field | Value |
|-------|-------|
| Timestamp | [ISO 8601] |
| Loop | [Inception / Construction] |
| Verdict | [PASS / BLOCKED / FAIL] |
| Evaluation Summary | ✓: [count] items, ✗: [count] items, ⊘: [count] N/A items |
| Reason | [brief: why this verdict] |
| Missing Items (if BLOCKED) | [list of ✗ items by number and name] |
| Evidence Gaps (if BLOCKED) | [which evidence locations not found or insufficient] |
| Contradictions (if FAIL) | [what contradicted] |
| Recovery Actions (if BLOCKED/FAIL) | [what user must do to unblock/resolve] |
| Owner | AI Agent (Review) |
| Impact | Workflow advances or halts |
```

### Recording Success Check

- Both files updated without error?
  - YES → Proceed to Phase 5
  - NO → **HALT** with message: "Audit trail unavailable — cannot record verdict"

**Completion Marker: Phase 4 ✓**

---

## PHASE 5: User Choice (Enforce Explicit Transition)

### Action 5.1: Present Verdict and Choices

Based on verdict from Phase 3:

**If PASS:**
- Present: "All required deliverables verified. Ready to proceed."
- Offer: 1) Proceed to next loop / 2) Revise artifacts / 3) Additional setup (if applicable)

**If BLOCKED:**
- Present: "Missing evidence." List blocked deliverables.
- Offer: 1) Review blockers / 2) Revise artifacts / 3) Re-evaluate

**If FAIL:**
- Present: "Evidence contradicts requirements." List contradictions.
- Offer: 1) Review contradictions / 2) Revise artifacts / 3) Re-evaluate

### Action 5.2: Wait for Explicit User Response

- User MUST select an explicit option
- Response must NOT be ambiguous
- If ambiguous → Ask user to clarify

### Action 5.3: Record User Decision

Create entry in `ai-sdlc-docs/audit/audit.md`:

| Field | Value |
|-------|-------|
| Timestamp | [ISO 8601] |
| Review Loop | [Inception / Construction] |
| Verdict | [PASS / BLOCKED / FAIL] |
| User Choice | [option number and description] |
| Impact | [Workflow advances / Halts for revision / Retry review] |

### Action 5.4: Execute User Choice

- **Proceed:** Update state-log.md, start next loop/activity
- **Revise:** Record decision, stay in current loop
- **Re-evaluate:** Return to Phase 2, re-execute full protocol

**Completion Marker: Phase 5 ✓**

---

## Error Handling & Auto-Halt Conditions

**Workflow HALTS immediately if:**

1. Phase 1: state-log.md missing or current_loop invalid
2. Phase 1: Any required review file missing
3. Phase 2: Evaluation completion check fails (< N items evaluated)
4. Phase 2: Any ⊘ N/A item missing documented reason
5. Phase 4: Recording fails (audit.md or state-log.md not writable)
6. Phase 5: User doesn't provide explicit choice (ambiguous response)

**For each HALT:**
- Record in audit.md: "[timestamp] WORKFLOW HALTED: [reason]"
- Present to user: Clear message of what went wrong + how to fix
- Wait for user acknowledgment before retry

---

## Protocol Compliance Checklist

Before declaring review complete, verify:

- [ ] Phase 1: state-log.md loaded and current_loop valid?
- [ ] Phase 1: All required review files exist?
- [ ] Phase 2: All N items evaluated (N=13 for Inception, 11 for Construction)?
- [ ] Phase 2: Every ⊘ N/A has documented reason?
- [ ] Phase 2: Completion count = total count check passed?
- [ ] Phase 3: Verdict calculated (not guessed)?
- [ ] Phase 4: Verdict recorded in state-log.md?
- [ ] Phase 4: Audit entry created in audit.md?
- [ ] Phase 5: User presented with explicit choice?
- [ ] Phase 5: User decision recorded in audit.md?

**If ANY item is NO → Stop and fix before proceeding**

---

## Summary

This protocol is the **concrete implementation** of review-principles:

1. **Phase 1-2** enforce Principle 2 (mandatory checklist evaluation)
2. **Phase 3** enforces Principle 1 (evidence-only verdict)
3. **Phase 4** records everything in audit trail
4. **Phase 5** enforces Principle 3 (no automatic transition; user choice required)

**Result:** Review gate becomes impenetrable because all steps are explicit, verifiable, and recorded.
