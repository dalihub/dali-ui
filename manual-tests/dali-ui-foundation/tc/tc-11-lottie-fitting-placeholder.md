# 11. Lottie: PlaceholderUrl

LottieAnimationView의 PlaceholderUrl 설정/해제와, 본 콘텐츠가 READY가 아닌 동안의
placeholder 표시 동작을 확인한다.

placeholder는 **"로딩 중" 이미지**다 (`SetPlaceholderUrl` 문서: *"shown while loading"*).
URL을 지운 상태는 로딩이 아니라 **로딩할 대상이 없는 상태**라 placeholder가 표시되지 않는 것이
계약에 맞는다 (Glide/Coil도 `placeholder`(로딩 중)와 `fallback`(URL 없음)을 별개 이미지로
다룬다). "URL이 없을 때 보여줄 이미지"가 요구로 나오면 별도 `fallback` API를 논의할 사안이다.

로컬 파일은 로딩(성공이든 실패든)이 즉시 끝나 "로딩 중 표시"의 스쳐가는 프레임을 붙잡을 수
없다. 대신 이 화면은 **관측 가능한 안정 상태**로 같은 경로를 검증한다 (전부 실측):

- [Bad URL] 로드는 즉시 **FAILED**로 끝나고, 실패한 로드도 ResourceReadySignal을 발생시킨다.
  라벨이 `GetLoadingStatus()` 결과를 함께 찍어 성공/실패를 구분한다.
- FAILED 상태의 프리뷰는 **깨진 이미지(내장 broken 아이콘)**를 표시한다. 로드 전에 placeholder를
  설정해 두었더라도 로딩이 끝났으므로(FAILED) placeholder는 남지 않는다.
- 본 콘텐츠가 READY가 아닌 상태에서 [Set Placeholder]를 누르면 placeholder 이미지가 프리뷰에
  표시되고 유지된다 — placeholder 렌더 경로를 결정적으로 확인하는 지점이다.

## 화면 구성

- 중앙: Lottie 애니메이션 프리뷰 (240x160)
- 상태 라벨 (2줄): 1줄 = `PH: <GetPlaceholderUrl() 반환값 그대로>` (빈 값이면 `none`),
  2줄 = 마지막 이벤트 (신호 수신 시 `ResourceReady: READY|FAILED`)
- 버튼 행: Set Placeholder / Reload Lottie / Bad URL / Clear URL / Clear Holder

## 테스트 1: PlaceholderUrl 왕복

1. [Set Placeholder] 버튼을 탭한다
2. **기대 결과**: 라벨 1줄에 `placeholder_image.png`로 끝나는 실제 경로 표시 — getter 반환값이다
3. [Clear Holder] 버튼을 탭한다
4. **기대 결과**: 라벨 1줄이 `PH: none`

## 테스트 2: 로드 실패 상태

1. [Bad URL] 버튼을 탭한다 (존재하지 않는 경로 로드)
2. **기대 결과**: 라벨 2줄이 `ResourceReady: FAILED`
3. **기대 결과**: 프리뷰가 재생을 멈추고 깨진 이미지를 표시한다

## 테스트 3: READY가 아닌 동안 placeholder 표시

1. (테스트 2의 FAILED 상태에서) [Set Placeholder] 버튼을 탭한다
2. **기대 결과**: 프리뷰에 placeholder 이미지가 표시되고 유지된다

## 테스트 4: 정상 로드 복귀

1. [Reload Lottie] 버튼을 탭한다
2. **기대 결과**: placeholder가 사라지고 Lottie 애니메이션이 재생됨
3. **기대 결과**: 라벨 2줄이 `ResourceReady: READY`

## 통과 기준

- SetPlaceholderUrl 후 GetPlaceholderUrl이 동일한 값을 반환해야 한다 (라벨이 getter를 그대로 출력)
- 실패한 로드가 FAILED로 보고되어야 한다 (성공과 구분)
- 본 콘텐츠가 READY가 아닌 동안 설정된 placeholder가 표시되어야 한다
- 정상 로드 완료(READY) 시 placeholder가 사라지고 애니메이션이 표시되어야 한다
