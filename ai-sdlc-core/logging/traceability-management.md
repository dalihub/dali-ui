---
title: Traceability Management
rule_id: TRACEABILITY_MANAGEMENT_RULE
phase: common
version: 1.0.0
owner: SR-AX-LAB
criticality: mandatory
source_file: ai-sdlc-core/logging/traceability-management.md
outputs:
  - ai-sdlc-docs/traceability/traceability-matrix.md (canonical requirement→code→test mapping)
---

# Traceability Management

## Purpose

Define end-to-end requirement traceability from REQ → AC → Architecture → Code → Test, with decision tracking and coverage metrics. The traceability matrix is the **canonical handoff artifact** linking requirements to implementation and tests.

---

## Reference Rules

- `ai-sdlc.md` — Workflow orchestration (Steps 3-4 manage traceability)
- `loop-continuation.md` — Prior requirement inheritance in continuation workflows
- `audit-management.md` — Audit recording for traceability updates
- Template: `templates/19-traceability-matrix.md` (structure & validation checklist)

---

## Traceability Model

**Core Mapping:** REQ → AC → Architecture Component → Code (file:symbol) → Test

| Component | Format | Purpose |
|-----------|--------|---------|
| REQ | `REQ-001` | User or system requirement |
| AC | `AC-001` | Verifiable acceptance criteria |
| Architecture | Component name | Responsible module |
| Code | `file.ts:function` | Implementation symbol |
| Test | test name + evidence label | Verification (executed-pass, code-review-only, etc.) |

**Decision Impact:** DEC-### entries from audit.md that materially shaped implementation must be recorded in traceability matrix.

---

## Lifecycle Overview

**Building Phase** (A2→C4): Each activity populates one column
- **A2 (Requirements Analysis):** Add REQ and AC columns
- **A3 (Architecture Design):** Add Architecture column
- **C1 (Code Generation):** Add Code column
- **C3 (Code Verification):** Add Test column (unit test evidence)
- **C4 (Build Testing):** Update Test column (integration results)

**Finalization Phase** (Release Step 3-4): Validate and complete
- Populate Decision Impact section
- Calculate coverage metrics (REQ/AC/Code/Test coverage %)
- Add optional sections if applicable (Unit Coverage, Prior REQ Traceability, DEFECT_FIX Addendum)
- Pass validation checklist before marking complete (see template Section 7)

---

## Activity Update Points

Each SKILL responsible for traceability updates at its phase:

| Activity | SKILL | Step | Updates | Validation |
|----------|-------|------|---------|------------|
| Requirements Analysis | requirements-analysis/SKILL.md | 7 | REQ + AC | Every REQ has AC |
| Architecture Design | architecture-design/SKILL.md | 8 | Architecture | Every AC has component |
| Code Generation | code-generation/SKILL.md | 7 | Code (file:symbol) | All file refs exist |
| Code Verification | code-verification/SKILL.md | 9 | Test (unit evidence) | All tests documented |
| Build Testing | build-testing/SKILL.md | 9 | Test (integration) | Integration status updated |
| Release | release/SKILL.md | 3 | Decision Impact + metrics | All sections complete |

**Update Pattern:**
1. Load traceability-matrix.md
2. Populate assigned column for affected ACs
3. Verify no broken references
4. Record update in audit.md: "traceability-matrix: [COLUMN] for [COUNT] ACs"

---

## Mandatory Constraints

**Never:**
- ❌ Leave a cell blank without documenting N/A reason
- ❌ Reference code files or tests that don't exist
- ❌ Mark Test as `executed-pass` without execution evidence
- ❌ Skip finalization or validation (traceability is a mandatory gate)
- ❌ Flatten multi-unit traceability (preserve unit-level trace)

**Always:**
- ✅ Populate columns as activities complete
- ✅ Use evidence labels from glossary.md (executed-pass, code-review-only, manual-pass, not-applicable)
- ✅ Record DEC-### decision impact entries that shaped implementation
- ✅ Validate code/test references exist in repository
- ✅ Keep traceability matrix in sync with state-log.md and audit.md

---

## Summary

**traceability-management.md** defines:
1. **Model**: REQ→AC→Architecture→Code→Test with DEC-### tracking
2. **Lifecycle**: Incremental building (A2-C4), then finalization (Release)
3. **Activity Points**: Which SKILL updates which column and when
4. **Constraints**: Rules for maintaining traceability integrity
5. **Validation**: Checklist in template Section 7 before release

The traceability matrix is the **single source of truth** linking requirement → design → code → test. It is mandatory for all workflow completions and is audited at Step 4 before release.
