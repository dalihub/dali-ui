# DALi Signal 단일 connection 저장 최적화 제안

## 상태와 범위

- 상태: TODO / 설계 및 성능 검증 필요, 미구현
- 주요 대상: `dali-core`의 `BaseSignal`과 `SignalConnectionPool`
- 관련 파일:
  - `dali/public-api/signals/base-signal.h`
  - `dali/public-api/signals/base-signal.cpp`
  - `dali/public-api/signals/signal-connection-pool.h`
  - `dali/public-api/signals/signal-connection-pool.cpp`
- 목표: callback이 정확히 하나인 signal의 동적 allocation과 예약 메모리 축소

이 제안은 facade 전용 API나 동작을 DALi signal에 추가하지 않는다. Callback 하나만 연결된
일반 DALi signal 전체에 적용할 수 있는 범용적인 small-count storage 최적화를 기록한다.
Facade가 application callback들을 하나의 native bridge callback으로 모으는 방식을 선택할
경우에도 같은 fast path를 활용할 수 있지만, 이는 제안의 필수 전제나 DALi 측 계약이
아니다.

## 현재 구조

`BaseSignal`은 callback 저장에 vector를 사용하지 않는다. `SignalConnectionPool`이
`SignalConnectionNode`를 lazy block pool에 저장하고 다음 정책을 사용한다.

1. 최초 block은 node 두 개의 capacity로 할당
2. 이후 block은 capacity를 두 배씩 증가
3. 작은 callback 집합은 block을 선형 탐색
4. 세 번째 block이 생성되는 시점부터 lookup hash map을 lazy 생성
5. Emit 순서는 node의 index 기반 doubly-linked list로 관리
6. Emit 중 제거된 node는 null 처리한 뒤 emit 종료 후 정리

따라서 callback이 없는 signal에는 connection block allocation이 없지만, 첫 callback을
연결하면 node 하나만 사용하더라도 두 node 용량의 block이 할당된다.

현재 64-bit layout을 기준으로 첫 block의 raw storage는 대략 다음과 같다.

```text
SignalConnectionBlock header  16 bytes
SignalConnectionNode × 2      48 bytes
--------------------------------------
First block                   64 bytes
```

별도로 callback을 표현하는 `CallbackBase` allocation도 필요하다. 위 수치는 allocator
metadata, alignment rounding과 callback payload를 포함하지 않는다.

## 제안: 0/1/N connection storage

`SignalConnectionPool`이 다음 세 상태를 갖도록 한다.

```text
0 connections
└─ allocation 없음

1 connection
└─ SignalConnectionPool의 기존 object storage에 node를 inline 보관

2+ connections
└─ block pool로 전환

large N
└─ 기존 lazy hash-map lookup 사용
```

첫 connection에서는 heap block을 만들지 않고 `SignalConnection`과 emit-list 정보에
필요한 최소 state를 pool object 내부에 placement construction한다. 두 번째 connection이
추가되면 첫 inline connection을 새 pool block으로 이동하고 기존 block pool 및 emit-list
경로로 전환한다.

`BaseSignal`과 `SignalConnectionPool`은 public API object layout에 포함되므로 class size를
늘리지 않는 것을 전제로 한다. 현재 pool이 보유한 heap-mode pointer와 count state를
inline node storage와 union 또는 tagged representation으로 공유할 수 있는지 먼저
검증한다. 기존 size와 alignment를 보존할 수 없다면 이 방안은 public ABI 변경 없이
적용할 수 없다.

초기 구현은 connection 수가 다시 하나로 줄었을 때 inline mode로 되돌리는 demotion을
하지 않는 방향을 권고한다. 한 번 pool mode로 전환한 signal은 파괴될 때까지 기존 pool을
재사용하면 node 이동, emit 중 제거와 tracker notification의 복잡도를 제한할 수 있다.

## 기대 효과

### Callback이 없는 경우

현재와 동일하게 connection storage allocation이 없다. `BaseSignal`과
`SignalConnectionPool`의 object size를 유지하면 고정 메모리도 증가하지 않는다.

