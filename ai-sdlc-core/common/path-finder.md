# Path Finder Rule

## Purpose

Path Finder generates the execution workflow by loading organization workflow definitions,
understanding user request and workspace context, proposing a tailored workflow, and 
recording approved workflow decisions.

---

## Directory Reference

This rule references organization-specific directories using a **dynamic path resolution** approach:

**Default pattern:** `ai-sdlc-org/`

**Dynamic resolution:** When a directory matching the pattern `ai-sdlc-*` (other than `ai-sdlc-core`, `ai-sdlc-docs`) exists in the workspace root, Path Finder automatically uses it. This allows directory renames (e.g., `ai-sdlc-org` → `ai-sdlc-samsung`) without code changes.

**Path discovery algorithm:**
1. List all directories in workspace root matching `ai-sdlc-*`
2. Exclude `ai-sdlc-core` and `ai-sdlc-docs` (reserved)
3. Use the first matching directory as the organization directory
4. If multiple organization directories exist, ask user to select one
5. If none exist, use `ai-sdlc-org` as the default

---

# Workflow Generation Process

## Step 1: Load Organization Workflow & Activity Definitions

Path Finder MUST:

1. **Discover organization directory** using dynamic path resolution (see Directory Reference above)
2. Load all workflow profiles from `{org-dir}/config/workflow/workflow.md` (if present)
3. Load all organization activities from `{org-dir}/config/workflow/activity.md` (if present)

Where `{org-dir}` is the discovered organization directory (e.g., `ai-sdlc-org`, `ai-sdlc-samsung`).

If multiple workflow profiles are defined in `{org-dir}/config/workflow/workflow.md`:
- Display all available `profile_id` values to the user
- Ask user to select the workflow profile they want as the base
- Use selected profile as the foundation for workflow generation

If only one profile exists:
- Use it automatically as the base workflow

If `{org-dir}/config/workflow/workflow.md` does not exist:
- Use the default workflow structure defined in glossary.md (two loops: Inception and Construction)

## Step 2: Gather Requirements

Path Finder MUST understand the user request by:

1. Analyzing explicit user request (work type, scope, constraints)
2. Assessing current workspace state (NEW vs EXISTING, artifact status)
3. Asking clarifying questions if needed:
   - Work type (NEW_BUILD, REQ_ADD, DEFECT_FIX, TECH_CHANGE) if unclear
   - Scope depth (Minimal, Standard, Comprehensive) if ambiguous
   - Unit mode (Single-Unit vs Multi-Unit) if applicable
   - Any organization-specific constraints or preferences

## Step 3: Propose Workflow

Path Finder MUST:

1. **Start with selected workflow profile** as the base sequence
2. **Apply customization in this priority order:**
   - User explicit requests (activity additions, removals, changes)
   - Current workspace status (artifact validity, state assessment)
   - Organization policy ({org-dir}/config/workflow/workflow.md constraints, activity replacement)
   - Default workflow sequence (glossary.md structure)
3. **Validate all mandatory reviews are included:**
   - Inception Review (after Inception Loop)
   - Construction Review (after Construction Loop)
   - Integration Ready Check (after C3, before C4)
   - These MUST NOT be removed, skipped, or made conditional under any circumstances
4. **Present proposed workflow** with selection rationale

## Activity Selection Criteria

When selecting activities, apply these criteria (in priority order):

1. **User Request** (explicit activity selection)
   - User explicitly requests or declines specific activities
   - Overrides other criteria except mandatory reviews

2. **Current Status** (state.log)
   - Prior workflow artifacts and completion state
   - Artifact validity assessment (FRESH/STALE/CLEAN status)

3. **Organization Workflow Profile** ({org-dir}/config/workflow/workflow.md if applicable)
   - Load selected workflow profile from organization
   - Apply organization-specific activity sequencing and conditions
   - Organization policies must not conflict with mandatory reviews

4. **Default Workflow Structure** (glossary.md)
   - Use standard Inception Loop (A0-A5) and Construction Loop (C1-C5) structure
   - Select activities based on work type and workspace state (NEW vs EXISTING)
   - Apply execution markers (MUST/COND/SKIP) from glossary definitions

---

# Continuation Workflow Context

When a prior workflow exists (see `loop-continuation.md`):

## Load Prior Context
1. Check `ai-sdlc-docs/state/state-log.md` for prior workflow state
2. If mid-execution: follow resumption rules in `loop-continuation.md`
3. If completed: follow continuation rules in `loop-continuation.md`
4. Inherit completed artifacts from predecessor workflow

