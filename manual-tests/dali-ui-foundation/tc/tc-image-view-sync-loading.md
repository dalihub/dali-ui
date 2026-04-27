# ImageView: SyncLoading / FastTrack / OrientCorr / PreMult

SynchronousLoading, FastTrackUpload, OrientationCorrection, PreMultipliedAlpha 동작을 확인한다.

## 화면 구성

- 중앙: 이미지 프리뷰 (200x200)
- 상태 라벨 1: Sync / FastTrack / OrientCorr / PreMult 플래그 상태
- 상태 라벨 2: ResourceReadySignal 카운트
- 버튼 행 1: Sync ON / Sync OFF
- 버튼 행 2: FastTrack ON / FastTrack OFF
- 버튼 행 3: OrientCorr ON / OrientCorr OFF
- 버튼 행 4: PreMult ON / PreMult OFF
- 버튼 행 5: Load IMG_A / Load EXIF / Reload

## 테스트 1: SynchronousLoading

1. [Sync ON] 버튼을 탭한다
2. [Load IMG_A] 버튼을 탭한다
3. **기대 결과**: 이미지가 즉시 표시됨 (ResourceReadySignal 카운트 미증가)
4. [Sync OFF] 버튼을 탭한다
5. [Reload] 버튼을 탭한다
6. **기대 결과**: ResourceReadySignal 카운트 증가

## 테스트 2: OrientationCorrection

1. [Load EXIF] 버튼을 탭한다
2. 현재 이미지 방향을 확인한다
3. [OrientCorr OFF] 버튼을 탭한다
4. [Reload] 버튼을 탭한다
5. **기대 결과**: EXIF 회전 정보가 적용되지 않아 이미지 방향이 다름
6. [OrientCorr ON] 버튼을 탭한다
7. [Reload] 버튼을 탭한다
8. **기대 결과**: EXIF 회전 정보가 올바르게 적용됨

## 테스트 3: PreMultipliedAlpha

1. [PreMult ON] / [PreMult OFF] 토글 후 [Reload]
2. **기대 결과**: 시각적 차이 확인

## 통과 기준

- SynchronousLoading ON: 이미지가 즉시 표시되고 ResourceReadySignal이 발생하지 않아야 한다
- OrientationCorrection ON: exif-rotated.jpg가 올바른 방향으로 표시되어야 한다
- 각 플래그의 Get/Is 반환값이 설정한 값과 일치해야 한다