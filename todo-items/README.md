# Todo Items

이 폴더는 현재 작업에서 바로 구현하지 않았지만, 앞으로 누군가 처리해 주기를 바라는
설계 개선, 리팩토링, 기술 부채와 후속 작업을 기록하기 위한 공간이다.

각 문서는 즉시 실행 지시나 확정된 구현 계획이 아니다. 작업의 배경, 현재 구조,
제안 방향, 예상 영향, 미결 사항과 검증 항목을 남겨 이후 작업자가 충분한 맥락을
가지고 다시 검토할 수 있게 한다.

향후 AI-driven 방식으로 `dali-ui`의 정기적인 분석과 리팩토링이 자동화된다면, 이
폴더의 문서들이 작업 후보로 발견되고 우선순위와 유효성을 검토받은 뒤 처리될 수
있기를 기대한다. 자동화된 작업 또한 문서의 제안을 그대로 적용하기보다 당시 코드와
요구사항을 다시 확인하고, 필요한 승인과 검증 절차를 따라야 한다.

완료되었거나 더 이상 유효하지 않은 문서는 구현 결과 또는 폐기 이유를 기록한 뒤 이
폴더에서 제거하거나 적절한 결정 기록 문서로 이동한다.

## 작업 목록

| 문서 | 상태 | 요약 |
|---|---|---|
| [SoundFeedback optional dependency](sound-feedback-optional-dependency.md) | 설계 필요 | Native multimedia 의존성을 필요한 제품에서만 제공하는 구조 |
| [Style DefaultPreset 이름 변경](rename-style-default-preset.md) | 명칭 결정 필요 | Built-in fallback style을 더 정확히 나타내는 public API 이름 검토 |
| [DALi Signal 단일 connection 최적화](optimize-dali-signal-single-connection.md) | 설계·측정 필요 | 0/1/N connection storage 검토 |
| [Trait BaseObject storage](trait-base-object-storage.md) | 설계 필요 | Trait 저장 구조 검토 |
| [Chromium WebView backend bugs](chromium-webview-backend-bugs.md) | 확인된 버그 | History item과 JavaScript handler routing 수정 |
