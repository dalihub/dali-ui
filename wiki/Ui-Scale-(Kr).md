# Ui Scale

dali-ui는 세 가지 독립적인 스케일 메커니즘을 제공하며, 이들이 조합되어 뷰의 최종 렌더링 크기가 결정됩니다.

| 메커니즘 | 적용 시점 | 설정 주체 |
|---|---|---|
| **정적 스케일** (`ScalingFactor`, `_spx`, `_sdp`) | 시작 시 `UiConfig::Apply()`로 고정, 런타임 변경 불가 | 앱 / 프레임워크 개발자 |
| **동적 스케일** (`UiScale`, `UiScalePolicy`) | 런타임, 레이아웃 패스마다 뷰 트리 전체에 전파 | 시스템 / OS 접근성 설정 |
| **윈도우별 스케일** (`Extension::LayoutScaleManager::SetWindowScale()`) | 런타임, 한 윈도우 안의 모든 뷰에 적용 | 플랫폼 통합 개발자 / 모듈 제공자 |

이 문서는 **동적 스케일** 시스템(`UiScale`)과, 그 위에 독립적으로 곱해지는 **윈도우별 스케일**을 다룹니다.

런타임 스케일 API인 `Extension::LayoutScaleManager`(`dali-ui-foundation/extension-api/layout-scale-manager.h`), `Extension::View::SetUiScalePolicy()` / `GetUiScalePolicy()`, `Extension::View::GetEffectiveLayoutScale()`(`dali-ui-foundation/extension-api/view.h`)는 플랫폼 통합 개발자와 모듈 제공자를 위한 extension API입니다. 디스플레이 스케일을 적용하는 시스템·접근성 핸들러나 한 윈도우의 콘텐츠 스케일을 정하는 모듈이 사용합니다. 앱 개발자용 공개 API 목록에는 포함되지 않지만 공개 API와 함께 설치되며 그대로 사용할 수 있습니다. 이 문서의 `LayoutScaleManager`는 `Dali::Ui::Extension::LayoutScaleManager`입니다.

<br/>

## 동작 원리

트리 내 모든 뷰는 **유효 스케일(effective scale)** 을 보유합니다. 이 단일 `float` 배율은 Measure 및 Arrange 패스에서 모든 레이아웃 크기(사이즈, 마진, 패딩, 위치)에 곱해집니다.

**시스템 스케일**(UI 스케일)은 `LayoutScaleManager`가 보유하는 프로세스 전역 스케일이며, 보통 OS나 접근성 설정 핸들러가 `LayoutScaleManager::SetUiScale()`로 설정합니다. `LayoutScaleManager::GetUiScale()`은 이 저장값을 반환하고, 각 뷰의 유효 스케일은 `Extension::View::GetEffectiveLayoutScale()`로 조회합니다.

유효 스케일은 서로 독립적인 두 항의 곱입니다.

```
effectiveScale = windowScale × uiScale
```

- `uiScale`은 `UiScalePolicy`가 선택하는 항입니다. `LayoutScaleManager`가 보유한 시스템 스케일, 또는 스케일에서 제외된 뷰의 `1.0`입니다.
- `windowScale`은 그 뷰가 속한 `Window`에 설정된 스케일이며, 설정되지 않았으면 `1.0`입니다.
- 전역 마스터 스위치가 꺼져 있는 동안(`SetScalable(false)`)에는 두 항 모두 `1.0`입니다. 아래 *전역 마스터 스위치* 절을 참고하세요.

뷰의 `uiScale` 항은 다음 규칙으로 결정됩니다.
1. 부모 체인을 올라가며 구체적인 스케일을 정의한 가장 가까운 조상(또는 루트)을 찾습니다.
2. 루트는 `LayoutScaleManager`에서 스케일을 상속합니다.

