# Recovery Paths Rule

## Purpose
Define how to analyze and diagnose Review failures. Recovery is part of the workflow, not an exception.

## Core Principle
A BLOCKED or FAIL Review is not a failure of the workflow — it is the workflow doing its job. The workflow's job is to make problems visible before release.

## Common Recovery Procedure

For any BLOCKED or FAIL verdict:

1. **Analyze failure**: Identify failed checklist item and root cause
2. **Assess impact**: Determine which activities/artifacts are affected
3. **Document findings**: Record analysis in audit log
4. **Handoff to Path Finder**: Pass analysis to Path Finder
5. **Path Finder updates work-path**: Determines recovery workflow and activity sequence
6. **Execute updated work-path**: Re-run from the recovery point
7. **Re-run review**: Verify recovery succeeded

## Inception Review Recovery

When Inception Review is BLOCKED or FAIL, analyze the failure:

| Failure | Root Cause | Affected Artifact | Path Finder Action |
|---|---|---|---|
| Unclear requirements | Requirements elaboration incomplete | requirements-brief.md | Update work-path: re-run A1 and/or A2 |
| Invalid acceptance criteria | Requirements analysis incomplete | requirements-analysis.md | Update work-path: re-run A2 |
| Architecture issue | Architecture design flawed | architecture-design.md | Update work-path: re-run A4 and A5 |
| Unit split issue | Units generation incomplete or invalid | unit-generation.md | Update work-path: re-run A5 |
| Missing approval | Approval not yet received | (any artifact) | Wait for approval; no activity change needed |

**Procedure**: 
1. Identify which artifact failed validation
2. Document the specific issue according to `logging/audit-management.md`
3. Pass failure analysis to Path Finder
4. Path Finder updates work-path to re-run the affected activity
5. Execute updated work-path starting from the affected activity

## Integration Ready Check Recovery

### Verdict: BLOCKED

When Integration Ready Check is BLOCKED, analyze the missing item:

| Missing Item | Root Cause | Affected Unit(s) | Path Finder Action |
|---|---|---|---|
| Unit not completed | Code generation or verification incomplete | Single unit | Update work-path: re-run C2 or C3 for that unit |
| Unit test evidence missing | Tests not executed | Single unit | Update work-path: re-run C3 for that unit |
| Unit test FAIL | Implementation or test logic incorrect | Single unit | Update work-path: re-run C2 or C3 for that unit |
| Unit PR not open (team mode) | PR workflow skipped | Single unit | Update work-path: ensure C2/C3 includes PR creation |
| Integration Owner not assigned (team mode) | Ownership unclear | Multiple units | Assign Integration Owner; no activity change |
| Unit Summary State stale (team mode) | State tracking out of sync | Multiple units | Update unit-generation.md; no activity change |
| Cross-unit dependencies unresolved | Interface contract conflicts | Multiple units | Document dependency; consult Path Finder for resolution strategy |

**Procedure**:
1. Identify which unit(s) or integration point failed
2. Document the specific blocker according to `logging/audit-management.md`
3. Pass analysis to Path Finder
4. Path Finder updates work-path for the affected unit(s)
5. Execute updated work-path; re-run only affected units if multi-unit

### Verdict: FAIL

When Integration Ready Check is FAIL:

| Cause | Impact | Path Finder Action |
|---|---|---|
| Unit evidence contradicts itself (e.g., test says PASS but code unchanged) | Single or multiple units | Update work-path: re-run C2/C3 for affected units |
| User explicitly rejects unit readiness | Single unit | Update work-path per user feedback; re-run relevant activities |

**Procedure**:
1. Identify which unit(s) failed and why
2. Document contradiction or rejection according to `logging/audit-management.md`
3. Pass analysis to Path Finder
4. Path Finder updates work-path
5. Execute updated work-path

## Construction Review Recovery

When Construction Review is BLOCKED or FAIL, analyze the failure:

| Failure | Root Cause | Affected Artifact | Path Finder Action |
|---|---|---|---|
| Missing implementation | Code generation incomplete | code-generation-summary.md | Update work-path: re-run C2 |
| Missing test evidence | Code verification incomplete | code-verification-summary.md | Update work-path: re-run C3 |
| Build failure | Build or integration test failed | build artifacts | Update work-path: re-run C4 |
| Traceability broken | Design-to-code mapping incomplete | implementation-design.md | Update work-path: re-run C1 or C2 |
| Release not ready | Release preparation incomplete | release-note.md | Update work-path: re-run C5 |

**Procedure**:
1. Identify which activity/artifact failed validation
2. Document the specific issue according to `logging/audit-management.md`
3. Pass failure analysis to Path Finder
4. Path Finder updates work-path to re-run the affected activity
5. Execute updated work-path starting from the affected activity

## Backtracking Rules

### Minimum Backtrack Principle
- Backtrack only to the activity that produced the problematic artifact
- Do NOT restart inception unless requirements themselves were wrong
- Do NOT redo unaffected units in multi-unit work — only redo the affected unit(s)

### Recording a Backtrack Decision

When Recovery-Paths identifies a failure requiring backtrack:
1. Document according to `logging/audit-management.md`:
   - Trigger: which Review failed
   - Analysis: why it failed
   - Affected artifact(s): what needs fixing
   - Root cause activity: which activity to re-run
2. Record as `DEC-###` entry (per `logging/audit-management.md`):
   - `trigger_source: recovery`
   - `workflow_context: recovery-backtrack`

### Loop-Level Backtracking

When Construction Loop reveals that **the architecture itself is wrong**:

1. **Identify architectural issue** in construction activity output
2. **Document according to `logging/audit-management.md`**: specific architectural problem
3. **Assess scope**: which units/components are affected
4. **Pass to Path Finder**: "Architecture needs re-design; affected components: X, Y"
5. **Path Finder updates work-path**: 
   - Return to A4 (Architecture Design)
   - Re-derive A5 (Units Generation) if structure changed materially
   - Re-enter Construction Loop with updated architecture
6. **Record as major `DEC-###` entry** (per `logging/audit-management.md`): `workflow_context: loop-level-backtrack`

**Note**: This is expected and not a failure. Better to discover architecture problems during construction than after release.

For continuation workflows encountering loop-level backtrack, also see `loop-continuation.md` — this may trigger re-derivation of prior units or reverse-engineering.

## Time-Boxing Recovery

If recovery cycles exceed any of the following, escalate to the user:
- > 3 recovery cycles on the same Review
- > 1 loop-level backtrack in a single workflow
- > 1 day of wall-clock time blocked on a single recovery item

Escalation format:
```
Recovery is taking longer than expected.
Failed Review {Inception | Construction}
Cycle count: {N}
Root issue: {short description}

Options:
A. Continue current recovery
B. Escalate scope (e.g., bigger architecture change)
C. Reduce scope (drop part of the work)
D. Pause workflow
```

## Anti-Patterns
- ❌ Marking a BLOCKED Review as PASS to "unblock" the workflow — this defeats the entire SDLC
- ❌ Restarting from scratch when a single task produced a bad output
- ❌ Skipping a Review "because we know it would pass"
- ❌ Recovery without recording required audit evidence
- ❌ Weakening tests to make them pass
- ❌ Backtracking beyond the smallest affected task
- ❌ Proceeding with recovery without defining backtrack scope