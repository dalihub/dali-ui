# ScrollBarAdaptor와 사용자 스크롤바 연결

기록일: 2026-11-17. 코드 확인 기준: devel `e3e92062`.
상태: 설계 후속. 이번 패치에서는 구현하지 않는다.

## 배경과 현재 구조

ScrollViewImpl과 RecyclerViewImpl은 ScrollBar를 직접 생성하여 자식으로 붙이고,
콘텐츠 크기, viewport 크기, 위치와 표시 정책을 직접 전달한다. ScrollBarInterface를
전달받는 소비 API는 없다. 사용자 바를 주입하거나 기본 바를 반환하는 전용 API도 없다.

2026-11-18 패치에서는 PageScrollableInterface, ScrollBarInterface,
PageScrollView의 DestroyingSignal 관련 계약과 구현, ScrollBar의 GetTarget을 제거한다.
Adaptor 구현은 이 제거 작업의 선행 조건이 아니다. 제거 후에도 기본 스크롤바의
기존 크기·위치·표시 정책 갱신 기능은 유지한다.

## 제안 방향

1. non-virtual 공개 handle인 ScrollBarAdaptor를 추가한다. 상태는 impl에 둔다.
2. From(ScrollBar)는 기본 바에 위임하는 adaptor를 생성한다.
3. 사용자 바는 표시할 View와 콜백 설정을 받아 별도 Create 경로로 생성한다.
4. ScrollView에 SetScrollBar(ScrollBarAdaptor)를 추가한다. 기본 생성 경로도
   adaptor를 사용한다. GetScrollBar()와 adaptor의 GetView()로 기본 바를 얻어
   외형을 수정하는 경로도 검토한다.
5. RecyclerView에도 같은 내부 연결을 사용할 수 있다. RecyclerView의 공개
   설정·조회 API까지 추가할지는 별도 결정한다.

메서드명과 시그니처는 제안이며 확정 API가 아니다. 최초 범위는 현재와 같은 상태
표시다. 바 드래그로 콘텐츠를 움직이는 역방향 입력은 추가 요구가 생길 때 검토한다.

## 콜백과 ABI 방향

- 상태를 공개 ScrollBarState로 묶지 않고 크기·위치·표시 정책을 개별 전달하는
  방향을 선호한다. 각 기존 시그니처와 의미는 배포 후 유지해야 한다.
- 앱이 structSize를 채우는 공개 콜백 테이블 대신 Pimpl 기반 ScrollBarCallbacks
  설정 객체를 검토한다. 새 non-virtual 등록 API와 내부 저장 항목으로 확장한다.
- 생성 시 동작 연결을 확정한다. From으로 만든 adaptor에 사용자 콜백을 나중에
  덮어쓰는 setter는 두지 않는 방향이다.
- 설정 변경이 생성된 adaptor에 소급되지 않도록 한다. 다만 Dali::Callback은
  move-only이므로, 설정 객체의 복사·공유·이동 및 Create의 전달 방식은 미결이다.
  앞서 논의한 단순 함수 포인터 복사 모델을 그대로 적용해서는 안 된다.
- 공개 handle의 크기·상속·vtable과 기존 등록 API를 유지한다. 내부 저장 확장도
  외부에 노출된 impl ABI 경계를 확인한 후 수행한다.

## 수명 관련 미결 사항

Dali::Callback은 wrapper와 복사된 함수 객체를 소유하지만,
New(object, memberFunction)은 수신 인스턴스를 소유하거나 추적하지 않는다.
람다의 this/raw pointer 캡처도 수신 객체를 유지하지 않는다.
ConnectionTracker의 자동 해제는 Signal::Connect가 연결을 추적하기 때문에 가능하며,
Callback을 보관하는 것만으로 적용되지 않는다.

사용자 수신 객체 소멸 시 안전하게 호출을 중단할 수 있도록 tracker 기반 연결,
소유 handle 캡처 또는 weak 참조 중 적절한 계약을 결정한다. 함수 포인터와 void*
context만 보관하는 예시는 최종 수명 설계로 확정되지 않았다.

## 연결 규칙과 검증

- 기존 바 분리·새 바 부착, 초기 상태 전달, 레이아웃 방향과 배치 책임 결정.
- 빈 adaptor의 의미와 동일 adaptor/View의 중복 부착 처리 결정.
- 필수 콜백 누락 및 등록 실패의 처리 방식 결정.
- 콜백 중 바 교체·연결 해제·수신 객체 소멸 등 재진입 검증.
- 기본 바와 사용자 바 교체, 크기·위치·표시 정책 및 RTL 반영 검증.
- 수신 객체가 먼저 소멸한 경우와 scroller/adaptor가 먼저 소멸한 경우 검증.
- 구버전 앱이 새 라이브러리에서 기존 콜백만으로 동작하는지 검증.

제거 전 GetTarget은 기본 바 자신의 View를 반환하며 소비 호출처가 없었다.
이 메서드를 유지할 필요 없이 adaptor의 GetView 계약을 별도로 설계할 수 있다.