윈도우 스케일은 **상속 체인마다 정확히 한 번**, 체인의 종단 뷰(`ENABLED` 뷰, `DISABLED` 뷰, 또는 상속하는 루트)에서 적용됩니다. 상속하는 뷰는 이미 곱해진 값을 그대로 받으므로, 윈도우 스케일이 트리를 내려가며 누적되지 않습니다.

```
LayoutScaleManager (시스템 스케일 1.5) + 윈도우 스케일 2.0
  └── RootLayout             effectiveScale = 3.0  (2.0 × 1.5, LayoutScaleManager에서 상속)
        ├── StackLayout       effectiveScale = 3.0  (상속)
        │     ├── Label       effectiveScale = 3.0  (상속)
        │     └── Button      effectiveScale = 2.0  (DISABLED — 시스템 스케일만 해제)
        └── Badge             effectiveScale = 3.0  (ENABLED — 시스템 스케일 직접 추적)
```

`LayoutScaleManager::SetUiScale()`이 호출되면, 등록된 모든 레이아웃 루트가 유효 스케일을 재계산하고 자동으로 전체 재레이아웃을 트리거합니다. 윈도우 스케일 변경도 같은 일을 하지만, 해당 윈도우의 루트에 대해서만 수행합니다. 씬에 없는 뷰(추가된 적이 없거나 제거된 뷰)는 등록된 루트에 속하지 않아 재레이아웃되지 않지만, 유효 스케일을 캐시하지도 않습니다. 그 뷰의 `Extension::View::GetEffectiveLayoutScale()`과 `View::Measure()`는 새 UI 스케일이나 스위치 상태를 바로 따르고, 윈도우에 추가되면 현재 스케일로 배치됩니다.

<br/>

## LayoutScaleManager

`LayoutScaleManager`는 현재 시스템 스케일 값과 각 윈도우의 스케일을 보유하는 프로세스 전역 싱글턴입니다. `LayoutManager`가 아니며 직접 배치하지 않습니다. 보유한 값은 레이아웃 패스가 적용합니다.

```cpp
#include <dali-ui-foundation/extension-api/layout-scale-manager.h>

using Dali::Ui::Extension::LayoutScaleManager;

// 현재 시스템 스케일 읽기
float scale = LayoutScaleManager::Get().GetUiScale();  // 기본값: 1.0

// 시스템 스케일 갱신 (보통 OS/vconf 핸들러에서 호출)
LayoutScaleManager::Get().SetUiScale(1.5f);
```

`SetUiScale()` 호출 시 다음 세 가지가 원자적으로 처리됩니다.
1. 저장된 스케일 값 업데이트.
2. 등록된 모든 레이아웃 루트와 그 전체 서브트리의 유효 스케일 캐시 리셋.
3. 각 루트에 `InvalidateMeasure()` 호출 → 다음 프레임에 전체 재레이아웃 예약.

> **참고:** `SetUiScale()`은 양수이고 유한하며 정상수(`>= FLT_MIN`)인 값만 받습니다. `NaN`, 무한대, 0, 음수, 비정상수(subnormal)는 무시되고 현재 값이 유지되며 오류가 로그에 기록됩니다.

<br/>

### 전역 마스터 스위치

`LayoutScaleManager`는 애플리케이션 전체의 동적 스케일을 켜고 끄는 프로세스 전역 **마스터 스위치**도 제공합니다.

```cpp
bool enabled = LayoutScaleManager::Get().IsScalable();  // 기본값: true

LayoutScaleManager::Get().SetScalable(false);  // 모든 뷰의 유효 스케일이 1.0으로 축소
LayoutScaleManager::Get().SetScalable(true);   // 저장된 스케일 재적용
```

스케일이 꺼져 있는 동안에는 모든 뷰의 유효 스케일이 `UiScalePolicy`(`ENABLED`, `INHERIT`, `DISABLED`)나 트리 내 위치와 무관하게 `1.0`으로 결정됩니다. `uiScale` 항과 윈도우 스케일이 모두 `1.0`이 됩니다. `SetUiScale()`로 설정한 값과 `SetWindowScale()`로 설정한 각 윈도우의 스케일은 꺼진 동안에도 보존되며, 스케일을 다시 켜는 즉시 재적용됩니다. 스위치를 전환하면 `SetUiScale()`과 동일하게 등록된 모든 레이아웃 루트가 무효화되고 전체 재레이아웃이 트리거됩니다.

> **참고:** 스케일이 꺼진 상태에서 `SetUiScale()` 또는 `SetWindowScale()`을 호출하면 새 값은 저장되지만 재레이아웃은 하지 않습니다. 지연된 값은 다음 `SetScalable(true)` 호출 시 반영됩니다.

> **참고:** `GetUiScale()`과 `GetWindowScale()`은 스케일이 꺼져 있는 동안에도 저장된 값을 반환합니다. 이 값은 `SetScalable(true)`가 재적용할 값이며, 지금 뷰가 배치되는 스케일이 아닙니다.

<br/>

### 윈도우별 스케일

각 `Window`는 그 안의 모든 뷰의 `uiScale` 항에 곱해지는 **윈도우 스케일**을 가질 수 있습니다.

```cpp
Window window = application.GetWindow();

float windowScale = LayoutScaleManager::Get().GetWindowScale(window);  // 기본값: 1.0

LayoutScaleManager::Get().SetWindowScale(window, 1.25f);  // 콘텐츠를 25% 크게
LayoutScaleManager::Get().SetWindowScale(window, 1.0f);   // 기본값으로 복귀
```

윈도우 스케일은 `UiScalePolicy`와는 **독립적**이지만, 마스터 스위치와는 독립적이지 않습니다.

- 정책이 `UiScalePolicy::DISABLED`인 뷰를 포함해, 그 윈도우의 모든 뷰에 적용됩니다.
- 시스템 스케일과 마찬가지로 `SetScalable(false)`로 꺼지고 `SetScalable(true)`로 재적용됩니다. 스케일이 꺼진 동안 설정한 값은 다시 켤 때 반영됩니다.
- 한 윈도우로 범위가 한정됩니다. 설정 시 그 윈도우에 현재 존재하는 레이아웃 루트만 무효화되며, 다른 윈도우는 영향을 받지 않습니다.

윈도우 자체의 픽셀 크기는 바뀌지 않습니다 — 같은 크기의 윈도우에 들어가는 논리적 콘텐츠의 양이 바뀝니다. `MATCH_PARENT` 루트는 여전히 윈도우를 정확히 채우고, 그 안의 콘텐츠는 유효 스케일로 배치되므로, 윈도우 스케일을 키우면 콘텐츠가 커지고 화면에 들어가는 양은 *줄어듭니다*.

`1.0`을 전달하는 것이 곧 리셋입니다. 기본값으로 되돌리고 그 윈도우의 레코드를 제거합니다. 별도의 리셋 메서드는 없습니다.

윈도우를 파괴하기 전에 리셋할 필요는 없습니다. 파괴된 윈도우의 레코드는 자동으로 제거되며, 이후 생성된 윈도우에 적용되는 일도 없습니다.

윈도우 스케일은 뷰가 그 윈도우 안에 있는 동안에만 적용됩니다. 윈도우에서 제거된 뷰는 하위 트리 전체와 함께 윈도우 스케일 항을 잃으며, 애플리케이션이 핸들을 가진 채 윈도우가 파괴된 뷰도 마찬가지입니다. 다시 윈도우에 추가될 때까지 `Extension::View::GetEffectiveLayoutScale()`과 `View::Measure()`는 현재 UI 스케일 항만 사용하고(분리된 동안의 `SetUiScale()`·`SetScalable()`도 바로 따릅니다), 윈도우에 추가하면 그 윈도우의 스케일이 다시 적용됩니다.

