# Lottie: SetDynamicProperty (FILL_COLOR)

SetDynamicProperty로 프레임 단위 동적 색상 변경을 확인한다. 워커 스레드 콜백을 통해 레이어 색상을 실시간으로 변경한다.

## 화면 구성

- 중앙: Lottie 애니메이션 프리뷰 (240x240)
- 상태 라벨: 현재 Fill 색상 정보
- 버튼 행 1: Play / Pause
- 버튼 행 2: Fill: Red / Fill: Green / Fill: Blue
- 버튼 행 3: Clear Dynamic (reload)

## 테스트 1: DynamicProperty 색상 변경

1. [Play] 버튼을 탭한다
2. **기대 결과**: 원래 색상으로 재생됨
3. [Fill: Red] 버튼을 탭한다
4. **기대 결과**: 모든 레이어(keyPath="**")가 빨간색으로 변경됨
5. [Fill: Blue] 버튼을 탭한다
6. **기대 결과**: 파란색으로 변경됨
7. [Fill: Green] 버튼을 탭한다
8. **기대 결과**: 초록색으로 변경됨

> **재생 중 판정 방법**: 프리뷰 영역의 **평균 채널값(R/G/B)** 으로 판정한다. 색 덮어쓰기의
> 신호가 프레임 차이를 압도한다 — 실측(2026-08-26, aiv-260825): 원본 163/167/141 →
> Red 207/53/37 → Blue 22/54/221. 어느 프레임이 표시 중이든 지배 채널이 갈린다.
>
> **일시정지 한계 (알려진 격차)**: 일시정지 상태에서 Fill을 눌러도 현재 프레임은 다시
> 그려지지 않는다. dali-ui 쪽은 강제 1프레임 렌더 신호를 걸지만(RESEND_DYNAMIC_PROPERTY),
> thorvg 네이티브 렌더러가 같은 프레임 번호의 재평가를 건너뛰어 화면에 닿지 않는다 —
> dali-adaptor(vector-animation-renderer-native)에 수정 요청이 필요한 영역이다. 그래서
> 이 TC의 픽셀 판정은 재생 중 평균 채널값으로 한다.

## 테스트 2: DynamicProperty 해제

1. [Clear Dynamic] 버튼을 탭한다
2. **기대 결과**: 같은 URL로 SetResourceUrl을 다시 불러 컴포지션이 리로드되고, 동적 속성이
   초기화되어 원래 색상이 복원됨 (같은 URL 재설정 = 명시적 리로드 요청 — rlottie /
   lottie-android / lottie-ios 모두 동적 속성 제거 API가 없어 리로드가 표준 해제 경로다)

## 통과 기준

- Fill 버튼 클릭 직후 재생 중 애니메이션 색상이 변경되어야 한다 (프리뷰 평균 채널값의
  지배 채널 전환으로 판정)
- Clear 후 원래 색상이 복원되어야 한다 (평균 채널값이 원본 수준으로 복귀)
