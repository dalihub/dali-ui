"""MkDocs hook to exclude file-reference pages from search results.

The search plugin indexes pages during on_page_context, and hooks run
after plugins, so setting page.meta there is too late. Instead we
filter the generated search_index.json in on_post_build.
"""

import json
import os
import re


def on_post_build(config):
    """Remove file-reference entries from the search index."""
    index_path = os.path.join(config["site_dir"], "search", "search_index.json")
    if not os.path.exists(index_path):
        return

    with open(index_path, "r", encoding="utf-8") as f:
        data = json.load(f)

    original_count = len(data.get("docs", []))

    # Filter out file pages:
    #   - Doxygen mangled file names contain _8h, _8cpp, _8hpp, etc.
    #   - The file list page ends with /files.html
    file_page_pattern = re.compile(r"(_8\w+\.html|/files\.html)")

    data["docs"] = [
        doc for doc in data.get("docs", [])
        if not file_page_pattern.search(doc.get("location", ""))
    ]

    filtered_count = len(data["docs"])
    print(f"search_exclude_files: removed {original_count - filtered_count} file entries from search index")

    with open(index_path, "w", encoding="utf-8") as f:
        json.dump(data, f, ensure_ascii=False)