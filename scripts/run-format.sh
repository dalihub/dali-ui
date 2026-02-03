#!/bin/bash

# 1. 명확하게 16버전 이상의 실행 파일만 탐색
CF=$(command -v clang-format-15 || command -v clang-format-16)

if [ -z "$CF" ]; then
    echo "❌ [ERROR] Clang-format 15+ is required."
    echo "Please install:"
    echo "> wget https://apt.llvm.org/llvm.sh && chmod +x llvm.sh && sudo ./llvm.sh 15"
    echo "> sudo apt install -y clang-format-15"
    exit 1
fi

echo "✅ Using $CF"

# 2. 대상 파일 수집
TARGETS="dali-ui-foundation dali-ui-components"
FILES=$(find $TARGETS -path "./build" -prune -o -path "./third-party" -prune -o -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -print)

# 3. 모드별 실행
case "$1" in
    ("check")
        $CF --dry-run --Werror -style=file $FILES
        ;;
    ("fix")
        $CF -i -style=file $FILES
        echo "✨ Formatting complete!"
        ;;
    (*)
        echo "Usage: $0 {check|fix}"
        exit 1
        ;;
esac
