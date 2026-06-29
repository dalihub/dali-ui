# Lottie: RenderScale / FrameCache / RedrawOnScale

SetRenderScale / GetRenderScale, SetFrameCacheEnabled / IsFrameCacheEnabled, SetRedrawOnScaleDown / IsRedrawOnScaleDown, SetRedrawOnScaleUp / IsRedrawOnScaleUp, SetNotifyAfterRasterization / IsNotifyAfterRasterizationEnabled 동작을 확인한다.

## 화면 구성

- 중앙: Lottie 애니메이션 프리뷰 (240x240)
- 상태 라벨: RenderScale / FrameCache / RedrawSD / RedrawSU / NotifyAfterRaster 정보
- 버튼 행 1: Scale 0.25x / Scale 1.0x / Scale 2.0x
- 버튼 행 2: FrameCache ON / FrameCache OFF
- 버튼 행 3: RedrawSD ON / RedrawSD OFF
- 버튼 행 4: RedrawSU ON / RedrawSU OFF
- 버튼 행 5: NotifyAfterRaster ON / NotifyAfterRaster OFF

## 테스트 1: RenderScale

1. [Scale 0.25x] 버튼을 탭한다
2. **기대 결과**: 저해상도 렌더링 (흐림/픽셀화 확인)
3. [Scale 1.0x] 버튼을 탭한다
4. **기대 결과**: 일반 해상도
5. [Scale 2.0x] 버튼을 탭한다
6. **기대 결과**: 고해상도 (선명하지만 메모리 사용량 증가)
7. GetRenderScale 반환값이 라벨에 표시되는지 확인

## 테스트 2: FrameCache

1. [FrameCache ON] 버튼을 탭한다
2. **기대 결과**: 동일 크기 재사용 시 성능 개선
3. [FrameCache OFF] 버튼을 탭한다
4. **기대 결과**: 매 프레임 재랜더링

## 테스트 3: RedrawOnScale

1. [RedrawSD OFF] 버튼을 탭한다
2. **기대 결과**: 축소 시 재랜더링 없이 기존 텍스처 스케일링
3. [RedrawSD ON] 버튼을 탭한다
4. **기대 결과**: 축소 시 재랜더링

## 통과 기준

- RenderScale 0.25x: 이미지가 흐리게/픽셀화되어 보여야 한다
- RenderScale 2.0x: 선명해야 하지만 메모리 사용량이 증가해야 한다
- 각 Set 호출 후 Is.../Get... 반환값이 라벨에 올바르게 표시되어야 한다