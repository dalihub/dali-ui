# 91. View Layout: Margin

plain View 자식의 margin이 MATCH_PARENT, WRAP_CONTENT, 고정 크기에 적용되는지 확인한다.

## 화면 구성

- MATCH_PARENT 빨강, WRAP_CONTENT 초록과 내부 노랑, 고정 파랑
- 각 자식의 50px margin과 서로 다른 Y 좌표

## 테스트 절차

1. 각 색상 영역과 부모 가장자리의 간격을 확인한다.
2. **기대 결과**: 세 sizing mode 모두에서 50px margin이 해당 자식의 최종 bounds에 반영된다.
3. 초록과 노랑을 확인한다.
4. **기대 결과**: 초록은 내부 100x100 노랑을 기준으로 wrap되고 자신의 margin은 부모 배치에 적용된다.

## 통과 기준

- margin이 content 크기와 부모 내 배치에서 중복 계산되지 않는다.
