---
title: Units Generation
phase: inception
version: 1.0.0
owner: SR-AX-LAB
outputs:
  - ai-sdlc-docs/inception/units/unit-of-work-plan.md
  - ai-sdlc-docs/inception/units/unit-generation.md
  - ai-sdlc-docs/inception/units/unit-of-work-dependency.md
  - ai-sdlc-docs/inception/units/unit-of-work-story-map.md
  - ai-sdlc-docs/inception/units/unit-spec.md
  - ai-sdlc-docs/state/units/{unit-id}-team-status.md
  - ai-sdlc-docs/state/state-log.md
  - ai-sdlc-docs/audit/audit.md
triggers:
  - units generation
  - generate units
  - unit decomposition
allowed-tools:
  - Read
  - Write
  - Edit
---

# Units Generation

## Description

Split approved architecture into implementable units of work, then generate the unit planning and unit definition artifacts needed for downstream execution. Preserve the required approval flow, dependency handling, and team-mode ownership tracking so later stages can proceed without redoing decomposition.

---

## Reference Rules

- `logging/state-management.md` — Use for state-log updates and stage progress tracking.
- `logging/audit-management.md` — Use for audit logging, approvals, and decision records.
- `logging/traceability-management.md` — Use for traceability updates and dependency tracking.

---

## Outputs

| Artifact | Template | Description |
|----------|----------|---------|
| `ai-sdlc-docs/inception/units/unit-of-work-plan.md` | `templates/05-unit-generation.md` | Planning checklist with `[Answer]:` tags and approval request |
| `ai-sdlc-docs/inception/units/unit-generation.md` | `templates/05-unit-generation.md` | Generated unit definitions and ownership summary |
| `ai-sdlc-docs/inception/units/unit-of-work-dependency.md` | `templates/05-unit-generation.md` | Dependency matrix between units |
| `ai-sdlc-docs/inception/units/unit-of-work-story-map.md` | `templates/05-unit-generation.md` | Story or requirement to unit mapping |
| `ai-sdlc-docs/inception/units/unit-spec.md` | `templates/05-unit-generation.md` | Per-unit implementation detail |
| `ai-sdlc-docs/state/units/{unit-id}-team-status.md` | `templates/13-unit-team-status.md` | Team-mode per-unit status artifact |
| `ai-sdlc-docs/state/state-log.md` | (standard) | Record planning and generation progress |
| `ai-sdlc-docs/audit/audit.md` | (standard) | Record questions, approvals, decisions, and timestamps |

---

## Execution Steps

### Step 1: Load Context and Rules

- Read all activity inputs first.
- Load the related common rules before starting.
- Confirm the approved inception context and identify whether decomposition is needed.
- Keep the original planning and generation flow intact.

### Step 2: Build the Unit-of-Work Plan

- Create `ai-sdlc-docs/inception/units/unit-of-work-plan.md` as a checklist.
- Include mandatory artifact checkboxes for unit-generation, dependency matrix, story map, and per-unit spec.
- Add only the questions that change the decomposition decision.
- Embed every user question with a `[Answer]:` tag.

### Step 3: Collect Answers and Resolve Ambiguity

- Ask the user to fill the `[Answer]:` tags directly in the plan.
- Wait until all answer tags are completed.
- Review answers for vague, contradictory, undefined, or combined responses.
- If any ambiguity remains, add follow-up `[Answer]:` tags and do not proceed.

### Step 4: Request and Record Approval

- Ask the user to review the plan and confirm readiness to proceed.
- Do not generate unit artifacts until the user explicitly approves.
- Log the prompt and the response in `ai-sdlc-docs/audit/audit.md` with an ISO 8601 timestamp.
- Update `ai-sdlc-docs/state/state-log.md` to mark planning complete and set the current task to Units Generation.

### Step 5: Generate Unit Artifacts

- Read the approved `unit-of-work-plan.md` in full.
- Identify the next uncompleted step and execute exactly what it describes.
- Generate `unit-generation.md`, `unit-of-work-dependency.md`, `unit-of-work-story-map.md`, and `unit-spec.md`.
- Apply the approved decomposition approach.

### Step 6: Initialize Team-Mode Status Artifacts

- When team mode applies, create one `ai-sdlc-docs/state/units/{unit-id}-team-status.md` file per unit.
- Use `templates/13-unit-team-status.md`.
- Set the unit summary state to `planned` and the automated unit test result to `pending`.

### Step 7: Update Progress and Complete

- Mark completed steps in the plan.
- Update `ai-sdlc-docs/state/state-log.md`, `ai-sdlc-docs/audit/audit.md`, and traceability artifacts by following `logging/state-management.md`, `logging/audit-management.md`, and `logging/traceability-management.md` as work advances.
- Verify that the units are understandable, ownable, and integratable.
- Present the completion message and wait for explicit approval before construction begins.

---

## Approval Gate

- [ ] Review the plan and generated unit artifacts.
- [ ] Verify that every requirement or story is assigned to exactly one unit.
- [ ] Verify that dependencies are explicit and acyclic.
- [ ] Record approval in `audit.md`.
- [ ] Update `state-log.md` with PASS status.
- [ ] Initialize team-mode status artifacts when applicable.

---

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

## SKILL Rules

- Load the related common rules before starting.
- Read all activity inputs first.
- Verify `state-log.md` current status before updating it.
- Record all decisions, approvals, and follow-up questions in `audit.md`.
- Do not fabricate information.
- Do not remove mandatory steps or bypass approval.
- Do not disable audit or traceability updates.
- Do not weaken dependency or ownership rules.
- Do not allow overlapping ownership for the same implementation path.
- Ensure every requirement or story is assigned to exactly one unit.
- Ensure every unit has clear `scope_in`, `scope_out`, `dependencies`, and ownership.
- Ensure dependency graphs have no cycles.
- Follow the Question Rules above when asking or refining decomposition questions.
- If answers are vague or contradictory, add follow-up `[Answer]:` tags instead of guessing.
- In team mode, initialize per-unit status artifacts before construction begins.
- Preserve the approved decomposition behavior and downstream-ready artifact set.




