[English](https://github.sec.samsung.net/NUI/dali-ui/wiki/ProgressBar) · [Components](https://github.sec.samsung.net/NUI/dali-ui/wiki/Components-%28kr%29)

# ProgressBar

`Dali::Ui::ProgressBar`는 둥근 track과 trail(채워진 부분)로 가로 방향 진행률을 표시합니다. 완료량을 알 수 있으면 determinate 모드, 전체 작업량을 알 수 없는 대기 상태에는 indeterminate 모드를 사용합니다. 슬라이더가 아닌 비대화형 `ProgressView`이므로 애플리케이션이 진행 값을 갱신합니다.

## 설정 및 첫 번째 ProgressBar

애플리케이션 초기화 시 컴포넌트를 생성하거나 기본 스타일을 조회하기 전에 `Dali::Ui::Components::UiConfig`를 한 번 적용합니다. foundation 설정도 포함하므로 별도의 foundation `UiConfig`를 추가로 적용하지 않습니다. 기존 DALi UI 애플리케이션에서 components 라이브러리의 헤더와 링크 설정을 사용합니다.

다음 콘텐츠 생성 함수는 애플리케이션 초기화가 시작된 이후 호출하며, 반환된 View를 애플리케이션의 콘텐츠 컨테이너에 추가합니다. 이미 설정을 적용하는 애플리케이션에서는 `Apply()` 줄을 생략합니다.

```cpp
#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>

using namespace Dali;
using namespace Dali::Ui;

View CreateProgressContent()
{
  Components::UiConfig::New().Apply();

  StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
  root.SetRequestedWidth(MATCH_PARENT);
  root.SetRequestedHeight(WRAP_CONTENT);
  root.SetSpacing(12.0f);

  ProgressBar basic = ProgressBar::New();
  basic.SetValue(0.25f); // Default range is 0..1.
  basic.SetAccessibilityName("Download progress");
  root.Add(basic);

  ProgressBarStyle style = ProgressBarStyle::Default().Configure()
    .SetPadding(Insets(8.0f, 8.0f, 12.0f, 12.0f))
    .SetTrackThickness(14.0f)
    .SetTrackColor(UiColor(0xD5D9E0u))
    .SetTrailColor(UiColor(0x6750A4u))
    .SetDividerColor(UiColor(0xFFFFFFu, 0.8f))
    .Build();

  ProgressBar custom = ProgressBar::New(0.0f, 100.0f, style);
  custom.SetDividerStepCount(4); // Three visual dividers; no value snapping.
  custom.SetValue(50.0f);
  custom.SetAccessibilityName("Installation progress");
  root.Add(custom);
  return root;
}
```

`ProgressBar()`만 호출하면 빈 핸들이 생성됩니다. `New()`, `New(style)`, `New(minimum, maximum)`, `New(minimum, maximum, style)`을 사용합니다. 명시적으로 전달하는 스타일은 초기화된 핸들이어야 하며, 새 bar의 초기 값은 해당 범위의 최솟값입니다.

## 불변 스타일 및 기본값

`ProgressBarStyle::Default()`는 설정의 `DefaultKey()` 항목을 조회하며, 항목이 없으면 `DefaultPreset()`을 사용합니다. 둘 다 설정 적용 후 호출해야 합니다. `DefaultPreset()`은 캐시된 내장 스타일이고, `Builder()`는 재정의된 기본 스타일이 아닌 내장 값에서 시작합니다.

`Configure()`는 스타일을 복사해 변경 가능한 move-only builder를 만듭니다. 다섯 setter로 padding, 두께, track 색상, trail 색상, divider 색상을 설정합니다. `Build()`는 builder를 한 번 소비하고 불변 스타일을 반환합니다. 이름이 있는 builder에는 `<utility>`를 포함하고 `std::move(builder).Build()`를 사용합니다. 파생 스타일 생성이나 bar의 런타임 setter는 원본 스타일을 변경하지 않습니다.

| 설정 | 내장/기본값 |
| --- | --- |
| 범위 / 초기 값 | `0..1` / `0`; 명시적 범위에서는 해당 최솟값 |
| Determinate / reversed | `true` / `false` |
| Value / divider step count | `0` / `0` |
| 요청 너비 / 높이 | `MATCH_PARENT` / `WRAP_CONTENT` |
| 스타일 padding (`start, end, top, bottom`) | `Insets(0, 0, 10, 10)` |
| Track 두께 | 논리 단위 `10` |
| Track / trail / divider 색상 | `UiColor(0x1C1C1Eu, 0.1f)` / `UiColor::PRIMARY` / `UiColor(0xFFFFFFu, 0.6f)` |

내장 스타일의 자연 높이는 유효 배율 적용 전 논리 단위 30입니다. 자연 너비에는 가로 padding만 포함되므로 너비가 정해진 부모나 명시적인 요청 너비를 제공해야 합니다. Track은 padding 안쪽 콘텐츠의 세로 중앙에 배치되며, 사용 가능한 높이에 따라 두께가 제한됩니다.

애플리케이션 공통 기본값은 `config.Apply()` **이전**에 `config.StyleSheet().SetStyle(ProgressBarStyle::DefaultKey(), creator)`로 스타일 생성 함수를 등록합니다. [설정 API](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/components-ui-config.h)와 아래에 연결한 default-key 재정의 테스트를 참고하세요.

## 런타임 API 그룹

| 역할 | API |
| --- | --- |
| `ProgressView`에서 상속한 값과 범위 | `SetValue()` / `GetValue()`, `SetRange()` / `GetRange(minimum, maximum)` |
| 값 스냅 | `SetValueStepCount()` / `GetValueStepCount()` |
| 모드 | `SetDeterminate()` / `IsDeterminate()` |
| 외형 | `SetTrackThickness()` / `GetTrackThickness()`, `SetTrackColor()` / `GetTrackColor()`, `SetTrailColor()` / `GetTrailColor()` |
| 시각적 구간과 방향 | `SetDividerStepCount()` / `GetDividerStepCount()`, `SetReversed()` / `IsReversed()`, 상속한 `SetLayoutDirection()` |
| 오버레이 위치 정보 | `GetTrailEndPosition()`, `GetDividerCount()`, `GetDividerPosition(index)` |

스타일의 padding과 divider 색상은 생성 시 bar를 초기화합니다. ProgressBar 전용 런타임 divider 색상 setter는 없지만, 상속한 `View::SetPadding()`으로 레이아웃 padding을 설정할 수 있습니다.

값과 범위 양 끝은 유한해야 하며 `maximum > minimum`이어야 합니다. 범위 밖의 유한 값은 범위 안으로 제한됩니다. Value step count가 `0`이면 연속 값이고, 양수 `N`이면 같은 간격 `N`개와 유효 값 `N + 1`개를 만듭니다. 가장 가까운 단계로 스냅하며 정확한 중간값은 높은 단계로 반올림합니다. 범위나 step count를 바꾸면 현재 값도 다시 보정됩니다.

Divider step count는 별개입니다. `0` 또는 `1`이면 divider가 없고 `N > 1`이면 `N - 1`개입니다. 두 step count는 모두 0 이상이어야 합니다. 두께는 유한한 양수, 스타일 inset은 유한한 0 이상의 값이어야 합니다. 잘못된 입력이나 범위 밖 divider 인덱스는 오류 반환이 아닌 DALi assertion을 발생시킵니다.

## 모드와 방향

`bar.SetDeterminate(false)`로 indeterminate 모드에 진입하고 `bar.SetDeterminate(true)`로 현재 값의 fill을 복원합니다. Indeterminate 중에도 범위, 값, 스냅은 동작합니다. Determinate trail과 내장 divider는 숨겨지며, scene에 연결되고 track 너비와 높이가 0보다 클 때 둥근 track 안에서 네 개의 bar가 애니메이션됩니다. Scene에서 분리되면 애니메이션을 정리하고 재연결 시 필요에 따라 복원합니다.

Reversal은 determinate fill의 시작 방향을 바꾸며 divider 인덱스에는 영향을 주지 않습니다. 유효 RTL과 reversal의 조합은 다음과 같습니다.

| 레이아웃 방향 | `reversed = false` | `reversed = true` | Divider 인덱스 순서 |
| --- | --- | --- | --- |
| LTR | 왼쪽에서 오른쪽 | 오른쪽에서 왼쪽 | 왼쪽에서 오른쪽 |
| RTL | 오른쪽에서 왼쪽 | 왼쪽에서 오른쪽 | 오른쪽에서 왼쪽 |

`SetReversed()`를 indeterminate 애니메이션의 방향 제어 API로 사용하지 마세요.

## Signal, 툴팁 및 divider 라벨

`ValueChangedSignal()`은 유효 값과 렌더러 상태를 반영한 뒤 `(ProgressView, float)`를 전달합니다. `DeterminateChangedSignal()`은 모드를 반영한 뒤 `(ProgressView, bool)`을 전달합니다. 같은 유효 값이나 모드를 반복 설정하면 해당 signal은 발생하지 않습니다. 범위만 바뀔 때는 `ValueChangedSignal()`이 발생하지 않을 수 있습니다.

`TrailEndPositionChangedSignal()`과 `DividersChangedSignal()`은 `ProgressBar`만 전달합니다. 알림 전에 로컬 위치 스냅샷이 반영되며, 한 이벤트 턴의 변경은 합쳐집니다. 이 signal은 일반 상태가 아닌 위치 변경을 알립니다. 모드 변경, scene 연결, 월드 좌표 이동, 색상 변경, 로컬 결과가 같은 레이아웃은 그 자체로 위치 signal을 발생시키지 않습니다.

컴포넌트는 툴팁이나 divider 라벨을 생성하지 않습니다. 애플리케이션 소유 오버레이는 다음과 같이 동기화합니다.

1. 오버레이 수명 동안 유지되는 `ConnectionTracker`로 위치 callback을 연결한 뒤, 즉시 getter를 읽어 초기 상태를 동기화합니다.
2. `GetTrailEndPosition()`으로 툴팁 위치를 구합니다. 좌상단 원점의 ProgressBar-local 시각 좌표이며 RTL/reversal을 반영한 수학적 끝점입니다. 아주 작은 0이 아닌 값에서는 둥근 trail의 실제 외형이 끝점을 넘어갈 수 있습니다.
3. Divider signal마다 `GetDividerCount()`와 `index < count`인 모든 위치를 다시 읽어 라벨 집합을 재구성하거나 전체 동기화합니다.
4. 로컬 좌표를 오버레이 부모의 좌표로 변환합니다. 오버레이가 다른 곳에 있다면 부모/월드 이동에 대한 동기화도 애플리케이션이 처리합니다.
5. 값 변경으로 텍스트를 갱신하고 모드 변경으로 오버레이를 숨기거나 표시합니다. 위치 signal은 `DeterminateChangedSignal()`을 대신하지 않습니다.

스냅샷은 최초 arrange 전, 크기 0, scene 밖, indeterminate 상태에서도 유효합니다. Indeterminate의 끝점은 움직이는 애니메이션 bar가 아니라 저장된 값을 나타냅니다. [전체 오버레이 예제](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/manual-tests/dali-ui-components/tc/tc-progress-bar-basics.cpp)를 참고하세요.

## 접근성

상속한 역할은 `PROGRESS_BAR`이며 내부 track, trail, divider, 애니메이션 View는 접근성에서 숨겨집니다. `SetAccessibilityName()`으로 의미 있는 이름을 제공합니다. Determinate는 정규화 비율을 반올림한 백분율을 노출합니다(예: `0..100`에서 `24.6`은 `25%`). Indeterminate에는 기본 값 텍스트가 없습니다. 비어 있지 않은 `SetAccessibilityValue()`는 두 모드의 기본 동작을 재정의하므로 애플리케이션이 해당 텍스트를 유지하거나 지워야 합니다.

읽기 전용 표시기이므로 increment/decrement 동작은 지원하지 않습니다. 명시적 접근성 값이 없을 때 determinate 비율 변경과 모드 전환에 따라 기본 값 변경 알림이 발생합니다. 자동 음성 출력이나 busy 안내를 보장하는 것은 아니며, 접근성 클라이언트와 애플리케이션 정책에 따라 달라집니다. 애플리케이션이 생성한 오버레이 라벨의 접근성 정책도 별도로 정해야 합니다.

## 소스 및 검증 예제

- 공개 API: [ProgressBar](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/progress-bar.h), [ProgressBarStyle](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/styles/progress-bar-style.h), [ProgressView](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-foundation/public-api/views/progress-view.h).
- 구현: [bar 레이아웃·렌더링·위치](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/internal/progress-bar-impl.cpp), [스타일 기본값·검증](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/styles/progress-bar-style.cpp), [값 보정·접근성](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-foundation/extension-api/progress-view-impl.cpp).
- 자동 테스트: [ProgressBar UTC](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/automated-tests/src/dali-ui-components/utc-Dali-ProgressBar.cpp)는 스타일/기본값 재정의, 위치, 크기 0/scene 밖 동작, 모드 수명주기를 다룹니다. [ProgressView UTC](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/automated-tests/src/dali-ui-foundation/utc-Dali-ProgressView.cpp)는 값 제한, 스냅, signal, 접근성 값을 다룹니다.
- 수동 검증: [시나리오](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/manual-tests/dali-ui-components/tc/tc-progress-bar-basics.md)와 [예제 소스](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/manual-tests/dali-ui-components/tc/tc-progress-bar-basics.cpp)는 기본/사용자 스타일, 값, RTL/reversal, 오버레이 동기화, 반복 모드 전환을 확인합니다. 이는 검증 지침이며 대상 기기에서 실행되었다는 증거는 아닙니다.
