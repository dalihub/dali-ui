# Organization Naming Rules

Organization-specific naming conventions and standards for AI-SDLC artifacts, identifiers, and documentation.

---

## Workflow & Activity Identifiers

### Workflow ID Format

**Format**: `WF-YYYYMMDD-###`

**Rules**:
- Date component: workflow creation date (YYYY = year, MM = month, DD = day)
- Sequence component: 3-digit zero-padded counter per day (001, 002, 003...)
- Example: `WF-20260724-001`

**Reference**: See [[glossary]] > "Workflow"

---

### Activity IDs (Inception Loop)

| Activity | ID Format | Example | Naming Rule |
|----------|-----------|---------|-------------|
| Context Analysis | A0 | A0 | Fixed identifier |
| Requirements Elaboration | A1 | A1 | Fixed identifier |
| Requirements Analysis | A2 | A2 | Fixed identifier |
| Workflow Planning | A3 | A3 | Fixed identifier |
| Architecture Design | A4 | A4 | Fixed identifier |
| Units Generation | A5 | A5 | Fixed identifier |

**Rules**:
- Inception activities use `A#` prefix (A0–A5)
- ID is fixed and immutable; all workflows use the same activity IDs

**Reference**: See [[glossary]] > "Activities (11 total)"

---

### Activity IDs (Construction Loop)

| Activity | ID Format | Example | Naming Rule |
|----------|-----------|---------|-------------|
| Implementation Design | C1 | C1 | Fixed identifier |
| Code Generation | C2 | C2 | Fixed identifier |
| Code Verification | C3 | C3 | Fixed identifier |
| Build & Testing | C4 | C4 | Fixed identifier |
| Release | C5 | C5 | Fixed identifier |

**Rules**:
- Construction activities use `C#` prefix (C1–C5)
- ID is fixed and immutable; all workflows use the same activity IDs

**Reference**: See [[glossary]] > "Activities (11 total)"

---

### Unit of Work ID Format

**Format**: `UNIT-###`

**Rules**:
- 3-digit zero-padded counter (001, 002, 003...)
- Counter is workflow-specific (resets per workflow)
- Used in: A5 (Units Generation) output → units/unit-spec.md, unit-of-work-plan.md

**Example**: `UNIT-001`, `UNIT-042`

**Reference**: See [[glossary]] > "Identifiers"

---

## Requirement & Decision Identifiers

### Requirement ID Format

**Format**: `REQ-###`

**Rules**:
- 3-digit zero-padded counter (001, 002, 003...)
- Counter is workflow-specific (resets per workflow)
- Used in: A2 (Requirements Analysis) output → requirements-analysis.md

**Example**: `REQ-001`, `REQ-042`

**Reference**: See [[glossary]] > "Identifiers"

---

### Decision ID Format

**Format**: `DEC-###`

**Rules**:
- 3-digit zero-padded counter (001, 002, 003...)
- Counter is workflow-specific (resets per workflow)
- Used in: audit.md across all activities
- Each significant decision (architectural, trade-off, risk mitigation) records its own ID

**Example**: `DEC-001`, `DEC-013`

**Reference**: See [[glossary]] > "Identifiers"

---

### Defect ID Format

**Format**: `DEF-###`

**Rules**:
- 3-digit zero-padded counter (001, 002, 003...)
- Counter is workflow-specific; only used in `DEFECT_FIX` work type
- Used in: traceability-matrix.md and defect analysis artifacts
- Links defect to root cause and corrective action in audit.md

**Example**: `DEF-001`, `DEF-019`

**Reference**: See [[glossary]] > "Identifiers"

---

### Test Case ID Format

**Format**: `TC-###`

**Rules**:
- 3-digit zero-padded counter (001, 002, 003...)
- Counter is workflow-specific
- Used in: C4 (Build & Testing) output → test-plan.md, test-report.md
- Links each test case to one or more Acceptance Criteria

**Example**: `TC-001`, `TC-101`

**Reference**: See [[glossary]] > "Identifiers"

---

### Audit Entry ID Format

**Format**: `{PREFIX}-###`

Audit entries created by review gates, integration checks, and user transitions use dedicated prefixes (distinct from the `DEC-###` decision log):

| Prefix | Entry Type | Created By |
|--------|-----------|------------|
| `INC-REV-###` | Inception Review verdict | review-rule.md (loop=inception) |
| `CON-REV-###` | Construction Review verdict | review-rule.md (loop=construction) |
| `INT-READY-###` | Integration Ready Check verdict | integration-ready-check-rule.md |
| `USR-TRANS-###` | User transition decision after a gate | review-protocol.md (Phase 5) |

