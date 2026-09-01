---
title: Reverse Engineering
phase: inception
version: 1.0.0
owner: SR-AX-LAB
outputs:
  - ai-sdlc-docs/inception/reverse-engineering/reverse-index.md
  - ai-sdlc-docs/inception/reverse-engineering/architecture.md
  - ai-sdlc-docs/inception/reverse-engineering/component-inventory.md
  - ai-sdlc-docs/inception/reverse-engineering/technology-stack.md
  - ai-sdlc-docs/inception/reverse-engineering/dependencies.md
  - ai-sdlc-docs/inception/reverse-engineering/code-structure.md
  - ai-sdlc-docs/inception/reverse-engineering/build-test-config.md
  - ai-sdlc-docs/inception/reverse-engineering/business-overview.md
  - ai-sdlc-docs/inception/reverse-engineering/api-documentation.md
  - ai-sdlc-docs/inception/reverse-engineering/data-model.md
  - ai-sdlc-docs/state/state-log.md
  - ai-sdlc-docs/audit/audit.md
  - ai-sdlc-docs/traceability/traceability-matrix.md
triggers:
  - reverse engineering
  - brownfield analysis
  - reverse-index
allowed-tools:
  - Read
  - Write
  - Edit
  - Bash
  - Glob
  - Grep
  - WebFetch
  - WebSearch
---

# Reverse Engineering

## Description

Inspect an existing brownfield workspace to discover facts about the current codebase, infrastructure, architecture, dependencies, technology stack, build and test commands, and data or API surfaces so downstream tasks can proceed safely without guessing.

---

## Reference Rules

- `common/ai-execution-rule.md` — use when the codebase is large enough to benefit from parallel subagent exploration
- `logging/state-management.md` — use when updating `state-log.md`
- `logging/audit-management.md` — use when updating `audit.md`
- `logging/traceability-management.md` — use when updating `traceability-matrix.md`

---

## Outputs

| Artifact | Template | Description |
|----------|----------|---------|
| `ai-sdlc-docs/inception/reverse-engineering/reverse-index.md` | `templates/15-reverse-index.md` | Single source of truth for artifact status, depth decision, and used-by mapping |
| `ai-sdlc-docs/inception/reverse-engineering/architecture.md` | (none) | System architecture, boundaries, and communication patterns |
| `ai-sdlc-docs/inception/reverse-engineering/component-inventory.md` | (none) | Component list with purpose and responsibilities |
| `ai-sdlc-docs/inception/reverse-engineering/technology-stack.md` | (none) | Language, framework, runtime, and package manager details |
| `ai-sdlc-docs/inception/reverse-engineering/dependencies.md` | (none) | Internal and external dependency mapping |
| `ai-sdlc-docs/inception/reverse-engineering/code-structure.md` | (none) | Directory layout, entry points, and file organization |
| `ai-sdlc-docs/inception/reverse-engineering/build-test-config.md` | `templates/17-build-test-config.md` | Exact build, test, lint, CI, and environment commands |
| `ai-sdlc-docs/inception/reverse-engineering/business-overview.md` | (none) | Create only when business context is needed; otherwise mark `not_applicable` in `reverse-index.md` |
| `ai-sdlc-docs/inception/reverse-engineering/api-documentation.md` | (none) | Create only when interfaces or APIs are relevant; otherwise mark `not_applicable` in `reverse-index.md` |
| `ai-sdlc-docs/inception/reverse-engineering/data-model.md` | `templates/16-data-model.md` | Create only when persistent data storage is present; otherwise mark `not_applicable` in `reverse-index.md` |
| `ai-sdlc-docs/state/state-log.md` | (standard) | Record completion status |
| `ai-sdlc-docs/audit/audit.md` | (standard) | Record decisions, depth, and skip rationale |
| `ai-sdlc-docs/traceability/traceability-matrix.md` | `ai-sdlc-docs/traceability/traceability-matrix.md` | Preserve requirement-to-test mapping for downstream handoff |

---

## Execution Steps

### Step 1: Determine Depth and Scope

