#!/bin/bash

# 1. 명확하게 16버전 이상의 실행 파일만 탐색
CF=$(command -v clang-format-20)

if [ -z "$CF" ]; then
    echo "> [ERROR] Clang-format 20+ is required."
    echo "Please install:"
    echo "> sudo apt install -y clang-format-20"
    exit 1
fi

echo "✅ Using $CF"

case "$1" in
    ("full")
        echo "> Fixing all src files with $CF..."
        # 2. 대상 파일 수집
        TARGETS="dali-ui-foundation dali-ui-elements dali-ui-components"
        FILES=$(find $TARGETS -path "./build" -prune -o -path "./automated-tests" -prune -o -path "./samples" -prune -o -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) ! -name "*.autogen.h" -print)
        FIXED_COUNT=0

        for file in $FILES; do
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
            echo "> Formatting complete! $FIXED_COUNT file(s) were updated."
        fi
        ;;

    ("check")
        echo "> Fixing staged files with $CF..."
        # --cached: staged 상태인 파일 목록
        # --diff-filter=ACMR: Added, Copied, Modified, Renamed 파일만 (Deleted 제외)
        STAGED_FILES=$(git diff --cached --name-only --diff-filter=ACMR | grep -E '\.(cpp|h|hpp)$' | grep -v 'automated-tests/\|samples/\|build/\|\.autogen\.h$')

        if [ -z "$STAGED_FILES" ]; then
            echo "ℹ> No staged C++ files to process."
            exit 0
        fi
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
        echo "Usage: $0 {full|check}"
        exit 1
        ;;
esac
