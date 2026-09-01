# Base Activities

## Purpose

Base Activities defines the standard activities available in the AI-SDLC framework.
Each activity is an independent, self-contained execution unit with clear inputs, outputs, and execution conditions.

## DALi Project Rule Loading

For every workflow executed against `dali-ui`, load
`rules/dali-ui-context.md`. Its Ubuntu development baseline applies to planning,
implementation, build, and validation activities.

When the active profile is `dali_component_feature`, apply this additional
routing before executing each activity:

| Activity | Required project rules and reference use |
|---|---|
| Profile prerequisite | Load `rules/oneui-components-reference.md`; obtain and validate the reference source before A0 |
| A0 | Inspect existing DALi component patterns and OneUIComponents source, tests, and samples; record the reference commit |
| A1, A2 | Load `rules/component-development.md`; derive component behavior and acceptance criteria from the reference |
| A4 | Load component development, public API/ABI, Handle-Body, and component-boundary rules; document intentional reference differences |
| C1 | Apply component structure, style, builder, construction-time style, configuration, naming, and test-design rules |
| C2 | Implement against the approved design and applicable component rules |
| C3 | Load `rules/build-and-test.md`; verify rule compliance and reference behavior with executable unit evidence on the Ubuntu baseline |
| C4 | Load `rules/build-and-test.md`; run the integrated Ubuntu build and approved test scope; record environment and exact commands |
| C5 | Skipped by default; if explicitly added for release preparation, record the OneUIComponents reference commit and intentional differences in release evidence |

These requirements apply only to `dali_component_feature`. Other profiles do
not load or require `rules/oneui-components-reference.md` unless the user
explicitly requests it.

### DALi C5 Default

C5 is `SKIP` in every DALi development workflow profile because project
releases aggregate multiple completed changes at a separate milestone. Keep
the C5 definition below available for an explicit release-preparation request.
When skipped, complete Construction Review after C4 and use `completed` as the
workflow final status. PR preparation and per-change integration remain C2/C3
concerns, not C5 release execution.

### org_standard API Change Routing

When the active profile is `org_standard`, activate this routing if the user
request, approved design, or affected files change a DALi public, extension, or
integration API contract:

| Activity | Required project rules and evidence |
|---|---|
| A0 | Identify API level, exported declarations, consumers, subclasses, call sites, ABI-sensitive layout, and affected tests, samples, and documentation |
| A1, A2 | Load `rules/public-api-abi.md`; classify ordinary work versus explicit pre-release redesign; record compatibility, ABI, migration, and approval requirements |
| A4 | Load `rules/handle-body-pattern.md`, `rules/component-boundaries.md`, and `rules/api-naming.md`; design the target API and implementation boundary |
| C1 | List each API addition, removal, or change and map it to implementation, migration, documentation, and test work |
| C2 | Implement only the approved API shape and keep public handles, exports, and API-level dependencies compliant |
| C3 | Load `rules/validation-checks.md` and `rules/build-and-test.md`; inspect the actual public-header diff, run applicable checks, and record rule-by-rule evidence |
| C4 | Execute the integrated Ubuntu build and approved regression scope; include affected samples and manual-tests where applicable |

This routing is conditional within `org_standard`; it does not require a
separate API-refactoring workflow profile. If no API contract is affected,
record the condition as not applicable and do not add API-specific artifacts or
review requirements.

---

# Inception Activities

## A0: Context Analysis

**Purpose**: Analyze existing project state and identify scope of change impact

**Description**: 
- Understand existing architecture, code structure, and dependencies
- Identify affected components
- Assess artifact freshness and relevance

**Executor**: ai-sdlc-core/skills/reverse-engineering/SKILL.md

**Inputs**:
- existing codebase
- requirements brief

**Outputs**:
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

---

## A1: Requirements Elaboration

**Purpose**: Elicit and detail user requirements

**Description**:
- Clarify user intent and acceptance criteria
- Identify functional and non-functional requirements
- Document use cases and scenarios

**Executor**: ai-sdlc-core/skills/requirements-elaboration/SKILL.md

**Inputs**:
- user request
- existing project context

**Outputs**:
- ai-sdlc-docs/inception/requirements/requirements-brief.md
- ai-sdlc-docs/state/state-log.md
- ai-sdlc-docs/audit/audit.md

---

## A2: Requirements Analysis

**Purpose**: Validate and analyze requirements for completeness and feasibility

