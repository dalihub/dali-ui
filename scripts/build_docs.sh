#!/bin/bash

# Configuration
DOXYFILE="Doxyfile.xml"
DOXYBOOK_CONFIG="doxybook_config.json"

# Unified Directories
BASE_DIR="build/doc"
SOURCE_DOC_DIR="$BASE_DIR/source"
XML_OUT_DIR="$BASE_DIR/xml"
STAGING_DIR="$BASE_DIR/staging"
API_OUT_DIR="$STAGING_DIR/api"

echo "Cleaning up previous staging and xml artifacts..."
rm -rf "$XML_OUT_DIR" "$STAGING_DIR" site

echo "Step 1: Setting up staging directory..."
mkdir -p "$STAGING_DIR"

# 1. Copy hand-written docs from unified build/doc/source (The home for all doc sources)
if [ -d "$SOURCE_DOC_DIR" ]; then
    # Copy all markdown and assets from the build/doc/source folder
    cp -r "$SOURCE_DOC_DIR"/* "$STAGING_DIR/" 2>/dev/null || true
fi

echo "Step 2: Running Doxygen (XML Extraction)..."
doxygen $DOXYFILE

echo "Step 3: Running Doxybook2 (XML -> Markdown)..."
DOXYBOOK_BIN=""
# 1. Check local build/doc/bin
if [ -f "$BASE_DIR/bin/doxybook2" ]; then
    DOXYBOOK_BIN="$BASE_DIR/bin/doxybook2"
# 2. Check system command
elif command -v doxybook2 &> /dev/null; then
    DOXYBOOK_BIN="doxybook2"
# 3. Check common system path
elif [ -f "/usr/local/bin/doxybook2" ]; then
    DOXYBOOK_BIN="/usr/local/bin/doxybook2"
fi

if [ ! -z "$DOXYBOOK_BIN" ]; then
    mkdir -p "$API_OUT_DIR"
    $DOXYBOOK_BIN --input "$XML_OUT_DIR" --output "$API_OUT_DIR" --config "$DOXYBOOK_CONFIG"
else
    echo "Warning: doxybook2 not found. Skipping MD conversion."
fi

echo "Step 4: Building MkDocs (Markdown -> HTML)..."
# Try using the local virtual environment if it exists
if [ -f "$BASE_DIR/venv/bin/activate" ]; then
    echo "Using virtual environment in $BASE_DIR/venv..."
    source "$BASE_DIR/venv/bin/activate"
fi

MKDOCS_CMD=""
if command -v mkdocs &> /dev/null; then
    MKDOCS_CMD="mkdocs build"
elif python3 -m mkdocs --version &> /dev/null; then
    MKDOCS_CMD="python3 -m mkdocs build"
fi

if [ ! -z "$MKDOCS_CMD" ]; then
    # Simply run using root mkdocs.yml (already set to docs_dir: build/doc/staging)
    $MKDOCS_CMD
    # Deactivate if we were in a venv
    if [ ! -z "$VIRTUAL_ENV" ]; then deactivate; fi
else
    echo "Error: mkdocs not found. Please run './scripts/install_docs_tools.sh' first."
    exit 1
fi

echo "Documentation build complete. Output is in 'site/'"
