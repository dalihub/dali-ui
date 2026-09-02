---
title: Requirements Analysis
phase: inception
version: 1.0.0
owner: SR-AX-LAB
outputs:
  - ai-sdlc-docs/inception/requirements/requirements-analysis.md
  - ai-sdlc-docs/state/state-log.md
  - ai-sdlc-docs/audit/audit.md
  - ai-sdlc-docs/traceability/traceability-matrix.md
triggers:
  - requirements analysis
  - analyze requirements
  - requirements brief
allowed-tools:
  - Read
  - Write
  - Edit
---

# Requirements Analysis

## Description

Analyze a requirements brief deeply enough to produce a testable, prioritized requirements analysis with risks, assumptions, open questions, and traceability.

---

## Reference Rules

- `logging/state-management.md` — use to update `state-log.md` correctly.
- `logging/audit-management.md` — use to update `audit.md` correctly.
- `logging/traceability-management.md` — use to update `traceability-matrix.md` correctly.

---

## Outputs

| Artifact | Template | Description |
|----------|----------|---------|
| `ai-sdlc-docs/inception/requirements/requirements-analysis.md` | `templates/02-requirements-analysis.md` | Primary requirements analysis artifact |
| `ai-sdlc-docs/state/state-log.md` | (standard) | Record completion status |
| `ai-sdlc-docs/audit/audit.md` | (standard) | Record assumptions, trade-offs, and approval prompt |
| `ai-sdlc-docs/traceability/traceability-matrix.md` | `ai-sdlc-docs/traceability/traceability-matrix.md` | REQ → AC → Architecture → Code → Test mapping |

---

## Execution Steps

### Step 1: Load Context

- Read all available activity inputs before analysis.
- Review the requirements brief and any existing workspace artifacts that are relevant.
- Load `ai-sdlc-docs/state/state-log.md` and capture the current status.
- Note any missing or unavailable artifact as a gap instead of guessing.

### Step 2: Assess Requirements

- Analyze scope, goals, stakeholders, constraints, success criteria, and priority signals from the brief.
- Review any existing requirements documents and align with the current request.
- Convert non-markdown source material to markdown only when needed for the output artifact.

### Step 3: Analyze Completeness

- Evaluate functional requirements, corrected behavior statements, user scenarios, business context, technical context, and quality attributes.
- Populate the non-functional requirements table using the NFR checklist rules below.
- Identify dependencies, edge cases, risks, and implementation impacts.
- Separate confirmed information from assumptions and hypotheses.

### Step 4: Generate Clarifying Questions

- Identify any missing, ambiguous, or conflicting areas that materially affect the analysis.
- Write clarifying questions clearly and only for the unresolved gaps that matter.
- If questions remain, pause and wait for user answers before finalizing the artifact.

### Step 5: Draft the Requirements Analysis

- Create or update `ai-sdlc-docs/inception/requirements/requirements-analysis.md`.
- Include requirement breakdown with MoSCoW priority.
- Include functional requirements or corrected behavior statements.
- Include a non-functional requirements table with `Category`, `Status`, `Target`, and `Verification`.
- Include acceptance criteria in Given/When/Then format.
- Include risk analysis, impact analysis, assumptions, and open questions.
- Include REQ → AC mapping for every requirement.
- Keep the content aligned to the template structure and remove anything that does not map cleanly.

### Step 6: Update Traceability Matrix

Requirements Analysis (A2) populates the REQ and AC columns:

- Load `ai-sdlc-docs/traceability/traceability-matrix.md`
- Create initial mapping table:
  - REQ-### entries (from this analysis artifact)
  - AC-### entries for each REQ (in Given/When/Then format)
  - Mark Code and Test columns as "pending"
- Validation: Every REQ-### has at least one AC-### mapped
- Record in audit.md: "traceability-matrix updated: REQ + AC columns populated for [COUNT] requirements"

### Step 7: Update Tracking Artifacts

- Update `ai-sdlc-docs/state/state-log.md` according to `logging/state-management.md`.
- Update `ai-sdlc-docs/audit/audit.md` according to `logging/audit-management.md`.

### Step 9: Record Completion

- Present the completion message.
- Wait for explicit user approval before proceeding to the next workflow stage.

---

## Approval Gate

- [ ] Review the output artifact
- [ ] Verify completeness and accuracy
- [ ] Record approval in `audit.md`
- [ ] Update `state-log.md` with PASS status
- [ ] Update `traceability-matrix.md`

---

## SKILL Rules

### Must
- Load `logging/state-management.md`, `logging/audit-management.md`, and `logging/traceability-management.md` before drafting.
- Read all activity inputs first.
- Verify the current `state-log.md` status before drafting.
- Preserve the source terminology unless the request is ambiguous.
- Classify every non-functional requirement with exactly one status: `In scope`, `Out of scope`, or `Not applicable`.
- For every `In scope` NFR, provide both a target and a later verification approach.
- For every `Out of scope` NFR, provide a rationale.
- For every `Not applicable` NFR, provide a rationale.
- For `NEW_BUILD` and `REQ_ADD` work, consider at least: Security, Reliability and Availability, Observability, and Maintainability.
- For UI-touching work, also consider: Accessibility and Performance.
- For AI/LLM-touching work, also consider: AI-Specific NFRs, Cost, and Privacy and Compliance.
- Use the NFR checklist categories: Performance, Scalability, Reliability and Availability, Security, Privacy and Compliance, Observability, Maintainability, Portability, Accessibility, Internationalization, Cost, and AI-Specific NFRs.
- Capture the NFR results in `requirements-analysis.md` as a table with Category, Status, Target, and Verification.
- Classify every requirement with MoSCoW priority.
- Write every acceptance criterion in Given/When/Then format.
- Ensure every acceptance criterion is testable.
- Map each requirement to at least one acceptance criterion.
- Record assumptions, risks, and open questions explicitly.
- Keep the output aligned to the template structure.

### Question Rules
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

### Must Not
- Do not fabricate information or fill gaps without marking them as assumptions.
- Do not silently resolve ambiguity that affects scope or priority.
- Do not remove mandatory steps, bypass approval, disable audit, or disable traceability.
- Do not weaken mandatory gates.
- Do not write content that does not map to the template.
- Do not include work-type metadata or other unrelated operational content.
- Do not include any NFR category without a rationale.
- Do not omit tracking artifact updates when they are part of the workflow.

### Handling Ambiguity
- If information is missing, note the gap and continue with the best available context.
- If a question materially affects architecture, validation, or scope, surface it before finalizing the analysis.
- If priority is unclear, use the brief as the starting point and document the reasoning.



