# Recycler API ABI 리팩토링 인계

Recycler 공개 API의 상태 은닉, 복사 의미, 소유권과 확장성을 개선하기 위한 후속 작업이다.
현재는 pre-release이므로 명확한 설계를 위해 공개 API를 변경할 수 있다는 사용자 전제를
적용한다. 아래 설계와 commit 순서는 대화에서 정리한 권장안이며, 이번 작업에서 구현을
시작한 것은 아니다.

기록일은 2026-10-16이며 코드 확인 기준 HEAD는 `8c3da689`이다. 다음 작업자는 당시
소스와 저장소 지침을 다시 확인하고, 아래 미결 사항을 확정한 뒤 구현한다.

## 대상과 사용자 기준

대상 헤더는 `dali-ui-foundation/public-api/views/recycler/`의 다음 네 파일이다.

- `group-body-decoration.h`
- `item-adapter.h` — 최초 요청의 `item-adaptor.h`에 해당하는 실제 파일명
- `item-decoration.h`
- `item-view-holder.h`

사용자 기준은 다음과 같다.

- dali-core, adaptor, ui는 선형으로 의존하는 하나의 라이브러리 묶음이다.
- public-api와 extension-api의 객체는 원칙적으로 구현 상태를 은닉한다.
- 공개 필드의 고정 자료형 예외는 Vector2처럼 표현할 정보가 본질적으로 완결되어
  영구히 필드를 늘릴 필요가 없는 소수의 기하·수학 타입에 한정한다.
- 단순해 보이는 struct에 동결 주석을 추가하는 것만으로 예외를 인정하지 않는다.
- 성능, 메모리, UI 스레드와 입력 이벤트 지연 위험을 함께 검토해야 한다.

관련 저장소 규칙은 [public-api-abi.md](../rules/public-api-abi.md)를 참고한다.
특히 integration-api는 public/extension과 달리 재컴파일을 허용하는 API 안정성
대상이다. integration 구현 클래스의 레이아웃 노출을 public ABI 위반과 동일하게
분류하지 말고, 외부 확장 계약과 내부 구현 경계를 확인한다.

## 사용할 두 가지 구현 은닉 패턴

1. **Underline형 값 PImpl**: 공개 객체가 Impl 포인터를 소유한다. 복사본은 독립적인
   메타데이터를 가지며, 이동은 소유권을 이전한다.
2. **Dali handle/object**: 공개 handle의 복사는 동일한 참조 계수 구현 객체를 가리킨다.
   공개 handle에는 데이터 멤버나 가상 함수를 추가하지 않는다.

기준 구현은 다음과 같다.

- [Underline 구현](../dali-ui-foundation/public-api/text/style/underline.cpp): 생성,
  복사 생성, 복사 대입마다 Impl을 할당하고 이동 시 포인터를 이전한다.
- [View 선언](../dali-ui-foundation/public-api/views/view.h): 복사하면 같은 객체를 참조한다.

독립된 값인지 공유 identity인지 먼저 선택한다. 비용 절감만을 이유로 값 객체를
handle로 바꾸면 기존 동작이 달라질 수 있다. 값 의미를 유지하면서 복사 대입 시 기존
Impl 저장공간을 재사용하는 최적화는 가능하다.

앞선 대화에서 제안했던 비소유 opaque callback context는 별도의 수명 모델을 도입하므로
최종 권장안에서는 제외했다. 성능 측정 때문에 다시 검토한다면 명시적인 설계 변경으로
다뤄야 한다.

## 타입별 최종 권장안과 위험

