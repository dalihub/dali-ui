---
title: Architecture Design
phase: inception
version: 1.0.0
owner: SR-AX-LAB
outputs:
  - ai-sdlc-docs/inception/architecture/architecture-design.md
  - ai-sdlc-docs/state/state-log.md
  - ai-sdlc-docs/audit/audit.md
  - ai-sdlc-docs/traceability/traceability-matrix.md
triggers:
  - architecture design
allowed-tools:
  - Read
  - Write
  - Edit
---

# Architecture Design

## Description

Define the structural solution boundary for the requested change, describing only the architecture necessary for the current change while avoiding unrelated redesign.

---

## Reference Rules

- `logging/state-management.md` — for updating `state-log.md`
- `logging/audit-management.md` — for updating `audit.md`
- `logging/traceability-management.md` — for updating `traceability-matrix.md`

---

## Outputs

| Artifact | Template | Description |
|----------|----------|---------|
| `ai-sdlc-docs/inception/architecture/architecture-design.md` | `templates/04-architecture-design.md` | Primary consolidated architecture document |
| `ai-sdlc-docs/state/state-log.md` | (standard) | Record activity completion |
| `ai-sdlc-docs/audit/audit.md` | (standard) | Record decisions and approvals |
| `ai-sdlc-docs/traceability/traceability-matrix.md` | (standard) | REQ → AC → Architecture mapping |

---

## Execution Steps

### Step 1: Analyze the Context

- Read the current requirements context
- Identify key business capabilities and functional areas
- Determine design scope and complexity

### Step 2: Design the Architecture

Create a single consolidated architecture document at `ai-sdlc-docs/inception/architecture/architecture-design.md`, including the following sections:

- **2.1 Components** — For each component define: name, purpose, responsibilities, interfaces
- **2.2 Component Methods** — For each component define: method signatures, high-level purpose, input/output types (detailed business logic is defined later in Functional Design during the Construction phase)
- **2.3 Services / Orchestration Units** — Define only when the change introduces or modifies a service or orchestration boundary. For each unit define: name, purpose, responsibilities, participating components, service interactions, orchestration flow, state handled, failure handling responsibilities. If none, state: "No service or orchestration boundary changes are introduced by this change."
- **2.4 Component Dependencies** — dependency relationships, communication patterns, data flow between components
- **2.5 Data Flow and State Ownership** — data flow across components/services, state ownership boundaries, source of truth for key data
- **2.6 Integration Points** — external systems/modules, API contracts or integration methods, versioning/compatibility considerations
- **2.7 Failure Handling and Rollback** — failure scenarios, error propagation strategy, rollback strategy (if applicable)
- **2.8 Operational Considerations (Optional)** — scalability, performance, resilience, security considerations if relevant
- **2.9 Architectural Risks and Trade-offs** — key risks, design trade-offs, assumptions and constraints

### Step 3: Log the Approval Prompt

- Log the approval prompt with an ISO 8601 timestamp in `ai-sdlc-docs/audit/audit.md`
- Include the complete approval prompt text

### Step 4: Present the Completion Message

Present: "Architecture design is complete."

### Step 5: Wait for Explicit Approval

- Do not proceed until the user explicitly approves the architecture design
- If the user requests changes, update the design and repeat the approval process

### Step 6: Record the Approval Response

- Log the user's approval response with an ISO 8601 timestamp in `ai-sdlc-docs/audit/audit.md`
- Include the exact user response text and mark the approval status clearly

### Step 7: Update Traceability Matrix

Architecture Design (A4) populates the Architecture Component column:

- Load `ai-sdlc-docs/traceability/traceability-matrix.md`
- For each AC from the requirements analysis:
  - Identify which architecture component is responsible
  - Cross-reference to the components defined in architecture-design.md
- Validation: Every AC has an Architecture component assigned
- Record in audit.md: "traceability-matrix updated: Architecture column populated for [COUNT] ACs"

### Step 8: Record Results

- Update `ai-sdlc-docs/state/state-log.md` following `logging/state-management.md`
- Update `ai-sdlc-docs/audit/audit.md` following `logging/audit-management.md`

