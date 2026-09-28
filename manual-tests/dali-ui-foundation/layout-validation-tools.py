#!/usr/bin/env python3
"""Offline artifact validation. Never launches or controls a TC."""
import argparse
import hashlib
import json
import math
from pathlib import Path
import re
import subprocess
import sys
import tempfile

HERE = Path(__file__).resolve().parent
JSON_RECORDS = {"LR_CASE_CONTRACT", "LR_CASE_END", "LR_ACTION", "LR_CHECK", "LR_RESULT", "LR_NOOP", "LR_CLEANUP_ERROR", "LR_FRAME"}


# The producer's verdict vocabulary. FAIL, PASS and EXTERNAL_REQUIRED are TERMINAL: they are
# only emitted once completed_scenarios == required_scenarios. FAILING, INCOMPLETE and PENDING
# are non-terminal and say nothing final about the case.
TERMINAL_VERDICTS = {"FAIL", "PASS", "EXTERNAL_REQUIRED"}
FAILED_VERDICTS = {"FAIL", "FAILING"}
VERDICTS = TERMINAL_VERDICTS | FAILED_VERDICTS | {"INCOMPLETE", "PENDING"}

# How the frame fence can end abnormally. A normal completion emits no record at all, so an
# LR_FRAME line always accompanies a render.frame-timeout failing row.
FRAME_RESULTS = {"timeout", "disconnected", "predicate-exception"}


def require(condition, message):
    if not condition:
        raise ValueError(message)


def unique_object(pairs):
    result = {}
    for key, value in pairs:
        require(key not in result, f"duplicate JSON field: {key}")
        result[key] = value
    return result


def finite_json_float(text):
    value = float(text)
    require(math.isfinite(value), f"nonfinite JSON exponent: {text}")
    return value


def strict_json(text):
    return json.loads(text, object_pairs_hook=unique_object, parse_float=finite_json_float,
                      parse_constant=lambda value: (_ for _ in ()).throw(ValueError(f"nonfinite JSON: {value}")))


def read_json(path):
    return strict_json(Path(path).read_text(encoding="utf-8"))


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def integer(value, minimum=0):
    return type(value) is int and value >= minimum


def finite_number(value):
    try:
        return type(value) in (int, float) and math.isfinite(value)
    except (OverflowError, ValueError):
        return False


def nonempty_string(value):
    return isinstance(value, str) and bool(value) and value == value.strip()


def hash_string(value):
    return isinstance(value, str) and re.fullmatch(r"[0-9a-f]{64}", value) is not None


def records(path):
    for number, raw in enumerate(Path(path).read_text(encoding="utf-8", errors="strict").splitlines(), 1):
        line = re.sub(r"\x1b\[[0-?]*[ -/]*[@-~]", "", raw).lstrip()
        kind = line.split(None, 1)[0] if line else ""
        if kind in JSON_RECORDS:
            parts = line.split(None, 1)
            require(len(parts) == 2, f"{path}:{number}: truncated {kind}")
            try:
                value = strict_json(parts[1])
            except (ValueError, TypeError) as error:
                raise ValueError(f"{path}:{number}: malformed {kind}: {error}") from error
            require(isinstance(value, dict), f"{path}:{number}: {kind} must contain an object")
            yield kind, value
        elif kind.startswith("LR_") and any(known.startswith(kind) for known in JSON_RECORDS):
            raise ValueError(f"{path}:{number}: truncated LR record marker")
        elif line.startswith("LR_") and "{" in line:
            raise ValueError(f"{path}:{number}: unknown structured LR record")
        elif re.search(r"\bLR_(CASE_CONTRACT|CASE_END|ACTION|CHECK|RESULT|NOOP|CLEANUP_ERROR|FRAME)\b", line):
            # A record marker that is not at the start of its line is not a record: whatever
            # wrote it interleaved with the producer, so the evidence cannot be trusted.
            raise ValueError(f"{path}:{number}: record marker not at line start")


def numeric_check(row):
    """Preserve the producer's comparison type; do not round int64 through float."""
    actual, expected, kind = row["actual"], row["expected"], row["kind"]
    require(isinstance(actual, str) and isinstance(expected, str), "check values must be strings")
    tolerance = row["tolerance"]
    require(finite_number(tolerance) and tolerance >= 0, "invalid tolerance")
    if kind == "text":
        require(tolerance == 0, "text checks cannot have a numeric tolerance")
        return actual == expected
    if kind == "integer":
        require(tolerance == 0, "integer checks must be exact")
        require(re.fullmatch(r"-?[0-9]+", actual) is not None and
                re.fullmatch(r"-?[0-9]+", expected) is not None, "malformed integer check")
        a, e = int(actual), int(expected)
        require(-(2**63) <= a < 2**63 and -(2**63) <= e < 2**63, "integer outside int64 range")
        return a == e
    if kind == "failure":
        return False
    if kind == "color":
        # Captured pixel colour: "r,g,b" 8-bit channels, each within the per-channel tolerance.
        require(re.fullmatch(r"[0-9]{1,3},[0-9]{1,3},[0-9]{1,3}", actual) is not None and
                re.fullmatch(r"[0-9]{1,3},[0-9]{1,3},[0-9]{1,3}", expected) is not None, "malformed color check")
        a, e = [int(v) for v in actual.split(",")], [int(v) for v in expected.split(",")]
        require(all(0 <= v <= 255 for v in a+e), "color channel outside 8-bit range")
        return all(abs(x-y) <= tolerance for x, y in zip(a, e))
    require(kind == "numeric", f"unknown check kind: {kind}")
    try:
        a, e = float(actual), float(expected)
    except (ValueError, TypeError):
        return False
    return all(math.isfinite(v) for v in (a, e)) and abs(a-e) <= tolerance


