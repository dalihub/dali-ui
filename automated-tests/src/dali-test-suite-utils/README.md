# dali-test-suite-utils

테스트 공통 유틸리티 모음. 두 개의 계층으로 관리된다.

---

## 디렉토리 구조

```
dali-test-suite-utils/
├── README.md
├── dali-ui-test-suite-utils.h     ← 우산 헤더 (dali-ui 자체 관리)
├── test-application.h/.cpp        ← 그 외 파일 전부 dali-core 에서 복사
├── test-gesture-generator.h/.cpp
├── ...                            
└── dali-ui/                       ← dali-ui 용 구현 (dali-toolkit에서 필요한 만큼 복사)
    ├── ui-adaptor.cpp
    ├── ui-adaptor-impl.h
    ├── ui-async-task-manager.h/.cpp
    ├── ...
```

---

## 관리 정책

### 최상위 파일 (dali-core 동기화)

`dali-test-suite-utils/` 바로 아래 파일들(`test-*.h/.cpp`, `mesh-builder.*`, `render-surface-interface.h` 등)은
**dali-core의 `automated-tests/src/dali-test-suite-utils/`에서 복사**해 와서 유지한다.

- dali-core에서 업스트림 변경이 생기면 해당 파일들을 일괄 덮어쓰기로 동기화해야 한다.
- 이 파일들에는 dali-ui 전용 코드를 추가하지 않는다.

### `dali-ui/` 서브디렉토리 (자체 관리)

`dali-ui/` 아래 파일들(`ui-*.h/.cpp`)은 **dali-ui가 자체적으로 관리**한다.

- dali-toolkit이 구현한 adaptor/timer/window 등의 dummy 모듈을 원본으로 삼아 포팅한 것이다.
  - 원본: `dali-toolkit/automated-tests/src/dali-toolkit-test-utils/toolkit-*.h/.cpp`
  - 포팅: `toolkit-` 접두사를 `ui-`로 변경, dali-toolkit 의존성(`dali-toolkit-common.h`, `DALI_TOOLKIT_API` 등) 제거
- dali-toolkit과 직접 동기화하지 않는다. 필요에 따라 선택적으로 참고·수정한다.
- `dali-ui-test-suite-utils.h`가 이 디렉토리의 우산 헤더 역할을 한다.

---
