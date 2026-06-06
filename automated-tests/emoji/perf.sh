#!/usr/bin/env bash
set -euo pipefail

usage()
{
  cat <<'USAGE'
Usage:
  ./perf.sh

Runs the actual Dali::Ui::Label emoji performance test and writes log, summary,
and HTML report under a timestamped /tmp/dali-emoji-perf-* directory by default.

Useful environment overrides:
  DALI_EMOJI_PERF_RUNS=5
  DALI_EMOJI_PERF_MAX_ITEMS=321
  DALI_EMOJI_PERF_FONT_FAMILY=SamsungOneUI_400
  DALI_EMOJI_PERF_FIXTURE=/path/to/emoji-test-latest.tsv
  DALI_EMOJI_PERF_OUTPUT_DIR=/tmp
  DALI_EMOJI_PERF_PIPELINE=1
  DALI_EMOJI_PERF_SWEEP_MAX_ROWS=0

  -h  Show this help.
USAGE
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    -h)
      usage
      exit 0
      ;;
    *)
      usage >&2
      exit 2
      ;;
  esac
done

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
BUILD_DIR="${DALI_EMOJI_BUILD_DIR:-${REPO_ROOT}/automated-tests/build}"
BINARY="${BUILD_DIR}/src/dali-ui-foundation-internal/dali-emoji-label-perf"
SWEEP_BINARY="${BUILD_DIR}/src/dali-ui-foundation-internal/dali-emoji-sequence-sweep"
OUTPUT_ROOT="${DALI_EMOJI_PERF_OUTPUT_DIR:-/tmp}"
STAMP="$(date +%Y%m%d-%H%M%S)"
RUN_DIR="${OUTPUT_ROOT%/}/dali-emoji-perf-${STAMP}"
LOG_PATH="${RUN_DIR}/label.log"
PIPELINE_LOG_PATH="${RUN_DIR}/pipeline.log"
PIPELINE_FAILURE_LOG="${RUN_DIR}/pipeline-failures.log"
SUMMARY_PATH="${RUN_DIR}/summary.txt"
HTML_PATH="${RUN_DIR}/report.html"

if [[ ! -x "${BINARY}" ]]; then
  echo "Missing label perf binary: ${BINARY}" >&2
  echo "Run ${SCRIPT_DIR}/build.sh first." >&2
  exit 1
fi

mkdir -p "${RUN_DIR}"

cd "${REPO_ROOT}"

set +e
"${BINARY}" 2>&1 | tee "${LOG_PATH}"
RUN_STATUS=${PIPESTATUS[0]}
set -e

PIPELINE_STATUS="SKIPPED"
if [[ "${DALI_EMOJI_PERF_PIPELINE:-1}" != "0" ]]; then
  if [[ -x "${SWEEP_BINARY}" ]]; then
    set +e
    DALI_EMOJI_SWEEP_MODE="${DALI_EMOJI_PERF_SWEEP_MODE:-all}" \
    DALI_EMOJI_SWEEP_PROFILE=1 \
    DALI_EMOJI_SWEEP_FAILURE_LOG="${PIPELINE_FAILURE_LOG}" \
    DALI_EMOJI_SWEEP_MAX_FAILURES="${DALI_EMOJI_PERF_SWEEP_MAX_FAILURES:-0}" \
    DALI_EMOJI_SWEEP_MAX_ROWS="${DALI_EMOJI_PERF_SWEEP_MAX_ROWS:-0}" \
      "${SWEEP_BINARY}" 2>&1 | tee "${PIPELINE_LOG_PATH}"
    PIPELINE_STATUS=${PIPESTATUS[0]}
    set -e
  else
    echo "Missing sweep binary for pipeline profile: ${SWEEP_BINARY}" | tee "${PIPELINE_LOG_PATH}" >&2
    PIPELINE_STATUS="MISSING"
  fi
fi

python3 - "${LOG_PATH}" "${PIPELINE_LOG_PATH}" "${SUMMARY_PATH}" "${HTML_PATH}" "${RUN_STATUS}" "${PIPELINE_STATUS}" "${RUN_DIR}" <<'PY'
import html
import re
import subprocess
import sys
from pathlib import Path

