#!/usr/bin/env python3
#
# Copyright (c) 2026 Samsung Electronics Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
"""Generate local Unicode emoji sample PNGs from chart HTML.

Example:

  python3 automated-tests/emoji/tools/generate-unicode-emoji-samples.py \
    --output /tmp/dali-emoji-unicode-samples \
    --sample-set sample

  python3 automated-tests/emoji/tools/generate-unicode-emoji-samples.py \
    --chart-file /tmp/full-emoji-list.html \
    --chart-file /tmp/full-emoji-modifiers.html \
    --output /tmp/dali-emoji-unicode-samples \
    --sample-set sample
"""

import argparse
import base64
import binascii
import datetime
import html.parser
import json
import re
import sys
from collections import OrderedDict
from pathlib import Path
from urllib.parse import unquote, unquote_to_bytes, urljoin, urlparse
from urllib.request import Request, url2pathname, urlopen


DEFAULT_CHART_URLS = [
    "https://unicode.org/emoji/charts/full-emoji-list.html",
    "https://unicode.org/emoji/charts/full-emoji-modifiers.html",
    "https://unicode.org/emoji/charts/emoji-zwj-sequences.html",
    "https://unicode.org/emoji/charts/emoji-sequences.html",
]
PNG_SIGNATURE = b"\x89PNG\r\n\x1a\n"

SAMPLE_SET_ALIASES = OrderedDict(
    [
        ("sample", ("sample", "unicodesample")),
        ("apple", ("appl", "apple")),
        ("google", ("goog", "google")),
        ("windows", ("wind", "windows", "microsoft", "microsoftwindows", "mswindows")),
        ("samsung", ("sams", "samsung")),
        ("twitter", ("twtr", "twitter")),
        ("facebook", ("fb", "facebook")),
    ]
)

ALIAS_TO_SAMPLE_SET = {
    alias: sample_set for sample_set, aliases in SAMPLE_SET_ALIASES.items() for alias in aliases
}


class Cell:
    def __init__(self, tag, attrs):
        self.tag = tag
        self.attrs = attrs
        self.text_parts = []
        self.images = []

    def text(self):
        return " ".join("".join(self.text_parts).split())

    def colspan(self):
        value = self.attrs.get("colspan", "1")
        try:
            return max(1, int(value))
        except ValueError:
            return 1


class Row:
    def __init__(self):
        self.cells = []

    def has_header_cells(self):
        return any(cell.tag == "th" for cell in self.cells)

    def has_data_cells(self):
        return any(cell.tag == "td" for cell in self.cells)


class EmojiChartParser(html.parser.HTMLParser):
    def __init__(self):
        super().__init__(convert_charrefs=True)
        self.rows = []
        self.current_row = None
        self.current_cell = None

    def handle_starttag(self, tag, attrs):
        tag = tag.lower()
        attr_map = dict(attrs)

        if tag == "tr":
            self._finish_row()
            self.current_row = Row()
            return

        if tag in ("td", "th"):
            if self.current_row is None:
                self.current_row = Row()
            self._finish_cell()
            self.current_cell = Cell(tag, attr_map)
            return

        if tag == "img" and self.current_cell is not None:
            self.current_cell.images.append(attr_map)
            return

        if tag == "br" and self.current_cell is not None:
            self.current_cell.text_parts.append(" ")

    def handle_endtag(self, tag):
        tag = tag.lower()
        if tag in ("td", "th"):
            self._finish_cell()
        elif tag == "tr":
            self._finish_row()

    def handle_data(self, data):
        if self.current_cell is not None:
            self.current_cell.text_parts.append(data)

    def close(self):
        super().close()
        self._finish_row()

    def _finish_cell(self):
        if self.current_cell is not None and self.current_row is not None:
            self.current_row.cells.append(self.current_cell)
        self.current_cell = None

    def _finish_row(self):
        self._finish_cell()
        if self.current_row is not None and self.current_row.cells:
            self.rows.append(self.current_row)
        self.current_row = None


class ChartSource:
    def __init__(self, kind, location, text):
        self.kind = kind
        self.location = location
        self.text = text

    def display_name(self):
        if self.kind == "file":
            return Path(self.location).name

        parsed = urlparse(self.location)
        basename = Path(parsed.path).name
        return basename if basename else self.location

    def metadata(self):
        return {
            "type": self.kind,
            "location": self.location,
            "name": self.display_name(),
        }


