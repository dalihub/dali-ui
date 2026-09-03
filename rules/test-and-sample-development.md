# Test and Sample Development Rules

These rules cover how repository tests and samples are authored. Build and
execution evidence remains governed by [Build and Test](build-and-test.md).

## Rule: Follow the Automated-Test Harness Contract

- Status: required
- Scope: tests
- Applies To: New or substantially modified files under `automated-tests/src/`

### Intent

The DALi test harness discovers test cases by parsing source and CMake input.
Tests must follow that contract so they are registered, executable, and useful
when a regression fails.

### Preferred

- Use the exact `int UtcDaliFeatureBehavior(void)` signature for test cases.
- Register each new test source in the target module's `TC_SOURCES` list.
- Keep test-only helpers and state in an anonymous namespace.
- Construct the module's established test application and configuration before
  using DALi UI objects.
- Compare observable actual values with explicit expected values. Use
  `DALI_TEST_ASSERTION` for APIs whose contract requires an assertion.
- Exercise success, boundary, invalid-input, signal, and lifecycle behavior
  that is relevant to the changed contract.
- Record the narrow test command in the implementation or verification plan so
  the case can be reproduced independently.

### Avoid

- Empty-parameter syntax such as `int UtcDaliFeatureBehavior()`; the generator
  expects `(void)`.
- Adding a source without registering it in the owning module.
- Assertions that only repeat implementation details without checking public
  behavior.
- Timing-dependent waits when event processing, notifications, or deterministic
  animation progression can expose the same result.
- Claiming a test passed when its executable was not run.

### Exceptions

Test harness utilities, generated headers, and sources that contain no test
entry points do not use the `Utc...` function contract. Internal implementation
tests may compile the affected internal source directly when the shared library
intentionally hides the tested symbols; document that boundary in CMake.

### Validation

```sh
rg -n '^int Utc[A-Za-z0-9_]+\(void\)$' automated-tests/src
rg -n 'TC_SOURCES' automated-tests/src/*/CMakeLists.txt
```

Build and run the affected module and test prefix using the commands in
[`automated-tests/README.md`](../automated-tests/README.md) after installing the
current libraries as required by [Build and Test](build-and-test.md).

## Rule: Pair Manual-Test Sources With Scenario Documents

- Status: required
- Scope: tests, docs
- Applies To: New or substantially modified `manual-tests/*/tc/tc-*.cpp` files

### Intent

A manual-test UI is only reproducible when the reviewer knows what is on the
screen, how to operate it, what to observe at each step, and how to decide the
result. The scenario document is part of the test, not optional commentary.

### Preferred

- Place `tc-<feature>.cpp` and `tc-<feature>.md` in the same module `tc/`
  directory with exactly the same basename.
- Follow the launcher contract in
  [`manual-tests/README.md`](../manual-tests/README.md): derive from
  `ManualTest::TestCase`, implement `GetName()`, `GetDescription()`, and
  `OnEnter()`, and register the class with `REGISTER_MANUAL_TEST`.
- Clean up timers, animations, signal-owned state, and retained handles in
  `OnExit()` when the test creates lifecycle-sensitive resources.
- Write the scenario document with these sections, adding preconditions or
  runtime evidence only when needed:
  - Purpose.
  - `화면 구성`.
  - Numbered test scenarios or one explicit `테스트 절차` table.
  - An observable `기대 결과` for every operation or group of operations.
  - `통과 기준` that can be evaluated without reading the implementation.
- State exact values, directions, labels, visibility, or signal-driven effects
  when those details determine pass or fail.
- Keep source controls, initial state, expected results, and scenario wording in
  sync whenever either file changes.
- Use only public APIs when the manual test is intended as application-facing
  usage guidance. State the narrower purpose when an extension or integration
  API is deliberately exercised.
- Describe unexecuted behavior as an expected result. Put dated environment,
  screenshots, logs, or measured values in a separate runtime-evidence section
  only after they are actually captured.

### Avoid

- A manual-test source without its same-basename Markdown scenario.
- Instructions such as "verify it works" without an observable condition.
- Describing controls or expected states that the source does not provide.
- Presenting a planned manual check as completed runtime evidence.
- Editing a module build list when its existing `tc/*.cpp` glob already
  discovers the new source.

### Exceptions

Launcher sources, `manual-test-case.h`, and other manual-test infrastructure do
not need scenario documents. Legacy unpaired test cases may remain until they
are substantively modified; a substantive update must add the missing document
in the same change.

### Validation

Review every reported path. It must either gain its matching document or be a
documented legacy/infrastructure exception.

```sh
find manual-tests -type f -path '*/tc/tc-*.cpp' -print | sort | while IFS= read -r source; do
  test -f "${source%.cpp}.md" || echo "missing scenario: ${source%.cpp}.md"
done
```

Then compare every changed source/document pair for matching controls, initial
state, expected results, and pass criteria.

## Rule: Document User-Facing Samples Beside Their Source

- Status: required
- Scope: samples, docs
- Applies To: New user-facing sample directories and substantially modified samples without a README

### Intent

A sample is application-facing guidance. Its source must compile, while its
README must let a developer understand, build, run, and evaluate the example
without reverse-engineering the implementation.

### Preferred

- Add `README.md` beside the sample's source and `CMakeLists.txt`.
- Explain the sample's purpose and the public APIs or behaviors it demonstrates.
- Document current prerequisites, build commands, executable path, and run
  command using paths relative to a clearly named working directory.
- List keyboard, pointer, touch, or on-screen controls and their observable
  results.
- Call out required resources, platform capabilities, environment variables, or
  unsupported environments.
- Keep the sample focused on public application APIs. If its purpose requires an
  extension or integration API, say so explicitly in the README.
- Keep source registration consistent with the existing sample CMake helpers and
  top-level discovery pattern.
- Separate expected verification behavior from captured runtime evidence. Date
  and identify the environment for screenshots or logs that are checked in.

### Avoid

- A README that only repeats the sample name.
- Build or run commands that omit their starting directory or required
  environment.
- Controls documented in the README but absent from the source.
- Checked-in screenshots, logs, or PASS claims presented as current evidence
  when the described revision was not run.
- Using internal implementation APIs merely to make an application-facing
  sample easier to write.

### Exceptions

Legacy sample directories are not required to gain a README unless they are
substantively modified. Internal diagnostics, fixtures, or performance probes
may use a shorter document, but the change should record why they are not
application-facing guidance.

### Validation

For each added or substantially modified sample, review `CMakeLists.txt`, source,
resources, and `README.md` together. Build the affected sample as described in
the README and record execution evidence only when it was actually run.

```sh
find samples -mindepth 2 -maxdepth 2 -name README.md -print | sort
```
