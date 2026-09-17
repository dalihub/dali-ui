#!/usr/bin/env python3

import argparse
import json
import os
import re
import subprocess
from collections import defaultdict
from pathlib import Path


HUNK_PATTERN = re.compile(r"^@@ -\d+(?:,\d+)? \+(\d+)(?:,(\d+))? @@")


def parse_arguments():
    parser = argparse.ArgumentParser(description="Check LCOV line coverage for changed code")
    parser.add_argument("--coverage", required=True, type=Path)
    parser.add_argument("--base", required=True)
    parser.add_argument("--head", required=True)
    parser.add_argument("--threshold", required=True, type=float)
    parser.add_argument("--output-directory", required=True, type=Path)
    return parser.parse_args()


def collect_changed_lines(base, head):
    result = subprocess.run(
        [
            "git",
            "-c",
            "core.quotePath=false",
            "diff",
            "--unified=0",
            "--no-color",
            "--diff-filter=ACMR",
            base,
            head,
            "--",
        ],
        check=True,
        capture_output=True,
        text=True,
    )

    changed_lines = defaultdict(set)
    current_path = None

    for line in result.stdout.splitlines():
        if line.startswith("+++ "):
            current_path = line[4:]
            if current_path == "/dev/null":
                current_path = None
            elif current_path.startswith("b/"):
                current_path = current_path[2:]
            continue

        match = HUNK_PATTERN.match(line)
        if current_path is None or match is None:
            continue

        start = int(match.group(1))
        count = int(match.group(2) or 1)
        changed_lines[current_path].update(range(start, start + count))

    return changed_lines


def repository_path(source_path, repository_root):
    path = Path(source_path)
    if path.is_absolute():
        try:
            path = path.resolve().relative_to(repository_root)
        except ValueError:
            return None
    return path.as_posix().removeprefix("./")


def collect_line_coverage(coverage_path, repository_root):
    coverage = defaultdict(int)
    current_path = None

    with coverage_path.open(encoding="utf-8") as stream:
        for raw_line in stream:
            line = raw_line.rstrip("\n")
            if line.startswith("SF:"):
                current_path = repository_path(line[3:], repository_root)
            elif line.startswith("DA:") and current_path is not None:
                fields = line[3:].split(",")
                line_number = int(fields[0])
                execution_count = int(fields[1])
                coverage[(current_path, line_number)] += execution_count
            elif line == "end_of_record":
                current_path = None

    return coverage


def write_results(output_directory, threshold, covered_lines, coverable_lines):
    percentage = 100.0 if coverable_lines == 0 else covered_lines * 100.0 / coverable_lines
    passed = coverable_lines == 0 or percentage >= threshold
    coverage_text = (
        "N/A (no coverable changed lines)"
        if coverable_lines == 0
        else f"{percentage:.1f}% ({covered_lines}/{coverable_lines})"
    )
    status_text = "PASS" if passed else "FAIL"

    output_directory.mkdir(parents=True, exist_ok=True)
    (output_directory / "status").write_text(f"{status_text}\n", encoding="utf-8")
    (output_directory / "result.json").write_text(
        json.dumps(
            {
                "passed": passed,
                "percentage": percentage,
                "covered_lines": covered_lines,
                "coverable_lines": coverable_lines,
                "threshold": threshold,
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )

    server_url = os.environ.get("GITHUB_SERVER_URL")
    repository = os.environ.get("GITHUB_REPOSITORY")
    run_id = os.environ.get("GITHUB_RUN_ID")
    run_link = ""
    if server_url and repository and run_id:
        run_link = f"\n[Open workflow run]({server_url}/{repository}/actions/runs/{run_id})\n"

    summary = (
        "## Changed-code coverage\n\n"
        "| Metric | Result |\n"
        "| --- | ---: |\n"
        f"| Status | {status_text} |\n"
        f"| Coverage | {coverage_text} |\n"
        f"| Required | {threshold:.1f}% |\n"
        f"{run_link}"
    )
    (output_directory / "summary.md").write_text(summary, encoding="utf-8")


def main():
    arguments = parse_arguments()
    repository_root = Path.cwd().resolve()
    changed_lines = collect_changed_lines(arguments.base, arguments.head)
    coverage = collect_line_coverage(arguments.coverage, repository_root)

    coverable_counts = [
        count
        for (path, line_number), count in coverage.items()
        if line_number in changed_lines.get(path, set())
    ]
    covered_lines = sum(count > 0 for count in coverable_counts)

    write_results(
        arguments.output_directory,
        arguments.threshold,
        covered_lines,
        len(coverable_counts),
    )


if __name__ == "__main__":
    main()
