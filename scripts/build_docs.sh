#!/bin/bash

# MkDoxy handles Doxygen XML generation and Markdown conversion internally.
# This script only needs to run mkdocs build.

SOURCE_DOC_DIR="build/doc/source"
GUIDES_DIR="docs"

# Ensure the docs source directory exists
if [ ! -d "$SOURCE_DOC_DIR" ]; then
    echo "Error: Documentation source directory '$SOURCE_DOC_DIR' not found."
    exit 1
fi

# Stage hand-authored guide pages from docs/ into the mkdocs source tree.
if [ -d "$GUIDES_DIR" ]; then
    echo "Staging guide pages from '$GUIDES_DIR' into '$SOURCE_DOC_DIR/guides'..."
    rm -rf "$SOURCE_DOC_DIR/guides"
    mkdir -p "$SOURCE_DOC_DIR/guides"
    cp -r "$GUIDES_DIR"/. "$SOURCE_DOC_DIR/guides/"
fi

echo "Cleaning up previous build artifacts..."
rm -rf site

echo "Building documentation with MkDocs + MkDoxy..."

MKDOCS_CMD=""
if command -v mkdocs &> /dev/null; then
    MKDOCS_CMD="mkdocs build"
elif python3 -m mkdocs --version &> /dev/null; then
    MKDOCS_CMD="python3 -m mkdocs build"
fi

if [ ! -z "$MKDOCS_CMD" ]; then
    $MKDOCS_CMD
else
    echo "Error: mkdocs not found. Install with: pip install mkdocs-material mkdoxy"
    exit 1
fi

echo "Documentation build complete. Output is in 'site/'"
