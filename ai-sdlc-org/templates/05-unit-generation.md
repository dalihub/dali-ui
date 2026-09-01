---
artifact_type: UNIT_SPEC
---

# Unit Specification

## Unit Summary
<!-- State lifecycle: planned -> design_ready -> code_generated -> code_verified -> integrated -> released
     Update rule: each unit owner MUST update the State column in this table
     whenever their unit-status artifact (state/units/{unit-id}-team-status.md) changes state.
     ROLE-INT reads this table as the entry check for Integration Ready Check evaluation. -->

| Unit ID | Unit Name | Type | Owner | Reviewer | Risk | Depends On | Provides To | State |
|---------|-----------|------|-------|----------|------|------------|-------------|-------|

## Unit Details

### Unit: <UNIT-NAME>
- unit id:
- unit type: feature / api / ui / data / infra / test / integration / documentation
- owner:
- reviewer:
- validation owner:
- objective:
- business requirement id:
- related acceptance criteria:
- scope in:
- scope out:
- target files / components / repositories:
- owned interfaces:
  - name: string
  - version: semver (e.g., 1.0.0)
  - breaking_change: boolean
- consumed interfaces:
  - name: string
  - version: semver (e.g., 1.0.0)
  - provider_unit: string
- dependency units:
- downstream impacted units:
- expected outputs:
- implementation constraints:
- security/privacy consideration:
- performance/reliability consideration:
- compatibility consideration:
- verification focus:
- required test evidence:
- integration preconditions:
- integration notes:
- rollback or disable strategy:
- open assumptions:
- unit status artifact: ai-sdlc-docs/state/units/{unit-id}-team-status.md

## Dependency Map
| From Unit | To Unit | Dependency Type | Contract / Evidence Needed | Blocking? |
|-----------|---------|-----------------|-----------------------------|-----------|

## Path Ownership Map
| Path | Owning Unit | Owner (GitHub) | Notes |
|------|-------------|----------------|-------|

<!-- Every path that two or more developers will touch must appear here.
     A path appearing under two units = boundary conflict → re-split required before Inception Review. -->

## Delivery Order
1.
2.

## Team Execution Notes
- parallelizable?: yes / no
- separate session required?: yes / no
- cross-review required?: yes / no
- integration owner:
- backup integration owner:
- integration sequence:

