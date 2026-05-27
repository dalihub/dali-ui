# Ui Scale

dali-ui는 두 가지 독립적인 스케일 메커니즘을 제공하며, 이 둘이 조합되어 뷰의 최종 렌더링 크기가 결정됩니다.

| 메커니즘 | 적용 시점 | 설정 주체 |
|---|---|---|
| **정적 스케일** (`ScalingFactor`, `_spx`, `_sdp`) | 시작 시 `UiConfig::Apply()`로 고정, 런타임 변경 불가 | 앱 / 프레임워크 개발자 |
| **동적 스케일** (`UiScale`, `UiScalePolicy`) | 런타임, 레이아웃 패스마다 뷰 트리 전체에 전파 | 시스템 / OS 접근성 설정 |

이 문서는 **동적 스케일** 시스템(`UiScale`)을 다룹니다.

<br/>

## 동작 원리

트리 내 모든 뷰는 **유효 스케일(effective scale)** 을 보유합니다. 이 단일 `float` 배율은 Measure 및 Arrange 패스에서 모든 레이아웃 크기(사이즈, 마진, 패딩, 위치)에 곱해집니다.

뷰의 유효 스케일은 다음 규칙으로 결정됩니다.
1. 부모 체인을 올라가며 구체적인 스케일을 정의한 가장 가까운 조상(또는 루트)을 찾습니다.
2. 루트는 `UiScaleManager`에서 스케일을 상속합니다.

```
UiScaleManager (시스템 스케일)
  └── RootLayout             effectiveScale = 1.5  (UiScaleManager에서 상속)
        ├── StackLayout       effectiveScale = 1.5  (상속)
        │     ├── Label       effectiveScale = 1.5  (상속)
        │     └── Button      effectiveScale = 1.0  (DISABLED — 서브트리 스케일 해제)
        └── Badge             effectiveScale = 1.5  (ENABLED — 시스템 스케일 직접 추적)
```

`UiScaleManager::SetScale()`이 호출되면, 등록된 모든 레이아웃 루트가 유효 스케일을 재계산하고 자동으로 전체 재레이아웃을 트리거합니다.

<br/>

## UiScaleManager

`UiScaleManager`는 현재 시스템 스케일 값을 보유하는 프로세스 전역 싱글턴입니다.

```cpp
#include <dali-ui-foundation/public-api/ui-scale-manager.h>

// 현재 시스템 스케일 읽기
float scale = UiScaleManager::Get().GetScale();  // 기본값: 1.0

// 시스템 스케일 갱신 (보통 OS/vconf 핸들러에서 호출)
UiScaleManager::Get().SetScale(1.5f);
```

`SetScale()` 호출 시 다음 세 가지가 원자적으로 처리됩니다.
1. 저장된 스케일 값 업데이트.
2. 등록된 모든 레이아웃 루트와 그 전체 서브트리의 유효 스케일 캐시 리셋.
3. 각 루트에 `InvalidateMeasure()` 호출 → 다음 프레임에 전체 재레이아웃 예약.

> **참고:** `SetScale()`은 `NaN`, 0, 음수 값을 무시합니다. 이러한 값을 전달하면 no-op 처리되고 오류가 로그에 기록됩니다.

<br/>

## UiScalePolicy

각 `View`는 `SetUiScalePolicy()`를 통해 스케일 전파에 참여하는 방식을 직접 지정할 수 있습니다.

```cpp
#include <dali-ui-foundation/public-api/ui-scale-policy.h>

view.SetUiScalePolicy(UiScalePolicy::INHERIT);   // 기본값
view.SetUiScalePolicy(UiScalePolicy::ENABLED);
view.SetUiScalePolicy(UiScalePolicy::DISABLED);
```

| 정책 | 유효 스케일 | 설명 |
|---|---|---|
| `INHERIT` | 부모에서 상속 (루트는 `UiScaleManager`) | 기본값. 트리를 따라 스케일을 상속합니다. 대부분의 뷰는 이 정책을 사용합니다. |
| `ENABLED` | 항상 `UiScaleManager::GetScale()` 값 | 부모 정책에 관계없이 항상 시스템 스케일을 직접 추적합니다. |
| `DISABLED` | 항상 `1.0` | 스케일에서 완전히 제외됩니다. 하위 `INHERIT` 뷰도 `1.0`을 받습니다. |

<br/>

### INHERIT (기본값)

부모로부터 유효 스케일을 상속합니다. 모든 뷰의 기본 정책이 `INHERIT`이므로, `UiScaleManager`에 설정된 시스템 스케일은 별도 설정 없이 트리 전체로 자연스럽게 전파됩니다.

```cpp
// 별도 정책 설정 불필요 — INHERIT이 기본값입니다.
Label label = Label::New("Hello");
// label.SetUiScalePolicy(UiScalePolicy::INHERIT); // 묵시적
```

