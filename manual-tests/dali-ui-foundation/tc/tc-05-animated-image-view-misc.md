# 05. AnimatedImageView: Misc APIs — 축소·샘플링 품질과 플래그 왕복

PreMultipliedAlpha, DesiredSize, PlaceholderUrl, ImageLoadWithViewSize, SamplingMode 동작을 확인한다. 핵심은 **디코딩 설정(DesiredSize / SamplingMode / LoadWithViewSize)이 실제 렌더 품질에 반영되는가**이다.

## 화면 구성

- 중앙: 애니메이션 프리뷰 (200x200, 빨간색 배경)
- 상태 라벨 1: DesiredSize / Sampling (버튼 핸들러에서 동기 갱신)
- 상태 라벨 2: PreMult / LoadWithViewSize / Placeholder (Placeholder는 GetPlaceholderUrl 반환 경로를 그대로 출력)
- 버튼 행 1: Play / Stop
- 버튼 행 2: Desired 0x0 / Desired 50x50 / Desired 200x200
- 버튼 행 3: Sampling BOX / Sampling NEAREST / Sampling LINEAR
- 버튼 행 4: PreMult ON / PreMult OFF / LoadWithViewSize ON / LoadWithViewSize OFF
- 버튼 행 5: Set Placeholder / Clear Placeholder / Reload URL

> **디코딩 설정이 반영되는 방식**: 값을 바꾸는 setter는 비주얼을 다시 만들고, 그때 현재
> 설정으로 **다시 디코딩**된다(프레임워크 동작). 리빌드 후 재생은 멈추고 첫 프레임이 정지
> 상태로 표시되므로, 바로 그 정지 화면끼리 픽셀로 비교하면 된다. [Reload URL]은 캐시를
> 재사용하지 않는 명시적 리로드(Reload())를 수행한 뒤 재생을 재개한다.

## 테스트 1: DesiredSize

1. [Desired 50x50] 버튼을 탭한다 (재생이 멈추고 첫 프레임이 표시됨)
2. **기대 결과**: 50x50으로 디코딩되어 **눈에 띄게 흐리게** 표시됨
3. [Desired 0x0] 버튼을 탭한다
4. **기대 결과**: 원본 해상도로 복원 — 50x50 정지 화면과 그림이 달라야 한다

## 테스트 2: SamplingMode

1. [Desired 50x50] 상태에서 [Sampling NEAREST]를 탭한다
2. [Sampling LINEAR]를 탭한다
3. **기대 결과**: 축소 디코딩 상태에서 NEAREST와 LINEAR의 정지 화면이 서로 달라야 한다
   (원본 크기에서는 차이가 나지 않으므로 순서가 중요하다)

## 테스트 3: ImageLoadWithViewSize

1. [Desired 0x0] 상태에서 [LoadWithViewSize ON]을 탭한다
2. **기대 결과**: 뷰 크기(200x200)로 디코딩됨 — 라벨의 LoadWithViewSize가 ON
3. [LoadWithViewSize OFF]를 탭한다
4. **기대 결과**: 원본 크기(256x256)로 디코딩됨 — 라벨이 OFF

## 테스트 4: PreMultipliedAlpha

1. [PreMult ON] / [PreMult OFF]를 차례로 탭한다
2. **기대 결과**: IsPreMultipliedAlpha 반환값이 라벨에 ON/OFF로 정확히 표시됨

> 알파를 미리 곱해 저장하는 방식이라, 렌더러가 양쪽을 제대로 처리하면 최종 합성 결과가
> 같게 나오는 것이 정상이다. 시각 차이는 기준이 아니고 getter 왕복까지가 기준이다.

## 테스트 5: Placeholder

1. [Set Placeholder] 버튼을 탭한다
2. **기대 결과**: 라벨의 Placeholder에 설정한 경로(placeholder_image.png)가 표시됨
3. [Clear Placeholder] 버튼을 탭한다
4. **기대 결과**: GetPlaceholderUrl이 빈 문자열 반환 — 라벨에 none 표시

> 로딩 중 플레이스홀더가 잠깐 보이는 것은 로컬 리소스에서는 사람 눈으로도 관측할 수 없어
> 기준이 아니다. 이 TC의 기준은 GetPlaceholderUrl 왕복이다.

## 통과 기준

- 모든 Set 호출 후 Get/Is 반환값이 라벨에 올바르게 표시되어야 한다
- DesiredSize 50x50의 정지 화면이 0x0의 정지 화면과 달라야 한다 (저해상도 디코딩)
- 50x50 상태에서 Sampling NEAREST와 LINEAR의 정지 화면이 서로 달라야 한다
