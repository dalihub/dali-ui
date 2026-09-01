---
title: Review Rule
version: 1.0.0
owner: SR-AX-LAB
description: Execute review gate for Inception or Construction loop. Enforces 3 core principles, 5-phase protocol, and loop-specific checklist. Evidence-based verdict and explicit user choice before loop transition.
source_file: ai-sdlc-core/review/review-rule.md
triggers:
  - After all inception activities complete (loop=inception)
  - After all construction activities complete (loop=construction)
depends_on:
  - ./review-protocol.md
  - {org-dir}/config/review/inception-review-checklist.md or construction-review-checklist.md
  - ai-sdlc-docs/state/state-log.md
allowed-tools:
  - Read
  - Write
  - Edit
  - AskUserQuestion
---

# Review Rule

## Core Principles (MANDATORY)

**These three principles MUST govern all review decisions. No exceptions.**

### Principle 1: Evidence-Only Verdict

Review verdicts are determined **ONLY by evidence**, not by assumptions, reasoning, confidence, or timeline pressure.

**PASS:** Evidence exists and satisfies all checklist requirements. No gaps. No contradictions.  
**BLOCKED:** Required evidence is missing or insufficient.  
**FAIL:** Evidence exists but contradicts requirements, or user explicitly rejects output.

**Invalid verdict basis:**
- ❌ "I think this is correct"
- ❌ "The code looks good"
- ❌ "No one objected"
- ❌ "We don't have time for full review"

**Only use actual evidence from ai-sdlc-docs/ artifacts.**

### Principle 2: Mandatory Checklist Evaluation

All required deliverables **MUST be evaluated**. 100% completion required. Not a single item can be skipped.

See review-protocol.md Phase 2 for detailed evaluation steps.

**Before declaring verdict:**
- All deliverables evaluated?
- Every ⊘ N/A has documented reason?
- If NO → evaluation incomplete, HALT

### Principle 3: No Automatic Transition

A **PASS verdict does NOT automatically advance to the next loop or close the workflow**.

**User control at every gate:**
- AI MUST NOT advance/close automatically
- AI MUST present user with explicit choices after PASS
- User MUST confirm next action in writing
- Decision MUST be recorded in audit.md

This preserves user control and awareness at every transition.

---

## Purpose

Execute a complete review gate for the current loop (Inception or Construction). This skill:
1. Enforces 3 core principles above
2. Loads and executes 5-phase review protocol
3. Evaluates loop-specific checklist
4. Records verdict and enforces explicit user choice before loop transition

---

## Execution Flow

### Step 1: Determine Current Loop & Load Protocol

1. **Read state-log.md** → Determine loop (inception or construction)
2. **Load review-protocol.md**
   - Understand 5-phase protocol definition
   - Understand auto-halt conditions
   - Understand user choice scenarios

### Step 2: Load Loop-Specific Checklist

**If inception:**
- Load {org-dir}/config/review/inception-review-checklist.md
- Required deliverables for Inception loop

**If construction:**
- Load {org-dir}/config/review/construction-review-checklist.md
- Required deliverables for Construction loop

Where `{org-dir}` is the discovered organization directory (see path-finder.md for dynamic resolution).

Both checklists are business-unit customizable (Level 1/2/3).

### Step 3: Execute 5-Phase Protocol

**Phase 1: Preparation**
- Verify state-log.md exists and current_loop is valid (inception or construction)
- Verify review-protocol.md and corresponding checklist exist
- HALT if any file missing or state invalid

**Phase 2: Checklist Evaluation**
- Create evaluation table with columns: Item#, Deliverable, Check_Level, Status, Evidence_Path, Notes
- For each required deliverable in checklist:
  - Read requirement and check level (Level 1/2/3)
  - Check condition (if conditional: e.g., "if multi_unit = true")
  - Locate evidence in ai-sdlc-docs/
  - Determine status: ✓ COMPLETE, ✗ MISSING, ⊘ N/A (with reason)
- Completion check: all items evaluated? Every ⊘ has reason?
- HALT if evaluation incomplete or any ⊘ lacks justification

**Phase 3: Verdict Determination**
- Count ✓, ✗, ⊘ deliverables evaluated
- Auto-calculate verdict:
  - If ✗_count > 0 → VERDICT = BLOCKED (evidence missing)
  - Else if ✓_count + ⊘_count_justified = total → VERDICT = PASS
  - Special case: If evidence contradicts requirement → VERDICT = FAIL