log_path = Path(sys.argv[1])
pipeline_log_path = Path(sys.argv[2])
summary_path = Path(sys.argv[3])
html_path = Path(sys.argv[4])
run_status = int(sys.argv[5])
pipeline_status = sys.argv[6]
run_dir = Path(sys.argv[7])

ansi = re.compile(r'\x1b\[[0-9;]*m')
kv = re.compile(r'(\w+)=([^\s]+)')
phase_order = ['initial_48', 'same_text_48', 'scale_32', 'scale_64']

rows = []
begin = {}
for raw in log_path.read_text(errors='replace').splitlines():
    line = ansi.sub('', raw)
    if 'LABEL_PERF_BEGIN' in line:
        begin = dict(kv.findall(line))
    if 'LABEL_PERF_PHASE' not in line:
        continue
    data = dict(kv.findall(line))
    rows.append({
        'run': int(data.get('run', '0')),
        'phase': data.get('phase', ''),
        'fontSize': data.get('fontSize', ''),
        'count': int(data.get('count', '0')),
        'completed': int(data.get('completed', '0')),
        'updateCallMs': int(data.get('updateCallMs', '0')),
        'asyncAllMs': int(data.get('asyncAllMs', '0')),
        'status': data.get('status', ''),
    })

pipeline_summary = {}
pipeline_profile = {}
if pipeline_log_path.exists():
    for raw in pipeline_log_path.read_text(errors='replace').splitlines():
        line = ansi.sub('', raw)
        if 'EMOJI_SWEEP_SUMMARY' in line:
            pipeline_summary = dict(kv.findall(line))
        elif 'EMOJI_SWEEP_PROFILE' in line:
            pipeline_profile = dict(kv.findall(line))

def average(values):
    return (sum(values) / len(values)) if values else None

def pct(values, index):
    if not values:
        return None
    ordered = sorted(values)
    pos = min(len(ordered) - 1, max(0, round((len(ordered) - 1) * index)))
    return ordered[pos]

def fmt(value):
    if value is None:
        return ''
    if isinstance(value, float) and not value.is_integer():
        return f'{value:.1f}'
    return str(int(value))

def fmt_float(value, digits=3):
    if value is None:
        return ''
    return f'{value:.{digits}f}'

def as_float(data, key):
    try:
        return float(data.get(key, '0'))
    except Exception:
        return 0.0

pipeline_stage_keys = [
    ('validateScriptMs', 'Validate script'),
    ('setScriptsForFontMs', 'Set scripts'),
    ('validateFontsMs', 'Validate fonts'),
    ('shapeTextMs', 'Shape text'),
    ('glyphMetricsMs', 'Glyph metrics'),
    ('createBitmapMs', 'Create bitmap'),
]
pipeline_stages = [
    {'key': key, 'label': label, 'ms': as_float(pipeline_profile, key)}
    for key, label in pipeline_stage_keys
]
pipeline_stage_total = sum(stage['ms'] for stage in pipeline_stages)

summary_rows = []
for phase in phase_order:
    selected = [row for row in rows if row['phase'] == phase and row['status'] == 'OK']
    async_values = [row['asyncAllMs'] for row in selected]
    update_values = [row['updateCallMs'] for row in selected]
    count = selected[0]['count'] if selected else 0
    async_average = average(async_values)
    update_average = average(update_values)
    summary_rows.append({
        'phase': phase,
        'runs': len(selected),
        'count': count,
        'completed': selected[0]['completed'] if selected else 0,
        'asyncAverage': async_average,
        'asyncMin': min(async_values) if async_values else None,
        'asyncMax': max(async_values) if async_values else None,
        'asyncP90': pct(async_values, 0.90),
        'asyncMsPerLabel': (async_average / count) if async_average is not None and count > 0 else None,
        'updateAverage': update_average,
        'updateMin': min(update_values) if update_values else None,
        'updateMax': max(update_values) if update_values else None,
        'updateMsPerLabel': (update_average / count) if update_average is not None and count > 0 else None,
    })

