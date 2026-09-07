# 79. LayoutTransition: Animator

animator callback이 progress에 따라 ENTER/EXIT/CHANGE 속성을 직접 기록하는지 확인한다.

## 화면 구성

- ENTER/EXIT/CHANGE 버튼
- spec TC와 같은 0.4초 외형 계약을 callback으로 구현한 자식 목록

## 테스트 절차

1. ENTER, CHANGE, EXIT를 차례로 누른다.
2. **기대 결과**: ENTER/EXIT은 height와 opacity를, CHANGE는 layout bounds를 callback 진행률에 맞춰 보간한다.
3. transition 도중 다른 버튼을 눌러 successor slot을 시작한다.
4. **기대 결과**: 취소된 slot의 잔여 상태 때문에 자식이 사라지거나 잘못된 opacity에 머물지 않는다.

## 통과 기준

- callback 방식이 spec 방식과 같은 최종 geometry와 visibility를 만든다.
