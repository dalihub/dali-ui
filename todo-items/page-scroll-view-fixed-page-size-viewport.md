# PageScrollView 고정 page size의 viewport 변경 후 페이지 갱신

## 상태와 범위

- 기록일: 2026-10-14
- 상태: TODO / UI 통합 재현 및 수정 검토 필요
- 작업 타입: 리뷰 후속, 코드 수정 검토, 테스트 수정 검토
- 대상: `dali-ui-foundation/integration-api/page-scroll-view-impl.{cpp,h}`,
  `scroll-view-impl.cpp`, `public-api/layouts/scroll-view-layout-manager.cpp` 및
  PageScrollView 자동화 테스트
- 출처: [PR #886](https://github.sec.samsung.net/NUI/dali-ui/pull/886) 리뷰 후속
- 이 문서는 후속 작업 기록이며 즉시 코드 구현 지시가 아니다.

## 문제와 배경

`SetPageSize()`로 고정 page size를 지정하면 viewport 크기가 바뀌어도 content 길이,
page 길이와 page count는 그대로일 수 있다. 그러나 viewport가 커지면 최대 scroll
offset이 줄어 현재 위치가 clamp될 수 있으며, 이 경우 현재 페이지도 다시 계산해야 한다.

PR #886의 head `27192545955e45069268de74307fd96eb966a1d2`에서는
`RefreshViewport()`가 위치를 clamp한 뒤 `OnScrollableAreaChanged()`를 호출한다.
하지만 page geometry는 content 길이, page 길이와 방향만 비교하므로 고정 page size에서
viewport만 바뀌면 geometry와 count 모두 변경되지 않은 것으로 판단한다. 그 결과
`GetCurrentPage()`와 `PageChangedSignal`이 실제 위치를 반영하지 못할 수 있다.

Base `c4f75b20740194a1626f77bbffe340b3e05ff369`도 count가 같으면 페이지 재계산을
생략하는 기존 문제가 있다. 따라서 이 항목은 PR #886으로 새로 발생한 회귀로 분류하지
않고 별도 후속 작업으로 기록한다.

## 재현 및 기대 결과

1. Horizontal pager의 content 길이를 500, viewport 길이를 100으로 설정한다.
2. `SetPageSize(Vector2(100, 80))`로 고정 page size를 지정한다.
3. `ScrollToPage(4, false)`로 page 4, offset 400에 정착시킨다.
4. Viewport 길이만 350으로 늘리고 layout을 진행한다.
5. 최대 offset은 150이 된다. 위치가 150으로 clamp된 후에도 page count는 5이다.
6. 기존 위치 기반 계산 `round(offset / pageLength)`에 따르면 현재 페이지는 2여야 한다.
   `GetCurrentPage()`가 4에 머무르거나 페이지 변경 신호가 누락되는지 확인한다.

## 제안 방향과 미결 사항

- Viewport 변경 또는 clamp에 따른 위치 변경을 페이지 재계산 조건에 포함하는 방안 검토
- 고정 page size에서도 실제 위치, 현재 페이지와 페이지 변경 신호의 일관성 유지
- Animation 진행 중에는 snap target을 사용하는 기존 계약과 정착 상태의 재계산 조건 구분
- Partial last page의 명시적 선택 및 NotifyPages*로 보존한 선택 처리와의 충돌 검토
- PR #886 병합 여부와 후속 수정에 따라 당시 코드에서 원인과 수정 범위 재확인
- Public handle의 크기나 vtable 변경 없이 구현 상태와 내부 계산으로 해결하는 방향 검토

## 검증 상태와 후속 검증

현재 확인은 base/head 코드 흐름 분석에 한정된다. 로컬 필수 헤더 누락으로 실제 UI
통합 재현은 실행하지 못했으므로 후속 작업에서 먼저 위 시나리오를 실행해야 한다.

1. Horizontal/vertical 양 축에서 고정 page size와 viewport 확대 후 clamp 확인
2. Page count가 동일한 경우의 현재 페이지 변경 및 정확히 한 번의 신호 발생 확인
3. Clamp가 발생하지 않는 resize와 반복 layout에서 불필요한 신호가 없는지 확인
4. Viewport 기반 기본 page size, partial last page, snap 진행 중 resize 및 NotifyPages*
   선택 보존에 대한 기존 테스트 유지