---

## Approval Gate

- [ ] Review the output artifact
- [ ] Verify completeness and accuracy
- [ ] Record approval in `audit.md`
- [ ] Update `state-log.md` with PASS status

Standard Completion Prompt (MANDATORY) — present "Architecture design is complete." and offer:
1. Request changes to the architecture design
2. Continue to units generation

If option 2 is selected, proceed to units generation. If units generation is not needed, the Inception Review will be evaluated as part of the main workflow.

---

## SKILL Rules

- Load the Related Common Rules before starting
- Read all requirements context inputs first
- Verify current status in `state-log.md`
- Record all decisions and approvals in `audit.md`
- Do not fabricate information
- Describe only the architecture necessary for the current change; avoid unrelated redesign
- The output format MUST comply with the templates in `templates/`, prioritizing the template's structure over this rule's "Outputs" section when they conflict
- Produce detailed architecture when: a new component or service is added; interfaces or contracts change; cross-module orchestration changes; data ownership changes; scalability, resilience, security, or compliance behavior changes; or a defect indicates a design weakness rather than only a code error
- Lightweight architecture is allowed when: the fix is isolated internal logic; no boundary or contract changes are needed; and no new runtime dependencies are introduced
- Do not introduce artificial service layers
- Do not create a service solely to wrap a single component with identical responsibilities or for small, single-component changes
- Use ISO 8601 timestamp format for all logged prompts and responses
- Do not proceed past the Approval Gate until the user's approval is clear and unambiguous
- This task is complete only when work can be split into implementable units, or a deliberate single-unit path exists, without unresolved structural ambiguity

### NFR Rules

- Translate every in-scope NFR into concrete architecture decisions, component boundaries, and operational constraints; do not defer architecture-level NFR decisions to implementation unless the category is explicitly out of scope
- For each in-scope NFR category, record in `architecture-design.md`: the architectural decision or constraint, the components or services affected, any failure mode or trade-off, and the later verification evidence expected from implementation or testing
- Focus each NFR decision on what the architecture must make possible
- Address these core NFR categories for every architecture:
  - **Performance** — throughput allocation by component/service, latency budget by request path, resource budget per runtime/worker, warm-path and cold-start implications, architecture-level verification plan
  - **Scalability** — horizontal scaling assumptions, vertical scaling limits, stateful vs. stateless boundaries, data growth and back-pressure strategy, scale-test scenarios
  - **Reliability and Availability** — availability target or service-level expectation, fault domains and failure modes, retry/timeout/circuit-breaker placement, graceful degradation paths, recovery and rollback approach
  - **Security** — authentication/authorization boundaries, trust zones and interface boundaries, input validation placement, secret storage and rotation approach, sensitive data flow restrictions, threat model summary
  - **Privacy and Compliance** — personal data flow boundaries, retention and deletion expectations, consent and control touchpoints, regulatory scope and auditability
  - **Observability** — logging boundaries and log ownership, metrics exported by component, tracing span boundaries, alert ownership and escalation paths, dashboard views needed for operations
  - **Maintainability** — component responsibility boundaries, dependency direction and coupling limits, testability of modules/services, documentation and code-ownership expectations, technical debt containment
  - **Portability** — target runtime versions, OS/platform/container assumptions, environment-specific dependency isolation, cloud/on-prem/hybrid deployment stance
- Address these conditional NFR categories only when applicable:
  - **Accessibility** (when the architecture has user-facing UI) — keyboard accessibility support at the system boundary, screen reader compatibility constraints, color and contrast considerations, localization and RTL architecture support
  - **Internationalization** (when the architecture has localized UI or locale-aware output) — supported languages, locale-aware formatting at boundaries, RTL support, translation pipeline integration
  - **Cost** — cost drivers by component/service, expensive operations that need isolation, budget guardrails or throttles, cost attribution boundaries
  - **AI-Specific NFRs** (when the system uses AI or LLMs) — model selection boundary, prompt versioning and storage, output evaluation and guardrail flow, fallback architecture when the model is unavailable, PII handling boundaries for prompts and outputs, cost and latency trade-off controls