> **참고:** `SetWindowScale()`이 받는 값의 범위는 `SetUiScale()`과 같습니다. 양수이고 유한하며 정상수(`>= FLT_MIN`)여야 합니다. `NaN`, 무한대, 0, 음수, 비정상수(subnormal)와 빈 `Window`는 모두 무시되고 현재 윈도우 스케일이 유지되며 오류가 로그에 기록됩니다. 레이아웃이 유효 스케일로 나눗셈을 수행하기 때문에 이렇게 엄격합니다.

<br/>

### UiScaleManager (deprecated)

`UiScaleManager`(`ui-scale-manager.h`)는 이 클래스의 이전 이름입니다. 이전 릴리스로 빌드한 애플리케이션을 위해 남겨 둔 deprecated 클래스이며, 새 코드는 `LayoutScaleManager`를 사용합니다. 두 클래스는 하나의 상태를 공유하므로 어느 쪽으로 설정한 값이든 양쪽에서 읽히고, 마지막 호출이 적용됩니다. 두 핸들은 서로 다른 객체라서 `UiScaleManager` 핸들을 `LayoutScaleManager::DownCast()`하거나 그 반대로 하면 빈 핸들이 반환됩니다.

| UiScaleManager (deprecated) | LayoutScaleManager |
|---|---|
| `UiScaleManager::Get()` | `LayoutScaleManager::Get()` |
| `GetScale()` | `GetUiScale()` |
| `SetScale(float)` | `SetUiScale(float)` |
| `IsScalable()` | `IsScalable()` |
| `SetScalable(bool)` | `SetScalable(bool)` |
| `RegisterLayoutRoot(View)`, `UnregisterLayoutRoot(View)` | 없음. 라이브러리가 레이아웃 루트를 직접 등록합니다 |
| 없음 | `GetWindowScale(Window)`, `SetWindowScale(Window, float)` |

`ViewImpl::GetEffectiveScale()`도 함께 deprecated되었습니다. 같은 값을 반환하는 `Extension::View::GetEffectiveLayoutScale()`을 사용합니다.

<br/>

## UiScalePolicy

각 `View`는 `Extension::View::SetUiScalePolicy()`로 스케일 전파에 참여하는 방식을 지정하고, `Extension::View::GetUiScalePolicy()`로 읽습니다. `View`와 `ViewImpl`의 `SetUiScalePolicy()` / `GetUiScalePolicy()`는 deprecated되었으며 계속 동작합니다.

```cpp
#include <dali-ui-foundation/extension-api/view.h>

Extension::View::SetUiScalePolicy(view, UiScalePolicy::INHERIT);   // 기본값
Extension::View::SetUiScalePolicy(view, UiScalePolicy::ENABLED);
Extension::View::SetUiScalePolicy(view, UiScalePolicy::DISABLED);
```

| 정책 | `uiScale` 항 | 설명 |
|---|---|---|
| `INHERIT` | 부모에서 상속 (루트는 `LayoutScaleManager`) | 기본값. 트리를 따라 스케일을 상속합니다. 대부분의 뷰는 이 정책을 사용합니다. |
| `ENABLED` | 항상 `LayoutScaleManager::GetUiScale()` 값 | 부모 정책에 관계없이 항상 시스템 스케일을 직접 추적합니다. |
| `DISABLED` | 항상 `1.0` | 시스템 스케일에서 제외됩니다. 하위 `INHERIT` 뷰도 이 항으로 `1.0`을 받습니다. |

> **참고:** 위 열은 결과 유효 스케일이 아니라 `uiScale` **항**입니다. 정책은 윈도우 스케일을 선택하지 않으며, 윈도우 스케일은 정책이 고른 항에 곱해집니다. 따라서 윈도우 스케일이 `2.0`이면 `DISABLED` 뷰의 유효 스케일은 `1.0`이 아니라 `2.0`입니다.

<br/>

### INHERIT (기본값)