| 번호 | 대상 | 권장 방식 | 판단 근거 | 메모리와 성능 및 동작 위험 | 필요한 대응 |
|---|---|---|---|---|---|
| 1 | GroupBodyDecoration | Dali handle/object | 배경 View, 그룹 상태, Signal을 소유하는 동작 객체 | 최초 구현 할당, 참조 관리 비용. 공유 복사는 같은 상태를 변경 | 상태를 한 구현 객체에 집중. 단일 부착 또는 부착별 상태 정책 확정 |
| 2 | ItemDecoration | Dali handle/object | RecyclerView에 등록되는 identity와 수명을 가진 객체 | RecyclerView가 handle을 보유하면 지역 handle 소멸만으로 제거되지 않음 | 사용자 콜백 인터페이스 분리, 등록·제거·소유권 재설계 |
| 3 | ItemDecorationInvalidationSupport | 별도 타입 제거 후 ItemDecoration의 비가상 API로 통합 검토 | invalidation은 공통 구현 상태와 기능 | 별도 PImpl 할당 불필요. 기존 Signal 수명과 전달 순서 변경 위험 | 사용자 구현에서도 invalidation 요청 가능하게 설계 |
| 4 | ItemViewHolder | Underline형 값 PImpl | 기존 복사는 메타데이터를 독립 복사하고 View handle은 공유 | 가장 큰 성능 위험. 현재 복사 경로를 유지하면 재활용당 다중 할당, cache locality 저하 | move와 참조 사용, Impl 재활용, noexcept 이동, 복사 의미 검증 |
| 5 | ItemAdapter::ChangeInfo | Underline형 값 PImpl | 변경 시점의 독립된 이벤트 값 | 생성·복사 시 할당. 고빈도 알림에서 UI 스레드 비용 누적 | const 참조 Signal 전달 유지, 불필요한 복사 제거, batch 검토 |
| 6 | ItemAdapter | 기존 Dali handle/object 유지 | 이미 상태 없는 BaseHandle 기반 공개 객체 | 추가 PImpl 계층 불필요 | 내부 emit 함수와 구현 접근 경계 정리 |
| 7 | ItemOffsets | 기존 Insets 계열 값 타입 재사용 우선 | 네 방향 물리 길이에 정의를 제한하면 고정 자료형 예외 후보 | owning PImpl은 반복 offset 계산에서 할당할 수 있음 | 기존 타입의 순서·단위·방향·합산 의미 검토. 예외 여부 확정 |
| 8 | ChangeType와 GroupRowType | enum 유지 | 소유권과 무관 | 중간 삽입·재배치로 기존 숫자가 바뀔 수 있음 | 기존 숫자 유지, 기반 타입과 값 명시, 미지원 값 처리 정의 |

표의 1번과 2번은 같은 구현 패턴이다. 차이는 1번이 구체 decoration이고 2번이 공통
기반 handle이라는 역할이다. 3번은 세 번째 PImpl 패턴이 아니라 기능을 공통 handle에
배치한다는 의미다. InvalidateLayout 같은 함수명은 설명용이며 확정 API가 아니다.

## 현재 확인한 설계 문제

### GroupBodyDecoration

공개 클래스에 색상, margin, corner radius, unordered_map, Signal이 직접 들어 있다.
private 필드도 공개 객체 크기와 배치에 영향을 준다. GroupState 정의와 컨테이너도
구현으로 이동한다. 다만 컨테이너의 value 타입인 GroupState 필드 변경이 항상 바깥
클래스 sizeof를 바꾼다고 단정하지 않는다.

두 공개 인터페이스의 다중 상속은 PImpl 도입만으로 사라지지 않는다. 구체 handle과
공통 handle, 사용자 구현 인터페이스의 역할을 분리해야 한다.

현재 그룹 상태의 키는 groupIndex뿐이다. 같은 decoration을 다른 RecyclerView에도
등록하면 동일 그룹 번호가 배경 View와 상태를 공유할 수 있다. 한 구현 객체의 다중
부착을 금지하거나 attachment별 상태로 분리해야 한다.

### ItemDecoration과 선택 인터페이스

ItemDecoration은 사용자 상속 콜백, DestroyedSignal 상태, 주소 기반 등록 identity를
한 클래스에 결합한다. PImpl은 데이터 확장만 해결하며 공개 vtable의 추가·삭제·변경은
해결하지 않는다. 사용자 구현 인터페이스의 미래 확장은 별도로 설계해야 한다.

현재 ItemDecorationInvalidationSupport를 dynamic_cast로 탐색하는 방식은 기존 vtable
추가를 피하지만 별도 vtable 계약과 다중 상속을 늘린다. 공통 handle의 비가상 함수와
내부 Signal로 통합하는 방향을 검토한다. 기본 virtual callback의 inline 정의를
out-of-line으로 옮기더라도 virtual 함수 집합 확장 문제는 해결되지 않는다.