summary_by_phase = {row['phase']: row for row in summary_rows}

try:
    font_match = subprocess.check_output(
        ['fc-match', '-v', 'Noto Color Emoji'],
        text=True,
        stderr=subprocess.STDOUT,
    )
    font_match = '\n'.join(
        line for line in font_match.splitlines()
        if 'file:' in line or 'family:' in line or 'fullname:' in line
    )
except Exception as exc:
    font_match = f'fc-match failed: {exc}'

with summary_path.open('w') as out:
    out.write('DALi emoji actual Label performance\n')
    out.write(f'log={log_path}\n')
    out.write(f'html={html_path}\n')
    out.write(f'runDir={run_dir}\n')
    out.write(f'runStatus={run_status}\n')
    out.write(f'pipelineStatus={pipeline_status}\n')
    out.write(f"fixture={begin.get('fixture', '')}\n")
    out.write(f"count={begin.get('count', '')}\n")
    out.write(f"runs={begin.get('runs', '')}\n")
    out.write(f"fontFamily={begin.get('fontFamily', '')}\n")
    out.write('fontMatch:\n')
    out.write(font_match + '\n\n')
    out.write('phase runs count completed asyncAverageMs asyncMsPerLabel asyncP90Ms asyncMinMs asyncMaxMs updateAverageMs updateMsPerLabel updateMinMs updateMaxMs\n')
    for row in summary_rows:
        out.write(
            f"{row['phase']} {row['runs']} {row['count']} {row['completed']} "
            f"{fmt(row['asyncAverage'])} {fmt_float(row['asyncMsPerLabel'])} "
            f"{fmt(row['asyncP90'])} {fmt(row['asyncMin'])} {fmt(row['asyncMax'])} "
            f"{fmt(row['updateAverage'])} {fmt_float(row['updateMsPerLabel'])} {fmt(row['updateMin'])} {fmt(row['updateMax'])}\n"
        )
    out.write('\npipelineStatus mode totalRows executed passed failed skipped elapsedMs metricGlyphs bitmapGlyphs colorBitmaps\n')
    out.write(
        f"{pipeline_status} "
        f"{pipeline_summary.get('mode', '')} "
        f"{pipeline_summary.get('totalRows', '')} "
        f"{pipeline_summary.get('executed', '')} "
        f"{pipeline_summary.get('passed', '')} "
        f"{pipeline_summary.get('failed', '')} "
        f"{pipeline_summary.get('skipped', '')} "
        f"{pipeline_summary.get('elapsedMs', '')} "
        f"{pipeline_profile.get('metricGlyphs', '')} "
        f"{pipeline_profile.get('bitmapGlyphs', '')} "
        f"{pipeline_profile.get('colorBitmaps', '')}\n"
    )
    out.write('\npipelineStage stage ms percent\n')
    for stage in pipeline_stages:
        percent = (stage['ms'] / pipeline_stage_total * 100.0) if pipeline_stage_total > 0 else 0.0
        out.write(f"{stage['key']} {stage['ms']:.3f} {percent:.1f}%\n")

css = '''body{font-family:system-ui,-apple-system,Segoe UI,sans-serif;margin:28px;color:#1f2933}
table{border-collapse:collapse;width:100%;margin:16px 0 28px}
th,td{border:1px solid #d8dee9;padding:8px 10px;text-align:right}
th:first-child,td:first-child{text-align:left}
th{background:#eef2f7}
code,pre{background:#f1f3f5;border-radius:4px}
code{padding:2px 4px}
pre{padding:12px;overflow:auto}
.meta{color:#52616b}
.note{background:#fff8db;border-left:4px solid #f08c00;padding:12px 14px;margin:16px 0}
.quick{display:grid;grid-template-columns:repeat(auto-fit,minmax(180px,1fr));gap:12px;margin:18px 0 24px}
.metric{background:#f8fafc;border:1px solid #d8dee9;border-radius:6px;padding:12px}
.metric b{display:block;font-size:24px;line-height:1.1;margin-top:4px}
.metric span{color:#52616b;font-size:12px}
.barWrap{background:#edf2f7;border-radius:4px;height:18px;min-width:180px;overflow:hidden}
.bar{height:18px;background:#2f80ed}
.stack{display:flex;height:26px;border-radius:4px;overflow:hidden;background:#edf2f7;margin:12px 0}
.seg{height:26px}
.seg0{background:#2f80ed}.seg1{background:#27ae60}.seg2{background:#f2994a}.seg3{background:#9b51e0}.seg4{background:#eb5757}.seg5{background:#56ccf2}
.small{font-size:12px;color:#52616b}'''