부모로부터 유효 스케일을 상속합니다. 모든 뷰의 기본 정책이 `INHERIT`이므로, `LayoutScaleManager`에 설정된 시스템 스케일은 별도 설정 없이 트리 전체로 자연스럽게 전파됩니다.

```cpp
// 별도 정책 설정 불필요 — INHERIT이 기본값입니다.
Label label = Label::New("Hello");
// Extension::View::SetUiScalePolicy(label, UiScalePolicy::INHERIT); // 묵시적
```

<br/>

### DISABLED — 서브트리를 시스템 스케일에서 제외

**시스템** 스케일을 따르지 않아야 하는 서브트리에 사용합니다.
주요 사용 사례: 고정 크기 HUD 요소, 접근성 설정으로 크기가 변하지 않아야 하는 오버레이.

```cpp
// 이 패널과 모든 자식이 시스템 스케일을 무시합니다.
Extension::View::SetUiScalePolicy(panel, UiScalePolicy::DISABLED);
```

`DISABLED` 뷰의 자식들은 그 `1.0` 항을 상속하므로, `panel` 하위 서브트리만 시스템 스케일 추적을 멈춥니다. 형제 뷰들은 정상적으로 스케일링됩니다.

> **참고:** 이것은 *모든* 스케일에서 제외되는 것이 아닙니다. 뷰가 속한 `Window`에 윈도우 스케일이 설정되어 있으면 그대로 적용되므로, 서브트리는 자연 픽셀 크기가 아니라 윈도우 스케일로 렌더링됩니다. 윈도우 스케일까지 제외하는 공개 정책은 없으며, 마스터 스위치만이 모든 뷰에 대해 이를 끕니다. (프레임워크 자신은 물리 픽셀로 관리하는 뷰 — 텍스트 커서와 핸들, 인라인 치환 레이어 — 만 윈도우 스케일에서 제외합니다.)

```
LayoutScaleManager 시스템 스케일 = 2.0, 윈도우 스케일 없음
  └── RootLayout         effectiveScale = 2.0
        ├── Content       effectiveScale = 2.0  (INHERIT)
        └── Panel         effectiveScale = 1.0  (DISABLED)
              ├── Icon    effectiveScale = 1.0  (DISABLED 부모에서 INHERIT)
              └── Text    effectiveScale = 1.0  (DISABLED 부모에서 INHERIT)
```

<br/>

### ENABLED — DISABLED 서브트리에서 스케일 복구

`DISABLED` 서브트리 안에서도 특정 뷰만 시스템 스케일을 추적해야 할 때 사용합니다. `ENABLED`는 부모 상속을 무시하고 항상 `LayoutScaleManager`에서 직접 값을 읽습니다.

```cpp
Extension::View::SetUiScalePolicy(panel, UiScalePolicy::DISABLED);
Extension::View::SetUiScalePolicy(badge, UiScalePolicy::ENABLED);  // DISABLED 패널 안에서도 스케일 적용
```

```
LayoutScaleManager 시스템 스케일 = 2.0, 윈도우 스케일 없음
  └── Panel         effectiveScale = 1.0  (DISABLED)
        ├── Icon    effectiveScale = 1.0  (INHERIT)
        └── Badge   effectiveScale = 2.0  (ENABLED — DISABLED 부모 무시)
```

윈도우 스케일이 `1.5`이면 같은 트리는 `1.5` / `1.5` / `3.0`이 됩니다. 윈도우 스케일은 각 종단 뷰의 항에 곱해지고, `Icon`은 `Panel`에서 이미 곱해진 값을 상속합니다.

> **참고:** `ENABLED`는 앱 개발자에게는 거의 필요하지 않습니다. 트리 내 어느 위치에 놓이더라도 항상 스케일을 추적해야 하는 프레임워크 수준의 뷰에 주로 사용됩니다.

<br/>

## 세 스케일 메커니즘의 조합