def base_identity(value):
    require(isinstance(value["tc"], str) and re.fullmatch(r"TC[0-9]{2,3}", value["tc"]), "invalid TC identity")
    require(integer(value["pid"], 1) and integer(value["run"], 1), "invalid process/run identity")
    return value["tc"], value["pid"], value["run"]


def contract_actions(value):
    scenarios = value["scenarios"]
    require(isinstance(scenarios, list) and scenarios, "empty scenario contract")
    names, result = set(), []
    for scenario in scenarios:
        name = scenario["id"]
        require(nonempty_string(name) and name not in names, "duplicate/invalid scenario contract")
        names.add(name)
        steps, step_names, row_start = scenario["steps"], set(), 1
        require(isinstance(steps, list) and steps, f"empty step contract: {name}")
        for step in steps:
            require(nonempty_string(step["id"]) and step["id"] not in step_names, f"duplicate/invalid step: {name}")
            step_names.add(step["id"])
            required = step["required_checks"]
            require(integer(required, 1), f"invalid mandatory count: {name}")
            result.append((name, step["id"], required, row_start))
            row_start += required
    return scenarios, result


def reviewed_manifest(manifest_path, manifest_sha256):
    """Load a separately reviewed manifest, pinned outside the candidate log."""
    require(hash_string(manifest_sha256), "an externally pinned manifest SHA-256 is required")
    path = Path(manifest_path) if manifest_path is not None else HERE / "layout-validation-manifest.json"
    raw = path.read_bytes()
    require(hashlib.sha256(raw).hexdigest() == manifest_sha256, "reviewed manifest fingerprint differs from the external pin")
    manifest = strict_json(raw.decode("utf-8"))
    require(isinstance(manifest, dict), "reviewed manifest must be an object")
    return manifest


def reviewed_contracts(manifest_path, manifest_sha256, profile, requested):
    """Load the required case contracts without trusting the runtime contract."""
    require(profile in {"diagnostic", "release"}, "an explicit diagnostic/release contract profile is required")
    manifest = reviewed_manifest(manifest_path, manifest_sha256)
    bundle = manifest.get("reviewed_contracts")
    require(isinstance(bundle, dict) and type(bundle.get("schema")) is int and bundle["schema"] == 1,
            "missing or unsupported reviewed_contracts schema")
    profiles = bundle.get("profiles")
    require(isinstance(profiles, dict) and isinstance(profiles.get(profile), dict),
            f"missing reviewed profile contract: {profile}")
    cases = profiles[profile]
    selected = {}
    for tc in requested:
        expected = cases.get(tc)
        require(isinstance(expected, dict) and expected.get("reviewed") is True,
                f"missing/unreviewed manifest contract: {profile}/{tc}")
        require("scenarios" in expected, f"missing manifest scenarios: {profile}/{tc}")
        contract_actions(expected)
        selected[tc] = expected
    return selected


