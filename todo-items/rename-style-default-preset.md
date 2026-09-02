# Style `DefaultPreset()` public API 이름 변경 검토

## 상태와 범위

- 상태: TODO / public 명칭 결정 및 migration 정책 필요
- 대상 API:
  - `TextButtonStyle::DefaultPreset()`
  - `CheckBoxStyle::DefaultPreset()`
  - `MarkdownViewStyle::DefaultPreset()`
  - `SwitchStyle::DefaultPreset()`
  - 향후 동일 pattern을 사용하는 component style
- 목표: 외부 style 설정을 반영한 `Default()`와 framework에 내장된 fallback style의
  차이를 public 이름만으로 명확하게 표현한다.

## 현재 의미

현재 style API에는 서로 다른 두 종류의 기본값이 있다.

- `Default()`: `UiConfig` 또는 component style sheet에 등록된 현재 기본 style을 반환하고,
  override가 없으면 built-in 값으로 fallback한다.
- `DefaultPreset()`: 외부 설정과 무관한 framework 내장 style을 반환한다.

`Preset`은 여러 선택 가능한 theme preset 중 하나처럼 읽힐 수 있지만, 실제 API는
framework가 보장하는 built-in fallback 한 개를 의미한다. 또한 `Default()`와
`DefaultPreset()`을 나란히 보면 어느 쪽이 application configuration을 반영하는지 즉시
드러나지 않는다.

## 이름 후보

### `BuiltIn()` — 권장 후보

```cpp
SwitchStyle::BuiltIn();
SwitchStyle::Default();
```

- framework 코드에 내장된 값이라는 의미가 짧고 분명하다.
- `Default()`가 runtime/config-resolved 값이라는 대비가 생긴다.
- 단점은 "built-in 중 어떤 것인가"라는 질문이 생길 수 있다는 점이지만 현재 style별
  built-in 값이 하나이므로 모호성이 작다.

### `BuiltInDefault()`

- 의미가 가장 명시적이다.
- `Default()`와 이름이 유사해 자동완성에서 관계를 찾기 쉽다.
- 이름이 길고 `Default()`와 시각적으로 혼동될 수 있다.

### `Fallback()` 또는 `FallbackStyle()`

- `Default()` lookup 실패 시 사용된다는 runtime 역할을 잘 표현한다.
- Application이 직접 호출하여 base style로 구성하는 사용법에는 지나치게 실패 경로처럼
  들린다.

### `Preset()`

- 기존 이름에서 변경 폭이 작다.
- 어떤 preset인지 드러나지 않고 향후 preset이 여러 개가 되면 다시 이름을 바꿔야 한다.

## 권장 방향

Public 이름은 `BuiltIn()`을 우선 검토한다. 문서에서는 다음 용어를 일관되게 사용한다.

- built-in style: library에 compile된 immutable base/fallback
- default style: 현재 `UiConfig`/style sheet를 반영하여 resolve된 style

이름만 먼저 확정하고 실제 변경은 모든 component style에 한 번에 적용한다. 일부 style만
변경하면 component 간 API pattern이 깨진다.

## Migration과 ABI 고려

이미 배포된 public API라면 즉시 symbol을 제거하지 않는다.

1. 새 이름을 추가하고 기존 `DefaultPreset()`은 새 함수로 forwarding한다.
2. Source deprecation annotation과 문서 migration 안내를 추가한다.
3. 지원하는 deprecation 기간 동안 기존 exported symbol을 유지한다.
4. Major ABI 전환 시점에만 기존 이름 제거를 검토한다.

아직 외부 배포 전인 API라면 alias 없이 한 번에 이름을 변경할 수 있다. 어느 경우든
generated public invoke wrapper, API reference, wiki, sample 및 automated test를 함께
갱신해야 한다.

## 결정할 사항

- 최종 public 이름: `BuiltIn()`, `BuiltInDefault()`, `Fallback()` 중 선택
- 기존 API의 배포 여부와 deprecation 기간
- `ComponentsStyleSheet` 내부 helper도 같은 용어로 변경할지
- 향후 light/dark, compact 등 여러 built-in variant를 public preset으로 제공할 계획이 있는지

## 검증 항목

1. 모든 component style이 동일한 static factory pattern을 제공하는지 확인
2. `Default()`가 configured style을 우선하고 built-in으로 fallback하는 기존 동작 유지
3. Built-in handle이 immutable base 값과 sharing 특성을 유지하는지 확인
4. Public symbol 및 ABI baseline 검사
5. Generated wrapper와 API reference에 새 이름이 노출되는지 확인
6. Wiki와 sample에서 `Default()`와 built-in API의 차이가 일관되게 설명되는지 확인
