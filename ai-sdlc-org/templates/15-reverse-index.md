---
artifact_type: REVERSE_INDEX
---

# Reverse Engineering Index

## Overview
- work type:
- workspace mode: brownfield
- depth: impact-focused / path-focused / comprehensive / minimal
- generated at:

## Artifact Status

### Mandatory Artifacts
| Artifact | Status | Used By | Notes |
|----------|--------|---------|-------|
| architecture.md | pending / created | requirements-elaboration, requirements-analysis, architecture-design | |
| component-inventory.md | pending / created | requirements-analysis, architecture-design, units-generation | |
| technology-stack.md | pending / created | workflow-planning, implementation-design, code-generation, code-verification | |
| dependencies.md | pending / created | requirements-analysis, architecture-design, units-generation, build-testing | |
| code-structure.md | pending / created | implementation-design, code-generation, code-verification | |
| build-test-config.md | pending / created | code-generation, code-verification, build-testing, release | |

### Conditional Artifacts
| Artifact | Status | Condition | Used By | Rationale (if not_applicable/skipped) |
|----------|--------|-----------|---------|--------------------------------------|
| business-overview.md | pending / created / not_applicable / skipped | NEW_BUILD, REQ_ADD, or business context needed | requirements-elaboration, requirements-analysis | |
| api-documentation.md | pending / created / not_applicable / skipped | Interface/API changes or system exposes APIs | requirements-analysis, architecture-design, implementation-design | |
| data-model.md | pending / created / not_applicable / skipped | System uses database, schema, or persistent data store | requirements-analysis, architecture-design, implementation-design | |

### Optional Artifacts
| Artifact | Status | When Useful | Used By | Notes |
|----------|--------|-------------|---------|-------|
| code-quality-assessment.md | pending / created / skipped | TECH_CHANGE or tech debt context needed | architecture-design (TECH_CHANGE) | |

## Used-By Task Loading Guide

Downstream tasks MUST use this index to determine which RE artifacts to load:

### How to Load RE Context
1. Read `ai-sdlc-docs/inception/reverse-engineering/reverse-index.md`
2. For each artifact with status `created` that lists the current task in the "Used By" column:
   - Load the artifact
3. For each artifact with status `not_applicable` or `skipped`:
   - Do NOT attempt to load it
   - Note the rationale if relevant to the current task

### Per-Task Required Artifacts

| Task | Must Load (if created) | May Reference (if created) |
|------|----------------------|---------------------------|
| requirements-elaboration | architecture.md, business-overview.md | component-inventory.md, dependencies.md |
| requirements-analysis | architecture.md, component-inventory.md, dependencies.md | business-overview.md, api-documentation.md, data-model.md |
| workflow-planning | technology-stack.md | architecture.md, dependencies.md |
| architecture-design | architecture.md, component-inventory.md, dependencies.md | api-documentation.md, data-model.md |
| units-generation | component-inventory.md, dependencies.md | architecture.md |
| implementation-design | technology-stack.md, code-structure.md | build-test-config.md, api-documentation.md, data-model.md |
| code-generation, code-verification | code-structure.md, build-test-config.md | technology-stack.md, api-documentation.md |
| build-testing | build-test-config.md, dependencies.md | technology-stack.md |
| release | build-test-config.md | technology-stack.md |
