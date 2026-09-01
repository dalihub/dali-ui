---
title: Workflow Planning
phase: inception
version: 1.0.0
owner: SR-AX-LAB
outputs:
  - ai-sdlc-docs/inception/workflow/workflow-planning.md
  - ai-sdlc-docs/state/state-log.md
  - ai-sdlc-docs/audit/audit.md
triggers:
  - workflow planning
  - plan workflow
  - plan execution
allowed-tools:
  - Read
  - Write
  - Edit
  - AskUserQuestion
---

# Workflow Planning

Owner: SR-AX-LAB

## Description

Choose the lightest executable workflow path that preserves quality, traceability, and delivery control.

---

## Reference Rules

- `common/ai-execution-rule.md` — use for model selection when multiple models are available
- `logging/state-management.md` — use for state updates
- `logging/audit-management.md` — use for audit log updates
- `logging/traceability-management.md` — use for traceability updates

---

## Outputs

| Artifact | Template | Description |
|----------|----------|---------|
| `ai-sdlc-docs/inception/workflow/workflow-planning.md` | `templates/03-workflow-planning.md` | Primary output artifact |
| `ai-sdlc-docs/state/state-log.md` | (standard) | Record workflow completion and operating decisions |
| `ai-sdlc-docs/audit/audit.md` | (standard) | Record decisions and approval status |

---

## Execution Steps

### Step 1: Review Requirements Brief Initial Assessment

- Load `ai-sdlc-docs/inception/requirements/requirements-brief.md`
- Review the **Initial Assessment** section:
  - Scope Estimate
  - Complexity Estimate
  - Depth Level (preliminary)
- Review the **Stakeholders** section for key contacts

### Step 2: Confirm Developer Count

- Ask the user: "How many developers will participate in implementation?"
- Stop until the user provides an explicit answer
- Record the confirmed developer count in `workflow-planning.md`

### Step 3: Determine Depth and Operating Mode

- Review the preliminary depth level from the requirements brief
- Confirm or adjust depth based on the available analysis findings
- Select operating mode: single-developer or team mode
- Keep the operating mode consistent with the confirmed developer count
- Record the decision in `workflow-planning.md`

### Step 4: Select Workflow Path

- Select the exact inception and construction path based on scope, complexity, and workspace mode
- Identify which tasks will run and which may be skipped
- Record the chosen path in `workflow-planning.md`

### Step 5: Identify Planned Outputs and Approvals

- List all expected artifacts
- Identify required approvals
- Define the validation strategy and release expectation
- Record the plan in `workflow-planning.md`

### Step 6: Define Ownership and Responsibilities

- Define ownership for implementation, automated validation, environment provisioning, and integration
- Define integration entry gate conditions for multi-unit execution
- Define the multi-unit completion synchronization rule
- Record the ownership model in `workflow-planning.md`

### Step 7: Update State, Audit, and Traceability Logs

- Update `ai-sdlc-docs/state/state-log.md` according to `logging/state-management.md`
- Update `ai-sdlc-docs/audit/audit.md` according to `logging/audit-management.md`
- Update `ai-sdlc-docs/traceability/traceability-matrix.md` according to `logging/traceability-management.md`
- If team mode is selected, record operating mode, workflow id, and github repository in `state-log.md`

---

## Approval Gate

- Review the output artifact
- Verify completeness and accuracy
- Record approval in `audit.md`
- Update `state-log.md` with PASS status

---

## Skill Rules

- Load the related common rules before starting
- Read all activity inputs first
- Verify `state-log.md` current status
- Record all decisions in `audit.md`
- Do not fabricate information
- Ask for the developer count before proceeding
- Do not proceed without an explicit developer-count answer
- Keep the operating mode consistent with the confirmed developer count
- Do not remove mandatory steps or approvals
- Do not bypass audit, state, or traceability updates
- Do not weaken the workflow gate
- Do not add traceability or work-type content that is not supported by this source


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



