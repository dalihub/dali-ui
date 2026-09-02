# 컴포넌트 상태 효과를 위한 Offscreen Rendering 정책 분리

## 상태와 범위

- 상태: TODO / 설계 제안, 미구현
- 대상: disable 상태에서 opacity 효과를 적용하는 Slider 등의 복합 컴포넌트와
  `View`의 offscreen rendering 상태 관리
- 주요 대상 영역:
  - `dali-ui-foundation/public-api/views/view.h`
  - `dali-ui-foundation/public-api/views/view.cpp`
  - `dali-ui-foundation/internal/views/view/view-data-impl.h`
  - `dali-ui-foundation/internal/views/view/view-data-impl.cpp`
  - disable 상태 효과를 적용하는 component 및 state effect 구현

이 문서는 복합 컴포넌트의 disable opacity 효과가 내부 콘텐츠를 비쳐 보이게 만드는
문제와, 이를 offscreen rendering으로 방지할 때 application의 명시적 설정을 보존하기
위한 상태 분리 방안을 기록한다.

## 문제 배경

Slider 등의 컴포넌트는 disable 상태로 전환될 때 opacity 0.4 효과를 적용한다. 내부
구조가 복잡하거나 여러 반투명 레이어로 구성된 컴포넌트에 opacity를 직접 적용하면
각 레이어가 개별적으로 합성되어 컴포넌트 내부가 엑스레이처럼 비쳐 보일 수 있다.

이 문제를 피하려면 disable 상태에서는 컴포넌트를 하나의 offscreen 결과로 먼저
합성한 뒤 opacity를 적용해야 한다. 상태 전환은 다음 동작을 필요로 한다.

1. Disable 전환
   - 컴포넌트의 disable opacity 효과 적용
   - framework 내부 요청으로 offscreen rendering을 `REFRESH_ALWAYS`로 활성화
2. Enable 복귀
   - disable opacity 효과 제거 및 원래 opacity 상태 복원
   - framework 내부 offscreen rendering 요청 해제
   - application이 설정한 offscreen rendering 상태 복원

단일 offscreen rendering 상태만 사용하면 system이 disable 상태를 처리하면서
application이 `SetOffscreenRenderingEnabled()`와 `SetOffscreenRenderingRefreshRate()`로 설정한
값을 덮어쓸 수 있다. Enable 복귀 시 offscreen rendering을 무조건 끄는 방식도
application의 기존 설정을 잃게 만든다.

## 제안: user 설정과 system 설정 분리

`ViewDataImpl`의 offscreen rendering enabled 상태와 refresh rate를 application이 요청한
값과 framework가 내부적으로 요청한 값으로 분리한다.

```cpp
bool mUserOffscreenRenderingEnabled{false};
View::OffscreenRefreshRate mUserOffscreenRefreshRate{
  View::OffscreenRefreshRate::REFRESH_ALWAYS};

bool mSystemOffscreenRenderingEnabled{false};
View::OffscreenRefreshRate mSystemOffscreenRefreshRate{
  View::OffscreenRefreshRate::REFRESH_ALWAYS};
```

별도의 system 설정 여부 플래그는 두지 않는다. 현재 필요한 system 정책은 disable
상태에서 offscreen rendering을 `REFRESH_ALWAYS`로 활성화하는 것이므로
`mSystemOffscreenRenderingEnabled == false`를 system 요청이 없는 상태로 사용한다.

Effective offscreen rendering 설정은 다음 우선순위로 결정한다.

```cpp
const bool effectiveEnabled =
  mUserOffscreenRenderingEnabled || mSystemOffscreenRenderingEnabled;

const auto effectiveRefreshRate =
  mSystemOffscreenRenderingEnabled
    ? mSystemOffscreenRefreshRate
    : mUserOffscreenRefreshRate;
```

- `SetOffscreenRenderingEnabled()`은 user enabled 상태를 갱신
- `SetOffscreenRenderingRefreshRate()`는 user refresh rate를 갱신
- public property 설정은 기존 `0/1/2` 값을 user enabled 상태와 refresh rate로 변환
- disable 상태 진입은 system enabled 상태와 system refresh rate를 각각 `true`와
  `REFRESH_ALWAYS`로 갱신
- enable 상태 복귀는 system enabled 상태를 `false`로 갱신
- 각 경로에서 effective 설정이 실제로 변경될 때만 render effect와 render task 갱신
- system override 중 user 설정이 변경되면 user 값은 저장하되 effective 설정은 system
  값을 유지