class HeaderLayout:
    def __init__(self, code_index, sample_set_columns):
        self.code_index = code_index
        self.sample_set_columns = sample_set_columns


class ExtractionRecord:
    def __init__(self, sequence_key, sample_set, relative_path, source_name):
        self.sequence_key = sequence_key
        self.sample_set = sample_set
        self.relative_path = relative_path
        self.source_name = source_name


def warn(message):
    print(f"warning: {message}", file=sys.stderr)


def normalize_text(text):
    return re.sub(r"[^a-z0-9]+", "", text.lower())


def text_tokens(text):
    return [normalize_text(part) for part in re.split(r"[^A-Za-z0-9]+", text) if part]


def canonical_sample_set_from_text(text):
    normalized = normalize_text(text)
    if normalized in ALIAS_TO_SAMPLE_SET:
        return ALIAS_TO_SAMPLE_SET[normalized]

    for token in text_tokens(text):
        if token in ALIAS_TO_SAMPLE_SET:
            return ALIAS_TO_SAMPLE_SET[token]

    return None


def canonical_sample_set_from_option(text):
    sample_set = canonical_sample_set_from_text(text)
    if sample_set is not None:
        return sample_set

    normalized = normalize_text(text)
    if normalized in SAMPLE_SET_ALIASES:
        return normalized

    return None


def expand_cells(cells):
    expanded = []
    for cell in cells:
        expanded.extend([cell] * cell.colspan())
    return expanded


def is_code_header(text):
    normalized = normalize_text(text)
    return normalized == "code" or "code" in text_tokens(text)


def parse_header_layout(row):
    if not row.has_header_cells():
        return None

    code_index = None
    sample_set_columns = OrderedDict()

    for index, cell in enumerate(expand_cells(row.cells)):
        cell_text = cell.text()
        if code_index is None and is_code_header(cell_text):
            code_index = index

        sample_set = canonical_sample_set_from_text(cell_text)
        if sample_set is not None and index not in sample_set_columns:
            sample_set_columns[index] = sample_set

    if code_index is None or not sample_set_columns:
        return None

    return HeaderLayout(code_index, sample_set_columns)


def sequence_key_from_code_text(text):
    codepoints = re.findall(r"U\+([0-9A-Fa-f]{1,6})", text)
    if not codepoints:
        return None

    return "-".join(f"{int(codepoint, 16):x}" for codepoint in codepoints)


def parse_data_uri(uri):
    header, separator, payload = uri.partition(",")
    if not separator:
        raise ValueError("data URI does not contain a comma separator")

    media_type = header[5:].lower()
    if not media_type.startswith("image/png"):
        return None

    if ";base64" in media_type:
        try:
            return base64.b64decode(payload, validate=False)
        except binascii.Error as error:
            raise ValueError(f"invalid base64 data URI: {error}") from error

    return unquote_to_bytes(payload)


def make_request(url):
    return Request(url, headers={"User-Agent": "dali-ui-emoji-sample-generator/1.0"})


def read_url(url, timeout):
    with urlopen(make_request(url), timeout=timeout) as response:
        return response.read().decode("utf-8", "replace")


def read_url_bytes(url, timeout):
    with urlopen(make_request(url), timeout=timeout) as response:
        return response.read()


def local_path_from_src(src, chart_path):
    parsed = urlparse(src)

    if parsed.scheme == "file":
        return Path(url2pathname(unquote(parsed.path)))

    path_text = unquote(parsed.path if parsed.path else src)
    path = Path(path_text)
    if path.is_absolute():
        return path

    return chart_path.parent / path


def read_image_src(src, source, timeout):
    if src.startswith("data:"):
        return parse_data_uri(src)

    parsed = urlparse(src)
    if parsed.scheme in ("http", "https"):
        return read_url_bytes(src, timeout)

    if source.kind == "url":
        return read_url_bytes(urljoin(source.location, src), timeout)

    return local_path_from_src(src, Path(source.location)).read_bytes()


def extract_png_from_cell(cell, source, timeout, context):
    for image in cell.images:
        src = image.get("src", "")
        if not src:
            continue

        try:
            data = read_image_src(src, source, timeout)
        except OSError as error:
            warn(f"{context}: failed to read image {src!r}: {error}")
            continue
        except ValueError as error:
            warn(f"{context}: failed to parse image {src!r}: {error}")
            continue

        if data is None:
            continue

        if not data.startswith(PNG_SIGNATURE):
            continue

        return data

    return None


