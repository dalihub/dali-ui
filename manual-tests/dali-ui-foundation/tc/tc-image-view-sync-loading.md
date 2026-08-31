# ImageView: SyncLoading / FastTrack / OrientCorr / PreMult

SynchronousLoading, FastTrackUpload, OrientationCorrection, PreMultipliedAlpha 동작을 확인한다.

진입 라벨은 getter를 읽어 그린다 — **PreMult의 초기값은 ON**이다(컴포넌트 기본
`mPreMultipliedAlpha(true)`; 구 화면은 하드코딩 "OFF"를 박아 첫 탭이 값을 뒤집은 것처럼
보였다). 플래그 버튼들은 `Reload()` 없이 세터만 부른다 — 세터가 비주얼을 재생성하므로
충분하다(실측: OrientCorr 토글이 리로드 없이 44.6% 변화·복원 0px).

## 화면 구성

- 중앙: 이미지 프리뷰 (200x200) — 접근성 이름 `ImagePreview`
- 상태 라벨 1: Sync / FastTrack / OrientCorr / PreMult — 네 플래그 전부 살아 있는 `Is*()` 읽기
- 상태 라벨 2: ResourceReadySignal 카운트
- 버튼 행 1: Sync ON / Sync OFF
- 버튼 행 2: FastTrack ON / FastTrack OFF
- 버튼 행 3: OrientCorr ON / OrientCorr OFF
- 버튼 행 4: PreMult ON / PreMult OFF
- 버튼 행 5: Load IMG_A / Load EXIF / Load PNG / Reload

## 테스트 1: SynchronousLoading — Reload로 판정한다

(구 절차의 [Load IMG_A]는 이미 떠 있는 URL이라 무동작이었다 — 그대로 따라 하면 Sync와
무관한 이유로 "카운트 미증가"를 보고 PASS를 적게 된다.)

1. [Sync OFF] 상태에서 [Reload] 버튼을 탭한다
2. **기대 결과**: 카운터 **+1**
3. [Sync ON] 버튼을 탭한다 (세터 자체가 리로드를 유발해 +1 — 산수에 넣을 것)
4. [Reload] 버튼을 **두 번** 탭한다
5. **기대 결과**: 카운터 **불변** — 동기 로드의 Reload 경로는 신호를 쏘지 않는다

"이미지가 즉시 표시됨"이라는 지연 주장은 바깥에서 잴 수단이 없어 참고로 내린다.

## 테스트 2: OrientationCorrection

소재 `exif-rotated.jpg`(360x640, Orientation=6 = 시계 90도)에는 "TOP"(초록)·"BOTTOM"(빨강)
띠와 화살표가 그려져 있다.

1. [Load EXIF] 버튼을 탭한다 (OrientCorr 기본 ON)
2. **기대 결과**: 보정 적용 — **TOP 띠가 오른쪽**, 화살표가 오른쪽 (태그 지시 방향)
3. [OrientCorr OFF] 버튼을 탭한다
4. **기대 결과**: 보정 해제 — **TOP 띠가 위**, 가로세로 비율도 달라짐 (실측 44.6% 변화)
5. [OrientCorr ON] 버튼을 탭한다
6. **기대 결과**: 2단계 상태로 **비트 동일** 복원 (실측 0px)

## 테스트 3: PreMultipliedAlpha — 알파가 있어야 대상이 된다

두 JPEG은 알파가 없어 미리곱이 픽셀을 바꿀 수 없다. 알파 있는 `heartsframe-plain.png`
(반투명 5.0%)로 본다. **초기값이 ON**임을 기억할 것.

1. [Load PNG] 버튼을 탭한다
2. [PreMult OFF] 버튼을 탭하고 렌더를 비교한다
3. **기대 결과**: **불투명 영역은 동일**하고, 차이는 반투명 경계 픽셀에만 나타난다
   (실측 3.58%·최대 채널 편차 54·임계 64에서 0px — 확대 리샘플링 시 straight/premultiplied
   보간이 경계에서 갈리는 정상 프린지이며, 미리곱이 존재하는 이유가 바로 이 경계 처리다.
   육안으로는 두 렌더가 구분되지 않는다)
4. [PreMult ON] 버튼을 탭한다
5. **기대 결과**: 2단계 상태로 **비트 동일** 복원 (실측 0px)

## 테스트 4: FastTrackUpload — 플래그 왕복까지만

FastTrack은 성능 속성이라 화면·카운터 어디에도 관측점이 없다 — **이 화면·카운터로는 동작을
판정할 수 없다**를 명시해 둔다. 검증 대상은 `IsFastTrackUploadEnabled()` 왕복뿐이다.

1. [FastTrack ON] / [FastTrack OFF] 버튼을 탭한다
2. **기대 결과**: 라벨의 `FastTrack:` 필드가 각 값 그대로

> 참고 기록: API 문서는 FastTrack ON이면 Sync·Reload가 무시된다고 하나 실측은 Sync가
> 항상 이긴다(순서 불문 Reload +0). 부작용이라 회귀 기준으로 삼지 않으며 기록으로만 남긴다.

## 통과 기준

- Sync OFF의 Reload는 카운터 +1, Sync ON의 Reload는 카운터 불변이어야 한다
- OrientationCorrection: ON/OFF 렌더가 다르고(TOP 띠 방향), ON→OFF→ON이 비트 동일 복원
- PreMult: 알파 PNG에서 ON/OFF 차이가 반투명 경계에 한정되고(임계 64에서 0px), 왕복 복원
- 네 플래그의 Is* 반환값이 설정한 값과 일치해야 한다 (PreMult 초기값 ON 포함)

> **알려진 컴포넌트 결함 F22 (이슈 리포트 대상, 3.12 막힘 유지)**: Sync ON에서도
> `SetResourceUrl(다른 URL)`은 ResourceReadySignal을 쏜다(`image-visual.cpp`의 인라인
> emit에 SYNCHRONOUS_LOADING 억제가 없다). Reload 경로는 플래그를 지키고 재생성 경로는
> 어긴다 — 고치는 쪽이 어느 의미가 옳은지 함께 정해야 한다.
