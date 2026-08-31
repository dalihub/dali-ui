# Disabled / PseudoDisabled Policy

disabled와 pseudo-disabled 정책이 계약대로 갈리는지 **탭과 상태 문자열로** 확인한다.

`tc-interactive-hover-state.md`의 테스트 5(시나리오 5.4)를 hover 없이 분리한 화면이다
(2026-08-26 사용자 결정 — 에뮬레이터에 포인터 장치가 없어 사람이 hover 절차를 수행할 수
없고, hover는 이 검증의 전제였을 뿐 대상이 아니었다). 기존 hover 화면은 5.3 복원을 위해
그대로 남아 있다.

컴포넌트 계약 (`interactive-trait.h`):

> pseudo-disabled: The view is **visually presented as disabled**, but it **still
> accepts user interaction** in order to provide guidance or an alternative flow.

**"보이기는 비활성인데 입력은 받는다"** — 같은 탭이 pseudo-disabled에서는 받아들여지고
disabled에서는 거부되는 대비가 이 화면의 전부다.

## 화면 구성

- `Policy target` 카드 (InteractiveView) + 상태 라벨 (`State: [...]` — 컴포넌트
  `GetState()`/trait getter를 읽음)
- 버튼: Reset / Toggle enabled / Toggle pseudo-disabled
- Event log (StateChangedSignal의 prev/current를 그대로 찍음; 진입 시 초기화)
- 상태별 카드 색: Normal `#D6E5F0` / PseudoDisabled `#D5DBDF` / Disabled `#E2E5E7`

## 테스트 절차

| 단계 | 확인 (실측 2026-08-26) |
|---|---|
| 1. 진입 | `State: [Normal]` |
| 2. [Toggle pseudo-disabled] 탭 | `State: [PseudoDisabled]` · 카드가 흐려짐 |
| 3. **그 상태에서 Policy target 탭** | **`State: [PseudoDisabled, Focused]`** — 입력을 받는다 |
| 4. [Reset] → [Toggle enabled] 탭 | `State: [Disabled]` · 카드가 흐려짐 |
| 5. **그 상태에서 Policy target 탭** | **`State: [Disabled]` 그대로** — 입력을 안 받는다 |
| 6. 카드 렌더 비교 | PseudoDisabled·Disabled 각각이 Normal과 다르다. **두 흐림끼리는
  (근사)동일하게 보일 수 있다** — disabled overlay의 0.4배 불투명도가 최종색을 겹치게
  만든다(실측 2026-08-26: 라벨 영역 동일). 두 상태의 구분은 색이 아니라 3·5단계의 탭
  대비가 맡는다 |

## 통과 기준

- pseudo-disabled: 흐려 보이지만 **탭 입력을 받는다** (상태에 Focused가 추가됨)
- disabled: 흐려 보이며 **탭 입력도 받지 않는다** (상태 불변)
- PseudoDisabled·Disabled 렌더가 각각 Normal과 달라야 한다 (두 흐림 상호 구분은 상태
  문자열·탭 대비의 몫 — 픽셀로는 겹칠 수 있다, 실측)
- Reset이 Normal로 되돌려야 한다
