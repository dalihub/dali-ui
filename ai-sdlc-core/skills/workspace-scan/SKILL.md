---
title: Workspace Scan Skill
phase: pre-inception
version: 1.0.0
owner: SR-AX-LAB
source_file: workspace-scan/SKILL.md
triggers:
  - "workspace scan"
  - "check workspace status"
  - "determine artifact status"
allowed-tools:
  - Read
  - Write
  - Edit
  - Bash
  - AskUserQuestion
---

# Workspace Scan Skill

## Purpose

Scan the workspace to determine artifact status before executing AI-SDLC main workflow. This scan assesses:
1. Code existence (code present or not)
2. Documentation existence (ai-sdlc-docs/ present or not)
3. Code-docs alignment (are they in sync?)

And produces one of 5 artifact status values:

1. **NEW**: Code X, Docs X (new project, no code and no docs)
2. **ALIGNED**: Code O, Docs O, code = docs (perfectly aligned)
3. **STALE (mismatch)**: Code O, Docs O, code ≠ docs (misaligned)
4. **STALE (incomplete)**: Code X, Docs O (design complete, implementation not started)
5. **STALE (undocumented)**: Code O, Docs X (existing code, no documentation)

This prevents integration failures from misaligned or outdated documentation.

---

## When to Execute

- Before executing AI-SDLC main workflow
- At the beginning of any workflow to determine current state

---

## Outputs

| Artifact | Location | Description |
|----------|----------|---------|
| state-log.md | `ai-sdlc-docs/state/state-log.md` | Workspace Scan Summary with artifact status |
| audit.md | `ai-sdlc-docs/audit/audit.md` | Workspace Scan Decision with findings and rationale |

---

## Execution Steps

### Step 1: Check ai-sdlc-docs/ Existence

Does `ai-sdlc-docs/` exist in the workspace?

**Action:**
- List `ai-sdlc-docs/` directory structure
- If NO → Artifact Status: NEW, proceed to Step 5
- If YES → Proceed to Step 2

---

### Step 2: Code Existence Check (Only if ai-sdlc-docs/ EXISTS)

Does code exist in the workspace?

**Action:**
- Check if code exists in the workspace
- If code missing → Artifact Status: STALE (incomplete), proceed to Step 5
- If code exists → Proceed to Step 3

---

### Step 3: Drift Detection (Only if Code AND Docs BOTH EXIST)

When both code and docs exist, perform drift detection to verify documented artifacts still match current codebase.

#### 3a. Architecture Drift

**Question:** Does the codebase still match the documented architecture?

**Check:**
- Read: `ai-sdlc-docs/inception/architecture/architecture-design.md`
- Verify in codebase:
  - Component structure: Are documented components still present?
  - Layer organization: Do layers match (e.g., 3-layer vs. 4-layer)?
  - Dependencies: Are documented dependencies still valid?
  - Service boundaries: Have microservice boundaries changed?

**Action if drift detected:**
- Record component/layer changes
- Note which components have been added/removed/merged
- Mark as "Architecture Drift: DETECTED"

#### 3b. Unit Drift

**Question:** Do the documented units/modules correspond to actual code organization?

**Check:**
- Read: `ai-sdlc-docs/inception/units/unit-generation.md` → Unit Summary table
- Verify in codebase:
  - File mappings: Are files still in documented locations?
  - Unit boundaries: Do unit boundaries match actual module/service separation?
  - File count: Has the number of files per unit changed significantly?
  - Module organization: Do modules still exist as documented?

**Action if drift detected:**
- Identify which units have changed (boundary, scope, file count)
- Record specific changes (files added/removed, units merged/split)
- Mark as "Unit Drift: DETECTED"

#### 3c. Traceability Drift

**Question:** Are the files referenced in documentation still present in the codebase?

