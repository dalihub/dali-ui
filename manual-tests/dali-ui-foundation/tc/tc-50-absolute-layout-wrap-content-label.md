# 50. AbsoluteLayout: Wrap Content Label

비례 위치를 쓰는 WRAP_CONTENT Label이 WRAP_CONTENT AbsoluteLayout의 측정에 포함되는지 확인한다.

## 화면 구성

- MATCH_PARENT 초록 root
- 50px padding의 빨강 WRAP_CONTENT 부모와 파랑 `WRAP Label`
- 상단 중앙의 `Click to set size/wrap` 버튼

## 테스트 절차

1. 초기 화면에서 빨강 부모와 파랑 라벨을 확인한다.
2. **기대 결과**: 부모가 라벨과 padding을 감싸며 라벨은 부모 중앙에 배치된다.
3. 토글 버튼을 눌러 부모를 400x200으로 바꾼 뒤 다시 WRAP_CONTENT로 되돌린다.
4. **기대 결과**: 고정 크기에서도 라벨이 중앙에 있고 복귀 후 부모가 다시 내용 크기로 줄어든다.

## 통과 기준

- 고정/WRAP_CONTENT 전환을 반복해도 라벨 또는 부모 크기가 사라지거나 누적되지 않는다.