summary_html = ''.join(
    '<tr>'
    f"<td>{html.escape(row['phase'])}</td>"
    f"<td>{row['runs']}</td>"
    f"<td>{row['count']}</td>"
    f"<td>{row['completed']}</td>"
    f"<td>{fmt(row['asyncAverage'])}</td>"
    f"<td>{fmt_float(row['asyncMsPerLabel'])}</td>"
    f"<td>{fmt(row['asyncP90'])}</td>"
    f"<td>{fmt(row['asyncMin'])}-{fmt(row['asyncMax'])}</td>"
    f"<td>{fmt(row['updateAverage'])}</td>"
    f"<td>{fmt_float(row['updateMsPerLabel'])}</td>"
    f"<td>{fmt(row['updateMin'])}-{fmt(row['updateMax'])}</td>"
    '</tr>'
    for row in summary_rows
)

raw_html = ''.join(
    '<tr>'
    f"<td>{row['run']}</td>"
    f"<td>{html.escape(row['phase'])}</td>"
    f"<td>{row['fontSize']}</td>"
    f"<td>{row['count']}</td>"
    f"<td>{row['completed']}</td>"
    f"<td>{row['asyncAllMs']}</td>"
    f"<td>{fmt_float((row['asyncAllMs'] / row['count']) if row['count'] else None)}</td>"
    f"<td>{row['updateCallMs']}</td>"
    f"<td>{html.escape(row['status'])}</td>"
    '</tr>'
    for row in sorted(rows, key=lambda r: (r['run'], phase_order.index(r['phase']) if r['phase'] in phase_order else 99))
)

max_async = max((row['asyncAverage'] or 0 for row in summary_rows), default=0)
label_graph = ''.join(
    '<tr>'
    f"<td>{html.escape(row['phase'])}</td>"
    f"<td>{fmt(row['asyncAverage'])}</td>"
    f"<td>{fmt_float(row['asyncMsPerLabel'])}</td>"
    f"<td><div class='barWrap'><div class='bar' style='width:{((row['asyncAverage'] or 0) / max_async * 100.0) if max_async else 0:.1f}%'></div></div></td>"
    '</tr>'
    for row in summary_rows
)

initial_phase = summary_by_phase.get('initial_48', {})
scale32_phase = summary_by_phase.get('scale_32', {})
scale64_phase = summary_by_phase.get('scale_64', {})
run_count = begin.get('runs', '')

def metric_card(title, row):
    return (
        "<div class='metric'><span>" + html.escape(title) + "</span>"
        "<b>" + fmt(row.get('asyncAverage')) + " ms</b>"
        "<span>" + fmt_float(row.get('asyncMsPerLabel')) + " ms/label</span></div>"
    )

quick_html = ''.join([
    "<div class='metric'><span>Total labels</span><b>" + html.escape(begin.get('count', '')) + "</b></div>",
    metric_card("Initial 48 FontSize " + html.escape(run_count) + "-run average", initial_phase),
    metric_card("32 FontSize " + html.escape(run_count) + "-run average", scale32_phase),
    metric_card("64 FontSize " + html.escape(run_count) + "-run average", scale64_phase),
])