**Check:**
- Read: `ai-sdlc-docs/inception/requirements/requirements-analysis.md` and `architecture/architecture-design.md`
- Scan: All file references in reverse-index.md or architecture document
- Verify in codebase:
  - File existence: Does each referenced file still exist?
  - File location: Has the file path changed?
  - File content: Is the file still responsible for the documented concern?

**Action if drift detected:**
- Identify broken file references
- Note file relocations or renames
- Mark as "Traceability Drift: DETECTED"

#### 3d. RE Artifact Drift

**Question:** Does the reverse-engineering artifact (reverse-index.md) still reflect current codebase?

**Check:**
- Read: `ai-sdlc-docs/inception/reverse-engineering/reverse-index.md` (if exists)
- Verify: Does it still accurately summarize the codebase structure?
  - Is the reverse-index summary still valid?
  - Are documented patterns/conventions still observed?
  - Are major components/files still present?
  - Do relationships (dependencies, integrations) still hold?

**Action if drift detected:**
- Note reverse-index is outdated
- Record estimated codebase change volume (commits, refactorings)
- Mark as "RE Artifact Drift: DETECTED"

---

### Step 4: Drift Detection Summary & Decision

#### If NO drift is detected (Code AND Docs both exist)

**Artifact Status: ALIGNED (code = docs)**

**Action:**
- Record in audit.md artifact freshness check PASS
- Context Analysis: OPTIONAL
- Use existing artifacts for downstream activities

#### If Docs exist but code missing

**Artifact Status: STALE (incomplete)**

**Action:**
- Code does not exist, but documentation exists
- Design is complete but implementation not started
- Context Analysis: NOT NEEDED
- Proceed to Requirements Elaboration and implementation

#### If Code exists but docs missing

**Artifact Status: STALE (undocumented)**

**Action:**
- Code exists but documentation missing
- Implementation exists but not documented
- Context Analysis: STRONGLY RECOMMENDED
  - Reverse-engineer current code structure
  - Create documentation for existing code
- Then proceed to Requirements Elaboration with documented baseline

#### If Code and Docs exist but don't match

**Artifact Status: STALE (mismatch)**

**Action:**
- Record in audit.md which drifts detected
- Context Analysis: STRONGLY RECOMMENDED (but CONDITIONAL)
- User decides whether to execute reverse-engineering before Requirements Elaboration
- Update state-log.md with user's choice

---

### Step 5: Record Scan Results

#### Record in state-log.md

```markdown
## Workspace Scan Summary

**Executed:** [YYYY-MM-DD HH:MM]
**Artifact Status**: [NEW | ALIGNED | STALE (mismatch) | STALE (incomplete) | STALE (undocumented)]

### Code & Docs Status
- Code exists: [YES | NO]
- Docs exist: [YES | NO]

### Findings (if STALE)
- Architecture drift: [NONE | DETECTED]
- Unit drift: [NONE | DETECTED]
- Traceability drift: [NONE | PARTIAL | DETECTED]
- RE artifact drift: [NONE | DETECTED]

### Context Analysis Decision
- If NEW: "Not applicable, proceed to Requirements Elaboration"
- If ALIGNED: "Context Analysis optional"
- If STALE (mismatch): "Context Analysis strongly recommended"
- If STALE (incomplete): "Not needed, code does not exist"
- If STALE (undocumented): "Context Analysis strongly recommended"

### Details
[Findings summary and next steps]
```

#### Record in audit.md

