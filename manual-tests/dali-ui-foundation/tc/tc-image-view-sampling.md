# ImageView: SamplingMode / DesiredSize

SamplingMode / DesiredSize / ImageLoadWithViewSize 동작을 확인한다.

**SamplingMode는 디코딩 시점의 리사이즈 필터다** — 리사이즈가 없으면 할 일이 없다
(`image-enumerations.h`: *the filter applied when scaling the image*; 화면 확대는 GPU 몫).
그래서 모드 비교는 **DesiredSize를 먼저 걸어** 디코드 리사이즈를 만든 뒤에만 성립하고,
`Desired 0x0`에서는 **네 모드가 모두 같은 그림**이어야 한다(대조군). 구 명세는 0x0에서
비교하라고 시켜 구조적으로 0픽셀이었다.

소재는 프리뷰(240x240)보다 큰 `gallery-large-3.jpg`(512x512)다 — DesiredSize와
LoadWithViewSize(큰 이미지를 뷰 크기로 줄여 읽어 메모리 절약)가 진짜 다운스케일이 되도록.

## 화면 구성

- 중앙: 이미지 프리뷰 (240x240) — 접근성 이름 `ImagePreview`
- 상태 라벨 1: SamplingMode 및 GetSamplingMode 일치 여부 (진짜 비교)
- 상태 라벨 2: DesiredSize / LoadWithViewSize (진짜 getter)
- 버튼 행 1: BOX / NEAREST / LINEAR / BOX_LIN
- 버튼 행 2: Desired 0x0 / Desired 50x50 / Desired 150x150
- 버튼 행 3: LoadWithViewSize ON / LoadWithViewSize OFF

## 테스트 1: SamplingMode — 리사이즈가 있어야 갈린다

1. **[Desired 50x50]을 먼저** 탭한다 (디코드 리사이즈 512→50을 만든다)
2. [BOX] → [NEAREST] → [BOX_LIN] 순서로 탭하며 각 렌더를 비교한다
3. **기대 결과**: 세 모드의 결과가 **서로 달라야** 한다 (실측 16~31% 차이)
4. [Desired 0x0]으로 되돌리고 네 모드를 다시 돈다
5. **기대 결과**: 0x0에서는 **네 모드가 모두 같아야** 한다 (대조군 — 실측 전 쌍 0px)
6. `GetSamplingMode match: OK` 확인

## 테스트 2: DesiredSize 제한

1. [Desired 50x50] / [Desired 150x150] / [Desired 0x0]을 차례로 탭한다
2. **기대 결과**: 세 상태의 렌더가 서로 다르고, 0x0으로 돌아오면 처음 프레임과
   **비트 동일** (실측 0px)

## 테스트 3: ImageLoadWithViewSize

1. [LoadWithViewSize ON] 버튼을 탭한다
2. **기대 결과**: OFF와 렌더가 달라짐 (뷰 크기 240 디코드 — 512 원본 대비)
3. **ON 상태에서 [NEAREST]를 탭한다**
4. **기대 결과**: **ON 효과가 유지된다** — 모드 변경 후에도 OFF 상태와 계속 달라야 한다
   (과거 "설정이 조용히 죽는" 증상은 앱이 세터 뒤에 덧붙이던 중복 `Reload()`가 원인이었고,
   제거 후 유지됨을 실측 — 컴포넌트 결함 아님)

## 통과 기준

- Desired 50x50에서 세 SamplingMode의 결과가 서로 달라야 한다
- Desired 0x0에서 네 SamplingMode의 결과가 모두 같아야 한다 (적용 조건의 대조군)
- DesiredSize 세 값의 렌더가 서로 다르고, 0x0 왕복이 비트 동일해야 한다
- LoadWithViewSize ON/OFF가 다르고, ON이 다른 설정 변경을 넘어 유지되어야 한다
- GetSamplingMode / GetDesiredWidth / GetDesiredHeight / IsImageLoadWithViewSizeEnabled
  반환값이 설정한 값과 일치해야 한다

("선명/블러"라는 화질 판단은 이 도구의 영역이 아니라 사람 눈의 몫 — 기계 기준은 위의
상이/동일 판정까지다.)
