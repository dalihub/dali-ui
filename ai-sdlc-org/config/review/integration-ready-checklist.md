# Integration Ready Checklist

## Purpose
Verify that all required units are ready for integrated build & testing.
Unlike the Inception/Construction review checklists (which verify *deliverable
existence/content*), this checklist verifies **runtime state values** of each
unit. It is **customizable by business unit** to define which conditions are
mandatory and what the required values are.

## How to Use This Checklist

1. **Business Unit Customization**: Define required conditions below and their
   Enforcement Mode (Required / Conditional / Advisory)
2. **Execution**: Before Integration Ready Check, read each unit's live state and
   assert each condition's field == expected value
3. **Per-Unit**: Every condition is evaluated **once per required unit**
4. **Recording**: Mark each (unit × condition) as ✓ (holds), ✗ (violated),
   or ⊘ (N/A with justification)
5. **Review**: integration-ready-check-rule.md uses this checklist to determine
   PASS / BLOCKED / FAIL

---

## Customization Guide: Enforcement Modes

Instead of Level 1/2/3 (used for document checklists), value checks use
**Enforcement Modes** — because the question is "does the field equal the
required value?", not "how deeply do we inspect a file?".

**Required (MUST)**
- Condition MUST hold for every required unit
- Any violation → VERDICT = BLOCKED
- Use when: the condition is a hard gate for integration

**Conditional (COND)**
- Only evaluated when a trigger condition is met (e.g. `team_mode = true`)
- If trigger not met → ⊘ N/A with documented reason
- Use when: the condition applies only in certain modes

**Advisory (WARN)**
- Evaluated and logged, but a violation does NOT block
- Recorded in audit.md as a warning
- Use when: you want visibility without gating

---

## Evaluation Source

All values are read from live tracking files (NOT static deliverables):

| Source File | Provides |
|---|---|
| `ai-sdlc-docs/inception/units/unit-generation.md` | Unit Summary: state, owner |
| `ai-sdlc-docs/state/units/{unit-id}-team-status.md` | test result, blocking items |
| `ai-sdlc-docs/state/state-log.md` | `operating_mode`, unit roster |

---

## Required Conditions (Customize This Section)

### 1. Code Verification Complete

**Field:** `unit.state` (from unit-generation.md Unit Summary)
**Operator:** `==`
**Expected Value:** `code_verified`
**Enforcement Mode:** Required
**Applies To:** every required unit

```yaml
Assertion:
  source: unit-generation.md > Unit Summary > {unit-id}.state
  operator: "=="
  expected: "code_verified"
  on_violation: BLOCKED
```

**Status (per unit):** ☐

---

### 2. Automated Unit Tests Passing

**Field:** `unit.automated_unit_test_result` (from {unit-id}-team-status.md)
**Operator:** `==`
**Expected Value:** `PASS`
**Enforcement Mode:** Required
**Applies To:** every required unit

```yaml
Assertion:
  source: "{unit-id}-team-status.md > automated_unit_test_result"
  operator: "=="
  expected: "PASS"
  on_violation: BLOCKED
```

**Status (per unit):** ☐

---

### 3. No Blocking Items

**Field:** `unit.blocking_items` (from {unit-id}-team-status.md)
**Operator:** `is_empty`
**Expected Value:** `[]`
**Enforcement Mode:** Required
**Applies To:** every required unit

```yaml
Assertion:
  source: "{unit-id}-team-status.md > blocking_items"
  operator: "is_empty"
  expected: []
  on_violation: BLOCKED
```

**Status (per unit):** ☐

---

### 4. Integration Owner Assigned

**Field:** `integration_owner` (from unit-generation.md)
**Operator:** `is_assigned`
**Expected Value:** non-empty session/owner id
**Enforcement Mode:** Conditional
**Trigger:** Only if `operating_mode == "team"` in state-log.md
**Applies To:** project-level (evaluated once, not per unit)

```yaml
Assertion:
  condition: state-log.md > operating_mode == "team"
  source: unit-generation.md > integration_owner
  operator: "is_assigned"
  on_violation: BLOCKED
  when_trigger_false: "⊘ N/A (single-developer or non-team mode)"
```

**Status:** ☐

---

## Verdict Determination

### PASS Conditions
- Every **Required** condition holds (✓) for **all** required units
- Every **Conditional** condition either holds or is ⊘ N/A with justification
- **Advisory** violations may exist (logged, non-blocking)

### BLOCKED Conditions
- One or more **Required** conditions are ✗ for any unit
- OR a **Conditional** condition is triggered but violated

### FAIL Conditions
- Evidence contradicts itself (e.g. state = code_verified but test_result = FAIL)
- OR user explicitly rejects integration readiness

---

## Checklist Completion Record (Per Unit)

| Unit ID | 1. code_verified | 2. tests PASS | 3. no blockers | Unit Result |
|---------|:---:|:---:|:---:|:---:|
| unit-A  |  |  |  |  |
| unit-B  |  |  |  |  |
| ...     |  |  |  |  |

**Project-Level Conditions**

| # | Condition | Mode | Status | Notes |
|---|---|---|:---:|---|
| 4 | Integration Owner assigned | Conditional | | (team mode only) |

**Aggregate:** `all_units_pass_integration_ready_check = [true/false]`

---

## AI Evaluation

integration-ready-check-rule.md loads this checklist and, for each required unit:
1. Read the source field from the live tracking file
2. Apply the operator against the expected value
3. Mark ✓ (holds) / ✗ (violated) / ⊘ (N/A with justification)
4. After all units: if any Required ✗ → BLOCKED; if contradiction → FAIL;
   else PASS
5. Write `integration_ready_check_verdict` to state-log.md and `INT-READY-###`
   to audit.md

---

## Adding Custom Conditions

To add a 5th condition, follow this template:

```markdown
### 5. [Your Condition Name]

**Field:** `unit.[field]`  (source file)
**Operator:** [== / != / >= / is_empty / is_assigned / matches]
**Expected Value:** [value]
**Enforcement Mode:** [Required / Conditional / Advisory]
**Applies To:** [every required unit / project-level]

\`\`\`yaml
Assertion:
  source: "..."
  operator: "..."
  expected: ...
  on_violation: [BLOCKED / WARN]
\`\`\`

**Status:** ☐
```

Then update the Completion Record tables above.