현재 소멸은 GroupBodyDecoration이 배경을 제거한 뒤 ItemDecoration이 파괴 Signal을
발생시키는 순서다. handle 전환 시 등록 소유권과 정리 순서를 함께 변경해야 한다.

두 클래스의 암시적 복사도 문제다. 확인한 dali-core의 SignalMixin은 소유 BaseSignal
포인터를 삭제하지만 복사를 금지하지 않아 decoration 복사 시 얕은 포인터 복사와
이중 해제 등의 위험이 있다. 그룹 map 복사 역시 같은 배경 View를 여러 객체가 정리할
수 있다. 구현 시 현재 core 소스를 재확인한다. handle 전환 전 기존 형태를 유지하는
기간에는 복사·이동 금지를 검토하되 별도 동작 변경으로 기록한다.

### ItemAdapter와 ChangeInfo

ItemAdapter handle 자체는 기존 패턴에 부합한다. ChangeInfo는 미래 payload, 식별자,
batch 정보 등의 확장 여지가 있는 이벤트이므로 고정 struct 예외로 보지 않는다.
const 참조로 전달해도 공개 필드 offset 의존성은 남는다.

GetItemCount, GetItemViewType, CreateViewHolder, BindViewHolder, RecycleViewHolder와
GetImpl 및 Impl 생성자는 실제 사용 목적과 노출 수준을 정리한다. 내부 주석과
@cond internal은 접근을 막지 않는다. 저장소의 DALI_INTERNAL 및 기존 구현 접근
관례를 확인해 적용한다. integration-api의 ItemAdapterImpl을 외부 확장 대상으로
유지할지 내부로 이동할지는 별도 확인한다.

### ItemViewHolder

position, viewType, view, rowType, groupIndex는 영구히 완결된 수학 타입이 아니라
확장 가능한 런타임 상태다. 공개 필드를 getter로 바꾸고 앱의 SetView와 내부 메타데이터
수정을 구분한다.

값 PImpl 복사는 메타데이터를 독립 복사하되 View handle은 같은 View를 참조한다.
View 자체를 복제하는 것이 아니다. GroupAdapter는 내부 adapter 호출 전후 position을
임시 변경·복원하므로 holder를 공유 handle로 바꾸면 이 의미가 달라진다.

## 성능상 주의할 실제 호출 경로

관련 코드는 `dali-ui-foundation/integration-api/`에 있다.

- `recycler-view-impl.cpp`: RecycleAll, RecycleRecord, CacheRecycledItem,
  ObtainItemView에서 holder의 값 복사와 vector 이동이 발생한다. PImpl 전환 전에
  소유권 이전을 move로 정리한다. 이동 후 지역 View 접근 등은 함께 수정한다.
- 같은 파일의 NotifyDecorationBoundsUpdated는 활성 holder와 decoration의 중첩
  반복이다. 내부 반복문까지 공개 getter만 쓰도록 강제할 필요는 없다.
- GetDecorationOffsets는 활성 holder를 선형 탐색한다. 기존 탐색 비용과 PImpl의
  추가 비용을 구분해서 측정한다.
- `item-adapter-impl.cpp`: Notify 계열은 임시 ChangeInfo를 동기 Signal로 전달한다.
  값 PImpl은 이벤트당 추가 할당을 만들 수 있다.
- `group-adapter-impl.cpp`: holder position과 그룹 정보를 호출 전후 변경·복원한다.

PImpl은 이벤트를 큐에 넣거나 지연시키지 않는다. 다만 메인 스레드의 추가 할당·해제,
깊은 복사와 메모리 접근 비용이 입력 처리와 프레임을 지연시킬 수 있다. 실제 기기
측정 없이 임계 지연이 없다고 보장하지 않는다.

## 위험도가 낮은 작업부터 진행할 commit 순서

각 commit은 관련 호출부·샘플·테스트를 함께 갱신하여 빌드 가능한 상태로 유지한다.
아래 항목은 모두 미착수다. git commit 생성 자체는 후속 작업의 사용자 요청과 저장소
관례에 맞춰 수행한다.

