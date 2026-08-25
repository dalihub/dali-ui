# Dali UI 접근성 Role과 Screen Reader 발화

이 문서는 현재 Dali UI 및 screen-reader 소스에 구현된 동작을 정리한다.
`SCREEN_READER_TV` 빌드와 영어 `en_US` PO 카탈로그를 기준으로 한다.

[English version](Accessibility-Role-Screen-Reader-Announcements.md)

## 범위와 용어

- **포커스 발화**: 접근성 하이라이트가 객체로 이동할 때 조합하는 설명이다.
- **이름(Name)**: `LABELLED_BY` 관계 이름, accessible name, text-interface name
  순으로 선택한다. `reading_info_type`에 `name`이 있어야 발화한다.
- **Role**: `reading_info_type`에 `role`이 있을 때만 발화한다.
- **State**: `reading_info_type`에 `state`가 있을 때만 발화한다.
- `<ATSPI localized role>`: screen-reader PO 문자열이 아니라 AT-SPI가 영어
  locale에서 반환하는 localized role 이름이다.

Dali UI의 role 변환은
`dali-ui-foundation/integration-api/view-accessible.cpp`에, screen-reader의
발화 조립과 상태 이벤트 처리는 각각 `src/reading_composer.c` 및
`src/app_tracker.c`에 있다.

## TV 포커스 발화 순서

각 요소 사이에는 짧은 구분 pause가 들어간다(구현상 `" , "`). 없는 요소는
생략한다.

| Role 그룹 | 포커스 발화 순서 | 한국어 의미 |
| --- | --- | --- |
| `ENTRY`, `PASSWORD_TEXT` | **Label/name → role → current text content** | **라벨/이름 → role → 현재 입력 내용**. 비밀번호는 문자 자체가 아닌 글자 수만 읽는다. TV 전용 순서에서는 `Editing` state를 붙이지 않는다. |
| `PROGRESS_BAR` | **Role → value/state** | **role → 값/상태**. 현재 TV composer는 accessible name을 붙이지 않는다. |
| `TOGGLE_BUTTON`, `SWITCH`, `RADIO_BUTTON`, `CHECK_BOX`, `SLIDER`, `SPIN_BUTTON` | **Name → role → state/value** | **이름 → role → 상태/값** |
| 그 외 모든 role | **(State) → name → role** | **(상태) → 이름 → role**. 해당 role이 state trait를 만들 때만 상태가 발화된다. |
| `described_by_accessible`의 role/state trait가 있는 객체 | 해당 relation 객체의 trait로 대체 | 최대 한 단계까지만 relation 객체의 role/state를 사용한다. |

TV에서는 extended description이 primary trait 뒤에 붙고,
`ATSPI_STATE_ENABLED`가 없으면 `disabled`가 항상 마지막에 발화된다.

## Role 매핑과 영어 발화

모든 `Dali::Ui::Accessibility::Role`을 나열했다. 꺾쇠괄호 표기는 객체나
AT-SPI가 제공하는 값이며, `—`는 해당 role 자체가 별도 role 문자열을 더하지
않는다는 뜻이다.