Assess the workspace state and decide whether full reverse engineering is required, which artifacts are mandatory, which are conditional, and whether any prior reverse-engineering outputs can be reused. Record the depth decision in `reverse-index.md`.

### Step 2: Create the Reverse Index

Create `ai-sdlc-docs/inception/reverse-engineering/reverse-index.md` first. List every reverse-engineering artifact with one of these statuses:

- `pending` — not yet created
- `created` — created and populated
- `not_applicable` — condition not met, with rationale
- `skipped` — intentionally skipped, with rationale

Include the used-by mapping for each artifact. Treat this file as the single source of truth for reverse-engineering coverage.

### Step 3: Execute Discovery

Analyze the codebase to populate artifacts in this order:

1. Technology stack
2. Build/test configuration
3. Code structure
4. Architecture
5. Component inventory
6. Dependencies
7. Conditional artifacts only when their conditions are met

Use parallel subagent exploration for large codebases when needed. Reuse prior reverse-engineering artifacts and check for drift before re-deriving when this is a continuation workflow.

### Step 4: Populate Artifacts and Validate Delegated Output

Create each mandatory artifact, create conditional artifacts when applicable, and mark non-applicable artifacts in `reverse-index.md` with a clear rationale. If delegated agent output exists, validate it before continuing.

### Step 5: Update State and Audit Records

Update `ai-sdlc-docs/state/state-log.md`, `ai-sdlc-docs/audit/audit.md`, and `ai-sdlc-docs/traceability/traceability-matrix.md` according to `logging/state-management.md`, `logging/audit-management.md`, and `logging/traceability-management.md`.

### Step 6: Present Completion Summary

Present a concise summary of what was discovered and what was skipped.

---

## Approval Gate

- [ ] Review the output artifact
- [ ] Verify completeness and accuracy
- [ ] Record approval in `audit.md`
- [ ] Update `state-log.md` with PASS status
- [ ] Update `traceability-matrix.md`
- [ ] Confirm `reverse-index.md` matches the final artifact set

---

## SKILL Rules

### Must
- Load the related common rules before starting.
- Read all activity inputs first.
- Verify the current `state-log.md` status before making decisions.
- Record all decisions in `audit.md`.
- Determine whether the workspace is brownfield or greenfield before choosing the reverse-engineering depth.
- Run reverse engineering when the workspace contains existing code or infrastructure.
- Create `reverse-index.md` as the first output artifact.
- Create all mandatory artifacts for brownfield work.
- Create conditional artifacts when their conditions are met.
- Mark conditional artifacts as `not_applicable` in `reverse-index.md` when their conditions are not met.
- Validate delegated agent output before continuing when delegated output exists.
- Reuse prior reverse-engineering artifacts and check for drift when this is a continuation workflow.
- Present a completion summary of what was discovered and what was skipped.

### Must Not
- Do not fabricate information.
- Do not silently skip reverse engineering.
- Do not remove mandatory steps.
- Do not bypass approval.
- Do not disable audit logging.
- Do not disable traceability.
- Do not weaken mandatory gates.
- Do not break downstream dependency compatibility.
- Do not omit rationale for any `not_applicable` or `skipped` artifact.
- Do not skip `reverse-index.md` generation on brownfield work.
- Do not ignore user refusal handling for reverse engineering or `reverse-index.md`.

### User Refusal Handling
- If the user refuses reverse engineering or refuses to generate `reverse-index.md`, present a risk warning explaining that impact analysis becomes unsafe and can lead to missed dependencies, integration failures, and regressions.
- Offer three paths: full reverse engineering, minimal reverse engineering, or proceeding without reverse engineering with explicit risk acceptance.
- If the user explicitly accepts the risk and chooses to proceed without reverse engineering, record that acceptance in `audit.md` and create a minimal `reverse-index.md` with all artifacts marked `skipped` and a clear rationale.

### Execution Integrity
- Preserve existing behavior by keeping discovery factual and avoiding design decisions in reverse engineering.
- Keep the sequence of operations intact so downstream work still receives valid artifacts and status records.
- Record skip decisions, approval status, and completion state consistently across `reverse-index.md`, `audit.md`, and `state-log.md`.



