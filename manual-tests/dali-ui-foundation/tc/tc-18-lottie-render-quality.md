# 18. Lottie: RenderScale / FrameCache / RedrawOnScale

SetRenderScale / GetRenderScale, SetFrameCacheEnabled / IsFrameCacheEnabled,
SetRedrawOnScaleDown / IsRedrawOnScaleDown, SetRedrawOnScaleUp / IsRedrawOnScaleUp,
SetNotifyAfterRasterization / IsNotifyAfterRasterizationEnabled 동작을 확인한다.

라벨 다섯 필드는 전부 진짜 getter다 — 앱이 자기 멤버를 되돌려주는 게 아니라 컴포넌트에
물어본다. 다만 다섯 설정 중 **바깥에서 렌더 결과로 관측되는 것은 RenderScale 하나**다.
나머지 넷(FrameCache, RedrawSD/SU, NotifyAfterRasterization)은 내부 최적화 플래그라
값 왕복까지가 기능 테스트의 범위다 — 아래 "참고" 참조.

## 화면 구성

- 중앙: Lottie 애니메이션 프리뷰 (240x240)
- 상태 라벨: RenderScale / FrameCache / RedrawSD / RedrawSU / NotifyAfterRaster
- 재생 라벨: `State: PLAYING|PAUSED | Frame: n/N` (GetPlayState/GetCurrentFrame/GetTotalFrame)
- 재생 버튼: Play / Pause / Jump →10
- 버튼 행 1: Scale 0.25x / Scale 1.0x / Scale 2.0x
- 버튼 행 2: FrameCache ON / FrameCache OFF
- 버튼 행 3: RedrawSD ON / RedrawSD OFF
- 버튼 행 4: RedrawSU ON / RedrawSU OFF
- 버튼 행 5: NotifyAfterRaster ON / NotifyAfterRaster OFF

## 테스트 1: RenderScale (값 왕복 + 픽셀)

1. [Pause] → [Jump →10]을 탭해 `State: PAUSED | Frame: 10/N`을 확인한다
2. [Scale 0.25x] 버튼을 탭한다
3. **기대 결과**: 라벨 `RenderScale: 0.250000`, State/Frame이 PAUSED/10으로
   보존된 프리뷰를 캡처한다
4. [Scale 2.0x] 버튼을 탭하고 State/Frame이 그대로임을 확인한 뒤 캡처한다
5. **기대 결과**: 라벨 `RenderScale: 2.000000`, 두 캡처가 **다르다** — 실제로 다른
   해상도로 구웠다는 증거 ("더 흐리다/선명하다"는 사람의 판단이라 기계 판정은 차이까지)

RenderScale은 런타임 visual 속성이므로 변경해도 visual을 재생성하거나 재생
상태·현재 프레임을 초기화하지 않아야 한다. 정지 프레임은 setter의 부수 효과로
만들지 않고 테스트가 Pause/Jump로 명시적으로 고정한다.

## 테스트 2: 플래그 왕복 (누적)

1. [FrameCache ON] → 라벨 `FrameCache: ON`
2. [RedrawSD OFF] → `RedrawSD: OFF`
3. [RedrawSU ON] → `RedrawSU: ON`
4. [NotifyAfterRaster ON] → `NotifyRast: ON`
5. **기대 결과**: 마지막 라벨에 앞의 뒤집기들이 **전부 그대로 남아** 있어야 한다 —
   이웃 플래그를 덮어쓰는 setter가 여기서 걸린다

## 참고 (기능 테스트 대상이 아님)

- FrameCache의 "동일 크기 재사용 시 성능 개선", RedrawSD OFF의 "재렌더링 없이 텍스처
  스케일링"은 래스터화 횟수·프레임 타이밍을 보고하는 관측 지점이 없어 이 화면에서 판정
  불가다 — 프로파일러/트레이싱 사안.
- RenderScale 2.0x의 "메모리 사용량 증가"도 화면·트리에 수치가 없어 같은 부류다.
- `SetNotifyAfterRasterization`은 **렌더 스레드에게** 알리는 내부 플래그라 앱이 받을 수
  있는 신호가 아니다 — 관측 수단이 아니다.

## 통과 기준

- 각 Set 호출 후 Is.../Get... 반환값이 라벨에 올바르게, 누적으로 표시되어야 한다
- RenderScale 변경 후에도 PAUSED 상태와 현재 프레임이 보존되어야 한다
- 명시적으로 고정한 같은 프레임에서 RenderScale 0.25x와 2.0x 캡처가 서로 달라야 한다