**Phase 4: Recording**
- Update state-log.md:
  - **If inception:** inception_review_verdict = [PASS/BLOCKED/FAIL]
  - **If construction:** construction_review_verdict = [PASS/BLOCKED/FAIL]
  - Timestamp (ISO 8601)
- Create audit.md entry:
  - **If inception:** INC-REV-001
  - **If construction:** CON-REV-001
  - Record timestamp, phase, verdict, evaluation summary, reason, missing items, recovery actions
- HALT if recording fails

**Phase 5: User Choice**
- Present verdict to user
- **If inception + PASS:** Offer "Proceed to Construction" / "Revise artifacts" / "Additional setup (team mode)"
- **If inception + BLOCKED/FAIL:** Offer "Review issues" / "Revise artifacts" / "Re-evaluate"
- **If construction + PASS:** Offer "Close workflow" / "Review residual risks" / "Revise artifacts"
- **If construction + BLOCKED/FAIL:** Offer "Review issues" / "Revise artifacts" / "Re-evaluate"
- Wait for explicit user response (not ambiguous)
- Record user choice in audit.md: USR-TRANS-[001]
- Execute user choice:
  - **Proceed/Close:** Update state-log.md, transition loop or close workflow
  - **Revise:** Record decision, stay in current loop
  - **Re-evaluate:** Return to Phase 2, re-execute full protocol

---

## Auto-Halt Conditions

Workflow **HALTS immediately** if any of these occur:

1. **Phase 1**
   - state-log.md missing or loop invalid
   - review-protocol.md missing
   - Loop-specific checklist missing

2. **Phase 2**
   - Number of evaluated deliverables ≠ total in checklist
   - Any ⊘ N/A item without documented reason

3. **Phase 4**
   - state-log.md not writable
   - audit.md not writable
   - Recording fails for any reason

4. **Phase 5**
   - User response is ambiguous or does not match available options
   - User does not select explicit option

**For each HALT:**
- Record in audit.md: "[timestamp] WORKFLOW HALTED: [reason]"
- Present clear error message to user
- Provide recovery instruction
- Wait for user acknowledgment

---

## Non-Compliance: Critical Violations

These actions are **NOT allowed** and will cause audit flag:

- ❌ Starting Phase 2 without loading Phase 1 preparation
- ❌ Declaring verdict without evaluating all required deliverables
- ❌ Marking item ✓ COMPLETE without evidence verification
- ❌ Marking item ⊘ N/A without condition check or documented reason
- ❌ Recording verdict without audit trail entry
- ❌ Advancing/closing without explicit user confirmation (PASS verdict)
- ❌ Using reasoning, confidence, or timeline pressure as verdict basis (Principle 1)
- ❌ Skipping any evaluation item (Principle 2)
- ❌ Skipping Phase 5 user choice (Principle 3)

---

## Key Enforcements

✅ **Always enforced (3 Principles):**
- **Principle 1:** Evidence-only verdict (no reasoning, confidence, timeline pressure)
- **Principle 2:** All required deliverables evaluated (100%, no skipping)
- **Principle 3:** User explicit choice required (no auto-transition)
- Audit trail complete (every decision recorded)

✅ **Auto-halt on violation:**
- Incomplete evaluation (Principle 2)
- Missing evidence (Principle 1)
- Verdict based on non-evidence (Principle 1)
- Ambiguous user response (Principle 3)
- Recording failure

✅ **Verdict is calculated, not guessed:**
```
if (missing_deliverables > 0) BLOCKED
else if (complete + justified_N/A == total) PASS
else FAIL (if contradiction detected)
```

---

## Summary

This skill enforces **3 core principles** through **5-phase protocol** for **Inception or Construction loop**:

1. **Principles (this file)** = WHY reviews work this way (Evidence-Only, Mandatory Evaluation, No Auto-Transition)
2. **Protocol (review-protocol.md)** = HOW to execute reviews (5 phases with auto-halt)
3. **Checklist (ai-sdlc-org/config/review)** = WHAT to evaluate (business-unit-defined required deliverables)
4. **Skill (this file)** = WHERE & WHEN it runs (After all loop activities complete)

**Result:** Impenetrable review gate with explicit evidence, complete evaluation, and explicit user control before any loop transition.
