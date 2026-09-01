# Artifact Generation Rules

## Purpose

This document defines how AI produces **valid, logically sound artifacts**. It governs artifact **structure, content quality, and internal consistency**.

**Naming Convention Reference:** All artifact identifiers (REQ, DEC, UNIT, etc.) and file/directory naming MUST follow [[naming-rules]] — see that document for the complete naming standard.

For **execution integrity, verification, and gate control**, see `ai-execution-rule.md` and `ai-sdlc.md`.

---

## Core Principle

Artifacts must be grounded in upstream inputs, claims must be evidence-based, and uncertainty must be explicitly named.

---

## Rule 1 — Artifact Structure & Clarity

**Artifacts must follow templates and have clear, unambiguous structure.**

Before producing an artifact:

1. **Follow the template** — Use corresponding template from `templates/`
   - Example: Use `02-requirements-analysis.md` template when writing requirements-analysis
   - Template provides structure, sections, and format standards
   
2. **Format matches work type** — requirements-analysis artifact has different structure than implementation-design

3. **Each step is clear and executable** — no vague tasks like "handle issue" or "do validation"
   - Bad: "Validate the approach"
   - Good: "Check that all N endpoints accept GET/POST as documented"

4. **Criteria are testable and observable** — acceptance criteria must be measurable, not subjective
   - Bad: "The code quality is good"
   - Good: "All functions < 50 lines; cyclomatic complexity ≤ 5"

**Check:** If format/structure fails, stop and consult template before proceeding.

---

## Rule 2 — Grounding & Traceability

**Artifacts must be grounded in upstream inputs and maintain traceability via traceability-matrix.md.**

Before producing a downstream artifact:

1. **Read upstream artifacts** in the current session (do not rely on prior-session knowledge)
   - Example: Read `requirements-analysis.md` and `architecture-design.md` before writing `implementation-design.md`
2. **If upstream not read in current session**: artifact is ungrounded (lacks foundation for claims)
   - Artifact must be regenerated after reading upstream
3. **Grounding check**: Artifact header or audit entry should note which upstream files were read

### Traceability requirement

1. **Maintain traceability per traceability-matrix.md**
   - Cite upstream artifact ID/section in your artifact
   - Add or update corresponding entry in traceability-matrix.md
   
2. **Example mapping**:
   ```
   REQ-5.2 (Requirements) 
   → AC-201 (Acceptance Criteria)
   → Architecture §3.2 (Design Decision)
   → U5/impl-design.md (Implementation)
   → Test #7 (Verification)
   ```

3. **Untraced claim**
   - If a claim/decision has no upstream reference, mark it as `assumed` per Rule 3
   - Or add the upstream reference to traceability-matrix.md

**Check:** Upstream artifacts were read in current session; artifact cites upstream ID/section; traceability-matrix.md updated.

---

## Rule 3 — Uncertainty Must Be Named Explicitly

**All unverified facts must be explicitly marked with uncertainty phrases.**

Use one of these when uncertain about a fact:
- `unverified — must confirm by [action]`
- `assumed — [basis]; verify before using`
- `untestable — [reason]`
- `chained-unverified — depends on [upstream artifact/assumption]`

For analysis confidence: use **confirmed**, **likely**, or **hypothesis under investigation**.

**Propagation:** If upstream artifact has `assumed` facts, downstream must mark them as `chained-unverified`. Never treat inherited assumptions as confirmed without independently verifying them.

**Check:** All inherited assumptions from upstream are explicitly marked `chained-unverified`.

---

## Rule 4 — Claims Must Have Evidence

**Any execution, testing, or verification claim requires observable evidence from the current session.**

Execution claims requiring evidence:
- "tests pass" / "build succeeded" / "file was created"
- "I verified X" / "confirmed by testing"

Banned phrases (require evidence to use):
- "tests should/would pass"
- "code is correct therefore tests pass"
- "manually verified"
- "all acceptance criteria met" (without linking to evidence)

If you cannot produce evidence: State `Not executed — [reason]` instead of marking as verified or tested.

**Check:** Every execution claim has evidence, or is marked `Not executed`.

---

## Rule 5 — Artifacts Are Drafts Until Approved

**AI-generated artifacts are drafts until explicit user approval; self-approval is prohibited.**

Before approval, artifact is NOT:
- Confirmed fact or requirement
- Approved design decision
- Verified result or evidence
- Input for the next phase

The AI may not assume approval by: proceeding without waiting, saying "I'll assume", or inferring from silence.

**Check:** Artifact is presented as draft; no assumption of approval made.

---

## Rule 6 — Plans & Checklists

**Plans and checklists must be concrete and unambiguous, separating planned work from completed evidence.**

Generated plans and checklists must:
- **Use ordered steps** for sequence-critical work
- **Use checkboxes only for execution tracking** (not plan structure)
- **Avoid vague language** — every step must have a clear, measurable outcome
  - Bad: "Set up the build"
  - Good: "Install [X] v1.2.3, run `npm install`, verify `build.sh` completes without error"
- **Separate planned work from completed evidence** — do not mix "what we will do" with "what we did"

**Check:** Every step is concrete and unambiguous; no "handle issue" or "do validation" tasks.

---

## Rule 7 — Multi-Unit Artifacts

**For multi-unit artifacts, each unit must independently meet all rules; no aggregation or "mostly complete" states.**

For artifacts covering multiple units (e.g., code-generation across 5 files):

1. **Report each unit separately** with its own structure and acceptance criteria
2. **Do not aggregate** — e.g., "4 of 5 units ready" is not a valid artifact state
3. **If any unit is incomplete or uncertain**, the overall artifact is incomplete
4. **Grounding applies per-unit** — if one unit's upstream was not read, that unit's output is ungrounded

**Check:** Each unit is independently valid per Rules 1-6; no unit is aggregated or marked "mostly done".