| Dali UI role | Dali UI가 보내는 AT-SPI role | 영어 role 발화 (한국어 의미) | 포커스 시 state/value | role별 상태 변경 발화 |
| --- | --- | --- | --- | --- |
| `ADJUSTABLE` | `SLIDER` | `Slider` (슬라이더) | 현재 값 텍스트/퍼센트/숫자 | 별도 규칙 없음; 공통 이벤트 참조. |
| `ALERT` | `ALERT` | `<ATSPI localized role>` (경고) | — | 별도 규칙 없음. |
| `BUTTON` | `PUSH_BUTTON` | `Button` (버튼) | expandable이면 `Expanded`/`Collapsed` | `expanded` → `Expanded` (펼침) / `Collapsed` (접힘). |
| `CHECK_BOX` | `CHECK_BOX` | `Check box` (체크 상자) | `Checked` (선택됨) / `Not checked` (선택 안 됨) | `checked` → `Checked` / `Not checked`. |
| `COMBO_BOX` | `COMBO_BOX` | `Dropdown button` (드롭다운 버튼) | — | 별도 규칙 없음. |
| `CONTAINER` | `FILLER` | `<ATSPI localized role>` (컨테이너/채움 영역) | — | 별도 규칙 없음. |
| `DIALOG` | `DIALOG` | 보통 role 발화 없음; `sub-role=Alert`면 `Alert` (경고) prefix | 표시 item 수가 description으로 추가될 수 있음 | 공통 `expanded` 규칙 적용 가능. |
| `ENTRY` | `ENTRY` | `Edit box` (입력 상자) | 현재 입력 내용. TV에서는 `Editing`을 붙이지 않음 | keyboard feedback 활성화 시 텍스트 삽입/삭제를 별도 발화. |
| `HEADER` | `HEADING` | `Heading` (제목) | tree table 선택/개수 문맥이 있으면 추가 | 공통 `expanded` 규칙 적용 가능. |
| `IMAGE` | `IMAGE` | TV: `Image` (이미지) | — | 별도 규칙 없음. |
| `LINK` | `LINK` | `<ATSPI localized role>` (링크) | — | 공통 `expanded` 규칙 적용 가능. non-TV에는 활성화 안내가 추가됨. |
| `LIST` | `LIST` | `<ATSPI localized role>` (목록) | — | 별도 규칙 없음. |
| `LIST_ITEM` | `LIST_ITEM` | 보통 role 발화 없음; parent가 animated이면 `Reorder, Button` (재정렬, 버튼) | tree table이면 `Selected`; scroll bar 하위면 선택/페이지 위치 | tree table의 `selected`만 선택 문맥과 함께 발화. `animated`는 drag 시작/놓은 위치를 발화. |
| `MENU` | `MENU` | `<ATSPI localized role>` (메뉴) | — | 별도 규칙 없음. |
| `MENU_BAR` | `MENU_BAR` | `<ATSPI localized role>` (메뉴 모음) | — | 별도 규칙 없음. |
| `MENU_ITEM` | `MENU_ITEM` | `<ATSPI localized role>` (메뉴 항목) | 선택 시 `Selected` (선택됨) | parent가 `MENU`가 아닐 때만 이름, 탭 위치, `Selected`를 발화. |
| `NONE` | `UNKNOWN` | — | — | 별도 규칙 없음. |
| `NOTIFICATION` | `NOTIFICATION` | `<ATSPI localized role>` (알림) | — | 별도 규칙 없음. |
| `PASSWORD_TEXT` | `PASSWORD_TEXT` | `Edit box` (입력 상자) | 문자 수만 발화하며 비밀번호 원문은 발화하지 않음 | `ENTRY` 참조. password는 keyboard feedback 대상에서 제외. |
| `POPUP_MENU` | `POPUP_MENU` | 보통 role 발화 없음; `sub-role=Alert`면 `Alert` prefix | TV에서 parent가 combo box가 아니면 item 수 추가 | 공통 `expanded` 규칙 적용 가능. |
| `PROGRESS_BAR` | `PROGRESS_BAR` | `progress bar` (진행 표시줄) | 값 텍스트/퍼센트/숫자. 숫자이고 100% 미만이면 `In progress` (진행 중)도 추가 | `accessible-value`가 값 발화 경로를 호출. |
| `RADIO_BUTTON` | `RADIO_BUTTON` | `Radio button` (라디오 버튼) | checked면 `Selected`, 아니면 `Not selected` (선택 안 됨). editable 또는 color chooser parent는 예외 | `checked`는 공통 `Selected` / `Not selected` 규칙 사용. |
| `SCROLL_BAR` | `SCROLL_BAR` | 스타일에 따라 alphabetical index bar (알파벳 인덱스 바), page `n of m`, 또는 — | style별 페이지/인덱스 문자열은 role trait에 포함 | 별도 규칙 없음. |
| `SPIN_BUTTON` | `SPIN_BUTTON` | `<ATSPI localized role>` (스핀 버튼) | 현재 값 텍스트/퍼센트/숫자 | 별도 규칙 없음; 공통 이벤트 참조. |
| `TAB` | `PAGE_TAB` | `Tab` (탭) | parent가 `PAGE_TAB_LIST`이고 legacy Elm이 아니면 선택 시 `Selected` | `selected=true` → 이름 뒤 `Selected`. |
| `TAB_LIST` | `PAGE_TAB_LIST` | TV: `Tab` (탭) | —; 필요한 항목 수는 component/item description이 제공 | 별도 규칙 없음. |
| `TEXT` | `TEXT` | `Text box` (텍스트 상자) | — | 별도 role 규칙 없음. keyboard feedback 활성화 시 텍스트 삽입/삭제 발화. |
| `TOGGLE_BUTTON` | `TOGGLE_BUTTON` | TV: `Toggle button` (토글 버튼), non-TV: `Switch` (스위치) | `On` (켜짐) / `Off` (꺼짐) | `checked` → `On` / `Off`. |
| `TOOL_BAR` | `TOOL_BAR` | `<ATSPI localized role>` (도구 모음) | — | 별도 규칙 없음. |
| `SCENE_3D` | `FILLER` | `<ATSPI localized role>` (3D 장면/채움 영역) | — | 별도 규칙 없음. |
| `MODEL` | `IMAGE` | `<ATSPI localized role>` (모델/이미지) | — | 별도 규칙 없음. |
| `SWITCH` | `SWITCH` | TV/non-TV 모두 `Switch` (스위치) | `On` (켜짐) / `Off` (꺼짐) | `checked` → `On` / `Off`. |
| `LABEL` | `LABEL` | — | — | 별도 규칙 없음. |
| `SCROLL_PANE` | `SCROLL_PANE` | `<ATSPI localized role>` (스크롤 패널) | — | 별도 규칙 없음. |
| `TABLE` | `TABLE` | TV: `Grid` (격자) | — | 별도 규칙 없음. |
| `TABLE_CELL` | `TABLE_CELL` | `<ATSPI localized role>` (표 셀) | — | 별도 규칙 없음. |
| `TABLE_COLUMN_HEADER` | `TABLE_COLUMN_HEADER` | `<ATSPI localized role>` (표 열 머리글) | — | 별도 규칙 없음. |
| `TABLE_ROW_HEADER` | `TABLE_ROW_HEADER` | `<ATSPI localized role>` (표 행 머리글) | — | 별도 규칙 없음. |
| `EMBEDDED` | `EMBEDDED` | TV: `Screen` (화면) | — | 별도 규칙 없음. |

