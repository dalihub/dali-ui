# 58. FlexLayout: Wrap and Grow

WRAP이 설정된 두 FlexLayout에서 basis와 grow 비율에 따른 줄별 공간 분배를 확인한다.

## 화면 구성

- 세 자식을 가진 위·아래 FlexLayout
- 위: basis 50/50/100, grow 1/1/2
- 아래: basis 100/100/200, grow 1/1/2

## 테스트 절차

1. 각 줄의 색상 폭 비율을 비교한다.
2. **기대 결과**: 남은 공간이 grow 1:1:2 비율로 분배된다.
3. 창 폭을 충분히 줄일 수 있으면 wrapping을 확인한다.
4. **기대 결과**: basis 합이 가용 폭을 넘는 줄의 항목이 다음 줄로 이동하고 각 줄에서 grow가 다시 계산된다.

## 통과 기준

- grow 비율이 줄 단위로 적용되고 wrap 이후 항목이 겹치지 않는다.
