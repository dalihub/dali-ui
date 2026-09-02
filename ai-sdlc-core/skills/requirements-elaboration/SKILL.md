---
title: Requirements Elaboration
phase: inception
version: 1.0.0
owner: SR-AX-LAB
outputs:
  - ai-sdlc-docs/inception/requirements/requirements-brief.md
  - ai-sdlc-docs/state/state-log.md
  - ai-sdlc-docs/audit/audit.md
triggers:
  - requirements elaboration
  - elaborate requirements
allowed-tools:
  - Read
  - Write
  - Edit
---

# Requirements Elaboration

## Description

Convert the initial user request into a clear, bounded work statement that is specific enough for impact analysis and planning. Produce a requirements brief, record completion status, and log any material decisions before waiting for approval.

---

## Reference Rules

- `logging/state-management.md` — use when updating state records
- `logging/audit-management.md` — use when recording decisions and approval prompts
- `logging/traceability-management.md` — use when updating traceability records
- `common/loop-continuation.md` — preserve context and handle resumption/continuation across workflows

---

## Outputs

| Artifact | Template | Description |
|----------|----------|---------|
| `ai-sdlc-docs/inception/requirements/requirements-brief.md` | `templates/01-requirements-brief.md` | Primary requirements brief |
| `ai-sdlc-docs/state/state-log.md` | (standard) | Record activity completion |
| `ai-sdlc-docs/audit/audit.md` | (standard) | Record decisions and approval prompt |

---

## Execution Steps

### Step 1: Load Available Context

Review the user request and any available supporting context before writing the brief.

### Step 2: Analyze the User Request

Identify the request intent, missing information, constraints, stakeholders, acceptance expectations, and other gaps that materially affect the requirements brief.

If any mandatory information is missing, ask clarifying questions before proceeding. Do not guess or assume missing scope, constraints, stakeholders, or validation details.

### Step 3: Generate Requirements Brief

Create `ai-sdlc-docs/inception/requirements/requirements-brief.md` using `templates/01-requirements-brief.md` as the structural template.

Include only information that is supported by the user request or available context. Keep the output bounded and remove any content that does not map to the template.

### Step 4: Update State, Audit, and Traceability Records

Update the records according to:
- `logging/state-management.md`
- `logging/audit-management.md`
- `logging/traceability-management.md`

Apply the appropriate updates to:
- `ai-sdlc-docs/state/state-log.md`
- `ai-sdlc-docs/audit/audit.md`
- `ai-sdlc-docs/traceability/traceability-matrix.md`

Record completion status and any material decisions made during elaboration.

### Step 5: Present Completion and Pause

Present the standard completion prompt and wait for explicit user approval before continuing to the next workflow stage.

---

## Approval Gate

- [ ] Review the output artifact
- [ ] Verify completeness and accuracy
- [ ] Record approval in `audit.md`
- [ ] Update `state-log.md` with PASS status

---

## SKILL Rules

- Use `templates/01-requirements-brief.md` as the structural template for the output
- Read the available context before drafting the brief
- Do not guess or assume missing business context, scope, constraints, stakeholders, or validation criteria
- Do not fabricate information
- Keep only content that maps to the target template
- Exclude any non-mapped content from the output brief
- Update `state-log.md` after completing the brief
- Update `audit.md` when recording material decisions or the approval prompt
- Update `traceability-matrix.md` when traceability changes are required
- Follow the relevant common management rules before updating state, audit, or traceability artifacts
- Wait for explicit user approval before proceeding

## Question Rules

1. **Ask minimum first** - Start with mandatory questions only
2. **Never guess or assume** - If information is missing, ask
3. **One topic at a time** - Group related questions
4. **Provide options** - Use A/B/C/D format for clarity
5. **Allow "skip"** - User can skip non-critical questions
6. **Allow "not sure"** - Mark as "TBD" and continue
7. **Track gaps** - Record missing information in requirements-brief as "TBD"
8. **Ask only what changes the workflow** - Do not ask for information already present
9. **Keep questions grouped by topic** - Related questions together
10. **Prefer explicit answer choices** - For approval-impacting decisions



