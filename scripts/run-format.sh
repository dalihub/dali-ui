#!/bin/bash

# 1. 명확하게 16버전 이상의 실행 파일만 탐색
CF=$(command -v clang-format-15 || command -v clang-format-16)

if [ -z "$CF" ]; then
    echo "> [ERROR] Clang-format 15+ is required."
    echo "Please install:"
    echo "> wget https://apt.llvm.org/llvm.sh && chmod +x llvm.sh && sudo ./llvm.sh 15"
    echo "> sudo apt install -y clang-format-15"
    exit 1
fi

echo "✅ Using $CF"

# 2. 대상 파일 수집
# TARGETS="dali-ui-foundation dali-ui-components"
# FILES=$(find $TARGETS -path "./build" -prune -o -path "./third-party" -prune -o -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -print)

# --cached: staged 상태인 파일 목록
# --diff-filter=ACMR: Added, Copied, Modified, Renamed 파일만 (Deleted 제외)
STAGED_FILES=$(git diff --cached --name-only --diff-filter=ACMR | grep -E '\.(cpp|h|hpp)$' | grep -v 'third-party/\|build/')

if [ -z "$STAGED_FILES" ]; then
    echo "ℹ> No staged C++ files to process."
    exit 0
fi

# 3. 모드별 실행
case "$1" in
    ("check")
        # $CF --dry-run --Werror -style=file $STAGED_FILES
        echo "> Checking staged files with $CF..."
        # 파일 목록을 하나씩 diff에 안전하게 전달
        # -n 1: 파일 하나당 한 번씩 실행
        # sh -c: 인자를 받아서 실행할 작은 쉘 스크립트 정의
        echo "$STAGED_FILES" | xargs -n 1 sh -c '
            FILE="$1"
            CF_EXE="'"$CF"'"
            if ! $CF_EXE -style=file "$FILE" | diff -u "$FILE" - --label "$FILE (current)" --label "$FILE (formatted)"; then
                exit 1
            fi
        ' --
        if [ $? -ne 0 ]; then
            exit 1
        else
            echo "> Style check passed!"
        fi
        ;;
    ("fix")
        echo "> Fixing staged files with $CF..."
        FIXED_COUNT=0

        for file in $STAGED_FILES; do
            # 수정 전 해시값 저장
            BEFORE_HASH=$(md5sum "$file")

            # 포맷팅 실행
            $CF -i -style=file "$file"

            # 수정 후 해시값 비교
            AFTER_HASH=$(md5sum "$file")

            if [ "$BEFORE_HASH" != "$AFTER_HASH" ]; then
                echo "> Fixed: $file"
                git add "$file"
                FIXED_COUNT=$((FIXED_COUNT + 1))
            fi
        done

        if [ $FIXED_COUNT -eq 0 ]; then
            echo "> No files needed formatting."
        else
            echo "> Formatting complete! $FIXED_COUNT file(s) were updated and re-staged."
        fi
        ;;

    (*)
        echo "Usage: $0 {check|fix}"
        exit 1
        ;;
esac