### Callback이 하나인 경우

- 첫 connection block 약 64 bytes의 예약 메모리 제거 가능
- Block allocation 한 번과 allocator metadata 제거
- Heap fragmentation과 첫 connection 시 allocation latency 감소
- Emit iteration에서 block address lookup을 단순화할 수 있는 fast path 가능

`CallbackBase` 자체와 functor payload allocation은 이 제안의 범위가 아니므로 그대로
남는다. Callback callable까지 connection storage에 병합하려면 별도의 erased callback
storage 또는 small-object optimization 설계가 필요하다.

### Callback이 두 개 이상인 경우

Inline node를 기존 pool로 이동하는 단순 전환안을 사용하면 steady-state 예약 메모리는
현재 구조와 거의 같다. 이 제안의 주된 이점은 lifetime 동안 connection 수가 하나를
넘지 않는 signal에 집중된다.

Facade 단일 bridge 구조에서는 application callback 수와 관계없이 native signal
connection 수가 하나일 수 있으므로 이 fast path를 계속 사용할 수 있다. 다만 같은 DALi
version을 사용하는 순수 native application도 callback 하나에서는 동일한 최적화를 받는다.
따라서 callback 하나일 때 facade의 순수 native 대비 상대 overhead가 줄어드는 것은 아니며,
양쪽의 절대 메모리가 함께 감소한다.

## 장점

1. Facade와 무관하게 모든 DALi signal 사용자에게 적용되는 범용 최적화
2. Signal public API와 callback semantics 변경 없음
3. Class size를 유지할 수 있다면 public ABI 유지 가능
4. 단일 callback signal에서 allocation 수, 예약 메모리와 fragmentation 감소
5. 기존 pool, 작은 집합 선형 탐색 및 큰 집합 lazy hash map 정책 유지 가능
6. Facade, adapter 또는 module이 하나의 bridge callback을 사용하는 구조에도 자연스럽게 유효

## 단점과 위험

### Pool state 복잡도

`SignalConnectionPool`이 empty, inline과 heap mode를 구분해야 한다. Allocation, free,
destruction, block traversal, node-to-index 및 index-to-node가 모두 inline mode를 처리해야
하며 tagged state가 잘못 전환되면 double destruction이나 callback leak이 발생할 수 있다.

### Inline node 이동

두 번째 callback이 연결될 때 첫 node의 주소가 바뀔 수 있다. 현재
`ConnectionTrackerInterface`는 `CallbackBase*`와 `SlotObserver*`를 관찰하고 node pointer를
외부 계약으로 사용하지 않지만, 전체 코드와 테스트에서 node address 안정성을 암묵적으로
가정하는 경로가 없는지 확인해야 한다.

특히 callback 실행 중 같은 signal에 두 번째 callback을 추가하면 emit loop가 inline node를
참조하는 동안 pool mode로 전환될 수 있다. Emit 시작 시 tail snapshot, 다음 node index,
현재 callback 삭제와 signal 자체 파괴를 포함한 기존 규칙을 유지하도록 전환 시점과 local
state 사용을 검증해야 한다.

### ABI와 layout 제약

다음 값은 지원 architecture와 compiler configuration별로 유지되어야 한다.

- `sizeof(BaseSignal)`
- `alignof(BaseSignal)`
- `sizeof(SignalConnectionPool)`
- `alignof(SignalConnectionPool)`

Inline storage의 alignment가 `SignalConnectionNode`를 만족해야 한다. Public header의 private
member representation이 바뀌더라도 기존 binary가 생성한 object와 새 library가 같은 layout을
해석해야 하므로 size만 같다고 ABI가 자동으로 보장되는 것은 아니다. 배포와 ABI 정책에 맞는
compatibility 검토가 필요하다.

### 제한된 적용 효과

Connection 수가 빠르게 두 개 이상으로 증가하는 signal은 inline-to-pool transition 비용을
추가로 부담하고 steady-state에서는 기존과 거의 같은 메모리를 사용한다. 실제 signal별
connection-count 분포를 측정하지 않으면 전체 framework에서의 이득을 과대평가할 수 있다.