<br/>

### DISABLED — 서브트리를 1.0으로 고정

시스템 스케일과 무관하게 항상 자연 픽셀 크기(1:1)로 렌더링해야 하는 서브트리에 사용합니다.
주요 사용 사례: 고정 크기 HUD 요소, 크기가 변하지 않아야 하는 오버레이.

```cpp
// 이 패널과 모든 자식이 자연 픽셀 크기로 렌더링됩니다.
panel.SetUiScalePolicy(UiScalePolicy::DISABLED);
```

`DISABLED` 뷰의 자식들은 `1.0`을 상속하므로, `panel` 하위 서브트리만 비스케일 상태가 됩니다. 형제 뷰들은 정상적으로 스케일링됩니다.

```
UiScaleManager scale = 2.0
  └── RootLayout         effectiveScale = 2.0
        ├── Content       effectiveScale = 2.0  (INHERIT)
        └── Panel         effectiveScale = 1.0  (DISABLED)
              ├── Icon    effectiveScale = 1.0  (DISABLED 부모에서 INHERIT)
              └── Text    effectiveScale = 1.0  (DISABLED 부모에서 INHERIT)
```

<br/>

### ENABLED — DISABLED 서브트리에서 스케일 복구

`DISABLED` 서브트리 안에서도 특정 뷰만 시스템 스케일을 추적해야 할 때 사용합니다. `ENABLED`는 부모 상속을 무시하고 항상 `UiScaleManager`에서 직접 값을 읽습니다.

```cpp
panel.SetUiScalePolicy(UiScalePolicy::DISABLED);
badge.SetUiScalePolicy(UiScalePolicy::ENABLED);  // DISABLED 패널 안에서도 스케일 적용
```

```
UiScaleManager scale = 2.0
  └── Panel         effectiveScale = 1.0  (DISABLED)
        ├── Icon    effectiveScale = 1.0  (INHERIT)
        └── Badge   effectiveScale = 2.0  (ENABLED — DISABLED 부모 무시)
```

> **참고:** `ENABLED`는 앱 개발자에게는 거의 필요하지 않습니다. 트리 내 어느 위치에 놓이더라도 항상 스케일을 추적해야 하는 프레임워크 수준의 뷰에 주로 사용됩니다.

<br/>

## 정적 스케일과 동적 스케일의 조합

뷰의 최종 렌더링 크기는 두 스케일 메커니즘의 조합으로 결정됩니다.

조건:
- `ScalingFactor` = `1.5` (시작 시 `UiConfig::SetScalingFactor()`로 설정)
- `UiScaleManager` 스케일 = `2.0` (런타임에 설정)
- 요청 너비 `100_spx`인 뷰

최종 시각적 너비:

```
100_spx = 100 × ScalingFactor = 100 × 1.5 = 150 px   (natural units)
시각적 너비 = 150 × effectiveScale = 150 × 2.0 = 300 px
```

정적 스케일(`ScalingFactor`)은 디자인 단위를 시작 시점에 한 번 디바이스 픽셀로 변환합니다.
동적 스케일(`effectiveScale`)은 런타임에 레이아웃 트리 전체를 늘리거나 줄입니다.

<br/>

## 커스텀 뷰에서 스케일 변경에 대응하기

`ViewImpl`을 상속해 커스텀 뷰를 구현한 경우, 픽셀 단위 값을 내부에 캐시하고 있다면 스케일 변경 시 해당 캐시를 무효화해야 합니다. `SetScale()`이 호출되면 루트에서 `InvalidateMeasure()`가 자동으로 실행되므로, `OnMeasure` / `OnArrange`를 오버라이드한 뷰는 매 레이아웃 패스마다 새로운 constraint와 `effectiveScale`을 받습니다 — 별도 처리는 필요 없습니다.

레이아웃 패스 외부에서 값을 캐시하는 뷰는 프로퍼티 변경 알림을 구독하거나, `OnMeasure` 내에서 `GetEffectiveScale()`을 지연 호출 방식으로 사용하세요.

<br/>

## 요약

| 목적 | 방법 |
|---|---|
| 전체 시스템 스케일 변경 적용 | `UiScaleManager::Get().SetScale(newScale)` |
| 현재 시스템 스케일 읽기 | `UiScaleManager::Get().GetScale()` |
| 서브트리 스케일 해제 | `view.SetUiScalePolicy(UiScalePolicy::DISABLED)` |
| 특정 뷰를 항상 시스템 스케일에 추적 | `view.SetUiScalePolicy(UiScalePolicy::ENABLED)` |
| 기본 상속 동작으로 초기화 | `view.SetUiScalePolicy(UiScalePolicy::INHERIT)` |
| 뷰의 유효 스케일 조회 (프레임워크용) | `viewImpl.GetEffectiveScale()` |

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr)#development-guides)