**Description**:
- Detect conflicts, gaps, and ambiguities
- Assess feasibility and risks
- Refine requirements with stakeholder input

**Executor**: ai-sdlc-core/skills/requirements-analysis/SKILL.md

**Inputs**:
- ai-sdlc-docs/inception/requirements/requirements-brief.md
- reverse-engineering artifacts (if available)

**Outputs**:
- ai-sdlc-docs/inception/requirements/requirements-analysis.md
- ai-sdlc-docs/state/state-log.md
- ai-sdlc-docs/audit/audit.md
- ai-sdlc-docs/traceability/traceability-matrix.md

---

## A3: Workflow Planning

**Purpose**: Plan execution strategy and task allocation

**Description**:
- Define work breakdown structure
- Estimate effort and schedule
- Plan resource allocation
- Define quality gates and review points

**Executor**: ai-sdlc-core/skills/workflow-planning/SKILL.md

**Inputs**:
- ai-sdlc-docs/inception/requirements/requirements-brief.md
- ai-sdlc-docs/inception/requirements/requirements-analysis.md
- reverse-engineering artifacts (if used)

**Outputs**:
- ai-sdlc-docs/inception/workflow/workflow-planning.md
- ai-sdlc-docs/state/state-log.md
- ai-sdlc-docs/audit/audit.md

---

## A4: Architecture Design

**Purpose**: Design system architecture and technical solution

**Description**:
- Design overall system structure
- Define integration points
- Plan deployment architecture
- Address non-functional requirements

**Executor**: ai-sdlc-core/skills/architecture-design/SKILL.md

**Inputs**:
- ai-sdlc-docs/inception/requirements/requirements-analysis.md
- ai-sdlc-docs/inception/workflow/workflow-planning.md
- reverse-engineering artifacts (if brownfield)

**Outputs**:
- ai-sdlc-docs/inception/architecture/architecture-design.md
- ai-sdlc-docs/state/state-log.md
- ai-sdlc-docs/audit/audit.md
- ai-sdlc-docs/traceability/traceability-matrix.md

---

## A5: Units Generation

**Purpose**: Generate work units (work items/tasks) from design

**Description**:
- Decompose architecture into implementable units
- Define unit acceptance criteria
- Organize unit execution sequence
- Estimate unit-level effort

**Executor**: ai-sdlc-core/skills/units-generation/SKILL.md

**Inputs**:
- ai-sdlc-docs/inception/architecture/architecture-design.md
- ai-sdlc-docs/inception/workflow/workflow-planning.md
- ai-sdlc-docs/inception/requirements/requirements-analysis.md

**Outputs**:
- ai-sdlc-docs/inception/units/unit-of-work-plan.md
- ai-sdlc-docs/inception/units/unit-generation.md
- ai-sdlc-docs/inception/units/unit-of-work-dependency.md
- ai-sdlc-docs/inception/units/unit-of-work-story-map.md
- ai-sdlc-docs/inception/units/unit-spec.md
- ai-sdlc-docs/state/units/{unit-id}-team-status.md
- ai-sdlc-docs/state/state-log.md
- ai-sdlc-docs/audit/audit.md

---

# Construction Activities

## C1: Implementation Design

**Purpose**: Design implementation details for each work unit

**Description**:
- Detailed design per work unit
- Code structure planning
- API/interface specifications
- Test strategy per unit

**Executor**: ai-sdlc-core/skills/implementation-design/SKILL.md

**Inputs**:
- ai-sdlc-docs/inception/requirements/requirements-analysis.md
- ai-sdlc-docs/inception/workflow/workflow-planning.md
- ai-sdlc-docs/inception/architecture/architecture-design.md
- ai-sdlc-docs/inception/units/unit-generation.md (when units exist)

**Outputs**:
- ai-sdlc-docs/construction/implementation-design/{unit-name}-implementation-design.md
- ai-sdlc-docs/state/state-log.md
- ai-sdlc-docs/audit/audit.md

---

## C2: Code Generation

**Purpose**: Write source code based on implementation design

**Description**:
- Implement code per unit specifications
- Follow coding standards
- Document code
- Prepare for verification

**Executor**: ai-sdlc-core/skills/code-generation/SKILL.md

**Inputs**:
- ai-sdlc-docs/construction/implementation-design/{unit-name}-implementation-design.md
- ai-sdlc-docs/inception/units/unit-spec.md (when units exist)