pipeline_stack = ''
pipeline_rows = ''
for index, stage in enumerate(pipeline_stages):
    percent = (stage['ms'] / pipeline_stage_total * 100.0) if pipeline_stage_total > 0 else 0.0
    pipeline_stack += f"<div class='seg seg{index % 6}' title='{html.escape(stage['label'])}: {stage['ms']:.3f} ms ({percent:.1f}%)' style='width:{percent:.3f}%'></div>"
    pipeline_rows += (
        '<tr>'
        f"<td>{html.escape(stage['label'])}</td>"
        f"<td>{stage['ms']:.3f}</td>"
        f"<td>{percent:.1f}%</td>"
        f"<td><div class='barWrap'><div class='bar seg{index % 6}' style='width:{percent:.1f}%'></div></div></td>"
        '</tr>'
    )

pipeline_meta = '\n'.join([
    f'pipelineStatus={pipeline_status}',
    f"mode={pipeline_summary.get('mode', '')}",
    f"totalRows={pipeline_summary.get('totalRows', '')}",
    f"executed={pipeline_summary.get('executed', '')}",
    f"passed={pipeline_summary.get('passed', '')}",
    f"failed={pipeline_summary.get('failed', '')}",
    f"skipped={pipeline_summary.get('skipped', '')}",
    f"elapsedMs={pipeline_summary.get('elapsedMs', '')}",
    f"metricGlyphs={pipeline_profile.get('metricGlyphs', '')}",
    f"bitmapGlyphs={pipeline_profile.get('bitmapGlyphs', '')}",
    f"colorBitmaps={pipeline_profile.get('colorBitmaps', '')}",
])

html_path.write_text(f'''<!doctype html><html><head><meta charset="utf-8">
<title>DALi Emoji Label Performance</title><style>{css}</style></head><body>
<h1>DALi Emoji Label Performance</h1>
<p class="meta">Actual <code>Dali::Ui::Label</code> async rendering benchmark.</p>
<div class="note">Top section measures elapsed time until every label emits <code>AsyncRenderFinishedSignal</code>. The pipeline section uses the sweep profile path to show aggregate text rendering stack cost.</div>
<h2>At a Glance</h2>
<div class="quick">{quick_html}</div>
<h2>Label Rendering Time</h2>
<table><thead><tr><th>Phase</th><th>Async average ms</th><th>ms/label</th><th>Relative</th></tr></thead><tbody>{label_graph}</tbody></table>
<h2>Pipeline Stack Time</h2>
<div class="stack">{pipeline_stack}</div>
<table><thead><tr><th>Stage</th><th>ms</th><th>Share</th><th>Relative</th></tr></thead><tbody>{pipeline_rows}</tbody></table>
<pre>{html.escape(pipeline_meta)}</pre>
<h2>Label Summary</h2>
<table><thead><tr><th>Phase</th><th>Runs</th><th>Count</th><th>Completed</th><th>Async average ms</th><th>ms/label</th><th>Async p90 ms</th><th>Async range ms</th><th>Update average ms</th><th>Update ms/label</th><th>Update range ms</th></tr></thead><tbody>{summary_html}</tbody></table>
<h2>Metadata</h2>
<pre>{html.escape('runDir=' + str(run_dir) + chr(10) + 'runStatus=' + str(run_status) + chr(10) + 'fixture=' + begin.get('fixture', '') + chr(10) + 'count=' + begin.get('count', '') + chr(10) + 'runs=' + begin.get('runs', '') + chr(10) + 'fontFamily=' + begin.get('fontFamily', '') + chr(10) + font_match)}</pre>
<h2>Raw Runs</h2>
<table><thead><tr><th>Run</th><th>Phase</th><th>Font size</th><th>Count</th><th>Completed</th><th>Async ms</th><th>ms/label</th><th>Update ms</th><th>Status</th></tr></thead><tbody>{raw_html}</tbody></table>
<p class="meta">Label log: <code>{html.escape(str(log_path))}</code><br>Pipeline log: <code>{html.escape(str(pipeline_log_path))}</code><br>Summary: <code>{html.escape(str(summary_path))}</code></p>
</body></html>''')

print(f'Emoji label perf summary: {summary_path}')
print(f'Emoji label perf HTML: {html_path}')
print(f'Emoji label perf output dir: {run_dir}')
PY

exit "${RUN_STATUS}"
