# 03. AnimatedImageView: FrameSpeedFactor / FrameDelay

SetFrameSpeedFactor / GetFrameSpeedFactor, SetFrameDelay / GetFrameDelay 동작을 확인한다.

## 화면 구성

- 중앙: 애니메이션 프리뷰 (200x200, 진입 시 dog-anim.webp)
- 상태 라벨: Speed / Delay / Frame 정보 (100ms 폴링)
- 버튼 행 1: Play / Stop / URLs (dog 8)
- 버튼 행 2: Speed 0.25x / 0.5x / 1.0x / 2.0x / 4.0x
- 버튼 행 3: Delay 0ms / 100ms / 300ms / 500ms

> **참고 1**: 값을 바꾸는 setter는 비주얼을 다시 만들며 재생을 멈춘다(프레임워크 동작).
> setter를 누른 뒤 [Play]로 재개한다.
>
> **참고 2 — FrameDelay는 URL 배열 소스에 적용된다.** 인코딩된 애니메이션 파일(webp/gif)은
> 파일에 든 프레임 간격을 쓰고, FrameDelay는 URL 배열 캐시에만 배선돼 있다(실측 2026-08-26:
> webp에서 Delay 500ms가 렌더 간격을 바꾸지 않음). Delay를 확인하려면 [URLs (dog 8)]로
> 소스를 먼저 바꾼다.
>
> **참고 3**: 높은 배속(예: 4.0x)은 요구 간격(25ms)이 프레임 디코드 속도보다 짧아지면
> 디코드가 병목이 되어 화면상 속도가 그 배속에 도달하지 못할 수 있다. 시각 비교는
> 0.25x vs 1.0x처럼 디코드 여유가 있는 쌍으로 한다.

## 테스트 1: FrameSpeedFactor

1. [Play] 버튼을 탭하여 기본 속도(1.0x)를 확인한다
2. [Speed 0.25x] 버튼을 탭한 뒤 [Play]를 탭한다
3. **기대 결과**: 매우 느린 재생 — 같은 시간 동안 Frame 카운터 전진이 1.0x의 약 1/4
4. [Speed 1.0x] 버튼을 탭한 뒤 [Play]를 탭한다
5. **기대 결과**: 기본 속도 복귀 — 0.25x보다 뚜렷이 자주 전진
6. GetFrameSpeedFactor 반환값이 라벨에 표시되는지 확인 (0.25/0.5/1.0/2.0/4.0)

## 테스트 2: FrameDelay

1. [URLs (dog 8)]로 URL 배열 소스로 바꾼다
2. [Delay 500ms] 버튼을 탭한 뒤 [Play]를 탭한다
3. **기대 결과**: 각 프레임이 0.5초간 표시됨
4. [Delay 100ms] 버튼을 탭한 뒤 [Play]를 탭한다
5. **기대 결과**: 프레임 전환이 뚜렷이 빨라짐
6. GetFrameDelay 반환값이 라벨에 표시되는지 확인

## 통과 기준

- Speed: 같은 관측 구간에서 0.25x의 Frame 카운터 전진 빈도가 1.0x보다 뚜렷이 낮아야 한다
- Delay(URL 배열 소스): 같은 구간에서 500ms의 전진 빈도가 100ms보다 뚜렷이 낮아야 한다
- GetFrameSpeedFactor / GetFrameDelay 반환값이 설정한 값과 일치해야 한다