def is_existing_png(path):
    try:
        with path.open("rb") as input_file:
            return input_file.read(len(PNG_SIGNATURE)) == PNG_SIGNATURE
    except OSError:
        return False


def write_png(output_path, data):
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_bytes(data)


def extract_unicode_version(text):
    patterns = [
        r"/Public/([0-9]+(?:\.[0-9]+){1,2})/emoji/",
        r"Emoji\s+Version\s+([0-9]+(?:\.[0-9]+)*)",
        r"Unicode\s+Emoji\s+([0-9]+(?:\.[0-9]+)*)",
    ]
    for pattern in patterns:
        match = re.search(pattern, text, re.IGNORECASE)
        if match:
            return match.group(1)
    return None


def parse_chart(text):
    parser = EmojiChartParser()
    parser.feed(text)
    parser.close()
    return parser.rows


def extract_from_source(source, output_dir, selected_sample_sets, no_overwrite, timeout, extracted_keys):
    rows = parse_chart(source.text)
    header_layout = None
    seen_sample_columns = set()
    records = []

    for row in rows:
        parsed_header = parse_header_layout(row)
        if parsed_header is not None:
            header_layout = parsed_header
            seen_sample_columns.update(header_layout.sample_set_columns.values())
            continue

        if header_layout is None or not row.has_data_cells():
            continue

        cells = expand_cells(row.cells)
        if header_layout.code_index >= len(cells):
            continue

        sequence_key = sequence_key_from_code_text(cells[header_layout.code_index].text())
        if sequence_key is None:
            continue

        for column_index, sample_set in header_layout.sample_set_columns.items():
            if selected_sample_sets is not None and sample_set not in selected_sample_sets:
                continue

            if column_index >= len(cells):
                continue

            extraction_key = (sample_set, sequence_key)
            if extraction_key in extracted_keys:
                continue

            relative_path = Path(sample_set) / f"{sequence_key}.png"
            output_path = output_dir / relative_path
            context = f"{source.display_name()} {sequence_key} {sample_set}"

            if no_overwrite and is_existing_png(output_path):
                extracted_keys.add(extraction_key)
                records.append(
                    ExtractionRecord(
                        sequence_key,
                        sample_set,
                        relative_path.as_posix(),
                        source.display_name(),
                    )
                )
                continue

            data = extract_png_from_cell(cells[column_index], source, timeout, context)
            if data is None:
                continue

            try:
                write_png(output_path, data)
            except OSError as error:
                warn(f"{context}: failed to write {output_path}: {error}")
                continue

            extracted_keys.add(extraction_key)
            records.append(
                ExtractionRecord(
                    sequence_key,
                    sample_set,
                    relative_path.as_posix(),
                    source.display_name(),
                )
            )

    return records, seen_sample_columns


def write_index(output_dir, records):
    index_path = output_dir / "index.tsv"
    with index_path.open("w", encoding="utf-8", newline="") as output:
        output.write("sequence_key\tsample_set\tfile\tsource\n")
        for record in records:
            output.write(
                f"{record.sequence_key}\t{record.sample_set}\t{record.relative_path}\t{record.source_name}\n"
            )


def write_metadata(output_dir, sources, records, seen_sample_columns, unicode_versions):
    metadata_path = output_dir / "metadata.json"
    extracted_sample_sets = sorted({record.sample_set for record in records})
    unique_versions = sorted(version for version in set(unicode_versions) if version)

    metadata = OrderedDict(
        [
            ("generatedAt", datetime.datetime.now(datetime.timezone.utc).replace(microsecond=0).isoformat()),
            ("sourceCharts", [source.metadata() for source in sources]),
            ("unicodeVersion", unique_versions[0] if len(unique_versions) == 1 else None),
            ("unicodeVersions", unique_versions),
            ("extractedSampleSets", extracted_sample_sets),
            ("seenSampleColumns", sorted(seen_sample_columns)),
            ("extractedVendors", extracted_sample_sets),
            ("seenVendorColumns", sorted(seen_sample_columns)),
            ("imageCount", len(records)),
        ]
    )

    with metadata_path.open("w", encoding="utf-8") as output:
        json.dump(metadata, output, indent=2)
        output.write("\n")