def check_log(path, requested, external=False, *, manifest_path=None, manifest_sha256=None, profile=None):
    require(requested and len(set(requested)) == len(requested), "requested TC list must be nonempty and unique")
    require(all(isinstance(tc, str) and re.fullmatch(r"TC[0-9]{2,3}", tc) for tc in requested), "invalid requested TC")
    reviewed = reviewed_contracts(manifest_path, manifest_sha256, profile, requested)
    # Select the newest run encountered for each TC. An abandoned pre-Reset run
    # cannot poison a later run, and an old PASS cannot hide a newer incomplete run.
    grouped, latest, seen = {}, {}, set()
    for kind, value in records(path):
        base = base_identity(value)
        if base[0] not in requested:
            continue
        if base not in seen:
            seen.add(base)
            latest[base[0]] = base
        grouped.setdefault(base, []).append((kind, value))
    require(set(latest) == set(requested), f"missing requested TCs: {sorted(set(requested)-set(latest))}")
    # A failing row is EVIDENCE, not a parse error: the structural invariants below (every
    # scenario completed, every declared row observed, unique IDs, totals consistent) are the
    # only thing that decides whether the log can be trusted, and they must all be evaluated
    # even for a log full of failures. The failing rows are reported separately.
    completed, external_tcs, failing_rows, frame_failures = [], [], {}, {}
    for tc in requested:
        base = latest[tc]
        contract, actions, rows, final, ending = None, {}, {}, None, None
        failing, frames = [], {}
        previous_totals = (0, 0, 0)
        for kind, value in grouped[base]:
            require(ending is None or (kind == "LR_CASE_END" and ending == value),
                    f"new/changed records after case end: {base}")
            if kind == "LR_CASE_CONTRACT":
                require(contract is None or contract == value, f"changed immutable contract: {base}")
                contract_actions(value)
                require(value["scenarios"] == reviewed[tc]["scenarios"],
                        f"runtime contract differs from reviewed manifest: {profile}/{tc}")
                contract = value
                continue
            require(contract is not None, f"missing case contract before {kind}: {base}")
            scenarios, expected_actions = contract_actions(contract)
            if kind == "LR_NOOP":
                # An ignored button press: it creates no rows and changes no state.
                require(nonempty_string(value.get("button")) and nonempty_string(value.get("reason")), f"malformed noop: {base}")
                continue
            if kind == "LR_FRAME":
                # An abnormal frame-fence completion. It is not a row and changes no count, but
                # it names WHY the action's render.frame-timeout row exists, which the row alone
                # does not, and the pairing of the two is enforced with the row counts below.
                require(value.get("result") in FRAME_RESULTS and nonempty_string(value.get("reason")) and
                        nonempty_string(value.get("scenario")) and isinstance(value.get("step"), str) and
                        integer(value.get("action_seq"), 1) and integer(value.get("attempts")) and
                        finite_number(value.get("elapsed_ms")) and value["elapsed_ms"] >= 0 and
                        integer(value.get("deadline_ms"), 1), f"malformed frame record: {base}")
                # A record is evidence only if it can be attributed: it must name an action the
                # contract declares AND that this run has already started, with the same scope.
                # Otherwise a record for an action that never ran would silently pass, carrying a
                # reason for a timeout nothing in the run can be paired with.
                sequence = value["action_seq"]
                require(sequence <= len(expected_actions), f"frame record for an undeclared action: {base}/{sequence}")
                require(sequence in actions, f"frame record before its action: {base}/{sequence}")
                require((value["scenario"], value["step"]) == (actions[sequence]["scenario"], actions[sequence]["step"]),
                        f"frame record scope mismatch: {base}/{sequence}")
                # The producer mirrors its records to stdout and stderr, so a repeat that is
                # identical AFTER PARSING (key order and spacing do not matter) is the same
                # record read twice; a repeat with a different VALUE is a rewritten one.
                require(sequence not in frames or frames[sequence] == value, f"changed frame record: {base}/{sequence}")
                frames[sequence] = value
                continue
            if kind == "LR_CLEANUP_ERROR":
                # A cleanup that threw leaves the process state unknown, so the run's evidence
                # is void. Scoped to the latest run of a requested TC: an abandoned earlier run
                # cannot poison a later one, and another TC's cleanup is not this TC's problem.
                require(False, f"cleanup failure: {base}")
            if kind == "LR_CASE_END":
                require(final is not None, f"case ended without a result: {base}")
                fields = ("failures", "completed_steps", "completed_scenarios", "required_scenarios")
                require(all(integer(value[k]) for k in fields), f"invalid case end counts: {base}")
                require(value["cleanup_passed"] is True, f"cleanup failure at case end: {base}")
                require(value["reason"] in {"reset", "exit"}, f"invalid case end reason: {base}")
                # The case end must agree with the final result on every count, failures
                # included: they describe the same run, and a disagreement means one of the two
                # records is not this run's.
                require(all(value[k] == final[k] for k in fields), f"case end differs from final result: {base}")
                ending = value
            elif kind == "LR_ACTION":
                sequence = value["action_seq"]
                require(integer(sequence, 1) and sequence == len(actions)+1, f"noncontiguous action: {base}")
                require(sequence <= len(expected_actions), f"extra action: {base}")
                name, step, required, _ = expected_actions[sequence-1]
                require((value["scenario"], value["step"], value["required_checks"]) == (name, step, required),
                        f"action differs from immutable contract: {base}/{sequence}")
                require(integer(value["required_checks"], 1), "invalid action count type")
                actions[sequence] = value
            elif kind == "LR_CHECK":
                sequence = value["action_seq"]
                require(integer(sequence, 1) and sequence in actions, f"row without action: {base}")
                name, step, required, start = expected_actions[sequence-1]
                require((value["scenario"], value["step"]) == (name, step), f"row scope mismatch: {base}")
                require(integer(value["row"], 1) and start <= value["row"] < start+required, f"row outside action range: {base}")
                require(nonempty_string(value["id"]), "empty assertion ID")
                identity = (sequence, value["row"])
                require(identity not in rows or rows[identity] == value, f"changed frozen row: {base}/{identity}")
                # Recompute the comparison independently: the producer's verdict flag must
                # agree with its own values, so a "passed" row carrying mismatched or
                # non-numeric values is invalid evidence rather than a pass.
                passed = value["passed"] is True
                require(numeric_check(value) == passed, f"row verdict disagrees with its values: {base}/{value['id']}")
                if not passed and identity not in rows:
                    failing.append({"action_seq": sequence, "row": value["row"], "id": value["id"],
                                    "scenario": value["scenario"], "step": value["step"],
                                    "actual": value["actual"], "expected": value["expected"],
                                    "tolerance": value["tolerance"]})
                rows[identity] = value
            else:
                require(kind == "LR_RESULT", "unsupported check-log record")
                fields = ("failures", "completed_steps", "completed_scenarios", "required_scenarios", "checks")
                require(all(integer(value[k]) for k in fields), f"invalid result counts: {base}")
                require(value["required_scenarios"] == len(scenarios), f"result changed required scenarios: {base}")
                totals = (value["completed_steps"], value["completed_scenarios"], value["checks"])
                require(all(a <= b for a, b in zip(previous_totals, totals)), f"cumulative result regressed: {base}")
                # Verdict grammar, not verdict value: FAIL/PASS/EXTERNAL_REQUIRED are terminal
                # and may only appear once every declared scenario has completed, and the
                # failure counter and the verdict must tell the same story.
                require(value["verdict"] in VERDICTS, f"unknown verdict: {base}: {value['verdict']}")
                require((value["verdict"] in TERMINAL_VERDICTS) ==
                        (value["completed_scenarios"] == value["required_scenarios"]),
                        f"terminal verdict before completion (or non-terminal after it): {base}: {value['verdict']}")
                require((value["failures"] != 0) == (value["verdict"] in FAILED_VERDICTS),
                        f"failure count disagrees with the verdict: {base}: {value['verdict']}")
                previous_totals, final = totals, value
        require(contract is not None and final is not None, f"missing contract/final: {base}")
        require(ending is not None, f"missing latest-run case end/cleanup evidence: {base}")
        scenarios, expected_actions = contract_actions(contract)
        allowed = {"PASS", "EXTERNAL_REQUIRED"} if external else {"PASS"}
        require(final["completed_scenarios"] == len(scenarios), f"missing scenario: {base}")
        require(final["completed_steps"] == len(actions) == len(expected_actions), f"missing action: {base}")
        for sequence, (_, _, required, _) in enumerate(expected_actions, 1):
            observed = [value for (seq, _), value in rows.items() if seq == sequence]
            # A fence failure short-circuits the action, so it is the usual cause of a short row
            # count. Name it here rather than leaving the operator with a bare arithmetic message.
            hint = f"; frame failure: {frames[sequence]['result']}" if sequence in frames else ""
            require(len(observed) == required, f"missing/extra check rows: {base}/{sequence}: {len(observed)}/{required}{hint}")
            require(len({value["id"] for value in observed}) == required, f"duplicate assertion IDs replace required checks: {base}/{sequence}")
            # The record and the row are two halves of one fact and neither stands alone: a
            # record without its row describes a timeout the action did not report, and a row
            # without its record is a timeout whose cause was never written down.
            has_row = any(row["action_seq"] == sequence and row["id"] == "render.frame-timeout" for row in failing)
            require(has_row or sequence not in frames,
                    f"frame failure record without its render.frame-timeout row: {base}/{sequence}")
            require(not has_row or sequence in frames,
                    f"render.frame-timeout row without its frame record: {base}/{sequence}")
        require(len(rows) == final["checks"], f"cumulative row mismatch: {base}")
        # The summary must account for exactly the failing rows and nothing else: a failure
        # counted without a row (an overflow, or an Append after the step finished) would
        # otherwise be invisible.
        require(final["failures"] == len(failing), f"summary failures differ from failing rows: {base}: {final['failures']}/{len(failing)}")
        if failing:
            require(final["verdict"] == "FAIL", f"failing rows without a FAIL verdict: {base}: {final['verdict']}")
            failing_rows[tc] = failing
        else:
            require(final["verdict"] in allowed, f"incomplete/external verdict: {base}: {final['verdict']}")
        completed.append(tc)
        if frames:
            frame_failures[tc] = [{"action_seq": sequence, "scenario": frames[sequence]["scenario"],
                                   "step": frames[sequence]["step"], "result": frames[sequence]["result"],
                                   "reason": frames[sequence]["reason"], "attempts": frames[sequence]["attempts"],
                                   "elapsed_ms": frames[sequence]["elapsed_ms"]}
                                  for sequence in sorted(frames)]
        if final["verdict"] == "EXTERNAL_REQUIRED":
            external_tcs.append(tc)
    return {"status": "observations-valid-with-failures" if (failing_rows or frame_failures) else "observations-valid",
            "tcs": sorted(completed), "contract_profile": profile, "contract_manifest_sha256": manifest_sha256,
            "external_verification_pending": bool(external_tcs), "external_tcs": sorted(external_tcs),
            "failing_rows": {tc: failing_rows[tc] for tc in sorted(failing_rows)},
            "frame_failures": {tc: frame_failures[tc] for tc in sorted(frame_failures)}}


