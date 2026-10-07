#!/usr/bin/env python3

import argparse
import difflib
import sys
from pathlib import Path


TEST_UTILS_PATHS = {
    "core": Path("automated-tests/src/dali/dali-test-suite-utils"),
    "adaptor": Path("automated-tests/src/dali-adaptor/dali-test-suite-utils"),
    "toolkit": Path("automated-tests/src/dali-toolkit/dali-toolkit-test-utils"),
    "ui": Path("automated-tests/src/dali-test-suite-utils"),
}


def parse_arguments():
    parser = argparse.ArgumentParser(
        description="Check shared DALi test utilities after applying dependency patches"
    )
    for name in TEST_UTILS_PATHS:
        parser.add_argument(
            f"--{name}", required=True, type=Path, help=f"dali-{name} repository root"
        )
    return parser.parse_args()


def read_source(path):
    # Match the build server's diff: compare the complete file bytes.
    return path.read_bytes()


def check_sync(roots):
    failures = 0
    comparisons = 0
    directories = {name: roots[name] / path for name, path in TEST_UTILS_PATHS.items()}
    for directory in directories.values():
        if not directory.is_dir():
            print(f"ERROR: Missing test utility directory: {directory}")
            failures += 1
    if failures:
        return 1
    try:
        sources = sorted(
            path for path in directories["core"].iterdir()
            if path.is_file() and not path.name.startswith(".")
        )
    except OSError as error:
        print(f"ERROR: Cannot list Core test utility directory: {error}")
        return 1
    for source in sources:
        targets = {
            repository: directory / source.name
            for repository, directory in directories.items()
            if repository != "core" and (directory / source.name).is_file()
        }
        if not targets:
            continue
        try:
            reference = read_source(source)
        except OSError as error:
            print(f"ERROR: Cannot read required file {source}: {error}")
            failures += 1
            continue
        for repository, target in targets.items():
            try:
                contents = read_source(target)
            except OSError as error:
                print(f"ERROR: Cannot read required file {target}: {error}")
                failures += 1
                continue
            comparisons += 1
            if reference == contents:
                continue
            failures += 1
            print(f"ERROR: Shared test utility differs: core vs {repository}: {source.name}")
            diff = difflib.unified_diff(
                reference.decode("utf-8", errors="replace").splitlines(keepends=True),
                contents.decode("utf-8", errors="replace").splitlines(keepends=True),
                fromfile=source.as_posix(),
                tofile=target.as_posix(),
            )
            for line in diff:
                sys.stdout.write(line)
                if not line.endswith("\n"):
                    sys.stdout.write("\n\\ No newline at end of file\n")

    if failures:
        print(f"Shared test utilities are out of sync: {failures} error(s).")
        return 1
    print(f"Shared test utilities are in sync: {comparisons} comparisons passed.")
    return 0


def main():
    arguments = parse_arguments()
    roots = {name: getattr(arguments, name).resolve() for name in TEST_UTILS_PATHS}
    return check_sync(roots)


if __name__ == "__main__":
    sys.exit(main())
