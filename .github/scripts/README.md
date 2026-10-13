# Shared test utility check

`check-test-utils-sync.py` checks the shared test support sources in the resolved
Core, Adaptor, Toolkit and UI checkouts. The independent
`check-test-utils-sync.yml` workflow runs this platform-independent check once on
Linux, separately from the Ubuntu and Windows unit tests. It uses Python's
standard library and does not modify any checked file.

Pass repository roots explicitly when checking local repositories:

```sh
python3 .github/scripts/check-test-utils-sync.py \
  --core ../dali-core --adaptor ../dali-adaptor --toolkit ../dali-toolkit --ui .
```

Like the build server, the checker enumerates files directly inside Core's
test utility directory and compares same-name files in Adaptor, Toolkit and UI.
New Core files are checked automatically; there is no file manifest to update.
Subdirectories and target-only files, including UI's `dali-ui/` mocks, are
outside this check. The shared utility workflow checks out Toolkit for source
comparison only and does not build any repository.

Like the build server's `if [ -f ... ]` condition, missing counterparts are
skipped. There is no repository-specific exception list.

Files are compared byte for byte, matching the build server's `diff -q`.
Line endings, whitespace and final newlines are all significant. Run the local
checker against checkouts without automatic line-ending conversion. Unreadable
compared files fail. Missing checkout directories fail as a workflow setup error;
an empty Core directory has no
comparisons, just like the build server. A file deleted from Core is no longer
a comparison candidate, as Core supplies the reference file list.

If a PR changes a shared utility, update the corresponding dependency sources
and select their Gerrit changes in the existing `Test Dependencies` section (or
manual workflow inputs). The check runs after those patches have been applied.

The workflow checks UI sources after merging the PR into the latest target
branch, matching the unit test workflows. It executes the checker from a separate
trusted base revision checkout; PR and Gerrit checkouts are read only as data.
On manual runs, the selected workflow revision provides both the checker and UI
sources. Repository token permissions are limited to reading contents.
Before comparison, the workflow rejects symbolic links in test utility
directories and their ancestors to prevent reading files outside the checkouts.

The `Check shared test utilities` result is independent of the unit tests: a
mismatch does not stop their builds. Add it as a required status check in the
repository's branch rules if mismatches should prevent merging. A failing check
reports file differences in the Actions log.

Run the checker regression tests with:

```sh
python3 -m unittest discover -s .github/scripts/tests -v
```
