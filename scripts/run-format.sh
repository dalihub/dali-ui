#!/bin/bash

# 현재 브랜치 이름을 가져옵니다.
branch="$(git rev-parse --abbrev-ref HEAD)"

# 특정 브랜치인 경우 훅을 종료합니다.
if [ "$branch" = "wiki" ]; then
  exit 0
fi

# 커밋 전 스타일 체크 스크립트 실행
echo "Running pre-commit style check..."
./scripts/run-format.sh check

# 스크립트 결과(exit code)를 Git에 전달
# 0이면 성공(커밋 진행), 1이면 실패(커밋 중단)
RESULT=$?

if [ $RESULT -ne 0 ]; then
    echo "Error: Code style check failed!"
    echo "Please run './scripts/run-format.sh fix' and try again."
    exit 1
fi

exit 0

