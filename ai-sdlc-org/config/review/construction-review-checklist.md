# Construction Review Checklist

## Purpose
Verify that required Construction deliverables are complete. This checklist is **customizable by business unit** to define which deliverables are mandatory.

## How to Use This Checklist

1. **Business Unit Customization**: Define your required deliverables below (Construction phase outputs)
2. **Execution**: Before Construction Review, verify each deliverable exists and meets the check level defined by your business unit
3. **Recording**: Mark items as ✓ (complete), ✗ (missing), or ⊘ (N/A with justification)
4. **Review**: construction-review.md uses this checklist to determine PASS/BLOCKED/FAIL

---

## Customization Guide: Check Levels

**Level 1: Existence Only (Lightest)**
- ✓ File exists at specified path
- Use when: You only care that the deliverable was created

**Level 2: Existence + Basic Content (Medium)**
- ✓ File exists at specified path
- ✓ File is not empty (minimum 100 characters)
- ✓ Contains required sections (e.g., ## Summary, ## Results)
- Use when: You want to ensure minimum content quality

**Level 3: Existence + Content + Structure (Strictest)**
- ✓ File exists at specified path
- ✓ File is not empty
- ✓ Contains all required sections
- ✓ Contains all required fields (e.g., test_result, pass_count, fail_count)
- ✓ Content is substantive (not just headers)
- Use when: You need high-quality, structured deliverables

---

## Required Deliverables (Customize This Section)

### 1. Detailed Design Document

**Evidence Location:** `ai-sdlc-docs/construction/implementation-design/{unit-name}-implementation-design.md`

**Recommended Check Level:** Level 2

**Example Customization (Level 2):**
```yaml
Minimum Checks:
  - ✓ File exists
  - ✓ File not empty
  - ✓ Contains section: ## Architecture
  - ✓ Contains section: ## Implementation Details
```

**Example Customization (Level 3):**
```yaml
Minimum Checks:
  - ✓ File exists
  - ✓ File not empty
  - ✓ Contains section: ## Architecture
  - ✓ Contains section: ## Implementation Details
  - ✓ Contains section: ## Database Schema (if applicable)
  - ✓ Contains field: trade_offs (list)
  - ✓ Contains field: known_limitations (list)
```

**Status:** ☐

---

### 2. System Test Results

**Evidence Location:** `ai-sdlc-docs/construction/test/test-report.md`

**Recommended Check Level:** Level 2

**Example Customization (Level 2):**
```yaml
Minimum Checks:
  - ✓ File exists
  - ✓ File not empty
  - ✓ Contains section: ## Summary
  - ✓ Contains section: ## Results
  - ✓ Contains field: test_verdict (PASS / FAIL / BLOCKED)
```

**Example Customization (Level 3):**
```yaml
Minimum Checks:
  - ✓ File exists
  - ✓ File not empty
  - ✓ Contains section: ## Test Scope
  - ✓ Contains section: ## Test Results
  - ✓ Contains section: ## Failed Tests (if any)
  - ✓ Contains field: total_tests (number)
  - ✓ Contains field: passed_tests (number)
  - ✓ Contains field: failed_tests (number)
  - ✓ Contains field: test_verdict (PASS / FAIL / BLOCKED)
```

**Status:** ☐

---

### 3. Code Implementation Summary

**Evidence Location:** `ai-sdlc-docs/construction/code/{unit-name}-code-generation-summary.md`

**Recommended Check Level:** Level 1

**Example Customization (Level 1):**
```yaml
Minimum Checks:
  - ✓ File exists
```

**Example Customization (Level 2):**
```yaml
Minimum Checks:
  - ✓ File exists
  - ✓ File not empty
  - ✓ Contains section: ## Summary
  - ✓ Contains section: ## Code Changes
```

**Example Customization (Level 3):**
```yaml
Minimum Checks:
  - ✓ File exists
  - ✓ File not empty
  - ✓ Contains section: ## Summary
  - ✓ Contains section: ## Code Changes (with file paths)
  - ✓ Contains section: ## Build Status
  - ✓ Contains field: build_status (PASS / FAIL)
  - ✓ Build status is PASS (not FAIL)
```

**Status:** ☐

---

### 4. DALi Component Implementation Compliance

**Condition:** Only required when `profile_id == dali_component_feature`

**Evidence Locations:**

- `ai-sdlc-docs/inception/architecture/architecture-design.md`
- `ai-sdlc-docs/construction/implementation-design/{unit-name}-implementation-design.md`
- `ai-sdlc-docs/construction/code/{unit-name}-code-verification-summary.md`
- `ai-sdlc-docs/construction/test/test-report.md`
- `ai-sdlc-docs/audit/audit.md`

**Recommended Check Level:** Level 3

```yaml
Minimum Checks:
  - ✓ Public and implementation base hierarchy matches the approved design
  - ✓ Public handles remain lightweight and API/ABI constraints are satisfied
  - ✓ Component style, builder, factory, and UiConfig lifecycle rules verified
  - ✓ Ubuntu execution environment and exact build/test commands recorded
  - ✓ OneUIComponents reference behavior covered by tests or documented evidence
  - ✓ Reference commit and intentional differences recorded
  - ✓ Applicable wiki, samples, and tests updated
```

**Status:** ☐

---

### 5. DALi API Change Implementation Compliance

**Condition:** Only required when `profile_id == org_standard` and a public,
extension, or integration API contract is affected

**Evidence Locations:**

- `ai-sdlc-docs/construction/implementation-design/{unit-name}-implementation-design.md`
- `ai-sdlc-docs/construction/code/{unit-name}-code-verification-summary.md`
- `ai-sdlc-docs/construction/test/test-report.md`

**Recommended Check Level:** Level 3

```yaml
Minimum Checks:
  - ✓ Actual API changes match the approved design and API change inventory
  - ✓ Public and extension ABI constraints verified against the header diff
  - ✓ Public handles, implementation state, exports, and dependencies comply with rules
  - ✓ Applicable validation checks executed with results recorded
  - ✓ Current dali-ui built and installed before affected tests executed
  - ✓ Tests, samples, manual-tests, and wiki updated where applicable
  - ✓ No unapproved API or ABI break remains
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
- Evidence exists but contradicts requirements (e.g., test_verdict = FAIL)
- OR user explicitly rejects output quality

---

## Checklist Completion Record

| Item # | Deliverable | Check Level | Status | Evidence File | Notes |
|--------|---|---|---|---|---|
| 1 | Detailed Design Document | [Level 1/2/3] | | | |
| 2 | System Test Results | [Level 1/2/3] | | | |
| 3 | Code Implementation Summary | [Level 1/2/3] | | | |
| 4 | DALi Component Implementation Compliance | Level 3 / N/A | | | `dali_component_feature` only |
| 5 | DALi API Change Implementation Compliance | Level 3 / N/A | | | `org_standard` API changes only |

---

## AI Evaluation

construction-review.md loads this checklist and evaluates each deliverable:
1. Check file existence at Evidence Location
2. If Level 2+: Verify required sections/fields exist
3. Mark: ✓ COMPLETE / ✗ MISSING / ⊘ N/A
4. Verdict: All ✓ or ⊘ justified → PASS; any ✗ → BLOCKED

---

## Adding Custom Deliverables

To add another deliverable, follow this template and use the next item number:

```markdown
### N. [Your Deliverable Name]

**Evidence Location:** `ai-sdlc-docs/construction/[your-file].md`

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
