import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path


SCRIPT = Path(__file__).resolve().parents[1] / "check-test-utils-sync.py"
TEST_UTILS_PATHS = {
    "core": "automated-tests/src/dali/dali-test-suite-utils",
    "adaptor": "automated-tests/src/dali-adaptor/dali-test-suite-utils",
    "toolkit": "automated-tests/src/dali-toolkit/dali-toolkit-test-utils",
    "ui": "automated-tests/src/dali-test-suite-utils",
}


class CheckTestUtilsSyncTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory(prefix="dali-test-utils-")
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.repositories = {name: self.root / name for name in TEST_UTILS_PATHS}
        self.directories = {
            name: self.repositories[name] / path for name, path in TEST_UTILS_PATHS.items()
        }
        for directory in self.directories.values():
            directory.mkdir(parents=True)
        for directory in self.directories.values():
            (directory / "test-harness.cpp").write_bytes(b"shared implementation\n")
        for repository in ("core", "ui"):
            (self.directories[repository] / "test-custom-actor.cpp").write_bytes(
                b"shared implementation\n"
            )

    def run_check(self):
        arguments = [sys.executable, str(SCRIPT)]
        for name, root in self.repositories.items():
            arguments.extend((f"--{name}", str(root)))
        result = subprocess.run(arguments, capture_output=True, text=True, check=False)
        # Use the build server's native diff decision as an independent oracle.
        if shutil.which("diff") and all(path.is_dir() for path in self.directories.values()):
            expected = 0
            for source in self.directories["core"].iterdir():
                if not source.is_file() or source.name.startswith("."):
                    continue
                for repository in ("adaptor", "toolkit", "ui"):
                    target = self.directories[repository] / source.name
                    if target.is_file():
                        comparison = subprocess.run(
                            ["diff", "-q", str(source), str(target)],
                            capture_output=True, check=False,
                        )
                        if comparison.returncode:
                            expected = 1
            self.assertEqual(result.returncode, expected, result.stdout + result.stderr)
        return result

    def test_hidden_core_files_are_ignored_like_ls(self):
        for directory in self.directories.values():
            (directory / ".hidden").write_bytes(directory.name.encode())
        result = self.run_check()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_matching_files_pass(self):
        result = self.run_check()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn("in sync", result.stdout)

    def test_line_ending_differences_fail_like_diff(self):
        for path in self.directories["ui"].iterdir():
            path.write_bytes(b"shared implementation\r\n")
        result = self.run_check()
        self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
        self.assertIn("test-harness.cpp", result.stdout)

    def test_mismatch_in_each_repository_fails_with_diff(self):
        for repository in TEST_UTILS_PATHS:
            with self.subTest(repository=repository):
                path = self.directories[repository] / "test-harness.cpp"
                path.write_bytes(b"changed implementation\n")
                result = self.run_check()
                self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
                self.assertIn("test-harness.cpp", result.stdout)
                self.assertIn("@@", result.stdout)
                path.write_bytes(b"shared implementation\n")

    def test_missing_counterparts_are_skipped_like_build_server(self):
        for repository in ("adaptor", "toolkit", "ui"):
            with self.subTest(repository=repository):
                path = self.directories[repository] / "test-harness.cpp"
                path.unlink()
                result = self.run_check()
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                path.write_bytes(b"shared implementation\n")

    def test_new_core_files_are_checked_without_a_manifest(self):
        for directory in self.directories.values():
            (directory / "new-shared-file.txt").write_bytes(b"new shared content\n")
        result = self.run_check()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        (self.directories["toolkit"] / "new-shared-file.txt").write_bytes(b"diverged\n")
        result = self.run_check()
        self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
        self.assertIn("new-shared-file.txt", result.stdout)

    def test_core_only_file_is_skipped(self):
        (self.directories["core"] / "new-shared-file.h").write_bytes(b"new content\n")
        result = self.run_check()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_missing_repository_directory_fails(self):
        for repository in TEST_UTILS_PATHS:
            with self.subTest(repository=repository):
                directory = self.directories[repository]
                renamed = directory.with_name(directory.name + "-missing")
                directory.rename(renamed)
                result = self.run_check()
                self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
                self.assertIn("Missing test utility directory", result.stdout)
                renamed.rename(directory)

    def test_empty_core_directory_matches_build_server(self):
        for path in self.directories["core"].iterdir():
            path.unlink()
        result = self.run_check()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn("0 comparisons passed", result.stdout)

    def test_optional_file_is_compared_when_present(self):
        for repository in ("adaptor", "toolkit"):
            with self.subTest(repository=repository):
                path = self.directories[repository] / "test-custom-actor.cpp"
                path.write_bytes(b"diverged\n")
                result = self.run_check()
                self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
                self.assertIn("test-custom-actor.cpp", result.stdout)
                path.unlink()

    def test_core_subdirectories_are_not_traversed(self):
        directory = self.directories["core"] / "core-specific"
        directory.mkdir()
        (directory / "nested.h").write_bytes(b"Core extension\n")
        result = self.run_check()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_core_ui_file_missing_in_ui_is_skipped(self):
        (self.directories["ui"] / "test-custom-actor.cpp").unlink()
        result = self.run_check()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_repository_specific_files_are_ignored(self):
        (self.directories["ui"] / "ui-specific.cpp").write_bytes(b"UI extension\n")
        result = self.run_check()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_final_newline_and_whitespace_differences_fail(self):
        path = self.directories["ui"] / "test-harness.cpp"
        for contents in (b"shared implementation", b"shared implementation \n"):
            with self.subTest(contents=contents):
                path.write_bytes(contents)
                result = self.run_check()
                self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
                self.assertIn("test-harness.cpp", result.stdout)

    def test_matching_non_utf8_bytes_pass(self):
        for directory in self.directories.values():
            (directory / "test-harness.cpp").write_bytes(b"\xff")
        result = self.run_check()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)


if __name__ == "__main__":
    unittest.main()