## Assess Artifact Validity (for activity selection)
Before selecting activities, determine:
- Is reverse-engineering context still valid? (See drift detection in `loop-continuation.md` Step 3)
- Has architecture changed since prior workflow?
- Are prior units still applicable?
- Do test artifacts need extension or refresh?

## Apply to Activity Selection
Use artifact validity assessment when determining:
- Whether A0 (Context Analysis) is needed — rerun if code structure changed materially
- Whether to reuse A4 (Architecture Design) output — if boundaries/contracts changed, re-run A4
- How to handle A5 (Units Generation) — extend prior units if new units needed, or create fresh split
- How to scope C4 (Build & Testing) — inherit regression test approach, extend with new test cases

**Note**: Detailed activity definitions and executors are located in `skills/` directory.

---

# Mandatory Review Checkpoints

Path Finder MUST include all three mandatory review checkpoints in every workflow:

1. **Inception Review** — after Inception Loop completion (A0-A5)
   - Gate: PASS required to proceed to Construction Loop
   - Must not be skipped, made conditional, or replaced

2. **Construction Review** — after Construction Loop completion (C1-C5)
   - Gate: PASS required for release
   - Must not be skipped, made conditional, or replaced

3. **Integration Ready Check** — after C3 (Code Verification), before C4 (Build & Testing)
   - Verdict-based review for integration readiness
   - Must not be skipped, made conditional, or replaced

These checkpoints MUST NOT be removed, skipped, made conditional, or replaced under any circumstances,
including organization tailoring or user requests.

---

# Work-Path Output Format

When creating `ai-sdlc-docs/inception/plans/work-path.md`, Path Finder MUST use the following structure:

```
| Num | Activity Name | Executor |
|-----|---------------|----------|
```

Where:
- **Num**: Sequential execution order number
- **Activity Name**: Full activity name (include code like A0, C1, and short description)
- **Executor**: Path to the SKILL.md file, review executor, or check file responsible for execution

All activities MUST be numbered in execution order. Review checkpoints MUST be included with their sequential numbers.

---

# Recovery Workflow Context

When a Review returns BLOCKED or FAIL (see `recovery-paths.md`):

## Receive Recovery Analysis
Recovery-Paths provides:
- Failure analysis: which Review failed and why
- Affected artifacts: which outputs need fixing
- Root cause activity: which activity produced the problematic artifact
- Impact scope: which units/components are affected (if multi-unit)

## Update work-path

Path Finder MUST:
1. Load the current `ai-sdlc-docs/inception/plans/work-path.md`
2. Identify the activity that produced the affected artifact
3. Update work-path to re-run from that activity onward, maintaining the table format
4. Skip activities already completed that are not affected by the recovery
5. Record the recovery decision according to `../logging/audit-management.md` Recovery Decision Requirements
6. Present the updated work-path to the user for confirmation
7. Execute from the recovery point

## Loop-Level Recovery

When recovery requires returning to Inception Loop from Construction Loop:
1. Stop current Construction activity
2. Load recovery analysis identifying architectural problem
3. Update work-path: A4 (Architecture Design) → A5 (Units Generation, if needed) → Inception Review → Construction Loop
4. Record the backtracking decision according to `../logging/audit-management.md` Recovery Decision Requirements with `workflow_context: loop-level-backtrack`

---

# Workflow Proposal

Path Finder MUST present the workflow proposal with:
- Workflow profile selected and rationale
- Project context and execution strategy
- Activity sequence with selection rationale
- All three mandatory review checkpoints confirmed as included
- Organization tailoring applied (if any)
- User confirmation request

---

# User Confirmation

Path Finder MUST wait for explicit user confirmation.

If rejected:
- Record rejection reason according to `../logging/audit-management.md` Change Log Requirements
- Regenerate workflow based on user feedback
- Present updated proposal
- Limit to 3 iterations per session

If approved:
- Create and write `ai-sdlc-docs/inception/plans/work-path.md` in the table format specified above
- Record approval decision according to `../logging/audit-management.md` Change Log Requirements and Decision Log Requirements

---

# Decision Recording

Path Finder MUST record decisions according to:
- `../logging/audit-management.md` for audit entries (Decision Log, Change Log, and applicable requirement categories)
- `ai-sdlc-docs/state/state-log.md` for workflow state updates

Specific requirements are defined in `../logging/audit-management.md` Decision Trigger Matrix and requirement sections.