| 순서 | Commit 범위 | 위험도 | 주요 검증 |
|---|---|---|---|
| 1 | ChangeType와 GroupRowType 숫자·기반 타입 명시 | 낮음 | 기존 숫자·크기·switch 동작 유지 |
| 2 | ItemOffsets 표현 정리 또는 고정 기하 타입 유지 결정 기록 | 낮음 | 방향·단위와 여러 decoration offset 합산 |
| 3 | ChangeInfo 값 PImpl, 생성 API·getter·전체 사용처 갱신 | 낮음에서 중간 | 모든 알림, 독립 복사, 중첩 알림, 할당 비용 |
| 4 | holder는 기존 struct인 채 불필요한 복사를 move·참조로 변경 | 중간 | 생성·bind·recycle 순서, cache 재사용, position 복원 |
| 5 | ItemViewHolder 값 PImpl과 내부 쓰기 권한 분리 | 중간에서 높음 | 값 복사와 View 공유 의미, noexcept 이동, 스크롤 할당·프레임 |
| 6 | ItemAdapter 내부 emit·구현 접근 경계 정리 | 중간 | adapter·group adapter·RecyclerView 연동, 외부 사용 예제 |
| 7 | 사용자 decoration callback 계약과 내부 전달 경로 분리, 임시 bridge 유지 | 높음 | 사용자 구현·기본 콜백, 호출 순서, invalidation |
| 8 | ItemDecoration과 GroupBodyDecoration handle 전환, 등록 소유권과 정리·invalidation 통합 | 가장 높음 | 수명·명시적 제거·다중 부착, 순환 참조, 배경 정리, 실제 스크롤 |
| 9 | 임시 bridge와 구 인터페이스 제거, 문서·예제 정리 | 중간 | 구 API 잔존 확인, 관련 전체 테스트와 공개 헤더 빌드 |

4와 5는 분리하여 기존 동작 변경과 PImpl 비용을 각각 확인한다. 8의 소유권 변경은
등록·제거·배경 정리·파괴 Signal 대체를 한 번에 일관되게 적용한다. 7의 bridge가
과도한 임시 코드를 요구하면 7과 8을 하나의 전환 commit으로 합친다.

## 다음 작업에서 확정할 항목

- ItemOffsets와 기존 Insets 타입의 의미 호환성 및 고정 자료형 예외 승인 여부.
- 사용자 정의 decoration의 구현 인터페이스와 미래 hook 확장 방식.
- RecyclerView가 decoration을 강하게 보유하는 계약과 명시적 제거 방식.
- 같은 decoration의 다중 부착 금지 또는 attachment별 상태 분리.
- ItemAdapterImpl의 외부 integration 계약과 실제 사용자.
- 적용할 테스트·빌드 명령, 측정 기기와 허용 회귀 범위.

위 미결 사항은 최종 API 이름이나 구체 구현까지 확정된 것으로 간주하지 않는다.
사용자는 기록을 요청한 상태이며 이 문서 작성에서 리팩토링·commit은 실행하지 않았다.

## 검증과 단계 통과 기준

| 시점 또는 상황 | 기준 |
|---|---|
| 3번 후 | 변경 이벤트 의미 보존, 이벤트당 할당과 중첩 알림 확인 |
| 4번 후 | holder 처리 동작 보존, 불필요한 복사 감소 |
| 5번 후 | 재활용 중 추가 할당과 프레임 시간 회귀가 합의한 범위 이내 |
| 7번 전 | 사용자 콜백·소유권·다중 부착 정책 확정 |
| 8번 후 | 누수·해제 후 접근·참조 순환 검증, 실제 기기 입력·스크롤 지연 확인 |
| 빠른 스크롤과 fling | UI 스레드 시간, 프레임 시간 분포, 생성·재활용당 할당 횟수 |
| 안정된 재활용 | 불필요한 Impl 할당이 계속 반복되지 않는지 확인 |
| 대량 변경과 중첩 callback | 할당·처리 시간, 이벤트 데이터 수명과 독립성 |
| 그룹·decoration 증가 | 활성 아이템 수 곱하기 decoration 수의 반복 비용 |
| 장시간 실행 후 제거 | 메모리 추이, Signal 해제, 배경 View 정리 |

이번 인계는 정적 코드 검토와 설계 정리다. 성능 측정과 구현 검증은 후속 작업에
남아 있다. 특히 5번 성능 회귀가 해결되기 전에 decoration 전환을 겹쳐 진행하지 않는다.