# An explicit SHA-256 citation in contract prose. Only this shape is checked.
DOCUMENT_DIGEST_CITATION = re.compile(r"SHA-256\s+`([0-9a-f]{64})`")


def check_document_digests(paths, manifest_digest):
    """Every manifest SHA-256 the prose CITES must be the manifest's current digest.

    A stale citation is indistinguishable, to the operator who reads it, from the pin they
    are told to load into the trust store: it survived a manifest revision once already.
    """
    cited = 0
    for path in paths:
        path = Path(path)
        if not path.is_file():
            continue
        for value in DOCUMENT_DIGEST_CITATION.findall(path.read_text(encoding="utf-8")):
            cited += 1
            require(value == manifest_digest,
                    f"document cites a stale manifest SHA-256: {path.name}: {value} != {manifest_digest}")
    return cited


def audit():
    manifest = read_json(HERE / "layout-validation-manifest.json")
    expected = {f"TC{i}" for i in range(95,158)}
    require(len(manifest["cases"]) == len(expected) and {case["id"] for case in manifest["cases"]} == expected, "manifest case IDs missing or duplicated")
    for case in manifest["cases"]:
        stem = HERE / "tc" / case["stem"]
        require(stem.with_suffix(".cpp").is_file() and stem.with_suffix(".md").is_file(), f"missing pair {case['id']}")
        require(f"REGISTER_MANUAL_TEST(Tc{case['id'][2:]})" in stem.with_suffix(".cpp").read_text(), f"registration missing {case['id']}")
    # The suite sources are the tc-<number>-layout-*.cpp files numbered TC95-TC157.
    suite = [path for path in (HERE / "tc").glob("tc-*-layout-*.cpp")
             if re.fullmatch(r"tc-[0-9]+-layout-.+\.cpp", path.name) and "TC" + path.name.split("-")[1] in expected]
    require(len(suite) == len(expected), "unexpected TC count")
    # The md PROSE totals are what an operator reads before running anything, so they must
    # agree with the reviewed contract, not only with the table next to them: a stale total
    # makes the operator stop scenarios early and call the case complete.
    release = manifest["reviewed_contracts"]["profiles"]["release"]
    for case in manifest["cases"]:
        text = (HERE / "tc" / case["stem"]).with_suffix(".md").read_text(encoding="utf-8")
        scenarios = release[case["id"]]["scenarios"]
        steps = sum(len(scenario["steps"]) for scenario in scenarios)
        checks = sum(step["required_checks"] for scenario in scenarios for step in scenario["steps"])
        for stated in re.findall(r"총 scenario 수는 ([0-9]+)개", text):
            require(int(stated) == len(scenarios),
                    f"md scenario total differs from the contract: {case['id']}: {stated}/{len(scenarios)}")
        profiles = re.findall(r"^Profile [^:\n]*: ([0-9]+) scenario, ([0-9]+) action, ([0-9,]+) checks", text, re.M)
        require(profiles, f"md states no profile totals: {case['id']}")
        for stated in profiles:
            observed = (len(scenarios), steps, checks)
            require(tuple(int(value.replace(",", "")) for value in stated) == observed,
                    f"md profile totals differ from the contract: {case['id']}: {stated}/{observed}")
    manifest_digest = digest(HERE / "layout-validation-manifest.json")
    # Every manifest digest the PROSE quotes must be the current one.
    documents = sorted(HERE.glob("layout-validation-*.md")) + sorted((HERE / "tc" / case["stem"]).with_suffix(".md") for case in manifest["cases"])
    citations = check_document_digests(documents, manifest_digest)
    return {"status": "inventory-valid", "cases": len(expected),
            "document_digest_citations": citations, "manifest_sha256": manifest_digest}


