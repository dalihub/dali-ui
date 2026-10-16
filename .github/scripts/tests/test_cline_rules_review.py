import json
import os
import shutil
import subprocess
import tempfile
import textwrap
import unittest
from pathlib import Path


SCRIPTS = Path(__file__).resolve().parents[1]
WORKFLOW = SCRIPTS.parent / "workflows" / "rules-review.yml"
ISSUE = {
    "summary": "테스트 이슈 <sample>",
    "rule": "test.md / Test rule",
    "location": "sample.cpp:1",
    "severity": "required",
    "problem": "규칙 위반",
    "recommendation": "수정 필요",
}

MOCK_CLINE = r'''#!/usr/bin/env python3
import json
import os
import sys
from pathlib import Path

if sys.argv[1:] == ["--version"]:
    print("2.1.0")
    sys.exit(0)

state_directory = Path(os.environ["CLINE_DATA_DIR"])
assert state_directory.exists()
(state_directory / "mock-credentials").write_text(os.environ["CLINE_OFFLINE_TOKEN"])
args = sys.argv[1:]
assert args[args.index("--offline-token") + 1] == os.environ["CLINE_OFFLINE_TOKEN"]
assert args[args.index("--model") + 1] == os.environ["CLINE_MODEL"]
assert args[args.index("--retries") + 1] == "3"
assert "--json" in args
state = Path(os.environ["MOCK_STATE"])
attempt = int(state.read_text()) + 1 if state.exists() else 1
state.write_text(str(attempt))
scenarios = json.loads(os.environ["MOCK_SCENARIOS"])
scenario = scenarios[min(attempt - 1, len(scenarios) - 1)]
directory = Path(".rules-review-input")
if "expected_error" in scenario:
    assert scenario["expected_error"] in (directory / "validation-error.txt").read_text()
    assert "Correct the previous findings" in args[-1]
if scenario.get("require_previous"):
    assert (directory / "previous-result.json").exists()
if "result" in scenario:
    (directory / "result.json").write_text(scenario["result"])
if scenario.get("leak_token"):
    print(os.environ["CLINE_OFFLINE_TOKEN"])
    print(os.environ["CLINE_OFFLINE_TOKEN"], file=sys.stderr)
print(json.dumps({"type": "agent_event", "event": {"type": "done", "text": "done"}}))
print("mock stderr attempt " + str(attempt), file=sys.stderr)
sys.exit(scenario.get("status", 0))
'''


