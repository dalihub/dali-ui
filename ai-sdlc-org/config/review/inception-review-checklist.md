# Inception Review Checklist

## Purpose
Verify that required Inception deliverables are complete. This checklist is **customizable by business unit** to define which deliverables are mandatory.

## How to Use This Checklist

1. **Business Unit Customization**: Define your required deliverables below (Inception phase outputs)
2. **Execution**: Before Inception Review, verify each deliverable exists and meets the check level defined by your business unit
3. **Recording**: Mark items as ✓ (complete), ✗ (missing), or ⊘ (N/A with justification)
4. **Review**: inception-review.md uses this checklist to determine PASS/BLOCKED/FAIL

---

## Customization Guide: Check Levels

**Level 1: Existence Only (Lightest)**
- ✓ File exists at specified path
- Use when: You only care that the deliverable was created

**Level 2: Existence + Basic Content (Medium)**
- ✓ File exists at specified path
- ✓ File is not empty (minimum 100 characters)
- ✓ Contains required sections (e.g., ## Purpose, ## Scope)
- Use when: You want to ensure minimum content quality

**Level 3: Existence + Content + Structure (Strictest)**
- ✓ File exists at specified path
- ✓ File is not empty
- ✓ Contains all required sections
- ✓ Contains all required fields (e.g., risk_list, decision_list)
- ✓ Content is substantive (not just headers)
- Use when: You need high-quality, structured deliverables

---

## Required Deliverables (Customize This Section)

### 1. Requirements Document

**Evidence Location:** `ai-sdlc-docs/inception/requirements/requirements-analysis.md`

**Recommended Check Level:** Level 2

**Example Customization (Level 2):**
```yaml
Minimum Checks:
  - ✓ File exists
  - ✓ File not empty
  - ✓ Contains section: ## Purpose
  - ✓ Contains section: ## Scope
```

**Example Customization (Level 3):**
```yaml
Minimum Checks:
  - ✓ File exists
  - ✓ File not empty
  - ✓ Contains section: ## Purpose
  - ✓ Contains section: ## Scope
  - ✓ Contains field: acceptance_criteria (list)
  - ✓ Contains field: out_of_scope (list)
```

**Status:** ☐

---

### 2. Design Document

**Evidence Location:** `ai-sdlc-docs/inception/architecture/architecture-design.md`

**Recommended Check Level:** Level 2

**Example Customization (Level 2):**
```yaml
Minimum Checks:
  - ✓ File exists
  - ✓ File not empty
  - ✓ Contains section: ## Architecture
  - ✓ Contains section: ## Risks & Assumptions
```

**Example Customization (Level 3):**
```yaml
Minimum Checks:
  - ✓ File exists
  - ✓ File not empty
  - ✓ Contains section: ## Architecture
  - ✓ Contains section: ## Components
  - ✓ Contains section: ## Risks & Assumptions
  - ✓ Contains field: risk_list (list with severity)
```

**Status:** ☐

---

### 3. Unit Definition (if multi-unit)

**Evidence Location:** `ai-sdlc-docs/inception/units/unit-generation.md`

**Condition:** Only required if `multi_unit = true` in state-log.md

**Recommended Check Level:** Level 2

**Example Customization (Level 2):**
```yaml
Minimum Checks:
  - ✓ File exists
  - ✓ File not empty
  - ✓ Contains section: ## Unit Summary
  - ✓ Contains field: unit_list (table with unit_id, owner, dependencies)
```

**Example Customization (Level 3):**
```yaml
Minimum Checks:
  - ✓ File exists
  - ✓ File not empty
  - ✓ Contains section: ## Unit Summary
  - ✓ Contains field: unit_list (table with unit_id, name, owner, dependencies, state)
  - ✓ Contains section: ## Cross-Unit Dependencies
  - ✓ All units have assigned owners
```

**Status:** ☐

---

### 4. DALi Component Design Compliance

**Condition:** Only required when `profile_id == dali_component_feature`

**Evidence Locations:**

- `ai-sdlc-docs/inception/requirements/requirements-analysis.md`
- `ai-sdlc-docs/inception/architecture/architecture-design.md`

**Recommended Check Level:** Level 3

```yaml
Minimum Checks:
  - ✓ OneUIComponents reference branch and commit recorded
  - ✓ Corresponding reference component identified, or approved absence documented
  - ✓ Component behavior and acceptance criteria mapped from the reference
  - ✓ Public and implementation base classes selected with rationale
  - ✓ Component style and configuration approach defined
  - ✓ Stateful-style TBD areas do not introduce an unapproved shared contract
  - ✓ Intentional differences from the reference documented
```

**Status:** ☐

---

### 5. DALi API Change Design Compliance

**Condition:** Only required when `profile_id == org_standard` and a public,
extension, or integration API contract is affected

**Evidence Locations:**

- `ai-sdlc-docs/inception/reverse-engineering/api-documentation.md`
- `ai-sdlc-docs/inception/requirements/requirements-analysis.md`
- `ai-sdlc-docs/inception/architecture/architecture-design.md`

**Recommended Check Level:** Level 3

```yaml
Minimum Checks:
  - ✓ Affected API level and consumers identified
  - ✓ Existing declarations, subclasses, call sites, and ABI-sensitive layout analyzed
  - ✓ Change classified as ordinary work or explicit pre-release redesign
  - ✓ Compatibility, ABI, migration, and documentation impact recorded
  - ✓ Target API and Handle-Body structure comply with applicable rules
  - ✓ Any intentional API or ABI break has an explicit approved decision
```

**Status:** ☐

---

## Verdict Determination

### PASS Conditions
- All defined deliverables are ✓ (complete at chosen check level)
- OR items are ⊘ (N/A) with documented justification
- AND no evidence contradicts requirements

### BLOCKED Conditions
- One or more defined deliverables are ✗ (missing or insufficient)
- AND cannot be marked N/A with justification

### FAIL Conditions
- Evidence exists but contradicts requirements
- OR user explicitly rejects output quality

---

## Checklist Completion Record

| Item # | Deliverable | Check Level | Status | Evidence File | Notes |
|--------|---|---|---|---|---|
| 1 | Requirements Document | [Level 1/2/3] | | | |
| 2 | Design Document | [Level 1/2/3] | | | |
| 3 | Unit Definition | [Level 1/2/3] | | | |
| 4 | DALi Component Design Compliance | Level 3 / N/A | | | `dali_component_feature` only |
| 5 | DALi API Change Design Compliance | Level 3 / N/A | | | `org_standard` API changes only |

---

## AI Evaluation

inception-review.md loads this checklist and evaluates each deliverable:
1. Check file existence at Evidence Location
2. If Level 2+: Verify required sections/fields exist
3. Mark: ✓ COMPLETE / ✗ MISSING / ⊘ N/A
4. Verdict: All ✓ or ⊘ justified → PASS; any ✗ → BLOCKED

---

## Adding Custom Deliverables

To add another deliverable, follow this template and use the next item number:

```markdown
### N. [Your Deliverable Name]

**Evidence Location:** `ai-sdlc-docs/inception/[your-file].md`

**Condition:** [Always / Only if X = Y]

**Recommended Check Level:** Level [1/2/3]

**Example Customization (Level 2):**
\`\`\`yaml
Minimum Checks:
  - ✓ File exists
  - ✓ File not empty
  - ✓ Contains section: ## [Section Name]
\`\`\`

**Status:** ☐
```

Then update the Checklist Completion Record table above.