def shrink(path):
    failure = read_json(path)
    require(isinstance(failure["nodes"], list) and failure["nodes"], "nodes required")
    candidates = []
    for index in range(len(failure["nodes"])):
        item = dict(failure)
        item["nodes"] = failure["nodes"][:index] + failure["nodes"][index+1:]
        item["candidate_removed_index"] = index
        item["reproduced"] = False
        candidates.append(item)
    return {"candidates": candidates, "requires_same_assertion_failure_on_replay": True}



def self_test():
    """Synthetic evidence exercises both acceptance and rejection end to end."""
    checks = []

    def passed(name, condition):
        require(condition, "self-test: "+name)
        checks.append(name)

    def rejected(name, callback):
        try:
            callback()
        except (ValueError, KeyError, TypeError, OverflowError):
            checks.append(name)
            return
        raise ValueError("self-test accepted invalid evidence: "+name)

    def rejected_with(name, callback, substring):
        """Rejection for the stated REASON: a rejection by an unrelated rule is not proof."""
        try:
            callback()
        except (ValueError, KeyError, TypeError, OverflowError) as error:
            require(substring in str(error), f"self-test: {name}: rejected for the wrong reason: {error}")
            checks.append(name)
            return
        raise ValueError("self-test accepted invalid evidence: "+name)

    def cli(name, arguments, expected_code):
        result = subprocess.run([sys.executable, str(Path(__file__).resolve())]+arguments,
                                capture_output=True, text=True, check=False)
        passed(name, result.returncode == expected_code and isinstance(strict_json(result.stdout), dict))

    def clone(value):
        return json.loads(json.dumps(value))

    def lines(path, values, extra=""):
        path.write_text("".join(kind+" "+json.dumps(value, allow_nan=True)+"\n" for kind, value in values)+extra, encoding="utf-8")

    def case(pid=100, run=1, scenarios=2, tc="TC95"):
        base = {"tc":tc,"pid":pid,"run":run}
        contract = dict(base, scenarios=[{"id":f"s{i}","steps":[{"id":"run","required_checks":2}]} for i in range(scenarios)])
        result = [("LR_CASE_CONTRACT", contract)]
        for i in range(scenarios):
            scope = dict(base, scenario=f"s{i}",step="run",action_seq=i+1)
            result.append(("LR_ACTION", dict(scope, required_checks=2)))
            result.append(("LR_CHECK", dict(scope,row=1,id="width",kind="numeric",actual="1.0005",expected="1",tolerance=.001,passed=True)))
            result.append(("LR_CHECK", dict(scope,row=2,id="count",kind="integer",actual="1",expected="1",tolerance=0,passed=True)))
        result.append(("LR_RESULT", dict(base, scenario=f"s{scenarios-1}",completed_steps=scenarios,completed_scenarios=scenarios,required_scenarios=scenarios,checks=scenarios*2,failures=0,verdict="PASS")))
        result.append(("LR_CASE_END", dict(base, completed_steps=scenarios, completed_scenarios=scenarios,
                                           required_scenarios=scenarios, failures=0, cleanup_passed=True, reason="exit")))
        return result

    passed("numeric-finite-tolerance", numeric_check({"kind":"numeric","actual":"1.0005","expected":"1","tolerance":.001}))
    passed("numeric-nan-rejected", not numeric_check({"kind":"numeric","actual":"nan","expected":"nan","tolerance":0}))
    passed("numeric-inf-rejected", not numeric_check({"kind":"numeric","actual":"inf","expected":"inf","tolerance":0}))
    passed("text-numeric-spelling-exact", not numeric_check({"kind":"text","actual":"01","expected":"1","tolerance":0}))
    passed("integer-above-double-precision", not numeric_check({"kind":"integer","actual":"9007199254740993","expected":"9007199254740992","tolerance":0}))
    passed("color-within-channel-tolerance", numeric_check({"kind":"color","actual":"229,57,53","expected":"231,55,56","tolerance":3}))
    passed("color-outside-channel-tolerance", not numeric_check({"kind":"color","actual":"229,57,53","expected":"229,57,60","tolerance":3}))
    rejected("color-malformed", lambda: numeric_check({"kind":"color","actual":"229,57","expected":"229,57,53","tolerance":3}))
    rejected("unknown-comparison-kind", lambda: numeric_check({"kind":"guess","actual":"1","expected":"1","tolerance":0}))
    rejected("duplicate-json-fields", lambda: strict_json('{"passed":false,"passed":true}'))
    rejected("nonfinite-json", lambda: strict_json('{"precision":NaN}'))
    rejected("overflowing-json-exponent", lambda: strict_json('{"precision":1e400}'))
    with tempfile.TemporaryDirectory(prefix="layout-tools-self-test-") as directory:
        root=Path(directory); log=root/"checks.log"; good=case()
        manifest_path = root/"reviewed-manifest.json"
        manifest = {"reviewed_contracts": {"schema": 1, "profiles": {
            "diagnostic": {"TC95": {"reviewed": True, "scenarios": clone(good[0][1]["scenarios"])}},
            "release": {"TC152": {"reviewed": True, "scenarios": clone(case(scenarios=1, tc="TC152")[0][1]["scenarios"])}}
        }}}
        manifest_path.write_text(json.dumps(manifest))
        manifest_pin = digest(manifest_path)
        contract_cli = ["--manifest", str(manifest_path), "--manifest-sha256", manifest_pin, "--profile", "diagnostic"]

        def checked(path, requested, external=False):
            return check_log(path, requested, external, manifest_path=manifest_path,
                             manifest_sha256=manifest_pin, profile="diagnostic")

        base_scope={"tc":"TC95","pid":100,"run":1}
        lines(log,good)
        rejected("missing-external-contract-pin", lambda: check_log(log,["TC95"],manifest_path=manifest_path,profile="diagnostic"))
        rejected("missing-contract-profile", lambda: check_log(log,["TC95"],manifest_path=manifest_path,manifest_sha256=manifest_pin))
        rejected("wrong-contract-pin", lambda: check_log(log,["TC95"],manifest_path=manifest_path,manifest_sha256="0"*64,profile="diagnostic"))
        rejected("wrong-profile-case-contract", lambda: check_log(log,["TC95"],manifest_path=manifest_path,manifest_sha256=manifest_pin,profile="release"))

        def invalid_manifest(name, value):
            manifest_path.write_text(json.dumps(value))
            rejected(name, lambda: check_log(log,["TC95"],manifest_path=manifest_path,
                                            manifest_sha256=digest(manifest_path),profile="diagnostic"))
            manifest_path.write_text(json.dumps(manifest))

        invalid_manifest("missing-reviewed-contract-metadata", {})
        bad_manifest=clone(manifest);bad_manifest["reviewed_contracts"]["profiles"]["diagnostic"]["TC95"]["reviewed"]=False
        invalid_manifest("unreviewed-contract-refused", bad_manifest)
        bad_manifest=clone(manifest);bad_manifest["reviewed_contracts"]["profiles"]["diagnostic"]["TC95"]["scenarios"][0]["steps"][0]["required_checks"]=0
        invalid_manifest("empty-reviewed-assertions-refused", bad_manifest)
        lines(log,case(scenarios=1))
        rejected("deleted-scenario-and-shrunken-runtime-contract", lambda: checked(log,["TC95"]))
        lines(log,good)
        contracted=clone(good)
        for scenario in contracted[0][1]["scenarios"]: scenario["steps"][0]["required_checks"]=1
        contracted=[entry for entry in contracted if entry[0]!="LR_CHECK" or entry[1]["row"]==1]
        for kind,value in contracted:
            if kind=="LR_ACTION":value["required_checks"]=1
            if kind=="LR_RESULT":value["checks"]=2
        lines(log,contracted)
        rejected("deleted-assertion-and-shrunken-runtime-contract", lambda: checked(log,["TC95"]))
        lines(log,good)
        passed("complete-two-scenario-log", checked(log,["TC95"])["status"] == "observations-valid")
        lines(log,good[:1]+[("LR_NOOP",{"tc":"TC95","pid":100,"run":1,"button":"Run all","reason":"step still running"})]+good[1:])
        passed("noop-record-ignored", checked(log,["TC95"])["status"] == "observations-valid")
        lines(log,good[:-1]);rejected("missing-latest-run-cleanup-end",lambda:checked(log,["TC95"]))
        for name,updates in [
            ("cleanup-end-failure", {"failures":1,"cleanup_passed":False}),
            ("cleanup-end-unconfirmed", {"cleanup_passed":False}),
            ("cleanup-end-count-mismatch", {"completed_steps":1}),
            ("cleanup-end-invalid-reason", {"reason":"unknown"}),
            ("cleanup-end-wrong-run", {"run":2}),
        ]:
            bad=clone(good);bad[-1][1].update(updates);lines(log,bad)
            rejected(name,lambda:checked(log,["TC95"]))
        lines(log,good+[("LR_RESULT",clone(good[-2][1]))])
        rejected("result-after-closed-run",lambda:checked(log,["TC95"]))
        lines(log,good+[("LR_SAMPLE",{"tc":"TC95","pid":100,"run":1})])
        rejected_with("retired-sample-record-rejected",lambda:checked(log,["TC95"]),"unknown structured LR record")
        latest=case(run=2)
        lines(log,good+latest[:-1]);rejected("older-end-cannot-close-new-run",lambda:checked(log,["TC95"]))
        changed=clone(good);changed[-1][1]["reason"]="reset";lines(log,changed)
        passed("complete-run-cleanup-through-reset",checked(log,["TC95"])["status"]=="observations-valid")
        cleanup_error=("LR_CLEANUP_ERROR",{"tc":"TC95","pid":100,"run":1,"scenario":"s1"})
        lines(log,good[:-1]+[cleanup_error]+good[-1:])
        rejected_with("cleanup-error-marker-overrides-pass",lambda:checked(log,["TC95"]),"cleanup failure")
        lines(log,[("LR_CLEANUP_ERROR",{"tc":"TC95","pid":100,"run":1,"scenario":"s0"})]+case(run=2))
        passed("cleanup-error-of-other-run-ignored",checked(log,["TC95"])["status"]=="observations-valid")
        lines(log,good,'noise LR_CHECK {"tc":"TC95","pid":100,"run":1}\n')
        rejected_with("embedded-record-marker-rejected",lambda:checked(log,["TC95"]),"record marker not at line start")
        lines(log,good)
        cli("complete-log-cli-zero",["check-log",str(log),"--tc","TC95"]+contract_cli,0)
        lines(log,good[:-1]+[clone(good[2])]+good[-1:])
        passed("identical-frozen-page-repeat", checked(log,["TC95"])["status"] == "observations-valid")
        bad=clone(good);bad=[entry for entry in bad if entry[1].get("scenario") != "s1" or entry[0]=="LR_RESULT"]
        bad[-2][1].update(completed_steps=1,completed_scenarios=1,required_scenarios=1,checks=2)
        lines(log,bad);rejected("lowered-summary-cannot-hide-scenario",lambda:checked(log,["TC95"]))
        lines(log,good[1:]);rejected("missing-immutable-contract",lambda:checked(log,["TC95"]))
        bad=clone(good);bad.pop(2);lines(log,bad);rejected("missing-result-page",lambda:checked(log,["TC95"]))
        cli("missing-page-cli-invalid",["check-log",str(log),"--tc","TC95"]+contract_cli,2)
        bad=clone(good);bad[3][1]["id"]="width";lines(log,bad);rejected("duplicate-assertion-cannot-fill-count",lambda:checked(log,["TC95"]))
        bad=clone(good);bad[3][1]["row"]=3;lines(log,bad);rejected("noncontiguous-row-numbers",lambda:checked(log,["TC95"]))
        bad=clone(good);bad[2][1]["actual"]="nan";lines(log,bad);rejected("passed-flag-cannot-hide-nan",lambda:checked(log,["TC95"]))
        bad=clone(good);bad[2][1].update(kind="text",actual="01",expected="1",tolerance=0);lines(log,bad);rejected("text-log-is-not-numeric",lambda:checked(log,["TC95"]))
        # A FAILING log is valid evidence: every structural invariant is still evaluated and the
        # failing rows are named, instead of the first one aborting the whole validation.
        failing=clone(good)
        failing[2][1].update(actual="2",expected="1",tolerance=.001,passed=False)
        failing[-2][1].update(failures=1,verdict="FAIL");failing[-1][1].update(failures=1)
        lines(log,failing)
        report=checked(log,["TC95"])
        passed("complete-failing-log-valid-with-named-failures",
               report["status"]=="observations-valid-with-failures" and
               [row["id"] for row in report["failing_rows"]["TC95"]]==["width"])
        cli("failing-log-cli-nonzero",["check-log",str(log),"--tc","TC95"]+contract_cli,1)
        truncated=[entry for entry in clone(failing)
                   if entry[1].get("scenario")!="s1" and entry[0] not in ("LR_RESULT","LR_CASE_END")]
        stopped=dict(base_scope,scenario="s0",completed_steps=1,completed_scenarios=1,required_scenarios=2,
                     checks=2,failures=1,verdict="FAILING")
        truncated.append(("LR_RESULT",stopped))
        truncated.append(("LR_CASE_END",dict(base_scope,completed_steps=1,completed_scenarios=1,
                                             required_scenarios=2,failures=1,cleanup_passed=True,reason="exit")))
        lines(log,truncated)
        rejected_with("truncated-failing-log-invalid-for-completion",lambda:checked(log,["TC95"]),"missing scenario")
        bad=clone(failing);bad[-2][1]["failures"]=2;bad[-1][1]["failures"]=2;lines(log,bad)
        rejected_with("failing-summary-mismatch",lambda:checked(log,["TC95"]),"summary failures differ from failing rows")
        early=dict(base_scope,scenario="s0",completed_steps=1,completed_scenarios=1,required_scenarios=2,
                   checks=2,failures=0,verdict="PASS")
        lines(log,good[:4]+[("LR_RESULT",early)]+good[4:])
        rejected_with("terminal-verdict-before-completion",lambda:checked(log,["TC95"]),"terminal verdict before completion")
        lines(log,good)
        lines(log,good,"LR_CHECK\n");rejected("truncated-record-after-complete-run",lambda:checked(log,["TC95"]))
        lines(log,good,"LR_CHE\n");rejected("truncated-record-marker",lambda:checked(log,["TC95"]))
        lines(log,good,'LR_CHECK {"tc":\n');rejected("truncated-json-after-complete-run",lambda:checked(log,["TC95"]))
        bad=clone(good);changed=clone(good[2]);changed[1]["actual"]="1.0004";lines(log,bad[:-1]+[changed]+bad[-1:]);rejected("changed-frozen-row",lambda:checked(log,["TC95"]))
        new=case(run=2);lines(log,good+[new[0]]);rejected("old-pass-cannot-hide-new-incomplete-run",lambda:checked(log,["TC95"]))
        lines(log,[good[0]]+new);passed("reset-selects-newest-run",checked(log,["TC95"])["status"]=="observations-valid")
        bad=clone(good);bad[-2][1]["verdict"]="EXTERNAL_REQUIRED";lines(log,bad);rejected("external-evidence-required-by-default",lambda:checked(log,["TC95"]))
        passed("external-mode-remains-pending",checked(log,["TC95"],external=True)["external_verification_pending"])
        cli("external-pending-cli-nonzero",["check-log",str(log),"--tc","TC95","--external"]+contract_cli,1)

        # The frame fence. An abnormal completion writes render.frame-timeout, so the action is
        # short of its declared rows and protocol.required_checks fires too. The LR_FRAME record
        # carries the reason; it is not a row, and it must never appear without its failing row.
        frame_scope=dict(base_scope,scenario="s1",step="run",action_seq=2)
        frame_record={"result":"timeout","reason":"frame watchdog reached the total deadline",
                      "attempts":3,"elapsed_ms":4001.2,"deadline_ms":4000}

        def frame_log(*, failing=True, record=True, updates=None, extra="", at=7, duplicate=None):
            entries=clone(good)
            if failing:
                entries[5]=["LR_CHECK",dict(frame_scope,row=1,id="render.frame-timeout",kind="failure",
                                            actual="frame watchdog reached the total deadline",expected="a fresh frame",tolerance=0,passed=False)]
                entries[6]=["LR_CHECK",dict(frame_scope,row=2,id="protocol.required_checks",kind="failure",
                                            actual="0",expected="2",tolerance=0,passed=False)]
                entries[-2][1].update(failures=2,verdict="FAIL")
                entries[-1][1].update(failures=2)
            if record:
                # `at` places the record relative to the action it names (index 7 is after the
                # second action, index 4 before it); `duplicate` repeats it with these changes.
                written=dict(frame_record,**(updates or {}))
                entries.insert(at,["LR_FRAME",dict(frame_scope,**written)])
                if duplicate is not None:
                    entries.insert(at+1,["LR_FRAME",dict(frame_scope,**dict(written,**duplicate))])
            lines(log,entries,extra)

        frame_log()
        report=checked(log,["TC95"])
        passed("frame-record-accepted-with-failure-row",
               report["status"]=="observations-valid-with-failures" and
               [row["id"] for row in report["failing_rows"]["TC95"]]==["render.frame-timeout","protocol.required_checks"] and
               len(report["frame_failures"]["TC95"])==1 and
               report["frame_failures"]["TC95"][0]["action_seq"]==2 and
               report["frame_failures"]["TC95"][0]["result"]=="timeout")
        frame_log(failing=False)
        rejected_with("frame-record-without-failure-row-rejected",lambda:checked(log,["TC95"]),
                      "frame failure record without its render.frame-timeout row")
        frame_log(updates={"result":"unknown"})
        rejected_with("frame-record-malformed-rejected",lambda:checked(log,["TC95"]),"malformed frame record")
        frame_log(record=False,extra='noise LR_FRAME {"tc":"TC95","pid":100,"run":1}\n')
        rejected_with("frame-marker-mid-line-rejected",lambda:checked(log,["TC95"]),"record marker not at line start")
        # A record is only evidence if it can be attributed to an action of THIS run: an
        # action the contract never declared, one that has not started yet, or a different
        # scenario/step names a timeout nothing in the run can be paired with.
        frame_log(updates={"action_seq":999999,"scenario":"nonexistent"})
        rejected_with("frame-record-undeclared-action-rejected",lambda:checked(log,["TC95"]),
                      "frame record for an undeclared action")
        frame_log(at=4)
        rejected_with("frame-record-before-its-action-rejected",lambda:checked(log,["TC95"]),
                      "frame record before its action")
        frame_log(updates={"scenario":"s0"})
        rejected_with("frame-record-scope-mismatch-rejected",lambda:checked(log,["TC95"]),
                      "frame record scope mismatch")
        # The producer mirrors every record to stdout and stderr, so a repeat that is identical
        # after parsing (key order and spacing do not matter) is one record read twice and is
        # accepted as one; a repeat that says something else is not.
        frame_log(duplicate={"elapsed_ms":4002.0})
        rejected_with("frame-record-altered-duplicate-rejected",lambda:checked(log,["TC95"]),
                      "changed frame record")
        frame_log(duplicate={})
        report=checked(log,["TC95"])
        passed("frame-record-identical-duplicate-accepted",
               report["status"]=="observations-valid-with-failures" and len(report["frame_failures"]["TC95"])==1)
        # The other pairing direction: a timeout row whose cause was never written down.
        frame_log(record=False)
        rejected_with("frame-row-without-record-rejected",lambda:checked(log,["TC95"]),
                      "render.frame-timeout row without its frame record")
        frame_log()
        cli("frame-failure-cli-nonzero",["check-log",str(log),"--tc","TC95"]+contract_cli,1)
        lines(log,[("LR_FRAME",dict(frame_scope,**frame_record))]+case(run=2))
        passed("frame-record-of-other-run-ignored",checked(log,["TC95"])["status"]=="observations-valid")
        lines(log,good)
        # A contract document quoting a manifest digest that is no longer the manifest's own.
        stale_doc = root / "layout-validation-stale-map.md"
        stale_doc.write_text("표의 수치는 단일 계약(`layout-validation-manifest.json`, SHA-256 `%s`)에서 생성했습니다.\n" % ("0" * 64),
                             encoding="utf-8")
        rejected_with("doc-quoted-digest-must-match-manifest",
                      lambda: check_document_digests([stale_doc], manifest_pin),
                      "document cites a stale manifest SHA-256")
    return {"status":"self-test-pass","checks":len(checks),"cases":checks}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="command", required=True)
    sub.add_parser("audit")
    sub.add_parser("self-test")
    check = sub.add_parser("check-log")
    check.add_argument("log")
    check.add_argument("--tc", action="append", required=True)
    check.add_argument("--external", action="store_true")
    check.add_argument("--manifest", default=str(HERE / "layout-validation-manifest.json"))
    check.add_argument("--manifest-sha256", help="Required SHA-256 pinned in a trusted source outside the candidate log")
    check.add_argument("--profile", choices=["diagnostic", "release"], help="Required reviewed contract profile")
    mini = sub.add_parser("shrink-candidates")
    mini.add_argument("failure")
    args = parser.parse_args()
    try:
        if args.command == "audit": result = audit()
        elif args.command == "self-test": result = self_test()
        elif args.command == "check-log": result = check_log(args.log, args.tc, args.external,
                                                             manifest_path=args.manifest, manifest_sha256=args.manifest_sha256, profile=args.profile)
        else: result = shrink(args.failure)
        print(json.dumps(result, indent=2, ensure_ascii=False, allow_nan=False))
        # Exit 0: nothing to act on. Exit 1: the evidence is valid and says something is
        # wrong or still owed (failing rows or an external verification).
        # Exit 2: the evidence itself could not be validated -- see the except branch.
        if result.get("failing_rows") or result.get("frame_failures"):
            return 1
        if result.get("external_verification_pending"):
            return 1
        return 0
    except (OSError, ValueError, KeyError, TypeError, OverflowError) as error:
        print(json.dumps({"status":"invalid-evidence","error":str(error)}, ensure_ascii=False))
        return 2


if __name__ == "__main__":
    sys.exit(main())