### 구현 및 테스트 유지비

기존 하나의 pool representation에 inline special case가 추가된다. Signal은 callback 중
connection 변경과 owner 파괴를 허용하므로 일반 container의 small-buffer optimization보다
검증 범위가 넓다.

## 대안

### 첫 block capacity를 1로 축소

최초 block을 header와 node 하나로 할당하면 구현 변경이 비교적 작다.

```text
SignalConnectionBlock header  16 bytes
SignalConnectionNode × 1      24 bytes
--------------------------------------
First block                   40 bytes
```

Inline storage보다 절감 폭이 작고 allocation은 남는다. 두 번째 callback에서 새 block을
추가하면 block 수가 더 빨리 증가하여 hash-map 생성 threshold와 다수 callback의 allocation
패턴이 바뀔 수 있다. Threshold를 active connection 수 기준으로 재설계해야 할 수 있다.

### Inline node를 유지하고 overflow만 pool에 저장

두 번째 callback 이후에도 첫 node를 inline에 유지하면 migration을 피할 수 있다. 그러나
emit list와 global node index가 서로 다른 storage를 동시에 표현해야 하고, block traversal과
lookup이 항상 두 저장소를 처리해야 한다. 단일 callback fast path보다 다수 callback의
일반 경로 복잡도가 커지므로 우선안으로 권고하지 않는다.

### Callback callable까지 inline 저장

`CallbackBase`와 작은 functor를 connection storage에 함께 넣으면 allocation을 더 줄일 수
있다. Callback type-erasure, destructor, alignment, tracker가 관찰하는 callback identity와
기존 `MakeCallback()` 계약까지 변경해야 하므로 이 문서의 후속 독립 과제로 분리한다.

## 검증 계획

### 기능 테스트

1. 0 → 1 → 0 connection 전환
2. 0 → 1 → 2 connection 전환과 등록 순서
3. Pool mode 진입 후 2 → 1 → 0 제거와 slot 재사용
4. Duplicate callback 연결과 제거
5. Tracker가 먼저 파괴되는 경우
6. Signal이 먼저 파괴되는 경우
7. Emit 중 현재 callback 제거
8. Emit 중 다음 callback 제거
9. Emit 중 두 번째 callback 추가와 다음 emit에서의 호출
10. Emit 중 signal owner 파괴
11. Callback이 null 처리된 뒤 deferred cleanup
12. Lazy hash-map 생성 전후 lookup과 erase

### ABI와 layout 검증

1. 지원 architecture별 기존 및 변경 후 size와 alignment 비교
2. ABI checker로 `BaseSignal`과 `SignalConnectionPool` layout 확인
3. 기존 public symbol과 function signature 변화 없음 확인
4. Inline storage가 요구 alignment를 만족하는지 compile-time assertion 추가

### 성능 및 메모리 측정

다음 connection 수에서 signal construction, first connect, emit, disconnect와 destruction을
측정한다.

```text
0, 1, 2, 3, 6, 7, 14 connections
```

측정 항목은 다음과 같다.

- Allocation 및 free 횟수
- Allocator usable size를 포함한 실제 heap 사용량
- Signal object 고정 크기
- Connect와 disconnect latency
- Emit latency
- Code size
- 실제 application과 framework의 signal별 connection-count 분포

## 완료 조건

1. Public ABI와 object layout 호환성 검증 통과
2. Callback 하나에서 connection block allocation 제거 또는 대안 대비 명확한 개선 확인
3. Callback 두 개 이상에서 기존 구현 대비 두드러진 성능 저하 없음
4. Emit 중 mutation과 lifetime 관련 기존 signal test 전체 통과
5. 새 0/1/N transition test 통과
6. 실제 사용처 측정에서 단일 callback signal 비율과 전체 메모리 개선 확인
7. 구현 복잡도 대비 이득이 충분하지 않으면 측정 결과와 폐기 이유를 문서에 기록