@unittest.skipUnless(shutil.which("node") and shutil.which("git"), "node and git required")
class RulesReviewTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory(prefix="rules-review-test-")
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.pr = self.root / "pr"
        self.pr.mkdir()
        self.git("init", "-q")
        self.git("config", "user.name", "Review Test")
        self.git("config", "user.email", "test@example.invalid")
        (self.pr / "rules").mkdir()
        (self.pr / "rules" / "test.md").write_text("# Test rule\n")
        self.git("add", ".")
        self.git("commit", "-qm", "base")
        self.base = self.git("rev-parse", "HEAD")
        (self.pr / "sample.cpp").write_text("int sample;\n")
        self.commit_head()
        self.bin = self.root / "bin"
        self.bin.mkdir()
        mock = self.bin / "cline"
        mock.write_text(MOCK_CLINE)
        mock.chmod(0o755)
        self.state = self.root / "mock-state"
        self.debug = self.root / "rules-review-debug"
        self.comment = self.root / "rules-review-comment.md"

    def git(self, *arguments):
        return subprocess.run(
            ["git", "-C", str(self.pr), *arguments],
            capture_output=True, text=True, check=True,
        ).stdout.strip()

    def commit_head(self):
        self.git("add", ".")
        self.git("commit", "-qm", "head")
        self.head = self.git("rev-parse", "HEAD")

    def run_review(self, scenarios, **overrides):
        environment = {
            **os.environ,
            "PATH": str(self.bin) + os.pathsep + os.environ["PATH"],
            "PR_WORKSPACE": str(self.pr),
            "GITHUB_WORKSPACE": str(self.root),
            "RUNNER_TEMP": str(self.root),
            "BASE_SHA": self.base,
            "HEAD_SHA": self.head,
            "CLINE_OFFLINE_TOKEN": "fake-offline-token-for-test",
            "CLINE_MODEL": "GaussO6",
            "CLINE_VERSION": "2.1.0",
            "REVIEW_MAX_ATTEMPTS": "3",
            "MAX_DIFF_BYTES": "180000",
            "COMMENT_MAX_CHARS": "60000",
            "MOCK_SCENARIOS": json.dumps(scenarios),
            "MOCK_STATE": str(self.state),
            **overrides,
        }
        return subprocess.run(
            ["bash", str(SCRIPTS / "cline-rules-review.sh")],
            env=environment, capture_output=True, text=True, timeout=30,
        )

    def assert_success(self, result, attempts):
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertEqual(int(self.state.read_text()), attempts)
        comment = self.comment.read_text()
        self.assertNotIn("리뷰 미완료", comment)
        self.assertIn(self.head, comment)
        self.assertIn(f"시도: {attempts}", comment)
        self.assertEqual((self.debug / "cline-version.txt").read_text().strip(), "2.1.0")
        self.assertFalse(list(self.root.glob("dali-ui-cline-state.*")))

    def test_valid_findings_render_and_escape_html(self):
        result = self.run_review([{"result": json.dumps([ISSUE])}])
        self.assert_success(result, 1)
        self.assertIn("테스트 이슈 &lt;sample&gt;", self.comment.read_text())
        self.assertIn("mock stderr", (self.debug / "attempt-1/cline-stderr.log").read_text())

    def test_empty_array_is_successful_review(self):
        result = self.run_review([{"result": "[]"}])
        self.assert_success(result, 1)
        self.assertIn("감지된 이슈 없음", self.comment.read_text())

    def test_invalid_json_is_repaired_using_previous_result(self):
        result = self.run_review([
            {"result": "```json\n[]\n```"},
            {"result": json.dumps([ISSUE]), "require_previous": True, "expected_error": "Unexpected"},
        ])
        self.assert_success(result, 2)
        self.assertIn("```json", (self.debug / "attempt-1/result.json").read_text())
        self.assertTrue((self.debug / "attempt-1/validation-errors.log").read_text())

    def test_schema_error_is_returned_for_correction(self):
        result = self.run_review([
            {"result": json.dumps([{**ISSUE, "severity": "warning"}])},
            {"result": json.dumps([ISSUE]), "require_previous": True, "expected_error": "severity"},
        ])
        self.assert_success(result, 2)

    def test_missing_file_is_repaired(self):
        result = self.run_review([
            {},
            {"result": "[]", "expected_error": "생성하지 않았습니다"},
        ])
        self.assert_success(result, 2)

    def test_exhaustion_fails_with_incomplete_comment_and_artifacts(self):
        result = self.run_review([{"result": "not JSON"}])
        self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
        self.assertEqual(self.state.read_text(), "3")
        comment = self.comment.read_text()
        self.assertIn("리뷰 미완료", comment)
        self.assertIn("AI 리뷰 결과는 확인할 수 없습니다", comment)
        self.assertIn("시도: 3", comment)
        self.assertTrue((self.debug / "attempt-3/validation-errors.log").exists())
        self.assertTrue((self.debug / "comment.md").exists())

    def test_nonzero_exit_cannot_pass_with_valid_or_stale_result(self):
        result = self.run_review([
            {"result": "[]", "status": 9},
            {},
            {},
        ])
        self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
        self.assertEqual(self.state.read_text(), "3")
        self.assertIn("status 9", (self.debug / "attempt-1/validation-errors.log").read_text())
        self.assertIn("생성하지 않았습니다", (self.debug / "attempt-2/validation-errors.log").read_text())

    def test_nonzero_exit_can_recover_on_later_complete_attempt(self):
        result = self.run_review([
            {"status": 9},
            {"result": "[]", "expected_error": "status 9"},
        ])
        self.assert_success(result, 2)

    def test_static_findings_survive_ai_failure(self):
        directory = self.pr / "dali-ui-components"
        directory.mkdir()
        (directory / "sample.cpp").write_text(
            "#include <dali-ui-foundation/internal/view.h>\n"
        )
        self.commit_head()
        result = self.run_review([{}])
        self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
        comment = self.comment.read_text()
        self.assertIn("리뷰 미완료", comment)
        self.assertIn("dali-ui-components/sample.cpp:1", comment)
        self.assertIn("감지된 이슈 1건", comment)

    def test_debug_artifacts_redact_offline_token(self):
        result = self.run_review([{"result": "[]", "leak_token": True}])
        self.assert_success(result, 1)
        for path in self.debug.rglob("*"):
            if path.is_file():
                self.assertNotIn("fake-offline-token-for-test", path.read_text(), str(path))
        self.assertIn("[REDACTED]", (self.debug / "attempt-1/cline-stderr.log").read_text())

    def test_missing_token_clears_stale_comment_and_fails_before_cli(self):
        self.comment.write_text("old successful review")
        result = self.run_review([], CLINE_OFFLINE_TOKEN="")
        self.assertEqual(result.returncode, 1)
        self.assertIn("CLINE_OFFLINE_TOKEN secret is required", result.stderr)
        self.assertFalse(self.state.exists())
        self.assertFalse(self.comment.exists())

    def test_failed_preparation_still_collects_artifacts(self):
        result = self.run_review([], BASE_SHA="0" * 40)
        self.assertEqual(result.returncode, 1)
        self.assertTrue(self.debug.is_dir())
        self.assertFalse(self.state.exists())

    def test_validator_rejects_invalid_shapes_fields_and_oversize(self):
        path = self.root / "result.json"
        cases = [
            ("[]", True),
            (json.dumps([ISSUE] * 50), True),
            (json.dumps([ISSUE] * 51), False),
            (json.dumps({"issues": [ISSUE]}), False),
            ("[null]", False),
            (json.dumps([{**ISSUE, "problem": " "}]), False),
            (json.dumps([{**ISSUE, "location": 123}]), False),
            (json.dumps([ISSUE, {**ISSUE, "severity": "warning"}]), False),
            (json.dumps([{**ISSUE, "summary": "x" * (1024 * 1024)}]), False),
        ]
        for content, expected in cases:
            with self.subTest(expected=expected, size=len(content)):
                path.write_text(content)
                result = subprocess.run(
                    ["node", str(SCRIPTS / "validate-rules-review.cjs"), str(path)],
                    capture_output=True, text=True,
                )
                self.assertEqual(result.returncode == 0, expected, result.stderr)
        path.unlink()
        path.symlink_to(self.pr / "sample.cpp")
        result = subprocess.run(
            ["node", str(SCRIPTS / "validate-rules-review.cjs"), str(path)],
            capture_output=True, text=True,
        )
        self.assertNotEqual(result.returncode, 0)


