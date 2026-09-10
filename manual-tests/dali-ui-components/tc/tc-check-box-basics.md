# CheckBox 기본 동작

[소스](tc-check-box-basics.cpp)의 `CheckBox: basics` 항목을 선택한다.
빌드·환경·실행 명령은 [TextButton 준비와 실행](tc-text-button-basics.md#준비와-실행)과 같다.

## 화면 구성

세로로 box-only, `Labelled (AUTO animate)`, `Pre-checked`,
`Read-only (click ignored)`, `Always animate`, `Never animate`와 상태 줄이 있다.
box-only는 56×56, 나머지는 WRAP_CONTENT 폭과 높이 56을 사용한다.
기본 아이콘 리소스가 설치되어야 Lottie 체크 표시를 확인할 수 있다.
이 TC에는 ScrollView가 없으므로 모든 항목이 보이는 창 크기로 테스트한다.

## 조작과 기대 결과

1. 진입하면 `Pre-checked`만 선택되어 있다. 이 초기 `SetSelected(true)`도 시그널을
   발생시키므로 상태 줄이 `Pre-checked : checked`인 것은 정상이다.
2. box-only와 Labelled를 각각 두 번 탭한다. 선택·해제가 번갈아 적용되고 상태 줄은
   `(box-only) : checked/unchecked`, `Labelled (AUTO animate) : checked/unchecked`로 바뀐다.
   AUTO 모드의 사용자 탭에는 선택·해제 애니메이션이 나타나야 한다.
3. Pre-checked를 탭하면 해제되고 다시 탭하면 선택된다.
4. Read-only를 반복 탭한다. 체크 상태와 상태 줄이 바뀌지 않아야 한다.
   이 행은 `SetToggleByClickEnabled(false)` 예제이지 disabled 상태 예제가 아니다.
5. Always animate와 Never animate를 번갈아 두 번씩 탭한다. 둘 다 상태와 문구가
   바뀌되 전자는 애니메이션을 사용하고 후자는 즉시 최종 체크 상태를 표시해야 한다.
6. `< Back` 또는 하드웨어 Back으로 목록에 돌아갔다가 재진입한다.
   초기 선택 상태로 복원되고 이전 화면 항목이 중복되지 않아야 한다. Escape는 앱 종료다.

## 통과 기준과 한계

모든 활성 항목에서 화면 체크 표시와 최신 상태 줄이 일치하고, 읽기 전용 항목은
사용자 탭으로 변하지 않아야 한다. 리소스 로드 실패와 동작 실패를 구분해서 기록한다.
실행 기기·화면 크기·배율·선택 전후 화면·애니메이션 관찰 결과를 남긴다.
이 문서는 미실행 시나리오다. 커스텀 icon generator, 프로그램 변경 시 ENABLED 모드,
접근성 이름·역할·읽기 순서는 이 TC만으로 검증하지 않는다.
