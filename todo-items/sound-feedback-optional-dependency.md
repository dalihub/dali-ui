# SoundFeedback의 선택적 native multimedia 의존성 설계

## 상태와 범위

- 상태: TODO / 구조 논의 및 제품 요구사항 확인 필요
- 작업 타입: feedback backend 분리, build/package dependency 정책
- 관련 영역:
  - Switch를 포함한 interactive component의 입력 feedback
  - `dali-ui-foundation/internal/feedback/feedback-style.*`
  - `dali-adaptor`의 `FeedbackPlayer`
  - Tizen native multimedia sound API와 해당 shared object
- 목표: SoundFeedback이 필요한 제품에는 native sound를 제공하되, 필요하지 않은
  사업부·제품에는 multimedia library 의존성과 초기화 비용을 강제하지 않는다.

OneUIComponent/NUI 환경에서는 managed API가 sound backend 의존성을 감춘다. 동일 기능을
native dali-ui에서 직접 제공하려면 Tizen multimedia shared object를 link하거나 runtime에
load해야 한다. 모든 dali-ui 사용 제품이 sound feedback을 요구하는 것은 아니므로,
component 또는 foundation의 필수 link dependency로 즉시 추가해서는 안 된다.

## 현재 확인된 구조

기존 `FeedbackStyle`은 `Dali::FeedbackPlayer`를 통해 sound pattern과 sound file을
재생한다. 따라서 새로운 SoundFeedback 요구사항을 구현하기 전에 다음을 먼저 결정해야
한다.

1. 기존 adaptor `FeedbackPlayer` 계약을 확장하여 사용할 수 있는가?
2. 요구하는 system sound 기능이 adaptor 계약으로 표현되지 않아 dali-ui가 native
   multimedia API를 직접 호출해야 하는가?
3. SoundFeedback이 Switch 전용 기능인가, 모든 interactive component가 공유할
   foundation 기능인가?

기존 경로로 요구사항을 만족할 수 있다면 별도의 multimedia 직접 의존성을 추가하지
않는 것이 우선이다. 직접 호출이 불가피할 때만 아래 optional backend 구조를 적용한다.

## 권장 구조: backend interface와 선택적 provider

Component는 sound library를 직접 알지 않고 작은 내부 interface만 호출한다.

```text
Switch / Interactive component
        |
        v
SoundFeedback service
        |
        +-- Null backend (기본, 의존성 없음)
        |
        +-- Tizen multimedia backend (선택적 module/package)
```

### 공통 service

- 입력 종류와 feedback id만 받는다.
- Component lifetime마다 player를 만들지 않고 process/service 단위로 backend를 공유한다.
- Backend가 없거나 재생할 수 없으면 조용히 no-op 처리한다.
- UI thread를 blocking하지 않으며 backend 오류가 selection 동작을 막지 않는다.

### Null backend

- Foundation의 기본 구현으로 제공한다.
- Multimedia header, symbol 및 shared object를 참조하지 않는다.
- SoundFeedback을 사용하지 않는 제품의 build와 package에는 추가 의존성이 생기지 않는다.

### Tizen multimedia backend

- 별도 target 또는 plugin으로 분리한다.
- 이 target만 native multimedia library에 link한다.
- 제품 build option 또는 package composition으로 포함 여부를 결정한다.
- Backend 등록은 startup/config 단계에서 명시적으로 수행한다.

## 의존성 제공 방식 후보

### 1. 별도 link target과 package 분리 — 권장

예: foundation은 service/interface와 null backend만 포함하고,
`dali-ui-sound-feedback-tizen` 같은 선택적 target이 multimedia library를 link한다.

장점:

- 필요 없는 제품의 ELF `DT_NEEDED`와 RPM dependency에 multimedia가 나타나지 않는다.
- Backend 누락이 명확하고 빌드·패키징 검증이 쉽다.
- 다른 platform backend를 추가하기 쉽다.

단점:

- 제품별 package composition과 backend 등록이 필요하다.
- Optional module과 core의 version compatibility 정책이 필요하다.

### 2. Runtime dynamic loading

Foundation이 `dlopen`/`dlsym`으로 multimedia library와 symbol을 찾는다.

장점은 단일 binary에서 library 유무를 runtime에 허용할 수 있다는 점이다. 반면 symbol
version, signature, unload, error handling 및 보안 검토가 복잡하고 코드에 platform API
지식이 남는다. 배포 단위를 분리할 수 없는 명확한 요구가 있을 때만 검토한다.

### 3. Compile-time macro로 core에서 제외

구현은 단순하지만 동일 dali-ui package가 제품별로 서로 다른 기능과 ABI/동작을 갖게 될
수 있다. Binary variant 관리 비용이 크므로 장기 구조로 권장하지 않는다.

## 결정할 사항

- 기존 `FeedbackPlayer`로 필요한 system sound를 표현할 수 있는지
- Public API가 필요한지, Style/config 내부 설정만으로 충분한지
- Sound id, volume, policy 및 사용자 system setting을 어느 계층이 해석할지
- Backend 등록 실패와 multimedia service 비가용 상태의 진단 정책
- Optional backend target/package 이름과 최소 version 계약
- 접근성, silent mode, DND 및 제품 sound policy 적용 책임

## 검증 항목

1. Sound backend를 제외한 build에서 multimedia header와 library 없이 compile/link 성공
2. Core/foundation ELF에 multimedia `DT_NEEDED`가 없음을 확인
3. Sound package를 제외한 제품 package dependency에 multimedia가 없음을 확인
4. Backend 포함 시 tap/Enter에 한 번만 sound가 재생되는지 확인
5. Drag 중 매 frame 재생되지 않고 selection 확정 정책대로만 재생되는지 확인
6. Disabled, pseudo-disabled 및 취소된 입력에서 재생되지 않는지 확인
7. Backend 오류나 service 부재가 selection callback과 animation을 방해하지 않는지 확인
8. 여러 component가 backend/player를 공유하여 반복 allocation과 초기화를 만들지 않는지 확인
