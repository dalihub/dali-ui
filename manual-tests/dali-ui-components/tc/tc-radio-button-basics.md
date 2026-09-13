# RadioButton 그룹과 상태

[소스](tc-radio-button-basics.cpp)의 `RadioButton: grouping and states`를 선택한다.
빌드·환경·실행 명령은 [TextButton 준비와 실행](tc-text-button-basics.md#준비와-실행)과 같다.
이 TC에는 ScrollView가 없으므로 전체 행이 보이는 창 크기를 사용한다.

## 화면과 조작

| 화면 | 조작 | 기대 결과 |
|---|---|---|
| Parent auto-group | Auto A → B → C 순으로 탭하고 C를 재탭한다. | 같은 직접 부모 아래에서 한 항목만 선택된다. C 재탭은 선택을 해제하지 않는다. 상태 줄은 선택된 이름과 `selected`를 표시한다. |
| Named group | Named A → B → C를 탭한다. | 직접 부모가 달라도 동일 group name 안에서 단일 선택된다. 위 Auto 그룹 선택은 유지된다. |
| AUTO / always / never | 세 아이콘을 번갈아 탭한다. | 해당 행 안에서 단일 선택된다. AUTO와 ENABLED는 사용자 선택 전환을 애니메이션하고 DISABLED는 즉시 반영한다. |
| Read-only / disabled preselected / custom | 처음 두 아이콘을 탭한다. | Read-only는 클릭 선택을 무시한다. disabled preselected는 초기 선택 상태로 시작하며 직접 입력을 무시한다. |
| 같은 행의 `48 x 32 RTL` | 마지막 아이콘을 선택한다. | 요청 아이콘 크기 48×32와 RTL 레이아웃이 적용된 상태로 동작한다. 같은 행의 disabled preselected가 그룹 변경으로 해제되는 것은 직접 입력과 구분한다. |
| `Clear both groups` | Auto·Named를 선택한 뒤 누른다. | 이 두 그룹만 프로그램으로 선택 해제되고 상태 줄은 `Parent and named groups cleared programmatically`가 된다. 애니메이션 행과 상태 행은 이 버튼의 대상이 아니다. |

## 테마와 접근성 수동 확인

선택 애니메이션 도중과 정지 후 시스템 테마를 바꾸고 최종 선택 표시·색상이 현재
상태를 따르는지 확인한다. 테마 변경 수단이 없는 환경이면 해당 항목은 미실행으로 기록한다.

스크린 리더가 있는 기기에서 각 아이콘으로 탐색하고 RADIO_BUTTON 역할, 선택 상태,
소스의 `SetAccessibilityName()`에 지정된 이름(Auto A, Named A 등)을 확인한다.
이 이름은 아이콘 아래에 시각적 라벨로 표시되는 것이 아니다. 읽기 전용 행의 이름은
앱이 지정한 이름이며, 별도 read-only 접근성 상태 설정을 증명하지 않는다.
시각 관찰과 스크린 리더 결과는 별도로 기록한다.

## 통과 기준과 기록

단일 선택·재클릭 유지·그룹 독립성·프로그램 해제·입력 무시·애니메이션 전환이 위 표와
일치해야 한다. `< Back` 또는 하드웨어 Back 후 재진입도 확인한다. Escape는 앱 종료다.
기기·빌드·테마·배율·스크린 리더 이름/버전과 항목별 통과/실패/미실행을 기록한다.
이 문서는 실행 증거가 아니며 standalone radio나 텍스트 행 합성은
[별도 샘플](../../../samples/radio-button/radio-button-example.cpp)에서 다룬다.
