# ImageView: SamplingMode / DesiredSize

SamplingMode별 화질 차이와 DesiredSize / ImageLoadWithViewSize 동작을 확인한다.

## 화면 구성

- 중앙: 이미지 프리뷰 (240x240, 작은 이미지를 확대하여 SamplingMode 차이 가시화)
- 상태 라벨 1: SamplingMode 및 GetSamplingMode 일치 여부
- 상태 라벨 2: DesiredSize / LoadWithViewSize 상태
- 버튼 행 1: BOX / NEAREST / LINEAR / BOX_LIN
- 버튼 행 2: Desired 0x0 / Desired 50x50 / Desired 150x150
- 버튼 행 3: LoadWithViewSize ON / LoadWithViewSize OFF

## 테스트 1: SamplingMode 변경

1. [NEAREST] 버튼을 탭한다
2. **기대 결과**: 픽셀이 선명하게 보임 (계단 현상)
3. [LINEAR] 버튼을 탭한다
4. **기대 결과**: 부드럽게 블러 처리됨
5. [BOX] 버튼을 탭한다
6. **기대 결과**: BOX 필터링 적용, LINEAR와 유사하나 미세한 차이
7. GetSamplingMode 일치 여부가 "OK"로 표시되는지 확인

## 테스트 2: DesiredSize 제한

1. [Desired 50x50] 버튼을 탭한다
2. **기대 결과**: 저해상도로 로딩되어 흐리게 표시됨
3. [Desired 0x0] 버튼을 탭한다
4. **기대 결과**: 원본 해상도로 로딩됨

## 테스트 3: ImageLoadWithViewSize

1. [LoadWithViewSize ON] 버튼을 탭한다
2. **기대 결과**: 뷰 크기(240x240)에 맞춰 로딩됨

## 통과 기준

- NEAREST 모드에서 픽셀 블록이 선명하게 보여야 한다
- DesiredSize 제한 시 저해상도로 로딩되어야 한다
- GetSamplingMode / GetDesiredWidth / GetDesiredHeight 반환값이 설정한 값과 일치해야 한다