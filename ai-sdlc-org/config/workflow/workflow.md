# Base Workflow

## Purpose

Base Workflow defines the standard workflow paths, execution sequences, and review checkpoints 
that organize the activities defined in `activity.md` (in the same config/workflow/ directory). Path Finder uses this to 
generate organization-tailored workflows.

**Reference**: See `activity.md` for detailed activity definitions (A0-A5, C1-C5).

## DALi UI Platform Baseline

All workflows executed for `dali-ui` MUST load
`rules/dali-ui-context.md`. Planning, implementation, build, and validation use
Ubuntu as the development baseline. A different environment requires an
explicitly recorded exception and must not be presented as Ubuntu evidence.

---

# Activity Sequence for organization
A Workflow is selected based on workspace state and work type.

## Standard Workflow
**profile_id** : org_standard
**execution sequence**:
```
Inception Loop:
A0 → A1 → A2 → A3 → A4 → A5[COND]

[INCEPTION REVIEW]

Construction Loop:
C1 → C2 → C3 → C4 → C5[SKIP]

[CONSTRUCTION REVIEW]
```

**Sequential Execution**:
- Activities execute in listed order
- Review checkpoints are gates (PASS required to proceed)

**A5 Condition**:

Execute A5 only when `multi_unit == true` or the approved design contains two
or more independently managed work units. Otherwise mark A5 `SKIP` with the
single-unit rationale in `work-path.md` and `audit.md`.

**DALi API Change Policy**:

Apply this policy when the request, approved design, or affected files add,
remove, rename, or change a DALi API contract. API-contract changes include:

- Changes below `public-api`, `extension-api`, or `integration-api`.
- Changes to exported declarations, base classes, class layout, virtual
  functions, public or protected signatures, enum values, or API-owned types.
- Changes that require updates to API consumers, subclasses, tests, samples, or
  user-facing documentation.

When triggered:

- Load `rules/public-api-abi.md`, `rules/handle-body-pattern.md`,
  `rules/api-naming.md`, `rules/docs-and-wiki.md`, and
  `rules/validation-checks.md`.
- Classify the change as ordinary compatibility-preserving work or an explicit
  pre-release redesign.
- Do not pass Inception Review without API-level, consumer, compatibility, ABI,
  and migration-impact analysis.
- Do not pass C3 or Construction Review when an applicable required rule is
  violated without an explicitly approved redesign decision.
- Do not load `rules/oneui-components-reference.md` unless the user explicitly
  requests it; that rule is otherwise exclusive to `dali_component_feature`.

## Advanced development

**Profile_id**: org_advanced_sw

**Execution Sequence**:
```
Inception Loop:
A1 → A2 → A3 → A4 → A5[COND]

[INCEPTION REVIEW]

Construction Loop:
C1 → C2 → C3 → C4 → C5[SKIP]

[CONSTRUCTION REVIEW]
```

**Path Characteristics**:
- No reverse engineering (A0 skipped)
- All inception activities required for new architecture
- C1 through C4 are required; C5 is skipped by the DALi release policy
- 4 weeks typical timeline

## DALi Component Feature

**profile_id**: dali_component_feature

**Execution Sequence**:

```text
A0 → A1 → A2 → A3 → A4 → A5[COND]

[INCEPTION REVIEW]

C1 → C2 → C3

[INTEGRATION READY CHECK]

C4 → C5[SKIP]

[CONSTRUCTION REVIEW]
```

**Profile Preconditions**:

- Resolve these preconditions during Path Finder requirements gathering after
  profile selection and before A0; they are not a new activity or review gate.
- Load `rules/dali-ui-context.md`.
- Load `rules/oneui-components-reference.md`.
- Before A0, obtain and validate readable OneUIComponents `devel` source using
  the acquisition and ownership rules in that document.
- If the reference source is unavailable, keep the workflow `BLOCKED`; do not
  infer component behavior and continue.
- Use Ubuntu as the implementation, build, and validation baseline.

**Required Project Rules**:

- `rules/component-development.md`
- `rules/oneui-components-reference.md`
- `rules/public-api-abi.md`
- `rules/handle-body-pattern.md`
- `rules/component-boundaries.md`
- `rules/api-naming.md`
- `rules/docs-and-wiki.md`
- `rules/build-and-test.md`
- `rules/validation-checks.md`

**Path Characteristics**:

- A0 is required to analyze existing DALi code and the OneUIComponents
  reference at a recorded commit.
- A5 is conditional on whether the component work needs multiple independently
  tracked units.
- Component base-class, style, builder, construction, and configuration design
  follows `rules/component-development.md`.
- Mandatory review gates remain unchanged.

## DALi Release Activity Policy

C5 is `SKIP` by default in every DALi development workflow profile.

`dali-ui` releases aggregate multiple independently completed changes and PRs
at a separate release milestone. An individual feature, refactoring, or module
change therefore completes after C4, Construction Review, and the mandatory
traceability finalization. Its final workflow status is `completed`, not
`released`.

Keep the C5 activity definition available for an explicit release-preparation
request. A user may add C5 to the generated work path because an explicit user
request has higher Path Finder priority than the organization profile. When C5
is skipped, record this organization-policy rationale in `work-path.md` and
`audit.md`; do not create a placeholder release note.

---

# Activity Execution Markers

**Reference**: See [[glossary]] > "Execution Markers" for detailed marker definitions (MUST, COND, SKIP).

---

# Review Checkpoints

## Inception Review

**Trigger**: After Inception Loop completion (A0-A5)
**Gate**: PASS / BLOCKED / FAIL (PASS required to proceed to Construction)
**Purpose**: Verify inception activities are complete and design is ready
**Executor**: ../review/inception-review-checklist.md

---

## Construction Review

**Trigger**: After all selected Construction activities complete
**Gate**: PASS / BLOCKED / FAIL (PASS required to complete the workflow)
**Purpose**: Verify implementation quality and readiness for workflow completion
**Executor**: ../review/construction-review-checklist.md

---

## Integration Ready Check

**Trigger**: After C3 (Code Verification), before C4 (Build & Testing)
**Purpose**: Verify code is ready for integrated build
**Executor**: ai-sdlc-core/review/integration-ready-check-rule.md
**Checklist**: ../review/integration-ready-checklist.md

---
