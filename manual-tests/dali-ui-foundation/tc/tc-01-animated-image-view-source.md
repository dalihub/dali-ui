# 01. AnimatedImageView: SetResourceUrl / SetResourceUrls

SetResourceUrl / GetResourceUrl, SetResourceUrls / GetResourceUrls 동작을 확인한다. 단일 URL(WebP/GIF)과 URL 배열(PNG 시퀀스) 전환을 확인한다.

## 화면 구성

- 중앙: 애니메이션 프리뷰 (200x200)
- 상태 라벨: URL / URL count / Frames 정보 (URL 라벨은 GetResourceUrl 반환값을 그대로 출력, Frames 라벨은 GetTotalFrame()을 100ms 폴링으로 출력)
- 버튼 행 1: WebP / GIF / GIF2
- 버튼 행 2: URLs Dog(8) / URLs Logo(15)

## 테스트 1: 단일 URL 전환

1. [WebP] 버튼을 탭한다
2. **기대 결과**: dog-anim.webp 재생, GetResourceUrl이 설정한 경로 전체를 반환, Frames: 8
3. [GIF] 버튼을 탭한다
4. **기대 결과**: dali-logo-anim.gif로 전환되어 재생, Frames: 15
5. [GIF2] 버튼을 탭한다
6. **기대 결과**: animatedLoading.gif로 전환되어 재생, Frames: 3

## 테스트 2: URL 배열 전환

1. [URLs: Dog(8)] 버튼을 탭한다
2. **기대 결과**: GetResourceUrls count=8, 8프레임 PNG 시퀀스 재생, Frames: 8
3. [URLs: Logo(15)] 버튼을 탭한다
4. **기대 결과**: GetResourceUrls count=15, 15프레임 PNG 시퀀스 재생, Frames: 15

## 통과 기준

- 단일 URL: GetResourceUrl이 설정한 경로를 **전체 경로 그대로** 반환해야 한다 (라벨은 반환값을 자르지 않고 출력한다 — 파일명만 비교하면 디렉터리를 잃은 getter도 통과한다)
- URL 배열: GetResourceUrls 크기가 설정한 개수와 일치해야 한다
- Frames: GetTotalFrame()이 소스의 실제 프레임 수와 일치해야 한다 — 소스마다 값이 달라, 멈추지 않는 이 화면에서 "소스가 실제로 바뀌었다"를 판정하는 근거가 된다