def load_sources(chart_urls, chart_files, timeout):
    sources = []

    for url in chart_urls:
        sources.append(ChartSource("url", url, read_url(url, timeout)))

    for chart_file in chart_files:
        path = Path(chart_file)
        sources.append(ChartSource("file", str(path), path.read_text(encoding="utf-8")))

    return sources


def parse_args():
    parser = argparse.ArgumentParser(
        description="Generate a local Unicode emoji sample PNG pack from chart HTML.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""Examples:
  python3 automated-tests/emoji/tools/generate-unicode-emoji-samples.py \\
    --output /tmp/dali-emoji-unicode-samples \\
    --sample-set sample

  python3 automated-tests/emoji/tools/generate-unicode-emoji-samples.py \\
    --chart-file /tmp/full-emoji-list.html \\
    --chart-file /tmp/full-emoji-modifiers.html \\
    --output /tmp/dali-emoji-unicode-samples \\
    --sample-set sample
""",
    )
    parser.add_argument(
        "--chart-url",
        action="append",
        default=[],
        help="Unicode emoji chart URL to read. May be specified more than once.",
    )
    parser.add_argument(
        "--chart-file",
        action="append",
        default=[],
        help="Local Unicode emoji chart HTML file to read. May be specified more than once.",
    )
    parser.add_argument(
        "--output",
        default="/tmp/dali-emoji-unicode-samples",
        help="Output directory for metadata, index, and sample-set PNG directories.",
    )
    parser.add_argument(
        "--sample-set",
        dest="sample_sets",
        action="append",
        default=[],
        help=(
            "Limit extraction to a sample set. May be specified more than once. "
            "Supported values include sample, apple, google, windows, samsung, twitter, facebook."
        ),
    )
    parser.add_argument(
        "--vendor",
        dest="sample_sets",
        action="append",
        help=(
            "Legacy alias for --sample-set. Useful for older scripts or vendor-named chart columns."
        ),
    )
    parser.add_argument(
        "--no-overwrite",
        action="store_true",
        help="Keep an existing sample_set/key.png file when it is already a PNG.",
    )
    parser.add_argument(
        "--timeout",
        type=float,
        default=30.0,
        help="Network timeout in seconds for chart and relative image URL reads.",
    )
    return parser.parse_args()


def main():
    args = parse_args()

    chart_urls = args.chart_url
    chart_files = args.chart_file
    if not chart_urls and not chart_files:
        chart_urls = list(DEFAULT_CHART_URLS)

    selected_sample_sets = None
    if args.sample_sets:
        selected_sample_sets = set()
        for sample_set_text in args.sample_sets:
            sample_set = canonical_sample_set_from_option(sample_set_text)
            if sample_set is None:
                supported = ", ".join(SAMPLE_SET_ALIASES.keys())
                print(
                    f"Unsupported sample set {sample_set_text!r}. Supported sample sets: {supported}",
                    file=sys.stderr,
                )
                return 2
            selected_sample_sets.add(sample_set)

    output_dir = Path(args.output)
    output_dir.mkdir(parents=True, exist_ok=True)

    try:
        sources = load_sources(chart_urls, chart_files, args.timeout)
    except OSError as error:
        print(f"Failed to read chart input: {error}", file=sys.stderr)
        return 1

    all_records = []
    all_seen_sample_columns = set()
    extracted_keys = set()
    unicode_versions = []

    for source in sources:
        unicode_versions.append(extract_unicode_version(source.text))
        records, seen_sample_columns = extract_from_source(
            source,
            output_dir,
            selected_sample_sets,
            args.no_overwrite,
            args.timeout,
            extracted_keys,
        )
        all_records.extend(records)
        all_seen_sample_columns.update(seen_sample_columns)

    write_index(output_dir, all_records)
    write_metadata(output_dir, sources, all_records, all_seen_sample_columns, unicode_versions)

    print(f"Wrote {output_dir / 'index.tsv'}")
    print(f"Wrote {output_dir / 'metadata.json'}")
    print(f"Images: {len(all_records)}")
    print("Sample sets:", ", ".join(sorted({record.sample_set for record in all_records})) or "none")

    if selected_sample_sets is not None and not all_records:
        warn("no images were extracted for the selected sample set(s)")

    return 0


if __name__ == "__main__":
    sys.exit(main())
