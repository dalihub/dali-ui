# Focus Group Containment

FocusGroup 설정 시 기본 FocusFinder 기반 방향키 이동이 그룹 내부로 제한되는지 확인한다.

## 화면 구성

- 상단: 상태 라벨
- "Set FocusGroup OFF" 버튼
- "Set FocusGroup ON" 버튼
- Group: [Inside A] / [Inside B] (세로 배치, 기본 파란색 `#D5DBE8`)
- Outside: 그룹 밖 view (기본 분홍색 `#E8D5E0`)
- 포커스된 view는 주황색(`#FF9800`)으로 표시됨

## 테스트 1: FocusGroup OFF — 그룹 밖으로 이동

1. "Set FocusGroup OFF" 버튼을 탭한다 (FocusGroup: OFF 표시)
2. Inside B에 포커스를 둔다
3. ↓ 키를 누른다
4. **기대 결과**: Outside로 포커스 이동하며 Outside가 주황색(`#FF9800`)으로 변경됨

## 테스트 2: FocusGroup ON — 그룹 안에 갇힘

1. "Set FocusGroup ON" 버튼을 탭한다 (FocusGroup: ON 표시)
2. Inside B에 포커스를 둔다
3. ↓ 키를 누른다
4. **기대 결과**: 포커스가 Outside로 이동하지 않음. Inside B가 주황색(`#FF9800`)을 유지하고 Outside는 기본 분홍색(`#E8D5E0`)을 유지

## 테스트 3: 그룹 내 이동은 정상

1. "Set FocusGroup ON" 버튼을 탭한다 (FocusGroup: ON 표시)
2. Inside A에 포커스를 둔다
3. ↓ 키를 누른다
4. **기대 결과**: Inside B로 포커스 이동하며 Inside B가 주황색(`#FF9800`)으로 변경됨
5. ↑ 키를 누른다
6. **기대 결과**: Inside A로 포커스 이동하며 Inside A가 주황색(`#FF9800`)으로 변경됨

## 테스트 4: FocusGroup OFF 후 다시 이동 가능

1. "Set FocusGroup OFF" 버튼을 탭한다 (FocusGroup: OFF 표시)
2. Inside B에서 ↓ 키를 누른다
3. **기대 결과**: Outside로 포커스 이동하며 Outside가 주황색(`#FF9800`)으로 변경됨

## 통과 기준

- FocusGroup ON: 기본 방향키 탐색으로 그룹 밖 이동 불가
- FocusGroup ON: 그룹 내 이동은 정상
- FocusGroup OFF: 그룹 밖 이동 가능