뷰의 최종 렌더링 크기는 세 스케일 메커니즘의 조합으로 결정됩니다.

조건:
- `ScalingFactor` = `1.5` (시작 시 `UiConfig::SetUnitScalingFactor()`로 설정)
- 시스템 스케일 = `2.0` (`LayoutScaleManager::SetUiScale()`로 런타임에 설정)
- 윈도우 스케일 = `1.25` (이 뷰가 속한 윈도우에 런타임으로 설정)
- 요청 너비 `100_spx`인 뷰

최종 시각적 너비:

```
100_spx = 100 × ScalingFactor = 100 × 1.5 = 150 px   (natural units)
effectiveScale = windowScale × uiScale = 1.25 × 2.0 = 2.5
시각적 너비 = 150 × effectiveScale = 150 × 2.5 = 375 px
```

정적 스케일(`ScalingFactor`)은 디자인 단위를 시작 시점에 한 번 디바이스 픽셀로 변환합니다.
동적 스케일(`effectiveScale`)은 런타임에 레이아웃 트리 전체를 늘리거나 줄이며, 윈도우 스케일은 그중 한 윈도우에만 적용되는 항입니다.

<br/>

## 커스텀 뷰에서 스케일 변경에 대응하기

`ViewImpl`을 상속해 커스텀 뷰를 구현한 경우, 픽셀 단위 값을 내부에 캐시하고 있다면 스케일 변경 시 해당 캐시를 무효화해야 합니다. `SetUiScale()`이 호출되면 루트에서 `InvalidateMeasure()`가 자동으로 실행되므로, `OnMeasure` / `OnArrange`를 오버라이드한 뷰는 매 레이아웃 패스마다 새로운 constraint와 `effectiveScale`을 받습니다 — 별도 처리는 필요 없습니다.

레이아웃 패스 외부에서 값을 캐시하는 뷰는 프로퍼티 변경 알림을 구독하거나, `OnMeasure` 내에서 `Ui::Extension::View::GetEffectiveLayoutScale(*this)`를 지연 호출하세요(`ViewImpl` 하위 클래스 안에서는 `Ui::` 한정자를 붙입니다. 한정자 없는 `Extension::`은 `CustomActorImpl::Extension`을 가리킵니다).

<br/>

## 요약

아래 표의 `LayoutScaleManager`는 `Dali::Ui::Extension::LayoutScaleManager`입니다.

| 목적 | 방법 |
|---|---|
| 전체 시스템 스케일 변경 적용 | `LayoutScaleManager::Get().SetUiScale(newScale)` |
| 현재 시스템 스케일 읽기 | `LayoutScaleManager::Get().GetUiScale()` |
| 전역 동적 스케일 on/off (윈도우 스케일 포함) | `LayoutScaleManager::Get().SetScalable(enable)` |
| 스케일 활성화 여부 조회 | `LayoutScaleManager::Get().IsScalable()` |
| 한 윈도우에 추가 스케일 적용 | `LayoutScaleManager::Get().SetWindowScale(window, scale)` |
| 윈도우의 스케일 조회 | `LayoutScaleManager::Get().GetWindowScale(window)` |
| 윈도우를 기본 스케일로 되돌리기 | `LayoutScaleManager::Get().SetWindowScale(window, 1.0f)` |
| 서브트리를 시스템 스케일에서 제외 | `Extension::View::SetUiScalePolicy(view, UiScalePolicy::DISABLED)` |
| 특정 뷰를 항상 시스템 스케일에 추적 | `Extension::View::SetUiScalePolicy(view, UiScalePolicy::ENABLED)` |
| 기본 상속 동작으로 초기화 | `Extension::View::SetUiScalePolicy(view, UiScalePolicy::INHERIT)` |
| 뷰의 유효 스케일 조회 (프레임워크용) | `Extension::View::GetEffectiveLayoutScale(viewImpl)` |

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr)#development-guides)
