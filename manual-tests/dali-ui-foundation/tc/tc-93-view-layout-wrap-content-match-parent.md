# 93. View Layout: Wrap Content Match Parent

WRAP_CONTENT 부모 아래 MATCH_PARENT follower의 측정·배치 순서를 확인한다.

## 화면 구성

- 고정 300x300 + margin 50인 빨강
- margin 100인 MATCH_PARENT 초록
- 초록 내부 margin 50인 MATCH_PARENT 파랑

## 테스트 절차

1. 겹친 색상 영역의 외곽과 내부 크기를 확인한다.
2. **기대 결과**: root는 빨강이 결정한 400x400이고 초록은 margin을 제외한
   200x200, 내부 파랑은 다시 margin을 제외한 100x100이다.

## 통과 기준

- MATCH_PARENT follower가 WRAP_CONTENT 부모 크기를 역으로 키우지 않는다.