@unittest.skipUnless(shutil.which("node"), "node required")
class PullRequestResolutionTests(unittest.TestCase):
    def run_resolver(self, value, deleted_head=False):
        source = WORKFLOW.read_text().split("script: |", 1)[1].split("\n      - name:", 1)[0]
        source = textwrap.dedent(source)
        harness = r'''
const outputs = {};
const calls = [];
const github = { rest: { pulls: { get: async args => {
  calls.push(args);
  return { data: {
    number: args.pull_number,
    base: { sha: 'base-sha', repo: { full_name: 'owner/base' } },
    head: { sha: 'head-sha', repo: process.env.DELETED_HEAD === 'true' ? null : { full_name: 'fork/head' } }
  } };
} } } };
const AsyncFunction = Object.getPrototypeOf(async function() {}).constructor;
new AsyncFunction('github', 'context', 'core', process.env.RESOLVER)(
  github, { repo: { owner: 'owner', repo: 'base' } },
  { setOutput: (name, value) => { outputs[name] = value; } }
).then(() => console.log(JSON.stringify({ outputs, calls })))
 .catch(error => { console.log(JSON.stringify({ error: error.message, calls })); process.exitCode = 1; });
'''
        result = subprocess.run(
            ["node", "-e", harness], capture_output=True, text=True,
            env={**os.environ, "PR_NUMBER": value, "RESOLVER": source,
                 "DELETED_HEAD": str(deleted_head).lower()},
        )
        return result, json.loads(result.stdout)

    def test_valid_pr_resolves_base_and_fork_head(self):
        result, data = self.run_resolver("123")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(data["outputs"]["number"], "123")
        self.assertEqual(data["outputs"]["head-repository"], "fork/head")
        self.assertEqual(data["outputs"]["base-sha"], "base-sha")
        self.assertEqual(data["calls"][0]["pull_number"], 123)

    def test_invalid_pr_number_never_calls_api(self):
        for value in ["", "0", "-1", "1.5", "1e2", "12\n13", "$(touch file)", "9007199254740992"]:
            with self.subTest(value=value):
                result, data = self.run_resolver(value)
                self.assertNotEqual(result.returncode, 0)
                self.assertEqual(data["calls"], [])

    def test_deleted_fork_fails_before_checkout(self):
        result, data = self.run_resolver("123", deleted_head=True)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("head repository is unavailable", data["error"])


if __name__ == "__main__":
    unittest.main()