**Outputs**:
- ai-sdlc-docs/construction/plans/{unit-name}-code-generation-plan.md
- ai-sdlc-docs/construction/code/{unit-name}-code-generation-summary.md
- ai-sdlc-docs/state/state-log.md
- ai-sdlc-docs/audit/audit.md

---

## C3: Code Verification

**Purpose**: Verify code quality and unit functionality

**Description**:
- Unit testing
- Code review
- Static analysis
- Integration testing (cross-unit)

**Executor**: ai-sdlc-core/skills/code-verification/SKILL.md

**DALi Execution Requirements**:

- Load `rules/build-and-test.md`.
- Build and install the current `dali-ui` source before building or executing
  affected automated tests.
- Use targeted test modules or cases only when the approved implementation
  design establishes a reliable unit boundary.
- Keep C3 `BLOCKED` when current build/install or automated unit-test evidence
  is missing.

**Inputs**:
- ai-sdlc-docs/construction/implementation-design/{unit-name}-implementation-design.md
- ai-sdlc-docs/construction/plans/{unit-name}-code-generation-plan.md
- ai-sdlc-docs/construction/code/{unit-name}-code-generation-summary.md
- ai-sdlc-docs/inception/units/unit-spec.md (when units exist)

**Outputs**:
- ai-sdlc-docs/construction/code/{unit-name}-code-verification-summary.md
- ai-sdlc-docs/state/state-log.md
- ai-sdlc-docs/audit/audit.md

---

## C4: Build & Testing

**Purpose**: Build system and execute integration/system testing

**Description**:
- Integrated build execution
- System-level testing
- Performance testing
- Workflow completion readiness assessment

**Executor**: ai-sdlc-core/skills/build-testing/SKILL.md

**DALi Execution Requirements**:

- Load `rules/build-and-test.md`.
- Execute the integrated Ubuntu build and install after all required units pass
  C3 and the Integration Ready Check.
- Build and run the complete automated-test suite unless the approved test plan
  records a justified regression scope.
- Include affected manual-tests and samples in the approved validation scope.
- Record environment, exact commands, outcomes, exclusions, and residual risks.

**Inputs**:
- ai-sdlc-docs/inception/requirements/requirements-analysis.md
- ai-sdlc-docs/inception/architecture/architecture-design.md
- ai-sdlc-docs/inception/units/unit-generation.md (when units exist)
- ai-sdlc-docs/construction/plans/{unit-name}-code-generation-plan.md
- ai-sdlc-docs/construction/code/{unit-name}-code-generation-summary.md
- ai-sdlc-docs/construction/code/{unit-name}-code-verification-summary.md

**Outputs**:
- ai-sdlc-docs/construction/test/test-plan.md
- ai-sdlc-docs/construction/test/test-report.md
- ai-sdlc-docs/construction/test/regression-report.md
- ai-sdlc-docs/state/state-log.md
- ai-sdlc-docs/audit/audit.md
- ai-sdlc-docs/traceability/traceability-matrix.md

---

## C5: Release

**Purpose**: Deploy solution to target environment

**Description**:
- Prepare release package (including dependency review and removal if applicable)
- Execute deployment
- Verify deployment success
- Document release changes

**Executor**: ai-sdlc-core/skills/release/SKILL.md  
**Note**: Release Skill specializes in dependency removal for release preparation; general release tasks may require additional custom implementation per project needs

**Inputs**:
- ai-sdlc-docs/construction/test/test-report.md
- ai-sdlc-docs/construction/test/regression-report.md (when applicable)
- ai-sdlc-docs/inception/units/unit-generation.md (multi-unit: Unit Summary state)

**Outputs**:
- ai-sdlc-docs/construction/release/release-note.md
- ai-sdlc-docs/traceability/traceability-matrix.md
- ai-sdlc-docs/state/state-log.md
- ai-sdlc-docs/audit/audit.md

---

# Activity Modularity

Each activity is an independent module with:

- **Rule Definition**: Standard behavior and requirements
- **Implementation Flexibility**: Can be executed via:
  - Rule file (guidance-based)
  - Skill (user-initiated tool)
  - Agent (autonomous execution)
  - Custom logic (organization-specific)

**Example**: A1 (Requirements Elaboration) can be:
- Rule-based: Follow requirements-elaboration.md
- Skill-based: Execute /office-hours skill
- Agent-based: Delegate to domain-expert-agent

**Path Finder does NOT dictate HOW activities execute** — only WHICH 
activities to include in the workflow and their execution order.

---
