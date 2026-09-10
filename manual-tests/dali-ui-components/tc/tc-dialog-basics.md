# Dialog 기본·사용자 스타일과 modal 소유권

[소스](tc-dialog-basics.cpp)의 `Dialog: 스타일과 modal 소유권` 항목을 선택한다.
기존 TC 런처의 `REGISTER_MANUAL_TEST`와 CMake `tc/*.cpp` 검색으로 등록한다.
새 TC를 추가한 뒤 CMake configure를 다시 실행해야 소스가 빌드 목록에 포함된다.

## 준비와 실행

수정된 Dialog-family typed-style API가 설치된 DALi core/adaptor/foundation/components 환경과
C++17 도구가 필요하다. 저장소 루트에서 실행하며 사용자 설치 prefix의 pkg-config·동적 라이브러리
검색 경로는 해당 환경에 맞게 설정한다. 런처가 Components UiConfig를 적용하므로 TC는 재적용하지 않는다.

```sh
cmake -S manual-tests/dali-ui-components -B /tmp/dali-ui-components-manual-build
cmake --build /tmp/dali-ui-components-manual-build
./manual-tests/dali-ui-components/bin/manual-test-dali-ui-components
```

세로 ScrollView 안에 조작 버튼·상태 줄과 네 미리보기가 있다. 콘텐츠 너비 312 이상이 확보되는
창 크기를 사용한다. 다른 창 크기·배율에서는 클리핑 여부를 별도 기록한다.

## 화면 구성과 초기 기대값

1. 기본 AlertDialog: 무인자 New()로 생성한 제목·본문과 `기본 동작` action.
2. 사용자 Dialog: Default().Configure()에서 흰 배경, Dialog 자체 padding, spacing 12,
   둥근 모서리를 지정한 header/body/footer 예제. Header와 Body의 padding은
   각 Label에 직접 설정하며 Dialog에 부착한 뒤에도 유지된다.
3. 기본 DialogContainer: 기본 Dialog를 보관한다. 설치 전 앱이 명시 modal flag를 true로 설정했다.
4. 사용자 DialogContainer: 파란 계열 generated scrim 위에 사용자 AlertDialog를 보관한다.
   Alert style은 사용자 DialogStyle·제목 22·본문 16·파란 action style·action row 48을 조합한다.
   설치 전 Alert의 명시 modal flag는 false다.

초기 상태 줄은 두 content가 모두 설치됨, 기본 Dialog flag true(원래 true),
사용자 Alert flag true(원래 false), generated scrim, 제목 갱신 0회, action/scrim 클릭 0회를 표시한다.
표시되는 값은 `GetModalContent()`와 `IsAccessibilityModal()`에서 읽는 관찰값이며
성공/실패 자동 판정이나 AT 발화 결과가 아니다.

## 조작과 기대 결과

| 조작 | 기대 결과 |
|---|---|
| `제목 갱신`을 두 번 누른다. | 기본·사용자 Alert 제목에 갱신 번호가 붙는다. 기존 사용자 header 객체가 유지되며 사용자 제목 색/크기, 본문, action이 유지된다. 상태의 제목 갱신 횟수와 `제목 객체 유지`가 현재 getter 결과를 반영한다. |
| `scrim 교체 / 복원`을 누른다. | 사용자 container의 generated scrim이 주황색 custom InteractiveView로 교체된다. 스타일의 파란색으로 덮이지 않아야 한다. 다시 누르면 보관했던 generated scrim으로 복원된다. modal content는 유지된다. |
| content 바깥의 scrim 영역을 탭한다. | 상태의 scrim 클릭 횟수만 증가한다. 이 TC는 자동 dismiss를 연결하지 않았으므로 content는 남는다. generated/custom 양쪽에서 반복한다. |
| `modal 제거 / 재설치`를 누른다. | 두 container의 content가 사라지고 scrim만 남는다. 상태의 설치 여부는 두 항목 모두 false, 기본 Dialog flag는 true, 사용자 Alert flag는 원래 값 false로 복원되어야 한다. |
| 분리 상태에서 `제목 갱신`을 누른 뒤 `modal 제거 / 재설치`를 다시 누른다. | 보관했던 동일 content가 재설치되고 분리 중 변경한 Alert 제목이 보인다. 두 설치 여부와 두 explicit flag가 모두 true다. |
| 두 Alert의 `기본 동작` / `사용자 동작`을 누른다. | action 클릭 횟수와 마지막 동작 문구가 갱신된다. 자체 닫기 동작은 없다. |

제거·재설치와 scrim 교체를 세 번 이상 반복한다. 중복 section/action이 생기거나
이미 제거된 scrim이 클릭을 전달하거나, 사용자 Alert의 explicit flag가 분리 후 true로 남으면 실패다.
잘못된 실제 값을 TC 코드가 false로 덮어써 통과시키지 않도록, 제거 후에는 getter로만 관찰한다.

## 수명·접근성·통과 기준

`< Back` 또는 하드웨어 Back으로 목록에 돌아간 후 재진입하면 새 핸들·제목·카운터·기본 scrim으로
시작해야 한다. OnExit는 연결을 해제하고 modal content를 먼저 분리한 뒤 보관 핸들을 해제한다.
타이머나 Window 전역 signal은 사용하지 않는다. Escape는 런처의 앱 종료다.

이 화면은 launcher 안의 미리보기이며 전역 입력 차단·focus trap을 구현하지 않는다.
Dialog role의 암묵적 modal 의미와 `IsAccessibilityModal()`의 명시 flag는 다르다.
명시 false로 복원되어도 DIALOG role의 실제 accessible 상태가 비-modal이라고 단정하면 안 된다.
실제 focus 이동/복원, 접근성 이름, 키보드 dismiss 정책과 action 연결은 앱 책임이다.
이 TC에는 timer 기반 자동 판정이나 스크린 리더/AT 성공 표시가 없다.

위 표의 외형·내용·소유권 결과가 일치해야 통과로 기록한다. 빌드 버전, 기기, 창 크기, 배율,
각 조작 전후 상태 줄·화면과 미실행 항목을 남긴다. 실제 AT 탐색·발화·D-Bus 통지 및 modal
재진입 회귀 UTC 결과는 별도 근거로 기록한다. 이 문서는 시나리오이며 실행 완료 보고가 아니다.

[Dialog API 안내](../../../wiki/Dialog.md), [공통 계약](../../../wiki/ComponentConsistency.md).
