# Base Workflow

## Purpose

Base Workflow defines the standard workflow paths, execution sequences, and review checkpoints 
that organize the activities defined in `activity.md` (in the same config/workflow/ directory). Path Finder uses this to 
generate organization-tailored workflows.

**Reference**: See `activity.md` for detailed activity definitions (A0-A5, C1-C5).

---

# Activity Sequence for organization
A Workflow is selected based on workspace state and work type.

## Standard Workflow
**profile_id** : org_standard
**execution sequence**:
```
Inception Loop:
A0 → A1 → A2 → A3 → A4 → A5

[INCEPTION REVIEW]

Construction Loop:
C1 → C2 → C3 → C4 → C5 

[CONSTRUCTION REVIEW]
```

**Sequential Execution**:
- Activities execute in listed order
- Review checkpoints are gates (PASS required to proceed)

## Advanced development

**Profile_id**: org_advanced_sw

**Execution Sequence**:
```
Inception Loop:
A1 → A2 → A3 → A4 → A5[COND]

[INCEPTION REVIEW]

Construction Loop:
C1 → C2 → C3 → C4 → C5 

[CONSTRUCTION REVIEW]
```

**Path Characteristics**:
- No reverse engineering (A0 skipped)
- All inception activities required for new architecture
- Complete construction loop required
- 4 weeks typical timeline

---

# Activity Execution Markers

**Reference**: See [[glossary]] > "Execution Markers" for detailed marker definitions (MUST, COND, SKIP).

---

# Review Checkpoints

## Inception Review

**Trigger**: After Inception Loop completion (A0-A5)
**Gate**: PASS / BLOCKED / FAIL (PASS required to proceed to Construction)
**Purpose**: Verify inception activities are complete and design is ready
**Executor**: ../review/inception-review-checklist.md

---

## Construction Review

**Trigger**: After Construction Loop completion (C1-C5)
**Gate**: PASS / BLOCKED / FAIL (PASS required for release)
**Purpose**: Verify implementation quality and readiness for release
**Executor**: ../review/construction-review-checklist.md

---

## Integration Ready Check

**Trigger**: After C3 (Code Verification), before C4 (Build & Testing)
**Purpose**: Verify code is ready for integrated build
**Executor**: ai-sdlc-core/review/integration-ready-check-rule.md
**Checklist**: ../review/integration-ready-checklist.md

---