- system override 해제 시 가장 최근에 저장된 user 값을 effective 설정으로 적용

이 구조에서는 application이 `REFRESH_ONCE` 또는 `REFRESH_ALWAYS`를 설정한 상태에서
disable과 enable 전환이 발생해도 원래 요청이 보존된다.

## 필요한 내부 API

Public enabled/refresh-rate setter는 user 요청만 다룬다. Component 또는 state effect가
public setter를 호출하지 않도록 system 전용 internal API를 제공한다.

예상 역할은 다음과 같다.

```cpp
void SetUserOffscreenRenderingEnabled(bool enabled);
void SetUserOffscreenRenderingRefreshRate(View::OffscreenRefreshRate refreshRate);
void SetSystemOffscreenRenderingEnabled(bool enabled);
void SetSystemOffscreenRenderingRefreshRate(View::OffscreenRefreshRate refreshRate);
bool IsEffectiveOffscreenRenderingEnabled() const;
View::OffscreenRefreshRate GetEffectiveOffscreenRenderingRefreshRate() const;
void UpdateEffectiveOffscreenRendering();
```

구체적인 함수명과 노출 위치는 구현 시 integration/internal 경계를 검토한다. System
설정은 application에 노출하지 않고 component 상태 효과 구현만 접근할 수 있어야 한다.

## Opacity 상태와의 결합

Offscreen rendering 전환과 opacity 변경은 같은 disable 상태 전환의 일부로 처리해야
한다. Enable 복귀 시 opacity를 무조건 1.0으로 설정하면 application 또는 다른 효과가
설정한 opacity를 잃을 수 있으므로 기존 state effect의 multiplier 또는 원래 값 복원
정책과 결합해야 한다.

다음 순서와 frame 경계가 시각적 깜빡임 없이 적용되는지 확인해야 한다.

1. Disable 진입 시 offscreen render task 준비
2. 합성된 결과에 disable opacity 적용
3. Enable 복귀 시 disable opacity 제거
4. System offscreen rendering 해제 및 user 설정 복원

Offscreen 결과가 준비되기 전에 opacity가 먼저 적용되거나, enable frame에서 render task가
먼저 제거되면 한 frame 동안 내부 레이어가 비쳐 보일 수 있다. 상태 변경 알림과 render
task 활성화 시점을 함께 설계해야 한다.

## 미결 사항

1. Public getter와 `OFFSCREEN_RENDERING` property getter가 user 요청값과 effective 값 중
   무엇을 반환할지 결정
2. 여러 system 기능이 동시에 offscreen rendering을 요청하게 될 경우 단일 system
   enabled/rate 상태로 충분한지 재검토
3. Disable 상태 중 application이 `REFRESH_ONCE`를 요청했을 때 system override 해제 후
   refresh 시점과 finished signal 의미 정의
4. View가 stage에 연결되거나 분리되는 동안 disable 상태가 변경될 때 render task
   lifecycle 검증
5. 기존 render effect, corner radius 및 visual depth index 처리와의 상호작용 확인
6. 모든 disable opacity 대상에 일괄 적용할지, 복합 컴포넌트만 opt-in할지 결정

## 검증 계획

### 상태 조합 테스트

1. User offscreen rendering disabled 상태에서 disable → enable
2. User enabled + `REFRESH_ALWAYS` 상태에서 disable → enable 후 user 설정 유지
3. User enabled + `REFRESH_ONCE` 상태에서 disable → enable 후 user 설정 유지
4. Disable 중 user 설정 변경 후 enable 시 최신 user 설정 적용
5. Disable과 enable 반복 전환 시 render task 누수 또는 중복 생성 없음
6. System 설정과 동일한 effective 설정을 다시 적용할 때 불필요한 재생성 없음

### 시각 및 컴포넌트 테스트

1. Slider 등 복합 컴포넌트의 disable opacity 0.4 표시
2. Disable 상태에서 내부 레이어가 비쳐 보이지 않음
3. Enable 복귀 시 opacity와 user offscreen rendering 설정 복원
4. Corner radius와 shadow가 있는 컴포넌트의 offscreen 합성 결과 확인
5. 상태 전환 frame에서 깜빡임 또는 한 frame의 잘못된 합성 없음

### 회귀 테스트

1. Public offscreen rendering enabled 및 refresh-rate getter/setter 동작
2. `OFFSCREEN_RENDERING` property 기반 설정 동작
3. `REFRESH_ONCE` finished signal과 output texture 동작
4. View 제거 및 파괴 시 offscreen render task 정리
