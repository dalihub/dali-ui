---
title: Implementation Design
phase: construction
version: 1.0.0
owner: SR-AX-LAB
outputs:
  - ai-sdlc-docs/construction/implementation-design/{unit-name}-implementation-design.md
  - ai-sdlc-docs/state/state-log.md
  - ai-sdlc-docs/audit/audit.md
triggers:
  - implementation-design
allowed-tools:
  - Read
  - Write
  - Edit
---

# Implementation Design

## Description

Translate approved requirements, architecture, and unit boundaries into code-level implementation intent before any code changes are made, so downstream code generation can proceed without inventing unstated behavior.

---

## Reference Rules

- `common/ai-execution-rule.md` — context-window management when designing multiple units
- `logging/state-management.md` — updating `state-log.md`
- `logging/audit-management.md` — updating `audit.md`
- `logging/traceability-management.md` — updating the traceability matrix

---

## Outputs

| Artifact | Template | Description |
|----------|----------|---------|
| `ai-sdlc-docs/construction/implementation-design/{unit-name}-implementation-design.md` | `ai-sdlc-{org}/templates/06-unit-implementation-design.md` | Primary output artifact (one file per unit) |
| `ai-sdlc-docs/state/state-log.md` | (standard) | Record activity completion |
| `ai-sdlc-docs/audit/audit.md` | (standard) | Record decisions |

---

## Execution Steps

### Step 1: Code-Level Design Analysis

Analyze existing code patterns, then design functions, data structures, interfaces, and algorithms at code level.

- Existing Code Analysis (brownfield only): read target files and document current code patterns, error handling patterns, logging patterns, and test patterns to follow.
- Function/Method Design: for each function/method, define name and purpose, parameters (name, type, description), return type and description, error cases and exceptions, side effects, and complexity estimate.
- Data Structure Design: for each structure, define name and purpose, fields/properties (name, type, description), validation rules, and relationships to other structures.
- Algorithm Design (complex logic): define input/output specification, step-by-step pseudocode or flow, edge cases, and performance considerations.
- Interface Design: for each interface, define name and purpose, methods (signature, description), and implementing classes.

Save as `ai-sdlc-docs/construction/implementation-design/{unit-name}-implementation-design.md`.

### Step 2: Design Validation

Verify completeness (all acceptance criteria mapped to functions, all signatures defined, all data structures specified, all error cases considered, all external dependencies identified), consistency (signatures match interface definitions, data structures consistent across functions, error handling consistent with existing patterns), and feasibility (all functions implementable, no circular dependencies, no missing dependencies). If any check fails, return to Step 1 and complete the design before proceeding.

### Step 3: Nonfunctional Design

Based on the code-level design, define performance requirements (response time, throughput, memory, query optimization), security requirements (authentication/authorization, input validation, encryption, audit logging), reliability requirements (error handling, retry logic, circuit breaker, graceful degradation), and maintainability requirements (documentation, logging, configuration management). Save to the implementation-design file.

### Step 4: Deployment Environment Design

Analyze the functional and nonfunctional design to generate only questions relevant to this unit's infrastructure needs, suggest a deployment architecture, and save to the implementation-design file. If the file does not exist, create it according to `templates/06-unit-implementation-design.md`.

### Step 5: Wait for Explicit Approval

Do not proceed until the user explicitly, clearly, and unambiguously approves the functional, nonfunctional, and infrastructure design. If the user requests changes, update the design and repeat the approval process.

### Step 6: Record Results

Update state, audit, and traceability following `logging/state-management.md`, `logging/audit-management.md`, and `logging/traceability-management.md`.

---

## Approval Gate

- [ ] Review the output artifact
- [ ] Verify completeness and accuracy (functional, nonfunctional, infrastructure design)
- [ ] Confirm downstream code generation can proceed without inventing unstated behavior
- [ ] Record approval in `audit.md`
- [ ] Update `state-log.md` with PASS status

---

## SKILL Rules

- Load the Related Common Rules before starting.
- Read all activity inputs (approved requirements, architecture, unit boundaries) first.
- Verify current status in `state-log.md`.
- Use `templates/06-unit-implementation-design.md` as the structural template, one file per unit.
- Name implementation design files using the unit-name from `ai-sdlc-docs/inception/units/unit-generation.md`.
- When the template and this rule's required output conflict, prioritize the template's structure.
- Define all mandatory content: code-level design (function signatures, class structures, data structures, algorithm pseudocode, error handling), traceability (acceptance criteria to functions, requirements to data structures, architecture components to code files), and context (change location, affected interfaces/dependencies, nonfunctional considerations, verification approach, risks and assumptions).
- Record all decisions in `audit.md`.
- Do not proceed to code generation until explicit user approval is received.
- Do not fabricate information or invent unstated behavior.
- Do not proceed past a validation check that has failed.
- When defining nonfunctional design, use the NFR checklist below to convert approved NFRs into code-level implementation decisions before implementation begins.
  - For each in-scope category, record the target or threshold, the code-level design choice, the affected functions, classes, or data structures, and the verification approach.
  - Core implementation categories: performance, security, reliability and availability, observability, maintainability, and portability.
  - Conditional implementation categories: privacy and compliance, accessibility, internationalization, cost, and AI-specific NFRs.
  - Capture the results in the unit implementation design file as implementation constraints, code decisions, and verification notes.
  - Keep the checklist specific to the unit that is currently being implemented.
  - Do not invent implementation detail for categories that were explicitly marked out of scope.



