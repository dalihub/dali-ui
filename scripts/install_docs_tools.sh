#!/bin/bash

# Exit on error
set -e

echo "==============================================="
echo "   DALi UI Documentation Tools Installer"
echo "==============================================="

# Configuration
BASE_DIR="build/doc"
BIN_DIR="$BASE_DIR/bin"
VENV_DIR="$BASE_DIR/venv"

mkdir -p "$BIN_DIR"

# 1. System Dependencies (Still need system level for doxygen and python basics)
echo "[1/4] Checking system dependencies..."
sudo apt update
sudo apt install -y doxygen python3-pip python3-venv unzip wget

# 2. Doxybook2 (Local Install)
echo "[2/4] Installing Doxybook2 locally to $BIN_DIR..."
TEMP_ZIP="doxybook2_temp.zip"
wget -O "$TEMP_ZIP" https://github.com/matusnovak/doxybook2/releases/download/v1.5.0/doxybook2-linux-amd64-v1.5.0.zip
unzip -o "$TEMP_ZIP" bin/doxybook2
mv bin/doxybook2 "$BIN_DIR/"
rm -rf bin "$TEMP_ZIP"
echo "Doxybook2 installed to $BIN_DIR/doxybook2"

# 3. MkDocs & Theme (Local Venv Install)
echo "[3/4] Setting up Python Virtual Environment in $VENV_DIR..."
python3 -m venv "$VENV_DIR"
source "$VENV_DIR/bin/activate"
pip3 install --upgrade pip
pip3 install mkdocs-material
echo "MkDocs-Material installed in virtual environment."

# 4. Finalizing
echo "[4/4] Finalizing setup..."
chmod +x scripts/build_docs.sh
echo "==============================================="
echo "   SUCCESS: All tools are installed in $BASE_DIR!"
echo "   Run './scripts/build_docs.sh' to build."
echo "==============================================="
