# AI Execution Rules

## Purpose

This document defines **how to execute** activities in the AI-SDLC workflow. All activity execution must follow these rules.

For **artifact content quality standards**, see `artifact-generation-rule.md`.
For **workflow orchestration and sequencing**, see `ai-sdlc.md`.

## Part 1: Core Execution Principles

### Principle 1: Evidence-Based Judgment
All execution decisions, verdicts, and claims require observable evidence from this session.

- The AI ran the command/test/verification in this session using a tool
- The output is visible in this session's context
- Does NOT count: prior-session execution, inferred outcomes, theoretical behavior, code review alone, reasoning

Only execution results count as evidence. Reasoning, code review, theoretical behavior, and prior-session assumptions DO NOT count.

- "Code looks good, therefore tests pass" ❌
- "This should work" ❌
- "Tests executed and passed" (with output) ✅

### Principle 2: Approval is a Gate
When a rule or gate requires explicit approval, AI MUST stop and wait. Do not assume approval from silence, context, or questions.

### Principle 3: State is Authoritative
Truth is in state-log.md and audit.md. When in-context belief conflicts with state files, trust the files and resolve the conflict.

## Part 2: Execution & Verification

### Running Commands and Tests
- Execute commands explicitly in the session
- Capture raw output (do not paraphrase)
- If execution fails: state `Not executed — [reason]` instead of describing what would happen

### No Fabricated Results
Do NOT use these phrases without execution evidence:
- "tests pass" / "tests should pass" / "tests would pass"
- "I verified X" / "code is correct therefore tests pass"
- "manually verified"
- "all acceptance criteria met" (without linking to test evidence)

### Result States (only 3 valid)
| State | Condition |
|-------|-----------|
| **PASS** | Executed and all criteria met |
| **FAIL** | Executed and one or more criteria failed |
| **BLOCKED** | Not executed (evidence missing, approval pending, upstream incomplete) |

### Multi-Unit Aggregation Rule
- Report each unit's result separately
- Cannot aggregate ("4 of 5 pass" is invalid)
- If ANY unit is BLOCKED or FAIL, overall verdict is BLOCKED or FAIL

### Artifact Generation
Follow `artifact-generation-rule.md` entirely when producing any artifact. Artifact is valid only if all rules pass.

## Part 3: Subagent & Delegation Patterns

When the environment supports subagents:
- **Parallel exploration**: brownfield reverse-engineering → spawn subagents for different code areas concurrently
- **Independent code generation**: multi-unit work → each unit's code generation runs in parallel
- **Independent verification**: spawn separate subagent for code review (unbiased verification after tests)
- **Anti-pattern**: do NOT spawn subagents for tasks requiring sustained user dialog (requirements, approval gates)

## Part 4: Tool-Use Hierarchy

Prefer tools in this order:
1. **Dedicated file tools** (Read, Edit, Write) over shell `cat`/`sed`/`echo`
2. **Specialized search tools** over `find` + `grep` pipelines
3. **Project-specific MCP servers** over generic shell commands
4. **Shell commands** as last resort

Note tool usage in artifact audit entries.

## Part 5: Context Window Management

### Managing Limited Context
- **Keep in context**: state-log.md, requirements, architecture, current unit
- **Load on-demand**: templates, audit entries, reverse-engineering, other units
- **When full**: summarize completed work to loop-summary.md, then discard

(Tiered loading structure defined in ai-sdlc.md)

## Part 6: Token Budget Heuristics

Rough guidance per activity (assumes ~200K context):

| Activity | Max tokens |
|----------|-----------|
| Requirements brief + analysis | ≤ 4K |
| Workflow planning | ≤ 1K |
| Architecture design | ≤ 8K |
| Per-unit implementation design | ≤ 4K per unit |
| Per-unit code generation | ≤ 2K per unit |
| Test plan + report | ≤ 4K |
| Release note | ≤ 2K |

If artifact exceeds budget, decompose further (more units).

## Part 7: Determinism & Reproducibility

### For Automation Artifacts
Pin exact values:
- Exact commands (not "the usual build command")
- Exact file paths (not "the relevant config")
- Exact version numbers

### For Prose Artifacts
Determinism is NOT required. Clarity and traceability are the goal.

## Part 8: Gate Verdict Rules

**PASS conditions (ALL must be true):**
1. Artifact generated and valid per artifact-generation-rule.md
2. Execution evidence complete (PASS/FAIL states from Part 2)
3. Upstream artifacts are PASS or approved
4. User approval obtained (if gate requires approval)

**BLOCKED conditions (ANY apply):**
1. Artifact not generated
2. Artifact validation failed (artifact-generation-rule)
3. No execution evidence ("Not executed")
4. Upstream artifact BLOCKED or FAIL
5. User approval pending
6. Required tests/build not run

**FAIL conditions:**
1. Artifact generated but execution failed
2. Criteria not met (test failed, build failed, validation failed)
3. Root cause identified and documented

**After FAIL verdict:**
Do NOT proceed to next activity. Determination of recovery path and restart point is handled by recovery-path.md.

### Recording the Verdict
Gate verdict MUST be recorded in **three places with same verdict**:
- **state-log.md** (FACT: activity state change)
- **audit.md** (WHY: decision rationale)
- **traceability-matrix.md** (LINK: artifact completion)

See state-management.md, logging/audit-management.md, traceability-management.md for format.

## Part 9: Artifact Verification at Gate

Before declaring gate PASS, verify all `artifact-generation-rule.md` rules pass:

```
☐ Rule 1: Structure & Clarity
☐ Rule 2: Grounding & Traceability
☐ Rule 3: Uncertainty Naming
☐ Rule 4: Claims Have Evidence
☐ Rule 5: Draft Status
☐ Rule 6: Plans & Checklists
☐ Rule 7: Multi-Unit Consistency
```

If any check fails, gate is BLOCKED until resolved.

## Part 10: Metrics for state-log.md

Track per workflow:
- Workflow start/end time
- Activities executed, skipped (with reasons)
- Approval cycles per gate
- Drift items detected (continuation only)
- Final release readiness verdict

## Part 11: Failure Modes to Watch

1. **Scope creep** — added features not in scope_in
2. **Silent skipping** — skipped activity without recording in state-log
3. **Re-doing completed work** — re-running activity when prior work covers it
4. **Templates ignored** — artifact generated without consulting template
5. **Hallucinated paths** — paths in artifact not verified to exist
6. **Hallucinated test results** — reported test outcomes without execution
7. **Approval drift** — acting approved before getting explicit approval
8. **Uncertain facts as confident** — stated assumptions as facts without uncertainty phrases

Before each gate verdict, self-check against these modes.
