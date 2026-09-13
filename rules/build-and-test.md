# Build and Test Rules

## Rule: Build and Install Current dali-ui Before Automated Tests

- Status: required
- Scope: build, tests, workflow
- Applies To: C3 Code Verification and C4 Build & Testing on Ubuntu

### Intent

`dali-ui` automated tests link against the installed DALi libraries through
`pkg-config`. Test evidence is valid only when the current source has first
been built and installed into the active `DESKTOP_PREFIX`.

### Preconditions

- Run in an Ubuntu environment as defined by
  [dali-ui Project Context](dali-ui-context.md).
- Apply the DALi environment setup so `DESKTOP_PREFIX`, compiler paths, and
  runtime paths refer to the intended local installation.
- Ensure compatible `dali-core` and `dali-adaptor` builds are installed.
- Record the source revision or working-tree state being validated.

### C3: Unit-Level Verification

C3 verifies the affected unit. It does not run tests against an unbuilt source
tree.

1. Build and install the current `dali-ui` source with the Ubuntu debug and
   coverage configuration:

   ```sh
   cd build/tizen
   CXXFLAGS='-g -O0 --coverage' LDFLAGS='--coverage' cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX -DCMAKE_BUILD_TYPE=Debug -DENABLE_GRAPHICS_BACKEND="GLES"
   make install -j
   ```

2. Build the affected automated-test module from `automated-tests`:

   ```sh
   ./build.sh <module>
   ```

   Use `./build.sh` without a module when the affected unit cannot be isolated
   safely.

3. Execute the affected test case, prefix, or module:

   ```sh
   ./execute.sh <testcase-or-module>
   ./execute.sh -p <testcase-prefix>
   ```

4. Record the exact build, install, test-build, and test-execution commands and
   their outcomes in the code-verification evidence.

If the current libraries cannot be built and installed, or the required unit
tests cannot be executed, C3 remains `BLOCKED`.

### C4: Integrated Build and Testing

C4 validates the integrated source after all required units pass C3.

1. Execute the supported Ubuntu build and install for the complete integrated
   `dali-ui` source. The existing build directory and compiler cache may be
   reused, but the integrated build/install command must complete successfully.
2. Build the complete automated-test suite:

   ```sh
   cd automated-tests
   ./build.sh
   ```

3. Run the complete automated-test suite unless the approved test plan defines
   a justified regression scope:

   ```sh
   ./execute.sh
   ```

4. Build `manual-tests` and `samples` when they are affected by the change or
   required by the approved validation plan.
5. Record the Ubuntu version, compiler and build-tool versions when relevant,
   exact commands, results, failures, exclusions, and residual risks in the
   test report.

Any source or test change made after successful C3 evidence invalidates the
affected evidence. Rebuild, reinstall, and rerun the affected verification
before C4 can pass.

### Avoid

- Running automated tests before installing the current `dali-ui` build.
- Treating compilation performed implicitly by a test build as proof that the
  `dali-ui` libraries were installed from the current source.
- Claiming C3 or C4 success from static review or unexecuted commands.
- Running destructive cleanup commands such as `git clean` without explicit
  user authorization.
- Substituting Windows, macOS, or another distribution's result for required
  Ubuntu evidence without an approved exception.

### Exceptions

A smaller C3 test scope is allowed when the implementation design identifies a
reliable unit boundary and names the affected test cases. C4 scope reduction
requires an approved test plan with regression rationale and residual risk.

### Validation

The C3 and C4 evidence must show, in order:

- Current source identity.
- Ubuntu environment and active installation prefix.
- Successful `dali-ui` build and install.
- Successful automated-test build.
- Executed tests and their results.
- Any conditional manual-test or sample build results.