**Rules**:
- 3-digit zero-padded counter (001, 002, 003...)
- Counter is workflow-specific, incremented per entry of that prefix
- Used in: `ai-sdlc-docs/audit/audit.md`
- Always use the `###` zero-padded form (not `[001]`, not bare `#`)

**Example**: `INC-REV-001`, `INT-READY-002`, `USR-TRANS-001`

**Reference**: See [[glossary]] > "Identifiers"

---

## File & Directory Naming

### Unit Name Format

**Format**: `{unit-id}` or `{unit-id}-{descriptive-name}`

**Rules**:
- Minimum: Unit ID only (e.g., `UNIT-001`)
- Recommended: Unit ID + descriptive name (e.g., `UNIT-001-auth-service`)
- Use lowercase with hyphens for descriptive part
- Keep descriptive names short (2–4 words max)

**Example Files**:
- Minimum: `UNIT-001-implementation-design.md`
- Recommended: `UNIT-001-auth-service-implementation-design.md`

---

## Skill & Activity Naming

### Skill Directory Format

**Format**: `ai-sdlc-{org}/skills/{activity-type}/{subdomain}/SKILL.md`

**Rules**:
- Activity type uses kebab-case (e.g., `requirements-elaboration`, `code-verification`)
- Subdomain is optional (used when multiple implementations exist)
- SKILL.md file name is fixed (uppercase, no variation)

**Reference**: See [[config/workflow/activity]] > "Activity Mapping"

---

## Documentation & Comments

### Comment Conventions

**Rules**:
- Avoid unnecessary comments; well-named identifiers should be self-documenting
- Comments explain the WHY, not the WHAT
- Keep comments short (single line preferred)
- Examples of good comments:
  - "// Skip A0 because artifact status is CLEAN"
  - "// DEC-001: Architecture reuse reduces implementation risk by 30%"

---

### Cross-Document References

**Format**: `[[filename]]` or `[[filename]] > "Section Name"`

**Rules**:
- Use wiki-style bracket notation for internal links
- Reference the base filename (no .md extension)
- Include section name in quotes if referencing a specific section
- Examples:
  - `See [[glossary]] for terminology`
  - `See [[config/workflow/workflow]] > "Review Checkpoints"`
  - `Reference: [[glossary]] > "Identifiers"`

---

## Custom Organization Tailoring

### Profile ID Format

**Format**: `org_{profile_type}` or `org_{team}_{profile_type}`

**Rules**:
- Use lowercase with underscores
- Examples: `org_standard`, `org_advanced_sw`, `org_data_team_standard`
- Defined in [[config/workflow/workflow]]

**Reference**: See [[config/workflow/workflow]] > "Workflow Paths"

---

### Custom Execution Modes

**Rules**:
- Document custom modes in [[config/workflow/workflow]] > "Custom Execution Modes"
- Follow the same naming conventions as standard modes (lowercase, hyphens)
- Include condition, purpose, and execution sequence in documentation

---

## Version & Metadata

### Document Version Format

**Format**: Semantic versioning in frontmatter: `version: X.Y.Z`

**Rules**:
- Major (X): Breaking changes to artifact structure or required fields
- Minor (Y): New optional fields or non-breaking enhancements
- Patch (Z): Documentation updates or clarifications
- Include version in frontmatter of all rule files and templates
- Update version when document is modified

**Example**:
```markdown
---
title: Architecture Design Rule
version: 1.2.3
owner: AI-SDLC Team
---
```

---

### Ownership & Governance

**Rules**:
- All **SKILL** files must include `owner:` in frontmatter
- Example: `owner: SR-AX-LAB`, `owner: Platform Engineering`
- Owner is responsible for maintaining accuracy and consistency
- Update owner field when ownership changes

---

## Validation & Consistency

### Rule Violations

**When naming rules are violated**:
1. Document the violation in audit.md with context
2. Flag for resolution in next workflow or process improvement
3. Update naming rules if needed to address the violation

**Examples of common violations**:
- Using informal IDs instead of standard format (e.g., `req1` instead of `REQ-001`)
- Inconsistent state values (e.g., `In Development` instead of `in-development`)
- Missing or incorrect review verdict format

---

## FAQ

**Q: Can we shorten identifiers for simplicity?**  
A: No. Standard format (REQ-001, not req1) ensures consistency across all workflows and tools.

**Q: What if we need new identifier types?**  
A: Propose the new type in config/workflow/workflow.md, update this document, and apply to future workflows.

**Q: How do we handle multi-workflow traceability?**  
A: Each workflow maintains separate REQ/DEC/UNIT counters. Cross-workflow links are recorded in traceability-matrix.md.

---

## Related Documents

- [[config/workflow/workflow]] — Workflow paths and review checkpoints
- [[glossary]] — Terminology and definitions
- [[config/workflow/activity]] — Activity mappings and governance
