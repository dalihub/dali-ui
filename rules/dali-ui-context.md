# dali-ui Project Context

This document provides the shared project context that DALi-tailored AI-SDLC
workflows must load before planning or implementation. Detailed constraints
remain in the individual rule documents linked below.

## Project Structure

`dali-ui` is a C++ UI framework built on `dali-core` and `dali-adaptor`. It is
split into two libraries:

- `dali-ui-foundation`: base UI classes, layouts, events, and traits.
- `dali-ui-components`: higher-level components built on foundation APIs.

Component code must respect the dependency direction and API-level boundaries
defined in [Component Boundaries](component-boundaries.md) and
[Public API and ABI](public-api-abi.md).

## Rule: Ubuntu Is the Development Baseline

- Status: required
- Scope: build, tests, workflow
- Applies To: DALi-tailored AI-SDLC planning, implementation, and validation

### Intent

`dali-ui` development and its documented build and validation commands are
based on Ubuntu. Workflow plans and evidence must use an Ubuntu environment as
the baseline so that toolchain, package, path, and shell assumptions are
consistent.

### Preferred

- Record the Ubuntu version and relevant compiler/build-tool versions when they
  affect reproducibility.
- Plan and execute build, install, automated-test, manual-test, and sample
  commands using the repository's Ubuntu workflow.
- Build and install the current `dali-ui` libraries before running automated
  tests that link to the installed libraries through `pkg-config`.
- Treat validation performed in another environment as supplementary unless an
  explicit project decision accepts it as equivalent.

### Avoid

- Silently replacing the Ubuntu workflow with macOS, Windows, or a different
  distribution's commands.
- Claiming Ubuntu build or test evidence from static inspection or from results
  produced on an unidentified platform.
- Running automated tests against an older installed `dali-ui` build.

### Exceptions

A container, virtual machine, or compatible CI runner is acceptable when it
provides the required Ubuntu environment. A non-Ubuntu validation environment
requires an explicit decision that records the reason, known differences, and
residual risk.

### Validation

Build and test reports must record the execution environment and exact commands.
When automated tests are run, verify that the current source was built and
installed first.

## Rule Routing

Load the rules relevant to the selected work:

- New component development: [Component Development](component-development.md)
- Component feature reference: [OneUIComponents Reference](oneui-components-reference.md)
- Ubuntu build and validation: [Build and Test](build-and-test.md)
- API and ABI design: [Public API and ABI](public-api-abi.md)
- Public handle and implementation structure: [Handle-Body Pattern](handle-body-pattern.md)
- Foundation dependency surfaces: [Component Boundaries](component-boundaries.md)
- API naming: [API Naming](api-naming.md)
- User-facing documentation: [Docs and Wiki](docs-and-wiki.md)
- Validation searches: [Validation Checks](validation-checks.md)
