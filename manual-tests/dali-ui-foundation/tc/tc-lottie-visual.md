# Lottie: SetImageColor / SetDesiredSize

SetImageColor / GetImageColor, SetDesiredWidth / GetDesiredWidth / SetDesiredHeight /
GetDesiredHeight 동작을 확인한다.

라벨 둘 다 진짜 getter다. `SetImageColor`는 비주얼을 다시 만들지 않고 MIX_COLOR를 제자리
갱신하므로 재생 중에도 즉시 적용되고, `SetDesiredWidth/Height`는 비주얼을 다시 만들어
바로 적용된다 — 리로드 단계가 필요 없다.

## 화면 구성

- 중앙: Lottie 애니메이션 프리뷰 (200x200)
- 상태 라벨: `Color: Rn Gn Bn An` / `DesiredSize: WxH`
- 버튼 행 1: White / Red / Blue / 50% Alpha (ImageColor)
- 버튼 행 2: Desired 0x0 / Desired 50x50 / Desired 200x200

## 테스트 1: ImageColor (값 왕복 + 렌더 도달)

1. [Red] 버튼을 탭한다
2. **기대 결과**: 라벨 `Color: R255 G0 B0 A255`, 프리뷰가 붉은 색조로 렌더된다
   (영역 평균 채널로 판정 가능 — 정지 없이도 R 채널이 G/B를 압도)
3. [Blue] 버튼을 탭한다
4. **기대 결과**: `Color: R0 G0 B255 A255`
5. [50% Alpha] 버튼을 탭한다
6. **기대 결과**: `Color: R255 G255 B255 A127` — `(int)(0.5f×255)`는 **버림**이라 127이
   맞다; 반올림하는 빌드는 128을 내고 여기서 걸린다

[White]("틴트 제거") 단계는 기본값과 같은 값을 확인하게 되어 아무것도 안 하는 빌드도
통과하므로 왕복 단계에서 뺀다 — 색 변화 자체는 위의 Red/Blue가 확인한다.

## 테스트 2: DesiredSize (값 왕복 + 렌더 도달)

1. [Desired 50x50] 버튼을 탭한다
2. **기대 결과**: 라벨 `DesiredSize: 50x50`, setter가 비주얼을 다시 만들어 정지 프레임에서 캡처 가능
3. [Desired 200x200] 버튼을 탭하고 캡처를 비교한다
4. **기대 결과**: 라벨 `DesiredSize: 200x200`, 두 캡처가 **다르다** — 실제로 다른 해상도로
   구웠다는 증거 ("흐리다"는 사람의 판단이라 기계 판정은 차이까지)

[Desired 0x0]("제한 없음") 역시 기본값 확인이라 왕복 단계에서 뺀다.

## 통과 기준

- SetImageColor 후 GetImageColor가 동일한 값을 반환해야 한다 (50% Alpha는 A127)
- Red 틴트가 렌더에 실제로 적용되어야 한다 (프리뷰 평균 채널에서 R 우세)
- DesiredSize 변경이 렌더에 실제로 반영되어야 한다 (50x50과 200x200 캡처가 다름)