```markdown
### DEC-### — Workspace Scan Decision

**When:** [date]
**Artifact Status**: [NEW | ALIGNED | STALE (mismatch) | STALE (incomplete) | STALE (undocumented)]

**Code & Docs Status:**
- Code exists: [YES | NO]
- Docs exist: [YES | NO]

**Scope Evaluated:**
- Code existence
- Documentation (ai-sdlc-docs/) existence
- Code-docs alignment (if both exist)
- Architecture consistency (if both exist)
- Unit/module structure (if both exist)
- File/artifact references (if both exist)
- Reverse-engineering artifact age and validity (if docs exist)

**Findings:**
[Summary of status and drift detection results]

**Stale Type Details** (if applicable):
- mismatch: Code and docs both exist but don't match
- incomplete: Code missing, docs exist
- undocumented: Code exists, docs missing

**Context Analysis Decision:**
- Not applicable (if NEW or STALE incomplete)
- Optional (if ALIGNED)
- Strongly recommended (if STALE mismatch or STALE undocumented)

**Rationale:**
[Brief explanation of why this decision]

**Next Step:**
- Proceed based on artifact status and user decision on Context Analysis
```

---

## Decision Matrix

| Code | Docs | Status | Artifact Status | Context Analysis |
|------|------|--------|-----------------|------------------|
| X | X | new | NEW | N/A |
| O | O | aligned | ALIGNED | OPTIONAL |
| O | O | misaligned | STALE (mismatch) | STRONGLY_RECOMMENDED |
| X | O | incomplete | STALE (incomplete) | NOT_NEEDED |
| O | X | undocumented | STALE (undocumented) | STRONGLY_RECOMMENDED |

---

## Execution Checklist

Use this checklist during workspace scan:

```
□ Step 1: ai-sdlc-docs/ Existence Check
  □ Check if directory exists
  □ If NO → Artifact Status: NEW

□ Step 2: Code Existence Check (if ai-sdlc-docs/ EXISTS)
  □ Check if code exists
  □ If NO → Artifact Status: STALE (incomplete), skip to Step 5
  □ If YES → Proceed to Step 3

□ Step 3a: Architecture Drift (if Code AND Docs exist)
  □ Read architecture-design.md
  □ Verify component structure in codebase
  □ Check layer organization
  □ Verify documented dependencies
  □ Document any discrepancies

□ Step 3b: Unit Drift (if Code AND Docs exist)
  □ Read unit-generation.md Unit Summary
  □ Verify file mappings in codebase
  □ Check unit boundaries
  □ Compare file counts
  □ Document any boundary/scope changes

□ Step 3c: Traceability Drift (if Code AND Docs exist)
  □ Scan for file references
  □ Verify each file still exists
  □ Check for file relocations
  □ Document broken references

□ Step 3d: RE Artifact Drift (if Code AND Docs exist)
  □ Read reverse-index.md (if exists)
  □ Estimate codebase change volume
  □ Assess reverse-index currency
  □ Document age and change impact

□ Step 4: Drift Summary & Decision
  □ Summarize drift detection results
  □ Determine artifact status (NEW / ALIGNED / STALE with type)
  □ Identify STALE type: mismatch / incomplete / undocumented

□ Step 5: Record Results
  □ Record in state-log.md
  □ Record decision in audit.md
  □ Determine Context Analysis decision
```

---

## Notes for Execution

- **Manual Verification:** If AI cannot automatically verify file existence or codebase structure, request user input or access to codebase
- **Partial Drift:** If only some components show drift, document which ones and proceed with full re-engineering (all or nothing)
- **Timestamp Check:** Use git log to estimate codebase age relative to ai-sdlc-docs/ last modification time
- **Audit Trail:** Always record the workspace scan decision in audit.md with clear reasoning
- **NEW Status:** No code, no docs. Context Analysis not applicable. Proceed to Requirements Elaboration.
- **ALIGNED Status:** Code and docs exist and match. Context Analysis is OPTIONAL. User can request it for additional context.
- **STALE (mismatch):** Code and docs exist but don't match. Context Analysis STRONGLY RECOMMENDED. Reverse-engineer current code structure.
- **STALE (incomplete):** Code missing, docs exist. Context Analysis NOT NEEDED. Proceed to Requirements Elaboration and start implementation.
- **STALE (undocumented):** Code exists, docs missing. Context Analysis STRONGLY RECOMMENDED. Reverse-engineer and document existing code structure.