## Runtime state-change 발화

아래 이벤트는 `reading_info_type`에 `state`가 있을 때만 처리한다. 포커스
발화를 통째로 반복하지 않고, 변경된 상태만 발화한다.

| AT-SPI 이벤트 | Role / 조건 | 영어 발화 (한국어 의미) |
| --- | --- | --- |
| `state-changed:checked` | `TOGGLE_BUTTON`, `SWITCH` | set: `On` (켜짐), clear: `Off` (꺼짐) |
| `state-changed:checked` | `CHECK_BOX` | set: `Checked` (선택됨), clear: `Not checked` (선택 안 됨) |
| `state-changed:checked` | `RADIO_BUTTON`을 포함한 그 외 모든 role | set: `Selected` (선택됨), clear: `Not selected` (선택 안 됨) |
| `state-changed:selected` | parent가 `MENU`가 아닌 `MENU_ITEM` | set 시 `<name>, <tab index> of <count>, Selected` (이름, 탭 위치, 선택됨) |
| `state-changed:selected` | parent가 `TREE_TABLE`인 `LIST_ITEM` | set 시에만 이름, 이미지/선택 문맥, 선택 개수를 발화 |
| `state-changed:selected` | `PAGE_TAB` | set 시에만 `<name>, Selected` (이름, 선택됨) |
| `state-changed:expanded` | 이 이벤트를 보내는 모든 role | set: `Expanded` (펼침), clear: `Collapsed` (접힘) |
| `property-change:accessible-value` | 모든 role | 값 읽기 경로를 호출. `PROGRESS_BAR`, `SLIDER`, `SPIN_BUTTON`은 위 표의 값 trait를 가짐. |
| `property-change:accessible-name` | highlight 상태이며 name 읽기가 켜진 객체 | 변경된 accessible name을 발화 |

## 구현상 유의점

1. `LABEL`, `DIALOG`, `POPUP_MENU` 및 일부 구조 role은
   `generate_role_trait_from_role()`에서 직접 role 문자열을 더하지 않는다.
2. `TEXT`의 `Text box`는 TV 전용 `COM_SID_PRINCIPLE_CFJUL_TEXT_BOX`에서
   오며 non-TV에서는 role trait가 비어 있다.
3. `TOGGLE_BUTTON`과 `SWITCH`는 TV에서 role 명칭이 의도적으로 다르다.
   전자는 `Toggle button`, 후자는 항상 `Switch`다. 두 role 모두 focus와
   `checked` 변경 시 `On`/`Off`를 사용한다.
4. 이 문서는 현재 코드 동작의 목록이며 접근성 권장 사양 자체는 아니다.
   의미상 적절하지 않은 role도 generic `checked` 또는 `expanded` 이벤트를
   보내면 공통 처리에 따라 발화될 수 있다.
